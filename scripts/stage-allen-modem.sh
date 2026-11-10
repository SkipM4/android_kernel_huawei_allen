#!/usr/bin/env bash
set -euo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
if (( $# != 2 )); then
    printf 'Usage: %s <kernel output directory> <new modem_driver image.img>\n' "$0" >&2
    exit 1
fi
build=$(realpath -- "$1")
image=$(realpath -m -- "$2")
module="${image%.img}.ko"
stock="$PWD/images/backup/allen-stock-5.10.43-20250805-modem_driver.img"
llvm="$PWD/tools/toolchains/clang-r416183b1/bin"
if [[ "$image" != *.img || -e "$image" || -e "$module" ]]; then
    printf 'Expected a new .img path with no existing image or companion module.\n' >&2
    exit 1
fi

mkdir -p .tmp "$(dirname -- "$image")"
work=$(mktemp -d "$PWD/.tmp/allen-modem.XXXXXX")
cleanup() {
    result=$?
    if (( result != 0 )); then
        printf 'Modem staging or validation failed (exit %s).\n' "$result" >&2
        for log in "$work"/*.log; do
            [[ ! -f "$log" ]] || tail -n 8 "$log" >&2
        done
    fi
    rm -rf -- "$work"
}
trap cleanup EXIT

cp -- "$build/drivers/platform_drivers/modem/modem_driver.ko" "$work/module.ko"
"$llvm/llvm-strip" --strip-debug "$work/module.ko"
"$build/scripts/sign-file" sha512 "$build/certs/signing_key.pem" \
    "$build/certs/signing_key.x509" "$work/module.ko"
scripts/check-module-abi.py "$work/module.ko" "$build/Module.symvers"

python3 - "$work" "$build" <<'PY'
import struct
import sys
from pathlib import Path

work, build = map(Path, sys.argv[1:])
data = (work / 'module.ko').read_bytes()
magic = b'~Module signature appended~\n'
assert data.endswith(magic), 'Module signature missing'
end = len(data) - len(magic) - 12
_, _, identifier, signer, key_id, size = struct.unpack('>5B3xI', data[end:end + 12])
assert identifier == 2 and signer == key_id == 0, 'Expected a PKCS#7 signature'
(work / 'signature.der').write_bytes(data[end - size:end])
(work / 'payload.bin').write_bytes(data[:end - size])
cert = (build / 'certs/signing_key.x509').read_bytes()
assert cert in (build / 'arch/arm64/boot/Image').read_bytes(), 'Signing certificate absent from kernel'
PY
openssl x509 -inform DER -in "$build/certs/signing_key.x509" -out "$work/cert.pem"
openssl cms -verify -binary -inform DER -in "$work/signature.der" \
    -content "$work/payload.bin" -certfile "$work/cert.pem" \
    -nointern -noverify -out /dev/null

# Clone the original filesystem and preserve its exact module SELinux label.
debugfs -R "ea_get -f \"$work/label\" /modem_driver.ko security.selinux" \
    "$stock" > "$work/metadata.log" 2>&1
cp -- "$stock" "$work/modem.img"
cat > "$work/edit.cmd" <<EOF
rm /modem_driver.ko
write "$work/module.ko" /modem_driver.ko
set_inode_field /modem_driver.ko uid 0
set_inode_field /modem_driver.ko gid 0
set_inode_field /modem_driver.ko mode 0100644
ea_set -f "$work/label" /modem_driver.ko security.selinux
EOF
debugfs -w -f "$work/edit.cmd" "$work/modem.img" > "$work/edit.log" 2>&1
if e2fsck -fy "$work/modem.img" > "$work/repair.log" 2>&1; then
    :
else
    result=$?
    # e2fsck status 1 means the known debugfs bitmap mutation was corrected.
    (( result == 1 )) || exit "$result"
fi
e2fsck -fn "$work/modem.img" > "$work/check.log" 2>&1
debugfs -R "dump /modem_driver.ko \"$work/readback.ko\"" "$work/modem.img" \
    > "$work/readback.log" 2>&1
debugfs -R "ea_get -f \"$work/label-readback\" /modem_driver.ko security.selinux" \
    "$work/modem.img" >> "$work/readback.log" 2>&1
cmp -s "$work/module.ko" "$work/readback.ko"
cmp -s "$work/label" "$work/label-readback"
debugfs -R 'stat /modem_driver.ko' "$work/modem.img" > "$work/stat.log" 2>&1
rg -q 'Mode:  *0644' "$work/stat.log"
rg -q 'User:  *0 +Group:  *0' "$work/stat.log"

mv -- "$work/module.ko" "$module"
mv -- "$work/modem.img" "$image"
printf 'Signed matching module, certificate, ext4 and metadata readback checks pass.\n'
printf 'Candidate modem_driver image: %s\n' "$image"
