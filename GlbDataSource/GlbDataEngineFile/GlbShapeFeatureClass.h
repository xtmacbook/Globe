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
	   shape 文件几何类型：
	       点:    SHPT_POINT   ---  wkbPoint
		          SHPT_POINTZ  ---  wkbPoint25D
			      SHPT_POINTM  ---  wkbPoint25D
			    
		   多点:  SHPT_MULTIPOINT  --- wkbMultiPoint
		          SHPT_MULTIPOINTZ --- wkbMultiPoint25D
				  SHPT_MULTIPOINTM --- wkbMultiPoint25D

		   线:    SHPT_ARC  --- wkbLineString
		          SHPT_ARCZ --- wkbLineString25D
				  SHPT_ARCM --- wkbLineString25D
				  
		   多边形:SHPT_POLYGON   --- wkbPolygon
		          SHPT_POLYGONZ  --- wkbPolygon25D
				  SHPT_POLYGONM  --- wkbPolygon25D

			多面：SHPT_MULTIPATCH --- wkbMultiPolygon
		    
	*/
	glbBool Initialize(CGlbFileDataSource* dataSource,OGRLayer* layer,OGRDataSource* ogrDataSource);
public:
	glbBool QueryInterface(const glbWChar *riid,void **ppvoid);
public:
	/**
	* @brief 获取数据集的类型
	*
	* @return 返回数据集类型
	*/
	GlbDatasetTypeEnum    GetType();
	/**
	* @brief 获取数据集的名字
	*
	* @return 返回数据集的名字
	*/
	const glbWChar*       GetName();
	/**
	* @brief 获取数据集的别名
	*
	* @return 返回数据集的别名
	*/
	const glbWChar*       GetAlias();
	/**
	* @brief 获取数据集所属的数据源
	*
	* @return 返回所属数据源接口指针
	*/
	const IGlbDataSource* GetDataSource();
	/**
	* @brief 获取数据集的父数据集
	*
	* @return 有父数据集，返回父数据集接口指针。没有，返回NULL
	*/
	const IGlbDataset*    GetParentDataset();
	const GlbSubDatasetRoleEnum GetSubRole();
	/**
	* @brief 获取最近一次执行的错误信息
	*
	* @return 错误信息
	*/
	glbWChar*             GetLastError(){return GlbGetLastError();}
public:
	/**
	* @brief 获取空间索引信息
	*
	* @return 成功:返回空间索引信息，失败:NULL
	*/
	const glbWChar*       GetSRS();
	/**
	* @brief 设置数据集空间参考系坐标
	*
	* @param srs 要设置的空间参考系坐标
	* @return 成功:true，失败:false
	*/
	glbBool	              AlterSRS(const glbWChar* srs);
	/**
	* @brief 获取数据集外包
	*
	* @return 返回外包
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
	CGlbWString								mpr_name;			///<数据集名字
	CGlbWString								mpr_aliasname;		///<数据集别名
	CGlbWString								mpr_fullname;		///<数据集带路径全名
	CGlbWString								mpr_srs;			///<数据集空间坐标参考信息
	glbref_ptr<CGlbExtent>					mpr_extent;			///<数据集外包
	glbref_ptr<CGlbFields>					mpr_fields;			///<数据集字段信息列表
	glbref_ptr<CGlbIndexes>					mpr_indexes;		///<数据集索引信息列表
	glbref_ptr<CGlbFileDataSource>	        mpr_datasource;		///<数据集所属数据源
	GlbDatasetTypeEnum						mpr_datasettype;	///<数据集类型
	GlbGeometryTypeEnum						mpr_geotype;		///<数据集几何类型
	OGRLayer*								mpr_ogrlayer;		///<数据集第三方对象
	OGRDataSource*							mpr_ogrdatasource;	///<数据集第三方所属数据源对象
	glbBool                                 mpr_hasZ;
	glbBool                                 mpr_hasM;
};

CGlbFields*   GetFieldsFromOrgLayer(OGRLayer* orgLayer);
GLBVARIANT**  ReadFromOrgFeature(OGRLayer* orgLayer,OGRFeature* orgFea,CGlbFields* fields);
IGlbGeometry* GetGeometryFromOGR(OGRGeometry* ogrGeo);
