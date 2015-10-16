#include <iostream>

#include "Testing.h"

typedef unsigned int ui32;

using std::cerr;

int main()
{
	srand(501);
	rand();
	
	freopen("log", "w", stderr);
	
	for (ui32 i = 0; i < 10; ++i)
	{
		cerr << "Testing " << i << '\n';
		Testing(rand());	
		cerr << '\n';
	}
	
	return 0;
}
