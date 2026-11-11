# Stock System Write-Protection Action Audit

## Scope And Baseline

Read-only follow-up on installed r36 (#36), branch bringup/clang12 after
5d7df3291. The previous r36/r32 comparison reproduces vendor_init SIGSEGV
at address 0x14 during sys_wp_init_action in both candidates. PID 1 restarts
the vendor subcontext and Android completes boot. This audit identifies the
missing path and the userspace call context without triggering the action
again, writing protection controls or changing kernel/firmware images.

The installed /init is a symlink to /system/bin/init. Its ELF Build ID is
efa6ef180997cd890a55aec976ce3d27. A temporary readable copy is transferred
and checked against the installed binary, then removed from the device.
The embedded .gnu_debugdata supplies function names for a bounded static
analysis. Binary offsets and live ASLR addresses are not replacement APIs.

## Missing Interface: /proc/sys_wp_soft

The stock vendor action is defined in post-fs at
/vendor/etc/init/init.platform.rc:26. It executes hw_do_sys_wp_init_action
inside u:r:vendor_init:s0, not inside PID 1.

The installed implementation first tries its eMMC path. On failure it uses
the UFS-named branch: open /proc/sys_wp_soft with O_RDWR, write two bytes
containing the string "1" including its terminator, read two bytes back and
check that the first byte is '1'. The logged do_with_ufs ENOENT corresponds
to opening this proc file, not a missing per-device UFS sysfs attribute.
The file is absent on the live device; there is no matching implementation,
Kconfig option or block-layer hook in the published Allen kernel tree.

The retained uncompressed original ARM64 Image also lacks the literal
sys_wp_soft string, while hisi_perf_ctrl is present as a positive comparison.
The saved stock configuration contains neither HW_SYSTEM_WR_PROTECT nor
SCSI_UFS_LUN_PROTECT. This does not prove the original kernel's runtime boot
behavior: it is not flashed in this audit. It does establish that restoring
the old software-WP interface is not simply enabling an existing stock option.

## Userspace Crash: Main-Init Callback In Vendor Subcontext

The action logs success or failure, then calls the same property callback to
set ro.odm.sys.wp to "true" or "false" respectively. On the failure path the
"SYS_WP is disabled" message precedes that call. It reports this particular
action's result, not the state of every hardware or software protection layer.

The callback is android::init::InitPropertySet, not the client-facing property
socket API. Its call chain is:

```text
hw_do_sys_wp_init_action
  -> InitPropertySet
  -> HandlePropertySet
  -> CheckPermissions
  -> PropertyInfoArea::GetPropertyInfo
  -> PropertyInfoArea::GetPropertyInfoIndexes
```

CheckPermissions loads its process-local property-info mapping and passes it
to GetPropertyInfo. GetPropertyInfoIndexes reads a 32-bit field at mapping
offset 20 (0x14), before any check that would handle a missing mapping.

Bounded, read-only /proc/<pid>/mem inspection reads just the callback word
and property-info pointer in PID 1 and the current, restarted vendor_init
subcontext. Both callbacks resolve to InitPropertySet in their own init ELF
mappings. PID 1 has a non-NULL property-info mapping; vendor_init has NULL.
No ptrace attachment, process suspension, function invocation, memory write
or SELinux change is used. The static call path and live child state explain
the previously captured fault address 0x14 without reproducing the crash.
There is no event tombstone/register dump establishing the exact fault PC;
the previous crash-helper failure and that diagnostic limit remain recorded.

Adding only the missing proc file does not repair this context mismatch:
both success and failure branches call the same internal setter. Returning
fake successful protection status would be especially wrong and could still
reach the same crash. Changing ro.secure/ro.debuggable to skip the action is
not a repair either. A proper userspace fix must execute the internal setter
in main init, or use supported parent/property-service communication from the
vendor subcontext while retaining real protection results and permissions.
The matching OEM init/plugin source is absent from the published archive;
no installed binary or init configuration is patched here.

## External Software-WP Reference Boundary

The previously pinned Kirin 9000 Linux 5.10.43 reference,
HK416AAA/HUAWEI_Kirin9000_Kernel at
170c9e979789b1078c1ecdf3816a1766334998fd, also lacks
block/software_system_wp.c and its block build/configuration integration.

A published older reference exists in hz128/huawei_NOH_AN00_kernel at
48b104b9093b9819f24693b65b57d0b399b5c6a3, Linux 4.14.116, an older NOH
platform release rather than Allen 5.10.43. Its implementation is examined
for the interface contract only, not imported:

- HW_SYSTEM_WR_PROTECT builds block/software_system_wp.c. The proc interface
  stores a flag and returns it through seq_file; it is not the whole feature.
- should_trap_this_bio enforces that flag against named system/cust/vendor/
  product partitions, including A/B names, and fails protected writes with EIO.
  A complete implementation requires its real block-I/O integration.
- The old partition naming and 4.14 BIO submission contracts are not a
  validated mapping for Allen's 5.10 dynamic partitions/device-mapper stack.
  Neither the matching hook nor this configuration exists in the local tree.
- The separate optional UFS fPowerOnWPEn path is not equivalent to this
  software partition-write policy. It is not enabled or invoked as a substitute.

Only the source provenance, contract and adoption boundary are retained.
No write-protection emulation, hard-coded partition list, stub proc node,
hardware flag write or partial 4.14 backport is introduced.

## Outcome And Next Boundary

The missing path and userspace crash mechanism are identified, but the stock
action remains unresolved. The matching userspace source and a verified 5.10
protection contract are prerequisites for repairing it without fake status
or a protection bypass. This is not attributed to the restored PERF_CTRL
provider and is not treated as full security certification.

Device remains on r36 in normal Android. Kernel source/configuration, paired
images, independent Magisk ramdisk, init binaries/scripts, node permissions,
SELinux rules, calibration and user settings remain unchanged. Enforcing,
taint 4 and module signature enforcement Y are retained. No reboot, flash,
fault injection or earlier functional test is repeated. Temporary copies,
downloaded source, symbols, disassembly and full logs are removed after the
record is checked. The independent RTNL duration-accounting defect is an
available next kernel-side iteration; it is not mixed into this action audit.
