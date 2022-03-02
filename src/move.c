//
// Created by Dominic Reed on 3/29/21.
//
#include "board.h"
#include "move_gen.h"
#include <stdlib.h>

inline Move make_move(int from, int to, int type) {
    Move move;
    move.en_passant = 0;
    move.from = from;
    move.to = to;
    move.promo = type;
    return move;
}
char get_char(int location) {
    return 'a'+(location%8);
}
int get_rank(int location) {
    return 1+ location/8;
}

void print_sq(char* move, int location, int push) {
    move[push + 0] = ('a'+(location%8));
    move[push + 1] = '1' + (location/8);
}

void print_move(char* move_str, Move *move) {
    print_sq(move_str, move->from, 0);
    print_sq(move_str, move->to, 2);
    if(move->promo == QUEEN_PROMO) move_str[4] = 'q';
    if(move->promo == ROOK_PROMO) move_str[4] = 'r';
    if(move->promo == BISHOP_PROMO) move_str[4] = 'b';
    if(move->promo == KNIGHT_PROMO) move_str[4] = 'n';
    if(move->promo == NORMAL_MOVE || move->promo == CASTLING) move_str[4] = '\0';
    else move_str[5] = '\0';
}

int get_sq(char *string, int index) {
    return (string[0+index]-'a') + (8*(string[1+index]-'1'));
}
int str_cmp(char* string1, char* string2) {
    for(int i = 0 ; i<4; i++) {
        if (string1[i] != string2[i])
            return 0;
    }
    if(string1[4] != '\0' && string1[4] != string2[4]) return 0;
    return 1;
}

/** returns move from string */
Move move_from_str(Board board, char string[5]) {
    Move move_list[256];
    int moves = get_move_list(&board, move_list);
    char holder[6];
    for(int i=0; i < moves; i++) {
        print_move(holder, &move_list[i]);
        if (str_cmp(string, holder))
            return move_list[i];
    }
    //if this occurs it is an error
    return move_list[0];
}


int cmp_move(const  void * a, const void  * b) {
    Move *a1 = (Move *) a;
    Move *b1 = (Move *) b;
    if (get_char(a1->from) != get_char(b1->from)) return get_char(a1->from) -get_char(b1->from);
    if (get_rank(a1->from) != get_rank(b1->from)) return get_rank(a1->from) - get_rank(b1->from);
    if (get_char(a1->to) != get_char(b1->to)) return get_char(a1->to) -get_char(b1->to);
    return get_rank(a1->from) - get_rank(b1->from);
}
