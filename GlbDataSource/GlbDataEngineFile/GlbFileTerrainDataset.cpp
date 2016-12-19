#include "stdafx.h"
#include "GlbFileTerrainDataset.h"
#include "GlbError.h"
#include "GlbPath.h"
#include "GlbFileDataSource.h"
#include "GlbFileRasterDataset.h"
#include "GlbSQLitePyramid.h"

#define DOMTILESZIE 256
#define DEMTILESZIE 17
CGlbFileTerrainDataset::CGlbFileTerrainDataset(void)
{
	mpr_ds     = NULL;
	mpr_ted    = NULL;
	mpr_name   = L"";
	mpr_alias  = L"";
	mpr_extent = NULL;
}


CGlbFileTerrainDataset::~CGlbFileTerrainDataset(void)
{
	mpr_ds     = NULL;
	mpr_ted    = NULL;
	mpr_name   = L"";
	mpr_alias  = L"";
	mpr_extent = NULL;
}

glbBool CGlbFileTerrainDataset::Initialize( CGlbFileDataSource* ds, const glbWChar* name, const glbWChar* alias, CGlbTedSQLite* ted)
{
	if (ds == NULL || name == NULL || ted == NULL)
	{
		GlbSetLastError(L"��������ȷ");
		return false;
	}
	mpr_ds    = ds;
	mpr_name  = name;
	mpr_alias = (alias==NULL)?L"":alias;
	mpr_ted   = ted;
	return true;
}

