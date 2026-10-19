#!/usr/bin/env python3
"""Make an Arduino ZIP from an inspected PlatformIO package; never publish."""
import argparse
from pathlib import Path
import zipfile
from check_release import normalised_files, package_files


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    if args.output.exists():
        raise SystemExit('Output already exists; preserve it and choose a new path.')
    files = normalised_files(package_files(args.source))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(args.output, 'x', compression=zipfile.ZIP_DEFLATED) as archive:
        for name, data in sorted(files.items()):
            entry = zipfile.ZipInfo('SuperJupiterRecordJournal/' + name, date_time=(2026, 10, 19, 0, 0, 0))
            entry.compress_type = zipfile.ZIP_DEFLATED
            entry.external_attr = 0o100644 << 16
            archive.writestr(entry, data)
    print(args.output)


if __name__ == '__main__':
    main()
