#pragma warning(disable:4251)
#include "StdAfx.h"
#include "GlbFileRasterDataset.h"
#include "GlbExtent.h"
#include "GlbPath.h"
#include "GlbConvert.h"
#include "GlbError.h"
#include "GlbConvert.h"
#include <math.h>
#include "GlbString.h"
#include "GlbFileDataSource.h"
#include "GlbSQLitePyramid.h"

#define GLBSQL_COUNT 100

CGlbFileRasterDataset::CGlbFileRasterDataset(void)
{
	mpr_ds          = NULL;
	mpr_url         = L"";
	mpr_name        = L"";
	mpr_alias       = L"";
	mpr_srs         = L"";
	mpr_extent      = NULL;
	mpr_format      = L"";
	mpr_pixeltype   = GLB_PIXELTYPE_UNKNOWN;
	mpr_pixellayout = GLB_PIXELLAYOUT_UNKNOWN;
	mpr_gdalDataset = NULL;
	mpr_pyramid     = NULL;
	mpr_dealExtent = NULL;
	mpr_isMove = false;
	mpr_isChange = false;
	mpr_changeValue = DBL_MAX;
	mpr_isMaxLevel = false;
}

CGlbFileRasterDataset::~CGlbFileRasterDataset(void)
{
	mpr_ds     = NULL;
	mpr_extent = NULL;
	if (mpr_gdalDataset != NULL)
		GDALClose(mpr_gdalDataset);
	mpr_pyramid = NULL;	
}

glbBool CGlbFileRasterDataset::Initialize
	( CGlbFileDataSource* ds, 
	const glbWChar* url,
	const glbWChar* name, 
	const glbWChar* alias, 
	GDALDatasetH    dst, 
	CGlbSQLitePyramid* fp )
{
	if (ds == NULL
		||url  == NULL
		||name == NULL
		||(dst == NULL && fp == NULL))
	{
		GlbSetLastError(L"有无效参数");
		return false;
	}
	mpr_url   = url;
	mpr_ds    = ds;
	mpr_name  = name;
	mpr_alias = (alias==NULL)?L"":alias;

	mpr_gdalDataset = dst;
	mpr_pyramid     = fp;
	if(mpr_gdalDataset == NULL)
	{
		mpr_pixeltype  = mpr_pyramid->mpr_pixeltype;
		mpr_pixellayout= mpr_pyramid->mpr_pixellayout;
		return true;
	}
	//像素布局模式
	const glbChar* tempstr = GDALGetMetadataItem(mpr_gdalDataset, "INTERLEAVE", "IMAGE_STRUCTURE");
	if (tempstr != NULL)
	{
		CGlbString pixelLayout = tempstr;
		pixelLayout.ToUpper();
		if (pixelLayout == "BAND")
		{
			mpr_pixellayout = GLB_PIXELLAYOUT_BSQ;
		}
		else if (pixelLayout == "PIXEL")
		{
			mpr_pixellayout = GLB_PIXELLAYOUT_BIP;
		}
		else if (pixelLayout == "LINE")
		{
			mpr_pixellayout = GLB_PIXELLAYOUT_BIL;
		}
		else
		{
			GlbSetLastError(L"未知像素布局模式");
			return false;
		}
	}else{
		mpr_format = GetFormat();
		if (mpr_format == L"BMP")		
			mpr_pixellayout = GLB_PIXELLAYOUT_BIP;
		else
		{
			GlbSetLastError(L"未知像素布局模式");
			return false;				
		}
	}
	//没有波段，返回
	glbInt32 nBands = GDALGetRasterCount(mpr_gdalDataset);
	if (nBands == 0)
	{
		GlbSetLastError(L"没有波段");
		return false;
	}
	//像素类型
	mpr_pixeltype = GlbGDALToPixelType(GDALGetRasterDataType(GDALGetRasterBand(mpr_gdalDataset, 1)));
	if(mpr_pixeltype == GLB_PIXELTYPE_UNKNOWN)
	{
		GlbSetLastError(L"像素类型不支持");
		return false;
	}
	for(glbInt32 i=2; i<= nBands; i++)
	{
		if (mpr_pixeltype != GlbGDALToPixelType(GDALGetRasterDataType(GDALGetRasterBand(mpr_gdalDataset, i))))
		{
			GlbSetLastError(L"波段像素类型不一致");
			return false;
		}
	}
	GetSRS();
	return true;
}
glbBool CGlbFileRasterDataset::QueryInterface( const glbWChar *riid, void **ppvoid )
{
	if (riid == NULL || ppvoid == NULL)
	{
		GlbSetLastError(L"参数不正确");
		return false;
	}
	CGlbWString tmpiid = riid;
	if(tmpiid == L"IGlbRasterDataset")
	{
		*ppvoid = (IGlbRasterDataset*)this;
		return true;
	}
	else if(tmpiid == L"IGlbGeoDataset")
	{
		*ppvoid = (IGlbGeoDataset*)this;
		return true;
	}
	else if(tmpiid == L"IGlbDataset")
	{
		*ppvoid = (IGlbDataset*)this;
		return true;
	}
	GlbSetLastError(L"查询的接口不存在");
	return false;
}
GlbDatasetTypeEnum CGlbFileRasterDataset::GetType()
{
	return GLB_DATASET_RASTER;
}
const glbWChar* CGlbFileRasterDataset::GetName()
{
	return mpr_name.c_str();
}
const glbWChar* CGlbFileRasterDataset::GetAlias()
{
	return mpr_alias.c_str();
}
const IGlbDataSource* CGlbFileRasterDataset::GetDataSource()
{
	if (mpr_ds == NULL)
	{
		return NULL;
	}
	return (IGlbDataSource*)mpr_ds.get();
}
const IGlbDataset* CGlbFileRasterDataset::GetParentDataset()
{
	return NULL;
}
const GlbSubDatasetRoleEnum CGlbFileRasterDataset::GetSubRole()
{
	return GLB_SUBDATASETROLE_UNKNOWN;
}
glbWChar* CGlbFileRasterDataset::GetLastError()
{
	return GlbGetLastError();
}
const glbWChar* CGlbFileRasterDataset::GetSRS()
{
	//有栅格数据集
	if(mpr_gdalDataset != NULL)
	{
		if(mpr_srs.length()>0)
		{
			return mpr_srs.c_str();
		}
		mpr_srs = L"";
		const glbChar* srs = GDALGetProjectionRef(mpr_gdalDataset);
		if(srs != NULL && srs[0]!='\0')
		{
			CGlbString strsrs = srs;
			mpr_srs           = strsrs.ToWString();
		}
		if(mpr_srs.length()==0)
		{
			//map,zoi
			CGlbWString mapfile = CGlbPath::GetBaseName(mpr_name)+L".map";
			mapfile             = mpr_url + mapfile;
			FILE *fp            = NULL;
			fopen_s(&fp,mapfile.ToString().c_str(), "rb");
			if(!fp)return NULL;

			CGlbString szDatum,szProj,szProjParms;
			glbDouble  left,right,top,bottom;
			glbChar    buf[4096];
			int iLine = 0;

			while (fgets(buf, 4096, fp))//读一行
			{
				iLine++;
				if (iLine>53)
				{
					break;
				}
				if (iLine ==5)//第5行
				{
					szDatum = buf;
					szDatum.TrimRight();
				}
				if (iLine ==9)
				{
					if (!strncmp(buf, "Map Projection", 14))//第9行
					{
						szProj = buf;
						szProj.TrimRight();
					}
				}
				if (iLine ==40)
				{
					if (!strncmp(buf,"Projection Setup",16))//第40行
					{
						szProjParms = buf;
						szProjParms.TrimRight();
					}
				}
				if (iLine ==50)
				{
					if (!strncmp(buf,"MMPLL,1,",8))//lefttop 第50行
					{
						sscanf_s(buf,"MMPLL,1, %lf, %lf",&left,&top);
					}
				}
				if (iLine==51)
				{
					if (!strncmp(buf,"MMPLL,2,",8))//righttop 第51行
					{
						sscanf_s(buf,"MMPLL,2, %lf, %lf",&right,&top);
					}
				}
				if (iLine==52)
				{
					if (!strncmp(buf,"MMPLL,3,",8))//rightbottom 第52行
					{
						sscanf_s(buf,"MMPLL,3, %lf, %lf",&right,&bottom);
					}
				}
				if (iLine==53)
				{
					if (!strncmp(buf,"MMPLL,4,",8))//leftbottom 第53行
					{
						sscanf_s(buf,"MMPLL,4, %lf, %lf",&left,&bottom);
					}
				}
			}

			fclose(fp);        
			CGlbSpatialReference* spatialReference = 
				CGlbSpatialReference::CreateFromOzi(
				szDatum.ToWString().c_str(),
				szProj.ToWString().c_str(),
				szProjParms.ToWString().c_str());
			if (spatialReference == NULL) return NULL;
			mpr_srs = spatialReference->ExportToWKT();
			delete spatialReference;
			if(mpr_srs.length()==0)return NULL;
			//初始化extent
			if(mpr_extent == NULL) 
				mpr_extent = new CGlbExtent();
			mpr_extent->Set(left,right,bottom,top);					
		}
		return mpr_srs.c_str();
	}
	else if (mpr_pyramid != NULL)//金字塔
	{
		if(mpr_pyramid->mpr_srs.length()==0)return NULL;
		return mpr_pyramid->mpr_srs.c_str();
	}
	return NULL;
}
glbBool CGlbFileRasterDataset::AlterSRS( const glbWChar* srs )
{
	if (srs == NULL)
	{
		GlbSetLastError(L"参数是NULL");
		return false;
	}
	CGlbSpatialReference* glbSRS = CGlbSpatialReference::CreateFromWKT(srs);
	if(glbSRS == NULL)
		glbSRS = CGlbSpatialReference::CreateFromProj4(srs);
	else if(glbSRS == NULL)
		glbSRS = CGlbSpatialReference::CreateFromEsriFile(srs);
	else if(glbSRS == NULL)
		glbSRS = CGlbSpatialReference::CreateFromWellKnown(srs);
	else if(glbSRS == NULL){
		glbInt32 espn = 0;
		CGlbConvert::ToInt32(srs,espn);
		glbSRS = CGlbSpatialReference::CreateFromEPSG(espn);
	}
	if(glbSRS == NULL)
	{
		GlbSetLastError(L"不识别的坐标系");
		return false;
	}
	//有栅格数据集
	if (mpr_gdalDataset != NULL)
	{
		glbWChar*   srsWStr = glbSRS->ExportToWKT();
		CGlbWString glbWSRS = srsWStr;
		free(srsWStr);srsWStr=NULL;
		delete glbSRS;

		if(GDALSetProjection(mpr_gdalDataset, glbWSRS.ToString().c_str())!= CE_None)
		{
			GlbSetLastError(L"修改坐标信息失败");
			return false;
		}
		GDALFlushCache(mpr_gdalDataset);
		mpr_srs = glbWSRS;
		return true;
	}
	else//有金字塔
	{
		GlbSetLastError(L"只有金字塔不修改坐标系");
		return false;
	}
}
const CGlbExtent* CGlbFileRasterDataset::GetExtent()
{
	//已经获取过外包，直接返回
	if(mpr_extent != NULL) 
		return mpr_extent.get();
	//有栅格数据集
	if(mpr_gdalDataset != NULL)
	{
		double dGeoTrans[6];
		glbInt32 rows    = GDALGetRasterYSize(mpr_gdalDataset);
		glbInt32 columns = GDALGetRasterXSize(mpr_gdalDataset);
		if(GDALGetGeoTransform(mpr_gdalDataset, dGeoTrans) != CE_None)
		{
			mpr_extent = new CGlbExtent(0, columns, 0, rows);
		}else{		
			double orgX = dGeoTrans[0] ;//屏幕左上横向坐标
			double orgY = dGeoTrans[3] ; //屏幕左上纵向坐标
			double rsX = dGeoTrans[1] ;// 横向分辨率
			double rsY = dGeoTrans[5] ;// 纵向分辨率 topleft is origin，所以应该取反
			if (rsY < 0)rsY *= -1;
			mpr_extent = new CGlbExtent(orgX, orgX + rsX*columns,
				orgY - rsY*rows, orgY);
		}
		return mpr_extent.get();
	}
	else if(mpr_pyramid)
	{
		glbInt32 minL,maxL;
		GetPyramidLevels(&minL,&maxL);
		mpr_extent = new CGlbExtent();
		GetLevelExtent(maxL,mpr_extent.get());
		return mpr_extent.get();
	}
	else
	{
		GlbSetLastError(L"没有初始化");
		return NULL;
	}
}
glbBool CGlbFileRasterDataset::HasZ()
{
	return false;
}
glbBool CGlbFileRasterDataset::HasM()
{
	return false;
}
glbBool CGlbFileRasterDataset::GetOrigin( glbDouble *originX, glbDouble *originY )
{
	if (originX == NULL || originY == NULL)
	{
		GlbSetLastError(L"有无效参数");
		return false;
	}
	if (mpr_extent == NULL)
	{
		if(GetExtent() == NULL)
			return false;
	}
	mpr_extent->Get(originX, NULL, NULL, originY);
	return true;
}

