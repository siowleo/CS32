#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>

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
	virtual ~HumanPlayer(){}
	virtual bool isHuman() const { return true; }
	// place ships for human player
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
	// get direction for human player to place ships
	bool getLineWithDirection(Direction& d) const;
	// delete copy constructor and assignment operator
	HumanPlayer(const HumanPlayer&) = delete;
	HumanPlayer& operator=(const HumanPlayer&) = delete;
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
	: Player(nm, g)
{}

bool HumanPlayer::placeShips(Board& b)
{
	const Game& m_game = Player::game();
	int k = 0;
	Direction dir;
	int r, c;

	cout << Player::name() << " must place "
		<< m_game.nShips() << " ships.\n";
	b.clear();
	b.display(false);  // display the map
	while (k < m_game.nShips())
	{
		cout << "Enter h or v for direction of "
			<< m_game.shipName(k) << " (length"
			<< m_game.shipLength(k) << "): ";
		if (!getLineWithDirection(dir))  // try to get direction
		{
			cout << "Direction must be h or v.\n";
			continue;
		}

		while (true)
		{
			cout << "Enter row and column of "
				<< ((dir == HORIZONTAL) ? "leftmost " : "topmost ")
				<< "cell (e.g. 3 5): ";
			if (!getLineWithTwoIntegers(r, c))  // try to get coordinate
			{
				cout << "You must enter two integers.\n";
				continue;
			}
			if (r < 0 || r >= m_game.rows() || c < 0 || c >= m_game.cols() || !b.placeShip(Point(r, c), k, dir))  // try to place ship
			{
				cout << "The ship can not be placed there.\n";
				continue;
			}
			break;
		}
		k++;
		b.display(false);  // display the map
	}

	return true;
}

Point HumanPlayer::recommendAttack()
{
	int r, c;
	while (true)
	{
		cout << "Enter the row and column to attack (e.g, 3 5): ";
		if (!getLineWithTwoIntegers(r, c))
		{
			cout << "You must enter two integers.\n";
			continue;
		}
		break;
	}
	return Point(r, c);
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{}

void HumanPlayer::recordAttackByOpponent(Point p)
{}

bool HumanPlayer::getLineWithDirection(Direction& d) const
{
	string input;
	cin >> input;
	cin.ignore(10000, '\n');
	if (input[0] == 'h')
		d = HORIZONTAL;
	else if (input[0] == 'v')
		d = VERTICAL;
	else
		return false;
	return true;
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
	bool placeShipsRecursively(Board& b, int placedNumShips, int& totalNumShips, int& maxRows, int& maxCols);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);

	MediocrePlayer(const MediocrePlayer&) = delete;
	MediocrePlayer& operator=(const MediocrePlayer&) = delete;
private:
	bool** hitMap;   // true after hit
	Point hitPoint;  // if (-1,-1), it's in state 1, otherwise state 2
};

MediocrePlayer::MediocrePlayer(string nm, const Game& g)
	: Player(nm, g), hitPoint(-1, -1)
{
	hitMap = new bool*[game().rows()];
	for (int i = 0; i < game().rows(); ++i)
	{
		hitMap[i] = new bool[game().cols()];
		for (int j = 0; j < game().cols(); j++)
			hitMap[i][j] = false;
	}
}

MediocrePlayer::~MediocrePlayer()
{
	for (int i = 0; i < game().rows(); i++)
		delete[] hitMap[i];
	delete[] hitMap;
}

bool MediocrePlayer::placeShips(Board& b)
{
	const Game& m_game = game();
	int nShips = m_game.nShips();
	int nRows = m_game.rows(), nCols = m_game.cols();

	b.clear();
	for (int k = 0; k < 50; ++k)  // try 50 times to place ship
	{
		b.block();
		for (int r = 0; r < nRows; ++r)
			for (int c = 0; c < nCols; ++c)
				if (b.placeShip(Point(r, c), 0, HORIZONTAL) || b.placeShip(Point(r, c), 0, VERTICAL))  // place ships recursively only when the biggiest ship fits the board
				{
					if (placeShipsRecursively(b, 1, nShips, nRows, nCols))
					{
						b.unblock();
						return true;
					}
					b.unplaceShip(Point(r, c), 0, VERTICAL); b.unplaceShip(Point(r, c), 0, HORIZONTAL);
				}
		b.unblock();
	}
	return false;
}

