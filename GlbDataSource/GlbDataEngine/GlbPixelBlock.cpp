#include "StdAfx.h"
#include "GlbPixelBlock.h"
#include "IGlbDataEngine.h"
#include "gdal.h"
#include "GlbString.h"
#include "cpl_string.h"
#include "GlbPath.h"
#include <fstream>
#include <iomanip>

#ifndef byte
typedef unsigned char byte;
#endif

#define  NullValue 10000000

static GDALDataType GlbPixelTypeToGDAL(GlbPixelTypeEnum pt)
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
	case GLB_PIXELTYPE_DOUBLE64:
		{
			dt = GDT_Float64;
			break;
		}
	default:
		dt = GDT_Unknown;
	}
	return dt;
};

CGlbPixelBlock::CGlbPixelBlock(GlbPixelTypeEnum pixelType,GlbPixelLayoutTypeEnum pixelLayout)
{
	mpr_pixelType   = pixelType;
	mpr_pixelLayout = pixelLayout;
	mpr_nBands      = 0;
	mpr_rows        = 0;
	mpr_colums      = 0;
	mpr_data        = NULL;
	mpr_maskdata    = NULL;
	mpr_extent      = NULL;
}

CGlbPixelBlock::~CGlbPixelBlock()
{
	if (mpr_data != NULL)
	{
		free(mpr_data);
	}
	if (mpr_maskdata != NULL)
	{
		free(mpr_maskdata);
	}
	mpr_extent      = NULL;
}

glbBool CGlbPixelBlock::Initialize( glbInt32 nBands, glbInt32 columns, glbInt32 rows,glbDouble nodata/*=0*/ )
{
	if (nBands<=0 || rows<=0 || columns<=0)
	{
		GlbSetLastError(L"所有参数必须>0");
		return false;
	}

	mpr_nBands  = nBands;
	mpr_colums  = columns;
	mpr_rows    = rows;
	int dataLen = getPixelSize(mpr_pixelType)*nBands*rows*columns;	
	int maskLen = rows*columns*sizeof(glbByte);

	mpr_data = (glbByte*)malloc(dataLen);
	if (mpr_data == NULL)
	{
		GlbSetLastError(L"申请内存失败");
		return false;
	}
	mpr_maskdata = (glbByte*)malloc(maskLen);
	if (mpr_maskdata == NULL)
	{
		GlbSetLastError(L"申请内存失败");
		return false;
	}
	Clear(nodata);
	return true;	
}

glbBool CGlbPixelBlock::IsValid()
{
	return mpr_data && mpr_maskdata;		
}

glbInt32 CGlbPixelBlock::GetPixelData(glbByte** ppData)
{
	if (IsValid() && ppData != NULL)
	{
		*ppData = mpr_data;
		return getPixelSize(mpr_pixelType)*mpr_nBands*mpr_rows*mpr_colums;//长度
	}
	return 0;
}

glbInt32 CGlbPixelBlock::GetMaskData(glbByte** ppMask)
{
	if (IsValid() && ppMask != NULL)
	{
		*ppMask = mpr_maskdata;
		return mpr_rows*mpr_colums;//长度
	}
	return 0;
}

glbInt32 CGlbPixelBlock::GetBandCount()
{
	return mpr_nBands;
}

GlbPixelTypeEnum CGlbPixelBlock::GetPixelType()
{
	return mpr_pixelType;
}

void CGlbPixelBlock::GetSize(glbInt32&colums, glbInt32&rows)
{
	colums = mpr_colums;
	rows   = mpr_rows;
}

GlbPixelLayoutTypeEnum CGlbPixelBlock::GetPixelLayout()
{
	return mpr_pixelLayout;
}

glbBool CGlbPixelBlock::GetPixelVal( glbInt32 iband,  glbInt32 col,  glbInt32 row,  void** ppVal)
{
	if (!IsValid()
		||iband<0 
		||iband>=mpr_nBands 
		||col<0
		||col>= mpr_colums 
		||row<0
		||row>=mpr_rows
		||ppVal==NULL)
	{
		GlbSetLastError(L"无效参数");
		return false;
	}
	glbInt32 pixelSize = getPixelSize(mpr_pixelType);
	if (mpr_pixelLayout ==GLB_PIXELLAYOUT_BSQ)//band
	{//波段顺序
		byte *src =  (byte*)mpr_data;
		src       += mpr_colums*mpr_rows*pixelSize*iband;
		src       += (col+row*mpr_colums)*pixelSize;
		*ppVal = src;
	}
	else if (mpr_pixelLayout == GLB_PIXELLAYOUT_BIP)//pixel
	{//像素交叉
		byte *src = (byte*)mpr_data;
		glbInt32 packPixelSize =0;
		packPixelSize += pixelSize*mpr_nBands;
		src           += (col + row*mpr_colums)*packPixelSize;
		src           += pixelSize*iband;
		*ppVal = src;
	}
	else if (mpr_pixelLayout == GLB_PIXELLAYOUT_BIL)//line
	{//波段交叉
		byte *src = (byte*)mpr_data;
		glbInt32 lineSize =0;
		lineSize += pixelSize*mpr_nBands*mpr_colums;
		src      += row*lineSize;//line offset
		src      += pixelSize*iband*mpr_colums;//band offset
		src      += col*pixelSize;//pixel offset
		*ppVal = src;
	}
	return true;
}

