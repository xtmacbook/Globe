#pragma warning(disable:4251)
#include "stdafx.h"
#include "GlbSQLitePyramid.h"
#include "GlbString.h"
#include "GlbPath.h"
#include "GlbError.h"
#include "GlbCompress.h"
#include <assert.h>
#define MAXCOUNT 1000

glbBool _compress(GlbCompressTypeEnum compress, CGlbPixelBlock* pixelblock, glbByte** outdata, glbUInt32& outlen)
{
	glbInt32 rows,cols;
	pixelblock->GetSize(cols, rows);
	glbByte* pdata = NULL;
	glbInt32 plen = pixelblock->GetPixelData(&pdata);
	if (compress == GLB_COMPRESSTYPE_ZLIB)
	{
		CGlbCompress::Compress(GLB_COMPRESSTYPE_ZLIB, pdata, plen, outdata, outlen);//zlib
	}
	else if (compress == GLB_COMPRESSTYPE_JPEG)
	{
		if(    pixelblock->GetPixelLayout() != GLB_PIXELLAYOUT_BIP 
			|| pixelblock->GetPixelType()   != GLB_PIXELTYPE_BYTE)
		{
			GlbSetLastError(L"JPEG压缩只支持RGB像素交叉格式");
			return false;
		}
		glbInt32 bandCount = pixelblock->GetBandCount();
		if(bandCount != 3 && bandCount != 4)
		{
			GlbSetLastError(L"JPEG压缩只支持3/4个波段的数据压缩");
			return false;
		}
		if (pixelblock->GetBandCount() == 3)
		{
			GlbBmpDataBuffer bdBuffer;
			bdBuffer.width = cols;
			bdBuffer.height = rows;
			bdBuffer.data = pdata;
			CGlbCompress::Compress(GLB_COMPRESSTYPE_JPEG, (glbByte*)&bdBuffer, plen, outdata, outlen);
		}
		else if (pixelblock->GetBandCount() >= 4)
		{
			pixelblock->GetBandCount();
			//获取rgb通道
			glbByte* tempRGB = (glbByte*)malloc(cols*rows*3);
			if (tempRGB == NULL)
			{
				GlbSetLastError(L"申请内存失败");
				return false;
			}
			glbByte* tempA = (glbByte*)malloc(cols*rows);
			if (tempA == NULL)
			{
				GlbSetLastError(L"申请内存失败");
				free(tempRGB);
				return false;
			}
			//分离rgba
			for (glbInt32 i=0;i<rows;i++)
			{
				for (glbInt32 j=0;j<cols;j++)
				{
					memcpy(tempRGB+(i*cols+j)*3, pdata+(i*cols+j)*4, 3);
					*(tempA+i*cols+j) = *(pdata+(i*cols+j)*4+ 3);
				}
			}
			//压缩
			glbByte*  tempJPG = NULL;
			glbUInt32 jpgLen = 0;
			GlbBmpDataBuffer bdBuffer;
			bdBuffer.width  = cols;
			bdBuffer.height = rows;
			bdBuffer.data   = tempRGB;
			CGlbCompress::Compress(GLB_COMPRESSTYPE_JPEG, (glbByte*)&bdBuffer, cols*rows*3, &tempJPG, jpgLen);
			free(tempRGB);
			glbByte*  outA = NULL;
			glbUInt32 lenA = 0;
			CGlbCompress::Compress(GLB_COMPRESSTYPE_ZLIB, tempA, cols*rows, &outA, lenA);//zlib
			free(tempA);
			//合并
			outlen   = jpgLen + lenA + sizeof(glbUInt32);
			*outdata = (glbByte*)malloc(outlen);
			if (*outdata == NULL)
			{
				GlbSetLastError(L"申请内存失败");
				return false;
			}
			memcpy(*outdata, &jpgLen, sizeof(glbUInt32));//记录jpg压缩后长度
			memcpy(*outdata+sizeof(glbUInt32), tempJPG, jpgLen);
			memcpy(*outdata+jpgLen+sizeof(glbUInt32), outA, lenA);
			free(tempJPG);
			free(outA);
		}
	}
	else if (compress == GLB_COMPRESSTYPE_NO)
	{
		//直接用原来的内存
		outlen = plen;
		*outdata = pdata;
	}
	else
	{
		GlbSetLastError(L"压缩格式不支持");
		return false;
	}
	return true;
}

