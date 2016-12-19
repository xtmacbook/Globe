#pragma once
#include <vector>
#include "GlbPoint.h"
#include "IGlbGeometry.h"
using namespace std;
/**
  * @brief ����������
  *
  * ����̳���IGlbGeometry���ö���������������
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbTin :public IGlbGeometry
{
public:
	CGlbTin(glbBool hasM = false);
	~CGlbTin();
public:
/**
  * @brief ��ȡ�������
  *
  * @return ���
  */
	CGlbExtent*			GetExtent();
/**
  * @brief ��ȡ���󼸺�����
  *
  * @return ��������
  */
	GlbGeometryTypeEnum	GetType();
/**
  * @brief �жϼ��ζ����Ƿ�Ϊ��
  *
  * @return �գ�����true����Ϊ�գ�����false
  */
	glbBool				IsEmpty();
/**
  * @brief ��ȡ��ռ���ڴ��С.
  */
   glbUInt32            GetSize();
/**
  *  @brief ��ȡ����γ��:
  *  @brief 2 �����ά����X,Y
  *  @brief 3 ������ά����X,Y,Z
  *  @brief 0 ����ά������,ֻ��CGlbGeometryCollect�᷵��0
  */
	glbInt32            GetCoordDimension();
/**
  *  @brief �Ƿ������ֵ
  *  @brief true ������,false������
  *  @brief CGlbGeometryCollect �����ø÷���ֵȷ�����Ӷ����Ƿ������.
                                �����Ӷ����HasM���ж����Ƿ������.
  */
	glbBool             HasM();
/**
  *  @brief �ÿն���
  */
	void                Empty();
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
  *             3. ctype ֻ����GLB_GEOCLASS_POINT,GLB_GEOCLASS_TRIANGLE
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
 //CGlbTin ����
 public:
/**
  * @brief  ��ȡ�����
  *
  *
  * @return �����
  */
	glbInt32			GetVertexCount();
/**
  * @brief     ��ȡ�����x,y,z����
  *
  * @param idx ��������±�
  * @param ptx x����
  * @param pty y����
  * @param ptz z����
  * @return �ɹ�true��ʧ��false
  */
	glbBool				GetVertex(glbInt32 idx,glbDouble* ptx,glbDouble* pty,glbDouble* ptz);
