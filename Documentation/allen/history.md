# Allen Kernel Bring-Up

Bring up the official ALN HarmonyOS 4.2 Linux 5.10.43 sources on x86_64 Linux,
then move the tested baseline toward Linux stable and Android common/GKI.
Project safety rules are in `AGENTS.md`.

## Current Status

The secure accepted baseline is Linux 5.10.274 #101, source `5b0cc1b997`,
with normal Android and automatic matching signed Live modem. The device is
currently running local test-only build #103 from that same source. It disables
mandatory module signatures and Huawei root scans; SELinux starts Enforcing,
was toggled to Permissive and restored through Magisk. The accepted #101 pair is
the exact rollback. Details are in `docs/clang12-security-relaxed-test.md`.
The stage closeout supplies one source archive with two standard profiles in
`Code_Opensource/kernel/arch/arm64/configs/`: `allen_secure_defconfig` and
`allen_relaxed_defconfig`. See `Code_Opensource/kernel/README.allen` for portable
build instructions and `docs/allen-5.10.274-stage-closeout.md` for the local
cleanup and source-package boundary.
The official 5.10 branch and release tag still identify 5.10.274 on
November 20, 2026.
The final 271-274 range accounts for all 1,114 official entries and is built
with Android Clang/LLD 12.0.7. Effective security configuration, certificate
and generated OEM constants remain intact. Current layout, selected callback
CFI, raw-assembly, module signature/CRC and device readback checks pass.
CE, essential services, automatic ISP startup, independent ramdisk, Enforcing,
forced signatures, PRMEM and original trace/screen state passed on #101. #103
also reaches Android with the matching Live modem and unchanged ramdisk. Five
recent accepted image pairs remain; #101 is the immediate secure rollback.

The stable update and normal-boot milestone is complete. Android common
parity, OEM/core separation, stable GKI/KMI certification and broad hardware,
network, performance and power acceptance remain open. All work stays in
local Git; nothing is exported or published as a release.

Accepted79 private-ring evidence is retained,
not replayed. The accepted162-165 migration
keeps one canonical io_uring core: installed Hyperframe34-37 defaults remain,
and immutable `IORING_SETUP_STANDARD_OPS` explicitly enables standard34-36.
Fresh task/HF layout, lifetime, enforcing CFI and assembly checks pass; nine
finite private-ring checks cover both modes and fixed-file reference release.
Retained Hyperframe buffers require page alignment, and CLOSE's user_data is
its cancel/unlink flag; arbitrary-buffer support is not certified. The
previous SDHCI epilogue and concrete io_uring backup-allocation repair remain.
Filesystem-owned keys, Huawei SDP cleanup and host OpenSSL4 PEM/CMS
compatibility remain. #73's DWC3 disconnect/reconnect evidence is scoped to that
accepted implementation. #71 is rejected for a real OEM
post-fork cpuset lock assertion; #72 repairs that caller without removing
the assertion. #65 remains a failed
HKIP firmware-boot candidate, not an accepted runtime. Ordered synchronization
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

### Linux 5.10.136-140 Milestone

All 812 canonical stable entries are resolved in order: 750 exact picks,
20 adaptations, 40 already-present implementations and two absent/unselected
platform patches. The 770 original-author commits end at `c218c464d`.
Stable provenance now lands inside actual commit messages, before the mbox
separator; earlier accepted history is not rewritten. A separate upstream
ext4 correction (`61a1d87a324a`, local `266bbac94`) compares a logical directory
block against a block count, not bytes. One integration cleanup removes a
duplicate NFT bound-chain guard, ending final source at `c303cfc45`.

Request-only block trace migration includes both Huawei IOTrace callbacks;
deadline migration includes CPU pause and the unselected stub provider.
Effective cpuset validation, RT count ordering and TASK_NEW CFS reweighting
retain OEM hooks. NFT data type/size validation and cmp16 producer/consumer
layouts are checked in current natural objects without replaying the old
register-parser proof or relaxing CFI. Standard BLK_DEV_IO_TRACE callbacks
remain disabled; actual generated raw/perf tracing and Huawei callbacks are
checked instead of force-building them.

One full build produces #70 with no effective/emitted configuration delta,
unchanged signing material, 27 natural C contexts, 41 targets in 18 final CFI
groups and fourteen unchanged current OEM selectors. There are 13,843 exports:
seven additions, no removals and 267 CRC changes versus #69. All 390 rebuilt
modem and two optional dummy imports match; the dummy is not installed.

Stock boot roundtrip, CMS signature and ext4 metadata pass. Normal Android
automatically loads the matching Live modem with partition/module/build-id
readbacks, unlocked storage, required services and same-thread six-stage ISP
startup. Independent ramdisk, Enforcing, taint4, mandatory signatures and
PRMEM remain. Complete logs contain no actual kernel/module/HKIP fault;
517 AVC denials are visible, none requests module loading. Original tracing
and Asleep are restored. #69 is immediate paired rollback and the latest-five
test window is #66-70; originals and failed #65 evidence remain below 20 GB.
Next is complete 5.10.141-145. Target 5.10.274, inherited source/hardware/power
findings and ACK/vendor-core/GKI remain open; no release is exported or
published.

### Linux 5.10.141-145 Milestone

All 260 canonical originals resolve in order: 214 exact picks, 16 adaptations,
29 already present and one absent/unselected PFUZE driver. The 230
original-author source commits end at `917c376f5`; complete author/date/message
provenance and five per-release ledgers agree. A separate upstream cgroup
transfer lock correction (`ab1de7ead871`, local `c7cfda261`) follows, without
being disguised as a canonical entry.

The standard cgroup/cpuset lock-order change requires the CPU hotplug read
lock outside threadgroup write locking. #71 reaches Android and loads modem,
but ten actual assertions expose an additional Huawei post-fork top-app
caller missing that outer lock. The pair is rejected and #70 is restored.
Local `78095eb5f` adds only the explicit CPU header and balanced caller-side
read lock; VIP behavior and the cpuset assertion remain. The necessary #72
build exits successfully. #71's original build exit was not directly observed
after terminal restoration; its complete error-free log and products were
verified, not misreported as an observed exit0.

