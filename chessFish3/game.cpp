// game.cpp : Defines the entry point for the application.
//

#include "game.h"
#include <bitset>
#include <chrono>

constexpr U64 oneRow   = 0b0000000000000000000000000000000000000000000000000000000011111111; // oneRow
constexpr U64 twoRow   = 0b0000000000000000000000000000000000000000000000001111111100000000; // oneRow << 8
constexpr U64 threeRow = 0b0000000000000000000000000000000000000000111111110000000000000000; // oneRow << 16
constexpr U64 fourRow  = 0b0000000000000000000000000000000011111111000000000000000000000000; // oneRow << 24
constexpr U64 fiveRow  = 0b0000000000000000000000001111111100000000000000000000000000000000; // oneRow << 32
constexpr U64 sixRow   = 0b0000000000000000111111110000000000000000000000000000000000000000; // oneRow << 40
constexpr U64 sevenRow = 0b0000000011111111000000000000000000000000000000000000000000000000; // oneRow << 48
constexpr U64 eightRow = 0b1111111100000000000000000000000000000000000000000000000000000000; // oneRow << 56

constexpr U64 A        = 0b1000000010000000100000001000000010000000100000001000000010000000; // A
constexpr U64 B        = 0b0100000001000000010000000100000001000000010000000100000001000000; // A >> 1
constexpr U64 C        = 0b0010000000100000001000000010000000100000001000000010000000100000; // A >> 2
constexpr U64 D        = 0b0001000000010000000100000001000000010000000100000001000000010000; // A >> 3
constexpr U64 E        = 0b0000100000001000000010000000100000001000000010000000100000001000; // A >> 4
constexpr U64 F        = 0b0000010000000100000001000000010000000100000001000000010000000100; // A >> 5
constexpr U64 G        = 0b0000001000000010000000100000001000000010000000100000001000000010; // A >> 6
constexpr U64 H        = 0b0000000100000001000000010000000100000001000000010000000100000001; // A >> 7

constexpr U64 all      = 0b1111111111111111111111111111111111111111111111111111111111111111; // << 8 move row up, >> 8 move row down (left right imposible)
constexpr U64 nothing  = 0b0000000000000000000000000000000000000000000000000000000000000000;
constexpr U64 border   = 0b1111111110000001100000011000000110000001100000011000000111111111; // the border of the field
constexpr U64 corners  = 0b1000000100000000000000000000000000000000000000000000000010000001;
constexpr U64 wkcastle = 0b0000000000000000000000000000000000000000000000000000000000000110;
constexpr U64 wqcastle = 0b0000000000000000000000000000000000000000000000000000000001110000;
constexpr U64 bkcastle = 0b0000011000000000000000000000000000000000000000000000000000000000;
constexpr U64 bqcastle = 0b0111000000000000000000000000000000000000000000000000000000000000;

#define en_passent_target(bord) (((1ULL<<63) >> (bord->enPassantTarget)) & (bord->enPassentValid ? UINT64_MAX : 0))
#define white_plays(bord) (bord->whiteToPlay == 1)

// bit manipulation macros
#define get_bit(bitboard, index) (bitboard & (1ULL << index))
#define set_bit(bitboard, index) (bitboard |= (1ULL << index))
#define pop_bit(bitboard, index) (get_bit(bitboard, index) ? bitboard ^= (1ULL << index) : 0)


//Pieces lastCapturedPiece = NOPIECE;

// Function to copy values from bordIn to bordOut
void copyBoard(const Board* bordIn, Board* bordOut) {
	if (!bordIn || !bordOut) {
		return; // Handle nullptr input
	}

	// Copy the members from bordIn to bordOut
	bordOut->rook = bordIn->rook;
	bordOut->knight = bordIn->knight;
	bordOut->bishop = bordIn->bishop;
	bordOut->queen = bordIn->queen;
	bordOut->king = bordIn->king;
	bordOut->pawn = bordIn->pawn;
	bordOut->white = bordIn->white;
	bordOut->black = bordIn->black;
	bordOut->whiteToPlay = bordIn->whiteToPlay;
	bordOut->whiteKingsideCastle = bordIn->whiteKingsideCastle;
	bordOut->whiteQueensideCastle = bordIn->whiteQueensideCastle;
	bordOut->blackKingsideCastle = bordIn->blackKingsideCastle;
	bordOut->blackQueensideCastle = bordIn->blackQueensideCastle;
	bordOut->enPassentValid = bordIn->enPassentValid;
	bordOut->enPassantTarget = bordIn->enPassantTarget;
	bordOut->halfmoveClock = bordIn->halfmoveClock;
	bordOut->reserved = bordIn->reserved;
	//bordOut->extra = bordIn->extra;
}

int findMoveIndex(MOVELIST* moveList, Move* targetMove) {
	for (int i = 0; i < moveList->count; ++i) {
		if (moveList->moves[i] == *targetMove) {
			return i;  // Return the index of the matching move
		}
	}
	return -1;  // Return -1 if the move was not found
}

U64 incrementByOne(U64 number) {
	U64 mask = 1;
	while (number & mask) {
		number &= ~mask; // Clear the current bit
		mask <<= 1;      // Move to the next bit
	}
	number |= mask;     // Set the first cleared bit
	return number;
}

