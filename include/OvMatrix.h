#pragma once
#include "OvTypeDef.h"
#include "OvMemObject.h"

class OvVector3;
class OvQuaternion;
//typedef	D3DXMATRIXA16	OvMatrix;
class OvMatrix : public OvMemObject
{
public:

	OvMatrix();
	OvMatrix( const OvMatrix& mat );
	OvMatrix
		(OvFloat k11,OvFloat k12,OvFloat k13,OvFloat k14
		,OvFloat k21,OvFloat k22,OvFloat k23,OvFloat k24
		,OvFloat k31,OvFloat k32,OvFloat k33,OvFloat k34
		,OvFloat k41,OvFloat k42,OvFloat k43,OvFloat k44);

	OvMatrix	operator * (const OvMatrix&) const;
	OvMatrix	operator * (OvFloat ) const;

	friend OvMatrix operator * (OvFloat, const OvMatrix& );
	friend OvMatrix operator * (const OvMatrix&, const OvMatrix& );

	OvMatrix&	Identity();

	OvVector3	 GetScale();
	OvQuaternion GetRotate();
	OvVector3	 GetTranslate();

	union
	{
		struct 
		{
			OvFloat _11,_12,_13,_14;
			OvFloat _21,_22,_23,_24;
			OvFloat _31,_32,_33,_34;
			OvFloat _41,_42,_43,_44;
		};
		OvFloat m[4][4];
	};

};

OvMatrix&	OvMatrixIdentity( OvMatrix& out );

OvMatrix&	OvMatrixScaling( OvMatrix& out, OvFloat total );
OvMatrix&	OvMatrixScaling( OvMatrix& out, const OvVector3& total );
OvMatrix&	OvMatrixScaling( OvMatrix& out, OvFloat x,OvFloat y,OvFloat z );

OvMatrix&	OvMatrixRotation( OvMatrix& out, const OvQuaternion& quat );
OvMatrix&	OvMatrixRotation( OvMatrix& out, const OvVector3& axis, OvFloat radian );

OvMatrix&	OvMatrixTranslation( OvMatrix& out, const OvVector3& pos );
OvMatrix&	OvMatrixTranslation( OvMatrix& out, OvFloat x,OvFloat y,OvFloat z );

OvMatrix&	OvMatrixProjectLH( OvMatrix& out, OvFloat fov, OvFloat aspect, OvFloat Zf, OvFloat Zn );
OvMatrix&	OvMatrixView( OvMatrix& out, const OvVector3& look, const OvVector3& up, const OvVector3& pos );