Only the canonical unnecessary XOR_BLOCKS selection is removed; ChaCha,
ThinLTO, enforcing CFI, BHB, Werror, signing materials and security remain.
Current anon_vma producer/consumer layouts, inlined pollfree/BPF guards,
cgroup locks and PSOCK publication pass in nine once-decoded objects. #72
retains those exact objects/contexts and decodes only the changed OEM caller.
The final ELF checks 27 targets in 15 CFI groups, accurate XOR retirement and
BPF/NFT/cpuset callback assignments. All 390 modem and two optional dummy
imports match 13,843 exports, with two additions, two removals and 275 CRC
changes versus #70. Fourteen OEM selectors retain their scoped providers;
dummy is not installed.

#72 normal Android, automatic matching Live modem, readbacks/build-id,
unlocked storage/services and same-thread ISP startup pass. A short real
framework window captures 14 top-app events and eight repaired OEM attach
stacks without trace loss or CPU-lock WARN. Complete logs show no actual
kernel/module/HKIP fault; 542 AVCs remain visible, none requests module load.
Independent ramdisk, Enforcing, taint4, mandatory signatures and PRMEM remain;
original tracing and Asleep are restored. #70 is immediate paired recovery;
the latest-five build window is #68-72, with failed #71 evidence explicit.
Originals and precise source/proof references stay below 20 GB. Next is
complete 5.10.146-150. Target274, HID wrap, inherited lifetime/hardware/power
findings and ACK/vendor-core/GKI/KMI remain open; no release is exported or
published.

### Linux 5.10.146-150 Milestone

All 641 canonical originals resolve in order: 577 exact picks, 27 adaptations,
35 already present and two specifically inapplicable changes. The 604
original-author commits end at `f2bebf7c2`; complete messages/provenance and
five per-release ledgers agree, with no separate kernel integration needed.
The coherent DWC3 migration uses canonical softconnect, runtime-PM-before-
reset, live IRQ event acknowledgement and post-PM duplicate rejection while
retaining the required platform PHY notification and event-clear wait.
Zonelist retries retain OEM accounting; cpuset excludes per-CPU kthreads
without removing the accepted CPU/threadgroup lock repair. Obsolete io_uring
SCM producer code is not restored, and independent HMFS is not certified by
unselected F2FS fixes.

The ordinary full build exits0 and produces #73. Only the compiler capability
probe CONFIG_CC_HAS_AUTO_VAR_INIT_ZERO_ENABLER changes n -> y; explicit
INIT_STACK_NONE and enforcing security remain. Ten new objects are decoded
once with current generated dependencies: actual random timer types, three
DWC3 layouts, UDC dispatch, cpuset and selected lifetime/accounting guards
agree. The final ELF checks 60 symbols and 28 targets in 15 CFI groups.
All 390 modem and two optional dummy imports match 13,844 exports, with two
additions, one removal and eight shared CRC changes versus #72. Fourteen OEM
selectors and signing materials remain; dummy is not installed.

One paired #73 boot reaches normal Android, automatically loads matching
Live modem and completes the same-thread six-stage ISP startup. Full
partition/module/loaded build-id readbacks and independent ramdisk/security
agree. The actual USB cycle records halt -> core reset -> RunStop and init's
automatic gadget rebind; both later already-connected requests return0
without DCTL writes. Actual random timer execution, zero lost trace events
and ordinary ADB transfer pass. Complete logs contain no actual kernel/module
fault; 548 AVC denials remain visible, none requests module loading. Original
tracing and Asleep are restored. #72 is immediate paired recovery; the
latest-five build window becomes #69-73. Next is complete 5.10.151-155;
target274, ACK/vendor-core/GKI/KMI and inherited hardware/lifetime/power
findings remain open. No release is exported or published.

### Linux 5.10.151-155 Milestone

All 387 canonical originals resolve in order: 361 exact picks, 14 adaptations,
11 already present and one specifically inapplicable SDIO ownership change.
The 375 original-author source commits end at `e597c18c7`; separate
`13b01f4cb` fixes a real host OpenSSL4 no-ENGINE signing-tool link failure,
without changing the target signature policy or existing PEM/CMS path.
fscrypt uses filesystem-owned master keys and active/structural references;
the relocated pointer provider and Huawei SDP cleanup consumer migrate
together. Keyring teardown precedes filesystem device teardown. Encryption
format, user key policy, OEM key buffers and HWDPS APIs remain.

The necessary full build exits0 and produces #74. Only the canonical COMPAT
A57/A72 AES erratum is newly selected; capability64 preserves the existing
ARM64 assignments and NCAPS76. Current 178 ordinary C contexts and eighteen
once-decoded interface objects establish new SELinux gfp callback/dispatch,
crypto/HW_F2FS/ext4 layouts, fscrypt RCU release and reduced IP protocol
structures. Final ELF checks 32 targets in eighteen CFI groups. All 390 modem
and two optional dummy imports match 13,845 exports, with one addition and
8,884 shared CRC changes versus #73. Fourteen OEM selectors and signing
materials remain; dummy is not installed.

One paired #74 boot reaches normal Android with automatic matching Live
modem, partition/file/live build-id readbacks, unlocked CE storage, required
services and the six-stage same-thread ISP startup. Independent ramdisk,
Enforcing, taint4, signature enforcement and PRMEM remain. Full logs contain
no actual kernel/module/HKIP fault; 516 AVC denials remain, none for module
loading. Original tracing and Asleep are restored. #73 is immediate paired
recovery; local metadata and recent #70-74 test pairs remain below 20 GB.
Next is complete canonical 5.10.156-160. The pinned target274, ACK/vendor-core/
GKI/KMI architecture and inherited subsystem/hardware/power findings remain
open; no release is exported or published.

### Linux 5.10.156-160 Milestone

All 520 canonical originals resolve in order: 486 exact picks, 11 adaptations,
19 already present and four specifically absent unrelated hardware providers.
The 497 source commits end at `dac68d33f`; separate `b20c287b7` allocates the
first io_uring quiesce backup before entering the loop, fixing an uninitialized
OOM return and overwritten retry-node leak. All original dates agree; one
157#147 exact-source commit retains an explicitly recorded `git am` in-body
`From:` author/message exception without rewriting accepted history.