bool MediocrePlayer::placeShipsRecursively(Board& b, int placedNumShips, int& totalNumShips, int& maxRows, int& maxCols)
{
	if (placedNumShips == totalNumShips)
		return true;
	// try place ships
	for (int r = 0; r < maxRows; r++)
		for (int c = 0; c < maxCols; c++)
			for (int i = 1; i < totalNumShips; i++)
			{
				// try horizontal
				if (b.placeShip(Point(r, c), i, HORIZONTAL))
				{
					if (placeShipsRecursively(b, placedNumShips + 1, totalNumShips, maxRows, maxCols))
						return true;
					b.unplaceShip(Point(r, c), i, HORIZONTAL);
				}
				// try vertical
				if (b.placeShip(Point(r, c), i, VERTICAL))
				{
					if (placeShipsRecursively(b, placedNumShips + 1, totalNumShips, maxRows, maxCols))
						return true;
					b.unplaceShip(Point(r, c), i, VERTICAL);
				}
			}

	return false;
}

Point MediocrePlayer::recommendAttack()  // TODO
{
	int nRows = game().rows(), nCols = game().cols();
	int r = hitPoint.r, c = hitPoint.c;
	if (r < 0)  // state 1
	{
		for (int i = 0; i < 200; i++)  // try random points
		{
			r = randInt(nRows); c = randInt(nCols);
			if (!hitMap[r][c])  // not hit before
				return Point(r, c);
		}

		for (r = 0; r < nRows; r++)
			for (c = 0; c < nCols; c++)
				if (!hitMap[r][c])
					return Point(r, c);
	}
	else  // state 2
	{
		int new_r, new_c;
		for (int i = 0; i < 160; ++i)  // 6.1287e-05
		{
			new_r = r; new_c = c;
			if (randInt(2) == 0)
				new_r = r - 4 + randInt(9);
			else
				new_c = c - 4 + randInt(9);
			if (new_r >= 0 && new_r < nRows && new_c >= 0 && new_c < nCols && !hitMap[new_r][new_c])
				return Point(new_r, new_c);
		}
	}

	hitPoint.r = -1;  // if the ship's length > 5, set state to 1
	return recommendAttack();
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	if (validShot)
	{
		hitMap[p.r][p.c] = true;             // record valid shot
		if (shotHit && shipDestroyed)
			hitPoint.r = -1;                 // reset to state 1
		else if (shotHit && hitPoint.r < 0)  // if hit a ship and it's in state 1
			hitPoint = p;
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
	bool placeShipsRecursively(Board& b, int placedNumShips, int totalNumShips, int maxRows, int maxCols);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
	void clearMap();
	// clear hitMap's block
	void blockMap(Point center, int shipLength, char shipSymbol, Direction d);
	// block areas around the placed ships
	void unblockMap(Point center, int shipLength, char shipSymbol, Direction d);
	// search good direction
	int searchDirection(Point center);

	GoodPlayer(const GoodPlayer&) = delete;
	GoodPlayer& operator=(const GoodPlayer&) = delete;

private:
	char** hitMap;
	int direction;            // 0 is state 1; 1, 2, 3, 4: NORTH, SOUTH, WEST, EAST
	int m_step;
	int stepLength;
	vector<int>  shipsOfOpponent;     // recored the length of opponent's ships, sinkedShips[i] == -1, means ship i is sinked.
	vector<Point> centerPoints;           // center points
	vector<Point> hitPoints;          // record hit points when it's in state 2
};


GoodPlayer::GoodPlayer(string nm, const Game& g)
	: Player(nm, g), direction(0), m_step(g.shipLength(0)), stepLength(1)
{
	m_step = 3;
	int nRows = g.rows(), nCols = g.cols();

	hitMap = new char*[nRows];
	for (int r = 0; r < nRows; ++r)
	{
		hitMap[r] = new char[nCols];
		for (int c = 0; c < nCols; ++c)
			hitMap[r][c] = '.';
	}
}

GoodPlayer::~GoodPlayer()
{
	for (int i = 0; i < game().rows(); i++)
		delete[] hitMap[i];
	delete[] hitMap;
}


bool GoodPlayer::placeShips(Board& b)
{
	const Game& m_game = game();

	for (int i = 0; i < m_game.nShips(); i++)  // initialize the steps and all ship length
		shipsOfOpponent.push_back(m_game.shipLength(i));

	for (int k = 0; k < 50; ++k)  // try 50 times to place ship
	{
		b.clear();
		if (placeShipsRecursively(b, 0, m_game.nShips(), m_game.rows(), m_game.cols()))
		{
			clearMap();
			return true;
		}
	}

	return false;
}

bool GoodPlayer::placeShipsRecursively(Board& b, int placedNumShips, int totalNumShips, int maxRows, int maxCols)
{
	if (placedNumShips == totalNumShips)
		return true;
	// try place ships
	for (int k = 0; k < 50; ++k)
	{
		int r = randInt(maxRows), c = randInt(maxCols);
		if (randInt(2) % 2 == 0)
		{
			if (hitMap[r][c] == '.' && b.placeShip(Point(r, c), placedNumShips, HORIZONTAL))
			{
				blockMap(Point(r, c), game().shipLength(placedNumShips), game().shipSymbol(placedNumShips), HORIZONTAL);
				if (placeShipsRecursively(b, placedNumShips + 1, totalNumShips, maxRows, maxCols))
					return true;
				unblockMap(Point(r, c), game().shipLength(placedNumShips), game().shipSymbol(placedNumShips), HORIZONTAL);
				b.unplaceShip(Point(r, c), placedNumShips, HORIZONTAL);
			}
		}
		else
		{
			if (hitMap[r][c] == '.' && b.placeShip(Point(r, c), placedNumShips, VERTICAL))
			{
				blockMap(Point(r, c), game().shipLength(placedNumShips), game().shipSymbol(placedNumShips), VERTICAL);
				if (placeShipsRecursively(b, placedNumShips + 1, totalNumShips, maxRows, maxCols))
					return true;
				unblockMap(Point(r, c), game().shipLength(placedNumShips), game().shipSymbol(placedNumShips), VERTICAL);
				b.unplaceShip(Point(r, c), placedNumShips, VERTICAL);
			}
		}
	}
	return false;
}

Point GoodPlayer::recommendAttack()
{
	const int nRows = game().rows(), nCols = game().cols();
	int r, c;
	if (direction == 0)  // if in state 1, randomly pick blocks by m_step
	{
		if (m_step < 1)
			m_step = 1;
		for (int i = 0; i < 200; i++)
		{
			r = randInt(nRows); c = randInt(nCols);
			if ((r + c) % m_step == 0 && hitMap[r][c] == '.')
				return Point(r, c);
		}
		m_step--;
		return recommendAttack();
	}
	else  // if in state 2
	{
		Point curr_center = centerPoints.back();
		switch (direction)
		{
		case 1: r = curr_center.r - stepLength;  // NORTH
			c = curr_center.c;
			break;
		case 2: r = curr_center.r + stepLength;  // SOUTH
			c = curr_center.c;
			break;
		case 3: r = curr_center.r;               // WEST
			c = curr_center.c - stepLength;
			break;
		case 4: r = curr_center.r;               // EAST
			c = curr_center.c + stepLength;
			break;
		}

		if (r < 0 || r >= nRows || c < 0 || c >= nCols || hitMap[r][c] != '.')  // if not a valid choice, switch direction
		{
			direction = searchDirection(centerPoints.back());
			stepLength = 1;
			if (direction == 0)  // if it's a false center point
			{
				centerPoints.pop_back();
				stepLength = 1;
				if (centerPoints.empty())
					direction = 0;
				else
					direction = 4;
			}

			return recommendAttack();
		}
		return Point(r, c);
	}

}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	if (validShot)
	{
		hitMap[p.r][p.c] = 'X';  // update hitMap and hitPoints
		if (shotHit)
			hitPoints.push_back(p);
		if (shotHit && shipDestroyed)  // switch to state 1?
		{
			stepLength = 1;  // switch step length to 1
			int length = game().shipLength(shipId);  // current ship length
			if (!centerPoints.empty())
				centerPoints.pop_back();
			// try to find the points that are not a part of the destroyed ship
			// the destroyed point must be the head or tail of the ship
			// since we won't let the ship that been hit but not destroyed
			for (int i = 0; i < hitPoints.size(); i++)
			{
				switch (direction)
				{
				case 1: if (hitPoints[i].c != p.c || hitPoints[i].r - p.r > length - 1)  // NORTH, find ship in direction of SOUTH
					centerPoints.push_back(hitPoints[i]);
					break;
				case 2: if (hitPoints[i].c != p.c || p.r - hitPoints[i].r > length - 1)
					centerPoints.push_back(hitPoints[i]);
					break;
				case 3: if (hitPoints[i].r != p.r || hitPoints[i].c - p.c > length - 1)  // WEST, find ship in direction of EAST
					centerPoints.push_back(hitPoints[i]);
					break;
				case 4: if (hitPoints[i].r != p.r || p.c - hitPoints[i].c > length - 1)
					centerPoints.push_back(hitPoints[i]);
					break;
				}
			}

			hitPoints.clear();
			if (centerPoints.empty())
				direction = 0;     // if no center points to hit, back to state 1
			else
			{
				direction = searchDirection(centerPoints.back());
				hitPoints.push_back(centerPoints.back());  // need also record the new center point
			}

			shipsOfOpponent[shipId] = -1;
			if (length == m_step)  // need update the minLength?
			{
				m_step = 0;
				for (int i = 0; i < shipsOfOpponent.size(); i++)
					if (shipsOfOpponent[i] != -1 && shipsOfOpponent[i] > m_step)
						m_step = shipsOfOpponent[i];
			}
		}
		else if (shotHit && direction == 0)  // if hit a ship and it's in state 1
		{
			centerPoints.clear();
			centerPoints.push_back(p);
			direction = searchDirection(p);
		}
		else if (!shotHit && direction > 0)
		{
			if (stepLength > 1)  // hit before, search in opposite direction
			{
				if (direction < 3)
					direction = 3 - direction;
				else
					direction = 7 - direction;
				stepLength = 1;
			}
		}
		else if (shotHit)
		{
			stepLength++;
		}
	}
}

