#include "StdAfx.h"
#include "GlbTin.h"

CGlbTin::CGlbTin(glbBool hasM/* = false*/)
{	
	mpr_vergeometryclass=NULL;
	mpr_trigeometryclass=NULL;
	mpr_vertexcoord     =NULL;
	mpr_trivertex       =NULL;
	mpr_extent          =NULL;
	mpr_hasM            =hasM;
	mpr_m               =NULL;
	mpr_vertexescount   =0;
	mpr_trianglecount   =0;
}

CGlbTin::~CGlbTin()
{
	mpr_vergeometryclass=NULL;
	mpr_trigeometryclass=NULL;
	Empty();
}

CGlbExtent* CGlbTin::GetExtent()
{
	if(IsEmpty())return NULL;
	if (mpr_extent == NULL)
	{
		mpr_extent = new CGlbExtent();
		UpdateExtent();		
	}
	return mpr_extent.get();
}

GlbGeometryTypeEnum CGlbTin::GetType()
{
	return GLB_GEO_TIN;
}

glbBool CGlbTin::IsEmpty()
{
	if(mpr_vertexescount==0)
		return true;
	return false;
}

glbUInt32 CGlbTin::GetSize()
{
	glbUInt32 lsize = sizeof(glbInt32);
	lsize += sizeof(glbInt32);
	lsize += sizeof(glbref_ptr<CGlbGeometryClass>);
	lsize += sizeof(glbref_ptr<CGlbGeometryClass>);
	lsize += sizeof(glbDouble*);
	if(mpr_vertexcoord)
		lsize += sizeof(glbDouble)*mpr_vertexescount*3;
	lsize += sizeof(glbInt32*);
	lsize += sizeof(glbInt32)*mpr_trianglecount*3;
	
	for (size_t i=0;i<mpr_verpvalue.size();i++)
	{
		if(mpr_verpvalue[i])lsize += glbArraySize(*mpr_verpvalue[i]);
	}
	lsize += sizeof(void*)*mpr_verpvalue.size();

	for (size_t i=0;i<mpr_tripvalue.size();i++)
	{
		if(mpr_tripvalue[i])lsize += glbArraySize(*mpr_tripvalue[i]);		
	}
	lsize += sizeof(void*)*mpr_tripvalue.size();

	lsize += sizeof(glbref_ptr<CGlbExtent>);
	if(mpr_extent.get() != NULL)
		lsize += sizeof(CGlbExtent);

	lsize += sizeof(glbBool);

	if(mpr_hasM && mpr_m)
		lsize += sizeof(glbDouble)*mpr_vertexescount;
	return lsize;
}

glbInt32 CGlbTin::GetCoordDimension()
{
	return 3;
}

glbBool  CGlbTin::HasM()
{
	return mpr_hasM;
}
void CGlbTin::EmptyVertex()
{
	mpr_vertexescount   =0;
	if(mpr_vertexcoord)free(mpr_vertexcoord);
	mpr_vertexcoord = NULL;

	for (size_t i=0;i<mpr_verpvalue.size();i++)
	{
		if(mpr_verpvalue[i])
		{
			glbArrayClean(*mpr_verpvalue[i]);
			delete mpr_verpvalue[i];
		}
		mpr_verpvalue[i] = NULL;
	}
	mpr_verpvalue.clear();
	if(mpr_vergeometryclass.get() 
		&& mpr_vergeometryclass.get()->GetFieldCount()>0
		)
	{
		glbInt32 cnt = mpr_vergeometryclass.get()->GetFieldCount();
		mpr_verpvalue.reserve(cnt);
		for(glbInt32 i=0;i<cnt;i++)
		{
			CGlbField* fd = mpr_vergeometryclass.get()->GetField(i);
			glbArray*  value= new glbArray();
			value->vt = fd->GetType();
			value->dims=1;
			value->dimSize=0;
			mpr_verpvalue.push_back(value);
		}
	}
	mpr_extent = NULL;
	if(mpr_m) free(mpr_m);
	mpr_m = NULL;
}
void CGlbTin::EmptyTriangle()
{
	mpr_trianglecount   =0;
	if(mpr_trivertex)  free(mpr_trivertex);
	mpr_trivertex   = NULL;
	for (size_t i=0;i<mpr_tripvalue.size();i++)
	{
		if(mpr_tripvalue[i])
		{
			glbArrayClean(*mpr_tripvalue[i]);
			delete mpr_tripvalue[i];
		}
		mpr_tripvalue[i] = NULL;
	}
	if(mpr_trigeometryclass.get() 
		&& mpr_trigeometryclass.get()->GetFieldCount()>0
		)
	{
		glbInt32 cnt = mpr_trigeometryclass.get()->GetFieldCount();
		mpr_tripvalue.reserve(cnt);
		for(glbInt32 i=0;i<cnt;i++)
		{
			CGlbField* fd = mpr_trigeometryclass.get()->GetField(i);
			glbArray*  value= new glbArray();
			value->vt = fd->GetType();
			value->dims=1;
			value->dimSize=0;
			mpr_tripvalue.push_back(value);
		}
	}
}
void CGlbTin::Empty()
{
	EmptyVertex();
	EmptyTriangle();
}
glbBool CGlbTin::SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls)
{
	if(ctype == GLB_GEOCLASS_POINT)
	{
		return SetVertexClass(gcls);
	}else{
		return SetTriangleClass(gcls);
	}
	return false;
}
const CGlbGeometryClass* CGlbTin::GetGeoClass(GlbGeometryClassEnum ctype)
{
	if(ctype == GLB_GEOCLASS_POINT)
	{
		return mpr_vergeometryclass.get();
	}else{
		return mpr_trigeometryclass.get();
	}
	return NULL;
}
glbInt32 CGlbTin::GetVertexCount()
{
	return mpr_vertexescount;
}

glbBool CGlbTin::GetVertex( glbInt32 idx,glbDouble* ptx,glbDouble* pty,glbDouble* ptz )
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(idx<0||idx>=mpr_vertexescount||!ptx||!pty||!ptz)
	{
		GlbSetLastError(L"参数无效");
		return false;
	}
	*ptx=mpr_vertexcoord[idx*3];
	*pty=mpr_vertexcoord[idx*3+1];
	*ptz=mpr_vertexcoord[idx*3+2];
	return true;
}

glbBool CGlbTin::AddVertex( glbDouble x,glbDouble y,glbDouble z )
{
	glbDouble coords[3] = {0.0};
	coords[0] = x;
	coords[1] = y;
	coords[2] = z;
	return AddVertexes(coords,1);
}