VFS initializes actual fsdata declarations while preserving HW_F2FS/OEM
accounting. DMA-heap duplicate checking and publication share the list lock.
DWC3 preserves OEM PM hooks while importing canonical request/ENDTRANSFER
lifecycle corrections. SDHCI keeps HS200/HS400 eligibility and the actual
reset/PHY/delay epilogue; reinit state covers the extra OEM reset site.
THP gains synchronous page-table IPI invalidation and paired source notifiers;
MMU_NOTIFIER is unselected, so its wrappers compile out, not active coverage.
copy_file_range fallback retains VFS filesystem-freeze protection.

The full #75 kernel/module build exits0. Only canonical TCP perturb order16
is newly emitted, equivalent to the prior hardcoded shift. Current 238 natural
C contexts and eleven once-decoded interface objects pass; final ELF checks
36 targets in 22 finite CFI groups. All 390 modem and two optional dummy
imports match 13,845 exports; 682 shared CRCs change versus #74, with no
export addition/removal. Fourteen OEM selectors, signing material and security
remain. Matching modules are rebuilt, not just resigned; dummy is not installed.

One paired #75 installation reaches normal Android with automatic matching
Live modem, full readbacks, unlocked CE/services and same-thread six-stage ISP
startup. Full logs contain no actual kernel/module/HKIP fault; 517 AVC denials
remain, none for module loading. Independent ramdisk, Enforcing, taint4,
signature enforcement and PRMEM remain; trace and Asleep are restored.
#74 is immediate paired recovery. Compact local metadata and recent #71-75
test pairs are retained below 20 GB. Next is complete canonical 5.10.161-165
toward pinned274; ACK/vendor-core/GKI/KMI and inherited subsystem/hardware/
power findings remain open. No release/export bundle or publication occurs.

### Linux 5.10.161 Milestone And Approved162 Boundary

Complete canonical161 accounts for all18 originals:16 exact picks, one
already-present Bluetooth overflow fix and the adapted SUBLEVEL marker.
Seventeen source commits end at `46497f881`; its actual kernel tree agrees
with the complete ordered preview. All17 raw authors, dates and declared
messages match. No162 code is included in this accepted milestone.

One actual full #76 Image/modules build exits0. Configuration, security,
signing material, all13,845 exports/CRCs and14 generated OEM selectors stay
unchanged. All390 modem and two optional dummy imports match. Three necessary
HID/loopback objects establish the real callbacks and consumers; final ELF
checks41 targets in27 finite CFI groups. Prior75 layout decoders are not replayed.

One paired installation reaches normal Android with automatic matching Live
modem, full partition/module/build-id readbacks, unlocked CE/services and
same-thread six-stage ISP startup. Full logs contain no actual kernel/module/
HKIP fault;502 AVC denials remain, none for module loading. Independent
ramdisk, Enforcing, taint4, forced signatures and PRMEM remain; trace and
Asleep are restored. #75 is immediate paired recovery. Local metadata and
recent72-76 pairs remain below20 GB; no release/export/publication occurs.

The user approves the162 compatibility decision after read-only evidence
shows the installed library uses fixed Hyperframe opcodes34-37, overlapping
canonical SHUTDOWN/RENAMEAT/UNLINKAT34-36. New completion/cache/resource-ref
ownership also differs from the old explicit completion-plus-free bridge.
Next is a single upstream io_uring implementation with an explicit mode and
minimal real layout/lifetime adaptation, not disabling Hyperframe or silently
renumbering installed userspace. At that161 checkpoint the remaining162-165
originals were saved previews, not yet imported or accepted. Their completed
next milestone follows. Target274 and
ACK/vendor-core/GKI/KMI plus inherited hardware/power findings remain open.

### Linux 5.10.162-165 Single-Core Migration And #77

All1,015 canonical originals are resolved in order:959 exact picks,35
adaptations,19 already-present and two inapplicable entries. The994 source
commits retain raw authors, dates, complete messages and stable trailers;
all four release trees match the approved projection through `6935346ef`.
Separate `da36d4e1f` corrects four renamed raw-HF callback `.size` labels,
without changing instructions or policy. Per-release ledgers record every
original and adaptation, including selected HW_F2FS, DWC3, cpufreq, VFS,
encryption and mixed-tree module-build providers.

Canonical162's5.15.85-derived io_uring/io-wq replaces the old fs core rather
than keeping two implementations. The user-approved default/standard ring
mode preserves installed userspace without aliases or renumbering. Actual
task/request/context and rw/open layouts drive22 generated HF selectors;
typed task-work completion, separate issue references, single dismantle,
delayed-worker drain and crypto vote release before fput are checked in
current objects. Final ELF validates84 targets in50 CFI groups plus15 typed
raw bridges. Selected core/DDR/GPU devfreq and net-stats consumers agree.

One complete #77 Image/modules build exits0 with unchanged configuration,
signing material and enforcing controls. The export table gains six symbols,
loses one and changes8,966 CRCs; all390 rebuilt modem and two optional dummy
imports match. The14 original OEM selectors remain. Signed boot/modem
staging, paired installation/readbacks and automatic Live modem pass; the
independent ramdisk is unchanged. Complete logs contain no actual kernel,
module or HKIP fault;561 AVC denials remain, none for module loading.

A finite UID1013/capabilities-zero private regression passes defaultHF34-37,
standard34-36, mode-specific boundaries/restrictions, real private file
operations and fixed-file replacement/error/async-close last-reference
release. Earlier probe failures were its unaligned HF buffer and nonzero
CLOSE tag, not new kernel faults; only the harness was corrected. No global
trace, queue or policy changes occur. Trace state and Asleep are restored.
Accepted77 metadata and recent73-77 pairs stay local below20 GB. Next is
complete166-170 toward pinned274; ACK/vendor-core/GKI/KMI and inherited
subsystem/hardware/power findings remain open. No release is exported or
published.

### Linux 5.10.166-170 And #78

All371 canonical originals are resolved in order:336 exact picks,17
adaptations,16 already-present and two absent/unselected Qualcomm DWC3
entries. The353 source commits end at `1e81b711ac`; all five release trees
match the approved projection. Raw authors/dates and stable trailers remain.
One declared168#74 message exception collapses a duplicate blank line after
a lore URL; no nonempty line changes, raw provenance remains and history is
not rewritten. Future imports use verbatim message cleanup.

