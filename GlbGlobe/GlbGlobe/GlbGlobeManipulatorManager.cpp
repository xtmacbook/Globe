#include "StdAfx.h"
#include "GlbGlobeManipulatorManager.h"
#include "GlbGlobeManipulator.h"
#include "GlbPlaneManipulator.h"
#include "GlbGlobePathManipulator.h"
#include "GlbPlanePathManipulator.h"
#include "GlbFPSManipulator.h"
#include "GlbPlaneFPSManipulator.h"

using namespace GlbGlobe;

CGlbGlobeManipulatorManager::CGlbGlobeManipulatorManager(CGlbGlobe *globe)
{
	GlbGlobeTypeEnum typeEnum = globe->GetType();
	mpr_glbManiType = GLB_MANIPULATOR_FREE;
	if (typeEnum == GLB_GLOBETYPE_GLOBE)	//ÇòÄ£Ê½
	{
		mpr_manipulator = new CGlbGlobeManipulator(globe);
	}
	else if (typeEnum == GLB_GLOBETYPE_FLAT)	//Æ½ÃæÄ£Ê½
	{
		mpr_manipulator = new CGlbPlaneManipulator(globe);
	}
	mpr_globe = globe;
}
osg::Matrixd CGlbGlobeManipulatorManager::getMatrix() const
{
	if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			osg::Vec3d cameraPos,focusPos;
			glbDouble distance;
			CGlbGlobeManipulator * _cp = (CGlbGlobeManipulator*)(mpr_manipulator);
			{// ²âÊÔ´úÂë ml
				_cp->GetCameraPos(cameraPos);
				_cp->GetFocusPos(focusPos);
				distance = _cp->GetDistance();				
				if (distance < 0.1)
				{
					int kk = 0;
				}
			}			
			return _cp->getMatrix();
		}else if (mpr_manipulator->GetType() == GLB_MANIPULATOR_ANIMATIONPATH)
		{
			CGlbGlobePathManipulator* _pm = (CGlbGlobePathManipulator*)(mpr_manipulator);
			return _pm->getMatrix();
		}else if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
		{
			CGlbFPSManipulator * _fpsm = (CGlbFPSManipulator*)(mpr_manipulator);
			if(_fpsm)
			{
				return _fpsm->getMatrix();
			}
		}
	}else if (mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbPlaneManipulator * _cp = (CGlbPlaneManipulator*)(mpr_manipulator);
			return _cp->getMatrix();
		}else if (mpr_manipulator->GetType() == GLB_MANIPULATOR_ANIMATIONPATH)
		{
			CGlbPlanePathManipulator* _pm = (CGlbPlanePathManipulator*)(mpr_manipulator);
			return _pm->getMatrix();
		}
		else if(GLB_MANIPULATOR_DRIVE == mpr_manipulator->GetType())
		{
			CGlbPlaneFPSManipulator * _pfm = (CGlbPlaneFPSManipulator*)(mpr_manipulator);
			if(_pfm)
			{
				return _pfm->getMatrix();
			}
		}

	}
	return osg::Matrixd::identity();
}

osg::Matrixd CGlbGlobeManipulatorManager::getInverseMatrix() const
{
	return osg::Matrixd::inverse(getMatrix());
}

void CGlbGlobeManipulatorManager::setByMatrix(const osg::Matrixd& matrix)
{
	if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbGlobeManipulator * _cp = (CGlbGlobeManipulator*)(mpr_manipulator);
			_cp->setByMatrix(matrix);
		}else if (mpr_manipulator->GetType() == GLB_MANIPULATOR_ANIMATIONPATH)
		{
			CGlbGlobePathManipulator* _pm = (CGlbGlobePathManipulator*)(mpr_manipulator);
			_pm->setByMatrix(matrix);
		}
		else if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
		{
			CGlbFPSManipulator * _fpsm = (CGlbFPSManipulator*)(mpr_manipulator);
			if(_fpsm)
			{
				_fpsm->setByMatrix(matrix);
			}
		}
	}else if (mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbPlaneManipulator * _cp = (CGlbPlaneManipulator*)(mpr_manipulator);
			_cp->setByMatrix(matrix);
		}else if (mpr_manipulator->GetType() == GLB_MANIPULATOR_ANIMATIONPATH)
		{
			CGlbPlanePathManipulator* _pm = (CGlbPlanePathManipulator*)(mpr_manipulator);
			_pm->setByMatrix(matrix);
		}
		else if(GLB_MANIPULATOR_DRIVE == mpr_manipulator->GetType())
		{
			CGlbPlaneFPSManipulator * _pfm = (CGlbPlaneFPSManipulator*)(mpr_manipulator);
			if(_pfm)
			{
				return _pfm->setByMatrix(matrix);
			}
		}
	}
}

