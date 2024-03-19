#include <cstring>
#include <transtable.hpp>

void TranspositionTable::put(const uint64_t hash, const TranspositionTableEntry& value) {
  if (exists(hash) && items[hash & TRANSPOSITION_TABLE_MASK].depth > value.depth) {
    return;
  }
  items[hash & TRANSPOSITION_TABLE_MASK] = value;
}

const TranspositionTableEntry& TranspositionTable::get(const uint64_t hash) {
  return items[hash & TRANSPOSITION_TABLE_MASK];
}

bool TranspositionTable::exists(const uint64_t hash) const {
  return items[hash & TRANSPOSITION_TABLE_MASK].hash == hash;
}

void TranspositionTable::clear() { memset(items, 0, sizeof(items)); }
