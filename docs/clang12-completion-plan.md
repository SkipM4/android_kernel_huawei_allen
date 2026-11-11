# Bring-up Completion Plan

## Acceptance Rule

User requests stepwise repair and validation until all required work is
complete. Baseline is installed r45 from bringup/clang12 at 782c4f17e; immediate
paired rollback is r44. Each kernel iteration stays isolated, with matching
boot/modem_driver validation, unchanged independent ramdisk unless separately
required, retained security controls and accurate functional scope.

An item is complete only after its real acceptance condition passes. Source
availability, missing peers/SIMs and manual checks are external prerequisites,
not successful tests. A harmless/inapplicable diagnostic may be classified
instead of suppressed, but its functional uncertainty must be resolved before
closing the corresponding item. No dummy interface, fake success, protection
bypass, speculative broad backport or implementation-mirroring test suite.

## Work Queue

| Step | Work | Acceptance Condition | State |
| --- | --- | --- | --- |
| 01 | Wi-Fi external networking | Existing-network association, network-bound DNS/TLS and verified transfer; classify calibration access separately | Needs Unlocked Foreground UI |
| 02 | Cellular service | With an active SIM: registration, data and user-confirmed incoming/outgoing audio call; no guessed dial target | Needs SIM/Manual Check |
| 03 | Bluetooth peer audio | Pair/connect a supplied headset, playback/capture route and reconnect checks | Needs Peer/Manual Check |
| 04 | Frame RTG | Audit configuration/task/Binder/module ABI; restore real interface and validate intended caller behavior | r45 Root-Identity Fix Installed; Positive Global/Timeout/Scheduling Open |
| 05 | L3/DDR multi-level voting | Match DT/firmware vote contract, restore full tables/ratios and validate real level changes | r41 Bounded Pass; Unused Normalized Bookkeeping Classified |
| 06 | Thermal daemon fallback | Resolve calculated-result/source/unit contract and validate without disabling thermal protection | Normal-Screen Warm/Cool Recovery Pass; Charging/High-Temperature Safety Open |
| 07 | Wi-Fi calibration access | Identify caller/lifecycle/permission contract; prove correct access or justified non-use without widening protection | Two Empty Records Confirmed; Usage/Write-Safety Contract Open |
| 08 | Vendor-init WP action | Matching userspace/protection source, correct process-context reporting and genuine protection behavior | Needs Matching Source |
| 09 | PCIe affinity/delivery | Verify physical/logical interrupt topology and classify functional impact before any affinity implementation | Endpoint INTx Delivery Pass; Root PME/AER/Migration Open |
| 10 | Charging/standby | Appropriate charger and battery state; charging transitions, temperatures, overnight power and wake stability | Needs Hardware/Time |
| 11 | Camera/audio integration | User-confirmed front/rear recording, audio-video sync, microphone routes and calls/headset integration | Needs Manual Check |
| 12 | Final regression/security | Installed final pair, ABI/signatures/readback, enforcing controls and scoped regression with every required item closed | Pending |

Existing bounded passes include storage/display/touch/USB, hardware video
codec exercises, basic sensors, audible settings playback and recorder user
reports, bounded RTC/deep suspend, PERF_CTRL queries and repaired RTNL timing.
They are not automatically recertified on each new kernel. Repeat or extend
only when a relevant change, fault or uncovered requirement justifies it.

## Current Prerequisites

Read-only r37 connectivity baseline has no active default network, Wi-Fi OFF,
mobile data enabled and both SIM slots ABSENT. One saved Wi-Fi network exists.
USB is supplying power; battery reports full at the snapshot, which is not
enough to validate charging progression. The matching OEM init/plugin source
is absent, as documented in clang12-system-wp-source-audit.md.

A read-only r41 follow-up after its normal reboot still shows the keyguard,
Wi-Fi OFF, both SIM slots ABSENT and no active default network. No new radio
enable attempt or lock-screen bypass is performed during the MIPS validation.

A bounded Wi-Fi enable attempt does not establish a default network within
60 seconds. Service logs show screenOn=false; window policy shows the device
is keyguard-locked. Wi-Fi OFF and the original mobile/airplane/scan/location
settings are restored. A foreground unlocked settings check is needed before
classifying this result as a driver issue. No external transfer is attempted
without an active Wi-Fi network; no lock-screen bypass is used.

The follow-up snapshot still shows the same locked/asleep device and absent
SIMs. Continue source work while these manual prerequisites wait. The frame
RTG stock-feature closure changes only its five original flags in a temporary
configuration. Its affected objects compile, and three ignored nested user
copies are repaired. The timer now reuses its persistent worker on re-enable
and reports setup failures to the enable ioctl instead of false success.
Runtime scheduling remains disabled on r37; lifecycle synchronization,
authorization, complete link and module compatibility must pass before
exposing the restored interface. Enabled-build debug metadata shows unchanged
task_struct size but 23 shifted member offsets, so size alone cannot clear
this compatibility gate.

