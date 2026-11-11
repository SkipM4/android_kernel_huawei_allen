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
  hw_sate_init_th path; invalid sysfs permissions and duplicate proc entries.
  The sysfs-group warning was initially classified too broadly: later full
  context attributes it to invalid permissions, not duplicate group creation.
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
  module version (one), satellite GPIO can-sleep access (sixteen), invalid
  sysfs attribute permissions (one) and proc registration (two). S/W taint
  remains 516.

Next isolate the satellite init_gpio reads. Their caller is the driver's
initialization kthread, so the standard sleeping GPIO read API is suitable;
retain GPIO direction/output values and do not exercise satellite service
or issue radio-control writes for this diagnostic repair.

## r23: Use Sleeping GPIO Reads In Satellite Initialization

Base: r22, commit `b34ac92f9`. Sixteen boot warnings have the same chain:
gpiod_get_raw_value -> init_gpio -> hw_bbic_dev_init -> hw_sate_init_thread
-> kthread. The two reads in init_gpio use the atomic GPIO API despite
the controller's can_sleep flag. This dedicated kthread holds no spinlock
or interrupt-disabled section across the helper and already calls GPIO
request/direction operations, so sleeping reads match its context.

Change only these two calls to gpio_get_value_cansleep in
`drivers/hwnet/hw_sate_comm/dev/bbic_dev.c`. Both legacy APIs read the raw
physical level through the same GPIO backend; no active-low translation,
direction/output value, power/reset sequence, SIM switching, timing delay,
DT value or satellite-service policy changes. Other GPIO users remain
separate audit work, rather than being globally replaced without context.
Reference: this tree's Linux 5.10.43 gpiolib and asm-generic/gpio.h contracts;
no external implementation is adopted.

The remaining sysfs warning is specifically `succ_value` mode 0774 in
hw_pd.c: the core reports invalid executable bits and masks the mode to
0664. This is not duplicate group registration. Duplicate module-version
paths are /module/tcpc_class/version and /module/nkd/version; duplicate
proc entries are /proc/ion_process_info and /proc/memtrack through
memcheck_ion_createfs. These call sites are separate future repairs.

Checks after paired r23 flash:

- Diff checks, Clang 12.0.7 ThinLTO and paired staging pass; modem imports,
  signature and filesystem/metadata checks pass. The linked init_gpio helper
  calls gpiod_get_raw_value_cansleep at both read sites.
- Android reaches #23 and boot_completed=1. Full boot/modem_driver readback
  matches the candidate pair; independent Magisk ramdisk is unchanged.
  The matching module is loaded, sig_enforce is Y and SELinux is Enforcing.
- Full log starts at 0.000000. All sixteen GPIO warnings disappear; the
  twenty-two init_gpio raw-level records exactly match the preceding r22
  boot. No bad rwlock, CFI failure or panic is recorded. Five other warnings
  remain; this does not certify satellite communications or all GPIO callers.
- USB power remains present. A read-only check finds the existing PD state,
  CC orientation and vbus-only attributes accessible as root. No satellite,
  PD completion or power-control attribute is written for this test.

## r24: Declare A Valid PD Sysfs Mode

Base: r23, commit `770213707`. Change only the succ_value DEVICE_ATTR mode
in `drivers/huawei_platform/usb/pd/hw_pd.c` from 0774 (S_IRWXU | S_IRWXG |
S_IROTH) to 0664. sysfs already masks executable bits to 0664 while warning,
so this supplies the same effective kernel creation mode without invalid
bits. Keep the attribute name, store callback, cable completion/state logic
and absence of a show callback unchanged. Do not change the core validator,
write a fake cable result, or use chmod as a driver fix.

The live r23 attribute has mode 0666, owner/group root/root, after userspace
setup. That later permissions policy is separate from its declared creation
mode; this change neither introduces nor fixes it. Reference: Linux 5.10.43
fs/sysfs/group.c's mode mask and the attributed pd_dpm_probe call chain.
No external implementation is adopted.

Checks after paired r24 flash:

- Diff checks, Clang 12.0.7 ThinLTO, boot round-trip and signed matching
  modem staging pass; all 390 versioned imports match and CMS/filesystem
  metadata checks pass.
- Flash boot and one normal reboot both reach #24, boot_completed=1 and
  normal mode. The Huawei home launcher is present on the second boot.
  Full partition readback matches the r24 boot/modem_driver pair and the
  unchanged independent Magisk ramdisk. The module is loaded, sig_enforce
  is Y, and SELinux remains Enforcing.
