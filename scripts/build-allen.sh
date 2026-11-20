#!/usr/bin/env bash
set -euo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
toolchain="$PWD/tools/toolchains/clang-r416183b1"
kernel=$(realpath -- "${KERNEL_DIR:-$PWD}")
output=$(realpath -m -- "${OUTPUT_DIR:-$PWD/out/allen-clang12}")
export PATH="$PWD/tools/host/bin:$toolchain/bin:$PATH"

printf 'Working directory: %s\n' "$PWD"
git status --short
git branch --show-current
git log -1 --oneline
for tool in python gcc clang make; do
    command -v "$tool"
    "$tool" --version | head -n 1
done
"$toolchain/bin/ld.lld" --version
bc --version | head -n 1
df -h .
usage=$(du -s --block-size=1 . | cut -f1)
if (( usage >= 20000000000 )); then
    printf 'Workspace exceeds the 20 GB limit; stop and ask before continuing.\n' >&2
    exit 1
fi

mkdir -p "$output" .tmp
log="$PWD/.tmp/build-clang12.log"
printf 'Building with Android Clang 12; log: %s\n' "$log"
if make -C "$kernel" \
    ARCH=arm64 O="$output" LLVM=1 LLVM_IAS=1 \
    CROSS_COMPILE=aarch64-linux-gnu- CROSS_COMPILE_COMPAT=arm-linux-gnueabi- \
    CLANG_PREBUILTS_PATH="$toolchain/" LOCALVERSION= \
    LD="$toolchain/bin/ld.lld --thinlto-jobs=2 --thinlto-cache-policy=cache_size_bytes=1g" \
    -j"${JOBS:-8}" "$@" >"$log" 2>&1; then
    tail -n 10 "$log"
else
    result=$?
    rg -n -m 12 'fatal error:|error:|No rule to make target|Error [1-9]|not found' "$log" || true
    tail -n 20 "$log"
    exit "$result"
fi
