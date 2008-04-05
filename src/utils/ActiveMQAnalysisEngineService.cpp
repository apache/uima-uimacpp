/*

 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.

* Analysis Engine service wrapper implementation based on 
* Active MQ C++ client.
*/

#include "ActiveMQAnalysisEngineService.hpp"
#include "deployCppService.hpp"

#include <activemq/concurrent/Thread.h>
#include <activemq/concurrent/Runnable.h>
#include <activemq/concurrent/Concurrent.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConstants.h>
#include <activemq/util/Integer.h>

#include "uima/xmlwriter.hpp"
#include "uima/xcasdeserializer.hpp"
#include "uima/xmiwriter.hpp"
#include "uima/xmideserializer.hpp"
#include "uima/xmlerror_handler.hpp"

using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace activemq::concurrent;
using namespace uima;

enum traceLevels {NONE, INFO, FINE, FINER, FINEST };
traceLevels uimacpp_ee_tracelevel=INFO;
#define MSGHEADER   apr_time_now() << " ThreadId: " << Thread::getId() << __FILE__ << ":" << __LINE__ 
#define FORMATMSG(x)  stringstream lstr; lstr << MSGHEADER << " " << x;
#define LOGINFO(n,x) { if (n > uimacpp_ee_tracelevel) {} else {FORMATMSG(x); ResourceManager::getInstance().getLogger().logMessage(lstr.str().c_str());} }
#define LOGWARN(x) { FORMATMSG(x); ResourceManager::getInstance().getLogger().logWarning(lstr.str().c_str()); }
#define LOGERROR(x){ FORMATMSG(x); ResourceManager::getInstance().getLogger().logError(lstr.str().c_str(),-99); }

//===================================================
//AMQConnection
//---------------------------------------------------
  AMQConnection::AMQConnection( const char * aBrokerURL) : 
                                      iv_brokerURL(aBrokerURL), 
                                      iv_pConnection(0), 
                                      iv_pConsumerSession(0),
                                      iv_pConsumer(0),
                                      iv_inputQueueName(),
                                      iv_pInputQueue(0),
                                      iv_pListener(0),
                                      iv_pProducerSession(0),
                                      iv_pProducer(0),
                                      iv_pTextMessage(0),
                                      iv_replyDestinations(),
                                      iv_valid(false) {

    try {
      LOGINFO(INFO, "AMQConnection() connecting to " + iv_brokerURL);
      // Create a ConnectionFactory
      ActiveMQConnectionFactory* connectionFactory = 
        new ActiveMQConnectionFactory(iv_brokerURL);

      // Create a Connection
      if (connectionFactory == NULL) {	
        LOGERROR("AMQConnection() could not create connection factory");
        ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQConnection() could not create connection factory");
        ErrorInfo errInfo;
        errInfo.setMessage(msg);
        UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
          UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
          errInfo.getMessage(),
          errInfo.getMessage().getMessageID(),
          ErrorInfo::unrecoverable);
      }
      this->iv_pConnection = connectionFactory->createConnection();
      if (this->iv_pConnection == NULL) {
        LOGERROR("AMQConnection() could not create connection.");
        ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQConnection() could not create connection to " + iv_brokerURL);
        ErrorInfo errInfo;
        errInfo.setErrorId(UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE);
        errInfo.setMessage(msg);
        UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
          UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
          errInfo.getMessage(),
          errInfo.getMessage().getMessageID(),
          ErrorInfo::unrecoverable);
      } else {
        //default exception listener
        this->iv_pConnection->setExceptionListener(this);	
      }

      delete connectionFactory;
      // Create a Producer Session
      LOGINFO(FINEST,"AMQConnection() create Producer Session " + iv_brokerURL);
      this->iv_pProducerSession = this->iv_pConnection->createSession( Session::AUTO_ACKNOWLEDGE );

      if  (this->iv_pProducerSession == NULL) {
        LOGERROR("AMQConnection() createSession() failed."); 
        ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQConnection() createSession failed." );
        ErrorInfo errInfo;
        errInfo.setMessage(msg);
        UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
          UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
          errInfo.getMessage(),
          errInfo.getMessage().getMessageID(),
          ErrorInfo::unrecoverable);
      } 
      this->iv_pProducer = this->iv_pProducerSession->createProducer(NULL);
      if (this->iv_pProducer == NULL) {
        LOGERROR("AMQConnection() could not create MessageProducer ");
        ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQConnection() create MessageProducer failed.");
        ErrorInfo errInfo;
        errInfo.setMessage(msg);
        UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
          UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
          errInfo.getMessage(),
          errInfo.getMessage().getMessageID(),
          ErrorInfo::unrecoverable);
      }
      this->iv_pProducer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

      //create TextMessage
      this->iv_pTextMessage = this->iv_pProducerSession->createTextMessage();
      if (this->iv_pTextMessage == NULL) {
        LOGERROR("AMQConnection() create textMessage failed. ");
        ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQConnection() failed to create message.");
        ErrorInfo errInfo;
        errInfo.setMessage(msg);
        UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
          UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
          errInfo.getMessage(),
          errInfo.getMessage().getMessageID(),
          ErrorInfo::unrecoverable);
      }
      this->iv_valid = true;
      LOGINFO(0, "AMQConnection() connected successfully to " + iv_brokerURL);
    } catch (cms::CMSException& e) {
      LOGERROR("AMQConnection(): " + e.getMessage());  
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam(e.getMessage());
      ErrorInfo errInfo;
      errInfo.setErrorId(UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE);
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    } catch (...) {
      cout << "... " << endl;
      LOGERROR("AMQConnection() failed to create a connection");  
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam("AMQConnection() create connection failed");
      ErrorInfo errInfo;
      errInfo.setErrorId(UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE);
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }
  }

