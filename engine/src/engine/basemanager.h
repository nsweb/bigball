


#ifndef BB_BASEMANAGER_H
#define BB_BASEMANAGER_H


#define STATIC_MANAGER_H( Manager ) \
private:\
	static Manager* m_pStaticInstance;\
public:\
	static Manager* GetStaticInstance()		{ return m_pStaticInstance; }

#define STATIC_MANAGER_CPP( Manager ) \
	Manager* Manager::m_pStaticInstance = NULL;

namespace bigball
{

class BIGBALL_API BaseManager 
{
public:
					BaseManager();
	virtual			~BaseManager();

	virtual void	Create();
	virtual void	Destroy();	
	virtual void	Tick( struct TickContext& TickCtxt );
	virtual void	_Render( struct RenderContext& RenderCtxt );
};

} /* namespace bigball */

#endif // BB_BASEMANAGER_H