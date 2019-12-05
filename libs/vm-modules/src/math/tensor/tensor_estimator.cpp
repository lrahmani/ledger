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

#include "math/tensor.hpp"
#include "vm/array.hpp"
#include "vm/module.hpp"
#include "vm/object.hpp"
//#include "vm_modules/math/tensor.hpp"
#include "vm_modules/math/tensor_estimator.hpp"
#include "vm_modules/math/type.hpp"
#include "vm_modules/use_estimator.hpp"

#include <cstdint>
#include <vector>

using namespace fetch::vm;

namespace fetch {
namespace vm_modules {
namespace math {

using ArrayType  = fetch::math::Tensor<VMTensor::DataType>;
using SizeType   = ArrayType::SizeType;
using SizeVector = ArrayType::SizeVector;

VMTensor::TensorEstimator::TensorEstimator(VMTensor &tensor)
  : tensor_{tensor}
{}

ChargeAmount VMTensor::TensorEstimator::size()
{
  return low_charge;
}

ChargeAmount VMTensor::TensorEstimator::AtOne(TensorType::SizeType /*idx1*/)
{
  return low_charge;
}

ChargeAmount VMTensor::TensorEstimator::AtTwo(uint64_t /*idx1*/, uint64_t /*idx2*/)
{
  return low_charge;
}

ChargeAmount VMTensor::TensorEstimator::AtThree(uint64_t /*idx1*/, uint64_t /*idx2*/,
                                                uint64_t /*idx3*/)
{
  return low_charge;
}

ChargeAmount VMTensor::TensorEstimator::AtFour(uint64_t /*idx1*/, uint64_t /*idx2*/,
                                               uint64_t /*idx3*/, uint64_t /*idx4*/)
{
  return low_charge;
}

ChargeAmount VMTensor::TensorEstimator::SetAtOne(uint64_t /*idx1*/, DataType const & /*value*/)
{
  return low_charge;
}

ChargeAmount VMTensor::TensorEstimator::SetAtTwo(uint64_t /*idx1*/, uint64_t /*idx2*/,
                                                 DataType const & /*value*/)
{
  return low_charge;
}

ChargeAmount VMTensor::TensorEstimator::SetAtThree(uint64_t /*idx1*/, uint64_t /*idx2*/,
                                                   uint64_t /*idx3*/, DataType const & /*value*/)
{
  return low_charge;
}

ChargeAmount VMTensor::TensorEstimator::SetAtFour(uint64_t /*idx1*/, uint64_t /*idx2*/,
                                                  uint64_t /*idx3*/, uint64_t /*idx4*/,
                                                  DataType const & /*value*/)
{
  return low_charge;
}

ChargeAmount VMTensor::TensorEstimator::Fill(DataType const & /*value*/)
{
  return charge_func_of_tensor_size();
}

ChargeAmount VMTensor::TensorEstimator::FillRandom()
{
  return charge_func_of_tensor_size();
}

ChargeAmount VMTensor::TensorEstimator::Squeeze()
{
  return charge_func_of_tensor_size();
}

ChargeAmount VMTensor::TensorEstimator::Unsqueeze()
{
  return charge_func_of_tensor_size();
}

ChargeAmount VMTensor::TensorEstimator::Reshape(
    fetch::vm::Ptr<fetch::vm::Array<TensorType::SizeType>> const &new_shape)
{
  FETCH_UNUSED(new_shape);
  return charge_func_of_tensor_size();
}

ChargeAmount VMTensor::TensorEstimator::Transpose()
{
  return charge_func_of_tensor_size();
}

ChargeAmount VMTensor::TensorEstimator::FromString(fetch::vm::Ptr<fetch::vm::String> const &string)
{
  std::size_t val_size = 2;
  return static_cast<ChargeAmount>(static_cast<std::size_t>(string->Length()) / val_size);
}

ChargeAmount VMTensor::TensorEstimator::ToString()
{
  return charge_func_of_tensor_size();
}

ChargeAmount VMTensor::TensorEstimator::charge_func_of_tensor_size(std::size_t factor)
{
  return static_cast<ChargeAmount>(vm::CHARGE_UNIT * factor * tensor_.size());
}

}  // namespace math
}  // namespace vm_modules
}  // namespace fetch
