#pragma once
#include "OvTypeDef.h"
#include "OvMemObject.h"

struct OvRect;
interface_class OvScreenDrawer : public OvMemObject
{

public:

	/// Draw
	virtual void DrawRect( const OvRect& rect ) = 0;
	virtual void DrawString( const OvRect& rect, const OvString& text ) = 0;
	virtual void DrawLine( const OvIPoint2 & start, const OvIPoint2 & end ) = 0;

};