glbBool CGlbTin::AddVertexes(glbDouble* coords,glbInt32 count)
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

	
	//分配新内存
	newCoords = (glbDouble*)malloc((mpr_vertexescount + count)
		                            *3
									*sizeof(glbDouble));
	if(!newCoords)goto newMallocError;
	
	if(mpr_vergeometryclass.get() 
		&& mpr_vergeometryclass.get()->GetFieldCount()>0)
	{
		glbInt32 fcount = mpr_vergeometryclass.get()->GetFieldCount();
		newValues.reserve(fcount);
		for(glbInt32 i=0;i<fcount;i++)newValues.push_back(NULL);
		for(glbInt32 i=0;i<fcount;i++)
		{
			if(i >= (glbInt32)mpr_verpvalue.size())
			{
				GlbSetLastError(L"字段数目有变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			if(mpr_verpvalue[i]->dimSize == 0)continue;

			CGlbField*      fld   = mpr_vergeometryclass.get()->GetField(i);
			GlbDataTypeEnum ftype = fld->GetType();
			if(ftype != mpr_verpvalue[i]->vt)
			{
				GlbSetLastError(L"字段类型变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			size_t          ftypesize = GetDataTypeSize(ftype);
			void* nvalue    = malloc(ftypesize*(mpr_vertexescount + count));
			if(!nvalue)goto newMallocError;
			newValues.push_back(nvalue);
		}		
	}
	//3.度量
	if(mpr_hasM)
	{
		newM = (glbDouble*)malloc((mpr_vertexescount + count)*sizeof(glbDouble));
		if(NULL == newM)goto newMallocError;
	}
	//////////////////内存分配正确////////////////
	//1.坐标赋值
	memcpy(newCoords + mpr_vertexescount * 3,
		   coords,
		   count*sizeof(glbDouble) * 3);
	if(mpr_vertexcoord)
	{
		memcpy(newCoords,
			   mpr_vertexcoord,
			   mpr_vertexescount*3*sizeof(glbDouble));
		//释放旧地址
		free(mpr_vertexcoord);
	}
	mpr_vertexcoord=newCoords;	
	//2.属性值赋值
	size_t fcount = newValues.size();
	for(size_t i=0;i<fcount;i++)
	{
		if(newValues[i]==NULL)continue;
		CGlbField*      fld       = mpr_vergeometryclass.get()->GetField(i);
		GlbDataTypeEnum ftype     = fld->GetType();
		//缺省值
		SetGeoValue(newValues[i],mpr_vertexescount,count,fld->GetDefault(),ftype);

		void** pValue = NULL;
		switch(ftype)
		{
		case GLB_DATATYPE_BYTE:    pValue = (void**)&mpr_verpvalue[i]->pBytes;break;
		//case GLB_DATATYPE_CHAR:    pValue = (void**)&mpr_verpvalue[i]->pChars;break;
		case GLB_DATATYPE_BOOLEAN: pValue = (void**)&mpr_verpvalue[i]->pBools;break;
		case GLB_DATATYPE_INT16:   pValue = (void**)&mpr_verpvalue[i]->pInts;break;
		case GLB_DATATYPE_INT32:   pValue = (void**)&mpr_verpvalue[i]->pLongs;break;
		case GLB_DATATYPE_INT64:   pValue = (void**)&mpr_verpvalue[i]->pLLongs;break;
		case GLB_DATATYPE_FLOAT:   pValue = (void**)&mpr_verpvalue[i]->pFloats;break;
		case GLB_DATATYPE_DOUBLE:  pValue = (void**)&mpr_verpvalue[i]->pDoubles;break;
		case GLB_DATATYPE_DATETIME:pValue = (void**)&mpr_verpvalue[i]->pDates;break;
		case GLB_DATATYPE_VECTOR:  pValue = (void**)&mpr_verpvalue[i]->pVectors;break;
		//case GLB_DATATYPE_STRING:
		//case GLB_DATATYPE_GEOMETRY:
		//case GLB_DATATYPE_BLOB:
		}
		if(*pValue)
		{
			CopyInsertGeoValue(newValues[i],*pValue,mpr_vertexescount,mpr_vertexescount,count,ftype);			
			//释放旧地址
			glbArrayClean(*mpr_verpvalue[i]);
		}
		mpr_verpvalue[i]->vt = ftype;
		mpr_verpvalue[i]->dims=1;
		mpr_verpvalue[i]->dimSize=mpr_vertexescount + count;
		*pValue = newValues[i];
	}
	//3.度量值
	if(mpr_hasM)
	{
		memset(newM + mpr_vertexescount,0,sizeof(glbDouble)*count);
		if(mpr_m)
		{
			memcpy(newM,
				   mpr_m,
				   mpr_vertexescount*sizeof(glbDouble));
			//释放旧地址
			free(mpr_m);
		}
		mpr_m = newM;
	}
	//修改基数
	mpr_vertexescount += count;
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
			x = coords[i*3];
			y = coords[i*3+1];
			z = coords[i*3+2];
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
	GlbSetLastError(L"内存不足");
	return false;
}

glbBool CGlbTin::SetVertex( glbInt32 idx,glbDouble x,glbDouble y, glbDouble z )
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if (idx<0||idx>=mpr_vertexescount)
	{
		GlbSetLastError(L"idx参数无效");
		return false;
	}
	mpr_vertexcoord[idx*3]=x;
	mpr_vertexcoord[idx*3+1]=y;
	mpr_vertexcoord[idx*3+2]=z;

	return true;
}

glbBool CGlbTin::DeleteVertex( glbInt32 idx )
{
	return DeleteVertexes(idx,1);
}

glbBool CGlbTin::DeleteVertexes(glbInt32 idx,glbInt32 count)
{
	if(count <=0)return true;

	if (IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if (idx < 0 || idx >= mpr_vertexescount)
	{
		GlbSetLastError(L"索引必须>=0且小于点数目");
		return false;
	}
	if(idx+count>= mpr_vertexescount)
		count = mpr_vertexescount - idx;
	if(mpr_vertexescount == count)
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
		                           *(mpr_vertexescount - count)
								   *3);
	if(NULL == newCoords) goto newMallocError;
	//2.属性
	if(mpr_vergeometryclass.get() 
		&& mpr_vergeometryclass.get()->GetFieldCount()>0)
	{
		glbInt32 fcount = mpr_vergeometryclass.get()->GetFieldCount();
		newValues.reserve(fcount);
		for(glbInt32 i=0;i<fcount;i++)newValues.push_back(NULL);
		for(glbInt32 i=0;i<fcount;i++)
		{
			if(i >= (glbInt32)mpr_verpvalue.size())
			{
				GlbSetLastError(L"字段数目有变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			if(mpr_verpvalue[i]->dimSize == 0)continue;

			CGlbField*      fld   = mpr_vergeometryclass.get()->GetField(i);
			GlbDataTypeEnum ftype = fld->GetType();
			if(ftype != mpr_verpvalue[i]->vt)
			{
				GlbSetLastError(L"字段类型变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			size_t          ftypesize = GetDataTypeSize(ftype);
			void* nvalue = malloc(ftypesize*(mpr_vertexescount-count));
			if(!nvalue)goto newMallocError;
			newValues.push_back(nvalue);
		}		
	}
	//3.度量值
	if(mpr_hasM)
	{
		newM = (glbDouble*)malloc((mpr_vertexescount - count)*sizeof(glbDouble));
		if(NULL == newM)goto newMallocError;
	}
	//////内存分配正确
	//1.拷贝坐标串
	memcpy(newCoords,
		   mpr_vertexcoord,
		   idx*3*sizeof(glbDouble));
	memcpy(newCoords          + idx*3,
		   mpr_vertexcoord    + (idx + count)*3,
		   (mpr_vertexescount - idx - count)*3*sizeof(glbDouble));	
	free(mpr_vertexcoord);
	mpr_vertexcoord = newCoords;
	//2.拷贝属性值
	size_t fcount = mpr_verpvalue.size();
	for(size_t i=0;i<fcount;i++)
	{
		if(newValues[i] == NULL)continue;
		CGlbField* fld = mpr_vergeometryclass->GetField(i);
		GlbDataTypeEnum ftype = fld->GetType();
		void** pValue = NULL;
		switch(ftype)
		{
		case GLB_DATATYPE_BYTE:    pValue = (void**)&mpr_verpvalue[i]->pBytes;break;
		//case GLB_DATATYPE_CHAR:    pValue = (void**)&mpr_verpvalue[i]->pChars;break;
		case GLB_DATATYPE_BOOLEAN: pValue = (void**)&mpr_verpvalue[i]->pBools;break;
		case GLB_DATATYPE_INT16:   pValue = (void**)&mpr_verpvalue[i]->pInts;break;
		case GLB_DATATYPE_INT32:   pValue = (void**)&mpr_verpvalue[i]->pLongs;break;
		case GLB_DATATYPE_INT64:   pValue = (void**)&mpr_verpvalue[i]->pLLongs;break;
		case GLB_DATATYPE_FLOAT:   pValue = (void**)&mpr_verpvalue[i]->pFloats;break;
		case GLB_DATATYPE_DOUBLE:  pValue = (void**)&mpr_verpvalue[i]->pDoubles;break;
		case GLB_DATATYPE_DATETIME:pValue = (void**)&mpr_verpvalue[i]->pDates;break;
		case GLB_DATATYPE_VECTOR:  pValue = (void**)&mpr_verpvalue[i]->pVectors;break;
		//case GLB_DATATYPE_STRING:
		//case GLB_DATATYPE_GEOMETRY:
		//case GLB_DATATYPE_BLOB:
		}
		CopyRemoveGeoValue(newValues[i],*pValue,mpr_vertexescount,idx,count,ftype);
		glbArrayClean(*mpr_verpvalue[i]);
		mpr_verpvalue[i]->vt = ftype;
		mpr_verpvalue[i]->dims=1;
		mpr_verpvalue[i]->dimSize = mpr_vertexescount - count;
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
			   (mpr_vertexescount-idx - count)*sizeof(glbDouble));		
		free(mpr_m);
		mpr_m = newM;
	}
	//修改基数
	mpr_vertexescount -= count;
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
	GlbSetLastError(L"内存不足");
	return false;
}

const glbDouble* CGlbTin::GetVertexes()
{
	return mpr_vertexcoord;
}

glbBool CGlbTin::GetM(glbInt32 idx,double* m)
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
	if(idx<0|| idx>=mpr_vertexescount)
	{
		GlbSetLastError(L"索引必须>=0且小于点数目");
		return false;
	}
	*m = mpr_m[idx];
	return true;
}

glbBool CGlbTin::SetM(glbInt32 idx,double m)
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
	if(idx<0|| idx>=mpr_vertexescount)
	{
		GlbSetLastError(L"索引必须>=0且小于点数目");
		return false;
	}
	mpr_m[idx] = m;
	return true;
}

const glbDouble* CGlbTin::GetMs()
{
	return mpr_m;
}

glbInt32 CGlbTin::GetTriangleCount()
{
	return mpr_trianglecount;
}

glbBool CGlbTin::GetTriangle( glbInt32 idx,glbInt32* first_ptidx,glbInt32* second_ptidx,glbInt32* third_ptidx)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(!mpr_trivertex)
	{
		GlbSetLastError(L"没有三角形");
		return false;
	}
	if(idx<0||idx>=mpr_trianglecount||!first_ptidx||!second_ptidx||!third_ptidx)
	{
		GlbSetLastError(L"参数无效");
		return false;
	}

	*first_ptidx = mpr_trivertex[3*idx];
	*second_ptidx= mpr_trivertex[3*idx+1];
	*third_ptidx = mpr_trivertex[3*idx+2];
	return true;
}

glbBool CGlbTin::AddTriangle( glbInt32 first_ptidx,glbInt32 second_ptidx,glbInt32 third_ptidx)
{
	glbInt32 ptidxes[3] = {0};
	ptidxes[0] = first_ptidx;
	ptidxes[1] = second_ptidx;
	ptidxes[2] = third_ptidx;

	return AddTriangles(ptidxes,1);
}

glbBool	CGlbTin::AddTriangles(glbInt32* ptidxes,glbInt32 count)
{
	if(count <=0) return true;
	if(ptidxes == NULL)
	{
		GlbSetLastError(L"ptidxes 是NULL");
		return false;
	}
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	
	//分配内存
	glbInt32*     newTriIdxes = NULL;	
	vector<void*> newValues;
	//1.三角形顶点
	newTriIdxes = (glbInt32*)malloc((mpr_trianglecount + count)
		                            *3
									*sizeof(glbInt32));
	if(!newTriIdxes)
	{
		GlbSetLastError(L"内存不足");
		goto newMallocError;
	}
	//2.三角形属性
	if(mpr_trigeometryclass.get() 
		&& mpr_trigeometryclass.get()->GetFieldCount()>0)
	{
		glbInt32 fcount = mpr_trigeometryclass.get()->GetFieldCount();
		newValues.reserve(fcount);
		for(glbInt32 i=0;i<fcount;i++)newValues.push_back(NULL);
		for(glbInt32 i=0;i<fcount;i++)
		{			
			if(i >= (glbInt32)mpr_tripvalue.size())
			{
				GlbSetLastError(L"字段数目有变化,需要重新SetTriangleClass");
				goto newMallocError;
			}
			if(mpr_tripvalue[i]->dimSize == 0)continue;

			CGlbField*      fld   = mpr_trigeometryclass.get()->GetField(i);
			GlbDataTypeEnum ftype = fld->GetType();
			if(ftype != mpr_tripvalue[i]->vt)
			{
				GlbSetLastError(L"字段类型变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			size_t ftypesize = GetDataTypeSize(ftype);
			void* nvalue = malloc(ftypesize*(mpr_trianglecount + count));
			if(!nvalue)
			{
				GlbSetLastError(L"内存不足");
				goto newMallocError;
			}
			newValues.push_back(nvalue);
		}		
	}
	//////////////////内存分配正确////////////////
	memcpy(newTriIdxes + mpr_trianglecount*3,
		   ptidxes,
		   count*sizeof(glbInt32)*3);
	if(mpr_trivertex)
	{
		memcpy(newTriIdxes,
			   mpr_trivertex,
			   mpr_trianglecount*3*sizeof(glbInt32));
		free(mpr_trivertex);
	}
	mpr_trivertex = newTriIdxes;
	
	//追加三角形的默认属性
	size_t fcount = newValues.size();
	for(size_t i=0;i<fcount;i++)
	{
		if(newValues[i]==NULL)continue;
		CGlbField* fld=mpr_trigeometryclass.get()->GetField(i);
		GlbDataTypeEnum ftype = fld->GetType();
		size_t ftypesize = GetDataTypeSize(ftype);
		//缺省值
		SetGeoValue(newValues[i],mpr_trianglecount,count,fld->GetDefault(),ftype);
			
		void** pValue = NULL;
		switch(ftype)
		{
		case GLB_DATATYPE_BYTE:    pValue = (void**)&mpr_tripvalue[i]->pBytes;break;
		//case GLB_DATATYPE_CHAR:    pValue = (void**)&mpr_tripvalue[i]->pChars;break;
		case GLB_DATATYPE_BOOLEAN: pValue = (void**)&mpr_tripvalue[i]->pBools;break;
		case GLB_DATATYPE_INT16:   pValue = (void**)&mpr_tripvalue[i]->pInts;break;
		case GLB_DATATYPE_INT32:   pValue = (void**)&mpr_tripvalue[i]->pLongs;break;
		case GLB_DATATYPE_INT64:   pValue = (void**)&mpr_tripvalue[i]->pLLongs;break;
		case GLB_DATATYPE_FLOAT:   pValue = (void**)&mpr_tripvalue[i]->pFloats;break;
		case GLB_DATATYPE_DOUBLE:  pValue = (void**)&mpr_tripvalue[i]->pDoubles;break;
		case GLB_DATATYPE_DATETIME:pValue = (void**)&mpr_tripvalue[i]->pDates;break;
		case GLB_DATATYPE_VECTOR:  pValue = (void**)&mpr_tripvalue[i]->pVectors;break;
		//case GLB_DATATYPE_STRING:
		//case GLB_DATATYPE_GEOMETRY:
		//case GLB_DATATYPE_BLOB:
		}
		if(*pValue)
		{
			CopyInsertGeoValue(newValues[i],*pValue,mpr_trianglecount,mpr_trianglecount,count,ftype);
			//释放旧地址
			glbArrayClean(*mpr_tripvalue[i]);
		}
		mpr_tripvalue[i]->vt = ftype;
		mpr_tripvalue[i]->dims=1;
		mpr_tripvalue[i]->dimSize = mpr_trianglecount + count;
		*pValue = newValues[i];
	}

	mpr_trianglecount += count;
	return true;

newMallocError://分配新的内存出错
	if(newTriIdxes) free(newTriIdxes);	
	vector<void*>::iterator itr    = newValues.begin();
	vector<void*>::iterator itrEnd = newValues.end();
	for(itr;itr!=itrEnd;itr++)
	{
		if((*itr))free((*itr));
	}	
	return false;
}

glbBool CGlbTin::SetTriangle( glbInt32 idx,glbInt32 first_ptidx,glbInt32 second_ptidx,glbInt32 third_ptidx )
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(idx<0||idx>=mpr_trianglecount)
	{
		GlbSetLastError(L"参数idx必须>=0且小于三角形数目");
		return false;
	}

	mpr_trivertex[idx*3]=first_ptidx;
	mpr_trivertex[idx*3+1]=second_ptidx;
	mpr_trivertex[idx*3+2]=third_ptidx;

	return true;
}

glbBool CGlbTin::DeleteTriangle( glbInt32 idx )
{
	return DeleteTriangles(idx,1);
}

glbBool	CGlbTin::DeleteTriangles(glbInt32 idx,glbInt32 count)
{
	if(count<=0)return true;
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(idx<0||idx>=mpr_trianglecount)
	{
		GlbSetLastError(L"idx必须>=0且小于三角形数目");
		return false;
	}
	if(idx+count>= mpr_trianglecount)
		count = mpr_trianglecount - idx;
	if(mpr_trianglecount == count)
	{
		EmptyTriangle();
		return true;
	}
	glbInt32*     new_trivertex = NULL;
	vector<void*> newValues;
	//1.三角形
	new_trivertex = (glbInt32*)malloc(sizeof(glbInt32)
		                              *(mpr_trianglecount - count)
									  *3);
	if(!new_trivertex)
	{
		GlbSetLastError(L"内存不足");
		goto newMallocError;
	}
	//2.属性
	if(mpr_trigeometryclass.get()
		&& mpr_trigeometryclass.get()->GetFieldCount()>0)
	{
		glbInt32 fcount = mpr_trigeometryclass.get()->GetFieldCount();
		newValues.reserve(fcount);
		for(glbInt32 i=0;i<fcount;i++)newValues.push_back(NULL);
		for(glbInt32 i=0;i<fcount;i++)
		{			
			if(i >= (glbInt32)mpr_tripvalue.size())
			{
				GlbSetLastError(L"字段数目有变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			if(mpr_tripvalue[i]->dimSize == 0)continue;

			CGlbField*      fld   =mpr_trigeometryclass.get()->GetField(i);
			GlbDataTypeEnum ftype = fld->GetType();
			if(ftype != mpr_tripvalue[i]->vt)
			{
				GlbSetLastError(L"字段类型变化,需要重新SetVertexClass");
				goto newMallocError;
			}
			size_t ftypesize = GetDataTypeSize(ftype);
			void* nvalue = malloc(ftypesize*(mpr_trianglecount - count));
			if(!nvalue)
			{
				GlbSetLastError(L"内存不足");
				goto newMallocError;
			}
			newValues.push_back(nvalue);
		}		
	}
	//////////////////内存分配正确////////////////
	// 1.拷贝三角形索引
	memcpy(new_trivertex,
		   mpr_trivertex,
		   idx*3*sizeof(glbInt32));
	memcpy(new_trivertex     + idx*3,
		   mpr_trivertex     + (idx+count)*3,
		  (mpr_trianglecount - idx - count)*sizeof(glbInt32)*3);
	free(mpr_trivertex);
	mpr_trivertex = new_trivertex;
	//2.拷贝属性值
	size_t fcount = mpr_tripvalue.size();
	for(size_t i=0;i<fcount;i++)
	{
		if(newValues[i]==NULL)continue;
		CGlbField* fld = mpr_trigeometryclass->GetField(i);
		GlbDataTypeEnum ftype = fld->GetType();
		void** pValue = NULL;
		switch(ftype)
		{
		case GLB_DATATYPE_BYTE:    pValue = (void**)&mpr_tripvalue[i]->pBytes;break;
		//case GLB_DATATYPE_CHAR:    pValue = (void**)&mpr_tripvalue[i]->pChars;break;
		case GLB_DATATYPE_BOOLEAN: pValue = (void**)&mpr_tripvalue[i]->pBools;break;
		case GLB_DATATYPE_INT16:   pValue = (void**)&mpr_tripvalue[i]->pInts;break;
		case GLB_DATATYPE_INT32:   pValue = (void**)&mpr_tripvalue[i]->pLongs;break;
		case GLB_DATATYPE_INT64:   pValue = (void**)&mpr_tripvalue[i]->pLLongs;break;
		case GLB_DATATYPE_FLOAT:   pValue = (void**)&mpr_tripvalue[i]->pFloats;break;
		case GLB_DATATYPE_DOUBLE:  pValue = (void**)&mpr_tripvalue[i]->pDoubles;break;
		case GLB_DATATYPE_DATETIME:pValue = (void**)&mpr_tripvalue[i]->pDates;break;
		case GLB_DATATYPE_VECTOR:  pValue = (void**)&mpr_tripvalue[i]->pVectors;break;
		//case GLB_DATATYPE_STRING:
		//case GLB_DATATYPE_GEOMETRY:
		//case GLB_DATATYPE_BLOB:
		}
		CopyRemoveGeoValue(newValues[i],*pValue,mpr_trianglecount,idx,count,ftype);
		glbArrayClean(*mpr_tripvalue[i]);
		mpr_tripvalue[i]->vt = ftype;
		mpr_tripvalue[i]->dims=1;
		mpr_tripvalue[i]->dimSize= mpr_trianglecount - count;
		*pValue = newValues[i];
	}

	mpr_trianglecount -= count;
	return true;

newMallocError://分配新的内存出错
	if(new_trivertex)free(new_trivertex);	
	vector<void*>::iterator itr    = newValues.begin();
	vector<void*>::iterator itrEnd = newValues.end();
	for(itr;itr!=itrEnd;itr++)
	{
		if((*itr))free((*itr));
	}
	return false;
}

const glbInt32*	CGlbTin::GetTriangles()
{
	return mpr_trivertex;
}

glbBool CGlbTin::GetVertexValue(glbInt32 fidx, glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(pidx<0||pidx>=mpr_vertexescount)
	{
		GlbSetLastError(L"pidx必须>=0且小于顶点数目");
		return false;
	}
	if(!mpr_vergeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	
	if(fidx<0||fidx>=mpr_vergeometryclass.get()->GetFieldCount())
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");
		return false;
	}
	if(fidx >= (glbInt32)mpr_verpvalue.size())
	{
		GlbSetLastError(L"字段数目变化,需要重新SetPointClass");
		return false;
	}
	CGlbField* fld= mpr_vergeometryclass.get()->GetField(fidx);
	GlbDataTypeEnum  ftype = fld->GetType();
	if(mpr_verpvalue[fidx]->vt != ftype)
	{
		GlbSetLastError(L"字段类型变化,需要重新SetPointClass");
		return false;
	}
	if(mpr_verpvalue[fidx]->dimSize == 0)
	{//取缺省值
		GetGeoValue(pvalue,(void*)fld->GetDefault(),0,ftype);
	}
	else
	{
		void* pValue = NULL;
		switch(ftype)
		{
		case GLB_DATATYPE_BYTE:    pValue = mpr_verpvalue[fidx]->pBytes;break;
		//case GLB_DATATYPE_CHAR:    pValue = mpr_verpvalue[fidx]->pChars;break;
		case GLB_DATATYPE_BOOLEAN: pValue = mpr_verpvalue[fidx]->pBools;break;
		case GLB_DATATYPE_INT16:   pValue = mpr_verpvalue[fidx]->pInts;break;
		case GLB_DATATYPE_INT32:   pValue = mpr_verpvalue[fidx]->pLongs;break;
		case GLB_DATATYPE_INT64:   pValue = mpr_verpvalue[fidx]->pLLongs;break;
		case GLB_DATATYPE_FLOAT:   pValue = mpr_verpvalue[fidx]->pFloats;break;
		case GLB_DATATYPE_DOUBLE:  pValue = mpr_verpvalue[fidx]->pDoubles;break;
		case GLB_DATATYPE_DATETIME:pValue = mpr_verpvalue[fidx]->pDates;break;
		case GLB_DATATYPE_VECTOR:  pValue = mpr_verpvalue[fidx]->pVectors;break;
		//case GLB_DATATYPE_STRING:
		//case GLB_DATATYPE_GEOMETRY:
		//case GLB_DATATYPE_BLOB:
		}
		GetGeoValue(pvalue,pValue,pidx,ftype);
	}
	return true;
}

glbBool CGlbTin::GetVertexValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(!mpr_vergeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}

	glbInt32 fidx = mpr_vergeometryclass.get()->FindFieldIndex(fname);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}
	
	return GetVertexValue(fidx,pidx,pvalue);
}

glbBool CGlbTin::SetVertexValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(!mpr_vergeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(pidx<0||pidx>=mpr_vertexescount)
	{
		GlbSetLastError(L"pidx必须>=0且小于顶点数目");
		return false;
	}
	
	if(fidx<0||fidx>=mpr_vergeometryclass.get()->GetFieldCount())
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");		
		return false;
	}
	if(fidx >= (glbInt32)mpr_verpvalue.size())
	{
		GlbSetLastError(L"字段数目变化,需要重新SetPointClass");
		return false;
	}
	CGlbField* fld        = mpr_vergeometryclass.get()->GetField(fidx);
	GlbDataTypeEnum ftype = fld->GetType();
	if(mpr_verpvalue[fidx]->vt != ftype)
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
	case GLB_DATATYPE_BYTE:   pValue = (void**)&mpr_verpvalue[fidx]->pBytes;break;
	//case GLB_DATATYPE_CHAR:   pValue = (void**)&mpr_verpvalue[fidx]->pChars;break;
	case GLB_DATATYPE_BOOLEAN:pValue = (void**)&mpr_verpvalue[fidx]->pBools;break;
	case GLB_DATATYPE_INT16:  pValue = (void**)&mpr_verpvalue[fidx]->pInts;break;
	case GLB_DATATYPE_INT32:  pValue = (void**)&mpr_verpvalue[fidx]->pLongs;break;
	case GLB_DATATYPE_INT64:  pValue = (void**)&mpr_verpvalue[fidx]->pLLongs;break;
	case GLB_DATATYPE_FLOAT:  pValue = (void**)&mpr_verpvalue[fidx]->pFloats;break;
	case GLB_DATATYPE_DOUBLE: pValue = (void**)&mpr_verpvalue[fidx]->pDoubles;break;
	case GLB_DATATYPE_DATETIME:pValue = (void**)&mpr_verpvalue[fidx]->pDates;break;
	case GLB_DATATYPE_VECTOR: pValue = (void**)&mpr_verpvalue[fidx]->pVectors;break;
	//case GLB_DATATYPE_STRING:
	//case GLB_DATATYPE_GEOMETRY:
	//case GLB_DATATYPE_BLOB:
	}

	if(*pValue == NULL || mpr_verpvalue[fidx]->dimSize == 0)
	{//初始化缺省值
		size_t ftypesize = GetDataTypeSize(ftype);
		*pValue = malloc(mpr_vertexescount*ftypesize);
		if(*pValue == NULL)
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
		
		SetGeoValue(*pValue,0,mpr_vertexescount,fld->GetDefault(),ftype);		
	}
	SetGeoValueVariant(*pValue,pidx,1,pvalue,ftype);
	return true;
}

glbBool CGlbTin::SetVertexValueByName(glbWChar* name ,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(!name)
	{
		GlbSetLastError(L"name是NULL");
		return false;
	}
	if(!mpr_vergeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	
	glbInt32 fidx = mpr_vergeometryclass.get()->FindFieldIndex(name);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}

	SetVertexValue(fidx,pidx,pvalue);
	return true;
}

const glbArray* CGlbTin::GetVertexesValue( glbInt32 fidx)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(!mpr_vergeometryclass)
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(fidx<0||fidx>mpr_vergeometryclass->GetFieldCount()-1)
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");
		return false;
	}
	if(fidx >= (glbInt32)mpr_verpvalue.size())
	{
		GlbSetLastError(L"字段数目有变化,需要重新SetPointClass");
		return NULL;
	}
	CGlbField* fld= mpr_vergeometryclass.get()->GetField(fidx);
	if(fld->GetType() != mpr_verpvalue[fidx]->vt)
	{
		GlbSetLastError(L"字段类型有变后,需要重新SetPointClass");
		return NULL;
	}
	return mpr_verpvalue[fidx];
}

