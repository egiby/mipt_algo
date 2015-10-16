#include <vector>
#include <iostream>

namespace NALCS
{
	typedef unsigned int ui32;
	using std::vector;
	
	void recalcPair(ui32 first, ui32 second, ui32 &first_ans, ui32 &second_ans)
	{
		first_ans = std::max(first, second);
		second_ans = std::min(first, second);
	}
	
	template<class Array>
	vector<ui32> calcDynamic(const Array &a, const Array &b, ui32 a_size, ui32 b_size)
	{
		ui32 vertical_edge = 0;
		vector<ui32> horizontal_edges(a_size);
		
		for (ui32 i = 0; i < a_size; ++i)
			horizontal_edges[i] = i + 1;
		
		for (ui32 i = 0; i < b_size; ++i)
		{
			vertical_edge = 0;
			
			for (ui32 j = 0; j < a_size; ++j)
			{
				if (a[j] == b[i] && vertical_edge < horizontal_edges[j])
					recalcPair(horizontal_edges[j], vertical_edge, vertical_edge, horizontal_edges[j]);
				else
					recalcPair(horizontal_edges[j], vertical_edge, horizontal_edges[j], vertical_edge);
			}
		}
		
		return horizontal_edges;
	}
	
	template<class Array>
	vector<vector<ui32> > ALCS(const Array &a, const Array &b, ui32 a_size, ui32 b_size)
	{
		vector<ui32> number_unchanged = calcDynamic(a, b, a_size, b_size);
		
		vector<vector<ui32> > ans;
		
		for (ui32 i = 0; i < a_size; ++i)
		{
			ans.push_back(vector<ui32>(1));
			for (ui32 j = 0; j <= i; ++j)
			{
				if (i != j)
					ans[j].push_back(*(ans[j].end() - 1));

				if (number_unchanged[i] <= j)
					(*(ans[j].end() - 1))++;
			}
		}
		
		return ans;
	}
}
