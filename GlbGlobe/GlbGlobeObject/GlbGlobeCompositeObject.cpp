#include "StdAfx.h"
#include "GlbGlobeCompositeObject.h"

#include "CGlbGlobe.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeTypes.h"

#include "GlbCalculateNodeSizeVisitor.h"


#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

using namespace GlbGlobe;

glbInt32 ComputeNodeSize( osg::Node *node )
{
	glbInt32 mpr_objSize=0;
		CGlbCalculateNodeSizeVisitor cnsv;
		node->accept(cnsv);
		mpr_objSize = cnsv.getTextureSize() + cnsv.getNodeMemSize();	

	return mpr_objSize;
}
//////////////////////////////////////////////////////////////////////////
CGlbGlobeCompositeObject::CGlbGlobeCompositeObject(void)
{
	
}

CGlbGlobeCompositeObject::~CGlbGlobeCompositeObject(void)
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeRObject* obj = (*iter).get();
		obj->RemoveFromScene(true);
		obj->SetDestroy();
		obj->SetParentObject(NULL);
		if(mpt_globe)
		{//�ӳ���������ɾ������
			mpt_globe->mpr_sceneobjIdxManager->RemoveObject(obj);
		}
	}
}
/*
*   ���ø߳�ģʽ
*   �Ӷ��� �ĸ߳�ģʽ���Բ���һ�µ�!!!!
*/
glbBool CGlbGlobeCompositeObject::SetAltitudeMode( GlbAltitudeModeEnum mode ,glbBool &isNeedDirectDraw)
{
	glbBool dd = false;
	isNeedDirectDraw = false;
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeREObject* obj = dynamic_cast<CGlbGlobeREObject*>((*iter).get());
		if(obj)
		{
			/*
			    ��Ϊ�Ӷ����SetAltitudeMode���ڵ���ʱ�������Ⱦ����.
				���ԣ����Ŷ�����Բ��ø�����Ⱦ����.
			*/
			obj->SetAltitudeMode(mode,dd);
			/*
				��Ϊ�� �Ӷ����е���ҪDirectDraw���еĲ���Ҫ
				���ԣ�
				     �ڴ�ֱ�ӵ����Ӷ����DirectDraw.
			*/
			if(dd)obj->DirectDraw(obj->GetCurrLevel());
		}		
	}
	mpt_altitudeMode = mode;
	return true;
}

/*
*  ������Ⱦ˳��
*  ��Ⱦ˳��ֻ�� ������ģʽ������.
*  
*   !!!!! ��Ҫ�� �����Ӷ������Ⱦ˳�� ����һ��.
*/
void CGlbGlobeCompositeObject::SetRenderOrder( glbInt32 order )
{	
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeREObject* obj = dynamic_cast<CGlbGlobeREObject*>((*iter).get());
		if(obj)
		{
			/*
			    ��Ϊ�Ӷ����SetAltitudeMode���ڵ���ʱ�������Ⱦ����.
				���ԣ����Ŷ�����Բ��ø�����Ⱦ����.
			*/
			obj->SetRenderOrder(order);
		}		
	}
	mpt_renderOrder = order;	
}
/*
	����װ�����ȼ�
	��Ϊ��װ��ʱ���ԡ����϶���Ϊ��λװ�ص�
	���ԣ������Ӷ����װ��˳�� ������һ�µ�.
*/
void CGlbGlobeCompositeObject::SetLoadOrder(glbInt32 order)
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeREObject* obj = dynamic_cast<CGlbGlobeREObject*>((*iter).get());
		if(obj)
			obj->SetLoadOrder(order);
	}
	mpt_loadOrder = order;
}
/*
	�����Ƿ� ʵ����
	������ ��Ҫ�������Ӷ�����Ҫʵ����
*/
void CGlbGlobeCompositeObject::SetUseInstance(glbBool useInstance)
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeRObject* obj = (*iter).get();
		if(obj)
			obj->SetUseInstance(useInstance);
	}
	mpt_isUseInstance = useInstance;
}
/*
*   ���϶��� ����Ϊһ��������ȣ�����
*   ���ԣ�
*        ���� = min(�Ӷ���ľ���)
*/
glbDouble CGlbGlobeCompositeObject::GetDistance( osg::Vec3d &cameraPos,glbBool isCompute )
{
	glbDouble minDistance=std::numeric_limits<double>::max();
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		double tmp=(*iter)->GetDistance(cameraPos, isCompute);
		if (tmp<minDistance)
			minDistance=tmp;
	}
	return minDistance;
}
/*
*   ���϶��� ����Ϊһ��������ȣ�����
*   
*   �Ӷ����ڳ����������ǲ����ڵ�!!!!
*    ���ԣ�
*         ���϶����LoadData���� һ�ε����Ӷ����LoadData.
*/
void CGlbGlobeCompositeObject::LoadData( glbInt32 level )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->LoadData(level);
	}
	return CGlbGlobeREObject::LoadData(level);
}
/*
*   ���϶��� ����Ϊһ��������ȣ�����
*   
*   �Ӷ����ڳ����������ǲ����ڵ�!!!!
*    ���ԣ�
*         ���϶����AddToScene���� һ�ε����Ӷ����AddToScene.
*/
void CGlbGlobeCompositeObject::AddToScene()
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->AddToScene();
	}
}
/*
*   ���϶��� ����Ϊһ��������ȣ�����
*   
*   �Ӷ����ڳ����������ǲ����ڵ�!!!!
*    ���ԣ�
*         ���϶����RemoveFromScene���� һ�ε����Ӷ����RemoveFromScene.
*/
glbInt32 CGlbGlobeCompositeObject::RemoveFromScene( glbBool isClean )
{
	glbInt32 objsize=0;
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		objsize+=(*iter)->RemoveFromScene(isClean);
	}
	return objsize;
}

