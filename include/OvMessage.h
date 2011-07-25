#pragma once
#include "OvRefObject.h"
#include "OvValue.h"

enum
{
	Msg_None,
	Msg_Mouse,
	Msg_Keyboard,
	Msg_PropNotify,
};

OvSmartPointer_Struct(OvMessage);
struct OvMessage : public OvRefObject
{
	OvInt	type;
};

struct OvMouseMsg : public OvMessage
{
	OvInt x;
	OvInt y;
	OvInt wm_type;

	OvMouseMsg()
	{
		type = Msg_Mouse;
		x = y = 0;
	}
};

struct OvKeyboardMsg : public OvMessage
{
	OvUInt	key;
	OvInt	wm_type;

	OvKeyboardMsg()
	{
		type = Msg_Keyboard;
		key = 0;
	}
};

struct OvPropNotifyMsg : public OvMessage
{
	enum PropNotify { Prop_Insert, Prop_Remove };

	PropNotify notify;
	const OvString& key;
	OvValueSPtr val;

	OvPropNotifyMsg( PropNotify _notify, const OvString& _key, OvValueSPtr _val )
		: notify( _notify ), key( _key ), val( _val )
	{
		type = Msg_PropNotify;
	}
};