#include "StdAfx.h"
#include "GlbGlobeViewNavigator.h"
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/TexEnv>
#include <osg/BlendFunc>
#include <osg/TexGen>
using namespace GlbGlobe;

#define NaviagatorSize 160

CGlbGlobeViewNavigator::CGlbGlobeViewNavigator(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe)
{
	osg::ref_ptr<osg::Geode> geode = createViewNavigator();
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
	osg::ref_ptr<CGlbGlobeViewNavigatorUpdateCallback> vp = new CGlbGlobeViewNavigatorUpdateCallback(pview,pglobe);	
	this->setUpdateCallback(vp.get());
	osg::ref_ptr<CGlbGlobeViewNavigatorEventHandler> ep = new CGlbGlobeViewNavigatorEventHandler(pview,geode.get());
	this->setEventCallback(ep.get());
	this->setNodeMask(0x02);
}


CGlbGlobeViewNavigator::~CGlbGlobeViewNavigator(void)
{
}

glbBool CGlbGlobeViewNavigator::CGlbGlobeViewNavigatorEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	osg::Vec2d centerPos(NaviagatorSize*0.52,NaviagatorSize*0.52);
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::FRAME):
		{
			switch(moveflag)
			{
			case 0:
				return false;
			case 1:
				{  
					osg::Vec3d cameraPos;
					mpr_p_view->GetCameraPos(cameraPos);
					if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					{
						glbDouble angle = cameraPos.z()*0.000001 > 1? 1 : cameraPos.z()*0.000001; 
						mpr_p_view->Move(angle,0.0);
					}else if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					{
						glbDouble delt = cameraPos.z()*0.001;
						mpr_p_view->Move(delt,0.0);
					}					
					aa.requestRedraw();
					return true;
				}
			case 2:	
				{  
					osg::Vec3d cameraPos;
					mpr_p_view->GetCameraPos(cameraPos);
					if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					{	
						glbDouble angle = cameraPos.z()*0.000001 > 1? 1 : cameraPos.z()*0.000001;
						mpr_p_view->Move(-angle,0.0);
					}else if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					{
						glbDouble delt = cameraPos.z()*0.001;
						mpr_p_view->Move(-delt,0.0);
					}
					aa.requestRedraw();
					return true;
				}
			case 3:
				{  
					osg::Vec3d cameraPos;
					mpr_p_view->GetCameraPos(cameraPos);
					if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					{	 
						glbDouble angle = cameraPos.z()*0.000001 > 1? 1 : cameraPos.z()*0.000001;
						mpr_p_view->Move(0.0,angle);
					}else if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					{
						glbDouble delt = cameraPos.z()*0.001;
						mpr_p_view->Move(0.0,delt);
					} 
					aa.requestRedraw();
					return true;
				}
			case 4:
				{  
					osg::Vec3d cameraPos;
					mpr_p_view->GetCameraPos(cameraPos);
					if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					{	
						glbDouble angle = cameraPos.z()*0.000001 > 1? 1 : cameraPos.z()*0.000001;
						mpr_p_view->Move(0.0,-angle); 
					}else if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					{
						glbDouble delt = cameraPos.z()*0.001;
						mpr_p_view->Move(0.0,-delt);
					}
					aa.requestRedraw();
					return true;
				}
			case 5:
				{
					glbDouble anglex = ea.getX() - ea.getXmax() + centerPos.x();
					glbDouble angley = ea.getY() - ea.getYmax() + centerPos.y();
					glbDouble distan = sqrt(anglex*anglex + angley*angley);
					if (distan < NaviagatorSize*0.15)
					{
						distan = NaviagatorSize*0.15;
					}
					anglex /= distan;
					angley /= distan;

					osg::Vec3d cameraPos;
					mpr_p_view->GetCameraPos(cameraPos);
					glbDouble coefficient = 0.0;
					if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_GLOBE)
					{ 
						coefficient = cameraPos.z()*0.000001 > 1? 1 : cameraPos.z()*0.000001;
					}else if (mpr_p_view->GetGlobe()->GetType() == GLB_GLOBETYPE_FLAT)
					{
						coefficient = cameraPos.z()*0.001;
					} 				
					anglex *= coefficient;
					angley *= coefficient;

					mpr_p_view->Move(-anglex,angley);
					aa.requestRedraw();
					return true;
				}
			default:
				return false;
			}
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
			_ga_t0 = &ea;
			switch(getNavigatorPositionType(ea))
			{
			case NAVIGATOR_LEFT_BTN:
			case NAVIGATOR_RIGHT_BTN:
			case NAVIGATOR_UP_BTN:
			case NAVIGATOR_DOWN_BTN:
			case NAVIGATOR_CENTER_BTN:
			case NAVIGATOR_YAW_BTN:
			case NAVIGATOR_PITCH_BTN:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_HAND));	//改变光标，大手形
					return false;
				}
				break;
			case NAVIGATOR_NONE_BTN:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_ARROW));	//改变光标，箭头
					return false;
				}
				break;
			default:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_ARROW));	//改变光标，箭头
					return false;
				}
				break;
			}
		}
		break;
	case osgGA::GUIEventAdapter::PUSH:
		{
			_ga_t0 = &ea;
			push_pitch = mpr_p_view->GetPitch();	//记录鼠标按下时，pitch角
			push_pitch = osg::DegreesToRadians(push_pitch);
			push_pitch = push_pitch > 0? 0 : push_pitch;
			switch(getNavigatorPositionType(ea))
			{
			case NAVIGATOR_LEFT_BTN:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_CROSS));	//改变光标，小手形
					moveflag = 1;
					return true;
				}
				break;
			case NAVIGATOR_RIGHT_BTN:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_CROSS));	//改变光标，小手形
					moveflag = 2;
					return true;
				}
				break;
			case NAVIGATOR_UP_BTN:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_CROSS));	//改变光标，小手形
					moveflag = 3;
					return true;
				}
				break;
			case NAVIGATOR_DOWN_BTN:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_CROSS));	//改变光标，小手形
					moveflag = 4;
					return true;
				}
				break;
			case NAVIGATOR_CENTER_BTN:
			case NAVIGATOR_YAW_BTN:
				{
					_ga_t1 = &ea;
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_CROSS));	//改变光标，小手形
					return false;
				}
			case NAVIGATOR_PITCH_BTN:	
				{
					_ga_t1 = &ea;
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_CROSS));	//改变光标，小手形
					return false;
				}
				break;
			default:
				break;
			}
		}
		break;
	case osgGA::GUIEventAdapter::RELEASE:
		{
			_ga_t0 = &ea;
			_ga_t1 = NULL;
			moveflag = 0;
			push_pitch = mpr_p_view->GetPitch();	//记录鼠标按下时，pitch角
			push_pitch = osg::DegreesToRadians(push_pitch);
			push_pitch = push_pitch > 0? 0 : push_pitch;

			//移动纹理1,重新设置纹理1
			osg::Geometry* geo = dynamic_cast<osg::Geometry*>(_geode->getDrawable(0));
			osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array;
			tc->push_back(osg::Vec2(0.0,0.0));
			tc->push_back(osg::Vec2(1.0,0.0));
			tc->push_back(osg::Vec2(1.0,1.0));
			tc->push_back(osg::Vec2(0.0,1.0));
			geo->setTexCoordArray(1,tc);

			switch(getNavigatorPositionType(ea))
			{
			case NAVIGATOR_LEFT_BTN:
			case NAVIGATOR_RIGHT_BTN:
			case NAVIGATOR_UP_BTN:
			case NAVIGATOR_DOWN_BTN:
			case NAVIGATOR_CENTER_BTN:
			case NAVIGATOR_YAW_BTN:
			case NAVIGATOR_PITCH_BTN:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_HAND));	//改变光标，大手形
					return false;
				}
				break;
			case NAVIGATOR_NONE_BTN:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_ARROW));	//改变光标，箭头
					return false;
				}
			default:
				{
					mpr_p_view->SetCursor(::LoadCursor(NULL,IDC_ARROW));	//改变光标，箭头
					return false;
				}
				break;
			}
			return false;
		}
		break;
	case osgGA::GUIEventAdapter::DRAG:
		{
			if (_ga_t1 == NULL)
				return false;
			switch(getNavigatorPositionType(*_ga_t1))
			{
			case NAVIGATOR_CENTER_BTN:		//中心点Move
				{
					_ga_t0 = &ea;
					glbDouble anglex = ea.getX() - ea.getXmax() + centerPos.x();
					glbDouble angley = ea.getY() - ea.getYmax() + centerPos.y();
					glbDouble distan = sqrt(anglex*anglex + angley*angley);
					if (distan < NaviagatorSize*0.15)
					{
						distan = NaviagatorSize*0.15;
					}
					anglex /= distan;
					angley /= distan;
					//mpr_p_view->Move(anglex,angley);
					anglex *= 0.12;
					angley *= 0.12;
					//移动纹理1,重新设置纹理1
					osg::Geometry* geo = dynamic_cast<osg::Geometry*>(_geode->getDrawable(0));
					osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array;
					tc->push_back(osg::Vec2(-anglex,-angley));
					tc->push_back(osg::Vec2(1.0-anglex,-angley));
					tc->push_back(osg::Vec2(1.0-anglex,1.0-angley));
					tc->push_back(osg::Vec2(-anglex,1.0-angley));
					geo->setTexCoordArray(1,tc);

					aa.requestRedraw();
					moveflag = 5;
					return true;
				}
				break;
			case NAVIGATOR_YAW_BTN:			//Yaw角
				{
					if (_ga_t1 == NULL)
						return false;
					glbDouble yaw = mpr_p_view->GetYaw();
					osg::Vec2d vec0,vec1;
					vec0.x() = ea.getX() - ea.getXmax() + centerPos.x(); 
					vec0.y() = ea.getY() - ea.getYmax() + centerPos.y();
					glbDouble angle0 = atan2(vec0.y(),vec0.x());
					vec1.x() = _ga_t0->getX() - _ga_t0->getXmax() + centerPos.x();
					vec1.y() = _ga_t0->getY() - _ga_t0->getYmax() + centerPos.y();
					glbDouble angle1 = atan2(vec1.y(),vec1.x());
					glbDouble angle = osg::RadiansToDegrees(angle0 - angle1);
 					angle += yaw;
					osg::Vec3d oldfocus;
					mpr_p_view->GetFocusPos(oldfocus);
					glbDouble distan = mpr_p_view->GetDistance();
					mpr_p_view->SetYaw(angle,oldfocus,distan);
					_ga_t0 = &ea;
					aa.requestRedraw();
					return true;
				}
				break;
			case NAVIGATOR_PITCH_BTN:		//pitch角
				{
					if (_ga_t1 == NULL)
						return false;
					_ga_t0 = &ea;
					glbDouble x = ea.getX() - ea.getXmax() + centerPos.x(); 
					glbDouble y = ea.getY() - ea.getYmax() + centerPos.y();
					glbDouble pitch = atan2(y,x);
					if (pitch > osg::PI_2)
					{
						pitch = -90.0;
					}else if (pitch < 0)
					{
						pitch = 0.0;
					}else{
						pitch = -osg::RadiansToDegrees(pitch);
					}
					osg::Vec3d oldfocus;
					mpr_p_view->GetFocusPos(oldfocus);
					glbDouble distan = mpr_p_view->GetDistance();
 					mpr_p_view->SetPitch(pitch,oldfocus,distan);
					aa.requestRedraw();
					return true;
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return false;
}

glbBool CGlbGlobeViewNavigator::CGlbGlobeViewNavigatorEventHandler::handleTouch(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	osg::Vec2d centerPos(NaviagatorSize*0.52,NaviagatorSize*0.52);
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
				push_pitch = mpr_p_view->GetPitch();	//记录鼠标按下时，pitch角
				push_pitch = osg::DegreesToRadians(push_pitch);
				push_pitch = push_pitch > 0? 0 : push_pitch;
				switch(getTouchNavigatorPositionType(ea))
				{
				case NAVIGATOR_LEFT_BTN:
					{
						moveflag = 1;
						return true;
					}
					break;
				case NAVIGATOR_RIGHT_BTN:
					{
						moveflag = 2;
						return true;
					}
					break;
				case NAVIGATOR_UP_BTN:
					{
						moveflag = 3;
						return true;
					}
					break;
				case NAVIGATOR_DOWN_BTN:
					{
						moveflag = 4;
						return true;
					}
					break;
				case NAVIGATOR_CENTER_BTN:
				case NAVIGATOR_YAW_BTN:
					{
						_ga_t1 = &ea;
						return false;
					}
				case NAVIGATOR_PITCH_BTN:	
					{
						_ga_t1 = &ea;
						return false;
					}
					break;
				default:
					break;
				}
			}
			break;
		case osgGA::GUIEventAdapter::TOUCH_MOVED:	//move
			{
				if (_ga_t1 == NULL)
					return false;
				switch(getTouchNavigatorPositionType(*_ga_t1))
				{
				case NAVIGATOR_CENTER_BTN:		//中心点Move
					{
						_ga_t0 = &ea;
						glbDouble tx = ea.getTouchData()->get(0).x;
						glbDouble ty = ea.getTouchData()->get(0).y;
						mpr_p_view->WindowToScene(tx,ty);
						glbDouble anglex = tx - ea.getXmax() + centerPos.x();
						glbDouble angley = ty - ea.getYmax() + centerPos.y();
						glbDouble distan = sqrt(anglex*anglex + angley*angley);
						if (distan < NaviagatorSize*0.15)
						{
							distan = NaviagatorSize*0.15;
						}
						anglex /= distan;
						angley /= distan;
						//mpr_p_view->Move(anglex,angley);
						anglex *= 0.12;
						angley *= 0.12;
						//移动纹理1,重新设置纹理1
						osg::Geometry* geo = dynamic_cast<osg::Geometry*>(_geode->getDrawable(0));
						osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array;
						tc->push_back(osg::Vec2(-anglex,-angley));
						tc->push_back(osg::Vec2(1.0-anglex,-angley));
						tc->push_back(osg::Vec2(1.0-anglex,1.0-angley));
						tc->push_back(osg::Vec2(-anglex,1.0-angley));
						geo->setTexCoordArray(1,tc);

						aa.requestRedraw();
						moveflag = 5;
						return true;
					}
					break;
				case NAVIGATOR_YAW_BTN:			//Yaw角
					{
						if (_ga_t1 == NULL)
							return false;
						glbDouble yaw = mpr_p_view->GetYaw();
						osg::Vec2d vec0,vec1;
						glbDouble tx = ea.getTouchData()->get(0).x;
						glbDouble ty = ea.getTouchData()->get(0).y;
						mpr_p_view->WindowToScene(tx,ty);
						glbDouble tx0 = _ga_t0->getTouchData()->get(0).x;
						glbDouble ty0 = _ga_t0->getTouchData()->get(0).y;
						mpr_p_view->WindowToScene(tx0,ty0);
						vec0.x() = tx - ea.getXmax() + centerPos.x(); 
						vec0.y() = ty - ea.getYmax() + centerPos.y();
						glbDouble angle0 = atan2(vec0.y(),vec0.x());
						vec1.x() = tx0 - _ga_t0->getXmax() + centerPos.x();
						vec1.y() = ty0 - _ga_t0->getYmax() + centerPos.y();
						glbDouble angle1 = atan2(vec1.y(),vec1.x());
						glbDouble angle = osg::RadiansToDegrees(angle0 - angle1);
						angle += yaw;
						osg::Vec3d oldfocus;
						mpr_p_view->GetFocusPos(oldfocus);
						glbDouble distan = mpr_p_view->GetDistance();
						mpr_p_view->SetYaw(angle,oldfocus,distan);
						_ga_t0 = &ea;
						aa.requestRedraw();
						return true;
					}
					break;
				case NAVIGATOR_PITCH_BTN:		//pitch角
					{
						if (_ga_t1 == NULL)
							return false;
						_ga_t0 = &ea;
						glbDouble tx = ea.getTouchData()->get(0).x;
						glbDouble ty = ea.getTouchData()->get(0).y;
						mpr_p_view->WindowToScene(tx,ty);
						glbDouble x = tx - ea.getXmax() + centerPos.x(); 
						glbDouble y = ty - ea.getYmax() + centerPos.y();
						glbDouble pitch = atan2(y,x);
						if (pitch > osg::PI_2)
						{
							pitch = -90.0;
						}else if (pitch < 0)
						{
							pitch = 0.0;
						}else{
							pitch = -osg::RadiansToDegrees(pitch);
						}
						osg::Vec3d oldfocus;
						mpr_p_view->GetFocusPos(oldfocus);
						glbDouble distan = mpr_p_view->GetDistance();
						mpr_p_view->SetPitch(pitch,oldfocus,distan);
						aa.requestRedraw();
						return true;
					}
					break;
				default:
					break;
				}
			}
			break;		
		case osgGA::GUIEventAdapter::TOUCH_ENDED:	//end
			{
				_ga_t0 = &ea;
				_ga_t1 = NULL;
				moveflag = 0;
				push_pitch = mpr_p_view->GetPitch();	//记录鼠标按下时，pitch角
				push_pitch = osg::DegreesToRadians(push_pitch);
				push_pitch = push_pitch > 0? 0 : push_pitch;

				//移动纹理1,重新设置纹理1
				osg::Geometry* geo = dynamic_cast<osg::Geometry*>(_geode->getDrawable(0));
				osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array;
				tc->push_back(osg::Vec2(0.0,0.0));
				tc->push_back(osg::Vec2(1.0,0.0));
				tc->push_back(osg::Vec2(1.0,1.0));
				tc->push_back(osg::Vec2(0.0,1.0));
				geo->setTexCoordArray(1,tc);
			}
			break;
		}
	}
	return false;
}

