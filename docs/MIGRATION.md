# Migration

0.1.0 is the initial evolving preview of SuperJupiter RecordJournal. No prior public API. Breaking changes require a new minor version and a migration note. Published tags are immutable.


## 0.1.1 compatibility review

Package, header, component namespace and CMake target names are unchanged. Existing tags and storage byte formats are preserved. Read statuses and numeric-validity flags before using derived values; handle additional diagnostic enum values in switches. TimeAggregate adds Summary.numericError; RecordJournal/ConfigStore add StaleState. Numeric-invalid observations retain chronology in ThresholdEvents, MachineState and Integration. Reopen stale storage handles under external writer exclusion. See INTEROPERABILITY.md and the release changelog for component-specific fixes. Hardware validation pending.
