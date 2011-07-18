#include "OvQuaternion.h"
#include "OvVector3.h"
#include <math.h>

OvQuaternion::OvQuaternion()
: w(1)
{
}

OvQuaternion::OvQuaternion(const OvQuaternion& quat)
{
	memcpy((void*)this,(void*)&quat,sizeof(OvQuaternion));
}

OvQuaternion::OvQuaternion( const OvVector3& _vec, OvFloat _w )
: vec( _vec )
, w( _w )
{
}

OvQuaternion::OvQuaternion(  OvFloat fX, OvFloat fY, OvFloat fZ, OvFloat fW )
: vec( fX, fY, fZ )
, w( fW )
{
}

OvQuaternion::operator OvFloat* ()
{
	return (OvFloat*)this;
}
OvQuaternion::operator const OvFloat* () const
{
	return (const OvFloat*)this;
}

OvQuaternion& OvQuaternion::operator += ( const OvQuaternion& quat)
{
	*this = *this + quat;
	return *this;
}
OvQuaternion& OvQuaternion::operator -= ( const OvQuaternion& quat)
{
	*this = *this - quat;
	return *this;
}
OvQuaternion& OvQuaternion::operator *= ( const OvQuaternion& quat)
{
	*this = *this * quat;
	return *this;
}
OvQuaternion& OvQuaternion::operator *= ( OvFloat val)
{
	vec *= val;
	w *= val;
	return *this;
}
OvQuaternion& OvQuaternion::operator /= ( OvFloat val)
{
	vec /= val;
	w /= val;
	return *this;
}

//

OvQuaternion  OvQuaternion::operator + () const
{
	return *this;
}
OvQuaternion  OvQuaternion::operator - () const
{
	return -1*(*this);
}

//

OvQuaternion OvQuaternion::operator + ( const OvQuaternion& quat) const
{
	return OvQuaternion( vec + quat.vec ,w + quat.w);
}
OvQuaternion OvQuaternion::operator - ( const OvQuaternion& quat) const
{
	return OvQuaternion( vec - quat.vec,w - quat.w);
}
OvQuaternion OvQuaternion::operator * ( const OvQuaternion& quat) const
{
	return OvQuaternion(
		quat.y * z  -  quat.z * y  +  quat.x * w  +  quat.w * x,
		quat.z * x  -  quat.x * z  +  quat.y * w  +  quat.w * y,
		quat.x * y  -  quat.y * x  +  quat.z * w  +  quat.w * z,
		quat.w * w  -  quat.x * x  -  quat.y * y  -  quat.z * z);
}
OvQuaternion OvQuaternion::operator * ( OvFloat val) const
{
	return OvQuaternion( vec * val, w * val);
}
OvQuaternion OvQuaternion::operator / ( OvFloat val) const
{
	return OvQuaternion( vec / val, w / val);
}


OvQuaternion operator * (OvFloat val, const OvQuaternion& quat)
{
	return quat / val;
}

OvBool OvQuaternion::operator == ( const OvQuaternion& quat) const
{
	return ! (memcmp((void*)this,(void*)&quat,sizeof(OvQuaternion)));
}
OvBool OvQuaternion::operator != ( const OvQuaternion& quat) const
{
	return !(*this == quat);
}

OvQuaternion&		OvQuaternion::Identity()
{
	vec *= 0;
	w = 1;
	return *this;
}

OvQuaternion OvQuaternion::Conjugate() const
{
	return OvQuaternion( -vec, w );
}

OvQuaternion		OvQuaternion::Inverse() const
{
	OvFloat norm2 = ( x*x + y*y + z*z + w*w );
	return OvQuaternion ( -vec / norm2, w  / norm2 );
}

OvFloat OvQuaternion::Norm() const
{
	return sqrtf( x*x + y*y + z*z + w*w );
}

OvQuaternion	OvEulerToQuaternion(OvFloat fX_Rotation,OvFloat fY_Rotation,OvFloat fZ_Rotation)
{
	OvQuaternion	kQuat;
	OvFloat half_x = fX_Rotation / 2.0f;
	OvFloat half_y = fY_Rotation / 2.0f;
	OvFloat half_z = fZ_Rotation / 2.0f;

	OvFloat sin_x = sinf( half_x );
	OvFloat cos_x = cosf( half_x );

	OvFloat sin_y = sinf( half_y );
	OvFloat cos_y = cosf( half_y );

	OvFloat sin_z = sinf( half_z );
	OvFloat cos_z = cosf( half_z );

	kQuat.w		=	(+cos_x * cos_y * cos_z) - (sin_x * sin_y * sin_z);
	kQuat.x		=	(-sin_x * cos_y * cos_z) - (cos_x * sin_y * sin_z);
	kQuat.y		=	(+sin_x * cos_y * sin_z) - (cos_x * sin_y * cos_z);
	kQuat.z		=	(-cos_x * cos_y * sin_z) - (sin_x * sin_y * cos_z);

	return kQuat;

}

OvQuaternion& OvQuaternionMake( OvQuaternion& out, OvFloat axisX,OvFloat axisY,OvFloat axisZ,OvFloat radian )
{
	return OvQuaternionMake( out, OvVector3( axisX, axisY, axisZ ), radian );
}

OvQuaternion& OvQuaternionMake( OvQuaternion& out, const OvVector3& axis,OvFloat radian )
{
	float theta = radian/2.0f;
	out.vec = axis.Normalize() * sinf( theta );
	out.w = cosf( theta );
	return out;
}
