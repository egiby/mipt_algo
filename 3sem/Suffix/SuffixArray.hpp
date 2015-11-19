#ifndef _SUFFIXARRAY
#define _SUFFIXARRAY

#include "Sort.hpp"

#include <vector>
#include <array>
#include <unordered_map>
#include <climits>
#include <cassert>

#include <iostream>

using std::vector;
using std::array;
using std::pair;
using std::unordered_map;
using std::make_pair;
using std::cerr;

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
        if (index == 0)
            return false;
        
        index--;
        return true;
    }
    
    ui32 getAlphabetSize() const
    {
        return alphabet_size;
    }
};

class SuffixArray
{
    vector<ui32> str;
    
    vector<ui32> suffix_array;
    vector<ui32> lcp;
    ui32 alphabet_size;
    
    void getSmallAlphabet()
    {
        vector<ui32> alphabet(str);
        
        LSDSort(alphabet.begin(), alphabet.end());
        
        unordered_map<ui32, ui32> ch;
        
        for (auto c: alphabet)
            if (ch.find(c) == ch.end())
            {
                ui32 color = ch.size() + 1;
                ch[c] = color;
            }
        
        for (ui32 i = 0; i < str.size(); ++i)
            str[i] = ch[str[i]];
    }
    
    void getAlphabetSize(bool is_alphabet_small)
    {
        if (!is_alphabet_small)
            getSmallAlphabet();
            
        alphabet_size = *std::max_element(str.begin(), str.end()) + 1;
    }
    
    vector<ui32> getCompressedString()
    {
        ui32 old_size = str.size();
        
        str.push_back(0);
        str.push_back(0);
        if (old_size % 3 == 1)
            str.push_back(0);
        
        vector<pair<array<ui32, 3>, ui32> > triples;
        
        for (ui32 i = 0; i + 2 < str.size(); ++i)
        {
            if (!(i % 3))
                continue;
                
            array<ui32, 3> triple;
            for (ui32 j = 0; j < 3; ++j)
                triple[j] = str[i + j];
            
            triples.push_back(make_pair(triple, i));
        }
        
        LSDSort(triples.begin(), triples.end(), GetArrayDigit<array<ui32, 3> >(alphabet_size));
        
        vector<ui32> new_str(triples.size());
        
        ui32 color = 1, num_left = (old_size + 1 + (old_size % 3  == 1)) / 3;
        
        for (ui32 i = 0; i < triples.size(); ++i)
        {
            if (i && triples[i].first != triples[i - 1].first)
                color++;
            
            ui32 index = triples[i].second / 3;
            
            if (triples[i].second % 3 == 2)
                index += num_left;
            
            new_str[index] = color;
        }
        
        str.pop_back();
        str.pop_back();
        if (old_size % 3 == 1)
        {
            str.pop_back();
        }
        
        assert(str.size() == old_size);
        
        return new_str;
    }
    
    ui32 getElement(ui32 i, const vector<ui32> &a)
    {
        return i < a.size() ? a[i] + 1 : 0;
    }
    
    array<ui32, 3> makeTriple(ui32 i, const vector<ui32> &rank)
    {
        array<ui32, 3> triple;
        triple[0] = str[i];
        triple[1] = getElement(i + 1, str);
        triple[2] = getElement(i + 2, rank);
        
        return triple;
    }
    
    void buildSuffixArray()
    {
        if (str.size() <= 1)
            return;
        
        // recursion
        SuffixArray new_array(getCompressedString(), true);
        
        // building of array_1_2
        vector<ui32> array_1_2;
        
        ui32 num_left = (str.size() + 1 + (str.size() % 3 == 1)) / 3;
        
        for (ui32 i = 0; i < new_array.size(); ++i)
        {
            if (new_array[i] < num_left)
                array_1_2.push_back(new_array[i] * 3 + 1);
            else
                array_1_2.push_back(3 * (new_array[i] - num_left) + 2);
        }
        
        // building of array_0
        vector<ui32> array_0;
        
        vector<pair<array<ui32, 1>, ui32> > pairs;
        
        for (ui32 i = 0; i < array_1_2.size(); ++i)
            if (array_1_2[i] % 3 == 1)
            {
                pairs.push_back(make_pair(array<ui32, 1>({{str[array_1_2[i] - 1]}}), array_1_2[i] - 1));
            }
        
        countingSort(pairs.begin(), pairs.end(), GetArrayDigit<array<ui32, 1> >(alphabet_size + (str.size() % 3 == 1)));
        
        for (ui32 i = 0; i < pairs.size(); ++i)
        {
            array_0.push_back(pairs[i].second);
        }
        
        // merging of array_1_2 and array_0
        vector<ui32> rank_1_2(str.size() + (str.size() % 3 == 1));
        
        for (ui32 i = 0; i < array_1_2.size(); ++i)
            rank_1_2[array_1_2[i]] = i;
            
        ui32 l = (str.size() % 3 == 1), r = 0, idx = 0;
        
        while (l < array_1_2.size() && r < array_0.size())
        {
            bool result = 0;
            
            if (array_1_2[l] % 3 == 1)
            {
                pair<ui32, ui32> left_pair, right_pair;
                
                left_pair = make_pair(str[array_1_2[l]], getElement(array_1_2[l] + 1, rank_1_2));
                right_pair = make_pair(str[array_0[r]], getElement(array_0[r] + 1, rank_1_2));
                
                result = left_pair < right_pair;
            }
            else
            {
                array<ui32, 3> left_triple, right_triple;
                
                left_triple = makeTriple(array_1_2[l], rank_1_2);
                right_triple = makeTriple(array_0[r], rank_1_2);
                
                result = left_triple < right_triple;
            }
            
            if (result)
                suffix_array[idx++] = array_1_2[l++];
            else
                suffix_array[idx++] = array_0[r++];
        }
        
        while (l < array_1_2.size())
            suffix_array[idx++] = array_1_2[l++];
        
        while (r < array_0.size())
            suffix_array[idx++] = array_0[r++];
    }
    
    void calcLCP()
    {
        // okay...
    }
public:
    SuffixArray (const vector<ui32> &str, const vector<ui32> &suffix_array, bool is_alphabet_small = 0)
    : str(str), suffix_array(suffix_array)
    {
        getAlphabetSize(is_alphabet_small);
    }
    
    SuffixArray (const vector<ui32> &str, bool is_alphabet_small = 0)
    : str(str), suffix_array(vector<ui32>())
    {
        getAlphabetSize(is_alphabet_small);
    }
    
    size_t size() const
    {
        return str.size();
    }
    
    const vector<ui32>& getArray()
    {
        if (suffix_array.empty())
        {
            suffix_array = vector<ui32>(0, str.size());
            buildSuffixArray();
        }
        
        return suffix_array;
    }
    
    const vector<ui32>& getLcp()
    {
        if (lcp.empty())
        {
            calcLCP();
        }
        
        return lcp;
    }
    
    const ui32& operator [] (const ui32 &n)
    {
        if (suffix_array.empty())
        {
            suffix_array = vector<ui32>(str.size(), 0);
            buildSuffixArray();
        }
        return suffix_array[n];
    }
};
#endif
