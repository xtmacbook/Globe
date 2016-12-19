#pragma once
#include <vector>
#include "IGlbGeometry.h"
using namespace std;

/**
  * @brief ��
  *
  * ����̳���IGlbGeometry���ö������һϵ�е�����͵��Ӧ��n������ֵ���
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbLine:public IGlbGeometry
{
public:
/**
  * @brief ���캯��
  *
  * @param coordDimension ����ά��,2������XY,3������XYZ
  * @param hasM           true �����ж���ֵ,false����û�ж���ֵ
  */
	CGlbLine(glbByte coordDimension = 2,glbBool hasM = false);
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	~CGlbLine(void);
public:
/**
  * @brief ��ȡ�������
  *
  * @return ���
  */
	const CGlbExtent*				GetExtent();
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
	glbUInt32                       GetSize();
/**
  *  @brief ��ȡ����γ��:
  *  @brief 2 �����ά����X,Y
  *  @brief 3 ������ά����X,Y,Z
  */
	glbInt32                        GetCoordDimension();
/**
  *  @brief �Ƿ������ֵ
  *  @brief true ������,false������
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
  * @brief ��ȡ��������
  *
  *
  * @return ��������ĵ����
  */
	glbInt32						GetCount();
/**
  * @brief �ж��߶����Ƿ��Ǳջ�
  *
  *
  * @return �ǣ�true����false
  */
	glbBool							IsClosed();
/**
  * @brief ���һ����
  * @brief �������ά��,z��ֵΪ 0
  * @brief ���������ֵ,����ֵ��ֵΪ0
  * @param x ��ӵ�x����
  * @param y ��ӵ�y����
  * @return �ɹ�true��ʧ��false
  */
	glbBool							AddPoint(glbDouble x,glbDouble y) ;
