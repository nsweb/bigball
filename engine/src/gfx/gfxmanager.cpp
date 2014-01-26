

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

}

void GfxManager::Tick( float DeltaSeconds )
{
	
}

Shader* GfxManager::LoadShader( String const& ShaderName )
{
	auto pPair = m_Shaders.Find( ShaderName );
	if( pPair )
		return pPair->Value;

	Shader* pShader = new Shader();
	pShader->Create( ShaderName );
	return pShader;
}


} /*namespace bigball*/