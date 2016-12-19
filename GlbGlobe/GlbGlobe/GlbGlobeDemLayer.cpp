#include "StdAfx.h"
#include "GlbGlobeDemLayer.h"
#include "GlbDataSourceFactory.h"
#include "CGlbGlobe.h"
#include "GlbWString.h"
#include "GlbString.h"

using namespace GlbGlobe;

CGlbGlobeDemLayer::CGlbGlobeDemLayer(void)
{
	mpr_layer_type = GLB_GLOBELAYER_DEM;
	mpr_isAccumulationMode = false;
}

CGlbGlobeDemLayer::~CGlbGlobeDemLayer(void)
{
	// 删除后需要刷新地面
	glbref_ptr<CGlbExtent> dirtyExt = GetBound(false);
	mpr_globe->AddDemDirtyExtent(*dirtyExt.get(),true);
}

const glbWChar*	CGlbGlobeDemLayer::GetName()
{
	return mpr_layer_name.c_str();
}
glbBool CGlbGlobeDemLayer::SetName(glbWChar* name)
{
	mpr_layer_name = name;
	return true;
}
GlbGlobeLayerTypeEnum CGlbGlobeDemLayer::GetType()
{
	return mpr_layer_type;
}
glbBool CGlbGlobeDemLayer::Load(xmlNodePtr node, glbWChar* relativepath )
{
	xmlChar* szKey = NULL;
	IGlbDataSource* dsource = NULL;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DatasourceID")))
	{		
		szKey = xmlNodeGetContent(node);
		glbInt32 dsid;
		sscanf((char*)szKey,"%d",&dsid);	
		CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
		dsource = factory->GetDataSourceByID(dsid);
		xmlFree(szKey);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DatasetName")))
	{		
		szKey = xmlNodeGetContent(node);		
		if (dsource)
		{
			char* str = u2g((char*)szKey);
			CGlbString glbstr = str;
			CGlbWString datsetname = glbstr.ToWString();		
			IGlbDataset* dataset = dsource->GetDatasetByName(datsetname.c_str());	
			SetDataset(dataset);
			free(str);
		}
		xmlFree(szKey);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerID")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_layer_id);	
		xmlFree(szKey);
		node = node->next;
	}			
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerName")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		mpr_layer_name = glbstr.ToWString();	
		xmlFree(szKey);
		free(str);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerGroupID")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_layergroup_id);
		xmlFree(szKey);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_opacity);	
		xmlFree(szKey);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Visible")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%c",&mpr_show);
		xmlFree(szKey);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RenderOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_renderorder);	
		xmlFree(szKey);
		node = node->next;
	}		

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LoadOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_loadorder);
		xmlFree(szKey);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActiveAction")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_activeaction);
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionYaw")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionyaw);	
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionPitch")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionpitch);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionRoll")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionroll);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionLonOrX")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLonOrX);
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionLatOrY")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLatOrY);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionAltOrZ")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionAltOrZ);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DefaultVisDis")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_defaultvisibledist);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"AccumulationMode")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%c",&mpr_isAccumulationMode);
		xmlFree(szKey);
		node = node->next;
	}	

	return true;
}
glbBool CGlbGlobeDemLayer::Save(xmlNodePtr node, glbWChar* relativepath )
{
	IGlbDataSource *p_datasource = (IGlbDataSource *)mpr_dataset->GetDataSource();
	glbInt32 dsid = p_datasource->GetID();
	
	char str[128];
	sprintf(str,"%d",dsid);
	xmlNewTextChild(node, NULL, BAD_CAST "DatasourceID", BAD_CAST str);

	const glbWChar* datasetname = mpr_dataset->GetName();
	CGlbWString dsnameString = datasetname;
	char* szOut = g2u((char*)dsnameString.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "DatasetName", BAD_CAST szOut);
	free(szOut);

	sprintf(str,"%d",mpr_layer_id);
	xmlNewTextChild(node, NULL, BAD_CAST "LayerID", BAD_CAST str);

	szOut = g2u((char*)mpr_layer_name.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "LayerName", BAD_CAST szOut);
	free(szOut);

	sprintf(str,"%d",mpr_layergroup_id);
	xmlNewTextChild(node, NULL, BAD_CAST "LayerGroupID", BAD_CAST str);
	sprintf(str,"%d",mpr_opacity);
	xmlNewTextChild(node, NULL, BAD_CAST "Opacity", BAD_CAST str);
	sprintf(str,"%c",mpr_show);
	xmlNewTextChild(node, NULL, BAD_CAST "Visible", BAD_CAST str);

	sprintf(str,"%d",mpr_renderorder);
	xmlNewTextChild(node, NULL, BAD_CAST "RenderOrder", BAD_CAST str);
	sprintf(str,"%d",mpr_loadorder);
	xmlNewTextChild(node, NULL, BAD_CAST "LoadOrder", BAD_CAST str);

	sprintf(str,"%d",mpr_activeaction);//图层激活动作
	xmlNewTextChild(node, NULL, BAD_CAST "ActiveAction", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionyaw);//图层飞向跳转时的偏航角【绕y轴旋转】
	xmlNewTextChild(node, NULL, BAD_CAST "ActionYaw", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionpitch);//图层飞向跳转时的倾斜角【绕x轴旋转】
	xmlNewTextChild(node, NULL, BAD_CAST "ActionPitch", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionroll);//图层飞向跳转时的旋转角【绕z轴旋转】
	xmlNewTextChild(node, NULL, BAD_CAST "ActionRoll", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionLonOrX);//图层飞向跳转时的视点经度或X
	xmlNewTextChild(node, NULL, BAD_CAST "ActionLonOrX", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionLatOrY);//图层飞向跳转时的视点纬度或Y
	xmlNewTextChild(node, NULL, BAD_CAST "ActionLatOrY", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionAltOrZ);//图层飞向跳转时的视点高度或Z
	xmlNewTextChild(node, NULL, BAD_CAST "ActionAltOrZ", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_defaultvisibledist);//图层飞向跳转时的视点高度或Z
	xmlNewTextChild(node, NULL, BAD_CAST "DefaultVisDis", BAD_CAST str);

	sprintf(str,"%c",mpr_isAccumulationMode);
	xmlNewTextChild(node, NULL, BAD_CAST "AccumulationMode", BAD_CAST str);

	return true;
}
glbBool CGlbGlobeDemLayer::Show(glbBool show,glbBool isOnState)
{
	glbBool res = CGlbGlobeLayer::Show(show,isOnState);
	if (res)
	{// 刷新dem图层区域
		glbref_ptr<CGlbExtent> dirtyExt = GetBound(false);
		mpr_globe->AddDemDirtyExtent(*dirtyExt.get(),true);
	}
	return res;
}
glbBool CGlbGlobeDemLayer::SetOpacity(glbInt32 opacity)
{	
	return false;
}
glbref_ptr<CGlbExtent> CGlbGlobeDemLayer::GetBound(glbBool isWorld)
{	
	IGlbDataset* p_dataset = GetDataset();
	IGlbRasterDataset* p_raster_dataset = dynamic_cast<IGlbRasterDataset*>(p_dataset);
	if (p_raster_dataset)
	{
		glbInt32 minLevel,maxLevel;
		p_raster_dataset->GetPyramidLevels(&minLevel,&maxLevel);
		glbref_ptr<CGlbExtent> extent = (CGlbExtent*)p_raster_dataset->GetExtent();
		if (!extent) return NULL;
		glbref_ptr<CGlbExtent>	_bound = new CGlbExtent;
		if (mpr_globe->GetType()==GLB_GLOBETYPE_GLOBE && isWorld)
		{					
			glbDouble west,east,south,north;
			extent->Get(&west,&east,&south,&north);
			osg::Vec3d _vec;
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(west),osg::DegreesToRadians(north),0,_vec.x(),_vec.y(),_vec.z());
			_bound->Merge(_vec.x(),_vec.y(),_vec.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(west),osg::DegreesToRadians(south),0,_vec.x(),_vec.y(),_vec.z());
			_bound->Merge(_vec.x(),_vec.y(),_vec.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(east),osg::DegreesToRadians(south),0,_vec.x(),_vec.y(),_vec.z());
			_bound->Merge(_vec.x(),_vec.y(),_vec.z());
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(east),osg::DegreesToRadians(north),0,_vec.x(),_vec.y(),_vec.z());
			_bound->Merge(_vec.x(),_vec.y(),_vec.z());
		}
		else
			_bound->Merge(*extent.get());
		return _bound;
	}
	return NULL;
}