glbBool CGlbPixelBlock::SetPixelVal( glbInt32 iband, glbInt32 col, glbInt32 row, void* ppVal )
{
	if (!IsValid()
		||iband<0
		||iband>=mpr_nBands
		||col<0
		||col>= mpr_colums
		||row<0
		||row>=mpr_rows
		||ppVal==NULL)
	{
		GlbSetLastError(L"无效参数");
		return false;
	}
	glbInt32 pixelSize = getPixelSize(mpr_pixelType);
	byte *src = (byte*)mpr_data;
	if (mpr_pixelLayout ==GLB_PIXELLAYOUT_BSQ)//band
	{//波段顺序		
		src += mpr_colums*mpr_rows*pixelSize*iband;
		src += (col+row*mpr_colums)*pixelSize;//pixel offset
		memcpy(src, ppVal, pixelSize);
	}
	else if (mpr_pixelLayout == GLB_PIXELLAYOUT_BIP)//pixel
	{//像素交叉
		glbInt32 packPixelSize =0;
		packPixelSize +=  pixelSize*mpr_nBands;
		src           += (col+row*mpr_colums)*packPixelSize;//pack pixel offset
		src           += pixelSize*iband;
		memcpy(src, ppVal, pixelSize);
	}
	else if (mpr_pixelLayout == GLB_PIXELLAYOUT_BIL)//line
	{//波段交叉		
		glbInt32 lineSize =0;
		lineSize += pixelSize*mpr_nBands*mpr_colums;
		src      += row*lineSize;//line offset
		src      += pixelSize*iband*mpr_colums;//band offset
		src      += col*pixelSize;//pixel offset
		memcpy(src, ppVal, pixelSize);
	}
	return true;
}

glbBool CGlbPixelBlock::GetMaskVal(glbInt32 col,  glbInt32 row,  glbByte** ppVal)
{
	if (!IsValid()
		||col<0
		||col>= mpr_colums
		||row<0
		||row>=mpr_rows
		||ppVal==NULL)
	{
		GlbSetLastError(L"无效参数");
		return false;
	}
	glbByte *src = mpr_maskdata;
	src += row*mpr_colums;
	src += col;
	*ppVal = src;
	return true;
}

glbBool CGlbPixelBlock::SetMaskVal(glbInt32 col,  glbInt32 row,  glbByte mVal)
{
	if (!IsValid()
		||col<0
		||col>= mpr_colums
		||row<0
		||row>=mpr_rows)
	{
		GlbSetLastError(L"无效参数");
		return false;
	}
	glbByte *src = mpr_maskdata;
	src += row*mpr_colums;
	src += col;
	*src = mVal;
	return true;
}

static glbBool ModifyPixelVal(glbDouble dval, glbInt32 x, glbInt32 y, glbInt32 iband, CGlbPixelBlock* tempB);

glbBool CGlbPixelBlock::Clear( double nodata/*=0*/ )
{
	if (!IsValid())
	{
		return false;
	}
	int pixelSize = getPixelSize(mpr_pixelType);
	memset(mpr_maskdata,0,mpr_rows*mpr_colums);
	memset(mpr_data,0,mpr_rows*mpr_colums * pixelSize*mpr_nBands);
	glbBool rt = true;
	for(glbInt32 r = 0;r < mpr_rows;r++)
		for(glbInt32 c = 0;c < mpr_colums;c++)
			rt = ModifyPixelVal(nodata,c,r,0,this);
	return rt;
}