glbBool CGlbFileRasterDataset::GetSize( glbInt32 *columns, glbInt32 *rows )
{
	if (columns==NULL || rows==NULL)
	{
		GlbSetLastError(L"有无效参数");
		return false;
	}
	//有栅格数据集
	if (mpr_gdalDataset != NULL)
	{
		*rows    = GDALGetRasterYSize(mpr_gdalDataset);
		*columns = GDALGetRasterXSize(mpr_gdalDataset);
		return true;
	}
	else//金字塔
	{
		GlbSetLastError(L"只有金字塔");
		return false;
	}
}

glbBool CGlbFileRasterDataset::GetRS( glbDouble *resX, glbDouble *resY )
{
	if (resX==NULL || resY==NULL)
	{
		GlbSetLastError(L"有无效参数");
		return false;
	}
	//有栅格数据集
	if (mpr_gdalDataset != NULL)
	{
		double dGeoTrans[6];
		if(GDALGetGeoTransform(mpr_gdalDataset, dGeoTrans)==CE_None)
		{
			*resX = dGeoTrans[1] ;// 横向分辨率
			*resY = dGeoTrans[5] ;//纵向分辨率
			if (*resY < 0)*resY *= -1;
		}else{
			*resX = 1;
			*resY = 1;
		}
		return true;
	}
	else//金字塔
	{
		GlbSetLastError(L"只有金字塔数据，不能获取分辨率");
		return false;
	}
}

GlbPixelTypeEnum CGlbFileRasterDataset::GetPixelType()
{
	if (mpr_gdalDataset != NULL)
	{
		return mpr_pixeltype;
	}
	else if(mpr_pyramid != NULL)
	{
		return mpr_pyramid->mpr_pixeltype;
	}
	return GLB_PIXELTYPE_UNKNOWN;	
}

GlbPixelLayoutTypeEnum CGlbFileRasterDataset::GetPixelLayout()
{
	if (mpr_gdalDataset != NULL)
	{
		return mpr_pixellayout;
	}
	else if(mpr_pyramid != NULL)
	{
		return mpr_pyramid->mpr_pixellayout;
	}
	return GLB_PIXELLAYOUT_UNKNOWN;
}

const glbWChar* CGlbFileRasterDataset::GetCompressType()
{
	if (mpr_compress.length()>0)
	{
		return mpr_compress.c_str();
	}
	//有栅格数据集
	if (mpr_gdalDataset != NULL)
	{
		const glbChar* compress = GDALGetMetadataItem(mpr_gdalDataset, "COMPRESSION", "IMAGE_STRUCTURE");
		if (compress != NULL)
		{
			CGlbString tempstr = compress;
			mpr_compress = tempstr.ToWString();
		}
		return mpr_compress.c_str();
	}
	return NULL;	
}

const glbWChar* CGlbFileRasterDataset::GetFormat()
{
	if (mpr_format.length()>0)
	{
		return mpr_format.c_str();
	}
	//有栅格数据集
	if (mpr_gdalDataset != NULL)
	{
		const char* format = GDALGetDriverShortName(GDALGetDatasetDriver(mpr_gdalDataset));
		if (format != NULL)
		{
			CGlbString tempstr = format;
			mpr_format = tempstr.ToWString();
		}
		return mpr_format.c_str();
	}
	return NULL;
}

glbBool CGlbFileRasterDataset::GetNoData( glbDouble *noDataVal )
{
	if (noDataVal==NULL)
	{
		GlbSetLastError(L"无效参数");
		return false;
	}
	if(GetBandCount() <= 0)return false;
	
	glbInt32 isOK        = FALSE;
	glbDouble _noDataVal = GDALGetRasterNoDataValue(GDALGetRasterBand(mpr_gdalDataset, 1), &isOK);
	if(isOK == TRUE)
	{
		*noDataVal = _noDataVal;
		return true;
	}
	else
	{
		GlbSetLastError(L"没有无效值");
		return false;
	}
}

glbBool CGlbFileRasterDataset::AlterNoData( glbDouble noDataVal )
{
	if (mpr_gdalDataset == NULL)
	{
		GlbSetLastError(L"没有原始数据");
		return false;
	}
	glbInt32 count = GetBandCount();
	if(count <= 0)
	{
		GlbSetLastError(L"没有波段");
		return false;
	}
	CPLErr err = CE_None;
	for (glbInt32 i=1;i<=count;i++)
	{
		err = GDALSetRasterNoDataValue(GDALGetRasterBand(mpr_gdalDataset, i), noDataVal);
		if(err != CE_None)
			break;
	}
	if (err == CE_None)
	{
		//刷新
		GDALFlushCache(mpr_gdalDataset);
		return true; 
	}
	else
	{
		GlbSetLastError(L"设置失败");
		return false;
	}
}

glbInt32 CGlbFileRasterDataset::GetBandCount()
{
	if (mpr_gdalDataset != NULL)
	{
		return GDALGetRasterCount(mpr_gdalDataset);
	}
	else
	{
		if (mpr_pyramid != NULL)
		{
			return mpr_pyramid->mpr_bandCount;
		}		
	}
	return 0;
}

const IGlbRasterBand* CGlbFileRasterDataset::GetBand( glbInt32 bandIndex )
{
	if (mpr_gdalDataset != NULL)
	{
		glbInt32 bandcount = GDALGetRasterCount(mpr_gdalDataset);
		if (bandIndex<0 || bandIndex>=bandcount)
		{
			GlbSetLastError(L"无效参数[0,波段数-1]");
			return NULL;
		}
		GDALRasterBandH pGDALBand = GDALGetRasterBand(mpr_gdalDataset, 1+bandIndex);
		//创建Band
		CGlbFileRasterBand* band = new CGlbFileRasterBand(pGDALBand);
		return band;
	}
	else
	{
		GlbSetLastError(L"只有金字塔");
		return NULL; 
	}
}

glbBool CGlbFileRasterDataset::GetBlockSize( glbInt32* blockSizeX, glbInt32* blockSizeY )
{
	if (blockSizeX==NULL || blockSizeY==NULL)
	{
		GlbSetLastError(L"有无效参数");
		return false;
	}
	if (mpr_gdalDataset != NULL)
	{
		if(GetBandCount() <= 0)
		{
			GlbSetLastError(L"没有波段");
			return false;
		}
		GDALRasterBandH pGDALBand = GDALGetRasterBand(mpr_gdalDataset, 1);//获取第一个波段
		GDALGetBlockSize(pGDALBand, blockSizeX, blockSizeY);
		return true;
	}
	else
	{
		GlbSetLastError(L"只有金字塔");
		return false;
	}
}

CGlbPixelBlock* CGlbFileRasterDataset::CreatePixelBlock( glbInt32 columns, glbInt32 rows )
{
	if (mpr_gdalDataset == NULL)
	{
		GlbSetLastError(L"没有原始数据");
		return NULL;
	}

	CGlbPixelBlock* pblock = new CGlbPixelBlock(mpr_pixeltype, mpr_pixellayout);
	if(!pblock->Initialize(GetBandCount(), columns, rows))
	{
		delete pblock;
		return NULL;
	}
	return pblock;

}

glbBool CGlbFileRasterDataset::Read( glbInt32 startColumn, glbInt32 startRow, CGlbPixelBlock *pPixelBlock )
{
	if (pPixelBlock == NULL
		||startColumn <0
		||startRow    <0)
	{
		GlbSetLastError(L"有参数无效");
		return false;
	}
	if(pPixelBlock->IsValid() == false)
	{
		GlbSetLastError(L"像素块没初始化");
		return false;
	}
	if (mpr_gdalDataset == NULL)
	{
		GlbSetLastError(L"没有原始数据");
		return false;
	}
	if (pPixelBlock->GetPixelLayout() != mpr_pixellayout)
	{
		GlbSetLastError(L"布局不一致");
		return false;
	}
	if (pPixelBlock->GetBandCount() != GetBandCount())
	{
		GlbSetLastError(L"波段数目不一致");
		return false;
	}
	
	glbInt32 rows = 0;
	glbInt32 cols = 0;
	if(!GetSize(&cols, &rows))return false;
			
	if(startColumn>=cols || startRow>=rows)
	{
		GlbSetLastError(L"起始行或列超范围");
		return false;
	}

	glbInt32 xsize, ysize;	
	pPixelBlock->GetSize(xsize, ysize);
	glbInt32  rdxsize  = xsize;
	glbInt32  rdysize  = ysize;
	if(xsize + startColumn > cols)
		rdxsize = cols - startColumn;
	if(ysize + startRow > rows)
		rdysize = rows - startRow;

	GlbPixelTypeEnum rdPixelType = pPixelBlock->GetPixelType();
	glbInt32 nPixelSpace= 0;
	glbInt32 nLineSpace = 0;
	glbInt32 nBandSpace = 0;
	glbInt32 bands      = GetBandCount();
	
	GetRastIOLayout(mpr_pixellayout,rdPixelType,bands,xsize,ysize,
		            nPixelSpace,nLineSpace,nBandSpace);

	glbByte* pdata = NULL;
	glbByte* pmask = NULL;
	
	pPixelBlock->GetPixelData(&pdata);
	pPixelBlock->GetMaskData (&pmask);
	
	{//[互斥]读数据
		GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		//读数据
		if( GDALDatasetRasterIO
			(mpr_gdalDataset, 
			GF_Read, 
			startColumn, 
			startRow, 
			rdxsize, 
			rdysize,
			pdata, 
			xsize, 
			ysize, 
			GlbPixelTypeToGDAL(rdPixelType),
			bands, 
			NULL, 
			nPixelSpace, 
			nLineSpace, 
			nBandSpace) != CE_None)
		{
			CGlbString err = CPLGetLastErrorMsg();
			GlbSetLastError(err.ToWString().c_str());
			return false;
		}
			
		{
			//读掩码
			memset(pmask,255,xsize*ysize);	
			/*
			以第一个波段的掩码做为整个数据集的掩码.
			*/
			GDALRasterBandH maskband = GDALGetMaskBand(GDALGetRasterBand(mpr_gdalDataset,1));
			GDALRasterIO(maskband, 
				GF_Read, 
				startColumn, 
				startRow, 
				rdxsize, 
				rdysize,
				pmask, 
				xsize, 
				ysize, 
				GDT_Byte,0, 0);
		}		
	}//[互斥]读数据	
	return true;
}

glbBool CGlbFileRasterDataset::ReadEx
	   ( const CGlbExtent *destExtent, 
	     CGlbPixelBlock   *pPixelBlock,
		 GlbRasterResampleTypeEnum rspType/*,glbBool isRelapce*/)
{
	return _ReadEx(destExtent,pPixelBlock,rspType);
}

