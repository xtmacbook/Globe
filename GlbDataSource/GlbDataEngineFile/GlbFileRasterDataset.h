/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbFileRasterDataset.h
  * @brief   դ�����ݼ���ͷ�ļ�
  *
  * �����������CGlbFileRasterDataset�����
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

class CGlbSQLitePyramid;///<�������࣬��������
/**
  * @brief CGlbFileRasterDataset��
  *
  * ��ȡդ�����ݼ��ĸ�����Ϣ������������
  * �̳нӿ�IGlbRasterDataset
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
	* @brief ����Ӱ�������
	*
	* @param tileSizeX ��Ƭx��С
	* @param tileSizeY ��Ƭy��С
	* @param lzts �ռ���Ƭ��С
	* @param isGlobe �Ƿ�����
	* @param progress ������
	* @return �ɹ�����true��ʧ�ܷ���false
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
	* @brief �����߳̽�����
	*
	* @param tileSizeX ��Ƭx��С
	* @param tileSizeY ��Ƭy��С
	* @param lzts �ռ���Ƭ��С
	* @param isGlobe �Ƿ�����
	* @param isReplace �Ƿ��滻value��ֻ��Ը̣߳�
	* @param rpValue Ҫ�滻��ֵ��ֻ��Ը̣߳�
	* @param progress ������
	* @return �ɹ�����true��ʧ�ܷ���false
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
	* @brief ��ȡ����������󼶱�Ŀռ䷶Χ
	*
	* @param ext �ռ䷶Χ
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	glbBool GetMaxLevelExtent(CGlbExtent* ext);
private:
	glbref_ptr<CGlbFileDataSource> mpr_ds;    ///<����Դ
	CGlbWString                    mpr_url;   ///<Ŀ¼
	CGlbWString                    mpr_name;  ///<���ݼ���
	CGlbWString                    mpr_alias; ///<���ݼ�����
	CGlbWString                    mpr_srs;   ///<������Ϣ
	glbref_ptr<CGlbExtent>         mpr_extent;///<���
	CGlbWString                    mpr_format;//<�ļ���ʽ����
	GlbPixelTypeEnum               mpr_pixeltype;///<��������
	GlbPixelLayoutTypeEnum         mpr_pixellayout;///<���ز�������
	CGlbWString                    mpr_compress;///<ѹ������
	GDALDatasetH                   mpr_gdalDataset;///<gdal���ݼ���Ϣ
	glbref_ptr<CGlbSQLitePyramid>  mpr_pyramid;///<������
	///<ChenPeng
	glbBool                               mpr_isMove;///<�Ƿ���Ҫ�Ϳ�
	glbBool                               mpr_isChange;///<�Ƿ���Ҫ�޸�
	glbDouble                          mpr_changeValue;///<ָ����Χ���޸�ֵ
	glbref_ptr<CGlbExtent> mpr_dealExtent;///<��Ҫ�����ָ����Χ
	glbBool                               mpr_isMaxLevel;///<���������ݵ�ʱ��ֻ��Ҫ�������ݵ����
};

