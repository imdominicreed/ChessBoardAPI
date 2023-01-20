#include "move.hpp"

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

bitboard Board::getKing(Color color) {
  return colorPiecesBB[color] & pieceTypeBB[King];
}

bitboard Board::getAttackBoard(Color turn) {
  bitboard king_m = getKing(turn);
  int k;
  POP_BSF(k, king_m);
  bitboard attacks = get_king_attacks(k);
  bitboard my_pieces = colorPiecesBB[turn];
  bitboard all_pieces = colorPiecesBB[BLACK] | colorPiecesBB[WHITE];
  attacks |= get_rooks_attack(my_pieces & (pieceTypeBB[Queen] | pieceTypeBB[Rook]), all_pieces);
  attacks |= get_bishop_attack(my_pieces & (pieceTypeBB[Queen] | pieceTypeBB[Bishop]), all_pieces);
  attacks |= get_knight_attack(my_pieces & pieceTypeBB[Knight]);
  if (turn == WHITE)
    attacks |= get_white_pawn_attack(my_pieces & pieceTypeBB[Pawn]);
  else
    attacks |= get_black_pawn_attack(my_pieces & pieceTypeBB[Pawn]);
  return attacks;
}

inline Move *single_promo(Move *move_list, int src, int dst,
                          MoveType move_type) {
  (*move_list++) = make_move(src, dst, move_type);
  return move_list;
}

inline Move *promote_pawn_moves(Move *move_list, int src, int dst,
                                bool capture) {
  move_list = single_promo(move_list, src, dst,
                           capture ? QueenPromotionCapture : QueenPromotion);
  move_list = single_promo(move_list, src, dst,
                           capture ? KnightPromotionCapture : KnightPromotion);
  move_list = single_promo(move_list, src, dst,
                           capture ? RookPromotionCapture : RookPromotion);
  return single_promo(move_list, src, dst,
                      capture ? BishopPromotionCapture : BishopPromotion);
}

Move *get_black_pawn_moves(bitboard pieces, bitboard opp_pieces,
                           bitboard en_passant, Move *move_list, int src,
                           bitboard king) {
  // Sets up attacks.
  bitboard mask = 1ULL << src;
  bitboard attacks = 0b101;
  if (mask & FIRST_COLUMN) attacks = 0b100;
  if (mask & LAST_COLUMN) attacks = 0b01;
  attacks <<= src - 9;
  attacks &= opp_pieces | (1ULL << en_passant) ;
  // Sets up movement.
  if (king & attacks) return nullptr;
  bitboard movement = mask >> 8;
  if (movement & ~pieces) {
    attacks |= movement;
    movement >>= 8;
    if (movement & ~pieces && mask & SEVENTH_ROW)
      (*move_list++) = make_move(src, src - 16, DoublePawnPush);
  }
  // Adds pawns moves to list.
  int dst;
  while (attacks) {
    POP_BSF(dst, attacks)
    bitboard dst_mask = 1ULL << dst;
    if (dst_mask & FIRST_ROW) {
      move_list =
          promote_pawn_moves(move_list, src, dst, dst_mask & opp_pieces);
    } else {
      Move move;
      if (en_passant == dst)
        move = make_move(src, dst, EnpassantCapture);
      else
        move = make_move(src, dst, dst_mask & opp_pieces ? Capture : QuietMove);

      *move_list++ = move;
    }
  }
  return move_list;
}

inline Move *get_white_pawn_moves(bitboard pieces, bitboard opp_pieces,
                                  uint8_t en_passant, Move *move_list, int src,
                                  bitboard king) {
  // Sets up attacks.
  bitboard mask = 1ULL << src;
  bitboard attacks = 0b101;
  if (mask & FIRST_COLUMN) attacks = 0b100;
  if (mask & LAST_COLUMN) attacks = 0b1;
  attacks <<= src + 7;
  attacks &= opp_pieces | (1ULL << en_passant);
  // Sets up movement.
  if (attacks & king) return nullptr;
  bitboard movement = mask << 8;
  if (movement & ~pieces) {
    attacks |= movement;
    movement <<= 8;
    if (movement & ~pieces && mask & SECOND_ROW)
      *move_list++ = make_move(src, src + 16, DoublePawnPush);
  }
  // Adds pawns moves to list.
  int dst;
  while (attacks) {
    POP_BSF(dst, attacks)
    bitboard dst_mask = 1ULL << dst;
    if (dst_mask & LAST_ROW) {
      move_list =
          promote_pawn_moves(move_list, src, dst, dst_mask & opp_pieces);
    } else {
      Move move;
      if (en_passant == dst)
        move = make_move(src, dst, EnpassantCapture);
      else
        move = make_move(src, dst, dst_mask & opp_pieces ? Capture : QuietMove);
      *move_list++ = move;
    }
  }
  return move_list;
}