glbBool CGlbFileRasterDataset::_ReadEx(
	    const CGlbExtent *destExtent,
		CGlbPixelBlock   *pPixelBlock,
		GlbRasterResampleTypeEnum rspType,
		glbBool          isReplace/*=false*/,
		glbDouble        rpValue  /*=0.0*/,
		glbBool          isDem/*=false*/)
{
	if (destExtent == NULL || pPixelBlock == NULL)
	{
		GlbSetLastError(L"有无效参数");
		return false;
	}
	if(pPixelBlock->IsValid() == false)
	{
		GlbSetLastError(L"像素块没初始化");
		return false;
	}
	if (mpr_gdalDataset == NULL)
	{
		GlbSetLastError(L"没有原始数据");
		return false;
	}	
	if (pPixelBlock->GetPixelLayout() != mpr_pixellayout)
	{
		GlbSetLastError(L"布局不一致");
		return false;
	}
	if (pPixelBlock->GetBandCount() != GetBandCount())
	{
		GlbSetLastError(L"波段数目不一致");
		return false;
	}
	
	glbref_ptr<CGlbExtent> interExt = GetExtent()->Intersect(*destExtent);
	if (interExt==NULL)
	{
		GlbSetLastError(L"范围与数据集无交集");
		return false;
	}

	glbDouble resX,   resY;
	glbDouble outResX,    outResY;
	glbInt32  outCol,     outRow;
	glbInt32  outxsize,   outysize;
	glbDouble interMinX,  interMaxX,interMinY,interMaxY;
	glbDouble dsMinX,     dsMaxX,   dsMinY,   dsMaxY;
	glbInt32 readCol,    readxsize,readRow,  readysize;
	{
		GetRS(&resX, &resY);
		GetExtent()->Get(&dsMinX,   &dsMaxX,   &dsMinY,   &dsMaxY);
		interExt->Get(   &interMinX,&interMaxX,&interMinY,&interMaxY);

		glbInt32  outCols,outRows;
		glbDouble outMinX,outMaxX,outMinY,outMaxY;
		pPixelBlock->GetSize(outCols,outRows);
		destExtent->Get(&outMinX,&outMaxX,&outMinY,&outMaxY);

		outResX = destExtent->GetXWidth() /outCols;
		outResY = destExtent->GetYHeight()/outRows;

		outCol   =  (glbInt32)floor((interMinX - outMinX)  /outResX);
		outRow   =  (glbInt32)floor((outMaxY   - interMaxY)/outResY);
		outxsize =  (glbInt32)ceil((interMaxX  - interMinX)/outResX);
		outysize =  (glbInt32)ceil((interMaxY  - interMinY)/outResY);
		
		readCol   = (glbInt32)floor((interMinX - dsMinX)   /resX);
		readRow   = (glbInt32)floor((dsMaxY    - interMaxY)/resY);
		readxsize = (glbInt32)ceil((interMaxX  - interMinX)/resX);
		readysize = (glbInt32)ceil((interMaxY  - interMinY)/resY);

		if(isDem)
		{
			readxsize ++;
			readysize ++;
		}
	}
	{
		glbInt32 cols,rows;
		GetSize(&cols,&rows);
		if(readCol + readxsize > cols)
			readxsize = cols - readCol;
		if(readRow + readysize > rows)
			readysize = rows - readRow;
	}
	GlbPixelTypeEnum rdPixelType = pPixelBlock->GetPixelType();
	glbInt32 nPixelSpace= 0;
	glbInt32 nLineSpace = 0;
	glbInt32 nBandSpace = 0;
	glbInt32 bands      = GetBandCount();
	
	GetRastIOLayout(mpr_pixellayout,rdPixelType,bands,readxsize,readysize,
		            nPixelSpace,nLineSpace,nBandSpace);

	glbByte* pdata = NULL;
	glbByte* pmask = NULL;
	glbref_ptr<CGlbPixelBlock> rdPixelBlock = NULL;
	if(readxsize != outxsize || readysize != outysize)
	{//辨率不一致,重采样.
		rdPixelBlock = new CGlbPixelBlock(rdPixelType,mpr_pixellayout);
		if(!rdPixelBlock->Initialize(bands,readxsize,readysize))
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
	
		rdPixelBlock->GetPixelData(&pdata);	
		rdPixelBlock->GetMaskData(&pmask);
	}else{
		pPixelBlock->GetPixelData(&pdata);
		pPixelBlock->GetMaskData(&pmask);
	}
	{//[互斥]
		GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
		if(GDALDatasetRasterIO(
			mpr_gdalDataset,
			GF_Read,
			readCol,
			readRow,
			readxsize,
			readysize,
			pdata,
			readxsize,
			readysize, 
			GlbPixelTypeToGDAL(rdPixelType),
			bands,
			NULL,
			nPixelSpace,
			nLineSpace,
			nBandSpace) != CE_None)
		{
			CGlbString err = CPLGetLastErrorMsg();
			GlbSetLastError(err.ToWString().c_str());		
			return false;
		}
		
		{//读掩码
			memset(pmask,255,readxsize*readysize);		
			/*
			以第一个波段的掩码做为整个数据集的掩码.
			*/
			GDALRasterBandH maskband = GDALGetMaskBand(GDALGetRasterBand(mpr_gdalDataset,1));
			GDALRasterIO(maskband,
				GF_Read,
				readCol,
				readRow,
				readxsize,
				readysize,
				pmask, 
				readxsize,
				readysize,
				GDT_Byte,0, 0);

			//处理包钢数据---平面模式挖空指定区域、球面模式压平指定区域
			if(mpr_isMaxLevel && (mpr_isChange || mpr_isMove))
			{
				glbref_ptr<CGlbExtent> tempInterExt = mpr_dealExtent->Intersect(*interExt);
				if(tempInterExt.valid())
				{
					glbDouble tempInterMinX,  tempInterMaxX,tempInterMinY,tempInterMaxY;
					tempInterExt->Get(   &tempInterMinX,&tempInterMaxX,&tempInterMinY,&tempInterMaxY);
					glbInt32 tempReadCol,tempReadxsize,tempReadRow, tempReadysize;
					tempReadCol   = (glbInt32)floor((tempInterMinX - interMinX)   /resX);
					//修改日志：压平地形中间总有没有压平的地方
					tempReadRow   = (glbInt32)ceil((interMaxY    - tempInterMaxY)/resY);
					tempReadxsize = (glbInt32)ceil((tempInterMaxX  - tempInterMinX)/resX);
					tempReadysize = (glbInt32)ceil((tempInterMaxY  - tempInterMinY)/resY);

					for(glbInt32 i = tempReadRow; i < tempReadRow + tempReadysize;i++)
					{
						for(glbInt32 j = tempReadCol; j < tempReadCol + tempReadxsize;j++)
						{
							//修改日志：不仅球面可以压平，平面也有压平的需求
							if(/*mpr_pyramid->mpr_isGlobe == true && */mpr_isChange)
							{
								if(isDem)
								{
									//修改dem,球面模式一下，因为需要与全球影像进行融合，
									//所以不能用平面模式那样修改dom进行透明，
									//现在的方案是压平
									if(readxsize != outxsize || readysize != outysize)
									{
										//rdPixelBlock->ReplaceNoData(Nodata,rpValue);
										float val = mpr_changeValue;
										rdPixelBlock->SetPixelVal(0,j,i,&val);
										rdPixelBlock->SetMaskVal(j,i,255);
									}
									else
									{
										//pPixelBlock->ReplaceNoData(Nodata,rpValue);
										float val = mpr_changeValue;
										pPixelBlock->SetPixelVal(0,j,i,&val);
										pPixelBlock->SetMaskVal(j,i,255);
									}
								}
							}
							else if(mpr_pyramid->mpr_isGlobe == false && mpr_isMove)//球面模式不能镂空，因为需要与底层融合，会被重新修改掉
							{
								//平面模式现阶段不需要与其他影像融合所以可以直接修改dom实现镂空
								//用arcGis查看像素值发现边缘位置不是（0，0，0）而是20以内的数值
								if(*(pdata + (i * readxsize + j) * 3 + 0) < 20 &&
									*(pdata + (i * readxsize + j) * 3 + 1) < 20 &&
									*(pdata + (i * readxsize + j) * 3 + 2) < 20)
								{
									//修改dom
									*(pmask + i * readxsize  + j) = 0;
								}
							}//mpr_isChange
						}
					}
				}//tempInterExt.valid()
			}//isBaoGangMaxlevel
		}
	}//[互斥]读
	glbDouble Nodata    = 0.0;
	glbBool   hasNodata = GetNoData(&Nodata);
	glbBool   rt        = true;	

	if(0)
	{
		CGlbWString ssp = CGlbWString::FormatString(L"D:/test1.txt");
		if(readxsize != outxsize || readysize != outysize)
		{
			rdPixelBlock->ExportToText(ssp.c_str());
		}
		else
		{
			pPixelBlock->ExportToText(ssp.c_str());
		}
	}

	if(isReplace && hasNodata)
	{
		if(readxsize != outxsize || readysize != outysize)
			rdPixelBlock->ReplaceNoData(Nodata,rpValue);
		else
			pPixelBlock->ReplaceNoData(Nodata,rpValue);
	}

	if(0)
	{
		CGlbWString ssp = CGlbWString::FormatString(L"D:/test1.txt");
		if(readxsize != outxsize || readysize != outysize)
		{
			rdPixelBlock->ExportToText(ssp.c_str());
		}
		else
		{
			pPixelBlock->ExportToText(ssp.c_str());
		}
	}

	if(readxsize != outxsize || readysize != outysize)
	{//辨率不一致,重采样.		
		if(rspType == GLB_RASTER_NEAREST)
		{
			rt = GlbDownsample_Near
				(
				   rdPixelBlock.get(),
				   pPixelBlock,
				   outCol,
				   outRow,
				   outxsize,
				   outysize,
				   hasNodata,
				   Nodata,
				   isReplace,
				   rpValue
				);
		}else if(rspType == GLB_RASTER_CUBIC){
			rt = GlbDownsample_Cubic
				(
				   rdPixelBlock.get(),
				   pPixelBlock,
				   outCol,
				   outRow,
				   outxsize,
				   outysize,
				   hasNodata,
				   Nodata,
				   isReplace,
				   rpValue
				);
		}else{//GLB_RASTER_AVER
			rt = GlbDownsample_Average
				(
				   rdPixelBlock.get(),
				   pPixelBlock,
				   outCol,
				   outRow,
				   outxsize,
				   outysize
				);
		}
		/*{
			CGlbExtent ext(interMinX,interMaxX,
				           interMinY,interMaxY);
			CGlbWString ssp = CGlbWString::FormatString(L"e:\\a\\test\\temp\\%d.tiff",testCnt);			
			rdPixelBlock->ExportToTiff(ssp.c_str(),&ext,mpr_srs.c_str());
			ssp = CGlbWString::FormatString(L"e:\\a\\test\\temp\\%drp.tiff",testCnt);	
			pPixelBlock->ExportToTiff(ssp.c_str(),destExtent,mpr_srs.c_str());
			testCnt++;
		}*/	
	}//辨率不一致,重采样.
	//if(rt && isReplace && hasNodata )
	//{
	//	pPixelBlock->ReplaceNoData(Nodata,rpValue);
	//}
	if(0)
	{
		CGlbWString ssp = CGlbWString::FormatString(L"D:/test2.txt");
		pPixelBlock->ExportToText(ssp.c_str());
	}
	return rt;
}

