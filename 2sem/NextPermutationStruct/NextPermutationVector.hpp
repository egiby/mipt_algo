#include "INextPermutationStruct.hpp"

#include <algorithm>
#include <vector>

class NextPermutationVector: public INextPermutatiomStruct
{
	std::vector<int> array; 
	
	std::vector<int>::iterator rightIndex(ui32  index)
	{
		return index < array.size() ? array.begin() + index : array.end();
		//~ return array.begin() + index;
	}
	
public:
	void insert(ui32 index, int value)
	{
		array.insert(rightIndex(index), value);
	}
	
	void assign(ui32 index, int value)
	{
		array[index] = value;
	}
	
	void reverse(ui32 left, ui32 right)
	{
		std::reverse(array.begin() + left, rightIndex(right));
	}
	
	long long subsegmentSum(ui32 left, ui32 right)
	{
		return std::accumulate(array.begin() + left, rightIndex(right), 0ll);
	}
	
	bool nextPermutation(ui32 left, ui32 right)
	{
		return std::next_permutation(array.begin() + left, rightIndex(right));
	}
	
	int getValue(ui32 index)
	{
		return array[index];
	}
	
	ui32 size()
	{
		return array.size();
	}
	
	~NextPermutationVector()
	{
	}
};