GlbGlobeNavigatorPositionType CGlbGlobeViewNavigator::CGlbGlobeViewNavigatorEventHandler::getNavigatorPositionType(const osgGA::GUIEventAdapter& ea)
{
	//在导航盘坐标系下，以导航盘中心为原点，x轴正方向向右，y轴正方向向左,导航盘坐标范围为((-80,80),(-80,80))
	osg::Vec2d centerPos(NaviagatorSize*0.52,NaviagatorSize*0.52);
	glbFloat x = ea.getX() - ea.getXmax() + centerPos.x();
	glbFloat y = ea.getY() - ea.getYmax() + centerPos.y();
	glbFloat pitchPosx = NaviagatorSize * 0.335 * cos(push_pitch);
	glbFloat pitchPosy = NaviagatorSize * 0.335 * sin(-push_pitch);
	if (x < NaviagatorSize*0.05  && x > -NaviagatorSize*0.05 && y < NaviagatorSize*0.05 && y > -NaviagatorSize*0.05)	//中心点
	{
		return NAVIGATOR_CENTER_BTN;
	}else if (x < -NaviagatorSize*0.14 && x > -NaviagatorSize*0.19 && y < NaviagatorSize*0.04 && y > -NaviagatorSize*0.04)	//left
	{
		return NAVIGATOR_LEFT_BTN;
	}else if (x > NaviagatorSize*0.14 && x < NaviagatorSize*0.19 && y < NaviagatorSize*0.04 && y > -NaviagatorSize*0.04)		//right
	{
		return NAVIGATOR_RIGHT_BTN;
	}else if (y < -NaviagatorSize*0.14 && y > -NaviagatorSize*0.19 && x < NaviagatorSize*0.04 && x > -NaviagatorSize*0.04)	//down
	{
		return NAVIGATOR_DOWN_BTN;
	}else if (y > NaviagatorSize*0.14 && y < NaviagatorSize*0.19 && x < NaviagatorSize*0.04 && x > -NaviagatorSize*0.04)		//up
	{
		return NAVIGATOR_UP_BTN;
	}else if (((x*x+y*y) > (NaviagatorSize*0.22)*(NaviagatorSize*0.22)) && ((x*x+y*y) < (NaviagatorSize*0.28)*(NaviagatorSize*0.28)) )	//yaw
	{
		return NAVIGATOR_YAW_BTN;
	}else if ((x-pitchPosx) < NaviagatorSize*0.02 && (x-pitchPosx) > -NaviagatorSize*0.02 && (y-pitchPosy) < NaviagatorSize*0.02 && (y-pitchPosy) > -NaviagatorSize*0.02)	//pitch
	{
		return NAVIGATOR_PITCH_BTN;
	}else if(x < NaviagatorSize*0.45 && x > -NaviagatorSize*0.45 && y < NaviagatorSize*0.45 && y > -NaviagatorSize*0.45) 
	{
		return NAVIGATOR_NONE_BTN;
	}else{
		return NAVIGATOR_OUT_BTN;
	}
}