/**
  * @brief ���һ������
  *
  * @param x ���x����
  * @param y ���y����
  * @param z ���z����
  * @return �ɹ�true��ʧ��false
  */
	glbBool				AddVertex(glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief ���һ������
  *
  * @param coords ������������XYZXYZ...
  * @param count  ��Ŀ
  * @return �ɹ�true��ʧ��false
*/
	glbBool             AddVertexes(glbDouble* coords,glbInt32 count);
/**
  * @brief �޸Ķ�������
  *
  * @param idx �������
  * @param x   ���x����
  * @param y   ���y����
  * @param z   ���z����
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetVertex(glbInt32 idx,glbDouble x,glbDouble y, glbDouble z);
/**
  * @brief ɾ��һ������
  *
  * @param idx �������
  * @return �ɹ�true��ʧ��false
  */
	glbBool				DeleteVertex(glbInt32 idx);
/**
  * @brief ɾ��һ������
  *
  * @param idx   �������
  * @param count ��Ŀ
  * @return �ɹ�true��ʧ��false
  */
	glbBool             DeleteVertexes(glbInt32 idx,glbInt32 count);
/**
  * @brief ��ȡ���󶥵������б�
  *
  * @return ���������б�
  */
	const glbDouble*	GetVertexes();
/**
  * @brief ��ȡ��Ķ���ֵ
  *
  * @param idx ����λ��
  * @param m   ����ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool             GetM(glbInt32 idx,glbDouble* m);
/**
  * @brief �޸ĵ�Ķ���ֵ
  *
  * @param idx ����λ��
  * @param m   ����ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool             SetM(glbInt32 idx,glbDouble m);
/**
  * @brief ��ȡ��������
  */
	const glbDouble*    GetMs();
/**
  * @brief ��ȡ�����θ���
  *
  * @return �����θ���
  */
	glbInt32			GetTriangleCount();
/**
  * @brief ��ȡ������
  *
  * @param idx          �����ε�����
  * @param first_ptidx  ��һ��������
  * @param second_ptidx �ڶ���������
  * @param third_ptidx  ������������
  * @return �ɹ�true��ʧ��false
  */
	glbBool				GetTriangle(glbInt32 idx,glbInt32* first_ptidx,glbInt32* second_ptidx,glbInt32* third_ptidx);
/**
  * @brief ���һ��������
  *
  * @param first_ptidx  ��һ��������
  * @param second_ptidx �ڶ���������
  * @param third_ptidx  ������������
  * @return �ɹ�true��ʧ��false
  */
	glbBool				AddTriangle(glbInt32 first_ptidx,glbInt32 second_ptidx,glbInt32 third_ptidx);
/**
  * @brief ���һ��������
  *
  * @param ptidxes  �����ζ�����������
  * @param count    ��Ŀ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				AddTriangles(glbInt32* ptidxes,glbInt32 count);
/**
  * @brief �޸�������
  *
  * @param idx          �����ε�����
  * @param first_ptidx  ��һ��������
  * @param second_ptidx �ڶ���������
  * @param third_ptidx  ������������
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetTriangle(glbInt32 idx,glbInt32 first_ptidx,glbInt32 second_ptidx,glbInt32 third_ptidx);
/**
  * @brief ɾ��һ��������
  *
  * @param idx  �����ε�����
  * @return �ɹ�true��ʧ��false
  */
	glbBool				DeleteTriangle(glbInt32 idx);
/**
  * @brief ɾ��һ��������
  *
  * @param idx    �����ε�����
  * @param count  ��Ŀ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				DeleteTriangles(glbInt32 idx,glbInt32 count);
/**
  * @brief ��ȡ������
  *
  * @return ������
  */
	const glbInt32*		GetTriangles();
/**
  * @brief ��ȡһ��������ֶ�ֵ
  *
  * @param fidx   �ֶ�����
  * @param pidx   ��������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				GetVertexValue(glbInt32 fidx, glbInt32 pidx, GLBVARIANT& pvalue);
/**
  * @brief ��ȡһ��������ֶ�ֵ
  *
  * @param fname  �ֶ���
  * @param pidx   ��������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				GetVertexValueByName(glbWChar* fname,glbInt32 pidx, GLBVARIANT& pvalue);
/**
  * @brief �޸�һ��������ֶ�ֵ
  *
  * @param fidx   �ֶ�����
  * @param pidx   ��������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetVertexValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief �޸�һ��������ֶ�ֵ
  *
  * @param name   �ֶ���
  * @param pidx   ��������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetVertexValueByName(glbWChar* name ,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief ��ȡ���ж����ֶ�ֵ
  *
  * @param  fidx    �ֶ�����
  * @return �ֶ�ֵ
  */
	const glbArray*		GetVertexesValue(glbInt32 fidx);
/**
  * @brief ��ȡ���ж����ֶ�ֵ
  *
  * @param fname   �ֶ���
  * @return �ֶ�ֵ
  */
	const glbArray*		GetVertexesValueByName(glbWChar* fname);
/**
  * @brief �޸Ķ����ֶ�ֵ
  *
  * @param fidx    �ֶ�����
  * @param pvalues �ֶ�ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetVertexesValue(glbInt32 fidx,glbArray& pvalues);
/**
  * @brief �޸Ķ����ֶ�ֵ
  *
  * @param fname   �ֶ���
  * @param pvalues �ֶ�ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetVertexesValueByName(glbWChar* fname ,glbArray& pvalues);
/**
  * @brief ��ȡ���ε��ֶ�ֵ
  *
  * @param fidx   �ֶ�����
  * @param pidx   ����������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool 			GetTriangleValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief ��ȡ���ε��ֶ�ֵ
  *
  * @param fname  �ֶ���
  * @param pidx   ����������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				GetTriangleValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief �޸��������ֶ�ֵ
  *
  * @param fidx   �ֶ�����
  * @param pidx   ����������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetTriangleValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief �޸��������ֶ�ֵ
  *
  * @param fname  �ֶ���
  * @param pidx   ����������
  * @param pvalue ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetTriangleValueByName(glbWChar* fname,glbInt32 pidx, GLBVARIANT& pvalue);
/**
  * @brief ��ȡ�������ֶ�ֵ
  *
  * @param fidx    �ֶ�����
  * @return �ֶ�ֵ
  */
	const glbArray*		GetTrianglesValue(glbInt32 fidx);
/**
  * @brief �����ֶ�����ȡ���������ε�ָ�������ֶε�����ֵ�б�
  *
  * @param fname ָ�����ֶ���
  * @param pvalues Ҫ��ȡ��ֵ
  * @return �ɹ�true��ʧ��false
  */
	const glbArray*		GetTrianglesValueByName(glbWChar* fname);
/**
  * @brief �޸��������ֶ�ֵ
  *
  * @param fidx    �ֶ�����
  * @param pvalues ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetTrianglesValue(glbInt32 fidx,glbArray& pvalues);
/**
  * @brief �޸��������ֶ�ֵ
  *
  * @param fname   �ֶ���
  * @param pvalues ֵ
  * @return �ɹ�true��ʧ��false
  */
	glbBool				SetTrianglesValueByName(glbWChar* fname, glbArray& pvalues);	
private:
	void				UpdateExtent();
	void                EmptyVertex();
	void                EmptyTriangle();
	glbBool				SetVertexClass(CGlbGeometryClass* gcls);
    glbBool				SetTriangleClass(CGlbGeometryClass* gcls);
private:
	glbInt32						mpr_vertexescount;			///< �������
	glbInt32						mpr_trianglecount;			///< �����θ���
	glbref_ptr<CGlbGeometryClass>	mpr_vergeometryclass;		///< ������
	glbref_ptr<CGlbGeometryClass>	mpr_trigeometryclass;		///< ��������
	glbDouble*						mpr_vertexcoord;			///< ���������б�
	glbInt32*						mpr_trivertex;				///< �����ζ���������ÿ������������ʾһ��������
	vector<glbArray*>				mpr_verpvalue;				///< ��������ֵ�б�
	vector<glbArray*>				mpr_tripvalue;				///< ����������ֵ�б�
	glbref_ptr<CGlbExtent>			mpr_extent;					///< ���	
	glbBool                         mpr_hasM;                   ///<�Ƿ������
	glbDouble*                      mpr_m;
};

