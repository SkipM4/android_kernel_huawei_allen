# Allen Kernel Bring-Up

Bring up the official ALN HarmonyOS 4.2 Linux 5.10.43 sources on x86_64 Linux,
then move the tested baseline toward Linux stable and Android common/GKI.
Project safety rules are in `AGENTS.md`.

## Current Status

The installed upstream milestone is Linux 5.10.135 #69, with kernel source
`6d6830bed`, normal Android, completed ISP firmware startup and automatic
loading of the same-build signed modem. #68/5.10.130 is the immediate paired
rollback. #65 remains a failed HKIP firmware-boot candidate, not an accepted
runtime. Ordered synchronization
toward the pinned 5.10.274 target continues; Android common parity, vendor/core
separation and GKI certification are not complete. No release is exported or
published.

The upstream track is `upstream/linux-5.10.y`, forked from the
r49 validation record `13570860d`. Its first adjacent stable batch advances
5.10.43 -> 5.10.44: 103 exact picks, four code backports and one release-marker
backport, with 17 already-present fixes and six absent/unselected driver
patches accounted for. A separate DWC3 repair balances IRQ/runtime-PM cleanup
in the Huawei suspended-pullup branch. The 108 imported commits plus this
repair end at `dcb7a62fb`.

Independent Linux 5.10.44 #50 full compilation, final ELF, module imports,
export CRC comparison and signing verification pass. At the user's later
request, the matched boot/modem pair is installed and #50 reaches normal
Android on two boots. Matching partition/module readbacks, unchanged Magisk
ramdisk, live new modem, encrypted user storage and enforcing controls pass.
Bounded USB transfer, direct I/O and real framework frame-stop regression
also pass. r49/5.10.43 is the immediate paired rollback; `bringup/clang12`
retains that baseline. No release export/publication occurs. Later 5.10.y
batches and Android/GKI architecture remain open; this is not full hardware,
power or GKI certification. See `docs/linux-5.10-upstream-sync.md`,
`docs/kernel-module-updates.md` and the complete
`docs/linux-5.10.44-imports.tsv` ledger.

The next source milestone resolves all 298 entries through 5.10.49: 258
exact picks, 14 code adaptations, five version markers, 18 already-present
fixes and three absent/unselected regulator drivers. Kernel source ends at
`41fba098d`; Linux 5.10.49 #51 full build, matched installation, normal Android
boot and automatic Live modem pass. #50 is its immediate paired rollback.
Per-release ledgers and
adaptation details are in `docs/linux-5.10-upstream-sync.md`. The refreshed
latest target is 5.10.274, not yet imported.

The third milestone resolves all 596 entries in 5.10.50: 566 exact picks,
12 code adaptations, one version marker, ten already-present fixes and seven
inapplicable entries. Minimal official follow-ups and a separate DWC3
reader-lifetime repair end the source at `75f387b5d`. Linux 5.10.50 #52 passes
full build, final ELF/layout checks, signed matched installation, normal
Android and automatic Live modem loading. Its 8,878 changed export CRCs
require rebuilt modules; all 390 modem imports match. #51 is the immediate
paired rollback. Effective configuration and security controls remain;
complete boot logs contain no actual WARN/fatal/module fault. This is a
scoped stable-milestone pass, not full hardware, stable KMI or GKI acceptance.

The fourth milestone resolves all 767 official entries through 5.10.55:
712 exact picks, 12 code adaptations, five version markers, 36 already
present and two inapplicable entries. The official early XArray follow-up
and separate whitespace cleanup end the kernel source at `2f2c2dc22`.
Linux 5.10.55 #53 passes the full build, affected CFI/layout checks, signed
matched installation, normal Android and automatic Live modem loading.
Its 58 changed export CRCs are preserved, with all 390 modem imports matching.
Ramdisk, configuration and enforcing controls remain unchanged; complete
boot logs contain no actual kernel or module fault. #52 is the immediate
paired rollback. Next is 5.10.56-63, including an actual OEM assembly-offset
gate before accepting the upstream MSI device-layout change.

The fifth milestone resolves all 614 official entries through 5.10.64:
562 exact picks, nineteen code adaptations, nine version markers, 23 already
present and one inapplicable entry. Separate integrations replace 29 proven
OEM assembly operands with twelve generated offsets, move MAS queue
iteration onto the standard request-reference lifecycle, and coordinate two
real compile-time vendor interfaces. Source ends at `378a72fba`; #54 passes
full build, actual layout/assembly/once/flush CFI checks, signed matched
installation, normal Android and automatic matching Live modem loading.
All 390 modem imports match despite 8,949 changed export CRCs. Ramdisk,
configuration and enforcing controls remain; complete logs contain no actual
kernel/module fault. #53 is the immediate paired rollback. Next is the
bounded complete 5.10.65-70 range; latest stable and GKI remain open.

