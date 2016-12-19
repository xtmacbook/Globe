
// stdafx.cpp : source file that includes just the standard includes
// creatarGlobeMfc.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if GLB_USE_VLD > 0
#   ifndef VLD_FORCE_ENABLE
#   define VLD_FORCE_ENABLE
#   endif 

#include "vld.h"
#endif
