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

#include <chrono>
#include <memory>
#include <vector>

#include "dmlf/colearn/abstract_message_controller.hpp"
#include "dmlf/colearn/muddle_learner_networker_impl.hpp"
#include "dmlf/colearn/utilities.hpp"
#include "dmlf/collective_learning/utilities/typed_msg_controller_wrapper.hpp"

#include "json/document.hpp"
#include "network/uri.hpp"

namespace fetch {
namespace dmlf {
namespace collective_learning {
namespace utilities {

using MessageControllerPtr = std::shared_ptr<TypedMsgControllerlWrapper>;
using dmlf::colearn::MuddleMessageController;

inline uint16_t random_port()
{
  // TODO(LR) : use time since epoch as seed
  std::random_device rd;
  std::mt19937       eng(rd());

  uint16_t start = 10000;
  uint16_t end   = 50000;

  std::uniform_int_distribution<uint16_t> distr(start, end);
  return distr(eng);
}

inline void suppress_muddle_message_controller_logs()
{
  fetch::SetLogLevel("TCPServer", fetch::LogLevel::WARNING);
  fetch::SetLogLevel("TCPClientImpl", fetch::LogLevel::WARNING);
  fetch::SetLogLevel("Muddle:Test", fetch::LogLevel::WARNING);
  fetch::SetLogLevel("MuddlePeers:Test", fetch::LogLevel::WARNING);
  fetch::SetLogLevel("MuddlePeers:Test", fetch::LogLevel::WARNING);
  fetch::SetLogLevel("Router:Test", fetch::LogLevel::WARNING);
  fetch::SetLogLevel("DirectHandler:Test", fetch::LogLevel::WARNING);
  fetch::SetLogLevel("NetworkManager", fetch::LogLevel::WARNING);
  fetch::SetLogLevel("MuddleLearnerNetworkerImpl", fetch::LogLevel::WARNING);
  fetch::SetLogLevel("MuddleOutboundUpdateTask", fetch::LogLevel::WARNING);
}

MessageControllerPtr MakeMuddleMessageControllerFromJson(fetch::json::JSONDocument const &config,
                                                         std::size_t  instance_number,
                                                         const double broadcast_proportion = 1.0,
                                                         bool         suppress_log         = true)
{
  // logging setup
  if (suppress_log)
  {
    suppress_muddle_message_controller_logs();
  }

  // get config from json
  auto peers = config.root()["peers"];
  // rdv peer config
  auto rdv_peer = peers[0];
  auto rdv_uri  = rdv_peer["uri"].As<std::string>();
  // current peer config
  auto my_config   = peers[instance_number];
  auto self_uri    = fetch::network::Uri(my_config["uri"].As<std::string>());
  auto port        = self_uri.GetTcpPeer().port();
  auto private_key = my_config["key"].As<std::string>();

  // setup muddle
  auto muddle = std::make_shared<MuddleMessageController>(private_key, port, rdv_uri);
  muddle->set_broadcast_proportion(broadcast_proportion);

  // add all peers
  std::size_t count = peers.size();
  for (std::size_t i{0}; i < count; ++i)
  {
    if (i != instance_number)
    {
      muddle->addTarget(peers[i]["pub"].As<std::string>());
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  return std::make_shared<TypedMsgControllerlWrapper>(muddle);
}

std::vector<MessageControllerPtr> MakeLocalMuddleMessageControllersSwarm(
    std::size_t count, const double broadcast_proportion = 1.0, bool suppress_log = true)
{
  // logging setup
  if (suppress_log)
  {
    suppress_muddle_message_controller_logs();
  }

  // result
  std::vector<MessageControllerPtr> msg_ctrls;
  msg_ctrls.reserve(count);

  // setup key pairs
  std::vector<std::string> private_keys = fetch::dmlf::colearn::utilities::GenerateKeys(count);
  std::vector<std::string> public_keys =
      fetch::dmlf::colearn::utilities::ComputePublicKeys(private_keys);

  // set up the first muddle as a RDV point
  const uint16_t    RDV_PORT   = random_port();
  const std::string RDV_URI    = "tcp://127.0.0.1:" + std::to_string(RDV_PORT);
  auto              muddle_rdv = std::make_shared<dmlf::colearn::MuddleMessageController>(
      private_keys[0], RDV_PORT, std::string{});
  muddle_rdv->set_broadcast_proportion(broadcast_proportion);
  msg_ctrls.emplace_back(std::make_shared<TypedMsgControllerlWrapper>(muddle_rdv));
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  // setup the rest of message controllers using the RDV point URI
  for (std::size_t i{1}; i < count; ++i)
  {
    auto muddle =
        std::make_shared<dmlf::colearn::MuddleMessageController>(private_keys[i], 0, RDV_URI);
    muddle->set_broadcast_proportion(broadcast_proportion);
    for (std::size_t j{0}; j < count; ++j)
    {
      if (j != i)
      {
        muddle->addTarget(public_keys[j]);
      }
    }
    msg_ctrls.emplace_back(std::make_shared<TypedMsgControllerlWrapper>(muddle));
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  return msg_ctrls;
}

}  // namespace utilities
}  // namespace collective_learning
}  // namespace dmlf
}  // namespace fetch
