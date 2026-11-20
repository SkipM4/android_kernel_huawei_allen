# Allen 5.10.274 Stage Closeout

The stable synchronization milestone ends at Linux 5.10.274. The secure
accepted configuration is #101; the installed local debug configuration is
#103. Both reached normal Android with the matching automatically loaded modem
module. #103 started SELinux Enforcing, completed a Magisk `setenforce 0` / `1`
round trip, and was left Enforcing. The independent ramdisk was unchanged.
These remain scoped device checks, not full hardware/power or GKI/KMI approval.

## One Tree, Two Defconfigs

The supported configurations are standard `savedefconfig` output in
`Code_Opensource/kernel/arch/arm64/configs/`:

- `allen_secure_defconfig` restores the #101 security choices.
- `allen_relaxed_defconfig` reproduces #103's relaxed signature/root-scan
  choices and post-boot SELinux mode control; it starts Enforcing.

The legacy `merge_charlotte_defconfig` and the separate test-only fragment were
replaced by those two complete profiles. The original material remains in Git
history and in the untouched vendor source archive. There are no duplicate
kernel source trees for the two profiles.

The existing public module certificate moved from the workspace `configs/`
directory to `Code_Opensource/kernel/certs/allen-stock-module-signing.pem`.
Only its path changed. Regenerating each defconfig produces the respective
tested effective configuration after this path mapping, with no other
effective configuration difference. Saving the regenerated configurations back
to defconfig reproduces the committed files. Runtime C and assembly code were
not changed by packaging.

## Cleanup And Local Retention

The original vendor source archive, original boot/Magisk ramdisk, original
partition backups, device-tree material, Git history and external compiler
remain local. The retained five image/module groups are #97, #99, #100, #101
and #103; #101 is the precise secure rollback for #103. Failed #65/#71 evidence
and compact accepted-build metadata are also retained.

Consumed stable-import mirrors/previews, old analysis scripts, command/IR
replay snapshots, complete diagnostic logs, and reproducible kernel build
outputs were retired. The active output directory retains the next-build
configuration/counter and its private signing key/certificate. Duplicate copies
of that same private key were removed. No private key enters the source package.

Current #103 configuration, build/module identities, runtime results, and log
digests/counts remain under `out/allen-clang12-r103-metadata/`; the #101 secure
metadata remains under its existing sibling directory. The cleanup inventory
is local under `out/allen-clang12-phase274-metadata/`. Historical temporary-path
references describe already completed analyses; those caches are intentionally
not kept as a second source/build system.

## Source Archive

`scripts/package-allen-source.py` constructs
`dist/linux-5.10.274-allen.tar.xz` from a committed kernel subtree. It contains
one conventional kernel-at-root directory with `COPYING`, `LICENSES`, source,
the two defconfigs, the public trust certificate, and `README.allen`.

The archive keeps source file permissions and original notices, normalizes
ownership/timestamps to the source commit, and includes `SOURCE_INFO.json` and
a per-file `SHA256SUMS`. The adjacent `.tar.xz.sha256` checks the compressed
archive. It excludes local images, device identities/logs, compiler binaries,
`.git`, private keys, and build/cache output. Sixteen reviewed stock artifacts
are filtered: five unused host ELF binaries, ten unselected firmware blobs,
and one empty import artifact. Test datasets, optional testing tools, developer
analysis/debug scripts, GDB/tracing helpers and binary/log test fixtures are
also excluded at the user's request. The complete exclusions are recorded in
`SOURCE_INFO.json`; kernel C/assembly and required Kbuild scripts are retained.

The archive is checked by extracting it, validating every file against its
manifest, preserving file modes, and regenerating both defconfigs from that
extraction. This checks the source package; the earlier complete kernel builds
and device tests are not repeated by this phase. New builds generate new local
signing keys, so binary identity with #101/#103 is not promised.

This is a locally prepared source archive. No upload, tag, push or public
release operation is performed. Android common parity, removal of remaining
OEM coupling, stable KMI/GKI certification and broad device acceptance remain
the next phase.
