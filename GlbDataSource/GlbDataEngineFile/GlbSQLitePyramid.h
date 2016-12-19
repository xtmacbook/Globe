/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbSQLitePyramid.h
  * @brief   ������SQLite������ͷ�ļ�
  *
  * �����������CGlbSQLitePyramid�����
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#pragma once
#include "IGlbDataEngine.h"
#include "SQLite.h"
/**
  * @brief CGlbSQLitePyramid��
  *
  * ������SQLite������
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CGlbSQLitePyramid:public CGlbReference
{
public:
	/**
	* @brief ���캯��
	*
	* @return void
	*/
	CGlbSQLitePyramid(void);
	/**
	* @brief ��������
	*
	* @return void
	*/
	~CGlbSQLitePyramid(void);

	/**
	* @brief �����ݿ�
	*
	* @param fullname ���ݿ���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool Open(const glbWChar* fullname);
	/**
	* @brief �������ݿ�
	*
	* @param fullname ���ݿ���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool Create(const glbWChar* fullname);
	/**
	* @brief ��ȡһ����Ƭ
	*
	* @param tileRow ��
	* @param tileCol ��
	* @param level ����
	* @param pixelblock ���ؿ�
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool Read(glbInt32 tileRow,glbInt32 tileCol,glbInt32 level,
		CGlbPixelBlock* pixelblock);
	/**
	* @brief д��һ����Ƭ,������֧�ֶ��߳�
	*
	* @param tileRow ��
	* @param tileCol ��
	* @param level ����
	* @param pixelblock ���ؿ�
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool Write(glbInt32 tileRow,glbInt32 tileCol,glbInt32 level,
		CGlbPixelBlock* pixelblock);
	glbBool UpdatePydInfo();
	/**
	* @brief ��ʼ����
	*
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BeginTransaction();
	/**
	* @brief �ύ����
	*
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool CommitTransaction();
	/**
	* @brief �ع�����
	*
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool RollbackTransaction();
	/**
	* @brief ��������
	*
	* @return true��ʾ�������У�false��ʾû������
	*/
	bool IsInTransaction();

public:
	glbBool                mpr_isGlobe;///<�Ƿ�����
	GlbPixelTypeEnum       mpr_pixeltype;///<��������
	GlbPixelLayoutTypeEnum mpr_pixellayout;///<���ز�������,Ϊ�˿�����ʾ��Ӧ�������ؽ���ģʽ��
	glbInt32               mpr_bandCount;///<������
	glbDouble              mpr_lzts;///<0����Ƭ�ռ��С
	glbInt32               mpr_tileSizeX;///<��Ƭ����x��С
	glbInt32               mpr_tileSizeY;///<��Ƭ����y��С
	glbInt32               mpr_minLevel;///<��С����
	glbInt32               mpr_maxLevel;///<��󼶱�
	glbDouble              mpr_originX;///<ԭ��x����
	glbDouble              mpr_originY;///<ԭ��y����
	glbInt32               mpr_rows;///<��󼶱�ֿ�����
	glbInt32               mpr_cols;///<��󼶱�ֿ�����
	glbBool                mpr_isShareEdge;///<�Ƿ���߽�
	GlbCompressTypeEnum    mpr_compress;//ѹ������
	CGlbWString            mpr_srs;///<������Ϣ
private:
	CSQLite*            mpr_sqlite;///<SQLite���ݿ�
	GlbCriticalSection  mpr_cs;    ///<�ٽ������߳�ͬ��
};

