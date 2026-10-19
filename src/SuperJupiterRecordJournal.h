// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_RECORD_JOURNAL_H
#define SUPERJUPITER_RECORD_JOURNAL_H
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#define SUPERJUPITER_RECORD_JOURNAL_VERSION "0.1.0"
namespace superjupiter {
namespace record_journal {
// Storage adapters must bounds-check, report failures and implement real durability in sync().
class Storage {
public:
  virtual bool read(size_t offset, uint8_t *destination, size_t length) = 0;
  virtual bool write(size_t offset, const uint8_t *source, size_t length) = 0;
  virtual bool sync() = 0;
  virtual ~Storage() {}
};
inline uint32_t checksum(const uint8_t *bytes, size_t length) {
  uint32_t crc = 0xffffffffUL;
  for (size_t i = 0; i < length; ++i) {
    crc ^= bytes[i];
    for (unsigned j = 0; j < 8; ++j)
      crc = (crc >> 1) ^ ((crc & 1) ? 0xedb88320UL : 0);
  }
  return ~crc;
}
inline void put32(uint8_t *p, uint32_t value) {
  for (unsigned i = 0; i < 4; ++i)
    p[i] = static_cast<uint8_t>(value >> (8 * i));
}
inline uint32_t get32(const uint8_t *p) {
  uint32_t value = 0;
  for (unsigned i = 0; i < 4; ++i)
    value |= static_cast<uint32_t>(p[i]) << (8 * i);
  return value;
}

enum class Status { Ok, Full, NotRecovered, IoError, Corrupt, InvalidIndex };
template <size_t Payload, size_t Slots> class RecordJournal {
  static_assert(Payload > 0 && Payload <= 1024 && Slots > 0 && Slots <= 65535,
                "RecordJournal requires bounded positive sizes");
  static_assert(Slots <= SIZE_MAX / (Payload + 17), "RecordJournal storage size exceeds size_t");
  static const size_t Bytes = Payload + 17;
  Storage &storage_;
  size_t base_, count_;
  bool ready_;
  bool valid(const uint8_t *b, size_t slot) const {
    return b[0] == 0x53 && b[1] == 0x4a && b[2] == 1 && b[3] == 0 && get32(b + 4) == slot &&
           get32(b + 8) == Payload && get32(b + 12 + Payload) == checksum(b, 12 + Payload);
  }

public:
  explicit RecordJournal(Storage &storage, size_t base = 0)
      : storage_(storage), base_(base), count_(0), ready_(false) {}
  static size_t requiredBytes() { return Bytes * Slots; }
  size_t size() const { return count_; }
  Status recover() {
    ready_ = false;
    count_ = 0;
    if (base_ > SIZE_MAX - requiredBytes())
      return Status::IoError;
    uint8_t b[Bytes];
    for (size_t i = 0; i < Slots; ++i) {
      if (!storage_.read(base_ + i * Bytes, b, Bytes))
        return Status::IoError;
      if (b[Bytes - 1] == 0 || b[Bytes - 1] == 255) {
        ready_ = true;
        return Status::Ok;
      }
      if (b[Bytes - 1] != 0xa5 || !valid(b, i))
        return Status::Corrupt;
      ++count_;
    }
    ready_ = true;
    return Status::Ok;
  }
  Status append(const uint8_t *payload) {
    if (!ready_)
      return Status::NotRecovered;
    if (!payload)
      return Status::IoError;
    if (count_ == Slots)
      return Status::Full;
    uint8_t b[Bytes] = {};
    b[0] = 0x53;
    b[1] = 0x4a;
    b[2] = 1;
    put32(b + 4, static_cast<uint32_t>(count_));
    put32(b + 8, Payload);
    for (size_t i = 0; i < Payload; ++i)
      b[12 + i] = payload[i];
    put32(b + 12 + Payload, checksum(b, 12 + Payload));
    const size_t offset = base_ + count_ * Bytes;
    const uint8_t invalid = 0, commit = 0xa5;
    ready_ = false;
    if (!storage_.write(offset + Bytes - 1, &invalid, 1) || !storage_.sync() ||
        !storage_.write(offset, b, Bytes - 1) || !storage_.sync() ||
        !storage_.write(offset + Bytes - 1, &commit, 1) || !storage_.sync())
      return Status::IoError;
    ++count_;
    ready_ = true;
    return Status::Ok;
  }
  Status get(size_t index, uint8_t *payload) {
    if (!ready_)
      return Status::NotRecovered;
    if (index >= count_ || !payload)
      return Status::InvalidIndex;
    uint8_t b[Bytes];
    if (!storage_.read(base_ + index * Bytes, b, Bytes))
      return Status::IoError;
    if (b[Bytes - 1] != 0xa5 || !valid(b, index)) {
      ready_ = false;
      return Status::Corrupt;
    }
    for (size_t i = 0; i < Payload; ++i)
      payload[i] = b[12 + i];
    return Status::Ok;
  }
};
} // namespace record_journal
} // namespace superjupiter
#endif
