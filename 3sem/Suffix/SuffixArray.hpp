#ifndef _SUFFIXARRAY
#define _SUFFIXARRAY

#include "SuffixCommon.hpp"

#include <vector>
#include <array>
#include <climits>
#include <cassert>

#include <iostream>
using std::cerr;

using std::vector;
using std::array;
using std::pair;
using std::make_pair;

class MergeCompare
{
    const vector<ui32>& string;
    const vector<ui32>& rank_1_2;
    
    array<ui32, 3> makeTriple(ui32 i)
    {
        array<ui32, 3> triple;
        triple[0] = string[i];
        triple[1] = getElement(i + 1, string, 1);
        triple[2] = getElement(i + 2, rank_1_2, 1);
        
        return triple;
    }
    
public:
    MergeCompare (const vector<ui32> &string, const vector<ui32> &rank_1_2)
    : string(string), rank_1_2(rank_1_2)
    {
    }
    
    bool operator () (const pair<ui32, bool> &l_p, const pair<ui32, bool> &r_p)
    {
        bool result = 0;
        ui32 l = l_p.first;
        ui32 r = r_p.first;
        if (l_p.second)
            std::swap(l, r);
        
        if (l % 3 == 1)
            result = make_pair(string[l], getElement(l + 1, rank_1_2, 1)) < 
            make_pair(string[r], getElement(r + 1, rank_1_2, 1));
        else
            result = makeTriple(l) < makeTriple(r);
        
        return result ^ l_p.second;
    }
};

class GetPairFirstDigit: public IGetDigit<pair<ui32, ui32> >
{
    ui32 alphabet_size;
public:
    GetPairFirstDigit (ui32 alphabet_size)
    : alphabet_size(alphabet_size)
    {
    }
    
    ui32 operator () (const pair<ui32, ui32> &a) const
    {
        return a.first;
    }
    
    bool next()
    {
        return false;
    }
    
    ui32 getAlphabetSize() const
    {
        return alphabet_size;
    }
};

class SuffixArray
{
    vector<ui32> string;
    
    vector<ui32> suffix_array;
    vector<ui32> lcp;
    ui32 alphabet_size;
    
    vector<ui32> getCompressedString(ui32 &new_alphabet_size) const
    {
        ui32 new_size = string.size() + 2;
        
        if (string.size() % 3 == 1)
            new_size++;
        
        
        vector<pair<array<ui32, 3>, ui32> > triples(string.size() - (1 + (string.size() - 1) / 3) + (string.size() % 3 == 1));
        ui32 idx(0);
        
        for (ui32 i = 0; i + 2 < new_size; ++i)
        {
            if (!(i % 3))
                continue;
                
            array<ui32, 3> triple;
            for (ui32 j = 0; j < 3; ++j)
                triple[j] = getElement(i + j, string);
            
            triples[idx++] = make_pair(triple, i);
        }
        
        DigitalSort(triples, GetArrayDigit<array<ui32, 3> >(alphabet_size));
        
        vector<ui32> new_str(triples.size());
        
        ui32 color = 1, num_left = (string.size() + 1 + (string.size() % 3  == 1)) / 3;
        
        for (ui32 i = 0; i < triples.size(); ++i)
        {
            if (i && triples[i].first != triples[i - 1].first)
                color++;
            
            ui32 index = triples[i].second / 3;
            
            if (triples[i].second % 3 == 2)
                index += num_left;
            
            new_str[index] = color;
        }
        
        new_alphabet_size = color + 1;
        
        return new_str;
    }
    
    void buildArray12(vector<pair<ui32, bool> > &array_1_2, const SuffixArray &new_array, ui32 num_left) const
    {
        for (ui32 i = 0; i < new_array.size(); ++i)
        {
            if (new_array[i] < num_left)
                array_1_2[i] = make_pair(new_array[i] * 3 + 1, 0);
            else
                array_1_2[i] = make_pair(3 * (new_array[i] - num_left) + 2, 0);
        }
    }
    
