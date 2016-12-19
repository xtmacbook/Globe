#include "StdAfx.h"
#include "GlbGlobeViewElevationRuler.h"
#include <osgText/Text>
#include <osgText/Font>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/TexEnv>
#include <osg/BlendFunc>
#include <osg/TexGen>

#define ElevationRulerWidth 100
#define ElevationRulerHeight 300

using namespace GlbGlobe;

glbBool CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	getElevationRulerPositionType(ea);
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME):
		{
		   return false;;
		}
	default:
		break;
	}
	if (ea.getHandled()) 
		return false;

	if (ea.getTouchData())
		return handleTouch(ea,aa);

	switch(ea.getEventType())
	{
	case osgGA::GUIEventAdapter::MOVE:
		{	
			switch(getElevationRulerPositionType(ea))
			{
			case ELEVATIONRULER_EARTH_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，大手形
					//高亮earth文字区块

					return false;
				}
			case ELEVATIONRULER_COUNTRY_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，大手形
					//高亮country文字区块

					return false;
				}
			case ELEVATIONRULER_PROVINCE_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，大手形
					//高亮province文字区块

					return false;
				}
			case ELEVATIONRULER_CITY_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，大手形
					//高亮city文字区块

					return false;
				}
			case ELEVATIONRULER_STREET_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，大手形
					//高亮street文字区块

					return false;
				}
			case ELEVATIONRULER_HOUSE_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，大手形
					//高亮house文字区块

					return false;
				}
			case ELEVATIONRULER_UG_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，大手形
					//高亮UG文字区块

					return false;
				}
			case ELEVATIONRULER_UP_BTN:
			case ELEVATIONRULER_DOWN_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，大手形
					//高亮标尺

					return false;
				}
			case ELEVATIONRULER_CURSOR_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，大手形
					//高亮游标
					
					return false;
				}
			case ELEVATIONRULER_NONE_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_ARROW);
					mpr_p_view->SetCursor(cursor);	//改变光标，箭头
					return false;
				}
			default:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_ARROW);
					mpr_p_view->SetCursor(cursor);	//改变光标，箭头
					return false;
				}
				break;
			}
			return false; 
		}
	case osgGA::GUIEventAdapter::PUSH:
		{
			_ga_t0 = &ea;
			_ga_t1 = &ea;
			switch(getElevationRulerPositionType(ea))
			{ 
			case ELEVATIONRULER_EARTH_BTN:
				{
					glbDouble distan = 18000000.0;
					return FlyTo(distan);
				}
				break;
			case ELEVATIONRULER_COUNTRY_BTN:
				{
					glbDouble distan = 4500000.0;
					return FlyTo(distan);
				}
				break;
			case ELEVATIONRULER_PROVINCE_BTN:
				{
					glbDouble distan = 800000.0;
					return FlyTo(distan);
				}
				break;
			case ELEVATIONRULER_CITY_BTN:
				{
					glbDouble distan = 10000.0;
					return FlyTo(distan);
				}
				break;
			case ELEVATIONRULER_STREET_BTN:
				{
					glbDouble distan = 1000.0;
					return FlyTo(distan);
				}
				break;
			case ELEVATIONRULER_HOUSE_BTN:
				{
					glbDouble distan = 150.0;
					return FlyTo(distan);
				}
				break;
			case ELEVATIONRULER_UG_BTN:
				{
					glbDouble distan = -60.0;
					return FlyTo(distan);
				}
				break;
			case ELEVATIONRULER_UP_BTN:
				{
					//获取当前高度
					osg::Vec3d cameraPos;
					mpr_p_view->GetCameraPos(cameraPos);
					//相机所在经纬度的地形高度
					glbDouble TerrainCameraAlt = mpr_p_view->GetTerrainElevationOfCameraPos();
					glbDouble distan = cameraPos.z()-TerrainCameraAlt;
					if (distan > 0)
						return FlyTo(distan*1.2);
					else if (distan > -ElevateThreshold)
						return FlyTo(abs(distan));
					else
						return FlyTo(distan*0.8);
				}
				break;
			case ELEVATIONRULER_DOWN_BTN:
				{
					//获取当前高度
					osg::Vec3d cameraPos;
					mpr_p_view->GetCameraPos(cameraPos);
					//相机所在经纬度的地形高度
					glbDouble TerrainCameraAlt = mpr_p_view->GetTerrainElevationOfCameraPos();
					glbDouble distan = cameraPos.z()-TerrainCameraAlt;
					if (distan > ElevateThreshold)
						return FlyTo(distan*0.8);
					else if (distan > 0)
						return FlyTo(-abs(distan));
					else
						return FlyTo(distan*1.2);
				}
				break;
			}
			return false;
		}
	case osgGA::GUIEventAdapter::RELEASE:
		{
			_ga_t1 = NULL;
			switch(getElevationRulerPositionType(ea))
			{
			case ELEVATIONRULER_NONE_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_ARROW);
					mpr_p_view->SetCursor(cursor);	//改变光标，箭头
					return false;
				}
			case ELEVATIONRULER_EARTH_BTN: 
			case ELEVATIONRULER_COUNTRY_BTN: 
			case ELEVATIONRULER_PROVINCE_BTN:	
			case ELEVATIONRULER_CITY_BTN: 
			case ELEVATIONRULER_STREET_BTN:
			case ELEVATIONRULER_HOUSE_BTN:
			case ELEVATIONRULER_UG_BTN:
			case ELEVATIONRULER_CURSOR_BTN:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_HAND);
					mpr_p_view->SetCursor(cursor);	//改变光标，箭头
					return false;
				}
			default:
				{
					HCURSOR cursor = ::LoadCursor(NULL,IDC_ARROW);
					mpr_p_view->SetCursor(cursor);	//改变光标，箭头
					return false;
				}
				break;
			}
			return false;
		}
	case osgGA::GUIEventAdapter::DRAG:
		{
			if (_ga_t1 == NULL)
				return false;
			if (getElevationRulerPositionType(*_ga_t1) == ELEVATIONRULER_CURSOR_BTN)
			{
				glbFloat x = ea.getX() - ea.getXmax() + ElevationRulerWidth*(1.4-0.5);
				glbFloat y = ea.getY() - ea.getYmax() + ElevationRulerHeight*(1.3-0.5);
				glbDouble delt = (75.0-y)/150.0;
				glbDouble distan = DeltToDistance(delt);
				osg::Vec3d cameraPos;
				mpr_p_view->GetCameraPos(cameraPos);
				glbDouble dist = cameraPos.z() - mpr_p_view->GetTerrainElevationOfCameraPos();
				dist = dist - distan;
				//osg::Vec3d focusPos;
				//mpr_p_view->GetFocusPos(focusPos);
				//mpr_p_view->JumpTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,mpr_p_view->GetYaw(),mpr_p_view->GetPitch());
				if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)
				{
					osg::Vec3d up(cos(osg::DegreesToRadians(cameraPos.x()))*cos(osg::DegreesToRadians(cameraPos.y())), sin(osg::DegreesToRadians(cameraPos.x()))*cos(osg::DegreesToRadians(cameraPos.y())), sin(osg::DegreesToRadians(cameraPos.y())));
					up.normalize();
					mpr_p_view->TranslateCamera(-up.x()*dist,-up.y()*dist,-up.z()*dist);
				}else if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
				{
					osg::Vec3d up(0.0,0.0,1.0);
					mpr_p_view->TranslateCamera(-up.x()*dist,-up.y()*dist,-up.z()*dist);
				}else
					return false;
				_ga_t1 = &ea;
				return true;
			}
			return false;
		}
	default: 
		break;
	}
	return false;
}

