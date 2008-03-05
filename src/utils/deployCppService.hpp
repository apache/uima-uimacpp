/*
* This file contains code common to the ActiveMQ and WebSphere MQ
* implementations of the UIMA C++ service wrapper.
*/

#ifndef __DEPLOY_SERVICE__
#define __DEPLOY_SERVICE__

#include <apr_network_io.h>
#include <apr_proc_mutex.h>
#include <apr_errno.h>
#include "uima/api.hpp"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <vector>

#include <apr_general.h>
#include <apr_thread_proc.h>
#include <apr_thread_cond.h>
#include <apr_signal.h>

class SocketLogger;
class Monitor;
class UimacppService;
class ServiceParameters;


/*
 * Constants
 */
#define PROCESS_CAS_COMMAND 2000
#define GET_META_COMMAND    2001
#define CPC_COMMAND     2002
#define REQUEST             3000
#define RESPONSE            3001
#define XMI_PAYLOAD         1000
#define XCAS_PAYLOAD        1004
#define META_PAYLOAD        1002
#define EXC_PAYLOAD         1003
#define NO_PAYLOAD          1005




static int initialize(ServiceParameters &, apr_pool_t*);
 static void* APR_THREAD_FUNC handleCommands(apr_thread_t *thd, void *data);
 static int terminateService();
 static void signal_handler(int signum);


/**
 * This class holds command line parameters that 
 * configure this service.
 */

class ServiceParameters {
	public:
		//Constructor

		ServiceParameters() :
					iv_aeDescriptor(), iv_brokerURL("tcp://localhost:61616"),
          iv_queueName(), iv_numInstances(1),
					iv_prefetchSize(1), iv_javaport(0), 
          iv_datapath(), iv_loglevel(0), iv_tracelevel(-1),
          iv_errThreshhold(0), iv_errWindow(0), iv_terminateOnCPCError(false),
          iv_mqHost("localhost"), iv_mqPort(1414), iv_mqChannel(), iv_mqQueueMgr(),
          iv_user(), iv_password(), iv_initialfsheapsize(0) {
    }

  
		~ServiceParameters() {

		}

   
		void print() {
		  cout << asString() << endl;
	  }	


    string ServiceParameters::asString() {
      stringstream str;
      str << "AE descriptor " <<  iv_aeDescriptor 
         << " Initial FSHeap size " << this->iv_initialfsheapsize
                << " Input Queue " << iv_queueName
                 << " Num Instances " << iv_numInstances 
                 << " Prefetch size " << iv_prefetchSize 
	         << " ActiveMQ broker URL " << iv_brokerURL 
                 << " MQ host " << iv_mqHost
                 << " MQ port " << iv_mqPort
                 << " MQ qmgr " << iv_mqQueueMgr
                 << " MQ channel " << iv_mqChannel
                 << " Java port " << iv_javaport 
                 << " logging level " << iv_loglevel 
                 << " trace level " << iv_tracelevel 
                 << " datapath " << iv_datapath << endl;
     return str.str();
    }
		
		const string & getBrokerURL() const {
			return iv_brokerURL;
		}
		
		const string & getQueueName() const {
			return iv_queueName;
		}

		const string & getAEDescriptor() const {
					return iv_aeDescriptor;
		}
		
		const int getTraceLevel() const {
			return iv_tracelevel;
		}
		const int getLoggingLevel() const {
					return iv_loglevel;
		}
		const int getNumberOfInstances() const {
			return iv_numInstances;
		}
		const int getPrefetchSize() const {
			return iv_prefetchSize;
		}
		const int getJavaPort() const {
		  return iv_javaport;
		}
		
		const string & getDataPath() {
			return iv_datapath;
		}

    const int getErrorThreshhold() {
			return iv_errThreshhold;
		}

    const int getErrorWindow() {
			return iv_errWindow;
		}
    const bool terminatOnCPCError() {
			return iv_terminateOnCPCError;
		}

		const string & getMQHost() {
			return iv_mqHost;
		}

