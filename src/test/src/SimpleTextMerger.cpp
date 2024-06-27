/** @name SimpleTextMerger.cpp

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


-------------------------------------------------------------------------- */


#include "uima/api.hpp"
#include <iostream>

using namespace uima;
using namespace std;

class SimpleTextMerger : public Annotator {
  CAS *pCas{nullptr};
  AnnotatorContext* pAnc;

  icu::UnicodeString usMergedDoc{};
  bool readyToOutput{false};

  vector<icu::UnicodeString> usAnnotationT;
  Type srcDocInfo;
  Feature lastSegment;

  TyErrorId getConfigValues() {
    return UIMA_ERR_NONE;
  }

public:

  SimpleTextMerger() { cout << "SimpleTextMerger: constructor" << endl; }
  ~SimpleTextMerger() { cout << "SimpleTextMerger: destructor" << endl; }

  int getCasInstancesRequired() override {
    return 1;
  }

  TyErrorId initialize(AnnotatorContext &rclAnnotatorContext) override {
    cout << "SimpleTextMerger: initialize" << endl;

    pAnc = &rclAnnotatorContext;
    if (rclAnnotatorContext.isParameterDefined("AnnotationTypesToCopy") &&
      rclAnnotatorContext.extractValue("AnnotationTypesToCopy", usAnnotationT) == UIMA_ERR_NONE) {

      cout << "Copying types: " << endl;
      for (auto const & type : usAnnotationT) {
        cout << '\t' << type << endl;
      }
    } else {
      pAnc->getLogger().logError("initialize: Cannot get AnnotationTypesToCopy");
      UIMA_EXC_THROW_NEW(Exception,
                   UIMA_ERR_USER_ANNOTATOR_COULD_NOT_INIT,
                   UIMA_MSG_ID_EXCON_CONFIG_VALUE_EXTRACT,
                   ErrorMessage(UIMA_MSG_ID_LITERAL_STRING, "Invalid value for AnnotationTypesToCopy"),
                   ErrorInfo::unrecoverable);
    }

    return UIMA_ERR_NONE;
  }

  TyErrorId typeSystemInit(TypeSystem const & crTypeSystem) override {
    srcDocInfo = crTypeSystem.getType("uima.tt.SourceDocumentInformation");
    lastSegment = srcDocInfo.getFeatureByBaseName("lastSegment");
    return UIMA_ERR_NONE;
  }

  TyErrorId process(CAS &cas, ResultSpecification const &crResultSpecification) override {
    if (!pCas)
      pCas = &pAnc->getEmptyCAS();

    int initialLen = usMergedDoc.length();
    UnicodeStringRef docText = cas.getDocumentText();
    usMergedDoc.append(docText.getBuffer(), docText.length());

    FSIndexRepository& indexRep = pCas->getIndexRepository();

    for (const auto & copiedType : usAnnotationT) {
      Type type = pCas->getTypeSystem().getType(copiedType);
      if (type == srcDocInfo)
        continue;

      ANIterator iter = cas.getAnnotationIndex(type).iterator();

      while (iter.isValid()) {
        AnnotationFS anFS = iter.get();
        // !! This does not copy all Features of the AnnotationFS, unlike Java's CasCopier
        // C++ does not have an equivalent ASAIK
        // So we just make a bare Annotation
        AnnotationFS copy = pCas->createAnnotation(type,
          anFS.getBeginPosition() + initialLen, anFS.getEndPosition() + initialLen);
        indexRep.addFS(copy);
        iter.moveToNext();
      }
    }

    ANIterator it = cas.getAnnotationIndex(srcDocInfo).iterator();
    if (!it.isValid()) {
      UIMA_EXC_THROW_NEW(Exception,
                   UIMA_ERR_USER_ANNOTATOR_COULD_NOT_PROCESS,
                   UIMA_MSG_ID_EXC_INVALID_ITERATOR,
                   ErrorMessage(UIMA_MSG_ID_LITERAL_STRING, "Invalid Iterator: No SourceDocumentInformation found"),
                   ErrorInfo::unrecoverable);
    }

    AnnotationFS info = it.get();
    if (info.getBooleanValue(lastSegment)) {
      pCas->setDocumentText(usMergedDoc);
      AnnotationFS sdi = pCas->createAnnotation(srcDocInfo, 0, usMergedDoc.length());
      sdi.setBooleanValue(lastSegment, true);
      indexRep.addFS(sdi);

      usMergedDoc = icu::UnicodeString();
      readyToOutput = true;
    }
    return UIMA_ERR_NONE;
  }

  bool hasNext() override {
    return readyToOutput;
  }

  CAS &next() override {
    if (!readyToOutput) {
      UIMA_EXC_THROW_NEW(Exception,
                         UIMA_ERR_USER_ANNOTATOR_COULD_NOT_PROCESS,
                         UIMA_MSG_ID_EXCON_PROCESSING_CAS,
                         ErrorMessage(UIMA_MSG_ID_LITERAL_STRING, "There is not next() available."),
                         ErrorInfo::unrecoverable);
    }

    CAS &ret = *pCas;
    pCas = nullptr;
    readyToOutput = false;
    return ret;
  }
};

// This macro exports an entry point that is used to create the annotator.

MAKE_AE(SimpleTextMerger);
