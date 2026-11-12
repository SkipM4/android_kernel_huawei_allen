# Allen Kernel Bring-Up

Build the official ALN HarmonyOS 4.2 Linux 5.10.43 sources on x86_64 Linux
before attempting newer kernels. Project safety rules are in `AGENTS.md`.

## Current Status

Clang 12.0.7 builds a signed, ABI-matched boot/modem_driver pair with enforcing
CFI. r16 reaches the Huawei home screen on two boots, but fails after about an
hour when storaged exercises a hyperframe direct-I/O hook. r17 supplies typed
CFI entries for the audited hyperframe callbacks, reaches the home screen on
two boots and passes actual direct-I/O read/write checks. Its second boot runs
continuously for over 65 minutes without panic or restart, beyond r16's known
failure window. r17 is the first accepted self-built normal-boot baseline and
is retained as the tested rollback pair. This establishes the boot goal, not
complete OEM feature parity or production stability. See `docs/clang12-working-baseline.md` for
reproduction, the accepted image pair, rollback and remaining limitations.

Use the stock-derived compatibility build in `docs/stock-module-compat.md`
for paired device tests, not the initial released-configuration build below.
See `docs/clang12-cfi-iterations.md` for each attributed failure and recovery.

Functional follow-up reaches r20: static sensorhub route-lock initialization
and ION reuse eligibility repairs pass paired flash and targeted sensor/ION,
storage, USB and display sleep/wake checks. r20 repairs video decoder IRQ-flags
writeback and passes named hardware AVC/HEVC decode and live accel/gyro/mag
event tests. r20 is the recorded hardware-functional reference, not full certification.
The user reports preliminary front/rear camera switching and photos working
on r19. Multi-level L3/DDR voting restoration passes independent native
selector/parser checks, #41 full-link/pair checks and bounded real-vote
validation. The installed device is r48. A separate normalized vote-scale
reference audit classifies it as unused original bookkeeping within the
examined scope; no dummy state is restored or performance parity claimed.
See `docs/clang12-mips-multilevel-audit.md`.
See `docs/clang12-functional-tests.md` for results and the next isolated fix.

