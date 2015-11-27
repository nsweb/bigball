

#include "../bigball.h"
#include "shader.h"
#include "drawutils.h"
#include "rendercontext.h"

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

}
void DrawUtils::Destroy()
{

}

void DrawUtils::Tick( TickContext& TickCtxt )
{
	
}

void DrawUtils::_Render( struct RenderContext& RenderCtxt )
{

	// Purge old elements
	RemoveOldElements( RenderCtxt.m_DeltaSeconds );
}

void DrawUtils::RemoveOldElements( float DeltaSeconds )
{

}

void DrawUtils::PushSegment( vec3 P0, vec3 P1, vec4 Color, float PersistTime )
{
	Draw::Segment Seg = { P0, P1, Color, PersistTime};
	m_Segments.push_back( Seg );
}
void DrawUtils::PushSegments( Array<vec3> const& SegmentList, vec4 Color, float PersistTime )
{
	Draw::SegmentList SegList = { SegmentList, Color, PersistTime};
	m_SegmentList.push_back( SegList );
}
void DrawUtils::PushOBB( transform T, vec4 Color, float PersistTime )
{
	Draw::Shape Shape = { Draw::ShapeType::Box, T, Color, PersistTime };
	m_Shapes.push_back( Shape );
}
void DrawUtils::PushAABB( vec3 Pos, float Scale, vec4 Color, float PersistTime )
{
	Draw::Shape Shape = { Draw::ShapeType::Box, transform(quat(1.f,0.f,0.f,0.f), Pos, Scale), Color, PersistTime };
	m_Shapes.push_back( Shape );
}


} /*namespace bigball*/