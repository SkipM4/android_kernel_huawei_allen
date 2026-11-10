# Allen Kernel Bring-Up

Build the official ALN HarmonyOS 4.2 Linux 5.10.43 sources on x86_64 Linux
before attempting newer kernels. Project safety rules are in `AGENTS.md`.

## Current Status

Clang 12.0.7 builds a signed, ABI-matched boot/modem_driver pair with enforcing
CFI. r16 reaches the Huawei home screen on two boots, but fails after about an
hour when storaged exercises a hyperframe direct-I/O hook. r17 supplies typed
CFI entries for the audited hyperframe callbacks, reaches the home screen on
two boots and passes actual direct-I/O read/write checks. Its second boot runs
continuously for over 65 minutes without panic or restart, beyond r16's known
failure window. r17 is the first accepted self-built normal-boot baseline and
is retained as the tested rollback pair. This establishes the boot goal, not
complete OEM feature parity or production stability. See `docs/clang12-working-baseline.md` for
reproduction, the accepted image pair, rollback and remaining limitations.

Use the stock-derived compatibility build in `docs/stock-module-compat.md`
for paired device tests, not the initial released-configuration build below.
See `docs/clang12-cfi-iterations.md` for each attributed failure and recovery.

Functional follow-up reaches r19: static sensorhub route-lock initialization
and ION reuse eligibility repairs pass paired flash and targeted sensor/ION,
storage, USB and display sleep/wake checks. r19 is the current test candidate,
not a full hardware certification. Camera consent, Wi-Fi association, the
video IRQ-flags bug and multi-level L3 voting remain unresolved or unverified.
See `docs/clang12-functional-tests.md` for results and the next isolated fix.

## Layout

- `Code_Opensource/kernel/`: official sources with targeted bring-up fixes.
- `configs/allen-stock-5.10.43.config`: configuration read from the running phone.
- `tools/toolchains/clang-r416183b1/`: local Android Clang 12.0.7 toolchain.
- `tools/host/bin/bc`: local build dependency, extracted from the Arch package.
- `out/allen-clang12/`: independent kernel output directory; not tracked.
- `out/allen-clang12-stock-compat/`: stock-derived paired-test output; not tracked.
- `images/original/`: firmware device-tree backup; not tracked.
- `docs/bringup-baseline.md`: device baseline, toolchain provenance and risks.
- `docs/stock-module-compat.md`: installed modem ABI and signing compatibility.
- `docs/clang12-r2-flash-test.md`: r2 CFI boot failure, verified rollback and next fix.
- `docs/clang12-cfi-iterations.md`: targeted CFI fixes and subsequent paired tests.
- `docs/clang12-working-baseline.md`: accepted r17 boot baseline and test scope.
- `docs/clang12-functional-tests.md`: r18/r19 repairs and hardware test limits.

The original source archive, `boot.img` and `ramdisk-magisk.img` remain at the
project root and are excluded from Git. Never overwrite them.

## Build

From `/home/data/Allen`:

```sh
scripts/build-allen.sh merge_charlotte_defconfig
scripts/build-allen.sh
```

The target and compiler revision come from the official README and Makefile.
The wrapper uses LLVM tools and the integrated assembler, not the host GCC for
ARM64. `JOBS` overrides the default eight build jobs; `OUTPUT_DIR` selects an
independent output directory. `LOCALVERSION=` keeps the
stock `5.10.43` release string instead of adding a Git suffix; this alone does
not establish module ABI compatibility.
ThinLTO links use two backend jobs and a bounded cache to avoid the observed
Full LTO memory failure and excessive cache growth.

Additional make targets can be passed directly, for example:

```sh
scripts/build-allen.sh Image.lz4
```

Full output goes to `.tmp/build-clang12.log`. Inspect only the relevant error
context, record durable conclusions, then delete the log when no longer needed.
The release configuration selects LZ4, so the default kernel output is
`out/allen-clang12/arch/arm64/boot/Image.lz4`. The README's generic `Image.gz`
example does not describe the actual default selected by this configuration.

## Device And Images

The verified device serial is `FMR0223C20002621`. Always pass `-s` to ADB and
fastboot. The user confirms `HWALN` is the device's abbreviation for HW Allen.

The device has separate `boot` and `ramdisk` partitions. The original boot image
uses Android header version 2 and includes a small firmware ramdisk stub; the
Magisk image uses header version 0 and belongs to the separate ramdisk partition.
Preserve the stock boot structure, stub and device tree when replacing its
kernel. Do not merge the Magisk ramdisk into boot.

Configuration success or compilation success is not a boot or module-ABI test.
Do not flash until the candidate image, target partition, recovery path and
fastboot identity have been verified according to `AGENTS.md`.

The initial released-config kernel does not satisfy the installed modem module's
symbol CRCs or CFI imports. See `docs/stock-module-compat.md` for the separate
stock-derived configuration, verified OEM public certificate and ABI checker.

The ThinLTO/CFI `r2` candidate is a matching boot/modem_driver pair, not a
boot-only replacement. Both test images and their rollback requirements are
recorded in that document. The paired flash test reaches a MAS block-completion
CFI panic before Android starts; both originals are restored and normal boot is
verified. See `docs/clang12-r2-flash-test.md`. Do not reuse r2 as a working kernel.
Subsequent fixes reach normal Android; r16's later direct-I/O CFI failure is
repaired in the accepted r17 baseline. Originals are restored after failed
tests; functional candidates later replace the installed r17 while its pair
is retained as a rollback baseline. See
`docs/clang12-cfi-iterations.md` for the complete failure and recovery history.

For future versions, create a new boot candidate from the uncompressed Image:

```sh
scripts/repack-allen-boot.sh out/allen-clang12-stock-compat/arch/arm64/boot/Image \
  images/test/allen-boot-test-5.10.43-clang12-r20.img
```

The repacker refuses to overwrite an existing image and validates the actual
packed components. A corresponding ABI-matched, signed modem-driver filesystem
image is still required; a successful repack does not establish successful boot.

Stage that matching module partition from the same kernel output directory:

```sh
scripts/stage-allen-modem.sh out/allen-clang12-stock-compat \
  images/test/allen-modem_driver-test-5.10.43-clang12-r20.img
```

Use a new versioned path for each iteration. This produces the companion signed
`.ko` and an ext4 image cloned from the verified original partition, checking
ABI imports, the embedded signing certificate, CMS signature, filesystem
integrity, ownership/mode and SELinux-label readback. It refuses existing output
paths and cleans its owned temporary files. These are staging checks, not a
runtime-CFI or boot guarantee; never flash this partition without its paired boot.