CGlbSQLitePyramid::CGlbSQLitePyramid(void)
{
	mpr_isGlobe     = true;
	mpr_pixeltype   = GLB_PIXELTYPE_UNKNOWN;
	mpr_pixellayout = GLB_PIXELLAYOUT_UNKNOWN;
	mpr_bandCount   = 0;
	mpr_lzts        = 0.0;
	mpr_tileSizeX   = 0;
	mpr_tileSizeY   = 0;
	mpr_minLevel    = 0;
	mpr_maxLevel    = 0;
	mpr_originX     = 0.0;
	mpr_originY     = 0.0;
	mpr_rows        = 0;
	mpr_cols        = 0;
	mpr_isShareEdge = false;
	mpr_compress    = GLB_COMPRESSTYPE_NO;
	mpr_srs         = L"";
	mpr_sqlite      = new CSQLite();	
}

CGlbSQLitePyramid::~CGlbSQLitePyramid(void)
{
	if(mpr_sqlite != NULL)
		delete mpr_sqlite;
}

glbBool CGlbSQLitePyramid::Open( const glbWChar* fullname )
{
	CGlbWString dbname = fullname;
	
	if(CGlbPath::FileExist(fullname) == false)
	{
		GlbSetLastError(L"文件不存在");
		return false;
	}
	if(!mpr_sqlite->Open(dbname.ToUTF8String().c_str()))//UTF8编码的
	{		
		return false;
	}
	
	CGlbString strsql = "select * from GlbPyramidInfo;";	
	CSQLiteDataReader* Reader = mpr_sqlite->ExcuteQuery(strsql.c_str());
	if (Reader == NULL || !Reader->Read())
	{
		if(Reader)delete Reader;
		return false;
	}
	if (Reader->GetIntValue(0) == 0)
		mpr_isGlobe = false;
	else
		mpr_isGlobe = true;
	mpr_pixeltype   = (GlbPixelTypeEnum)Reader->GetIntValue(1);
	mpr_pixellayout = (GlbPixelLayoutTypeEnum)Reader->GetIntValue(2);
	mpr_bandCount   = Reader->GetIntValue(3);
	mpr_lzts        = Reader->GetFloatValue(4);
	mpr_tileSizeX   = Reader->GetIntValue(5);
	mpr_tileSizeY   = Reader->GetIntValue(6);
	mpr_minLevel    = Reader->GetIntValue(7);
	mpr_maxLevel    = Reader->GetIntValue(8);
	mpr_originX     = Reader->GetFloatValue(9);
	mpr_originY     = Reader->GetFloatValue(10);
	mpr_rows        = Reader->GetIntValue(11);
	mpr_cols        = Reader->GetIntValue(12);
	if (Reader->GetIntValue(13) == 0)
	{
		mpr_isShareEdge = false;
	}
	else
	{
		mpr_isShareEdge = true;
	}
	mpr_compress = (GlbCompressTypeEnum)Reader->GetIntValue(14);
	CGlbString srs = (const glbChar*)Reader->GetStringValue(15);
	mpr_srs = srs.ToWString();
	delete Reader;
	return true;
}