const glbArray* CGlbTin::GetVertexesValueByName( glbWChar* name)
{	
	if(!name)
	{
		GlbSetLastError(L"name是NULL");
		return false;
	}
	if(!mpr_vergeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	
	glbInt32 fidx = mpr_vergeometryclass.get()->FindFieldIndex(name);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}
	return GetVertexesValue(fidx);
}

glbBool CGlbTin::SetVertexesValue( glbInt32 fidx,glbArray& pvalues )
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(!mpr_vergeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(fidx<0||fidx>=mpr_vergeometryclass.get()->GetFieldCount())
	{
		GlbSetLastError(L"字段索引必须>=0且小于字段数目");
		return false;
	}
    if(fidx >= (glbInt32)mpr_verpvalue.size())
	{
		GlbSetLastError(L"字段数目变化,需要重新SetPointClass");
		return false;
	}
    if(pvalues.dims!=1)
	{
		GlbSetLastError(L"pvalues.dims维度必须是1");
		return false;
	}
	if( !(  pvalues.dimSize == mpr_vertexescount		
		||  pvalues.dimSize ==  0
		))
	{
		GlbSetLastError(L"pvalues.dimSizes必须等于点数目或0");
		return false;
	}
	CGlbField* fld=mpr_vergeometryclass.get()->GetField(fidx);
	GlbDataTypeEnum ftype = fld->GetType();
	if (ftype != pvalues.vt)
	{
		GlbSetLastError(L"值类型与字段类型不相符");
		return false;
	}
	if(ftype != mpr_verpvalue[fidx]->vt)
	{
		GlbSetLastError(L"字段类型变化,需要重新SetPointClass");
		return NULL;
	}
	if(pvalues.dimSize == 0)
	{
		glbArrayClean(*mpr_verpvalue[fidx]);
		mpr_verpvalue[fidx]->vt     =ftype;
		mpr_verpvalue[fidx]->dims   =1;
		mpr_verpvalue[fidx]->dimSize=0;
		return true;
	}
	void** pValue   = NULL;
	void*  pSrcValue= NULL;
	switch(ftype)
	{
	case GLB_DATATYPE_BYTE:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pBytes;
			pSrcValue = pvalues.pBytes;
		}break;
	/*case GLB_DATATYPE_CHAR:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pChars;
			pSrcValue = pvalues.pChars;
		}break;*/
	case GLB_DATATYPE_BOOLEAN:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pBools;
			pSrcValue = pvalues.pBools;
		}break;
	case GLB_DATATYPE_INT16:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pInts;
			pSrcValue = pvalues.pInts;
		}break;
	case GLB_DATATYPE_INT32:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pLongs;
			pSrcValue = pvalues.pLongs;
		}break;
	case GLB_DATATYPE_INT64:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pLLongs;
			pSrcValue = pvalues.pLLongs;
		}break;
	case GLB_DATATYPE_FLOAT:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pFloats;
			pSrcValue = pvalues.pFloats;
		}break;
	case GLB_DATATYPE_DOUBLE:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pDoubles;
			pSrcValue = pvalues.pDoubles;
		}break;
	case GLB_DATATYPE_DATETIME:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pDates;
			pSrcValue = pvalues.pDates;
		}break;
	case GLB_DATATYPE_VECTOR:
		{
			pValue = (void**)&mpr_verpvalue[fidx]->pVectors;
			pSrcValue = pvalues.pVectors;
		}break;
	//case GLB_DATATYPE_STRING:
	//case GLB_DATATYPE_GEOMETRY:
	//case GLB_DATATYPE_BLOB:
	}
	size_t          ftypesize = GetDataTypeSize(ftype);
	if(*pValue == NULL || mpr_verpvalue[fidx]->dimSize == 0)
	{
		*pValue = malloc(mpr_vertexescount*ftypesize);
		if(*pValue == NULL)
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
		mpr_verpvalue[fidx]->dimSize = mpr_vertexescount;
	}
	memcpy(*pValue,pSrcValue,mpr_vertexescount*ftypesize);
	return true;
}

