#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
public:
	GameImpl(int nRows, int nCols);
	int rows() const;
	int cols() const;
	bool isValid(Point p) const;
	Point randomPoint() const;
	bool addShip(int length, char symbol, string name);
	int nShips() const;
	int shipLength(int shipId) const;
	char shipSymbol(int shipId) const;
	string shipName(int shipId) const;
	Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause, int round);


private:
	int m_nRows;
	int m_nCols;
	int m_length;
	int m_nShips;
	struct Ship
	{
		int length;
		string name;
		char symbol;

	};
	vector<Ship> ships;

};


void waitForEnter()
{
	cout << "Press enter to continue: ";
	cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
	m_nRows = nRows;
	m_nCols = nCols;
	m_nShips = 0;
}

int GameImpl::rows() const
{
	return m_nRows;
}

int GameImpl::cols() const
{
	return m_nCols;
}

bool GameImpl::isValid(Point p) const
{
	return p.r >= 0 && p.r < rows() && p.c >= 0 && p.c < cols();
}

Point GameImpl::randomPoint() const
{
	return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
	Ship temp;
	if (m_length > 0)
	{
		m_length = length;
	}
	temp.length = m_length;
	temp.symbol = symbol;
	temp.name = name;
	ships.push_back(temp);
	m_nShips++;

	return true;
}

int GameImpl::nShips() const
{
	return m_nShips;
}

int GameImpl::shipLength(int shipId) const
{
	return ships[shipId].length;
}

char GameImpl::shipSymbol(int shipId) const
{
	return ships[shipId].symbol;
}

string GameImpl::shipName(int shipId) const
{
	return ships[shipId].name;  // This compiles but may not be correct
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause = true, int round = 0)
{

	if (round == 0)
		if (!p1->placeShips(b1) || !p2->placeShips(b2))  // place ships
			return nullptr;
	// play game recursively, untill one of them wins
	if (b1.allShipsDestroyed())
	{
		if (p1->isHuman())  // if human lose, display winner's board
		{
			cout << "You lose, the winner's board:" << endl;
			b2.display(false);
		}
		return p2;
	}
	if (b2.allShipsDestroyed())
	{
		if (p2->isHuman())
		{
			cout << "You lose, the winner's board:" << endl;
			b1.display(false);
		}
		return p1;
	}

	// p1 attack p2
	cout << p1->name() << "'s turn. Board for "
		<< p2->name() << ": " << endl;
	b2.display(p1->isHuman());  // display p2's board
	bool shotHit, shipDestroyed;
	int shipId;

	Point p = p1->recommendAttack();
	bool validHit = b2.attack(p, shotHit, shipDestroyed, shipId);
	cout << p1->name();
	if (!validHit)
	{
		cout << " wasted a shot at " << '(' << p.r << ',' << p.c << ")." << endl;
	}
	else
	{
		cout << " attacked " << '(' << p.r << ',' << p.c << ')' << " and ";
		if (shotHit)
		{
			if (shipDestroyed)
				cout << "destroyed the " << shipName(shipId);
			else
				cout << "hit something";
		}
		else
		{
			cout << "missed";
		}
		cout << ", resulting in: " << endl;
		b2.display(p1->isHuman()); // display attack result
	}

	p1->recordAttackResult(p, validHit, shotHit, shipDestroyed, shipId);  // p1 records its attack result
	if (shouldPause)
		waitForEnter();
	return play(p2, p1, b2, b1, shouldPause, round + 1);
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
	if (nRows < 1 || nRows > MAXROWS)
	{
		cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
		exit(1);
	}
	if (nCols < 1 || nCols > MAXCOLS)
	{
		cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
		exit(1);
	}
	m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
	delete m_impl;
}

int Game::rows() const
{
	return m_impl->rows();
}

int Game::cols() const
{
	return m_impl->cols();
}

bool Game::isValid(Point p) const
{
	return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
	return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
	if (length < 1)
	{
		cout << "Bad ship length " << length << "; it must be >= 1" << endl;
		return false;
	}
	if (length > rows() && length > cols())
	{
		cout << "Bad ship length " << length << "; it won't fit on the board"
			<< endl;
		return false;
	}
	if (!isascii(symbol) || !isprint(symbol))
	{
		cout << "Unprintable character with decimal value " << symbol
			<< " must not be used as a ship symbol" << endl;
		return false;
	}
	if (symbol == 'X' || symbol == '.' || symbol == 'o')
	{
		cout << "Character " << symbol << " must not be used as a ship symbol"
			<< endl;
		return false;
	}
	int totalOfLengths = 0;
	for (int s = 0; s < nShips(); s++)
	{
		totalOfLengths += shipLength(s);
		if (shipSymbol(s) == symbol)
		{
			cout << "Ship symbol " << symbol
				<< " must not be used for more than one ship" << endl;
			return false;
		}
	}
	if (totalOfLengths + length > rows() * cols())
	{
		cout << "Board is too small to fit all ships" << endl;
		return false;
	}
	return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
	return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
	assert(shipId >= 0 && shipId < nShips());
	return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
	if (p1 == nullptr || p2 == nullptr || nShips() == 0)
		return nullptr;
	Board b1(*this);
	Board b2(*this);
	return m_impl->play(p1, p2, b1, b2, shouldPause);
}

