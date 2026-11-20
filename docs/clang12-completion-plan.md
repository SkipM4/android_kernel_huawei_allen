# Bring-up Completion Plan

## Acceptance Rule

User requests stepwise kernel repair and validation. The latest instruction
ends work in the local source Git tree; no artifact export, release bundle
or publication is requested. The newly requested stage imports Linux 5.10
stable patches before progressively aligning with Android common/GKI, then
explicitly authorizes matched-module installation and boot testing.
For subsequent stable milestones, normal Android boot and automatic matching
module loading are the user's required runtime gate. Keep paired-build,
readback, security and boot-log sanity checks; do not replay passed full
hardware regressions without a new relevant change, fault or unresolved issue.
Installed candidate is #86/5.10.200 on `upstream/linux-5.10.y`, kernel source
`c0e3bac720`; immediate paired rollback is #85/5.10.195. Complete196-200
resolves601 originals with587 original-author commits. #86 full build,118
CFI groups/230 targets, current shared layouts, signed matching modules,
normal Android/automatic Live modem/CE/services/ISP and WARN/Oops counts pass.
The #84 configfs recursion and official revert remain recorded in history;
the target is running normally, not in recovery. The original configfs cursor
race, ACK/vendor split, GKI/KMI and full hardware/power remain open.
Accepted79 private-ring evidence is retained, not replayed.
The accepted162-165 single-core explicit-standard-mode/default-Hyperframe migration passes
current build/layout/CFI, normal Android/automatic modules and nine finite
private-ring checks. Retained HF requires aligned buffers and uses CLOSE
user_data as cancel/unlink; general-buffer, concurrency and full hardware
acceptance do not follow from this pass. #71 is rejected
for the OEM post-fork CPU-lock assertion, fixed in #72 without disabling it.
#65 remains a
failed HKIP firmware-boot candidate with successful paired #64 recovery;
r49/5.10.43 remains on
`bringup/clang12` at validation record `13570860d` (kernel source `09aeaf9ec`).
Each kernel iteration stays isolated, with matching boot/modem_driver
validation, unchanged independent
ramdisk unless separately required, retained security controls and accurate
functional scope.

An item is complete only after its real acceptance condition passes. Source
availability, missing peers/SIMs and manual checks are external prerequisites,
not successful tests. A harmless/inapplicable diagnostic may be classified
instead of suppressed, but its functional uncertainty must be resolved before
closing the corresponding item. No dummy interface, fake success, protection
bypass, speculative broad backport or implementation-mirroring test suite.

## Upstream Synchronization Stage

`upstream/linux-5.10.y` preserves the r49 bring-up branch and imports the
first adjacent stable range, 5.10.43 -> 5.10.44. All 131 entries are resolved:
103 exact picks, four code adaptations, one release-marker adaptation,
17 already present and six absent/unselected platform-driver patches.
The 108 imported commits retain original author/provenance and stable SHA
trailers; one separate DWC3 OEM IRQ/PM cleanup repair ends the source batch
at `dcb7a62fb`. The original Android common merge of the same stable release
is verified, not imported as an unrelated-history merge.

Independent full compilation produces 5.10.44 #50 with the r49 effective
configuration and signing material, enforcing CFI and ThinLTO. All 13,821
export CRCs remain equal; all 390 new modem imports match. Final ELF and
temporary-module CMS signature checks pass. The configuration files differ
by their version comment and newly visible disabled NVME_TCP entry, not by
effective selections. Initially no image is staged or installed. A later
module-reference audit corrects the claim that changed vermagic always
forbids loading: MODVERSIONS/version sections allow the loader to ignore
the release token, while the project deliberately retains same-build pairing.
Export equality alone does not certify complete ABI or GKI/KMI.

The user-requested follow-up stages and installs matching #50 boot/modem
images while retaining the independent ramdisk and r49 rollback. Two normal
#50 boots have complete logs without actual kernel WARN/fatal/module-load
faults. The first succeeds but an unsupported user-state diagnostic triggers
an unnecessary automatic r49 rollback; after correcting that script, the same
pair is installed again with no new kernel edit or compilation. Matching readbacks,
live rebuilt modem build-id, CE storage/services and Enforcing/taint4/signatureY/
PRMEM pass. USB2MiB transfer, O_DIRECT2MiB write/fsync/read/compare and bounded
real frame-stop/scheduling regression pass; six activity-QoS refusals and
boot/regression AVCs remain classified, not hidden. Task files/traces are
removed and the device stays #50/Asleep. Full hardware/power/GKI remains open.

| Step | Work | Acceptance Condition | State |
| --- | --- | --- | --- |
| U01 | First stable batch through 5.10.44 | Ordered provenance ledger, duplicate/absence classification, reviewed adaptations and matched build/installation with scoped regression | #50 Installed; Source/Build/Pair/Boot/USB/Direct-I/O/Frame-Stop Pass; Full Runtime Open |
| U02 | Next 5.10.y batches, starting at 5.10.45 | Bounded ordered imports toward a pinned target; matched build, normal Android boot and automatic module loading at milestones, retaining paired rollback | All Ordered Entries Through5.10.200 Resolved; #86 Build/118-Group-230-Target CFI/Signed Pair/Normal Android/Auto Module/CE/ISP/WARN-Oops Pass; #85 Immediate Recovery; Failed84/71/65 Evidence Retained; Next Complete201-205 Toward Pinned274 |
| U03 | Android common alignment | Audit version-matched ACK deltas and preserve required hardware/userspace contracts | Not Started; Stable Picks Are Not ACK Parity |
| U04 | Vendor/core split and GKI preparation | Audit OEM hooks, vendor-module dependencies, configuration/KMI and relevant installed regressions | Not Started; No GKI Certification |

Detailed sources, adaptations and validation limits are recorded in
`linux-5.10-upstream-sync.md`; `linux-5.10.44-imports.tsv` records every
stable/local identity and exclusion. `kernel-module-updates.md` records
official module/KMI references and the repeatable same-build update method.
Consumed upstream diagnostics are
removed after recording, while original images, r49 output, #50 output and
recent r46-r50 rollback pairs remain local and below 20 GB. This closes only
U01's source/build and named runtime gates, not the complete stable series
or inherited device acceptance queue. No exported release or publication
is created; local test/recovery images are retained as required for this test.

The next source milestone resolves 5.10.45-49 with 258 exact picks, 14 code
adaptations, five release markers, 18 already-present fixes and three absent
unselected regulator drivers. Source ends at `41fba098d`; #51 passes full build,
paired installation, normal Android/RUNNING_UNLOCKED and automatic matching
Live modem. Effective configuration/signing material match #50; 13,821
exports remain, with one get_net_ns CRC change and no modem mismatch. Complete
boot logs have no actual WARN/fatal/module faults; 489 AVCs remain classified,
none for module loading. No broad hardware regression is repeated. #50 is
immediate rollback, latest r47-r51 pairs are kept, and replaced #50 build
metadata is retained. The retired r49 cache also has verified metadata;
its rollback pair and bring-up branch remain intact. Per-release ledgers and
real DWC3, kthread, THP and DMA adaptations are in the synchronization record.

The third milestone imports all 596 official 5.10.50 entries and minimal
proven credential/PSI follow-ups. Two unsafe intermediate swap fixes are
officially reverted before installation because their percpu_ref prerequisite
is absent. A separate DWC3 integration repair drains debugfs readers before
endpoint release. Source `75f387b5d` builds as #52 with the unchanged effective
configuration/signing material. Final layout and machine-code lifetime checks
pass. The export table changes substantially: two additions, one removal and
8,878 changed CRCs; all 390 rebuilt modem imports and two optional dummy imports
match. Signed paired staging/installation/readback and one normal Android boot
with automatic matching Live modem pass. Ramdisk, CE storage/services and
Enforcing/taint4/CFI/signatureY/PRMEM hold; complete early logs show no actual
WARN/fatal/module fault. The 494 later boot AVCs remain classified, not hidden.
No broad hardware regression is repeated. #51 is immediate rollback and
latest r48-r52 pairs remain local; metadata is preserved before output reuse.
Next is an ordered 5.10.51-55 batch, not a claim of reaching 5.10.274 or GKI.

The fourth milestone resolves all 767 official 5.10.51-55 entries: 712 exact
picks, twelve code adaptations, five version markers, 36 already-present and
two inapplicable entries. Source `2f2c2dc22` also includes the official early
XArray wrap fix and separate whitespace cleanup. #53 full compilation,
fresh affected C layouts, final bool wait-function CFI, matched imports and
signatures pass. The export table adds one symbol and changes 58 CRCs;
all 390 modem imports match. One signed paired installation reaches normal
Android with automatic matching Live modem, matching partition/module
readbacks, unchanged ramdisk and CE/core services available. Enforcing
controls hold and complete time-zero/subsequent logs contain no actual
kernel or module fault; 545 AVC denials remain visible and none requests
module loading. The screen returns to Asleep without a broad hardware replay.
#52 is immediate rollback, latest r49-r53 pairs remain local, and #53 metadata
is retained before output reuse. Next is 5.10.56-63; its upstream MSI lock
requires a real OEM assembly-offset migration gate, not compatibility padding
or skipping the standard field. Neither latest stable nor GKI is complete.

The fifth milestone extends the initial 5.10.56-63 range through complete
5.10.64 to satisfy the real flush-reference publication prerequisite of
the standard MAS iterator migration. All 614 entries are resolved with
original provenance. Separate integrations generate twelve offsets for
29 proven assembly operands and coordinate restricted-hook and opaque TTY
declarations without weakening protection or removing hardware paths.
Source `378a72fba` builds as #54; final actual C layouts, assembly/stack
contract, once four-argument modem CFI and flush canonical publication checks
pass. The table adds four exports, removes one and changes 8,949 CRCs;
all 390 rebuilt modem imports match. Signed paired installation reaches
normal Android with automatic matching Live modem, runtime build-id and
complete file/partition readbacks. Ramdisk/CE/services/security remain;
complete logs contain no actual fault and retain 546 AVC denials, none for
module loading. Screen/trace state is restored without a broad hardware
replay. #53 is immediate rollback, latest r50-r54 pairs and metadata remain
local. Next is complete 5.10.65-70, not latest stable or GKI completion.

The sixth milestone resolves all 842 official entries through 5.10.70:
798 exact picks, twenty code adaptations, six markers, thirteen already
present and five absent/unselected alternatives. Standard const list_sort
is coordinated across its real callers without the earlier private cast.
The separate MAS all-tag migration reuses standard static/dynamic flags,
protected tag/ref lookup and flush-aware put with the existing three-bank
numbering. Source `a08144e8a` builds as #55. Fresh C records, final const
CFI/canonical entries, all-tag control flow and twelve-offset continuity
pass; unchanged nine assembly sources reuse the accepted scoped #54 proof
without a second 29-operand audit. All 390 modem and two optional imports
match, despite fifteen changed CRCs and one new export. Existing selections
remain; one corrected Kconfig dependency makes a disabled clock selector
visible. Signed matched installation, normal Android, automatic Live modem,
full readbacks/build-id, unchanged ramdisk/security and complete logs pass.
The screen returns to Asleep, with no broad hardware replay. #54 is immediate
rollback, r51-r55 pairs and metadata remain local below 20 GB. Next is
complete 5.10.71-75 with real MDIO-layout/FIB-function gates, not latest
stable, full hardware or GKI completion.

The seventh milestone resolves all 330 official entries through 5.10.75:
309 exact picks, five code adaptations, five markers and eleven already
present fixes. The selected schedutil exit retains its last-policy cache
with a temporary kobject reference across put/save/clear; the standard
release and mutex-lifetime fixes complete before building. Source
`99ff6138b` builds as #56. Fresh C objects and final enforcing CFI validate
MDIO's appended shutdown member and standard five-argument FIB calls.
The accepted const-sort/all-tag implementation and mapped assembly proofs
are reused through scoped source/configuration/type/final-address continuity
checks, not replayed. All 390 modem and two optional imports match the three
changed export CRCs; all 6,432 configuration entries and signing material
remain. Signed matched installation, normal Android, automatic Live modem,
full readbacks/build-id, unchanged ramdisk/security and complete logs pass.
The final log retains 517 AVC denials, none for module loading; tracing and
the Asleep screen are restored without broad hardware replay. #55 is
immediate rollback, r52-r56 pairs and metadata remain local below 20 GB.
Next is complete 5.10.76-80, not latest stable or GKI completion.

The eighth milestone resolves all 779 official entries through 5.10.80:
732 exact picks, nineteen code adaptations, five markers, twenty-two already
present fixes and one inapplicable entry. The revised standard post-fork
initialization precedes selected OEM QoS/uclamp trace consumers while the
child's css_set is pinned. A separate RCC/PERF private-helper rename fixes
the first real full-build error without changing upstream idle accounting.
Source `02e9deb3a` builds as #57. Five final layout/CFI/source-context reports
bind 31 distinct actual objects and the same final ELF; seventeen distinct
final selected record names are measured, not a full opaque OEM ABI promise.
New BPF/console/Qdisc/stream/TCP orphan contracts and enforcing callbacks pass;
existing offset/const-sort/all-tag proofs retain explicit scoped continuity.
All 13,827 exports remain with 2,051 changed CRCs; all 390 modem and two
optional imports match. All 6,432 configuration entries and signing material
remain unchanged. Signed paired installation, normal Android, automatic
matching Live modem, full readbacks/build-id, CE storage/core services and
unchanged ramdisk/security pass. Complete logs contain no actual kernel or
module fault; 517 AVC denials remain visible, none for module loading. Tracing
and Asleep screen are restored, with no broad hardware replay. #56 is the
immediate rollback; #53-57 pairs and accepted #57 metadata remain local below
20 GB after retiring obsolete #52 images by retained contract. Next is the
complete 5.10.81-85 range with a fresh UFS layout/generated-offset/assembly
gate. Neither the pinned 5.10.274 target, Android common parity nor GKI is
complete; no release export or publication occurs.

