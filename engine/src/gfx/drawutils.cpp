

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
    m_UtilSegShader = GfxManager::GetStaticInstance()->LoadShader( "utilseg" );
    m_UtilShapeShader = GfxManager::GetStaticInstance()->LoadShader( "utilshape" );
    
    glGenVertexArrays( eVACount, m_VArrays );
    glGenBuffers( eVBCount, m_VBuffers );
    
    glBindVertexArray( m_VArrays[eVASeg] );
    glBindBuffer( GL_ARRAY_BUFFER, m_VBuffers[eVBSeg] );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Draw::Vertex) /*stride*/, (void*)0 /*offset*/	);
    glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Draw::Vertex) /*stride*/, (void*)12 /*offset*/	);
    
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    //////////////////////////////////////////////////////////
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
    
    glBindVertexArray( m_VArrays[eVAShape] );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_VBuffers[eVBShapeElt] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, COUNT_OF(IdxData) * sizeof(GLuint), IdxData, GL_STATIC_DRAW );
    
    glBindBuffer( GL_ARRAY_BUFFER, m_VBuffers[eVBShapePos] );
    glBufferData( GL_ARRAY_BUFFER, COUNT_OF(CubeData) * sizeof(vec3), CubeData, GL_STATIC_DRAW );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) /*stride*/, (void*)0 /*offset*/ );

    glBindBuffer( GL_ARRAY_BUFFER, m_VBuffers[eVBShapeCol] );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(u8vec4) /*stride*/, (void*)0 /*offset*/	);
    glVertexAttribDivisor( 1, 1 );
    
    glBindBuffer( GL_ARRAY_BUFFER, m_VBuffers[eVBShapeMat] );
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
    glDeleteBuffers( eVBCount, m_VBuffers );
    glDeleteVertexArrays( eVACount, m_VArrays );
}

void DrawUtils::Tick( TickContext& TickCtxt )
{
	
}

void DrawUtils::_Render( struct RenderContext& RenderCtxt )
{
    mat4 ViewInvMat( RenderCtxt.m_View.m_Transform.GetRotation(), RenderCtxt.m_View.m_Transform.GetTranslation(), (float)RenderCtxt.m_View.m_Transform.GetScale() );
    
    // Render Segments
    m_UtilSegShader->Bind();
    {
        ShaderUniform UniProj = m_UtilSegShader->GetUniformLocation("proj_mat");
        m_UtilSegShader->SetUniform( UniProj, RenderCtxt.m_ProjMat );
        ShaderUniform UniView = m_UtilSegShader->GetUniformLocation("view_mat");
        m_UtilSegShader->SetUniform( UniView, bigball::inverse(ViewInvMat) );
    
        glBindVertexArray( m_VArrays[eVASeg] );
    
        glBindBuffer(GL_ARRAY_BUFFER, m_VBuffers[eVBSeg]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_SegBuffer.size() * sizeof(Draw::Vertex), (GLvoid*)m_SegBuffer.Data(), GL_DYNAMIC_DRAW );
        
        for( int iSL = 0; iSL < m_SegmentList.size(); iSL++)
        {
            Draw::BufferRange const& seg_list = m_SegmentList[iSL];
            glDrawArrays(/*GL_TRIANGLE_STRIP*/GL_LINE_STRIP, seg_list.Offset, seg_list.Count );
        }
    
        glBindVertexArray(0);
    }
    m_UtilSegShader->Unbind();
    
    // Render Shapes
    m_UtilShapeShader->Bind();
    {
        ShaderUniform UniProj = m_UtilShapeShader->GetUniformLocation("proj_mat");
        m_UtilShapeShader->SetUniform( UniProj, RenderCtxt.m_ProjMat );
        ShaderUniform UniView = m_UtilShapeShader->GetUniformLocation("view_mat");
        m_UtilShapeShader->SetUniform( UniView, bigball::inverse(ViewInvMat) );
    
        glBindVertexArray( m_VArrays[eVAShape] );
    
        glBindBuffer(GL_ARRAY_BUFFER, m_VBuffers[eVBShapeCol]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_ShapeColors.size() * sizeof(u8vec4), (GLvoid*)m_ShapeColors.Data(), GL_DYNAMIC_DRAW );
        
        glBindBuffer(GL_ARRAY_BUFFER, m_VBuffers[eVBShapeMat]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_ShapeMatrices.size() * sizeof(mat4), (GLvoid*)m_ShapeMatrices.Data(), GL_DYNAMIC_DRAW );

        glDrawElementsInstanced( GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0, m_ShapeMatrices.size() );
    
        glBindVertexArray(0);
    }
    m_UtilShapeShader->Unbind();
    

	// Purge old elements
	RemoveOldElements( RenderCtxt.m_DeltaSeconds );
}

void DrawUtils::RemoveOldElements( float DeltaSeconds )
{
    m_SegmentList.clear();
    m_SegBuffer.clear();
    
    for (int i = 0; i < Draw::ShapeType::Count; i++)
    {
        m_Shapes[i].Offset = 0;
        m_Shapes[i].Count = 0;
    }
    m_ShapeColors.clear();
    m_ShapeMatrices.clear();
    
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
	Draw::BufferRange SegList = { Offset, 2 };
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
	Draw::BufferRange SegList = { Offset, SegmentList.size() };
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
    Draw::BufferRange SegList = { Offset, SegmentList.size() };
    m_SegmentList.push_back( SegList );
}
void DrawUtils::PushOBB( transform T, u8vec4 Color, float PersistTime )
{
    mat4 m( T.GetRotation(), T.GetTranslation(), T.GetScale() );

    m_ShapeColors.push_back( Color );
    m_ShapeMatrices.push_back( m );
    
    m_Shapes[Draw::ShapeType::Box].Offset = 0;
    m_Shapes[Draw::ShapeType::Box].Count = m_ShapeMatrices.size();
}
void DrawUtils::PushAABB( vec3 Pos, float Scale, u8vec4 Color, float PersistTime )
{
    PushOBB( transform( quat(1.f,0.f,0.f,0.f), Pos, Scale ), Color );
}


} /*namespace bigball*/