/* destructor */
  AMQConnection::~AMQConnection() {
    if (this->iv_pConsumer != NULL) {
      delete this->iv_pConsumer;
      this->iv_pConsumer = NULL;
    }
    if (this->iv_pProducer != NULL) {
      delete this->iv_pProducer;
      this->iv_pProducer = NULL;
    }
    if (this->iv_pConsumerSession != NULL) {
      delete this->iv_pConsumerSession;
      this->iv_pConsumerSession = NULL;
    }
    if (this->iv_pProducerSession != NULL) {
      delete this->iv_pProducerSession;
      this->iv_pProducerSession = NULL;
    }
    if (this->iv_pInputQueue != NULL) {
      delete this->iv_pInputQueue;
      this->iv_pInputQueue=NULL;
    }
    if (this->iv_pConnection != NULL) {
      this->iv_pConnection->close();
      delete this->iv_pConnection;
      this->iv_pConnection = NULL;
    }
    if (this->iv_pTextMessage != NULL) {
      delete this->iv_pTextMessage;
      this->iv_pTextMessage=NULL;
    } 
    
    //destinations
    map<string, cms::Destination*>::iterator ite; 
    for (ite= iv_replyDestinations.begin();ite !=  iv_replyDestinations.end();ite++) {
      delete ite->second;
    }
  }

/* create a MessageConsumer session and register a MessageListener
to receive messages from the input queue. */
  void AMQConnection::createMessageConsumer(string queueName,
                                        MessageListener * listener,
                                        int prefetch) {
    LOGINFO(FINEST, "AMQConnection::createMessageConsumer() consumer start " + queueName);
    this->iv_inputQueueName = queueName;
    stringstream str;
    //we add one to prefetch size to get the same behavior as the
    //Spring listener used in UIMA Java SDK which calls the
    //ActiveMQMessageConsumer.receive() api.
    //create endpoint destination
    str << queueName;
    str << "?consumer.prefetchSize=" << prefetch+1 << endl;	

    if (this->iv_pConsumerSession != NULL || iv_pConsumer != NULL) {
      LOGERROR("AMQConnection::createMessageConsumer() A session already exists. ");
      ErrorInfo errInfo;
      errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "A session already exists."));
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }

    this->iv_pConsumerSession = this->iv_pConnection->createSession( Session::AUTO_ACKNOWLEDGE );		
    if  (this->iv_pConsumerSession == NULL) {
      LOGERROR("AMQConnection() createSession failed."); 
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam("AMQConneciton() createSession failed." );
      ErrorInfo errInfo;
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    } 

    this->iv_pInputQueue = this->iv_pConsumerSession->createQueue(this->iv_inputQueueName);

    if (this->iv_pInputQueue == NULL) {
      LOGERROR("AMQConnection::createMessageConsumer() createQueue failed. " + queueName);
      stringstream str;
      str << "AMQConnection::createMessageConsumer() createQueue failed. " << queueName << endl;
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam(str.str());
      ErrorInfo errInfo;
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }

    this->iv_pConsumer = this->iv_pConsumerSession->createConsumer(this->iv_pInputQueue);
    if (this->iv_pConsumer == NULL) {
      LOGERROR("AMQConnection::createMessageConsumer() createConsumer failed. " + queueName);
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam("AMQConnection::createMessageConsumer() createConsumer() failed.");
      ErrorInfo errInfo;
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }
    //register listener
    this->iv_pConsumer->setMessageListener(listener); //caller owns listener 
    this->iv_pListener = listener;	 
    LOGINFO(FINEST, "AMQConnection::createMessageConsumer() " + queueName + " successful.");
  }

