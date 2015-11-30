

#ifndef BB_DRAWUTILS_H
#define BB_DRAWUTILS_H

#include "../engine/basemanager.h"

namespace bigball
{

class BIGBALL_API Shader;

namespace Draw
{
	enum class ShapeType
	{
		Box = 0,
	};

    struct Vertex
    {
        vec3	Pos;
        u8vec4	Col;
    };
	struct SegmentList
	{
        int Offset;
        int Count;
	};
	struct Shape
	{
		ShapeType Type;
		transform T;
		vec4 Color;
		//float PersistTime;
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
	void		PushOBB( transform T, u8vec4 Color, float PersistTime = 0.f );
	void		PushAABB( vec3 Pos, float Scale, u8vec4 Color, float PersistTime = 0.f );

protected:
	Shader*				m_UtilsShader;

	Array<Draw::SegmentList>	m_SegmentList;
    Array<Draw::Vertex>         m_SegBuffer;
    
	Array<Draw::Shape>			m_Shapes;
    
    /** Dynamic VB used to render segments */
    GLuint					m_Seg_VAO;
    GLuint					m_Seg_VBO;

	void		RemoveOldElements( float DeltaSeconds );
};

} /* namespace bigball */

#endif // BB_DRAWUTILS_H