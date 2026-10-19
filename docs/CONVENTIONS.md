# SuperJupiter library conventions

This library follows the conventions established by SuperJupiter SensorQuality 0.2.0.
These conventions describe this independently installable component. A template generator is not required to use it.

| Surface | Convention | This library |
| --- | --- | --- |
| Brand | Always full SuperJupiter name | SuperJupiter RecordJournal |
| Repository | `superjupiter-<component-name>` | `superjupiter-record-journal` |
| Package | `SuperJupiter<Component>` | `SuperJupiterRecordJournal` |
| Public header | Branded `.h` entry point | `SuperJupiterRecordJournal.h` |
| C++ | Isolated component namespace | `superjupiter::record_journal` |
| CMake | Namespaced consumer target | `SuperJupiter::RecordJournal` |
| API | PascalCase types, lowerCamelCase members | `RecordJournal`, `sampledAtMs` |
| Units | Explicit field suffixes, documented value units | `Ms`, `PerSecond` |
| Version | Immutable `vMAJOR.MINOR.PATCH` tags | `v0.1.0` |
| Evidence | Separate host tests, board compilation and physical tests | Hardware pending |

## Design and compatibility

Keep a narrow, portable C++11 core, bounded state, no hidden clock reads, no heap
allocation and no mandatory service dependency. Caller-owned configuration,
units, clocks and synchronisation must be explicit. Do not import proprietary
application code, customer datasets or private repository history. Preserve
third-party licences if integrations are added later.

Do not put generic public types in the root `superjupiter` namespace. Use C++11
nested namespace syntax rather than requiring a newer language version. Header
guards and CMake options must include the full component identifier. Do not put
`using namespace` directives in public headers. Sensor drivers, networking,
storage adapters and application domain models remain outside this core.

0.x is an explicitly evolving preview. Document breaking migrations and increase
the minor version. Patches preserve the documented API. At 1.0, adopt normal
major-version compatibility guarantees. Do not rename an indexed package or
retag a published release. `@0.1.0` is exact; `@^0.1.0` permits compatible updates.

## Build and installation

`add_subdirectory` consumes `SuperJupiter::RecordJournal` without developer tools
or install rules by default. Explicitly enable any of these options if needed:

- `SUPERJUPITER_RECORD_JOURNAL_BUILD_TESTS`
- `SUPERJUPITER_RECORD_JOURNAL_BUILD_EXAMPLES`
- `SUPERJUPITER_RECORD_JOURNAL_INSTALL`

A top-level build defaults all three ON. `cmake --install build --prefix <path>`
installs a relocatable header-only CMake package. An application uses:

```cmake
find_package(SuperJupiterRecordJournal 0.1 CONFIG REQUIRED)
target_link_libraries(your_application PRIVATE SuperJupiter::RecordJournal)
```

The package requires CMake 3.16+ and C++11. Do not use fast-math, finite-math-only
or equivalent assumptions. Such options can invalidate NaN/Inf detection. AVR
`double` may have float precision; host double precision is not a board benchmark.

## Release checklist

1. Update both manifests, the CMake version, public version macro, changelog,
   current installation examples and validation record together.
2. Run `python3 tools/check_release.py --tag v0.1.0`. This validates consistency;
   it does not create a tag or publish anything.
3. Run strict C++11 GCC/Clang tests with sanitizers, namespace isolation, CMake
   subproject/relocated install consumers, fast-math rejection and the board matrix.
4. Run registered-library lint with `--library-manager update`. Use `submit`
   only before a new library has been added to the official index. CI reads the
   actual official index to choose the appropriate mode and fails on fetch errors.
5. Run `pio pkg pack -o build/SuperJupiterRecordJournal-0.1.0.tar.gz` and check it
   with `python3 tools/check_release.py --archive <file>`. Test archive consumers
   with `python3 tools/verify_consumers.py --archive <file>` and PlatformIO.
6. Review the diff and CI. Run the version checker with `--require-clean` after
   committing. Create the version tag once, on the reviewed commit, and publish
   only the inspected package. Publishing credentials stay outside the repository.
7. Verify the actual public release download, PlatformIO exact-version install
   and Arduino index/archive after propagation. Record checksum, commit, tool
   versions and separate evidence types. Acceptance is not propagation.
8. Preserve hardware-pending wording until real observations justify a precise
   validation update. Keep support issues distinct from package availability.

Pinned CI actions and monthly Dependabot proposals are reviewed, not automatically
merged. Bug reports need version/configuration/input/expected/actual behaviour;
security reports use the private contact in SECURITY.md.
