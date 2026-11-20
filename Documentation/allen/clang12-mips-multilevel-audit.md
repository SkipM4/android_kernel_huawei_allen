# MIPS L3/DDR Multi-Level Voting

## Installed Baseline And Failure

The r41 pair is installed and passes normal Android startup and a subsequent
normal reboot. Its paired rollback is r37. The failure analysis uses the
recorded r37 baseline; no DT, calibration, voltage, ramdisk or user data
changes are needed to recover the factory voting contract.

All six live `core-dev-table` properties match the retained original Allen
DT. The published `mips_mem.c` ignores `nr-level` and reads fixed two-column
rows. It does not contain an intentional multi-level rejection gate.

| Monitor | Levels | Real Rows | DT Cells | r37 Result |
| --- | --- | --- | --- | --- |
| DDR c0 | 2 | 2 | 6 | Only first CPU row displayed |
| DDR c1/c2 | 2 | 6 each | 18 each | Only first CPU row displayed |
| L3 c0 | 1, property absent | 8 | 16 | Eight correct rows |
| L3 c1/c2 | 2 | 11 each | 33 each | Probe fails; sysfs object absent |

The DDR cell counts are even, so they pass the old size check. The next
target value, 417, is misread as a CPU frequency and divided by 1000 into a
zero sentinel. The L3 tables have odd cell counts and fail that check. This
is not evidence of measured DDR underclocking: `freq_map` displays stored
targets, not actual device frequency. The scheduler also requires both L3
and DDR monitors, so the missing c1/c2 L3 objects gate their DDR callbacks.

The actual attributes are `freq_map`, `target_ratio`, `mipsmem_enable`,
`ipc_min_sample_time` and `mem_load_window_size_ns`. Userspace enables the
observed monitors and adjusts ratios/sample times; source defaults alone
do not describe the running configuration.

## Factory Implementation Evidence

Reference implementation: the retained original Allen Linux 5.10.43 Image
from `images/extracted/allen-stock-5.10.43/kernel`, not an unrelated platform.
Offline symbol recovery uses `marin-m/vmlinux-to-elf` commit
`6aa8039ae67ccc7f49d019d6b18d016f0cc4af80` (GPL-3). Its temporary Python
environment does not install global host packages or upload the Image.

The recovered `_text` address, `0xffffffc010000000`, is checked against the
raw Image and exact `linux_banner` bytes. Embedded absolute pointers to two
known strings independently confirm this preferred address base; relative
symbol alignment alone would not establish it. Recovery provides
symbols and machine code, not original DWARF or original ELF section metadata.

The factory contract is:

- One CPU-kHz column followed by `nr-level` target-MHz columns; absent level
  property means one level. CPU values become MHz; stored targets become Hz.
- An independent target-ratio table for every level. Within a table, CPU
  thresholds are supplied as kHz and converted to MHz.
- Select a CPU-frequency row independently per level using the same observed
  instruction/refill ratio. Take the maximum selected target across levels.
- Write one scalar MHz vote through the DT-specified bit and write-enable
  mask. Two raw frequencies are not packed into the vote field.
- Initial ratio is 600 for every level. A comma separates independently
  configured tables; a subsequent scalar write configures only level zero,
  with omitted levels set to ratio zero, not copied from level zero.
- `freq_map` writes supply row index, CPU MHz and one target-Hz value per
  level. Single-level writes therefore retain the existing three fields.

At factory `check_mem_freq_change+0x7b4`, address `0xffffffc011be74d4`, the
target pointer is indexed by level. The subsequent compare/select loop
merges the MHz maximum. `set_hw_vote` writes that scalar at
`0xffffffc011be5b8c`. The native `store_target_ratio` loop and its comma,
space and colon literals establish the userspace grammar and omitted-level
behavior; scalar live output from r37 cannot establish the original input.

The DDR votes use DT base `0xffb81000`, offsets `0x278`/`0x27c`; L3 uses
`0xff340000`, offsets `0xe50`/`0xe54`. Each cluster has a DT-selected lower
or upper 15-bit frequency field with a corresponding write-enable bit.
These addresses and masks are evidence, not new driver hardcoding.

## Source Repair

`platform_source/cee/drivers/mips_mem/mips_mem.c` now parses the real row
width, stores per-level targets/ratios, restores comma-separated sysfs
configuration and omitted-level semantics, merges the maximum vote, and
initializes the vote from all final-row targets. The full source path is
under `Code_Opensource/kernel/`.