Standard warning/oops controls and make_task_dead preserve OEM fault hooks
and ordinary exit cleanup. Managed clocks, NVMEM cell/publication fixes,
PSI pollfree, BPF verifier and named VFS adaptations retain real hardware
providers and security contracts. One complete #78 Image/modules build
exits0. Eleven new ordinary objects establish current typed/layout facts;
final ELF checks97 targets in55 CFI groups and actual raw bridge continuity.
Unchanged HF primary providers/selectors retain77 proofs without a private
probe replay. Configuration, signing material and enforcing controls stay.

Four new managed-clock exports and three NVMEM-related CRC changes are
accounted for. All390 rebuilt modem and two optional dummy imports match.
Signed matching boot/modem staging and paired installation pass, with the
independent ramdisk unchanged. Normal Android automatically loads the Live
modem; full readbacks/loaded build-id, unlocked CE, services and six-stage
ISP startup pass. Complete logs have no actual kernel/module/HKIP fault;
518 AVC denials remain, none for module loading. Read-only warn/oops counts
are0; no fault or limit write is induced. Trace0/wifi and Asleep are restored.

A later upstream child-clock resource-size fix remains source-only open;
all six current callers are unselected and modem does not import that API.
This is not whole-clock lifetime or hardware/power certification. Compact78
metadata and recent74-78 pairs stay local below20 GB, with77 immediate
rollback. Next is complete171-175 toward pinned274. ACK/vendor-core/GKI/KMI
and inherited findings remain open; no release export/publication occurs.

### Linux 5.10.171-175 And #79

All647 canonical originals are resolved in order:615 exact picks,20
adaptations,10 already-present and two absent/unselected regulator entries.
The635 source commits end at `aa4c719872`; all five release trees match the
approved projection and all raw authors/dates/messages retain exact original
content with declared trailers. Two separate upstream-backed corrections,
`78a7868847` and `a0dc72d14b`, prevent the canonical backport's poll-allocation
NULL dereference and ext4 rename error-path double unlock before any build.

Fixed-file locking includes its immediate NONBLOCK followup; no171-only
intermediate is built. All four F2FS fixes also reach actual selected
HW_F2FS, preserving its quick-write, crypto and writeback semantics. Fresh
actual poll/task/HF types and selected ext4/HIDPP/trace/irq/PCI contracts,
112 targets in64 final CFI groups and raw bridge/worker offsets pass.
One complete Image/modules build exits0. Effective configuration, signing
material and36 selectors stay; only two already-disabled configuration
lines naturally disappear through canonical Kconfig changes.

All13,854 exports remain, with21 real CRC changes and no provider/type/
namespace change.390 freshly rebuilt modem and two optional dummy imports
match. Signed matching boot/modem staging and installation pass; independent
ramdisk stays unchanged. Normal Android, automatic Live modem, full readbacks/
loaded build-id, unlocked CE, services and six-stage ISP startup pass.
Three new UID1013/caps-zero private regression groups pass async fixed-input
splice/replace/bad-slot cleanup, ordinary poll content/EOF, and anonymous/
shmem/mixed-source buffer registration with error recovery. No old HF suite,
system queue, forced spurious event or memory-pressure/fault test is replayed.

Complete logs contain no actual kernel/module/HKIP fault;533 AVC denials
remain, none module-load. Enforcing, taint4, mandatory signatures and PRMEM
stay; read-only warn/oops counts are0 and limits0/10000. Trace0/wifi and
Asleep are restored. Compact79 metadata and recent75-79 pairs stay local
below20 GB with78 immediate rollback. The unselected child-clock resource
issue remains source-only open. Next is complete176-180 toward pinned274;
ACK/vendor-core/GKI/KMI and wider hardware/network/power work remain open.
No release export, tag, push or publication occurs.

### Linux 5.10.176-180 And #80

All841 canonical originals are resolved in order:777 exact,40 adapted,21
already present and three absent/unselected entries.817 source commits
retain all raw original authors, timestamps/timezones and complete messages
plus declared trailers. All five release trees match the approved projection.
Ordered source ends at `b863b88f8e`; separate `baa22bf578` removes one imported
PowerPC comment trailing space without rewriting the canonical commit.

Standard VFS SGID handling replaces the obsolete helper and updates actual
callers. Scheduler long-sleeper/migration and uclamp capacity-inversion groups
retain selected HISI EAS, WALT, SMT and isolation contracts. All six F2FS
fixes also reach selected HW_F2FS: quota errors, paired capacity consumers,
raw compressed-page writeback, cached IPU selection and moved-directory
locking. IPv6 destructor ownership, locked NF validation reset, DWC3 public
EndTransfer/delay and suspend state, TTY write serialization, trace IRQ-work
drain/reset and deferred-printk zonelist locking remain scoped to real paths.

One prepare and one full Image/modules build exit0 with Android Clang/LLD
12.0.7 and retained security.511 actual ordinary contexts and35 necessary
objects bind current layouts, including five matching rq providers, current
task/thread, selected MAS bio/request/UFS offsets and56-byte nfnetlink
subsystem. The Android KABI reserve invocation remains, but this tree's
macro emits no padding; no stable-KMI claim follows.139 final targets in81
CFI groups, current UDP destructor stores, protocol/NF/PM tables and raw
crypto/thread operands pass. Direct-only TTY helpers are not labelled as
indirect CFI targets. Generic inline crypto, DCCP/SCTP and zoned F2FS remain
unselected source-only work; unchanged HF/PCI IR is not re-decoded.

All13,857 exports are compared with79's13,854: eight additions, five removals
and2732 changed CRCs, with no common provider/type/namespace change.390
freshly rebuilt modem and two optional dummy imports match. Original-format
boot/modem staging, signatures/CMS and metadata/readback checks pass. Only
the matching boot/modem pair is installed; independent ramdisk stays.
Normal Android, automatic Live modem/loaded build-id, unlocked CE/services
and six-stage ISP startup pass. Complete logs have no actual kernel/module/
HKIP fault;519 AVC denials remain, none module-load. Enforcing, mandatory
signatures, taint4 and PRMEM stay; read-only warn/oops counts are0, limits
0/10000. Original trace0/wifi and Asleep restore.

Compact80 metadata and recent76-80 test pairs remain local below20 GB with79
immediate rollback. Originals, signing materials, referenced proof/blob/
alternate stores and failed71/65 evidence remain. The unselected child-clock
allocation issue and full hardware/network/performance/power/ACK/GKI/KMI
work remain open. Next is complete181-185 toward pinned5.10.274; no release
export, tag, push or publication occurs.

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
- `out/allen-clang12-upstream-5.10/`: installed 5.10.200 #86 output, reused sequentially for stable milestones; not tracked.
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
Makefile still controls the release, currently `5.10.200` on the upstream
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

