# Initial Bring-Up Baseline

## Source And Host

- Source: supplied `ALN_HarmonyOS4.2.0_Code_Opensource.tar.gz`, released
  2024-10-09; Linux 5.10.43 for ALN / Kirin 9000S (Charlotte).
- Original import: commit `881d03c79`, branch `stock-5.10.43`. All supplied source
  files are preserved in the baseline before source changes.
- Active bring-up branch: `bringup/clang12`.
- Host: x86_64 Arch Linux; Python 3.14.9, GCC 16.2.1, system Clang 23.1.4,
  GNU Make 4.4.1. System Clang is not used for the kernel build.
- Bison, Flex, Perl, OpenSSL and LLVM tools are available. `bc` was absent and
  is provided locally from Arch's `bc-1.08.2-1-x86_64.pkg.tar.zst`; no system
  package installation was performed.
- No standalone GNU ARM cross compiler is installed. The build uses LLVM=1,
  LLVM_IAS=1 and ARM target prefixes. The OEM Makefile probes GNU `elfedit` even
  in this mode; the missing-tool message did not prevent configuration or the
  full compile. No GNU cross compiler was needed for this configuration.
- The `paseo-progress` command is unavailable, so progress is reported in chat.

## Compiler Provenance

The official source Makefile selects `clang-r416183b1` unless BiSheng or another
explicit compiler mode is selected. The older README's `clang-r353983c` example
is not the actual default in this release.

- Source repository:
  `https://android.googlesource.com/platform/prebuilts/clang/host/linux-x86`
- Android branch: `android12-release`.
- Pinned repository commit: `3f17560d9bd078537f29294c2bcf80b591821a09`.
- Download:
  `https://android.googlesource.com/platform/prebuilts/clang/host/linux-x86/+archive/3f17560d9bd078537f29294c2bcf80b591821a09/clang-r416183b1.tar.gz`
- Compiler: Android build 7485623, Clang 12.0.7, LLVM revision
  `c935d99d7cf2016289302412d708641d52d2f7ee`; LLD 12.0.7.
- Applicability: x86_64-hosted Android ARM64 builds, matching the release's
  recorded Clang version and compiler path. This is Clang 12.0.7, not 12.0.1.

## Verified Device

Read-only ADB inspection on 2026-11-10:

- Serial: `FMR0223C20002621`; model: `ALN-AL00`.
- Product device: `HWALN`; user confirms this is HW Allen and matches the target.
- Board: `ALN`; hardware and board platform: `kirin9000s`; ODM device: `8425L`.
- Device-tree model: `kirin9000s`; compatible: `hisilicon,charlotte`.
- Build: `ALN-AL80 4.2.0.186(C00E182)`.
- Running kernel: 5.10.43, built 2025-08-05 11:12:57 CST, using BiSheng Clang
  15.0.4 / LLD 15.0.4.
- Mode: normal Android boot, `sys.boot_completed=1`.
- Magisk `su` provides UID 0 in `u:r:magisk:s0`; SELinux remains Enforcing.
- Another ADB device is connected. No commands target that device.

Read-only partition links:

| Partition | Block device |
| --- | --- |
| boot | /dev/block/sdd52 |
| ramdisk | /dev/block/sdd71 |
| recovery | /dev/block/sdd53 |
| recovery_ramdisk | /dev/block/sdd58 |
| fw_dtb | /dev/block/sdd62 |
| dtbo | /dev/block/sdd63 |
| rrecord | /dev/block/sdd22 |
| bootfail_info | /dev/block/sdd19 |

Original firmware images remain `boot.img` and `ramdisk-magisk.img`. The running
FDT is preserved at `images/original/allen-running-5.10.43-20250805.dtb`.
The live configuration is tracked at `configs/allen-stock-5.10.43.config`.
Root command output exhibited CRLF conversion, corrupting a direct binary
transfer. These backups use base64 transport with carriage returns stripped
before decoding; the configuration decompresses and the FDT format is valid.

## Image Format

