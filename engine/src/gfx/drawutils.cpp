

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
    m_UtilsShader = GfxManager::GetStaticInstance()->LoadShader( "utils" );
    
    glGenVertexArrays( 1, &m_Seg_VAO);
    glBindVertexArray( m_Seg_VAO);
    
    glGenBuffers( 1, &m_Seg_VBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_Seg_VBO );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Draw::Vertex) /*stride*/, (void*)0 /*offset*/	);
    glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Draw::Vertex) /*stride*/, (void*)12 /*offset*/	);
    
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
void DrawUtils::Destroy()
{
    glDeleteBuffers( 1, &m_Seg_VBO );
    glDeleteVertexArrays( 1, &m_Seg_VAO );
}

void DrawUtils::Tick( TickContext& TickCtxt )
{
	
}

void DrawUtils::_Render( struct RenderContext& RenderCtxt )
{
    mat4 ViewMat( RenderCtxt.m_View.m_Transform.GetRotation(), RenderCtxt.m_View.m_Transform.GetTranslation(), (float)RenderCtxt.m_View.m_Transform.GetScale() );
    //mat4 ViewMat(1.f);
    
    m_UtilsShader->Bind();
    ShaderUniform UniProj = m_UtilsShader->GetUniformLocation("proj_mat");
    m_UtilsShader->SetUniform( UniProj, RenderCtxt.m_ProjMat );
    ShaderUniform UniView = m_UtilsShader->GetUniformLocation("view_mat");
    m_UtilsShader->SetUniform( UniView, bigball::inverse(ViewMat) );
    
    glBindVertexArray( m_Seg_VAO );
    
    //for( int iSL = 0; iSL < m_SegmentList.size(); iSL++)
    {
        //Draw::SegmentList const& seg_list = m_SegmentList[iSL];
        //int idx_buffer_offset = 0;
        
        glBindBuffer(GL_ARRAY_BUFFER, m_Seg_VBO);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_SegBuffer.size() * sizeof(Draw::Vertex), (GLvoid*)m_SegBuffer.Data(), GL_DYNAMIC_DRAW/*GL_STREAM_DRAW*/);
        
        for( int iSL = 0; iSL < m_SegmentList.size(); iSL++)
        //for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
        {
            Draw::SegmentList const& seg_list = m_SegmentList[iSL];
            glDrawArrays(/*GL_TRIANGLE_STRIP*/GL_LINE_STRIP, seg_list.Offset, seg_list.Count );
            
            //idx_buffer_offset += pcmd->ElemCount;
        }
    }
    
    glBindVertexArray(0);
    m_UtilsShader->Unbind();
    

	// Purge old elements
	RemoveOldElements( RenderCtxt.m_DeltaSeconds );
}

void DrawUtils::RemoveOldElements( float DeltaSeconds )
{
    m_SegmentList.clear();
    m_SegBuffer.clear();
    
#if 0
    for( int i = m_Segments.size() - 1; i >= 0 ; i++ )
    {
        if( m_Segments[i].PersistTime < 0.f )
            continue;
        
        m_Segments[i].PersistTime = bigball::max( 0.f, m_Segments[i].PersistTime - DeltaSeconds );
        if( m_Segments[i].PersistTime == 0.f )
            m_Segments.erase(i);
    }
    
    for( int i = m_SegmentList.size() - 1; i >= 0 ; i++ )
    {
        if( m_SegmentList[i].PersistTime < 0.f )
            continue;
        
        m_SegmentList[i].PersistTime = bigball::max( 0.f, m_SegmentList[i].PersistTime - DeltaSeconds );
        if( m_SegmentList[i].PersistTime == 0.f )
            m_SegmentList.erase(i);
    }
    
    for( int i = m_Shapes.size() - 1; i >= 0 ; i++ )
    {
        if( m_Shapes[i].PersistTime < 0.f )
            continue;
        
        m_Shapes[i].PersistTime = bigball::max( 0.f, m_Shapes[i].PersistTime - DeltaSeconds );
        if( m_Shapes[i].PersistTime == 0.f )
            m_Shapes.erase(i);
    }
#endif
}

void DrawUtils::PushSegment( vec3 P0, vec3 P1, u8vec4 Color0, u8vec4 Color1, float PersistTime )
{
    const int Offset = m_SegBuffer.size();
    m_SegBuffer.push_back( { P0, Color0 } );
    m_SegBuffer.push_back( { P1, Color1 } );
    m_SegmentList.push_back( { Offset, 2 } );
}
void DrawUtils::PushSegmentList( Array<vec3> const& SegmentList, u8vec4 Color, float PersistTime )
{
    const int Offset = m_SegBuffer.size();
    for( int i =0; i < SegmentList.size(); i++ )
    {
        m_SegBuffer.push_back( { SegmentList[i], Color } );
    }
	m_SegmentList.push_back( { Offset, SegmentList.size() } );
}
void DrawUtils::PushOBB( transform T, u8vec4 Color, float PersistTime )
{
	Draw::Shape Shape = { Draw::ShapeType::Box, T, Color };
	m_Shapes.push_back( Shape );
}
void DrawUtils::PushAABB( vec3 Pos, float Scale, u8vec4 Color, float PersistTime )
{
	Draw::Shape Shape = { Draw::ShapeType::Box, transform(quat(1.f,0.f,0.f,0.f), Pos, Scale), Color };
	m_Shapes.push_back( Shape );
}


} /*namespace bigball*/