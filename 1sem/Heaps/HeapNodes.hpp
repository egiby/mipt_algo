#ifndef _HEAP_NODES
#define _HEAP_NODES

#include <iostream>
#include <cassert>

typedef unsigned int ui32;

struct LeftistHeapNode
{
private:
	static ui32 Rank(LeftistHeapNode* const vert)
	{
		if (!vert)
			return 0;
		return vert->rank;
	}
	
	ui32 rank;
	
public:
	int key;
	LeftistHeapNode *left, *right;
	
	explicit LeftistHeapNode(const int key)
	:rank(0), key(key), left(0), right(0)
	{
	}
	
	void Balance()
	{
		if (LeftistHeapNode::Rank(right) > LeftistHeapNode::Rank(left))
			std::swap(right, left);
		rank = LeftistHeapNode::Rank(right) + 1;
	}
	
	~LeftistHeapNode()
	{
		delete left;
		delete right;
	}
};

struct SkewHeapNode
{
	int key;
	SkewHeapNode *left, *right;
	
	explicit SkewHeapNode(const int key)
	:key(key), left(0), right(0)
	{
	}
	
	void Balance()
	{
		std::swap(left, right);
	}
	
	~SkewHeapNode()
	{
		delete left;
		delete right;
	}
};

#endif