The sixth milestone resolves all 842 official entries through 5.10.70:
798 exact picks, twenty code adaptations, six version markers, thirteen
already present and five inapplicable entries. The coordinated standard
const list_sort interface replaces the earlier private cast; a separate
MAS all-tag migration retains three-bank numbering and uses the standard
request-reference/STATIC/STARTED contract. Source ends at `a08144e8a`;
#55 passes the full build, actual layout/const-CFI/all-tag gates, signed
matched installation, normal Android and automatic matching Live modem.
All 390 modem imports match, despite fifteen changed export CRCs. Existing
configuration, ramdisk and security controls remain; complete logs have
no actual kernel/module fault. #54 is the immediate paired rollback.
Next is the complete 5.10.71-75 range; latest stable and GKI remain open.

The seventh milestone resolves all 330 official entries through 5.10.75:
309 exact picks, five code adaptations, five markers and eleven already
present fixes. The schedutil adaptation uses the standard kobject lifetime
while retaining the selected last-policy cache behavior. Source ends at
`99ff6138b`; #56 passes full build, fresh MDIO/FIB C-layout and enforcing
CFI checks, scoped offset/const-sort/all-tag continuity, signed matched
installation, normal Android and automatic matching Live modem. All 390
modem imports match the three changed MDIO/FIB export CRCs. All 6,432
configuration entries, ramdisk and security controls remain unchanged;
complete logs have no actual kernel/module fault. #55 is the immediate
paired rollback. Next is complete 5.10.76-80; latest stable and GKI remain open.

The eighth milestone resolves all 779 official entries through 5.10.80:
732 exact picks, nineteen code adaptations, five markers, twenty-two already
present fixes and one inapplicable entry. The fork adaptation initializes the
pinned child task group before selected OEM QoS trace consumers; a separate
integration namespaces private RCC/PERF idle helpers. Source ends at
`02e9deb3a`; #57 passes full build, final selected layout/enforcing CFI gates,
scoped offset/const-sort/all-tag continuity, signed paired installation,
normal Android and automatic matching Live modem. All 390 modem imports
match despite 2,051 changed export CRCs; all 13,827 exports remain. Existing
configuration, signing material, ramdisk and security controls are unchanged.
Complete logs contain no actual kernel/module fault; 517 AVC denials remain
visible, none for module loading. #56 is the immediate paired rollback.
Next is complete 5.10.81-85 with a fresh UFS layout/assembly gate; the pinned
5.10.274 target, Android common parity and GKI remain open.

The ninth milestone resolves all 557 official entries through 5.10.85:
519 exact picks, sixteen code adaptations, five markers, sixteen already
present fixes and one inapplicable entry. A separate two-file UFS integration
replaces proven stale state/mutex assembly literals with generated offsets
while retaining the upstream task-management pointer layout. Source ends at
`0d678d58e`; #58 passes full build, five final ABI/CFI gates, fourteen generated
constants and thirteen affected assembly operands, signed paired installation,
normal Android and automatic matching Live modem. All 390 modem imports match
the 2,575 changed export CRCs. Effective configuration, ramdisk and security
remain; only the already-disabled FORTIFY entry disappears from emitted
configuration. Complete logs contain no actual kernel/module fault; 501 AVCs
remain visible, none for module loading. #57 is immediate rollback and recent
#54-58 pairs remain local. Next is complete 5.10.86-90; latest stable, Android
common parity, GKI and full hardware/power acceptance remain open.

The tenth milestone resolves all 259 official entries through 5.10.90:
229 exact picks, seven code adaptations, five markers and eighteen already
present fixes. Source ends at `74057806b` without a new integration commit;
the accepted UFS offset repair remains. #59 passes full build, five final
ABI/CFI gates bound to 46 actual objects, signed paired installation, normal
Android and automatic matching Live modem. New MMC/BPF callback pointers,
types and enforcing dispatch pass; unchanged layouts and assembly retain
scoped continuity without extra private compilation. All 13,829 export
CRCs remain equal and 390 modem imports match. Effective configuration,
ramdisk and security remain; the new disabled BPF bool only adds an emitted
entry. Complete logs contain no actual kernel/module fault; 516 AVCs remain,
none for module loading. #58 is immediate rollback and #55-59 pairs remain
local. Next is complete 5.10.91-95, followed by a separately recorded official
XFRM MTIMER policy fix before building. The pinned 5.10.274 target, Android
common parity, GKI and full hardware/power acceptance remain open.

