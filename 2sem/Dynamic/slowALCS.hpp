#include <vector>
#include <iostream>

namespace NSlowALCS
{
	typedef unsigned int ui32;
	
	using std::max;
	using std::vector;
	using std::swap;
	
	template<class ArrayIterator>
	vector<ui32> calcLCS(ArrayIterator a, ArrayIterator b, ui32 a_size, ui32 b_size)
	{
		vector<vector<ui32> > d(2, vector<ui32>(b_size));
		
		d[0][0] = (a[0] == b[0]);
		
		for (ui32 j = 1; j < b_size; ++j)
			d[0][j] = max(d[0][j - 1], ui32(a[0] == b[j]));
		
		for (ui32 i = 1; i < a_size; ++i)
		{
			d[1][0] = max(d[0][0], (ui32)(a[i] == b[0]));
			
			for (ui32 j = 1; j < b_size; ++j)
				d[1][j] = max(d[0][j - 1] + (ui32)(a[i] == b[j]), max(d[1][j - 1], d[0][j]));
			
			swap(d[0], d[1]);
		}
		
		return d[0];
	}
	
	template<class Array>
	vector<vector<ui32> > slowALCS(const Array &a, const Array &b, ui32 a_size, ui32 b_size)
	{
		vector<vector<ui32> > ans;
		
		for (ui32 i = 0; i < a_size; ++i)
			ans.push_back(calcLCS(b.begin(), a.begin() + i, b_size, a_size - i));
		
		return ans;
	}
}
