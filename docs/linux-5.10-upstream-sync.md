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

## Second Milestone: 5.10.45 Through 5.10.49

The official `linux-5.10.y` head and kernel.org release data are refreshed on
2026-11-13 and agree on 5.10.274 at
`3a323bc22b881a5bdc1646080f93f5ecb22850bf`. This remains the pinned long-term
target, not an imported or installed version. The next bounded milestone
groups five adjacent releases, 5.10.45 through 5.10.49, with 298 entries.
The following 5.10.50 release alone has 596 entries and stays separate.

| Release | Official Commit | Picked | Code Adaptations | Marker Adaptation | Already Present | Not Applicable |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| 5.10.45 | `037a447b7a0baa71593cb9a57ac7bdb7b9303c01` | 36 | 0 | 1 | 1 | 0 |
| 5.10.46 | `3de043c6851d7c604e0cabdf8e2aca7797952aa9` | 122 | 5 | 1 | 14 | 3 |
| 5.10.47 | `4357ae26d4cd133a86982f23cb6b321304faac50` | 88 | 9 | 1 | 3 | 0 |
| 5.10.48 | `a09a52277207fa79fc1aa7c32be6035c264a79c4` | 6 | 0 | 1 | 0 | 0 |
| 5.10.49 | `904ad453baa0aae7189ebd07f0d43cb694fb2987` | 6 | 0 | 1 | 0 | 0 |

The per-release `linux-5.10.45-imports.tsv` through
`linux-5.10.49-imports.tsv` ledgers record all stable/local identities and
decisions. All 258 exact picks retain original added/deleted lines. All 277
imported commits retain original authors/dates and stable-SHA trailers.
The remaining 18 fixes are already present; three missing, unselected
regulator drivers (ChromeOS EC, RTMV20 and RT4801) do not affect Allen.
These exclusions are not an all-platform vanilla-kernel claim.

### Adapted Contracts

| Area | Local Commit | Integration Decision |
| --- | --- | --- |
| mlx5 software steering | `3e7ae7830` | Retain the later err.h/IS_ERR/PTR_ERR UAR allocation fix while adding original SW-owner-v2 handling |
| Network namespace | `a58aeddd1` | Move get_net_ns into CONFIG_NET_NS implementation and add disabled stub; retain maybe_get_net zero-reference rejection |
| Cypress hub | `734c7b8c1` | VID/PID constants already exist, but the autosuspend-disable match does not; add only the missing original entry |
| Swap/UFFD | `360c68cc9` | Add upstream flag normalization and both call sites without changing PARA_SWAPD UFS/extent encoding |
| DWC3 endpoint debugfs | `d09dd4341` | Dynamic creation already exists but per-endpoint removal does not; add balanced lookup/remove/dput and preserve whole-root OEM teardown safely |
| ARM64 DMA zones | `63f10c908` | Use upstream standard-mode calculation; retain active dma_zone_only=true full-DRAM boot contract |
| Crashkernel mapping | `c9fa43489` | Add upstream early mapping flag/removal of reservation-specific mappings; retain KFENCE/L3, tagged memory, protected page tables and secure-memory mapping |
| Lockdep instrumentation | `5c4c8f4ea` | Apply original annotations/scopes without replacing the existing debug_locks_off body |
| Inet race annotation | `4fc264c52` | Retain later READ_ONCE protocol snapshot while adding both original inet_num annotations |
| Kthread cancellation | `9e3940767` | Replace post-removal OEM recheck with upstream timer-cancel, counter-check and locked remove/requeue transaction; retain raw worker locks |
| Rmap termination | `d79b5a216` | Use upstream page_not_mapped while retaining targeted-reclaim target_vma interface |
| THP truncation | `a887e5eb4` | Preserve full original single-page zap/truncate fix and existing header wrappers/speculative-fault declarations |
| THP split reporting | `3e981fe34` | Preserve original warning/freeze fix; pass NULL for the existing targeted-reclaim parameter |
| Certificate revocation | `d6971efc0` | Add original preload support alongside existing signing branches; do not change trusted keys or force-signature policy |

The DWC3 adaptation is recorded after its incomplete status is discovered,
not silently classified as already present. The OEM role switch removes the
whole debugfs root before freeing endpoints, while core removal, OTG changes
and init-failure cleanup also reach endpoint freeing. Root/regset pointers
are cleared at whole-root teardown; endpoint creation/removal skip an absent
root rather than use a stale dentry or the global debugfs root. The removal
helper balances the reference obtained by debugfs_lookup. It adds no struct
member or public ABI. Exact failed-init/OTG races are not runtime-certified.

The kthread change adopts upstream return-value semantics rather than merely
adding a second canceling check. Removing a queued work item before learning
that it cannot be requeued can break caller reference accounting. The upstream
transaction checks cancellation after releasing/reacquiring the worker lock
for timer synchronization, then removes/requeues atomically under that lock.

`git diff --check` passes for this milestone. Eleven code adaptations have
clean checkpatch results; the other three retain original stable findings:
hub indentation (1 error/5 warnings), inet data_race annotation comments
(2 warnings) and certificate extern/MAINTAINERS notices (2 warnings). None
is reformatted solely to make the older checker silent.

### Build And Runtime Gate

Kernel source ends at `41fba098de021480fba9c732b2b3c0622d92a457`.
The independent upstream output is reused with the #50 configuration and
identical signing material. Full compilation produces 5.10.49 #51, with
AArch64 final ELF, enforcing CFI/ThinLTO and unchanged effective selections;
the configuration differs only by its version comment. All 13,821 exports
remain, with no added/removed symbols and one changed get_net_ns CRC following
the namespace implementation move. The modem does not import that symbol;
all 390 rebuilt modem imports match. This is not an unchanged complete KMI.

Final ELF inspection confirms endpoint cleanup precedes kfree, the debugfs
lookup reference reaches dput, and whole-root teardown clears root/regset.
Signed-module CMS/certificate, ext4/label/ownership readbacks and boot
kernel-round-trip/stub/DTB/header/partition bounds pass. The verified #50
image pair remains the immediate rollback, and independent ramdisk is not
rewritten. The local matched files use `5.10.49-clang12-r51` naming.

Serial-pinned fastboot identity checks and both partition writes succeed.
One normal #51 boot reaches sys.boot_completed=1, normal mode, CE storage
available/RUNNING_UNLOCKED and SurfaceFlinger/activity/power services. The
modem is automatically loaded Live; its runtime build-id and file agree with
the new signed module. Full boot/modem readbacks match, ramdisk is unchanged,
SELinux stays Enforcing, taint 4, signature enforcement Y and PRMEM enabled.
Complete time-zero and subsequent boot snapshots contain no actual kernel
WARN/BUG/Oops/panic/CFI/module-version/signature faults. There are 489 initial
AVCs, none for module loading; no claim of AVC-free operation is made.
The device is Asleep, no diagnostic tracer/probe is installed, and no broad
hardware regression or second reboot is performed after the required gate
passes. The pinned latest target is not reached.

The superseded full r49 build cache is retired only after retaining and
verifying its configuration, export table, build identity and signing/image
contract in `out/allen-clang12-r49-metadata`. Its local rollback images and
`bringup/clang12` source remain intact, and signing material remains in the
upstream output. This avoids a third full output and keeps the workspace
below 20 GB without deleting original images or unrelated diagnostics.

GKI requires an Android common core, suitable configuration/KMI, and vendor
hardware support separated into modules. This stable batch removes no
required platform support and does not attempt that architectural split.
After the stable base is established, audit the matching Android common
delta, KMI/module dependencies and OEM core hooks one subsystem at a time.
The remaining r49 functional/manual/source prerequisites remain recorded in
[clang12-completion-plan.md](clang12-completion-plan.md).

## Third Milestone: 5.10.50

The adjacent official range is v5.10.49
`904ad453baa0aae7189ebd07f0d43cb694fb2987` through v5.10.50
`43b0742ef44c30f202afbf8355e9326710af9ca1`. All 596 entries are resolved in
[linux-5.10.50-imports.tsv](linux-5.10.50-imports.tsv): 566 exact picks,
12 code adaptations, one release-marker adaptation, ten already-present
fixes and seven inapplicable entries. Independent verification confirms the
official order, unique identities, subjects and paths; all 579 imported
commits retain original authors/dates and stable-SHA trailers, and all 566
exact picks retain original added/deleted lines, modes and binary changes.
The release marker is `728994cfb`; the additional safety prerequisites below
end the candidate source at `75f387b5d75dd5cea2d79c984a986f945e6938f6`.

### Adapted Contracts

| Area | Local Commit | Integration Decision |
| --- | --- | --- |
| DWC3 debugfs creation | `bbe4eec9d` | Create the root before core-mode/endpoint initialization and retain it for the controller lifetime; remove OEM role-switch root churn while retaining vendor files and final/error cleanup |
| GUP compound pages | `8c65621b3` | Adopt the post-reference compound-head recheck, pin bias and reference rollback while preserving OEM page-pinner accounting |
| Credential ucounts | `8ec1f29ec` | Add upstream credential/namespace references, preserving HKIP/checkroot and scheduler UID hooks; check allocation before committing credentials |
| Uclamp group minimum | `a133caf22` | Replace the OEM user-defined-dependent bypass with upstream group minimum protection and maximum limiting; retain independent HISI and Android hooks |
| Uclamp group locking | `1e1171e0a` | Add upstream mutex/RCU scope and assertions; retain the Android group-online hook outside that scope |
| ARM64 reserved page table | `a55474c32` | Adopt standard TTBR/trampoline/linker offsets; retain the OEM 64 KiB protected-region alignment and end marker |
| Uclamp range refresh | `750c054c6` | Clamp requests to the group range and refresh both active clamps; retain the existing helper's transient IDLE clearing |
| PSI trigger lifetime | `51c3850ad` | Initialize waitqueue/timer once per group; stop only the retired worker after RCU, and move the OEM scheduling reset into new-worker creation under the trigger lock |
| Swap fault lifetime | `751bdd285` | Initially adapt the original guard across every regular/speculative-fault exit, retaining CMA/memcg/zram contracts; explicitly reverted before installation below |
| IPv6/GRE segmentation | `0e0377766` | Set the inner IP protocol only in the IP-over-IPv6 caller; retain the later headroom READ_ONCE/WRITE_ONCE fix |
| FunctionFS cross-references | `d099acca0` | Adopt the full upstream bind/release transaction and callback nesting; retain the OEM kill_sb informational log |
| Read-only file THP eligibility | `f1ab0e985` | Adopt the active query and shared VMA helper; retain later file-offset alignment, VM_NO_KHUGEPAGED and OEM hihugepage policies |

Nine adaptations have clean diff-only checkpatch results. The credential,
initial uclamp-minimum and ARM64 changes retain one error/five warnings on
original upstream lines; their original patches report the same findings.
Commit-message checks cannot resolve absent upstream ancestors in the
whole-tree Huawei Git history and are not substituted for source checks.
`git diff --check` passes for the complete source range.

The seven exclusions are absent/unselected UniPhier, MT6358, DA9052,
FAN53880 and Hi655x regulator sources, the absent/unselected legacy Hi6210
I2S source, and a BPF selftest `.gitignore` rule omitted from the published
tree. The last case already leaves tracked `test_progs.h` searchable.
These are not removals of selected Allen hardware support. Existing later
BPF/GSO, block request lifetime and IRQ fixes are preserved rather than
replaced with older intermediate implementations.

### Known Upstream Follow-Ups Brought Forward

Source review identifies real defects in the intermediate upstream release,
not merely context conflicts. The following minimal proven repairs precede
the first installation; they are outside the 596-entry ledger and must be
recognized when their later stable ranges are processed.

| Official Source | Local Commit | Reason |
| --- | --- | --- |
| Linux 5.10.y `b493af3a66e067f93e5e03465507866ddeabff9e`, mainline `bbb6d0f3e1feb43d663af089c7dedb23be6a04fb` | `79d8100ee` | Acquire ucounts before the fallible security hook in both credential constructors; cleanup must not release a borrowed reference |
| Mainline `34dc2fd6e6908499b669c7b45320cddf38b332e1` | `94269d6b1` | Keep the old owned ucounts reference until replacement allocation succeeds; original code loses it on failure |
| Linux 5.10.y `4addb102154b7cf6e2310ccbe20c3c08619e520d`, mainline `5457025fa8ca3c0d2732109513de839e3e797190` | `88fdb3d60` | Synchronize the PSI timer at final cgroup release, using this tree's equivalent del_timer_sync API; a scheduler reader can rearm it after trigger-level deletion |
| Linux 5.10.53 `277b311ae170b90fcc30551d1a30d17fceb113a4` | `1c635e048` | Revert the swap fault guard because the required mainline percpu_ref prerequisite is absent from this RCU-based 5.10 tree |
| Linux 5.10.53 `7d7d0e84ac0d80815abfb7db3ba3e683830b310d` | `75f387b5d` | Apply the matching official shmem swapin revert for the same missing prerequisite |

Sources are original commits from
<https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git> and
<https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git>.
The mainline allocation-failure fix at `94269d6b1` preserves its original
author/date and exact source patch; its explicit source mapping is recorded
in the table because its commit message lacks a SHA trailer. No existing
commit is amended to repair that documentation omission.

PSI kernfs removal drains pressure files and destroys their triggers; the
old poll worker stops and final cgroup freeing follows an RCU grace period.
No remaining scheduler/worker can arm the timer, and its callback does not
rearm itself. Final-group `del_timer_sync()` therefore closes the freed-group
callback window without deleting a replacement trigger's timer. A separate
pre-existing worker wake-consumption race has only an unmerged proposal in
the reviewed sources; it is not presented as an accepted upstream pick or
runtime-certified fix.

The official swap revert discussion is
<https://lists.openwall.net/linux-kernel/2021/07/19/494>. Mainline
`63d8620ecf93` provides the missing percpu_ref-based serialization. Here,
`get_swap_device()` still opens an RCU critical section, so the imported
sleeping fault/I/O paths cannot safely hold it. The first local build is
deliberately stopped before linking/installation when this dependency is
confirmed. The official reverts are preferred over a larger unreviewed swap
reconstruction. The final fault function returns exactly to its pre-guard
implementation, including the earlier OEM policies. This avoids the newly
introduced sleep-in-RCU defect; it does not certify every historical swapoff
race as resolved.

The credential series is itself later reverted by Linux 5.10.y. Those
reverts must be verified and processed in order, not skipped to retain an
unintended permanent feature. The two small early fixes make the interim
series safe on the identified error paths without changing its ABI policy.

### Separate DWC3 Integration Repair