### Complete Canonical 5.10.201-205 And Accepted #87

The complete ordered stable range from v5.10.200 through v5.10.205
(`ca4427ebc6269ed2c255db88dc6e7bd2036c3103`) is integrated: 657 originals,
611 exact, 29 adapted, 14 already present and three inapplicable absent
Qualcomm DWC3 changes. The five per-release import ledgers preserve original
provenance and dispositions; 640 ordered original-author commits are retained.

Selected adaptations retain OEM hardware/security contracts while carrying
upstream hrtimer hotplug migration, atomic-long credential references, perf
lost-event accounting, trace-file lifetime fixes, device removable state,
MMC regulator state, ext4 allocation ordering and both F2FS implementations.
The first actual build error was an OEM one-argument `array_size` macro
colliding with upstream saturating arithmetic. Separate commit `652175b840`
uses standard `ARRAY_SIZE` in five bootfail files and removes both obsolete
header macros; the upstream overflow helpers remain unchanged.

The unchanged effective/emitted configuration builds Image and modules with
Android Clang/LLD12.0.7, ThinLTO, enforcing CFI, Werror and original signing
material. Current natural compilation contexts and shared provider/consumer
layouts cover device, credentials, perf, MMC, GPIO, tracing and retained
IO/HF/network contracts. Final ELF checks cover 122 CFI groups, 236 targets,
current table stores and actual HF/GPIO registrations. Both retained local
verity work callbacks are checked separately; direct-only trace reference
helpers are not misrepresented as indirect CFI callbacks.

All 390 modem imports and two optional dummy imports match. Versus #86,
four exports are added, none removed and 8,986 common CRCs change. Same-build
signed boot/modem images pass staging and device readback checks. Normal
Android #87 automatically loads the matching Live modem; loaded build ID,
critical services, unlocked CE and automatic six-stage ISP startup pass.
SELinux Enforcing, signature enforcement, PRMEM, taint4 and the independent
Magisk ramdisk remain unchanged. Complete boot/later logs show no actual
kernel/module fault; WARN/Oops counts are zero. The 508 visible AVC denials
include no module-load denial. Trace0/wifi and Asleep screen are restored.

Accepted #86/5.10.200 is immediate paired recovery. Next is complete206-210
toward pinned5.10.274. Android common parity, OEM/core separation and stable
KMI/GKI certification remain open, as do broad hardware/network/performance/
power validation and the known configfs cursor race. This is local source,
build and necessary boot/module acceptance, not a release publication.

### Complete Canonical 5.10.206-210 And Accepted #89

The complete official v5.10.205-exclusive through v5.10.210-inclusive range
ends at `aa6ca808a46767ba80d92d79d5cfd308f4d71b73`. All785 records are accounted
for:709 exact,36 adapted and40 already covered, preserving745 ordered
original-author commits. The five per-release ledgers retain provenance.
Source endpoint is `e2e06739bad6`; no separate out-of-range fix was needed.

Adapters preserve real MAS/SCSI hooks through the original206 conversions,
207 reverts and corrected batch flag fix; selected HW_F2FS receives compressed
move rejection, fault accounting and recovery repairs. Other adaptations
retain OEM Binder diagnostics with corrected async accounting, bounded packed
trace_marker text starting at its real ip/buf location, MMC health-detect
arguments with full SBC pairing, PM diagnostics with unlocked reference drops,
and distinct online CPU-isolation versus dying hrtimer behavior. Complete210
already includes the official dpm_prepare error-loop correction at entry241.
An intermediate-source review briefly interrupted compilation unnecessarily;
the frozen final source contains the fix, and no duplicate patch was added.

Configuration preparation retains all effective/security options and all36
OEM generated values. Four obsolete unselected architecture options disappear
from emitted config only. Android Clang/LLD12.0.7, LLVM/IAS, ThinLTO, enforcing
CFI, Werror, BTI/BHB, signing material and PRMEM/HKIP remain. Initial #88 link
was interrupted at the20GB workspace cap. The user authorized a temporary24GB
limit; resumed complete Image/modules build produced actual #89. Consumed
link intermediates/cache were retired and workspace restored below20GB.

Current5050 natural contexts and100 necessary decoded objects bind changed
shared types and actual OEM consumers. Final checks cover125 CFI groups,
243 targets, current BPF three-argument release callbacks and table pointers,
LSM compat ioctl physical hook head/canonical pointer, hrtimer online state,
trace flexible storage and HF/GPIO registrations. Direct-only SCSI helpers
are not falsely claimed as indirect CFI targets. Unselected IPSET/hibernation
changes remain source-only, not runtime coverage.

All390 modem imports and two optional dummy imports match. Six exports are
added, none removed and9,000 common CRCs change versus #87. Same-build signed
boot/modem images pass staging and are installed as a pair. Normal Android
5.10.210/#89 automatically loads the matching Live modem; partition/file and
loaded build-ID readbacks agree. Independent Magisk ramdisk, Enforcing,
signature enforcementY, taint4, PRMEM, critical services, unlocked CE and
automatic six-stage ISP startup pass. Complete logs show no actual kernel or
module fault, WARN/Oops counts are0;511 AVC denials remain, none module-load.
Trace0/wifi and Asleep screen are restored.

Immediate paired recovery is accepted #87/5.10.205. Next is complete211-215
toward pinned5.10.274. Android common parity, OEM/core separation, stableKMI/
GKI certification, known configfs cursor race and broad hardware/network/
performance/power acceptance remain open. Local Git/build/device work only;
no release export, publication or independent old-module compatibility claim.

### Linux 5.10.211-215: Android Boot And Matching Modules On #91

The complete official range contains 763 entries (123, 42, 71, 233 and 294
per release): 701 exact imports, 21 adaptations, 40 already-present changes
and one absent PWM regulator implementation. The 722 ordered original-author
commits are recorded in the five release ledgers. Source ends at
`bc330d53cc9ddaf014cdc28c55406963d7b7524b`, including separate whitespace,
socket/devlink integration and timer-assembly corrections.

