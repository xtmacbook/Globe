#include "StdAfx.h"
#include "GlbClipObject.h"
#include "GlbGlobeRObject.h"
#include "CGlbGlobe.h"
#include "GlbGlobeCallBack.h"

using namespace GlbGlobe;

CGlbClipObject::CGlbClipObject( void )
{
	mpr_clipNode = new osg::ClipNode;
	//mpr_globe = NULL;
	mpr_clipNode->setCullingActive(false);
	mpr_clipID = 0;
	mpr_markOffsetPos = true;
}

CGlbClipObject::~CGlbClipObject( void )
{
	mpr_clipNode = NULL;
	//mpr_globe = NULL;
}

glbBool CGlbClipObject::AddClipPlane( glbDouble planeA,glbDouble planeB,glbDouble planeC,glbDouble planeD )
{
	if(mpr_clipNode->getNumClipPlanes() == 0)
		mpr_clipID = 0;
	osg::ref_ptr<osg::ClipPlane> clipPlane = new osg::ClipPlane();
	clipPlane->setClipPlane(planeA,planeB,planeC,planeD);
	clipPlane->setClipPlaneNum(mpr_clipID);
	mpr_clipID++;
	return mpr_clipNode->addClipPlane(clipPlane.get());
}

glbBool CGlbClipObject::RemoveClipPlane( glbUInt32 pos )
{
	return mpr_clipNode->removeClipPlane(pos);
}

void CGlbClipObject::GetClipPlane( glbUInt32 pos,glbDouble &planeA, glbDouble &planeB, glbDouble &planeC, glbDouble &planeD )
{
	osg::ClipPlane *clipPlane = mpr_clipNode->getClipPlane(pos);
	osg::Vec4d plane= clipPlane->getClipPlane();
	planeA = plane.x();
	planeB = plane.y();
	planeC = plane.z();
	planeD = plane.w();
}

glbInt32 CGlbClipObject::GetClipPlaneCount()
{
	return mpr_clipNode->getNumClipPlanes();
}

void CGlbClipObject::SetClipBox( CGlbExtent* ext )
{
	if(ext == NULL)
		return;
	osg::Vec3d min,max;
	ext->GetMin(&min.x(),&min.y(),&min.z());
	ext->GetMax(&max.x(),&max.y(),&max.z());
	osg::BoundingBox bb(min,max);
	mpr_clipNode->createClipBox(bb);
}

void CGlbClipObject::ComputeClipPlane( osg::Vec3d v1, osg::Vec3d v2, osg::Vec3d v3, glbDouble &planeA, glbDouble &planeB, glbDouble &planeC, glbDouble &planeD )
{
	if(mpr_markOffsetPos)
	{
		mpr_offsetPos = v1;
		mpr_markOffsetPos = false;
	}

	osg::Vec3d newV1 = v1 - mpr_offsetPos;
	osg::Vec3d newV2 = v2 - mpr_offsetPos;
	osg::Vec3d newV3 = v3 - mpr_offsetPos;

	osg::Plane clipPlane(newV1,newV2,newV3);
	osg::Vec4d plane= clipPlane.asVec4();
	planeA = plane.x();
	planeB = plane.y();
	planeC = plane.z();
	planeD = plane.w();
}

