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

#include "vm_modules/math/math.hpp"
#include "vm_modules/math/tensor/tensor.hpp"
#include "vm_modules/math/type.hpp"
#include "vm_test_toolkit.hpp"

#include "gmock/gmock.h"

#include <sstream>

using namespace fetch::vm;

namespace math_tensor_tests {

using ::testing::Between;

using DataType = fetch::vm_modules::math::DataType;

class MathTensorTests : public ::testing::Test
{
public:
  std::stringstream stdout;
  VmTestToolkit     toolkit{&stdout};
};

TEST_F(MathTensorTests, tensor_squeeze_test)
{
  static char const *tensor_serialiase_src = R"(
    function main() : Tensor
      var tensor_shape = Array<UInt64>(3);
      tensor_shape[0] = 4u64;
      tensor_shape[1] = 1u64;
      tensor_shape[2] = 4u64;
      var x = Tensor(tensor_shape);
      var squeezed_x = x.squeeze();
      return squeezed_x;
    endfunction
  )";

  Variant res;
  ASSERT_TRUE(toolkit.Compile(tensor_serialiase_src));
  ASSERT_TRUE(toolkit.Run(&res));

  auto const                    tensor = res.Get<Ptr<fetch::vm_modules::math::VMTensor>>();
  fetch::math::Tensor<DataType> gt({4, 4});

  EXPECT_TRUE(tensor->GetTensor().shape() == gt.shape());
}

/// GETTER AND SETTER TESTS ///

TEST_F(MathTensorTests, tensor_set_and_at_one_test)
{
  static char const *tensor_serialiase_src = R"(
    function main() : Tensor
      var tensor_shape = Array<UInt64>(1);
      tensor_shape[0] = 2u64;

      var x = Tensor(tensor_shape);
      var y = Tensor(tensor_shape);
      x.fill(2.0fp64);

      y.setAt(0u64,x.at(0u64));
      y.setAt(1u64,x.at(1u64));

     return y;
    endfunction
  )";

  ASSERT_TRUE(toolkit.Compile(tensor_serialiase_src));
  Variant res;
  ASSERT_TRUE(toolkit.Run(&res));

  auto const                    tensor = res.Get<Ptr<fetch::vm_modules::math::VMTensor>>();
  fetch::math::Tensor<DataType> gt({2});
  gt.Fill(static_cast<DataType>(2.0));

  EXPECT_TRUE(gt.AllClose(tensor->GetTensor()));
}

TEST_F(MathTensorTests, tensor_set_and_at_two_test)
{
  static char const *tensor_serialiase_src = R"(
    function main() : Tensor
      var tensor_shape = Array<UInt64>(2);
      tensor_shape[0] = 2u64;
      tensor_shape[1] = 2u64;

      var x = Tensor(tensor_shape);
      var y = Tensor(tensor_shape);
      x.fill(2.0fp64);

      y.setAt(0u64,0u64,x.at(0u64,0u64));
      y.setAt(0u64,1u64,x.at(0u64,1u64));
      y.setAt(1u64,0u64,x.at(1u64,0u64));
      y.setAt(1u64,1u64,x.at(1u64,1u64));

     return y;
    endfunction
  )";

  ASSERT_TRUE(toolkit.Compile(tensor_serialiase_src));
  Variant res;
  ASSERT_TRUE(toolkit.Run(&res));

  auto const                    tensor = res.Get<Ptr<fetch::vm_modules::math::VMTensor>>();
  fetch::math::Tensor<DataType> gt({2, 2});
  gt.Fill(static_cast<DataType>(2.0));

  EXPECT_TRUE(gt.AllClose(tensor->GetTensor()));
}

