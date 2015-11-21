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
        triple[1] = getElement(i + 1, str);
        triple[2] = getElement(i + 2, rank_1_2);
        
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
            result = make_pair(str[l], getElement(l + 1, rank_1_2)) < make_pair(str[r], getElement(r + 1, rank_1_2));
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
        
        for (ui32 i = 0; i < str.size(); ++i)
            suffix_array[i] = pre_array[i].first;
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
public:
    SuffixArray (const vector<ui32> &str, const vector<ui32> &suffix_array, bool is_alphabet_small = 0)
    : str(str), suffix_array(suffix_array)
    {
        getAlphabetSize(is_alphabet_small);
    }
    
    SuffixArray (const vector<ui32> &str, bool is_alphabet_small = 0)
    : str(str), suffix_array(vector<ui32>(str.size(), 0))
    {
        getAlphabetSize(is_alphabet_small);
        buildSuffixArray();
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
    
    const vector<ui32>& getLCP()
    {
        if (lcp.empty())
        {
            calcLCP();
        }
        
        return lcp;
    }
    
    const ui32& operator [] (const ui32 &n) const
    {
        return suffix_array[n];
    }
};
#endif
