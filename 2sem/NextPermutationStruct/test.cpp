#include "Test.hpp"

#include <iostream>
#include <string>
#include <random>

#include <cstdio>
#include <ctime>

template<class Type>
void allPermutations(ui32 array_size)
{
	INextPermutatiomStruct * array = create<Type>();
	
	for (ui32 i = 0; i < array_size; ++i)
		array->insert(i, i);
	
	printStruct(array);
	
	while (array->nextPermutation(0, array_size))
	{
		printStruct(array);
	}
	
	delete array;
}

template<class Type>
void moreThanAllPermutations()
{
	INextPermutatiomStruct * array = create<Type>();
	
	for (int i = 0; i < 4; ++i)
		array->insert(i, i);
		
	printStruct(array);
	
	for (int i = 0; i < 1000; ++i)
	{
		array->nextPermutation(0, 4);
		printStruct(array);
	}
	
	delete array;
}

template<class Type>
void testing(const char * filename, ui32 small_size, RandomTest &test)
{ 
	freopen(filename, "w", stdout);
	
	allPermutations<Type>(small_size);
	std::cout << std::endl;
	
	moreThanAllPermutations<Type>();
	std::cout << std::endl;
	
	test.getTest<Type>();
	
	fclose(stdout);
}

void timeTesting(const char * filename, ui32 array_size, ui32 number_events, int left, int right) // only treap!
{
	freopen(filename, "w", stdout);
	
	RandomTest test(array_size, number_events, left, right);
	
	clock_t t_start = clock();
	
	test.getTest<NextPermutationTreap>();
	
	clock_t t_end = clock();
	
	std::cerr << std::fixed << (double)(t_end - t_start) / CLOCKS_PER_SEC << std::endl;
	
	fclose(stdout);
}
	

int main()
{
	RandomTest test(10, 17, -10, 10);
	
	testing<NextPermutationVector>("vector.txt", 3, test);
	testing<NextPermutationTreap>("treap.txt", 3, test);
	
	system("diff vector.txt treap.txt >log");
	
	timeTesting("time.txt", 100000, 200000, INT_MIN, INT_MAX);
}
