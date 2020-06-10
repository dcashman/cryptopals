load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//test:__pkg__"])


cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
         ":Analysis",
         ":BinaryBlob",
         ":freq",
         ":matasano_problems",
         ":utils",
    ],
)

cc_library(
    name = "Analysis",
    srcs = ["Analysis.cc"],
    hdrs = ["Analysis.h"],
    deps = [
         ":BinaryBlob",
    ],
)

cc_library(
    name = "BinaryBlob",
    srcs = ["BinaryBlob.cc"],
    hdrs = ["BinaryBlob.h"],
    deps = [
         ":utils",
    ]
)

cc_library(
    name = "Block",
    srcs = ["Block.cc"],
    hdrs = ["Block.h"],
    deps = [
         ":BinaryBlob",
         "@boringssl//:crypto",
    ],
)

cc_library(
    name = "freq",
    srcs = ["freq.cc"],
)

cc_library(
    name = "matasano_problems",
    srcs = ["matasano_problems.cc"],
    hdrs = ["matasano_problems.h"],
    deps = [
         ":Analysis",
         ":Block",
    ],
)

cc_library(
    name = "utils",
    srcs = ["utils.c"],
    hdrs = ["utils.h"],
)