The eleventh milestone resolves all 663 official entries through 5.10.95:
641 exact picks, eight code adaptations, five markers, seven PRESENT fixes
and two missing-driver exceptions. The 654 original-author source commits
end at `e4353a21c`; separate exact official XFRM MTIMER policy fix
`ec2b3b670` is applied before building, outside that range. #60 passes full
build and five final ABI/CFI reports bound to 72 actual objects. Eight changed
records, nine public interfaces, selected Perf dispatch and UFS early
publication pass bounded natural-object checks; unchanged layout/assembly
proofs retain explicit continuity. All 13,829 exports remain, with 838 real
CRC changes and every rebuilt modem/dummy import matching. Signed paired
installation reaches normal Android with automatic matching Live modem,
complete readbacks/build-id and unchanged ramdisk/security. Full logs contain
no actual kernel/module fault; 517 AVC denials remain, none for module loading.
Tracing and Asleep are restored without a second boot or broad hardware
replay. #59 is immediate rollback and #56-60 pairs remain local. Next is
complete 5.10.96-100; the pinned latest-stable target, Android common parity,
GKI and inherited hardware/power acceptance remain open.

The twelfth milestone resolves all 208 official entries through 5.10.100:
189 exact picks, three code adaptations, five markers and eleven PRESENT
fixes. The 197 original-author commits end at `25e0e4b59`; separate one-line
cpuset assertion integration `96e409478` matches the retained hierarchy mutex
without changing the original import accounting. #61 passes full build and
five final ABI/CFI reports bound to 75 current objects. Three changed Perf
records and their core/ARM PMU/ARM64/OEM consumers require only four natural
object decodes; unchanged layout, assembly, sort and all-tag proofs retain
explicit continuity. The table has 13,831 exports, two additions, no removals
and 1,512 real CRC changes; every rebuilt modem/dummy import matches. Signed
paired installation reaches normal Android with automatic matching Live
modem, full readbacks/build-id and unchanged ramdisk/security. Complete logs
contain no actual kernel/module fault; 517 AVCs remain visible, none for
module loading. Tracing and Asleep are restored without a second boot or
broad hardware replay. #60 is immediate rollback and #57-61 pairs remain
local. Next is complete 5.10.101-105; the pinned 5.10.274 target, Android
common parity, GKI and inherited hardware/power acceptance remain open.

The thirteenth milestone resolves all 483 official entries through 5.10.105:
440 exact picks, eighteen code adaptations, five markers, sixteen PRESENT
fixes and four missing-prerequisite entries. The 463 original-author commits
end at `138273525`; five separate PM/modern ARM64 integrations and the complete
upstream READ_ONCE assembly follow-up end the build source at `5b0828436`.
The sole effective configuration addition enables the new BHB mitigation.
#62 passes full compilation and five final ABI/CFI reports bound to 85 current
objects. Only nine new natural C objects are decoded once to check three
changed CPU/IPv6 records and actual PM, netfilter and BHB consumers; old
layout, OEM assembly, sort and MAS proofs retain scoped continuity. Three
non-KPTI vector banks and actual final callback targets pass, without a KVM
or hardware-mitigation effectiveness claim. All 13,831 exports are recorded,
with one addition, one removal and 2,646 real CRC changes; every rebuilt
modem/dummy import matches. Signed paired installation reaches normal Android
with automatic matching Live modem, full readbacks/build-id and unchanged
ramdisk/security. Complete logs contain no actual kernel/module fault; 517
AVCs remain visible, none for module loading. Tracing and Asleep are restored
without a second boot or broad hardware replay. #61 is immediate rollback;
recent #58-62 pairs and accepted #62 metadata remain local. Next is complete
5.10.106-110, not completion of the pinned 5.10.274 target, Android common
parity, GKI or inherited hardware/power acceptance.

The fourteenth milestone resolves all 762 official entries through 5.10.110:
706 exact picks, eleven code adaptations, five markers, 35 PRESENT decisions
and five missing prerequisites. The 722 original-author commits and two
separate interleaved ALSA/sixpack integrations end at `18c37e107`. Complete
source/provenance review preserves stronger inherited implementations instead
of downgrading them to older stable context. #63 passes full compilation and
five final ABI/CFI reports bound to 101 current objects/entire commands. Only
thirteen new natural C objects are decoded once to check eight changed
pstore/PCI/audit/SELinux/coredump/ext4 records and actual XFRM/UDP/UART/xHCI
interfaces. Old layouts, OEM/BHB assembly, sort and MAS proofs retain scoped
continuity. All 6,433 configuration options and signing material remain.
The table has 13,835 exports, five additions, one removal and 3,904 real CRC
changes; every rebuilt modem/dummy import matches. Signed paired installation
reaches normal Android with automatic matching Live modem, full readbacks/
build-id and unchanged ramdisk/security. Complete same-boot logs contain no
actual kernel/module fault; 517 AVCs remain visible, none for module loading.
Tracing and Asleep are restored without a second boot or broad hardware
replay. #62 is immediate rollback; recent #59-63 pairs and accepted #63
metadata remain local. A separate inherited, selected io_uring fixed-file
initialization/reference-release defect remains open, not fixed by the absent
old SCM-helper entry or certified by boot. Next is complete 5.10.111-115 and
its independently reviewed lifetime repair; the pinned latest-stable target,
Android common parity, GKI and inherited hardware/power acceptance remain open.

