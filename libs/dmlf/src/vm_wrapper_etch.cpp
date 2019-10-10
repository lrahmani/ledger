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

#include "dmlf/vm_wrapper_etch.hpp"

#include "variant/variant.hpp"

#include <string>

namespace fetch {
namespace dmlf {

std::vector<std::string> VmWrapperEtch::Setup(const Flags & /*flags*/)
{
  module_ = VmFactory::GetModule(VmFactory::USE_SMART_CONTRACTS);  // Set according to flags
  status_ = VmWrapperInterface::WAITING;
  return std::vector<std::string>();
}
std::vector<std::string> VmWrapperEtch::Load(std::string source)
{
  // Create executable
  status_                       = VmWrapperInterface::COMPILING;
  command_                      = source;
  fetch::vm::SourceFiles files  = {{"default.etch", source}};
  auto                   errors = VmFactory::Compile(module_, files, *executable_);

  if (!errors.empty())
  {
    status_ = VmWrapperInterface::FAILED_COMPILATION;
    if (errorOutputHandler_ != nullptr)
    {
      std::for_each(errors.begin(), errors.end(), errorOutputHandler_);
    }
    return errors;
  }

  // Create the VM instance
  vm_ = std::make_unique<VM>(module_.get());
  vm_->AttachOutputDevice(VM::STDOUT, outputStream_);
  vm_->SetIOObserver(persistent_);
  status_ = VmWrapperInterface::COMPILED;
  return errors;
}
void VmWrapperEtch::Execute(const std::string &entrypoint, const Params & /*params*/)
{
  status_ = VmWrapperInterface::RUNNING;
  std::string        error;
  fetch::vm::Variant output;
  outputStream_ = std::stringstream();  // Clear the output stream
  /*auto result = */ vm_->Execute(*executable_, entrypoint, error, output);

  DoOutput();
  if (error == "")
  {
    status_ = VmWrapperInterface::COMPLETED;
  }
  else
  {
    status_ = VmWrapperInterface::FAILED_RUN;
    if (errorOutputHandler_ != nullptr)
    {
      errorOutputHandler_(error);
    }
  }
}

void VmWrapperEtch::DoOutput()
{
  if (outputHandler_ == nullptr)
    return;

  std::string line;
  while (std::getline(outputStream_, line))
  {
    outputHandler_(line);
  }
}

void VmWrapperEtch::SetStdout(OutputHandler handler)
{
  outputHandler_ = handler;
}

void VmWrapperEtch::SetStderr(OutputHandler handler)
{
  errorOutputHandler_ = handler;
}

}  // namespace dmlf
}  // namespace fetch
