#pragma once
#include "OvRefObject.h"

enum
{
	Msg_None,
	Msg_Mouse,
	Msg_Keyboard,
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