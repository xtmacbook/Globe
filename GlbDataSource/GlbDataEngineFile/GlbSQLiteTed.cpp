#pragma warning(disable:4251)
#include "stdafx.h"
#include "GlbSQLitePyramid.h"
#include "GlbString.h"
#include "GlbPath.h"
#include "GlbError.h"
#include "GlbCompress.h"

CGlbTedSQLite::CGlbTedSQLite( void )
{
	mpr_isGlobe     = true;
	mpr_lzts        = 36.0;
	mpr_domMinLevel = INT_MAX;
	mpr_domMaxLevel = INT_MIN;
	mpr_demMinLevel = INT_MAX;
	mpr_demMaxLevel = INT_MIN;
	mpr_srs         = L"";
	mpr_sqlite      = new CSQLite();
}

CGlbTedSQLite::~CGlbTedSQLite( void )
{
	if(mpr_sqlite != NULL)
		delete mpr_sqlite;
	
	map<glbInt32/*level*/,GlbLevelInfo*>::iterator itr = mpr_domLevelInfo.begin();
	while (itr != mpr_domLevelInfo.end())
	{
		if (itr->second != NULL)
		{
			delete itr->second;
			itr->second = NULL;
		}
		itr++;
	}

	itr = mpr_demLevelInfo.begin();
	while (itr != mpr_demLevelInfo.end())
	{
		if (itr->second != NULL)
		{
			delete itr->second;
			itr->second = NULL;
		}
		itr++;
	}
}

glbBool CGlbTedSQLite::Open( const glbWChar* fullname )
{
	CGlbWString dbname = fullname;
	
	if(CGlbPath::FileExist(fullname) == NULL)
	{
		GlbSetLastError(L"文件不存在");
		return false;
	}
	if(!mpr_sqlite->Open(dbname.ToUTF8String().c_str()))
	{
		GlbSetLastError(L"打开文件失败");
		return false;
	}
	
	CGlbString strsql         = "select * from GlbTedInfo;";
	CSQLiteDataReader* Reader = mpr_sqlite->ExcuteQuery(strsql.c_str());
	if (Reader==NULL || !Reader->Read())
	{
		if(Reader==NULL)delete Reader;
		GlbSetLastError(L"不是地形数据集");
		return false;
	}
	if (Reader->GetIntValue(0) == 0)
		mpr_isGlobe = false;
	else
		mpr_isGlobe = true;
	mpr_lzts        = Reader->GetFloatValue(1);
	mpr_domMinLevel = Reader->GetIntValue(2);
	mpr_domMaxLevel = Reader->GetIntValue(3);
		
	mpr_demMinLevel = Reader->GetIntValue(4);
	mpr_demMaxLevel = Reader->GetIntValue(5);

	mpr_domCompress = (GlbCompressTypeEnum)Reader->GetIntValue(6);
	mpr_demCompress = (GlbCompressTypeEnum)Reader->GetIntValue(7);

	CGlbString srs = (const glbChar*)Reader->GetStringValue(8);
	mpr_srs = srs.ToWString();
	delete Reader;Reader=NULL;

	mpr_domLevelInfo.clear();
	strsql    = "select * from GlbTedDomLevelsInfo;";
	Reader    = mpr_sqlite->ExcuteQuery(strsql.c_str());
	if(Reader == NULL)
	{
		GlbSetLastError(L"不是地形数据集");
		return false;
	}
	while (Reader && Reader->Read())//允许为空
	{
		glbInt32 iLevel         = Reader->GetIntValue(0);
		GlbLevelInfo* levelinfo = new GlbLevelInfo;
		
		levelinfo->OriginX = Reader->GetFloatValue(1);
		levelinfo->OriginY = Reader->GetFloatValue(2);
		levelinfo->Rows    = Reader->GetIntValue(3);
		levelinfo->Cols    = Reader->GetIntValue(4);
		{
			strsql =CGlbString::FormatString("select min(GlbRow),max(GlbRow),min(GlbCol),max(GlbCol) from GlbDomTiles where GlbLevel=%ld",iLevel);
			CSQLiteDataReader* select_reader = mpr_sqlite->ExcuteQuery(strsql.c_str());
			if(select_reader && select_reader->Read())
			{
				levelinfo->startRow = select_reader->GetIntValue(0);
				levelinfo->endRow   = select_reader->GetIntValue(1);
				levelinfo->startCol = select_reader->GetIntValue(2);
				levelinfo->endCol   = select_reader->GetIntValue(3);			
				delete select_reader;		
			}
		}
		mpr_domLevelInfo[iLevel] = levelinfo;
	}
	delete Reader;
	
	mpr_demLevelInfo.clear();
	strsql    = "select * from GlbTedDemLevelsInfo;";
	Reader    = mpr_sqlite->ExcuteQuery(strsql.c_str());
	if(Reader == NULL)
	{
		GlbSetLastError(L"不是地形数据集");
		return false;
	}
	while (Reader && Reader->Read())//允许为空
	{
		glbInt32 iLevel         = Reader->GetIntValue(0);
		GlbLevelInfo* levelinfo = new GlbLevelInfo;

		levelinfo->OriginX = Reader->GetFloatValue(1);
		levelinfo->OriginY = Reader->GetFloatValue(2);
		levelinfo->Rows    = Reader->GetIntValue(3);
		levelinfo->Cols    = Reader->GetIntValue(4);
		{
			strsql =CGlbString::FormatString("select min(GlbRow),max(GlbRow),min(GlbCol),max(GlbCol) from GlbDemTiles where GlbLevel=%ld",iLevel);			
			CSQLiteDataReader* select_reader = mpr_sqlite->ExcuteQuery(strsql.c_str());
			if(select_reader && select_reader->Read())
			{
				levelinfo->startRow = select_reader->GetIntValue(0);
				levelinfo->endRow   = select_reader->GetIntValue(1);
				levelinfo->startCol = select_reader->GetIntValue(2);
				levelinfo->endCol   = select_reader->GetIntValue(3);
				delete select_reader;		
			}
		}
		mpr_demLevelInfo[iLevel] = levelinfo;
	}
	delete Reader;
	return true;
}