`22e101b95` removes and drains each endpoint debugfs directory before freeing
its TRB ring and endpoint. The controller-lifetime root means role switching
no longer happens to drain all readers through an early whole-root deletion.
The guarded `trb_ring` read can otherwise dereference released ring memory.
Existing debugfs removal waits for active readers and runs outside the DWC3
spinlock; no extra lock, speculative NULL check or hardware policy change is
added. This is a local integration repair, not an attributed upstream pick.

### Candidate ABI Review And Validation Boundary

The credential structure gains a pointer after `user_ns`; the expected
configured size changes from 168 to 176 bytes, with `group_info` and the RCU
union shifted by eight bytes. Existing preceding fields remain unchanged.
All 262 modem object inputs resolve to available C sources, and its source
and undefined-symbol inventory do not use credential/ucount APIs. Twenty
retained OEM assembly units have old credential debug metadata, but scoped
instruction/relocation checks find no corresponding runtime credential
access. HKIP_PROTECT_CRED and DEBUG_CREDENTIALS remain disabled, and active
HHEE root checks rebuild from C. No concrete new opaque-layout break is
identified; this scan is not complete binary ABI certification.

The corrected full build produces Linux 5.10.50 #52 from `75f387b5d` with
Android Clang/LLD 12.0.7, enforcing CFI and ThinLTO. Effective enabled/value
configuration and signing material match #51; configuration text differs
by the version comment and the removed disabled PSTORE_BLK entry. The final
ELF is AArch64, the credential layout is 176 bytes with ucounts at 144 and
group_info at 152, reserved_pg_dir precedes swapper_pg_dir by 4096 bytes,
and the OEM protected-region end retains 64 KiB alignment.

The final export table has 13,822 entries: `__xfrm_state_mtu` and
`tracepoint_probe_register_prio_may_exist` are added, `xfrm_state_mtu` is
removed, and 8,878 retained export CRCs change. This is a substantial KMI
change, not independent old-module compatibility. Both configured modules
rebuild; all 390 modem imports and two dummy-module imports match. The dummy
module remains outside the installed inventory. Final machine-code review
confirms ucounts acquisition before security callbacks, replacement
allocation before releasing the old reference, endpoint debugfs reader
draining before TRB/endpoint frees, and final PSI timer synchronization
before group memory release.

The signed matching modem filesystem and repacked boot image pass CMS,
certificate, label/ownership, filesystem, kernel round-trip, firmware-stub,
DTB/header and partition-bound checks. Their local names use
`5.10.50-clang12-r52`. Only boot and modem_driver are flashed, with verified
#51 images as the immediate paired rollback; independent Magisk ramdisk and
user data are unchanged. Both partition readbacks and the installed signed
module match. Its runtime build-id matches the newly rebuilt module, which
loads automatically as Live without force-loading.

One normal #52 boot reaches sys.boot_completed=1, CE storage available and
RUNNING_UNLOCKED, with SurfaceFlinger/activity/power services present.
Complete time-zero and subsequent boot logs contain no actual kernel WARN,
BUG/Oops/panic, CFI failure or module-load/signature fault. The later capture
contains 494 AVC denials, including existing OEM property/calibration/access
boundaries; none requests module loading. Initial-console and data=journal
notices remain classified rather than suppressed. Enforcing, taint 4,
signature enforcement Y and enabled PRMEM hold. No broad hardware regression
is repeated under the user's stable-milestone acceptance rule. The screen
is Asleep, global tracing is off, and only the original wifi instance remains.

The accepted #52 pair, immediate #51 rollback, latest r48-r52 pairs and
original/r17 recovery files remain local. Replaced #51 and accepted #52
build metadata are retained before output reuse. Consumed diagnostics and
the superseded r47 pair are removed under the retention rule; upstream
reference/import state needed for the next batch remains below 20 GB.
No release is exported, published or tagged. The pinned target remains
5.10.274; subsequent ordered batches and GKI/core-vendor separation remain
open.

## Fourth Milestone: 5.10.51 Through 5.10.55

The next bounded official range has 767 entries. Per-release ledgers record
every official/local identity, outcome, path and integration reason:

| Release | Official Commit | Entries | Exact | Code Adaptations | Marker | Present | Inapplicable |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 5.10.51 | `f68261346518c970aea06e6fb3eb031247340400` | 213 | 198 | 5 | 1 | 9 | 0 |
| 5.10.52 | `2cd5fe24a7f025448f19d98c4f4c45ff79ce0784` | 240 | 229 | 1 | 1 | 9 | 0 |
| 5.10.53 | `71046eac2db9aeccf10763d034a1a123911c9a81` | 123 | 111 | 2 | 1 | 9 | 0 |
| 5.10.54 | `08277b9dde633e1447e96b8cb89da2b40f96ae69` | 166 | 151 | 4 | 1 | 8 | 2 |
| 5.10.55 | `11fe69a17195cf58eff523f26f90de50660d0100` | 25 | 23 | 0 | 1 | 1 | 0 |

The totals are 712 exact picks, 12 code adaptations, five release markers,
36 already-present entries and two inapplicable entries. The final marker
is `3619be693`. The separate early prerequisite and whitespace-only cleanup
below end the candidate source at `2f2c2dc22de20eacf8dfe03c9551ee74975a6e3a`.

### Real Context Adaptations

| Area | Local Commit | Integration Decision |
| --- | --- | --- |
| BPF ringbuf ownership | `04a93e38a` | Add the upstream kmemleak annotation while retaining the existing VM_MAP mapping contract |
| io_uring personalities | `756a58b3c` | Move only the io_ring_ctx structure hunk to its existing shared header; retain all original XArray behavioral changes |
| io_uring buffers | `6975df25d` | Make the corresponding shared-header member change and retain the full XArray allocation/traversal/destruction transaction |
| SD voltage switch | `23f2f5b9f` | Use the S18A-only test, preserving the independent retry guard and OEM diagnostic |
| CPU hotplug/cpuset | `17555557d` | Omit only the already-present cpuset include; keep both upstream waits after hotplug unlock and skip frozen tasks |
| Certificate ignore file | `45c3d7696` | Restore the omitted official three-line certs ignore file instead of excluding a source-tree maintenance fix |
| THP/UFFD fork | `77344e0c8` | Update the declaration/definition/sole caller together; retain the existing pte_alloc_one(mm, addr) and maybe_mkwrite(pte, vm_flags) context APIs |
| UDP GSO annotations | `9af38a85a` | Add all four original READ_ONCE/WRITE_ONCE accesses while retaining the independent XENGINE prelude |
| Bonding RCU | `db10f555c` | Preserve the existing no-slave guard and add the complete upstream lock/error/success-unlock scope |
| MMC alias ownership | `1d1412a36` | Adopt alias IDA bypass and paired release; consistently preserve the actual mshc namespace in allocation, reservation and release |
| UFFD selftest allocation | `e16a9284b` | Use anonymous mmap/MAP_FAILED with the existing fatal err helper and balanced braces, not the older print/return flow |
| UFFD range validation | `8596b91ae` | Retain minor-fault registration, remove pointer untagging and migrate all seven callers, including the later CONTINUE path |

The two excluded patches target the absent legacy Hi6421 regulator, whose
MFD dependency is disabled. Selected Allen platform regulators are different
implementations; a matching vendor name does not justify transplanting old
HiKey PMIC code. Existing SDHCI presets, F2FS cache/casefold lifetimes,
ascending memmap initialization, XHCI lock coverage and endpoint helpers
remain rather than being overwritten with older implementations. Both
official swap/shmem reverts are recognized as already applied before #52.

MMC integration is based on the actual retained running DT and all six
firmware DTBs: their aliases are mshc1/mshc2, not mmc. The selected Zodiac
probe/slot/reset path also requires mshc IDs. Core-only namespace migration
can renumber exposed mmc/mmcblk devices; changing every lookup without
changing the DT makes probe fail. Using mshc consistently preserves the
required hardware/userspace contract and corrects the old reservation-stem
mismatch exposed by alias IDA bypass. No dual-stem fallback, hard-coded
index, DT rewrite or removable-media regression claim is added.

### Official Follow-Up And Static Validation

`51a646d37` exactly brings forward Linux 5.10.61 commit
`695ab28a7fa107d0350ab19eba8ec89fac45a95d`, mainline
`a30f895ad3239f45012e860d4f94c1a388b36d14`. The original personality XArray
conversion treats xa_alloc_cyclic return 1 as failure although it means
successful wrapping, freeing an identity still stored in the array. The
official fix cleans up only on negative return and returns the allocated
ID on either success value. It preserves original author/date/source lines
and SHA provenance, sits outside the 767-entry ledger and must be recognized
when the ordered 5.10.61 range is reached. Generic XArray semantics are not
modified. Source: the official Linux stable repository and
<https://kernel.org/pub/linux/kernel/v5.x/ChangeLog-5.10.61>.

`2f2c2dc22` removes five inherited trailing-whitespace findings and a final
blank line exposed by exact stable picks. The original import commits stay
exact; this separate cleanup changes no code semantics. The complete source
range passes git diff --check. Diff-only strict checkpatch for the twelve
code adaptations reports zero errors, two warnings and three checks: the
warnings concern preserved SD diagnostic indentation and creation of the
missing ignore file; the checks are original upstream XArray formatting.
An independent ordered private-index reconstruction, overlaid with the
official early fix and this cleanup, matches every source blob and mode in
the final candidate. This is source validation, not a compilation or boot
pass.

### Selected Interface Gates

The exported prepare_to_wait_exclusive return type changes from void to
bool with its declaration, implementation and rq_qos first-waiter behavior.
V4L2 core operations gain command before ioctl; ext4 superblock state gains
the MMP buffer owner; private timer state gains timers_pending. The THP
fork function gains distinct destination/source VMAs across all three sites.
SCSI mode-sense success changes to zero and failures to negative errno;
independent review confirms every selected SD caller is coordinated. These
are real type/semantic gates, not an unchanged ABI claim. Disabled SAS/SR
paths are not represented as tested; the upstream SAS malformed-header
success-result behavior remains a conditional review item if enabled.

Configured io_ring_ctx layout prediction keeps size 1216, alignment 64,
CQ at 640, locking/completion blocks at 768/896 and OEM buf at 1184 after
both XArray conversions. Their size decrease is absorbed by existing
cacheline padding. Actual hyperframe instructions access buf at 1184, not
the converted IDR fields; no compatibility padding or assembly rewrite is
needed for this exact conversion. Final fresh-C layouts and CFI/export/
module checks are still required. This scoped evidence does not certify
every opaque OEM layout or a stable KMI.

### Final Build And Paired Runtime Acceptance

Source `2f2c2dc22` completes the full Linux 5.10.55 #53 build with Android
Clang/LLD 12.0.7, ThinLTO and enforcing CFI. Effective configuration values
and signing material remain equal to #52. The 13,823-entry export table adds
tcp_mtu_to_mss, removes no exports and changes 58 CRCs, including the new
prepare_to_wait_exclusive return type and V4L2 operation layout. All 390
fresh modem imports and two optional dummy-module imports match; this is
not an unchanged complete ABI claim.

Fresh C object layouts confirm io_ring_ctx size 1216 and OEM buf offset
1184, and io_kiocb size 240 with ctx/buf at 80/232, matching the inspected
hyperframe instructions. V4L2 core operations are 104 bytes with ioctl/
compat_ioctl32 at 56/64 and their selected consumers are rebuilt. ext4 state
is 2176 bytes, with journal at 648 and later cacheline-aligned state unchanged;
the timer boolean occupies existing padding without shifting vectors.
Credentials remain 176 bytes with ucounts/group_info at 144/152. The final
wait-function implementation returns bool, and its canonical jump-table
entry belongs to the new bool CFI type in __cfi_check. The reserved page-table
interval is 4096 bytes and the OEM protection end remains 64-KiB aligned.
These checks cover the affected selected contracts, not every opaque OEM
object or a stable KMI.

Signed matching boot/modem_driver images use `5.10.55-clang12-r53` naming.
Kernel round-trip, preserved firmware stub/DTB/header, CMS certificate and
ext4 ownership/label/readback checks pass. Only boot and modem_driver are
written after live #52 rollback and device/partition identity checks; the
independent Magisk ramdisk and user-data partitions are untouched.

One normal #53 Android boot reaches sys.boot_completed=1, available CE
storage/RUNNING_UNLOCKED and all three checked core services. The new modem
loads automatically as Live, with matching file and runtime build-id; both
partition readbacks and the unchanged ramdisk match. SELinux Enforcing,
taint 4, forced signatures, enforcing CFI and enabled PRMEM remain. Complete
time-zero and subsequent logs show no actual kernel WARN, BUG, Oops, panic,
CFI or module-load fault. The final window contains 545 AVC denial records,
none requesting module loading; they remain visible, not suppressed.
The screen is restored to Asleep and tracing remains disabled with only
the original wifi instance. No broader hardware regression is replayed.

The accepted #53 pair, immediate #52 rollback, latest r49-r53 pairs and
original/r17 recovery files remain local. Accepted build metadata is saved
before output reuse; consumed task-owned logs and superseded r48 images
are removed without touching unrelated diagnostics. No release bundle,
publication or push is made. Next is the ordered 5.10.56-63 batch, with a
separate actual OEM offset gate for the 5.10.60 MSI device-layout change.
The pinned target remains 5.10.274; stable synchronization alone does not
establish Android common parity or GKI.

## Fifth Milestone: 5.10.56 Through 5.10.64

The next ordered range resolves all 614 official entries from v5.10.55
through v5.10.64: 562 exact picks, nineteen code adaptations, nine version
markers, 23 already-present fixes and one absent/unselected driver patch.
The initial 587-entry range ends at b108b2631/5.10.63. A concrete flush
publication dependency of the MAS iterator migration extends the same
installation milestone through the complete 27-entry 5.10.64 range, ending
at `03d07321d`. No 5.10.63 intermediate candidate is installed.

| Release | Official Tip | Entries | Exact | Code Adaptations | Markers | Present | N/A |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 5.10.56 | `9746c25334cb364ab6651ee6dfd4cab3218d0c06` | 69 | 62 | 2 | 1 | 4 | 0 |
| 5.10.57 | `1cd6e30b83d741562b55bf5b7763b1238a91150c` | 31 | 27 | 2 | 1 | 0 | 1 |
| 5.10.58 | `132a8267adabd645476b542b3b132c1b91988fe8` | 135 | 128 | 2 | 1 | 4 | 0 |
| 5.10.59 | `5805e5eec901e830c7741d4916270d0b9cfd6743` | 19 | 16 | 1 | 1 | 1 | 0 |
| 5.10.60 | `2c5bd949b1df3f9fb109107b3d766e2ebabd7238` | 103 | 100 | 1 | 1 | 1 | 0 |
| 5.10.61 | `452ea6a15ed2ac74789b7b3513777cc94ea3b751` | 95 | 88 | 4 | 1 | 2 | 0 |
| 5.10.62 | `f6dd002450bf7b9143aff3af42ad1e12efe9a4f8` | 105 | 94 | 3 | 1 | 7 | 0 |
| 5.10.63 | `e07f317d5a289f06b7eb9025d2ada744cf22c940` | 30 | 26 | 2 | 1 | 1 | 0 |
| 5.10.64 | `cb83afdc0b865d7c8a74d2b2a1f7dd393e1d196d` | 27 | 21 | 2 | 1 | 3 | 0 |