glbBool CGlbPixelBlock::ExportToText(const glbWChar* fileName)
{
	if (fileName == NULL)
	{
		return false;
	}
	wofstream fout(fileName);
	for (glbInt32 i=0;i<mpr_rows;i++)
	{
		int printCount = 0;
		fout<< L"row " << setw( 2 ) << i << L" data:";
		for (glbInt32 j=0;j<mpr_colums;j++)
		{
			printCount++;
			if(printCount > 10 && printCount % 10 == 1)
				fout << setw(12) << " ";
				//fout << "            ";
			glbByte* bmask = NULL;
			GetMaskVal(j,i,&bmask);
			fout<<L"("<< setw( 3 ) << *bmask<<L",";
			for (glbInt32 k=0;k<mpr_nBands;k++)
			{
				void* pval = NULL;
				GetPixelVal(k,j,i,&pval);
				switch(mpr_pixelType)
				{
				case GLB_PIXELTYPE_BYTE:
					{
						glbByte val = *(glbByte*)pval;
						fout<< setw( 7 ) << val;
					}
					break;
				case	GLB_PIXELTYPE_USHORT16:
					{
						glbUInt16 val = *(glbUInt16*)pval;
						fout<< setw( 7 ) << val;
					}
					break;
				case	GLB_PIXELTYPE_SHORT16:
					{
						glbInt16 val = *(glbInt16*)pval;
						fout<< setw( 7 ) << val;
					}
					break;
				case	GLB_PIXELTYPE_UINT32:
					{
						glbUInt32 val = *(glbUInt32*)pval;
						fout<< setw( 7 ) << val;
					}
					break;
				case	GLB_PIXELTYPE_INT32:
					{
						glbInt32 val = *(glbInt32*)pval;
						fout<< setw( 7 ) << val;
					}
					break;
				case	GLB_PIXELTYPE_FLOAT32:
					{
						glbFloat val = *(glbFloat*)pval;
						fout<< setw( 7 ) << val;
					}
					break;
				case	GLB_PIXELTYPE_DOUBLE64:
					{
						glbDouble val = *(glbDouble*)pval;
						fout<< setw( 7 ) << val;
					}
					break;
				}
			}
			fout<<L")";

			if(printCount != mpr_colums)
				fout << L",";

			if(printCount > 1 && printCount % 10 == 0)
				fout<<endl;
		}
		if(printCount % 10 != 0)
			fout<<endl;
	}
	fout.close();
	return true;
}
glbBool CGlbPixelBlock::ExportToTiff(const glbWChar* fileName,
										const CGlbExtent *extent,
										const glbWChar *srs)
{
	GDALDriverH pDstDriver = GDALGetDriverByName("GTIFF");//BMP,JPEG
	if (pDstDriver == NULL)
	{ 
		return false;
	}
	glbChar **papszOptions = NULL;
	//构造参数，压缩、块大小、pixelLayout
	if (mpr_colums>128 && mpr_rows>128)
	{
		papszOptions = CSLSetNameValue(papszOptions, "TILED", "YES");
		papszOptions = CSLSetNameValue(papszOptions,"BLOCKXSIZE", "128");
		papszOptions = CSLSetNameValue(papszOptions,"BLOCKYSIZE", "128");
	}
	if (mpr_pixelLayout == GLB_PIXELLAYOUT_BSQ)
	{
		papszOptions = CSLSetNameValue(papszOptions,"INTERLEAVE","BAND");
	}
	else if (mpr_pixelLayout == GLB_PIXELLAYOUT_BIP)
	{
		papszOptions = CSLSetNameValue(papszOptions,"INTERLEAVE","PIXEL");
	}
	else if (mpr_pixelLayout == GLB_PIXELLAYOUT_BIL)
	{
		papszOptions = CSLSetNameValue(papszOptions,"INTERLEAVE","LINE");
	}
	
	CGlbWString strtmp  = fileName;
	GDALDatasetH pOutDS = GDALCreate(pDstDriver,
		                             strtmp.ToString().c_str(),
									 mpr_colums,
									 mpr_rows,
									 mpr_nBands,
									 GlbPixelTypeToGDAL(mpr_pixelType),
									 papszOptions);
	CSLDestroy(papszOptions);
	if(pOutDS == NULL)
	{
		CGlbString err = CPLGetLastErrorMsg();
		GlbSetLastError(err.ToWString().c_str());
	}
	//设置坐标信息
	if (srs != NULL)
	{
		CGlbWString strsrs = srs;
		if(GDALSetProjection(pOutDS, strsrs.ToString().c_str())!= CE_None)
		{
			CGlbString err = CPLGetLastErrorMsg();
			GlbSetLastError(err.ToWString().c_str());
			GDALClose(pOutDS);
			DeleteFile(fileName);
			return false;
		}
	}
	glbDouble minx,maxx,miny,maxy;
	if(extent == NULL)
	{
		extent = mpr_extent.get();
	}
	if(extent == NULL)
	{
		minx = 0;
		maxx = mpr_colums;
		miny = 0;
		maxy = mpr_rows;
	}
	else
	{
		extent->Get(&minx,&maxx,&miny,&maxy);
	}
	//设置外包信息
	double dGeoTrans[6] = {0};
	dGeoTrans [0] = minx;//屏幕左上横向坐标
	dGeoTrans [3] = maxy; //屏幕左上纵向坐标
	dGeoTrans [1] = (maxx-minx)/mpr_colums;// 横向分辨率
	dGeoTrans [5] = (miny-maxy)/mpr_rows;// 纵向分辨率 topleft is origin，所以应该取反
	if(GDALSetGeoTransform(pOutDS, dGeoTrans)!= CE_None)
	{
		CGlbString err = CPLGetLastErrorMsg();
		GlbSetLastError(err.ToWString().c_str());
		GDALClose(pOutDS);
		DeleteFile(fileName);
		return false;
	}
	glbInt32 nPixelSpace,nLineSpace,nBandSpace; 
	if (mpr_pixelLayout == GLB_PIXELLAYOUT_BSQ)
	{//波段顺序
		nPixelSpace = getPixelSize(mpr_pixelType);
		nLineSpace  = nPixelSpace*mpr_colums*mpr_rows;
		nBandSpace  = 1;
		GDALDatasetRasterIO(pOutDS, GF_Write,0,0,mpr_colums,mpr_rows,mpr_data,mpr_colums,mpr_rows,
			GlbPixelTypeToGDAL(mpr_pixelType),mpr_nBands,NULL,nPixelSpace,nLineSpace,nBandSpace);
	}
	else if (mpr_pixelLayout == GLB_PIXELLAYOUT_BIP)
	{//像素交叉
		nPixelSpace = mpr_nBands * getPixelSize(mpr_pixelType);
		nLineSpace = nPixelSpace*mpr_colums;
		nBandSpace = 1;
		GDALDatasetRasterIO(pOutDS, GF_Write,0,0,mpr_colums,mpr_rows,mpr_data,mpr_colums,mpr_rows,
			GlbPixelTypeToGDAL(mpr_pixelType),mpr_nBands,NULL,nPixelSpace,nLineSpace,nBandSpace);
	}
	else if (mpr_pixelLayout == GLB_PIXELLAYOUT_BIL)
	{//波段交叉
		nPixelSpace = getPixelSize(mpr_pixelType);
		nLineSpace = nPixelSpace*mpr_colums;
		nBandSpace = nLineSpace*mpr_rows;
		GDALDatasetRasterIO(pOutDS, GF_Write,0,0,mpr_colums,mpr_rows,mpr_data,mpr_colums,mpr_rows,
			GlbPixelTypeToGDAL(mpr_pixelType),mpr_nBands,NULL,nPixelSpace,nLineSpace,nBandSpace);
	}
	//保存mask值
	//创建一个mask波段
	if(GDALCreateMaskBand(GDALGetRasterBand(pOutDS,1), GMF_PER_DATASET)!= CE_None)
	{
		CGlbString err = CPLGetLastErrorMsg();
		GlbSetLastError(err.ToWString().c_str());
		GDALClose(pOutDS);
		DeleteFile(fileName);
		return false;
	}
	//写入mask值
	GDALRasterBandH maskband = GDALGetMaskBand(GDALGetRasterBand(pOutDS,1));
	GDALRasterIO(maskband, GF_Write, 0,0,mpr_colums,mpr_rows,
		mpr_maskdata, mpr_colums,mpr_rows, GDT_Byte,0, 0);
	GDALFlushRasterCache(maskband);

	GDALFlushCache(pOutDS);
	GDALClose(pOutDS); 
	return true;
}

