/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbFileRasterBand.h
  * @brief   栅格波段类头文件
  *
  * 这个档案定义CGlbFileRasterBand这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#pragma once
#include "IGlbDataEngine.h"
#include <gdal.h>

/**
  * @brief CGlbFileRasterBand类
  *
  * 获取波段信息
  * 继承接口IGlbRasterBand
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class  CGlbFileRasterBand:
	public IGlbRasterBand
{
public:
	/**
	* @brief 构造函数
	*
	* @param band gdal波段
	* @return void
	*/
	CGlbFileRasterBand(GDALRasterBandH band);
	/**
	* @brief 析构函数
	*
	* @return void
	*/
	~CGlbFileRasterBand(void);
	
	/**
	* @brief 获取波段id
	*
	* @return 返回id
	*/
	glbInt32 GetID();
	/**
	* @brief 获取波段名
	*
	* @return 返回波段名
	*/
	const glbWChar* GetName();
private:
	GDALRasterBandH mpr_band;//不需要删除
};

