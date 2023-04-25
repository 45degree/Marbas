#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

#include "glog/logging.h"

int
main(int argc, char* argv[]) {
  ::testing::FLAGS_gmock_verbose = "error";
  FLAGS_logtostderr = true;
  FLAGS_minloglevel = 5;

  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);

  return RUN_ALL_TESTS();
}
