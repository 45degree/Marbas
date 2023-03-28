#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>

int
main(int argc, char* argv[]) {
  ::testing::FLAGS_gmock_verbose = "error";
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);

  return RUN_ALL_TESTS();
}