glbBool CGlbTin::SetVertexesValueByName( glbWChar* name ,glbArray& pvalues )
{
	if(!mpr_vergeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	glbInt32 fidx = mpr_vergeometryclass.get()->FindFieldIndex(name);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}
	
	return SetVertexesValue(fidx,pvalues);
}

glbBool CGlbTin::GetTriangleValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(pidx<0||pidx>=mpr_trianglecount)
	{
		GlbSetLastError(L"pidx必须>=0且小于三角形数目");
		return false;
	}
	if(!mpr_trigeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	
	if(fidx<0||fidx>=mpr_trigeometryclass.get()->GetFieldCount())
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");
		return false;
	}
	
	CGlbField* fld= mpr_trigeometryclass.get()->GetField(fidx);
	if(mpr_tripvalue[fidx]==NULL)
	{//取缺省值
		GetGeoValue(pvalue,(void*)fld->GetDefault(),0,fld->GetType());
	}
	else
	{
		GetGeoValue(pvalue,mpr_tripvalue[fidx],pidx,fld->GetType());
	}
	return true;
}

glbBool CGlbTin::GetTriangleValueByName(glbWChar* name,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(name == NULL)
	{
		GlbSetLastError(L"name是NULL");
		return false;
	}
	if(!mpr_trigeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	glbInt32 fidx = mpr_trigeometryclass.get()->FindFieldIndex(name);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}
	
	return GetTriangleValue(fidx,pidx,pvalue);
}

