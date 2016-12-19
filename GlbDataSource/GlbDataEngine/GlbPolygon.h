#pragma once
#include <vector>
#include "IGlbGeometry.h"
#include "GlbLine.h"
using namespace std;

/**
  * @brief �����
  *
  * ����̳���IGlbGeometry���ö�����һ���⻷������ڻ���ɣ��ڻ����⻷��Χ����
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class  GLBDATAENGINE_API CGlbPolygon :public IGlbGeometry
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
 CGlbPolygon(glbByte coordDimension = 2,glbBool hasM = false);
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
 ~CGlbPolygon(void);
public:
/**
  * @brief ��ȡ�������
  *
  * @return ���
  */
	const CGlbExtent*			GetExtent();
/**
  * @brief ��ȡ���󼸺�����
  *
  * @return ��������
  */
	GlbGeometryTypeEnum			GetType();
/**
  * @brief �жϼ��ζ����Ƿ�Ϊ��
  *
  * @return �գ�����true����Ϊ�գ�����false
  */
	glbBool						IsEmpty();
/**
  * @brief ��ȡ��ռ���ڴ��С.
  */
	glbUInt32                   GetSize();
/**
  *  @brief ��ȡ����γ��:
  *  @brief 2 �����ά����X,Y
  *  @brief 3 ������ά����X,Y,Z
  */
	glbInt32                    GetCoordDimension();
/**
  *  @brief �Ƿ������ֵ
  *  @brief true ������,false������
  */
	glbBool                     HasM();
/**
  *  @brief �ÿն���
  */
	void                        Empty();
/**
  * @brief ���ü�����
  *        ���ö��󼸺��������������
  *             1. ����û�м�����
  *             2. �������м�����
  *                    2.1 �ֶ����� ������Ӧ���ֶ�ֵ���Ͳ�ͬ
  *                            ������ֶ�ֵ�ᱻ���
  *                    2.2 �ֶ����� ������Ӧ���ֶ�ֵ������ͬ
  *                            ������ֶ�ֵ����.
  *                            ��ʱ���ֶ�ֵҲ����û�����,��������Ҫ��������!!!
  *             3. ctype ֻ����GLB_GEOCLASS_POINT
  *
  * @param gcls ������
  * @return �ɹ�true��ʧ��false
  */
	glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls);
/**
  * @brief ��ȡ������
  *
  * @return ������
  */
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype);
public:
/**
  * @brief ��ȡ�����ڻ�����
  *
  *
  * @return ����������ڻ��ĸ���
  */
	glbInt32					GetInRingCount();
/**
  * @brief ��ȡ������⻷
  *
  *
  * @return ������⻷
  */
	const CGlbLine*				GetExtRing();
/**
  * @brief ���ö�����⻷
  *
  * @param ring ������
  * @return �ɹ�true��ʧ��false
  */
	glbBool						SetExtRing(CGlbLine* ring);
/**
  * @brief ���������һ���ڻ�
  *
  * @param ring ������
  * @return �ɹ�true��ʧ��false
  */
	glbBool						AddInRing(CGlbLine* ring);
/**
  * @brief ���һ���ڻ�
  *
  * @param count ��Ŀ
  * @param rings �ڻ�
  * @return �ɹ�true��ʧ��false
  */
	glbBool                     AddInRings(CGlbLine** rings,glbInt32 count);
/**
  * @brief �Ӷ����л�ȡ����λ�õ��ڻ�
  *
  * @param idx Ҫ��ȡ������λ��
  * @return ��ȡ�Ļ�����
  */
	const CGlbLine*				GetInRing(glbInt32 idx);
/**
  * @brief ɾ���ڻ�
  *
  * @param idx ����λ��
  * @return �ɹ�true��ʧ��false
  */
	glbBool						DeleteInRing(glbInt32 idx);
/**
  * @brief ɾ���ڻ�
  *
  * @param ring �ڻ�
  * @return �ɹ�true��ʧ��false
  */
    glbBool                     DeleteInRing(CGlbLine* ring);
/**
  * @brief ɾ��һ���ڻ�
  *
  * @param idx Ҫɾ��������λ��
  * @param count ��Ŀ
  * @return �ɹ�true��ʧ��false
  */
	glbBool                     DeleteInRings(glbInt32 idx,glbInt32 count);
private:
	void						UpdateExtent();
