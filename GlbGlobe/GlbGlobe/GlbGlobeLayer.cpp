#include "StdAfx.h"
#include "GlbGlobeLayer.h"
#include <osg/Switch>

using namespace GlbGlobe;

CGlbGlobeLayer::CGlbGlobeLayer(void)
{
	mpr_layer_id = -1;
	mpr_layergroup_id = -1;
	mpr_globe = NULL;
	mpr_activeaction = GLB_GLOBE_FLYTO;
	mpr_show = true;
	mpr_stateShow = false;	
	mpr_isOnState = false;
	mpr_opacity = 100;
	mpr_spatialRS = L"";
	mpr_dataset = NULL;	
	mpr_renderorder = -1;
	mpr_loadorder = -1;
	mpr_actionyaw = 0;
	mpr_actionpitch = 0;
	mpr_actionroll = 0;
	mpr_actionLonOrX = 0;
	mpr_actionLatOrY = 0;
	mpr_actionAltOrZ = 0;
	mpr_defaultvisibledist = 5000.0;	//默认5000米
	mpr_layer_node = new osg::Switch;
	mpr_dirty     = false;
	mpr_isDestory = false;
}

CGlbGlobeLayer::~CGlbGlobeLayer(void)
{	
	mpr_layer_node = NULL;
	mpr_dataset = NULL;	
	mpr_parent_node = NULL;
}

/*************************************************/
glbInt32 CGlbGlobeLayer::GetId()
{
	return mpr_layer_id;
}
glbBool CGlbGlobeLayer::SetId(glbInt32 id)
{	
	mpr_layer_id = id;
	return true;
}
glbInt32 CGlbGlobeLayer::GetGroupId()
{
	return mpr_layergroup_id;
}
glbBool CGlbGlobeLayer::SetGroupId(glbInt32 id)
{
	mpr_layergroup_id = id;
	return true;
}
glbBool CGlbGlobeLayer::SetActiveAction(GlbGlobeActiveActionEnum action)
{
	mpr_activeaction = action;
	return true;
}
GlbGlobeActiveActionEnum CGlbGlobeLayer::GetActiveAction()
{
	return mpr_activeaction;
}
glbBool CGlbGlobeLayer::SetGlobe(CGlbGlobe* globe)
{
	mpr_globe = globe;
	return true;
}
glbBool CGlbGlobeLayer::Show(glbBool show,glbBool isOnState)
{
	/*
	* 调度器根据可见范围控制对象显示和隐藏此时isOnState==true,isShow==true或false
	*/
	if(isOnState)
	{// 调度器控制显隐		
		if (mpr_layer_node && mpr_layer_node->asGroup()->getNumChildren() > 0)
		{// 节点存在
			if(show)
			{// 如果调度器要求显示，用户要求隐藏或显示，以用户为准
				if(mpr_show == mpr_layer_node->asSwitch()->getValue(0))
					return false;
				mpr_layer_node->asSwitch()->setValue(0,mpr_show);			
			}
			else
			{// 距离超出显示范围后，调度器要求隐藏
				if(show == mpr_layer_node->asSwitch()->getValue(0))
					return false;
				mpr_layer_node->asSwitch()->setValue(0,show);		
			}	
		}	
		else
		{// demlayer,domlayer 使用  马林 2016.01.21
			if (mpr_stateShow == show)
				return false;
			mpr_stateShow = show;
		}
	}
	else
	{// 用户控制显隐
		if (mpr_show == show) return false;
		mpr_show = show;
		if (mpr_layer_node && mpr_layer_node->asGroup()->getNumChildren() > 0)
			mpr_layer_node->asSwitch()->setValue(0,mpr_show);
	}	
	return true;
}
glbBool CGlbGlobeLayer::IsShow()
{
	return mpr_show;
}
glbBool CGlbGlobeLayer::SetOpacity(glbInt32 opacity)
{
	if (mpr_opacity==opacity)
		return false;
	mpr_opacity = opacity;
	return true;
}

