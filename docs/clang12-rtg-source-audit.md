# Per-Process RTG Interface Audit

## Scope And Device State

Read-only follow-up after r32, on branch bringup/clang12 at c34035785.
The confirmed ALN-AL00/HWALN device runs Linux 5.10.43 #32 in normal Android
with boot completed, SELinux Enforcing and taint 4. No configuration, kernel,
firmware, device-tree, permissions, scheduler state or partitions change.

The earlier r30 runtime snapshot reports RtgSchedIpcFile failing to open
/proc/<pid>/rtg. On r32, that entry is absent for both PID 1 and the current
system_server, while their ffrt entries exist. The examined recent, bounded
logcat snapshot has no new RtgSchedIpcFile message; this is not proof that the
interface has recovered or that the earlier error recurs continuously.

## Actual Configuration Gate

The proc operations and the tgid directory entry in fs/proc/base.c are both
guarded by CONFIG_HW_RTG_SCHED. CONFIG_SCHED_CGROUP_RTG is not their gate.
The final r32 ELF contains proc_ffrt_open/proc_ffrt_ioctl, but no
proc_rtg_open/proc_rtg_ioctl implementation. This confirms a build-time
interface omission rather than an entry hidden only from an unprivileged
reader. It does not establish that all related-thread scheduling is absent.

The saved stock configuration enables PERF_CTRL, FRAME_RTG,
HW_RTG_MULTI_FRAME, HW_RTG_VIP_FOR_MULTI_FRAME, HW_RTG_SCHED and
HW_RTG_CONSTRAINT. None survive in the generated r32 configuration.
SCHED_RTG, SCHED_WALT and FFRT_RTG remain enabled. The dependency chain is:

```text
HW_RTG_SCHED -> FRAME_RTG -> SCHED_RTG && PERF_CTRL
```

init/Kconfig already sources kernel/hwrtg/Kconfig and, under HISI_EAS_SCHED,
kernel/sched/frame/Kconfig. The missing link is PERF_CTRL: its definition
exists in platform_source/cee/drivers/perf_ctrl/Kconfig, but that file is not
sourced by the published configuration graph. The parent driver Makefile
also does not traverse perf_ctrl/. The directory's own Makefile requests
perf_ctrl.o, but perf_ctrl.c is absent, including in the original official
source import at 881d03c79. This is not a deliberate override in the bring-up
fragment and not simply a forgotten HW_RTG_SCHED proc registration.

## User/Kernel Protocol Checks

The current system_server maps the stock 64-bit libiAwareRtgSched.so and
libhwrmeRtgSched.so. The examined libiAwareRtgSched implementation formats
/proc/%d/rtg using getpid() and opens it O_RDWR. The kernel directory entry
would have mode 0644. proc_rtg_open separately accepts UID 1000 or application
UIDs >= 10000, rejecting root and other low UIDs. A future root open probe
must not be treated as an expected success or used to justify permission
changes. Runtime SET_ENABLE and SET_CONFIG require UID 1000; the feature
starts disabled and requires actual initialization, not only file creation.

Selected AArch64 and Thumb disassembly of the installed 64-bit and 32-bit
libiAwareRtgSched libraries agrees with the native kernel command encoding:

| Operation | ioctl | Payload size |
| --- | --- | --- |
| SET_ENABLE | 0xc020ab03 | 32 bytes |
| SET_CONFIG | 0xc018ab01 | 24 bytes |
| BEGIN_FRAME_FREQ | 0xc010ab14 | 16 bytes |

The sampled enable/config payloads use a UID/PID/TID header. Their data
pointers are at offsets 24 and 16 respectively. The 32-bit library allocates
and clears the same 32/24-byte payloads and writes its pointer into the low
word at those offsets, retaining zero in the high word. It does not use the
smaller ordinary 32-bit C-pointer layout for these commands. The native and
compat proc handlers dispatch to the same ioctl implementation. The two
copies of iaware_rtg.h and proc_state.h in this tree are byte-identical.

No incompatibility is found in these sampled commands and layouts. This is
not a full ioctl ABI, runtime enable, scheduling correctness or caller
authorization certification. No ioctl is injected into the running device.

## Reference Check And Restoration Boundary

