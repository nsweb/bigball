
#include "../bigball.h"
#include "file.h"

namespace bigball
{

File::File() :
	m_file_handle(nullptr),
	m_async(false)
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
	return (m_file_handle != nullptr && m_file_handle != INVALID_HANDLE_VALUE);
#else
	return (m_file_handle != nullptr);
#endif
}

bool File::Open( char const* FileName, bool bWriteAccess, bool bAsync )
{
	m_flags |= bWriteAccess ? ArchiveFlag_Write : ArchiveFlag_Read;
	m_flags &= bWriteAccess ? ~ArchiveFlag_Read : ~ArchiveFlag_Write;

	m_async = bAsync;

#if _WIN32 || _WIN64
	uint32 Flags = FILE_ATTRIBUTE_NORMAL;
	if( bAsync )
		Flags |= FILE_FLAG_OVERLAPPED;

	if( IsReading() )
		m_file_handle = CreateFile( FileName, GENERIC_READ, 0/*FILE_SHARE_READ*/, nullptr, OPEN_EXISTING, Flags, nullptr );
	else
		m_file_handle = CreateFile( FileName, GENERIC_WRITE, 0/*FILE_SHARE_WRITE*/, nullptr, CREATE_ALWAYS, Flags, nullptr );

	if( !m_file_handle || m_file_handle == INVALID_HANDLE_VALUE )
		return false;
#else
	m_file_handle = fopen( FileName, IsReading() ? "rb" : "wb" );
	if( !m_file_handle )
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
	if( m_file_handle && m_file_handle != INVALID_HANDLE_VALUE )
		CloseHandle( m_file_handle );
#else
	if( m_file_handle )
		fclose( m_file_handle );
#endif
	m_file_handle = nullptr;
}

void File::Seek( uint32 Offset )
{
#if _WIN32 || _WIN64
	// Try to move hFile file pointer some distance  
	DWORD dwPtr = SetFilePointer( m_file_handle, Offset,	NULL, FILE_BEGIN ); 
	if( dwPtr == INVALID_SET_FILE_POINTER )
	{ 
		// Obtain the error code. 
		DWORD dwError = GetLastError() ; 
	}
#else
	fseek( m_file_handle, Offset, SEEK_SET );
#endif
}

uint32 File::Tell()
{
#if _WIN32 || _WIN64
	// Crappy, but there is no GetFilePointer in Win32 API
	return SetFilePointer( m_file_handle, 0, NULL, FILE_CURRENT );
#else
	return ftell( m_file_handle );
#endif
}

uint32 File::Serialize( void* buffer, uint32 Size )
{
	int SerializedCount = 0;
	if( IsReading() )
	{
#if _WIN32 || _WIN64
		ReadFile( m_file_handle, buffer, Size, (LPDWORD)&SerializedCount, nullptr );
#else
		SerializedCount = fread( buffer, 1, Size, m_file_handle );
#endif
	}
	else
	{
#if _WIN32 || _WIN64
		if( !WriteFile( m_file_handle, buffer, Size, (LPDWORD)&SerializedCount, nullptr ) )
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
		SerializedCount = fwrite( buffer, 1, Size, m_file_handle );
#endif
	}
	return SerializedCount;
}

bool File::SerializeAsync( void* buffer, uint32 Size )
{
#if _WIN32 || _WIN64
	BOOL Result = 1;
	uint32 SerializedCount = 0;
	Memory::Memzero(&m_overlapped, sizeof(m_overlapped));
	if( IsReading() )
	{
		Result = ReadFile(m_file_handle, buffer, Size, (LPDWORD)&SerializedCount, &m_overlapped);
	}
	else
	{
		Result = WriteFile(m_file_handle, buffer, Size, (LPDWORD)&SerializedCount, &m_overlapped);
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
	return HasOverlappedIoCompleted( &m_overlapped );
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
	GetFileSizeEx( m_file_handle, &LInt );
	Size = (size_t) LInt.QuadPart;
	//DWORD dwSize = GetFileSize( m_hFile, NULL );
#else
	size_t Pos = ftell(m_file_handle);
	fseek( m_file_handle, 0, SEEK_END );
	Size = ftell(m_file_handle);
	fseek( m_file_handle, Pos, SEEK_SET );
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
namespace FileUtils
{
	bool FileExits(char const* FileName)
	{
#if _WIN32 || _WIN64
		WIN32_FIND_DATA FileInfo;
		HANDLE FindHandle = ::FindFirstFile(FileName, &FileInfo);
		bool bFileFound = (FindHandle == INVALID_HANDLE_VALUE ? false : true);
		::FindClose(FindHandle);

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

	size_t FileSize(char const* FileName)
	{
		size_t Size = 0;
#if _WIN32 || _WIN64
		HANDLE file_handle = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file_handle == INVALID_HANDLE_VALUE)
			return 0;

		LARGE_INTEGER LInt;
		Memory::Memzero(&LInt, sizeof(LInt));
		GetFileSizeEx(file_handle, &LInt);
		Size = (size_t)LInt.QuadPart;
		CloseHandle(file_handle);
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

	void ListFiles(char const* strSearch, Array<String>& OutFiles)
	{
#if _WIN32 || _WIN64
		WIN32_FIND_DATA file_info;
		HANDLE hFind = FindFirstFile(strSearch, &file_info);
		if (hFind == INVALID_HANDLE_VALUE)
			return;

		OutFiles.push_back(file_info.cFileName);

		while (FindNextFile(hFind, &file_info))
		{
			OutFiles.push_back(file_info.cFileName);
		}

		FindClose(hFind);
#endif
	}
} // namespace FileUtils



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

    
uint32 Archive::SerializeString( String& BufferStr )
{
    uint32 serialized_count = 0;
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
		serialized_count = BufferStr.Len();
    }
    else
    {
		serialized_count = Serialize(BufferStr.c_str(), BufferStr.Len());
    }
    
	return serialized_count;
}
    
uint32 MemoryReader::Serialize( void* buffer, uint32 size )
{
	// Ensure we have enough data
	if (m_offset + size <= (uint32)m_data.size())
	{
		Memory::Memcpy(buffer, &m_data[m_offset], size);
		m_offset += size;
		return size;
	}

	return 0;
}


uint32 MemoryWriter::Serialize(void* buffer, uint32 size)
{
	uint32 size_needed = m_offset + size;
	if (size_needed > (uint32)m_data.size())
	{
		m_data.resize(size_needed);
	}
	if (size)
	{
		Memory::Memcpy(&m_data[m_offset], buffer, size);
		m_offset += size;
	}
	return size;
}

} /* namespace bigball */
