

#include "../bigball.h"
#include "shader.h"
#include "drawutils.h"
#include "rendercontext.h"
#include "gfxmanager.h"

namespace bigball
{

STATIC_MANAGER_CPP( DrawUtils )


DrawUtils::DrawUtils()
{
	m_pStaticInstance = this;
}

DrawUtils::~DrawUtils()
{
	m_pStaticInstance = nullptr;
}

void DrawUtils::Create()
{
    m_util_seg_shader = GfxManager::GetStaticInstance()->LoadShader( "utilseg" );
    m_util_shape_shader = GfxManager::GetStaticInstance()->LoadShader( "utilshape" );
    
    glGenVertexArrays( eVACount, m_varrays );
    glGenBuffers( eVBCount, m_vbuffers );
    
    glBindVertexArray( m_varrays[eVASeg] );
    glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBSeg] );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Draw::Vertex) /*stride*/, (void*)0 /*offset*/	);
    glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Draw::Vertex) /*stride*/, (void*)12 /*offset*/	);
    
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    //////////////////////////////////////////////////////////
    // Shape
    const vec3 cube_data[] = { vec3(-1.f, -1.f, -1.f), vec3(-1.f, 1.f, -1.f), vec3(1.f, 1.f, -1.f), vec3(1.f, -1.f, -1.f),
                              vec3(-1.f, -1.f, 1.f), vec3(-1.f, 1.f, 1.f), vec3(1.f, 1.f, 1.f), vec3(1.f, -1.f, 1.f) };
    
    GLuint idx_data[] = {
            0,1,2, 0,2,3,
            1,0,4, 1,4,5,
            2,1,5, 2,5,6,
            3,2,6, 3,6,7,
            0,3,7, 0,7,4,
            5,4,7, 5,7,6
    };
    
    glBindVertexArray( m_varrays[eVAShape] );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vbuffers[eVBShapeElt] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, COUNT_OF(idx_data) * sizeof(GLuint), idx_data, GL_STATIC_DRAW );
    
    glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBShapePos] );
    glBufferData( GL_ARRAY_BUFFER, COUNT_OF(cube_data) * sizeof(vec3), cube_data, GL_STATIC_DRAW );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) /*stride*/, (void*)0 /*offset*/ );

    glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBShapeCol] );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(u8vec4) /*stride*/, (void*)0 /*offset*/	);
    glVertexAttribDivisor( 1, 1 );
    
    glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBShapeMat] );
    for (int i = 0; i < 4; i++ )
    {
        glEnableVertexAttribArray( 2 + i );
        glVertexAttribPointer( 2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4) /*stride*/, (void*)(sizeof(vec4) * i) /*offset*/ );
        glVertexAttribDivisor( 2 + i, 1 );
    }
    
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
void DrawUtils::Destroy()
{
    glDeleteBuffers( eVBCount, m_vbuffers );
    glDeleteVertexArrays( eVACount, m_varrays );
}

void DrawUtils::Tick( TickContext& tick_ctxt )
{
	
}

void DrawUtils::_Render( struct RenderContext& render_ctxt )
{
    mat4 view_inv_mat( render_ctxt.m_view.m_Transform.GetRotation(), render_ctxt.m_view.m_Transform.GetTranslation(), (float)render_ctxt.m_view.m_Transform.GetScale() );
    
    // Render Segments
    m_util_seg_shader->Bind();
    {
        ShaderUniform UniProj = m_util_seg_shader->GetUniformLocation("proj_mat");
        m_util_seg_shader->SetUniform( UniProj, render_ctxt.m_proj_mat );
        ShaderUniform UniView = m_util_seg_shader->GetUniformLocation("view_mat");
        m_util_seg_shader->SetUniform( UniView, bigball::inverse(view_inv_mat) );
    
        glBindVertexArray( m_varrays[eVASeg] );
    
        glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBSeg]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_seg_buffer.size() * sizeof(Draw::Vertex), (GLvoid*)m_seg_buffer.Data(), GL_DYNAMIC_DRAW );
        
        for( int iSL = 0; iSL < m_seg_list.size(); iSL++)
        {
            Draw::BufferRange const& seg_list = m_seg_list[iSL];
            glDrawArrays(/*GL_TRIANGLE_STRIP*/GL_LINE_STRIP, seg_list.m_offset, seg_list.m_count );
        }
    
        glBindVertexArray(0);
    }
    m_util_seg_shader->Unbind();
    
    // Render Shapes
    m_util_shape_shader->Bind();
    {
        ShaderUniform UniProj = m_util_shape_shader->GetUniformLocation("proj_mat");
        m_util_shape_shader->SetUniform( UniProj, render_ctxt.m_proj_mat );
        ShaderUniform UniView = m_util_shape_shader->GetUniformLocation("view_mat");
        m_util_shape_shader->SetUniform( UniView, bigball::inverse(view_inv_mat) );
    
        glBindVertexArray( m_varrays[eVAShape] );
    
        glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBShapeCol]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_shape_colors.size() * sizeof(u8vec4), (GLvoid*)m_shape_colors.Data(), GL_DYNAMIC_DRAW );
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBShapeMat]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_shape_matrices.size() * sizeof(mat4), (GLvoid*)m_shape_matrices.Data(), GL_DYNAMIC_DRAW );

        glDrawElementsInstanced( GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0, m_shape_matrices.size() );
    
        glBindVertexArray(0);
    }
    m_util_shape_shader->Unbind();
    

	// Purge old elements
	RemoveOldElements( render_ctxt.m_delta_seconds );
}

