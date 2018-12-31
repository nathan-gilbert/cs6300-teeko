/*
 * util.cpp
 *
 * This file contains many functions that were handy to all other 
 * modules in thsi game. 
 *
 */


#include "util.h"

/* Enumerating all possible wins. */
string poswins[] = {
	"1 2 3 4",
	"2 3 4 5",
	"6 7 8 9",
	"7 8 9 10",
	"11 12 13 14",
	"12 13 14 15",
	"16 17 18 19",
	"17 18 19 20",
	"21 22 23 24",
	"22 23 24 25",
	"1 6 11 16",
	"6 11 16 21",
	"2 7 12 17",
	"7 12 17 22",
	"3 8 13 18",
	"8 13 18 23",
	"4 9 14 19",
	"9 14 19 24",
	"5 10 15 20",
	"10 15 20 25",
	"1 7 13 19",
	"7 13 19 25",
	"5 9 13 17",
	"9 13 17 21",
	"6 12 18 24",
	"2 8 14 20",
	"4 8 12 16",
	"22 18 14 10",
	"1 2 6 7",
	"2 3 7 8",
	"3 4 8 9",
	"4 5 9 10",
	"6 7 11 12",
	"7 8 12 13",
	"8 9 13 14",
	"9 10 14 15",
	"11 12 16 17",
	"12 13 17 18",
	"13 14 18 19",
	"14 15 19 20",
	"16 17 21 22",
	"17 18 22 23",
	"18 19 23 24",
	"19 20 24 25"
};

int hueristic(Node n)
{
	if(terminalCheck(n))
	{
		if(machine_plays_black && n.getPlayer())
			return INT_MIN;
		else if(machine_plays_black && !n.getPlayer())
			return INT_MAX;
		else if(!machine_plays_black && !n.getPlayer())
			return INT_MIN;
		else
			return INT_MAX;

		return n.getPlayer() ? INT_MAX : INT_MIN;
	}

	int black_win_count = 0;
	int red_win_count = 0;
	int total_black = 0;
	int total_red = 0;
	int black_vacant = 0;
	int red_vacant = 0;
	int location = 0;
	int row, col;
	int k,i;

	vector<int> pos_win(4);
	vector<int> blackcopy(4);
	vector<int> redcopy(4);
	vector<int>::iterator iter;

	//For the black player at this node.
	for(k = 0;k < N_POS_WINS;k++)
	{
		getWin(pos_win, poswins[k]);

		blackcopy[0] = pos_win[0];
		blackcopy[1] = pos_win[1];
		blackcopy[2] = pos_win[2];
		blackcopy[3] = pos_win[3];
	
		redcopy[0] = pos_win[0];
		redcopy[1] = pos_win[1];
		redcopy[2] = pos_win[2];
		redcopy[3] = pos_win[3];

		/* For black. */
		for(i = 0;i < 4; i++)
		{
			if(n.blackpos[i] == 0)
				continue;

			for(iter = blackcopy.begin();iter != blackcopy.end();iter++)
			{
				if(*iter == n.blackpos[i])
				{
					black_win_count++;
					blackcopy.erase(iter);
					break;
				}
			}
		}

		/*
		 * Essentially, if this vector has size 1, then we know 
		 * three winning positions have been occupied at this node. 
		 */
		if((int)blackcopy.size() == 1)
		{
			location = blackcopy[0];
			row = ( location - 1 ) % BOARD_SIZE;
			col = ( location - 1 ) - ( row * BOARD_SIZE );

			if(n.board[row][col] == '-')
				black_vacant++;
		}

		/* 
		 * Does this player have 3 in a winning position with 
		 * the fourth position open?
		 */
		if(black_vacant == 1 && black_win_count == 3)
			total_black++;

		black_win_count = 0;
		black_vacant = 0;

		/* For red. */
		for(i = 0;i < 4; i++)
		{
			if(n.redpos[i] == 0)
				continue;

			for(iter = redcopy.begin();iter != redcopy.end();iter++)
			{
				if(*iter == n.redpos[i])
				{
					red_win_count++;
					redcopy.erase(iter);
					break;
				}
			}
		}

		if((int)redcopy.size() == 1)
		{
			location = redcopy[0];
			row = ( location - 1 ) % BOARD_SIZE;
			col = ( location - 1 ) - ( row * BOARD_SIZE );

			if(n.board[row][col] == '-')
				red_vacant++;
		}

		if(red_vacant == 1 && red_win_count == 3)
			total_red++;

		red_win_count = 0;
		red_vacant = 0;

		pos_win.clear();
		pos_win.resize(4,0);
		blackcopy.clear();
		blackcopy.resize(4,0);
		redcopy.clear();
		redcopy.resize(4,0);
	}

	return n.getPlayer() ? (total_black - total_red) : (total_red - total_black);
}

