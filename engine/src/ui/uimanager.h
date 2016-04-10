


#ifndef BB_UIMANAGER_H
#define BB_UIMANAGER_H

#include "../gfx/dynamicvb.h"

namespace bigball
{

typedef void (*DrawEditorCB)( bool*, struct RenderContext& );
typedef void (*ToggleEditorCB)( bool );
typedef void (*DrawCustomMenuBarCB)(struct RenderContext&);
    
    
class BIGBALL_API Shader;

struct UIVertex
{
	vec2	m_os;
	vec2	m_tex;
	u8vec4	m_col;
};

//////////////////////////////////////////////////////////////////////////
class BIGBALL_API UIManager
{

public:

						UIManager();
	virtual				~UIManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& tick_ctxt );
	virtual void		_Render( struct RenderContext& render_ctxt );
	static UIManager*	GetStaticInstance()		{ return m_pStaticInstance; }

	void				RenderDrawLists(struct ImDrawData* data);
	void				ToggleDebugMenu();
    void                ToggleEditor();
    void                SetDrawEditorFn( DrawEditorCB fn )          { m_draw_editor_fn = fn; }
    void                SetToggleEditorFn( ToggleEditorCB fn )      { m_toggle_editor_fn = fn; }
	void                SetDrawCustomMenuFn(DrawCustomMenuBarCB fn) { m_draw_custom_menu_fn = fn; }

protected:
	void				InitImGui();
	void				DrawDebugMenu();
	void				DrawProfiler();

	static UIManager*		m_pStaticInstance;

public:
    DrawEditorCB            m_draw_editor_fn;
    ToggleEditorCB          m_toggle_editor_fn;
	DrawCustomMenuBarCB		m_draw_custom_menu_fn;
	GLuint					m_DebugFontTexId;
	Shader*					m_UIShader;
	/** Dynamic VB used to render ui elements */
	GLuint					m_UI_VAO;
	//GLuint					m_UI_VB_TEMP;
	//UnsynchronizedVBO		m_UI_VBO;
	GLuint					m_UI_VBO;
	GLuint					m_UI_EBO;

	bool					m_show_debug_menu;
	bool					m_show_profiler;
    bool                    m_show_editor;
};

} /* namespace bigball */

#endif // BB_UIMANAGER_H