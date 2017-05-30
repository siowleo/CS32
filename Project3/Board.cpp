
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
public:
	BoardImpl(const Game& g);
	void clear();
	void block();
	void unblock();
	bool placeShip(Point topOrLeft, int shipId, Direction dir);
	bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
	void display(bool shotsOnly) const;
	bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
	bool allShipsDestroyed() const;

private:
	int m_rows;
	int m_cols;
	int m_nShips;
	int m_nDestroyedShips;
	char board[10][10];
	const Game& m_game;
	struct Ship
	{
		int length;
		string name;
		char symbol;
		int hit;

	};
	vector<Ship> ships;
	vector<Point> points;
};


BoardImpl::BoardImpl(const Game& g)
	: m_game(g)
{
	m_rows = m_game.rows();
	m_cols = m_game.cols();
	m_nShips = m_game.nShips();
	m_nDestroyedShips = 0;
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_cols; j++)
		{
			board[i][j] = '.';
		}
	}
}

void BoardImpl::clear()
{
	for (int r = 0; r < m_game.rows(); r++)
		for (int c = 0; c < m_game.cols(); c++)
		{
			if (board[r][c] != '.')
			{
				board[r][c] == '.';
			}
		}
}

void BoardImpl::block()
{
	// Block cells with 50% probability
	for (int r = 0; r < m_game.rows(); r++)
		for (int c = 0; c < m_game.cols(); c++)
			if (randInt(2) == 0)
			{
				board[r][c] == '#';
				Point blocked(r, c);
				points.push_back(blocked);
			}
}

void BoardImpl::unblock()
{
	for (int r = 0; r < m_game.rows(); r++)
		for (int c = 0; c < m_game.cols(); c++)
		{
			if (board[r][c] == '#')
			{
				board[r][c] = '.';
			}
		}
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	if (shipId < 0 || shipId > m_nShips)
	{
		return false;
	}
	if (topOrLeft.r < 0 || topOrLeft.r >= m_rows)
	{
		return false;
	}
	if (topOrLeft.c < 0 || topOrLeft.c >= m_cols)
	{
		return false;
	}
	if (dir == HORIZONTAL)
	{
		if (m_game.shipLength(shipId) + topOrLeft.c >= m_cols) //checks if placement of ship exceeds height of board
		{
			return false;
		}
		for (int k = topOrLeft.c; k < topOrLeft.c + m_game.shipLength(shipId); k++) //checks to see if ship is valid
		{
			if (board[topOrLeft.r][k] != '.')
			{
				return false;
			}
		}
		Ship tempShip; //temporary ship
		tempShip.name = m_game.shipName(shipId);
		tempShip.symbol = m_game.shipSymbol(shipId);
		tempShip.length = m_game.shipLength(shipId);
		if (!ships.empty()) //if vector is not empty
		{
			for (int k = 0; k < ships.size(); k++)
			{
				if (tempShip.symbol == ships[k].symbol) //checks ship is already used
					return false;
			}
		}
		tempShip.hit = 0;
		ships.push_back(tempShip); //pushes ship onto vector if not used yet
		for (int k = topOrLeft.c; k < topOrLeft.c + m_game.shipLength(shipId); k++)
		{
			board[topOrLeft.r][k] = m_game.shipSymbol(shipId); //places ship onto board
		}
		return true;
	}
	if (dir == VERTICAL)
	{
		if (m_game.shipLength(shipId) + topOrLeft.r >= m_rows)
			return false;
		for (int k = topOrLeft.r; k < topOrLeft.r + m_game.shipLength(shipId); k++)
		{
			if (board[k][topOrLeft.c] != '.')
				return false;
		}
		Ship tempShip; //temporary ship
		tempShip.name = m_game.shipName(shipId);
		tempShip.symbol = m_game.shipSymbol(shipId);
		tempShip.length = m_game.shipLength(shipId);
		if (!ships.empty())
		{
			for (int k = 0; k < ships.size(); k++)
			{
				if (tempShip.symbol == ships[k].symbol)
					return false;
			}
		}
		tempShip.hit = 0;
		for (int k = topOrLeft.r; k < topOrLeft.r + m_game.shipLength(shipId); k++)
		{
			board[k][topOrLeft.c] = m_game.shipSymbol(shipId);
		}
		ships.push_back(tempShip);
		return true;
	}
	return false;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	if (shipId < 0 || shipId > m_nShips)
	{
		return false;
	}
	if (topOrLeft.r < 0 || topOrLeft.r >= m_rows)
	{
		return false;
	}
	if (topOrLeft.c < 0 || topOrLeft.c >= m_cols)
	{
		return false;
	}
	if (dir == HORIZONTAL)
	{
		if (m_game.shipLength(shipId) + topOrLeft.c >= m_cols) //checks if placement of ship exceeds height of board
		{
			return false;
		}
		for (int k = topOrLeft.c; k < topOrLeft.c + m_game.shipLength(shipId); k++)
		{
			if (board[topOrLeft.r][k] == '.')
				return false;
		}
		for (int k = topOrLeft.c; k < topOrLeft.c + m_game.shipLength(shipId); k++)
		{
			board[topOrLeft.r][k] = '.';
		}
		vector<Ship>::iterator p = ships.begin();
		ships.erase(p + shipId);
		return true;
	}
	if (dir == VERTICAL)
	{
		if (m_game.shipLength(shipId) + topOrLeft.r >= m_rows) //checks if placement of ship exceeds height of board
		{
			return false;
		}
		for (int k = topOrLeft.r; k < topOrLeft.r + m_game.shipLength(shipId); k++)
		{
			if (board[k][topOrLeft.c] == '.')
				return false;
		}
		for (int k = topOrLeft.r; k < topOrLeft.r + m_game.shipLength(shipId); k++)
		{
			board[k][topOrLeft.c] = '.';
		}
		vector<Ship>::iterator p;
		p = ships.begin();
		ships.erase(p + shipId);
		return true;
	}
	return false;
}

