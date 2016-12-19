/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbFileRasterDataset.h
  * @brief   栅格数据集类头文件
  *
  * 这个档案定义CGlbFileRasterDataset这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#pragma once
#include "IGlbDataEngine.h"
#include "GlbDataEngineType.h"
#include "GlbFileDataSource.h"
#include "GlbFileRasterBand.h"

class CGlbSQLitePyramid;///<金字塔类，方便引用
/**
  * @brief CGlbFileRasterDataset类
  *
  * 获取栅格数据集的各种信息及创建金字塔
  * 继承接口IGlbRasterDataset
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class  CGlbFileRasterDataset:
	public IGlbRasterDataset
{
public:
	CGlbFileRasterDataset(void);
	~CGlbFileRasterDataset(void);
	glbBool                Initialize(CGlbFileDataSource* ds, 
		                              const glbWChar*     url,
									  const glbWChar*     name, 
									  const glbWChar*     alias, 
									  GDALDatasetH        dst, 
									  CGlbSQLitePyramid*  fp);
	glbBool                QueryInterface(const glbWChar *riid, void **ppvoid);
	GlbDatasetTypeEnum     GetType();
	const glbWChar*        GetName();
	const glbWChar*        GetAlias();
	const IGlbDataSource*  GetDataSource();
	const IGlbDataset*     GetParentDataset();
	const GlbSubDatasetRoleEnum GetSubRole();
	glbWChar*              GetLastError();
	const glbWChar*	       GetSRS();
	glbBool                AlterSRS(const glbWChar* srs);
	const CGlbExtent*      GetExtent() ;
	glbBool                HasZ();
	glbBool                HasM();
	glbBool                GetOrigin(glbDouble* originX, glbDouble* originY);
	glbBool                GetSize(glbInt32 *columns, glbInt32 *rows);
	glbBool                GetRS(glbDouble *resX, glbDouble *resY);
	GlbPixelTypeEnum       GetPixelType();
	GlbPixelLayoutTypeEnum GetPixelLayout();
	const glbWChar*        GetCompressType();
	const glbWChar*        GetFormat();
	glbBool                GetNoData(glbDouble *noDataVal);
	glbBool                AlterNoData(glbDouble noDataVal);
	glbInt32               GetBandCount();
	const IGlbRasterBand*  GetBand(glbInt32 bandIndex);
	glbBool                GetBlockSize(glbInt32* blockSizeX,glbInt32* blockSizeY);
	CGlbPixelBlock*        CreatePixelBlock(glbInt32 columns, glbInt32 rows);
	glbBool                Read(glbInt32 startColum, glbInt32 startRow, CGlbPixelBlock *pPixelBlock);
	glbBool                ReadEx(const CGlbExtent *destExtent, CGlbPixelBlock *pPixelBlock,GlbRasterResampleTypeEnum rspType/*,glbBool isRelapce*/);
	glbBool                   Write(glbInt32 startColum, glbInt32 startRow, CGlbPixelBlock *pPixelBlock);
	glbBool                   HasPyramid(){ return mpr_pyramid!=NULL; }
	glbBool                   BuildPyramid(
		                                   glbInt32      srcCol,
		                                   glbInt32      srcRow,
		                                   glbInt32      srcCols,
		                                   glbInt32      srcRows,
										   GlbRasterResampleTypeEnum rspType,
		                                   glbInt32      tileSizeX, 
		                                   glbInt32      tileSizeY, 										   
										   glbDouble     lzts        = 36.0, 
								           glbBool       isGlobe     =true, 
										   glbBool       isShareEdge = false, 
								           glbBool       isReplace   =false,
										   glbDouble     rpValue     =0.0, 
										   IGlbProgress* progress    =0);
	glbBool                  DeletePyramid();
	GlbPixelLayoutTypeEnum   GetPyramidPixelLayout();
	GlbPixelTypeEnum         GetPyramidPixelType();
	glbDouble                GetPyramidLZTS();
	glbBool                  IsPyramidShareEdge();
	glbBool                  GetPyramidLevels(glbInt32* minLevel, glbInt32* maxLevel);
	glbBool                  GetPyramidLevelRS(glbInt32 level, 
		                                       glbDouble *resolutionX, 
											   glbDouble *resolutionY);
	glbBool                  GetPyramidTileIndexes(glbInt32  iLevel,
		                                           glbInt32& startTileColumn,
												   glbInt32& startTileRow,
		                                           glbInt32& endTileColumn,
												   glbInt32& endTileRow);
	glbBool                  GetExtentCoverTiles(const CGlbExtent* extent, 
		                                         glbInt32 iLevel,
												 glbInt32& startTileColumn, 
												 glbInt32& startTileRow,
		                                         glbInt32& endTileColumn, 
												 glbInt32& endTileRow);
	glbBool                  GetTilesCoverExtent(glbInt32 level,
		                                         glbInt32 startTileColumn, 
												 glbInt32 startTileRow,
												 glbInt32 endTileColumn, 
		                                         glbInt32 endTileRow,
												 CGlbExtent* extent);
	glbBool                  GetLevelExtent(glbInt32 ilevel, CGlbExtent*);
	glbBool                  GetPyramidBlockSize(glbInt32* blockSizeX, glbInt32* blockSizeY);
	CGlbPixelBlock*          CreatePyramidPixelBlock();
	glbBool                  ReadPyramidByTile(glbInt32 level, 
		                                       glbInt32 tileColumn,
	                                           glbInt32 tileRow, 
											   CGlbPixelBlock *pPixelBlock);
	glbBool                  WritePyramidByTile(glbInt32 level, 
		                                        glbInt32 tileColumn,
	                                            glbInt32 tileRow, 
												CGlbPixelBlock *pPixelBlock);
	void SetSelfDealMoveExtent(CGlbExtent *extent,glbBool isMove);
	void SetSelfDealChangeExtent(CGlbExtent *extent,glbDouble value,glbBool isChange);
