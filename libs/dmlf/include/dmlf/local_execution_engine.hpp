#pragma once
//------------------------------------------------------------------------------
//
//   Copyright 2018-2019 Fetch.AI Limited
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//------------------------------------------------------------------------------

#include "dmlf/execution_interface.hpp"

namespace fetch {
namespace dmlf {

class LocalExecutionEngine : public ExecutionInterface
{
public:
  LocalExecutionEngine();
  virtual ~LocalExecutionEngine();

  using Name        = ExecutionInterface::Name;
  using SourceFiles = ExecutionInterface::SourceFiles;
  using Target      = ExecutionInterface::Target;
  using Artifact    = ExecutionInterface::Artifact;
  using Result      = ExecutionInterface::Result;
  using Returned    = ExecutionInterface::Returned;
  using Params      = ExecutionInterface::Params;

  using Stage = ExecutionErrorMessage::Stage;
  using Code  = ExecutionErrorMessage::Code;
  using Error = ExecutionResult::Error;

  virtual Returned CreateExecutable(Target const &target, Name const &execName,
                                    SourceFiles const &sources)                 override;
  virtual Returned DeleteExecutable(Target const &target, Name const &execName) override;

  virtual Returned CreateState(Target const &target, Name const &stateName)                  override;
  virtual Returned CopyState(Target const &target, Name const &srcName, Name const &newName) override;
  virtual Returned DeleteState(Target const &target, Name const &stateName)                  override;

  virtual Returned Run(Target const &target, Name const &execName, Name const &stateName,
                       std::string const &entrypoint) override;

  LocalExecutionEngine(LocalExecutionEngine const &other) = delete;
  LocalExecutionEngine &operator=(LocalExecutionEngine const &other)  = delete;
  bool                operator==(LocalExecutionEngine const &other) = delete;
  bool                operator<(LocalExecutionEngine const &other)  = delete;
};

}  // namespace dmlf
}  // namespace fetch