glbBool CGlbPixelBlock::ConvertToRGBA()
{
	if (mpr_pixelType != GLB_PIXELTYPE_BYTE ||
		mpr_pixelLayout != GLB_PIXELLAYOUT_BIP)
	{
		GlbSetLastError(L"像素格式不正确");
		return false;
	}
	if (!IsValid())
	{
		GlbSetLastError(L"没有数据");
		return false;
	}
	if (mpr_nBands == 4)
	{
		return true;
	}
	else if (mpr_nBands == 3)
	{
		mpr_nBands = 4;
		glbInt32 count = getPixelSize(mpr_pixelType)*mpr_nBands*mpr_rows*mpr_colums;
		glbByte* retdata = (glbByte*)malloc(count);
		if (retdata == NULL)
		{
			GlbSetLastError(L"申请内存失败");
			return false;
		}
		for (glbInt32 i=0;i<mpr_rows;i++)
		{
			for (glbInt32 j=0;j<mpr_colums;j++)
			{
				memcpy(retdata+(i*mpr_colums+j)*4, mpr_data+(i*mpr_colums+j)*3, 3);
				if (*(mpr_maskdata+(i*mpr_colums+j)) == 0)
				{
					*(retdata+(i*mpr_colums+j)*4+3) = 0;
				}
				else
				{
					*(retdata+(i*mpr_colums+j)*4+3) = 255;
				}
			}
		}
		free(mpr_data);
		mpr_data = retdata;
		return true;
	}
	else
	{
		GlbSetLastError(L"波段数只能是3或4");
		return false;
	}
}

