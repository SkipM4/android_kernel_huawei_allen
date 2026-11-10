# Clang 12 CFI Bring-Up Iterations

Baseline: official ALN HarmonyOS 4.2 Linux 5.10.43 sources, Android Clang
12.0.7 r416183b1, the stock-compat ThinLTO configuration and serial
`FMR0223C20002621`. Standard CFI and forced signed-module loading remain
enforcing; the independent Magisk ramdisk is not changed.

## r3: MAS Request Completion (2026-11-10)

The r2 failure and verified original-image rollback are recorded in
`docs/clang12-r2-flash-test.md`.

Change only the proven MAS request-completion registration:

- Add `mas_blk_busyidle_set_end_io` in
  `platform_source/basicplatform/block/mas_blk_busy_idle_interface.c` and its
  declaration in the corresponding platform header.
- Replace the raw callback-address store in `mas_blk_busy_idle_notify.S` with
  a direct call to this compiled C helper. Saved callee registers and the
  existing completion/bookkeeping behavior are preserved.

This uses the standard Android noncanonical-CFI model already selected by the
official kernel Makefile. It does not change CFI flags or bypass its checks.
Final disassembly confirms that C sets `rq->end_io` to
`__cfi_mas_blk_busyidle_end_rq.cfi_jt`, not the implementation address. The
helper uses the compiled request layout rather than another hardcoded offset.

Full incremental kernel/module compilation succeeds. The matching module has
390 versioned imports, no missing exports and no CRC mismatches. Its SHA-512
CMS signature verifies against the local certificate embedded in this kernel.
Boot repack round-trip/header/stub/DTB checks and signed-module/ext4/xattr
readback pass.

Test pair:

- `images/test/allen-boot-test-5.10.43-clang12-r3.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r3.img`.
- Matching signed module: `images/test/allen-modem_driver-test-5.10.43-clang12-r3.ko`.

After Android identity, partition-capacity and original-backup checks, fastboot
model/unlock checks pass through the same Huawei OEM read-only commands as r2.
Both writes return OKAY with no intervening reboot. The built kernel identifies
as `#3 SMP PREEMPT Tue Nov 10 18:56:38 CST 2026`.

r3 still does not reach Android/ADB and returns to fastboot. Engineering
recovery's current console pstore identifies `5.10.43 #3` and:

```text
[14.206777s] Kernel panic - not syncing: CFI failure (target: __cfi_ufs_mq_sync_burst_check_timer_expire+0x0/0x20)
call_timer_fn -> __run_timers -> run_timer_softirq
```

The r2 completion target is no longer the observed failure. The new concrete
failure is the MAS/UFS scheduler's sync-burst timer: its published assembly
passes the raw wrapper address directly to `init_timer_key`. Restrict the next
change to this timer-registration boundary, materializing its callback in C
while preserving flags and lockdep arguments. Do not batch-convert untested
callbacks or disable CFI.

Both original boot and modem_driver images are restored before reboot. Normal
Android, boot completion, original August 5 BiSheng kernel, loaded stock modem
module and SELinux Enforcing are verified. Complete partition readbacks match
both originals and the unchanged Magisk ramdisk backup. Full build/diagnostic
logs are temporary; only the failure evidence above is retained in Git.

## r4: MAS/UFS Sync-Burst Timer (2026-11-10)

Add `mas_ufs_mq_sync_burst_timer_init` in the existing ioscheduler C interface
and its platform-header declaration. Only the failing sync-burst timer's
assembly initialization is redirected to this C helper. It calls
`init_timer_key` with the original zero flags and NULL name/key arguments.
Other timer registrations are not changed speculatively.

Full incremental compilation succeeds. Final disassembly confirms the callback
argument is `__cfi_ufs_mq_sync_burst_check_timer_expire.cfi_jt`. The new reusable
`scripts/stage-allen-modem.sh` stages and validates the matching signed modem
partition; ABI, cryptographic signature, filesystem/metadata readback and boot
repack checks pass. Standard CFI and module-signing enforcement remain enabled.

Test pair:

