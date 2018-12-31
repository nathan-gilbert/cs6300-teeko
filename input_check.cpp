/*
 * Example of parsing gamestate file for teeko and printing game configurations
 * in both an easy to read format and a format corresponding to the input
 * file.  This code can be integrated into C and C++ programs if desired.
 * Note that this is only one way of implementing game states and other
 * representations may be more efficient or otherwise desirable.
 */

/*
 * Nathan Gilbert
 * Loosely based on teeko-gamestate.c supplied with the assignment description.
 */
#include "input_check.h"

/*
 * This function handles reading in the input file and ensuring that
 * it has the proper syntax.
 */
bool readInit(string inFile) {
  int row, col;              /* board indecies */
  int piece, board_position; /* temporary values */
  string line;

  ifstream input;

  /* open gamestate file */
  input.open(inFile.c_str(), ios::in);

  if (!input) {
    cerr << "File open error: " << inFile << endl;
    exit(EXIT_FAILURE);
  }

  // Initialize the board.
  initBoard();

  /* read first line as string */
  if (getline(input, line) == NULL) {
    cerr << "Input file format error 1: " << inFile << endl;
    exit(EXIT_FAILURE);
  }

  // Removing trailing characters.
  int trailing = line.find_last_not_of(' ');

  if (trailing != -1)
    line.resize(trailing + 1);

  /* black or red to move? */
  if (line == "red") {
    black_to_move = false;
  } else if (line == "black") {
    black_to_move = true;
  } else {
    cerr << "Input file format error 2: " << inFile << endl;
    exit(EXIT_FAILURE);
  }

  /*
   * second line is for black player
   * start by checking for valid counts
   */
  input >> line;

  /* strip : */
  if (line[line.size() - 1] == ':') {
    line[line.size() - 1] = '\0';
  } else {
    cerr << "Input file format error 4: " << inFile << endl;
    exit(EXIT_FAILURE);
  }

  piece_count_black = atoi(line.c_str());

  if ((piece_count_black < 0) || (piece_count_black > N_PIECES)) {
    cerr << "Input file format error 3: " << inFile << endl;
    exit(EXIT_FAILURE);
  }

  /* now get the actual pieces */
  for (piece = 0; piece < piece_count_black; piece++) {
    input >> line;

    board_position = atoi(line.c_str());

    // Update the bookkeeping array of black's positions.
    blackpos[piece] = board_position;

    if ((board_position < 1) || (board_position > (BOARD_SIZE * BOARD_SIZE))) {
      fprintf(stderr, "Invalid board position for black!\n");
      exit(EXIT_FAILURE);
    }

    /* convert to row/col indecies (numbers start at 1) */
    row = (board_position - 1) % BOARD_SIZE;
    col = (board_position - 1) - (row * BOARD_SIZE);

    /* make sure this is an allowed move */
    if (teeko_board[row][col] != '-') {
      fprintf(stderr, "Invalid board position for black (occupied)!\n");
      exit(EXIT_FAILURE);
    }

    teeko_board[row][col] = 'B';
  }

  /*
   * third line is for black player
   * start by checking for valid counts
   */
  input >> line;
  piece_count_red = atoi(line.c_str());

  if ((piece_count_red < 0) || (piece_count_red > N_PIECES) ||
      (black_to_move && (piece_count_red != piece_count_black)) ||
      (!black_to_move && (piece_count_red < N_PIECES) &&
       (piece_count_red != (piece_count_black - 1)))) {
    cerr << "Invalid number of pieces for red!" << endl;
    exit(EXIT_FAILURE);
  }

  /* now get the actual pieces */
  for (piece = 0; piece < piece_count_red; piece++) {
    input >> line;

    board_position = atoi(line.c_str());

    redpos[piece] = board_position;

    if ((board_position < 1) || (board_position > (BOARD_SIZE * BOARD_SIZE))) {
      cerr << "Invalid board position for red!" << endl;
      exit(EXIT_FAILURE);
    }

    /* convert to row/col indecies (numbers start at 1) */
    row = (board_position - 1) % BOARD_SIZE;
    col = (board_position - 1) - (row * BOARD_SIZE);

    /* make sure this is an allowed move */
    if (teeko_board[row][col] != '-') {
      cerr << "Invalid board position for red (occupied)!" << endl;
      exit(EXIT_FAILURE);
    }

    teeko_board[row][col] = 'R';
  }

  input.close();
  return true;
}

/*
 * The following two functions, getBlackMove and getRedMove() are used
 * to determine, 1. is the computer, random, or user moving. 2. Get the
 * move by calling the appropiate subfunction.
 */
void getBlackMove(void) {
  if (nocomp || (!machine_plays_black && !noplayer)) {
    getMove(true, !(piece_count_black < 4));
  } else if (noplayer && !machine_plays_black) {
    getRandom(true, !(piece_count_black < 4));
  } else {
    getCompMove(!(piece_count_black < 4));
  }
}

