
#include "pcu_player.h"

/*
 * In this file, I decided to break minimax and alpha-beta minimax into two
 * separate functions for ease of readability. In a less pedantic implemenation,
 * I would have consolidated them to a single function.
 */

/*
 * Standard minimax. Here instead of a single minimax function, I implemented
 * this using indirect recursion. The function oscillates between maxvalue and
 * minvalue. In the cases that follow, the minimax and alpha-beta functions
 * first prime the recursion by implicitly doing a max-value step within
 * themselves, then caling minvalue, etc.
 */
void minimax(Node n, vector<int> &move, bool allPlayed) {
  vector<Node> children;

  /* Update the global variable for this call to minimax.*/
  minimax_called++;

  // Checking for other flags...
  if (trace || fulltrace) {
    cout << "Invoking minimax at level: 0 (maximizing)" << endl;
    cout << (n.getPlayer() ? "black" : "red") << endl;
    printCompact(n.board);
  }

  if (fulltrace) {
    printStats();
    cout << "Hueristic value for this move: "
         << (alth ? hueristic2(n) : hueristic(n)) << endl;
    ;
  }

  // Have we started at a win state for some reason?
  if ((machine_plays_black ? checkWin2(n.blackpos) : checkWin2(n.redpos))) {
    move[0] = -1;
    move[1] = -1;

    return;
  }

  int newValue = 0, oldHighest = INT_MIN;

  // Have all pieces been played?
  if (allPlayed) {
    // Generate all successor nodes.
    generateChildren(n, children);

    // Count how many there were.
    moves_generated += children.size();

    // Getting the max of the min values.
    for (int i = 0; i < (int)children.size(); i++) {
      if (terminalCheck(children[i])) {
        gameover++;
        move[0] = children[i].getMove()[0];
        move[1] = children[i].getMove()[1];
        break;
      }

      // Get the value for the next child.
      newValue = minvalue(children[i], 1);

      // Update accordingly.
      if (newValue > oldHighest) {
        move[0] = children[i].getMove()[0];
        move[1] = children[i].getMove()[1];
        oldHighest = newValue;
      }
    }

    cout << "Choosing move: " << move[0] << " ==> " << move[1]
         << " value of: " << oldHighest << endl;
  } else {
    // If there are still pieces to place, then we need to generate children
    // differently.
    generateChildren2(n, children);
    moves_generated += children.size();

    // Getting the max of the min values.
    for (int i = 0; i < (int)children.size(); i++) {
      newValue = minvalue(children[i], 1);

      if (newValue > oldHighest) {
        move[0] = children[i].getMove()[0];
        move[1] = children[i].getMove()[1];
        oldHighest = newValue;
      }
    }

    cout << "Choosing move: " << move[0] << " value of: " << oldHighest << endl;
  }
}

/*
 * minimax with alpha-beta pruning.
 */
void alpha_beta(Node n, vector<int> &move, bool allPlayed) {
  minimax_called++;

  vector<Node> children;
  int newValue = 0, alpha = INT_MIN, beta = INT_MAX;

  /* The same boiler plate code as above. */
  if (trace || fulltrace) {
    cout << "Invoking minimax at level: 0 (maximizing), alpha=" << alpha
         << " beta=" << beta << endl;
    cout << (n.getPlayer() ? "black" : "red") << endl;
    printCompact(n.board);
  }

  if (fulltrace) {
    printStats();
    cout << "Hueristic value for this move: "
         << (alth ? hueristic2(n) : hueristic(n)) << endl;
    ;
  }

  if ((machine_plays_black ? checkWin2(n.blackpos) : checkWin2(n.redpos))) {
    move[0] = -1;
    move[1] = -1;

    return;
  }

  if (allPlayed) {
    generateChildren(n, children);

    moves_generated += children.size();

    // Getting the max of the min values.
    for (int i = 0; i < (int)children.size(); i++) {
      if (terminalCheck(children[i])) {
        gameover++;
        move[0] = children[i].getMove()[0];
        move[1] = children[i].getMove()[1];
        break;
      }

      newValue = minvalue_ab(children[i], 1, alpha, beta);

      if (newValue > alpha) {
        move[0] = children[i].getMove()[0];
        move[1] = children[i].getMove()[1];
        alpha = newValue;
      }
    }

    cout << "Choosing move: " << move[0] << " ==> " << move[1]
         << " value of: " << alpha << endl;
  } else {
    generateChildren2(n, children);
    moves_generated += children.size();

    // Getting the max of the min values.
    for (int i = 0; i < (int)children.size(); i++) {
      newValue = minvalue_ab(children[i], 1, alpha, beta);

      if (newValue > alpha) {
        move[0] = children[i].getMove()[0];
        move[1] = children[i].getMove()[1];
        alpha = newValue;
      }
    }

    cout << "Choosing move: " << move[0] << " value of: " << alpha << endl;
  }
}

