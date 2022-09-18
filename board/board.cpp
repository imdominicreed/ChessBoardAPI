#include "board.hpp"
Zorbist z;

unsigned long long Zorbist::random() {
  return (0xFFFFLL & rand()) | ((0xFFFFLL & rand()) << 16) |
         ((0xFFFFLL & rand()) << 32) | ((0xFFFFLL & rand()) << 48);
}

Zorbist::Zorbist() {
  std::srand(time(nullptr));
  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 12; j++) {
      table[i][j] = random();
    }
  }
  turn = random();
}

char* strrev(char* str) {
  int i = 16 - 1, j = 0;

  char ch;
  while (i > j) {
    ch = str[i];
    str[i] = str[j];
    str[j] = ch;
    i--;
    j++;
  }
  return str;
}

Board import_fen(char* str) {
  Board ret = {};
  bitboard mask = 1ULL << 63;
  char* copy = strdup(str);
  const char del[2] = "/";
  char* row = strtok(copy, del);
  char* last = NULL;
  while (row != NULL) {
    int end = strlen(row);
    mask >>= 8;
    for (int i = 0; i < end; i++) {
      if (row[i] == ' ') break;
      if (row[i] - '0' < 9) {
        mask <<= row[i] - '0';
        if (!mask) mask = 1ULL << (row[i] - '0' - 1);
      } else {
        mask <<= 1;
        if (!mask) mask = 1;
        if (row[i] == 'r' || row[i] == 'R' || row[i] == 'Q' || row[i] == 'q')
          ret.rooks |= mask;
        if (row[i] == 'b' || row[i] == 'B' || row[i] == 'Q' || row[i] == 'q')
          ret.bishops |= mask;
        else if (row[i] == 'n' || row[i] == 'N')
          ret.knights |= mask;
        else if (row[i] == 'p' || row[i] == 'P')
          ret.pawns |= mask;
        if (row[i] - 'A' < 26)
          ret.white_pieces |= mask;
        else
          ret.black_pieces |= mask;
      }
    }
    mask >>= 8;
    last = (char*)realloc(last, strlen(row) + 1);
    strncpy(last, row, strlen(row) + 1);
    row = strtok(NULL, del);
  }

  char* turn = index(last, ' ') + 1;
  ret.white = turn[0] == 'w';
  char* castling = index(turn, ' ') + 1;
  int i = 0;
  while (castling[i] != ' ') {
    if (castling[i] == 'k') ret.castling |= 0b0100;
    if (castling[i] == 'K') ret.castling |= 0b0001;
    if (castling[i] == 'Q') ret.castling |= 0b0010;
    if (castling[i] == 'q') ret.castling |= 0b1000;
    i++;
  }
  char* en = castling + i + 1;
  if (en[0] != '-') ret.en_passant = (1ULL << get_sq(en, 0));
  ret.key = 0;
  return ret;
}
void Board::makeEmptySquare(unsigned long long mask) {
  white_pieces &= ~mask;
  black_pieces &= ~mask;
  rooks &= ~mask;
  pawns &= ~mask;
  bishops &= ~mask;
  knights &= ~mask;
}

void Board::updateHash(int from, Piece piece, bool color) {
  key ^= z.table[from][piece + (color ? 0 : 6)];
}

void move_piece(unsigned long long* board, int from, int to) {
  *board &= ~(1ULL << from);
  *board |= 1ULL << to;
}

void Board::doBlackCastle(int rook_src, int rook_dst, int king_src,
                          int king_dst) {
  move_piece(&black_pieces, king_src, king_dst);
  move_piece(&rooks, rook_src, rook_dst);
  move_piece(&black_pieces, rook_src, rook_dst);
}

void Board::doWhiteCastle(int rook_src, int rook_dst, int king_src,
                          int king_dst) {
  move_piece(&white_pieces, king_src, king_dst);
  move_piece(&rooks, rook_src, rook_dst);
  move_piece(&white_pieces, rook_src, rook_dst);
}

