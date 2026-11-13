# Linux 5.10 Upstream Synchronization

## Initial Source-Only Boundary

The first source-only synchronization batch advances the Allen tree from
5.10.43 to 5.10.44 on `upstream/linux-5.10.y`. The branch starts at
`13570860d872d99f8930a61bc5f2841a1778f41c`, the r49 validation record.
`bringup/clang12` remains at that baseline, and `stock-5.10.43` retains the
official source import at `881d03c79e8c20d7265fb2667c591e4d858564cd`.
At that checkpoint the installed device remains r49, Linux 5.10.43; the
source-only stage performs no ADB/fastboot operation, module installation
or device regression.

All 131 commits in the adjacent stable range are accounted for. There are
108 imported commits and one separate OEM integration repair, ending at
`dcb7a62fb2f0458b37a0ea1062cedb5a75574ca3`. Independent full compilation
produces Linux 5.10.44 #50. That checkpoint is not a boot pass, complete
5.10.y update, release candidate certification or GKI conversion. The later
user-requested paired installation and scoped runtime results are below.

## Verified Sources

| Reference | Identity | Use |
| --- | --- | --- |
| Linux stable repository | `https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git` | Original ordered commits and provenance |
| Linux v5.10.43 | `951358a824f96be927ae50fad1e72e05bbb57b56` | Exclusive start of the imported range |
| Linux v5.10.44 | `f2b1fc360fa1283b61359f232c6965f4170260f0` | Inclusive end of the imported range |
| Android common merge | `82658bfd8898b71f700cba34de6923d344214fef` | Confirms the same stable release was merged into `android12-5.10-lts` |
| Observed Linux 5.10.y tip | `3a323bc22b881a5bdc1646080f93f5ecb22850bf`, 5.10.274 | Later synchronization target observed on 2026-11-13, not imported |

The official Android common merge is available at
<https://android.googlesource.com/kernel/common/+/82658bfd8898b71f700cba34de6923d344214fef>.
Its parents are `3c79e1658f8a0a39cb41fd27c62729cee700998a` and exactly the
Linux v5.10.44 commit above. This establishes the stable-batch relationship,
not that Allen has all Android common changes.

The official incremental patch and changelog are
<https://cdn.kernel.org/pub/linux/kernel/v5.x/incr/patch-5.10.43-44.xz> and
<https://cdn.kernel.org/pub/linux/kernel/v5.x/ChangeLog-5.10.44>.
The GKI architectural reference is
<https://source.android.com/docs/core/architecture/kernel/generic-kernel-image>.

## Import Method And Ledger

The published Huawei tree was imported as a whole tree and has no shared
upstream Git ancestry. An unrelated-history merge would not establish a
correct common base. Instead, original stable format-patches are applied in
order with `git am --directory=Code_Opensource/kernel`.

Each imported commit retains its original author/date, original upstream
provenance and a stable SHA trailer. Context/implementation adaptations are
marked `BACKPORT` with an Allen-specific explanation. Existing later fixes
are retained rather than replaced by their older intermediate versions.
Present files outside the selected Allen platform also receive applicable
stable fixes; missing, unselected platform drivers are explicitly excluded.

The durable ledger is [linux-5.10.44-imports.tsv](linux-5.10.44-imports.tsv).
It records every stable SHA, local SHA where applicable, outcome, subject,
affected upstream-relative paths and a note where an adaptation or exclusion
needs explanation. A `-` local SHA means no duplicate or inapplicable commit
was created, not an unresolved import.

| Outcome | Entries | Meaning |
| --- | ---: | --- |
| `picked` | 103 | Added/deleted source lines match the original stable commits exactly; directory prefix and hunk offsets may differ |
| `backported` | 4 | Code fixes adapted to existing Allen/Android context |
| `backported` release marker | 1 | Version updated only after all applicable entries are resolved |
| `already-present` | 17 | The fix or a retaining later implementation already exists |
| `not-applicable` | 6 | Required platform-driver source is absent and not selected |

### Adaptations

| Stable SHA | Local SHA | Retained Contract |
| --- | --- | --- |
| `4b8b7bc3a726268e5c15d9bafe27863a85fdfc8e` | `f6239dd42360ae413dba4b1e9fa8a36b406590f3` | Move SPI cleanup from release to unregister while retaining the existing per-controller `add_lock`, not the older global lock |
| `5ca472d40e2daebbfd926701902debcab0bbd15c` | `eb3a10dab85cd80304b888f8c1679fcaeaade151` | Disable cgroup controllers during parsing while retaining the following optional-feature parser; existing `jump_label_init()` precedes normal argument parsing |
| `ad241cb1cffbe7f9d121ba728a381416e8540954` | `4c8edf3f67fc75fdad378536426ecb4b5d4978c9` | Workqueue stall detection snapshots `now` and handles VM suspension without removing the Android lockup trace hook |
| `0ff5f83ae147e63c297e0a5515c9c271b7448f6f` | `e71c827877d4764e68ba002d950a98f178c620cb` | NCM timer starts only for the first aggregate datagram after OEM size validation; upstream `HRTIMER_MODE_REL_SOFT` matches existing initialization, and OEM multi-skb/fallback layout stays intact |
| `f2b1fc360fa1283b61359f232c6965f4170260f0` | `510542275dbb302b501f94839499a62e2918dfd7` | Update `SUBLEVEL` to 44 without removing Huawei Makefile exports |