//caller owns the ExceptionListener
  void AMQConnection::setExceptionListener(ExceptionListener  * el) {
    this->iv_pConnection->setExceptionListener(el);
  }

  void AMQConnection::onException(const CMSException & ex) {
    //mark endpoint as broken.
    this->iv_valid = false;
    //log that connection is invalid.
    LOGWARN("AMQConnection()::onException() Connection to " + iv_brokerURL 
                       + " may be broken. " + ex.getMessage());
  }


  TextMessage * AMQConnection::getTextMessage() {
    if (this->iv_pTextMessage == NULL) {
      LOGERROR("AMQConnection::getTextMessage() failed. ");
      ErrorInfo errInfo;
      errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "TextMessage could not be created."));
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }
    iv_pTextMessage->clearProperties();
    iv_pTextMessage->clearBody();
    return this->iv_pTextMessage;
  }

  void AMQConnection::sendMessage(string queuename) {
    LOGINFO(FINEST, "AMQConnection::sendMessage() to " + queuename);
    if (this->iv_pProducer == NULL) {
      LOGERROR("AMQConnection::sendMessage() Invalid Message producer. ");
      ErrorInfo errInfo;
      errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "No message producer."));
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }
    //look up destination queue
    cms::Destination * pDest;
    map<string,Destination*>::iterator ite = this->iv_replyDestinations.find(queuename);
    if (ite != this->iv_replyDestinations.end()) {
      pDest = ite->second;
    } else {
      pDest = this->iv_pProducerSession->createQueue(queuename);
      if (pDest != NULL) {
        this->iv_replyDestinations[queuename] = pDest;
      }
    }

    if (pDest == NULL) {
      LOGERROR("AMQConnection::sendMessage() invalid destination " + queuename);
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam("AMQConnection::sendMessage() invalid destination.");
      ErrorInfo errInfo;
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }

    this->iv_pProducer->send(pDest,this->iv_pTextMessage);

    //cout << "producer->send elapsed time " << (apr_time_now() - stime) << endl;
    this->iv_pTextMessage->clearBody();
    this->iv_pTextMessage->clearProperties();
    LOGINFO(FINEST, "AMQConnection::sendMessage() successful to " + queuename);
  }

  void AMQConnection::sendMessage(const Destination * cmsReplyTo) {
    LOGINFO(FINEST, "AMQConnection::sendMessage() to " + cmsReplyTo->toProviderString());
    if (this->iv_pProducer == NULL) {
      LOGERROR("AMQConnection::getTextMessage Invalid message producer. ");
      ErrorInfo errInfo;
      errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "A MessageProducer does not exist."));
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }
    this->iv_pProducer->send(cmsReplyTo,this->iv_pTextMessage);

    //cout << "producer->send elapsed time " << (apr_time_now() - stime) << endl;
    this->iv_pTextMessage->clearBody();
    this->iv_pTextMessage->clearProperties();
    LOGINFO(4, "AMQConnection::sendMessage() successful to " + cmsReplyTo->toProviderString());
  }

// must be called to start receiving messages 
  void AMQConnection::start() {
    if (this->iv_pConnection != NULL) {
      this->iv_pConnection->start();
      LOGINFO(0,"AMQConnection::start() Start receiving messages.");
    } else {
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam("AMQConnection::start() failed. A connection does not exist.");
      ErrorInfo errInfo;
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }	
  }

//stops receiving messages
  void AMQConnection::stop() {
    LOGINFO(0,"AMQConnection::stop(). ");
    if (this->iv_pConnection != NULL) {
      this->iv_pConnection->stop();
    } else {
      LOGERROR("AMQConnection::stop() invalid connection. ");
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam("stop() invalid connection.");
      ErrorInfo errInfo;
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }
  }



//===================================================
//AMQConnectionCache
//---------------------------------------------------
  AMQConnectionsCache::AMQConnectionsCache() {

  }

  AMQConnectionsCache::~AMQConnectionsCache() {
    map<string,AMQConnection*>::iterator ite;
    for (ite = this->iv_connections.begin(); ite != this->iv_connections.end(); ite++) {
      if (ite->second != NULL) {
        delete ite->second;
      }	
    }	
  }

//Retrieves a Connection from the cache if it 
//exists or establishes a connection to the
//the specified broker and adds to the cache 
//and returns the new connection.
  AMQConnection * AMQConnectionsCache::getConnection(string brokerURL) {
    LOGINFO(FINE,"AMQConnectionCache::getConnection() looking up connection to " 
      + brokerURL );	 
    AMQConnection * connection = NULL;

    try {	    	
      map<string,AMQConnection*>::iterator ite;
      ite = this->iv_connections.find(brokerURL);
      if (ite == iv_connections.end()) {
        LOGINFO(FINE,"AMQConnectionsCache::getConnection() create new connection to " +
          brokerURL);
        connection = new AMQConnection(brokerURL.c_str());
        if (connection == NULL) {
          LOGERROR("AMQConnectionCache::getConnection Could not create a endpoint connection to " +
            brokerURL);
        } else {								    
          this->iv_connections[brokerURL] = connection;
        }
      } else {
        connection = ite->second;
        //if not a valid connection, reconnect
        if (connection == NULL) {
          LOGERROR("AMQConnectionCache::getConnection() could not connect to "
            + brokerURL);
        } else {
          if (!connection->isValid()) {
            LOGWARN("AMQConnectionCache::getEndPoint() Existing connection invalid. Reconnecting to " + brokerURL );
            delete connection;
            this->iv_connections.erase(brokerURL);
            connection = new AMQConnection(brokerURL.c_str());
            if (connection == NULL) {
              LOGERROR("AMQConnectionCache::getConnection() could not connect to "
                + brokerURL );
            } else {
              LOGWARN("AMQConnectionCache::getConnection() reconnected to " + 
                brokerURL);
              this->iv_connections[brokerURL] = connection; 
            }
          }
        }
      }
      return connection;		
    } catch (cms::CMSException & e) {
      LOGERROR("AMQConnectionCache::getConnection() " + e.getMessage());
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam("AMQConnectionCache::getConnection() " + e.getMessage());
      ErrorInfo errInfo;
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);

    } catch (...) {
      LOGERROR("AMQConnectionCache::getConnection() Unknown Exception. ");
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam("AMQConnectionCache::getConnection Unknown Exception");
      ErrorInfo errInfo;
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }
  }