Map searches are bounded by the real row count. DT and sysfs map inputs
preserve nonzero ascending CPU rows, and target votes must fit the DT field.
Ratio replacement and sysfs readers share the scheduler's existing state
lock; replaced ratio and IPC arrays are freed only after readers release it.
Parsing/allocation happens outside the spinlock. Mapping uses device-managed
I/O lifetime so failed probes do not leak the new early mapping. Shared
policy ownership and publication ordering from a3141ba6c are retained.
The initial maximum vote occurs only after successful kobject registration,
under the same state lock and before policy publication. A failed sysfs
registration therefore does not leave an unowned maximum vote behind.

At this trust boundary, malformed numbers, incomplete/excess columns,
empty ratio levels and nonascending CPU thresholds return errors rather
than partially updating a live table. These invalid-input safety rules do
not reproduce the factory parser's permissive suffix/extra-token handling.

## Counter API Boundary

The factory `get_mips_mem_info` returns an 80-byte raw phase counter vector;
the published helper returns a 40-byte filtered `struct pcounts`. Copying
the raw producer into the existing caller would overwrite its buffer.
The repair keeps the current filtered API and scheduler/task layout.

Both producers merge completed history entries 1 through at most 5.
Instruction/cycle events are 8/17; L3 voting uses refill event 23. DDR uses
event 162 on c0 and 42 on c1/c2. Allen c0 CPU DT nodes are compatible with
`arm,cortex-a53`, matching the existing `test_slow_cpu` selection; the other
clusters do not match it. The current phase event list supplies these events.
For the existing 4 ms sampling window, the published cycle-duration
frequency calculation and factory MHz conversion agree.
An empty cycle count yields IPC zero instead of invoking C division by zero;
this matches the factory ARM64 `udiv` result. If a memory interval contains
no complete MIPS sampling window, the callback skips that incomplete sample
instead of passing a zero-duration window to the filtered producer.

### Normalized Vote-Scale Classification

The factory `set_hw_vote` additionally updates per-CPU normalized vote-scale
bookkeeping, exposed by `mips_mem_get_freq_scale`. This state is absent from
the published source. The writer at `0xffffffc011be5bf4` and getter load at
`0xffffffc011be6d18` reference the field at `0xffffffc012f458f8`, initially
zero in the Image. Its formula is selected vote MHz times 1,024,000 divided
by `policy->cpuinfo.max_freq` in kHz. Original
`cpufreq_frequency_table_cpuinfo` establishes hardware max at policy offset
40; `cpufreq_quick_get_max` instead reads adjustable policy max at offset 56.
The relevant public policy layout agrees; these two maxima are not exchanged.

No direct call/branch, absolute/relative helper pointer, helper ADR or
ADRP+ADD materialization, or plaintext export name is located in the Image.
Known exported names are present. No absolute pointer to the data field is
found either. An affine ARM64 address scan inspects 1,111 neighboring-page
address seeds across 664 whole functions, tracking ADR/ADRP, displacement
arithmetic, register copies, memory references and pointer call/store
candidates. It finds only the known writer and getter and no field-pointer
escape. Positive controls require finding both known instructions.

This is a practical encoded-reference audit, not a formal general alias or
control-flow proof. Within that scope, classify the field as unused original
bookkeeping rather than a demonstrated missing active feedback mechanism.
Do not add unused state/getters or invent a consumer to reproduce symbol
presence. The consumer audit is closed with this classification; complete
OEM performance/power parity is still not certified.

## Validation And Remaining Work

- The exact modified selector functions, compiled with host UBSan, match
  10,952 original ARM64 machine-code executions over all six DT tables,
  row/ratio equality boundaries and varying multi-segment ratio tables.
- The actual modified ratio functions match 189 executions of the original
  native parser, with standard allocator/string/lock calls modeled offline.
  The native parser controls level splitting, conversion and replacement;
  this does not certify physical MMIO or concurrency.
- Sysfs show/write round trips pass; nine malformed boundary inputs are
  rejected. Modified driver object compilation and `git diff --check` pass.
- Initial #40 full link and module checks pass. Final #41 link includes the
  startup/empty-sample corrections and also passes. Configuration, signing
  certificate and complete exported symbol/CRC table are unchanged from r37.
  The #41 boot round trip, retained OEM DTB/firmware stub, partition bounds,
  matching modem's 390 imports, PKCS#7 signature and filesystem/label readback
  checks pass. Original-feature #39 remains unflashed
  and is not mixed into this iteration; r37 export/configuration/signing
  metadata and paired rollback images are retained.

