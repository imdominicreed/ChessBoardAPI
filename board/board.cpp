#include "board.hpp"
Zorbist z;

uint64_t Zorbist::random() {
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
  if (en[0] != '-') ret.en_passant = (en[0] - 'a') + ((en[1] - '1') * 8);
  ret.key = 0;
  return ret;
}
inline void Board::makeEmptySquare(int sq, PieceType piece, Color color) {
  updateHash(sq, piece, color);
  bitboard mask = 1ULL << sq;
  pieceTypeBB[piece] &= ~mask;
  colorPiecesBB[color] &= ~mask;
}

void Board::updateHash(int sq, PieceType piece, Color color) {
  key ^= z.table[sq][piece + (color == WHITE ? 0 : 6)];
}

void Board::updateEnpassantHash(int src) { key ^= z.en_passant[src % 8]; }

inline void Board::move_piece(int from, int to, PieceType piece) {
  updateMoveHash(from, to, piece);
  colorPiecesBB[turn] ^= 1ULL << from;
  colorPiecesBB[turn] |= 1ULL << to;
  pieceTypeBB[piece] ^= 1ULL << from;
  pieceTypeBB[piece] |= 1ULL << to;
}

void Board::updateMoveHash(int src, int dst, PieceType piece) {
  updateHash(src, piece, turn);
  updateHash(dst, piece, turn);
}

PieceType Board::getPieceType(int sq) {
  bitboard mask = 1ULL << sq;
  if (pieceTypeBB[Pawn] & mask) return Pawn;
  if (pieceTypeBB[Knight] & mask) return Knight;
  if (pieceTypeBB[Rook] & mask) return Rook;
  if (pieceTypeBB[Queen] & mask) return Queen;
  if (pieceTypeBB[Bishop] & mask) return Bishop;
  if (pieceTypeBB[King] & mask) return King;
  return None;
}

void Board::createPiece(int sq, PieceType piece) {
  updateHash(sq, piece, turn);
  pieceTypeBB[piece] |= 1ULL << sq;
  colorPiecesBB[turn] |= 1ULL << sq;
}

void Board::undoMove(UndoMove move) {
  // move_list.pop_back();
  int to_sq = to(move.move);
  int from_sq = from(move.move);
  MoveType move_type = type(move.move);

  turn = (Color)(turn ^ 1);
  en_passant = move.en_passant_sq;
  castling = move.castling_rights;

  if (move_type >= KnightPromotion) {
    makeEmptySquare(to_sq, getPieceType(to_sq), turn);
    createPiece(from_sq, Pawn);
  } else if (is_castle(move.move)) {
    move_piece(to_sq, from_sq, King);
    move_piece(to_sq + (KingCastle == move_type ? -1 : 1),
               to_sq + (KingCastle == move_type ? 1 : -2), Rook);
  } else {
    move_piece(to_sq, from_sq, getPieceType(to_sq));
  }

  turn = (Color)(turn ^ 1);

  if (is_capture(move.move)) {
    createPiece(move_type == EnpassantCapture ? to_sq + (BLACK == turn ? -8 : 8)
                                              : to_sq,
                move.captured_piece);
  }
  turn = (Color)(turn ^ 1);
  key = move.key;
}

