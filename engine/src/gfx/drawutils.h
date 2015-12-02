

#ifndef BB_DRAWUTILS_H
#define BB_DRAWUTILS_H

#include "../engine/basemanager.h"

namespace bigball
{

class BIGBALL_API Shader;

namespace Draw
{
	enum ShapeType
	{
		Box = 0,
        Count
	};

    struct Vertex
    {
        vec3	Pos;
        u8vec4	Col;
    };
	struct BufferRange
	{
        int Offset;
        int Count;
	};
}


////////////////////////////////////////////////////////////
class BIGBALL_API DrawUtils
{
	STATIC_MANAGER_H(DrawUtils)

public:
						DrawUtils();
	virtual				~DrawUtils();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& TickCtxt );
	virtual void		_Render( struct RenderContext& RenderCtxt );

	/* Functions for drawing utility shapes 
	 * PersistTime : amount of time the shape should be drawn ( <0 : infinite / 0 : one frame / >0 time in second)
	 */
	void		PushSegment( vec3 P0, vec3 P1, u8vec4 Color0, u8vec4 Color1, float PersistTime = 0.f );
	void		PushSegmentList( Array<vec3> const& SegmentList, u8vec4 Color, float PersistTime = 0.f );
    void		PushSegmentList( Array<vec3> const& SegmentList, Array<u8vec4> const& ColorList, float PersistTime = 0.f );
	void		PushOBB( transform T, u8vec4 Color, float PersistTime = 0.f );
	void		PushAABB( vec3 Pos, float Scale, u8vec4 Color, float PersistTime = 0.f );

protected:
	Shader*                     m_UtilSegShader;
    Shader*                     m_UtilShapeShader;

	Array<Draw::BufferRange>	m_SegmentList;
    Array<Draw::Vertex>         m_SegBuffer;
    
    Draw::BufferRange           m_Shapes[Draw::ShapeType::Count];
    Array<u8vec4>               m_ShapeColors;
    Array<mat4>                 m_ShapeMatrices;
    
    enum eVAType
    {
        eVASeg = 0,
        eVAShape,
        eVACount
    };
    enum eVBType
    {
        eVBSeg = 0,        /** Dynamic VB used to render segments */
        eVBShapeElt,
        eVBShapePos,
        eVBShapeMat,
        eVBShapeCol,
        eVBCount
    };

    GLuint					m_VArrays[eVACount];
    GLuint					m_VBuffers[eVBCount];

	void		RemoveOldElements( float DeltaSeconds );
};

} /* namespace bigball */

#endif // BB_DRAWUTILS_H