The existing audio-source candidate Hooo1941/kernel_huawei_hepburn at commit
8d07a0c080f5e6d0a29aed5f61bac1f4a79001cf, Linux 5.10.43 under kernel,
also lacks perf_ctrl.c at the published driver path. Its RTG ioctl source,
proc_state header and perf_ctrl Kconfig/Makefile match the local files.
Its parent driver graph likewise omits perf_ctrl. The perf_ctrl API header
adds userspace-throttle and HECA commands, so it is not an interchangeable
whole performance-control implementation. No reference code is imported.

Restoring HW_RTG_SCHED is not a proc-only fix: it adds task_struct.rtg_depth,
fork-policy handling and Binder RTG propagation, compiles frame state/timers,
and allows real RT/CFS policy and utilization changes. Enabling PERF_CTRL
also selects scheduler/frame performance handlers. Removing its dependency
without auditing those owners is not equivalent to restoring the stock
contract. Creating an empty proc endpoint, returning fake ioctl success,
enabling CGroup RTG or relaxing SELinux cannot resolve this source gap.

Keep r32 installed. The next bounded action is to locate a matching complete
performance-control implementation and audit its registration, ABI and
required feature closure, or establish a justified separate frame-RTG
dependency model. Only then consider an isolated configuration/source
iteration with paired module-ABI, build and rollback checks. No build or
flash is needed for this documentation-only audit. Downloaded libraries,
reference files and disassembly are removed after inspection.

## Follow-up: Restore The Dispatcher Without Enabling The Feature

Two Linux 5.10.43 releases contain the missing dispatcher, unlike the earlier
9000S/Hepburn candidates:

- HK416AAA/HUAWEI_Kirin9000_Kernel at
  170c9e979789b1078c1ecdf3816a1766334998fd, Kirin 9000.
- wutaijieing/android_kernel_LIO_kirin990_103 at
  2f71c5e433024c76412bab655e6ff54b3e656ca8, repository identifying Kirin 990;
  its inspected source reports Linux 5.10.43.

Their platform_source/cee/drivers/perf_ctrl/perf_ctrl.c files are identical,
with a GPLv2 notice. The driver dispatches ioctl requests to existing query
and control handlers; it does not contain platform register mappings. It
registers a misc device named hisi_perf_ctrl, not perf_ctrl. The correctly
named /dev/hisi_perf_ctrl is also absent on the current r32 device.

The local restoration uses the pinned Kirin 9000 file as its reference:

- Restore perf_ctrl.c and connect its existing Kconfig and Makefile to the
  parent driver graph.
- Use Allen's relocated platform_include/cee/linux/perf_ctrl.h and correct
  that header's stale mm_lb include to its existing basicplatform location.
- Bind command IDs explicitly to the local handlers rather than relying on
  positional array order. The released prefix through SET_TASK_L3C_PART is
  retained; the 11 later Allen command IDs use Allen's existing HVGR, HWP,
  SMT, global-render, frame-VIP and PID-query implementations.
- Preserve the legacy unsupported SET_TASK_UTIL slot, ioctl boundary
  validation, compat pointer conversion and device name. Remove only
  no-op open/release callbacks and unused includes, simplify misc registration
  and add module-license metadata. No new dummy handler is introduced.

The 47 named command slots have no duplicate IDs and cover all current
commands except the deliberately unsupported SET_TASK_UTIL slot. This is
source dispatch coverage, not proof that each optional hardware handler is
enabled or functional. Existing optional-handler error/default behavior is
not redesigned. In particular, the retained legacy capability-word and
other native/compat payload contracts require separate runtime ABI review;
the earlier per-process RTG checks do not certify this different device API.

### Enabled-Build Result And Next Missing Provider

Enabling only PERF_CTRL adds that symbol and exposes FRAME_RTG,
SCHED_STAT_YIELD and DDR_PERF_CTRL as disabled options; no existing active
configuration value changes. The new dispatcher and existing
sched_perf_ctrl/frame_perf_ctrl objects compile with Android Clang 12.
The complete enabled kernel build fails because the pre-existing
thermal_platform Makefile also requests thermal_perf_ctrl.o, whose source
is missing. No linked enabled kernel or test image is produced or flashed.

