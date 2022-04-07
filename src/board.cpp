#include "board.hpp"
#include <algorithm>
#include "magic.hpp"
#include "move_gen.hpp"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include<iostream>
#include<time.h>

Zorbist z;

unsigned long long Zorbist::random() {
	return (0xFFFFLL & rand()) | ((0xFFFFLL & rand()) << 16) | ((0xFFFFLL & rand()) << 32) | ((0xFFFFLL & rand()) << 48);
}

Zorbist::Zorbist() {
	std::cerr << "table contructed" << std::endl;
	std::srand(time(nullptr));
	for(int i = 0; i < 64; i++) {
		for(int j = 0; j < 12; j++) {
			table[i][j] = random();
		}
	}
	turn = random();
}



char *strrev(char *str)
{
    int i = 16 - 1, j = 0;

    char ch;
    while (i > j)
    {
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
        i--;
        j++;
    }
    return str;
}
Board* import_fen(char* str) {
    Board* ret = (Board*) calloc(1, sizeof(Board));
    bitboard mask = 1ULL << 63;
    char* copy = strdup(str);
    const char del[2] = "/";
    char* row = strtok(copy, del);
    char* last = NULL;
    while (row != NULL) {
        int end = strlen(row);       
        mask >>= 8;
        for(int i = 0; i < end; i++) {
            if(row[i] == ' ') break;
            if(row[i] - '0' < 9) {
                mask <<= row[i]-'0';
                if(!mask) mask = 1ULL << row[i]-'0'-1;
            } else {
                mask <<= 1;
                if(!mask)  mask = 1;
                if(row[i] == 'r' ||  row[i] == 'R' || row[i] == 'Q' || row[i] == 'q') 
                    ret->rooks |= mask;
                if(row[i] == 'b' ||  row[i] == 'B' || row[i] == 'Q' || row[i] == 'q') 
                    ret->bishops |= mask;
                else if(row[i] == 'n' || row[i] == 'N')
                    ret->knights |= mask;
                else if(row[i] == 'p' || row[i] == 'P')
                    ret->pawns |= mask;
                if(row[i]- 'A' < 26) ret->white_pieces |= mask;
                else ret->black_pieces |= mask;
            }
        }
        mask >>= 8;
        last = (char*) realloc(last, strlen(row)+1);
        strncpy(last, row, strlen(row)+1);
        row = strtok(NULL, del);
    }        

    char* turn = index(last, ' ')+1;
    ret->white = turn[0] == 'w';
    char* castling = index(turn, ' ')+1;
    int i =0;
    while(castling[i] != ' ') {
        if(castling[i] == 'k') ret->castling |= 0b0100;
        if(castling[i] == 'K') ret->castling |= 0b0001;
        if(castling[i] == 'Q') ret->castling |= 0b0010;
        if(castling[i] == 'q') ret->castling |= 0b1000;
        i++;
    }
    char* en = castling +i +1;
    if(en[0] != '-') ret->en_passant = (1ULL << get_sq(en, 0));
	ret->key = 0;
    return ret;

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

int get_hash_number(Board* b, int curr) {
	int offset = 0;
	unsigned long long mask = 1ULL << curr;
	if(mask & b->black_pieces) offset = 6;
	if(mask & b->pawns) return offset + 1;
	if(mask & b->knights) return offset + 4;
	if(mask & b->bishops) offset += 2;
	if(mask & b->rooks) offset += 3;
	return offset;
}

void update_hash(Board* b, int from, int to) {
	int piece = get_hash_number(b, from);
	b->key ^= z.table[from][piece];
	b->key ^= z.table[to][piece];
	if((1ULL << to) & (b->white_pieces | b->black_pieces))
		b->key ^= z.table[to][get_hash_number(b, to)];
	b->key ^= z.turn;
}

Board do_move(Move *move, Board board) {
	update_hash(&board, move->from, move->to);
    unsigned long long from = 1ULL << move->from;
    unsigned long long to = 1ULL << move->to;
    board.en_passant = 0;
    bitboard *pieces = board.white ? &board.white_pieces : &board.black_pieces;
    bitboard king = get_king(&board, board.white);
     if (king & from)  board.castling &= board.white ? 0b1100 : 0b11;
    if ((king& from) && (move->from-move->to == 2 || move->from-move->to == -2)) {
        move_piece(pieces, move->from, move->to);
        move_piece(&board.rooks, (8*(move->to/8)) + ((move->to % 8 > 4) ? 7 : 0), move->to + ((move->to % 8 < 4) ? 1 : -1));
        move_piece(pieces, (8*(move->to/8)) +((move->to % 8 > 4) ? 7 : 0), move->to + ((move->to % 8 < 4) ? 1 : -1));
        board.white = !board.white;
        return board;
    }
    make_empty_square(to, &board);
    move_piece(board.white ? &board.white_pieces : &board.black_pieces, move->from, move->to);
    if (from & board.rooks) {
        move_piece(&board.rooks, move->from, move->to);
        if(!(from & board.bishops)) {
            int castling = 1 << (((move->from %8) < 4) ? 1 : 0);
            castling <<= board.white ? 0 : 2;
            board.castling &= ~castling;
        }
    }
    if (from & board.bishops) {
        move_piece(&board.bishops, move->from, move->to);
    }
    if (from & board.pawns) {
        make_empty_square(from, &board);
        if(move->en_passant) {
            make_empty_square(1ULL << move->to + (board.white ? -8 : 8 ), &board);
        }
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
	board->key = 0;
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
    if (mask & board->white_pieces)
     letter -= 32;
    return letter;
}
bool in_check(Board *board) {
    return get_attack_board(board, board->white) & get_king(board, !board->white);
}
std::string printBoard(Board *board) {
    std::string s;
    std::string rev;
    for (int i = 0; i <= 64; ++i) {
        if (!(i % 8) && i) {
	    std::reverse(rev.begin(), rev.end());
            s += rev + '\n';
	    rev = "";
        }
	rev += get_char_sq(i, board);
        rev += ' ';
    }
    return s;
}
