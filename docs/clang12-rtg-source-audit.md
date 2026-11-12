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

### r42 Enabled Device Validation

The fresh current-source frame-enabled build is #42, not the stale #39
candidate. It includes the r41 MIPS repair and changes only the five selected
frame flags relative to r41; additional unselected Kconfig entries do not
enable other features. Task debug metadata still reports 4,672 bytes but 23
existing member offsets shift, with rtg_depth added. Examined binder_proc,
binder_thread, binder_transaction and binder_node sizes/member offsets are
unchanged. This is not a blanket task-layout or external-module ABI guarantee.

The enabled export table adds 13 symbols, removes none and changes 8,936 CRCs
relative to r41. The fresh matching modem has all 390 imports present and
matching, and the uninstalled OEM dummy artifact has two matching imports.
Only modem_driver is observed loaded on the baseline. Enforcing CFI, ThinLTO,
MODVERSIONS and forced signing remain configured. The new source files have
no compiler diagnostics. Signed-module PKCS#7/certificate, ext4/label readback
and boot kernel/DT/firmware/header round-trip checks pass.

The confirmed r41 device partitions match its retained immediate rollback
pair before flashing. The same serial/model/unlocked bootloader is confirmed
using the supported OEM queries; standard product/all still return Command
not supported and are not substituted for identity or partition bounds.
Both local r42 flashes succeed. Exact image-length boot/modem hashes match
after boot, and the full independent ramdisk hash is unchanged. Neither
calibration, DT/firmware, user data nor SELinux policy is modified.

A temporary APK, packaged with checksum-verified official Android build tools
35.0.0 and the tagged AOSP android-10.0.0_r1 JNI header, executes ordinary
64-bit and ARM32 application processes. Its actual domain is untrusted_app_27;
no run-as domain, injected credentials, mode/label change or permissive window
is used to obtain application results. Each ABI passes:

- Native-sized eight-byte capability value 7 and surrounding canaries;
  standard ARM32 four-byte capability value 7 and four-byte canaries. Native
  callers reject the ARM32 encoding; inaccessible output pointers fail.
- Own init/query/hot-thread/stop/resume/force-init/destroy/re-init operations.
  ARM32 render queries use the observed OEM native-sized 512-byte layout,
  not a newly invented smaller C-struct ABI.
- EBUSY, invalid forced target and failed user copy leave the existing group
  usable, rather than clearing its pointer or replacing its owner.
- A forked child with the same UID cannot query, stop, destroy or force-init
  the parent's group, including force-init directed at itself. Each child
  opens its own proc file. Parent queries still succeed after these refusals.
- A child that initializes its own group and exits is reaped; subsequent
  parent initialization succeeds after normal task-free cleanup.

These are real ioctl and lifecycle checks, not mirrors of the C branches.
An initial ARM32 APK ABI-switch launch encounters Connection refused to the
secondary zygote before executing the probe. Force-stopping and relaunching
only the temporary APK then produces a complete passing ARM32 result on the
same kernel and policy. No system service/socket/property fix is inferred.
The two observed temporary-app AVCs concern FinalizerDaemon getopt on the
primary USAP unix socket, not render ioctl permission. No policy widening is
used for either the startup transition or these unrelated messages.

A freestanding controller executes with actual UID1000 in the existing
Magisk domain. It successfully initializes/queries/stops a test child in a
different process, force-replaces it with self and destroys self. Three
disable/re-enable cycles each report disabled capability gating followed by
capability 7. The frame_sched worker PID is unchanged. This validates the
kernel UID boundary, not every system_server SELinux permission or ordinary
library call. No global boost is injected to bypass the examined ioctl lists.

The temporary APK and controller are removed before normal reboot. A changed
boot ID and complete early logs establish two normal r42 boots. The stock
framework's sManagerService disables/enables RTG after each boot; the test
controller's default-only configuration is not left as a claimed equivalent
to system setup. Both boot logs and the complete post-probe log have no WARN,
BUG, CFI failure, panic or Oops. Each contains the previously attributed
unconditional wiphy_new_nm dump_stack; it is not a new warning. Taint stays 4,
SELinux stays Enforcing, PRMEM remains enabled and module sig_enforce is Y.

A bounded post-reboot MIPS telemetry check covers CPUs 0-11 with 2,622 samples,
862 containing nonzero instruction counts, and 2,280 post-writel desired-vote
events spanning 19 distinct name/vote pairs. It uses a task-owned trace
instance and a short anonymous zero-stream/hash workload, with start/end
markers, zero overruns and all six monitor maps/ratios/enables/sample windows
unchanged. The existing wifi trace instance is untouched. This is a relevant
counter/vote regression after scheduler layout activation, not a repeated
26,078-sample machine-code oracle, measured physical frequencies or power
parity. Original backups, r17 and the latest five pairs remain; r41 is the
immediate paired rollback. Task-owned probes/tools/traces and consumed full
logs are removed after recording these results.

