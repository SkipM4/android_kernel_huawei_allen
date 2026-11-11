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

## r26: One Version Declaration Per Combined Module

Base: r25, commit `bf6ddc74a`. The final ELF's __modver table confirms the
only two duplicate groups: tcpc_class contains core 2.0.20_G and regmap
1.2.1_G, while nkd contains two V1.0 declarations. Kbuild combines each
pair of translation units under a single module name, so the second
declaration attempts to create the same version attribute. WARN_ON_ONCE
reports only one warning even though both paths collide.

Remove MODULE_VERSION from the rt1715 rt-regmap helper and from NPU
direct_device/npu_direct_devinit.c. Keep it in tcpci_core.c and
npu_platform.c, preserving the values already visible on r25. Retain
component code, initcalls, descriptions/licenses and the regmap's own
version constant/log. No version_sysfs_builtin/sysfs guard changes or
renamed module/interface are introduced. Reference: this tree's Linux
5.10.43 MODULE_VERSION macro, composite Kbuild rules and attributed ELF
entries; no external driver code is adopted.

Checks after paired r26 flash:

- Diff checks, Clang 12.0.7 ThinLTO, boot round-trip and signed matching
  modem staging pass; all 390 imports, CMS and filesystem metadata match.
- The linked __modver table drops from 31 entries to 29 with no duplicate
  owners. Every previously visible version remains identical, including
  tcpc_class=2.0.20_G and nkd=V1.0; unrelated metadata entries are unchanged.
- Flash boot and one normal reboot reach #26, normal mode and
  boot_completed=1, with the Huawei launcher present. Full boot/modem_driver
  readback matches the r26 pair; independent Magisk ramdisk is unchanged.
  The module is loaded, sig_enforce is Y and SELinux remains Enforcing.
- Both logs begin at 0.000000 and contain no duplicate proc/version,
  BUG, CFI failure or panic. Only the early PRMEM LRU-drain WARNING remains,
  reducing the r24 count of four to one. CPU-feature S/W taint stays 516;
  this does not resolve CPU-feature differences or certify PRMEM protection.
- PD/CC/vbus-only state stays 1/1/0 and USB power remains present. NPU0,
  manager and direct nodes retain their modes, ownership and SELinux types.
  These are registration/state regression checks, not NPU inference,
  PD negotiation, OTG or radio certification. Wi-Fi/Bluetooth remain OFF.

Retain accepted r17, latest r22-r26 pairs and all original backups; retire
r21's older candidate artifacts after this validation. Its diagnostic
8-MiB printk configuration remains enabled in the current build.

The remaining warning needs a separate security-sensitive lifecycle audit:
prmem_init runs in mm_init, before workqueue_init and init_mm_internals
create mm_percpu_wq. Its alloc_contig_range path invokes the LRU drain;
PRMEM also requires its physical databases before __wr protection. Do not
move all PRMEM initialization, suppress the drain guard or guess reserved
memory addresses without tracing the earliest consumers and protection
ordering. The newly inconclusive Wi-Fi scan test and calibration read
context are recorded in clang12-functional-tests.md.

## r27: Reserve The PRMEM Physical Database Before Buddy Initialization

Base: r26. A fresh unchanged-r26 boot reproduces the sole remaining WARNING
at mm/swap.c:974, through prmem_try_alloc_contig_range / alloc_contig_range /
lru_cache_disable. PRMEM runs from mm_init before mm_percpu_wq exists; the
LRU drain guard is correct. The initial start_pfn was also calculated from
last_memblock_start before that variable was populated, causing an invalid
first allocation attempt. The subsequent scan succeeds in the highest memory
bank, using physical range 0x800000000-0x800e2a000.

Change only platform_source/see/drivers/prmem/prmem_phys_db.c:

- Reserve the preferred physical database in the existing
  prmem_alternative_alloc callback, after paging_init and before mem_init.
  Use standard memblock_phys_alloc_range for each exact candidate interval;
  it excludes already reserved memory and retains ownership through
  memblock_free_all. Candidate selection still starts at the highest memory
  bank's base and advances in 16-MiB steps. Stop before a full database would
  cross that bank's end; no device-specific address is added to the code.
- Retain the existing 2-MiB-aligned memblock fallback reservation. If the
  preferred reservation succeeds, release only the unused fallback with the
  existing memblock_free_late call. If it fails, warn and use that fallback,
  rather than suppressing a real reservation failure.
- The original mm_init -> prmem_init call and its internal order remain:
  auxiliary mapping, physical databases (page-table database before pmalloc),
  status, mark_wr_data_wr, then protected caches. No LRU/workqueue guard,
  memory-bank definition, pool size, protection call, CFI setting or secure
  region SMC/FFA implementation is changed.
- Reserved pages are compatible with the existing fallback design. PRMEM
  manages their ownership through its own chunk bitmaps; its release path
  does not return these pages to the buddy allocator. This is early boot
  reservation, not a later runtime contiguous-page migration request.

References: this tree's Linux 5.10.43 memblock allocator, ARM64 setup/mm_init,
PRMEM page ownership and protection paths. Also inspect
HK416AAA/HUAWEI_Kirin9000S_Kernel at
991c4dd5c219f90fa0575086bf565c991b3ab2ee (Linux 5.10.43, Kirin 9000S): its
prmem_phys_db.c is identical to the pre-change local file and contains the
same ordering bug, so it supplies no fix to transplant. No external driver
implementation is adopted.

Checks after paired r27 flash:

- Diff checks, Clang 12.0.7 ThinLTO, boot round-trip and signed modem staging
  pass, with all 390 versioned imports present and matching. Linked calls
  confirm memblock_phys_alloc_range replaces the PRMEM contiguous allocator;
  the original prmem_init protection order remains.
- Paired flash and a normal reboot both reach #27, normal mode,
  boot_completed=1 and the Huawei launcher. Full boot/modem_driver readback
  matches; the independent Magisk ramdisk is unchanged. The matching modem
  module loads, sig_enforce is Y and SELinux remains Enforcing.
- Both complete boot logs begin at 0.000000 and contain no WARNING, BUG,
  bad lock, enabled-IRQ exit, CFI failure or panic. Taint drops from 516 to 4:
  the W bit is gone, while the previously attributed CPU-feature S bit
  remains. This is not a claim that every OEM diagnostic message is resolved.
- The selected physical range is exactly the same as r26. The physical
  database capacities remain 42 page-table pages and 3584 pmalloc pages;
  free + used equals capacity, with the observed initial accounting also
  unchanged. PRMEM reports enabled and both SELinux pools report protected.
- Read only the kpageflags metadata for the complete selected region:
  all 3626 pages have PageReserved; none is on buddy, LRU or slab. No physical
  page contents are read. These checks validate reservation and normal pool
  use, not adversarial hypervisor protection, every PRMEM client, deep
  suspend or long-duration memory pressure.

Retain accepted r17, latest r23-r27 pairs, immediate rollback r26 and all
original backups; retire r22 candidate artifacts after this verification.
Wi-Fi foreground scan and automatic saved-network association/DHCP passed
on unchanged r26; see clang12-functional-tests.md. Audio's missing compatible
V2C implementation, multi-level L3 voting and calibration-access correctness
remain separate work. Do not loosen security policy merely because the
calibration open errors coexist with successful scanning and association.

## r37: RTNL Hold-Time Accounting

The OEM __rtnl_unlock diagnostic subtracts a global acquisition timestamp,
prints durations exceeding 300 whole milliseconds and dumps the current
stack. Only rtnl_lock originally initialized that timestamp. Three other
wrapped acquisitions omitted it:

- rtnl_lock_killable: register_netdev uses this path, then calls rtnl_unlock
  through netdev_run_todo. Repeated modem/Wi-Fi registration therefore
  measured from an older acquisition rather than its own successful lock.
- rtnl_trylock: a successful trylock also owns RTNL and can reach the same
  instrumented unlock without starting a new interval.
- refcount_dec_and_rtnl_lock: qdisc_put_unlocked uses the refcount helper and
  unlocks through the same path when the final reference is released.

The fix is limited to net/core/rtnetlink.c. All four wrapped acquisitions
record ktime_get_ns while holding the mutex; killable records only for return
0, trylock only for nonzero and the refcount helper only for true. Failed
acquisitions preserve their original result and do not alter another holder's
timestamp. The existing mutex/refcount primitives are not replaced.
__rtnl_unlock measures with the same clock before releasing the mutex.

ktime_get_ns is the local Linux 5.10 standard monotonic elapsed-time API.
It avoids the per-CPU/bounded-drift contract of local_clock on configurations
with unstable scheduler clocks. Allen currently uses GENERIC_SCHED_CLOCK,
without HAVE_UNSTABLE_SCHED_CLOCK; no observed cross-CPU backwards jump is
claimed here. The reproduced defect is missing acquisition accounting, not
a diagnosed hardware clock fault. No arch clock or timekeeper code changes.

The 300-ms condition, log text, dump_stack, mutex-unlock placement and deferred
skb/registration work remain unchanged. No timestamp guard, lock-owner state,
fake duration or warning suppression is added. Generic netlink dump paths
retain their existing paired raw cb_mutex locking and unlocking; they do not
use this OEM duration diagnostic and are not newly instrumented. The change
does not claim coverage of every possible raw mutex consumer.

Static diff and checkpatch pass with no errors/warnings. Complete Clang #37
build passes. Final ELF confirms success-gated timestamp stores in the three
conditional wrappers, ktime_get in all four acquisition paths and unlock,
and the retained duration test/log/stack path. The whole export set, CRCs,
export types and module identifiers remain equal to r36. The signed matching
modem module has 390 versioned imports, no missing exports or CRC mismatches,
and successful signature and ext4 metadata/readback checks.

The initial r36 ring has already rotated past startup. One normal r36 reboot
therefore obtains a complete time-zero comparison: 24 RTNL duration reports,
including modem registration intervals reported above two seconds. After
paired r37 installation, the full startup and post-query rings have zero
RTNL duration reports, no WARNING/BUG/CFI failure/Oops/panic, and one remaining
wiphy_new_nm stack from its inherited unconditional dump_stack. Read-only
link and IPv4/IPv6 route dumps succeed; no network setting or interface changes.

No deliberate long hold, contention, interrupted killable wait or refcount
destruction stress is injected into the active device. Failure-path behavior
is checked in the final ELF; real over-threshold logging is retained in code,
not demonstrated through forced disruption. This is diagnostic accounting
validation, not network latency, throughput, connectivity or all-lock coverage.
Partition readbacks match r37 and the independent Magisk ramdisk is unchanged.
SELinux Enforcing, taint 4, forced signing and live matching modem module remain.
The stock vendor_init write-protection action still fails independently.
r36 is retained as immediate rollback; temporary files and superseded r29
test artifacts are removed after recording the results.
