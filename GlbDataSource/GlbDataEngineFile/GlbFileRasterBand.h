/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbFileRasterBand.h
  * @brief   դ�񲨶���ͷ�ļ�
  *
  * �����������CGlbFileRasterBand�����
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#pragma once
#include "IGlbDataEngine.h"
#include <gdal.h>

/**
  * @brief CGlbFileRasterBand��
  *
  * ��ȡ������Ϣ
  * �̳нӿ�IGlbRasterBand
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
	* @brief ���캯��
	*
	* @param band gdal����
	* @return void
	*/
	CGlbFileRasterBand(GDALRasterBandH band);
	/**
	* @brief ��������
	*
	* @return void
	*/
	~CGlbFileRasterBand(void);
	
	/**
	* @brief ��ȡ����id
	*
	* @return ����id
	*/
	glbInt32 GetID();
	/**
	* @brief ��ȡ������
	*
	* @return ���ز�����
	*/
	const glbWChar* GetName();
private:
	GDALRasterBandH mpr_band;//����Ҫɾ��
};

