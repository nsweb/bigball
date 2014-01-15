


#ifndef BB_CORENAME_H
#define BB_CORENAME_H


namespace bigball
{

class BIGBALL_API Name
{
private:

public:
    inline Name()
    {
        
    }

    inline Name(char const *str)
    {
        
    }

    inline Name( String const& str )
    {

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

