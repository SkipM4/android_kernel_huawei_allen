# Early Boot Diagnostics And Remaining MIPS Contract

## r21: Retain The Pre-ADB Log (2026-11-11)

Base: functional r20, commit `0c33a6a63`, on ALN-AL00 / HWALN,
serial `FMR0223C20002621`. The normal r20 system still has taint 516 (S/W).
Earlier successful checks only covered logs beginning around 13 seconds;
they did not establish that all startup warnings were absent.

Read the full available ring, rather than relying on toybox dmesg's default
last-byte limit. A serial-pinned unchanged-r20 reboot with an explicit 2-MiB
read still begins at 13.40 seconds. The kernel already has LOG_BUF_SHIFT=20,
and a larger BusyBox read cannot recover overwritten records. Both user-space
read limits and kernel retention must be considered; this is not just a
missing dmesg option.

Change only `CONFIG_LOG_BUF_SHIFT=23` in the compatibility fragment to retain
an 8-MiB standard printk text ring during diagnostic iterations. Comparing
the resolved config with the running r20 `/proc/config.gz` confirms this is
the only option delta. Ring descriptors/info also consume additional RAM;
this is a temporary diagnostic footprint, not a performance or security fix.
The blackbox registration uses the compiled buffer and metadata sizes, not
an assumed 1-MiB length. Do not change DT reservations, frequency registers,
console log level, CFI, SELinux or warning/taint handling.

Reference: this tree's Linux 5.10.43 `init/Kconfig`, printk ring-buffer and
`rdr_ap_adapter.c` registration contracts. No external implementation is used
for this config change. Keep paired r20, accepted r17 and stock originals for
rollback; the separate Magisk ramdisk remains untouched.

The unchanged r20 reboot additionally reveals a nonfatal warning at 24.44
seconds:

```text
system_server / HwActivityTaskM -> netlink_sendmsg -> kernel_ntl_receive
 -> add_tcp_recover_rule -> _raw_write_lock_bh -> rwlock bad magic
```

This is not the ION reuse lock fixed in r19. In
`drivers/hwnet/hw_dpi_mark/dpi_hw_hook.c`, `g_tcp_recover_lock` is declared as
a zeroed `static rwlock_t` and has no initializer. The other two rwlocks in
that subsystem are initialized by its init function. Next give only this
proven faulty lock a standard static initializer; rule semantics, whitelist
contents and netlink ABI are separate from that repair.

Checks and findings after paired r21 flash:

- Clang 12.0.7 ThinLTO build, diff checks and paired staging pass. The signed
  modem module has 390 imports with no missing exports or CRC mismatches;
  CMS, filesystem/metadata and boot-component checks pass.
- Normal Android reaches build #21 and boot_completed=1. Full partition
  readback matches the r21 pair; Magisk ramdisk is unchanged, the matching
  module is loaded, sig_enforce is Y and SELinux remains Enforcing.
- An explicit 16-MiB dmesg read now starts at 0.000000, with the #21 version
  and primary CPU boot line retained. The diagnostic retention goal succeeds
  without clearing the ring, suppressing messages or changing driver code.
- The first W-taint warning is before secondary-CPU startup: PRMEM's
  physical-pool init calls alloc_contig_range, lru_cache_disable and
  __lru_add_drain_all from start_kernel. The drain function correctly warns
  that mm_percpu_wq is not yet initialized. Its guard must not be removed;
  the PRMEM reservation/init ordering needs a separate fix.
- S-taint is the CPU-feature variation check, not mixed EL1/EL2 startup or
  a DMA cache-alignment warning. CPU0 reports MIDR 0x411fd461, while CPU4
  reports 0x482fd422; several ID feature registers differ and trigger strict
  mask sanity checks. Do not zero the taint or relax unknown reserved-field
  checks without an architecture-specific compatibility analysis.
- Other retained warnings: duplicate module-version sysfs creation;
  sixteen can-sleep GPIO accesses through gpiod_get_raw_value in the
  hw_sate_init_th path; duplicate sysfs-group and proc entry registration.
  These are distinct caller/ordering issues, not new CFI failures.
- The TCP recovery bad-rwlock warning reproduces at 24.73 seconds, confirming
  the same netlink rule-update failure seen on unmodified r20. No panic or
  CFI failure is recorded. Taint remains 516 because diagnostics do not fix
  these faults.

