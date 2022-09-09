#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "board/board.hpp"
#include "magic/magic.hpp"
#include "move/move.hpp"
#include "move/move_gen.hpp"

int perft_helper(Board* board, int depth) {
  Move move_list[256];
  int n_moves, i;
  int nodes = 0;
  n_moves = board->get_move_list(move_list);
  if (n_moves < 0) {
    return 0;
  }
  if (depth == 0) return 1;
  for (i = 0; i < n_moves; i++) {
    Board next = board->do_move(&move_list[i]);
    if (!next.in_check()) nodes += perft_helper(&next, depth - 1);
  }
  return nodes;
}
int perft(Board board, int depth) {
  Move move_list[256];
  int i = 0;
  int n_moves = board.get_move_list(move_list);
  //   qsort(move_list, n_moves, sizeof(Move), cmp_move);
  int nodes = 0;
  while (i != n_moves) {
    Board pos = board.do_move(&move_list[i]);
    if (pos.in_check()) {
      i++;
      continue;
    }
    int curr_nodes = perft_helper(&pos, depth);
    char move_str[6];
    print_move(move_str, &move_list[i]);
    printf("%s", move_str);
    printf(": %d\n", curr_nodes);
    nodes += curr_nodes;

    i++;
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
    board.printBoard();
    Move move = board.move_from_str(move_str);
    board = board.do_move(&move);
    scanf("%s", move_str);
  }
  printf("%s", board.printBoard().c_str());
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