glbBool CGlbPixelBlock::Mask(glbInt32 iUseBand,void*maskVal)
{
	if (!IsValid())
	{
		return false;
	}
	if (iUseBand<0 &&iUseBand>=mpr_nBands)
	{
		return false;
	}
	for (glbInt32 j =0;j<mpr_rows;j++)
	{
		for (glbInt32 i =0;i<mpr_colums;i++)
		{
			glbByte *pValid=NULL;
			GetMaskVal(i,j,&pValid);
			switch(mpr_pixelType)
			{
			case GLB_PIXELTYPE_BYTE:
				{
					glbByte *val;
					GetPixelVal(iUseBand,i,j,(void**)&val);
					glbByte*pMaskVal = (glbByte*)maskVal;
					if (*val == *pMaskVal)
					{
						*pValid  = 0;
					}
					else
					{
						*pValid  = 255;
					}
				}
				break;
			case	GLB_PIXELTYPE_USHORT16:
				{
					glbUInt16 *val;
					GetPixelVal(iUseBand,i,j,(void**)&val);
					glbUInt16*pMaskVal = (glbUInt16*)maskVal;
					if (*val == *pMaskVal)
					{
						*pValid  = 0;
					}
					else
					{
						*pValid  = 255;
					}
				}
				break;
			case	GLB_PIXELTYPE_SHORT16:
				{
					glbInt16 *val;
					GetPixelVal(iUseBand,i,j,(void**)&val);
					glbInt16*pMaskVal = (glbInt16*)maskVal;
					if (*val == *pMaskVal)
					{
						*pValid  = 0;
					}
					else
					{
						*pValid  = 255;
					}
				}
				break;
			case	GLB_PIXELTYPE_UINT32:
				{
					glbUInt32 *val;
					GetPixelVal(iUseBand,i,j,(void**)&val);
					glbUInt32*pMaskVal = (glbUInt32*)maskVal;
					if (*val == *pMaskVal)
					{
						*pValid  = 0;
					}
					else
					{
						*pValid  = 255;
					}
				}
				break;
			case	GLB_PIXELTYPE_INT32:
				{
					glbInt32 *val;
					GetPixelVal(iUseBand,i,j,(void**)&val);
					glbInt32* pMaskVal = (glbInt32*)maskVal;
					if (*val == *pMaskVal)
					{
						*pValid  = 0;
					}
					else
					{
						*pValid  = 255;
					}
				}
				break;
			case	GLB_PIXELTYPE_FLOAT32:
				{
					glbFloat *val;
					GetPixelVal(iUseBand,i,j,(void**)&val);
					glbFloat*pMaskVal = (glbFloat*)maskVal;
					if (*val == *pMaskVal)
					{
						*pValid  = 0;
					}
					else
					{
						*pValid  = 255;
					}
				}
				break;
			case	GLB_PIXELTYPE_DOUBLE64:
				{
					glbDouble *val;
					GetPixelVal(iUseBand,i,j,(void**)&val);
					glbDouble*pMaskVal = (glbDouble*)maskVal;
					if (*val == *pMaskVal)
					{
						*pValid  = 0;
					}
					else
					{
						*pValid  = 255;
					}
				}
				break;
			default:
				return false;
			}
		}
	}
	return true;
}

glbBool CGlbPixelBlock::ReplaceNoData(glbDouble NoData,glbDouble rpValue)
{
	if(!IsValid())
		return false;

	for (glbInt32 j =0; j<mpr_rows; j++)
	{
		for(glbInt32 i=0; i<mpr_colums; i++)
		{
			glbByte *pValid=NULL;
			GetMaskVal(i,j,&pValid);
			if ((*pValid)==0)
			{
				for (glbInt32 iBand = 0; iBand<mpr_nBands; iBand++)
				{
					switch(mpr_pixelType)
					{
					case GLB_PIXELTYPE_BYTE:
						{
							glbByte *val;
							GetPixelVal(iBand,i,j,(void**)&val);
							if(*val == NoData)
							{
								*val = (glbByte)rpValue;
								*pValid = 255;
							}
						}
						break;
					case	GLB_PIXELTYPE_USHORT16:
						{
							glbUInt16 *val;
							GetPixelVal(iBand,i,j,(void**)&val);
							if(*val == NoData)
							{
								*val = (glbUInt16)rpValue;
								*pValid = 255;
							}
						}
						break;
					case	GLB_PIXELTYPE_SHORT16:
						{
							glbInt16 *val;
							GetPixelVal(iBand,i,j,(void**)&val);
							if(*val == NoData)
							{
								*val = (glbInt16)rpValue;
								*pValid = 255;
							}
						}
						break;
					case	GLB_PIXELTYPE_UINT32:
						{
							glbUInt32 *val;
							GetPixelVal(iBand,i,j,(void**)&val);
							if(*val == NoData)
							{
								*val = (glbUInt32)rpValue;
								*pValid = 255;
							}
						}
						break;
					case	GLB_PIXELTYPE_INT32:
						{
							glbInt32 *val;
							GetPixelVal(iBand,i,j,(void**)&val);
							if(*val == NoData)
							{
								*val = (glbInt32)rpValue;
								*pValid = 255;
							}
						}
						break;
					case	GLB_PIXELTYPE_FLOAT32:
						{
							glbFloat *val;
							GetPixelVal(iBand,i,j,(void**)&val);
							if(*val == NoData)
							{
								*val = (glbFloat)rpValue;
								*pValid = 255;
							}
							else if(fabs(*val) > NullValue)
							{
								*val = (glbFloat)rpValue;
								*pValid = 255;
							}
						}
						break;
					case	GLB_PIXELTYPE_DOUBLE64:
						{
							glbDouble *val;
							GetPixelVal(iBand,i,j,(void**)&val);
							if(*val == NoData)
							{
								*val = (glbDouble)rpValue;
								*pValid = 255;
							}
							else if(fabs(*val) > NullValue)
							{
								*val = (glbDouble)rpValue;
								*pValid = 255;
							}
						}
						break;
					default:
						return false;
					}
				}
			}
		}
	}
	return true;
}
//计算像素和
static glbBool AddPixelVal(glbDouble& dval, glbInt32 x, glbInt32 y, glbInt32 iband, CGlbPixelBlock* tempB)
{
	switch(tempB->GetPixelType())
	{
	case GLB_PIXELTYPE_BYTE:
		{
			glbByte* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			dval += *Data;
		}
		break;
	case GLB_PIXELTYPE_USHORT16:
		{
			glbUInt16* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			dval += *Data;
		}
		break;
	case GLB_PIXELTYPE_SHORT16:
		{
			glbInt16* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			dval += *Data;
		}
		break;
	case GLB_PIXELTYPE_UINT32:
		{
			glbUInt32* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			dval += *Data;
		}
		break;
	case GLB_PIXELTYPE_INT32:
		{
			glbInt32* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			dval += *Data;
		}
		break;
	case GLB_PIXELTYPE_FLOAT32:
		{
			glbFloat* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			dval += *Data;
		}
		break;
	case GLB_PIXELTYPE_DOUBLE64:
		{
			glbDouble* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			dval += *Data;
		}
		break;
	default:
		return false;
	}
	return true;
}
//修改像素值
static glbBool ModifyPixelVal(glbDouble dval, glbInt32 x, glbInt32 y, glbInt32 iband, CGlbPixelBlock* tempB)
{
	switch(tempB->GetPixelType())
	{
	case GLB_PIXELTYPE_BYTE:
		{
			glbByte* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			*Data = dval;
		}
		break;
	case GLB_PIXELTYPE_USHORT16:
		{
			glbUInt16* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			*Data = dval;
		}
		break;
	case GLB_PIXELTYPE_SHORT16:
		{
			glbInt16* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			*Data = dval;
		}
		break;
	case GLB_PIXELTYPE_UINT32:
		{
			glbUInt32* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			*Data = dval;
		}
		break;
	case GLB_PIXELTYPE_INT32:
		{
			glbInt32* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			*Data = dval;
		}
		break;
	case GLB_PIXELTYPE_FLOAT32:
		{
			glbFloat* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			*Data = dval;
		}
		break;
	case GLB_PIXELTYPE_DOUBLE64:
		{
			glbDouble* Data;
			tempB->GetPixelVal(iband,x,y,(void**)&Data);
			*Data = dval;
		}
		break;
	default:
		return false;
	}
	return true;
}

