#pragma once
#include "IGlbDataEngine.h"
class  CGlbFileDataSource : public IGlbRasterDataSource,
	                                             public IGlbFeatureDataSource
{
public:
	CGlbFileDataSource(void);
	~CGlbFileDataSource(void);

	glbBool             Initialize(const glbWChar* url);
	IGlbRasterDataset*  GetRasterDataset( const glbWChar * datasetName);
	IGlbTerrainDataset* GetTerrainDataset(const glbWChar* terrainDatasetName);
	IGlbRasterDataset*  CreateRasterDataset(const glbWChar              *name,
		                                   const glbWChar              *alias,
		                                   glbInt32                    nBands,
		                                   GlbPixelTypeEnum            pixelType,
		                                   glbInt32                    columns,
		                                   glbInt32                    rows,
		                                   glbDouble                   orgX    =0,
		                                   glbDouble                   orgY    =0,
		                                   glbDouble                   rsX     =1,
		                                   glbDouble                   rsY     =1,
		                                   const glbWChar*             format  =L"GTiff",
		                                   const glbWChar*             SRS     =NULL,
		                                   const GlbRasterStorageDef   *storeDef=NULL);
	IGlbTerrainDataset* CreateTerrainDataset(const glbWChar* name,
		                                     const glbWChar* alias,
		                                     bool isGlobe=true,
		                                     glbDouble              lzts=36,
		                                     const glbWChar*        SRS=NULL,
		                                     GlbCompressTypeEnum domCompress=GLB_COMPRESSTYPE_JPEG,
		                                     GlbCompressTypeEnum demCompress=GLB_COMPRESSTYPE_ZLIB);
	////IGlbDataSource
	glbInt32             GetCount();
	IGlbDataset*         GetDataset(glbInt32 datasetIndex);
	IGlbDataset*         GetDatasetByName(const glbWChar* datasetName);
	glbBool              DeleteDataset (const glbWChar * datasetName);
	const glbWChar*	     GetAlias();
	glbBool              SetAlias(const glbWChar* alias);
	const glbWChar*	     GetProviderName();
	glbInt32             GetID() ;
	glbBool              SetID(glbInt32 id);
	glbWChar*            GetLastError();
	////IGlbUnknown
	glbBool              QueryInterface(const glbWChar *riid,void **ppvoid);
	////IGlbFeatureDataSource
	IGlbDataset*         CreateDataset( const glbWChar* name ,
		                                const glbWChar* alias,
		                                GlbDatasetTypeEnum type,
		                                const glbWChar* srs,
		                                glbBool hasZ,
										glbBool hasM);
	glbBool              ExecuteSQL(const glbWChar* sqlstr );
	IGlbObjectCursor*    Query(const glbWChar* querystr);

	const glbWChar*	     GetURL(){ return mpr_url.c_str();}
private:
	void                 OpenFiles();
private:
	CGlbWString    mpr_url;   ///<目录
	glbInt32       mpr_id;    ///<数据源唯一id
	CGlbWString    mpr_alias; ///< 数据源别名
	map<CGlbWString, glbByte> mpr_datasets;///<文件夹中的文件列表
};

