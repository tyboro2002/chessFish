// engine.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>

void king_danger_squares_test();
void path_test();
void checking_test();
void move_test();
void move_test_check();
void randomMoveTest();
void fen_test();
void legalMoveTest();
void move_test_halfmove();

/*
* automatic tests
*/
void runAutomatedTests();
void runAutomatedTestsSilent();
bool inCheckTest();
bool mateInOneTest();
bool mateInTwoTest();
bool mateInThreeTest();

void kingMovesGenerator();
void knightMovesGenerator();

void randomTest();