TEST_F(MathTensorTests, tensor_set_and_at_three_test)
{
  static char const *tensor_serialiase_src = R"(
    function main() : Tensor
      var tensor_shape = Array<UInt64>(3);
      tensor_shape[0] = 2u64;
      tensor_shape[1] = 2u64;
      tensor_shape[2] = 2u64;

      var x = Tensor(tensor_shape);
      var y = Tensor(tensor_shape);
      x.fill(2.0fp64);

      y.setAt(0u64,0u64,0u64,x.at(0u64,0u64,0u64));
      y.setAt(0u64,1u64,0u64,x.at(0u64,1u64,0u64));
      y.setAt(1u64,0u64,0u64,x.at(1u64,0u64,0u64));
      y.setAt(1u64,1u64,0u64,x.at(1u64,1u64,0u64));
      y.setAt(0u64,0u64,1u64,x.at(0u64,0u64,1u64));
      y.setAt(0u64,1u64,1u64,x.at(0u64,1u64,1u64));
      y.setAt(1u64,0u64,1u64,x.at(1u64,0u64,1u64));
      y.setAt(1u64,1u64,1u64,x.at(1u64,1u64,1u64));

     return y;
    endfunction
  )";

  ASSERT_TRUE(toolkit.Compile(tensor_serialiase_src));
  Variant res;
  ASSERT_TRUE(toolkit.Run(&res));

  auto const                    tensor = res.Get<Ptr<fetch::vm_modules::math::VMTensor>>();
  fetch::math::Tensor<DataType> gt({2, 2, 2});
  gt.Fill(static_cast<DataType>(2.0));

  EXPECT_TRUE(gt.AllClose(tensor->GetTensor()));
}

TEST_F(MathTensorTests, tensor_set_and_at_four_test)
{
  static char const *tensor_serialiase_src = R"(
    function main() : Tensor
      var tensor_shape = Array<UInt64>(4);
      tensor_shape[0] = 2u64;
      tensor_shape[1] = 2u64;
      tensor_shape[2] = 2u64;
      tensor_shape[3] = 2u64;

      var x = Tensor(tensor_shape);
      var y = Tensor(tensor_shape);
      x.fill(2.0fp64);

      y.setAt(0u64,0u64,0u64,0u64,x.at(0u64,0u64,0u64,0u64));
      y.setAt(0u64,1u64,0u64,0u64,x.at(0u64,1u64,0u64,0u64));
      y.setAt(1u64,0u64,0u64,0u64,x.at(1u64,0u64,0u64,0u64));
      y.setAt(1u64,1u64,0u64,0u64,x.at(1u64,1u64,0u64,0u64));
      y.setAt(0u64,0u64,1u64,0u64,x.at(0u64,0u64,1u64,0u64));
      y.setAt(0u64,1u64,1u64,0u64,x.at(0u64,1u64,1u64,0u64));
      y.setAt(1u64,0u64,1u64,0u64,x.at(1u64,0u64,1u64,0u64));
      y.setAt(1u64,1u64,1u64,0u64,x.at(1u64,1u64,1u64,0u64));
      y.setAt(0u64,0u64,0u64,1u64,x.at(0u64,0u64,0u64,1u64));
      y.setAt(0u64,1u64,0u64,1u64,x.at(0u64,1u64,0u64,1u64));
      y.setAt(1u64,0u64,0u64,1u64,x.at(1u64,0u64,0u64,1u64));
      y.setAt(1u64,1u64,0u64,1u64,x.at(1u64,1u64,0u64,1u64));
      y.setAt(0u64,0u64,1u64,1u64,x.at(0u64,0u64,1u64,1u64));
      y.setAt(0u64,1u64,1u64,1u64,x.at(0u64,1u64,1u64,1u64));
      y.setAt(1u64,0u64,1u64,1u64,x.at(1u64,0u64,1u64,1u64));
      y.setAt(1u64,1u64,1u64,1u64,x.at(1u64,1u64,1u64,1u64));

     return y;
    endfunction
  )";

  ASSERT_TRUE(toolkit.Compile(tensor_serialiase_src));
  Variant res;
  ASSERT_TRUE(toolkit.Run(&res));

  auto const                    tensor = res.Get<Ptr<fetch::vm_modules::math::VMTensor>>();
  fetch::math::Tensor<DataType> gt({2, 2, 2, 2});
  gt.Fill(static_cast<DataType>(2.0));

  EXPECT_TRUE(gt.AllClose(tensor->GetTensor()));
}

