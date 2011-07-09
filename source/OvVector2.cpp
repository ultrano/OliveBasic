#include "OvVector2.h"
#include "OvVector2.h"
#include "OvUtility.h"

OvVector2 OvVector2::AXIS_X(1,0);
OvVector2 OvVector2::AXIS_Y(0,1);

OvVector2 OvVector2::Normalize()
{
	OvFloat kfLength = Length();
	return OvVector2(x/kfLength,y/kfLength);
}
OvFloat	OvVector2::DotProduct(const OvVector2& _rPt)
{
	return OvFloat(x*_rPt.x + y*_rPt.y );
}
OvFloat	OvVector2::Length()
{
	return  Length3D(x,y,0,0,0,0);
}
OvFloat	OvVector2::Length(const OvVector2& _rTargetPoint)
{
	return  Length3D(x,y,0,_rTargetPoint.x,_rTargetPoint.y,0);
}

OvVector2 OvVector2::operator +(const OvVector2& _rPt) const
{
	return OvVector2(x + _rPt.x,y + _rPt.y );
}
OvVector2 OvVector2::operator -(const OvVector2& _rPt) const
{
	return OvVector2(x - _rPt.x,y - _rPt.y );
}
OvVector2 OvVector2::operator -() const
{
	return OvVector2(-x,-y);
}
OvVector2 OvVector2::operator *(OvFloat _fScalar) const
{
	return OvVector2(x * _fScalar ,y * _fScalar );
}
OvVector2	OvVector2::operator /(OvFloat _fScalar) const
{
	return OvVector2(x / _fScalar ,y / _fScalar );
}

OvVector2& OvVector2::operator +=(const OvVector2& _rPt) 
{
	x += _rPt.x;
	y += _rPt.y;
	return *this;
}
OvVector2& OvVector2::operator -=(const OvVector2& _rPt) 
{
	x -= _rPt.x;
	y -= _rPt.y;
	return *this;
}
OvBool	OvVector2::operator ==(const OvVector2& _rPt) const
{
	return ((x == _rPt.x)&&(y == _rPt.y));
}
OvBool	OvVector2::operator !=(const OvVector2& _rPt) const
{
	return ((x != _rPt.x)&&(y != _rPt.y));
}