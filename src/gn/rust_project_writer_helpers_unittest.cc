// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gn/rust_project_writer_helpers.h"

#include "gn/string_output_buffer.h"
#include "gn/test_with_scheduler.h"
#include "gn/test_with_scope.h"
#include "util/build_config.h"
#include "util/test/test.h"

using RustProjectWriterHelper = TestWithScheduler;

TEST_F(RustProjectWriterHelper, WriteCrates) {
  TestWithScope setup;

  CrateList crates;
  Crate dep = Crate(SourceFile("tortoise/lib.rs"), 0, "//tortoise:bar", "2015");
  Crate target = Crate(SourceFile("hare/lib.rs"), 1, "//hare:bar", "2015");
  target.AddDependency(0, "tortoise");
  target.AddConfigItem("unix");
  target.AddConfigItem("feature=\\\"test\\\"");

  crates.push_back(dep);
  crates.push_back(target);

  std::ostringstream stream;
  WriteCrates(setup.build_settings(), crates, stream);
  std::string out = stream.str();
#if defined(OS_WIN)
  base::ReplaceSubstringsAfterOffset(&out, 0, "\r\n", "\n");
#endif
  const char expected_json[] =
      "{\n"
      "  \"roots\": [\n"
      "    \"tortoise/\",\n"
      "    \"hare/\"\n"
      "  ],\n"
      "  \"crates\": [\n"
      "    {\n"
      "      \"crate_id\": 0,\n"
      "      \"root_module\": \"tortoise/lib.rs\",\n"
      "      \"label\": \"//tortoise:bar\",\n"
      "      \"deps\": [\n"
      "      ],\n"
      "      \"edition\": \"2015\",\n"
      "      \"cfg\": [\n"
      "      ]\n"
      "    },\n"
      "    {\n"
      "      \"crate_id\": 1,\n"
      "      \"root_module\": \"hare/lib.rs\",\n"
      "      \"label\": \"//hare:bar\",\n"
      "      \"deps\": [\n"
      "        {\n"
      "          \"crate\": 0,\n"
      "          \"name\": \"tortoise\"\n"
      "        }\n"
      "      ],\n"
      "      \"edition\": \"2015\",\n"
      "      \"cfg\": [\n"
      "        \"unix\",\n"
      "        \"feature=\\\"test\\\"\"\n"
      "      ]\n"
      "    }\n"
      "  ]\n"
      "}\n";

  EXPECT_EQ(expected_json, out);
}

TEST_F(RustProjectWriterHelper, SysrootDepsAreCorrect) {
  TestWithScope setup;

  SysrootIndexMap sysroot_lookup;
  CrateList crates;

  AddSysroot(setup.build_settings(), "path", sysroot_lookup, crates);

  std::ostringstream stream;
  WriteCrates(setup.build_settings(), crates, stream);
  std::string out = stream.str();
#if defined(OS_WIN)
  base::ReplaceSubstringsAfterOffset(&out, 0, "\r\n", "\n");
#endif

const char expected_json[] =
    "{\n"
    "  \"roots\": [\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libcore/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/liballoc/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libpanic_abort/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libunwind/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libstd/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libcollections/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/liblibc/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libpanic_unwind/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libproc_macro/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/librustc_unicode/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libstd_unicode/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libtest/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/liballoc_jemalloc/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/liballoc_system/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libcompiler_builtins/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libgetopts/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libbuild_helper/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/librustc_asan/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/librustc_lsan/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/librustc_msan/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/librustc_tsan/\",\n"
    "    \"out/Debug/path/lib/rustlib/src/rust/src/libsyntax/\"\n"
    "  ],\n"
    "  \"crates\": [\n"
    "    {\n"
    "      \"crate_id\": 0,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libcore/lib.rs\",\n"
    "      \"label\": \"core\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 1,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/liballoc/lib.rs\",\n"
    "      \"label\": \"alloc\",\n"
    "      \"deps\": [\n"
    "        {\n"
    "          \"crate\": 0,\n"
    "          \"name\": \"core\"\n"
    "        }\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 2,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libpanic_abort/lib.rs\",\n"
    "      \"label\": \"panic_abort\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 3,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libunwind/lib.rs\",\n"
    "      \"label\": \"unwind\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 4,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libstd/lib.rs\",\n"
    "      \"label\": \"std\",\n"
    "      \"deps\": [\n"
    "        {\n"
    "          \"crate\": 1,\n"
    "          \"name\": \"alloc\"\n"
    "        },\n"
    "        {\n"
    "          \"crate\": 0,\n"
    "          \"name\": \"core\"\n"
    "        },\n"
    "        {\n"
    "          \"crate\": 2,\n"
    "          \"name\": \"panic_abort\"\n"
    "        },\n"
    "        {\n"
    "          \"crate\": 3,\n"
    "          \"name\": \"unwind\"\n"
    "        }\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 5,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libcollections/lib.rs\",\n"
    "      \"label\": \"collections\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 6,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/liblibc/lib.rs\",\n"
    "      \"label\": \"libc\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 7,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libpanic_unwind/lib.rs\",\n"
    "      \"label\": \"panic_unwind\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 8,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libproc_macro/lib.rs\",\n"
    "      \"label\": \"proc_macro\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 9,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/librustc_unicode/lib.rs\",\n"
    "      \"label\": \"rustc_unicode\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 10,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libstd_unicode/lib.rs\",\n"
    "      \"label\": \"std_unicode\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 11,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libtest/lib.rs\",\n"
    "      \"label\": \"test\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 12,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/liballoc_jemalloc/lib.rs\",\n"
    "      \"label\": \"alloc_jemalloc\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 13,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/liballoc_system/lib.rs\",\n"
    "      \"label\": \"alloc_system\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 14,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libcompiler_builtins/lib.rs\",\n"
    "      \"label\": \"compiler_builtins\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 15,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libgetopts/lib.rs\",\n"
    "      \"label\": \"getopts\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 16,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libbuild_helper/lib.rs\",\n"
    "      \"label\": \"build_helper\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 17,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/librustc_asan/lib.rs\",\n"
    "      \"label\": \"rustc_asan\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 18,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/librustc_lsan/lib.rs\",\n"
    "      \"label\": \"rustc_lsan\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 19,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/librustc_msan/lib.rs\",\n"
    "      \"label\": \"rustc_msan\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 20,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/librustc_tsan/lib.rs\",\n"
    "      \"label\": \"rustc_tsan\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    },\n"
    "    {\n"
    "      \"crate_id\": 21,\n"
    "      \"root_module\": \"out/Debug/path/lib/rustlib/src/rust/src/libsyntax/lib.rs\",\n"
    "      \"label\": \"syntax\",\n"
    "      \"deps\": [\n"
    "      ],\n"
    "      \"edition\": \"2018\",\n"
    "      \"cfg\": [\n"
    "        \"debug_assertion\"\n"
    "      ]\n"
    "    }\n"
    "  ]\n"
    "}\n";
;
  EXPECT_EQ(expected_json, out);
}

