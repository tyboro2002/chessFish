// engine.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include "game.h"

// TODO: Reference additional headers your program requires here.

void askForMove(Board* bord, Move* move);
void printEngines();

void makeRandomMove(Board* bord, MOVELIST* moveList);