//===================================================
//Listener that process UIMA requests. 
//---------------------------------------------------

  AMQListener::AMQListener(int id,
                           AMQConnection * connection,
                           AnalysisEngine * ae,
                           CAS * cas,
                           Monitor * stats) : 
                            iv_id(id),
                            iv_pConnection(connection),
                            iv_pEngine(ae),
                            iv_pCas(cas),
                            iv_pStatistics(stats),
                            iv_replyToConnections(),
                            iv_timeLastRequestCompleted(0),
                            iv_busy(false),
                            iv_count(0),
                            iv_aeDescriptor(),
                            iv_brokerURL(connection->getBrokerURL()),
                            iv_inputQueueName(connection->getInputQueueName()) {

    //get AE descriptor as XML to use when processing GETMETA requests.
    const AnalysisEngineMetaData & aeMetaData = iv_pEngine->getAnalysisEngineMetaData();			
    icu::UnicodeString xmlBuffer;
    xmlBuffer.insert(0, "<?xml version=\"1.0\"?>");
    iv_pEngine->getAnnotatorContext().getTaeSpecifier().toXMLBuffer(aeMetaData,
      false, xmlBuffer);
    UnicodeStringRef uref(xmlBuffer.getBuffer(), xmlBuffer.length());

    this->iv_aeDescriptor = uref.asUTF8();
  }

  AMQListener::~AMQListener() {

  }

