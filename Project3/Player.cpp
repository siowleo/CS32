#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
	AwfulPlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
private:
	Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
	: Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
	// Clustering ships is bad strategy
	b.clear();
	for (int k = 0; k < game().nShips(); k++)
		if (!b.placeShip(Point(k, 0), k, HORIZONTAL))
			return false;
	return true;
}

Point AwfulPlayer::recommendAttack()
{
	if (m_lastCellAttacked.c > 0)
		m_lastCellAttacked.c--;
	else
	{
		m_lastCellAttacked.c = game().cols() - 1;
		if (m_lastCellAttacked.r > 0)
			m_lastCellAttacked.r--;
		else
			m_lastCellAttacked.r = game().rows() - 1;
	}
	return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
	bool /* shotHit */, bool /* shipDestroyed */,
	int /* shipId */)
{
	// AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
	// AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************
bool getLineWithTwoIntegers(int& r, int& c)
{
	bool result(cin >> r >> c);
	if (!result)
		cin.clear();  // clear error state so can do more input operations
	cin.ignore(10000, '\n');
	return result;
}


// TODO:  You need to replace this with a real class declaration and
//        implementation.
// typedef AwfulPlayer HumanPlayer;

class HumanPlayer : public Player
{
public:
	HumanPlayer(string nm, const Game& g);
	virtual ~HumanPlayer();
	virtual bool isHuman() const;
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	bool getDirection(Direction& d) const;
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
	: Player(nm, g)
{}

HumanPlayer::~HumanPlayer()
{}

bool HumanPlayer::isHuman() const
{
	return true;
}

bool HumanPlayer::placeShips(Board& b)
{
	Direction dir;
	int r;
	int c;
	string location;
	const Game& m_game = Player::game();


	cout << "There are" << " " << m_game.nShips() << " " << "ships." << endl;
	b.clear();
	b.display(false); 
	for (int i = 0; i < m_game.nShips(); i++) //Loop around the array
	{
		cout << "h = horizontal, v = vertical" << endl; 
		cout << "Enter h or v for the direction of " << m_game.shipName(i) << " "<< "(" << "with a length of " << m_game.shipLength(i) << " " << "units): ";
		if (getDirection(dir) == false)  
		{
			cout << "Please enter h or v for direction of the ship." << endl;
			continue;
		}
			while (!false)
			{
				if (dir == HORIZONTAL) //determine if the position is the leftmost if the ship is oriented horizontally
				{
					location = "leftmost";
				}
				if (dir == VERTICAL) //determine if the position is the topmost if the ship is oriented vertically
				{
					location = "topmost";
				}
				cout << "Enter row and column of " << location << " " << "cell with a space in between the integers (e.g, 3 5): ";
				if (getLineWithTwoIntegers(r, c) == false) //if false, the entered inputs are not valid
				{
					cout << "Incorrect input. Please enter two integers." << endl;
					continue;
				}
				if (r < 0 || r >= m_game.rows() || c < 0 || c >= m_game.cols()) //if the placement of the ship exceeds the limits of the board, check error
				{
					cout << "You can't place the ship here!" << endl;
					continue;
				}
				if(!b.placeShip(Point(r, c), i, dir)) //the the placement of the ship equates to false
				{
					cout << "You can't place the ship here!" << endl;
					continue;
				}
				break;
			}
		b.display(false);  
	}
	return true;
}

Point HumanPlayer::recommendAttack()
{
	int r, c;
	while (true)
	{
		cout << "Enter the row and column with a space in between the integers to attack (e.g, 3 5) : ";
		if (!getLineWithTwoIntegers(r, c)) //if the input is not valid, please enter again
		{
			cout << "Incorrect input. Please enter two integers." << endl;
			continue;
		}
		break;
	}
	return Point(r, c);
}

bool HumanPlayer::getDirection(Direction& dir) const
{
	char direction;
	cin >> direction;
	if (direction == 'h') //determines if direction is horizontal
	{
		dir = HORIZONTAL;
	}
	else if (direction == 'v')//determines if direction is vertical
	{
		dir = VERTICAL;
	}
	else
	{
		return false;
	}
	return true;
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	//nothing here for human player
}

void HumanPlayer::recordAttackByOpponent(Point p)
{
	//nothing here for human player
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
// typedef AwfulPlayer MediocrePlayer;

class MediocrePlayer : public Player
{
public:
	MediocrePlayer(string nm, const Game& g);
	virtual ~MediocrePlayer();
	virtual bool placeShips(Board& b);
	bool placeShipsHelp(Board& b, int placedNumShips);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
private:
	bool** attacked;  
	Point hit;  
	stack<Point> cells;
	int state;
	vector<Point> availiblePoints;
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g) : Player(nm, g)
{
	hit.r = -1; //starting point
	hit.c = -1;
	state = 1;
	attacked = new bool*[game().rows()]; //dynicamically allocate for the 2D board
	for (int i = 0; i < game().rows(); ++i)
	{
		attacked[i] = new bool[game().cols()]; 
		for (int j = 0; j < game().cols(); j++)
		{
			attacked[i][j] = false; //initialize the board to false
		}
	}
}

MediocrePlayer::~MediocrePlayer()
{
	for (int i = 0; i < game().rows(); i++)
	{
		delete attacked[i]; //clean up, prevent memory leak
	}
}

bool MediocrePlayer::placeShips(Board& b)
{
	
	const Game& m_game = game();

	b.clear(); //clear board
	int g = 0;
	while (g < 50) //try the function fifty times
	{
		b.block(); //bloch the board
		int r = 0;
		while ( r < m_game.rows()) 
		{

			if (!cells.empty()) //if the stacj is not empty, return false
			{
				return false;
			}
			int c = 0;
			while ( c < m_game.cols()) //limit is the maximum number of columns on the board
			{
				if (b.placeShip(Point(r, c), 0, HORIZONTAL)) //if the placement of the ship is good horinztonally
				{
					if (placeShipsHelp(b, 1))
					{
						b.unblock(); //unblock board element if placement of the ship is good
						return true;
					}
					b.unplaceShip(Point(r, c), 0, VERTICAL); 
					b.unplaceShip(Point(r, c), 0, HORIZONTAL);
				}
				if (b.placeShip(Point(r, c), 0, VERTICAL)) //if the placement of the ship is good vertically
				{
					if (placeShipsHelp(b, 1))
					{
						b.unblock(); //unblock board element if placement of the ship is good
						return true;
					}
					b.unplaceShip(Point(r, c), 0, VERTICAL); 
					b.unplaceShip(Point(r, c), 0, HORIZONTAL);
				}
				++c;
			}
			++r;
		}
		b.unblock();
		++g;
	}
	return false;
}

bool MediocrePlayer::placeShipsHelp(Board& b, int numShips)
{
	
	const Game& m_game = game();
	if (numShips < 0) //error check for negative input
	{
		return false;
	}
	if (numShips == m_game.nShips()) //return true is number of ships on board is equal to those that are yet to be placed
	{
		return true;
	}
	int r = 0;
	while( r < m_game.rows())
	{
		int c = 0;
		while( c < m_game.cols())
		{
			int i = 1;
			while(i < m_game.nShips())
			{
				if (b.placeShip(Point(r, c), i, HORIZONTAL))//if the placement of the ship is good horizontally
				{
					if (placeShipsHelp(b, numShips + 1)) //increment the number of ships left
					{
						return true;
					}
					b.unplaceShip(Point(r, c), i, HORIZONTAL);
				}
				if (b.placeShip(Point(r, c), i, VERTICAL))//if the placement of the ship is good vertically
				{
					if (placeShipsHelp(b, numShips + 1))//increment the number of ships left
					{
						return true;
					}
					b.unplaceShip(Point(r, c), i, VERTICAL);
				}
				i++;
			}
			c++;
		}
		r++;
	}

	return false;
}

Point MediocrePlayer::recommendAttack()  
{
	const Game& m_game = game();
	vector<Point>::iterator it; //iterating through the vector of collected points
	it = availiblePoints.begin(); //iterate beginning from the start of the vector
	Point donePoints(0, 0); //initial point
	if (state == 1) //1st state, mediocreplayer will attack randomly
	{
		while (attacked[donePoints.r][donePoints.c] == true)
		{
			donePoints = m_game.randomPoint();
		}
	}
	if (state == 2) //2nd state, will determine the next move based on previous attacked coordinates
	{
		int gg = randInt((int)availiblePoints.size());
	    donePoints = availiblePoints[gg];
		while (attacked[donePoints.r][donePoints.c] == true)
		{
			int temp = randInt((int)availiblePoints.size());
			donePoints = availiblePoints[temp];
		}
		availiblePoints.erase(it + gg);
	}
	return donePoints;
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	const Game& m_game = game();
	if (validShot == false) //check to see if the shot is valid
	{
		return;
	}
	attacked[p.r][p.c] = true;
	if (shotHit == false && state == 1)
	{
		return;
	}
	if ((shotHit == false && state == 2) || (state == 2 && shotHit && !shipDestroyed))
	{
		return;
	}
	if (shipDestroyed || m_game.shipLength(shipId) >= 6)
	{
		state = 1;
	}
	if (shotHit == true && shipDestroyed == false && m_game.shipLength(shipId) < 6)
	{
		state = 2;
		int r = p.r - 4;
		while(r <= p.r + 4)
		{
			if (m_game.isValid(Point(r, p.c)) && r != p.r)
			{
				availiblePoints.push_back(Point(r, p.c));
			}
			r++;
		}
		int c = p.c - 4;
		while (c <= p.c + 4)
		{
			if (m_game.isValid(Point(p.r, c)) && c != p.c)
			{
				availiblePoints.push_back(Point(p.r, c));
			}
			c++;
		}
	}
}

void MediocrePlayer::recordAttackByOpponent(Point p)
{}  // MediocrePlayer ignores opponent's attach

	// Remember that Mediocre::placeShips(Board& b) must start by calling
	// b.block(), and must call b.unblock() just before returning.

	//*********************************************************************
	//  GoodPlayer
	//*********************************************************************

	// TODO:  You need to replace this with a real class declaration and
	//        implementation.
	// typedef AwfulPlayer GoodPlayer;

	// GoodPlayer will only randomly hit those board[i][j]
	// such that (i + j) % minLength == 0
	// where minLength is the minimum length of battleship in the game.

class GoodPlayer : public Player
{
public:
	GoodPlayer(string nm, const Game& g);
	virtual ~GoodPlayer();

	virtual bool placeShips(Board& b);
	bool GoodPlayer::placeShipsHelp(Board& b, int numShips);
	virtual bool isHuman() const{ return false; }
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
	int search(Point center);

private:
	char** attacked;
	int dir;            
	int step;
	int unitNumbers;
	vector<int>  shipPoints;    
	vector<Point> centers;           
	vector<Point> hit;         
	bool attack[10][10];
};


GoodPlayer::GoodPlayer(string nm, const Game& g): Player(nm, g)
{
	dir = 0;
	step = g.shipLength(0); //initiliaze to o ship length for player step
	unitNumbers = 1;
	step = 2;
	attacked = new char*[g.rows()];

	for (int k = 0; k < game().rows(); k++) //initilize board to all false booleans
	{ 
		for (int j = 0; j < game().cols(); j++) 
		{
			attack[k][j] = false;
		}
	}

	for (int i = 0; i < g.rows(); ++i)
	{
		attacked[i] = new char[g.cols()];
		for (int j = 0; j < g.cols(); ++j) //intilize and clean the board
		{
			if (attacked[i][j] != '.')
			{
				attacked[i][j] = '.';
			}
		}
	}
}

GoodPlayer::~GoodPlayer()
{
	for (int i = 0; i < game().rows(); i++)
	{
		delete attacked[i];
	}
}


bool GoodPlayer::placeShips(Board& b)
{
	const Game& m_game = game();

	for (int i = 0; i < m_game.nShips(); i++) //fill the vector will availible ships
	{
		shipPoints.push_back(m_game.shipLength(i));
	}

	for (int k = 0; k < 50; ++k)   //try fifty times to place the ship at appropriate places
	{
		b.clear();
		if (placeShipsHelp(b, 0) == true)
		{
			for (int r = 0; r < game().rows(); ++r)
			{
				for (int c = 0; c < game().cols(); ++c)
				{
					attacked[r][c] = '.';
				}
			}
			return true;
		}
	}

	return false;
}

bool GoodPlayer::placeShipsHelp(Board& b, int numShips)
{
	const Game& m_game = game();
	if (numShips == m_game.nShips())
	{
		return true;
	}
	int i = 0;
	while (i < 50)
	{
		int r = randInt(m_game.rows()), c = randInt(m_game.cols());
		if (attacked[r][c] == '.') 
		{
			if (randInt(2) % 2 == 0)
			{
				if ( b.placeShip(Point(r, c), numShips, HORIZONTAL)) //check to see is ship can be places horrizontally at specified point
				{
					if (placeShipsHelp(b, numShips + 1))
					{
						return true;
					}
					b.unplaceShip(Point(r, c), numShips, HORIZONTAL); //remove ship 
				}
			}
			else
			{
				if (b.placeShip(Point(r, c), numShips, VERTICAL)) //check to see is ship can be places vertically at specified point
				{
					if (placeShipsHelp(b, numShips + 1))
					{
						return true;
					}
					b.unplaceShip(Point(r, c), numShips, VERTICAL); //remove ship
				}
			}
		}
		++i;
	}

	return false;
}

Point GoodPlayer::recommendAttack()
{
	const int size = 200;
	const Game& m_game = game();
	int r;
	int c;
	if (dir == 0)  
	{
		for (int i = 0; i < size; i++)
		{
			if (attacked[randInt(m_game.rows())][randInt(m_game.cols())] == '.')
			{
				r = randInt(m_game.rows()); c = randInt(m_game.cols());
				if ((r + c) % step == 0 && attacked[r][c] == '.')
				{
					return Point(r, c);
				}
			}
		}
		step--;
		return recommendAttack();
	}
	if (dir != 0)
	{
		Point center = centers.back();
		//for the north direction, in which the row is subtracted by 1
		if (dir == 1)
		{
			r = center.r - unitNumbers;
			c = center.c;
		}
		//for the south direction, in which the row is subtracted by 1
		if (dir == 2)
		{
			r = center.r + unitNumbers;  
			c = center.c;
		}
		//for the west direction, in which the row is subtracted by 1
		if (dir == 3)
		{
			r = center.r;               
			c = center.c - unitNumbers;
		}
		//for the east direction, in which the row is subtracted by 1
		if (dir == 4)
		{
			r = center.r;               
			c = center.c + unitNumbers;
		}

		if (c < 0 || c >= m_game.cols() || r < 0 || r >= m_game.rows()) //error check to see if board limits are exceeded
		{
			dir = search(centers.back());
			unitNumbers = 1;
			if (dir == 0)  
			{
				centers.pop_back();
				if (centers.empty())
				{
					dir = 0;
				}
				if (!centers.empty())
				{
					dir = 4;
				}
			}

			return recommendAttack();
		}
		if (attacked[r][c] != '.') //check to see what point we hit
		{
			dir = search(centers.back());
			unitNumbers = 1;
			if (dir == 0)  
			{
				centers.pop_back();
				if (centers.empty())
				{
					dir = 0;
				}
				if (!centers.empty())
				{
					dir = 4;
				}
			}

			return recommendAttack();
		}
		return Point(r, c);
	}

}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	if (validShot == true) //if the shot is valid, we will redo the board
	{
		attacked[p.r][p.c] = 'X'; 
		if (shotHit == true)
		{
			hit.push_back(p);
		}
		if (shotHit == true  && shipDestroyed == true)  
		{
			unitNumbers = 1;  
			if (!centers.empty()) //if the stack isn't empty, pop it
			{
				centers.pop_back();
			}
			int i = 0;
			while(i < hit.size())
			{
				//check for the northern direction
				if (dir == 1)
				{
					if (hit[i].c != p.c || hit[i].r - p.r > game().shipLength(shipId) - 1)
					{
						centers.push_back(hit[i]);
					}
				}
				//check for the eastern direction
				if (dir == 2)
				{
					if (hit[i].c != p.c || p.r - hit[i].r > game().shipLength(shipId) - 1)
					{
						centers.push_back(hit[i]);
					}
				}
				//check for the southern direction
				if (dir == 3)
				{
					if (hit[i].r != p.r || hit[i].c - p.c > game().shipLength(shipId) - 1)
					{
						centers.push_back(hit[i]);
					}
				}
				//check for the western direction
				if (dir == 4)
				{
					if (hit[i].r != p.r || p.c - hit[i].c > game().shipLength(shipId) - 1)
					{
						centers.push_back(hit[i]);
					}
				}
				i++;
			}
			hit.clear();
			if (centers.empty()) // if the center is empty, no direction
			{
				dir = 0;     
			}
			else
			{
				dir = search(centers.back());
				hit.push_back(centers.back());  // if it isn't empty, put the center back onto the stack
			}
		}
	
		else if (shotHit == true && dir == 0)  
		{
			centers.clear();
			centers.push_back(p);
			dir = search(p);
		}
		else if (shotHit == true)
		{
			unitNumbers++;
		}
		else
		{
			
		}
	}
}

void GoodPlayer::recordAttackByOpponent(Point p)
{}

int GoodPlayer::search(Point center)
{
	int d = 0;
	int maximum = 0;
	int current = 0;
	int r1 = center.r + 1;
	int r2 = center.r - 1;
	int c1 = center.c - 1;
	int c2 = center.c + 1;
	for (int r = r1; r < game().rows() && attacked[r][center.c] == '.'; r++) //search for best move in the southern direction
		{
				current++;
		}

	if (current > maximum)
	{
		maximum = current;
		d = 2;
	}
	current = 0;
	for (int c = c1; c >= 0 && attacked[center.r][c] == '.'; c--) //search for best move in the western direction
	{
		current++;
	}
	if (current > maximum)
	{
		maximum = current;
		d = 3;
	}
	current = 0;
	for (int c = c2; c < game().cols() && attacked[center.r][c] == '.'; c++) //search for best move in the eastern direction
	{
		current++;
	}
	if (current > maximum)
	{
		maximum = current;
		d = 4;
	}
	for (int r = r2; r >= 0 && attacked[r][center.c] == '.'; r--) //search for best move in the northern direction
	{
		current++;
	}
	if (current > maximum)
	{
		maximum = current;
		d = 1;
	}
	current = 0;

	return d;
}

//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
	static string types[] = {
		"human", "awful", "mediocre", "good"
	};

	int pos;
	for (pos = 0; pos != sizeof(types) / sizeof(types[0]) && type != types[pos]; pos++);

	switch (pos)
	{
	case 0:  return new HumanPlayer(nm, g);
	case 1:  return new AwfulPlayer(nm, g);
	case 2:  return new MediocrePlayer(nm, g);
	case 3:  return new GoodPlayer(nm, g);
	default: return nullptr;
	}
}