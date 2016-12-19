/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbSQLitePyramid.h
  * @brief   金字塔SQLite操作类头文件
  *
  * 这个档案定义CGlbSQLitePyramid这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#pragma once
#include "IGlbDataEngine.h"
#include "SQLite.h"
/**
  * @brief CGlbSQLitePyramid类
  *
  * 金字塔SQLite操作类
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CGlbSQLitePyramid:public CGlbReference
{
public:
	/**
	* @brief 构造函数
	*
	* @return void
	*/
	CGlbSQLitePyramid(void);
	/**
	* @brief 析构函数
	*
	* @return void
	*/
	~CGlbSQLitePyramid(void);

	/**
	* @brief 打开数据库
	*
	* @param fullname 数据库名
	* @return true表示成功，false表示失败
	*/
	glbBool Open(const glbWChar* fullname);
	/**
	* @brief 创建数据库
	*
	* @param fullname 数据库名
	* @return true表示成功，false表示失败
	*/
	glbBool Create(const glbWChar* fullname);
	/**
	* @brief 读取一个切片
	*
	* @param tileRow 行
	* @param tileCol 列
	* @param level 级别
	* @param pixelblock 像素块
	* @return true表示成功，false表示失败
	*/
	glbBool Read(glbInt32 tileRow,glbInt32 tileCol,glbInt32 level,
		CGlbPixelBlock* pixelblock);
	/**
	* @brief 写入一个切片,加锁，支持多线程
	*
	* @param tileRow 行
	* @param tileCol 列
	* @param level 级别
	* @param pixelblock 像素块
	* @return true表示成功，false表示失败
	*/
	glbBool Write(glbInt32 tileRow,glbInt32 tileCol,glbInt32 level,
		CGlbPixelBlock* pixelblock);
	glbBool UpdatePydInfo();
	/**
	* @brief 开始事务
	*
	* @return true表示成功，false表示失败
	*/
    glbBool BeginTransaction();
	/**
	* @brief 提交事务
	*
	* @return true表示成功，false表示失败
	*/
    glbBool CommitTransaction();
	/**
	* @brief 回滚事务
	*
	* @return true表示成功，false表示失败
	*/
    glbBool RollbackTransaction();
	/**
	* @brief 在事务中
	*
	* @return true表示在事务中，false表示没有事务
	*/
	bool IsInTransaction();

public:
	glbBool                mpr_isGlobe;///<是否球面
	GlbPixelTypeEnum       mpr_pixeltype;///<像素类型
	GlbPixelLayoutTypeEnum mpr_pixellayout;///<像素布局类型,为了快速显示，应该是像素交叉模式？
	glbInt32               mpr_bandCount;///<波段数
	glbDouble              mpr_lzts;///<0级切片空间大小
	glbInt32               mpr_tileSizeX;///<切片像素x大小
	glbInt32               mpr_tileSizeY;///<切片像素y大小
	glbInt32               mpr_minLevel;///<最小级别
	glbInt32               mpr_maxLevel;///<最大级别
	glbDouble              mpr_originX;///<原点x坐标
	glbDouble              mpr_originY;///<原点y坐标
	glbInt32               mpr_rows;///<最大级别分块行数
	glbInt32               mpr_cols;///<最大级别分块列数
	glbBool                mpr_isShareEdge;///<是否共享边界
	GlbCompressTypeEnum    mpr_compress;//压缩类型
	CGlbWString            mpr_srs;///<坐标信息
private:
	CSQLite*            mpr_sqlite;///<SQLite数据库
	GlbCriticalSection  mpr_cs;    ///<临界区，线程同步
};