glbBool CGlbFileTerrainDataset::QueryInterface( const glbWChar *riid, void **ppvoid )
{
	if (riid == NULL || ppvoid == NULL)
	{
		GlbSetLastError(L"��������ȷ");
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
	GlbSetLastError(L"�ӿڲ�����");
	return false;
}
GlbDatasetTypeEnum CGlbFileTerrainDataset::GetType()
{ 
	return GLB_DATASET_TERRAIN; 
}
const glbWChar* CGlbFileTerrainDataset::GetName()
{ 
	return mpr_name.c_str(); 
}
const glbWChar* CGlbFileTerrainDataset::GetAlias()
{ 
	return mpr_alias.c_str();
}
const IGlbDataSource* CGlbFileTerrainDataset::GetDataSource()
{ 
	return mpr_ds.get();
}
const IGlbDataset* CGlbFileTerrainDataset::GetParentDataset()
{ 
	return NULL; 
}
const GlbSubDatasetRoleEnum CGlbFileTerrainDataset::GetSubRole()
{
	return GLB_SUBDATASETROLE_UNKNOWN;
}
glbWChar* CGlbFileTerrainDataset::GetLastError()
{
	return GlbGetLastError();
}
const glbWChar* CGlbFileTerrainDataset::GetSRS()
{
	if (mpr_ted != NULL)
	{
		return mpr_ted->mpr_srs.c_str();
	}
	GlbSetLastError(L"�������ݼ�������");
	return false;
}

glbBool CGlbFileTerrainDataset::AlterSRS( const glbWChar* srs )
{
	GlbSetLastError(L"��֧��");
	return false;
}

const CGlbExtent* CGlbFileTerrainDataset::GetExtent()
{
	if(mpr_ted == NULL)
		return NULL;
	
	if(mpr_extent == NULL)
	{
		mpr_extent = new CGlbExtent();		
		if(mpr_ted->mpr_domMaxLevel>mpr_ted->mpr_domMinLevel)
		{
			GetDomLevelExtent(mpr_ted->mpr_domMinLevel,mpr_extent.get());
		}else if(mpr_ted->mpr_demMaxLevel >= mpr_ted->mpr_demMinLevel){
			GetDemLevelExtent(mpr_ted->mpr_demMinLevel,mpr_extent.get());
		}
	}	
	return mpr_extent.get();
}
glbBool CGlbFileTerrainDataset::HasZ()
{
	return false;
}
glbBool CGlbFileTerrainDataset::HasM()
{
	return false;
}
glbBool CGlbFileTerrainDataset::GetLZTS(glbDouble& lzts)
{
	if (mpr_ted != NULL)
	{
		lzts = mpr_ted->mpr_lzts;
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::IsGlobe()
{
	if (mpr_ted != NULL)
	{
		return mpr_ted->mpr_isGlobe;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}
GlbPixelTypeEnum CGlbFileTerrainDataset::GetDomPixelType()
{
	return GLB_PIXELTYPE_BYTE;
}
GlbPixelTypeEnum CGlbFileTerrainDataset::GetDemPixelType()
{
	return GLB_PIXELTYPE_FLOAT32;
}
GlbPixelLayoutTypeEnum CGlbFileTerrainDataset::GetDomPixelLayout()
{
	return GLB_PIXELLAYOUT_BIP;
}
GlbPixelLayoutTypeEnum CGlbFileTerrainDataset::GetDemPixelLayout()
{
	return GLB_PIXELLAYOUT_BIP;
}
glbBool CGlbFileTerrainDataset::GetDomBlockSize( glbInt32& blockSizeX, glbInt32& blockSizeY )
{
	if (mpr_ted != NULL)
	{
		blockSizeX = DOMTILESZIE;
		blockSizeY = DOMTILESZIE;
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDemBlockSize( glbInt32& blockSizeX, glbInt32& blockSizeY )
{
	if (mpr_ted != NULL)
	{
		blockSizeX = DEMTILESZIE;
		blockSizeY = DEMTILESZIE;
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}
glbInt32 CGlbFileTerrainDataset::GetDomBandCount()
{
	return 4;
}
glbInt32 CGlbFileTerrainDataset::GetDemBandCount()
{
	return 1;
}
glbBool CGlbFileTerrainDataset::GetDomLevels( glbInt32& maxLevel,glbInt32& minLevel )
{
	if (mpr_ted != NULL)
	{
		maxLevel = mpr_ted->mpr_domMaxLevel;
		minLevel = mpr_ted->mpr_domMinLevel;
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDemLevels( glbInt32& maxLevel,glbInt32& minLevel )
{
	if (mpr_ted != NULL)
	{
		maxLevel = mpr_ted->mpr_demMaxLevel;
		minLevel = mpr_ted->mpr_demMinLevel;
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDomMaxLevelExtent(CGlbExtent* extent)
{
	if (extent == NULL)
	{
		GlbSetLastError(L"����ΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		return GetDomLevelExtent(mpr_ted->mpr_domMaxLevel, extent);
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDemMaxLevelExtent(CGlbExtent* extent)
{
	if (extent == NULL)
	{
		GlbSetLastError(L"����ΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		return GetDemLevelExtent(mpr_ted->mpr_demMaxLevel, extent);
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDomLevelExtent( glbInt32 iLevel, CGlbExtent* ext )
{
	if (ext == NULL)
	{
		GlbSetLastError(L"ext����ΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		if (iLevel<mpr_ted->mpr_domMinLevel || iLevel>mpr_ted->mpr_domMaxLevel)
		{
			GlbSetLastError(L"���𲻴���");
			return false;
		}
		glbDouble lzts = mpr_ted->mpr_lzts/pow(2.0, iLevel);
		map<glbInt32/*level*/,GlbLevelInfo*>::iterator itr = mpr_ted->mpr_domLevelInfo.find(iLevel);
		if (itr != mpr_ted->mpr_domLevelInfo.end())
		{
			ext->Set(itr->second->OriginX, 
				     itr->second->OriginX + lzts*(itr->second->Cols),
				     itr->second->OriginY - lzts*(itr->second->Rows), 
					 itr->second->OriginY);
		}
		else
		{
			GlbSetLastError(L"���𲻴���");
			return false;
		}
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDemLevelExtent( glbInt32 iLevel, CGlbExtent* ext )
{
	if (ext == NULL)
	{
		GlbSetLastError(L"ext����ΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		if (iLevel<mpr_ted->mpr_demMinLevel || iLevel>mpr_ted->mpr_demMaxLevel)
		{
			GlbSetLastError(L"���𲻴���");
			return false;
		}
		glbDouble lzts = mpr_ted->mpr_lzts/pow(2.0, iLevel);
		map<glbInt32/*level*/,GlbLevelInfo*>::iterator itr = mpr_ted->mpr_demLevelInfo.find(iLevel);
		if (itr != mpr_ted->mpr_demLevelInfo.end())
		{
			ext->Set(itr->second->OriginX, 
				     itr->second->OriginX + lzts*(itr->second->Cols),
				     itr->second->OriginY - lzts*(itr->second->Rows), 
					 itr->second->OriginY);
		}
		else
		{
			GlbSetLastError(L"���𲻴���");
			return false;
		}
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDomTileIndexes
	    ( glbInt32  iLevel, 
	      glbInt32& startTileColumn, 
		  glbInt32& startTileRow, 
		  glbInt32& endTileColumn, 
		  glbInt32& endTileRow )
{
	if (mpr_ted != NULL)
	{
		if (iLevel<mpr_ted->mpr_domMinLevel || iLevel>mpr_ted->mpr_domMaxLevel)
		{
			GlbSetLastError(L"���𲻴���");
			return false;
		}
		glbDouble minx,maxx,miny,maxy;
		CGlbExtent ext;
		GetDomLevelExtent(iLevel, &ext);
		ext.Get(&minx,&maxx,&miny,&maxy);
		return GetDomExtentCoverTiles(&ext,
			                           iLevel,
									   startTileColumn,
									   startTileRow,
									   endTileColumn,
									   endTileRow);
	}	
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDemTileIndexes
	    ( glbInt32  iLevel, 
		  glbInt32& startTileColumn, 
		  glbInt32& startTileRow, 
		  glbInt32& endTileColumn, 
		  glbInt32& endTileRow )
{
	if (mpr_ted != NULL)
	{
		if (iLevel<mpr_ted->mpr_demMinLevel || iLevel>mpr_ted->mpr_demMaxLevel)
		{
			GlbSetLastError(L"���𲻴���");
			return false;
		}
		glbDouble minx,maxx,miny,maxy;
		CGlbExtent ext;
		GetDemLevelExtent(iLevel, &ext);
		ext.Get(&minx,&maxx,&miny,&maxy);
		return GetDemExtentCoverTiles(&ext,
			                           iLevel,
									   startTileColumn,
									   startTileRow,
									   endTileColumn,
									   endTileRow);
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDomExtentCoverTiles
	    ( const CGlbExtent* extent, 
		  glbInt32          iLevel,
		  glbInt32&         startTileColumn, 
		  glbInt32&         startTileRow, 
		  glbInt32&         endTileColumn, 
		  glbInt32&         endTileRow )
{
	if (extent == NULL)
	{
		GlbSetLastError(L"����ΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		glbDouble minx,maxx,miny,maxy;
		extent->Get(&minx,&maxx,&miny,&maxy);
		glbDouble lzts = mpr_ted->mpr_lzts/pow(2.0, iLevel);
		if(mpr_ted->mpr_isGlobe)
		{
			startTileColumn = (glbInt32)floor((180.0+minx)/lzts);			
			endTileColumn   = (glbInt32)ceil(( 180.0+maxx)/lzts)-1;
			startTileRow    = (glbInt32)floor((90.0-maxy)/lzts);
			endTileRow      = (glbInt32)ceil(( 90.0-miny)/lzts)-1;
		}
		else
		{
			/*(0,0) Ϊ�ο�ԭ��*/
			startTileColumn = (glbInt32)floor((minx/lzts));			
			endTileColumn   = (glbInt32)(ceil(maxx/lzts)) - 1 ;
			startTileRow    = (glbInt32)floor((miny/lzts));
			endTileRow      = (glbInt32)(ceil(maxy/lzts)) - 1 ;
		}
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDemExtentCoverTiles
	    ( const CGlbExtent* extent, 
		  glbInt32          iLevel,
		  glbInt32&         startTileColumn, 
		  glbInt32&         startTileRow, 
		  glbInt32&         endTileColumn, 
		  glbInt32&         endTileRow )
{
	if (extent == NULL)
	{
		GlbSetLastError(L"����ΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		glbDouble minx,maxx,miny,maxy;
		extent->Get(&minx,&maxx,&miny,&maxy);
		//�Ƿ������
		glbDouble lzts = mpr_ted->mpr_lzts/pow(2.0, iLevel);
		if(mpr_ted->mpr_isGlobe)
		{
			startTileColumn = (glbInt32)floor((180.0+minx)/lzts);			
			endTileColumn   = (glbInt32)ceil(( 180.0+maxx)/lzts)-1;
			startTileRow    = (glbInt32)floor((90.0-maxy)/lzts);
			endTileRow      = (glbInt32)ceil(( 90.0-miny)/lzts)-1;
		}
		else
		{
			/*(0,0) Ϊ�ο�ԭ��*/
			startTileColumn = (glbInt32)floor(minx/lzts);
			startTileRow    = (glbInt32)floor(miny/lzts);
			endTileColumn   = (glbInt32)(ceil(maxx/lzts)) -1;
			endTileRow      = (glbInt32)(ceil(maxy/lzts)) -1;
		}
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDomTilesCoverExtent
	    (glbInt32    iLevel,
		 glbInt32    startTileColumn, 
		 glbInt32    startTileRow, 
		 glbInt32    endTileColumn, 
		 glbInt32    endTileRow,
		 CGlbExtent* extent)
{
	if (extent == NULL)
	{
		GlbSetLastError(L"����ΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		if (iLevel<mpr_ted->mpr_domMinLevel || iLevel>mpr_ted->mpr_domMaxLevel)
		{
			GlbSetLastError(L"���𲻴���");
			return false;
		}
		glbDouble lzts = mpr_ted->mpr_lzts/pow(2.0, iLevel);
		glbDouble minx,maxx,miny,maxy;
		if (mpr_ted->mpr_isGlobe)
		{
			minx = (startTileColumn *lzts) -180.0;
			maxx = (endTileColumn+1)*lzts  -180.0;
			maxy = 90-(startTileRow *lzts);
			miny = 90-(endTileRow+1)*lzts;
		}
		else
		{
			minx = startTileColumn  *lzts;
			maxx = (endTileColumn+1)*lzts;
			miny = startTileRow  *lzts;
			maxy = (endTileRow+1)*lzts;
		}
		extent->Set(minx,maxx,miny,maxy);
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

glbBool CGlbFileTerrainDataset::GetDemTilesCoverExtent
	    (glbInt32    iLevel,
		 glbInt32    startTileColumn,
		 glbInt32    startTileRow,
		 glbInt32    endTileColumn,
		 glbInt32    endTileRow,
		 CGlbExtent* extent)
{
	if (extent == NULL)
	{
		GlbSetLastError(L"����ΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		if (iLevel<mpr_ted->mpr_demMinLevel || iLevel>mpr_ted->mpr_demMaxLevel)
		{
			GlbSetLastError(L"���𲻴���");
			return false;
		}
		glbDouble lzts = mpr_ted->mpr_lzts/pow(2.0, iLevel);
		glbDouble minx,maxx,miny,maxy;
		if (mpr_ted->mpr_isGlobe)
		{
			minx = (startTileColumn *lzts) -180.0;
			maxx = (endTileColumn+1)*lzts  -180.0;
			maxy = 90-(startTileRow *lzts);
			miny = 90-(endTileRow+1)*lzts;
		}
		else
		{
			minx = startTileColumn  *lzts;
			maxx = (endTileColumn+1)*lzts;
			miny = startTileRow  *lzts;
			maxy = (endTileRow+1)*lzts;
		}
		extent->Set(minx,maxx,miny,maxy);
		return true;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return false;
}

CGlbPixelBlock* CGlbFileTerrainDataset::CreateDomPixelBlock()
{
	if (mpr_ted != NULL)
	{
		CGlbPixelBlock* pb = new CGlbPixelBlock(GLB_PIXELTYPE_BYTE, GLB_PIXELLAYOUT_BIP);
		if (!pb->Initialize(4, DOMTILESZIE,DOMTILESZIE))
		{
			delete pb;
			return NULL;
		}
		return pb;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return NULL;
}

CGlbPixelBlock* CGlbFileTerrainDataset::CreateDemPixelBlock()
{
	if (mpr_ted != NULL)
	{
		CGlbPixelBlock* pb = new CGlbPixelBlock(GLB_PIXELTYPE_FLOAT32, GLB_PIXELLAYOUT_BIP);
		if (!pb->Initialize(1, DEMTILESZIE,DEMTILESZIE))
		{
			delete pb;
			return NULL;
		}
		return pb;
	}
	GlbSetLastError(L"û�г�ʼ��");
	return NULL;
}

glbBool CGlbFileTerrainDataset::ReadDom( glbInt32 level, glbInt32 startColumn,glbInt32 startRow, CGlbPixelBlock* pixelBlock )
{
	if (pixelBlock == NULL)
	{
		GlbSetLastError(L"����pixelBlockΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		glbInt32 rows,cols;
		pixelBlock->GetSize(cols,rows);
		if (pixelBlock->GetPixelType()   != GLB_PIXELTYPE_BYTE ||
			pixelBlock->GetPixelLayout() != GLB_PIXELLAYOUT_BIP ||
			pixelBlock->GetBandCount()   != 4 ||
			rows != DOMTILESZIE || cols != DOMTILESZIE)
		{
			GlbSetLastError(L"����pixelBlock��ƥ��,����CreateDomPixelBlock����");
			return false;
		}
		if(!mpr_ted->ReadDom(startRow, startColumn, level, pixelBlock)) 
			return false;
		//�������ؿ�������Χ
		glbDouble originX = -180.0;
		glbDouble originY = 90.0;
		if(!mpr_ted->mpr_isGlobe)
		{
			originX=originY = 0.0;
		}
		glbDouble level_ts = mpr_ted->mpr_lzts/pow(2.0, level);
		glbDouble tileymin,tileymax;
		if(mpr_ted->mpr_isGlobe)
		{
			tileymin = originY - (startRow+1)*level_ts;
			tileymax = originY - startRow*level_ts;
		}
		else
		{
			tileymin = startRow*level_ts - originY;
			tileymax = (startRow+1)*level_ts - originY;
		}
		glbDouble tilexmin = startColumn*level_ts+originX;
		glbDouble tilexmax = (startColumn+1)*level_ts+originX;
		CGlbExtent* ext = new CGlbExtent(tilexmin, tilexmax, tileymin, tileymax);
		pixelBlock->SetExtent(ext);
		return true;
	}
	GlbSetLastError(L"�������ݼ�������");
	return false;
}

glbBool CGlbFileTerrainDataset::ReadDem( glbInt32 level, glbInt32 startColumn,glbInt32 startRow, CGlbPixelBlock* pixelBlock )
{
	if (pixelBlock == NULL)
	{
		GlbSetLastError(L"�������ΪNULL");
		return false;
	}
	if (mpr_ted != NULL)
	{
		glbInt32 rows,cols;
		pixelBlock->GetSize(cols,rows);
		if (pixelBlock->GetPixelType() != GLB_PIXELTYPE_FLOAT32 ||
			//pixelBlock->GetPixelLayout() != GLB_PIXELLAYOUT_BIP ||
			pixelBlock->GetBandCount() != 1 ||
			rows != DEMTILESZIE || cols != DEMTILESZIE)
		{
			GlbSetLastError(L"���ؿ����ظ�ʽ�����ݼ������ظ�ʽ����");
			return false;
		}
		if(!mpr_ted->ReadDem(startRow, startColumn, level, pixelBlock)) 
			return false;
		//if(level == 2 && startRow == 2 && startColumn == -3)
		//{
		//	CGlbWString ssp = CGlbWString::FormatString(L"D:\\testDem.txt");			
		//	pixelBlock->ExportToText(ssp.c_str());
		//}
		//�������ؿ�������Χ
		glbDouble originX = -180.0;
		glbDouble originY = 90.0;
		if(!mpr_ted->mpr_isGlobe)
		{
			originX=originY = 0.0;
		}
		glbDouble level_ts = mpr_ted->mpr_lzts/pow(2.0, level);
		glbDouble tileymin,tileymax;
		if(mpr_ted->mpr_isGlobe)
		{
			tileymin = originY - (startRow+1)*level_ts;
			tileymax = originY - startRow*level_ts;
		}
		else
		{
			tileymin = startRow*level_ts - originY;
			tileymax = (startRow+1)*level_ts - originY;
		}
		glbDouble tilexmin = startColumn*level_ts+originX;
		glbDouble tilexmax = (startColumn+1)*level_ts+originX;
		CGlbExtent* ext = new CGlbExtent(tilexmin, tilexmax, tileymin, tileymax);
		pixelBlock->SetExtent(ext);
		return true;
	}
	GlbSetLastError(L"�������ݼ�������");
	return false;
}

glbBool CGlbFileTerrainDataset::ReadDem(glbDouble lonOrX,glbDouble latOrY, CGlbPixelBlock* pixelBlock)
{
	if (pixelBlock == NULL)
	{
		GlbSetLastError(L"�������ΪNULL");
		return false;
	}
	if (mpr_ted == NULL)
	{
		GlbSetLastError(L"���ݼ���ʼ��ʧ��");
		return false;
	}
	
	glbInt32 rows,cols;
	pixelBlock->GetSize(cols,rows);
	if (pixelBlock->GetPixelType() != GLB_PIXELTYPE_FLOAT32 ||	
		pixelBlock->GetBandCount() != 1 ||
		rows != DEMTILESZIE || cols != DEMTILESZIE)
	{
			GlbSetLastError(L"���ؿ����ظ�ʽ�����ݼ������ظ�ʽ����");
			return false;
	}
	glbInt32  level = 0;
	glbDouble lzts = 0;
	glbBool   rdOk = false;
	glbInt32  col,row;
	for (level = mpr_ted->mpr_demMaxLevel ; level>=mpr_ted->mpr_demMaxLevel;level--)
	{
		lzts = mpr_ted->mpr_lzts/pow(2.0, level);		
		if(mpr_ted->mpr_isGlobe)
		{
			col = (glbInt32)floor((180.0+lonOrX)/lzts);
			row = (glbInt32)floor((90.0-latOrY)/lzts);
		}
		else
		{
			col = (glbInt32)floor(lonOrX/lzts);
			row = (glbInt32)floor(latOrY/lzts);			
		}
		if(mpr_ted->ReadDem(row, col, level, pixelBlock) == true)
		{
			rdOk = true;
			break;
		}
	}
	//�������ؿ�������Χ
	glbDouble originX = -180.0;
	glbDouble originY = 90.0;
	if(!mpr_ted->mpr_isGlobe)
	{
		originX=originY = 0.0;
	}
	glbDouble tileymin,tileymax;
	if(mpr_ted->mpr_isGlobe)
	{
		tileymin = originY - (row+1)*lzts;
		tileymax = originY - row    *lzts;
	}
	else
	{
		tileymin = row    *lzts - originY;
		tileymax = (row+1)*lzts - originY;
	}
	glbDouble tilexmin = col    *lzts + originX;
	glbDouble tilexmax = (col+1)*lzts + originX;
	CGlbExtent* ext = new CGlbExtent(tilexmin, tilexmax, tileymin, tileymax);
	pixelBlock->SetExtent(ext);
	return true;
}
glbBool CGlbFileTerrainDataset::WriteDom
	    (glbInt32        level,
		 glbInt32        tileCol,
		 glbInt32        tileRow,
		 GlbTileNeighbor& neighbors,
		 CGlbPixelBlock* pixelBlock )
{
	if(mpr_ted == NULL)
	{
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
	if (pixelBlock == NULL)
	{
		GlbSetLastError(L"����pixelBlockΪNULL");
		return false;
	}
	if(level <0)
	{
		GlbSetLastError(L"����levelС��0");
		return false;
	}
	if (mpr_ted->mpr_isGlobe)
	{
	     if(tileCol <0)
		 {
			 GlbSetLastError(L"����tileColС��0");
			return false;
		 }
		 if(tileRow <0)
		 {
			 GlbSetLastError(L"����tileRowС��0");
			return false;
		 }
	}
		
	glbInt32 rows,cols;
	pixelBlock->GetSize(cols,rows);
	if (pixelBlock->GetPixelType()   != GLB_PIXELTYPE_BYTE  ||
		pixelBlock->GetPixelLayout() != GLB_PIXELLAYOUT_BIP ||
		pixelBlock->GetBandCount()   != 4                   ||
		rows != DOMTILESZIE || cols != DOMTILESZIE)
	{
		GlbSetLastError(L"pixelBlock ��ƥ��");
		return false;
	}
				
	glbref_ptr<CGlbPixelBlock> oldBlock = NULL;	
	//if(neighbors.hasLeft||neighbors.hasRight||neighbors.hasUp||neighbors.hasDown)
	if(mpr_ted->mpr_isGlobe)
	{//�Ǳ߽��
		oldBlock = new CGlbPixelBlock(GLB_PIXELTYPE_BYTE, GLB_PIXELLAYOUT_BIP);
		if(!oldBlock->Initialize(4, DOMTILESZIE, DOMTILESZIE))
		{
			GlbSetLastError(L"�ڴ治��");
			return false;
		}
		if (ReadDom(level, tileCol, tileRow, oldBlock.get())==false)
		{//�����ݲ�����,�߽����Ҫ�Ӹ���һ������
			glbref_ptr<CGlbPixelBlock> upper = new CGlbPixelBlock(GLB_PIXELTYPE_BYTE, GLB_PIXELLAYOUT_BIP);
			if(!upper->Initialize(4, DOMTILESZIE, DOMTILESZIE))
			{
				GlbSetLastError(L"�ڴ治��");
				return false;
			}
			if(ReadDom(level-1,tileCol/2,tileRow/2,upper.get()) == false)// || tileCol < 0
			{
				oldBlock = NULL;
			}else{
				glbref_ptr<CGlbPixelBlock> newPixelblock = new CGlbPixelBlock(GLB_PIXELTYPE_BYTE,GLB_PIXELLAYOUT_BIP);//ResizePixelDataByBilinear(upper.get(),2);				
				if(!newPixelblock->Initialize(4, DOMTILESZIE*2, DOMTILESZIE*2))
				{
					GlbSetLastError(L"�ڴ治��");
					return false;
				}
				if(!GlbUpsample_Bilinear(upper.get(),newPixelblock.get()))
				{
					oldBlock = NULL;
				}else{
					/*��ֵ�õ�����������*/
					glbInt32 up_colIdx = tileCol%2 *(DOMTILESZIE);
					glbInt32 up_rowIdx = 0;
					if(IsGlobe())
						up_rowIdx = tileRow%2 *(DOMTILESZIE);
					else
						up_rowIdx = 256 - tileRow%2 *(DOMTILESZIE);
					glbInt32 up_col    = 0;
					glbByte* oldData   = NULL;
					glbByte* oldMask   = NULL;
					glbByte* newData   = NULL;
					glbByte* newMask   = NULL;
					oldBlock->GetPixelData(&oldData);
					oldBlock->GetMaskData(&oldMask);
					newPixelblock->GetPixelData(&newData);
					newPixelblock->GetMaskData(&newMask);
					for(glbInt32 i=0;i<DOMTILESZIE;i++)
					{
						up_col = up_colIdx;
						for(glbInt32 j=0;j<DOMTILESZIE;j++)
						{
							memcpy(oldData+(i*DOMTILESZIE+j)*4, newData+(up_rowIdx*DOMTILESZIE*2+up_col)*4, 4);
							*(oldMask+i*DOMTILESZIE+j) = *(newMask+up_rowIdx*DOMTILESZIE*2+up_col);
							up_col++;
						}
						up_rowIdx++;
					}					
				}
			}//�ϼ���
		}	
		if(oldBlock)//(0)
		{//�ϲ�����
			glbByte* pdata = NULL;
			glbByte* pmask = NULL;
			pixelBlock->GetPixelData(&pdata);
			glbByte* oldmask = NULL;
			glbByte* olddata = NULL;
			oldBlock->GetPixelData(&olddata);
			for (glbInt32 i=0;i<DOMTILESZIE;i++)
			{
				for (glbInt32 j=0;j<DOMTILESZIE;j++)
				{										
					pixelBlock->GetMaskVal(j,i,&pmask);										
					oldBlock->GetMaskVal(j,i,&oldmask);	
					if ((*pmask == 0 
				     || *(pdata+(i*DOMTILESZIE+j)*4+3) == 0
					)
					&& *oldmask !=0)
					{
						memcpy(pdata+(i*DOMTILESZIE+j)*4, olddata+(i*DOMTILESZIE+j)*4, 4);
						*pmask = *oldmask;
					}
				}
			}			
		}//�ϲ�����
	}//�߽��
	glbBool isInTran = mpr_ted->IsInTransaction();
	if (!isInTran)
		mpr_ted->BeginTransaction();	
	bool rt = mpr_ted->WriteDom(tileRow, tileCol, level, pixelBlock);
	if(!rt)
	{
		if (!isInTran)
			mpr_ted->RollbackTransaction();
	}else{
		if (!isInTran)
			mpr_ted->CommitTransaction();
	}
	return rt;
}

glbBool CGlbFileTerrainDataset::WriteDem
	    (glbInt32 level, 
		 glbInt32 tileCol,
		 glbInt32 tileRow, 
		 GlbTileNeighbor& neighbors,
		 CGlbPixelBlock* pixelBlock )
{
	if(mpr_ted == NULL)
	{
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
	if (pixelBlock == NULL)
	{
		GlbSetLastError(L"����pixelBlockΪNULL");
		return false;
	}

	glbBool isInTran = mpr_ted->IsInTransaction();
	if (mpr_ted->mpr_isGlobe)
	{
		glbInt32 istartx,istarty,iendx,iendy;
		CGlbExtent ext(-180,180,-90,90);
		GetDemExtentCoverTiles(&ext, level,istartx,istarty,iendx,iendy);
		if (level<0 || tileCol<istartx || tileCol>iendx|| 
			tileRow<istarty ||tileRow>iendy)
		{
			GlbSetLastError(L"�����������ȷ");
			return false;
		}

		glbInt32 rows,cols;
		pixelBlock->GetSize(cols,rows);
		if (pixelBlock->GetPixelType() != GLB_PIXELTYPE_FLOAT32 ||
			pixelBlock->GetBandCount() != 1 ||
			rows != DEMTILESZIE || cols != DEMTILESZIE)
		{
			GlbSetLastError(L"pixelBlock ��ƥ��");
			return false;
		}
		if(!isInTran)
			mpr_ted->BeginTransaction();

		glbref_ptr<CGlbPixelBlock> oldBlock = NULL;
		if(  neighbors.hasLeft   ||neighbors.hasRight
			||neighbors.hasUp     ||neighbors.hasDown
			||neighbors.hasLeftUp ||neighbors.hasLeftDown
			||neighbors.hasRightUp||neighbors.hasRightDown
			)
		{//�Ǳ߽��	&& ԭʼ���������ж�Ӧ�Ŀ�
			oldBlock = new CGlbPixelBlock(GLB_PIXELTYPE_FLOAT32,GLB_PIXELLAYOUT_BIP);
			if(!oldBlock->Initialize(1,DEMTILESZIE,DEMTILESZIE))
			{
				GlbSetLastError(L"�ڴ治��");
				return false;
			}

			if(ReadDem(level,tileCol,tileRow,oldBlock.get()) == false)
			{//�����ݲ����ڣ�����һ����ȡ����
				glbref_ptr<CGlbPixelBlock> upper = new CGlbPixelBlock(GLB_PIXELTYPE_FLOAT32, GLB_PIXELLAYOUT_BIP);
				if(!upper->Initialize(1,DEMTILESZIE,DEMTILESZIE))
				{
					GlbSetLastError(L"�ڴ治��");
					return false;
				}
				//���tileCol < 0����Ϊ���ɰ��ֵ��ε�ʱ�򣬳��ֵĸ�ֵ����
				if(ReadDem(level-1,tileCol/2,tileRow/2,upper.get()) == false)// || tileCol < 0
				{
					oldBlock = NULL;
				}
				else
				{
					glbref_ptr<CGlbPixelBlock> newPixelBlock = new CGlbPixelBlock(GLB_PIXELTYPE_FLOAT32, GLB_PIXELLAYOUT_BIP);
					if(!newPixelBlock->Initialize(1,DEMTILESZIE * 2,DEMTILESZIE * 2))
					{
						GlbSetLastError(L"�ڴ治��");
						return false;
					}
					if(!GlbUpsample_Average(upper.get(),newPixelBlock.get()))
					{
						oldBlock = NULL;
					}
					else
					{
						/*��ֵ�õ�����������*/
						glbInt32 upcol_idx = tileCol % 2 * (DEMTILESZIE);
						glbInt32 uprow_idx = tileRow % 2 * (DEMTILESZIE);
						glbFloat *oldData = NULL;
						glbByte *oldMask = NULL;
						glbFloat *newData = NULL;
						glbByte *newMask = NULL;
						glbInt32 up_col        = 0;
						oldBlock->GetPixelData((glbByte**)&oldData);
						oldBlock->GetMaskData(&oldMask);
						newPixelBlock->GetPixelData((glbByte**)&newData);
						newPixelBlock->GetMaskData(&newMask);
						for(glbInt32 i = 0;i < DEMTILESZIE;i++)
						{//��
							up_col = upcol_idx;
							for(glbInt32 j = 0;j < DEMTILESZIE;j++)
							{//��
								memcpy(oldData+(i*DEMTILESZIE+j), newData+(uprow_idx*DEMTILESZIE*2+up_col), 4);
								*(oldMask+i*DEMTILESZIE+j) = *(newMask+uprow_idx*DEMTILESZIE*2+up_col);	
								up_col++;
							}//��
							uprow_idx++;
						}//��
					}
				}
			}

			if(oldBlock.valid())
			{
				glbFloat *oldData = NULL;
				glbByte *oldMask = NULL;
				oldBlock->GetPixelData((glbByte**)&oldData);
				glbFloat *pData = NULL;
				glbByte *pMask = NULL;
				pixelBlock->GetPixelData((glbByte**)&pData);

				for(glbInt32 i = 0; i < DEMTILESZIE;i++)
				{
					for(glbInt32 j = 0; j < DEMTILESZIE;j++)
					{
						pixelBlock->GetMaskVal(j,i,&pMask);
						oldBlock->GetMaskVal(j,i,&oldMask);
						if(*pMask == 0 && *oldMask != 0)
						{
							*(pData + i * DEMTILESZIE + j) = *(oldData + i * DEMTILESZIE +j);
							*pMask = *oldMask;
						}
					}
				}
			}


			//�߽�� ���� ������������ı߽綼����Ч����
			glbref_ptr<CGlbPixelBlock> nbBlock = NULL;
			nbBlock = new CGlbPixelBlock(GLB_PIXELTYPE_FLOAT32,GLB_PIXELLAYOUT_BIP);
			if(!nbBlock->Initialize(1,DEMTILESZIE,DEMTILESZIE))
			{
				GlbSetLastError(L"�ڴ治��");
				return false;
			}
			if(neighbors.hasLeft)
			{
				if(ReadDem(level,tileCol - 1,tileRow,nbBlock.get()))
				{
					//����ֵ�޸ı߽�
					void*    pval  = NULL;
					glbByte* pmask = NULL;
					for (glbInt32 i=0;i<DEMTILESZIE;i++)
					{
						//ֻ��һ��ͨ��,ȡ��ֵ��� ���Ǿ�ֵ�ұ�
						pixelBlock->GetPixelVal(0, 0, i, &pval);
						nbBlock->SetPixelVal(0, DEMTILESZIE-1, i, pval);
						pixelBlock->GetMaskVal(0,i,&pmask);
						nbBlock->SetMaskVal(DEMTILESZIE-1,i,*pmask);
					}
					//���¿�
					if(!mpr_ted->WriteDem(tileRow, tileCol-1, level, nbBlock.get()))
					{
						if (!isInTran)
							mpr_ted->RollbackTransaction();				
						return false;
					}
				}
			}
			if(neighbors.hasRight)
			{
				if (ReadDem(level, tileCol+1, tileRow, nbBlock.get()))
				{
					//����ֵ�޸ı߽�
					void*    pval  = NULL;
					glbByte* pmask = NULL;
					for (glbInt32 i=0;i<DEMTILESZIE;i++)
					{
						//ֻ��һ��ͨ��,ȡ��ֵ�ұ� ���Ǿ�ֵ���
						pixelBlock->GetPixelVal(0, DEMTILESZIE-1, i, &pval);
						nbBlock->SetPixelVal(0, 0, i, pval);
						pixelBlock->GetMaskVal(DEMTILESZIE-1,i,&pmask);
						nbBlock->SetMaskVal(0,i,*pmask);
					}
					//���¿�
					if(!mpr_ted->WriteDem(tileRow, tileCol+1, level, nbBlock.get()))
					{
						if (!isInTran)
							mpr_ted->RollbackTransaction();

						return false;
					}
				}
			}
			if (neighbors.hasUp)
			{
				if (ReadDem(level, tileCol, tileRow-1, nbBlock.get()))
				{
					//����ֵ�޸ı߽�
					void*    pval  = NULL;
					glbByte* pmask = NULL;
					for (glbInt32 i=0;i<DEMTILESZIE;i++)
					{
						//ֻ��һ��ͨ��,ȡ��ֵ�ϱ� ���Ǿ�ֵ�±�
						pixelBlock->GetPixelVal(0, i, 0, &pval);
						nbBlock->SetPixelVal(0, i, DEMTILESZIE-1, pval);
						pixelBlock->GetMaskVal(i, 0, &pmask);
						nbBlock->SetMaskVal(i, DEMTILESZIE-1, *pmask);
					}

					//���¿�
					if(!mpr_ted->WriteDem(tileRow-1, tileCol, level, nbBlock.get()))
					{
						if (!isInTran)
							mpr_ted->RollbackTransaction();

						return false;
					}
				}
			}
			if (neighbors.hasDown)
			{
				if (ReadDem(level, tileCol, tileRow+1, nbBlock.get()))
				{
					//����ֵ�޸ı߽�
					void* pval = NULL;
					glbByte* pmask = NULL;
					for (glbInt32 i=0;i<DEMTILESZIE;i++)
					{
						//ֻ��һ��ͨ��,ȡ��ֵ�±� ���Ǿ�ֵ�ϱ�
						pixelBlock->GetPixelVal(0, i, DEMTILESZIE-1, &pval);
						nbBlock->SetPixelVal(0, i, 0, pval);
						pixelBlock->GetMaskVal(i, DEMTILESZIE-1, &pmask);
						nbBlock->SetMaskVal(i, 0, *pmask);
					}
					//���¿�
					if(!mpr_ted->WriteDem(tileRow+1, tileCol, level, nbBlock.get()))
					{
						if (!isInTran)
							mpr_ted->RollbackTransaction();
						return false;
					}
				}
			}
			if (neighbors.hasLeftUp)//Left Up
			{
				if (ReadDem(level, tileCol-1, tileRow-1, nbBlock.get()))
				{
					//����ֵ�޸ı߽�
					void* pval = NULL;
					glbByte* pmask = NULL;
					//ֻ����һ����,ȡ��ֵ���� ���Ǿ�ֵ����
					pixelBlock->GetPixelVal(0, 0, 0, &pval);
					nbBlock->SetPixelVal(0, DEMTILESZIE-1, DEMTILESZIE-1, pval);

					pixelBlock->GetMaskVal(0, 0, &pmask);
					nbBlock->SetMaskVal(DEMTILESZIE-1, DEMTILESZIE-1, *pmask);
					//���¿�
					if(!mpr_ted->WriteDem(tileRow-1, tileCol-1, level, nbBlock.get()))
					{
						if (!isInTran)
							mpr_ted->RollbackTransaction();
						return false;
					}
				}
			}
			if (neighbors.hasRightUp)//Right Up
			{
				if (ReadDem(level, tileCol+1, tileRow-1, nbBlock.get()))
				{
					//����ֵ�޸ı߽�
					void* pval = NULL;
					glbByte* pmask = NULL;
					//ֻ����һ����,ȡ��ֵ���� ���Ǿ�ֵ����
					pixelBlock->GetPixelVal(0, DEMTILESZIE-1, 0, &pval);
					nbBlock->SetPixelVal(0, 0, DEMTILESZIE-1, pval);

					pixelBlock->GetMaskVal(DEMTILESZIE-1, 0, &pmask);
					nbBlock->SetMaskVal(0, DEMTILESZIE-1, *pmask);
					//���¿�
					if(!mpr_ted->WriteDem(tileRow-1, tileCol+1, level, nbBlock.get()))
					{
						if (!isInTran)
							mpr_ted->RollbackTransaction();
						return false;
					}
				}
			}
			if (neighbors.hasLeftDown)//Left Down
			{
				if (ReadDem(level, tileCol-1, tileRow+1, nbBlock.get()))
				{
					//����ֵ�޸ı߽�
					void* pval = NULL;
					glbByte* pmask = NULL;
					//ֻ����һ����,ȡ��ֵ���� ���Ǿ�ֵ����
					pixelBlock->GetPixelVal(0, 0, DEMTILESZIE-1, &pval);
					nbBlock->SetPixelVal(0, DEMTILESZIE-1, 0, pval);

					pixelBlock->GetMaskVal(0, DEMTILESZIE-1, &pmask);
					nbBlock->SetMaskVal(DEMTILESZIE-1, 0, *pmask);
					//���¿�
					if(!mpr_ted->WriteDem(tileRow+1, tileCol-1, level, nbBlock.get()))
					{
						if (!isInTran)
							mpr_ted->RollbackTransaction();				
						return false;
					}
				}
			}
			if (neighbors.hasRightDown)//Right Down
			{
				if (ReadDem(level, tileCol+1, tileRow+1, nbBlock.get()))
				{
					//����ֵ�޸ı߽�
					void* pval = NULL;
					glbByte* pmask = NULL;
					//ֻ����һ����,ȡ��ֵ���� ���Ǿ�ֵ����
					pixelBlock->GetPixelVal(0, DEMTILESZIE-1, DEMTILESZIE-1, &pval);
					nbBlock->SetPixelVal(0, 0, 0, pval);

					pixelBlock->GetMaskVal(DEMTILESZIE-1, DEMTILESZIE-1, &pmask);
					nbBlock->SetMaskVal(0, 0, *pmask);
					//���¿�
					if(!mpr_ted->WriteDem(tileRow+1, tileCol+1, level, nbBlock.get()))
					{
						if (!isInTran)
							mpr_ted->RollbackTransaction();
						return false;
					}
				}
			}
		}
	}
	//���»�����
	if(!mpr_ted->WriteDem(tileRow, tileCol, level, pixelBlock))
	{
		if (!isInTran)
			mpr_ted->RollbackTransaction();
		return false;
	}
	if (!isInTran)
		mpr_ted->CommitTransaction();
	
	return true;	
}

glbBool CGlbFileTerrainDataset::MergerGlobeDem()
{
	if (mpr_ted == NULL)
	{
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
	if (!mpr_ted->mpr_isGlobe)
		return true;
	
	mpr_ted->BeginTransaction();
	
	glbref_ptr<CGlbPixelBlock> leftBlock = new CGlbPixelBlock(GLB_PIXELTYPE_FLOAT32, GLB_PIXELLAYOUT_BIP);
	if(!leftBlock->Initialize(1, DEMTILESZIE, DEMTILESZIE))
	{
		mpr_ted->RollbackTransaction();
		GlbSetLastError(L"�ڴ治��");
		return false;
	}
	glbref_ptr<CGlbPixelBlock> rightBlock = new CGlbPixelBlock(GLB_PIXELTYPE_FLOAT32, GLB_PIXELLAYOUT_BIP);
	if(!rightBlock->Initialize(1, DEMTILESZIE, DEMTILESZIE))
	{
		mpr_ted->RollbackTransaction();
		GlbSetLastError(L"�ڴ治��");
		return false;
	}
	glbFloat  pvalue;
	glbFloat *pvalLeft      = NULL;
	glbFloat *pvalRight     = NULL;
	glbInt32  hasWriteTiles = 0;
	for (glbInt32 level = mpr_ted->mpr_demMinLevel;level<=mpr_ted->mpr_demMaxLevel;level++)
	{
		glbInt32 istartx,istarty,iendx,iendy;
		GetDemTileIndexes(level,istartx,istarty,iendx,iendy);
		for (glbInt32 tileRow=istarty;tileRow<=iendy;tileRow++)
		{
			if( ReadDem(level, istartx, tileRow, leftBlock.get())
			 && ReadDem(level, iendx,   tileRow, rightBlock.get()))
			{
				for (glbInt32 irow=0;irow<DEMTILESZIE;irow++)
				{
					 leftBlock->GetPixelVal(0,0,            irow, (void**)&pvalLeft);
					rightBlock->GetPixelVal(0,DEMTILESZIE-1,irow, (void**)&pvalRight);
					pvalue = (*pvalLeft + *pvalRight)/2;
					*pvalLeft = pvalue;
					*pvalRight = pvalue;
				}
				
				if(!mpr_ted->WriteDem(tileRow, istartx, level, leftBlock.get()))
				{
					mpr_ted->RollbackTransaction();
					return false;
				}
				
				if(!mpr_ted->WriteDem(tileRow, iendx, level, rightBlock.get()))
				{
					mpr_ted->RollbackTransaction();
					return false;
				}	
				hasWriteTiles += 2;
				if(hasWriteTiles == 100)
				{
					mpr_ted->CommitTransaction();
					mpr_ted->BeginTransaction();
					hasWriteTiles = 0;
				}
			}			
		}
	}
	mpr_ted->CommitTransaction();
	return true;
}

glbBool CGlbFileTerrainDataset::ImportDomDataset
( IGlbRasterDataset* dataset, 
  glbInt32 minLevel,
  glbInt32 maxLevel,
  IGlbProgress* progress/*=NULL*/ 
)
{
	if (dataset == NULL)return true;
	if (mpr_ted == NULL)
	{
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
	if (!dataset->HasPyramid())
	{
		GlbSetLastError(L"���ݼ�û�н����������ȴ���");
		return false;
	}
	glbref_ptr<CGlbPixelBlock> pydBlock = dataset->CreatePyramidPixelBlock();
	if(pydBlock == NULL)return false;
	if(pydBlock->GetPixelType() != GLB_PIXELTYPE_BYTE)
	{
		GlbSetLastError(L"�������������Ͳ���BYTE");
		return false;
	}
	if(pydBlock->GetBandCount() != 4)
	{
		GlbSetLastError(L"������������4,RGBA");
		return false;
	}
	glbInt32 rows,cols;
	pydBlock->GetSize(cols,rows);
	if(rows != DOMTILESZIE || cols != DOMTILESZIE)
	{
		GlbSetLastError(L"���������ؿ�ߴ粻ƥ��(256*256)");
		return false;
	}
	
	mpr_ted->BeginTransaction();

	glbInt32 dsMinlevel,dsMaxlevel;	
	dataset->GetPyramidLevels(&dsMinlevel,&dsMaxlevel);
	minLevel = max(minLevel,dsMinlevel);
	maxLevel = min(maxLevel,dsMaxlevel);

	glbInt32 hasWriteTiles = 0;
	GlbTileNeighbor neighbors;
	for (glbInt32 level = minLevel;level <= maxLevel;level++)
	{
		glbInt32 tileStartCol,tileStartRow;
		glbInt32 tileEndCol,tileEndRow;
		dataset->GetPyramidTileIndexes(level,tileStartCol,tileStartRow,tileEndCol,tileEndRow);
		for (glbInt32 tileRow = tileStartRow;tileRow <= tileEndRow;tileRow++)
		{
			for (glbInt32 tileCol = tileStartCol;tileCol <= tileEndCol;tileCol++)
			{
				neighbors.hasLeft     = false;
				neighbors.hasRight    = false;
	            neighbors.hasUp       = false;
				neighbors.hasDown     = false;
				neighbors.hasLeftUp   = false;
				neighbors.hasRightUp  = false;
				neighbors.hasLeftDown = false;
				neighbors.hasRightDown= false;
				if(tileCol == tileStartCol)
					neighbors.hasLeft = true;
				else if(tileCol == tileEndCol)
					neighbors.hasRight  = true;
				
				if(tileRow == tileStartRow)
					neighbors.hasUp = true;
				else if(tileRow == tileEndRow)
					neighbors.hasDown = true;
				if(dataset->ReadPyramidByTile(level,tileCol,tileRow,pydBlock.get()))
				{
					if(this->WriteDom(level,tileCol,tileRow,neighbors,pydBlock.get()))
						hasWriteTiles++;
					else{
						mpr_ted->RollbackTransaction();
						return false;
					}
				}
				if(hasWriteTiles == 100)
				{
					mpr_ted->CommitTransaction();
					mpr_ted->BeginTransaction();
					hasWriteTiles = 0;
				}
			}//col
		}//row
	}//level
	mpr_ted->CommitTransaction();
	return true;
}

glbBool CGlbFileTerrainDataset::ImportDemDataset
( IGlbRasterDataset* dataset, 
  glbInt32           minLevel,
  glbInt32           maxLevel,
  IGlbProgress*      progress/*=NULL*/ )
{
	if (dataset == NULL) return true;
	if (mpr_ted == NULL)
	{
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
	if (!dataset->HasPyramid())
	{
		GlbSetLastError(L"���ݼ�û�н����������ȴ���");
		return false;
	}
	glbref_ptr<CGlbPixelBlock> pydBlock = dataset->CreatePyramidPixelBlock();
	if(pydBlock == NULL)return false;

	if (pydBlock->GetPixelType() != GLB_PIXELTYPE_FLOAT32)
	{
		GlbSetLastError(L"�������������Ͳ���Float32");
		return false;
	}
	if(pydBlock->GetBandCount() != 1)
	{
		GlbSetLastError(L"����������������1");
		return false;
	}
	glbInt32 rows,cols;
	pydBlock->GetSize(cols,rows);
	if(rows != DEMTILESZIE || cols != DEMTILESZIE)
	{
		GlbSetLastError(L"���������ؿ�ߴ粻ƥ��(17*17)");
		return false;
	}
	mpr_ted->BeginTransaction();
	glbInt32 hasWriteTiles = 0;
	GlbTileNeighbor neighbors;

	glbInt32 tdMaxLevel,tdMinLevel;
	GetDemLevels(tdMaxLevel,tdMinLevel);
	for (glbInt32 level = minLevel;level <= maxLevel;level++)
	{
		glbInt32 tileStartCol,tileEndCol;
		glbInt32 tileStartRow,tileEndRow;
		dataset->GetPyramidTileIndexes(level,tileStartCol,tileStartRow,tileEndCol,tileEndRow);
		for (glbInt32 tileRow=tileStartRow;tileRow<=tileEndRow;tileRow++)
		{
			for (glbInt32 tileCol=tileStartCol;tileCol<=tileEndCol;tileCol++)
			{
				neighbors.hasLeft     = false;
				neighbors.hasRight    = false;
	            neighbors.hasUp       = false;
				neighbors.hasDown     = false;
				neighbors.hasLeftUp   = false;
				neighbors.hasRightUp  = false;
				neighbors.hasLeftDown = false;
				neighbors.hasRightDown= false;
				//if(level <= tdMaxLevel && level >= tdMinLevel)
				//{
					if(tileCol == tileStartCol)
					{
						neighbors.hasLeft = true;
						if(tileRow == tileStartRow)
							neighbors.hasLeftUp=true;
						if(tileRow == tileEndRow)
							neighbors.hasLeftDown=true;					
					}
					else if(tileCol == tileEndCol)
					{
						neighbors.hasRight = true;
						if(tileRow == tileStartRow)
							neighbors.hasRightUp    = true;
						if(tileRow == tileEndRow)
							neighbors.hasRightDown    = true;
					}
					if(tileRow == tileStartRow)
						neighbors.hasUp = true;
					if(tileRow == tileEndRow)
						neighbors.hasDown = true;
				//}
				if(dataset->ReadPyramidByTile(level,tileCol,tileRow,pydBlock.get()))
				{
					//{
					//	if(level == 2 && tileRow == 2 && tileCol == -3)
					//	{
					//		CGlbWString ssp = CGlbWString::FormatString(L"D:\\level _%d_col_%d_row_%d_testDem.txt",level,tileCol,tileRow);			
					//		pydBlock->ExportToText(ssp.c_str());
					//	}
					//}
					if(this->WriteDem(level,tileCol,tileRow,neighbors,pydBlock.get()))
						hasWriteTiles++;
					else{
						mpr_ted->RollbackTransaction();
						return false;
					}
				}
				//if(level == 2 && tileRow == 2 && tileCol == -3)
				//{
				//	CGlbWString ssp = CGlbWString::FormatString(L"D:\\level _%d_col_%d_row_%d_testDem_w.txt",level,tileCol,tileRow);
				//	CGlbPixelBlock* p_dempixelblock = this->CreateDemPixelBlock();
				//	glbBool accessdemsuccess = this->ReadDem(level,tileCol,tileRow,p_dempixelblock);
				//	p_dempixelblock->ExportToText(ssp.c_str());
				//}
				if(hasWriteTiles == 100)
				{
					mpr_ted->CommitTransaction();
					mpr_ted->BeginTransaction();						
					hasWriteTiles = 0;
				}
			}//col
		}//row
	}//level

	mpr_ted->CommitTransaction();
	return true;
}