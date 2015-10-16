#include "slowALCS.hpp"
#include "ALCS.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <random>
#include <algorithm>

typedef unsigned int ui32;

using std::vector;
using std::string;
using std::cout;
using std::cerr;

ui32 makeRandom(ui32 left, ui32 right, std::default_random_engine &generator) // [l, r)
{
	std::uniform_int_distribution<ui32> number(left, right - 1);
	return number(generator);
}

bool testAllCombinations()
{
	const ui32 N = 3;
	
	vector<char> a, b;
	
	bool good = 1;
	
	ui32 p_a = 1;
	
	for (ui32 i = 1; i <= N; ++i)
	{
		p_a *= 10;
		
		ui32 p_b = 1;
		for (ui32 j = i; j <= N; ++j)
		{
			ui32 gen_b = 0;
			ui32 gen_a = 0;
			p_b *= 10;
			
			for (ui32 k = 0; k < p_a; ++k)
				for (ui32 q = 0; q < p_b; ++q)
				{
					string a = std::to_string(gen_a++), b = std::to_string(gen_b++);
					vector<vector<ui32> > d_fast = NALCS::ALCS(a, b, i, j), d_slow = NSlowALCS::slowALCS(a, b, i, j);
					good &= (d_fast == d_slow);
				}
		}
	}
	
	return good;
}

void getRandomVectors(ui32 a_size, ui32 b_size, ui32 left, ui32 right, vector<ui32> &a, vector<ui32> &b, std::default_random_engine &generator)
{
	a.resize(a_size);
	b.resize(b_size);
	
	for (auto& num: a)
		num = makeRandom(left, right, generator);

	for (auto& num: b)
		num = makeRandom(left, right, generator);
}

bool testRandomVectors(ui32 left, ui32 right, std::default_random_engine &generator)
{
	vector<ui32> a, b;
	
	getRandomVectors(makeRandom(1, 500, generator), makeRandom(1, 500, generator), left, right, a, b, generator);
	
	return NALCS::ALCS(a, b, a.size(), b.size()) == NSlowALCS::slowALCS(a, b, a.size(), b.size());
}

void testTime(ui32 length, std::default_random_engine &generator)
{
	vector<ui32> a, b;
	
	getRandomVectors(length, length, 1, 100, a, b, generator);
	
	clock_t t_start = clock();
	
	NALCS::ALCS(a, b, length, length);
	
	clock_t t_end = clock();
	
	fprintf(stderr, "%lf\n", (double)(t_end - t_start) / CLOCKS_PER_SEC);
}

bool test(int seed)
{
	std::default_random_engine generator(seed);
	
	bool good = 1;
	
	good &= testAllCombinations();
	good &= testRandomVectors(0, 500, generator);
	good &= testRandomVectors(0, 10, generator);
	good &= testRandomVectors(0, 2, generator);
	
	testTime(1000, generator);
	testTime(1500, generator);
	testTime(2000, generator);
	testTime(2500, generator);
	testTime(3000, generator);
	testTime(3500, generator);
	testTime(4000, generator);
	testTime(4500, generator);
	testTime(5000, generator);
	testTime(5500, generator);
	
	return good;
}

int main()
{
	freopen("log", "w", stderr);
	for (ui32 i = 0; i < 4; ++i)
	{
		if (test(time(NULL) + i))
			cout << "test " << i << " OK\n";
		else
			cout << "test " << i << " FAILED\n";
	}
	
	return 0;
}