void getRedMove(void) {
  if (nocomp || (machine_plays_black && !noplayer)) {
    getMove(false, !(piece_count_red < 4));
  } else if (noplayer && machine_plays_black) {
    getRandom(false, !(piece_count_black < 4));
  } else {
    getCompMove(!(piece_count_red < 4));
  }
}

/*
 * The following functions determine if the user or comps move are
 * correct. The boolean player is a value to determine which player the user is
 * playing as.
 * player = true -> Black Player
 * player = false -> Red Player
 */
void getMove(bool player, bool allPlayed) {
  int from, to, row, col;
  bool placed = false;

  // Are there more pieces to play?
  if (!allPlayed) {
    // Has the current piece been placed?
    while (!placed) {
      cout << (player ? "Black" : "Red") << ", where will you place piece "
           << (player ? piece_count_black + 1 : piece_count_red + 1) << ": ";

      cin >> to;

      // Exit the game.
      if (to == -1)
        exit(EXIT_SUCCESS);

      if (to > 26 || to < 1) {
        cout << "Incorrect choice: " << to << endl;
        continue;
      }

      /* convert to row/col indecies (numbers start at 1) */
      row = (to - 1) % BOARD_SIZE;
      col = (to - 1) - (row * BOARD_SIZE);

      if (teeko_board[row][col] == '-') {
        if (player) {
          blackpos[piece_count_black] = to;
          piece_count_black++;
        } else {
          redpos[piece_count_red] = to;
          piece_count_red++;
        }

        // Place the piece on the master game board.
        teeko_board[row][col] = player ? 'B' : 'R';
        cout << "Placed piece at: " << to << endl;
        placed = true;
      } else {
        cout << "Cannot place piece there. " << endl;
        placed = false;
      }
    }
  } else {
    int from_row, from_col;

    while (!placed) {
      cout << (player ? "Black" : "Red")
           << ", please enter move [ex: from to]: ";
      cin >> from >> to;
      cout << from << " " << to << endl;

      // Exit the game.
      if (to == -1 || from == -1)
        exit(EXIT_SUCCESS);

      // Get input from user and checking for malformed values.
      if (from < 1 || from > 25 || to > 25 || to < 1) {
        cout << "Incorrect options: from =" << from << " to =" << to << endl;
        placed = false;
        from = 0;
        to = 0;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        continue;
      }

      // Is this input valid?
      if (!acceptable(from, to))
        continue;

      /* convert to row/col indecies (numbers start at 1) */
      from_row = (from - 1) % BOARD_SIZE;
      from_col = (from - 1) - (from_row * BOARD_SIZE);

      /* Move old piece. */
      if (teeko_board[from_row][from_col] == (player ? 'B' : 'R')) {
        teeko_board[from_row][from_col] = '-';
        placed = true;
      } else {
        cout << "Cannot remove piece from there. " << endl;
        placed = false;
        continue;
      }

      /* convert to row/col indecies (numbers start at 1) */
      row = (to - 1) % BOARD_SIZE;
      col = (to - 1) - (row * BOARD_SIZE);

      /* Place new piece. */
      if (teeko_board[row][col] == '-') {
        teeko_board[row][col] = player ? 'B' : 'R';

        // Updating black or red's position.
        for (int i = 0; i < 4; i++)
          if ((player ? blackpos[i] : redpos[i]) == from)
            (player ? blackpos[i] : redpos[i]) = to;

        placed = true;
      } else {
        cout << "Cannot place piece there. " << endl;

        // Reset old piece.
        teeko_board[from_row][from_col] = ((player) ? 'B' : 'R');
        placed = false;
      }
    }
  }
}

/*
 * Where the magic happens. This function describes the method in which
 * the computer move is generated using the minimax algorithm. Many of the
 * functions called by this function can be found in pcu_player.* or util.*.
 */
