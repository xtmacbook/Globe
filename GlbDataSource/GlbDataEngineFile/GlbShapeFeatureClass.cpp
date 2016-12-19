#pragma warning(disable:4251)
#include "StdAfx.h"
#include "GlbString.h"
#include "GlbWString.h"
#include "GlbPath.h"
#include "GlbShapeFeatureClass.h"
#include "GlbShapeFeatureCursor.h"
#include "GlbFeature.h"
#include "ogr_attrind.h"
#include "ogrshape.h"
#include "ogr_gensql.h"
#include "GlbPoint.h"
#include "GlbLine.h"
#include "GlbPolygon.h"

CGlbShapeFeatureClass::CGlbShapeFeatureClass(void)
{
	mpr_name.clear();
	mpr_aliasname.clear();
	mpr_fullname.clear();
	mpr_srs.clear();
	mpr_extent        = NULL;
	mpr_fields        = NULL;
	mpr_indexes       = NULL;
	mpr_datasource    = NULL;
	mpr_datasettype   = GLB_DATASET_UNKNOWN;
	mpr_geotype       = GLB_GEO_UNKNOWN;
	mpr_ogrlayer      = NULL;
	mpr_ogrdatasource = NULL;
	mpr_hasZ          = false;
	mpr_hasM          = false;
}

CGlbShapeFeatureClass::~CGlbShapeFeatureClass(void)
{
	mpr_extent        = NULL;
	mpr_fields        = NULL;
	mpr_indexes       = NULL;
	mpr_datasource    = NULL;
	if (mpr_ogrdatasource)
	{//ɾ�� shape����Դ.
		delete mpr_ogrdatasource;
		mpr_ogrdatasource = NULL;
	}
	/*
	*   ��Ϊɾ��mpr_ogrdatasource������Դ��ɾ������layer
	*   ���ԣ�
	*        mpr_ogrlayer = NULL�Ϳ���
	*/
	mpr_ogrlayer = NULL;
}
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
glbBool CGlbShapeFeatureClass::Initialize( CGlbFileDataSource* dataSource,OGRLayer* layer,OGRDataSource* ogrDataSource )
{
	if (dataSource == NULL || layer == NULL || ogrDataSource == NULL)
	{
		GlbSetLastError(L"������NULL");
		return false;
	}		
	mpr_datasource    = dataSource;
	mpr_ogrdatasource = ogrDataSource;
	mpr_ogrlayer      = layer;
	// ��ʼ��ȫ��
	CGlbString layerName(mpr_ogrlayer->GetName());
	mpr_name      = layerName.ToWString() + L".shp";
	mpr_aliasname = mpr_name;
	mpr_fullname  = mpr_datasource->GetURL();
	mpr_fullname.append(L"\\");
	mpr_fullname.append(mpr_name);
	mpr_critical.SetName(mpr_name.c_str());
	// ��ʼ��geometry����
	OGRwkbGeometryType ogrGeoType = layer->GetGeomType();
	switch (ogrGeoType)
	{
	case wkbPoint:
	case wkbPoint25D:
	case wkbMultiPoint:		
	case wkbMultiPoint25D:
		 {
			 mpr_geotype     = GLB_GEO_MULTIPOINT;
			 mpr_datasettype = GLB_DATASET_FEATURECLASS_MULTIPOINT;
		 }break;
	case wkbLineString:
	case wkbLinearRing:
	case wkbLineString25D:
		{
			mpr_geotype     = GLB_GEO_MULTILINE;
			mpr_datasettype = GLB_DATASET_FEATURECLASS_MULTILINE;
		}
		break;
	case wkbPolygon:		
	case wkbPolygon25D:		
	case wkbMultiPolygon:
	case wkbMultiPolygon25D:
		{
			mpr_geotype     = GLB_GEO_MULTIPOLYGON;
			mpr_datasettype = GLB_DATASET_FEATURECLASS_MULTIPOLYGON;
		}break;
	default:
		mpr_geotype     = GLB_GEO_UNKNOWN;
		mpr_datasettype = GLB_DATASET_UNKNOWN;
		break;
	}
	switch(layer->GetGeomType())
	{
	case wkbPoint25D:
	case wkbLineString25D:
	case wkbPolygon25D:
	case wkbMultiPoint25D:
	case wkbMultiPolygon25D:
		mpr_hasZ = true;break;
	default:
		mpr_hasZ = false;
	}
	// ��ʼ�����ݼ����
	OGREnvelope* ps = new OGREnvelope();
	OGRErr err      = mpr_ogrlayer->GetExtent(ps);
	if (!err)
	{
		mpr_extent = new CGlbExtent(ps->MinX,ps->MaxX,ps->MinY,ps->MaxY);
	}
	delete ps;
	// ��ʼ���ֶ���Ϣ
	mpr_fields = GetFieldsFromOrgLayer(mpr_ogrlayer);
	//��ʼ������ϵ	
	OGRSpatialReference* spatial = mpr_ogrlayer->GetSpatialRef();
	if (spatial)
	{
		char* wkt;
		OGRErr err = spatial->exportToWkt(&wkt);
		if (err == OGRERR_NONE)
		{
			CGlbString glbwkt(wkt);
			mpr_srs = glbwkt.ToWString();
			VSIFree(wkt);			
		}		
	}
	return true;
}