glbBool CGlbGlobeDemLayer::HasPyramid()
{
	IGlbDataset* p_dataset = GetDataset();
	IGlbRasterDataset* p_raster_dataset = dynamic_cast<IGlbRasterDataset*>(p_dataset);
	if (p_raster_dataset)
	{		
		return p_raster_dataset->HasPyramid();
	}
	return false;
}
glbBool CGlbGlobeDemLayer::BuildPyramid(IGlbProgress * progress,glbDouble lzts , glbBool isGlobe)
{
	IGlbDataset* p_dataset = GetDataset();
	IGlbRasterDataset* p_raster_dataset = dynamic_cast<IGlbRasterDataset*>(p_dataset);
	if (p_raster_dataset)
	{
		//GlbPyramidIndexMethodEnum indexMethod = GLB_PYRAMIDINDEXMETHOD_LEFTBOTTOM_YUP;
		//if (!isGlobe)
		//	indexMethod = GLB_PYRAMIDINDEXMETHOD_ZEROPOINT_YUP;		
		glbInt32 columns,rows;
		p_raster_dataset->GetSize(&columns,&rows);
		return p_raster_dataset->BuildPyramid( 0,
										0,
										columns,
										rows,
										GLB_RASTER_CUBIC,
										256,
										256,
										lzts,
										true,
										false,
										false,
										0.0,
										progress);

	/*	int ctimes = (int)Math.Ceiling(((double)cols / bsize));
		int rtimes = (int)Math.Ceiling(((double)rows / bsize));
		int srow = 0;
		int scol = 0;
		int srows = 0;
		int scols = 0;
		for (int r = 0; r < rtimes; r++)
		{
			srow = r * bsize;
			srows = bsize;
			for (int c = 0; c < ctimes; c++)
			{
				scol = c * bsize;
				scols = bsize;
				mpr_datasets[i].BuildPyramid(scol,
					srow,
					scols,
					srows,
					mpr_resmpleType,
					tsx,
					tsy,
					mpr_lzts,
					mpr_isglobe,
					mpr_isShareEdge,
					-1,
					isReplace,
					rpVal,
					null);
			}
		}*/
	}
	return true;
}
glbBool CGlbGlobeDemLayer::GetPyramidLevels(glbInt32 *minLevel, glbInt32 *maxLevel)
{
	IGlbDataset* p_dataset = GetDataset();
	IGlbRasterDataset* p_raster_dataset = dynamic_cast<IGlbRasterDataset*>(p_dataset);
	if (p_raster_dataset && p_raster_dataset->HasPyramid())
	{		
		return p_raster_dataset->GetPyramidLevels(minLevel,maxLevel);
	}
	return false;
}

