#include "move.hpp"

bitboard get_king(Board *board, int white) {
  bitboard pieces = board->black_pieces | board->white_pieces;
  pieces &= ~(board->pawns | board->bishops | board->rooks | board->knights);
  pieces &= ~(!white ? board->white_pieces : board->black_pieces);
  return pieces;
}

bitboard get_bishop_attack(bitboard pieces, bitboard all_pieces) {
  bitboard attacks = 0;
  int src = 0;
  while (pieces) {
    POP_BSF(src, pieces)
    attacks |= get_bishop_board(src, all_pieces);
  }
  return attacks;
}

bitboard get_rooks_attack(bitboard pieces, bitboard all_pieces) {
  bitboard attacks = 0;
  int src;
  while (pieces) {
    POP_BSF(src, pieces)
    attacks |= get_rook_board(src, all_pieces);
  }
  return attacks;
}

bitboard get_knight_attack(bitboard pieces) {
  bitboard attacks = 0;
  int src;
  while (pieces) {
    POP_BSF(src, pieces)
    attacks |= get_knight_attacks(src);
  }
  return attacks;
}

bitboard get_white_pawn_attack(bitboard pieces) {
  bitboard ret = 0;
  while (pieces) {
    bitboard attacks = 0b101;
    if (pieces & FIRST_COLUMN) attacks = 0b100;
    if (pieces & LAST_COLUMN) attacks = 0b1;
    int dst;
    POP_BSF(dst, pieces);
    attacks <<= dst + 7;
    ret |= attacks;
  }
  return ret;
}

bitboard get_black_pawn_attack(bitboard pieces) {
  bitboard ret = 0;
  while (pieces) {
    bitboard attacks = 0b101;
    int dst;
    POP_BSF(dst, pieces);
    bitboard mask = 1ULL << dst;
    if (mask & FIRST_COLUMN) attacks = 0b100;
    if (mask & LAST_COLUMN) attacks = 0b1;
    shift(attacks, dst - 9);
    ret |= attacks;
  }
  return ret;
}

bitboard get_attack_board(Board *board, bool white) {
  bitboard king_m = get_king(board, white);
  int k;
  POP_BSF(k, king_m);
  bitboard attacks = get_king_attacks(k);
  bitboard my_pieces = white ? board->white_pieces : board->black_pieces;
  bitboard all_pieces = board->black_pieces | board->white_pieces;
  attacks |= get_rooks_attack(my_pieces & board->rooks, all_pieces);
  attacks |= get_bishop_attack(my_pieces & board->bishops, all_pieces);
  attacks |= get_knight_attack(my_pieces & board->knights);
  if (white)
    attacks |= get_white_pawn_attack(my_pieces & board->pawns);
  else
    attacks |= get_black_pawn_attack(my_pieces & board->pawns);
  return attacks;
}

void single_promo(Move move_list[], int src, int dst, int *moveIndex,
                  int type) {
  move_list[*moveIndex] = make_move(src, dst, type);
  ++*moveIndex;
}

void promote_pawn_moves(Move move_list[], int src, int dst, int *moveIndex) {
  single_promo(move_list, src, dst, moveIndex, QUEEN_PROMO);
  single_promo(move_list, src, dst, moveIndex, KNIGHT_PROMO);
  single_promo(move_list, src, dst, moveIndex, ROOK_PROMO);
  single_promo(move_list, src, dst, moveIndex, BISHOP_PROMO);
}

bitboard get_black_pawn_moves(bitboard pieces, bitboard opp_pieces,
                              bitboard en_passant, Move move_list[], int src,
                              int *moveIndex) {
  // Sets up attacks.
  bitboard mask = 1ULL << src;
  bitboard attacks = 0b101;
  if (mask & FIRST_COLUMN) attacks = 0b100;
  if (mask & LAST_COLUMN) attacks = 0b1;
  if (src > 9)
    attacks <<= src - 9;
  else
    attacks >>= -(src - 9);
  attacks &= opp_pieces | en_passant;
  // Sets up movement.
  bitboard ret = attacks;
  bitboard movement = mask >> 8;
  if (movement & ~pieces) {
    attacks |= movement;
    movement >>= 8;
    if (movement & ~pieces && mask & SEVENTH_ROW) attacks |= movement;
  }
  // Adds pawns moves to list.
  int dst;
  while (attacks) {
    POP_BSF(dst, attacks)
    bitboard dst_mask = 1ULL << dst;
    if (dst_mask & FIRST_ROW) {
      promote_pawn_moves(move_list, src, dst, moveIndex);
    } else {
      move_list[*moveIndex] = make_move(src, dst, NORMAL_MOVE);
      move_list[*moveIndex].en_passant = dst_mask == en_passant ? dst - 8 : 0;
      ++*moveIndex;
    }
  }
  return ret;
}

