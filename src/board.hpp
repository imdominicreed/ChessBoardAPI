//
// Created by Dominic Reed on 3/30/21.
//

#ifndef UNTITLED_BOARD_H
#define UNTITLED_BOARD_H
#include <stdbool.h>
#include <stdio.h>
#include <string>
#define sliding_size 4
#define jumping_size 8
#define pawn_size 4


static const int pawn_vector[] = {8,16,7,9};


enum  {
    NULL_MOVE = 0, NORMAL_MOVE, EN_PASSANT, CASTLING, QUEEN_PROMO, ROOK_PROMO, BISHOP_PROMO, KNIGHT_PROMO,
    LONG_CASTLE = 2, SHORT_CASTLE = 1, BLACK = 2
};

typedef struct {
    unsigned long long pawns;
    unsigned long long bishops;
    unsigned long long rooks;
    unsigned long long knights;
    unsigned long long white_pieces;
    unsigned long long black_pieces;
    unsigned long long en_passant;
    int castling;
    bool white;
} Board;
typedef struct {
    int from;
    int to;
    int promo;
    int en_passant;
} Move;





#endif //UNTITLED_BOARD_H

std::string printBoard(Board *board);
void start_board(Board *board);
Board do_move(Move *move, Board board);
Board* import_fen(char *str);
bool in_check(Board *board);