The fifteenth milestone resolves all 564 official 5.10.111-115 entries:
532 exact picks, twelve code adaptations, five markers, twelve PRESENT
decisions and three missing prerequisites. The 549 original-author commits
end at `1727a9f07`. Separate commit `792b8bc8b` first restores io_uring
fixed-file node initialization and direct reference ownership; after a real
compiler error, `9667daa8b` routes the OEM OOM task producer through the
standard delayed queue. Original provenance and stronger inherited contracts
remain distinct from these integrations.

Full compilation naturally produces #64. Four scoped final reports bind
25 current objects/entire commands; only sixteen new natural IR objects
measure changed selected layouts, public interfaces and the new OOM timer
CFI target. Fourteen generated OEM offsets and effective configuration/signing
remain. The disabled `CONFIG_NVM` entry disappears because upstream disables
LightNVM; this is not removal of NVMEM or NVMe support. All 13,835 exports
remain, with 8,962 real CRC changes, and every rebuilt modem/dummy import
matches. Normal module PLT sections and separately stripped/re-signed modem
staging pass.

The same #64 pair reaches normal Android with automatic matching Live modem,
complete readbacks/build-id and unchanged ramdisk/security. Two diagnostic
wrapper errors cause paired #63 recovery before final acceptance: root does
not satisfy the OEM io_uring UID1013 rule, and ordinary ADB cannot list the
subsequently UID1013-owned private 0700 directory. Only the wrapper is fixed;
the kernel and probe binary are not rebuilt. The final drop-only UID1013
ordinary-file probe passes fixed reads, replacement, error cleanup and actual
last-reference release after asynchronous close. No actual kernel/module fault
is found; 516 pre-probe AVCs and ten new probe-window AVCs remain visible,
without a module-loading or probe-named denial in the additive attribution.
Tracing and Asleep remain restored. #63 is immediate rollback; recent #60-64
pairs and accepted #64 metadata remain local. Next is complete 5.10.116-120;
full io_uring/OOM safety, selected AMA mempolicy, the pinned stable target,
Android common/GKI and inherited hardware/power acceptance remain open.

The sixteenth milestone resolves all 394 official 5.10.116-120 entries:
347 exact picks, seventeen code adaptations, five markers and 25 qualified
no-new-commit dispositions. The 369 original-author source commits end at
`aab63b923`; separate `0436052d6` repairs HKIP subjective-credential tracking.
The RNG series retains the BL31 firmware API, sixteen-byte buffer and sixteen
credited initialization bits. SELinux cleanup remains limited by PRMEM arena
ownership; the iterator-copy fix retains UFS inline-crypto completion hooks.

Actual #65 compilation, selected interfaces and matched signed staging pass,
but CameraDaemon firmware loading triggers HKIP's GID root-check BUG before
normal Android. Official 117#39 correctly uses temporary kernel credentials
for firmware access; the OEM bitmap still reflects the prior credentials.
Both #64 partitions recover successfully. The separate repair updates HKIP
root bits after each override/revert publication, preserving paired clearing,
credential references, HVC checks and upstream firmware behavior.

Necessary #66 compilation and current ELF/CFI/module gates pass. All 26 prior
natural object/complete-command identities remain unchanged; only the new
credential object is decoded once, and ten current CFI memberships are
refreshed without replaying the prior thirteen IR objects. Configuration and
signing remain unchanged from #65. There are 13,839 exports, six additions,
two removals and 2,894 CRC changes versus #64; all 390 modem and two optional
dummy imports match. No loader or protection bypass is used.

The paired #66 installation reaches normal Android with automatic matching
Live modem, full readbacks/build-id and unchanged ramdisk/security. One actual
CameraDaemon thread completes ISP firmware loading, ISPCPU startup and power-on
return in the same boot. Complete logs contain no actual kernel/module fault;
517 AVC denials remain visible, none requests module loading. Tracing and
Asleep are restored. #64 remains immediate rollback; failed #65 evidence is
kept separately. Next is complete 5.10.121-125. Arena capacity/lifetime,
selected AMA mempolicy, full io_uring/OOM safety, latest stable, ACK/GKI and
inherited hardware/power acceptance remain open.

