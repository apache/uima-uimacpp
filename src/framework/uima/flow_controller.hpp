#ifndef UIMA_FLOW_CONTROLLER_HPP
#define UIMA_FLOW_CONTROLLER_HPP

/** \file flow_controller.hpp .
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

   Description:

-----------------------------------------------------------------------------


-------------------------------------------------------------------------- */

#include "uima/annotator_context.hpp"
#include "uima/flow.hpp"
#include "uima/pragmas.hpp"
#include "uima/engine.hpp"

namespace uima {
  class FixedFlowController;

  /**
   * A <TT>FlowController</TT> dictates how CASes are routed within Aggregate Analysis Engines.
   */
  class UIMA_LINK_IMPORTSPEC FlowController {
  public:
    virtual ~FlowController() = default;

    /**
     * Initializes this Flow Controller and any related members.
     * @param anContext
     *    Description of the AnalysisEngine that this Flow Controller belongs to.
     */
    virtual void initialize(const AnnotatorContext& anContext)=0;
    virtual void destroy()=0;
    virtual void reconfigure()=0;
    virtual std::unique_ptr<Flow> computeFlow(const CAS&)=0;
  };

  class UIMA_LINK_IMPORTSPEC FixedFlowObject : public Flow {
  public:
    FixedFlowObject(FixedFlowController *const flowController, int startStep,
                    bool internallyCreatedCAS = false) : flowController(flowController), currentStep(startStep),
                                                         wasPassedToCASMultiplier(false),
                                                         newCASProduced(false),
                                                         internallyCreatedCAS(internallyCreatedCAS) {
      assert(EXISTS(flowController));
    }

    Step next() override;

    std::unique_ptr<Flow> newCasProduced(const CAS& cas, const icu::UnicodeString& producedBy) override;

  private:
    FixedFlowController* flowController;
    int currentStep;
    bool wasPassedToCASMultiplier;
    bool newCASProduced;
    bool internallyCreatedCAS;
  };


  class UIMA_LINK_IMPORTSPEC FixedFlowController : public FlowController {
  public:
    enum class ActionAfterCasMultiplier { CONTINUE, STOP, DROP, DROP_IF_NEW_CAS_PRODUCED };

    FixedFlowController() = default;

    void initialize(const AnnotatorContext &anContext) override;

    void destroy() override;

    void reconfigure() override;

    std::unique_ptr<Flow> computeFlow(const CAS&) override;

    const std::vector<icu::UnicodeString>& getDelegateKeys() const;

    const std::map<icu::UnicodeString, AnnotatorContext*>& getDelegateSpecifierMap() const;

    ActionAfterCasMultiplier getAction() const;

  private:
    // FIXME: Use std::map<icu::UnicodeString, AnalysisEngineDescription*> instead??
    const std::map<icu::UnicodeString, AnnotatorContext*>* delegateSpecifierMap{};
    const FixedFlow* flowContraints{};
    const AnnotatorContext* annotatorContext{};
    ActionAfterCasMultiplier action{ActionAfterCasMultiplier::DROP_IF_NEW_CAS_PRODUCED};
  };

}

#endif //UIMA_FLOW_CONTROLLER_HPP