GlbGlobeNavigatorPositionType CGlbGlobeViewNavigator::CGlbGlobeViewNavigatorEventHandler::getTouchNavigatorPositionType(const osgGA::GUIEventAdapter& ea)
{
	//在导航盘坐标系下，以导航盘中心为原点，x轴正方向向右，y轴正方向向左,导航盘坐标范围为((-80,80),(-80,80))
	osg::Vec2d centerPos(NaviagatorSize*0.52,NaviagatorSize*0.52);
	glbDouble tx = ea.getTouchData()->get(0).x;
	glbDouble ty = ea.getTouchData()->get(0).y;
	mpr_p_view->WindowToScene(tx,ty);
	glbFloat x = tx - ea.getXmax() + centerPos.x();
	glbFloat y = ty - ea.getYmax() + centerPos.y();
	glbFloat pitchPosx = NaviagatorSize * 0.335 * cos(push_pitch);
	glbFloat pitchPosy = NaviagatorSize * 0.335 * sin(-push_pitch);
	if (x < NaviagatorSize*0.05  && x > -NaviagatorSize*0.05 && y < NaviagatorSize*0.05 && y > -NaviagatorSize*0.05)	//中心点
	{
		return NAVIGATOR_CENTER_BTN;
	}else if (x < -NaviagatorSize*0.14 && x > -NaviagatorSize*0.19 && y < NaviagatorSize*0.04 && y > -NaviagatorSize*0.04)	//left
	{
		return NAVIGATOR_LEFT_BTN;
	}else if (x > NaviagatorSize*0.14 && x < NaviagatorSize*0.19 && y < NaviagatorSize*0.04 && y > -NaviagatorSize*0.04)		//right
	{
		return NAVIGATOR_RIGHT_BTN;
	}else if (y < -NaviagatorSize*0.14 && y > -NaviagatorSize*0.19 && x < NaviagatorSize*0.04 && x > -NaviagatorSize*0.04)	//down
	{
		return NAVIGATOR_DOWN_BTN;
	}else if (y > NaviagatorSize*0.14 && y < NaviagatorSize*0.19 && x < NaviagatorSize*0.04 && x > -NaviagatorSize*0.04)		//up
	{
		return NAVIGATOR_UP_BTN;
	}else if (((x*x+y*y) > (NaviagatorSize*0.22)*(NaviagatorSize*0.22)) && ((x*x+y*y) < (NaviagatorSize*0.28)*(NaviagatorSize*0.28)) )	//yaw
	{
		return NAVIGATOR_YAW_BTN;
	}else if ((x-pitchPosx) < NaviagatorSize*0.02 && (x-pitchPosx) > -NaviagatorSize*0.02 && (y-pitchPosy) < NaviagatorSize*0.02 && (y-pitchPosy) > -NaviagatorSize*0.02)	//pitch
	{
		return NAVIGATOR_PITCH_BTN;
	}else if(x < NaviagatorSize*0.45 && x > -NaviagatorSize*0.45 && y < NaviagatorSize*0.45 && y > -NaviagatorSize*0.45) 
	{
		return NAVIGATOR_NONE_BTN;
	}else{
		return NAVIGATOR_OUT_BTN;
	}
}