Sources are the same official Linux stable repository, ordered release
ranges and matching ChangeLog-5.10.56 through ChangeLog-5.10.64 files at
kernel.org. Each local identity and outcome is recorded in the nine
`linux-5.10.<sublevel>-imports.tsv` ledgers. The unselected absent driver
is Richtek RTMV20; it is not the active Allen regulator.

### Context Adaptations And Standardization

Conntrack timestamp/GC adaptations preserve the existing cloud-network
teardown hook and resize-mutex context while applying the complete upstream
expiry and bounded full-table cycle. ECN uses the original corrected inner
header offset without reverting newer packet-pull/header-reload code.
SCMI checks probe and delayed-response status inside the newer protocol
lifecycle rather than restoring obsolete handles. Pipe quota and poll_usage
changes preserve neighboring OEM includes and apply the original two-buffer
minimum and epoll wakeup contracts.

`4564a653f` adopts the full upstream sched/rt single PI-priority snapshot
and class-setting refactor from stable `a3e6bd0c71bb5e4821aff9ab8f221bfc08039d73`,
mainline `f558c2b834ec27e75d37b1c860c139e7b7c3a8e4`. It removes the OEM
ARCH_PLATFORM always-MOVE workaround only after the same effective priority
governs dequeue and final class/priority. Existing VIP normalization, the
fair-nice clamp/graded setter, Android RVH, successful-parameter dynamic-prio
clearing and latency/uclamp/re-vote boundaries remain. Independent source
review confirms these contracts. The old configured workaround already
suppresses the upstream double-enqueue scenario; no prior device crash or
new scheduling performance pass is claimed.

The BPF lockdown reason moves into the existing private LSM table, and the
linker sanitizer selectors retain Android TEXT_CFI_JT. Virtio XDP/GRO changes
preserve the has_cvq invariant. The MSM timeout callback retains newer CQE
crypto members. Memcontrol updates its two-output API and only caller, with
the scan_control explanation placed in its existing mm/internal.h definition.
Encrypted-symlink getattr is inserted before existing F2FS dedup helpers.
These context adjustments do not erase required hardware functionality.

The three ordered 5.10.63 credential reverts complete together, removing
the entire temporary per-credential ucounts feature and the local
allocation-before-put repair `94269d6b1`. Ordinary namespace ucounts,
HKIP/checkroot, sched_core_setuid and independent fork_idle changes remain.
No build or installation occurs between the three reverts. Actual configured
C headers restore credential size 168 and group_info offset 144; final object
and module checks remain required.

### MSI Assembly Layout Integration

The exact 5.10.60 MSI lock insertion is retained. Actual configured
raw_spinlock_t is 24 bytes, moving device size 872 to 896, gendisk.queue
1064 to 1088 and Scsi_Host.hostdata 3112 to 3160. Existing OEM assembly
really loads these fields; rebuilding C modules alone cannot repair it.
Nested SCSI-host/UFS tail accesses can move by 72, not a blanket 24 bytes.
Hyperframe also allocates and zeros a real local struct device before
dma_buf_attach, so its stack allocation must grow with the object.

Separate integration `a59c7f232` generates twelve offsetof/sizeof constants
through the existing ARM64 Kbuild asm-offsets flow and changes only 29
proven operands in nine assembly files. It retains opcode forms, CFI and
unrelated instructions/debug metadata. The local hyperframe device frame
is derived from its actual fixed prefix, C object size, canary and alignment,
giving 960 rather than 928 bytes. Independent instruction/type tracing and
actual Clang 12 headers confirm each field equation; ordinary numeric
matches for request/queue/io_uring and other objects are not rewritten.
No ABI compatibility padding, dummy field, runtime indirection or skipped
upstream lock is introduced. This is bounded integration, not certification
of all future opaque OEM layouts.

### MAS Request References And Flush Publication

Stable `26ee94ba343c63d9d23112521c68fa72c82a8805` assumes the queue tag iterator
already owns a request reference before the expiry callback. The selected
OEM three-bank assembly iterator instead reads tags->rqs and calls directly,
without protected get/put. Keeping only the MAS timeout hook while deleting
the callback's extra reference would violate that prerequisite.

Separate integration `d5a3f2b02` routes the existing typed UFS queue wrapper
to the standard C bt_iter, reusing lock-protected find_and_get_req, the
callback and flush-aware blk_mq_put_rq_ref. Only private iterator metadata
gains an explicit tag offset. Normal/high-priority/reserved banks retain
their allocation numbering and scan order, while normal queues retain their
existing two-bank offsets. The standard hardware-context filter and false
return stopping the current bitmap now apply; this is not instruction-for-
instruction equivalence with the opaque loop. Public layouts, exports and
all-tag dispatch remain unchanged.

That migration makes the missing 5.10.64 flush publication fixes directly
relevant. A reused flush request can become visible through an old tag alias
while blk_rq_init is rebuilding it; publishing ref=1 before end_io allows
the standard iterator to acquire an incompletely initialized generation.
Stable `ceffaa61b5bb5296e07cb7f4f494377eb659058f`, mainline
`c2da19ed50554ce52ecbad3655c98371fe58599f`, moves reference publication after
end_io and smp_wmb. Stable `cad6239f5080fdb1acdfb7faeaa8b252125a68d1`, mainline
`a9ed27a764156929efe714033edb3e9023c5f321`, recognizes flush requests by that
initialized callback identity instead of a racy mq_hctx dereference.
Local `7299e3048`/`d9fb9db87` apply these original changes while preserving
MAS routing/sync flags and removing the obsolete NULL guards. The third
flush mapping-clear patch is already present and is classified accordingly.
The unfinished 5.10.63 build is stopped, not accepted or installed; complete
5.10.64 is imported before the next full build. Sources: the official
ChangeLog-5.10.64 and linked original commit messages.

### Compile-Time Interface Integration

The first complete 5.10.64 build identifies two concrete vendor integration
errors. Separate `ae19ca7fe` coordinates the Android/HCK restricted-hook
calls with the two-argument tracepoint_update_call introduced by stable
`7799ad4d181f4694f62e668e1b86bc64f7fac88b`, mainline
`231264d6927f6740af36855a622d0e240be9d94c`. Only the two obsolete false
arguments are removed. These permanent two-slot hooks register but cannot
unregister, so the old no-sync semantics, callback-data guard, mutex,
WRITE_ONCE, static-key enabling and exports remain; the upstream normal
registration/unregistration state transitions are not reverted.

Separate `378a72fba` adds only a file-scope opaque struct termiox declaration
after stable `d24347e2ff1193164224e5c0558e8682e28e91f6`, mainline
`c762a2b846b619c0f92f23e2e8e16f70d20df800`, removes the dead UAPI definitions.
The existing vendor tty/operation pointers are ABI-only, with no initializer
or active operation, and the four termiox ioctls already return ENOTTY.
Forward declaration fixes the real connectivity Werror without restoring
the unused UAPI, shifting existing fields or weakening compiler checks.
Both official imports remain exact in the ledgers; these are separate
integration commits. Kernel source now ends at `378a72fba`.

### Source And Final Build Validation

Independent checks confirm all original ordering, identities and outcomes,
and all exact changed lines/newline/mode transitions and original author/
date/stable trailers. The eight semantic-PRESENT cases are checked against
their historical source context, not inferred solely from patch failure.
The complete source matches an independent ordered reconstruction overlaid
with the assembly/reference integrations, `9e6d738f0` whitespace cleanup
and the two compile-time interface integrations.
Original picks remain exact. Complete source git diff --check passes.

Complete Linux 5.10.64 #54 compilation and final AArch64 ELF linking pass
from `378a72fba` with all 6,431 effective configuration values and signing
materials equal to #53. Fresh C objects confirm the restored cred168/
group_info144 and the selected device, disk, SCSI/UFS, pipe, virtio, XFRM,
IGMP, OTG and signed SPI layouts. Twelve generated constants and all 29
mapped assembly operands match those actual types. Hyperframe's device ends
before its canary, its frame remains 16-byte aligned, and saved-register/CFA
rules are unchanged. This is evidence from current objects and instructions,
not stale embedded OEM debug types.

The changed __do_once_done four-argument interface has its new module
parameter, matching export CRC and final CFI canonical acceptance path.
Both actual modem call sites pass __this_module. Final flush instructions
write the same callback canonical address later compared by is_flush_rq
and blk_mq_put_rq_ref, then execute dmb ishst before publishing the reference;
the CFI jump-table entry branches to the real flush_end_io implementation.
No CFI exception or runtime flush stress result is claimed. Diagnostic
parsers are corrected only for observed zero-operand ret and quoted/hash
ThinLTO local symbols; these are not new kernel changes or device faults.

The full table has 13,826 exports: four additions, one removal and 8,949
changed CRCs versus #53. Additions are fscrypt_symlink_getattr and the three
SRCU polling/state helpers; sk_psock_destroy is removed. All 390 rebuilt
modem imports and two optional dummy imports match. The large CRC change is
retained rather than hidden with old modules or patched version tables.
CMS verification, embedded certificate, boot round-trip/firmware/DTB/header,
signed ext4 filesystem and original label/ownership checks pass.

### Installed Runtime Acceptance And Boundary

One matched boot/modem_driver installation reaches normal Android as
5.10.64 #54. The modem loads automatically as Live, with matching runtime
build-id and complete signed file and partition readbacks. The independent
Magisk ramdisk is unchanged. CE storage is available/RUNNING_UNLOCKED and
SurfaceFlinger, activity and power services are present. Enforcing SELinux,
taint4, signatureY, CFI and enabled PRMEM remain.

Complete time-zero and subsequent logs contain no actual WARN, BUG, Oops,
panic, CFI or module fault. The final window retains 546 AVC denials, none
requesting module loading. The screen returns to Asleep, tracing remains
off with only the original wifi instance, and no broad hardware regression
is repeated. #53/5.10.55 is the immediate paired rollback; recent r50-r54
pairs and the original/first-accepted recovery files remain local, with
metadata saved before build-output reuse and the workspace below 20 GB.

Next is the bounded complete 5.10.65-70 range, including the shared request
getter's stale-tag check and standard const list_sort callback migration.
Later stable releases, Android common parity and GKI remain open; no release
is exported, published or pushed. Normal boot/module acceptance does not
certify calls/data, whole hardware, performance or power.

## Sixth Milestone: 5.10.65 Through 5.10.70

The next complete source batch resolves all 842 official entries from
v5.10.64 through v5.10.70: 798 exact picks, twenty code adaptations, six
release markers, thirteen already-present fixes and five inapplicable
entries. The ordered imports end at `a38b36225`; the separate MAS all-tag
integration and two-line whitespace cleanup freeze the kernel source at
`a08144e8a`. No intermediate release in this batch is installed.

| Release | Official Endpoint | Entries | Exact | Code | Marker | Present | N/A |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 5.10.65 | `c31c2cca229aa5280d108618bb264c713840a4c2` | 237 | 220 | 7 | 1 | 4 | 5 |
| 5.10.66 | `44a32dcb2fb5598dc8b61645852df0ea829aeace` | 5 | 4 | 0 | 1 | 0 | 0 |
| 5.10.67 | `faf816b0f8d0fa8ea24f579fb6a51e5ed3efd750` | 306 | 294 | 7 | 1 | 4 | 0 |
| 5.10.68 | `4d8524048a35c03ff04c25ca1bcdf569f9aeb14a` | 126 | 121 | 3 | 1 | 1 | 0 |
| 5.10.69 | `5f4196eaa90c139ac470111b8e2deabf5f283baa` | 65 | 62 | 1 | 1 | 1 | 0 |
| 5.10.70 | `f93026b28e2afe5060a493b0bbcee19d12961b7e` | 103 | 97 | 2 | 1 | 3 | 0 |

Sources are the original ordered stable commits and official
ChangeLog-5.10.65 through ChangeLog-5.10.70 at
<https://cdn.kernel.org/pub/linux/kernel/v5.x/>. The six corresponding
`linux-5.10.65-imports.tsv` through `linux-5.10.70-imports.tsv` ledgers
record every original/local identity, adaptation and exclusion. All 824
actual import commits preserve their original author, date/timezone, message
and stable trailer. Independent checks match the 798 exact patch streams,
the approved before/after states of twenty adaptations, all six markers,
and both complete-post-hunk and separately reviewed semantic PRESENT cases.
The five exclusions are absent, unselected alternate platform drivers.

### Standard Interfaces And Retained Platform Contracts

The mutex HANDOFF change retains the independently audited per-mutex VIP
acquisition, waiter and cancellation lifecycle. Deadline clock, hrtimer,
RCU and cpuset fixes preserve their selected platform hooks and locking
contracts. The mq-deadline discard-merge change targets the actual relocated
`mq-deadline-main.c`, retaining vendor declarations. ARM64 TLBI uses the
upstream atomic ASID snapshot and barrier while preserving configured
erratum and OEM level/batch arguments. Userfaultfd, ethtool and io_uring
changes retain their existing Android interfaces rather than replacing
them with older implementations.

Stable `996f7c4a1fcf512af3502507f69e4c4760d5f9c8` moves generic PHY init/exit
to the correct DWC3 core lifecycle while retaining the complete selected
failure ladder. The inspected device tree assigns reset-only generic PHYs
to the child and the real platform PHY to its parent. This source review
does not certify all OEM repeated-core-init paths or new USB stress results.

Stable `55e6f8b3c0f5cc600df12ddd0371d2703b910fd7` applies the coordinated
standard const list_sort interface and callbacks across 41 files. It removes
the earlier private nonconst typedef/cast adaptation instead of adding a
new CFI exception. Independent source review finds 43 direct calls and
forty const comparators, with no additional opaque/OEM consumer in the
examined tree. The inherited nonnull attribute comma-spacing and continuation
checkpatch messages also occur in the original stable patch; they do not
indicate a type or CFI error. Final enforcing CFI and actual callback
membership remain build gates, not conclusions from a frontend probe alone.

The original public-key digest-size fix and flow-block callback-list change
are retained. They change public structures and related export CRCs; all
required modules must follow this build. The selected fresh-header layouts
and unchanged OEM assembly inputs are reviewed before compiling, with actual
new C object layouts and twelve generated values still required afterward.

### MAS All-Tag Standard Iteration