Remaining RTG work includes the separate PERF_CTRL global wake-queue lifecycle,
real timer expiry under an intended permitted caller, full frame-thread
scheduling/END_FREQ integration and bounded concurrency/performance coverage.
The approved application group ownership and tested re-enable scope pass;
this does not close the complete scheduler or bring-up acceptance queue.

### Global Initialization Error Transport

Read-only r42 checks confirm /dev/hisi_perf_ctrl remains system:system 0660
and hisi_perf_ctrl_device. Separate global commands are INIT_RENDER_TID_GLO
44, GET_RENDER_RT_GLO 45 and DESTROY_RENDER_RT_GLO 46. Native and ARM32
initialization requests both use the same 12-byte fixed-field payload.
The packed task membership field is a u32 with four eight-bit slots:
queue_index << PER_SHIFT, where PER_SHIFT is 3, means queue_index * 8.
It is not a three-bit-per-queue packing error and is not modified.

A bounded actual-UID1000 Magisk-domain probe issues only inaccessible
initialization/output addresses and an absent PID. On r42, both ABIs return
positive 1 for global initialization's failed user copy and missing target,
while the adjacent getter correctly returns -EFAULT for failed user copy.
No valid global queue is initialized, sampled, resumed or destroyed. This
isolates the backend's bool local variable converting negative errno to true,
not an ioctl encoding or generic compat transport error. The API declares an
int return, and nonnegative ioctl results may be interpreted as success.

The local status variable is changed to int so existing EFAULT, ENODEV and
EBUSY failures retain their signed values. Successful operations, request
layout, dispatch, caller permissions, queue state and existing resume/reset
semantics remain unchanged. This does not resolve global ownership,
task-identity/re-init cleanup or queue-depth lifecycle questions; those require
the matching caller/implementation contract rather than speculative changes.
Full current-source link/pair and installed negative-return validation follow
as an isolated iteration with r42 retained as rollback.

The read-only client check distinguishes common hwrme_client group commands
17/18 from the independent global commands 44/45/46. A scoped immediate-
materialization scan of the 64-bit hwrme/iaware/perfhub candidates does not
locate a direct global-command caller. It is not a complete indirect-call,
all-library or permission proof, and does not justify changing global
ownership, force-init or same-target non-force resume behavior. The client
copies are verified against their exact device-side digests; private vendor
libraries are read with existing root access, without changing labels,
permissions, policy or firmware. Copies/disassembly are temporary only.

### r43 Global Error Validation

The one-line status-type repair passes diff/checkpatch and full #43 build,
with no diagnostic in render_rt.c. Configuration, the complete exported
symbol/CRC table and signing certificate are byte-identical to r42. The
matched staged modem has 390 present/matching imports; its PKCS#7 signature,
certificate inclusion, ext4 integrity and SELinux metadata pass. Boot kernel
round-trip, original firmware/DT/header preservation and partition bounds
pass. No unrelated frame feature, task field, ABI or credential gate changes.

The retained r42 boot/modem partitions match before flashing. Supported
serial/model/unlock checks confirm the same Allen bootloader; unsupported
product/all queries and zero fastboot sizes are not used as identity/bounds.
Both r43 flashes succeed, paired image-length readbacks match and the full
independent ramdisk hash is unchanged. Actual UID1000 in the existing Magisk
domain executes the same bounded read-only native and ARM32 probes used on
r42. Failed initialization copy now returns -EFAULT (-14), and the absent
PID returns -ENODEV (-19), rather than positive 1. The adjacent getter still
returns -EFAULT for its failed copy. No valid global queue state is changed.

Probe removal is followed by a normal reboot with a different boot ID. Both
complete early logs and the post-probe log have no WARN, BUG, CFI failure,
panic or Oops. The inherited unconditional wiphy stack remains separately
classified. SELinux stays Enforcing, taint stays 4, PRMEM remains enabled and
module sig_enforce is Y. The loaded matching modem is Live, and ordinary
framework frame configuration remains present. Previously passed r42 group,
timer re-enable and MIPS tests are not needlessly replayed for this independent
local errno-type change; their bounded scope is not expanded into global
queue or performance certification.

Original backups, r17 and the latest r36/r37/r41/r42/r43 pairs remain. r42 is
the immediate paired rollback. Temporary device probes, library/disassembly
copies and consumed full logs are removed after recording these conclusions.
Next global work must resolve the matching caller and non-force resume,
owner retirement and depth/reset contracts before changing queue semantics.

