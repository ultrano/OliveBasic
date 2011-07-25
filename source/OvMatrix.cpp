#include "OvMatrix.h"
#include "OvVector3.h"
#include "OvQuaternion.h"
#include <math.h>

//! Constructor
OvMatrix::OvMatrix()
{
	Identity();
};

OvMatrix::OvMatrix( const OvMatrix& mat )
{
	memcpy((void*)this,(void*)&mat,sizeof(OvMatrix));
}
OvMatrix::OvMatrix
(OvFloat k11,OvFloat k12,OvFloat k13,OvFloat k14
 ,OvFloat k21,OvFloat k22,OvFloat k23,OvFloat k24
 ,OvFloat k31,OvFloat k32,OvFloat k33,OvFloat k34
 ,OvFloat k41,OvFloat k42,OvFloat k43,OvFloat k44)
 :_11(k11),_12(k12),_13(k13),_14(k14)
 ,_21(k21),_22(k22),_23(k23),_24(k24)
 ,_31(k31),_32(k32),_33(k33),_34(k34)
 ,_41(k41),_42(k42),_43(k43),_44(k44)
{
};

//! Operator Overload
OvMatrix	OvMatrix::operator * (const OvMatrix& mat) const
{
	OvMatrix	out;
	const OvMatrix&	p1 = *this;
	const OvMatrix&	p2 = mat;

	for ( OvByte row = 0 ; row < 4 ; ++row )
	{
		for ( OvByte col = 0 ; col < 4 ; ++col )
		{
			out.m[row][col] 
			= p1.m[row][0] * p2.m[0][col]
			+ p1.m[row][1] * p2.m[1][col]
			+ p1.m[row][2] * p2.m[2][col]
			+ p1.m[row][3] * p2.m[3][col];
		}
	}
	
	return out;
}

OvMatrix	OvMatrix::operator * ( OvFloat scalar ) const
{
	OvMatrix out;
	OvFloat* mat1 = (OvFloat*)&out;
	OvFloat* mat2 = (OvFloat*)this;
	for (OvInt i=0;i<16;++i)
	{
		mat1[i] = mat2[i] * scalar;
	}
	return out;
}

OvMatrix operator * (OvFloat scalar, const OvMatrix& mat)
{
	return mat * scalar;
}

OvMatrix& OvMatrix::Identity()
{
	return OvMatrixIdentity(*this);
}

OvVector3 OvMatrix::GetScale()
{
	OvVector3 out;
	out.x = OvVector3( _11, _12, _13 ).Length();
	out.y = OvVector3( _21, _22, _23 ).Length();
	out.z = OvVector3( _31, _32, _33 ).Length();
	return out;
}

OvQuaternion OvMatrix::GetRotate()
{
	OvQuaternion out;

	float tr = _11 + _22 + _33;

	if (tr > 0) { 
		float S = sqrtf(tr+1.0f) * 2; // S=4*out.w 
		out.w = 0.25f * S;
		out.x = (_32 - _23) / S;
		out.y = (_13 - _31) / S; 
		out.z = (_21 - _12) / S; 
	} else if ((_11 > _22)&(_11 > _33)) { 
		float S = sqrtf(1.0f + _11 - _22 - _33) * 2; // S=4*out.x 
		out.w = (_32 - _23) / S;
		out.x = 0.25f * S;
		out.y = (_12 + _21) / S; 
		out.z = (_13 + _31) / S; 
	} else if (_22 > _33) { 
		float S = sqrtf(1.0f + _22 - _11 - _33) * 2; // S=4*out.y
		out.w = (_13 - _31) / S;
		out.x = (_12 + _21) / S; 
		out.y = 0.25f * S;
		out.z = (_23 + _32) / S; 
	} else { 
		float S = sqrtf(1.0f + _33 - _11 - _22) * 2; // S=4*out.z
		out.w = (_21 - _12) / S;
		out.x = (_13 + _31) / S;
		out.y = (_23 + _32) / S;
		out.z = 0.25f * S;
	}

	return out;
}

