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

#include "dmlf/collective_learning/collective_learning_client.hpp"
#include "dmlf/collective_learning/utilities/mnist_client_utilities.hpp"
#include "dmlf/collective_learning/utilities/utilities.hpp"
#include "dmlf/networkers/local_learner_networker.hpp"
#include "dmlf/simple_cycling_algorithm.hpp"
#include "dmlf/colearn/muddle_learner_networker_impl.hpp"
#include "dmlf/colearn/utils.hpp"
#include "json/document.hpp"
#include "math/tensor.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace fetch::ml::ops;
using namespace fetch::ml::layers;
using namespace fetch::dmlf::collective_learning;

using DataType         = fetch::fixed_point::FixedPoint<32, 32>;
using TensorType       = fetch::math::Tensor<DataType>;
using VectorTensorType = std::vector<TensorType>;
using SizeType         = fetch::math::SizeType;
using LearnerNetworkerImpl = fetch::dmlf::colearn::MuddleLearnerNetworkerImpl;

int main(int argc, char **argv)
{
  // This example will create multiple local distributed clients with simple classification neural
  // net and learns how to predict hand written digits from MNIST dataset

  if (argc != 2)
  {
    std::cout << "Usage : " << argv[0] << " config_file.json" << std::endl;
    return 1;
  }

  std::cout << "FETCH Distributed MNIST Demo" << std::endl;

  // handle config params
  fetch::json::JSONDocument                                doc;
  fetch::dmlf::collective_learning::ClientParams<DataType> client_params =
      fetch::dmlf::collective_learning::utilities::ClientParamsFromJson<TensorType>(
          std::string(argv[1]), doc);
  auto data_file      = doc["data"].As<std::string>();
  auto labels_file    = doc["labels"].As<std::string>();
  auto n_clients      = doc["n_clients"].As<SizeType>();
  auto n_peers        = doc["n_peers"].As<SizeType>();
  auto n_rounds       = doc["n_rounds"].As<SizeType>();
  auto synchronise    = doc["synchronise"].As<bool>();
  auto test_set_ratio = doc["test_set_ratio"].As<float>();
  
  FETCH_UNUSED(n_peers);

  std::shared_ptr<std::mutex> console_mutex_ptr = std::make_shared<std::mutex>();

  // Set up networkers
  fetch::SetGlobalLogLevel(fetch::LogLevel::ERROR);
  std::vector<std::shared_ptr<LearnerNetworkerImpl>> networkers(n_clients);
  std::vector<std::string> private_keys = fetch::dmlf::colearn::utils::GenerateKeys(n_clients);
  std::vector<std::string> public_keys = fetch::dmlf::colearn::utils::ComputePublicKeys(private_keys);
  const double BROADCAST_PROPORTION{1.0};

  // set up the first networker as a RDV point
  const uint16_t RDV_PORT{15005};
  const std::string RDV_URI = "tcp://127.0.0.1:" + std::to_string(RDV_PORT);
  networkers.at(0) = std::make_shared<LearnerNetworkerImpl>(private_keys[0], RDV_PORT, std::string{});
  networkers.at(0)->ColearnRegisterUpdateType<fetch::dmlf::Update<TensorType>>("gradients");

  // set up the rest of the networkers
  for (SizeType i(1); i < n_clients; ++i)
  {
    networkers.at(i) = std::make_shared<LearnerNetworkerImpl>(private_keys[i], 0, RDV_URI);
    //networkers.at(i)->Initialize<fetch::dmlf::Update<TensorType>>();
    networkers.at(i)->ColearnRegisterUpdateType<fetch::dmlf::Update<TensorType>>("gradients");
  }
  for (SizeType i(0); i < n_clients; ++i)
  {
    for(SizeType j(0); j < n_clients; ++j)
    {
      if(j!=i)
      {
        networkers.at(i)->addTarget(public_keys[j]);
      }
    }
    networkers.at(i)->set_broadcast_proportion(BROADCAST_PROPORTION);
  }

  // Create training clients
  std::vector<std::shared_ptr<CollectiveLearningClient<TensorType>>> clients(n_clients);
  for (SizeType i{0}; i < n_clients; ++i)
  {
    clients.at(i) = fetch::dmlf::collective_learning::utilities::MakeMNISTClient<TensorType>(
        std::to_string(i), client_params, data_file, labels_file, test_set_ratio, networkers.at(i),
        console_mutex_ptr);
  }

  /**
   * Main loop
   */

  for (SizeType it{0}; it < n_rounds; ++it)
  {
    // Start all clients
    std::cout << "================= ROUND : " << it << " =================" << std::endl;
    std::vector<std::thread> threads;
    for (auto &c : clients)
    {
      c->RunAlgorithms(threads);
    }

    // Wait for everyone to finish
    for (auto &t : threads)
    {
      t.join();
    }

    // Synchronize weights by giving all clients average of all client's weights
    if (synchronise)
    {
      std::cout << std::endl << "Synchronising weights" << std::endl;
      fetch::dmlf::collective_learning::utilities::SynchroniseWeights<TensorType>(clients);
    }
  }

  return 0;
}