- `boot.img`: Android boot image header v2; page size 2048; nonempty kernel;
  small nonempty ramdisk stub; no second-stage payload; nonempty v2 DTB payload.
  Kernel compression is legacy LZ4 (magic `02 21 4c 18`), matching
  `CONFIG_ARM64_LZ4=y` and the default `Image.lz4` build target.
- `ramdisk-magisk.img`: Android boot image header v0; page size 2048; no kernel;
  Magisk-patched standalone ramdisk.
- Preserve image headers, boot cmdline, ramdisk stub and firmware device tree.
  The stock boot cmdline includes `arm64.nobf16` and SELinux Enforcing.

## Configuration And Compatibility

`merge_charlotte_defconfig` is the released ARM64 configuration, not a custom
merge script. It already records Android Clang 12.0.7. Configuration generation
with the downloaded matching compiler passes without source edits.

The running firmware differs from the release configuration in several areas:

- Full Clang LTO, CFI and HKIP CFI hardening are enabled on the phone but disabled
  in the release configuration.
- `KERNEL_CODEMERGE` is enabled on the phone. Enabling it in this checkout would
  introduce OEM PGO file and toolchain dependencies; do not enable it blindly.
- The phone enforces signed modules using an OEM signing key unavailable here;
  the release configuration does not enable module signing.
- The phone additionally enables strong stack protection, POSIX queues/IPC
  namespaces, RPMB verification, page-cache and scheduler features, and changes
  PRMEM/HKIP capacities.

First compile the released configuration unchanged. Before flashing, assess
module CRCs, CFI expectations and the effect of the newer installed firmware.
Matching the kernel release number does not prove ABI compatibility.
The running phone currently loads `modem_driver`; compatibility with that
module must be assessed before treating a candidate as usable.

## Current Verification And Next Step

- Official source unpacked and imported into Git without modifying the archive
  or original images.
- Downloaded Clang and LLD execute successfully on this host.
- Official configuration generation passes; existing OEM Kconfig formatting
  warnings are not modified.
- Initial full compilation passes with Clang 12.0.7: ARM64 `vmlinux`, `Image`,
  legacy-LZ4 `Image.lz4`, `modem_driver.ko` and `hisi_dummy_ko.ko` are generated.
- Kernel release is `5.10.43`. Built module vermagic is
  `5.10.43 SMP preempt mod_unload modversions aarch64`; this is not a CRC/CFI
  compatibility check against the installed firmware module.
- No flashing, reboot, partition write or SELinux policy/mode change performed.

The successful build nevertheless exposed a malformed test in
`Code_Opensource/kernel/scripts/link-vmlinux.sh`: missing spaces around a `[[`
expression made the shell try to execute a command instead of testing
`verify_plat`, silently skipping the platform interface checker. The fix uses
POSIX `[ ... ]` syntax and `=` in the checker's related product comparisons.
This follows standard shell test syntax; no external driver code is adopted and
no runtime subsystem behavior is intentionally changed.

The release does not contain Charlotte interface baseline files. Restoring the
checker call therefore does not establish an OEM platform-interface audit; its
existing behavior skips platforms without a supplied baseline.

Source correction commit: `3884cafc1`. Verification passes:

- Shell syntax checks and `git diff --check`.
- Incremental full build through `scripts/build-allen.sh`, including the
  restored Charlotte checker call. No hard build or shell errors remain.
- Legacy-LZ4 decoding matches the uncompressed `Image` byte-for-byte, and the
  Kbuild uncompressed-size trailer is correct. A plain `lz4 -t` on the complete
  kernel reports trailing data because this four-byte trailer is outside the
  LZ4 stream; it is not image corruption.
- ARM64 Image magic, little-endian mode, 4 KB page flags and stock release
  string are validated.

Compilation and image integrity are verified, but boot and existing-module
compatibility are not. OEM compiler warnings and linker section-mismatch
warnings remain; no unrelated warning cleanup is performed.

Next: locate the installed `modem_driver` and compare its symbol CRCs and CFI
requirements with this build, then package a candidate preserving the original
boot header, LZ4 format, stub ramdisk and DTB. Retain a recoverable known-good
image before any flash test.

Follow-up results and the unflashed paired candidate are recorded in
`docs/stock-module-compat.md`.