glbBool CGlbTedSQLite::Create( const glbWChar* fullname )
{
	CGlbWString dbname = fullname;
	if(CGlbPath::FileExist(fullname))
	{
		GlbSetLastError(L"文件已存在");
		return false;
	}
	//创建数据库
	if(!mpr_sqlite->Open(dbname.ToUTF8String().c_str()))//UTF8编码的
	{
		GlbSetLastError(L"创建地形文件失败");
		return false;
	}
	//创建表GlbTedInfo
	CGlbString strsql = "create table [GlbTedInfo](";
	strsql += "[GlbMode]        INTEGER, [GlbLZTS]        REAL,";
	strsql += "[GlbDomMinLevel] INTEGER, [GlbDomMaxLevel] INTEGER,";
	strsql += "[GlbDemMinLevel] INTEGER, [GlbDemMaxLevel] INTEGER,";
	strsql += "[GlbDomCompress] INTEGER, [GlbDemCompress] INTEGER,";
	strsql += "[GlbSRS]         TEXT);";
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))
	{
		mpr_sqlite->Close();
		delete mpr_sqlite;
		mpr_sqlite = NULL;
		
		CGlbPath::DeleteFile(fullname);
		return false;
	}
	//插入GlbTedInfo
	strsql = CGlbString::FormatString("insert into GlbTedInfo values(%d,%lf,%d,%d,%d,%d,%d,%d,'%s');",
		                               mpr_isGlobe,    mpr_lzts,       mpr_domMinLevel,
									   mpr_domMaxLevel,mpr_demMinLevel,mpr_demMaxLevel,
									   mpr_domCompress,mpr_demCompress,mpr_srs.ToString().c_str());
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
	{
		mpr_sqlite->Close();
		delete mpr_sqlite;
		mpr_sqlite = NULL;
		
		CGlbPath::DeleteFile(fullname);
		return false;
	}
	//创建表GlbTedDomLevelsInfo
	strsql = "create table [GlbTedDomLevelsInfo]([GlbLevel] INTEGER,";
	strsql += "[GlbOriginX] REAL,[GlbOriginY] REAL,[GlbRows] INTEGER,[GlbCols] INTEGER);";
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
	{
		mpr_sqlite->Close();
		delete mpr_sqlite;
		mpr_sqlite = NULL;
		
		CGlbPath::DeleteFile(fullname);
		return false;
	}
	//创建表GlbTedDemLevelsInfo
	strsql = "create table [GlbTedDemLevelsInfo]([GlbLevel] INTEGER, ";
	strsql += "[GlbOriginX] REAL,[GlbOriginY] REAL,[GlbRows] INTEGER,[GlbCols] INTEGER);";
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
	{
		mpr_sqlite->Close();
		delete mpr_sqlite;
		mpr_sqlite = NULL;
		
		CGlbPath::DeleteFile(fullname);
		return false;
	}
	//创建表GlbDomTiles
	strsql = "create table [GlbDomTiles]([GlbLevel] INTEGER,[GlbRow] INTEGER,[GlbCol] INTEGER,";
	strsql += "[GlbData] BLOB,[GlbMask] BLOB, primary key([GlbLevel],[GlbRow],[GlbCol]));";
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
	{  
		mpr_sqlite->Close();
		delete mpr_sqlite;
		mpr_sqlite = NULL;
		
		CGlbPath::DeleteFile(fullname);
		return false;
	}
	//创建表GlbDemTiles
	strsql = "create table [GlbDemTiles]([GlbLevel] INTEGER,[GlbRow] INTEGER,[GlbCol] INTEGER,";
	strsql += "[GlbData] BLOB,[GlbMask] BLOB, primary key([GlbLevel],[GlbRow],[GlbCol]));";
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
	{  
		mpr_sqlite->Close();
		delete mpr_sqlite;
		mpr_sqlite = NULL;
		
		CGlbPath::DeleteFile(fullname);
		return false;
	}
	return true;
}

