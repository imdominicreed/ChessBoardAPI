//
// Created by Dominic Reed on 4/16/21.
//
#include "board.h"
#include "bitutil.h"

int eval(struct Board *board) {
    int white = BITS(board->white_pieces & (board->knights | board->bishops)) * 30;
    int black = BITS(board->black_pieces & (board->knights | board->bishops)) * 30;
    white += BITS(board->white_pieces & board->pawns) * 10;
    black += BITS(board->black_pieces & board->pawns) * 10;
    white += BITS(board->white_pieces & board->rooks) * 50;
    black += BITS(board->black_pieces & board->rooks) * 50;
    white += BITS(board->white_pieces & board->rooks & board->bishops) * 10;
    black += BITS(board->black_pieces & board->rooks & board->bishops) * 10;
    return white-black;
}

int alpha_beta(struct Board board, int depth, int alpha, int beta) {
    if (depth == 0)
        return eval(&board);
    struct Move move_list[256];
    int n = getMoveList(&board, move_list);
    int best_position = 90000000 * (board.white ? -1: 1);
    if(n== 0) return best_position *-1;
    if (board.white) {
        for (int i = 0; i < n; ++i) {
            int pos = alpha_beta(doMove(&move_list[i], board), depth - 1, alpha, beta);
            best_position = pos < best_position ? best_position : pos;
            alpha = alpha < pos ? pos : alpha;
            if (beta <= alpha)
                break;
        }
    } else {
        for (int i = 0; i < n; ++i) {
            int pos = alpha_beta(doMove(&move_list[i], board), depth -1, alpha, beta);
            best_position = pos > best_position ? best_position : pos;
            beta = beta > pos ? pos : beta;
            if (beta <= alpha)
                break;
        }
    }
    return best_position;
}
struct Move get_best_move(struct Board *board, int depth) {
    struct Move move_list[256];
    int n = getMoveList(board, move_list);
    int best_position = 90000000 * (board->white ? -1: 1);
    struct Move best_move;
    for (int i = 0; i < n; ++i) {
        int position = alpha_beta(doMove(&move_list[i], *board), depth-1, -90000000, 90000000);
        if ((board->white && position> best_position) || (!board->white && position< best_position) ) {
            best_position = position;
            best_move = move_list[i];
        }
    }
    return best_move;
}