void BoardImpl::display(bool shotsOnly) const
{
	cout << "  ";
	for (int c = 0; c < m_game.cols(); c++)
		cout << c;

	cout << endl;

	for (int r = 0; r < m_game.rows(); r++)
	{
		cout << r << ' ';
		for (int c = 0; c < m_game.cols(); c++)
		{
			if (shotsOnly)
			{
				if (board[r][c] != 'X' && board[r][c] != 'o')
				{
					cout << '.';
				}
			}
				else
				{
					cout << board[r][c];
				}
		}
		cout << endl;
	}
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	if (p.r < 0 || p.r >= m_rows)
	{
		return false;
	}
	if (p.c < 0 || p.c >= m_cols)
	{
		return false;
	}
	if (board[p.r][p.c] == 'X' || board[p.r][p.c] == 'o')
	{
		return false;
	}
	for (int i = 0; i < m_nShips; i++)
	{
		if (board[p.r][p.c] == ships[i].symbol)
		{
			shotHit = true;
			ships[i].hit++;
			board[p.r][p.c] == 'X';
			if (ships[i].hit == ships[i].length)
			{
				vector<Ship>::iterator p = ships.begin();
				ships.erase(p + shipId);
				shipDestroyed = true;
				m_nDestroyedShips++;
			}
			return true;
		}
	}
	shotHit = false;
	board[p.r][p.c] = 'o';
	return true;
}

bool BoardImpl::allShipsDestroyed() const
{
	if (m_nDestroyedShips == m_nShips)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
	m_impl = new BoardImpl(g);
}

Board::~Board()
{
	delete m_impl;
}

void Board::clear()
{
	m_impl->clear();
}

void Board::block()
{
	return m_impl->block();
}

void Board::unblock()
{
	return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
	m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
	return m_impl->allShipsDestroyed();
}



