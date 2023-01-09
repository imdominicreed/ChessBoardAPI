
#pragma once
#include <cstdint>

typedef uint64_t bitboard;
typedef unsigned short Move;

#define BIT(sq) (1ULL << (sq))
#define LSB(x) (__builtin_ctzll(x))
#define MSB(x) (63 - __builtin_clzll(x))
#define BITS(x) (__builtin_popcountll(x))
#define POP_BSF(b, x) \
  (b) = LSB(x);       \
  (x) &= ~BIT(b);
#define shift(i, n) \
  if ((n) > 0)      \
    ((i) <<= (n));  \
  else              \
    ((i) >>= -(n))

static const bitboard LAST_COLUMN = 0x8080808080808080;
static const bitboard SEVENTH_COLUMN = 0X4040404040404040;
static const bitboard FIRST_COLUMN = 0x0101010101010101;
static const bitboard SECOND_COLUMN = 0x0202020202020202;
static const bitboard SEVENTH_ROW = 0x00FF000000000000;
static const bitboard SECOND_ROW = 0x000000000000FF00;
static const bitboard FIRST_ROW = 0xFF;
static const bitboard LAST_ROW = 0xFF00000000000000;
enum Square {
  A1,
  B1,
  C1,
  D1,
  E1,
  F1,
  G1,
  H1,
  A2,
  B2,
  C2,
  D2,
  E2,
  F2,
  G2,
  H2,
  A3,
  B3,
  C3,
  D3,
  E3,
  F3,
  G3,
  H3,
  A4,
  B4,
  C4,
  D4,
  E4,
  F4,
  G4,
  H4,
  A5,
  B5,
  C5,
  D5,
  E5,
  F5,
  G5,
  H5,
  A6,
  B6,
  C6,
  D6,
  E6,
  F6,
  G6,
  H6,
  A7,
  B7,
  C7,
  D7,
  E7,
  F7,
  G7,
  H7,
  A8,
  B8,
  C8,
  D8,
  E8,
  F8,
  G8,
  H8,
};