glbBool CGlbShapeFeatureClass::QueryInterface( const glbWChar *riid,void **ppvoid )
{
	if (riid == NULL || ppvoid == NULL)
	{
		GlbSetLastError(L"������NULL");
		return false;
	}
	CGlbWString tmpiid = riid;
	if(tmpiid == L"IGlbFeatureClass")
	{
		*ppvoid = (IGlbFeatureClass*)this;
		return true;
	}
	else if(tmpiid == L"IGlbGeoDataset")
	{
		*ppvoid = (IGlbGeoDataset*)this;
		return true;
	}
	else if(tmpiid == L"IGlbDataset")
	{
		*ppvoid = (IGlbDataset*)this;
		return true;
	}
	GlbSetLastError(L"�ӿڲ�����");
	return false;
}

GlbDatasetTypeEnum CGlbShapeFeatureClass::GetType()
{
	return mpr_datasettype;
}

const glbWChar* CGlbShapeFeatureClass::GetName()
{
	return mpr_name.c_str();
}

const glbWChar* CGlbShapeFeatureClass::GetAlias()
{
	return mpr_aliasname.c_str();
}

const IGlbDataSource* CGlbShapeFeatureClass::GetDataSource()
{
	if (mpr_datasource == NULL)
	{
		return NULL;
	}
	return dynamic_cast<IGlbDataSource*>(mpr_datasource.get());
}

const IGlbDataset* CGlbShapeFeatureClass::GetParentDataset()
{
	return NULL;
}

const GlbSubDatasetRoleEnum CGlbShapeFeatureClass::GetSubRole()
{
	return GLB_SUBDATASETROLE_UNKNOWN;
}

const CGlbExtent* CGlbShapeFeatureClass::GetExtent()
{
	return mpr_extent.get();
}

const glbWChar* CGlbShapeFeatureClass::GetSRS()
{
	if (mpr_srs == L"")
	{		
		return NULL;
	}
	return mpr_srs.c_str();
}

glbBool CGlbShapeFeatureClass::AlterSRS( const glbWChar* srs )
{
	GlbSetLastError(L"��֧�� �޸�����ϵ");
	return false;
}

glbBool CGlbShapeFeatureClass::AddField( CGlbField* field )
{
	GlbSetLastError(L"��֧�� ����ֶ�");
	return false;
}

glbBool CGlbShapeFeatureClass::DeleteField( const glbWChar * fname )
{
	GlbSetLastError(L"��֧�� ɾ���ֶ�");
	return false;
}

const CGlbFields* CGlbShapeFeatureClass::GetFields()
{
	return mpr_fields.get();
}

const CGlbIndexes* CGlbShapeFeatureClass::GetIndexes()
{
	return mpr_indexes.get();
}

glbBool CGlbShapeFeatureClass::AddIndex( CGlbIndex* index )
{
	GlbSetLastError(L"��֧�� �������");
	return false;
}

glbBool CGlbShapeFeatureClass::DeleteIndex( const glbWChar* iname )
{
	GlbSetLastError(L"��֧�� ɾ������");
	return false;
}

GlbSpatialIndexInfo* CGlbShapeFeatureClass::GetSpatialIndexInfo()
{
	GlbSetLastError(L"��֧�� �ռ�����");
	return NULL;
}

glbBool CGlbShapeFeatureClass::BuildSpatialIndex( GlbSpatialIndexInfo * sidxInfo )
{
	GlbSetLastError(L"��֧�� �ռ�����");
	return false;
}

glbBool CGlbShapeFeatureClass::DeleteSpatialIndex()
{
	GlbSetLastError(L"��֧�� �ռ�����");
	return false;
}
/*
	mpr_ogrlayer �ķ��������̰߳�ȫ��.
*/
glbInt32 CGlbShapeFeatureClass::GetFeatureCount()
{
	if (mpr_ogrlayer == NULL)
	{
		GlbSetLastError(L"û�г�ʼ��");
		return -1;
	}
	
	GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
	return mpr_ogrlayer->GetFeatureCount();
}

