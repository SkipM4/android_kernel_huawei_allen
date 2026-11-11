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
