#include <constants.hpp>
#include <game.hpp>
#include <limits>
#include <string>

inline int bitlen_u128(uint128_t u) {
  int len = 0;
  while (u) {
    u >>= 1;
    len++;
  }
  return len;
}

#define SCAN_REVERSE_START(x, v) \
  while (x) {                    \
    int pos_##v = bitlen_u128(x) - 1;

#define SCAN_REVERSE_END(x, v)    \
  x ^= ((uint128_t)1 << pos_##v); \
  }

GameState::GameState() {
  board[RED] = INITIAL_RED;
  board[GREEN] = INITIAL_GREEN;
  turn = RED;
  round = 1;
}

GameState::GameState(GameState const &gameState) {
  board[RED] = gameState.board[RED];
  board[GREEN] = gameState.board[GREEN];
  turn = gameState.turn;
}

GameState::GameState(std::string state) {
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
    uint128_t mask = (uint128_t)1 << i;
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
    uint128_t mask = (uint128_t)1 << i;
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

double GameState::evaluate(int maxiumColor) {
  double redScore = 0;
  double greenScore = 0;
  int lastRed = 100;
  int lastGreen = 100;
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
  redScore -= pow(2, 5 - lastRed);
  greenScore -= pow(2, 5 - lastGreen);
  if (lastRed == 13) {
    redScore += 10000;
  }
  if (lastGreen == 13) {
    greenScore += 10000;
  }
  return maxiumColor == Color::RED ? redScore - 0.7 * greenScore : greenScore - 0.7 * redScore;
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

Move GameState::searchBestMove(int depth) {
  if (round <= 4) {
    return OPENINGS[turn].at(board[turn]);
  };
  Move besetMove =
      maxValue(*this, depth, -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), turn)
          .first;
  return besetMove;
}

std::pair<Move, double> maxValue(GameState &gameState, int depth, double alpha, double beta, int maxiumColor) {
  if (gameState.isGameOver() || depth == 0) {
    return {{Move{-1, -1}}, (depth + 1) * gameState.evaluate(maxiumColor)};
  }
  double maxEval = -std::numeric_limits<double>::infinity();
  Move bestMove;
  for (Move move : gameState.legalMoves()) {
    GameState newState(gameState);
    newState.applyMove(move);
    double eval = minValue(newState, depth - 1, alpha, beta, maxiumColor).second;
    if (eval > maxEval) {
      maxEval = eval;
      bestMove = move;
    }
    alpha = std::max(alpha, eval);
    if (beta <= alpha) {
      break;  // Beta cut-off
    }
  }
  return {bestMove, maxEval};
}

std::pair<Move, double> minValue(GameState &gameState, int depth, double alpha, double beta, int maxiumColor) {
  if (gameState.isGameOver() || depth == 0) {
    return {Move{-1, -1}, (depth + 1) * gameState.evaluate(maxiumColor)};
  }
  double minEval = std::numeric_limits<double>::infinity();
  Move bestMove;
  for (Move move : gameState.legalMoves()) {
    GameState newState(gameState);
    newState.applyMove(move);
    double eval = maxValue(newState, depth - 1, alpha, beta, maxiumColor).second;
    if (eval < minEval) {
      minEval = eval;
      bestMove = move;
    }
    beta = std::min(beta, eval);
    if (beta <= alpha) {
      break;  // Alpha cut-off
    }
  }
  return {bestMove, minEval};
}
