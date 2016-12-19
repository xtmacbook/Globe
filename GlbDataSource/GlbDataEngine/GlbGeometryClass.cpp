#include "StdAfx.h"
#include "IGlbGeometry.h"

CGlbGeometryClass::CGlbGeometryClass()
{
	
}

CGlbGeometryClass::~CGlbGeometryClass()
{
}

glbBool CGlbGeometryClass::AddField( CGlbField* fld )
{
	if(!fld)
	{
		GlbSetLastError(L"参数不正确");
		return false;
	}
	if (fld->GetType() == GLB_DATATYPE_STRING || 
		fld->GetType() == GLB_DATATYPE_GEOMETRY || 
		fld->GetType() == GLB_DATATYPE_BLOB || 
		fld->GetType() == GLB_DATATYPE_UNKNOWN)
	{
		GlbSetLastError(L"字段类型是非数字类型");
		return false;
	}
	if(fld->GetDefault() == NULL)
	{
		GlbSetLastError(L"字段没有缺省值");
		return false;
	}	

	CGlbField*      tfld=NULL;
	const glbWChar* name=NULL;
	const glbWChar* tname=NULL;
	name=fld->GetName();
	for (size_t i=0;i<mpr_vecfields.size();i++)
	{
		tfld=mpr_vecfields.at(i).get();
		tname=tfld->GetName();
		if(wcscmp(tname,name)==0)
		{
			GlbSetLastError(L"同名字段已存在");
			return false;
		}
	}
	fld->SetCanNull(false);
	mpr_vecfields.push_back(fld);
	return true;
}

CGlbField* CGlbGeometryClass:: GetField(glbInt32 fldindex)
{
	if(fldindex<0||fldindex>=(glbInt32)mpr_vecfields.size())
	{
		GlbSetLastError(L"fldindex必须>=0且小于字段数目");
		return NULL;
	}
	return mpr_vecfields.at(fldindex).get();
}

CGlbField* CGlbGeometryClass::GetFieldByName(const glbWChar* name)
{
	if(!name)
	{
		GlbSetLastError(L"参数是NULL");
		return NULL;
	}
	CGlbField* fld=NULL;
	for (size_t i=0;i<mpr_vecfields.size();i++)
	{
		fld=mpr_vecfields.at(i).get();
		if(wcscmp(fld->GetName(),name)==0)
		{
			return fld;
		}
	}
	GlbSetLastError(L"不存在参数指定的字段");
	return NULL;
}

glbInt32 CGlbGeometryClass::GetFieldCount()
{
	return static_cast<glbInt32>(mpr_vecfields.size());
}

glbInt32 CGlbGeometryClass::FindFieldIndex(const glbWChar* name)
{
	if(!name)
	{
		GlbSetLastError(L"参数是NULL");
		return -1;
	}
	CGlbField*       fld=NULL;
	const glbWChar*  tname=NULL;
	
	for(size_t i=0;i<mpr_vecfields.size();i++)
	{
		fld=mpr_vecfields.at(i).get();
		tname=fld->GetName();
		if(wcscmp(name,tname)==0)
		{
			return i;
		}
	}	
	return -1;
}

glbBool CGlbGeometryClass::RemoveField(glbInt32 fldindex)
{
	if(fldindex<0||fldindex>=(glbInt32)mpr_vecfields.size())
	{
		GlbSetLastError(L"fldindex必须>=0且小于字段数目");
		return false;
	}
	mpr_vecfields.erase(mpr_vecfields.begin()+fldindex);
	return true;
}

glbBool CGlbGeometryClass::RemoveFieldByName(const glbWChar* name)
{
	if(NULL == name)
	{
		GlbSetLastError(L"参数为空");
		return false;
	}
	for (size_t i = 0; i < mpr_vecfields.size(); i++)
	{
		if (wcscmp(mpr_vecfields[i]->GetName(),name) == 0)
		{
			mpr_vecfields.erase(mpr_vecfields.begin()+i);
			return true;
		}		
	}
	return false;
}

CGlbGeometryCollect::CGlbGeometryCollect()
{
	mpr_extent = NULL;
}

CGlbGeometryCollect::~CGlbGeometryCollect()
{
	Empty();
}

GlbGeometryTypeEnum CGlbGeometryCollect::GetType()
{
	return GLB_GEO_COLLECT;
}

glbBool CGlbGeometryCollect::IsEmpty()
{
	if(mpr_geometries.size()==0)return true;
	return false;
}

CGlbExtent* CGlbGeometryCollect::GetExtent()
{
	if(IsEmpty())return NULL;
	if (mpr_extent == NULL)
	{
		mpr_extent = new CGlbExtent();
	}
	glbBool flag = true;
	glbDouble minx = 0.0,miny = 0.0,minz = 0.0,maxx = 0.0,maxy = 0.0,maxz = 0.0;
	for (size_t i = 0; i < mpr_geometries.size(); i++)
	{
		if (mpr_geometries[i])
		{
			CGlbExtent* _extent = (CGlbExtent*)mpr_geometries[i]->GetExtent();
			if (_extent != NULL)
			{
				if (flag)
				{
					_extent->GetMin(&minx,&miny,&minz);
					_extent->GetMax(&maxx,&maxy,&maxz);
					flag = false;
				}
				else
				{
					glbDouble minX = 0.0,minY = 0.0,minZ =0.0,maxX = 0.0,maxY = 0.0,maxZ = 0.0;
					_extent->GetMin(&minX,&minY,&minZ);
					_extent->GetMax(&maxX,&maxY,&maxZ);
					minx = min(minx,minX);
					maxx = max(maxx,maxX);
					miny = min(miny,minY);
					maxy = max(maxy,maxY);
					minz = min(minz,minZ);
					maxz = max(maxz,maxZ);					
				}
			}
		}
	}
	mpr_extent->SetMin(minx,miny,minz);
	mpr_extent->SetMax(maxx,maxy,maxz);
	return mpr_extent.get();
}