/*
 * Maxvalue function for vannilla minimax.
 */
int maxvalue(Node n, int depth) {
  minimax_called++;

  if (trace || fulltrace) {
    cout << "\nInvoking minimax at level: " << depth << " (maximizing)" << endl;
    cout << (n.getPlayer() ? "black" : "red") << endl;
    printCompact(n.board);
  }

  if (fulltrace) {
    printStats();
    cout << "Hueristic value for this move: "
         << (alth ? hueristic2(n) : hueristic(n)) << endl;
    ;
  }

  // Are we at a goal state?
  if (terminalCheck(n)) {
    gameover++;
    return alth ? hueristic2(n) : hueristic(n);
  }

  // Have we reached the depth limit?
  if (depth == dvalue) {
    cutoff++;
    return alth ? hueristic2(n) : hueristic(n);
  }

  // If both of above tests are false, let's initialize some variables...
  vector<Node> children;
  int v = INT_MIN;
  int tmp = 0;
  int num_black = 0, num_red = 0;

  // How many places are on this move's board?
  for (int i = 0; i < 4; i++) {
    if (n.blackpos[i] != 0)
      num_black++;
  }

  for (int i = 0; i < 4; i++) {
    if (n.redpos[i] != 0)
      num_red++;
  }

  // If there are still pieces left to place, generate moves accordingly.
  if ((!n.getPlayer() ? num_black < 4 : num_red < 4)) {
    generateChildren2(n, children);
    moves_generated += children.size();
  } else {
    generateChildren(n, children);
    moves_generated += children.size();
  }

  // If plausible, order the moves accordingly.
  if (plaus || forward) {
    plausible(children);
  }

  // Only follow the top <fvalue> children if forward pruning is enabled.
  for (int i = 0; i < (forward ? fvalue : (int)children.size()); i++) {
    tmp = minvalue(children[i], depth + 1);

    if (tmp > v) {
      v = tmp;
    }
  }

  return v;
}

/*
 * Almost exactly like maxvalue, except with alpha-beta cutoffs.
 */
int maxvalue_ab(Node n, int depth, int alpha, int beta) {
  minimax_called++;

  if (trace || fulltrace) {
    cout << "\nInvoking minimax at level: " << depth
         << " (maximizing, alpha =" << alpha << " beta =" << beta << ")"
         << endl;
    cout << (n.getPlayer() ? "black" : "red") << endl;
    printCompact(n.board);
  }

  if (fulltrace) {
    printStats();
    cout << "Hueristic value for this move: "
         << (alth ? hueristic2(n) : hueristic(n)) << endl;
    ;
  }

  if (terminalCheck(n)) {
    gameover++;
    return alth ? hueristic2(n) : hueristic(n);
  }

  if (depth == dvalue) {
    cutoff++;
    return alth ? hueristic2(n) : hueristic(n);
  }

  vector<Node> children;
  int v = INT_MIN;
  int tmp = 0;
  int num_black = 0, num_red = 0;

  for (int i = 0; i < 4; i++) {
    if (n.blackpos[i] != 0)
      num_black++;
  }

  for (int i = 0; i < 4; i++) {
    if (n.redpos[i] != 0)
      num_red++;
  }

  if ((!n.getPlayer() ? num_black < 4 : num_red < 4)) {
    generateChildren2(n, children);
    moves_generated += children.size();
  } else {
    generateChildren(n, children);
    moves_generated += children.size();
  }

  // If plausible
  if (plaus || forward) {
    plausible(children);
  }

  for (int i = 0; i < (forward ? fvalue : (int)children.size()); i++) {
    tmp = minvalue_ab(children[i], depth + 1, alpha, beta);

    if (tmp > v) {
      v = tmp;
    }

    if (v >= beta) {
      beta_cutoff++;
      return v;
    }

    if (v > alpha)
      alpha = v;
  }

  return v;
}

