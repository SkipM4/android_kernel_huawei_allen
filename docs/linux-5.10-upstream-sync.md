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

At this source checkpoint the device remains the validated 5.10.49 #51,
with its matching pair retained as immediate rollback. The corrected full
build is in progress; no 5.10.50 boot pass, module-load pass or release claim
is made. Final ELF/layout, export/import CRCs, signatures, image bounds and
paired runtime results must pass before the checkpoint is accepted. The
pinned latest target remains 5.10.274; GKI/core-vendor separation is still a
later architectural task.
