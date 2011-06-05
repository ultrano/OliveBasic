#pragma once

#include "OvMemObject.h"
#include "OvTypeDef.h"

struct OvRect : public OvMemObject
{
	OvSize left;
	OvSize top;
	OvSize right;
	OvSize bottom;

	OvRect();
	OvRect( OvSize pleft, OvSize ptop, OvSize pright, OvSize pbottom );
	~OvRect();

	OvBool IsInRect( OvFloat px, OvFloat py ) const;
	OvBool IsIntersect( const OvRect& rect ) const;

	OvSize Height() const;
};