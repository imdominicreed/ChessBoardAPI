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

 public:
  std::string printBoard();
  void start_board();
  Board do_move(Move *move);
  int get_move_list(Move *move_list);
  bool in_check();
  bitboard get_king(int white);
  bitboard get_attack_board(bool white);
  char get_char_sq(int square);
  void update_hash(int from, Piece piece);
  void make_empty_square(unsigned long long mask);
  void do_black_castle(int rook_src, int rook_dst, int king_src, int king_dst);
  void do_white_castle(int rook_src, int rook_dst, int king_src, int king_dst);
  void remove_castle(Move *move);
  Move move_from_str(char string[5]);
};
Board import_fen(char *str);
