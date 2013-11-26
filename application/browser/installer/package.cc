// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xwalk/application/browser/installer/package.h"

#include "base/file_util.h"
#include "base/logging.h"
#include "crypto/signature_verifier.h"
#include "xwalk/application/common/id_util.h"

namespace xwalk {
namespace application {


Package::Package(ScopedStdioHandle* file, bool is_valid)
  : file_(file),
    is_valid_(is_valid) {
}

Package::~Package() {
}

// static
scoped_ptr<Package> Package::Create(const base::FilePath& source_path) {
  // TODO(riju): create specific package type
  LOG(ERROR) << "Invalid package type";
  return scoped_ptr<Package>();
}

}  // namespace application
}  // namespace xwalk