glbBool CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerEventHandler::handleTouch( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	if (!ea.getTouchData())
		return false;
	unsigned int touchpointnum = ea.getTouchData()->getNumTouchPoints();
	if (touchpointnum == 1)	//单点触控
	{
		switch(ea.getTouchData()->get(0).phase)
		{
		case osgGA::GUIEventAdapter::TOUCH_BEGAN:	//push
			{
				_ga_t0 = &ea;
				_ga_t1 = &ea;
				switch(getTouchElevationRulerPositionType(ea))
				{ 
				case ELEVATIONRULER_EARTH_BTN:
					{
						glbDouble distan = 18000000.0;
						return FlyTo(distan);
					}
					break;
				case ELEVATIONRULER_COUNTRY_BTN:
					{
						glbDouble distan = 4500000.0;
						return FlyTo(distan);
					}
					break;
				case ELEVATIONRULER_PROVINCE_BTN:
					{
						glbDouble distan = 800000.0;
						return FlyTo(distan);
					}
					break;
				case ELEVATIONRULER_CITY_BTN:
					{
						glbDouble distan = 10000.0;
						return FlyTo(distan);
					}
					break;
				case ELEVATIONRULER_STREET_BTN:
					{
						glbDouble distan = 1000.0;
						return FlyTo(distan);
					}
					break;
				case ELEVATIONRULER_HOUSE_BTN:
					{
						glbDouble distan = 150.0;
						return FlyTo(distan);
					}
					break;
				case ELEVATIONRULER_UG_BTN:
					{
						glbDouble distan = -10.0;
						return FlyTo(distan);
					}
					break;
				case ELEVATIONRULER_UP_BTN:
					break;
				case ELEVATIONRULER_DOWN_BTN:
					break;
				}
				return false;
			}
			break;
		case osgGA::GUIEventAdapter::TOUCH_MOVED:	//move
			{
				if (_ga_t1 == NULL)
					return false;
				if (getElevationRulerPositionType(*_ga_t1) == ELEVATIONRULER_CURSOR_BTN)
				{
					glbFloat x = ea.getX() - ea.getXmax() + ElevationRulerWidth*(1.4-0.5);
					glbFloat y = ea.getY() - ea.getYmax() + ElevationRulerHeight*(1.3-0.5);
					glbDouble delt = (75.0-y)/150.0;
					glbDouble distan = DeltToDistance(delt);
					osg::Vec3d focusPos;
					mpr_p_view->GetFocusPos(focusPos);
					mpr_p_view->JumpTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,mpr_p_view->GetYaw(),mpr_p_view->GetPitch());
					_ga_t1 = &ea;
					return true;
				}
				return false;
			}
			break;		
		case osgGA::GUIEventAdapter::TOUCH_ENDED:	//end
			{
				_ga_t1 = NULL;
				return false;
			}
			break;
		}
	}
	return false;
}

