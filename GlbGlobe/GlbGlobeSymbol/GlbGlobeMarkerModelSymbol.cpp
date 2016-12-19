#include "StdAfx.h"
#include "GlbGlobeMarkerModelSymbol.h"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "GlbLog.h"

// 静态编译的osgdb_**.lib库这样用
//USE_OSGPLUGIN(3ds)
//#pragma comment( lib,"osgdb_3ds.lib" )  

using namespace GlbGlobe;

CGlbGlobeMarkerModelSymbol::CGlbGlobeMarkerModelSymbol(void)
{
}


CGlbGlobeMarkerModelSymbol::~CGlbGlobeMarkerModelSymbol(void)
{
}

osg::Node * CGlbGlobeMarkerModelSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo,glbInt32 level)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerModelSymbolInfo *markerModelInfo = 
		                        static_cast<GlbMarkerModelSymbolInfo *>(renderInfo);
	if(NULL == markerModelInfo)
		return NULL;
	osg::Node *modelNode = NULL;
	CGlbFeature *feature = obj->GetFeature();
	CGlbWString datalocate = L"";

	if(markerModelInfo->locate != NULL)
		datalocate = markerModelInfo->locate->GetValue(feature);

	if (obj->IsUseInstance())
	{
		//modelNode  = 从实例化管理器Find(datalocate )
		//CGlbGlobeInstanceManager::findInstance(datalocate)
	}
	else
	{
		/*
		       level:3,2,1,0.  3是最精细的
			   3: modepath = dir/mdname
			   2:modepath = dir/2/mdname
			   1:modepath = dir/1/mdname
			   0:modepath = dir/0/mdname
			   
			   datalocate ------> level3 modepath
		*/
		CGlbWString dir      = CGlbPath::GetDir(datalocate.c_str());
		CGlbWString fname= CGlbPath::GetFileName(datalocate);
		if(dir.size() == 0)
			return NULL;
		while(level<3)
		{			
			CGlbWString lodFile = dir;
			if (dir.at(dir.size()-1)=='\\')
				lodFile +=L"%ld\\%s";
			else
				lodFile +=L"\\%ld\\%s";
			lodFile = CGlbWString::FormatString((glbWChar*)lodFile.c_str(),level,fname.c_str());			
			modelNode = osgDB::readNodeFile(lodFile.ToString());
			if(modelNode == NULL)
				level++;
			else
				break;
		}
		if(modelNode == NULL)
		{
			glbInt32 index = datalocate.find_first_of(L'.');
			if(index == 0)
			{// 处理当前执行文件的相对路径情况 ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			modelNode = osgDB::readNodeFile(datalocate.ToString());
			if (modelNode==NULL)
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取模型文件(%s)失败. \r\n",datalocate.c_str());
			}
		}
		//把modeNode 添加到实例化管理器
		//CGlbGlobeInstanceManager::addInstance(datalocate,modeNode)
	}
	return modelNode;
}