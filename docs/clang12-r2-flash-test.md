# Clang 12 r2 Paired Flash Test

## Outcome (2026-11-10)

The ThinLTO/CFI r2 kernel boots into kernel initialization but does not reach
Android or ADB. At about 14.2 seconds, enforcing CFI detects an invalid MAS
request-completion target and panics. The device returns to fastboot.

Both original partitions are restored and normal Android boot is confirmed.
The independent Magisk ramdisk remains unchanged. This iteration changes only
documentation; no kernel workaround is applied.

## Device And Deployment

- Source/build baseline: `bringup/clang12` at `32fdffda3`, official ALN
  HarmonyOS 4.2 Linux 5.10.43 sources and Android Clang 12.0.7 r416183b1.
- Target serial: `FMR0223C20002621`; Android identity `ALN-AL00`, `HWALN`,
  `kirin9000s`. HWALN is the user-confirmed abbreviation for HW Allen.
- Installed firmware: `ALN-AL80 4.2.0.186(C00E182)`; original kernel build
  `#1 SMP PREEMPT Tue Aug 5 11:12:57 CST 2025`, BiSheng Clang 15.0.4.
- Candidate kernel build: `#2 SMP PREEMPT Tue Nov 10 17:23:02 CST 2026`.
- User explicitly authorizes this boot/modem_driver paired test.

Before reboot, Android partition reads confirm both current partitions match
their original rollback backups. Image formats and partition bounds are checked
against the verified device layout. The working tree is clean before testing.

Standard `getvar product`, `getvar all`, unlock and partition queries return
`Command not supported` on this Huawei bootloader. Serial-pinned
`oem get-product-model` returns `ALN-AL00`; `oem get-bootinfo` returns `unlocked`.
Partition names and capacities come from the preceding Android read-only checks,
not guessed fastboot aliases.

After the identity/unlock checks, these operations succeed, without a reboot
between the two writes:

```sh
cd /home/data/Allen
fastboot -s FMR0223C20002621 flash boot images/test/allen-boot-test-5.10.43-clang12-r2.img
fastboot -s FMR0223C20002621 flash modem_driver images/test/allen-modem_driver-test-5.10.43-clang12-r2.img
fastboot -s FMR0223C20002621 reboot
```

Fastboot reports a footer-copy warning because its unsupported partition-size
queries produce zero. Both transfers/writes return OKAY. Subsequent recovery
readback verifies both complete partitions match the candidate files exactly;
there is no evidence of image truncation. This is not a claim of OEM AVB signing.

## Failure Evidence And Attribution

No target ADB appears during the bounded 180-second observation. Serial-pinned
fastboot then shows the target present. `fastboot reboot recovery` succeeds and
engineering recovery provides ADB root. Its generic model/device properties
are `generic_a15`; the serial, `kirin9000s` hardware and previously confirmed
fastboot identity maintain device continuity. Recovery runs the original
BiSheng kernel, not r2. Its ADB transport state is `recovery`, so diagnostics
must not wait exclusively for the `device` state.

Read-only diagnostics collect pstore and the rrecord/bootfail_info partitions.
The current console record identifies build `5.10.43 #2`, `kirin9000s`, and:

```text
[14.209780s] Kernel panic - not syncing: CFI failure (target: __cfi_mas_blk_busyidle_end_rq+0x0/0x1c)
__blk_mq_end_request+0x158/0x184
scsi_end_request -> scsi_io_completion -> scsi_finish_command
```

This is a block-I/O completion CFI failure, not a failed kernel-image load or
an established modem-module ABI/signature failure. Modem loading and normal
userspace operation remain untested for r2.

The separate dmesg pstore record has an earlier timestamp and a sysfs NULL
dereference. It also labels its kernel `5.10.43 #2`, so the build number alone
cannot establish provenance. Its `sysfs_kf_write` extent is `0x54`, unlike the
r2 implementation's `0xb8`; it is not the same built kernel. Decoding rrecord
with the official packed structures finds only older October 25 / November 2
records, including kernel-image verification and partition-mount failures.
Neither those records nor the old sysfs panic is attributed to r2. Full logs
and raw diagnostic partition copies are temporary and are removed after
extracting these facts.

## Narrowed Source Analysis

The relevant original-source chain is:

- `block/blk-mq.c`: `__blk_mq_end_request` invokes `rq->end_io(rq, error)`.
- `include/linux/blkdev.h`: `rq_end_io_fn` is
  `void (struct request *, blk_status_t)`.
- `platform_source/basicplatform/block/mas_blk_busy_idle_interface.c`:
  the `__cfi_mas_blk_busyidle_end_rq` wrapper has the matching prototype.
- `platform_source/basicplatform/block/mas_blk_busy_idle_notify.S`:
  the published precompiled assembly uses `adrp`/`add` on the wrapper's raw
  symbol and stores that address directly into `rq->end_io`.
- The official Makefile uses `-fno-sanitize-cfi-canonical-jump-tables`.

The built LLVM IR confirms the wrapper type is `void (request *, i8)` with
CFI type `_ZTSFvP7requesthE`. Final disassembly confirms the MAS assignment
stores the wrapper implementation address; the failed call's return offset
is the CFI slowpath for `rq->end_io`. Thus the immediate compatibility issue
is the callback address supplied by opaque assembly to a CFI-instrumented C
caller, rather than an obvious source-level parameter mismatch.

Next minimal change: expose this callback assignment through a small compiled
C entry point so Clang/LTO can produce and track a valid CFI function pointer,
and update only the corresponding assembly assignment. Verify the resulting
CFI target and retain enforcing CFI, module signatures and MAS hardware
behavior. Do not globally disable CFI, remove MAS or batch-rewrite other
assembly callbacks. A revised image pair and another device test are required
before claiming that this approach fixes boot.

## Rollback And Checks

After collecting diagnostics, recovery reboots to bootloader. Model and unlock
checks are repeated, then both originals are flashed before reboot:

```sh
cd /home/data/Allen
fastboot -s FMR0223C20002621 flash boot images/backup/allen-stock-5.10.43-20250805-boot.img
fastboot -s FMR0223C20002621 flash modem_driver images/backup/allen-stock-5.10.43-20250805-modem_driver.img
fastboot -s FMR0223C20002621 reboot
```

Both writes succeed. Post-rollback checks confirm:

- Original `ALN-AL00 / HWALN / kirin9000s` identity, normal boot and
  `sys.boot_completed=1`, with the original August 5 BiSheng kernel.
- Complete boot and modem_driver readbacks match their original backups.
- Complete ramdisk readback matches the unchanged Magisk backup.
- Magisk UID 0, SELinux Enforcing and stock modem_driver loaded.
- `/data` is mounted read-write and the modem_driver ext4 partition read-only.

No erase, format, user-data write, bootloader/NV flash or SELinux mode change is
performed. The unrelated attached device is never operated on. Original images,
r2 candidates and build output remain available; workspace usage stays below
the 20 GB limit.