inline Move *get_white_castling_moves(Board *board, Move *move_list,
                                      int index) {
  bitboard mask = 1ULL << index;
  bitboard possible_sq = 0b1100000ULL;
  bitboard check_sq = 0b01100000ULL | mask;
  bitboard pieces = board->colorPiecesBB[WHITE] | board->colorPiecesBB[BLACK];
  bitboard op_attacks = board->getAttackBoard(BLACK);

  if ((CastlingRights::WHITE_KING & board->castling) &&
      !(check_sq & op_attacks) && !(pieces & possible_sq))
    *move_list++ = make_move(index, index + 2, KingCastle);

  check_sq = 0b1100ULL | mask;
  possible_sq = 0b1110ULL;
  if ((CastlingRights::WHITE_QUEEN & board->castling) &&
      !(check_sq & op_attacks) && !(pieces & possible_sq))
    *move_list++ = make_move(index, index - 2, QueenCastle);

  return move_list;
}

inline Move *get_black_castling_moves(Board *board, Move *move_list,
                                      int index) {
  bitboard mask = 1ULL << index;
  bitboard possible_sq = 0x6000000000000000ULL;
  bitboard check_sq = possible_sq | mask;
  bitboard pieces = board->colorPiecesBB[WHITE] | board->colorPiecesBB[BLACK];
  bitboard op_attacks = board->getAttackBoard(WHITE);
  if ((CastlingRights::BLACK_KING & board->castling) &&
      !(check_sq & op_attacks) && !(pieces & possible_sq))
    *move_list++ = make_move(index, index + 2, KingCastle);

  check_sq = 0xc00000000000000ULL | mask;
  possible_sq = 0xe00000000000000ULL;
  if ((CastlingRights::BLACK_QUEEN & board->castling) &&
      !(check_sq & op_attacks) && !(pieces & possible_sq)) {
    *move_list++ = make_move(index, index - 2, QueenCastle);
  }
  return move_list;
}

inline Move *get_moves(Move *move_list, bitboard attacks, int src,
                       bitboard pieces) {
  int dst;
  while (attacks) {
    POP_BSF(dst, attacks)
    bitboard dst_mask = 1ULL << dst;
    *move_list++ = make_move(src, dst, pieces & dst_mask ? Capture : QuietMove);
  }
  return move_list;
}

Move *Board::getMoveList(Move *move_list) {
  bitboard king = pieceTypeBB[King] & colorPiecesBB[turn ^ 1];
  if (king == 0) return nullptr;

  bitboard all_pieces = colorPiecesBB[WHITE] | colorPiecesBB[BLACK];
  bitboard my_pieces = colorPiecesBB[turn];

  // bishop
  bitboard pieces = my_pieces & (pieceTypeBB[Bishop] | pieceTypeBB[Queen]);
  int src;
  while (pieces) {
    POP_BSF(src, pieces);
    bitboard mask = ~my_pieces & get_bishop_board(src, all_pieces);
    if (mask & king) return nullptr;
    move_list = get_moves(move_list, mask, src, all_pieces);
  }

  // rooks
  pieces = my_pieces & (pieceTypeBB[Rook] | pieceTypeBB[Queen]);
  while (pieces) {
    POP_BSF(src, pieces);
    bitboard mask = ~my_pieces & get_rook_board(src, all_pieces);
    if (mask & king) return nullptr;
    move_list = get_moves(move_list, mask, src, all_pieces);
  }

  // knights
  pieces = my_pieces & pieceTypeBB[Knight];
  while (pieces) {
    POP_BSF(src, pieces);
    bitboard mask = ~my_pieces & get_knight_attacks(src);
    if (mask & king) return nullptr;
    move_list = get_moves(move_list, mask, src, all_pieces);
  }

  // pawns
  pieces = my_pieces & pieceTypeBB[Pawn];
  bitboard op_pieces = ~my_pieces & all_pieces;
  while (pieces) {
    POP_BSF(src, pieces);
    move_list = turn == WHITE
                    ? get_white_pawn_moves(all_pieces, op_pieces, en_passant,
                                           move_list, src, king)
                    : get_black_pawn_moves(all_pieces, op_pieces, en_passant,
                                           move_list, src, king);
    if (move_list == nullptr) return nullptr;
  }

  // King
  pieces = colorPiecesBB[turn] & pieceTypeBB[King];
  src = LSB(pieces);
  POP_BSF(src, pieces);
  bitboard mask = ~my_pieces & get_king_attacks(src);
  if (mask & king) return nullptr;
  move_list = get_moves(move_list, mask, src, all_pieces);
  if (turn == WHITE)
    move_list = get_white_castling_moves(this, move_list, src);
  else
    move_list = get_black_castling_moves(this, move_list, src);
  return move_list;
}
