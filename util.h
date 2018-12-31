#ifndef _UTIL_H
#define _UTIL_H

#include "node.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

#define BOARD_SIZE 5
#define N_PIECES 4
#define DEFAULT_VALUE -1
const int N_POS_WINS = 44;

/* In the final version, simply move all globals here. */
extern char teeko_board[BOARD_SIZE][BOARD_SIZE];
extern int machine_plays_black;
extern bool black_to_move;
extern int piece_count_red;
extern int piece_count_black;
extern int blackpos[4];
extern int redpos[4];
extern int nocomp;
extern int ab;
extern int plaus;
extern int alth;
extern int trace;
extern int stats;
extern int forward;
extern int fvalue;
extern int dvalue;
extern int noplayer;
extern int minimax_called;
extern int alpha_cutoff;
extern int beta_cutoff;
extern int gameover;
extern int cutoff;
extern int fulltrace;
extern unsigned int moves_generated;

bool checkWin(bool);
bool checkWin2(int[]);
void initBoard(void);
void preamble(void);
void printStats(void);
void printBoard(bool);
void printCompact(char[][BOARD_SIZE]);
void tokenize(string, vector<string> &);
bool terminalCheck(Node);
void getWin(vector<int> &, string);
void copyState(Node &, char[][BOARD_SIZE]);

/* Generate children for full or non-full boards. */
void generateChildren(Node, vector<Node> &);
void generateChildren2(Node, vector<Node> &);

void minimax(Node, vector<int> &, bool);
void alpha_beta(Node, vector<int> &, bool);

/* Max and min for standard minimax. */
int maxvalue(Node, int);
int minvalue(Node, int);

/* Max and Min functions for alpha-beta search. */
int maxvalue_ab(Node, int, int, int);
int minvalue_ab(Node, int, int, int);

/* Hueristic functions. */
int hueristic(Node);
int hueristic2(Node);

/* Plausible */
void plausible(vector<Node> &);

#endif