class ClipNodeCallback : public osg::NodeCallback
{
public:
	ClipNodeCallback(osg::ClipNode *clipNode,osg::Node *objNode,CGlbGlobe *globe,glbBool enableClip,osg::Vec3d offsetPos)
	{
		m_clipNode = clipNode;
		m_objNode = objNode;
		m_globe      = globe;
		m_enableClip = enableClip;
		m_offsetPos = offsetPos;
	}
	~ClipNodeCallback()
	{
		m_clipNode = NULL;
		m_objNode = NULL;
		m_globe = NULL;
	}
	virtual void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		osg::ref_ptr<osg::Group> parentGroup = dynamic_cast<osg::Group*>(node);
		if(!parentGroup.valid())
			return;
		if(m_enableClip)
		{
			osg::ref_ptr<osg::MatrixTransform> clipNodeMt = new osg::MatrixTransform;
			osg::ref_ptr<osg::MatrixTransform> clipNodeInvMt = new osg::MatrixTransform;
			osg::Matrix localToWorld;
			localToWorld.makeTranslate(m_offsetPos);
			clipNodeMt->setMatrix(localToWorld);
			clipNodeMt->addChild(m_clipNode.get());
			parentGroup->removeChild(m_objNode.get());
			osg::Matrixd inverse;
			inverse.invert(localToWorld);
			clipNodeInvMt->setMatrix(inverse);
			clipNodeInvMt->addChild(m_objNode.get());
			m_clipNode->addChild(clipNodeInvMt.get());
			clipNodeMt->setName("ClipNodeMt");
			parentGroup->addChild(clipNodeMt.get());
		}
		else
		{
			osg::ref_ptr<osg::Node> clipNodeMt = NULL;
			for(glbInt32 i = 0; i < parentGroup->getNumChildren(); i++)
			{
				osg::ref_ptr<osg::Node> child = parentGroup->getChild(i);
				if(child.valid() && child->getName() == "ClipNodeMt")
				{
					clipNodeMt = child;
					break;
				}
			}
			if(clipNodeMt.valid())
				parentGroup->removeChild(clipNodeMt.get());
			parentGroup->addChild(m_objNode.get());
		}
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask> task = new CGlbGlobeCallBack::CGlbGlobeFadeCallBackTask(node,this);
		m_globe->mpr_p_callback->AddFadeTask(task.get());
		traverse(node,nv);
	}
private:
	osg::ref_ptr<osg::ClipNode> m_clipNode;
	osg::ref_ptr<osg::Node>        m_objNode;
	glbref_ptr<CGlbGlobe>          m_globe;
	glbBool                                        m_enableClip;
	osg::Vec3d                                  m_offsetPos;
};

void CGlbClipObject::Clip( CGlbGlobeObject* obj, glbBool enableClip/*=true*/ )
{
	if(obj == NULL)
		return;
	CGlbGlobeRObject *robj = (CGlbGlobeRObject*)obj;
	osg::Node *objNode = robj->GetOsgNode();
	osg::Node *parentNode = robj->GetParentNode();
	if(objNode == NULL || parentNode == NULL)
		return;

	glbref_ptr<CGlbGlobe> globe = robj->GetGlobe();
	osg::ref_ptr<ClipNodeCallback> cncb = new ClipNodeCallback(mpr_clipNode.get(),objNode,globe.get(),enableClip,mpr_offsetPos);
	parentNode->addUpdateCallback(cncb.get());
}

void CGlbClipObject::Clip(CGlbGlobeLayer* layer, glbBool enableClip/*=true*/ )
{
	if(layer == NULL)
		return;
	osg::Node *objNode = layer->GetOsgNode();
	osg::Node *parentNode = layer->GetParentNode();
	if(objNode == NULL || parentNode == NULL)
		return;

	glbref_ptr<CGlbGlobe> globe = layer->GetGlobe();
	osg::ref_ptr<ClipNodeCallback> cncb = new ClipNodeCallback(mpr_clipNode.get(),objNode,globe.get(),enableClip,mpr_offsetPos);
	parentNode->addUpdateCallback(cncb.get());
}

void GlbGlobe::CGlbClipObject::SetOpposeSide(unsigned int pos)
{
	if(!mpr_clipNode.valid())
		return;
	osg::ClipPlane *clipPlane = mpr_clipNode->getClipPlane(pos);
	if(clipPlane == NULL)
		return;
	osg::Vec4d plane = clipPlane->getClipPlane();
	clipPlane->setClipPlane(-plane.x(),-plane.y(),-plane.z(),-plane.w());
}

void GlbGlobe::CGlbClipObject::ConvertLatLongHeightToXYZ(glbDouble lat,glbDouble lon,glbDouble ele,glbDouble &posX, glbDouble &posY, glbDouble &posZ)
{
	g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),ele,posX,posY,posZ);
}