Exact stable `cc3dd119d3cf3435878b24a4a0ed21be6950573c`, mainline
`67f3b2f822b7e71cfc9b42dbd9f3144fa2933e0b`, rejects stale request aliases
by checking rq->tag against the bitmap index in the standard protected
getter. The selected OEM all-tag hook bypasses that getter, drops
STATIC_RQS/STARTED flags and visits raw request pointers without a reference.
The actual SCSI-core busy count and error-handler wait/wakeup paths use this
interface; it is not merely an unused assembly routine.

Separate `4582d5ea9` replaces only that selected all-tag traversal with the
standard C bt_tags_iter/getter/flush-aware put contract. Private iterator
metadata gains an explicit offset. Normal, high-priority and reserved banks
retain their allocation numbering and scan order. STATIC_RQS visits static
requests without get/put; dynamic STARTED visits acquire a reference, filter
the request state and release the reference even when filtered or when the
callback returns false. False stops the current bitmap, not the whole
tagset. The native two-bank path, public layouts and symbols remain intact.

The non-null OEM hook remains the selected-branch marker, but traversal no
longer calls the opaque hook. Only the current UFS provider is established;
equivalence for arbitrary future vendor providers is not claimed. The
constructor lock, three-bank offsets and teardown are reviewed, while rqs
publication still includes plain writes. Reusing the established getter and
drain contract is not a claim that all publication races are solved.
Selected HUFS reset/TMF completion routines use hardware bitmaps, so this
change's demonstrated caller scope is SCSI-core counting/EH, not completed
live reset-race stress.

Independent whole-kernel reconstruction, overlaid with the separately
reviewed all-tag integration and whitespace-only `a08144e8a`, exactly
matches the frozen source. Original official picks remain unchanged;
source diff --check passes. Full compilation, final IR/ELF and matched
installation are the next acceptance gates; the installed accepted device
at this source-only boundary remains 5.10.64 #54.

### Final Build And Installed Acceptance

Complete Linux 5.10.70 #55 compilation and final AArch64 linking pass from
`a08144e8a` without a kernel integration compile failure. All existing
6,431 configuration selections and signing materials match #54. The sole
newly visible configuration entry, COMMON_CLK_XLNX_CLKWZRD, remains disabled:
exact stable `f577e9f58ff079a61af08fae75b76cb8cc0c3057` corrects its existing
Kconfig dependency from IOMEM to HAS_IOMEM. No selected feature or security
setting changes.

Fresh actual C objects confirm every inspected direct field size/offset in
cred, device, gendisk, SCSI-host and UFS records, including cred168/
group_info144 with no per-credential ucounts. The public-key signature and
flow-block-offload structures are each 80 bytes, with the intended unsigned
32-bit digest size and new callback-list tail. Current twelve generated
offsets, unchanged nine mapped assembly sources and relevant header/config
continuity reuse #54's scoped 29-operand proof; a second complete 29-object
instruction audit is not claimed or needed for unchanged operands.

Actual list_sort and five selected caller objects use the coordinated const
types. The export CRC changes from cba4abe3 to e091c977. Final __cfi_check
accepts exactly the sorter's canonical address and the five aligned
comparator entries for their new types; all six canonical jumps target their
implementations, and every sorter indirect call uses the same enforcing
callback type. No nonconst cast or CFI bypass remains.

The five actual all-tag executable control-flow graphs match the approved
standard traversal. Final instructions confirm static/dynamic separation,
three-bank and native-two-bank offsets/flags, lock-protected tag/ref checks,
unlock before callback, STARTED filtering and reference puts for filtered
or false callbacks. The selected dispatch no longer calls the opaque
all-tag hook. Bitmap/request callback CFI, three SCSI-core caller identities
and the existing flush callback's store/barrier/reference publication and
canonical put identity match the final ELF. This is bounded implementation
evidence, not live cancellation, flush/reset stress or arbitrary vendor-hook
equivalence.

The complete table has 13,827 exports: pci_pio_to_address is added, no export
is removed, and fifteen CRCs change versus #54. All 390 rebuilt modem imports
and two optional dummy-module imports match. Boot kernel round-trip, firmware
stub, original DTB/header preservation, signing certificate/CMS, signed ext4
filesystem and original module label/ownership readback checks pass.

One matched boot/modem_driver installation reaches normal Android as
5.10.70 #55. Automatic Live modem loading has the new matching runtime
build-id, complete signed-file and partition readbacks. Independent Magisk
ramdisk, Enforcing SELinux, taint4, forced signatures, CFI and enabled PRMEM
remain. CE storage is available/RUNNING_UNLOCKED; SurfaceFlinger, activity
and power services are present. Complete time-zero and subsequent logs have
no actual WARN, BUG, Oops, panic, CFI or module fault. The final window retains
501 AVC denials, none requesting module loading. Tracing remains off with
only the original wifi instance, and the screen returns to Asleep. No broad
hardware regression is repeated.

The immediate paired rollback is #54/5.10.64. Current metadata is saved
before output reuse; recent r51-r55 pairs, originals and the first accepted
r17 recovery pair remain local and below 20 GB. The next bounded range is
5.10.71-75, with explicit FIB/MDIO interface gates. Later releases toward
the pinned 5.10.274 target, Android common parity and GKI remain open. No
release export, publication or push occurs.

## Seventh Milestone: 5.10.71 Through 5.10.75

The next complete range resolves 330 official entries: 309 exact picks,
five code adaptations, five release markers and eleven already-present
fixes, with no inapplicable entry. Source ends at `99ff6138b`; no intermediate
71-74 candidate is installed. The five per-release ledgers retain every
original/local identity and decision.

| Release | Official Endpoint | Entries | Exact | Code | Marker | Present |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| 5.10.71 | `5cd40b137cba45a5a3d0b9a8554f779a3e0e93b4` | 93 | 84 | 3 | 1 | 5 |
| 5.10.72 | `5aa003b38148d584f20455ecac85c51187d0b71e` | 30 | 29 | 0 | 1 | 0 |
| 5.10.73 | `0268aa579b1f741b12300bc7f084ffe990cfde5f` | 82 | 76 | 1 | 1 | 4 |
| 5.10.74 | `77434fe5a077f05f0851931d6a71d1098b6004f8` | 23 | 22 | 0 | 1 | 0 |
| 5.10.75 | `3a9842b42e421f6496a78a42a123639cf6d7ed31` | 102 | 98 | 1 | 1 | 2 |

Sources are the original stable range and official ChangeLog-5.10.71 through
ChangeLog-5.10.75 at <https://cdn.kernel.org/pub/linux/kernel/v5.x/>.
Independent actual-import checks cover all 319 original authors/dates,
messages and stable trailers, 309 exact picks with 374 file sections, five
approved code pre/post states, five SUBLEVEL-only markers and all 330 ledger
rows. Nine historical PRESENT cases match nineteen complete post-hunks;
two semantic PRESENT cases retain independently verified complete function
implementations. The whole actual kernel subtree exactly matches the
independently ordered private reconstruction, including every file mode.
Source diff --check and strict checkpatch for the five code adaptations pass.

### Schedutil Lifetime And Other Adaptations

Stable `a7d4fc84404d45d72f4490417e8cc3efa4af93f1` moves tunable freeing to
the standard kobject release callback. The selected OEM SCHEDUTIL_OPT exit
also saves settings after gov_attr_set_put drops the last policy reference.
Mechanical free-to-release conversion would let that save access freed
memory. `504bc8279` takes a temporary kobject reference around the existing
put/last-save/global-clear sequence, then releases it, without changing the
last-policy decision or cache contents. Exact following stable
`3945c481360caee7d496c95b5c73afe923a1c7cd` destroys the attr-set mutex before
the final kobject_put. Both changes complete in order before compilation;
no intermediate installation or live sysfs lifetime stress is claimed.

The UFS TM UPIU trace tag correction retains the intervening Android hook.
Its native trace uses the local tag addressing utmrdl_base_addr; the selected
HUFS implementation uses a separate TMF bitmap path. The ext4 htree iteration
fix retains all original progress/error returns in the relocated directory
context. Overlayfs rejects unsupported direct I/O before credential override,
preserving Android credential restoration, balanced fd/inode cleanup and
existing asynchronous ownership. Device links reject consumer==supplier
at the original boundary without losing existing extended flag validation.
These changes do not delete required hardware behavior.

The conntrack resize/cleanup mutex implementation is already present, with
complete functions matching the original post-state except the unchanged
cloud-network hook. The xHCI command abort function retains the official
superseding `09f736aa95476631227d2dc0e6b9aeee1ad7ed58` implementation. Its
full 64-bit CRCR update must not be downgraded to the earlier low-half-only
write. These classifications are based on complete source states, not
inferred from an unsuccessful forward patch or a live abort test.

### Selected Interface Gates

Exact stable `8de12ad9162ca2648c82d631c47c2f7de565aa73` changes
fib_add_nexthop to five arguments so the RTA_FLOW attribute contributes to
the rtnexthop length. Selected IPv4, IPv6 and nexthop callers must coordinate
with its actual new function type and export CRC. Exact stable
`a41938d07201d0e7793f5aade5574a49f32ae82f` appends mdio_driver.shutdown and
wires the standard device shutdown dispatch. Both interfaces require
fresh target C objects and final enforcing CFI checks, not old-module reuse.

The nine public-header changes are reviewed against selected configuration
and opaque assembly dependencies. Actual frozen source measurements confirm
sixteen records and six CFI declarations: fifteen records keep all direct
field offsets/widths, while mdio_driver grows from 168 to 176 with its sole
new pointer at 168. Mapped nine assembly sources and relevant selected
declarations remain continuous; no new sock padding or MDIO/FIB assembly
operand is found. Existing const list_sort and MAS all-tag implementation
evidence is reused only where source, types and configuration agree.

### Build And Installed Acceptance

The full build completes as 5.10.75 #56 with all 6,432 effective configuration
entries and signing material unchanged from #55. Fresh selected C objects
confirm the existing credential and other reviewed layouts. Actual MDIO
objects confirm the 176-byte driver, shutdown pointer at 168, registration
assignment and enforcing indirect-call CFI. Actual FIB objects use the
standard five-argument definition, including all three selected IPv6 calls.
Final named function/callback canonical entries and enforcing machine-code
checks pass. The three real changed CRCs are fib_add_nexthop and the two
MDIO driver registration exports; all 13,827 exports remain, without an
addition or removal. All 390 rebuilt modem imports and two optional dummy
imports match. No old module, spoofed CRC or permissive CFI path is used.

The nine mapped assembly sources, relevant declarations and twelve generated
values retain the accepted offset contract. The accepted #55 const sorter
and MAS all-tag implementation proofs are reused only after source, selected
configuration, current object types and final canonical-address checks.
Changed all-tag objects retain the approved executable control-flow and
callback identities. These bounded continuity checks do not repeat the old
29-operand audit or claim a new full implementation/runtime test.

The repacked boot and matching CMS-signed modem filesystem pass kernel
round-trip, firmware stub/DTB/header preservation, signature verification,
filesystem and original label/ownership readbacks. One paired installation
reaches normal Android as 5.10.75 #56. Automatic Live modem loading has the
matching new runtime build-id and complete signed-file/partition readbacks.
Independent Magisk ramdisk, Enforcing SELinux, taint4, forced signatures,
CFI and enabled PRMEM remain. CE storage is available/RUNNING_UNLOCKED and
the three core services are present. Complete time-zero and subsequent logs
contain no actual WARN, BUG, Oops, panic, CFI or module fault; 517 AVC denials
remain visible, none requesting module loading. Tracing remains off with
only the original wifi instance, and the screen returns to Asleep. No broad
hardware regression is repeated.

The immediate paired rollback is #55/5.10.70. Metadata is saved before output
reuse; recent r52-r56 pairs, originals and the first accepted r17 recovery
pair remain local and below 20 GB. The next bounded range is 5.10.76-80,
with selected header/layout and callback changes audited before compilation.
Later releases toward the pinned 5.10.274 target, Android common parity and
GKI remain open. No release export, publication or push occurs.

## Eighth Milestone: 5.10.76 Through 5.10.80

The next complete range resolves all 779 official entries: 732 exact picks,
nineteen code adaptations, five release markers, twenty-two already-present
fixes and one inapplicable entry. The 756 source commits end at `0253fdc1d`.
No intermediate 76-79 kernel is compiled or installed. Five per-release
ledgers retain every original/local identity and disposition.

| Release | Official Endpoint | Entries | Exact | Code | Marker | Present | N/A |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 5.10.76 | `378e85d1aeb5c93db43f2089630ec40c7385bd54` | 95 | 86 | 3 | 1 | 5 | 0 |
| 5.10.77 | `09df347cfd189774130f8ae8267324b97aaf868e` | 78 | 67 | 2 | 1 | 8 | 0 |
| 5.10.78 | `5040520482a594e92d4f69141229a6dd26173511` | 15 | 11 | 1 | 1 | 2 | 0 |
| 5.10.79 | `bd816c278316f20a5575debc64dde4422229a880` | 20 | 19 | 0 | 1 | 0 | 0 |
| 5.10.80 | `f884bb85b8d877d4e0c670403754813a7901705b` | 571 | 549 | 13 | 1 | 7 | 1 |

Sources are the original stable range and official ChangeLog-5.10.76 through
ChangeLog-5.10.80 at <https://cdn.kernel.org/pub/linux/kernel/v5.x/>. Actual
import verification checks all 756 original authors/dates, complete messages
and stable trailers, 732 exact picks with 932 file sections, nineteen code
pre/post contracts and five SUBLEVEL-only markers. Fifteen historical
PRESENT cases match forty-four complete post-hunks; seven semantic PRESENT
cases have complete independent source contracts. The accepted 208-entry
76-79 prefix is reused by identity while the 571-entry tail is checked once.
All five ledgers and every kernel file mode/blob at the `0253fdc1d` import
checkpoint match the independently ordered, revised reconstruction. Source
diff --check passes. The nineteen
adaptation diffs have no checkpatch errors or warnings; the ESR macro-name
and task-header extern checks are inherited from the original stable patches.

### Real Context And Ownership Adaptations

The xHCI DCS quirk retains the already-newer cancellation/dequeue traversal
and full 64-bit CRCR abort repair while applying the original VIA selector,
quirk bit and complete hardware-cycle correction. Both SLUB fixes preserve
OEM sanitization, KFENCE/KASAN and bulk memcg accounting: reconstructed count
and freelist depth move together, without changing public alloc/free types.
CQHCI's HALT correction follows the Android crypto split into cqhci-core.c;
the controller remains unselected, so this is not hardware acceptance.
SDHCI's voltage mapping retains the 1.8 V override and does not advertise a
new supply range. AMBA removes only the obsolete IRQ warnings.