The ninth milestone resolves all 557 official entries through 5.10.85:
519 exact picks, sixteen code adaptations, five markers, sixteen already
present fixes and one N/A entry. All 540 original-author commits, complete
ordered historical source inventories and five TSV ledgers pass independent
verification. A separate two-file UFS integration replaces measured stale
state and nested command-lock operands with compiler-generated offsets,
without padding away the upstream task-management pointer insertion.
Integrated source `0d678d58e` builds naturally as #58. One eight-record
frontend, fourteen generated constants and thirteen freshly checked affected
HUFS machine operands agree; eighteen unrelated operands retain explicit
scoped continuity. Five final ABI/CFI reports bind 29 distinct actual objects
and thirteen selected physical record names. UFS/HUFS, SDHCI, IPv6, inode,
atomic BPF and selected OEM F2FS trace consumers pass their bounded gates;
const-sort/all-tag implementation evidence is reused with fresh identity.
The 13,829 exports have four additions, two removals and 2,575 changed CRCs;
all 390 modem and two optional imports match. Effective configuration and
signing material remain; emitted options change 6,432 -> 6,431 only because
the already-disabled FORTIFY line disappears under upstream's Clang rule.
Signed paired installation, normal Android, automatic matching Live modem,
full readbacks/build-id, CE storage/core services and unchanged
ramdisk/security pass. Complete logs contain no actual kernel/module fault;
501 AVC denials remain, none for module loading. Tracing and Asleep screen
are restored without a second boot or broad hardware replay. #57 is immediate
rollback; #54-58 pairs and accepted #58 metadata remain below 20 GB after
contract-checked retirement of obsolete #53 images. Next is complete
5.10.86-90 with fresh selected MMC shutdown/BPF control/virtio-consumer gates;
the legacy selected AMA mempolicy path remains separately unaudited. The
pinned latest-stable target, Android common parity, GKI and full inherited
hardware/power queue are not complete. No release export or publication
occurs.

The tenth milestone resolves all 259 official entries through 5.10.90:
229 exact picks, seven code adaptations, five markers and eighteen PRESENT
fixes. All 241 original-author commits and ordered historical inventories
pass independent verification; source ends at `74057806b`, with no new
integration commit. The earlier UFS repair remains. #59 passes full build
and five final reports bound to 46 actual objects. Thirteen existing record
proofs retain explicit continuity; only class and ctl_table are newly
measured from natural selected objects. Actual MMC/BPF initializer pointers,
types and enforcing indirect dispatch, selected TUN/PACKET inline helpers
and FUSE direct callers pass. Unchanged layout/assembly/const/all-tag proofs
are not recompiled or replayed. All 13,829 export CRCs are equal to #58 and
390 modem/two optional imports match. Effective selections and signing
material remain; the only emitted delta is the new BPF bool explicitly n.
Signed paired installation, normal Android, automatic matching Live modem,
full readbacks/build-id, CE storage/core services and unchanged ramdisk/
security pass. Complete logs contain no actual kernel/module fault; 516
AVCs remain visible, none for module loading. Tracing and Asleep are restored
without a second boot or broad hardware replay. #58 is immediate rollback;
#55-59 pairs and accepted #59 metadata remain below 20 GB after contract-
checked retirement of #54 artifacts. Next is complete 5.10.91-95, then a
separate early official `614811692` native XFRM MTIMER policy fix before
building; that fix is not hidden in the original 663-entry range. Selected
AMA mempolicy, latest stable, Android common, GKI and inherited hardware/
power acceptance remain open. No release export or publication occurs.

The eleventh milestone resolves all 663 official entries through 5.10.95:
641 exact picks, eight code adaptations, five markers, seven PRESENT fixes
and two missing-driver exceptions. Independent ordered import verification
covers all 654 original-author source commits through `e4353a21c`. A separate
exact official native XFRM MTIMER NLA_U32 policy fix ends build source at
`ec2b3b670`; it is not folded into the original range. Its selected deprecated
parser rejects undersized payloads before the reader, without a live malformed
input probe or whole-XFRM safety claim. The accepted UFS offset repair remains.
#60 passes full compilation and five final ABI/CFI reports binding 72 actual
objects. Fifteen previous record proofs, fourteen generated offsets and the
accepted assembly scopes retain explicit continuity. Eight changed records
plus one same-source XFRM callback-member context, nine typed public exports,
selected Perf acquire/RCU dispatch and UFS early publication are checked from
necessary natural objects. Only list_sort.o is byte-identical; five changed
sort callers and four changed MAS all-tag objects receive bounded current
checks. No private frontend or old implementation/assembly replay is needed.
All 13,829 exports remain with 838 real CRC changes; all 390 rebuilt modem and
two optional dummy imports match. Every effective/emitted configuration option
and signing material remain. Signed paired installation, normal Android,
automatic matching Live modem, complete readbacks/build-id, CE storage/core
services, unchanged ramdisk, Enforcing/CFI/forced signatures and PRMEM pass.
Complete logs contain no actual kernel/module fault; 517 AVC denials remain,
none for module loading. Tracing and Asleep are restored without another boot
or broad hardware replay. #59 is immediate rollback; #56-60 pairs and accepted
#60 metadata remain below 20 GB after contract-checked retirement of #55 test
artifacts. Next is complete 5.10.96-100. Selected AMA mempolicy, pinned latest
stable, Android common, GKI and inherited hardware/power acceptance remain
open. No release export, publication or push occurs.

The twelfth milestone resolves all 208 official entries through 5.10.100:
189 exact picks, three code adaptations, five markers and eleven PRESENT
fixes. All 197 original-author source commits through `25e0e4b59`, five
ledgers and ordered historical identities pass independent review. Separate
one-line cpuset assertion integration `96e409478` fixes the actual build
failure by naming the retained hierarchy mutex; the complete sibling CSS/RCU
lifetime and both direct/three outer lock contracts remain. It is outside
the original 208 entries. The integrated binder retains 328 original source
identities plus the real cpuset file, 329 in all, and 75 object/entire-command
witnesses. Effective/emitted configuration and signing material remain.
#61 full compilation and five final reports bind 75 actual objects. Three
changed Perf records and their core/ARM PMU/ARM64/OEM phase consumers pass
four necessary natural-object decodes. Twenty-three old physical records,
fourteen offsets, nine assembly sources and scoped machine operands retain
continuity without old IR or disassembly replay. Sort and MAS sources/commands
remain; changed incidental object bytes require only fresh final canonical
and typed memberships, not another implementation decode. All 13,831 exports
are recorded, with two additions, no removals and 1,512 real CRC changes,
including the unchanged-source conditional UFS signature's transitive CRC.
All 390 rebuilt modem and two optional imports match. Signed paired
installation, normal Android, automatic matching Live modem, full readbacks/
build-id, CE storage/core services, unchanged ramdisk and enforcing controls
pass. Complete logs contain no actual kernel/module fault; all 517 AVCs remain
visible, none for module loading. Tracing and Asleep are restored without
another boot or broad hardware replay. #60 is immediate rollback; #57-61 pairs
and accepted #61 metadata remain below 20 GB after contract-checked retirement
of #56 test artifacts. Next is complete 5.10.101-105. Selected AMA mempolicy,
the pinned latest-stable target, Android common, GKI and inherited hardware/
power acceptance remain open. No release export, publication or push occurs.

The thirteenth milestone resolves all 483 official entries through 5.10.105:
440 exact picks, eighteen code adaptations, five markers, sixteen PRESENT
fixes and four missing prerequisites. All 463 original-author commits through
`138273525` and five ordered ledgers pass source/provenance review. Five
separate PM/modern ARM64 integrations retain the public wakeup reader,
capability allocation and newer Android/pKVM vector model. A real LTO assembly
include-order failure is repaired separately by the complete upstream
`52c9f93a9c482251cb0d224faa602ba26d462be8` READ_ONCE guard change, ending
the build source at `5b0828436`. It preserves the C READ_ONCE/RCpc implementation
and all protections, without rewriting SMCCC instructions or restoring legacy
KVM templates. Original source/configuration proofs remain immutable; the
additive binder has 452 source identities. The only effective configuration
addition selects `CONFIG_MITIGATE_SPECTRE_BRANCH_HISTORY=y`.
#62 full compilation and five final reports bind 85 current objects/entire
commands. Nine new natural C objects are decoded once to measure CPU 1176,
IPv6 namespace 832 and by-value net 4800 byte records, actual PM/netfilter APIs
and BHB typed callbacks. Twenty-six accepted physical records, fourteen OEM
offsets, nine assembly sources, const sort and MAS lifecycle proofs retain
continuity without old IR or disassembly replay. Three non-KPTI vector banks,
per-CPU VBAR values and final capability-62 pointers pass; unselected KVM/KPTI
and hardware-mitigation effectiveness are not certified. All 13,831 exports
are recorded, with one addition, one removal and 2,646 real CRC changes.
All 390 rebuilt modem and two optional imports match. Signed paired
installation, normal Android, automatic matching Live modem, full readbacks/
build-id, CE storage/core services, unchanged ramdisk and enforcing controls
pass. Complete same-boot time-zero/subsequent logs contain no actual kernel/
module fault; all 517 AVCs remain visible, none for module loading. Tracing
and Asleep are restored without another boot or broad hardware replay.
#61 is immediate rollback; recent #58-62 pairs and accepted #62 metadata
remain below 20 GB after contract-checked retirement of #57 test artifacts.
Next is complete 5.10.106-110. Selected AMA mempolicy, the pinned latest-stable
target, Android common, GKI and inherited hardware/power acceptance remain
open. No release export, publication or push occurs.

The fourteenth milestone resolves all 762 official 5.10.106-110 entries:
706 exact picks, eleven code adaptations, five markers, 35 PRESENT decisions
and five missing prerequisites. The 722 original-author commits and two
separate interleaved ALSA/sixpack lifetime integrations end at `18c37e107`.
Complete source/provenance review distinguishes historical reverse checks,
semantic presence and superseding implementations without downgrading stronger
inherited code. All 6,433 effective/emitted configuration options, signing
material and required protections remain. The actual source binder contains
605 identities. #63 full compilation succeeds naturally; five final reports
bind 101 current objects/entire commands. Only thirteen new natural C objects
are decoded once to check eight changed pstore/PCI/audit/SELinux/coredump/ext4
records and XFRM/UDP/UART/xHCI interfaces. Old layouts, OEM/BHB assembly,
sort and MAS proofs retain scoped continuity, with fresh final CFI/data
identities and no implementation replay. All 13,835 exports are recorded,
with five additions, one removal and 3,904 real CRC changes; every rebuilt
modem/dummy import matches. Signed paired staging and one normal Android
boot with automatic matching Live modem pass, including full readbacks/
build-id, CE storage/core services and unchanged ramdisk/security. Complete
same-boot logs contain no actual kernel/module fault; all 517 AVCs remain
visible, none for module loading. Tracing and Asleep are restored without
another boot or broad hardware replay. #62 is immediate rollback; recent
#59-63 pairs and accepted #63 metadata remain below 20 GB after retiring
obsolete #58 test artifacts by their retained contract.

The selected io_uring fixed-file lifecycle has an independent inherited
finding: a premature registration return bypasses ref-node initialization,
and UNIX teardown lacks symmetric fput after removal of the SCM producer.
The missing old uid-helper entry does not fix it, and normal boot does not
certify it. A separate minimal direct-fput/node-initialization repair is under
review; no unsafe old-kernel reproduction, forced disable or speculative
fallback is used. Next is complete 5.10.111-115 plus that independently
reviewed lifetime repair. Selected AMA mempolicy, the pinned latest-stable
target, Android common, GKI and inherited hardware/power acceptance remain
open. No release export, publication, tag or push occurs.

The fifteenth milestone resolves all 564 official 5.10.111-115 entries:
532 exact picks, twelve code adaptations, five markers, twelve PRESENT
decisions and three missing prerequisites. All 549 original-author commits
end at `1727a9f07`. The distinct prior three-function fixed-file repair
`792b8bc8b` removes the premature return and balances direct fput ownership
without restoring absent SCM producers or importing a newer resource framework.
After the actual full-build error, separate `9667daa8b` migrates the OEM OOM
task producer to the standard delayed queue, not a timer-callback cast.

One actual configuration run retains effective selections, signing material
and fourteen generated OEM offsets. The emitted disabled-only `CONFIG_NVM`
entry disappears under upstream's LightNVM `BLOCK && BROKEN` dependency;
NVMe/NVMEM are not disabled. The necessary full-build retry naturally produces
#64 with Werror, ThinLTO, enforcing CFI, signatures, PRMEM and BHB intact.
Four final scoped reports bind 25 current object/entire-command identities.
Only sixteen new natural IR objects validate changed selected records,
public formals/consumers, task/thread offsets and the timer callback's real
CFI membership. Nine normal-object contracts and both modules' PLT sections
pass without replaying old machine implementations. The 13,835 exports have
8,962 real CRC changes; every rebuilt modem/dummy import matches.

Signed paired staging and normal Android with automatic matching Live modem
pass, including readbacks/build-id, CE storage/core services and unchanged
ramdisk/security. Two actual wrapper errors require recovery to #63 and
reinstallation of the same #64 pair: root is denied by the retained UID1013
io_uring gate, then ordinary ADB cannot list the probe-owned 0700 directory.
Only wrapper credentials/diagnostic access and failure-output retention are
corrected; the kernel and native probe are not rebuilt. Final UID1013/drop-cap
execution passes all three groups: fixed reads/replacement, partial/error
cleanup and asynchronous final file-reference release, with exact private
resources removed. This is a post-fix owned-file regression, not unsafe #63
reproduction or certification of all file types, races or memory failures.

