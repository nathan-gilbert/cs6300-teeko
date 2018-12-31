/* 
 * Nathan Gilbert
 */
#ifndef NODE_H
#define NODE_H

#include <vector>

using namespace std;

#define BOARD_SIZE 5

class Node 
{
	private:
		int value;							//The heuristic value for this node.
		bool term;							//Is this node a terminal? (Rarely used in reality. )
		vector<int > move;				//The move that got us to this node.
		bool player; 						//Is this node a ply for Black or Red?

	public:
		//Constructors/Destructors
		Node();
		Node(const Node &);
		~Node();

		/* The state of things. */		
		char board[BOARD_SIZE][BOARD_SIZE];	
		int blackpos[4];
		int redpos[4];

		/* Methods */
		bool terminal() const;
		int getValue() const;
		void setValue(int);
		void setPlayer(bool);
		bool getPlayer(void) const;
		void setTerm(bool);
		void setMove(vector<int>);
		vector<int> getMove() const;
		
		/* Overloaded operator used for sorting. */
		bool operator< (const Node rhs) const;
};

#endif
