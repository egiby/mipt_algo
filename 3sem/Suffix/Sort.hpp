#ifndef _COUNTINGSORT
#define _COUNTINGSORT

#include <algorithm>
#include <vector>
#include <cassert>

typedef unsigned int ui32;

using std::vector;
using std::pair;

template<class Type>
class IGetDigit
{
public:
    virtual ui32 operator () (const Type &p) const = 0;
    virtual bool next() = 0;
    virtual ui32 getAlphabetSize() const = 0;
};

class GetIntegerDigit: public IGetDigit<ui32>
{
    ui32 power;
    
public:
    GetIntegerDigit ()
    : power(0)
    {
    }
    
    ui32 operator () (const ui32 &a) const
    {
        return (a >> power) & 7;
    }
    
    bool next()
    {
        if (power == 30)
            return false;
            
        power += 3;
        return true;
    }
    
    ui32 getAlphabetSize() const
    {
        return 8;
    }
};

template<class Type, class GetDigit>
void countingSort(vector<Type> &array, GetDigit get_digit)
{
    if (!array.size())
        return;
    
    vector<ui32> buckets(get_digit.getAlphabetSize() + 1);
    
    for (auto a: array)
    {
        //~ assert(get_digit(*it) < get_digit.getAlphabetSize());
        ++buckets[get_digit(a) + 1];
    }
    
    for (ui32 i = 1; i < buckets.size(); ++i)
        buckets[i] += buckets[i - 1];
    
    vector<Type> new_array(array.size());
    for (auto a: array)
        new_array[buckets[get_digit(a)]++] = a;
    
    array = new_array;
}

template<class Type, class GetDigit = GetIntegerDigit>
void LSDSort(vector<Type> &array, GetDigit get_digit = GetDigit())
{
    do
    {
        countingSort(array, get_digit);
    }
    while (get_digit.next());
}

#endif
