#ifdef HAVE_SPDLOG
#include <spdlog/spdlog.h>
#endif
#include <cache.hpp>
#include <constants.hpp>
#include <game.hpp>
#include <limits>
#include <string>

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

cache::lru_cache<uint64_t, TranspositionTableEntry> HASH_TABLE(1 << 20);

GameState::GameState() : board{0, INITIAL_RED, INITIAL_GREEN}, turn(RED), round(1) {}

GameState::GameState(GameState const &gameState)
    : board{0, gameState.board[RED], gameState.board[GREEN]}, turn(gameState.turn), round(gameState.round) {}

GameState::GameState(const std::string &state) : board{0, 0, 0}, turn(RED), round(10) {
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
  // sort moves
  std::sort(moves.begin(), moves.end(), [this](Move a, Move b) {
    return turn == RED
               ? PIECE_DISTANCES[a.dst] - PIECE_DISTANCES[a.src] < PIECE_DISTANCES[b.dst] - PIECE_DISTANCES[b.src]
               : PIECE_DISTANCES[a.dst] - PIECE_DISTANCES[a.src] > PIECE_DISTANCES[b.dst] - PIECE_DISTANCES[b.src];
  });
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
  board[turn] ^= (uint128_t)1 << move.src;
  board[turn] |= (uint128_t)1 << move.dst;
  turn = turn == Color::RED ? Color::GREEN : Color::RED;
  if (turn == RED) {
    round++;
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
  redScore -= 1 << std::max(0, 5 - lastRed);
  greenScore -= 1 << std::max(0, 5 - lastGreen);
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
  uint64_t hash = 0;
  for (int i = 0; i < 81; i++) {
    if (board[RED] >> i & 1) {
      hash ^= ZOBRIST_TABLE[i][RED];
    } else if (board[GREEN] >> i & 1) {
      hash ^= ZOBRIST_TABLE[i][GREEN];
    }
  }
  if (turn == GREEN) {
    hash ^= 0xc503204d9e521ac5ULL;
  }
  return hash;
}

bool GameState::isGameOver() {
  bool redWin = true;
  bool greenWin = true;
  uint128_t red = board[RED];
  uint128_t green = board[GREEN];
  SCAN_REVERSE_START(red, src)
  if (PIECE_DISTANCES[pos_src] > 3) {
    redWin = false;
  }
  SCAN_REVERSE_END(red, src)
  SCAN_REVERSE_START(green, src)
  if (PIECE_DISTANCES[pos_src] < 13) {
    greenWin = false;
  }
  SCAN_REVERSE_END(green, src)
  return redWin || greenWin;
}

Move GameState::searchBestMove(int timeLimit) {
  if (round <= 4) {
    // 开局库
    return OPENINGS[turn].at(board[turn]);
  }
  int depth = 1, eval = -INF, current;
  Move move = NULL_MOVE;
  auto deadline = SECONDS_LATER(timeLimit);
  while (depth < 100) {
    try {
      eval = mtdf(*this, depth, eval, deadline);
      move = HASH_TABLE.get(hash()).bestMove;
#ifdef HAVE_SPDLOG
      spdlog::info("complete search depth: {}, score: {}", depth, eval);
#endif
    } catch (std::runtime_error &e) {
#ifdef HAVE_SPDLOG
      spdlog::info("timeout search depth: {}", depth);
#endif
      break;
    }
    if (eval > 9999) {
      // 找到胜利着法
      break;
    }
    depth++;
  }
  return move;
}

int mtdf(GameState &gameState, int depth, int guess, time_point_t deadline) {
  int beta = guess;
  int upperbound = INF;
  int lowerbound = -INF;
  int score;
  do {
    beta = (score == lowerbound ? score + 1 : score);
    score = alphaBetaSearch(gameState, depth, beta - 1, beta, deadline);
    (score < beta ? upperbound : lowerbound) = score;
  } while (lowerbound < upperbound);
  return score;
}

int alphaBetaSearch(GameState &gameState, int depth, int alpha, int beta, time_point_t deadline) {
  // 查询置换表
  uint64_t hash = gameState.hash();
  int alphaOrig = alpha;
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
  }

  // 叶子结点
  if (gameState.isGameOver() || depth == 0) {
    return gameState.evaluate();
  }

  Move bestMove = NULL_MOVE;
  HashFlag flag = HASH_LOWERBOUND;
  int value = -INF, current;
  for (Move move : gameState.legalMoves()) {
    GameState newState(gameState);
    newState.applyMove(move);
    current = -alphaBetaSearch(newState, depth - 1, -beta, -alpha, deadline);
    if (current > value) {
      value = current;
      bestMove = move;
    }
    alpha = std::max(alpha, value);
    if (alpha >= beta) {
      // 发生截断
      break;
    }
    // 超时检测
    if (NOW >= deadline) {
      throw std::runtime_error("timeout");
    }
  }
  if (value <= alphaOrig) {
    flag = HASH_UPPERBOUND;
  } else if (value >= beta) {
    flag = HASH_LOWERBOUND;
  } else {
    flag = HASH_EXACT;
  }
  HASH_TABLE.put(hash, {value, depth, flag, bestMove});
  return alpha;
}