### Already-Present Boundaries

All 17 identities are in the ledger. Notable cases that must not regress:

- Both proc opener-mm tracking and the later read-permission relaxation
  already exist. Do not reintroduce the intermediate strict-open behavior.
- DWC3 init-failure NULL assignment, exit guard and IRQ synchronization
  already exist. An additional OEM early-return defect is repaired separately.
- TCPM teardown already cancels the requested state/VDM/FRS hrtimers and a
  newer send-discover timer. Do not replace it with an older teardown.
- `scsi_host_alloc()` already has the requested lifetime fixes plus the
  later ehandler reset. Its OEM high-priority workqueue flag is retained;
  applicable later `scsi_add_host_with_dma()` reference fixes are imported.

### Missing Platform Drivers

| Stable SHA | Absent Source | Applicability |
| --- | --- | --- |
| `750a0d75564293be3ed50f13ef7f38ab75106421` | `drivers/usb/dwc3/dwc3-meson-g12a.c` | Amlogic platform glue absent and unselected |
| `2af93b437a61e80df55aa2d321e931d124a93ef7` | `drivers/usb/dwc3/dwc3-meson-g12a.c` | Same absent Amlogic driver |
| `e3a502abf506e5c054b61a7c524caaa48e8e8537` | `drivers/regulator/max77620-regulator.c` | Source absent; `CONFIG_MFD_MAX77620=n` |
| `c365ff97617cbea8b31aa340507c7dc4395da89a` | `drivers/regulator/bd718x7-regulator.c` | Source absent; `CONFIG_MFD_ROHM_BD718XX=n` |
| `5a5f5cfb5f0996d65eae3cc034513d90f4be6783` | `drivers/regulator/fan53880.c` | Source absent; regulator disabled |
| `aafc51fddfa87178eb599e68526620ac0c02dc5a` | `drivers/regulator/rtmv20-regulator.c` | Source absent; regulator disabled |

These exclusions do not establish an all-platform vanilla 5.10.44 tree.
Revisit the ledger if any excluded driver is later restored or selected.

## Separate OEM Integration Repair

Review of the already-present upstream DWC3 IRQ fix identifies a real
Huawei-specific imbalance in `drivers/usb/dwc3/gadget.c`:
`dwc3_gadget_pullup()` returns from its `pcd_suspended` branch after a
successful `pm_runtime_get_sync()` and `disable_irq()`, bypassing both releases.

Local commit `dcb7a62fb2f0458b37a0ea1062cedb5a75574ca3` routes that branch
through the existing common `enable_irq()` / `pm_runtime_put()` exit after
unlocking. It preserves the successful return, `gadget_pullup` update and
OEM suspend/notification policy. It is not claimed as an exact upstream pick.
Final-ELF inspection confirms the suspended branch unlocks, sets return 0
and reaches IRQ enable followed by the PM put implementation before return.
This closes the source/compiled cleanup defect. The later runtime USB check
below does not establish coverage of the exact suspended-pullup branch.

## Initial Build Validation

- Independent full build in `out/allen-clang12-upstream-5.10` completes with
  Android Clang/LLD 12.0.7, ThinLTO and enforcing CFI. Final ELF is AArch64,
  has the CFI check/failure symbols and reports Linux 5.10.44 #50.
- The configuration starts from r49. Effective selections are unchanged;
  the file differs by the release comment and a newly visible disabled
  `CONFIG_NVME_TCP` entry after the upstream dependency correction. Do not
  describe the files as byte-identical or enable a new driver for this batch.
- All 13,821 exported entries retain their names and CRCs relative to r49:
  no additions, removals or CRC changes. The new modem module has 390 imports,
  with no missing exports or CRC mismatch, and matching 5.10.44 vermagic.
- Signing material matches r49 and the matching certificate is embedded in
  the final kernel. A stripped temporary copy of the new module is signed
  with the existing SHA-512 policy and its PKCS#7 signature passes CMS
  verification. The build output module is not installed or image-staged.
