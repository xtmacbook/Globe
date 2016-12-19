#include "StdAfx.h"
#include "GlbVTerrainDataset.h"


CGlbVTerrainDataset::CGlbVTerrainDataset(CGlbExtent* ext)
{
	mpr_ext = ext;
	mpr_lzts= ext->GetXWidth();
	GetDemLevelExtent(0,mpr_ext.get());
}


CGlbVTerrainDataset::~CGlbVTerrainDataset(void)
{
}
glbBool CGlbVTerrainDataset::QueryInterface( const glbWChar *riid, void **ppvoid )
{
	if (riid == NULL || ppvoid == NULL)
	{
		GlbSetLastError(L"参数不正确");
		return false;
	}
	CGlbWString tmpiid = riid;
	if(tmpiid == L"IGlbTerrainDataset")
	{
		*ppvoid = (IGlbTerrainDataset*)this;
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
	GlbSetLastError(L"接口不存在");
	return false;
}
GlbDatasetTypeEnum CGlbVTerrainDataset::GetType()
{
	return GLB_DATASET_TERRAIN;
}

const glbWChar* CGlbVTerrainDataset::GetName()
{
	return L"虚拟平面地形";
}

const glbWChar* CGlbVTerrainDataset::GetAlias()
{
	return L"虚拟平面地形";
}
	
const IGlbDataSource* CGlbVTerrainDataset::GetDataSource()
{
	return NULL;
}
	
const IGlbDataset* CGlbVTerrainDataset::GetParentDataset()
{
	return NULL;
}

const GlbSubDatasetRoleEnum CGlbVTerrainDataset::GetSubRole()
{
	return GLB_SUBDATASETROLE_UNKNOWN;
}
	
glbWChar* CGlbVTerrainDataset::GetLastError()
{
	return GlbGetLastError();
}
	
const glbWChar* CGlbVTerrainDataset::GetSRS()
{
	return NULL;
}

glbBool CGlbVTerrainDataset::AlterSRS(const glbWChar* srs)
{
	GlbSetLastError(L"不支持 修改坐标系");
	return false;
}
	
const CGlbExtent* CGlbVTerrainDataset::GetExtent()
{
	return mpr_ext.get();
}
	
glbBool CGlbVTerrainDataset::HasZ()
{
	return false;
}
	
glbBool CGlbVTerrainDataset::HasM()
{
	return false;
}
	
glbBool CGlbVTerrainDataset::GetLZTS(glbDouble& lzts)
{
	lzts = mpr_lzts;
	return true;
}
	
glbBool CGlbVTerrainDataset::IsGlobe()
{
	return false;
}
	
GlbPixelTypeEnum CGlbVTerrainDataset::GetDomPixelType()
{
	return GLB_PIXELTYPE_BYTE;
}
	
GlbPixelTypeEnum CGlbVTerrainDataset::GetDemPixelType()
{
	return GLB_PIXELTYPE_FLOAT32;
}
	
GlbPixelLayoutTypeEnum CGlbVTerrainDataset::GetDomPixelLayout()
{
	return GLB_PIXELLAYOUT_BIP;
}
	
GlbPixelLayoutTypeEnum CGlbVTerrainDataset::GetDemPixelLayout()
{
	return GLB_PIXELLAYOUT_BIP;
}
	
glbBool CGlbVTerrainDataset::GetDomBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY)
{
	blockSizeX = blockSizeY = 256;
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDemBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY)
{
	blockSizeX = blockSizeY = 17;
	return true;
}
	
glbInt32 CGlbVTerrainDataset::GetDomBandCount()
{
	return 4;
}
	
glbInt32 CGlbVTerrainDataset::GetDemBandCount()
{
	return 1;
}
	