glbBool CGlbFileRasterDataset::Write( glbInt32 startColumn, glbInt32 startRow, CGlbPixelBlock *pPixelBlock )
{
	if (mpr_gdalDataset == NULL)
	{
		GlbSetLastError(L"没有原始数据");
		return false;
	}
	if (pPixelBlock==NULL
		||startColumn <0
		||startRow     <0)
	{
		GlbSetLastError(L"有无效参数");
		return false;
	}
	if(pPixelBlock->IsValid() == false)
	{
		GlbSetLastError(L"像素块没初始化");
		return false;
	}
	if (pPixelBlock->GetPixelType() != mpr_pixeltype)
	{
		GlbSetLastError(L"像素类型不一致");
		return false;
	}
 	if (pPixelBlock->GetPixelLayout() != mpr_pixellayout)
 	{
 		GlbSetLastError(L"布局不一致");
 		return false;
 	}
	if (pPixelBlock->GetBandCount() != GetBandCount())
	{
		GlbSetLastError(L"波段数目不一致");
		return false;
	}	
	glbInt32 columns,rows;
	GetSize(&columns, &rows);
	if (   startColumn>=columns
		|| startRow   >=rows)
	{
		GlbSetLastError(L"超范围");
		return false;
	}

	glbInt32 xsize,ysize;
	glbByte* pdata     =NULL;
	glbByte* pmask     =NULL;
	glbInt32 bands     = pPixelBlock->GetBandCount();
	pPixelBlock->GetSize(xsize, ysize);
	glbInt32 wrxsize = xsize;
	glbInt32 wrysize = ysize;

	if (startColumn + xsize >columns)
		wrxsize = columns - startColumn;
	if (startRow + ysize >rows)
		wrysize = rows    - startRow;

	pPixelBlock->GetPixelData(&pdata);
	pPixelBlock->GetMaskData( &pmask);

	glbInt32 nPixelSpace = 0;
	glbInt32 nLineSpace  = 0;
	glbInt32 nBandSpace  = 0;
	GetRastIOLayout(mpr_pixellayout,mpr_pixeltype,bands,xsize,ysize,
		            nPixelSpace,nLineSpace,nBandSpace);

	{//[互斥]写数据
		GlbScopedLock<GlbCriticalSection> lock(mpr_critical);	
		if(GDALDatasetRasterIO(
			mpr_gdalDataset,
			GF_Write,
			startColumn,
			startRow,
			wrxsize,
			wrysize, 
			pdata,
			xsize,
			ysize,
			GlbPixelTypeToGDAL(mpr_pixeltype), 
			bands,
			NULL,
			nPixelSpace,
			nLineSpace,
			nBandSpace)!=  CE_None)
		{
			CGlbString err = CPLGetLastErrorMsg();
			GlbSetLastError(err.ToWString().c_str());
			return false;
		}		
		GDALRasterBandH maskband = GDALGetMaskBand(GDALGetRasterBand(mpr_gdalDataset,1));
		if(maskband != NULL)//表示有mask波段
		{
			if(GDALRasterIO(maskband,
				GF_Write,
				startColumn,
				startRow,
				wrxsize,
				wrysize,
				pmask,
				xsize,
				ysize,
				GDT_Byte,0, 0) != CE_None)
			{
				CGlbString err = CPLGetLastErrorMsg();
				GlbSetLastError(err.ToWString().c_str());
				return false;
			}
		}
		//刷新
		GDALFlushCache(mpr_gdalDataset);
	}
	return true;
}
glbBool CGlbFileRasterDataset::_BuildDomPyramid
	(
	glbInt32      srcCol,
	glbInt32      srcRow,
	glbInt32      srcCols,
	glbInt32      srcRows,
	GlbRasterResampleTypeEnum rspType,
	glbInt32      tileSizeX,
	glbInt32      tileSizeY, 
	glbDouble     lzts, 
	glbBool       isGlobe,
	IGlbProgress* progress
	)
{
	if(0)//替换包钢无效值区域---暂时是没有用到的，先保留，可能以后会用到这个思路
	{
		glbref_ptr<CGlbExtent> destExtent = new CGlbExtent();
		destExtent->Set(4096.226,7499.248,11990.494,15346.032);
		glbref_ptr<CGlbExtent> interExt = GetExtent()->Intersect(*destExtent.get());
		glbDouble resX,   resY;
		glbDouble interMinX,  interMaxX,interMinY,interMaxY;
		glbDouble dsMinX,     dsMaxX,   dsMinY,   dsMaxY;
		glbInt32 readCol,    readxsize,readRow,  readysize;
		GetRS(&resX,&resY);
		GetExtent()->Get(&dsMinX,   &dsMaxX,   &dsMinY,   &dsMaxY);
		interExt->Get(   &interMinX,&interMaxX,&interMinY,&interMaxY);
		readCol   = (glbInt32)floor((interMinX - dsMinX)   /resX);
		readRow   = (glbInt32)floor((dsMaxY    - interMaxY)/resY);
		readxsize = (glbInt32)ceil((interMaxX  - interMinX)/resX);
		readysize = (glbInt32)ceil((interMaxY  - interMinY)/resY);
		GlbPixelTypeEnum pixelType = GLB_PIXELTYPE_BYTE;
		glbInt32 nPixelSpace = 0;
		glbInt32 nLineSpace = 0;
		glbInt32 nBandSpace = 0;
		glbInt32 bands = GetBandCount();
		GetRastIOLayout(mpr_pixellayout,pixelType,bands,readxsize,readysize,
			nPixelSpace,nLineSpace,nBandSpace);
		{//修改掩码
			glbByte *pdata = NULL;
			int dataLen = readxsize * readysize * bands * GLB_PIXELTYPE_BYTE;
			pdata = (glbByte*)malloc(dataLen);

			//首先read一下
			if(GDALDatasetRasterIO(
				mpr_gdalDataset,
				GF_Read,
				readCol,
				readRow,
				readxsize,
				readysize, 
				pdata,
				readxsize,
				readysize,
				GlbPixelTypeToGDAL(GLB_PIXELTYPE_BYTE), 
				bands,
				NULL,
				nPixelSpace,
				nLineSpace,
				nBandSpace)!=  CE_None)
			{
				CGlbString err = CPLGetLastErrorMsg();
				GlbSetLastError(err.ToWString().c_str());
				return false;
			}

			//修改
			memset(pdata,128.0,dataLen);
			if(GDALDatasetRasterIO(
				mpr_gdalDataset,
				GF_Write,
				readCol,
				readRow,
				readxsize,
				readysize, 
				pdata,
				readxsize,
				readysize,
				GlbPixelTypeToGDAL(GLB_PIXELTYPE_BYTE), 
				bands,
				NULL,
				nPixelSpace,
				nLineSpace,
				nBandSpace)!=  CE_None)
			{
				CGlbString err = CPLGetLastErrorMsg();
				GlbSetLastError(err.ToWString().c_str());
				return false;
			}
			//刷新
			GDALFlushCache(mpr_gdalDataset);
			//再read一下
			memset(pdata,0.0,dataLen);
			if(GDALDatasetRasterIO(
				mpr_gdalDataset,
				GF_Read,
				readCol,
				readRow,
				readxsize,
				readysize, 
				pdata,
				readxsize,
				readysize,
				GlbPixelTypeToGDAL(GLB_PIXELTYPE_BYTE), 
				bands,
				NULL,
				nPixelSpace,
				nLineSpace,
				nBandSpace)!=  CE_None)
			{
				CGlbString err = CPLGetLastErrorMsg();
				GlbSetLastError(err.ToWString().c_str());
				return false;
			}

			glbByte* pmask = NULL;
			int maskLen = readxsize * readysize * sizeof(glbByte);
			pmask = (glbByte*)malloc(maskLen);
			memset(pmask,0.0,readxsize*readysize);
			GDALRasterBandH maskband = GDALGetMaskBand(GDALGetRasterBand(mpr_gdalDataset,1));
			if(maskband == NULL)
				return true;
			//先read
			//if(GDALRasterIO(maskband,GF_Read,readCol,readRow,readxsize,readysize,
			//	pmask,readxsize,readysize,GDT_Byte,0,0) != CE_None)
			//{
			//	CGlbString err = CPLGetLastErrorMsg();
			//	GlbSetLastError(err.ToWString().c_str());
			//	return false;
			//}
			//write
			memset(pmask,0.0,maskLen);
			if(GDALRasterIO(maskband,GF_Write,readCol,readRow,readxsize,readysize,
				pmask,readxsize,readysize,GDT_Byte,0,0) != CE_None)
			{
				CGlbString err = CPLGetLastErrorMsg();
				GlbSetLastError(err.ToWString().c_str());
				return false;
			}
			//刷新
			GDALFlushCache(mpr_gdalDataset);

			//再read
			if(GDALRasterIO(maskband,GF_Read,readCol,readRow,readxsize,readysize,
				pmask,readxsize,readysize,GDT_Byte,0,0) != CE_None)
			{
				CGlbString err = CPLGetLastErrorMsg();
				GlbSetLastError(err.ToWString().c_str());
				return false;
			}
			return true;
		}
	}
	glbInt32 maxLevel = 0;
	double resX,resY;
	GetRS(&resX, &resY);
	{//确定金字塔级数		
		double resX0       = lzts/tileSizeX;
		double resY0       = lzts/tileSizeY;
		glbInt32 maxlevelX = (glbInt32)(log(resX0/resX)/log(2.0));
		glbInt32 maxlevelY = (glbInt32)(log(resY0/resY)/log(2.0));
		maxLevel           = min(maxlevelX, maxlevelY);
		maxLevel           = max(maxLevel, 0);
	}
	glbInt32 dsRows,dsCols;
	GetSize(&dsCols,&dsRows);
	glbInt32 nSrcCols = srcCols;
	glbInt32 nSrcRows = srcRows;
	if(srcCol + srcCols > dsCols)
		nSrcCols = dsCols - srcCol;
	if(srcRow + srcRows > dsRows)
		nSrcRows = dsRows - srcRow;
	if(mpr_pyramid == NULL)
	{//创建金字塔文件
		mpr_pyramid        = new CGlbSQLitePyramid();
		CGlbWString dbname = dynamic_cast<CGlbFileDataSource*>(mpr_ds.get())->GetURL();
		dbname             += mpr_name;
		dbname             += L".pyd";
		if(!mpr_pyramid->Create(dbname.c_str()))
		{
			DeletePyramid();
			return false;
		}
	}
	{//设置金字塔信息		
		mpr_pyramid->mpr_isGlobe     = isGlobe;
		mpr_pyramid->mpr_pixeltype   = GLB_PIXELTYPE_BYTE;
		mpr_pyramid->mpr_pixellayout = GLB_PIXELLAYOUT_BIP;//像素交叉模式			
		mpr_pyramid->mpr_bandCount   = 4;

		glbDouble minx,maxx,miny,maxy;
		GetExtent()->Get(&minx, &maxx, &miny, &maxy);
		glbDouble originX = -180.0;
		glbDouble originY = 90.0;
		if(!isGlobe)
		{
			originX=originY = 0.0;
		}
		glbDouble level_ts = lzts/pow(2.0, maxLevel);
		glbInt32 tileStartRow = 0;   
		glbInt32 tileEndRow =  0;
		if(isGlobe)
		{
			tileStartRow = (glbInt32)floor((originY - maxy)/level_ts);
			tileEndRow = (glbInt32)ceil((originY - miny)/level_ts) - 1;
		}
		else
		{
			tileStartRow = (glbInt32)floor(miny/level_ts);
			tileEndRow = (glbInt32)ceil(maxy/level_ts) - 1;
		}
		glbInt32 tileStartCol = (glbInt32)floor((minx-originX)/level_ts);
		glbInt32 tileEndCol =(glbInt32)ceil((maxx-originX)/level_ts)-1;
		glbDouble temporY, temporX = tileStartCol*level_ts+originX;
		if(isGlobe)
			temporY = originY - tileStartRow*level_ts;
		else
		{
			temporY = (tileEndRow+1)*level_ts-originY;
		}

		mpr_pyramid->mpr_lzts        = lzts;
		mpr_pyramid->mpr_tileSizeX   = tileSizeX;
		mpr_pyramid->mpr_tileSizeY   = tileSizeY;
		mpr_pyramid->mpr_minLevel    = 0;
		mpr_pyramid->mpr_maxLevel    = maxLevel;
		mpr_pyramid->mpr_originX     = temporX;
		mpr_pyramid->mpr_originY     = temporY;
		mpr_pyramid->mpr_rows        = tileEndRow - tileStartRow + 1;
		mpr_pyramid->mpr_cols        = tileEndCol - tileStartCol + 1;
		mpr_pyramid->mpr_srs         = mpr_srs;
		mpr_pyramid->mpr_isShareEdge = false;
		mpr_pyramid->mpr_compress    = GLB_COMPRESSTYPE_JPEG;//这个是不是可以灵活ZIP	

		mpr_pyramid->UpdatePydInfo();
	}
	CGlbFileRasterDataset* srcDataset = this;
	for (glbInt32 ilevel=maxLevel;ilevel>=0;ilevel--)
	{
		if(ilevel == maxLevel)
			mpr_isMaxLevel = true;
		else
			mpr_isMaxLevel = false;
		CGlbFileRasterDataset* tmpDataset = _BuildDomPyramidLevel
			(
			srcCol,
			srcRow,
			nSrcCols,
			nSrcRows,
			rspType,
			tileSizeX,
			tileSizeY,			
			lzts, 
			isGlobe,
			progress,
			ilevel,
			srcDataset
			);
		if(srcDataset != this)
		{			
			CGlbWString tname = srcDataset->GetName();
			delete srcDataset;
			mpr_ds.get()->DeleteDataset(tname.c_str());
		}
		if(tmpDataset == NULL)
			break;
		if(ilevel != 0)
		{
			srcDataset = tmpDataset;			
		}else{
			CGlbWString tname = tmpDataset->GetName();
			delete tmpDataset;
			mpr_ds.get()->DeleteDataset(tname.c_str());
		}
	}
	return true;
}

