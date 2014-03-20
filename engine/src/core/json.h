

#ifndef BB_JSON_H
#define BB_JSON_H

namespace bigball
{

namespace json
{
typedef int32 TokenIdx;

enum TokenType
{
	PRIMITIVE = 0,
	OBJECT = 1,
	ARRAY = 2,
	STRING = 3
};

class Object
{
public:
	Object()	{}
	~Object()	{}
	bool		ParseFile( char const* Path );
	bool		ParseString( char const* sz );
	TokenIdx	GetToken( char const* sz, TokenType Type, TokenIdx ParentIdx = INDEX_NONE );
	bool		GetStringValue( TokenIdx Idx, String& Val );
	bool		IsStringValue( TokenIdx Idx, char const* sz );
	float		GetFloatValue( TokenIdx Idx, float DefaultValue = 0.f );

private:
	Array<jsmntok_t> m_Tokens;
	String			 m_Str;
};
	

} /* namespace json */

} /* namespace bigball */

#endif // BB_JSON_H
