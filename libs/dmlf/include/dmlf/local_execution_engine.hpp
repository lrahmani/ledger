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

  Returned CreateExecutable(const Target &target, const Name &execName, const SourceFiles &sources) override;
  Returned DeleteExecutable(const Target &target, const Name &execName) override;
  
  Returned CreateState(const Target &target, const Name &stateName) override;
  Returned CopyState(const Target &target, const Name &srcName, const Name &newName) override;
  Returned DeleteState(const Target &target, const Name &stateName) override;
  
  Returned Run(const Target &target, const Name &execName, const Name &stateName, std::string const &entrypoint) override;

  LocalExecutionInterface(const LocalExecutionInterface &other) = delete;
  LocalExecutionInterface &operator=(const LocalExecutionInterface &other) = delete;
  bool operator==(const LocalExecutionInterface &other) = delete;
  bool operator<(const LocalExecutionInterface &other) = delete;

private:
  // Missing my names
  std::unordered_map<std::string, Executable> executables_;
  std::unordered_map<std::string, State> states_;

  std::shared_ptr<fetch::vm::Module> module_ = VmFactory::GetModule(VmFactory::USE_SMART_CONTRACTS);
  VM vm_;
};

} // namespace dmlf
} // namespace fetch
