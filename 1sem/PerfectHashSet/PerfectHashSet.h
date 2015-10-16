#include <iostream>
#include <vector>
#include <random>
#include <string>

#include <climits>
#include <cassert>

using std::cerr;
using std::vector;

typedef unsigned int ui32;
typedef unsigned long long ui64;

namespace HashSet
{	
	ui64 Sqr(ui64 a)
	{
		return a * a;
	}
	
	struct Node
	{
		// (flag == true) if there is key in set
		bool is_good, flag;
		ui32 key;
		
		Node ()
		:is_good(0), flag(0)
		{
		}
	};
	
	const ui64 PRIME = (1LL << 32) + 15;
	std::uniform_int_distribution<ui64> FirstParameter(1, PRIME - 1);
	std::uniform_int_distribution<ui64> SecondParameter(0, PRIME - 1);
	std::default_random_engine generator = std::default_random_engine(501);	
	
	class IHashTable
	{
	public:
		virtual void Init(const vector<ui32> &keys) = 0;
		virtual bool IsPossibleKey(const ui32 key) = 0;
		virtual bool Has(const ui32 key) = 0;
		virtual void Insert(const ui32 key) = 0;
		virtual void Erase(const ui32 key) = 0;
		virtual ui32 Size() const = 0;
		virtual void Clear() = 0;
		virtual ~IHashTable() 
		{
		}
	};
	
	class HashFunction
	{
		ui64 a, b, n;
		
		ui64 GetFirstParam()
		{
			return FirstParameter(generator);
		}
		
		ui64 GetSecondParam()
		{
			return SecondParameter(generator);
		}
		
	public:
		HashFunction () 
		{
		}
		
		explicit HashFunction (ui64 n)
		:a(GetFirstParam()), b(GetSecondParam()), n(n)
		{
		}
		
		HashFunction (ui64 a, ui64 b, ui64 n)
		:a(a), b(b), n(n)
		{
		}
		
		ui32 operator () (ui32 key) const
		{
			ui64 a_q = a >> 32, a_r = a & UINT_MAX;
			return (((a_q * key * (1LL << 32)) % PRIME + (a_r * key) % PRIME) % PRIME) % n;
		}
	};
	
	// Exceptions
	class Exception
	{
	public:
		ui32 key;
		Exception () 
		{
		}
		
		virtual std::string Message() const
		{
			return "Unknown error!";
		}
	};
	
	class UnknownKeyException: public Exception
	{
	public:
		UnknownKeyException () 
		{
		}
		
		explicit UnknownKeyException (ui32 bad_key)
		{
			key = bad_key;
		}
		
		virtual std::string Message() const
		{
			return "Unknown key!\n";
		}
	};
	
	class EqualKeysException: public Exception
	{
	public:
		EqualKeysException () 
		{
		}
		
		explicit EqualKeysException (ui32 bad_key)
		{
			key = bad_key;
		}
		
		virtual std::string Message() const
		{
			return "Equal keys!\n";
		}
	};
	
	void CheckEqual(ui32 first_key, ui32 second_key)
	{
		if (first_key == second_key)
		{
			EqualKeysException key_exception(first_key);
			throw &key_exception;
		}
	}
	
	void CheckAllPairs(const vector<ui32> &keys)
	{
		for (ui32 i = 0; i < keys.size(); ++i)
			for (ui32 j = i + 1; j < keys.size(); ++j)
				CheckEqual(keys[i], keys[j]);
	}
	
	class SecondLevelTable: public IHashTable
	{
		vector<Node> date;
		HashFunction hash;
		ui32 n;
		ui32 size;
		
		Node* Element(const ui32 key)
		{
			return &date[hash(key)];
		}
		
		bool GoodKey(const ui32 key)
		{
			return Element(key)->is_good && Element(key)->key == key;
		}
		
		void CheckKey(const ui32 key)
		{
			if (!GoodKey(key))
			{	
				UnknownKeyException key_exception(key);
				throw &key_exception;
			}
		}
		
		void Modify(const ui32 key, const bool event)
		{
			CheckKey(key);
			size += (event - Element(key)->flag);
			Element(key)->flag = event;
		}
		
