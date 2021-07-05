//
// Created by Dominic Reed on 3/29/21.
//
#include "board.h"
#include "bitutil.h"
#include "magic.h"


bb getKing(Board *board, int white) {
    unsigned long long pieces = board->black_pieces | board->white_pieces;
    pieces &= ~( board->pawns | board->bishops | board->rooks | board->knights);
    pieces &= ~(!white ? board->white_pieces : board->black_pieces);
    return  pieces;
}

void single_promo(Move move_list[],int src, int dst, int *moveIndex, int type) {
    move_list[*moveIndex] = make_move(src, dst, type);
    ++*moveIndex;
}
void promote_pawn_moves(Move move_list[],int src, int dst, int *moveIndex) {
    single_promo(move_list, src, dst, moveIndex, QUEEN_PROMO);
    single_promo(move_list, src, dst, moveIndex, KNIGHT_PROMO);
    single_promo(move_list, src, dst, moveIndex, ROOK_PROMO);
    single_promo(move_list, src, dst, moveIndex, BISHOP_PROMO);
}
bb get_black_pawn_moves(bb pieces, bb opp_pieces, Move move_list[],  int src, int *moveIndex) {
    bb mask = 1ULL << src;
    bb attacks = 0;
    bb attack = mask;
    attack >>= 7;
    bb last_first_row = 0xFF000000000000FF;
    if (!(mask & LAST_COLUMN) && attack & opp_pieces) {
        if (last_first_row & attack)
            promote_pawn_moves(move_list, src, src - 7, moveIndex);
        else {
            move_list[*moveIndex] = make_move(src, src - 7, NORMAL_MOVE);
            ++*moveIndex;
            attacks |= attack;
        }
    }
    attack = mask;
    attack >>= 9;
    if (!(mask & FIRST_COLUMN) && attack & opp_pieces) {
        if (last_first_row & attack)
            promote_pawn_moves(move_list, src, src - 9, moveIndex);
        else {
            move_list[*moveIndex] = make_move(src, src - 9, NORMAL_MOVE);
            ++*moveIndex;
            attacks |= attack;
        }
    }
    attack = mask>>8;
    if (!(attack & pieces)) {
        if (last_first_row & attack)
            promote_pawn_moves(move_list, src, src - 8, moveIndex);
        else {
            move_list[*moveIndex] = make_move(src, src - 8, NORMAL_MOVE);
            ++*moveIndex;
            attack >>= 8;
            if ((mask & SEVENTH_ROW) && !(attack & pieces)) {
                move_list[*moveIndex] = make_move(src, src - 16, NORMAL_MOVE);
                ++*moveIndex;
            }
        }
    }
    return attacks;
}
bb get_white_pawn_moves(bb pieces, bb opp_pieces, Move move_list[],  int src, int *moveIndex) {
    bb mask = 1ULL << src;
    bb attacks = 0;
    bb attack = mask;
    attack <<= 7;
    bb last_first_row = 0xFF000000000000FF;
    if (!(mask & FIRST_COLUMN) && attack & opp_pieces) {
        if (last_first_row & attack)
            promote_pawn_moves(move_list, src, src + 7, moveIndex);
        else {
            move_list[*moveIndex] = make_move(src, src + 7, NORMAL_MOVE);
            ++*moveIndex;
            attacks |= attack;
        }
    }
    attack = mask;
    attack <<= 9;
    if (!(mask & LAST_COLUMN) && attack & opp_pieces) {
        if (last_first_row & attack)
            promote_pawn_moves(move_list, src, src + 9, moveIndex);
        else {
            move_list[*moveIndex] = make_move(src, src + 9, NORMAL_MOVE);
            ++*moveIndex;
            attacks |= attack;
        }
    }
    attack = mask<<8;
    if ( !(attack & pieces)) {
        if (last_first_row & attack)
            promote_pawn_moves(move_list, src, src + 8, moveIndex);
        else {
            move_list[*moveIndex] = make_move(src, src + 8, NORMAL_MOVE);
            ++*moveIndex;
            attack <<= 8;
            if ((mask & SECOND_ROW) && !(attack & pieces)) {
                move_list[*moveIndex] = make_move(src, src + 16, NORMAL_MOVE);
                ++*moveIndex;
            }
        }
    }
    return attacks;
}
void get_castling_moves(Board *board, Move *move_list, int index, int *moveIndex) {
    int castleBit = board->white ? 1 : 4;
    unsigned long long mask = 1ULL << index;
    unsigned long long pieces = board->black_pieces | board->white_pieces;
    if (castleBit & board->castling) {
        if (!((mask <<1) & pieces) && !((mask <<2) & pieces)) {
            move_list[*moveIndex] = make_move(index, index + 2, CASTLING);
            ++*moveIndex;
        }
    }
    castleBit<<=1;
    if (castleBit & board->castling) {
        if (!((mask >>1) & pieces) && !((mask >>2) & pieces) && !((mask >>3) & pieces)) {
            move_list[*moveIndex] = make_move(index, index - 2, CASTLING);;
            ++*moveIndex;
        }
    }
}
int get_moves(Move move_list[],  bb attacks, int src, int *moveIndex) {
    int dst;
    while (attacks){
        POP_BSF(dst, attacks);
        move_list[*moveIndex] = make_move(src, dst, NORMAL_MOVE);
        ++*moveIndex;
    }
    return 1;
}


