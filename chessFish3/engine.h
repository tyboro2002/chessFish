// engine.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include "game.h"

// TODO: Reference additional headers your program requires here.

void askForMove(Board* bord, Move* move, MOVELIST* moveList);
void printEngines();

void makeRandomMove(Board* bord, MOVELIST* moveList, PositionTracker* positionTracker);
void makeMiniMaxMove(Board* bord, MOVELIST* moveList, int depth, bool maximize, TranspositionTable* transpositionTable, PositionTracker* positionTracker);
void makeMiniMaxOptimizedMove(Board* bord, MOVELIST* moveList, int depth, bool maximize, TranspositionTable* transpositionTable, PositionTracker* positionTracker);
/*
* only for testing
*/

void minimax_root(Board* bord, int depth, bool maximize, Move* moveOut, MOVELIST* moveList, TranspositionTable* transpositionTable, PositionTracker* positionTracker);
void minimax_rootOptimized(Board* bord, int depth, bool maximize, Move* moveOut, MOVELIST* moveList, TranspositionTable* transpositionTable, PositionTracker* positionTracker);