    const int getMQPort() {
			return iv_mqPort;
		}

    const string & getMQChannel() {
			return iv_mqChannel;
		}

    const string & getMQQueueMgr() {
			return iv_mqQueueMgr;
		}

    const string & getUserName() {
			return iv_user;
		}

    const string & getPassword() {
			return iv_password;
		}

    const size_t getInitialFSHeapSize() {
      return iv_initialfsheapsize;
    }

		void setBrokerURL(const string url) {
			iv_brokerURL = url;
		}
		
		void setQueueName(const string qname) {
			iv_queueName = qname;
		}
		
		void setAEDescriptor(const string loc) {
					iv_aeDescriptor = loc;
	  }
		
		void setNumberOfInstances(int num) {
			iv_numInstances=num;
		}

		void setTraceLevel(int num) {
			iv_tracelevel=num;
		}

		void setLoggingLevel(int num) {
					iv_loglevel=num;
		}
		void setJavaPort(int val) {
			iv_javaport = val;
		}
		
		void setDataPath(const string path) {
			iv_datapath=path;
		}

		void parseArgs(int argc, char* argv[]) {
		 int index =0;
		 while (++index < argc) {
			 char * arg = argv[index];
			 if ( 0 == strcmp(arg, "-b") ) {
				  if (++index < argc) {
				  	this->iv_brokerURL = argv[index];
				  }
			 } else if (0 == strcmp(arg, "-n")) {
				 if (++index < argc) {
				 		this->iv_numInstances = atoi(argv[index]);
				 }
       } else if (0 == strcmp(arg, "-fsheapsz")) {
				 if (++index < argc) {
				 		this->iv_initialfsheapsize = atoi(argv[index]);
            this->iv_initialfsheapsize = this->iv_initialfsheapsize/4;
				 }
       } else if (0 == strcmp(arg, "-p")) {
				 if (++index < argc) {
				 		this->iv_prefetchSize = atoi(argv[index]);
				 }
			 } else if (0 == strcmp(arg, "-t")) {
				 if (++index < argc) {
				 		this->iv_tracelevel = atoi(argv[index]);
				 }
			 } else if (0 == strcmp(arg, "-l")) {
			 	 if (++index < argc) {
			 			this->iv_loglevel = atoi(argv[index]);
			   }
			 } else if (0 == strcmp(arg, "-jport")) {
				 if (++index < argc) {
					  this->iv_javaport = atoi(argv[index]);
				 }
       } else if (0 == strcmp(arg, "-d")) {
				 if (++index < argc) {
				 		this->iv_datapath = argv[index];
				 }
       } else if (0 == strcmp(arg, "-e")) {
				 if (++index < argc) {
				 		this->iv_errThreshhold = atoi(argv[index]);
				 }
       }  else if (0 == strcmp(arg, "-w")) {
				 if (++index < argc) {
				 		this->iv_errWindow = atoi(argv[index]);
				 }
       } else if (0 == strcmp(arg, "-mqh")) {
				 if (++index < argc) {
				 		this->iv_mqHost = argv[index];
         }
       } else if (0 == strcmp(arg, "-mqp")) {
				 if (++index < argc) {
				 		this->iv_mqPort = atoi(argv[index]);
         }
       } else if (0 == strcmp(arg, "-mqc")) {
				 if (++index < argc) {
				 		this->iv_mqChannel = argv[index];
         }
       } else if (0 == strcmp(arg, "-mqm")) {
				 if (++index < argc) {
				 		this->iv_mqQueueMgr = argv[index];
         }
       } else if (0 == strcmp(arg, "-user")) {
				 if (++index < argc) {
				 		this->iv_user = argv[index];
         }
       } else if (0 == strcmp(arg, "-pw")) {
				 if (++index < argc) {
				 		this->iv_password = argv[index];
         }
       } else if (0 == strcmp(arg, "-a")) {
				 if (++index < argc) {
            if (stricmp(argv[index],"true")==0) {
				 		  this->iv_terminateOnCPCError = true;
            } else {
              this->iv_terminateOnCPCError=false;
            }
				 }
       } else {
				 if (this->iv_aeDescriptor.length() == 0) {
					 this->iv_aeDescriptor = argv[index];
				 } else {
					 if (this->iv_queueName.length() == 0) {
					   this->iv_queueName = argv[index];
					 } else {
						 cerr << "unexpected argument " << argv[index] << endl;
					 }
				 }
			 }
		 }
	}
	
		
	private:
	
