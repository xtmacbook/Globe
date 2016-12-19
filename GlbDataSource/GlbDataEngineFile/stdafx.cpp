// stdafx.cpp : source file that includes just the standard includes
// GlbDataEngineFile.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information
#pragma warning(disable:4251)
#include "stdafx.h"
#include "IGlbDataEngine.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
GDALDataType GlbPixelTypeToGDAL(GlbPixelTypeEnum pt)
{
	GDALDataType dt;
	switch(pt)
	{
	case GLB_PIXELTYPE_UNKNOWN:
		{
			dt = GDT_Unknown;
			break;
		}
	case GLB_PIXELTYPE_BYTE:
		{
			dt = GDT_Byte;
			break;
		}
	case GLB_PIXELTYPE_USHORT16:
		{
			dt = GDT_UInt16;
			break;
		}
	case GLB_PIXELTYPE_SHORT16:
		{
			dt = GDT_Int16;
			break;
		}
	case GLB_PIXELTYPE_UINT32:
		{
			dt = GDT_UInt32;
			break;
		}
	case GLB_PIXELTYPE_INT32:
		{
			dt = GDT_Int32;
			break;
		}
	case GLB_PIXELTYPE_FLOAT32:
		{
			dt = GDT_Float32;
			break;
		}
	/*case GLB_PIXELTYPE_DOUBLE64:
		{
			dt = GDT_Float64;
			break;
		}*/
	default:
		dt = GDT_Unknown;
	}
	return dt;
};

GlbPixelTypeEnum GlbGDALToPixelType(GDALDataType dt)
{
	GlbPixelTypeEnum pt;
	switch(dt)
	{
	case GDT_Unknown:
		{
			pt = GLB_PIXELTYPE_UNKNOWN;
			break;
		}
	case GDT_Byte:
		{
			pt = GLB_PIXELTYPE_BYTE;
			break;
		}
	case GDT_UInt16:
		{
			pt = GLB_PIXELTYPE_USHORT16;
			break;
		}
	case GDT_Int16:
		{
			pt = GLB_PIXELTYPE_SHORT16;
			break;
		}
	case GDT_UInt32:
		{
			pt = GLB_PIXELTYPE_UINT32;
			break;
		}
	case GDT_Int32:
		{
			pt = GLB_PIXELTYPE_INT32;
			break;
		}
	case GDT_Float32:
		{
			pt = GLB_PIXELTYPE_FLOAT32;
			break;
		}
	/*case GDT_Float64:
		{
			pt = GLB_PIXELTYPE_DOUBLE64;
			break;
		}*/
	default:
		pt = GLB_PIXELTYPE_UNKNOWN;
	}
	return pt;
};
void GetRastIOLayout(GlbPixelLayoutTypeEnum layout,
	                 GlbPixelTypeEnum       pixeltype,
					 glbInt32               bands,
					 glbInt32               xsize,
					 glbInt32               ysize,
	                 glbInt32& pixelSpace,glbInt32& lineSpace,glbInt32& bandSpace)
{
	glbInt32 pixelsize  = getPixelSize(pixeltype);
	if(layout == GLB_PIXELLAYOUT_BIP)
	{//像素交叉
		pixelSpace = bands     * pixelsize;
		lineSpace  = pixelSpace* xsize;
		bandSpace  = pixelsize;
	}
	else if(layout == GLB_PIXELLAYOUT_BSQ)
	{//波段顺序
		pixelSpace = pixelsize;
		lineSpace  = pixelSpace * xsize;
		bandSpace  = lineSpace  * ysize;
	}
	else//波段交叉
	{
		pixelSpace = pixelsize;
		lineSpace  = pixelSpace *xsize;
		bandSpace  = lineSpace;
	}
}
template <class T>
glbBool GlbDownsample_NearT
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
		)
{
	glbInt32 srcCol,srcRow;
	glbInt32 srcRows,srcCols;
	T*       srcValue = NULL;
	glbByte  *srcMask  = NULL;
	T        tNodata = (T)nodata;
	if(hasNodata == false)
		tNodata = (T)0;

	glbInt32 bands = srcPixelBlock->GetBandCount();	
	srcPixelBlock->GetSize(srcCols,srcRows);
	for(glbInt32 col =0; col <dstCols; col++)
	{
		srcCol = (int) (0.5 + (col/(double)dstCols) * srcCols);		
		for(glbInt32 row =0; row <dstRows; row++)
		{
			srcRow = (int) (0.5 + (row/(double)dstRows) * srcRows);
			for(glbInt32 band =0; band<bands ;band++)
			{				
				srcPixelBlock->GetPixelVal(band,srcCol,srcRow,(void**)&srcValue);
				srcPixelBlock->GetMaskVal(srcCol,srcRow,&srcMask);
				if(*srcMask == 0 /*&& *srcValue == tNodata*/ && isReplace)
				{
					*srcValue = (T)rpValue;
					*srcMask  = 255;
				}
				dstPixelBlock->SetPixelVal(band,
					                       dstStartCol + col,
										   dstStartRow + row,
										   srcValue);				
				dstPixelBlock->SetMaskVal(dstStartCol + col,
										  dstStartRow + row,
										  *srcMask);
			}
		}
	}
	return true;
}
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
		)
{
	glbBool rt = false;
	GlbPixelTypeEnum pixelType = srcPixelBlock->GetPixelType();
	switch(pixelType)
	{
	case GLB_PIXELTYPE_BYTE:
		rt = GlbDownsample_NearT<glbByte>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_USHORT16:
		rt = GlbDownsample_NearT<glbUInt16>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_SHORT16:
		rt = GlbDownsample_NearT<glbInt16>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_UINT32:
		rt = GlbDownsample_NearT<glbUInt32>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_INT32:
		rt = GlbDownsample_NearT<glbInt32>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_FLOAT32:
		rt = GlbDownsample_NearT<glbFloat>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	}
	return rt;
}