void CGlbGlobeManipulatorManager::setByInverseMatrix(const osg::Matrixd& matrix)
{
	setByMatrix(osg::Matrixd::inverse(matrix));
}

void CGlbGlobeManipulatorManager::home(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbGlobeManipulator * _cp = (CGlbGlobeManipulator*)(mpr_manipulator);
			_cp->home(ea,aa);
		}
	}else if (mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbPlaneManipulator * _cp = (CGlbPlaneManipulator*)(mpr_manipulator);
			_cp->home(ea,aa);
		}
	}
}

glbBool CGlbGlobeManipulatorManager::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us)
{
	if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbGlobeManipulator * _cp = (CGlbGlobeManipulator*)(mpr_manipulator);
			return _cp->handle(ea,us);
		}else if (mpr_manipulator->GetType() == GLB_MANIPULATOR_ANIMATIONPATH)
		{
			CGlbGlobePathManipulator * _pm = (CGlbGlobePathManipulator*)(mpr_manipulator);
			return _pm->handle(ea,us);
		}else if (mpr_manipulator->GetType() == GLB_MANIPULATOR_DRIVE)
		{
			CGlbFPSManipulator * _pm = (CGlbFPSManipulator*)(mpr_manipulator);
			return _pm->handle(ea,us);
		}
		
	}else if (mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbPlaneManipulator * _cp = (CGlbPlaneManipulator*)(mpr_manipulator);
			return _cp->handle(ea,us);
		}else if (mpr_manipulator->GetType() == GLB_MANIPULATOR_ANIMATIONPATH)
		{
			CGlbPlanePathManipulator * _pm = (CGlbPlanePathManipulator*)(mpr_manipulator);
			return _pm->handle(ea,us);
		}
		else if(GLB_MANIPULATOR_DRIVE == mpr_manipulator->GetType())
		{
			CGlbPlaneFPSManipulator * _pfm = (CGlbPlaneFPSManipulator*)(mpr_manipulator);
			if(_pfm)
			{
				return _pfm->handle(ea,us);
			}
		}
	}
	return false;
}

CGlbGlobeManipulatorManager::~CGlbGlobeManipulatorManager(void)
{
	if(mpr_manipulator)delete mpr_manipulator;
}

glbBool CGlbGlobeManipulatorManager::SetDragMode()
{
	if (mpr_globe == NULL)
	{
		return false;
	}
	if (mpr_manipulator)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			return true;
		}
		else if(mpr_manipulator->GetType() == GLB_MANIPULATOR_DRIVE)
		{
			///////////////////////////////////////////////////////
			if(mpr_manipulator)
			{
				if(mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
				{

					osg::Vec3d fouse;
					mpr_manipulator->GetFocusPos(fouse);
					glbDouble distan	= mpr_manipulator->GetDistance();
					GLdouble yaw		= - mpr_manipulator->GetCameraYaw();
					GLdouble  pitch		= mpr_manipulator->GetCameraPitch() - 90.0;
					
					mpr_manipulator     = new CGlbGlobeManipulator(mpr_globe);
					mpr_manipulator->JumpTo(fouse.x(),fouse.y(),fouse.z(),distan,yaw,pitch);
					mpr_glbManiType		= GLB_MANIPULATOR_FREE;
				}
				else if(mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
				{

					osg::Vec3d fouse;
					mpr_manipulator->GetFocusPos(fouse);
					glbDouble distan	= mpr_manipulator->GetDistance();
					GLdouble yaw		= -mpr_manipulator->GetCameraYaw();
					GLdouble  pitch		= mpr_manipulator->GetCameraPitch() - 90.0;

					mpr_manipulator     = new CGlbPlaneManipulator(mpr_globe);
					mpr_manipulator->JumpTo(fouse.x(),fouse.y(),fouse.z(),distan,yaw,pitch);
					mpr_glbManiType		= GLB_MANIPULATOR_FREE;
				}
			}
			return true;
		}
		else
		{	
			mpr_glbManiType = GLB_MANIPULATOR_FREE;
			//ÇÐ»»²Ù¿ØÆ÷£¬È¡camera×´Ì¬
			osg::Vec3d focusPos;
			mpr_manipulator->GetFocusPos(focusPos);
			glbDouble distan = mpr_manipulator->GetDistance();
			glbDouble yaw = mpr_manipulator->GetYaw();
			glbDouble pitch = mpr_manipulator->GetPitch();			
		
			if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				delete mpr_manipulator;
				mpr_manipulator = NULL;
				mpr_manipulator = new CGlbGlobeManipulator(mpr_globe);
				mpr_manipulator->JumpTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,yaw,pitch);				
			}else
			{
				delete mpr_manipulator;
				mpr_manipulator = NULL;
				mpr_manipulator = new CGlbPlaneManipulator(mpr_globe);
				mpr_manipulator->JumpTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,yaw,pitch);	
			}
		}		
	}
	return true;
}