GlbGlobeElevationRulerPositionType CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerEventHandler::getElevationRulerPositionType(const osgGA::GUIEventAdapter& ea)
{
	//在导航标尺坐标系下，以导航标尺中心为原点，x轴正方向向右，y轴正方向向左
	glbFloat x = ea.getX() - ea.getXmax() + ElevationRulerWidth*(1.4-0.5);
	glbFloat y = ea.getY() - ea.getYmax() + ElevationRulerHeight*(1.3-0.5);
	//获取游标位置,和相机海拔高度相关系
	glbFloat cursorPosY = 0.0;

	if (x > ElevationRulerWidth*0.18 && x < ElevationRulerWidth*0.36 && y > ElevationRulerHeight*0.28 && y < ElevationRulerHeight*0.35) //Up键
	{
		return ELEVATIONRULER_UP_BTN;
	}else if (x > ElevationRulerWidth*0.18 && x < ElevationRulerWidth*0.36 && y < -ElevationRulerHeight*0.28 && y > -ElevationRulerHeight*0.35)	//Down键
	{
		return ELEVATIONRULER_DOWN_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < ElevationRulerHeight*0.26 && y > ElevationRulerHeight*0.21)	//全球
	{
		return ELEVATIONRULER_EARTH_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < ElevationRulerHeight*0.17 && y > ElevationRulerHeight*0.12)	//国家
	{
		return ELEVATIONRULER_COUNTRY_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < ElevationRulerHeight*0.08 && y > ElevationRulerHeight*0.03)	//省
	{
		return ELEVATIONRULER_PROVINCE_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < -ElevationRulerHeight*0.02 && y > -ElevationRulerHeight*0.07)	//城市
	{
		return ELEVATIONRULER_CITY_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < -ElevationRulerHeight*0.11 && y > -ElevationRulerHeight*0.16)	//街道
	{
		return ELEVATIONRULER_STREET_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < -ElevationRulerHeight*0.20 && y > -ElevationRulerHeight*0.25)	//房屋
	{
		return ELEVATIONRULER_HOUSE_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < -ElevationRulerHeight*0.29 && y > -ElevationRulerHeight*0.34)	//地下
	{
		return ELEVATIONRULER_UG_BTN;
	}else if (x > ElevationRulerWidth*0.18 && x < ElevationRulerWidth*0.36)	//游标
	{
		//求当前游标位置
		osg::Vec3d cameraPos;
		mpr_p_view->GetCameraPos(cameraPos);
		glbDouble hei = cameraPos.z() - mpr_p_view->GetTerrainElevationOfFoucsPos();
		if (cameraPos.z() > 100000.0)
			hei = cameraPos.z();
		glbDouble delt = DistanceToDelt(hei);
		cursorPosY = (0.5-delt)*ElevationRulerHeight*0.5;

		if (y < cursorPosY + ElevationRulerHeight*0.03 && y > cursorPosY - ElevationRulerHeight*0.03) 
			return ELEVATIONRULER_CURSOR_BTN;
	}else if (x > -ElevationRulerWidth*0.36 && x < ElevationRulerWidth*0.36 && y > -ElevationRulerHeight*0.35 && y < ElevationRulerHeight*0.35) //其他位置
	{
		return ELEVATIONRULER_NONE_BTN;
	}else{									//导航标尺以外
	   	return ELEVATIONRULER_OUT_BTN;
	}
	return ELEVATIONRULER_OUT_BTN;
}