template <class T>
glbBool GlbDownsample_AverageT
        (
		    CGlbPixelBlock *srcPixelBlock,
			CGlbPixelBlock *dstPixelBlock,
			glbInt32        dstStartCol,
			glbInt32        dstStartRow,
			glbInt32        dstCols,
			glbInt32        dstRows
		)
{
	glbInt32 srcCol, srcRow;
	glbInt32 srcCol2,srcRow2;
	glbInt32 srcRows,srcCols;
	T*       srcValue = NULL;
	glbByte  *srcMask  = NULL;

	glbInt32 bands = srcPixelBlock->GetBandCount();	
	srcPixelBlock->GetSize(srcCols,srcRows);
	for(glbInt32 col =0; col <dstCols; col++)
	{
		srcCol = (int) (0.5 + (col    /(double)dstCols) * srcCols);
		srcCol2= (int) (0.5 + ((col+1)/(double)dstCols) * srcCols);
		
		if(srcCol2 > srcCols || col == dstCols-1)
			srcCol2 = srcCols;
		for(glbInt32 row =0; row <dstRows; row++)
		{
			srcRow = (int) (0.5 + (row    /(double)dstRows) * srcRows);        
            srcRow2= (int) (0.5 + ((row+1)/(double)dstRows) * srcRows);
			if( srcRow2 > srcRows || row == dstRows -1 )
				srcRow2 = srcRows;

			for(glbInt32 band =0; band<bands; band++)
			{
				glbDouble srcTotal = 0;
				glbInt32  srcCount = 0;
				for(glbInt32 iCol = srcCol; iCol<srcCol2; iCol++)
				{
					for(glbInt32 iRow = srcRow; iRow<srcRow2; iRow++)
					{
						srcPixelBlock->GetPixelVal(band,iCol,iRow,(void**)&srcValue);
						srcPixelBlock->GetMaskVal(iCol,iRow,&srcMask);						
						if(*srcMask == 0) continue;
						srcTotal += *srcValue;
						srcCount ++;
					}
				}//Col

				if(srcCount == (srcCol2 - srcCol) * (srcRow2 - srcRow))
				{
					dstPixelBlock->SetMaskVal(col + dstStartCol,row + dstStartRow,255);
					T pixelval = (T)(srcTotal /srcCount);					
					dstPixelBlock->SetPixelVal(band,col + dstStartCol,row + dstStartRow,&pixelval);
				}
				else
				{
					dstPixelBlock->SetMaskVal(col + dstStartCol,row + dstStartRow,0);
					//dstPixelBlock->SetPixelVal(band,col + dstStartCol,row + dstStartRow,&tNodata);
				}
			}//band			
		}//row
	}//col
	return true;
}
glbBool GlbDownsample_Average
	    (
		    CGlbPixelBlock *srcPixelBlock,
			CGlbPixelBlock *dstPixelBlock,
			glbInt32        dstStartCol,
			glbInt32        dstStartRow,
			glbInt32        dstCols,
			glbInt32        dstRows
		)
{
	glbBool rt = false;
	GlbPixelTypeEnum pixelType = srcPixelBlock->GetPixelType();
	switch(pixelType)
	{
	case GLB_PIXELTYPE_BYTE:
		rt = GlbDownsample_AverageT<glbByte>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows
			);break;
	case GLB_PIXELTYPE_USHORT16:
		rt = GlbDownsample_AverageT<glbUInt16>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows
			);break;
	case GLB_PIXELTYPE_SHORT16:
		rt = GlbDownsample_AverageT<glbInt16>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows
			);break;
	case GLB_PIXELTYPE_UINT32:
		rt = GlbDownsample_AverageT<glbUInt32>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows
			);break;
	case GLB_PIXELTYPE_INT32:
		rt = GlbDownsample_AverageT<glbInt32>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows
			);break;
	case GLB_PIXELTYPE_FLOAT32:
		rt = GlbDownsample_AverageT<glbFloat>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows
			);break;
	}
	return rt;
}

