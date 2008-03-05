/*
* Analysis Engine service wrapper based on 
* Active MQ C++ client.
*/

#ifndef __ACTIVEMQ_AE_SERVICE__
#define __ACTIVEMQ_AE_SERVICE__




#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <activemq/core/ActiveMQConsumer.h>


#include "unicode/unistr.h"
#include "apr.h"
#include "uima/api.hpp"
#include <apr_network_io.h>

#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace activemq::concurrent;
using namespace cms;
using namespace uima;

//Forward declarations
class AMQEndPoint;
class Monitor;
class ServiceParameters;


//UIMA_EXC_CLASSDECLARE(UimacppException, Exception);
//========================================================
// This class manages an ActiveMQ connection. 
// It wraps either a MessageConsumer or a MessageProducer
// session.
//--------------------------------------------------------
class AMQConnection : public ExceptionListener {
  friend class AMQEndPoint;
private:
	
  string brokerURL;
  Connection* connection;
  AMQEndPoint * session;
  bool iv_valid;

public:
	//Establish connection to a ActiveMQ broker. This creates a Consumer Session.
	AMQConnection ( const char * aBrokerURL);

  /** Creates a MessageConsumer session and registers a listener. */
	void createSession(string aQueueName, MessageListener * pListener, int prefetch); 
 
  /** Creates a MessageProducer session not associated with any queue.
	 * The queue name must be specifed when this is used to send a 
	 * message 
	 */
  void createSession();
	
  /* destructor */
	~AMQConnection();

	/* caller owns the ExceptionListener */
	void setExceptionListener(ExceptionListener  * el);

	/* This also is a default Exception handler */
	void onException( const CMSException& ex);

	// If this is consumer, must be called to start receiving messages 
	void start();

	//If this is a consumer, stops receiving messages
	void stop();

	/** returns a TextMessage owned by this class. */
	TextMessage * getTextMessage();

	/** sends the message and clears it. */
	void sendMessage(string queueName);

  /** sends the message and clears it. */
	void sendMessage(const Destination * cmsReplyTo);
  
  bool isValid() {
    return iv_valid;
  }

};

/** This class represents a session - either a Message Consumer 
  *  or a Message Producer.
  */
class AMQEndPoint  {
private:

	AMQConnection* pConnection;
  Session* session;

	MessageConsumer * consumer;  
  cms::Queue* destination;  //Message Consumer input queue  
  MessageListener * pListener;
	
  MessageProducer * producer;  
	TextMessage * textMessage;
  //cache queue destination objects and
  //reuse when sending messages.
  map<string, cms::Destination*> replyDestinations;
 
	void cleanup();
	
public:
	string brokerURL;
  string queueName;
	/** This creates a session. */
	AMQEndPoint ( AMQConnection * aconnection, 
                string queueName,
                MessageListener * listener=0); 
	
	/** destructor */
	~AMQEndPoint();

	/** returns a TextMessage owned by this class. */
	TextMessage * getTextMessage();

  /** sends the message and clears it. */
  void sendMessage(const cms::Destination * dest);
  
	/** sends the message and clears it. */
	void sendMessage(string queueName);
  
	/** is connection valid */
	bool isValid() {
			return pConnection->isValid();
	}

};


//==================================================
// This class is used to cache and reuse connections.
// Used by the listener to send responses.
//--------------------------------------------------
class AMQConnectionsCache  {
private:
	map<string, AMQConnection *> allEndpoints; //key is brokerurl
	
public:
	AMQConnectionsCache(); 

	~AMQConnectionsCache(); 


	//Retrieves a Connection from the cache if it 
	//exists or establishes a connection to the
	//the specified broker and adds to the cache 
	//and returns the new connection.
	AMQConnection * getConnection(string brokerURL); 
	
};