Complete final boot and additive post-probe logs contain no actual kernel or
module fault. All 516 pre-probe AVCs and ten new probe-window AVCs remain
visible; the later eighteen-record attribution contains system logging/TEE/
init denials, not a probe-named or module-loading denial. Tracing and Asleep
remain restored. #63 is immediate paired rollback; recent #60-64 pairs and
accepted #64 metadata remain local below 20 GB. Next is complete 5.10.116-120;
selected AMA mempolicy, full io_uring/OOM safety, latest-stable target,
Android common, GKI and inherited hardware/power acceptance remain open.
No release export, publication, tag or push occurs.

The sixteenth milestone resolves all 394 official 5.10.116-120 entries:
347 exact picks, seventeen code adaptations, five markers and 25 qualified
no-new-commit dispositions. The 369 original-author commits end at
`aab63b923`. Full #65 compilation, fourteen records, i32 DRBG enum,
28 callbacks in ten CFI types, twelve APIs, matched modules and signed
staging pass. Thirteen natural IR objects are decoded once. Emitted options
change 6,432 -> 6,433 through two hidden library selections and a retired
disabled entry; security/signing and fourteen generated OEM offsets remain.

Actual #65 boot fails at 18.664 seconds in HKIP's GID root check during
CameraDaemon firmware loading, after the modem has linked. Persistent pstore
identifies the initiating fault; this is not RNG/BL31 initialization or a
diagnostic-wrapper failure. Both #64 partitions restore normal Android/root.
Official 117#39 installs kernel credentials for firmware reads; the inherited
protected bitmap still reflects the prior subjective credentials.

Separate `0436052d6` updates HKIP UID/GID root bits after each override/revert
publication and before the source vendor hook call. Recompute-on-revert clears
the temporary classification and supports saved nested credentials. HHEE/PID/
HVC checks, references, real_cred, upstream firmware behavior, !PROTECT_CRED,
permissions and SELinux remain. No process exemption, permanent root bit,
global lock, IRQ suppression or protection bypass is introduced. Full
asynchronous pointer/bitmap atomicity is not claimed.

Necessary #66 rebuilding and finite additive gates pass. Configuration,
signing and 26 prior natural object/entire-command contexts are identical to
#65. Only new kernel/cred.o is decoded once; prior thirteen IR objects and
layout measurements are not replayed. Current ten CFI groups and three
credential API symbols are rebound. Two checker-only fixes preserve actual
nonexported definitions and configured-out vendor hook semantics. All 390
modem plus two optional dummy imports match 13,839 exports, with six additions,
two removals and 2,894 CRC changes versus #64. CMS/ext4 and boot roundtrip pass.

The paired #66 installation reaches normal Android with automatic matching
Live modem, full readbacks/build-id and unchanged ramdisk/security. One real
CameraDaemon thread completes firmware loading, ISPCPU startup and power-on
return in the same boot. Complete logs contain no actual kernel/module fault;
517 AVCs remain visible, none requests module loading. Tracing and Asleep are
restored. #64 is paired recovery, #65 remains failed evidence. Local closeout
retains accepted #66 metadata and bounded recent pairs below 20 GB. Next is complete
5.10.121-125 from the new integrated source/documentation baseline, not an
invented accepted65 boundary. Arena capacity/reclamation/lifetime, selected
AMA mempolicy, complete io_uring/OOM safety, latest stable, ACK/GKI and full
inherited hardware/power remain open; no release export, publication or push.

### Linux 5.10.121-130 Follow-up

The complete 121-125 range and its separate USB placement/ext4 directory
call-context corrections reach source `606f5a0f8`, accepted normal Android
#67, automatic matching signed modem and completed ISP firmware startup.
LIB_MEMNEQ is the sole effective configuration addition in that batch.
Current eight natural contexts, 28 targets/eleven CFI groups, matching imports,
signatures, readbacks and complete logs pass. The accepted #67 runtime has
528 visible AVC denials, none requests module loading; full hardware and
source-only lifetime findings do not pass by implication.

The next complete 126-130 range resolves all 252 original entries: 226 exact,
ten adapted and sixteen already present. Its 236 original-author commits end
at `7c9fa502b` without a separate kernel integration. One necessary full build
produces #68 with no effective/emitted configuration delta. Sixteen new
ordinary contexts, selected NFT3 cached register boundaries and byte consumers,
fourteen current generated selectors, and 28 current targets in eleven CFI
groups pass. Corrections to the diagnostic checker preserve actual verdict/
value control flow, error edges and optimized private-byte addressing; no
kernel workaround, forced type emission, repeated decoder or weakened CFI is
used. Particular prior27/prior8 source applicability stays separate.

There are 13,837 exports, two additions, three removals and no changed shared
CRCs versus #67. All 390 rebuilt modem and two optional dummy imports match.
Stock-format boot roundtrip, stripped/CMS-signed module, certificate, ext4
owner/mode/SELinux label and paired readbacks pass. Normal Android #68 loads
the matching Live modem automatically, preserves unlocked user storage and
required services, and completes the same-thread ISP firmware path. Complete
logs contain no actual kernel/module fault; 519 AVCs remain visible, none
requests module loading. Independent ramdisk, Enforcing, taint4, signatureY,
PRMEM, tracing and Asleep remain. No accepted boot or broad hardware test is
repeated without a relevant change or fault.

#67 is immediate paired rollback; #64-68 local test pairs and compact metadata
remain below 20 GB. Next is complete 131-135; official original inputs are
prepared but not imported by that preparation. The pinned 5.10.274 target,
SELinux PRMEM arena capacity/reclamation/lifetime, two inherited source-only
fbcon findings, selected AMA/full io_uring/OOM and async HKIP atomicity,
network/hardware/power and ACK/vendor-core/GKI remain open. A successful stable
milestone is not release, independent KMI or production certification.

### Linux 5.10.131-135 Follow-up

The complete 432-entry official range resolves 374 exact picks, 27 adaptations
and 31 already-present implementations. Its 401 original-author commits end
at `6d6830bed`; no separate kernel integration is required. Exec/POSIX timer
teardown, network unregister/reference waiting, TCP single snapshots and TTY
insertion/publication retain the real OEM hardware/caller contracts. x86
RETBleed/objtool changes update retained mcount and shared build-rule callers
without changing ARM64 security configuration.

One necessary full kernel/module build produces #69. Effective/emitted
configuration and signing material are unchanged. Thirteen actual ordinary
C contexts, the new BPF callback's three-way type agreement/64-byte UAPI,
29 actual targets in twelve final CFI groups and fourteen generated selectors
pass. VLAN was mistakenly selected by the diagnostic context list despite
CONFIG_VLAN_8021Q=n; it remains source-only, without a forced build or new
configuration. No old NFT parser/layout proof is replayed. There are 13,836
exports, only retired `tty_schedule_flip` removed, and no shared CRC changes;
390 modem and two optional dummy imports match.

Matched stock-format boot and CMS-signed ext4 module staging pass. Normal #69
Android automatically loads the matching Live modem, with complete partition/
module/build-id readbacks, unlocked user storage, required services and six
same-thread ISP firmware startup stages. Independent ramdisk, Enforcing,
taint4, forced signatures and PRMEM remain. Complete boot/later logs contain
no actual kernel/module fault; 519 AVC denials remain visible, none requests
module loading. Original tracing and Asleep are restored. #68 is immediate
paired rollback; #65-69 test pairs and compact metadata remain below 20 GB.
Next is complete 136-140, with 812 original inputs prepared but not imported.
The pinned target274, source-only lifetime findings, ACK/vendor-core/GKI and
full hardware/power remain open. No release is exported or published.

### Linux 5.10.136-140 Follow-up

All 812 originals resolve to 750 exact picks, 20 adaptations, 40 already
present and two absent/unselected platform entries. The 770 original-author
commits end at `c218c464d`. Actual commit messages now retain the stable SHA
before the format-patch separator, without rewriting accepted history. Two
separate integrations follow: canonical upstream ext4 directory block/size
unit correction (`61a1d87a324a`, local `266bbac94`) and a redundant NFT bound
guard cleanup, ending source at `c303cfc45`.

Block trace request-only interfaces include both Huawei callbacks. Deadline
admission/hotplug migration includes OEM CPU pause and the disabled stub,
with effective cpuset bounds. RT/CFS and storage/USB updates preserve actual
hooks. New NFT descriptor validation, cmp16 state/ops and generated tracing
types are checked in six once-decoded current objects and 27 natural C
contexts; old register-parser/BPF source proofs are reused only through
explicit applicability. Disabled BLK_DEV_IO_TRACE callbacks are not forced
into the build. Final ELF verifies 41 targets in 18 CFI groups plus real NFT
and retained BPF canonical assignments; fourteen OEM selectors are unchanged.

One necessary full build produces #70 with empty effective/emitted config
delta and unchanged security/signing material. Complete exports are 13,843,
with seven additions and 267 CRC changes; 390 modem and two optional dummy
imports match. Stock boot/CMS/ext4 staging and paired installation pass.
Normal Android automatically loads the matching Live modem with complete
readbacks/build-id, unlocked storage/services and six-stage ISP startup.
Independent ramdisk, Enforcing, taint4, mandatory signatures and PRMEM remain.
Complete logs show no actual kernel/module/HKIP fault; 517 visible AVCs do
not request module loading. Original trace/Asleep state is restored.

#69 is immediate paired rollback; #66-70 test pairs and compact metadata stay
local below 20 GB, preserving originals and failed #65 evidence. Next is
complete 141-145. Target274, SELinux PRMEM arena/lifetime, two source-only
fbcon findings, broader AMA/io_uring/OOM/ext4, async HKIP/NFC ownership and
full hardware/network/power remain open. Stable maintenance is not ACK parity,
vendor/core separation, GKI or independent stable KMI; no release is published.

### Linux 5.10.141-145 Follow-up

All 260 canonical originals resolve in order to 214 exact picks, 16 adapted,
29 already present and one absent/unselected driver. The 230 author/date/full
message-provenance commits end at `917c376f5`; five durable ledgers record
every original. The explicit separate upstream cgroup transfer CPU-lock
correction (`ab1de7ead871`, local `c7cfda261`) follows.

#71 reaches Android and auto-loads modem but is rejected for ten real CPU-lock
assertions. The actual stack identifies Huawei kernel_clone/top-app migration,
an OEM caller omitted by the canonical-route-only source audit. Paired #70
recovery is verified. `78095eb5f` adds only the CPU header and outer read-lock
pair before existing threadgroup write locking. The cpuset assertion, VIP
semantics, security and error-path releases remain. #72's necessary corrective
build exits0; #71's unobserved original terminal exit remains distinguished
from its verified complete log/products rather than invented as a pass.

Canonical XOR selection removal is the only effective config delta. Current
anon_vma layouts, inlined pollfree/BPF guards, cgroup ordering and PSOCK tags
pass in nine cached objects; #72 checks exact unchanged context/object
identities and decodes only the changed OEM caller. Final ELF verifies 27
targets in 15 CFI groups, retired XOR absence and real BPF/NFT/cpuset
assignments. Complete exports are 13,843, with two additions/removals and 275
CRC changes versus #70; 390 modem and two dummy imports match, with fourteen
current selectors. No dummy installation, CFI relaxation or module spoofing.

#72 matched boot/CMS/ext4 staging, normal Android/automatic Live modem,
partition/module/build-id readback, storage/services and six ISP stages pass.
One short real framework window records 14 top-app events and eight repaired
OEM attach stacks without loss or WARN. Complete logs have no actual kernel/
module/HKIP fault; 542 visible AVCs do not request module loading. Independent
ramdisk, Enforcing/signatureY/taint4/PRMEM and original trace/Asleep remain.
Immediate recovery is #70; latest-five build pairs are #68-72, with #71 failed
and originals/#65 failure evidence explicit. Local metadata and precise
dependency stores stay below 20 GB. Next is complete146-150; target274,
source-only HID wrap and other lifetime/hardware/power/ACK/GKI/KMI work remain
open. No release export/publication occurs.

### Complete Canonical 5.10.146-150 And Accepted #73

All641 canonical originals resolve in sequence:577 exact picks,27
adaptations,35 already present and two specifically inapplicable operations.
The604 original-author commits end at f2bebf7c2, with full messages/provenance
and five durable per-release ledgers. No separate kernel integration is
needed. Standard DWC3 requested-state/PM/reset/live-IRQ ownership replaces
the unsafe OEM pre-PM restart while retaining necessary PHY/event handling.
Zonelist retry preserves OEM counters; top-cpuset affinity skips per-CPU
kthreads and retains the accepted CPU/threadgroup locks. F2FS is unselected
and does not certify distinct HMFS; absent io_uring SCM producer code is not
reintroduced. Unselected hugetlb's unrelated old_pte source gap remains open.

The single ordinary full build exits0, directly observed and reaped, and
produces #73. Only CONFIG_CC_HAS_AUTO_VAR_INIT_ZERO_ENABLER changes n -> y;
explicit INIT_STACK_NONE and security remain. Current103 natural contexts
freeze actual source/generated dependencies; ten new objects are decoded
once for timer/USB/cpuset and selected guards. Final ELF60 symbols/28 targets/
15 CFI groups,390 modem/two optional dummy imports and14 OEM selectors pass.
Exports13,844 have two additions,one removal and eight CRC changes versus72.

