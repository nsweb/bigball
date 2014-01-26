
#include "../bigball.h"
#include "file.h"

namespace bigball
{

File::File() :
	m_AccessMode(eAccessMode::Read),
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

bool File::Open( char const* FileName, eAccessMode Access, bool bAsync )
{
	m_AccessMode = Access;
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
	if( m_FileHandle && m_FileHandle == INVALID_HANDLE_VALUE )
		CloseHandle( m_FileHandle );
#else
	if( m_FileHandle )
		fclose( m_FileHandle );
#endif
	m_FileHandle = nullptr;
}



//---------------------------------------------------------------------------------
//void File::Write_Link( void* pLink )
//{
//	DWORD nWritten = 0;
//	WriteFile( m_hFile, &pLink, sizeof(void*), &nWritten, NULL );
//}
//
////---------------------------------------------------------------------------------
//void File::Read_Link( void* pLink )
//{
//	DWORD nRead = 0;
//	void* Key = NULL;
//	ReadFile( m_hFile, &Key, sizeof(void*), &nRead, NULL );
//
//	//(*m_pRegisterLink)[Key] = pLink;
//}
//
////---------------------------------------------------------------------------------
//void* File::Get_Link( void* pLink )
//{
//	void* pDebug = NULL;
//
//	//HashMapType_LT::iterator iterHM = m_pRegisterLink->find( pLink );
//	//if( iterHM != m_pRegisterLink->end() )
//	//	pDebug = (void*) iterHM->second;
//
//	if( pLink && !pDebug )
//	{
//		//M_ERROR( E_FAILED, ("cLinkTable::Get_Link(): Probleme de link !") );
//	}
//
//	return pDebug;
//}

uint32 File::Serialize( void* pBuffer, uint32 Size )
{
	uint32 SerializedCount = 0;
	if( IsReading() )
	{
#if _WIN32 || _WIN64
		ReadFile( m_FileHandle, pBuffer, Size, &SerializedCount, nullptr );
#else
		SerializedCount = fread( pBuffer, Size, 1, m_FileHandle );
#endif
	}
	else
	{
#if _WIN32 || _WIN64
		WriteFile( m_FileHandle, pBuffer, Size, &SerializedCount, nullptr );
#else
		SerializedCount = fwrite( pBuffer, Size, 1, m_FileHandle );
#endif
	}
	return SerializedCount;
}

uint32 File::SerializeString( String& BufferStr )
{
	if( IsReading() )
	{
		char Buffer[256];
		while( (uint32 SizeRead = Serialize( Buffer, sizeof(Buffer) )) > 0 )
		{
			
		}
	}
	else
	{
		Serialize( BufferStr.
	}
}

bool File::SerializeAsync( void* pBuffer, uint32 Size )
{
#if _WIN32 || _WIN64
	BOOL Result = 1;
	uint32 SerializedCount = 0;
	Memory::Memzero( &m_Overlapped, sizeof(m_Overlapped) );
	if( IsReading() )
	{
		Result = ReadFile( m_hFile, pBuffer, Size, &SerializedCount, &m_Overlapped );
	}
	else
	{
		Result = WriteFile( m_hFile, pBuffer, Size, &SerializedCount, &m_Overlapped );
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
	bFileFound = ( FindHandle == INVALID_HANDLE_VALUE ? false:true );
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
	HANDLE FileHandle = CreateFile( _strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
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


///////////////////////////////////////////////////////////////////////////////////////
//void ListFiles( const LPWSTR _strSearchFile, vector<wstring>& _OutFiles )
//{
//	bool bFileFound = false;
//	WIN32_FIND_DATA FileInfo;
//	HANDLE hFind = ::FindFirstFile( _strSearchFile, &FileInfo ) ;
//	bFileFound = ( hFind == INVALID_HANDLE_VALUE ? false:true );
//	if( !bFileFound )
//		return;
//
//	_OutFiles.push_back( FileInfo.cFileName );
//
//	while( FindNextFile( hFind, &FileInfo )  )
//	{
//	  _OutFiles.push_back( FileInfo.cFileName );
//	}
//
//	::FindClose( hFind );
//
//	//return bFileFound;
//}


/////////////////////////////////////////////////////////////////////////////////////
//

//
////-----------------------------------------------------------------------------
//// Internal create function (uses fopen)
//bool File::Create(const LPWSTR _ustrFileName, const char* mode)
//{ // this function is private, no need for multi thread protection (ie faster)
//
//	m_hFile = ::fopen( _ustrFileName, mode );
//	/*if (m_hFile == NULL )
//	{
//		Filename = cFileFunc::BuildFilename(name);
//		// Try again...
//		m_hFile = ::fopen(Filename.c_str(), mode.c_str());
//	}
//	else
//	{
//		Filename = name;    // no need to call BuildFilename()
//	}
//
//#if defined(_DEBUG) || defined(_RELEASE_WITH_ASSERT)
//	m_Filename = Filename;
//#endif//_DEBUG*/
//
//	bool bFileIsOpened = (m_hFile != NULL);
//
//	if (bFileIsOpened == false)
//	{
//		//M_WARNING(E_FILE_CANT_OPEN_FILE, ("can't open file %s", Filename.c_str()) );
//		return false;
//	}
///*
//#pragma MESS("right now, we only support r or w (not both)")
//	if(mode.Find("r") != U32_ERROR)
//	{
//		m_eIOMode = E_IO_READ;
//		m_eSerializeMode = E_IO_READ;
//	}
//	else if(mode.Find("w") != U32_ERROR)
//	{
//		m_eIOMode = E_IO_WRITE;
//		m_eSerializeMode = E_IO_WRITE;
//	}*/
//
//	return true;
//}
//
////-----------------------------------------------------------------------------
//bool File::IsOpen() const 
//{ 
//	//M_LOCK();
//	bool bResult = (m_hFile != NULL);
//	return bResult;
//}
//
//
////-----------------------------------------------------------------------------
//bool File::Open(const LPWSTR _ustrFileName, const char* mode)
//{ 
//	if(m_hFile != NULL)
//		Close();
//
//	bool bResult = Create(name, mode);
//	return bResult;
//}
//
//
////-----------------------------------------------------------------------------
//void File::Close(void)                                        
//{
//	//M_LOCK();
//
//	if (m_hFile == NULL)
//		return;
//
///*#ifdef	_DEBUG
//	s32 iFileIsClosed = ::fclose(m_FileHandle);
//	M_ASSERT(iFileIsClosed == 0, ("%s", m_Filename.c_str()) );
//#else //_DEBUG*/
//	fclose(m_hFile);
////#endif//_DEBUG
//
//	m_hFile = NULL; // just set it to NULL so that a further open call would be OK
//
//	// tells cStreamIO we have closed the stream
//	/*SetSerializeMode(E_IO_NOT_OPENED);
//	m_eIOMode = E_IO_NOT_OPENED;*/
//}
/*
//-----------------------------------------------------------------------------
size_t File::Write(void* buffer,const size_t size, const size_t nb)   
{
	M_LOCK();
	size_t _SizeWritten = ::fwrite(buffer,size,nb,m_FileHandle);
	if(_SizeWritten != nb)
	{
#ifdef _DEBUG
		M_ERROR(E_FILE_CANT_WRITE, ("%s: size written = %d, size asked = %d", m_Filename.c_str(), _SizeWritten, nb) );
#else
		M_ERROR(E_FILE_CANT_WRITE, ("size written = %d, size asked = %d", _SizeWritten, nb) );
#endif
	}
	return _SizeWritten;
}


//-----------------------------------------------------------------------------
size_t File::Read(void* buffer,const size_t size,const size_t nb)          
{
	M_LOCK();

	size_t _SizeRead = ::fread(buffer,size,nb,m_FileHandle);
	// M_WARNING(_SizeRead == nb, ("%s: size read = %d, size asked = %d", m_Filename.c_str(), _SizeRead, nb) );
	return _SizeRead;
}


//-----------------------------------------------------------------------------
s32 File::Seek (const s32 _s32Offset, const E_MC_SEEK _eOrigin)
{
	M_LOCK();
	s32 iResult = ::fseek(m_FileHandle, _s32Offset, _eOrigin);
	return iResult;
}

//-----------------------------------------------------------------------------
s32 File::Tell(void)
{
	M_LOCK();
	s32 iResult = ::ftell(m_FileHandle);
	return iResult;
}


//---------------------------------------------------
size_t File::GetFileSize()
{
	if (m_FileHandle == NULL)
		return 0;

	M_LOCK();

	//TODO: A vérifier si le fichier est ouvert en mode texte !
	s32 OldPos = ::ftell(m_FileHandle);  // Saves the current file position

	if ( Seek(0, E_MC_SEEK_END) != 0 )
	{
		Seek(OldPos, E_MC_SEEK_BEGIN);  // Restores the current file position
		return 0;
	}
	s32 EndPos = ::ftell(m_FileHandle);
	M_ASSERT(EndPos != -1L, ("GetFileSize() error"));
	Seek(OldPos, E_MC_SEEK_BEGIN);  // Restores the current file position

	return EndPos;
}*/


//-----------------------------------------------------------------------------
//s32 File::fprintf(const char *format , ... )
//{
//	s32 SizeWritten = 0;
//	if(format)
//	{
//		va_list marker;
//
//		va_start( marker, format ); 
//		//M_LOCK();
//		SizeWritten = vfprintf(m_hFile, format, marker);
//		//M_ASSERT(SizeWritten>=0, ("%s: problem on fprintf", m_Filename.c_str()) );
//		va_end( marker );
//	}
//	return SizeWritten;
//}



/*mvcLinkTable::mvcLinkTable()
{
	m_pRegisterLink			= NULL;
}

//---------------------------------------------------------------------------------
mvcLinkTable::~mvcLinkTable()
{
	Destroy();
}

//---------------------------------------------------------------------------------
void mvcLinkTable::Write_Link( HANDLE hFile, void* pLink )
{
	DWORD nWritten = 0;
	WriteFile( hFile, &pLink, sizeof(void*), &nWritten, NULL );
}

//---------------------------------------------------------------------------------
void mvcLinkTable::Read_Link( HANDLE hFile, void* pLink )
{
	DWORD nRead = 0;
	void* Key = NULL;
	ReadFile( hFile, &Key, sizeof(void*), &nRead, NULL );

	(*m_pRegisterLink)[Key] = pLink;
}

//---------------------------------------------------------------------------------
void* mvcLinkTable::Get_Link( void* pLink )
{
	void* pDebug = NULL;//(void*) m_pRegisterLink->GetValueByKey( (u32) pLink );

	HashMapType_LT::iterator iterHM = m_pRegisterLink->find( pLink );
	if( iterHM != m_pRegisterLink->end() )
		pDebug = (void*) iterHM->second;

	if( pLink && !pDebug )
	{
		//M_ERROR( E_FAILED, ("cLinkTable::Get_Link(): Probleme de link !") );
	}

	return pDebug;
}

//---------------------------------------------------------------------------------
void mvcLinkTable::Create()
{
	SAFE_DELETE( m_pRegisterLink );
	m_pRegisterLink	= new HashMapType_LT;
}

//---------------------------------------------------------------------------------
void mvcLinkTable::Destroy()
{
	SAFE_DELETE( m_pRegisterLink );
}*/

} /* namespace bigball */
