// chessFish3.cpp : Defines the entry point for the application.
//

#include "chessFish3.h"
#include "game.h"
#include <chrono>

using namespace std;

void time_code(Board* bord) {
	//timing code
	// Get the starting timestamp
	auto startTime = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 1000000; ++i) {
		//bitmap_white_pawns(bord) | bitmap_black_pawns(bord) | bitmap_black_king(bord) | bitmap_white_king(bord);
		bitmap_white_knight(A5, bord);
	}

	// Get the ending timestamp
	auto endTime = std::chrono::high_resolution_clock::now();

	// Calculate the duration in microseconds (change to other duration units as needed)
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

	std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

int main()
{
	Board bord;
	Move move;
	//setup(&bord);
	setupEmpty(&bord);
	printBoard(&bord);
	addPiece(&bord, WKNIGHT, A2);
	addPiece(&bord, WKING, C1);
	//addPiece(&bord, BPAWN, F7);
	printBoard(&bord);

	//time_code(&bord);

	/*
	move.src = E2;
	move.dst = E4;
	move.special = SPECIAL_WPAWN_2SQUARES;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = E7;
	move.dst = E5;
	move.special = SPECIAL_BPAWN_2SQUARES;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = B2;
	move.dst = B4;
	move.special = SPECIAL_WPAWN_2SQUARES;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = F7;
	move.dst = F6;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = B4;
	move.dst = B5;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = C7;
	move.dst = C5;
	move.special = SPECIAL_BPAWN_2SQUARES;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = B5;
	move.dst = C6;
	move.special = SPECIAL_WEN_PASSANT;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = G7;
	move.dst = G5;
	move.special = SPECIAL_BPAWN_2SQUARES;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = A2;
	move.dst = A3;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = G5;
	move.dst = G4;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = H2;
	move.dst = H4;
	move.special = SPECIAL_WPAWN_2SQUARES;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = G4;
	move.dst = H3;
	move.special = SPECIAL_BEN_PASSANT;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = G1;
	move.dst = F3;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = G8;
	move.dst = H6;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = F1;
	move.dst = A6;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = F8;
	move.dst = B4;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = E1;
	move.dst = G1;
	move.special = SPECIAL_WK_CASTLING;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = E8;
	move.dst = G8;
	move.special = SPECIAL_BK_CASTLING;
	makeMove(&bord, &move);
	printBoard(&bord);

	time_code(&bord);
	*/
	/*
    // illegal test moves
	move.src = H3;
	move.dst = H1;
	move.special = SPECIAL_PROMOTION_ROOK;
	makeMove(&bord, &move);
	printBoard(&bord);

	move.src = E4;
	move.dst = E8;
	move.special = SPECIAL_PROMOTION_ROOK;
	makeMove(&bord, &move);
	printBoard(&bord);
	*/

	return 0;
}
