#include "time.h"
#include "tests.h"
#include "game.h"
#include "engine.h"
#include <chrono>

using namespace std;

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
		all_white_attacks(&bord, 1) | all_black_attacks(&bord, 1);
	}

	// Get the ending timestamp
	auto endTime = std::chrono::high_resolution_clock::now();

	// Calculate the duration in microseconds (change to other duration units as needed)
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

	std::cout << "Time taken: " << duration << " microseconds" << std::endl;
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


void time_minimax_code() {
	Board bord;
	Move move;
	MOVELIST moveList;
	// Clear move list
	moveList.count = 0;   // set each field for each move
	setup(&bord);

	//timing code
	// Get the starting timestamp
	auto startTime = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 1000000; ++i){
		minimax_root(&bord, 10, false, &move, &moveList);
	}

	// Get the ending timestamp
	auto endTime = std::chrono::high_resolution_clock::now();

	// Calculate the duration in microseconds (change to other duration units as needed)
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

	std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

void time_mate_test_code() {
	//timing code
	// Get the starting timestamp
	auto startTime = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 1; ++i) {
		runAutomatedTestsSilent();
	}

	// Get the ending timestamp
	auto endTime = std::chrono::high_resolution_clock::now();

	// Calculate the duration in microseconds (change to other duration units as needed)
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

	std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}