/*
 * Node.cpp
 * Definition file for the representation of moves. 
 */
#include "node.h"

//Default constructor
Node::Node()
{
	move.resize(2,0);
	term = false;
	value = 0;
	player = true;
	move[0] = 0;
	move[1] = 0;

	for(int i = 0;i < 4;i++)
	{
		blackpos[i] = 0;
		redpos[i] = 0;	
	}
}

//Copy constructor
Node::Node(const Node &n)
{
	move.resize(2,0);
	term = n.terminal();
	value = n.getValue();
	player = n.getPlayer();

	for(int i = 0;i < BOARD_SIZE;i++)
	{
		for(int j = 0; j < BOARD_SIZE;j++)
		{
			board[i][j] = n.board[i][j];
		}
	}

	for(int i = 0;i < 4;i++)
	{
		blackpos[i] = n.blackpos[i];
		redpos[i] = n.redpos[i];
	}

	move[0] = n.getMove()[0];	
	move[1] = n.getMove()[1];	
}

Node::~Node()
{
}

/*
 * Various set and get methods for the private variables of a node.
 */
int Node::getValue() const
{
	return value;
}

void Node::setValue(int v)
{
	value = v;
}

bool Node::terminal() const
{
	return term;
}

void Node::setPlayer(bool p)
{
	player = p;
}

bool Node::getPlayer(void) const
{
	return player;
}

void Node::setTerm(bool t)
{
	term = t;

	if(term)
		value = INT_MAX;
}

vector<int> Node::getMove() const
{
	return move;
}

/*
 * The move that got us to this node. 
 */
void Node::setMove(vector<int> m)
{
	move[0] = m[0];
	move[1] = m[1];
}

/* Less than defined in terms of greater-than. Used to sort the vector of nodes during plausible move ordering. */
bool Node::operator< (const Node rhs) const
{
    return (value > rhs.getValue());
}
