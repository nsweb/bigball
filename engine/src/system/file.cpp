
#include "../bigball.h"
#include "file.h"

namespace bigball
{

File::File() :
	m_FileHandle(nullptr),
	m_bAsync(false)
{


	//m_pRegisterLink = NULL;
}

File::~File()
{
	//SAFE_DELETE( m_pRegisterLink );
	Close();
}

bool File::IsValidHandle()
{
#if _WIN32 || _WIN64
	return (m_FileHandle != nullptr && m_FileHandle != INVALID_HANDLE_VALUE);
#else
	return (m_FileHandle != nullptr);
#endif
}

bool File::Open( char const* FileName, bool bWriteAccess, bool bAsync )
{
	m_Flags |= bWriteAccess ? ArchiveFlag_Write : ArchiveFlag_Read;
	m_Flags &= bWriteAccess ? ~ArchiveFlag_Read : ~ArchiveFlag_Write;

	m_bAsync = bAsync;

#if _WIN32 || _WIN64
	uint32 Flags = FILE_ATTRIBUTE_NORMAL;
	if( bAsync )
		Flags |= FILE_FLAG_OVERLAPPED;

	if( IsReading() )
		m_FileHandle = CreateFile( FileName, GENERIC_READ, 0/*FILE_SHARE_READ*/, nullptr, OPEN_EXISTING, Flags, nullptr );
	else
		m_FileHandle = CreateFile( FileName, GENERIC_WRITE, 0/*FILE_SHARE_WRITE*/, nullptr, CREATE_ALWAYS, Flags, nullptr );

	if( !m_FileHandle || m_FileHandle == INVALID_HANDLE_VALUE )
		return false;
#else
	m_FileHandle = fopen( FileName, IsReading() ? "rb" : "wb" );
	if( !m_FileHandle )
		return false;
#endif

	//if( _bUseLinkTable && _eAccess == eAccessRead )
	//	m_pRegisterLink	= new HashMapType_LT;

	return true;
}

//---------------------------------------------------------------------------------
void File::Close()
{
#if _WIN32 || _WIN64
	if( m_FileHandle && m_FileHandle != INVALID_HANDLE_VALUE )
		CloseHandle( m_FileHandle );
#else
	if( m_FileHandle )
		fclose( m_FileHandle );
#endif
	m_FileHandle = nullptr;
}

void File::Seek( uint32 Offset )
{
#if _WIN32 || _WIN64
	// Try to move hFile file pointer some distance  
	DWORD dwPtr = SetFilePointer( m_FileHandle, Offset,	NULL, FILE_BEGIN ); 
	if( dwPtr == INVALID_SET_FILE_POINTER )
	{ 
		// Obtain the error code. 
		DWORD dwError = GetLastError() ; 
	}
#else
	fseek( m_FileHandle, Offset, SEEK_SET );
#endif
}

uint32 File::Tell()
{
#if _WIN32 || _WIN64
	// Crappy, but there is no GetFilePointer in Win32 API
	return SetFilePointer( m_FileHandle, 0, NULL, FILE_CURRENT );
#else
	return ftell( m_FileHandle );
#endif
}

uint32 File::Serialize( void* pBuffer, uint32 Size )
{
	int SerializedCount = 0;
	if( IsReading() )
	{
#if _WIN32 || _WIN64
		ReadFile( m_FileHandle, pBuffer, Size, (LPDWORD)&SerializedCount, nullptr );
#else
		SerializedCount = fread( pBuffer, 1, Size, m_FileHandle );
#endif
	}
	else
	{
#if _WIN32 || _WIN64
		if( !WriteFile( m_FileHandle, pBuffer, Size, (LPDWORD)&SerializedCount, nullptr ) )
		{
			LPVOID lpMsgBuf;
			DWORD dw = GetLastError(); 

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0, NULL );
		}
#else
		SerializedCount = fwrite( pBuffer, 1, Size, m_FileHandle );
#endif
	}
	return SerializedCount;
}

uint32 File::SerializeString( String& BufferStr )
{
	uint32 SerializedCount = 0;
	if( IsReading() )
	{
		BufferStr = "";
		char Buffer[256];
		uint32 SizeRead = 0;

		while( (SizeRead = Serialize( Buffer, sizeof(Buffer) )) > 0 )
		{
			int OldLen = BufferStr.Len();
			BufferStr.resize( OldLen + SizeRead );
			Memory::Memcpy( &BufferStr[OldLen], Buffer, SizeRead );
		}
		SerializedCount = BufferStr.Len();
	}
	else
	{
		SerializedCount = Serialize( BufferStr.c_str(), BufferStr.Len() );
	}

	return SerializedCount;
}