bitboard get_white_pawn_moves(bitboard pieces, bitboard opp_pieces,
                              bitboard en_passant, Move move_list[], int src,
                              int *moveIndex) {
  // Sets up attacks.
  bitboard mask = 1ULL << src;
  bitboard attacks = 0b101;
  if (mask & FIRST_COLUMN) attacks = 0b100;
  if (mask & LAST_COLUMN) attacks = 0b1;
  attacks <<= src + 7;
  attacks &= opp_pieces | en_passant;
  // Sets up movement.
  bitboard ret = attacks;
  bitboard movement = mask << 8;
  if (movement & ~pieces) {
    attacks |= movement;
    movement <<= 8;
    if (movement & ~pieces && mask & SECOND_ROW) attacks |= movement;
  }
  // Adds pawns moves to list.
  int dst;
  while (attacks) {
    POP_BSF(dst, attacks)
    bitboard dst_mask = 1ULL << dst;
    if (dst_mask & LAST_ROW) {
      promote_pawn_moves(move_list, src, dst, moveIndex);
    } else {
      move_list[*moveIndex] = make_move(src, dst, NORMAL_MOVE);
      move_list[*moveIndex].en_passant = dst_mask == en_passant ? dst + 8 : 0;
      ++*moveIndex;
    }
  }
  return ret;
}

void get_castling_moves(Board *board, Move *move_list, int index,
                        int *moveIndex) {
  int castleBit = board->white ? 1 : 4;
  bitboard mask = 1ULL << index;
  bitboard possible_sq = 0b1100000ULL << (board->white ? 0 : 56);
  bitboard check_sq = 0b01100000ULL << (board->white ? 0 : 56);
  bitboard pieces = board->black_pieces | board->white_pieces;
  bitboard op_attacks = get_attack_board(board, !board->white);
  if ((castleBit & board->castling) && !((mask | check_sq) & op_attacks) &&
      !(pieces & possible_sq)) {
    move_list[*moveIndex] = make_move(index, index + 2, CASTLING);
    ++*moveIndex;
  }
  check_sq = 0b1100ULL << (board->white ? 0 : 56);
  possible_sq = 0b1110ULL << (board->white ? 0 : 56);
  castleBit <<= 1;
  if ((castleBit & board->castling) && !((mask | check_sq) & op_attacks) &&
      !(pieces & possible_sq)) {
    move_list[*moveIndex] = make_move(index, index - 2, CASTLING);
    ++*moveIndex;
  }
}

int get_moves(Move move_list[], bitboard attacks, int src, int *moveIndex) {
  int dst;
  while (attacks) {
    POP_BSF(dst, attacks);
    move_list[*moveIndex] = make_move(src, dst, NORMAL_MOVE);
    ++*moveIndex;
  }
  return 1;
}

int get_move_list(Board *board, Move *move_list) {
  if (!get_king(board, board->white)) return -1;
  bitboard all_pieces = board->white_pieces | board->black_pieces;
  bitboard king = get_king(board, !board->white);
  bitboard mypieces = board->white ? board->white_pieces : board->black_pieces;
  int list_index = 0;
  bitboard pieces = mypieces & board->bishops;
  int src;
  while (pieces) {
    POP_BSF(src, pieces);
    bitboard mask = get_bishop_board(src, all_pieces);
    if (mask & king) return -1;
    get_moves(move_list, ~mypieces & get_bishop_board(src, all_pieces), src,
              &list_index);
  }
  pieces = mypieces & board->rooks;
  while (pieces) {
    POP_BSF(src, pieces);
    bitboard mask = ~mypieces & get_rook_board(src, all_pieces);
    if (mask & king) return -1;
    get_moves(move_list, mask, src, &list_index);
  }
  pieces = mypieces & board->knights;
  while (pieces) {
    POP_BSF(src, pieces);
    bitboard mask = ~mypieces & get_knight_attacks(src);
    if (mask & king) return -1;
    get_moves(move_list, ~mypieces & mask, src, &list_index);
  }

  pieces = mypieces & board->pawns;
  bitboard op_pieces = ~mypieces & all_pieces;
  while (pieces) {
    POP_BSF(src, pieces);
    bitboard attacks =
        board->white
            ? get_white_pawn_moves(all_pieces, op_pieces, board->en_passant,
                                   move_list, src, &list_index)
            : get_black_pawn_moves(all_pieces, op_pieces, board->en_passant,
                                   move_list, src, &list_index);
    if (attacks & king) return -1;
  }

  pieces = get_king(board, board->white);
  src = LSB(pieces);
  POP_BSF(src, pieces);
  bitboard mask = ~mypieces & get_king_attacks(src);
  if (mask & king) return -1;
  get_moves(move_list, mask, src, &list_index);
  get_castling_moves(board, move_list, src, &list_index);
  return list_index;
}