CGlbFileRasterDataset* CGlbFileRasterDataset::_BuildDomPyramidLevel
	(
	glbInt32      srcCol,
	glbInt32      srcRow,
	glbInt32      srcCols,
	glbInt32      srcRows,
	GlbRasterResampleTypeEnum rspType,
	glbInt32      tileSizeX,
	glbInt32      tileSizeY, 
	glbDouble     lzts, 
	glbBool       isGlobe,
	IGlbProgress* progress,
	glbInt32      level,
	CGlbFileRasterDataset *srcDataset
	)
{
	glbDouble level_ts   = lzts/pow(2.0, level);
	double    level_resX = level_ts/tileSizeX;
	double    level_resY = level_ts/tileSizeY;

	glbDouble originX  = -180.0;
	glbDouble originY  = 90.0;
	if(!isGlobe)
		originX=originY = 0.0;

	glbDouble minx,maxx,miny,maxy;
	glbDouble srcResX,srcResY;
	GetRS(&srcResX,&srcResY);
	GetExtent()->Get(&minx,&maxx,&miny,&maxy);
	maxx = minx + (srcCol + srcCols) * srcResX;
	minx = minx + srcCol * srcResX;			
	miny = maxy - (srcRow + srcRows) * srcResY;
	maxy = maxy - srcRow *srcResY;

	glbInt32 tileStartRow = 0;   
	glbInt32 tileEndRow   = 0;
	if(isGlobe)
	{
		tileStartRow = (glbInt32)floor((originY - maxy)/level_ts);
		tileEndRow   = (glbInt32)ceil((originY - miny) /level_ts) - 1;
	}
	else
	{
		tileStartRow = (glbInt32)floor(miny/level_ts);
		tileEndRow   = (glbInt32)ceil( maxy/level_ts) - 1;
	}

	glbInt32 tileStartCol = (glbInt32)floor((minx-originX)/level_ts);
	glbInt32 tileEndCol   = (glbInt32)ceil( (maxx-originX)/level_ts)-1;
	
	//生成一个临时数据集		
	glbDouble temporY, temporX = tileStartCol*level_ts + originX;
	if(isGlobe)
		temporY = originY - tileStartRow*level_ts;
	else
		temporY = (tileEndRow+1)*level_ts-originY;
	CGlbWString tempname =  CGlbPath::RemoveExtName(mpr_name) +  L"_";
	tempname += CGlbConvert::Int32ToWStr(level);
	tempname += L"_";
	tempname += CGlbConvert::Int32ToWStr(srcCol);
	tempname += L"_";
	tempname += CGlbConvert::Int32ToWStr(srcRow);
	tempname += L"~.tif";

	GlbRasterStorageDef storageDef;
	if (tileSizeX<64)
		storageDef.blockSizeX = 64;
	else
	{
		if (tileSizeX%2!=0)
			storageDef.blockSizeX = tileSizeX-1;
		else
			storageDef.blockSizeX = tileSizeX;		
	}
	if (tileSizeY<64)
		storageDef.blockSizeY = 64;
	else
	{
		if (tileSizeY%2!=0)
			storageDef.blockSizeY = tileSizeY-1;
		else
			storageDef.blockSizeY = tileSizeY;
	}
	storageDef.pixelLayout = GLB_PIXELLAYOUT_BIP;//临时数据集的格式
	storageDef.compress    = GLB_COMPRESSTYPE_LZW;
	storageDef.hasNodata   = false;
	double nodata =0.0;
	if (GetNoData(&nodata))
	{
		storageDef.hasNodata = true;
		storageDef.nodata    = nodata;
	}

	glbInt32 bands = GetBandCount();
	glbref_ptr<CGlbFileRasterDataset> tempds = (CGlbFileRasterDataset*)mpr_ds.get()->CreateRasterDataset
		(
		tempname.c_str(), L"", 
		bands,
		mpr_pyramid->mpr_pixeltype,//mpr_pixeltype,
		(tileEndCol-tileStartCol+1)*tileSizeX,
		(tileEndRow-tileStartRow+1)*tileSizeY, 
		temporX,
		temporY, 
		level_resX,
		level_resY,
		L"GTIFF",
		GetSRS(),
		&storageDef);
	if(tempds == NULL)return NULL;

	CGlbExtent extent;
	glbref_ptr<CGlbPixelBlock> pixelblock = new CGlbPixelBlock(mpr_pyramid->mpr_pixeltype,
		                                                       mpr_pyramid->mpr_pixellayout);
	if(!pixelblock->Initialize(bands,tileSizeX,tileSizeY))
	{
		GlbSetLastError(L"内存不足");
		return NULL;
	}	
	glbDouble rdMinX,rdMaxX;
	glbDouble rdMinY,rdMaxY;
	glbInt32  writeRow,writeCol;
	glbBool   isBorder = false;

	mpr_pyramid->BeginTransaction();
	glbInt32 hasSqltimes = 0;
	for(glbInt32 tileRow=tileStartRow; tileRow<=tileEndRow; tileRow++)
	{
		if(isGlobe)
		{
			rdMinY = originY - (tileRow+1)*level_ts;
			rdMaxY = originY - tileRow    *level_ts;
			writeRow = (tileRow - tileStartRow)*tileSizeY;
		}
		else
		{
			rdMinY = tileRow    *level_ts - originY;
			rdMaxY = (tileRow+1)*level_ts - originY;
			writeRow = (tileEndRow - tileRow)*tileSizeY;
		}

		for(glbInt32 tileCol=tileStartCol; tileCol<=tileEndCol; tileCol++)
		{
			isBorder = false;
			if(tileCol == tileStartCol || tileCol == tileEndCol
				||tileRow == tileStartRow ||tileRow == tileEndRow
				)
			{
				isBorder = true;
			}
			rdMinX = tileCol*level_ts    + originX;
	        rdMaxX = (tileCol+1)*level_ts+ originX;
	        writeCol = (tileCol - tileStartCol)*tileSizeX;
			extent.Set(rdMinX,rdMaxX,rdMinY,rdMaxY);
			//bool isReplace = level==MAXLEVEL?true:fasle;
			//if(srcDataset->ReadEx(&extent,pixelblock.get(),rspType/*,false,isReplace*/)== false)
			if(srcDataset->_ReadEx(&extent,pixelblock.get(),rspType,true)== false)
			{
				mpr_pyramid->CommitTransaction();
				return NULL;
			}
			if(tempds->Write(writeCol,writeRow,pixelblock.get()) == false)
			{
				mpr_pyramid->CommitTransaction();
				return NULL;
			}
			if(_BuildDomPyramidLevelTile(tileRow,tileCol,level,pixelblock.get(),isBorder) == false)
			{
				mpr_pyramid->CommitTransaction();
				return NULL;
			}
			hasSqltimes ++;
			if(hasSqltimes == 100)
			{
				mpr_pyramid->CommitTransaction();
				mpr_pyramid->BeginTransaction();
				hasSqltimes = 0;
			}
			pixelblock->Clear();
		}
	}
	mpr_pyramid->CommitTransaction();
	return tempds.release();
}

glbBool CGlbFileRasterDataset::_BuildDomPyramidLevelTile
	(
	glbInt32 tileRow,
	glbInt32 tileCol,
	glbInt32 level,
	CGlbPixelBlock* pixelblock,
	glbBool  isBorder
)
{
	glbref_ptr<CGlbPixelBlock> wrBlock = pixelblock;
	glbInt32 bands = pixelblock->GetBandCount();
	glbInt32 colums;
	glbInt32 rows;
	pixelblock->GetSize(colums,rows);
	if(bands == 3)
	{		
		glbref_ptr<CGlbPixelBlock> newPixelblock = new CGlbPixelBlock(mpr_pyramid->mpr_pixeltype,
		                                                              mpr_pyramid->mpr_pixellayout);
		if(!newPixelblock->Initialize(bands+1,colums,rows))
		{				
			GlbSetLastError(L"内存不足");
			return false;
		}
		glbInt32 count = colums*rows;
		glbByte* pdata=NULL;
		glbByte* pmask=NULL;
		pixelblock->GetPixelData(&pdata);
		pixelblock->GetMaskData(&pmask);
		glbByte* newpdata=NULL;
		glbByte* newpmask=NULL;
		newPixelblock->GetPixelData(&newpdata);
		newPixelblock->GetMaskData(&newpmask);
		for(glbInt32 i=0;i<count;i++)
		{
			*(newpmask+i)=*(pmask+i);
			*(newpdata+i*(bands+1))   = *(pdata+i*bands);	
			*(newpdata+i*(bands+1)+1) = *(pdata+i*bands+1);
			*(newpdata+i*(bands+1)+2) = *(pdata+i*bands+2);

			if(*(pmask+i) ==0)			
				*(newpdata+i*(bands+1)+3) = 0;
			else
				*(newpdata+i*(bands+1)+3) = 255;	
		}
		wrBlock = newPixelblock;
	}
	if(isBorder)//因为程序现在实现的是分块生成金字塔，所以相邻块的区域需要进行融合
	{
		glbref_ptr<CGlbPixelBlock> oldPixelblock = new CGlbPixelBlock(mpr_pyramid->mpr_pixeltype,
		                                                              mpr_pyramid->mpr_pixellayout);
		if(!oldPixelblock->Initialize(4,colums,rows))
		{				
			GlbSetLastError(L"内存不足");
			return false;
		}
		if(mpr_pyramid->Read(tileRow, tileCol, level, oldPixelblock.get()))
		{
			glbByte* pdata = NULL;
			glbByte* pmask = NULL;
			glbByte* olddata=NULL;
			glbByte* oldmask=NULL;
			wrBlock->GetPixelData(&pdata);
			wrBlock->GetMaskData(&pmask);
			oldPixelblock->GetPixelData(&olddata);
			oldPixelblock->GetMaskData(&oldmask);
			for(glbInt32 i=0;i<colums*rows;i++)
			{
				if(*(oldmask+i) == 255
					&&*(pmask+i) == 0
					)
				{
					*(pdata + i*4)     = *(olddata + i*4);
					*(pdata + i*4 + 1) = *(olddata + i*4 + 1);
					*(pdata + i*4 + 2) = *(olddata + i*4 + 2);
					*(pdata + i*4 + 3) = *(olddata + i*4 + 3);
					*(pmask + i) = *(oldmask+i);
				}
			}
		}
	}//边界块
	return mpr_pyramid->Write(tileRow, tileCol, level, wrBlock.get());
}

glbBool CGlbFileRasterDataset::_BuildDemPyramid( 
		                      glbInt32      srcCol,
		                      glbInt32      srcRow,
							  glbInt32      srcColumns,
							  glbInt32      srcRows,
							  GlbRasterResampleTypeEnum rspType,
		                      glbInt32      tileSizeX, 
		                      glbInt32      tileSizeY, 
							  glbDouble     lzts, 
		                      glbBool       isGlobe, 
							  glbBool       isReplace,
							  glbDouble     rpValue, 
							  IGlbProgress* progress)
{
	glbInt32  maxLevel = 0;	
	glbInt32  dsRows,dsCols;
	glbDouble resX,resY;
	{//确定Level
		GetRS(&resX, &resY);
		glbDouble resX0     = lzts/(tileSizeX -1);
		glbDouble resY0     = lzts/(tileSizeY -1);
		glbInt32  maxlevelX = (glbInt32)(log(resX0/resX)/log(2.0));
		glbInt32  maxlevelY = (glbInt32)(log(resY0/resY)/log(2.0));
		maxLevel            = min(maxlevelX, maxlevelY);
		maxLevel            = max(maxLevel, 0);
	}
	
	GetSize(&dsCols,&dsRows);
	glbInt32 nSrcColumns    = srcColumns;
	glbInt32 nSrcRows       = srcRows;
	if(srcCol + srcColumns > dsCols)
		nSrcColumns = dsCols - srcCol;
	if(srcRow + srcRows > dsRows)
		nSrcRows = dsRows - srcRow;	
	
	glbDouble dsOriginX,dsOriginY;
	GetExtent()->Get(&dsOriginX, NULL, NULL, &dsOriginY);
	glbDouble srcMinX =  dsOriginX + resX / 2;
	glbDouble srcMaxX = dsCols  * resX + dsOriginX - resX / 2;
	glbDouble srcMinY =  dsOriginY - dsRows * resY + resY / 2;
	glbDouble srcMaxY =  dsOriginY - resY / 2;
	
	if(mpr_pyramid == NULL)
	{//创建金字塔数据库
		mpr_pyramid        = new CGlbSQLitePyramid();
		CGlbWString dbname = dynamic_cast<CGlbFileDataSource*>(mpr_ds.get())->GetURL();
		dbname             += mpr_name;
		dbname             += L".pyd";
		if(!mpr_pyramid->Create(dbname.c_str()))
		{
			DeletePyramid();				
			return false;
		}
	}
	
	{//设置金字塔信息
		glbDouble originX = -180.0;
		glbDouble originY = 90.0;
		if(!isGlobe)originX=originY = 0.0;
		glbInt32 bands = GetBandCount();
		//计算规范后的范围、块编号
		glbDouble level_ts     = lzts/pow(2.0, maxLevel);
		glbInt32 tileStartRow = 0;   
		glbInt32 tileEndRow   = 0;
		if(isGlobe)
		{
			tileStartRow = (glbInt32)floor((originY - srcMaxY)/level_ts);
			tileEndRow   = (glbInt32)ceil(( originY - srcMinY)/level_ts) - 1;
		}
		else
		{
			tileStartRow = (glbInt32)floor(srcMinY/level_ts);
			tileEndRow   = (glbInt32)ceil( srcMaxY/level_ts) - 1;
		}
		glbInt32  tileStartCol = (glbInt32)floor((srcMinX - originX)/level_ts);
		glbInt32  tileEndCol   = (glbInt32)ceil(( srcMaxX - originX)/level_ts)-1;
		glbDouble maxLevelOrginY, maxLevelOrginX;
		maxLevelOrginX = tileStartCol*level_ts + originX;
		if(isGlobe)
			maxLevelOrginY = originY - tileStartRow*level_ts;		
		else
			maxLevelOrginY = (tileEndRow+1)*level_ts-originY;

		mpr_pyramid->mpr_pixeltype   = GLB_PIXELTYPE_FLOAT32;//高程必须是float的
		mpr_pyramid->mpr_pixellayout = GLB_PIXELLAYOUT_BIP;//像素交叉模式
		mpr_pyramid->mpr_isGlobe     = isGlobe;
		mpr_pyramid->mpr_bandCount   = bands;
		mpr_pyramid->mpr_lzts        = lzts;
		mpr_pyramid->mpr_tileSizeX   = tileSizeX;
		mpr_pyramid->mpr_tileSizeY   = tileSizeY;
		mpr_pyramid->mpr_minLevel    = 0;
		mpr_pyramid->mpr_maxLevel    = maxLevel;
		mpr_pyramid->mpr_originX     = maxLevelOrginX;
		mpr_pyramid->mpr_originY     = maxLevelOrginY;
		mpr_pyramid->mpr_rows        = (tileEndRow-tileStartRow+1);
		mpr_pyramid->mpr_cols        = (tileEndCol-tileStartCol+1);
		mpr_pyramid->mpr_srs         = mpr_srs;
		mpr_pyramid->mpr_isShareEdge = true;
		mpr_pyramid->mpr_compress = GLB_COMPRESSTYPE_ZLIB;
		mpr_pyramid->UpdatePydInfo();
	}		
    
	CGlbFileRasterDataset* srcDataset = this;
	for (glbInt32 ilevel=maxLevel;ilevel>=0;ilevel--)
	{
		if(ilevel == maxLevel)
			mpr_isMaxLevel = true;
		else
			mpr_isMaxLevel = false;

		CGlbFileRasterDataset* tmpDataset = _BuildDemPyramidLevel
			(
			srcCol,
		    srcRow,
			nSrcColumns,
			nSrcRows,
			rspType,
			tileSizeX,
			tileSizeY,			
			lzts, 
			isGlobe,
			isReplace,
			rpValue, 
			progress,
			ilevel,
			maxLevel,
			srcDataset
			);

		if(srcDataset != this)
		{			
			CGlbWString tname = srcDataset->GetName();
			delete srcDataset;
			((IGlbDataSource*)mpr_ds.get())->DeleteDataset(tname.c_str());
		}
		if(tmpDataset == NULL)
			break;
		if(ilevel != 0)
		{
			srcDataset = tmpDataset;
		}else{
			CGlbWString tname = tmpDataset->GetName();
			delete tmpDataset;
			mpr_ds.get()->DeleteDataset(tname.c_str());
		}
	}
	return true;
}

