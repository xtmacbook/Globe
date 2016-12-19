/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    SQLite.cpp
  * @brief   SQLite���ݿ������װ�ࡢSQLite�����װ�ࡢSQLite�����ݷ�װ��ʵ���ļ�
  *
  * �����������CSQLite��CSQLiteDataReader��CSQLiteCommand�����ֻ֧࣬��utf8����֧��utf16
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#include "SQLite.h"
#include "GlbError.h"
#include "GlbString.h"

CSQLiteDataReader::CSQLiteDataReader(sqlite3* pMdb,sqlite3_stmt *pStmt):  
mpr_pdb(pMdb),
mpr_pstmt(pStmt)  
{  
  
}  
  
CSQLiteDataReader::~CSQLiteDataReader()  
{  
    Close();  
}  
void CSQLiteDataReader::SetLastError()
{
	if(mpr_pdb)
	{
		CGlbString err = sqlite3_errmsg(mpr_pdb);
		GlbSetLastError(err.ToWString().c_str());
	}
}
// ��ȡһ������  
glbBool CSQLiteDataReader::Read()  
{  
    if(mpr_pstmt == NULL)
    {  
		GlbSetLastError(L"û�г�ʼ��");
        return false;
    }  
    if(sqlite3_step(mpr_pstmt) != SQLITE_ROW)
    {  
		SetLastError();
        return false;
    }  
    return true;
}  
  
// �ر�Reader��reset�����ͷ�����
void CSQLiteDataReader::Close()  
{  
    if(mpr_pstmt != NULL)
    {  
        sqlite3_reset(mpr_pstmt);
		sqlite3_finalize(mpr_pstmt);
		mpr_pstmt = NULL;
		mpr_pdb   = NULL;
    }
}  
  
// �ܵ�����  
glbInt32 CSQLiteDataReader::ColumnCount(void)  
{ 
	if(mpr_pstmt == NULL)
	{  
		return -1;
	}
    return sqlite3_column_count(mpr_pstmt);  
}  

// ��ȡĳ�е�����   
const glbChar* CSQLiteDataReader::GetName(glbInt32 nCol)  
{
	if(mpr_pstmt == NULL)
	{  
		return NULL;
	}
    return sqlite3_column_name(mpr_pstmt, nCol);  
}  
  
// ��ȡĳ�е���������  
GLB_SQLITE_DATATYPE CSQLiteDataReader::GetDataType(glbInt32 nCol)  
{
	if(mpr_pstmt == NULL)
	{  
		return SQLITE_DATATYPE_UNKNOWEN;
	}
    return (GLB_SQLITE_DATATYPE)sqlite3_column_type(mpr_pstmt, nCol);  
}
  
// ��ȡĳ�е�ֵ(�ַ���)  
const glbByte* CSQLiteDataReader::GetStringValue(glbInt32 nCol)
{
	if(mpr_pstmt == NULL)
	{  
		return NULL;
	}
    return sqlite3_column_text(mpr_pstmt, nCol);
}  
  
// ��ȡĳ�е�ֵ(����)
glbInt32 CSQLiteDataReader::GetIntValue(glbInt32 nCol)
{
	if(mpr_pstmt == NULL)
	{  
		return 0;
	}
    return sqlite3_column_int(mpr_pstmt, nCol);
}  
  
// ��ȡĳ�е�ֵ(������)
glbInt64 CSQLiteDataReader::GetInt64Value(glbInt32 nCol)  
{
	if(mpr_pstmt == NULL)
	{  
		return 0;
	}
    return sqlite3_column_int64(mpr_pstmt, nCol);  
}  
  
// ��ȡĳ�е�ֵ(������)
glbDouble CSQLiteDataReader::GetFloatValue(glbInt32 nCol)  
{
	if(mpr_pstmt == NULL)
	{  
		return 0.0;
	}
    return sqlite3_column_double(mpr_pstmt, nCol);  
}  
  
// ��ȡĳ�е�ֵ(����������)
const glbByte* CSQLiteDataReader::GetBlobValue(glbInt32 nCol, glbInt32 &nLen)  
{
	if(mpr_pstmt == NULL)
	{  
		return NULL;
	}
    nLen = sqlite3_column_bytes(mpr_pstmt, nCol);  
    return (const glbByte*)sqlite3_column_blob(mpr_pstmt, nCol);  
}  
  
