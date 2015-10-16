#include "Test.hpp"

#include <iostream>

int main()
{
	freopen("log", "w", stderr);
	
	for (ui32 i = 0; i < 10; ++i)
	{
		cerr << "Test " << i << '\n';
		Test(time(0));
		cerr << '\n';
	}
	
	//~ while (1);
	
	return 0;
}
