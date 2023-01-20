#include "magic.hpp"

bitboard knight_mask[64];
bitboard king_mask[64];
bitboard bishop_mask[64];
bitboard rook_mask[64];
bitboard rook_table[64][4096];
bitboard bishop_table[64][1024];

bitboard last_column(bitboard location) { return location & LAST_COLUMN; }
bitboard first_column(bitboard location) { return location & FIRST_COLUMN; }
bitboard second_column(bitboard location) { return location & SECOND_COLUMN; }
bitboard seventh_column(bitboard location) { return location & SEVENTH_COLUMN; }
bitboard second_row(bitboard location) { return location & SECOND_ROW; }
bitboard seventh_row(bitboard location) { return location & SEVENTH_ROW; }

int knight_second_column_check(bitboard location, int move) {
  return (((first_column(location) || second_column(location)) &&
           (move == -10 || move == 6)) ||
          ((last_column(location) || seventh_column(location)) &&
           (move == -6 || move == 10)));
}
int knight_first_column_check(bitboard location, int move) {
  return ((first_column(location) && (move == -17 || move == 15)) ||
          (last_column(location) && (move == 17 || move == -15)));
}
int knight_out_of_bounds(bitboard location, int move) {
  return knight_first_column_check(location, move) ||
         knight_second_column_check(location, move);
}
int king_out_of_bounds(bitboard location, int move) {
  return (last_column(location) && (move == 1 || move == 9 || move == -7)) ||
         (first_column(location) && (move == -1 || move == -9 || move == 7));
}

void lookup_table_jumping_setup(const int vectors[], bitboard look_up[]) {
  bitboard mask = 1;
  int in = 0;
  while (mask) {
    bitboard attacks = 0;
    for (int i = 0; i < 8; ++i) {
      int vector = vectors[i];
      bitboard dst = mask;
      shift(dst, vector);
      if (dst == 0 || knight_out_of_bounds(mask, vector) ||
          king_out_of_bounds(mask, vector))
        continue;
      attacks |= dst;
    }
    look_up[in] = attacks;
    mask <<= 1;
    in++;
  }
}
void lookup_rook_setup() {
  for (int sq = 0; sq < 64; ++sq) {
    rook_mask[sq] = (get_ray(sq, NORTH) & ~LAST_ROW) |
                    (get_ray(sq, SOUTH) & ~FIRST_ROW) |
                    (get_ray(sq, EAST) & ~LAST_COLUMN) |
                    (get_ray(sq, WEST) & ~FIRST_COLUMN);
  }
}
void lookup_bishop_setup() {
  bitboard edges = (FIRST_COLUMN | LAST_COLUMN | FIRST_ROW | LAST_ROW);
  for (int sq = 0; sq < 64; ++sq) {
    bishop_mask[sq] = (get_ray(sq, NORTH_WEST) | get_ray(sq, SOUTH_WEST) |
                       get_ray(sq, NORTH_EAST) | get_ray(sq, SOUTH_EAST)) &
                      ~edges;
  }
}
bitboard get_blockers(int index, bitboard mask) {
  bitboard blockers = 0;
  int num_bits = BITS(mask);
  for (int i = 0; i < num_bits; ++i) {
    int bit_in;
    POP_BSF(bit_in, mask);
    if (index & (1 << i)) blockers |= (1ULL << bit_in);
  }
  return blockers;
}
bitboard get_rook_attacks_magic(int sq, bitboard blockers) {
  bitboard attacks = 0;
  attacks |= get_ray(sq, NORTH);
  if (get_ray(sq, NORTH) & blockers)
    attacks &= ~(get_ray(LSB(get_ray(sq, NORTH) & blockers), NORTH));
  attacks |= get_ray(sq, SOUTH);
  if (get_ray(sq, SOUTH) & blockers)
    attacks &= ~(get_ray(MSB(get_ray(sq, SOUTH) & blockers), SOUTH));

  attacks |= get_ray(sq, EAST);
  if (get_ray(sq, EAST) & blockers)
    attacks &= ~(get_ray(LSB(get_ray(sq, EAST) & blockers), EAST));

  attacks |= get_ray(sq, WEST);
  if (get_ray(sq, WEST) & blockers) {
    attacks &= ~(get_ray(MSB(get_ray(sq, WEST) & blockers), WEST));
  }

  return attacks;
}
void init_rook_magic() {
  for (int sq = 0; sq < 64; ++sq) {
    for (int bits = 0; bits < (1 << ROOK_INDEX_BITS[sq]); ++bits) {
      bitboard blockers = get_blockers(bits, rook_mask[sq]);
      int key = (blockers * ROOK_MAGIC[sq]) >> (64 - ROOK_INDEX_BITS[sq]);
      bitboard debug;
      if (key == 2552 && sq == 56) debug = rook_mask[sq];
      get_blockers(bits, rook_mask[sq]);
      rook_table[sq]
                [(blockers * ROOK_MAGIC[sq]) >> (64 - ROOK_INDEX_BITS[sq])] =
                    get_rook_attacks_magic(sq, blockers);
    }
  }
}

