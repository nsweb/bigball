

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


bool Object::ParseFile( char const* path )
{
	File js_file;
	if( js_file.Open( path, false /*bWrite*/, false ) )
	{
		String file_buffer;
		js_file.SerializeString( file_buffer );
		
		return ParseString( file_buffer.c_str() );
	}

	return false;
}

bool Object::ParseString( char const* sz )
{
	m_str = sz;

	jsmn_parser p;
	int32 max_token = 128;
	while( 1 )
	{
		max_token <<= 1;
		m_tokens.resize( max_token );

		jsmn_init(&p);
		int r = jsmn_parse(&p, sz, m_str.Len(), m_tokens.Data(), max_token);
		if( r >= 0 )
			break;
		else if( r == JSMN_ERROR_NOMEM )
			continue;
		else
			return false;
	}

	return true;
}

TokenIdx Object::GetToken( char const* sz, TokenType type, TokenIdx parent_idx /*= INDEX_NONE */ )
{
	if( parent_idx >= m_tokens.size() )
		return INDEX_NONE;

	TokenIdx end_token_idx = (parent_idx >= 0 ? parent_idx + m_tokens[parent_idx].size : m_tokens[0].size);
	TokenIdx token_idx = parent_idx + 1;
	while( token_idx <= end_token_idx )
	{
		if( IsStringValue( token_idx, sz ) )
		{
			if( token_idx + 1 < m_tokens.size() && m_tokens[token_idx+1].type == type )
			{
				return token_idx+1;
			}
		}
		end_token_idx += m_tokens[token_idx].size;
		token_idx += 1;
	}

	return INDEX_NONE;
}

bool Object::GetStringValue( TokenIdx token_idx, String& val )
{
	BB_ASSERT( token_idx >= 0 && token_idx < m_tokens.size() );
	if( m_tokens[token_idx].type == JSMN_STRING )
	{
		val = String( m_str.c_str() + m_tokens[token_idx].start, m_tokens[token_idx].end - m_tokens[token_idx].start );
		return true;
	}
	return false;
}

bool Object::IsStringValue( TokenIdx token_idx, char const* sz )
{
	BB_ASSERT( token_idx >= 0 && token_idx < m_tokens.size() );
	if( m_tokens[token_idx].type == JSMN_STRING )
	{
		int sz_len = (int)strlen(sz);
		if( m_tokens[token_idx].end - m_tokens[token_idx].start == sz_len )
		{
			char const* szToken = m_str.c_str() + m_tokens[token_idx].start;
			return Memory::Memcmp( szToken, sz, sz_len ) == 0;
		}
	}
	return false;
}

float Object::GetFloatValue( TokenIdx token_idx, float default_value )
{
	BB_ASSERT( token_idx >= 0 && token_idx < m_tokens.size() );
	if( m_tokens[token_idx].type == JSMN_PRIMITIVE )
	{
		String strVal( m_str.c_str() + m_tokens[token_idx].start, m_tokens[token_idx].end - m_tokens[token_idx].start );
		float Val = (float)std::atof( strVal.c_str() );
		return Val;
	}
	return default_value;
}

} /* namespace json */

} /* namespace bigball */