#!/usr/bin/env bash
set -euo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
if (( $# != 2 )); then
    printf 'Usage: %s <uncompressed ARM64 Image> <new boot image>\n' "$0" >&2
    exit 1
fi
kernel=$(realpath -- "$1")
output=$(realpath -m -- "$2")
stock="$PWD/boot.img"
magiskboot="$PWD/tools/host/bin/magiskboot"
if [[ -e "$output" ]]; then
    printf 'Refusing to overwrite an existing image: %s\n' "$output" >&2
    exit 1
fi

printf 'Working directory: %s\n' "$PWD"
git status --short
mkdir -p .tmp
work=$(mktemp -d "$PWD/.tmp/allen-boot.XXXXXX")
cleanup() {
    result=$?
    if (( result != 0 )); then
        printf 'Boot repack or validation failed (exit %s).\n' "$result" >&2
        for log in "$work/repack.log" "$work/verify.log"; do
            if [[ -f "$log" ]]; then
                tail -n 12 "$log" >&2
            fi
        done
    fi
    rm -rf -- "$work"
}
trap cleanup EXIT
mkdir "$work/components" "$work/verify"

# Keep the compressed firmware stub and OEM DTB container byte-for-byte.
(
    cd "$work/components"
    "$magiskboot" unpack -n -h "$stock"
    cp -- "$kernel" kernel
    PATCHVBMETAFLAG=false "$magiskboot" repack "$stock" "$work/boot.img"
) >"$work/repack.log" 2>&1
(
    cd "$work/verify"
    "$magiskboot" unpack -n "$work/boot.img"
    "$magiskboot" decompress kernel kernel.dec
) >"$work/verify.log" 2>&1
cmp -s "$kernel" "$work/verify/kernel.dec"
cmp -s "$work/components/ramdisk.cpio" "$work/verify/ramdisk.cpio"
cmp -s "$work/components/dtb" "$work/verify/dtb"

python3 - "$kernel" "$stock" "$work/boot.img" <<'PY'
import struct
import sys
from pathlib import Path

kernel, stock, candidate = map(Path, sys.argv[1:])
with kernel.open('rb') as source:
    assert source.read(64)[56:60] == b'ARM\x64', 'Expected an uncompressed ARM64 Image'
headers = []
for image in (stock, candidate):
    with image.open('rb') as source:
        header = source.read(1660)
        assert header[:8] == b'ANDROID!'
        fields = struct.unpack_from('<10I', header, 8)
        assert fields[7:9] == (2048, 2), 'Unexpected page size or header version'
        source.seek(fields[7])
        assert source.read(4) == b'\x02\x21\x4c\x18', 'Expected legacy LZ4'
        source.seek(-64, 2)
        assert source.read(4) == b'AVBf', 'AVB footer missing'
    headers.append(header)
assert headers[0][12:576] == headers[1][12:576], 'Firmware header fields changed'
assert headers[0][608:1632] == headers[1][608:1632], 'Extended cmdline changed'
assert headers[0][1644:1660] == headers[1][1644:1660], 'DTB/header fields changed'
assert candidate.stat().st_size <= stock.stat().st_size, 'Candidate exceeds boot partition'
print('Kernel round-trip, firmware stub, OEM DTB, header fields and partition bound pass.')
print('This is an unlocked-device test image, not an OEM-signed AVB validation claim.')
PY
mkdir -p -- "$(dirname -- "$output")"
mv -- "$work/boot.img" "$output"
printf 'Candidate boot image: %s\n' "$output"
