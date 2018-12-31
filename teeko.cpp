/*
 * TeekoMaster 5000
 */
#include "input_check.h"
#include "util.h"
#include <iostream>
#include <popt.h>
#include <stdlib.h>

using namespace std;

/* Some useful globals. */
int machine_plays_black = false;

/* Alpha-beta pruning enabled. */
int ab = false;

/* Basic trace enabled. */
int trace = false;

/* Full trace enabled */
int fulltrace = false;

/* No human players */
int noplayer = false;

/* No computer players. */
int nocomp = false;

/* Display statistics at end. */
int stats = false;

/* Plausible move ordering. */
int plaus = false;

/* Depth of search. */
int dvalue = 4;

/* Use alternate heuristic. */
int alth = false;

/* Use forward pruning. */
bool forward_prune = false;

/* Forward pruning value. */
int fvalue = DEFAULT_VALUE;

/* Total pieces played red.*/
int piece_count_red;

/* Total pieces played black.*/
int piece_count_black;

/* Total moves (nodes) generated during search. */
unsigned int moves_generated = 0;

/* The Master Game board.  */
char teeko_board[BOARD_SIZE][BOARD_SIZE];

/* Is it black's turn to move? */
bool black_to_move;

/* Number of times minimax was called. */
int minimax_called = 0;

/* Number of alpha cutoffs. */
int alpha_cutoff = 0;

/* Number of beta cutoffs. */
int beta_cutoff = 0;

/* Terminated by win. */
int gameover = 0;

/* Search terminated by depth. */
int cutoff = 0;

/* Some bookkeeping to know the teeko location of red and black. */
int redpos[] = {0, 0, 0, 0};
int blackpos[] = {0, 0, 0, 0};

char *Usage = "[OPTIONS...] [game-state]";

struct poptOption OptionsTable[] = {
    {"black", 'b', POPT_ARG_NONE | POPT_ARGFLAG_ONEDASH, &machine_plays_black,
     0, "Computer plays as black. Default is user plays as black.", NULL},
    {"ab", 'a', POPT_ARG_NONE | POPT_ARGFLAG_ONEDASH, &ab, 0,
     "Use alpha-beta pruning.", NULL},
    {"nocomp", 'n', POPT_ARG_NONE | POPT_ARGFLAG_ONEDASH, &nocomp, 0,
     "No CPU player. All moves are player controlled.", NULL},
    {"plausible", 'p', POPT_ARG_NONE | POPT_ARGFLAG_ONEDASH, &plaus, 0,
     "Use plausible move ordering.", NULL},
    {"trace", 't', POPT_ARG_NONE | POPT_ARGFLAG_ONEDASH, &trace, 0,
     "Print the game state passed to each invocation of minimax, along with "
     "the depth of the search at that point. If alpha-beta pruning is enabled, "
     "also prints the values of alpha and beta passed to the minimax routine.",
     NULL},
    {"fulltrace", 'u', POPT_ARG_NONE | POPT_ARGFLAG_ONEDASH, &fulltrace, 0,
     "All relevant information printed.", NULL},
    {"alt", 'h', POPT_ARG_NONE | POPT_ARGFLAG_ONEDASH, &alth, 0,
     "Use alternate evaluation function.", NULL},
    {"auto", 'o', POPT_ARG_NONE | POPT_ARGFLAG_ONEDASH, &noplayer, 0,
     "Play a complete game, the user moves are chosen randomly from the set of "
     "legal moves and the program playing as normal.",
     NULL},
    {"stats", 's', POPT_ARG_NONE | POPT_ARGFLAG_ONEDASH, &stats, 0,
     "Prints out total number of moves generated and the number of times that "
     "search was terminated due either to a position constituting a win for a "
     "black or red or the depth limit being executed.",
     NULL},
    {"depth", 'd', POPT_ARG_INT | POPT_ARGFLAG_ONEDASH, &dvalue, 0,
     "The depth limit for search, specified in plies.", NULL},
    {"forward", 'f', POPT_ARG_INT | POPT_ARGFLAG_ONEDASH, &fvalue, 0,
     "Use forward pruning with a maximum of <limit> alternatives. Implies the "
     "-p option. Default is no forward pruning.",
     NULL},
    POPT_AUTOHELP{NULL, 0, 0, NULL, 0, NULL, NULL}};

/* The main loop of the game.  */
int playGame(void);

int main(int argc, char *argv[]) {
  poptContext PoptContext;
  int PoptReturnCode;
  const char **args;
  int argpt = 0;
  int arg;
  string inFile;
  black_to_move = true;

  PoptContext = poptGetContext(NULL, argc, (const char **)argv, OptionsTable,
                               POPT_CONTEXT_POSIXMEHARDER);
  poptSetOtherOptionHelp(PoptContext, Usage);
  PoptReturnCode = poptGetNextOpt(PoptContext);

  if (PoptReturnCode != -1) {
    cerr << poptBadOption(PoptContext, POPT_BADOPTION_NOALIAS) << ": "
         << poptStrerror(PoptReturnCode) << endl;
    return (EXIT_FAILURE);
  }

  args = poptGetArgs(PoptContext);
  arg = 0;

  while ((args != NULL) && (args[arg] != NULL)) {
    arg++;
  }

  if (nocomp && noplayer) {
    cerr << "-nocomp and -auto cannot be used at the same time. " << endl;
    return (EXIT_FAILURE);
  }

  if (fvalue != DEFAULT_VALUE)
    forward_prune = true;

  if (arg > 0) {
    inFile = args[argpt++];

    if (!readInit(inFile)) {
      cerr << "Input read error. " << endl;
      return (EXIT_FAILURE);
    }

    // Print all options chosen.
    preamble();
    printBoard(false);

    if (!playGame())
      cout << "\t\t\tBlack wins!" << endl;
    else
      cout << "\t\t\tRed wins!" << endl;

    printBoard(true);

    if (stats)
      printStats();
  } else {
    preamble();
    initBoard();
    printBoard(false);

    if (!playGame())
      cout << "\t\t\tBlack wins!" << endl;
    else
      cout << "\t\t\tRed wins!" << endl;

    printBoard(true);

    if (stats)
      printStats();
  }

  return (EXIT_SUCCESS); /* normal exit */
}

void printStats() {
  float per;

  if (alpha_cutoff == 0)
    per = 0.0;
  else if (beta_cutoff == 0)
    per = 1.0;
  else
    per = (float)(alpha_cutoff / beta_cutoff);

  // per = (float)(alpha_cutoff/beta_cutoff);
  cout << "STATS: " << endl;
  cout << "Minimax called: " << minimax_called << " times. " << endl;
  cout << moves_generated << " moves generated. " << endl;
  cout << "Search Termination: game-over-> " << gameover << ", depth limit-> "
       << cutoff << endl;

  if (ab)
    cout << (alpha_cutoff + beta_cutoff) << " alpha/beta cutoffs ("
         << alpha_cutoff << " alpha, " << beta_cutoff << " beta): " << per * 100
         << "%" << endl;
}

int playGame(void) {
  while (1) {
    if (black_to_move) {
      // Black move
      getBlackMove();
      black_to_move = !black_to_move;

      // If Black wins return 0. If Red wins
      // return 1.
      if (checkWin(true))
        return 0;
    } else {
      printBoard(false);

      // Red move
      getRedMove();
      black_to_move = !black_to_move;

      // Checking for Red player win.
      if (checkWin(false))
        return 1;

      printBoard(false);
    }
  }

  return -1;
}
