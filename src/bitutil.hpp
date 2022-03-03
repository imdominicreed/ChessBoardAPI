
#define bitboard unsigned long long
#define BIT(sq) (1ULL << (sq))
#define LSB(x) (__builtin_ctzll(x))
#define MSB(x) (63 - __builtin_clzll(x))
#define BITS(x) (__builtin_popcountll(x))
#define POP_BSF(b, x) (b) = LSB(x); (x) &= ~BIT(b);
#define shift(i, n)  if((n)>0) ((i)<<= (n)); else ((i)>>= -(n))
static const bitboard LAST_COLUMN = 0x8080808080808080;
static const bitboard SEVENTH_COLUMN = 0X4040404040404040;
static const bitboard FIRST_COLUMN = 0x0101010101010101;
static const bitboard SECOND_COLUMN = 0x0202020202020202;
static const bitboard SEVENTH_ROW = 0x00FF000000000000;
static const bitboard SECOND_ROW = 0x000000000000FF00;
static const bitboard FIRST_ROW = 0xFF;
static const bitboard LAST_ROW = 0xFF00000000000000;


