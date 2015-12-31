

#include "../bigball.h"
#include "shader.h"
#include "gfxmanager.h"

namespace bigball
{

STATIC_MANAGER_CPP( GfxManager )


GfxManager::GfxManager()
{
	m_pStaticInstance = this;
}

GfxManager::~GfxManager()
{
	m_pStaticInstance = nullptr;
}

void GfxManager::Create()
{

}
void GfxManager::Destroy()
{
	uint32 ShaderCount = m_Shaders.GetPairCount();
	for( uint32 i = 0; i < ShaderCount; ++i )
	{
		auto pPair = m_Shaders.GetPairAt( i );
		BB_DELETE( pPair->Value );
	}
}

void GfxManager::Tick( TickContext& TickCtxt )
{
	
}

Shader* GfxManager::LoadShader( String const& ShaderName )
{
	auto pPair = m_Shaders.Find( ShaderName );
	if( pPair )
		return pPair->Value;

	Shader* pShader = new Shader();
	if( !pShader->Create( ShaderName ) )
	{
		BB_DELETE( pShader );
		return nullptr;
	}

	m_Shaders.Add( ShaderName, pShader );
	return pShader;
}
    
Shader*	GfxManager::LoadShaderFromMemory( String const& ShaderName, const char** src_buffers )
{
    auto pPair = m_Shaders.Find( ShaderName );
    if( pPair )
        return pPair->Value;
    
    Shader* pShader = new Shader();
    if( !pShader->CreateFromMemory( src_buffers ) )
    {
        BB_DELETE( pShader );
        return nullptr;
    }
    
    m_Shaders.Add( ShaderName, pShader );
    return pShader;
}


} /*namespace bigball*/