### Global Force-Init Depth Retirement

The matching retained original Allen 5.10.43 Image is recovered offline with
the same pinned marin-m/vmlinux-to-elf commit documented in the MIPS audit.
The raw Image and recovered kernel section agree byte-for-byte; _text,
linux_banner and independent embedded absolute string pointers corroborate
the preferred base 0xffffffc010000000. Recovery supplies machine code and
symbols, not original DWARF. No device Image or private data is uploaded.

The original initializer at 0xffffffc0105e40f0 retains the numeric-PID
non-force resume rule. The original destroy backend at 0xffffffc0105e49b4
only clears the active pointer; it does not reset queues. These agree with
the published source and establish that same-live-target pause/resume is
not a missing-reset bug. No owner permission or resume behavior is changed.
PID reuse after root retirement remains a separate unresolved identity issue.

The depth transition does contain a concrete lifetime defect, also inherited
by the original Image. At init+0x178, the new depth is stored before the call
to reset_wake_queue at init+0x194. Reset at 0xffffffc0105e4334 traverses only
that new depth. Its task write at reset+0xb0 skips entries only when their
exited flag is set. The task-free hook reads the current depth at
remove_render_rthread+0x128 and clears all membership bits at +0x16c.

Consequently, force-init 4 -> 1 leaves queues 1-3 and their live memberships
behind. If one of those tasks is freed while depth is 1, its hidden entries
are not marked exited. Force-init 1 -> 4 can then dereference its stale task
pointer when clearing the old queue. This is a source/machine-code-proven
potential UAF, not a crash observed on the device. No unsafe reproduction is
attempted on r43, and no matching caller has yet been proved to use this
transition in normal operation.

The minimal repair clears every old queue's memberships and sets its logical
entry count to zero before publishing the new depth. It uses the existing
reset_inqueue_tasks helper under g_wake_queue_rwlock, already shared with
wakeup, reset and final task-free hooks. Thus a concurrent task free either
marks an entry before retirement or sees it fully retired; later growth
does not scan stale logical entries. Existing exited entries are not
dereferenced. First init has no old queues, and the non-force path never
enters init_wake_queue. Storage, task layout, ioctl ABI, credentials, packing,
queue feedback/rotation and pause/resume policy remain unchanged.

Diff/checkpatch and the frame-enabled render_rt.o build pass with no compiler
diagnostic in the modified file. Optimized ThinLTO IR confirms the old-depth
loop and zeroed logical counts precede the new-depth store. No branch-mirror
test or dangerous old-kernel lifetime probe is added. At this initial source
check stage, device remains r43 and installed validation is pending.

### r44 Installed Depth-Retirement Repair

The fresh full link is #44 from 062a2f726. Its final initializer machine code
clears old task memberships and logical counts before the new depth store,
not merely in pre-link IR. No compiler diagnostic concerns the modified file.
Configuration, the entire Module.symvers/export CRC table and the signing
key/certificate are byte-identical to r43. No task/Binder field, header,
feature gate, ioctl encoding or credential policy changes. The matching
modem has 390 present/matching imports; PKCS#7 verification, certificate
inclusion, ext4 integrity and label/owner/mode readback pass. Boot kernel
round-trip, original firmware/DT/header preservation and real bounds pass.

The installed r43 partitions match the retained rollback pair beforehand.
Supported OEM serial/model/unlock checks confirm the same Allen bootloader;
unsupported product/all queries do not substitute for identity or bounds.
Both r44 flashes succeed. Exact image-length boot/modem readbacks match,
and the independent ramdisk hash remains unchanged before/after installation
and a subsequent normal reboot. Two changed-boot-ID normal startups reach
boot completion with #44. Complete early logs have no WARN, BUG, CFI failure,
panic or Oops; the known unconditional wiphy stack remains separately
classified. SELinux stays Enforcing, taint 4, sig_enforce Y and PRMEM enabled;
the matching modem is Live. The ordinary framework still enables RTG.

These are installed pairing/startup checks and static lifetime-fix evidence,
not positive global queue, root-exit/PID-reuse, actual timer-expiry or full
frame scheduling certification. Only system_server is observed holding a
perf_ctrl descriptor in a pre-flash snapshot. It does not prove global command
usage or inactivity. No unknown system queue is force-replaced, no vulnerable
old-kernel sequence is induced, and prior unrelated tests are not replayed.
The next smallest source audit concerns retired-root identity while retaining
same-live-target resume; caller and positive runtime acceptance remain open.