CGlbFileRasterDataset* CGlbFileRasterDataset::_BuildDemPyramidLevel
							(
		                      glbInt32      srcCol,
		                      glbInt32      srcRow,
							  glbInt32      srcCols,
							  glbInt32      srcRows,
							  GlbRasterResampleTypeEnum rspType,
		                      glbInt32      tileSizeX, 
		                      glbInt32      tileSizeY, 
							  glbDouble     lzts, 
		                      glbBool       isGlobe, 
							  glbBool       isReplace,
							  glbDouble     rpValue, 
							  IGlbProgress* progress,
							  glbInt32      level,
							  glbInt32      maxLevel,
		                      CGlbFileRasterDataset *srcDataset)
{
	glbDouble level_ts   = lzts/pow(2.0, level);
	double    level_resX = level_ts/(tileSizeX - 1);
	double    level_resY = level_ts/(tileSizeY - 1);

	glbDouble originX  = -180.0;
	glbDouble originY  = 90.0;
	if(!isGlobe)originX = originY = 0.0;
	glbInt32 tileStartRow = 0;   
	glbInt32 tileEndRow   = 0;

	glbDouble srcMinX,srcMaxX;
	glbDouble srcMinY,srcMaxY;
	glbDouble srcResX,srcResY;	
	{//数据读取范围
		glbDouble srcOX,srcOY;		
		GetExtent()->Get(&srcOX,NULL,NULL,&srcOY);
		GetRS(&srcResX,&srcResY);
		srcMinX =  srcCol*srcResX + srcOX + srcResX / 2;
		srcMaxX = (srcCol+srcCols/* -1*/)*srcResX + srcOX - srcResX / 2;
		srcMinY =  srcOY - (srcRow + srcRows/* -1*/) * srcResY + srcResY / 2;
		srcMaxY =  srcOY -  srcRow * srcResY - srcResY / 2;
	}
	
	if(isGlobe)
	{
		tileStartRow = (glbInt32)floor((originY - srcMaxY)/level_ts);
		tileEndRow   = (glbInt32)ceil( (originY - srcMinY)/level_ts) - 1;
	}
	else
	{
		tileStartRow = (glbInt32)floor(srcMinY/level_ts);
		tileEndRow   = (glbInt32)ceil( srcMaxY/level_ts) - 1;
	}

	glbInt32 tileStartCol = (glbInt32)floor((srcMinX-originX)/level_ts);
	glbInt32 tileEndCol   = (glbInt32)ceil( (srcMaxX-originX)/level_ts)-1;


	//生成一个临时数据集		
	glbDouble temporY, temporX = tileStartCol*level_ts + originX;
	if(isGlobe)
		temporY = originY - tileStartRow*level_ts;
	else
		temporY = (tileEndRow+1)*level_ts-originY;

	CGlbWString tempname =  CGlbPath::RemoveExtName(mpr_name) +  L"_";
	tempname += CGlbConvert::Int32ToWStr(level);
	tempname += L"_";
	tempname += CGlbConvert::Int32ToWStr(srcCol);
	tempname += L"_";
	tempname += CGlbConvert::Int32ToWStr(srcRow);
	tempname += L"~.tif";

	GlbRasterStorageDef storageDef;
	if (tileSizeX<64)
		storageDef.blockSizeX = 64;
	else
	{
		if (tileSizeX%2!=0)
			storageDef.blockSizeX = tileSizeX-1;
		else
			storageDef.blockSizeX = tileSizeX;		
	}
	if (tileSizeY<64)
		storageDef.blockSizeY = 64;
	else
	{
		if (tileSizeY%2!=0)
			storageDef.blockSizeY = tileSizeY-1;
		else
			storageDef.blockSizeY = tileSizeY;
	}
	storageDef.pixelLayout = GLB_PIXELLAYOUT_BIP;//临时数据集的格式
	storageDef.compress    = GLB_COMPRESSTYPE_ZLIB;
	storageDef.hasNodata   = false;
	double nodata = 0.0;
	if (GetNoData(&nodata))
	{
		storageDef.hasNodata = true;
		storageDef.nodata    = nodata;
	}

	glbInt32 bands = GetBandCount();
	CGlbFileRasterDataset* tempds = (CGlbFileRasterDataset* )mpr_ds.get()->CreateRasterDataset
									(
									tempname.c_str(), L"", 
									bands,
									mpr_pyramid->mpr_pixeltype,
									(tileEndCol-tileStartCol+1)*(tileSizeX - 1)+1,
									(tileEndRow-tileStartRow+1)*(tileSizeY - 1)+1, 
									temporX,
									temporY, 
									level_resX,
									level_resY,
									L"GTIFF",
									GetSRS(),
									&storageDef);
	if(tempds == NULL)return NULL;
	CGlbExtent rdExt;
	glbref_ptr<CGlbPixelBlock> rdPixelblock = new CGlbPixelBlock(mpr_pyramid->mpr_pixeltype,mpr_pixellayout);
	if(!rdPixelblock->Initialize(bands,tileSizeX,tileSizeY))
	{
		delete tempds;
		GlbSetLastError(L"内存不足");
		return NULL;
	}
	glbDouble rdMinX,rdMaxX;
	glbDouble rdMinY,rdMaxY;
	glbInt32  wrtCol,wrtRow;
	for(glbInt32 irow=tileStartRow; irow<=tileEndRow; irow++)
	{
		for(glbInt32 icol=tileStartCol; icol<=tileEndCol; icol++)
		{			
			if(isGlobe)
			{
				rdMinY = originY - (irow + 1) * level_ts;
				rdMaxY = originY - irow       * level_ts;			
			}
			else
			{
				rdMinY = irow    *level_ts - originY;
				rdMaxY = (irow+1)*level_ts - originY;
			}
			rdMinX  = icol   *level_ts + originX;
			rdMaxX = (icol+1)*level_ts + originX;
			rdExt.Set(rdMinX,rdMaxX,rdMinY,rdMaxY);		
			if(isGlobe)
				wrtRow = (irow - tileStartRow) * (tileSizeY - 1);
			else
				wrtRow = (tileEndRow - irow) * (tileSizeY - 1);
			wrtCol = (icol - tileStartCol)   * (tileSizeX - 1);
		
			isReplace = (isReplace && level == maxLevel)?true:false;
			if(!srcDataset->_ReadEx(&rdExt,rdPixelblock.get(),rspType,isReplace,rpValue,true))
			{
				delete tempds;return NULL;
			}
			if(!tempds->Write(wrtCol,wrtRow,rdPixelblock.get()))
			{
				delete tempds;return NULL;
			}			
			rdPixelblock.get()->Clear(0.0);
		}
	}

	//处理导出切片
	CGlbFileRasterDataset* preDataset = NULL;
	if(level != maxLevel)preDataset = srcDataset;
	_BuildDemPyramidLevel(
		isGlobe,
		level,
		tileStartCol,
		tileEndCol,
		tileStartRow,
		tileEndRow,
		tileSizeX,
		tileSizeY,
		tempds,
		preDataset);
	return tempds;
}

glbBool CoverEffectiveValue(CGlbPixelBlock *pydPixelblock,
	CGlbPixelBlock *prePydPixelblock,glbInt32 preTileRow, glbInt32 preTileCol)
{
	if(pydPixelblock == NULL || prePydPixelblock == NULL)
		return false;

	glbFloat *val;
	glbByte *mval;
	glbByte *pmask = NULL;
	glbInt32 tileSizeX,tileSizeY;
	pydPixelblock->GetSize(tileSizeX,tileSizeY);
	pydPixelblock->GetMaskData(&pmask);

	if(preTileRow == 0 && preTileCol == 0)
	{
		//覆盖第一行的前半部分
		glbInt32 r = 0;
		for(glbInt32 c = 0; c < tileSizeX;c++)
		{
			if(c%2 != 0) continue;
			prePydPixelblock->GetMaskVal(c,r,&mval);
			if(*mval == 0) continue;
			prePydPixelblock->GetPixelVal(0,c,r,(void**)&val);
			pydPixelblock->SetPixelVal        (0,c/2,r,val);
			*(pmask + r * tileSizeX + c / 2) = *mval;
		}
		//覆盖第一列的前半部分
		glbInt32 c = 0;
		for(glbInt32 r = 0; r < tileSizeY;r++)
		{
			if(r%2 != 0) continue;
			prePydPixelblock->GetMaskVal(c,r,&mval);
			if(*mval == 0) continue;
			prePydPixelblock->GetPixelVal(0,c,r,(void**)&val);
			pydPixelblock->SetPixelVal        (0,c,r/2,val);
			*(pmask + r / 2 * tileSizeX + c) = *mval;
		}
	}
	else if(preTileRow == 0 && preTileCol == 1)
	{
		//覆盖第一行的后半部分
		glbInt32 r = 0;
		for(glbInt32 c = tileSizeX/* + 1*/; c < 2 * tileSizeX - 1;c++)
		{
			if(c%2 != 0) continue;
			prePydPixelblock->GetMaskVal(c - tileSizeX + 1,r,&mval);
			if(*mval == 0) continue;
			prePydPixelblock->GetPixelVal(0,c - tileSizeX + 1,r,(void**)&val);
			pydPixelblock->SetPixelVal(0,c / 2,r,val);
			*(pmask + r * tileSizeX + c / 2) = *mval;
		}
		//覆盖最后一列的前半部分
		glbInt32 c = tileSizeX - 1;
		for(glbInt32 r = 0; r < tileSizeY;r++)
		{
			if(r%2 != 0) continue;
			prePydPixelblock->GetMaskVal(c,r,&mval);
			if(*mval == 0) continue;
			prePydPixelblock->GetPixelVal(0,c,r,(void**)&val);
			pydPixelblock->SetPixelVal        (0,c,r/2,val);
			*(pmask + r / 2 * tileSizeX + c) = *mval;
		}
	}
	else if(preTileRow == 1 && preTileCol == 0)
	{
		//覆盖最后一行的前半部分
		glbInt32 r = tileSizeY - 1;
		for(glbInt32 c = 0; c < tileSizeX;c++)
		{
			if(c%2 != 0) continue;
			prePydPixelblock->GetMaskVal(c,r,&mval);
			if(*mval == 0) continue;
			prePydPixelblock->GetPixelVal(0,c,r,(void**)&val);
			pydPixelblock->SetPixelVal        (0,c/2,r,val);
			*(pmask + r * tileSizeX + c / 2) = *mval;
		}
		//覆盖第一列的后半部分
		glbInt32 c = 0;
		for(glbInt32 r = tileSizeY/* + 1*/; r < 2 * tileSizeY - 1;r++)
		{
			if(r%2 != 0) continue;
			prePydPixelblock->GetMaskVal(c,r - tileSizeY + 1,&mval);
			if(*mval == 0) continue;
			prePydPixelblock->GetPixelVal(0,c,r - tileSizeY + 1,(void**)&val);
			pydPixelblock->SetPixelVal(0,c,r / 2,val);
			*(pmask + r / 2 * tileSizeX + c) = *mval;
		}
	}
	else if(preTileRow == 1 && preTileCol == 1)
	{
		//覆盖最后一行的后半部分
		glbInt32 r = tileSizeY - 1;
		for(glbInt32 c = tileSizeX/* + 1*/; c < 2 * tileSizeX - 1;c++)
		{
			if(c%2 != 0) continue;
			prePydPixelblock->GetMaskVal(c - tileSizeX + 1,r,&mval);
			if(*mval == 0) continue;
			prePydPixelblock->GetPixelVal(0,c - tileSizeX + 1,r,(void**)&val);
			pydPixelblock->SetPixelVal        (0,c/2,r,val);
			*(pmask + r * tileSizeX + c / 2) = *mval;
		}
		//覆盖最后一列的后半部分
		glbInt32 c = tileSizeX - 1;
		for(glbInt32 r = tileSizeY/* + 1*/; r < 2 * tileSizeY - 1;r++)
		{
			if(r%2 != 0) continue;
			prePydPixelblock->GetMaskVal(c,r - tileSizeY + 1,&mval);
			if(*mval == 0) continue;
			prePydPixelblock->GetPixelVal(0,c,r - tileSizeY + 1,(void**)&val);
			pydPixelblock->SetPixelVal(0,c,r / 2,val);
			*(pmask + r / 2 * tileSizeX + c) = *mval;
		}
	}

	return true;
}

