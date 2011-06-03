#pragma once

#include "OvMemObject.h"

class OvRect : public OvMemObject
{
	OvRect();
	~OvRect();

private:

	OvFloat m_left;
	OvFloat m_right;
	OvFloat m_top;
	OvFloat m_bottom;


};