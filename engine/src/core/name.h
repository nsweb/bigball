


#ifndef BB_CORENAME_H
#define BB_CORENAME_H

#include "map.h"

namespace bigball
{

typedef uint32 NameHandle;
typedef Map<String,NameHandle> NameMap;

class NameManager
{
private: 
	static NameManager* m_pStaticInstance; 
	NameManager();
	~NameManager();

public:
	Map<String,NameHandle> m_StringTable;
	uint32 m_GlobalHandle;

public:
	
	static void StaticInit();
	static NameManager* GetStaticInstance() { return m_pStaticInstance; }
	NameHandle FindOrAddHandle( String const& str );

};

class BIGBALL_API Name
{
private:

	NameHandle	m_Handle;
	uint32		m_Num;

public:
    inline Name()
    {
        m_Handle = 0;
		m_Num = INDEX_NONE;
    }

    inline Name(char const *str)
    {
        m_Handle = NameManager::GetStaticInstance()->FindOrAddHandle( String(str) );
		m_Num = INDEX_NONE;
    }

    inline Name( String const& str )
    {
		m_Handle = NameManager::GetStaticInstance()->FindOrAddHandle( str );
		m_Num = INDEX_NONE;
    }

    inline char const* c_str() const
    {
        return nullptr;
    }
	inline String ToString() const
	{
		return String();
	}

    inline bool operator ==( Name const& Other ) const
    {
        return false;
    }
};


} /* namespace bigball */

#endif // BB_CORENAME_H

