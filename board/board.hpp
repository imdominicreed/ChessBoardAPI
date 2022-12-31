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

static const int pawn_vector[] = {8, 16, 7, 9, 0};
struct Move;

struct Zorbist {
  uint64_t table[64][12];
  uint64_t turn;
  uint64_t en_passant[8];
  uint64_t castling[16];

  bitboard random();
  Zorbist();
};

enum PieceType { Pawn, Knight, Bishop, Rook, Queen, King, NumPieces = 6 };
enum Piece {
  WhitePawn,
  WhiteKnight,
  WhiteBishop,
  WhiteRook,
  WhiteQueen,
  WhiteKing,
  BlackPawn,
  BlackKnight,
  BlackBishop,
  BlackRook,
  BlackQueen,
  BlackKing
};

enum CastlingRights {
  NO_CASTLING,
  WHITE_KING,
  WHITE_QUEEN = WHITE_KING << 1,
  BLACK_KING = WHITE_KING << 2,
  BLACK_QUEEN = WHITE_KING << 3,

  KING_SIDE = WHITE_KING | BLACK_KING,
  QUEEN_SIDE = WHITE_QUEEN | BLACK_QUEEN,
  WHITE_CASTLING = WHITE_KING | WHITE_QUEEN,
  BLACK_CASTLING = BLACK_KING | BLACK_QUEEN,

  NumCastling = 16
};
enum Color { WHITE, BLACK };

struct Board {
  uint64_t key;
  bitboard pieceTypeBB[NumPieces];
  bitboard colorPiecesBB[2];
  CastlingRights castling[NumCastling];
  Color turn;

 public:
  std::string toString();
  void startBoard();
  Board doMove(Move move);
  int getMoveList(Move *move_list);
  bool inCheck();
  bitboard getKing(int white);
  bitboard getAttackBoard(bool white);
  char getCharSq(int square);
  void updateHash(int from, Piece piece, bool color);
  void makeEmptySquare(bitboard mask);
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
