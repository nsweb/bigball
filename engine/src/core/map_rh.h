
#ifndef BB_COREMAP_RH_H
#define BB_COREMAP_RH_H

#include "hash.h"

namespace bigball
{

/** Implementation of robin hood hashing */
template<typename K, typename V> 
class MapRH : protected Hash<K>
{
public:
	struct Pair
	{
		K Key;
		V Value;
	};

	MapRH() :
		m_Pairs(nullptr),
		m_HashTable(nullptr),
		//m_NextTable(nullptr),
		m_HashSize(0),
		m_Mask(0),
		m_NbActivePairs(0)
	{

	}
	~MapRH()
	{
		BB_DELETE_ARRAY(m_Pairs);
		BB_FREE(m_HashTable);
		//BB_FREE(m_NextTable);
		m_HashSize = 0;
		m_Mask = 0;
		m_NbActivePairs = 0;
	}

	MapRH( MapRH&& Other) : 
		m_Pairs(Other.m_Pairs),
		m_HashTable(Other.m_HashTable),
		m_HashSize(Other.m_HashSize),
		m_Mask(Other.m_Mask),
		m_NbActivePairs(Other.m_NbActivePairs)	
	{
	}

	MapRH( MapRH const& Other) : 
		m_HashSize(Other.m_HashSize),
		m_Mask(Other.m_Mask),
		m_NbActivePairs(Other.m_NbActivePairs)	

	{
		CopyHashPairs( Other );
	}

	MapRH& operator=( MapRH&& Other )		
	{ 
		m_Pairs = Other.m_Pairs;
		m_HashTable = Other.m_HashTable;
		m_HashSize = Other.m_HashSize;
		m_Mask = Other.m_Mask;
		m_NbActivePairs = Other.m_NbActivePairs;
		return *this; 
	}

	MapRH& operator=( MapRH const& Other ) 
	{
		m_HashSize = Other.m_HashSize;		
		m_Mask = Other.m_Mask;
		m_NbActivePairs = Other.m_NbActivePairs;

		BB_DELETE_ARRAY(m_Pairs);
		BB_FREE(m_HashTable);
		CopyHashPairs( Other );
		return *this; 
	}

	void CopyHashPairs( MapRH const& Other )
	{
		if( Other.m_HashSize )
		{
			m_Pairs	= new Pair[Other.m_HashSize];
			// Cannot memcpy here, can Pair is not necessarily POD
			for( uint32 i = 0; i < Other.m_HashSize; ++i )
				m_Pairs[i] = Other.m_Pairs[i];

			m_HashTable = (uint32*) Memory::Malloc( Other.m_HashSize * sizeof(uint32) );
			Memory::Memcpy( m_HashTable, Other.m_HashTable, sizeof(uint32) );
		}
		else
		{
			m_Pairs = nullptr;
			m_HashTable = nullptr;
		}
	}

	uint32 hash_key( K const& Key ) const
	{			
		uint32 HashValue = ((Hash<K> const &)*this)(Key);
 
		// MSB is used to indicate a deleted elem, so
		// clear it
		HashValue &= 0x7fffffff;
 
		// Ensure that we never return 0 as a hash,
		// since we use 0 to indicate that the elem has never
		// been used at all.
		HashValue |= HashValue==0;
		return HashValue; 
	}

	static bool is_deleted( uint32 HashValue )
	{
		// MSB set indicates that this hash is a "tombstone"
		return (HashValue >> 31) != 0;
	}
 
	int probe_distance(uint32 HashValue, uint32 slot_index) const
	{	
		return (slot_index + m_HashSize - desired_pos(HashValue)) & m_Mask;
	}

	uint32 desired_pos( uint32 HashValue ) const
	{
		return HashValue & m_Mask;
	}
 
	uint32& elem_hash(uint32 ix)
	{
		return m_HashTable[ix];
	}
	uint32 elem_hash(uint32 ix) const
	{
		return m_HashTable[ix];
	}

	void reserve( uint32 HashSize )
	{
		if( HashSize > m_HashSize )
		{
			// Get more entries
			uint32 OldHashSize = m_HashSize;
			Pair* OldPairs = m_Pairs;
			uint32* OldHashTable = m_HashTable;

			m_HashSize = (bigball::IsPowerOfTwo(HashSize) ? HashSize : bigball::NextPowerOfTwo(HashSize));
			m_Mask = m_HashSize - 1;

			m_HashTable = (uint32*) Memory::Malloc( m_HashSize * sizeof(uint32) );
			Memory::Memset( m_HashTable, 0, m_HashSize * sizeof(uint32) );			// flag all new elems as free

			m_Pairs	= new Pair[m_HashSize];/* (Pair*) Memory::Malloc( m_HashSize * sizeof(Pair) );*/


			// Reinsert old data
			for( uint32 i = 0; i < OldHashSize; ++i )
			{
				auto& e = OldPairs[i];
				uint32 HashValue = OldHashTable[i];

				if( HashValue != 0 && !is_deleted(HashValue) )
				{
					insert_helper( std::move(e.Key), std::move(e.Value), HashValue );
					e.~Pair();
				}
			}

			BB_DELETE_ARRAY(OldPairs);
			BB_FREE(OldHashTable);
		}
	}

