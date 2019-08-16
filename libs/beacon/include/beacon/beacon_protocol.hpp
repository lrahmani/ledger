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

#include "core/service_ids.hpp"
#include "network/service/protocol.hpp"

namespace fetch {
namespace beacon {
class BeaconService;

class BeaconServiceProtocol : public service::Protocol
{
public:
  enum
  {
    SUBMIT_SIGNATURE_SHARE = 1
  };

  // Construction / Destruction
  explicit BeaconServiceProtocol(BeaconService &service);

  BeaconServiceProtocol(BeaconServiceProtocol const &) = delete;
  BeaconServiceProtocol(BeaconServiceProtocol &&)      = delete;
  ~BeaconServiceProtocol() override                    = default;

  // Operators
  BeaconServiceProtocol &operator=(BeaconServiceProtocol const &) = delete;
  BeaconServiceProtocol &operator=(BeaconServiceProtocol &&) = delete;

private:
  BeaconService &service_;
};
}  // namespace beacon
}  // namespace fetch