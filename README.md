# SuperJupiter RecordJournal

<!-- SUPERJUPITER-CONSOLIDATION-MIGRATION:START -->
## Consolidated library available

For new projects, start with **[SuperJupiter Embedded Reliability](https://github.com/Super-Jupiter/superjupiter-embedded-reliability)**. This standalone package and its published releases remain available. Existing installations need no change.

If migrating with PlatformIO, replace the standalone dependency with:

```ini
lib_deps = superjupiter/SuperJupiterEmbeddedReliability@0.1.0
```

Replace this standalone include:

```cpp
#include <SuperJupiterRecordJournal.h>
```

with this consolidated include:

```cpp
#include <SuperJupiter/EmbeddedReliability/RecordJournal.h>
```

The component namespace and behaviour are preserved from standalone **0.1.1**. If using an earlier version, review its API migration requirements first. Use one implementation of this component per application; do not mix standalone and consolidated copies.

See the [consolidated migration guide](https://github.com/Super-Jupiter/superjupiter-embedded-reliability/blob/v0.1.0/docs/MIGRATION.md) and consolidated README for Arduino/CMake installation. **Hardware validation pending.**
<!-- SUPERJUPITER-CONSOLIDATION-MIGRATION:END -->

Append-only bounded records with CRC checks and interrupted-write recovery.

**Version 0.1.1: evolving preview. Hardware validation pending.**

Original MIT-licensed portable C++11 by [SuperJupiter](https://www.superjupiter.net).
No mandatory service, hidden clock reads, network calls or heap allocation in the core.

## What it does

Storage must be preallocated to requiredBytes(), initially 0xff, with ordered durable sync and byte-rewrite support.
The core does not erase, format, wear-level, compact or operate raw flash. Use a suitable filesystem/FRAM adapter.
recover scans the contiguous committed prefix. A zero commit marker is an interrupted append; an erased marker ends the log.
A committed record with bad version, sequence, payload length or CRC is Corrupt and blocks appending.
append invalidates and syncs the next slot, writes and syncs content, then commits and syncs. Any failure requires recover before reuse.
Payload has fixed size. get verifies CRC again. CRC detects accidental damage, not tampering; a damaged commit marker may truncate recovery.
Single writer only, no rolling overwrite. Application owns retention, transport and backups. Hardware power-loss validation pending.

## Install

PlatformIO exact release (after registry publication):

```ini
lib_deps = superjupiter/SuperJupiterRecordJournal@0.1.1
```

Arduino: use the versioned [GitHub release ZIP](https://github.com/Super-Jupiter/superjupiter-record-journal/releases/tag/v0.1.1).
Library Manager registration and index propagation are separate steps; check the official index before relying on IDE availability.

## Example

```cpp
#include <SuperJupiterRecordJournal.h>
using namespace superjupiter::record_journal;
int main() {
class Memory: public Storage {
public:
 uint8_t bytes[256];int budget;bool failSync;
 Memory():budget(-1),failSync(false) {for(size_t i=0;i<sizeof(bytes);++i) bytes[i]=255;}
 bool read(size_t o,uint8_t* p,size_t n) {if(o>sizeof(bytes)||n>sizeof(bytes)-o) return false;for(size_t i=0;i<n;++i) p[i]=bytes[o+i];return true;}
 bool write(size_t o,const uint8_t* p,size_t n) {if(o>sizeof(bytes)||n>sizeof(bytes)-o) return false;for(size_t i=0;i<n;++i) {if(budget==0) return false;bytes[o+i]=p[i];if(budget>0) --budget;}return true;}
 bool sync() {return !failSync;}
};

Memory storage; // Demonstration RAM adapter only, not persistent hardware.
RecordJournal<4,8> journal(storage);
uint8_t record[4]={1,2,3,4};
journal.recover();journal.append(record);
return 0;
}

```

For Arduino, open `examples/Minimal`. It uses synthetic data only.
Results are deliberately local so applications can inspect outcomes before adding I/O.

## CMake

```cmake
add_subdirectory(path/to/superjupiter-record-journal)
target_link_libraries(your_app PRIVATE SuperJupiter::RecordJournal)
```

For an installed package use `find_package(SuperJupiterRecordJournal 0.1 CONFIG REQUIRED)`.
Top-level CMake builds tests/examples; a subproject does not enable developer tools by default.

## Documentation and verification

- [API and boundaries](https://github.com/Super-Jupiter/superjupiter-record-journal/blob/v0.1.1/docs/API.md)
- [Tests](https://github.com/Super-Jupiter/superjupiter-record-journal/blob/v0.1.1/tests/test.cpp)
- [Validation status](https://github.com/Super-Jupiter/superjupiter-record-journal/blob/v0.1.1/docs/VALIDATION.md)
- [Shared conventions](https://github.com/Super-Jupiter/superjupiter-record-journal/blob/v0.1.1/docs/CONVENTIONS.md)
- [Migration policy](https://github.com/Super-Jupiter/superjupiter-record-journal/blob/v0.1.1/docs/MIGRATION.md)

```sh
cmake -S . -B build/cmake
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
python3 tools/check_release.py --tag v0.1.1
```

Keep raw observations and their provenance in your application. Derived results are not physical truth.
Do not use fast-math or finite-math-only compiler assumptions. AVR double may have float precision.
Software tests, board compilation and physical validation are reported separately.

## Fits the SuperJupiter measurement portfolio

Complements [SensorQuality](https://github.com/Super-Jupiter/superjupiter-sensor-quality) and
[SampleWindow](https://github.com/Super-Jupiter/superjupiter-sample-window) without a required dependency.
Each component can be used independently. Domain models, customer data and proprietary applications are excluded.

## Support and licence

MIT. Report reproducible bugs through repository issues; private security reports: info@superjupiter.net.
Include version, configuration, inputs, expected and observed behaviour. No credentials or private customer data.


## Compatibility review 0.1.1

This patch includes independent regression tests and a reviewed [interoperability contract](https://github.com/Super-Jupiter/superjupiter-record-journal/blob/v0.1.1/docs/INTEROPERABILITY.md). Hardware validation pending.

- Removed unused duplicated math helpers and unnecessary includes.
- Sequential stale journal instances cannot overwrite a committed record; rerecovery required.