Binder uses `binder_get_cred(proc)->euid` rather than the mutable task euid.
The existing Android KMI-preserving extension already owns the open-time
credential reference and releases it at final free. It is retained instead
of adding a field to binder_proc. The following credential/LSM patches are
already complete; no invented callback migration or extra credential field
is needed.

The pinctrl failed-enable fix cannot free a two-stage registration handle
inside pinctrl_enable. It keeps the corrected non-freeing enable ownership
from upstream `5038a66dad0199de60e5671603ea6623eb9e5c79`, and performs descriptor,
mutex and object cleanup at the legacy pinctrl_register failure boundary,
matching the uninitialization semantics of
`ae1cf4759972c5fe665ee4c5e0c29de66fe3cf4a`. These later upstream references
prevent double-free or dangling devres/V500 handles; no public type changes.
CTI pairs the runtime-PM put with the existing parent-device get. The ARM64
ESR extraction keeps the stub-HVC dispatch and stack-restoration order.

Block completes the standard resource-budget/restart logic, moves the
32-request plug limit and factor-two multi-queue threshold to the private
header, and removes the inaccurate requeue BUG_ON in both MAS branches.
Existing MAS reference, end/flush publication and sorter contracts remain.
The regulator patch updates the complete S5M8767 binding documentation, but
its driver hunk has no implementation to repair. DW MMC's reported defective
wait-DATA branch is absent from the complete OEM state machine; it is N/A,
not a falsely claimed existing fix or a new controller workaround.

The full UFS keep-link-active refactor, including the phase guard, is already
present and does not change the selected storage layout. The ZRAM fix changes
only the original read-count comparison across an OEM debug argument. Its
unselected memory-tracking branch has a separate pre-existing missing comma;
that unrelated defect is not silently bundled. POSIX CPU timers gain the
complete helper, initialization reuse, fork reset and configured-off stub
while retaining HHEE placement. The task-work option remains disabled, so
this patch does not add a selected task_struct field.

### Fork Group Lifetime And Standard TCP Counters

Stable `c85c6fadbef0a3eab41540ea628fa8fe8928c820` moves group/CPU/class-fork
initialization to the two-argument sched_post_fork. The group comes from the
css_set pinned between cgroup_can_fork and cgroup_post_fork, rather than a
possibly freed group pointer copied from the parent. The complete upstream
function and existing early dynamic-priority/RTG resets are retained.

The first private adaptation kept OEM inheritance before the new group
initialization. Independent transitive review found a real selected path:
QoS inheritance emits sched_qos, whose enabled trace assignment calls
get_task_min_util -> uclamp_eff_value -> uclamp_tg_restrict. With
UCLAMP_TASK_GROUP enabled and the child's effective clamp inactive, this
reads the stale copied sched_task_group. The revised adaptation moves the
single sched_post_fork call immediately after proc_fork_connector and before
the unchanged QoS/VIP blocks. Their mutual order and notifications remain;
all run before cgroup_post_fork releases the pin. The class-fork and clamp
initialization do not require the later QoS/VIP state. Trace CPU/min-util
values now describe initialized state; their numeric values are not promised
to remain identical. The four early Android hook positions and interfaces
are unchanged, and ANDROID_VENDOR_HOOKS is disabled. This closes the concrete
selected trace hazard, not a blanket proof about arbitrary opaque probes.

Stable `a342cb4772f44915b1e6688b101a41c01f9e71aa` completes the standard
eleven-file TCP/DCCP migration from percpu_counter to bare per-CPU unsigned
counters. Declarations, exports, every producer/consumer, DCCP lifecycle,
full possible-CPU summation and the 100 ms deferrable TCP cache timer move
together. The proto orphan_count pointee type changes even though its field
offset does not; this requires actual C types, final CFI and rebuilt modules,
not only a matching physical struct size.

### Selected Source And C-Layout Gates

The thirty-one original header patches cover thirty-four include paths;
twenty-nine include files and two ARM64 headers actually change. Complete
source/configuration binding and one fresh C compilation cover twenty-two
selected records, the new fork/qdisc/timer interfaces and existing const
credential, MDIO/FIB and sorter contracts. All 6,432 emitted configuration
entries remain unchanged. The new hidden ARCH_HAS_CC_PLATFORM symbol is not
selected or emitted on ARM64; no synthetic disabled entry is inserted.

Actual selected layouts measure BPF array aux at 144 bytes: its new owner
contains a 24-byte debug spinlock and occupies 32 bytes. Console grows to
104 bytes with the speed fields at 80/84; Qdisc_ops grows to 176 bytes with
the new callback at 112. Proto remains 416 bytes with its changed unsigned
per-CPU pointer at 320. The 32-byte skb overlay and 20-byte stream message at
offset 20 fit the existing 48-byte control buffer. Binder's 688-byte public
record and 696-byte credential extension remain unchanged. Five selected
fundamental named-field layouts retain the accepted baseline; current
anonymous credential-union evidence is also kept for actual/final matching.

Nine mapped assembly sources remain identical. Existing offset, const-sort
and MAS all-tag implementation evidence is reused only through the scoped
source/configuration/type contracts and still requires final object/ELF
continuity. These checks are not a blanket opaque OEM ABI certificate.

### Build And Installed Acceptance

The first full build finds a real namespace collision from the exact
proc/uptime idle-accounting fix: its public
get_idle_time(struct kernel_cpustat *, int) declaration conflicts with the
selected OEM RCC and PERF translation units' private static helpers.
Separate integration `02e9deb3a` prefixes both conditional definitions and
their direct callers by subsystem. Their bodies and CPU accounting remain
unchanged, as do the upstream public declaration and callers; PERF's missing
final newline is also added. No export, public type, feature selection or
protection is weakened. The existing C-layout measurement is reused only
after explicit two-file namespace-delta and input-identity binding.

The repaired full build produces 5.10.80 #57. Five final ABI/source-context/
CFI reports bind the same source, final ELF and 31 distinct actual objects.
The actual final records cover seventeen distinct selected names; the earlier
twenty-two-record frontend has a broader source scope, not twenty-two final
object layout certificates. BPF owner, console, Qdisc, stream-buffer and TCP
orphan types match their actual consumers. The changed callback assignments,
canonical targets and enforcing indirect dispatches pass. Timer validation
uses actual BTI/branch decoding and the real call_timer_fn range/type/slow-path
checks; it does not assume every allowed canonical entry has a .cfi_jt name.
Accepted const-sort, all-tag and assembly implementation proofs are reused
only through their explicit current source/configuration/object contracts.

All 13,827 exports remain, with no addition or removal and 2,051 real CRC
changes versus #56. All 390 rebuilt modem imports and two optional dummy
imports match. Existing signing material is unchanged, and the stripped,
CMS-verified modem is staged on the original labelled ext4 partition.
Original boot header, firmware stub and DTB roundtrip checks pass. These
are same-build integration gates, not an independently stable KMI.

One paired installation reaches normal Android with automatic matching Live
modem loading. Full boot/modem partition and signed module file readbacks,
runtime module build-id, encrypted user storage/RUNNING_UNLOCKED and all three
core services pass. The independent ramdisk remains unchanged. SELinux
Enforcing, taint 4, enforcing CFI, forced signatures and PRMEM remain enabled.
Complete time-zero/subsequent logs contain no actual kernel or module fault;
517 AVC denials remain visible and none requests module loading. Existing
tracing state and the Asleep screen are restored without a broad hardware
replay. No second build or boot is run after this scoped pass.

The #56/5.10.75 pair is the immediate rollback. Accepted #57 metadata and
the latest #53-57 pairs remain local below 20 GB; obsolete #52 test images
are retired only after matching their retained build contract. Originals,
the first accepted bring-up pair and durable source history remain intact.
Next is the complete bounded 5.10.81-85 range, including a fresh selected UFS
layout/generated-offset/assembly gate. Later releases toward the pinned
5.10.274 target, Android common parity, GKI and full hardware/power acceptance
remain open. No release export, publication or push occurs.

## Ninth Milestone: 5.10.81 Through 5.10.85

The complete next range resolves all 557 official entries: 519 exact picks,
sixteen code adaptations, five release markers, sixteen already-present
fixes and one inapplicable entry. The 540 original-author source commits end
at `392b1a551`; a separate two-file UFS integration ends the kernel source at
`0d678d58e`. No intermediate 81-84 kernel is built or installed. Five
per-release ledgers retain every original/local identity and disposition.

| Release | Official Endpoint | Entries | Exact | Code | Marker | Present | N/A |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 5.10.81 | `99957dcea4e9702cc9060f576233ac1ac84c2a39` | 22 | 18 | 2 | 1 | 1 | 0 |
| 5.10.82 | `d5259a9ba6993a843278203323902bc0c049097e` | 154 | 143 | 6 | 1 | 3 | 1 |
| 5.10.83 | `a324ad794566ae2ad2d5aa7f7e3c1825fb5cd5b7` | 122 | 115 | 5 | 1 | 1 | 0 |
| 5.10.84 | `a0582e24d371d62a749a14e4187af304bd64ef42` | 126 | 117 | 3 | 1 | 5 | 0 |
| 5.10.85 | `e4f2aee6612e56c2a9a5da6131ccd80e57d5075b` | 133 | 126 | 0 | 1 | 6 | 0 |

Sources are the original stable range and official ChangeLog-5.10.81 through
ChangeLog-5.10.85 at <https://cdn.kernel.org/pub/linux/kernel/v5.x/>. Actual
independent import verification checks all 540 original authors, dates,
complete messages and stable trailers; 519 exact picks cover 702 complete
file sections. Existing executable-mode differences in 28 file sections
across 27 commits are preserved rather than silently normalized. Sixteen
code adapters, five SUBLEVEL-only markers, ten historical PRESENT cases with
thirty complete post-hunks, six semantic PRESENT contracts and the sole N/A entry are
independently bound to actual historical pre/post blobs and modes. All 557
ordered source inventories, five TSV ledgers and the final kernel tree
match the approved reconstruction. The UFS integration is not counted as an
upstream pick.

### Standard Lifecycle And Context Adaptations

UFS retains the complete upstream task-management request pointer index,
publication, completion, retirement and managed allocation. The existing
host-lock wrapper and selected HUFS waitqueue/runtime-PM behavior remain;
the already-absent deleted helpers are not reintroduced. SDHCI retains the
upstream ADMA splitting/default/quirk changes and the real `int max_adma`
member. Zodiac's constants, callback interface and selected 4 KiB page size
remain unchanged; this is not a 64 KiB hardware test.

EROFS keeps the original safe attach/enqueue pagevec reuse contract and both
caller booleans while preserving OEM short-lived page tagging/allocation.
The block-size validation helper is complete; redundant blkcg issue
initialization is removed only after throttle ownership transfer, retaining
OEM foreground tags and success accounting. The IOPRIO ADMIN-before-NICE
patch is N/A because the complete existing RT boundary grants only ADMIN;
an order-only patch must not introduce a missing NICE-only permission.

DWC3 moves NoStream retirement to ENDTRANSFER completion and adds the
original early descriptor guards without removing its post-callback recheck
or OEM PM/error paths. Binder applies the original sender-euid partial
revert across the existing credential wrapper while retaining credential
references and security checks. Hotplug receives the complete upstream
SCS/KASAN reset operations. TCPM preserves OEM attach/debounce metadata
while removing the upstream sink no-VBUS reset.

Overlay uses the original pipe-safe splice implementation and existing
credential wrapper, including the selected `override_creds=false` default;
it does not silently force a credentials switch. The existing FD
post-reference identity/retry/RCU contract remains intact. All five
POLLFREE/wait/Binder/signalfd/AIO entries are already present, including the
complete keep-queued AIO lifecycle and later POLLFREE protection. USB
endpoint-zero limits retain the stronger existing OUT-direction guard.
Thermal NULL-ops protection and SELinux's empty-on-OOM hashtab handling
remain, including the protected PRMEM allocator.

Standard F2FS remains unselected while `HW_F2FS_FS=y`; the WHINT parser
adaptation is source-only. The changed shared F2FS lookup trace header is
checked in the actual selected OEM consumers separately. Compat VDSO flag
handling preserves the real compiler/target options and does not invoke a
clean command. SLOB's NOLEAKTRACE flag fix retains adjacent OEM cache masks.

### Compiler-Generated UFS Assembly Integration

The new `ufs_hba.tmf_rqs` pointer moves the record's later fields by eight
bytes. A bounded pointer-flow audit of the two annotated UFS assembly units
finds two additional stale literals in `ufshcd_query_hi1861_bkops`: a state
load and the nested `dev_cmd.lock` mutex address. Accepted #57 C-object
evidence measures state at 17,648 and `dev_cmd` at 20,312, with its mutex at
relative offset eight, hence absolute 20,320. These are measured fields,
not an inferred lock at the start of `dev_cmd`.

Separate integration `0d678d58e` adds `OEM_UFS_STATE` and
`OEM_UFS_DEV_CMD_LOCK` through `offsetof` in ARM64 asm-offsets and replaces
only the two corresponding assembly operands. The complete 104-instruction
function retains its registers, control flow and lock order. No structure
padding, old offset freeze, callback cast or permissive CFI is introduced.
This extends the selected mapped-operand audit, not a blanket certificate
for every opaque OEM assembly ABI.

### Selected Source And Real C Layouts

One actual Clang compilation measures eight selected records under the
accepted effective configuration. UFS grows by exactly one pointer to
23,504 bytes; all old named fields retain their width and their expected
position before/after the insertion. The measured state and nested command
mutex now occupy 17,656 and 20,328. All fourteen generated OEM constants
agree: eight earlier UFS values move by eight, the two new fields are
generated, and the four unrelated existing values remain unchanged.

The real SDHCI host is 1,472 bytes with the new 32-bit `max_adma` field and
its existing five-argument ADMA callback. TCP zerocopy UAPI grows to forty
bytes while preserving all five legacy field positions. BPF `writecnt` is
the real 64-bit atomic type; the IPv6 stub's new `fib6_nh_release_dsts` slot
and `inode_update_time` function type are measured. BPF JIT/dispatcher
remains unselected, without a fake external declaration or selected-CFI
claim.

Effective feature selections, signing material and enforcing protections
are unchanged. Emitted configuration entries decrease from 6,432 to 6,431
solely because upstream 5.10.81 excludes Clang from FORTIFY and the already
disabled `CONFIG_FORTIFY_SOURCE` line disappears. It remains effectively
disabled; byte-identical emitted configuration is not claimed. Fresh final
objects/ELF, matched-module and installed acceptance are separate gates.

### Full Build And Installed Acceptance

The complete build succeeds as 5.10.85 #58 without another source repair.
The inherited OEM boot-time exit/init section-mismatch notice and ordinary
compiler/linker warnings remain visible; a warning-free build is not
claimed. Five final reports bind the same integrated source, configuration,
ELF and 29 distinct actual objects. Thirteen distinct selected physical
record names are checked; this is not an inventory of every OEM layout.