CGlbFeature* CGlbShapeFeatureClass::GetFeature( glbInt32 oid )
{
	if (mpr_ogrlayer == NULL)
	{
		GlbSetLastError(L"û�г�ʼ��");
		return NULL;
	}
	
	OGRFeature* fea = NULL;
	{//mpr_ogrlayer �ķ��������̰߳�ȫ��.
		GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		fea = mpr_ogrlayer->GetFeature(oid);
	}
	GLBVARIANT** values = ReadFromOrgFeature(mpr_ogrlayer,fea,mpr_fields.get());
	if(values)
	{
		CGlbFeature* feature = new CGlbFeature(this,(CGlbFields*)this->GetFields());
		if(feature->Init(values,NULL))
		{
			OGRGeometry* ogrGeo = fea->GetGeometryRef();
			IGlbGeometry* geo   = GetGeometryFromOGR(ogrGeo);
			feature->PutGeometry(geo,0,false);
			delete fea;//fea ��Ҫdelete
			return feature;
		}
		else
		{
			for(glbInt32 i=0; i<mpr_fields->GetFieldCount(); i++)
			{
				glbVariantClean(*values[i]);
			}
			delete[] values;
			delete feature;
			delete fea;//fea ��Ҫdelete
		}
	}
	return NULL;
}

glbBool CGlbShapeFeatureClass::UpdateFeature( CGlbFeature* feature )
{
	GlbSetLastError(L"�÷���δʵ��");
	return false;
}

glbBool CGlbShapeFeatureClass::DeleteFeature( glbInt32 oid )
{
	GlbSetLastError(L"�÷���δʵ��");
	return false;
}

glbBool CGlbShapeFeatureClass::DeleteFeatures( CGlbQueryFilter* queryfilter )
{
	GlbSetLastError(L"�÷���δʵ��");
	return false;
}

IGlbFeatureCursor* CGlbShapeFeatureClass::Query( CGlbQueryFilter* queryfilter )
{
	// ��������sql
	CGlbWString sqlwstr;
	CGlbString  sql;
	if(queryfilter)
	{
		sqlwstr = L"select ";
		if(queryfilter->GetPrefixClause())
		{
			CGlbWString prefix = queryfilter->GetPrefixClause();
			prefix.ToUpper();
			glbInt32 pos = prefix.find(L"GLBOID");
			while (pos >= 0)
			{
				prefix = prefix.replace(pos,6,L"FID");
				pos = prefix.find(L"GLBOID");
			}
			sqlwstr.append(prefix);
			sqlwstr.append(L" ");
		}
		if(queryfilter->GetFields())
		{
			sqlwstr.append(queryfilter->GetFields());
		}else{
			sqlwstr.append(L"*");
		}
		sqlwstr.append(L" from ");
		CGlbWString datasetname = mpr_name;
		datasetname = CGlbPath::RemoveExtName(datasetname);
		sqlwstr.append(L"\"");
		sqlwstr.append(datasetname.c_str());
		sqlwstr.append(L"\"");
		sqlwstr.append(L" ");
		CGlbWString wherestr = L"";
		if(queryfilter->GetWhereStr())
		{
			wherestr = L"where ";
			wherestr += queryfilter->GetWhereStr();
		}
		glbInt32 pos = wherestr.find(L"GLBOID");
		while (pos >= 0)
		{
			wherestr = wherestr.replace(pos,6,L"FID");
			pos = wherestr.find(L"GLBOID");
		}
		if(wherestr.length()>0)
			sqlwstr.append(wherestr);
		if(queryfilter->GetPostfixClause())
		{
			sqlwstr.append(L" ");
			CGlbWString postfix = queryfilter->GetPostfixClause();
			postfix.ToUpper();
			glbInt32 pos = postfix.find(L"GLBOID");
			while (pos >= 0)
			{
				postfix = postfix.replace(pos,6,L"FID");
				pos = postfix.find(L"GLBOID");
			}
			sqlwstr.append(postfix);	
		}
		sql = sqlwstr.ToString();		
	}else{
		sqlwstr = L"select * from ";
		CGlbWString datasetname = mpr_name;
		datasetname = CGlbPath::RemoveExtName(datasetname);
		sqlwstr.append(L"\"");
		sqlwstr.append(datasetname.c_str());
		sqlwstr.append(L"\"");
		sql = sqlwstr.ToString();
	}
	OGRPolygon* oPoly = NULL;
	if(queryfilter)
	{
		OGRLinearRing* oRing = NULL;		
		glbInt32 opation;
		IGlbGeometry* geo = queryfilter->GetSpatialFilter(&opation);
		if (geo)
		{
			oRing = new OGRLinearRing();
			oPoly = new OGRPolygon();
			const CGlbExtent* ext = geo->GetExtent();
			glbDouble minx,miny,maxx,maxy;
			ext->GetMin(&minx,&miny);
			ext->GetMax(&maxx,&maxy);
			oRing->addPoint( minx, miny );
			oRing->addPoint( minx, maxy );
			oRing->addPoint( maxx, maxy );
			oRing->addPoint( maxx, miny );
			oRing->addPoint( minx, miny );
			oPoly->addRing( oRing );
		}
	}
	OGRLayer* ogrLayer = NULL;
	{//mpr_ogrlayer �ķ��������̰߳�ȫ��.
		GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		ogrLayer = mpr_ogrdatasource->ExecuteSQL(sql.c_str(),oPoly,NULL);
	}
	if (oPoly)
	{
		delete oPoly;
	}
	if (ogrLayer == NULL)
	{
		GlbSetLastError(L"���������ݼ�������");
		return NULL;
	}
	CGlbShapeFeatureCursor* featureCursor = new CGlbShapeFeatureCursor();
	glbInt32 startrow,endrow;	
	startrow = 0;
	endrow   = INT_MAX-1;
	if(queryfilter)queryfilter->GetStartEndRow(&startrow,&endrow);
	if (!featureCursor->Initialize(this,ogrLayer,startrow,endrow))
		return NULL;
	return featureCursor;
}

