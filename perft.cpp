#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <utility>

#include "board/board.hpp"

struct Entry {
  uint32_t key;
  int depth;
  int nodes;
};

int SIZE = 1<<20;
Entry tt[1 << 20];


int perft_helper(Board* board, int depth) {
  auto *entry = &tt[board->key % SIZE];
  if( entry->key == board->key && entry->depth == depth) {
    return entry->nodes;
  }
  int nodes = 0;
  Move move_list[256];

  Move* curr = move_list;
  Move* end = board->getMoveList(move_list);

  if (end == nullptr) {
    return 0;
  }
  if (depth == 0) return 1;


  while (curr != end) {
    UndoMove undo = board->doMove(*curr);
    if (!board->inCheck()) nodes += perft_helper(board, depth - 1);
    board->undoMove(undo);
    
    curr++;
  }

  tt[board->key % SIZE] = {board->key, depth, nodes};

  return nodes;
}

int perft(Board board, int depth) {
  Move move_list[256];
  Move* start = move_list;
  Move* end = board.getMoveList(move_list);
  int nodes = 0;
  while (end != start) {
    std::string debug_move = to_string(*start);
    UndoMove undo = board.doMove(*start);
    if (board.inCheck()) {
      board.undoMove(undo);
      start++;
      continue;
    }
    int curr_nodes = perft_helper(&board, depth);
    board.undoMove(undo);
    std::string move_str = to_string(*start);
    printf("%s", move_str.c_str());
    printf(": %d\n", curr_nodes);
    nodes += curr_nodes;
    start++;
  }
  return nodes;
}

void perftIO() {
  char move_str[1001];
  printf("Enter \"fen position {fen} go perft {depth}\"\n");
  for (int i = 0; i < 2; ++i) {
    scanf("%s", move_str);
  }

  scanf("%[^m]", move_str);
  char* fen = move_str + 1;
  Board board = import_fen(fen);
  scanf("%s", move_str);
  scanf("%s", move_str);
  while (move_str[0] != 'g' || move_str[1] != 'o') {

    Move move = board.moveFromStr(move_str);
    board.doMove(move);
    scanf("%s", move_str);
  }
  printf("%s\n", board.toString().c_str());
  printf("print something random then depth.\n");
  scanf("%s", move_str);
  int depth;
  scanf("%d", &depth);
  clock_t t;
  t = clock();
  int nodes = perft(board, depth - 1);
  t = clock() - t;
  int nodes_per_second = (int)(nodes / ((double)t / CLOCKS_PER_SEC));
  printf("\nn ps: %d\ntotal nodes: %d\n", nodes_per_second, nodes);
  printf("total time: %f\n", ((double)t / CLOCKS_PER_SEC));
}
int main() {
    for(int i = 0; i < SIZE; i++) tt[i] = {1, -1,1};
  init_tables();
  perftIO();
}
