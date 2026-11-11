# MIPS L3/DDR Multi-Level Voting

## Installed Baseline And Failure

The installed r37 kernel remains the device baseline during this source
iteration. No DT, calibration, voltage, ramdisk or user data changes are
needed to recover the factory voting contract.

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

The recovered `_text` address, `0xffffffc010000000`, is independently checked
against the raw Image and the exact `linux_banner` bytes. Recovery provides
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

The factory also has memory-frequency scaling feedback not present in the
published source. This is a separate performance-contract gap, not restored
by the table/selector repair. Do not claim complete OEM scheduling parity.

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
  and is not mixed into this iteration; r37 linked/configuration/signing
  metadata and paired rollback images are retained.

The superseded unflashed #40 pair is discarded after #41 checks pass.
Only r17 and the latest r31/r32/r36/r37/r41 pairs are retained; r37 remains
installed and is the paired rollback for this candidate.

Next: recheck the installed r37 pair and device identity before installing
the #41 pair, with the independent ramdisk unchanged. After installation:
verify all six full maps and userspace ratio grammar, then observe genuine
votes/level selection with a bounded recoverable workload, unchanged thermal
protection and a normal reboot. Initial max votes or sysfs presence alone
are not acceptance. Performance/power equivalence and the separate feedback
gap remain open until supported by evidence.
