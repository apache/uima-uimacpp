/*
* Analysis Engine service wrapper implementation based on 
* Active MQ C++ client.
*/


#include <activemq/concurrent/Thread.h>
#include <activemq/concurrent/Runnable.h>
#include <activemq/concurrent/Concurrent.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConstants.h>
#include <activemq/util/Integer.h>

#include "unicode/unistr.h"
#include "apr.h"

#include "uima/api.hpp"
#include "uima/xmlwriter.hpp"
#include "uima/xcasdeserializer.hpp"
#include "uima/xmiwriter.hpp"
#include "uima/xmideserializer.hpp"
#include "uima/xmlerror_handler.hpp"

using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::exceptions;
using namespace uima;

#include "ActiveMQAnalysisEngineService.hpp"
#include "deployCppService.hpp"

//UIMA_EXC_CLASSIMPLEMENT(UimacppException, Exception);

int uimacpp_ee_tracelevel=0;
#define MSGHEADER   apr_time_now() << " ThreadId: " << Thread::getId() << __FILE__ << ":" << __LINE__ 
#define FORMATMSG(x)  stringstream lstr; lstr << MSGHEADER << " " << x;
#define LOGINFO(n,x) { if (n > uimacpp_ee_tracelevel) {} else {FORMATMSG(x); ResourceManager::getInstance().getLogger().logMessage(lstr.str().c_str());} }
#define LOGWARN(x) { FORMATMSG(x); ResourceManager::getInstance().getLogger().logWarning(lstr.str().c_str()); }
#define LOGERROR(x){ FORMATMSG(x); ResourceManager::getInstance().getLogger().logError(lstr.str().c_str(),-99); }