The current installed test candidate is r48: r21 retains full early logs,
r22 initializes the TCP recovery rule lock, and r23 uses sleeping GPIO reads
in the satellite init thread. r24 removes invalid PD sysfs executable bits
without changing its effective creation mode or store callback. r25 keeps ION
as the sole owner of its existing proc interfaces, removing duplicate memcheck
registration without changing their ABI. r26 removes duplicate version
declarations from combined TCPC/NPU modules, keeping the active version values.
Paired readback, real ION allocation/mapping/accounting and registration checks
pass. r27 reserves the PRMEM physical database during the existing early
memblock stage without delaying write protection or changing its selected
physical range. It boots normally twice with no WARNING in either full early
log; taint falls from 516 to 4 (the recorded CPU-feature S bit remains).
Database capacity/accounting, Reserved page ownership and protected SELinux
pools pass targeted checks, not adversarial protection certification.
See `docs/clang12-early-diagnostics.md` for attribution and
test limits. Accepted r17 and the latest r44/r45/r46/r47/r48 pairs are retained;
r47 is the immediate r48 rollback, with r46 also preserved. Original
boot/ramdisk/modem backups remain intact.
r28 imports the audited Linux 5.10.43 Hepburn/Charlotte CLT codec, absent from
the ALN archive, and maps its six pages within the existing Allen DT resources.
Local PCM/DSP, DT, firmware, calibration and independent Magisk ramdisk remain
unchanged. The normal codec-backed sound card and audio policy manager now
register on two normal boots. Silent 48 kHz stereo playback consumes all
96,000 frames with zero underruns and a built-in speaker route. A follow-up
observes the real PCM running, then closing, balanced playback power events
and codec autosuspend after clock disable. No kernel WARNING, BUG, CFI failure
or panic is found in the captured full boot/test logs; taint remains 4.
The user also confirms normal audible playback in the system sound settings
on r28; this is a basic user-observed playback pass, not fidelity certification.
A 30-second screen-off/wake check also passes silent playback and codec runtime
release without device/audio-service restart. USB/charging wakeup sources stay
active and system-suspend counters do not advance while the cable is attached.
The subsequent unplugged check confirms one 71-second deep suspend and codec
power-down/restore without reboot, but UFS resume emits an unbalanced hub IRQ
enable warning and taint rises to 516. r29 supplies the missing initial
is_irq_enabled state after successful hub IRQ registration. Its paired boot,
readback and actual O_DIRECT write/read/compare checks pass with UFS IRQ
activity, no kernel WARNING and taint 4. The first unplugged recheck and two
later deep resumes no longer emit the UFS warning; taint stays 4. Post-resume
silent playback, PCM/clock release and actual O_DIRECT write/read/compare
also pass without device/audio-service restart. Two eventpoll wakeups abort
freezing before device suspend, not a UFS/codec resume failure. RTC wakeups
expose a separate NO_SUSPEND/wake-IRQ contract conflict; see
`docs/clang12-functional-tests.md` for the tested scope and next isolated step.
r30 applies a SoC-only RTC IRQ flag correction after auditing the handler's
MMIO lifetime and workqueue notification path. It preserves the PMU RTC,
alarm programming and wake-enable pair. Compilation, signed paired flash,
readback and read-only RTC time progression pass; wakeup remains enabled,
boot logs have no kernel WARNING and taint is 4. The unplugged recheck completes
four deep suspends with approximately 226 seconds of cumulative sleep and no
suspend/resume failure. Two wakes are attributed to the SoC RTC, with its
ordinary handler executed and no misconfigured-IRQ diagnostic. UFS and idle
codec callbacks recover normally; post-resume silent playback consumes all
96,000 target frames without underruns, then the PCM closes and codec clocks
disable with runtime autosuspend. Device and audio-service identities remain
unchanged; full logs have no kernel WARNING and taint stays 4. r29 remains the
immediate paired rollback. This validates short idle RTC/deep wake, not all
alarm delivery cases or long-duration power parity.
An r30 follow-up also passes a short MIC-source capture: the correctly
attributed shell client receives 96,000 mono 48 kHz samples from the bottom
built-in microphone, with nonzero PCM and no client silencing. The unchanged
PCM/DSP path captures six S16_LE channels at 48 kHz underneath the mono HAL
stream. Capture stops and closes, codec clocks disable and runtime autosuspend
returns without restarting the device or audio services. For the probe, only
aggregate statistics are retained; no audio file is saved and no permission
or privacy setting is changed. A targeted follow-up obtains advancing
application capture timestamps after startup without regression. HAL-side
timestamp statistics remain unavailable. A read-only boundary audit identifies an unimplemented
get_capture_position callback in the unchanged stock primary HAL's normal
input initialization; the loaded HIDL layer returns NOT_SUPPORTED without an
ALSA timestamp query. No kernel timestamp workaround is justified by that
result. The user subsequently confirms normal recording in the system
recorder on r30. This adds a basic user-observed recording pass; no recorded
content is requested or transferred. These checks are not microphone
fidelity/channel or hardware timestamp certification.
The later r30 runtime audit finds recurring battery OCV/capacity lookup
failures, thermal-service fallback, PCIe PME/AER MSI affinity failures and a
missing per-process RTG interface. r31 fixes the battery consumer's optional
feature contract: the original and live DT both disable battery_ocv and
provide no OCV table, so BCI now skips only its OCV-based BASP corrections
when that provider is disabled. Existing gauge capacity and low-voltage
thresholds remain; enabled-provider lookup errors are not hidden. Compilation,
signed paired flash/readback and unchanged independent ramdisk checks pass.
The full early/observation log has ongoing battery work without the two lookup
errors or kernel WARNING, BUG, CFI failure or panic; taint remains 4 and battery
properties update. This does not certify low-battery behavior, capacity
accuracy or charging modes. PCIe affinity and inherited audio MUX diagnostics
remain; the RTG gap is subsequently audited separately. A later read-only
thermal audit locates the
shelltemp messages at the stock daemon's calculated-result bound check,
not a demonstrated physical-sensor read failure. Default coefficients can
trigger it with valid inputs; live-history attribution and fallback freshness
remain unresolved. No thermal protection or coefficients change. See
`docs/clang12-thermal-source-audit.md` and the r31/remaining runtime sections
of `docs/clang12-functional-tests.md` for scope and next steps.
The r41 publication follow-up confirms userspace-owned millidegree storage
and named slot smoothing, not a kernel unit conversion. A bounded normal-screen
warming/cooling check then observes both front/back publication and cached
fallback values update; the cache is not permanently frozen in that window.
High-temperature throttling and charging safety remain open. A separate
read-only calibration audit finds both matching NV
records all zero with no valid CRC; the inherited clear-on-sync-failure write
gate makes permission/credential bypass unsafe. No calibration write or radio
configuration change occurs; optional usage and authorized lifecycle remain
open. See `docs/clang12-calibration-access-audit.md`.
A separate PCIe audit traces IRQ267's -22 to the DesignWare MSI affinity
setter, which is also unimplemented in upstream Linux 5.10.43. Logical MSI
vectors use a separately dispatched physical interrupt; no fake affinity
success or CPU-hotplug workaround is added. An r41 topology/activation check
then observes 122 fresh deliveries on the endpoint's separate GIC INTx IRQ82
while root-port IRQ267 affinity refusals recur. Root PME/AER delivery and
migration remain untested; see `docs/clang12-pcie-affinity-audit.md`.
r32 fixes the separate raw-battery-temperature interface: status is returned
independently of signed millidegree data, the missing aggregate callback uses
the existing BATT_0 measurement path, and thermal callbacks propagate errors.
The DT disables the second battery's capacity contribution; its unregistered
raw-temperature provider now returns ENODEV instead of a successful zero.
Compilation, signed paired flash/readback and unchanged ramdisk checks pass.
Six passive reads of bat_raw_temp and bat0_raw_temp agree with the existing
battery readings; full early/observation logs have no kernel WARNING, BUG,
CFI failure or panic, and taint stays 4. Sensor resolution, acquisition defaults,
temperature compensation and charging policy remain unchanged. This fixes
the interface contract, not the stock shell calculation guard or full thermal
accuracy/safety; see `docs/clang12-functional-tests.md` for the r32 scope.
A read-only RTG follow-up identifies HW_RTG_SCHED as the missing proc gate,
not SCHED_CGROUP_RTG. Its FRAME_RTG dependency is lost because PERF_CTRL is
not connected to the published build graph and its driver source is absent.
Sampled stock 64/32-bit ioctl commands and payloads match the local handler;
this is not full runtime ABI certification. The known Hepburn reference also
lacks that driver. No placeholder interface, scheduler bulk-enable or flash
was introduced in that audit; r32 remained installed. See `docs/clang12-rtg-source-audit.md`
for the source gap and isolated restoration boundary.
A subsequent source-only step restores the performance-control dispatcher
from a pinned Kirin 9000 Linux 5.10.43 release, adapting command slots to
Allen's existing handlers and fixing a relocated header include. Its objects
compile, but the enabled kernel build exposes a second missing provider,
thermal_perf_ctrl.c. The old thermal candidate omits Allen's GPU sensor and
turns some lookup failures into successful zero values, so it is not imported.
At that source-only stage PERF_CTRL and frame scheduling stayed disabled,
and r32 remained installed pending completion of the thermal query contract.
A follow-up now supplies the missing thermal queries using Allen's existing
DT-configured IPA sensor range and system_h provider, without changing thermal
policy or units. Failed cooling/sensor queries propagate errors rather than
successful zero values. A complete PERF_CTRL-enabled build succeeds; frame
RTG, statistics-layout and DDR-provider options stay disabled. The dispatcher
also corrects the standard four-byte compat capability-word query and rejects
the absent HWP backend instead of its no-op fallback.
r36 now boots normally twice with its signed matching modem module. Root
native/compat capability, IPA and both cooling-zone queries pass; invalid
zones return EINVAL and all buffer canaries remain intact. The node keeps
system:system ownership, mode 0660 and hisi_perf_ctrl_device labeling;
ordinary shell access is denied. These are bounded transport checks, not
certification of every system caller, scheduling command or thermal accuracy.
Frame RTG and /proc/<pid>/rtg remain unavailable. See
`docs/clang12-rtg-source-audit.md` for the build/ABI and runtime scope.
A full early-boot comparison with r32 reproduces vendor_init SIGSEGV during
the stock sys_wp_init_action, following a UFS path-open failure. Init restarts
the subcontext and Android completes boot, but that action remains unresolved.
No kernel WARNING, BUG, CFI failure, Oops or panic appears in the compared
boots. RTNL duration diagnostics and an unconditional wiphy allocation stack
dump are not kernel crash evidence. A read-only follow-up identifies the
missing path as /proc/sys_wp_soft and the crash path as an internal main-init
property setter used from vendor_init, whose property-info mapping is NULL.
Both success/failure branches use that setter, so adding a dummy proc node
would not repair the context mismatch. The retained original kernel and
stock configuration also lack the old software-WP interface; a 4.14 reference
requires real BIO enforcement, not just a proc file. No write-protection
bypass, userspace binary patch or flash is introduced. See
`docs/clang12-system-wp-source-audit.md` and `docs/clang12-functional-tests.md`
for the separate source/contract requirements and remaining limits.
r37 corrects RTNL hold-time accounting after successful killable, trylock and
reference-count acquisitions, using the standard monotonic clock throughout.
The register_netdev killable path previously reused an older timestamp.
Lock/return semantics, deferred work and the existing 300-ms diagnostic stay
unchanged. A fresh r36 time-zero baseline has 24 duration reports; r37 has
none during startup or bounded link/IPv4/IPv6 route enumeration. Full build,
paired readback and security checks pass. This repairs diagnostic accounting,
not measured network performance; no forced long RTNL hold is injected. See
`docs/clang12-early-diagnostics.md` and `docs/clang12-functional-tests.md`.
r41 restores L3/DDR multi-level tables, independent ratios and maximum MHz
votes from the original Allen contract, without changing the filtered counter
API or DT. All six maps and actual compound userspace ratios pass device
checks. Short streaming/cache/mixed workloads produce 26,078 selections that
match original machine code, including genuine second-level maxima, and
4,836 post-writel vote events. Paired readback, normal startup/reboot and
Enforcing/CFI/signature gates pass; taint remains 4. Frame-enabled #39 remains
unflashed. These are requested-vote checks, not effective-clock, power or
complete OEM scheduler certification. See `docs/clang12-mips-multilevel-audit.md`.
r42 enables the five audited original frame features after the approved
render ownership and lifecycle repairs. Applications can manage only their
own process; actual UID1000 retains controller scope. Rejected initialization
preserves the active group, uses the correct group buffer and retains task
identity through RCU publication. Proc reset drains active ioctls and timer
cleanup. Fresh link, matching signed modem and paired readback pass; shifted
task member offsets prohibit mixing an older frame-disabled modem.
Real 64/32-bit untrusted_app_27 callers pass capability canaries, self-group
operations, same-UID cross-process rejection, force-init protection and exit
cleanup. A real-UID1000 Magisk-domain controller passes test-child control and
three disable/re-enable cycles without replacing the persistent worker.
Two normal boots retain Enforcing, taint 4, PRMEM and forced module signing,
with no WARN/BUG/CFI/panic; the inherited wiphy dump_stack remains classified.
The framework re-enables RTG after reboot, and a bounded counter/vote telemetry
regression passes without changing monitor settings. This is not global
wake-queue, timer-expiry stress, full frame scheduling or performance parity
certification. See `docs/clang12-rtg-source-audit.md` and
`docs/clang12-completion-plan.md` for remaining gates and manual prerequisites.
r43 fixes independent PERF_CTRL global initialization error transport: a bool
local variable converted negative errno into positive 1. Real native/compat
checks reproduce this on r42 and verify EFAULT/ENODEV after the int repair.
Configuration, full export/CRC table and certificate remain identical to r42;
matched signed pairing/readback and normal startup/reboot pass with unchanged
ramdisk and security controls. No valid global queue is activated by these
negative probes. Global ownership/resume/depth/exit semantics and complete
frame/timer integration remain separate work, not covered by this error fix.
r44 retires all old global wake queues before force-init changes their depth.
The original Allen machine code confirms an inherited 4 -> 1 -> 4 lifetime
hazard: hidden entries can miss task-free notification and later dereference
a freed task. The existing lock now covers membership retirement and logical
count clearing before the new depth is published; non-force pause/resume and
caller policy are unchanged. Full link, unchanged configuration/export CRCs/
certificate, matched signed pair/readback and two normal boots pass. This is
not a deliberately triggered old-kernel UAF or positive global lifecycle test;
unknown system global state is not force-replaced. See the RTG audit for the
remaining caller, root-retirement identity and runtime acceptance gates.
r45 requires non-force global resume/get to match the lookup-pinned task to
the original unretired root entry, rejecting numeric PID and freed-address
reuse without reviving or rotating old queues. Wake/free hooks select the
root by task pointer. Same-object pause/resume, force-init, ABI and caller
policy remain unchanged. Full link, identical configuration/export CRCs/
certificate, signed paired readback and two normal boots pass with unchanged
ramdisk/security. This is installed/static identity-fix evidence, not a live
PID-reuse or positive global lifecycle test. Scoped loaded JNI/client sites
still use group commands 17/18 and do not prove global inactivity. Matching
callers, positive global lifecycle and full timer/frame acceptance remain open.
r46 serializes boost activation with timer renewal/cleanup under the existing
mutex. The worker consumes its timeout notification before checking for a
renewed pending timer, preventing stale cleanup and retaining a concurrent
expiry rather than overwriting its wake bit. Duration/util policy, ioctl ABI
and caller permissions are unchanged. Full link/final ELF, identical complete
export CRCs/configuration/certificate, signed paired readback and two normal
boots pass with unchanged ramdisk/security. No production global boost is
injected: intended-caller expiry/restart stress and full frame scheduling
remain open, not replaced by static or startup checks.
A follow-up on unchanged r46 verifies real stock native frame calls through
existing RTG, raw-syscall and instance-stack events. The system controller
allocates a frame group; SystemUI begin/end requests and END_FREQ return
success, the latter turns group scheduling off, and the controller clears
members afterward. At that checkpoint post-stop VIP/PI ownership is still
unattributed; the OEM proc priority display alone cannot prove effective
priority restoration. That follow-up changes no kernel/policy and performs
no build or flash; it is bounded native lifecycle coverage, not full acceptance.
r47 reproduces the post-stop priority-89 failure as a frame shutdown/member
clear race, not an independent PI donation. It moves the existing frame-info
writer lock ahead of the state transition so member removal cannot skip
restoration, and protects the member-update counter reset with that same lock.
Full build/final ELF, unchanged configuration/export CRCs/signing material,
paired readback and normal startup pass. Two real END_FREQ calls return 0
with both member-policy restorations completed before return; a concurrent
controller clear follows restoration. Separate observed mutex/rwsem VIP
donations still request and clear normally. This closes the reproduced frame
stop race in the bounded native test, not all VIP/PI or frame-power behavior.
Permitted boost expiry/renewal, global-queue lifecycle and full performance/
power acceptance remain open; no unknown queue or global boost is injected.
See `docs/clang12-rtg-source-audit.md` for the measured scope and next audit.
The separate r47 mutex audit reproduces a cancellation cleanup defect:
a private lock's normal release restores priority 120, but after its last
VIP waiter returns EINTR the owner unlocks and still runs at priority 89.
Loss-free stacks attribute the remaining type-2 vote to that mutex, not RTG.
r48 implements the user-approved waiter-level donation identity and owner
per-level aggregation. Cancellation withdraws before last-waiter flag removal;
unlock/handoff withdraw only that lock's contributions while retaining owner
lifetime. Other locks, normal VIP and independent VIP types keep their votes;
uncontended fast paths remain. Nine private scenarios and 36 actual scheduled-in
checks pass, including cancellation, differing levels, handoff, ordinary VIP,
rwsem coexistence and a higher real RT/policy-reset case. This closes the
reproduced mutex leak within that tested scope, not all native PI/legacy votes.
The full #48 build and matching signed boot/modem pair pass installation and
readback. Mutex layout is unchanged, but waiter layout and the task thread
offset change; 8,948 export CRCs change, including module_layout. Old modules
must not be mixed with r48. The scoped review of 20 original generated assembly
units is not blanket opaque-assembly ABI certification.
Two real native frame windows record 11 successful begin/end timestamp pairs;
controller cleanup restores both members from VIP 89 before returning, with
ordinary scheduled-in priorities afterward. Two production GPU-to-RenderThread
mutex donations also request/withdraw successfully through the new code.
The controller clears first, so a later END_FREQ returns FRAME_ERR_PID (-100);
direct END_FREQ=0 is not re-observed on r48 and the r47 result is not relabelled.
The genuine temporary out-of-tree probe is unloaded and a cleanup reboot
restores taint 4. SELinux Enforcing, forced signatures, CFI, PRMEM and independent
ramdisk remain. No actual kernel WARN/fatal record occurs in the scoped logs;
two unrelated logserver capability AVCs are classified, not called a zero-AVC
private window. Boost expiry/renewal, positive global lifecycle and full frame/
performance/power acceptance remain open. See `docs/clang12-rtg-source-audit.md`.
Two source-inherited unconnected MUX diagnostics remain. Audible fidelity,
individual microphone mapping, hardware capture timing, calls/sidetone and
active-audio/long-duration deep suspend are not certified; see
`docs/clang12-audio-source-audit.md` and `docs/clang12-functional-tests.md`.
A correctly attributed shell-context probe passes Bluetooth ON/OFF on r24;
named hardware AVC/HEVC encoders each complete thirty synthetic frames and
EOS, with VENC IRQ activity. Pairing/peer transfer, video decode fidelity,
camera recording and full audio functionality remain outside these checks; see
`docs/clang12-functional-tests.md` for the exact scope.
Wi-Fi foreground scanning passes on r26 after explicitly waking the device
and foregrounding native settings; scanner results and successful driver
completion agree. Its existing saved network automatically associates and
obtains DHCP configuration. The rejected shell scan probe is not a hardware
failure; explicit external connectivity and sustained transfer remain untested.
Calibration-partition open errors (-EACCES) coexist with these successes and
need separate lifecycle diagnosis without relaxing protection.

