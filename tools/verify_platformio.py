#!/usr/bin/env python3
"""Compile a clean PlatformIO application using an archive or registry coordinate."""
import argparse
from pathlib import Path
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--package', required=True)
    args = parser.parse_args()
    package = args.package
    if Path(package).is_file():
        package = Path(package).resolve().as_posix()
    with tempfile.TemporaryDirectory(prefix='superjupiter-platformio-consumer-') as temporary:
        project = Path(temporary)
        (project / 'src').mkdir()
        (project / 'platformio.ini').write_text(
            '[env:consumer]\nplatform = espressif32@6.12.0\nboard = esp32dev\n'
            'framework = arduino\nlib_deps = ' + package + '\n', encoding='utf-8')
        (project / 'src/main.cpp').write_text('#include <Arduino.h>\n#include <SuperJupiterRecordJournal.h>\nusing namespace superjupiter::record_journal;\nvoid setup() {\n Serial.begin(115200);\nclass Memory: public Storage {\npublic:\n uint8_t bytes[4096];int budget;bool failSync;\n Memory():budget(-1),failSync(false) {for(size_t i=0;i<sizeof(bytes);++i) bytes[i]=255;}\n bool read(size_t o,uint8_t* p,size_t n) {if(o>sizeof(bytes)||n>sizeof(bytes)-o) return false;for(size_t i=0;i<n;++i) p[i]=bytes[o+i];return true;}\n bool write(size_t o,const uint8_t* p,size_t n) {if(o>sizeof(bytes)||n>sizeof(bytes)-o) return false;for(size_t i=0;i<n;++i) {if(budget==0) return false;bytes[o+i]=p[i];if(budget>0) --budget;}return true;}\n bool sync() {return !failSync;}\n};\n\nMemory storage; // Demonstration RAM adapter only, not persistent hardware.\nRecordJournal<4,8> journal(storage);\nuint8_t record[4]={1,2,3,4};\njournal.recover();journal.append(record);\n Serial.println("SuperJupiter RecordJournal example ran. Hardware validation pending.");\n}\nvoid loop() {}\n', encoding='utf-8')
        subprocess.run([sys.executable, '-m', 'platformio', 'run', '-d', str(project)], check=True)
    print('PASS: clean PlatformIO package consumer (compilation only)')


if __name__ == '__main__':
    main()
