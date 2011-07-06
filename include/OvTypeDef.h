#pragma once

//////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <queue>

#define OvString			std::string
#define OvVector			std::vector
#define OvList				std::list
#define OvPair				std::pair
#define OvSet				std::set
#define OvMap				std::map
#define OvQueue				std::queue

//////////////////////////////////////////////////////////////////////////

typedef float				OvFloat;
typedef int					OvInt;
typedef unsigned int		OvUInt;
typedef size_t				OvSize;
typedef short				OvShort;
typedef unsigned short		OvUShort;
typedef bool				OvBool;
typedef double				OvDouble;
typedef char				OvChar;
typedef unsigned char		OvUChar;
typedef unsigned char		OvByte;

typedef float				OvTimeTick;

//////////////////////////////////////////////////////////////////////////

#include "OvMemObject.h"
template<typename T>
struct	OvTPoint2 : public OvMemObject { T	x, y; };
typedef OvTPoint2<OvInt>	OvIPoint2;
typedef OvTPoint2<OvFloat>	OvFPoint2;

template<typename T>
struct	OvTPoint3 : public OvMemObject { T	x, y, z; };
typedef OvTPoint3<OvInt>	OvIPoint3;
typedef OvTPoint3<OvFloat>	OvFPoint3;

//////////////////////////////////////////////////////////////////////////

#define interface_class		class
#define template_class		class
#define abstract_class		class

/// override 예약어는 MS사의 규약이라 만일을 대비해 define 처리함.
#define OVERRIDE			override

//////////////////////////////////////////////////////////////////////////