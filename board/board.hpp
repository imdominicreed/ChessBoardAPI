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
#include "../util/bitutil.hpp"

#define sliding_size 4
#define jumping_size 8
#define pawn_size 4

static const int pawn_vector[] = {8, 16, 7, 9, 0};

struct Zorbist {
  uint64_t table[64][12];
  uint64_t turn;
  uint64_t en_passant[8];
  uint64_t castling[16];

  uint64_t random();
  Zorbist();
};

enum PieceType {
  Pawn,
  Knight,
  Bishop,
  Rook,
  Queen,
  King,
  NumPieces = 6,
  None,
};
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

struct UndoMove {
  Move move;
  uint8_t en_passant_sq;
  uint8_t castling_rights;
  PieceType captured_piece;
  uint64_t key;
};

struct Board {
  uint64_t key;
  bitboard pieceTypeBB[NumPieces];
  bitboard colorPiecesBB[2];
  uint8_t castling;
  Color turn;
  uint8_t en_passant;

  inline void move_piece(int from, int to, PieceType piece);
  std::string toString();
  void startBoard();
  UndoMove doMove(Move move);
  void undoMove(UndoMove move);
  Move *getMoveList(Move *move_list);
  bool inCheck();
  bitboard getKing(Color turn);
  bitboard getAttackBoard(Color turn);
  char getCharSq(int square);
  inline void makeEmptySquare(int sq, PieceType piece, Color color);
  void doBlackCastle(int rook_src, int rook_dst, int king_src, int king_dst);
  void doWhiteCastle(int rook_src, int rook_dst, int king_src, int king_dst);
  void removeCastle(Move move);
  Move moveFromStr(std::string move);

  void updateHash(int sq, PieceType piece, Color color);
  void updateMoveHash(int src, int dst, PieceType piece);
  void updateEnpassantHash(int src);
  PieceType getPieceType(int sq);
  void createPiece(int sq, PieceType piece);
  bool operator==(const Board);
  bool operator!=(const Board);
};
Board import_fen(char *str);