GlbGlobeElevationRulerPositionType CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerEventHandler::getTouchElevationRulerPositionType(const osgGA::GUIEventAdapter& ea)
{
	glbDouble tx = ea.getTouchData()->get(0).x;
	glbDouble ty = ea.getTouchData()->get(0).y;
	mpr_p_view->WindowToScene(tx,ty);
	//在导航标尺坐标系下，以导航标尺中心为原点，x轴正方向向右，y轴正方向向左
	glbDouble x = tx - ea.getXmax() + ElevationRulerWidth*(1.4-0.5);
	glbDouble y = ty - ea.getYmax() + ElevationRulerHeight*(1.3-0.5);
	//获取游标位置,和相机海拔高度相关系
	glbDouble cursorPosY = 0.0;

	if (x > ElevationRulerWidth*0.18 && x < ElevationRulerWidth*0.36 && y > ElevationRulerHeight*0.28 && y < ElevationRulerHeight*0.35) //Up键
	{
		return ELEVATIONRULER_UP_BTN;
	}else if (x > ElevationRulerWidth*0.18 && x < ElevationRulerWidth*0.36 && y < -ElevationRulerHeight*0.28 && y > -ElevationRulerHeight*0.35)	//Down键
	{
		return ELEVATIONRULER_DOWN_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < ElevationRulerHeight*0.26 && y > ElevationRulerHeight*0.21)	//全球
	{
		return ELEVATIONRULER_EARTH_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < ElevationRulerHeight*0.17 && y > ElevationRulerHeight*0.12)	//国家
	{
		return ELEVATIONRULER_COUNTRY_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < ElevationRulerHeight*0.08 && y > ElevationRulerHeight*0.03)	//省
	{
		return ELEVATIONRULER_PROVINCE_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < -ElevationRulerHeight*0.02 && y > -ElevationRulerHeight*0.07)	//城市
	{
		return ELEVATIONRULER_CITY_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < -ElevationRulerHeight*0.11 && y > -ElevationRulerHeight*0.16)	//街道
	{
		return ELEVATIONRULER_STREET_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < -ElevationRulerHeight*0.20 && y > -ElevationRulerHeight*0.25)	//房屋
	{
		return ELEVATIONRULER_HOUSE_BTN;
	}else if (x > -ElevationRulerWidth*0.35 && x < ElevationRulerWidth*0.06 && y < -ElevationRulerHeight*0.29 && y > -ElevationRulerHeight*0.34)	//地下
	{
		return ELEVATIONRULER_UG_BTN;
	}else if (x > ElevationRulerWidth*0.18 && x < ElevationRulerWidth*0.36)	//游标
	{
		//求当前游标位置
		osg::Vec3d cameraPos;
		mpr_p_view->GetCameraPos(cameraPos);
		glbDouble hei = cameraPos.z() - mpr_p_view->GetTerrainElevationOfFoucsPos();
		if (cameraPos.z() > 100000.0)
			hei = cameraPos.z();
		glbDouble delt = DistanceToDelt(hei);
		cursorPosY = (0.5-delt)*ElevationRulerHeight*0.5;

		if (y < cursorPosY + ElevationRulerHeight*0.03 && y > cursorPosY - ElevationRulerHeight*0.03) 
			return ELEVATIONRULER_CURSOR_BTN;
	}else if (x > -ElevationRulerWidth*0.36 && x < ElevationRulerWidth*0.36 && y > -ElevationRulerHeight*0.35 && y < ElevationRulerHeight*0.35) //其他位置
	{
		return ELEVATIONRULER_NONE_BTN;
	}else{									//导航标尺以外
		return ELEVATIONRULER_OUT_BTN;
	}
	return ELEVATIONRULER_OUT_BTN;
}

glbBool CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerEventHandler::FlyTo(glbDouble distan_target)
{ 
	//原来相机状态，焦点，相机位置
	osg::Vec3d focusPos;
	mpr_p_view->GetFocusPos(focusPos);
	osg::Vec3d cameraPos;
	mpr_p_view->GetCameraPos(cameraPos);
	glbDouble oldyaw = mpr_p_view->GetYaw();
	//目标相机状态，焦点不变
	//相机所在经纬度的地形高度
	glbDouble TerrainCameraAlt = mpr_p_view->GetTerrainElevationOfCameraPos();
	//相机目标位置
	osg::Vec3d newcameraPos(cameraPos.x(),cameraPos.y(),TerrainCameraAlt+distan_target);
	//由焦点，相机位置计算distan,yaw,pitch
	glbDouble distan,yaw,pitch;
	if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		osg::Matrixd localToworld;
		g_ellipsmodel->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),localToworld);
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(newcameraPos.y()),osg::DegreesToRadians(newcameraPos.x()),newcameraPos.z(),newcameraPos.x(),newcameraPos.y(),newcameraPos.z());
		osg::Vec3d cameraPos_local = newcameraPos * osg::Matrixd::inverse(localToworld);
		distan = cameraPos_local.length();
		cameraPos_local.normalize();
		glbDouble pit = min(max(cameraPos_local.z(),-1.0),1.0);
		pitch = osg::RadiansToDegrees(asin(-pit));
		pitch = min(pitch,10.0);
		//if (abs(pitch) > 89.9)
			yaw = oldyaw;
 		//else
 		//	yaw = osg::RadiansToDegrees(atan2(cameraPos_local.x(),cameraPos_local.y()));
	}else if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		osg::Vec3d lookat = focusPos-newcameraPos;
		distan = lookat.length();
		lookat.normalize();
		glbDouble pit = min(max(lookat.z(),-1.0),1.0);
		pitch = osg::RadiansToDegrees(asin(pit));
		pitch = min(pitch,10.0);
		//if (abs(pitch) > 89.9)
			yaw = oldyaw;
		//else
		//	yaw = osg::RadiansToDegrees(atan2(lookat.x(),lookat.y()));
	}

	glbDouble seconds = 0.0; 
	if (abs(cameraPos.z() - newcameraPos.z()) > 100000)
		seconds = 3.0;
	else
		seconds = 2.0;

	mpr_p_view->FlyTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,yaw,pitch,seconds);
	return true;
}