TEST_F(MathTensorTests, tensor_set_from_string)
{
  static char const *tensor_from_string_src = R"(
    function main() : Tensor
      var tensor_shape = Array<UInt64>(3);
      tensor_shape[0] = 4u64;
      tensor_shape[1] = 1u64;
      tensor_shape[2] = 1u64;

      var x = Tensor(tensor_shape);
      x.fill(2.0fp64);

      var string_vals = "1.0, 1.0, 1.0, 1.0";
      x.fromString(string_vals);

      return x;

    endfunction
  )";

  ASSERT_TRUE(toolkit.Compile(tensor_from_string_src));
  Variant res;
  ASSERT_TRUE(toolkit.Run(&res));

  auto const                    tensor = res.Get<Ptr<fetch::vm_modules::math::VMTensor>>();
  fetch::math::Tensor<DataType> gt({4, 1, 1});
  gt.Fill(static_cast<DataType>(1.0));

  EXPECT_TRUE(gt.AllClose(tensor->GetTensor()));
}

/// MATRIX OPERATION TESTS ///

TEST_F(MathTensorTests, tensor_min_test)
{
  fetch::math::Tensor<DataType> tensor = fetch::math::Tensor<DataType>::FromString(
      "0.5, 7.1, 9.1; 6.2, 7.1, 4.; -99.1, 14328.1, 10.0;");
  fetch::vm_modules::math::VMTensor vm_tensor(&toolkit.vm(), 0, tensor);

  DataType result = vm_tensor.Min();
  DataType gt{-99.1};

  EXPECT_TRUE(result == gt);
}

TEST_F(MathTensorTests, tensor_min_etch_test)
{
  static char const *tensor_min_src = R"(
    function main() : Fixed64
      var tensor_shape = Array<UInt64>(2);
      tensor_shape[0] = 3u64;
      tensor_shape[1] = 3u64;
      var x = Tensor(tensor_shape);
      x.fill(7.0fp64);
      x.setAt(0u64, 1u64, -7.0fp64);
      x.setAt(1u64, 1u64, 23.1fp64);
      var ret = x.min();
      return ret;
    endfunction
  )";

  std::string const state_name{"tensor"};

  ASSERT_TRUE(toolkit.Compile(tensor_min_src));
  Variant res;
  ASSERT_TRUE(toolkit.Run(&res));

  auto const min_val = res.Get<DataType>();
  DataType   gt{-7.0};

  std::cout << "gt: " << gt << std::endl;
  std::cout << "min_val: " << min_val << std::endl;

  EXPECT_TRUE(gt == min_val);
}

/// SERIALISATION TESTS ///

TEST_F(MathTensorTests, tensor_state_test)
{
  static char const *tensor_serialiase_src = R"(
    function main()
      var tensor_shape = Array<UInt64>(2);
      tensor_shape[0] = 2u64;
      tensor_shape[1] = 10u64;
      var x = Tensor(tensor_shape);
      x.fill(7.0fp64);
      var state = State<Tensor>("tensor");
      state.set(x);
    endfunction
  )";

  std::string const state_name{"tensor"};

  ASSERT_TRUE(toolkit.Compile(tensor_serialiase_src));

  EXPECT_CALL(toolkit.observer(), Write(state_name, _, _));
  ASSERT_TRUE(toolkit.Run());

  static char const *tensor_deserialiase_src = R"(
    function main() : Tensor
      var state = State<Tensor>("tensor");
      return state.get();
    endfunction
  )";

  ASSERT_TRUE(toolkit.Compile(tensor_deserialiase_src));

  Variant res;
  EXPECT_CALL(toolkit.observer(), Exists(state_name));
  EXPECT_CALL(toolkit.observer(), Read(state_name, _, _)).Times(Between(1, 2));
  ASSERT_TRUE(toolkit.Run(&res));

  auto const                    tensor = res.Get<Ptr<fetch::vm_modules::math::VMTensor>>();
  fetch::math::Tensor<DataType> gt({2, 10});
  gt.Fill(static_cast<DataType>(7.0));

  EXPECT_TRUE(gt.AllClose(tensor->GetTensor()));
}

}  // namespace math_tensor_tests
