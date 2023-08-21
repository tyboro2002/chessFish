// chessFish3.cpp : Defines the entry point for the application.
//

#include "chessFish3.h"
#include "game.h"
#include "engine.h"
#include "tests.h"
#include "time.h"
#include <limits>

using namespace std;

void runGame() {
	Board bord;
	Move move;
    MOVELIST moveList;
	TranspositionTable transpositionTable;
	PositionTracker positionTracker;
	//setupEmpty(&bord);
	//std::string fen = "4kb1r/p2ppppp/8/8/8/8/P1PPPPPP/RQ2KB1R w - - 0 1";
	//readInFen(&bord, &fen);
	setup(&bord);
	printBoard(&bord);
	int duration;
	cout << "how many moves do you want to play max?" << endl;
	cin >> duration;
	if (duration == -1) {
		duration = std::numeric_limits<int>::max();
	}
	cout << "wich engine do you want to player A to be?" << endl;
	printEngines();
	int engineNumberA;
	cin >> engineNumberA;
	cout << "wich engine do you want to player B to be?" << endl;
	printEngines();
	int engineNumberB;
	cin >> engineNumberB;
	int i = 0;
	int depth;
	if (engineNumberA == 2 || engineNumberB == 2 || engineNumberA == 3 || engineNumberB == 3) {
		cout << "what depth do you want" << endl;
		cin >> depth;
	}
	else {
		depth = 0;
	}
    bool finished = false;
	while (i < duration && !finished && !isDraw(&bord, &positionTracker)) {
		cout << "move: " << i+1<< endl;
		printBoard(&bord);
        GenLegalMoveList(&moveList, &bord, &positionTracker);
        if (moveList.count == 0) {
            cout << "white has no more legal moves" << endl;
            finished = true;
        }
		if (engineNumberA == 0) {
			askForMove(&bord, &move, &moveList);
			cout << "You selected: " << moveToString(&move) << endl;
			makeMove(&bord, &move, &positionTracker);
		}else if (engineNumberA == 1) {
			makeRandomMove(&bord, &moveList, &positionTracker);
		}else if (engineNumberA == 2) {
			makeMiniMaxMove(&bord, &moveList, depth, true, &transpositionTable, &positionTracker);
		}else if (engineNumberA == 3) {
			makeMiniMaxOptimizedMove(&bord, &moveList, depth, true, &transpositionTable, &positionTracker);
		}
        printBoard(&bord);
        GenLegalMoveList(&moveList, &bord, &positionTracker);
        if (moveList.count == 0) {
            cout << "black has no more legal moves" << endl;
            finished = true;
        }
        if (!finished && !isDraw(&bord, &positionTracker)) {
            if (engineNumberB == 0) {
                askForMove(&bord, &move, &moveList);
                cout << "You selected: " << moveToString(&move) << endl;
                makeMove(&bord, &move, &positionTracker);
            }
			else if (engineNumberB == 1) {
				makeRandomMove(&bord, &moveList, &positionTracker);
			}else if (engineNumberB == 2) {
				makeMiniMaxMove(&bord, &moveList, depth, true, &transpositionTable, &positionTracker);
			}else if (engineNumberB == 3) {
				makeMiniMaxOptimizedMove(&bord, &moveList, depth, true, &transpositionTable, &positionTracker);
			}
            i++;
        }
	}
	if (isDraw(&bord, &positionTracker)) {
		cout << "a draw occured" << endl;
		printPositionRecords(&positionTracker);
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
	//move_test_halfmove();
	
	//time_minimax_code();
	//time_mate_test_code();
	//runAutomatedTests();
	runGame();


	//kingMovesGenerator();
	//knightMovesGenerator();

	//randomTest();
	return 0;
}
