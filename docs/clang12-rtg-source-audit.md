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
