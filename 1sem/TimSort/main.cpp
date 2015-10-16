#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

#include <cstdio>
#include <ctime>
#include <cassert>

#include "TimSort.h"
#include "Test.h"

using std::cout;
using std::vector;

typedef unsigned int ui32;

struct Time
{
	double tim_sort_time, std_sort_time;
	
	Time () 
	{
	}
};

template<typename Type, typename Compare>
double TimeOfSort(vector<Type> &array, void (*sort)(typename vector<Type>::iterator, typename vector<Type>::iterator, Compare), Compare comp)
{
	clock_t t = clock();
	sort(array.begin(), array.end(), comp);
	clock_t t2 = clock();

	return (double)(t2 - t) / CLOCKS_PER_SEC;
}

template<typename Type, typename Compare>
Time Test(const vector<Type> &array, void (*CorrectSort)(typename vector<Type>::iterator, typename vector<Type>::iterator, Compare), Compare comp)
{
    vector<Type> a(array), b(array);
    
	Time time;
	
	time.std_sort_time = TimeOfSort(a, CorrectSort, comp);
	time.tim_sort_time = TimeOfSort(b, TimSort, comp);

	assert(a == b);
	return time;
}

void PrintTime(Time time)
{
	cout.precision(3);
	cout << std::fixed << "TimSort - " << time.tim_sort_time << "s\n";
	cout << std::fixed << "Std::sort - " << time.std_sort_time << "s\n";
	cout << std::fixed << "tim/std - " << time.tim_sort_time / time.std_sort_time << '\n';
	cout << std::fixed << "std/tim - " << time.std_sort_time / time.tim_sort_time << '\n';
	cout << '\n';
}


template<class Type, class Compare = std::less<Type> >
void GetTest(const ui32 N, vector<Type> (*Gen)(ui32), void (*CorrectSort)(typename vector<Type>::iterator, typename vector<Type>::iterator, Compare), Compare comp = std::less<Type>())
{
	Time time;
	vector<Type> b;
	b = Gen(N);

	time = Test(b, CorrectSort, comp);
	cout << N << "\n";
	PrintTime(time);
}

int main()
{
	freopen("log", "w", stderr);

	const ui32 N_NUM = 10000000;
	const ui32 N_STR = 1000000;
	const ui32 N_SORT = 10000000;
	const ui32 N_POINT = 10000000;
	
	cout << "Random integer vector, n = ";
	GetTest<int>(N_NUM, RandomVector<int>, std::sort);
	
	cout << "Partially sorted string vector, n = ";
	GetTest<string>(N_STR, PartSortedVector<string>, std::sort);
	
	cout << "Sorted vector with some swaps, n = ";
	GetTest<int>(N_SORT, SubSortedVector<int>, std::sort);
	
	cout << "Point vector, stable test n = ";
	GetTest<Point>(N_POINT, RandomVector<Point>, std::stable_sort, Cmp);

	return 0;
}
