#pragma once

#include <map>
#include <vector>

using uint128_t = __uint128_t;
using time_point_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

enum Color {
  EMPTY,
  RED,
  GREEN,
};

enum HashFlag {
  HASH_EXACT,
  HASH_LOWERBOUND,
  HASH_UPPERBOUND,
};

struct Move {
  int src;
  int dst;
};

struct TranspositionTableEntry {
  int value;
  int depth;
  HashFlag flag;
  Move bestMove;
};

const uint128_t BOARD_MASK = ((uint128_t)0x1ffff << 64) | 0xffffffffffffffff;
const uint128_t INITIAL_RED = ((uint128_t)0x1e0e0 << 64) | 0x6020000000000000;
const uint128_t INITIAL_GREEN = 0x80c0e0f;
const int PIECE_DISTANCES[81] = {
    0, 1, 2,  3,  4,  5,  6,  7,  8,   // 0
    1, 2, 3,  4,  5,  6,  7,  8,  9,   // 1
    2, 3, 4,  5,  6,  7,  8,  9,  10,  // 2
    3, 4, 5,  6,  7,  8,  9,  10, 11,  // 3
    4, 5, 6,  7,  8,  9,  10, 11, 12,  // 4
    5, 6, 7,  8,  9,  10, 11, 12, 13,  // 5
    6, 7, 8,  9,  10, 11, 12, 13, 14,  // 6
    7, 8, 9,  10, 11, 12, 13, 14, 15,  // 7
    8, 9, 10, 11, 12, 13, 14, 15, 16,  // 8
};
const int PIECE_SCORE_TABLE[81] = {
    0,  4,  5,  12, 0,  0,  10, 10, 10,  // 0
    4,  6,  13, 16, 20, 21, 14, 11, 10,  // 1
    5,  13, 17, 21, 22, 24, 23, 20, 12,  // 2
    12, 16, 21, 23, 25, 26, 28, 27, 20,  // 3
    0,  20, 22, 25, 27, 29, 30, 32, 31,  // 4
    0,  21, 24, 26, 29, 31, 33, 34, 36,  // 5
    10, 14, 23, 28, 30, 33, 35, 36, 38,  // 6
    10, 11, 20, 27, 32, 34, 36, 38, 40,  // 7
    10, 10, 12, 20, 31, 36, 38, 40, 42,  // 8
};

const std::map<uint128_t, Move> OPENINGS[3] = {
    {},
    {
        {((uint128_t)0x000000000001e0e0 << 64) | 0x6020000000000000, {53, 52}},
        {((uint128_t)0x000000000001e0e0 << 64) | 0x6010000000000000, {71, 51}},
        {((uint128_t)0x000000000001e060 << 64) | 0x6018000000000000, {78, 42}},
        {((uint128_t)0x000000000001a060 << 64) | 0x6018040000000000, {61, 41}},
    },
    {
        {((uint128_t)0x0000000000000000 << 64) | 0x00000000080c0e0f, {27, 28}},
        {((uint128_t)0x0000000000000000 << 64) | 0x00000000100c0e0f, {9, 29}},
        {((uint128_t)0x0000000000000000 << 64) | 0x00000000300c0c0f, {2, 38}},
        {((uint128_t)0x0000000000000000 << 64) | 0x00000040300c0c0b, {19, 39}},
    }};

class GameState {
 private:
  uint128_t board[3];
  Color turn;
  int round;

 public:
  GameState();
  GameState(GameState const &gameState);
  explicit GameState(const std::string &state);
  std::vector<int> getBoard();
  Color getTurn() const;
  std::vector<Move> legalMoves();
  void jumpMoves(int src, uint128_t &to);
  void applyMove(Move move);
  int evaluate();
  bool isGameOver();
  Move searchBestMove(int timeLimit);
  std::string toString();
  uint64_t hash();
};

int mtdf(GameState &gameState, int depth, int guess, time_point_t deadline);
int alphaBetaSearch(GameState &gameState, int depth, int alpha, int beta, time_point_t deadline);
