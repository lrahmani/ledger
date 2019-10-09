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

#include <numeric>

namespace fetch {
namespace dmlf {

LocalExecutionEngine::LocalExecutionEngine()
{
  CreateTarget(Target{"local"});
}

LocalExecutionEngine::~LocalExecutionEngine()
{
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::CreateExecutable(Target const &host, Name const &execName,
                                    SourceFiles const &sources)
{
  if (!HasTarget(host)) 
  {
    return MakeErrorResult(ErrorCode::BAD_TARGET, std::string{"Host "+host+" not found"});
  }

  if(!HasExecutable(host,execName))
  {
    return MakeErrorResult(ErrorCode::BAD_EXECUTABLE, std::string{"Executable name"+execName+" already exists"});
  }
  
  auto exec = std::make_shared<Executable>(); 
  auto errors = fetch::vm_modules::VMFactory::Compile(module_, sources, *exec);

  if (!errors.empty()) 
  {
    std::string err_msg = std::accumulate(errors.begin(), errors.end(), std::string{""});
    return MakeErrorResult(Error{ErrorStage::COMPILE, ErrorCode::COMPILATION_ERROR, err_msg});
  }

  executables_[execName] = exec;
  
  return MakeSuccessResult();
}

LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::DeleteExecutable(Target const &/*host*/, Name const &execName)
{
  auto exec = executables_.find(execName);
  if(exec == executables_.end())
  {
    return MakeErrorResult(ErrorCode::BAD_EXECUTABLE, std::string{"Executable "+execName+" doesn't existed"});
  }
  executables_.erase(exec);

  return MakeSuccessResult();
}

LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::CreateState(Target const &host, Name const &stateName)
{
  if(HasState(host,stateName))
  {
    return MakeErrorResult(ErrorCode::BAD_STATE, std::string{"State name"+stateName+" already exists"});
  }
  states_[stateName] = std::make_shared<State>();

  return MakeSuccessResult();
}

LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::CopyState(Target const &host, Name const &srcName, Name const &newName)
{
  if(!HasState(host,srcName))
  {
    return MakeErrorResult(ErrorCode::BAD_STATE, std::string{"State name"+srcName+" doesn't exist"});
  }
  if(HasState(host,newName))
  {
    return MakeErrorResult(ErrorCode::BAD_STATE, std::string{"State name"+newName+" already exists"});
  }
  states_.emplace(newName, std::make_shared<State>(states_[srcName]->DeepCopy()));

  return MakeSuccessResult();
}

LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::DeleteState(Target const &/*host*/, Name const &stateName)
{
  auto state = states_.find(stateName);
  if(state == states_.end())
  {
    return MakeErrorResult(ErrorCode::BAD_STATE, std::string{"State "+stateName+" doesn't exist"});
  }
  states_.erase(state);

  return MakeSuccessResult();
}

LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::Run(Target const &host, Name const &execName, Name const &stateName,
                       std::string const &entrypoint)
{
  // TOFIX double lookup
  if(!HasTarget(host))
  {
    return MakeErrorResult(ErrorCode::BAD_TARGET, std::string{"Host "+host+" not found"});
  }
  if(!HasExecutable(host, execName))
  {
    return MakeErrorResult(ErrorCode::BAD_EXECUTABLE, std::string{"Executable "+execName+" doesn't existed"});
  }
  if(!HasState(host, stateName))
  {
    return MakeErrorResult(ErrorCode::BAD_STATE, std::string{"State "+stateName+" doesn't exist"});
  }

  auto vm = vms_[host];
  auto exec = executables_[execName];
  auto state = states_[stateName];

  vm->SetIOObserver(*state);

  std::string runtime_error;
  fetch::vm::Variant output;
  auto success = vm->Execute(*exec, entrypoint, runtime_error, output);

  if (!runtime_error.empty())
  {
    std::string err_msg = std::accumulate(runtime_error.begin(), runtime_error.end(), std::string{""});
    return MakeErrorResult(Error{ErrorStage::RUNNING, ErrorCode::RUNTIME_ERROR, err_msg});
  }
  // TOFIX could runtine_error be empty and success == false ?
  if (!success) 
  {
    return MakeErrorResult(Error{ErrorStage::RUNNING, ErrorCode::RUNTIME_ERROR, std::string{}});
  }

  auto promise = CreatePromise();
  auto result = PromiseOfResult{promise};
  FulfillPromise(promise, ExecutionResult{output, Error{ErrorStage::ENGINE, ErrorCode::SUCCESS, std::string{}}, std::string{}});

  return result;
}


bool LocalExecutionEngine::CreateTarget(Target const &host)
{
  if(HasTarget(host))
  {
    return false;
  }
  
  vms_[host] = std::make_shared<VM>(module_.get()); 
  return true;
}

bool LocalExecutionEngine::HasTarget(Target const &host)
{
  return vms_.find(host) != vms_.end(); 
}

bool LocalExecutionEngine::HasExecutable(Target const & /*host*/, Name const &execName)
{
  return executables_.find(execName) != executables_.end();
}

bool LocalExecutionEngine::HasState(Target const & /*host*/, Name const &stateName)
{
  return states_.find(stateName) != states_.end();
}

LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::MakeErrorResult(Error err)
{
  auto prom = std::make_shared<service::details::PromiseImplementation>();
  fetch::network::PromiseOf<ExecutionResult> ret{prom};
  ExecutionResult res{fetch::vm::Variant{}, err, std::string{}};
  serializers::MsgPackSerializer serializer;
  serializer << res;
  prom->Fulfill(serializer.data());
  return ret;
}

LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::MakeErrorResult(ErrorCode err_code, std::string err_msg)
{
  Error err{ErrorStage::ENGINE, err_code, err_msg};
  return MakeErrorResult(err);
}

LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::MakeSuccessResult()
{
  auto prom = std::make_shared<service::details::PromiseImplementation>();
  fetch::network::PromiseOf<ExecutionResult> ret{prom};
  Error err{ErrorStage::ENGINE, ErrorCode::SUCCESS, std::string{}};
  ExecutionResult res{fetch::vm::Variant{}, err, std::string{}};
  serializers::MsgPackSerializer serializer;
  serializer << res;
  prom->Fulfill(serializer.data());
  return ret;
}

LocalExecutionEngine::PromiseFulfiller LocalExecutionEngine::CreatePromise()
{
  auto promise = std::make_shared<service::details::PromiseImplementation>();
  return promise; 
}

LocalExecutionEngine::PromiseOfResult LocalExecutionEngine::FulfillPromise(PromiseFulfiller promise, ExecutionResult res)
{
  serializers::MsgPackSerializer serializer;
  serializer << res;
  promise->Fulfill(serializer.data());
  fetch::network::PromiseOf<ExecutionResult> result{promise};  
  return result;
}
#pragma clang diagnostic pop

}  // namespace dmlf
}  // namespace fetch
