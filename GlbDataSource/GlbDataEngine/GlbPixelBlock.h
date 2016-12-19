/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbPixelBlock.h
  * @brief   ���ؿ���ͷ�ļ�
  *
  * �����������CGlbPixelBlock�����
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#pragma once
#include "GlbExtent.h"
#include "GlbSpatialReference.h"
#include "GlbDataEngineExport.h"
#include "GlbDataEngineType.h"
#include "glbref_ptr.h"

/**
  * @brief ���ؿ�Ĺ�����
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-11 14:40
  */
class GLBDATAENGINE_API CGlbPixelBlock:public CGlbReference
{
public:
	/**
	* @brief ���캯��
	*
	* @param pixelType   ��������
	* @param pixelLayout ���ز��ַ�ʽ
	* @return void
	*/
	CGlbPixelBlock(GlbPixelTypeEnum pixelType, GlbPixelLayoutTypeEnum pixelLayout);
	/**
	* @brief ��ʼ������
	*
	* @param nBands  ������
	* @param columns ����
	* @param rows    ����
	* @return �ɹ�:����true��ʧ��:false
	*/
	glbBool Initialize(glbInt32 nBands, glbInt32 columns, glbInt32 rows,glbDouble nodata=0);
	/**
	* @brief ��������
	*
	* @return void
	*/
	~CGlbPixelBlock();
	/**
	* @brief �жϸ����ؿ��Ƿ���Ч
	*
	* @return ��Ч:����true����Ч:false
	*/
	glbBool               IsValid();
	/**
	* @brief ��ȡ����bufferָ��
	*
	* @param ppData ����buffer�ĵ�ַ
	* @return �ɹ�:�����������ݵ��ֽ�����ʧ��:0
	*/
	glbInt32              GetPixelData(glbByte** ppData);	
	/**
	* @brief ��ȡ����bufferָ��
	*        ����ȡֵ��Χ[0,255],0������Ч.        
	*
	* @param ppMask ����buffer�ĵ�ַ
	* @return �ɹ�:�������������ֽ�����ʧ��:0
	*/
	glbInt32              GetMaskData(glbByte** ppMask);
	/**
	* @brief �õ�������
	*
	* @return �ɹ�:���ز�������ʧ��:0
	*/
	glbInt32               GetBandCount();
	/**
	* @brief ��ȡ��������
	*
	* @return �ɹ�:�����������ͣ�ʧ��:��Ч����
	*/
	GlbPixelTypeEnum       GetPixelType();
	/**
	* @brief ��ȡ���ؿ������������
	*
	* @param colums ��������
	* @param rows   ��������
	* @return void
	*/
	void                   GetSize(glbInt32& colums, glbInt32& rows);
	/**
	* @brief ��ȡ���ز���
	*
	* @return �ɹ�:�������ز��֣�ʧ��:��Ч��������
	*/
	GlbPixelLayoutTypeEnum GetPixelLayout();	
	/**
	* @brief �������ı��ļ�
	*
	* @param fileName ����ļ����ļ���(��·��)
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool                ExportToText(const glbWChar* fileName);	
	/**
	* @brief ������tiff�ļ�
	*
	* @param fileName ����ļ����ļ���(��·��)
	* @param extent �ռ䷶Χ
	* @param srs ������Ϣ
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool                ExportToTiff(const glbWChar*   fileName,
		                                const CGlbExtent* extent=NULL,
										const glbWChar*   srs   =NULL);
	/**
	* @brief �ӵ�ǰ�鹹���rgba����
	*        �������ͱ�����byte
	*        ���ز��ֱ��������ؽ���
	*        ���ͨ������4��������mpr_data
	*        ���ͨ������3������Ϊ��rgb����mask��Ϊaͨ���ϲ���rgba���޸Ĳ�����Ϊ4���޸�mpr_data
	*        ����ͨ����ʽ����֧��
	*
	* @return �ɹ�:����true��ʧ��:false
	*/
	glbBool                ConvertToRGBA();

	/**
	* @brief ��ȡ���ص�ֵ
	*
	* @param iband �������
	* @param col �� 
	* @param row ��
	* @param ppVal �������صĵ�ָ���ַ,���Ͳ��̶�
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool GetPixelVal( glbInt32 iband,  glbInt32 col,  glbInt32 row,  void** ppVal);
	
	/**
	* @brief �������ص�ֵ
	*
	* @param iband �������
	* @param col �� 
	* @param row ��
	* @param ppVal ���صĵ�ָ���ַ,���Ͳ��̶�
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool SetPixelVal( glbInt32 iband,  glbInt32 col,  glbInt32 row,  void* ppVal);
	
	/**
	* @brief ��ȡ����ֵ
	*        ����ȡֵ��Χ[0,255],0������Ч.
	*
	* @param col �� 
	* @param row ��
	* @param ppVal ���ص�ַ
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool GetMaskVal(glbInt32 col,  glbInt32 row,  glbByte** ppVal);
	/**
	* @brief ��������ֵ
	*        ����ȡֵ��Χ[0,255],0������Ч.
	*
	* @param col  �� 
	* @param row  ��
	* @param mVal ����ֵ
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool SetMaskVal(glbInt32 col,  glbInt32 row,  glbByte mVal);
	/**
	* @brief ����ǿ����0
	*
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool Clear(double nodata=0);
	
	/**
	* @brief ��pixelVal����maskVal��������������Ϊ��,��iUseBandΪ��׼
	*
	* @param iUseBand ��׼���� 
	* @param maskVal maks������ֵ
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool Mask(glbInt32 iUseBand,void*maskVal);

	/**
	* @brief ����Ӧ��maskΪ0�����в�������ֵ,���Ϊ��Чֵ�����û����Чֵ,�����
	*
	* @param pNoDataVal ����ò���ΪNULL,������;����,��Чֵȫ�����Ϊ��ֵ 
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool ReplaceNoData(glbDouble NoData,glbDouble rpValue);

	/**
	* @brief Դ���ؿ�͵�ǰ���ؿ���,�������ǰ���ؿ�
	*        �ٽ�������ȥƽ��ֵ
	*        �����������һ��,���ܴ���.
	*        ���src��size�������size��һ��,���ܴ���.
	*        ������Ͳ�һ��,������
	*
	* @param src Դ���ؿ�
	* @param sStartX Դ����X��ʼλ�ã�������Ϊ-1
	* @param sStartY Դ����Y��ʼλ�ã�������Ϊ-1
	* @param dStartX Ŀ������X��ʼλ�ã�������Ϊ-1
	* @param dStartY Ŀ������Y��ʼλ�ã�������Ϊ-1
	* @param cx ��ϵ����ؿ�� ���²����Ǹ�ֵ
	* @param cy ��ϵ����ظ߶� ���Ҳ����Ǹ�ֵ
	* @return �ɹ�:����true��ʧ��:fasle
	*/
	glbBool Blend(CGlbPixelBlock*src,glbInt32 sStartX,glbInt32 sStartY, 
		glbInt32 dStartX,glbInt32 dStartY,glbInt32 cx=4,glbInt32 cy=4);
	/**
	* @brief ��ȡ���ؿ�ķ�Χ
	*
	* @return �ɹ�:���ط�Χ��ʧ��:NULL
	*/
	const CGlbExtent* GetExtent() {return mpr_extent.get();}
	/**
	* @brief �������ؿ�ķ�Χ
	*
	* @param ext ��Χ
	* @return void
	*/
	void SetExtent(CGlbExtent* ext) {mpr_extent=ext;}
private:
	GlbPixelTypeEnum       mpr_pixelType;  ///<��������
	GlbPixelLayoutTypeEnum mpr_pixelLayout;///<���ز���
	glbInt32               mpr_nBands;     ///<������
	glbInt32               mpr_rows;       ///<����
	glbInt32               mpr_colums;     ///<����	
	glbByte*               mpr_data;       ///<��������
	glbByte*               mpr_maskdata;   ///<mask���ݣ�0��ʾ����Ч���ݣ���������Ч����
	glbref_ptr<CGlbExtent> mpr_extent;     ///<��Χ
};
