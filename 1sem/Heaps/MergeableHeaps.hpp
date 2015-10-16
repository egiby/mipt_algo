#ifndef _MERGEABLE_HEAPS
#define _MERGEABLE_HEAPS

#include "BinomialHeap.hpp"
#include "BasicHeap.hpp"
#include "Heap.hpp"

#include <cassert>

enum HeapType
{
	BINOMIAL_HEAP,
	LEFTIST_HEAP,
	SKEW_HEAP
};

IMergeableHeap* Create(HeapType type_heap)
{
	if (type_heap == BINOMIAL_HEAP)
		return new BinomialHeap;
	
	if (type_heap == LEFTIST_HEAP)
		return new LeftistHeap;
	
	if (type_heap == SKEW_HEAP)
		return new SkewHeap;
	
	assert(0);
}

#endif
