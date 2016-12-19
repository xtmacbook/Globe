// stdafx.cpp : source file that includes just the standard includes
// GlbDataEngine.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
#if GLB_USE_VLD > 0
#   ifndef VLD_FORCE_ENABLE
#   define VLD_FORCE_ENABLE
#   endif 

#include "vld.h"
#endif