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

#include "dmlf/colearn/abstract_message_controller.hpp"
#include "dmlf/collective_learning/client_algorithm.hpp"
#include "dmlf/collective_learning/utilities/boston_housing_client_utilities.hpp"
#include "dmlf/collective_learning/utilities/muddle_message_controller_utilities.hpp"
#include "dmlf/collective_learning/utilities/typed_msg_controller_wrapper.hpp"
#include "dmlf/collective_learning/utilities/utilities.hpp"
#include "json/document.hpp"
#include "math/tensor.hpp"
#include "math/utilities/ReadCSV.hpp"
#include "ml/dataloaders/tensor_dataloader.hpp"
#include "ml/exceptions/exceptions.hpp"
#include <algorithm>
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
using MessageControllerPtr =
    std::shared_ptr<fetch::dmlf::collective_learning::utilities::TypedMsgControllerlWrapper>;

int main(int argc, char **argv)
{
  // This example will create muddle networking distributed client with simple regression neural net
  // and learns how to predict prices from Boston Housing dataset

  if (argc != 4)
  {
    std::cout << "learner_config.json networker_config instance_number" << std::endl;
    return 1;
  }

  auto networker_config = fetch::json::JSONDocument(std::string(argv[2]));
  int  instance_number  = std::atoi(argv[3]);

  fetch::json::JSONDocument                                doc;
  fetch::dmlf::collective_learning::ClientParams<DataType> client_params =
      fetch::dmlf::collective_learning::utilities::ClientParamsFromJson<TensorType>(
          std::string(argv[1]), doc);

  auto data_file      = doc["data"].As<std::string>();
  auto labels_file    = doc["labels"].As<std::string>();
  auto results_dir    = doc["results"].As<std::string>();
  auto n_peers        = doc["n_peers"].As<SizeType>();
  auto n_rounds       = doc["n_rounds"].As<SizeType>();
  auto seed           = doc["random_seed"].As<SizeType>();
  auto test_set_ratio = doc["test_set_ratio"].As<float>();

  FETCH_UNUSED(n_peers);

  /**
   * Prepare environment
   */

  std::shared_ptr<std::mutex> console_mutex_ptr = std::make_shared<std::mutex>();

  // Load data

  TensorType data_tensor  = fetch::math::utilities::ReadCSV<TensorType>(data_file);
  TensorType label_tensor = fetch::math::utilities::ReadCSV<TensorType>(labels_file);

  // Shuffle data
  utilities::Shuffle(data_tensor, label_tensor, seed);

  // Create networker
  MessageControllerPtr networker =
      fetch::dmlf::collective_learning::utilities::MakeMuddleMessageControllerFromJson(
          networker_config);

  // Create learning client
  auto client = fetch::dmlf::collective_learning::utilities::MakeBostonClient<TensorType>(
      std::to_string(instance_number), client_params, data_tensor, label_tensor, test_set_ratio,
      networker, console_mutex_ptr);

  /**
   * Main loop
   */

  for (SizeType it{0}; it < n_rounds; ++it)
  {
    std::cout << "================= ROUND : " << it << " =================" << std::endl;

    // Start client
    client->RunAlgorithms();
  }

  return 0;
}
