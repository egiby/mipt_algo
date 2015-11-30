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
        return (a >> power) & 255;
    }
    
    bool next()
    {
        if (power == 24)
            return false;
            
        power += 8;
        return true;
    }
    
    ui32 getAlphabetSize() const
    {
        return 256;
    }
};

template<class Type, class GetDigit>
void countingSort(vector<Type> &array, GetDigit get_digit)
{
    if (!array.size())
        return;
    
    vector<ui32> baskets(get_digit.getAlphabetSize() + 1);
    
    for (ui32 i = 0; i < array.size(); ++i)
    {
        //~ assert(get_digit(*it) < get_digit.getAlphabetSize());
        ++baskets[get_digit(array[i]) + 1];
    }
    
    for (ui32 i = 1; i < baskets.size(); ++i)
        baskets[i] += baskets[i - 1];
    
    vector<Type> new_array(array.size());
    for (ui32 i = 0; i < array.size(); ++i)
        new_array[baskets[get_digit(array[i])]++] = array[i];
    
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
