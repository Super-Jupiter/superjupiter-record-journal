#include <SuperJupiterRecordJournal.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <new>
static unsigned long checks = 0;
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    ++checks;                                                                                      \
    if (!(x)) {                                                                                    \
      std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x);                                    \
      std::abort();                                                                                \
    }                                                                                              \
  } while (false)
static bool near(double a, double b) { return std::fabs(a - b) <= 1e-9 * (1 + std::fabs(b)); }
using namespace superjupiter::record_journal;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  class Memory : public Storage {
  public:
    uint8_t bytes[256];
    int budget;
    bool failSync;
    int syncBudget = -1;
    bool failRead = false;
    Memory() : budget(-1), failSync(false) {
      for (size_t i = 0; i < sizeof(bytes); ++i)
        bytes[i] = 255;
    }
    bool read(size_t o, uint8_t *p, size_t n) {
      if (failRead)
        return false;
      if (o > sizeof(bytes) || n > sizeof(bytes) - o)
        return false;
      for (size_t i = 0; i < n; ++i)
        p[i] = bytes[o + i];
      return true;
    }
    bool write(size_t o, const uint8_t *p, size_t n) {
      if (o > sizeof(bytes) || n > sizeof(bytes) - o)
        return false;
      for (size_t i = 0; i < n; ++i) {
        if (budget == 0)
          return false;
        bytes[o + i] = p[i];
        if (budget > 0)
          --budget;
      }
      return true;
    }
    bool sync() {
      if (syncBudget == 0)
        return false;
      if (syncBudget > 0)
        --syncBudget;
      return !failSync;
    }
  };

  uint8_t value[4] = {1, 2, 3, 4}, out[4] = {};
  Memory m;
  RecordJournal<4, 2> j(m);
  CHECK(j.append(value) == Status::NotRecovered);
  CHECK(j.recover() == Status::Ok);
  CHECK(j.append(value) == Status::Ok);
  CHECK(j.get(0, out) == Status::Ok && out[3] == 4);
  RecordJournal<4, 2> reboot(m);
  CHECK(reboot.recover() == Status::Ok && reboot.size() == 1);
  CHECK(reboot.append(value) == Status::Ok);
  CHECK(reboot.append(value) == Status::Full);
  m.bytes[12] ^= 1;
  CHECK(reboot.get(0, out) == Status::Corrupt);
  CHECK(reboot.recover() == Status::Corrupt);
  for (int cut = 0; cut <= 22; ++cut) {
    Memory mem;
    RecordJournal<4, 3> log(mem);
    CHECK(log.recover() == Status::Ok);
    CHECK(log.append(value) == Status::Ok);
    mem.budget = cut;
    Status status = log.append(value);
    mem.budget = -1;
    RecordJournal<4, 3> restored(mem);
    CHECK(restored.recover() == Status::Ok);
    CHECK(restored.size() == 1 || restored.size() == 2);
    CHECK(restored.get(0, out) == Status::Ok && out[0] == 1);
    if (status == Status::Ok)
      CHECK(restored.size() == 2);
  }
  CHECK(checksum(reinterpret_cast<const uint8_t *>("123456789"), 9) == 0xcbf43926UL);

  Memory io;
  RecordJournal<4, 2> log(io);
  io.failRead = true;
  CHECK(log.recover() == Status::IoError);
  io.failRead = false;
  CHECK(log.recover() == Status::Ok);
  for (int failure = 0; failure < 3; ++failure) {
    Memory mem;
    RecordJournal<4, 3> store(mem);
    store.recover();
    store.append(value);
    mem.syncBudget = failure;
    CHECK(store.append(value) == Status::IoError);
    CHECK(store.append(value) == Status::NotRecovered);
    mem.syncBudget = -1;
    RecordJournal<4, 3> recovered(mem);
    CHECK(recovered.recover() == Status::Ok);
    CHECK(recovered.size() >= 1 && recovered.size() <= 2);
    CHECK(recovered.get(0, out) == Status::Ok && out[0] == 1);
  }

  std::printf("PASS SuperJupiterRecordJournal: %lu checks\n", checks);
}
