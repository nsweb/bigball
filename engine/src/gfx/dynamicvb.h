


#ifndef BB_DYNAMICVB_H
#define BB_DYNAMICVB_H

//#include "../engine/basemanager.h"

namespace bigball
{


class BIGBALL_API UnsynchronizedVBO
{
public:
};

class BIGBALL_API PersistentMappedVBO
{
public:
						PersistentMappedVBO();
						~PersistentMappedVBO();

	void				Init( uint32 MaxVertex, uint32 VertSizeInBytes );
	void				Cleanup();
	void				Bind();
	void				Unbind();

	GLuint		m_VB_ID;
	uint32		m_DestHead;
	uint32		m_BufferSize;
	void*		m_VertexDataPtr;
};


} /* namespace bigball */

#endif // BB_DYNAMICVB_H