typedef struct _GlbLevelInfo
{
	glbDouble OriginX;///<ԭ��x����
	glbDouble OriginY;///<ԭ��y����
	glbInt32 Rows;///<�ֿ�����
	glbInt32 Cols;///<�ֿ�����
	glbInt32 startRow;
	glbInt32 endRow;
	glbInt32 startCol;
	glbInt32 endCol;
} GlbLevelInfo;//����������Ϣ
/**
  * @brief CGlbTedSQLite��
  *
  * ���ν�����SQLite������
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CGlbTedSQLite:public CGlbReference
{
public:
	/**
	* @brief ���캯��
	*
	* @return void
	*/
	CGlbTedSQLite(void);
	/**
	* @brief ��������
	*
	* @return void
	*/
	~CGlbTedSQLite(void);
	/**
	* @brief �����ݿ�
	*
	* @param fullname ���ݿ���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool Open(const glbWChar* fullname);
	/**
	* @brief �������ݿ�
	*
	* @param fullname ���ݿ���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool Create(const glbWChar* fullname);
	/**
	* @brief ��ȡdomһ����Ƭ
	*
	* @param tileRow ��
	* @param tileCol ��
	* @param level ����
	* @param pixelblock ���ؿ�
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool ReadDom(glbInt32 tileRow,glbInt32 tileCol,glbInt32 level,
		CGlbPixelBlock* pixelblock);
	/**
	* @brief ��ȡdemһ����Ƭ
	*
	* @param tileRow ��
	* @param tileCol ��
	* @param level ����
	* @param pixelblock ���ؿ�
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool ReadDem(glbInt32 tileRow,glbInt32 tileCol,glbInt32 level,
		CGlbPixelBlock* pixelblock);
	/**
	* @brief д��Domһ����Ƭ
	*
	* @param isUpdate �Ƿ����
	* @param tileRow ��
	* @param tileCol ��
	* @param level ����
	* @param pixelblock ���ؿ�
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool WriteDom(glbInt32 tileRow,
					 glbInt32 tileCol,
					 glbInt32 level,
		             CGlbPixelBlock* pixelblock);
	/**
	* @brief д��Demһ����Ƭ
	*
	* @param tileRow ��
	* @param tileCol ��
	* @param level ����
	* @param pixelblock ���ؿ�
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
	glbBool WriteDem(glbInt32 tileRow,
		             glbInt32 tileCol,
					 glbInt32 level,
		             CGlbPixelBlock* pixelblock);	
	/**
	* @brief ��ʼ����
	*
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BeginTransaction();
	/**
	* @brief �ύ����
	*
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool CommitTransaction();
	/**
	* @brief �ع�����
	*
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool RollbackTransaction();
	/**
	* @brief ��������
	*
	* @return true��ʾ�������У�false��ʾû������
	*/
	bool IsInTransaction();
private:
	
	glbBool UpdateDomTileInfo(glbInt32 level, glbInt32 column, glbInt32 row);
	
	glbBool UpdateDemTileInfo(glbInt32 level, glbInt32 column, glbInt32 row);
public:
	glbBool                              mpr_isGlobe;///<�Ƿ�����
	glbDouble                            mpr_lzts;///<0����Ƭ�ռ��С
	glbInt32                             mpr_domMinLevel;///<Ӱ����С����
	glbInt32                             mpr_domMaxLevel;///<Ӱ����󼶱�
	glbInt32                             mpr_demMinLevel;///<�߳���С����
	glbInt32                             mpr_demMaxLevel;///<�߳���󼶱�
	map<glbInt32/*level*/,GlbLevelInfo*> mpr_domLevelInfo;
	map<glbInt32/*level*/,GlbLevelInfo*> mpr_demLevelInfo;
	CGlbWString                          mpr_srs;///<������Ϣ
	GlbCompressTypeEnum                  mpr_domCompress;///Ӱ��ѹ����ʽ
	GlbCompressTypeEnum                  mpr_demCompress;///�߳�ѹ����ʽ
private:
	CSQLite*            mpr_sqlite;  ///<SQLite���ݿ�
	GlbCriticalSection  mpr_cs;      ///<�ٽ������߳�ͬ��
};

glbBool _compress(GlbCompressTypeEnum compress, CGlbPixelBlock* pixelblock, glbByte** outdata, glbUInt32& outlen);