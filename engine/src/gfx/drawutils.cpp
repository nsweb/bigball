

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
    
    // Shape
    const vec3 CubeData[] = { {-1.f, -1.f, -1.f}, {-1.f, 1.f, -1.f}, {1.f, 1.f, -1.f}, {1.f, -1.f, -1.f},
                              {-1.f, -1.f, 1.f}, {-1.f, 1.f, 1.f}, {1.f, 1.f, 1.f}, {1.f, -1.f, 1.f} };
    
    GLuint IdxData[] = {
            0,1,2, 0,2,3,
            1,0,4, 1,4,5,
            2,1,5, 2,5,6,
            3,2,6, 3,6,7,
            0,3,7, 0,7,4,
            5,4,7, 5,7,6
    };
    
    glGenVertexArrays( 1, &m_Shape_VAO);
    glBindVertexArray( m_Shape_VAO);
    glGenBuffers( 1, &m_Shape_VBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_Shape_VBO );
    
    glEnableVertexAttribArray(0);
    glBufferData( GL_ARRAY_BUFFER, COUNT_OF(CubeData) * sizeof(vec3), CubeData, GL_STATIC_DRAW );
    
    glGenBuffers( 1, &m_Shape_VEO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_Shape_VEO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, COUNT_OF(IdxData) * sizeof(GLuint), IdxData, GL_STATIC_DRAW );
    
}
void DrawUtils::Destroy()
{
    GLuint BOArray[] = { m_Seg_VBO, m_Shape_VBO, m_Shape_VEO };
    glDeleteBuffers( COUNT_OF(BOArray), BOArray );
    GLuint AOArray[] = { m_Seg_VAO, m_Shape_VAO };
    glDeleteVertexArrays( COUNT_OF(AOArray), AOArray );
}

void DrawUtils::Tick( TickContext& TickCtxt )
{
	
}

void DrawUtils::_Render( struct RenderContext& RenderCtxt )
{
    mat4 ViewInvMat( RenderCtxt.m_View.m_Transform.GetRotation(), RenderCtxt.m_View.m_Transform.GetTranslation(), (float)RenderCtxt.m_View.m_Transform.GetScale() );
    
    m_UtilsShader->Bind();
    ShaderUniform UniProj = m_UtilsShader->GetUniformLocation("proj_mat");
    m_UtilsShader->SetUniform( UniProj, RenderCtxt.m_ProjMat );
    ShaderUniform UniView = m_UtilsShader->GetUniformLocation("view_mat");
    m_UtilsShader->SetUniform( UniView, bigball::inverse(ViewInvMat) );
    
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
	Draw::Vertex v0 = { P0, Color0 };
	Draw::Vertex v1 = { P1, Color1 };
    m_SegBuffer.push_back( v0 );
    m_SegBuffer.push_back( v1 );
	Draw::SegmentList SegList = { Offset, 2 };
    m_SegmentList.push_back( SegList );
}
void DrawUtils::PushSegmentList( Array<vec3> const& SegmentList, u8vec4 Color, float PersistTime )
{
    const int Offset = m_SegBuffer.size();
    for( int i =0; i < SegmentList.size(); i++ )
    {
		Draw::Vertex v = { SegmentList[i], Color };
        m_SegBuffer.push_back( v );
    }
	Draw::SegmentList SegList = { Offset, SegmentList.size() };
	m_SegmentList.push_back( SegList );
}
void DrawUtils::PushSegmentList( Array<vec3> const& SegmentList, Array<u8vec4> const& ColorList, float PersistTime )
{
    const int Offset = m_SegBuffer.size();
    for( int i =0; i < SegmentList.size(); i++ )
    {
        Draw::Vertex v = { SegmentList[i], ColorList[i] };
        m_SegBuffer.push_back( v );
    }
    Draw::SegmentList SegList = { Offset, SegmentList.size() };
    m_SegmentList.push_back( SegList );
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