glbBool CGlbGlobeManipulatorManager::SetLockMode(GlbGlobeLockModeEnum glbType)
{
	if (mpr_globe && mpr_manipulator)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				CGlbGlobeManipulator* _manipulator = dynamic_cast<CGlbGlobeManipulator*>(mpr_manipulator);
				return _manipulator->SetLockMode(glbType);
			}else if (mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
			{
				CGlbPlaneManipulator* _manipulator = dynamic_cast<CGlbPlaneManipulator*>(mpr_manipulator);
				return _manipulator->SetLockMode(glbType);
			}else if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				CGlbFPSManipulator * _fpsm = (CGlbFPSManipulator*)(mpr_manipulator);
				if(_fpsm)
				{
					//_fpsm->SetLockMode(glbType);
				}
			}
		}
	}
	return false;
}

glbBool CGlbGlobeManipulatorManager::SetDriveMode()
{
	if(NULL == mpr_globe || NULL == mpr_manipulator)
	{
		return false;
	}
	if(GLB_MANIPULATOR_DRIVE == mpr_manipulator->GetType())
	{
		return true;
	}
	
	if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{

		CGlbGlobeManipulator* glbPM =  (CGlbGlobeManipulator*)mpr_manipulator;
		osg::Matrixd m = glbPM->getMatrix();

		IGlbGlobeManipulator * originManipulator = mpr_manipulator;
		mpr_manipulator = new CGlbFPSManipulator(mpr_globe);
		((CGlbFPSManipulator*)mpr_manipulator)->setByMatrix(m);
		((CGlbFPSManipulator*)mpr_manipulator)->beforePFSManipulator();

		//((CGlbFPSManipulator*)mpr_manipulator)->globeManipulator2FPSManipulator();
		mpr_glbManiType = GLB_MANIPULATOR_DRIVE;

		/*if(((CGlbFPSManipulator*)mpr_manipulator)->globeManipulator2FPSManipulator())
		{
		mpr_glbManiType = GLB_MANIPULATOR_DRIVE;
		}
		else
		{
		((CGlbFPSManipulator*)mpr_manipulator)->outPFSManipulator();
		mpr_manipulator = originManipulator;
		}*/
	}
	else if(GLB_GLOBETYPE_FLAT ==  mpr_globe->GetType())
	{
		CGlbGlobeManipulator* glbPM =  (CGlbGlobeManipulator*)mpr_manipulator;
		osg::Matrixd m = glbPM->getMatrix();

		mpr_manipulator = new CGlbPlaneFPSManipulator(mpr_globe);
		((CGlbPlaneFPSManipulator*)mpr_manipulator)->setByMatrix(m);
		((CGlbPlaneFPSManipulator*)mpr_manipulator)->beforePFSManipulator();

		//((CGlbPlaneFPSManipulator*)mpr_manipulator)->globeManipulator2FPSManipulator();
		mpr_glbManiType = GLB_MANIPULATOR_DRIVE;
	}

	return true;
}

glbBool CGlbGlobeManipulatorManager::SetPathMode(CGlbGlobeDynamic *obj)
{
	if (mpr_globe == NULL)
	{
		return false;
	}
	if (mpr_manipulator)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_ANIMATIONPATH)
		{
			return true;
		}else
		{	
			mpr_glbManiType = GLB_MANIPULATOR_ANIMATIONPATH;
			//ÇÐ»»²Ù¿ØÆ÷£¬È¡camera×´Ì¬
			osg::Vec3d focusPos;
			mpr_manipulator->GetFocusPos(focusPos);
			glbDouble distan = mpr_manipulator->GetDistance();
			glbDouble yaw = mpr_manipulator->GetYaw();
			glbDouble pitch = mpr_manipulator->GetPitch();

			if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			{
				mpr_manipulator = new CGlbGlobePathManipulator(mpr_globe,obj);
				mpr_manipulator->JumpTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,yaw,pitch);				
			}else
			{
				mpr_manipulator = new CGlbPlanePathManipulator(mpr_globe,obj);
				mpr_manipulator->JumpTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,yaw,pitch);	
			}
		}		
	}
	return true;
}

