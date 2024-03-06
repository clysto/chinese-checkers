#pragma once

#include <game.hpp>
#include <cstdint>

#define TABLE_SIZE 1 << 23

enum HashFlag {
  HASH_EXACT,
  HASH_LOWERBOUND,
  HASH_UPPERBOUND,
};

struct TranspositionTableEntry {
  uint64_t hash;
  int value;
  int depth;
  HashFlag flag;
  Move bestMove;
};

class TranspositionTable {
 private:
  TranspositionTableEntry table[TABLE_SIZE];

 public:
  TranspositionTable();

  TranspositionTableEntry& get(uint64_t hash);
  void put(uint64_t hash, TranspositionTableEntry entry);
  bool exists(uint64_t hash);
};