constexpr U64 rook_magics[64] = {
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

constexpr U64 bishop_magics[64] = {
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

constexpr U64 kingMoves[64] = {
	0b0100000011000000000000000000000000000000000000000000000000000000,
	0b1010000011100000000000000000000000000000000000000000000000000000,
	0b0101000001110000000000000000000000000000000000000000000000000000,
	0b0010100000111000000000000000000000000000000000000000000000000000,
	0b0001010000011100000000000000000000000000000000000000000000000000,
	0b0000101000001110000000000000000000000000000000000000000000000000,
	0b0000010100000111000000000000000000000000000000000000000000000000,
	0b0000001000000011000000000000000000000000000000000000000000000000,
	0b1100000001000000110000000000000000000000000000000000000000000000,
	0b1110000010100000111000000000000000000000000000000000000000000000,
	0b0111000001010000011100000000000000000000000000000000000000000000,
	0b0011100000101000001110000000000000000000000000000000000000000000,
	0b0001110000010100000111000000000000000000000000000000000000000000,
	0b0000111000001010000011100000000000000000000000000000000000000000,
	0b0000011100000101000001110000000000000000000000000000000000000000,
	0b0000001100000010000000110000000000000000000000000000000000000000,
	0b0000000011000000010000001100000000000000000000000000000000000000,
	0b0000000011100000101000001110000000000000000000000000000000000000,
	0b0000000001110000010100000111000000000000000000000000000000000000,
	0b0000000000111000001010000011100000000000000000000000000000000000,
	0b0000000000011100000101000001110000000000000000000000000000000000,
	0b0000000000001110000010100000111000000000000000000000000000000000,
	0b0000000000000111000001010000011100000000000000000000000000000000,
	0b0000000000000011000000100000001100000000000000000000000000000000,
	0b0000000000000000110000000100000011000000000000000000000000000000,
	0b0000000000000000111000001010000011100000000000000000000000000000,
	0b0000000000000000011100000101000001110000000000000000000000000000,
	0b0000000000000000001110000010100000111000000000000000000000000000,
	0b0000000000000000000111000001010000011100000000000000000000000000,
	0b0000000000000000000011100000101000001110000000000000000000000000,
	0b0000000000000000000001110000010100000111000000000000000000000000,
	0b0000000000000000000000110000001000000011000000000000000000000000,
	0b0000000000000000000000001100000001000000110000000000000000000000,
	0b0000000000000000000000001110000010100000111000000000000000000000,
	0b0000000000000000000000000111000001010000011100000000000000000000,
	0b0000000000000000000000000011100000101000001110000000000000000000,
	0b0000000000000000000000000001110000010100000111000000000000000000,
	0b0000000000000000000000000000111000001010000011100000000000000000,
	0b0000000000000000000000000000011100000101000001110000000000000000,
	0b0000000000000000000000000000001100000010000000110000000000000000,
	0b0000000000000000000000000000000011000000010000001100000000000000,
	0b0000000000000000000000000000000011100000101000001110000000000000,
	0b0000000000000000000000000000000001110000010100000111000000000000,
	0b0000000000000000000000000000000000111000001010000011100000000000,
	0b0000000000000000000000000000000000011100000101000001110000000000,
	0b0000000000000000000000000000000000001110000010100000111000000000,
	0b0000000000000000000000000000000000000111000001010000011100000000,
	0b0000000000000000000000000000000000000011000000100000001100000000,
	0b0000000000000000000000000000000000000000110000000100000011000000,
	0b0000000000000000000000000000000000000000111000001010000011100000,
	0b0000000000000000000000000000000000000000011100000101000001110000,
	0b0000000000000000000000000000000000000000001110000010100000111000,
	0b0000000000000000000000000000000000000000000111000001010000011100,
	0b0000000000000000000000000000000000000000000011100000101000001110,
	0b0000000000000000000000000000000000000000000001110000010100000111,
	0b0000000000000000000000000000000000000000000000110000001000000011,
	0b0000000000000000000000000000000000000000000000001100000001000000,
	0b0000000000000000000000000000000000000000000000001110000010100000,
	0b0000000000000000000000000000000000000000000000000111000001010000,
	0b0000000000000000000000000000000000000000000000000011100000101000,
	0b0000000000000000000000000000000000000000000000000001110000010100,
	0b0000000000000000000000000000000000000000000000000000111000001010,
	0b0000000000000000000000000000000000000000000000000000011100000101,
	0b0000000000000000000000000000000000000000000000000000001100000010,
};

constexpr U64 knightMoves[64] = {
	0b0000000000100000010000000000000000000000000000000000000000000000,
	0b0000000000010000101000000000000000000000000000000000000000000000,
	0b0000000010001000010100000000000000000000000000000000000000000000,
	0b0000000001000100001010000000000000000000000000000000000000000000,
	0b0000000000100010000101000000000000000000000000000000000000000000,
	0b0000000000010001000010100000000000000000000000000000000000000000,
	0b0000000000001000000001010000000000000000000000000000000000000000,
	0b0000000000000100000000100000000000000000000000000000000000000000,
	0b0010000000000000001000000100000000000000000000000000000000000000,
	0b0001000000000000000100001010000000000000000000000000000000000000,
	0b1000100000000000100010000101000000000000000000000000000000000000,
	0b0100010000000000010001000010100000000000000000000000000000000000,
	0b0010001000000000001000100001010000000000000000000000000000000000,
	0b0001000100000000000100010000101000000000000000000000000000000000,
	0b0000100000000000000010000000010100000000000000000000000000000000,
	0b0000010000000000000001000000001000000000000000000000000000000000,
	0b0100000000100000000000000010000001000000000000000000000000000000,
	0b1010000000010000000000000001000010100000000000000000000000000000,
	0b0101000010001000000000001000100001010000000000000000000000000000,
	0b0010100001000100000000000100010000101000000000000000000000000000,
	0b0001010000100010000000000010001000010100000000000000000000000000,
	0b0000101000010001000000000001000100001010000000000000000000000000,
	0b0000010100001000000000000000100000000101000000000000000000000000,
	0b0000001000000100000000000000010000000010000000000000000000000000,
	0b0000000001000000001000000000000000100000010000000000000000000000,
	0b0000000010100000000100000000000000010000101000000000000000000000,
	0b0000000001010000100010000000000010001000010100000000000000000000,
	0b0000000000101000010001000000000001000100001010000000000000000000,
	0b0000000000010100001000100000000000100010000101000000000000000000,
	0b0000000000001010000100010000000000010001000010100000000000000000,
	0b0000000000000101000010000000000000001000000001010000000000000000,
	0b0000000000000010000001000000000000000100000000100000000000000000,
	0b0000000000000000010000000010000000000000001000000100000000000000,
	0b0000000000000000101000000001000000000000000100001010000000000000,
	0b0000000000000000010100001000100000000000100010000101000000000000,
	0b0000000000000000001010000100010000000000010001000010100000000000,
	0b0000000000000000000101000010001000000000001000100001010000000000,
	0b0000000000000000000010100001000100000000000100010000101000000000,
	0b0000000000000000000001010000100000000000000010000000010100000000,
	0b0000000000000000000000100000010000000000000001000000001000000000,
	0b0000000000000000000000000100000000100000000000000010000001000000,
	0b0000000000000000000000001010000000010000000000000001000010100000,
	0b0000000000000000000000000101000010001000000000001000100001010000,
	0b0000000000000000000000000010100001000100000000000100010000101000,
	0b0000000000000000000000000001010000100010000000000010001000010100,
	0b0000000000000000000000000000101000010001000000000001000100001010,
	0b0000000000000000000000000000010100001000000000000000100000000101,
	0b0000000000000000000000000000001000000100000000000000010000000010,
	0b0000000000000000000000000000000001000000001000000000000000100000,
	0b0000000000000000000000000000000010100000000100000000000000010000,
	0b0000000000000000000000000000000001010000100010000000000010001000,
	0b0000000000000000000000000000000000101000010001000000000001000100,
	0b0000000000000000000000000000000000010100001000100000000000100010,
	0b0000000000000000000000000000000000001010000100010000000000010001,
	0b0000000000000000000000000000000000000101000010000000000000001000,
	0b0000000000000000000000000000000000000010000001000000000000000100,
	0b0000000000000000000000000000000000000000010000000010000000000000,
	0b0000000000000000000000000000000000000000101000000001000000000000,
	0b0000000000000000000000000000000000000000010100001000100000000000,
	0b0000000000000000000000000000000000000000001010000100010000000000,
	0b0000000000000000000000000000000000000000000101000010001000000000,
	0b0000000000000000000000000000000000000000000010100001000100000000,
	0b0000000000000000000000000000000000000000000001010000100000000000,
	0b0000000000000000000000000000000000000000000000100000010000000000
};

//TODO define constexpr for diagonal moves
constexpr U64 Right(U64 num) {
	return num >> 1;
}

constexpr U64 Left(U64 num) {
	return num << 1;
}

constexpr U64 Up(U64 num) {
	return num << 8;
}

constexpr U64 Down(U64 num) {
	return num >> 8;
}

using namespace std;

int countTrailingZeros(U64 number) {
	unsigned long index;
	if (_BitScanForward64(&index, number)) {
		return static_cast<int>(index);
	}
	return 64; // Return 64 if the input number is 0
}

int getFirst1BitSquare(U64 number) {
	unsigned long index;
	if (_BitScanForward64(&index, number)) {
		return 63-static_cast<int>(index);
	}
	return -1; // Return 64 if the input number is 0
}

int countSetBits(U64 number) {
	int count = 0;
	while (number) {
		count++;
		number &= (number - 1); // Clear the least significant set bit
	}
	return count;
}

std::string squareToString(Square square) {
	std::string file = "ABCDEFGH";
	std::string rank = "12345678";

	int index = static_cast<int>(square);
	int fileIndex = index % 8;
	int rankIndex = 7 - index / 8; // Adjusted for reversed numbering

	return std::string(1, file[fileIndex]) + rank[rankIndex];
}

std::string moveToString(Move* move) {
	std::string captureString = "";
	if (move->capture != -52) {
		captureString = " is capturing a piece";
	}
	return "move from: " + squareToString(move->src) +
		" to: " + squareToString(move->dst) + " " +
		specialToString(move->special) +
		captureString;
}

U64 bitmap_all_white_pawns(Board* bord) {
	U64 wpawns = bord->pawn & bord->white; // all positions of white pawns
	U64 doublePawns = (wpawns & twoRow) & ~((bord->white | bord->black) >> 8); // all positions of white pawns able to move 2
	U64 nonCaptures = (((doublePawns << 8) | (doublePawns << 16) | (wpawns << 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	U64 captures = ((wpawns & (~border)) << 7 | (wpawns & (~border)) << 9 | (wpawns & H) << 9 | (wpawns & A) << 7); // all capturing moves a pawn can do
	U64 enPassent = en_passent_target(bord); // all squares that are able to be en passented
	return (nonCaptures | (captures & bord->black) | (enPassent & captures));
}

U64 bitmap_all_black_pawns(Board* bord) {
	U64 bpawns = bord->pawn & bord->black; // all positions of black pawns
	U64 doublePawns = (bpawns & sevenRow) & ~((bord->white | bord->black) << 8); // all positions of black pawns able to move 2
	U64 nonCaptures = (((doublePawns >> 8) | (doublePawns >> 16) | (bpawns >> 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	U64 captures = ((bpawns & (~border)) >> 7 | (bpawns & (~border)) >> 9 | (bpawns & H) >> 7 | (bpawns & A) >> 9); // all capturing moves a pawn can do
	U64 enPassent = en_passent_target(bord); // all squares that are able to be en passented
	return (nonCaptures | (captures & bord->white) | (enPassent & captures));
}

U64 bitmap_white_king_danger_squares(int position, Board* bord) {
	clearSquare(bord, position);
	U64 white_king_danger_squares = all_black_attacks(bord);
	addPiece(bord, WKING, position);
	return white_king_danger_squares;
}

U64 bitmap_black_king_danger_squares(int position, Board* bord) {
	clearSquare(bord, position);
	U64 black_king_danger_squares = all_white_attacks(bord);
	addPiece(bord, BKING, position);
	return black_king_danger_squares;
}

U64 bitmap_all_white_king(Board* bord, int diepte) { // TODO test of het gewoon pseudo legal moves gebruiken nie backfired
	U64 wkings = bord->king & bord->white; // the square given
	U64 wkings_not_on_border = wkings & (~border);
	U64 all_dirs_non_border = Down(wkings_not_on_border) | Up(wkings_not_on_border) | Left(wkings_not_on_border) | Right(wkings_not_on_border) | Down(Left(wkings_not_on_border)) | Down(Right(wkings_not_on_border)) | Up(Left(wkings_not_on_border)) | Up(Right(wkings_not_on_border));
	U64 all_dirs_non_corner = Right(wkings & A) | Up(wkings & A) | Down(wkings & A) | Left(wkings & H) | Up(wkings & H) | Down(wkings & H) | Up(wkings & oneRow) | Left(wkings & oneRow) | Right(wkings & oneRow) | Down(wkings & eightRow) | Left(wkings & eightRow) | Right(wkings & eightRow);
	U64 empty = ~(bord->white | bord->black);
	U64 castel = nothing;
	if (countTrailingZeros(wkings) == (63-E8)) {
		castel = ((((wkcastle & empty) == 6) & bord->whiteKingsideCastle) << 1) | ((((wqcastle & empty) == 112) & bord->whiteQueensideCastle) << 5);
	}
	if (diepte) {
		return (((all_dirs_non_border | all_dirs_non_corner) & (~bord->white)) | castel);//& (~bitmap_white_king_danger_squares(63 - countTrailingZeros(wkings), bord));
	}
	else {
		return (((all_dirs_non_border | all_dirs_non_corner) & (~bord->white)) | castel) & (~bitmap_white_king_danger_squares(63 - countTrailingZeros(wkings), bord));
	}
}

U64 bitmap_all_black_king(Board* bord, int diepte) {	// TODO test of het gewoon pseudo legal moves gebruiken nie backfired
	U64 bkings = bord->king & bord->black; // the square given
	U64 bkings_not_on_border = bkings & (~border);
	U64 all_dirs_non_border = Down(bkings_not_on_border) | Up(bkings_not_on_border) | Left(bkings_not_on_border) | Right(bkings_not_on_border) | Down(Left(bkings_not_on_border)) | Down(Right(bkings_not_on_border)) | Up(Left(bkings_not_on_border)) | Up(Right(bkings_not_on_border));
	U64 all_dirs_non_corner = Right(bkings & A) | Up(bkings & A) | Down(bkings & A) | Left(bkings & H) | Up(bkings & H) | Down(bkings & H) | Up(bkings & oneRow) | Left(bkings & oneRow) | Right(bkings & oneRow) | Down(bkings & eightRow) | Left(bkings & eightRow) | Right(bkings & eightRow);
	U64 empty = ~(bord->white | bord->black);
	U64 castel = nothing;
	if (countTrailingZeros(bkings) == (63 - E1)) {
		castel = (((((bkcastle & empty) == 432345564227567616) & bord->blackKingsideCastle) << 57)) | (((((bqcastle & empty) == 8070450532247928832) & bord->blackQueensideCastle) << 61));
	}
	if (diepte) {
		return (((all_dirs_non_border | all_dirs_non_corner) & (~bord->black)) | castel);// &(~bitmap_black_king_danger_squares(63 - countTrailingZeros(bkings), bord));

	}
	else {
		return (((all_dirs_non_border | all_dirs_non_corner) & (~bord->black)) | castel) &(~bitmap_black_king_danger_squares(63 - countTrailingZeros(bkings), bord));

	}
}

/*
// Function to generate a bitboard representing all squares a rook can attack from a given position (0 to 63)
U64 bitmap_white_rook(int position, Board* bord) {
	U64 occupied = bord->white | bord->black;
	U64 rookAttacks = 0ULL;
	int row = position / 8;
	int col = position % 8;

	// Generate attacks along the same row (horizontal)
	U64 horizontalMask = eightRow >> (8 * row);
	U64 horizontalAttacks = horizontalMask & ~((1ULL << 63) >> position);

	// Remove the squares that are blocked by other pieces along the row to the right of the rook
	U64 rightOccupied = occupied & (horizontalMask >> (col + 1));
	U64 rightClearMask = rightOccupied - 1ULL;
	horizontalAttacks ^= (horizontalAttacks & rightOccupied) & (horizontalAttacks ^ rightClearMask);

	// Remove the squares that are blocked by other pieces along the row to the left of the rook
	U64 leftOccupied = occupied & (horizontalMask << (8 - col));
	U64 leftClearMask = (leftOccupied - 1ULL) << 1ULL;
	horizontalAttacks ^= (horizontalAttacks & leftOccupied) & (horizontalAttacks ^ leftClearMask);

	// Generate attacks along the same column (vertical)
	U64 verticalMask = A >> col;
	U64 verticalAttacks = verticalMask & ~((1ULL << 63) >> position);

	// Remove the squares that are blocked by other pieces along the column above the rook
	U64 upOccupied = occupied & (verticalMask >> (row + 1));
	U64 upClearMask = rightOccupied - 1ULL;
	verticalAttacks ^= (verticalAttacks & upOccupied) & (verticalAttacks ^ upClearMask);

	// Remove the squares that are blocked by other pieces along the column below the rook
	U64 downOccupied = occupied & (verticalMask << (8 - row));
	U64 downClearMask = (leftOccupied - 1ULL) << 1ULL;
	verticalAttacks ^= (verticalAttacks & downOccupied) & (verticalAttacks ^ downClearMask);

	rookAttacks = horizontalAttacks | verticalAttacks;
	return rookAttacks;
*/

U64 bitmap_white_pawns(int position, Board* bord) {
	U64 wpawns = ((1ULL << 63) >> position); // the square given
	U64 doublePawns = (wpawns & twoRow) & ~((bord->white|bord->black) >> 8); // all positions of white pawns able to move 2
	U64 nonCaptures = (((doublePawns << 8) | (doublePawns << 16) | (wpawns << 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	U64 captures = ((wpawns & (~border)) << 7 | (wpawns & (~border)) << 9 | (wpawns & H) << 9 | (wpawns & A) << 7); // all capturing moves a pawn can do
	U64 enPassent = en_passent_target(bord); // all squares that are able to be en passented
	return (nonCaptures | (captures & bord->black) | (enPassent & captures));
}
U64 bitmap_black_pawns(int position, Board* bord) {
	U64 bpawns = ((1ULL << 63) >> position); // the square given
	U64 doublePawns = (bpawns & sevenRow) & ~((bord->white | bord->black) << 8); // all positions of black pawns able to move 2
	U64 nonCaptures = (((doublePawns >> 8) | (doublePawns >> 16) | (bpawns >> 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	U64 captures = ((bpawns & (~border)) >> 7 | (bpawns & (~border)) >> 9 | (bpawns & H) >> 7 | (bpawns & A) >> 9); // all capturing moves a pawn can do
	U64 enPassent = en_passent_target(bord); // all squares that are able to be en passented
	return (nonCaptures | (captures & bord->white) | (enPassent & captures));
}

U64 bitmap_white_king(int position, Board* bord) {
	U64 empty = ~(bord->white | bord->black);
	U64 castel = nothing;
	if (position == E1) {
		castel = ((((wkcastle & empty) == 6) & bord->whiteKingsideCastle) << 1) | (((wqcastle & empty) == 112) & bord->whiteQueensideCastle) << 5;//| ((bord->extra >> 15) & 1) | ((bord->extra >> 14) & 1);
	}
	return (kingMoves[position] & (~bord->white))|castel;
	/*
	U64 wkings = ((1ULL << 63) >> position); // the square given
	U64 wkings_not_on_border = wkings & (~border);
	U64 all_dirs_non_border = Down(wkings_not_on_border) | Up(wkings_not_on_border) | Left(wkings_not_on_border) | Right(wkings_not_on_border) | Down(Left(wkings_not_on_border)) | Down(Right(wkings_not_on_border)) | Up(Left(wkings_not_on_border)) | Up(Right(wkings_not_on_border));
	U64 all_dirs_non_corner = Right(wkings & A) | Up(wkings & A) | Down(wkings & A) | Left(wkings & H) | Up(wkings & H) | Down(wkings & H) | Up(wkings & oneRow) | Left(wkings & oneRow) | Right(wkings & oneRow) | Down(wkings & eightRow) | Left(wkings & eightRow) | Right(wkings & eightRow);
	U64 empty = ~(bord->white | bord->black);
	U64 castel = nothing;
	if (position == E8) {
		castel = ((((wkcastle & empty) == 6) & ((bord->extra >> 17) & 1)) << 1) | ((((wqcastle & empty) == 112) & ((bord->extra >> 16) & 1) & 1) << 5);//| ((bord->extra >> 15) & 1) | ((bord->extra >> 14) & 1);
	}
	return (((all_dirs_non_border | all_dirs_non_corner) & (~bord->white)) | castel);// &(~bitmap_white_king_danger_squares(position, bord));
	*/
}
U64 bitmap_black_king(int position, Board* bord) { //TODO use map lookup
	U64 empty = ~(bord->white | bord->black);
	U64 castel = nothing;
	if (position == E8) {
		castel = ((((bkcastle & empty) == 432345564227567616) & bord->blackKingsideCastle) << 57) | ((((bqcastle & empty) == 8070450532247928832) & bord->blackQueensideCastle) << 61);
	}
	return (kingMoves[position] & (~bord->black)) | castel;
	/*
	U64 bkings = ((1ULL << 63) >> position); // the square given
	U64 bkings_not_on_border = bkings & (~border);
	U64 all_dirs_non_border = Down(bkings_not_on_border) | Up(bkings_not_on_border) | Left(bkings_not_on_border) | Right(bkings_not_on_border) | Down(Left(bkings_not_on_border)) | Down(Right(bkings_not_on_border)) | Up(Left(bkings_not_on_border)) | Up(Right(bkings_not_on_border));
	U64 all_dirs_non_corner = Right(bkings & A) | Up(bkings & A) | Down(bkings & A) | Left(bkings & H) | Up(bkings & H) | Down(bkings & H) | Up(bkings & oneRow) | Left(bkings & oneRow) | Right(bkings & oneRow) | Down(bkings & eightRow) | Left(bkings & eightRow) | Right(bkings & eightRow);
	U64 empty = ~(bord->white | bord->black);
	U64 castel = nothing;
	if (position == E8) {
		castel = (((((bkcastle & empty) == 432345564227567616) & ((bord->extra >> 15) & 1)) << 57)) | (((((bqcastle & empty) == 8070450532247928832) & ((bord->extra >> 14) & 1)) << 61));
	}
	return (((all_dirs_non_border | all_dirs_non_corner) & (~bord->black)) | castel);// &(~bitmap_black_king_danger_squares(position, bord));
	*/
}

// rook attacks
U64 bitmap_white_rook(int square, Board* bord){
	square = 63 - square;
	U64 block = bord->white | bord->black;
	// attacks bitboard
	U64 attacks = 0ULL;

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
U64 bitmap_black_rook(int square, Board* bord){
	square = 63 - square;
	U64 block = bord->white | bord->black;
	// attacks bitboard
	U64 attacks = 0ULL;

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
U64 bitmap_white_bishop(int square, Board* bord){

	square = 63 - square;
	U64 block = bord->white | bord->black;
	// attack bitboard
	U64 attacks = 0;

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
U64 bitmap_black_bishop(int square, Board* bord){


	square = 63 - square;
	U64 block = bord->white | bord->black;
	// attack bitboard
	U64 attacks = 0;

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

/*
// mask knight attacks
U64 bitmap_white_knight (int square, Board* bord){

	square = 63 - square;
	// attack bitboard
	U64 attacks = 0;

	// piece bitboard
	U64 bitboard = 0ULL;//bord->knight & bord->white ;
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
*/
/*
// mask knight attacks
U64 bitmap_black_knight(int square, Board* bord) {

	square = 63 - square;
	// attack bitboard
	U64 attacks = 0;

	// piece bitboard
	U64 bitboard = 0ULL;//bord->knight & bord->white ;
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
*/

// mask knight attacks
U64 bitmap_white_knight(int square, Board* bord) {
	return knightMoves[square] & (~bord->white);
}

// mask knight attacks
U64 bitmap_black_knight(int square, Board* bord) {
	return knightMoves[square] & (~bord->black);
}

U64 bitmap_white_queen(int square, Board* bord) {
	return bitmap_white_rook(square, bord) | bitmap_white_bishop(square, bord);
}

U64 bitmap_black_queen(int square, Board* bord) {
	return bitmap_black_rook(square, bord) | bitmap_black_bishop(square, bord);
}

/*
* all pieces attacking the white king
*/
U64 white_checking_pieces(Board* bord) {
	U64 attackers = 0ULL; // empty bitboard
	int king_position = 63 - countTrailingZeros(bord->king & bord->white);
	attackers |= bitmap_white_rook(king_position, bord) & (bord->rook & bord->black);
	attackers |= bitmap_white_knight(king_position, bord) & (bord->knight & bord->black);
	attackers |= bitmap_white_bishop(king_position, bord) & (bord->bishop & bord->black);
	attackers |= bitmap_white_queen(king_position, bord) & (bord->queen & bord->black);
	attackers |= bitmap_white_pawns(king_position, bord) & (bord->pawn & bord->black);
	return attackers;
}

/*
* all pieces attacking the black king
*/
U64 black_checking_pieces(Board* bord) {
	U64 attackers = 0ULL; // empty bitboard
	int king_position = 63 - countTrailingZeros(bord->king & bord->black);
	attackers |= bitmap_black_rook(king_position, bord) & (bord->rook & bord->white);
	attackers |= bitmap_black_knight(king_position, bord) & (bord->knight & bord->white);
	attackers |= bitmap_black_bishop(king_position, bord) & (bord->bishop & bord->white);
	attackers |= bitmap_black_queen(king_position, bord) & (bord->queen & bord->white);
	attackers |= bitmap_black_pawns(king_position, bord) & (bord->pawn & bord->white);
	return attackers;
}

U64 squaresBetweenBitmap(int startSquare, int endSquare) {
	startSquare = 63 - startSquare;
	endSquare = 63 - endSquare;
	U64 result = 0ULL;
	/* //removed because its faster without and not realy needed (i hope)
	// Ensure valid input range (0-63)
	if (startSquare < 0 || startSquare > 63 || endSquare < 0 || endSquare > 63) {
		return result;
	}
	*/

	// Swap if endSquare is smaller than startSquare
	if (endSquare < startSquare) {
		std::swap(startSquare, endSquare);
	}

	// Calculate the difference in ranks (rows) and files (columns)
	int rankDifference = (endSquare / 8) - (startSquare / 8);
	int fileDifference = (endSquare % 8) - (startSquare % 8);

	/* //removed because normaly all inputs are valid
	if(rankDifference != fileDifference){
		return 0ULL;
	}
	*/

	// Calculate the direction of movement for rank and file
	int rankDirection = (rankDifference > 0) ? 1 : (rankDifference < 0) ? -1 : 0;
	int fileDirection = (fileDifference > 0) ? 1 : (fileDifference < 0) ? -1 : 0;

	int currentSquare = startSquare + rankDirection * 8 + fileDirection;
	while (currentSquare != endSquare) {
		result |= (1ULL << currentSquare);
		currentSquare += rankDirection * 8 + fileDirection;
	}

	return result;
}

/*
* returns all 1 if no checking pieces or path to white king if there are
*/
U64 white_checking_bitmap(Board* bord) {
	U64 att_path = 0ULL; //empty bitboard
	U64 checks = white_checking_pieces(bord);
	U64 att_rooks = checks & bord->rook;
	U64 att_knight = checks & bord->knight;
	U64 att_bishop = checks & bord->bishop;
	U64 att_queen = checks & bord->queen;
	U64 att_pawn = checks & bord->pawn;
	int wking_square = getFirst1BitSquare(bord->king & bord->white);
	while (att_rooks) {
		int rook_square = getFirst1BitSquare(att_rooks);
		att_path |= squaresBetweenBitmap(rook_square, wking_square);
		att_rooks &= (att_rooks - 1);
	}
	while (att_bishop) {
		int bishop_square = getFirst1BitSquare(att_bishop);
		att_path |= squaresBetweenBitmap(bishop_square, wking_square);
		att_bishop &= (att_bishop - 1);
	}
	while (att_queen) {
		int queen_square = getFirst1BitSquare(att_queen);
		att_path |= squaresBetweenBitmap(queen_square, wking_square);
		att_queen &= (att_queen - 1);
	}
	att_path |= att_knight;
	att_path |= att_pawn;
	if ((att_path | checks) == 0) {
		return all;
	}
	return att_path | checks;
}

/*
* returns all 1 if no checking pieces or path to black king if there are
*/
U64 black_checking_bitmap(Board* bord) {
	U64 att_path = 0ULL; //empty bitboard
	U64 checks = black_checking_pieces(bord);
	U64 att_rooks = checks & bord->rook;
	U64 att_knight = checks & bord->knight;
	U64 att_bishop = checks & bord->bishop;
	U64 att_queen = checks & bord->queen;
	U64 att_pawn = checks & bord->pawn;
	int bking_square = getFirst1BitSquare(bord->king & bord->black);
	while (att_rooks) {
		int rook_square = getFirst1BitSquare(att_rooks);
		att_path |= squaresBetweenBitmap(rook_square, bking_square);
		att_rooks &= (att_rooks - 1);
	}
	while (att_bishop) {
		int bishop_square = getFirst1BitSquare(att_bishop);
		att_path |= squaresBetweenBitmap(bishop_square, bking_square);
		att_bishop &= (att_bishop - 1);
	}
	while (att_queen) {
		int queen_square = getFirst1BitSquare(att_queen);
		att_path |= squaresBetweenBitmap(queen_square, bking_square);
		att_queen &= (att_queen - 1);
	}
	att_path |= att_knight;
	att_path |= att_pawn;
	if ((att_path | checks) == 0) {
		return all;
	}
	return att_path | checks;
}

/*
* bitmap of al squares reachable by white pieces
*/
U64 all_white_attacks(Board* bord) {
	U64 wrook = bord->white & bord->rook;
	U64 wknight = bord->white & bord->knight;
	U64 wbishop = bord->white & bord->bishop;
	U64 wqueen = bord->white & bord->queen;
	U64 wking = bord->white & bord->king;
	U64 wpawn = bord->white & bord->pawn;
	U64 attacks = 0ULL;
	attacks |= bitmap_white_king(63-countTrailingZeros(wking), bord);
	if (countSetBits(white_checking_pieces(bord)) > 1) {
		return attacks;
	}
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
	attacks |= bitmap_all_white_pawns(bord);
	return attacks;// &white_checking_bitmap(bord);
}

/*
* bitmap of al squares reachable by black pieces
*/
U64 all_black_attacks(Board* bord) {
	U64 brook = bord->black & bord->rook;
	U64 bknight = bord->black & bord->knight;
	U64 bbishop = bord->black & bord->bishop;
	U64 bqueen = bord->black & bord->queen;
	U64 bking = bord->black & bord->king;
	U64 bpawn = bord->black & bord->pawn;
	U64 attacks = 0ULL;
	attacks |= bitmap_black_king(63 - countTrailingZeros(bking), bord);;
	if (countSetBits(black_checking_pieces(bord)) > 1) {
		return attacks;
	}
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
	attacks |= bitmap_all_black_pawns(bord);
	return attacks;// &black_checking_bitmap(bord);
}

/*
* all moves generating and putting them in a movelist
*/
void white_pawn_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_white_pawns(position, bord);// &white_checking_bitmap(bord);
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
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_pawn_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_black_pawns(position, bord);// &black_checking_bitmap(bord);
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
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_rook_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_white_rook(position, bord);// &white_checking_bitmap(bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_rook_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_black_rook(position, bord);// &black_checking_bitmap(bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_knight_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_white_knight(position, bord);// &white_checking_bitmap(bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_knight_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_black_knight(position, bord);// &black_checking_bitmap(bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_bishop_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_white_bishop(position, bord);// &white_checking_bitmap(bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_bishop_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_black_bishop(position, bord);// &black_checking_bitmap(bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_queen_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = (bitmap_white_bishop(position, bord) | bitmap_white_rook(position, bord));// &white_checking_bitmap(bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->black) {
			m->capture = m->dst;
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_queen_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = (bitmap_black_bishop(position, bord) | bitmap_black_rook(position, bord));// &black_checking_bitmap(bord);
	Move* m = &movelist->moves[movelist->count];
	while (destinations) {
		int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
		m->src = (Square)position;
		m->dst = (Square)(63 - bitIndex);
		m->special = NOT_SPECIAL;
		if (((1ULL << 63) >> m->dst) & bord->white) {
			m->capture = m->dst;
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_king_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_white_king(position, bord);// &~white_checking_bitmap(bord);;
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
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void black_king_moves(int position, MOVELIST* movelist, Board* bord) {
	U64 destinations = bitmap_black_king(position, bord);// &~black_checking_bitmap(bord);
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
		}else {
			m->capture = -52;
		}
		m++;
		movelist->count++;
		destinations &= (destinations - 1); // Clear the least significant set bit
	}
}

void white_moves(MOVELIST* movelist, Board* bord) {
	U64 wrook = bord->white & bord->rook;
	U64 wknight = bord->white & bord->knight;
	U64 wbishop = bord->white & bord->bishop;
	U64 wqueen = bord->white & bord->queen;
	U64 wking = bord->white & bord->king;
	U64 wpawn = bord->white & bord->pawn;
	while (wking) {
		int bitIndex = countTrailingZeros(wking); // Get the index of the least significant set bit
		white_king_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		wking &= (wking - 1); // Clear the least significant set bit
	}
	if (countSetBits(white_checking_pieces(bord)) > 1) {
		return;
	}
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
	}
	while (wpawn) {
		int bitIndex = countTrailingZeros(wpawn); // Get the index of the least significant set bit
		white_pawn_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		wpawn &= (wpawn - 1); // Clear the least significant set bit
	}
}

void black_moves(MOVELIST* movelist, Board* bord) {
	U64 brook = bord->black & bord->rook;
	U64 bknight = bord->black & bord->knight;
	U64 bbishop = bord->black & bord->bishop;
	U64 bqueen = bord->black & bord->queen;
	U64 bking = bord->black & bord->king;
	U64 bpawn = bord->black & bord->pawn;
	while (bking) {
		int bitIndex = countTrailingZeros(bking); // Get the index of the least significant set bit
		black_king_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		bking &= (bking - 1); // Clear the least significant set bit
	}
	if (countSetBits(black_checking_pieces(bord)) > 1) {
		return;
	}
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
	}
	while (bpawn) {
		int bitIndex = countTrailingZeros(bpawn); // Get the index of the least significant set bit
		black_pawn_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
		bpawn &= (bpawn - 1); // Clear the least significant set bit
	}
}

void GenMoveList(MOVELIST* list, Board* bord) {
	// Generate all moves, including illegal (e.g. put king in check) moves
	if (white_plays(bord)) {
		white_moves(list, bord);
	}
	else {
		black_moves(list, bord);
	}
}

bool EvaluateQuick(Board* bord) {
	if (white_plays(bord)) {
		return (((bord->king & bord->white) & all_black_attacks(bord))) == 0;
	}
	else {
		return (((bord->king & bord->black) & all_white_attacks(bord))) == 0;
	}
	//return OpponentHasMoves(bord);
}

void addLegalMoveList(MOVELIST* list, Board* bord) {
	int i, j;
	bool okay;
	MOVELIST list2;
	list2.count = 0;
	PositionTracker positionTracker;

	// Generate all moves, including illegal (e.g. put king in check) moves
	if (white_plays(bord)) {
		white_moves(&list2, bord);
	}
	else {
		black_moves(&list2, bord);
	}
	Board bordCopy;
	// Loop copying the proven good ones
	for (i = j = 0; i < list2.count; i++)
	{
		copyBoard(bord, &bordCopy);
		makeMove(&bordCopy, &list2.moves[i],&positionTracker);
		//positionTracker.removePosition(&bordCopy);
		okay = EvaluateQuick(bord);

		//popMove(bord, &list2.moves[i]);
		if (okay)
			list->moves[j++] = list2.moves[i];
	}
	list->count = j;
}

void GenLegalMoveList(MOVELIST* list, Board* bord) {
	int i, j;
	bool okay;
	list->count = 0;
	MOVELIST list2;
	PositionTracker positionTracker;
	list2.count = 0;

	// Generate all moves, including illegal (e.g. put king in check) moves
	if (white_plays(bord)) {
		white_moves(&list2, bord);
	}
	else {
		black_moves(&list2, bord);
	}
	Board bordCopy;
	// Loop copying the proven good ones
	for (i = j = 0; i < list2.count; i++)
	{
		copyBoard(bord, &bordCopy);
		makeMove(&bordCopy, &list2.moves[i], &positionTracker);
		//positionTracker.removePosition(&bordCopy);
		okay = EvaluateQuick(&bordCopy);
		//popMove(bord, &list2.moves[i]);
		if (okay)
			list->moves[j++] = list2.moves[i];
	}
	list->count = j;
}

/*
* returns true if oponent has moves and false otherwise (false also means checkmated of stalemate)
*/
bool OpponentHasMoves(Board* bord) {
	int i, j;
	bool okay;
	MOVELIST list2;
	PositionTracker positionTracker;
	list2.count = 0;

	// Generate all moves, including illegal (e.g. put king in check) moves
	if (!white_plays(bord)) { // to generate moves for oponent
		white_moves(&list2, bord);
	}
	else {
		black_moves(&list2, bord);
	}
	Board bordCopy;
	// Loop copying the proven good ones
	for (i = j = 0; i < list2.count; i++)
	{
		copyBoard(bord, &bordCopy);
		makeMove(&bordCopy, &list2.moves[i], &positionTracker);
		//positionTracker.removePosition(&bordCopy);
		okay = EvaluateQuick(&bordCopy);
		if (okay) j++;
	}
	return j > 0;
}

/*
* returns true if we has moves and false otherwise (false also means checkmated of stalemate)
*/
bool weHaveMoves(Board* bord) {
	int i, j;
	bool okay;
	MOVELIST list2;
	PositionTracker positionTracker;
	list2.count = 0;

	// Generate all moves, including illegal (e.g. put king in check) moves
	if (white_plays(bord)) { // to generate moves for oponent
		white_moves(&list2, bord);
	}
	else {
		black_moves(&list2, bord);
	}
	Board bordCopy;
	// Loop copying the proven good ones
	for (i = j = 0; i < list2.count; i++)
	{
		copyBoard(bord, &bordCopy);
		makeMove(&bordCopy, &list2.moves[i], &positionTracker);
		//positionTracker.removePosition(&bordCopy);
		okay = EvaluateQuick(&bordCopy);
		if (okay) j++;
	}
	return j > 0;
}

bool inCheck(Board* bord) {
	if (white_plays(bord)) {
		return countSetBits(white_checking_pieces(bord)) != 0;
		//return ((bord->white & bord->king) & all_black_attacks(bord)) != 0;
	}else {
		return countSetBits(black_checking_pieces(bord)) != 0;
		//return ((bord->black & bord->king) & all_white_attacks(bord)) != 0;
	}
}

DRAWTYPE isDraw(Board* bord, PositionTracker* positionTracker) {
	if (positionTracker->getPositionOccurrences(bord) >= 3) return DRAWTYPE_REPITITION;
	return NOT_DRAW;
}

// Function to convert 12 sets of 64-bit numbers to a 64-character string
std::string convertTo64CharString(U64 rook, U64 knight, U64 bishop, U64 queen, U64 king, U64 pawn, U64 white, U64 black) {
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
void overlay(std::string* str, U64 bitpattern, char character) {
	std::string& myString = *str;
	size_t strLength = myString.length();

	for (size_t i = 0; i < strLength; ++i) {
		if (bitpattern & ((1ULL << 63) >> i)) {
			myString[i] = character;
		}
	}
}

void printBitBoard(U64 bitbord, std::string extra) {
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
	if (white_plays(bord)) {
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
	//printBitBoard(en_passent_target(bord), "all white pawn attacks");
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
	//extra setup
	bord->whiteToPlay = 1;
	bord->whiteKingsideCastle = 1;
	bord->whiteQueensideCastle = 1;
	bord->blackKingsideCastle = 1;
	bord->blackQueensideCastle = 1;
	bord->enPassentValid = 0;
	bord->enPassantTarget = 0;
	bord->halfmoveClock = 0;
	bord->reserved = 0;
	//bord->extra = 0b1111100000000000000;
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
	bord->whiteToPlay = 1;
	bord->whiteKingsideCastle = 1;
	bord->whiteQueensideCastle = 1;
	bord->blackKingsideCastle = 1;
	bord->blackQueensideCastle = 1;
	bord->enPassentValid = 0;
	bord->enPassantTarget = 0;
	bord->halfmoveClock = 0;
	bord->reserved = 0;
	//bord->extra = 0b1111100000000000000;
}

void addPiece(Board* bord, Pieces piece, int square) {
	U64 placeBit = ((1ULL << 63) >> square);
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
	U64 placeBit = ~((1ULL << 63) >> square);
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
	U64 sq = ((1ULL << 63) >> square);
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
	else if (bord->black & sq) {
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

Square stringToSquare(std::string inp) {
	if (inp == "a8") {
		return A8;
	}
	else if (inp == "a7") {
		return A7;
	}
	else if (inp == "a6") {
		return A6;
	}
	else if (inp == "a5") {
		return A5;
	}
	else if (inp == "a4") {
		return A4;
	}
	else if (inp == "a3") {
		return A3;
	}
	else if (inp == "a2") {
		return A2;
	}
	else if (inp == "a1") {
		return A1;
	}
	else if (inp == "b8") {
		return B8;
	}
	else if (inp == "b7") {
		return B7;
	}
	else if (inp == "b6") {
		return B6;
	}
	else if (inp == "b5") {
		return B5;
	}
	else if (inp == "b4") {
		return B4;
	}
	else if (inp == "b3") {
		return B3;
	}
	else if (inp == "b2") {
		return B2;
	}
	else if (inp == "b1") {
		return B1;
	}else if (inp == "c8") {
		return C8;
	}
	else if (inp == "c7") {
		return C7;
	}
	else if (inp == "c6") {
		return C6;
	}
	else if (inp == "c5") {
		return C5;
	}
	else if (inp == "c4") {
		return C4;
	}
	else if (inp == "c3") {
		return C3;
	}
	else if (inp == "c2") {
		return C2;
	}
	else if (inp == "c1") {
		return C1;
	}else if (inp == "d8") {
		return D8;
	}
	else if (inp == "d7") {
		return D7;
	}
	else if (inp == "d6") {
		return D6;
	}
	else if (inp == "d5") {
		return D5;
	}
	else if (inp == "d4") {
		return D4;
	}
	else if (inp == "d3") {
		return D3;
	}
	else if (inp == "d2") {
		return D2;
	}
	else if (inp == "d1") {
		return D1;
	}else if (inp == "e8") {
		return E8;
	}
	else if (inp == "e7") {
		return E7;
	}
	else if (inp == "e6") {
		return E6;
	}
	else if (inp == "e5") {
		return E5;
	}
	else if (inp == "e4") {
		return E4;
	}
	else if (inp == "e3") {
		return E3;
	}
	else if (inp == "e2") {
		return E2;
	}
	else if (inp == "e1") {
		return E1;
	}else if (inp == "f8") {
		return F8;
	}
	else if (inp == "f7") {
		return F7;
	}
	else if (inp == "f6") {
		return F6;
	}
	else if (inp == "f5") {
		return F5;
	}
	else if (inp == "f4") {
		return F4;
	}
	else if (inp == "f3") {
		return F3;
	}
	else if (inp == "f2") {
		return F2;
	}
	else if (inp == "f1") {
		return F1;
	}else if (inp == "g8") {
		return G8;
	}
	else if (inp == "g7") {
		return G7;
	}
	else if (inp == "g6") {
		return G6;
	}
	else if (inp == "g5") {
		return G5;
	}
	else if (inp == "g4") {
		return G4;
	}
	else if (inp == "g3") {
		return G3;
	}
	else if (inp == "g2") {
		return G2;
	}
	else if (inp == "g1") {
		return G1;
	}else if (inp == "h8") {
		return H8;
	}
	else if (inp == "h7") {
		return H7;
	}
	else if (inp == "h6") {
		return H6;
	}
	else if (inp == "h5") {
		return H5;
	}
	else if (inp == "h4") {
		return H4;
	}
	else if (inp == "h3") {
		return H3;
	}
	else if (inp == "h2") {
		return H2;
	}
	else if (inp == "h1") {
		return H1;
	}
	return A8;
}

std::string specialToString(SPECIAL special) {
	if (special == NOT_SPECIAL) {
		return "";
	}
	else if (special == SPECIAL_WK_CASTLING) {
		return "White is kingside casteling";
	}
	else if (special == SPECIAL_BK_CASTLING) {
		return "Black is kingside casteling";
	}
	else if (special == SPECIAL_WQ_CASTLING) {
		return "White is queenside casteling";
	}
	else if (special == SPECIAL_BQ_CASTLING) {
		return "Black is queenside casteling";
	}
	else if (special == SPECIAL_PROMOTION_QUEEN) {
		return "Promotes a pawn to a Queen";
	}
	else if (special == SPECIAL_PROMOTION_ROOK) {
		return "Promotes a pawn to a Rook";
	}
	else if (special == SPECIAL_PROMOTION_BISHOP) {
		return "Promotes a pawn to a Bishop";
	}
	else if (special == SPECIAL_PROMOTION_KNIGHT) {
		return "Promotes a pawn to a Knight";
	}
	else if (special == SPECIAL_WPAWN_2SQUARES) {
		return "Moves a White pawn 2 squares";
	}
	else if (special == SPECIAL_BPAWN_2SQUARES) {
		return "Moves a black pawn 2 squares";
	}
	else if (special == SPECIAL_WEN_PASSANT) {
		return "White captures en passent";
	}
	else if (special == SPECIAL_BEN_PASSANT) {
		return "Black captures en passent";
	}
}

void readInFen(Board* bord, std::string* fen) {
	setupEmpty(bord);
	fen->erase(std::remove(fen->begin(), fen->end(), '/'), fen->end());
	//cout << *fen << endl;
	int index = -1;
	for (char c : *fen) {
		if (c != ' ') {
			if (std::isdigit(c)) {
				index += (c - '0');
			}
			else {
				index += 1;
			}
			//std::cout << c << " at " << index << endl;
			if (c == 'r') {
				addPiece(bord, BROOK, index);
			}else if (c == 'n') {
				addPiece(bord, BKNIGHT, index);
			}else if (c == 'b') {
				addPiece(bord, BBISCHOP, index);
			}else if (c == 'q') {
				addPiece(bord, BQUEEN, index);
			}else if (c == 'k') {
				addPiece(bord, BKING, index);
			}else if (c == 'p') {
				addPiece(bord, BPAWN, index);
			}else if (c == 'R') {
				addPiece(bord, WROOK, index);
			}else if (c == 'N') {
				addPiece(bord, WKNIGHT, index);
			}else if (c == 'B') {
				addPiece(bord, WBISCHOP, index);
			}else if (c == 'Q') {
				addPiece(bord, WQUEEN, index);
			}else if (c == 'K') {
				addPiece(bord, WKING, index);
			}else if (c == 'P') {
				addPiece(bord, WPAWN, index);
			}
		}
		else {
			//printBoard(bord);
			// Find the position of the first space
			size_t spacePos = fen->find(' ');

			// Extract the substring starting from the character immediately after the space
			std::string result = fen->substr(spacePos + 1);
			//bord->extra = 0b0000000000000000000;
			bord->whiteToPlay = 0;
			bord->whiteKingsideCastle = 0;
			bord->whiteQueensideCastle = 0;
			bord->blackKingsideCastle = 0;
			bord->blackQueensideCastle = 0;
			bord->enPassentValid = 0;
			bord->enPassantTarget = 0;
			bord->halfmoveClock = 0;
			bord->reserved = 0;
			if (result.front() == 'w') {
				bord->whiteToPlay = 1;
				//bord->extra |= 0b1000000000000000000;
			}
			result.erase(0, 2);
			if (result.front() == 'K') {
				result.erase(0, 1);
				bord->whiteKingsideCastle = 1;
				//bord->extra |= 0b0100000000000000000;
			}
			if (result.front() == 'Q') {
				result.erase(0, 1);
				bord->whiteQueensideCastle = 1;
				//bord->extra |= 0b0010000000000000000;
			}
			if (result.front() == 'k') {
				result.erase(0, 1);
				bord->blackKingsideCastle = 1;
				//bord->extra |= 0b0001000000000000000;
			}
			if (result.front() == 'q') {
				result.erase(0, 1);
				bord->blackQueensideCastle = 1;
				//bord->extra |= 0b0000100000000000000;
			}
			if (result.front() == '-') {
				result.erase(0, 1);
			}
			result.erase(0, 1);
			if (result.front() != '-') {
				std::string temp = result.substr(0,2);
				Square enPassent = stringToSquare(temp);
				result.erase(0, 3);
				bord->enPassentValid = 1;
				bord->enPassantTarget = enPassent;
				//bord->extra |= 0b0000010000000000000;
				//bord->extra |= (enPassent << 7);
			}
			else {
				result.erase(0, 2);
			}
			std::string halfm = result.substr(0, result.find(' '));
			int halfmcount = std::stoi(halfm);
			bord->halfmoveClock = halfmcount;
			//bord->extra |= (halfmcount);


			//std::cout << std::bitset<19>(bord->extra) << std::endl;
			//std::cout << "Stripped string: " << result << std::endl;
			break; // Stop when a space is encountered
		}
	}
}

// Function to make a move
void makeMove(Board* bord, Move* move, PositionTracker* positionTracker) {
	//bord->extra = (bord->extra & ~0x7F) | ((bord->extra + 1) & 0x7F); //TODO halfmove clock
	//lastCapturedPiece = NOPIECE; //TODO needed if i make a popMove
	// Update the pawns bitboard to reflect the move
	U64 fromBit = ((1ULL << 63) >> move->src);
	U64 toBit = ((1ULL << 63) >> move->dst);
	bool capture = false;

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
			//lastCapturedPiece = pieceAt(move->dst, bord); //TODO needed if i make a popMove
			bord->rook &= ~toBit;
			bord->knight &= ~toBit;
			bord->bishop &= ~toBit;
			bord->queen &= ~toBit;
			bord->king &= ~toBit;
			bord->pawn &= ~toBit;
			bord->white &= ~toBit;
			bord->black &= ~toBit;
			capture = true;
		}

		if ((bord->white & fromBit) != 0) {
			bord->white ^= fromBit; // Clear the source square
			bord->white |= toBit;   // Set the destination square
		}else if ((bord->black & fromBit) != 0) {
			bord->black ^= fromBit; // Clear the source square
			bord->black |= toBit;   // Set the destination square
		}

		if ((bord->rook & fromBit) != 0) {
			bord->halfmoveClock += 1;
			//bord->extra = (bord->extra & ~0x7F) | (((bord->extra & 0x7F) + 1) & 0x7F);
			if ((((1ULL << 63) >> (A8)) & fromBit) != 0) {
				bord->whiteKingsideCastle = 0;
				//bord->extra &= ~(1ULL << 14); // remove white kingside casteling ability
			}
			else if ((((1ULL << 63) >> (H8)) & fromBit) != 0) {
				bord->whiteQueensideCastle = 0;
				//bord->extra &= ~(1ULL << 15); // remove white queenside casteling ability
			}
			else if ((((1ULL << 63) >> (A1)) & fromBit) != 0) {
				bord->blackKingsideCastle = 0;
				//bord->extra &= ~(1ULL << 16); // remove black kingside casteling ability
			}
			else if ((((1ULL << 63) >> (H1)) & fromBit) != 0) {
				bord->blackQueensideCastle = 0;
				//bord->extra &= ~(1ULL << 17); // remove black queenside casteling ability
			}
			bord->rook ^= fromBit; // Clear the source square
			bord->rook |= toBit;   // Set the destination square
		}else if ((bord->knight & fromBit) != 0) {
			bord->halfmoveClock += 1;
			bord->knight ^= fromBit; // Clear the source square
			bord->knight |= toBit;   // Set the destination square
		}else if ((bord->bishop & fromBit) != 0) {
			bord->halfmoveClock += 1;
			bord->bishop ^= fromBit; // Clear the source square
			bord->bishop |= toBit;   // Set the destination square
		}else if ((bord->queen & fromBit) != 0) {
			bord->halfmoveClock += 1;
			bord->queen ^= fromBit; // Clear the source square
			bord->queen |= toBit;   // Set the destination square
		}else if ((bord->king & fromBit) != 0) {
			bord->halfmoveClock += 1;
			if ((bord->white & fromBit) != 0) { //remove white casteling ability
				bord->whiteKingsideCastle = 0;
				bord->whiteQueensideCastle = 0;
				//bord->extra &= ~((((1ULL << 2) - 1) << 16));
			}
			else if ((bord->black & fromBit) != 0) { //remove black casteling ability
				bord->blackKingsideCastle = 0;
				bord->blackQueensideCastle = 0;
				//bord->extra &= ~((((1ULL << 2) - 1) << 14));
			}
			bord->king ^= fromBit; // Clear the source square
			bord->king |= toBit;   // Set the destination square
		}else if ((bord->pawn & fromBit) != 0) {
			bord->halfmoveClock = 0;
			bord->pawn ^= fromBit; // Clear the source square
			bord->pawn |= toBit;   // Set the destination square
		}
	}
	bord->enPassentValid = 0;
	//bord->extra &= ~((((1ULL << 7) - 1) << 7)); // remove en passent target
	if (move->special != NOT_SPECIAL) {
		if (move->special == SPECIAL_WPAWN_2SQUARES) {
			bord->enPassentValid = 1;
			bord->enPassantTarget = (move->dst + 8);
			//bord->extra |= ((1ULL << 13));
			//bord->extra |= ((move->dst + 8) << 7);
		}
		else if (move->special == SPECIAL_BPAWN_2SQUARES) {
			bord->enPassentValid = 1;
			bord->enPassantTarget = (move->dst - 8);
			//bord->extra |= ((1ULL << 13));
			//bord->extra |= ((move->dst - 8) << 7);
		}
		else if (move->special == SPECIAL_WEN_PASSANT) {
			U64 enPassentBit = ((1ULL << 63) >> (move->dst + 8));
			//clear all bitboards on the to position of the en passant pawn
			bord->pawn &= ~enPassentBit;
			bord->white &= ~enPassentBit;
			bord->black &= ~enPassentBit;
		}
		else if (move->special == SPECIAL_BEN_PASSANT) {
			U64 enPassentBit = ((1ULL << 63) >> (move->dst - 8));
			//clear all bitboards on the to position of the en passant pawn
			bord->pawn &= ~enPassentBit;
			bord->white &= ~enPassentBit;
			bord->black &= ~enPassentBit;
		}
		else if (move->special == SPECIAL_WK_CASTLING) { //remove white casteling ability
			bord->whiteKingsideCastle = 0;
			bord->whiteQueensideCastle = 0;
			//bord->extra &= ~((((1ULL << 2) - 1) << 16));
			U64 rookSQ = ((1ULL << 63) >> (H1));
			bord->rook &= ~rookSQ;
			bord->white &= ~rookSQ;
			U64 kingSQ = ((1ULL << 63) >> (E1));
			bord->king &= ~kingSQ;
			bord->white &= ~kingSQ;

			U64 newRookSQ = ((1ULL << 63) >> (F1));
			bord->rook |= newRookSQ;
			bord->white |= newRookSQ;
			U64 newKingSQ = ((1ULL << 63) >> (G1));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
		}
		else if (move->special == SPECIAL_BK_CASTLING) { //remove black casteling ability
			bord->blackKingsideCastle = 0;
			bord->blackQueensideCastle = 0;
			//bord->extra &= ~((((1ULL << 2) - 1) << 14));
			U64 rookSQ = ((1ULL << 63) >> (H8));
			bord->rook &= ~rookSQ;
			bord->black &= ~rookSQ;
			U64 kingSQ = ((1ULL << 63) >> (E8));
			bord->king &= ~kingSQ;
			bord->black &= ~kingSQ;

			U64 newRookSQ = ((1ULL << 63) >> (F8));
			bord->rook |= newRookSQ;
			bord->black |= newRookSQ;
			U64 newKingSQ = ((1ULL << 63) >> (G8));
			bord->king |= newKingSQ;
			bord->black |= newKingSQ;
		}
		else if (move->special == SPECIAL_WQ_CASTLING) { //remove white casteling ability
			bord->whiteKingsideCastle = 0;
			bord->whiteQueensideCastle = 0;
			//bord->extra &= ~((((1ULL << 2) - 1) << 16));
			U64 rookSQ = ((1ULL << 63) >> (A1));
			bord->rook &= ~rookSQ;
			bord->white &= ~rookSQ;
			U64 kingSQ = ((1ULL << 63) >> (E1));
			bord->king &= ~kingSQ;
			bord->white &= ~kingSQ;

			U64 newRookSQ = ((1ULL << 63) >> (D1));
			bord->rook |= newRookSQ;
			bord->white |= newRookSQ;
			U64 newKingSQ = ((1ULL << 63) >> (C1));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
		}
		else if (move->special == SPECIAL_BQ_CASTLING) { //remove black casteling ability
			bord->blackKingsideCastle = 0;
			bord->blackQueensideCastle = 0;
			//bord->extra &= ~((((1ULL << 2) - 1) << 14));
			U64 rookSQ = ((1ULL << 63) >> (A8));
			bord->rook &= ~rookSQ;
			bord->black &= ~rookSQ;
			U64 kingSQ = ((1ULL << 63) >> (E8));
			bord->king &= ~kingSQ;
			bord->black &= ~kingSQ;

			U64 newRookSQ = ((1ULL << 63) >> (D8));
			bord->rook |= newRookSQ;
			bord->black |= newRookSQ;
			U64 newKingSQ = ((1ULL << 63) >> (C8));
			bord->king |= newKingSQ;
			bord->black |= newKingSQ;
		}
		else if (move->special == SPECIAL_PROMOTION_BISHOP) {
			if (!white_plays(bord)) {
				U64 promotionPawn = oneRow & bord->pawn & bord->black;
				bord->pawn &= ~promotionPawn;
				bord->bishop |= promotionPawn;
			}
			else {
				U64 promotionPawn = eightRow & bord->pawn & bord->white;
				bord->pawn &= ~promotionPawn;
				bord->bishop |= promotionPawn;
			}
		}
		else if (move->special == SPECIAL_PROMOTION_KNIGHT) {
			if (!white_plays(bord)) {
				U64 promotionPawn = oneRow & bord->pawn & bord->black;
				bord->pawn &= ~promotionPawn;
				bord->knight |= promotionPawn;
			}
			else {
				U64 promotionPawn = eightRow & bord->pawn & bord->white;
				bord->pawn &= ~promotionPawn;
				bord->knight |= promotionPawn;
			}
		}
		else if (move->special == SPECIAL_PROMOTION_QUEEN) {
			if (!white_plays(bord)) {
				U64 promotionPawn = oneRow & bord->pawn & bord->black;
				bord->pawn &= ~promotionPawn;
				bord->queen |= promotionPawn;
			}
			else {
				U64 promotionPawn = eightRow & bord->pawn & bord->white;
				bord->pawn &= ~promotionPawn;
				bord->queen |= promotionPawn;
			}
		}
		else if (move->special == SPECIAL_PROMOTION_ROOK) {
			if (!white_plays(bord)) {
				U64 promotionPawn = oneRow & bord->pawn & bord->black;
				bord->pawn &= ~promotionPawn;
				bord->rook |= promotionPawn;
			}
			else {
				U64 promotionPawn = eightRow & bord->pawn & bord->white;
				bord->pawn &= ~promotionPawn;
				bord->rook |= promotionPawn;
			}
		}
	}
	bord->whiteToPlay = bord->whiteToPlay ? 0 : 1;
	//bord->extra ^= (1ULL << 18); // swap playing player
	positionTracker->addPosition(bord);
	if (capture) bord->halfmoveClock = 0;

}

/*
// Function to remove a move
void popMove(Board* bord, Move* move) {
	// Update the pawns bitboard to reflect the move
	U64 fromBit = ((1ULL << 63) >> move->src);
	U64 toBit = ((1ULL << 63) >> move->dst);

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
		*/
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
/*
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
		*/
			/* //TODO
			if ((bord->white & fromBit) != 0) {
				bord->extra &= ~((((1ULL << 2) - 1) << 16));
			}
			else if ((bord->black & fromBit) != 0) {
				bord->extra &= ~((((1ULL << 2) - 1) << 14));
			}
			*/
/*
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
		*/
		/* //TODO
		else if (move->special == SPECIAL_WEN_PASSANT) {
			U64 enPassentBit = ((1ULL << 63) >> (move->dst + 8));
			//clear all bitboards on the to position of the en passant pawn
			bord->pawn &= ~enPassentBit;
			bord->white &= ~enPassentBit;
			bord->black &= ~enPassentBit;
		}
		else if (move->special == SPECIAL_BEN_PASSANT) {
			U64 enPassentBit = ((1ULL << 63) >> (move->dst - 8));
			//clear all bitboards on the to position of the en passant pawn
			bord->pawn &= ~enPassentBit;
			bord->white &= ~enPassentBit;
			bord->black &= ~enPassentBit;
		}
		*/
		/* //TODO
		else if (move->special == SPECIAL_WK_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 16));
			U64 rookSQ = ((1ULL << 63) >> (H1));
			bord->rook &= ~rookSQ;
			bord->white &= ~rookSQ;
			U64 kingSQ = ((1ULL << 63) >> (E1));
			bord->king &= ~kingSQ;
			bord->white &= ~kingSQ;

			U64 newRookSQ = ((1ULL << 63) >> (F1));
			bord->rook |= newRookSQ;
			bord->white |= newRookSQ;
			U64 newKingSQ = ((1ULL << 63) >> (G1));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
		}
		else if (move->special == SPECIAL_BK_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 14));
			U64 rookSQ = ((1ULL << 63) >> (H8));
			bord->rook &= ~rookSQ;
			bord->black &= ~rookSQ;
			U64 kingSQ = ((1ULL << 63) >> (E8));
			bord->king &= ~kingSQ;
			bord->black &= ~kingSQ;

			U64 newRookSQ = ((1ULL << 63) >> (F8));
			bord->rook |= newRookSQ;
			bord->black |= newRookSQ;
			U64 newKingSQ = ((1ULL << 63) >> (G8));
			bord->king |= newKingSQ;
			bord->black |= newKingSQ;
		}
		else if (move->special == SPECIAL_WQ_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 16));
			U64 rookSQ = ((1ULL << 63) >> (A1));
			bord->rook &= ~rookSQ;
			bord->white &= ~rookSQ;
			U64 kingSQ = ((1ULL << 63) >> (E1));
			bord->king &= ~kingSQ;
			bord->white &= ~kingSQ;

			U64 newRookSQ = ((1ULL << 63) >> (D1));
			bord->rook |= newRookSQ;
			bord->white |= newRookSQ;
			U64 newKingSQ = ((1ULL << 63) >> (C1));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
		}
		else if (move->special == SPECIAL_BQ_CASTLING) {
			bord->extra &= ~((((1ULL << 2) - 1) << 14));
			U64 rookSQ = ((1ULL << 63) >> (A8));
			bord->rook &= ~rookSQ;
			bord->black &= ~rookSQ;
			U64 kingSQ = ((1ULL << 63) >> (E8));
			bord->king &= ~kingSQ;
			bord->black &= ~kingSQ;

			U64 newRookSQ = ((1ULL << 63) >> (D8));
			bord->rook |= newRookSQ;
			bord->black |= newRookSQ;
			U64 newKingSQ = ((1ULL << 63) >> (C8));
			bord->king |= newKingSQ;
			bord->black |= newKingSQ;
		}
		*/
/*
		else if (move->special == SPECIAL_PROMOTION_BISHOP) {
			U64 promotionPawn = bord->bishop & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->bishop &= ~promotionPawn;
		}
		else if (move->special == SPECIAL_PROMOTION_KNIGHT) {
			U64 promotionPawn = bord->knight & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->knight &= ~promotionPawn;
		}
		else if (move->special == SPECIAL_PROMOTION_QUEEN) {
			U64 promotionPawn = bord->queen & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->queen &= ~promotionPawn;
		}
		else if (move->special == SPECIAL_PROMOTION_ROOK) {
			U64 promotionPawn = bord->rook & ((1ULL << move->dst));
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
*/