/**************************************************************************/
/*  Licensed Materials - Property of IBM                                  */
/*  (C) Copyright IBM Corporation 2003, 2004.                             */
/*  All rights reserved.                                                  */		
/*                                                                        */
/* US Government Users Restricted Rights - Use, duplication or            */
/* disclosure restricted by GSA ADP Schedule Contract with                */
/* IBM Corporation.                                                       */
/**************************************************************************/
/*  Permission Notice                                                     */
/*                                                                        */
/*  Permission is granted to copy, use, modify, and merge this sample     */
/*  software into your applications and to permit others to do any of the */
/*  foregoing. You may further distribute this software for               */
/*  commercial purposes only as part of your application that adds        */
/*  significant value and function beyond that provided by these          */
/*  samples.                                                              */
/*  You must include this permission statement and retain the copyright   */
/*  notice in all copies and modified versions of this software.          */
/*                                                                        */
/**************************************************************************/
/*                                                                        */
/*  DISCLAIMER OF WARRANTIES                                              */
/*                                                                        */
/*  The sample software is provided to you by IBM to assist you in        */
/*  developing your applications. THIS SOFTWARE IS PROVIDED AS-IS,        */
/*  WITHOUT WARRANTY OF ANY KIND. IBM SHALL NOT BE LIABLE FOR ANY         */
/*  DAMAGES ARISING OUT OF YOUR USE OR THE USE BY ANY THIRD PARTY OF      */
/*  OF THE SAMPLE SOFTWARE EVEN IF IT HAS BEEN ADVISED OF THE POSSIBILITY */
/*  OF SUCH DAMAGES. IN ADDITION, IBM SHALL NOT BE LIABLE FOR ANY THIRD   */
/*  PARTY CLAIMS AGAINST YOU.                                             */
/*                                                                        */
/**************************************************************************/

#include "uima/api.hpp"


using namespace uima;

 
class TextSegmentConsumer : public Annotator {
private:
  
  int numProcessed; 
  int numSegments;

  


public:

  TextSegmentConsumer(void)
  {
    cout << "TextSegmentConsumer: Constructor" << endl;
  }

  ~TextSegmentConsumer(void)
  {
    cout << "TextSegmentConsumer: Destructor" << endl;
  }
  
  /** */
  TyErrorId initialize(AnnotatorContext & rclAnnotatorContext)
  {
    cout << "TextSegmentConsumer: initialize()" << endl;
    numProcessed=0;
	numSegments=0;
    if (rclAnnotatorContext.isParameterDefined("TotalNumberOfSegments") &&
		rclAnnotatorContext.extractValue("TotalNumberOfSegments", numSegments) == UIMA_ERR_NONE)  {
	    if (numSegments < 1) {
 			UIMA_EXC_THROW_NEW(Exception,
	                  UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT,
                      UIMA_MSG_ID_EXCON_CONFIG_VALUE_EXTRACT,
                      ErrorMessage(UIMA_MSG_ID_LITERAL_STRING, "Invalid value for 'TotalNumberOfSegments'"),
                      ErrorInfo::unrecoverable);
        }    
     } else {
	   numSegments=3;	
	 }
	 cout << "TextSegmentConsumer::initialize() .. Total number of segments expected: " 
	             << numSegments << endl;    	
    return (TyErrorId)UIMA_ERR_NONE;
  }


  TyErrorId typeSystemInit(TypeSystem const & crTypeSystem) {
    cout << "TextSegmentConsumer: typeSystemInit()" << endl;
    return(TyErrorId)UIMA_ERR_NONE;
  }

  /** */
  TyErrorId destroy()
  {
    cout << "TextSegmentConsumer: destroy()" << endl;
    return (TyErrorId)UIMA_ERR_NONE;
  }

  
  TyErrorId process(CAS & rCAS, const ResultSpecification& spec)
  {
   cout << "TextSegmentConsumer: process() begins" << endl;

   if (rCAS.getDocumentText().length() > 0) {
         cout << "TextSegmentConsumer::process() Got a text segment " << rCAS.getDocumentText() << endl;
	   numProcessed++;
   } else {
	   UIMA_EXC_THROW_NEW(Exception,
		   UIMA_ERR_USER_ANNOTATOR_COULD_NOT_PROCESS,
		   UIMA_MSG_ID_EXCON_CONFIG_VALUE_EXTRACT,
		   ErrorMessage(UIMA_MSG_ID_LITERAL_STRING, "getDocumentText() returned string of zero length."),
		   ErrorInfo::unrecoverable);
   }

   cout << "TextSegmentConsumer: process() ends" << endl;
   return (TyErrorId)UIMA_ERR_NONE;
  }

  TyErrorId batchProcessComplete()
  {
  cout << "TextSegmentConsumer: batchProcessComplete()" << endl;
  return (TyErrorId)UIMA_ERR_NONE;
  }

  TyErrorId collectionProcessComplete()
  {
  cout << "TextSegmentConsumer: collectionProcessComplete() num processed: " << numProcessed << endl;

  if (numProcessed != numSegments) {
	  UIMA_EXC_THROW_NEW(Exception,
		   UIMA_ERR_USER_ANNOTATOR_COULD_NOT_PROCESS,
		   UIMA_MSG_ID_EXCON_CONFIG_VALUE_EXTRACT,
		   ErrorMessage(UIMA_MSG_ID_LITERAL_STRING, "getDocumentText() returned string of zero length."),
		   ErrorInfo::unrecoverable);
  }
  
  return (TyErrorId)UIMA_ERR_NONE;
  }


};

// This macro exports an entry point that is used to create the annotator.

MAKE_AE(TextSegmentConsumer);
