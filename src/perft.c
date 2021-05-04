
#include "board.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

long long Perfthelper(struct Board *board, int depth)
{

    struct Move move_list[256];
    long long n_moves, i;
    long long nodes = 0;
    n_moves = getMoveList(board, move_list);
    if (n_moves == 0)
        return 0;
    if (depth == 0)
        return 1ULL;

    for (i = 0; i < n_moves; i++) {
        struct Board next = doMove(&move_list[i], *board);
        nodes += Perfthelper(&next, depth - 1);
    }
    return nodes;
}
long long perft(struct Board board, int depth) {
    printBoard(&board);
    struct Move move_list[256];
    int i = 0;
    int n_moves = getMoveList(&board, move_list);
    qsort(move_list, n_moves, sizeof(struct Move), cmpMove);
    int nodes = 0;
    while (i != n_moves) {
        struct Board pos = doMove(&move_list[i], board);
        printMove(&move_list[i]);
        int curr_nodes = Perfthelper(&pos, depth);
        nodes+= curr_nodes;
        printf(": %d\n", curr_nodes);
        i++;
    }
    return nodes;
}
void perftIO() {
    struct Board board;
    startBoard(&board);
    char move_str[1001];
    for (int i = 0; i < 4; ++i) {

        scanf("%s", move_str);
    }
    while (strcmp(move_str, "go")) {
        struct Move move = moveFromString(move_str);
        board = doMove(&move, board);
        printBoard(&board);
        scanf("%s", move_str);
    }
    scanf("%s", move_str);
    int depth;
    scanf("%d", &depth);
    clock_t t;
    t = clock();
    long long nodes = perft(board, depth-1);
    t =  clock()-t;
    double nodes_per_second = nodes / (((double) t) / CLOCKS_PER_SEC);
    printf("total nodes: %u nodes per second: %f", nodes, nodes_per_second);

}
int main() {
    perftIO();
}