r43 is the immediate paired rollback. Original backups, r17 and the latest
r37/r41/r42/r43/r44 pairs remain. Consumed full logs, recovered ELF/tool/IR
and other task-owned temporary artifacts are removed; unrelated diagnostics
are preserved. The workspace stays below 20 GB and whole bring-up is open.

### Global Root Identity Repair

The previously recovered original initializer and the published source use
the same numeric-PID non-force resume rule. The retained current source
establishes why this is not a stable root identity: release_task detaches the
PID, free_pid removes its numeric ID from the namespace IDR before its RCU
release, and put_task_struct_rcu_user can defer the final task put. The render
free hook runs only in __put_task_struct, before free_task. Thus a PID can
refer to a different task while the registered old task is still retained.
After final free, g_inited_pid still retains its old numeric value as well.

On r44, a same-number lookup of a replacement task passes the initializer's
numeric check and can reactivate the old queues without reset. The getter
can likewise use that replacement as the root for feedback/reset. This is a
source-proven identity defect, not a PID-reuse failure observed on the device.
Just clearing g_inited_pid in the final free hook would not close the window
before that hook. Pointer equality alone would miss a reused slab address
after final free unless the stored entry's retirement flag is also checked.

Non-force init and get now require the lookup-pinned task to match the first
global queue's root pointer and its existing exited flag to remain clear,
under g_wake_queue_rwlock. The flag marks final task-structure retirement,
not a new exit_state/zombie or scheduling-liveness policy. The predicate
reads only static queue storage and compares the pointer value; it does not
dereference a freed task or retain a new long-lived task reference. Failed
resume keeps the existing EBUSY and does not publish the active pointer;
failed get keeps ENOENT and does not feed back or rotate/reset queues.

Pause/destroy retains the registered root storage, so a matching unretired
task can still resume without reset when the active pointer is NULL. Force
init continues to replace the target and uses r44's complete old-depth
retirement. No new UID/caller gate, feature, ioctl payload, PID conversion or
queue policy is introduced. The destroy request's legacy numeric-PID guard
remains unchanged; this is not a general PID-handle guarantee for delayed
userspace destroy requests across a later force replacement.

The wakeup and final-free hooks also select the root by task pointer, not
task->pid versus the stored userspace numeric token. Both selections remain
under the same lock. Wakeup already requires an active root and uses a direct
pointer comparison without another flag read on its hot path. Final free
marks the existing queue entries exited before the task memory is freed;
r44 retirement clears replaced tasks' memberships under that lock. Kernel
TID exchange also demonstrates that a task's numeric PID is not its identity.

Diff/checkpatch and frame-enabled render_rt.o compilation pass without a
diagnostic in the modified file. Optimized ThinLTO IR contains both the
pointer/retirement tests before resume publication and getter feedback/reset.
No implementation-mirroring test or system-queue takeover probe is added.
Device remains r44 at this source-check stage; full link, matched pairing
and installed validation follow separately. Positive global lifecycle,
matching caller behavior and scheduling/power acceptance remain open.

### r45 Installed Root-Identity Repair

The full #45 link from 782c4f17e includes the pointer/retirement checks before
non-force resume publication and getter feedback/reset in the final ELF.
The wake/free hook machine code compares the root pointer to current/the
freeing task, rather than comparing numeric PID fields. The modified file
has no compiler diagnostic. Configuration, the entire export/CRC table and
signing key/certificate are byte-identical to r44; no task/Binder layout,
feature gate, ioctl payload or caller-policy change is introduced. The
matching modem has 390 present/matching imports; PKCS#7, certificate inclusion,
ext4 integrity and label/owner/mode readback pass. Boot kernel round-trip,
original DT/firmware/header preservation and real partition bounds pass.

The r44 device partitions match the retained rollback pair before flashing.
Supported serial/model/unlock checks confirm the same Allen bootloader;
unsupported product/all queries do not substitute for identity or bounds.
Both r45 flashes succeed, exact image-length paired readbacks match and the
independent ramdisk hash remains unchanged through a subsequent normal reboot.
Two changed-boot-ID normal startups reach boot completion with #45. Complete
early logs have no WARN, BUG, CFI failure, panic or Oops; the classified
unconditional wiphy stack remains. SELinux stays Enforcing, taint 4,
sig_enforce Y and PRMEM enabled, with the matching modem Live. The ordinary
framework still enables RTG after reboot.

These installed pairing/startup checks do not reproduce numeric PID/slab
reuse or certify positive global queue/exit/resume behavior. No unknown
system queue is replaced and no new diagnostic/permission bypass is added.
Previously passed independent group/native/compat/MIPS tests are not
needlessly replayed for the isolated global identity selection change.
The legacy destroy token, actual caller lifecycle, timer expiry and full
frame scheduling/performance acceptance remain separate open gates.