Signed matched installation reaches normal Android with automatic Live
modem, all readbacks/build-id, required services, unlocked storage and six
same-thread ISP startup stages. The relevant USB regression actually halts,
resets and restarts via automatic init rebind; two later already-connected
requests return0 with no DCTL writes. A printf diagnostic initially wrote
one character at a time and was corrected to a complete echo write; an
analysis window initially missed init's earlier automatic reconnect and
was corrected from cached trace, without another device cycle or kernel
edit. Actual random timer execution, zero trace loss and restored ADB
transfer pass. Complete logs have no actual kernel/module/HKIP fault;548
AVCs remain visible, none requests module load. PRMEM,ramdisk and enforcing
controls remain; original trace/Asleep are restored. This does not cover a
sustained disconnected runtime-PM state, OTG or full suspend.

Immediate paired recovery is72; latest-five build pairs become69-73, with71
still failed. Compact metadata is preserved before output reuse; consumed
full logs and expired68 test pair are retired, not referenced object/cache
dependencies. Next is complete151-155 toward pinned274. Inherited subsystem,
hardware/power findings and ACK/vendor-core/GKI/KMI remain open; no release
bundle, tag, push or publication occurs.

### Complete Canonical 5.10.151-155 And Accepted #74

All 387 canonical originals resolve in sequence: 361 exact picks, 14
adaptations, 11 already present and one specifically inapplicable SDIO
reference change. All 375 source commits preserve original author/date and
complete messages with stable provenance, ending at `e597c18c7`.
The separate `13b01f4cb` host compatibility fix addresses a real OpenSSL4
no-ENGINE link failure while retaining the existing PEM/CMS signing path
and mandatory target verification. No canonical import is replaced by it.

The fscrypt master-key hashtable, active/structural refs and early keyring
teardown include the relocated fscrypt_info pointer and duplicated SDP
cleanup. OEM key buffers, HWDPS APIs, user key policy and encryption format
remain. Current build selection is HW_F2FS, not unselected HMFS or upstream
F2FS; normal unlocked CE storage is observed without formatting, key removal
or destructive filesystem testing. DWC3's two exact IMI fixes leave the
accepted73 pullup/PM/reset body unchanged. SELinux conversion uses the new
gfp-aware provider, locked GFP_ATOMIC and unlocked GFP_KERNEL consumers.
The existing PRMEM ownership/reclamation limitation remains open.

One actual full kernel/module build exits0 and produces #74. Only the
canonical COMPAT A57/A72 AES erratum is newly selected; reserved capability64,
existing ARM assignments and NCAPS76 remain coherent. Actual178 C contexts
freeze complete source/generated dependencies. Eighteen necessary current
IR objects pass crypto/HW_F2FS/ext4/RCU, SELinux and reduced IPv4/IPv6 protocol
layout/dispatch checks without re-decoding ten73 objects. Final ELF checks
32 targets in18 CFI groups; 13,845 exports include one addition and8,884 CRC
changes. All390 modem and two optional dummy imports match, and14 generated
OEM selectors remain unchanged. Dummy is not installed.

Signed paired installation and readback reach normal Android with automatic
matching Live modem, current loaded build-id, CE storage/services and all six
same-thread ISP startup stages. Time-zero/boot/later logs have no actual
kernel/module/HKIP fault;516 AVCs remain, none requests module loading.
Independent ramdisk, Enforcing/taint4/forced signatures/PRMEM remain, and
original tracing/Asleep are restored. No redundant second boot, USB
disconnect cycle or full build follows these passed gates.

#73/5.10.150 is immediate paired recovery. Compact metadata and the accepted
local boundary are saved before build-output reuse; recent pairs become70-74
after retiring only expired69 files and consumed logs. Original images,
signing materials, failed65HKIP evidence and precise referenced source/proof/
alternate stores remain below20 GB. Next is complete canonical156-160 toward
pinned274. ACK/vendor-core/GKI/KMI and inherited subsystem/hardware/power
work remain open; no release bundle, tag, push or publication occurs.

### Complete Canonical 5.10.156-160 And Accepted #75

All520 canonical originals resolve in order:486 exact,11 adapted,19 present
and4 absent unrelated hardware providers. Five exact ordered-prefix trees
agree with projection;497 source commits end at `dac68d33f`. One157#147
git-am in-body From author/message exception is explicitly preserved with
exact source/date/signoffs;496 raw authors and all497 timestamps agree. The
separate `b20c287b7` fixes a concrete original io_uring uninitialized initial
OOM return and overwritten retry-backup leak, using mainline lifecycle
without its unrelated rsrc refactor. No accepted history is amended.

VFS fsdata initialization preserves HW_F2FS/OEM accounting. DMA-heap duplicate
check/publication uses one list lock with the existing device pin after
rejection. DWC3 receives canonical request/descriptor/ENDTRANSFER fixes while
OEM PM/IRQ hooks stay. SDHCI voltage-switch state preserves HS200/HS400 and
the reset/PHY/delay epilogue, including the extra OEM reset site. memcg
validates the actual cgroup1 control fd, and copy_file_range keeps VFS freeze
protection. THP paired source notifiers compile out with MMU_NOTIFIER=n;
active synchronous page-table IPI and CFI dispatch are covered, not active
notifier runtime. DMABUF_HEAPS is unselected; its separate pin lifetime is open.

One actual prepare/full Image+modules build exits0 for5.10.160 #75. Only TCP
perturb order16 is newly emitted, equivalent to the old shift. ThinLTO,
enforcing CFI, BTI/BHB, existing Werror/IAS, mandatory signatures/signing
material, Enforcing and PRMEM/HKIP remain. Current238 natural contexts,42
individually reviewed prior dependency changes and eleven once-decoded
interfaces pass. Actual SDHCI1472-byte host/ops, io_uring136-byte quiesce and
single-initial/single-retry backup lifecycle, five-argument trace poll,20/40-byte
IP wire groups and THP/SMP agree. Final ELF checks36 targets in22 CFI groups.

All13,845 exports remain with682 shared CRC changes versus74, no provider/
type/namespace change. All390 modem and2 optional dummy imports match;
14 OEM selectors and signing material stay. Boot roundtrip, CMS/embedded
certificate and original ext4 metadata/label/byte readback pass. One paired75
installation reaches normal Android with automatically loaded matching Live
modem, full partition/file/live build-id readbacks, unlocked CE/services and
same-thread ISP startup. Complete logs have no actual kernel/module/HKIP
fault;517 AVC denials remain, none for module loading. Independent ramdisk,
security and original trace/Asleep stay; no redundant boot/build/USB replay.

#74/5.10.155 is immediate paired recovery. Compact75 metadata and local
source/documentation boundary precede output reuse. Latest five test pairs
are71-75; expired70 and consumed full logs are retired, precise proof/alternate/
original/signing/failed-HKIP inputs retained below20 GB. Next is complete
canonical161-165 toward pinned274. ACK/vendor-core/GKI/KMI, inherited
subsystem/hardware/network/power and inactive notifier coverage remain open;
no release/export bundle, tag, push or publication occurs.

### Complete Canonical5.10.161 And Accepted #76

Complete161 imports all18 originals:16 exact, one existing Bluetooth fix and
one adapted SUBLEVEL marker. Seventeen source commits end at `46497f881`;
all raw author identities/timestamps and declared messages agree, and the
actual tree matches projection. No162 migration belongs to this milestone.
One actual prepare/full Image+modules build exits0 for5.10.161 #76 with
unchanged configuration, security/signing material, all13,845 exports/CRCs
and14 OEM selectors. All390 modem and two optional dummy imports match.
Current240 actual natural contexts and three necessary HID/loopback IR
objects pass; final ELF checks41 targets in27 CFI groups without replaying
accepted75 layout decoders.

One paired76 installation reaches normal Android with automatic matching
Live modem, full partition/module/build-id readbacks, unlocked CE/services
and same-thread ISP startup. Full logs have no actual kernel/module/HKIP
fault;502 AVC denials remain, none for module loading. Independent ramdisk,
Enforcing/taint4/signatureY/PRMEM stay; original trace/wifi and Asleep restore.
#75 is immediate paired recovery. Compact local metadata and latest72-76
test pairs remain below20 GB, while precise rejected71/failed65 evidence and
original/signing/proof/alternate stores remain. No export or publication.

The user explicitly approves Q3 A for162: keep the installed default
Hyperframe34-37 interface, add explicit standard numbering mode and adapt
actual new io_uring request/layout/lifetime ownership. The canonical import
is5.15.85 code with5.10 reductions, also present in Android common. Installed
vendor library opcodes overlap standard34-36 and the old bridge explicitly
frees after completion; blind porting or disabling the feature is not an
acceptable workaround. Continue complete162-165 with full ordered context
validation, current migration-specific build/CFI/layout and matching modules.
Previews alone do not establish import/build/runtime acceptance or target274.

### Complete Canonical 5.10.166-170 And Accepted #78

All371 official originals are resolved in order with353 source commits
through `1e81b711ac`; five release trees match the approved projection.
Raw authors/dates/trailers remain. One168#74 message has only a duplicate
blank-line cleanup exception, with complete raw original retained; history
is not amended and future imports use verbatim cleanup. No separate kernel
repair is added in this batch.

One actual prepare/full Image+modules build exits0 with unchanged config,
keys and36 OEM/HF selectors.366 actual contexts and eleven new ordinary
objects cover clock/NVMEM/RTC/sysfs/sysctl/PSI. New typed facts,97 normal
targets in55 final CFI groups, four initcalls/two attributes and relinked HF
bridges/worker-thread offsets pass. Old primary providers remain unchanged;
accepted77 private migration proof is retained without replay.

All13,854 exports, four additions and three NVMEM CRC changes are accounted
for;390 modem and two optional dummy imports match. Matching CMS-signed
staging, paired installation/readbacks and automatic Live modem pass.
Normal Android, unlocked CE, required services and six-stage ISP startup
complete. Full logs contain no actual kernel/module/HKIP fault;518 AVC
denials remain, none module load. Enforcing, mandatory signatures, taint4
and PRMEM stay. New warn/oops counts are read only and both0; limits remain
0/10000. Independent ramdisk, trace0/wifi and Asleep are preserved/restored.

The later upstream child-clock devres allocation-size fix remains source
only open: six callers are unselected and current modem has no such import.
No destructive clock/fault experiment or blanket lifetime claim follows.
Compact78 metadata and recent74-78 pairs remain below20 GB with77 immediate
rollback. Next is complete171-175 toward pinned274; ACK/vendor-core/GKI/KMI,
inherited subsystem and full hardware/network/performance/power findings
remain open. No release export, tag, push or publication occurs.

### Complete Canonical 5.10.171-175 And Accepted #79

All647 official originals are resolved with635 ordered source commits
through `aa4c719872`; all five release trees and raw author/date/message
identities match the approved projection. Verbatim cleanup preserves full
original messages without a new exception. Separate ACK poll-allocation
and mainline ext4-unlock corrections produce final source `a0dc72d14b`
before any build. The171 fixed-file change includes172's required NONBLOCK
followup; all four generic F2FS fixes also reach selected HW_F2FS.

One actual prepare/full Image+modules build exits0 with unchanged effective
selections, keys and36 selectors.433 ordinary contexts and14 necessary
decoded objects establish actual poll/HF/task/filesystem/HIDPP/trace/irq
contracts. Three additional PCI objects follow a real export-type change;
435 distinct contexts and17 decoded objects in total retain exact current
dependencies. Final112-target/64-group CFI assignments, HF bridges and
worker/thread offsets pass. Reader-only inlining/flexible-array corrections
do not change source or repeat compilation.

All13,854 exports remain with21 changed CRCs;390 modem and two optional
dummy imports match. Signed matching staging, paired installation/readbacks,
normal Android, automatic Live modem, unlocked CE/services and six-stage
ISP startup pass. Complete logs have no actual kernel/module/HKIP fault;
533 AVC denials remain, none module-load. Enforcing, forced signatures,
taint4 and PRMEM stay; read-only warn/oops counts are0, limits0/10000.
Three finite new UID1013/caps-zero private-ring groups pass async fixed-input
splice, ordinary poll and registered-buffer source/error recovery. No old
HF migration replay, system queue, forced event or fault/stress test follows.

Independent ramdisk remains; trace0/wifi and Asleep restore. Compact79
metadata binds final source including both corrections and the local
documentation-only boundary. Recent75-79 pairs remain below20 GB with78
immediate rollback; original/signing/precise proof and failed71/65 evidence
remain. Unselected child-clock source and wider lifetime/hardware/power/
ACK/GKI/KMI findings stay open. Next is complete176-180 toward pinned274.
No release export, tag, push or publication occurs.

### Complete Canonical 5.10.176-180 And Accepted #80

All841 originals are resolved with817 ordered source commits through
`b863b88f8e`:777 exact,40 adapted,21 present and three absent/unselected.
All five release trees and raw author/timezone/complete-message identities
match. Separate `baa22bf578` removes one imported PowerPC comment trailing
space, without altering original history. Standard VFS SGID, scheduler
long-sleeper/migration/uclamp, IPv6 destructors, NF transaction state and
DWC3/TTY/trace/allocator fixes retain actual hardware/security contracts.
All six generic F2FS originals also reach selected HW_F2FS.

One prepare and one full Image/modules build exit0 with unchanged effective
selections, signing keys and36 generated OEM/HF selectors. Canonical
AS_IS_LLVM/AS_VERSION select the actual LLVM12.0.7 IAS.511 natural contexts
and35 once-decoded objects bind current layouts, including five rq providers,
task/thread, MAS bio/request/UFS offsets and56-byte nfnetlink subsystem.
Android KABI reserve syntax remains but this tree's macro emits no padding;
no stableKMI claim.139 final targets in81 CFI groups, protocol/NF/PM tables,
UDP destructor stores and raw crypto/thread offsets pass. Direct-only TTY
helpers and unselected generic crypto/DCCP/SCTP/zoned paths stay scoped;
unchanged HF/PCI IR and accepted private tests are not replayed.