IGlbFeatureUpdateCursor* CGlbShapeFeatureClass::Update( CGlbQueryFilter* queryfilter )
{
	GlbSetLastError(L"�÷���δʵ��");
	return false;
}

IGlbFeatureInsertCursor* CGlbShapeFeatureClass::Insert()
{
	GlbSetLastError(L"�÷���δʵ��");
	return false;
}

GlbGeometryTypeEnum CGlbShapeFeatureClass::GetGeometryType()
{
	if (mpr_datasource == NULL)
	{
		GlbSetLastError(L"featureclass��ʼ��ʧ��");
		return GLB_GEO_UNKNOWN;
	}
	return mpr_geotype;
}

glbBool CGlbShapeFeatureClass::AddGeoField( GlbGeometryClassEnum subgeocls, CGlbField* field )
{
	GlbSetLastError(L"�÷���δʵ��");
	return false;
}

glbBool CGlbShapeFeatureClass::DeleteGeoField( GlbGeometryClassEnum subgeocls, const glbWChar* fname )
{
	GlbSetLastError(L"��֧�� �����ֶ�");
	return false;
}

const CGlbGeometryClass* CGlbShapeFeatureClass::GetGeoClass( GlbGeometryClassEnum subgeocls )
{
	GlbSetLastError(L"��֧�� �����ֶ�");
	return NULL;
}

glbBool CGlbShapeFeatureClass::GetGeometry( glbInt32 oid,IGlbGeometry** geo,glbInt32 level)
{
	if (geo == NULL)
	{
		GlbSetLastError(L"����geo��NULL");
		return false;
	}
	if (mpr_ogrlayer == NULL)
	{
		GlbSetLastError(L"û�г�ʼ��");
		return false;
	}
	
	OGRFeature* fea = NULL;
	{//mpr_ogrlayer �ķ��������̰߳�ȫ��.
		GlbScopedLock<GlbCriticalSection> lock(mpr_critical);
		mpr_ogrlayer->GetFeature(oid);
	}
	if(fea == NULL)
	{
		GlbSetLastError(L"Ҫ�ز�����");
		return false;
	}
	
	OGRGeometry* ogrGeo = fea->GetGeometryRef();
	if (ogrGeo == NULL)
	{
		*geo = NULL;
		delete fea;//fea ��Ҫɾ��
		return true;
	}
	*geo = GetGeometryFromOGR(ogrGeo);
	delete fea;//fea ��Ҫɾ��
	return true;
}

