


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
	virtual void		Tick( struct TickContext& tick_ctxt );

	Shader*				LoadShader( String const& shader_name );
    Shader*				LoadShaderFromMemory( String const& shader_name, const char** src_buffers );

protected:
	Map<String,Shader*>		m_shaders;

};

} /* namespace bigball */

#endif // BB_GFXMANAGER_H