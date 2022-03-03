//
// Created by domin on 7/31/2021.
//

#ifndef UNTITLED_MOVE_GEN_H
#define UNTITLED_MOVE_GEN_H

#endif //UNTITLED_MOVE_GEN_H
#include "board.hpp"
#include "bitutil.hpp"
#include "move.hpp"

bitboard get_king(Board *board, int white);
int get_move_list(Board *board, Move *move_list);
bitboard get_attack_board(Board *board, bool white);