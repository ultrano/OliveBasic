#include "OvWinMsgComponent.h"
#include "OvActObject.h"
#include "OvComponentMsg.h"
OvRTTI_IMPL(OvWinMsgComponent);
OvFACTORY_OBJECT_IMPL(OvWinMsgComponent);

struct SMsgCompSet : public OvMemObject
{
	static SMsgCompSet& GetInstance() { static SMsgCompSet inst; return inst; };
	OvSet<OvWinMsgComponent*> comp_set;
};

OvWinMsgComponent::OvWinMsgComponent( factory )
{
	SMsgCompSet::GetInstance().comp_set.insert( this );
}

OvWinMsgComponent::OvWinMsgComponent()
{
	SMsgCompSet::GetInstance().comp_set.insert( this );
}

OvWinMsgComponent::~OvWinMsgComponent()
{
	SMsgCompSet::GetInstance().comp_set.erase( this );
}

void OvWinMsgComponent::Update( OvTimeTick elapsed )
{

}

void OvWinMsgComponent::OnMsgNotified( OvComponentMsgSPtr msg )
{
	if ( OvActObject* target = GetTarget().GetRear() )
	{
		target->PostComponentMsg( msg );
	}
}

LRESULT CALLBACK OvWinMsgNotify( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	OvComponentMsgSPtr msg = NULL;

	switch (message)
	{
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
		
		OvMouseMsg* mouse = OvNew OvMouseMsg;
		msg = mouse;

		mouse->x=(int)(short)LOWORD(lParam);
		mouse->y=(int)(short)HIWORD(lParam);
		switch ( message )
		{
		case WM_LBUTTONUP:		mouse->down = false; mouse->which = OvMouseMsg::Which_Left;	break;
		case WM_LBUTTONDOWN:	mouse->down = true; mouse->which = OvMouseMsg::Which_Left;	break;
		case WM_RBUTTONDOWN:	mouse->down = false; mouse->which = OvMouseMsg::Which_Right;break;
		case WM_RBUTTONUP:		mouse->down = true; mouse->which = OvMouseMsg::Which_Right;	break;
		case WM_MOUSEMOVE:		mouse->which = OvMouseMsg::Which_Move;	break;
		}
		break;
	}

	if ( msg )
	{
		for each ( OvWinMsgComponent* comp in SMsgCompSet::GetInstance().comp_set )
		{
			comp->OnMsgNotified( msg );
		}
		return 1;
	}
	return 0;
}