private:
	glbBool _ReadEx( const CGlbExtent *destExtent,
		             CGlbPixelBlock   *pPixelBlock,
					 GlbRasterResampleTypeEnum rspType,
					 glbBool          isReplace=false,
					 glbDouble        rpValue  =0.0,
					 glbBool          isDem=false);
	/** 
	* @brief 创建影像金字塔
	*
	* @param tileSizeX 切片x大小
	* @param tileSizeY 切片y大小
	* @param lzts 空间切片大小
	* @param isGlobe 是否球面
	* @param progress 进度条
	* @return 成功返回true，失败返回false
	*/
	glbBool _BuildDomPyramid( glbInt32      srcCol,
		                      glbInt32      srcRow,
							  glbInt32      srcCols,
							  glbInt32      srcRows,
							  GlbRasterResampleTypeEnum rspType,
							  glbInt32      tileSizeX, 
		                      glbInt32      tileSizeY, 
							  glbDouble     lzts, 
		                      glbBool       isGlobe, 
							  IGlbProgress* progress);

	CGlbFileRasterDataset* _BuildDomPyramidLevel(
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
		                      CGlbFileRasterDataset *srcDataset);

	glbBool _BuildDomPyramidLevelTile(
		                      glbInt32 tileRow,
		                      glbInt32 tileCol,							  
							  glbInt32 level,
		                      CGlbPixelBlock* pixelblock,
							  glbBool  isBorder);

	/** 
	* @brief 创建高程金字塔
	*
	* @param tileSizeX 切片x大小
	* @param tileSizeY 切片y大小
	* @param lzts 空间切片大小
	* @param isGlobe 是否球面
	* @param isReplace 是否替换value（只针对高程）
	* @param rpValue 要替换的值（只针对高程）
	* @param progress 进度条
	* @return 成功返回true，失败返回false
	*/
	glbBool _BuildDemPyramid( 
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
							  IGlbProgress* progress);
	CGlbFileRasterDataset*  _BuildDemPyramidLevel(
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
		                      CGlbFileRasterDataset *srcDataset);
	glbBool _BuildDemPyramidLevel(
		                      glbBool   isGlobe,
		                      glbInt32  level,
							  glbInt32  tileStartCol,
		                      glbInt32  tileEndCol,
                              glbInt32  tileStartRow,
		                      glbInt32  tileEndRow, 							   	                      
		                      glbInt32  tileSizeX,
		                      glbInt32  tileSizeY,							  
		                      CGlbFileRasterDataset* pydDataset,
		                      CGlbFileRasterDataset* preDataset);
	/** 
	* @brief 获取金字塔的最大级别的空间范围
	*
	* @param ext 空间范围
	* @return 成功返回true，失败返回false
	*/
	glbBool GetMaxLevelExtent(CGlbExtent* ext);
private:
	glbref_ptr<CGlbFileDataSource> mpr_ds;    ///<数据源
	CGlbWString                    mpr_url;   ///<目录
	CGlbWString                    mpr_name;  ///<数据集名
	CGlbWString                    mpr_alias; ///<数据集别名
	CGlbWString                    mpr_srs;   ///<坐标信息
	glbref_ptr<CGlbExtent>         mpr_extent;///<外包
	CGlbWString                    mpr_format;//<文件格式类型
	GlbPixelTypeEnum               mpr_pixeltype;///<像素类型
	GlbPixelLayoutTypeEnum         mpr_pixellayout;///<像素布局类型
	CGlbWString                    mpr_compress;///<压缩类型
	GDALDatasetH                   mpr_gdalDataset;///<gdal数据集信息
	glbref_ptr<CGlbSQLitePyramid>  mpr_pyramid;///<金字塔
	///<ChenPeng
	glbBool                               mpr_isMove;///<是否需要掏空
	glbBool                               mpr_isChange;///<是否需要修改
	glbDouble                          mpr_changeValue;///<指定范围的修改值
	glbref_ptr<CGlbExtent> mpr_dealExtent;///<需要处理的指定范围
	glbBool                               mpr_isMaxLevel;///<当处理数据的时候只需要处理数据的最大级
};

