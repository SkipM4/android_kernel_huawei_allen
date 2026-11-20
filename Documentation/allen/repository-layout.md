# Repository Layout

The repository root is the Linux kernel source root. `Makefile`, `Kconfig`,
`COPYING`, `arch/`, `drivers/`, `include/`, `scripts/`, `tools/` and the other
kernel directories are used directly. There is no enclosing `Code_Opensource`
or `kernel` directory in the current checkout.

## Build Inputs

The supported profiles are `arch/arm64/configs/allen_secure_defconfig` and
`arch/arm64/configs/allen_relaxed_defconfig`. Both use the unchanged public
trust certificate at `certs/allen-stock-module-signing.pem`. No external stock
config, public key directory or vendor signing program is required by these
profiles. Kbuild generates a new private module-signing key in the chosen
output directory unless the builder supplies one.

Use Android Clang/LLD 12.0.7 and the root `README.md` instructions. The local
`scripts/build-allen.sh` also defaults to this source root, accepts a
`KERNEL_DIR` override, and preserves the established LLVM/IAS and ThinLTO flags.
Its default external compiler location is `tools/toolchains/clang-r416183b1`;
that downloaded toolchain and `tools/host` remain ignored local dependencies.

The original kernel's Android common contribution guide is preserved at
`Documentation/process/android-common-patches.md`. Project audits and stable
import ledgers are in `Documentation/allen/`, including the former workspace
README as `history.md`.

## History

This layout is a normal descendant of the previously published
`124f5cb945456927a2c92d8b4fa5a40c1c5f4054`. The earlier 32,375 commits retain
their original hashes and authors. Old paths in historical commands and audit
entries describe the layout at those commits; for current paths, remove the
`Code_Opensource/kernel/` prefix and replace `docs/` with `Documentation/allen/`.
Historical config fragments remain retrievable from those commits and are not
additional supported profiles.

The unrelated original `external` and `vendor/open_source` projects are absent
from the current tree and remain in the untouched original distribution and
Git ancestors. The empty stock import artifact named `-c` is also removed.
The local operational `AGENTS.md`, firmware/boot backups,
images, output, archive, compiler and private signing material remain ignored.
They are not needed to compile the source.

## Validation Scope

Runtime C/assembly, both defconfigs and the public certificate retain their
previous contents. The maintained changes are layout, build-tool defaults,
documentation and archive entry paths. The root configurations are validated
against the accepted #101/#103 effective configuration and generated OEM
offsets using fresh `O=` outputs, `savedefconfig`, `modules_prepare` and
`certs/`. This layout check does not extend the prior hardware acceptance or
claim GKI/KMI certification.

Both fresh root-directory profile checks passed: their regenerated effective
configuration and OEM selectors match the recorded #101/#103 baselines, and
`savedefconfig`, `modules_prepare` and `certs/` all completed. No full kernel
rebuild, device reboot or flash was performed for this directory-only change.
