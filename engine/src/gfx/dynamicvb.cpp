

#include "../bigball.h"
#include "dynamicvb.h"


namespace bigball
{

PersistentMappedVBO::PersistentMappedVBO() : 
	m_VB_ID(0),
	m_DestHead(0),
	m_BufferSize(0),
	m_VertexDataPtr(nullptr)
{

}


PersistentMappedVBO::~PersistentMappedVBO()
{

}


void PersistentMappedVBO::Init( uint32 MaxVertex, uint32 VertSizeInBytes )
{
	m_DestHead = 0;
	m_BufferSize = 3 * MaxVertex * VertSizeInBytes;	// Triple buffering

	GLbitfield MapFlags =	  GL_MAP_WRITE_BIT 
							| GL_MAP_PERSISTENT_BIT // keeped mapped while drawing
							| GL_MAP_COHERENT_BIT;	// writes automatically visible to GPU
	GLbitfield CreateFlags = MapFlags | GL_DYNAMIC_STORAGE_BIT;

	glGenBuffers( 1, &m_VB_ID);
	glBindBuffer( GL_ARRAY_BUFFER, m_VB_ID );
	glBufferStorage( GL_ARRAY_BUFFER, m_BufferSize, nullptr, CreateFlags );				// Create data store for immutable buffer object
	m_VertexDataPtr = (uint8*) glMapBufferRange( GL_ARRAY_BUFFER, 0, m_BufferSize, MapFlags );	// Mapped forever 
}

void PersistentMappedVBO::Cleanup()
{
	glBindBuffer( GL_ARRAY_BUFFER, m_VB_ID);
	glUnmapBuffer( GL_ARRAY_BUFFER );
	glDeleteBuffers( 1, &m_VB_ID );
	
	m_DestHead = 0;
	m_BufferSize = 0;
	m_VertexDataPtr = nullptr;

}

} /*namespace bigball*/