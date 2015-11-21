#ifndef _SUFFIXARRAY
#define _SUFFIXARRAY

#include "SuffixCommon.hpp"

#include <vector>
#include <array>
#include <unordered_map>
#include <climits>
#include <cassert>

#include <iostream>
using std::cerr;

using std::vector;
using std::array;
using std::pair;
using std::unordered_map;
using std::make_pair;

class MergeCompare
{
    const vector<ui32>& str;
    const vector<ui32>& rank_1_2;
    
    array<ui32, 3> makeTriple(ui32 i)
    {
        array<ui32, 3> triple;
        triple[0] = str[i];
        triple[1] = getElement(i + 1, str, 1);
        triple[2] = getElement(i + 2, rank_1_2, 1);
        
        return triple;
    }
    
public:
    MergeCompare (const vector<ui32> &str, const vector<ui32> &rank_1_2)
    : str(str), rank_1_2(rank_1_2)
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
            result = make_pair(str[l], getElement(l + 1, rank_1_2, 1)) < 
            make_pair(str[r], getElement(r + 1, rank_1_2, 1));
        else
            result = makeTriple(l) < makeTriple(r);
        
        return result ^ l_p.second;
    }
};

class SuffixArray
{
    vector<ui32> str;
    
    vector<ui32> suffix_array;
    vector<ui32> lcp;
    ui32 alphabet_size;
    
    ui32 getSmallAlphabet()
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
        
        return ch.size() + 1;
    }
    
    void getAlphabetSize()
    {
        if (!alphabet_size)
            alphabet_size = getSmallAlphabet();
    }
    
    vector<ui32> getCompressedString(ui32 &new_alphabet_size)
    {
        ui32 new_size = str.size() + 2;
        
        if (str.size() % 3 == 1)
            new_size++;
        
        vector<pair<array<ui32, 3>, ui32> > triples;
        
        for (ui32 i = 0; i + 2 < new_size; ++i)
        {
            if (!(i % 3))
                continue;
                
            array<ui32, 3> triple;
            for (ui32 j = 0; j < 3; ++j)
                triple[j] = getElement(i + j, str);
            
            triples.push_back(make_pair(triple, i));
        }
        
        LSDSort(triples.begin(), triples.end(), GetArrayDigit<array<ui32, 3> >(alphabet_size));
        
        vector<ui32> new_str(triples.size());
        
        ui32 color = 1, num_left = (str.size() + 1 + (str.size() % 3  == 1)) / 3;
        
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
    
    void buildSuffixArray()
    {
        if (str.size() <= 1)
            return;
        
        // recursion
        ui32 new_alphabet_size(0);
        
        vector<ui32> compressed_str = getCompressedString(new_alphabet_size);
        
        SuffixArray new_array(compressed_str, new_alphabet_size);
        
        // building of array_1_2
        vector<pair<ui32, bool> > array_1_2;
        
        ui32 num_left = (str.size() + 1 + (str.size() % 3 == 1)) / 3;
        
        for (ui32 i = 0; i < new_array.size(); ++i)
        {
            if (new_array[i] < num_left)
                array_1_2.push_back(make_pair(new_array[i] * 3 + 1, 0));
            else
                array_1_2.push_back(make_pair(3 * (new_array[i] - num_left) + 2, 0));
        }
        
        // building of array_0
        vector<pair<ui32, bool> > array_0;
        
        vector<pair<array<ui32, 1>, ui32> > pairs;
        
        for (ui32 i = 0; i < array_1_2.size(); ++i)
            if (array_1_2[i].first % 3 == 1)
            {
                pairs.push_back(make_pair(array<ui32, 1>({{str[array_1_2[i].first - 1]}}), array_1_2[i].first - 1));
            }
        
        countingSort(pairs.begin(), pairs.end(), GetArrayDigit<array<ui32, 1> >(alphabet_size + (str.size() % 3 == 1)));
        
        for (ui32 i = 0; i < pairs.size(); ++i)
        {
            array_0.push_back(make_pair(pairs[i].second, 1));
        }
        
        // merging of array_1_2 and array_0
        vector<ui32> rank_1_2(str.size() + (str.size() % 3 == 1));
        
        for (ui32 i = 0; i < array_1_2.size(); ++i)
            rank_1_2[array_1_2[i].first] = i;

        vector<pair<ui32, bool> > pre_array(str.size());
        
        MergeCompare cmp(str, rank_1_2);
        std::merge(array_1_2.begin() + (str.size() % 3 == 1), array_1_2.end(), array_0.begin(), array_0.end(),
        pre_array.begin(), cmp);
        
        // It is win!
        for (ui32 i = 0; i < str.size(); ++i)
            suffix_array[i] = pre_array[i].first;
    }
    
    SuffixArray (const vector<ui32> &str, ui32 _alphabet_size)
    : str(str), suffix_array(vector<ui32>(str.size(), 0))
    {
        alphabet_size = _alphabet_size;
        buildSuffixArray();
    }
    
public:
    SuffixArray (const vector<ui32> &str, const vector<ui32> &suffix_array)
    : str(str), suffix_array(suffix_array)
    {
        alphabet_size = 0;
        getAlphabetSize();
    }
    
    SuffixArray (const vector<ui32> &str)
    : str(str), suffix_array(vector<ui32>(str.size(), 0))
    {
        alphabet_size = 0;
        getAlphabetSize();
        buildSuffixArray();
    }
    
    size_t size() const
    {
        return str.size();
    }
    
    const vector<ui32>& getArray() const
    {
        return suffix_array;
    }
    
    void calcLCP()
    {
        vector<ui32> rank(str.size());
        
        for (ui32 i = 0; i < suffix_array.size(); ++i)
            rank[suffix_array[i]] = i;
            
        ui32 last = 0;
        
        lcp.resize(str.size(), 0);
        
        for (ui32 i = 0; i < str.size(); ++i)
        {
            if (last > 0)
                last--;
                
            if (rank[i] == str.size() - 1)
            {
                last = 0;
                continue;
            }
            
            ui32 j = suffix_array[rank[i] + 1];
            
            while (std::max(i + last, j + last) < str.size() && str[i + last] == str[j + last])
                last++;
            
            lcp[rank[i]] = last;
        }
    }
    
    const vector<ui32>& getLCP() const
    {
        return lcp;
    }
    
    const ui32& operator [] (const ui32 &n) const
    {
        return suffix_array[n];
    }
};
#endif
