#pragma once

#include "OvMemObject.h"
#include "OvTypeDef.h"

struct OvRect : public OvMemObject
{
	OvFloat left;
	OvFloat top;
	OvFloat right;
	OvFloat bottom;

	OvRect();
	OvRect( OvFloat pleft, OvFloat ptop, OvFloat pright, OvFloat pbottom );
	~OvRect();

	OvBool IsInRect( OvFloat px, OvFloat py ) const;
	OvBool IsIntersect( const OvRect& rect ) const;

};