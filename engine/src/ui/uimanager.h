


#ifndef BB_UIMANAGER_H
#define BB_UIMANAGER_H

#include "../gfx/dynamicvb.h"

namespace bigball
{

class BIGBALL_API Shader;

struct UIVertex
{
	vec2	Pos;
	vec2	Tex;
	u8vec4	Col;
};

//////////////////////////////////////////////////////////////////////////
class BIGBALL_API UIManager
{

public:

						UIManager();
	virtual				~UIManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& TickCtxt );
	virtual void		_Render( struct RenderContext& RenderCtxt );
	static UIManager*	GetStaticInstance()		{ return m_pStaticInstance; }

	void				RenderDrawLists(struct ImDrawData* data);
	void				ToggleDebugMenu();

protected:
	void				InitImGui();
	void				DrawDebugMenu();
	void				DrawProfiler();

	static UIManager*		m_pStaticInstance;

public:
	GLuint					m_DebugFontTexId;
	Shader*					m_UIShader;
	/** Dynamic VB used to render ui elements */
	GLuint					m_UI_VAO;
	//GLuint					m_UI_VB_TEMP;
	//UnsynchronizedVBO		m_UI_VBO;
	GLuint					m_UI_VBO;
	GLuint					m_UI_EBO;

	bool					m_bShowDebugMenu;
	bool					m_bShowProfiler;
};

} /* namespace bigball */

#endif // BB_UIMANAGER_H