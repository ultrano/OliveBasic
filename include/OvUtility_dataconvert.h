#pragma once
#include "OvTypeDef.h"
#include "OvVector2.h"
#include "OvVector3.h"
#include "OvQuaternion.h"
#include "OvObjectID.h"
#include "OvColor.h"

template<typename T>
T		OvFromString( const OvString& str );
template<typename T>
OvString OvToString( const T& val );

/// Ovint
template<>
OvInt	OvFromString( const OvString& str );
template<>
OvString OvToString( const OvInt& val );

/// OvUInt
template<>
OvUInt	OvFromString( const OvString& str );
template<>
OvString OvToString( const OvUInt& val );

/// OvBool
template<>
OvBool	OvFromString( const OvString& str );
template<>
OvString OvToString( const OvBool& val );

/// OvFloat
template<>
OvFloat	OvFromString( const OvString& str );
template<>
OvString OvToString( const OvFloat& val );

/// OvVector2
template<>
OvVector2	OvFromString( const OvString& str );
template<>
OvString	OvToString( const OvVector2& val );

/// OvVector3
template<>
OvVector3	OvFromString( const OvString& str );
template<>
OvString	OvToString( const OvVector3& val );

/// OvQuaternion
template<>
OvQuaternion OvFromString( const OvString& str );
template<>
OvString	OvToString( const OvQuaternion& val );

/// OvObjectID
template<>
OvObjectID	OvFromString( const OvString& str );
template<>
OvString	OvToString( const OvObjectID& val );

/// OvColor
template<>
OvColor		OvFromString( const OvString& str );
template<>
OvString	OvToString( const OvColor& val );

/// OvString
template<>
OvString	OvFromString( const OvString& str );
template<>
OvString	OvToString( const OvString& val );