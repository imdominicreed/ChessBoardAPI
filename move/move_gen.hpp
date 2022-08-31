#pragma once
#include "../board/board.hpp"
#include "../magic/magic.hpp"
#include "../move/move.hpp"
#include "../util/bitutil.hpp"

struct Move;
struct Board;

bitboard get_king(Board *board, int white);
int get_move_list(Board *board, Move *move_list);
bitboard get_attack_board(Board *board, bool white);