13,857 exports have eight additions, five removals and2732 changed CRCs;
390 rebuilt modem and two optional dummy imports match. Signed matching
staging, paired installation/readbacks, normal Android, automatic Live modem,
unlocked CE/services and automatic six-stage ISP startup pass. Independent
ramdisk stays. Complete logs have no actual kernel/module/HKIP fault;519
AVC denials remain, none module-load. Enforcing, mandatory signatures, taint4
and PRMEM stay; read-only warn/oops counts are0, limits0/10000. Trace0/wifi
and Asleep restore. Compact80 metadata and recent76-80 pairs remain local
below20 GB with79 immediate rollback. Original/signing/referenced proof and
failed71/65 evidence remain. Child-clock unselected source, wider lifetime/
hardware/network/power and ACK/GKI/KMI findings stay open. Next is complete
181-185 toward pinned274; no release export, tag, push or publication occurs.

### Complete Canonical 5.10.181-185 And Accepted #81

All515 originals resolve through `d90c2c11b7` with510 ordered source
commits:493 exact,17 adapted, three present and two absent/unselected
PCA9450 entries. Five endpoint trees and raw author/timestamp/timezone/
complete-message identities match. No separate integration is needed.
VLAN duplicate contexts use named tap owners; TCP close retains the accepted
no-netref timer cleanup after release_sock. Binder partial/full cleanup,
conditional io_poll_update locking and socket capability publication carry
the complete canonical operations. Both F2FS fixes also reach selected
HW_F2FS. DWC3 keeps OEM debugfs/FIFO behavior with15 balanced PM owners;
DECnet removal is complete but was unselected. OEM SB_DELAY_SHRINK bit24
and memory-hotplug extension remain; no hardware feature is blindly deleted.

One prepare and one full Image/modules build exit0 for5.10.185 #81.
Effective selections, keys, security and36 OEM/HF selectors remain; only
five disabled configuration entries change their emitted representation.
609 actual natural contexts include66 regulator-header consumers;46 current
objects establish18 layout groups and48 targets in12 new CFI types.
sock grows1072->1080 bytes from canonical sk_wait_pending; current final
UDP/IPv6 destructor stores use sk_destruct at byte968. Generic/Kirin/camera
regulator descriptors agree. Current Binder cleanup bounds, one IO mutex
guard and15 DWC3 success owners/18 release sites pass cached-IR ownership
checks.187 targets in91 final CFI groups and actual tables/thread operands
pass; direct-only TTY helpers remain separate. Unchanged HF/PCI/rq/MAS
proofs and accepted private-ring evidence remain scoped references, not
replayed or substituted for current changed layouts.

13,861 exports have six additions, two removals and2742 changed common
CRCs; providers/export types/namespaces remain.390 freshly rebuilt modem
and two optional dummy imports match. Signed paired staging, installation/
readbacks, normal Android, automatic Live modem, unlocked CE/services and
same-thread six-stage ISP startup pass. Independent ramdisk stays.
Complete logs contain no actual kernel/module/HKIP fault;518 AVC denials
remain, none module-load. Enforcing, signaturesY, taint4 and PRMEM stay;
read-only warn/oops counts are0, limits0/10000. Trace0/wifi and Asleep restore.
Compact81 metadata and recent77-81 pairs remain local with80 immediate
rollback, below20 GB. Original/signing/referenced evidence and failed71/65
records remain. Next is complete186-190 toward pinned274; full hardware/
network/power, ACK/vendor-core split and GKI/KMI work remain open.
No release export, tag, push or publication occurs.

### Complete Canonical 5.10.186-190 And Accepted #82

All838 originals resolve with811 ordered original-author commits:782 exact,
29 adapted,21 already present and six inapplicable entries. Each original
author/timestamp/timezone, complete message and stable trail remains. The
official trace_pipe fmt-release followup is separate, as is cleanup of seven
trailing-space lines introduced by exact originals. Final source is
`46243eea39`; no incomplete preview or version-only acceptance substitutes
for the full batch.

NFT transaction lists/mutex/state migrate to per-net storage without
regressing newer binding/PREPARE_ERROR/genmask behavior. Canonical scheduling
followups remain complete; absent obsolete loop validation is not resurrected.
IO send/receive control, IOPOLL flush locking, task-work context release,
pending-request iowait and final NOWAIT EAGAIN use actual local owners.
VFS owns moved-child rename locks; selected HW_F2FS gets the checkpoint-error
mount-write release while retaining shrink-length UAPI and cp_global_sem.
ext4 uses the actual int-returning freeze_bdev and one-argument thaw_bdev.
DWC3 preserves Kirin PM/PHY handling while propagating negative resume,
removing AutoRetry coherently and excluding device reset in host-only mode.
MM cache initialization precedes unchanged PRMEM. Missing Qualcomm/Meson/
BayTrail glue is not invented or substituted for Allen hardware.

One prepare and one complete Image/modules build exit0 for5.10.190 #82.
Effective and emitted config deltas are empty; pinned Android Clang/LLD12.0.7,
LLVM/IAS, ThinLTO, enforcing CFI, BTI/BHB, actual Werror, keys and36 selectors
remain.2448 current C contexts and61 once-decoded objects establish35 layout
groups and57 callback facts. Four old removed/composite .cmd leftovers are
excluded using current archive membership; OEM ../ paths and generated
regdb certificate C are represented without another compiler invocation.
io_sr_msg grows48->56 bytes but fits the existing64-byte union, so request
size240 and all actual HF offsets remain. NFT pernet/net, trace fmt/ring-page
generation, GPIO/HIV500, SDHCI/Zodiac and device consumers agree.211 indirect
targets in107 current final CFI groups,11 HF hook registrations/three work
stores/completion/15 direct bridges, current worker/thread offsets and11
HIV500 GPIO/IRQ/parent stores pass. Direct-only hf_io_req_complete remains
separate, not fabricated into a nonexistent CFI group.

13,865 exports have five additions, one removal and8898 changed common CRCs;
providers/types/namespaces remain.390 rebuilt modem and two optional dummy
imports match; same-build signatures and stock-format paired staging pass.
Only boot/modem_driver are installed. Normal Android, automatic Live modem,
partition/file/loaded build-id, unlocked CE/services and same-thread six-stage
ISP startup pass. Time-zero/boot/later logs contain no actual kernel/module/
HKIP fault;518 AVC denials remain visible, none module-load. Read-only WARN/
Oops counts are0, limits0/10000. Enforcing, signaturesY, taint4, PRMEM, original
trace0/wifi and Asleep remain; independent ramdisk is untouched.

Compact82 metadata and recent78-82 pairs stay local with81 immediate
rollback, below20 GB. Only expired77 test files and consumed logs/link/derived
IR caches are retired; original images, keys and referenced source/proof/
alternate inputs and failed71/65 evidence remain. Next is complete191-195
toward pinned274. Broad hardware/network/performance/power, ACK parity,
vendor-core separation and GKI/KMI remain open. No release export, tag,
push or publication occurs.

## Next-Stage Work Queue

These inherited bring-up follow-ups remain open independently of source
synchronization. Only explicitly observed #50 regressions extend the r49
results; older hardware/performance passes are not automatically transferred.

| Step | Work | Acceptance Condition | State |
| --- | --- | --- | --- |
| 01 | Wi-Fi external networking | Existing-network association, network-bound DNS/TLS and verified transfer; classify calibration access separately | r49 Association/DNS/Default TLS Response Pass; Verified Transfer Open |
| 02 | Cellular service | With an active SIM: registration, data and user-confirmed incoming/outgoing audio call; no guessed dial target | Needs SIM/Manual Check |
| 03 | Bluetooth peer audio | Pair/connect a supplied headset, playback/capture route and reconnect checks | Needs Peer/Manual Check |
| 04 | Frame RTG | Audit configuration/task/Binder/module ABI; restore real interface and validate intended caller behavior | r49 Scoped Lifetime Passes Retained; #50 Real Frame/Stop Regression Pass; Stress/AUX/Full Frame Open |
| 05 | L3/DDR multi-level voting | Match DT/firmware vote contract, restore full tables/ratios and validate real level changes | r41 Bounded Pass; Unused Normalized Bookkeeping Classified |
| 06 | Thermal daemon fallback | Resolve calculated-result/source/unit contract and validate without disabling thermal protection | Normal-Screen Warm/Cool Recovery Pass; Charging/High-Temperature Safety Open |
| 07 | Wi-Fi calibration access | Identify caller/lifecycle/permission contract; prove correct access or justified non-use without widening protection | Two Empty Records Confirmed; Usage/Write-Safety Contract Open |
| 08 | Vendor-init WP action | Matching userspace/protection source, correct process-context reporting and genuine protection behavior | Needs Matching Source |
| 09 | PCIe affinity/delivery | Verify physical/logical interrupt topology and classify functional impact before any affinity implementation | Endpoint INTx Delivery Pass; Root PME/AER/Migration Open |
| 10 | Charging/standby | Appropriate charger and battery state; charging transitions, temperatures, overnight power and wake stability | Needs Hardware/Time |
| 11 | Camera/audio integration | User-confirmed front/rear recording, audio-video sync, microphone routes and calls/headset integration | Needs Manual Check |
| 12 | Whole-device regression/security | Installed pair, ABI/signatures/readback, enforcing controls and complete required device coverage | #50 Pair/Boot/Security/Named Scoped Regression Pass; Whole-Device Acceptance Open |
| 13 | io_uring fixed-file lifetime | Reachable ref-node initialization and balanced table/deferred/error ownership, with a scoped post-fix registration/update/unregister check | #64 Separate Minimal Source Repair And UID1013 Owned-File Read/Replace/Error/Async Last-Reference Release Pass; All File Types/Races/Memory-Pressure/Allocation-Failure Safety Open; No Unsafe #63 Reproduction |

Existing bounded passes include storage/display/touch/USB, hardware video
codec exercises, basic sensors, audible settings playback and recorder user
reports, bounded RTC/deep suspend, PERF_CTRL queries and repaired RTNL timing.
They are not automatically recertified on each new kernel. Repeat or extend
only when a relevant change, fault or uncovered requirement justifies it.

## Local Iteration Closeout

r49 closes the persistent boost-floor defect established after r48's ordinary
renewal observation. Full build, final ELF, identical configuration/export CRCs
and signing material relative to r48, matching modem imports/signature,
paired installation/readback and unchanged independent ramdisk pass. The real
stock scene samples the released Settings slot while its boost is pending,
then verifies its stored floor is zero after expiry. Five production boost
ioctls return 0 and genuine overlapping renewal still works. All three trace
buffers are loss-free. Global disable has source/final-ELF validation, not a
production-disable runtime pass.

A bounded positive global queue test runs only after the read-only observer
shows registration 0 and no active queue. Its own ordinary-priority tasks
pass force depth 4, real pipe wake relation/getter, pause, non-force same-root
resume, force replacement at depth 1, old-root exit without damaging the new
queue, and new-root exit returning ENOENT. This is not production/32-bit or
PID/address-reuse certification. Diagnostic tasks, module and proc entry are
removed, and the necessary cleanup reboot restores taint 4 and resets the
historical registration token.

The authorized passwordless foreground check removes the previous manual
unlock prerequisite for Wi-Fi diagnosis. The saved network associates and
becomes a validated default Wi-Fi network. The corrected UID-2000 shell
context passes package/AppOps attribution, network-bound DNS and a default-TLS
HTTP response from a public mirror. That response is 403; two other endpoints
reset during TLS, so expected-file integrity transfer remains open. Do not
infer a kernel or calibration defect from these endpoint results. Wi-Fi OFF,
other settings, home foreground, Asleep/keyguard and enforcing controls are
restored; no network credentials, policies or calibration parameters change.

No new kernel edit, rebuild or flash is needed after these scoped checks.
The local closeout commits only lasting source/result records and removes
consumed task diagnostics. Original images, accepted r17, immediate rollback
r48 and the latest r45-r49 pairs remain below the workspace limit. No release
package, exported artifacts, release tag or publication is created. This
completes the current local iteration, not every next-stage item in the table.

The detailed r49 build/slot/timer/global evidence is in
clang12-rtg-source-audit.md; the network attribution and endpoint limits are in
clang12-functional-tests.md. The source-only global-disable/stale-timeout/
shorter-request branches, AUX/configured-thread changes, native PI/legacy/
opaque assembly/32-bit integration, frame pacing/power, missing SIM/peer/manual
checks, stock vendor-init WP source and calibration write-safety contract stay
explicitly open for the next stage. No dummy success or protection bypass is
used to close them.

## Prerequisite And Validation History

Read-only r37 connectivity baseline has no active default network, Wi-Fi OFF,
mobile data enabled and both SIM slots ABSENT. One saved Wi-Fi network exists.
USB is supplying power; battery reports full at the snapshot, which is not
enough to validate charging progression. The matching OEM init/plugin source
is absent, as documented in clang12-system-wp-source-audit.md.

A read-only r41 follow-up after its normal reboot still shows the keyguard,
Wi-Fi OFF, both SIM slots ABSENT and no active default network. No new radio
enable attempt or lock-screen bypass is performed during the MIPS validation.

A bounded Wi-Fi enable attempt does not establish a default network within
60 seconds. Service logs show screenOn=false; window policy shows the device
is keyguard-locked. Wi-Fi OFF and the original mobile/airplane/scan/location
settings are restored. A foreground unlocked settings check is needed before
classifying this result as a driver issue. No external transfer is attempted
without an active Wi-Fi network; no lock-screen bypass is used.

The follow-up snapshot still shows the same locked/asleep device and absent
SIMs. Continue source work while these manual prerequisites wait. The frame
RTG stock-feature closure changes only its five original flags in a temporary
configuration. Its affected objects compile, and three ignored nested user
copies are repaired. The timer now reuses its persistent worker on re-enable
and reports setup failures to the enable ioctl instead of false success.
Runtime scheduling remains disabled on r37; lifecycle synchronization,
authorization, complete link and module compatibility must pass before
exposing the restored interface. Enabled-build debug metadata shows unchanged
task_struct size but 23 shifted member offsets, so size alone cannot clear
this compatibility gate.

