# Kernel Module Updates

## Policy For Allen

Update the kernel and all required loadable modules as a matched build while
the Huawei tree has no independently maintained stable KMI. For the installed
normal Android system, the required loadable module is `modem_driver.ko` on
the independent read-only ext4 `modem_driver` partition. The installed 5.10.200
#86 kernel and module are built together with the r49-derived configuration
and newly selected standard BHB mitigation, Android Clang/LLD 12.0.7,
enforcing CFI and the existing signing
certificate. No loader, version CRC, signature or SELinux check is bypassed.

This is the immediate solution for modules to follow stable updates. It does
not make the module ABI stable across arbitrary versions. Every later batch
must rebuild required modules and recheck their contracts, not just change
the release string or resign an old incompatible binary.

## Upstream References

References checked on 2026-11-13:

| Source | Applicable Guidance |
| --- | --- |
| [Linux 5.10 external modules](https://www.kernel.org/doc/html/v5.10/kbuild/modules.html) | Build through Kbuild against the configured/built target kernel; `modules_prepare` does not produce the required `Module.symvers` for MODVERSIONS |
| [Linux 5.10 module signing](https://www.kernel.org/doc/html/v5.10/admin-guide/module-signing.html) | Kernel must trust the signing certificate; strip before signing; appended signature presence alone is not verification |
| [Android loadable modules](https://source.android.com/docs/core/architecture/kernel/loadable-kernel-modules) | MODVERSIONS detects versioned interface changes, but does not repair ABI breakage; incompatible modules must be rebuilt |
| [Android stable KMI](https://source.android.com/docs/core/architecture/kernel/stable-kmi) | Independent module reuse requires constrained configuration/toolchain and monitored KMI symbols within the same Android/LTS KMI branch |
| [Android ABI monitoring](https://source.android.com/docs/core/architecture/kernel/abi-monitor) | Monitor symbol lists and associated types; different Android KMI branches do not share an ABI guarantee |
| [GKI versioning](https://source.android.com/docs/core/architecture/kernel/gki-versioning) | The stable sublevel is not part of the KMI identity; a matching KMI version can span multiple kernel releases |

The original Android common 5.10.44 merge loader is checked at
<https://android.googlesource.com/kernel/common/+/82658bfd8898b71f700cba34de6923d344214fef/kernel/module.c>.
Its `same_magic()` behavior matches Allen's `kernel/module.c`.
Do not apply newer kernel-only build syntax or a GKI signing-policy change
to this vendor 5.10 tree without a separate integration review.

## Release, CRC And Signature Are Separate Gates

The earlier source-sync record was too categorical about release mismatch.
With `CONFIG_MODVERSIONS=y`, `same_magic()` ignores the first space-delimited
vermagic token when the module contains a `__versions` section. It still
compares the remaining vermagic attributes. `check_modstruct_version()` and
`check_version()` then check `module_layout` and versioned symbol CRCs.
Without version CRCs, the full vermagic must match; force-loading is not used.

Therefore, a 5.10.43 module is not automatically rejected by 5.10.44 solely
because those release tokens differ. Conversely, a matching release is not
proof of compatible ABI. MODVERSIONS is a consistency check for exported
interfaces, not proof that opaque OEM objects, private offsets, CFI function
types, compiler behavior and runtime assumptions still agree.

`scripts/check-module-abi.py` intentionally enforces a stricter same-build
release policy for this project's packaging, in addition to import CRCs.
Its explanation now distinguishes policy from the kernel loader's actual
rule. It does not predict all permitted loader cases or certify a stable KMI.
No old modem binary is intentionally loaded into #50 to turn this inspection
into an unneeded live ABI experiment.

The matching module is stripped before signing with the existing SHA-512
policy. The signature is verified against the certificate embedded in the
target kernel, and image readback must match the signed bytes. Signatures
prove authenticity/integrity, not ABI compatibility; resigning is not a
substitute for rebuilding. Private signing material remains untracked and
must not be published with artifacts.

Current GKI vendor modules use verified partitions and a different signing/
allowlist policy. Allen retains `CONFIG_MODULE_SIG_FORCE=y` and its existing
trusted keys. Disabling that protection does not create GKI compatibility.

The next 5.10.49/#51 milestone preserves all 13,821 exports but changes the
get_net_ns CRC after the upstream namespace implementation move. This is not
an unchanged complete export table. The rebuilt modem does not import that
symbol; all of its 390 imports match, and the matching signed module loads
automatically on #51. A successful CRC check still is not complete KMI proof.

The 5.10.50/#52 milestone has 13,822 exports, with two additions,
one removal and 8,878 changed CRCs versus #51. All 390 freshly rebuilt modem
imports and two optional dummy-module imports match. The signed new modem
loads automatically on #52 and its runtime build-id matches the staged file.
The required modem is updated with the kernel; CRC churn is not hidden by
resigning an old module or patching its version table.

The 5.10.55/#53 milestone has 13,823 exports, one addition, no
removals and 58 changed CRCs versus #52. All 390 freshly rebuilt modem imports
and the two optional dummy-module imports match. Final affected layouts and
the changed bool wait-function CFI are checked separately. The new signed
modem loads automatically on normal Android, with matching runtime build-id,
file and partition readbacks. The next MSI/device-layout gate illustrates why
rebuilding modules alone cannot validate OEM assembly's private offsets.

The installed 5.10.64/#54 milestone has 13,826 exports, four additions,
one removal and 8,949 changed CRCs versus #53. All 390 freshly rebuilt modem
imports and two optional dummy imports match. Twelve generated offsets and
29 actual assembly operands are checked against fresh C layouts, not old
OEM debug metadata. The changed __do_once_done interface and final CFI type
agree with both real modem call sites passing __this_module. Flush callback
assignment/recognition and ordered reference publication are independently
verified in the final ELF. The signed matching modem loads automatically
on normal Android with matching runtime build-id, file/partition readbacks
and retained security. These checks are not an independent stable KMI.

The installed 5.10.70/#55 milestone has 13,827 exports, one addition, no
removals and fifteen changed CRCs versus #54. All 390 freshly rebuilt modem
imports and two optional dummy imports match. The coordinated const
list_sort interface has its changed export CRC and five matching actual
callback/CFI canonical entries, rather than a cast or permissive CFI path.
Fresh public-key/flow layouts and MAS all-tag static/dynamic/ref/STARTED
control flow pass; unchanged mapped assembly sources and current twelve
offsets reuse #54's scoped operand proof. Matching CMS-signed modem staging,
automatic loading, live build-id and complete readbacks pass on normal
Android with existing security. These are same-build integration gates,
not a promise of arbitrary old-module reuse or whole OEM ABI certification.

The installed 5.10.75/#56 milestone retains all 13,827 exports, with exactly
three CRC changes versus #55: fib_add_nexthop and the two MDIO registration
exports. All 390 rebuilt modem and two optional dummy imports match. Fresh
selected C objects and final enforcing CFI validate the standard five-argument
FIB interface, MDIO's appended shutdown member, its registration assignment
and real indirect callback. Unchanged implementation proofs for the const
sorter, all-tag lifecycle and generated assembly offsets are reused only
through scoped source/configuration/type/final-address continuity gates.
The new CMS-signed modem loads automatically on normal Android with matching
runtime build-id and complete file/partition readbacks; all 6,432 configuration
entries, ramdisk and security remain. This remains a same-build update method,
not independent stable KMI or GKI certification.

The installed 5.10.80/#57 milestone retains all 13,827 exports, with no
addition or removal and 2,051 CRC changes versus #56. All 390 rebuilt modem
and two optional dummy imports match. Actual BPF owner, console, Qdisc,
stream-buffer and bare per-CPU TCP orphan types agree with their selected C
consumers; the new callback assignments, canonical targets and enforcing
dispatches pass. Unchanged const-sort, all-tag and generated-offset
implementation proofs are reused through explicitly bound current evidence.
The CMS-signed matching modem loads automatically on normal Android with
matching runtime build-id and full file/partition readbacks. Existing
configuration, signing material, independent ramdisk and enforcing controls
remain. Substantial CRC changes are retained, not concealed by version-table
patching or old-binary resigning. The next UFS change requires fresh actual
layout/assembly verification in addition to rebuilding modules.

The installed 5.10.85/#58 milestone has 13,829 exports: four additions, two
removals and 2,575 changed CRCs versus #57. All 390 rebuilt modem and two
optional dummy imports match. A separate integration replaces two proven
stale UFS state/nested-mutex assembly literals with `offsetof` generators;
fourteen actual generated constants, thirteen affected machine operands and
selected C consumers agree with the new upstream pointer layout. Eighteen
unrelated operands and unchanged const-sort/all-tag implementation proofs
retain explicit scoped continuity, not a blanket opaque-ABI promise. Five
final reports bind 29 distinct actual objects and thirteen selected physical
record names, including the new IPv6/inode interfaces, existing five-argument
SDHCI dispatch, atomic BPF and selected OEM F2FS trace-name consumers.
The CMS-signed matching modem loads automatically on normal Android with
matching runtime build-id and complete readbacks. Effective configuration,
signing material, independent ramdisk and enforcing protections remain;
only the already-disabled FORTIFY line disappears from emitted configuration.
Required modules continue to follow the kernel through rebuilding and paired
installation, not resigning old modules or spoofing version tables.

The installed 5.10.90/#59 milestone retains all 13,829 export CRCs exactly,
without added or removed exports. All 390 rebuilt modem and two optional
dummy imports match. The new MMC shutdown and BPF sysctl callbacks are not
exports; their actual class/table initializers, final canonical pointers,
precise types and enforcing dispatch are checked independently of CRCs.
Five reports bind 46 actual objects, with scoped continuity for thirteen
previous records and two newly measured natural callback-member records.
Selected TUN/PACKET inline and FUSE direct consumers are rebuilt; unchanged
layout/assembly/const-sort/all-tag implementation proofs are not replayed.
The CMS-signed matching modem loads automatically on normal Android with
matching runtime build-id and complete file/partition readbacks. Effective
configuration, signing material, independent ramdisk and enforcing controls
remain. CRC equality is not a blanket old-binary reuse guarantee; same-build
pairing remains the verified method. The next range changes selected XFRM,
clocksource, packet-scheduler and cgroup trace records plus a block-PM public
signature, so it requires fresh bounded checks in addition to rebuilding.

The installed 5.10.95/#60 milestone retains all 13,829 exports with no addition
or removal, but 838 real CRCs change. These include the standard block-PM
single-argument runtime-resume helper and selected clocksource, packet-rate
and XFRM interfaces. No CRC is hidden or rewritten. All 390 rebuilt modem and
two optional dummy imports match the current table. Five final reports bind
72 actual objects, eight changed physical records and an explicit additional
XFRM callback-member context. Precise public types, selected SCSI caller,
Perf acquire/RCU dispatch, UFS early driver-data store and fresh enforcing
canonical targets pass separately from export CRC checks. Unchanged physical
records/assembly retain bounded source/configuration/command continuity;
changed sort and MAS objects receive necessary current-object checks.
The separately picked official native MTIMER length-policy fix is bound to
the built source, not hidden in the original 663 entries. The stripped/signed
matching modem passes CMS/certificate and ext4 metadata checks, then loads
automatically on normal Android with matching build-id and complete module/
partition readbacks. Configuration, signing material, independent ramdisk,
Enforcing/CFI/forced signatures and PRMEM remain. This successful matched
rebuild does not establish arbitrary old-binary reuse or a stable GKI KMI.

The installed 5.10.100/#61 milestone has 13,831 exports: two additions, no
removals and 1,512 real CRC changes versus #60. Both new JBD2 journal-head
exports and actual Perf CRCs are retained. Even with the conditional
ufshcd_init signature source unchanged, its transitive CRC changes from
0xba066086 to 0x06aac99b; source continuity is not CRC equality. Every one of
the 390 rebuilt modem imports and two optional dummy imports matches. Five
final reports bind 75 current objects; only four necessary natural objects
measure three selected changed Perf records and real core/ARM PMU/ARM64/OEM
consumers. Old physical-layout, assembly, typed sort and MAS lifetime proofs
are inherited under exact source/configuration/command continuity; final
canonical memberships and callback identities are fresh. The separate
cpuset assertion repair is included in the built-source identity, outside
the original 208 entries. Stripped/signed modem CMS, target certificate and
ext4 labels/ownership pass before installation. Normal Android automatically
loads the matching Live modem, with identical loaded build-id and complete
module/partition readbacks. Configuration, signing material, independent
ramdisk, Enforcing/CFI/forced signatures and PRMEM remain. This matched
rebuild is not independent old-binary reuse or a stable GKI KMI guarantee.

The installed 5.10.105/#62 milestone retains 13,831 exports, with one addition,
one removal and 2,646 real CRC changes versus #61. `xfrm_state_mtu` replaces
`__xfrm_state_mtu`; the genuine void-to-bool `nf_queue_entry_get_refs` interface
changes its CRC from `0xc2188a94` to `0xcd33a110`. Its actual emitted callers
consume the returned value. The UFS `ufshcd_init` and standard `list_sort`
CRCs remain equal this time; neither equality is an all-ABI guarantee.
All 390 freshly rebuilt modem imports and two optional dummy imports match.
Five final reports bind 85 current objects/entire commands, retaining scoped
old proofs and measuring only three changed CPU/IPv6 records across nine
new natural C objects. Public PM reader/clear, netfilter bool and BHB callback
types are checked against the actual build; KVM/KPTI unselected paths are not
fabricated. The sole effective configuration addition is the standard BHB
mitigation, with the existing toolchain and signing material retained.
The stripped modem CMS/certificate, ext4 labels/ownership and local staging
pass. Staged image identities are bound to the successful final build before
the first reboot and to the installed partition/module readbacks afterward.
Normal Android automatically loads the matching Live modem, with an equal
runtime build-id. The independent ramdisk, Enforcing/CFI/forced signatures
and PRMEM remain. This is same-build updating, not independent old-binary
reuse, hardware-mitigation certification or a stable GKI KMI guarantee.

The installed 5.10.110/#63 milestone has 13,835 exports, five additions,
one removal and 3,904 real CRC changes versus #62. New exports are the
pelt_thermal tracepoint/iterator/static-call key, uart_xchar_out and
udp_encap_disable; virtio_finalize_features is now private and not exported.
The two XFRM migration interfaces genuinely gain if_id arguments.
xfrm_state_migrate retains its source signature but its dependency-derived
CRC changes too, so signature equality alone is not a module ABI guarantee.
ufshcd_init and list_sort CRCs remain equal for this batch.

All 390 freshly rebuilt modem imports and two optional dummy imports match.
Five final reports bind the successful/reaped complete build, actual source
and unchanged configuration, final ELF/export table and 101 current
objects/entire compiler commands. Thirteen new natural C objects check eight
changed pstore/PCI/audit/SELinux/coredump/ext4 records and actual XFRM/UDP/
UART/xHCI interfaces. Inherited layout, OEM/BHB assembly, sort and MAS proofs
retain scoped continuity without old implementation replay; final canonical
CFI memberships and data identities are fresh. The independent selected
io_uring fixed-file lifecycle finding remains open, not certified by these
module/interface gates.

Stripped modem CMS/certificate, ext4 label/ownership, boot format and local
roundtrip checks pass before installation. One paired installation reaches
normal Android with automatic matching Live modem, identical runtime
build-id and full partition/module readbacks. All 6,433 configuration
options, signing material, independent ramdisk, Enforcing/CFI/forced
signatures and PRMEM remain. This is the repeatable same-build update method,
not independent old-module reuse or a stable GKI KMI claim.

The installed 5.10.115/#64 milestone retains 13,835 exports, without additions
or removals, but has 8,962 real CRC changes versus #63. `hex_to_bin` genuinely
changes from char to unsigned char: natural AArch64 IR remains i32(i8), uses
zero-extension and the unsigned-char CFI type, while its CRC changes from
`0x11f7ed4c` to `0xab6d5b3b`. `cookie_tcp_reqsk_alloc` gains the address-family
ops argument and actual store/callers; its CRC becomes `0xf710eecf` from
`0xfcd6c6e4`. `ufshcd_init` changes from `0x06aac99b` to `0x6e37dedd`, and
the unchanged socket-provider signature does not prevent its dependency CRC
from changing. `list_sort` remains `0xe091c977`; none of these equalities
certifies the full binary ABI. All 390 newly rebuilt modem imports and two
optional dummy imports match.

Four scoped reports bind the actual integrated source, successful/reaped
natural #64 build, configuration, final ELF/exports, 25 current objects/entire
commands and both normal module hashes. Sixteen new natural IR objects check
selected record/API/task changes and the new delayed OOM timer target's actual
CFI membership; nine normal-object contracts retain scoped prior proofs
without broad old IR/assembly replay. `task_struct` grows for the standard
timer and regenerated thread offsets match its actual layout. Fourteen
generated OEM device/storage offsets and effective configuration/signing
remain; LightNVM's disabled-only emitted option disappears as recorded.

Original 111#166 removes NOLOAD from the three PLT/trampoline output sections
while retaining FIPS initcall context. Each actual normal modem/dummy module
has three one-byte zero-filled PROGBITS sections with flags 0x6. Normal
Kbuild `.ko` outputs are unsigned link/postlink/CFI artifacts; do not mistake
them for installed signed copies or require an invented sign-file command.
The modem staging phase separately strips, re-signs and verifies CMS and
the retained target certificate, ext4 labels/ownership and byte readback.
The optional dummy is still not installed to fake coverage.

The paired #64 images reach normal Android with automatic matching Live
modem and equal runtime build-id/readbacks, preserving ramdisk, Enforcing,
forced signatures, CFI and PRMEM. Two owned-probe wrapper errors cause paired
#63 recovery and reinstallation of the same #64 images, with no kernel
rebuild. After correcting only the probe identity and private-directory
diagnostic, a drop-only UID1013 ordinary-file regression passes fixed-file
reads/replacement/error cleanup and final-reference release. This does not
weaken the existing OEM UID restriction or establish independent old-module
reuse, stable KMI, GKI or full io_uring/OOM safety.

The 5.10.120/#65 full build has 13,839 exports, six additions, two removals
and 2,894 real CRC changes versus #64. All 390 modem and two optional dummy
imports, selected interfaces, CMS/ext4 staging and boot roundtrip pass.
Actual boot nevertheless fails in HKIP's GID check during CameraDaemon
firmware loading, after the modem has linked. Matching modules do not certify
whole-kernel boot. Both #64 partitions restore normal Android/root.

Official 117#39 (`5a73581116362340087fad4df7e4530c4a819821`, upstream
`581dd69830341d299b0c097fc366097ab497d679`) correctly reads firmware under
temporary kernel credentials. Separate `0436052d6` synchronizes protected
UID/GID root bits after subjective override and matching revert. Revert
recomputes restored state rather than leaving a permanent privileged bit.
Credential references, upstream firmware behavior, HVC enforcement, the
PROTECT_CRED guard, SELinux, CRC and signature checks remain intact.

Necessary #66 rebuilding retains exact #65 configuration, signing and whole
export table. All 26 prior natural object/complete-command identities remain;
the new credential object is decoded once, and the same ten CFI types are
bound to the current final ELF. The three intentionally nonexported normal
API definitions and configured-out vendor hooks are represented accurately
by independently reviewed checker corrections, not by kernel changes.
Old #65 link addresses or runtime acceptance are not relabelled #66.

The same-build #66 signed boot/modem pair reaches normal Android and loads
the matching Live modem automatically. Complete partition/module readbacks,
runtime build-id, unchanged independent ramdisk, Enforcing/taint4/signatureY
and enabled PRMEM pass. The actual CameraDaemon ISP firmware read/start/return
path completes in one boot and thread; logs contain no actual kernel/module
fault. There are 517 visible AVC denials and no module-loading request denial.
Tracing and Asleep are restored. Failed #65 evidence remains separate; #64
is immediate paired recovery. This does not establish stable KMI/GKI, all
credential-observer atomicity or complete hardware/power acceptance.

### 5.10.125 #67: Matched Rebuild And Installed Acceptance

The complete 121-125 batch imports 734 official entries through 705
original-author source commits. A USB application-location correction and an
ext4 two-call compatibility integration are separate, giving 707 source
commits at `606f5a0f8`. CONFIG_LIB_MEMNEQ is the sole effective configuration
addition; the target toolchain, CFI/ThinLTO, mandatory signatures, signing
materials, PRMEM and fourteen generated OEM device/storage values remain.

The actual #67 full build produces 13,838 exports. Against accepted #66,
`flow_indr_dev_exists` is added, `mdio_bus_init` and `xfrm4_protocol_init` are
removed, and 403 shared export CRCs change. All 390 modem and two optional
dummy imports match the current release and symvers without missing exports
or CRC mismatches. Eight new normal-build contexts establish the changed
selected records/types/provider contracts; the final ELF independently passes
28 selected targets across eleven CFI groups. Old object/layout proofs are
not rerun or mistaken for current ELF addresses.

Strip-before-sign CMS verification, target certificate presence, original
boot roundtrip and module ext4 owner/mode/SELinux label readback pass. The
matched boot/modem pair reaches normal Android and automatically loads the
same-build Live modem. Partition/module/build-id readbacks, required services,
unlocked encrypted storage, unchanged independent ramdisk, Enforcing/taint4/
signatureY and enabled PRMEM pass. One real CameraDaemon thread completes the
ISP firmware read/start/return path in the accepted boot. Complete logs have
no actual kernel/module fault; 528 AVC denials remain visible, none requests
module loading. The original trace state and Asleep screen are restored.

#66 is immediate paired rollback. The optional dummy is not installed or
force-loaded. Stable synchronization and same-build module acceptance do not
establish independent stable KMI, Android common parity, GKI or complete
hardware/performance/power acceptance. This remains a local source/build/
recovery workflow, not release export or publication.

The installed 5.10.130/#68 milestone has 13,837 exports: two additions
(`nft_parse_register_load` and `nft_parse_register_store`), three removals
(`drm_fb_helper_modinit` and the old exported NFT load/store validators), and
no changed shared CRCs versus #67. All 390 rebuilt modem and two optional
dummy imports match. The existing datatype normalization is retained while
the new register APIs validate full-width values before publishing u8 fields.
Sixteen actual natural contexts, selected cached NFT producer/consumer
evidence, 28 current normal targets in eleven CFI groups and fourteen current
generated selectors pass separate scoped gates. Optimized byte-address
consumers are checked against their actual private-data/debug/type anchors,
not guessed struct emission or old offsets. These are not all-expression
layout or independent stable-KMI proofs.

The matching stripped/CMS-signed modem is staged with the original ext4
ownership, mode and SELinux label, installed with the #68 boot and loaded
automatically on normal Android. Live build-id and full partition/module
readbacks match, independent ramdisk and enforcing controls remain, and the
same-thread ISP firmware startup completes. No old module is resigned,
version-patched or force-loaded. Immediate paired rollback is #67; the pinned
latest-stable target, inherited source findings and ACK/GKI remain open.

The installed 5.10.135/#69 milestone has 13,836 exports: only the retired
`tty_schedule_flip` disappears, with no changed shared CRC, provider, export
type or namespace versus #68. All 390 rebuilt modem imports and two optional
dummy imports match. The retired symbol is not a modem dependency. The TTY
port lock and UART_V500 queue remain, while pty insertion and buffer commit
share one transaction without recursively calling the locking public push.

Thirteen current ordinary C contexts bind actual source/commands/dependencies;
VLAN is disabled by the unchanged configuration and is source-only, not a
forced-build success. The new BPF SK_LOOKUP test producer, table assignment
and syscall type check agree on one precise CFI type and the retained 64-byte
UAPI context. The actual final table pointer and selected 29 targets in twelve
CFI groups pass separately from export CRCs. Fourteen current generated OEM
selectors retain specific direct-source applicability; equality of an old
object or opaque layout is not presumed. No old NFT decoder/layout proof is
replayed.

The stripped matching modem is CMS-verified against the unchanged trusted
certificate embedded in the current kernel, staged with stock ext4 ownership,
mode and SELinux label, and installed with #69 boot. Normal Android loads it
automatically. Live build-id and complete module/partition readbacks match;
ISP firmware startup, unlocked user storage, required services and enforcing
controls remain. The dummy is not installed. This is same-build integration,
not independent stable KMI, arbitrary old-module reuse or GKI certification.

The installed 5.10.140/#70 milestone has 13,843 exports: seven additions, no
removals and 267 changed CRCs versus #69, with unchanged providers, export
types and namespaces. All 390 rebuilt modem and two optional dummy imports
match. New request-only block tracing migrates Huawei IOTrace callbacks as
well as canonical producers. Six current optimized objects establish actual
trace/NFT callback types and descriptor/cmp16 layouts; final CFI checks 41
targets in 18 groups and the real new NFT ops assignment. The disabled
standard BLK_DEV_IO_TRACE callbacks are source-only, not forced-built.
Current fourteen OEM selectors have unchanged direct providers and values.
These gates are particular source/interface checks, not opaque-layout or
independent stable-KMI certification.

The same-build stripped modem is CMS-verified against the unchanged kernel
certificate and staged with stock ext4 ownership/mode/SELinux label. Paired
installation boots normal Android and automatically loads it. Full partition,
module and live build-id readbacks match; unlocked storage/services and ISP
firmware startup complete. Enforcing, mandatory signatures, taint4, PRMEM and
independent ramdisk remain. Complete boot/later logs have no actual module,
kernel or HKIP fault. The dummy is not installed, no older module is resigned
or version-spoofed, and #69 is immediate paired recovery.

### 5.10.145 #72: Matched Rebuild After A Real Caller Failure

Complete exports remain 13,843, with `debugfs_lookup_and_remove` and
`mmput_async` added, the two XOR exports removed, and 275 changed shared CRCs
versus #70. Providers/export types/namespaces remain. Canonical ChaCha no
longer selects the unused XOR implementation; four XOR CFI groups are retired
accurately, not forced back into the configuration. Enforcing CFI still
checks 27 retained/current targets in 15 groups and actual BPF/NFT/cpuset
canonical assignments. All 390 rebuilt modem and two optional dummy imports
match; fourteen generated OEM selectors retain direct-source applicability.

#71's source/build/interface gates pass, but actual normal boot discovers
the Huawei post-fork top-app route calling cpuset attachment without the
new required outer CPU read lock. That pair is rejected and #70 restored;
successful module loading alone does not conceal the actual WARN. Minimal
caller-side integration produces #72. Its ordinary build exits0 and its
single changed OEM object verifies the correct lock order. The prior nine
145 interface objects and contexts are identical, so no repeated IR decode
or old parser/layout proof is needed. #71's original terminal exit was not
directly observed after restoration; its log/products were verified with
that limitation retained separately.

The matching #72 modem is stripped before CMS signing against the unchanged
kernel certificate and staged with stock ext4 metadata. Paired installation
boots normal Android and automatically loads the Live modem. Partition/file/
loaded build-id readbacks, unlocked storage/services and same-thread ISP
startup pass. Eight real OEM post-fork attach stacks execute in a bounded
framework window without CPU-lock WARN or lost trace events. Complete logs,
Enforcing/signatureY/taint4/PRMEM and independent ramdisk agree. #70 remains
immediate paired recovery. Dummy is not installed, old binaries are not
resigned or version-spoofed, and independent stable KMI/GKI remains open.

### 5.10.150 #73: Standard DWC3 And Timer Interfaces

The complete146-150 source range resolves641 canonical originals in order
and builds the required modem in the same ordinary full invocation. Its
directly observed and reaped exit0 produces #73. Only the compiler capability
probe CONFIG_CC_HAS_AUTO_VAR_INIT_ZERO_ENABLER changes; explicit stack-init
choice, CFI/ThinLTO/BTI/BHB/Werror, mandatory signatures and signing material
remain. Current exports total13,844: __do_once_slow_done/start added,
register_gifconf removed and eight shared CRC changes versus #72. All390
modem and two optional dummy imports match; providers/types/namespaces and
fourteen generated OEM selector values retain their actual contracts.

Ten genuinely new interface objects are decoded once from the current full
build and generated dependencies. Random's callback now has the standard
timer_list signature, not the retired work_struct CFI type. The three DWC3
producer/consumer layouts and typed UDC pullup dispatch agree. The final ELF
checks60 selected symbols and28 normal targets in15 CFI groups, retaining
canonical BPF/NFT/cpuset/DWC3 assignments. No old layout/parser family is
replayed or treated as certification of opaque vendor interfaces.

The modem is stripped before CMS signing, verified against the unchanged
embedded certificate and staged with original ext4 ownership/mode/SELinux
label. Normal paired #73 boot automatically loads the matching Live module;
partition/file/live build-id readbacks, services, unlocked storage and the
same-thread ISP firmware startup pass. Independent ramdisk, PRMEM,
Enforcing/taint4/signatureY remain. Complete logs have no actual kernel/module
fault; existing AVC denials contain no module-load request.

The changed DWC3 path has a single real disconnect/automatic init rebind,
with halt/reset/run ordering and two later already-connected requests
without another DCTL write. Actual random timer execution and restored ADB
transfer pass without CFI failure or trace loss. This is not sustained
disconnected runtime-PM, all host/OTG or full suspend validation. #72 is
immediate paired recovery. Dummy remains uninstalled, old binaries are not
resigned or version-spoofed, and independent stable KMI/GKI remains open.

### 5.10.155 #74: Filesystem-Owned Keys And Current Modules

The complete387 canonical151-155 inputs produce375 original-author commits,
followed by a separate real host OpenSSL4 no-ENGINE compatibility fix.
Only unavailable ENGINE includes/helpers/PKCS#11 branches are excluded;
unsupported PKCS#11 input fails explicitly. Existing PEM/DER certificate
handling, SHA512/CMS module signing and target forced signatures remain.
The small compatibility pattern is referenced to Android common Change-Id
I5ecac477c274ef040934710fd4a042c133942e34 (preserved in
`51023832d07f54381dab9b568c71eed72d408ac2`) and Linux upstream
`558bdc45dfb2669e1741384a0c80be9c82fa052c`, without importing its unrelated
provider/shared-error-helper refactor. The actual host-tool link failure is
not hidden by a prebuilt signer or a signature-enforcement change.

The full current kernel/module build exits0. There are13,845 exports,
reuseport_has_conns_set is added, none is removed, and8,884 common CRCs
change against73. Provider/export-type/namespace sets are unchanged. All390
modem and two optional dummy imports match;14 generated OEM selectors are
unchanged. New filesystem-owned fscrypt master keys include the actual
Huawei SDP consumer, active/structural refs and early keyslot teardown.
Actual selected HW_F2FS/ext4/crypto layouts, gfp-aware SELinux dispatch and
reduced IP protocol layouts agree. Final ELF checks32 targets in18 CFI
groups. These gates do not certify every opaque OEM contract or arbitrary
old-module reuse; matching required modules are rebuilt, not merely resigned.

Boot roundtrip and original ext4 metadata/CMS signature checks pass. One
paired #74 installation reaches normal Android with automatic matching
Live modem, partition/module/live build-id readbacks, unlocked CE storage,
required services and same-thread six-stage ISP startup. Independent
ramdisk/security remain and complete logs have no actual kernel/module/HKIP
fault. #73 is immediate paired recovery. Source format/key policy remain,
dummy stays uninstalled, and stable KMI/GKI remains open.

### 5.10.160 #75: Current SDHCI And Synchronous IPI Contracts

The complete canonical156-160 range accounts for520 originals and497 source
commits; a separate focused io_uring backup-allocation fix precedes the normal
full build. One actual Image/modules invocation exits0, producing5.10.160 #75.
The new TCP perturb order16 selection is equivalent to the old hardcoded
allocation; compiler/IAS, ThinLTO, CFI, signatures/signing material, SELinux
Enforcing and PRMEM/HKIP remain.

Current238 natural C contexts and eleven once-decoded interface objects bind
the selected SDHCI generic/platform/Zodiac layouts and typed callback dispatch,
actual io_uring quiesce/backup lifetime, five-argument trace poll, mirrored IP
wire headers and active synchronous page-table IPI callback. MMU_NOTIFIER is
not selected, so paired THP source notifier wrappers compile out; this is not
an active notifier test. Final ELF checks36 targets in22 finite CFI groups,
including final SDHCI core/Zodiac ops assignments and retained BPF/NFT/cpuset/
DWC3/IPv6 canonical assignments. No old74 interface decoder is replayed.

There are13,845 exports, with no addition/removal/provider/type/namespace
change against74. The682 shared CRC changes are real: all390 rebuilt modem
and two optional dummy imports match current Module.symvers; a previously
signed74 module is not a substitute. Fourteen generated OEM selectors and
signing material are unchanged. Required modem is stripped then CMS signed
and verified against the certificate embedded in the current kernel. Original
ext4 metadata/labels and byte readback plus boot roundtrip/bounds pass.

One paired75 installation reaches normal Android with automatically loaded
matching Live modem, full partition/file/live build-id readbacks, unlocked
CE/services and same-thread ISP startup. Full logs have no actual kernel/
module/HKIP fault;517 AVC denials remain, none for module loading. Independent
ramdisk and enforcing security stay, and original trace/Asleep are restored.
#74 is immediate paired recovery; dummy remains uninstalled. Current source
provenance includes one explicit157#147 git-am in-body From author/message
exception with exact source/date/signoffs retained, not a rewrite of history.
Same-build success is not opaque OEM ABI, independent module reuse, stable
KMI, ACK/vendor split or GKI certification.

### 5.10.161 #76: Complete Independent Stable Milestone

Complete161 accounts for18 originals and17 source commits. The actual full
Image/modules build exits0 with unchanged accepted75 configuration/security/
signing material. All13,845 exports retain CRCs, providers, types and
namespaces;390 rebuilt modem and two optional dummy imports match. Fourteen
generated OEM selectors and their provider sources remain. Three current
HID/loopback objects bind actual types/consumers and final ELF checks41
targets in27 CFI groups; previous75 layout/IR proofs are not replayed.

The new modem is rebuilt, stripped and CMS signed against the unchanged
embedded certificate, not substituted by a resigned old module. Original
ext4 labels/ownership/byte readback and boot roundtrip pass. One paired76
boot reaches normal Android with automatic Live modem, matching partition/
file/build-id readbacks, unlocked CE/services and complete same-thread ISP
startup. Full logs contain no actual kernel/module/HKIP fault;502 AVCs
remain, none module loading. Ramdisk and security remain, trace/Asleep restore,
and75 is immediate paired recovery. Dummy remains uninstalled.

The next162 io_uring replacement is not included. Its now-approved explicit
standard-mode/installed-default Hyperframe compatibility boundary requires
new actual layout, completion-lifetime and CFI checks in addition to rebuilt
modules; current zero CRC delta does not authorize an unchanged private ABI,
arbitrary old-module reuse, stable KMI or GKI certification.

## Repeatable Matched-Build Flow

1. Pin the source batch, target configuration, LLVM revision and signing
   material. Build in an independent output directory with the real device
   feature selections. Do not replace it with the generic released defconfig.
2. Complete the full kernel build to produce `Module.symvers`, generated
   headers and the required modules. In-tree modem code is already built in
   the same invocation; do not compile it independently with guessed flags.
3. For a genuine external module, use Kbuild against that exact configured
   output, preserving ARCH/LLVM/CFI settings. Build dependent modules together,
   or provide their version information through `KBUILD_EXTRA_SYMBOLS`.
4. Review missing/changed exports and relevant internal/OEM contracts. Check
   each required module's versioned imports. A later batch with changed CRCs
   requires the new module, not a patched `__versions` section.
5. Strip and sign the rebuilt module, verify CMS and the target certificate,
   stage it on the actual module partition with unchanged labels/ownership,
   and verify its filesystem and byte readback.
6. Repack the target kernel with the original boot layout, preserving the
   firmware stub and OEM DTB. Install the kernel and required module partition
   together before reboot; preserve the independent Magisk ramdisk.
7. Confirm normal boot, automatic module loading, matching runtime module
   build-id/file, full partition readback and enforcing security. Run only
   relevant regression gates, then retain the pair and immediate rollback.

For subsequent stable batches the user explicitly accepts normal Android
boot and automatic matching-module loading. Keep the integrity/security and
necessary boot-log checks in this flow, but do not repeat already-passed full
hardware tests without a new relevant change, fault or unresolved issue.

For the sequential upstream build output, the existing project commands are:

```sh
cd /home/data/Allen
OUTPUT_DIR=out/allen-clang12-upstream-5.10 JOBS=8 scripts/build-allen.sh
scripts/check-module-abi.py \
  out/allen-clang12-upstream-5.10/drivers/platform_drivers/modem/modem_driver.ko \
  out/allen-clang12-upstream-5.10/Module.symvers
```

The original staging scripts take a new output image path and refuse to
overwrite existing candidates. The previous accepted pair is #77's immediate
rollback, not the rejected #71:

- `images/test/allen-boot-test-5.10.161-clang12-r76.img`.
- `images/test/allen-modem_driver-test-5.10.161-clang12-r76.img`.
- `images/test/allen-modem_driver-test-5.10.161-clang12-r76.ko`.

The installed next pair uses `5.10.165-clang12-r77` with the same device/purpose
naming. It passes full matching kernel/module build, signed ext4 staging,
paired installation/readback and normal Android with automatic Live modem.

Complete162-165 imports the canonical in-tree/external `modules` and
`modules_install` unification while retaining Allen's real mixed-tree
vmlinux/builtin/depmod prerequisites. #77 has13,850 exports versus13,845 at
#76: six additions, one removal and8,966 changed CRCs, with no provider,
export-type or namespace changes. All390 rebuilt modem and two optional
dummy imports match the new table; old `__versions`, CRCs or vermagic are
not patched or reused. Signing material and14 existing OEM selectors remain;
22 new actual HF selectors follow the canonical replacement's real layouts.
Current task/HF/devfreq/CFI contracts are checked independently of the module
CRC gate. One normal Android boot automatically loads the signed matching
modem, with complete partition/file/build-id checks and unchanged independent
ramdisk/security. The nine private io_uring migration checks are additional
scoped evidence, not a stable-KMI, cellular-data or whole-device certificate.

These are local test/recovery files, not an exported release bundle or an
OEM-signed AVB claim. Their build, installation and runtime evidence is in
[linux-5.10-upstream-sync.md](linux-5.10-upstream-sync.md).

### 5.10.166-170 #78: Matching Current Interface Update

The current installed pair uses `5.10.170-clang12-r78`; #77/5.10.165 is the
immediate boot/modem_driver rollback. Full Image/modules compilation exits0
with unchanged effective configuration, signing keys and36 OEM/HF selectors.
All13,854 exports are compared with77's13,850: four new prepared/enabled
managed-clock helpers, no removal and three changed CRCs for
devm_nvmem_register, nvmem_register and rtc_nvmem_register. Provider, export
type and namespace stay. All390 rebuilt modem and two optional dummy imports
match the actual new table; no prior binary or synthetic versions are used.

Eleven necessary current objects establish typed managed-clock release,
112-byte NVMEM config agreement with RTC, new sysfs/sysctl callbacks and PSI
pollfree-before-RCU/free. Final97-target/55-type CFI memberships and actual
relative initcall/attribute/raw bridge assignments pass. Unchanged HF/task
providers retain77's explicitly linked facts without old object decode or
private runtime replay. These scoped interface checks remain separate from
the complete module CRC comparison, not stable KMI certification.

Original-format boot and signed modem ext4 staging pass certificate/CMS,
metadata/label and roundtrip checks. Paired installation reaches normal
Android with automatically loaded matching Live modem, full partition/file/
loaded build-id readbacks and unchanged independent ramdisk. Unlocked CE,
required services and automatic six-stage ISP startup pass; complete logs
have no actual kernel/module/HKIP fault.518 AVC denials remain, none for
module loading. SELinux Enforcing, forced signatures, taint4 and PRMEM stay;
read-only warn/oops counts are0. Trace0/wifi and Asleep are restored.

devm_get_clk_from_child's later upstream allocation-size repair remains a
named source-only finding; all six source consumers are unselected and the
actual modem does not import it. No child-clock unbind or unrelated dummy
load is used to enlarge acceptance. Required modules continue to follow
version updates by rebuilding against the complete current kernel, not by
relaxing validation. Next is complete171-175; all files remain local.

### 5.10.171-175 #79: Actual Poll, Filesystem And PCI Contracts

The installed pair uses `5.10.175-clang12-r79`; #78/5.10.170 is immediate
paired rollback. Complete647-original ordered source plus separate ACK
poll-allocation and mainline ext4-unlock corrections precede the one full
Image/modules build. Effective configuration, keys and36 selectors stay;
two already-disabled entries disappear through canonical Kconfig changes.

All13,854 exports remain with21 changed CRCs, including irq-domain,
io_uring socket and PCI host-bridge/DesignWare interfaces. Provider, export
type and namespace remain. All390 freshly rebuilt modem and two optional
dummy imports match. No CRC/vermagic editing, old module reuse or relaxed
loader substitutes for current compilation and interface checks.

Current selected poll/HF/task/HW_F2FS/ext4/HIDPP/trace/irq contracts,57 new
callbacks in35 types and final112-target/64-group CFI assignments pass.
The complete export inventory additionally triggers three actual PCI
objects: allocator, core reader and DesignWare agree on1024-byte host-bridge
layout and the new no_inc_mrrs bit/mask. These are necessary type/opaque
consumer checks separate from MODVERSIONS, not a stable-KMI claim.

Matching signed boot/modem staging, target certificate/CMS, ext4 metadata/
label and roundtrip checks pass. Only boot/modem_driver are installed;
independent ramdisk remains. Normal Android automatically loads the matching
Live modem, with partition/file/loaded build-id, unlocked CE/services and
six-stage ISP startup checks. Complete logs have no actual kernel/module/
HKIP fault;533 AVC denials remain, none module-load. Enforcing, mandatory
signatures, taint4 and PRMEM stay; read-only warn/oops counts are0.

Three finite UID1013/caps-zero private-ring regression groups cover current
async fixed-input splice, normal poll and registered-buffer source handling.
They do not certify arbitrary HF buffers, OOM/stress, cellular service or
full hardware. Trace0/wifi and Asleep restore. Compact79 metadata and
recent75-79 test pairs remain local; child-clock unselected-source and
ACK/vendor-core/GKI/KMI findings stay open. Next is complete176-180,
continuing mandatory same-build module updates without release publication.

### 5.10.176-180 #80: Changed Exports And Current Ownership

The installed pair uses `5.10.180-clang12-r80`; #79/5.10.175 is immediate
paired rollback. Complete841-original ordered source plus a separate imported
comment-whitespace cleanup precede one full Image/modules build. Effective
selections, keys and36 selectors remain; new canonical LLVM assembler
selectors describe the actual Clang12.0.7 IAS without changing the toolchain.

All13,857 exports are compared with79's13,854. Eight additions include the
new VFS SGID helpers, IPv6 cleanup/destructor, UDP common cleanup, MDIO owner
helpers and proc_dou8vec_minmax. Five removals include the obsolete VFS and
MDIO helpers and the old UDP exports.2732 common CRCs change; common
provider/export-type/namespace entries remain.390 freshly rebuilt modem and
two optional dummy imports match the actual new table. No old binary, forced
CRC/vermagic or permissive signature/loader substitutes for rebuilding.

511 current ordinary contexts and35 necessary objects check rq/SMT/task,
VFS/HW_F2FS, MAS/UFS, IPv6/NF and DWC3/TTY/trace contracts. Actual bio/request/
queue fields agree with selected raw crypto operands; generic inline crypto
is unselected, so canonical keyslot fixes are source-only. The56-byte
nfnetlink subsystem has cleanup removed; its preserved Android KABI macro
invocation emits no padding in this tree. No stable ABI is inferred from its
spelling.139 targets in81 final CFI groups, new protocol/transaction/PM tables
and UDP destructor pointer stores pass; direct-only TTY helpers are separate.
Unchanged HF/PCI proofs remain references without old IR/runtime replay.

Matching signed boot/modem staging checks original header/stub/DTB, CMS and
certificate, ext4 metadata/label and readback. Paired installation reaches
normal Android with automatic matching Live modem, full partition/file/
loaded build-id checks, unlocked CE/services and six-stage ISP startup.
Independent ramdisk remains. Complete logs have no actual kernel/module/
HKIP fault;519 AVC denials remain, none module-load. Enforcing, signaturesY,
taint4 and PRMEM stay; read-only warn/oops counts are0 and limits0/10000.
Trace0/wifi and Asleep restore. Required modules continue to follow each
complete kernel update by matching rebuild, not independent old-module reuse.

Compact80 metadata and recent76-80 pairs stay local; unselected child-clock,
full hardware/network/power and ACK/vendor-core/GKI/KMI findings stay open.
Next is complete181-185; no release export or publication occurs.

### 5.10.181-185 #81: New Socket And Regulator Layouts

The installed pair uses `5.10.185-clang12-r81`; #80/5.10.180 is immediate
paired rollback. Complete515-original sequential source needs no separate
integration before one full Image/modules build. Effective selections,
signing material and36 OEM/HF selectors remain. Disabled DECnet/SX8/RCAR/
DPT-I2O configuration spelling changes do not enable another driver.

All13,861 exports are compared with80's13,857: six additions are
device_set_node, the supplier-property and regulator-ramp helpers, DPCM
prepare and two USB endpoint checkers. Removed entries are neigh_lookup_nodev
and the old supplier current-limit setter.2742 common CRCs change; common
providers, export types and namespaces remain.390 newly rebuilt modem and
two optional dummy imports match the actual table. No old binaries, forced
CRC/vermagic or weakened signatures substitute for rebuilding.

609 current ordinary contexts, including66 actual regulator-header
consumers, and46 relevant objects bind new IO/Binder/TCP/ext4/HW_F2FS/
cgroup/DWC3/regulator/power contracts. Canonical sk_wait_pending grows
sock1072->1080 bytes and shifts tail fields by eight bytes; current shared
layouts and final UDP/IPv6 destructor stores at byte968 agree. An unchanged
primary C file alone is not grounds to reuse old offset evidence after a
shared header changes. New regulator_desc fields agree in generic, Kirin
and camera PMIC providers. device remains896 bytes; its empty archdata
member is not mistaken for a flexible array. Current187-target/91-type final
CFI, callback tables, Binder explicit partial/full bounds, conditional IO
mutex and15 DWC3 PM owners/18 release sites pass. Unchanged HF/PCI/rq/MAS
evidence stays scoped; no independent stable-KMI promise follows.

Same-build signed boot/modem staging checks original header/stub/DTB,
certificate/CMS, ext4 metadata/label and module readback. Only boot and
modem_driver are installed. Normal Android automatically loads the matching
Live modem; partition/file/loaded build-id, unlocked CE/services and
six-stage automatic ISP startup pass. Complete logs have no actual kernel/
module/HKIP fault;518 AVC denials remain, none module-load. Independent
ramdisk, Enforcing, mandatory signatures, taint4 and PRMEM stay; read-only
warn/oops counts are0. Trace0/wifi and Asleep restore. No optional dummy
installation or broad private-ring replay is used to inflate acceptance.

Compact81 metadata and recent77-81 pairs remain local with80 immediate
recovery. Next is complete186-190 toward pinned5.10.274. Full hardware,
network/power, ACK parity, vendor-core separation and GKI/KMI remain open;
required modules continue to follow each kernel update by paired rebuild.
No release export or publication occurs.

### Complete186-190 / Accepted #82

Required modules continue to follow stable releases by paired rebuilding,
not by independent old-binary reuse. Complete838 original dispositions and
the required official trace fmt-release followup produce5.10.190 #82 from
source `46243eea39`. Effective/emitted configuration, Android Clang/LLD12.0.7,
LLVM/IAS, ThinLTO, enforcing CFI, BTI/BHB, actual Werror, trusted signing
material and36 OEM/HF selectors remain unchanged.

The full export comparison is13,861->13,865: additions are __sock_i_ino,
dev_pm_set_dedicated_wake_irq_reverse, tcp_bpf_bypass_getsockopt,
trace_event_printf and xfrm4_udp_encap_rcv; nft_parse_register is removed.
8898 common CRCs change; common providers/export types/namespaces remain.
All390 freshly rebuilt modem and two optional dummy imports match. Signature
verification, stock-format boot roundtrip and modem ext4 label/ownership/
signed-byte readback pass. No CRC, vermagic, CFI or signature bypass occurs.

2448 current ordinary contexts and61 relevant objects bind35 layout groups,
IO/HF, NFT/pernet/proto, trace, actual HW_F2FS/ext4/VFS, SDHCI/Zodiac,
GPIO/HIV500 and DWC3/PM. The new message-control field expands io_sr_msg
48->56 bytes but not the64-byte request union or240-byte request. HF selectors
are derived again from current C layouts;15 typed bridges,11 registrations,
work/completion/allocation operands and11 actual HIV500 callback stores pass.
211 indirect targets in107 current final CFI types and protocol/pernet/trace
tables agree. Unchanged HF private raw allocation and MAS/PCI facts remain
scoped; they do not certify all opaque assembly or stable KMI. Direct-call-only
HF completion is checked as a branch, not an invented indirect CFI target.

Only local190/#82 boot/modem_driver are flashed. Normal Android automatically
loads matching signed Live modem; full partition/file/loaded build-id,
unlocked CE/services and automatic same-thread six-stage ISP firmware pass.
Time-zero, boot and later logs have no actual kernel/module/HKIP fault;
518 AVC denials remain, none module-load. WARN/Oops counts stay0, limits
0/10000. Independent ramdisk, Enforcing, signaturesY, taint4, PRMEM and
trace0/wifi remain, and screen returns Asleep. Optional dummy installation,
cellular service and broad private-ring/hardware coverage are not implied.

Compact82 metadata and local78-82 pairs retain81 immediate recovery. Complete
191-195 follows toward pinned274. Stable module reuse still requires a chosen
Android KMI branch, toolchain/configuration and monitored vendor symbol/type
contract; current stable imports are not GKI or stable-KMI certification.
No release export, tag, push or publication occurs.

### Complete191-195 / Accepted #85

699 originals and separately recorded scheduler/cpuset integrations produce
195. Candidate84 fails in init due to the official5.10.195 configfs bad
backport recursively acquiring configfs_dirent_lock. Recovery preserves
ramoops/rrecord/bootfail and restores82. Official stable revert2cc1f4986233
is imported separately as38bf2c8e46; repaired85 is the accepted runtime.
The original cursor race remains for the later correct upstream fix.

Effective selections, Clang/LLD12.0.7, LLVM/IAS, ThinLTO, enforcing CFI,
BTI/BHB, actual Werror, trusted keys and36 selectors remain.4922 natural
contexts bind the actual archives/modules. Only configfs/version objects
change after84; exact50 interface objects retain their scoped facts and one
fresh configfs decode verifies balanced locks.226 final targets/116 CFI groups
and current HF/GPIO stores pass. Exports13,865->13,866 add managed MMC host,
eventfd read and coordinated rtnetlink parser; the old rtnetlink parser and
TCP quickack helper disappear.190 common CRCs change.390 rebuilt modem and
two optional dummy imports match; providers/types/namespaces remain.

Matched signed85 boot/modem reach normal Android and automatic Live modem,
with full partition/file/build-id readbacks, unlocked CE/services and automatic
six-stage ISP firmware. Logs and read-only WARN/Oops counts pass;504 AVCs
remain, none module-load. Enforcing, signaturesY, taint4, PRMEM, independent
ramdisk and trace0/wifi remain; screen is Asleep.82 is immediate recovery.
No old-binary reuse, dummy installation, CRC/vermagic spoofing or weakened
security is involved. Failed84 evidence remains separate. Complete196-200,
ACK/vendor split and stableKMI/GKI remain open; no release publication.

## Actual Module Inventory And Limits

The full configured build also produces `hisi_dummy_ko.ko`. Read-only normal
Android inventory finds only the modem module file and only that module
loaded. The existing `init.ko.rc` requests the dummy module under a missing
`/vendor/lib/modules` directory; the command exits with status 1 on both r49
and #50. The dummy module is not substituted for hardware, manually loaded
to fake coverage, or added to a new partition. Its optional userspace/source
integration remains a separate issue.

The configured #52 modem links 262 objects, all with corresponding available
C sources. Its inspected sources and undefined symbols do not use credential
or ucounts APIs. The core still retains twenty generated OEM assembly units
and private platform contracts; old type debug metadata is not evidence that
their runtime offsets remain compatible. The 5.10.50 credential-layout audit
finds no concrete new assembly access mismatch, but does not certify the full
binary ABI. Same-build compilation and a successful modem initialization do
not establish working calls or cellular data. Available hardware/SIM/manual
gates remain in [clang12-completion-plan.md](clang12-completion-plan.md).

## Independent Module Reuse Later

To update the core without rebuilding vendor binaries, first establish a
version-matched Android common base, a chosen/frozen KMI generation, explicit
vendor symbol lists and type-level ABI monitoring under a pinned toolchain.
Separate hardware-specific core hooks into vendor modules only after their
dependencies and runtime contracts are understood. Stay within that KMI
branch and enforce its allowlist/CRC constraints. A new Android KMI branch
or incompatible API/layout requires rebuilding/adapting vendor modules.

The present stable batch establishes neither that architecture nor a blanket
promise of module reuse. Its working update method is paired rebuilding and
verified installation, without spoofing versions or weakening protections.

### Complete 5.10.196-200 / Accepted #86

The #86 kernel and required `modem_driver.ko` were built together by the
configured Kbuild using the existing configuration, generated headers,
`Module.symvers`, Android Clang/LLD12.0.7 and trusted SHA-512 signing key. The
stripped module's390 versioned imports all match the current export CRCs; the
optional dummy module's two imports also match. Four exports are added, none
removed and3,295 common CRCs change versus #85. These changes are recorded,
not hidden by reusing or resigning the old module.

The staged `boot` and `modem_driver` ext4 images were produced from this same
build, verified against the kernel's certificate and original filesystem
metadata, then flashed as a pair. Device readback and the loaded modem build
ID match the staged bytes. The independent Magisk ramdisk was not touched.
This demonstrates the currently supported update workflow, not a stable ABI
across kernel sublevels.

For independently reusable vendor modules, the project still needs a
version-matched Android common/GKI base, frozen KMI generation, explicit
allowlist/symbol lists, ABI type monitoring and a deliberate split of OEM
hooks from core code. `CONFIG_MODVERSIONS`, matching vermagic, successful
signature verification or this paired boot pass alone do not establish KMI
compatibility. Until that work is done, rebuild every required module from
the matching source/configuration/toolchain and install it with its kernel.

### Complete 5.10.201-205 / Accepted #87

Kernel #87 and modem are rebuilt together with the same configuration,
Clang/LLD12.0.7, generated headers and trusted signing key. All390 modem and
two optional dummy imports match current exports. Four exports are added,
none removed and8,986 common CRCs change versus #86: old binaries are not
reused, CRCs/vermagic are not spoofed and security is not relaxed.

The same-build signed boot/modem pair boots normal Android5.10.205 and
automatically loads the Live modem. Partition/file/build-ID readbacks,
certificate/filesystem checks and independent ramdisk preservation pass.
Current device/credential/perf/MMC/GPIO/trace shared layouts and final CFI
contracts are checked rather than inherited from older offsets. Immediate
paired recovery is #86. Independent module reuse still requires a matched
Android common base, explicit KMI generation/symbol policy, type ABI checks
and OEM dependency separation; this boot pass does not establish GKI/KMI.

### Complete 5.10.206-210 / Accepted #89

The actual #89 kernel and required modem module are built together with the
same current configuration/generated headers, Clang/LLD12.0.7 and trusted
signing key. All390 modem imports and two optional dummy imports match the
current export table. Six exports are added, none removed and9,000 common
CRCs change versus #87. Current BPF release callback signatures, LSM compat
hook placement, hrtimer and OEM shared consumers are independently checked.

The same-build boot/modem pair boots normal Android5.10.210, automatically
loads the matching Live modem and passes partition/file/build-ID readbacks.
The independent ramdisk and security protections remain. #87 is paired
recovery. This is paired rebuilding, not stable KMI: independently reusable
vendor binaries still require an Android common base, explicit KMI generation
and symbol policy, type-level ABI monitoring and OEM/core dependency split.

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

### Linux 5.10.216-220: Matching Module Acceptance On #93

The complete frozen stable range is recorded in the 216-220 TSV ledgers. The
220 source includes 751 exact upstream imports, 18 necessary adaptations and
two already-present changes. The selected Huawei tree retains VFS/NFS locking
contracts while following upstream fdtable lookup, `renamedata`, module
termination and kallsyms interfaces. Two post-release OEM integration commits
are recorded separately.

Build #93 completes with Android Clang/LLD 12.0.7, ThinLTO, CFI, forced
module signatures, SELinux and PRMEM/HKIP. The newly defined
`CONFIG_CPU_MITIGATIONS=y` is the only effective configuration delta from
accepted #91 metadata. The modem's 390 versioned imports and the optional
dummy module's two imports match current CRCs; this does not certify stable
KMI/GKI.

The paired 5.10.220/#93 `boot` and `modem_driver` images pass format,
signature, filesystem and metadata staging. Device acceptance passes normal
Android startup, automatic matching Live modem loading, partition and module
readback/build-ID agreement, SELinux Enforcing, signature enforcement, taint
4, PRMEM, services, unlocked CE, ISP startup, trace restoration and asleep
screen. No classified kernel/module fault appears in relevant logs; WARN/Oops
counts are zero and 511 AVCs include no module-load denial. The independent
Magisk ramdisk remains unchanged. Immediate paired recovery is 5.10.219/#92;
no release export or publication was made.
