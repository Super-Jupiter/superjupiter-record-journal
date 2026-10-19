# SuperJupiter RecordJournal API

Include `SuperJupiterRecordJournal.h`. All public types/functions are inside `superjupiter::record_journal`.

## Behaviour and limits

Storage must be preallocated to requiredBytes(), initially 0xff, with ordered durable sync and byte-rewrite support.
The core does not erase, format, wear-level, compact or operate raw flash. Use a suitable filesystem/FRAM adapter.
recover scans the contiguous committed prefix. A zero commit marker is an interrupted append; an erased marker ends the log.
A committed record with bad version, sequence, payload length or CRC is Corrupt and blocks appending.
append invalidates and syncs the next slot, writes and syncs content, then commits and syncs. Any failure requires recover before reuse.
Payload has fixed size. get verifies CRC again. CRC detects accidental damage, not tampering; a damaged commit marker may truncate recovery.
Single writer only, no rolling overwrite. Application owns retention, transport and backups. Hardware power-loss validation pending.

## Minimal complete usage

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

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-record-journal/blob/v0.1.0/src/SuperJupiterRecordJournal.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Sequential stale journal instances cannot overwrite a committed record; rerecovery required.

StaleState rejects a cached handle after another sequential writer committed newer data. Reopen with recover/load; this preflight is not a substitute for external writer exclusion. The version-1 storage byte format is unchanged.
