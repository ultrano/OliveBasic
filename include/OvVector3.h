#pragma once
#include "OvTypeDef.h"

class OvVector3 : public OvFPoint3
{
public:
	static OvVector3	AXIS_X;
	static OvVector3	AXIS_Y;
	static OvVector3	AXIS_Z;
public :
	OvVector3() {x = 0; y = 0; z = 0; };
	OvVector3(OvFloat _fX,OvFloat _fY,OvFloat _fZ) { x = _fX; y = _fY; z = _fZ;  };

	OvVector3	Normalize();
	OvFloat		DotProduct(const OvVector3& _rPt);
	OvFloat		Length();
	OvFloat		Length(const OvVector3& _rTargetPoint);

	OvVector3	operator +(const OvVector3& _rPt) const;
	OvVector3	operator -(const OvVector3& _rPt) const;
	OvVector3	operator -() const;
	OvVector3	operator *(OvFloat _fScalar) const;
	OvVector3	operator /(OvFloat _fScalar) const;
	//OvVector3	operator *(const OvMatrix mulMat) const;
	//OvVector3 operator *(const OvQuaternion& _rQuter) const;

	OvVector3&	operator +=(const OvVector3& _rPt) ;
	OvVector3&	operator -=(const OvVector3& _rPt) ;
	OvBool		operator ==(const OvVector3& _rPt) const;
	OvBool		operator !=(const OvVector3& _rPt) const;
};



OvFloat	 OvVector3DotProduct(const OvVector3& _rPt0,const OvVector3& _rPt1);
OvVector3 OvVector3CrossProduct(const OvVector3& _rPt0,const OvVector3& _rPt1);
OvFloat	 OvVector3Length(const OvVector3& _rPt0,const OvVector3& _rPt1);