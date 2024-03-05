const EMPTY = 0;
const RED = 1;
const GREEN = 2;

class GameState {
  constructor(str) {
    if (str) {
      this.state = new Module.GameState(str);
    } else {
      this.state = new Module.GameState();
    }
    this.cacheBoard();
  }
  legalMoves() {
    const moves = {};
    const legalMoves = this.state.legalMoves();
    for (let i = 0; i < legalMoves.size(); i++) {
      const { src, dst } = legalMoves.get(i);
      if (moves[src] === undefined) {
        moves[src] = new Set();
      }
      moves[src].add(dst);
    }
    legalMoves.delete();
    return moves;
  }
  applyMove(src, dst) {
    this.state.applyMove({ src, dst });
    this.cacheBoard();
  }
  cacheBoard() {
    const board = this.state.getBoard();
    this.boardCache = [];
    for (let i = 0; i < board.size(); i++) {
      this.boardCache.push(board.get(i));
    }
    board.delete();
  }
  toString() {
    return this.state.toString();
  }
  delete() {
    return this.state.delete();
  }
  searchBestMove(timeLimit) {
    return this.state.searchBestMove(timeLimit);
  }
  get board() {
    return this.boardCache;
  }
  get turn() {
    return this.state.turn.value;
  }
}