static void LeftBlend(CGlbPixelBlock* src,glbInt32 sStartX,glbInt32 sStartY, 
	CGlbPixelBlock* dest,glbInt32 dStartX,glbInt32 dStartY,glbInt32 cx,glbInt32 cy)
{
	glbInt32 rows,cols;
	dest->GetSize(cols, rows);
	if (sStartY==-1&&dStartY==-1)
	{
		sStartY = 0;
		dStartY = 0;
		cy = rows;
	}
	if (sStartX==-1&&dStartX==-1)
	{
		sStartX = 0;
		dStartX = 0;
		cx = cols;
	}
	//src
	for (glbInt32 iy=sStartY;iy<sStartY+cy;iy++)
	{
		for (glbInt32 ix=sStartX;ix<sStartX+cx;ix++)
		{
			for (glbInt32 iBand=0;iBand<dest->GetBandCount();iBand++)
			{
				glbByte* Mask=NULL;
				glbDouble dval = 0;
				glbInt32 icount = 0;
				for (glbInt32 i=0;i<2*cx+1;i++)
				{
					if (ix+i-cx>=cols)
					{
						dest->GetMaskVal(ix+i-cx-cols,iy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix+i-cx-cols, iy, iBand, dest);
						}
					}
					else
					{
						src->GetMaskVal(ix+i-cx,iy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix+i-cx, iy, iBand, src);
						}
					}
				}
				//存在有效值
				if (icount != 0)
				{
					dval/=icount;
					ModifyPixelVal(dval, ix, iy, iBand, src);
				}
			}
		}
	}
	//this
	for (glbInt32 iy=dStartY;iy<dStartY+cy;iy++)
	{
		for (glbInt32 ix=dStartX;ix<dStartX+cx;ix++)
		{
			for (glbInt32 iBand=0;iBand<dest->GetBandCount();iBand++)
			{
				glbByte* Mask=NULL;
				glbDouble dval = 0;
				glbInt32 icount = 0;
				for (glbInt32 i=0;i<2*cx+1;i++)
				{
					if (ix+i-cx>=0)
					{
						dest->GetMaskVal(ix+i-cx,iy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix+i-cx, iy, iBand, dest);
						}
					}
					else
					{
						src->GetMaskVal(ix+i-cx+cols,iy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix+i-cx+cols, iy, iBand, src);
						}
					}
				}
				//存在有效值
				if (icount != 0)
				{
					dval/=icount;
					ModifyPixelVal(dval, ix, iy, iBand, dest);
				}
			}
		}
	}
}