- `images/test/allen-boot-test-5.10.43-clang12-r4.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r4.img`.
- Matching signed module: `images/test/allen-modem_driver-test-5.10.43-clang12-r4.ko`.

Device/partition/original-backup checks and fastboot model/unlock confirmation
pass. Both flashes succeed without an intervening reboot. Kernel build is
`#4 SMP PREEMPT Tue Nov 10 19:35:32 CST 2026`.

r4 returns to fastboot without reaching Android/ADB. Current console pstore
from engineering recovery identifies `5.10.43 #4` and:

```text
[14.278937s] Kernel panic - not syncing: CFI failure (target: __cfi__ufs_mq_complete_request_remote+0x0/0x1c)
flush_smp_call_function_queue -> ipi_handler
Comm: fsck_boost
```

The new failing address is a remote I/O completion IPI callback, not the r3
sync-burst timer target. The same ioscheduler assembly directly assigns its
raw wrapper address to `rq->csd.func` before
`smp_call_function_single_async`. Next move only that CSD initialization to
compiled C; retain the original request data, zero flags and destination-CPU
selection. Preserve/reload any caller-saved registers needed after the new
helper call. No global CFI relaxation is justified by these results.

Both originals are restored, normal Android returns with boot completion,
the August 5 BiSheng kernel, loaded stock module and SELinux Enforcing.
Byte-for-byte readbacks confirm original boot/modem_driver and unchanged
Magisk ramdisk. Temporary full logs are removed after documenting the evidence.

## r5: MAS/UFS Remote Completion CSD (2026-11-10)

Add `mas_ufs_mq_init_remote_csd` to the existing ioscheduler C interface and
declare it in its platform header. It initializes the callback, request data
and zero flags exactly as the existing `blk_mq_complete_request_remote` in
`block/blk-mq.c`. Redirect only the proven raw assembly CSD initialization.
Reload the caller-saved request-context pointer after the new C call; retain
the original online-CPU check and `smp_call_function_single_async` destination.

Compilation succeeds. Final disassembly verifies the CSD field offsets match
the published assembly layout and `rq->csd.func` uses
`__cfi__ufs_mq_complete_request_remote.cfi_jt`. Boot repack and matching signed
module staging checks pass, with 390 imports, no missing exports and no CRC
mismatches. CFI, signature enforcement and SELinux are not relaxed.

Test pair:

- `images/test/allen-boot-test-5.10.43-clang12-r5.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r5.img`.
- Matching signed module: `images/test/allen-modem_driver-test-5.10.43-clang12-r5.ko`.

Identity, partition bounds, original backups and fastboot unlock/model checks
pass; both candidate flashes succeed. Build is
`#5 SMP PREEMPT Tue Nov 10 20:24:04 CST 2026`. Android/ADB does not start; the
device returns to fastboot. Recovery console pstore identifies `5.10.43 #5`:

```text
[14.391978s] Kernel panic - not syncing: CFI failure (target: delayed_work_timer_fn+0x0/0x2c)
call_timer_fn -> __run_timers -> run_timer_softirq
```

The remote-completion CFI target is no longer the observed failure. The new
target is the standard delayed-work timer callback: MAS ioscheduler, flush,
busy/idle and background-operation assembly contains raw references to this
exported symbol. Rewriting each initialization site would be unnecessary
complexity. The existing kernel `__cficanonical` attribute is already used by
initcall stubs to make assembly-visible symbols valid CFI entry points.
Next apply it only to this proven failing callback and inspect the linked
symbol before testing; this preserves CFI type checks and the workqueue body.

Both original partitions are restored and verified byte-for-byte. Normal
Android boot, original BiSheng kernel, loaded stock modem module and Enforcing
SELinux are confirmed; the Magisk ramdisk readback is unchanged. Full logs are
removed after retaining this evidence.

## r6: Canonical Delayed-Work Timer Entry (2026-11-10)

Apply the existing `__cficanonical` attribute only to
`delayed_work_timer_fn` in `kernel/workqueue.c`. Its body, timer argument and
export stay unchanged. The attribute is defined by `compiler-clang.h` and is
already used with `__ADDRESSABLE` by the kernel's initcall stubs when assembly
takes a symbol address. Unlike `__nocfi`, it provides a valid canonical CFI
entry; it does not disable type checks. This exported function is already
addressable, so no new address-keeping helper or assembly rewrite is needed.

