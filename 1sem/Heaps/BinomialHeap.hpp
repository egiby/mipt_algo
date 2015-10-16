#ifndef _BINOMIAL_HEAP
#define _BINOMIAL_HEAP

#include "Heap.hpp"

#include <iostream>
#include <vector>

#include <cassert>

typedef unsigned int ui32;

using std::vector;
using std::cerr;

namespace NBinomialHeap
{	
	struct BinomialTree
	{
		ui32 degree;
		BinomialTree *child, *right;
		
		int key;
		
		explicit BinomialTree(int key)
		:degree(0), child(0), right(0), key(key)
		{
		}
		
		~BinomialTree()
		{
			delete child;
			delete right;
		}
	};
	
	// first_heap->key < second_heap->key
	BinomialTree* GetMeld(BinomialTree* first_heap, BinomialTree* second_heap)
	{		
		second_heap->right = first_heap->child;
		first_heap->child = second_heap;
		first_heap->right = 0;
		first_heap->degree++;
		return first_heap;
	}
	
	BinomialTree* Meld(BinomialTree* first_heap, BinomialTree* second_heap)
	{
		if (!first_heap)
			return second_heap;
		if (!second_heap)
			return first_heap;
		
		assert(first_heap->degree == second_heap->degree);
		
		if (first_heap->key < second_heap->key)
			return GetMeld(first_heap, second_heap);
		return GetMeld(second_heap, first_heap);
	}
	
	IMergeableHeap* Create();
}

class BinomialHeap: public IMergeableHeap
{
	NBinomialHeap::BinomialTree* min_tree;
	vector<NBinomialHeap::BinomialTree*> trees;
	
	explicit BinomialHeap(NBinomialHeap::BinomialTree *tree)
	{
		trees.resize(tree->degree + 1);
		trees[tree->degree] = tree;
		tree->right = 0;
		min_tree = tree;
	}
	
	explicit BinomialHeap(const int key)
	{
		NBinomialHeap::BinomialTree* tree = new NBinomialHeap::BinomialTree(key);
		trees.push_back(tree);
		min_tree = trees[0];
	}
	
	void RecalculateMinTree()
	{
		min_tree = 0;
		for (ui32 i = 0; i < trees.size(); ++i)
			if (trees[i] && (!min_tree || trees[i]->key < min_tree->key))
				min_tree = trees[i];
	}
	
	void Clear()
	{
		min_tree = 0;
		trees.clear();
	}
	
public:
	
	BinomialHeap()
	:min_tree(0)
	{
	}
	
	void Meld(IMergeableHeap* imheap)
	{
		BinomialHeap* heap = Cast<BinomialHeap>(imheap);
		NBinomialHeap::BinomialTree* tree = 0;
		trees.resize(std::max(trees.size(), heap->trees.size()), 0);
		
		for (ui32 i = 0; i < heap->trees.size(); ++i)
		{
			if (!trees[i])
				std::swap(heap->trees[i], trees[i]);
			if (!trees[i])
				std::swap(tree, trees[i]);
			
			if (tree)
			{
				tree = NBinomialHeap::Meld(trees[i], tree);
				trees[i] = heap->trees[i];
			}
			else if (heap->trees[i])
			{
				tree = NBinomialHeap::Meld(heap->trees[i], trees[i]);
				trees[i] = 0;
			}
			
			assert(!tree || tree->degree == i + 1);
			assert(!trees[i] || trees[i]->degree == i);	
		}
		
		while (tree && tree->degree < trees.size())
		{
			if (!trees[tree->degree])
			{
				trees[tree->degree] = tree;
				tree = 0;
				break;
			}
			
			tree = NBinomialHeap::Meld(tree, trees[tree->degree]);
			trees[tree->degree - 1] = 0;
		}
		
		if (tree)
			trees.push_back(tree);
			
		RecalculateMinTree();
		
		heap->Clear();
	}
	
	void Insert(const int key)
	{
		BinomialHeap* heap = new BinomialHeap(key);
		Meld(heap);
		delete heap;
	}
	
	int ExtractMin()
	{
		if (!min_tree)
			throw EmptyHeapException();
			
		NBinomialHeap::BinomialTree *tree = min_tree->child;
		trees[min_tree->degree] = 0;
		int min_tree_key = min_tree->key;
		
		min_tree->right = min_tree->child = 0;
		delete min_tree;
		
		while (tree)
		{
			NBinomialHeap::BinomialTree *right_tree = tree->right;
			BinomialHeap *heap = new BinomialHeap(tree);
			Meld(heap);
			delete heap;
			tree = right_tree;
		}
		
		RecalculateMinTree();
		
		return min_tree_key;
	}
	
	bool Empty() const
	{
		return !min_tree;
	}
	
	~BinomialHeap() 
	{
		for (ui32 i = 0; i < trees.size(); ++i)
			delete trees[i];
	}
};

#endif
