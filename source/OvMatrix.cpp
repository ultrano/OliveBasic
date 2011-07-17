#include "OvMatrix.h"
#include "OvVector3.h"
#include "OvQuaternion.h"

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
	memset( this, 0, sizeof(OvMatrix) );

	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;

	return *this;
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

	ax = quat.Inverse() * ax * quat;
	ay = quat.Inverse() * ay * quat;
	az = quat.Inverse() * az * quat;

	out.Identity();

	out._11 = ax.vec.x;
	out._21 = ax.vec.y;
	out._31 = ax.vec.z;

	out._12 = ay.vec.x;
	out._22 = ay.vec.y;
	out._32 = ay.vec.z;

	out._13 = az.vec.x;
	out._23 = az.vec.y;
	out._33 = az.vec.z;

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