int get_move_list(Board *board, Move *move_list) {
    //implement promotion, castling, and checks
    if (!getKing(board, board->white))return 0;
    bb all_pieces = board->white_pieces | board->black_pieces;
    bb king = getKing(board, !board->white);
    bb mypieces = board->white ? board->white_pieces : board-> black_pieces;
    int list_index = 0;
    bb pieces = mypieces & board->bishops;
    int src;
    while (pieces) {
        POP_BSF(src, pieces);
        bb mask =  get_bishop_board(src , all_pieces);
        if (mask & king) return 0;
        get_moves(move_list, ~mypieces & get_bishop_board(src , all_pieces), src, &list_index);
    }
    pieces = mypieces & board->rooks;
    while (pieces) {
        POP_BSF(src, pieces);
        bb mask =  ~mypieces & get_rook_board(src , all_pieces);
        if (mask & king) return 0;
        get_moves(move_list, mask, src, &list_index);
    }
    pieces = mypieces & board->knights;
    while (pieces) {
        POP_BSF(src, pieces);
        bb mask = ~mypieces &  get_knight_attacks(src);
        if (mask & king) return 0;
        get_moves(move_list,~mypieces &  mask, src, &list_index);
    }

    pieces = mypieces & board->pawns;
    bb op_pieces = ~mypieces & all_pieces;
    while (pieces) {
        POP_BSF(src, pieces);
        bb attacks = board->white ? get_white_pawn_moves(all_pieces, op_pieces, move_list, src, &list_index) : get_black_pawn_moves(all_pieces, op_pieces, move_list, src, &list_index);
        if (attacks & king) return 0;
    }

    pieces = getKing(board, board->white);
    src = LSB(pieces);
    POP_BSF(src, pieces);
    bb mask =~mypieces &  get_king_attacks(src);
    if (mask & king) return 0;
    get_moves(move_list,mask, src, &list_index);
    get_castling_moves(board, move_list, src, &list_index);
    return list_index;
}

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
    bb *pieces = board.white ? &board.white_pieces : &board.black_pieces;
    if ((getKing(&board, board.white) & from) && (move->from-move->to == 2 || move->from-move->to == -2)) {
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
    if (from & board.bishops) {
        move_piece(&board.bishops, move->from, move->to);
    }
    if (from & board.pawns) {
        make_empty_square(from, &board);
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
void mirror(bb *board, int shift) {
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
}
char get_char_sq(int square, Board *board) {
    unsigned long long mask = 1ULL << square;
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
    for (int i = 0; i < 64; ++i) {
        if (!(i % 8))
            printf("\n");
        printf("%c ", get_char_sq(i, board));
    }
    printf("\n");
}
