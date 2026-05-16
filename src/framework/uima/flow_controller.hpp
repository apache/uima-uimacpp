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

   Description: This file contains the FlowController class and Flow interface
   that control the flow of CASes inside an Aggregate Analysis Engine

-----------------------------------------------------------------------------

   7/18/2024: created
-------------------------------------------------------------------------- */

#include "uima/annotator_context.hpp"
#include "uima/flow.hpp"
#include "uima/pragmas.hpp"
#include "uima/engine.hpp"

namespace uima {
  class FixedFlowController;

  /**
   * A <TT>FlowController</TT> dictates how CASes are routed within Aggregate Analysis Engines.
   * <p>
   * For each new CAS that is passed to the Aggregate Analysis Engine containing the FlowController,
   * FlowController::computeFlow will be called. This method must return a Flow object that is responsible for
   * routing that CAS through the components of the Aggregate Analysis Engine.
   * <p>
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

    /** Deinitialize this Flow Controller     */
    virtual void destroy()=0;

    /** Reconfigure this Flow Controller      */
    virtual void reconfigure()=0;

    /**
     * Computes and returns a Flow object will the input CAS through the Aggregate. The
     * <code>Flow</code> object should be given a handle to the CAS, so that it can use information in
     * the CAS to make routing decisions.
     * FlowController implementations can define their own class that implements Flow.
     * @param cas A CAS that this FlowController should process.
     * @return a Flow object that has responsibility for routing <code>cas</code> through the
     *         Aggregate Analysis Engine.
     */
    virtual std::unique_ptr<Flow> computeFlow(CAS &cas)=0;
  };


  /**
   * This class represents the Flow object used in a Fixed Flow Controller
   */
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
    /** The Flow Controller that defines this Flow */
    FixedFlowController* flowController;

    /** Index of the delegate sequence this Flow is at*/
    int currentStep;

    /** Whether this flow's CAS was passed to a CAS Multiplier*/
    bool wasPassedToCASMultiplier;

    /** Whether this flow's CAS has produced a new CAS */
    bool newCASProduced;

    /** Whether this flow's CAS was produced internally by a CAS Multiplier */
    bool internallyCreatedCAS;
  };


  class UIMA_LINK_IMPORTSPEC FixedFlowController : public FlowController {
  public:
    enum class ActionAfterCasMultiplier { CONTINUE, STOP, DROP, DROP_IF_NEW_CAS_PRODUCED };

    FixedFlowController() : delegateSpecifierMap(), flowContraints(), annotatorContext(),
                            action(ActionAfterCasMultiplier::DROP_IF_NEW_CAS_PRODUCED) {
    }

    void initialize(const AnnotatorContext &anContext) override;

    void destroy() override;

    void reconfigure() override;

    std::unique_ptr<Flow> computeFlow(CAS &) override;

    const std::vector<icu::UnicodeString>& getDelegateKeys() const;

    const std::map<icu::UnicodeString, AnnotatorContext*>& getDelegateSpecifierMap() const;

    ActionAfterCasMultiplier getAction() const;

  private:
    /** Maps from delegate engine keys (not names) to their corresponding AnnotatorContexts */
    const std::map<icu::UnicodeString, AnnotatorContext*>* delegateSpecifierMap;

    /** The FlowContraints object that defines this FlowController*/
    const FixedFlow* flowContraints;

    /** The AnnotatorContext of the aggregate engine that owns this FlowController */
    const AnnotatorContext* annotatorContext;

    /** The action to be taken after a CAS has been input to a CAS Multiplier. For now this cannot be overridden yet.\n
     * Values include:\n
     * - CONTINUE: the CAS will continue with the flow\n
     * - STOP: the CAS will not continue with the flow and be returned\n
     * - DROP: the CAS will not continue and be dropped\n
     * - DROP_IF_NEW_CAS_PRODUCED (default): If the CAS Multiplier produced a new CAS from this input CAS then this CAS will
     * be dropped, otherwise it will continue.
     */
    ActionAfterCasMultiplier action;
  };

}

#endif //UIMA_FLOW_CONTROLLER_HPP