Four Huawei assembly calls now use upstream `timer_delete_sync` instead of
requiring the removed external `del_timer_sync` symbol. IO ring socket removal
changes the generated HF buffer offset from 1792 to 1784; current final-machine
checks cover that operand and HF callback registration. Current PCI consumers,
shared block layouts and SELinux sockptr callback types were inspected.
Clang/LLD 12.0.7 Image/modules build #91 completes with the existing security
configuration. The 390 modem imports and two optional dummy imports match
current exports; 3 exports are added, 2 removed and 5,195 common CRCs change.
This requires matching rebuilt modules and does not establish stable GKI KMI.

Paired boot/modem installation boots normal Android 5.10.215 #91 and loads
the matching modem automatically. Partition/module readbacks and loaded build
ID agree; independent Magisk ramdisk, SELinux Enforcing, signature enforcement,
PRMEM, CE storage, critical services and automatic ISP startup pass. Recorded
WARN/Oops counters are zero; 500 AVC denials include no module-load denial.
Trace state and Asleep screen are restored. An initial validation-script
version mismatch caused a rollback after candidate boot; the corrected
script subsequently completed scoped acceptance. Immediate recovery is
5.10.210 #89. The local evidence directory retains its original build90 lane
name, while the actual successful build is #91.

Next work is the complete 216-220 range toward pinned 5.10.274, followed by
explicit Android common/KMI comparison and OEM/core separation. GKI ABI,
full hardware/network/performance/power acceptance remain open. No release
export or publication is performed.

### Linux 5.10.216-220: 220 Boots With Matching Modules On #93

The frozen official range contains 1,334 entries: 1,274 ordered upstream
commits, 56 already-present patches and four not-applicable entries. The
five ledgers record 216-219 and 220 separately. The 220 ledger contains 751
exact imports, 18 source adaptations and two already-present changes. OEM
adaptations preserve the existing descriptor lookup lifetime, VFS rename
data/locking contracts and NFS filehandle attribute handling. Build-driven
repairs also update remaining OEM fdtable and `vfs_rename()` callers, and fix
the Livepatch kallsyms conditional boundary. Source release is 5.10.220 at
`c700caaf68`; two additional OEM integration commits follow that official
endpoint.

The Clang/LLD 12.0.7 `Image modules` build completes as build #93 with
ThinLTO, enforcing CFI, module signature enforcement, SELinux and the
matching 390-import modem module intact. `CONFIG_CPU_MITIGATIONS` is newly
enabled by the 5.10.220 source defaults; the prior source did not define it.
The signed modem and optional dummy module imports match current export CRCs.
This is same-build module compatibility, not stable GKI/KMI certification.
Boot repacking and modem signing/filesystem/metadata readback checks pass.

Paired `boot` and `modem_driver` installation boots normal Android
5.10.220/#93 on the target; the matching modem automatically loads as Live.
Partition/module readbacks and module build ID match the staged build. The
independent Magisk ramdisk is unchanged, SELinux is Enforcing, signature
enforcement and PRMEM remain active, essential services and unlocked CE
storage are available, and the existing automatic six-stage ISP startup
path passes. Complete boot logs contain no classified kernel/module faults;
WARN/Oops counters are zero and 511 observed AVCs include no module-load
denial. Trace state and asleep screen are restored. Immediate paired recovery
is 5.10.219/#92.

The workspace is below 20 GB after retiring consumed link intermediates.
No release image export or publication is performed. Continue with
5.10.221-225 toward the pinned 5.10.274 endpoint. Android common parity,
stable KMI/GKI certification, OEM/core separation and broad hardware,
network, performance and power acceptance remain open.

### Linux 5.10.221-230: Normal Android On #94

All 1,901 official source entries across 221-230 are recorded in the ten
release ledgers: 1,815 exact imports, 47 targeted adaptations, 35 already
present fixes and four not-applicable changes. Two source integrations follow
the official 5.10.230 endpoint; one carries xattr-directory checkpoint
tracking into HW_F2FS while retaining its strict-fsync path. The first full
build exposed a missing HW_F2FS checkpoint reason, corrected before the
accepted build.

Clang/LLD 12.0.7 Image/modules build #94, module signing and matching-module
CRC gates pass with ThinLTO, CFI, SELinux and PRMEM/HKIP retained. All 390
modem imports and two optional dummy imports match. Selected configuration
changes from accepted #91 are ARM64 erratum 3194386, CPU mitigations and
`CONFIG_PROC_MEM_ALWAYS_FORCE`; the signing certificate is unchanged. There
are 8,923 changed export CRCs, 19 added exports and three removed exports.
Matching modules are therefore required; this does not certify stable KMI
or GKI.

Paired `boot` and `modem_driver` installation reaches normal Android
5.10.230/#94; the matching signed modem loads automatically as Live. Staged
partition readbacks and loaded module Build ID match, while the independent
Magisk ramdisk remains unchanged. SELinux Enforcing, signature enforcement,
taint 4, PRMEM, essential services, unlocked CE and automatic six-stage ISP
startup pass. Scoped boot-log checks find no classified kernel/module fault;
WARN/Oops counters are zero and 518 visible AVCs include no module-load
denial. Trace state and asleep display are restored. Immediate paired
recovery is 5.10.220/#93. Consumed link intermediates and expired old image
pairs are cleaned, leaving the workspace below 20 GB; no release export or
publication occurs.

Next is the complete 5.10.231-240 range toward pinned 5.10.274. Android
common parity, stable KMI/GKI certification, OEM/core separation and broad
hardware, network, performance and power acceptance remain open.

### Linux 5.10.231-240: Accepted #95

The official 5.10.231-240 input contains 2,575 ordered source entries. The
ten ledgers record 2,496 exact imports, 67 targeted adaptations, eight fixes
already present in the selected tree and four not-applicable driver changes.
The final source is 5.10.240 plus two local compile-context corrections: the
TCP readiness helper is ordered after its pressure helper, and a duplicate
IPv6 cached device context from the retained OEM/UAF fixes is removed.

Build #95 uses Android Clang/LLD 12.0.7 with ThinLTO, CFI, forced module
signatures, SELinux and PRMEM/HKIP retained. The Image/modules build passed;
the signing certificate is unchanged. The modem module has 390 versioned
imports and the optional dummy module has two; both match the same-build
export CRCs and 5.10.240 vermagic. Export CRCs changed for 8,996 symbols,
with 11 additions and one removal, so matching modules remain required. This
is not stable KMI/GKI certification.