//===================================================
//AMQConnection
//---------------------------------------------------
AMQConnection::AMQConnection( const char * aBrokerURL) : connection(0), session(0), iv_valid(false) {
 
	 try {
		 this->brokerURL = aBrokerURL;
     
		 LOGINFO(0, "AMQConnection() start " + brokerURL);
		 // Create a ConnectionFactory
		 ActiveMQConnectionFactory* connectionFactory = 
       new ActiveMQConnectionFactory(brokerURL);
    
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
		 LOGINFO(3, "AMQConnection() create connection " + brokerURL);
		 connection = connectionFactory->createConnection();
		 if (connection == NULL) {
			  LOGERROR("AMQConnection() could not create connection.");
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
			  msg.addParam("AMQConnection() could not create connection.");
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
			connection->setExceptionListener(this);	
     }
      
		 delete connectionFactory;
			
     LOGINFO(0, "AMQConnection() end " + brokerURL);
	 } catch (cms::CMSException& e) {
      cout << "CMSException " << endl;
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
    if (session != NULL) {
      delete session;
      session=NULL;
    }
    if (connection != NULL) {
      this->connection->close();
      delete connection;
      connection = NULL;
    }
  }
  /* create a MessageConsumer session */
  void AMQConnection::createSession(string queueName,
															 MessageListener * listener,
															 int prefetch) {
     LOGINFO(3, "AMQConnection::createSession() consumer start " + queueName);
     stringstream str;
     //we add one to prefetch size to get the same behavior as the
     //Spring listener used in UIMA Java SDK which calls the
     //ActiveMQMessageConsumer.receive() api.
     //create endpoint destination
		 str << queueName;
     str << "?consumer.prefetchSize=" << prefetch+1 << endl;	
 
		 if (this->session != NULL ) {
				LOGERROR("AMQConnection::createSession() A session exists. ");
				ErrorInfo errInfo;
				errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "A session exists."));
				UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		 }
		 this->session = new AMQEndPoint(this, queueName, listener);
		 if (this->session == NULL ) {
			  LOGERROR("AMQConnection::createSession failed.");
			  ErrorInfo errInfo;
        errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "AMQConnection::createSession() failed."));
			   UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		 }
		 LOGINFO(3, "AMQConnection::createSession() consumer done " + queueName);
  }

  //caller owns the ExceptionListener
  void AMQConnection::setExceptionListener(ExceptionListener  * el) {
		this->connection->setExceptionListener(el);
	}
 

  /* creates a MessageProducer not associated with any queue */
  void AMQConnection::createSession() {
    try {
      LOGINFO(3, "AMQConnection::createSession() producer start ");
      if (this->session != NULL ) {
        LOGERROR("AMQConnection::createSession() A session exists. ");
        ErrorInfo errInfo;
        errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "A session exists."));
        UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
      }
      string queueName;
      this->session = new AMQEndPoint(this, queueName);
      if (this->session == NULL ) {
        LOGERROR("WMQConnection::createSession failed. ");
        ErrorInfo errInfo;
        errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "AMQConnection::createSession failed."));
        UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
      }
      LOGINFO(3, "AMQConnection::createSession() producer done ");
    }  catch (CMSException & ex) {
      LOGERROR("AMQConnection::createSession()" + ex.getMessage());
      ErrorInfo errInfo;
      errInfo.setErrorId(UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE);
      errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, ex.getMessage()));
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
    }
  }

   void AMQConnection::onException(const CMSException & ex) {
			//mark endpoint as broken.
			this->iv_valid = false;
			//log that connection is invalid.
			LOGWARN("AMQConnection()::onException() Connection to " + brokerURL 
						 + " may be broken. " +
						 ex.getMessage());
	 }
	
	
   TextMessage * AMQConnection::getTextMessage() {
     if (session == NULL) {
       LOGERROR("AMQConnection::getTextMessage() Invalid session. ");
       ErrorInfo errInfo;
       errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "A session does not exist."));
       UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
     }
     return this->session->getTextMessage();
   }

   void AMQConnection::sendMessage(string queuename) {
     LOGINFO(4, "AMQConnection::sendMessage() to " + queuename);
     if (session == NULL) {
       LOGERROR("AMQConnection::getTextMessage Invalid session. ");
       ErrorInfo errInfo;
       errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "A session does not exist."));
       UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
     }
     return this->session->sendMessage(queuename);
   }

   void AMQConnection::sendMessage(const Destination * cmsReplyTo) {
     LOGINFO(4, "AMQConnection::sendMessage() to " + cmsReplyTo->toProviderString());
     if (session == NULL) {
       LOGERROR("AMQConnection::getTextMessage Invalid session. ");
       ErrorInfo errInfo;
       errInfo.setMessage(ErrorMessage(UIMA_MSG_ID_LOG_ERROR, "A session does not exist."));
      UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
     }
     return this->session->sendMessage(cmsReplyTo);
   }

   // must be called to start receiving messages 
   void AMQConnection::start() {
     if (connection != NULL) {
       connection->start();
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
     if (connection != NULL) {
       connection->stop();
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
	  
  //----------------------------------------------------
	// AMQEndPoint
	//----------------------------------------------------

  AMQEndPoint::AMQEndPoint(AMQConnection * conn, 
                            string queueName,
                            MessageListener * listener) {
	 session=NULL;
	 producer=NULL;
   consumer=NULL;
	 destination=NULL;
   textMessage=NULL;
	 this->pConnection = conn;
	 this->queueName = queueName;
   this->pListener = listener;

	 try {

       // Create a Session
			LOGINFO(3,"AMQEndPoint() create Session " + brokerURL);
			session = conn->connection->createSession( Session::AUTO_ACKNOWLEDGE );
			
		  if  (session == NULL) {
			  LOGERROR("AMQEndPoint() createSession failed."); 
			  ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
			  msg.addParam("AMQEndPoint() createSession failed." );
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
      } 
		  	 
      cms::Queue * newDest = NULL;
      if (this->queueName.length() > 0 ) {
          
        LOGINFO(3, "AMQEndPoint::create Queue " + this->queueName);
        newDest = session->createQueue(this->queueName);

		    if (newDest == NULL) {
			    LOGERROR("AMQEndPoint() createQueue failed. " + queueName);
		      stringstream str;
			    str << "createQueue() failed. " << queueName << endl;
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
      }

      if (pListener != NULL) {
        if (newDest == NULL) {
          ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
          stringstream str;
          str << "AMQEndPoint() invalid consumer destination " << queueName << endl;
			    msg.addParam(str.str());
			    ErrorInfo errInfo;
			    errInfo.setMessage(msg);
			    UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
        }
        destination=newDest;
		    //create MessageConsumer
			  LOGINFO(3,"AMQEndPoint() create Consumer " + brokerURL + " " + queueName);
		    consumer = session->createConsumer(destination);
		    if (consumer == NULL) {
			    LOGERROR("AMQEndPoint() could not create MessageConsumer " + queueName);
          ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
			    msg.addParam("AMQEndPoint() createConsumer() failed.");
			    ErrorInfo errInfo;
			    errInfo.setMessage(msg);
			    UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		    }
		    //register listener
		    consumer->setMessageListener(listener); //caller owns listener 
      } else {
        LOGINFO(3,"AMQEndPoint() create Producer ");
		    producer = session->createProducer(0);
		    if (producer == NULL) {
			    LOGERROR("AMQEndPoint() could not create MessageProducer ");
          ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
			    msg.addParam("AMQEndPoint() createConsumer() failed.");
			    ErrorInfo errInfo;
			    errInfo.setMessage(msg);
			    UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		    }
        producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );
        if (newDest != NULL) {
          replyDestinations[queueName] = newDest;
        }
      }
			
			LOGINFO(0, "AMQEndPoint() start " + brokerURL + " " + queueName);
	 } catch (cms::CMSException& e) {
			LOGERROR("AMQEndPoint(): " + e.getMessage());  
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

	AMQEndPoint::~AMQEndPoint() {
    cleanup();
	}



 	/** this class owns TextMessage */
 TextMessage * AMQEndPoint::getTextMessage() {
	  if (session == NULL) {
			LOGERROR("AMQEndPoint::getTextMessage() session is NULL. ");
			ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
			msg.addParam("getTextMessage() invalid producerSession.");
			ErrorInfo errInfo;
			errInfo.setMessage(msg);
			 UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		}
	  if (textMessage == NULL) {
			textMessage = session->createTextMessage();
		}
		if (textMessage == NULL) {
			LOGERROR("AMQEndPoint::getTextMessage() textMessage object is NULL. ");
			ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
			msg.addParam("getTextMessage() failed to create message.");
			ErrorInfo errInfo;
			errInfo.setMessage(msg);
			 UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		}
		return textMessage; 
	}

	/** this class owns Message */
 void AMQEndPoint::sendMessage(const Destination * cmsReplyTo) {
	  LOGINFO(3,"AMQEndPoint::sendMessage() start");
		try {
			if (this->pConnection == NULL) {
			  LOGERROR("AMQEndPoint::sendMessage() Invalid connection. ");
			  ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() Connection does not exist.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			   UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		  } 
			
			if (session == NULL) {
				LOGERROR("AMQEndPoint::sendMessage() producer session is NULL. ");
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() producer session is NULL.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
			}
			if (cmsReplyTo==NULL) {
				LOGERROR("AMQEndPoint::sendMessage() CMSReplyTo is null." + queueName);
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() destination is NULL.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
			}
			if (producer == NULL) { 
				LOGERROR("AMQEndPoint::sendMessage() producer is null " + queueName);
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() Producer is NULL.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
			}
			if (textMessage == NULL) { 
				LOGERROR("AMQEndPoint::sendMessage() textMessage is null " + queueName);
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() textMessage is NULL.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
			}
     
      producer->send(cmsReplyTo,this->textMessage);
      
			//cout << "producer->send elapsed time " << (apr_time_now() - stime) << endl;
			this->textMessage->clearBody();
			this->textMessage->clearProperties();
			LOGINFO(3,"AMQEndPoint::sendMessage() done");
		} catch (cms::CMSException & e) {
      stringstream str;
      str << "AMQConnection::sendMessage() "  << e.getMessage();
			LOGERROR(str.str());
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
	}


 	/** this class owns Message */
 void AMQEndPoint::sendMessage(string queueName) {
	  LOGINFO(3,"AMQEndPoint::sendMessage() start");
		try {
			if (pConnection == NULL) {
			  LOGERROR("AMQEndPoint::sendMessage() Invalid connection. ");
			  ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() Connection does not exist.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		  } 
			
			if (session == NULL) {
				LOGERROR("AMQEndPoint::sendMessage() producer session is NULL. ");
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() producer session is NULL.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
			}
	
			if (producer == NULL) { 
				LOGERROR("AMQEndPoint::sendMessage() producer is null " + queueName);
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() Producer is NULL.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
			}
			if (textMessage == NULL) { 
				LOGERROR("AMQEndPoint::sendMessage() textMessage is null " + queueName);
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() textMessage is NULL.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
			}
      cms::Destination * pDest;
      map<string,Destination*>::iterator ite = replyDestinations.find(queueName);
      if (ite != replyDestinations.end()) {
        pDest = ite->second;
      } else {
        pDest = session->createQueue(queueName);
        if (pDest != NULL) {
          replyDestinations[queueName] = pDest;
        }
      }

      if (pDest == NULL) {
        LOGERROR("AMQEndPoint::sendMessage() invalid destination " + queueName);
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::sendMessage() invalid destination.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
      }
     
      producer->send(pDest,this->textMessage);
      
			//cout << "producer->send elapsed time " << (apr_time_now() - stime) << endl;
			this->textMessage->clearBody();
			this->textMessage->clearProperties();
			LOGINFO(3,"AMQEndPoint::sendMessage() done");
		} catch (cms::CMSException & e) {
			LOGERROR("AMQConnection::sendMessage() "  + e.getMessage());
			ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
      msg.addParam("AMQEndPoint::sendMessage() Connection does not exist.");
			ErrorInfo errInfo;
			errInfo.setMessage(msg);
		 UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		} 
	}

 
 void AMQEndPoint::cleanup () {

    try {
		 
		 if (destination != NULL) {
			 delete destination;
			 destination=NULL;
		 }

     if (this->consumer !=NULL) {
        this->consumer->close();
        delete this->consumer;
        this->consumer=NULL;
     }
     if (this->producer !=NULL) {
        this->producer->close();
        delete this->producer;
        this->producer=NULL;
     }

     if (this->textMessage != NULL) {
       delete this->textMessage;
       this->textMessage=NULL;
     }
	   
     if (session != NULL) {  //session owns consumer
			 if (this->isValid()) {
			   session->close(); 
			 }
       delete session;
			 session = NULL;
		 }
		 
     if (pListener != NULL) {
       delete pListener;
       pListener=NULL;
     }

     //destinations
     map<string, cms::Destination*>::iterator ite; 
		 for (ite= replyDestinations.begin();ite !=  replyDestinations.end();ite++) {
			  delete ite->second;
		 }

	 } catch (CMSException& e) {
		 LOGERROR("AMQEndPoint::cleanup() " + e.getMessage());
	 }


 } 

//===================================================
//AMQConnectionCache
//---------------------------------------------------
 AMQConnectionsCache::AMQConnectionsCache() {

	}

 AMQConnectionsCache::~AMQConnectionsCache() {
		map<string,AMQConnection*>::iterator ite;
		for (ite = this->allEndpoints.begin(); ite != this->allEndpoints.end(); ite++) {
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
   string threadIdStr = Integer::toString( Thread::getId() );
   LOGINFO(2,"AMQConnectionCache::getEndPoint() looking up connection to " 
     + brokerURL );	 
   AMQConnection * endpoint=NULL;
  	
   try {	    	
     map<string,AMQConnection*>::iterator ite;
     ite = this->allEndpoints.find(brokerURL);
     if (ite == allEndpoints.end()) {
       LOGINFO(1,"AMQConnectionsCache::getConnection() create new connection to " +
         brokerURL);

       endpoint = new AMQConnection(brokerURL.c_str());
       if (endpoint == NULL) {
         LOGERROR("AMQConnectionCache::getConnection Could not create a endpoint connection to " +
           brokerURL);
       } else {								    
         this->allEndpoints[brokerURL] = endpoint;
       }
     } else {
       endpoint = ite->second;
       //if not a valid connection, reconnect
       if (endpoint == NULL) {
         LOGERROR("AMQConnectionCache::getConnection() could not connect to "
           + brokerURL);
       } else {
         if (!endpoint->isValid()) {
           LOGWARN("AMQConnectionCache::getEndPoint() Existing connection invalid. Reconnecting to " + brokerURL );
           delete endpoint;
           this->allEndpoints.erase(brokerURL);
           endpoint = new AMQConnection(brokerURL.c_str());
           if (endpoint == NULL) {
             LOGERROR("AMQConnectionCache::getConnection() could not connect to "
               + brokerURL );
           } else {
             LOGWARN("AMQConnectionCache::getConnection() reconnected to " + 
               brokerURL);
             this->allEndpoints[brokerURL] = endpoint; 
           }
         }
       }
     }

     return endpoint;		
   } catch (cms::CMSException & e) {
     LOGERROR("AMQConnectionCache::getConnection() " + e.getMessage());
     ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
     msg.addParam("AMQConnectionCache::getConnection " + e.getMessage());
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
     return NULL;
   }
 }


//===================================================
//ActiveMQListener
//---------------------------------------------------
 void AMQListener::initialize() {
		try {
			string threadIdStr = Integer::toString( Thread::getId() );
			if (!uima::ResourceManager::hasInstance()) {
				uima::ResourceManager::createInstance("ActiveMQAnalysisEngineService");
			}
			//create AE
			ErrorInfo errInfo;
      UnicodeString ustr(aeDescriptor.c_str());
      UnicodeString ufn = ResourceManager::resolveFilename(ustr,ustr);
			pEngine = uima::TextAnalysisEngine::createTextAnalysisEngine((UnicodeStringRef(ufn).asUTF8().c_str()),
               errInfo);
			if (pEngine) {
				LOGINFO(0,"AMQListener::initialize() AnalysisEngine initialization successful.");
			} else {
				LOGERROR("AMQListener::initializer() could not create AE" + errInfo.asString());
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
      
      //initial FSHeap size
      if (this->initialfsheapsize > 0) {
        pEngine->getAnnotatorContext().
          assignValue(UIMA_ENGINE_CONFIG_OPTION_FSHEAP_PAGESIZE,this->initialfsheapsize);
      }

			//create CAS
			cas = pEngine->newCAS();
			if (cas == NULL) {
				LOGERROR("AMQListener::initialize() Could not create CAS");
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

      pCmsReplyToConnection =  new AMQConnection(this->brokerURL.c_str());
      if (pCmsReplyToConnection == NULL) {
        LOGERROR("AMQListener::initialize() Could not create cmsReplyTo Connection");
				ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam("AMQEndPoint::initialize() could not create broker connection for replies.");
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
      }

      pCmsReplyToConnection->createSession();
		
		} catch (uima::Exception e) {
			LOGERROR("AMQListener::initialize()" + e.asString());
      ErrorMessage msg(UIMA_MSG_ID_LOG_ERROR);
        msg.addParam(e.asString());
			  ErrorInfo errInfo;
			  errInfo.setMessage(msg);
			  UIMA_EXC_THROW_NEW(uima::Uima_runtime_error, 
                         UIMA_ERR_RESMGR_COULD_NOT_INITIALIZE_RESOURCE,
                         errInfo.getMessage(),
                         errInfo.getMessage().getMessageID(),
                         ErrorInfo::unrecoverable);
		}
	}

 AMQListener::AMQListener(string aeDescriptor, size_t fsheapsz,
                string aBrokerURL, 
                string queueName, int id, Monitor * stats)
													/*	AMQConnectionsCache & replyToConnections) */ : 
			pCmsReplyToConnection(0), pEngine(0), cas(0), count(0), 
         initialfsheapsize(fsheapsz),
				 busy(false), pStatistics(stats), replyToConnections(replyToConnections)
         {	  
				this->aeDescriptor = aeDescriptor;
				this->brokerURL = aBrokerURL;
				this->queueName = queueName;
				this->id = id;  
				initialize();
	}

	AMQListener::~AMQListener() {
		if (cas != NULL) {
			delete cas;
		}
		if (pEngine != NULL) {
			delete pEngine;
		}
    if (pCmsReplyToConnection != NULL) {
      delete pCmsReplyToConnection;
    }
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
    apr_time_t timeIdle = apr_time_now() - timeLastRequestCompleted;

		const TextMessage* textMessage=0;
    int command = 0;
		// Get the Thread Id String
		string threadIdStr = Integer::toString( Thread::getId() );
	
		stringstream astr;
		astr << "*****Message#: " << ++count << "*****" << endl;
		LOGINFO(1,astr.str());
		
		try {	
			busy=true;
			textMessage = dynamic_cast< const TextMessage* >( message );
			if (textMessage==0) {
				LOGERROR("AMQListener::onMessage() invalid pointer to TextMessage");
        endTime = apr_time_now();
        pStatistics->processingComplete(0,false,endTime-startTime);
        this->timeLastRequestCompleted = apr_time_now();
        return;
			}
			if (textMessage->propertyExists("MessageFrom")) {	
					LOGINFO(2,"Received from " + textMessage->getStringProperty("MessageFrom"));
			} else {	   
				LOGERROR("AMQListener::onMessage() ERROR MessageFrom not set.");
				endTime = apr_time_now();
        pStatistics->processingComplete(0,false,endTime-startTime);
        this->timeLastRequestCompleted = apr_time_now();
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
        pStatistics->processingComplete(0,false,endTime-startTime);
        this->timeLastRequestCompleted = apr_time_now();
        return;
			}
			stringstream str;
			str << "Command: " << command ;
			LOGINFO(2, str.str().c_str());
			
			if (command == PROCESS_CAS_COMMAND) { //process CAS
				LOGINFO(1,"Process CAS request start.");

				//get the payload property
				if (!textMessage->propertyExists("Payload")) {
					LOGERROR("AMQListener::onMessage() Required property Payload not set.");
					endTime = apr_time_now();
          sendResponse(textMessage, timeToDeserializeCAS,
                     timeToSerializeCAS, timeInAnalytic,
                     timeIdle,endTime-startTime,
										"Required property 'Command' not set." ,true);
          
          endTime = apr_time_now();
          pStatistics->processingComplete(command,false,endTime-startTime);
          this->timeLastRequestCompleted = apr_time_now();
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
          pStatistics->processingComplete(command,false,endTime-startTime);
          this->timeLastRequestCompleted = apr_time_now();
          return;
				}
				str.flush();
				str  << "Payload: " << payload;
				LOGINFO(2, str.str());
				LOGINFO(2, "  Content: " +  text);
			
				//InputSource
				MemBufInputSource memIS((XMLByte const *)text.c_str(),
					text.length(),
					"sysID");

				//reset the CAS
				cas->reset();
				stringstream xmlstr;  
				//deserialize payload data into the CAS,
				//call AE process method and serialize
				//the CAS which will be sent with the
				//response.
				if (payload == XCAS_PAYLOAD) {
          startDeserialize = apr_time_now();
					XCASDeserializer::deserialize(memIS, *cas);	 
          startAnnotatorProcess=apr_time_now();
          timeToDeserializeCAS = startAnnotatorProcess-startDeserialize;
          
          pEngine->process(*cas);
          startSerialize=apr_time_now();
          timeInAnalytic = startSerialize - startAnnotatorProcess;

					XCASWriter xcaswriter(*cas, true);
					xcaswriter.write(xmlstr);
          timeToSerializeCAS = apr_time_now() - startSerialize;
				} else if (payload == XMI_PAYLOAD) {
          //deserialize incoming xmi CAS data.
          startDeserialize = apr_time_now();
					XmiSerializationSharedData sharedData;
					XmiDeserializer::deserialize(memIS, *cas, sharedData);      
          startAnnotatorProcess=apr_time_now();
          timeToDeserializeCAS = startAnnotatorProcess-startDeserialize;
         
					pEngine->process(*cas);
          startSerialize=apr_time_now();
          timeInAnalytic = startSerialize - startAnnotatorProcess;

				  //serialize CAS 
					XmiWriter xmiwriter(*cas, true, &sharedData);
					xmiwriter.write(xmlstr);
				  timeToSerializeCAS = apr_time_now() - startSerialize;
				} else {
					xmlstr << "Invalid Payload " << payload;
					LOGERROR("AMQListener::onMessage() " + xmlstr.str());
          stringstream str;
          str << "Invalid Payload " << payload << endl;
          endTime=apr_time_now();
          sendResponse(textMessage, timeToDeserializeCAS,
                     timeToSerializeCAS, timeInAnalytic,
                     timeIdle,endTime-startTime,
						str.str() ,true);
          endTime=apr_time_now();
          pStatistics->processingComplete(command,false,endTime-startTime);
          this->timeLastRequestCompleted = apr_time_now();
          return;
				}
				//done with this CAS.
				cas->reset(); 
				//record end time
				endTime = apr_time_now();
				//send reply
				LOGINFO(2,"UIMA processing finished.");		
				sendResponse(textMessage, timeToDeserializeCAS,
                     timeToSerializeCAS, timeInAnalytic,
                     timeIdle, 
                     endTime-startTime,
                     xmlstr.str(),
                     false);
        endTime=apr_time_now();
        pStatistics->processingComplete(command,true,endTime-startTime,
        timeToDeserializeCAS, timeInAnalytic, timeToSerializeCAS);
        LOGINFO(1,"Process CAS finished.");		
			} else if (command ==  GET_META_COMMAND ) { //get Meta 	 
				LOGINFO(1, "Process getMeta request start.");
				const AnalysisEngineMetaData & aeMetaData = pEngine->getAnalysisEngineMetaData();			
				icu::UnicodeString xmlBuffer;
				xmlBuffer.insert(0, "<?xml version=\"1.0\"?>");
				pEngine->getAnnotatorContext().getTaeSpecifier().toXMLBuffer(aeMetaData,
					false, xmlBuffer);
				UnicodeStringRef uref(xmlBuffer.getBuffer(), xmlBuffer.length());
				endTime = apr_time_now();
				LOGINFO(2,"UIMA processing finished.");	
				sendResponse(textMessage, timeToDeserializeCAS,
                     timeToSerializeCAS, timeInAnalytic,
                     timeIdle, endTime-startTime,uref.asUTF8(),false);			
        endTime=apr_time_now();
        pStatistics->processingComplete(command,true,endTime-startTime);
				LOGINFO(1,"Process getMeta request finished.");			
			} else if (command ==  CPC_COMMAND ) { //CPC       			
				LOGINFO(1, "Processing CollectionProcessComplete request start");				
				pEngine->collectionProcessComplete();
				endTime = apr_time_now();
				LOGINFO(2,"UIMA processing finished.");	
				sendResponse(textMessage, timeToDeserializeCAS,
                     timeToSerializeCAS, timeInAnalytic,
                     timeIdle,endTime-startTime,"CPC completed.",false);
        endTime=apr_time_now();
        pStatistics->processingComplete(command,true,endTime-startTime);
				LOGINFO(1, "Processing CollectionProcessComplete request finished.");	
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
        pStatistics->processingComplete(0,false,endTime-startTime);
			}
			busy=false;
      this->timeLastRequestCompleted = apr_time_now();
		} catch (XMLException& e) {
			stringstream str;
		  str << "AMQListener::onMessage() XMLException." << e.getMessage();
			LOGERROR(str.str());
      endTime = apr_time_now();
      sendResponse(textMessage, timeToDeserializeCAS,
                     timeToSerializeCAS, timeInAnalytic,
                     timeIdle, endTime-startTime,str.str(),true);
			busy=false;
      endTime = apr_time_now();
      pStatistics->processingComplete(command,false,endTime-startTime);
      this->timeLastRequestCompleted = apr_time_now();
		}  catch (CMSException& e) {
			LOGERROR("AMQListener::onMessage()" + e.getMessage());
			busy=false;
      endTime = apr_time_now();
      pStatistics->processingComplete(0,false,endTime-startTime);
      this->timeLastRequestCompleted = apr_time_now();
		} catch (uima::Exception e) {
			LOGERROR("AMQListener::onMessage() UIMA Exception " + e.asString());
			apr_time_t endTime;
			endTime = apr_time_now();
			sendResponse(textMessage, timeToDeserializeCAS,
                     timeToSerializeCAS, timeInAnalytic,
                     timeIdle,endTime-startTime,e.asString(),true);
			busy=false;
      endTime = apr_time_now();
      pStatistics->processingComplete(command,false,endTime-startTime);
      this->timeLastRequestCompleted = apr_time_now();
		}	catch(...) {
			LOGERROR("AMQListener::onMessage() Unknown exception ");
			//TODO: log / shurdown ?}   
			busy=false;
      endTime = apr_time_now();
      pStatistics->processingComplete(command,false,endTime-startTime);
      this->timeLastRequestCompleted = apr_time_now();
		}
    this->timeLastRequestCompleted = apr_time_now();
	}

  void AMQListener::sendResponse(const TextMessage * request, 
                                  apr_time_t timeToDeserialize,
                                  apr_time_t timeToSerialize, 
                                  apr_time_t timeInAnalytic, 
                                  apr_time_t idleTime,
                                  apr_time_t elapsedTime, 
                                  string msgContent, bool isExceptionMsg) {

      string threadIdStr = Integer::toString( Thread::getId() );
      //TODO retry
      string serverURI;
      AMQConnection * replyTo=NULL;
      TextMessage * reply=NULL;

      try {
        const Destination * cmsReplyTo = request->getCMSReplyTo();
        if (cmsReplyTo !=  NULL)  {
          reply = pCmsReplyToConnection->getTextMessage();
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

          LOGINFO(3,"replyTo ServerURI: " +  tmp);

          //special handling when protocol is http.
          //HTTP protocol not supported by ActiveMQ C++ client.
          //replace the http broker URL with the broker URL of
          //the queue this listener is attached to.
          if (tmp.find("http:") != string::npos) { 
            serverURI=this->brokerURL;
            LOGINFO(2,"HTTP reply address: " + tmp);
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
          LOGINFO(2,"Sending Reply to " + serverURI);

          //look up the endpoint connection
          replyTo = replyToConnections.getConnection(serverURI);
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

        //set header info
        if (request->propertyExists("MessageFrom") ) {
          reply->setStringProperty("MessageFrom", this->queueName);
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
        //log the reply message content
        stringstream str;
        if (cmsReplyTo != NULL) {
          str << "Sending Response to " << cmsReplyTo->toProviderString();
        } else {
          str << "Sending Response to " << request->getStringProperty("MessageFrom") 
          << " at " << serverURI;
        }
        LOGINFO(2,str.str());
        str.flush();
        str << "MessageFrom  : " << reply->getStringProperty("MessageFrom") << "\n"
          << "  ServerURI    : " << reply->getStringProperty("ServerURI") << "\n" 
          << "  MessageType  : " << reply->getIntProperty("MessageType") << "\n" 
          << "  TimeInService: " << reply->getLongProperty("TimeInService") << "\n" ;
        if (request->propertyExists("CasReference")  ) {
          str << "CasReference  : " << reply->getStringProperty("CasReference") << "\n";
        }
        if (request->propertyExists("Command")  ) {
          str << "Command : " << reply->getIntProperty("Command") << "\n";
        }
        if (request->propertyExists("Payload") ) {
          str << "Payload : " << reply->getIntProperty("Payload") << "\n";
        } 
        str << "Message text " << msgContent;
        LOGINFO(2,"PRINT Reply message:\n" + str.str());

        //cargo
        reply->setText(msgContent);		
        //send
        if (cmsReplyTo != NULL) {
          //cout << "cmsReplyTo=" << cmsReplyTo->toProviderString() << endl;
          pCmsReplyToConnection->sendMessage(cmsReplyTo);
        } else {
          replyTo->sendMessage(request->getStringProperty("MessageFrom"));
        }
        LOGINFO(2,"AMQListener::sendResponse DONE");

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

	AMQAnalysisEngineService::AMQAnalysisEngineService(ServiceParameters & desc, Monitor * stats) {
		try {
      pStatistics = stats;
		  initialize( desc.getAEDescriptor(), 
                  desc.getInitialFSHeapSize(),
			            desc.getBrokerURL(),
			            desc.getQueueName(), 
			            desc.getPrefetchSize(),
			            desc.getNumberOfInstances());
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

	void AMQAnalysisEngineService::initialize(string  aeDescriptor,
                           size_t fsheapsz, 
                           string  serverURI, 
																		string  queueName, int prefetch, int numInstances) {
		count=0;
		this->queueName = queueName;		 	
		//Construct as many MessageConsumers as specified in numInstances 
		try {
      for (int i=0; i < numInstances; i++) {
        //create a connection to MQ Broker 
			  AMQConnection * newConnection = new AMQConnection(serverURI.c_str());
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
			  consumerConnections.push_back(newConnection);

			  //create a MessageListener
			  AMQListener * newListener = new AMQListener(aeDescriptor, 
                 fsheapsz, serverURI, queueName, i, pStatistics);
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
			  listeners[i] = newListener;
        
        //create MessageConsumer session and register Listener       
        newConnection->createSession(queueName, newListener, prefetch);
		  }
      pStatistics->setNumberOfInstances(numInstances);
		} catch (uima::Exception e) {
      cout << __LINE__ << "got a uima exception " << endl;
			LOGERROR("AMQAnalysisEngineService::initialize() " + e.asString());
		  throw e;
		}
	}

	void AMQAnalysisEngineService::setTraceLevel(int level) {
		uimacpp_ee_tracelevel=level;
		cout << "tracelevel=" << uimacpp_ee_tracelevel << endl;
	}

	void AMQAnalysisEngineService::onException( const CMSException& ex ) {
		LOGERROR("CMS Exception occured. Shutting down the service." + ex.getMessage()); 
		cerr << "Broken connection. Stopped receiving messages." << endl;
		stop();
		cleanup();
	}

	void AMQAnalysisEngineService::onMessage( const Message* message ) {
		LOGINFO(0,"AMQAnalsisEngineService::onMessage() Got a message.");
	}

	void AMQAnalysisEngineService::start() {	
		for (size_t i=0; i<consumerConnections.size(); i++) {
			cout << "Starting listener " << i << endl;
			AMQConnection * connection = consumerConnections.at(i);
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
    pStatistics->setStartTime();
		
	}

	int AMQAnalysisEngineService::stop() {
		//TODO: let listeners finish processing first
		//stop messages notification
		for (size_t i=0; i < consumerConnections.size(); i++) {
			AMQConnection * connection = consumerConnections.at(i);
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
			for (size_t i=0; i<consumerConnections.size(); i++) {
        //cout << "deleting consumerConnection " << i << endl;	
        if (consumerConnections.at(i) != NULL) {
          consumerConnections.at(i)->stop();
				  delete consumerConnections.at(i);
        }
			}      
      consumerConnections.clear();
		} catch (CMSException& e) {
			LOGERROR("AMQAnalysisEngineService::cleanup() " +  e.getMessage());
		}  
	}           

