#ifdef HAVE_SPDLOG
#include <spdlog/spdlog.h>
#endif
#include <algorithm>
#include <constants.hpp>
#include <game.hpp>

inline int bitlen_u128(uint128_t u) {
  if (u == 0) {
    return 0;
  }
  uint64_t upper = u >> 64;
  if (upper) {
    return 128 - __builtin_clzll(upper);
  } else {
    return 64 - __builtin_clzll((uint64_t)u);
  }
}

#define INF INT_MAX
#define SECONDS_LATER(x) std::chrono::high_resolution_clock::now() + std::chrono::seconds(x)
#define NOW std::chrono::high_resolution_clock::now()
#define NULL_MOVE \
  Move { -1, -1 }

#define SCAN_REVERSE_START(x, v) \
  while (x) {                    \
    int pos_##v = bitlen_u128(x) - 1;

#define SCAN_REVERSE_END(x, v)    \
  x ^= ((uint128_t)1 << pos_##v); \
  }

inline bool operator<(const Move &a, const Move &b) {
  return PIECE_DISTANCES[a.dst] - PIECE_DISTANCES[a.src] < PIECE_DISTANCES[b.dst] - PIECE_DISTANCES[b.src];
}

inline bool operator==(const Move &a, const Move &b) { return a.src == b.src && a.dst == b.dst; }

// 全局置换表
cache::lru_cache<uint64_t, TranspositionTableEntry> HASH_TABLE(4 * 1024 * 1024);
// Killer 着法表
Move KILLER_TABLE[32][2];

GameState::GameState() : board{0, INITIAL_RED, INITIAL_GREEN}, turn(RED), round(1), zobristHash(0) { hash(); }

GameState::GameState(GameState const &gameState)
    : board{0, gameState.board[RED], gameState.board[GREEN]},
      turn(gameState.turn),
      round(gameState.round),
      zobristHash(gameState.zobristHash) {}

GameState::GameState(const std::string &state) : board{0, 0, 0}, turn(RED), round(10), zobristHash(0) {
  board[RED] = 0;
  board[GREEN] = 0;
  int p = 0;
  int i = 0;
  for (char c : state) {
    switch (c) {
      case '0':
        p++;
        break;
      case '1':
        board[RED] |= (uint128_t)1 << p++;
        break;
      case '2':
        board[GREEN] |= (uint128_t)1 << p++;
        break;
      case 'r':
        turn = RED;
        goto end;
      case 'g':
        turn = GREEN;
        goto end;
      default:
        break;
    }
    i++;
  }
end:
  hash();
  try {
    round = std::stoi(state.substr(i + 1));
  } catch (std::invalid_argument const &e) {
    round = 10;
  }
}

std::string GameState::toString() {
  std::string result;
  for (int i = 0; i < 81; i++) {
    if (board[RED] >> i & 1) {
      result += "1";
    } else if (board[GREEN] >> i & 1) {
      result += "2";
    } else {
      result += "0";
    }
    if (i % 9 == 8 && i != 80) {
      result += "/";
    }
  }
  result += turn == RED ? " r " : " g ";
  result += std::to_string(round);
  return result;
}

std::vector<int> GameState::getBoard() {
  std::vector<int> result;
  for (int i = 0; i < 81; i++) {
    if (board[RED] >> i & 1) {
      result.push_back(RED);
    } else if (board[GREEN] >> i & 1) {
      result.push_back(GREEN);
    } else {
      result.push_back(EMPTY);
    }
  }
  return result;
}

Color GameState::getTurn() const { return turn; }

std::vector<Move> GameState::legalMoves() {
  std::vector<Move> moves;
  uint128_t from = board[turn];
  SCAN_REVERSE_START(from, src)
  uint128_t to = ADJ_POSITIONS[pos_src] & ~(board[RED] | board[GREEN]);
  jumpMoves(pos_src, to);
  SCAN_REVERSE_START(to, dst)
  moves.push_back({pos_src, pos_dst});
  SCAN_REVERSE_END(to, dst)
  SCAN_REVERSE_END(from, src)
  return moves;
}

std::vector<Move> GameState::sortedLegalMoves(int depth, Move historyBestMove) {
  std::vector<Move> moves;
  int min_distance = INF, max_distance = -INF;
  int distance = 0, min_index = 0, max_index = 0;
  bool killer1Legal = false, killer2Legal = false;
  uint128_t from = board[turn];
  SCAN_REVERSE_START(from, src)
  uint128_t to = ADJ_POSITIONS[pos_src] & ~(board[RED] | board[GREEN]);
  jumpMoves(pos_src, to);
  SCAN_REVERSE_START(to, dst)
  distance = PIECE_DISTANCES[pos_dst] - PIECE_DISTANCES[pos_src];
  if (KILLER_TABLE[depth][0].src == pos_src && KILLER_TABLE[depth][0].dst == pos_dst) {
    killer1Legal = true;
  } else if (KILLER_TABLE[depth][1].src == pos_src && KILLER_TABLE[depth][1].dst == pos_dst) {
    killer2Legal = true;
  } else {
    // 跳过向后走两步及其以上的着法
    if (turn == GREEN && PIECE_DISTANCES[pos_dst] - PIECE_DISTANCES[pos_src] <= -2) {
    } else if (turn == RED && PIECE_DISTANCES[pos_src] - PIECE_DISTANCES[pos_dst] <= -2) {
    } else {
      if (distance > max_distance) {
        max_distance = distance;
        max_index = moves.size();
      }
      if (distance < min_distance) {
        min_distance = distance;
        min_index = moves.size();
      }
      moves.push_back({pos_src, pos_dst});
    }
  }
  SCAN_REVERSE_END(to, dst)
  SCAN_REVERSE_END(from, src)
  // 把最大距离的着法放到最后
  if (turn == GREEN) {
    std::iter_swap(moves.begin() + max_index, moves.end() - 1);
  } else {
    std::iter_swap(moves.begin() + min_index, moves.end() - 1);
  }
  // 把 Killer 着法放到最后面
  if (killer2Legal) {
    moves.push_back(KILLER_TABLE[depth][1]);
  }
  if (killer1Legal) {
    moves.push_back(KILLER_TABLE[depth][0]);
  }
  // 把历史最佳着法放到最后面
  if (historyBestMove.src >= 0) {
    moves.push_back(historyBestMove);
  }
  return moves;
}

void GameState::jumpMoves(int src, uint128_t &to) {
  uint128_t jumps = JUMP_POSITIONS[src].at(ADJ_POSITIONS[src] & (board[RED] | board[GREEN]));
  jumps &= ~(board[RED] | board[GREEN]);
  if ((jumps | to) == to) {
    return;
  }
  to |= jumps;
  SCAN_REVERSE_START(jumps, dst)
  jumpMoves(pos_dst, to);
  SCAN_REVERSE_END(jumps, dst)
}

void GameState::applyMove(Move move) {
  if (zobristHash != 0) {
    zobristHash ^= ZOBRIST_TABLE[move.src][turn];
    zobristHash ^= ZOBRIST_TABLE[move.dst][turn];
    zobristHash ^= 0xc503204d9e521ac5ULL;
  }
  board[turn] ^= (uint128_t)1 << move.src;
  board[turn] |= (uint128_t)1 << move.dst;
  turn = turn == Color::RED ? Color::GREEN : Color::RED;
  if (turn == RED) {
    round++;
  }
}

void GameState::undoMove(Move move) {
  turn = turn == Color::RED ? Color::GREEN : Color::RED;
  board[turn] ^= (uint128_t)1 << move.dst;
  board[turn] |= (uint128_t)1 << move.src;
  if (turn == RED) {
    round--;
  }
  if (zobristHash != 0) {
    zobristHash ^= ZOBRIST_TABLE[move.src][turn];
    zobristHash ^= ZOBRIST_TABLE[move.dst][turn];
    zobristHash ^= 0xc503204d9e521ac5ULL;
  }
}

int GameState::evaluate() {
  int redScore = 0;
  int greenScore = 0;
  int lastRed = INF;
  int lastGreen = INF;
  uint128_t red = board[RED];
  uint128_t green = board[GREEN];
  SCAN_REVERSE_START(red, src)
  if (PIECE_DISTANCES[80 - pos_src] < lastRed) {
    lastRed = PIECE_DISTANCES[80 - pos_src];
  }
  redScore += PIECE_SCORE_TABLE[80 - pos_src];
  SCAN_REVERSE_END(red, src)
  SCAN_REVERSE_START(green, src)
  if (PIECE_DISTANCES[pos_src] < lastGreen) {
    lastGreen = PIECE_DISTANCES[pos_src];
  }
  greenScore += PIECE_SCORE_TABLE[pos_src];
  SCAN_REVERSE_END(green, src)
  redScore -= 1 << std::max(0, 4 - lastRed);
  greenScore -= 1 << std::max(0, 4 - lastGreen);
  if (lastRed == 13) {
    redScore = 10000;
    greenScore = 0;
  }
  if (lastGreen == 13) {
    greenScore = 10000;
    redScore = 0;
  }
  return turn == Color::RED ? redScore - greenScore : greenScore - redScore;
}

uint64_t GameState::hash() {
  if (zobristHash == 0) {
    uint128_t red = board[RED];
    uint128_t green = board[GREEN];
    SCAN_REVERSE_START(red, src)
    zobristHash ^= ZOBRIST_TABLE[pos_src][RED];
    SCAN_REVERSE_END(red, src)
    SCAN_REVERSE_START(green, src)
    zobristHash ^= ZOBRIST_TABLE[pos_src][GREEN];
    SCAN_REVERSE_END(green, src)
    if (turn == GREEN) {
      zobristHash ^= 0xc503204d9e521ac5ULL;
    }
  }
  return zobristHash;
}

bool GameState::isGameOver() {
  return board[RED] == INITIAL_GREEN || board[GREEN] == INITIAL_RED;
}

Move GameState::searchBestMove(int timeLimit) {
  if (round <= 4) {
    // 开局库
    return OPENINGS[turn].at(board[turn]);
  }
  // 初始化 Killer 着法表
  for (int i = 0; i < 32; i++) {
    KILLER_TABLE[i][0] = NULL_MOVE;
    KILLER_TABLE[i][1] = NULL_MOVE;
  }
  int depth = 1, eval = -INF, bestEval = -INF;
  Move move = NULL_MOVE, bestMove = NULL_MOVE;
  auto deadline = SECONDS_LATER(timeLimit);
  while (depth < 100) {
    bestEval = eval;
    bestMove = move;
    eval = mtdf(*this, depth, eval, deadline, move);
    uint64_t h = hash();
#ifdef HAVE_SPDLOG
    spdlog::info("complete search depth: {}, score: {}, move: {} {}", depth, eval, move.src, move.dst);
#endif
    if (eval > 9999 || NOW >= deadline) {
      // 找到胜利着法
      break;
    }
    depth++;
  }
  if (eval > bestEval) {
    bestEval = eval;
    bestMove = move;
  }
#ifdef HAVE_SPDLOG
  spdlog::info("final eval: {}", bestEval);
#endif
  return bestMove;
}

int mtdf(GameState &gameState, int depth, int guess, time_point_t deadline, Move &bestMove) {
  int beta;
  int upperbound = INF;
  int lowerbound = -INF;
  int score = guess;
  do {
    beta = (score == lowerbound ? score + 1 : score);
    score = alphaBetaSearch(gameState, depth, beta - 1, beta, deadline, bestMove);
    (score < beta ? upperbound : lowerbound) = score;
  } while (lowerbound < upperbound);
  return score;
}

int alphaBetaSearch(GameState &gameState, int depth, int alpha, int beta, time_point_t deadline, Move &bestMove) {
  // 查询置换表
  uint64_t hash = gameState.hash();
  int alphaOrig = alpha;
  bestMove = NULL_MOVE;

  if (HASH_TABLE.exists(hash)) {
    auto result = HASH_TABLE.get(hash);
    if (result.depth >= depth) {
      if (result.flag == HASH_EXACT) {
        return result.value;
      } else if (result.flag == HASH_LOWERBOUND) {
        alpha = std::max(alpha, result.value);
      } else if (result.flag == HASH_UPPERBOUND) {
        beta = std::min(beta, result.value);
      }
      if (alpha >= beta) {
        return result.value;
      }
    }
    if (result.bestMove.src >= 0) {
      bestMove = result.bestMove;
    }
  }

  // 叶子结点
  if (gameState.isGameOver() || depth == 0) {
    return gameState.evaluate();
  }

  HashFlag flag;
  Move opponentMove, move;
  auto moves = gameState.sortedLegalMoves(depth, bestMove);
  int value = -INF;
  // reverse search moves
  for (auto it = moves.rbegin(); it != moves.rend(); it++) {
    move = *it;
    gameState.applyMove(move);
    int current = -alphaBetaSearch(gameState, depth - 1, -beta, -alpha, deadline, opponentMove);
    gameState.undoMove(move);
    if (current > value) {
      value = current;
      bestMove = move;
    }
    alpha = std::max(alpha, value);
    if (alpha >= beta) {
      // 发生 Beta 截断
      KILLER_TABLE[depth][1] = KILLER_TABLE[depth][0];
      KILLER_TABLE[depth][0] = move;
      break;
    }
    // 超时检测
    if (NOW >= deadline) {
      break;
    }
  }
  if (value <= alphaOrig) {
    flag = HASH_UPPERBOUND;
  } else if (value >= beta) {
    flag = HASH_LOWERBOUND;
  } else {
    flag = HASH_EXACT;
  }
  HASH_TABLE.put(hash, {hash, value, depth, flag, bestMove});
  return alpha;
}