END_FREQ now follows the observed native/compat caller's self-thread scope.
An isolated original-feature full build is completed after removing obsolete
#2 cache while preserving configuration/symbol/signing metadata. The active
r37 output and device remain unchanged. The final #39 enabled link, matching
modem import CRCs and staged-module PKCS#7 verification pass. Its capability
getter uses the real RTG gate and handles standard four-byte compat queries
without removing the legacy eight-byte form. These are build/static-signing
results, not installed interface or application-use certification.
Read-only inspection of the actual
SELinux policy confirms ordinary application self:file access to render
commands 0xab23-0xab28, but not global boost 0xab64 in the examined lists.
Application render ownership, including force-init of an existing group,
requires repair before activation. The proposed self-process/system-control
boundary is awaiting user confirmation because it changes cross-process
behavior; do not silently block all application render calls or flash the
unresolved candidate.

Long-term records omit personal network/subscription identifiers, temporary
checksums and complete logs. Task-owned host/device probes and downloaded
test data are removed after results are recorded. Workspace remains below
20 GB; original backups and immediate rollback are preserved.

The L3 audit also finds silently truncated two-level DDR tables. All six
live tables match the original DT; the fixed two-column source is wrong.
Original Allen machine-code recovery establishes independent level ratios,
omitted-level zero semantics and a single maximum MHz vote. The source
repair passes independent native selector/parser comparisons and object
compilation. Final #41 link/pair checks pass, including startup/empty-sample
corrections using r37's frame-disabled configuration, with
r37 configuration/signing metadata retained separately. Paired flash/readback,
two normal boots and all six complete maps pass. Userspace's real compound
ratios are no longer truncated. All 26,078 real-sample vote selections match
the original machine code; the second level determines the maximum in both
c1/c2 L3 and DDR monitors, and 4,836 post-writel events show real vote changes.
These desired votes are not effective physical clock measurements. The filtered scheduler
counter API is retained rather than copying the incompatible original raw
producer. The subsequent original per-CPU normalized vote-scale audit finds
only its known writer/getter in the examined address-reference scope, with
no located call/export or field-pointer escape. It is classified as unused
bookkeeping, not a demonstrated active feedback gap; no unused state is
introduced. This practical audit is not a general alias proof or performance
parity certification. See clang12-mips-multilevel-audit.md.

The synchronized r41 cold-window follow-up still observes ten fallback
events per front/back calculation slot while physical inputs change and
published shell values remain stable. This does not certify warming recovery
or protection and does not justify changing stock coefficients or sensor
units. The subsequent publication audit confirms framework-owned integer
storage and named slot aggregation/smoothing in the stock daemon. Back's
published output is not slot 5 alone. Real warming/recovery remains open;
see clang12-thermal-source-audit.md.

The calibration read-only audit finds two all-zero records with no valid
CRC for the matching original/r41 layout. No contents or parameters are
transferred and no partition write occurs. The original kernel shares the
caller-credential read and automatic clear-on-sync-failure write gate.
Changing permissions or credentials cannot recover absent records and could
expose that destructive gate. Intended optional usage and authorized
read/write lifecycle still require resolution; see
clang12-calibration-access-audit.md. Device remains r41 without another
kernel edit, build or flash during these audits.

The subsequent live configuration audit finds MP16C/bisheng and
cali_data_mask=0x07 in the DT-selected configuration, with no mask override.
It requests NV reads rather than explicitly skipping them. Read refusal and
successful reads of the two zero records both leave the loader without a
valid selected record; no permission or mask change is justified to claim
calibration recovery. The source retains its no-new-NV-cache/DPD-not-ready
behavior, but real firmware calibration and RF/network correctness are not
certified by this source inspection.

The normal-screen thermal follow-up observes front recovery in a 157-second
window, then both front/back publication rising and falling during a separate
282-second screen-on/off extension. Slot-5 cached values update between
rejection episodes in the same daemon lifetime. This is not a permanently
frozen cache under the tested normal warming/cooling conditions. No thermal
protection or coefficient changes occur. The display is restored to Asleep,
radios/settings remain unchanged, and the kernel log covers the full
extension without warning/fatal markers; the first window's beginning has
rotated out. High-temperature throttling, charging and overnight-power
acceptance remain open, not inferred from this bounded recovery result.

The r41 topology follow-up identifies Wi-Fi's actual endpoint INTx IRQ82
separately from root-port PME/AER logical MSI IRQ267 and its physical GIC
dispatch IRQ83. A bounded Wi-Fi enable/disable interval observes 122 new
IRQ82 deliveries while 45 existing IRQ267 affinity refusals recur. Both
root MSI counters remain zero, so root PME/AER delivery/migration is not
certified. Wi-Fi and display are restored, the boot is unchanged and the
complete interval log has no warning/fatal markers. No affinity or kernel
change is made; see clang12-pcie-affinity-audit.md.

The user approves the application self-process/system-controller render
boundary. The shared five group backends enforce it on copied requests under
the existing render lock, including force-init takeover rejection. Failed
initialization no longer clears an existing group or the separate global
wake-queue buffer; task identity and RCU publication replace PID-only lifetime
assumptions. Enabled object and style checks pass. This does not activate RTG:
r41 remains installed, and current-source link/pair, enable/disable concurrency
and real native/compat/application behavior still require validation. See
clang12-rtg-source-audit.md for the independent global-queue scope limitation.

r42 completes current-source enabled link and matched boot/modem packaging,
paired flash/readback and two normal boots with unchanged independent ramdisk
and security controls. Real 64/32-bit untrusted_app_27 callers pass ownership,
force-init rejection, query-width canaries and exit cleanup. Actual UID1000
in the existing Magisk domain controls only test-child/self groups and passes
three disable/re-enable cycles with the same worker. The normal framework
configures RTG after reboot. A bounded post-enable MIPS telemetry capture has
2,622 samples, 862 with nonzero instructions, and 2,280 requested-vote events,
with zero overruns and unchanged monitor settings. This does not repeat the
full original-machine-code oracle or measure effective hardware clocks.

The independent PERF_CTRL global wake-queue lifecycle, actual timer expiry
under intended caller policy, full frame-thread scheduling/END_FREQ coverage
and performance/power parity remain open. An initial ARM32 process launch
during the temporary APK ABI change reports a secondary-zygote connection
refusal; an ordinary force-stop/relaunch of only that APK subsequently passes
on the unchanged kernel/policy. It is not claimed as a repaired zygote defect.
Existing SIM/peer/source/hardware/manual prerequisites remain unsatisfied;
the whole bring-up goal is not complete. Temporary app/tools/tracing and
consumed full logs are removed; r41 is the immediate paired rollback.

r43 addresses the independent global initializer's bool status variable.
Actual UID1000 native/compat probes reproduce positive 1 on r42 for failed
copy and missing PID; both return the existing negative EFAULT/ENODEV on r43.
The adjacent failed-copy getter remains EFAULT. No valid global initialization,
sampling, resume or destruction occurs. Full configuration/export/CRC and
certificate identity with r42, signed paired readback and normal startup/reboot
pass. Independent ramdisk and security controls remain unchanged; r42 is now
the immediate rollback. The global caller/permission and force/resume/depth/
exit cleanup contracts remain open. A scoped library immediate scan is not a
complete caller proof and does not justify speculative ownership changes.

r44 retires old global wake-queue memberships and logical counts before a
forced depth change, preventing hidden freed-task entries from being scanned
on later growth. Original Allen machine code confirms the inherited hazard
and the existing same-live-target pause/resume behavior. The small repair
retains that behavior and the existing lock, ABI, layout and caller policy.
Object/optimized IR, final ELF and full link checks pass. Configuration,
complete export/CRC table and certificate match r43; signed paired readback,
unchanged ramdisk and two complete normal boots pass with retained security.
r43 is now the immediate rollback. No old-kernel UAF is deliberately induced,
and no unknown system global queue is force-replaced for a positive probe.
Global positive runtime, root-retirement/PID-reuse identity, intended callers
and complete timer/frame scheduling acceptance remain open.

The pre-r44 read-only snapshot still has an asleep/locked device, both SIMs
absent and USB power. Only system_server has an observed open perf_ctrl
descriptor in that snapshot; this does not prove which commands it uses or
that global queues are inactive. No lock, permission, radio, thermal or
calibration bypass is attempted. Original backups and r17 plus the latest
r37/r41/r42/r43/r44 pairs remain; consumed task-owned diagnostics are removed.

r45 closes the source-level global root identity defect: non-force init/get
require the lookup-pinned task and the existing root retirement flag to
match, and wake/free hooks select the actual task pointer. PID removal can
precede final task free, so clearing the numeric token only in that hook
would be insufficient. Same-object pause/resume and force-init remain;
there is no new liveness, caller, UID, PID-namespace or ioctl-layout policy.
The destroy request's numeric guard is unchanged, not a general delayed
PID-handle guarantee across force replacement. Object/optimized IR, final
ELF and full link checks pass; configuration, complete exports/CRCs and
certificate match r44. Paired readback, unchanged ramdisk and two complete
normal boots pass with retained security. r44 is the immediate rollback.

No unknown global queue is force-replaced and no live PID/address-reuse
sequence is induced. Loaded system_server JNI/client copies are verified
against their exact device digests. Their examined init/get sites use group
commands 17/18, not global 44/45/46; this scoped result does not establish
all indirect callers or global inactivity. Positive global lifecycle,
request behavior across force replacement, timer expiry and complete frame
scheduling remain open. Original backups, r17 and the latest r41/r42/r43/
r44/r45 pairs remain; consumed task-owned artifacts are removed.

r46 addresses a concrete boost-restart race: an old queued timeout can clear
a newly armed boost, and lock-external activation can be undone by cleanup
before renewal. Activation now uses the existing timer mutex; the worker
consumes its notification before checking for a pending renewed timer, with
no late bit-set to overwrite a concurrent expiry. The existing argument,
duration/util and permission contracts remain. Object/optimized IR and final
ELF checks, full build, identical configuration/complete export CRCs/signing
materials, matching signed pair/readback and two normal boots pass. Ramdisk
and security controls remain unchanged; r45 is the immediate rollback.

The source timer entry is reachable only through proc boost 0xc014ab64 in the
examined tree, not the ordinary frame min-util commands. Scoped verified
loaded-library sites/immediate inspection does not locate an intended
permitted production boost caller or prove global inactivity. No global
boost or queue takeover is injected to fill this gap. Intended-caller expiry/
restart stress, positive global lifecycle and full frame scheduling remain
open, alongside the existing manual/hardware/source prerequisites. Original
backups, r17 and the latest r42/r43/r44/r45/r46 pairs remain; r41 metadata is
retained for frame-disabled comparison, while its superseded pair and
consumed task-owned artifacts are removed. Whole-device acceptance is open.

The unchanged-r46 follow-up verifies real stock native frame use without a
kernel rebuild, flash, test APK, permission change or queue takeover. An
independent trace instance records ten begin/end timestamp pairs and actual
SystemUI/RenderThread scheduling. A targeted instance-stack window confirms
the native frame ioctl path, and existing raw_syscalls events identify the
real controller's successful group allocation and the application's successful
BEGIN/END_FRAME and END_FREQ calls. The latter changes the group-enable
counter to zero before returning 0, followed by successful controller clear.
The observed allocation ID is not made into a platform constant.

Actual internal priorities include 87 outside the source's VIP band and 89
inside it. One cycle retains priority 89 after its frame-disable counter;
the later proc display can translate VIP/PI priority back to the raw policy,
so that checkpoint cannot close effective-priority restoration. The subsequent
r47 audit attributes and fixes a frame state/member-clear race rather than
blindly forcing normal priority. This does not reopen the passed frame ioctl
results or prove that the global boost timer ran.

All capture buffers are loss-free, full interval kernel logs contain no
warning/fatal markers, and boot/security state stays unchanged. The display,
keyguard, radio settings, brightness mode and existing global tracer/instances
are preserved; automatic brightness's naturally updated value is not
overwritten. Copied binaries/DEX, complete traces/logs and temporary scripts
are removed. Real permitted boost expiry/renewal, positive global lifecycle,
complete frame/power acceptance and external prerequisites remain open.

r47 reproduces the missing restoration on unchanged r46 with actual native
calls and separately filtered priority stacks. The controller assigns frame
VIP level 10; END_FREQ writes the disabled flag outside the member lock, and
a concurrent controller clear then skips resetting members because it sees
that flag. END_FREQ acquires the lock after their pointers are gone and
returns 0 while both threads remain at internal priority 89. This is an
observed frame cleanup race, not the separate short lock donations also seen.

The existing frame-info writer lock now covers the state check/change and
per-member restoration. The member-update counter reset also uses that lock.
No new global serialization, task layout/reference, ioctl ABI, permissions or
priority policy is added. The #47 full build, final ELF ordering, identical
configuration/complete export CRCs/signing material, matching modules and
signed paired image checks pass. Paired flash reaches normal Android;
boot/modem readbacks match, independent ramdisk is unchanged, SELinux remains
Enforcing, module signature enforcement/CFI/PRMEM remain and taint is 4.

Two bounded real wake/sleep cycles record 1,952 paired native ioctls, including
28 RTG requests returning 0 and two END_FREQ calls. Both calls perform the
UI/RenderThread NORMAL policy changes before returning; the first captures
the overlapping controller VIP update and member clear in the corrected
order. Independent mutex/rwsem votes still request and clear successfully.
Main/stack trace buffers are loss-free and full-window logs have no actual
kernel WARN, BUG, Oops, panic or CFI failure. No redundant second startup or
repeat of passed frame tests is used. Display/keyguard/settings/tracer state
are restored and task-owned diagnostics are removed after recording results.