glbUInt32 CGlbGeometryCollect::GetSize()
{
	glbUInt32 lsize = sizeof(vector<glbref_ptr<IGlbGeometry>>);
	lsize += mpr_geometries.size()*sizeof(glbref_ptr<IGlbGeometry>);
	for (size_t i = 0; i < mpr_geometries.size(); i++)
	{
		if (mpr_geometries[i])
			lsize += mpr_geometries[i].get()->GetSize();
	}
	lsize += sizeof(glbref_ptr<CGlbExtent>);
	if(mpr_extent.get())
		lsize += sizeof(CGlbExtent);
	return lsize;
}

glbInt32  CGlbGeometryCollect::GetCoordDimension()
{
	return 0;
}

glbBool   CGlbGeometryCollect::HasM()
{
	return false;
}

void      CGlbGeometryCollect::Empty()
{
	for(size_t i=0;i<mpr_geometries.size();i++)
		mpr_geometries[i] = NULL;
	mpr_geometries.clear();
	mpr_extent = NULL;
}

glbBool CGlbGeometryCollect::SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls)
{
	return false;
}

const CGlbGeometryClass* CGlbGeometryCollect::GetGeoClass(GlbGeometryClassEnum ctype)
{
	return NULL;
}

glbBool CGlbGeometryCollect::AddGeometry( IGlbGeometry* geo)
{
	mpr_geometries.push_back(geo);	
	return true;
}

glbBool CGlbGeometryCollect::DeleteGeometry( glbInt32 idx )
{
	if (idx < 0 || idx >= (glbInt32)mpr_geometries.size())
	{
		GlbSetLastError(L"参数必须>=0且小于几何数目");
		return false;
	}
	mpr_geometries.erase(mpr_geometries.begin() + idx);
	return true;
}


glbBool CGlbGeometryCollect::GetGeometry( glbInt32 idx,IGlbGeometry** geo)
{
	if (idx < 0 || idx >= (glbInt32)mpr_geometries.size())
	{
		GlbSetLastError(L"参数idx必须>=0且小于几何数目");
		return false;
	}
	if(geo == NULL)
	{
		GlbSetLastError(L"参数geo是NULL");
		return false;
	}
	*geo = mpr_geometries[idx].get();	
	return true;
}

glbInt32 CGlbGeometryCollect::GetCount()
{
	return mpr_geometries.size();
}

glbBool   glbArrayClean(glbArray& glbArr)
{
	if(glbArr.vt == GLB_DATATYPE_BLOB && glbArr.pBlobs)
	{
		glbBlob* pBlobs  = glbArr.pBlobs;
		glbInt32 cnt     = glbArr.dims*glbArr.dimSize;
		for(glbInt32 i=0; i<cnt; i++)
			free(pBlobs[i].pdata);
		free(pBlobs);
		pBlobs = NULL;
	}
	else if(glbArr.vt == GLB_DATATYPE_GEOMETRY && glbArr.pGeos)
	{
		glbref_ptr<IGlbGeometry>** pGeos = glbArr.pGeos;
		glbInt32 cnt    = glbArr.dims*glbArr.dimSize;
		for(glbInt32 i=0; i<cnt; i++)
			delete pGeos[i];
		free(pGeos);
		pGeos = NULL;
	}
	else if(glbArr.vt == GLB_DATATYPE_STRING && glbArr.pStrs)
	{
		glbWChar** pStrs = glbArr.pStrs;
		glbInt32 cnt     = glbArr.dims*glbArr.dimSize;
		for(glbInt32 i=0; i<cnt; i++)
			free(pStrs[i]);
		free(pStrs);
		pStrs = NULL;		
	}else{
		switch(glbArr.vt)
		{
			case GLB_DATATYPE_BYTE:
				{
					if(glbArr.pBytes == NULL)return true;
					free(glbArr.pBytes);
					glbArr.pBytes = NULL;
				}break;
			//case GLB_DATATYPE_CHAR:
			//	{
			//		if(glbArr.pChars == NULL)return true;
			//		free(glbArr.pChars);
			//		glbArr.pChars = NULL;
			//	}break;
			case GLB_DATATYPE_BOOLEAN:
				{
					if(glbArr.pBools == NULL)return true;
					free(glbArr.pBools);
					glbArr.pBools = NULL;
				}break;
			case GLB_DATATYPE_INT16:
				{
					if(glbArr.pInts == NULL)return true;
					free(glbArr.pInts);
					glbArr.pInts = NULL;
				}break;
			case GLB_DATATYPE_INT32:
				{
					if(glbArr.pLongs == NULL)return true;
					free(glbArr.pLongs);
					glbArr.pLongs = NULL;
				}break;
			case GLB_DATATYPE_INT64:
				{
					if(glbArr.pLLongs == NULL)return true;
					free(glbArr.pLLongs);
					glbArr.pLLongs = NULL;
				}break;
			case GLB_DATATYPE_FLOAT:
				{
					if(glbArr.pFloats == NULL)return true;
					free(glbArr.pFloats);
					glbArr.pFloats = NULL;
				}break;
			case GLB_DATATYPE_DOUBLE:
				{
					if(glbArr.pDoubles == NULL)return true;
					free(glbArr.pDoubles);
					glbArr.pDoubles = NULL;
				}break;
			case GLB_DATATYPE_DATETIME:
				{
					if(glbArr.pDates == NULL)return true;
					free(glbArr.pDates);
					glbArr.pDates = NULL;
				}break;
			case GLB_DATATYPE_VECTOR:
			{
				if(glbArr.pVectors == NULL)return true;
				free(glbArr.pVectors);
				glbArr.pVectors = NULL;
			}break;
		}
	}
	return true;
}
glbBool   glbVariantClean(GLBVARIANT& var)
{
	if(var.isnull)
		return true;
	if(var.isarray)
	{
		glbArrayClean(var.arrayVal);
		return true;
	}//数组
	switch (var.vt)
	{
	case GLB_DATATYPE_UNKNOWN:
	case GLB_DATATYPE_BYTE:
	//case GLB_DATATYPE_CHAR:
	case GLB_DATATYPE_BOOLEAN:
	case GLB_DATATYPE_INT16:
	case GLB_DATATYPE_INT32:
	case GLB_DATATYPE_INT64:
	case GLB_DATATYPE_FLOAT:
	case GLB_DATATYPE_DOUBLE:
	case GLB_DATATYPE_DATETIME:
	case GLB_DATATYPE_VECTOR:
		break;
	case GLB_DATATYPE_STRING:
		{
			if(var.strVal)
				free(var.strVal);
			var.strVal = NULL;
		}
		break;
	case GLB_DATATYPE_GEOMETRY:
		{
			if(var.geo)
				delete var.geo;
			var.geo = NULL;
		}
		break;
	case GLB_DATATYPE_BLOB:
		{
			if (var.blob.pdata)
				free(var.blob.pdata);
			var.blob.pdata = NULL;
		}
		break;
	default:break;
	}
	return true;
}

