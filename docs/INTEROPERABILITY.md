# Interoperability contract

Each SuperJupiter component owns its namespace and can be used independently. No root aliases or mandatory cloud service.

- `SensorQuality` and `SampleWindow` accept scalar floats; other numerical components use double. Check finite/range status before narrowing a double. AVR double can have float precision.
- All timestamped components use caller-supplied uint32 milliseconds **except PulseMetrics, which uses microseconds**. Use one clock domain per pipeline and explicit conversion. Never feed UTC or microseconds into millisecond APIs.
- Consecutive accepted timestamps advance by 1..0x7fffffff ticks. Normal wrap is supported. A full wrap or arbitrary long outage cannot be inferred from a uint32 timestamp alone; reset/reanchor explicitly.
- One quality monitor/window/filter/trend per channel. Replay can emit different channels at the same time; this is not a duplicate observation within one channel.
- TimeAnchor is a forward, expiring mapping from one monotonic anchor to UTC, not a clock synchroniser. Store anchor revisions with data. Do not retroactively overwrite historical timestamps after reanchoring.
- Retain raw values/provenance separately. Calibration, filtering, trends, integration and summaries are derived information. Flags are evidence, not automatic consent to discard data or to control machinery.
- Check result status/validity before reading derived fields. SampleWindow uses hasStatistics/numericError; TimeAggregate exposes numericError. Zero fields on errors are not measurements.
- TimeAggregate has one completed-summary slot. Consume a Completed result with takeCompleted before the next push. Coverage is observation span, not proof of continuous measurement.
- Invalid numeric readings may break signal continuity while retaining observation chronology. Filter/Trend reject numeric inputs without advancing accepted-sample time. This is intentional: gate derived processing explicitly and use the raw/quality timeline for complete observation history.
- StoreForward is a RAM queue. Application-owned IDs must persist across reboot and be scoped by device/stream. Confirm acknowledgement before popping; receivers need ID-based deduplication. Default-constructible, copy-assignable payloads must themselves avoid allocation if allocation-free operation is required.
- RecordJournal/ConfigStore require distinct non-overlapping storage regions, an adapter that checks bounds, byte-rewrite support and genuinely durable ordered sync. They do not implement flash erase/wear levelling, authentication or encryption. Their unchanged version-1 byte layouts use little-endian fields and CRC32 for accidental damage only.
- Storage stale-handle checks prevent sequential overwrites, not races. One externally synchronised writer owns each region. After uncertain writes call recover/load and reconcile before retrying. CRC and commit markers are not protection against malicious changes or all corruption patterns.
- No component is inherently thread/ISR safe. Callers synchronise clocks, payloads, buffers and adapters. Caller-owned tables and replay arrays remain alive and unchanged.

Hardware validation pending. Synthetic tests and board compilation do not establish physical accuracy, power-loss durability, runtime stack headroom or field reliability.
