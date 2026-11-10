#!/usr/bin/env python3
"""Compare an installed module's versioned imports with a built kernel."""

import argparse
import subprocess
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("module", type=Path)
    parser.add_argument("symvers", type=Path)
    args = parser.parse_args()

    recorded = subprocess.check_output(
        ["modprobe", "--show-modversions", str(args.module)], text=True
    )
    imports = {}
    for line in recorded.splitlines():
        crc, symbol = line.split()
        imports[symbol] = int(crc, 16)

    exports = {}
    for line in args.symvers.read_text().splitlines():
        crc, symbol, *_ = line.split()
        exports[symbol] = int(crc, 16)

    missing = sorted(imports.keys() - exports.keys())
    mismatched = sorted(
        symbol for symbol in imports.keys() & exports.keys()
        if imports[symbol] != exports[symbol]
    )
    vermagic = subprocess.check_output(
        ["modinfo", "-F", "vermagic", str(args.module)], text=True
    ).strip()
    release = (args.symvers.parent / "include/config/kernel.release").read_text().strip()
    release_matches = vermagic.split()[0] == release

    print(f"Module vermagic: {vermagic}")
    print(f"Kernel release: {release}; release matches: {release_matches}")
    print(f"Versioned imports: {len(imports)}")
    print(f"Missing exports: {len(missing)}; sample: {', '.join(missing[:12])}")
    print(f"CRC mismatches: {len(mismatched)}; sample: {', '.join(mismatched[:12])}")
    print("This checks versioned imports, not runtime CFI behavior or successful boot.")
    return int(not release_matches or bool(missing) or bool(mismatched))


if __name__ == "__main__":
    raise SystemExit(main())
