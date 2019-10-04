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
}

LocalExecutionInterface::LocalExecutionInterface()
: vm_(module_.get())
{  
}

Returned LocalExecutionInterface::CreateExecutable(Target const  &/*target*/, const Name &execName, const SourceFiles &sources)
{
  if (executables_.find(execName) != executables_.cend()) {
    return 1;
  }

  Executable newExecutable;

  auto errors = VmFactory::Compile(module_, sources, newExecutable);

  if (!errors.empty()) {
    return 1;
  }

  executables_.emplace(execName, std::move(newExecutable));
  return 0;
}

Returned LocalExecutionInterface::DeleteExecutable(const Target &/*target*/, const Name &execName)
{
  auto count = executables_.erase(execName);

  if (count == 0)
  {
    return 1;
  }

  return 0;
}

Returned LocalExecutionInterface::CreateState(const Target &/*target*/, const Name &stateName)
{
  if (states_.find(stateName) != states_.end()) 
  {
    return 1;
  }

  states_.emplace(stateName, State());
  return 0;
}
Returned LocalExecutionInterface::CopyState(const Target &/*target*/, const Name &srcName, const Name &newName)
{
  auto it = states_.find(srcName);

  if (it == states_.end()) 
  {
    return 1;
  }

  const State &source = it->second;

  if (states_.find(newName) != states_.end()) 
  {
    return 1;
  }

  states_.emplace(newName, source.DeepCopy());
  return 0;
}
Returned LocalExecutionInterface::DeleteState(const Target &/*target*/, const Name &stateName) 
{
  auto count = states_.erase(stateName);

  if (count == 0) 
  {
    return 1;
  }
  return 0;
}

Returned LocalExecutionInterface::Run(const Target &/*target*/, const Name &execName, const Name &stateName, std::string const &entrypoint)
{
  auto execIt = executables_.find(execName);
  if (execIt == executables_.end())
  {
    return 1;
  }
  Executable const &executable = execIt->second; 

  auto stateIt = states_.find(stateName);
  if (stateIt == states_.end())
  {
    return 1;
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
    return 1;
  }

  return 0;
}

} // namespace dmlf
} // namespace fetch
