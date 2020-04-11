// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>

#include <algorithm>

#include "base/command_line.h"
#include "base/strings/stringprintf.h"
#include "gn/commands.h"
#include "gn/setup.h"
#include "gn/standard_out.h"

namespace commands {

const char kOutputs[] = "outputs";
const char kOutputs_HelpShort[] = "outputs: Which files a source/target make.";
const char kOutputs_Help[] =
    R"(gn outputs <out_dir> <list of target or file names...> *

  Lists the output files corresponding to the given target(s) or file name(s).
  There can be multiple outputs because there can be more than one output
  generated by a build step, and there can be more than one toolchain matched.
  You can also list multiple inputs which will generate a union of all the
  outputs from those inputs.

   - The input target/file names are relative to the current directory.

   - The output file names are relative to the root build directory.

   This command is useful for finding a ninja command that will build only a
   portion of the build.

Target outputs

  If the parameter is a target name that includes a toolchain, it will match
  only that target in that toolchain. If no toolchain is specified, it will
  match all targets with that name in any toolchain.

  The result will be the outputs specified by that target which could be a
  library, executable, output of an action, a stamp file, etc.

File outputs

  If the parameter is a file name it will compute the output for that compile
  step for all targets in all toolchains that contain that file as a source
  file.

  If the source is not compiled (e.g. a header or text file), the command will
  produce no output.

  If the source is listed as an "input" to a binary target or action will
  resolve to that target's outputs.

Example

  gn outputs out/debug some/directory:some_target
      Find the outputs of a given target.

  gn outputs out/debug src/project/my_file.cc | xargs ninja -C out/debug
      Compiles just the given source file in all toolchains it's referenced in.

  git diff --name-only | xargs gn outputs out/x64 | xargs ninja -C out/x64
      Compiles all files changed in git.
)";

int RunOutputs(const std::vector<std::string>& args) {
  if (args.size() < 2) {
    Err(Location(),
        "Expected a build dir and one or more input files or targets.\n"
        "Usage: \"gn outputs <out_dir> <target-or-file>*\"")
        .PrintToStdout();
    return 1;
  }

  // Deliberately leaked to avoid expensive process teardown.
  Setup* setup = new Setup;
  if (!setup->DoSetup(args[0], false))
    return 1;
  if (!setup->Run())
    return 1;

  std::vector<std::string> inputs(args.begin() + 1, args.end());

  UniqueVector<const Target*> target_matches;
  UniqueVector<const Config*> config_matches;
  UniqueVector<const Toolchain*> toolchain_matches;
  UniqueVector<SourceFile> file_matches;
  if (!ResolveFromCommandLineInput(setup, inputs, false, PATTERN_MATCH_TARGET,
                                   &target_matches, &config_matches,
                                   &toolchain_matches, &file_matches))
    return 1;

  // We only care about targets and files.
  if (target_matches.empty() && file_matches.empty()) {
    Err(Location(), "The input matched no targets or files.").PrintToStdout();
    return 1;
  }

  // Resulting outputs.
  std::vector<OutputFile> outputs;

  // Files. This must go first because it may add to the "targets" list.
  std::vector<const Target*> all_targets =
      setup->builder().GetAllResolvedTargets();
  for (const SourceFile& file : file_matches) {
    std::vector<TargetContainingFile> targets;
    GetTargetsContainingFile(setup, all_targets, file, false, &targets);
    if (targets.empty()) {
      Err(Location(), base::StringPrintf("No targets reference the file '%s'.",
                                         file.value().c_str()))
          .PrintToStdout();
      return 1;
    }

    // There can be more than one target that references this file, evaluate the
    // output name in all of them.
    for (const TargetContainingFile& pair : targets) {
      if (pair.second == HowTargetContainsFile::kInputs) {
        // Inputs maps to the target itself. This will be evaluated below.
        target_matches.push_back(pair.first);
      } else if (pair.second == HowTargetContainsFile::kSources) {
        // Source file, check it.
        const char* computed_tool = nullptr;
        std::vector<OutputFile> file_outputs;
        pair.first->GetOutputFilesForSource(file, &computed_tool,
                                            &file_outputs);
        outputs.insert(outputs.end(), file_outputs.begin(), file_outputs.end());
      }
    }
  }

  // Targets.
  for (const Target* target : target_matches) {
    std::vector<SourceFile> output_files;
    Err err;
    if (!target->GetOutputsAsSourceFiles(LocationRange(), true, &output_files,
                                         &err)) {
      err.PrintToStdout();
      return 1;
    }

    // Convert to OutputFiles.
    for (const SourceFile& file : output_files)
      outputs.emplace_back(&setup->build_settings(), file);
  }

  // Print.
  for (const OutputFile& output_file : outputs)
    printf("%s\n", output_file.value().c_str());
  return 0;
}

}  // namespace commands
