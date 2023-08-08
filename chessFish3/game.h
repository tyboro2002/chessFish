// game.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

typedef unsigned long long U64;

// Calculate an upper limit to the length of a list of moves
#define MAXMOVES (27 + 2*13 + 2*14 + 2*8 + 8 + 8*4  +  3*27)
				//[Q   2*B    2*R    2*N   K   8*P] +  [3*Q]
				//             ^                         ^
				//[calculated practical maximum   ] + [margin]

// TODO: Reference additional headers your program requires here.
struct Board {
	// pieces
	U64 rook;
	U64 knight;
	U64 bishop;
	U64 queen;
	U64 king;
	U64 pawn;
	// colors
	U64 white;
	U64 black;
	// extra info
	U64 extra;
	// extra info consists of 19 bits meaning
	// 1) white to play
	// 2) white kingside casteling aviable
	// 3) white queenside casteling aviable
	// 4) black kingside casteling aviable
	// 5) black queenside casteling aviable
	// 6,7,8,9,10,11,12) posible en passent target (bit 6 displays if data is valid)
	// 13,14,15,16,17,18,19) halfmove clock (if 100 (dec) its draw)
};

enum Square {
	A8 = 0, B8, C8, D8, E8, F8, G8, H8,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A1, B1, C1, D1, E1, F1, G1, H1,
};

enum Pieces {
	NOPIECE = 0,
	WROOK,
	WKNIGHT,
	WBISCHOP,
	WQUEEN,
	WKING,
	WPAWN,
	BROOK,
	BKNIGHT,
	BBISCHOP,
	BQUEEN,
	BKING,
	BPAWN,
};

enum SPECIAL
{
	NOT_SPECIAL = 0,
	SPECIAL_WK_CASTLING,
	SPECIAL_BK_CASTLING,
	SPECIAL_WQ_CASTLING,
	SPECIAL_BQ_CASTLING,
	SPECIAL_PROMOTION_QUEEN,
	SPECIAL_PROMOTION_ROOK,
	SPECIAL_PROMOTION_BISHOP,
	SPECIAL_PROMOTION_KNIGHT,
	SPECIAL_WPAWN_2SQUARES,
	SPECIAL_BPAWN_2SQUARES,
	SPECIAL_WEN_PASSANT,
	SPECIAL_BEN_PASSANT,
};

// Results of a test for legal position, note that they are powers
//  of 2, allowing a mask of reasons
enum ILLEGAL_REASON
{
	IR_NULL = 0, IR_PAWN_POSITION = 1, //pawns on 1st or 8th rank
	IR_NOT_ONE_KING_EACH = 2, IR_CAN_TAKE_KING = 4,
	IR_WHITE_TOO_MANY_PIECES = 8, IR_WHITE_TOO_MANY_PAWNS = 16,
	IR_BLACK_TOO_MANY_PIECES = 32, IR_BLACK_TOO_MANY_PAWNS = 64
};

enum DRAWTYPE
{
	NOT_DRAW,
	DRAWTYPE_50MOVE,
	DRAWTYPE_INSUFFICIENT,      // draw if superior side wants it
	//  since inferior side has insufficient
	//  mating material
	DRAWTYPE_INSUFFICIENT_AUTO, // don't wait to be asked, e.g. draw
	//  immediately if bare kings
	DRAWTYPE_REPITITION,
};

// Stalemate or checkmate game terminations
enum TERMINAL
{
	NOT_TERMINAL = 0,
	TERMINAL_WCHECKMATE = -1,   // White is checkmated
	TERMINAL_WSTALEMATE = -2,   // White is stalemated
	TERMINAL_BCHECKMATE = 1,    // Black is checkmated
	TERMINAL_BSTALEMATE = 2     // Black is stalemated
};

class Move{
public:
	// Move is a lightweight type, it is accommodated in only 32 bits
	Square  src : 8;
	Square  dst : 8;
	SPECIAL special : 8;
	int     capture : 8;
	// ' ' (empty) if move not a capture
	// for some reason Visual C++ 2005 (at least)
	// blows sizeof(Move) out to 64 bits if
	// capture is defined as char instead of int

	bool operator ==(const Move& other) const { return(*((int32_t*)this) == *((int32_t*)(&other)));}

	bool operator !=(const Move& other) const { return(*((int32_t*)this) != *((int32_t*)(&other)));}
};

// List of moves
struct MOVELIST{
	int count;  // number of moves
	Move moves[MAXMOVES];
};

int countSetBits(U64 number);

void setup(Board* bord);
void setupEmpty(Board* bord);
void addPiece(Board* bord, Pieces piece, int square);
void clearSquare(Board* bord, int square);
void printBoard(Board* bord);
void printBitBoard(U64 bitbord, std::string extra);
void makeMove(Board* bord, Move* move);
void popMove(Board* bord, Move* move);

void white_pawn_moves(int position, MOVELIST* movelist, Board* bord);
void black_pawn_moves(int position, MOVELIST* movelist, Board* bord);
void white_rook_moves(int position, MOVELIST* movelist, Board* bord);
void black_rook_moves(int position, MOVELIST* movelist, Board* bord);
void white_knight_moves(int position, MOVELIST* movelist, Board* bord);
void black_knight_moves(int position, MOVELIST* movelist, Board* bord);
void white_bishop_moves(int position, MOVELIST* movelist, Board* bord);
void black_bishop_moves(int position, MOVELIST* movelist, Board* bord);
void white_queen_moves(int position, MOVELIST* movelist, Board* bord);
void black_queen_moves(int position, MOVELIST* movelist, Board* bord);
void white_king_moves(int position, MOVELIST* movelist, Board* bord);
void black_king_moves(int position, MOVELIST* movelist, Board* bord);
void white_moves(MOVELIST* movelist, Board* bord);
void black_moves(MOVELIST* movelist, Board* bord);

U64 bitmap_all_white_pawns(Board* bord);
U64 bitmap_all_black_pawns(Board* bord);
U64 bitmap_all_white_king(Board* bord, int diepte);
U64 bitmap_all_black_king(Board* bord, int diepte);

U64 white_checking_pieces(Board* bord);
U64 black_checking_pieces(Board* bord);

U64 bitmap_white_pawns(int position, Board* bord);
U64 bitmap_black_pawns(int position, Board* bord);
U64 bitmap_white_king(int position, Board* bord);
U64 bitmap_black_king(int position, Board* bord);
U64 bitmap_white_rook(int position, Board* bord);
U64 bitmap_black_rook(int position, Board* bord);
U64 bitmap_white_bishop(int square, Board* bord);
U64 bitmap_black_bishop(int square, Board* bord);
U64 bitmap_white_knight(int square, Board* bord);
U64 bitmap_black_knight(int square, Board* bord);

U64 all_white_attacks(Board* bord, int diepte);
U64 all_black_attacks(Board* bord, int diepte);

//void GenLegalMoveList(MOVELIST* list, Board* bord);
void GenMoveList(MOVELIST* list, Board* bord);
void GenLegalMoveList(MOVELIST* list, Board* bord);

U64 squaresBetweenBitmap(int startSquare, int endSquare);
U64 white_checking_bitmap(Board* bord);
U64 black_checking_bitmap(Board* bord);

void copyBoard(const Board* bordIn, Board* bordOut);
void readInFen(Board* bord, std::string* fen);


std::string squareToString(Square square);
std::string specialToString(SPECIAL special);
std::string moveToString(Move* move);