template <class T>
glbBool GlbDownsample_CubicT
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
		)
{
	glbInt32 srcCol, srcRow;
	glbInt32 srcCol2,srcRow2;
	glbInt32 srcRows,srcCols;
	T        tNodata = (T)nodata;
	glbByte  *srcMask = NULL;
	T*       srcValue = NULL;
	if(hasNodata == false)
		tNodata = (T)0.0;

	glbInt32 bands = srcPixelBlock->GetBandCount();	
	srcPixelBlock->GetSize(srcCols,srcRows);
	for(glbInt32 col =0; col <dstCols; col++)
	{
		srcCol  = (int) floor(((col+0.5)/(double)dstCols) * srcCols - 0.5)-1;
        srcCol2 = srcCol + 4;
        if(srcCol < 0)srcCol = 0;
        if( srcCol > srcCols || col == dstCols-1 )
			srcCol2 = srcCols;
		for(glbInt32 row =0; row <dstRows; row++)
		{
			srcRow  = (int) floor(((row+0.5)/(double)dstRows) * srcRows - 0.5)-1;
			srcRow2 = srcRow + 4;
			if(srcRow  < 0)srcRow = 0;			
			if(srcRow2 > srcRows || row == dstRows-1 )
				srcRow2 = srcRows;
			for(glbInt32 band=0; band<bands; band++)
			{
				if( srcRow2 - srcRow != 4 || srcCol2 - srcCol != 4 )
				{//不满足4*4窗口
					int nLSrcYOff = (int) (0.5+(row/(double)dstRows) * srcRows);
					int nLSrcXOff = (int) (0.5+(col/(double)dstCols) * srcCols);					
					if( nLSrcYOff > srcRows - 1 )
						nLSrcYOff = srcRows - 1;
					srcPixelBlock->GetPixelVal(band,nLSrcXOff,nLSrcYOff,(void**)&srcValue);
					srcPixelBlock->GetMaskVal(nLSrcXOff,nLSrcYOff,&srcMask);
					if(*srcMask == 0 /*&& *srcValue == tNodata*/ && isReplace)
					{
						*srcMask = 255;
						*srcValue= (T)rpValue;
					}
					dstPixelBlock->SetPixelVal(band,
						                       col + dstStartCol,
											   row + dstStartRow,
											   srcValue);
					dstPixelBlock->SetMaskVal(col + dstStartCol,
											  row + dstStartRow,
											  *srcMask);
				}else{
					#define CubicConvolution(distance1,distance2,distance3,f0,f1,f2,f3) \
                                            (( -f0 + f1 - f2 + f3) * distance3\
                                             + (2.0*(f0 - f1) + f2 - f3) * distance2\
                                             + (-f0+ f2) * distance1\
                                             + f1)					
					T         adfRowResults[4];
					glbByte   adfRowMasks[4];
					glbDouble dfSrcX    = (((col+0.5)/(double)dstCols)*srcCols);
					glbDouble dfDeltaX  = dfSrcX - 0.5 - (srcCol + 1);
                    glbDouble dfDeltaX2 = dfDeltaX  * dfDeltaX;
                    glbDouble dfDeltaX3 = dfDeltaX2 * dfDeltaX;
                    for (glbInt32 ic = 0; ic < 4; ic++ )
                    {
						T       pafSrcRow[4];						
						glbByte pafSrcRowMask[4];
						for( glbInt32 ir =0; ir <4; ir++ )
						{
							srcPixelBlock->GetPixelVal(band,
								                       srcCol + ic,
													   srcRow + ir,
													   (void**)&srcValue);							
							srcPixelBlock->GetMaskVal(srcCol + ic,
													  srcRow + ir,
													  &srcMask);
							if(*srcMask == 0 /*&& *srcValue == tNodata*/ && isReplace)
							{
								*srcMask = 255;
								*srcValue= (T)tNodata;
							}
							pafSrcRow[ic]     = *srcValue;
							pafSrcRowMask[ic] = *srcMask;
							
						}	                                          
						
                        adfRowResults[ic] =(T)CubicConvolution(dfDeltaX, dfDeltaX2, dfDeltaX3,
                                            pafSrcRow[0],
                                            pafSrcRow[1],
                                            pafSrcRow[2],
                                            pafSrcRow[3]);
						adfRowMasks[ic]  = (glbByte)CubicConvolution(dfDeltaX, dfDeltaX2, dfDeltaX3,
                                            pafSrcRowMask[0],
                                            pafSrcRowMask[1],
                                            pafSrcRowMask[2],
                                            pafSrcRowMask[3] );
					}
					glbDouble dfSrcY    = (((row+0.5)/(double)dstRows) * srcRows);
					glbDouble dfDeltaY  = dfSrcY - 0.5 - (srcRow + 1);
					glbDouble dfDeltaY2 = dfDeltaY * dfDeltaY;
					glbDouble dfDeltaY3 = dfDeltaY2 * dfDeltaY;

					T dstValue = (T)CubicConvolution(dfDeltaY, dfDeltaY2, dfDeltaY3,
                                        adfRowResults[0],
                                        adfRowResults[1],
                                        adfRowResults[2],
                                        adfRowResults[3] );
					glbByte dstMask =(glbByte)CubicConvolution(dfDeltaY, dfDeltaY2, dfDeltaY3,
                                        adfRowMasks[0],
                                        adfRowMasks[1],
                                        adfRowMasks[2],
                                        adfRowMasks[3] );
					if(dstMask == 0)
					{
						dstPixelBlock->SetPixelVal(band,
						                       col + dstStartCol,
											   row + dstStartRow,
											   &tNodata);
						dstPixelBlock->SetMaskVal(col + dstStartCol,
											      row + dstStartRow,
											      0);
					}else{
						dstPixelBlock->SetPixelVal(band,
						                       col + dstStartCol,
											   row + dstStartRow,
											   &dstValue);
						dstPixelBlock->SetMaskVal(col + dstStartCol,
											      row + dstStartRow,
											      255);
					}
				}//满足4*4窗口
			}//波段
		}//行
	}//列
	return true;
}
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
		)
{
	glbBool rt = false;
	GlbPixelTypeEnum pixelType = srcPixelBlock->GetPixelType();
	switch(pixelType)
	{
	case GLB_PIXELTYPE_BYTE:
		rt = GlbDownsample_CubicT<glbByte>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_USHORT16:
		rt = GlbDownsample_CubicT<glbUInt16>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_SHORT16:
		rt = GlbDownsample_CubicT<glbInt16>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_UINT32:
		rt = GlbDownsample_CubicT<glbInt32>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_INT32:
		rt = GlbDownsample_CubicT<glbInt32>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	case GLB_PIXELTYPE_FLOAT32:
		rt = GlbDownsample_CubicT<glbFloat>
			(
			   srcPixelBlock,
			   dstPixelBlock,
			   dstStartCol,
			   dstStartRow,
			   dstCols,
			   dstRows,
			   hasNodata,
			   nodata,
			   isReplace,
			   rpValue
			);break;
	}
	return rt;
}