glbBool CGlbSQLitePyramid::Create( const glbWChar* fullname )
{
	CGlbWString dbname = fullname;
	
	if(CGlbPath::FileExist(fullname))
	{
		GlbSetLastError(L"文件已存在");
		return false;
	}
	if(!mpr_sqlite->Open(dbname.ToUTF8String().c_str()))//UTF8编码的
	{
		return false;
	}
	//创建表GlbPyramidInfo
	CGlbString strsql = "create table [GlbPyramidInfo]([GlbMode] INTEGER,";
	strsql += "[GlbPixelType]       INTEGER,[GlbPixelLayout]  INTEGER,[GlbBandCount]       INTEGER,";
	strsql += "[GlbLZTS]            DOUBLE, [GlbTileSizeX]    INTEGER,[GlbTileSizeY]       INTEGER,";
	strsql += "[GlbMinLevel]        INTEGER,[GlbMaxLevel]     INTEGER,[GlbMaxLevelOriginX] DOUBLE,";
	strsql += "[GlbMaxLevelOriginY] DOUBLE, [GlbMaxLevelRows] INTEGER,[GlbMaxLevelCols]    INTEGER,";
	strsql += "[GlbIsShareEdge]     INTEGER,[GlbCompress]     INTEGER,[GlbSRS]             TEXT);";
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))
	{  
		mpr_sqlite->Close();
		CGlbPath::DeleteFile(fullname);
		return false;
	}  
	//创建表GlbTiles,带索引
	strsql = "create table [GlbTiles]([GlbLevel] INTEGER,[GlbRow] INTEGER,[GlbCol] INTEGER,";
	strsql += "[GlbData] BLOB,[GlbMask] BLOB, primary key([GlbLevel],[GlbRow],[GlbCol]));";
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
	{  
		mpr_sqlite->Close();
		CGlbPath::DeleteFile(fullname);
		return false;
	}
	//插入GlbPyramidInfo
	strsql = CGlbString::FormatString("insert into GlbPyramidInfo values(%d,%d,%d,%d,%.15lf,%d,%d,%d,%d,%.15lf,%.15lf,%d,%d,%d,%d,'%s');",
		mpr_isGlobe, mpr_pixeltype, mpr_pixellayout, mpr_bandCount, mpr_lzts, mpr_tileSizeX, mpr_tileSizeY,
		mpr_minLevel,mpr_maxLevel,mpr_originX,mpr_originY,mpr_rows,mpr_cols,mpr_isShareEdge,mpr_compress,mpr_srs.ToString().c_str());
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
	{
		mpr_sqlite->Close();
		CGlbPath::DeleteFile(fullname);
		return false;
	}
	return true;
}

glbBool CGlbSQLitePyramid::Read( glbInt32 tileRow,glbInt32 tileCol,glbInt32 level, CGlbPixelBlock* pixelblock )
{
	glbInt32 rows,cols;
	pixelblock->GetSize(cols, rows);
	CGlbString strsql = CGlbString::FormatString
		                ("select GlbData,GlbMask from GlbTiles where GlbLevel=%d and GlbRow=%d and GlbCol=%d;",
						  level,tileRow,tileCol);
	CSQLiteDataReader* reader = mpr_sqlite->ExcuteQuery(strsql.c_str());
	if (reader == NULL || !reader->Read())
	{
		if(reader)delete reader;
		return false;
	}

	glbByte* pdata = NULL;
	glbInt32 plen  = pixelblock->GetPixelData(&pdata);
	//像素data
	glbInt32 nlen;
	glbByte* data = const_cast<glbByte*>(reader->GetBlobValue(0, nlen));
	//解压
	glbByte*  outdata = NULL;
	glbUInt32 outlen  = 0;
	if (mpr_compress == GLB_COMPRESSTYPE_ZLIB)
	{
		CGlbCompress::UnCompress(GLB_COMPRESSTYPE_ZLIB, data, nlen, &outdata, outlen);//zlib
		memcpy(pdata, outdata, outlen);
		free(outdata);
	}
	else if (mpr_compress == GLB_COMPRESSTYPE_JPEG)
	{
		assert(mpr_pixellayout==GLB_PIXELLAYOUT_BIP && mpr_pixeltype== GLB_PIXELTYPE_BYTE);
		if (pixelblock->GetBandCount() == 3)
		{
			CGlbCompress::UnCompress(GLB_COMPRESSTYPE_JPEG, data, nlen, &outdata, outlen);//jpeg
			memcpy(pdata, outdata, outlen);
			free(outdata);
		}
		else if (pixelblock->GetBandCount() == 4)
		{
			glbUInt32 inlen   = *(glbUInt32*)data;
			glbByte*  tempRGB = NULL;
			glbUInt32 lenRGB;
			CGlbCompress::UnCompress(GLB_COMPRESSTYPE_JPEG, data+4, inlen, &tempRGB, lenRGB);//jpeg
			glbByte* tempA = NULL;
			glbUInt32 lenA;
			CGlbCompress::UnCompress(GLB_COMPRESSTYPE_ZLIB, data+4+inlen, nlen-4-inlen, &tempA, lenA);//zlib
			assert(lenRGB==rows*cols*3 && lenA==rows*cols);
			for (glbInt32 i=0;i<rows;i++)
			{
				for (glbInt32 j=0;j<cols;j++)
				{
					memcpy(pdata+(i*cols+j)*4, tempRGB+(i*cols+j)*3, 3);
					*(pdata+(i*cols+j)*4+ 3) = *(tempA + i*cols+j); 
				}
			}
			free(tempRGB);
			free(tempA);
		}
		else
		{
			GlbSetLastError(L"jpeg压缩只支持3/4波段");
			delete reader;
			return false;
		}
	}
	else if (mpr_compress == GLB_COMPRESSTYPE_NO)
	{
		memcpy(pdata, data, nlen);
	}
	//标记mask
	data = const_cast<glbByte*>(reader->GetBlobValue(1, nlen));
	if (data!=NULL)
	{
		//解压
		CGlbCompress::UnCompress(GLB_COMPRESSTYPE_ZLIB, data, nlen, &outdata, outlen);
		plen = pixelblock->GetMaskData(&pdata);
		memcpy(pdata, outdata, outlen);
		free(outdata);
	}
	delete reader;
	return true;
}