The pinned Kirin 9000 release supplies a thermal_perf_ctrl.c candidate, but
it is not imported. It reads sensor IDs 0, 1 and 2 for maximum SoC temperature.
Allen's live tsens name order is cluster0, cluster1, cluster2, gpu, modem,
npu, peri, ddr: that candidate would omit GPU from this maximum despite its
comment referring to GPU. Its cooling-power query also leaves zero values
when a cooling-device lookup fails and still reports success. Do not copy
these platform assumptions or silently manufacture successful sensor data.

The compatibility fragment therefore explicitly keeps PERF_CTRL, FRAME_RTG,
SCHED_STAT_YIELD and DDR_PERF_CTRL disabled. This preserves the prior
effective feature state when regenerating from the original stock config,
rather than accidentally activating newly visible options. The source
dispatcher restoration is dormant, pending its required query-provider
closure. Frame RTG, Binder propagation, task layout and thermal control
policy remain unchanged. Device r32 and its retained paired images are not
touched.

After restoring the disabled baseline, the complete Clang build passes.
The final ELF retains ffrt and omits the dormant perf_ctrl/rtg handlers; the
exported symbol set and every export CRC agree with the r32 baseline.
Enforcing CFI, ThinLTO and forced module signing remain configured.
checkpatch reports no errors or warnings for the new dispatcher. These are
disabled-baseline and source checks, not enabled-driver runtime or signed
paired-image certification. Temporary reference files and build logs are
removed; original backups and the retained r28-r32 test pairs remain intact.

Next audit the two missing thermal query callbacks against Allen's named
sensor/zone mapping, actual units and missing-cooling-device contract, then
complete an enabled link before considering a runtime or frame-RTG step.
Do not remove the required thermal object, disable thermal protection or
return fake successful zero values just to obtain an enabled build.

## Follow-up: Complete The Thermal Query Provider

The new thermal_platform/thermal_perf_ctrl.c retains the two query entry
points and fixed-width payloads from the pinned Kirin 9000 5.10.43 reference,
but uses the local IPA contracts instead of its platform assumptions:

- get_ipa_status supplies the four existing frequency-limit fields. These
  are IPA limits, not newly measured instantaneous CPU/GPU frequencies.
- Maximum SoC temperature uses ipa_get_sensor_value over g_ipa_sensor_num,
  the same DT-configured range used by the existing IPA policy. Allen's live
  ipa_sensors_info contains three CPU clusters and four IPA actors/sensors
  named cluster0, cluster1, cluster2 and gpu, with identity actor indices.
  The corresponding first four tsens entries include GPU at index 3.
- ipa_get_sensor_value already converts its integer-degree source to
  millidegrees. The new query adds no conversion, clipping, compensation,
  smoothing or extra precision. It does not use the smoothed soc_thermal
  value or include unrelated modem/NPU/peripheral/DDR sensors in this field.
- The board-temperature field keeps the reference's named system_h
  peripheral source. Its existing provider also returns millidegrees; it is
  not substituted with the filtered board_thermal value.
- Each backend must succeed before output is copied to userspace. Acquisition
  errors are returned directly, rather than added together or overwritten.
  GET_IPA_STAT is an output-only query, so it no longer reads a dummy input
  payload. All six output fields are supplied by the existing providers.
- Cooling-power queries validate the user-selected SoC/board zone and query
  all four named CPU/GPU cooling devices through thermal_zone_cdev_get_power.
  Allen's two live zones each bind thermal-devfreq-0 and thermal-cpufreq-0/1/2.
  A missing binding or failed query returns the existing helper's error, not
  a successful zero or partially populated result. The underlying helper's
  error classification and power units are not changed.

Only query code is added. Existing thermal sensor acquisition defaults,
negative-value handling below this interface, filtering, trips, governors,
frequency limits, protection, calibration and DT remain unchanged. This is
not a repair of the separate stock shell-temperature calculation guard.
The helper is compiled only when IPA_THERMAL is enabled, matching its public
header contract. A zero measured power remains valid data when the backend
actually returns success; missing power is not inferred to be zero.

### Query ABI Boundaries

ipa_stat has six 32-bit fields (24 bytes); thermal_cdev_power has a 32-bit
zone selector and four 32-bit power fields (20 bytes). These two payloads
have no native pointers or unsigned-long fields and keep the same layout
for native and compat callers. Runtime query transport is not yet tested.