int hueristic2(Node n)
{
	//Seed the random numbers. 
	srand((unsigned)time(0));
	int i;

	/* Return max possible if at a terminal. */
	if(terminalCheck(n))
	{
		if(machine_plays_black && n.getPlayer())
			return INT_MIN;
		else if(machine_plays_black && !n.getPlayer())
			return INT_MAX;
		else if(!machine_plays_black && !n.getPlayer())
			return INT_MIN;
		else
			return INT_MAX;

		return n.getPlayer() ? INT_MAX : INT_MIN;
	}

	int distance = 0, current = 0;
	int bonus = 0;
	int cost = 0;

	for(i = 0; i < 4; i++)
	{
		bonus = 1 + rand() % 50;
		cost = 1 + rand() % 100;

		//He who controls 13, controls the universe!
		if(current == 13)
		{
			distance += 300;
			continue;
		}

		//Depending on which player we are dealing we need different 
		//locations. 
		current = n.getPlayer() ? n.blackpos[i] : n.redpos[i];

		//Is this piece in the center?
		if((current > 5) && (current % 5 != 1) && (current % 5 != 0) && current < 21)
		{
			/* Add points if pieces are in center. */
			distance += bonus;
		}
		else
		{
			/* Subtract points if has pegs on outer ring of board. */
			distance -= cost;
		}
	}

	//The opponents win count. 
	int k, row, col, location;
	int win_count = 0;
	int win = 0;	
	vector<int> pos_win(4);
	vector<int>::iterator iter;

	/*
	 * This subtracts major points for moves that have the opposition with three
	 * pieces in a winning position. This gives motivation for the search to undertake blocking 
	 * moves. 
	 */
	for(k = 0;k < N_POS_WINS;k++)
	{
		getWin(pos_win, poswins[k]);

		for(i = 0;i < 4; i++)
		{
			current = !n.getPlayer() ? n.blackpos[i] : n.redpos[i];

			if(current == 0)
				continue;

			for(iter = pos_win.begin();iter != pos_win.end(); iter++)
			{
				if(*iter == current)
				{
					win_count++;
					pos_win.erase(iter);
					break;
				}
			}
		}

		if((int)pos_win.size() == 1)
		{
			location = pos_win[0];
			row = ( location - 1 ) % BOARD_SIZE;
			col = ( location - 1 ) - ( row * BOARD_SIZE );

			if(n.board[row][col] == '-')
				win++;
		}

		win_count = 0;
		pos_win.clear();
		pos_win.resize(4,0);
	}

	//Calculate adjusted weight.
	distance -= 10*win;

	return distance;
}

/*
 * A function to help determine if a node 
 * has a win. 
 */
bool checkWin2(int pos[])
{
	for(int i = 0;i <4;i++)
	{
		if(pos[i] == 0)
			return false;
	}

	int win_count = 0;
	vector<int> pos_win(4);

	for(int i = 0;i < N_POS_WINS;i++)
	{
		getWin(pos_win, poswins[i]);

		for(int i = 0;i < 4; i++)
		{
			for(int j = 0;j < 4;j++)
			{
				if(pos[i] == pos_win[j])
				{
					win_count++;
					continue;
				}
			}
		}

		if(win_count == 4)
			return true;

		win_count = 0;
		pos_win.clear();
	}

	return false;
}

/*
 * player = true is black.
 * player = false is red.
 */
bool checkWin(bool player)
{
	if(piece_count_black < 4)
		return false;

	int win_count = 0;
	vector<int> pos_win(4);

	for(int i = 0;i < N_POS_WINS;i++)
	{
		getWin(pos_win, poswins[i]);

		if(player)
		{
			for(int i = 0;i < 4; i++)
			{
				for(int j = 0;j < 4;j++)
				{
					if(blackpos[i] == pos_win[j])
					{
						win_count++;
						continue;
					}
				}
			}

			if(win_count == 4)
				return true;

			win_count = 0;
		}
		else
		{
			for(int i = 0;i < 4; i++)
			{
				for(int j = 0;j < 4;j++)
				{
					if(redpos[i] == pos_win[j])
					{
						win_count++;
						continue;
					}
				}
			}
			
			if(win_count == 4)
				return true;
	
			win_count = 0;	
		}

		pos_win.clear();
	}

	return false;
}

void getWin(vector<int> &tmp, string win)
{
	vector<string> pos;

	//Get the four placements of this win.
	tokenize(win, pos);

	for(int j = 0;j < 4;j++)
	{
		//tmp.push_back(atoi(pos[j].c_str()));
		tmp[j] = atoi(pos[j].c_str());
	}

	pos.clear();
}

/*
 * Initializes the board. 
 *
 */
void initBoard(void)
{
	piece_count_red = 0;
	piece_count_black = 0;

	int row, col;

	/* Initialize to empty */
	for(row = 0; row < BOARD_SIZE; row++) 
	{
		for(col = 0; col < BOARD_SIZE; col++) 
		{
			teeko_board[row][col] = '-';
		}
   }
}

