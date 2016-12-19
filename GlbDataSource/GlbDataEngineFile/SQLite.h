/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    SQLite.h
  * @brief   SQLite数据库操作封装类、SQLite命令封装类、SQLite读数据封装类头文件
  *
  * 这个档案定义CSQLite、CSQLiteDataReader、CSQLiteCommand三个类，只支持utf8，不支持utf16
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
  * @brief GLB_SQLITE_DATATYPE数据类型枚举类
  *					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
typedef enum GLB_SQLITE_DATATYPE
{
	SQLITE_DATATYPE_UNKNOWEN = 0,///<未知数据类型
    SQLITE_DATATYPE_INTEGER  = SQLITE_INTEGER,///<整型数据
    SQLITE_DATATYPE_FLOAT    = SQLITE_FLOAT,///<浮点数据
    SQLITE_DATATYPE_TEXT     = SQLITE_TEXT,///<字符串数据
    SQLITE_DATATYPE_BLOB     = SQLITE_BLOB,///<blob数据
    SQLITE_DATATYPE_NULL     = SQLITE_NULL///<null数据
};  
  
/**
  * @brief CSQLiteDataReader类
  *
  * SQLite读操作类
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CSQLiteDataReader  
{  
public:
	/**
	* @brief 构造函数
	*
	* @param pStmt SQLite命令
	* @return void
	*/
    CSQLiteDataReader(sqlite3* pMdb,sqlite3_stmt *pStmt);
	/**
	* @brief 析构函数
	*
	* @return void
	*/
    ~CSQLiteDataReader();  
public:  
	/**
	* @brief 读取一行数据
	*
	* @return true表示成功，false表示失败
	*/
    glbBool Read();
	/**
	* @brief 关闭Reader，读取结束后调用
	*
	* @return void
	*/
    void Close();  
	/**
	* @brief 获取总的列数
	*
	* @return 成功返回列数，失败返回-1
	*/
    glbInt32 ColumnCount(void);  
    // 获取某列的名称   
    const glbChar* GetName(glbInt32 nCol);  
    // 获取某列的数据类型  
    GLB_SQLITE_DATATYPE GetDataType(glbInt32 nCol);  
    // 获取某列的值(字符串)
    const glbByte* GetStringValue(glbInt32 nCol);  
    // 获取某列的值(整形)  
    glbInt32 GetIntValue(glbInt32 nCol);
    // 获取某列的值(长整形)  
    glbInt64 GetInt64Value(glbInt32 nCol);  
    // 获取某列的值(浮点形)  
    glbDouble GetFloatValue(glbInt32 nCol);  
    // 获取某列的值(二进制数据)  
    const glbByte* GetBlobValue(glbInt32 nCol, glbInt32 &nLen);
	/**
	* @brief 绑定字符串参数
	*
	* @param index 要绑定参数的序号，从1开始
	* @param szValue 绑定字符串
	* @return true表示成功，false表示失败
	*/
    glbBool BindParam(glbInt32 index, const glbChar* szValue);
	/**
	* @brief 绑定整数参数
	*
	* @param index 要绑定参数的序号，从1开始
	* @param szValue 绑定整数
	* @return true表示成功，false表示失败
	*/
    glbBool BindParam(glbInt32 index, glbInt32 nValue);
	/**
	* @brief 绑定浮点数参数
	*
	* @param index 要绑定参数的序号，从1开始
	* @param szValue 绑定浮点数
	* @return true表示成功，false表示失败
	*/
    glbBool BindParam(glbInt32 index, glbDouble dValue);
	/**
	* @brief 绑定blob参数
	*
	* @param index 要绑定参数的序号，从1开始
	* @param blobValue 绑定blob数据
	* @param nLen 绑定blob数据长度
	* @return true表示成功，false表示失败
	*/
    glbBool BindParam(glbInt32 index, const glbByte* blobValue, glbInt32 nLen); 
private:
	void SetLastError();
private:  
    sqlite3_stmt* mpr_pstmt;
	sqlite3*      mpr_pdb;
};  
/**
  * @brief CSQLiteCommand类
  *
  * SQLite命令类
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CSQLiteCommand
{
public:
	/**
	* @brief 构造函数
	*
	* @return void
	*/
    CSQLiteCommand(sqlite3* pMdb);
	/**
	* @brief 析构函数
	*
	* @return void
	*/
    ~CSQLiteCommand();
public:  
	/**
	* @brief 设置命令
	*
	* @param lpSql sql语句
	* @return true表示成功，false表示失败
	*/
    glbBool SetCommandText(const glbChar* lpSql);
	/**
	* @brief 绑定字符串参数
	*
	* @param index 要绑定参数的序号，从1开始
	* @param szValue 绑定字符串
	* @return true表示成功，false表示失败
	*/
    glbBool BindParam(glbInt32 index, const glbChar* szValue);
	/**
	* @brief 绑定整数参数
	*
	* @param index 要绑定参数的序号，从1开始
	* @param szValue 绑定整数
	* @return true表示成功，false表示失败
	*/
    glbBool BindParam(glbInt32 index, glbInt32 nValue);
	/**
	* @brief 绑定浮点数参数
	*
	* @param index 要绑定参数的序号，从1开始
	* @param szValue 绑定浮点数
	* @return true表示成功，false表示失败
	*/
    glbBool BindParam(glbInt32 index, glbDouble dValue);
	/**
	* @brief 绑定blob参数
	*
	* @param index 要绑定参数的序号，从1开始
	* @param blobValue 绑定blob数据
	* @param nLen 绑定blob数据长度
	* @return true表示成功，false表示失败
	*/
    glbBool BindParam(glbInt32 index, const glbByte* blobValue, glbInt32 nLen); 
	/**
	* @brief 执行insert\update\delete等非返回结果命令
	*
	* @return true表示成功，false表示失败
	*/
    glbBool Excute();
private:
	void SetLastError();
private:
    sqlite3*      mpr_db;    ///<SQLite类
    sqlite3_stmt* mpr_stmt;  ///<SQLite命令
};  
/**
  * @brief CSQLite类
  *
  * SQLite数据库类
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-12 14:40
  */
class CSQLite
{
public:
	/**
	* @brief 构造函数
	*
	* @return void
	*/
    CSQLite(void);
	/**
	* @brief 析构函数
	*
	* @return void
	*/
    ~CSQLite(void);
public: 
	/**
	* @brief 打开数据库
	*
	* @param lpDbFlie 数据库名，带路径
	* @return true表示成功，false表示失败
	*/
    glbBool Open(const glbChar* lpDbFlie);

	/**
	* @brief 关闭数据库，释放资源
	*
	* @return void
	*/
    void Close();
  
	/**
	* @brief 执行非查询操作（插入、更新或删除）
	*
	* @param lpSql sql语句
	* @return true表示成功，false表示失败
	*/
    glbBool ExcuteNonQuery(const glbChar* lpSql);
	
  
	/**
	* @brief 执行查询操作
	*
	* @param lpSql 执行命令
	* @return 返回数据查看类
	*/
    CSQLiteDataReader* ExcuteQuery(const glbChar* lpSql);
  
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
	sqlite3* GetSqlite(){return mpr_db;}
private:
	void SetLastError();
private:
    sqlite3 *mpr_db;       ///<SQLite数据库
	glbBool  mpr_is_intran;///<是否开启事务
};
