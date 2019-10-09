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

#include <map>
#include <memory>
#include <sstream>

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
  using Variant    = ExecutionInterface::Variant;
  using PromiseOfResult    = ExecutionInterface::PromiseOfResult;
  using Params      = ExecutionInterface::Params;

  using ErrorStage = ExecutionErrorMessage::Stage;
  using ErrorCode  = ExecutionErrorMessage::Code;
  using Error = ExecutionResult::Error;
  
  // FROM LocalVmLauncher
  using Executable = fetch::vm::Executable; // changed from using Program 
  using VM = fetch::vm::VM;
  using VmFactory = fetch::vm_modules::VMFactory;
  using State = VmPersistent;

  // FROM VmLauncherInterface
  // Flags?
  using VmOutputHandler = std::ostream;
  //using Params = std::vector<std::string>; // TOFIX
  // Program name, Error
  using ProgramErrorHandler = std::function<void (std::string const&, std::vector<std::string>)>;
  // Program name, VM name, State name, Error
  using ExecuteErrorHandler = std::function<void (std::string const&, std::string const&, std::string const&, std::string const&)>;
  
  virtual PromiseOfResult CreateExecutable(Target const &host, Name const &execName,
                                    SourceFiles const &sources)                 override;
  virtual PromiseOfResult DeleteExecutable(Target const &host, Name const &execName) override;

  virtual PromiseOfResult CreateState(Target const &host, Name const &stateName)                  override;
  virtual PromiseOfResult CopyState(Target const &host, Name const &srcName, Name const &newName) override;
  virtual PromiseOfResult DeleteState(Target const &host, Name const &stateName)                  override;

  virtual PromiseOfResult Run(Target const &host, Name const &execName, Name const &stateName,
                       std::string const &entrypoint) override;
  
  // utilities
  bool CreateTarget(Target const &host);
  bool HasTarget(Target const &host);

  LocalExecutionEngine(LocalExecutionEngine const &other) = delete;
  LocalExecutionEngine &operator=(LocalExecutionEngine const &other)  = delete;
  bool                operator==(LocalExecutionEngine const &other) = delete;
  bool                operator<(LocalExecutionEngine const &other)  = delete;

private:
  using PromiseFulfiller = std::shared_ptr<service::details::PromiseImplementation>;

  bool HasExecutable(Target const &host, Name const &execName);
  bool HasState(Target const &host, Name const &stateName);
  PromiseOfResult MakeErrorResult(Error err);
  PromiseOfResult MakeErrorResult(ErrorCode err_code, std::string err_msg);
  PromiseOfResult MakeSuccessResult();

  PromiseFulfiller CreatePromise();
  PromiseOfResult FulfillPromise(PromiseFulfiller promise, ExecutionResult res);

  std::map<std::string, std::shared_ptr<Executable>> executables_;
  std::map<std::string, std::shared_ptr<VM>> vms_;
  std::map<std::string, std::shared_ptr<State>> states_;
  ProgramErrorHandler programErrorHandler_ = nullptr;
  ExecuteErrorHandler executeErrorhandler_ = nullptr;
  
  std::shared_ptr<fetch::vm::Module> module_ = VmFactory::GetModule(VmFactory::USE_SMART_CONTRACTS);
  
};

}  // namespace dmlf
}  // namespace fetch
