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
  reports 0x482fd422; several ID feature registers differ and are rejected
  by strict masks. Do not zero the taint or relax unknown reserved-field
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