void CGlbGlobeViewNavigator::CGlbGlobeViewNavigatorUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
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
			osg::Geode* geodeNavigator = dynamic_cast<osg::Geode*>(group->getChild(0));
			if (geodeNavigator)
			{ 
				osg::Geometry* geo = dynamic_cast<osg::Geometry*>(geodeNavigator->getDrawable(0)); 
				glbDouble yaw = mpr_p_glbView->GetYaw();
				yaw = osg::DegreesToRadians(yaw);
				//旋转纹理0,重设纹理坐标
				//原纹理坐标绕(0.48,0.48)点垂线旋转yaw角
				osg::Vec2 rotate_center(0.48,0.47);	//旋转中心
				osg::ref_ptr<osg::Vec2Array> tc = new osg::Vec2Array;
				tc->push_back(osg::Vec2(0.0,0.0));
				tc->push_back(osg::Vec2(1.0,0.0));
				tc->push_back(osg::Vec2(1.0,1.0));
				tc->push_back(osg::Vec2(0.0,1.0));				
				for (size_t i = 0; i < tc->size(); i++)
				{
					(*tc)[i] -= rotate_center;
					glbDouble r = (*tc)[i].length();
					glbDouble ang = atan2((*tc)[i].y(),(*tc)[i].x()) - yaw;
					(*tc)[i] = osg::Vec2(r*cos(ang),r*sin(ang));
					(*tc)[i] += rotate_center;
				}
				geo->setTexCoordArray(0,tc);
				glbDouble pitch = mpr_p_glbView->GetPitch();
				pitch = osg::DegreesToRadians(pitch) + osg::PI_2;
				if ( pitch > osg::PI_2)
					pitch = osg::PI_2;
				//旋转纹理3
				osg::ref_ptr<osg::Vec2Array> tc3 = new osg::Vec2Array;
				tc3->push_back(osg::Vec2(0.0,0.0));
				tc3->push_back(osg::Vec2(1.0,0.0));
				tc3->push_back(osg::Vec2(1.0,1.0));
				tc3->push_back(osg::Vec2(0.0,1.0));
				for (size_t i = 0; i < tc3->size(); i++)
				{
					(*tc3)[i] -= rotate_center;
					glbDouble r = (*tc3)[i].length();
					glbDouble ang = atan2((*tc3)[i].y(),(*tc3)[i].x()) + pitch;
					(*tc3)[i] = osg::Vec2(r*cos(ang),r*sin(ang));
					(*tc3)[i] += rotate_center;
				}
				geo->setTexCoordArray(3,tc3);
			}
		}
		traverse(node,nv);
	}
}

