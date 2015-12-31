


#ifndef BB_GFXMANAGER_H
#define BB_GFXMANAGER_H

#include "../engine/basemanager.h"

namespace bigball
{

class BIGBALL_API Shader;
//BIGBALL_TEMPLATE template class BIGBALL_API Map<String,Shader*>;

////////////////////////////////////////////////////////////
class BIGBALL_API GfxManager : public BaseManager 
{
	STATIC_MANAGER_H(GfxManager)

public:
						GfxManager();
	virtual				~GfxManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& TickCtxt );

	Shader*				LoadShader( String const& ShaderName );
    Shader*				LoadShaderFromMemory( String const& ShaderName, const char** src_buffers );

protected:
	Map<String,Shader*>		m_Shaders;

};

} /* namespace bigball */

#endif // BB_GFXMANAGER_H