All fourteen generated offsets match the single frontend. Thirteen affected
HUFS machine operands are freshly checked; eighteen unrelated operands
reuse the earlier proof only through explicit source/value/configuration
continuity. Selected generic/HUFS UFS consumers match the new pointer
layout. SDHCI's actual host packing and five-argument ADMA dispatch agree.
IPv6's real new stub initializer, typed consumer slot, final function pointer
and enforcing CFI match the canonical target. The new inode export and
existing time-update indirect type pass. BPF atomic reads are checked as
actual ARM64 acquire assembly and direct-helper dataflow, not an assumed
LLVM load or new callback. Shared F2FS lookup names are copied into dynamic
trace records in the selected OEM consumers.

Current const-sort and MAS all-tag proofs retain explicit implementation,
object and final-address continuity. Changed objects are decoded once;
shared hosts/blk-mq IR is reused by the two readers. The all-tag gate checks
fresh final bitmap/request/endio canonical memberships and flush
assignment/getter/put identity, without replaying old implementation tests.
These bounded gates do not certify TMF, 64 KiB ADMA, network stress or every
opaque OEM contract.

The export table has 13,829 symbols: four additions, two removals and 2,575
changed CRCs versus #57. Additions are the two unaligned siphash helpers,
`dst_cache_reset_now` and `inode_update_time`; the aligned siphash helpers
are removed. All 390 rebuilt modem imports and two optional dummy imports
match. CRC churn is retained, not concealed by old-binary resigning or
version-table patching. The stripped modem's CMS signature, embedded kernel
certificate, original ext4 ownership/label and file readback pass. Original
boot header, firmware stub and DTB roundtrip checks pass.

One paired installation reaches normal Android with automatic matching Live
modem loading. Full boot/modem partition, signed module and unchanged
independent ramdisk readbacks pass, as do the loaded module build-id, user 0
CE storage/RUNNING_UNLOCKED and three core services. SELinux Enforcing,
taint 4, enforcing CFI, forced signatures and PRMEM remain. Complete
time-zero/subsequent logs contain no actual kernel or module fault. All 501
AVC denials remain visible, none for module loading. Original tracing state
and the Asleep screen are restored; no second boot or broad hardware replay
is run after the scoped pass.

The #57/5.10.80 pair is the immediate rollback. Accepted #58 metadata and
the latest #54-58 pairs remain local below 20 GB. Obsolete #53 test images
are retired only after matching their retained build contract; original
images, the first accepted bring-up pair and source history remain intact.
Next is the complete 5.10.86-90 range. Its selected MMC shutdown callback,
BPF default-control interface and actual virtio-header consumers require
fresh bounded checks, while unchanged layout/assembly contracts retain
scoped continuity. The existing selected AMA allocation path remains a
separate mempolicy integration issue; a standard non-AMA stable fix does
not certify it. The pinned 5.10.274 target, Android common parity, GKI and
full hardware/power acceptance remain open. No release export, publication
or push occurs.

## Tenth Milestone: 5.10.86 Through 5.10.90

The complete next range resolves all 259 official entries: 229 exact picks,
seven code adaptations, five release markers and eighteen already-present
fixes. The 241 original-author source commits end at `74057806b`, without a
separate integration commit. The accepted two-file UFS offset repair remains
intact. No intermediate 86-89 kernel is built or installed. Five per-release
ledgers retain every original/local identity and disposition.

| Release | Official Endpoint | Entries | Exact | Code | Marker | Present |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| 5.10.86 | `37050f17f2d243f0d6ebe63ed3df84f91abce614` | 2 | 1 | 0 | 1 | 0 |
| 5.10.87 | `272aedd4a3059e6e3b0c4c0f9a8b09e5a9fa2c21` | 34 | 30 | 0 | 1 | 3 |
| 5.10.88 | `856f88f27bbc4d3b4b88ce6fe23964ffe60ea649` | 99 | 90 | 1 | 1 | 7 |
| 5.10.89 | `eb967e323f7fb073c51401070f7d2cb381a003f7` | 76 | 67 | 5 | 1 | 3 |
| 5.10.90 | `d3e491a20d152e5fba6c02a38916d63f982d98a5` | 48 | 41 | 1 | 1 | 5 |

Sources are the original stable range and official ChangeLog-5.10.86 through
ChangeLog-5.10.90 at <https://cdn.kernel.org/pub/linux/kernel/v5.x/>. Actual
independent import verification checks all 241 original authors, dates,
complete messages and stable trailers; 229 exact picks cover 276 complete
file sections. Two existing executable-mode variants are preserved, and
the original selftest permission change is applied exactly. Seven code
adapters, five SUBLEVEL-only markers, fourteen historical PRESENT cases with
23 complete post-hunks and four semantic PRESENT contracts are bound to
actual historical blobs/modes. All 259 ordered source inventories, five
ledgers and the final kernel tree match the approved reconstruction. The
accepted #58 documentation root is rebound explicitly without replaying the
source review or replacing the actual tree with a private snapshot.

### Standard Callbacks And Preserved Platform Contracts

MMC receives the complete upstream `shutdown_pre` callback and internal
host-stop helper, separating card-detect cancellation from the existing
ordinary remove path. Zodiac's GPIO IRQ bypass and deferred-resume guards
remain, as do selected MMC interfaces and records. This change requires a
fresh actual class callback/CFI check rather than another private UFS layout
measurement.

BPF retains the full bounds-propagation helper change and optional
`BPF_UNPRIV_DEFAULT_OFF` configuration, kernel initializer and sysctl
handler. The standard handler permits values 0..2, retains the privileged
write boundary and irreversible state 1, and does not silently change the
selected default. No live BPF permission or sysctl setting is modified.
The existing atomic write-count helper is unchanged.

Ext4 applies the complete inconsistent index/leaf extent check while
retaining its selected OEM extent caching context. USB Ethernet applies the
original setup/MAC-address locking across the existing private-info hook,
without changing the hardware callbacks. Standard F2FS completes the
original final-xattr sanity-check diagnostic; the bounds, FSCK marking and
error path already existed. Standard F2FS remains unselected, so this is
not selected OEM F2FS xattr acceptance.

The THP fallback uses the original standard nodemask allocator only in the
non-AMA branch. Selected `CONFIG_MM_AMA=y` retains a separate legacy
allocator that changes `mems_allowed` and ignores the VMA nodemask. That
behavior is not fixed or certified by this standard branch patch; it remains
a named OEM mempolicy integration issue, not an undisclosed equivalence.

Audit queue robustness, AX25 detach, SCTP endpoint RCU and FunctionFS eventfd
cleanup are semantic PRESENT cases with complete source contracts. Existing
locking/reference behavior and newer local guards are retained. AX25/SCTP
remain unselected; FunctionFS's source classification is not proof of every
concurrent OEM reset path. Other PRESENT entries use the complete original
reverse patch at their ordered historical source state.

### Selected Source And Configuration Gates

The seven original include-directory patches affect six paths; four include
files and 25 total headers actually change. All changed headers are
classified against the selected configuration. Forty-one unique legacy
declarations, thirteen previously measured record names, fourteen generated
offsets and the corrected assembly sources have unchanged source contracts.
No extra private compiler frontend or repeat of thirteen UFS instructions is
required. Const-sort and all-tag declaring/implementation files remain
identical; fresh final-address continuity is still checked.

The new MMC and BPF callback types match the existing class and ctl-table
members. TUN/PACKET are selected and consume the changed virtio helpers even
though `VIRTIO_NET=n`, so actual consumer objects are checked rather than
misclassified as disabled. Standard TEE is unselected; its real refcount
record/file-operation changes and lock-protected get/put lifecycle receive
source review, not a fabricated current compiled or OEM firmware guarantee.
FUSE's new time-update helper preserves four existing private record
declarations and is checked through selected direct consumers.

One actual olddefconfig/prepare run preserves every effective selection and
all fourteen generated OEM values. Emitted configuration options increase
from 6,431 to 6,432 only because the new BPF default-off bool is explicitly
disabled. General COMPAT remains selected; COMPAT_VDSO was disabled and
remains disabled despite its upstream dependency broadening for Clang/LLD.
The existing compiler flags, signing material and enforcing protections
remain. The following final build and installed gates are separate from
this source/configuration evidence.

### Full Build And Installed Acceptance

The complete build succeeds naturally as 5.10.90 #59 without another source
repair. Ordinary OEM compiler/linker warnings and the inherited boot-time
exit/init section-mismatch notice remain visible. Five final reports bind
the same source, configuration, ELF and 46 distinct actual objects. Fifteen
selected physical record names are covered: thirteen previous records retain
explicit source/command/configuration continuity, and only the real class
and ctl_table callback members are newly measured from eleven selected
natural build objects. No private compiler frontend or repeat of unaffected
UFS machine operands is performed.

The class is 120 bytes with shutdown_pre at offset 72; ctl_table is 64 bytes
with proc_handler at offset 32. The actual MMC class initializer and BPF
sysctl table point to the final canonical functions. Their precise types,
CFI membership and enforcing device_shutdown/proc_sys_call_handler indirect
branches pass. MMC's direct stop helper cancels delayed card-detect work.
BPF's actual body retains CAP_SYS_ADMIN and the compiled min/max conversion;
its kernel initializer is zero under the selected disabled default-off bool.
Inlining is read from the actual object, not mistaken for an absent check.
No live permission or sysctl setting is changed.

Selected TUN/PACKET consume the updated virtio helpers through their actual
compiled inline paths; selected FUSE callers use the direct time-update
helper. Existing const-sort and MAS all-tag objects are byte-identical to
#58, so old implementation IR is not decoded again. Their source, exact
commands and configuration remain bound, with fresh final canonical targets
and enforcing type memberships checked. All fourteen generated offsets and
both accepted affected/unrelated assembly proof scopes retain explicit
continuity. These bounded checks do not certify all opaque OEM ABI, actual
MMC shutdown stress, packet traffic or filesystem concurrency.

All 13,829 export CRCs are unchanged versus #58, with no addition or removal.
The 390 rebuilt modem imports and two optional dummy imports match. The
project still uses same-build pairing rather than inferring arbitrary binary
reuse from CRC equality. The new stripped modem's CMS signature, embedded
kernel certificate, original ext4 ownership/label and file readback pass.
The boot image retains the original header, firmware stub and OEM DTB with
verified kernel roundtrip and real partition bounds.

One paired installation reaches normal Android with automatic matching Live
modem loading. Full boot/modem partition, signed module and independent
ramdisk readbacks pass, as do the loaded build-id, user 0 CE storage,
RUNNING_UNLOCKED and three core services. SELinux Enforcing, taint 4,
enforcing CFI, forced signatures and PRMEM remain. Complete time-zero and
subsequent logs contain no actual kernel or module fault. All 516 AVC denials
remain visible, none for module loading. Original tracing state and the
Asleep screen are restored; no second boot or broad hardware replay is run.

The #58/5.10.85 pair is the immediate rollback. Accepted #59 metadata and the
latest #55-59 pairs remain local below 20 GB. Obsolete #54 test artifacts
are retired only after matching their retained build contract; original
images, the first accepted bring-up pair and source history remain intact.
Next is the complete 5.10.91-95 range, with fresh selected clocksource,
packet-scheduler, XFRM and cgroup trace-record gates and a changed block-PM
signature. The original 5.10.94 XFRM patch omits native MTIMER attribute
length policy: official stable follow-up `614811692` from 5.10.192 is planned
as a separate early pick after the complete ordered range, before building.
It is not included in the 663-entry range or counted as already installed.
The selected AMA mempolicy issue, pinned 5.10.274 target, Android common
parity, GKI and full inherited hardware/power acceptance remain open.
No release export, publication or push occurs.

## Eleventh Milestone: 5.10.91 Through 5.10.95

The complete range resolves all 663 official entries: 641 exact picks,
eight code adaptations, five release markers, seven already-present fixes
and two missing-prerequisite driver entries. All 654 original-author source
commits end at `e4353a21c`. A separate exact official XFRM policy follow-up
ends the build source at `ec2b3b670`; it is not counted in the original range.
The accepted two-file UFS offset repair remains. No intermediate 91-94
kernel is built or installed. Five per-release ledgers retain every
original/local identity and disposition.

| Release | Official Endpoint | Entries | Exact | Code | Marker | Present | N/A |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 5.10.91 | `df395c763ba08b8b4385481af07d5d1c658dd917` | 44 | 41 | 0 | 1 | 2 | 0 |
| 5.10.92 | `c982c1a83932dccbfe8b8106bd6385be8330e5a5` | 26 | 25 | 0 | 1 | 0 | 0 |
| 5.10.93 | `fd187a4925578f8743d4f266c821c7544d3cddae` | 24 | 21 | 0 | 1 | 2 | 0 |
| 5.10.94 | `c525532e4f872078206789f5bcd12bba7f689780` | 561 | 547 | 8 | 1 | 3 | 2 |
| 5.10.95 | `77656fde3c0125d6ef6f7fb46af6d2739d7b7141` | 8 | 7 | 0 | 1 | 0 | 0 |

Sources are the original stable range and official ChangeLog-5.10.91 through
ChangeLog-5.10.95 at <https://cdn.kernel.org/pub/linux/kernel/v5.x/>. Actual
independent import verification checks all 654 original authors, dates,
complete messages and stable trailers. The 641 exact picks cover 788 complete
file sections. Existing executable-mode differences are retained in fifteen
exact picks: sixteen file sections across fourteen distinct paths. No mode
transition is introduced.
Eight code adapters, five SUBLEVEL-only markers, four historical PRESENT
cases with seven complete post-hunks, three semantic PRESENT contracts and
two actual absent-driver exceptions are bound to historical blobs/modes.
All 663 ordered source inventories, five ledgers and the final original
kernel tree match the approved reconstruction. The accepted #59 docs root
is rebound additively without replacing it with a private source snapshot
or replaying the already-reviewed source decisions.

### Standard Interfaces And Preserved Platform Contracts

UFS moves device driver-data publication ahead of its core callback setup
and removes the original three redundant upstream setters. The conditional
HUFS timer_irq argument and existing OEM probe/core body remain; this is
not a claim about every pre-core OEM lifetime race. ARM LPAE uses the
complete original physical-address-to-descriptor formatter with both
caller data arguments, retaining the existing multi-page map/unmap API
and explicit one-PTE sync count. No unrelated Apple or Huawei IOMMU
hardware certification is inferred.

The managed-DMA helper and declaration are applied across the appended OEM
bulk allocator without replacing it. PL011 removes redundant MMIO request
callbacks while retaining devm resource ownership, mapbase validation and
OEM PM/ioctl members. The standard block-PM runtime-resume helper changes
to its actual single queue argument together with the selected SCSI caller;
no cast or old-signature shim is added.

