//
// Created by Dominic Reed on 4/17/21.
//
#include "engine.h"
#include "board.h"

int check_if_legal(Move move, Board board) {
    Move move_list[128];
    int num_moves = get_move_list(&board, move_list);
    for (int i = 0; i < num_moves; ++i) {
        if (move_list[i].from == move.from && move_list[i].to == move.to)
            return 1;
    }
    return 0;
}

int main() {

    setbuf(stdout, 0);
    Board board;
    start_board(&board);
    Move move_list[128];
    int num_moves = get_move_list(&board, move_list);
    printBoard(&board);

    while (num_moves) {
        char movestr[4];
        printf("\n enter a move!");
        scanf("%s", movestr);
        Move move = move_to_str(movestr);
        while (!check_if_legal(move, board)) {
            printf("\n enter a VALID move!");
            scanf("%s", movestr);
            move = move_to_str(movestr);
        }
        board = do_move(&move, board);
        printBoard(& board);
        Move engine = get_best_move(&board, 3);
        board = do_move(&engine, board);
        num_moves = get_move_list(&board, move_list);
        printBoard(&board);
    }
}
