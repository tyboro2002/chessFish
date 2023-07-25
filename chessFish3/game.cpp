// chessFish3.cpp : Defines the entry point for the application.
//

#include "game.h"
#include <bitset>
#include <chrono>

constexpr unsigned long long oneRow   = 0b0000000000000000000000000000000000000000000000000000000011111111; // oneRow
constexpr unsigned long long twoRow   = 0b0000000000000000000000000000000000000000000000001111111100000000; // oneRow << 8
constexpr unsigned long long threeRow = 0b0000000000000000000000000000000000000000111111110000000000000000; // oneRow << 16
constexpr unsigned long long fourRow  = 0b0000000000000000000000000000000011111111000000000000000000000000; // oneRow << 24
constexpr unsigned long long fiveRow  = 0b0000000000000000000000001111111100000000000000000000000000000000; // oneRow << 32
constexpr unsigned long long sixRow   = 0b0000000000000000111111110000000000000000000000000000000000000000; // oneRow << 40
constexpr unsigned long long sevenRow = 0b0000000011111111000000000000000000000000000000000000000000000000; // oneRow << 48
constexpr unsigned long long eightRow = 0b1111111100000000000000000000000000000000000000000000000000000000; // oneRow << 56

constexpr unsigned long long A        = 0b1000000010000000100000001000000010000000100000001000000010000000; // A
constexpr unsigned long long B        = 0b0100000001000000010000000100000001000000010000000100000001000000; // A >> 1
constexpr unsigned long long C        = 0b0010000000100000001000000010000000100000001000000010000000100000; // A >> 2
constexpr unsigned long long D        = 0b0001000000010000000100000001000000010000000100000001000000010000; // A >> 3
constexpr unsigned long long E        = 0b0000100000001000000010000000100000001000000010000000100000001000; // A >> 4
constexpr unsigned long long F        = 0b0000010000000100000001000000010000000100000001000000010000000100; // A >> 5
constexpr unsigned long long G        = 0b0000001000000010000000100000001000000010000000100000001000000010; // A >> 6
constexpr unsigned long long H        = 0b0000000100000001000000010000000100000001000000010000000100000001; // A >> 7

constexpr unsigned long long all      = 0b1111111111111111111111111111111111111111111111111111111111111111; // << 8 move row up, >> 8 move row down (left right imposible)
constexpr unsigned long long border   = 0b1111111110000001100000011000000110000001100000011000000111111111; // the border of the field
constexpr unsigned long long corners  = 0b1000000100000000000000000000000000000000000000000000000010000001;
constexpr unsigned long long wkcastle = 0b0000000000000000000000000000000000000000000000000000000000000110;
constexpr unsigned long long wqcastle = 0b0000000000000000000000000000000000000000000000000000000001110000;
constexpr unsigned long long bkcastle = 0b0000011000000000000000000000000000000000000000000000000000000000;
constexpr unsigned long long bqcastle = 0b0111000000000000000000000000000000000000000000000000000000000000;

#define en_passent_target(bord) ((~((((bord->extra & ((1ULL << 13))) >> 13) << 64) - 1)) & ((((1ULL << 63) >> (((bord->extra >> 7) << 58) >> 58)))))
#define white_plays(bord) ((bord->extra &= (1ULL << 18)) != 0)
// bit manipulation macros
#define get_bit(bitboard, index) (bitboard & (1ULL << index))
#define set_bit(bitboard, index) (bitboard |= (1ULL << index))
#define pop_bit(bitboard, index) (get_bit(bitboard, index) ? bitboard ^= (1ULL << index) : 0)


Pieces lastCapturedPiece = NOPIECE;

constexpr unsigned long long rook_magics[64] = {
	0x8a80104000800020ULL,
	0x140002000100040ULL,
	0x2801880a0017001ULL,
	0x100081001000420ULL,
	0x200020010080420ULL,
	0x3001c0002010008ULL,
	0x8480008002000100ULL,
	0x2080088004402900ULL,
	0x800098204000ULL,
	0x2024401000200040ULL,
	0x100802000801000ULL,
	0x120800800801000ULL,
	0x208808088000400ULL,
	0x2802200800400ULL,
	0x2200800100020080ULL,
	0x801000060821100ULL,
	0x80044006422000ULL,
	0x100808020004000ULL,
	0x12108a0010204200ULL,
	0x140848010000802ULL,
	0x481828014002800ULL,
	0x8094004002004100ULL,
	0x4010040010010802ULL,
	0x20008806104ULL,
	0x100400080208000ULL,
	0x2040002120081000ULL,
	0x21200680100081ULL,
	0x20100080080080ULL,
	0x2000a00200410ULL,
	0x20080800400ULL,
	0x80088400100102ULL,
	0x80004600042881ULL,
	0x4040008040800020ULL,
	0x440003000200801ULL,
	0x4200011004500ULL,
	0x188020010100100ULL,
	0x14800401802800ULL,
	0x2080040080800200ULL,
	0x124080204001001ULL,
	0x200046502000484ULL,
	0x480400080088020ULL,
	0x1000422010034000ULL,
	0x30200100110040ULL,
	0x100021010009ULL,
	0x2002080100110004ULL,
	0x202008004008002ULL,
	0x20020004010100ULL,
	0x2048440040820001ULL,
	0x101002200408200ULL,
	0x40802000401080ULL,
	0x4008142004410100ULL,
	0x2060820c0120200ULL,
	0x1001004080100ULL,
	0x20c020080040080ULL,
	0x2935610830022400ULL,
	0x44440041009200ULL,
	0x280001040802101ULL,
	0x2100190040002085ULL,
	0x80c0084100102001ULL,
	0x4024081001000421ULL,
	0x20030a0244872ULL,
	0x12001008414402ULL,
	0x2006104900a0804ULL,
	0x1004081002402ULL,
};

constexpr unsigned long long bishop_magics[64] = {
	0x40040844404084ULL,
	0x2004208a004208ULL,
	0x10190041080202ULL,
	0x108060845042010ULL,
	0x581104180800210ULL,
	0x2112080446200010ULL,
	0x1080820820060210ULL,
	0x3c0808410220200ULL,
	0x4050404440404ULL,
	0x21001420088ULL,
	0x24d0080801082102ULL,
	0x1020a0a020400ULL,
	0x40308200402ULL,
	0x4011002100800ULL,
	0x401484104104005ULL,
	0x801010402020200ULL,
	0x400210c3880100ULL,
	0x404022024108200ULL,
	0x810018200204102ULL,
	0x4002801a02003ULL,
	0x85040820080400ULL,
	0x810102c808880400ULL,
	0xe900410884800ULL,
	0x8002020480840102ULL,
	0x220200865090201ULL,
	0x2010100a02021202ULL,
	0x152048408022401ULL,
	0x20080002081110ULL,
	0x4001001021004000ULL,
	0x800040400a011002ULL,
	0xe4004081011002ULL,
	0x1c004001012080ULL,
	0x8004200962a00220ULL,
	0x8422100208500202ULL,
	0x2000402200300c08ULL,
	0x8646020080080080ULL,
	0x80020a0200100808ULL,
	0x2010004880111000ULL,
	0x623000a080011400ULL,
	0x42008c0340209202ULL,
	0x209188240001000ULL,
	0x400408a884001800ULL,
	0x110400a6080400ULL,
	0x1840060a44020800ULL,
	0x90080104000041ULL,
	0x201011000808101ULL,
	0x1a2208080504f080ULL,
	0x8012020600211212ULL,
	0x500861011240000ULL,
	0x180806108200800ULL,
	0x4000020e01040044ULL,
	0x300000261044000aULL,
	0x802241102020002ULL,
	0x20906061210001ULL,
	0x5a84841004010310ULL,
	0x4010801011c04ULL,
	0xa010109502200ULL,
	0x4a02012000ULL,
	0x500201010098b028ULL,
	0x8040002811040900ULL,
	0x28000010020204ULL,
	0x6000020202d0240ULL,
	0x8918844842082200ULL,
	0x4010011029020020ULL,
};

