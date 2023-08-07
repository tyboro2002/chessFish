// chessFish3.cpp : Defines the entry point for the application.
//

#include "chessFish3.h"
#include "game.h"
#include <chrono>
#include <random>

using namespace std;

/*
* helper functions
*/
int generateRandomNumber(int upperBound) {
	upperBound = upperBound - 1; //remove upper bound from posible outcomes
	std::random_device rd; // Seed the random number generator
	std::mt19937 gen(rd()); // Mersenne Twister engine
	std::uniform_int_distribution<> distribution(0, upperBound);

	return distribution(gen);
}

void makeRandomMove(Board* bord, MOVELIST* moveList) {
	GenLegalMoveList(moveList, bord);
	int choosen = generateRandomNumber(moveList->count);
	cout << moveList->count << endl;
	makeMove(bord, &moveList->moves[choosen]);
	printBoard(bord);
}

/*
* testing functions
*/

void time_code() {
	Board bord;
	Move move;
	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move

	setup(&bord);
	printBoard(&bord);
	//timing code
	// Get the starting timestamp
	auto startTime = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 1000000; ++i) {
		//bitmap_white_pawns(bord) | bitmap_black_pawns(bord) | bitmap_black_king(bord) | bitmap_white_king(bord);
		all_white_attacks(&bord,1) | all_black_attacks(&bord,1);
	}

	// Get the ending timestamp
	auto endTime = std::chrono::high_resolution_clock::now();

	// Calculate the duration in microseconds (change to other duration units as needed)
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

	std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

void king_danger_squares_test() {
	Board bord;
	Move move;
	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move

	setup(&bord);
	setupEmpty(&bord);
	printBoard(&bord);
	addPiece(&bord, BKING, E7);
	addPiece(&bord, BROOK, E3);
	addPiece(&bord, WKING, E1);
	addPiece(&bord, WROOK, E4);
	addPiece(&bord, WROOK, H7);
	printBoard(&bord);
	printBitBoard(all_black_attacks(&bord, 1), "black moves");
	printBitBoard(bitmap_black_king(E7,&bord), "black king moves");
	printBitBoard(black_checking_pieces(&bord), "black checking pieces");

	black_king_moves(E7, &moveList, &bord);

	cout << endl;
}

void path_test() {
	printBitBoard(squaresBetweenBitmap(A8, H1), "between A8 H1");
	printBitBoard(squaresBetweenBitmap(C7, C2), "between C7 C2");
	printBitBoard(squaresBetweenBitmap(H2, D6), "between H2 D6");
	//printBitBoard(squaresBetweenBitmap(H3, D6), "between H3 D6");
	printBitBoard(squaresBetweenBitmap(H3, H3), "between H3 H3");
}

void checking_test() {
	Board bord;
	Move move;
	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move

	setup(&bord);
	setupEmpty(&bord);
	printBoard(&bord);
	addPiece(&bord, BKING, E7);
	addPiece(&bord, BROOK, E3);
	addPiece(&bord, WKING, E1);
	addPiece(&bord, WROOK, E4);
	addPiece(&bord, WROOK, H7);
	addPiece(&bord, WKNIGHT, G6);
	printBoard(&bord);
	printBitBoard( black_checking_bitmap(&bord),"black checking");

	black_king_moves(E7, &moveList, &bord);

	cout << endl;
}

void move_test() {
	Board bord;
	Move move;
	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move

	setup(&bord);
	printBoard(&bord);

	move.src = F8;
	move.dst = B4;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &move);
	printBoard(&bord);
}

void move_test_check() {
	Board bord;
	Move move;
	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move

	setupEmpty(&bord);
	addPiece(&bord, WKING, A4);
	addPiece(&bord, BROOK, F4);
	addPiece(&bord, BKING, D6);
	addPiece(&bord, WKNIGHT, D3);
	printBoard(&bord);
	printBitBoard(all_white_attacks(&bord, 1), "all white attacks");

}

void time_bitwise_code() {
	//timing code
	// Get the starting timestamp
	auto startTime = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 1000000; ++i) {
		countSetBits(3);
	}

	// Get the ending timestamp
	auto endTime = std::chrono::high_resolution_clock::now();

	// Calculate the duration in microseconds (change to other duration units as needed)
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

	std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

void fen_test() {
	Board bord;
	setupEmpty(&bord);
	std::string fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
	readInFen(&bord, &fen);
}

void legalMoveTest() {
	Board bord;
	Move move;
	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move

	setupEmpty(&bord);
	std::string fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
	readInFen(&bord, &fen);

	//GenLegalMoveList(&moveList, &bord);
	//cout << (moveList.count == 22) << endl;

	setupEmpty(&bord);
	fen = "rnb1kbnr/pp1p1ppp/8/2p1Q3/2q1P3/8/PPPP1PPP/RNB1KB1R b KQkq - 1 2";
	readInFen(&bord, &fen);

	GenLegalMoveList(&moveList, &bord);
	cout << endl;
}

void randomMoveTest() {
	Board bord;
	Move move;
	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move
	setup(&bord);
	for (int i = 0; i < 7; i++) {
		makeRandomMove(&bord, &moveList);
	}
}

int main()
{
	//king_danger_squares_test();
	//path_test();
	//checking_test();
	//time_code();
	//move_test();
	//move_test_check();
	//time_bitwise_code();
	//fen_test();
	//legalMoveTest();
	randomMoveTest();
	return 0;
}
