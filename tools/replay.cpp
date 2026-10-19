#include <SuperJupiterRecordJournal.h>
using namespace superjupiter::record_journal;
int main() {
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

  Memory storage; // Demonstration RAM adapter only, not persistent hardware.
  RecordJournal<4, 8> journal(storage);
  uint8_t record[4] = {1, 2, 3, 4};
  journal.recover();
  journal.append(record);
  return 0;
}