/*
* Receive a TextMessage and examine the header properties
* to determine the type of request and payload.  Processes
* one request at a time and is blocked till the requestis 
* handled and the response sent.
*/
  void AMQListener::onMessage( const Message* message ) {


    apr_time_t startTime = apr_time_now();
    apr_time_t endTime;

    apr_time_t startSerialize;
    apr_time_t startDeserialize;
    apr_time_t startAnnotatorProcess;

    apr_time_t timeToDeserializeCAS = 0;
    apr_time_t timeToSerializeCAS = 0;
    apr_time_t timeInAnalytic = 0;
    apr_time_t timeIdle = apr_time_now() - iv_timeLastRequestCompleted;

    const TextMessage* textMessage=0;
    int command = 0;

    stringstream astr;
    astr << "*****Message#: " << ++iv_count << "*****" << endl;
    LOGINFO(INFO,astr.str());
    astr.seekp(0);

    try {	
      iv_busy=true;
      textMessage = dynamic_cast< const TextMessage* >( message );
      if (textMessage==0) {
        LOGERROR("AMQListener::onMessage() invalid pointer to TextMessage");
        endTime = apr_time_now();
        iv_pStatistics->processingComplete(0,false,endTime-startTime);
        this->iv_timeLastRequestCompleted = apr_time_now();
        return;
      }
      if (textMessage->propertyExists("MessageFrom")) {	
        LOGINFO(FINER,"Received from " + textMessage->getStringProperty("MessageFrom"));
      } else {	   
        LOGERROR("AMQListener::onMessage() ERROR MessageFrom not set.");
        endTime = apr_time_now();
        iv_pStatistics->processingComplete(0,false,endTime-startTime);
        this->iv_timeLastRequestCompleted = apr_time_now();
        return; 
      }       

      if (textMessage->propertyExists("Command") ){
        command = textMessage->getIntProperty("Command");
      } else {
        LOGERROR("AMQListener::onMessage() Required property Command not set.");
        endTime = apr_time_now();
        sendResponse(textMessage, timeToDeserializeCAS,
          timeToSerializeCAS, timeInAnalytic,
          timeIdle, endTime-startTime,
          "Required property 'Command' not set." ,true);
        endTime = apr_time_now();
        iv_pStatistics->processingComplete(0,false,endTime-startTime);
        this->iv_timeLastRequestCompleted = apr_time_now();
        return;
      }
      astr.seekp(0);
      astr << "Received request Command: " << command ;
      if (textMessage->propertyExists("CasReference")) {
        astr << " CasReference " << textMessage->getStringProperty("CasReference");
      }
      LOGINFO(INFO,astr.str());
      astr.seekp(0);
      if (command == PROCESS_CAS_COMMAND) { //process CAS
        //get the payload property
        if (!textMessage->propertyExists("Payload")) {
          LOGERROR("AMQListener::onMessage() Required property Payload not set.");
          endTime = apr_time_now();
          sendResponse(textMessage, timeToDeserializeCAS,
            timeToSerializeCAS, timeInAnalytic,
            timeIdle,endTime-startTime,
            "Required property 'Command' not set." ,true);

          endTime = apr_time_now();
          iv_pStatistics->processingComplete(command,false,endTime-startTime);
          this->iv_timeLastRequestCompleted = apr_time_now();
          return;
        }
        int payload = textMessage->getIntProperty("Payload");
        //get the text in the payload 
        string text = textMessage->getText();
        if (text.length() == 0) {
          LOGERROR("AMQListener::onMessage() There is no payload data. Nothing to process.");
          text = "There is no payload data. Nothing to process.";
          endTime = apr_time_now();
          sendResponse(textMessage, timeToDeserializeCAS,
            timeToSerializeCAS, timeInAnalytic,
            timeIdle, endTime-startTime,
            text ,true);
          endTime=apr_time_now();
          iv_pStatistics->processingComplete(command,false,endTime-startTime);
          this->iv_timeLastRequestCompleted = apr_time_now();
          return;
        }

        astr  << "Payload: " << payload;
        LOGINFO(FINER, astr.str());
        LOGINFO(FINER, "  Content: " +  text);
        astr.seekp(0);

        //InputSource
        MemBufInputSource memIS((XMLByte const *)text.c_str(),
          text.length(),
          "sysID");

        //reset the CAS
        iv_pCas->reset();
        stringstream xmlstr;  
        //deserialize payload data into the CAS,
        //call AE process method and serialize
        //the CAS which will be sent with the
        //response.
        if (payload == XCAS_PAYLOAD) {
          startDeserialize = apr_time_now();
          XCASDeserializer::deserialize(memIS, *iv_pCas);	 
          startAnnotatorProcess=apr_time_now();
          timeToDeserializeCAS = startAnnotatorProcess-startDeserialize;

          iv_pEngine->process(*iv_pCas);
          startSerialize=apr_time_now();
          timeInAnalytic = startSerialize - startAnnotatorProcess;

          XCASWriter xcaswriter(*iv_pCas, true);
          xcaswriter.write(xmlstr);
          timeToSerializeCAS = apr_time_now() - startSerialize;
        } else if (payload == XMI_PAYLOAD) {
          //deserialize incoming xmi CAS data.
          startDeserialize = apr_time_now();
          XmiSerializationSharedData sharedData;
          XmiDeserializer::deserialize(memIS, *iv_pCas, sharedData);      
          startAnnotatorProcess=apr_time_now();
          timeToDeserializeCAS = startAnnotatorProcess-startDeserialize;

          iv_pEngine->process(*iv_pCas);
          startSerialize=apr_time_now();
          timeInAnalytic = startSerialize - startAnnotatorProcess;

          //serialize CAS 
          XmiWriter xmiwriter(*iv_pCas, true, &sharedData);
          xmiwriter.write(xmlstr);
          timeToSerializeCAS = apr_time_now() - startSerialize;
        } else {
          xmlstr << "Invalid Payload " << payload;
          LOGERROR("AMQListener::onMessage() " + xmlstr.str());
          endTime=apr_time_now();
          sendResponse(textMessage, timeToDeserializeCAS,
            timeToSerializeCAS, timeInAnalytic,
            timeIdle,endTime-startTime,xmlstr.str() ,true);
          endTime=apr_time_now();
          iv_pStatistics->processingComplete(command,false,endTime-startTime);
          this->iv_timeLastRequestCompleted = apr_time_now();
          return;
        }
        //done with this CAS.
        iv_pCas->reset(); 
        //record end time
        endTime = apr_time_now();
        //send reply
        LOGINFO(FINER,"AnalysisEngine::process() completed successfully. Sending reply.");   
        sendResponse(textMessage, timeToDeserializeCAS,
                      timeToSerializeCAS, timeInAnalytic,
                      timeIdle, endTime-startTime,
                      xmlstr.str(),false);
        endTime=apr_time_now();
        iv_pStatistics->processingComplete(command,true,endTime-startTime,
          timeToDeserializeCAS, timeInAnalytic, timeToSerializeCAS);
        LOGINFO(FINE,"Process CAS finished.");		
      } else if (command ==  GET_META_COMMAND ) { //get Meta 	 
        LOGINFO(FINE, "Process getMeta request start.");
        endTime = apr_time_now();
        sendResponse(textMessage, timeToDeserializeCAS,
                  timeToSerializeCAS, timeInAnalytic,
                  timeIdle, endTime-startTime,this->iv_aeDescriptor,false);			
        endTime=apr_time_now();
        iv_pStatistics->processingComplete(command,true,endTime-startTime);
        LOGINFO(FINE,"Process getMeta request finished.");			
      } else if (command ==  CPC_COMMAND ) { //CPC       			
        LOGINFO(FINE, "Processing CollectionProcessComplete request start");				
        iv_pEngine->collectionProcessComplete();
        endTime = apr_time_now();
        sendResponse(textMessage, timeToDeserializeCAS,
                    timeToSerializeCAS, timeInAnalytic,
                    timeIdle,endTime-startTime,"CPC completed.",false);
        endTime=apr_time_now();
        iv_pStatistics->processingComplete(command,true,endTime-startTime);
        LOGINFO(FINE, "Processing CollectionProcessComplete request finished.");	
      } else {
        endTime = apr_time_now();
        stringstream str;
        str << " Invalid Request " << command << endl;
        LOGERROR(str.str());
        sendResponse(textMessage, 
                      timeToDeserializeCAS,
                      timeToSerializeCAS, 
                      timeInAnalytic,
                      timeIdle,
                      endTime-startTime,str.str(),true);
        endTime = apr_time_now();
        iv_pStatistics->processingComplete(0,false,endTime-startTime);
      }
      iv_busy=false;
      iv_timeLastRequestCompleted = apr_time_now();
    } catch (XMLException& e) {
      stringstream str;
      str << "AMQListener::onMessage() XMLException." << e.getMessage();
      LOGERROR(str.str());
      endTime = apr_time_now();
      sendResponse(textMessage, timeToDeserializeCAS,
                    timeToSerializeCAS, timeInAnalytic,
                    timeIdle, endTime-startTime,str.str(),true);
      endTime = apr_time_now();
      iv_pStatistics->processingComplete(command,false,endTime-startTime);
      iv_timeLastRequestCompleted = apr_time_now();
      iv_busy=false;
    }  catch (CMSException& e) {
      LOGERROR("AMQListener::onMessage()" + e.getMessage());
      endTime = apr_time_now();
      iv_pStatistics->processingComplete(0,false,endTime-startTime);
      iv_timeLastRequestCompleted = apr_time_now();
      iv_busy=false;
    } catch (uima::Exception e) {
      LOGERROR("AMQListener::onMessage() UIMA Exception " + e.asString());
      endTime = apr_time_now();
      sendResponse(textMessage, timeToDeserializeCAS,
                      timeToSerializeCAS, timeInAnalytic,
                      timeIdle,endTime-startTime,e.asString(),true);
      endTime = apr_time_now();
      iv_pStatistics->processingComplete(command,false,endTime-startTime);
      iv_timeLastRequestCompleted = apr_time_now();
      iv_busy=false;
    }	catch(...) {
      LOGERROR("AMQListener::onMessage() Unknown exception ");
      //TODO: log / shurdown ?}   
      endTime = apr_time_now();
      iv_pStatistics->processingComplete(command,false,endTime-startTime);
      iv_timeLastRequestCompleted = apr_time_now();
      iv_busy=false;
    }
    iv_timeLastRequestCompleted = apr_time_now();
  }

  void AMQListener::sendResponse(const TextMessage * request, 
                               apr_time_t timeToDeserialize,
                               apr_time_t timeToSerialize, 
                               apr_time_t timeInAnalytic, 
                               apr_time_t idleTime,
                               apr_time_t elapsedTime, 
                               string msgContent, bool isExceptionMsg) {

     //TODO retry
     string serverURI;
     AMQConnection * replyTo=NULL;
     TextMessage * reply=NULL;

     try {
       const Destination * cmsReplyTo = request->getCMSReplyTo();
       if (cmsReplyTo !=  NULL)  {
         reply = this->iv_pConnection->getTextMessage();
       } else {
         LOGINFO(2, "AMQListener::sendResponse() start");
         if (!request->propertyExists("ServerURI")  ) {
           LOGERROR("AMQListener::sendResponse() ServerURI header property does not exist.");
           return;
         }
         if (!request->propertyExists("MessageFrom")  ) {
           LOGERROR("AMQListener::sendResponse() MessageFrom header property does not exist.");
           return;
         }

         //get handle to a connection
         string tmp = request->getStringProperty("ServerURI");
         LOGINFO(FINEST,"replyTo ServerURI: " +  tmp);

         //special handling when protocol is http.
         //HTTP protocol not supported by ActiveMQ C++ client.
         //replace the http broker URL with the broker URL of
         //the queue this listener is attached to.
         if (tmp.find("http:") != string::npos) { 
           serverURI=this->iv_brokerURL;
           LOGINFO(FINER,"HTTP reply address: " + tmp);
         } else {  //send reply via tcp
           size_t begpos = tmp.find("tcp:",0);
           tmp = tmp.substr(begpos);
           size_t endpos = tmp.find_first_of(",");

           if (begpos == string::npos) { 
             LOGERROR("AMQListener::sendResponse() Could not find tcp URL in ServerURI header property.");	 
             return;
           }
           if (endpos == string::npos) {
             serverURI = tmp;
           } else {
             serverURI = tmp.substr(0, endpos);
           }
         }
         LOGINFO(FINER,"ReplyTo BrokerURL " + serverURI);

         //look up the endpoint connection
         replyTo = this->iv_replyToConnections.getConnection(serverURI);
         if (replyTo == NULL) {
           LOGERROR("Could not get a connection to " + serverURI);
           return;
         }
         //get message object
         reply = replyTo->getTextMessage();
       }

       if (reply == NULL) {
         LOGERROR("AMQListener::sendResponse() invalid textMessage object " );
         return;
       }

       //construct the reply message
       if (request->propertyExists("MessageFrom") ) {
         reply->setStringProperty("MessageFrom", this->iv_inputQueueName);
       }
       if (request->propertyExists("ServerURI") ) {
         reply->setStringProperty("ServerURI", serverURI);
       }
       reply->setIntProperty("MessageType",RESPONSE);
       reply->setLongProperty("TimeInService", elapsedTime*1000);
       reply->setLongProperty("IdleTime", idleTime*1000);
       if (request->propertyExists("CasReference")  ) {
         reply->setStringProperty("CasReference", request->getStringProperty("CasReference"));
       }
       if (request->propertyExists("Command")  ) {
         reply->setIntProperty("Command", request->getIntProperty("Command"));
         if (request->getIntProperty("Command") == PROCESS_CAS_COMMAND) {
           reply->setLongProperty("TimeToSerializeCAS", timeToSerialize*1000);
           reply->setLongProperty("TimeToDeserializeCAS", timeToDeserialize*1000);
           reply->setLongProperty("TimeInAnalytic", timeInAnalytic*1000);
         }
       }
       if (isExceptionMsg) {
         reply->setIntProperty("Payload",EXC_PAYLOAD);
       } else {
         if (request->propertyExists("Payload") ) {
           reply->setIntProperty("Payload",request->getIntProperty("Payload"));
         } else {
           reply->setIntProperty("Payload",NO_PAYLOAD);
         }
       }
       //cargo
       reply->setText(msgContent);	
       //log the reply message content
       stringstream str;
       str << "Sending Reply Command: " << reply->getIntProperty("Command") << " MessageType: " << reply->getIntProperty("MessageType") << " ";
       if (reply->propertyExists("CasReference") ) {
         str << "CasReference: " << reply->getStringProperty("CasReference");
       }
       LOGINFO(INFO,str.str());

       if (cmsReplyTo != NULL) {
         str << " to " << cmsReplyTo->toProviderString();
       } else {
         str << " to " << request->getStringProperty("MessageFrom") 
           << " at " << serverURI;
       }
       str << " Message text: " << msgContent;
       LOGINFO(FINEST,"PRINT Reply message:\n" + str.str());	

       //send
       if (cmsReplyTo != NULL) {
         //cout << "cmsReplyTo=" << cmsReplyTo->toProviderString() << endl;
         iv_pConnection->sendMessage(cmsReplyTo);
       } else {
         replyTo->sendMessage(request->getStringProperty("MessageFrom"));
       }
       LOGINFO(FINER,"AMQListener::sendResponse DONE");

     } catch (CMSException& ex ) {
       LOGERROR("AMQListener::onMessage()" +  ex.getMessage());
     } catch (...)  { 
       LOGERROR("AMQListener::onMessage() UnExpected error sending reply.");
     }
}

