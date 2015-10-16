#include "INextPermutationStruct.hpp"

#include <iostream>
#include <random>

#include <cstdlib>
#include <climits>
#include <cassert>
#include <ctime>

class NextPermutationTreap: public INextPermutatiomStruct
{		
	struct Node
	{
		Node *left, *right;
		int priority, value, min_value, max_value;
		ui32 size;
		long long subsegment_sum;
		bool reverse_flag, is_increasing, is_decreasing;
	
		Node ()
		{
		}
		
		Node (int value, std::default_random_engine &generator, std::uniform_int_distribution<int> Priority)
		:left(0), right(0), priority(Priority(generator)), value(value), min_value(value), max_value(value), size(1), subsegment_sum(value), reverse_flag(0), is_increasing(1), is_decreasing(1)
		{
		}
		
		~Node ()
		{			
			delete left;
			delete right;
		}
	};
	
	ui32 getSize(Node * tree)
	{
		if (!tree)
			return 0;
		return tree->size;
	}
		
	int getMin(Node * tree)
	{
		if (!tree)
			return INT_MAX;
		return tree->min_value;
	}
	
	int getMax(Node * tree)
	{
		if (!tree)
			return INT_MIN;
		return tree->max_value;
	}
	
	long long getSum(Node * tree)
	{
		if (!tree)
			return 0LL;
		return tree->subsegment_sum;
	}
	
	void xorFlag(Node * tree)
	{
		if (!tree)
			return;
		tree->reverse_flag ^= 1;
	}
	
	void push(Node * tree)
	{
		if (!tree)
			return;
			
		if (!tree->reverse_flag)
			return;
		
		std::swap(tree->left, tree->right);
		xorFlag(tree->left);
		xorFlag(tree->right);
		
		tree->reverse_flag = 0;
		std::swap(tree->is_decreasing, tree->is_increasing);
	}
	
	bool isIncrease(Node * tree)
	{
		if (!tree)
			return 1;
		
		push(tree);
		
		return tree->is_increasing;
	}
	
	bool isDecrease(Node * tree)
	{
		if (!tree)
			return 1;
		
		push(tree);
		
		return tree->is_decreasing;
	}
	
	void recalculate(Node * tree)
	{
		if (!tree)
			return;
		
		assert(tree->reverse_flag != 1);
		push(tree->left);
		push(tree->right);
		
		tree->size = getSize(tree->left) + 1 + getSize(tree->right);
		tree->subsegment_sum = getSum(tree->left) + tree->value + getSum(tree->right);
		tree->min_value = std::min(getMin(tree->left), std::min(tree->value, getMin(tree->right)));
		tree->max_value = std::max(getMax(tree->left), std::max(tree->value, getMax(tree->right)));
		
		tree->is_increasing = 0;
		tree->is_decreasing = 0;
		
		if (isIncrease(tree->left) && isIncrease(tree->right) && getMin(tree->right) >= getMax(tree->left) && tree->value >= getMax(tree->left) && tree->value <= getMin(tree->right))
			tree->is_increasing = 1;
		if (isDecrease(tree->left) && isDecrease(tree->right) && getMax(tree->right) <= getMin(tree->left) && tree->value <= getMin(tree->left) && tree->value >= getMax(tree->right))
			tree->is_decreasing = 1; 
	}
	
	void split(ui32 index, Node * tree, Node * &left, Node * &right)
	{
		if (!tree)
		{
			left = right = 0;
			return;
		}
		
		push(tree);
		
		if (getSize(tree->left) >= index)
		{
			right = tree;
			split(index, tree->left, left, right->left);
			recalculate(right);
		}
		else
		{
			left = tree;
			split(index - getSize(tree->left) - 1, tree->right, left->right, right);
			recalculate(left);
		}
		
		assert((!left || left->reverse_flag != 1) && (!right || right->reverse_flag != 1));
	}
	
	Node * merge(Node * left, Node * right)
	{
		if (!left)
			return right;
		if (!right)
			return left;
		
		Node * tree;
		
		push(left);
		push(right);
		
		if (left->priority > right->priority)
		{
			tree = left;
			tree->right = merge(left->right, right);
		}
		else
		{
			tree = right;
			tree->left = merge(left, right->left);
		}
		
		recalculate(tree);
		return tree;
	}
	
	void doReverse(Node * tree)
	{
		if (!tree)
			return;
		
		push(tree);
		tree->reverse_flag = 1;
	}
	
