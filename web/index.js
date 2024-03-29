function saveState(board, myColor, lastMove) {
  localStorage.setItem('board', board);
  localStorage.setItem('color', myColor);
  localStorage.setItem('lastMove', JSON.stringify(lastMove));
}

function restoreState() {
  const board = localStorage.getItem('board');
  if (!board) {
    return null;
  }
  const color = parseInt(localStorage.getItem('color'));
  const lastMove = JSON.parse(localStorage.getItem('lastMove'));
  const computerThinkTime = parseInt(localStorage.getItem('mode'));
  return { board, color, lastMove, computerThinkTime };
}

function App() {
  const savedState = restoreState();
  const pieceClasses = ['empty', 'red', 'green'];
  // State
  let gameState = savedState ? new GameState(savedState.board) : new GameState();
  let moves = gameState.legalMoves();
  let lastMove = savedState ? savedState.lastMove : null;
  let srcPiece = -1;
  let windowWidth = window.innerWidth;
  let myColor = savedState ? savedState.color : RED;
  let showNumber = false;
  let computerThinkTime = savedState ? savedState.computerThinkTime : 10;
  let showDialog = false;
  let countDown = 0;
  let gameId = 0;
  let myLastMove = null;
  // Bind value
  let userSelectColor = RED;
  let userSelectMode = 10;

  const worker = new Worker('worker.js');

  // Init
  if (gameState.turn != myColor) {
    countDown = computerThinkTime;
    // 等待 worker 初始化
    setTimeout(() => computerMove(), 200);
  }

  // Event handlers
  const handlePieceClick = (p) => {
    if (gameState.isGameOver()) {
      return;
    }
    if (gameState.turn !== myColor) {
      return;
    }
    if (gameState.board[p] === gameState.turn) {
      srcPiece = p;
    } else if (moves[srcPiece] && moves[srcPiece].has(p)) {
      gameState.applyMove(srcPiece, p);
      myLastMove = [srcPiece, p];
      lastMove = [srcPiece, p];
      srcPiece = -1;
      moves = gameState.legalMoves();
      saveState(gameState.toString(), myColor, lastMove);
      computerMove();
    }
  };

  const computerMove = () => {
    if (gameState.isGameOver()) {
      return;
    }
    const sessionId = gameId;
    countDown = computerThinkTime;
    const t = setInterval(() => {
      if (countDown === 0) {
        clearInterval(t);
        return;
      }
      countDown--;
      m.redraw();
    }, 1000);
    worker.postMessage({ state: gameState.toString(), time: computerThinkTime });
    worker.onmessage = (e) => {
      if (gameId !== sessionId) {
        return;
      }
      const { src, dst } = e.data;
      gameState.applyMove(src, dst);
      lastMove = [src, dst];
      moves = gameState.legalMoves();
      saveState(gameState.toString(), myColor, lastMove);
      m.redraw();
    };
  };

  const handleRestart = () => {
    gameState.delete();
    gameState = new GameState();
    gameId++;
    moves = gameState.legalMoves();
    lastMove = null;
    srcPiece = -1;
    countDown = 0;
    myColor = parseInt(userSelectColor);
    localStorage.setItem('color', myColor);
    computerThinkTime = parseInt(userSelectMode);
    localStorage.setItem('mode', computerThinkTime);
    if (myColor === GREEN) {
      computerMove();
    }
    showDialog = false;
    saveState(gameState.toString(), myColor, lastMove);
  };

  const handleDialogToggle = () => {
    if (!showDialog) {
      userSelectColor = myColor;
      userSelectMode = computerThinkTime;
    }
    showDialog = !showDialog;
  };

  const handleCopy = () => {
    const state = gameState.toString();
    navigator.clipboard.writeText(state);
  };

  const handleShowNumber = (event) => {
    showNumber = !showNumber;
  };

  const handleUndoMove = () => {
    if (myLastMove && gameState.turn === myColor) {
      let [src, dst] = lastMove;
      gameState.undoMove(src, dst);
      [src, dst] = myLastMove;
      gameState.undoMove(src, dst);
      moves = gameState.legalMoves();
      lastMove = null;
      srcPiece = -1;
      myLastMove = null;
      m.redraw();
    }
  };

  window.addEventListener('resize', () => {
    windowWidth = window.innerWidth;
    m.redraw();
  });

  return {
    view() {
      return m('div', { id: 'app' }, [
        m('div', { class: 'info-area' }, [
          m(
            'span',
            gameState.isGameOver()
              ? [m('i.bi.bi-trophy-fill'), '游戏结束']
              : gameState.turn == myColor
              ? [m('i.bi.bi-joystick'), '你的回合']
              : [m('i.bi.bi-hourglass-split'), '电脑思考中：' + countDown]
          ),
        ]),
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
                m('g', { class: 'circle-group', onclick: () => handlePieceClick(p) }, [
                  m('circle', {
                    cx,
                    cy,
                    r: 19,
                    class: classNames('circle', pieceClasses[gameState.board[p]], {
                      bordered: srcPiece === p || lastMove?.includes(p),
                      path: moves[srcPiece] && moves[srcPiece].has(p),
                    }),
                  }),
                  showNumber && m('text', { x: cx, y: cy, class: 'piece-label' }, p),
                ])
              ),
            ]
          ),
        ]),
        m('div', { class: 'control-area' }, [
          m('div.button-group', [
            m('div.button', { onclick: handleDialogToggle }, [m('i.bi.bi-dice-5-fill'), '新游戏']),
            m('div.line'),
            m('div.button', { onclick: handleUndoMove }, [m('i.bi.bi-arrow-left-square-fill'), '悔棋']),
            m('div.line'),
            m('div.button.copy-board', { onclick: handleCopy }, [m('i.bi.bi-clipboard-check-fill'), '复制棋盘']),
            m('div.line.copy-board'),
            m('div.button', { onclick: handleShowNumber }, [
              m('i.bi.bi-1-circle-fill'),
              showNumber ? '隐藏编号' : '显示编号',
            ]),
          ]),
        ]),
        m(
          'div',
          {
            class: classNames('dialog-wrapper', {
              show: showDialog,
            }),
          },
          [
            m('div.dialog', [
              m('div.body', [
                m('p.title', '新的游戏'),
                m('label', { for: 'color-select' }, '我的颜色'),
                m(
                  'select',
                  { value: userSelectColor, id: 'color-select', onchange: (e) => (userSelectColor = e.target.value) },
                  [m('option', { value: RED }, '红色'), m('option', { value: GREEN }, '绿色')]
                ),
                m('p.hint', '红色先走，绿色后走。'),
                m('label', { for: 'mode-select' }, '对战难度'),
                m(
                  'select',
                  { value: userSelectMode, id: 'mode-select', onchange: (e) => (userSelectMode = e.target.value) },
                  [
                    m('option', { value: 5 }, '简单 (电脑思考时间: 5s)'),
                    m('option', { value: 10 }, '普通 (电脑思考时间: 10s)'),
                    m('option', { value: 15 }, '困难 (电脑思考时间: 15s)'),
                  ]
                ),
              ]),
              m('div.footer', [
                m('button', { onclick: handleDialogToggle }, '取消'),
                m('button.primary', { onclick: handleRestart }, '确定'),
              ]),
            ]),
          ]
        ),
      ]);
    },
  };
}

self.Module = {
  onRuntimeInitialized() {
    m.mount(document.body, App);
  },
};
