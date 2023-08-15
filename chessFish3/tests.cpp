#include "tests.h"
#include "game.h"
#include "engine.h"
#include <bitset>

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

	/*
	setupEmpty(&bord);
	std::string fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
	readInFen(&bord, &fen);

	//GenLegalMoveList(&moveList, &bord);
	//cout << (moveList.count == 22) << endl;

	setupEmpty(&bord);
	fen = "rnb1kbnr/pp1p1ppp/8/2p1Q3/2q1P3/8/PPPP1PPP/RNB1KB1R b KQkq - 1 2";
	readInFen(&bord, &fen);

	GenLegalMoveList(&moveList, &bord);

	*/
	setupEmpty(&bord);
	std::string fen = "r1bq2r1/b4pk1/p1pp1p2/1p2pPQ1/1P2P1PB/3P4/1PP3P1/R3K2R b - - 0 1";
	readInFen(&bord, &fen);
	GenLegalMoveList(&moveList, &bord);
	cout << endl;
}

/*
* further tests are automatic tests
*/
bool mateInOneTest() {
	Board bord;
	Move move;
	Move moveOut;
	setupEmpty(&bord);
	std::string fen = "4kb1r/p2ppppp/8/8/8/8/P1PPPPPP/RQ2KB1R w - - 0 1";
	readInFen(&bord, &fen);

	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move
	GenLegalMoveList(&moveList, &bord);

	minimax_root(&bord, 1, true, &moveOut, &moveList);
	move.src = B1;
	move.dst = B8;
	move.capture = -52;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &moveOut);
	//printBoard(&bord);
	return move == moveOut;
}

bool mateInTwoTest() {
	Board bord;
	Move move;
	Move moveOut;
	setupEmpty(&bord);
	std::string fen = "r1bq2r1/b4pk1/p1pp1p2/1p2pP2/1P2P1PB/3P4/1PP3P1/R1Q1K2R w - - 0 1";
	readInFen(&bord, &fen);
	//printBoard(&bord);
	int depth = 3; // set the depth for the minimax
	MOVELIST moveList;
	// Clear move list

	//white move
	moveList.count = 0;   // set each field for each move
	GenLegalMoveList(&moveList, &bord);
	minimax_root(&bord, depth, true, &moveOut, &moveList);
	move.src = C1;
	move.dst = H6;
	move.capture = -52;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &moveOut);
	//printBoard(&bord);
	if (!(move == moveOut)) {
		return false;
	}

	//black move
	moveList.count = 0;   // set each field for each move
	GenLegalMoveList(&moveList, &bord);
	minimax_root(&bord, depth, true, &moveOut, &moveList);
	move.src = G7;
	move.dst = H6;
	move.capture = H6;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &moveOut);
	//printBoard(&bord);
	if (!(move == moveOut)) {
		return false;
	}


	//white move
	moveList.count = 0;   // set each field for each move
	GenLegalMoveList(&moveList, &bord);
	minimax_root(&bord, depth, true, &moveOut, &moveList);
	move.src = H4;
	move.dst = F6;
	move.capture = F6;
	move.special = NOT_SPECIAL;
	makeMove(&bord, &moveOut);
	//printBoard(&bord);
	if (!(move == moveOut)) {
		return false;
	}
	return move == moveOut;
}

void runAutomatedTests() {

	cout << "mate in one test" << endl;
	if (mateInOneTest()) {
		cout << "SUCEED" << endl;
	}
	else {
		cout << "FAILED" << endl;
	}
	
	cout << "mate in two test" << endl;
	if (mateInTwoTest()) {
		cout << "SUCEED" << endl;
	}
	else {
		cout << "FAILED" << endl;
	}
}

void runAutomatedTestsSilent() {
	if (!mateInOneTest()) cout << "mate in one test FAILED" << endl;
	if (!mateInTwoTest()) cout << "mate in two test FAILED" << endl;
}

void kingMovesTest() {
	Board bord;
	for (int i = 0; i < 64; i++) {
		setupEmpty(&bord);
		addPiece(&bord, WKING, (Square)i);
		printBoard(&bord);
		printBitBoard(bitmap_white_king(i, &bord),"");
		cout << std::bitset<64>(bitmap_white_king(i,&bord)) << endl;
	}
}