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
#pragma warning(disable:4251)

#include <vector>
#include "GlbField.h"
#include "glbref_ptr.h"
#include "GlbExtent.h"
#include "GlbCommTypes.h"
#include "GlbError.h"
#include "GlbDataEngineExport.h"
using namespace std;

#define MAXLEVEL 4 //��󼸺�level���𣬲��ᵽ���С��0

class IGlbGeometry;

/**
  * @brief ���Լ�����
  *
  * ����̳�CGlbReference
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class  GLBDATAENGINE_API CGlbGeometryClass :public CGlbReference
{
public:	
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	CGlbGeometryClass();
/**
  * @brief ��������
  *
  * û���κβ���������ֵ
  */
	~CGlbGeometryClass();
/**
  * @brief ���һ���ֶ�
  * @brief ���ͱ�������������:
  *        GLB_DATATYPE_BYTE    �޷��� 8λ
  *        GLB_DATATYPE_CHAR    �з��� 8λ
  *        GLB_DATATYPE_BOOLEAN ����
  *        GLB_DATATYPE_INT16   ������16λ
  *        GLB_DATATYPE_INT32   ��������32λ
  *        GLB_DATATYPE_INT64   64λ����
  *        GLB_DATATYPE_FLOAT   �����ȸ�������32�ֽ�
  *        GLB_DATATYPE_DOUBLE  ��������64λ
  *        GLB_DATATYPE_DATE    ����+ʱ��
  *        GLB_DATATYPE_VECTOR  ������3��double,xyz��
  * @brief �ֶα�����ȱʡֵ.
  * @brief �ֶβ�����NULL
  * @brief �ֶ����Ʋ����ִ�Сд
  *
  * @param fld ��Ҫ��ӵ��ֶ���Ϣ
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool             AddField( CGlbField* fld);
/**
  * @brief ��ȡһ���ֶ�
  *
  * ���������Ӽ�������ң�������
  *
  * @param fldindex Ҫ��ȡ�ֶε�����
  * @return �ɹ����ֶΣ�ʧ��NULL
  */
	CGlbField*          GetField(glbInt32 fldindex);
/**
  * @brief �������ֻ�ȡһ���ֶ�
  *
  * �������ֵ�ƥ����Ҹ��ֶΣ�������
  *
  * @param name Ҫ��ȡ�ֶε�����
  * @return �ɹ����ֶΣ�ʧ��NULL
  */
	CGlbField*          GetFieldByName(const glbWChar* name);
/**
  * @brief ��ȡ�������ֶ�����
  *
  * ���ؼ������ֶ�����
  *
  * @return �ֶεĸ���
  */
	glbInt32            GetFieldCount();
/**
  * @brief �������ֲ����ֶ��ڼ��ϵ�����
  *
  * ͨ������ƥ���ҵ����ֶΣ����������±�����ֵ
  *
  * @param name Ҫ������ֶε�����
  * @return ���ҵ����ظ��ֶ���ȷ���������򷵻�-1
  */
	glbInt32            FindFieldIndex(const glbWChar* name);
/**
  * @brief ɾ��ָ������λ�õ��ֶ�
  *
  * ͨ�������ҵ����ֶΣ�ɾ��֮
  *
  * @param fldindex Ҫɾ�����ֶ�����
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool             RemoveField(glbInt32 fldindex);
/**
  * @brief ��������ɾ��ĳ�������ֶ�
  *
  * ͨ�������ҵ����ֶΣ�ɾ��֮
  *
  * @param name Ҫɾ�����ֶ���
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool             RemoveFieldByName(const glbWChar* name);
private:    
	vector<glbref_ptr<CGlbField>>	mpr_vecfields;		///<�ֶ��б�	
};

/**
  * @brief ���ζ���ӿ�
  *
  * ����̳�CGlbReference
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API IGlbGeometry :public CGlbReference
{
public:
/**
  * @brief ��������
  *
  * û���κβ���������ֵ
  */
	virtual ~IGlbGeometry(){};
public:
/**
  * @brief ��ȡ���󼸺�����
  *
  * @return ��������
  */
	virtual GlbGeometryTypeEnum GetType() =0;
/**
  * @brief �жϼ��ζ����Ƿ�Ϊ��
  *
  * @return �գ�����true����Ϊ�գ�����false
  */
	virtual glbBool             IsEmpty() =0;
/**
  * @brief ��ȡ���ζ������
  *
  * @return ���
  */
	virtual const CGlbExtent*   GetExtent()=0;
/**
  * @brief ��ȡ������Ϣ
  *
  * ���һ�δ�����Ϣ
  */
	glbWChar*	                GetLastError(){ return GlbGetLastError();}
/**
  * @brief ��ȡ��ռ���ڴ��С.
  */
	virtual glbUInt32           GetSize() =0;