Keep expanded log retention while resolving these boot faults, and reassess
the extra RAM footprint after diagnostics. r21 is a diagnostic candidate,
not a warning-free or performance-certified replacement for the functional
r20 baseline.

## MIPS Multi-Level Reference

The live L3 c1/c2 nodes both have `nr-level=2` and three-cell frequency rows.
Their vote-reg fields occupy separate halves of a shared register. Current
published ALN code parses only CPU/one-target rows and emits one scalar vote;
r15's lifetime fix prevents failed probes from leaving dangling monitors,
but does not implement the absent multi-level algorithm.

A pinned external reference now confirms the table-parsing convention:

- Repository: `HK416AAA/HarmonyOS6_Hi36C0_Kernel`, commit
  `029c553e42179d4b862c320ca39ba207419771d2`.
- Root Makefile version: Linux 5.10.97; repository describes HarmonyOS 6 /
  Hi36C0, not certified as the Allen/Kirin 9000S hardware implementation.
- Files: `samples/ldk/power/mips_mem.c` and `mips_memlat.h`.
- It reads `nr-level`, parses one CPU column plus nlevel target columns,
  represents target frequencies as an array in kHz, and consumes
  `refill-event-id` through an LDK interface.
- It does not provide the matching scalar/multi-level vote-selection and
  register-encoding algorithm in these files. Its device-info layout,
  lifetime handling and private interface differ from the ALN source.

Use this as evidence for table shape only, not as permission to discard the
second target, copy the probe lifetime bugs, substitute its structs into ALN,
or guess how two targets are encoded. No external code is adopted in r21.
Next locate a compatible voting implementation or original-kernel behavior
before restoring these monitors.

## r22: Initialize The TCP Recovery Rule Lock

Base: diagnostic r21, commit `465d1c8bc`. Change only the declaration of
`g_tcp_recover_lock` in `drivers/hwnet/hw_dpi_mark/dpi_hw_hook.c` to
`static DEFINE_RWLOCK(g_tcp_recover_lock)`. Standard initialization supplies
the raw lock, debug magic and owner state before any netlink rule update.
No duplicate late initialization, new fallback, whitelist alteration, rule
algorithm change, callback-type change or netlink ABI change is introduced.
The 8-MiB diagnostic log ring stays enabled so remaining earlier faults
remain visible. Reference: Linux 5.10.43's rwlock initializer and the
attributed r20/r21 write_lock_bh call chain; no external driver code.

The separate existing whitelist-reader/writer concurrency semantics are
not redesigned by this initialization fix. A local TCP smoke test is useful
for transport regression checking, but does not certify concurrency, rule
policy or physical networking.

Checks after paired r22 flash:

- Clang 12.0.7 ThinLTO build and paired staging pass. The signed modem
  module's 390 imports match; CMS, filesystem/metadata and boot checks pass.
- Final ELF data contains the standard initialized queued rwlock, including
  debug magic and no-owner state. No lock-check suppression is introduced.
- Normal Android reaches build #22 on the flash boot and one normal reboot,
  with boot_completed=1 and the Huawei launcher present. Full boot and
  modem_driver readback match the staged pair; independent ramdisk matches
  the original Magisk image. SELinux remains Enforcing and sig_enforce is Y.
- A temporary shell-context Java probe completes sixteen 4096-byte TCP
  loopback echo rounds with exact content comparison, then removes its DEX.
  This is transport regression coverage, not an external network test.
- Full logs start at 0.000000. Neither boot has bad-rwlock, CFI failure or
  panic messages. The first observation exceeds the r20/r21 postboot warning
  window. No positive TCP whitelist-insertion trace occurs, so successful
  whitelist insertion/deletion is not claimed or forced for this check.
- The 21 unrelated warnings remain: PRMEM early LRU drain (one), duplicate
  module version (one), satellite GPIO can-sleep access (sixteen), duplicate
  sysfs group (one) and proc registration (two). S/W taint remains 516.

Next isolate the satellite init_gpio reads. Their caller is the driver's
initialization kthread, so the standard sleeping GPIO read API is suitable;
retain GPIO direction/output values and do not exercise satellite service
or issue radio-control writes for this diagnostic repair.
