// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include "GlbWString.h"
#include "gdal.h"
#include "ogrsf_frmts.h"
#include "GlbDataEngineType.h"
#include "GlbPixelBlock.h"
#include "assert.h"

//像素类型转换glb-->gdal
GDALDataType GlbPixelTypeToGDAL(GlbPixelTypeEnum pt);
//像素类型转换gdal-->glb
GlbPixelTypeEnum GlbGDALToPixelType(GDALDataType dt);
void GetRastIOLayout(GlbPixelLayoutTypeEnum layout,
	                 GlbPixelTypeEnum       pixeltype,
					 glbInt32               bands,
					 glbInt32               xsize,
					 glbInt32               ysize,
	                 glbInt32& pixelSpace,glbInt32& lineSpace,glbInt32& bandSpace);
glbBool GlbDownsample_Near
		(
		    CGlbPixelBlock *srcPixelBlock,
			CGlbPixelBlock *dstPixelBlock,
			glbInt32        dstStartCol,
			glbInt32        dstStartRow,
			glbInt32        dstCols,
			glbInt32        dstRows,
			glbBool         hasNodata,
			glbDouble       nodata,
			glbBool         isReplace,
			glbDouble       rpValue
		);
glbBool GlbDownsample_Average
	    (
		    CGlbPixelBlock *srcPixelBlock,
			CGlbPixelBlock *dstPixelBlock,
			glbInt32        dstStartCol,
			glbInt32        dstStartRow,
			glbInt32        dstCols,
			glbInt32        dstRows
		);
glbBool GlbDownsample_Cubic
	    (
		    CGlbPixelBlock *srcPixelBlock,
			CGlbPixelBlock *dstPixelBlock,
			glbInt32        dstStartCol,
			glbInt32        dstStartRow,
			glbInt32        dstCols,
			glbInt32        dstRows,
			glbBool         hasNodata,
			glbDouble       nodata,
			glbBool         isReplace,
			glbDouble       rpValue
		);
glbBool GlbUpsample_Bilinear
	    (
		CGlbPixelBlock *srcBlock,
		CGlbPixelBlock *dstBlock
		);
glbBool GlbUpsample_Average
	(
	CGlbPixelBlock *srcBlock,
	CGlbPixelBlock *dstBlock
	);
// TODO: reference additional headers your program requires here