		string iv_aeDescriptor;

    //ActiveMQ 
		string iv_brokerURL;

		string iv_queueName;
		int iv_numInstances;
		int iv_prefetchSize;
    int iv_javaport;
    string iv_datapath;
		int iv_loglevel;
    int iv_tracelevel;
    int iv_errThreshhold;
    int iv_errWindow;
    bool iv_terminateOnCPCError;
    //Websphere MQ
    string iv_mqHost;       
    int iv_mqPort;
    string iv_mqChannel;
    string iv_mqQueueMgr;
    string iv_user;
    string iv_password;
    size_t iv_initialfsheapsize;
};

/**
 * Command line parameters required to start this service.
 */

class Monitor {
	public:
		//Constructor

		Monitor (apr_pool_t * pool, string brokerURL,
                string queueName, string aeDesc,
                int numInstances, int prefetchSize, int processCasErrorThreshhold,
                int processCasErrorWindow, bool terminateOnCPCError) :
        iv_brokerURL(brokerURL), iv_queueName(queueName),
					iv_aeDescriptor(aeDesc),  iv_numInstances(numInstances), iv_prefetchSize(0),
					iv_cpcErrors(0),
          iv_getmetaErrors(0), iv_processcasErrors(0),
          iv_deserTime(0), iv_serTime(0), iv_annotatorTime(0),
          iv_messageProcessTime(0), iv_cpcTime(0), iv_getmetaTime(0),
          iv_startTime(apr_time_now()), 
          iv_numCasProcessed(0), 
          iv_errorThreshhold(processCasErrorThreshhold),
          iv_errorWindow(processCasErrorWindow),
          iv_terminateOnCPCError(terminateOnCPCError),
          mutex(0), cond(0), cond_mutex(0)
          
    {
       apr_status_t rv = apr_thread_mutex_create(&mutex,APR_THREAD_MUTEX_UNNESTED, pool);
       apr_thread_mutex_create(&cond_mutex,APR_THREAD_MUTEX_UNNESTED, pool);
       apr_thread_cond_create(&cond, pool);
    }

		~Monitor() {

		}

		void print();
		
		void shutdown() {
      apr_thread_cond_signal(this->cond);
      return ;
    }
  
		const string & getBrokerURL() const {
			return iv_brokerURL;
		}
		
		const string & getQueueName() const {
			return iv_queueName;
		}

		const string & getAEDescriptor() const {
					return iv_aeDescriptor;
		}

    const int getNumberOfInstances() const {
					return iv_numInstances;
		}
		
		void setBrokerURL(const string url) {
			iv_brokerURL = url;
		}
		
		void setQueueName(const string qname) {
			iv_queueName = qname;
		}
		
		void setAEDescriptor(const string loc) {
					iv_aeDescriptor = loc;
	  }
		
		void setNumberOfInstances(int num) {
			iv_numInstances=num;
		}

    void setStartTime() {
	    apr_thread_mutex_lock(mutex);
      iv_startTime = apr_time_now();
	    apr_thread_mutex_unlock(mutex);
    }

    void processingComplete(int command, bool success, apr_time_t totalTime,
                                 apr_time_t deserTime=0,
                                 apr_time_t analyticTime=0,
                                 apr_time_t serTime=0) {		  
      apr_thread_mutex_lock(mutex);
      iv_messageProcessTime += totalTime;
      if (command == PROCESS_CAS_COMMAND) {
        iv_numCasProcessed++;
        iv_deserTime += deserTime;
        iv_annotatorTime += analyticTime;
        iv_serTime += serTime;
      } else if (command == GET_META_COMMAND) {
        iv_getmetaTime += totalTime;
      } else if (command == CPC_COMMAND) {
        iv_cpcTime += totalTime;
      } 

      if (!success) {
        incrementErrorCount(command);
      }
	    apr_thread_mutex_unlock(mutex);    
    }
  
