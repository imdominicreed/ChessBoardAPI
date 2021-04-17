//
// Created by Dominic Reed on 3/29/21.
//
#include "board.h"
#include "bitutil.h"
#include "magic.h"
bb getKing(struct Board *board, int white) {
    unsigned long long pieces = board->black_pieces | board->white_pieces;
    pieces &= ~( board->pawns | board->bishops | board->rooks | board->knights);
    pieces &= ~(!white ? board->white_pieces : board->black_pieces);
    return  pieces;
}

int getSlidingMoveList(struct Board *board, struct Move move_list[], const int vectors[], bb src, bb attack, int index, int *moveIndex) {
    bb king = getKing(board, !board->white);
    bb pieces =(board->white_pieces | board->black_pieces);
    for (int i = 0; i < sliding_size; ++i) {
        int vector = vectors[i];
        bb mask = src;
        shift(mask, vector);
        int to = index+vector;
        while (mask & attack) {

            if (king & mask) return 0;
            if(!mask || ((board->white_pieces & mask) && board -> white)  || ((board->black_pieces & mask) && !board -> white))
                break;
            move_list[*moveIndex] = makeMove(index, to, NORMAL_MOVE);
            ++*moveIndex;
            if((pieces & mask) ) break;
            shift(mask, vector);
            to += vector;
        }
    }
    return 1;
}

int getJumpingPieces(struct Board *board, struct Move move_list[],  bb attacks, int src, int *moveIndex) {
    if (getKing(board, !board->white) & attacks) return 0;
    attacks &= ~(board->white ? board->white_pieces : board->black_pieces);
    int dst;
    while (attacks){
        POP_BSF(dst, attacks);
        move_list[*moveIndex] =  makeMove(src, dst, NORMAL_MOVE);;
        ++*moveIndex;
    }
    return 1;
}
bool pawnOutOfBounds(bb location, int move) {
    return (firstColumn(location) && (move == -7 || move == 9)) ||  (lastColumn(location) && (move == 7 || move == -9));
}

void single_promo(struct Move move_list[],int src, int dst, int *moveIndex, int type) {
    move_list[*moveIndex] = makeMove(src, dst, type);
    ++*moveIndex;
}
void promote_pawn_moves(struct Move move_list[],int src, int dst, int *moveIndex) {
    single_promo(move_list, src, dst, moveIndex, QUEEN_PROMO);
    single_promo(move_list, src, dst, moveIndex, KNIGHT_PROMO);
    single_promo(move_list, src, dst, moveIndex, ROOK_PROMO);
    single_promo(move_list, src, dst, moveIndex, BISHOP_PROMO);
}
bb get_black_pawn_moves(bb pieces, bb opp_pieces, struct Move move_list[],  int src, int *moveIndex) {
    bb mask = 1ULL << src;
    bb attacks = 0;
    bb attack = mask;
    attack >>= 7;
    if (!(mask & LAST_COLUMN) && attack & opp_pieces) {
        move_list[*moveIndex] = makeMove(src, src - 7, NORMAL_MOVE);
        ++*moveIndex;
        attacks |= attack;
    }
    attack = mask;
    attack >>= 9;
    if (!(mask & FIRST_COLUMN) && attack & opp_pieces) {
        move_list[*moveIndex] = makeMove(src, src-9, NORMAL_MOVE);
        ++*moveIndex;
        attacks |= attack;
    }
    attack = mask>>8;
    if (!(attack & pieces)) {
        move_list[*moveIndex] = makeMove(src, src - 8, NORMAL_MOVE);
        ++*moveIndex;
        attack >>=8;
        if ((mask & SEVENTH_ROW) && !(attack & pieces)) {
            move_list[*moveIndex] = makeMove(src, src  -16, NORMAL_MOVE);
            ++*moveIndex;
        }
    }
    return attacks;
}
bb get_white_pawn_moves(bb pieces, bb opp_pieces, struct Move move_list[],  int src, int *moveIndex) {
    bb mask = 1ULL << src;
    bb attacks = 0;
    bb attack = mask;
    attack <<= 7;
    if (!(mask & FIRST_COLUMN) && attack & opp_pieces) {
        move_list[*moveIndex] = makeMove(src, src+  7, NORMAL_MOVE);
        ++*moveIndex;
        attacks |= attack;
    }
    attack = mask;
    attack <<= 9;
    if (!(mask & LAST_COLUMN) && attack & opp_pieces) {
        move_list[*moveIndex] = makeMove(src, src+  9, NORMAL_MOVE);
        ++*moveIndex;
        attacks |= attack;
    }
    attack = mask<<8;
    if ( !(attack & pieces)) {
        move_list[*moveIndex] = makeMove(src, src+  8, NORMAL_MOVE);
        ++*moveIndex;
        attack <<=8;
        if ((mask & SECOND_ROW) && !(attack & pieces))  {
            move_list[*moveIndex] = makeMove(src, src+  16, NORMAL_MOVE);
            ++*moveIndex;
        }
    }
    return attacks;
}
void getCastlingMoveList(struct Board *board, struct Move move_list[],  int index, int *moveIndex) {
    int castleBit = board->white ? 0 : 4;
    unsigned long long mask = 1ULL << index;
    unsigned long long pieces = board->black_pieces | board->white_pieces;
    if (castleBit & board->castling) {
        if (!((mask <<1) & pieces) && !((mask <<2) & pieces)) {
            move_list[*moveIndex] = makeMove(index, index+2, CASTLING);
            ++*moveIndex;
        }
    }
    castleBit<<=1;
    if (castleBit & board->castling) {
        if (!((mask >>1) & pieces) && !((mask >>2) & pieces) && !((mask >>3) & pieces)) {
            move_list[*moveIndex] =  makeMove(index, index-2, CASTLING);;
            ++*moveIndex;
        }
    }
}
int get_moves(struct Move move_list[],  bb attacks, int src, int *moveIndex) {
    int dst;
    while (attacks){
        POP_BSF(dst, attacks);
        move_list[*moveIndex] =  makeMove(src, dst, NORMAL_MOVE);
        ++*moveIndex;
    }
    return 1;
}


