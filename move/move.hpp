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

 public:
  std::string toString();
};

Move make_move(int from, int to, MoveType type, bool capture);
int cmp_move(const void *a, const void *b);
int get_sq(char *string, int index);