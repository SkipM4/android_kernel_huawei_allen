# Allen Kernel Bring-Up

Build the official ALN HarmonyOS 4.2 Linux 5.10.43 sources on x86_64 Linux
before attempting newer kernels. Project safety rules are in `AGENTS.md`.

## Layout

- `Code_Opensource/kernel/`: official sources with a link-script portability fix.
- `configs/allen-stock-5.10.43.config`: configuration read from the running phone.
- `tools/toolchains/clang-r416183b1/`: local Android Clang 12.0.7 toolchain.
- `tools/host/bin/bc`: local build dependency, extracted from the Arch package.
- `out/allen-clang12/`: independent kernel output directory; not tracked.
- `images/original/`: firmware device-tree backup; not tracked.
- `docs/bringup-baseline.md`: device baseline, toolchain provenance and risks.

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
ARM64. `JOBS` overrides the default eight build jobs. `LOCALVERSION=` keeps the
stock `5.10.43` release string instead of adding a Git suffix; this alone does
not establish module ABI compatibility.

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