glbBool CSQLiteDataReader::BindParam(glbInt32 index, const glbChar* szValue)  
{
	if(mpr_pstmt == NULL)
	{  
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
	if( szValue == NULL||index<0)
	{
		GlbSetLastError(L"����Ч����");
		return false;
	}
    if(sqlite3_bind_text(mpr_pstmt, index, szValue,-1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
		SetLastError();
        return false;
    }
    return true;  
}  
  
glbBool CSQLiteDataReader::BindParam(glbInt32 index, glbInt32 nValue)  
{
	if(mpr_pstmt == NULL)
	{  
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
    if(sqlite3_bind_int(mpr_pstmt, index, nValue) != SQLITE_OK)
    {  
		SetLastError();
        return false;
    }  
    return true;  
}  
  
glbBool CSQLiteDataReader::BindParam(glbInt32 index, glbDouble dValue)  
{
	if(mpr_pstmt == NULL)
	{  
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
    if(sqlite3_bind_double(mpr_pstmt, index, dValue) != SQLITE_OK)
    {  
		SetLastError();
        return false;
    }  
    return true;  
}  
  
glbBool CSQLiteDataReader::BindParam(glbInt32 index, const glbByte* blobBuf, glbInt32 nLen)  
{
	if(mpr_pstmt == NULL)
	{  
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
    if(sqlite3_bind_blob(mpr_pstmt, index, blobBuf,nLen,NULL) != SQLITE_OK)
    {  
		SetLastError();
        return false;
    }  
    return true;  
}  
/*******************************************************************************/
CSQLiteCommand::CSQLiteCommand(sqlite3* pMdb):  
mpr_db(pMdb)
{
}
  
CSQLiteCommand::~CSQLiteCommand()  
{  
	if(mpr_stmt != NULL)
    {
        sqlite3_finalize(mpr_stmt);
		mpr_stmt = NULL;
    }
}  
void CSQLiteCommand::SetLastError()
{
	if(mpr_db)
	{
		CGlbString err = sqlite3_errmsg(mpr_db);
		GlbSetLastError(err.ToWString().c_str());
	}
}
glbBool CSQLiteCommand::SetCommandText(const glbChar* lpSql)  
{
	if(mpr_db == NULL)
	{  
		return false;
	}

    if(sqlite3_prepare_v2(mpr_db, lpSql, -1, &mpr_stmt, NULL) != SQLITE_OK)
    {   
		SetLastError();
        return false;
    }
    return true;  
}  
  
glbBool CSQLiteCommand::BindParam(glbInt32 index, const glbChar* szValue)  
{
	if(mpr_stmt == NULL || szValue == NULL)
	{  
		return false;
	}
    if(sqlite3_bind_text(mpr_stmt, index, szValue,-1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
		SetLastError();
        return false;
    }
    return true;  
}  
  
glbBool CSQLiteCommand::BindParam(glbInt32 index, glbInt32 nValue)  
{
	if(mpr_stmt == NULL)
	{  
		return false;
	}
    if(sqlite3_bind_int(mpr_stmt, index, nValue) != SQLITE_OK)
    {  
		SetLastError();
        return false;
    }  
    return true;  
}  
  
glbBool CSQLiteCommand::BindParam(glbInt32 index, glbDouble dValue)  
{
	if(mpr_stmt == NULL)
	{  
		return false;
	}
    if(sqlite3_bind_double(mpr_stmt, index, dValue) != SQLITE_OK)
    {  
		SetLastError();
        return false;
    }  
    return true;  
}  
  
glbBool CSQLiteCommand::BindParam(glbInt32 index, const glbByte* blobBuf, glbInt32 nLen)  
{
	if(mpr_stmt == NULL || blobBuf == NULL)
	{  
		return false;
	}
    if(sqlite3_bind_blob(mpr_stmt, index, blobBuf,nLen,NULL) != SQLITE_OK)
    {  
		SetLastError();
        return false;
    }  
    return true;  
}  
  
glbBool CSQLiteCommand::Excute()
{
	if(mpr_stmt == NULL)
	{  
		return false;
	}
    sqlite3_step(mpr_stmt);
    if(sqlite3_reset(mpr_stmt) != SQLITE_OK)
	{
		SetLastError();
		return false;
	}
	return true;
}

/*********************************************************************************/
  
CSQLite::CSQLite(void):  
mpr_db(NULL),
mpr_is_intran(false)
{
	
}  

CSQLite::~CSQLite(void)
{  
	Close();
}

glbBool CSQLite::Open(const glbChar* lpDbFlie)  
{  
	if(lpDbFlie == NULL)  
	{  
		return false;  
	}  
	if(sqlite3_open(lpDbFlie, &mpr_db) != SQLITE_OK)  
	{  
		SetLastError();
		return false;
	}  
	return true;  
}  

void CSQLite::Close()  
{  
	if(mpr_db != NULL)  
	{  
		sqlite3_close(mpr_db);  
		mpr_db = NULL;  
	}  
}  

void CSQLite::SetLastError()
{
	if(mpr_db)
	{
		CGlbString err = sqlite3_errmsg(mpr_db);
		GlbSetLastError(err.ToWString().c_str());
	}
}

glbBool CSQLite::ExcuteNonQuery(const glbChar* lpSql)  
{  
	if(mpr_db == NULL)
	{
		GlbSetLastError(L"û�д��ļ�");
		return false;
	}
	if(lpSql == NULL)  
	{  
		GlbSetLastError(L"��Ч����");
		return false;  
	}  
	sqlite3_stmt* stmt;    
	if(sqlite3_prepare_v2(mpr_db, lpSql, -1, &stmt, NULL) != SQLITE_OK)  
	{   
		SetLastError();
		return false;    
	}    
	sqlite3_step(stmt);  

	if(sqlite3_finalize(stmt) != SQLITE_OK)
	{
		SetLastError();
		return false;    
	}
	return true;
}  


// ��ѯ  
CSQLiteDataReader* CSQLite::ExcuteQuery(const glbChar* lpSql)  
{  
	if(mpr_db == NULL)
	{
		GlbSetLastError(L"û�д��ļ�");
		return NULL;
	}
	if(lpSql == NULL)  
	{  
		GlbSetLastError(L"��Ч����");
		return NULL;  
	}  
	sqlite3_stmt* stmt;
	if(sqlite3_prepare_v2(mpr_db, lpSql, -1, &stmt, NULL) != SQLITE_OK)
    {   
		SetLastError();
        return NULL;
    }
	return new CSQLiteDataReader(mpr_db,stmt);
}  

// ��ʼ����  
glbBool CSQLite::BeginTransaction()  
{
	if(mpr_db == NULL)
	{
		GlbSetLastError(L"û�д��ļ�");
		return false;
	}
	if (mpr_is_intran)
	{
		GlbSetLastError(L"��֧��Ƕ��");
		return false;
	}
	if(sqlite3_exec(mpr_db,"BEGIN TRANSACTION;",NULL,NULL,NULL) != SQLITE_OK)
	{  
		SetLastError();
		return false;
	}
	mpr_is_intran = true;
	return true;
}  

// �ύ����  
glbBool CSQLite::CommitTransaction()  
{
	if(mpr_db == NULL)
	{
		GlbSetLastError(L"û�д��ļ�");
		return false;
	}
	if (!mpr_is_intran)
	{
		GlbSetLastError(L"û������");
		return false;
	}
	if(sqlite3_exec(mpr_db,"COMMIT TRANSACTION;",NULL,NULL,NULL) != SQLITE_OK)  
	{  
		SetLastError();
		return false;  
	}
	mpr_is_intran = false;
	return true;  
}  

// �ع�����  
glbBool CSQLite::RollbackTransaction()  
{
	if(mpr_db == NULL)
	{
		GlbSetLastError(L"û�д��ļ�");
		return false;
	}
	if (!mpr_is_intran)
	{
		GlbSetLastError(L"û�п������񣬲���Ҫ�ع�����");
		return false;
	}
	if(sqlite3_exec(mpr_db,"ROLLBACK  TRANSACTION;",NULL,NULL,NULL) != SQLITE_OK)  
	{  
		SetLastError();
		return false;  
	}
	mpr_is_intran = false;
	return true;  
}  

//��������
bool CSQLite::IsInTransaction()
{
	return mpr_is_intran;
}