glbBool MergeEffectiveValue(CGlbPixelBlock *pydPixelblock,CGlbPixelBlock *prePydPixelblock)
{
	if(pydPixelblock == NULL || prePydPixelblock == NULL)
		return false;

	glbFloat *val;
	glbByte *mval;
	glbByte *pmask = NULL;
	glbInt32 tileSizeX,tileSizeY;
	pydPixelblock->GetSize(tileSizeX,tileSizeY);
	pydPixelblock->GetMaskData(&pmask);

	for(glbInt32 r = 0; r < tileSizeY; r++)
	{
		for(glbInt32 c = 0; c < tileSizeX; c++)
		{
			prePydPixelblock->GetMaskVal(c,r,&mval);
			if(*mval != 0)
			{
				prePydPixelblock->GetPixelVal(0,c,r,(void**)&val);
				pydPixelblock->SetPixelVal(0,c,r,val);
				*(pmask + r * tileSizeX + c) = *mval;
			}
		}
	}
	return true;
}

glbBool CGlbFileRasterDataset::_BuildDemPyramidLevel(
	                          glbBool   isGlobe,
	                          glbInt32  level,
							  glbInt32  tileStartCol,
		                      glbInt32  tileEndCol,
                              glbInt32  tileStartRow,
		                      glbInt32  tileEndRow, 							   	                      
		                      glbInt32  tileSizeX,
		                      glbInt32  tileSizeY, 		                      
		                      CGlbFileRasterDataset* pydDataset,
		                      CGlbFileRasterDataset* preDataset)
{	
	glbInt32 bands = pydDataset->GetBandCount();
	glbref_ptr<CGlbPixelBlock> pydPixelblock = new CGlbPixelBlock(mpr_pyramid->mpr_pixeltype,mpr_pixellayout);
	if(!pydPixelblock->Initialize(bands,tileSizeX,tileSizeY))
	{
		GlbSetLastError(L"内存不足");
		return false;
	}
	glbref_ptr<CGlbPixelBlock> prePydPixelblock = NULL;
	if(preDataset)
	{
		prePydPixelblock = new CGlbPixelBlock(mpr_pyramid->mpr_pixeltype,mpr_pixellayout);
		if(!prePydPixelblock->Initialize(bands,tileSizeX,tileSizeY))
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
	}
	glbInt32 preTileStartRow = 0;
	glbInt32 preTileEndRow   = 0;
	glbInt32 preTileStartCol = 0;
	glbInt32 preTileEndCol   = 0;
	glbInt32 pydRdCol;
	glbInt32 pydRdRow;
	mpr_pyramid->BeginTransaction();
	glbInt32 hasSqltimes = 0;
	for(glbInt32 irow = tileStartRow; irow <= tileEndRow;irow++)
	{
		if(isGlobe)
			pydRdRow = (irow - tileStartRow)*(tileSizeY - 1);
		else
			pydRdRow = (tileEndRow - irow)  *(tileSizeY - 1);
		for(glbInt32 icol = tileStartCol;icol <= tileEndCol;icol++)
		{
			pydRdCol = (icol - tileStartCol)*(tileSizeX - 1);
			pydDataset->Read(pydRdCol,pydRdRow,pydPixelblock.get());
			if(preDataset)
			{
				//分块生成金子塔数据处理不同块之间的有效值覆盖
				if(irow == tileStartRow || icol == tileStartCol)
				{
					if(mpr_pyramid->Read(irow,icol,level,prePydPixelblock.get()))
					{
						if(!MergeEffectiveValue(pydPixelblock.get(),prePydPixelblock.get()))
							return false;
					}
				}
				//替换边界
				preTileStartRow = irow*2;
				preTileEndRow   = irow*2+1;
				preTileStartCol = icol*2;
				preTileEndCol   = icol*2+1;
				for(glbInt32 preTileRow = preTileStartRow;preTileRow<=preTileEndRow;preTileRow++)
				{
					for(glbInt32 preTileCol = preTileStartCol;preTileCol<=preTileEndCol;preTileCol++)
					{
						if(!mpr_pyramid->Read(preTileRow,preTileCol,level + 1,prePydPixelblock.get()))continue;
						if(isGlobe)
						{
							if(!CoverEffectiveValue(pydPixelblock.get(),prePydPixelblock.get(),
								preTileRow - preTileStartRow,preTileCol - preTileStartCol))
								return false;
						}
						else
						{
							if(!CoverEffectiveValue(pydPixelblock.get(),prePydPixelblock.get(),
								1 - (preTileRow - preTileStartRow),preTileCol - preTileStartCol))
								return false;
						}
					}
				}				
			}//替换边界

			//if(level == 2 && irow == 2 && icol == -3)
			//{
				//CGlbWString ssp = CGlbWString::FormatString(L"D:\\level _%d_col_%d_row_%d_testDem.txt",level,icol,irow);			
				//pydPixelblock->ExportToText(ssp.c_str());
			//}
			
			mpr_pyramid->Write(irow,icol,level,pydPixelblock.get());
			hasSqltimes++;
			if(hasSqltimes == 100)
			{
				mpr_pyramid->CommitTransaction();
				mpr_pyramid->BeginTransaction();
				hasSqltimes = 0;
			}
		}//tileCol		
	}//tileRow
	mpr_pyramid->CommitTransaction();
	return true;
}

glbBool CGlbFileRasterDataset::BuildPyramid
	( 
	glbInt32  srcCol,
	glbInt32  srcRow,
	glbInt32  srcCols,
	glbInt32  srcRows,
	GlbRasterResampleTypeEnum rspType,
	glbInt32  tileSizeX, 
	glbInt32  tileSizeY, 
	glbDouble lzts/*36*/, 
	glbBool   isGlobe/*true*/, 
	glbBool   isShareEdge/*false*/,
	glbBool   isReplace/*=false*/,
	glbDouble rpValue/*=0.0*/, 
	IGlbProgress* progress/*NULL*/)
{
	if (mpr_gdalDataset == NULL)
	{
		GlbSetLastError(L"没有数据");
		return false;
	}
	glbInt32 rows,cols;
	GetSize(&cols,&rows);
	if( srcCol <0||srcCol>=cols)
	{
		GlbSetLastError(L"起始列必须>=0小于数据集列数");
		return false;
	}
	if(srcRow <0||srcRow>=rows)
	{
		GlbSetLastError(L"起始行必须>=0小于数据集行数");
		return false;
	}
	if(srcCols<0)
	{
		GlbSetLastError(L"列数必须>0");
		return false;
	}
	if(srcRows<0)
	{
		GlbSetLastError(L"行数必须>0");
		return false;
	}
	if(tileSizeX<=0|| tileSizeY<=0 )
	{
		GlbSetLastError(L"分块尺寸必须>0");
		return false;
	}
	if(lzts<=0)
	{
		GlbSetLastError(L"0级范围必须>0");
		return false;
	}	
	if(isGlobe)
	{
		/*
		X:[-180,180],Y:[-90,90]
		*/
		const CGlbExtent* ext = GetExtent();
		glbDouble minX,maxX,minY,maxY;
		ext->Get(&minX,&maxX,&minY,&maxY);
		if(isShareEdge)
		{
			double resX,resY;
			GetRS(&resX,&resY);
			minX  += resX/2;
			maxX  -= resX/2;
			minY  += resY/2;
			maxY -= resY/2;
		}
		//if(   minX < -180.0||minX>180.0
		//	||maxX < -180.0||maxX>180.0
		//	||minY <-90.0  ||minY>90.0
		//	||maxY <-90.0  ||maxY>90.0
		//	)
		//{
		//	GlbSetLastError(L"球面模式,范围应在X[-180,180],Y[-90,90]");
		//	return false;
		//}
	}
    glbInt32 bands = GetBandCount();
	if(!isShareEdge)
	{
		if(bands !=3 && bands != 4)
		{
			GlbSetLastError(L"影像DOM波段数必须是3或4");
			return false;
		}
		return _BuildDomPyramid
			(
			srcCol,
			srcRow,
			srcCols,
			srcRows,
			rspType,
			tileSizeX, 
			tileSizeY, 
			lzts, 
			isGlobe, 
			progress);
	}
	else
	{
		if(bands != 1)
		{
			GlbSetLastError(L"高程DEM只能一个波段");
			return false;
		}
		return _BuildDemPyramid
			(
			srcCol,
			srcRow,
			srcCols,
			srcRows,
			rspType,
			tileSizeX, 
			tileSizeY, 
			lzts, 
			isGlobe,
			isReplace,
			rpValue, 
			progress);
	}
}