END_FREQ now follows the observed native/compat caller's self-thread scope.
An isolated original-feature full build is completed after removing obsolete
#2 cache while preserving configuration/symbol/signing metadata. The active
r37 output and device remain unchanged. The final #39 enabled link, matching
modem import CRCs and staged-module PKCS#7 verification pass. Its capability
getter uses the real RTG gate and handles standard four-byte compat queries
without removing the legacy eight-byte form. These are build/static-signing
results, not installed interface or application-use certification.
Read-only inspection of the actual
SELinux policy confirms ordinary application self:file access to render
commands 0xab23-0xab28, but not global boost 0xab64 in the examined lists.
Application render ownership, including force-init of an existing group,
requires repair before activation. The proposed self-process/system-control
boundary is awaiting user confirmation because it changes cross-process
behavior; do not silently block all application render calls or flash the
unresolved candidate.

Long-term records omit personal network/subscription identifiers, temporary
checksums and complete logs. Task-owned host/device probes and downloaded
test data are removed after results are recorded. Workspace remains below
20 GB; original backups and immediate rollback are preserved.

The L3 audit also finds silently truncated two-level DDR tables. All six
live tables match the original DT; the fixed two-column source is wrong.
Original Allen machine-code recovery establishes independent level ratios,
omitted-level zero semantics and a single maximum MHz vote. The source
repair passes independent native selector/parser comparisons and object
compilation. Final #41 link/pair checks pass, including startup/empty-sample
corrections using r37's frame-disabled configuration, with
r37 configuration/signing metadata retained separately. Paired flash/readback,
two normal boots and all six complete maps pass. Userspace's real compound
ratios are no longer truncated. All 26,078 real-sample vote selections match
the original machine code; the second level determines the maximum in both
c1/c2 L3 and DDR monitors, and 4,836 post-writel events show real vote changes.
These desired votes are not effective physical clock measurements. The filtered scheduler
counter API is retained rather than copying the incompatible original raw
producer. The subsequent original per-CPU normalized vote-scale audit finds
only its known writer/getter in the examined address-reference scope, with
no located call/export or field-pointer escape. It is classified as unused
bookkeeping, not a demonstrated active feedback gap; no unused state is
introduced. This practical audit is not a general alias proof or performance
parity certification. See clang12-mips-multilevel-audit.md.

The synchronized r41 cold-window follow-up still observes ten fallback
events per front/back calculation slot while physical inputs change and
published shell values remain stable. This does not certify warming recovery
or protection and does not justify changing stock coefficients or sensor
units. The subsequent publication audit confirms framework-owned integer
storage and named slot aggregation/smoothing in the stock daemon. Back's
published output is not slot 5 alone. Real warming/recovery remains open;
see clang12-thermal-source-audit.md.

The calibration read-only audit finds two all-zero records with no valid
CRC for the matching original/r41 layout. No contents or parameters are
transferred and no partition write occurs. The original kernel shares the
caller-credential read and automatic clear-on-sync-failure write gate.
Changing permissions or credentials cannot recover absent records and could
expose that destructive gate. Intended optional usage and authorized
read/write lifecycle still require resolution; see
clang12-calibration-access-audit.md. Device remains r41 without another
kernel edit, build or flash during these audits.

The subsequent live configuration audit finds MP16C/bisheng and
cali_data_mask=0x07 in the DT-selected configuration, with no mask override.
It requests NV reads rather than explicitly skipping them. Read refusal and
successful reads of the two zero records both leave the loader without a
valid selected record; no permission or mask change is justified to claim
calibration recovery. The source retains its no-new-NV-cache/DPD-not-ready
behavior, but real firmware calibration and RF/network correctness are not
certified by this source inspection.

The normal-screen thermal follow-up observes front recovery in a 157-second
window, then both front/back publication rising and falling during a separate
282-second screen-on/off extension. Slot-5 cached values update between
rejection episodes in the same daemon lifetime. This is not a permanently
frozen cache under the tested normal warming/cooling conditions. No thermal
protection or coefficient changes occur. The display is restored to Asleep,
radios/settings remain unchanged, and the kernel log covers the full
extension without warning/fatal markers; the first window's beginning has
rotated out. High-temperature throttling, charging and overnight-power
acceptance remain open, not inferred from this bounded recovery result.

The r41 topology follow-up identifies Wi-Fi's actual endpoint INTx IRQ82
separately from root-port PME/AER logical MSI IRQ267 and its physical GIC
dispatch IRQ83. A bounded Wi-Fi enable/disable interval observes 122 new
IRQ82 deliveries while 45 existing IRQ267 affinity refusals recur. Both
root MSI counters remain zero, so root PME/AER delivery/migration is not
certified. Wi-Fi and display are restored, the boot is unchanged and the
complete interval log has no warning/fatal markers. No affinity or kernel
change is made; see clang12-pcie-affinity-audit.md.