void Board::removeCastle(Move* move) {
  if (move->to == Square::A1) castling &= 0b1101;
  if (move->to == Square::H1) castling &= 0b1110;
  if (move->to == Square::A8) castling &= 0b0111;
  if (move->to == Square::H8) castling &= 0b1011;
}

void Board::updateMoveHash(int src, int dst, Piece piece) {
  updateHash(src, piece, white);
  updateHash(dst, piece, white);
}

Piece Board::getPieceType(int sq) {
  bitboard mask = 1ULL << sq;
  if (pawns & mask) return Piece::Pawn;
  if (knights & mask) return Piece::Knight;
  if (rooks & mask) return bishops & mask ? Piece::Queen : Piece::Rook;
  if (bishops & mask) return Piece::Bishop;
  return Piece::King;
}

Board Board::doMove(Move* move) {
  Board board = *this;
  board.key ^= z.turn;
  bitboard from = 1ULL << move->from;
  bitboard to = 1ULL << move->to;
  bitboard* pieces = board.white ? &board.white_pieces : &board.black_pieces;
  if (move->move_type == MoveType::kNormalMove) {
    // Handling capturing!
    if (move->capture) {
      board.updateHash(move->to, getPieceType(move->to), !white);
      board.makeEmptySquare(to);
    }

    if (from & board.pawns) {
      board.updateMoveHash(move->from, move->to, Piece::Pawn);
      move_piece(&board.pawns, move->from, move->to);

    } else if (from & board.bishops && from & board.rooks) {
      board.updateMoveHash(move->from, move->to, Piece::Queen);
      move_piece(&board.bishops, move->from, move->to);
      move_piece(&board.rooks, move->from, move->to);

    } else if (from & board.bishops) {
      board.updateMoveHash(move->from, move->to, Piece::Bishop);
      move_piece(&board.bishops, move->from, move->to);

    } else if (from & board.rooks) {
      board.updateMoveHash(move->from, move->to, Piece::Rook);
      move_piece(&board.rooks, move->from, move->to);

    } else if (from & board.knights) {
      board.updateMoveHash(move->from, move->to, Piece::Knight);
      move_piece(&board.knights, move->from, move->to);
    } else
      updateHash(move->from, Piece::King, white);

    if (move->from == Square::E1) board.castling &= 0b1100;
    if (move->from == Square::E8) board.castling &= 0b11;
    if (move->from == Square::A1 || move->to == Square::A1)
      board.castling &= 0b1101;
    if (move->from == Square::H1 || move->to == Square::H1)
      board.castling &= 0b1110;
    if (move->from == Square::A8 || move->to == Square::A8)
      board.castling &= 0b0111;
    if (move->from == Square::H8 || move->to == Square::H8)
      board.castling &= 0b1011;

    move_piece(pieces, move->from, move->to);
  } else if (move->move_type == MoveType::kJumpPawnMove) {
    updateMoveHash(move->from, move->to, Piece::Pawn);
    move_piece(&board.pawns, move->from, move->to);
    move_piece(pieces, move->from, move->to);
    board.en_passant = board.white ? from : to;
    board.white = !board.white;
    board.en_passant <<= 8;
    return board;
  } else if (move->move_type == MoveType::kBlackLongCastle) {
    updateMoveHash(56, 59, Piece::Rook);
    updateMoveHash(60, 58, Piece::King);

    board.doBlackCastle(56, 59, 60, 58);
    board.castling &= 0b11;
  } else if (move->move_type == MoveType::kBlackShortCastle) {
    updateMoveHash(53, 51, Piece::Rook);
    updateMoveHash(60, 62, Piece::King);

    board.doBlackCastle(63, 61, 60, 62);
    board.castling &= 0b11;
  } else if (move->move_type == MoveType::kWhiteLongCastle) {
    updateMoveHash(0, 3, Piece::Rook);
    updateMoveHash(4, 2, Piece::King);

    board.doWhiteCastle(0, 3, 4, 2);
    board.castling &= 0b1100;
  } else if (move->move_type == MoveType::kWhiteShortCastle) {
    updateMoveHash(7, 5, Piece::Rook);
    updateMoveHash(4, 6, Piece::King);

    board.doWhiteCastle(7, 5, 4, 6);
    board.castling &= 0b1100;
  } else if (move->move_type == MoveType::kEnPassant) {
    if (board.white)
      board.en_passant >>= 8;
    else
      board.en_passant <<= 8;
    board.makeEmptySquare(board.en_passant);
    updateHash(move->to, Piece::Pawn, !white);

    updateMoveHash(move->from, move->to, Piece::Pawn);
    move_piece(&board.pawns, move->from, move->to);
    move_piece(pieces, move->from, move->to);
  } else if (move->move_type == MoveType::kQueenPromo) {
    updateHash(move->from, Piece::Pawn, white);
    updateHash(move->to, Piece::Queen, white);

    board.makeEmptySquare(from);
    board.removeCastle(move);

    if (move->capture) {
      board.makeEmptySquare(to);
    }

    *pieces |= 1ULL << move->to;
    board.bishops |= 1ULL << move->to;
    board.rooks |= 1ULL << move->to;
  } else if (move->move_type == MoveType::kKnightPromo) {
    updateHash(move->from, Piece::Pawn, white);
    updateHash(move->to, Piece::Knight, white);

    board.makeEmptySquare(from);
    board.removeCastle(move);

    if (move->capture) {
      board.makeEmptySquare(to);
    }

    *pieces |= to;
    board.knights |= to;
  } else if (move->move_type == MoveType::kBishopPromo) {
    updateHash(move->from, Piece::Pawn, white);
    updateHash(move->to, Piece::Bishop, white);

    board.makeEmptySquare(from);
    board.removeCastle(move);

    if (move->capture) {
      board.makeEmptySquare(to);
    }

    *pieces |= 1ULL << move->to;
    board.bishops |= 1ULL << move->to;
  } else if (move->move_type == MoveType::kRookPromo) {
    updateHash(move->from, Piece::Pawn, white);
    updateHash(move->to, Piece::Rook, white);

    board.makeEmptySquare(from);
    board.removeCastle(move);

    if (move->capture) {
      board.makeEmptySquare(to);
    }

    *pieces |= 1ULL << move->to;
    board.rooks |= 1ULL << move->to;
  }
  board.white = !board.white;
  board.en_passant = 0;
  return board;
}

