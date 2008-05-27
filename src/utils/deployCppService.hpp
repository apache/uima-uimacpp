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

* This file contains code common to the ActiveMQ and WebSphere MQ
* implementations of the UIMA C++ service wrapper.
*/

#ifndef __DEPLOY_SERVICE__
#define __DEPLOY_SERVICE__
#include "uima/api.hpp"
#include <apr_network_io.h>
#include <apr_proc_mutex.h>
#include <apr_errno.h>
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
#define CPC_COMMAND         2002
#define REQUEST             3000
#define RESPONSE            3001
#define XMI_PAYLOAD         1000
#define XCAS_PAYLOAD        1004
#define META_PAYLOAD        1002
#define EXC_PAYLOAD         1003
#define NO_PAYLOAD          1005

static char * getmeta_selector = "Command = 2001";
static char * annotator_selector = "Command = 2000 or Command = 2002";

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
          iv_user(), iv_password(), iv_initialfsheapsize(0),
          iv_wpmEndpoints("localhost:7276:BootstrapBasicMessaging"), 
          iv_wpmBusname() {}

  
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
                 << " WAS bootstrap service address " << iv_wpmEndpoints
                 << " WAS SIB name " << iv_wpmBusname
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

    const string & getWPMEndpoints() {
			return iv_wpmEndpoints;
		}

    const string & getWPMBusname() {
			return iv_wpmBusname;
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
       } else if (0 == strcmp(arg, "-wash")) {
				 if (++index < argc) {
				 		this->iv_wpmEndpoints = argv[index];
         }
       } else if (0 == strcmp(arg, "-wasb")) {
				 if (++index < argc) {
				 		this->iv_wpmBusname = argv[index];
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
     //This is a temporary fix to enable the service wrapper to 
     //connect to the WAS messaging engine.
     if (this->iv_mqHost.find_first_of(":") != string::npos) {
        this->iv_wpmEndpoints = iv_mqHost;
        this->iv_mqHost = "";
        this->iv_wpmBusname = this->iv_mqQueueMgr;
        this->iv_mqQueueMgr = "";
     }
	}	

  string getServiceName() {
    if (getWPMBusname().length() == 0) {
      stringstream str;
      str << getMQPort();
      return getQueueName() + "@" + getMQHost() + ":" + str.str();
     } else {
      return getQueueName() + "@" + getWPMBusname();
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
    //Websphere default messaging
    string iv_wpmEndpoints;
    string iv_wpmBusname;
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
                int processCasErrorWindow, bool terminateOnCPCError)         
    {
       iv_brokerURL = brokerURL;
       iv_queueName = queueName;
       iv_aeDescriptor = aeDesc;
       iv_numInstances = numInstances;
       iv_prefetchSize = 0;
       iv_cpcErrors = 0;
       iv_getmetaErrors = 0;
       iv_processcasErrors = 0;
       iv_deserTime = 0;
       iv_serTime = 0;
       iv_annotatorTime = 0;
       iv_startTime = apr_time_now();
       iv_numCasProcessed = 0;
       iv_errorThreshhold = processCasErrorThreshhold;
       iv_errorWindow = processCasErrorWindow;
       iv_terminateOnCPCError = terminateOnCPCError;
       iv_getmetaId = -1;
       iv_numRunning = 0;
       mutex = 0;
       lmutex = 0;
       cond = 0;
       cond_mutex = 0;
       apr_status_t rv = apr_thread_mutex_create(&mutex,APR_THREAD_MUTEX_UNNESTED, pool);
       apr_thread_mutex_create(&cond_mutex,APR_THREAD_MUTEX_UNNESTED, pool);
       apr_thread_mutex_create(&lmutex,APR_THREAD_MUTEX_UNNESTED, pool);
       apr_thread_cond_create(&cond, pool);
    }

		void print();
		
		void shutdown() {
      //apr_thread_mutex_unlock(mutex);
      //  apr_thread_mutex_unlock(lmutex);
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
      iv_numRunning = num;
		}

    void listenerStopped(int id) {
      cout << "listener stopped " << id << endl;
      if (id == iv_getmetaId || iv_numRunning == 1) {
        shutdown();
      } else {
        apr_thread_mutex_lock(mutex);
        iv_numRunning--;
        apr_thread_mutex_unlock(mutex);
      }
    }

    void  setGetMetaListenerId(int id) {
      iv_getmetaId = id;
    }

    void setStartTime() {
	    apr_thread_mutex_lock(mutex);
      iv_startTime = apr_time_now();
	    apr_thread_mutex_unlock(mutex);
    }

    void logMessage(string message) {
       apr_thread_mutex_lock(lmutex);
       uima::ResourceManager::getInstance().getLogger().logMessage(message);
       apr_thread_mutex_unlock(lmutex);
    }

    void logWarning(string message) {
       apr_thread_mutex_lock(lmutex);
       uima::ResourceManager::getInstance().getLogger().logWarning(message);
       apr_thread_mutex_unlock(lmutex);
    }
    void logError(string message) {
       apr_thread_mutex_lock(lmutex);
       uima::ResourceManager::getInstance().getLogger().logError(message,-99);
       apr_thread_mutex_unlock(lmutex);
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
    apr_thread_mutex_t *lmutex; 
		string iv_brokerURL;
		string iv_queueName;
    string iv_aeDescriptor;
		int iv_numInstances;
    int iv_prefetchSize;
    long iv_cpcErrors;
    long iv_getmetaErrors;
    long iv_processcasErrors;
    int iv_getmetaId;
    int iv_numRunning;

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
                cerr << " number of PROCESSCAS errors exceeded the threshhold. Terminating the service." << endl;
                shutdown();
           } else {
             //TODO sliding window
         
           }
        }
      } else if (command == GET_META_COMMAND) {
        iv_getmetaErrors++;
        cerr << "getMeta Error terminating the service." << endl;
        shutdown();
      } else if (command == CPC_COMMAND) {
        iv_cpcErrors++;
        if (this->iv_terminateOnCPCError) {
          cerr << "CPC Error terminating the service." << endl;
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


 static SocketLogger * singleton_pLogger =0;
 static Monitor * singleton_pMonitor=0;
 //static ServiceParameters serviceDesc;


 //static apr_pool_t *pool=0;
 static apr_socket_t *s=0;  //logger socket
 static apr_socket_t *cs=0; //receive commands socket
 static apr_sockaddr_t *sa=0;
 static apr_status_t rv=0;

 ///static apr_thread_t *thread=0;
 ///static apr_threadattr_t *thd_attr=0;

  static void signal_handler(int signum) {
    stringstream str;
    str << __FILE__ << __LINE__ << " Received Signal: " << signum;
    cerr << str.str() << endl;
    singleton_pMonitor->shutdown();
  }


 static int terminateService() {
   
   cout << "deployCppService::terminateService" << endl;

   if (cs) {
     apr_socket_close(cs);
     cs=0;
   }
  
   if (singleton_pMonitor) {
     delete singleton_pMonitor;
     singleton_pMonitor=0;
   }
  
   if (singleton_pLogger) {
     uima::ResourceManager::getInstance().unregisterLogger(singleton_pLogger);
      delete singleton_pLogger;
      singleton_pLogger =0;
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
    singleton_pMonitor = new Monitor(pool,
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

      rv = apr_socket_create(&s, sa->family, SOCK_STREAM, APR_PROTO_TCP, pool);
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
      rv = apr_socket_create(&cs, sa->family, SOCK_STREAM, APR_PROTO_TCP, pool);
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
      singleton_pLogger = new SocketLogger(s,pool);
      if (singleton_pLogger == NULL) {
        cerr << "ERROR: SocketLogger() failed. " << endl;
        return -7;
      }
      uima::ResourceManager::getInstance().registerLogger(singleton_pLogger);
      uima::ResourceManager::getInstance().getLogger().logMessage("deployCppService::initialize() Registered Java Logger.",0);
    } 
    //singleton_pMonitor->logMessage("deployCppService::initialize() done");
    return 0;
 } //initialize


static void* APR_THREAD_FUNC handleCommands(apr_thread_t *thd, void *data) {

      //if we are here service initialization was successful, send message
      //to controller.
      string msg = "0";
      apr_size_t len = msg.length();
      rv = apr_socket_send(cs, msg.c_str(), &len);
      len = 1;
      apr_socket_send(cs,"\n", &len);
      //receive JMX, admin requests from controller 
      char buf[9];
      memset(buf,0,9);
      len = 8;
      while ( (rv = apr_socket_recv(cs, buf, &len)) != APR_EOF) {
        string command = buf;
        memset(buf,0,9);
        len=8;
        //cout << "apr_socket_recv command=" << command << endl;
        if (command.compare("GETSTATS")==0) {
          //singleton_pLogger->log(LogStream::EnMessage,"deployCppService","getStats","retrieving stats",0);
          singleton_pMonitor->writeStatistics(cs);
          //singleton_pLogger->log(LogStream::EnMessage,"deployCppService","getStats","sent statistics",0); 
        } else if (command.compare("RESET")==0) {
          singleton_pLogger->log(uima::LogStream::EnMessage,"deployCppService", "RESET",
            "reset JMX statistics",0);
          singleton_pMonitor->reset();
        } else if (command.compare("SHUTDOWN")==0) {
          singleton_pMonitor->shutdown();
          break;
        } else {
          if (rv != APR_SUCCESS) {
            singleton_pMonitor->shutdown();
            break;
          } else {
            char * c = new char[256];
            apr_strerror(rv, c, 255);
            stringstream str;
            str << c;
            str << "deployCppService::handleCommand() aprerror=" << rv << " invalid command=" << command;
            cerr << str.str() << endl;
            delete c;
          }
         }
      }
      apr_thread_exit(thd, APR_SUCCESS);
      cout << "deployCppService::handleCommand() calling shutdown. " << endl;
      singleton_pMonitor->shutdown();
      return NULL;
 }


 



#endif




