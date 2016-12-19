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
	glbref_ptr<CGlbShapeFeatureClass>	mpr_featureclass;	///<�α��������featureclass
	glbref_ptr<CGlbFields>				mpr_fields;			///<�α����������ݼ����ֶ���Ϣ�б�
	glbref_ptr<CGlbExtent>              mpr_extent;
	OGRLayer*							mpr_ogrlayer;		///<���������ݼ�����
	OGRFeature*							mpr_ogrfeature;		///<������Ҫ�ض���
	glbInt32							mpr_startindex;		///<Ҫ��ѯ��ҳ��¼�������м�¼��ָ����ʼ��
	glbInt32							mpr_endindex;		///<Ҫ��ѯ��ҳ��¼�������м�¼��ָ����ֹ��
	glbInt32							mpr_index;			///<Ҫ��ѯ��ҳ��¼�ĵ�ǰ��ѯ������λ��
	glbInt32							mpr_count;			///<Ҫ��ѯ��ҳ��������
};

GlbDataTypeEnum GetFieldType(OGRFieldType fieldType);
