#ifndef _HEAP
#define _HEAP

#include <exception>
#include <iostream>

class IMergeableHeap
{
public:
	virtual void Meld(IMergeableHeap* heap) = 0;
	virtual void Insert(const int key) = 0;
	virtual int ExtractMin() = 0;
	virtual bool Empty() const = 0;
	virtual ~IMergeableHeap()
	{
	}
};

class ErrorMeldException: public std::exception
{
public:
	const char* what() const throw()
	{
		return "Not equal types of heaps or link == 0!";
	}
};

class EmptyHeapException: public std::exception
{
	public:
	const char* what() const throw()
	{
		return "Heap is empty!";
	}
};

template<class Heap>
Heap* Cast(IMergeableHeap* heap)
{
	if (!heap || !dynamic_cast<Heap*>(heap))
		throw ErrorMeldException();
		
	return dynamic_cast<Heap*>(heap);
}

#endif
