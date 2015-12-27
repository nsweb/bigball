


#ifndef BB_UIMANAGER_H
#define BB_UIMANAGER_H

#include "../gfx/dynamicvb.h"

namespace bigball
{

typedef void (*DrawEditorCB)( bool*, struct RenderContext& );
typedef void (*ToggleEditorCB)( bool );
    
    
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
    void                ToggleEditor();
    void                SetDrawEditorFn( DrawEditorCB pFn ) { m_pDrawEditorFn = pFn; }
    void                SetToggleEditorFn( ToggleEditorCB pFn ) { m_pToggleEditorFn = pFn; }

protected:
	void				InitImGui();
	void				DrawDebugMenu();
	void				DrawProfiler();

	static UIManager*		m_pStaticInstance;

public:
    DrawEditorCB            m_pDrawEditorFn;
    ToggleEditorCB          m_pToggleEditorFn;
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
    bool                    m_bShowEditor;
};

} /* namespace bigball */

#endif // BB_UIMANAGER_H