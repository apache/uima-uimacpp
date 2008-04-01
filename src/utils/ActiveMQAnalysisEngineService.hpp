/*
* Analysis Engine service wrapper based on 
* Active MQ C++ client.
*/

#ifndef __ACTIVEMQ_AE_SERVICE__
#define __ACTIVEMQ_AE_SERVICE__
#include "uima/api.hpp"

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/ExceptionListener.h>

using namespace cms;
using namespace uima;

//Forward declarations
class Monitor;
class ServiceParameters;


//==========================================================
// This class wraps a ActiveMQ JMS connection 
// and provides utility methods to create a MessageProducer
// and MessageConsumer and to send and receive messages.
//----------------------------------------------------------
class AMQConnection : public ExceptionListener {
private:
  string iv_brokerURL;
  Connection* iv_pConnection;
  bool iv_valid;

  //consumer session
  Session* iv_pConsumerSession;
	MessageConsumer * iv_pConsumer;
  string iv_inputQueueName;
  cms::Queue * iv_pInputQueue;      
  MessageListener * iv_pListener;
	
  //producer session
  Session * iv_pProducerSession;
  MessageProducer * iv_pProducer;  
	TextMessage * iv_pTextMessage;
  map<string, cms::Destination*> iv_replyDestinations; //queuename-destination

public:
	/** Establish connection to the broker and create a Message Producer session. 
   */
	AMQConnection ( const char * aBrokerURL);

  /** Creates a MessageConsumer session and registers a listener. 
      Caller owns the listener. */
	void createMessageConsumer(string aQueueName, MessageListener * pListener, int prefetch); 
 	
  /** destructor */
	~AMQConnection();

	/** caller owns the ExceptionListener */
	void setExceptionListener(ExceptionListener  * el);

	/** This also is a default Exception handler */
	void onException( const CMSException& ex);

	/** If this is consumer, must be called to start receiving messages */ 
	void start();

	/** If this is a consumer, stops receiving messages. */
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

  /** get the brokerURL */
  string getBrokerURL() {
    return this->iv_brokerURL;
  }
  
  /** get the input queue name */
  string getInputQueueName() {
    return this->iv_inputQueueName;
  }
};


//==================================================
// This class is used to cache and reuse connections
// used to send reply messages.
//--------------------------------------------------
class AMQConnectionsCache  {
private:
	map<string, AMQConnection *> iv_connections; //key is brokerurl
	
public:
	AMQConnectionsCache(); 

	~AMQConnectionsCache(); 

  /**
	 * Retrieves a Connection from the cache if it 
	 * exists or establishes a new connection to the
	 * the specified broker and adds it to the cache.
   */
	AMQConnection * getConnection(string brokerURL); 
	
};

//======================================================
// A MessageListener that handles getMeta, processCAS
// and Collection Processing Complete requests.
//
// Records timing and error JMX statistics.
//
//------------------------------------------------------
class AMQListener : public MessageListener {
private:
	int iv_id;									    //Listener id
	string iv_inputQueueName;				//queue this listener gets messages from
	string iv_brokerURL;				    //broker this listener is connected to 
  AMQConnection * iv_pConnection; //connection this Listener is registered with.
                                  //Used to send replies to queues on the same broker.
	AnalysisEngine * iv_pEngine;	  //AnalysisEngine
	CAS * iv_pCas;								  //CAS
	string iv_aeDescriptor;			    //AE descriptor XML 
	int iv_count;                   //num messages processed
	bool iv_busy;								    //is processing a message
  apr_time_t iv_timeLastRequestCompleted; //used to calculate idle time between requests
  Monitor * iv_pStatistics;       //JMX statistics
	AMQConnectionsCache    iv_replyToConnections; //maintain connections cache for
                                                //sending reply to other brokers.
	
  void sendResponse(const TextMessage * request, apr_time_t timeToDeserialize,
                    apr_time_t timeToSerialize, apr_time_t timeInAnalytic,
                    apr_time_t idleTime, apr_time_t elapsedTime, 
		                string msgContent, bool isExceptionMsg); 
public:
	/** constructor */ 
  AMQListener(int id,
              AMQConnection * pConnection,
              AnalysisEngine * pEngine,
              CAS * pCas,
              Monitor * pStatistics); 
  /** destructor */
	~AMQListener(); 

	bool isBusy() {
		return this->iv_busy;
	}

  /*
	* Process the message. Handles GETMETA, PROCESSCAS
  * and CPC requests.
	*/
	virtual void onMessage( const Message* message ); 
	
};

//===================================================
// This class creates and configures a C++ service
// according to parameters passed in as arguments.
//
// This class creates connections to an ActiveMQ broker 
// and registers one or more MessageConsumers to receive 
// messages from a specified queue.  The MessageConsumers can
// process requests for GETMETA, PROCESSCAS and Collection
// Process Complete. 
//
// Each MessageConsumer registers a MessageListener that will
// receive and process messages. Each instance of the MessageListener
// is initialized with an instance of the AnalysisEngine and a CAS.
// 
// The service wrapper sets acknowledgment mode to AUTO_ACKNOWLEDGE mode 
// by default and lets the underlying middleware handle the message
// acknowledgements.
//
// The lifecycle of the service may be managed by the UIMA AS Java 
// controller bean org.apache.uima.controller.UimacppServiceController.
// In this case, the C++ service is started by the controller bean.
// A socket connection is established between the controller bean and
// the C++ process and used to route logging messages and JMX statistics.
//
// See the UIMA-EE documentation for how to start and manage a C++
// servcice from Java using the UimacppServiceController bean.
//------------------------------------------------------------------------
class AMQAnalysisEngineService : public ExceptionListener,
	                               public MessageListener  {
private:
  string iv_brokerURL;  
	string iv_inputQueueName;  
  string iv_aeDescriptor;
	int iv_numInstances;					
	int iv_prefetchSize;
  size_t iv_initialFSHeapSize;
  Monitor * iv_pStatistics;

	vector <AMQConnection*> iv_vecpConnections;
  vector <AnalysisEngine *> iv_vecpAnalysisEngines;
  vector <CAS *> iv_vecpCas;
  map<int, AMQListener *> iv_listeners;    //id - listener

  void initialize(); 
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