glbBool CGlbTedSQLite::ReadDom( glbInt32 tileRow,glbInt32 tileCol,glbInt32 level, CGlbPixelBlock* pixelblock )
{
	CGlbString strsql = CGlbString::FormatString
		                ("select GlbData,GlbMask from GlbDomTiles where GlbLevel=%d and GlbRow=%d and GlbCol=%d;"
		                  ,level,tileRow,tileCol                       
		                );
	CSQLiteDataReader* reader = mpr_sqlite->ExcuteQuery(strsql.c_str());
	if (reader == NULL || !reader->Read())
	{
		delete reader;		
		return false;
	}
	
	glbByte* pdata = NULL;
	glbInt32 plen = pixelblock->GetPixelData(&pdata);
	//像素data
	glbInt32 nlen;
	glbByte* data = const_cast<glbByte*>(reader->GetBlobValue(0, nlen));
	glbInt32 rows,cols;
	pixelblock->GetSize(cols,rows);
	//解压
	if (mpr_domCompress == GLB_COMPRESSTYPE_JPEG)
	{
		glbUInt32 inlen   = *(glbUInt32*)data;
		glbByte*  tempRGB = NULL;
		glbUInt32 lenRGB;
		CGlbCompress::UnCompress(GLB_COMPRESSTYPE_JPEG, data+4, inlen, &tempRGB, lenRGB);//jpeg
		glbByte*  tempA = NULL;
		glbUInt32 lenA;
		CGlbCompress::UnCompress(GLB_COMPRESSTYPE_ZLIB, data+4+inlen, nlen-4-inlen, &tempA, lenA);//zlib
		for (glbInt32 i=0;i<rows;i++)
		{
			for (glbInt32 j=0;j<cols;j++)
			{
				memcpy(pdata+(i*cols+j)*4, tempRGB+(i*cols+j)*3, 3);
				*(pdata+(i*cols+j)*4+3) = *(tempA + i*cols+j); 
			}
		}
		free(tempRGB);
		free(tempA);
	}
	else if (mpr_domCompress == GLB_COMPRESSTYPE_ZLIB)
	{
		glbByte*  temp = NULL;
		glbUInt32 len;
		CGlbCompress::UnCompress(GLB_COMPRESSTYPE_ZLIB, data, nlen, &temp, len);//zlib
		memcpy(pdata, temp, len);
		free(temp);
	}
	else if (mpr_domCompress == GLB_COMPRESSTYPE_NO)
	{
		memcpy(pdata, data, nlen);
	}

	//标记mask
	data = const_cast<glbByte*>(reader->GetBlobValue(1, nlen));
	if (data!=NULL)
	{
		//解压
		glbByte*  outdata = NULL;
		glbUInt32 outlen  = 0;
		CGlbCompress::UnCompress(GLB_COMPRESSTYPE_ZLIB, data, nlen, &outdata, outlen);
		plen = pixelblock->GetMaskData(&pdata);
		memcpy(pdata, outdata, outlen);
		free(outdata);
	}
	delete reader;
	return true;
}