	public:
		void Clear() 
		{
			date.clear();
			size = 0;
			n = 0;
			hash = HashFunction(0, 0, 0);
		}
		
		void Init(const vector<ui32> &keys)
		{
			CheckAllPairs(keys);
			Clear();
			n = Sqr(keys.size());
			if (!n)
				n++;
			
			while (1)
			{
				hash = HashFunction(n);
				vector<bool> used(n);
				bool good = 1;
				for (ui32 i = 0; i < keys.size(); ++i)
					if (used[hash(keys[i])])
					{	
						good = 0;
						break;
					}
					else
						used[hash(keys[i])] = 1;
				if (good)
					break;
			}
			
			date.resize(n);
			
			for (ui32 i = 0; i < keys.size(); ++i)
			{	
				Element(keys[i])->is_good = 1;
				Element(keys[i])->key = keys[i];
			}
		}
		
		bool IsPossibleKey(const ui32 key)
		{
			return GoodKey(key);
		}
		
		bool Has(const ui32 key)
		{
			CheckKey(key);
			return GoodKey(key) && Element(key)->flag;
		}
		
		void Insert(const ui32 key)
		{
			Modify(key, 1);
		}
		
		void Erase(const ui32 key)
		{
			Modify(key, 0);
		}
		
		ui32 Size() const
		{
			return size;
		}
	};
}
	
class PerfectHashSet: public HashSet::IHashTable 
{
	ui32 size;
	
	vector<HashSet::SecondLevelTable> date;
	ui32 n;
	
	HashSet::HashFunction hash;
	
	HashSet::SecondLevelTable* Element(const ui32 key)
	{
		return &date[hash(key)];
	}
	
	bool LinearSize(const vector<vector<ui32> > &baskets)
	{
		ui64 sum = 0;
		for (ui32 i = 0; i < n; ++i)
		{
			sum += HashSet::Sqr(baskets[i].size());
			if (sum > 3 * n)
				return 0;
		}
		return 1;
	}
	
	void CheckEqualElements(const vector<vector<ui32> > &baskets)
	{
		for (ui32 i = 0; i < baskets.size(); ++i)
		{
			if (baskets[i].size() <= 3)
				HashSet::CheckAllPairs(baskets[i]);
			else
			{
				for (ui32 index = 1; index < baskets[i].size(); ++index)
					HashSet::CheckEqual(baskets[i][index], baskets[i][index - 1]);
			}
		}
	}
	
	void Modify(const ui32 key, const bool event)
	{
		size -= Element(key)->Size();
		if (event)
			Element(key)->Insert(key);
		else
			Element(key)->Erase(key);
		size += Element(key)->Size();
	}
	
public:
	
	PerfectHashSet ()
	{
	}
	
	void Init(const vector<ui32> &keys)
	{
		n = keys.size();
		size = 0;
		date.resize(n);
		
		vector<vector<ui32> > baskets; 
		while (1)
		{
			baskets.clear();
			assert(baskets.size() == 0);
			baskets.resize(n);
			
			hash = HashSet::HashFunction(n);
			for (ui32 i = 0; i < n; ++i)
				baskets[hash(keys[i])].push_back(keys[i]);
			
			if (LinearSize(baskets))
			{
				for (ui32 i = 0; i < n; ++i)
					date[i].Init(baskets[i]);
				break;
			}
			CheckEqualElements(baskets);
		}
	}
	
	bool IsPossibleKey(const ui32 key)
	{
		return Element(key)->IsPossibleKey(key);
	}
	
	bool Has(const ui32 key)
	{
		return Element(key)->Has(key);
	}
	
	void Insert(const ui32 key)
	{
		Modify(key, 1);
	}
	
	void Erase(const ui32 key)
	{
		Modify(key, 0);
	}
	
	ui32 Size() const
	{
		return size;
	}
	
	void Clear()
	{
		for (ui32 i = 0; i < n; ++i)
			date[i].Clear();
		date.clear();
		size = 0;
		n = 0;
	}
	
	~PerfectHashSet()
	{
		Clear();
	}
};