template<class T>
glbBool GlbUpsample_BilinearT
(
	CGlbPixelBlock *srcBlock,
	CGlbPixelBlock *dstBlock
)
{
	glbInt32 srcCols,srcRows;
	srcBlock->GetSize(srcCols,srcRows);
	glbInt32 dstCols,dstRows;
	dstBlock->GetSize(dstCols,dstRows);
	glbInt32 bands = dstBlock->GetBandCount();
	glbFloat colScale = (glbFloat)srcCols/dstCols;
	glbFloat rowScale = (glbFloat)srcRows/dstRows;
	glbFloat srcColf = 0.0;
	glbFloat srcRowf = 0.0;
	glbInt32 srcCol  = 0;
	glbInt32 srcRow  = 0;

	glbByte* dmask11 = NULL;
	glbByte* dmask12 = NULL;
	glbByte* dmask21 = NULL;
	glbByte* dmask22 = NULL;
	T*       d11     = NULL;
	T*       d12     = NULL;
	T*       d21     = NULL;
	T*       d22     = NULL;
	for(glbInt32 dstRow =0;dstRow <dstRows;dstRow++)
	{
		srcRowf = dstRow*rowScale;
		if(srcRowf <=1e-8)srcRowf = 0.0;
		if(srcRowf >srcRows - 2)srcRowf = (glbFloat)srcRows - 2;
		srcRow = (glbInt32)srcRowf;
		for(glbInt32 dstCol =0;dstCol <dstCols;dstCol++)
		{
			srcColf = dstCol*colScale;
			if(srcColf <=1e-8)srcColf = 0.0;
			if(srcColf > srcCols -2)srcColf = (glbFloat)srcCols -2;
			srcCol = (glbInt32)srcColf;
			for(glbInt32 band =0;band <bands;band++)
			{				
				srcBlock->GetMaskVal(srcCol,srcRow,&dmask11);
				srcBlock->GetMaskVal(srcCol+1,srcRow,&dmask12);
				srcBlock->GetMaskVal(srcCol,srcRow+1,&dmask21);
				srcBlock->GetMaskVal(srcCol+1,srcRow+1,&dmask22);
				T val = 0;
				if(*dmask11 == 255)
				{
					srcBlock->GetPixelVal(band,srcCol,srcRow,(void**)&d11);
					val += (T)(*d11*(srcCol + 1 - srcColf)*(srcRow+1 - srcRowf));
				}
				if(*dmask12 == 255)
				{
					srcBlock->GetPixelVal(band,srcCol+1,srcRow,(void**)&d12);
					val += (T)(*d12*(srcCol + 1 - srcColf)*(srcRowf - srcRow));
				}
				if(*dmask21 == 255)
				{
					srcBlock->GetPixelVal(band,srcCol,srcRow+1,(void**)&d21);
					val += (T)(*d21*(srcColf - srcCol)*(srcRow+1 -srcRowf));
				}
				if(*dmask22 == 255)
				{
					srcBlock->GetPixelVal(band,srcCol+1,srcRow+1,(void**)&d22);
					val += (T)(*d22*(srcColf - srcCol)*(srcRowf - srcRow ));
				}
				
				if(*dmask11 == 255||*dmask12 == 255
					|| *dmask21 == 255 ||*dmask22 == 255)
				{
					dstBlock->SetPixelVal(band,dstCol,dstRow,&val);
					dstBlock->SetMaskVal(dstCol,dstRow,255);
				}else{
					dstBlock->SetPixelVal(band,dstCol,dstRow,&val);
					dstBlock->SetMaskVal(dstCol,dstRow,0);
				}
			}
		}//col
	}//row
	return true;
}