	/** Call this function to optimize allocated mem, when map is static and will not receive new elements */
	void optimize()
	{
		uint32 OldHashSize = m_HashSize;
		Pair* OldPairs = m_Pairs;
		uint32* OldHashTable = m_HashTable;

		m_HashSize = (m_NbActivePairs * 10) / 9;
	}

	const Pair*	Find( K const& Key ) const
	{
		//if( !m_HashTable )	return nullptr;	// Nothing has been allocated yet

		// Compute hash value for this key
		uint32 HashValue = hash_key( Key );
		return Find( Key, HashValue );
	}

	Pair* Find( K const& Key, uint32 HashValue ) const
	{
		//if( !m_HashTable )	return nullptr;	// Nothing has been allocated yet

		// Look for it in the table
		uint32 pos = desired_pos(HashValue);
		int32 dist = 0;
		for(;;)
		{							
			if( elem_hash(pos) == 0 ) 
				return nullptr;
			else if (dist > probe_distance(elem_hash(pos), pos)) 
				return nullptr;
			else if (elem_hash(pos) == HashValue && m_Pairs[pos].Key == Key) 
				return &m_Pairs[pos];				

			pos = (pos+1) & m_Mask;
			++dist;
		}
	}

	Pair* Add( K Key, V Value )
	{
		uint32 HashValue = hash_key( Key );
		Pair* P = Find(Key, HashValue);
		if(P)
			return P;	// Persistent pair

		if( ++m_NbActivePairs > (m_HashSize*9) / 10 )
			reserve( m_HashSize + 1 );

		return insert_helper( std::move(Key), std::move(Value), HashValue );
	}

	Pair* insert_helper( K&& Key, V&& Value, uint32 HashValue )
	{
		// This is a new pair
		uint32 pos = desired_pos(HashValue);
		int32 dist = 0;
		for(;;)
		{			
			if(elem_hash(pos) == 0)
			{			
				m_Pairs[pos].Key = Key;
				m_Pairs[pos].Value = Value;
				m_HashTable[pos] = HashValue;
				return &m_Pairs[pos];
			}

			// If the existing elem has probed less than us, then swap places with existing
			// elem, and keep going to find another slot for that elem.
			int existing_elem_probe_dist = probe_distance(elem_hash(pos), pos);
			if (existing_elem_probe_dist < dist)
			{	
				if(is_deleted(elem_hash(pos)))
				{
					m_Pairs[pos].Key = Key;
					m_Pairs[pos].Value = Value;
					m_HashTable[pos] = HashValue;
					return &m_Pairs[pos];
				}

				//uint32 hash_temp = elem_hash(pos);
				//HashValue = hash_temp;
				//elem_hash(pos) = hash_temp;
				std::swap(HashValue, elem_hash(pos));
				std::swap(Key, m_Pairs[pos].Key);
				std::swap(Value, m_Pairs[pos].Value);
				dist = existing_elem_probe_dist;				
			}

			pos = (pos+1) & m_Mask;
			++dist;			
		}
	}

	inline uint32 GetPairIndex(const Pair* pair) const
	{
		return ((uint32)((size_t(pair) - size_t(m_Pairs))) / sizeof(Pair));
	}

	bool Remove( K const& Key )
	{
		const uint32 HashValue = hash_key( Key );
		const Pair* P = Find( Key, HashValue );
		if(!P)	
			return false;
		BB_ASSERT(P->Key==Key);
		uint32 idx = GetPairIndex(P);

		m_Pairs[idx].~Pair();
		elem_hash(idx) |= 0x80000000; // mark as deleted
		--m_NbActivePairs;

		return true;
	}

#if 0
	void Serialize( File* DataFile )
	{
		if( DataFile->IsReading() )
		{
			uint32 NewHashSize = 0;
			DataFile->Serialize( NewHashSize );
			reserve( NewHashSize );
		}
		else
		{
			DataFile->Serialize( m_HashSize );
		}

		DataFile->Serialize( m_NbActivePairs );
		DataFile->Serialize( m_Pairs, sizeof(Pair) * m_NbActivePairs );
		DataFile->Serialize( m_HashTable, sizeof(uint32) * m_HashSize );
		//DataFile->Serialize( m_NextTable, sizeof(uint32) * m_HashSize );
	}
#endif

	uint32 GetReservedSize() const			{ return m_HashSize;			}
	uint32 GetActivePairCount() const		{ return m_NbActivePairs;		}
	Pair* GetPairAt( uint32 Index ) const 	{ BB_ASSERT(Index >= 0 && Index < m_NbActivePairs); return &m_Pairs[Index]; }
	bool IsPairValid( uint32 Index ) const 	
	{ 
		BB_ASSERT(Index >= 0 && Index < m_NbActivePairs); 
		uint32 HashValue = m_HashTable[Index]; 
		return ( HashValue != 0 && !is_deleted(HashValue) );
	}

public:
	Pair*			m_Pairs;
	uint32*			m_HashTable;
	//uint32*			m_NextTable;
	uint32			m_HashSize;
	uint32			m_Mask;
	uint32			m_NbActivePairs;

};

} /* namespace bigball */


#endif // BB_COREMAP_RH_H