constexpr unsigned long long Right(unsigned long long num) {
	return num >> 1;
}

constexpr unsigned long long Left(unsigned long long num) {
	return num << 1;
}

constexpr unsigned long long Up(unsigned long long num) {
	return num << 8;
}

constexpr unsigned long long Down(unsigned long long num) {
	return num >> 8;
}

using namespace std;

int countTrailingZeros(unsigned long long number) {
	unsigned long index;
	if (_BitScanForward64(&index, number)) {
		return static_cast<int>(index);
	}
	return 64; // Return 64 if the input number is 0
}


unsigned long long bitmap_all_white_pawns(Board* bord) {
	unsigned long long wpawns = bord->pawn & bord->white; // all positions of white pawns
	unsigned long long doublePawns = (wpawns & twoRow); // all positions of white pawns able to move 2
	unsigned long long nonCaptures = (((doublePawns << 8) | (doublePawns << 16) | (wpawns << 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	unsigned long long captures = ((wpawns & (~border)) << 7 | (wpawns & (~border)) << 9 | (wpawns & H) << 9 | (wpawns & A) << 7); // all capturing moves a pawn can do
	unsigned long long enPassent = (~((((bord->extra & ((1ULL << 13))) >> 13) << 64) - 1)) & ((((1ULL << 63) >> (((bord->extra >> 7) << 58) >> 58)))); // all squares that are able to be en passented
	return (nonCaptures | (captures & bord->black) | (enPassent & captures));
}

unsigned long long bitmap_all_black_pawns(Board* bord) {
	unsigned long long bpawns = bord->pawn & bord->black; // all positions of black pawns
	unsigned long long doublePawns = (bpawns & sevenRow); // all positions of black pawns able to move 2
	unsigned long long nonCaptures = (((doublePawns >> 8) | (doublePawns >> 16) | (bpawns >> 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	unsigned long long captures = ((bpawns & (~border)) >> 7 | (bpawns & (~border)) >> 9 | (bpawns & H) >> 7 | (bpawns & A) >> 9); // all capturing moves a pawn can do
	unsigned long long enPassent = (~((((bord->extra & ((1ULL << 13))) >> 13) << 64) - 1)) & ((((1ULL << 63) >> (((bord->extra >> 7) << 58) >> 58)))); // all squares that are able to be en passented
	return (nonCaptures | (captures & bord->white) | (enPassent & captures));
}

unsigned long long bitmap_all_white_king(Board* bord) {
	unsigned long long wkings = bord->king & bord->white; // all positions of white kings
	unsigned long long all_dirs_non_border = Down((wkings & (~border))) | Up((wkings & (~border))) | Left((wkings & (~border))) | Right((wkings & (~border)));
	unsigned long long all_dirs_non_corner = Right(wkings & A) | Up(wkings & A) | Down(wkings & A) | Left(wkings & H) | Up(wkings & H) | Down(wkings & H) | Up(wkings & oneRow) | Left(wkings & oneRow) | Right(wkings & oneRow) | Down(wkings & eightRow) | Left(wkings & eightRow) | Right(wkings & eightRow);
	unsigned long long empty = ~(bord->white | bord->black);
	unsigned long long castel = ((((wkcastle & empty) == 6) & ((bord->extra >> 17) & 1)) << 1) | ((((wqcastle & empty) == 112) & ((bord->extra >> 16) & 1) & 1) << 5);//| ((bord->extra >> 15) & 1) | ((bord->extra >> 14) & 1);
	return (((all_dirs_non_border | all_dirs_non_corner)& (~bord->white)) |castel);
}

unsigned long long bitmap_all_black_king(Board* bord) {
	unsigned long long bkings = bord->king & bord->black; // all positions of black kings
	unsigned long long all_dirs_non_border = Down((bkings & (~border))) | Up((bkings & (~border))) | Left((bkings & (~border))) | Right((bkings & (~border)));
	unsigned long long all_dirs_non_corner = Right(bkings & A) | Up(bkings & A) | Down(bkings & A) | Left(bkings & H) | Up(bkings & H) | Down(bkings & H) | Up(bkings & oneRow) | Left(bkings & oneRow) | Right(bkings & oneRow) | Down(bkings & eightRow) | Left(bkings & eightRow) | Right(bkings & eightRow);
	unsigned long long empty = ~(bord->white | bord->black);
	unsigned long long castel = (((((bkcastle & empty) == 432345564227567616) & ((bord->extra >> 15) & 1)) << 57)) | (((((bqcastle & empty) == 8070450532247928832) & ((bord->extra >> 14) & 1)) << 61));
	return (((all_dirs_non_border | all_dirs_non_corner) & (~bord->black)) | castel);
}

/*
// Function to generate a bitboard representing all squares a rook can attack from a given position (0 to 63)
unsigned long long bitmap_white_rook(int position, Board* bord) {
	unsigned long long occupied = bord->white | bord->black;
	unsigned long long rookAttacks = 0ULL;
	int row = position / 8;
	int col = position % 8;

	// Generate attacks along the same row (horizontal)
	unsigned long long horizontalMask = eightRow >> (8 * row);
	unsigned long long horizontalAttacks = horizontalMask & ~((1ULL << 63) >> position);

	// Remove the squares that are blocked by other pieces along the row to the right of the rook
	unsigned long long rightOccupied = occupied & (horizontalMask >> (col + 1));
	unsigned long long rightClearMask = rightOccupied - 1ULL;
	horizontalAttacks ^= (horizontalAttacks & rightOccupied) & (horizontalAttacks ^ rightClearMask);

	// Remove the squares that are blocked by other pieces along the row to the left of the rook
	unsigned long long leftOccupied = occupied & (horizontalMask << (8 - col));
	unsigned long long leftClearMask = (leftOccupied - 1ULL) << 1ULL;
	horizontalAttacks ^= (horizontalAttacks & leftOccupied) & (horizontalAttacks ^ leftClearMask);

	// Generate attacks along the same column (vertical)
	unsigned long long verticalMask = A >> col;
	unsigned long long verticalAttacks = verticalMask & ~((1ULL << 63) >> position);

	// Remove the squares that are blocked by other pieces along the column above the rook
	unsigned long long upOccupied = occupied & (verticalMask >> (row + 1));
	unsigned long long upClearMask = rightOccupied - 1ULL;
	verticalAttacks ^= (verticalAttacks & upOccupied) & (verticalAttacks ^ upClearMask);

	// Remove the squares that are blocked by other pieces along the column below the rook
	unsigned long long downOccupied = occupied & (verticalMask << (8 - row));
	unsigned long long downClearMask = (leftOccupied - 1ULL) << 1ULL;
	verticalAttacks ^= (verticalAttacks & downOccupied) & (verticalAttacks ^ downClearMask);

	rookAttacks = horizontalAttacks | verticalAttacks;
	return rookAttacks;
*/

unsigned long long bitmap_white_pawns(int position, Board* bord) {
	unsigned long long wpawns = ((1ULL << 63) >> position); // the square given
	unsigned long long doublePawns = (wpawns & twoRow); // all positions of white pawns able to move 2
	unsigned long long nonCaptures = (((doublePawns << 8) | (doublePawns << 16) | (wpawns << 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	unsigned long long captures = ((wpawns & (~border)) << 7 | (wpawns & (~border)) << 9 | (wpawns & H) << 9 | (wpawns & A) << 7); // all capturing moves a pawn can do
	unsigned long long enPassent = en_passent_target(bord); // all squares that are able to be en passented
	return (nonCaptures | (captures & bord->black) | (enPassent & captures));
}
unsigned long long bitmap_black_pawns(int position, Board* bord) {
	unsigned long long bpawns = ((1ULL << 63) >> position); // the square given
	unsigned long long doublePawns = (bpawns & sevenRow); // all positions of black pawns able to move 2
	unsigned long long nonCaptures = (((doublePawns >> 8) | (doublePawns >> 16) | (bpawns >> 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	unsigned long long captures = ((bpawns & (~border)) >> 7 | (bpawns & (~border)) >> 9 | (bpawns & H) >> 7 | (bpawns & A) >> 9); // all capturing moves a pawn can do
	unsigned long long enPassent = en_passent_target(bord); // all squares that are able to be en passented
	return (nonCaptures | (captures & bord->white) | (enPassent & captures));
}
unsigned long long bitmap_white_king(int position, Board* bord) {
	unsigned long long wkings = ((1ULL << 63) >> position); // the square given
	unsigned long long all_dirs_non_border = Down((wkings & (~border))) | Up((wkings & (~border))) | Left((wkings & (~border))) | Right((wkings & (~border)));
	unsigned long long all_dirs_non_corner = Right(wkings & A) | Up(wkings & A) | Down(wkings & A) | Left(wkings & H) | Up(wkings & H) | Down(wkings & H) | Up(wkings & oneRow) | Left(wkings & oneRow) | Right(wkings & oneRow) | Down(wkings & eightRow) | Left(wkings & eightRow) | Right(wkings & eightRow);
	unsigned long long empty = ~(bord->white | bord->black);
	unsigned long long castel = ((((wkcastle & empty) == 6) & ((bord->extra >> 17) & 1)) << 1) | ((((wqcastle & empty) == 112) & ((bord->extra >> 16) & 1) & 1) << 5);//| ((bord->extra >> 15) & 1) | ((bord->extra >> 14) & 1);
	return (((all_dirs_non_border | all_dirs_non_corner) & (~bord->white)) | castel);
}
unsigned long long bitmap_black_king(int position, Board* bord) {
	unsigned long long bkings = ((1ULL << 63) >> position); // the square given
	unsigned long long all_dirs_non_border = Down((bkings & (~border))) | Up((bkings & (~border))) | Left((bkings & (~border))) | Right((bkings & (~border)));
	unsigned long long all_dirs_non_corner = Right(bkings & A) | Up(bkings & A) | Down(bkings & A) | Left(bkings & H) | Up(bkings & H) | Down(bkings & H) | Up(bkings & oneRow) | Left(bkings & oneRow) | Right(bkings & oneRow) | Down(bkings & eightRow) | Left(bkings & eightRow) | Right(bkings & eightRow);
	unsigned long long empty = ~(bord->white | bord->black);
	unsigned long long castel = (((((bkcastle & empty) == 432345564227567616) & ((bord->extra >> 15) & 1)) << 57)) | (((((bqcastle & empty) == 8070450532247928832) & ((bord->extra >> 14) & 1)) << 61));
	return (((all_dirs_non_border | all_dirs_non_corner) & (~bord->black)) | castel);
}

// rook attacks
unsigned long long bitmap_white_rook(int square, Board* bord){
	square = 63 - square;
	unsigned long long block = bord->white | bord->black;
	// attacks bitboard
	unsigned long long attacks = 0ULL;

	// init files & ranks
	int f, r;

	// init target files & ranks
	int tr = square / 8;
	int tf = square % 8;

	// generate attacks
	for (r = tr + 1; r <= 7; r++)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if (block & (1ULL << (r * 8 + tf))) break;
	}

	for (r = tr - 1; r >= 0; r--)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if (block & (1ULL << (r * 8 + tf))) break;
	}

	for (f = tf + 1; f <= 7; f++)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if (block & (1ULL << (tr * 8 + f))) break;
	}

	for (f = tf - 1; f >= 0; f--)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if (block & (1ULL << (tr * 8 + f))) break;
	}

	// return attack map for bishop on a given square
	return attacks & (~bord->white);
}

// rook attacks
unsigned long long bitmap_black_rook(int square, Board* bord){
	square = 63 - square;
	unsigned long long block = bord->white | bord->black;
	// attacks bitboard
	unsigned long long attacks = 0ULL;

	// init files & ranks
	int f, r;

	// init target files & ranks
	int tr = square / 8;
	int tf = square % 8;

	// generate attacks
	for (r = tr + 1; r <= 7; r++)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if (block & (1ULL << (r * 8 + tf))) break;
	}

	for (r = tr - 1; r >= 0; r--)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if (block & (1ULL << (r * 8 + tf))) break;
	}

	for (f = tf + 1; f <= 7; f++)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if (block & (1ULL << (tr * 8 + f))) break;
	}

	for (f = tf - 1; f >= 0; f--)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if (block & (1ULL << (tr * 8 + f))) break;
	}

	// return attack map for bishop on a given square
	return attacks & (~bord->black);
}

// bishop attacks
unsigned long long bitmap_white_bishop(int square, Board* bord){

	square = 63 - square;
	unsigned long long block = bord->white | bord->black;
	// attack bitboard
	unsigned long long attacks = 0;

	// init files & ranks
	int f, r;

	// init target files & ranks
	int tr = square / 8;
	int tf = square % 8;

	// generate attacks
	for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	// return attack map for bishop on a given square
	return attacks & (~bord->white);
}

// bishop attacks
unsigned long long bitmap_black_bishop(int square, Board* bord){


	square = 63 - square;
	unsigned long long block = bord->white | bord->black;
	// attack bitboard
	unsigned long long attacks = 0;

	// init files & ranks
	int f, r;

	// init target files & ranks
	int tr = square / 8;
	int tf = square % 8;

	// generate attacks
	for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if (block & (1ULL << (r * 8 + f))) break;
	}

	// return attack map for bishop on a given square
	return attacks & (~bord->black);
}

// mask knight attacks
unsigned long long bitmap_white_knight (int square, Board* bord){

	square = 63 - square;
	// attack bitboard
	unsigned long long attacks = 0;

	// piece bitboard
	unsigned long long bitboard = 0ULL;//bord->knight & bord->white ;
	// set piece on bitboard
	set_bit(bitboard, square);

	// generate knight
	if ((bitboard ) & (~H)) attacks |= (bitboard >> 17); //under right
	if ((bitboard ) & (~A)) attacks |= (bitboard >> 15); //under left
	if ((bitboard ) & (~(H|G))) attacks |= (bitboard >> 10); //right under
	if ((bitboard ) & (~(A | B))) attacks |= (bitboard >> 6); //left under
	if ((bitboard ) & (~A)) attacks |= (bitboard << 17); //top left
	if ((bitboard ) & (~H)) attacks |= (bitboard << 15); //top right
	if ((bitboard ) & (~(A | B))) attacks |= (bitboard << 10); //left top
	if ((bitboard ) & (~(H | G))) attacks |= (bitboard << 6); //right top

	// return attack map for knight on a given square
	return attacks & (~bord->white);
}

// mask knight attacks
unsigned long long bitmap_black_knight(int square, Board* bord) {

	square = 63 - square;
	// attack bitboard
	unsigned long long attacks = 0;

	// piece bitboard
	unsigned long long bitboard = 0ULL;//bord->knight & bord->white ;
	// set piece on bitboard
	set_bit(bitboard, square);

	// generate knight
	if ((bitboard) & (~H)) attacks |= (bitboard >> 17); //under right
	if ((bitboard) & (~A)) attacks |= (bitboard >> 15); //under left
	if ((bitboard) & (~(H | G))) attacks |= (bitboard >> 10); //right under
	if ((bitboard) & (~(A | B))) attacks |= (bitboard >> 6); //left under
	if ((bitboard) & (~A)) attacks |= (bitboard << 17); //top left
	if ((bitboard) & (~H)) attacks |= (bitboard << 15); //top right
	if ((bitboard) & (~(A | B))) attacks |= (bitboard << 10); //left top
	if ((bitboard) & (~(H | G))) attacks |= (bitboard << 6); //right top

	// return attack map for knight on a given square
	return attacks & (~bord->black);
}

unsigned long long all_white_attacks(Board* bord) {
	unsigned long long wrook = bord->white & bord->rook;
	unsigned long long wknight = bord->white & bord->knight;
	unsigned long long wbishop = bord->white & bord->bishop;
	unsigned long long wqueen = bord->white & bord->queen;
	unsigned long long wking = bord->white & bord->king;
	unsigned long long wpawn = bord->white & bord->pawn;
	unsigned long long attacks = 0ULL;
	while (wrook) {
		int bitIndex = countTrailingZeros(wrook); // Get the index of the least significant set bit
		attacks |= bitmap_white_rook(63-bitIndex,bord); // Call the corresponding function with the index of the set bit
		wrook &= (wrook - 1); // Clear the least significant set bit
	}
	while (wknight) {
		int bitIndex = countTrailingZeros(wknight); // Get the index of the least significant set bit
		attacks |= bitmap_white_knight(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		wknight &= (wknight - 1); // Clear the least significant set bit
	}
	while (wbishop) {
		int bitIndex = countTrailingZeros(wbishop); // Get the index of the least significant set bit
		attacks |= bitmap_white_bishop(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		wbishop &= (wbishop - 1); // Clear the least significant set bit
	}
	while (wqueen) {
		int bitIndex = countTrailingZeros(wqueen); // Get the index of the least significant set bit
		attacks |= bitmap_white_bishop(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		attacks |= bitmap_white_rook(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		wqueen &= (wqueen - 1); // Clear the least significant set bit
	}
	attacks |= bitmap_all_white_king(bord);
	attacks |= bitmap_all_white_pawns(bord);
	return attacks;
}

unsigned long long all_black_attacks(Board* bord) {
	unsigned long long brook = bord->black & bord->rook;
	unsigned long long bknight = bord->black & bord->knight;
	unsigned long long bbishop = bord->black & bord->bishop;
	unsigned long long bqueen = bord->black & bord->queen;
	unsigned long long bking = bord->black & bord->king;
	unsigned long long bpawn = bord->black & bord->pawn;
	unsigned long long attacks = 0ULL;
	while (brook) {
		int bitIndex = countTrailingZeros(brook); // Get the index of the least significant set bit
		attacks |= bitmap_black_rook(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		brook &= (brook - 1); // Clear the least significant set bit
	}
	while (bknight) {
		int bitIndex = countTrailingZeros(bknight); // Get the index of the least significant set bit
		attacks |= bitmap_black_knight(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		bknight &= (bknight - 1); // Clear the least significant set bit
	}
	while (bbishop) {
		int bitIndex = countTrailingZeros(bbishop); // Get the index of the least significant set bit
		attacks |= bitmap_black_bishop(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		bbishop &= (bbishop - 1); // Clear the least significant set bit
	}
	while (bqueen) {
		int bitIndex = countTrailingZeros(bqueen); // Get the index of the least significant set bit
		attacks |= bitmap_black_bishop(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		attacks |= bitmap_black_rook(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		bqueen &= (bqueen - 1); // Clear the least significant set bit
	}
	attacks |= bitmap_all_black_king(bord);
	attacks |= bitmap_all_black_pawns(bord);
	return attacks;
}

/*
* all moves generating and putting them in a movelist
*/
void white_pawn_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_white_pawns(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63-bitIndex);
		if (m->src - m->dst == 16) {
			m->special = SPECIAL_WPAWN_2SQUARES;
		}else if (m->dst == (63-countTrailingZeros(en_passent_target(bord)))) {
			m->special = SPECIAL_WEN_PASSANT;
			m->capture = m->dst + 8;
		}else {
			m->special = NOT_SPECIAL;
		}
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_pawn_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_black_pawns(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		if (m->dst - m->src == 16) {
			m->special = SPECIAL_BPAWN_2SQUARES;
		}
		else if (m->dst == (63 - countTrailingZeros(en_passent_target(bord)))) {
			m->special = SPECIAL_BEN_PASSANT;
			m->capture = m->dst - 8;
		}
		else {
			m->special = NOT_SPECIAL;
		}
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_rook_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_white_rook(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_rook_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_black_rook(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_knight_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_white_knight(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_knight_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_black_knight(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_bishop_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_white_bishop(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_bishop_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_black_bishop(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_queen_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_white_bishop(position, bord) | bitmap_white_rook(position, bord) ;
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_queen_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_black_bishop(position, bord) | bitmap_black_rook(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_king_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_white_king(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		if (m->dst == G1) {
			m->special = SPECIAL_WK_CASTLING;
		}
		else if (m->dst == C1) {
			m->special = SPECIAL_WQ_CASTLING;
		}else {
			m->special = NOT_SPECIAL;
		}
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_king_moves(int position, MOVELIST* movelist, Board* bord) {
	unsigned long long destinations = bitmap_black_king(position, bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		if (m->dst == G8) {
			m->special = SPECIAL_BK_CASTLING;
		}
		else if (m->dst == C8) {
			m->special = SPECIAL_BQ_CASTLING;
		}
		else {
			m->special = NOT_SPECIAL;
		}
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_moves(MOVELIST* movelist, Board* bord) {
	unsigned long long wrook = bord->white & bord->rook;
	unsigned long long wknight = bord->white & bord->knight;
	unsigned long long wbishop = bord->white & bord->bishop;
	unsigned long long wqueen = bord->white & bord->queen;
	unsigned long long wking = bord->white & bord->king;
	unsigned long long wpawn = bord->white & bord->pawn;
	while (wrook) {
		int bitIndex = countTrailingZeros(wrook); // Get the index of the least significant set bit
		white_rook_moves(63 - bitIndex,movelist,bord); // Call the corresponding function with the index of the set bit
		wrook &= (wrook - 1); // Clear the least significant set bit
	}
	while (wknight) {
		int bitIndex = countTrailingZeros(wknight); // Get the index of the least significant set bit
		white_knight_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		wknight &= (wknight - 1); // Clear the least significant set bit
	}
	while (wbishop) {
		int bitIndex = countTrailingZeros(wbishop); // Get the index of the least significant set bit
		white_bishop_moves(63 - bitIndex, movelist, bord); (63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		wbishop &= (wbishop - 1); // Clear the least significant set bit
	}
	while (wqueen) {
		int bitIndex = countTrailingZeros(wqueen); // Get the index of the least significant set bit
		white_queen_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		wqueen &= (wqueen - 1); // Clear the least significant set bit
	}while (wpawn) {
		int bitIndex = countTrailingZeros(wpawn); // Get the index of the least significant set bit
		white_pawn_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		wpawn &= (wpawn - 1); // Clear the least significant set bit
	}while (wking) {
		int bitIndex = countTrailingZeros(wking); // Get the index of the least significant set bit
		white_king_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		wking &= (wking - 1); // Clear the least significant set bit
	}
}

void black_moves(MOVELIST* movelist, Board* bord) {
	unsigned long long brook = bord->black & bord->rook;
	unsigned long long bknight = bord->black & bord->knight;
	unsigned long long bbishop = bord->black & bord->bishop;
	unsigned long long bqueen = bord->black & bord->queen;
	unsigned long long bking = bord->black & bord->king;
	unsigned long long bpawn = bord->black & bord->pawn;
	while (brook) {
		int bitIndex = countTrailingZeros(brook); // Get the index of the least significant set bit
		black_rook_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		brook &= (brook - 1); // Clear the least significant set bit
	}
	while (bknight) {
		int bitIndex = countTrailingZeros(bknight); // Get the index of the least significant set bit
		black_knight_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		bknight &= (bknight - 1); // Clear the least significant set bit
	}
	while (bbishop) {
		int bitIndex = countTrailingZeros(bbishop); // Get the index of the least significant set bit
		black_bishop_moves(63 - bitIndex, movelist, bord); (63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
		bbishop &= (bbishop - 1); // Clear the least significant set bit
	}
	while (bqueen) {
		int bitIndex = countTrailingZeros(bqueen); // Get the index of the least significant set bit
		black_queen_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		bqueen &= (bqueen - 1); // Clear the least significant set bit
	}while (bpawn) {
		int bitIndex = countTrailingZeros(bpawn); // Get the index of the least significant set bit
		black_pawn_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		bpawn &= (bpawn - 1); // Clear the least significant set bit
	}while (bking) {
		int bitIndex = countTrailingZeros(bking); // Get the index of the least significant set bit
		black_king_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		bking &= (bking - 1); // Clear the least significant set bit
	}
}

bool EvaluateQuick(Board* bord) {
	if ((bord->extra &= (1ULL << 18)) != 0) {
		return (((bord->king & bord->white) & all_black_attacks(bord))) == 0;
	}
	else {
		return (((bord->king & bord->black) & all_white_attacks(bord))) == 0;
	}
}

void GenLegalMoveList(MOVELIST* list, Board* bord){
	int i, j;
	bool okay;
	MOVELIST list2;
	list2.count = 0;

	// Generate all moves, including illegal (e.g. put king in check) moves
	if (white_plays(bord)) {
		white_moves(&list2, bord);
	}else {
		black_moves(&list2, bord);
	}

	// Loop copying the proven good ones
	for (i = j = 0; i < list2.count; i++)
	{
		makeMove(bord, &list2.moves[i]);
		okay = EvaluateQuick(bord);
		popMove(bord, &list2.moves[i]);
		if (okay)
			list->moves[j++] = list2.moves[i];
	}
	list->count = j;
}

// Function to convert 12 sets of 64-bit numbers to a 64-character string
std::string convertTo64CharString(unsigned long long rook, unsigned long long knight, unsigned long long bishop, unsigned long long queen, unsigned long long king, unsigned long long pawn, unsigned long long white, unsigned long long black) {
	std::string result;
	for (int i = 0; i < 64; ++i) {
		uint64_t bitMask = ((1ULL << 63) >> i);
		char representativeChar = '.'; // Default character
		if ((rook & bitMask) != 0) {
			if ((white & bitMask) != 0) { representativeChar = 'R';}
			else if ((black & bitMask) != 0) { representativeChar = 'r';}
		}else if ((knight & bitMask) != 0) {
			if ((white & bitMask) != 0) { representativeChar = 'N'; }
			else if ((black & bitMask) != 0) { representativeChar = 'n'; }
		}else if ((bishop & bitMask) != 0) {
			if ((white & bitMask) != 0) { representativeChar = 'B'; }
			else if ((black & bitMask) != 0) { representativeChar = 'b'; }
		}else if ((queen & bitMask) != 0) {
			if ((white & bitMask) != 0) { representativeChar = 'Q'; }
			else if ((black & bitMask) != 0) { representativeChar = 'q'; }
		}else if ((king & bitMask) != 0) {
			if ((white & bitMask) != 0) { representativeChar = 'K'; }
			else if ((black & bitMask) != 0) { representativeChar = 'k'; }
		}else if ((pawn & bitMask) != 0) {
			if ((white & bitMask) != 0) { representativeChar = 'P'; }
			else if ((black & bitMask) != 0) { representativeChar = 'p'; }
		}
		result.push_back(representativeChar);
	}
	return result;
}

/*
* this function overlays the string with the character for each place where the coresponding bit in bitpatroon is 1 
* (bitpatroon and string always have same length)
*/
void overlay(std::string* str, unsigned long long bitpattern, char character) {
	std::string& myString = *str;
	size_t strLength = myString.length();

	for (size_t i = 0; i < strLength; ++i) {
		if (bitpattern & ((1ULL << 63) >> i)) {
			myString[i] = character;
		}
	}
}

void printBitBoard(unsigned long long bitbord, std::string extra) {
	std::cout << endl;
	std::cout << extra << endl;
	std::string temp = std::bitset<64>(bitbord).to_string();
	std::cout << "8 " << temp.substr(0, 8) << endl;
	std::cout << "7 " << temp.substr(8, 8) << endl;
	std::cout << "6 " << temp.substr(16, 8) << endl;
	std::cout << "5 " << temp.substr(24, 8) << endl;
	std::cout << "4 " << temp.substr(32, 8) << endl;
	std::cout << "3 " << temp.substr(40, 8) << endl;
	std::cout << "2 " << temp.substr(48, 8) << endl;
	std::cout << "1 " << temp.substr(56, 8) << endl;
	std::cout << "  abcdefgh" << endl;
}

void printBoard(Board* bord){
	std::string temp = convertTo64CharString(bord->rook, bord->knight, bord->bishop, bord->queen, bord->king, bord->pawn, bord->white, bord->black);
	//overlay(&temp, bitmap_white_rook(63,bord), 'X');
	//overlay(&temp, bqcastle, 'O');
	// 0b000000000000000000000000000000000000000000000000000000000000000
	std::cout << endl;
	if ((bord->extra & 1ULL << 18) != 0) {
		cout << "white to play:" << endl;
	}
	else {
		cout << "black to play:" << endl;
	}
	std::cout << "8 " << temp.substr(0,8) << endl;
	std::cout << "7 " << temp.substr(8,8) << endl;
	std::cout << "6 " << temp.substr(16,8) << endl;
	std::cout << "5 " << temp.substr(24,8) << endl;
	std::cout << "4 " << temp.substr(32,8) << endl;
	std::cout << "3 " << temp.substr(40,8) << endl;
	std::cout << "2 " << temp.substr(48,8) << endl;
	std::cout << "1 " << temp.substr(56,8) << endl;
	std::cout << "  abcdefgh" << endl;

	//printBitBoard(all_white_attacks(bord), "all white attacks");
	printBitBoard(en_passent_target(bord), "all white pawn attacks");
	//cout << std::bitset<18>(bord->extra) << endl;
	//cout << std::bitset<64>((((1ULL << 63) >> (((bord->extra >> 7) << 58) >> 58)))) << endl;
	//cout << std::bitset<64>((bord->extra & ((1ULL << 13))) >> 13) << endl;
	//cout << std::bitset<64>((~((((bord->extra & ((1ULL << 13))) >> 13) << 64) - 1)) & ((((1ULL << 63) >> (((bord->extra >> 7) << 58) >> 58))))) << endl;
}

void setup(Board* bord) {
	bord->rook   = 0b1000000100000000000000000000000000000000000000000000000010000001;
	bord->knight = 0b0100001000000000000000000000000000000000000000000000000001000010;
	bord->bishop = 0b0010010000000000000000000000000000000000000000000000000000100100;
	bord->queen  = 0b0001000000000000000000000000000000000000000000000000000000010000;
	bord->king   = 0b0000100000000000000000000000000000000000000000000000000000001000;
	bord->pawn   = 0b0000000011111111000000000000000000000000000000001111111100000000;
	bord->white  = 0b0000000000000000000000000000000000000000000000001111111111111111;
	bord->black  = 0b1111111111111111000000000000000000000000000000000000000000000000;
	// extra setup
	bord->extra = 0b1111100000000000000;
}

void setupEmpty(Board* bord) {
	bord->rook   = 0b0000000000000000000000000000000000000000000000000000000000000000;
	bord->knight = 0b0000000000000000000000000000000000000000000000000000000000000000;
	bord->bishop = 0b0000000000000000000000000000000000000000000000000000000000000000;
	bord->queen  = 0b0000000000000000000000000000000000000000000000000000000000000000;
	bord->king   = 0b0000000000000000000000000000000000000000000000000000000000000000;
	bord->pawn   = 0b0000000000000000000000000000000000000000000000000000000000000000;
	bord->white  = 0b0000000000000000000000000000000000000000000000000000000000000000;
	bord->black  = 0b0000000000000000000000000000000000000000000000000000000000000000;
	// extra setup
	bord->extra = 0b1111100000000000000;
}

void addPiece(Board* bord, Pieces piece, int square) {
	unsigned long long placeBit = ((1ULL << 63) >> square);
	if (piece == WROOK) {
		bord->white |= placeBit;
		bord->rook  |= placeBit;
	}else if (piece == WKNIGHT) {
		bord->white |= placeBit;
		bord->knight |= placeBit;
	}else if (piece == WBISCHOP) {
		bord->white |= placeBit;
		bord->bishop |= placeBit;
	}else if (piece == WQUEEN) {
		bord->white |= placeBit;
		bord->queen |= placeBit;
	}else if (piece == WKING) {
		bord->white |= placeBit;
		bord->king |= placeBit;
	}else if (piece == WPAWN) {
		bord->white |= placeBit;
		bord->pawn |= placeBit;
	}else if (piece == BROOK) {
		bord->black |= placeBit;
		bord->rook |= placeBit;
	}else if (piece == BKNIGHT) {
		bord->black |= placeBit;
		bord->knight |= placeBit;
	}else if (piece == BBISCHOP) {
		bord->black |= placeBit;
		bord->bishop |= placeBit;
	}else if (piece == BQUEEN) {
		bord->black |= placeBit;
		bord->queen |= placeBit;
	}else if (piece == BKING) {
		bord->black |= placeBit;
		bord->king |= placeBit;
	}else if (piece == BPAWN) {
		bord->black |= placeBit;
		bord->pawn |= placeBit;
	}
}

void clearSquare(Board* bord, int square) {
	unsigned long long placeBit = ~((1ULL << 63) >> square);
	bord->rook &= placeBit;
	bord->knight &= placeBit;
	bord->bishop &= placeBit;
	bord->queen &= placeBit;
	bord->king &= placeBit;
	bord->pawn &= placeBit;

	bord->white &= placeBit;
	bord->black &= placeBit;
}

Pieces pieceAt(int square, Board* bord) {
	unsigned long long sq = ((1ULL << 63) >> square);
	if (bord->white & sq) {
		if (bord->rook & sq) {
			return WROOK;
		}else if (bord->knight & sq) {
			return WKNIGHT;
		}else if (bord->bishop & sq) {
			return WBISCHOP;
		}else if (bord->queen & sq) {
			return WQUEEN;
		}else if (bord->king & sq) {
			return WKING;
		}else if (bord->pawn & sq) {
			return WPAWN;
		}
	}
	else if (bord->white & sq) {
		if (bord->rook & sq) {
			return BROOK;
		}
		else if (bord->knight & sq) {
			return BKNIGHT;
		}
		else if (bord->bishop & sq) {
			return BBISCHOP;
		}
		else if (bord->queen & sq) {
			return BQUEEN;
		}
		else if (bord->king & sq) {
			return BKING;
		}
		else if (bord->pawn & sq) {
			return BPAWN;
		}
	}
	return NOPIECE;
}

// Function to make a move
void makeMove(Board* bord, Move* move) {
	lastCapturedPiece = NOPIECE;
	// Update the pawns bitboard to reflect the move
	unsigned long long fromBit = ((1ULL << 63) >> move->src);
	unsigned long long toBit = ((1ULL << 63) >> move->dst);

	if ((move->special == NOT_SPECIAL) ||
		( move->special == SPECIAL_WPAWN_2SQUARES) ||
		(move->special == SPECIAL_BPAWN_2SQUARES) || 
		(move->special == SPECIAL_WEN_PASSANT) || 
		(move->special == SPECIAL_BEN_PASSANT) ||
		(move->special == SPECIAL_PROMOTION_BISHOP) || 
		(move->special == SPECIAL_PROMOTION_KNIGHT) ||
		(move->special == SPECIAL_PROMOTION_QUEEN) || 
		(move->special == SPECIAL_PROMOTION_ROOK)) {
		if (((bord->white | bord->black) & toBit) != 0) {
			//clear all bitboards on the to position
			lastCapturedPiece = pieceAt(move->dst, bord);
			bord->rook &= ~toBit;
			bord->knight &= ~toBit;
			bord->bishop &= ~toBit;
			bord->queen &= ~toBit;
			bord->king &= ~toBit;
			bord->pawn &= ~toBit;
			bord->white &= ~toBit;
			bord->black &= ~toBit;
		}

		if ((bord->white & fromBit) != 0) {
			bord->white ^= fromBit; // Clear the source square
			bord->white |= toBit;   // Set the destination square
		}else if ((bord->black & fromBit) != 0) {
			bord->black ^= fromBit; // Clear the source square
			bord->black |= toBit;   // Set the destination square
		}

		if ((bord->rook & fromBit) != 0) {
			if ((((1ULL << 63) >> (A8)) & fromBit) != 0) {
				bord->extra &= ~(1ULL << 14); // remove white kingside casteling ability
			}
			else if ((((1ULL << 63) >> (H8)) & fromBit) != 0) {
				bord->extra &= ~(1ULL << 15); // remove white queenside casteling ability
			}
			else if ((((1ULL << 63) >> (A1)) & fromBit) != 0) {
				bord->extra &= ~(1ULL << 16); // remove black kingside casteling ability
			}
			else if ((((1ULL << 63) >> (H1)) & fromBit) != 0) {
				bord->extra &= ~(1ULL << 17); // remove black queenside casteling ability
			}
			bord->rook ^= fromBit; // Clear the source square
			bord->rook |= toBit;   // Set the destination square
		}else if ((bord->knight & fromBit) != 0) {
			bord->knight ^= fromBit; // Clear the source square
			bord->knight |= toBit;   // Set the destination square
		}else if ((bord->bishop & fromBit) != 0) {
			bord->bishop ^= fromBit; // Clear the source square
			bord->bishop |= toBit;   // Set the destination square
		}else if ((bord->queen & fromBit) != 0) {
			bord->queen ^= fromBit; // Clear the source square
			bord->queen |= toBit;   // Set the destination square
		}else if ((bord->king & fromBit) != 0) {
			if ((bord->white & fromBit) != 0) {
				bord->extra &= ~((((1ULL << 2) - 1) << 16));
			}
			else if ((bord->black & fromBit) != 0) {
				bord->extra &= ~((((1ULL << 2) - 1) << 14));
			}
			bord->king ^= fromBit; // Clear the source square
			bord->king |= toBit;   // Set the destination square
		}else if ((bord->pawn & fromBit) != 0) {
			bord->pawn ^= fromBit; // Clear the source square
			bord->pawn |= toBit;   // Set the destination square
		}
	}

	bord->extra &= ~((((1ULL << 7) - 1) << 7)); // remove en passent target
	if (move->special != NOT_SPECIAL) {
		if (move->special == SPECIAL_WPAWN_2SQUARES) {
			bord->extra |= ((1ULL << 13));
			bord->extra |= ((move->dst + 8) << 7);
		}
		else if (move->special == SPECIAL_BPAWN_2SQUARES) {
			bord->extra |= ((1ULL << 13));
			bord->extra |= ((move->dst - 8) << 7);
		}
		else if (move->special == SPECIAL_WEN_PASSANT) {
			unsigned long long enPassentBit = ((1ULL << 63) >> (move->dst + 8));
			//clear all bitboards on the to position of the en passant pawn
			bord->pawn &= ~enPassentBit;
			bord->white &= ~enPassentBit;
			bord->black &= ~enPassentBit;
		}
		else if (move->special == SPECIAL_BEN_PASSANT) {
			unsigned long long enPassentBit = ((1ULL << 63) >> (move->dst - 8));
			//clear all bitboards on the to position of the en passant pawn
			bord->pawn &= ~enPassentBit;
			bord->white &= ~enPassentBit;
			bord->black &= ~enPassentBit;
		}
		else if (move->special == SPECIAL_WK_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 16));
			unsigned long long rookSQ = ((1ULL << 63) >> (H1));
			bord->rook &= ~rookSQ;
			bord->white &= ~rookSQ;
			unsigned long long kingSQ = ((1ULL << 63) >> (E1));
			bord->king &= ~kingSQ;
			bord->white &= ~kingSQ;

			unsigned long long newRookSQ = ((1ULL << 63) >> (F1));
			bord->rook |= newRookSQ;
			bord->white |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (G1));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
		}
		else if (move->special == SPECIAL_BK_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 14));
			unsigned long long rookSQ = ((1ULL << 63) >> (H8));
			bord->rook &= ~rookSQ;
			bord->black &= ~rookSQ;
			unsigned long long kingSQ = ((1ULL << 63) >> (E8));
			bord->king &= ~kingSQ;
			bord->black &= ~kingSQ;

			unsigned long long newRookSQ = ((1ULL << 63) >> (F8));
			bord->rook |= newRookSQ;
			bord->black |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (G8));
			bord->king |= newKingSQ;
			bord->black |= newKingSQ;
		}
		else if (move->special == SPECIAL_WQ_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 16));
			unsigned long long rookSQ = ((1ULL << 63) >> (A1));
			bord->rook &= ~rookSQ;
			bord->white &= ~rookSQ;
			unsigned long long kingSQ = ((1ULL << 63) >> (E1));
			bord->king &= ~kingSQ;
			bord->white &= ~kingSQ;

			unsigned long long newRookSQ = ((1ULL << 63) >> (D1));
			bord->rook |= newRookSQ;
			bord->white |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (C1));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
		}
		else if (move->special == SPECIAL_BQ_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 14));
			unsigned long long rookSQ = ((1ULL << 63) >> (A8));
			bord->rook &= ~rookSQ;
			bord->black &= ~rookSQ;
			unsigned long long kingSQ = ((1ULL << 63) >> (E8));
			bord->king &= ~kingSQ;
			bord->black &= ~kingSQ;

			unsigned long long newRookSQ = ((1ULL << 63) >> (D8));
			bord->rook |= newRookSQ;
			bord->black |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (C8));
			bord->king |= newKingSQ;
			bord->black |= newKingSQ;
		}
		else if (move->special == SPECIAL_PROMOTION_BISHOP) {
			if ((bord->extra &= (1ULL << 18)) != 0) {
				unsigned long long promotionPawn = oneRow & bord->pawn & bord->black;
				bord->pawn &= ~promotionPawn;
				bord->bishop |= promotionPawn;
			}
			else {
				unsigned long long promotionPawn = eightRow & bord->pawn & bord->white;
				bord->pawn &= ~promotionPawn;
				bord->bishop |= promotionPawn;
			}
		}
		else if (move->special == SPECIAL_PROMOTION_KNIGHT) {
			if ((bord->extra &= (1ULL << 18)) != 0) {
				unsigned long long promotionPawn = oneRow & bord->pawn & bord->black;
				bord->pawn &= ~promotionPawn;
				bord->knight |= promotionPawn;
			}
			else {
				unsigned long long promotionPawn = eightRow & bord->pawn & bord->white;
				bord->pawn &= ~promotionPawn;
				bord->knight |= promotionPawn;
			}
		}
		else if (move->special == SPECIAL_PROMOTION_QUEEN) {
			if ((bord->extra &= (1ULL << 18)) != 0) {
				unsigned long long promotionPawn = oneRow & bord->pawn & bord->black;
				bord->pawn &= ~promotionPawn;
				bord->queen |= promotionPawn;
			}
			else {
				unsigned long long promotionPawn = eightRow & bord->pawn & bord->white;
				bord->pawn &= ~promotionPawn;
				bord->queen |= promotionPawn;
			}
		}
		else if (move->special == SPECIAL_PROMOTION_ROOK) {
			if ((bord->extra &= (1ULL << 18)) != 0) {
				unsigned long long promotionPawn = oneRow & bord->pawn & bord->black;
				bord->pawn &= ~promotionPawn;
				bord->rook |= promotionPawn;
			}
			else {
				unsigned long long promotionPawn = eightRow & bord->pawn & bord->white;
				bord->pawn &= ~promotionPawn;
				bord->rook |= promotionPawn;
			}
		}
	}
	bord->extra ^= (1ULL << 18); // swap playing player
}


// Function to remove a move
void popMove(Board* bord, Move* move) {
	// Update the pawns bitboard to reflect the move
	unsigned long long fromBit = ((1ULL << 63) >> move->src);
	unsigned long long toBit = ((1ULL << 63) >> move->dst);

	if ((move->special == NOT_SPECIAL) ||
		(move->special == SPECIAL_WPAWN_2SQUARES) ||
		(move->special == SPECIAL_BPAWN_2SQUARES) ||
		(move->special == SPECIAL_WEN_PASSANT) ||
		(move->special == SPECIAL_BEN_PASSANT) ||
		(move->special == SPECIAL_PROMOTION_BISHOP) ||
		(move->special == SPECIAL_PROMOTION_KNIGHT) ||
		(move->special == SPECIAL_PROMOTION_QUEEN) ||
		(move->special == SPECIAL_PROMOTION_ROOK)) {
		
		// reset the color bitboards
		if ((bord->white & toBit) != 0) {
			bord->white |= fromBit; // set the source square
			bord->white ^= toBit;   // clear the destination square
		}
		else if ((bord->black & toBit) != 0) {
			bord->black |= fromBit; // set the source square
			bord->black ^= toBit;   // clear the destination square
		}


		if ((bord->rook & toBit) != 0) {
			/* //TODO
			if ((((1ULL << 63) >> (A8)) & fromBit) != 0) {
				bord->extra |= (1ULL << 14); // re add white kingside casteling ability
			}
			else if ((((1ULL << 63) >> (H8)) & fromBit) != 0) {
				bord->extra |= (1ULL << 15); // re add white queenside casteling ability
			}
			else if ((((1ULL << 63) >> (A1)) & fromBit) != 0) {
				bord->extra |= (1ULL << 16); // re add black kingside casteling ability
			}
			else if ((((1ULL << 63) >> (H1)) & fromBit) != 0) {
				bord->extra |= (1ULL << 17); // re add black queenside casteling ability
			}
			*/
			bord->rook |= fromBit; // set the source square
			bord->rook ^= toBit;   // clear the destination square
		}
		else if ((bord->knight & toBit) != 0) {
			bord->knight |= fromBit; // Clear the source square
			bord->knight ^= toBit;   // Set the destination square
		}
		else if ((bord->bishop & toBit) != 0) {
			bord->bishop |= fromBit; // Clear the source square
			bord->bishop ^= toBit;   // Set the destination square
		}
		else if ((bord->queen & toBit) != 0) {
			bord->queen |= fromBit; // Clear the source square
			bord->queen ^= toBit;   // Set the destination square
		}
		else if ((bord->king & toBit) != 0) {
			/* //TODO
			if ((bord->white & fromBit) != 0) {
				bord->extra &= ~((((1ULL << 2) - 1) << 16));
			}
			else if ((bord->black & fromBit) != 0) {
				bord->extra &= ~((((1ULL << 2) - 1) << 14));
			}
			*/
			bord->king |= fromBit; // Clear the source square
			bord->king ^= toBit;   // Set the destination square
		}
		else if ((bord->pawn & toBit) != 0) {
			bord->pawn |= fromBit; // Clear the source square
			bord->pawn ^= toBit;   // Set the destination square
		}
	}

	//bord->extra &= ~((((1ULL << 7) - 1) << 7)); // remove en passent target //TODO
	if (move->special != NOT_SPECIAL) {
		if (move->special == SPECIAL_WPAWN_2SQUARES) {
			bord->extra &= ~((((1ULL << 7) - 1) << 7));
		}
		else if (move->special == SPECIAL_BPAWN_2SQUARES) {
			bord->extra &= ~((((1ULL << 7) - 1) << 7));
		}
		/* //TODO
		else if (move->special == SPECIAL_WEN_PASSANT) {
			unsigned long long enPassentBit = ((1ULL << 63) >> (move->dst + 8));
			//clear all bitboards on the to position of the en passant pawn
			bord->pawn &= ~enPassentBit;
			bord->white &= ~enPassentBit;
			bord->black &= ~enPassentBit;
		}
		else if (move->special == SPECIAL_BEN_PASSANT) {
			unsigned long long enPassentBit = ((1ULL << 63) >> (move->dst - 8));
			//clear all bitboards on the to position of the en passant pawn
			bord->pawn &= ~enPassentBit;
			bord->white &= ~enPassentBit;
			bord->black &= ~enPassentBit;
		}
		*/
		/* //TODO
		else if (move->special == SPECIAL_WK_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 16));
			unsigned long long rookSQ = ((1ULL << 63) >> (H1));
			bord->rook &= ~rookSQ;
			bord->white &= ~rookSQ;
			unsigned long long kingSQ = ((1ULL << 63) >> (E1));
			bord->king &= ~kingSQ;
			bord->white &= ~kingSQ;

			unsigned long long newRookSQ = ((1ULL << 63) >> (F1));
			bord->rook |= newRookSQ;
			bord->white |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (G1));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
		}
		else if (move->special == SPECIAL_BK_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 14));
			unsigned long long rookSQ = ((1ULL << 63) >> (H8));
			bord->rook &= ~rookSQ;
			bord->black &= ~rookSQ;
			unsigned long long kingSQ = ((1ULL << 63) >> (E8));
			bord->king &= ~kingSQ;
			bord->black &= ~kingSQ;

			unsigned long long newRookSQ = ((1ULL << 63) >> (F8));
			bord->rook |= newRookSQ;
			bord->black |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (G8));
			bord->king |= newKingSQ;
			bord->black |= newKingSQ;
		}
		else if (move->special == SPECIAL_WQ_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 16));
			unsigned long long rookSQ = ((1ULL << 63) >> (A1));
			bord->rook &= ~rookSQ;
			bord->white &= ~rookSQ;
			unsigned long long kingSQ = ((1ULL << 63) >> (E1));
			bord->king &= ~kingSQ;
			bord->white &= ~kingSQ;

			unsigned long long newRookSQ = ((1ULL << 63) >> (D1));
			bord->rook |= newRookSQ;
			bord->white |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (C1));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
		}
		else if (move->special == SPECIAL_BQ_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 14));
			unsigned long long rookSQ = ((1ULL << 63) >> (A8));
			bord->rook &= ~rookSQ;
			bord->black &= ~rookSQ;
			unsigned long long kingSQ = ((1ULL << 63) >> (E8));
			bord->king &= ~kingSQ;
			bord->black &= ~kingSQ;

			unsigned long long newRookSQ = ((1ULL << 63) >> (D8));
			bord->rook |= newRookSQ;
			bord->black |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (C8));
			bord->king |= newKingSQ;
			bord->black |= newKingSQ;
		}
		*/
		else if (move->special == SPECIAL_PROMOTION_BISHOP) {
			unsigned long long promotionPawn = bord->bishop & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->bishop &= ~promotionPawn;
		}
		else if (move->special == SPECIAL_PROMOTION_KNIGHT) {
			unsigned long long promotionPawn = bord->knight & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->knight &= ~promotionPawn;
		}
		else if (move->special == SPECIAL_PROMOTION_QUEEN) {
			unsigned long long promotionPawn = bord->queen & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->queen &= ~promotionPawn;
		}
		else if (move->special == SPECIAL_PROMOTION_ROOK) {
			unsigned long long promotionPawn = bord->rook & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->rook &= ~promotionPawn;
		}
	}
	bord->extra ^= (1ULL << 18); // swap playing player
	// add captured pieces back
	if (move->capture) {
		addPiece(bord, lastCapturedPiece, move->dst);
	}
}