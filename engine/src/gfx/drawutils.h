

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
        vec3	m_pos;
        u8vec4	m_col;
    };
	struct BufferRange
	{
        int m_offset;
        int m_count;
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
	virtual void		Tick( struct TickContext& tick_ctxt );
	virtual void		_Render( struct RenderContext& render_ctxt );

	/* Functions for drawing utility shapes 
	 * persist_time : amount of time the shape should be drawn ( <0 : infinite / 0 : one frame / >0 time in second)
	 */
	void		PushSegment( vec3 p0, vec3 p1, u8vec4 color0, u8vec4 color1, float persist_time = 0.f );
	void		PushSegmentList( Array<vec3> const& segment_list, u8vec4 Color, float persist_time = 0.f );
    void		PushSegmentList( Array<vec3> const& segment_list, Array<u8vec4> const& color_list, float persist_time = 0.f );
	void		PushOBB( transform t, u8vec4 color, float persist_time = 0.f );
	void		PushAABB( vec3 pos, float scale, u8vec4 color, float persist_time = 0.f );

protected:
	Shader*                     m_util_seg_shader;
    Shader*                     m_util_shape_shader;

	Array<Draw::BufferRange>	m_seg_list;
    Array<Draw::Vertex>         m_seg_buffer;
    
    Draw::BufferRange           m_shapes[Draw::ShapeType::Count];
    Array<u8vec4>               m_shape_colors;
    Array<mat4>                 m_shape_matrices;
    
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

    GLuint					m_varrays[eVACount];
    GLuint					m_vbuffers[eVBCount];

	void		RemoveOldElements( float delta_seconds );
};

} /* namespace bigball */

#endif // BB_DRAWUTILS_H