The dispatcher reference has a separate unsigned-long capability query:
native AArch64 requests eight bytes, while a standard compat request encodes
four. The old compat forwarding path would still copy an eight-byte native
word. The restored dispatcher now handles the exact four-byte compat
GET_DEV_CAP encoding with put_user(compat_ulong_t). Native/eight-byte
requests retain the existing route and value. The new route uses standard
uaccess fault handling and changes no capability bits.

Read-only inspection of the installed 32/64-bit libhwrme_client.so also finds
matching GET_RELATED_TID (0xc8087805) and GET_RENDER_RT (0xc2007812) request
encodings in both variants. The stock 32-bit caller uses the native-sized
render request, not the smaller ordinary 32-bit C-struct encoding. This is
sampled caller evidence, not a complete performance-control ABI audit.
No library calls, scheduling commands or ioctl requests are injected into
the current device.

CPU_HWP_CTRL is not enabled in this build. Its existing header fallback
returns a successful no-op, so the public dispatch slot is now populated
only when the real HWP backend is enabled. Otherwise the established
unsupported-command path returns EINVAL. The shared OEM fallback itself is
not rewritten, and no backend or fake-success implementation is added.

### Build Scope And Runtime Gate

The compatibility fragment now enables built-in PERF_CTRL. FRAME_RTG,
SCHED_STAT_YIELD and DDR_PERF_CTRL remain explicitly disabled; HW_RTG_SCHED
and its Binder/task-layout paths remain unavailable through that dependency.
The initial complete enabled build succeeds after adding the thermal helper.
The final build after both dispatcher boundary changes also succeeds.
checkpatch reports no errors or warnings in either changed C file. The final
ELF contains both thermal query entry points and the misc-device init entry;
the compat capability branch compares the exact four-byte command encoding
and uses a 32-bit sttr store. Its 49-slot dispatch table contains 46 active
handlers, with reserved, SET_TASK_UTIL and inactive HWP slots left NULL.

The fresh modem module has 390 versioned imports, with no missing exports or
CRC mismatches. The full kernel export set and every export CRC agree with
the saved disabled baseline. Enforcing CFI, ThinLTO and forced module signing
remain configured. These checks do not validate runtime CFI behavior, module
loading, an installed image pair or userspace query results. No new paired
images are packaged or flashed in this step.

The device remains on r32. No new boot/modem image is flashed, no independent
ramdisk, node permission, SELinux rule or user setting changes, and no prior
audio/network/suspend functional pass is repeated. An enabled build is not
proof of device-node ownership/labeling, native/compat ioctl behavior, query
accuracy or full scheduling compatibility. Next perform bounded read-only
query tests only after paired image, module-ABI and security checks; keep
r32 as the immediate rollback for this unbooted candidate. Frame RTG
restoration remains a separate iteration.

## r36: Bounded Native And Compat Runtime Validation

The completed #36 build is packaged with the existing boot and modem staging
scripts. Boot kernel round-trip, unchanged firmware stub/DT/header and partition
bounds pass. The signed matching modem module has 390 versioned imports,
no missing exports or CRC mismatches, and successful PKCS#7 verification;
its ext4 content, owner, mode and SELinux label are checked on readback.
The installed r32 boot/modem pair first matches the retained rollback files.

Both r36 flashes succeed after serial/model/unlock and partition checks.
Fastboot product/all queries remain unsupported; supported OEM identity
queries and the existing ADB partition mapping are used, not assumed sizes
reported as zero by fastboot. Device-side exact-length hashes match both
installed r36 images. Independent Magisk ramdisk readback is unchanged.
Normal Android completes boot, the matching modem module is live, taint is 4,
module signature enforcement is Y and SELinux remains Enforcing.

The misc node is /dev/hisi_perf_ctrl, major/minor 10:90 in this boot. It keeps
system:system, mode 0660 and u:object_r:hisi_perf_ctrl_device:s0 without any
chmod/chown, policy edit or permissive interval. Ordinary shell open returns
EACCES. Successful root probes run in the existing Magisk domain and do not
prove that each intended system/app caller can access the device.

Temporary freestanding AArch64 and ARM EABI executables perform only the
three read-only query command numbers. No scheduling-setting command runs:

- Native GET_DEV_CAP returns the eight-byte value 7. The 32-bit process tests
  both the standard four-byte encoding and the legacy eight-byte encoding;
  each returns 7, and the legacy high word is zero. Canaries on both sides
  remain intact, including the word immediately after the four-byte result.
- Native and compat GET_IPA_STAT return success with all six fields supplied.
  The existing CPU limit fields are 1530000/2150000/2620000 and GPU limit
  750000000 in their provider units, not instantaneous clocks. Sampled SoC
  outputs are 44000/49000 and system_h outputs 28550/28500 millidegrees.
  Surrounding raw CPU/GPU and system_h reads have consistent units and scale.
  They are sequential, time-varying acquisitions, not an atomic comparison
  or independent proof of sensor calibration or maximum selection accuracy.
- Both SoC and board cooling queries succeed in both ABIs with four output
  fields in GPU/CPU0/CPU1/CPU2 order. GPU power is legitimately zero in these
  samples while CPU fields are populated. No missing backend is simulated,
  so actual backend-error transport is not claimed as runtime-tested.
- Zone selectors -1 and 2 return EINVAL in both ABIs, leave the input payload
  untouched and preserve all canaries. Valid query buffers also remain
  within their fixed 24/20-byte boundaries.

Full time-zero kernel rings before/after queries show no WARNING, BUG, CFI
failure, Oops or panic. An early native crash marker is investigated rather
than omitted: vendor_init dies during stock sys_wp_init_action, then init
restarts its subcontext. Restoring r32 reproduces the same action failure and
SIGSEGV address 0x14; r36 is then restored and boots normally again with the
same inherited failure. This is not a repaired write-protection path or a
claim that the whole system has no crashes. The detailed comparison is in
docs/clang12-functional-tests.md. No extra query run is needed on the second
boot of the unchanged candidate after the first native/compat pass.

PERF_CTRL is accepted only within this bounded query/registration scope.
Capability bit CAP_RTG_CMD reflects SCHED_RTG, not restored FRAME_RTG or
HW_RTG_SCHED; /proc/1/rtg is still absent as configured. Full command ABI,
normal-caller use, frame scheduling and thermal safety remain separate work.
r32 stays available as immediate paired rollback. Device probes, host
temporary executables and diagnostic logs are removed after recording the
results; original backups, accepted r17 and the latest r29-r32/r36 pairs remain.

## r37 Follow-up: Enabled Source Audit And String-Copy Repair

The installed baseline is now r37, with PERF_CTRL enabled and FRAME_RTG still
disabled. A separate temporary configuration enables only the five original
frame features: FRAME_RTG, HW_RTG_SCHED, HW_RTG_CONSTRAINT,
HW_RTG_MULTI_FRAME and HW_RTG_VIP_FOR_MULTI_FRAME. olddefconfig with the
complete trusted Clang/native/compat toolchain arguments changes no other
enabled value. The two-flag FRAME_RTG/HW_RTG_SCHED subset is insufficient:
without MULTI_FRAME, parse_rtg_attr is the existing EFAULT fallback rather
than the original attribute implementation. SCHED_STAT_YIELD, DDR_PERF_CTRL,
CGroup RTG and tracing/debug options remain disabled.

The active ioctl source includes kernel/hwrtg/include/proc_state.h and
iaware_rtg.h through relative paths. Their public include/hwrtg copies are
still identical. The original proc entry keeps mode 0644, task euid/egid
ownership and security_task_to_inode. Open permits UID 1000 or application
UIDs >= 10000; configuration, enable, attribute and RT-thread assignment
require the actual current UID 1000, not the payload's claimed UID. Frame
begin/end checks the current TID, CFS assignment checks the current PID and
group ownership, and activity control checks caller/group UID. These are
sampled source checks, not complete authorization certification.

Three string handlers -- SET_CONFIG, SET_RTG_THREAD and SET_RTG_CFS_THREAD --
previously ignored the nested copy_from_user result in both ABI branches.
An inaccessible or partially accessible string could reach parsing as an
incomplete stack buffer. Each handler now converts the compat pointer once,
then checks a single copy and returns the existing INVALID_ARG error before
termination or parsing on failure. Successful payload layout, command IDs,
length bounds, parser behavior and authorization are unchanged. No buffer
zeroing, fake success or speculative fallback is added.

