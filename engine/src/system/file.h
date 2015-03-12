
#ifndef BB_SYSTEMFILE_H
#define BB_SYSTEMFILE_H 


namespace bigball
{

#define ArchiveFlag_Read		(0x00000001)
#define ArchiveFlag_Write		(0x00000002)
#define ArchiveFlag_Compressed  (0x00000004)


/////////////////////////////////////////////////////////////////////
class Archive
{
public:
						Archive() : m_Flags(0)  {}
						~Archive() {}

	template<typename K>
	uint32				SerializeRaw( K& Val )			{ return Serialize( &Val, sizeof(K) );		}
	virtual uint32		Serialize( void* pBuffer, uint32 Size ) = 0;
	uint32				IsReading()		{ return m_Flags & ArchiveFlag_Read; }
	uint32				IsWriting() 	{ return m_Flags & ArchiveFlag_Write; }
	virtual void		Seek( uint32 Offset ) = 0;
	virtual uint32		Tell() = 0;

	uint32			m_Flags;
};

/////////////////////////////////////////////////////////////////////
class MemoryArchive : public Archive
{
public:
	MemoryArchive() : m_Offset(0)		{}
	~MemoryArchive()					{}

	void	ResizeData( uint32 Size)	{ m_Data.resize(Size);	}
	BYTE*	Data()						
	{ 
		BB_ASSERT( m_Data.size() > 0 ); 
		return m_Data.Data();	
	}
	virtual void		Seek( uint32 Offset )	
	{ 
		BB_ASSERT( Offset <= (uint32)m_Data.size() ); 
		m_Offset = Offset; 
	}
	virtual uint32		Tell()			{ return m_Offset;		}
	uint32	Size()						{ return (uint32)m_Data.size();	}

protected:
	Array<BYTE>		m_Data;
	uint32			m_Offset;
};

/////////////////////////////////////////////////////////////////////
class MemoryReader : public MemoryArchive
{
public:
	MemoryReader()						{ m_Flags |= ArchiveFlag_Read; }
	~MemoryReader()						{}

	virtual uint32		Serialize( void* pBuffer, uint32 Size );
};

/////////////////////////////////////////////////////////////////////
class MemoryWriter : public MemoryArchive
{
public:
						MemoryWriter()  { m_Flags |= ArchiveFlag_Write; }
						~MemoryWriter() {}

	virtual uint32		Serialize( void* pBuffer, uint32 Size );
};

/////////////////////////////////////////////////////////////////////
class File : public Archive
{
public:
						File();
						~File();

	bool				Open( char const* FileName, bool bWriteAccess, bool bAsync = false );
	void				Close();
	size_t				GetFileSize();
	bool				IsValidHandle();
	bool				HasAsyncIOCompleted();

	virtual uint32		Serialize( void* pBuffer, uint32 Size );
	uint32				SerializeString( String& BufferStr );
	bool				SerializeAsync( void* pBuffer, uint32 Size );
	virtual void		Seek( uint32 Offset );
	virtual uint32		Tell();

private:
	bool			m_bAsync;

#if _WIN32 || _WIN64
	HANDLE			m_FileHandle;
	OVERLAPPED		m_Overlapped;
#else
	FILE*			m_FileHandle;
#endif
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
struct RGBColor
{
	uint8 r;
	union
	{
		struct 
		{
			uint8 g;
			uint8 b;
		};
		uint16 gb;
	};
};

#if _WIN32 || _WIN64
#pragma pack(pop)
#endif



}  /* namespace bigball */


#endif // BB_SYSTEMFILE_H