The standard F2FS IO-align reserve/accounting/mount/sysfs contract is complete,
but standard F2FS is unselected and the selected HW_F2FS implementation is
separate and unchanged. UNIX's global inflight/GC ONCE annotations retain
the existing plain per-socket counters, listener locks and io_uring cleanup
refinements. MCEUSB's four timeout sites, PPP's minimum packet size and
host libelf linker flags are semantic PRESENT cases; their retained stronger
checks and actual disabled-device guards are explicit. The absent Qualcomm
DWC3/regulator drivers are recorded as missing prerequisites, not restored
or misclassified as successful picks. MEDIA_TEST_SUPPORT changes help text
only. These source contracts do not certify hardware or stress behavior.

### Separate Official Native XFRM Policy Follow-Up

The original 5.10.94 mapping-rate-limit patch adds three xfrm_state members,
the UAPI MTIMER attribute and complete clone, mapping and user lifecycle
changes, preserving the existing ESN-pointer guard. It also adds the compat
NLA_U32 entry but omits the native entry in the original upstream patch.
The selected native reader can therefore consume an undersized attribute;
faithful source adaptation alone does not close that validation boundary.

Separate local commit `ec2b3b670` applies official stable
`614811692e21cef324d897202ad37c17d4390da3`, upstream
`5e2424708da7207087934c5c75211e8584d553a0`, without altering the original
663-entry ledger. The fix first appears in 5.10.192 as entry 81 of 130 and
addresses CVE-2023-3773. One native policy line declares NLA_U32. Its complete
original author, message, upstream/stable provenance and one-file/one-line
scope pass independent actual-source verification before building. Every
other source file remains unchanged.

The existing deprecated native parser then rejects payloads shorter than
four bytes with `-ERANGE` before the AE reader. This is a minimum-length
guarantee, not strict exact-four-byte or whole-XFRM safety certification.
No malformed-input device probe, live XFRM configuration or permission
change is performed. When the later original stable entry is reached,
its disposition must account explicitly for this retained early pick.

### Selected Source And Configuration Gates

All 73 changed headers and sixteen include paths are classified against
actual device selections. Forty-one previous declarations retain their
contracts; three Qdisc declarations use exact token continuity because
other records in their whole header change. Fifteen accepted physical
record proofs, fourteen generated values and nine assembly sources retain
explicit source, actual configuration and compiler-command continuity.
No extra private frontend or old layout/assembly implementation replay is
required.

The changed selected clocksource, psched_ratecfg, HTB/police embeddings,
xfrm_state and three cgroup raw trace records require natural full-build
object checks. Existing perf/XFRM callback member contracts need only
bounded actual type/dispatch context, not invented callbacks. COMPAT and
Android compat routing do not select the absent standard XFRM_USER_COMPAT
module. Standard F2FS and DEVTMPFS remain unselected; no compiled or device
node behavior is attributed to them. The selected clocksource record still
matters with WATCHDOG disabled.

One actual olddefconfig/prepare run leaves all 6,432 emitted options and
every effective selection unchanged. Nine Kconfig file changes are reviewed
against ARM64 rather than assuming new ARM/MIPS or unselected-driver options
are enabled. Signing material, CFI/ThinLTO, forced signatures, SELinux and
PRMEM configuration remain. All fourteen OEM-generated values match #59.
The actual fixed source and 285 selected source inputs bind the separate
follow-up explicitly. The following full-build, final record/CFI, module and installed gates are
separate from this passed source/configuration evidence.

### Full Build And Installed Acceptance

The complete build succeeds naturally as 5.10.95 #60, with no additional
source repair or forced build counter. Inherited OEM compiler/linker warnings
and the boot-time exit/init section-mismatch notice remain visible. Five
final reports bind the fixed source, configuration, ELF and 72 distinct actual
objects. Fifteen accepted physical record proofs retain complete declaration,
configuration and compiler-command continuity; fourteen generated offsets,
nine assembly sources and the accepted thirteen affected plus eighteen other
machine-operand scopes are inherited without unchanged measurement replay.

Only sixteen necessary natural objects are decoded for eight changed records:
clocksource, psched_ratecfg, HTB and police embeddings, xfrm_state and three
cgroup trace records. The same-source xfrm_mgr callback member is measured as
an explicit ninth context, not another changed record. Clocksource uncertainty,
packet-rate MPU and by-value embeddings, the three mapping-state fields and
64-bit cgroup trace IDs match the selected compiled layouts. Nine real public
export signatures and final canonical CFI memberships pass. The standard
single-queue block-PM resume signature matches the actual selected SCSI call;
no old-parameter cast or CRC shim is used.

The actual XFRM static manager initializer points to its final canonical
new_mapping function, with precise type and enforcing dispatch membership.
Selected ARM64 Perf reads its guest callback pointer once with acquire
semantics under the actual RCU lifetime, then uses the correct typed indirect
branches. No unselected KVM callback is invented. The selected UFS core stores
its typed hba pointer in device driver_data before the first core error/callback
and mutex initialization; the conditional HUFS timer argument remains. LPAE's
actual formatter receives both caller data arguments. Selected PM/MM helpers
and affected consumers bind their natural current types and bodies. These
bounded gates do not certify all opaque OEM layout, pre-core lifetime races,
packet/filesystem concurrency or hardware behavior.

Only lib/list_sort.o is byte-identical to #59. Its old implementation proof is
reused; the five changed comparator-caller objects receive current typed-call
checks and fresh final canonical/type-membership checks. All four MAS all-tag
objects change despite complete compiler-command continuity. Current tag
executable CFG/types match the stored standard reference, STATIC and STARTED
contract; MQ last-reference/end_io and the three SCSI callbacks retain actual
precise types and calls. Flush saves its nonzero rq_status, clears the reusable
request status and completes with the saved value. Fresh bitmap/request/end_io
canonical membership and flush assignment/getter/put identity pass. Across
these bounded consumers, 24 distinct current IR objects require 25 decodes:
MQ is read once for sort and once for different ref/end_io bodies after the
first cache was consumed. No successful sort gate is rerun, no old IR or
assembly is decoded again, and no private compiler frontend is added.

All 13,829 exports remain, with no addition/removal and 838 real CRC changes
versus #59. The 390 rebuilt modem imports and two optional dummy imports all
match. Public block-PM, clocksource, packet-rate and XFRM CRC changes are
retained, not concealed; perf registration and the conditional ufshcd_init CRC
remain unchanged. CRC matching is not whole-binary or stable-KMI certification.
The stripped modem's CMS signature, embedded kernel certificate, original ext4
ownership/label and byte readback pass. The boot kernel roundtrip, original
header/stub/OEM DTB and actual partition bounds pass.

One paired boot/modem_driver installation reaches normal Android with automatic
matching Live modem loading. Full boot/modem partition, signed module and
independent ramdisk readbacks pass, together with loaded module build-id,
user 0 CE storage, RUNNING_UNLOCKED and three core services. SELinux Enforcing,
taint 4, enforcing CFI, forced signatures and PRMEM remain. Complete time-zero
and subsequent logs contain no actual kernel or module fault. All 517 AVC
denials remain visible, none for module loading. Original tracing state and
the Asleep screen are restored; no second boot or broad hardware replay is run.

The #59/5.10.90 pair is the immediate rollback. Accepted #60 metadata and the
latest #56-60 pairs remain local below 20 GB. Obsolete #55 test artifacts are
retired only after matching the retained build contract; original images,
the first accepted bring-up pair and source history remain intact. Next is
the complete 5.10.96-100 range. The selected AMA mempolicy issue, pinned
5.10.274 target, Android common parity, GKI and full inherited hardware/power
acceptance remain open. No release export, publication or push occurs.

## Twelfth Milestone: 5.10.96 Through 5.10.100

The complete range resolves all 208 official entries: 189 exact picks, three
code adaptations, five release markers and eleven already-present fixes.
All 197 original-author source commits end at `25e0e4b59`, without
an integration commit in the original range. A separate one-line cpuset lock
assertion repair ends the build source at `96e409478`; it is not counted in the
original 208 entries. The accepted native XFRM policy fix and UFS generated-
offset repair remain intact. No intermediate 96-99 kernel
is built or installed. Five per-release ledgers retain every original/local
identity and disposition.

| Release | Official Endpoint | Entries | Exact | Code | Marker | Present |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| 5.10.96 | `f255ac9e8776ffe10c6b31a27bb438debca85a81` | 101 | 93 | 3 | 1 | 4 |
| 5.10.97 | `c8ed22bd97d47b7803173c4e2bd3cfd52693cf7f` | 24 | 19 | 0 | 1 | 4 |
| 5.10.98 | `12a0a56cbae34596d3cc771d461e73ec95606e91` | 3 | 2 | 0 | 1 | 0 |
| 5.10.99 | `fb063a6465f9d17a1bd46a678bc1ffaa84e2f486` | 75 | 71 | 0 | 1 | 3 |
| 5.10.100 | `d4f7d322a4ad1d1a280f1b1f36df8d0eb855966c` | 5 | 4 | 0 | 1 | 0 |

Sources are the original stable range and official ChangeLog-5.10.96 through
ChangeLog-5.10.100 at <https://cdn.kernel.org/pub/linux/kernel/v5.x/>. Independent
original-source review covers all 208 raw commit objects, authors/dates,
complete messages, patches and historical modes. The 189 exact picks cover
237 complete file sections. Existing executable-mode variants remain in six
picks: eight sections across six distinct paths, with no new mode transition.
Three complete code adapters, five SUBLEVEL-only markers, three semantic
PRESENT contracts and eight complete original reverse-PRESENT checks with
twenty post-hunks are bound to immutable historical blobs/modes.

The actual 197 linear source commits retain those exact authors/dates,
complete messages and stable trailers. All 208 actual before/after file
inventories, all 197 changed-path sets, every row in five ledgers and the
five version roots match the approved historical reconstruction. Exact
historical identities carry the earlier full source/semantic proofs without
another original/local patch-stream or implementation replay. The accepted
#60 documentation root is rebound additively; it is not replaced with the
private preview tree. All 213 source-path inventories, 198 real changed paths,
25 changed headers and seven changed include paths remain explicit.

### Standard Interfaces And Preserved Platform Contracts

The ARM64 erratum 1418040 change applies the complete original helper/comment
and switch/new-exec hooks, while retaining ptrauth, MTE, compatibility migration
and vendor-hook context. This erratum is disabled in the accepted configuration,
so the source adaptation is not a hardware-workaround runtime certificate.
Generic TCPM uses the original SNK_ATTACH_WAIT/SNK_DEBOUNCED VBUS-off no-op;
the OEM debounce reset tied to the removed disconnect is removed with it.
Every other handler branch remains. Generic TYPEC is unselected and is not
confused with the separate selected OEM Type-C implementation.

Fsnotify invalidates the dcache before sending IN_DELETE using the complete
original 5.10 changed functions and both new helpers. Existing surrounding
OEM namei context and executable mode remain, as does the original NFS
sillyrename exception. No broad VFS rewrite or newer API shim is substituted.
This paragraph records source adaptation; full-build evidence is below and
does not establish filesystem/network race acceptance.

The /proc/net/ptype namespace filter and packet/fanout constructors already
match the complete original post-state; the packet_type record retains its
existing KABI reserve suffix without layout changes. PSI's one-trigger
lifetime fix already spans all five relevant files. Its complete trigger,
cgroup and kernfs release/acquire/serialization contracts retain the existing
cgroup context slot and accepted poll_scheduled reset; no source or record
change is introduced by the PRESENT classification.

Both release_agent privilege paths already match the original fix, with a
later official correction retained in the write handler. Upstream
`467a726b754f474936980da793b4ff2ec3e382a7` checks the opener's file credentials
and the cgroup namespace owner rather than the current caller's credentials.
The full mount parser remains the original post-state. These stronger checks
must not be reverted to the older intermediate capable() implementation.
No live release_agent configuration, permission or userspace setting changes.

### Separate Cpuset Lock-Assertion Integration

The first actual full build fails at the newly added sibling-update assertion.
Official stable `557727313534d9f92e782179372833fd3499f231`, upstream
`2bdfd2825c9662463371e6691b1a794e97fa36b4`, names cpuset_rwsem, while this
Huawei tree retains cpuset_mutex for the same sleepable hierarchy transaction.
The exact original source patch is still recorded as such; its source fidelity
alone does not make that lock name compile in this older locking context.

Separate local commit `96e409478` changes only that assertion to the standard
lockdep_assert_held(&cpuset_mutex), matching three existing assertions in the
same file. Both direct sibling-update callers and all three outer write/offline
paths hold this mutex. No new lock, include or layout change is introduced;
callback_lock remains confined to the same short assignments. The full sibling
helper retains the original css_tryget_online reference, RCU unlock before the
sleepable update, relock and css_put sequence.

Independent actual-source review confirms exactly one line replaced in one
mode-preserved file, with every other source file and both direct/three outer
functions unchanged. The original 197 commits, five ledgers and their full
historical source proofs remain separately frozen. LOCKDEP is unselected, so
this source assertion is not a claim of runtime lockdep stress acceptance.
The integrated selected-source binder inherits the original 328 identities
and adds the real cpuset file explicitly, for 329 sources; it does not pretend
that the original scope included that file. The same successful actual
configuration run and generated-input hashes are reused without repeating
olddefconfig/prepare. Successful full build and installed acceptance remain
separate from this one-line source/configuration review.

### Selected Source And Configuration Gates

The exact Perf time fix removes perf_event.shadow_ctx_time and adds
perf_event_context.timeoffset. Selected perf_cpu_context embeds that context
by value, so it is an explicit third changed physical record. PERF_EVENTS=y
requires fresh natural build evidence from the actual core, ARM_PMU, ARM64
and OEM phase_perf consumers. The fourth changed declaration,
perf_cgroup_info, is not selected because CGROUP_PERF=n; no artificial current
object or layout certificate is constructed for it. Existing Perf guest
callback types and their enclosing public API remain unchanged.

All 25 changed headers and seven include paths are classified against the
actual device configuration. Twenty-three previously accepted physical
records and forty-one legacy declaration-file contracts retain exact source
continuity. Nine assembly sources, their generator, all fourteen generated
values, const-sort and MAS all-tag source contracts remain whole-file
identical. The original selected source binder covers 328 paths; the separate
cpuset integration makes that 329, retaining 75 accepted #60
object/entire-command witnesses. These inherited proofs do not imply blanket
opaque-OEM ABI or stable GKI KMI certification.

Only two Kconfig files change. The framebuffer-console legacy-acceleration
option depends on an unselected FRAMEBUFFER_CONSOLE and its y default is
PARISC-only; ARM64 leaves it disabled. NET_DSA_MT7530's new MEDIATEK_GE_PHY
selection is inactive because that switch is disabled. The actual option
name is CONFIG_MEDIATEK_GE_PHY. PowerPC's latent-entropy per-file compiler
flag changes do not select any ARM64 implementation. BLK_DEV_INTEGRITY and
standard F2FS remain unselected; selected OEM filesystem code is separate.

