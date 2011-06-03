#include "OvRect.h"
#include "OvUtility.h"

OvRect::OvRect()
: left	( 0.0f )
, top	( 0.0f )
, right	( 0.0f )
, bottom( 0.0f )
{

}

OvRect::OvRect( OvFloat pleft, OvFloat ptop, OvFloat pright, OvFloat pbottom )
: left	( pleft )
, top	( ptop )
, right	( pright )
, bottom( pbottom )
{

}

OvRect::~OvRect()
{

}

OvBool OvRect::IsInRect( OvFloat px, OvFloat py ) const
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
				, max( rect.top , rect.bottom ) ); //< 뒤집힌 사각형의 경우를 위한 처리.

	return ( right  > temp.left  &&
		 	 left   < temp.right &&
		 	 bottom > temp.top	 &&
		 	 top    < temp.bottom );
}
