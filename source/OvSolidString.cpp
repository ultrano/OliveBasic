#include "OvSolidString.h"


OvSolidString::OvSolidString()
: m_data( NULL )
{
}
OvSolidString::OvSolidString( const OvString &s )
: m_data( OvNew Data( s ) )
{
}
OvSolidString::OvSolidString( const OvSolidString &other )
: m_data( other.m_data )
{
}

OvSolidString::~OvSolidString()
{
	m_data = NULL;
}

OvSolidString OvSolidString::operator+( const OvSolidString &other ) const
{
	return OvSolidString( str() + other.str() );
}

const OvSolidString & OvSolidString::operator = ( const OvSolidString &other )
{
	m_data = other.m_data;
	return *this;
}

OvBool OvSolidString::operator == ( const OvSolidString &other ) const
{
	return (str() == other.str());
}
OvBool OvSolidString::operator == ( const OvString &other ) const
{
	return (str() == other);
}
OvBool OvSolidString::operator == ( const OvChar* other ) const
{
	return (str() == other);
}

OvBool OvSolidString::operator != ( const OvSolidString &other ) const
{
	return (str() != other.str());
}
OvBool OvSolidString::operator != ( const OvString &other ) const
{
	return (str() != other);
}
OvBool OvSolidString::operator != ( const OvChar* other ) const
{
	return (str() != other);
}

const OvString& OvSolidString::str() const
{
	static OvString empty;
	return m_data?m_data->str:empty;
}

const OvChar* OvSolidString::c_str() const
{
	return str().c_str();
}

OvSize OvSolidString::size() const
{
	return str().size();
}

void OvSolidString::clear()
{
	m_data = NULL;
}

OvBool operator == ( const OvString &arg1, const OvSolidString &arg2 ) 
{
	return (arg1 == arg2.str());
}
OvBool operator != ( const OvString &arg1, const OvSolidString &arg2 ) 
{
	return (arg1 != arg2.str());
}

OvBool operator == ( const OvChar* arg1, const OvSolidString &arg2 ) 
{
	return (arg1 == arg2.str());
}
OvBool operator != ( const OvChar* arg1, const OvSolidString &arg2 ) 
{
	return (arg1 != arg2.str());
}