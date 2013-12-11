// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/command_line.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "xwalk/runtime/common/xwalk_runtime_features.h"
#include "content/public/test/test_utils.h"

TEST(XWalkRuntimeFeaturesTest, ValidateStableFeatures) {
  CommandLine cmd(CommandLine::NO_PROGRAM);
  xwalk::XWalkRuntimeFeatures::Initialize(&cmd);
  EXPECT_TRUE(xwalk::XWalkRuntimeFeatures::isRawSocketsAPIEnabled());
  EXPECT_TRUE(xwalk::XWalkRuntimeFeatures::isDeviceCapabilitiesAPIEnabled());
}

TEST(XWalkRuntimeFeaturesTest, ValidateExperimentalFeatures) {
  CommandLine cmd(CommandLine::NO_PROGRAM);
  xwalk::XWalkRuntimeFeatures::Initialize(&cmd);
}

TEST(XWalkRuntimeFeaturesTest, CommandLineOverrideDefaults) {
  CommandLine cmd(CommandLine::NO_PROGRAM);
  cmd.AppendSwitch("--enable-raw-sockets");
  xwalk::XWalkRuntimeFeatures::Initialize(&cmd);
  EXPECT_TRUE(xwalk::XWalkRuntimeFeatures::isRawSocketsAPIEnabled());

  CommandLine cmd2 = CommandLine(CommandLine::NO_PROGRAM);
  cmd2.AppendSwitch("--disable-raw-sockets");
  xwalk::XWalkRuntimeFeatures::Initialize(&cmd2);
  EXPECT_FALSE(xwalk::XWalkRuntimeFeatures::isRawSocketsAPIEnabled());
}
