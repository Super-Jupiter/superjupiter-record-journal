# Contributing

Please open an issue with the smallest reproduction you can provide: library
version, board/compiler, configuration, exact values and timestamps, expected
flags and actual flags. Redact credentials and private data.

Before proposing a change:

- Read docs/CONVENTIONS.md and preserve the component namespace and registered
  package identity. Breaking API changes need a migration note and version bump.
- Run `python3 tools/check_release.py` and `python3 tools/verify_consumers.py`
  in addition to the host suite. Registry exports must include consumer tools.

- Run the host suite: `cmake -S . -B build`, `cmake --build build`, then
  `ctest --test-dir build --output-on-failure`.
- Add a focused regression case for any changed behaviour.
- Keep the core C++11-compatible, allocation-free and independent of Arduino I/O.
- Distinguish simulated results, successful compilation and physical measurements.
- Preserve licensing and attribution for any third-party contribution. Do not
  submit code or data you do not have permission to share.

AI-assisted changes are welcome when reviewed and tested by their contributor.
Do not present generated measurements or guessed hardware behaviour as evidence.

We aim for small, reusable tools. Sensor drivers, network stacks and application
backends should remain separate. Be constructive and respectful in discussions.
There is no guaranteed support response time or maintenance service level.
