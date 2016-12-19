/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    SQLite.h
  * @brief   SQLite���ݿ������װ�ࡢSQLite�����װ�ࡢSQLite�����ݷ�װ��ͷ�ļ�
  *
  * �����������CSQLite��CSQLiteDataReader��CSQLiteCommand�����ֻ֧࣬��utf8����֧��utf16
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#pragma once  
#include <windows.h> 
#include "sqlite3.h"
#include "GlbCommTypes.h"  

/**
  * @brief GLB_SQLITE_DATATYPE��������ö����
  *					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
typedef enum GLB_SQLITE_DATATYPE
{
	SQLITE_DATATYPE_UNKNOWEN = 0,///<δ֪��������
    SQLITE_DATATYPE_INTEGER  = SQLITE_INTEGER,///<��������
    SQLITE_DATATYPE_FLOAT    = SQLITE_FLOAT,///<��������
    SQLITE_DATATYPE_TEXT     = SQLITE_TEXT,///<�ַ�������
    SQLITE_DATATYPE_BLOB     = SQLITE_BLOB,///<blob����
    SQLITE_DATATYPE_NULL     = SQLITE_NULL///<null����
};  
  
/**
  * @brief CSQLiteDataReader��
  *
  * SQLite��������
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CSQLiteDataReader  
{  
public:
	/**
	* @brief ���캯��
	*
	* @param pStmt SQLite����
	* @return void
	*/
    CSQLiteDataReader(sqlite3* pMdb,sqlite3_stmt *pStmt);
	/**
	* @brief ��������
	*
	* @return void
	*/
    ~CSQLiteDataReader();  
public:  
	/**
	* @brief ��ȡһ������
	*
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool Read();
	/**
	* @brief �ر�Reader����ȡ���������
	*
	* @return void
	*/
    void Close();  
	/**
	* @brief ��ȡ�ܵ�����
	*
	* @return �ɹ�����������ʧ�ܷ���-1
	*/
    glbInt32 ColumnCount(void);  
    // ��ȡĳ�е�����   
    const glbChar* GetName(glbInt32 nCol);  
    // ��ȡĳ�е���������  
    GLB_SQLITE_DATATYPE GetDataType(glbInt32 nCol);  
    // ��ȡĳ�е�ֵ(�ַ���)
    const glbByte* GetStringValue(glbInt32 nCol);  
    // ��ȡĳ�е�ֵ(����)  
    glbInt32 GetIntValue(glbInt32 nCol);
    // ��ȡĳ�е�ֵ(������)  
    glbInt64 GetInt64Value(glbInt32 nCol);  
    // ��ȡĳ�е�ֵ(������)  
    glbDouble GetFloatValue(glbInt32 nCol);  
    // ��ȡĳ�е�ֵ(����������)  
    const glbByte* GetBlobValue(glbInt32 nCol, glbInt32 &nLen);
	/**
	* @brief ���ַ�������
	*
	* @param index Ҫ�󶨲�������ţ���1��ʼ
	* @param szValue ���ַ���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BindParam(glbInt32 index, const glbChar* szValue);
	/**
	* @brief ����������
	*
	* @param index Ҫ�󶨲�������ţ���1��ʼ
	* @param szValue ������
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BindParam(glbInt32 index, glbInt32 nValue);
	/**
	* @brief �󶨸���������
	*
	* @param index Ҫ�󶨲�������ţ���1��ʼ
	* @param szValue �󶨸�����
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BindParam(glbInt32 index, glbDouble dValue);
	/**
	* @brief ��blob����
	*
	* @param index Ҫ�󶨲�������ţ���1��ʼ
	* @param blobValue ��blob����
	* @param nLen ��blob���ݳ���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BindParam(glbInt32 index, const glbByte* blobValue, glbInt32 nLen); 
private:
	void SetLastError();
private:  
    sqlite3_stmt* mpr_pstmt;
	sqlite3*      mpr_pdb;
};  
/**
  * @brief CSQLiteCommand��
  *
  * SQLite������
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CSQLiteCommand
{
public:
	/**
	* @brief ���캯��
	*
	* @return void
	*/
    CSQLiteCommand(sqlite3* pMdb);
	/**
	* @brief ��������
	*
	* @return void
	*/
    ~CSQLiteCommand();
public:  
	/**
	* @brief ��������
	*
	* @param lpSql sql���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool SetCommandText(const glbChar* lpSql);
	/**
	* @brief ���ַ�������
	*
	* @param index Ҫ�󶨲�������ţ���1��ʼ
	* @param szValue ���ַ���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BindParam(glbInt32 index, const glbChar* szValue);
	/**
	* @brief ����������
	*
	* @param index Ҫ�󶨲�������ţ���1��ʼ
	* @param szValue ������
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BindParam(glbInt32 index, glbInt32 nValue);
	/**
	* @brief �󶨸���������
	*
	* @param index Ҫ�󶨲�������ţ���1��ʼ
	* @param szValue �󶨸�����
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BindParam(glbInt32 index, glbDouble dValue);
	/**
	* @brief ��blob����
	*
	* @param index Ҫ�󶨲�������ţ���1��ʼ
	* @param blobValue ��blob����
	* @param nLen ��blob���ݳ���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool BindParam(glbInt32 index, const glbByte* blobValue, glbInt32 nLen); 
	/**
	* @brief ִ��insert\update\delete�ȷǷ��ؽ������
	*
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool Excute();
private:
	void SetLastError();
private:
    sqlite3*      mpr_db;    ///<SQLite��
    sqlite3_stmt* mpr_stmt;  ///<SQLite����
};  
/**
  * @brief CSQLite��
  *
  * SQLite���ݿ���
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CSQLite
{
public:
	/**
	* @brief ���캯��
	*
	* @return void
	*/
    CSQLite(void);
	/**
	* @brief ��������
	*
	* @return void
	*/
    ~CSQLite(void);
public: 
	/**
	* @brief �����ݿ�
	*
	* @param lpDbFlie ���ݿ�������·��
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool Open(const glbChar* lpDbFlie);

	/**
	* @brief �ر����ݿ⣬�ͷ���Դ
	*
	* @return void
	*/
    void Close();
  
	/**
	* @brief ִ�зǲ�ѯ���������롢���»�ɾ����
	*
	* @param lpSql sql���
	* @return true��ʾ�ɹ���false��ʾʧ��
	*/
    glbBool ExcuteNonQuery(const glbChar* lpSql);
	
  
	/**
	* @brief ִ�в�ѯ����
	*
	* @param lpSql ִ������
	* @return �������ݲ鿴��
	*/
    CSQLiteDataReader* ExcuteQuery(const glbChar* lpSql);
  
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
	sqlite3* GetSqlite(){return mpr_db;}
private:
	void SetLastError();
private:
    sqlite3 *mpr_db;       ///<SQLite���ݿ�
	glbBool  mpr_is_intran;///<�Ƿ�������
};
