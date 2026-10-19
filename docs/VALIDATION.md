# Validation

## Release 0.1.0

Hardware validation pending. Host tests and board compilation do not validate sensors, timing accuracy, electrical behaviour, power loss or field reliability.
This evolving preview is not a certified safety component.

Reproduce the numerical/state/boundary/failure cases with `tests/test.cpp` under strict C++11 and sanitizers.
CI also checks namespace isolation, rejects fast-math, validates metadata and archive contents,
tests source and relocated CMake consumers, and compiles the synthetic example on ESP32, ESP32-S3, ESP32-C3 and Arduino Uno.
The GitHub Actions run for the exact release commit is the authoritative per-platform result, not this intended matrix.

The public tests are the exact executable cases for this version. Tests are illustrative evidence, not exhaustive correctness proof.
Adapters require separate application-specific failure injection and physical validation.


## Release 0.1.1

Hardware validation pending. Independent review regressions run under GCC/Clang sanitizers, source/archive/relocated CMake consumers and the four-board compilation matrix. The exact release commit CI and release receipts are authoritative; a historical failure is not a current passing result. No physical device, sensor or storage power-loss test is claimed.