## Layout

- `Code_Opensource/kernel/`: official sources with targeted bring-up fixes.
- `configs/allen-stock-5.10.43.config`: configuration read from the running phone.
- `tools/toolchains/clang-r416183b1/`: local Android Clang 12.0.7 toolchain.
- `tools/host/bin/bc`: local build dependency, extracted from the Arch package.
- `out/allen-clang12/`: independent kernel output directory; not tracked.
- `out/allen-clang12-stock-compat/`: stock-derived paired-test output; not tracked.
- `images/original/`: firmware device-tree backup; not tracked.
- `docs/bringup-baseline.md`: device baseline, toolchain provenance and risks.
- `docs/stock-module-compat.md`: installed modem ABI and signing compatibility.
- `docs/clang12-r2-flash-test.md`: r2 CFI boot failure, verified rollback and next fix.
- `docs/clang12-cfi-iterations.md`: targeted CFI fixes and subsequent paired tests.
- `docs/clang12-working-baseline.md`: accepted r17 boot baseline and test scope.
- `docs/clang12-functional-tests.md`: functional repairs, checks and audio/BT gaps.
- `docs/clang12-early-diagnostics.md`: r21-r27 log, lock, GPIO, PD, registration and PRMEM fixes.
- `docs/clang12-audio-source-audit.md`: codec source audit and scoped r28 CLT integration.

