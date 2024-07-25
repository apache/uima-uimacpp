/** \file flow_controller.cpp .
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

   Description: Implementation of FixedFlowController and FixedFlowObject

----------------------------------------------------------------------------- */


#include "uima/flow_controller.hpp"

namespace uima {
  Step FixedFlowObject::next() {
    if (wasPassedToCASMultiplier) {
      switch (flowController->getAction()) {
        case FixedFlowController::ActionAfterCasMultiplier::CONTINUE:
          break;
        case FixedFlowController::ActionAfterCasMultiplier::STOP:
          return Step(internal::FinalStep());
        case FixedFlowController::ActionAfterCasMultiplier::DROP:
          return Step(internal::FinalStep(internallyCreatedCAS));
        case FixedFlowController::ActionAfterCasMultiplier::DROP_IF_NEW_CAS_PRODUCED:
          if (newCASProduced)
            return Step(internal::FinalStep(internallyCreatedCAS));
          break;
      }
      wasPassedToCASMultiplier = newCASProduced = false;
    }

    const std::vector<icu::UnicodeString>& delegateKeys = flowController->getDelegateKeys();
    if (currentStep >= delegateKeys.size())
      return Step(internal::FinalStep());

    const icu::UnicodeString &engineName = delegateKeys[currentStep];
    const AnnotatorContext* engineContext = flowController->getDelegateSpecifierMap().at(engineName);
    const AnalysisEngineMetaData* engineMetadata = engineContext->getTaeSpecifier().getAnalysisEngineMetaData();
    const OperationalProperties* operationalProps = engineMetadata->getOperationalProperties();
    if ( operationalProps && operationalProps->getOutputsNewCASes())
      wasPassedToCASMultiplier = true;

    return Step(internal::SimpleStep(delegateKeys[currentStep++]));
  }

  std::unique_ptr<Flow> FixedFlowObject::newCasProduced(const CAS &cas, const icu::UnicodeString &producedBy) {
    newCASProduced = true;
    const std::vector<icu::UnicodeString>& delegateKeys = flowController->getDelegateKeys();

    int i = 0;
    while (producedBy != delegateKeys.at(i))
      ++i;
    return std::make_unique<FixedFlowObject>(flowController, i+1, true);
  }

/* -------------------------------------------------------------------------------------------------------------- */
/*                                 FixedFlowController implementation                                             */
/* -------------------------------------------------------------------------------------------------------------- */
  void FixedFlowController::initialize(const AnnotatorContext &anContext) {
    annotatorContext = &anContext;
    delegateSpecifierMap = &anContext.getDelegates();
    flowContraints = anContext.getTaeSpecifier().getAnalysisEngineMetaData()->getFixedFlow();
  }

  void FixedFlowController::destroy() {
  }

  void FixedFlowController::reconfigure() {
    if (annotatorContext)
      initialize(*annotatorContext);
  }

  std::unique_ptr<Flow> FixedFlowController::computeFlow(CAS&) {
    return std::make_unique<FixedFlowObject>(this, 0);
  }

  const std::vector<icu::UnicodeString> & FixedFlowController::getDelegateKeys() const {
    return flowContraints->getNodes();
  }

  const std::map<icu::UnicodeString, AnnotatorContext *> & FixedFlowController::getDelegateSpecifierMap() const {
    return *delegateSpecifierMap;
  }

  FixedFlowController::ActionAfterCasMultiplier FixedFlowController::getAction() const {
    return action;
  }

}