The user approves the application self-process/system-controller render
boundary. The shared five group backends enforce it on copied requests under
the existing render lock, including force-init takeover rejection. Failed
initialization no longer clears an existing group or the separate global
wake-queue buffer; task identity and RCU publication replace PID-only lifetime
assumptions. Enabled object and style checks pass. This does not activate RTG:
r41 remains installed, and current-source link/pair, enable/disable concurrency
and real native/compat/application behavior still require validation. See
clang12-rtg-source-audit.md for the independent global-queue scope limitation.

r42 completes current-source enabled link and matched boot/modem packaging,
paired flash/readback and two normal boots with unchanged independent ramdisk
and security controls. Real 64/32-bit untrusted_app_27 callers pass ownership,
force-init rejection, query-width canaries and exit cleanup. Actual UID1000
in the existing Magisk domain controls only test-child/self groups and passes
three disable/re-enable cycles with the same worker. The normal framework
configures RTG after reboot. A bounded post-enable MIPS telemetry capture has
2,622 samples, 862 with nonzero instructions, and 2,280 requested-vote events,
with zero overruns and unchanged monitor settings. This does not repeat the
full original-machine-code oracle or measure effective hardware clocks.

The independent PERF_CTRL global wake-queue lifecycle, actual timer expiry
under intended caller policy, full frame-thread scheduling/END_FREQ coverage
and performance/power parity remain open. An initial ARM32 process launch
during the temporary APK ABI change reports a secondary-zygote connection
refusal; an ordinary force-stop/relaunch of only that APK subsequently passes
on the unchanged kernel/policy. It is not claimed as a repaired zygote defect.
Existing SIM/peer/source/hardware/manual prerequisites remain unsatisfied;
the whole bring-up goal is not complete. Temporary app/tools/tracing and
consumed full logs are removed; r41 is the immediate paired rollback.

r43 addresses the independent global initializer's bool status variable.
Actual UID1000 native/compat probes reproduce positive 1 on r42 for failed
copy and missing PID; both return the existing negative EFAULT/ENODEV on r43.
The adjacent failed-copy getter remains EFAULT. No valid global initialization,
sampling, resume or destruction occurs. Full configuration/export/CRC and
certificate identity with r42, signed paired readback and normal startup/reboot
pass. Independent ramdisk and security controls remain unchanged; r42 is now
the immediate rollback. The global caller/permission and force/resume/depth/
exit cleanup contracts remain open. A scoped library immediate scan is not a
complete caller proof and does not justify speculative ownership changes.

r44 retires old global wake-queue memberships and logical counts before a
forced depth change, preventing hidden freed-task entries from being scanned
on later growth. Original Allen machine code confirms the inherited hazard
and the existing same-live-target pause/resume behavior. The small repair
retains that behavior and the existing lock, ABI, layout and caller policy.
Object/optimized IR, final ELF and full link checks pass. Configuration,
complete export/CRC table and certificate match r43; signed paired readback,
unchanged ramdisk and two complete normal boots pass with retained security.
r43 is now the immediate rollback. No old-kernel UAF is deliberately induced,
and no unknown system global queue is force-replaced for a positive probe.
Global positive runtime, root-retirement/PID-reuse identity, intended callers
and complete timer/frame scheduling acceptance remain open.

The pre-r44 read-only snapshot still has an asleep/locked device, both SIMs
absent and USB power. Only system_server has an observed open perf_ctrl
descriptor in that snapshot; this does not prove which commands it uses or
that global queues are inactive. No lock, permission, radio, thermal or
calibration bypass is attempted. Original backups and r17 plus the latest
r37/r41/r42/r43/r44 pairs remain; consumed task-owned diagnostics are removed.

r45 closes the source-level global root identity defect: non-force init/get
require the lookup-pinned task and the existing root retirement flag to
match, and wake/free hooks select the actual task pointer. PID removal can
precede final task free, so clearing the numeric token only in that hook
would be insufficient. Same-object pause/resume and force-init remain;
there is no new liveness, caller, UID, PID-namespace or ioctl-layout policy.
The destroy request's numeric guard is unchanged, not a general delayed
PID-handle guarantee across force replacement. Object/optimized IR, final
ELF and full link checks pass; configuration, complete exports/CRCs and
certificate match r44. Paired readback, unchanged ramdisk and two complete
normal boots pass with retained security. r44 is the immediate rollback.

No unknown global queue is force-replaced and no live PID/address-reuse
sequence is induced. Loaded system_server JNI/client copies are verified
against their exact device digests. Their examined init/get sites use group
commands 17/18, not global 44/45/46; this scoped result does not establish
all indirect callers or global inactivity. Positive global lifecycle,
request behavior across force replacement, timer expiry and complete frame
scheduling remain open. Original backups, r17 and the latest r41/r42/r43/
r44/r45 pairs remain; consumed task-owned artifacts are removed.
