load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
         ":Analysis",
         ":BinaryBlob",
         ":freq",
         ":matasano_set01",
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
    name = "freq",
    srcs = ["freq.cc"],
)

cc_library(
    name = "matasano_set01",
    srcs = ["matasano_set01.cc"],
    hdrs = ["matasano_set01.h"],
    deps = [
         ":Analysis",
         "@boringssl//:crypto",
    ],
)

cc_library(
    name = "utils",
    srcs = ["utils.c"],
    hdrs = ["utils.h"],
)