//======================================================
// A MessageListener that services getMeta, processCAS
// and Collection Processing Complete requests.
//
// On initialization, it creates an instance of an 
// AnalysisEngine and a CAS.
//
// Handles GETMETA, PROCESSCAS and CPC requests.
// Records timing and error JMX statistics.
//
//------------------------------------------------------
class AMQListener : public MessageListener {
private:
	int id;									//Listener id
	string queueName;				//queue this listener gets messages from
	string brokerURL;				//broker this listener is connected to 
  AMQConnection * pCmsReplyToConnection;

	AnalysisEngine * pEngine;	//AnalysisEngine
	CAS * cas;								//CAS
	string aeDescriptor;			//AE descriptor filename
  size_t initialfsheapsize;

	int count;                //num messages processed
	bool busy;								//is processing a message
  apr_time_t timeLastRequestCompleted;
  
  Monitor * pStatistics;   //JMX statistics

	AMQConnectionsCache    replyToConnections; //connections cache used when sending reply.

	/*
	* instantiates an AnalysisEngine from the given AE descriptor
	* and create a CAS. 
	*/
	void initialize(); 

	void sendResponse(const TextMessage * request, apr_time_t elapsedTime, 
		string msgContent, bool isExceptionMsg); 

  void sendResponse(const TextMessage * request, apr_time_t timeToDeserialize,
                    apr_time_t timeToSerialize, apr_time_t timeInAnalytic,
                    apr_time_t idleTime, 
                    apr_time_t elapsedTime, 
		string msgContent, bool isExceptionMsg); 
public:
	/** constructor */ 
	AMQListener(string  aeDescriptor, 
                    size_t initialFsHeapSize,
                    string brokerURL, 
                    string  queueName, 
                    int id,
                    Monitor * pStatistics); 
	~AMQListener(); 

	bool isBusy() {
		return busy;
	}

  /*
	* Receive a TextMessage and examine the header properties
	* to determine the type of request and payload.  Processes
	* one request at a time and is blocked till the requestis 
	* handled and the response sent.
	*/
	virtual void onMessage( const Message* message ); 
	
};

//================================================
// This class creates and configures a C++  service
// according to parameters passed in as arguments.
//
// It requires that the UIMA C++ SDK as well as the
// annotator shared library be available in the
// PATH (Windows) or LD_LIBRARY_PATH (Linux).
//
// This class connects to an ActiveMQ broker and registers
// one or more MessageConsumers to receive messages from
// a specified queue.  The MessageConsumers can
// process requests for GETMETA, PROCESSCAS and Collection
// Process Complete.
//
// On initialization, each MessageConsumer instance creatss
// an AnalysisEngine and a CAS based on the input AE descriptor.
// By default, one instance of a MessageConsumer is created. 
// 
// The service wrapper sets acknowledgment mode to AUTO_ACKNOWLEDGE mode 
// by default and lets the underlying middleware handle the message
// acknowledgements.
//
// If given a port number, the service will establish a socket connection
// to a server on the local machine and route logging and JMX statistics 
// over the socket connection.
//
// See the UIMA-EE documentation for how to start and manage a C++
// servcice from Java using the UimacppServiceController bean.
//------------------------------------------------------------------------
class AMQAnalysisEngineService : public ExceptionListener,
	public MessageListener  {
private:
	string queueName;          
	int count;					
	int prefetchsize;
	map<int, AMQListener *> listeners;    //id - listener
	vector <AMQConnection*> consumerConnections; 
	Monitor * pStatistics;
  
  void initialize(string  aeDescriptor, 
                  size_t initialFsHeapSize,
                  string  brokerURL, 
                  string  queueName,
		              int prefetch, 
                  int numInstances); 
public:
   void cleanup();
  
   ~AMQAnalysisEngineService(); 

	AMQAnalysisEngineService(ServiceParameters & desc, Monitor * pStatistics);

	void setTraceLevel(int level);

	void onException( const CMSException& ex );
		
	virtual void onMessage( const Message* message );
	
	void start();
		
	int stop(); 	  
};




#endif