glbBool CGlbTedSQLite::ReadDem( glbInt32 tileRow,glbInt32 tileCol,glbInt32 level, CGlbPixelBlock* pixelblock )
{
	glbInt32 rows,cols;
	pixelblock->GetSize(cols, rows);
	CGlbString strsql = CGlbString::FormatString
		               ("select GlbData,GlbMask from GlbDemTiles where GlbLevel=%d and GlbRow=%d and GlbCol=%d;"
	                     ,level,tileRow,tileCol);
	CSQLiteDataReader* reader = mpr_sqlite->ExcuteQuery(strsql.c_str());
	if (reader == NULL || !reader->Read())
	{
		delete reader;		
		return false;
	}

	glbByte* pdata = NULL;
	glbInt32 plen = pixelblock->GetPixelData(&pdata);
	//像素data
	glbInt32 nlen;
	glbByte* data = const_cast<glbByte*>(reader->GetBlobValue(0, nlen));
	//解压
	if (mpr_demCompress == GLB_COMPRESSTYPE_ZLIB)
	{
		glbByte*  tempdata = NULL;
		glbUInt32 templen;
		CGlbCompress::UnCompress(GLB_COMPRESSTYPE_ZLIB, data, nlen, &tempdata, templen);//zlib
		memcpy(pdata, tempdata, templen);
		free(tempdata);
	}
	else if (mpr_demCompress == GLB_COMPRESSTYPE_NO)
	{
		memcpy(pdata, data, nlen);
	}
	
	//标记mask
	data = const_cast<glbByte*>(reader->GetBlobValue(1, nlen));
	if (data!=NULL)
	{
		//解压
		glbByte*  outdata = NULL;
		glbUInt32 outlen = 0;
		CGlbCompress::UnCompress(GLB_COMPRESSTYPE_ZLIB, data, nlen, &outdata, outlen);
		plen = pixelblock->GetMaskData(&pdata);
		memcpy(pdata, outdata, outlen);
		free(outdata);
	}
	delete reader;
	return true;
}

glbBool CGlbTedSQLite::WriteDom
(glbInt32 tileRow,
 glbInt32 tileCol,
 glbInt32 level, 
 CGlbPixelBlock* pixelblock )
{
	glbInt32 rows,cols;
	pixelblock->GetSize(cols, rows);
	
	GlbScopedLock<GlbCriticalSection> lock(mpr_cs);
	
	CGlbString strsql = "";
	{//删除已存在的
		strsql = CGlbString::FormatString("delete from GlbDomTiles where GlbLevel=%d and GlbRow=%d and GlbCol=%d;",
			level, tileRow, tileCol);
		mpr_sqlite->ExcuteNonQuery(strsql.c_str());
	}

	strsql =  "insert into GlbDomTiles(GlbLevel,GlbRow,GlbCol,GlbData,GlbMask)";
	strsql += " values(?1,?2,?3,?4,?5);";
	CSQLiteCommand insertcmd(mpr_sqlite->GetSqlite());
	if(!insertcmd.SetCommandText(strsql.c_str()))
		return false;
	
	//绑定
	insertcmd.BindParam(1, level);
	insertcmd.BindParam(2, tileRow);
	insertcmd.BindParam(3, tileCol);

	glbByte* pdata = NULL;
	glbInt32 plen  = pixelblock->GetPixelData(&pdata);
	//压缩
	glbByte*  outdata = NULL;
	glbUInt32 outlen = 0;
	if(!_compress(mpr_domCompress, pixelblock, &outdata, outlen))		
		return false;
	insertcmd.BindParam(4, outdata, outlen);

	glbByte* outmaskdata = NULL;
	glbUInt32 outmasklen = 0;
	//标记mask
	plen = pixelblock->GetMaskData(&pdata);
	//压缩
	CGlbCompress::Compress(GLB_COMPRESSTYPE_ZLIB, pdata, plen, &outmaskdata, outmasklen);
	insertcmd.BindParam(5, outmaskdata, outmasklen);

	if(!insertcmd.Excute())
	{
		//释放绑定的内存
		free(outdata);
		free(outmaskdata);
		return false;
	}
	
	//释放绑定的内存
	free(outdata);
	free(outmaskdata);	
	UpdateDomTileInfo(level, tileCol, tileRow);	
	return true;
}

