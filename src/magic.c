//
// Created by Dominic Reed on 4/15/21.
//
#include "magic.h"
#include "rays.h"
#include "bitutil.h"
#include "stdio.h"
bb lastColumn(bb location) {
    return location & LAST_COLUMN;
}
bb firstColumn(bb location) {
    return location & FIRST_COLUMN;
}
bb secondColumn(bb location) {
    return  location & SECOND_COLUMN;
}
bb seventhColumn(bb location) {
    return location & SEVENTH_COLUMN;
}
bb secondRow(bb location) {
    return location & SECOND_ROW;
}
bb seventhRow(bb location) {
    return location & SEVENTH_ROW;
}

int knightSecondColumnCheck(bb location, int move) {
    /* printf("second column %d : %d\n", move, ((firstColumn(location) ||(secondColumn(location))) && ((move == -10 || move == 6))));
     printf("second last %d : %d\n", move, ((lastColumn(location) ||seventhColumn(location)) && (move == -6 || move == 10))); */
    return (((firstColumn(location) ||secondColumn(location)) && (move == -10 || move == 6))
            || ((lastColumn(location) ||seventhColumn(location)) && (move == -6 || move == 10)));
}
int knightFirstColumnCheck(bb location, int move) {
    /*printf("first column: %d\n",(firstColumn(location) && ((move == -17 || move == 15))));
    printf(" last: %d\n", (lastColumn(location) && (move == 17 || move == -15)));*/
    return ((firstColumn(location) && (move == -17 || move == 15)) || (lastColumn(location) && (move == 17 || move == -15)));
}
int knightOutOfBounds(bb location, int move) {
    return knightFirstColumnCheck(location, move) || knightSecondColumnCheck(location, move);
}
int kingOutOfBounds(bb location, int move) {
    return (firstColumn(location) && (move == 1 || move == 9 || move == -7))
           || (lastColumn(location) && (move == -1 || move == -9 || move ==7));
}

void lookup_table_jumping_setup(const int vectors[], bb look_up[]){
    bb mask = 1;
    int in = 0;
    while (mask) {
        bb attacks = 0;
        for (int i = 0; i < 8; ++i) {
            int vector = vectors[i];
            bb dst = mask;
            shift(dst, vector);
            if (dst == 0 || knightOutOfBounds(mask, vector) || kingOutOfBounds(mask, vector)) continue;
            attacks |= dst;
        }
        look_up[in] = attacks;
        mask <<=1;
        in++;
    }
}
void lookup_rook_setup() {
    for (int sq = 0; sq < 64; ++sq) {
        rook_mask[sq] = (getRay(sq, NORTH) & ~LAST_ROW)
                |(getRay(sq, SOUTH) & ~FIRST_ROW)
                | (getRay(sq, EAST) & ~LAST_COLUMN)
                | (getRay(sq, WEST) & ~FIRST_COLUMN);
    }
}
void lookup_bishop_setup() {
    bb edges = (FIRST_COLUMN | LAST_COLUMN | FIRST_ROW | LAST_ROW);
    for (int sq = 0; sq < 64; ++sq) {
        bb debug = getRay(sq, NORTH_EAST);
        bishop_mask[sq] = (getRay(sq, NORTH_WEST) |getRay(sq, SOUTH_WEST)
                        | getRay(sq, NORTH_EAST) | getRay(sq, SOUTH_EAST))& ~edges;
    }
}
bb getBlockers(int index, bb mask) {
    bb blockers = 0;
    int num_bits = BITS(mask);
    for (int i = 0; i < num_bits; ++i) {
        int bit_in;
        POP_BSF(bit_in, mask);
        if (index & (1 << i))
            blockers |= (1ULL <<bit_in);
    }
    return blockers;
}
bb getRookAttacksMagic(int sq, bb blockers) {
    bb attacks = 0;
    attacks |= getRay(sq, NORTH);
    if(getRay(sq, NORTH) & blockers)
        attacks &= ~(getRay(LSB(getRay(sq, NORTH) & blockers), NORTH));
    attacks |= getRay(sq, SOUTH);
    if (getRay(sq, SOUTH) & blockers)
        attacks &= ~(getRay(MSB(getRay(sq, SOUTH) & blockers), SOUTH));

    attacks |= getRay(sq, EAST);
    if (getRay(sq, EAST) & blockers)
        attacks &= ~(getRay(LSB(getRay(sq, EAST) & blockers), EAST));

    attacks |= getRay(sq, WEST);
    if (getRay(sq, WEST) & blockers) {
        attacks &= ~(getRay(MSB(getRay(sq, WEST) & blockers), WEST));
    }


    return attacks;
}
void initRookMagic() {
    for (int sq = 0; sq < 64; ++sq) {
        for (int bits = 0; bits < (1<<rookIndexBits[sq]); ++bits) {
            bb blockers = getBlockers(bits, rook_mask[sq]);
            int key = (blockers*rookMagics[sq]) >> (64- rookIndexBits[sq]);
            bb debug;
            if (key == 2552 && sq == 56 )
                 debug = rook_mask[sq];
            getBlockers(bits, rook_mask[sq]);
            rook_table[sq][(blockers*rookMagics[sq]) >> (64- rookIndexBits[sq])] =getRookAttacksMagic(sq, blockers);
        }
    }
}

