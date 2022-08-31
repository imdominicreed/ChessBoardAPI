#pragma once
#include <stdlib.h>

#include "../board/board.hpp"
#include "move_gen.hpp"

struct Board;

struct Move {
  int from;
  int to;
  int promo;
  int en_passant;
};

Move move_from_str(Board board, char string[5]);
Move make_move(int from, int to, int type);
void print_move(char move_str[5], Move *move);
int cmp_move(const void *a, const void *b);
int get_sq(char *string, int index);