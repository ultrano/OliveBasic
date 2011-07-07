#pragma once
#include "OvRefObject.h"

enum
{
	CompMsg_Mouse,
	CompMsg_keyboard,
};

OvSmartPointer_Struct(OvComponentMsg);
struct OvComponentMsg : public OvRefObject
{
	OvInt	type;
};

struct OvMouseMsg : public OvComponentMsg
{
	enum 
	{
		Which_None,
		Which_Left,
		Which_Right,
		Which_Move,
	};
	OvInt x;
	OvInt y;
	OvInt which;
	OvBool down;

	OvMouseMsg()
	{
		type = CompMsg_Mouse;
		which = Which_None;
		x = y = 0;
		down = false;
	}
};