glbBool CGlbTin::SetTriangleValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(!mpr_trigeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(pidx<0||pidx>=mpr_trianglecount)
	{
		GlbSetLastError(L"pidx必须>=0且小于三角形数目");
		return false;
	}
	
	if(fidx<0||fidx>=mpr_trigeometryclass.get()->GetFieldCount())
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");
		return false;
	}
	if(fidx >= (glbInt32)mpr_tripvalue.size())
	{
		GlbSetLastError(L"字段数目变化,需要重新SetPointClass");
		return false;
	}
	CGlbField* fld= mpr_trigeometryclass.get()->GetField(fidx);
	GlbDataTypeEnum ftype = fld->GetType();
	if (mpr_tripvalue[fidx]->vt != ftype)
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
	case GLB_DATATYPE_BYTE:   pValue = (void**)&mpr_tripvalue[fidx]->pBytes;break;
	//case GLB_DATATYPE_CHAR:   pValue = (void**)&mpr_tripvalue[fidx]->pChars;break;
	case GLB_DATATYPE_BOOLEAN:pValue = (void**)&mpr_tripvalue[fidx]->pBools;break;
	case GLB_DATATYPE_INT16:  pValue = (void**)&mpr_tripvalue[fidx]->pInts;break;
	case GLB_DATATYPE_INT32:  pValue = (void**)&mpr_tripvalue[fidx]->pLongs;break;
	case GLB_DATATYPE_INT64:  pValue = (void**)&mpr_tripvalue[fidx]->pLLongs;break;
	case GLB_DATATYPE_FLOAT:  pValue = (void**)&mpr_tripvalue[fidx]->pFloats;break;
	case GLB_DATATYPE_DOUBLE: pValue = (void**)&mpr_tripvalue[fidx]->pDoubles;break;
	case GLB_DATATYPE_DATETIME:   pValue = (void**)&mpr_tripvalue[fidx]->pDates;break;
	case GLB_DATATYPE_VECTOR: pValue = (void**)&mpr_tripvalue[fidx]->pVectors;break;
	//case GLB_DATATYPE_STRING:
	//case GLB_DATATYPE_GEOMETRY:
	//case GLB_DATATYPE_BLOB:
	}
	if(*pValue == NULL || mpr_tripvalue[fidx]->dimSize == 0)
	{//初始化缺省值
		size_t ftypesize = GetDataTypeSize(ftype);
		*pValue = malloc(mpr_trianglecount*ftypesize);
		if(*pValue == NULL)
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
		mpr_tripvalue[fidx]->dimSize = mpr_trianglecount;
		SetGeoValue(*pValue,0,mpr_trianglecount,fld->GetDefault(),ftype);		
	}
	SetGeoValueVariant(mpr_tripvalue[fidx],pidx,1,pvalue,ftype);
	return true;
}