The paired 5.10.240/#95 `boot` and `modem_driver` images boot normal Android
on ALN-AL00 and automatically load the matching signed modem as Live. Boot,
modem, module and unchanged independent Magisk ramdisk readbacks match;
SELinux is Enforcing, signature enforcement and PRMEM remain enabled, taint
is 4, critical services and unlocked CE storage are available, and the
automatic six-stage ISP startup path passes. Complete relevant logs contain
no classified kernel/module fault; WARN/Oops counters are zero and 518
visible AVCs include no module-load denial. Trace state and asleep display
are restored. Immediate paired recovery is 5.10.230/#94.

The temporary ThinLTO and final-link intermediates were retired after the
successful build, returning the workspace below 20 GB. No release image
export, tag or publication was performed. Continue with 5.10.241-250 toward
the pinned 5.10.274 endpoint; Android common parity, stable KMI/GKI
certification, OEM/core separation and broad hardware/network/performance/
power acceptance remain open.

### Linux 5.10.241-250: Accepted #97

The frozen official 5.10.241-250 range contains 2,006 ordered entries.
Per-release ledgers record 1,939 exact picks, 56 targeted adaptations, six
fixes already present and five not-applicable changes. The upstream source
ends at 5.10.250 (`b29eaaa7a0`); three separate local follow-ups correct a
KSM page-reference release, avoid the new `in_range()` macro collision in
OEM WALT, and use explicitly typed min/max at eight OEM expressions while
preserving the previous C integer-conversion behavior. The 5.10.248 documentation-only
virtio adaptation has a recorded local-author metadata exception.

The first configuration-prepare attempt failed at arm64 `asm-offsets` with
`MIGRATE_CMA` undeclared even though the regenerated configuration has
`CONFIG_CMA=y`. A separate synchronized prepare and the final full build pass;
no CMA/security option was disabled, and the first failure's precise cause
is not established. The first full build also caught the WALT helper-name
collision and stricter minmax signedness checks; the narrow follow-ups were
compiled directly and the final Android Clang/LLD 12.0.7 `Image modules`
build #97 passes with ThinLTO, CFI, forced module signatures, SELinux and
the unchanged signing certificate. The signed modem's 390 and dummy module's
two versioned imports match this same build. The current export table has 19
additions, six removals and 8,994 changed CRCs compared with the retained
5.10.230/#94 baseline; this cumulative table comparison is not a GKI/KMI
certification. `CONFIG_KSM` is not selected in this device build, so the
separate KSM reference-pair correction has not received a target-object or
runtime test.

Only paired `boot` and `modem_driver` were installed from #97 over the
accepted 5.10.240/#95 system. The device starts normal Android 5.10.250/#97
and automatically loads the matching signed modem as Live. Partition and
module-file readbacks/build ID match, while the independent Magisk ramdisk
is unchanged. SELinux Enforcing, module-signature enforcement, taint 4,
PRMEM, critical services, unlocked CE, automatic six-stage ISP startup,
trace state and asleep screen pass. Complete relevant logs show no matched
kernel/module fault; WARN/Oops counts are zero. The 510 visible AVC denials
include none for module loading. Immediate paired recovery is #95. Consumed
ThinLTO and final-link intermediates and the selected #92/#94 image pairs were
retired, returning the workspace below 20 GB. No release export, tag or
publication was performed.

Next is the complete 5.10.251-260 range toward pinned 5.10.274. Android
common parity, stable KMI/GKI certification, OEM/core separation and broad
hardware/network/performance/power acceptance remain open.

### Linux 5.10.251-260: Accepted #99

The complete official range contains 1,879 ordered entries: 1,822 exact
picks, 46 adaptations, four fixes already present and seven changes without
a corresponding local implementation. Ten per-release ledgers preserve
the official sequence and each disposition. Local adaptations retain the
product's storage, power and scheduler contracts rather than replacing
whole vendor files.

The selected HW_F2FS implementation carries all five corresponding upstream
filesystem fixes, including scalar sysfs widths, write-completion lifetimes,
inline FIEMAP for unallocated inodes and ACL entry bounds. MMC write retries
retain single-block recovery through requeues and clear that state before
the next logical request reuses its tag. The ARM64 TLBI synchronization
refactor also covers this tree's extra batched-unmap completion hook.
The new upstream erratum default adds only two effective configuration
symbols; captured device MIDRs do not match its CPU list. Existing security
options and signing materials remain intact.

The first full compilation exposed an OEM accessory HID caller missed by
the upstream API conversion. It now passes the actual EP0 buffer capacity
separately from the event length. A final source audit also completed the
Bluetooth SRCU implementation after its header adaptation. Bluetooth is
disabled in the product configuration; the final incremental build did not
relink or advance the actual build number. The accepted products remain
5.10.260/#99, including the selected HW_F2FS corrections.

Android Clang/LLD 12.0.7 `Image modules` and selected driver, network,
tracing and filesystem builds pass. Final ELF checks cover enforcing CFI's
failure endpoint, the changed HID and ring-buffer interfaces, and 94
repeat-TLBI alternatives including the local batch paths. All 390 modem
and two optional dummy module imports match the same-build CRCs. Five
exports were added, four removed and 9,021 common CRCs changed relative to
#97. This establishes matching-module compatibility, not stable GKI/KMI.

Only paired `boot` and `modem_driver` were installed. Normal Android starts
and automatically loads the signed modem as Live; partition/module
readbacks and Build ID match. The independent Magisk ramdisk, SELinux
Enforcing, signature enforcement, taint 4 and PRMEM remain intact. CE,
essential services, automatic six-stage ISP startup, trace state and asleep
screen pass. Relevant complete logs contain no matched kernel/module
fault, WARN/Oops counts are zero, and none of the 517 AVC denials concerns
module loading. Immediate paired recovery is 5.10.250/#97. Consumed link
intermediates, caches and the expired #89 image pair were retired to keep
the workspace below 20 GB; the latest five image pairs remain.

Next is the complete 5.10.261-270 batch, then the remaining releases toward
the pinned 5.10.274 endpoint. Android common parity, OEM/core separation,
formal GKI/KMI and broad hardware, network, power and performance acceptance
remain open. No release export, tag, push or publication was performed.

