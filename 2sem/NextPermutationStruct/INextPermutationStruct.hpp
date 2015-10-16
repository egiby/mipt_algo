#ifndef _INEXTPERMUTATIONSTRUCT
#define _INEXTPERMUTATIONSTRUCT

#include <iostream>

typedef size_t ui32;

class INextPermutatiomStruct
{
public:
	virtual void insert(ui32 index, int value) = 0; // index >= 0
	virtual void assign(ui32 index, int value) = 0;
	virtual void reverse(ui32 left, ui32 right) = 0;
	virtual long long subsegmentSum(ui32 left, ui32 right) = 0;
	virtual bool nextPermutation(ui32 left, ui32 right) = 0;
	virtual int getValue(ui32 index) = 0;
	virtual ui32 size() = 0;
	
	virtual ~INextPermutatiomStruct()
	{
	}
}; 

#endif