glbBool CGlbSQLitePyramid::Write( glbInt32 tileRow,glbInt32 tileCol,glbInt32 level, CGlbPixelBlock* pixelblock )
{
	glbInt32 rows,cols;
	pixelblock->GetSize(cols, rows);
	GlbScopedLock<GlbCriticalSection> lock(mpr_cs);

	CGlbString strsql = CGlbString::FormatString
		                ("delete from GlbTiles where GlbLevel=%d and GlbRow=%d and GlbCol=%d;",
		                  level, tileRow, tileCol);
	mpr_sqlite->ExcuteNonQuery(strsql.c_str());

	strsql = "insert into GlbTiles(GlbLevel,GlbRow,GlbCol,GlbData,GlbMask) values(?1,?2,?3,?4,?5);";
	CSQLiteCommand insertcmd(mpr_sqlite->GetSqlite());
	if(!insertcmd.SetCommandText(strsql.c_str()))
		return false;
	
	//绑定
	insertcmd.BindParam(1, level);
	insertcmd.BindParam(2, tileRow);
	insertcmd.BindParam(3, tileCol);

	glbByte* pdata = NULL;
	glbInt32 plen = pixelblock->GetPixelData(&pdata);
	//压缩
	glbByte* outdata = NULL;
	glbUInt32 outlen = 0;
	if(!_compress(mpr_compress, pixelblock, &outdata, outlen))
		return false;
	insertcmd.BindParam(4, outdata, outlen);

	glbByte*  outmaskdata = NULL;
	glbUInt32 outmasklen = 0;
	//标记mask
	plen = pixelblock->GetMaskData(&pdata);
	//压缩
	CGlbCompress::Compress(GLB_COMPRESSTYPE_ZLIB, pdata, plen, &outmaskdata, outmasklen);
	insertcmd.BindParam(5, outmaskdata, outmasklen);

	if(!insertcmd.Excute())
	{		//释放绑定的内存
		if (mpr_compress != GLB_COMPRESSTYPE_NO)
			free(outdata);
		free(outmaskdata);
		return false;
	}
	//释放绑定的内存
	if (mpr_compress != GLB_COMPRESSTYPE_NO)
		free(outdata);
	free(outmaskdata);
	return true;
}

glbBool CGlbSQLitePyramid::UpdatePydInfo()
{
	CGlbString strsql = "delete from GlbPyramidInfo";
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
		return false;
	strsql = CGlbString::FormatString("insert into GlbPyramidInfo values(%d,%d,%d,%d,%.15lf,%d,%d,%d,%d,%.15lf,%.15lf,%d,%d,%d,%d,'%s');",
		mpr_isGlobe, mpr_pixeltype, mpr_pixellayout, mpr_bandCount, mpr_lzts, mpr_tileSizeX, mpr_tileSizeY,
		mpr_minLevel,mpr_maxLevel,mpr_originX,mpr_originY,mpr_rows,mpr_cols,mpr_isShareEdge,mpr_compress,mpr_srs.ToString().c_str());
	if(!mpr_sqlite->ExcuteNonQuery(strsql.c_str()))  
		return false;
	return true;
}
glbBool CGlbSQLitePyramid::BeginTransaction()
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

glbBool CGlbSQLitePyramid::CommitTransaction()
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

glbBool CGlbSQLitePyramid::RollbackTransaction()
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

bool CGlbSQLitePyramid::IsInTransaction()
{
	if (mpr_sqlite != NULL)
	{
		return mpr_sqlite->IsInTransaction();
	}	
	return false;
}
