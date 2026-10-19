// SPDX-License-Identifier: MIT
#include <SuperJupiterRecordJournal.h>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
static unsigned checks = 0;
#define CHECK(x)                                                               \
  do {                                                                         \
    ++checks;                                                                  \
    if (!(x)) {                                                                \
      std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #x);         \
      std::abort();                                                            \
    }                                                                          \
  } while (false)
static bool close(double a, double b) {
  return std::fabs(a - b) <= 1e-12 * (1 + std::fabs(b));
}
using namespace superjupiter::record_journal;

class Memory : public Storage {
public:
  uint8_t bytes[256];
  Memory() {
    for (size_t i = 0; i < sizeof(bytes); ++i)
      bytes[i] = 255;
  }
  bool read(size_t o, uint8_t *p, size_t n) {
    if (o > sizeof(bytes) || n > sizeof(bytes) - o)
      return false;
    for (size_t i = 0; i < n; ++i)
      p[i] = bytes[o + i];
    return true;
  }
  bool write(size_t o, const uint8_t *p, size_t n) {
    if (o > sizeof(bytes) || n > sizeof(bytes) - o)
      return false;
    for (size_t i = 0; i < n; ++i)
      bytes[o + i] = p[i];
    return true;
  }
  bool sync() { return true; }
};

int main() {
  (void)close;

  Memory storage;
  uint8_t first[4] = {1, 2, 3, 4}, other[4] = {9, 8, 7, 6}, out[4] = {};
  RecordJournal<4, 4> a(storage), b(storage);
  CHECK(a.recover() == Status::Ok && b.recover() == Status::Ok);
  CHECK(a.append(first) == Status::Ok);
  CHECK(b.append(other) != Status::Ok);
  RecordJournal<4, 4> restored(storage);
  CHECK(restored.recover() == Status::Ok && restored.size() == 1);
  CHECK(restored.get(0, out) == Status::Ok && out[0] == 1);

  std::printf("PASS SuperJupiterRecordJournal review: %u checks\n", checks);
}