The original source archive, `boot.img` and `ramdisk-magisk.img` remain at the
project root and are excluded from Git. Never overwrite them.

## Build

From `/home/data/Allen`:

```sh
scripts/build-allen.sh merge_charlotte_defconfig
scripts/build-allen.sh
```

The target and compiler revision come from the official README and Makefile.
The wrapper uses LLVM tools and the integrated assembler, not the host GCC for
ARM64. `JOBS` overrides the default eight build jobs; `OUTPUT_DIR` selects an
independent output directory. `LOCALVERSION=` keeps the
stock `5.10.43` release string instead of adding a Git suffix; this alone does
not establish module ABI compatibility.
ThinLTO links use two backend jobs and a bounded cache to avoid the observed
Full LTO memory failure and excessive cache growth.

Additional make targets can be passed directly, for example:

```sh
scripts/build-allen.sh Image.lz4
```

Full output goes to `.tmp/build-clang12.log`. Inspect only the relevant error
context, record durable conclusions, then delete the log when no longer needed.
The release configuration selects LZ4, so the default kernel output is
`out/allen-clang12/arch/arm64/boot/Image.lz4`. The README's generic `Image.gz`
example does not describe the actual default selected by this configuration.

## Device And Images

The verified device serial is `FMR0223C20002621`. Always pass `-s` to ADB and
fastboot. The user confirms `HWALN` is the device's abbreviation for HW Allen.

