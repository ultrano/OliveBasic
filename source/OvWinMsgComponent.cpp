#include "OvWinMsgComponent.h"
#include "OvActObject.h"
#include "OvMessage.h"
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

void OvWinMsgComponent::OnMsgNotified( OvMessageSPtr msg )
{
	if ( OvActObject* target = GetTarget().GetRear() )
	{
		target->PostComponentMsg( msg );
	}
}

LRESULT CALLBACK OvWinMsgNotify( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	OvMessageSPtr msg = NULL;

	switch (message)
	{
	case WM_KEYUP: 
	case WM_KEYDOWN: 
		{
			OvKeyboardMsg* keyboard = OvNew OvKeyboardMsg;
			msg = keyboard;
			keyboard->key	  = wParam;
			keyboard->wm_type = message;
		}
		break;
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
		{
			OvMouseMsg* mouse = OvNew OvMouseMsg;
			msg = mouse;

			mouse->x=(int)(short)LOWORD(lParam);
			mouse->y=(int)(short)HIWORD(lParam);
			mouse->wm_type = message;

			break;
		}
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
