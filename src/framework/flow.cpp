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
-----------------------------------------------------------------------------


-------------------------------------------------------------------------- */

#include "uima/flow.hpp"

namespace uima {
  Step::Step(const internal::SimpleStep &simpleStep): uStep(simpleStep),
                                                      type(StepType::SIMPLESTEP) {
  }

  Step::Step(const internal::ParallelStep &parallelStep): uStep(parallelStep),
                                                          type(StepType::PARALLELSTEP) {
  }

  Step::Step(const internal::FinalStep &finalStep): uStep(finalStep),
                                                    type(StepType::FINALSTEP) {
  }

  Step::Step(const Step& other) :type(other.type) {
    // properly initialize union value that was not constructed (invalid)
    switch (other.type) {
      case StepType::SIMPLESTEP:
        new (&uStep.simpleStep) auto(other.uStep.simpleStep);
        break;
      case StepType::FINALSTEP:
        new (&uStep.finalStep) auto(other.uStep.finalStep);
        break;
      case StepType::PARALLELSTEP:
        new (&uStep.parallelStep) auto(other.uStep.parallelStep);
      default:
        break;
    }

  }

  Step & Step::operator=(const Step &other) {
    if (this == &other) {
      return *this;
    }
    // destroy the current type in the union and reinitialize it with other
    switch (this->type) {
      case StepType::SIMPLESTEP:
        uStep.simpleStep.~SimpleStep();
        break;
      case StepType::FINALSTEP:
        uStep.finalStep.~FinalStep();
        break;
      case StepType::PARALLELSTEP:
        uStep.parallelStep.~ParallelStep();
        break;
      default:
        break;
    }

    type = other.type;
    switch (other.type) {
      case StepType::SIMPLESTEP:
        new (&uStep.simpleStep) auto(other.uStep.simpleStep);
        break;
      case StepType::FINALSTEP:
        new (&uStep.finalStep) auto(other.uStep.finalStep);
        break;
      case StepType::PARALLELSTEP:
        new (&uStep.parallelStep) auto(other.uStep.parallelStep);
        break;
      default:
        break;
    }
    return *this;
  }

  Step::~Step() {
    switch (type) {
      case StepType::SIMPLESTEP:
        uStep.simpleStep.~SimpleStep();
        break;
      case StepType::FINALSTEP:
        uStep.finalStep.~FinalStep();
        break;
      case StepType::PARALLELSTEP:
        uStep.parallelStep.~ParallelStep();
        break;
      default:
        break;
    }
  }

  const internal::SimpleStep * Step::getSimpleStep() const {
    if (type != StepType::SIMPLESTEP) return nullptr;
    return &uStep.simpleStep;
  }

  const internal::ParallelStep * Step::getParallelStep() const {
    if (type != StepType::PARALLELSTEP) return nullptr;
    return &uStep.parallelStep;
  }

  const internal::FinalStep * Step::getFinalStep() const {
    if (type != StepType::FINALSTEP) return nullptr;
    return &uStep.finalStep;
  }

  Step::StepType Step::getType() const {
    return type;
  }
}
