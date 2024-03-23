self.Module = {
  onRuntimeInitialized() {},
};

importScripts('chinesecheckers.js');
importScripts('game.js');

self.addEventListener('message', (event) => {
  let { state, time } = event.data;
  const gameState = new GameState(state);
  const bestMove = gameState.searchBestMove(time);
  self.postMessage(bestMove);
  gameState.delete();
});