/**
  * @brief ���һ����
  * @brief ����Ƕ�ά��,z �ᱻ����.
  * @brief ���������ֵ,����ֵ��ֵΪ0
  * @param x ��ӵ�x����
  * @param y ��ӵ�y����
  * @param z ��ӵ�z����
  * @return �ɹ�true��ʧ��false
  */
	glbBool                         AddPoint(glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief ����һ����
  * @brief �������ά��,z��ֵΪ 0
  * @brief ���������ֵ,����ֵ��ֵΪ0
  *
  * @param idx ������λ��
  * @param x   �����x����
  * @param y   �����y����
  * @return �ɹ�true��ʧ��false
  */
	glbBool							InsertPoint(glbInt32 idx,glbDouble x,glbDouble y);
/**
  * @brief ����һ����
  * @brief ����Ƕ�ά��,z �ᱻ����
  * @brief ���������ֵ,����ֵ��ֵΪ0
  *
  * @param idx ������λ��
  * @param x   �����x����
  * @param y   �����y����
  * @param z   �����z����
  * @return �ɹ�true��ʧ��false
  */
	glbBool							InsertPoint(glbInt32 idx,glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief ����һ����
  * @brief ���������ֵ,����ֵ��ֵΪ0
  *
  * @param coords ��������б�
                  ��ά��,��XYXY...������,��ά����XYZXYZ...������
  * @param count  ��ĸ���
  * @return �ɹ�true��ʧ��false
  */
	glbBool							InsertPoints(glbInt32 idx,glbDouble* coords,glbInt32 count);
/**
  * @brief ɾ��һ����
  *
  * @param idx ɾ���������λ��
  * @return �ɹ�true��ʧ��false
  */
	glbBool							DeletePoint(glbInt32 idx);
/**
  * @brief ɾ��һ����
  *
  * @param idx ɾ���������λ��
  * @param idx ��ĸ���
  * @return �ɹ�true��ʧ��false
  */
	glbBool							DeletePoints(glbInt32 idx,glbInt32 count);
/**
  * @brief ��ȡ����ָ������λ�õĵ�x,y����
  *
  * @param idx Ҫ��ȡ��������±�
  * @param ptx x����
  * @param pty y����
  * @return �ɹ�true��ʧ��false
  */
	glbBool							GetPoint(glbInt32 idx,glbDouble*ptx,glbDouble* pty);
/**
  * @brief ��ȡ����ָ������λ�õĵ�x,y,z����
  * @brief ����Ƕ�ά��,��Ϊû��z���꣬��ʧ��.
  *
  * @param idx Ҫ��ȡ��������±�
  * @param ptx x����
  * @param pty y����
  * @param pty z����
  * @return �ɹ�true��ʧ��false
  */
	glbBool							GetPoint(glbInt32 idx,glbDouble*ptx,glbDouble* pty,glbDouble *ptz);
/**
  * @brief ���ö���ָ������λ�õ������
  * @brief �������ά��,z��ֵΪ 0
  * @brief ���������ֵ,����ֵ��ֵΪ0
  *
  * @param idx Ҫ���õ�����λ��
  * @param x Ҫ���õĵ��x����
  * @param y Ҫ���õĵ��y����
  * @return �ɹ�true��ʧ��false
  */
	glbBool							SetPoint(glbInt32 idx,glbDouble x,glbDouble y);
/**
  * @brief ���ö���ָ������λ�õ������
  * @brief ����Ƕ�άά��,z ����
  * @brief ���������ֵ,����ֵ��ֵΪ0

  * @param idx Ҫ���õ�����λ��
  * @param x Ҫ���õĵ��x����
  * @param y Ҫ���õĵ��y����
  * @param z Ҫ���õĵ��y����
  * @return �ɹ�true��ʧ��false
  */
	glbBool							SetPoint(glbInt32 idx,glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief ��ȡ����ֵ����
  * @brief ��ά��,��XYXY...������,��ά����XYZXYZ...������
  *
  * @return ����ֵ�б�
  */
	const glbDouble*				GetPoints();
/**
  * @brief ��ȡ��Ķ���ֵ
  *
  * @param idx Ҫ���õ�����λ��
  * @param m  ����ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool                         GetM(glbInt32 idx,double* m);
/**
  * @brief ���õ�Ķ���ֵ
  *
  * @param idx Ҫ���õ�����λ��
  * @param m   ����ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool                         SetM(glbInt32 idx,double m);
/**
  * @brief ��ȡ��������
  */
	const glbDouble*                GetMs();
/**
  * @brief ��ȡһ�����ֶ�ֵ
  *        ����ʧ�ܵ�ԭ��:
  *            1.������Empty
  *            2.û���ֶ�
  *            3.������<0 �� ���ڵ���Ŀ
  *            4.�ֶ����� <0 �� >= �ֶ���Ŀ
  *            5.�ֶα仯��,û������SetPointClass,���:
                    5.1 �ֶ���Ŀ�仯
					5.2 �ֶ����ͱ仯
  *
  * @param fidx   �ֶ�����
  * @param pidx   ������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool							GetPointValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief ����һ�����ֶ�ֵ
  *        ����ʧ�ܵ�ԭ��:
  *            1.������Empty
  *            2.û���ֶ�
  *            3.������<0 �� ���ڵ���Ŀ
  *            4.�ֶ����� <0 �� >= �ֶ���Ŀ
  *            5.�ֶα仯��,û������SetPointClass,���:
                    5.1 �ֶ���Ŀ�仯
					5.2 �ֶ����ͱ仯
  *            6.ֵ�������ֶ����Ͳ�һ��
  *
  * @param fidx   �ֶ�����
  * @param pidx   ������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool							SetPointValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief ��ȡ��ļ����ֶ�ֵ
  *
  * @param fname  �ֶ�����
  * @param pidx   ������
  * @param pvalue Ҫ��ȡ��ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool							GetPointValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief ���õ�ļ����ֶ�ֵ
  *
  * @param fname  �ֶ�����
  * @param pidx   ������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool							SetPointValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief ��ȡ���е���ֶ�ֵ
  *
  * @param fidx       �ֶ�����
  * @param pvalues    �ֶ�ֵ
  * @return �ɹ�true��ʧ��false
  */
	const glbArray*					GetPointsValue(glbInt32 fidx);
/**
  * @brief ��ȡ���е���ֶ�ֵ
  *
  * @param fname      �ֶ�����
  * @param pvalues    �ֶ�ֵ
  * @return �ɹ�true��ʧ��false
  */
	const glbArray*					GetPointsValueByName(glbWChar* fname);
/**
  * @brief �����ֶ�ֵ
  *        ����ʧ�ܵ�ԭ��:
  *            1.������Empty
  *            2.û���ֶ�
  *            3.�ֶ����� <0 �� >= �ֶ���Ŀ
  *            4.�ֶα仯��,û������SetPointClass,���:
                    4.1 �ֶ���Ŀ�仯
					4.2 �ֶ����ͱ仯
  *            5.ֵ�������ֶ����Ͳ�һ��
  *            6.ֵ��dims != 1
  *            7.ֵ��dimSize !=0 �� dimSize != ����Ŀ
  * @param fidx    �ֶ�����
  * @param pvalues �ֶ�ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool							SetPointsValue(glbInt32 fidx,glbArray& pvalues);
/**
  * @brief �����ֶ�ֵ
  *
  * @param fname   �ֶ�����
  * @param pvalues ֵ�б�
  * @return �ɹ�true��ʧ��false
  */
	glbBool							SetPointsValueByName(glbWChar* fname,glbArray& pvalues);
private:
	void							UpdateExtent();
private:	
	glbDouble*						mpr_pointscoord;	///<�������б�
	glbInt32						mpr_pointscount;	///<�����
	glbref_ptr<CGlbGeometryClass>	mpr_geometryclass;	///<��ļ��������ֶμ���
	vector<glbArray*>				mpr_pointsvalue;	///<��ļ�������ֵ�б�
	glbref_ptr<CGlbExtent>			mpr_extent;			///<���
	glbByte                         mpr_coordDimension; ///<����ά��
	glbBool                         mpr_hasM;           ///<�Ƿ������
	glbDouble*                      mpr_m;
 };

 /**
  * @brief ����
  *
  * ����̳���IGlbGeometry
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbMultiLine:public IGlbGeometry
{
public:
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	CGlbMultiLine(glbByte coordDimension = 2,glbBool hasM = false);
/**
  * @brief ���캯��
  *
  * û���κβ���������ֵ
  */
	~CGlbMultiLine();
public:
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
  * @brief ��ȡ�������
  *
  * @return ���
  */
	const CGlbExtent*			GetExtent();
/**
  * @brief ��ȡ��ռ���ڴ��С.
  */
	glbUInt32                   GetSize();
/**
  *  @brief ��ȡ����γ��:
  *  @brief 2 �����ά����X,Y
  *  @brief 3 ������ά����X,Y,Z
  *  @brief 0 ����ά������,ֻ��CGlbGeometryCollect�᷵��0
  */
	glbInt32                    GetCoordDimension();
/**
  *  @brief �Ƿ������ֵ
  *  @brief true ������,false������
  *  @brief CGlbGeometryCollect �����ø÷���ֵȷ�����Ӷ����Ƿ������.
  *                             �����Ӷ����HasM���ж����Ƿ������.
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
  * @brief ��ȡ����Ŀ
  *
  *
  * @return ����Ŀ
  */
	glbInt32					GetCount();
/**
  * @brief ��ȡһ����
  *
  * @param idx ������
  * @return ��
  */
	const CGlbLine*				GetLine(glbInt32 idx);
/**
  * @brief ���һ����
  *
  * @param curve ��
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool						AddLine( CGlbLine* curve);
/**
  *@brief ���һ����
*/
	glbBool                     AddLines(CGlbLine** curves,glbInt32 cnt);
/**
  * @brief ɾ��һ����
  *
  * @param idx ������
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool						DeleteLine(glbInt32 idx);
/**
  * @brief ɾ��һ����
  *
  * @param line ��
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool                     DeleteLine(CGlbLine* line);
/**
  * @brief ɾ��һ����
  *
  * @param idx  ������
  * @param cnt  ����Ŀ
  * @return �ɹ�����true��ʧ�ܷ���false
  */
	glbBool                     DeleteLines(glbInt32 idx,glbInt32 cnt);
private:
	void						UpdateExtent();
private:	
	glbInt32                         mpr_count;
	glbref_ptr<CGlbLine>*            mpr_curves;			///<�߶����б�
	glbref_ptr<CGlbExtent>		     mpr_extent;			///<���
	glbref_ptr<CGlbGeometryClass>    mpr_geocls;
	glbByte                          mpr_coordDimension; ///<����ά��
	glbBool                          mpr_hasM;  
};