Compilation succeeds. Final disassembly shows `delayed_work_timer_fn` as a
BTI/jump-table entry branching to the unchanged `delayed_work_timer_fn.cfi`
body. Published MAS assembly now resolves its raw references to that checked
entry. Boot repack, the matching module's 390 imports/zero missing exports/zero
CRC mismatches, embedded certificate, CMS signature and ext4/metadata
readback checks all pass.

Test pair:

- `images/test/allen-boot-test-5.10.43-clang12-r6.img`.
- `images/test/allen-modem_driver-test-5.10.43-clang12-r6.img`.
- Matching signed module: `images/test/allen-modem_driver-test-5.10.43-clang12-r6.ko`.

Device identity, target capacities, original backups and fastboot model/unlock
are verified. Both writes succeed without an intervening reboot. Kernel build
is `#6 SMP PREEMPT Tue Nov 10 20:56:52 CST 2026`.

r6 still does not reach Android/ADB and returns to fastboot. Current console
and dmesg pstore records both identify `5.10.43 #6`. Inspecting the first panic,
not just the end of the recursive trace, establishes two distinct failures:

```text
[14.390300s] CFI failure (target: __cfi_mas_blk_idle_notify_work+0x0/0x1c)
Workqueue: busyidle_notify; process_one_work -> worker_thread
[15.390935s] CFI failure (target: __cfi_mas_blk_panic_notify+0x0/0x1c)
atomic_notifier_call_chain -> panic
```

The first fault is the busy/idle work callback; the panic-notifier fault occurs
while reporting it and recurs until platform crash capture. The delayed-work
timer target is no longer the observed failure. Both new targets have proven
raw assembly references: the work initializer materializes a symbol address,
and `mas_blk_dump.S` stores the notifier symbol in its static notifier block.
Next apply canonical CFI entries to these two observed callbacks and verify
their linked addresses; retain the actual work and panic-reporting behavior.
Do not mistake the secondary notifier panic for the initiating failure.

Both original partitions are restored and full readbacks match their backups.
Normal Android boot, the original BiSheng kernel, loaded stock modem module and
Enforcing SELinux are verified; the independent Magisk ramdisk is unchanged.
Full logs are removed after extracting this evidence. The five most recent
candidate pairs (r2 through r6) are retained within the 20 GB workspace limit.

## r7: Busy/Idle Work And Panic Notifier (2026-11-10)

Give `__cfi_mas_blk_idle_notify_work` and `__cfi_mas_blk_panic_notify` canonical
entries in the existing busy/idle and dump C interfaces. Use `__ADDRESSABLE`
alongside `__cficanonical`, following the initcall-stub pattern: these functions
are referenced by published assembly, not exported C callback tables. No
function body, work initialization, notifier priority or panic handling is
removed or changed.

Compilation and final disassembly pass: both public symbols are checked BTI
jump-table entries targeting their `.cfi` implementations. Matching-module
ABI/signature/ext4 checks and boot repack checks pass. Test images are the
`allen-boot-test-5.10.43-clang12-r7.img` and
`allen-modem_driver-test-5.10.43-clang12-r7.img` pair in `images/test/`, with
the companion signed r7 module. Original r2 candidates are now outside the
five-version retention window and are removed; original device backups remain.

The usual identity, partition, backup and fastboot unlock/model checks pass;
both flashes succeed. Build is `#7 SMP PREEMPT Tue Nov 10 21:30:53 CST 2026`.
r7 still returns to fastboot without reaching Android. Current recovery pstore
identifies `5.10.43 #7` and contains only one panic:

```text
[14.735973s] CFI failure (target: __cfi_mas_blk_flush_init+0x0/0x1c)
do_boottime_initcall -> do_one_initcall -> do_initcalls
```

Busy/idle work is no longer the initiating fault; the panic notifier no longer
causes the recursive crash sequence. The new failure is a flush initcall whose
published assembly supplies its raw wrapper address through a PREL32 initcall
entry. The compiler's own initcall-stub idiom provides the relevant precedent.