void CGlbGlobeViewNavigator::SetPosition(int left,int top,int width,int height)
{
	this->setViewport(new osg::Viewport(width - NaviagatorSize + left,height - NaviagatorSize + top,NaviagatorSize,NaviagatorSize));	//右上角
}

osg::ref_ptr<osg::Geode> CGlbGlobeViewNavigator::createViewNavigator()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry; 
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texcoord0 = new osg::Vec2Array;
	vertices->push_back(osg::Vec3(-1.0,0.0,-1.0)); 
	vertices->push_back(osg::Vec3(1.0,0.0,-1.0));
	vertices->push_back(osg::Vec3(1.0,0.0,1.0));
	vertices->push_back(osg::Vec3(-1.0,0.0,1.0));
	geom->setVertexArray(vertices);
	texcoord0->push_back(osg::Vec2(0.0,0.0));
	texcoord0->push_back(osg::Vec2(1.0,0.0));
	texcoord0->push_back(osg::Vec2(1.0,1.0));
	texcoord0->push_back(osg::Vec2(0.0,1.0));
	geom->setTexCoordArray(0,texcoord0);
	geom->setTexCoordArray(1,texcoord0);
	geom->setTexCoordArray(2,texcoord0);
	geom->setTexCoordArray(3,texcoord0);
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

	//////////////////////////////////////////////////////////////////////////
	HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
	CGlbWString dpath = CGlbPath::GetModuleDir(hmd);

	CGlbWString dpath0 = dpath + L"\\res\\004.png";
	osg::ref_ptr<osg::Image> image0 = osgDB::readImageFile(dpath0.ToString().c_str());
	
	osg::ref_ptr<osg::Texture2D> tex2d0 = new osg::Texture2D;
	if (image0.get())
		tex2d0->setImage(image0.get());
	tex2d0->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP);
	tex2d0->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP);
	//设置纹理环境
	osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
	texenv->setMode(osg::TexEnv::DECAL);//贴花
	//启用纹理单元0
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,tex2d0.get(),osg::StateAttribute::ON);
	//设置纹理环境
	geom->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());

	//////////////////////////////////////////////////////////////////////////
	CGlbWString dpath1 = dpath + L"\\res\\003.png";
	osg::ref_ptr<osg::Image> image1 = osgDB::readImageFile(dpath1.ToString().c_str());
	osg::ref_ptr<osg::Texture2D> tex2d1 = new osg::Texture2D;
	if (image1.get())
		tex2d1->setImage(image1.get());
	tex2d1->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP);
	tex2d1->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP);
	//启用纹理单元1
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(1,tex2d1.get(),osg::StateAttribute::ON);
	//设置纹理环境
	geom->getOrCreateStateSet()->setTextureAttribute(1,texenv.get());

	//////////////////////////////////////////////////////////////////////////
	CGlbWString dpath2 = dpath + L"\\res\\001.png";
	osg::ref_ptr<osg::Image> image2 = osgDB::readImageFile(dpath2.ToString().c_str());
	osg::ref_ptr<osg::Texture2D> tex2d2 = new osg::Texture2D;
	if (image2.get())
		tex2d2->setImage(image2.get());
	tex2d2->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
	tex2d2->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
	//启用纹理单元1
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(2,tex2d2.get(),osg::StateAttribute::ON);
	//设置纹理环境
	geom->getOrCreateStateSet()->setTextureAttribute(2,texenv.get());

	//////////////////////////////////////////////////////////////////////////
	CGlbWString dpath3 = dpath + L"\\res\\002.png";
	osg::ref_ptr<osg::Image> image3 = osgDB::readImageFile(dpath3.ToString().c_str());
	osg::ref_ptr<osg::Texture2D> tex2d3 = new osg::Texture2D;
	if (image3.get())
		tex2d3->setImage(image3.get());
	tex2d3->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP);
	tex2d3->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP);
	//启用纹理单元1
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(3,tex2d3.get(),osg::StateAttribute::ON);
	//设置纹理环境
	geom->getOrCreateStateSet()->setTextureAttribute(3,texenv.get());

	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	//osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::DST_ALPHA );	//设置混合方程
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_COLOR,osg::BlendFunc::ONE_MINUS_SRC_COLOR,osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
	geode->getOrCreateStateSet()->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	geode->setName("Navigatorgeode");
	geode->setNodeMask(0x02);
	return geode;
}