static void RightBlend(CGlbPixelBlock* src,glbInt32 sStartX,glbInt32 sStartY, 
	CGlbPixelBlock* dest,glbInt32 dStartX,glbInt32 dStartY,glbInt32 cx,glbInt32 cy)
{
	glbInt32 rows,cols;
	dest->GetSize(cols, rows);
	if (sStartY==-1&&dStartY==-1)
	{
		sStartY = 0;
		dStartY = 0;
		cy = rows;
	}
	if (sStartX==-1&&dStartX==-1)
	{
		sStartX = 0;
		dStartX = 0;
		cx = cols;
	}
	//this
	for (glbInt32 iy=sStartY;iy<sStartY+cy;iy++)
	{
		for (glbInt32 ix=sStartX;ix<sStartX+cx;ix++)
		{
			for (glbInt32 iBand=0;iBand<dest->GetBandCount();iBand++)
			{
				glbByte* Mask=NULL;
				glbDouble dval = 0;
				glbInt32 icount = 0;
				for (glbInt32 i=0;i<2*cx+1;i++)
				{
					if (ix+i-cx>=cols)
					{
						src->GetMaskVal(ix+i-cx-cols,iy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix+i-cx-cols, iy, iBand, src);
						}
					}
					else
					{
						dest->GetMaskVal(ix+i-cx,iy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix+i-cx, iy, iBand, dest);
						}
					}
				}
				//存在有效值
				if (icount != 0)
				{
					dval/=icount;
					ModifyPixelVal(dval, ix, iy, iBand, dest);
				}
			}
		}
	}
	//src
	for (glbInt32 iy=dStartY;iy<dStartY+cy;iy++)
	{
		for (glbInt32 ix=dStartX;ix<dStartX+cx;ix++)
		{
			for (glbInt32 iBand=0;iBand<dest->GetBandCount();iBand++)
			{
				glbByte* Mask=NULL;
				glbDouble dval = 0;
				glbInt32 icount = 0;
				for (glbInt32 i=0;i<2*cx+1;i++)
				{
					if (ix+i-cx>=0)
					{
						src->GetMaskVal(ix+i-cx,iy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix+i-cx, iy, iBand, src);
						}
					}
					else
					{
						dest->GetMaskVal(ix+i-cx+cols,iy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix+i-cx+cols, iy, iBand, dest);
						}
					}
				}
				//存在有效值
				if (icount != 0)
				{
					dval/=icount;
					ModifyPixelVal(dval, ix, iy, iBand, src);
				}
			}
		}
	}
}

static void UpBlend(CGlbPixelBlock* src,glbInt32 sStartX,glbInt32 sStartY, 
	CGlbPixelBlock* dest,glbInt32 dStartX,glbInt32 dStartY,glbInt32 cx,glbInt32 cy)
{
	glbInt32 rows,cols;
	dest->GetSize(cols, rows);
	if (sStartY==-1&&dStartY==-1)
	{
		sStartY = 0;
		dStartY = 0;
		cy = rows;
	}
	if (sStartX==-1&&dStartX==-1)
	{
		sStartX = 0;
		dStartX = 0;
		cx = cols;
	}
	//src
	for (glbInt32 iy=sStartY;iy<sStartY+cy;iy++)
	{
		for (glbInt32 ix=sStartX;ix<sStartX+cx;ix++)
		{
			for (glbInt32 iBand=0;iBand<dest->GetBandCount();iBand++)
			{
				glbByte* Mask=NULL;
				glbDouble dval = 0;
				glbInt32 icount = 0;
				for (glbInt32 i=0;i<2*cy+1;i++)
				{
					if (iy+i-cy>=rows)
					{
						dest->GetMaskVal(ix,iy+i-cy-rows,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix, iy+i-cy-rows, iBand, dest);
						}
					}
					else
					{
						src->GetMaskVal(ix,iy+i-cy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix, iy+i-cy, iBand, src);
						}
					}
				}
				//存在有效值
				if (icount != 0)
				{
					dval/=icount;
					ModifyPixelVal(dval, ix, iy, iBand, src);
				}
			}
		}
	}
	//this
	for (glbInt32 iy=dStartY;iy<dStartY+cy;iy++)
	{
		for (glbInt32 ix=dStartX;ix<dStartX+cx;ix++)
		{
			for (glbInt32 iBand=0;iBand<dest->GetBandCount();iBand++)
			{
				glbByte* Mask=NULL;
				glbDouble dval = 0;
				glbInt32 icount = 0;
				for (glbInt32 i=0;i<2*cy+1;i++)
				{
					if (iy+i-cy>=0)
					{
						dest->GetMaskVal(ix,iy+i-cy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix, iy+i-cy, iBand, dest);
						}
					}
					else
					{
						src->GetMaskVal(ix,iy+i-cy+rows,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix, iy+i-cy+rows, iBand, src);
						}
					}
				}
				//存在有效值
				if (icount != 0)
				{
					dval/=icount;
					ModifyPixelVal(dval, ix, iy, iBand, dest);
				}
			}
		}
	}
}