A linked-address audit now establishes the scope of this error class rather
than guessing the next callback: 20 distinct `__cfi_*` C wrapper symbols are
taken by MAS assembly, and only the two r7 targets are canonical entries.
The remaining 18 resolve to raw implementation addresses, including the new
flush-init target. All have existing definitions in MAS C interface files.

This stronger evidence supersedes the earlier plan to wait for each callback
to fail individually. Next add canonical/addressable entries only to these
18 audited MAS assembly-facing wrappers, keeping their prototypes and bodies
unchanged. Recheck all 20 linked entry points before deploying the next pair.
This is a single assembly-to-C CFI ABI repair, not a general compiler flag
change, an untyped whitelist or a CFI bypass; unrelated driver callbacks stay
untouched. A genuinely wrong prototype must still fail CFI and be diagnosed.

Both original partitions are restored and verified byte-for-byte. Normal
Android, original BiSheng kernel, loaded stock module, Enforcing SELinux and
unchanged Magisk ramdisk are confirmed. Full logs and the temporary audit list
are removed after recording these results.

## r8: Audited MAS Assembly-To-C Wrapper ABI (2026-11-10)

Complete the r7 linked-address audit's repair for the remaining 18 wrapper
symbols, in four existing C interfaces: bkops, busy/idle, flush and ioscheduler.
Only `__cficanonical` and `__ADDRESSABLE` are added; return types, parameters,
function bodies and the published assembly stay unchanged. Both branches of
the existing kernel-version conditionals keep equivalent annotations.

Compilation succeeds. Inspecting the actual ELF addresses confirms all 20
assembly-referenced C wrapper symbols are BTI/canonical CFI entries branching
to their corresponding `.cfi` bodies. Name-only disassembly initially omits
the manual-GC proc-show symbol because it shares an address with a CFI type
group alias; address-based inspection resolves that verifier limitation.
No source workaround or weakened gate is used for the alias.

Matching-module ABI/signature/certificate/ext4 checks and boot repack checks
pass. Standard CFI and forced module signatures remain enforcing. The r8 test
pair and signed module use the usual `images/test/allen-*-clang12-r8` paths;
obsolete r3 candidates are removed, preserving the original backups and the
r4-r8 retention window.

Identity, partition bounds, original backups and fastboot unlock/model checks
pass; both writes succeed without an intervening reboot. Kernel build is
`#8 SMP PREEMPT Tue Nov 10 21:52:52 CST 2026`. r8 returns to fastboot without
reaching Android. Current recovery console identifies `5.10.43 #8`, with one
panic:

```text
[14.760819s] CFI failure (target: mas_gear_ctrl_hck_init+0x0/0x26c)
do_boottime_initcall -> do_one_initcall -> do_initcalls
```

The flush-init C wrapper is no longer the failing target. This new initcall is
different: `mas_gear_ctrl_hck_init` itself is an opaque assembly implementation,
with no C body in which to emit CFI metadata. Its local `.init.text` function is
registered directly in `.initcall6.init` by a PREL32 entry.

Next replace only this registration with a standard C `device_initcall`,
exposing the assembly entry for a direct call from the compiler-generated
canonical initcall stub. Build the C registration under the same
`CONFIG_MAS_UFS_GEAR_CTRL` condition and preserve initcall ordering, return
status and the existing hook-registration body. Do not bypass CFI in the core
initcall runner or disable the gear-control subsystem.

Both original partitions are restored and full readbacks match their backups.
Normal Android, original BiSheng kernel, loaded stock modem module, Enforcing
SELinux and unchanged Magisk ramdisk are confirmed. Full temporary logs are
removed after retaining these findings.

## r9: Standard C Initcall For Assembly Gear-Control Init (2026-11-10)

Replace the direct assembly PREL32 initcall registration with a C
`device_initcall(mas_gear_ctrl_hck_init)` in `mas_blk_gear_ctrl_init.c`.
Make the existing assembly function global without changing its body. The
new object follows the original object under `CONFIG_MAS_UFS_GEAR_CTRL`,
preserving level 6, ordering and return status. This follows the existing
standard initcall implementation in `include/linux/init.h`, not a CFI bypass.

