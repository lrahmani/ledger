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

#include "math/tensor.hpp"
#include "vm/common.hpp"
#include "vm/object.hpp"
#include "vm_modules/math/tensor/tensor_estimator.hpp"
#include "vm_modules/math/type.hpp"

#include <cstdint>
#include <vector>

namespace fetch {

namespace math {
template <typename, typename>
class Tensor;
}
namespace vm {
class Module;
template <typename T>
struct Array;
}  // namespace vm

namespace vm_modules {
namespace math {

class VMTensor : public fetch::vm::Object
{
public:
  using DataType   = fetch::vm_modules::math::DataType;
  using TensorType = typename fetch::math::Tensor<DataType>;

  VMTensor(fetch::vm::VM *vm, fetch::vm::TypeId type_id, std::vector<uint64_t> const &shape);

  VMTensor(fetch::vm::VM *vm, fetch::vm::TypeId type_id, TensorType tensor);

  VMTensor(fetch::vm::VM *vm, fetch::vm::TypeId type_id);

  static fetch::vm::Ptr<VMTensor> Constructor(
      fetch::vm::VM *vm, fetch::vm::TypeId type_id,
      fetch::vm::Ptr<fetch::vm::Array<TensorType::SizeType>> const &shape);

  static void Bind(fetch::vm::Module &module, bool enable_experimental);

  TensorType::SizeVector shape() const;

  TensorType::SizeType size() const;

  vm::Ptr<vm::Array<TensorType::SizeType>> VMShape() const;

  ////////////////////////////////////
  /// ACCESSING AND SETTING VALUES ///
  ////////////////////////////////////

  template <typename... Indices>
  DataType At(Indices... indices) const;

  template <typename... Args>
  void SetAt(Args... args);

  vm::Ptr<VMTensor> Copy();

  void Fill(DataType const &value);

  void FillRandom();

  /////////////////
  /// RESHAPING ///
  /////////////////

  fetch::vm::Ptr<VMTensor> Squeeze() const;

  fetch::vm::Ptr<VMTensor> Unsqueeze() const;

  bool Reshape(fetch::vm::Ptr<fetch::vm::Array<TensorType::SizeType>> const &new_shape);

  fetch::vm::Ptr<VMTensor> Transpose() const;

  ////////////////////////
  /// BASIC COMPARISON ///
  ////////////////////////

  bool IsEqualOperator(vm::Ptr<VMTensor> const &other);

  bool IsNotEqualOperator(vm::Ptr<VMTensor> const &other);

  vm::Ptr<VMTensor> NegateOperator();

  // TODO (ML-340) - Below Operators should be bound and above operators removed when operators can
  // take estimators

  bool IsEqual(vm::Ptr<Object> const &lhso, vm::Ptr<Object> const &rhso) override;

  bool IsNotEqual(vm::Ptr<Object> const &lhso, vm::Ptr<Object> const &rhso) override;

  void Negate(vm::Ptr<Object> &object) override;

  ////////////////////////
  /// BASIC ARITHMETIC ///
  ////////////////////////

  vm::Ptr<VMTensor> AddOperator(vm::Ptr<VMTensor> const &other);

  vm::Ptr<VMTensor> SubtractOperator(vm::Ptr<VMTensor> const &other);

  vm::Ptr<VMTensor> MultiplyOperator(vm::Ptr<VMTensor> const &other);

  vm::Ptr<VMTensor> DivideOperator(vm::Ptr<VMTensor> const &other);

  // TODO (ML-340) - Below Operators should be bound and above operators removed when operators can
  // take estimators

  void Add(vm::Ptr<Object> &lhso, vm::Ptr<Object> &rhso) override;

  void Subtract(vm::Ptr<Object> &lhso, vm::Ptr<Object> &rhso) override;

  void InplaceAdd(vm::Ptr<Object> const &lhso, vm::Ptr<Object> const &rhso) override;

  void InplaceSubtract(vm::Ptr<Object> const &lhso, vm::Ptr<Object> const &rhso) override;

  void Multiply(vm::Ptr<Object> &lhso, vm::Ptr<Object> &rhso) override;

  void Divide(vm::Ptr<Object> &lhso, vm::Ptr<Object> &rhso) override;

  void InplaceMultiply(vm::Ptr<Object> const &lhso, vm::Ptr<Object> const &rhso) override;

  void InplaceDivide(vm::Ptr<Object> const &lhso, vm::Ptr<Object> const &rhso) override;

  /////////////////////////
  /// MATRIX OPERATIONS ///
  /////////////////////////

  DataType Min();

  DataType Max();

  DataType Sum();

  vm::Ptr<VMTensor> ArgMax(SizeType const &indices = SizeType{0});

  vm::Ptr<VMTensor> ArgMaxNoIndices();

  vm::Ptr<VMTensor> Dot(vm::Ptr<VMTensor> const &other);

  //////////////////////////////
  /// PRINTING AND EXPORTING ///
  //////////////////////////////

  void FromString(fetch::vm::Ptr<fetch::vm::String> const &string);

  fetch::vm::Ptr<fetch::vm::String> ToString() const;

  TensorType &GetTensor();

  TensorType const &GetConstTensor();

  bool SerializeTo(serializers::MsgPackSerializer &buffer) override;

  bool DeserializeFrom(serializers::MsgPackSerializer &buffer) override;

  TensorEstimator &Estimator();

  static const std::size_t RECTANGULAR_SHAPE_SIZE = 2;

private:
  TensorType      tensor_;
  TensorEstimator estimator_;
};

}  // namespace math
}  // namespace vm_modules
}  // namespace fetch
