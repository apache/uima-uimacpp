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
  Step::Step(const internal::SimpleStep &simpleStep): step(simpleStep),
                                                      type(StepType::SIMPLESTEP) {
  }

  Step::Step(const internal::ParallelStep &parallelStep): step(parallelStep),
                                                          type(StepType::PARALLELSTEP) {
  }

  Step::Step(const internal::FinalStep &finalStep): step(finalStep),
                                                    type(StepType::FINALSTEP) {
  }

  Step::Step(const Step& other) :type(other.type){
    switch (other.type) {
      case StepType::SIMPLESTEP:
        step.simpleStep = other.step.simpleStep;
        break;
      case StepType::FINALSTEP:
        step.finalStep = other.step.finalStep;
        break;
      default:
        break;
    }

  }

  Step & Step::operator=(const Step &other) {
    if (this == &other) {
      return *this;
    }
    type = other.type;
    switch (other.type) {
      case StepType::SIMPLESTEP:
        step.simpleStep = other.step.simpleStep;
        break;
      case StepType::FINALSTEP:
        step.finalStep = other.step.finalStep;
        break;
      default:
        break;
    }
    return *this;
  }

  Step::~Step() {
    switch (type) {
      case StepType::SIMPLESTEP:
        step.simpleStep.~SimpleStep();
      break;
      case StepType::FINALSTEP:
        step.finalStep.~FinalStep();
      break;
      case StepType::PARALLELSTEP:
        step.parallelStep.~ParallelStep();
      break;
      default:
        break;
    }
  }

  const internal::SimpleStep * Step::getSimpleStep() const {
    if (type != StepType::SIMPLESTEP) return nullptr;
    return &step.simpleStep;
  }

  const internal::ParallelStep * Step::getParallelStep() const {
    if (type != StepType::PARALLELSTEP) return nullptr;
    return &step.parallelStep;
  }

  const internal::FinalStep * Step::getFinalStep() const {
    if (type != StepType::FINALSTEP) return nullptr;
    return &step.finalStep;
  }

  Step::StepType Step::getType() const {
    return type;
  }
}
