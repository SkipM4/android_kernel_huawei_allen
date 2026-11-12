# Bring-up Completion Plan

## Acceptance Rule

User requests stepwise repair and validation until all required work is
complete. Baseline is installed r48 from bringup/clang12 at 5dffb0337; immediate
paired rollback is r47, with r46 also retained. Each kernel iteration stays
isolated, with matching boot/modem_driver validation, unchanged independent
ramdisk unless separately required, retained security controls and accurate
functional scope.

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
| 04 | Frame RTG | Audit configuration/task/Binder/module ABI; restore real interface and validate intended caller behavior | r48 Installed; Mutex/Native Clear/Boost Expiry Pass; Direct END_FREQ on r48/Renewal/Global Open |
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

r46 addresses a concrete boost-restart race: an old queued timeout can clear
a newly armed boost, and lock-external activation can be undone by cleanup
before renewal. Activation now uses the existing timer mutex; the worker
consumes its notification before checking for a pending renewed timer, with
no late bit-set to overwrite a concurrent expiry. The existing argument,
duration/util and permission contracts remain. Object/optimized IR and final
ELF checks, full build, identical configuration/complete export CRCs/signing
materials, matching signed pair/readback and two normal boots pass. Ramdisk
and security controls remain unchanged; r45 is the immediate rollback.

The source timer entry is reachable only through proc boost 0xc014ab64 in the
examined tree, not the ordinary frame min-util commands. Scoped verified
loaded-library sites/immediate inspection does not locate an intended
permitted production boost caller or prove global inactivity. No global
boost or queue takeover is injected to fill this gap. Intended-caller expiry/
restart stress, positive global lifecycle and full frame scheduling remain
open, alongside the existing manual/hardware/source prerequisites. Original
backups, r17 and the latest r42/r43/r44/r45/r46 pairs remain; r41 metadata is
retained for frame-disabled comparison, while its superseded pair and
consumed task-owned artifacts are removed. Whole-device acceptance is open.

The unchanged-r46 follow-up verifies real stock native frame use without a
kernel rebuild, flash, test APK, permission change or queue takeover. An
independent trace instance records ten begin/end timestamp pairs and actual
SystemUI/RenderThread scheduling. A targeted instance-stack window confirms
the native frame ioctl path, and existing raw_syscalls events identify the
real controller's successful group allocation and the application's successful
BEGIN/END_FRAME and END_FREQ calls. The latter changes the group-enable
counter to zero before returning 0, followed by successful controller clear.
The observed allocation ID is not made into a platform constant.

Actual internal priorities include 87 outside the source's VIP band and 89
inside it. One cycle retains priority 89 after its frame-disable counter;
the later proc display can translate VIP/PI priority back to the raw policy,
so that checkpoint cannot close effective-priority restoration. The subsequent
r47 audit attributes and fixes a frame state/member-clear race rather than
blindly forcing normal priority. This does not reopen the passed frame ioctl
results or prove that the global boost timer ran.

All capture buffers are loss-free, full interval kernel logs contain no
warning/fatal markers, and boot/security state stays unchanged. The display,
keyguard, radio settings, brightness mode and existing global tracer/instances
are preserved; automatic brightness's naturally updated value is not
overwritten. Copied binaries/DEX, complete traces/logs and temporary scripts
are removed. Real permitted boost expiry/renewal, positive global lifecycle,
complete frame/power acceptance and external prerequisites remain open.

r47 reproduces the missing restoration on unchanged r46 with actual native
calls and separately filtered priority stacks. The controller assigns frame
VIP level 10; END_FREQ writes the disabled flag outside the member lock, and
a concurrent controller clear then skips resetting members because it sees
that flag. END_FREQ acquires the lock after their pointers are gone and
returns 0 while both threads remain at internal priority 89. This is an
observed frame cleanup race, not the separate short lock donations also seen.

The existing frame-info writer lock now covers the state check/change and
per-member restoration. The member-update counter reset also uses that lock.
No new global serialization, task layout/reference, ioctl ABI, permissions or
priority policy is added. The #47 full build, final ELF ordering, identical
configuration/complete export CRCs/signing material, matching modules and
signed paired image checks pass. Paired flash reaches normal Android;
boot/modem readbacks match, independent ramdisk is unchanged, SELinux remains
Enforcing, module signature enforcement/CFI/PRMEM remain and taint is 4.