TEST_F(RustProjectWriterHelper, ExtractCompilerTargetTupleSimple) {
  TestWithScope setup;

  Target target(setup.settings(), Label(SourceDir("//foo/"), "bar"));
  target.set_output_type(Target::RUST_LIBRARY);
  target.visibility().SetPublic();
  SourceFile lib("//foo/lib.rs");
  target.sources().push_back(lib);
  target.source_types_used().Set(SourceFile::SOURCE_RS);
  target.rust_values().set_crate_root(lib);
  target.rust_values().crate_name() = "foo";
  target.config_values().rustflags().push_back("--cfg=feature=\"foo_enabled\"");
  target.config_values().rustflags().push_back("--target");
  target.config_values().rustflags().push_back("x86-someos");
  target.config_values().rustflags().push_back("--edition=2018");

  std::optional<std::string> result = ExtractCompilerTargetTriple(&target);
  auto expected = std::optional<std::string>{"x86-someos"};
  EXPECT_EQ(expected, result);
}

TEST_F(RustProjectWriterHelper, ExtractCompilerTargetTupleMissing) {
  TestWithScope setup;

  Target target(setup.settings(), Label(SourceDir("//foo/"), "bar"));
  target.set_output_type(Target::RUST_LIBRARY);
  target.visibility().SetPublic();
  SourceFile lib("//foo/lib.rs");
  target.sources().push_back(lib);
  target.source_types_used().Set(SourceFile::SOURCE_RS);
  target.rust_values().set_crate_root(lib);
  target.rust_values().crate_name() = "foo";
  target.config_values().rustflags().push_back("--cfg=feature=\"foo_enabled\"");
  target.config_values().rustflags().push_back("x86-someos");
  target.config_values().rustflags().push_back("--edition=2018");

  std::optional<std::string> result = ExtractCompilerTargetTriple(&target);
  auto expected = std::nullopt;
  EXPECT_EQ(expected, result);
}

TEST_F(RustProjectWriterHelper, ExtractCompilerTargetTupleDontFallOffEnd) {
  TestWithScope setup;

  Target target(setup.settings(), Label(SourceDir("//foo/"), "bar"));
  target.set_output_type(Target::RUST_LIBRARY);
  target.visibility().SetPublic();
  SourceFile lib("//foo/lib.rs");
  target.sources().push_back(lib);
  target.source_types_used().Set(SourceFile::SOURCE_RS);
  target.rust_values().set_crate_root(lib);
  target.rust_values().crate_name() = "foo";
  target.config_values().rustflags().push_back("--cfg=feature=\"foo_enabled\"");
  target.config_values().rustflags().push_back("--edition=2018");
  target.config_values().rustflags().push_back("--target");

  std::optional<std::string> result = ExtractCompilerTargetTriple(&target);
  auto expected = std::nullopt;
  EXPECT_EQ(expected, result);
}