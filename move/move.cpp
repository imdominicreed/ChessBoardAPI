#include "move.hpp"

#include <sstream>

Move make_move(int from, int to, MoveType type, bool capture) {
  Move move;
  move.move_type = type;
  move.from = from;
  move.to = to;
  move.capture = capture;
  return move;
}
char get_char(int location) { return 'a' + (location % 8); }
int get_rank(int location) { return 1 + location / 8; }

std::string Move::toString() {
  std::stringstream s;

  s << get_char(from) << get_rank(from) << get_char(to) << get_rank(to);
  if (move_type == MoveType::kQueenPromo) s << 'q';
  if (move_type == MoveType::kRookPromo) s << 'r';
  if (move_type == MoveType::kBishopPromo) s << 'b';
  if (move_type == MoveType::kKnightPromo) s << 'n';
  return s.str();
}

int get_sq(char *string, int index) {
  return (string[0 + index] - 'a') + (8 * (string[1 + index] - '1'));
}
int str_cmp(char *string1, char *string2) {
  for (int i = 0; i < 4; i++) {
    if (string1[i] != string2[i]) return 0;
  }
  if (string1[4] != '\0' && string1[4] != string2[4]) return 0;
  return 1;
}

/** returns move from string */
Move Board::moveFromStr(char string[5]) {
  Move move_list[256];
  int moves = getMoveList(move_list);
  for (int i = 0; i < moves; i++) {
    if (str_cmp(string, (char *)move_list[i].toString().c_str()))
      return move_list[i];
  }
  // if this occurs it is an error
  return move_list[0];
}

int cmp_move(const void *a, const void *b) {
  Move *a1 = (Move *)a;
  Move *b1 = (Move *)b;
  if (get_char(a1->from) != get_char(b1->from))
    return get_char(a1->from) - get_char(b1->from);
  if (get_rank(a1->from) != get_rank(b1->from))
    return get_rank(a1->from) - get_rank(b1->from);
  if (get_char(a1->to) != get_char(b1->to))
    return get_char(a1->to) - get_char(b1->to);
  return get_rank(a1->from) - get_rank(b1->from);
}