/*
 * The minvalue function. Used by minimax and maxvalue.
 */
int minvalue(Node n, int depth) {
  minimax_called++;

  if (trace || fulltrace) {
    cout << "\nInvoking minimax at level: " << depth << " (minimizing)" << endl;
    cout << (n.getPlayer() ? "black" : "red") << endl;
    printCompact(n.board);
  }

  if (fulltrace) {
    printStats();
    cout << "Hueristic value for this move: "
         << (alth ? hueristic2(n) : hueristic(n)) << endl;
    ;
  }

  if (terminalCheck(n)) {
    gameover++;
    return alth ? hueristic2(n) : hueristic(n);
  }

  if (depth == dvalue) {
    cutoff++;
    return alth ? hueristic2(n) : hueristic(n);
  }

  vector<Node> children;

  int v = INT_MAX;
  int tmp = 0;
  int num_black = 0, num_red = 0;

  for (int i = 0; i < 4; i++) {
    if (n.blackpos[i] != 0)
      num_black++;
  }

  for (int i = 0; i < 4; i++) {
    if (n.redpos[i] != 0)
      num_red++;
  }

  if ((!n.getPlayer() ? num_black < 4 : num_red < 4)) {
    generateChildren2(n, children);
    moves_generated += children.size();
  } else {
    generateChildren(n, children);
    moves_generated += children.size();
  }

  // If plausible
  if (plaus || forward) {
    plausible(children);
  }

  if (forward) {
    vector<Node>::iterator iter;
    int i = 0;

    /*
     * Here we must start at the end of the children list, because we want
     * to minimize.
     */
    for (iter = children.end(); iter != children.begin(); iter--) {
      tmp = maxvalue(children[i], depth + 1);

      if (tmp < v) {
        v = tmp;
      }

      i++;
      if (i == fvalue)
        break;
    }
  } else {
    for (int i = 0; i < (int)children.size(); i++) {
      tmp = maxvalue(children[i], depth + 1);
      if (tmp < v) {
        v = tmp;
      }
    }
  }

  return v;
}

/*
 * Same as minvalue, except with alpha-beta cutoff.
 */
int minvalue_ab(Node n, int depth, int alpha, int beta) {
  minimax_called++;

  if (trace || fulltrace) {
    cout << "\nInvoking minimax at level: " << depth
         << " (minimizing, alpha =" << alpha << " beta =" << beta << ")"
         << endl;
    cout << (n.getPlayer() ? "black" : "red") << endl;
    printCompact(n.board);
  }

  if (fulltrace) {
    printStats();
    cout << "Hueristic value for this move: "
         << (alth ? hueristic2(n) : hueristic(n)) << endl;
    ;
  }

  if (terminalCheck(n) || depth == dvalue)
    return alth ? hueristic2(n) : hueristic(n);

  vector<Node> children;

  int v = INT_MAX;
  int tmp = 0;
  int num_black = 0, num_red = 0;

  for (int i = 0; i < 4; i++) {
    if (n.blackpos[i] != 0)
      num_black++;
  }

  for (int i = 0; i < 4; i++) {
    if (n.redpos[i] != 0)
      num_red++;
  }

  if ((!n.getPlayer() ? num_black < 4 : num_red < 4)) {
    generateChildren2(n, children);
    moves_generated += children.size();
  } else {
    generateChildren(n, children);
    moves_generated += children.size();
  }

  // If plausible
  if (plaus || forward) {
    plausible(children);
  }

  if (forward) {
    vector<Node>::iterator iter;
    int i = 0;

    for (iter = children.end(); iter != children.begin(); iter--) {
      tmp = maxvalue_ab(children[i], depth + 1, alpha, beta);

      if (tmp < v) {
        v = tmp;
      }

      if (v <= alpha) {
        alpha_cutoff++;
        return v;
      }

      if (v < beta)
        beta = v;

      i++;
      if (i == fvalue)
        break;
    }
  } else {
    for (int i = 0; i < (int)children.size(); i++) {
      tmp = maxvalue_ab(children[i], depth + 1, alpha, beta);

      if (tmp < v) {
        v = tmp;
      }

      if (v <= alpha) {
        alpha_cutoff++;
        return v;
      }

      if (v < beta)
        beta = v;
    }
  }

  return v;
}

