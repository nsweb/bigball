
#ifndef BB_COREMAP_H
#define BB_COREMAP_H

#include "hash.h"

namespace bigball
{

template<typename K, typename V> 
class Map : protected Hash<K>
{
public:
	struct Pair
	{
		K Key;
		V Value;
	};

	Map() :
		m_Pairs(nullptr),
		m_HashTable(nullptr),
		m_NextTable(nullptr),
		m_HashSize(0),
		m_Mask(0),
		m_NbActivePairs(0)
	{

	}
	~Map()
	{
		BB_DELETE_ARRAY(m_Pairs);
		BB_FREE(m_HashTable);
		BB_FREE(m_NextTable);
		m_HashSize = 0;
		m_Mask = 0;
		m_NbActivePairs = 0;
	}

	void reserve( uint32 HashSize )
	{
		if( HashSize > m_HashSize )
		{
			// Get more entries
			m_HashSize = bigball::NextPowerOfTwo(HashSize);
			m_Mask = m_HashSize - 1;

			BB_FREE(m_HashTable);
			m_HashTable = (uint32*) Memory::Malloc( m_HashSize * sizeof(uint32) );
			Memory::Memset( m_HashTable, 0xFF, m_HashSize * sizeof(uint32) );

			// Get some bytes for new entries
			Pair* NewPairs	= new Pair[m_HashSize];/* (Pair*) Memory::Malloc( m_HashSize * sizeof(Pair) );*/		ASSERT(NewPairs);
			uint32* NewNext	= (uint32*) Memory::Malloc( m_HashSize * sizeof(uint32) );	ASSERT(NewNext);

			// Copy old data if needed
			for( uint32 i = 0; i < m_NbActivePairs; ++i )
				NewPairs[i] = m_Pairs[i];

			for( uint32 i=0; i < m_NbActivePairs; i++ )
			{
				uint32 HashValue = ((Hash<K> const &)*this)(m_Pairs[i].Key) & m_Mask;
				m_NextTable[i] = m_HashTable[HashValue];
				m_HashTable[HashValue] = i;
			}

			// Delete old data
			BB_FREE(m_NextTable);
			BB_DELETE_ARRAY(m_Pairs);

			// Assign new pointer
			m_Pairs = NewPairs;
			m_NextTable = NewNext;
		}
	}

	const Pair*	Find( K const& Key ) const
	{
		if( !m_HashTable )	return nullptr;	// Nothing has been allocated yet

		// Compute hash value for this key
		uint32 HashValue = ((Hash<K> const &)*this)(Key) & m_Mask;

		// Look for it in the table
		uint32 Offset = m_HashTable[HashValue];

		while( Offset != INDEX_NONE && m_Pairs[Offset].Key != Key )
		{
			//ASSERT(m_Pairs[Offset].mID0!=INVALID_USER_ID);
			Offset = m_NextTable[Offset];		// Better to have a separate array for this
		}
		if( Offset == INDEX_NONE )	return nullptr;

		ASSERT( Offset < m_NbActivePairs );
		// Match m_Pairs[Offset] => the pair is persistent
		return &m_Pairs[Offset];
	}

	Pair* Find( K const& Key, uint32 HashValue ) const
	{
		if( !m_HashTable )	return nullptr;	// Nothing has been allocated yet

		// Look for it in the table
		uint32 Offset = m_HashTable[HashValue];

		while( Offset != INDEX_NONE && m_Pairs[Offset].Key != Key )
		{
			//ASSERT(m_Pairs[Offset].mID0!=INVALID_USER_ID);
			Offset = m_NextTable[Offset];		// Better to have a separate array for this
		}
		if( Offset == INDEX_NONE )	return nullptr;

		ASSERT( Offset < m_Pairs.size() );
		// Match m_Pairs[Offset] => the pair is persistent
		return &m_Pairs[Offset];
	}

	const Pair*	Add( K const& Key, V const& Value )
	{
		// Compute hash value for this key
		uint32 HashValue = ((Hash<K> const &)*this)(Key) & m_Mask;
		Pair* P = Find(Key, HashValue);
		if(P)
			return P;	// Persistent pair

		// This is a new pair
		if( m_NbActivePairs >= m_HashSize )
		{
			// Get more entries
			m_HashSize = bigball::NextPowerOfTwo(m_NbActivePairs + 1);
			m_Mask = m_HashSize - 1;

			BB_FREE(m_HashTable);
			m_HashTable = (uint32*) Memory::Malloc( m_HashSize * sizeof(uint32) );
			Memory::Memset( m_HashTable, 0xFF, m_HashSize * sizeof(uint32) );

			// Get some bytes for new entries
			Pair* NewPairs	= new Pair[m_HashSize];/* (Pair*) Memory::Malloc( m_HashSize * sizeof(Pair) );*/		ASSERT(NewPairs);
			uint32* NewNext	= (uint32*) Memory::Malloc( m_HashSize * sizeof(uint32) );	ASSERT(NewNext);

			// Copy old data if needed
			for( uint32 i = 0; i < m_NbActivePairs; ++i )
				NewPairs[i] = m_Pairs[i];

			for( uint32 i=0; i < m_NbActivePairs; i++ )
			{
				uint32 HashValue = ((Hash<K> const &)*this)(m_Pairs[i].Key) & m_Mask;
				m_NextTable[i] = m_HashTable[HashValue];
				m_HashTable[HashValue] = i;
			}

			// Delete old data
			BB_FREE(m_NextTable);
			BB_DELETE_ARRAY(m_Pairs);

			// Assign new pointer
			m_Pairs = NewPairs;
			m_NextTable = NewNext;

			// Recompute hash value with new hash size
			HashValue = ((Hash<K> const &)*this)(Key) & m_Mask;
		}

		Pair* p = &m_Pairs[m_NbActivePairs];
		p->Key = Key;
		p->Value = Value;

		m_NextTable[m_NbActivePairs] = m_HashTable[HashValue];
		m_HashTable[HashValue] = m_NbActivePairs++;

		return p;
	}

