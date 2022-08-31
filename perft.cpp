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
  n_moves = get_move_list(board, move_list);
  if (n_moves < 0) n_moves = 0;
  if (depth == 0) return 1;
  for (i = 0; i < n_moves; i++) {
    Board next = do_move(&move_list[i], *board);
    if (!in_check(&next)) nodes += perft_helper(&next, depth - 1);
  }
  return nodes;
}
int perft(Board board, int depth) {
  Move move_list[256];
  int i = 0;
  int n_moves = get_move_list(&board, move_list);
  qsort(move_list, n_moves, sizeof(Move), cmp_move);
  int nodes = 0;
  while (i != n_moves) {
    Board pos = do_move(&move_list[i], board);
    if (in_check(&pos)) {
      i++;
      continue;
    }
    int curr_nodes = perft_helper(&pos, depth);

    char move_str[5];
    print_move(move_str, &move_list[i]);
    printf("%s", move_str);
    printf(": %d\n", curr_nodes);
    nodes += curr_nodes;

    i++;
  }
  return nodes;
}
void perftIO() {
  Board* board;
  char move_str[1001];
  printf("Enter \"fen position {fen} go perft {depth}\"\n");
  for (int i = 0; i < 2; ++i) {
    scanf("%s", move_str);
  }

  scanf("%[^m]", move_str);
  char* fen = move_str + 1;
  board = import_fen(fen);
  scanf("%s", move_str);
  scanf("%s", move_str);
  while (move_str[0] != 'g' || move_str[1] != 'o') {
    printBoard(board);
    Move move = move_from_str(*board, move_str);
    *board = do_move(&move, *board);
    scanf("%s", move_str);
  }
  printBoard(board);
  printf("print something random then depth.\n");
  scanf("%s", move_str);
  int depth;
  scanf("%d", &depth);
  clock_t t;
  t = clock();
  int nodes = perft(*board, depth - 1);
  t = clock() - t;
  int nodes_per_second = (int)(nodes / ((double)t / CLOCKS_PER_SEC));
  printf("total nodes: %d nodes per second: %d nps", nodes, nodes_per_second);
  free(board);
}
int main() {
  init_tables();
  perftIO();
}
