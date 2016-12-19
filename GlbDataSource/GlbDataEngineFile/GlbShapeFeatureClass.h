#pragma once
#include "GlbFileDataSource.h"
#include "ogrsf_frmts.h"
#include "GlbFeature.h"

class CGlbShapeFeatureCursor;

class  CGlbShapeFeatureClass :	public IGlbFeatureClass
{
public:
	CGlbShapeFeatureClass(void);
	~CGlbShapeFeatureClass(void);
	/*
	   shape �ļ��������ͣ�
	       ��:    SHPT_POINT   ---  wkbPoint
		          SHPT_POINTZ  ---  wkbPoint25D
			      SHPT_POINTM  ---  wkbPoint25D
			    
		   ���:  SHPT_MULTIPOINT  --- wkbMultiPoint
		          SHPT_MULTIPOINTZ --- wkbMultiPoint25D
				  SHPT_MULTIPOINTM --- wkbMultiPoint25D

		   ��:    SHPT_ARC  --- wkbLineString
		          SHPT_ARCZ --- wkbLineString25D
				  SHPT_ARCM --- wkbLineString25D
				  
		   �����:SHPT_POLYGON   --- wkbPolygon
		          SHPT_POLYGONZ  --- wkbPolygon25D
				  SHPT_POLYGONM  --- wkbPolygon25D

			���棺SHPT_MULTIPATCH --- wkbMultiPolygon
		    
	*/
	glbBool Initialize(CGlbFileDataSource* dataSource,OGRLayer* layer,OGRDataSource* ogrDataSource);
public:
	glbBool QueryInterface(const glbWChar *riid,void **ppvoid);
public:
	/**
	* @brief ��ȡ���ݼ�������
	*
	* @return �������ݼ�����
	*/
	GlbDatasetTypeEnum    GetType();
	/**
	* @brief ��ȡ���ݼ�������
	*
	* @return �������ݼ�������
	*/
	const glbWChar*       GetName();
	/**
	* @brief ��ȡ���ݼ��ı���
	*
	* @return �������ݼ��ı���
	*/
	const glbWChar*       GetAlias();
	/**
	* @brief ��ȡ���ݼ�����������Դ
	*
	* @return ������������Դ�ӿ�ָ��
	*/
	const IGlbDataSource* GetDataSource();
	/**
	* @brief ��ȡ���ݼ��ĸ����ݼ�
	*
	* @return �и����ݼ������ظ����ݼ��ӿ�ָ�롣û�У�����NULL
	*/
	const IGlbDataset*    GetParentDataset();
	const GlbSubDatasetRoleEnum GetSubRole();
	/**
	* @brief ��ȡ���һ��ִ�еĴ�����Ϣ
	*
	* @return ������Ϣ
	*/
	glbWChar*             GetLastError(){return GlbGetLastError();}
public:
	/**
	* @brief ��ȡ�ռ�������Ϣ
	*
	* @return �ɹ�:���ؿռ�������Ϣ��ʧ��:NULL
	*/
	const glbWChar*       GetSRS();
	/**
	* @brief �������ݼ��ռ�ο�ϵ����
	*
	* @param srs Ҫ���õĿռ�ο�ϵ����
	* @return �ɹ�:true��ʧ��:false
	*/
	glbBool	              AlterSRS(const glbWChar* srs);
	/**
	* @brief ��ȡ���ݼ����
	*
	* @return �������
	*/
	const                 CGlbExtent* GetExtent();
	glbBool               HasZ(){return mpr_hasZ;}
	glbBool               HasM(){return mpr_hasM;}
public:
	glbBool                  AddField(CGlbField* field);
	
	glbBool                  DeleteField(const glbWChar * fname);
	
	const CGlbFields*        GetFields();
	
	const CGlbIndexes*       GetIndexes();
	
	glbBool                  AddIndex(CGlbIndex* index);
	
	glbBool	                 DeleteIndex(const glbWChar* iname);
	
	GlbSpatialIndexInfo*     GetSpatialIndexInfo();
	
	glbBool                  BuildSpatialIndex(GlbSpatialIndexInfo * sidxInfo);
	
	glbBool                  DeleteSpatialIndex();
	
	glbInt32                 GetFeatureCount();
	
	CGlbFeature*             GetFeature(glbInt32 oid);
	
	glbBool                  UpdateFeature(CGlbFeature* feature);
	
	glbBool                  DeleteFeature(glbInt32 oid);
	
	glbBool                  DeleteFeatures(CGlbQueryFilter* queryfilter);
	
	IGlbFeatureCursor*       Query(CGlbQueryFilter* queryfilter);
	
	IGlbFeatureUpdateCursor* Update(CGlbQueryFilter* queryfilter);
	
	IGlbFeatureInsertCursor* Insert();
	
	GlbGeometryTypeEnum      GetGeometryType();
	
	glbBool                  AddGeoField(GlbGeometryClassEnum subgeocls, CGlbField* field);
	
	glbBool                  DeleteGeoField(GlbGeometryClassEnum subgeocls, const glbWChar* fname);
	
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum subgeocls);
	
	glbBool                  GetGeometry(glbInt32 oid,IGlbGeometry** geo,glbInt32 level = 0);
		
private:
	CGlbWString								mpr_name;			///<���ݼ�����
	CGlbWString								mpr_aliasname;		///<���ݼ�����
	CGlbWString								mpr_fullname;		///<���ݼ���·��ȫ��
	CGlbWString								mpr_srs;			///<���ݼ��ռ�����ο���Ϣ
	glbref_ptr<CGlbExtent>					mpr_extent;			///<���ݼ����
	glbref_ptr<CGlbFields>					mpr_fields;			///<���ݼ��ֶ���Ϣ�б�
	glbref_ptr<CGlbIndexes>					mpr_indexes;		///<���ݼ�������Ϣ�б�
	glbref_ptr<CGlbFileDataSource>	        mpr_datasource;		///<���ݼ���������Դ
	GlbDatasetTypeEnum						mpr_datasettype;	///<���ݼ�����
	GlbGeometryTypeEnum						mpr_geotype;		///<���ݼ���������
	OGRLayer*								mpr_ogrlayer;		///<���ݼ�����������
	OGRDataSource*							mpr_ogrdatasource;	///<���ݼ���������������Դ����
	glbBool                                 mpr_hasZ;
	glbBool                                 mpr_hasM;
};

CGlbFields*   GetFieldsFromOrgLayer(OGRLayer* orgLayer);
GLBVARIANT**  ReadFromOrgFeature(OGRLayer* orgLayer,OGRFeature* orgFea,CGlbFields* fields);
IGlbGeometry* GetGeometryFromOGR(OGRGeometry* ogrGeo);
