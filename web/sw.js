importScripts('chinesecheckers.js');
importScripts('game.js');

self.Module = {
  onRuntimeInitialized() {},
};

self.addEventListener('message', (event) => {
  let state = event.data;
  const gameState = new GameState(state);
  const bestMove = gameState.searchBestMove(5);
  self.postMessage(bestMove);
  gameState.delete();
});