glbInt32 CGlbGlobeLayer::GetOpacity()
{
	return mpr_opacity;
}
glbBool CGlbGlobeLayer::SetSpatialRS(glbWChar* new_srs)
{
	mpr_spatialRS = new_srs;
	return true;
}
const glbWChar* CGlbGlobeLayer::GetSpatialRS()
{
	return mpr_spatialRS.c_str();
}
glbBool CGlbGlobeLayer::SetDataset(IGlbDataset* dataset)
{
	if (!dataset)
		return false;

	if (dataset == mpr_dataset.get())
		return true;

	mpr_dataset = dataset;
	return true;
}
IGlbDataset* CGlbGlobeLayer::GetDataset()
{
	return mpr_dataset.get();
}

glbBool CGlbGlobeLayer::SetRenderOrder(glbInt32 order)
{
	mpr_renderorder = order;
	return true;
}
glbInt32 CGlbGlobeLayer::GetRenderOrder()
{
	return mpr_renderorder;
}
glbBool CGlbGlobeLayer::SetLoadOrder(glbInt32 order)
{
	mpr_loadorder = order;
	return true;
}
glbInt32 CGlbGlobeLayer::GetLoadOrder()
{
	return mpr_loadorder;
}
glbBool CGlbGlobeLayer::SetActionYaw(glbDouble yaw)
{
	mpr_actionyaw = yaw;
	return true;
}
glbDouble CGlbGlobeLayer::GetActionYaw()
{
	return mpr_actionyaw;
}
glbBool CGlbGlobeLayer::SetActionPitch(glbDouble pitch)
{
	mpr_actionpitch = pitch;
	return true;
}
glbDouble CGlbGlobeLayer::GetActionPitch()
{
	return mpr_actionpitch;
}
glbBool CGlbGlobeLayer::SetActionRoll(glbDouble roll)
{
	mpr_actionroll = roll;
	return true;
}
glbDouble CGlbGlobeLayer::GetActionRoll()
{
	return mpr_actionroll;
}
glbBool CGlbGlobeLayer::SetActionLonOrX(glbDouble lonOrX)
{
	mpr_actionLonOrX = lonOrX;
	return true;
}
glbDouble CGlbGlobeLayer::GetActionLonOrX()
{
	return mpr_actionLonOrX;
}
glbBool CGlbGlobeLayer::SetActionLatOrY(glbDouble latOrY)
{
	mpr_actionLatOrY = latOrY;
	return true;
}
glbDouble CGlbGlobeLayer::GetActionLatOrY()
{
	return mpr_actionLatOrY;
}
glbBool CGlbGlobeLayer::SetActionAltOrZ(glbDouble altOrZ)
{
	mpr_actionAltOrZ = altOrZ;
	return true;
}
glbDouble CGlbGlobeLayer::GetActionAltOrZ()
{
	return mpr_actionAltOrZ;
}
glbBool CGlbGlobeLayer::SetDefaultVisibleDistance(glbDouble distance)
{
	mpr_defaultvisibledist = distance;
	return true;
}
glbDouble CGlbGlobeLayer::GetDefaultVisibleDistance()
{
	return mpr_defaultvisibledist;
}

osg::Node* CGlbGlobeLayer::GetOsgNode()
{
	return mpr_layer_node.get();
}

glbBool CGlbGlobeLayer::SetParentNode(osg::Node* parentnode)
{
	mpr_parent_node = parentnode;
	return true;
}

osg::Node* CGlbGlobeLayer::GetParentNode()
{
	return mpr_parent_node.get();
}

glbBool CGlbGlobeLayer::IsDirty()
{
	return mpr_dirty;
}

glbBool CGlbGlobeLayer::SetDiry(glbBool dirty)
{
	mpr_dirty = dirty;
	return true;
}
void CGlbGlobeLayer::SetDestroy()
{
	mpr_isDestory = true;
}
//glbWChar* CGlbGlobeLayer::GetLastError()
//{
//	return NULL;
//}

static glbInt32 layerID = 0;

glbBool GlbGlobe::CGlbGlobeLayer::Load2( xmlNodePtr node, glbWChar* relativepath )
{
	mpr_layer_id = layerID;
	layerID++;
	return true;
}

CGlbGlobe * GlbGlobe::CGlbGlobeLayer::GetGlobe()
{
	return mpr_globe;
}
