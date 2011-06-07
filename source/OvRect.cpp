#include "OvRect.h"
#include "OvUtility.h"

OvRect::OvRect()
: left	( 0 )
, top	( 0 )
, right	( 0 )
, bottom( 0 )
{

}

OvRect::OvRect( OvUInt pleft, OvUInt ptop, OvUInt pright, OvUInt pbottom )
: left	( pleft )
, top	( ptop )
, right	( pright )
, bottom( pbottom )
{

}

OvRect::~OvRect()
{

}

OvBool OvRect::IsInRect( OvUInt px, OvUInt py ) const
{
	return ! ((left   >  px ) ||
			  (right  <= px ) ||
			  (top    >  py ) ||
			  (bottom <= py ) );
}

OvBool OvRect::IsIntersect( const OvRect& rect ) const
{
	OvRect temp(  min( rect.left, rect.right )
				, min( rect.top , rect.bottom )
				, max( rect.left, rect.right )
				, max( rect.top , rect.bottom ) ); //< ������ �簢���� ��츦 ���� ó��.

	return ( right  > temp.left  &&
		 	 left   < temp.right &&
		 	 bottom > temp.top	 &&
		 	 top    < temp.bottom );
}

OvUInt OvRect::Height() const
{
	return ( ( bottom - top ) );
}

OvUInt OvRect::Width() const
{
	return ( ( right - left ) );
}