    void buildArray0(vector<pair<ui32, bool> > &array_0, const vector<pair<ui32, bool> > &array_1_2) const
    {
        vector<pair<ui32, ui32> > pairs(array_0.size());
        ui32 idx(0);
        
        for (ui32 i = 0; i < array_1_2.size(); ++i)
            if (array_1_2[i].first % 3 == 1)
            {
                pairs[idx++] = make_pair(string[array_1_2[i].first - 1], array_1_2[i].first - 1);
            }
        
        countingSort(pairs, GetPairFirstDigit(alphabet_size + (string.size() % 3 == 1)));
        
        for (ui32 i = 0; i < pairs.size(); ++i)
        {
            array_0[i] = make_pair(pairs[i].second, 1);
        }
    }
    
    void mergeArrays(const vector<pair<ui32, bool> > &array_0, const vector<pair<ui32, bool> > &array_1_2)
    {
        vector<ui32> rank_1_2(string.size() + (string.size() % 3 == 1));
        
        for (ui32 i = 0; i < array_1_2.size(); ++i)
            rank_1_2[array_1_2[i].first] = i;

        vector<pair<ui32, bool> > pre_array(string.size());
        
        MergeCompare cmp(string, rank_1_2);
        std::merge(array_1_2.begin() + (string.size() % 3 == 1), array_1_2.end(), array_0.begin(), array_0.end(),
        pre_array.begin(), cmp);
        
        // It is win!
        for (ui32 i = 0; i < string.size(); ++i)
            suffix_array[i] = pre_array[i].first;
    }
    
    void buildSuffixArray()
    {
        if (string.size() <= 1)
            return;
        
        getAlphabetSize(alphabet_size, string);
        
        // recursion
        ui32 new_alphabet_size(0);
        
        vector<ui32> compressed_str = getCompressedString(new_alphabet_size);
        SuffixArray new_array(compressed_str, new_alphabet_size);
        
        // building of array_1_2
        vector<pair<ui32, bool> > array_1_2(new_array.size());
        
        ui32 num_left = (string.size() + 1 + (string.size() % 3 == 1)) / 3;
        buildArray12(array_1_2, new_array, num_left);
        
        // building of array_0
        vector<pair<ui32, bool> > array_0(num_left);
        buildArray0(array_0, array_1_2);
        
        // merging of array_1_2 and array_0
        mergeArrays(array_0, array_1_2);
    }
    
    SuffixArray (const vector<ui32> &string, ui32 _alphabet_size)
    : string(string), suffix_array(vector<ui32>(string.size(), 0))
    {
        alphabet_size = _alphabet_size;
        buildSuffixArray();
    }
    
    void calcLCP()
    {
        vector<ui32> rank(string.size());
        
        for (ui32 i = 0; i < suffix_array.size(); ++i)
            rank[suffix_array[i]] = i;
            
        ui32 last = 0;
        
        lcp.resize(string.size(), 0);
        
        for (ui32 i = 0; i < string.size(); ++i)
        {
            if (last > 0)
                last--;
                
            if (rank[i] == string.size() - 1)
            {
                last = 0;
                continue;
            }
            
            ui32 j = suffix_array[rank[i] + 1];
            
            while (std::max(i + last, j + last) < string.size() && string[i + last] == string[j + last])
                last++;
            
            lcp[rank[i]] = last;
        }
    }
    
public:
    explicit SuffixArray (const vector<ui32> &_string)
    : string(_string), suffix_array(vector<ui32>(string.size(), 0)), lcp(vector<ui32>()), alphabet_size(0)
    {
        buildSuffixArray();
    }
    
    SuffixArray (const vector<ui32> &_string, const vector<ui32> &suffix_array)
    : string(_string), suffix_array(suffix_array), lcp(vector<ui32>()), alphabet_size(0)
    {
    }
    
    size_t size() const
    {
        return string.size();
    }
    
    const vector<ui32>& getArray() const
    {
        return suffix_array;
    }
    
    const vector<ui32>& getLCP()
    {
        if (lcp.empty())
            calcLCP();
        return lcp;
    }
    
    const ui32& operator [] (const ui32 &n) const
    {
        return suffix_array[n];
    }
};
#endif