Compilation succeeds. Decode the final ELF PREL32 entry and confirm exactly
one gear-control registration, pointing to the compiler-generated canonical
BTI/CFI stub. Its C implementation directly calls the unchanged assembly
function. Boot repack and matching-module ABI, certificate, CMS signature,
ext4 and metadata checks pass; the module still has 390 versioned imports,
zero missing exports and zero CRC mismatches.

The r9 boot/modem_driver pair uses the usual versioned `images/test/` paths.
Obsolete r4 candidates are removed, preserving originals and r5-r9. Device
identity, partition bounds, original readbacks and fastboot model/unlock
checks pass. Both writes succeed without an intervening reboot. Build is
`#9 SMP PREEMPT Tue Nov 10 22:11:16 CST 2026`.

r9 returns to fastboot without reaching Android. Both recovery pstore records
identify build #9; the single fatal panic is now:

```text
[14.853498s] CFI failure (target: __cfi_hisi_page_cache_manage_init+0x0/0x1c)
do_boottime_initcall -> do_one_initcall -> do_initcalls
```

The gear-control initcall is no longer the failing target. The new pagecache
target is an existing C wrapper referenced directly by its assembly initcall,
the same noncanonical assembly-to-C ABI issue already repaired in MAS.
Audit the pagecache assembly's seven wrapper references and repair only that
interface next, keeping standard CFI enforcing and callback bodies intact.
An earlier nonfatal sensorhub warning reports an uninitialized spinlock at
`package_route_tbl+0x68`; retain it as a separate unresolved runtime issue,
not as the cause of this initcall panic.

Both original partitions are restored. Full hashes match the boot/modem_driver
backups and unchanged Magisk ramdisk. Normal Android (`boot_completed=1`),
original BiSheng kernel, loaded stock modem module and Enforcing SELinux are
confirmed. Full temporary logs are removed after retaining these findings.

## r10: Canonical Pagecache Assembly-To-C Entries (2026-11-10)

Add `__cficanonical` and `__ADDRESSABLE` to all seven C wrappers whose raw
addresses are referenced by `pagecache_manage.S`: its init/exit wrappers,
timeout work/timer and proc show/open/write callbacks. Only
`pagecache_manage_interface.c` changes; signatures, bodies and the assembly
remain unchanged. The source precedent remains the standard CFI initcall
stub in `include/linux/init.h`.

Compilation succeeds. Address-based final ELF inspection proves all seven
symbols are canonical BTI entries branching to their matching `.cfi` bodies.
The unique pagecache PREL32 initcall resolves to the canonical init wrapper.
Boot packaging and matching-module ABI/certificate/CMS/ext4/metadata checks
pass. The r10 pair and signed module use the usual `images/test/` paths;
obsolete r5 candidates are removed, preserving originals and r6-r10.

Identity, bounds, backup and fastboot model/unlock checks pass; both writes
succeed. Kernel build is `#10 SMP PREEMPT Tue Nov 10 22:28:01 CST 2026`.
r10 still returns to fastboot without reaching Android. Current recovery
pstore identifies build #10, with one fatal panic:

```text
[14.751306s] CFI failure (target: hyperframe_init+0x0/0x85c)
do_boottime_initcall -> do_one_initcall -> do_initcalls
```

The pagecache initcall is no longer the failing target. The new target is a
pure assembly implementation registered through its own raw PREL32 entry,
like the r9 gear-control failure. A read-only source/link audit finds this
is the last raw assembly-body initcall in the current tree; the remaining
assembly initcall entries target the repaired flush/pagecache C wrappers.
Next provide a standard C initcall stub for hyperframe under its existing
configuration, preserving its level, object ordering and initialization body.
Its assembly also registers opaque hook/work callbacks without CFI metadata;
record those as a separate ABI audit rather than bypassing the initcall check.
The previously recorded nonfatal sensorhub spinlock warning remains present.

