#include "OvRect.h"
#include "OvUtility.h"

OvRect::OvRect()
: left	( 0 )
, top	( 0 )
, right	( 0 )
, bottom( 0 )
{

}

OvRect::OvRect( const OvRect& rect )
: left	( rect.left )
, top	( rect.top )
, right	( rect.right )
, bottom( rect.bottom )
{

}

OvRect::OvRect( OvInt pleft, OvInt ptop, OvInt pright, OvInt pbottom )
: left	( pleft )
, top	( ptop )
, right	( pright )
, bottom( pbottom )
{

}

OvRect::~OvRect()
{

}

OvBool OvRect::IsInRect( OvInt px, OvInt py ) const
{
	bool ret = ! ((left   >  px ) ||
		(right  <= px ) ||
		(top    >  py ) ||
		(bottom <= py ) );
	return ret;
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

void OvRect::SetHeight( OvInt height )
{
	bottom = top + height;
}

OvInt OvRect::GetHeight() const
{
	return ( ( bottom - top ) );
}

void OvRect::SetWidth( OvInt height )
{
	right = left + height;
}

OvInt OvRect::GetWidth() const
{
	return ( ( right - left ) );
}
