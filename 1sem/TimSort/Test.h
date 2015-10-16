#include <ctime>
#include <algorithm>
#include <vector>
#include <string>

using std::vector;
using std::string;
using std::generate_n;

typedef unsigned int ui32;

class Point
{
public:
	int x, y;
	
	Point () {}
	
	Point (int x, int y)
	:x(x), y(y)
	{	
	}
};

bool operator == (Point p1, Point p2)
{
	return p1.x == p2.x && p1.y == p2.y;
}

bool Cmp(const Point p1, const Point p2)
{
	return p1.x < p2.x;
}

template<class Type>
Type Random()
{
	return static_cast<Type>(rand());
}

template<>
string Random()
{
	string s;
	const ui32 LEN = rand() % 500 + 1;
	for (ui32 i = 0; i < LEN; ++i)
		s.push_back(char(rand() % 26) + 'a');
	return s;
}

template<>
Point Random()
{
	const ui32 MAX_X = 50;
	return Point(rand() % MAX_X, rand());
}

void RandInit()
{
	const int SRAND_CONST = 501;
	srand(SRAND_CONST);
}

template<class Type>
vector<Type> RandomVector(const ui32 n)
{
	RandInit();
	vector<Type> a(n);
	generate_n(a.begin(), n, &Random<Type>);
	return a;
}

template<class Type>
vector<Type> SubSortedVector(const ui32 n)
{
	const ui32 NUMBER_RAND = 1000;
	vector<Type> a = RandomVector<Type>(n);
	sort(a.begin(), a.end());
	for (int i = 0; i < NUMBER_RAND; ++i)
	{
		int v = rand() % n, u = rand() % n;
		std::swap(a[v], a[u]);
	}
	return a;
}

template<class Type>
void SortPartOfVector(vector<Type> &a, const ui32 number_vector)
{
	ui32 len = a.size();
	vector<ui32> lengths;
	for (int i = 0; i + 2 < number_vector; ++i)
	{
		if (len == 0)
			lengths.push_back(0);
		else
		{
			ui32 d = rand() % len + 1;
			lengths.push_back(d);
			len -= d;
		}
	}
	lengths.push_back(len);
	typename vector<Type>::iterator iter = a.begin();
	for (ui32 i = 0; i < lengths.size(); ++i)
	{
		sort(iter, iter + lengths[i]);
		iter += lengths[i];
	}
}

template<class Type>
vector<Type> PartSortedVector(const ui32 n)
{
	const ui32 NUM_VECT = 1000;
	vector<Type> a = RandomVector<Type>(n);
	SortPartOfVector<Type>(a, NUM_VECT);
	return a;
}
