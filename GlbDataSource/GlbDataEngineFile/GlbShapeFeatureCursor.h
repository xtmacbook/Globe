#pragma once
#include "GlbShapeFeatureClass.h"

class  CGlbShapeFeatureCursor :public IGlbFeatureCursor
{
public:
	CGlbShapeFeatureCursor(void);
	~CGlbShapeFeatureCursor(void);
	glbBool Initialize(CGlbShapeFeatureClass* featureCls,OGRLayer* ogrLayer,glbInt32 startrow,glbInt32 endrow);
public:
	glbInt32                GetCount();
	CGlbFields*             GetFields();
	const IGlbFeatureClass* GetClass();
	glbBool                 Requery();
	glbBool                 MoveNext();
	CGlbFeature*            GetFeature();
	glbWChar*               GetLastError(){ return GlbGetLastError(); };
private:
	glbref_ptr<CGlbShapeFeatureClass>	mpr_featureclass;	///<游标对象所属featureclass
	glbref_ptr<CGlbFields>				mpr_fields;			///<游标对象包含数据集的字段信息列表
	glbref_ptr<CGlbExtent>              mpr_extent;
	OGRLayer*							mpr_ogrlayer;		///<第三方数据集对象
	OGRFeature*							mpr_ogrfeature;		///<第三方要素对象
	glbInt32							mpr_startindex;		///<要查询的页记录的在所有记录里指定起始行
	glbInt32							mpr_endindex;		///<要查询的页记录的在所有记录里指定终止行
	glbInt32							mpr_index;			///<要查询的页记录的当前查询到索引位置
	glbInt32							mpr_count;			///<要查询的页的总行数
};

GlbDataTypeEnum GetFieldType(OGRFieldType fieldType);
