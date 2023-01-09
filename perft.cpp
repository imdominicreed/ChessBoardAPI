#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "board/board.hpp"
#include "magic/magic.hpp"
#include "move/move.hpp"
#include "move/move_gen.hpp"

int perft_helper(Board* board, int depth) {
  int nodes = 0;
  Move move_list[256];

  Move* curr = move_list;
  Move* end = board->getMoveList(move_list);
  if (end == nullptr) {
    return 0;
  }
  if (depth == 0) return 1;
  while (curr != end) {
    Board next = board->doMove(*curr);
    if (!next.inCheck()) nodes += perft_helper(&next, depth - 1);
    curr++;
  }
  return nodes;
}

int perft(Board board, int depth) {
  Move move_list[256];
  Move* start = move_list;
  Move* end = board.getMoveList(move_list);
  int nodes = 0;
  while (end != start) {
    Board pos = board.doMove(*start);
    if (pos.inCheck()) {
      start++;
      continue;
    }
    int curr_nodes = perft_helper(&pos, depth);
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
    board = board.doMove(move);
    scanf("%s", move_str);
  }
  printf("print something random then depth.\n");
  scanf("%s", move_str);
  int depth;
  scanf("%d", &depth);
  clock_t t;
  t = clock();
  int nodes = perft(board, depth - 1);
  t = clock() - t;
  int nodes_per_second = (int)(nodes / ((double)t / CLOCKS_PER_SEC));
  printf("\nn ps: %d\ntotal nodes: %d", nodes_per_second, nodes);
}
int main() {
  init_tables();
  perftIO();
}
