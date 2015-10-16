#ifndef _STRESS
#define _STRESS

#include "MergeableHeaps.hpp"

#include <iostream>
#include <vector>

#include <ctime>
#include <cstdio>
#include <cstdlib>

typedef unsigned int ui32;

enum Event
{
	ADD_HEAP,
	INSERT,
	MELD,
	EXTRACT_MIN
};

void AddHeap(std::vector<IMergeableHeap*>&, HeapType);


std::vector<IMergeableHeap*> GenerateHeaps(HeapType heap_type)
{
	const ui32 N = 1000000;
	vector<IMergeableHeap*> heaps;
	for (ui32 i = 0; i < N; ++i)
	{
		AddHeap(heaps, heap_type);
	}
	return heaps;
}

void RecalculateVector(std::vector<IMergeableHeap*> &heaps, ui32 idx)
{
	delete heaps[idx];
	heaps[idx] = heaps.back();
	heaps.pop_back();
}

void AddHeap(std::vector<IMergeableHeap*> &heaps, HeapType heap_type)
{
	IMergeableHeap* heap = Create(heap_type);
	heap->Insert(rand());
	heaps.push_back(heap);
}

void Insert(std::vector<IMergeableHeap*> &heaps)
{
	ui32 idx = rand() % heaps.size();
	heaps[idx]->Insert(rand());
}

void Meld(std::vector<IMergeableHeap*> &heaps)
{
	ui32 idx1 = rand() % heaps.size(), idx2;
	do
	{
		idx2 = rand() % heaps.size();
	} 
	while (idx1 == idx2);
		
	heaps[idx1]->Meld(heaps[idx2]);
	
	RecalculateVector(heaps, idx2);
}

void ExtractMin(std::vector<IMergeableHeap*> &heaps)
{
	ui32 idx = rand() % heaps.size();
	//~ printf("%d\n", heaps[idx]->ExtractMin());
	heaps[idx]->ExtractMin();
	
	if (heaps[idx]->Empty())
		RecalculateVector(heaps, idx);
}

double Stress(HeapType heap_type, const int seed)
{
	std::vector<IMergeableHeap*> heaps;
	srand(seed);
	
	clock_t t_begin = clock();
	heaps = GenerateHeaps(heap_type);
	
	const ui32 M = 1000000;
	
	for (ui32 i = 0; i < M; ++i)
	{
		Event event;
		event = static_cast<Event>(rand() % 4);
		
		switch (event)
		{
			case ADD_HEAP:
				AddHeap(heaps, heap_type);
				break;
			case INSERT:
				Insert(heaps);
				break;
			case MELD:
				Meld(heaps);
				break;
			case EXTRACT_MIN:
				ExtractMin(heaps);
				break;
		}
			
		if (heaps.empty())
			break;
	}
	
	clock_t t_end = clock();
	
	for (auto heap: heaps)
		delete heap;
	
	return (1.0 * t_end - t_begin) / CLOCKS_PER_SEC;		
}

void Test(const int seed = 501)
{	
	//~ freopen("binomial_heap.out", "w", stdout);
	//~ cerr << Stress(BINOMIAL_HEAP, seed) << '\n';
	//~ fclose(stdout);
//~ 
	//~ freopen("leftist_heap.out", "w", stdout);
	//~ cerr << Stress(LEFTIST_HEAP, seed) << '\n';
	//~ fclose(stdout);
	//~ 
	//~ freopen("skew_heap.out", "w", stdout);
	//~ cerr << Stress(SKEW_HEAP, seed) << '\n';
	//~ fclose(stdout);
	Stress(BINOMIAL_HEAP, seed);
	Stress(LEFTIST_HEAP, seed);
	Stress(SKEW_HEAP, seed);
}

#endif