bool File::SerializeAsync( void* pBuffer, uint32 Size )
{
#if _WIN32 || _WIN64
	BOOL Result = 1;
	uint32 SerializedCount = 0;
	Memory::Memzero( &m_Overlapped, sizeof(m_Overlapped) );
	if( IsReading() )
	{
		Result = ReadFile( m_FileHandle, pBuffer, Size, (LPDWORD)&SerializedCount, &m_Overlapped );
	}
	else
	{
		Result = WriteFile( m_FileHandle, pBuffer, Size, (LPDWORD)&SerializedCount, &m_Overlapped );
	}
	if( !Result )
	{
		uint32 ErrorCode = GetLastError();
		if( ErrorCode == ERROR_IO_PENDING )
			return true;

		return false;
	}
	return true;
#else
	return false;
#endif
}

bool File::HasAsyncIOCompleted()
{
#if _WIN32 || _WIN64
	return HasOverlappedIoCompleted( &m_Overlapped );
#else
	return false;
#endif
}

size_t File::GetFileSize()
{
	if( !IsValidHandle() )
		return 0;

	size_t Size = 0;
#if _WIN32 || _WIN64
	LARGE_INTEGER LInt;
	Memory::Memzero( &LInt, sizeof(LInt) );
	GetFileSizeEx( m_FileHandle, &LInt );
	Size = (size_t) LInt.QuadPart;
	//DWORD dwSize = GetFileSize( m_hFile, NULL );
#else
	size_t Pos = ftell(m_FileHandle);
	fseek( m_FileHandle, 0, SEEK_END );
	Size = ftell(m_FileHandle);
	fseek( m_FileHandle, Pos, SEEK_SET );
#endif

	return Size;
}
//
//u32	File::fprintf( const char *format, ... )
//{
//	DWORD nToWrite = 0;
//	DWORD nWritten = 0;
//	char buffer[512]; // 512 or the max string you can have... or you can implement memory allocation.
//
//	va_list arguments;
//	va_start(arguments, format);
//
//	vsprintf_s( buffer, sizeof(buffer), format, arguments );
//
//	nToWrite = (DWORD)strlen(buffer);
//	WriteFile( m_hFile, buffer, nToWrite, &nWritten, NULL );
//
//	va_end(arguments); 
//
//	return nWritten;
//
//}

/////////////////////////////////////////////////////////////////////////////////////
bool FileExits( char const* FileName )
{
#if _WIN32 || _WIN64
	WIN32_FIND_DATA FileInfo;
	HANDLE FindHandle = ::FindFirstFile( FileName, &FileInfo ) ;
	bool bFileFound = ( FindHandle == INVALID_HANDLE_VALUE ? false:true );
	::FindClose( FindHandle );

	return bFileFound;
#else
	if( FILE* file = fopen( FileName, "r" ) ) 
	{
		fclose(file);
		return true;
	} 
	else
		return false;
#endif
}

size_t FileSize( char const* FileName )
{
	size_t Size = 0;
#if _WIN32 || _WIN64
	HANDLE FileHandle = CreateFile( FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( FileHandle == INVALID_HANDLE_VALUE )
		return 0;

	LARGE_INTEGER LInt;
	Memory::Memzero( &LInt, sizeof(LInt) );
	GetFileSizeEx( FileHandle, &LInt );
	Size = (size_t) LInt.QuadPart;
	CloseHandle( FileHandle );
#else
	if( FILE* file = fopen( FileName, "r" ) ) 
	{
		fseek( file, 0, SEEK_SET );
		Size = ftell(file);
		fclose(file);
	} 
#endif

	return Size;
}

void ListFiles( char const* strSearch, Array<String>& OutFiles )
{
#if _WIN32 || _WIN64
	WIN32_FIND_DATA FileInfo;
	HANDLE hFind = FindFirstFile( strSearch, &FileInfo ) ;
	if( hFind == INVALID_HANDLE_VALUE )
		return;

	OutFiles.push_back( FileInfo.cFileName );

	while( FindNextFile( hFind, &FileInfo )  )
	{
		OutFiles.push_back( FileInfo.cFileName );
	}

	FindClose( hFind );
#endif
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

uint32 MemoryReader::Serialize( void* pBuffer, uint32 Size )
{
	// Ensure we have enough data
	if( m_Offset + Size <= (uint32)m_Data.size() )
	{
		Memory::Memcpy( pBuffer, &m_Data[m_Offset], Size );
		m_Offset += Size;
		return Size;
	}

	return 0;
}


uint32 MemoryWriter::Serialize( void* pBuffer, uint32 Size )
{
	uint32 SizeNeeded = m_Offset + Size;
	if( SizeNeeded > (uint32)m_Data.size() )
	{
		m_Data.resize(SizeNeeded );
	}
	if( Size )
	{
		Memory::Memcpy( &m_Data[m_Offset], pBuffer, Size );
		m_Offset += Size;
	}
	return Size;
}

} /* namespace bigball */
