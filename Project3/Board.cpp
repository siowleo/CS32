#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
public:
	BoardImpl(const Game& g);
	~BoardImpl();
	void clear();
	void block();
	void unblock();
	bool placeShip(Point topOrLeft, int shipId, Direction dir);
	bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
	void display(bool shotsOnly) const;
	bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
	bool allShipsDestroyed() const;

private:
	const Game& m_game;
	int* m_nShips;     
	int hit;   
	char** board;
};

// Function Implementation
///////////////////////////////////////////////////////////

BoardImpl::BoardImpl(const Game& g) : m_game(g), hit(0)
{
	board = new char*[m_game.rows()];
	for (int i = 0; i < m_game.rows(); i++)
	{
		board[i] = new char[m_game.cols()];
	}

	m_nShips = new int[m_game.nShips()];
}

BoardImpl::~BoardImpl()
{
	for (int i = 0; i < m_game.rows(); i++)
	{
		delete board[i];
	}
	delete[] m_nShips;
}

void BoardImpl::block()  
{
	// Block cells with 50% probability
	for (int r = 0; r < m_game.rows(); r++)
	{
		for (int c = 0; c < m_game.cols(); c++)
		{
			if (randInt(2) == 0)
			{
				board[r][c] = 'X';
			}
		}
	}
}

void BoardImpl::clear()  
{
	for (int j = 0; j < m_game.nShips(); j++)
	{
		m_nShips[j] = 0;
	}

	for (int i = 0; i < m_game.rows(); i++)
	{
		for (int j = 0; j < m_game.cols(); j++)
		{
			if (board[i][j] != '.')
			{
				board[i][j] = '.';
			}
		}
	}
}

void BoardImpl::unblock()  
{
	for (int r = 0; r < m_game.rows(); r++)
	{
		for (int c = 0; c < m_game.cols(); c++)
		{
			if (board[r][c] == 'X')
			{
				board[r][c] = '.';
			}
		}
	}
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)  
{
	if (shipId < 0 || shipId > m_game.nShips()) //error check to see if shipId is valid
	{
		return false;
	}
	if (topOrLeft.r < 0 || topOrLeft.r >= m_game.rows()) //error check to make sure that toporleft is within the appropriate rows
	{
		return false;
	}
	if (topOrLeft.c < 0 || topOrLeft.c >= m_game.cols()) //error check to make sure that toporleft is within the appropriate columns
	{ 
		return false;
	}
	if (m_nShips[shipId] > 0)
	{
		return false;
	}
	if (dir == HORIZONTAL)  
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			if (topOrLeft.c + i >= m_game.cols())
			{
				return false;
			}
			if (board[topOrLeft.r][topOrLeft.c + i] != '.')
			{
				return false;
			}
		}
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			board[topOrLeft.r][topOrLeft.c + i] = m_game.shipSymbol(shipId);
		}
	}
	else if (dir == VERTICAL)
	{
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			if (topOrLeft.r + i >= m_game.rows())
			{
				return false;
			}

			if (board[topOrLeft.r + i][topOrLeft.c] != '.')
			{
				return false;
			}
		}
		for (int i = 0; i < m_game.shipLength(shipId); i++)
		{
			board[topOrLeft.r + i][topOrLeft.c] = m_game.shipSymbol(shipId);
		}
	}
	else
	{
		return false;
	}

	m_nShips[shipId] += m_game.shipLength(shipId);
	hit += m_game.shipLength(shipId);

	return true;
	
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	if (shipId < 0 || shipId > m_game.nShips()) //error check to see that the shipid is valid
	{
		return false;
	}

	if (m_nShips[shipId] == 0 || topOrLeft.r < 0 || topOrLeft.r >= m_game.rows() || topOrLeft.c < 0 || topOrLeft.c >= m_game.cols())  //error check to make sure that toporleft is within the appropriate rows
	{
		return false;
	}

	if (dir == VERTICAL && topOrLeft.r + m_game.shipLength(shipId) - 1 < m_game.rows())
	{
		if (board[topOrLeft.r + m_game.shipLength(shipId) - 1][topOrLeft.c] == m_game.shipSymbol(shipId))
		{
			for (int i = 0; i < m_game.shipLength(shipId); i++)
			{
				board[topOrLeft.r + i][topOrLeft.c] = '.';
			}
			m_nShips[shipId] -= m_game.shipLength(shipId);
			hit -= m_game.shipLength(shipId);
			return true;
		}
	}
	else if (dir == HORIZONTAL && topOrLeft.c + m_game.shipLength(shipId) - 1 < m_game.cols()) 
	{
		if (board[topOrLeft.r][topOrLeft.c + m_game.shipLength(shipId) - 1] == m_game.shipSymbol(shipId))
		{
			for (int i = 0; i < m_game.shipLength(shipId); i++)
			{
				board[topOrLeft.r][topOrLeft.c + i] = '.';
			}
			m_nShips[shipId] = false;
			hit -= m_game.shipLength(shipId);
			return true;
		}
	}
	else
	{
		return false;
	}
}

void BoardImpl::display(bool shotsOnly) const  
{
	cout << "  ";
	cout << "0123456789" << endl;

	for (int r = 0; r < m_game.rows(); r++)
	{
		cout << r << ' ';
		for (int c = 0; c < m_game.cols(); c++)
		{
			if (shotsOnly)
			{
				if (board[r][c] != 'o' && board[r][c] != 'X') //display the unaffected elements
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
	if (p.r < 0 || p.r >= m_game.rows())
	{
		return false;
	}
	if (p.c < 0 || p.c >= m_game.cols())
	{
		return false;
	}
	if (board[p.r][p.c] == 'X' || board[p.r][p.c] == 'o')
	{
		return false;
	}
	if (board[p.r][p.c] == '.')
	{
		board[p.r][p.c] = 'o';
	}
	shotHit = false;
	for (int i = 0; i < m_game.nShips(); i++)
		{
			if (m_game.shipSymbol(i) == board[p.r][p.c]) 
			{
				shotHit = true;
				board[p.r][p.c] = '#';
				shipId = i;
				if (--m_nShips[i] <= 0)
				{
					shipDestroyed = true;
				}
				else
				{
					shipDestroyed = false;
				}
				hit--;
				return true;
			}
		}
	return true;
}

bool BoardImpl::allShipsDestroyed() const  
{
	return hit == 0;
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