- Both full logs start at 0.000000. The invalid succ_value permissions
  warning disappears; no GPIO can-sleep, bad-rwlock, CFI failure or panic
  messages appear. WARNING count falls from r22's 21 to four: one early
  PRMEM drain, one module-version warning, and two duplicate proc entries.
  CPU-feature S-taint and W-taint remain, with taint 516 unchanged.
- Existing PD/CC/vbus-only values remain 1/1/0. The live succ_value mode
  remains 0666 root/root, consistent with the earlier userspace override.
  USB power stays present. No succ_value write, cable-completion injection,
  PD negotiation/OTG operation or permissions-policy change is tested.
  Wi-Fi and Bluetooth settings remain at their original disabled values.

Current test pair:

- `images/test/allen-boot-test-5.10.43-clang12-r24.img`
- `images/test/allen-modem_driver-test-5.10.43-clang12-r24.img`

Retain accepted r17, the latest r20-r24 pairs (including the r20 functional
reference), and stock originals. Keep expanded printk retention during these
diagnostic iterations. This is a boot/targeted-fix candidate, not complete
audio, radio, suspend or long-term stability certification. See
`clang12-functional-tests.md` for the newly attributed audio-codec gap and
the later corrected Bluetooth-probe scope. Restore compatible audio implementation
before enabling its absent configuration rules; PRMEM ordering and CPU-ID
mask changes remain separate, security-sensitive analysis tasks.

## r25: Keep A Single Owner For The ION Proc Interfaces

Base: r24, commit `846fb79aa`. Both ION and DFX memcheck register
`/proc/ion_process_info` and `/proc/memtrack`. ION's subsys_initcall runs
before memcheck's module_init and already owns both nodes. Its process
report lists ION descriptors and per-process totals; memcheck's competing
implementation lists generic DMA buffers with a different column layout.
Replacing the active provider would therefore change the existing ABI.

In `drivers/hwmemcheck/memcheck_account.c`, call memcheck_ion_createfs only
when CONFIG_ION is absent. ION is a bool in this tree, not a loadable module;
the enabled allocator unconditionally registers these nodes. Keep their
names, ION callbacks, modes and labels unchanged. Other memcheck proc nodes,
hooks and its exported memory-report function remain available. No proc-core
warning suppression, runtime node lookup, provider replacement or fallback
for failed allocator initialization is introduced. Reference: this tree's
Linux 5.10.43 ION/memcheck call chains and standard compile-time ownership.
The no-ION registration branch is preserved but is not device-tested.

Checks after paired r25 flash:

- Diff checks, Clang 12.0.7 ThinLTO and signed matching module staging pass;
  all 390 imports match, with CMS/filesystem/metadata and boot checks passing.
- Android reaches #25, normal mode and boot_completed=1; the Huawei launcher
  is present. Full boot/modem_driver readback matches r25 and independent
  Magisk ramdisk matches its original. The modem module is loaded,
  sig_enforce is Y and SELinux remains Enforcing.
- A temporary syscall-only probe queries the available heaps, selects the
  reported system heap, allocates uncached 4-KiB and cached 12-KiB buffers,
  maps and verifies all generated bytes. Both proc interfaces report its
  live buffers with matching PID/sizes; the process report totals 16 KiB
  and removes the descriptors after unmap/close. The same check passed
  on r24. This is CPU mapping/accounting coverage, not cross-device DMA
  synchronization, secure heaps or camera certification.
- Both proc nodes retain their modes/SELinux types; rss_topn remains present.
  The early log begins at 0.000000 and both duplicate proc warnings disappear.
  WARNING count falls from four to two: PRMEM early LRU drain and duplicate
  module-version registration. No CFI failure, BUG or panic is recorded in
  the boot/post-check logs. The older current r24 ring had already rolled
  past early boot, so its empty warning search is not treated as a clean boot.
- Wi-Fi/Bluetooth remain at their original disabled settings. No audio,
  peer-radio transfer, deep suspend or long-term stability pass is claimed.

Retain accepted r17 and the latest r21-r25 pairs; retire the older r20
artifacts after this validation, while keeping its functional results and
all original backups. Next repair the duplicate module version declarations:
the final ELF has exactly two colliding owners, tcpc_class (core plus regmap)
and nkd (platform plus direct-device component). Preserve the active version
values and all component implementations; do not bypass sysfs checks.