A follow-up read-only check of the system_server-loaded libhwrme_jni.so and
libhwrme_client.so verifies each copy against its exact device digest. The
JNI adapter links libhwrme_client, imports ResMngMmEvent and registers
android/hwrme/HwResMngEngine.nativeHwResMngMmEvent with signature (II)V.
PriorityAdmin::InitRenderPid at 0x9df8 constructs 0x40087811 at 0x9e20/0x9e28;
its ioctl call is at 0x9e30. GetRenderRelatedTid constructs 0xc2007812 at
0xb2c0/0xb2cc and calls ioctl at 0xb2d0. These are the common group commands
17/18, not the independent global commands 44/45/46. This is scoped loaded
adapter/client evidence, not execution of the JNI callback, an all-library
indirect-call proof, a permission proof or evidence that global state is idle.

r44 is the immediate paired rollback. Original backups, r17 and the latest
r41/r42/r43/r44/r45 pairs remain. Consumed library/disassembly/IR copies,
full logs and other task-owned artifacts are removed; unrelated diagnostics
are preserved. Workspace stays below 20 GB; whole bring-up remains open.

### Boost Restart And Timeout Notification Ordering

The complete current-source call search finds one timer-start caller:
ctrl_rtg_boost, reached through CMD_ID_ENABLE_RTG_BOOST (0xc014ab64).
The enabled proc dispatcher checks the existing RTG feature/lifecycle gate;
the handler accepts duration 1-5000 milliseconds and min_util 1-1024.
start_boost_timer converts milliseconds with msecs_to_jiffies, boosts the
default and configured multi-frame groups, AUX and configured boost threads,
and arms the timer. Its callback only records expiry and wakes frame_sched;
the worker clears those boost floors and disables groups without an ongoing
frame-frequency request. Ordinary SET_MIN_UTIL and SET_MIN_UTIL_AND_MARGIN
operate on the current task's frame group with is_boost=false; neither starts
this timer. Successful ordinary frame calls cannot certify timer expiry.

Read-only copies of the system_server-loaded libhwrmeRtgSched.so,
libiAwareRtgSched.so and libhwrme_client.so match their exact device digests.
The first links libhwrme_client and has no direct ioctl PLT call in the
examined disassembly. The examined libiAwareRtgSched SetFrameMinUtil site
constructs 0xc010ab1e, and EndFreq constructs 0xc010ab18 before forwarding to
SetCommandFreq. No low-half 0xab64 MOV/MOVK immediate is located in these
three text dumps. This bounded immediate/site inspection is not an all-caller,
indirect-call, execution or permission proof. The previously examined
application/system_server policy lists do not grant the boost command; no
permission widening or injected global boost is used to claim production
timer coverage.

Source ordering and the accepted r45 optimized object expose two related
restart races. After an old callback clears the wait bit, a new request can
arm a later timer before the worker obtains g_timer_lock. The old worker
then sees enabled/cleared-bit state and clears the newly applied boost before
its deadline. Also, the handler enables frame scheduling before entering
start_boost_timer's mutex; timeout cleanup can disable that state between
activation and the new timer/min-util update.

The small repair moves start_rtg_boost into the existing timer critical
section, after the timer-on check and before the unchanged overlapping-duration
guard. This preserves activation for a shorter ignored request and the
existing duration/util policy. The worker now atomically consumes the wake
bit before checking enabled state and whether a renewed timer is pending.
It skips obsolete cleanup while that timer is pending. There is no final
unconditional bit-set: merely adding a pending check followed by the old
late rearm would lose a real callback that clears the bit between that check
and rearm. A callback after the atomic consume remains queued for the next
worker iteration; a consumed notification while disabled cannot spin on an
uncleared bit. The callback remains mutex-free, preserving del_timer_sync's
lock ordering. No extra thread, timer, reference, duration, ioctl layout or
caller/security policy is introduced.

Diff/checkpatch and both enabled objects pass. Optimized IR contains the
atomic fetch-or, timer-on and timer-pending guards before all cleanup stores,
and activation is inside the timer mutex; the proc handler calls only the
timer entry after its existing argument checks. No branch-mirroring test or
unsafe global injection is added. Device remains r45 at this source-check
stage. Full link/paired installation follow separately; real intended-caller
expiry/restart stress, full frame integration and power/performance acceptance
remain open rather than being replaced by these static checks.

### r46 Installed Boost-Wake Ordering Repair

