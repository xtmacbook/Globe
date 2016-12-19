#include "StdAfx.h"
#include "GlbGlobeMarkerModelSymbol.h"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "GlbLog.h"

// ��̬�����osgdb_**.lib��������
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
		//modelNode  = ��ʵ����������Find(datalocate )
		//CGlbGlobeInstanceManager::findInstance(datalocate)
	}
	else
	{
		/*
		       level:3,2,1,0.  3���ϸ��
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
			{// ����ǰִ���ļ������·����� ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			modelNode = osgDB::readNodeFile(datalocate.ToString());
			if (modelNode==NULL)
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"��ȡģ���ļ�(%s)ʧ��. \r\n",datalocate.c_str());
			}
		}
		//��modeNode ��ӵ�ʵ����������
		//CGlbGlobeInstanceManager::addInstance(datalocate,modeNode)
	}
	return modelNode;
}