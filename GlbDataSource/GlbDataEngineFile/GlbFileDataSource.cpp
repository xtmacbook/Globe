#pragma warning(disable:4251)
#include "StdAfx.h"
#include "GlbFileDataSource.h"
#include "GlbFileTerrainDataset.h"
#include "GlbFileRasterDataset.h"
#include "GlbShapeFeatureClass.h"
#include "GlbPath.h"
#include "GlbString.h"
#include "GlbWString.h"
#include "GlbSQLitePyramid.h"
#include <Shlwapi.h>
#include "vrtdataset.h"

CGlbFileDataSource::CGlbFileDataSource(void)
{
}


CGlbFileDataSource::~CGlbFileDataSource(void)
{
}
glbBool CGlbFileDataSource::Initialize(const glbWChar* url)
{
	if(NULL == url)
	{
		GlbSetLastError(L"Ŀ¼·����NULL");
		return false;
	}
	if (PathIsDirectory(url) == FALSE)
	{
		GlbSetLastError(L"����Ŀ¼·��");
		return false;
	}
	mpr_url = url;
	glbInt32 lidx = mpr_url.rfind('\\',-1);
	if(lidx != mpr_url.length()-1)
		mpr_url += L"\\";
	OpenFiles();
	return true;
}
IGlbRasterDataset* CGlbFileDataSource::GetRasterDataset( const glbWChar * datasetName)
{
	IGlbDataset* rd = GetDatasetByName(datasetName);
	if (rd != NULL && rd->GetType() != GLB_DATASET_RASTER)
	{
		GlbSetLastError(L"����դ�����ݼ�");
		return NULL;
	}
	return (IGlbRasterDataset*)rd;	
}
IGlbTerrainDataset* CGlbFileDataSource::GetTerrainDataset(const glbWChar* terrainDatasetName)
{
	IGlbDataset* rd = GetDatasetByName(terrainDatasetName);
	if (rd != NULL && rd->GetType() != GLB_DATASET_TERRAIN)
	{
		GlbSetLastError(L"���ǵ������ݼ�");
		return NULL;
	}
	return dynamic_cast<IGlbTerrainDataset*>(rd);
}
IGlbRasterDataset*  CGlbFileDataSource::CreateRasterDataset(
	const glbWChar              *name,
	const glbWChar              *alias,
	glbInt32                    nBands,
	GlbPixelTypeEnum            pixelType,
	glbInt32                    columns,
	glbInt32                    rows,
	glbDouble                   orgX    /*=0*/,
	glbDouble                   orgY    /*=0*/,
	glbDouble                   rsX     /*=1*/,
	glbDouble                   rsY     /*=1*/,
	const glbWChar*             format  /*=L"GTiff"*/,
	const glbWChar*             SRS     /*=NULL*/,
	const GlbRasterStorageDef *storeDef/*=NULL*/)
{
	if (NULL == name)
	{
		GlbSetLastError(L"����name��NULL");
		return NULL;
	}

	CGlbWString fullname  = mpr_url + name;
	CGlbWString strformat = format;
	GDALDriverH pDriver   = GDALGetDriverByName(strformat.ToString().c_str());//("Bmp");
	if (pDriver == NULL)
	{
		GlbSetLastError(L"��֧�ֲ���formatָ�����ļ�����");
		return NULL;
	}

	//�������ݼ�
	VRTDataset *pDst = (VRTDataset *) VRTCreate(columns,rows);
	if (pDst == NULL)
	{
		CGlbString err = CPLGetLastErrorMsg();
		GlbSetLastError(err.ToWString().c_str());
		return NULL;
	}

	//����������Ϣ
	if (SRS != NULL)
	{
		CGlbWString strsrs = SRS;
		if(GDALSetProjection(pDst, strsrs.ToString().c_str())!= CE_None)
		{
			CGlbString err = CPLGetLastErrorMsg();
			GlbSetLastError(err.ToWString().c_str());
			GDALClose(pDst);
			return NULL;
		}
	}

	//���������Ϣ
	double dGeoTrans[6] = {0};
	dGeoTrans [0] = orgX;      //��СX����
	dGeoTrans [3] = orgY;      //���Y����
	dGeoTrans [1] = fabs(rsX); // ����ֱ���
	dGeoTrans [5] = -fabs(rsY);// ����ֱ��� topleft is origin������Ӧ��ȡ��
	if(GDALSetGeoTransform(pDst, dGeoTrans)!= CE_None)
	{
		CGlbString err = CPLGetLastErrorMsg();
		GlbSetLastError(err.ToWString().c_str());
		GDALClose(pDst);
		return NULL;
	}
	glbChar **papszOptions = NULL;
	//���������ѹ�������С��pixelLayout
	if(storeDef != NULL)
	{
		if(storeDef->compress == GLB_COMPRESSTYPE_ZLIB)
			papszOptions = CSLSetNameValue(papszOptions,"COMPRESS","DEFLATE");
		else if(storeDef->compress == GLB_COMPRESSTYPE_JPEG)
			papszOptions = CSLSetNameValue(papszOptions,"COMPRESS","JPEG");
		else if(storeDef->compress == GLB_COMPRESSTYPE_LZW)
			papszOptions = CSLSetNameValue(papszOptions,"COMPRESS","LZW");

		papszOptions         = CSLSetNameValue(papszOptions, "TILED", "YES");
		CGlbString strBlockX = CGlbString::FormatString("%d", storeDef->blockSizeX);
		papszOptions         = CSLSetNameValue(papszOptions,"BLOCKXSIZE", strBlockX.c_str());
		CGlbString strBlockY = CGlbString::FormatString("%d", storeDef->blockSizeY);
		papszOptions         = CSLSetNameValue(papszOptions,"BLOCKYSIZE", strBlockY.c_str());

		if (storeDef->pixelLayout == GLB_PIXELLAYOUT_BSQ)
		{
			papszOptions = CSLSetNameValue(papszOptions,"INTERLEAVE","BAND");
		}
		else if (storeDef->pixelLayout == GLB_PIXELLAYOUT_BIP)
		{
			papszOptions = CSLSetNameValue(papszOptions,"INTERLEAVE","PIXEL");
		}
		else if (storeDef->pixelLayout == GLB_PIXELLAYOUT_BIL)
		{
			papszOptions = CSLSetNameValue(papszOptions,"INTERLEAVE","LINE");
		}
		for (glbInt32 iBand = 0;iBand<nBands;iBand++)
		{
			pDst->AddBand( GlbPixelTypeToGDAL(pixelType), NULL );
			VRTSourcedRasterBand *poVRTBand = (VRTSourcedRasterBand *) pDst->GetRasterBand(iBand+1);
			if (storeDef->hasNodata)
				poVRTBand->SetNoDataValue(storeDef->nodata);//������Чֵ			
		}
	}

	GDALDatasetH poDataset = GDALCreateCopy(pDriver,fullname.ToString().c_str(), pDst, true, papszOptions,NULL,NULL);
	GDALClose(pDst);
	CSLDestroy(papszOptions);
	if (poDataset == NULL)
	{
		CGlbString err = CPLGetLastErrorMsg();
		GlbSetLastError(err.ToWString().c_str());
		return NULL;
	}
	//����һ��mask����
	if(((GDALDataset*)poDataset)->GetRasterBand(1)->CreateMaskBand(GMF_PER_DATASET)!= CE_None)
	{
		CGlbString err = CPLGetLastErrorMsg();
		GlbSetLastError(err.ToWString().c_str());
		GDALClose(poDataset);
		CGlbPath::DeleteFile(fullname.c_str());
		return NULL;
	}
	GDALFlushCache(poDataset);
	CGlbFileRasterDataset* frd = new CGlbFileRasterDataset();
	if(!frd->Initialize(this, mpr_url.c_str(),name, alias, poDataset, NULL))
	{
		delete frd;
		return NULL;
	}
	//���ӵ�map��
	mpr_datasets[name] = 0;
	return (IGlbRasterDataset*)frd;
}
IGlbTerrainDataset* CGlbFileDataSource::CreateTerrainDataset(
	const glbWChar* name,
	const glbWChar* alias,
	bool                   isGlobe/*=true*/,
	glbDouble              lzts   /*=36*/,
	const glbWChar*        SRS    /*=NULL*/,
	GlbCompressTypeEnum domCompress/*=GLB_COMPRESSTYPE_JPEG*/,
    GlbCompressTypeEnum demCompress/*=GLB_COMPRESSTYPE_ZLIB*/)
{
	if (NULL == name)
	{
		GlbSetLastError(L"����name��NULL");
		return NULL;
	}
	if (domCompress!=GLB_COMPRESSTYPE_JPEG &&
		domCompress!=GLB_COMPRESSTYPE_ZLIB  &&
		domCompress!=GLB_COMPRESSTYPE_NO)
	{
		GlbSetLastError(L"Ӱ������ֻ֧��jpg��zipѹ���Ͳ�ѹ��");
		return NULL;
	}
	if (demCompress!=GLB_COMPRESSTYPE_ZLIB&&
		demCompress!=GLB_COMPRESSTYPE_NO)
	{
		GlbSetLastError(L"�߳�����ֻ֧��zipѹ���Ͳ�ѹ��");
		return NULL;
	}
	CGlbWString    fullname = mpr_url + name;
	CGlbTedSQLite* ts       = new CGlbTedSQLite();
	ts->mpr_isGlobe         = isGlobe;
	ts->mpr_lzts            = lzts;
	ts->mpr_domCompress     = domCompress;
	ts->mpr_demCompress     = demCompress;
	if (!isGlobe && SRS != NULL)
	{
		ts->mpr_srs = SRS;
	}
	else//WGS84
	{
		//L"GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433],AUTHORITY[\"EPSG\",4326]]"
		ts->mpr_srs = L"GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223560493,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0],UNIT[\"degree\",0.0174532925199433],AUTHORITY[\"EPSG\",\"4326\"]]";
	}
	if(!ts->Create(fullname.c_str()))
	{
		delete ts;
		return NULL;
	}
	CGlbFileTerrainDataset* ted = new CGlbFileTerrainDataset();
	if(!ted->Initialize(this, name, alias, ts))
	{
		delete ted;
		return NULL;
	}
	//���ӵ�map��
	mpr_datasets[name] = 0;
	return ted;
}
////IGlbDataSource
/*
	ʶ��.shp shape�ļ�
	    դ���ļ�:gtiff,jpeg,bmp,gif,png,
*/
glbInt32 CGlbFileDataSource::GetCount()
{
	return mpr_datasets.size();
}
void CGlbFileDataSource::OpenFiles()
{
	CGlbFindFile* findfile = CGlbFindFile::CreateFind(mpr_url.c_str(), NULL);
	if(findfile == NULL)return ;

	//shp�ļ�����
	OGRSFDriverRegistrar *ogrDriverRegister = OGRSFDriverRegistrar::GetRegistrar();
	OGRSFDriver          *poDriver          = ogrDriverRegister->GetDriverByName("ESRI Shapefile");

	//��ȡ�����֣�����·����
	CGlbWString filename = findfile->GetNext();
	while(filename != L"")
	{
		CGlbWString exetentname = CGlbPath::GetExtentName(filename);
		exetentname.ToLower();
		if(exetentname == L"pyd")
		{
			CGlbWString  tempname  = mpr_url + CGlbPath::RemoveExtName(filename);
			GDALDatasetH poDataset = GDALOpen(tempname.ToString().c_str(), GA_ReadOnly);
			if (poDataset != NULL)
				GDALClose(poDataset);			
			else
				mpr_datasets[filename] = 0;			
		}
		else if(exetentname == L"ted")
		{
			mpr_datasets[filename] = 0;
		}
		else if(exetentname == L"shp")
		{
			CGlbWString tempname = mpr_url + filename;
			OGRDataSource *ogrDataSource = poDriver->Open(tempname.ToString().c_str(), 0);
			if(ogrDataSource != NULL)
			{
				delete ogrDataSource;
				mpr_datasets[filename] = 0;
			}
		}
		else
		{
			CGlbWString tempname = mpr_url + filename;
			GDALDatasetH poDataset = GDALOpen(tempname.ToString().c_str(), GA_ReadOnly);
			if (poDataset != NULL)
			{
				GDALClose(poDataset);
				mpr_datasets[filename] = 0;
			}			
		}//raster,shape
		//��ȡ�¸��ļ�
		filename = findfile->GetNext();
	}
	delete findfile;	
}
IGlbDataset* CGlbFileDataSource::GetDataset(glbInt32 datasetIndex)
{
	if (datasetIndex<0 || (glbUInt32)datasetIndex>=mpr_datasets.size())
	{
		GlbSetLastError(L"��Ч����");
		return NULL;
	}
	
	map<CGlbWString/*����׺�����ļ���*/, glbByte/*0*/>::iterator itr = mpr_datasets.begin();
	while (itr != mpr_datasets.end())
	{
		if (datasetIndex == 0)
		{
			break;
		}
		datasetIndex--;
		itr++;
	}
	return GetDatasetByName(itr->first.c_str());
}
IGlbDataset* CGlbFileDataSource::GetDatasetByName(const glbWChar* datasetName)
{
	if (NULL == datasetName)
	{
		GlbSetLastError(L"����datasetName��NULL");
		return NULL;
	}
	IGlbDataset* retds      = NULL;
	CGlbWString fullname    = mpr_url + datasetName;
	CGlbWString exetentname = CGlbPath::GetExtentName(datasetName);
	exetentname.ToLower();
	if(exetentname == L"pyd")
	{
		CGlbSQLitePyramid* fp = new CGlbSQLitePyramid();
		if(!fp->Open(fullname.c_str()))
		{
			delete fp;			
			return NULL;
		}		
		CGlbWString  tempname  = mpr_url + CGlbPath::RemoveExtName(datasetName);
		GDALDatasetH poDataset = GDALOpen(tempname.ToString().c_str(), GA_Update);
		if(poDataset == NULL)
			poDataset = GDALOpen(fullname.ToString().c_str(), GA_ReadOnly);
		CGlbFileRasterDataset* rds = new CGlbFileRasterDataset();
		if(!rds->Initialize(/*dynamic_cast<IGlbDataSource*>(*/this/*)*/,
			                mpr_url.c_str(),
							datasetName,
							L"",
							poDataset, fp))
		{
			delete rds;			
			return NULL;
		}
		retds = (IGlbDataset*)rds;
	}
	else if(exetentname == L"ted")
	{
		CGlbTedSQLite* ted = new CGlbTedSQLite();
		if(!ted->Open(fullname.c_str()))
		{
			delete ted;
			return NULL;
		}
		CGlbFileTerrainDataset* tds = new CGlbFileTerrainDataset();
		if(!tds->Initialize(this, datasetName, L"", ted))
		{
			delete tds;
			return NULL;
		}
		retds = (IGlbDataset*)tds;
	}
	else if(exetentname == L"shp")
	{
		//shp�ļ�����
		OGRSFDriverRegistrar *ogrDriverRegister = OGRSFDriverRegistrar::GetRegistrar();
		OGRSFDriver          *poDriver          = ogrDriverRegister->GetDriverByName("ESRI Shapefile");
			CGlbWString fullname  = mpr_url + datasetName;
			OGRDataSource *ogrDataSource = poDriver->Open(fullname.ToString().c_str(), 0);
			if(ogrDataSource == NULL)
			{
				CGlbString err = CPLGetLastErrorMsg();
				GlbSetLastError(err.ToWString().c_str());
				return NULL;
			}
			CGlbWString tempname = datasetName;
			CGlbWString name     = CGlbPath::RemoveExtName(tempname);
			OGRLayer* layer      = ogrDataSource->GetLayerByName(name.ToString().c_str());
			if(layer == NULL)
			{
				CGlbString err = CPLGetLastErrorMsg();
				GlbSetLastError(err.ToWString().c_str());
				return NULL;
			}
			CGlbShapeFeatureClass* feacls = new CGlbShapeFeatureClass();
			glbBool _rs = feacls->Initialize(this,layer,ogrDataSource);
			if (_rs)
				retds = feacls;
			else
			{				
				delete feacls;
				return NULL;
			}		
	}
	else
	{
		GDALDatasetH poDataset = GDALOpen(fullname.ToString().c_str(), GA_Update);
		if (poDataset == NULL)		
			poDataset = GDALOpen(fullname.ToString().c_str(), GA_ReadOnly);		
		if (poDataset != NULL)
		{
			CGlbWString tempname  = mpr_url + datasetName + L".pyd";
			CGlbSQLitePyramid* fp = new CGlbSQLitePyramid();
			if(!fp->Open(tempname.c_str()))
			{
				delete fp;
				fp = NULL;
			}
			CGlbFileRasterDataset* rds = new CGlbFileRasterDataset();
			if(!rds->Initialize(this,
				                mpr_url.c_str(),
								datasetName,
								L"",
								poDataset, fp))
			{
				delete rds;
				return NULL;
			}
			retds = (IGlbDataset*)rds;
		}
	}//raster,shape
	
	mpr_datasets[datasetName] = 0;
	return retds;
}
glbBool CGlbFileDataSource::DeleteDataset (const glbWChar * datasetName)
{
	if (NULL == datasetName)
	{
		GlbSetLastError(L"����datasetName��NULL");
		return false;
	}

	glbBool     rt          = false;
	CGlbWString fullname    = mpr_url + datasetName;
	CGlbWString exetentname = CGlbPath::GetExtentName(datasetName);
	exetentname.ToLower();
	if(exetentname == L"pyd")
	{
		rt = CGlbPath::DeleteFile(fullname.c_str());
	}
	else if(exetentname == L"ted")
	{
		rt = CGlbPath::DeleteFile(fullname.c_str());
	}
	else if(exetentname == L"shp")
	{
		rt = CGlbPath::DeleteFile(fullname.c_str());
		if(rt)
		{
			CGlbWString basename = CGlbPath::RemoveExtName(datasetName);
			fullname = mpr_url + basename + L".shx";
			CGlbPath::DeleteFile(fullname.c_str());
			fullname = mpr_url + basename + L".dbf";
			CGlbPath::DeleteFile(fullname.c_str());
			fullname = mpr_url + basename + L".prj";
			CGlbPath::DeleteFile(fullname.c_str());
			fullname = mpr_url + basename + L".qix";
			CGlbPath::DeleteFile(fullname.c_str());
		}
	}
	else
	{
		rt = CGlbPath::DeleteFile(fullname.c_str());
		if(rt)
		{
			CGlbWString basename = datasetName;
			fullname = mpr_url + basename +L".msk";
			CGlbPath::DeleteFile(fullname.c_str());
			fullname = mpr_url + basename +L".pyd";
			CGlbPath::DeleteFile(fullname.c_str());
		}
	}
	map<CGlbWString/*����׺�����ļ���*/, glbByte/*0*/>::iterator itr = mpr_datasets.find(datasetName); 
	if(itr != mpr_datasets.end())mpr_datasets.erase(itr);
	return true;
}
const glbWChar*	CGlbFileDataSource::GetAlias()
{
	return mpr_alias.c_str();
}
glbBool CGlbFileDataSource::SetAlias(const glbWChar* alias)
{
	if(NULL == alias)
		mpr_alias = L"";
	else
		mpr_alias = alias;
	return true;
}
const glbWChar*	CGlbFileDataSource::GetProviderName()
{
	return L"file";
}
glbInt32 CGlbFileDataSource::GetID() 
{
	return mpr_id;
}
glbBool CGlbFileDataSource::SetID(glbInt32 id)
{
	mpr_id = id;
	return true;
}
glbWChar* CGlbFileDataSource::GetLastError()
{
	return GlbGetLastError();
}
glbBool CGlbFileDataSource::QueryInterface(const glbWChar *riid,void **ppvoid)
{
	if (riid == NULL || ppvoid == NULL)
	{
		GlbSetLastError(L"��������ȷ");
		return false;
	}
	CGlbWString tmpiid = riid;
	if(tmpiid == L"IGlbDataSource")
	{		
		*ppvoid = (IGlbDataSource*)this;
		return true;
	}
	else if(tmpiid == L"IGlbRasterDataSource")
	{
		*ppvoid = (IGlbRasterDataSource*)this;
		return true;
	}
	else if(tmpiid == L"IGlbFeatureDataSource")
	{
		*ppvoid = (IGlbFeatureDataSource*)this;
		return true;
	}
	GlbSetLastError(L"�ӿڲ�����");
	return false;
}

IGlbDataset*  CGlbFileDataSource::CreateDataset( const glbWChar* name ,
		                                const glbWChar* alias,
		                                GlbDatasetTypeEnum type,
		                                const glbWChar* srs,
		                                glbBool hasZ, 
										glbBool hasM)
{
	GlbSetLastError(L"��֧�� �������ݼ�");
	return NULL;
}
glbBool CGlbFileDataSource::ExecuteSQL(const glbWChar* sqlstr )
{
	GlbSetLastError(L"��֧�� ִ��SQL");
	return false;
}
IGlbObjectCursor* CGlbFileDataSource::Query(const glbWChar* querystr)
{
	GlbSetLastError(L"��֧�� Query");
	return NULL;
}