private:
	glbref_ptr<CGlbLine>		mpr_extRing;	///< �⻷
	glbref_ptr<CGlbLine>*		mpr_inRings;	///< �ڻ�����
	glbInt32					mpr_inCount;    ///< �ڻ�����
	glbref_ptr<CGlbExtent>		mpr_extent;		///< ���	
	glbref_ptr<CGlbGeometryClass>	mpr_geometryclass;	///<��ļ��������ֶμ���
	glbByte                     mpr_coordDimension;  ///<����ά��
	glbBool                     mpr_hasM; 
};

/**
  * @brief ������
  *
  * ����̳���IGlbGeometry
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbMultiPolygon:public IGlbGeometry
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	CGlbMultiPolygon(glbByte coordDimension = 2,glbBool hasM = false);
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	~CGlbMultiPolygon();
public:
/**
  * @brief ��ȡ���󼸺�����
  *
  * @return ��������
  */
	GlbGeometryTypeEnum				GetType();
/**
  * @brief �жϼ��ζ����Ƿ�Ϊ��
  *
  * @return �գ�����true����Ϊ�գ�����false
  */
	glbBool							IsEmpty();
/**
  * @brief ��ȡ�������
  *
  * @return ���
  */
	const CGlbExtent*	     		GetExtent();
/**
  * @brief ��ȡ��ռ���ڴ��С.
  */
	glbUInt32                       GetSize();
/**
  *  @brief ��ȡ����γ��:
  *  @brief 2 �����ά����X,Y
  *  @brief 3 ������ά����X,Y,Z
  *  @brief 0 ����ά������,ֻ��CGlbGeometryCollect�᷵��0
  */
	glbInt32                        GetCoordDimension();
/**
  *  @brief �Ƿ������ֵ
  *  @brief true ������,false������
  *  @brief CGlbGeometryCollect �����ø÷���ֵȷ�����Ӷ����Ƿ������.
  *                             �����Ӷ����HasM���ж����Ƿ������.
  */
	glbBool                         HasM();
/**
  *  @brief �ÿն���
  */
	void                            Empty();
/**
  * @brief ���ü�����
  *        ���ö��󼸺��������������
  *             1. ����û�м�����
  *             2. �������м�����
  *                    2.1 �ֶ����� ������Ӧ���ֶ�ֵ���Ͳ�ͬ
  *                            ������ֶ�ֵ�ᱻ���
  *                    2.2 �ֶ����� ������Ӧ���ֶ�ֵ������ͬ
  *                            ������ֶ�ֵ����.
  *                            ��ʱ���ֶ�ֵҲ����û�����,��������Ҫ��������!!!
  *             3. ctype ֻ����GLB_GEOCLASS_POINT
  *
  * @param gcls ������
  * @return �ɹ�true��ʧ��false
  */
	glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls);
/**
  * @brief ��ȡ������
  *
  * @return ������
  */
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype);
public:
/**
  * @brief ��ȡ�������θ���
  *
  *
  * @return ��������Ķ���θ���
  */
	glbInt32						GetCount();
/**
  * @brief ���һ�������
  *
  * @param polygon Ҫ��ӵĶ����
  * @return �ɹ�true��ʧ��false
  */
	glbBool							AddPolygon( CGlbPolygon*  polygon);
/**
  * @brief ���һ�������
  *
 */
	glbBool                         AddPolygons(CGlbPolygon** polygons,glbInt32 cnt);
/**
  * @brief ɾ��һ�������
  *
  * @param idx ����λ��
  * @return �ɹ�true��ʧ��false
  */
	glbBool							DeletePolygon(glbInt32 idx);
/**
  * @brief ɾ��һ�������
  *
  * @param polygon �����
  * @return �ɹ�true��ʧ��false
  */
	glbBool                         DeletePolygon(CGlbPolygon* polygon);
/**
  * @brief ɾ��һ�������
  *
  */
	glbBool                         DeletePolygons(glbInt32 idx,glbInt32 cnt);
/**
  * @brief ��ȡһ�������
  *
  * @param idx Ҫ��ȡ������λ��
  * @return ��ȡ�Ķ���ζ���
  */
	const CGlbPolygon*	  		    GetPolygon(glbInt32 idx);
private:
	void							UpdateExtent();
private:
	glbInt32						mpr_count;			///< ����θ���
	glbref_ptr<CGlbPolygon>*	    mpr_polygons;		///< ����μ���
	glbref_ptr<CGlbExtent>			mpr_extent;			///< ���
	glbref_ptr<CGlbGeometryClass>   mpr_geocls;
	glbByte                         mpr_coordDimension; ///<����ά��
	glbBool                         mpr_hasM;  
};
