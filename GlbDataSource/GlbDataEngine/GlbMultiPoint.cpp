#include "StdAfx.h"
#include "GlbPoint.h"

CGlbMultiPoint::CGlbMultiPoint(glbByte coordDimension/* = 2*/,glbBool hasM/* = false*/)
{
	mpr_pointscoord = NULL;
	mpr_pointscount   = 0;
	mpr_geometryclass =NULL;
	mpr_extent        =NULL;
	mpr_coordDimension=coordDimension;
	mpr_hasM          =hasM;
	mpr_m             =NULL;
}

CGlbMultiPoint::~CGlbMultiPoint()
{
	mpr_geometryclass=NULL;
	Empty();	
}
GlbGeometryTypeEnum CGlbMultiPoint::GetType()
{
	return GLB_GEO_MULTIPOINT;
}
glbBool CGlbMultiPoint::IsEmpty()
{
	if(mpr_pointscount == 0)return true;
	return false;
}
const CGlbExtent*   CGlbMultiPoint::GetExtent()
{
	if(mpr_extent == NULL)
	{
		mpr_extent = new CGlbExtent();
		UpdateExtent();
	}
	return mpr_extent.get();
}
glbUInt32 CGlbMultiPoint::GetSize()
{
	glbUInt32 lsize = sizeof(glbDouble*);
	lsize += sizeof(glbInt32);
	lsize = sizeof(glbDouble)*mpr_pointscount*mpr_coordDimension;

	lsize += sizeof(glbref_ptr<CGlbGeometryClass>);
	lsize += sizeof(vector<void*>);
	lsize += mpr_pointsvalue.size()*sizeof(void*);
	glbUInt32 DTSize = 0;
	for (size_t i=0;i<mpr_pointsvalue.size();i++)
	{
		if(mpr_pointsvalue[i])lsize += glbArraySize(*mpr_pointsvalue[i]);
	}
	lsize += sizeof(glbref_ptr<CGlbExtent>);
	if(mpr_extent.get())
		lsize += sizeof(CGlbExtent);
	lsize += sizeof(glbByte);
	lsize += sizeof(glbBool);
	lsize += sizeof(glbDouble*);
	if(mpr_m && mpr_m)
		lsize += sizeof(glbDouble)*mpr_pointscount;
	return lsize;
}
glbInt32  CGlbMultiPoint::GetCoordDimension()
{
	return mpr_coordDimension;
}
glbBool   CGlbMultiPoint::HasM()
{
	return mpr_hasM;
}
void CGlbMultiPoint::Empty()
{
	if(mpr_pointscoord) free(mpr_pointscoord);
	mpr_pointscoord = NULL;
	mpr_pointscount = 0;
	for (size_t i=0;i<mpr_pointsvalue.size();i++)
	{
		if(mpr_pointsvalue[i])
		{
			glbArrayClean(*mpr_pointsvalue[i]);
			delete mpr_pointsvalue[i];
		}
		mpr_pointsvalue[i] = NULL;
	}
	mpr_pointsvalue.clear();
	if(mpr_geometryclass.get() 
		&& mpr_geometryclass.get()->GetFieldCount()>0)
	{
		glbInt32 cnt = mpr_geometryclass.get()->GetFieldCount();
		mpr_pointsvalue.reserve(cnt);
		for(glbInt32 i=0;i<cnt;i++)
		{
			CGlbField* fd = mpr_geometryclass.get()->GetField(i);
			glbArray*  value= new glbArray();
			value->vt = fd->GetType();
			value->dims=1;
			value->dimSize=0;
			mpr_pointsvalue.push_back(value);
		}
	}
	mpr_extent = NULL;
	if(mpr_m) free(mpr_m);
	mpr_m = NULL;
}
glbBool CGlbMultiPoint::SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls)
{
	if(ctype == GLB_GEOCLASS_POINT)
	{
		mpr_geometryclass = gcls;
		size_t            fcount    =  mpr_geometryclass->GetFieldCount();
		vector<glbArray*> newValues;
		newValues.reserve(fcount);
		for(size_t i=0;i<fcount;i++)newValues.push_back(NULL);

		glbArray*  value      = NULL;
		for(size_t i=0;i<fcount;i++)
		{		
			CGlbField* fld        = mpr_geometryclass.get()->GetField(i);
			GlbDataTypeEnum ftype = fld->GetType();
			if(i<mpr_pointsvalue.size())
			{
				value = mpr_pointsvalue[i];
				if(value->vt == ftype)
				{
					newValues[i]       = value;
					mpr_pointsvalue[i] = NULL;
				}else{
					glbArrayClean(*value);
				    delete value;
				    mpr_pointsvalue[i] = NULL;
					value        = new glbArray();
					value->vt     = ftype;
					value->dims   = 1;
					value->dimSize= 0;
					newValues[i] = value;
				}
			}else{
				value        = new glbArray();
				value->vt     = ftype;
				value->dims   = 1;
				value->dimSize= 0;
				newValues[i] = value;
			}		
		}
		fcount = mpr_pointsvalue.size();
		for(size_t i=0;i<fcount;i++)
		{
			value = mpr_pointsvalue[i];
			if(value)
			{
				glbArrayClean(*value);
				delete value;
				mpr_pointsvalue[i] = NULL;
			}
		}
		mpr_pointsvalue.clear();
		mpr_pointsvalue = newValues;
	}
	return false;
}
const   CGlbGeometryClass* CGlbMultiPoint::GetGeoClass(GlbGeometryClassEnum ctype)
{
	if(ctype == GLB_GEOCLASS_POINT)
	{
		return mpr_geometryclass.get();
	}
	return NULL;
}
glbInt32 CGlbMultiPoint::GetCount()
{
	return mpr_pointscount;
}
glbBool	CGlbMultiPoint::GetPoint(glbInt32 idx,glbDouble* ptx,glbDouble* pty)
{
	if(idx<0 || idx > mpr_pointscount)
	{
		GlbSetLastError(L"索引必须>=0且小与点数目");
		return false;
	}
	if(ptx == NULL || pty == NULL)
	{
		GlbSetLastError(L"参数ptx,pty都不能是NULL");
		return false;
	}
	*ptx = mpr_pointscoord[idx*mpr_coordDimension];
	*pty = mpr_pointscoord[idx*mpr_coordDimension+1];
	return true;
}
glbBool	CGlbMultiPoint::GetPoint(glbInt32 idx,glbDouble* ptx,glbDouble* pty,glbDouble* ptz)
{
	if(mpr_coordDimension == 2)
	{
		GlbSetLastError(L"二维多点没有z值");
		return false;
	}
	if(idx<0 || idx > mpr_pointscount)
	{
		GlbSetLastError(L"索引必须>=0且小与点数目");
		return false;
	}
	if(ptx == NULL || pty == NULL || ptz == NULL)
	{
		GlbSetLastError(L"参数ptx,pty,ptz都不能是NULL");
		return false;
	}
	*ptx = mpr_pointscoord[idx*mpr_coordDimension];
	*pty = mpr_pointscoord[idx*mpr_coordDimension+1];
	*ptz = mpr_pointscoord[idx*mpr_coordDimension+2];
	return true;
}
glbBool	CGlbMultiPoint::AddPoint(glbDouble x,glbDouble y)
{
	return AddPoint(x,y,0.0);
}
glbBool	CGlbMultiPoint::AddPoint(glbDouble x,glbDouble y,glbDouble z)
{
	glbDouble coords[3] = {0.0};
	coords[0] = x;
	coords[1] = y;
	coords[2] = z;
	return AddPoints(coords,1);
}
glbBool CGlbMultiPoint::AddPoints(glbDouble* coords,glbInt32 count)
{
	if(count <=0) return true;
	if(coords == NULL)
	{
		GlbSetLastError(L"参数coords是NULL");
		return false;
	}

	glbDouble*    newCoords = NULL;
	vector<void*> newValues;
	glbDouble*    newM      = NULL;

	//分配内存
	//1. 坐标
	newCoords = (glbDouble*)malloc((mpr_pointscount + count)
		                           *mpr_coordDimension
								   *sizeof(glbDouble));
	if(!newCoords)
	{
		GlbSetLastError(L"内存不足");
		goto newMallocError;
	}
	//2. 字段值
	if(mpr_geometryclass.get() 
		&& mpr_geometryclass.get()->GetFieldCount()>0)
	{
		glbInt32 fcount = mpr_geometryclass.get()->GetFieldCount();
		newValues.reserve(fcount);
		for(glbInt32 i=0;i<fcount;i++)newValues.push_back(NULL);
		for(glbInt32 i=0;i<fcount;i++)
		{
			if(i >= (glbInt32)mpr_pointsvalue.size())
			{
				GlbSetLastError(L"字段数目有变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			if(mpr_pointsvalue[i]->dimSize == 0)continue;
			
			CGlbField*      fld       = mpr_geometryclass.get()->GetField(i);
			GlbDataTypeEnum ftype     = fld->GetType();
			if(ftype != mpr_pointsvalue[i]->vt)
			{
				GlbSetLastError(L"字段类型变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			size_t          ftypesize = GetDataTypeSize(ftype);
			void* nvalue    = malloc(ftypesize*(mpr_pointscount + count));
			if(!nvalue)
			{
				GlbSetLastError(L"内存不足");
				goto newMallocError;
			}
			newValues.push_back(nvalue);
		}		
	}
	//3. 度量值
	if(mpr_hasM)
	{
		newM = (glbDouble*)malloc((mpr_pointscount + count)*sizeof(glbDouble));
		if(NULL == newM)
		{
			GlbSetLastError(L"内存不足");
			goto newMallocError;
		}
	}
	//////////////////内存分配正确////////////////
	//1.坐标赋值
	memcpy(newCoords + mpr_pointscount * mpr_coordDimension,
		   coords,
		   count*sizeof(glbDouble)*mpr_coordDimension);
	if(mpr_pointscoord)
	{
		memcpy(newCoords,
			   mpr_pointscoord,
			   mpr_pointscount*mpr_coordDimension*sizeof(glbDouble));
		//释放旧地址
		free(mpr_pointscoord);
	}
	mpr_pointscoord=newCoords;
	//2.字段值
	size_t fcount = newValues.size();
	for(size_t i=0;i<fcount;i++)
	{
		if(newValues[i]==NULL)continue;
		CGlbField*      fld       = mpr_geometryclass.get()->GetField(i);
		GlbDataTypeEnum ftype     = fld->GetType();
		//缺省值
		SetGeoValue(newValues[i],mpr_pointscount,count,fld->GetDefault(),ftype);
		
		void** pValue = NULL;
		switch(ftype)
		{
		case GLB_DATATYPE_BYTE:    pValue = (void**)&mpr_pointsvalue[i]->pBytes;break;
		//case GLB_DATATYPE_CHAR:    pValue = (void**)&mpr_pointsvalue[i]->pChars;break;
		case GLB_DATATYPE_BOOLEAN: pValue = (void**)&mpr_pointsvalue[i]->pBools;break;
		case GLB_DATATYPE_INT16:   pValue = (void**)&mpr_pointsvalue[i]->pInts;break;
		case GLB_DATATYPE_INT32:   pValue = (void**)&mpr_pointsvalue[i]->pLongs;break;
		case GLB_DATATYPE_INT64:   pValue = (void**)&mpr_pointsvalue[i]->pLLongs;break;
		case GLB_DATATYPE_FLOAT:   pValue = (void**)&mpr_pointsvalue[i]->pFloats;break;
		case GLB_DATATYPE_DOUBLE:  pValue = (void**)&mpr_pointsvalue[i]->pDoubles;break;
		case GLB_DATATYPE_DATETIME:pValue = (void**)&mpr_pointsvalue[i]->pDates;break;
		case GLB_DATATYPE_VECTOR:  pValue = (void**)&mpr_pointsvalue[i]->pVectors;break;
		//case GLB_DATATYPE_STRING:
		//case GLB_DATATYPE_GEOMETRY:
		//case GLB_DATATYPE_BLOB:
		}
		if(*pValue)
		{
			CopyInsertGeoValue(newValues[i],*pValue,mpr_pointscount,mpr_pointscount,count,ftype);			
			//释放旧地址
			glbArrayClean(*mpr_pointsvalue[i]);
		}
		mpr_pointsvalue[i]->vt = ftype;
		mpr_pointsvalue[i]->dims=1;
		mpr_pointsvalue[i]->dimSize=mpr_pointscount + count;
		*pValue = newValues[i];
	}
	//3.度量
	if(mpr_hasM)
	{
		memset(newM + mpr_pointscount,0,sizeof(glbDouble)*count);
		if(mpr_m)
		{
			memcpy(newM,
				   mpr_m,
				   mpr_pointscount*sizeof(glbDouble));
			//释放旧地址
			free(mpr_m);
		}
		mpr_m = newM;
	}
	//修改基数
	mpr_pointscount += count;
	// 更新外包
	if(mpr_extent.get())
	{
		glbDouble minx = 0.0,miny = 0.0;
		glbDouble maxx = 0.0,maxy = 0.0;
		glbDouble minz = 0.0,maxz = 0.0;
		mpr_extent->GetMin(&minx,&miny,&minz);
		mpr_extent->GetMax(&maxx,&maxy,&maxz);
		
		glbDouble x=0.0,y=0.0,z=0.0;
		for(glbInt32 i=0;i<count;i++)
		{
			x = coords[i*mpr_coordDimension];
			y = coords[i*mpr_coordDimension+1];
			if(mpr_coordDimension == 3)
				z = coords[i*mpr_coordDimension+2];

			minx = min(minx,x);
			maxx = max(maxx,x);
			miny = min(miny,y);
			maxy = max(maxy,y);
			minz = min(minz,z);
			maxz = max(maxz,z);
		}
		mpr_extent->SetMin(minx,miny,minz);
		mpr_extent->SetMax(maxx,maxy,maxz);
	}
	return true;

newMallocError://分配新的内存出错
	if(newCoords)free(newCoords);
	vector<void*>::iterator itr    = newValues.begin();
	vector<void*>::iterator itrEnd = newValues.end();
	for(itr;itr!=itrEnd;itr++)
	{
		if((*itr))free((*itr));
	}
	if(newM)free(newM);
	return false;
}
glbBool	CGlbMultiPoint::DeletePoint(glbInt32 idx)
{
	return DeletePoints(idx,1);
}
glbBool	CGlbMultiPoint::DeletePoints(glbInt32 idx,glbInt32 count)
{
	if(count <=0)return true;

	if (IsEmpty())
	{
		GlbSetLastError(L"没有点");
		return false;
	}
	if (idx < 0 || idx >= mpr_pointscount)
	{
		GlbSetLastError(L"索引必须>=0且小于点数目");
		return false;
	}
	if(idx+count>= mpr_pointscount)
		count = mpr_pointscount - idx;
	if(mpr_pointscount == count)
	{
		Empty();
		return true;
	}
	// 分配新的内存
	glbDouble*    newCoords = NULL; // 新的坐标存储数组
	vector<void*> newValues;        // 新的属性值存储
	glbDouble*    newM      = NULL; // 新的度量值
	//1.坐标
	newCoords = (glbDouble*)malloc(sizeof(glbDouble)
		                           *(mpr_pointscount - count)
								   *mpr_coordDimension);
	if(NULL == newCoords)
	{
		GlbSetLastError(L"内存不足");
		goto newMallocError;
	}
	//2.属性
	if(mpr_geometryclass.get() 
		&& mpr_geometryclass.get()->GetFieldCount()>0)
	{
		glbInt32 fcount = mpr_geometryclass.get()->GetFieldCount();
		newValues.reserve(fcount);
		for(glbInt32 i=0;i<fcount;i++)newValues.push_back(NULL);
		for(glbInt32 i=0;i<fcount;i++)
		{
			if(i >= (glbInt32)mpr_pointsvalue.size())
			{
				GlbSetLastError(L"字段数目有变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			if(mpr_pointsvalue[i]->dimSize == 0)continue;
			
			CGlbField*      fld   = mpr_geometryclass.get()->GetField(i);
			GlbDataTypeEnum ftype = fld->GetType();
			if(ftype != mpr_pointsvalue[i]->vt)
			{
				GlbSetLastError(L"字段类型变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			size_t          ftypesize = GetDataTypeSize(ftype);
			void* nvalue = malloc(ftypesize*(mpr_pointscount-count));
			if(!nvalue)
			{
				GlbSetLastError(L"内存不足");
				goto newMallocError;
			}
			newValues.push_back(nvalue);
		}		
	}
	//3.度量值
	if(mpr_hasM)
	{
		newM = (glbDouble*)malloc((mpr_pointscount - count)*sizeof(glbDouble));
		if(NULL == newM)goto newMallocError;
	}
	//////内存分配正确
	//1.拷贝坐标串
	memcpy(newCoords,
		   mpr_pointscoord,
		   idx*mpr_coordDimension*sizeof(glbDouble));
	memcpy(newCoords          + idx*mpr_coordDimension,
		   mpr_pointscoord    + (idx + count)*mpr_coordDimension,
		   (mpr_pointscount - idx - count)*mpr_coordDimension*sizeof(glbDouble));	
	free(mpr_pointscoord);
	mpr_pointscoord = newCoords;
	//2.拷贝属性值
	size_t fcount = mpr_pointsvalue.size();
	for(size_t i=0;i<fcount;i++)
	{
		if(newValues[i]==NULL)continue;
		CGlbField* fld = mpr_geometryclass->GetField(i);
		GlbDataTypeEnum ftype = fld->GetType();
		void** pValue = NULL;
		switch(ftype)
		{
		case GLB_DATATYPE_BYTE:    pValue = (void**)&mpr_pointsvalue[i]->pBytes;break;
		//case GLB_DATATYPE_CHAR:    pValue = (void**)&mpr_pointsvalue[i]->pChars;break;
		case GLB_DATATYPE_BOOLEAN: pValue = (void**)&mpr_pointsvalue[i]->pBools;break;
		case GLB_DATATYPE_INT16:   pValue = (void**)&mpr_pointsvalue[i]->pInts;break;
		case GLB_DATATYPE_INT32:   pValue = (void**)&mpr_pointsvalue[i]->pLongs;break;
		case GLB_DATATYPE_INT64:   pValue = (void**)&mpr_pointsvalue[i]->pLLongs;break;
		case GLB_DATATYPE_FLOAT:   pValue = (void**)&mpr_pointsvalue[i]->pFloats;break;
		case GLB_DATATYPE_DOUBLE:  pValue = (void**)&mpr_pointsvalue[i]->pDoubles;break;
		case GLB_DATATYPE_DATETIME:pValue = (void**)&mpr_pointsvalue[i]->pDates;break;
		case GLB_DATATYPE_VECTOR:  pValue = (void**)&mpr_pointsvalue[i]->pVectors;break;
		//case GLB_DATATYPE_STRING:
		//case GLB_DATATYPE_GEOMETRY:
		//case GLB_DATATYPE_BLOB:
		}
		CopyRemoveGeoValue(newValues[i],*pValue,mpr_pointscount,idx,count,ftype);
		glbArrayClean(*mpr_pointsvalue[i]);
		mpr_pointsvalue[i]->vt = ftype;
		mpr_pointsvalue[i]->dims=1;
		mpr_pointsvalue[i]->dimSize=mpr_pointscount - count;
		*pValue = newValues[i];
	}
	//3.拷贝度量值
	if(mpr_hasM)
	{
		memcpy(newM,
			   mpr_m,
			   idx*sizeof(glbDouble));
		memcpy(newM + idx,
			   mpr_m             +(idx + count),
			   (mpr_pointscount-idx - count)*sizeof(glbDouble));		
		free(mpr_m);
		mpr_m = newM;
	}
	//修改基数
	mpr_pointscount -= count;
	mpr_extent      = NULL;
	return true;
newMallocError://分配新的内存出错
	if(newCoords)free(newCoords);
	vector<void*>::iterator itr    = newValues.begin();
	vector<void*>::iterator itrEnd = newValues.end();
	for(itr;itr!=itrEnd;itr++)
	{
		if((*itr))free((*itr));
	}
	if(newM)free(newM);
	return false;
}
glbBool	CGlbMultiPoint::SetPoint(glbInt32 idx,glbDouble x,glbDouble y)
{
	if (IsEmpty())
	{
		GlbSetLastError(L"没有点");
		return false;
	}
	if (idx < 0 || idx >= mpr_pointscount)
	{
		GlbSetLastError(L"索引必须>=0且小于点数目");
		return false;
	}
	mpr_pointscoord[idx*mpr_coordDimension]   = x;
	mpr_pointscoord[idx*mpr_coordDimension+1] = y;
	return true;
}
glbBool	CGlbMultiPoint::SetPoint(glbInt32 idx,glbDouble x,glbDouble y,glbDouble z)
{
	if (IsEmpty())
	{
		GlbSetLastError(L"没有点");
		return false;
	}
	if (idx < 0 || idx >= mpr_pointscount)
	{
		GlbSetLastError(L"索引必须>=0且小于点数目");
		return false;
	}
	mpr_pointscoord[idx*mpr_coordDimension]   = x;
	mpr_pointscoord[idx*mpr_coordDimension+1] = y;
	if(mpr_coordDimension == 3)
		mpr_pointscoord[idx*mpr_coordDimension+2] = z;
	return true;
}
const glbDouble*	CGlbMultiPoint::GetPoints()
{
	return mpr_pointscoord;
}
glbBool CGlbMultiPoint::GetM(glbInt32 idx,glbDouble* m)
{
	if(mpr_hasM == false)
	{
		GlbSetLastError(L"没有度量值");
		return false;
	}
	if(IsEmpty())
	{
		GlbSetLastError(L"对象无点");
		return false;
	}
	if(idx<0|| idx>=mpr_pointscount)
	{
		GlbSetLastError(L"索引必须>=0且小于点数目");
		return false;
	}
	*m = mpr_m[idx];
	return true;
}
glbBool CGlbMultiPoint::SetM(glbInt32 idx,glbDouble m)
{
	if(mpr_hasM == false)
	{
		GlbSetLastError(L"没有度量值");
		return false;
	}
	if(IsEmpty())
	{
		GlbSetLastError(L"对象无点");
		return false;
	}
	if(idx<0|| idx>=mpr_pointscount)
	{
		GlbSetLastError(L"索引必须>=0且小于点数目");
		return false;
	}
	mpr_m[idx] = m;
	return true;
}
const glbDouble* CGlbMultiPoint::GetMs()
{
	return mpr_m;
}
glbBool	CGlbMultiPoint::GetPointValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有点");
		return false;
	}
	if(pidx<0||pidx>=mpr_pointscount)
	{
		GlbSetLastError(L"pidx必须>=0且小于点数目");
		return false;
	}
	if(!mpr_geometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	
	if(fidx<0||fidx>=mpr_geometryclass.get()->GetFieldCount())
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");
		return false;
	}
	if(fidx >= (glbInt32)mpr_pointsvalue.size())
	{
		GlbSetLastError(L"字段数目变化,需要重新SetPointClass");
		return false;
	}
	CGlbField*       fld   = mpr_geometryclass.get()->GetField(fidx);
	GlbDataTypeEnum  ftype = fld->GetType();
	if(mpr_pointsvalue[fidx]->vt != ftype)
	{
		GlbSetLastError(L"字段类型变化,需要重新SetPointClass");
		return false;
	}
	if(mpr_pointsvalue[fidx]->dimSize == 0)
	{//取缺省值
		GetGeoValue(pvalue,(void*)fld->GetDefault(),0,ftype);
	}
	else
	{
		void* pValue = NULL;
		switch(ftype)
		{
		case GLB_DATATYPE_BYTE:    pValue = mpr_pointsvalue[fidx]->pBytes;break;
		//case GLB_DATATYPE_CHAR:    pValue = mpr_pointsvalue[fidx]->pChars;break;
		case GLB_DATATYPE_BOOLEAN: pValue = mpr_pointsvalue[fidx]->pBools;break;
		case GLB_DATATYPE_INT16:   pValue = mpr_pointsvalue[fidx]->pInts;break;
		case GLB_DATATYPE_INT32:   pValue = mpr_pointsvalue[fidx]->pLongs;break;
		case GLB_DATATYPE_INT64:   pValue = mpr_pointsvalue[fidx]->pLLongs;break;
		case GLB_DATATYPE_FLOAT:   pValue = mpr_pointsvalue[fidx]->pFloats;break;
		case GLB_DATATYPE_DOUBLE:  pValue = mpr_pointsvalue[fidx]->pDoubles;break;
		case GLB_DATATYPE_DATETIME:pValue = mpr_pointsvalue[fidx]->pDates;break;
		case GLB_DATATYPE_VECTOR:  pValue = mpr_pointsvalue[fidx]->pVectors;break;
		//case GLB_DATATYPE_STRING:
		//case GLB_DATATYPE_GEOMETRY:
		//case GLB_DATATYPE_BLOB:
		}
		GetGeoValue(pvalue,pValue,pidx,ftype);
	}
	return true;
}
glbBool	CGlbMultiPoint::GetPointValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(!mpr_geometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}

	glbInt32 fidx = mpr_geometryclass.get()->FindFieldIndex(fname);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}
	return GetPointValue(fidx,pidx,pvalue);
}
glbBool	CGlbMultiPoint::SetPointValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有点");
		return false;
	}
	if(!mpr_geometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(pidx<0||pidx>=mpr_pointscount)
	{
		GlbSetLastError(L"pidx必须>=0且小于顶点数目");
		return false;
	}
	
	if(fidx<0||fidx>=mpr_geometryclass.get()->GetFieldCount())
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");		
		return false;
	}
	if(fidx >= (glbInt32)mpr_pointsvalue.size())
	{
		GlbSetLastError(L"字段数目变化,需要重新SetPointClass");
		return false;
	}
	CGlbField* fld        = mpr_geometryclass.get()->GetField(fidx);
	GlbDataTypeEnum ftype = fld->GetType();
	if(mpr_pointsvalue[fidx]->vt != ftype)
	{
		GlbSetLastError(L"字段类型变化,需要重新SetPointClass");
		return false;
	}
	if (ftype != pvalue.vt)
	{
		GlbSetLastError(L"值类型与字段类型不一致");
		return false;
	}
	void**  pValue = NULL;
	switch(ftype)
	{
	case GLB_DATATYPE_BYTE:   pValue = (void**)&mpr_pointsvalue[fidx]->pBytes;break;
	//case GLB_DATATYPE_CHAR:   pValue = (void**)&mpr_pointsvalue[fidx]->pChars;break;
	case GLB_DATATYPE_BOOLEAN:pValue = (void**)&mpr_pointsvalue[fidx]->pBools;break;
	case GLB_DATATYPE_INT16:  pValue = (void**)&mpr_pointsvalue[fidx]->pInts;break;
	case GLB_DATATYPE_INT32:  pValue = (void**)&mpr_pointsvalue[fidx]->pLongs;break;
	case GLB_DATATYPE_INT64:  pValue = (void**)&mpr_pointsvalue[fidx]->pLLongs;break;
	case GLB_DATATYPE_FLOAT:  pValue = (void**)&mpr_pointsvalue[fidx]->pFloats;break;
	case GLB_DATATYPE_DOUBLE: pValue = (void**)&mpr_pointsvalue[fidx]->pDoubles;break;
	case GLB_DATATYPE_DATETIME:pValue = (void**)&mpr_pointsvalue[fidx]->pDates;break;
	case GLB_DATATYPE_VECTOR: pValue = (void**)&mpr_pointsvalue[fidx]->pVectors;break;
	//case GLB_DATATYPE_STRING:
	//case GLB_DATATYPE_GEOMETRY:
	//case GLB_DATATYPE_BLOB:
	}
	
	if(*pValue == NULL || mpr_pointsvalue[fidx]->dimSize == 0)
	{//初始化缺省值
		size_t ftypesize = GetDataTypeSize(ftype);
		*pValue          = malloc(mpr_pointscount*ftypesize);
		if(*pValue == NULL)
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
		mpr_pointsvalue[fidx]->dimSize = mpr_pointscount;
		SetGeoValue(*pValue,0,mpr_pointscount,fld->GetDefault(),ftype);		
	}
	SetGeoValueVariant(*pValue,pidx,1,pvalue,ftype);
	return true;
}
glbBool	CGlbMultiPoint::SetPointValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(!fname)
	{
		GlbSetLastError(L"fname是NULL");
		return false;
	}
	if(!mpr_geometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	
	glbInt32 fidx = mpr_geometryclass.get()->FindFieldIndex(fname);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}
	return SetPointValue(fidx,pidx,pvalue);
}
const glbArray*	CGlbMultiPoint::GetPointsValue(glbInt32 fidx)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有点");
		return false;
	}
	if(!mpr_geometryclass)
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(fidx<0||fidx>=mpr_geometryclass->GetFieldCount())
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");
		return false;
	}
	if(fidx >= (glbInt32)mpr_pointsvalue.size())
	{
		GlbSetLastError(L"字段数目有变化,需要重新SetPointClass");
		return NULL;
	}
	CGlbField* fld    = mpr_geometryclass.get()->GetField(fidx);
	if(fld->GetType() != mpr_pointsvalue[fidx]->vt)
	{
		GlbSetLastError(L"字段类型有变后,需要重新SetPointClass");
		return NULL;
	}
	
	return mpr_pointsvalue[fidx];
}
const glbArray*	CGlbMultiPoint::GetPointsValueByName(glbWChar* fname)
{
	if(!fname)
	{
		GlbSetLastError(L"fname是NULL");
		return false;
	}
	if(!mpr_geometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	
	glbInt32 fidx = mpr_geometryclass.get()->FindFieldIndex(fname);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}

	return GetPointsValue(fidx);
}
glbBool	CGlbMultiPoint::SetPointsValue (glbInt32 fidx,glbArray& pvalues)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有点");
		return false;
	}
	if(!mpr_geometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(fidx<0||fidx>=mpr_geometryclass.get()->GetFieldCount())
	{
		GlbSetLastError(L"字段索引必须>=0且小于字段数目");
		return false;
	}
	if(fidx >= (glbInt32)mpr_pointsvalue.size())
	{
		GlbSetLastError(L"字段数目变化,需要重新SetPointClass");
		return false;
	}
	if( !(  pvalues.dimSize == mpr_pointscount		
		||  pvalues.dimSize ==  0
		))
	{
		GlbSetLastError(L"pvalues.dimSizes必须等于点数目或0");
		return false;
	}
	CGlbField* fld=mpr_geometryclass.get()->GetField(fidx);
	GlbDataTypeEnum ftype = fld->GetType();
	if(pvalues.dimSize == 0)
	{
		glbArrayClean(*mpr_pointsvalue[fidx]);
		mpr_pointsvalue[fidx]->vt     =ftype;
		mpr_pointsvalue[fidx]->dims   =1;
		mpr_pointsvalue[fidx]->dimSize=0;
		return true;
	}
	if(pvalues.dims!=1)
	{
		GlbSetLastError(L"pvalues.dims维度必须是1");
		return false;
	}
	if (ftype != pvalues.vt)
	{
		GlbSetLastError(L"值类型与字段类型不相符");
		return false;
	}
	if(ftype != mpr_pointsvalue[fidx]->vt)
	{
		GlbSetLastError(L"字段类型变化,需要重新SetPointClass");
		return NULL;
	}
	
	void** pValue   = NULL;
	void*  pSrcValue= NULL;
	switch(ftype)
	{
	case GLB_DATATYPE_BYTE:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pBytes;
			pSrcValue = pvalues.pBytes;
		}break;
	/*case GLB_DATATYPE_CHAR:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pChars;
			pSrcValue = pvalues.pChars;
		}break;*/
	case GLB_DATATYPE_BOOLEAN:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pBools;
			pSrcValue = pvalues.pBools;
		}break;
	case GLB_DATATYPE_INT16:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pInts;
			pSrcValue = pvalues.pInts;
		}break;
	case GLB_DATATYPE_INT32:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pLongs;
			pSrcValue = pvalues.pLongs;
		}break;
	case GLB_DATATYPE_INT64:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pLLongs;
			pSrcValue = pvalues.pLLongs;
		}break;
	case GLB_DATATYPE_FLOAT:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pFloats;
			pSrcValue = pvalues.pFloats;
		}break;
	case GLB_DATATYPE_DOUBLE:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pDoubles;
			pSrcValue = pvalues.pDoubles;
		}break;
	case GLB_DATATYPE_DATETIME:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pDates;
			pSrcValue = pvalues.pDates;
		}break;
	case GLB_DATATYPE_VECTOR:
		{
			pValue = (void**)&mpr_pointsvalue[fidx]->pVectors;
			pSrcValue = pvalues.pVectors;
		}break;
	//case GLB_DATATYPE_STRING:
	//case GLB_DATATYPE_GEOMETRY:
	//case GLB_DATATYPE_BLOB:
	}
	size_t ftypesize = GetDataTypeSize(ftype);
	if(*pValue == NULL || mpr_pointsvalue[fidx]->dimSize==0)
	{		
	    *pValue = malloc(mpr_pointscount*ftypesize);
		if(*pValue == NULL)
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
		mpr_pointsvalue[fidx]->dimSize = mpr_pointscount;
	}
	memcpy(*pValue,pSrcValue,mpr_pointscount*ftypesize);
	return true;
}
glbBool	CGlbMultiPoint::SetPointsValueByName (glbWChar* fname,glbArray& pvalues)
{
	if(fname == NULL)
	{
		GlbSetLastError(L"fname是NULL");
		return false;
	}
	if(!mpr_geometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	glbInt32 fidx = mpr_geometryclass.get()->FindFieldIndex(fname);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}

	return SetPointsValue(fidx,pvalues);
}
void CGlbMultiPoint::UpdateExtent()
{
	glbDouble x=0.0,y=0.0,z=0.0;
	if(IsEmpty()) 
		return;
	if(mpr_extent.get() == NULL)return;

	glbDouble minx,miny,minz,maxx,maxy,maxz;
	minx = maxx = mpr_pointscoord[0];
	miny = maxy = mpr_pointscoord[1];
	if(mpr_coordDimension == 3)
		minz = maxz = mpr_pointscoord[2];
	else
		minz = maxz = 0.0;
	for (glbInt32 i=0;i<mpr_pointscount;i++)
	{
		x=mpr_pointscoord[i*mpr_coordDimension];
		y=mpr_pointscoord[i*mpr_coordDimension+1];
		if(mpr_coordDimension == 3)
			z=mpr_pointscoord[i*mpr_coordDimension+2];
		if(x<minx) minx=x;
		if(x>maxx) maxx=x;
		if(y<miny) miny=y;
		if(y>maxy) maxy=y;
		if(z<minz) minz=z;
		if(z>maxz) maxz=z;
	}
	mpr_extent->SetMin(minx,miny,minz);
	mpr_extent->SetMax(maxx,maxy,maxz);
}