OvVector3 OvMatrix::GetTranslate()
{
	OvVector3 out;
	out.x = _41;
	out.y = _42;
	out.z = _43;
	return out;
}

OvMatrix& OvMatrixIdentity( OvMatrix& out )
{
	memset( &out, 0, sizeof(OvMatrix) );

	out._11 = out._22 = out._33 = out._44 = 1;

	return out;
}

OvMatrix& OvMatrixScaling( OvMatrix& out, OvFloat total )
{
	return OvMatrixScaling( out, total, total, total );
}

OvMatrix& OvMatrixScaling( OvMatrix& out, const OvVector3& total )
{
	return OvMatrixScaling( out, total.x, total.y, total.z );
}

OvMatrix& OvMatrixScaling( OvMatrix& out, OvFloat x,OvFloat y,OvFloat z )
{
	out.Identity();

	out._11 = x;
	out._22 = y;
	out._33 = z;

	return out;
}

OvMatrix& OvMatrixRotation( OvMatrix& out, const OvQuaternion& quat )
{
	OvQuaternion ax(1,0,0,0);
	OvQuaternion ay(0,1,0,0);
	OvQuaternion az(0,0,1,0);
	OvQuaternion inv = quat.Inverse();

	ax = inv * ax * quat;
	ay = inv * ay * quat;
	az = inv * az * quat;

	out.Identity();

	out._11 = ax.x;
	out._21 = ax.y;
	out._31 = ax.z;

	out._12 = ay.x;
	out._22 = ay.y;
	out._32 = ay.z;

	out._13 = az.x;
	out._23 = az.y;
	out._33 = az.z;

	return out;
}

OvMatrix& OvMatrixRotation( OvMatrix& out, const OvVector3& axis, OvFloat radian )
{
	return OvMatrixRotation( out, OvQuaternion( axis, radian ) );
}

OvMatrix& OvMatrixTranslation( OvMatrix& out, const OvVector3& pos )
{
	return OvMatrixTranslation( out, pos.x, pos.y, pos.z );
}

OvMatrix& OvMatrixTranslation( OvMatrix& out, OvFloat x,OvFloat y,OvFloat z )
{
	out.Identity();

	out._41 = x;
	out._42 = y;
	out._43 = z;

	return out;
}

OvMatrix& OvMatrixProjectLH( OvMatrix& out, OvFloat fov, OvFloat aspect, OvFloat Zf, OvFloat Zn )
{

	OvFloat w = 1.0f/tanf(fov/2.0f);
	OvFloat h = w * aspect;
	OvFloat Q = Zf / (Zf - Zn);

	//////////////////////////////////////////////////////////////////////////
	out._11 = w;	out._12 = 0;	out._13 = 0;	 out._14 = 0;

	out._21 = 0;	out._22 = h;	out._23 = 0;	 out._24 = 0;

	out._31 = 0;	out._32 = 0;	out._33 = Q;	 out._34 = 1;

	out._41 = 0;	out._42 = 0;	out._43 = -Zn*Q; out._44 = 0;
	//////////////////////////////////////////////////////////////////////////
	return out;
}

OvMatrix& OvMatrixView( OvMatrix& out, const OvVector3& look, const OvVector3& up, const OvVector3& pos )
{
	OvVector3 looknorm = look.Normal();
	OvVector3 upnorm = up.Normal();
	OvVector3 rightnrom = look.Cross(up).Normal();

	//////////////////////////////////////////////////////////////////////////
	out._11 = rightnrom.x;	out._12 = upnorm.x;	out._13 = looknorm.x;	 out._14 = 0;
															  			
	out._21 = rightnrom.y;	out._22 = upnorm.y;	out._23 = looknorm.y;	 out._24 = 0;
															  			
	out._31 = rightnrom.z;	out._32 = upnorm.z;	out._33 = looknorm.z;	 out._34 = 0;

	//////////////////////////////////////////////////////////////////////////
	
	out._41 = -rightnrom.Dot( pos );
	out._42 = -upnorm.Dot( pos );
	out._43 = -looknorm.Dot( pos );
	out._44 = 1;

	//////////////////////////////////////////////////////////////////////////

	return out;
}