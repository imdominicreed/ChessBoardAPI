//
// Created by Dominic Reed on 3/29/21.
//
#include "board.h"
#include <stdlib.h>

inline struct Move makeMove(int from, int to, int type) {
    struct Move move;
    move.from = from;
    move.to = to;
    return move;
}
char get_char(int location) {
    return 'a'+(location%8);
}
int getRank(int location) {
    return 1+ location/8;
}
void printSquare(int location) {
    printf("%c", ('a'+(location%8)));
    printf("%d", 1+ location/8);
}
void printMove(struct Move *move) {
    printSquare(move->from);
    printSquare(move->to);
}
int notAMove(struct Move *move) {
    return move->from == 0 && move->to == 0;
}

int getSquare(char string[4], int index) {
    return (string[0+index]-'a') + (8*(string[1+index]-'1'));
}
struct Move moveFromString(char string[4]) {
    struct Move move;
    move.from = getSquare(string, 0);
    move.to = getSquare(string, 2);
    return move;
}
void printMoveList(struct Board board, int sorted) {
    struct Move move_list[256];
    int i = 0;
    int n_moves = getMoveList(&board, move_list);
    if (sorted)qsort(move_list, n_moves, sizeof(struct Move), cmpMove);
    printf("%d:\n", n_moves);
    while (i != n_moves) {
        printMove(&move_list[i]);
        printf("\n");
        i++;
    }
}

int cmpMove(const  void * a, const void  * b) {
    struct Move *a1 = (struct Move *) a;
    struct Move *b1 = (struct Move *) b;
    if (get_char(a1->from) != get_char(b1->from)) return get_char(a1->from) -get_char(b1->from);
    if (getRank(a1->from) != getRank(b1->from)) return getRank(a1->from) - getRank(b1->from);
    if (get_char(a1->to) != get_char(b1->to)) return get_char(a1->to) -get_char(b1->to);
    return getRank(a1->from) - getRank(b1->from);
}