/**
  *  @brief ��ȡ����γ��:
  *  @brief 2 �����ά����X,Y
  *  @brief 3 ������ά����X,Y,Z
  *  @brief 0 ����ά������,ֻ��CGlbGeometryCollect�᷵��0
  */
	virtual glbInt32            GetCoordDimension()  =0;
/**
  *  @brief �Ƿ������ֵ
  *  @brief true ������,false������
  *  @brief CGlbGeometryCollect �����ø÷���ֵȷ�����Ӷ����Ƿ������.
  *                             �����Ӷ����HasM���ж����Ƿ������.
  */
	virtual glbBool             HasM() = 0;
/**
  *  @brief �ÿն���
  */
	virtual void                Empty() = 0;
	virtual glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls) = 0;
	virtual const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype) = 0;
};

/**
  * @brief ���ζ��󼯺���
  *
  * ����̳�IGlbGeometry
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbGeometryCollect :public IGlbGeometry
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	CGlbGeometryCollect();
/**
  * @brief ��������
  *
  * û���κβ���������ֵ
  */
	~CGlbGeometryCollect();
public:
/**
  * @brief ��ȡ���󼸺�����
  *
  * @return ��������
  */
	GlbGeometryTypeEnum		GetType();
/**
  * @brief �жϼ��ζ����Ƿ�Ϊ��
  *
  * @return �գ�����true����Ϊ�գ�����false
  */
	glbBool					IsEmpty();
/**
  * @brief ��ȡ�������
  *
  * @return ���
  */
	CGlbExtent*				GetExtent();
/**
  * @brief ��ȡ��ռ���ڴ��С.
  */
	glbUInt32               GetSize();
/**
  *  @brief ��ȡ����γ��:
  *  @brief 2 �����ά����X,Y
  *  @brief 3 ������ά����X,Y,Z
  *  @brief 0 ����ά������,ֻ��CGlbGeometryCollect�᷵��0
  */
	glbInt32                GetCoordDimension();
/**
  *  @brief �Ƿ������ֵ
  *  @brief true ������,false������
  *  @brief CGlbGeometryCollect �����ø÷���ֵȷ�����Ӷ����Ƿ������.
                                �����Ӷ����HasM���ж����Ƿ������.
  */
	glbBool                 HasM();
/**
  *  @brief �ÿն���
  */
	void                    Empty();
	glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls);
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype);
public:
/**
  * @brief ���һ�����ζ���
  *
  * @param geo ��ӵļ��ζ���
  * @return    �ɹ�����true��ʧ�ܷ���false
  */
	glbBool					AddGeometry( IGlbGeometry* geo);
/**
  * @brief ɾ��һ�����ζ���
  *
  * @param idx ɾ��������
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool					DeleteGeometry(glbInt32 idx);
/**
  * @brief ��ȡ���ζ���
  *
  * @param idx Ҫ��ȡ�ļ�������λ��
  * @param geo ���ؼ��ζ���
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool					GetGeometry( glbInt32 idx,IGlbGeometry** geo);
/**
  * @brief ��ȡ�����ж�������
  *
  * @return ���ض�������
  */
	glbInt32                GetCount();
private:
	vector<glbref_ptr<IGlbGeometry>> mpr_geometries;	///< ���δ洢����
	glbref_ptr<CGlbExtent>           mpr_extent;		///< ���
};

/**
  * @brief glbArray ��������ṹ��		 
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-11 14:40
  */
