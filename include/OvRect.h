#pragma once

#include "OvMemObject.h"
#include "OvTypeDef.h"

struct OvRect : public OvMemObject
{
	OvInt left;
	OvInt top;
	OvInt right;
	OvInt bottom;

	OvRect();
	OvRect( const OvRect& rect);
	OvRect( OvInt pleft, OvInt ptop, OvInt pright, OvInt pbottom );
	~OvRect();

	OvBool IsInRect( OvInt px, OvInt py ) const;
	OvBool IsIntersect( const OvRect& rect ) const;

	void   SetHeight( OvInt height );
	OvInt GetHeight() const;

	void   SetWidth( OvInt height );
	OvInt GetWidth() const;
};