glbBool GlbUpsample_Average
	(
	CGlbPixelBlock *srcBlock,
	CGlbPixelBlock *dstBlock
	)
{
	glbInt32 srcCols,srcRows;
	srcBlock->GetSize(srcCols,srcRows);
	glbInt32 dstCols,dstRows;
	dstBlock->GetSize(dstCols,dstRows);

	glbFloat *p_data = NULL;
	glbByte *p_mask = NULL;
	srcBlock->GetPixelData((glbByte**)&p_data);
	srcBlock->GetMaskData(&p_mask);
	
	glbFloat *p_new_data = NULL;
	glbByte *p_new_mask = NULL;
	dstBlock->GetPixelData((glbByte**)&p_new_data);
	dstBlock->GetMaskData(&p_new_mask);

#define DEMTILESZIE 17

	//
	//左上角
	for(glbInt32 i = 0;i <= DEMTILESZIE / 2;i++)//行
	{
		for(glbInt32 j = 0;j <= DEMTILESZIE / 2;j++)//列
		{
			//替换有效值
			p_new_data[ 2 * i * DEMTILESZIE * 2 + j * 2] = p_data[i * DEMTILESZIE + j];
			*(p_new_mask + 2 * i * DEMTILESZIE * 2 + j * 2) = *(p_mask + i * DEMTILESZIE + j);

			//组建插值
			p_new_data[ 2 * i * DEMTILESZIE * 2 + j * 2 + 1] = (p_data[ i * DEMTILESZIE + j] + p_data[ i * DEMTILESZIE + j + 1]) * 0.5;	
			*(p_new_mask +2 *  i * DEMTILESZIE * 2 + j * 2 + 1) = 255;
		}
	}

	//右上角
	for(glbInt32 i = 0;i <= DEMTILESZIE / 2 ;i++)//行
	{
		for(glbInt32 j = DEMTILESZIE / 2;j < DEMTILESZIE;j++)//列
		{
			//替换有效值
			p_new_data[ 2 * i * DEMTILESZIE * 2 + j * 2 + 1] = p_data[i * DEMTILESZIE + j];
			*(p_new_mask + 2 * i * DEMTILESZIE * 2 + j * 2 + 1) = *(p_mask + i * DEMTILESZIE + j );

			//组建插值
			if(j < DEMTILESZIE - 1)
			{
				p_new_data[ 2 * i * DEMTILESZIE * 2+j*2 + 2] = (p_data[ i * DEMTILESZIE + j] + p_data[ i * DEMTILESZIE + j + 1]) * 0.5;	
				*(p_new_mask + 2 * i * DEMTILESZIE * 2 + j * 2 + 2) = 255;
			}
		}
	}

	//左下角
	for(glbInt32 i = DEMTILESZIE / 2;i < DEMTILESZIE;i++)//行
	{
		for(glbInt32 j = 0;j <= DEMTILESZIE / 2;j++)//列
		{
			//替换有效值
			p_new_data[(2 * i + 1) * DEMTILESZIE * 2 + j * 2] = p_data[i * DEMTILESZIE + j];
			*(p_new_mask + (2 * i + 1) * DEMTILESZIE * 2 + j * 2) = *(p_mask + i * DEMTILESZIE + j );

			//组建插值
			p_new_data[(2 * i + 1)*DEMTILESZIE * 2 + j * 2 + 1] = (p_data[ i * DEMTILESZIE + j] + p_data[ i * DEMTILESZIE + j + 1]) * 0.5;	
			*(p_new_mask + (2 * i + 1) * DEMTILESZIE * 2 + j * 2 + 1) = 255;
		}
	}

	//右下角
	for(glbInt32 i = DEMTILESZIE / 2;i < DEMTILESZIE;i++)//行
	{
		for(glbInt32 j = DEMTILESZIE / 2;j < DEMTILESZIE;j++)//列
		{
			//替换有效值
			p_new_data[(i * 2 + 1) *  DEMTILESZIE * 2 + j * 2 + 1] = p_data[i * DEMTILESZIE + j ];
			*(p_new_mask + (i * 2 + 1) * DEMTILESZIE * 2 + j * 2 + 1) = *(p_mask + i * DEMTILESZIE + j );

			//组建插值
			if(j < DEMTILESZIE - 1)
			{
				p_new_data[(i * 2 + 1) * DEMTILESZIE * 2+j*2+2] = (p_data[ i * DEMTILESZIE + j] + p_data[ i * DEMTILESZIE + j + 1]) * 0.5;	
				*(p_new_mask + (i * 2 + 1) * DEMTILESZIE * 2 + j * 2 + 2) = 255;
			}
		}
	}

	//上半部分
	for (glbInt32 i = 1 ; i < DEMTILESZIE - 1; i+=2)
	{
		for(glbInt32 j = 0; j < DEMTILESZIE * 2; j++)
		{			
			p_new_data[ i * DEMTILESZIE * 2 + j] = (p_new_data[( i - 1 ) * DEMTILESZIE * 2 + j] + p_new_data[( i + 1 ) * DEMTILESZIE * 2 + j]) * 0.5;
			*(p_new_mask +  i * DEMTILESZIE * 2 + j) = 255;
		}
	}
	
	//下半部分
	for (glbInt32 i = DEMTILESZIE + 1; i < DEMTILESZIE * 2 - 1; i+=2)
	{
		for(glbInt32 j = 0; j < DEMTILESZIE * 2; j++)
		{			
			p_new_data[ i * DEMTILESZIE * 2 + j] = (p_new_data[( i - 1 ) * DEMTILESZIE * 2 + j] + p_new_data[( i + 1 ) * DEMTILESZIE * 2 + j]) * 0.5;
			*(p_new_mask + i * DEMTILESZIE * 2 + j) = 255;
		}
	}
	return true;
}