void plausible(vector<Node> &children) {
  // sort children vector.
  for (int i = 0; i < (int)children.size(); i++) {
    if (!alth)
      children[i].setValue(hueristic(children[i]));
    else
      children[i].setValue(hueristic2(children[i]));
  }

  sort(children.begin(), children.end());
}

/*
 * This function generates children for nodes that have not place all their pegs
 * yet.
 */
void generateChildren2(Node n, vector<Node> &children) {
  int row = 0, col = 0;
  Node kid(n);
  kid.setPlayer(!n.getPlayer());
  vector<int> move(2);
  int to;

  int piece_count = 0;

  // How many pieces has this player placed?
  for (int i = 0; i < 4; i++) {
    if ((n.getPlayer() ? kid.blackpos[i] : kid.redpos[i]) != 0)
      piece_count++;
  }

  for (row = 0; row < 5; row++) {
    for (col = 0; col < 5; col++) {
      // Only place piece where no one else is.
      if (kid.board[row][col] == '-') {
        to = ((col + 1) + (row * 5));

        kid.board[row][col] = (n.getPlayer() ? 'B' : 'R');
        (n.getPlayer() ? kid.blackpos[piece_count] : kid.redpos[piece_count]) =
            to;

        // Set move.
        move[0] = to;
        kid.setMove(move);

        // Add child to list.
        children.push_back(kid);

        // Set the piece back.
        kid.board[row][col] = '-';
      }
    }
  }
}

/*
 * This function generates children for players who have placed all their
 * children.
 */