The original-feature configuration compiles all six hwrtg objects, the frame
and multi-frame objects, Binder, fork and proc/base with Android Clang 12,
ThinLTO and enforcing CFI. The final changed ioctl object has no diagnostics
in its own C file, and checkpatch reports no errors or warnings for the patch.
The requested-object build is not a complete kernel link, task-layout/module
CRC check, image certification or runtime fault-injection test. The active
r37 output configuration, device, security settings and partitions are
unchanged. Do not enable the feature solely on this compile result.

Further source audit identifies a deterministic timer-lifecycle blocker:
deinit_frame_timer clears g_timer_on but keeps its persistent worker;
init_frame_timer rejects that existing worker on re-enable and never restores
g_timer_on. The ioctl enable wrapper also returns success after an invalid
string, failed state setup or timer-worker creation failure. Resolve this
real enable/disable contract separately before an enabled candidate. The
render capability query currently accepts only its native-sized command;
the exact switch rejects the ordinary four-byte compat encoding rather than
over-copying that request. Its intended 32-bit caller contract remains to be
checked. Full frame authorization and timer/state synchronization also remain
open; no claim is made that dormant OEM scheduling code is ready to expose.

### Timer Re-Enable And Failure Reporting Repair

The separate lifecycle repair keeps the original persistent frame_sched
worker instead of rejecting it on re-enable. Timer initialization and worker
creation now stay under the existing g_timer_lock; successful reuse restores
g_timer_on. kthread_create failures return PTR_ERR and clear the failed
pointer so a later enable can retry. No thread-stop path, new workqueue,
timer duration, boost policy or feature default is introduced.

init_frame_timer now returns a status, with both existing header copies
updated. The enable handler propagates invalid input, failed state setup and
timer initialization errors. If timer setup fails after state initialization,
it uses the existing deinit_proc_state cleanup and does not publish
g_rtg_enable. Already-enabled and successful disable requests retain their
existing success behavior. These changes do not make the whole ioctl/state
lifecycle atomic; concurrent configuration and timer/worker state ordering
still require review before activation.

Both changed objects compile in the original-feature configuration with no
diagnostics in their own C files. checkpatch passes, and the compiled LLVM
IR retains all three string-copy failure branches and the status-returning
timer entry. No installed enable/disable cycle or induced kthread failure is
tested while this feature is disabled on r37; runtime certification remains
open rather than replaced by a mock implementation test.

Clang debug metadata from the enabled and r37 fork objects gives task_struct
size 4672 bytes in both builds, but this is not layout compatibility: the
new rtg_depth member occupies byte offset 112, and 23 existing top-level
members, including proc_qos and sched_class, move by eight bytes. Other
top-level offsets in this comparison remain unchanged; nested types and
export CRCs are not certified by it. A complete enabled link and matching
module ABI review are mandatory. No enabled image is packaged or flashed.

## END_FREQ Caller Scope And Enabled-Link Candidate

Further read-only disassembly of the installed native and Thumb
libiAwareRtgSched implementations identifies END_FREQ as 0xc010ab18 in both
ABIs. EndFreq forwards to SetCommandFreq, which obtains getpid, gettid and
getuid and stores the caller's own UID/PID/TID header plus a zero frequency
word. Thumb calls are resolved through the library's actual PLT relocations,
not by interpreting its Thumb text as ARM instructions. This command is not
an observed system-server request to stop an arbitrary application's group.

The existing stop_frame_freq trusted the supplied TID and changed that
target's frame state, unlike adjacent self-thread frame/min-util handlers.
It now rejects a non-current TID with the existing FRAME_ERR_PID result and
looks up the current task's frame directly. Valid native/compat caller
payloads, NULL handling, missing-frame errors and successful state updates
are unchanged. The affected proc_state object compiles in the audited
original-feature configuration; checkpatch passes. An installed negative
ioctl test is still required before claiming runtime authorization coverage.

The compatibility fragment now explicitly selects the original five frame
features for an isolated enabled-link candidate. The installed r37 output
configuration remains unchanged. The obsolete initial #2 build cache is
removed after preserving its configuration, symbol metadata and signing
material; original image backups, accepted r17 and recent rollback pairs
remain. The independent candidate uses the existing signing key rather than
introducing an unrelated key change, and the projected workspace stays below
20 GB. A running build is not yet a linked or installed candidate.

