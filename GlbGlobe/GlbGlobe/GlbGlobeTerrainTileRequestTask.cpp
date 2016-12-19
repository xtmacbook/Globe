#include "StdAfx.h"
#include "GlbGlobeTerrainTileRequestTask.h"
#include "GlbGlobeTerrain.h"

using namespace GlbGlobe;

CGlbGlobeTerrainTileRequestTask::CGlbGlobeTerrainTileRequestTask(CGlbGlobeTerrainTile* tile,
																std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
																std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlayers,
																std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs, 
																std::vector<glbref_ptr<CGlbGlobeRObject>> mterrainobjs)
{
	mpr_terraintile = tile;
	mpr_domlayers = domlayers;
	mpr_demlayers = demlayers;
	mpr_terrainobjs = terrainobjs; 
	mpr_mterrainobjs = mterrainobjs;
}


CGlbGlobeTerrainTileRequestTask::~CGlbGlobeTerrainTileRequestTask(void)
{
	mpr_terraintile = NULL;
	size_t i = 0;
	//for(i = 0; i < mpr_domlayers.size(); i++)
	//{
	//	mpr_domlayers.at(i) = NULL;
	//}
	//for(i = 0; i < mpr_demlayers.size(); i++)
	//{
	//	mpr_demlayers.at(i) = NULL;
	//}
	//for(i = 0; i < mpr_terrainobjs.size(); i++)
	//{
	//	mpr_terrainobjs.at(i) = NULL;
	//}
	//for(i = 0; i < mpr_terrainobjs.size(); i++)
	//{
	//	mpr_mterrainobjs.at(i) = NULL;
	//}
	mpr_domlayers.clear();
	mpr_demlayers.clear();
	mpr_terrainobjs.clear();
	mpr_mterrainobjs.clear();
}

glbBool CGlbGlobeTerrainTileRequestTask::doRequest()
{
	if (mpr_terraintile)
	{
		// 进入数据集读取临界区
		//EnterCriticalSection(&(mpr_terraintile->GetTerrain()->mpr_critical));
		mpr_terraintile->LoadData(mpr_domlayers,mpr_demlayers,mpr_terrainobjs,mpr_mterrainobjs);
		//LeaveCriticalSection(&(mpr_terraintile->GetTerrain()->mpr_critical));
	}
	return false;
}

CGlbGlobeObject* CGlbGlobeTerrainTileRequestTask::getObject()
{
	return NULL;
}

CGlbGlobeRefreshTileTextureTask::CGlbGlobeRefreshTileTextureTask(CGlbGlobeTerrainTile* tile,									
																	std::vector<glbref_ptr<CGlbGlobeDomLayer>>   domlayers,                   
																	std::vector<glbref_ptr<CGlbGlobeRObject>>  terrainObjs,
																	glbBool isdomrefresh)
{
	mpr_terraintile = tile;
	mpr_domlayers = domlayers;
	mpr_terrainobjs = terrainObjs;
	mpr_isDomrefesh = isdomrefresh;
}

CGlbGlobeRefreshTileTextureTask::~CGlbGlobeRefreshTileTextureTask(void)
{
	mpr_terraintile = NULL;
	//size_t i = 0;
	//for(i = 0; i < mpr_domlayers.size(); i++)
	//{
	//	mpr_domlayers.at(i) = NULL;
	//}	
	//for(i = 0; i < mpr_terrainobjs.size(); i++)
	//{
	//	mpr_terrainobjs.at(i) = NULL;
	//}	
	mpr_domlayers.clear();
	mpr_terrainobjs.clear();
}

glbBool CGlbGlobeRefreshTileTextureTask::doRequest()
{
	if (mpr_terraintile)
	{
		mpr_terraintile->LoadDomData(mpr_domlayers,mpr_terrainobjs,mpr_isDomrefesh);
		return true;
	}
	return false;
}

CGlbGlobeObject* CGlbGlobeRefreshTileTextureTask::getObject()
{
	return NULL;
}

CGlbGlobeRefreshTileDemTask::CGlbGlobeRefreshTileDemTask(CGlbGlobeTerrainTile* tile,									
								std::vector<glbref_ptr<CGlbGlobeDemLayer>>   demlayers,                   
								std::vector<glbref_ptr<CGlbGlobeRObject>>  mterrainObjs)
{
	mpr_terraintile = tile;
	mpr_demlayers = demlayers;
	mpr_mterrainobjs = mterrainObjs;
	
}

CGlbGlobeRefreshTileDemTask::~CGlbGlobeRefreshTileDemTask(void)
{
	mpr_terraintile = NULL;
	/*	size_t i = 0;
	for(i = 0; i < mpr_demlayers.size(); i++)
	{
	mpr_demlayers.at(i) = NULL;
	}	
	for(i = 0; i < mpr_mterrainobjs.size(); i++)
	{
	mpr_mterrainobjs.at(i) = NULL;
	}*/	
	mpr_demlayers.clear();
	mpr_mterrainobjs.clear();
}

glbBool CGlbGlobeRefreshTileDemTask::doRequest()
{
	if (mpr_terraintile)
	{
//EnterCriticalSection(&(mpr_terraintile->GetTerrain()->mpr_tDatasetAccessCritical));
		mpr_terraintile->LoadDemData(mpr_demlayers,mpr_mterrainobjs);
//LeaveCriticalSection(&(mpr_terraintile->GetTerrain()->mpr_tDatasetAccessCritical));
		return true;
	}
	return false;
}

CGlbGlobeObject* CGlbGlobeRefreshTileDemTask::getObject()
{
	return NULL;
}

