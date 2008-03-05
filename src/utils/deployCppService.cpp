/*
* Analysis Engine service wrapper implementation based on 
* Active MQ C++ client.
*/

#include "ActiveMQAnalysisEngineService.hpp"
#include "deployCppService.hpp"
using namespace activemq::exceptions;

int shutdown_service;
//===================================================
//Main
//---------------------------------------------------

void printUsage() {
  cout << "Usage: deployCppService aeDescriptor queueName " << endl <<
    "       <-b brokerURL [defaults to tcp://localhost:61616]> " << endl <<   
    "       <-n numInstance [defaults to 1]> " << endl << 
    "       <-p prefetchSize [defaults to1]> " << endl <<
    "       <-l loglevel [defaults to 0 INFO]> " << endl <<
    "       <-t tracelevel [defaults to 0 and valid to 4]> " << endl << 
    "       <-d datapath [defaults to not set] " << endl ;
  cout << "Example: deployCppService DaveDetector.xml DAVEDETECTORQ -b tcp://localhost:61616 -n 1 -p 1 -l 0 -t 0 -d c:/mydata" << endl;        
}

int main(int argc, char* argv[]) {
  cout << "=====================================================\n";    
  cout << "Starting the UIMA C++ Remote Service using ActiveMQ broker." << std::endl;
  cout << "-----------------------------------------------------\n";
  // _CrtSetBreakAlloc(1988);

  try {
    if (argc < 3) {
      printUsage();
      return(-1);
    }

    /* create ResourceManager */
    if (!uima::ResourceManager::hasInstance()) {
      uima::ResourceManager::createInstance("ActiveMQAnalysisEngineService");
    }

    /* APR pool */
    apr_status_t rv;
    apr_pool_t *pool;
    rv = apr_pool_create(&pool, NULL);
    if (rv != APR_SUCCESS) {
      cerr << "ERROR: apr_pool_create() failed. " << endl;
      return -1;
    }


    /*parse descriptor */
    ServiceParameters serviceDesc;
    serviceDesc.parseArgs(argc, argv);
    //serviceDesc.print();

    /*set up logging and monitoring */
    initialize(serviceDesc, pool);   

    /*create service*/
    AMQAnalysisEngineService aeService(serviceDesc,pMonitor);
    aeService.setTraceLevel(serviceDesc.getTraceLevel());

    /*start receiving messages*/ 
    cout << "Start receiving messages " << endl;
    aeService.start();

    cout << "UIMA C++ Service " << serviceDesc.getQueueName() << " at " <<
      serviceDesc.getBrokerURL() << " Ready to process..." << endl;
    
    /* connect to java proxy if called from java */
    int javaport=serviceDesc.getJavaPort();   
    apr_thread_t *thread=0;
    apr_threadattr_t *thd_attr=0;
    if (javaport > 0) {
     
      apr_threadattr_create(&thd_attr, pool);
      rv = apr_thread_create(&thread, thd_attr, handleCommands, 0, pool);
      assert(rv == APR_SUCCESS);
      //rv = apr_thread_join(&rv, thread);
      //assert(rv == APR_SUCCESS);
    }

    //wait 
    apr_thread_mutex_lock(pMonitor->cond_mutex);
    apr_thread_cond_wait(pMonitor->cond, pMonitor->cond_mutex);
    apr_thread_mutex_unlock(pMonitor->cond_mutex);   
  
    /* shutdown */
    uima::ResourceManager::getInstance().getLogger().logMessage("deployCppService shutting down.");
   
    aeService.stop();
    terminateService();
    
    if (pool) {
      apr_pool_destroy(pool);
      pool=0;
    }
  }  catch (CMSException& e) {
    cerr << __FILE__ << __LINE__ << " " << e.getMessage() << endl;
    e.printStackTrace();
  } catch (XMLException& e) {
    cerr << __FILE__ << __LINE__ << " " << e.getMessage() << endl;
  } catch (uima::Exception e) {
    cerr << __FILE__ << __LINE__ << " " << e.asString() << endl;
  } catch (...) {
    cerr << __FILE__ << __LINE__ << " Unknown Exception" << endl;
  }
  cout << "-----------------------------------------------------\n";    
  cout << " UIMA C++ Remote Service terminated:\n";
  cout << "=====================================================\n";    


}  //main

