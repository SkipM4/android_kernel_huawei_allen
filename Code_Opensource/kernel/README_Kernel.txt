Allen Linux 5.10.274
===================

Read README.allen for the maintained compiler, build commands, checkpoint,
module-signing details, archive contents, and device-validation limits.

The supported ARM64 profiles share this one source tree:

  allen_secure_defconfig   - accepted #101 security choices
  allen_relaxed_defconfig  - accepted #103 local debugging choices

The original 5.10.43 merge_charlotte_defconfig and its old compiler directions
have been superseded. Use Android Clang/LLD 12.0.7 (clang-r416183b1) and a
separate output directory as documented in README.allen.
