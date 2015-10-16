#include <vector>
#include <algorithm>
#include <stack>
#include <iostream>

using std::vector;
using std::stack;
using std::reverse_iterator;

typedef unsigned int ui32;

enum EWhatMerge
{
	WM_NO_MERGE,
	WM_MERGE_XY,
	WM_MERGE_YZ
};

class TimSortParams
{
public:
	virtual ui32 MinRun(ui32 len) const = 0;
	virtual bool NeedMerge(ui32 lenX, ui32 lenY) const = 0;
	virtual ui32 GetGallop() const = 0;
	virtual EWhatMerge WhatNeedMerge (ui32 lenX, ui32 lenY, ui32 lenZ) const = 0;
};

class TimSortParamsDefault: public TimSortParams
{
public:
	ui32 MinRun(ui32 len) const
	{
		int minrun = len, parameter = 0;
		while (minrun >= (1 << 6))
		{
			parameter |= minrun & 1;
			minrun >>= 1;
		}
		minrun += parameter;
		return minrun;
	}
	
	bool NeedMerge(ui32 lenX, ui32 lenY) const
	{
		// Y is the top element of the stack
		return lenX <= lenY;
	}
	
	ui32 GetGallop() const
	{
		return 7;
	}
	
	EWhatMerge WhatNeedMerge(ui32 lenX, ui32 lenY, ui32 lenZ) const
	{
		// Z is the top element of the stack
		EWhatMerge res = WM_NO_MERGE;
		if (lenX <= lenY + lenZ && lenZ < lenX)
			res = WM_MERGE_YZ;
		else if (lenX <= lenY + lenZ)
			res = WM_MERGE_XY;
		else if (lenY <= lenZ)
			res = WM_MERGE_YZ;
		return res;
	}
};

template<class Iterator>
class Run
{
	Iterator start;
	ui32 len;

public:
    
	Run (Iterator start, ui32 len)
	:start(start), len(len)
	{
	}
	
	Iterator Begin() const
	{
		return start;
	}
	
	Iterator End() const
	{
		return start + len;
	}
	
	ui32 Size() const
	{
		return len;
	}
	
	void IncRun(ui32 n)
	{
		len += n;
	}
	
	~Run () 
	{
	}
};

template<class Iterator, class Compare>
bool NonDec(Iterator element, Compare comp)
{
	return !comp(*(element), *(element - 1));
}

template<class Iterator, class Compare>
void InsertionSort(Iterator left, Iterator right, Iterator start_sort, Compare comp)
{
	for (Iterator current = start_sort; current != right; ++current)
	{
		Iterator iter = current;
		while (iter != left && !NonDec(iter, comp))
		{
			std::swap(*iter, *(iter - 1));
			iter--;
		}
	}
}

template<class Iterator, class Compare>
void GetSequence(Run<Iterator> &run, Iterator end, bool flag, Compare comp)
{
	Iterator current = run.End() - 1;
	while (run.End() != end && (NonDec(run.End(), comp) == flag))
		run.IncRun(1);
	if (!flag)
		std::reverse(current, run.End());
}

template<class Iterator, class Compare>
Run<Iterator> GetRun(Iterator current, Iterator end, ui32 minrun, Compare comp)
{
	Run<Iterator> run(current, 1);
	Iterator start_sort = run.Begin();

	if (run.End() == end)
		return run;
	GetSequence(run, end, NonDec(run.End(), comp), comp);
	if (start_sort == run.Begin())
		start_sort = run.End();
	while (run.End() != end && run.Size() < minrun)
		run.IncRun(1);

	InsertionSort(run.Begin(), run.End(), start_sort, comp);

	return run;
}

template<class Iterator, class CurrentIterator, class Compare, class Type>
void GetGallop(Iterator &left, Iterator end, CurrentIterator &index, Type value, Compare comp)
{
	Iterator gallop_index = std::lower_bound(left, end, value, comp);
	index = std::copy(left, gallop_index, index);
	left = gallop_index;
}

template<class Iterator, class Compare>
void LeftMerge(Iterator left, Iterator middle, Iterator right, Compare comp, const ui32 gallop_const)
{
	vector<class std::iterator_traits<Iterator>::value_type> small_array(left, middle);
	Iterator iter_right = middle, current_iter = left;
	typename vector<class std::iterator_traits<Iterator>::value_type>::iterator iter_left = small_array.begin();

	ui32 gallop_left = 0, gallop_right = 0;
	while (iter_right != right && iter_left != small_array.end())
	{
		if (!comp(*iter_right, *iter_left))
		{
			++gallop_left;
			gallop_right = 0;
			*(current_iter++) = *(iter_left++);
		}
		else
		{
			gallop_left = 0;
			++gallop_right;
			*(current_iter++) = *(iter_right++);
		}
		if (iter_left != small_array.end() && gallop_left == gallop_const)
		{
			GetGallop(iter_left, small_array.end(), current_iter, *iter_right, comp);
			gallop_left = 0;
		}
		if (current_iter != right && gallop_right == gallop_const)
		{
			GetGallop(iter_right, right, current_iter, *iter_left, comp);
			gallop_right = 0;
		}
	}
	if (iter_right == right)
	{
		copy(iter_left, small_array.end(), current_iter);
	}
}