    void writeStatistics(apr_socket_t * cs) {
		  apr_thread_mutex_lock(mutex);
       stringstream str;
       apr_time_t totalProcessTime = (apr_time_now() - iv_startTime)*iv_numInstances;

       // idle time in millis 
       long idleTime = (totalProcessTime-iv_messageProcessTime)/1000;
       str << "NUMINSTANCES=" << iv_numInstances
           << " CPCERRORS=" << iv_cpcErrors
           << " GETMETAERRORS=" << iv_getmetaErrors
           << " PROCESSCASERRORS=" << iv_processcasErrors
           << " CPCTIME=" << iv_cpcTime/1000
           << " GETMETATIME=" << iv_getmetaTime/1000
           << " NUMCASPROCESSED=" << iv_numCasProcessed
           << " SERIALIZETIME=" << iv_serTime/1000 
           << " DESERIALIZETIME=" << iv_deserTime/1000
           << " ANNOTATORTIME=" << iv_annotatorTime/1000
           << " MESSAGEPROCESSTIME=" << iv_messageProcessTime/1000
           << " IDLETIME=" << idleTime << endl;

       apr_size_t len = str.str().length();
       apr_status_t rv = apr_socket_send(cs, str.str().c_str(), &len);
       len = 1;
       apr_socket_send(cs,"\n", &len);
       if (rv != APR_SUCCESS) {
            //TODO throw exception
            cout << "apr_socket_send() failed " << endl;
       }
          
       apr_thread_mutex_unlock(mutex);
    }

    void reset() {
		  apr_thread_mutex_lock(mutex);
       
       // idle time in millis 
       this->iv_numCasProcessed=0;
       this->iv_annotatorTime=0;
       this->iv_deserTime=0;
       this->iv_serTime=0;     
       this->iv_cpcTime=0;
       this->iv_getmetaTime=0;
       this->iv_messageProcessTime=0;

       this->iv_startTime=apr_time_now();

       this->iv_cpcErrors=0;
       this->iv_getmetaErrors=0;
       this->iv_processcasErrors=0;
 
	     apr_thread_mutex_unlock(mutex);
    }
	
    void  printStatistics();

public:
    apr_thread_mutex_t *cond_mutex;
    apr_thread_cond_t  *cond;
private:
	  apr_thread_mutex_t *mutex;
		string iv_brokerURL;
		string iv_queueName;
    string iv_aeDescriptor;
		int iv_numInstances;
    int iv_prefetchSize;
    long iv_cpcErrors;
    long iv_getmetaErrors;
    long iv_processcasErrors;

    apr_time_t iv_deserTime;
    apr_time_t iv_serTime;
    apr_time_t iv_annotatorTime;
    apr_time_t iv_messageProcessTime;
    apr_time_t iv_cpcTime;
    apr_time_t iv_getmetaTime;
    apr_time_t iv_startTime;
     
    long iv_numCasProcessed; 
    int iv_errorThreshhold;
    int iv_errorWindow;
    bool iv_terminateOnCPCError;


    void incrementErrorCount(int command) {
		  
      if (command == PROCESS_CAS_COMMAND) {
        iv_processcasErrors++;
        if (this->iv_errorThreshhold > 0 ) {
           if ( this->iv_errorWindow == 0 && iv_processcasErrors >=
              this->iv_errorThreshhold) {
                uima::ResourceManager::getInstance().getLogger().logMessage(
                "number of PROCESSCAS errors exceeded the threshhold. Terminating the service.");
                shutdown();
           } else {
              //TODO sliding error window
           }
        }
      } else if (command == GET_META_COMMAND) {
        iv_getmetaErrors++;
        uima::ResourceManager::getInstance().getLogger().logMessage(
               "getMeta Error terminating the service.");
         shutdown();
      } else if (command == CPC_COMMAND) {
        iv_cpcErrors++;
        if (this->iv_terminateOnCPCError) {
          uima::ResourceManager::getInstance().getLogger().logMessage(
               "CPC Error terminating the service.");
          shutdown();
        }
      } 
	   
    }
    
};