Recovery full partition hashes match the tested r10 boot/modem_driver pair
and unchanged Magisk ramdisk. Both original partitions are subsequently
restored and their full hashes verified. Normal Android, original BiSheng
kernel, loaded stock modem module and Enforcing SELinux are confirmed.
Processed full logs are removed after recording these conclusions.

## r11: Standard C Initcall For Assembly Hyperframe Init (2026-11-10)

Replace hyperframe's raw assembly PREL32 registration with
`device_initcall(hyperframe_init)` in a new C registration object immediately
following `hyperframe.o` under `CONFIG_HYPERFRAME`. Make the assembly entry
global; retain its body, return value and initialization order. The standard
initcall macro supplies enforcing CFI metadata without annotating its runner
as `__nocfi` or disabling the subsystem.

Compilation succeeds. Final ELF decoding confirms a unique hyperframe
registration pointing to a canonical CFI stub whose C body directly calls
the original assembly initialization. Boot repack and matching modem ABI,
certificate, CMS signature, ext4 and metadata checks pass. The r11 pair and
signed module use the usual versioned paths. Obsolete r6 candidates are
removed; originals and the five recent pairs r7-r11 remain.

Identity, partition bounds, backups and fastboot model/unlock checks pass.
Both writes succeed. Build is `#11 SMP PREEMPT Tue Nov 10 22:38:54 CST 2026`.
The new kernel passes the initcall stage and reaches PID 1 running Android
`init`, but still returns to fastboot before Android/ADB is ready. Recovery
pstore identifies build #11 and the initiating panic:

```text
[15.027363s] CFI failure (target: hck_mas_queue_busy_change_gear+0x0/0x40)
init -> pread64 -> read_pages -> scsi_queue_rq -> vendor hook
```

This is an opaque gear-control assembly hook callback, not a remaining
initcall failure. Its published registration takes the raw assembly address.
The same object registers three such hook callbacks plus `mas_gear_ctrl_work`
as a work callback; all four linked addresses lack compiler CFI metadata.
Next add typed canonical C wrappers for those four audited callback entries,
calling separately named global assembly bodies directly. Use the hook
prototypes in `hck_mas_ufs_gear_ctrl.h` and the standard work callback type;
preserve the existing registration, function bodies and callback checks.
The nonfatal sensorhub spinlock warning remains a separate unresolved issue.

Recovery full hashes match the r11 test pair and unchanged Magisk ramdisk.
Both originals are restored and full hashes verified. Normal stock Android,
original BiSheng kernel, loaded stock modem module and Enforcing SELinux are
confirmed; processed full logs are removed after recording the evidence.

## r12: Typed Entries For Opaque Gear-Control Callbacks (2026-11-10)

Add `mas_blk_gear_ctrl_interface.c` with four canonical/addressable C wrappers:
the three audited vendor-hook callbacks and `mas_gear_ctrl_work`. Each calls
the corresponding global `_asm` implementation directly. Rename only the
assembly definitions/type/size directives; leave registration references
pointing to the original names, which now resolve to the CFI entries. Build
the interface under the existing gear-control configuration. Hook signatures
come from `hck_mas_ufs_gear_ctrl.h`; the work callback uses the standard
`struct work_struct *` signature.

Source body comparison confirms all four assembly implementations are
unchanged. Compilation succeeds, and final ELF inspection proves each
public name is a canonical BTI entry leading to its typed `.cfi` wrapper,
which directly calls the correct preserved assembly body. Boot packaging and
matching modem ABI/certificate/CMS/ext4/metadata checks pass. The r12 pair
and signed module use the usual versioned paths; obsolete r7 candidates are
removed, retaining originals and r8-r12.

Identity, partition bounds, backups and fastboot model/unlock checks pass;
both writes succeed. Build is `#12 SMP PREEMPT Tue Nov 10 22:53:14 CST 2026`.
r12 passes the previous gear-control hook failure but still returns to
fastboot before Android/ADB becomes available. Current recovery pstore
identifies build #12 and one initiating panic:

```text
[15.755251s] CFI failure (target: mpage_end_io+0x0/0x1b4)
lo_complete_rq -> blk_mq_end_request -> blk_update_request -> bio_endio
```