	inline uint32 GetPairIndex(const Pair* pair) const
	{
		return ((uint32)((size_t(pair) - size_t(m_Pairs))) / sizeof(Pair));
	}

	bool Remove( K const& Key )
	{
		const uint32 HashValue = ((Hash<K> const &)*this)(Key) & m_Mask;
		const Pair* P = Find( Key, HashValue );
		if(!P)	return false;
		ASSERT(P->Key==Key);

		const uint32 PairIndex = GetPairIndex(P);

		// Walk the hash table to fix m_NextTable
		uint32 Offset = m_HashTable[HashValue];
		ASSERT(Offset!=INDEX_NONE);

		uint32 Previous=INDEX_NONE;
		while(Offset!=PairIndex)
		{
			Previous = Offset;
			Offset = m_NextTable[Offset];
		}

		// Let us go/jump us
		if(Previous!=INDEX_NONE)
		{
			ASSERT(m_NextTable[Previous]==PairIndex);
			m_NextTable[Previous] = m_NextTable[PairIndex];
		}
		// else we were the first
		else 
			m_HashTable[HashValue] = m_NextTable[PairIndex];
		// we're now free to reuse next[pairIndex] without breaking the list

#ifdef _DEBUG
		m_NextTable[PairIndex]=INDEX_NONE;
#endif

		// Fill holes
		if(1)
		{
			// 1) Remove last pair
			const uint32 LastPairIndex = m_NbActivePairs-1;
			if(LastPairIndex==PairIndex)
			{
				m_NbActivePairs--;
				return true;
			}

			const Pair* Last = &m_Pairs[LastPairIndex];
			const uint32 LastHashValue = ((Hash<K> const &)*this)(Last->Key) & m_Mask;

			// Walk the hash table to fix m_NextTable
			uint32 Offset = m_HashTable[LastHashValue];
			ASSERT(Offset!=INDEX_NONE);

			uint32 Previous=INDEX_NONE;
			while(Offset!=LastPairIndex)
			{
				Previous = Offset;
				Offset = m_NextTable[Offset];
			}

			// Let us go/jump us
			if(Previous!=INDEX_NONE)
			{
				ASSERT(m_NextTable[Previous]==LastPairIndex);
				m_NextTable[Previous] = m_NextTable[LastPairIndex];
			}
			// else we were the first
			else m_HashTable[LastHashValue] = m_NextTable[LastPairIndex];
			// we're now free to reuse m_NextTable[LastPairIndex] without breaking the list

#ifdef _DEBUG
			m_NextTable[LastPairIndex]=INDEX_NONE;
#endif

			// Don't invalidate entry since we're going to shrink the array

			// 2) Re-insert in free slot
			m_Pairs[PairIndex] = m_Pairs[LastPairIndex];
#ifdef _DEBUG
			ASSERT(m_NextTable[PairIndex]==INDEX_NONE);
#endif
			m_NextTable[PairIndex] = m_HashTable[LastHashValue];
			m_HashTable[LastHashValue] = PairIndex;

			mNbActivePairs--;
		}
		return true;
	}

public:
	Pair*			m_Pairs;
	uint32*			m_HashTable;
	uint32*			m_NextTable;
	uint32			m_HashSize;
	uint32			m_Mask;
	uint32			m_NbActivePairs;

};

} /* namespace bigball */



#if 0

    /* If E is different from K, Hash<K> must implement operator()(E const&)
     * and an equality operator between K and E must exist in order to use
     * this method. */

    /* I choose to make this inline because passing the key by reference
     * is usually suboptimal. */
    template <typename E>
    inline V const& operator[] (E const &key) const
    {
        /* Look for the hash in our table and return the value. */
        int i = FindIndex(key);
        ASSERT(i >= 0, "trying to read a nonexistent key in map");
        return m_array[i].m3;
    }

    template <typename E>
    inline V & operator[] (E const &key)
    {
        /* Look for the hash in our table and return the value if found. */
        uint32_t hash = ((Hash<K> const &)*this)(key);
        int i = FindIndex(key, hash);
        if (i >= 0)
            return m_array[i].m3;

        /* If not found, insert a new value. */
        m_array.Push(hash, key, V());
        return m_array.Last().m3;
    }

    template <typename E>
    inline void Remove(E const &key)
    {
        int i = FindIndex(key);
        if (i >= 0)
            m_array.Remove(i);
    }

    template <typename E>
    inline bool HasKey(E const &key)
    {
        return FindIndex(key) >= 0;
    }

    template <typename E>
    inline bool TryGetValue(E const &key, V& value)
    {
        int i = FindIndex(key);
        if (i >= 0)
        {
            value = m_array[i].m3;
            return true;
        }

        return false;
    }

    inline int Count() const
    {
        return m_array.Count();
    }

private:
    template <typename E>
    inline int FindIndex(E const &key, uint32_t hash)
    {
        for (int i = 0; i < m_array.Count(); ++i)
            if (m_array[i].m1 == hash)
                if (m_array[i].m2 == key)
                    return i;
        return -1;
    }

    template <typename E>
    inline int FindIndex(E const &key)
    {
        uint32_t hash = ((Hash<K> const &)*this)(key);
        return FindIndex(key, hash);
    }

    Array<uint32_t, K, V> m_array;

#endif

#endif // BB_COREMAP_H