Two bounded real wake/sleep cycles record 1,952 paired native ioctls, including
28 RTG requests returning 0 and two END_FREQ calls. Both calls perform the
UI/RenderThread NORMAL policy changes before returning; the first captures
the overlapping controller VIP update and member clear in the corrected
order. Independent mutex/rwsem votes still request and clear successfully.
Main/stack trace buffers are loss-free and full-window logs have no actual
kernel WARN, BUG, Oops, panic or CFI failure. No redundant second startup or
repeat of passed frame tests is used. Display/keyguard/settings/tracer state
are restored and task-owned diagnostics are removed after recording results.

At the r47 checkpoint, r46 is the immediate rollback and r43-r47 pairs remain;
the current r48 retention state is recorded below. Expired r42 images are
removed while their useful metadata remains. The independent mutex audit
confirms a separate cancellation defect on unchanged r47. A bounded root-owned
probe uses only a fresh, uncreated UHID file, private tasks and a private
anonymous page; it creates no HID device or input event and changes no system thread. With
an active VIP waiter, the owner changes 120 -> 89 -> 120 on normal release.
After the last waiter is cancelled and returns EINTR, the owner's inert write
returns EOPNOTSUPP after unlocking, but its priority remains 89 and it runs
at that priority. Its normal VIP setting remains zero. The control has a
successful type-2 request/clear pair; cancellation has a successful request
with no clear. Stacks identify the mutex/UHID route and the trace buffers are
loss-free. No actual kernel WARN/fatal record or AVC denial occurs in the
complete successful window; boot/security/display/tracer state is unchanged
and all private tasks/files are released.

The active generic VIP interface explicitly does not support nested requests
of one type, so a flag-only change or unconditional clear cannot preserve
donations from another held mutex. The user approves per-waiter contribution
identity/priority with owner-level aggregation and necessary ABI review.
OEM/vendor/KABI reserve macros allocate no storage; r48 consequently changes
waiter layout and the task thread offset, though mutex layout and overall task
size remain unchanged. Initialization/fork, ownership lifetime, lock order and
20 original generated assembly units receive scoped review, not blanket opaque
assembly certification. The full #48 build keeps the configuration/security
and signing material; 8,948 export CRCs change, including module_layout. New
matching modules and signed paired images pass; r47 modules must not be mixed
with r48. Paired installation/readback and unchanged independent ramdisk pass.

r48 withdraws cancellation before last-waiter flags permit fast unlock,
withdraws only the releasing mutex's donations before ownership transfer and
recomputes the remaining per-level maximum without clearing other lock votes.
Existing normal VIP/other types remain and uncontended fast paths are kept.
Nine private runtime scenarios and 36 actual scheduled-in checkpoints pass:
single cancellation, different-lock release/cancellation at levels 10/3,
different levels on one lock, retries, real handoff, normal VIP preservation,
rwsem coexistence and higher real RT/policy reset. The owner returns to 120
before unlock after its last donor cancels; a remaining low donor keeps 96.
The genuine signed out-of-tree probe is unloaded and removed, and its necessary
cleanup reboot restores taint 4100 -> 4 on #48 without security changes.

Two bounded native lock-screen windows record 11 successful frame timestamp
pairs. Controller cleanup restores UI/RenderThread from 89 to ordinary policy
inside each clear ioctl and before returning; actual later samples are UI
110/120 and RenderThread 116. The narrower window captures two production
GPU-to-RenderThread mutex request/withdrawal pairs through the new code at
110 -> 89 -> 110. All native buffers are loss-free, including 1,039 fully paired
selected ioctls in that window. No actual kernel fault occurs in scoped logs.
Two private-window daily_control/logserver capability AVCs are unrelated;
native windows have no AVC denial. Display/settings/tracers and enforcing
SELinux/signatures/CFI/PRMEM remain, with no system-priority or global-queue write.

