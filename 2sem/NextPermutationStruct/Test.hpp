#include "construct.hpp"

#include <random>
#include <iostream>
#include <vector>

#include <cstdio>

void printStruct(INextPermutatiomStruct * array)
{
	if (!array)
		return;
	
	for (ui32 i = 0; i < array->size(); ++i)
		std::cout << array->getValue(i) << ' ';
	std::cout << '\n';
}

class RandomTest
{
	enum TypeOfEvent
	{
		INSERT,
		ASSIGN,
		REVERSE,
		SUM,
		NEXT_PERMUTATION,
		VALUE,
		SIZE
	};
	
	struct Event
	{
		TypeOfEvent type_of_event;
		ui32 left, right, index;
		int value;
	};
	
	std::vector<Event> events;
	std::vector<int> start_array;
	std::uniform_int_distribution<int> Numbers;
	std::default_random_engine generator;
	
	ui32 getRandomIndex(ui32 left, ui32 right)
	{
		std::uniform_int_distribution<ui32> Index(left, right - 1);
		return Index(generator);
	}
	
	void generateArray(ui32 array_size)
	{
		for (ui32 i = 0; i < array_size; ++i)
			start_array.push_back(Numbers(generator));
	}
	
	void generateEvents(ui32 number_events)
	{
		ui32 current_size = start_array.size();
		
		for (ui32 i = 0; i < number_events; ++i)
		{
			Event event;
			event.type_of_event = static_cast<TypeOfEvent>(getRandomIndex(0, 7));
			event.index = getRandomIndex(0, current_size);
			event.left = getRandomIndex(0, current_size - 1);
			event.right = getRandomIndex(event.left + 1, current_size);
			event.value = Numbers(generator);
			events.push_back(event);
			
			if (event.type_of_event == INSERT)
				++current_size;
		}
	}
	
	RandomTest()
	{
	}
	
public:
	
	RandomTest(ui32 array_size, ui32 number_events, int left, int right, int seed = 501)
	{
		Numbers = std::uniform_int_distribution<int>(left, right);
		generator = std::default_random_engine(seed);
		
		generateArray(array_size);
		generateEvents(number_events);
	}
	
	template<class Array>
	void getTest()
	{
		INextPermutatiomStruct * array = create<Array>();
		
		for (ui32 i = 0; i < start_array.size(); ++i)
		{
			array->insert(i, start_array[i]);
		}
		
		printStruct(array);
		
		for (ui32 i = 0; i < events.size(); ++i)
		{
			std::cout << i << '\n'; 
			//~ printStruct(array);
			
			switch (events[i].type_of_event)
			{
				case INSERT:
					array->insert(events[i].index, events[i].value);
					std::cout << "0 " << events[i].index << ' ' << events[i].value << '\n';
					break;
				case ASSIGN:
					array->assign(events[i].index, events[i].value);
					std::cout << "1 " << events[i].index << ' ' << events[i].value << '\n';
					break;
				case REVERSE:
					std::cout << "2 " << events[i].left << ' ' << events[i].right << '\n';
					array->reverse(events[i].left, events[i].right);
					break;
				case SUM:
					std::cout << "3 " << events[i].left << ' ' << events[i].right << '\n';
					std::cout << "Sum: " << array->subsegmentSum(events[i].left, events[i].right) << '\n';
					break;
				case NEXT_PERMUTATION:
					std::cout << "4 " << events[i].left << ' ' << events[i].right << '\n';
					std::cout << "NextPermutation: " << array->nextPermutation(events[i].left, events[i].right) << '\n';
					break;
				case VALUE:
					std::cout << "5 " << events[i].index << '\n';
					std::cout << "Value: " << array->getValue(events[i].index) << '\n';
					break;
				case SIZE:
					std::cout << "6 \n";
					std::cout << "Size: " << array->size() << '\n';
					break;
				default:
					assert(0);
					break;
			}
		}
		printStruct(array);
			
		delete array;
	}
};
