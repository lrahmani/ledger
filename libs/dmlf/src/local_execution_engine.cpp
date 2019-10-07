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

#include "dmlf/vm_persistent.hpp"
#include "vm/vm.hpp"
#include "vm_modules/vm_factory.hpp"

#include <memory>
#include <sstream>

namespace fetch {
namespace dmlf {

namespace {
  using Returned = LocalExecutionInterface::Returned;
  using Variant = Returned::Variant;
  using Stage = Returned::Error::Stage;
  using Code  = Returned::Error::Code;
}

LocalExecutionInterface::LocalExecutionInterface()
: vm_(module_.get())
{  
}

Returned LocalExecutionInterface::CreateExecutable(Target const &target, Name const &execName, SourceFiles const &sources)
{
  if (!AmTarget(target))
  {
    return GetWrongTargetError();
  }

  if (executables_.find(execName) != executables_.end())
  {
    return GetWrongNameError(Code::BAD_EXECUTABLE);
  }

  Executable newExecutable;
  std::vector<std::string> errors = VmFactory::Compile(module_, sources, newExecutable);

  if (!errors.empty())
  {
    std::stringstream ss;
    std::for_each(errors.begin(), errors.end(), [&ss] (std::string const &line) { ss << line; });
    return Returned(Variant(), Error(Stage::COMPILE, Code::COMPILATION_ERROR), ss.str());
  }

  executables_.emplace(execName, std::move(newExecutable));
  return Success(Stage::COMPILE);
}

Returned LocalExecutionInterface::DeleteExecutable(Target const &target, const Name &execName)
{
  if (!AmTarget(target))
  {
    return GetWrongTargetError();
  }

  auto count = executables_.erase(execName);
  if (count == 0)
  {
    return GetWrongNameError(Code::BAD_EXECUTABLE);
  }

  return Success(Stage::ENGINE);
}

Returned LocalExecutionInterface::CreateState(const Target &target, const Name &stateName)
{
  if (!AmTarget(target))
  {
    return GetWrongTargetError();
  }
  if (states_.find(stateName) != states_.end()) 
  {
    return GetWrongNameError(Code::BAD_STATE);
  }

  states_.emplace(stateName, State());
  return Success(Stage::ENGINE);
}
Returned LocalExecutionInterface::CopyState(const Target &target, const Name &stateName, const Name &destName)
{
  if (!AmTarget(target))
  {
    return GetWrongTargetError();
  }
  auto it = states_.find(stateName);

  if (it == states_.end()) 
  {
    return GetWrongNameError(Code::BAD_STATE);
  }

  const State &source = it->second;

  if (states_.find(destName) != states_.end()) 
  {
    return GetWrongNameError(Code::BAD_DESTINATION);
  }

  states_.emplace(destName, source.DeepCopy());
  return Success(Stage::ENGINE);
}
Returned LocalExecutionInterface::DeleteState(Target const &target, Name const &stateName) 
{
  if (!AmTarget(target))
  {
    return GetWrongTargetError();
  }
  auto count = states_.erase(stateName);

  if (count == 0) 
  {
    return GetWrongNameError(Code::BAD_STATE);
  }
  return Success(Stage::ENGINE);
}

Returned LocalExecutionInterface::Run(Target const &target, Name const &execName, Name const &stateName, std::string const &entrypoint)
{
  if (!AmTarget(target))
  {
    return GetWrongTargetError();
  }
  auto execIt = executables_.find(execName);
  if (execIt == executables_.end())
  {
    return GetWrongNameError(Code::BAD_EXECUTABLE);
  }
  Executable const &executable = execIt->second; 

  auto stateIt = states_.find(stateName);
  if (stateIt == states_.end())
  {
    return GetWrongNameError(Code::BAD_STATE);
  }
  State &state = stateIt->second; 
  vm_.SetIOObserver(state);
  
  std::stringstream stdOutput;
  vm_.AttachOutputDevice(VM::STDOUT, stdOutput);
  
  std::string runTimeError;
  fetch::vm::Variant runOutput;
  auto thereWasAnError = vm_.Execute(executable, entrypoint, runTimeError, runOutput);

  if (!runTimeError.empty() || thereWasAnError) 
  {
    return Returned(runOutput, Error(Stage::RUNNING, Code::RUNTIME_ERROR), runTimeError + '\n' + stdOutput.str());
  }

  return Returned(runOutput, Error(Stage::RUNNING, Code::SUCCESS), stdOutput.str());
}

bool LocalExecutionInterface::AmTarget(std::string const &target) const
{
  return myNames_.find(target) != myNames_.end();
}

Returned LocalExecutionInterface::GetWrongTargetError() const
{
  return Returned(Variant(), Error(Stage::ENGINE, Code::BAD_TARGET), "");
}

Returned LocalExecutionInterface::GetWrongNameError(Code code) const
{
  return Returned(Variant(), Error(Stage::ENGINE, code), "");
}

Returned LocalExecutionInterface::Success(Stage stage) const
{
  return Returned(Variant(), Error(stage, Code::SUCCESS), "");
}

} // namespace dmlf
} // namespace fetch