class GLBDATAENGINE_API CGlbMultiTin :public IGlbGeometry
{
public:
	CGlbMultiTin(glbBool hasM = false);
	~CGlbMultiTin();
	GlbGeometryTypeEnum GetType();
    glbBool             IsEmpty();
    const CGlbExtent*   GetExtent();
    glbUInt32           GetSize();
    glbInt32            GetCoordDimension();
    glbBool             HasM();
    void                Empty();
	glbBool             SetGeoClass
		                (GlbGeometryClassEnum ctype,
		                 CGlbGeometryClass* gcls);
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype);
public:
	/**
	* @brief ��ȡ����Tin����
	*
	*
	* @return Tin����
	*/
	glbInt32			GetCount();
	/**
	* @brief ���һ��Tin
	*
	* @param tin Ҫ��ӵ�Tin
	* @return �ɹ�true��ʧ��false
	*/
	glbBool				AddTin( CGlbTin* tin);
	/**
	* @brief ���һ��Tin
	*
	*/
	glbBool             AddTins(CGlbTin** tins,glbInt32 cnt);
	/**
	* @brief ɾ��һ��Tin
	*
	* @param idx ����λ��
	* @return �ɹ�true��ʧ��false
	*/
	glbBool				DeleteTin(glbInt32 idx);
	/**
	* @brief ɾ��һ��Tin
	*
	*/
	glbBool             DeleteTins(glbInt32 idx,glbInt32 cnt);
	/**
	* @brief ��ȡһ��Tin
	*
	* @param  idx ����λ��
	* @return tin��ʧ�ܷ���NULL
	*/
	const CGlbTin*	    GetTin(glbInt32 idx);
private:
	void UpdateExtent();
private:
	glbInt32						mpr_count;			///< ����θ���
	glbref_ptr<CGlbTin>*	        mpr_tins;		    ///< ����μ���
	glbref_ptr<CGlbExtent>			mpr_extent;			///< ���
	glbref_ptr<CGlbGeometryClass>	mpr_vertexcls;		///< ������
	glbref_ptr<CGlbGeometryClass>	mpr_tricls;		    ///< ��������
	glbByte                         mpr_coordDimension; ///<����ά��
	glbBool                         mpr_hasM; 
};