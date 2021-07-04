//
// Created by Dominic Reed on 4/17/21.
//
#include "engine.h"
#include "board.h"

int check_if_legal(struct Move move, struct Board board) {
    struct Move move_list[128];
    int num_moves = getMoveList(&board, move_list);
    for (int i = 0; i < num_moves; ++i) {
        if (move_list[i].from == move.from && move_list[i].to == move.to)
            return 1;
    }
    return 0;
}

int main() {

    setbuf(stdout, 0);
    struct Board board;
    startBoard(&board);
    struct Move move_list[128];
    int num_moves = getMoveList(&board, move_list);
    printBoard(&board);

    while (num_moves) {
        char movestr[4];
        printf("\n enter a move!");
        scanf("%s", movestr);
        struct Move move = moveFromString(movestr);
        while (!check_if_legal(move, board)) {
            printf("\n enter a VALID move!");
            scanf("%s", movestr);
            move = moveFromString(movestr);
        }
        board = doMove(&move, board);
        printBoard(& board);
        struct Move engine = get_best_move(&board, 3);
        board = doMove(&engine , board);
        num_moves = getMoveList(&board, move_list);
        printBoard(&board);
    }
}