The full #46 link from ea82efe4b contains the atomic wait-bit consume followed
by enabled/pending-timer checks before cleanup in the final worker machine
code. The timer entry holds g_timer_lock before start_rtg_boost, the unchanged
overlapping-duration guard, boost-floor updates and timer renewal. The full
configuration, entire Module.symvers/export CRC table and signing key/
certificate are byte-identical to r45; neither modified file has a compiler
diagnostic. Inherited unrelated linker/section warnings remain separately
classified, not claimed repaired by this change.

The matching signed modem has all 390 imports present with matching CRCs;
PKCS#7, certificate-in-Image, ext4/owner/mode/label and boot kernel/DT/firmware/
header/bounds checks pass. Before flashing, the retained r45 pair matches the
installed partitions. Standard fastboot product/all queries still report
Command not supported despite their zero host exit status; supported OEM
queries confirm the same model and unlocked bootloader. Both flash result
texts report successful writes, and exact image-length r46 boot/modem
readbacks match. The independent ramdisk hash stays unchanged after both
installation and subsequent normal reboot.

Two changed-boot-ID normal startups reach boot completion with #46. Both
complete early logs have no WARNING, BUG, Oops, panic or CFI failure, with
one already classified unconditional wiphy_new_nm stack per boot. SELinux
stays Enforcing, taint 4, sig_enforce Y and PRMEM enabled; matching modem is
Live, frame_sched exists and the stock framework enables RTG after reboot.
These checks do not execute production boost expiry/restart or certify the
full frame/global lifecycle and power/performance requirements. No test
configuration, global boost injection, permission change or queue takeover
is used, and unrelated passed group/MIPS tests are not replayed.

r45 is the immediate paired rollback. Original backups, r17 and the latest
r42/r43/r44/r45/r46 pairs remain. Superseded r41 images are removed with its
useful configuration/export/signing metadata retained. Consumed task-owned
library/IR/disassembly copies and full logs are removed; unrelated diagnostics
remain. Workspace stays below 20 GB, and whole bring-up acceptance stays open.

### r46 Stock Frame Caller And END_FREQ Verification

This follow-up keeps the installed #46, its source/configuration and the r45
paired rollback unchanged. No kernel patch, build, flash, application install,
global queue takeover, boost injection or protection change is performed.
The purpose is to distinguish real stock frame use from interface availability
and earlier synthetic ownership/compat checks.

Read-only SystemUI and system_server maps contain libiAwareRtgSched.so,
libhwrmeRtgSched.so and libhwrme_client.so. Copies of the first two libraries,
their libhwrme_scene.so backend and the relevant framework DEX containers are
verified against the device files. The native IPC vtable identifies offsets
0x20/0x28 as BeginFrameFreq/EndFrameFreq and 0x40 as EndFreq. The scene backend
uses that EndFreq slot; its IPC implementation constructs 0xc010ab18.
This is scoped installed-library evidence, not an all-caller proof.

The retained kernel contracts remain distinct:

- BEGIN_FRAME_FREQ (0xc010ab14) starts the current task's frame-frequency
  state and supplies the frame-start timestamp through update_frame_state.
- END_FRAME_FREQ (0xc010ab15) resets the individual frame timestamp/state;
  it does not clear start_frame_freq or stop the global boost timer.
- END_FREQ (0xc010ab18) validates the current TID, looks up its frame group,
  clears start_frame_freq and calls set_frame_sched_state(false).
- The global boost timer is still started only by the examined 0xc014ab64
  entry. Neither the ordinary frame events nor END_FREQ execute its expiry.

Two bounded lock-screen wake/sleep cycles use a new tracefs instance with
selected rtg_frame_sched counters and SystemUI/RenderThread sched_switch
events. They record ten start and ten end timestamps, changing frame-util
values and 1,132 actual scheduled-in events for those two threads. A short
instance-stack follow-up records four native begin/end pairs through
__arm64_sys_ioctl, proc_rtg_ioctl_locked, ctrl_frame_state,
update_frame_state and set_frame_timestamp. The first conditional stack
trigger records no instance stacks because this source's stacktrace_trigger
calls the global-buffer trace_dump_stack; its consumed counter is not counted
as stack coverage. Per-instance options/stacktrace supplies the later stacks
without enabling the global tracer.

CONFIG_FTRACE_SYSCALLS is disabled, but existing raw_syscalls/sys_enter and
sys_exit events are available. A final, separately filtered instance records
native ioctl entry/return pairs from the observed system_server controller
thread and SystemUI main thread, together with their frame-state counters.
It reads no user payload contents and uses the actual proc RTG descriptors.
Every recorded RTG request returns nonnegative, with no unpaired boundary
calls or lost/overwritten events:

| Real operation | Recorded result |
| --- | --- |
| Controller SET_RTG_THREAD (0xc018ab02) | Allocates observed group 10, returning 10 |
| SystemUI BEGIN_FRAME_FREQ | Four requests return 0 |
| SystemUI END_FRAME_FREQ | Five requests return 0, including the final idempotent reset |
| SystemUI END_FREQ | Returns 0 with FRAME_SCHED_ENABLE_10 becoming 0 inside the same ioctl |
| Controller SET_RTG_ATTR (0xc018ab04) | Returns 0 |
| Controller SET_RTG_THREAD clear | Returns 0 after the observed END_FREQ |

Group 10 is the observed live allocation, not a new hard-coded platform
constant. This confirms bounded real native frame registration, begin/end,
stop and member-clear behavior. No test process replaces the system group.
It does not certify every application, 32-bit production frame integration,
performance, frame pacing, long-term power or the independent global queue.

Priority interpretation remains a separate gate. sched_switch records the
actual internal task->prio, including 87 and 89 during these windows. With
RT_PRIO_EXTEND_VIP and VIP_PRIO_WIDTH=10, is_vip_prio covers 89-98: 87 lies
outside that VIP band, whereas 89 is a VIP priority. The second cycle still
has internal priority 89 after its frame-disable counter. The subsequent
idle proc read reports policy 0, UI priority 120 and RenderThread priority
116, but sched/debug.c can display get_raw_kernel_priority for VIP/PI tasks.
That read is not proof that the internal effective priority has reverted.
Repeated FRAME_SCHED_ENABLE counters also include per-task update sites;
they are not independent group activations or a scheduler transaction boundary.
The post-stop VIP vote/Binder/native-PI owner must be traced before attributing
it to a cleanup defect or declaring priority restoration complete. No forced
priority reset is added.

All capture buffers have zero overruns, commit overruns and dropped events.
Kernel logs cover the complete capture interval without WARNING, BUG, Oops,
panic or CFI failure. The boot ID stays unchanged, SELinux remains Enforcing,
taint 4 and module sig_enforce Y. New instances are removed; the existing
tracer/instance state, keyguard, radios and settings modes are preserved.
Automatic brightness legitimately changes its stored output from 33 to 4
at observed zero ambient lux; no brightness setting is written or forcibly
restored. The display returns to Asleep. Task-owned copied libraries/DEX,
full traces/logs and diagnostic scripts are removed after recording evidence.

The next smallest audit is the post-stop VIP/PI vote source. Intended boost
expiry/renewal, positive global lifecycle, complete frame scheduling/power
and the existing manual/hardware/source prerequisites remain open. The
installed pair and retained images are unchanged; whole bring-up is not done.

### r47 Frame Stop And Member-Clear Race

The follow-up attributes the unexplained post-stop priority on unchanged r46
before changing scheduler policy. A first independent lock-screen cycle
observes ordinary restoration after END_FREQ. Its short RenderThread VIP
boost is type 2 (VIP_BOOST_MUTEX), requested by the existing gpu-work-server
and cleared by the mutex owner. That separate donation is not evidence of
frame priority leakage and is not forcibly reset.

A subsequent two-cycle window reproduces the open failure. The main instance
records 6,446 events; a separately filtered instance records 14 priority events
and their 14 call stacks. Both have zero overruns, commit overruns and dropped
events. All 2,137 selected native ioctl entries have paired returns. Task
selection uses process ownership rather than ambiguous truncated thread names.
No test app, priority injection, queue takeover or protection change is used.

The important failing order is:

1. The system controller updates the still-active frame group's priority.
   set_vip_prio requests level 10 for both UI and RenderThread, returning 0.
   The stacks identify parse_rtg_attr -> set_frame_prio ->
   do_update_frame_task_prio -> sched_rtg_set_task_scheduler; this is a normal
   frame VIP assignment, not a cgroup vote or mutex/native-PI donation.
2. SystemUI END_FREQ writes FRAME_SCHED_ENABLE to zero before acquiring the
   frame-info writer lock. The controller concurrently clears the group.
3. do_update_thread sees the zero state and skips restoring the old members,
   then releases their references and clears the member pointers. END_FREQ
   subsequently acquires the lock but has no members left to restore.
4. END_FREQ returns 0, yet both threads are scheduled at internal priority 89
   about 14.7 seconds later. The observed CPU group remains /vip with its
   configured VIP vote zero throughout the sampled window.

