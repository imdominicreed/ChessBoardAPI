//
// Created by Dominic Reed on 4/15/21.
//
#include "rays.h"
#include "bitutil.h"
unsigned long long RAYS[8][64];




 bb west(bb mask, int shift) {
    bb attacks = 0;
    shift(mask, shift);
     while (mask && !(mask & FIRST_COLUMN)){
        attacks |= mask;
        shift(mask,shift);
    }
    return attacks;
}
 bb east(bb mask, int shift) {
    bb attacks = 0;
    shift(mask, shift);
     while (mask && !(mask & LAST_COLUMN)){
        attacks |= mask;
        shift(mask,shift);
    }
    return attacks;
}
int col(int sq) {
    return sq%8;
}
int row(int sq) {
    return sq/8;
}
void init_rays() {
    for (int sq = 0; sq< 64; sq++) {
        RAYS[WEST][sq] = (1ULL << sq) - (1ULL << (sq & 56));
        RAYS[EAST][sq] = 2 * ((1ULL << (sq | 7)) - (1ULL << sq));
        RAYS[SOUTH][sq] =  0x0080808080808080ULL >> (63-sq);
        RAYS[NORTH][sq] = 0x0101010101010100ULL << sq;
        bb mask = 1ULL<< sq;
        RAYS[NORTH_WEST][sq] = west(mask, 9);
        RAYS[NORTH_EAST][sq] = east(mask, 7);
        RAYS[SOUTH_WEST][sq] = west(mask, -7);
        RAYS[SOUTH_EAST][sq] = east(mask, -9);

    }
}
unsigned long long getRay(int sq, int dir) {
    return RAYS[dir][sq];
}