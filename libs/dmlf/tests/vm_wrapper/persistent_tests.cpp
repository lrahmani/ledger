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

#include "gtest/gtest.h"

#include "dmlf/vm_persistent.hpp"

#include "vm/io_observer_interface.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace {

using Persistent = fetch::dmlf::VmPersistent;
using Status = fetch::vm::IoObserverInterface::Status;

}  // namespace

TEST(VmDmlfTests, persistent_write)
{
  Persistent persistent;

  int val = 5;
  Status status = persistent.Write("val", &val, sizeof(val));

  EXPECT_EQ(status, Status::OK);
}

TEST(VmDmlfTests, persistent_overwrite)
{
  Persistent persistent;

  int val = 5;
  Status status = persistent.Write("val", &val, sizeof(val));
  EXPECT_EQ(status, Status::OK);

  int val2 = 7;
  Status status2 = persistent.Write("val", &val2, sizeof(val2));
  EXPECT_EQ(status2, Status::OK);
}

TEST(VmDmlfTests, persistent_writeRead)
{
  Persistent persistent;

  int val = 5;
  Status status = persistent.Write("val", &val, sizeof(val));
  EXPECT_EQ(status, Status::OK);

  int recover;
  uint64_t sz = sizeof(recover);
  Status r_status = persistent.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val);
  EXPECT_EQ(sz, sizeof(recover));
}

TEST(VmDmlfTests, persistent_overwriteRead)
{
  Persistent persistent;

  int val = 5;
  Status status = persistent.Write("val", &val, sizeof(val));
  EXPECT_EQ(status, Status::OK);

  int recover;
  uint64_t sz = sizeof(recover);
  Status r_status = persistent.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val);
  EXPECT_EQ(sz, sizeof(recover));

  val = 7;
  status = persistent.Write("val", &val, sizeof(val));
  EXPECT_EQ(status, Status::OK);

  r_status = persistent.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val);
  EXPECT_EQ(sz, sizeof(recover));
}

TEST(VmDmlfTests, persistent_bufferSize)
{
  Persistent persistent;

  int val[3] = {1,2,3};
  Status status = persistent.Write("val", &val, sizeof(val));
  EXPECT_EQ(status, Status::OK);

  int recover[3];
  uint64_t sz = sizeof(recover);
  Status r_status = persistent.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover[0], val[0]);
  EXPECT_EQ(recover[1], val[1]);
  EXPECT_EQ(recover[2], val[2]);
  EXPECT_EQ(sz, sizeof(recover));

  int larger[4] = {10,20,30,40};
  sz = sizeof(larger);
  r_status = persistent.Read("val", &larger, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(larger[0], val[0]);
  EXPECT_EQ(larger[1], val[1]);
  EXPECT_EQ(larger[2], val[2]);
  EXPECT_EQ(larger[3],     40);
  EXPECT_EQ(sz, sizeof(recover)); // 3 not 4

  int smaller[2];
  sz = sizeof(smaller);
  r_status = persistent.Read("val", &smaller, sz);
  EXPECT_EQ(r_status, Status::BUFFER_TOO_SMALL);
}

TEST(VmDmlfTests, persistent_badName)
{
  Persistent persistent;

  int val = 5;
  Status status = persistent.Write("val", &val, sizeof(val));
  EXPECT_EQ(status, Status::OK);

  Status goodExist = persistent.Exists("val");
  EXPECT_EQ(goodExist, Status::OK);
  Status badExists = persistent.Exists("hello");
  EXPECT_NE(badExists, Status::OK);

  int recover;
  uint64_t sz = sizeof(recover);
  Status r_status = persistent.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val);
  EXPECT_EQ(sz, sizeof(recover));

  Status badRead = persistent.Read("goodbye", &recover, sz);
  EXPECT_EQ(badRead, Status::PERMISSION_DENIED);
}

TEST(VmDmlfTests, persistent_deepcopy_read)
{
  Persistent persistent;

  int val = 5;
  Status status = persistent.Write("val", &val, sizeof(val));
  EXPECT_EQ(status, Status::OK);
  int val2 = 10;
  status = persistent.Write("val2", &val2, sizeof(val2));
  EXPECT_EQ(status, Status::OK);

  int recover;
  uint64_t sz = sizeof(recover);
  Status r_status = persistent.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val);
  EXPECT_EQ(sz, sizeof(recover));

  Persistent deepcopy = persistent.DeepCopy();

  sz = sizeof(recover);
  r_status = deepcopy.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val);
  EXPECT_EQ(sz, sizeof(recover));

  sz = sizeof(recover);
  r_status = deepcopy.Read("val2", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val2);
  EXPECT_EQ(sz, sizeof(recover));
}

TEST(VmDmlfTests, persistent_deepcopy_readWrite)
{
  Persistent persistent;

  int val = 5;
  Status status = persistent.Write("val", &val, sizeof(val));
  EXPECT_EQ(status, Status::OK);
  int val2 = 10;
  status = persistent.Write("val2", &val2, sizeof(val2));
  EXPECT_EQ(status, Status::OK);

  int recover;
  uint64_t sz = sizeof(recover);
  Status r_status = persistent.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val);
  EXPECT_EQ(sz, sizeof(recover));

  Persistent deepcopy = persistent.DeepCopy();

  sz = sizeof(recover);
  r_status = deepcopy.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val);
  EXPECT_EQ(sz, sizeof(recover));

  sz = sizeof(recover);
  r_status = deepcopy.Read("val2", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);

  EXPECT_EQ(recover, val2);
  EXPECT_EQ(sz, sizeof(recover));

  int newVal = 9;
  status = persistent.Write("val", &newVal, sizeof(newVal));
  EXPECT_EQ(status, Status::OK);

  sz = sizeof(recover);
  r_status = deepcopy.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);
  EXPECT_EQ(recover, val);

  sz = sizeof(recover);
  r_status = persistent.Read("val", &recover, sz);
  EXPECT_EQ(r_status, Status::OK);
  EXPECT_EQ(recover, newVal);
}

}  // namespace