The seventeenth milestone imports all 734 official 5.10.121-125 entries:
674 exact picks, 31 adaptations, 26 already-present fixes and three specific
not-applicable dispositions. The 705 original-author commits retain their
identities; separate `1dd5d0603` corrects USB hunk placement and `606f5a0f8`
passes logical block zero to two new ext4 directory-check calls. Existing
OEM checker semantics and original byte offsets remain unchanged.

The actual #67 full build passes. Only CONFIG_LIB_MEMNEQ is newly selected;
CFI/ThinLTO/BHB/Werror, signing materials and fourteen generated OEM values
remain. Eight new natural contexts establish selected CFS, jack, thermal,
GPIO, LSM and library contracts without replaying old object/layout proofs.
The current ELF passes 28 targets in eleven CFI groups. There are 13,838
exports, one addition, two removals and 403 changed CRCs versus #66; all 390
modem and two optional dummy imports match. Signed matched-image checks pass.

Paired #67 installation reaches normal Android with automatic matching Live
modem, partition/module/build-id readbacks and unchanged independent ramdisk
and enforcing controls. The same-thread ISP firmware path completes all six
stages. Complete logs contain no actual kernel/module fault; 528 AVC denials
remain visible, none requests module loading. Tracing and Asleep are restored.
#66 is immediate rollback; compact metadata, originals and recent #63-67
pairs remain local below 20 GB. Next is complete 5.10.126-130; the pinned
latest-stable target, inherited subsystem/hardware/power findings and
ACK/vendor-core/GKI architecture remain open. No release is exported or
published.

### Linux 5.10.126-130 Milestone

The complete canonical stable range has 252 original entries: 226 exact
picks, ten adaptations and sixteen already-present operations, recorded in
five per-release ledgers. The 236 original-author source commits end at
`7c9fa502b`; no separate kernel integration is needed in this batch. NFT
register parsing keeps full-width validation before byte publication and
the existing generic/verdict datatype normalization. PM supplier API and
framebuffer fixes retain their real hardware contracts; duplicate operations
are not reinserted.

The single full build produces 5.10.130 #68 with unchanged effective
configuration and retained CFI/ThinLTO/BHB/Werror, signing materials and
security. Sixteen new ordinary contexts, selected NFT3 cached consumers,
fourteen current OEM selectors and 28 current targets in eleven CFI groups
pass scoped checks. Checker-only corrections account for inlined verdict/
value branches and optimized byte addressing; they do not change kernel
source or certify every expression layout. There are 13,837 exports, two
additions, three removals and no shared CRC changes versus #67; all 390 modem
and two optional dummy imports match.

Paired #68 installation reaches normal Android with automatic matching Live
modem, full partition/module/build-id readbacks, unlocked user storage and
unchanged independent ramdisk/security. The same-thread six-stage ISP
firmware startup completes. Complete logs contain no actual kernel/module
fault; 519 AVC denials remain visible, none requests module loading. Tracing
and Asleep are restored. #67 is immediate rollback; compact metadata,
originals and recent #64-68 pairs remain local below 20 GB. Next is complete
5.10.131-135, whose official inputs alone are prepared. The inherited SELinux
PRMEM arena and two source-only fbcon findings, full hardware/power, the
pinned 5.10.274 target and ACK/vendor-core/GKI work remain open. No release
is exported or published.

### Linux 5.10.131-135 Milestone

All 432 canonical stable entries are resolved in order: 374 exact picks,
27 adaptations and 31 already-present implementations. The 401 original-author
source commits end at `6d6830bed`, without a separate kernel integration.
POSIX timer teardown preserves the exec cleanup boundary; network unregister
and TCP single-snapshot fixes retain hardware/OEM consumers. TTY API retirement
and the pty insertion/commit transaction preserve the existing port lock and
UART_V500 queue. x86 RETBleed/objtool changes migrate retained mcount and shared
build-rule consumers without relaxing ARM64 CFI, ThinLTO or BHB.

One full build produces 5.10.135 #69 with no effective or emitted configuration
delta. Thirteen actual ordinary C contexts, the new BPF test callback's precise
type and 64-byte context, 29 targets in twelve final CFI groups, and fourteen
current generated OEM selectors pass scoped checks. VLAN remains disabled and
source-only; a diagnostic selection correction does not force it into the
build. There are 13,836 exports, only `tty_schedule_flip` removed, and no changed
shared CRCs versus #68. All 390 modem and two optional dummy imports match.