static void DownBlend(CGlbPixelBlock* src,glbInt32 sStartX,glbInt32 sStartY, 
	CGlbPixelBlock* dest,glbInt32 dStartX,glbInt32 dStartY,glbInt32 cx,glbInt32 cy)
{
	glbInt32 rows,cols;
	dest->GetSize(cols, rows);
	if (sStartY==-1&&dStartY==-1)
	{
		sStartY = 0;
		dStartY = 0;
		cy = rows;
	}
	if (sStartX==-1&&dStartX==-1)
	{
		sStartX = 0;
		dStartX = 0;
		cx = cols;
	}
	//this
	for (glbInt32 iy=sStartY;iy<sStartY+cy;iy++)
	{
		for (glbInt32 ix=sStartX;ix<sStartX+cx;ix++)
		{
			for (glbInt32 iBand=0;iBand<dest->GetBandCount();iBand++)
			{
				glbByte* Mask=NULL;
				glbDouble dval = 0;
				glbInt32 icount = 0;
				for (glbInt32 i=0;i<2*cy+1;i++)
				{
					if (iy+i-cy>=rows)
					{
						src->GetMaskVal(ix,iy+i-cy-rows,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix, iy+i-cy-rows, iBand, src);
						}
					}
					else
					{
						dest->GetMaskVal(ix,iy+i-cy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix, iy+i-cy, iBand, dest);
						}
					}
				}
				//存在有效值
				if (icount != 0)
				{
					dval/=icount;
					ModifyPixelVal(dval, ix, iy, iBand, dest);
				}
			}
		}
	}
	//src
	for (glbInt32 iy=dStartY;iy<dStartY+cy;iy++)
	{
		for (glbInt32 ix=dStartX;ix<dStartX+cx;ix++)
		{
			for (glbInt32 iBand=0;iBand<dest->GetBandCount();iBand++)
			{
				glbByte* Mask=NULL;
				glbDouble dval = 0;
				glbInt32 icount = 0;
				for (glbInt32 i=0;i<2*cy+1;i++)
				{
					if (iy+i-cy>=0)
					{
						src->GetMaskVal(ix,iy+i-cy,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix, iy+i-cy, iBand, src);
						}
					}
					else
					{
						dest->GetMaskVal(ix,iy+i-cy+rows,&Mask);
						if (*Mask != 0)//不是无效值
						{
							icount++;
							AddPixelVal(dval, ix, iy+i-cy+rows, iBand, dest);
						}
					}
				}
				//存在有效值
				if (icount != 0)
				{
					dval/=icount;
					ModifyPixelVal(dval, ix, iy, iBand, src);
				}
			}
		}
	}
}

glbBool CGlbPixelBlock::Blend(CGlbPixelBlock* src,glbInt32 sStartX,glbInt32 sStartY, 
	glbInt32 dStartX,glbInt32 dStartY,glbInt32 cx,glbInt32 cy)
{
	if (src == NULL)
	{
		GlbSetLastError(L"传入参数为NULL");
		return false;
	}
	
	glbInt32 sizeX,sizeY;
	src->GetSize(sizeX,sizeY);
	if (src->GetBandCount()!=mpr_nBands ||
		src->GetPixelType()!=mpr_pixelType ||
		src->GetPixelLayout() != mpr_pixelLayout ||
		sizeX != mpr_colums ||
		sizeY != mpr_rows)
	{
		GlbSetLastError(L"要混合的两个像素块格式不一致");
		return false;
	}
	if (cx<0 || cx>mpr_colums ||
		cy<0 || cx>mpr_rows)
	{
		GlbSetLastError(L"要混合的像素宽度不正确");
		return false;
	}
	if (sStartX>mpr_colums-cx ||
		sStartY>mpr_rows-cy)
	{
		GlbSetLastError(L"要混合的源像素块起始位置不正确");
		return false;
	}
	if (dStartX>=mpr_colums-cx ||
		dStartY>=mpr_rows-cy)
	{
		GlbSetLastError(L"要混合的目的像素块起始位置不正确");
		return false;
	}
	//左块混合
	if (dStartX==0 && dStartY==-1&&
		sStartX==mpr_colums-cx&&sStartY==-1)
	{
		LeftBlend(src,sStartX,sStartY,this,dStartX,dStartY,cx,cy);
	}
	//右块混合
	else if (dStartX==mpr_colums-cx && dStartY==-1&&
		sStartX==0&&sStartY==-1)
	{
		RightBlend(src,sStartX,sStartY,this,dStartX,dStartY,cx,cy);
	}
	//上块混合
	else if (dStartX==-1 && dStartY==0&&
		sStartX==-1&&sStartY==mpr_rows-cy)
	{
		UpBlend(src,sStartX,sStartY,this,dStartX,dStartY,cx,cy);
	}
	//下块混合
	else if (dStartX==-1 && dStartY==mpr_rows-cy&&
		sStartX==-1&&sStartY==0)
	{
		DownBlend(src,sStartX,sStartY,this,dStartX,dStartY,cx,cy);
	}
	else
	{
		GlbSetLastError(L"混合块不符合要求");
		return false;
	}
	return true;
}


