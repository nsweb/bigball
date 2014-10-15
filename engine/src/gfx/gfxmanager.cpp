

#include "../bigball.h"
#include "shader.h"
#include "gfxmanager.h"
#include "bufferlock.h"

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
	// Create lock manager
	BufferLockManager* pLockManager = new BufferLockManager( true );
}
void GfxManager::Destroy()
{
	uint32 ShaderCount = m_Shaders.GetPairCount();
	for( uint32 i = 0; i < ShaderCount; ++i )
	{
		auto pPair = m_Shaders.GetPairAt( i );
		BB_DELETE( pPair->Value );
	}

	// Destroy lock manager
	delete BufferLockManager::GetStaticInstance();
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


} /*namespace bigball*/