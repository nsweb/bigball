


#ifndef BB_UIMANAGER_H
#define BB_UIMANAGER_H

#include "../engine/basemanager.h"

namespace bigball
{

struct UIId
{
	uint32 Owner;
	uint32 Item;
	uint32 Index;
};

struct UIContext
{
	UIId Hot;		// about to be interacting (mouse hovering, highlight etc.)
	UIId Active;	// actually interacting

	//ActiveContext;	// specific data for widget when active
};

/** Page, menu etc. (important for draw order) */
class UILayer
{

};

/** Reurns true if the button was clicked */
bool DoButton( UIContext& Ctx, UIId& ButtonID, const char* Text );
{
	if( Ctx.Active == ButtonID )
	{
		if( mousewentup )
		{
			if( hot)
				result=true;
			Setnotactive;
		}
	}
	else if( Ctx.Hot == ButtonID )
	{
		if( mousewentdown )
		{
			Setactive;
		}
	}

	if( Inside )
		Sethot;

	// Display the button
}

void MainUpdateLoop()
{
	// Affichage en lua par ex
	for( menus )
	{
		for( items )
			DoItems;
	}
	for( popups )

	for( 3delt )
}

//////////////////////////////////////////////////////////////////////////
class BIGBALL_API UIManager : public BaseManager 
{
	STATIC_MANAGER_H(UIManager)

public:

						UIManager();
	virtual				~UIManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& TickCtxt );


protected:


};

} /* namespace bigball */

#endif // BB_UIMANAGER_H