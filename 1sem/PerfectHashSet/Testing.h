#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <string>

#include "PerfectHashSet.h"
#include "Test.h"

typedef unsigned int ui32;

using HashSet::IHashTable;

class StdSet: public IHashTable
{
	std::unordered_set<ui32> good_keys, date;
	
	void CheckKey(const ui32 key)
	{
		if (good_keys.find(key) == good_keys.end())
		{	
			HashSet::UnknownKeyException exception(key);
			throw &exception;
		}
	}
	
	void EqualKeys(ui32 key)
	{
		HashSet::EqualKeysException exception(key);
		throw &exception;
	}
		
public:
	void Init(const vector<ui32> &keys)
	{
		for (ui32 i = 0; i < keys.size(); ++i)
		{
			if (good_keys.find(keys[i]) != good_keys.end())
				EqualKeys(keys[i]);
			good_keys.insert(keys[i]);
		}
	}
	
	bool IsPossibleKey(const ui32 key)
	{
		return good_keys.find(key) != good_keys.end();
	}
	
	bool Has(const ui32 key)
	{
		CheckKey(key);
		return date.find(key) != date.end();
	}
	
	void Insert(const ui32 key)
	{
		CheckKey(key);
		date.insert(key);
	}
	
	void Erase(const ui32 key)
	{
		CheckKey(key);
		date.erase(key);
	}
	
	ui32 Size() const
	{
		return date.size();
	}
	
	void Clear()
	{
		good_keys.clear();
		date.clear();
	}
	
	~StdSet()
	{
		Clear();
	}
};

void ErrorWith(const vector<ui32> &keys, std::string message)
{
	cerr << "Error with " << message << " in test:\n";
	for (ui32 i = 0; i < keys.size(); ++i)
		cerr << keys[i] << ' ';
	cerr << '\n';
	exit(0);
}

void InitTest(const vector<ui32> &keys, bool good, IHashTable* table)
{
	try
	{
		table->Init(keys);
		if (!good)
			ErrorWith(keys, "Init");
	} 
	catch (HashSet::EqualKeysException*)
	{
		if (good)
			ErrorWith(keys, "Init");
	}
	catch (...)
	{
		cerr << "WHAT?!\n";
	}
	table->Clear();
}

double TimeOfInitTest(const vector<ui32> &keys, bool good, IHashTable* table)
{
	clock_t t1 = clock();
	InitTest(keys, good, table);
	clock_t t2 = clock();
	
	return (1.0 * t2 - t1) / CLOCKS_PER_SEC;
}

void TestInitAllPermutations(const ui32 n, IHashTable* table)
{
	vector<ui32> keys = RandomSortedVector(n);
	do
	{
		InitTest(keys, 1, table);
	} while (std::next_permutation(keys.begin(), keys.end()));
}

double RandomUnequalKeysTest(IHashTable* table)
{
	const ui32 N = rand() % 250000 + 1;
	vector<ui32> keys = RandomVector(N);
	return TimeOfInitTest(keys, 1, table);
}

void UnequalKeysTest(IHashTable* table)
{
	for (ui32 i = 1; i < 10; ++i)
		TestInitAllPermutations(i, table);
		
	double time = 0;
	for (ui32 i = 0; i < 10; ++i)
		time += RandomUnequalKeysTest(table);
	cerr << time / 10 << '\n';
}

void EqualKeysTest(IHashTable* table)
{
	const ui32 N = rand() % 100000 + 1;
	vector<ui32> random_vector = RandomVector(N);
	
	// (h0, h0, h0, h0, ...)
	vector<ui32> keys(N, rand());
	TimeOfInitTest(keys, 0, table);
	
	// (h0, h1, h0, h1, ...)
	ui32 h = rand();
	for (ui32 i = 1; i < keys.size(); i += 2)
		keys[i] = h;
	TimeOfInitTest(keys, 0, table);
	
	// (h0, h0, h1, h1, h2, h2, ...) + shuffle()
	for (ui32 i = 1; i < N; i += 2)
		keys[i] = keys[i - 1] = random_vector[i / 2];
	std::random_shuffle(keys.begin(), keys.end());
	TimeOfInitTest(keys, 0, table);
	
	// (h0, h1, h2, ... h0, h1, h2, ...)
	for (ui32 i = 0; i < N / 2; ++i)
		keys[i] = keys[N / 2 + i] = random_vector[i];
	TimeOfInitTest(keys, 0, table);
	
	//(h0, h1, …,hn−2, h0)
	keys = random_vector;
	keys[N - 1] = keys[0];
	TimeOfInitTest(keys, 0, table);
}

