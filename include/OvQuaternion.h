#pragma once
#include "OvTypeDef.h"
#include "OvVector3.h"

class OvQuaternion : OvMemObject
{
public:
	OvQuaternion() ;
	OvQuaternion(const OvQuaternion& );
	OvQuaternion( const OvVector3& axis, OvFloat radian );
	OvQuaternion( OvFloat fX, OvFloat fY, OvFloat fZ, OvFloat fW );

	operator OvFloat* ();
	operator const OvFloat* () const;

	OvQuaternion& operator += ( const OvQuaternion& );
	OvQuaternion& operator -= ( const OvQuaternion& );
	OvQuaternion& operator *= ( const OvQuaternion& );
	OvQuaternion& operator *= ( OvFloat );
	OvQuaternion& operator /= ( OvFloat );

	OvQuaternion  operator + () const;
	OvQuaternion  operator - () const;

	OvQuaternion operator + ( const OvQuaternion& ) const;
	OvQuaternion operator - ( const OvQuaternion& ) const;
	OvQuaternion operator * ( const OvQuaternion& ) const;
	OvQuaternion operator * ( OvFloat ) const;
	OvQuaternion operator / ( OvFloat ) const;

	friend OvQuaternion operator * (OvFloat, const OvQuaternion& );

	OvBool operator == ( const OvQuaternion& ) const;
	OvBool operator != ( const OvQuaternion& ) const;

	//!

	OvQuaternion&		Identity();
	OvQuaternion		Conjugate() const;
	OvQuaternion		Inverse() const;
	OvFloat				Norm() const;

public:

	OvFloat w;
	OvVector3 vec;
};

//OvQuaternion	OvQuaternionSphericalInterpolate(OvFloat fRate,const OvQuaternion& crScr,const OvQuaternion& crDest );
OvQuaternion	OvEulerToQuaternion(OvFloat fX_Rotation,OvFloat fY_Rotation,OvFloat fZ_Rotation);

OvQuaternion& OvQuaternionMake( OvQuaternion& out, OvFloat axisX,OvFloat axisY,OvFloat axisZ,OvFloat radian);
OvQuaternion& OvQuaternionMake( OvQuaternion& out, const OvVector3& axis,OvFloat radian);