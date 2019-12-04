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

#include "gmock/gmock.h"
#include "vm_modules/ml/model/model.hpp"
#include "vm_modules/ml/model/model_estimator.hpp"
#include "vm_test_toolkit.hpp"

#include <regex>
#include <sstream>

using namespace fetch::vm;

namespace {

using SizeType         = fetch::math::SizeType;
using DataType         = fetch::vm_modules::math::DataType;
using VmPtr            = fetch::vm::Ptr<fetch::vm::String>;
using VmModel          = fetch::vm_modules::ml::model::VMModel;
using VmModelEstimator = fetch::vm_modules::ml::model::ModelEstimator;
using DataType         = fetch::vm_modules::ml::model::ModelEstimator::DataType;

class VMModelEstimatorTests : public ::testing::Test
{
public:
  std::stringstream stdout;
  VmTestToolkit     toolkit{&stdout};
};

// sanity check that estimator behaves as intended
TEST_F(VMModelEstimatorTests, add_dense_layer_test)
{
  std::string model_type = "sequential";
  std::string layer_type = "dense";

  SizeType min_input_size  = 0;
  SizeType max_input_size  = 1000;
  SizeType input_step      = 10;
  SizeType min_output_size = 0;
  SizeType max_output_size = 1000;
  SizeType output_step     = 10;

  VmPtr             vm_ptr_layer_type{new fetch::vm::String(&toolkit.vm(), layer_type)};
  fetch::vm::TypeId type_id = 0;
  VmModel           model(&toolkit.vm(), type_id, model_type);
  VmModelEstimator  model_estimator(model);

  for (SizeType inputs = min_input_size; inputs < max_input_size; inputs += input_step)
  {
    for (SizeType outputs = min_output_size; outputs < max_output_size; outputs += output_step)
    {
      DataType val = (VmModelEstimator::ADD_DENSE_INPUT_COEF() * inputs);
      val += VmModelEstimator::ADD_DENSE_OUTPUT_COEF() * outputs;
      val += VmModelEstimator::ADD_DENSE_QUAD_COEF() * inputs * outputs;
      val += VmModelEstimator::ADD_DENSE_CONST_COEF();

      EXPECT_TRUE(model_estimator.LayerAddDense(vm_ptr_layer_type, inputs, outputs) ==
                  static_cast<ChargeAmount>(val));
    }
  }
}

// sanity check that estimator behaves as intended
TEST_F(VMModelEstimatorTests, add_dense_layer_activation_test)
{
  std::string model_type      = "sequential";
  std::string layer_type      = "dense";
  std::string activation_type = "relu";

  SizeType min_input_size  = 0;
  SizeType max_input_size  = 1000;
  SizeType input_step      = 10;
  SizeType min_output_size = 0;
  SizeType max_output_size = 1000;
  SizeType output_step     = 10;

  VmPtr             vm_ptr_layer_type{new fetch::vm::String(&toolkit.vm(), layer_type)};
  VmPtr             vm_ptr_activation_type{new fetch::vm::String(&toolkit.vm(), activation_type)};
  fetch::vm::TypeId type_id = 0;
  VmModel           model(&toolkit.vm(), type_id, model_type);
  VmModelEstimator  model_estimator(model);

  for (SizeType inputs = min_input_size; inputs < max_input_size; inputs += input_step)
  {
    for (SizeType outputs = min_output_size; outputs < max_output_size; outputs += output_step)
    {
      DataType val = (VmModelEstimator::ADD_DENSE_INPUT_COEF() * inputs);
      val += VmModelEstimator::ADD_DENSE_OUTPUT_COEF() * outputs;
      val += VmModelEstimator::ADD_DENSE_QUAD_COEF() * inputs * outputs;
      val += VmModelEstimator::ADD_DENSE_CONST_COEF();

      EXPECT_TRUE(model_estimator.LayerAddDenseActivation(vm_ptr_layer_type, inputs, outputs,
                                                          vm_ptr_activation_type) ==
                  static_cast<ChargeAmount>(val));
    }
  }
}

// sanity check that estimator behaves as intended
TEST_F(VMModelEstimatorTests, add_conv_layer_test)
{
  std::string model_type = "sequential";
  std::string layer_type = "convolution1D";

  SizeType min_input_size = 0;
  SizeType max_input_size = 1000;
  SizeType input_step     = 10;

  SizeType min_output_size = 0;
  SizeType max_output_size = 1000;
  SizeType output_step     = 10;

  SizeType min_kernel_size = 0;
  SizeType max_kernel_size = 100;
  SizeType kernel_step     = 10;

  SizeType min_stride_size = 0;
  SizeType max_stride_size = 100;
  SizeType stride_step     = 10;

  VmPtr             vm_ptr_layer_type{new fetch::vm::String(&toolkit.vm(), layer_type)};
  fetch::vm::TypeId type_id = 0;
  VmModel           model(&toolkit.vm(), type_id, model_type);
  VmModelEstimator  model_estimator(model);

  for (SizeType output_channels = min_output_size; output_channels < max_output_size;
       output_channels += output_step)
  {
    for (SizeType input_channels = min_input_size; input_channels < max_input_size;
         input_channels += input_step)
    {
      for (SizeType kernel_size = min_kernel_size; kernel_size < max_kernel_size;
           kernel_size += kernel_step)
      {
        for (SizeType stride_size = min_stride_size; stride_size < max_stride_size;
             stride_size += stride_step)
        {

          EXPECT_TRUE(model_estimator.LayerAddConv(vm_ptr_layer_type, output_channels,
                                                   input_channels, kernel_size, stride_size) ==
                      static_cast<ChargeAmount>(CHARGE_INFINITY));
        }
      }
    }
  }
}

// sanity check that estimator behaves as intended
TEST_F(VMModelEstimatorTests, add_conv_layer_activation_test)
{
  std::string model_type      = "sequential";
  std::string layer_type      = "convolution1D";
  std::string activation_type = "relu";

  SizeType min_input_size = 0;
  SizeType max_input_size = 1000;
  SizeType input_step     = 10;

  SizeType min_output_size = 0;
  SizeType max_output_size = 1000;
  SizeType output_step     = 10;

  SizeType min_kernel_size = 0;
  SizeType max_kernel_size = 100;
  SizeType kernel_step     = 10;

  SizeType min_stride_size = 0;
  SizeType max_stride_size = 100;
  SizeType stride_step     = 10;

  VmPtr             vm_ptr_layer_type{new fetch::vm::String(&toolkit.vm(), layer_type)};
  VmPtr             vm_ptr_activation_type{new fetch::vm::String(&toolkit.vm(), activation_type)};
  fetch::vm::TypeId type_id = 0;
  VmModel           model(&toolkit.vm(), type_id, model_type);
  VmModelEstimator  model_estimator(model);

  for (SizeType output_channels = min_output_size; output_channels < max_output_size;
       output_channels += output_step)
  {
    for (SizeType input_channels = min_input_size; input_channels < max_input_size;
         input_channels += input_step)
    {
      for (SizeType kernel_size = min_kernel_size; kernel_size < max_kernel_size;
           kernel_size += kernel_step)
      {
        for (SizeType stride_size = min_stride_size; stride_size < max_stride_size;
             stride_size += stride_step)
        {

          EXPECT_TRUE(model_estimator.LayerAddConvActivation(vm_ptr_layer_type, output_channels,
                                                             input_channels, kernel_size,
                                                             stride_size, vm_ptr_activation_type) ==
                      static_cast<ChargeAmount>(CHARGE_INFINITY));
        }
      }
    }
  }
}

// sanity check that estimator behaves as intended
TEST_F(VMModelEstimatorTests, compile_sequential_test)
{
  std::string model_type = "sequential";
  std::string layer_type = "dense";
  std::string loss_type  = "mse";
  std::string opt_type   = "adam";

  SizeType min_input_size  = 0;
  SizeType max_input_size  = 1000;
  SizeType input_step      = 10;
  SizeType min_output_size = 0;
  SizeType max_output_size = 1000;
  SizeType output_step     = 10;

  fetch::vm::TypeId type_id = 0;

  VmPtr vm_ptr_layer_type{new fetch::vm::String(&toolkit.vm(), layer_type)};
  VmPtr vm_ptr_loss_type{new fetch::vm::String(&toolkit.vm(), loss_type)};
  VmPtr vm_ptr_opt_type{new fetch::vm::String(&toolkit.vm(), opt_type)};

  for (SizeType inputs = min_input_size; inputs < max_input_size; inputs += input_step)
  {
    for (SizeType outputs = min_output_size; outputs < max_output_size; outputs += output_step)
    {
      VmModel          model(&toolkit.vm(), type_id, model_type);
      VmModelEstimator model_estimator(model);

      // add some layers
      model_estimator.LayerAddDense(vm_ptr_layer_type, inputs, outputs);
      SizeType weights_padded_size =
          fetch::math::Tensor<DataType>::PaddedSizeFromShape({outputs, inputs});
      weights_padded_size += fetch::math::Tensor<DataType>::PaddedSizeFromShape({outputs, 1});
      SizeType weights_size_sum = inputs * outputs + outputs;

      model_estimator.LayerAddDense(vm_ptr_layer_type, inputs, outputs);
      weights_padded_size += fetch::math::Tensor<DataType>::PaddedSizeFromShape({outputs, inputs});
      weights_padded_size += fetch::math::Tensor<DataType>::PaddedSizeFromShape({outputs, 1});
      weights_size_sum += inputs * outputs + outputs;

      model_estimator.LayerAddDense(vm_ptr_layer_type, inputs, outputs);
      weights_padded_size += fetch::math::Tensor<DataType>::PaddedSizeFromShape({outputs, inputs});
      weights_padded_size += fetch::math::Tensor<DataType>::PaddedSizeFromShape({outputs, 1});
      weights_size_sum += inputs * outputs + outputs;

      DataType val = VmModelEstimator::ADAM_PADDED_WEIGHTS_SIZE_COEF() * weights_padded_size;
      val += VmModelEstimator::ADAM_WEIGHTS_SIZE_COEF() * weights_size_sum;
      val += VmModelEstimator::COMPILE_CONST_COEF();

      EXPECT_TRUE(model_estimator.CompileSequential(vm_ptr_loss_type, vm_ptr_opt_type) ==
                  static_cast<ChargeAmount>(val));
    }
  }
}

// sanity check that estimator behaves as intended
TEST_F(VMModelEstimatorTests, compile_simple_test)
{
  std::string model_type = "regressor";
  std::string opt_type   = "adam";

  SizeType min_layer_size = 0;
  SizeType max_layer_size = 5;
  SizeType layer_step     = 1;

  fetch::vm::TypeId type_id = 0;

  VmPtr vm_ptr_opt_type{new fetch::vm::String(&toolkit.vm(), opt_type)};

  for (SizeType layers = min_layer_size; layers < max_layer_size; layers += layer_step)
  {

    fetch::vm::Ptr<fetch::vm::Array<SizeType>> vm_ptr_layers{};
    VmModel                                    model(&toolkit.vm(), type_id, model_type);
    VmModelEstimator                           model_estimator(model);

    EXPECT_TRUE(model_estimator.CompileSimple(vm_ptr_opt_type, vm_ptr_layers) ==
                static_cast<ChargeAmount>(CHARGE_INFINITY));
  }
}
}  // namespace