The device has separate `boot` and `ramdisk` partitions. The original boot image
uses Android header version 2 and includes a small firmware ramdisk stub; the
Magisk image uses header version 0 and belongs to the separate ramdisk partition.
Preserve the stock boot structure, stub and device tree when replacing its
kernel. Do not merge the Magisk ramdisk into boot.

Configuration success or compilation success is not a boot or module-ABI test.
Do not flash until the candidate image, target partition, recovery path and
fastboot identity have been verified according to `AGENTS.md`.

The initial released-config kernel does not satisfy the installed modem module's
symbol CRCs or CFI imports. See `docs/stock-module-compat.md` for the separate
stock-derived configuration, verified OEM public certificate and ABI checker.

The ThinLTO/CFI `r2` candidate is a matching boot/modem_driver pair, not a
boot-only replacement. Both test images and their rollback requirements are
recorded in that document. The paired flash test reaches a MAS block-completion
CFI panic before Android starts; both originals are restored and normal boot is
verified. See `docs/clang12-r2-flash-test.md`. Do not reuse r2 as a working kernel.
Subsequent fixes reach normal Android; r16's later direct-I/O CFI failure is
repaired in the accepted r17 baseline. Originals are restored after failed
tests; functional candidates later replace the installed r17 while its pair
is retained as a rollback baseline. See
`docs/clang12-cfi-iterations.md` for the complete failure and recovery history.

For future versions, create a new boot candidate from the uncompressed Image:

```sh
scripts/repack-allen-boot.sh out/allen-clang12-stock-compat/arch/arm64/boot/Image \
  images/test/allen-boot-test-5.10.43-clang12-r27.img
```

The repacker refuses to overwrite an existing image and validates the actual
packed components. A corresponding ABI-matched, signed modem-driver filesystem
image is still required; a successful repack does not establish successful boot.

Stage that matching module partition from the same kernel output directory:

```sh
scripts/stage-allen-modem.sh out/allen-clang12-stock-compat \
  images/test/allen-modem_driver-test-5.10.43-clang12-r27.img
```

Use a new versioned path for each iteration. This produces the companion signed
`.ko` and an ext4 image cloned from the verified original partition, checking
ABI imports, the embedded signing certificate, CMS signature, filesystem
integrity, ownership/mode and SELinux-label readback. It refuses existing output
paths and cleans its owned temporary files. These are staging checks, not a
runtime-CFI or boot guarantee; never flash this partition without its paired boot.