- `git diff --check` passes for the source range. The four code adaptations
  and separate DWC3 repair each pass checkpatch with no errors or warnings.
  Whole-batch checkpatch reports 9 errors/21 warnings versus 9 errors/22
  warnings in the original upstream range; diagnostic comparison attributes
  all local findings to unchanged upstream lines. Original picks are not
  reformatted merely to satisfy the older checker.
- Existing compiler/linker warnings remain. Full compilation is not a
  warning-free, all-architecture build or device test.

For the retained configured output, build and ABI inspection use:

```sh
cd /home/data/Allen
OUTPUT_DIR=out/allen-clang12-upstream-5.10 JOBS=8 scripts/build-allen.sh
scripts/check-module-abi.py \
  out/allen-clang12-upstream-5.10/drivers/platform_drivers/modem/modem_driver.ko \
  out/allen-clang12-upstream-5.10/Module.symvers
```

The generic released defconfig is not a substitute for the r49-derived
configuration/signing seed. `LOCALVERSION=` suppresses a Git suffix but does
not override the new 5.10.44 release. The project installs only same-build
kernel/module pairs. The earlier claim that a changed release alone forbids
module loading was too strong: with MODVERSIONS and a version section,
Linux/ACK/Allen `same_magic()` ignores the release token while checking the
remaining attributes and symbol CRCs. That permission is not full ABI safety;
export equality does not prove opaque OEM, layout or Android KMI compatibility.
See [kernel-module-updates.md](kernel-module-updates.md) for the actual rules
and the retained stricter project packaging policy.

## Initial Local Closeout

The installed r49 output, original images, accepted r17 and recent r45-r49
image pairs remain retained. Only the superseded #41 build cache is retired
after its reproducibility metadata is verified and retained in
`out/allen-clang12-r41-metadata`. The useful independent #50 output stays
local. Consumed reference clones, patch files, signature copies and complete
diagnostic logs are removed after their lasting results enter the records.
No new boot/modem image, exported bundle, release tag, push or publication
is created; the workspace stays below the 20 GB limit.

## Paired Installation Follow-Up

The user subsequently requests official module-update references, compilation
confirmation and a device boot test. The source remains the same #50 kernel;
the complete make invocation confirms its existing full build without a new
compile/link or kernel edit. Only the host ABI checker explanation changes
at `9b2cbcc9b`, distinguishing same-build policy from loader eligibility.

### Images And Installation

The device is reconfirmed as ALN-AL00 / HWALN / ALN with the recorded Huawei
Charlotte platform and normal #49 Android. The empty SoC property is not a
different-hardware claim; DT/platform/model agree with the retained device
baseline. Boot, modem filesystem/module and independent ramdisk hashes match
the retained r49 pair and Magisk ramdisk before installation. Read-only normal
Android inventory contains only the loaded modem; the optional dummy-module
init command targets a missing directory on both r49 and #50.

The matched new modem is stripped then signed with the existing trusted
SHA-512 policy. Import CRC, CMS certificate, ext4 consistency, module/label
readback and ownership/mode checks pass. The boot repack passes kernel
decompression round-trip, original firmware stub/OEM DTB/header preservation
and real partition bounds. No independent ramdisk, firmware, data or
high-risk partition is flashed.

Local paired test/recovery files are:

- `images/test/allen-boot-test-5.10.44-clang12-r50.img`.
- `images/test/allen-modem_driver-test-5.10.44-clang12-r50.img`.
- `images/test/allen-modem_driver-test-5.10.44-clang12-r50.ko`.

Serial-pinned fastboot device/product/all checks run before both installations.
The standard product/all queries remain unsupported; supported OEM model and
unlock queries reconfirm ALN-AL00/unlocked. Only the verified `boot` and
`modem_driver` partitions are flashed from the local matched pair.

The first #50 installation reaches normal Android, passes matching partition/
module readbacks and Enforcing/taint4/signatureY. The diagnostic script then
misclassifies an unsupported `cmd user is-user-unlocked` command as candidate
failure and automatically restores both r49 partitions. This is a diagnostic
failure, not a kernel boot fault. The actual supported user-state contract
uses CE availability and `dumpsys user` RUNNING_UNLOCKED. After correcting that
and boot-log attribution, the same #50 pair is installed again successfully;
no speculative kernel edit or repeated compilation follows.

Both #50 boots have complete time-zero logs without actual kernel WARNING,
BUG, Oops, panic, CFI failure, bad module version/CRC or signature error.
The inherited unconditional wiphy stack is not mislabelled as a warning.
Each completed-boot snapshot has 483 AVC records, not an AVC-free pass;
no module-load AVC is found. Logserver capability/property denials, the
missing dummy-module path and unsupported `cpuset_noprefix` name remain
classified against r49 rather than suppressed.

