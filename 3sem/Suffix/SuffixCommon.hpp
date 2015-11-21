#ifndef _SUFFIXCOMMON
#define _SUFFIXCOMMON

#include "Sort.hpp"

typedef unsigned int ui32;

template<class Array>
class GetArrayDigit: public IGetDigit<pair<Array, ui32> >
{
    ui32 index;
    ui32 alphabet_size;
    
    GetArrayDigit()
    {
    }
    
public:
    GetArrayDigit (ui32 alphabet_size)
    : index(std::tuple_size<Array>() - 1), alphabet_size(alphabet_size)
    {
    }
    
    ui32 operator () (const pair<Array, ui32> &a) const
    {
        return a.first[index];
    }
    
    bool next()
    {
        if (!index)
            return false;
        
        index--;
        return true;
    }
    
    ui32 getAlphabetSize() const
    {
        return alphabet_size;
    }
};

ui32 getElement(ui32 i, const vector<ui32> &a, ui32 inc = 0)
{
    return i < a.size() ? a[i] + inc : 0;
}

#endif
