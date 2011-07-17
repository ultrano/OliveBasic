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

OvQuaternion::OvQuaternion( const OvVector3& axis, OvFloat radian )
{
	OvQuaternionMake( *this, axis, radian );
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
		quat.vec.y * vec.z  -  quat.vec.z * vec.y  +  quat.vec.x * w  +  quat.w * vec.x,
		quat.vec.z * vec.x  -  quat.vec.x * vec.z  +  quat.vec.y * w  +  quat.w * vec.y,
		quat.vec.x * vec.y  -  quat.vec.y * vec.x  +  quat.vec.z * w  +  quat.w * vec.z,
		quat.w * w  -  quat.vec.x * vec.x  -  quat.vec.y * vec.y  -  quat.vec.z * vec.z);
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
	return Conjugate() / ( vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + w*w );
}

OvFloat OvQuaternion::Norm() const
{
	return sqrtf( vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + w*w );
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
	kQuat.vec.x		=	(-sin_x * cos_y * cos_z) - (cos_x * sin_y * sin_z);
	kQuat.vec.y		=	(+sin_x * cos_y * sin_z) - (cos_x * sin_y * cos_z);
	kQuat.vec.z		=	(-cos_x * cos_y * sin_z) - (sin_x * sin_y * cos_z);

	return kQuat;

}

OvQuaternion& OvQuaternionMake( OvQuaternion& out, OvFloat axisX,OvFloat axisY,OvFloat axisZ,OvFloat radian )
{
	return OvQuaternionMake( out, OvVector3( axisX, axisY, axisZ ), radian );
}

OvQuaternion& OvQuaternionMake( OvQuaternion& out, const OvVector3& axis,OvFloat radian )
{
	out.vec = axis.Normalize() * sinf(radian/2.0f);
	out.w = cosf(radian/2.0f);
	return out;
}
