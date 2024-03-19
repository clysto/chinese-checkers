#pragma once

#include <cstddef>
#include <cstdint>
#include <game.hpp>
#include <list>
#include <stdexcept>
#include <unordered_map>

#define TRANSPOSITION_TABLE_SIZE (1 << 22)
#define TRANSPOSITION_TABLE_MASK ((1 << 22) - 1)

struct TranspositionTableEntry {
  uint64_t hash;
  int value;
  int depth;
  HashFlag flag;
  Move bestMove;
};

class TranspositionTable {
 public:
  void put(const uint64_t hash, const TranspositionTableEntry& value);
  const TranspositionTableEntry& get(const uint64_t hash);
  bool exists(const uint64_t hash) const;
  void clear();

 private:
  TranspositionTableEntry items[TRANSPOSITION_TABLE_SIZE];
};