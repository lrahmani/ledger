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

#include <memory>

#include "core/mutex.hpp"

#include "core/byte_array/byte_array.hpp"
#include "core/byte_array/const_byte_array.hpp"
#include "dmlf/colearn/update_store_interface.hpp"
#include "dmlf/queue.hpp"
#include "dmlf/queue_interface.hpp"
#include "dmlf/shuffle_algorithm_interface.hpp"
#include "dmlf/type_map.hpp"

namespace fetch {
namespace dmlf {

class AbstractLearnerNetworker
{
public:
  using Bytes = byte_array::ByteArray;  
  using BytesConst = byte_array::ConstByteArray;

  AbstractLearnerNetworker()                                      = default;
  virtual ~AbstractLearnerNetworker()                             = default;
  AbstractLearnerNetworker(AbstractLearnerNetworker const &other) = delete;
  AbstractLearnerNetworker &operator=(AbstractLearnerNetworker const &other)  = delete;
  bool                      operator==(AbstractLearnerNetworker const &other) = delete;
  bool                      operator<(AbstractLearnerNetworker const &other)  = delete;

  // To implement
  virtual void        PushUpdate(UpdateInterfacePtr const &update) = 0;
  virtual std::size_t GetPeerCount() const                         = 0;

  template <typename T>
  void Initialize()
  {
    FETCH_LOCK(queue_m_);
    if (!queue_)
    {
      queue_ = std::make_shared<Queue<T>>();
      return;
    }
    throw std::runtime_error{"Learner already initialized"};
  }

  virtual std::size_t GetUpdateCount() const;

  template <typename T>
  std::shared_ptr<T> GetUpdate()
  {
    FETCH_LOCK(queue_m_);
    ThrowIfNotInitialized();
    auto que = std::dynamic_pointer_cast<Queue<T>>(queue_);
    return que->GetUpdate();
  }

  virtual void SetShuffleAlgorithm(const std::shared_ptr<ShuffleAlgorithmInterface> &alg);

  virtual void PushUpdateType(const std::string & /*key*/, UpdateInterfacePtr const & /*update*/);

  template <typename T>
  void RegisterUpdateType(std::string key)
  {
    FETCH_LOCK(queue_map_m_);
    update_types_.template put<T>(key);
    queue_map_[key] = std::make_shared<Queue<T>>();
  }

  template <typename T>
  std::size_t GetUpdateTypeCount() const
  {
    FETCH_LOCK(queue_map_m_);
    auto key = update_types_.template find<T>();
    return queue_map_.at(key)->size();
  }

  std::size_t GetUpdateTypeCount(const std::string &key) const;

  Bytes GetUpdateAsBytes(const std::string &key);

  using Score      = colearn::UpdateStoreInterface::Score;
  using Criteria   = colearn::UpdateStoreInterface::Criteria;
  using UpdateType = colearn::UpdateStoreInterface::UpdateType;
  using UpdatePtr  = colearn::UpdateStoreInterface::UpdatePtr;
  using Algorithm  = colearn::UpdateStoreInterface::Algorithm;

  virtual UpdatePtr GetUpdate(Algorithm const &algo, UpdateType const &type,
                              Criteria const &criteria);

  template <typename T>
  std::shared_ptr<T> GetUpdateType()
  {
    FETCH_LOCK(queue_map_m_);
    auto key  = update_types_.template find<T>();
    auto iter = queue_map_.find(key);
    auto que  = std::dynamic_pointer_cast<Queue<T>>(iter->second);
    return que->GetUpdate();
  }
  
  template <typename T>
  void ColearnPushUpdate(std::shared_ptr<T> const &update)
  {
    std::string upd_type{};
    {
      FETCH_LOCK(queue_map_m_);
      upd_type  = update_types_.template find<T>();
    }

    auto data = Serialize(update);

    ColearnPushUpdateImplem(upd_type, data); 
  }

  template <typename T>
  void ColearnRegisterUpdateType(std::string const& update_type)
  {
    FETCH_LOCK(queue_map_m_);
    update_types_.template put<T>(update_type);
  }
  
  template <typename T>
  std::size_t ColearnGetUpdateCount() const
  {
    std::string upd_type{};
    {
      FETCH_LOCK(queue_map_m_);
      upd_type  = update_types_.template find<T>();
    }

    return ColearnGetUpdateCountImplem(upd_type);
  }

  template <typename T>
  std::shared_ptr<T> ColearnGetUpdate()
  {
    std::string upd_type{};
    {
      FETCH_LOCK(queue_map_m_);
      upd_type = update_types_.template find<T>();
    }

    BytesConst bytes = ColearnGetUpdateImplem(upd_type);
    return Deserialize<T>(bytes);    
  }

protected:
  std::shared_ptr<ShuffleAlgorithmInterface> alg_;  // used by descendents

  virtual void NewMessage(Bytes const &msg);                             // called by descendents
  virtual void NewDmlfMessage(Bytes const &msg);                         // called by descendents
  virtual void NewMessage(const std::string &key, Bytes const &update);  // called by descendents
  
  virtual std::size_t ColearnGetUpdateCountImplem(std::string const &/*update_type*/) const
  {
    throw std::runtime_error("ColearnGetUpdateCountImplem must be implemented by descendents");
  }

  virtual BytesConst  ColearnGetUpdateImplem(std::string const &/*update_type*/)
  {
    throw std::runtime_error("ColearnGetUpdateImplem must be implemented by descendents");
  }

  virtual void        ColearnPushUpdateImplem(std::string const &/*update_type*/, BytesConst const &/*data*/)
  {
    throw std::runtime_error("ColearnPushUpdateImplem must be implemented by descendents");
  }

  template <typename T>
  Bytes Serialize(std::shared_ptr<T> const &update)
  {
    fetch::serializers::MsgPackSerializer serializer;
    serializer << *update;
    return serializer.data();
  }

  template <typename T>
  std::shared_ptr<T> Deserialize(BytesConst const &bytes)
  {
    auto update = std::make_shared<T>();
    fetch::serializers::MsgPackSerializer deserializer{bytes};
    deserializer >> *update;
    return update;
  }

private:
  using Mutex             = fetch::Mutex;
  using Lock              = std::unique_lock<Mutex>;
  using QueueInterfacePtr = std::shared_ptr<QueueInterface>;
  using QueueInterfaceMap = std::unordered_map<std::string, QueueInterfacePtr>;

  QueueInterfacePtr queue_;
  mutable Mutex     queue_m_;
  QueueInterfaceMap queue_map_;
  mutable Mutex     queue_map_m_;

  TypeMap<> update_types_;

  void ThrowIfNotInitialized() const;
};

}  // namespace dmlf
}  // namespace fetch
