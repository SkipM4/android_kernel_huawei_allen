# Kernel Module Updates

## Policy For Allen

Update the kernel and all required loadable modules as a matched build while
the Huawei tree has no independently maintained stable KMI. For the installed
normal Android system, the required loadable module is `modem_driver.ko` on
the independent read-only ext4 `modem_driver` partition. The installed 5.10.95
#60 kernel and module are built together with the r49-derived effective
configuration, Android Clang/LLD 12.0.7, enforcing CFI and the existing signing
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
overwrite existing candidates. The previous pair is #60's immediate rollback:

- `images/test/allen-boot-test-5.10.90-clang12-r59.img`.
- `images/test/allen-modem_driver-test-5.10.90-clang12-r59.img`.
- `images/test/allen-modem_driver-test-5.10.90-clang12-r59.ko`.

The installed next pair uses `5.10.95-clang12-r60` with the same device/purpose
naming. It passes full matching kernel/module build, signed ext4 staging,
paired installation/readback and normal Android with automatic Live modem.

These are local test/recovery files, not an exported release bundle or an
OEM-signed AVB claim. Their build, installation and runtime evidence is in
[linux-5.10-upstream-sync.md](linux-5.10-upstream-sync.md).

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
