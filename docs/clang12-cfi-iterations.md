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
