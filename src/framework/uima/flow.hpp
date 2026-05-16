#ifndef UIMA_FLOW_HPP
#define UIMA_FLOW_HPP

/** \file flow.hpp .
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

   Description: Flow interface for the FlowController

-----------------------------------------------------------------------------


-------------------------------------------------------------------------- */
#include <memory>

#include "uima/pragmas.hpp"
#include "uima/annotator.hpp"

namespace uima {
  namespace internal {
    /**
     * Indicates that a CAS should be routed to a single AnalysisEngine.
     */
    class UIMA_LINK_IMPORTSPEC SimpleStep {
      /* The key of the engine the CAS will be input to.
       Not to be confused with the engine name, which is specified by the its descriptor*/
      icu::UnicodeString engineKey;
      /* ResultSpecification *resultSpec; */
    public:
      /* If SimpleStepWithResultSpec is required when CapabilityLanguageFlowController is implemented,
       * this class could have an extra member ResultSpecification
      SimpleStep(const icu::UnicodeString &name, ResultSpecification *resultSpec) : engineKey(name) {
      } */

      SimpleStep(const icu::UnicodeString &name) : engineKey(name) {
      }

      const icu::UnicodeString &getEngineName() const {
        return engineKey;
      }

      /**
      bool hasResultSpec() const {
        return resultSpec != nullptr;
      }

      ResultSpecification* getResultSpec() const {
        return resultSpec;
      }
      */
    };

    /**
     * Not yet implemented
     */
    class UIMA_LINK_IMPORTSPEC ParallelStep {
    public:
      ParallelStep() {
        UIMA_EXC_THROW_NEW(NotYetImplementedException,
                           UIMA_ERR_NOT_YET_IMPLEMENTED,
                           UIMA_MSG_ID_EXC_NOT_YET_IMPLEMENTED,
                           ErrorMessage(UIMA_MSG_ID_EXCON_UNKNOWN_CONTEXT),
                           ErrorInfo::unrecoverable
        );
      }
    };

    /* Indicates that a CAS has finished being processed by the aggregate.
     */
    class UIMA_LINK_IMPORTSPEC FinalStep {
      /* Whether the CAS should be dropped. Should only be true for CASes produced internally by the aggregate.
       */
      bool forceDropCAS;

    public:
      FinalStep() : forceDropCAS(false) {
      }

      explicit FinalStep(bool forceDropCAS) : forceDropCAS(forceDropCAS) {
      }

      bool getForceDropCAS() const {
        return forceDropCAS;
      }
    };
  }


  /**
   * Class <TT>Step</TT> indicates where to route the current CAS to next.
   * It is a union type of possible step types: Simple Step, Parallel Step and Final Step.
   * It is returned using <code>internal::Flow::next</code>
   */
  class UIMA_LINK_IMPORTSPEC Step {
  public:
    enum class StepType { SIMPLESTEP, FINALSTEP, PARALLELSTEP, UNSPECIFIED };

    Step() : type(StepType::UNSPECIFIED) {
    }

    explicit Step(const internal::SimpleStep &simpleStep);

    explicit Step(const internal::ParallelStep &parallelStep);

    explicit Step(const internal::FinalStep &finalStep);

    Step(const Step &other);

    Step& operator=(const Step& other);

    ~Step();

    const internal::SimpleStep *getSimpleStep() const;

    const internal::ParallelStep *getParallelStep() const;

    const internal::FinalStep *getFinalStep() const;

    StepType getType() const;
  private:
    union step_union {
      internal::SimpleStep simpleStep;
      internal::ParallelStep parallelStep;
      internal::FinalStep finalStep;

      step_union(const internal::SimpleStep &simpleStep) : simpleStep(simpleStep) {
      }

      step_union(const internal::ParallelStep &parallelStep) : parallelStep(parallelStep) {
      }

      step_union(const internal::FinalStep &finalStep) : finalStep(finalStep) {
      }
      step_union() { }
      ~step_union() { }
    } uStep;

    StepType type;
  };


  /**
   * Base class for the Flow objects computed by the FlowController.
   * Flow objects are responsible for routing a CAS through an Aggregate Engine by returning a <TT>Step</TT>
   * @see FlowController::computeFlow
   */
  class UIMA_LINK_IMPORTSPEC Flow {
    /** The CAS that this Flow object is handling. The Flow object can choose to use this method or not */
    CAS* inputCAS{nullptr};
  public:
    virtual ~Flow(){};

    /**
     * Specify the next destination for the CAS via a Step object
     */
    virtual Step next()=0;

    /**
     * This method is called by the framework if this Flow's CAS has been sent to a CAS Multiplier that has created
     * a new output CAS. It may throw an exception if the Engine does not support CAS Multipliers.
     * @param cas         the new CAS that has been produced
     * @param producedBy  the key of the delegate engine that has produced this CAS
     * @return a new Flow object that will route the output CAS
     */
    virtual std::unique_ptr<Flow> newCasProduced(const CAS& cas, const icu::UnicodeString& producedBy)=0;

    /**
     * Called by the framework after a failure to see if the CAS should continue or not.
     * @param failedEngine the key of the engine whose failure led to this call
     * TODO: include the offending exception as a parameter?
     * @return whether processing should continue or be aborted
     */
    virtual bool continueOnFailure(const icu::UnicodeString& failedEngine) { return false; }

    /**
     * Called by the framework to alert this Flow object that processing has been stopped on this CAS so it can perform any cleanup
     */
    virtual void aborted() { }

    void setCas(CAS* cas) { inputCAS = cas; }
    CAS* getCas() const { return inputCAS; }
  };


}
#endif //UIMA_FLOW_HPP
