#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <iostream>

#include "../magic/magic.hpp"
#include "../move/move.hpp"
#include "../move/move_gen.hpp"

#define sliding_size 4
#define jumping_size 8
#define pawn_size 4

static const int pawn_vector[] = {8, 16, 7, 9};
struct Move;

struct Zorbist {
  unsigned long long table[64][12];
  unsigned long long turn;
  unsigned long long random();
  Zorbist();
};

struct Board {
  unsigned long long key;
  unsigned long long pawns;
  unsigned long long bishops;
  unsigned long long rooks;
  unsigned long long knights;
  unsigned long long white_pieces;
  unsigned long long black_pieces;
  unsigned long long en_passant;
  int castling;
  bool white;
};

std::string printBoard(Board *board);
void start_board(Board *board);
Board do_move(Move *move, Board board);
Board *import_fen(char *str);
bool in_check(Board *board);