/* SocketLogger */
class SocketLogger : public uima::Logger {
  private:
    apr_socket_t * socket;
    apr_thread_mutex_t *mutex;

  public:
    SocketLogger(apr_socket_t * sock, apr_pool_t * pool) : socket(sock) {
      apr_thread_mutex_create(&mutex,APR_THREAD_MUTEX_UNNESTED, pool);
    }

    virtual void log(uima::LogStream::EnEntryType entrytype,
					string classname,
          string methodname,
					string message,
          long lUserCode) {
      //cout << "SocketLogger::log() " << message << endl;
      apr_thread_mutex_lock(mutex);
       
      apr_status_t rv;
      stringstream str;
      str << entrytype << " " << classname << " " << methodname;
      if (entrytype == uima::LogStream::EnMessage) {
        if (lUserCode != 0) {
          str << " RC=" << lUserCode;
        }
      } else {
        str << " RC=" << lUserCode;
      } 
      str << " " << message << endl;

      apr_size_t len = str.str().length();
      rv = apr_socket_send(socket, str.str().c_str(), &len);
  
      if (rv != APR_SUCCESS) {
        //TODO throw exception
        cerr << "apr_socket_send() failed " << endl;
      }
      apr_thread_mutex_unlock(mutex);
    }

};


 static SocketLogger * logger =0;
 static Monitor * pMonitor=0;
 //static ServiceParameters serviceDesc;


 //static apr_pool_t *pool=0;
 static apr_socket_t *s=0;  //logger socket
 static apr_socket_t *cs=0; //receive commands socket
 static apr_sockaddr_t *sa=0;
 static apr_status_t rv=0;

 ///static apr_thread_t *thread=0;
 ///static apr_threadattr_t *thd_attr=0;

  static void signal_handler(int signum) {
    pMonitor->shutdown();
  }


 static int terminateService() {
   
   uima::ResourceManager::getInstance().getLogger().logMessage("deployCppService::terminateService");

   if (cs) {
     apr_socket_close(cs);
     cs=0;
   }
  
   if (pMonitor) {
     delete pMonitor;
     pMonitor=0;
   }
  
   if (logger) {
     uima::ResourceManager::getInstance().unregisterLogger(logger);
      delete logger;
      logger =0;
   }

   if (s) {
    apr_socket_close(s);
    s=0;
   }
   return 0;
 }


 static int initialize(ServiceParameters & serviceDesc,  apr_pool_t * pool) {
    
    if (serviceDesc.getLoggingLevel() == 0) {
      uima::ResourceManager::getInstance().setLoggingLevel(uima::LogStream::EnMessage);
    } else if (serviceDesc.getLoggingLevel() == 1) {
      uima::ResourceManager::getInstance().setLoggingLevel(uima::LogStream::EnWarning);
    } else if (serviceDesc.getLoggingLevel() == 2) {
      uima::ResourceManager::getInstance().setLoggingLevel(uima::LogStream::EnError);
    }

    /*use only first path that exists */
    if (serviceDesc.getDataPath().length() > 0) {
      uima::util::Filename dataPath("");
      dataPath.determinePath(serviceDesc.getDataPath().c_str());
      uima::util::Location dataLocation(dataPath.getAsCString());
      uima::ResourceManager::getInstance().setNewLocationData(dataLocation);
    }

    //register signal handler
    apr_signal(SIGINT, signal_handler);
    apr_signal(SIGTERM, signal_handler);

      /* create object to collect JMX stats */
    pMonitor = new Monitor(pool,
      serviceDesc.getBrokerURL(),serviceDesc.getQueueName(),
      serviceDesc.getAEDescriptor(), serviceDesc.getNumberOfInstances(),
      serviceDesc.getPrefetchSize(),
      serviceDesc.getErrorThreshhold(),
      serviceDesc.getErrorWindow(),
      serviceDesc.terminatOnCPCError());

     
    /* set up connection to Java controller bean if a port is specified */
    int javaport=serviceDesc.getJavaPort();
    if (javaport > 0) {
      //cout << "connecting to java controller port " << javaport << endl;
      rv = apr_sockaddr_info_get(&sa, "localhost", APR_INET, javaport, 0, pool);   
      if (rv != APR_SUCCESS) {
        cerr << "ERROR: apr_sockaddr_info_get localhost at port " << javaport << endl;
        return -2;
      }

      rv = apr_socket_create(&s, sa->family, SOCK_STREAM, pool);
      if (rv != APR_SUCCESS) {
        cerr << "ERROR: apr_socket_create() logger connection at port " << javaport << endl;
        return -3;
      }


      rv = apr_socket_connect(s, sa);
      if (rv != APR_SUCCESS) {
        cerr << "ERROR: apr_socket_connect() logger connection at  port " << javaport << endl;
        return -4;
      }

      //commands connection
      rv = apr_socket_create(&cs, sa->family, SOCK_STREAM, pool);
      if (rv != APR_SUCCESS) {
        cerr << "ERROR: apr_socket_create() commands connection at port " <<  javaport << endl;
        return -5;
      }

      rv = apr_socket_connect(cs, sa);
      if (rv != APR_SUCCESS) {
        cerr << "ERROR: apr_socket_connect() commands connection at port " <<  javaport << endl;
        return -6;
      }

      //register SocketLogger
      logger = new SocketLogger(s,pool);
      if (logger == NULL) {
        cerr << "ERROR: SocketLogger() failed. " << endl;
        return -7;
      }
      uima::ResourceManager::getInstance().registerLogger(logger);
      uima::ResourceManager::getInstance().getLogger().logMessage("deployCppService::initialize() Registered Java Logger.",0);
    } 
    uima::ResourceManager::getInstance().getLogger().logMessage("deployCppService::initalize() done");
    return 0;
 } //initialize