bitboard get_bishop_attacks_magic(int sq, bitboard blockers) {
  bitboard attacks = 0;
  attacks |= get_ray(sq, NORTH_EAST);
  if (get_ray(sq, NORTH_EAST) & blockers)
    attacks &= ~(get_ray(LSB(get_ray(sq, NORTH_EAST) & blockers), NORTH_EAST));

  attacks |= get_ray(sq, SOUTH_EAST);
  if (get_ray(sq, SOUTH_EAST) & blockers)
    attacks &= ~(get_ray(MSB(get_ray(sq, SOUTH_EAST) & blockers), SOUTH_EAST));

  attacks |= get_ray(sq, NORTH_WEST);
  if (get_ray(sq, NORTH_WEST) & blockers)
    attacks &= ~(get_ray(LSB(get_ray(sq, NORTH_WEST) & blockers), NORTH_WEST));

  attacks |= get_ray(sq, SOUTH_WEST);
  if (get_ray(sq, SOUTH_WEST) & blockers)
    attacks &= ~(get_ray(MSB(get_ray(sq, SOUTH_WEST) & blockers), SOUTH_WEST));

  return attacks;
}
void init_bishop_magic() {
  for (int sq = 0; sq < 64; ++sq) {
    for (int bits = 0; bits < 1 << BISHOP_INDEX_BITS[sq]; ++bits) {
      bitboard blockers = get_blockers(bits, bishop_mask[sq]);
      bishop_table[sq][(blockers * BISHOP_MAGIC[sq]) >>
                       (64 - BISHOP_INDEX_BITS[sq])] =
          get_bishop_attacks_magic(sq, blockers);
    }
  }
}
void init_tables() {
  init_rays();
  lookup_table_jumping_setup(KNIGHT_VECTOR, knight_mask);
  lookup_table_jumping_setup(KING_VECTOR, king_mask);
  lookup_rook_setup();
  lookup_bishop_setup();
  init_rook_magic();
  init_bishop_magic();
}

bitboard get_bishop_board(int sq, bitboard blockers) {
  blockers &= bishop_mask[sq];
  int key = (blockers * BISHOP_MAGIC[sq]) >> (64 - BISHOP_INDEX_BITS[sq]);
  return bishop_table[sq][key];
}
bitboard get_bishop_attacks(int sq) { return bishop_mask[sq]; }
bitboard get_rook_attacks(int sq) { return rook_mask[sq]; }
bitboard get_knight_attacks(int sq) { return knight_mask[sq]; }
bitboard get_king_attacks(int sq) { return king_mask[sq]; }
bitboard get_rook_board(int sq, bitboard blockers) {
  blockers &= rook_mask[sq];
  int key = (blockers * ROOK_MAGIC[sq]) >> (64 - ROOK_INDEX_BITS[sq]);
  return rook_table[sq][key];
}