void CGlbGlobeCompositeObject::SetParentNode( osg::Node *parent )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetParentNode(parent);
	}
	return CGlbGlobeREObject::SetParentNode(parent);
}
/*
    ���϶��󣬲��ŵ�����������
	���ԣ�
	     GetBound����NULL
*/
glbref_ptr<CGlbExtent> CGlbGlobeCompositeObject::GetBound( glbBool isWorld /*= true */ )
{
	return NULL;
}

void CGlbGlobeCompositeObject::UpdateElevate()
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->UpdateElevate();
	}
	return CGlbGlobeREObject::UpdateElevate();
}
/*
    ���϶��󣬲��ŵ����������У���ʰȡ
	���ԣ�
	     GetOutLine����NULL
*/
IGlbGeometry * CGlbGlobeCompositeObject::GetOutLine()
{
	return NULL;
}
/*
    ���϶��󣬲�������Ⱦ����
*/
glbBool CGlbGlobeCompositeObject::SetRenderInfo( GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw)
{
	return false;
}

GlbRenderInfo * CGlbGlobeCompositeObject::GetRenderInfo()
{
	return NULL;
}

glbBool CGlbGlobeCompositeObject::SetShow( glbBool isShow,glbBool isOnState/*=false */ )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetShow(isShow,isOnState);
	}
	return CGlbGlobeREObject::SetShow(isShow,isOnState);
}

glbBool CGlbGlobeCompositeObject::SetSelected( glbBool isSelected )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetSelected(isSelected);
	}
	return CGlbGlobeREObject::SetSelected(isSelected);
}

glbBool CGlbGlobeCompositeObject::SetBlink( glbBool isBlink )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetBlink(isBlink);
	}
	return CGlbGlobeREObject::SetBlink(isBlink);
}

void CGlbGlobeCompositeObject::DirectDraw( glbInt32 level )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->DirectDraw(level);
	}
	return CGlbGlobeREObject::DirectDraw(level);
}

void CGlbGlobeCompositeObject::DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->DrawToImage(image, imageW,imageH,ext );
	}
	return CGlbGlobeREObject::DrawToImage(image, imageW,imageH,ext );
}

glbInt32 CGlbGlobeCompositeObject::GetOsgNodeSize()
{
	glbInt32 osgNodeSize=0;
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		osgNodeSize+=(*iter)->GetOsgNodeSize();
	}
	return osgNodeSize;
}

glbBool CGlbGlobeCompositeObject::Load( xmlNodePtr* node,glbWChar* prjPath )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->Load(node,prjPath);
	}
	return true;
}

glbBool CGlbGlobeCompositeObject::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->Save(node,prjPath);
	}
	return true;
}

GlbGlobe::GlbGlobeObjectTypeEnum CGlbGlobeCompositeObject::GetType()
{
	return GLB_OBJECTTYPE_COMPLEX;
}

void CGlbGlobeCompositeObject::SetGlobe( CGlbGlobe *globe )
{
	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		(*iter)->SetGlobe(globe);
	}
	return CGlbGlobeREObject::SetGlobe(globe);
}
glbInt32 CGlbGlobeCompositeObject::GetCount()
{
	return mpr_subogjs.size();
}
/*
*   ����Ӷ���.
*   ���϶���
*          1. �Ӷ��� ��Ҫ��ӵ�����������������ʰȡ.
*          2. 
*/
glbBool CGlbGlobeCompositeObject::AddObject( CGlbGlobeRObject *obj )
{
	if (obj==NULL)return false;

	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		CGlbGlobeRObject* objTmp=(*iter).get();
		if (obj==objTmp)
		{//�Ѿ����
			return false;
		}
	}
	mpr_subogjs.push_back(obj);

	if (mpt_globe!=NULL)
		obj->SetGlobe(mpt_globe);

	if (mpt_parent)
		obj->SetParentNode(mpt_parent);
	//1. �Ӷ�����ӵ���������
	if (mpt_globe!=NULL)mpt_globe->mpr_sceneobjIdxManager->AddObject(obj);
	//2. �����Ӷ����Parent
	obj->SetParentObject(this);
	return true;
}

glbBool CGlbGlobeCompositeObject::RemoveObject( glbInt32 idx )
{
	if (idx<0)return false;

	glbInt32 objCnt =mpr_subogjs.size();
	if (idx>=objCnt)return false;

	CGlbGlobeRObjectList::iterator iterBegin=mpr_subogjs.begin();
	CGlbGlobeRObjectList::iterator iterEnd=mpr_subogjs.end();
	glbInt32 idxTmp=0;
	CGlbGlobeRObject* objTmp;
	for (CGlbGlobeRObjectList::iterator iter=iterBegin;iter!=iterEnd;++iter)
	{
		if (idxTmp==idx)
		{
			objTmp=(*iter).get();
			objTmp->RemoveFromScene(true);
			objTmp->SetParentObject(NULL);			
			if(mpt_globe)
			{//�ӳ���������ɾ������
				mpt_globe->mpr_sceneobjIdxManager->RemoveObject(objTmp);	
			}
			/*
			//if (mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
			�Ӷ�����RemoveFromeScene�����л������Ⱦ����.
			��������Ͳ���:
				DirtyOnTerrainObject()
			*/
			mpr_subogjs.erase(iter);
			
			break;
		}
		++idxTmp;
	}
	return true;
}

CGlbGlobeRObject *CGlbGlobeCompositeObject::GetRObject( glbInt32 idx )
{
	if (idx<0)return NULL;

	glbInt32 objCnt =mpr_subogjs.size();
	if (idx>=objCnt)return NULL;

	CGlbGlobeRObject* objTmp = mpr_subogjs[idx].get();
	return objTmp;
}


