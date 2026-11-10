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

## r5: MAS/UFS Remote Completion CSD (2026-11-10)

Add `mas_ufs_mq_init_remote_csd` to the existing ioscheduler C interface and
declare it in its platform header. It initializes the callback, request data
and zero flags exactly as the existing `blk_mq_complete_request_remote` in
`block/blk-mq.c`. Redirect only the proven raw assembly CSD initialization.
Reload the caller-saved request-context pointer after the new C call; retain
the original online-CPU check and `smp_call_function_single_async` destination.

Compilation succeeds. Final disassembly verifies the CSD field offsets match
the published assembly layout and `rq->csd.func` uses
`__cfi__ufs_mq_complete_request_remote.cfi_jt`. Boot repack and matching signed
module staging checks pass, with 390 imports, no missing exports and no CRC
mismatches. CFI, signature enforcement and SELinux are not relaxed.

Test pair:

- `images/test/allen-boot-test-5.10.43-clang12-r5.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r5.img`.
- Matching signed module: `images/test/allen-modem_driver-test-5.10.43-clang12-r5.ko`.

Identity, partition bounds, original backups and fastboot unlock/model checks
pass; both candidate flashes succeed. Build is
`#5 SMP PREEMPT Tue Nov 10 20:24:04 CST 2026`. Android/ADB does not start; the
device returns to fastboot. Recovery console pstore identifies `5.10.43 #5`:

```text
[14.391978s] Kernel panic - not syncing: CFI failure (target: delayed_work_timer_fn+0x0/0x2c)
call_timer_fn -> __run_timers -> run_timer_softirq
```

The remote-completion CFI target is no longer the observed failure. The new
target is the standard delayed-work timer callback: MAS ioscheduler, flush,
busy/idle and background-operation assembly contains raw references to this
exported symbol. Rewriting each initialization site would be unnecessary
complexity. The existing kernel `__cficanonical` attribute is already used by
initcall stubs to make assembly-visible symbols valid CFI entry points.
Next apply it only to this proven failing callback and inspect the linked
symbol before testing; this preserves CFI type checks and the workqueue body.

Both original partitions are restored and verified byte-for-byte. Normal
Android boot, original BiSheng kernel, loaded stock modem module and Enforcing
SELinux are confirmed; the Magisk ramdisk readback is unchanged. Full logs are
removed after retaining this evidence.

## r6: Canonical Delayed-Work Timer Entry (2026-11-10)

Apply the existing `__cficanonical` attribute only to
`delayed_work_timer_fn` in `kernel/workqueue.c`. Its body, timer argument and
export stay unchanged. The attribute is defined by `compiler-clang.h` and is
already used with `__ADDRESSABLE` by the kernel's initcall stubs when assembly
takes a symbol address. Unlike `__nocfi`, it provides a valid canonical CFI
entry; it does not disable type checks. This exported function is already
addressable, so no new address-keeping helper or assembly rewrite is needed.

Compilation succeeds. Final disassembly shows `delayed_work_timer_fn` as a
BTI/jump-table entry branching to the unchanged `delayed_work_timer_fn.cfi`
body. Published MAS assembly now resolves its raw references to that checked
entry. Boot repack, the matching module's 390 imports/zero missing exports/zero
CRC mismatches, embedded certificate, CMS signature and ext4/metadata
readback checks all pass.

Test pair:

- `images/test/allen-boot-test-5.10.43-clang12-r6.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r6.img`.
- Matching signed module: `images/test/allen-modem_driver-test-5.10.43-clang12-r6.ko`.

Device identity, target capacities, original backups and fastboot model/unlock
are verified. Both writes succeed without an intervening reboot. Kernel build
is `#6 SMP PREEMPT Tue Nov 10 20:56:52 CST 2026`.

r6 still does not reach Android/ADB and returns to fastboot. Current console
and dmesg pstore records both identify `5.10.43 #6`. Inspecting the first panic,
not just the end of the recursive trace, establishes two distinct failures:

```text
[14.390300s] CFI failure (target: __cfi_mas_blk_idle_notify_work+0x0/0x1c)
Workqueue: busyidle_notify; process_one_work -> worker_thread
[15.390935s] CFI failure (target: __cfi_mas_blk_panic_notify+0x0/0x1c)
atomic_notifier_call_chain -> panic
```

The first fault is the busy/idle work callback; the panic-notifier fault occurs
while reporting it and recurs until platform crash capture. The delayed-work
timer target is no longer the observed failure. Both new targets have proven
raw assembly references: the work initializer materializes a symbol address,
and `mas_blk_dump.S` stores the notifier symbol in its static notifier block.
Next apply canonical CFI entries to these two observed callbacks and verify
their linked addresses; retain the actual work and panic-reporting behavior.
Do not mistake the secondary notifier panic for the initiating failure.

Both original partitions are restored and full readbacks match their backups.
Normal Android boot, the original BiSheng kernel, loaded stock modem module and
Enforcing SELinux are verified; the independent Magisk ramdisk is unchanged.
Full logs are removed after extracting this evidence. The five most recent
candidate pairs (r2 through r6) are retained within the 20 GB workspace limit.