At the r47 checkpoint, r46 is the immediate rollback and r43-r47 pairs remain;
the current r48 retention state is recorded below. Expired r42 images are
removed while their useful metadata remains. The independent mutex audit
confirms a separate cancellation defect on unchanged r47. A bounded root-owned
probe uses only a fresh, uncreated UHID file, private tasks and a private
anonymous page; it creates no HID device or input event and changes no system thread. With
an active VIP waiter, the owner changes 120 -> 89 -> 120 on normal release.
After the last waiter is cancelled and returns EINTR, the owner's inert write
returns EOPNOTSUPP after unlocking, but its priority remains 89 and it runs
at that priority. Its normal VIP setting remains zero. The control has a
successful type-2 request/clear pair; cancellation has a successful request
with no clear. Stacks identify the mutex/UHID route and the trace buffers are
loss-free. No actual kernel WARN/fatal record or AVC denial occurs in the
complete successful window; boot/security/display/tracer state is unchanged
and all private tasks/files are released.

The active generic VIP interface explicitly does not support nested requests
of one type, so a flag-only change or unconditional clear cannot preserve
donations from another held mutex. The user approves per-waiter contribution
identity/priority with owner-level aggregation and necessary ABI review.
OEM/vendor/KABI reserve macros allocate no storage; r48 consequently changes
waiter layout and the task thread offset, though mutex layout and overall task
size remain unchanged. Initialization/fork, ownership lifetime, lock order and
20 original generated assembly units receive scoped review, not blanket opaque
assembly certification. The full #48 build keeps the configuration/security
and signing material; 8,948 export CRCs change, including module_layout. New
matching modules and signed paired images pass; r47 modules must not be mixed
with r48. Paired installation/readback and unchanged independent ramdisk pass.

r48 withdraws cancellation before last-waiter flags permit fast unlock,
withdraws only the releasing mutex's donations before ownership transfer and
recomputes the remaining per-level maximum without clearing other lock votes.
Existing normal VIP/other types remain and uncontended fast paths are kept.
Nine private runtime scenarios and 36 actual scheduled-in checkpoints pass:
single cancellation, different-lock release/cancellation at levels 10/3,
different levels on one lock, retries, real handoff, normal VIP preservation,
rwsem coexistence and higher real RT/policy reset. The owner returns to 120
before unlock after its last donor cancels; a remaining low donor keeps 96.
The genuine signed out-of-tree probe is unloaded and removed, and its necessary
cleanup reboot restores taint 4100 -> 4 on #48 without security changes.

Two bounded native lock-screen windows record 11 successful frame timestamp
pairs. Controller cleanup restores UI/RenderThread from 89 to ordinary policy
inside each clear ioctl and before returning; actual later samples are UI
110/120 and RenderThread 116. The narrower window captures two production
GPU-to-RenderThread mutex request/withdrawal pairs through the new code at
110 -> 89 -> 110. All native buffers are loss-free, including 1,039 fully paired
selected ioctls in that window. No actual kernel fault occurs in scoped logs.
Two private-window daily_control/logserver capability AVCs are unrelated;
native windows have no AVC denial. Display/settings/tracers and enforcing
SELinux/signatures/CFI/PRMEM remain, with no system-priority or global-queue write.

The controller clears first in both windows; a late END_FREQ returns -100
because membership is already gone. At that checkpoint direct END_FREQ=0
is not re-observed on r48 and remains a frame acceptance boundary, not a
mutex test failure or a relabelled r47 pass. Native PI/legacy shared-vote
guarantees, opaque assembly and 32-bit production integration are not certified
wholesale. An earlier full UAPI export also exposes pre-existing OEM CONFIG
leaks; diagnostic header
selection does not relax export checks or fix that separate portability issue.

r47 is the immediate paired rollback, with r46, accepted r17 and original
backups retained. Latest r44/r45/r46/r47/r48 pairs remain; superseded r43 images/
module and consumed task-owned probes, logs/traces and build diagnostics are
removed after recording useful metadata. Workspace remains below 20 GB.

Permitted boost expiry/renewal, positive global lifecycle, complete frame/power
acceptance and manual/hardware/source prerequisites stay open at that checkpoint.

The unchanged-r48 boost follow-up identifies the actual permitted production
caller: vendor.huawei.hardware.iawareperf@1.0-service in u:r:uniperf:s0, with
its own /proc/<pid>/rtg descriptor and loaded libperfgenius_wrapper.so.
HisiPerfAdapter::SetRtgSchedBoost constructs 0xc014ab64; the installed MLS
policy grants uniperf self:file ioctl 0xab64. This complements, rather than
invalidates, the earlier scoped SystemUI/system_server library inspection.
No permission change or direct global request is needed.

The hardware-product uniperf configuration matches the runtime cached copy.
RTG window-switch actions specify 650 ms/util 684 and click actions 250 ms/
util 600; screen_on has no RTG action. A bounded ordinary blank-lockscreen
tap window records one complete real boost ioctl returning 0, default-group
floor 600 and a 63-jiffy timer at running HZ=250. Instance stacks attribute
activation to start_boost_timer, expiry to on_timer_timeout in the timer
softirq and floor/group cleanup to frame_thread_func. The observed callback
arrives about 273 ms after arming; this is not an exact-latency guarantee.
Both trace buffers are loss-free, full-window logs have no actual kernel
fault or AVC denial, and boot/security/settings/keyguard/display plus the
original main/wifi tracers are preserved. No rebuild or flash is performed.

Only one global request is observed. Loaded framework code limits click
scene reset to at least 2 seconds and traversal/jitter reporting to 250 ms
intervals, with the current click window/count set to 1000 ms/three reports.
This checkpoint does not attribute the click-policy consumer or prove that
another tap must renew the timer. Arbitrary repeated input is not used to
force overlap. Intended-caller single activation/expiry passes; renewal/
stale-timeout restart stress, multi-group/AUX/configured boost-thread coverage
and positive global queue lifecycle remain open.

The subsequent read-only audit locates the consumer in com.huawei.iaware's
loaded HwIAware OAT/VDEX. IAwareSdk event 3033 reaches IAwareSdkService,
AppInnerBoostARSPlugin and AppInnerBoostDmePlugin (2019), then UniPerf. Click
traversal emits 4400; click jitter emits 4401 for at least three skipped
frames. This is installed-code routing, not another runtime request capture.
The APK lacking classes.dex does not mean its optimized implementation is
absent.

Five existing CMS getConfig replies are decoded using the inspected actual
serializers. Click remains 1000 ms/3 reports, slide 5000 ms/16 reports, and
both feature switches are enabled. The application-type table adds only
slide-count limits (8/18:10, 36:3), not click deduplication; intelligent scene
types also select the slide branch. This does not attribute the previously
unobserved second click request to that table or native SDK flow state.

Same-channel reporting has a 250 ms minimum interval, equal to the nominal
click boost duration; separate traversal/jitter channels can theoretically
overlap. Neither ensures a safe blank-lockscreen action generates a second
request before the old expiry. No input, unlock, forced jitter, configuration
write, trace instance, rebuild or flash occurs in this audit. A bounded
renewal test needs an explicitly allowed ordinary foreground scene and
complete real request/timer/floor evidence across overlap and final cleanup.
Renewal and stale-notification/restart stress remain unverified, not failed
or passed. At this read-only checkpoint direct END_FREQ on r48, full frame/
power and manual/hardware/source boundaries remain open. Boot/security/
settings/display, matching r48 and rollback pairs are unchanged; task-owned
diagnostics are removed after durable recording and the workspace stays below
20 GB.

The subsequent unchanged-r48 follow-up addresses that direct stop boundary.
Installed RtgSched Handler code has default 2000 ms initial/click end delays
and reaches RtgSchedImpl.endFreq -> nEndEventFreq(event 2); no override or
global boost is written. Two bounded wake/awake-idle/sleep observations use
three mono-clock instances each. Both planned taps are skipped: the first
inspection wait expires, and the second screenshot guard fails comparison.
Neither records a click, and the failed guard is not evidence that wallpaper
changed. No unlock, explicit priority reset, policy/property write or injected
ioctl occurs.

The first window preserves the controller-first/-100 result. In the second,
one fully paired SystemUI END_FREQ (0xc010ab18) returns 0. UI and RenderThread
both restore from frame RT 87 to ordinary policy 0/priority 110 inside the
stop_frame_freq/set_frame_sched_state stacks before that return. Controller
attribute update and member clear start afterward. All 118 later actual
scheduled-in samples are ordinary: UI 110/120 and RenderThread 116. This
closes direct awake-idle END_FREQ restoration on r48 within the observed
scope; it neither attributes a runtime click nor certifies every interleaving.

All six buffers are loss-free, identified RTG calls have complete pairs and
unknown boundary exits are not labelled as known RTG outcomes. Retained logs
cover both complete capture intervals without actual kernel WARN/fatal
records or AVC denial. Boot/security/keyguard/display/settings and original
main/wifi tracers match their baselines, and diagnostic instances are removed.
No source change, rebuild, flash, reboot or additional device test follows
the pass. Task-owned diagnostics are removed after durable recording;
original/accepted/r44-r48 pairs and r47/r46 rollback remain below 20 GB.
Renewal/stale-notification restart, positive global queue lifecycle,
multi-group/AUX/configured boost threads, full frame pacing/power,
native PI/legacy/opaque assembly/32-bit integration and existing manual/
hardware/source prerequisites remain open; whole bring-up is not complete.

The next read-only follow-up narrows renewal eligibility rather than running
another blind lockscreen test. Window-switch RTG actions last 650 ms, click
actions 250 ms. start_boost_timer retains a pending later deadline before
floor update/mod_timer, although start_rtg_boost has already run. Thus a
second successful request or enable event is not necessarily timer renewal.
The acceptance evidence must distinguish a retained deadline from actual
rearming and show floor continuity plus final expiry/cleanup.

Installed service bytecode attributes window-switch routes to activity/
package/screen and on-demand/Handler gates. The numeric 4098 SystemUI sites
are MotionEvent touchscreen-source IDs, not window boost calls; no reliable
non-writing lockscreen-panel 650 ms route is established. The separate key
route reaches ContinuePowerDevMng message 3/UniPerf 13200 only after the
AppQuickStartFeature KeyBoost gate. Its inspected default is false and
configuration parser requires a KeyBoost item with Switch=1. Verified CMS
custom/base getters (13/6) return the same two ContinuePowerDev items without
KeyBoost. These are current configuration/installed-contract facts, not a
sample of the running Java static field. No switch is changed for testing.

Stock Settings/Display activities are resolved read-only as a candidate
foreground scene. User manual unlock and explicit permission for one
non-writing short page-switch observation are requested before any such
operation; capture/input preparation waits for that answer. The candidate
does not guarantee actual request overlap, and renewal/stale-notification
restart remain unverified. The device stays #48/Asleep/locked with security,
settings and existing tracers unchanged. Only consumed task diagnostics are
removed after recording, originals/rollback pairs remain below 20 GB, and
the existing global queue/full frame/ABI/manual/hardware/source limits stay
open.

The user then authorizes normal passwordless unlock and direct use of the
proposed stock foreground scene. On the same #48/boot, one normal upward
unlock gesture precedes Settings -> Display Settings, a short observation,
Back/Home and Sleep. Actual foreground/screenshot identify Display Settings;
no control, policy, property or performance request is written. No rebuild,
flash, reboot or repetition of accepted boot/mutex/END_FREQ tests occurs.

Three mono-clock instances select all current uniperf service threads, its
raw ioctl pairs and timer starts/cancels, existing boost/frame counters with
instance stacks, and callback expiry events. Five complete real 0xc014ab64
requests on the verified production descriptor return 0. Each records the
same on_timer_timeout timer, duration 163 jiffies at HZ=250 and util 684.
Exact now/expires fields prove renewal before pending deadlines: the
three-request opening episode extends them by 37 then 81 jiffies; the
two-request return episode adds 11 jiffies. No superseded deadline fires.
Final callbacks occur about 659/651 ms after the respective last arms, at
deadline+2/deadline jiffies. These observed timings are not performance or
exact-latency certification.

The first episode keeps floor 684 in the default and two allocated frame
groups until the final callback, then clears all three through
frame_thread_func. The second keeps and clears the default and still-active
launcher group. Its Settings group is controller-cleared/released before
expiry and has no worker floor-zero event; active-only lookup explains the
coverage boundary, not successful stored-floor cleanup. Release/reallocation
needs a separate audit. Ordinary overlapping renewal and retained-active
group floor cleanup pass; stale queued timeout/restart stress, the shorter
retained-deadline branch, released-group reuse, AUX and configured boost
threads do not pass by implication.

All three buffers are loss-free on all twelve CPUs and known boost pairs are
complete. Two outstanding Binder entries and two unknown-command boundary
exits are not boost outcomes. Snapshot-bounded kernel logs retain the full
unlock/scene/restoration window without actual WARN/fatal/CFI records.
Thirteen non-uniperf AVCs occur in that broader interval, including nine
within the scene/restoration interval: qemu-property reads and an AOD/motion
Binder denial. Background default-property and logserver capability denials
also remain recorded. No uniperf RTG denial or policy relaxation occurs;
this is not an AVC-free or whole-hardware pass.

Boot/security and original main/wifi settings, filters/triggers and stopped
states match. Task instances are removed, original home foreground and
Asleep/keyguard showing=true/occluded=false are restored. Automatic mode and
30000 ms timeout stay unchanged; dynamic brightness reads 26 -> 27 and is
not overwritten. No new source defect or additional device scene follows
this scoped pass. Consumed task diagnostics are removed after documentation;
originals/accepted/r44-r48 pairs and r47/r46 rollback remain below 20 GB.
Stale-notification restart, shorter-request retention, released-group reuse,
positive global queue, AUX/configured boost threads, complete frame/power and
existing ABI/manual/hardware/source prerequisites remain open. Whole-device
acceptance remains incomplete.

