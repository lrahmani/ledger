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

#include "dmlf/local_execution_engine.hpp"

namespace fetch {
namespace dmlf {

LocalExecutionEngine::LocalExecutionEngine()
{
}

LocalExecutionEngine::~LocalExecutionEngine()
{
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
LocalExecutionEngine::Returned LocalExecutionEngine::CreateExecutable(Target const &target, Name const &execName,
                                    SourceFiles const &sources)
{
  auto prom = std::make_shared<service::details::PromiseImplementation>();
  
  fetch::network::PromiseOf<ExecutionResult> ret{prom};
  
  Error err{Stage::ENGINE, Code::SUCCESS, std::string{}};
  ExecutionResult res{fetch::vm::Variant{}, err, std::string{}};
  
  serializers::MsgPackSerializer serializer;
  serializer << res;
  prom->Fulfill(serializer.data());

  return ret;
}

LocalExecutionEngine::Returned LocalExecutionEngine::DeleteExecutable(Target const &target, Name const &execName)
{
  return fetch::network::PromiseOf<ExecutionResult>{};
}

LocalExecutionEngine::Returned LocalExecutionEngine::CreateState(Target const &target, Name const &stateName)
{
  return Returned{};
}

LocalExecutionEngine::Returned LocalExecutionEngine::CopyState(Target const &target, Name const &srcName, Name const &newName)
{
  return Returned{};
}

LocalExecutionEngine::Returned LocalExecutionEngine::DeleteState(Target const &target, Name const &stateName)
{
  return Returned{};
}

LocalExecutionEngine::Returned LocalExecutionEngine::Run(Target const &target, Name const &execName, Name const &stateName,
                       std::string const &entrypoint)
{
  return Returned{};
}
#pragma clang diagnostic pop

}  // namespace dmlf
}  // namespace fetch
