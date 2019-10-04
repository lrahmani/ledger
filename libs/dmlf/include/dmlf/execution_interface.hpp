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

#include "core/byte_array/byte_array.hpp"
#include "network/generics/promise_of.hpp"
#include "vm/common.hpp"

#include <functional>
#include <ostream>
#include <string>
#include <vector>

namespace fetch {
namespace dmlf {

class ExecutionInterface
{
public:
  ExecutionInterface()          = default;
  virtual ~ExecutionInterface() = default;

  using Name        = std::string;
  using SourceFiles = fetch::vm::SourceFiles;
  using Target      = std::string;
  using Artifact    = fetch::byte_array::ByteArray;
  using Result      = int;
  using Returned    = int;//fetch::network::PromiseOf<Result>;
  //using Params      = std::vector<Artifact>;

  virtual Returned CreateExecutable(const Target &target, const Name &execName, const SourceFiles &sources) = 0;
  virtual Returned DeleteExecutable(const Target &target, const Name &execName) = 0;
  
  virtual Returned CreateState(const Target &target, const Name &stateName) = 0;
  virtual Returned CopyState(const Target &target, const Name &srcName, const Name &newName) = 0;
  virtual Returned DeleteState(const Target &target, const Name &stateName) = 0;
  
  virtual Returned Run(Target const &target, Name const &execName, Name const &stateName, std::string const &entrypoint) = 0;

  ExecutionInterface(const ExecutionInterface &other) = delete;
  ExecutionInterface &operator=(const ExecutionInterface &other) = delete;
  bool operator==(const ExecutionInterface &other) = delete;
  bool operator<(const ExecutionInterface &other) = delete;
protected:
private:
};

} // namespace dmlf
} // namespace fetch