void mirror(bitboard* board, int shift) { *board |= (*board << shift); }

void Board::startBoard() {
  init_tables();
  rooks = 0b10001001;
  mirror(&rooks, 56);
  bishops = 0b00101100;
  mirror(&bishops, 56);
  knights = 0b01000010;
  mirror(&knights, 56);
  pawns = (0b11111111 << 8);
  white_pieces = 0b11111111 | pawns;
  mirror(&pawns, 40);
  black_pieces = white_pieces << 48;
  white = true;
  castling = 0b1111;
  key = 0;
  en_passant = 0;
}
char Board::getCharSq(int square) {
  bitboard mask = 1ULL << square;
  if (!(mask & (white_pieces | black_pieces))) return '-';
  char letter;
  if ((mask & bishops) && (mask & rooks))
    letter = 'q';
  else if (mask & rooks)
    letter = 'r';
  else if (mask & bishops)
    letter = 'b';
  else if (mask & pawns)
    letter = 'p';
  else if (mask & knights)
    letter = 'n';
  else
    letter = 'k';
  if (mask & white_pieces) letter -= 32;
  return letter;
}
bool Board::inCheck() { return getAttackBoard(white) & getKing(!white); }

std::string Board::toString() {
  std::string s;
  std::string rev;
  for (int i = 0; i <= 64; ++i) {
    if (!(i % 8) && i) {
      std::reverse(rev.begin(), rev.end());
      s += rev + '\n';
      rev = "";
    }
    rev += getCharSq(i);
    rev += ' ';
  }
  return s;
}
