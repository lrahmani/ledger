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

#include "gtest/gtest.h"

#include "dmlf/vm_wrapper_etch.hpp"

#include "variant/variant.hpp"
#include "vm/vm.hpp"
#include "vm_modules/vm_factory.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace {

using fetch::vm_modules::VMFactory;
using namespace fetch::vm;

using Params = fetch::dmlf::VmWrapperInterface::Params;
using Status = fetch::dmlf::VmWrapperInterface::Status;

}  // namespace

TEST(VmDmlfTests, etch_simpleHelloWorld)
{
  // load the contents of the script file
  auto const source = R"(
function main()

  printLn("Hello world!!");

endfunction)";

  fetch::dmlf::VmWrapperEtch vm;
  EXPECT_EQ(vm.status(), Status::UNCONFIGURED);

  vm.Setup(fetch::dmlf::VmWrapperInterface::Flags());
  EXPECT_EQ(vm.status(), Status::WAITING);

  std::vector<std::string> result;
  auto outputTest = [&result] (std::string line) {
    result.emplace_back(std::move(line));
  };
  vm.SetStdout(outputTest);

  auto errors = vm.Load(source);
  EXPECT_EQ(vm.status(), Status::COMPILED);
  EXPECT_TRUE(errors.empty());


  vm.Execute("main", Params());
  EXPECT_EQ(vm.status(), Status::COMPLETED);

  EXPECT_EQ(result.size(), 1);
  EXPECT_EQ(result[0], "Hello world!!");
}

TEST(VmDmlfTests, etch_doubleHelloWorld)
{
  // load the contents of the script file
  auto const source = R"(
function main()

  printLn("Hello world!!");

endfunction)";

  fetch::dmlf::VmWrapperEtch vm;
  EXPECT_EQ(vm.status(), Status::UNCONFIGURED);

  vm.Setup(fetch::dmlf::VmWrapperInterface::Flags());
  EXPECT_EQ(vm.status(), Status::WAITING);

  std::vector<std::string> result;
  auto outputTest = [&result] (std::string line) {
    result.emplace_back(std::move(line));
  };
  vm.SetStdout(outputTest);

  auto errors = vm.Load(source);
  EXPECT_EQ(vm.status(), Status::COMPILED);
  EXPECT_TRUE(errors.empty());


  vm.Execute("main", Params());
  EXPECT_EQ(vm.status(), Status::COMPLETED);

  auto const source2 = R"(
function main()

  printLn("Hello world again!!!");

endfunction)";

  errors = vm.Load(source2);
  EXPECT_EQ(vm.status(), Status::COMPILED);
  EXPECT_TRUE(errors.empty());

  vm.Execute("main", Params());
  EXPECT_EQ(vm.status(), Status::COMPLETED);

  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result[0], "Hello world!!");
  EXPECT_EQ(result[1], "Hello world again!!!");

}

TEST(VmDmlfTests, etch_compilationError)
{
  // load the contents of the script file
  auto const source = R"(
function main()

  printLn("Hello world!!")sas;

endfunction)";

  fetch::dmlf::VmWrapperEtch vm;
  EXPECT_EQ(vm.status(), Status::UNCONFIGURED);

  vm.Setup(fetch::dmlf::VmWrapperInterface::Flags());
  EXPECT_EQ(vm.status(), Status::WAITING);

  std::vector<std::string> result;
  auto errorOutput = [&result] (std::string line) {
    //std::cout << "---\nErr: " << line << "\n---\n";
    result.emplace_back(std::move(line));
  };
  vm.SetStderr(errorOutput);

  auto errors = vm.Load(source);
  EXPECT_EQ(vm.status(), Status::FAILED_COMPILATION);

  //EXPECT_EQ(result.size(), 2);
  //EXPECT_EQ(result[0], "default.etch: line 4: error at 'sas', expected ';' or assignment operator");
  //EXPECT_EQ(result[1], "Failed to compile.");
  EXPECT_EQ(result.size(), errors.size());
  for (std::size_t i = 0; i < result.size(); ++i)
  {
    EXPECT_EQ(result[i], errors[i]);
  }
}

TEST(VmDmlfTests, etch_runtimeError)
{
  // load the contents of the script file
  // note the array has size 2 and indexes to 3 so it causes a runtime 
  auto const source = R"(
function main()

    
    var myArray = Array<Int32>(2);
    myArray[0] = 0;
    myArray[1] = 1;
    
    for (i in 0:3)
       printLn("Hello World, how are you? [" + toString(myArray[i]) + "]");
    endfor
    
endfunction
)";

  fetch::dmlf::VmWrapperEtch vm;
  EXPECT_EQ(vm.status(), Status::UNCONFIGURED);

  vm.Setup(fetch::dmlf::VmWrapperInterface::Flags());
  EXPECT_EQ(vm.status(), Status::WAITING);

  std::vector<std::string> errorResult;
  auto errorOutput = [&errorResult] (std::string line) {
    //std::cout << "---\nErr: " << line << "\n---\n";
    errorResult.emplace_back(std::move(line));
  };
  vm.SetStderr(errorOutput);

  std::vector<std::string> outResult;
  auto outOutput = [&outResult] (std::string line) {
    outResult.emplace_back(std::move(line));
  };
  vm.SetStdout(outOutput);

  auto errors = vm.Load(source);
  EXPECT_EQ(vm.status(), Status::COMPILED);
  EXPECT_TRUE(errors.empty());


  vm.Execute("main", Params());
  EXPECT_EQ(vm.status(), Status::FAILED_RUN);

  EXPECT_EQ(errorResult.size(), 1);
  EXPECT_EQ(errorResult[0], "runtime error: line 10: index out of bounds");

  EXPECT_EQ(outResult.size(), 2);
  EXPECT_EQ(outResult[0], "Hello World, how are you? [0]");
  EXPECT_EQ(outResult[1], "Hello World, how are you? [1]");

}

TEST(VmDmlfTests, etch_persistent)
{
  // load the contents of the script file
  auto const source = R"(

persistent foo : Int32;

function main()

  use foo;

  printLn("Global is " + toString(foo.get(0)));

  foo.set(foo.get(0) + 1);

endfunction
)";

  fetch::dmlf::VmWrapperEtch vm;
  EXPECT_EQ(vm.status(), Status::UNCONFIGURED);

  vm.Setup(fetch::dmlf::VmWrapperInterface::Flags());
  EXPECT_EQ(vm.status(), Status::WAITING);

  std::vector<std::string> result;
  auto outputTest = [&result] (std::string line) {
    result.emplace_back(std::move(line));
  };
  vm.SetStdout(outputTest);

  auto errors = vm.Load(source);
  for (auto e : errors) {
    std::cout << "COMP: " <<  e << '\n';
  }
  EXPECT_EQ(vm.status(), Status::COMPILED);
  EXPECT_TRUE(errors.empty());


  vm.Execute("main", Params());
  EXPECT_EQ(vm.status(), Status::COMPLETED);
  vm.Execute("main", Params());
  EXPECT_EQ(vm.status(), Status::COMPLETED);

  EXPECT_EQ(result.size(), 2);
  EXPECT_EQ(result[0], "Global is 0");
  EXPECT_EQ(result[1], "Global is 1");
}

}  // namespace