//glbBool CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerEventHandler::FlyTo(glbDouble distan_target)
//{ 
//	osg::Vec3d focusPos;
//	mpr_p_view->GetFocusPos(focusPos);
//	glbDouble distan = mpr_p_view->GetDistance();
//	glbDouble pitch = mpr_p_view->GetPitch();
//	glbDouble yaw = mpr_p_view->GetYaw();
//	glbDouble seconds = 2.0; 
//
//	if (distan_target < 0.0)
//	{
//		if (abs(focusPos.z() - mpr_p_view->GetGlobe()->GetUnderGroundAltitude()) < 10.0)	//焦点在地下
//		{
//			distan = mpr_p_view->GetGlobe()->GetUnderGroundAltitude() + distan_target - mpr_p_view->GetGlobe()->GetUnderGroundAltitude();
//		}else{
//			//飞到地下
//			distan = focusPos.z() + distan_target - mpr_p_view->GetGlobe()->GetUnderGroundAltitude();
//		}		
//		focusPos.z() = mpr_p_view->GetGlobe()->GetUnderGroundAltitude();
//		seconds = 3.0;
//		mpr_p_view->FlyTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,yaw,pitch,seconds);
//		return true;
//	}
//
//	if (abs(focusPos.z() - mpr_p_view->GetGlobe()->GetUnderGroundAltitude()) < 10.0)	//焦点在地下，把焦点放到地面
//		focusPos.z() = mpr_p_view->GetTerrainElevationOfCameraPos();
//	glbDouble multiple  = distan_target/distan;
//	if (multiple  < 1.2 && multiple > 0.8)	 
//		return false;
//	if (multiple > 1.0)
//	{ 	
//		glbInt32 multiple_i = (glbInt32)multiple;
//		if (multiple_i < multiple)
//			multiple_i++;
//		distan = distan*(glbDouble)multiple_i;
//		while(distan > distan_target*1.25)
//			distan *= 0.9;
//
//		pitch = -90;
//		seconds = multiple_i/10;
//		if (seconds > 3.0)
//			seconds = 3.0;
//		else if (seconds < 1.0)
//			seconds = 1.0;
// 		mpr_p_view->FlyTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,yaw,pitch,seconds);
//	}else if(multiple < 1.0)
//	{
//		multiple = 1/multiple;
//		glbInt32 multiple_i = (glbInt32)multiple;
//		if (multiple_i < multiple)
//			multiple_i++;
//		distan = distan/(glbDouble)multiple_i;
//		while(distan < distan_target*0.8333)
//			distan *= 1.1; 
//		pitch = -90;
//		seconds = multiple_i/10;
//		if (seconds > 3.0)
//			seconds = 3.0;
//		else if (seconds < 1.0)
//			seconds = 1.0;
//		mpr_p_view->FlyTo(focusPos.x(),focusPos.y(),focusPos.z(),distan,yaw,pitch,seconds);
//	}
//	return true;
//}

glbDouble CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerEventHandler::DeltToDistance(glbDouble delt)
{
	if (delt >= 1.0)
		return 10.0;
	if (delt <= 0.0)
		return 30000000.0;
	if (delt >0.0 && delt <= 0.05)
		return 30000000.0 - (30000000.0-15000000.0)*delt*20.0;
	if (delt >0.05 && delt <= 0.25)
		return 15000000.0 - (15000000.0-4500000.0)*(delt-0.05)*5.0;
	if (delt >0.25 && delt <= 0.4)
		return 4500000.0 - (4500000.0-800000.0)*(delt-0.25)/0.15;
	if (delt >0.4 && delt <= 0.6)
		return 800000.0 - (800000.0-10000.0)*(delt-0.4)*5.0;
	if (delt >0.6 && delt <= 0.75)
		return 10000.0 - (10000.0-1000.0)*(delt-0.6)/0.15;
	if (delt >0.75 && delt <= 0.9)
		return 1000.0 - (1000.0-150.0)*(delt-0.75)/0.15;
	if (delt >0.9 && delt < 1.0)
		return 150.0 - (150.0-10.0)*(delt-0.9)*10.0;
	return 30000000.0;
}

