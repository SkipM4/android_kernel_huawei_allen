# Local Security-Relaxed Test

This is a local test-only configuration based on the accepted Linux 5.10.274
source (`f56d07c315104360d12cbfc2009d039840d2513d`). It does not replace the
accepted #101 security baseline, does not certify GKI/KMI compatibility, and is
not a release artifact.

## Configuration

The reproducible fragment is `configs/allen-test-security-relaxed.fragment`.
It is merged into the normal output configuration before building. The
security-related changes are:

- `CONFIG_MODULE_SIG_FORCE=n`: modules are still signed with the existing
  SHA-512 key and `CONFIG_MODULE_SIG_ALL=y`, but unsigned or unknown-key modules
  are no longer rejected solely by the force gate.
- `CONFIG_HUAWEI_PROC_CHECK_ROOT=n` and `CONFIG_HW_ROOT_SCAN=n`: the Huawei
  `/proc` root check and kernel root scanner are not built.
- `CONFIG_SECURITY_SELINUX_DEVELOP=y`, while SELinux remains enabled and
  `CONFIG_SECURITY_SELINUX_BOOTPARAM`/`DISABLE` remain off. The built-in
  `enforcing=1` argument is appended with `CONFIG_CMDLINE_EXTEND=y`, so boot
  still starts Enforcing. This exposes the development-mode
  `/sys/fs/selinux/enforce` write path for an explicit post-boot Magisk test.

ThinLTO, CFI, module signing, the signing certificate, PRMEM/HKIP and the
device-tree/ramdisk handling remain unchanged. The exact configuration is in
`.tmp/security-test/build103.json`.

## Build And Stage

Build #103 (`5.10.274`) was produced with the existing Android Clang/LLD 12
wrapper and exited successfully. The modem module has 390 versioned imports,
no missing exports or CRC mismatches, and `5.10.274` vermagic. The staged
artifacts passed boot round-trip/DTB/header checks, CMS module signature
verification, ext4 metadata readback and partition-size checks:

- `images/test/allen-boot-test-5.10.274-clang12-r103-relaxed-security.img`
- `images/test/allen-modem_driver-test-5.10.274-clang12-r103-relaxed-security.img`
- `images/test/allen-modem_driver-test-5.10.274-clang12-r103-relaxed-security.ko`

The independent Magisk ramdisk was not modified or flashed.

## Device Result

On the confirmed ALN-AL00/HWALN target, only `boot` and `modem_driver` were
flashed. The device reached normal Android on `5.10.274 #103`; the matching
modem loaded automatically as `Live`, all candidate partition/module readbacks
matched, CE storage and the three essential services were available, and the
screen was returned to `Asleep` with tracing left at `0` and the `wifi`
instance preserved.

The runtime security checks were scoped to this test image:

- At boot: `getenforce=Enforcing`, the SELinux enforce file reads `1`, taint is
  `4`, and module `sig_enforce=N`.
- Through Magisk root: `setenforce 0` returned `Permissive`/`0`, then
  `setenforce 1` restored `Enforcing`/`1` before acceptance was recorded.
- No kernel WARN, BUG, Oops, panic, CFI failure, module-format error, unknown
  symbol, or module-verification failure was found. No module-load AVC denial
  was found; ordinary OEM AVCs remain outside this test's scope.
- The final security archive/vmlinux contain no Huawei root-scanner objects or
  scanner symbols, and `/proc/check_root` and `/proc/rootscan` are absent.

The exact fallback pair is the accepted 5.10.274/#101 boot and modem pair. The
full command log and runtime record remain under `.tmp/security-test/` for
local audit; they are not release material.

This configuration deliberately weakens module-loading and root-detection
controls. Keep it limited to local debugging and restore the #101 secure pair
before any production-like or security-sensitive use.