This is a BIO-completion ABI failure, not a gear-control or sensor-driver
panic; the interrupt happens to interrupt a sensor work item. The target is
the global ext4 `mpage_end_io` in `fs/ext4/readpage.c`, exposed under
`CONFIG_MM_PAGECACHE_HELPER`. Do not confuse it with the unrelated static
function of the same name in `fs/mpage.c`. Pagecache assembly stores its raw
global address in `bio->bi_end_io`; the final ELF has a separate noncanonical
`.cfi_jt` entry. Next canonicalize only this ext4 completion callback, keeping
decryption/verity post-processing and normal read completion unchanged.
The nonfatal sensorhub spinlock warning remains unresolved separately.

Recovery partition hashes match the r12 pair and unchanged Magisk ramdisk.
Both originals are restored and full hashes verified. Stock Android reaches
`boot_completed=1` with the original BiSheng kernel, loaded stock modem
module and Enforcing SELinux. Processed full logs are removed after retaining
these conclusions.

## r13: Canonical Ext4 Multipage BIO Completion (2026-11-10)

Add `__cficanonical` and `__ADDRESSABLE` only to the ext4 `mpage_end_io`
definition in `fs/ext4/readpage.c`. Preserve its complete body, including
decryption/verity post-read processing and normal completion. The unrelated
static function in `fs/mpage.c` stays untouched. This supplies a typed CFI
entry for the raw address stored by pagecache assembly; it does not suppress
BIO completion checks.

Compilation succeeds. Final ELF inspection confirms the global ext4 symbol
is a BTI entry branching to its corresponding `.cfi` body. Boot repack and
matching modem ABI/certificate/CMS/ext4/metadata checks pass. The r13 pair and
signed module use the usual versioned paths; obsolete r8 candidates are
removed, preserving originals and r9-r13. Identity, partition bounds, backups
and fastboot model/unlock checks pass; both writes succeed. Build is
`#13 SMP PREEMPT Tue Nov 10 23:06:01 CST 2026`.

r13 still returns to fastboot without Android/ADB. Current console pstore
contains build #13, but the initiating fault is overwritten by a repeated
fault-reporting sequence on CPU 4, PID 34, `ksoftirqd/4`:

```text
[15.804063s] NULL pointer dereference at virtual address 0000000000000058
ESR=0x96000005, current-EL data abort, read access
[15.807857s] secondary CPU 1 Oops identifies 5.10.43 #13
pc: do_ptrauth_fault; lr: el1_fpac
```

Do not identify the secondary pointer-authentication Oops as the initiating
fault, or infer that all CFI failures are gone from this truncated console.
The dmesg pstore is stale: its timestamp and embedded build identify r12,
including the old mpage CFI panic. It is not r13 test evidence. Read-only
rrecord inspection finds old stock/bootloader records but no build #13 or
current fault; bootfail_info has no usable current record. No partition is
erased or reformatted to obtain diagnostics.

Source and final disassembly identify a concrete recursive-reporting hazard
in `arch/arm64/mm/fault.c:show_pte`: for a TTBR0 address it checks
`current->active_mm` only against `init_mm`, then loads `mm->pgd` at offset
0x58 without checking NULL. This is consistent with the repeated address,
but does not yet prove the original exception or why active_mm was absent.
Next make only that diagnostic page-table dump tolerate a NULL active_mm,
report the missing context and return to the normal fatal Oops path. This
must preserve the original fault and register report, not turn it into a
recoverable access or bypass CFI/PAC. Reflash a new diagnostic pair to recover
the primary PC/LR/backtrace before choosing any driver or memory fix.

Full recovery hashes match the r13 test pair and unchanged Magisk ramdisk.
Both originals are restored and their full hashes verified. Normal Android
(`boot_completed=1`), original BiSheng kernel, loaded stock modem module and
Enforcing SELinux are confirmed. Current best confidently attributed progress
remains r12 past the gear-control hook; preserve it if a later retention
window would otherwise exclude it. Processed full logs/partition snapshots
are removed after retaining these findings.
