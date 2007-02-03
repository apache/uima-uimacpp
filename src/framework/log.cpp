/** @name log.cpp
-----------------------------------------------------------------------------

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

-----------------------------------------------------------------------------

   Description: This file contains class LogFacility

-----------------------------------------------------------------------------


   6/24/1999   Initial creation
   7/19/1999   logError(const ErrorInfo & crclErrorInfo) added

-------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/*       Include dependencies                                              */
/* ----------------------------------------------------------------------- */

#include <time.h>
#include "uima/log.hpp"

#include "uima/trace.hpp"
#include "uima/comp_ids.h"
#include "uima/msg.h"
#include "uima/resmgr.hpp"
#include "uima/exceptions.hpp"

/* ----------------------------------------------------------------------- */
/*       Constants                                                         */
/* ----------------------------------------------------------------------- */

#define UIMA_LOG_APPLICATION_KEY_UNKNOWN      _TEXT("???")
const size_t                  UIMA_LOG_STATIC_CONVERSION_BUFSIZE = 1024;

/* ----------------------------------------------------------------------- */
/*       Forward declarations                                              */
/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/*       Types / Classes                                                   */
/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */
/*       Private Implementation                                            */
/* ----------------------------------------------------------------------- */
namespace uima {

  TyMessageId LogFacility::getTypeAsMessageId(LogStream::EnEntryType enType) const
  /* ----------------------------------------------------------------------- */
  {
    switch (enType) {
    case LogStream::EnMessage :
      return(UIMA_MSG_ID_LOG_MESSAGE);
    case LogStream::EnWarning :
      return(UIMA_MSG_ID_LOG_WARNING);
    case LogStream::EnError   :
      return(UIMA_MSG_ID_LOG_ERROR);
    default:
      assertWithMsg(false, _TEXT("Unknow EnLogEntryType"));  //lint !e506: Constant value Boolean
    }
    return(0);                                         /* shutup compiler */
  }

  void LogFacility::doLog(LogStream::EnEntryType enType, const TCHAR * cpszMsg, long lUserCode) const
  /* ----------------------------------------------------------------------- */
  {
    if (isLoggable(enType)) {
      if (ResourceManager::hasInstance() ) {
        ResourceManager & resmgr = ResourceManager::getInstance();
        if ( resmgr.isJavaLoggingEnabled()) {
          //route to java logger if one is available
          resmgr.writeToJavaLogger(enType, iv_strOrigin,
                                   cpszMsg);
        } else if (iv_logFile != NULL) {
          //write to local log file if one is available
          string message = format(enType, cpszMsg, lUserCode);
          fwrite(message.c_str(), 1, message.size(), iv_logFile);
          fflush(iv_logFile);
        }
      }
    }

  }

  std::string  LogFacility::format(LogStream::EnEntryType enType, const TCHAR * cpszMsg, long lUserCode) const {

    //if (isLoggable(enType)) {
    /***char dateStr [9];
    char timeStr [9];
    _strdate( dateStr);
    _strtime( timeStr );
       
    message += dateStr;
    message += " ";
    message += timeStr;
    message += " ";
    message += iv_strOrigin.c_str();
    message += " ";
             ***/

    time_t rawtime;
    time ( &rawtime );
    string currts = ctime(&rawtime);
    string message = currts.substr(0,currts.length()-1);

    //map enType to Java levels
    switch (enType) {
    case LogStream::EnMessage :
      message += (" INFO: ");
      break;
    case LogStream::EnWarning :
      message += " WARNING: ";
      break;
    case LogStream::EnError :
      message += " SEVERE: ";
      break;
    default:
      message += " INFO: ";
    }

    message += cpszMsg;
    message += "\n";
    //}
    return message;
  }

  bool LogFacility::isLoggable(LogStream::EnEntryType enType) const {
    ///LogStream::EnEntryType minLogLevel = ResourceManager::getInstance().getLoggingLevel();
    if (enType <  iv_logLevel) {
      return false;
    }
    return true;
  }

  void LogFacility::setLogLevel(LogStream::EnEntryType enType)  {
    ///LogStream::EnEntryType minLogLevel = ResourceManager::getInstance().getLoggingLevel();
    iv_logLevel = enType;
  }

  void LogFacility::log(LogStream::EnEntryType enType, const TCHAR * cpszMsg, long lUserCode) const
  /* ----------------------------------------------------------------------- */
  {
    doLog(enType, cpszMsg, lUserCode);

    if (enType == LogStream::EnError) {
      LogFacility * pclThis = CONST_CAST(LogFacility *, this);
      assert(pclThis == this);
      //remember last error as string/number
      pclThis->iv_lLastUserCode = lUserCode;
      pclThis->iv_strLastError  = string(cpszMsg);
      //reset error info object
      pclThis->iv_errInfo.reset();
    }
  }