bb getBishopAttacksMagic(int sq, bb blockers) {
    bb attacks = 0;

    attacks |= getRay(sq, NORTH_EAST);
    if(getRay(sq, NORTH_EAST) & blockers)
        attacks &= ~(getRay(LSB(getRay(sq, NORTH_EAST) & blockers), NORTH_EAST));

    attacks |= getRay(sq, SOUTH_EAST);
    if (getRay(sq, SOUTH_EAST) & blockers)
        attacks &= ~(getRay(MSB(getRay(sq, SOUTH_EAST) & blockers), SOUTH_EAST));

    attacks |= getRay(sq, NORTH_WEST);
    if (getRay(sq, NORTH_WEST) & blockers)
        attacks &= ~(getRay(LSB(getRay(sq, NORTH_WEST) & blockers), NORTH_WEST));

    attacks |= getRay(sq, SOUTH_WEST);
    if (getRay(sq, SOUTH_WEST) & blockers)
        attacks &= ~(getRay(MSB(getRay(sq, SOUTH_WEST) & blockers), SOUTH_WEST));

    return attacks;
}
void initBishopMagic() {
    for (int sq = 0; sq < 64; ++sq) {
        for (int bits = 0; bits < 1 << bishopIndexBits[sq]; ++bits) {
            bb blockers = getBlockers(bits, bishop_mask[sq]);
            int key = (blockers * bishopMagics[sq]) >> (64 - bishopIndexBits[sq]);
            if (sq == 5 && key == 13)
                printf("here");
            bishop_table[sq][(blockers * bishopMagics[sq]) >> (64 - bishopIndexBits[sq])] = getBishopAttacksMagic(sq, blockers);
        }
    }
}
void init_tables() {
    init_rays();
    lookup_table_jumping_setup(knights_vector, knight_mask);
    lookup_table_jumping_setup(king_vector, king_mask);
    lookup_rook_setup();
    lookup_bishop_setup();
    initRookMagic();
    initBishopMagic();
}
bb get_bishop_board(int sq, bb blockers) {
    bb mask =  bishop_mask[sq];
    blockers &= bishop_mask[sq];
    int key = (blockers * bishopMagics[sq]) >> (64 - bishopIndexBits[sq]);
    return bishop_table[sq][key];
}
bb get_bishop_attacks(int sq) {
    return  bishop_mask[sq];
}
bb get_rook_attacks(int sq) {
    return  rook_mask[sq];
}
bb get_knight_attacks(int sq) {
    return knight_mask[sq];
}
bb get_king_attacks(int sq) {
    return king_mask[sq];
}
bb get_rook_board(int sq, bb blockers) {
    blockers &= rook_mask[sq];
    int key = (blockers*rookMagics[sq]) >> (64-rookIndexBits[sq]);
    return rook_table[sq][key];
}