The other cycle clears members before the late end requests, which return the
existing FRAME_ERR_PID (-100) with no attached frame group. That race ordering
restores the members through the controller. It is distinct from the successful
END_FREQ that misses restoration; the missing-group error contract is retained.
Both complete capture intervals remain free of WARNING, BUG, Oops, panic and
CFI failure, with unchanged boot ID/security/settings and the display restored
to Asleep. Automatic brightness output changes normally; it is not overwritten.

r47 moves the existing frame-info writer lock ahead of the enable-state check
and update in set_frame_sched_state. The state change and per-member scheduler
restoration now share the same critical section as priority/member updates.
The no-change path still returns without resetting a group. The member-update
thread counter reset also moves inside that existing lock. There is no new
lock, task reference, task layout, ioctl ABI, priority policy or ownership rule.
Normal scheduler restoration continues using the existing PI-aware setter;
this is not a blanket reset of all VIP/PI activity.

The inspected group-util path already follows the frame-info -> RTG-group
lock order for member updates. The tick callback does not acquire the
frame-info lock, and the frequency-update callback does not call its priority
getter. The global lifecycle gate still drains ioctl users before resetting
frame-info locks. No inverse lock order is introduced in this examined closure.

Strict checkpatch and targeted object compilation pass. The #47 full build
passes; configuration, complete export/CRC table, kernel release and signing
material match r46. The final ELF acquires the writer lock before inspecting
or storing the state and before the member-update counter reset. Both produced
modules pass versioned-import checks (390 modem imports and two dummy imports).
The staged modem PKCS#7 signature, embedded certificate, filesystem/label
readback and boot kernel round-trip/OEM-container checks pass. r46 metadata
and its boot/modem pair are retained as the immediate rollback.

Paired installation passes supported serial/model/unlock checks and reaches
normal Android on #47. Exact image-length boot/modem readbacks match and the
full independent ramdisk is unchanged. The full early kernel log contains no
actual WARN, BUG, Oops, panic or CFI failure; exception matching uses token
boundaries rather than substrings in unrelated notice text. SELinux remains
Enforcing, signature enforcement is Y and taint remains 4; configuration
retains enforcing CFI and PRMEM. The first successful startup is not repeated
without a new fault or unmet requirement.

Two real lock-screen cycles use the same bounded conditions as the failing
r46 window: four seconds awake per cycle, six seconds between them and an
18-second final sleep interval. The main trace records 6,025 events, with all
1,952 selected ioctl entries paired. All 28 recorded RTG requests return 0.
The separate narrow instance records 20 priority events and their 20 stacks,
including four per-member restoration paths through set_frame_sched_state.
Both buffers have zero overruns, commit overruns or dropped events.

Both END_FREQ calls return 0 with UI and RenderThread NORMAL policy changes
completed inside the ioctl. In the first cycle the controller is finishing
the frame VIP update when END_FREQ enters; restoration changes both actual
priorities from 89 to 110 before controller member-clear counters appear.
Thus the original overlapping operations are observed in the corrected
order, rather than treating a zero enable counter as proof of restoration.
The first cycle subsequently schedules UI at ordinary priorities 110/120
and RenderThread at 116, not 89. The second call restores both from 87 to 110;
RenderThread does not run again in its tail window, so no extra scheduled-in
sample is claimed for that dormant thread.

Independent gpu-work-server donations are still observed: one mutex vote
(type 2) and one rwsem vote (type 1) each request level 10, return 0, clear
to zero and return 0. Their short VIP intervals precede the corresponding
frame stop and are not suppressed. The kernel log covers the whole capture
interval without actual WARN/fatal records. Boot ID/security/settings and
the pre-existing tracer/instances stay unchanged, the display returns to
Asleep and automatic brightness output is not overwritten. No test APK,
global queue takeover, injected global boost or protection change is used.

The reproduced frame shutdown/member-clear race has a bounded native pass.
This does not certify all VIP/PI restoration, 32-bit production integration,
frame pacing, performance or power. A separate source follow-up remains for
the generic mutex VIP vote: the last cancelled waiter removes mutex flags,
whereas vote clearing is in the slow unlock path. Its cancellation/fast-unlock
and shared per-type vote contract must be audited before changing cleanup;
it is not the source of the reproduced frame failure and r47 does not patch it.
Intended boost expiry/renewal, positive global lifecycle and the existing
manual/hardware/source prerequisites also remain open.

r46 remains the immediate paired rollback. Original backups, accepted r17
and the latest r43/r44/r45/r46/r47 pairs remain; superseded r42 images are
removed with useful metadata retained. Consumed task-owned traces, complete
logs, disassembly/IR and diagnostic scripts are removed. Workspace remains
below 20 GB and whole bring-up acceptance is not complete.
