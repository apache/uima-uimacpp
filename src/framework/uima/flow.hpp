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
    class UIMA_LINK_IMPORTSPEC SimpleStep {
      icu::UnicodeString engineName;
      /* ResultSpecification *resultSpec; */
    public:
      SimpleStep(const icu::UnicodeString &name, ResultSpecification *resultSpec = nullptr) : engineName(name) {
      }

      const icu::UnicodeString &getEngineName() const {
        return engineName;
      }

      /** Capability Language Flow Controller requires ResultSpecification, when it is eventually implemented
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

    class UIMA_LINK_IMPORTSPEC FinalStep {
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

  /*
   * Class <TT>Step</TT> is a union type of possible step types: Simple Step, Parallel Step and Final Step.
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
    union step_type {
      internal::SimpleStep simpleStep;
      internal::ParallelStep parallelStep;
      internal::FinalStep finalStep;

      step_type(const internal::SimpleStep &simpleStep) : simpleStep(simpleStep) {
      }

      step_type(const internal::ParallelStep &parallelStep) : parallelStep(parallelStep) {
      }

      step_type(const internal::FinalStep &finalStep) : finalStep(finalStep) {
      }
      step_type() { }
      ~step_type() { }
    } step;

    StepType type;
  };

  /**
   * Virtual base class for the Flow objects computed by the FlowController
   * @see FlowController::computeFlow
   */
  class UIMA_LINK_IMPORTSPEC Flow {
  public:
    virtual ~Flow(){};

    virtual Step next()=0;
    virtual std::unique_ptr<Flow> newCasProduced(const CAS&, const icu::UnicodeString&)=0;
    virtual bool continueOnFailure(const icu::UnicodeString&) { return false; }
    virtual void aborted() { }
  };


}
#endif //UIMA_FLOW_HPP