UndoMove Board::doMove(Move move) {
  // move_list.push_back(to_string(move));
  UndoMove undo;
  undo.move = move;
  undo.captured_piece = (PieceType)-1;
  undo.castling_rights = castling;
  undo.en_passant_sq = en_passant;
  undo.key = key;

  int to_sq = to(move);
  int from_sq = from(move);

  MoveType move_type = type(move);
  Color op_turn = (Color)(turn ^ 1);
  // if(en_passant) key ^= z.en_passant[en_passant % 8];
  en_passant = 0;
  PieceType piece;
  int castling_before;
  if (is_capture(move)) {
    castling_before = castling;
    if (to_sq == Square::A1)
      castling &= ~WHITE_QUEEN;
    else if (to_sq == Square::H1)
      castling &= ~WHITE_KING;
    else if (to_sq == Square::A8)
      castling &= ~BLACK_QUEEN;
    else if (to_sq == Square::H8)
      castling &= ~BLACK_KING;
    key ^= z.castling[~castling &
                      castling_before];  // before 1111 after its 1100 so 0011
  }

  switch (move_type) {
    case Capture: {
      PieceType capture_piece = getPieceType(to_sq);
      undo.captured_piece = capture_piece;
      makeEmptySquare(to_sq, capture_piece, op_turn);
    }
      [[fallthrough]];
    case QuietMove:
      piece = getPieceType(from_sq);
      move_piece(from_sq, to_sq, piece);
      // removes castling
      castling_before = castling;
      if (from_sq == Square::E1) castling &= ~WHITE_CASTLING;
      if (from_sq == Square::E8) castling &= ~BLACK_CASTLING;
      if (from_sq == Square::A1) castling &= ~WHITE_QUEEN;
      if (from_sq == Square::H1) castling &= ~WHITE_KING;
      if (from_sq == Square::A8) castling &= ~BLACK_QUEEN;
      if (from_sq == Square::H8) castling &= ~BLACK_KING;
      key ^= z.castling[~castling & castling_before];
      break;

    case DoublePawnPush:
      move_piece(from_sq, to_sq, Pawn);
      en_passant = turn == WHITE ? to_sq - 8 : to_sq + 8;
      key ^= z.en_passant[to_sq % 8];
      break;
    case KingCastle:
      move_piece(from_sq, to_sq, King);
      move_piece(to_sq + 1, to_sq - 1, Rook);
      castling &= turn == WHITE ? BLACK_CASTLING : WHITE_CASTLING;
      break;
    case QueenCastle:
      move_piece(from_sq, to_sq, King);
      move_piece(to_sq - 2, to_sq + 1, Rook);
      castling &= turn == WHITE ? BLACK_CASTLING : WHITE_CASTLING;
      break;
    case EnpassantCapture:
      undo.captured_piece = Pawn;
      makeEmptySquare(to_sq + (BLACK == turn ? 8 : -8), Pawn,
                      (Color)(turn ^ 1));
      move_piece(from_sq, to_sq, Pawn);
      break;

    case KnightPromotionCapture:
      undo.captured_piece = getPieceType(to_sq);
      makeEmptySquare(to_sq, undo.captured_piece, (Color)(turn ^ 1));
      [[fallthrough]];
    case KnightPromotion:
      makeEmptySquare(from_sq, Pawn, turn);
      createPiece(to_sq, Knight);
      break;

    case QueenPromotionCapture:
      undo.captured_piece = getPieceType(to_sq);
      makeEmptySquare(to_sq, undo.captured_piece, (Color)(turn ^ 1));
      [[fallthrough]];
    case QueenPromotion:
      makeEmptySquare(from_sq, Pawn, turn);
      createPiece(to_sq, Queen);
      break;

    case RookPromotionCapture:
      undo.captured_piece = getPieceType(to_sq);
      makeEmptySquare(to_sq, undo.captured_piece, (Color)(turn ^ 1));
      [[fallthrough]];
    case RookPromotion:
      makeEmptySquare(from_sq, Pawn, turn);
      createPiece(to_sq, Rook);
      break;

    case BishopPromotionCapture:
      undo.captured_piece = getPieceType(to_sq);
      makeEmptySquare(to_sq, undo.captured_piece, (Color)(turn ^ 1));
      [[fallthrough]];
    case BishopPromotion:
      makeEmptySquare(from_sq, Pawn, turn);
      createPiece(to_sq, Bishop);
      break;
  }

  turn = (Color)(turn ^ 1);
  key ^= z.turn;
  return undo;
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
bool Board::inCheck() {
  return getAttackBoard(turn) & getKing((Color)(turn ^ 1));
}

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