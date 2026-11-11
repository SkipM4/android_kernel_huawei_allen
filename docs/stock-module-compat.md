# Stock Module Compatibility Iteration

## Starting Point

The released configuration builds successfully, but it cannot directly replace
the installed kernel while retaining the installed modem module.

- Device: verified ALN-AL00 / HWALN / Kirin 9000S, serial
  `FMR0223C20002621`, normal Android boot with root and SELinux Enforcing.
- Installed module: `/vendor/modem/modem_driver/modem_driver.ko`.
- A copy is retained at `images/backup/allen-stock-5.10.43-modem_driver.ko`.
- Both the stock and built modules report
  `5.10.43 SMP preempt mod_unload modversions aarch64`.
- The stock module has 390 versioned imports. Against the initial released-
  configuration build, two exports are missing and 151 CRCs differ, including
  `module_layout` and APIs whose types depend on task/network structures.
- Missing CFI imports are `__cfi_slowpath` and
  `__ubsan_handle_cfi_check_fail_abort`.

Do not force module loading, disable MODVERSIONS, or flash that initial build
as if matching vermagic proved compatibility.

## Recoverable Image Baseline

Read-only backups of the actual current partitions are retained:

- `images/backup/allen-stock-5.10.43-20250805-boot.img`.
- `images/backup/allen-stock-5.10.43-20250805-ramdisk-magisk.img`.
- `images/backup/allen-stock-5.10.43-20250805-modem_driver.img`.

The boot/ramdisk images were transferred using base64 transport, checked against the partition
lengths and Android image magic, and match the supplied original images
byte-for-byte. The originals remain untouched.

The stock boot is Android header v2 with legacy LZ4 kernel compression, a gzip
ramdisk stub, and an AVB footer. Its v2 DTB payload begins with an OEM certificate
container, not a bare FDT. Preserve this entire payload unchanged rather than
replacing it with the live FDT or assuming a standard concatenated-DTB layout.
The separate Magisk ramdisk is not part of the kernel replacement.

The installed modem module is on the independent `modem_driver` partition,
`/dev/block/sdd68`, mounted read-only as ext4 at `/vendor/modem/modem_driver`.
The filesystem contains the module and `lost+found`; it is not the baseband
firmware or a user-data partition. Its image is also backed up using text-safe
binary transport. The module has UID/GID 0, mode 0644 and SELinux label
`u:object_r:vendor_file:s0`; any candidate replacement must preserve these.
No replacement or flash of this additional partition is authorized by the
initial boot/ramdisk-only flash workflow; obtain confirmation before that step.

Image tool: x86_64 `magiskboot` extracted from the official Magisk v28.1 APK:
`https://github.com/topjohnwu/Magisk/releases/download/v28.1/Magisk-v28.1.apk`.
It is stored at `tools/host/bin/magiskboot`, outside Git. Its unpack/repack
arguments were checked before use. Stock components are unpacked under
`images/extracted/allen-stock-5.10.43/`.

## Module Signature Trust

The firmware enforces signed modules. Its private signing key and OEM build
tools are not in the source release.

The OEM X.509 public certificate was extracted from the verified stock kernel
and is tracked at `configs/allen-stock-module-signing.pem`. Its serial is
`90A33915BD1B8C4B`, with subject O=Huawei, CN=Kernel Signing Key. OpenSSL CMS
verification of the unchanged stock module's detached signature succeeds using
this certificate; it is not merely selected by a matching name.

Use standard Kbuild signing with a local generated key and include the verified
OEM public certificate in `SYSTEM_TRUSTED_KEYS`. This preserves forced module
signature checking while permitting both existing OEM modules and locally
signed candidate modules. Private generated keys remain in the ignored output
directory; no private key is tracked or displayed.

## Configuration Approach

Use the saved live configuration as the base and apply
`configs/allen-stock-compat.fragment` in a separate output directory. Retain
Clang LTO, enforcing CFI, PRMEM and forced module signing.