glbDouble CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerEventHandler::DistanceToDelt(glbDouble distan)
{
	//(返回值在(0，1)之间)
	if (distan > 30000000.0)
		return 0;
	if (distan <= 10.0)
		return 1;
	if (distan > 15000000.0 && distan <= 30000000.0)
		return 0.05 - 0.05*(distan-15000000.0)/(30000000.0-15000000.0); 
	if (distan > 4500000.0 && distan <= 15000000.0)
		return 0.25 - 0.2*(distan-4500000.0)/(15000000.0-4500000.0); 
	if (distan > 800000.0 && distan <= 4500000.0)
		return 0.4 - 0.15*(distan-800000.0)/(4500000.0-800000.0); 
	if (distan > 10000.0 && distan <= 800000.0)
		return 0.6 - 0.2*(distan-10000.0)/(800000.0-10000.0); 
	if (distan > 1000.0 && distan <= 10000.0)
		return 0.75 - 0.15*(distan-1000.0)/(10000.0-1000.0); 
	if (distan > 150.0 && distan <= 1000.0)
		return 0.9 - 0.15*(distan-150.0)/(1000.0-150.0);
	if (distan > 10.0 && distan <= 150)
		return 1.0-0.1*(distan-10.0)/(150.0-10.0);
	return 0;
}

void CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
	{
		osg::Camera* camera = dynamic_cast<osg::Camera*>(node);
		if (camera)
		{
			osg::Group* group = dynamic_cast<osg::Camera*>(camera);
			if (!mpr_p_glbView)
				return;
			if (!mpr_p_globe)
				return;
			osg::Geode* geodeElevationRuler = dynamic_cast<osg::Geode*>(group->getChild(0));
			osg::Geode* geodeElevationRulertext = dynamic_cast<osg::Geode*>(group->getChild(1));
			if (geodeElevationRuler && geodeElevationRulertext)
			{
				osg::Vec3d cameraPos;
				mpr_p_glbView->GetCameraPos(cameraPos);
				glbDouble hei = cameraPos.z() - mpr_p_glbView->GetTerrainElevationOfCameraPos();
				if (cameraPos.z() > 100000.0)
					hei = cameraPos.z();
				glbDouble delt = DistanceToDelt(hei);
				delt = -4.5 + 10.0*delt;//(delt在(-4.5，5.5)之间)
				//移动纹理1,重新设置纹理1
				osg::Geometry* geo = dynamic_cast<osg::Geometry*>(geodeElevationRuler->getDrawable(0));
				osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array;
				tc->push_back(osg::Vec2(-3.0,-7.0+delt));
				tc->push_back(osg::Vec2(1.0,-7.0+delt));
				tc->push_back(osg::Vec2(1.0,7.0+delt));
				tc->push_back(osg::Vec2(-3.0,7.0+delt));
				geo->setTexCoordArray(1,tc);
			}
		}
		traverse(node,nv);
	}
}

glbDouble CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRulerUpdateCallback::DistanceToDelt(glbDouble distan)
{
	//(返回值在(0，1)之间)
	if (distan >= 30000000.0)
		return 0;
	if (distan <= 10.0)
		return 1;
	if (distan >= 15000000.0 && distan < 30000000.0)
		return 0.05 - 0.05*(distan-15000000.0)/(30000000.0-15000000.0); 
	if (distan >= 4500000.0 && distan < 15000000.0)
		return 0.25 - 0.2*(distan-4500000.0)/(15000000.0-4500000.0); 
	if (distan >= 800000.0 && distan < 4500000.0)
		return 0.4 - 0.15*(distan-800000.0)/(4500000.0-800000.0); 
	if (distan >= 10000.0 && distan < 800000.0)
		return 0.6 - 0.2*(distan-10000.0)/(800000.0-10000.0); 
	if (distan >= 1000.0 && distan < 10000.0)
		return 0.75 - 0.15*(distan-1000.0)/(10000.0-1000.0); 
	if (distan >= 150.0 && distan < 1000.0)
		return 0.9 - 0.15*(distan-150.0)/(1000.0-150.0);
	if (distan > 10.0 && distan < 150)
		return 1.0-0.1*(distan-10.0)/(150.0-10.0);
	return 0;
}