GlbGlobeManipulatorTypeEnum CGlbGlobeManipulatorManager::GetType()
{
	return mpr_glbManiType;
}

glbBool CGlbGlobeManipulatorManager::FlyTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch, glbDouble seconds)
{
	return mpr_manipulator->FlyTo(xOrlon,yOrlat,zOrAlt,distance,yaw,pitch,seconds);
}

glbBool CGlbGlobeManipulatorManager::FlyAround(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble seconds, bool repeat)
{
	return mpr_manipulator->FlyAround(xOrlon,yOrlat,zOrAlt,seconds,repeat);
}

glbBool CGlbGlobeManipulatorManager::IsFlying()
{
	return mpr_manipulator->IsFlying();
}

glbBool CGlbGlobeManipulatorManager::StopFlying()
{
	return mpr_manipulator->StopFlying();
}

glbBool CGlbGlobeManipulatorManager::JumpTo(glbDouble xOrlon, glbDouble yOrlat, glbDouble zOrAlt, glbDouble distance, glbDouble yaw, glbDouble pitch)
{
	return mpr_manipulator->JumpTo(xOrlon,yOrlat,zOrAlt,distance,yaw,pitch);
}

glbBool CGlbGlobeManipulatorManager::SetYaw(glbDouble yaw,osg::Vec3d old_focus, glbDouble olddistance)
{
	return mpr_manipulator->SetYaw(yaw,old_focus,olddistance);
}

glbDouble CGlbGlobeManipulatorManager::GetYaw()
{
	return mpr_manipulator->GetYaw();
}

glbBool CGlbGlobeManipulatorManager::SetPitch(glbDouble pitch,osg::Vec3d old_focus,glbDouble olddistance)
{
	return mpr_manipulator->SetPitch(pitch,old_focus,olddistance);
}

glbDouble CGlbGlobeManipulatorManager::GetPitch()
{
	return mpr_manipulator->GetPitch();
}

glbBool CGlbGlobeManipulatorManager::GetCameraPos(osg::Vec3d& cameraPos)
{
	return mpr_manipulator->GetCameraPos(cameraPos);
}

void CGlbGlobeManipulatorManager::SetCameraYaw(glbDouble yaw)
{
	return mpr_manipulator->SetCameraYaw(yaw);
}

glbDouble CGlbGlobeManipulatorManager::GetCameraYaw()
{
	return mpr_manipulator->GetCameraYaw();
}

void CGlbGlobeManipulatorManager::SetCameraPitch(glbDouble pitch)
{
	return mpr_manipulator->SetCameraPitch(pitch);
}

glbDouble CGlbGlobeManipulatorManager::GetCameraPitch()
{
	return mpr_manipulator->GetCameraPitch();
}

glbBool CGlbGlobeManipulatorManager::GetFocusPos(osg::Vec3d& focusPos)
{
	return mpr_manipulator->GetFocusPos(focusPos);
}

glbDouble CGlbGlobeManipulatorManager::GetDistance()
{
	return mpr_manipulator->GetDistance();
}

void CGlbGlobeManipulatorManager::SetCameraParam(glbDouble yaw, glbDouble pitch, osg::Vec3d focusPos, glbDouble distance)
{
	if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbGlobeManipulator * _cp = (CGlbGlobeManipulator*)(mpr_manipulator);
			_cp->SetCameraParam(yaw,pitch,focusPos,distance);
		}
	}else if (mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbPlaneManipulator * _cp = (CGlbPlaneManipulator*)(mpr_manipulator);
			_cp->SetCameraParam(yaw,pitch,focusPos,distance);
		}
	}
}