Necessary overrides:

- Disable `KERNEL_CODEMERGE`: OEM PGO profiles and compiler extensions are not
  provided; it is an optimization mode, not a module ABI workaround.
- Select standard `certs/signing_key.pem` rather than the unavailable OEM
  private-key build path; include the OEM public certificate as a trusted key.
- Keep COMPAT_VDSO disabled, matching the stock configuration instead of newly
  enabling it through the nonempty 32-bit compiler prefix.
- Select ThinLTO instead of the stock Full LTO mode. After the missing hardening
  override, the Full LTO compiler phase completes but its monolithic linker is
  killed with status 137. The host kernel journal confirms global OOM killed
  `ld.lld`, not a compiler diagnostic or a cgroup memory cap. The host has
  approximately 31 GiB RAM and no swap. Standard ThinLTO supports the same
  cross-module CFI mechanism; limit its backend to two jobs and its cache to
  1 GiB without changing host swap settings.
- Disable `HKIP_CFI_HARDEN`: the first compatibility build fails because the
  entire `platform_source/see/drivers/prmem/cfi_harden/` implementation is
  absent. Its Kconfig describes extra protection against malicious CFI-table
  writes. The existing standard Android enforcing-CFI path remains available;
  CFI permissive mode, SELinux permissive mode and unsigned-module loading are
  not enabled. Loss of this OEM-specific table protection is a security
  difference, not claimed as equivalent to the stock hardening implementation.

Configuration generation also drops settings whose definitions or dependency
paths are absent from the published configuration graph, including PERF_CTRL,
FRAME_RTG/HW_RTG_SCHED and SCHED_STAT_YIELD. The latter two affect task-related
types. HKIP hardening also changes the CFI field type inside `struct module`.
Therefore merely enabling standard CFI is not expected to prove full stock
module compatibility; inspect actual generated CRCs before any flash.

No speculative bulk Kconfig additions, CRC bypasses, or replacement security
implementations are introduced.

The later r32 read-only RTG audit identifies the specific dependency break:
PERF_CTRL has an unsourced Kconfig definition and no parent driver traversal,
and the published perf_ctrl directory lacks perf_ctrl.c. HW_RTG_SCHED's
definition is sourced but depends on FRAME_RTG, which depends on PERF_CTRL.
It is the actual /proc/<pid>/rtg gate; SCHED_CGROUP_RTG is unrelated to that
entry. See `docs/clang12-rtg-source-audit.md` before changing these options;
restoration affects task layout, Binder propagation and scheduler behavior,
not only ioctl file registration.

The later dispatcher restoration connects that definition and driver graph
and adds a pinned 5.10.43 reference implementation adapted to Allen's command
IDs. An enabled build still fails at the required missing thermal query
provider. The compatibility fragment explicitly disables PERF_CTRL,
FRAME_RTG, SCHED_STAT_YIELD and DDR_PERF_CTRL until that provider is audited;
this preserves the previous effective state during stock-config regeneration.
Do not infer enabled-driver, module-ABI or runtime success from the individually
compiled dispatcher/scheduler-query objects.

The next source step completes the thermal query provider and permits a full
PERF_CTRL-enabled link. The fragment now enables that built-in provider while
keeping FRAME_RTG, SCHED_STAT_YIELD and DDR_PERF_CTRL disabled, so the separate
RTG/task-layout paths are not restored automatically. Native/compat query ABI
boundaries and the inactive HWP slot are also handled explicitly. Runtime
and paired-image checks are still separate from enabled-build success; see
`docs/clang12-rtg-source-audit.md` for the current gate.

## Reproduction

From `/home/data/Allen`:

```sh
mkdir -p out/allen-clang12-stock-compat
Code_Opensource/kernel/scripts/kconfig/merge_config.sh -m \
  -O "$PWD/out/allen-clang12-stock-compat" \
  configs/allen-stock-5.10.43.config configs/allen-stock-compat.fragment
OUTPUT_DIR=out/allen-clang12-stock-compat scripts/build-allen.sh olddefconfig
OUTPUT_DIR=out/allen-clang12-stock-compat scripts/build-allen.sh
scripts/check-module-abi.py \
  images/backup/allen-stock-5.10.43-modem_driver.ko \
  out/allen-clang12-stock-compat/Module.symvers
```

The ABI checker reads the real installed module's version table and the actual
kernel exports. A nonzero result blocks treating the stock module as compatible.
It does not validate runtime CFI type IDs, external firmware protocols, or boot.

## Local Build Validation (Before The r2 Flash)

- Current boot/ramdisk backups and stock signature verification pass.
- The modem partition backup passes read-only ext4 checking and contains the
  exact installed module byte-for-byte. Inspected vendor/system/ODM/chip-product
  paths contain only this external kernel module.
- Stock-derived configuration generation passes with LTO/CFI and module-signing
  enforcement retained.
- The first compile failure is classified as an unpublished OEM hardening
  implementation, not a Clang compilation failure. That build was stopped.
- A subsequent Full LTO link fails with confirmed host OOM. Bounded ThinLTO
  completes the full kernel and module build successfully.
- Against this kernel, the stock module now has no missing exports but still
  has 151 CRC mismatches. The matching self-built module has 390 versioned
  imports, no missing exports and no CRC mismatches.
- Both the verified OEM certificate and local signing certificate are embedded
  in the built kernel. The candidate module is debug-stripped, SHA-512 signed,
  rechecked for ABI compatibility, and cryptographically verified against the
  same local certificate. `make` alone does not sign the output module; signing
  occurs explicitly during candidate staging.
- No device writes, reboots or flashes have been performed in this iteration.

## Candidate Pair

Both images are generated and validated locally. The 2026-11-10 paired flash
test fails with a MAS block-completion CFI panic; both original partitions are
restored and normal Android boot is verified. See
`docs/clang12-r2-flash-test.md` for attribution, rollback and the next narrow fix:

- `images/test/allen-boot-test-5.10.43-clang12-r2.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r2.img`.
- Signed module copy:
  `images/test/allen-modem_driver-test-5.10.43-clang12-r2.ko`.

The boot candidate is made from the verified original `boot.img` using
`scripts/repack-allen-boot.sh` and the uncompressed built `Image`. It preserves
the original compressed firmware ramdisk stub and complete OEM DTB container.
Validation checks the kernel decompression round-trip, identical stub and DTB,
protected header fields and cmdline, header v2, legacy LZ4, AVB footer presence
and the boot partition bound. It is an unlocked-device test image, not an
OEM-signed AVB authentication claim. No vbmeta-disable flags are requested.

The modem candidate clones the verified ext4 backup and replaces only
`/modem_driver.ko` with the matching signed module, restoring UID/GID 0, mode
0644 and the exact original SELinux xattr. The filesystem UUID/layout remain
from the original image. `debugfs` mutation left one unreferenced allocated
bitmap entry; `e2fsck` corrects this only in the candidate copy. Subsequent
read-only ext4 checking passes and module/xattr readback still matches.

The initial released-config kernel and its module are not part of this pair.
Do not mix versions or flash only the new boot: the old module fails CRC checks.
Conversely, reverting only boot would leave a module signed by a key the stock
kernel does not trust. Treat boot and modem_driver as one test/rollback pair.

The user approves the additional modem_driver partition and the r2 paired test
is complete. The existing Magisk ramdisk stays unchanged. Both backed-up boot
and modem_driver images are restored after collecting the failure evidence;
no partition is erased or formatted. r2 reaches kernel initialization but not
Android, so it is not a bootable-system candidate. Runtime modem/baseband
compatibility remains untested. Subsequent targeted CFI fixes and paired tests
are recorded in `docs/clang12-cfi-iterations.md`. Use its latest entry for the
current concrete failure and verified rollback, not this initial build result.
