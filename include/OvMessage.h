#pragma once
#include "OvRefable.h"

enum
{
	Msg_None,
	Msg_Mouse,
	Msg_Keyboard,
	Msg_PropNotify,
};

OvDescSPtr(struct,OvMessage);
struct OvMessage : public OvRefable
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
