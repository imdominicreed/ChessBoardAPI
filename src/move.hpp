//
// Created by domin on 7/31/2021.
//

#ifndef UNTITLED_MOVE_H
#define UNTITLED_MOVE_H

#include "board.hpp"

#endif //UNTITLED_MOVE_H
Move move_from_str(Board board, char string[5]);
Move make_move(int from, int to, int type);
void print_move(char move_str[5], Move *move);
int cmp_move(const void * a, const void * b);
int get_sq(char *string, int index);