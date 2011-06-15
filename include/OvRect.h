#pragma once

#include "OvMemObject.h"
#include "OvTypeDef.h"

struct OvRect : public OvMemObject
{
	OvUInt left;
	OvUInt top;
	OvUInt right;
	OvUInt bottom;

	OvRect();
	OvRect( const OvRect& rect);
	OvRect( OvUInt pleft, OvUInt ptop, OvUInt pright, OvUInt pbottom );
	~OvRect();

	OvBool IsInRect( OvUInt px, OvUInt py ) const;
	OvBool IsIntersect( const OvRect& rect ) const;

	void   SetHeight( OvUInt height );
	OvUInt GetHeight() const;

	void   SetWidth( OvUInt height );
	OvUInt GetWidth() const;
};