static void* APR_THREAD_FUNC handleCommands(apr_thread_t *thd, void *data) {
   char buf[9];
      memset(buf,0,9);
      apr_size_t len = 8;

      //Monitor * pMonitor = (Monitor*) data;
      while ( (rv = apr_socket_recv(cs, buf, &len)) != APR_EOF) {
        string command = buf;
        memset(buf,0,9);
        len=8;
        //cout << "apr_socket_recv command=" << command << endl;
        if (command.compare("GETSTATS")==0) {
          //logger->log(LogStream::EnMessage,"deployCppService","getStats","retrieving stats",0);
          pMonitor->writeStatistics(cs);
          //logger->log(LogStream::EnMessage,"deployCppService","getStats","sent statistics",0); 
        } else if (command.compare("RESET")==0) {
          logger->log(uima::LogStream::EnMessage,"deployCppService", "RESET",
            "reset JMX statistics",0);
          pMonitor->reset();
        } else if (command.compare("SHUTDOWN")==0) {
          pMonitor->shutdown();
          break;
        } else {
          if (rv != APR_SUCCESS) {
            pMonitor->shutdown();
            break;
          } else {
            char * c = new char[256];
            apr_strerror(rv, c, 255);
            stringstream str;
            str << c;
            str << "deployCppService::handleCommand() aprerror=" << rv << " invalid command=" << command;
            uima::ResourceManager::getInstance().getLogger().logError(str.str());
            delete c;
          }
         }
      }
      apr_thread_exit(thd, APR_SUCCESS);
      uima::ResourceManager::getInstance().getLogger().logError("deployCppService::handleCommand() calling shutdown. ");
      pMonitor->shutdown();
      return NULL;
 }


 



#endif

