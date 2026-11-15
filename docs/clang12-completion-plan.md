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
Installed candidate is #69/5.10.135 on `upstream/linux-5.10.y`, kernel source
through `6d6830bed`. Immediate paired rollback is #68/5.10.130. #65 remains a
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
| U02 | Next 5.10.y batches, starting at 5.10.45 | Bounded ordered imports toward a pinned target; matched build, normal Android boot and automatic module loading at milestones, retaining paired rollback | All Ordered Entries Through 5.10.135 Resolved; #69 Full Build/Current Interfaces And CFI/Matching Signed Pair/Normal Android/Auto Module/ISP Firmware Path Pass; #68 Immediate Rollback; #65 Failed Evidence Retained; Next Complete 5.10.136-140; Target 5.10.274 Not Imported |
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