	int findDecreasing(Node * tree, int last)
	{
		push(tree);
		
		if (!tree)
			return 0; // The last permutation
		
		if (!isDecrease(tree->right))
			return getSize(tree->left) + 1 + findDecreasing(tree->right, last);
			
		if (getMin(tree->right) < last)
			return getSize(tree->left) + 1 + findDecreasing(tree->right, last);
		
		if (tree->value < getMax(tree->right) || (!tree->right && tree->value < last))
		{
			return getSize(tree->left);
		}
		
		return findDecreasing(tree->left, tree->value);
	}
	
	int findNext(Node * tree, int value)
	{
		assert(tree);
		push(tree);
			
		if (getMax(tree->right) <= value && tree->value <= value)
			return findNext(tree->left, value);
		else if (getMax(tree->right) <= value)
			return getSize(tree->left);
		else
			return getSize(tree->left) + 1 + findNext(tree->right, value);
	}
	
	void cutTree(Node * tree, ui32 left, ui32 right, Node * &left_tree, Node * &middle_tree, Node * &right_tree) // [l, r)
	{
		split(left, tree, left_tree, right_tree);
		split(right - left, right_tree, middle_tree, right_tree);
	}
	
	void reconstructTree(Node * &tree, Node * left, Node * middle, Node * right)
	{
		tree = merge(left, middle);
		tree = merge(tree, right);
	}
	
	Node * tree;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> Priority;
	
public:
	
	NextPermutationTreap()
	:tree(0)
	{
		generator = std::default_random_engine(time(NULL));
		//~ generator = std::default_random_engine(501);
		Priority = std::uniform_int_distribution<int>(INT_MIN, INT_MAX);
	}
	
	void insert(ui32 index, int value) // index >= 0
	{
		Node *left, *right, *middle = new Node(value, generator, Priority);
		split(index, tree, left, right);
		reconstructTree(tree, left, middle, right);
	}
	
	void assign(ui32 index, int value)
	{
		Node *left, *right, *middle;
		cutTree(tree, index, index + 1, left, middle, right);
		
		middle->value = value;
		
		reconstructTree(tree, left, middle, right);
	}
	
	void reverse(ui32 left, ui32 right) // [l, r);
	{
		Node *left_tree, *right_tree, *middle_tree;
		
		cutTree(tree, left, right, left_tree, middle_tree, right_tree);
		
		doReverse(middle_tree);
		
		reconstructTree(tree, left_tree, middle_tree, right_tree);
	}
	
	ui32 size()
	{
		return getSize(tree);
	}
	
	long long subsegmentSum(ui32 left, ui32 right) // [l, r)
	{
		Node *left_tree, *middle_tree, *right_tree;
		cutTree(tree, left, right, left_tree, middle_tree, right_tree);
		
		long long ans = getSum(middle_tree);
		
		reconstructTree(tree, left_tree, middle_tree, right_tree);
		return ans;
	}
	
	bool nextPermutation(ui32 left, ui32 right)
	{
		bool ans = 0;
		Node *left_tree(0), *middle_tree(0), *right_tree(0);
		cutTree(tree, left, right, left_tree, middle_tree, right_tree);
		
		if (!middle_tree || getSize(middle_tree) == 1)
		{
			reconstructTree(tree, left_tree, middle_tree, right_tree);
			return 0;
		}
		
		int first_increase = findDecreasing(middle_tree, INT_MIN);
		
		assert(first_increase >= 0);
		
		Node *decreasing_tree(0), *element(0), *not_used_tree(0);
		
		cutTree(middle_tree, first_increase, first_increase + 1, not_used_tree, element, decreasing_tree);
		
		if (element->value < getMax(decreasing_tree))
		{
			int new_index = findNext(decreasing_tree, element->value);
		
			Node *new_place(0), *right_decreasing_tree(0);
			cutTree(decreasing_tree, new_index, new_index + 1, right_decreasing_tree, new_place, decreasing_tree);
			
			std::swap(element, new_place);
			reconstructTree(decreasing_tree, right_decreasing_tree, new_place, decreasing_tree);
			doReverse(decreasing_tree);
			ans = 1;
		}
		
		reconstructTree(middle_tree, not_used_tree, element, decreasing_tree);
		reconstructTree(tree, left_tree, middle_tree, right_tree);
		
		if (!ans)
			reverse(left, right);
		
		return ans;
	}
	
	int getValue(ui32 index)
	{
		return (int)subsegmentSum(index, index + 1);
	}
	
	~NextPermutationTreap()
	{
		delete tree;
	}
}; 
