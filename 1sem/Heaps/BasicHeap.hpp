#ifndef _BASIC_HEAP
#define _BASIC_HEAP

#include "Heap.hpp"
#include "HeapNodes.hpp"

namespace NBasicHeap
{
	template<class Node>
	Node* Meld(Node* first_heap, Node* second_heap)
	{
		if (!first_heap)
			return second_heap;
		if (!second_heap)
			return first_heap;
			
		if (first_heap->key > second_heap->key)
			std::swap(first_heap, second_heap);
		
		first_heap->right = Meld(first_heap->right, second_heap);
		
		first_heap->Balance();
		
		return first_heap;
	}
	
	template<class Node>
	class BasicHeap: public IMergeableHeap
	{
		Node *root;
		
		void Clear()
		{
			root = 0;
		}
		
	public:
		
		BasicHeap()
		:root(0)
		{
		}
		
		BasicHeap(const int key)
		{
			Node *vert = new Node(key);
			root = vert;
		}
		
		void Meld(IMergeableHeap* imheap)
		{
			BasicHeap *heap = Cast<BasicHeap>(imheap);
			
			if (!heap->root)
				return;
			
			root = NBasicHeap::Meld(root, heap->root);
			
			heap->Clear();
		}
		
		void Insert(const int key)
		{
			Node* vert = new Node(key);
			root = NBasicHeap::Meld(vert, root);
		}
		
		int ExtractMin()
		{
			if (!root)
				throw EmptyHeapException();
			
			int min = root->key;
			Node *right = root->right, *left = root->left;
			
			root->left = root->right = 0;
			delete root;
			
			root = NBasicHeap::Meld(right, left);
			return min;
		}
		
		bool Empty() const
		{
			return !root;
		}
		
		virtual ~BasicHeap()
		{
			delete root;
		}
	};
}

class LeftistHeap: public NBasicHeap::BasicHeap<LeftistHeapNode>
{
};

class SkewHeap: public NBasicHeap::BasicHeap<SkewHeapNode>
{
};

#endif
