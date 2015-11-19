#include <algorithm>
#include <vector>

#include <iostream>

typedef unsigned int ui32;

using std::vector;
using std::pair;

using std::cerr;

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
    : power(1)
    {
    }
    
    ui32 operator () (const ui32 &a) const
    {
        return (a / power) % 10;
    }
    
    bool next()
    {
        if (power == 1000000000)
            return false;
            
        power *= 10;
        return true;
    }
    
    ui32 getAlphabetSize() const
    {
        return 10;
    }
};

template<class Iterator, class GetDigit>
void countingSort(Iterator begin, Iterator end, GetDigit get_digit)
{
    if (end <= begin)
        return;
    
    vector<vector<Iterator> > buckets(get_digit.getAlphabetSize());
    
    for (Iterator it = begin; it != end; ++it)
        buckets[get_digit(*it)].push_back(it);
    
    vector<typename std::iterator_traits<Iterator>::value_type> new_array;

    for (auto bucket: buckets)
    {
        for (auto it: bucket)
        {
            new_array.push_back(*it);
        }
    }
    
    for (Iterator it = begin; it != end; ++it)
        *it = new_array[it - begin];
}

template<class Iterator, class GetDigit = GetIntegerDigit>
void LSDSort(Iterator begin, Iterator end, GetDigit get_digit = GetDigit())
{
    do
    {
        countingSort(begin, end, get_digit);
    }
    while (get_digit.next());
}