void generateChildren(Node n, vector<Node> &children) {
  int location, row, col, k;
  Node kid(n);
  kid.setPlayer(!n.getPlayer());

  vector<int> m(2);
  location = 0;
  int newlocation = 0, i = 0, j = 0;

  for (k = 0; k < 4; k++) {
    // Which player do we build for?
    location = (n.getPlayer() ? n.blackpos[k] : n.redpos[k]);

    // convert to row/col indicies (numbers start at 1)
    row = (location - 1) % 5;
    col = (location - 1) - (row * 5);

    // Pick the current piece up...
    kid.board[row][col] = '-';
    m[0] = location;

    // Only place piece where no one else is.
    // And make sure you don't go off the board.

    // 1. directly beneath.
    newlocation = location + 5;
    i = (newlocation - 1) % 5;
    j = (newlocation - 1) - (i * 5);

    if (newlocation < 26 && kid.board[i][j] == '-') {
      kid.board[i][j] = (n.getPlayer() ? 'B' : 'R');
      (n.getPlayer() ? kid.blackpos[k] : kid.redpos[k]) = newlocation;

      // Set move.
      m[1] = newlocation;
      kid.setMove(m);

      // Add child to children vector.
      children.push_back(kid);

      // Reverse state back.
      kid.board[i][j] = '-';
    }

    // 2. Bottom right diagonal.
    newlocation = location + 6;
    i = (newlocation - 1) % 5;
    j = (newlocation - 1) - (i * 5);

    if (((location % 5) != 0) && (newlocation < 26) && kid.board[i][j] == '-') {
      kid.board[i][j] = (n.getPlayer() ? 'B' : 'R');
      (n.getPlayer() ? kid.blackpos[k] : kid.redpos[k]) = newlocation;

      // Set move.
      m[1] = newlocation;
      kid.setMove(m);

      children.push_back(kid);

      // Reverse state back.
      kid.board[i][j] = '-';
    }

    // 3. directly right.
    newlocation = location + 1;
    i = (newlocation - 1) % 5;
    j = (newlocation - 1) - (i * 5);

    if (((location % 5) != 0) && kid.board[i][j] == '-') {
      kid.board[i][j] = (n.getPlayer() ? 'B' : 'R');
      (n.getPlayer() ? kid.blackpos[k] : kid.redpos[k]) = newlocation;

      // Set move.
      m[1] = newlocation;
      kid.setMove(m);
      children.push_back(kid);

      // Reverse state back.
      kid.board[i][j] = '-';
    }

    // 4. top right diagonal
    newlocation = location - 4;
    i = (newlocation - 1) % 5;
    j = (newlocation - 1) - (i * 5);

    if (((location % 5) != 0) && newlocation > 0 && kid.board[i][j] == '-') {
      kid.board[i][j] = (n.getPlayer() ? 'B' : 'R');
      (n.getPlayer() ? kid.blackpos[k] : kid.redpos[k]) = newlocation;

      // Set move.
      m[1] = newlocation;
      kid.setMove(m);
      children.push_back(kid);

      // Reverse state back.
      kid.board[i][j] = '-';
    }

    // 5. top
    newlocation = location - 5;
    i = (newlocation - 1) % 5;
    j = (newlocation - 1) - (i * 5);

    if (newlocation > 0 && kid.board[i][j] == '-') {
      kid.board[i][j] = (n.getPlayer() ? 'B' : 'R');
      (n.getPlayer() ? kid.blackpos[k] : kid.redpos[k]) = newlocation;

      // Set move.
      m[1] = newlocation;
      kid.setMove(m);
      children.push_back(kid);

      // Reverse state back.
      kid.board[i][j] = '-';
    }

    // 6. top left diagonal
    newlocation = location - 6;
    i = (newlocation - 1) % 5;
    j = (newlocation - 1) - (i * 5);

    if (((location % 5 != 1) && (newlocation > 0)) && kid.board[i][j] == '-') {
      kid.board[i][j] = (n.getPlayer() ? 'B' : 'R');
      (n.getPlayer() ? kid.blackpos[k] : kid.redpos[k]) = newlocation;

      // Set move.
      m[1] = newlocation;
      kid.setMove(m);
      children.push_back(kid);

      // Reverse state back.
      kid.board[i][j] = '-';
    }

    // 7. left
    newlocation = location - 1;
    i = (newlocation - 1) % 5;
    j = (newlocation - 1) - (i * 5);

    if ((location % 5 != 1) && (kid.board[i][j] == '-')) {
      kid.board[i][j] = (n.getPlayer() ? 'B' : 'R');
      (n.getPlayer() ? kid.blackpos[k] : kid.redpos[k]) = newlocation;

      // Set move.
      m[1] = newlocation;
      kid.setMove(m);
      children.push_back(kid);

      // Reverse state back.
      kid.board[i][j] = '-';
    }

    // 8. bottom left diagonal
    newlocation = location + 4;
    i = (newlocation - 1) % 5;
    j = (newlocation - 1) - (i * 5);

    if (((location % 5 != 1) && (newlocation < 26)) && kid.board[i][j] == '-') {
      kid.board[i][j] = (n.getPlayer() ? 'B' : 'R');
      (n.getPlayer() ? kid.blackpos[k] : kid.redpos[k]) = newlocation;

      // Set move.
      m[1] = newlocation;
      kid.setMove(m);
      children.push_back(kid);

      // Reverse state back.
      kid.board[i][j] = '-';
    }

    // Replace the piece for next round...
    kid.board[row][col] = (n.getPlayer() ? 'B' : 'R');
    (n.getPlayer() ? kid.blackpos[k] : kid.redpos[k]) = location;
  }
}

/*
 * Returns true if n is a terminal. False otherwise.
 *
 */
bool terminalCheck(Node n) {
  if ((n.getPlayer() ? checkWin2(n.redpos) : checkWin2(n.blackpos)))
    return true;
  else
    return false;

  return false;
}
