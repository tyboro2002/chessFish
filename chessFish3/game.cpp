// chessFish3.cpp : Defines the entry point for the application.
//

#include "game.h"
#include <bitset>

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
constexpr unsigned long long border   = 0b1111111110000001100000011000000110000001100000011000000111111111;

constexpr unsigned long long tempB = (1ULL << 63) >> (D5);

using namespace std;

unsigned long long bitmap_white_pawns(Board* bord) {
	unsigned long long wpawns = bord->pawn & bord->white; // all positions of white pawns
	unsigned long long doublePawns = (wpawns & twoRow); // all positions of white pawns able to move 2
	unsigned long long nonCaptures = (((doublePawns << 8) | (doublePawns << 16) | (wpawns << 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	unsigned long long captures = ((wpawns & (~border)) << 7 | (wpawns & (~border)) << 9 | (wpawns & H) << 9 | (wpawns & A) << 7); // all capturing moves a pawn can do
	unsigned long long enPassent = (~((((bord->extra & ((1ULL << 13))) >> 13) << 64) - 1)) & ((((1ULL << 63) >> (((bord->extra >> 7) << 58) >> 58)))); // all squares that are able to be en passented
	unsigned long long white = ~((((bord->extra & (1ULL << 18)) >> 18) << 64) - 1); //64 1bits if white is in play 0 otherwise
	return (nonCaptures | (captures & bord->black) | (enPassent & white & captures)) & white;
}

unsigned long long bitmap_black_pawns(Board* bord) {
	unsigned long long bpawns = bord->pawn & bord->black; // all positions of black pawns
	unsigned long long doublePawns = (bpawns & sevenRow); // all positions of black pawns able to move 2
	unsigned long long nonCaptures = (((doublePawns >> 8) | (doublePawns >> 16) | (bpawns >> 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
	unsigned long long captures = ((bpawns & (~border)) >> 7 | (bpawns & (~border)) >> 9 | (bpawns & H) >> 7 | (bpawns & A) >> 9); // all capturing moves a pawn can do
	unsigned long long enPassent = (~((((bord->extra & ((1ULL << 13))) >> 13) << 64) - 1)) & ((((1ULL << 63) >> (((bord->extra >> 7) << 58) >> 58)))); // all squares that are able to be en passented
	unsigned long long black = ((((bord->extra & (1ULL << 18)) >> 18) << 64) - 1); //64 1bits if white is in play 0 otherwise
	return (captures & bord->white); //(nonCaptures | (captures & bord->white) | (enPassent & black & captures)) & black;
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

void printBoard(Board* bord){
	std::string temp = convertTo64CharString(bord->rook, bord->knight, bord->bishop, bord->queen, bord->king, bord->pawn, bord->white, bord->black);
	overlay(&temp, bitmap_black_pawns(bord), 'X');
	//overlay(&temp, tempB, 'O');
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
	cout << std::bitset<18>(bord->extra) << endl;
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

// Function to make a move
void makeMove(Board* bord, Move* move) {
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
			bord->white |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (G8));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
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
			bord->white |= newRookSQ;
			unsigned long long newKingSQ = ((1ULL << 63) >> (C8));
			bord->king |= newKingSQ;
			bord->white |= newKingSQ;
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