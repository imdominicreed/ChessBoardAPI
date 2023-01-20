#include "board.hpp"
Zorbist z;

bitboard Zorbist::random() {
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
  for (int i = 0; i < 8; i++) en_passant[i] = random();

  castling[0] = 0;
  for (int i = 1; i < 16; i++) castling[i] = random();

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
        switch (row[i]) {
          case 'r':
          case 'R':
            ret.pieceTypeBB[Rook] |= mask;
            break;
          case 'b':
          case 'B':
            ret.pieceTypeBB[Bishop] |= mask;
            break;
          case 'q':
          case 'Q':
            ret.pieceTypeBB[Queen] |= mask;
            break;
          case 'n':
          case 'N':
            ret.pieceTypeBB[Knight] |= mask;
            break;
          case 'k':
          case 'K':
            ret.pieceTypeBB[King] |= mask;
            break;
          case 'p':
          case 'P':
            ret.pieceTypeBB[Pawn] |= mask;
        }
        if (row[i] - 'A' < 26)
          ret.colorPiecesBB[WHITE] |= mask;
        else
          ret.colorPiecesBB[BLACK] |= mask;
      }
    }
    mask >>= 8;
    last = (char*)realloc(last, strlen(row) + 1);
    strncpy(last, row, strlen(row) + 1);
    row = strtok(NULL, del);
  }

  char* turn = index(last, ' ') + 1;
  ret.turn = turn[0] == 'w' ? WHITE : BLACK;
  char* castling = index(turn, ' ') + 1;
  int i = 0;
  while (castling[i] != ' ') {
    if (castling[i] == 'k') ret.castling |= BLACK_KING;
    if (castling[i] == 'K') ret.castling |= WHITE_KING;
    if (castling[i] == 'Q') ret.castling |= WHITE_QUEEN;
    if (castling[i] == 'q') ret.castling |= BLACK_QUEEN;
    i++;
  }
  char* en = castling + i + 1;
  if (en[0] != '-')
    ret.en_passant = 1ULL << (((en[0] - 'a') * 8) + (en[1] - '1'));
  ret.key = 0;
  return ret;
}
inline void Board::makeEmptySquare(bitboard mask, PieceType piece,
                                   Color color) {
  pieceTypeBB[piece] &= ~mask;
  colorPiecesBB[color] &= ~mask;
}

void Board::updateHash(int from, Piece piece, bool color) {
  key ^= z.table[from][piece + (color ? 0 : 6)];
}

void Board::updateEnpassantHash(int src) { key ^= z.en_passant[src % 8]; }

inline void Board::move_piece(int from, int to, PieceType piece) {
  colorPiecesBB[turn] ^= 1ULL << from;
  colorPiecesBB[turn] |= 1ULL << to;
  pieceTypeBB[piece] ^= 1ULL << from;
  pieceTypeBB[piece] |= 1ULL << to;
}

// void Board::updateMoveHash(int src, int dst, Piece piece) {
//   updateHash(src, piece, white);
//   updateHash(dst, piece, white);
// }

PieceType Board::getPieceType(int sq) {
  bitboard mask = 1ULL << sq;
  if (pieceTypeBB[Pawn] & mask) return Pawn;
  if (pieceTypeBB[Knight] & mask) return Knight;
  if (pieceTypeBB[Rook] & mask) return Rook;
  if (pieceTypeBB[Queen] & mask) return Queen;
  if (pieceTypeBB[Bishop] & mask) return Bishop;
  return King;
}

void Board::createPiece(int sq, PieceType piece) {
  pieceTypeBB[piece] |= 1ULL << sq;
  colorPiecesBB[turn] |= 1ULL << sq;
}

