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

#include <cstdint>

namespace fetch {
namespace dmlf {

class IUpdate
{
public:
  using TimeStampType = std::uint64_t; 

  IUpdate()
  {
  }
  
  // API
  virtual byte_array::ByteArray serialise()        = 0;
  virtual void deserialise(byte_array::ByteArray&) = 0;
  virtual TimeStampType TimeStamp() const          = 0; 
  
  // Queue ordering
  bool operator>(const IUpdate& other) const
  {
    return TimeStamp() > other.TimeStamp();
  }

  virtual ~IUpdate()
  {
  }
protected:
private:
  IUpdate(const IUpdate &other) = delete;
  IUpdate &operator=(const IUpdate &other) = delete;
  bool operator==(const IUpdate &other) = delete;
  bool operator<(const IUpdate &other) = delete;
};

}  // namespace dmlf
}  // namespace fetch