One actual olddefconfig/prepare run preserves all 6,432 emitted options and
every effective selection. All fourteen generated OEM values, signing
material, CFI/ThinLTO, forced signatures, SELinux and PRMEM settings remain.
The actual 197-commit source, reviewed configuration and original selected-
source/command binder pass before the first full compilation. After the
cpuset source integration, its independent source review and new 329-source
binder preserve the unchanged actual configuration and command witnesses.
Full build, final natural record/CFI, module and installed acceptance remain
separate gates.

### Full Build And Installed Acceptance

After the separate one-line cpuset assertion repair, the complete build
succeeds naturally as 5.10.100 #61, without a forced counter or configuration
change. Inherited compiler/linker warnings and the boot-time exit/init section
notice remain visible. Five final reports bind the integrated source,
configuration, final ELF, export table and 75 distinct current objects with
their complete build commands.

The twenty-three accepted physical records and forty-one declaration-file
contracts retain exact selected-source, effective-configuration and compiler
continuity. Their old measurements are not replayed. Fourteen actual generated
offsets, nine whole assembly sources, thirteen corrected and eighteen other
scoped machine operands retain the accepted proof without re-disassembly or
numeric layout padding. These are bounded contracts, not all opaque OEM ABI.

Only four necessary natural objects are decoded, once each: Perf core, ARM
PMU, ARM64 Perf and selected OEM phase_perf. Three changed physical records
measure perf_event at 1072, perf_event_context at 304 and perf_cpu_context at
504 bytes; timeoffset is at byte 240 of the context. The by-value embedding,
real field widths, memory operations, precise types and actual indirect
dispatch agree across the selected consumers. Incomplete opaque-pointee DWARF
descriptions do not constitute a layout difference; each consumer's descriptor
is retained. CGROUP_PERF=n, so no fourth-record layout proof is fabricated.
Existing public signatures, selected PM/MM/LPAE/UFS/XFRM and Perf acquire/RCU
implementation proofs retain source/command continuity; fresh final canonical
CFI memberships and static callback targets are checked separately.

Only list_sort.o is byte-identical to #60. Its implementation proof and the
five changed caller objects' precise comparator/direct-call contracts are
inherited under complete source/command continuity; fresh canonical jumps
and type memberships pass without another IR decode. All four MAS all-tag
objects also change, but their seventeen complete sources and four complete
commands remain identical. Accepted tag/reference/STATIC/STARTED, callback and
flush saved-status/reset contracts are inherited; fresh typed canonical and
flush assignment/getter/put identities pass. Neither gate replays unchanged
implementation tests or reads old IR.

The final table has 13,831 exports: two additions, no removals and 1,512 real
CRC changes versus #60. New exports are jbd2_journal_grab_journal_head and
jbd2_journal_put_journal_head. Perf interface changes are recorded. Despite
unchanged conditional ufshcd_init signature source, its real transitive CRC
changes from 0xba066086 to 0x06aac99b; it is not disguised as equality.
perf_register_guest_info_callbacks and blk_post_runtime_resume CRCs remain
equal. All 390 rebuilt modem imports and two optional dummy imports match.
Same-build pairing is retained, not an arbitrary old-module or stable-KMI
guarantee. The stripped modem's CMS signature, kernel certificate, original
ext4 ownership/label and byte readback pass. Boot roundtrip, original header,
firmware stub, OEM DTB and actual partition bounds pass.

One paired boot/modem_driver installation reaches normal Android with
automatic matching Live modem loading. Complete partition and signed-module
readbacks, live build-id, unchanged independent ramdisk, user 0 CE storage,
RUNNING_UNLOCKED and three core services pass. SELinux Enforcing, taint 4,
enforcing CFI, forced signatures and PRMEM remain. Complete time-zero and
subsequent logs contain no actual kernel or module fault. All 517 AVC denials
remain visible, none for module loading. Original tracing and the Asleep
screen are restored; no second boot or broad hardware replay is run.

The #60/5.10.95 pair is the immediate rollback. Accepted #61 metadata and the
latest #57-61 pairs remain local below 20 GB. Obsolete #56 test artifacts are
retired only after matching the retained build contract; original images,
the first accepted bring-up pair and source history remain intact. Next is
the complete 5.10.101-105 range. Selected AMA mempolicy, the pinned 5.10.274
target, Android common parity, GKI and inherited hardware/power acceptance
remain open. No release export, publication or push occurs.

## Thirteenth Milestone: 5.10.101 Through 5.10.105

The complete range resolves all 483 official entries: 440 exact picks,
eighteen code adaptations, five release markers, sixteen PRESENT decisions
and four missing-prerequisite entries. The PRESENT scope contains seven
semantic contracts and nine complete historical reverse checks. All 463
original-author source commits end at `138273525`. Five separate integrations
and one exact early upstream follow-up end the build source at `5b0828436`;
none changes the original import counts or dispositions. No intermediate
101-104 kernel is built or installed. Five per-release ledgers retain every
original/local identity and classification.

| Release | Official Endpoint | Entries | Exact | Code | Marker | Present | Missing |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 5.10.101 | `3969aba589d663b2a704f31555c2caead75a1646` | 117 | 106 | 3 | 1 | 7 | 0 |
| 5.10.102 | `47667effb7d2f54dab3be33749c5a05152c07db0` | 121 | 110 | 3 | 1 | 6 | 1 |
| 5.10.103 | `915a747ac7f368a9979638e86fb9097d6a903672` | 80 | 73 | 2 | 1 | 2 | 2 |
| 5.10.104 | `97581b56b59fc79d6c376994a2e219349c31873f` | 106 | 102 | 2 | 1 | 1 | 0 |
| 5.10.105 | `67c781d938b850db236f6eb0bccc9737c29df57c` | 59 | 49 | 8 | 1 | 0 | 1 |

Sources are the original stable range and official ChangeLog-5.10.101 through
ChangeLog-5.10.105 at <https://cdn.kernel.org/pub/linux/kernel/v5.x/>. Independent
whole-source review and actual ordered-import checks bind original commit
objects, authors/dates, complete messages, patch contents, historical modes,
all adaptations/PRESENT decisions, the five release roots and every ledger
row. Additive approval preserves the original review inputs rather than
replacing them with a later private preview. The accepted #61 documentation
root is carried separately from the kernel-source reconstruction.

### Standard Interfaces And Retained Lifetimes

The complete six-path PM wakeup change adopts the standard two-slot IRQ queue,
raw lock, clear-by-IRQ semantics and `pm_wakeup_irq()` reader. Existing
first-wakeup diagnostics run outside the raw lock using the captured first-slot
decision; suspend-abort diagnostics and independent s2idle reason clearing
remain. Separate integration `271544ce2` converts the unselected OEM sensor
reader from direct variable access to the new API. This preserves source API
coherence, not sensorhub selection or runtime coverage. The core PM change is
a genuine API migration, not an ABI-preservation claim.

Network qdisc pointers gain the standard RCU annotation and matching
publication/access operations. The existing publish-before-notify graft
ordering and noqueue class rejection remain; the annotation itself does not
change physical net_device layout. Cpuset attachment acquires the CPU-hotplug
read lock before cpuset_mutex and releases it after the complete attachment
transaction. Per-task masks, Android affinity hooks, memory policy and counters
remain. The accepted #61 mutex assertion repair is inherited separately, not
folded into the original 5.10.103 patch or replayed.

File lookup adopts the full standard RCU fd-table retry algorithm, including
bounds/nospec validation, reference acquisition and table/entry identity
rechecks. Only the existing newer helper name changes adaptation context.
Pidfd installation moves outside both fork locks while retaining OEM/security
hooks and failure cleanup. IPv6 shutdown snapshots readiness before clearing
flags and calls multicast shutdown at most once, preserving the newer
address-list teardown. These are full-source changes and later build checks,
not dedicated live filesystem/network race stress acceptance.

The selected LTO/CFI module linker already retains the standard split-section
merge, page-aligned CFI checks/jump tables and unwind discard, so both related
5.10.102 entries are PRESENT without regressing the newer linker. AX88179 RX
retains the stronger official later implementation and its packet/metadata
bounds. UFS link-loss classification, FunctionFS file locking, RNDIS command
bounds and AX25 lifetime likewise retain complete-function presence proofs,
not isolated matching tokens.

### ARM64 Security And Separate Integrations

The complete AA64ISAR2 register definition, capture, initialization, update
and read paths retain the ISAR1 override and separate AArch32 record. The new
u64 field genuinely changes cpuinfo_arm64. Trampoline source keeps three text
slots, full leaf-page mapping, readonly data and original page permissions;
only the requested size assertion changes in the linker. CFI/HYP/PRMEM,
protected boundaries and the extra reserved page directory remain, including
the required two-page TTBR adjustment. Literal reads retain both KASLR and
non-KASLR branches and existing map/unmap/SDEI exits.

The stable 105#38 KVM backport requires absent legacy template/slot/SMCCC
machinery. Its missing-prerequisite outcome does not mean the issue is fixed.
Separate integration `d809ea3b8` adopts the complete modern upstream
`5bdf3437603d4af87f9c7f424b0c8aeed2420745` implementation, including its extra
mapping consumer, instead of downgrading the retained Android/pKVM model.
Integration `798842d1a` allocates BHB to the retained free capability 62,
preserving LDAPR 59, pKVM 60, HHEE 61 and ARM64_NCAPS 76. Integrations
`ce4603098` and `b8c4b1c83` complete modern BHB nVHE aliases and the modern
ClearBHB alias/hyp-entry additions. These are separate source integrations,
not extra original stable entries or KVM runtime certification.

The other missing prerequisites are outgoing-CPU deferred RCU IRQ-work,
absent Bay Trail DWC3-PCI tables and absent voluntarily threaded gadget IRQ
context. None justifies recreating obsolete infrastructure or adding duplicate
bottom-half handling to the retained tasklet path.

### Actual Build Failure And Exact Upstream Follow-Up

The first full build reaches an unexpanded ARM_SMCCC_ARCH_WORKAROUND_3 operand
in entry.o. Preprocessing identifies the LTO assembly include chain through
arm-smccc.h, init.h, compiler.h, rwonce.h and assembler.h before SMCCC constants
are defined. It is not an unencodable resolved immediate. The complete
official upstream `52c9f93a9c482251cb0d224faa602ba26d462be8`, "arm64: Do not
include __READ_ONCE() block in assembly files", is imported separately as
`5b0828436`. Its two-line guard/comment change excludes the C-only LTO block
from assembly. Native C READ_ONCE, atomic/type handling and RCpc/LDAPR remain
unchanged; no SMCCC instruction rewrite, protection bypass or feature disable
is used. Original source/configuration proofs remain immutable, with one-path
additive source/configuration bindings after this exact follow-up.

One actual olddefconfig/prepare run produces 6,433 options from 6,432. The only
effective/emitted change is CONFIG_MITIGATE_SPECTRE_BRANCH_HISTORY=y; all other
selections remain. KVM, UNMAP_KERNEL_AT_EL0 and EXT_INPUTHUB are disabled.
Fourteen generated OEM offsets, signing material, Werror, ThinLTO, enforcing
CFI, forced signatures and PRMEM remain. The assembly-only follow-up reuses
the same actual configuration; olddefconfig/prepare is not repeated. The
integrated selected-source binder grows from 451 to 452 explicit identities.

### Full Build And Installed Acceptance

The corrected full build succeeds naturally as 5.10.105 #62 without a forced
counter. Five final reports bind the successful build, source/configuration,
final ELF/export table and 85 distinct current objects/entire commands. The
twenty-six accepted physical records, fourteen generated offsets, nine whole
OEM assembly sources and thirteen corrected/eighteen other scoped operands
retain source/selection/compiler continuity without old IR/disassembly replay
or numeric padding. Const-sort and MAS all-tag/reference/flush implementation
proofs are inherited; final canonical jumps, typed memberships and relocated
flush pointer identities are freshly checked.

Only nine new natural C objects are decoded once: wakeup, power/main, IPv6
flowlabel/fib6_rules, cpuinfo/cpufeature, nf_queue/nfnetlink_queue and proton-pack.
The three changed physical records measure cpuinfo_arm64 at 1176 bytes,
netns_ipv6 at 832 and its by-value net container at 4800. ISAR2 is at byte 1024;
the IPv6 member is at byte 1856 of net. Real field widths, memory operations,
types and consuming paths agree. The PM public reader/clear and void-to-bool
netfilter API are checked against actual optimized emitted consumers.

The new entry.o/final-ELF scope verifies three 2048-byte non-KPTI banks, each
with sixteen 128-byte slots, actual return/branch targets, WA3 firmware operand
and saved/restored registers. Natural BHB callback types, real cpufeature CFI
dispatch, capability-62 canonical pointers and per-CPU store-to-VBAR values
agree. Existing __apply_alternatives remains __nocfi; no new protection claim
is made for it. KVM/KPTI unselected targets are not fabricated, and normal boot
does not certify hardware-mitigation effectiveness or performance/power.

The final table has 13,831 exports: xfrm_state_mtu replaces __xfrm_state_mtu,
with 2,646 real CRC changes versus #61. nf_queue_entry_get_refs changes from
0xc2188a94 to 0xcd33a110; ufshcd_init and list_sort CRCs remain equal. All 390
fresh modem imports and two optional dummy imports match. Stripped modem
CMS, embedded certificate, ext4 ownership/label and readback pass. Boot
roundtrip, original header, firmware stub, OEM DTB and partition bounds pass.
Staged hashes are bound to the successful final build before flashing, not
merely checked after installation.

One paired boot/modem_driver installation reaches normal Android with
automatic matching Live modem. Complete partition/module readbacks, loaded
build-id, unchanged independent ramdisk, user 0 CE storage/RUNNING_UNLOCKED
and three core services pass. SELinux Enforcing, taint 4, enforcing CFI,
forced signatures and PRMEM remain. Complete same-boot time-zero/subsequent
logs contain no actual kernel/module fault; all 517 AVCs remain visible and
none requests module loading. Tracing and Asleep are restored without a
second boot or broad hardware replay.

The #61/5.10.100 pair is immediate rollback. Accepted #62 metadata and recent
#58-62 pairs remain local below 20 GB after contract-checked retirement of #57
test artifacts. Original images, the first accepted bring-up pair, immutable
source proofs and needed reference dependencies remain. Next is complete
5.10.106-110. Selected AMA mempolicy, the pinned 5.10.274 target, Android common
parity, GKI and inherited hardware/power acceptance remain open. No release
export, publication, tag or push occurs.
