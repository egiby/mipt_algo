#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <unordered_set>


using std::vector;
using std::unordered_set;

typedef unsigned int ui32;

vector<ui32> RandomVector(const ui32 n)
{
	unordered_set<ui32> s;
	vector<ui32> a;
	while (s.size() != n)
		s.insert(rand());
	a = vector<ui32>(s.begin(), s.end());
	return a;
}

vector<ui32> RandomSortedVector(const ui32 n)
{
	vector<ui32> a = RandomVector(n);
	sort(a.begin(), a.end());
	return a;
}
