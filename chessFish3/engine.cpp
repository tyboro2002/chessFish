
#include "engine.h";
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

/*
* functions for the actual engine
*/
void askForMove(Board* bord, Move* move) {
	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move
	GenLegalMoveList(&moveList, bord);
	for (int i = 0; i < moveList.count; i++) {
		cout << i << ") " << moveToString(&moveList.moves[i]) << endl;
	}
	cout << "Enter the number of the move you want to play" << endl;
	int moveNumber;
	std::cin >> moveNumber;
	move->src = moveList.moves[moveNumber].src;
	move->dst = moveList.moves[moveNumber].dst;
	move->special = moveList.moves[moveNumber].special;
	move->capture = moveList.moves[moveNumber].capture;
}

void printEngines() {
	cout << "0) the human player" << endl;
	cout << "1) the random engine" << endl;
}

void makeRandomMove(Board* bord, MOVELIST* moveList) {
	GenLegalMoveList(moveList, bord);
	if (moveList->count == 0) {
		cout << "there are no legal moves";
		return;
	}
	/*
	for (int i = 0; i < moveList->count; i++) {
		cout << i << ") " << moveToString(&moveList->moves[i]) << endl;
	}
	*/
	int choosen = generateRandomNumber(moveList->count);
	cout << "Randomly selected: " << moveToString(&moveList->moves[choosen]) << endl;
	makeMove(bord, &moveList->moves[choosen]);
	//printBoard(bord);
}