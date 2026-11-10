# Clang 12 CFI Bring-Up Iterations

Baseline: official ALN HarmonyOS 4.2 Linux 5.10.43 sources, Android Clang
12.0.7 r416183b1, the stock-compat ThinLTO configuration and serial
`FMR0223C20002621`. Standard CFI and forced signed-module loading remain
enforcing; the independent Magisk ramdisk is not changed.

## r3: MAS Request Completion (2026-11-10)

The r2 failure and verified original-image rollback are recorded in
`docs/clang12-r2-flash-test.md`.

Change only the proven MAS request-completion registration:

- Add `mas_blk_busyidle_set_end_io` in
  `platform_source/basicplatform/block/mas_blk_busy_idle_interface.c` and its
  declaration in the corresponding platform header.
- Replace the raw callback-address store in `mas_blk_busy_idle_notify.S` with
  a direct call to this compiled C helper. Saved callee registers and the
  existing completion/bookkeeping behavior are preserved.

This uses the standard Android noncanonical-CFI model already selected by the
official kernel Makefile. It does not change CFI flags or bypass its checks.
Final disassembly confirms that C sets `rq->end_io` to
`__cfi_mas_blk_busyidle_end_rq.cfi_jt`, not the implementation address. The
helper uses the compiled request layout rather than another hardcoded offset.

Full incremental kernel/module compilation succeeds. The matching module has
390 versioned imports, no missing exports and no CRC mismatches. Its SHA-512
CMS signature verifies against the local certificate embedded in this kernel.
Boot repack round-trip/header/stub/DTB checks and signed-module/ext4/xattr
readback pass.

Test pair:

- `images/test/allen-boot-test-5.10.43-clang12-r3.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r3.img`.
- Matching signed module: `images/test/allen-modem_driver-test-5.10.43-clang12-r3.ko`.

After Android identity, partition-capacity and original-backup checks, fastboot
model/unlock checks pass through the same Huawei OEM read-only commands as r2.
Both writes return OKAY with no intervening reboot. The built kernel identifies
as `#3 SMP PREEMPT Tue Nov 10 18:56:38 CST 2026`.

r3 still does not reach Android/ADB and returns to fastboot. Engineering
recovery's current console pstore identifies `5.10.43 #3` and:

```text
[14.206777s] Kernel panic - not syncing: CFI failure (target: __cfi_ufs_mq_sync_burst_check_timer_expire+0x0/0x20)
call_timer_fn -> __run_timers -> run_timer_softirq
```

The r2 completion target is no longer the observed failure. The new concrete
failure is the MAS/UFS scheduler's sync-burst timer: its published assembly
passes the raw wrapper address directly to `init_timer_key`. Restrict the next
change to this timer-registration boundary, materializing its callback in C
while preserving flags and lockdep arguments. Do not batch-convert untested
callbacks or disable CFI.

Both original boot and modem_driver images are restored before reboot. Normal
Android, boot completion, original August 5 BiSheng kernel, loaded stock modem
module and SELinux Enforcing are verified. Complete partition readbacks match
both originals and the unchanged Magisk ramdisk backup. Full build/diagnostic
logs are temporary; only the failure evidence above is retained in Git.

## r4: MAS/UFS Sync-Burst Timer (2026-11-10)

Add `mas_ufs_mq_sync_burst_timer_init` in the existing ioscheduler C interface
and its platform-header declaration. Only the failing sync-burst timer's
assembly initialization is redirected to this C helper. It calls
`init_timer_key` with the original zero flags and NULL name/key arguments.
Other timer registrations are not changed speculatively.

Full incremental compilation succeeds. Final disassembly confirms the callback
argument is `__cfi_ufs_mq_sync_burst_check_timer_expire.cfi_jt`. The new reusable
`scripts/stage-allen-modem.sh` stages and validates the matching signed modem
partition; ABI, cryptographic signature, filesystem/metadata readback and boot
repack checks pass. Standard CFI and module-signing enforcement remain enabled.

Test pair:

- `images/test/allen-boot-test-5.10.43-clang12-r4.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r4.img`.
- Matching signed module: `images/test/allen-modem_driver-test-5.10.43-clang12-r4.ko`.

Device/partition/original-backup checks and fastboot model/unlock confirmation
pass. Both flashes succeed without an intervening reboot. Kernel build is
`#4 SMP PREEMPT Tue Nov 10 19:35:32 CST 2026`.

r4 returns to fastboot without reaching Android/ADB. Current console pstore
from engineering recovery identifies `5.10.43 #4` and:

```text
[14.278937s] Kernel panic - not syncing: CFI failure (target: __cfi__ufs_mq_complete_request_remote+0x0/0x1c)
flush_smp_call_function_queue -> ipi_handler
Comm: fsck_boost
```

The new failing address is a remote I/O completion IPI callback, not the r3
sync-burst timer target. The same ioscheduler assembly directly assigns its
raw wrapper address to `rq->csd.func` before
`smp_call_function_single_async`. Next move only that CSD initialization to
compiled C; retain the original request data, zero flags and destination-CPU
selection. Preserve/reload any caller-saved registers needed after the new
helper call. No global CFI relaxation is justified by these results.

Both originals are restored, normal Android returns with boot completion,
the August 5 BiSheng kernel, loaded stock module and SELinux Enforcing.
Byte-for-byte readbacks confirm original boot/modem_driver and unchanged
Magisk ramdisk. Temporary full logs are removed after documenting the evidence.
