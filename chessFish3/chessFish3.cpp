﻿// chessFish3.cpp : Defines the entry point for the application.
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
    MOVELIST moveList;
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
    bool finished = false;
	while (i < duration & !finished) {
		cout << "move: " << i+1<< endl;
		printBoard(&bord);
        GenLegalMoveList(&moveList, &bord);
        if (moveList.count == 0) {
            cout << "white has no more legal moves" << endl;
            finished = true;
        }
		if (engineNumberA == 0) {
			askForMove(&bord, &move, &moveList);
			cout << "You selected: " << moveToString(&move) << endl;
			makeMove(&bord, &move);
		}else if (engineNumberA == 1) {
			makeRandomMove(&bord, &moveList);
		}else if (engineNumberA == 2) {
			makeMiniMaxMove(&bord, &moveList, 2, false);
		}
        printBoard(&bord);
        GenLegalMoveList(&moveList, &bord);
        if (moveList.count == 0) {
            cout << "black has no more legal moves" << endl;
            finished = true;
        }
        if (!finished) {
            if (engineNumberB == 0) {
                askForMove(&bord, &move, &moveList);
                cout << "You selected: " << moveToString(&move) << endl;
                makeMove(&bord, &move);
            }
			else if (engineNumberB == 1) {
				makeRandomMove(&bord, &moveList);
			}else if (engineNumberB == 2) {
				makeMiniMaxMove(&bord, &moveList, 2, false);
			}
            i++;
        }
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
	
	//time_minimax_code();
	runGame();
	return 0;
}