glbBool CGlbTedSQLite::WriteDem
( glbInt32 tileRow,
  glbInt32 tileCol,
  glbInt32 level, 
  CGlbPixelBlock* pixelblock 
)
{
	glbInt32 rows,cols;
	pixelblock->GetSize(cols, rows);

	GlbScopedLock<GlbCriticalSection> lock(mpr_cs);
	
	CGlbString strsql = "";
	{//删除可能存在的
		strsql = CGlbString::FormatString("delete from GlbDemTiles where GlbLevel=%d and GlbRow=%d and GlbCol=%d;",
			level, tileRow, tileCol);
		mpr_sqlite->ExcuteNonQuery(strsql.c_str());
	}

	strsql = "insert into GlbDemTiles(GlbLevel,GlbRow,GlbCol,GlbData,GlbMask)";
	strsql +=" values(?1,?2,?3,?4,?5);";

	CSQLiteCommand insertcmd(mpr_sqlite->GetSqlite());
	if(!insertcmd.SetCommandText(strsql.c_str()))
		return false;
	//绑定
	insertcmd.BindParam(1, level);
	insertcmd.BindParam(2, tileRow);
	insertcmd.BindParam(3, tileCol);

	glbByte* pdata = NULL;
	glbInt32 plen  = pixelblock->GetPixelData(&pdata);
	//压缩
	glbByte* outdata = NULL;
	glbUInt32 outlen = 0;
	if(!_compress(mpr_demCompress, pixelblock, &outdata, outlen))
		return false;
	insertcmd.BindParam(4, outdata, outlen);

	glbByte*  outmaskdata = NULL;
	glbUInt32 outmasklen  = 0;
	plen = pixelblock->GetMaskData(&pdata);
	//压缩
	CGlbCompress::Compress(GLB_COMPRESSTYPE_ZLIB, pdata, plen, &outmaskdata, outmasklen);
	insertcmd.BindParam(5, outmaskdata, outmasklen);

	if(!insertcmd.Excute())
	{
		//释放绑定的内存
		free(outdata);
		free(outmaskdata);
		return false;
	}
	
	//释放绑定的内存
	free(outdata);
	free(outmaskdata);
	UpdateDemTileInfo(level, tileCol, tileRow);
	return true;
}

glbBool CGlbTedSQLite::UpdateDomTileInfo(glbInt32 level, glbInt32 column, glbInt32 row)
{
	mpr_domMinLevel = min(level,mpr_domMinLevel);
	mpr_domMaxLevel = max(level,mpr_domMaxLevel);
	
	glbDouble lzts          = mpr_lzts/pow(2.0,level);
	GlbLevelInfo* levelinfo = NULL;
	map<glbInt32,GlbLevelInfo*>::iterator itr = mpr_domLevelInfo.find(level);
	if (itr != mpr_domLevelInfo.end())
	{
		levelinfo = itr->second;
	}else{
		levelinfo              = new GlbLevelInfo;
		mpr_domLevelInfo[level]= levelinfo;
		levelinfo->startRow = INT_MAX;
		levelinfo->endRow   = INT_MIN;
		levelinfo->startCol = INT_MAX;
		levelinfo->endCol   = INT_MIN;
		levelinfo->Rows     = 0;
		levelinfo->Cols     = 0;
		levelinfo->OriginX  = DBL_MAX;
		levelinfo->OriginY  = 0-DBL_MAX;
	}
	if (mpr_isGlobe)
	{
		levelinfo->OriginX = min(lzts*column-180,levelinfo->OriginX);
		levelinfo->OriginY = max(90-lzts*row,levelinfo->OriginY);
	}
	else
	{
		levelinfo->OriginX = min(lzts*column,levelinfo->OriginX);
		levelinfo->OriginY = max(lzts*(row+1),levelinfo->OriginY);
	}
	levelinfo->startRow = min(row,levelinfo->startRow);
	levelinfo->endRow   = max(row,levelinfo->endRow);
	levelinfo->startCol = min(column,levelinfo->startCol);
	levelinfo->endCol   = max(column,levelinfo->endCol);
	levelinfo->Rows     = levelinfo->endRow - levelinfo->startRow +1;
	levelinfo->Cols     = levelinfo->endCol - levelinfo->startCol +1 ;
	{//更新元数据
		CGlbString strsql = CGlbString::FormatString
		                ("update GlbTedInfo set GlbDomMinLevel=%d,GlbDomMaxLevel=%d;",
						  mpr_domMinLevel,mpr_domMaxLevel);
		if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
			return false;
		
		strsql = CGlbString::FormatString("delete from GlbTedDomLevelsInfo where GLBLevel = %d",level);
		if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
			return false;		
		strsql = CGlbString::FormatString("insert into GlbTedDomLevelsInfo values(%d,%.15lf,%.15lf,%d,%d);",
										       level,levelinfo->OriginX,levelinfo->OriginY,levelinfo->Rows,
											   levelinfo->Cols);
		if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
			return false;		
	}
	return true;
}

