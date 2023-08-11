#include "tests.h"
#include "game.h"
#include "engine.h"

using namespace std;

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
	printBitBoard(bitmap_black_king(E7, &bord), "black king moves");
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
	printBitBoard(black_checking_bitmap(&bord), "black checking");

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