template<class Compare>
class ReverseCompare
{
	Compare comp;

public:
    ReverseCompare () {}
    
	ReverseCompare (Compare comp)
	:comp(comp)
	{
	}

	template<class Type>
	bool operator () (Type a, Type b)
	{
		return comp(b, a);
	}
	
	~ReverseCompare () {}
};

template<class Iterator, class Compare>
void Merge(Run<Iterator> &left_run, Run<Iterator> right_run, Compare comp, const TimSortParams *tim_sort_params)
{
	assert(left_run.End() == right_run.Begin());
	Iterator left = left_run.Begin(), middle = left_run.End(), right = right_run.End();

	if (middle - left < right - middle)
		LeftMerge(left, middle, right, comp, tim_sort_params->GetGallop());
	else
	{
		std::reverse_iterator<Iterator> reverse_left = std::reverse_iterator<Iterator>(right);
		std::reverse_iterator<Iterator> reverse_middle = std::reverse_iterator<Iterator>(middle);
		std::reverse_iterator<Iterator> reverse_right = std::reverse_iterator<Iterator>(left);
		ReverseCompare<Compare> reverse_comp(comp);
		LeftMerge(reverse_left, reverse_middle, reverse_right, reverse_comp, tim_sort_params->GetGallop());
	}
	//~ std::inplace_merge(left, middle, right, comp);

	// result in left_run
	left_run.IncRun(right_run.Size());
}

template<class Iterator>
Run<Iterator> Pop(stack<Run<Iterator> > &run_stack)
{
	Run<Iterator> run = run_stack.top();
	run_stack.pop();
	return run;
}

template<class Iterator, class Compare>
void GetMerge(stack<Run<Iterator> > &run_stack, Compare comp, const TimSortParams *tim_sort_params)
{
	while (run_stack.size() > 2)
	{
		Run<Iterator> runz = Pop(run_stack);
		Run<Iterator> runy = Pop(run_stack);
		Run<Iterator> runx = Pop(run_stack);

		EWhatMerge WhatMerge = tim_sort_params->WhatNeedMerge(runx.Size(), runy.Size(), runz.Size());

		if (WhatMerge == WM_MERGE_XY)
			Merge(runx, runy, comp, tim_sort_params);
		run_stack.push(runx);
		if (WhatMerge == WM_MERGE_YZ)
		{
			Merge(runy, runz, comp, tim_sort_params);
			run_stack.push(runy);
		}
		else if (WhatMerge == WM_MERGE_XY)
		{
			run_stack.push(runz);
		}
		else if (WhatMerge == WM_NO_MERGE)
		{
			run_stack.push(runy);
			run_stack.push(runz);
			break;
		}
		else
			assert(0);
	}
	
	if (run_stack.size() == 2)
	{
		Run<Iterator> runy = Pop(run_stack);
		Run<Iterator> runx = Pop(run_stack);
		bool flag = tim_sort_params->NeedMerge(runx.Size(), runy.Size());

		if (flag)
		{
			Merge(runx, runy, comp, tim_sort_params);
		}
		run_stack.push(runx);

		if (!flag)
		{
			run_stack.push(runy);
		}
	}
}

template<class Iterator, class Compare>
stack<Run<Iterator> > CreateRuns(Iterator begin, Iterator end, Compare comp, const TimSortParams *tim_sort_params)
{
	stack<Run<Iterator> > run_stack;
	ui32 minrun = tim_sort_params->MinRun(end - begin);
	Iterator current = begin;
	
	while (current != end)
	{
		Run<Iterator> run = GetRun(current, end, minrun, comp);
		current = run.End();

		run_stack.push(run);
		GetMerge(run_stack, comp, tim_sort_params);
	}
	return run_stack;
}

template<class Iterator, class Compare>
void FinalMerge(stack<Run<Iterator> > &run_stack, Compare comp, const TimSortParams *tim_sort_params)
{
	while (run_stack.size() > 1)
	{
		Run<Iterator> runy = Pop(run_stack);
		Run<Iterator> runx = Pop(run_stack);

		Merge(runx, runy, comp, tim_sort_params);

		run_stack.push(runx);
	}
}

template<class Iterator, class Compare>
void TimSort(Iterator begin, Iterator end, Compare comp, const TimSortParams *tim_sort_params)
{
	stack<Run<Iterator> > run_stack = CreateRuns(begin, end, comp, tim_sort_params);
	FinalMerge(run_stack, comp, tim_sort_params);
}

template<class Iterator, class Compare = std::less<typename std::iterator_traits<Iterator>::value_type> >
void TimSort(Iterator begin, Iterator end, Compare comp = Compare())
{
	TimSortParamsDefault settings;
	TimSort(begin, end, comp, &settings);
}
