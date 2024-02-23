function App() {
  const pieceClasses = ['empty', 'red', 'green'];
  // State
  let gameState = new GameState();
  let moves = gameState.legalMoves();
  let lastMove = null;
  let srcPiece = -1;
  let windowWidth = window.innerWidth;
  let myColor = RED;

  const worker = new Worker('sw.js');

  worker.onmessage = (event) => {
    const { src, dst } = event.data;
    gameState.applyMove(src, dst);
    lastMove = [src, dst];
    moves = gameState.legalMoves();
    m.redraw();
  };

  // Event handlers
  const handlePieceClick = (p) => {
    if (gameState.turn !== myColor) {
      return;
    }
    if (gameState.board[p] === gameState.turn) {
      srcPiece = p;
    } else if (moves[srcPiece] && moves[srcPiece].has(p)) {
      gameState.applyMove(srcPiece, p);
      lastMove = [srcPiece, p];
      srcPiece = -1;
      moves = gameState.legalMoves();
      computerMove();
    }
  };

  const computerMove = () => {
    worker.postMessage(gameState.toString());
  };

  const handleRestart = () => {
    gameState.delete();
    gameState = new GameState();
    moves = gameState.legalMoves();
    lastMove = null;
    srcPiece = -1;
    myColor = parseInt(document.getElementById('color-select').value);
    if (myColor === GREEN) {
      computerMove();
    }
  };

  const handleCopy = () => {
    const state = gameState.toString();
    navigator.clipboard.writeText(state);
  };

  window.addEventListener('resize', () => {
    windowWidth = window.innerWidth;
    m.redraw();
  });

  return {
    view() {
      return m('div', { id: 'app' }, [
        m('div', { class: 'game-area' }, [
          m(
            'svg',
            {
              class: classNames('board', {
                'red-turn': gameState.turn === RED,
                'green-turn': gameState.turn === GREEN,
              }),
              viewBox: windowWidth < 850 ? '80 0 460 683.5' : '0 0 620 683.5',
            },
            [
              m('path', {
                class: 'board-bg small',
                d: 'M92.993,341.769l197.321,341.769l39.372,0l197.321,-341.769l-197.321,-341.769l-39.372,0l-197.321,341.769Z',
              }),
              m('path', {
                class: 'board-bg big',
                d: 'M418.503,153.836l-88.817,-153.836l-39.372,-0l-88.817,153.836l-177.635,-0l-19.686,34.097l88.817,153.836l-88.817,153.836l19.686,34.097l177.635,0l88.817,153.836l39.372,0l88.817,-153.836l177.635,0l19.686,-34.097l-88.817,-153.836l88.817,-153.836l-19.686,-34.097l-177.635,-0Z',
              }),
              m('path', {
                class: 'circle fake-circles',
                d: FAKE_CIRCLES,
              }),
              (myColor === RED ? PIECE_POSITIONS : PIECE_POSITIONS_REVERSE).map(([cx, cy], p) =>
                m('circle', {
                  cx,
                  cy,
                  r: 19,
                  class: classNames('circle', pieceClasses[gameState.board[p]], {
                    bordered: srcPiece === p || lastMove?.includes(p),
                    path: moves[srcPiece] && moves[srcPiece].has(p),
                  }),
                  onclick: () => handlePieceClick(p),
                })
              ),
            ]
          ),
        ]),
        m('div', { class: 'control-area' }, [
          m('label', '我的颜色'),
          m('select', { id: 'color-select' }, [
            m('option', { value: RED }, '红色'),
            m('option', { value: GREEN }, '绿色'),
          ]),
          m('button', { onclick: handleRestart }, '新的游戏'),
          m('button', { onclick: handleCopy }, '复制棋盘'),
        ]),
      ]);
    },
  };
}

self.Module = {
  onRuntimeInitialized() {
    m.mount(document.body, App);
  },
};