Activation also requires classifying two global proc routes. The render
init/stop/destroy/query backends operate on global render state and do not
check caller UID; their inited helper tests state existence, not identity.
The proc boost handler similarly changes all frame groups, with numeric
bounds but no caller-UID or group-membership check. The proc mount has the
existing hidepid setting, but applications can use their own proc entry;
that alone cannot prove these command paths inaccessible. Check the actual
installed SELinux ioctl policy and intended library use before choosing a
permission repair. No permissive interval, mode/owner change or global
control request is used for this audit. The candidate is not approved for
flashing solely because the enabled link succeeds.

### Installed SELinux Extended-Permission Evidence

The current r37 binary policy is copied read-only from /sys/fs/selinux/policy
and verified against the device-side hash of the exact copy. Its temporary
device file and directory are removed immediately. No policy reload, label,
owner, mode or enforcing-state change occurs. A temporary host build of the
official SELinuxProject libsepol/checkpolicy 3.9 release reads the policy;
both downloaded release assets match their official SHA-256 digests. This
tool adds no kernel implementation or permanent host package installation.

The policy.conf dump is inspected with typeattribute membership and self
targets resolved, not only by searching literal same-type allow rules:

- untrusted_app and its installed _25/_27/_29 variants have basic self:file
  read/write/open/ioctl permission. Their explicit ioctl list includes
  0xab05, 0xab0a, 0xab14-0xab18, 0xab1a, 0xab1e-0xab20 and 0xab22-0xab28.
  Thus the five render operations and capability query, 0xab23-0xab28, are
  explicitly permitted for these applications' own proc files once the
  RTG feature is initialized. The old cross-target END_FREQ at 0xab18 is
  also inside this list, supporting the self-thread repair's relevance.
- platform_app, priv_app and system_app have a smaller self:file RTG list
  ending at 0xab22; their examined rules do not grant the render range.
- system_server has its self:file configuration list 0xab01-0xab05.
- The examined application/system-server self:file lists do not grant
  0xab64, the global boost command. The shared domain:file driver whitelist
  is 0x5450-0x5451, not a grant of every RTG command. Do not invent a boost
  permission change based on the C handler alone.

These are current policy/source reachability checks, not an application
execution test, exploitation attempt or complete audit of every SELinux
domain. r37 still has FRAME_RTG disabled. No global render or boost request
is injected into the device. The explicit render application permission
means a system-UID-only blanket restriction would change the original
allowed caller scope and is not adopted silently.

Before activation, the proposed repair is application ownership enforcement
at the copied-request/backend boundary, with the existing system controller
scope preserved. It must also prevent an application's force-init from
replacing another process's active global render group. Checking a user
pointer in a wrapper and then re-copying it in the backend would leave a
TOCTOU gap and is not sufficient. Exact cross-process caller compatibility
and lifecycle ordering remain open. User confirmation is requested for
this behavior-changing ownership scope; keep r37 installed while it waits.

The separate proc capability getter retained a stale CONFIG_HISI_RTG guard,
although this tree defines SCHED_RTG and has no HISI_RTG Kconfig symbol or
hidden compiler definition. It now uses SCHED_RTG, matching the restored
misc-device getter and the real backend. The standard four-byte compat
capability request 0x8004ab28 is handled only for the ARM32 ABI, using
put_user(compat_ulong_t). The legacy eight-byte form 0x8008ab28 remains on
its original path. Native callers do not gain the four-byte encoding.
The existing feature-enabled gate, NULL/uaccess errors, command IDs and
other payload layouts are retained; no scheduling-setting request is added.

### Complete Enabled-Link And Signed-Module Result

The first complete original-feature build links as #38. It adds 13 exports,
removes none, and changes 8936 export CRCs relative to the installed r37
build. The old retained r37 modem module has 151 mismatched imports against
this enabled kernel; the matching rebuilt module has all 390 imports present
and matching. The other produced module is the unchanged OEM hisi_dummy_ko
artifact, with two matching imports; it is not a hardware-function pass.
The observed live device loads only modem_driver, and the examined stock
module directories contain that same installed module. Configured legacy
HI1102 module flags are not evidence of another deployed Allen module.