//===================================================
//AMQAnalysisEngineService
//---------------------------------------------------
  AMQAnalysisEngineService::~AMQAnalysisEngineService() {
    cleanup();
  }

  AMQAnalysisEngineService::AMQAnalysisEngineService(ServiceParameters & desc,
                                                     Monitor * stats) : 
                                        iv_pStatistics(stats),
                                        iv_numInstances(desc.getNumberOfInstances()),
                                        iv_brokerURL(desc.getBrokerURL()),
                                        iv_inputQueueName(desc.getQueueName()),
                                        iv_aeDescriptor(desc.getAEDescriptor()),
                                        iv_prefetchSize(desc.getPrefetchSize()),
                                        iv_initialFSHeapSize(desc.getInitialFSHeapSize()),
                                        iv_vecpConnections(),
                                        iv_vecpAnalysisEngines(),
                                        iv_vecpCas(),
                                        iv_listeners() {
    try {
      initialize();
    } catch (CMSException & e) {
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam(e.getMessage());
      ErrorInfo errInfo;
      errInfo.setMessage(msg);
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
        UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
        errInfo.getMessage(),
        errInfo.getMessage().getMessageID(),
        ErrorInfo::unrecoverable);
    }
  }

  void AMQAnalysisEngineService::initialize() {

    try {
      //create a connection for each instance
      for (int i=0; i < iv_numInstances; i++) {
        //create a connection to MQ Broker 
        AMQConnection * newConnection = new AMQConnection(this->iv_brokerURL.c_str());
        if (newConnection == NULL) {
          LOGERROR("AMQAnalysisEngineService::initialize() Could not create ActiveMQ endpoint connection.");
          ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
          msg.addParam("AMQAnalysisEngineService::initialize() Failed to connect to broker.");
          ErrorInfo errInfo;
          errInfo.setMessage(msg);
          UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
            UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
            errInfo.getMessage(),
            errInfo.getMessage().getMessageID(),
            ErrorInfo::unrecoverable);
        }
        newConnection->setExceptionListener(this);
        this->iv_vecpConnections.push_back(newConnection);
      }

      if (!uima::ResourceManager::hasInstance()) {
        uima::ResourceManager::createInstance("ActiveMQAnalysisEngineService");
      }
      ErrorInfo errInfo;
      UnicodeString ustr(this->iv_aeDescriptor.c_str());
      UnicodeString ufn = ResourceManager::resolveFilename(ustr,ustr);

      //create a AnalysisEngine and CAS for each instance
      for (int i=0; i < iv_numInstances; i++) {

        //create AE
        AnalysisEngine * pEngine = uima::TextAnalysisEngine::createTextAnalysisEngine((UnicodeStringRef(ufn).asUTF8().c_str()),
          errInfo);
        if (pEngine) {
          LOGINFO(0,"AMQAnalysisEngineService::initialize() AnalysisEngine initialization successful.");
        } else {
          LOGERROR("AMQAnalysisEngineService::initializer() could not create AE" + errInfo.asString());
          ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
          msg.addParam("AMQListener::initialize() create AE failed.");
          ErrorInfo errInfo;
          errInfo.setErrorId(UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE),
            errInfo.setMessage(msg);
          UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
            UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
            errInfo.getMessage(),
            errInfo.getMessage().getMessageID(),
            ErrorInfo::unrecoverable);
        }

        this->iv_vecpAnalysisEngines.push_back(pEngine);

        //initial FSHeap size
        if (this->iv_initialFSHeapSize > 0) {
          pEngine->getAnnotatorContext().
            assignValue(UIMA_ENGINE_CONFIG_OPTION_FSHEAP_PAGESIZE,this->iv_initialFSHeapSize);
        }

        //create CAS
        CAS * cas = pEngine->newCAS();
        if (cas == NULL) {
          LOGERROR("AMQAnalysisEngineService::initialize() Could not create CAS");
          ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
          msg.addParam("AMQListener::initialize() create CAS failed.");
          ErrorInfo errInfo;
          errInfo.setErrorId(UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE),
            errInfo.setMessage(msg);
          UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
            UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
            errInfo.getMessage(),
            errInfo.getMessage().getMessageID(),
            ErrorInfo::unrecoverable);
        }
        this->iv_vecpCas.push_back(cas);
      }

      //create listeners and register these
      for (int i=0; i < iv_numInstances; i++) {
        AMQListener * newListener = new AMQListener(i,iv_vecpConnections.at(i),
          iv_vecpAnalysisEngines.at(i), iv_vecpCas.at(i),
          iv_pStatistics);
        if (newListener == NULL) {
          LOGERROR("AMQAnalysisEngineService::initialize() Could not create listener.");
          ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
          msg.addParam("AnalysisEngineServcie::initialize() Could not create listener.");
          ErrorInfo errInfo;
          errInfo.setMessage(msg);
          UIMA_EXC_THROW_NEW(Uima_runtime_error, 
            UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
            errInfo.getMessage(),
            errInfo.getMessage().getMessageID(),
            ErrorInfo::unrecoverable);
        }
        this->iv_listeners[i] = newListener;

        //create MessageConsumer session and register Listener       
        this->iv_vecpConnections.at(i)->createMessageConsumer(iv_inputQueueName,
          newListener,this->iv_prefetchSize);
      }
      iv_pStatistics->setNumberOfInstances(iv_numInstances);
    } catch (uima::Exception e) {
      cout << __LINE__ << "got a uima exception " << endl;
      LOGERROR("AMQAnalysisEngineService::initialize() " + e.asString());
      throw e;
    }
  }

  void AMQAnalysisEngineService::setTraceLevel(int level) {
    if (level < 0) {
    uimacpp_ee_tracelevel=NONE;
    } else if (level == 0) {
      uimacpp_ee_tracelevel = INFO;
    } else if (level == 1) {
      uimacpp_ee_tracelevel = FINE;
    } else if (level == 2) {
      uimacpp_ee_tracelevel = FINER;
    } else if (level > 2) {
      uimacpp_ee_tracelevel = FINEST;
    } else {
      uimacpp_ee_tracelevel = INFO;
    }
    cout << "tracelevel=" << uimacpp_ee_tracelevel << endl;
  }

  void AMQAnalysisEngineService::onException( const CMSException& ex ) {
    LOGERROR("CMS Exception occured. Shutting down the service." + ex.getMessage()); 
    cerr << "Broken connection. Stopped receiving messages." << endl;
    stop();
    this->iv_pStatistics->shutdown();
  }

  void AMQAnalysisEngineService::onMessage( const Message* message ) {
    LOGINFO(0,"AMQAnalsisEngineService::onMessage() Got a message.");
  }

  void AMQAnalysisEngineService::start() {	
    for (size_t i=0; i < iv_vecpConnections.size(); i++) {
      cout << "Starting listener " << i << endl;
      AMQConnection * connection = iv_vecpConnections.at(i);
      if (connection != NULL) {
        connection->start();
      } else {
        LOGERROR("AMQAnalysisServiceEngine::start() Invalid connection object.");
        ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQAnalysisServiceEngine::start() Connection does not exist.");
        ErrorInfo errInfo;
        errInfo.setMessage(msg);
        UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
          UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
          errInfo.getMessage(),
          errInfo.getMessage().getMessageID(),
          ErrorInfo::unrecoverable);
      }
    }
    //update the start time
    iv_pStatistics->setStartTime();
  }

  int AMQAnalysisEngineService::stop() {
    //TODO: let listeners finish processing first
    //stop messages notification
    for (size_t i=0; i < iv_vecpConnections.size(); i++) {
      AMQConnection * connection = iv_vecpConnections.at(i);
      if (connection != NULL) {
        connection->stop();
      } else {
        LOGERROR("AMQAnalysisEngineService::stop() Connection object is NULL.");
        ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQAnalysisServiceEngine::start() Connection does not exist.");
        ErrorInfo errInfo;
        errInfo.setMessage(msg);
        UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
          UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
          errInfo.getMessage(),
          errInfo.getMessage().getMessageID(),
          ErrorInfo::unrecoverable);
      }
    }
    return 0;
  }	
  void AMQAnalysisEngineService::cleanup() {
    // Destroy resources.
    try { 
      //cout << "num consumerConnections " << consumerConnections.size() << endl;
      for (size_t i=0; i < iv_vecpConnections.size(); i++) {
        //cout << "deleting consumerConnection " << i << endl;	
        if (iv_vecpConnections.at(i) != NULL) {
          iv_vecpConnections.at(i)->stop();
          delete iv_vecpConnections.at(i);
        }
        if (iv_vecpAnalysisEngines.at(i) != NULL) {
          delete iv_vecpAnalysisEngines.at(i);
        }
        if (iv_vecpCas.at(i) != NULL) {
          delete iv_vecpCas.at(i);
        }     
      }  
      iv_vecpAnalysisEngines.clear();
      iv_vecpCas.clear();
      iv_vecpConnections.clear();
      map<int, AMQListener*>::iterator ite; 
      for (ite= iv_listeners.begin();ite !=  iv_listeners.end();ite++) {
        delete ite->second;
      }

      iv_listeners.clear();
    } catch (CMSException& e) {
      LOGERROR("AMQAnalysisEngineService::cleanup() " +  e.getMessage());
    }  
  }           



