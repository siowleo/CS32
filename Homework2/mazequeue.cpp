#include<iostream>
#include<queue>
using namespace std;

bool pathExists(char maze[][10], int sr, int sc, int er, int ec);

class Coord
{
public:
	Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
	int r() const { return m_r; }
	int c() const { return m_c; }
private:
	int m_r;
	int m_c;
};
//(Our convention is that (0,0) is the northwest (upper left) corner, 
//with south (down) being the increasing r direction and east (right) being the increasing c direction.)

bool pathExists(char maze[][10], int sr, int sc, int er, int ec)
{
	queue <Coord> queueStack;
	Coord a(sr, sc);
	Coord b(er, ec);
	queueStack.push(a);
	maze[sr][sc] = 'L';

	while (!queueStack.empty())
	{
		Coord c = queueStack.front();
		queueStack.pop();
		if (c.r() == b.r() && c.c() == b.c())
		{
			return true;
		}
		if (maze[c.r() - 1][c.c()] == '.')
		{
			Coord d(c.r() - 1, c.c());
			queueStack.push(d);
			maze[c.r() - 1][c.c()] = 'L';
		}
		if (maze[c.r()][c.c() + 1] == '.')
		{
			Coord d(c.r(), c.c() + 1);
			queueStack.push(d);
			maze[c.r()][c.c() + 1] = 'L';
		}
		if (maze[c.r() + 1][c.c()] == '.')
		{
			Coord d(c.r() + 1, c.c());
			queueStack.push(d);
			maze[c.r() + 1][c.c()] = 'L';
		}
		if (maze[c.r()][c.c() - 1] == '.')
		{
			Coord d(c.r(), c.c() - 1);
			queueStack.push(d);
			maze[c.r()][c.c() - 1] = 'L';
		}
	}

}

