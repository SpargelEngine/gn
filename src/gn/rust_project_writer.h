// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOOLS_GN_RUST_PROJECT_WRITER_H_
#define TOOLS_GN_RUST_PROJECT_WRITER_H_

#include "gn/err.h"
#include "gn/target.h"

class Builder;
class BuildSettings;

class RustProjectWriter {
 public:
  // Write Rust build graph into a json file located by parameter file_name.
  //
  // Parameter quiet is not used.
  static bool RunAndWriteFiles(const BuildSettings* build_setting,
                               const Builder& builder,
                               const std::string& file_name,
                               bool quiet,
                               Err* err);
  static void RenderJSON(const BuildSettings* build_settings,
                         std::vector<const Target*>& all_targets,
                         std::ostream& rust_project);

 private:
  // This fuction visits the deps graph of a target in a DFS fashion.
  static void VisitDeps(const Target* target, std::set<const Target*>* visited);
};

#endif  // TOOLS_GN_RUST_PROJECT_WRITER_H_
