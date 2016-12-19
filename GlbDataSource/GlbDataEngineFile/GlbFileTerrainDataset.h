/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbFileTerrainDataset.h
  * @brief   地型数据集类头文件
  *
  * 这个档案定义CGlbFileTerrainDataset这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#pragma once
#include "IGlbDataEngine.h"
#include "GlbFileDataSource.h"

class CGlbTedSQLite;///<金字塔类，方便引用
class  CGlbFileTerrainDataset :
	public IGlbTerrainDataset
{
public:
	CGlbFileTerrainDataset(void);
	~CGlbFileTerrainDataset(void);

	glbBool            Initialize(CGlbFileDataSource* ds, const glbWChar* name, const glbWChar* alias, CGlbTedSQLite* ted);
    glbBool            QueryInterface(const glbWChar *riid, void **ppvoid);
    GlbDatasetTypeEnum GetType();
	const glbWChar*    GetName();
	const glbWChar*    GetAlias();
	const IGlbDataSource* GetDataSource();
	const IGlbDataset*    GetParentDataset();
	const GlbSubDatasetRoleEnum GetSubRole();
	glbWChar*             GetLastError();
    const glbWChar*	      GetSRS();
	glbBool               AlterSRS(const glbWChar* srs);
	const CGlbExtent*     GetExtent() ;
	glbBool               HasZ();
	glbBool               HasM();
	

	glbBool          GetLZTS(glbDouble& lzts);
	glbBool          IsGlobe();
	GlbPixelTypeEnum       GetDomPixelType();
	GlbPixelTypeEnum       GetDemPixelType();
	GlbPixelLayoutTypeEnum GetDomPixelLayout();
	GlbPixelLayoutTypeEnum GetDemPixelLayout();
	glbBool                GetDomBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY);
	glbBool                GetDemBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY);
	glbInt32 GetDomBandCount();
	glbInt32 GetDemBandCount();
	glbBool  GetDomLevels(glbInt32& maxLevel,glbInt32& minLevel);
	glbBool  GetDemLevels(glbInt32& maxLevel,glbInt32& minLevel);
	glbBool  GetDomLevelExtent(glbInt32 iLevel, CGlbExtent* ext);
	glbBool  GetDemLevelExtent(glbInt32 iLevel, CGlbExtent* ext);
	glbBool  GetDomTileIndexes(glbInt32 level, 
		                       glbInt32& startTileColumn,  
							   glbInt32& startTileRow, 
		                       glbInt32& endTileColumn,  
							   glbInt32&  endTileRow);
	glbBool GetDemTileIndexes(glbInt32 level, 
		                      glbInt32& startTileColumn,
							  glbInt32& startTileRow, 
		                      glbInt32& endTileColumn,
							  glbInt32&  endTileRow);
	glbBool GetDomExtentCoverTiles(const CGlbExtent* extent, 
		                           glbInt32          level,
								   glbInt32&         startTileColumn, 
								   glbInt32&         startTileRow,
		                           glbInt32&         endTileColumn, 
								   glbInt32&         endTileRow);
	glbBool GetDemExtentCoverTiles(const CGlbExtent* extent,
		                          glbInt32           level,
								  glbInt32&          startTileColumn, 
								  glbInt32&          startTileRow,
		                          glbInt32&          endTileColumn, 
								  glbInt32&          endTileRow);
    glbBool GetDomTilesCoverExtent(glbInt32 level,
		                           glbInt32 startTileColumn, 
								   glbInt32 startTileRow,
		                           glbInt32 endTileColumn, 
								   glbInt32 endTileRow, 
								   CGlbExtent* extent);
	glbBool GetDemTilesCoverExtent(glbInt32 level,
		                           glbInt32 startTileColumn, 
								   glbInt32 startTileRow,
		                           glbInt32 endTileColumn,
								   glbInt32 endTileRowconst,
								   CGlbExtent* extent);
	CGlbPixelBlock* CreateDemPixelBlock();
	CGlbPixelBlock* CreateDomPixelBlock();
	glbBool ReadDom(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock);
	glbBool ReadDem( glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock);
	glbBool ReadDem(glbDouble lonOrX,glbDouble latOrY, CGlbPixelBlock* pixelBlock);
	glbBool WriteDom(glbInt32        level,
		             glbInt32        tileCol,
					 glbInt32        tileRow,
					 GlbTileNeighbor& neighbors,
					 CGlbPixelBlock* pixelBlock);
	glbBool WriteDem(glbInt32 level,
		             glbInt32 tileCol,
					 glbInt32 tileRow,
					 GlbTileNeighbor& neighbors,
					 CGlbPixelBlock* pixelBlock);
	glbBool MergerGlobeDem();
	glbBool ImportDomDataset(IGlbRasterDataset* dataset, 
		                     glbInt32           minLevel,
						     glbInt32           maxLevel,
							 IGlbProgress* progress=NULL);
	glbBool ImportDemDataset(IGlbRasterDataset* dataset, 
		                     glbInt32           minLevel,
							 glbInt32           maxLevel,
							 IGlbProgress* progress=NULL);
private:
	glbBool GetDomMaxLevelExtent(CGlbExtent* extent);
	glbBool GetDemMaxLevelExtent(CGlbExtent* extent);
private:
	glbref_ptr<CGlbFileDataSource> mpr_ds;     ///<数据源
	CGlbWString                    mpr_name;   ///<数据集名
	CGlbWString                    mpr_alias;  ///<数据集别名
	glbref_ptr<CGlbTedSQLite>      mpr_ted;    ///<地形金字塔数据
	glbref_ptr<CGlbExtent>         mpr_extent; ///<范围
};