/*
 * Prints the board, final is true if this is board after 
 * a win. Final == false is the board during gameplay. 
 */
void printBoard(bool final)
{
    cout << "\tThe current game-state:\n\n" << endl;
	 int row, col;

    /* Indicate black/red and machine/user */
    if(black_to_move) 
	 {
		 if(!final)
		 {
			 printf ( "black's turn to move" );

			 if(machine_plays_black && !nocomp) 
			 {
				printf ( " (machine move)\n" );
			 } 
			 else if(noplayer && !machine_plays_black)
			 {
				printf ( " (random move)\n" );
			 }
			 else 
			 {
				printf ( " (user move)\n" );
			 }
		 }
    } 
	 else 
	 {
		 if(!final)
		 {
        printf ( "red's turn to move" );

        if ((machine_plays_black || nocomp) && !noplayer) 
		  {
            printf ( " (user move)\n" );
        }
		  else if(noplayer && machine_plays_black)
		  {
            printf ( " (random move)\n" );
		  }
		  else 
		  {
            printf ( " (machine move)\n" );
        }
		 }
    }

    for (row = 0; row < BOARD_SIZE; row++) 
	 {
        /* borders of board */
		for (col = 0; col < BOARD_SIZE; col++) 
		{
	  	  printf ( "+---" );
		}

		printf("+\n");

      /* pieces on board */
      for ( col = 0; col < BOARD_SIZE; col++ ) 
		{
	  	  printf ( "| %c ", teeko_board[row][col] );
		}
      
		printf ( "|\n" );
    }

    /* rest of border */
    for ( col = 0; col < BOARD_SIZE; col++ ) 
	 {
		printf ( "+---" );
    }

    printf ( "+\n" );
	 cout << endl;
}

/*
 * Prints all useful information about command line flags 
 * and user choice.
 */
void preamble(void)
{
	//Lists commandline arguments that have been set.
	if(machine_plays_black)
	{
		cout << "Machine is playing as black." << endl;
	}
	else
	{
		cout << "Machine is playing as red." << endl;
	}

	cout << "Depth Limit: " << dvalue << " plies." << endl;

	if(!ab)
		cout << "No Alpha-Beta pruning." << endl;

	if(ab && plaus)
		cout << "Alpha-Beta pruning with plausible move ordering." << endl;
	
	if(ab && (!plaus || !forward))
		cout << "Alpha-Beta pruning with no plausible move ordering." << endl;

	if(ab && forward)
	{
		cout << "Alpha-Beta pruning with no plausible move ordering and forward pruning." << endl;
		cout << "Forwad Pruning: " << fvalue << endl;
	}

	if(nocomp)
		cout << "No computer players. " << endl;

}

void printCompact(char board[][BOARD_SIZE])
{
	int row, col;
    cout << "The (compact) game-state:\n" << endl;

    /* first the count, then the square numbers */
    printf ( "%d:", piece_count_black );

    for(row = 0; row < BOARD_SIZE; row++) 
	 {
		for(col = 0; col < BOARD_SIZE; col++) 
		{
	   	if( board[row][col] == 'B') 
			{
				/* square numbers start at 1 */
				printf ( " %2d", ( row * BOARD_SIZE ) + col + 1 );
	    	}
		}
    }

    printf ( "\n" );

    /* first the count, then the square numbers */
    printf ( "%d:", piece_count_red );

    for ( row = 0; row < BOARD_SIZE; row++ ) 
	 {
		for ( col = 0; col < BOARD_SIZE; col++ ) 
		{
	  	  if ( board[row][col] == 'R' ) 
		  {
				/* square numbers start at 1 */
				printf ( " %2d", ( row * BOARD_SIZE ) + col + 1 );
	     }
		}
    }

	 cout << endl;
}

/*
 * Tokenizes a string. 
 */
void tokenize(string s, vector<string> &tokens) 
{
	tokens.clear();
	string tmp = "";

	for(int i = 0;i < (int)s.length();i++)
	{
		if(s.at(i) == ' ')
		{
			tokens.push_back(tmp);
			tmp = "";
			continue;
		}
		else
		{
			if((int)tmp.size() == 0)
			{
				tmp.assign(1,s.at(i));
			}
			else
			{
				tmp.append(1,s.at(i));	
			}	
		}
	}

	tokens.push_back(tmp);
}

/*
 * Copes the state between two nodes or a 
 * node and the master board. 
 */
void copyState(Node &kid, char board[][BOARD_SIZE])
{
	for(int i =0;i <BOARD_SIZE;i++)
	{
		for(int j = 0;j < BOARD_SIZE;j++)
		{
			kid.board[i][j] = board[i][j];
		}
	}
}