glbBool CGlbTin::SetTriangleValueByName(glbWChar* name,glbInt32 pidx,GLBVARIANT& pvalue)
{
	if(!mpr_trigeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	glbInt32 fidx = mpr_trigeometryclass.get()->FindFieldIndex(name);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}

	return SetTriangleValue(fidx,pidx,pvalue);
}

const glbArray* CGlbTin::GetTrianglesValue( glbInt32 fidx)
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(!mpr_trigeometryclass)
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(fidx<0||fidx>=mpr_trigeometryclass->GetFieldCount())
	{
		GlbSetLastError(L"fidx必须>=0且小于字段数目");
		return false;
	}
	if(fidx >= (glbInt32)mpr_tripvalue.size())
	{
		GlbSetLastError(L"字段数目有变化,需要重新SetPointClass");
		return NULL;
	}

	CGlbField* fld= mpr_trigeometryclass.get()->GetField(fidx);
	if(fld->GetType() != mpr_tripvalue[fidx]->vt)
	{
		GlbSetLastError(L"字段类型有变后,需要重新SetPointClass");
		return NULL;
	}
	return mpr_tripvalue[fidx];
}

const glbArray* CGlbTin::GetTrianglesValueByName( glbWChar* name)
{
	if(!name)
	{
		GlbSetLastError(L"name是NULL");
		return false;
	}
	if(!mpr_trigeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	
	glbInt32 fidx = mpr_trigeometryclass.get()->FindFieldIndex(name);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}

	return GetTrianglesValue(fidx);
}
glbBool CGlbTin::SetTrianglesValue( glbInt32 fidx,glbArray& pvalues )
{
	if(IsEmpty())
	{
		GlbSetLastError(L"没有顶点");
		return false;
	}
	if(!mpr_trigeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	if(fidx<0||fidx>=mpr_trigeometryclass.get()->GetFieldCount())
	{
		GlbSetLastError(L"字段索引必须>=0且小于字段数目");
		return false;
	}
    if(fidx >= (glbInt32)mpr_tripvalue.size())
	{
		GlbSetLastError(L"字段数目变化,需要重新SetPointClass");
		return false;
	}
    if(pvalues.dims!=1)
	{
		GlbSetLastError(L"pvalues.dims维度必须是1");
		return false;
	}
	if( !(  pvalues.dimSize == mpr_trianglecount		
		||  pvalues.dimSize ==  0
		))
	{
		GlbSetLastError(L"pvalues.dimSizes必须等于点数目或0");
		return false;
	}
	CGlbField* fld=mpr_trigeometryclass.get()->GetField(fidx);
	GlbDataTypeEnum ftype = fld->GetType();
	if (ftype != pvalues.vt)
	{
		GlbSetLastError(L"值类型与字段类型不相符");
		return false;
	}
	if(ftype != mpr_tripvalue[fidx]->vt)
	{
		GlbSetLastError(L"字段类型变化,需要重新SetPointClass");
		return NULL;
	}
	if(pvalues.dimSize == 0)
	{
		glbArrayClean(*mpr_tripvalue[fidx]);
		mpr_tripvalue[fidx]->vt     =ftype;
		mpr_tripvalue[fidx]->dims   =1;
		mpr_tripvalue[fidx]->dimSize=0;
		return true;
	}
	void** pValue   = NULL;
	void*  pSrcValue= NULL;
	switch(ftype)
	{
	case GLB_DATATYPE_BYTE:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pBytes;
			pSrcValue = pvalues.pBytes;
		}break;
	/*case GLB_DATATYPE_CHAR:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pChars;
			pSrcValue = pvalues.pChars;
		}break;*/
	case GLB_DATATYPE_BOOLEAN:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pBools;
			pSrcValue = pvalues.pBools;
		}break;
	case GLB_DATATYPE_INT16:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pInts;
			pSrcValue = pvalues.pInts;
		}break;
	case GLB_DATATYPE_INT32:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pLongs;
			pSrcValue = pvalues.pLongs;
		}break;
	case GLB_DATATYPE_INT64:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pLLongs;
			pSrcValue = pvalues.pLLongs;
		}break;
	case GLB_DATATYPE_FLOAT:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pFloats;
			pSrcValue = pvalues.pFloats;
		}break;
	case GLB_DATATYPE_DOUBLE:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pDoubles;
			pSrcValue = pvalues.pDoubles;
		}break;
	case GLB_DATATYPE_DATETIME:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pDates;
			pSrcValue = pvalues.pDates;
		}break;
	case GLB_DATATYPE_VECTOR:
		{
			pValue = (void**)&mpr_tripvalue[fidx]->pVectors;
			pSrcValue = pvalues.pVectors;
		}break;
	//case GLB_DATATYPE_STRING:
	//case GLB_DATATYPE_GEOMETRY:
	//case GLB_DATATYPE_BLOB:
	}
	size_t          ftypesize = GetDataTypeSize(ftype);
	if(*pValue == NULL || mpr_tripvalue[fidx]->dimSize==0)
	{
		*pValue = malloc(mpr_trianglecount*ftypesize);
		if(*pValue == NULL)
		{
			GlbSetLastError(L"内存不足");
			return false;
		}
		mpr_tripvalue[fidx]->dimSize = mpr_trianglecount;
	}
	memcpy(*pValue,pSrcValue,mpr_trianglecount*ftypesize);
	return true;
}