Stock-format boot roundtrip, CMS signature/certificate and ext4 metadata pass.
Paired #69 installation reaches normal Android with automatic matching Live
modem, complete partition/module/build-id readbacks, unlocked user storage,
required services and completed same-thread ISP firmware startup. Independent
ramdisk, SELinux Enforcing, taint4, forced signatures and PRMEM remain. Complete
logs contain no actual kernel/module fault; 519 AVC denials remain visible,
none requests module loading. Original tracing and Asleep are restored.
#68 is immediate paired rollback; compact metadata and recent #65-69 pairs
remain local below 20 GB. Next is complete 5.10.136-140, whose 812 original
inputs are prepared but not yet imported. The pinned 5.10.274 target, SELinux
PRMEM arena/fbcon/AMA lifetime findings, hardware/power and ACK/vendor-core/GKI
work remain open. No release export or publication occurs.

For later stable milestones, the user's acceptance gate is normal Android
boot with automatic loading of the matching module. Retain build, signing,
partition readback, security and necessary boot-log checks; repeat broader
hardware tests only when a new change, fault or unresolved issue requires it.

### Installed Bring-up Baseline

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
validation. The installed device is #55/5.10.70. A separate normalized vote-scale
reference audit classifies it as unused original bookkeeping within the
examined scope; no dummy state is restored or performance parity claimed.
See `docs/clang12-mips-multilevel-audit.md`.
See `docs/clang12-functional-tests.md` for results and the next isolated fix.