glbBool CGlbFileRasterDataset::DeletePyramid()
{
	if (HasPyramid())
	{
		if (mpr_gdalDataset == NULL)
		{
			GlbSetLastError(L"没有影像文件，不能删除金字塔");
			return false;
		}

		//删除对应的类,也就是要关闭数据库
		mpr_pyramid = NULL;//智能指针，释放内存
		//删除金字塔数据库文件
		CGlbWString dbname = mpr_ds.get()->GetURL();
		dbname += CGlbPath::RemoveExtName(mpr_name);
		dbname += L".pyd";
		DeleteFile(dbname.c_str());
		return true;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

GlbPixelLayoutTypeEnum CGlbFileRasterDataset::GetPyramidPixelLayout()
{
	if (HasPyramid())
	{
		return mpr_pyramid->mpr_pixellayout;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return GLB_PIXELLAYOUT_UNKNOWN;
}

GlbPixelTypeEnum CGlbFileRasterDataset::GetPyramidPixelType()
{
	if (HasPyramid())
	{
		return mpr_pyramid->mpr_pixeltype;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return GLB_PIXELTYPE_UNKNOWN;
}

glbDouble CGlbFileRasterDataset::GetPyramidLZTS()
{
	if (HasPyramid())
	{
		return mpr_pyramid->mpr_lzts;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return 0;
}

glbBool CGlbFileRasterDataset::IsPyramidShareEdge()
{
	if (HasPyramid())
	{
		return mpr_pyramid->mpr_isShareEdge;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

glbBool CGlbFileRasterDataset::GetPyramidLevelRS( glbInt32 level, glbDouble *resolutionX, glbDouble *resolutionY )
{
	if (resolutionX == NULL || resolutionY == NULL)
	{
		GlbSetLastError(L"传入参数不能为NULL");
		return false;
	}
	if (HasPyramid())
	{
		if (level<mpr_pyramid->mpr_minLevel || level>mpr_pyramid->mpr_maxLevel)
		{
			GlbSetLastError(L"获取的金字塔级别不存在");
			return false;
		}
		*resolutionX = (mpr_pyramid->mpr_lzts/pow(2.0, level))/mpr_pyramid->mpr_tileSizeX;
		*resolutionY = (mpr_pyramid->mpr_lzts/pow(2.0, level))/mpr_pyramid->mpr_tileSizeY;
		return true;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

glbBool CGlbFileRasterDataset::GetPyramidLevels( glbInt32* minLevel, glbInt32* maxLevel )
{
	if (minLevel == NULL || maxLevel == NULL)
	{
		GlbSetLastError(L"传入参数不能为NULL");
		return false;
	}
	if (HasPyramid())
	{
		*minLevel = mpr_pyramid->mpr_minLevel;
		*maxLevel = mpr_pyramid->mpr_maxLevel;
		return true;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

//glbBool CGlbFileRasterDataset::GetPyramidTileIndexByPoint(glbInt32 iLevel,glbDouble x,glbDouble y,glbInt32& TileColumn,glbInt32& TileRow)
//{
//	if (HasPyramid())
//	{
//		if (iLevel<mpr_pyramid->mpr_minLevel || iLevel>mpr_pyramid->mpr_maxLevel)
//		{
//			GlbSetLastError(L"该金字塔级别不存在");
//			return false;
//		}
//		glbDouble lzts = mpr_pyramid->mpr_lzts/pow(2.0, iLevel);
//		if(mpr_pyramid->mpr_isGlobe)
//		{
//			TileColumn = (glbInt32)((180.0+x)/lzts);
//			TileRow = (glbInt32)((90.0-y)/lzts);
//		}
//		else
//		{
//			TileColumn = (glbInt32)(x/lzts);
//			TileRow = (glbInt32)(y/lzts);
//		}
//		return true;
//	}
//	return false;
//}
glbBool CGlbFileRasterDataset::GetPyramidTileIndexes(glbInt32 iLevel,glbInt32& startTileColumn,glbInt32& startTileRow,
	glbInt32& endTileColumn,glbInt32& endTileRow)
{
	if (HasPyramid())
	{
		if (iLevel<mpr_pyramid->mpr_minLevel || iLevel>mpr_pyramid->mpr_maxLevel)
		{
			GlbSetLastError(L"该金字塔级别不存在");
			return false;
		}
		glbDouble minx,maxx,miny,maxy;
		CGlbExtent ext;
		GetLevelExtent(iLevel, &ext);
		ext.Get(&minx,&maxx,&miny,&maxy);
		glbDouble lzts = mpr_pyramid->mpr_lzts/pow(2.0, iLevel);
		if(mpr_pyramid->mpr_isGlobe)
		{
			if (minx == -180)
			{
				startTileColumn = 0;
			}
			else
			{
				startTileColumn = (glbInt32)(floor((180.0+minx)/lzts));
			}
			if (maxy==90)
			{
				startTileRow = 0;
			}
			else
			{
				startTileRow = (glbInt32)(floor((90.0-maxy)/lzts));
			}
			if (maxx==180)
			{
				endTileColumn = (glbInt32)((180.0+maxx)/lzts) - 1;
			}
			else
			{
				endTileColumn = (glbInt32)(ceil((180.0+maxx)/lzts))-1;
			}
			if(miny == -90)
			{
				endTileRow = (glbInt32)((90.0-miny)/lzts) - 1;
			}
			else
			{
				endTileRow = (glbInt32)(ceil((90.0-miny)/lzts))-1;
			}
		}
		else
		{
			startTileColumn = (glbInt32)floor(minx/lzts);
			startTileRow = (glbInt32)floor(miny/lzts);
			endTileColumn = (glbInt32)ceil(maxx/lzts) - 1;
			endTileRow = (glbInt32)ceil(maxy/lzts) - 1;
		}
		return true;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

glbBool CGlbFileRasterDataset::GetExtentCoverTiles( const CGlbExtent* extent, glbInt32 iLevel,glbInt32& startTileColumn, 
	glbInt32& startTileRow, glbInt32& endTileColumn, glbInt32& endTileRow )
{
	if (HasPyramid())
	{
		if (extent == NULL)
		{
			GlbSetLastError(L"参数外包不能为NULL");
			return false;
		}
		if (iLevel<mpr_pyramid->mpr_minLevel || iLevel>mpr_pyramid->mpr_maxLevel)
		{
			GlbSetLastError(L"该金字塔级别不存在");
			return false;
		}
		glbDouble minx,maxx,miny,maxy;
		extent->Get(&minx,&maxx,&miny,&maxy);
		//是否分球面
		glbDouble lzts = mpr_pyramid->mpr_lzts/pow(2.0, iLevel);
		if(mpr_pyramid->mpr_isGlobe)
		{
			if (minx == -180)
			{
				startTileColumn = 0;
			}
			else
			{
				startTileColumn = (glbInt32)(floor((180.0+minx)/lzts));
			}
			if (maxy==90)
			{
				startTileRow = 0;
			}
			else
			{
				startTileRow = (glbInt32)(floor((90.0-maxy)/lzts));
			}
			if (maxx==180)
			{
				endTileColumn = (glbInt32)((180.0+maxx)/lzts) - 1;
			}
			else
			{
				endTileColumn = (glbInt32)(ceil((180.0+maxx)/lzts))-1;
			}
			if(miny == -90)
			{
				endTileRow = (glbInt32)((90.0-miny)/lzts) - 1;
			}
			else
			{
				endTileRow = (glbInt32)(ceil((90.0-miny)/lzts))-1;
			}
		}
		else
		{
			startTileColumn = (glbInt32)(floor(minx/lzts));
			startTileRow = (glbInt32)(floor(miny/lzts));
			endTileColumn = (glbInt32)(ceil(maxx/lzts)) - 1;
			endTileRow = (glbInt32)(ceil(maxy/lzts)) - 1;
		}
		return true;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

glbBool CGlbFileRasterDataset::GetTilesCoverExtent( glbInt32 iLevel,glbInt32 startTileColumn, glbInt32 startTileRow,glbInt32 endTileColumn, 
	glbInt32 endTileRow,CGlbExtent* extent )
{
	if (HasPyramid())
	{
		if (extent == NULL)
		{
			GlbSetLastError(L"参数外包不能为NULL");
			return false;
		}
		if (iLevel<mpr_pyramid->mpr_minLevel || iLevel>mpr_pyramid->mpr_maxLevel)
		{
			GlbSetLastError(L"该金字塔级别不存在");
			return false;
		}
		glbDouble lzts = mpr_pyramid->mpr_lzts/pow(2.0, iLevel);
		glbDouble minx,maxx,miny,maxy;
		if (mpr_pyramid->mpr_isGlobe)
		{
			minx = (startTileColumn*lzts) -180.0;
			maxx = (endTileColumn+1)*lzts-180.0;
			maxy = 90-(startTileRow*lzts);
			miny = 90-(endTileRow+1)*lzts;
		}
		else
		{
			minx = startTileColumn*lzts;
			maxx = (endTileColumn+1)*lzts;
			miny = startTileRow*lzts;
			maxy = (endTileRow+1)*lzts;
		}
		extent->Set(minx,maxx,miny,maxy);
		return true;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

glbBool CGlbFileRasterDataset::GetMaxLevelExtent(CGlbExtent* extent)
{
	if (HasPyramid())
	{
		glbDouble minx,maxx,miny,maxy,lzts = mpr_pyramid->mpr_lzts/pow(2.0, mpr_pyramid->mpr_maxLevel);
		minx = mpr_pyramid->mpr_originX;
		//可以整除的
		maxx = minx + (mpr_pyramid->mpr_cols)*lzts;
		maxy = mpr_pyramid->mpr_originY;
		miny = maxy - (mpr_pyramid->mpr_rows)*lzts;

		extent->Set(minx,maxx,miny,maxy);
		return true;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

glbBool CGlbFileRasterDataset::GetLevelExtent(glbInt32 ilevel, CGlbExtent* extent)
{
	if (HasPyramid())
	{
		if (extent == NULL)
		{
			GlbSetLastError(L"参数外包不能为NULL");
			return false;
		}
		if (ilevel<mpr_pyramid->mpr_minLevel || ilevel>mpr_pyramid->mpr_maxLevel)
		{
			GlbSetLastError(L"该金字塔级别不存在");
			return false;
		}
		if (ilevel == mpr_pyramid->mpr_maxLevel)
		{
			return GetMaxLevelExtent(extent);
		}
		glbInt32 istartx,iendx,istarty,iendy;
		GetMaxLevelExtent(extent);
		GetExtentCoverTiles(extent, ilevel, istartx, iendx, istarty, iendy);
		GetTilesCoverExtent(ilevel, istartx, iendx, istarty, iendy, extent);
		return true;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

glbBool CGlbFileRasterDataset::GetPyramidBlockSize( glbInt32* blockSizeX, glbInt32* blockSizeY )
{
	if (blockSizeX == NULL || blockSizeY == NULL)
	{
		GlbSetLastError(L"传入参数不能为NULL");
		return false;
	}
	if (HasPyramid())
	{
		*blockSizeX = mpr_pyramid->mpr_tileSizeX;
		*blockSizeY = mpr_pyramid->mpr_tileSizeY;
		return true;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}
//glbInt32 columns, glbInt32 rows 是一个切片的大小
CGlbPixelBlock* CGlbFileRasterDataset::CreatePyramidPixelBlock()
{
	if (HasPyramid())
	{
		CGlbPixelBlock* pb = new CGlbPixelBlock(mpr_pyramid->mpr_pixeltype, mpr_pyramid->mpr_pixellayout);
		if (!pb->Initialize(mpr_pyramid->mpr_bandCount, mpr_pyramid->mpr_tileSizeX, mpr_pyramid->mpr_tileSizeY))
		{
			delete pb;
			return false;
		}
		return pb;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

//只能读一个切片,支持多线程
glbBool CGlbFileRasterDataset::ReadPyramidByTile( glbInt32 level, glbInt32 startTileColumn, glbInt32 startTileRow, 
	CGlbPixelBlock *pPixelBlock )
{
	if (HasPyramid())
	{
		if(mpr_pyramid->Read(startTileRow, startTileColumn, level, pPixelBlock))
		{
			//设置当前块外包
			glbDouble originX = -180.0;
			glbDouble originY = 90.0;
			if(!mpr_pyramid->mpr_isGlobe)
			{
				originX=originY = 0.0;
			}
			glbDouble level_ts = mpr_pyramid->mpr_lzts/pow(2.0, level);
			glbDouble tileymin,tileymax;
			if(mpr_pyramid->mpr_isGlobe)
			{
				tileymin = originY - (startTileRow+1)*level_ts;
				tileymax = originY - startTileRow*level_ts;
			}
			else
			{
				tileymin = startTileRow*level_ts - originY;
				tileymax = (startTileRow+1)*level_ts - originY;
			}
			glbDouble tilexmin = startTileColumn*level_ts+originX;
			glbDouble tilexmax = (startTileColumn+1)*level_ts+originX;
			CGlbExtent* ext = new CGlbExtent(tilexmin, tilexmax, tileymin, tileymax);
			pPixelBlock->SetExtent(ext);			
			return true;
		}
		else
			return false;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}
//写一个切片，如果存在,先删除，再插入，避免先查询是否存在,支持多线程。更新外包？
glbBool CGlbFileRasterDataset::WritePyramidByTile( glbInt32 level, glbInt32 startTileColumn, glbInt32 startTileRow, CGlbPixelBlock *pPixelBlock )
{
	if (HasPyramid())
	{
		if(mpr_pyramid->Write(startTileRow, startTileColumn, level, pPixelBlock))
			return true;
		else
			return false;
	}
	GlbSetLastError(L"不存在金字塔数据");
	return false;
}

void CGlbFileRasterDataset::SetSelfDealMoveExtent( CGlbExtent *extent,glbBool isMove )
{
	if(!extent)
		return;
	mpr_dealExtent = extent;
	mpr_isMove = isMove;
}

void CGlbFileRasterDataset::SetSelfDealChangeExtent( CGlbExtent *extent,glbDouble value,glbBool isChange )
{
	if(!extent)
		return;
	mpr_dealExtent = extent;
	mpr_isChange = isChange;
	mpr_changeValue = value;
}
