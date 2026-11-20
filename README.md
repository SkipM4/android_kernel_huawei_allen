# Huawei Allen Linux Kernel

Linux **5.10.274** for Huawei Mate 60 Pro **ALN-AL00 / HWALN**, Kirin 9000S.
This repository starts from the published 5.10.43 device source and includes
ordered Linux 5.10 stable updates and the device adaptations needed to boot it.

The repository root is the kernel source root: `Makefile`, `Kconfig`, `arch/`,
`drivers/`, `include/` and `scripts/` are used directly.

## Configurations

One source tree provides two standard ARM64 defconfigs:

| Configuration | Behavior |
| --- | --- |
| `allen_secure_defconfig` | Accepted #101 security settings: mandatory module signatures, Huawei root checks/scanning, and SELinux Enforcing. |
| `allen_relaxed_defconfig` | Accepted #103 debug settings: module signatures generated but not mandatory; Huawei root checks/scanning disabled; SELinux starts Enforcing and permits an explicit Magisk `setenforce` change after boot. |

Both configurations are in `arch/arm64/configs/` and retain ThinLTO, enforcing
CFI, BTI/BHB and PRMEM/HKIP. The relaxed profile does not disable every kernel
hardening feature. There is no automatic permissive-startup service.

The public trust certificate is included as
`certs/allen-stock-module-signing.pem`. Private signing keys are not included.

## Build

Use x86_64 Linux and **Android Clang/LLD 12.0.7, clang-r416183b1** (Android
build 7485623). Install GNU make, bash, Python 3, host GCC/G++, bc, flex, bison,
pkg-config, OpenSSL and libelf development packages, and standard Unix tools.

From this repository root:

```sh
export ALLEN_CLANG=/absolute/path/to/clang-r416183b1
export ALLEN_OUT="$PWD/out/allen"
export PATH="$ALLEN_CLANG/bin:$PATH"
mkdir -p "$ALLEN_OUT"

make_allen() {
    make ARCH=arm64 O="$ALLEN_OUT" LLVM=1 LLVM_IAS=1 \
        CROSS_COMPILE=aarch64-linux-gnu- \
        CROSS_COMPILE_COMPAT=arm-linux-gnueabi- \
        CLANG_PREBUILTS_PATH="$ALLEN_CLANG/" LOCALVERSION= \
        LD="$ALLEN_CLANG/bin/ld.lld --thinlto-jobs=2 --thinlto-cache-policy=cache_size_bytes=1g" \
        "$@"
}

make_allen allen_secure_defconfig
make_allen olddefconfig
make_allen -j8 Image modules
```

For the debug profile, select `allen_relaxed_defconfig` instead. Use separate
output directories when keeping both builds.

The outputs are relative to `ALLEN_OUT`:

- `arch/arm64/boot/Image`
- `drivers/platform_drivers/modem/modem_driver.ko`

Kbuild creates a fresh local signing key and certificate in the output's
`certs/` directory when no key is supplied. Keep its private key local. Strip
and sign a deployable module using that same build's `scripts/sign-file` and
key. Match the kernel and module versions, symbol CRCs and signing identity;
do not combine a new kernel with an incompatible stock module. New keys and
build timestamps mean a fresh build is not byte-identical to the test images.

The device uses a boot partition and an independent Magisk ramdisk partition.
Creating a bootable device image additionally needs the original firmware
DTB, boot header/stub, runtime firmware and matching module filesystem. Those
local device artifacts and the external compiler are not distributed here.

## Validated Checkpoint

Secure #101 and relaxed #103 both reached normal Android and automatically
loaded their matching signed modem module. #103 booted Enforcing with module
`sig_enforce=N`, passed a Magisk `setenforce 0` / `setenforce 1` round trip, and
was left Enforcing. CE storage, essential services, partition/module readbacks,
PRMEM and zero WARN/Oops counters were checked. The independent ramdisk was
unchanged.

This is still a device-specific OEM Android kernel. Android common parity,
OEM/core separation, stable GKI/KMI, and broad hardware, networking, performance
and power acceptance remain unfinished.

## Source History And Documentation

The root-layout migration preserves the previously published Git history and
commit identities. The original wrapper paths remain visible in old commits.
See [repository layout](Documentation/allen/repository-layout.md) for the path
mapping and current dependency layout.

- [Stage closeout](Documentation/allen/allen-5.10.274-stage-closeout.md)
- [Security profile validation](Documentation/allen/clang12-security-relaxed-test.md)
- [Stable synchronization records](Documentation/allen/linux-5.10-upstream-sync.md)
- [Historical project README](Documentation/allen/history.md)
- [Android common contribution guide](Documentation/process/android-common-patches.md)

COPYING, LICENSES/ and the original per-file copyright and license notices
remain in force. No blanket replacement license is applied.