The preserved bring-up baseline is r49: r21 retains full early logs,
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
test limits. Accepted r17 and the latest r49/r50/r51/r52/r53 pairs are retained;
#52 is the immediate #53 rollback. Original boot/ramdisk/modem backups remain
intact.
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
At that checkpoint the controller clears first, so a later END_FREQ returns
FRAME_ERR_PID (-100); direct END_FREQ=0 is not yet re-observed on r48 and the
r47 result is not relabelled.
The genuine temporary out-of-tree probe is unloaded and a cleanup reboot
restores taint 4. SELinux Enforcing, forced signatures, CFI, PRMEM and independent
ramdisk remain. No actual kernel WARN/fatal record occurs in the scoped logs;
two unrelated logserver capability AVCs are classified, not called a zero-AVC
private window. Boost expiry/renewal, positive global lifecycle and full frame/
performance/power acceptance remain open at that checkpoint.
A source-unchanged r48 follow-up locates the real global boost caller in the
stock iawareperf service's uniperf domain. Its loaded libperfgenius_wrapper
issues 0xc014ab64 through HisiPerfAdapter::SetRtgSchedBoost; the installed
SELinux policy already permits that command on its own RTG file. Ordinary
blank-lockscreen taps yield one fully paired request returning 0, boost floor
600 and a 63-jiffy timer at HZ=250. The real timer callback fires about 273 ms
after arming, then frame_sched clears the observed default-group floor and
disables that group. Loss-free instance stacks distinguish activation, timer
expiry and worker cleanup; this is production single-expiry evidence, not a
synthetic request or a direct END_FREQ restoration test.
No second global request is observed, so renewal/stale-timeout restart stress
remains open. Framework click reports have scene-reset, interval and count
limits; repeated taps are not renewal evidence. A subsequent read-only audit
locates the actual 3033 consumer in HwIAware's loaded optimized bytecode:
IAwareSdkService -> AppInnerBoostARSPlugin -> AppInnerBoostDmePlugin (2019)
-> UniPerf. Click traversal selects 4400; click jitter selects 4401 only for
at least three skipped frames. Existing CMS getters confirm click 1000 ms/
three reports, slide 5000 ms/16 reports and enabled switches. The additional
application-type count limits apply to slides, not click deduplication.
Same-channel report spacing is 250 ms, equal to nominal click boost duration;
independent traversal/jitter channels can overlap in principle, but no safe
blank-lockscreen operation is established that guarantees a second request
before expiry. No additional input, forced jitter or injected request is
used; actual renewal remains unverified. Multi-group/AUX/configured boost-
thread coverage, positive global queue lifecycle and full frame/power
acceptance also remain open. No kernel rebuild, flash or permission/settings
change occurs; boot/security/keyguard/display and existing tracers are
preserved.
A later source-unchanged awake-idle observation fills the direct END_FREQ gap
on r48: SystemUI's own ioctl returns 0, with UI/RenderThread restored from
frame RT 87 to ordinary policy 0/priority 110 inside the stop stacks before
return and before controller member clear. All 118 subsequent scheduled-in
samples for those members are ordinary. Both planned taps are skipped;
this is not click-trigger or boost-renewal acceptance. All six buffers are
loss-free, complete-window logs have no actual kernel fault or AVC denial,
and display/settings/keyguard/security plus existing tracers are restored.
No rebuild, flash, reboot or additional test follows the pass. Renewal/
stale-timeout restart, positive global queue, multi-group/AUX/configured boost
threads, full frame/power and existing ABI/manual/hardware/source boundaries
remain open.
A read-only follow-up attributes the longer 650 ms window-switch routes and
the KeyBoost-gated key path. Current custom/base CMS configuration has no
KeyBoost item; it is not enabled for testing. SystemUI's numeric 4098 sites
are input-source IDs, not window-boost evidence. A shorter overlapping request
may retain the existing later deadline, so a second successful ioctl alone
does not pass renewal. One non-writing stock Settings/Display foreground
observation is proposed, pending user manual unlock and explicit scene
confirmation; no input or trace preparation occurs while waiting. No new
kernel defect or renewal pass is claimed. See `docs/clang12-rtg-source-audit.md`.
The user's subsequent passwordless-unlock/foreground authorization permits
one stock Settings/Display-and-return observation on unchanged #48. Five
complete real uniperf boost ioctls return 0; the same timer is actually rearmed
before expiry, extending pending deadlines by 37/81/11 jiffies. The first
episode keeps util 684 in the default and two allocated frame groups across
the old deadlines until final expiry, then the worker clears all three. The
second clears the default and still-active group; its Settings group is
released before expiry, so release/reuse cleanup is not certified. All three
buffers are loss-free and no actual kernel fault occurs. Other-domain
property/capability/AOD AVCs are recorded, not hidden or fixed by relaxing
SELinux. The device returns to its original home foreground, Asleep and
keyguard showing; automatic mode/timeout and security/tracers are preserved,
but automatic brightness naturally reads 26 -> 27 and is not overwritten.
No kernel change, rebuild, flash or repeated scene follows this scoped
ordinary-renewal pass. Stale-notification/restart stress, shorter-request
retention, release/reuse, AUX/configured boost threads, positive global queue
and full frame/power acceptance remain open.
r49 fixes the newly attributed persistent boost-floor omission: released
multi-frame slots are skipped by the old active-only expiry lookup, while
global disable cancels the timer without withdrawing its floors. The existing
timer mutex now protects a shared withdrawal of the default group, every
persistent multi-frame slot, AUX and configured boost threads on expiry and
after synchronous cancellation. No ABI, configuration, exported CRC or
signing-material change occurs relative to r48.
Full #49 compilation, final ELF, matched module signatures/imports, paired
installation/readback and unchanged independent ramdisk pass. A normal stock
Settings/Display-and-return regression directly samples a released slot with
a pending boost, then observes its stored floor reach zero after final
expiry. Five fully paired production boost calls return 0; ordinary renewal
extends the real timer before the superseded deadlines, and all persistent
slot floors clear. Global disable/re-enable and stale-notification stress are
not certified by this runtime result.
A separate positive global-render probe starts only after a read-only check
shows no registration or active system queue. Owned ordinary-priority tasks
pass depth-4 force initialization, wake relation/getter, pause, same-live-root
non-force resume, depth-1 replacement, old-root exit and new-root exit cleanup.
No production queue or task priority is overwritten. The signed read-only
observer is unloaded; a necessary cleanup reboot restores taint 4 with only
the matching modem module loaded. Scoped logs have no actual kernel WARN or
fatal record; unrelated AVCs are recorded without relaxing SELinux.
The r49 Wi-Fi follow-up associates an existing saved network and obtains a
validated default Wi-Fi route. A correctly attributed shell probe resolves
DNS and receives HTTP 403 through default HTTPS certificate validation from
a public Linux mirror. Two other endpoints reset during TLS; the expected
file/integrity transfer therefore remains unpassed, not a proven kernel
failure or a sustained-network pass. Wi-Fi OFF, home foreground, Asleep,
keyguard, other settings and security controls are restored.
Per the user's latest scope, this iteration ends in the local source Git tree
without an exported bundle, release packaging or publication. The records
retain global-disable/stale-timeout stress, shorter-request retention,
configured-thread/AUX lifetime, PID/address-reuse/32-bit integration, complete
frame/power and external hardware/source prerequisites for the next stage.
See `docs/clang12-completion-plan.md` for the local closeout and open conditions.
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

