#include <table.hpp>

TranspositionTable::TranspositionTable() { memset(table, 0, sizeof(table)); }

TranspositionTableEntry &TranspositionTable::get(uint64_t hash) { return table[hash % TABLE_SIZE]; }

void TranspositionTable::put(uint64_t hash, TranspositionTableEntry entry) { table[hash % TABLE_SIZE] = entry; }

bool TranspositionTable::exists(uint64_t hash) { return table[hash % TABLE_SIZE].hash == hash; }