  void  LogFacility::log(LogStream::EnEntryType enType, ErrorInfo const & errInfo) const
  /* ----------------------------------------------------------------------- */
  {
    doLog(enType, errInfo.asString().c_str());

    LogFacility * pclThis = CONST_CAST(LogFacility *, this);
    assert(pclThis == this);
    //remember last error as error info object
    pclThis->iv_errInfo = errInfo;
    //reset string/number
    // MSV6 compile error: pclThis->iv_strLastError.clear();
    pclThis->iv_strLastError = std::string();
    pclThis->iv_lLastUserCode = 0;
  }


  void LogFacility::log(LogStream::EnEntryType enType, const icu::UnicodeString & ustrMsg, long lUserCode) const
  /* ----------------------------------------------------------------------- */
  {
    string strMsg;
    UnicodeStringRef ref(ustrMsg);
    ref.extract(strMsg, CCSID::getDefaultName());
    log(enType, strMsg.c_str(), lUserCode);
  }

  /* ----------------------------------------------------------------------- */
  /*       Implementation                                                    */
  /* ----------------------------------------------------------------------- */

  LogStream & LogStream::flush() {
    this->stringstream::flush();
    iv_pLogFacility->flushLogStream();
    str("");
    return (*this);
  }

  LogStream & LogFacility::getLogStream(LogStream::EnEntryType enLogEntryType) {
    iv_logStream.iv_enLogEntryType = enLogEntryType;
    flushLogStream();
    return iv_logStream;
  }

  void LogFacility::flushLogStream() {
    string strMsg = iv_logStream.str();
    if (strMsg.size() == 0) {
      return;
    }
    log(iv_logStream.iv_enLogEntryType, strMsg.c_str());
  }


  LogFacility::LogFacility(icu::UnicodeString const & crEngineName):
      iv_lLastUserCode(0),
      iv_logStream(*this, LogStream::EnMessage),
      iv_logFile(NULL),
      iv_logLevel(LogStream::EnMessage) {
    UnicodeStringRef ref(crEngineName);
    ref.extract(iv_strOrigin, CCSID::getDefaultName()  );
    iv_logFile = uima::ResourceManager::getInstance().getLogFile();
    iv_logLevel = uima::ResourceManager::getInstance().getLoggingLevel();
  }

  LogFacility::LogFacility(icu::UnicodeString const & crEngineName,
                           FILE * crpLogFile,
                           LogStream::EnEntryType crLoggingLevel):
      iv_lLastUserCode(0),
      iv_logStream(*this, LogStream::EnMessage),
      iv_logFile(crpLogFile),
      iv_logLevel(crLoggingLevel) {
    UnicodeStringRef ref(crEngineName);
    ref.extract(iv_strOrigin, CCSID::getDefaultName() );
  }

  LogFacility::~LogFacility() {
    flushLogStream();
  }

  const TCHAR * LogFacility::getLastErrorAsCStr(void) const {
    /* ----------------------------------------------------------------------- */
    LogFacility * pclThis = CONST_CAST(LogFacility *, this);
    ErrorInfo errInfo = getLastError();
    if (errInfo.getErrorId() == UIMA_ERR_NONE) {
      return (NULL);
    }
    pclThis->iv_strLastError = getLastError().asString();
    return(iv_strLastError.c_str());
  }

  ErrorInfo const & LogFacility::getLastError() const {
    /* ----------------------------------------------------------------------- */
    // check if we have a string or info based information on last error
    if (iv_strLastError.length() > 0) {
      // if we have error string the error object must be clear
      assert(iv_errInfo.getErrorId() == UIMA_ERR_NONE);
      // now set up the object with the string information
      LogFacility * pclThis = CONST_CAST(LogFacility *, this);
      assert(pclThis == this);
      if (iv_lLastUserCode == 0) {
        pclThis->iv_errInfo.setErrorId(UIMA_ERR_USER_ANNOTATOR_ERROR_UNSPECIFIED);
      } else {
        pclThis->iv_errInfo.setErrorId(iv_lLastUserCode);
      }
      ErrorMessage msg(UIMA_MSG_ID_LOG_TO_ERROR_INFO, iv_strOrigin);
      msg.addParam(iv_strLastError);
      pclThis->iv_errInfo.setMessage(msg);
      pclThis->iv_errInfo.setSeverity(ErrorInfo::unrecoverable);
      // now that the information is transferred into the object clear string
      pclThis->iv_lLastUserCode = 0;
      // MSV6 compile error:
      //pclThis->iv_strLastError.clear();
      pclThis->iv_strLastError = std::string();
    }
    assert(iv_strLastError.length() == 0);
    assert(iv_lLastUserCode == 0);
    return iv_errInfo;
  }


  void LogFacility::logWarning(const ErrorInfo & errInfo) const
  /* ----------------------------------------------------------------------- */
  {
    log(LogStream::EnWarning, errInfo);
  }


  void LogFacility::logError(const ErrorInfo & errInfo) const
  /* ----------------------------------------------------------------------- */
  {
    log(LogStream::EnError, errInfo);
  }

} //namespace uima
/* <EOF> */

