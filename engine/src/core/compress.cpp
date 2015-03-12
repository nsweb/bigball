
#include "../bigball.h"
#include "compress.h"

namespace Compression
{

/** Compress a buffer using zlib, will fail if CompressedBuffer is too small */
bool CompressZLIB( void* CompressedBuffer, uint32& CompressedSize, const void* UncompressedBuffer, uint32 UncompressedSize )
{
	// zlib uses unsigned long
	unsigned long ZCompressedSize = CompressedSize;
	unsigned long ZUncompressedSize	= UncompressedSize;
	// Compress data
	bool bOperationSucceeded = compress( (BYTE*) CompressedBuffer, &ZCompressedSize, (const BYTE*) UncompressedBuffer, ZUncompressedSize ) == Z_OK ? true : false;
	CompressedSize = ZCompressedSize;

	return bOperationSucceeded;
}

/** Uncompress a buffer using zlib, will fail if UncompressedBuffer is too small */
bool UncompressZLIB( void* UncompressedBuffer, uint32 UncompressedSize, const void* CompressedBuffer, uint32 CompressedSize )
{
	// zlib uses unsigned long
	unsigned long ZCompressedSize = CompressedSize;
	unsigned long ZUncompressedSize	= UncompressedSize;
	
	// Uncompress data.
	bool bOperationSucceeded = uncompress( (BYTE*) UncompressedBuffer, &ZUncompressedSize, (const BYTE*) CompressedBuffer, ZCompressedSize ) == Z_OK ? true : false;

	// Sanity check to make sure we uncompressed as much data as we expected to.
	BB_ASSERT( UncompressedSize == ZUncompressedSize );
	return bOperationSucceeded;
}

} // namespace Compression