The superseded unflashed #40 pair is discarded after #41 checks pass.
Only r17 and the latest r31/r32/r36/r37/r41 pairs are retained; r41 is now
installed and r37 is its paired rollback.

## r41 Device Validation

Before flashing, the installed r37 pair matches its retained rollback images.
Supported bootloader serial/model/unlock checks confirm the recorded Allen.
Product/all queries still report `Command not supported`, despite fastboot's
zero process status; neither that status nor its reported zero partition size
is accepted as a size/identity result. Bounds come from the confirmed ADB
block-device layout. Both r41 flashes succeed and exact image-length
device-side hashes match boot/modem_driver. The full independent ramdisk hash
is unchanged; the matching modem module is loaded and Live.

All six full maps match the original DT after both boots. Userspace configures
DDR ratios 400/600, 300/600 and 250/500 across c0/c1/c2; L3 uses 1000,
400/600 and 400/800. Commas now expose the second level that the r37 parser
silently discarded. All monitors are enabled, with the observed 4 ms window
and 3 ms minimum sample time. The read-only scheduler MIPS window is 4 ms;
phase counters are enabled for CPUs 0-11 with events 17, 8, 23, 42 and 162.

A freestanding ARM64 user process exercises streaming, cache-resident and
mixed arithmetic/random-read phases, each lasting two seconds, on logical
CPUs 0, 4 and 10. It uses bounded anonymous memory and the supported taskset
mask interface; no CPU governor, vote ratios, thermal settings or global
tracing state are changed. Each trace uses a task-owned isolated instance.
All captures contain start/end markers and zero ring-buffer overruns.
The target CPUs contribute 1,581/1,547/1,576 samples respectively.

All 26,078 recorded L3/DDR selections match the original machine-code oracle,
using the real instruction/refill counts and measured cycle/window estimate.
This includes empty-cycle samples without a warning or divide failure.
The second-level target strictly exceeds the first in the following samples:

| Monitor | Second Level Determines Maximum |
| --- | --- |
| DDR c1 | 780 |
| DDR c2 | 606 |
| L3 c1 | 30 |
| L3 c2 | 238 |

DDR c0's second column is always 417 MHz, equal to the minimum primary
target, so a strict second-level win is not expected there; both 417 and
749 MHz votes are observed. Its full two-column targets still match DT.

There are 4,836 filtered `power:clock_set_rate` events emitted by this driver
after `writel()`, with varying DDR and L3 scalar votes including idle clears.
These prove the live vote-update path is exercised; their state is the
maximum desired per-CPU vote, not a register readback or measurement of
effective hardware clock frequency. Existing DDR devfreq telemetry reports
1531 MHz through its clock provider, but that single snapshot does not prove
load-correlated physical clock transitions or performance/power parity.

All ratios, enable flags, IPC sample times and memory windows remain unchanged
after the workload. Thermal daemon/service remain running; no thermal control
is disabled. The task-owned device binary and trace instance are removed;
the existing wifi trace instance is untouched.

Both full early boot logs and the post-workload log have no WARNING, BUG,
panic or CFI failure. A new boot ID confirms the normal reboot, not a stale
boot-completed property. SELinux remains Enforcing, taint remains 4, PRMEM
remains enabled, the independent ramdisk is unchanged, and all six maps and
enable flags persist. This completes the bounded table/ratio/vote validation.
The separate normalized-scale audit is classified above, without another
kernel change, build or flash; whole-device signoff remains open.

## r42 Scheduler-Activation Regression

r42 enables the audited original frame features after render ownership and
proc/timer lifecycle repair. Its matched modem and task/Binder layout review
are recorded in clang12-rtg-source-audit.md; no stale #39 output is flashed.
After normal reboot and framework configuration, a task-owned isolated trace
captures 2,622 MIPS samples across CPUs 0-11, including 862 with nonzero
instruction counts, and 2,280 post-writel desired-vote events with 19 distinct
name/vote pairs. A short anonymous zero-stream/hash workload provides bounded
activity; no governor, ratio, sensor, thermal or hardware vote-setting write
is introduced. Start/end markers and zero per-CPU overruns pass. All six
monitor maps, ratios, enable flags and sample/window settings are unchanged.
The isolated instance is removed and the existing wifi instance is untouched.

This checks continuing counter/vote operation after scheduler layout changes.
It does not repeat the r41 original-machine-code oracle, certify every frame
load path or measure effective physical clocks/performance/power parity.
r41 remains the paired r42 rollback; security and normal-boot results are
recorded with the RTG iteration rather than inferred from the earlier kernel.