- `Code_Opensource/kernel/`: official sources with bring-up fixes and the first stable synchronization batch.
- `configs/allen-stock-5.10.43.config`: configuration read from the running phone.
- `tools/toolchains/clang-r416183b1/`: local Android Clang 12.0.7 toolchain.
- `tools/host/bin/bc`: local build dependency, extracted from the Arch package.
- `out/allen-clang12/`: default independent build output path; not tracked.
- `out/allen-clang12-r41-metadata/`: retained metadata for the retired #41 build cache; not tracked.
- `out/allen-clang12-r49-metadata/`: verified metadata for the retired r49 cache; rollback images/source remain retained; not tracked.
- `out/allen-clang12-r50-metadata/`: retained configuration/export/build metadata for the replaced #50 output; not tracked.
- `out/allen-clang12-r51-metadata/`: retained configuration/export/build metadata for the replaced #51 output; not tracked.
- `out/allen-clang12-r52-metadata/`: accepted #52 metadata retained before the next build; not tracked.
- `out/allen-clang12-r53-metadata/`: accepted #53 metadata retained before the next build; not tracked.
- `out/allen-clang12-r57-metadata/`: accepted #57 layout/CFI/build/runtime metadata retained before output reuse; not tracked.
- `out/allen-clang12-r58-metadata/`: accepted #58 source/layout/CFI/build/runtime metadata retained before output reuse; not tracked.
- `out/allen-clang12-r59-metadata/`: accepted #59 source/continuity/callback-CFI/build/runtime metadata retained before output reuse; not tracked.
- `out/allen-clang12-r60-metadata/`: accepted #60 source/layout/callback-CFI/build/runtime metadata retained before output reuse; not tracked.
- `out/allen-clang12-r61-metadata/`: accepted #61 integrated-source/Perf-layout/CFI/build/runtime metadata retained before output reuse; not tracked.
- `out/allen-clang12-r62-metadata/`: accepted #62 source/BHB/changed-layout/CFI/build/runtime metadata retained before output reuse; not tracked.
- `out/allen-clang12-r63-metadata/`: accepted #63 source/changed-interface/CFI/build/runtime metadata retained before output reuse; not tracked.
- `out/allen-clang12-r64-metadata/`: accepted #64 integrated-source/layout/CFI/build/owned-file-runtime metadata retained before output reuse; not tracked.
- `out/allen-clang12-r65-failed-metadata/`: source/build/interface and first-fault/recovery metadata for failed #65, not runtime acceptance; not tracked.
- `out/allen-clang12-r66-metadata/`: local closeout destination for accepted #66 paired credential/build/CFI/firmware-runtime metadata before output reuse; not tracked.
- `out/allen-clang12-r67-metadata/`: accepted #67 source/new-interface/CFI/module/firmware-runtime metadata before output reuse; not tracked.
- `out/allen-clang12-upstream-5.10/`: installed 5.10.125 #67 output, reused sequentially for stable milestones; not tracked.
- `images/original/`: firmware device-tree backup; not tracked.
- `docs/bringup-baseline.md`: device baseline, toolchain provenance and risks.
- `docs/stock-module-compat.md`: installed modem ABI and signing compatibility.
- `docs/clang12-r2-flash-test.md`: r2 CFI boot failure, verified rollback and next fix.
- `docs/clang12-cfi-iterations.md`: targeted CFI fixes and subsequent paired tests.
- `docs/clang12-working-baseline.md`: accepted r17 boot baseline and test scope.
- `docs/clang12-functional-tests.md`: functional repairs, checks and audio/BT gaps.
- `docs/clang12-early-diagnostics.md`: r21-r27 log, lock, GPIO, PD, registration and PRMEM fixes.
- `docs/clang12-audio-source-audit.md`: codec source audit and scoped r28 CLT integration.
- `docs/linux-5.10-upstream-sync.md`: stable source provenance, adaptations, build checks and GKI boundaries.
- `docs/linux-5.10.44-imports.tsv`: complete first-batch stable/local commit ledger.
- `docs/kernel-module-updates.md`: matched rebuild/signing workflow, actual MODVERSIONS rule and stable-KMI limits.

The original source archive, `boot.img` and `ramdisk-magisk.img` remain at the
project root and are excluded from Git. Never overwrite them.

## Build

From `/home/data/Allen`:

```sh
scripts/build-allen.sh merge_charlotte_defconfig
scripts/build-allen.sh
```

These commands describe the initial released-configuration build, not the
stock-derived configuration used for the retained r49 or installed #67.
For the retained upstream output and its r49-derived configuration, use:

```sh
OUTPUT_DIR=out/allen-clang12-upstream-5.10 scripts/build-allen.sh
```

The target and compiler revision come from the official README and Makefile.
The wrapper uses LLVM tools and the integrated assembler, not the host GCC for
ARM64. `JOBS` overrides the default eight build jobs; `OUTPUT_DIR` selects an
independent output directory. `LOCALVERSION=` prevents a Git suffix; the
Makefile still controls the release, currently `5.10.125` on the upstream
branch. This alone does not establish module ABI compatibility. Project
packaging uses same-build kernel/module pairs; MODVERSIONS may permit a
different release token, but is not a full binary-compatibility guarantee.
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
