#include "OvVector3.h"
#include "OvUtility.h"

OvVector3 OvVector3::AXIS_X(1,0,0);
OvVector3 OvVector3::AXIS_Y(0,1,0);
OvVector3 OvVector3::AXIS_Z(0,0,1);

OvVector3 OvVector3::Normalize() const
{
	OvFloat kfLength = Length();
	return kfLength? OvVector3(x/kfLength,y/kfLength,z/kfLength) : *this;
}

OvFloat	OvVector3::Dot(const OvVector3& p2)
{
	return OvFloat(x*p2.x + y*p2.y + z*p2.z);
}

OvVector3 OvVector3::Cross( const OvVector3& p2 )
{
	return OvVector3
		( y*p2.z - p2.y*z
		, z*p2.x - p2.x*z
		, x*p2.y - p2.y*x);
}

OvFloat	OvVector3::Length() const
{
	return  Length3D(x,y,z,0,0,0);
}

OvFloat	OvVector3::Length(const OvVector3& _rTargetPoint) const
{
	return Length3D(x,y,z,_rTargetPoint.x,_rTargetPoint.y,_rTargetPoint.z);
}

OvVector3 OvVector3::operator +(const OvVector3& _rPt) const
{
	return OvVector3(x + _rPt.x,y + _rPt.y,z + _rPt.z);
}

OvVector3 OvVector3::operator -(const OvVector3& _rPt) const
{
	return OvVector3(x - _rPt.x,y - _rPt.y,z - _rPt.z);
}

OvVector3 OvVector3::operator -() const
{
	return OvVector3(-x,-y,-z);
}

OvVector3 OvVector3::operator *(OvFloat _fScalar) const
{
	return OvVector3(x * _fScalar ,y * _fScalar ,z * _fScalar );
}
OvVector3	OvVector3::operator /(OvFloat _fScalar) const
{
	return OvVector3(x / _fScalar ,y / _fScalar ,z / _fScalar );
}
// OvVector3	OvVector3::operator *(const OvMatrix mulMat) const
// {
// 	/*
// 	D3DXVec3Transform((D3DXVECTOR4*)&outPut2,(D3DXVECTOR3*)this,(D3DXMATRIX*)&mulMat);
// 	return outPut;
// 	*/
// 	OvQuaternion outPut
// 	( ( x * mulMat._11 ) + ( y * mulMat._21 ) + ( z * mulMat._31 ) + ( 1 * mulMat._41 )
// 	, ( x * mulMat._12 ) + ( y * mulMat._22 ) + ( z * mulMat._32 ) + ( 1 * mulMat._42 )
// 	, ( x * mulMat._13 ) + ( y * mulMat._23 ) + ( z * mulMat._33 ) + ( 1 * mulMat._43 )
// 	, ( x * mulMat._14 ) + ( y * mulMat._24 ) + ( z * mulMat._34 ) + ( 1 * mulMat._44 )
// 	);
// 	OvFloat w = (outPut.w)? outPut.w : 1;
// 	return OvVector3
// 		( outPut.x / w
// 		, outPut.y / w
// 		, outPut.z / w
// 		);
// }

//OvVector3 OvVector3::operator *(const OvQuaternion& _rQuter) const
//{
//
//	return OvVector3();
//}

OvVector3& OvVector3::operator +=(const OvVector3& _rPt) 
{
	x += _rPt.x;
	y += _rPt.y;
	z += _rPt.z;
	return *this;
}
OvVector3& OvVector3::operator -=(const OvVector3& _rPt) 
{
	x -= _rPt.x;
	y -= _rPt.y;
	z -= _rPt.z;
	return *this;
}
OvBool	OvVector3::operator ==(const OvVector3& _rPt) const
{
	return ((x == _rPt.x)&&(y == _rPt.y)&&(z == _rPt.z));
}

OvBool	OvVector3::operator !=(const OvVector3& _rPt) const
{
	return ((x != _rPt.x)&&(y != _rPt.y)&&(z != _rPt.z));
}

OvVector3& OvVector3::operator*=( OvFloat val )
{
	*this = *this * val;
	return *this;
}

OvVector3& OvVector3::operator/=( OvFloat val )
{
	*this = *this / val;
	return *this;
}

OvFloat	 OvVector3DotProduct(const OvVector3& _rPt0,const OvVector3& _rPt1)
{
	return OvFloat(_rPt0.x*_rPt1.x + _rPt0.y*_rPt1.y + _rPt0.z*_rPt1.z);
}

OvVector3 OvVector3CrossProduct(const OvVector3& _rPt0,const OvVector3& _rPt1)
{
	return OvVector3(_rPt0.y*_rPt1.z - _rPt0.z*_rPt1.y,_rPt0.z*_rPt1.x - _rPt0.x*_rPt1.z,_rPt0.x*_rPt1.y - _rPt0.y*_rPt1.x);
}

OvFloat	 OvVector3Length(const OvVector3& _rPt0,const OvVector3& _rPt1)
{
	return Length3D(_rPt0.x,_rPt0.y,_rPt0.z,_rPt1.x,_rPt1.y,_rPt1.z);
}