typedef struct _GlbLevelInfo
{
	glbDouble OriginX;///<原点x坐标
	glbDouble OriginY;///<原点y坐标
	glbInt32 Rows;///<分块行数
	glbInt32 Cols;///<分块列数
	glbInt32 startRow;
	glbInt32 endRow;
	glbInt32 startCol;
	glbInt32 endCol;
} GlbLevelInfo;//级别的外包信息
/**
  * @brief CGlbTedSQLite类
  *
  * 地形金字塔SQLite操作类
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CGlbTedSQLite:public CGlbReference
{
public:
	/**
	* @brief 构造函数
	*
	* @return void
	*/
	CGlbTedSQLite(void);
	/**
	* @brief 析构函数
	*
	* @return void
	*/
	~CGlbTedSQLite(void);
	/**
	* @brief 打开数据库
	*
	* @param fullname 数据库名
	* @return true表示成功，false表示失败
	*/
	glbBool Open(const glbWChar* fullname);
	/**
	* @brief 创建数据库
	*
	* @param fullname 数据库名
	* @return true表示成功，false表示失败
	*/
	glbBool Create(const glbWChar* fullname);
	/**
	* @brief 读取dom一个切片
	*
	* @param tileRow 行
	* @param tileCol 列
	* @param level 级别
	* @param pixelblock 像素块
	* @return true表示成功，false表示失败
	*/
	glbBool ReadDom(glbInt32 tileRow,glbInt32 tileCol,glbInt32 level,
		CGlbPixelBlock* pixelblock);
	/**
	* @brief 读取dem一个切片
	*
	* @param tileRow 行
	* @param tileCol 列
	* @param level 级别
	* @param pixelblock 像素块
	* @return true表示成功，false表示失败
	*/
	glbBool ReadDem(glbInt32 tileRow,glbInt32 tileCol,glbInt32 level,
		CGlbPixelBlock* pixelblock);
	/**
	* @brief 写入Dom一个切片
	*
	* @param isUpdate 是否更新
	* @param tileRow 行
	* @param tileCol 列
	* @param level 级别
	* @param pixelblock 像素块
	* @return true表示成功，false表示失败
	*/
	glbBool WriteDom(glbInt32 tileRow,
					 glbInt32 tileCol,
					 glbInt32 level,
		             CGlbPixelBlock* pixelblock);
	/**
	* @brief 写入Dem一个切片
	*
	* @param tileRow 行
	* @param tileCol 列
	* @param level 级别
	* @param pixelblock 像素块
	* @return true表示成功，false表示失败
	*/
	glbBool WriteDem(glbInt32 tileRow,
		             glbInt32 tileCol,
					 glbInt32 level,
		             CGlbPixelBlock* pixelblock);	
	/**
	* @brief 开始事务
	*
	* @return true表示成功，false表示失败
	*/
    glbBool BeginTransaction();
	/**
	* @brief 提交事务
	*
	* @return true表示成功，false表示失败
	*/
    glbBool CommitTransaction();
	/**
	* @brief 回滚事务
	*
	* @return true表示成功，false表示失败
	*/
    glbBool RollbackTransaction();
	/**
	* @brief 在事务中
	*
	* @return true表示在事务中，false表示没有事务
	*/
	bool IsInTransaction();
private:
	
	glbBool UpdateDomTileInfo(glbInt32 level, glbInt32 column, glbInt32 row);
	
	glbBool UpdateDemTileInfo(glbInt32 level, glbInt32 column, glbInt32 row);
public:
	glbBool                              mpr_isGlobe;///<是否球面
	glbDouble                            mpr_lzts;///<0级切片空间大小
	glbInt32                             mpr_domMinLevel;///<影像最小级别
	glbInt32                             mpr_domMaxLevel;///<影像最大级别
	glbInt32                             mpr_demMinLevel;///<高程最小级别
	glbInt32                             mpr_demMaxLevel;///<高程最大级别
	map<glbInt32/*level*/,GlbLevelInfo*> mpr_domLevelInfo;
	map<glbInt32/*level*/,GlbLevelInfo*> mpr_demLevelInfo;
	CGlbWString                          mpr_srs;///<坐标信息
	GlbCompressTypeEnum                  mpr_domCompress;///影像压缩方式
	GlbCompressTypeEnum                  mpr_demCompress;///高程压缩方式
private:
	CSQLite*            mpr_sqlite;  ///<SQLite数据库
	GlbCriticalSection  mpr_cs;      ///<临界区，线程同步
};

glbBool _compress(GlbCompressTypeEnum compress, CGlbPixelBlock* pixelblock, glbByte** outdata, glbUInt32& outlen);