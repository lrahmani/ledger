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

#include "dmlf/execution/execution_interface.hpp"

namespace fetch {
namespace dmlf {

class ExecutionEngineInterface
{
public:
  ExecutionEngineInterface();
  virtual ~ExecutionEngineInterface();

  ExecutionEngineInterface(ExecutionEngineInterface const &other)  = delete;
  ExecutionEngineInterface(ExecutionEngineInterface const &&other) = delete;
  ExecutionEngineInterface &operator=(ExecutionEngineInterface const &other) = delete;
  ExecutionEngineInterface &operator=(ExecutionEngineInterface const &&other) = delete;

  using Name            = ExecutionInterface::Name;
  using SourceFiles     = ExecutionInterface::SourceFiles;
  using Target          = ExecutionInterface::Target;
  using Variant         = ExecutionInterface::Variant;
  using PromiseOfResult = ExecutionInterface::PromiseOfResult;
  using Params          = ExecutionInterface::Params;

  virtual PromiseOfResult CreateExecutable(Name const &execName, SourceFiles const &sources) = 0;
  virtual PromiseOfResult DeleteExecutable(Name const &execName)                             = 0;

  virtual PromiseOfResult CreateState(Name const &stateName)                  = 0;
  virtual PromiseOfResult CopyState(Name const &srcName, Name const &newName) = 0;
  virtual PromiseOfResult DeleteState(Name const &stateName)                  = 0;

  virtual PromiseOfResult Run(Name const &execName, Name const &stateName,
                              std::string const &entrypoint) = 0;
};

}  // namespace dmlf
}  // namespace fetch