glbBool CGlbGlobeDemLayer::CreatePixelBlock(CGlbPixelBlock ** pixelblock)
{
	IGlbDataset* p_dataset = GetDataset();
	IGlbRasterDataset* p_raster_dataset = dynamic_cast<IGlbRasterDataset*>(p_dataset);
	if (p_raster_dataset)
	{		
		if(p_raster_dataset->HasPyramid())
			*pixelblock = p_raster_dataset->CreatePyramidPixelBlock();
		else
			*pixelblock = p_raster_dataset->CreatePixelBlock(17,17);	
	}
	return false;
}
glbBool CGlbGlobeDemLayer::ReadData(glbInt32 level,glbInt32 tilerow,glbInt32 tilecolumn,CGlbPixelBlock* p_pixelblock)
{
	IGlbDataset* p_dataset = GetDataset();
	IGlbRasterDataset* p_raster_dataset = dynamic_cast<IGlbRasterDataset*>(p_dataset);
	if (p_raster_dataset)
	{
		if ( p_raster_dataset->HasPyramid() )
			return p_raster_dataset->ReadPyramidByTile(level,tilecolumn,tilerow,p_pixelblock);		
	}	
	return true;
}
glbBool CGlbGlobeDemLayer::ReadDataEx(CGlbExtent* p_extent, CGlbPixelBlock* p_pixelblock)
{
	IGlbDataset* p_dataset = GetDataset();
	IGlbRasterDataset* p_raster_dataset = dynamic_cast<IGlbRasterDataset*>(p_dataset);
	if (p_raster_dataset)
	{
		if ( ! p_raster_dataset->HasPyramid() )
			return p_raster_dataset->ReadEx(p_extent,p_pixelblock,GLB_RASTER_CUBIC);	
	}
	return true;
}
glbBool CGlbGlobeDemLayer::IsAccumulationMode()
{
	return mpr_isAccumulationMode;
}

