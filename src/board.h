//
// Created by Dominic Reed on 3/30/21.
//

#ifndef UNTITLED_BOARD_H
#define UNTITLED_BOARD_H
#include <stdbool.h>
#include <stdio.h>
#define sliding_size 4
#define jumping_size 8
#define pawn_size 4


static const int pawn_vector[] = {8,16,7,9};


enum  {
    NULL_MOVE = 0, NORMAL_MOVE, EN_PASSANT, CASTLING, QUEEN_PROMO, ROOK_PROMO, BISHOP_PROMO, KNIGHT_PROMO,
    LONG_CASTLE = 2, SHORT_CASTLE = 1, BLACK = 2
};

struct Board {
    unsigned long long pawns;
    unsigned long long bishops;
    unsigned long long rooks;
    unsigned long long knights;
    unsigned long long white_pieces;
    unsigned long long black_pieces;
    int en_passant;
    int castling;
    bool white;
};
struct Move{
    int from;
    int to;
    int promo;
};

struct Move moveFromString(char string[4]);
void printBoard(struct Board *board);
void startBoard(struct Board *board);
struct Move makeMove(int from, int to, int type);
int getMoveList(struct Board *board, struct Move move_list[]);
void printMove(struct Move *move);
struct Board doMove(struct Move *move, struct Board board);
int cmpMove(const void * a, const void * b);
void printMoveList(struct Board board, int sorted);

#endif //UNTITLED_BOARD_H