After the capability changes, the complete relink succeeds as #39. Every
export and CRC remains identical to the #38 enabled build. The final native
ELF retains proc_rtg_open/native/compat entries and real frame initialization.
Its compat path checks ABI and exact 0x8004ab28, materializes capability 7
and uses a four-byte sttr store; this is compiled-width evidence, not an
installed canary or normal-application query result. The changed ioctl C
file has no compiler diagnostics, and checkpatch reports no errors/warnings.
Enforcing CFI, ThinLTO and forced module signing remain configured.

The final modem module is processed on a temporary copy with the existing
staging operations: strip debug information and sign with SHA-512 using the
existing key. Its 390 imports still match; PKCS#7 content verification passes
with the matching certificate, and that certificate is present in the final
kernel Image. Raw build output is not treated as a signed installed module
merely because MODULE_SIG_ALL is configured; the established staging step
performs the signing. This check does not package or certify an ext4 image,
boot round-trip, partition readback, runtime CFI or module loading.

Superseded unflashed #38 ELF/LTO link output is removed before the relink to
avoid transient duplicate storage. The workspace remains below 20 GB;
original backups and all retained test/rollback image pairs are unchanged.
The candidate's original-feature configuration changes only the five audited
frame flags relative to r37. Its main baseline output and installed device
remain r37, with SELinux Enforcing. No #38/#39 image pair is packaged or
flashed while application render ownership and lifecycle certification remain
open. Consumed policy/library/source dumps, signing copies and full logs are
removed after recording the results.

### Approved Render Ownership And Transactional Initialization

The user approves applications managing only their own process while retaining
the actual UID1000 system controller's cross-process scope. The five shared
render backends now enforce this boundary after their single user copy and
under g_rt_rwlock. Both proc RTG and PERF_CTRL dispatch use these backends;
there is no second copy of a wrapper-validated request. Queries, stop and
destroy check the existing group's task identity. Initialization checks both
the existing owner, including force-init, and the resolved new target's
thread group. A matching UID or caller-supplied PID is not proof of ownership.
Root does not gain the UID1000 controller exception.

Initialization previously cleared the active pointer even when returning
EBUSY, and with RENDER_RT_GLOBAL enabled its memset cleared the first global
wake-queue buffer instead of RENDER_RT_GRP. It now validates and collects the
target before replacing the correctly selected group buffer. Failed busy,
ownership, lookup and missing-render-index requests leave the previous group
unchanged. RCU covers task lookup, collection, sorting and publication; task
freeing takes the same render lock before releasing the stored task/signal
identities. The free hook compares the actual render task, not a reused PID.
Allocation outside the spinlock uses GFP_KERNEL. No new task_struct fields,
credential overrides, policy changes or synthetic render results are added.

The affected enabled-config object compiles, compiled IR retains the ownership
checks, and diff/checkpatch checks pass. These are source/object results only:
the installed device remains r41 with FRAME_RTG disabled. Fresh full link,
paired ABI/signature checks and actual native/compat/application lifecycle
validation remain required; stale uninstalled #39 is not a current candidate.
The separate PERF_CTRL global wake-queue lifecycle is not covered by these
five group-backend checks. Full enable/disable concurrency and worker ordering
also remain gates before activating frame scheduling.

### Proc Enable/Disable And Timer Worker Ordering

The proc dispatcher now uses a lifecycle rwsem: enable/disable holds its write
side and normal native/compat commands hold the read side. This drains active
requests before reset and prevents overlapping controller enable/disable
operations without serializing normal frame ioctls with one another. It does
not change ioctl encodings, caller policy or scheduler-hook locking. Lock
ordering is lifecycle rwsem, then existing timer mutex, then downstream
frame/task locks; scheduler hooks do not acquire the new rwsem.

Disable now stops the timer before resetting proc state. The persistent
worker performs timeout cleanup under the existing timer mutex and checks
both g_timer_on and the pending-work bit after acquiring it. Thus disable
drains an executing cleanup, and a worker that wakes after disable/re-enable
does not consume a retired timeout. The callback only clears/wakes the bit;
it does not acquire the mutex used around del_timer_sync. The enabled objects
compile and diff/checkpatch pass. This lock review is not a runtime stress,
timeout-delivery or performance result; these still require the current
matched enabled candidate, and PERF_CTRL's independent routes remain separate.