void CGlbGlobeManipulatorManager::Drag(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
{
	if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbGlobeManipulator * _cp = (CGlbGlobeManipulator*)(mpr_manipulator);
			_cp->Push(ptx1,pty1);
			_cp->Drag(ptx1,pty1,ptx2,pty2);
		}
	}else if (mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbPlaneManipulator * _cp = (CGlbPlaneManipulator*)(mpr_manipulator);
			_cp->Drag(ptx1,pty1,ptx2,pty2);
		}
	}
}
void CGlbGlobeManipulatorManager::Rotate(glbInt32 ptx1,glbInt32 pty1,glbInt32 ptx2,glbInt32 pty2)
{
	if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbGlobeManipulator * _cp = (CGlbGlobeManipulator*)(mpr_manipulator);	
			_cp->Push(ptx1,pty1);
			_cp->Rotate(ptx1,pty1,ptx2,pty2);
		}
	}else if (mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbPlaneManipulator * _cp = (CGlbPlaneManipulator*)(mpr_manipulator);
			_cp->Push(ptx1,pty1);
			_cp->Rotate(ptx1,pty1,ptx2,pty2);
		}
	}
}
void CGlbGlobeManipulatorManager::Zoom(glbBool isScrollUp)
{
	if (mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbGlobeManipulator * _cp = (CGlbGlobeManipulator*)(mpr_manipulator);
			_cp->Zoom(isScrollUp);
		}
	}else if (mpr_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if (mpr_manipulator->GetType() == GLB_MANIPULATOR_FREE)
		{
			CGlbPlaneManipulator * _cp = (CGlbPlaneManipulator*)(mpr_manipulator);
			_cp->Zoom(isScrollUp);
		}
	}
}

glbBool CGlbGlobeManipulatorManager::Move(glbDouble dangleX,glbDouble dangleY)
{
	return mpr_manipulator->Move(dangleX,dangleY);
}

glbBool CGlbGlobeManipulatorManager::TranslateCamera(glbDouble x,glbDouble y,glbDouble z)
{
	return mpr_manipulator->TranslateCamera(x,y,z);
}


glbBool CGlbGlobeManipulatorManager::SetUnderGroundMode(glbBool mode)
{
	return mpr_manipulator->SetUnderGroundMode(mode);
}

glbBool CGlbGlobeManipulatorManager::SetUndergroundDepth(glbDouble zOrAlt)
{
	return mpr_manipulator->SetUndergroundDepth(zOrAlt);
}

glbBool CGlbGlobeManipulatorManager::UpdateCameraElevate(glbDouble elevation)
{
	return mpr_manipulator->UpdateCameraElevate(elevation);
}

glbBool CGlbGlobeManipulatorManager::UpdateFocusElevate(glbDouble elevation)
{
	return mpr_manipulator->UpdateFocusElevate(elevation);
}

void CGlbGlobeManipulatorManager::Shake(glbBool isShake)
{
	if (mpr_manipulator)
		mpr_manipulator->Shake(isShake);	
}

void CGlbGlobeManipulatorManager::SetShakeSope(glbDouble horizontalDegree, glbDouble verticalDegree)
{
	if (mpr_manipulator)
		mpr_manipulator->SetShakeSope(horizontalDegree,verticalDegree);	
}

void CGlbGlobeManipulatorManager::SetShakeTime(glbDouble seconds)
{
	if (mpr_manipulator)
		mpr_manipulator->SetShakeTime(seconds);	
}

void CGlbGlobeManipulatorManager::EnalbeVirtualReferencePlane(glbBool bEnable)
{
	if (mpr_manipulator)
		mpr_manipulator->EnalbeVirtualReferencePlane(bEnable);	
}		
void CGlbGlobeManipulatorManager::SetVirtualReferencePlane(glbDouble zOrAltitude)
{
	if (mpr_manipulator)
		mpr_manipulator->SetVirtualReferencePlane(zOrAltitude);	
}		
glbDouble CGlbGlobeManipulatorManager::GetVirtualReferencePlane()
{
	if (mpr_manipulator)
		return mpr_manipulator->GetVirtualReferencePlane();
	return 0.0;
}

void GlbGlobe::CGlbGlobeManipulatorManager::SetDriveMoveSpeed(glbDouble speed)
{
	if(mpr_manipulator)
	{
		if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
		{
			if(GLB_GLOBETYPE_FLAT ==  mpr_globe->GetType())
			{
				((CGlbPlaneFPSManipulator*)mpr_manipulator)->SetDriveMoveSpeed(speed);
			}
			else if(GLB_GLOBETYPE_GLOBE ==  mpr_globe->GetType())
			{
				((CGlbFPSManipulator*)mpr_manipulator)->SetDriveMoveSpeed(speed);
			}
		}
	}
}

