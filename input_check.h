/*
 * Nathan Gilbert
 *
 * These are the function prototypes for the functions in input_check.cpp.
 */
#ifndef INPUT_CHECK_H
#define INPUT_CHECK_H

#include "util.h"

bool readInit(string);
void printBoard(void);
bool playerMove(bool, int, int);
bool compMove(bool, int, int);
void getBlackMove(void);
void getRedMove(void);
void getMove(bool, bool);
void getRandom(bool, bool);
void getCompMove(bool);
bool acceptable(int, int);

#endif