CGlbGlobeViewElevationRuler::CGlbGlobeViewElevationRuler(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe)
{  
	osg::ref_ptr<osg::Geode> geode = createViewElevationRuler();
	osg::ref_ptr<osg::Geode> geodetext = createViewElevationRulerText();
	osg::BoundingSphere bsHeight = geode->computeBound();
	this->setClearMask( GL_DEPTH_BUFFER_BIT);
	this->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	osg::Vec3d upDirection( 0.0,0.0,1.0 );
	osg::Vec3d viewDirection( 0.0,-1.0,0.0 );
	double viewDistance = bsHeight.radius();
	osg::Vec3d center = bsHeight.center();

	osg::Vec3d eyePoint = center + viewDirection * viewDistance;

	this->setViewMatrixAsLookAt( eyePoint, center, upDirection );
	double znear = viewDistance - bsHeight.radius();
	double zfar = viewDistance + bsHeight.radius();
	float top = bsHeight.radius();
	float right = bsHeight.radius();
	SetPosition(0,0,width,height);
	this->setProjectionMatrixAsOrtho( -right, right, -top, top, znear, zfar );

	this->setRenderOrder(osg::Camera::POST_RENDER);
	this->setAllowEventFocus(false);

	this->setName("Navigator");
	this->addChild(geode.get());
	this->addChild(geodetext.get());

	osg::ref_ptr<CGlbGlobeViewElevationRulerUpdateCallback> up = new CGlbGlobeViewElevationRulerUpdateCallback(pview,pglobe);
	this->setUpdateCallback(up.get());
	osg::ref_ptr<CGlbGlobeViewElevationRulerEventHandler> vp = new CGlbGlobeViewElevationRulerEventHandler(pview,pglobe,geode.get());	
	this->setEventCallback(vp.get());
	this->setNodeMask(0x02);
}


CGlbGlobeViewElevationRuler::~CGlbGlobeViewElevationRuler(void)
{
}

void CGlbGlobeViewElevationRuler::SetPosition(int left,int top,int width,int height)
{ 
	this->setViewport(new osg::Viewport(width - ElevationRulerWidth*1.4 + left,height - ElevationRulerHeight*1.3 + top,ElevationRulerWidth,ElevationRulerHeight));
}

osg::ref_ptr<osg::Geode> CGlbGlobeViewElevationRuler::createViewElevationRuler()
{  
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry; 
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texcoord0 = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec2Array> texcoord1 = new osg::Vec2Array;
	vertices->push_back(osg::Vec3(-1.0,0.0,-1.0)); 
	vertices->push_back(osg::Vec3(1.0,0.0,-1.0));
	vertices->push_back(osg::Vec3(1.0,0.0,1.0));
	vertices->push_back(osg::Vec3(-1.0,0.0,1.0));
	geom->setVertexArray(vertices);
	texcoord0->push_back(osg::Vec2(-3.0,0.0));
	texcoord0->push_back(osg::Vec2(1.0,0.0));
	texcoord0->push_back(osg::Vec2(1.0,1.0));
	texcoord0->push_back(osg::Vec2(-3.0,1.0));
	geom->setTexCoordArray(0,texcoord0);
	texcoord1->push_back(osg::Vec2(-3.0,-7.0));
	texcoord1->push_back(osg::Vec2(1.0,-7.0));
	texcoord1->push_back(osg::Vec2(1.0,7.0));
	texcoord1->push_back(osg::Vec2(-3.0,7.0));
	geom->setTexCoordArray(1,texcoord1);
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,1.0f,0.0f));
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));
	geode->addDrawable(geom.get());

	HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
	CGlbWString dpath = CGlbPath::GetModuleDir(hmd);
	CGlbWString dpath0 = dpath + L"\\res\\005.png";
	osg::ref_ptr<osg::Image> image0 = osgDB::readImageFile(dpath0.ToString().c_str());

	osg::ref_ptr<osg::Texture2D> tex2d0 = new osg::Texture2D;
	if (image0.get())
		tex2d0->setImage(image0.get());
	tex2d0->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
	tex2d0->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
	//设置纹理环境
	osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
	texenv->setMode(osg::TexEnv::DECAL);//贴花
	//启用纹理单元0
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,tex2d0.get(),osg::StateAttribute::ON);
	//设置纹理环境
	geom->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());
	//////////////////////////////////////////////////////////////////////////
	CGlbWString dpath1 = dpath + L"\\res\\006.png";
	osg::ref_ptr<osg::Image> image1 = osgDB::readImageFile(dpath1.ToString().c_str());

	osg::ref_ptr<osg::Texture2D> tex2d1 = new osg::Texture2D;
	if (image1.get())
		tex2d1->setImage(image1.get());
	tex2d1->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
	tex2d1->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
	//启用纹理单元1
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(1,tex2d1.get(),osg::StateAttribute::ON);
	//设置纹理环境
	geom->getOrCreateStateSet()->setTextureAttribute(1,texenv.get());

	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON); 
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_COLOR,osg::BlendFunc::ONE_MINUS_SRC_COLOR,osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
	geode->getOrCreateStateSet()->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	geode->setName("ElevationRulergeode");
	geode->setNodeMask(0x02);
	return geode;
}


