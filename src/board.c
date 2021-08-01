//
// Created by Dominic Reed on 3/29/21.
//


#include "magic.h"
#include "move_gen.h"
#include <string.h>


void make_empty_square(unsigned long long mask, Board *board) {
    board->white_pieces &= ~mask;
    board->black_pieces &= ~mask;
    board->rooks &= ~mask;
    board->pawns &= ~mask;
    board->bishops &= ~mask;
    board->knights &= ~mask;
}
void move_piece(unsigned long long *board, int from, int to) {
    *board ^= 1ULL << from;
    *board ^= 1ULL << to;
}
Board do_move(Move *move, Board board) {
    unsigned long long from = 1ULL << move->from;
    unsigned long long to = 1ULL << move->to;
    board.en_passant = 0;
    bitboard *pieces = board.white ? &board.white_pieces : &board.black_pieces;
    bitboard king = get_king(&board, board.white);
    if ((king& from) && (move->from-move->to == 2 || move->from-move->to == -2)) {
        move_piece(pieces, move->from, move->to);
        move_piece(&board.rooks, (move->to % 8 > 4) ? 7 : 0, move->to + ((move->to % 8 > 4) ? 1 : -1));
        move_piece(pieces, (move->to % 8 > 4) ? 7 : 0, move->to + ((move->to % 8 > 4) ? 1 : -1));
        return board;
    }
    make_empty_square(to, &board);
    move_piece(board.white ? &board.white_pieces : &board.black_pieces, move->from, move->to);
    if (from & board.rooks) {
        move_piece(&board.rooks, move->from, move->to);
        int castling = 1<< (((from %8) < 4) ? 1 : 2);
        castling <<= board.white ? 0 : 2;
        board.castling &= ~castling;
    }
    if (king & from) {
        board.castling &= board.white ? 0b1100 : 0b11;
    }
    if (from & board.bishops) {
        move_piece(&board.bishops, move->from, move->to);
    }
    if (from & board.pawns) {
        make_empty_square(from, &board);
        if(move->en_passant)
            make_empty_square(1ULL << move->en_passant, &board);
        if ((from>>16) & to || (from<<16) & to) {
            board.en_passant = from>>16 & to ? from>>8 : from << 8;
        }
        switch (move->promo) {
            case QUEEN_PROMO:
                *pieces |= to;
                board.bishops |= to;
                board.rooks |= to;
                break;
            case KNIGHT_PROMO:
                *pieces |= to;
                board.knights |= to;
                break;
            case BISHOP_PROMO:
                *pieces |= to;
                board.bishops |= to;
                break;
            case ROOK_PROMO:
                *pieces |= to;
                board.rooks |= to;
                break;
            default:
                *pieces |= to;
                board.pawns |= to;
                break;
        }
    }
    if (from & board.knights) {
        move_piece(&board.knights, move->from, move->to);
    }
    board.white = !board.white;
    return board;
}
void mirror(bitboard *board, int shift) {
    *board |= (*board << shift);
}

void start_board(Board *board) {
    init_tables();
    board->rooks = 0b10001001;
    mirror(&board->rooks, 56);
    board->bishops = 0b00101100;
    mirror(&board->bishops, 56);
    board->knights = 0b01000010;
    mirror(&board->knights, 56);
    board->pawns = (0b11111111 << 8);
    board->white_pieces =  0b11111111 | board->pawns;
    mirror(&board->pawns, 40);
    board->black_pieces = board->white_pieces <<48;
    board->white = true;
    board->castling = 0b1111;
    board->en_passant = 0;
}
char get_char_sq(int square, Board *board) {
    bitboard mask = 1ULL << square;
    if (!(mask & (board->white_pieces | board->black_pieces)))
        return '-';
    char letter;
    if ((mask & board->bishops) && (mask & board->rooks))
        letter = 'q';
    else if (mask & board->rooks)
        letter = 'r';
    else if(mask & board->bishops)
        letter = 'b';
    else if(mask & board->pawns)
        letter = 'p';
    else if(mask & board->knights)
        letter = 'n';
    else
        letter = 'k';
    if (mask & board->black_pieces)
     letter -= 32;
    return letter;
}
void printBoard(Board *board) {
    char str[17];
    for (int i = 0; i < 64; ++i) {
        if (!(i % 8)) {
            str[16] = 0;
            strrev(str);
            printf("%s\n",str);
        }
        str[2 * (i%8)] = get_char_sq(i, board);
        str[2 * (i%8) +1] = ' ';
    }
    str[16] = 0;
    strrev(str);
    printf("%s\n", str);
}