glbBool CGlbTedSQLite::UpdateDemTileInfo(glbInt32 level, glbInt32 column, glbInt32 row)
{
	mpr_demMinLevel = min(level,mpr_demMinLevel);
	mpr_demMaxLevel = max(level,mpr_demMaxLevel);
	
	glbDouble lzts = mpr_lzts/pow(2.0,level);
	GlbLevelInfo* levelinfo = NULL;
	map<glbInt32,GlbLevelInfo*>::iterator itr = mpr_demLevelInfo.find(level);
	if (itr != mpr_demLevelInfo.end())
	{
		levelinfo = itr->second;
	}else{
		levelinfo = new GlbLevelInfo;
		mpr_demLevelInfo[level] = levelinfo;
		levelinfo->startRow = INT_MAX;
		levelinfo->endRow   = INT_MIN;
		levelinfo->startCol = INT_MAX;
		levelinfo->endCol   = INT_MIN;
		levelinfo->Rows     = 0;
		levelinfo->Cols     = 0;
		levelinfo->OriginX  = DBL_MAX;
		levelinfo->OriginY  = 0-DBL_MAX;
	}
	glbDouble ox,oy;
	if (mpr_isGlobe)
	{
		ox = lzts*column-180;
		oy = 90-lzts*row;
	}
	else
	{
		ox = lzts*column;
		oy = lzts*(row+1);
	}
	levelinfo->startRow = min(row,levelinfo->startRow);
	levelinfo->endRow   = max(row,levelinfo->endRow);
	levelinfo->startCol = min(column,levelinfo->startCol);
	levelinfo->endCol   = max(column,levelinfo->endCol);
	levelinfo->OriginX  = min(ox,levelinfo->OriginX);
	levelinfo->OriginY  = max(oy,levelinfo->OriginY);
	levelinfo->Rows     = levelinfo->endRow - levelinfo->startRow +1;
	levelinfo->Cols     = levelinfo->endCol - levelinfo->startCol +1 ;
	{//更新元数据
		CGlbString strsql = CGlbString::FormatString
		                ("update GlbTedInfo set GlbDemMinLevel=%d,GlbDemMaxLevel=%d;",
						  mpr_demMinLevel,mpr_demMaxLevel);
		if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
			return false;
		
		strsql = CGlbString::FormatString("delete from GlbTedDemLevelsInfo where GLBLevel = %d",level);
		if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
			return false;		
		strsql = CGlbString::FormatString("insert into GlbTedDemLevelsInfo values(%d,%.15lf,%.15lf,%d,%d);",
										       level,levelinfo->OriginX,levelinfo->OriginY,levelinfo->Rows,
											   levelinfo->Cols);
		if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
			return false;		
	}
	return true;
}

glbBool CGlbTedSQLite::BeginTransaction()
{
	if (mpr_sqlite != NULL)
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_cs);
		glbBool bret = mpr_sqlite->BeginTransaction();
		return bret;
	}
	GlbSetLastError(L"没有初始化");
	return false;
}

glbBool CGlbTedSQLite::CommitTransaction()
{
	if (mpr_sqlite != NULL)
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_cs);
		glbBool bret = mpr_sqlite->CommitTransaction();
		return bret;
	}
	GlbSetLastError(L"没有初始化");
	return false;
}

glbBool CGlbTedSQLite::RollbackTransaction()
{
	if (mpr_sqlite != NULL)
	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_cs);
		glbBool bret = mpr_sqlite->RollbackTransaction();
		return bret;
	}
	GlbSetLastError(L"没有初始化");
	return false;
}

bool CGlbTedSQLite::IsInTransaction()
{
	if (mpr_sqlite != NULL)
	{
		return mpr_sqlite->IsInTransaction();
	}	
	return false;
}