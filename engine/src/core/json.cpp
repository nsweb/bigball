

#include "../bigball.h"
#ifndef BB_JSON_H
#include "json.h"
#endif
#include "../system/file.h"


namespace bigball
{

namespace json
{
//void test()
//{
//	const char *js;
//	int r;
//	jsmn_parser p;
//	jsmntok_t t[10];
//
//	//js = "{}";
//	js = "{ \"entity\":	{ \"pattern\": \"Sun\",	\"toto\" : 3.0, \"Position\": {	} } }";
//
//	jsmn_init(&p);
//	r = jsmn_parse(&p, js, t, 10);
//	//check(r == JSMN_SUCCESS);
//	//check(t[0].type == JSMN_OBJECT);
//	//check(t[0].start == 0 && t[0].end == 2);
//}


bool Object::ParseFile( char const* Path )
{
	File jsFile;
	if( jsFile.Open( Path, false /*bWrite*/, false ) )
	{
		String FileBuffer;
		jsFile.SerializeString( FileBuffer );
		
		return ParseString( FileBuffer.c_str() );
	}

	return false;
}

bool Object::ParseString( char const* sz )
{
	m_Str = sz;

	jsmn_parser p;
	int32 MaxToken = 128;
	while( 1 )
	{
		MaxToken <<= 1;
		m_Tokens.resize( MaxToken );

		jsmn_init(&p);
		int r = jsmn_parse(&p, sz, m_Str.Len(), m_Tokens.Data(), MaxToken);
		if( r >= 0 )
			break;
		else if( r == JSMN_ERROR_NOMEM )
			continue;
		else
			return false;
	}

	return true;
}

TokenIdx Object::GetToken( char const* sz, TokenType Type, TokenIdx ParentIdx /*= INDEX_NONE */ )
{
	TokenIdx EndTokenIdx = (ParentIdx >= 0 && ParentIdx < m_Tokens.size() ? ParentIdx + m_Tokens[ParentIdx].size : m_Tokens.size());
	TokenIdx TokenIdx = ParentIdx + 1;
	while( TokenIdx < EndTokenIdx )
	{
		if( IsStringValue( TokenIdx, sz ) )
		{
			if( TokenIdx + 1 < m_Tokens.size() && m_Tokens[TokenIdx+1].type == Type )
			{
				return TokenIdx+1;
			}
		}
		TokenIdx += 1;
		EndTokenIdx += m_Tokens[TokenIdx].size;
	}

	return INDEX_NONE;
}

bool Object::GetStringValue( TokenIdx Idx, String& Val )
{
	BB_ASSERT( Idx >= 0 && Idx < m_Tokens.size() );
	if( m_Tokens[Idx].type == JSMN_STRING )
	{
		Val = String( m_Str.c_str() + m_Tokens[Idx].start, m_Tokens[Idx].end - m_Tokens[Idx].start );
		return true;
	}
	return false;
}

bool Object::IsStringValue( TokenIdx Idx, char const* sz )
{
	BB_ASSERT( Idx >= 0 && Idx < m_Tokens.size() );
	if( m_Tokens[Idx].type == JSMN_STRING )
	{
		int sz_len = (int)strlen(sz);
		if( m_Tokens[Idx].end - m_Tokens[Idx].start == sz_len )
		{
			char const* szToken = m_Str.c_str() + m_Tokens[Idx].start;
			return Memory::Memcmp( szToken, sz, sz_len ) == 0;
		}
	}
	return false;
}

float Object::GetFloatValue( TokenIdx Idx, float DefaultValue )
{
	BB_ASSERT( Idx >= 0 && Idx < m_Tokens.size() );
	if( m_Tokens[Idx].type == JSMN_PRIMITIVE )
	{
		String strVal( m_Str.c_str() + m_Tokens[Idx].start, m_Tokens[Idx].end - m_Tokens[Idx].start );
		float Val = (float)std::atof( strVal.c_str() );
		return Val;
	}
	return DefaultValue;
}

} /* namespace json */

} /* namespace bigball */