int getMoveList(struct Board *board, struct Move move_list[]) {
    //implement promotion, castling, and checks
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
    getCastlingMoveList(board, move_list, src, &list_index);
    return list_index;
}

void emptySquare(unsigned long long mask, struct Board *board) {
    board->white_pieces &= ~mask;
    board->black_pieces &= ~mask;
    board->rooks &= ~mask;
    board->pawns &= ~mask;
    board->bishops &= ~mask;
}
void movePiece(unsigned long long *board, int from, int to) {
    *board ^= 1ULL << from;
    *board ^= 1ULL << to;
}
struct Board doMove(struct Move *move, struct Board board) {
    unsigned long long from = 1ULL << move->from;
    unsigned long long to = 1ULL << move->to;
    if ((getKing(&board, board.white) & from) && (move->from-move->to == 2 || move->from-move->to == -2)) {
        movePiece(board.white ? &board.white_pieces : &board.black_pieces, move->from, move->to);
        movePiece(&board.rooks, (move->to % 8 >4) ? 8 : 0, move->to + ((move->to % 8 >4) ? 1 : -1));
        movePiece(board.white ? &board.white_pieces : &board.black_pieces, (move->to % 8 >4) ? 8 : 0, move->to + ((move->to % 8 >4) ? 1 : -1));
        return board;
    }
    emptySquare(to, &board);
    movePiece(board.white ? &board.white_pieces : &board.black_pieces, move->from, move->to);
    if (from & board.rooks) {
        movePiece(&board.rooks, move->from, move->to);
        int castling = 1<< (((from %8) < 4) ? 1 : 2);
        castling <<= board.white ? 0 : 2;
        board.castling &= ~castling;
    }
    if (from & board.bishops) {
        movePiece(&board.pawns, move->from, move->to);
    }
    if (from & board.pawns) {
        movePiece(&board.pawns, move->from, move->to);
    }
    if (from & board.knights) {
        movePiece(&board.knights, move->from, move->to);
    }
    board.white = !board.white;
    return board;
}
void mirror(bb *board, int shift) {
    *board |= (*board << shift);
}

void startBoard(struct Board *board) {
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
char getChar(int square, struct Board *board) {
    unsigned long long mask = 1L << square;
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
void printBoard(struct Board *board) {
    for (int i = 0; i < 64; ++i) {
        if (!(i % 8))
            printf("\n");
        printf("%c ", getChar(i, board));
    }
    printf("\n");
}
