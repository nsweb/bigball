
#ifndef BB_SYSTEMFILE_H
#define BB_SYSTEMFILE_H 


/////////////////////////////////////////////////////////////////////
//typedef /*std::*/hash_map<void*, void*> HashMapType_LT;

namespace bigball
{

class File
{
public:
						File();
						~File();

	enum eAccessMode
	{
		Read = 0,
		Write,
	};

	bool				Open( char const* FileName, eAccessMode Access, bool bAsync = false );
	void				Close();
	size_t				GetFileSize();
	inline bool			IsReading()					{ return m_AccessMode == Read ? true:false;		}
	inline bool			IsWriting()					{ return m_AccessMode == Write ? true:false;	}
	bool				IsValidHandle();
	bool				HasAsyncIOCompleted();

	template<typename K>
	uint32				Serialize( K& Val )			{ return Serialize( &K, sizeof(K) );		}
	uint32				Serialize( void* pBuffer, uint32 Size );
	uint32				SerializeString( String& BufferStr );
	bool				SerializeAsync( void* pBuffer, uint32 Size );

private:
	eAccessMode		m_AccessMode;
	bool			m_bAsync;

#if _WIN32 || _WIN64
	HANDLE			m_FileHandle;
	OVERLAPPED		m_Overlapped;
#else
	FILE*			m_FileHandle;
#endif
	//HashMapType_LT*	m_pRegisterLink;
};

/////////////////////////////////////////////////////////////////////
BIGBALL_API bool FileExits( char const* FileName );
BIGBALL_API size_t FileSize( char const* FileName );
BIGBALL_API void ListFiles( char const* strSearch, Array<String>& OutFiles );


#if _WIN32 || _WIN64
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

struct TgaFileHeader
{
	uint8	identsize;          // size of ID field that follows 18 byte header (0 usually)
	uint8	colourmaptype;      // type of colour map 0=none, 1=has palette
	uint8	imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	int16	colourmapstart;     // first colour map entry in palette
	int16	colourmaplength;    // number of colours in palette
	uint8	colourmapbits;      // number of bits per palette entry 15,16,24,32

	int16	xstart;             // image x origin
	int16	ystart;             // image y origin
	int16	width;              // image width in pixels
	int16	height;             // image height in pixels
	uint8	bits;               // image bits per pixel 8,16,24,32
	uint8	descriptor;         // image descriptor bits (vh flip bits)

	// pixel data follows header

};

struct BGRColor
{
	uint8 b;
	union
	{
		struct 
		{
			uint8 g;
			uint8 r;
		};
		uint16 gr;
	};
};

#if _WIN32 || _WIN64
#pragma pack(pop)
#endif

	//wstring vswprintf(const wchar_t* format, va_list args);
	//wstring swprintf(const wchar_t* format, ...);

}  /* namespace bigball */


/*class mvcLinkTable
{
public:
					mvcLinkTable();
					~mvcLinkTable();

	void			Create();
	void			Destroy();

	void			Read_Link( HANDLE _hFile, void* _pLink );
	void			Write_Link( HANDLE _hFile, void* _pLink );
	void*			Get_Link( void* _pLink );

private:
	HashMapType_LT*	m_pRegisterLink;

};*/

#endif // BB_SYSTEMFILE_H