glbBool GlbUpsample_Bilinear
(
	CGlbPixelBlock *srcBlock,
	CGlbPixelBlock *dstBlock
)
{
	glbBool          rt        = false;
	GlbPixelTypeEnum pixelType = srcBlock->GetPixelType();
	switch(pixelType)
	{
	case GLB_PIXELTYPE_BYTE:
		rt = GlbUpsample_BilinearT<glbByte>
			(
			   srcBlock,
			   dstBlock
			);break;
	case GLB_PIXELTYPE_USHORT16:		
		rt = GlbUpsample_BilinearT<glbUInt16>
			(
			   srcBlock,
			   dstBlock
			);break;
	case GLB_PIXELTYPE_SHORT16:
		rt = GlbUpsample_BilinearT<glbInt16>
			(
			   srcBlock,
			   dstBlock
			);break;
	case GLB_PIXELTYPE_UINT32:
		rt = GlbUpsample_BilinearT<glbInt32>
			(
			   srcBlock,
			   dstBlock
			);break;
	case GLB_PIXELTYPE_INT32:
		rt = GlbUpsample_BilinearT<glbInt32>
			(
			   srcBlock,
			   dstBlock
			);break;
	case GLB_PIXELTYPE_FLOAT32:
		rt = GlbUpsample_BilinearT<glbFloat>
			(
			   srcBlock,
			   dstBlock
			);break;
	}
	return rt;
}
