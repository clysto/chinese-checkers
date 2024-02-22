function App() {
  const pieceClasses = ['empty', 'red', 'green'];
  // State
  let gameState = new GameState();
  let moves = gameState.legalMoves();
  let lastMove = null;
  let srcPiece = -1;

  // Event handlers
  const handlePieceClick = (p) => {
    if (gameState.board[p] === gameState.turn) {
      srcPiece = p;
    } else if (moves[srcPiece] && moves[srcPiece].has(p)) {
      gameState.applyMove(srcPiece, p);
      lastMove = [srcPiece, p];
      srcPiece = -1;
      moves = gameState.legalMoves();
      fetch(
        'http://localhost:1234/searchbestmove?' +
          new URLSearchParams({
            state: gameState.toString(),
            depth: '5',
          })
      ).then((res) => {
        res.text().then((bestMove) => {
          const [src, dst] = bestMove.split(' ').map(Number);
          gameState.applyMove(src, dst);
          lastMove = [src, dst];
          moves = gameState.legalMoves();
          m.redraw();
        });
      });
    }
  };

  const handleRestart = () => {
    gameState.delete();
    gameState = new GameState();
    moves = gameState.legalMoves();
    lastMove = null;
    srcPiece = -1;
  };

  return {
    view() {
      return m('div', { id: 'app' }, [
        m('div', { class: 'game-area' }, [
          m('div', { class: 'board-wrapper' }, [
            m(
              'svg',
              {
                class: classNames('board', {
                  'red-turn': gameState.turn === RED,
                  'green-turn': gameState.turn === GREEN,
                }),
                viewBox: '0 0 620 683.5',
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
                PIECE_POSITIONS.map(([cx, cy], p) =>
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
        ]),
        m('div', { class: 'control-area' }, [m('button', { onclick: handleRestart }, 'New Game')]),
      ]);
    },
  };
}

self.Module = {
  onRuntimeInitialized() {
    m.mount(document.body, App);
  },
};