void GoodPlayer::recordAttackByOpponent(Point p)
{}

void GoodPlayer::clearMap()
{
	int nRows = game().rows(), nCols = game().cols();
	for (int r = 0; r < nRows; ++r)
		for (int c = 0; c < nCols; ++c)
			hitMap[r][c] = '.';
}

inline void GoodPlayer::blockMap(Point center, int shipLength, char shipSymbol, Direction d)
{
	int nRows = game().rows(), nCols = game().cols();
	for (int r = center.r - shipLength; r <= center.r + shipLength; ++r)
		for (int c = center.c - shipLength; c <= center.c + shipLength; ++c)
			if (r >= 0 && r < nRows && c >= 0 && c < nCols && hitMap[r][c] == '.')
				hitMap[r][c] = shipSymbol;
	if (d == VERTICAL)
	{
		for (int r = 0; r < nRows; ++r)
			if (hitMap[r][center.c] == '.')
				hitMap[r][center.c] = shipSymbol;
	}
	else
	{
		for (int c = 0; c < nCols; ++c)
			if (hitMap[center.r][c] == '.')
				hitMap[center.r][c] = shipSymbol;
	}
}

inline void GoodPlayer::unblockMap(Point center, int shipLength, char shipSymbol, Direction d)
{
	int nRows = game().rows(), nCols = game().cols();
	for (int r = center.r - shipLength - 1; r <= center.r + shipLength + 1; ++r)
		for (int c = center.c - shipLength - 1; c <= center.c + shipLength + 1; ++c)
			if (r >= 0 && r < nRows && c >= 0 && c < nCols && hitMap[r][c] == shipSymbol)
				hitMap[r][c] = '.';
	if (d == VERTICAL)
	{
		for (int r = 0; r < nRows; ++r)
			if (hitMap[r][center.c] == shipSymbol)
				hitMap[r][center.c] = '.';
	}
	else
	{
		for (int c = 0; c < nCols; ++c)
			if (hitMap[center.r][c] == shipSymbol)
				hitMap[center.r][c] = '.';
	}
}

int GoodPlayer::searchDirection(Point center)
{
	int d = 0;
	int maxPath = 0, currPath = 0;
	int nRows = game().rows(), nCols = game().cols();
	// search the direction that has the longest path
	// NORTH

	for (int r = center.r - 1; r >= 0 && hitMap[r][center.c] == '.'; r--)
		currPath++;
	if (currPath > maxPath)
	{
		maxPath = currPath;
		d = 1;
	}
	currPath = 0;

	// SOUTH
	for (int r = center.r + 1; r < nRows && hitMap[r][center.c] == '.'; r++)
		currPath++;
	if (currPath > maxPath)
	{
		maxPath = currPath;
		d = 2;
	}
	currPath = 0;
	// WEST
	for (int c = center.c - 1; c >= 0 && hitMap[center.r][c] == '.'; c--)
		currPath++;
	if (currPath > maxPath)
	{
		maxPath = currPath;
		d = 3;
	}
	currPath = 0;
	// EAST
	for (int c = center.c + 1; c < nCols && hitMap[center.r][c] == '.'; c++)
		currPath++;
	if (currPath > maxPath)
	{
		maxPath = currPath;
		d = 4;
	}
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