CGlbFields*  GetFieldsFromOrgLayer(OGRLayer* orgLayer)
{
	OGRFeatureDefn* featureDef = orgLayer->GetLayerDefn();
	CGlbFields*     fields     = new CGlbFields();
	// oid
	CGlbField* fieldOid = new CGlbField();
	fieldOid->SetType(GLB_DATATYPE_INT32);
	fieldOid->SetName(L"GLBOID");
	fieldOid->SetAlias(L"GLBOID");
	fieldOid->SetRequired(true);
	fieldOid->SetCanNull(false);
	fields->AddField(fieldOid);
	// geo
	CGlbField* fieldGeo = new CGlbField();
	fieldGeo->SetType(GLB_DATATYPE_GEOMETRY);
	fieldGeo->SetName(L"GLBGEO");
	fieldGeo->SetAlias(L"GLBGEO");
	fieldGeo->SetRequired(true);
	fieldGeo->SetCanNull(true);
	
	switch (orgLayer->GetGeomType())
	{
	case wkbPoint:
	case wkbPoint25D:
	case wkbMultiPoint:
	case wkbMultiPoint25D:
		fieldGeo->SetGeoType(GLB_GEO_MULTIPOINT);
		break;
	case wkbLineString:
	case wkbLinearRing:
	case wkbLineString25D:	
	case  wkbMultiLineString:
		fieldGeo->SetGeoType(GLB_GEO_MULTILINE);
		break;
	case wkbPolygon:		
	case wkbPolygon25D:
	case wkbMultiPolygon:
	case wkbMultiPolygon25D:
		fieldGeo->SetGeoType(GLB_GEO_MULTIPOLYGON);
		break;
	}
	fields->AddField(fieldGeo);

	glbInt32 fieldCount = featureDef->GetFieldCount();
	for(glbInt32 i = 0; i < fieldCount; i++)
	{
		OGRFieldDefn* fieldDef = featureDef->GetFieldDefn(i);
		CGlbField*    field    = new CGlbField();
		// ��ȡ����
		switch(fieldDef->GetType())
		{
		case OFTInteger:
			field->SetType(GLB_DATATYPE_INT32);
			break;
		case OFTReal:
			field->SetType(GLB_DATATYPE_DOUBLE);
			break;
		case OFTString:
			field->SetType(GLB_DATATYPE_STRING);
			break;
		case OFTDate:
		case OFTDateTime:
			field->SetType(GLB_DATATYPE_DATETIME);
			break;
		}
		
		/*
		   �ֶ����ͱ���
		   �п�������????
		*/
		CGlbWString fnameWide = L"";
		/*CPLString   osEncoding = "";
		OGRShapeLayer* shpLayer = dynamic_cast<OGRShapeLayer*>(orgLayer);
		if(shpLayer)
			osEncoding = shpLayer->GetEncoding();
		else{
			OGRGenSQLResultsLayer* sqlLayer = dynamic_cast<OGRGenSQLResultsLayer*>(orgLayer);
			if(sqlLayer)osEncoding = sqlLayer->GetEncoding();
		}
		
		if(osEncoding[0] != '\0')
		{
			glbWChar* wcharStr = UTF82WChar((glbChar*)fieldDef->GetNameRef());
			if(wcharStr)
			{
				fnameWide   = wcharStr;
				free(wcharStr);
			}						
		}else*/{
			CGlbString  fname(fieldDef->GetNameRef());
			fnameWide = fname.ToWString();
		}
		field->SetName(fnameWide.c_str());
		field->SetAlias(fnameWide.c_str());
		// ����
		field->SetLength(fieldDef->GetWidth());
		// Ĭ��ֵ
		const OGRField* fieldVal = fieldDef->GetDefaultRef();
		switch (field->GetType())
		{
		case GLB_DATATYPE_INT32:
			{
				CGlbWString df = CGlbWString::FormatString
					             (L"%ld",fieldVal->Integer);
				field->SetDefault(df.c_str());
			}break;
		case GLB_DATATYPE_DOUBLE:
			{
				CGlbWString df = CGlbWString::FormatString
					             (L"%lf",fieldVal->Real);
				field->SetDefault(df.c_str());
			}break;			
		case GLB_DATATYPE_STRING:
			{
				char* CStr = fieldVal->String;
				if (CStr)
				{//�п�������
					CGlbString Str(CStr);
					field->SetDefault(Str.ToWString().c_str());
				}
			}
			break;
		case GLB_DATATYPE_DATETIME:
			break;
		default:
			break;
		}
		fields->AddField(field);
	}
	return fields;
}
GLBVARIANT** ReadFromOrgFeature(OGRLayer* orgLayer,OGRFeature* orgFea,CGlbFields* fields)
{
	if (orgFea == NULL)return NULL;
	// ��ʼ��feature�ֶ�ֵ
	OGRFeatureDefn* featureDef = orgLayer->GetLayerDefn();
	glbInt32        count      = featureDef->GetFieldCount();
	GLBVARIANT**    values     = new GLBVARIANT*[count+2];

	// ����oid�ֶ�ֵ
	values[0]         = new GLBVARIANT();
	values[0]->isnull = false;
	values[0]->vt     = GLB_DATATYPE_INT32;
	values[0]->lVal   = (glbInt32)orgFea->GetFID();
	values[0]->isarray= false;
	// ����geo�ֶ�ֵ
	values[1]         = new GLBVARIANT();
	values[1]->isnull = true;
	values[1]->vt     = GLB_DATATYPE_GEOMETRY;
	values[1]->isarray= false;		
	for (glbInt32 i = 0; i < count; i++)
	{
		GlbDataTypeEnum ogrFieldType = fields->GetField(i+2)->GetType();
		if (ogrFieldType == GLB_DATATYPE_INT32)
		{
			values[i+2]         = new GLBVARIANT();
			values[i+2]->isnull = false;
			values[i+2]->vt     = GLB_DATATYPE_INT32;
			values[i+2]->lVal   = orgFea->GetFieldAsInteger(i);
			values[i+2]->isarray= false;
		}
		else if (ogrFieldType == GLB_DATATYPE_DOUBLE)
		{			
			values[i+2]         = new GLBVARIANT();
			values[i+2]->isnull = false;
			values[i+2]->vt     =  GLB_DATATYPE_DOUBLE;
			values[i+2]->dblVal = orgFea->GetFieldAsDouble(i);
			values[i+2]->isarray= false;
		}
		else if (ogrFieldType == GLB_DATATYPE_STRING)
		{
			CGlbWString wstr       = L"";
			const char* charstr    = orgFea->GetFieldAsString(i);
			/*CPLString   osEncoding = "";
			OGRShapeLayer* shpLayer = dynamic_cast<OGRShapeLayer*>(orgLayer);
			if(shpLayer)
				osEncoding = shpLayer->GetEncoding();
			else{
				OGRGenSQLResultsLayer* sqlLayer = dynamic_cast<OGRGenSQLResultsLayer*>(orgLayer);
				if(sqlLayer)osEncoding = sqlLayer->GetEncoding();
			}
			
			if(charstr && osEncoding[0] != '\0')
			{						
				glbWChar* wcharStr = UTF82WChar((glbChar*)charstr);
				if(wcharStr)
				{
					wstr   = wcharStr;
					free(wcharStr);
				}						
			}else if(charstr)*/{
				glbWChar* wcharStr = GlbChar2WChar((glbChar*)charstr);
				if(wcharStr)
				{
					wstr   = wcharStr;
					free(wcharStr);
				}
			}
			glbWChar* _wstr = NULL;
			if(wstr.length()>0)
			{
				_wstr = (glbWChar*)malloc(sizeof(glbWChar)*(wstr.length()+1));
				memcpy(_wstr,wstr.c_str(),sizeof(glbWChar)*wstr.length());
				_wstr[wstr.length()] = L'\0';
			}
			values[i+2]                  = new GLBVARIANT;
			values[i+2]->isnull          = true;
			if(_wstr)values[i+2]->isnull = false;
			values[i+2]->vt              =  GLB_DATATYPE_STRING;
			values[i+2]->strVal          = _wstr;
			values[i+2]->isarray= false;
		}else if(ogrFieldType == GLB_DATATYPE_DATETIME){
			int pnYear,pnMonth,pnDay;
            int pnHour,pnMinute,pnSecond;
            int pnTZFlag;
			orgFea->GetFieldAsDateTime(i,&pnYear,&pnMonth,&pnDay,
			                             &pnHour,&pnMinute,&pnSecond,&pnTZFlag);
			values[i+2]         = new GLBVARIANT();
			values[i+2]->isnull = false;
			values[i+2]->vt     =  GLB_DATATYPE_DATETIME;
			values[i+2]->date   = pnYear*pnMonth*pnDay*24*60*60
				                  + pnHour*60*60+pnMinute*60
								  + pnSecond;
			values[i+2]->isarray= false;
		}		
	}	
	return values;
}
IGlbGeometry* GetGeometryFromOGR(OGRGeometry* ogrGeo)
{
	if (ogrGeo == NULL)
		return NULL;
	switch (/*wkbFlatten(*/ogrGeo->getGeometryType()/*)*/)
	{
	case wkbPoint:
		{
			CGlbMultiPoint *mpt = new CGlbMultiPoint(2);			
			mpt->AddPoint(((OGRPoint*)ogrGeo)->getX(),((OGRPoint*)ogrGeo)->getY());
			return (IGlbGeometry*)mpt;
		}
	case wkbPoint25D:
		{
			CGlbMultiPoint *mpt = new CGlbMultiPoint(3);			
			mpt->AddPoint(((OGRPoint*)ogrGeo)->getX(),
			              ((OGRPoint*)ogrGeo)->getY(),
						  ((OGRPoint*)ogrGeo)->getZ());
			return (IGlbGeometry*)mpt;
		}
	case wkbLineString:
	case wkbLinearRing:
		{
			OGRLineString* poLS = (OGRLineString*) ogrGeo;
			CGlbLine* line = new CGlbLine();
			glbInt32  pcnt = poLS->getNumPoints();
			if(pcnt >0)
			{
				OGRRawPoint * ogrPoints = new OGRRawPoint[pcnt];
				poLS->getPoints(ogrPoints);
				line->InsertPoints(0,(glbDouble*)ogrPoints,pcnt);
				delete [] ogrPoints;
			}
			CGlbMultiLine* mline = new CGlbMultiLine();
			mline->AddLine(line);
			return (IGlbGeometry*)mline;
		}
	case wkbLineString25D:
		{
			OGRLineString* poLS = (OGRLineString*) ogrGeo;
			CGlbLine* line = new CGlbLine(3);
			glbInt32  pcnt = poLS->getNumPoints();
			if(pcnt >0)
			{
				OGRRawPoint* ogrPoints = new OGRRawPoint[pcnt];
				glbDouble*      pz     = new glbDouble[pcnt];  
				glbDouble*      coords = new glbDouble[pcnt*3];
				poLS->getPoints(ogrPoints,pz);
				for(glbInt32 i=0;i<pcnt;i++)
				{
					coords[i*3]   = ogrPoints[i].x;
					coords[i*3+1] = ogrPoints[i].y;
					coords[i*3+2] = pz[i];
				}
				line->InsertPoints(0,coords,pcnt);
				delete [] ogrPoints;
				delete [] pz;
				delete [] coords;
			}
			CGlbMultiLine* mline = new CGlbMultiLine(3);
			mline->AddLine(line);
			return (IGlbGeometry*)mline;			
		}break;
	case wkbPolygon:
		{
			CGlbMultiPolygon*        mulPolygon=NULL;
			mulPolygon     = new CGlbMultiPolygon(2);

			OGRPolygon *poPoly = (OGRPolygon *)ogrGeo;
			CGlbPolygon* poly  = new CGlbPolygon(2);
			CGlbMultiLine* mline = (CGlbMultiLine*)GetGeometryFromOGR(poPoly->getExteriorRing());
			if(mline && mline->GetCount()>0)
				poly->SetExtRing((CGlbLine*)mline->GetLine(0));
			if(mline)delete mline;
			mline = NULL;

			for(int i=0;i<poPoly->getNumInteriorRings();i++)
			{
				mline = (CGlbMultiLine*)GetGeometryFromOGR(poPoly->getInteriorRing(i));
				if(mline && mline->GetCount()>0)
					poly->AddInRing((CGlbLine*)mline->GetLine(0));
				if(mline)delete mline;
				mline = NULL;
			}

			mulPolygon->AddPolygon(poly);
			return (IGlbGeometry*)mulPolygon;
		}break;
	case wkbPolygon25D:
		{			
			CGlbPolygon* poly  = new CGlbPolygon(3);
			OGRPolygon* poPoly = (OGRPolygon*) ogrGeo;
			CGlbMultiLine* mline = (CGlbMultiLine*)GetGeometryFromOGR(poPoly->getExteriorRing());
			if(mline && mline->GetCount()>0)
				poly->SetExtRing((CGlbLine*)mline->GetLine(0));
			if(mline)delete mline;
			mline = NULL;

			for(glbInt32 i=0;i<poPoly->getNumInteriorRings();i++)
			{
				mline = (CGlbMultiLine*)GetGeometryFromOGR(poPoly->getInteriorRing(i));
				if(mline && mline->GetCount()>0)
					poly->AddInRing((CGlbLine*)mline->GetLine(0));
				if(mline)delete mline;
				mline = NULL;
			}
			CGlbMultiPolygon*        mulPolygon=NULL;
			mulPolygon     = new CGlbMultiPolygon(3);
			mulPolygon->AddPolygon(poly);
			return (IGlbGeometry*)mulPolygon;			
		}break;
	case wkbMultiPoint:
		{
			CGlbMultiPoint*        mPoint     = new CGlbMultiPoint();
			OGRGeometryCollection* poGeomColl = (OGRGeometryCollection*) ogrGeo;
			glbInt32               pcnt       = poGeomColl->getNumGeometries();
			if(pcnt >0)
			{
				glbDouble*             coords     = new glbDouble[pcnt*2];
				for(glbInt32 i=0;i<pcnt; i++)
				{
					OGRPoint* orgpt = (OGRPoint*)poGeomColl->getGeometryRef(i);
					coords[i*2]   = orgpt->getX();
					coords[i*2+1] = orgpt->getY();
				}
				mPoint->AddPoints(coords,pcnt);
				delete []coords;
			}
			return (IGlbGeometry*)mPoint;
		}break;
	case wkbMultiPoint25D:
		{
			CGlbMultiPoint*        mPoint     = new CGlbMultiPoint(3);
			OGRGeometryCollection* poGeomColl = (OGRGeometryCollection*) ogrGeo;
			glbInt32               pcnt       = poGeomColl->getNumGeometries();
			if(pcnt >0)
			{
				glbDouble*             coords     = new glbDouble[pcnt*3];
				for(glbInt32 i=0;i<pcnt; i++)
				{
					OGRPoint* orgpt = (OGRPoint*)poGeomColl->getGeometryRef(i);
					coords[i*3]   = orgpt->getX();
					coords[i*3+1] = orgpt->getY();
					coords[i*3+2] = orgpt->getZ();
				}
				mPoint->AddPoints(coords,pcnt);
				delete []coords;
			}
			return (IGlbGeometry*)mPoint;
		}break;
	case wkbMultiLineString:
		{
			CGlbMultiLine *multLine = NULL;
			OGRGeometryCollection *poGeomColl = (OGRGeometryCollection*)ogrGeo;
			glbInt32 numGeo = poGeomColl->getNumGeometries();
			if(numGeo > 0)
			{
				multLine = new CGlbMultiLine;
				for(glbInt32 iGeoIdx = 0;iGeoIdx < numGeo;iGeoIdx++)
				{
					CGlbMultiLine *mlines = (CGlbMultiLine *)GetGeometryFromOGR(poGeomColl->getGeometryRef(iGeoIdx));
					if(mlines && mlines->GetCount() > 0)
						multLine->AddLine((CGlbLine*)mlines->GetLine(0));
					if(mlines)delete mlines;
					mlines = NULL;
				}
			}
			return (IGlbGeometry*)multLine;
		}break;
	case wkbMultiPolygon:
		{
			CGlbMultiPolygon*        mulPolygon=NULL;
			OGRGeometryCollection* poGeomColl = (OGRGeometryCollection*) ogrGeo;
			glbInt32               NumGeo       = poGeomColl->getNumGeometries();
			if(NumGeo >0)
			{
				mulPolygon     = new CGlbMultiPolygon(2);
				for(glbInt32 iGeoIdx=0;iGeoIdx<NumGeo; iGeoIdx++)
				{
					OGRPolygon *poPoly = (OGRPolygon *)poGeomColl->getGeometryRef(iGeoIdx);
					
					CGlbPolygon* poly  = new CGlbPolygon(2);
					CGlbMultiLine* mline = (CGlbMultiLine*)GetGeometryFromOGR(poPoly->getExteriorRing());
					if(mline && mline->GetCount()>0)
						poly->SetExtRing((CGlbLine*)mline->GetLine(0));
					if(mline)delete mline;
					mline = NULL;
					
					for(int i=0;i<poPoly->getNumInteriorRings();i++)
					{
						mline = (CGlbMultiLine*)GetGeometryFromOGR(poPoly->getInteriorRing(i));
						if(mline && mline->GetCount()>0)
							poly->AddInRing((CGlbLine*)mline->GetLine(0));
						if(mline)delete mline;
						mline = NULL;
					}

					mulPolygon->AddPolygon(poly);
				}
			}
			return (IGlbGeometry*)mulPolygon;
		}break;
	case wkbMultiPolygon25D:
		{
			CGlbMultiPolygon*        mulPolygon=NULL;
			OGRGeometryCollection* poGeomColl = (OGRGeometryCollection*) ogrGeo;
			glbInt32               NumGeo       = poGeomColl->getNumGeometries();
			if(NumGeo >0)
			{
				mulPolygon     = new CGlbMultiPolygon(3);
				for(glbInt32 iGeoIdx=0;iGeoIdx<NumGeo; iGeoIdx++)
				{
					OGRPolygon *poPoly = (OGRPolygon *)poGeomColl->getGeometryRef(iGeoIdx);

					CGlbPolygon* poly  = new CGlbPolygon(3);
					CGlbMultiLine* mline = (CGlbMultiLine*)GetGeometryFromOGR(poPoly->getExteriorRing());
					if(mline && mline->GetCount()>0)
						poly->SetExtRing((CGlbLine*)mline->GetLine(0));
					if(mline)delete mline;
					mline = NULL;

					for(int i=0;i<poPoly->getNumInteriorRings();i++)
					{
						mline = (CGlbMultiLine*)GetGeometryFromOGR(poPoly->getInteriorRing(i));
						if(mline && mline->GetCount()>0)
							poly->AddInRing((CGlbLine*)mline->GetLine(0));
						if(mline)delete mline;
						mline = NULL;
					}

					mulPolygon->AddPolygon(poly);
				}
			}
			return (IGlbGeometry*)mulPolygon;
		}break;
	default:
		break;
	}
	return NULL;
}