void IsPossibleKeyTest(IHashTable* table)
{
	const ui32 N = rand() % 500000 + 1;
	vector<ui32> a = RandomVector(N * 2);
	vector<ui32> keys(a.begin(), a.begin() + N);
	
	table->Init(keys);
	for (ui32 i = 0; i < N; ++i)
		if (!table->IsPossibleKey(a[i]) || table->IsPossibleKey(a[N + i]))
		{
			ErrorWith(keys, "IsPossibleKey");
		}
}

void PrintHasAndSize(IHashTable* table, ui32 key)
{
	cerr << "Has: " << table->Has(key) << '\n';
	cerr << "Size: " << table->Size() << '\n';
}

void SmallTests(IHashTable* table)
{
	const ui32 N = rand() % 500000;
	vector<ui32> a = RandomVector(N + 1), keys(a.begin(), a.begin() + N);
	
	table->Init(keys);
	ui32 idx = rand() % N;
	
	cerr << "has, size, insert, has, size, erase, has, size\n";
	PrintHasAndSize(table, keys[idx]);
	table->Insert(keys[idx]);
	PrintHasAndSize(table, keys[idx]);
	table->Erase(keys[idx]);
	PrintHasAndSize(table, keys[idx]);
	cerr << '\n';
	
	cerr << "insert, insert, has, size, erase, has, size\n";
	table->Insert(keys[idx]);
	table->Insert(keys[idx]);
	PrintHasAndSize(table, keys[idx]);
	table->Erase(keys[idx]);
	PrintHasAndSize(table, keys[idx]);
	cerr << '\n';
	
	cerr << "insert bad key:\n";
	try
	{
		table->Insert(a[N]);
		cerr << "Error!\n";
	}
	catch (HashSet::UnknownKeyException *)
	{
	}
}

void Test(void (*TestFunc)(IHashTable*), int srand_const = 501)
{
	PerfectHashSet table;
	StdSet std_table;
	
	srand(srand_const);
	cerr << "PerfectHashSet:\n";
	TestFunc(&table);
	
	srand(srand_const);
	cerr << "\nstd::set:\n";	
	TestFunc(&std_table);
	
	cerr << '\n';
}

void CheckEqualOfOperation(IHashTable* table_1, IHashTable* table_2, ui32 key)
{
	if (table_1->Has(key) != table_2->Has(key) || table_1->Size() !=  table_2->Size())
	{
		cerr << "Fail in stress!!\n";
		exit(0);
	}
}

void Stress()
{
	const ui32 N = rand() % 100000 + 1;
	const ui32 M = rand() % 1000000 + 1;
	
	vector<bool> operations(M); 
	
	for (ui32 i = 0; i < M; ++i)
		operations[i] = rand() % 2; // 1 - insert, 0 - erase
	
	vector<ui32> keys = RandomVector(N);
	vector<ui32> test_keys(M);
	
	for (ui32 i = 0; i < M; ++i)
		test_keys[i] = keys[static_cast<ui32>(rand()) % N];
	
	PerfectHashSet table;
	StdSet std_table;
	
	table.Init(keys);
	std_table.Init(keys);
	
	for (ui32 i = 0; i < M; ++i)
	{
		if (operations[i])
		{
			table.Insert(test_keys[i]);
			std_table.Insert(test_keys[i]);
		}
		else
		{
			table.Erase(test_keys[i]);
			std_table.Erase(test_keys[i]);
		}
		CheckEqualOfOperation(&table, &std_table, test_keys[i]);
	}
	
	for (ui32 i = 0; i < N; ++i)
		CheckEqualOfOperation(&table, &std_table, keys[i]);
}

void Testing(int srand_const = 501)
{
	srand(srand_const);

	cerr << "1. test of Init\n";
	cerr << "a) Unequal keys:\n";
	Test(UnequalKeysTest, srand_const);
	
	cerr << "b) Equal keys\n";
	Test(EqualKeysTest, srand_const);

	cerr << "2. test of IsPossibleKey\n";
	Test(IsPossibleKeyTest, srand_const);
	
	cerr << "3. test of Insert, Has, Erase\n";
	
	cerr << "a) Small tests:\n";
	Test(SmallTests, srand_const);
	
	cerr << "b) Stress\n";
	Stress();
}
	
