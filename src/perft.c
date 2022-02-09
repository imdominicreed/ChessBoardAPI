
#include "board.h"
#include "move_gen.h"
#include "move.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

int perft_helper(Board *board, int depth)
{

    Move move_list[256];
    int n_moves, i;
    int nodes = 0;
    n_moves = get_move_list(board, move_list);
    if (n_moves == 0)
        return 0;
    if (depth == 0 )
        return 1ULL;

    for (i = 0; i < n_moves; i++) {
        Board next = do_move(&move_list[i], *board);
        nodes += perft_helper(&next, depth - 1);
    }
    return nodes;
}
int perft(Board board, int depth) {
    printBoard(&board);
    Move move_list[256];
    int i = 0;
    int n_moves = get_move_list(&board, move_list);
    qsort(move_list, n_moves, sizeof(Move), cmp_move);
    int nodes = 0;
    while (i != n_moves) {
        Board pos = do_move(&move_list[i], board);
        int curr_nodes = perft_helper(&pos, depth);
        if (curr_nodes != 0) {
            char move_str[5];
            print_move(move_str, &move_list[i]);
            printf("%s", move_str);
            printf(": %d\n", curr_nodes);
            nodes+= curr_nodes;
        }
        i++;
    }
    return nodes;
}
void perftIO() {
    Board board;
    start_board(&board);
    char move_str[1001];
    printf("Print 3 random things, Then moves. End with go.\n");
    for (int i = 0; i < 4; ++i) {
        scanf("%s", move_str);
    }
    while (strcmp(move_str, "go")) {
        Move move = move_from_str(board, move_str);
        board = do_move(&move, board);
        printBoard(&board);
        scanf("%s", move_str);
    }
    printf("print something random then depth.\n");
    scanf("%s", move_str);
    int depth;
    scanf("%d", &depth);
    clock_t t;
    t = clock();
    long long nodes = perft(board, depth-1);
    t =  clock()-t;
    double nodes_per_second = nodes / (((double) t) / CLOCKS_PER_SEC);
    printf("total nodes: %d nodes per second: %f", nodes, nodes_per_second);

}
int main() {
    perftIO();
}