typedef struct  _GlbARRAY
{
	GlbDataTypeEnum vt;		 ///<����
	glbInt32        dims;     ///<ά��
	glbInt32        dimSize;  ///<ά�ȴ�С
	union
	{
		glbByte*   pBytes;					///< byte����
		glbChar*   pChars;					///< char����
		glbBool*   pBools;				    ///< bool����
		glbInt16*  pInts;					///< int16����
		glbInt32*  pLongs;					///< int32����
		glbInt64*  pLLongs;				    ///< int64����
		glbFloat*  pFloats;					///< float����
		glbDouble* pDoubles;				///< double����
        glbWChar** pStrs;				    ///< ���ַ���
		glbDate*   pDates;					///< ����
		glbref_ptr<IGlbGeometry>** pGeos;	///< ��������
		glbVector*   pVectors;				///< double����
        glbBlob*     pBlobs;				///< blob����
	};
}glbArray;
/**
  * @brief �Զ������
  *	@brief �������������������Լ���������
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
struct tagGlbVariant
{
	GlbDataTypeEnum vt;		 ///<����
	glbBool         isnull;  ///<�Ƿ��ǿ�ֵ
	glbBool         isarray; ///<�Ƿ�����
	union
	{
		glbByte   bVal;					///< byte����
		glbChar   cVal;					///< char����
		glbBool   blVal;				///< bool����
		glbInt16  iVal;					///< int16����
		glbInt32  lVal;					///< int32����
		glbInt64  llVal;				///< int64����
		glbFloat  fVal;					///< float����
		glbDouble dblVal;				///< double����
        glbWChar  *strVal;				///< ���ַ���
		glbDate   date;					///< ����
		glbref_ptr<IGlbGeometry>* geo;	///< ��������
		glbVector   vector;				///< double����
        glbBlob     blob;				///< blob����
		glbArray    arrayVal;	        ///< ����
	};
};
typedef struct tagGlbVariant GLBVARIANT;
typedef struct tagGlbVariant *PGLBVARIANT;

GLBDATAENGINE_API glbBool   glbArrayClean(glbArray& glbArr);
GLBDATAENGINE_API glbBool   glbVariantClean(GLBVARIANT& var);

GLBDATAENGINE_API glbBool   glbArrayCopy(glbArray& destArr,glbArray& srcArr);
GLBDATAENGINE_API glbBool   glbVariantCopy(GLBVARIANT& destVar,GLBVARIANT& srcVar);

GLBDATAENGINE_API glbUInt32 glbArraySize(glbArray& glbArr);
GLBDATAENGINE_API glbUInt32 glbVariantSize(GLBVARIANT& var);
size_t GetDataTypeSize(GlbDataTypeEnum ftype);
/**
  * @brief ���ü����ֶ�ֵ
  *
  *
  * @param dstvalue  �����ֶ�ֵ
  * @param idx       ֵ����
  * @param count     ֵ��Ŀ
  * @param srcvalue  ֵ��Դ
  * @param ftype     ��������
  * @return ��
  */
void SetGeoValue(void*           dstvalue,
	             glbInt32        idx,
				 glbInt32        count,
				 const void*     srcvalue,
				 GlbDataTypeEnum ftype);
/**
  * @brief ���ü����ֶ�ֵ
  *
  *
  * @param dstvalue  �����ֶ�ֵ
  * @param idx       ֵ����
  * @param count     ֵ��Ŀ
  * @param srcvalue  ֵ��Դ
  * @param ftype     ��������
  * @return ��
  */
void SetGeoValueVariant(void*             dstvalue,
	                    glbInt32          idx,
						glbInt32          count,
						const GLBVARIANT& srcvalue,
						GlbDataTypeEnum   ftype);
/**
  * @brief ����ɾ������ֵ
  *
  * srcvalue[0,idx]                ---> dstvalue[0,idx]
  * srcvalue[idx+count,srccount-1] ---> dstvalue[idx+1,..]
  *
  * @param dstvalue  Ŀ�꼸��ֵ
  * @param srcvalue  Դ����ֵ
  * @param srccount  Դֵ��Ŀ
  * @param idx         
  * @param count     
  * @param ftype     ��������
  * @return ��
  */
void CopyRemoveGeoValue(void*           dstvalue,
	                    void*           srcvalue,
						glbInt32        srccount,
						glbInt32        idx,
						glbInt32        count,
						GlbDataTypeEnum ftype);
/**
  * @brief �������뼸��ֵ
  *
  * srcvalue [0,  idx]         ----> dstvalue[0,idx]
  * srcvalue [idx,srccount-1]  ----> dstvalue[idx+count]
  * 
  *
  * @param dstvalue    Ŀ�꼸��ֵ
  * @param srcvalue    ֵ��Դ
  * @param srccount    ��Դֵ��Ŀ  
  * @param idx         
  * @param count       Ҫ�����ֵ�ĸ���
  * @param ftype       ��������
  * @return ��
  */
void CopyInsertGeoValue(void*           dstvalue,
	                    void*           srcvalue,
						glbInt32        srccount,
						glbInt32        idx,
						glbInt32        count,
						GlbDataTypeEnum ftype);
/**
  * @brief ��ȡԴ����ָ��λ�õ�����
  *
  * ��Դ����srcvalue��idxλ�õ�ֵ������Ŀ�����dstvalue��
  *
  * @param dstvalue Ŀ��������׵�ַ
  * @param srcvalue Դ�����׵�ַ
  * @param pidx Դ����Ҫȡ��ֵ��λ������
  * @param ftype ��������
  * @return ��
  */
void GetGeoValue(GLBVARIANT& dstvalue,void* srcvalue,glbInt32 pidx,GlbDataTypeEnum ftype);
/**
  * @brief ��ȡԴ�����ֵ����������ε�Ŀ��������
  *
  * ��Դ����srcvalue��ֵ������count�ε�Ŀ������dstvalue��
  *
  * @param dstvalue Ŀ��������׵�ַ
  * @param srcvalue Դ�����׵�ַ
  * @param count �����Ĵ���
  * @param ftype ��������
  * @return ��
  */
void GetGeoValues(void** dstvalue,void* srcvalue,glbInt32 count,GlbDataTypeEnum ftype);