#pragma once
#include <stdlib.h>

#include "../board/board.hpp"
#include "move_gen.hpp"

struct Board;

enum class MoveType {
  kNullMove = 0,
  kNormalMove,
  kEnPassant,
  kCastling,
  kQueenPromo,
  kRookPromo,
  kBishopPromo,
  kKnightPromo,
  kWhiteLongCastle,
  kWhiteShortCastle,
  kBlackShortCastle,
  kBlackLongCastle,
  kJumpPawnMove,
};

struct Move {
  int from;
  int to;
  MoveType move_type;
  bool capture;
};

Move move_from_str(Board board, char string[5]);
Move make_move(int from, int to, MoveType type, bool capture);
void print_move(char move_str[5], Move *move);
int cmp_move(const void *a, const void *b);
int get_sq(char *string, int index);