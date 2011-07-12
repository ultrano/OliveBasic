#pragma once
#include "OvComponent.h"

class OvWinMsgComponent : public OvComponent
{
	OvRTTI_DECL(OvWinMsgComponent);
	OvFACTORY_OBJECT_DECL(OvWinMsgComponent);

	/// constructor for factory
	OvWinMsgComponent(factory);

public:
	OvWinMsgComponent();
	~OvWinMsgComponent();

	void OnMsgNotified( OvMessageSPtr msg );
	virtual void Update( OvTimeTick elapsed ) OVERRIDE;

private:

};

LRESULT CALLBACK OvWinMsgNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
