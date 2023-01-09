#pragma once
#include <stdlib.h>

#include <cstdint>
#include <sstream>

#include "../board/board.hpp"
#include "move_gen.hpp"

/**
 * Move:
 * 0-5 From
 * 6-11: To
 * 12-15: MoveType
 */

enum MoveType {
  QuietMove = 0x1000,
  DoublePawnPush,
  KingCastle,
  QueenCastle,
  Capture,
  EnpassantCapture,
  KnightPromotion,
  BishopPromotion,
  RookPromotion,
  QueenPromotion,
  KnightPromotionCapture,
  BishopPromotionCapture,
  RookPromotionCapture,
  QueenPromotionCapture,
};

inline Move make_move(int from, int to, MoveType type) {
  Move move = from;
  move |= to << 6;
  move |= type;
  return move;
}
inline int from(Move m) { return m & 0b111111; }
inline int to(Move m) { return (m >> 6) & 0b111111; }
inline MoveType type(Move m) { return (MoveType)(m & ~0b111111111111); }
inline std::string get_sq(int sq) {
  std::stringstream s;
  s << (char)('a' + (sq % 8));
  s << (char)('1' + (sq / 8));
  return s.str();
}
inline std::string to_string(Move m) {
  std::stringstream s;

  s << get_sq(from(m)) << get_sq(to(m));
  MoveType move_type = type(m);
  if (move_type == QueenPromotion || move_type == QueenPromotionCapture)
    s << 'q';
  if (move_type == RookPromotion || move_type == RookPromotionCapture) s << 'r';
  if (move_type == BishopPromotion || move_type == BishopPromotionCapture)
    s << 'b';
  if (move_type == KnightPromotion || move_type == KnightPromotionCapture)
    s << 'n';
  return s.str();
}