## Stable Import Through 5.10.200 / #86

The complete196-200 source range resolves601 stable entries with587 ordered
original-author commits:559 exact,28 adapted and14 already present. The
official configfs revert from the accepted195 source is not repeated. The
cpuacct stable change is adapted to this tree's core-aware runqueue-lock
assertion; its accounting and RCU semantics are retained.

One prepared configuration and full Image/modules build complete with no
effective config delta. ThinLTO, enforcing CFI, BTI/BHB, actual Werror,
signing keys, SELinux Enforcing, forced module signatures, PRMEM and36
generated selectors remain. Current selected interfaces use5,008 natural
contexts and68 first-pass LTO objects, with supplemental layouts for new
IPv6, xHCI/USB, ext4, nftables, trace, RPMsg and ARM64 paths. Final ELF checks
bind118 CFI groups/230 targets. The kernel reports13,870 exports versus13,866
at #85: four additions, no removals and3,295 changed CRCs. All390 required
modem imports and two optional dummy imports match the new table.

Same-build signed boot/modem images pass format, certificate and filesystem
checks. The ALN-AL00 boots normal Android #86, automatically loads the matching
Live modem, and reports matching partition/file/build-ID readbacks. The
independent Magisk ramdisk remains unchanged; critical services, unlocked CE,
automatic six-stage ISP startup, Enforcing, signature enforcement, taint4,
PRMEM, trace0/wifi and Asleep screen pass. Complete logs have no actual
kernel/module fault;508 AVC denials remain visible, none for module loading;
WARN/Oops counts are0.

The immediate paired rollback is accepted #85/5.10.195. Next import batch is
201-205 toward pinned5.10.274. Android common parity, independent stable
KMI/GKI certification, OEM/core separation, ACK parity, the known configfs
cursor race and complete hardware/network/performance/power acceptance remain
open. This work is local source/build/device verification only; no release is
exported or published.

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

### Linux 5.10.216-220: Accepted #93

The frozen official source range contains 1,334 entries, with 1,274 ordered
upstream commits, 56 already-present patches and four not-applicable
entries. Each release has a TSV disposition ledger. The 220 ledger records
751 unchanged imports, 18 targeted adaptations and two already-present
patches. Adaptations preserve OEM VFS rename data, explicit inode-lock and
NFS change-attribute behavior while following the upstream API; current
fdtable readers are moved to the RCU or lock-checked lookup helpers matching
their callers. Build findings also corrected these interfaces and the
Livepatch kallsyms conditional guard. The ordered source reaches official
5.10.220 at `c700caaf68`; two source integration commits follow.

Clang/LLD 12.0.7 `Image modules` build #93, same-build module import CRC
checks, signature verification, boot repack and modem image filesystem,
label and readback checks pass. `CONFIG_CPU_MITIGATIONS` is the only
effective config difference from the accepted prior build: 5.10.220 defines
it and defaults it on. Existing ThinLTO, enforcing CFI, BTI/BHB, module
signing, SELinux and PRMEM/HKIP remain. All 390 modem imports and both
optional dummy imports resolve with matching CRCs. This verifies matching
rebuild compatibility only; stable KMI/GKI is not certified.

The target was running accepted 5.10.219/#92 immediately before install.
Only paired `boot` and `modem_driver` were flashed for candidate #93; the
independent Magisk ramdisk was read back unchanged. Normal Android completes
and automatically loads the matching modem as Live. Staged partitions and
module file readback/build ID match. SELinux Enforcing, module-signature
enforcement, taint 4, PRMEM, critical services, unlocked CE, automatic ISP
startup, trace state and asleep screen pass. The complete relevant boot logs
contain no matched kernel/module fault; read-only WARN/Oops counts are zero.
511 AVC denials were visible, none for module load/request. Immediate paired
recovery is 5.10.219/#92. The accepted device record is
`.tmp/next216-input-preparation/r93-runtime-acceptance.json`.

The workspace was returned below 20 GB by retiring only consumed final-link
intermediates and the completed ThinLTO cache. Continue with 5.10.221-230
toward 5.10.274. GKI/KMI certification, Android common parity, OEM/core
separation and broad hardware/network/performance/power acceptance remain
open; no release export or publication is performed.

### Linux 5.10.221-230: Accepted #94

The frozen official ten-release input contains 1,901 entries. The ten
per-release ledgers record 1,815 exact imports, 47 targeted adaptations,
35 already-present fixes and four not-applicable changes. Official source
ends at `711d99f845cdb587b7d7cf5e56c289c3d96d27c5` (5.10.230); two local
integration commits follow. The HW_F2FS integration carries xattr-directory
checkpoint tracking while retaining strict-fsync behavior. A missing
checkpoint reason found by the first full build was fixed before acceptance.

Full Clang/LLD 12.0.7 Image/modules build #94, config/security, signature,
and same-build module gates pass. ThinLTO, CFI, SELinux, forced module
signatures and PRMEM/HKIP remain. Compared with accepted #91, selected
configuration changes are ARM64 erratum 3194386, CPU mitigations and
`CONFIG_PROC_MEM_ALWAYS_FORCE`; the signing certificate is unchanged. All
390 modem and two optional dummy imports match. Export CRCs changed for 8,923
symbols, with 19 additions and three removals; installed modules therefore
must match this build. This is not stable KMI/GKI certification.

Paired `boot` and `modem_driver` installation starts normal Android
5.10.230/#94 on the ALN-AL00. The matching signed modem loads automatically
as Live; partition readbacks and loaded module Build ID match. The independent
Magisk ramdisk remains unchanged. SELinux Enforcing, signature enforcement,
taint 4, PRMEM, critical services, unlocked CE and automatic six-stage ISP
startup pass. Scoped log checks find no kernel/module fault; WARN/Oops
counters are zero and 518 observed AVCs include no module-load/request
denial. Trace state and asleep screen are restored. Immediate paired
recovery is 5.10.220/#93. The workspace is below 20 GB after final-link
intermediate and expired image cleanup; no release export/publication occurs.

Next is the complete 5.10.231-240 range toward pinned 5.10.274. Android
common parity, OEM/core separation, stable KMI/GKI certification and broad
hardware/network/performance/power acceptance remain open.

### Linux 5.10.231-240: Accepted #95

The frozen official input contains 2,575 ordered entries. Ledgers in
`docs/linux-5.10.231-imports.tsv` through `docs/linux-5.10.240-imports.tsv`
record 2,496 exact imports, 67 targeted adaptations, eight already-present
fixes and four not-applicable changes. The selected source reaches 5.10.240;
two post-range local commits only correct compile context (TCP helper order
and a duplicate IPv6 cached context) and do not change the stable patch
scope.

Build #95 passed with Android Clang/LLD 12.0.7, ThinLTO, CFI, forced module
signatures, SELinux and PRMEM/HKIP. The same-build gate reports 390 modem
imports and two dummy imports with no missing or mismatched CRCs, 5.10.240
vermagic, unchanged signing certificate, and an export delta of 11 added,
one removed and 8,996 changed CRCs. This remains a matching-module check,
not a stable KMI/GKI certification. The BPF-JIT path was compiled once with
temporary target selection for source validation; the product configuration
still leaves `CONFIG_BPF_JIT` disabled.

Only paired `boot` and `modem_driver` were installed on the previously
accepted 5.10.230/#94 target. The device reached normal Android 5.10.240/#95,
loaded the matching signed modem as Live, preserved the independent Magisk
ramdisk, and passed partition/module readbacks, security, PRMEM, services,
unlocked CE, ISP startup, trace/screen restoration and complete scoped log
checks. WARN/Oops counters are zero; visible AVCs are recorded but none are
module-load denials. Immediate paired recovery is 5.10.230/#94.

Evidence is retained under `.tmp/next231-input-preparation/`, including the
source/build/device boundary, build/module gates, staged image manifest and
runtime acceptance record. Final-link and ThinLTO intermediates were
retired after the successful build; the workspace is below 20 GB. No release
export, tag or publication was made. Next boundary: import and review
5.10.241-250, then repeat the ten-version build/device checkpoint.

### Linux 5.10.241-250: Accepted #97

The frozen official range contains 2,006 ordered entries. The ten ledgers in
`docs/linux-5.10.241-imports.tsv` through `docs/linux-5.10.250-imports.tsv`
record 1,939 exact picks, 56 targeted adaptations, six fixes already present
and five not-applicable changes. The upstream endpoint is 5.10.250
(`b29eaaa7a0`); separate local follow-ups fix one KSM reference pairing, the
OEM WALT `in_range()` name conflict and eight typed min/max expressions. The
adapted 5.10.248 virtio documentation commit's non-upstream author metadata
is explicitly recorded as an exception rather than rewritten.

An initial `olddefconfig prepare` attempt failed while compiling arm64
`asm-offsets` because `MIGRATE_CMA` was undeclared. The output configuration
shows `CONFIG_CMA=y`; a later configuration-synced prepare and the final full
build pass without disabling CMA or altering security settings. The exact
cause of that one-time failure is unconfirmed. The first full compile exposed
the WALT macro collision and the 5.10.246 minmax signedness assertions; the
two relevant narrow integrations pass targeted object builds. Full
Android Clang/LLD 12.0.7 `Image modules` build #97 passes with ThinLTO, CFI,
forced module signatures, SELinux, CMA and the retained signing certificate.
Same-build versioned CRCs match for all 390 modem imports and both optional
dummy imports. The current export table differs from retained 5.10.230/#94
metadata by 19 additions, six removals and 8,994 CRCs; that is a cumulative
comparison, not a KMI/GKI certification. The KSM fix is source-reviewed only
because `CONFIG_KSM` is not selected in the device configuration.

Only the paired #97 `boot` and `modem_driver` images were flashed over the
accepted 5.10.240/#95 target. Normal Android 5.10.250/#97 starts and loads the
matching signed modem automatically as Live; device partition/module
readbacks and Build ID match, and the separate Magisk ramdisk is unchanged.
SELinux Enforcing, module signature enforcement, taint 4, PRMEM, essential
services, unlocked CE, automatic ISP startup, trace state and asleep screen
pass. Scoped complete logs contain no matched kernel/module fault; WARN/Oops
counts are zero and 510 AVC denials include no module-load denial. Immediate
paired recovery is #95. The accepted boundary and runtime record are under
`.tmp/next241-input-preparation/`; consumed link/cache intermediates and the
selected #92/#94 image pairs were removed to return below 20 GB. No release
export or publication was made.

Next is the full 5.10.251-260 batch toward pinned 5.10.274. Android common
parity, OEM/core separation, stable KMI/GKI certification and broad
hardware/network/performance/power acceptance remain open.

### Linux 5.10.251-260: Accepted #99

The ten per-release ledgers record all 1,879 frozen official first-parent
entries: 1,822 exact, 46 adapted, four present and seven not applicable to
the available local implementations. The source reaches the official
5.10.260 endpoint in order. Separate local commits synchronize the active
HW_F2FS copy and complete API consumers omitted from upstream's ordinary
driver inventory.

The selected filesystem now covers all five F2FS fixes in this range:
width-aware scalar sysfs access, checkpoint waiter ordering, warm-node
cleanup before pending-page decrement, delayed inline-node FIEMAP and ACL
entry bounds. MMC retry state persists through requeue but resets on a
new logical request, avoiding state inherited from a reused blk-mq PDU.
The TLBI refactor retains this tree's extra batched flush completion
synchronization. Configuration preparation adds only upstream
ARM64_ERRATUM_4118414 and its selected repeat-TLBI option; captured CPU
MIDRs do not match the implementation's erratum list. No mitigation was
disabled to make the build pass.

The first whole build caught the Android accessory driver's five-argument
HID call. Its separate adaptation passes the actual composite EP0 buffer
capacity to the new six-argument API. Final review caught and fixed the
HW_F2FS FIEMAP omission before any flash, then completed the unselected
Bluetooth SRCU C-side lifecycle. Kbuild's successful check after the latter
change left products and build number unchanged because Bluetooth is
disabled. Record both the generated product source and this final
incremental verification; the accepted product is #99, not an anticipated
#100. Disabled generic F2FS, pedit and trace-KDB paths have no product build
target and receive no runtime claim.

The full Android Clang/LLD 12.0.7 build and selected risk objects pass with
ThinLTO, enforcing CFI, BTI, forced signatures, SELinux and the retained
certificate. Final ELF checks verify 94 repeat-TLBI alternatives and the
local batch zero operands, enforcing CFI failure handling, the HID argument
contract and ring-buffer's new read-start interface. Same-build checks
resolve all 390 modem and two optional dummy imports. Relative to #97,
five exports are added, four removed and 9,021 common CRCs differ; formal
KMI stability is not established by those checks.

Paired boot/modem installation boots normal Android 5.10.260/#99 and
automatically loads the signed modem. Partition/module readbacks and Build
ID match, and the independent ramdisk stays unchanged. Enforcing/signature
enforcement, PRMEM, taint 4, CE, critical services, automatic ISP startup,
trace state and asleep screen pass. Complete relevant logs contain no
matched kernel/module fault, WARN/Oops counts are zero and 517 AVC denials
include no module-load denial. Immediate paired rollback is #97. Retiring
consumed caches/link intermediates and the expired #89 pair returns below
20 GB while retaining five image pairs and referenced evidence.

Continue with complete 5.10.261-270, then the remaining stable releases
toward pinned 5.10.274. Android common parity, OEM/core separation, formal
GKI/KMI and broad subsystem, hardware, performance and power acceptance
remain open; all work remains local Git with no release publication.

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