The retained second boot has `sys.boot_completed=1`, user 0 CE storage
available/RUNNING_UNLOCKED, read-write encrypted F2FS data, SurfaceFlinger/
activity/power services and a Live modem module. Boot/modem/module readbacks
match #50 and the full ramdisk readback remains equal. The running modem
build-id matches the new local module, not the r49 module. CFI remains
enforcing, PRMEM reports enabled, SELinux is Enforcing, signature enforcement
is Y and taint stays 4.

### Relevant Scoped Regression

- A task-owned 2 MiB pattern transfers host -> device -> host over the actual
  USB ADB transport with matching device SHA-256 and local byte integrity.
  The new DWC3 IRQ remains active; this is not NCM tethering or exact
  suspended-pullup race coverage.
- A temporary freestanding AArch64 probe uses this tree's ARM64 O_DIRECT flag,
  creates an exclusive owned file, writes 512 aligned 4096-byte blocks,
  fsyncs, rereads and compares every byte. The file is removed, and actual
  UFS hub/core interrupt counts increase. No buffered fallback, shared
  data file or partition write is used for the test.
- One normal passwordless wake/unlock -> stock Settings -> Display Settings
  -> Home -> Sleep sequence is observed without changing a control or
  scheduling configuration. Window/activity state and the temporary
  screenshot confirm Display Settings. The already-bound touch SPI peer
  remains present and has IRQ activity; synthetic UI input is not physical
  touch-controller or SPI hot-unbind certification.
- Two task-owned mono-clock instances record 940 complete real RTG ioctl
  entry/return pairs, including 433 BEGIN_FRAME_FREQ=0, 289 END_FRAME_FREQ=0
  and three END_FREQ=0. The latter show frame disable and ordinary-policy
  restoration of visible selected members before ioctl return. Real UI/
  RenderThread scheduling and final ordinary priorities are observed.
  Full frame timing/power is not certified.
- Six activity requests return the existing ACT_ERR_QOS (-103); the QoS gate
  is preserved and those calls are not counted as successful activity
  coverage. The two buffers have no overrun, commit overrun or dropped events
  on all twelve CPUs. Unknown ioctl boundary exits and outstanding non-RTG
  entries are not assigned RTG results; no known RTG entry is left unpaired.
- Complete overlapping pre/post kernel snapshots plus a live log stream
  cover the regression interval without actual kernel WARN/fatal/module
  faults. Eleven AVCs in the post-baseline log suffix are kept classified:
  five qemu-property reads, five mediaprovider accesses to owned shell data,
  and one motion-HAL Binder denial. No SELinux relaxation occurs. Snapshot
  overlap, not assumed equality of dmesg and monotonic clocks, defines scope.

The second boot ID stays unchanged during regression. Original main/wifi
tracers, module/security state, USB configuration and SPI peer set match.
Owned files, probe process and trace instances are removed; the final
foreground is Home and the device is Asleep with keyguard showing/not
occluded. Automatic brightness and 30000 ms timeout remain unchanged;
automatic brightness output changes normally and is not overwritten.

## Current Local Closeout And Next Stage

#50/5.10.44 now remains installed and has the scoped boot/module/USB/storage/
UI regression above. Its immediate paired rollback is r49/5.10.43. Keep
accepted r17, the latest r46-r50 pairs, the original backups, r49 output and
useful #50 output. Only the superseded r45 test pair and consumed task
diagnostics are retired after validation. No release bundle, tag, push or
publication is made; local test images exist solely for installation/recovery.
The workspace stays below 20 GB.

The next minimal import is 5.10.44 -> 5.10.45, using the same ordered ledger,
duplicate review and isolated matched kernel/module compilation. Continue
bounded stable batches toward a newly pinned 5.10.y target before adding
Android-specific deltas. The #50 pass does not transfer all older hardware
results or cover every changed scheduler/USB/SPI path.

The latest user instruction makes normal Android boot and automatic loading
of matching modules sufficient for subsequent stable milestones. Ordered
source review, matched full builds, signature/import checks, paired readback,
retained security controls and necessary boot-log review still apply. Broader
hardware regressions are not repeated unless a new relevant change, fault or
unresolved issue requires them. This narrower gate does not certify full
hardware functionality or convert stable maintenance into GKI compliance.

GKI requires an Android common core, suitable configuration/KMI, and vendor
hardware support separated into modules. This stable batch removes no
required platform support and does not attempt that architectural split.
After the stable base is established, audit the matching Android common
delta, KMI/module dependencies and OEM core hooks one subsystem at a time.
The remaining r49 functional/manual/source prerequisites remain recorded in
[clang12-completion-plan.md](clang12-completion-plan.md).