void getCompMove(bool allPlayed) {
  Node root;
  machine_plays_black ? root.setPlayer(true) : root.setPlayer(false);
  vector<int> move(2);
  int row, col, from, to;

  // Get current state.
  copyState(root, teeko_board);

  for (int i = 0; i < 4; i++) {
    root.blackpos[i] = blackpos[i];
    root.redpos[i] = redpos[i];
  }

  // AllPlayed == true when both red and black have place 4
  // pegs.
  if (!allPlayed) {
    // Which version of minimax are we running?
    if (ab)
      alpha_beta(root, move, false);
    else
      minimax(root, move, false);

    to = move[0];

    /* convert to row/col indecies (numbers start at 1) */
    row = (to - 1) % BOARD_SIZE;
    col = (to - 1) - (row * BOARD_SIZE);

    if (teeko_board[row][col] == '-') {
      if (machine_plays_black) {
        blackpos[piece_count_black] = to;
        piece_count_black++;
      } else {
        redpos[piece_count_red] = to;
        piece_count_red++;
      }
    } else {
      cerr << "PROBLEM! #12" << endl;
      exit(EXIT_FAILURE);
    }

    // Set piece.
    teeko_board[row][col] = machine_plays_black ? 'B' : 'R';
  } else {
    if (ab)
      alpha_beta(root, move, true);
    else
      minimax(root, move, true);

    from = move[0];
    to = move[1];

    /* Already in a winning state. Don't do anything. */
    if (to == -1 && from == -1)
      return;

    /* convert to row/col indicies (numbers start at 1) */
    row = (to - 1) % BOARD_SIZE;
    col = (to - 1) - (row * BOARD_SIZE);

    for (int i = 0; i < 4; i++)
      if ((machine_plays_black ? blackpos[i] : redpos[i]) == from)
        (machine_plays_black ? blackpos[i] : redpos[i]) = to;

    // Set piece.
    teeko_board[row][col] = machine_plays_black ? 'B' : 'R';

    /* convert to row/col indicies (numbers start at 1) */
    row = (from - 1) % BOARD_SIZE;
    col = (from - 1) - (row * BOARD_SIZE);

    // Erase piece.
    teeko_board[row][col] = '-';
  }
}

/*
 * This function is given a move in the form of ``from'', and ``to''. It
 * deterines if this is valid move. Returns true if it is, false otherwise.
 * This is basically just some math to determine if the move is to an adjacent
 * square.
 */
bool acceptable(int from, int to) {

  if (abs(from - to) != 5 && abs(from - to) != 4 && abs(from - to) != 1 &&
      abs(from - to) != 6) {
    if (!noplayer)
      cout << "Incorrect options: from=" << from << " to=" << to << endl;

    return false;
  }

  // For jumps left to right
  if ((from - to == -1) && (from % 5 == 0)) {
    if (!noplayer)
      cout << "Incorrect options: from=" << from << " to=" << to << endl;

    return false;
  }

  if ((from - to == 1) && (from % 5 == 1)) {
    if (!noplayer)
      cout << "Incorrect options: from=" << from << " to=" << to << endl;

    return false;
  }

  // For left diagonals
  if (from - to == 6 && from % 5 == 1) {
    if (!noplayer)
      cout << "Incorrect options: from=" << from << " to=" << to << endl;

    return false;
  }

  if (from - to == -6 && from % 5 == 0) {
    if (!noplayer)
      cout << "Incorrect options: from=" << from << " to=" << to << endl;

    return false;
  }

  // For right diagonals
  if (from - to == 4 && (from % 5) == 0) {
    if (!noplayer)
      cout << "Incorrect options: from=" << from << " to=" << to << endl;

    return false;
  }

  if (from - to == -4 && (from % 5) == 1) {
    if (!noplayer)
      cout << "Incorrect options: from=" << from << " to=" << to << endl;

    return false;
  }

  return true;
}

/*
 * This functions gets the moves for the random player.
 *
 */
void getRandom(bool player, bool allPlayed) {
  bool placed = false;
  int to, from, row, col;

  // Seed the random numbers.
  srand((unsigned)time(0));

  if (!allPlayed) {
    while (!placed) {
      // Generate random move.
      to = 1 + rand() % 25;

      /* convert to row/col indecies (numbers start at 1) */
      row = (to - 1) % BOARD_SIZE;
      col = (to - 1) - (row * BOARD_SIZE);

      // check to see if move is legal.
      if (teeko_board[row][col] == '-') {
        if (player) {
          blackpos[piece_count_black] = to;
          piece_count_black++;
        } else {
          redpos[piece_count_red] = to;
          piece_count_red++;
        }

        teeko_board[row][col] = player ? 'B' : 'R';
        placed = true;
      } else {
        placed = false;
      }
    }
  } else {
    int from_row, from_col;
    int from_index;

    // Generate random piece to pick up.
    from_index = rand() % 4;

    from = (player ? blackpos[from_index] : redpos[from_index]);

    /* convert to row/col indecies (numbers start at 1) */
    from_row = (from - 1) % BOARD_SIZE;
    from_col = (from - 1) - (from_row * BOARD_SIZE);

    while (!placed) {
      // Where to place this piece now...
      to = 1 + rand() % 25;

      // Is this input valid?
      if (!acceptable(from, to))
        continue;

      /* convert to row/col indecies (numbers start at 1) */
      row = (to - 1) % BOARD_SIZE;
      col = (to - 1) - (row * BOARD_SIZE);

      /* Place new piece. */
      if (teeko_board[row][col] == '-') {
        // Remove old piece.
        teeko_board[from_row][from_col] = '-';

        // Place new piece.
        teeko_board[row][col] = player ? 'B' : 'R';

        // Updating black or red's position.
        ((player) ? blackpos[from_index] : redpos[from_index]) = to;

        // Our work here is done...
        placed = true;
      } else {
        placed = false;
      }
    }
  }
}
