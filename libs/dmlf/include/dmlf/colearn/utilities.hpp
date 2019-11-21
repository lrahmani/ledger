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
#include "core/byte_array/const_byte_array.hpp"
#include "crypto/ecdsa.hpp"

namespace fetch {
namespace dmlf {
namespace colearn {
namespace utilities {

inline std::string GenerateKey()
{
  crypto::ECDSASigner key{};
  return std::string{fetch::byte_array::ToBase64(key.private_key())};
}

inline std::vector<std::string> GenerateKeys(std::size_t count)
{
  std::vector<std::string> output;
  output.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    output.emplace_back(GenerateKey());
  }
  return output;
}

inline std::string ComputePublicKey(std::string const &private_key)
{
  crypto::ECDSASigner key{};
  key.Load(fetch::byte_array::FromBase64(private_key));
  return std::string{fetch::byte_array::ToBase64(key.public_key())};
}

inline std::vector<std::string> ComputePublicKeys(std::vector<std::string> const &private_keys)
{
  size_t                   size = private_keys.size();
  std::vector<std::string> public_keys;
  public_keys.reserve(size);
  for (size_t i = 0; i < size; ++i)
  {
    public_keys.emplace_back(ComputePublicKey(private_keys[i]));
  }
  return public_keys;
}

}  // namespace utilities
}  // namespace colearn
}  // namespace dmlf
}  // namespace fetch
