#pragma once
#include "OvRefable.h"

class OvSolidString : OvMemObject
{
	struct Data : OvRefable
	{
		Data( const OvString &s ) : str( s ) {};
		const OvString str;
	};
public:
	// constructor
	OvSolidString();
	OvSolidString( const OvString &s );
	OvSolidString( const OvSolidString &other );

	// destructor
	~OvSolidString();

	// operator +
	OvSolidString operator + ( const OvSolidString &other ) const;

	// operator =
	const OvSolidString & operator = ( const OvSolidString &other );

	// operator ==
	OvBool operator == ( const OvSolidString &other ) const;
	OvBool operator == ( const OvString &other ) const;
	OvBool operator == ( const OvChar* other ) const;

	// operator !=
	OvBool operator != ( const OvSolidString &other ) const;
	OvBool operator != ( const OvString &other ) const;
	OvBool operator != ( const OvChar* other ) const;

	// to string
	const OvString& str() const;
	const OvChar*   c_str() const;
	OvSize			size() const;
	void clear();

private:
	OvSPtr<OvSolidString::Data> m_data;
};

OvBool operator == ( const OvString &arg1, const OvSolidString &arg2 ) ;
OvBool operator != ( const OvString &arg1, const OvSolidString &arg2 ) ;

OvBool operator == ( const OvChar* arg1, const OvSolidString &arg2 ) ;
OvBool operator != ( const OvChar* arg1, const OvSolidString &arg2 ) ;