glbBool CGlbTin::SetTrianglesValueByName( glbWChar* name, glbArray& pvalues )
{
	if(!name)
	{
		GlbSetLastError(L"name是NULL");
		return false;
	}
	if(!mpr_trigeometryclass.get())
	{
		GlbSetLastError(L"没有字段");
		return false;
	}
	glbInt32 fidx = mpr_trigeometryclass.get()->FindFieldIndex(name);
	if(fidx<0)
	{
		GlbSetLastError(L"字段不存在");
		return false;
	}
	
	return SetTrianglesValue(fidx,pvalues);
}
glbBool CGlbTin::SetVertexClass( CGlbGeometryClass* gls )
{
	mpr_vergeometryclass = gls;
	size_t            fcount    =  0;
	if(mpr_vergeometryclass.get())fcount = mpr_vergeometryclass->GetFieldCount();
	vector<glbArray*> newValues;
	newValues.reserve(fcount);
	for(size_t i=0;i<fcount;i++)newValues.push_back(NULL);

	glbArray*  value      = NULL;
	for(size_t i=0;i<fcount;i++)
	{		
		CGlbField* fld        = mpr_vergeometryclass.get()->GetField(i);
		GlbDataTypeEnum ftype = fld->GetType();
		if(i<mpr_verpvalue.size())
		{
			value = mpr_verpvalue[i];
			if(value->vt == ftype)
			{
				newValues[i]       = value;
				mpr_verpvalue[i] = NULL;
			}else{
				glbArrayClean(*value);
			    delete value;
			    mpr_verpvalue[i] = NULL;
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
	fcount = mpr_verpvalue.size();
	for(size_t i=0;i<fcount;i++)
	{
		value = mpr_verpvalue[i];
		if(value)
		{
			glbArrayClean(*value);
			delete value;
			mpr_verpvalue[i] = NULL;
		}
	}
	mpr_verpvalue.clear();
	mpr_verpvalue = newValues;
	return true;
}
glbBool CGlbTin::SetTriangleClass( CGlbGeometryClass* gls )
{
	mpr_trigeometryclass = gls;
	size_t            fcount    =  0;
	if(mpr_trigeometryclass.get())fcount = mpr_trigeometryclass->GetFieldCount();
	vector<glbArray*> newValues;
	newValues.reserve(fcount);
	for(size_t i=0;i<fcount;i++)newValues.push_back(NULL);

	glbArray*  value      = NULL;
	for(size_t i=0;i<fcount;i++)
	{		
		CGlbField* fld        = mpr_trigeometryclass.get()->GetField(i);
		GlbDataTypeEnum ftype = fld->GetType();
		if(i<mpr_tripvalue.size())
		{
			value = mpr_tripvalue[i];
			if(value->vt == ftype)
			{
				newValues[i]       = value;
				mpr_tripvalue[i] = NULL;
			}else{
				glbArrayClean(*value);
			    delete value;
			    mpr_tripvalue[i] = NULL;
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
	fcount = mpr_tripvalue.size();
	for(size_t i=0;i<fcount;i++)
	{
		value = mpr_tripvalue[i];
		if(value)
		{
			glbArrayClean(*value);
			delete value;
			mpr_tripvalue[i] = NULL;
		}
	}
	mpr_tripvalue.clear();
	mpr_tripvalue = newValues;
	return true;
}

void CGlbTin::UpdateExtent()
{
	glbDouble x,y,z;
	if(IsEmpty()) 
		return;
	if(mpr_extent.get() == NULL)return;

	glbDouble minx,miny,minz,maxx,maxy,maxz;
	minx = maxx = mpr_vertexcoord[0];
	miny = maxy = mpr_vertexcoord[1];
	minz = maxz = mpr_vertexcoord[2];
	for (glbInt32 i=0;i<mpr_vertexescount;i++)
	{
		x=mpr_vertexcoord[i*3];
		y=mpr_vertexcoord[i*3+1];
		z=mpr_vertexcoord[i*3+2];

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
