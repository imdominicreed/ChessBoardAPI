#include "rays.hpp"

bitboard RAYS[8][64];

bitboard west(bitboard mask, int shift) {
  bitboard attacks = 0;
  shift(mask, shift);
  while (mask && !(mask & FIRST_COLUMN)) {
    attacks |= mask;
    shift(mask, shift);
  }
  return attacks;
}

bitboard east(bitboard mask, int shift) {
  bitboard attacks = 0;
  shift(mask, shift);
  while (mask && !(mask & LAST_COLUMN)) {
    attacks |= mask;
    shift(mask, shift);
  }
  return attacks;
}

int col(int sq) { return sq % 8; }

int row(int sq) { return sq / 8; }

void init_rays() {
  for (unsigned int sq = 0; sq < 64; sq++) {
    RAYS[WEST][sq] = (1ULL << sq) - (1ULL << (sq & 56U));
    RAYS[EAST][sq] = 2 * ((1ULL << (sq | 7U)) - (1ULL << sq));
    RAYS[SOUTH][sq] = 0x0080808080808080ULL >> (63U - sq);
    RAYS[NORTH][sq] = 0x0101010101010100ULL << sq;
    bitboard mask = 1ULL << sq;
    RAYS[NORTH_WEST][sq] = west(mask, 9);
    RAYS[NORTH_EAST][sq] = east(mask, 7);
    RAYS[SOUTH_WEST][sq] = west(mask, -7);
    RAYS[SOUTH_EAST][sq] = east(mask, -9);
  }
}

bitboard get_ray(int sq, int dir) { return RAYS[dir][sq]; }