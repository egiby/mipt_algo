#ifndef _SUFFIXCOMMON
#define _SUFFIXCOMMON

#include "Sort.hpp"

#include <unordered_map>
using std::unordered_map;

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

ui32 getSmallAlphabet(vector<ui32> &string)
{
    vector<ui32> alphabet(string);
    
    DigitalSort(alphabet);
    
    unordered_map<ui32, ui32> mapping;
    
    for (auto c: alphabet)
        if (mapping.find(c) == mapping.end())
        {
            ui32 color = mapping.size() + 1;
            mapping[c] = color;
        }
    
    for (ui32 i = 0; i < string.size(); ++i)
        string[i] = mapping[string[i]];
    
    return mapping.size() + 1;
}

void getAlphabetSize(ui32 &alphabet_size, vector<ui32> &string)
{
    if (!alphabet_size)
        alphabet_size = getSmallAlphabet(string);
}

#endif
