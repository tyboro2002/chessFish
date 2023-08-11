// chessFish3.cpp : Defines the entry point for the application.
//

#include "chessFish3.h"
#include "game.h"
#include "engine.h"
#include "tests.h"
#include "time.h"

using namespace std;

void runGame() {
	Board bord;
	Move move;
	setup(&bord);
	int duration;
	cout << "how many moves do you want to play max?" << endl;
	cin >> duration;
	cout << "wich engine do you want to player A to be?" << endl;
	printEngines();
	int engineNumberA;
	cin >> engineNumberA;
	cout << "wich engine do you want to player B to be?" << endl;
	printEngines();
	int engineNumberB;
	cin >> engineNumberB;
	int i = 0;
	while (i < duration) {
		cout << "move: " << i+1<< endl;
		printBoard(&bord);
		if (engineNumberA == 0) {
			askForMove(&bord, &move);
			cout << "You selected: " << moveToString(&move) << endl;
			makeMove(&bord, &move);
		}else if (engineNumberA == 1) {
			MOVELIST moveList;
			// Clear move list
			moveList.count = 0;   // set each field for each move
			GenLegalMoveList(&moveList, &bord);
			makeRandomMove(&bord, &moveList);
		}
		printBoard(&bord);
		if (engineNumberB == 0) {
			askForMove(&bord, &move);
			cout << "You selected: " << moveToString(&move) << endl;
			makeMove(&bord, &move);
		}
		else if (engineNumberB == 1) {
			MOVELIST moveList;
			// Clear move list
			moveList.count = 0;   // set each field for each move
			GenLegalMoveList(&moveList, &bord);
			makeRandomMove(&bord, &moveList);
		}
		i++;
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
	//randomMoveTest();
	runGame();
	return 0;
}
