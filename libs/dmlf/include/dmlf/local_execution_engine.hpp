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

#include "dmlf/vm_persistent.hpp"
#include "vm/vm.hpp"
#include "vm_modules/vm_factory.hpp"

#include <unordered_map>
#include <memory>
#include <sstream>

namespace fetch {
namespace dmlf {

class LocalExecutionInterface : public ExecutionInterface 
{
public:
  LocalExecutionInterface();
  virtual ~LocalExecutionInterface() = default;

  using Executable = fetch::vm::Executable;
  using VM = fetch::vm::VM;
  using VmFactory = fetch::vm_modules::VMFactory;
  using State = VmPersistent;

  Returned CreateExecutable(Target const &target, Name const &execName, SourceFiles const &sources) override;
  Returned DeleteExecutable(Target const &target, Name const &execName) override;
  
  Returned CreateState(Target const &target, Name const &stateName) override;
  Returned CopyState(Target const &target, Name const &srcName, Name const &newName) override;
  Returned DeleteState(Target const &target, Name const &stateName) override;
  
  Returned Run(Target const &target, Name const &execName, Name const &stateName, std::string const &entrypoint) override;

  LocalExecutionInterface(LocalExecutionInterface const &other) = delete;
  LocalExecutionInterface &operator=(LocalExecutionInterface const &other) = delete;
  bool operator==(LocalExecutionInterface const &other) = delete;
  bool operator<(LocalExecutionInterface const &other) = delete;

private:
  bool AmTarget(std::string const &target) const;
  Returned GetWrongTargetError() const;

  std::unordered_map<std::string, Executable> executables_;
  std::unordered_map<std::string, State> states_;

  std::shared_ptr<fetch::vm::Module> module_ = VmFactory::GetModule(VmFactory::USE_SMART_CONTRACTS);
  VM vm_;

  std::unordered_set<std::string> myNames_ = {"local://", ""};
};

} // namespace dmlf
} // namespace fetch
