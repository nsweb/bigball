


#ifndef BB_GFXRENDERCONTEXT_H
#define BB_GFXRENDERCONTEXT_H

#include "../engine/camera.h"

namespace bigball
{

class BIGBALL_API Shader;
//BIGBALL_TEMPLATE template class BIGBALL_API Map<String,Shader*>;

struct BIGBALL_API RenderContext
{
	CameraView	m_View;
	CameraView*	m_pFrustumView;
	mat4		m_ProjMat;
	float		m_DeltaSeconds;
	uint32		m_FrameIdx;
	uint32		m_RenderMode;
};

} /* namespace bigball */

#endif // BB_GFXRENDERCONTEXT_H