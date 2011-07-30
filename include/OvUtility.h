#pragma once

#include <windows.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "OvGlobalFunc.h"
#include "OvUtility_RTTI.h"
#include "OvMacro.h"

//////////////////////////////////////////////////////////////////////////
#include "OvUtility_string.h"
#include "OvUtility_container.h"
#include "OvUtility_path.h"
#include "OvUtility_rtti.h"
#include "OvUtility_thread.h"
#include "OvUtility_dataconvert.h"
//////////////////////////////////////////////////////////////////////////

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif