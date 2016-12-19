/********************************************************************
 * Copyright (c) 2013 ��ά����
 * All rights reserved.
 *
 * @file    
 * @brief   
 * @version 1.0
 * @author  GWB
 * @date    2014-03-07 11:28
 ********************************************************************
 */
#pragma once

/**
  * @brief ���ݼ�����ö��
  *
  * ���岻ͬ�ĵ������ݼ�����
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbDatasetTypeEnum
{
	GLB_DATASET_UNKNOWN                       =-1,///<δ��������
	GLB_DATASET_OBJECTCLASS                   = 0,///<�������ݼ�
	GLB_DATASET_FEATURECLASS_POINT            = 1,///<�������ݼ�
	GLB_DATASET_FEATURECLASS_MULTIPOINT       = 2,///<������ݼ�
	GLB_DATASET_FEATURECLASS_LINE             = 3,///<�����ݼ�
	GLB_DATASET_FEATURECLASS_MULTILINE        = 4,///<�������ݼ�
	GLB_DATASET_FEATURECLASS_POLYGON          = 5,///<��������ݼ�
	GLB_DATASET_FEATURECLASS_MULTIPOLYGON     = 6,///<���������ݼ�
	GLB_DATASET_FEATURECLASS_TIN              = 7,///<���������ݼ�
	GLB_DATASET_FEATURECLASS_TEXT             = 8,///<��ע���ݼ�
	GLB_DATASET_NETWORK                       = 9,///<�������ݼ�
	GLB_DATASET_RASTER                        = 10,///<դ�����ݼ�
	GLB_DATASET_TERRAIN                       = 11,///<�������ݼ�
	GLB_DATASET_DRILL                         = 12,///<������ݼ�
	GLB_DATASET_SECTION                       = 13,///<�������ݼ�
	GLB_DATASET_FEATURECLASS_MULTITIN         = 14 ///<���������ݼ�

	//GLB_TRISOLID_FEATURECLASS     = 6,///<�������������ݼ�
	//GLB_MODEL_FEATURECLASS        = 7,///<ģ�����ݼ� 
	//GLB_ROUTE_FEATURECLASS        = 9,///<·�����ݼ�
};
/**
  * @brief ������������ö��
  *
  * ���岻ͬ�Ļ�����������
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbDataTypeEnum
{
  GLB_DATATYPE_UNKNOWN  = 0,///<δ֪
  GLB_DATATYPE_BYTE     = 1,///<�ֽ�,�޷��� 8λ
  //GLB_DATATYPE_CHAR     = 2,///<�з��� 8λ
  GLB_DATATYPE_BOOLEAN  = 3,///<����
  GLB_DATATYPE_INT16    = 4,///<������16λ
  GLB_DATATYPE_INT32    = 5,///<��������32λ
  GLB_DATATYPE_INT64    = 6,///<64λ����
  GLB_DATATYPE_FLOAT    = 7,///<�����ȸ�������32�ֽ�
  GLB_DATATYPE_DOUBLE   = 8,///<��������64λ
  GLB_DATATYPE_STRING   = 9,///<�ַ���
  GLB_DATATYPE_DATETIME = 10,///<����+ʱ��
  GLB_DATATYPE_GEOMETRY = 11,///<����
  GLB_DATATYPE_VECTOR   = 12,///<������3��double,xyz��
  GLB_DATATYPE_BLOB     = 13///<�����ƴ�����
};
/**
  * @brief �����ݼ�����ö��
  *
  * ���岻ͬ�������ݼ�����
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbSubDatasetRoleEnum
{
 GLB_SUBDATASETROLE_UNKNOWN     = 0,///<δ����
 //GLB_SUBDATASETROLE_NETWORKEDGE = 1,///<�������ݼ���
 GLB_SUBDATASETROLE_NETWORKNODE = 2,///<�������ݼ��ڵ�
 GLB_SUBDATASETROLE_NETWORKTURN = 3,///<��·���ݼ�ת��
 GLB_SUBDATASETROLE_DRILLSKEW   = 4,///<������ݼ���б
 GLB_SUBDATASETROLE_DRILLSTRA   = 5,///<������ݼ��ֲ�
 GLB_SUBDATASETROLE_DRILLSAMP   = 6,///<������ݼ���Ʒ
 GLB_SUBDATASETROLE_SECCTRL		= 7,///<�������ݼ������Ƶ�
 GLB_SUBDATASETROLE_SECNODE     = 8,///<�������ݼ����ڵ�
 GLB_SUBDATASETROLE_SECARC      = 9,///<�������ݼ�����
 GLB_SUBDATASETROLE_SECPOLY     = 10///<�������ݼ��������
};
/**
  * @brief ��ʩ���� �ڵ�����
*/
enum GlbNetworkNodeTypeEnum
{
	GLB_NETWORK_NORMAL_NODE = 0,///��ͨ�ڵ�
	GLB_NETWORK_SOURCE_NODE = 1,///Դ
	GLB_NETWORK_SINK_NODE   = 2 ///��
};
/**
  * @brief ��ʩ���� ��������
*/
enum GlbNetworkDirectionTypeEnum
{
	GLB_NETWORK_DIRECTION_SAME     = 0,///������ߵ����ֻ�����(F->T)��ͬ
	GLB_NETWORK_DIRECTION_OPPOSITE = 1,///������ߵ����ֻ�����(F->T)�෴
	GLB_NETWORK_DIRECTION_UNCERTAIN= 2,///����ȷ��
	GLB_NETWORK_DIRECTION_DISCONNECT=3 ///����ͨ
};
/**
  * @brief �ռ���������ö��
  *
  * ���岻ͬ�Ŀռ���������
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbSpatialIndexTypeEnum
{
	GLB_SPINDEX_UNKNOWN = 0,///<δ֪����
	GLB_SPINDEX_MGRID   = 1///<�༶��������
};
/**
  * @brief ��������ö��
  *
  * ���岻ͬ�ļ�������
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbGeometryTypeEnum
{
 GLB_GEO_UNKNOWN         = 0,///<δ֪
 GLB_GEO_POINT           = 1,///<��
 GLB_GEO_MULTIPOINT      = 2,///<���
 GLB_GEO_LINE            = 3,///<��
 GLB_GEO_MULTILINE       = 4,///<����
 GLB_GEO_POLYGON         = 5,///<�����
 GLB_GEO_MULTIPOLYGON    = 6,///<������
 GLB_GEO_TIN             = 7,///<������
 GLB_GEO_MULTITIN        = 8,///<��������
 GLB_GEO_TEXT            = 10,///<����
 GLB_GEO_COLLECT         = 11 ///<���μ���
};
/**
  * @brief ������
  *
  */
enum GlbGeometryClassEnum
{
 GLB_GEOCLASS_POINT     = 0,///<�㼸����
 GLB_GEOCLASS_TRIANGLE  = 1 ///<�����μ�����
};

/**
  * @brief ��������ö��
  *
  * ���岻ͬ����������
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbPixelTypeEnum
{
	GLB_PIXELTYPE_UNKNOWN   = 0,	///<"δ֪����"
	GLB_PIXELTYPE_BYTE      = 1,	///<"�ֽ�"			size=1�ֽ�
	GLB_PIXELTYPE_USHORT16  = 2,	///<"�޷���λ����"  size=2�ֽ�
	GLB_PIXELTYPE_SHORT16   = 3,	///<"16λ����"      size=2�ֽ�
	GLB_PIXELTYPE_UINT32    = 4,	///<"�޷���λ����"  size=4�ֽ�
	GLB_PIXELTYPE_INT32     = 5,	///<"32λ����"      size=4�ֽ�
	GLB_PIXELTYPE_FLOAT32   = 6, 	///<"32λ������"    size=4�ֽ�
	GLB_PIXELTYPE_DOUBLE64  = 7 	///<"64λ������"    size=8�ֽ�
};
/**
  * @brief ������������ö��
  *
  * ���岻ͬ�����������ʽ
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbPixelLayoutTypeEnum
{
 GLB_PIXELLAYOUT_UNKNOWN=-1,///<δ֪
 GLB_PIXELLAYOUT_BSQ = 0,  ///< ����˳���ʽ
 GLB_PIXELLAYOUT_BIP = 1,  ///< ��Ԫ���ν���ʽ
 GLB_PIXELLAYOUT_BIL = 2   ///< ���ΰ��н����ʽ
};

/**
  * @brief դ���ز�������ö��
  *
  * ���岻ͬ��դ���ز�����ʽ
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbRasterResampleTypeEnum
{
	GLB_RASTER_NEAREST = 0,///<�ͽ�����
	GLB_RASTER_CUBIC   = 1, ///<��������
	GLB_RASTER_AVER    = 2///<ƽ��ֵ
};

