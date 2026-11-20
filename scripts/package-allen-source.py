#!/usr/bin/env python3
"""Create one reproducible, source-only Allen kernel tar.xz from a Git commit."""

import argparse
import hashlib
import io
import json
import os
from pathlib import Path
import re
import subprocess
import tarfile


ROOT = Path(__file__).resolve().parent.parent
PACKAGE = 'linux-5.10.274-allen'
OMIT = {
    '-c': 'Empty stock import artifact.',
    'tools/dtbTool': 'Unused OEM prebuilt host ELF tool.',
    'tools/dtc': 'Unused OEM prebuilt host ELF tool; DTC source is retained.',
    'tools/minigzip': 'Unused OEM prebuilt host ELF tool.',
    'tools/mkbootimg': 'Unused OEM prebuilt host ELF tool.',
    'tools/lib64/libc++.so': 'Unused OEM prebuilt host ELF library.',
}
OMIT_PREFIXES = (
    'scripts/clang-tools/', 'scripts/coccinelle/', 'scripts/gdb/',
    'scripts/ksymoops/', 'scripts/tracing/', 'scripts/kconfig/tests/',
    'tools/testing/', 'tools/perf/tests/', 'tools/perf/scripts/',
    'tools/debugging/', 'tools/build/tests/', 'tools/memory-model/',
    'tools/virtio/ringtest/', 'tools/virtio/vhost_test/',
    'tools/bootconfig/samples/', 'tools/bootconfig/scripts/',
    'Documentation/trace/postprocess/',
)
OMIT_HELPERS = {
    'scripts/bloat-o-meter', 'scripts/bootgraph.pl', 'scripts/checkstack.pl',
    'scripts/decode_stacktrace.sh', 'scripts/decodecode', 'scripts/faddr2line',
    'scripts/leaking_addresses.pl', 'scripts/markup_oops.pl', 'scripts/objdiff',
    'scripts/show_delta', 'scripts/stackdelta', 'scripts/stackusage',
    'scripts/spdxcheck-test.sh', 'tools/bootconfig/test-bootconfig.sh',
}
PRIVATE_KEY = re.compile(rb'-----BEGIN (?:[A-Z0-9]+ )*PRIVATE KEY-----')


def git(*args):
    return subprocess.check_output(['git', '-C', str(ROOT), *args]).decode().strip()


def add_bytes(archive, name, data, timestamp):
    entry = tarfile.TarInfo(f'{PACKAGE}/{name}')
    entry.mode = 0o644
    entry.mtime = timestamp
    entry.size = len(data)
    archive.addfile(entry, io.BytesIO(data))


def package(revision, output):
    commit = git('rev-parse', '--verify', revision + '^{commit}')
    timestamp = int(git('show', '-s', '--format=%ct', commit))
    tree = git('rev-parse', f'{commit}^{{tree}}')
    files = git('ls-tree', '-r', '--name-only', commit).splitlines()
    omitted = dict(OMIT)
    omitted.update({name: 'Unselected OEM runtime firmware blob; EXTRA_FIRMWARE is empty.'
                    for name in files if name.startswith('firmware/') and name.endswith('.wa')})
    assert set(OMIT).issubset(files), 'Re-review changed archive filters'
    assert len(omitted) == 16, 'Re-review changed OEM firmware/tool inventory'
    omitted.update({name: 'Test data, optional testing tools, or diagnostic/developer helper.'
                    for name in files if name.startswith(OMIT_PREFIXES) or name in OMIT_HELPERS})
    required = {'COPYING', 'LICENSES/preferred/GPL-2.0', 'Makefile', 'README.md',
                'certs/allen-stock-module-signing.pem',
                'arch/arm64/configs/allen_secure_defconfig',
                'arch/arm64/configs/allen_relaxed_defconfig'}
    assert required.issubset(files), 'Commit is missing release source/configuration'
    configs = {name for name in files if name.startswith('arch/arm64/configs/')}
    assert configs == {name for name in required if name.endswith('_defconfig')}
    output.mkdir(parents=True, exist_ok=True)
    target = output / f'{PACKAGE}.tar.xz'
    checksum = output / f'{target.name}.sha256'
    partial = output / f'{target.name}.part'
    assert not any(path.exists() for path in (target, checksum, partial)), 'Refusing to overwrite an archive'
    checksums = []
    env = dict(os.environ)
    env.pop('XZ_OPT', None)
    env.pop('XZ_DEFAULTS', None)
    with partial.open('xb') as destination:
        compressor = subprocess.Popen(['xz', '-T2', '-4', '-c'], stdin=subprocess.PIPE,
                                      stdout=destination, env=env)
        source = subprocess.Popen(['git', '-C', str(ROOT), 'archive', '--format=tar',
                                   commit], stdout=subprocess.PIPE)
        try:
            with tarfile.open(fileobj=source.stdout, mode='r|') as original, \
                    tarfile.open(fileobj=compressor.stdin, mode='w|', format=tarfile.PAX_FORMAT) as result:
                for entry in original:
                    name = entry.name.rstrip('/')
                    if name in omitted:
                        continue
                    if entry.isdir():
                        continue
                    assert entry.isfile(), f'Review nonregular source: {name}'
                    assert not name.startswith('/') and '..' not in Path(name).parts
                    data = original.extractfile(entry).read()
                    assert not PRIVATE_KEY.search(data), f'Private key in source: {name}'
                    checksums.append((name, hashlib.sha256(data).hexdigest()))
                    item = tarfile.TarInfo(f'{PACKAGE}/{name}')
                    item.mode = 0o755 if entry.mode & 0o111 else 0o644
                    item.mtime = timestamp
                    item.size = len(data)
                    result.addfile(item, io.BytesIO(data))
                info = dict(package=PACKAGE, release='5.10.274', source_commit=commit,
                            kernel_tree=tree, source_date_epoch=timestamp,
                            files=len(checksums), archive_filters=omitted,
                            source_licenses='COPYING and LICENSES/; original per-file notices retained',
                            device_acceptance='Prior #101 secure and #103 relaxed tests; see README.md',
                            GKI_or_stable_KMI_certified=False, private_keys_distributed=False)
                data = (json.dumps(info, sort_keys=True, indent=2) + '\n').encode()
                add_bytes(result, 'SOURCE_INFO.json', data, timestamp)
                checksums.append(('SOURCE_INFO.json', hashlib.sha256(data).hexdigest()))
                sums = ''.join(f'{digest}  {name}\n' for name, digest in sorted(checksums)).encode()
                add_bytes(result, 'SHA256SUMS', sums, timestamp)
            source.stdout.close()
            assert source.wait() == 0, 'git archive failed'
            compressor.stdin.close()
            assert compressor.wait() == 0, 'xz compression failed'
        except BaseException:
            source.terminate()
            compressor.terminate()
            source.wait()
            compressor.wait()
            partial.unlink(missing_ok=True)
            raise
    partial.rename(target)
    with target.open('rb') as stream:
        digest = hashlib.file_digest(stream, 'sha256').hexdigest()
    checksum.write_text(f'{digest}  {target.name}\n')
    print(json.dumps(dict(archive=str(target), sha256=digest, source_commit=commit,
                          kernel_tree=tree, source_files=len(checksums) - 1,
                          omitted_files=len(omitted)), indent=2), flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--revision', default='HEAD')
    parser.add_argument('--output', type=Path, default=ROOT / 'dist')
    args = parser.parse_args()
    package(args.revision, args.output.resolve())