glbDouble GlbGlobe::CGlbGlobeManipulatorManager::GetDriveMoveSpeed()
{
	if(mpr_manipulator)
	{
		if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
		{
			if(GLB_GLOBETYPE_FLAT ==  mpr_globe->GetType())
			{
				return ((CGlbPlaneFPSManipulator*)mpr_manipulator)->GetDriveMoveSpeed();
			}
			else if(GLB_GLOBETYPE_GLOBE ==  mpr_globe->GetType())
			{
				return ((CGlbFPSManipulator*)mpr_manipulator)->GetDriveMoveSpeed();
			}
		}
	}

	return 0.0;
}

glbBool GlbGlobe::CGlbGlobeManipulatorManager::AddCollisionObject(CGlbGlobeRObject *obj)
{
	if(mpr_manipulator)
	{
		if(GLB_GLOBETYPE_FLAT == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbPlaneFPSManipulator*)mpr_manipulator)->AddCollisionObject(obj);
			}
		}
		else if(GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbFPSManipulator*)mpr_manipulator)->AddCollisionObject(obj);
			}
		}
	}
	return false;
}

glbBool GlbGlobe::CGlbGlobeManipulatorManager::RemoveCollisionObjectOrLayerById(int idx,glbBool lays)
{
	if(mpr_manipulator)
	{
		if(GLB_GLOBETYPE_FLAT == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbPlaneFPSManipulator*)mpr_manipulator)->RemoveCollisionObjectOrLayerById(idx,lays);
			}
		}
		else if(GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbFPSManipulator*)mpr_manipulator)->RemoveCollisionObjectOrLayerById(idx,lays);
			}
		}
	}

	return true;
}

glbInt32 GlbGlobe::CGlbGlobeManipulatorManager::GetCollisionObjectCount()
{
	if(mpr_manipulator)
	{
		if(GLB_GLOBETYPE_FLAT == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbPlaneFPSManipulator*)mpr_manipulator)->GetCollisionObjectCount();
			}
		}
		else if(GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbFPSManipulator*)mpr_manipulator)->GetCollisionObjectCount();
			}
		}
	}
	return 0;
}

glbBool GlbGlobe::CGlbGlobeManipulatorManager::RemoveAllCollsionObject()
{
	if(mpr_manipulator)
	{
		if(GLB_GLOBETYPE_FLAT == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbPlaneFPSManipulator*)mpr_manipulator)->RemoveAllCollsionObject();
			}
		}
		else if(GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbFPSManipulator*)mpr_manipulator)->RemoveAllCollsionObject();
			}
		}
	}

	return false;
}

CGlbGlobeRObject* GlbGlobe::CGlbGlobeManipulatorManager::getCollisionObjectByObjId(int idx)
{
	if(mpr_manipulator)
	{
		if(GLB_GLOBETYPE_FLAT == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbPlaneFPSManipulator*)mpr_manipulator)->getCollisionObjectByObjId(idx);
			}
		}
		else if(GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbFPSManipulator*)mpr_manipulator)->getCollisionObjectByObjId(idx);
			}
		}
	}
	return NULL;
}

void GlbGlobe::CGlbGlobeManipulatorManager::GetHaveAddObjsAndLays(std::vector<glbInt32>& objIds,std::vector<glbInt32>& layIds)
{
	if(mpr_manipulator)
	{
		if(GLB_GLOBETYPE_FLAT == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbPlaneFPSManipulator*)mpr_manipulator)->GetHaveAddObjsAndLays(objIds,layIds);
			}
		}
		else if(GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbFPSManipulator*)mpr_manipulator)->GetHaveAddObjsAndLays(objIds,layIds);
			}
		}
	}
}

glbBool GlbGlobe::CGlbGlobeManipulatorManager::AddCollisionLaysObject(glbInt32 layId,CGlbGlobeRObject* obj)
{
	if(mpr_manipulator)
	{
		if(GLB_GLOBETYPE_FLAT == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbPlaneFPSManipulator*)mpr_manipulator)->AddCollisionLaysObject(layId,obj);
			}
		}
		else if(GLB_GLOBETYPE_GLOBE == mpr_globe->GetType())
		{
			if(GLB_MANIPULATOR_DRIVE ==  mpr_manipulator->GetType())
			{
				return ((CGlbFPSManipulator*)mpr_manipulator)->AddCollisionLaysObject(layId,obj);
			}
		}
	}
	return false;
}