void DrawUtils::RemoveOldElements( float delta_seconds )
{
    m_seg_list.clear();
    m_seg_buffer.clear();
    
    for (int i = 0; i < Draw::ShapeType::Count; i++)
    {
        m_shapes[i].m_offset = 0;
        m_shapes[i].m_count = 0;
    }
    m_shape_colors.clear();
    m_shape_matrices.clear();
    
#if 0
    for( int i = m_Segments.size() - 1; i >= 0 ; i++ )
    {
        if( m_Segments[i].persist_time < 0.f )
            continue;
        
        m_Segments[i].persist_time = bigball::max( 0.f, m_Segments[i].persist_time - DeltaSeconds );
        if( m_Segments[i].persist_time == 0.f )
            m_Segments.erase(i);
    }
    
    for( int i = m_SegmentList.size() - 1; i >= 0 ; i++ )
    {
        if( m_SegmentList[i].persist_time < 0.f )
            continue;
        
        m_SegmentList[i].persist_time = bigball::max( 0.f, m_SegmentList[i].persist_time - DeltaSeconds );
        if( m_SegmentList[i].persist_time == 0.f )
            m_SegmentList.erase(i);
    }
    
    for( int i = m_Shapes.size() - 1; i >= 0 ; i++ )
    {
        if( m_Shapes[i].persist_time < 0.f )
            continue;
        
        m_Shapes[i].persist_time = bigball::max( 0.f, m_Shapes[i].persist_time - DeltaSeconds );
        if( m_Shapes[i].persist_time == 0.f )
            m_Shapes.erase(i);
    }
#endif
}

void DrawUtils::PushSegment( vec3 p0, vec3 p1, u8vec4 color0, u8vec4 color1, float persist_time )
{
    const int offset = m_seg_buffer.size();
	Draw::Vertex v0 = { p0, color0 };
	Draw::Vertex v1 = { p1, color1 };
    m_seg_buffer.push_back( v0 );
    m_seg_buffer.push_back( v1 );
	Draw::BufferRange seg_list = { offset, 2 };
    m_seg_list.push_back( seg_list );
}
void DrawUtils::PushSegmentList( Array<vec3> const& segment_list, u8vec4 color, float persist_time )
{
    const int offset = m_seg_buffer.size();
    for( int i =0; i < segment_list.size(); i++ )
    {
		Draw::Vertex v = { segment_list[i], color };
        m_seg_buffer.push_back( v );
    }
	Draw::BufferRange seg_list = { offset, segment_list.size() };
	m_seg_list.push_back( seg_list );
}
void DrawUtils::PushSegmentList( Array<vec3> const& segment_list, Array<u8vec4> const& color_list, float persist_time )
{
    const int offset = m_seg_buffer.size();
    for( int i =0; i < segment_list.size(); i++ )
    {
        Draw::Vertex v = { segment_list[i], color_list[i] };
        m_seg_buffer.push_back( v );
    }
    Draw::BufferRange seg_list = { offset, segment_list.size() };
    m_seg_list.push_back( seg_list );
}
void DrawUtils::PushOBB( transform t, u8vec4 color, float persist_time )
{
    mat4 m( t.GetRotation(), t.GetTranslation(), t.GetScale() );

    m_shape_colors.push_back( color );
    m_shape_matrices.push_back( m );
    
    m_shapes[Draw::ShapeType::Box].m_offset = 0;
    m_shapes[Draw::ShapeType::Box].m_count = m_shape_matrices.size();
}
void DrawUtils::PushAABB( vec3 pos, float scale, u8vec4 color, float persist_time )
{
    PushOBB( transform( quat(1.f,0.f,0.f,0.f), pos, scale ), color );
}


} /*namespace bigball*/