osg::ref_ptr<osg::Geode> CGlbGlobeViewElevationRuler::createViewElevationRulerText()
{
	//文字
	osg::ref_ptr<osgText::Font> font = new osgText::Font();
	font = osgText::readFontFile("fonts/simkai.ttf");
	osg::Vec4 fontcolor(0.8f,0.8f,0.8f,1.0f);
	osg::ref_ptr<osg::Geode> geodetext = new osg::Geode;
	osg::ref_ptr<osgText::Text> textearth = new osgText::Text;
	textearth->setFont(font.get());
	textearth->setText(L"全球");
	textearth->setCharacterSize(0.2f,0.5f); 
	textearth->setColor(fontcolor);
	textearth->setPosition(osg::Vec3f(0.2f,0.0f,0.65f));
	textearth->setAlignment(osgText::Text::RIGHT_CENTER);
	textearth->setAxisAlignment(osgText::Text::XZ_PLANE);
	textearth->setName("Earth");
	geodetext->addDrawable(textearth.get());

	osg::ref_ptr<osgText::Text> textcountry = new osgText::Text;
	textcountry->setFont(font.get());
	textcountry->setText(L"国家");
	textcountry->setCharacterSize(0.2f,0.5f);
	textcountry->setColor(fontcolor);
	textcountry->setPosition(osg::Vec3f(0.2f,0.0f,0.39f));
	textcountry->setAlignment(osgText::Text::RIGHT_CENTER);
	textcountry->setAxisAlignment(osgText::Text::XZ_PLANE);
	textcountry->setName("Country");
	geodetext->addDrawable(textcountry.get());

	osg::ref_ptr<osgText::Text> textprovince = new osgText::Text;
	textprovince->setFont(font.get());
	textprovince->setText(L"省级");
	textprovince->setCharacterSize(0.2f,0.5f);
	textprovince->setColor(fontcolor);
	textprovince->setPosition(osg::Vec3f(0.2f,0.0f,0.13f));
	textprovince->setAlignment(osgText::Text::RIGHT_CENTER);
	textprovince->setAxisAlignment(osgText::Text::XZ_PLANE);
	textprovince->setName("Province");
	geodetext->addDrawable(textprovince.get());

	osg::ref_ptr<osgText::Text> textcity = new osgText::Text;
	textcity->setFont(font.get());
	textcity->setText(L"城市");
	textcity->setCharacterSize(0.2f,0.5f);
	textcity->setColor(fontcolor);
	textcity->setPosition(osg::Vec3f(0.2f,0.0f,-0.13f));
	textcity->setAlignment(osgText::Text::RIGHT_CENTER);
	textcity->setAxisAlignment(osgText::Text::XZ_PLANE);
	textcity->setName("City");
	geodetext->addDrawable(textcity.get());

	osg::ref_ptr<osgText::Text> textstreet = new osgText::Text;
	textstreet->setFont(font.get());
	textstreet->setText(L"街道");
	textstreet->setCharacterSize(0.2f,0.5f);
	textstreet->setColor(fontcolor);
	textstreet->setPosition(osg::Vec3f(0.2f,0.0f,-0.38f));
	textstreet->setAlignment(osgText::Text::RIGHT_CENTER);
	textstreet->setAxisAlignment(osgText::Text::XZ_PLANE);
	textstreet->setName("Street");
	geodetext->addDrawable(textstreet.get());

	osg::ref_ptr<osgText::Text> texthouse = new osgText::Text;
	texthouse->setFont(font.get());
	texthouse->setText(L"房屋");
	texthouse->setCharacterSize(0.2f,0.5f);
	texthouse->setColor(fontcolor);
	texthouse->setPosition(osg::Vec3f(0.2f,0.0f,-0.65f));
	texthouse->setAlignment(osgText::Text::RIGHT_CENTER);
	texthouse->setAxisAlignment(osgText::Text::XZ_PLANE);
	texthouse->setName("House");
	geodetext->addDrawable(texthouse.get());

	osg::ref_ptr<osgText::Text> textUG = new osgText::Text;
	textUG->setFont(font.get());
	textUG->setText(L"地下");
	textUG->setCharacterSize(0.2f,0.5f);
	textUG->setColor(fontcolor);
	textUG->setPosition(osg::Vec3f(0.2f,0.0f,-0.9f));
	textUG->setAlignment(osgText::Text::RIGHT_CENTER);
	textUG->setAxisAlignment(osgText::Text::XZ_PLANE);
	textUG->setName("UnderGround");
	geodetext->addDrawable(textUG.get());

	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::DST_ALPHA );	//设置混合方程
	geodetext->getOrCreateStateSet()->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	geodetext->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geodetext->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geodetext->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON); 
	geodetext->setName("ElevationRulerTextgeode");
	geodetext->setNodeMask(0x02);
	return geodetext;
}