glbBool glbArrayCopy(glbArray& destArr,glbArray& srcArr)
{
	destArr.vt      = srcArr.vt;
	destArr.dims    = srcArr.dims;
	destArr.dimSize = srcArr.dimSize;
	if(srcArr.vt == GLB_DATATYPE_BLOB)
	{
		glbInt32 cnt   = srcArr.dims*srcArr.dimSize;
		destArr.pBlobs = new glbBlob[cnt];
		if(destArr.pBlobs == NULL)
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
			
		glbBlob* destBlobs = (glbBlob*)destArr.pBlobs;
		glbBlob* srcBlobs  = (glbBlob*)srcArr.pBlobs;
		for(glbInt32 i=0; i<cnt; i++)
		{
			destBlobs[i].size = srcBlobs[i].size;
			destBlobs[i].pdata= (glbByte*)malloc(srcBlobs[i].size);
			if(destBlobs[i].pdata == NULL)
			{
				GlbSetLastError(L"内存不足");
				return false;
			}
			memcpy(destBlobs[i].pdata,srcBlobs[i].pdata,srcBlobs[i].size);
		}
	}
	else if(srcArr.vt == GLB_DATATYPE_GEOMETRY)
	{
		glbInt32 cnt                     = srcArr.dims*srcArr.dimSize;
		glbref_ptr<IGlbGeometry>**ppdata = new glbref_ptr<IGlbGeometry>*[cnt];
		if(ppdata == NULL)
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
		destArr.pGeos            = ppdata;
		
		glbref_ptr<IGlbGeometry>** destGeos = (glbref_ptr<IGlbGeometry>**)destArr.pGeos;
		glbref_ptr<IGlbGeometry>** srcGeos  = (glbref_ptr<IGlbGeometry>**)srcArr.pGeos;
		for(glbInt32 i=0; i<cnt; i++)
		{
			destGeos[i] = new glbref_ptr<IGlbGeometry>();
			if(destGeos[i] == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				*destGeos[i] = srcGeos[i]->get();
		}
	}
	else if(srcArr.vt == GLB_DATATYPE_STRING)
	{
		glbInt32 cnt = srcArr.dims*srcArr.dimSize;
		destArr.pStrs = new glbWChar*[cnt];
		if(destArr.pStrs == NULL)
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
		glbWChar** destStrs = destArr.pStrs;
		glbWChar** srcStrs  = srcArr.pStrs;
		for(glbInt32 i=0; i<cnt; i++)
		{
			glbInt32 len = wcslen(srcStrs[i]);
			destStrs[i]  = (glbWChar*)malloc(sizeof(glbWChar)*(len+1));
			if(destStrs[i] == NULL)
			{
				GlbSetLastError(L"内存不足");
				return false;
			}
			memset(destStrs[i],0,sizeof(glbWChar)*(len+1));
			wcscpy_s(destStrs[i],len+1,srcStrs[i]);
		}
	}
	else{
		switch(srcArr.vt)
		{
		case GLB_DATATYPE_BYTE:
			{
				glbInt32 dsize          = sizeof(glbByte)*srcArr.dims*srcArr.dimSize;
				destArr.pBytes = (glbByte*)malloc(dsize);
				if(destArr.pBytes == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pBytes,srcArr.pBytes,dsize);
			}break;
		/*case GLB_DATATYPE_CHAR:
			{
				glbInt32 dsize          = sizeof(glbChar)*srcArr.dims*srcArr.dimSize;
				destArr.pChars = (glbChar*)malloc(dsize);
				if(destArr.pChars == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pChars,srcArr.pChars,dsize);
			}break;*/
		case GLB_DATATYPE_BOOLEAN:		
			{
				glbInt32 dsize          = sizeof(glbBool)*srcArr.dims*srcArr.dimSize;
				destArr.pBools = (glbBool*)malloc(dsize);
				if(destArr.pBools == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pBools,srcArr.pBools,dsize);
			}break;
		case GLB_DATATYPE_INT16:
			{
				glbInt32 dsize          = sizeof(glbInt16)*srcArr.dims*srcArr.dimSize;
				destArr.pInts = (glbInt16*)malloc(dsize);
				if(destArr.pInts == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pInts,srcArr.pInts,dsize);
			}break;
		case GLB_DATATYPE_INT32:
			{
				glbInt32 dsize          = sizeof(glbInt32)*srcArr.dims*srcArr.dimSize;
				destArr.pLongs = (glbInt32*)malloc(dsize);
				if(destArr.pLongs == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pLongs,srcArr.pLongs,dsize);
			}break;
		case GLB_DATATYPE_INT64:
			{
				glbInt32 dsize          = sizeof(glbInt64)*srcArr.dims*srcArr.dimSize;
				destArr.pLLongs = (glbInt64*)malloc(dsize);
				if(destArr.pLLongs == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pLLongs,srcArr.pLLongs,dsize);
			}break;
		case GLB_DATATYPE_FLOAT:
			{
				glbInt32 dsize          = sizeof(glbFloat)*srcArr.dims*srcArr.dimSize;
				destArr.pFloats = (glbFloat*)malloc(dsize);
				if(destArr.pFloats == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pFloats,srcArr.pFloats,dsize);
			}break;
		case GLB_DATATYPE_DOUBLE:
			{
				glbInt32 dsize          = sizeof(glbDouble)*srcArr.dims*srcArr.dimSize;
				destArr.pDoubles = (glbDouble*)malloc(dsize);
				if(destArr.pDoubles == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pDoubles,srcArr.pDoubles,dsize);
			}break;
		case GLB_DATATYPE_DATETIME:
			{
				glbInt32 dsize          = sizeof(glbDate)*srcArr.dims*srcArr.dimSize;
				destArr.pDates = (glbDate*)malloc(dsize);
				if(destArr.pDates == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pDates,srcArr.pDates,dsize);
			}break;
		case GLB_DATATYPE_VECTOR:
			{
				glbInt32 dsize          = sizeof(glbVector)*srcArr.dims*srcArr.dimSize;
				destArr.pVectors = (glbVector*)malloc(dsize);
				if(destArr.pVectors == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memcpy(destArr.pVectors,srcArr.pVectors,dsize);
			}break;
		}
	}
	return true;
}
glbBool glbVariantCopy(GLBVARIANT& destVar,GLBVARIANT& srcVar)
{
	glbVariantClean(destVar);
	destVar.isnull = srcVar.isnull;
	destVar.vt     = srcVar.vt;
	destVar.isarray= srcVar.isarray;
	if(destVar.isnull)
		return true;
	if(destVar.isarray)
	{
		glbArrayCopy(destVar.arrayVal,srcVar.arrayVal);
		return true;
	}//数组
	switch (srcVar.vt)
	{	
	case GLB_DATATYPE_BYTE:
		destVar.bVal = srcVar.bVal;
		break;
	/*case GLB_DATATYPE_CHAR:
		destVar.cVal = srcVar.cVal;
		break;*/
	case GLB_DATATYPE_BOOLEAN:
		destVar.blVal = srcVar.blVal;
		break;
	case GLB_DATATYPE_INT16:
		destVar.iVal = srcVar.iVal;
		break;
	case GLB_DATATYPE_INT32:
		destVar.lVal = srcVar.lVal;
		break;
	case GLB_DATATYPE_INT64:
		destVar.llVal = srcVar.llVal;
		break;
	case GLB_DATATYPE_FLOAT:
		destVar.fVal = srcVar.fVal;
		break;
	case GLB_DATATYPE_DOUBLE:
		destVar.dblVal = srcVar.dblVal;
		break;
	case GLB_DATATYPE_STRING:
		{
			destVar.strVal=NULL;
			if(srcVar.strVal)
			{
				glbInt32 len = wcslen(srcVar.strVal);
				destVar.strVal = (glbWChar*)malloc(sizeof(glbWChar)*(len+1));
				if(destVar.strVal == NULL)
				{
					GlbSetLastError(L"内存不足");
					return false;
				}
				memset(destVar.strVal,0,sizeof(glbWChar)*(len+1));
				wcscpy_s(destVar.strVal,len+1,srcVar.strVal);
			}
		}
		break;
	case GLB_DATATYPE_DATETIME:
		destVar.date = srcVar.date;
		break;
	case GLB_DATATYPE_GEOMETRY:
		{
			if(srcVar.geo)
			{
				destVar.geo = new glbref_ptr<IGlbGeometry>();
				destVar.geo = srcVar.geo;
			}
		}
		break;
	case GLB_DATATYPE_VECTOR:
		destVar.vector = srcVar.vector;
		break;
	case GLB_DATATYPE_BLOB:
		// blob值拷贝
		destVar.blob.size  = srcVar.blob.size;
		destVar.blob.pdata = (glbByte*)malloc(destVar.blob.size);
		if (destVar.blob.pdata == NULL)
		{
			GlbSetLastError(L"内存申请失败");
			return false;
		}
		memcpy(destVar.blob.pdata,srcVar.blob.pdata,srcVar.blob.size);
		break;
	case GLB_DATATYPE_UNKNOWN:
	default:
		break;
	}
	return true;
}

glbUInt32 glbArraySize(glbArray& glbArr)
{
	glbUInt32 lsize = sizeof(glbArray);
	glbInt32  cnt   = glbArr.dims*glbArr.dimSize;
	if(glbArr.vt == GLB_DATATYPE_BLOB && glbArr.pBlobs)
	{
		lsize += cnt*sizeof(glbBlob);
		lsize += cnt*sizeof(glbBlob*);
		glbBlob* varBlobs = (glbBlob*)glbArr.pBlobs;
		for(glbInt32 i=0; i<cnt; i++)
			lsize += varBlobs[i].size;
	}
	else if(glbArr.vt == GLB_DATATYPE_GEOMETRY && glbArr.pGeos)
	{
		lsize += cnt*sizeof(glbref_ptr<IGlbGeometry>);
		lsize += cnt*sizeof(glbref_ptr<IGlbGeometry>*);
		
		glbref_ptr<IGlbGeometry>** varGeos = glbArr.pGeos;
		for(glbInt32 i=0; i<cnt; i++)
			lsize += varGeos[i]->get()->GetSize();		
	}
	else if(glbArr.vt == GLB_DATATYPE_STRING && glbArr.pStrs)
	{
		lsize += cnt*sizeof(glbWChar*);
		for(glbInt32 i=0;i<cnt;i++)
			lsize += sizeof(glbWChar)*(wcslen(glbArr.pStrs[i])+1);
	}else{
		lsize += cnt*GetDataTypeSize(glbArr.vt);			
	}		
	return lsize;
}
glbUInt32 glbVariantSize(GLBVARIANT& var)
{
	glbUInt32 lsize = sizeof(GLBVARIANT);
	if(var.isarray)
	{
		lsize += glbArraySize(var.arrayVal);
	}else{
		if(var.vt == GLB_DATATYPE_GEOMETRY && var.geo && var.geo->get())
		{
			lsize += sizeof(glbref_ptr<IGlbGeometry>);
			lsize += var.geo->get()->GetSize();
		}else if(var.vt == GLB_DATATYPE_BLOB){
			lsize  += var.blob.size;
		}else if(var.vt == GLB_DATATYPE_STRING && var.strVal){
			lsize += (lstrlen(var.strVal)+1)*sizeof(glbWChar);
		}
	}
	return lsize;
}

size_t GetDataTypeSize(GlbDataTypeEnum ftype)
{
	switch(ftype)
	{
	case GLB_DATATYPE_UNKNOWN:
		return 0;
	case GLB_DATATYPE_BYTE:
		return sizeof(glbByte);
	//case GLB_DATATYPE_CHAR:
	//	return sizeof(glbChar);
	case GLB_DATATYPE_BOOLEAN:
		return sizeof(glbBool);
	case GLB_DATATYPE_INT16:
		return sizeof(glbInt16);//(short);//2;
	case GLB_DATATYPE_INT32:
		return sizeof(glbInt32);//(glbInt32);//4;
	case GLB_DATATYPE_INT64:
		return sizeof(glbInt64);//8
	case GLB_DATATYPE_FLOAT:
		return sizeof(glbFloat);//4
	case GLB_DATATYPE_DOUBLE:
		sizeof(glbDouble);//8
	case GLB_DATATYPE_DATETIME:
		return sizeof(glbDouble);//8
	case GLB_DATATYPE_VECTOR:
		return 3*sizeof(glbDouble);
	case GLB_DATATYPE_STRING:
	case GLB_DATATYPE_GEOMETRY:
	case GLB_DATATYPE_BLOB:
	default:
		return 0;
	}
}

void SetGeoValue
	(
		void*       dstvalue,// 几何字段值
		glbInt32        idx,     // 值索引
		glbInt32        count,   // 值数目
		const void*     srcvalue,// 值来源
		GlbDataTypeEnum ftype)
{
	if (srcvalue)
	{
		switch(ftype)
		{		
		case GLB_DATATYPE_BYTE:
			{
				glbByte v = 0;
				v = *((glbByte*)srcvalue);
				memset(((glbByte*)dstvalue+idx),v,sizeof(glbByte)*count);				
				return;
			}break;
		/*case GLB_DATATYPE_CHAR:
			{
				glbChar v = 0;
				v = *((glbChar*)srcvalue);
				memset(((glbChar*)dstvalue+idx),v,sizeof(glbChar)*count);
				return;
			}break;*/
		case GLB_DATATYPE_BOOLEAN:
			{
				glbBool v = 0;
				v = *((glbBool*)srcvalue);
				memset(((glbBool*)dstvalue+idx),v,sizeof(glbBool)*count);
				return;
			}break;
		case GLB_DATATYPE_INT16:
			{
				glbInt16 v = 0;
				v = *((glbInt16*)srcvalue);
				for(glbInt32 i=0;i<count;i++)
					*((glbInt16*)dstvalue+idx+i) = v;
				return;
			}break;
		case GLB_DATATYPE_INT32:
			{
				glbInt32 v = 0;
				v = *((glbInt32*)srcvalue);
				for(glbInt32 i=0;i<count;i++)
					*((glbInt32*)dstvalue+idx+i) = v;
				return;
			}break;
		case GLB_DATATYPE_INT64:
			{
				glbInt64 v = 0;
				v = *((glbInt64*)srcvalue);
				for(glbInt32 i=0;i<count;i++)
					*((glbInt64*)dstvalue+idx+i) = v;
				return;
			}break;
		case GLB_DATATYPE_FLOAT:
			{
				glbFloat v = 0.0;
				v = *((glbFloat*)srcvalue);
				for(glbInt32 i=0;i<count;i++)
					*((glbFloat*)dstvalue+idx+i) = v;
				return;
			}break;
		case GLB_DATATYPE_DOUBLE:
		case GLB_DATATYPE_DATETIME:
			{
				glbDouble v = 0.0;
				v = *((glbDouble*)srcvalue);
				for(glbInt32 i=0;i<count;i++)
					*((glbDouble*)dstvalue+idx+i) = v;
				return;
			}break;
		case GLB_DATATYPE_VECTOR:
			{
				for(glbInt32 i=0;i<count;i++)
					memcpy(((glbDouble*)dstvalue+(idx+i)*3),(glbDouble*)srcvalue,sizeof(glbDouble)*3);
				return;
			}break;;
		case GLB_DATATYPE_STRING:
		case GLB_DATATYPE_GEOMETRY:
		case GLB_DATATYPE_BLOB:
		case GLB_DATATYPE_UNKNOWN:		
		default:break;
		}
	}
	return;
}

void SetGeoValueVariant
	 (
	     void*             dstvalue,
		 glbInt32          idx,
		 glbInt32          count,
		 const GLBVARIANT& srcvalue,
		 GlbDataTypeEnum   ftype)
{
	switch(ftype)
	{
	case GLB_DATATYPE_BYTE:
		{
			glbByte v = 0;
			v         = srcvalue.bVal;
			memset(((glbByte*)dstvalue+idx),v,sizeof(glbByte)*count);
			return;
		}
	/*case GLB_DATATYPE_CHAR:
		{
			glbChar v = 0;
			v         = srcvalue.cVal;
			memset(((glbChar*)dstvalue+idx),v,sizeof(glbChar)*count);
			return;
		}*/
	case GLB_DATATYPE_BOOLEAN:
		{
			glbBool v = 0;
			v         = srcvalue.blVal;
			memset(((glbBool*)dstvalue+idx),v,sizeof(glbBool)*count);
			return;
		}
	case GLB_DATATYPE_INT16:
		{
			glbInt16 v = 0;
			v          = srcvalue.iVal;
			for(glbInt32 i=0;i<count;i++)
				*((glbInt16*)dstvalue+idx+i) = v;
		}return;
	case GLB_DATATYPE_INT32:
		{
			glbInt32 v = 0;
			v = srcvalue.lVal;
			for(glbInt32 i=0;i<count;i++)
				*((glbInt32*)dstvalue+idx+i) = v;
		}return;
	case GLB_DATATYPE_INT64:
		{
			glbInt64 v = 0;
			v = srcvalue.llVal;
			for(glbInt32 i=0;i<count;i++)
				*((glbInt64*)dstvalue+idx+i) = v;
		}return;
	case GLB_DATATYPE_FLOAT:
		{
			glbFloat v = 0.0;
			v = srcvalue.fVal;
			for(glbInt32 i=0;i<count;i++)
				*((glbFloat*)dstvalue+idx+i) = v;
		}return;
	case GLB_DATATYPE_DOUBLE:
	case GLB_DATATYPE_DATETIME:
		{
			glbDate v = 0;
			v = srcvalue.date;
			for(glbInt32 i=0;i<count;i++)
				*((glbDate*)dstvalue+idx+i) = v;
		}return;
	case GLB_DATATYPE_VECTOR:
		{
			for(glbInt32 i=0;i<count;i++)
				memcpy(((glbDouble*)dstvalue+(idx+i)*3),&srcvalue.vector,sizeof(glbDouble)*3);
		}return;
	case GLB_DATATYPE_STRING:
	case GLB_DATATYPE_GEOMETRY:
	case GLB_DATATYPE_BLOB:
	case GLB_DATATYPE_UNKNOWN:
	default:
		return ;
	}
}

void  CopyRemoveGeoValue
	  (
	      void*           dstvalue,
		  void*           srcvalue,
		  glbInt32        srccount,
		  glbInt32        idx,
		  glbInt32        count,
		  GlbDataTypeEnum ftype)
{
	switch(ftype)
	{
	case GLB_DATATYPE_BYTE:
		{
			memcpy((glbByte*)dstvalue,
				   (glbByte*)srcvalue,
				    idx*sizeof(glbByte));
			memcpy((glbByte*)dstvalue+idx,
				   (glbByte*)srcvalue+idx+count,
				   (srccount-idx-count)*sizeof(glbByte));
		}break;
	/*case GLB_DATATYPE_CHAR:
		{
			memcpy((glbChar*)dstvalue,
				   (glbChar*)srcvalue,
				   idx*sizeof(glbChar));
			memcpy((glbChar*)dstvalue+idx,
				   (glbChar*)srcvalue+idx+count,
				   (srccount-idx-count)*sizeof(glbChar));
		}break;*/
	case GLB_DATATYPE_BOOLEAN:
		{
			memcpy((glbBool*)dstvalue,
				   (glbBool*)srcvalue,
				   idx*sizeof(glbBool));
			memcpy((glbBool*)dstvalue+idx,
				   (glbBool*)srcvalue+idx+count,
				   (srccount-idx-count)*sizeof(glbBool));
		}
		break;
	case GLB_DATATYPE_INT16:
		{
			memcpy((glbInt16*)dstvalue,
				   (glbInt16*)srcvalue,
				   idx*sizeof(glbInt16));
			memcpy((glbInt16*)dstvalue+idx,
				   (glbInt16*)srcvalue+idx+count,
				   (srccount-idx-count)*sizeof(glbInt16));
		}
		break;
	case GLB_DATATYPE_INT32:
		{
			memcpy((glbInt32*)dstvalue,
				   (glbInt32*)srcvalue,
				   idx*sizeof(glbInt32));
			memcpy((glbInt32*)dstvalue+idx,
				   (glbInt32*)srcvalue+idx+count,
				   (srccount-idx-count)*sizeof(glbInt32));
		}
		break;
	case GLB_DATATYPE_INT64:
		{
			memcpy((glbInt64*)dstvalue,
				   (glbInt64*)srcvalue,
				   idx*sizeof(glbInt64));
			memcpy((glbInt64*)dstvalue+idx,
				   (glbInt64*)srcvalue+idx+count,
				   (srccount-idx-count)*sizeof(glbInt64));
		}
		break;
	case GLB_DATATYPE_FLOAT:
		{
			memcpy((glbFloat*)dstvalue,
				   (glbFloat*)srcvalue,
				   idx*sizeof(glbFloat));
			memcpy((glbFloat*)dstvalue+idx,
				   (glbFloat*)srcvalue+idx+count,
				   (srccount-idx-count)*sizeof(glbFloat));
		}
		break;
	case GLB_DATATYPE_DOUBLE:
	case GLB_DATATYPE_DATETIME:
		{
			memcpy((glbDouble*)dstvalue,
				   (glbDouble*)srcvalue,
				   idx*sizeof(glbDouble));
			memcpy((glbDouble*)dstvalue+idx,
				   (glbDouble*)srcvalue+idx+count,
				   (srccount-idx-count)*sizeof(glbDouble));
		}
		break;
	case GLB_DATATYPE_VECTOR:
		{
			memcpy((glbDouble*)dstvalue,
				   (glbDouble*)srcvalue,
				   idx*3*sizeof(glbDouble));
			memcpy((glbDouble*)dstvalue+idx*3,
				   (glbDouble*)srcvalue+(idx+count)*3,
				   (srccount-idx-count)*3*sizeof(glbDouble));
		}
		break;
	case GLB_DATATYPE_STRING:
	case GLB_DATATYPE_GEOMETRY:
	case GLB_DATATYPE_BLOB:
	case GLB_DATATYPE_UNKNOWN:
	default:break;
	}
	return ;
}

void  CopyInsertGeoValue
	  (
	       void*           dstvalue,
		   void*           srcvalue,
		   glbInt32        srccount,
		   glbInt32        idx,
		   glbInt32        count,
		   GlbDataTypeEnum ftype)
{
	switch(ftype)
	{	
	case GLB_DATATYPE_BYTE:
		{
			memcpy((glbByte*)dstvalue,
				   (glbByte*)srcvalue,
				   idx*sizeof(glbByte));
			memcpy((glbByte*)dstvalue+idx+count,
				   (glbByte*)srcvalue+idx,
				   (srccount-idx)*sizeof(glbByte));
		}
		break;
	/*case GLB_DATATYPE_CHAR:
		{
			memcpy((glbChar*)dstvalue,
				   (glbChar*)srcvalue,
				   idx*sizeof(glbChar));
			memcpy((glbChar*)dstvalue+idx+count,
				   (glbChar*)srcvalue+idx,
				   (srccount-idx)*sizeof(glbChar));
		}
		break;*/
	case GLB_DATATYPE_BOOLEAN:
		{
			memcpy((glbBool*)dstvalue,
				   (glbBool*)srcvalue,
				   idx*sizeof(glbBool));
			memcpy((glbBool*)dstvalue+idx+count,
				   (glbBool*)srcvalue+idx,
				   (srccount-idx)*sizeof(glbBool));
		}
		break;
	case GLB_DATATYPE_INT16:
		{
			memcpy((glbInt16*)dstvalue,
				   (glbInt16*)srcvalue,
				   idx*sizeof(glbInt16));
			memcpy((glbInt16*)dstvalue+idx+count,
				   (glbInt16*)srcvalue+idx,
				   (srccount-idx)*sizeof(glbInt16));
		}
		break;
	case GLB_DATATYPE_INT32:
		{
			memcpy((glbInt32*)dstvalue,
				   (glbInt32*)srcvalue,
				   idx*sizeof(glbInt32));
			memcpy((glbInt32*)dstvalue+idx+count,
				   (glbInt32*)srcvalue+idx,
				   (srccount-idx)*sizeof(glbInt32));
		}
		break;
	case GLB_DATATYPE_INT64:
		{
			memcpy((glbInt64*)dstvalue,
				   (glbInt64*)srcvalue,
				   idx*sizeof(glbInt64));
			memcpy((glbInt64*)dstvalue+idx+count,
				   (glbInt64*)srcvalue+idx,
				   (srccount-idx)*sizeof(glbInt64));
		}
		break;
	case GLB_DATATYPE_FLOAT:
		{
			memcpy((glbFloat*)dstvalue,
				   (glbFloat*)srcvalue,
				   idx*sizeof(glbFloat));
			memcpy((glbFloat*)dstvalue+idx+count,
				   (glbFloat*)srcvalue+idx,
				   (srccount-idx)*sizeof(glbFloat));
		}
		break;
	case GLB_DATATYPE_DOUBLE:
	case GLB_DATATYPE_DATETIME:
		{
			memcpy((glbDouble*)dstvalue,
				   (glbDouble*)srcvalue,
				   idx*sizeof(glbDouble));
			memcpy((glbDouble*)dstvalue+idx+count,
				   (glbDouble*)srcvalue+idx,
				   (srccount-idx)*sizeof(glbDouble));
		}
		break;
	case GLB_DATATYPE_VECTOR:
		{
			memcpy((glbDouble*)dstvalue,
				   (glbDouble*)srcvalue,
				   idx*3*sizeof(glbDouble));
			memcpy((glbDouble*)dstvalue+(idx+count)*3,
				   (glbDouble*)srcvalue+idx*3,
				   (srccount-idx)*3*sizeof(glbDouble));
		}
		break;
	case GLB_DATATYPE_STRING:
	case GLB_DATATYPE_GEOMETRY:
	case GLB_DATATYPE_BLOB:
	case GLB_DATATYPE_UNKNOWN:
	default:break;
	}
	return ;
}

void  GetGeoValue(GLBVARIANT& dstvalue,void* srcvalue,glbInt32 pidx,GlbDataTypeEnum ftype)
{
	dstvalue.isarray = false;
	switch(ftype)
	{
	case GLB_DATATYPE_BYTE:
		{
			dstvalue.isnull  = false;
			dstvalue.vt      = GLB_DATATYPE_BYTE;
			glbByte* _values = (glbByte*)srcvalue;
			dstvalue.bVal    = *(_values+pidx);
			return;
		}
	/*case GLB_DATATYPE_CHAR:
		{
			dstvalue.isnull = false;
			dstvalue.vt      = GLB_DATATYPE_CHAR;
			glbChar* _values = (glbChar*)srcvalue;
			dstvalue.cVal    = *(_values+pidx);
			return;
		}*/
	case GLB_DATATYPE_BOOLEAN:
		{
			dstvalue.isnull  = false;
			dstvalue.vt      = GLB_DATATYPE_BOOLEAN;
			glbBool* _values = (glbBool*)srcvalue;
			dstvalue.blVal   = *(_values+pidx);
			return;
		}
	case GLB_DATATYPE_INT16:
		{
			dstvalue.isnull   = false;
			dstvalue.vt       = GLB_DATATYPE_INT16;
			glbInt16* _values = (glbInt16*)srcvalue;
			dstvalue.iVal     = *(_values+pidx);
			return;
		}
	case GLB_DATATYPE_INT32:
		{
			dstvalue.isnull   = false;
			dstvalue.vt       = GLB_DATATYPE_INT32;
			glbInt32* _values = (glbInt32*)srcvalue;
			dstvalue.lVal     = *(_values+pidx);
			return;
		}
	case GLB_DATATYPE_INT64:
		{
			dstvalue.isnull   = false;
			dstvalue.vt       = GLB_DATATYPE_INT64;
			glbInt64* _values = (glbInt64*)srcvalue;
			dstvalue.llVal    = *(_values+pidx);
			return;
		}
	case GLB_DATATYPE_FLOAT:
		{
			dstvalue.isnull   = false;
			dstvalue.vt       = GLB_DATATYPE_FLOAT;
			glbFloat* _values = (glbFloat*)srcvalue;
			dstvalue.fVal     = *(_values+pidx);
			return;
		}
	case GLB_DATATYPE_DOUBLE:
		{
			dstvalue.isnull    = false;
			dstvalue.vt        = GLB_DATATYPE_DOUBLE;
			glbDouble* _values = (glbDouble*)srcvalue;
			dstvalue.dblVal    = *(_values+pidx);
			return;
		}
	case GLB_DATATYPE_DATETIME:
		{
			dstvalue.isnull    = false;
			dstvalue.vt        = GLB_DATATYPE_DATETIME;
			glbDouble* _values = (glbDouble*)srcvalue;
			dstvalue.dblVal    = *(_values+pidx);
			return;
		}
	case GLB_DATATYPE_VECTOR:
		{
			dstvalue.isnull    = false;
			dstvalue.vt        = GLB_DATATYPE_VECTOR;
			glbDouble* _values = (glbDouble*)srcvalue;
			glbDouble* _pvalue = (glbDouble*)&dstvalue.vector;
			memcpy(_pvalue,_values+3*pidx,sizeof(glbDouble)*3);
			return;
		}
	case GLB_DATATYPE_STRING:
	case GLB_DATATYPE_GEOMETRY:
	case GLB_DATATYPE_BLOB:
	case GLB_DATATYPE_UNKNOWN:
	default:break;
	}
	return ;
}

void  GetGeoValues(void** dstvalue,void* srcvalue,glbInt32 count,GlbDataTypeEnum ftype)
{
	switch(ftype)
	{	
	case GLB_DATATYPE_BYTE:
		{
			glbByte* _values  = (glbByte*)srcvalue;
			glbByte** _pvalue = (glbByte**)dstvalue;
			*_pvalue = (glbByte*)malloc(sizeof(glbByte)*count);
			for (glbInt32 i = 0; i < count; i++)
			{
				(*_pvalue)[i] = *_values;
			}
			return;
		}
	/*case GLB_DATATYPE_CHAR:
		{
			glbChar* _values = (glbChar*)srcvalue;
			glbChar** _pvalue = (glbChar**)dstvalue;
			*_pvalue = (glbChar*)malloc(sizeof(glbChar)*count);
			for (glbInt32 i = 0; i < count; i++)
			{
				(*_pvalue)[i] = *_values;
			}
			return;
		}*/
	case GLB_DATATYPE_BOOLEAN:
		{
			glbBool* _values = (glbBool*)srcvalue;
			glbBool** _pvalue = (glbBool**)dstvalue;
			*_pvalue = (glbBool*)malloc(sizeof(glbBool)*count);
			for (glbInt32 i = 0; i < count; i++)
			{
				(*_pvalue)[i] = *_values;
			}
			return;
		}
	case GLB_DATATYPE_INT16:
		{
			glbInt16* _values = (glbInt16*)srcvalue;
			glbInt16** _pvalue = (glbInt16**)dstvalue;
			*_pvalue = (glbInt16*)malloc(sizeof(glbInt16)*count);
			for (glbInt32 i = 0; i < count; i++)
			{
				(*_pvalue)[i] = *_values;
			}
			return;
		}
	case GLB_DATATYPE_INT32:
		{
			glbInt32* _values = (glbInt32*)srcvalue;
			glbInt32** _pvalue = (glbInt32**)dstvalue;
			*_pvalue = (glbInt32*)malloc(sizeof(glbInt32)*count);
			for (glbInt32 i = 0; i < count; i++)
			{
				(*_pvalue)[i] = *_values;
			}
			return;
		}
	case GLB_DATATYPE_INT64:
		{
			glbInt64* _values = (glbInt64*)srcvalue;
			glbInt64** _pvalue = (glbInt64**)dstvalue;
			*_pvalue = (glbInt64*)malloc(sizeof(glbInt64)*count);
			for (glbInt32 i = 0; i < count; i++)
			{
				(*_pvalue)[i] = *_values;
			}
			return;
		}
	case GLB_DATATYPE_FLOAT:
		{
			glbFloat* _values = (glbFloat*)srcvalue;
			glbFloat** _pvalue = (glbFloat**)dstvalue;
			*_pvalue = (glbFloat*)malloc(sizeof(glbFloat)*count);
			for (glbInt32 i = 0; i < count; i++)
			{
				(*_pvalue)[i] = *_values;
			}
			return;
		}
	case GLB_DATATYPE_DOUBLE:
	case GLB_DATATYPE_DATETIME:
		{
			glbDouble* _values = (glbDouble*)srcvalue;
			glbDouble** _pvalue = (glbDouble**)dstvalue;
			*_pvalue = (glbDouble*)malloc(sizeof(glbDouble)*count);
			for (glbInt32 i = 0; i < count; i++)
			{
				(*_pvalue)[i] = *_values;
			}
			return;
		}
	case GLB_DATATYPE_VECTOR:
		{
			glbDouble* _values = (glbDouble*)srcvalue;
			glbDouble** _pvalue = (glbDouble**)dstvalue;
			*_pvalue = (glbDouble*)malloc(sizeof(glbDouble)*count*3);
			for (glbInt32 i = 0; i < count; i++)
			{
				(*_pvalue)[i*3] = *_values;
				(*_pvalue)[i*3+1] = *(_values+1);
				(*_pvalue)[i*3+2] = *(_values+2);
			}
			return;
		}
	case GLB_DATATYPE_STRING:
	case GLB_DATATYPE_GEOMETRY:
	case GLB_DATATYPE_BLOB:
	case GLB_DATATYPE_UNKNOWN:
	default:
		return ;
	}
	return ;
}