The controller clears first in both windows; a late END_FREQ returns -100
because membership is already gone. Direct END_FREQ=0 is not re-observed on
r48 and remains a frame acceptance boundary, not a mutex test failure or a
relabelled r47 pass. Native PI/legacy shared-vote guarantees, opaque assembly
and 32-bit production integration are not certified wholesale. An earlier
full UAPI export also exposes pre-existing OEM CONFIG leaks; diagnostic header
selection does not relax export checks or fix that separate portability issue.

r47 is the immediate paired rollback, with r46, accepted r17 and original
backups retained. Latest r44/r45/r46/r47/r48 pairs remain; superseded r43 images/
module and consumed task-owned probes, logs/traces and build diagnostics are
removed after recording useful metadata. Workspace remains below 20 GB.

Permitted boost expiry/renewal, positive global lifecycle, complete frame/power
acceptance and manual/hardware/source prerequisites stay open at that checkpoint.

The unchanged-r48 boost follow-up identifies the actual permitted production
caller: vendor.huawei.hardware.iawareperf@1.0-service in u:r:uniperf:s0, with
its own /proc/<pid>/rtg descriptor and loaded libperfgenius_wrapper.so.
HisiPerfAdapter::SetRtgSchedBoost constructs 0xc014ab64; the installed MLS
policy grants uniperf self:file ioctl 0xab64. This complements, rather than
invalidates, the earlier scoped SystemUI/system_server library inspection.
No permission change or direct global request is needed.

The hardware-product uniperf configuration matches the runtime cached copy.
RTG window-switch actions specify 650 ms/util 684 and click actions 250 ms/
util 600; screen_on has no RTG action. A bounded ordinary blank-lockscreen
tap window records one complete real boost ioctl returning 0, default-group
floor 600 and a 63-jiffy timer at running HZ=250. Instance stacks attribute
activation to start_boost_timer, expiry to on_timer_timeout in the timer
softirq and floor/group cleanup to frame_thread_func. The observed callback
arrives about 273 ms after arming; this is not an exact-latency guarantee.
Both trace buffers are loss-free, full-window logs have no actual kernel
fault or AVC denial, and boot/security/settings/keyguard/display plus the
original main/wifi tracers are preserved. No rebuild or flash is performed.

Only one global request is observed. Loaded framework code limits click
scene reset to at least 2 seconds and traversal/jitter reporting to 250 ms
intervals, with the current click window/count set to 1000 ms/three reports.
This checkpoint does not attribute the click-policy consumer or prove that
another tap must renew the timer. Arbitrary repeated input is not used to
force overlap. Intended-caller single activation/expiry passes; renewal/
stale-timeout restart stress, multi-group/AUX/configured boost-thread coverage
and positive global queue lifecycle remain open.

The subsequent read-only audit locates the consumer in com.huawei.iaware's
loaded HwIAware OAT/VDEX. IAwareSdk event 3033 reaches IAwareSdkService,
AppInnerBoostARSPlugin and AppInnerBoostDmePlugin (2019), then UniPerf. Click
traversal emits 4400; click jitter emits 4401 for at least three skipped
frames. This is installed-code routing, not another runtime request capture.
The APK lacking classes.dex does not mean its optimized implementation is
absent.

Five existing CMS getConfig replies are decoded using the inspected actual
serializers. Click remains 1000 ms/3 reports, slide 5000 ms/16 reports, and
both feature switches are enabled. The application-type table adds only
slide-count limits (8/18:10, 36:3), not click deduplication; intelligent scene
types also select the slide branch. This does not attribute the previously
unobserved second click request to that table or native SDK flow state.

Same-channel reporting has a 250 ms minimum interval, equal to the nominal
click boost duration; separate traversal/jitter channels can theoretically
overlap. Neither ensures a safe blank-lockscreen action generates a second
request before the old expiry. No input, unlock, forced jitter, configuration
write, trace instance, rebuild or flash occurs in this audit. A bounded
renewal test needs an explicitly allowed ordinary foreground scene and
complete real request/timer/floor evidence across overlap and final cleanup.
Renewal and stale-notification/restart stress remain unverified, not failed
or passed. Existing direct END_FREQ on r48, full frame/power and manual/
hardware/source boundaries remain open. Boot/security/settings/display,
matching r48 and rollback pairs are unchanged; task-owned diagnostics are
removed after durable recording and the workspace stays below 20 GB.