Board Board::doMove(Move move) {
  Board board = *this;
  // board.key ^= z.turn;
  // if (board.en_passant) board.updateEnpassantHash(MSB(board.en_passant));

  int to_sq = to(move);
  int from_sq = from(move);
  bitboard from_mask = 1ULL << from_sq;
  bitboard to_mask = 1ULL << to_sq;
  MoveType move_type = type(move);
  int en_passant = board.en_passant;
  board.en_passant = 0;

  switch (move_type) {
    case Capture:
      board.makeEmptySquare(to_mask, getPieceType(to_sq), (Color)(turn^1));
      [[fallthrough]];
    case QuietMove:
      board.move_piece(from_sq, to_sq, getPieceType(from_sq));
      // removes castling
      if (from_sq == Square::E1) board.castling &= ~WHITE_CASTLING;
      if (from_sq == Square::E8) board.castling &= ~BLACK_CASTLING;
      if (from_sq == Square::A1 || to_sq == Square::A1)
        board.castling &= ~WHITE_QUEEN;
      if (from_sq == Square::H1 || to_sq == Square::H1)
        board.castling &= ~WHITE_KING;
      if (from_sq == Square::A8 || to_sq == Square::A8)
        board.castling &= ~BLACK_QUEEN;
      if (from_sq == Square::H8 || to_sq == Square::H8)
        board.castling &= ~BLACK_KING;
      break;

    case DoublePawnPush:
      board.move_piece(from_sq, to_sq, Pawn);
      board.en_passant = turn == WHITE ? to_sq - 8 : to_sq + 8;
      break;
    case KingCastle:
      board.move_piece(from_sq, to_sq, King);
      board.move_piece(to_sq + 1, to_sq - 1, Rook);
      board.castling &= turn == WHITE ? BLACK_CASTLING : WHITE_CASTLING;
      break;
    case QueenCastle:
      board.move_piece(from_sq, to_sq, King);
      board.move_piece(to_sq - 2, to_sq + 1, Rook);
      board.castling &= turn == WHITE ? BLACK_CASTLING : WHITE_CASTLING;
      break;
    case EnpassantCapture:
      board.makeEmptySquare(1ULL << (to_sq + (BLACK == turn ? 8 : -8)), Pawn, (Color)(turn^1));
      board.move_piece(from_sq, to_sq, Pawn);
      break;

    case KnightPromotionCapture:
      board.makeEmptySquare(to_mask, board.getPieceType(to_sq), (Color)(turn ^ 1));
      [[fallthrough]];
    case KnightPromotion:
      board.makeEmptySquare(from_mask, Pawn, turn);
      board.createPiece(to_sq, Knight);
      break;

    case QueenPromotionCapture:
      board.makeEmptySquare(to_mask, board.getPieceType(to_sq), (Color)(turn ^ 1));
      [[fallthrough]];
    case QueenPromotion:
      board.makeEmptySquare(from_mask, Pawn, turn);
      board.createPiece(to_sq, Queen);
      break;

    case RookPromotionCapture:
      board.makeEmptySquare(to_mask, board.getPieceType(to_sq), (Color)(turn^1));
      [[fallthrough]];
    case RookPromotion:
      board.makeEmptySquare(from_mask, Pawn, turn);
      board.createPiece(to_sq, Rook);
      break;

    case BishopPromotionCapture:
      board.makeEmptySquare(to_mask, board.getPieceType(to_sq), (Color)(turn^1));
      [[fallthrough]];
    case BishopPromotion:
      board.makeEmptySquare(from_mask, Pawn, turn);
      board.createPiece(to_sq, Bishop);
      break;
  }
  board.turn = (Color)(turn ^ 1);
  return board;
}

void Board::startBoard() {
  init_tables();
  pieceTypeBB[Rook] = 0b10001001;
  pieceTypeBB[Rook] |= pieceTypeBB[Rook] << 56;

  pieceTypeBB[Bishop] = 0b00101100;
  pieceTypeBB[Bishop] |= pieceTypeBB[Bishop] << 56;

  pieceTypeBB[Knight] = 0b01000010;
  pieceTypeBB[Knight] |= pieceTypeBB[Knight] << 56;

  pieceTypeBB[Pawn] = (0b11111111 << 8);
  pieceTypeBB[Pawn] |= pieceTypeBB[Pawn] << 40;

  colorPiecesBB[WHITE] = 0b11111111 | pieceTypeBB[Pawn];
  colorPiecesBB[BLACK] = colorPiecesBB[WHITE] << 48;

  turn = WHITE;
  castling = 0b1111;
  key = 0;
  en_passant = 0;
}
char Board::getCharSq(int square) {
  bitboard mask = 1ULL << square;
  if (!(mask & (colorPiecesBB[WHITE] | colorPiecesBB[BLACK]))) return '-';

  char letter;
  switch (getPieceType(square)) {
    case Queen:
      letter = 'q';
      break;
    case Rook:
      letter = 'r';
      break;
    case Bishop:
      letter = 'b';
      break;
    case Knight:
      letter = 'n';
      break;
    case Pawn:
      letter = 'p';
      break;
    case King:
      letter = 'k';
      break;
    default:
      break;
  }
  if (mask & colorPiecesBB[WHITE]) letter -= 32;
  return letter;
}
bool Board::inCheck() { return getAttackBoard(turn) & getKing((Color)(turn^1)); }

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
  s += turn == WHITE ? "White" : "Black";
  s += " to play!";
  return s;
}

bool Board::operator==(const Board b) {
  for (int i = 0; i < NumPieces; i++) {
    if (pieceTypeBB[i] != b.pieceTypeBB[i]) return false;
  }
  for (int i = 0; i < 2; i++) {
    if (colorPiecesBB[i] != b.colorPiecesBB[i]) return false;
  }
  return true;
}

bool Board::operator!=(const Board b) { return !(*this == b); }

Move Board::moveFromStr(std::string move) {
  Move move_list[256];
  Move* curr = move_list;
  Move* end = getMoveList(move_list);
  while (curr != end) {
    if (move == to_string(*curr)) return *curr;
    curr++;
  }
  return (Move)0;
}