#!/usr/bin/env python3
"""Test clean source/archive and relocated installed CMake consumer builds."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile
from check_release import ROOT, normalised_files, package_files


def run(*args):
    subprocess.run([str(x) for x in args], check=True)


def executable(build):
    name = 'superjupiter-record-journal-consumer'
    candidates = [build / name, build / (name + '.exe'), build / 'Release' / (name + '.exe')]
    return next(p for p in candidates if p.is_file())


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='superjupiter-record-journal-') as temporary:
        work = Path(temporary)
        source = ROOT
        if args.archive:
            source = work / 'package'
            for name, data in normalised_files(package_files(args.archive)).items():
                destination = source / name
                destination.parent.mkdir(parents=True, exist_ok=True)
                destination.write_bytes(data)
        consumer = source / 'tests/consumer'
        subproject = work / 'subproject'
        run('cmake', '-S', consumer, '-B', subproject, '-DSUPERJUPITER_RECORD_JOURNAL_SOURCE_DIR=' + str(source))
        run('cmake', '--build', subproject, '--config', 'Release')
        run(executable(subproject))
        build = work / 'library'
        prefix = work / 'installed'
        run('cmake', '-S', source, '-B', build, '-DSUPERJUPITER_RECORD_JOURNAL_BUILD_TESTS=OFF',
            '-DSUPERJUPITER_RECORD_JOURNAL_BUILD_EXAMPLES=OFF', '-DSUPERJUPITER_RECORD_JOURNAL_INSTALL=ON',
            '-DCMAKE_INSTALL_PREFIX=' + str(prefix))
        run('cmake', '--install', build, '--config', 'Release')
        relocated = work / 'relocated'
        shutil.move(str(prefix), relocated)
        installed = work / 'consumer'
        run('cmake', '-S', consumer, '-B', installed, '-DCMAKE_PREFIX_PATH=' + str(relocated))
        run('cmake', '--build', installed, '--config', 'Release')
        run(executable(installed))
    print('PASS: clean subproject and relocated find_package consumers')


if __name__ == '__main__':
    main()