### Linux 5.10.261-270: Accepted #100

All 2,199 official entries are accounted for in ten ordered release ledgers:
2,147 exact, 44 adapted, four present and four without a corresponding local
implementation. The range contributes 2,191 original-author commits. Final
source `4dd7b816cb` includes separately recorded corrections to selected OEM
consumers; all adapted originals were reviewed against their actual commits
and final behavior.

The selected HW_F2FS copy carries the corresponding capacity, pinned-range,
xattr/orphan validation, GC submission, writeback accounting, symlink and
move-range fixes. Huawei SDP uses the standard P256 ECDH transform while
retaining public curve/key validation. Schedutil stores utilization through
the canonical helper interface across the generic and HCK paths, preserving
WALT, the separate IO frequency floor and raw-util tracing. The highmem
conversion preserves MM_LB mapping. Perf sibling promotion retains terminal
states and consistent group accounting. SDHCI bounce-buffer cleanup follows
the MMC class-device lifetime.

Remoteproc retains the four-argument iomem translation contract and Huawei
resource handlers. ISP registration now follows complete initialization;
delete and worker shutdown precede backing-resource destruction. Live ISP
unbind, crash injection and autonomous detach remain outside acceptance.
The first whole compilation exposed a local anonymous-inode LSM caller that
still supplied the old default-result argument; its separate fix follows
the centrally defined hook default without changing authorization.

Android Clang/LLD 12.0.7 Image/modules succeeds as #100. Effective configuration,
signing materials and all 36 generated OEM constants remain unchanged.
ThinLTO, enforcing CFI, BTI, forced module signatures, SELinux and HKIP/PRMEM
remain enabled. Current checks cover 16 callbacks in four changed groups
and eight CFI types, including actual final table pointers; nine natural
HyperFrame/UFS/MAS assembly objects retain allocated bytes and relocation
semantics. Final ELF checks retain 94 repeat-TLBI alternatives and enforcing
CFI failure behavior. These are selected interface checks, not complete ABI
coverage or formal GKI/KMI certification.

All 390 modem and two optional dummy imports match this build. The staged
modem CMS signature and same-build payload pass. Relative to #99, 16 exports
are added, five removed and 2,634 common CRCs differ; modules require matching
kernel builds. Paired boot/modem installation starts normal Android
5.10.270/#100 and automatically loads the signed modem as Live. Readbacks
and live Build ID match, the independent ramdisk remains unchanged, and CE,
three essential services and same-thread six-stage ISP startup pass.
Enforcing, signature enforcement, taint 4, PRMEM, trace state and asleep
screen are retained. Relevant complete logs show no matched kernel/module
fault; WARN/Oops counts are zero and none of 518 AVC denials is for module
loading.

Immediate paired recovery is 5.10.260/#99. Consumed link intermediates,
caches, the consumed final ELF and expired #91 image pair are retired;
five recent image pairs and referenced evidence remain below 20 GB.
Continue with 271-274 toward the verified official 5.10 branch endpoint.
Android common parity, OEM/core separation, stable GKI/KMI and broad hardware,
network, performance and power validation remain open. Work stays in local
Git without release export or publication.

### Linux 5.10.271-274: Accepted #101, Current Stable Endpoint

The final four releases account for all 1,114 official entries: 1,093 exact,
18 adapted, two present and one without a corresponding local driver. The
range preserves 1,111 ordered original-author commits. Source `5b0cc1b997`
reaches the official 5.10.274 endpoint in sequence; the official branch and
release tag were checked again on November 20, 2026 and both still point to
`3a323bc22b881a5bdc1646080f93f5ecb22850bf`.

Selected HW_F2FS receives inline directory name/slot bounds, collapse-range
writeback errors and bounded recovery filenames. Its existing embedded GC
threads remain intact. The generic GC conversion preserves MAS fields and
all seven local consumers, including foreground-GC merging; generic F2FS
is disabled and receives no product runtime claim. Tick broadcast replacement
now serializes device publication and releases the old module outside the
lock. The mremap self-merge fix retains anonymous VMA names and speculative
fault arguments. Futex shared-mm identity and address-independent keyring
indexes migrate as complete field/initialization/lookup transactions. Empty
local KABI macros do not provide layout compatibility for their real fields.

The full Android Clang/LLD 12.0.7 Image/modules build succeeds as #101 without
a follow-up source correction. Effective configuration, signing materials
and all 36 generated OEM constants stay unchanged; ThinLTO, enforcing CFI,
BTI, forced signatures, SELinux and HKIP/PRMEM remain enabled. Current natural
objects verify mm/signal/task/futex and key overlay layouts, actual new-ID
stores, physical callback tables, IO write ownership and void TTY hangup.
Seventeen callbacks and eleven CFI types pass. HyperFrame retains its two
existing write-end branches without a duplicate completion release. Nine
raw OEM objects preserve allocated bytes and normalized relocations; final
ELF checks retain enforcing CFI failure handling and 94 TLBI alternatives.

All 390 modem and two optional dummy imports match current CRCs/vermagic.
The signed modem payload and CMS verification pass. Relative to #100, six
exports are added, one removed and 9,038 common CRCs differ. Matching modules
must still accompany each kernel; this is not stable Android KMI/GKI.
Only paired boot and modem_driver are installed. Normal Android starts as
5.10.274/#101 with automatic signed Live modem and matching readbacks/Build
ID. Independent ramdisk, Enforcing, signature enforcement, taint 4 and PRMEM
are retained. Unlocked CE, three services, same-thread six-stage ISP, trace
state and asleep screen pass. Relevant complete logs contain no matched
kernel/module fault, WARN/Oops counts are zero and none of 532 AVC denials
is a module-load denial. Live ISP unbind, broad hardware/network, performance
and power coverage remain outside this acceptance.

Immediate paired recovery is 5.10.270/#100. Retain the five recent accepted
image pairs #95/#97/#99/#100/#101; consumed linker caches/intermediates,
final ELF and expired #93 pair are retired after their evidence is recorded.
The workspace is below 20 GB and the local source/Git checkpoint is ready
for the next phase. This completes the current stable-update/boot milestone;
Android common parity, OEM/core separation and formal GKI/KMI restoration
remain future work. No release export, tag, push or publication is performed.