void CGlbGlobeDemLayer::SetAccumulationMode(glbBool isAccumulation)
{
	mpr_isAccumulationMode = isAccumulation;
}

glbBool GlbGlobe::CGlbGlobeDemLayer::Load2( xmlNodePtr node, glbWChar* relativepath )
{
	CGlbGlobeLayer::Load2(node,relativepath);
	xmlChar* szKey = NULL;
	IGlbDataSource* dsource = NULL;
	xmlNodePtr childNode = NULL;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionAltOrZ")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionAltOrZ);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionLatOrY")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLatOrY);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionLonOrX")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLonOrX);
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionPitch")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionpitch);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionRoll")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionroll);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActionYaw")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionyaw);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetActiveAction")))
	{
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString activeAction = (char*)str;
		if(activeAction == "NGLB_GLOBE_FLYTO")
		{
			mpr_activeaction = GlbGlobeActiveActionEnum::GLB_GLOBE_FLYTO;
		}
		else
		{
			mpr_activeaction = GlbGlobeActiveActionEnum::GLB_GLOBE_JUMPTO;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetDataset")))
	{
		childNode = node->xmlChildrenNode;//text
		childNode = childNode->next;//Name
		szKey = xmlNodeGetContent(childNode);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		CGlbWString datsetname = glbstr.ToWString();		
		xmlFree(szKey);
		childNode = childNode->next;//text
		childNode = childNode->next;//Alias
		childNode = childNode->next;//text
		childNode = childNode->next;//GetDataSource
		childNode = childNode->xmlChildrenNode;//text
		childNode = childNode->next;//GetAlias
		childNode = childNode->next;//text
		childNode = childNode->next;//Url
		szKey = xmlNodeGetContent(childNode);
		str = u2g((char*)szKey);
		glbstr = str;
		CGlbWString url = glbstr.ToWString();		
		xmlFree(szKey);
		childNode = childNode->next;//text
		childNode = childNode->next;//User
		glbstr = "";
		if (childNode && (!xmlStrcmp(childNode->name, (const xmlChar *)"User")))
		{
			szKey = xmlNodeGetContent(childNode);
			str = u2g((char*)szKey);
			glbstr = str;
			xmlFree(szKey);
			childNode = childNode->next;//text
			childNode = childNode->next;//Password
		}
		CGlbWString user = glbstr.ToWString();	
		glbstr = "";
		if (childNode && (!xmlStrcmp(childNode->name, (const xmlChar *)"Password")))
		{
			szKey = xmlNodeGetContent(childNode);
			str = u2g((char*)szKey);
			glbstr = str;
			xmlFree(szKey);
			childNode = childNode->next;//text
			childNode = childNode->next;//ProviderName
		}
		CGlbWString password = glbstr.ToWString();
		glbstr = "";
		if (childNode && (!xmlStrcmp(childNode->name, (const xmlChar *)"ProviderName")))
		{
			szKey = xmlNodeGetContent(childNode);
			str = u2g((char*)szKey);
			glbstr = str;
			xmlFree(szKey);
			childNode = childNode->next;//text
			childNode = childNode->next;//ProviderName
		}
		CGlbWString providerName = glbstr.ToWString();		
		CGlbDataSourceFactory* factory = CGlbDataSourceFactory::GetInstance();
		if(providerName == L"file")
		{
			if(url == L".\\res")
			{
				dsource = factory->GetDataSourceByID(0);
			}
		}
		else
			dsource = factory->OpenDataSource(url.c_str(),user.c_str(),password.c_str(),providerName.c_str());

		if(dsource == NULL)
			return false;

		IGlbDataset* dataset = dsource->GetDatasetByName(datsetname.c_str());
		SetDataset(dataset);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetDefaultVisibleDistance")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_defaultvisibledist);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetLoadOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_loadorder);
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetName")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		mpr_layer_name = glbstr.ToWString();	
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetOpacity")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_opacity);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GetRenderOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_renderorder);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsShow")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%c",&mpr_show);
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	return true;
}