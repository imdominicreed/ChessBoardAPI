//
// Created by Dominic Reed on 3/29/21.
//
#include "board.h"
#include <stdlib.h>

inline Move make_move(int from, int to, int type) {
    Move move;
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
void print_sq(int location) {
    printf("%c", ('a'+(location%8)));
    printf("%d", 1+ location/8);
}
void print_move(Move *move) {
    print_sq(move->from);
    print_sq(move->to);
}

int get_sq(char *string, int index) {
    return (string[0+index]-'a') + (8*(string[1+index]-'1'));
}
Move move_to_str(char *string) {
    Move move;
    move.from = get_sq(string, 0);
    move.to = get_sq(string, 2);
    return move;
}
void print_move_list(Board board, int sorted) {
    Move move_list[256];
    int i = 0;
    int n_moves = get_move_list(&board, move_list);
    if (sorted)qsort(move_list, n_moves, sizeof(Move), cmp_move);
    printf("%d:\n", n_moves);
    while (i != n_moves) {
        print_move(&move_list[i]);
        printf("\n");
        i++;
    }
}

int cmp_move(const  void * a, const void  * b) {
    Move *a1 = (Move *) a;
    Move *b1 = (Move *) b;
    if (get_char(a1->from) != get_char(b1->from)) return get_char(a1->from) -get_char(b1->from);
    if (get_rank(a1->from) != get_rank(b1->from)) return get_rank(a1->from) - get_rank(b1->from);
    if (get_char(a1->to) != get_char(b1->to)) return get_char(a1->to) -get_char(b1->to);
    return get_rank(a1->from) - get_rank(b1->from);
}