glbBool CGlbVTerrainDataset::GetDomLevels(glbInt32& maxLevel,glbInt32& minLevel)
{
	maxLevel = 23;
	minLevel = 0;
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDemLevels(glbInt32& maxLevel,glbInt32& minLevel)
{
	maxLevel = 23;
	minLevel = 0;
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDomLevelExtent(glbInt32 iLevel, CGlbExtent* ext)
{
	if (ext == NULL)
	{
		GlbSetLastError(L"ext参数为NULL");
		return false;
	}
	glbDouble lzts = mpr_lzts/pow(2.0, iLevel);
	glbInt32 istartx,iendx,istarty,iendy;
	GetDomExtentCoverTiles(mpr_ext.get(),iLevel,istartx, iendx, istarty, iendy);
	GetDomTilesCoverExtent(iLevel, istartx, iendx, istarty, iendy, ext);
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDemLevelExtent(glbInt32 iLevel, CGlbExtent* ext)
{
	if (ext == NULL)
	{
		GlbSetLastError(L"ext参数为NULL");
		return false;
	}
	glbDouble lzts = mpr_lzts/pow(2.0, iLevel);
	glbInt32 istartx,iendx,istarty,iendy;
	GetDemExtentCoverTiles(mpr_ext.get(),iLevel,istartx, iendx, istarty, iendy);
	GetDemTilesCoverExtent(iLevel, istartx, iendx, istarty, iendy, ext);
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDomTileIndexes
	(glbInt32 level, 
	 glbInt32& startTileCol,  
	 glbInt32& startTileRow, 
	 glbInt32& endTileCol,  
     glbInt32& endTileRow)
{
	glbDouble minx,maxx,miny,maxy;
	CGlbExtent ext;
	GetDomLevelExtent(level, &ext);
	ext.Get(&minx,&maxx,&miny,&maxy);
	glbDouble lzts = mpr_lzts/pow(2.0, level);
	
	startTileCol = (glbInt32)floor(minx/lzts);
	startTileRow = (glbInt32)floor(miny/lzts);
	endTileCol   = (glbInt32)ceil(maxx/lzts);
	endTileRow   = (glbInt32)ceil(maxy/lzts);
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDemTileIndexes
	(glbInt32 level, 
	 glbInt32& startTileCol,
	 glbInt32& startTileRow, 
	 glbInt32& endTileCol,  
	 glbInt32& endTileRow)
{
	glbDouble minx,maxx,miny,maxy;
	CGlbExtent ext;
	GetDemLevelExtent(level, &ext);
	ext.Get(&minx,&maxx,&miny,&maxy);
	glbDouble lzts = mpr_lzts/pow(2.0, level);
	
	startTileCol = (glbInt32)floor(minx/lzts);
	startTileRow = (glbInt32)floor(miny/lzts);
	endTileCol   = (glbInt32)ceil(maxx/lzts);
	endTileRow   = (glbInt32)ceil(maxy/lzts);
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDomExtentCoverTiles
	(const CGlbExtent* extent, 
	 glbInt32  level,
	 glbInt32& startTileCol,
	 glbInt32& startTileRow,
	 glbInt32& endTileCol,
	 glbInt32& endTileRow)
{
	glbDouble minx,maxx,miny,maxy;
	extent->Get(&minx,&maxx,&miny,&maxy);
	glbDouble lzts = mpr_lzts/pow(2.0, level);
	startTileCol = (glbInt32)(floor(minx/lzts));
	startTileRow = (glbInt32)(floor(miny/lzts));
	endTileCol   = (glbInt32)(ceil(maxx/lzts)) - 1;
	endTileRow   = (glbInt32)(ceil(maxy/lzts)) - 1;
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDemExtentCoverTiles
	(const CGlbExtent* extent, 
	 glbInt32  level,
	 glbInt32& startTileCol, 
	 glbInt32& startTileRow,
	 glbInt32& endTileCol, 
	 glbInt32& endTileRow)
{
	glbDouble minx,maxx,miny,maxy;
	extent->Get(&minx,&maxx,&miny,&maxy);
	glbDouble lzts = mpr_lzts/pow(2.0, level);
	startTileCol = (glbInt32)(floor(minx/lzts));
	startTileRow = (glbInt32)(floor(miny/lzts));
	endTileCol   = (glbInt32)(ceil(maxx/lzts)) - 1;
	endTileRow   = (glbInt32)(ceil(maxy/lzts)) - 1;
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDomTilesCoverExtent
	(glbInt32 level,
	 glbInt32 startTileCol, 
	 glbInt32 startTileRow,
	 glbInt32 endTileCol, 
	 glbInt32 endTileRow, 
	 CGlbExtent* extent)
{
	if (extent == NULL)
	{
		GlbSetLastError(L"参数extent无效");
		return false;
	}
	glbDouble lzts = mpr_lzts/pow(2.0, level);
	glbDouble minx,maxx,miny,maxy;
	
	minx = startTileCol*lzts;
	maxx = (endTileCol+1)*lzts;
	miny = startTileRow*lzts;
	maxy = (endTileRow+1)*lzts;
	
	extent->Set(minx,maxx,miny,maxy);
	return true;
}
	
glbBool CGlbVTerrainDataset::GetDemTilesCoverExtent
	(glbInt32 level,
	 glbInt32 startTileCol, 
	 glbInt32 startTileRow,
	 glbInt32 endTileCol, 
	 glbInt32 endTileRow, 
	 CGlbExtent* extent)
{
	if (extent == NULL)
	{
		GlbSetLastError(L"参数extent无效");
		return false;
	}
	glbDouble lzts = mpr_lzts/pow(2.0, level);
	glbDouble minx,maxx,miny,maxy;
	
	minx = startTileCol*lzts;
	maxx = (endTileCol+1)*lzts;
	miny = startTileRow*lzts;
	maxy = (endTileRow+1)*lzts;
	
	extent->Set(minx,maxx,miny,maxy);
	return true;
}

CGlbPixelBlock* CGlbVTerrainDataset::CreateDemPixelBlock()
{
	CGlbPixelBlock* pblock = NULL;
	pblock = new CGlbPixelBlock(GLB_PIXELTYPE_FLOAT32, GLB_PIXELLAYOUT_BIP);
	if (pblock->Initialize(1, 17, 17) == false)
	{
		delete pblock;
		return false;
	}
	return pblock;
}
	
CGlbPixelBlock* CGlbVTerrainDataset::CreateDomPixelBlock()
{
	CGlbPixelBlock* pblock = NULL;
	pblock = new CGlbPixelBlock(GLB_PIXELTYPE_BYTE, GLB_PIXELLAYOUT_BIP);
	if (pblock->Initialize(4, 256, 256) == false)
	{
		delete pblock;
		return false;
	}
	return pblock;
}
	
glbBool CGlbVTerrainDataset::ReadDom(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock)
{
	if(pixelBlock == NULL || pixelBlock->IsValid()==false)
	{
		GlbSetLastError(L"参数pixelBlock无效");
		return false;
	}
	glbInt32 rows,cols;
	pixelBlock->GetSize(cols,rows);
	if (pixelBlock->GetPixelType()   != GLB_PIXELTYPE_BYTE ||
		pixelBlock->GetPixelLayout() != GLB_PIXELLAYOUT_BIP ||
		pixelBlock->GetBandCount()   != 4 ||
		rows != 256 || cols != 256)
	{
		GlbSetLastError(L"参数pixelBlock不匹配,请用CreateDomPixelBlock创建");
		return false;
	}
	glbByte* pdata = NULL;
	glbByte* pmask = NULL;
	pixelBlock->GetPixelData(&pdata);	
	pixelBlock->GetMaskData(&pmask);
	for(glbInt32 row =0;row<256;row++)
	{
		for(glbInt32 col =0;col<256;col++)
		{
			*(pdata + 4*(row*256 + col))   = 198;
			*(pdata + 4*(row*256 + col)+1) = 198;
			*(pdata + 4*(row*256 + col)+2) = 198;
			*(pdata + 4*(row*256 + col)+3) = 255;
		}
	}
	memset(pmask,255,256*256);

	glbDouble level_ts = mpr_lzts/pow(2.0, level);
	glbDouble tileymin,tileymax;
	tileymin = tileRow*level_ts;
	tileymax = (tileRow+1)*level_ts;
	
	glbDouble tilexmin = tileCol*level_ts;
	glbDouble tilexmax = (tileCol+1)*level_ts;
	CGlbExtent* ext = new CGlbExtent(tilexmin, tilexmax, tileymin, tileymax);
	pixelBlock->SetExtent(ext);
	return true;
}

glbBool CGlbVTerrainDataset::ReadDem(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock)
{
	if(pixelBlock == NULL || pixelBlock->IsValid()==false)
	{
		GlbSetLastError(L"参数pixelBlock无效");
		return false;
	}
	glbInt32 rows,cols;
	pixelBlock->GetSize(cols,rows);
	if (pixelBlock->GetPixelType()   != GLB_PIXELTYPE_FLOAT32 ||
		pixelBlock->GetPixelLayout() != GLB_PIXELLAYOUT_BIP ||
		pixelBlock->GetBandCount()   != 1 ||
		rows != 17 || cols != 17)
	{
		GlbSetLastError(L"参数pixelBlock不匹配,请用CreateDemPixelBlock创建");
		return false;
	}
	glbFloat* pdata = NULL;
	glbByte* pmask = NULL;
	pixelBlock->GetPixelData((glbByte**)&pdata);	
	pixelBlock->GetMaskData(&pmask);
	
	srand((int)time(0));
	/*glbDouble extW = mpr_ext->GetXWidth();
	glbDouble */
	for(glbInt32 row =0;row<16;row++)
	{
		for(glbInt32 col =0;col<16;col++)
		{
			if(row == 0||row==16||col==0||col==16)
				*(pdata+row*17+col)=0.0;
			else
				*(pdata+row*17+col) = 0.0;//(glbFloat)(rand()%20);
		}
	}
	memset(pmask,255,17*17);

	glbDouble level_ts = mpr_lzts/pow(2.0, level);
	glbDouble tileymin,tileymax;
	tileymin = tileRow*level_ts;
	tileymax = (tileRow+1)*level_ts;
	
	glbDouble tilexmin = tileCol*level_ts;
	glbDouble tilexmax = (tileCol+1)*level_ts;
	CGlbExtent* ext = new CGlbExtent(tilexmin, tilexmax, tileymin, tileymax,0,20);
	pixelBlock->SetExtent(ext);
	return true;
}

glbBool CGlbVTerrainDataset::ReadDem(glbDouble lonOrX,glbDouble latOrY, CGlbPixelBlock* pixelBlock)
{
	return this->ReadDem(0,1,1,pixelBlock);
}
	
glbBool CGlbVTerrainDataset::WriteDom
	(glbInt32 level,
	 glbInt32 tileCol, 
	 glbInt32 tileRow, 
	 GlbTileNeighbor& neighbors, 
	 CGlbPixelBlock* pixelBlock)
{
	GlbSetLastError(L"不支持");
	return false;
}
	
glbBool CGlbVTerrainDataset::WriteDem
	(glbInt32 level, 
	 glbInt32 tileCol,
	 glbInt32 tileRow, 
	 GlbTileNeighbor& neighbors,
	 CGlbPixelBlock*  pixelBlock)
{
	GlbSetLastError(L"不支持");
	return false;
}

glbBool CGlbVTerrainDataset::MergerGlobeDem()
{
	GlbSetLastError(L"不支持");
	return false;
}

glbBool CGlbVTerrainDataset::ImportDomDataset
	(IGlbRasterDataset* dataset,
	 glbInt32 minLevel,
	 glbInt32 maxLevel,
	 IGlbProgress* progress/*=NULL*/)
{
	GlbSetLastError(L"不支持");
	return false;
}
	
glbBool CGlbVTerrainDataset::ImportDemDataset
	(IGlbRasterDataset* dataset,
	 glbInt32 minLevel,
	 glbInt32 maxLevel,
	 IGlbProgress* progress/*=NULL*/)
{
	GlbSetLastError(L"不支持");
	return false;
}