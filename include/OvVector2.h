#pragma once

#include "OvTypeDef.h"

class OvVector2 : public OvFPoint2
{
public:
	static OvVector2	AXIS_X;
	static OvVector2	AXIS_Y;
public:
	OvVector2(){ x = 0; y = 0; };
	OvVector2(OvFloat fX,OvFloat fY){ x = fX; y = fY; };
	OvVector2	Normalize();
	OvFloat		DotProduct(const OvVector2& _rPt);
	OvFloat		Length();
	OvFloat		Length(const OvVector2& _rTargetPoint);

	OvVector2	operator +(const OvVector2& _rPt) const;
	OvVector2	operator -(const OvVector2& _rPt) const;
	OvVector2	operator -() const;
	OvVector2	operator *(OvFloat _fScalar) const;
	OvVector2	operator /(OvFloat _fScalar) const;

	OvVector2&	operator +=(const OvVector2& _rPt) ;
	OvVector2&	operator -=(const OvVector2& _rPt) ;
	OvBool		operator ==(const OvVector2& _rPt) const;
	OvBool		operator !=(const OvVector2& _rPt) const;
};