#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <iostream>
#include <vector>

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
  unsigned long long en_passant[8];
  unsigned long long castling[16];

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
  std::string toString();
  void startBoard();
  Board doMove(Move *move);
  int getMoveList(Move *move_list);
  bool inCheck();
  bitboard getKing(int white);
  bitboard getAttackBoard(bool white);
  char getCharSq(int square);
  void updateHash(int from, Piece piece, bool color);
  void makeEmptySquare(unsigned long long mask);
  void doBlackCastle(int rook_src, int rook_dst, int king_src, int king_dst);
  void doWhiteCastle(int rook_src, int rook_dst, int king_src, int king_dst);
  void removeCastle(Move *move);
  Move moveFromStr(char string[5]);
  void updateMoveHash(int src, int dst, Piece piece);
  void updateEnpassantHash(int src);
  Piece getPieceType(int sq);
  bool operator==(const Board);
  bool operator!=(const Board);
};
Board import_fen(char *str);
