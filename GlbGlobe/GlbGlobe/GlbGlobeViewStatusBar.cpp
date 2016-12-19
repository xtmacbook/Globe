#include "StdAfx.h"
#include "GlbGlobeViewStatusBar.h"
#include <osgText/Text>
#include <osgText/Font>
#include <osg/BlendFunc>
#include <osg/BlendColor>

using namespace GlbGlobe;  

extern glbDouble testx;
extern glbDouble testy;

CGlbGlobeViewStatusBar::CGlbGlobeViewStatusBar(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe)
{
	osg::ref_ptr<osg::Geode> geode = createViewStatusBar();
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
	//this->setViewport(new osg::Viewport(0,0,width,25));
	SetPosition(0,0,width,height);
	this->setProjectionMatrixAsOrtho( -right * 1.0f, right * 1.0f, -top * 0.015f, top * 0.015f, znear, zfar );
	this->setRenderOrder(osg::Camera::POST_RENDER);
	this->setAllowEventFocus(false);

	this->setName("StatusBar");
	this->addChild(geode.get());
	osg::ref_ptr<CGlbGlobeViewStatusBarUpdateCallback> vp = new CGlbGlobeViewStatusBarUpdateCallback();
	vp->Put_GlbGlobe(pglobe);
	vp->Put_GlbGlobeView(pview);	
	this->setUpdateCallback(vp.get());
	this->setNodeMask(0x02);
}


CGlbGlobeViewStatusBar::~CGlbGlobeViewStatusBar(void)
{
}

void CGlbGlobeViewStatusBar::CGlbGlobeViewStatusBarUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
	{
		osg::Camera* camera = dynamic_cast<osg::Camera*>(node);
		if (camera && mpr_p_glbView && mpr_p_globe)
		{
			osg::Group* group = dynamic_cast<osg::Camera*>(camera);
//			std::list< osg::ref_ptr<osgGA::GUIEventHandler> >::iterator i;
//			std::list< osg::ref_ptr<osgGA::GUIEventHandler> > eventhandlers = mpr_p_glbView->GetEventHandle();
// 			glbDouble mouselon = 0.0;
// 			glbDouble mouselat = 0.0;
// 			for(i = eventhandlers.begin(); i != eventhandlers.end(); ++i)
// 			{
// 				osg::ref_ptr<osgGA::GUIEventHandler> gui = *i;
// 				//CGlbGlobeViewElementPickHandler* ph = dynamic_cast<CGlbGlobeViewElementPickHandler*>(gui.get());
// 				if(ph)
// 				{						
// 					mouselon = ph->GetLon();
// 					mouselat = ph->GetLat();
// 					break;
// 				}
// 			}
			osg::Vec3d cameraPos;
			mpr_p_glbView->GetCameraPos(cameraPos);
			osg::Vec3d center;
			mpr_p_glbView->GetFocusPos(center);		

			bool b1 = _isnan(cameraPos.x());
			bool b2 = _isnan(center.x());
			if ( (cameraPos==mpr_cameraPos && mpr_center==center)
													|| b1
													|| b2 )
			{// 无效数字
 				traverse(node,nv);	
				return;
			}
			
			osg::Geode* geodeStatusBar = dynamic_cast<osg::Geode*>(group->getChild(0));
			if (geodeStatusBar)
			{				
				//WCHAR wBuff[256];
				for(long j = 0; j<(long)geodeStatusBar->getNumDrawables();j++)
				{
					if(geodeStatusBar->getDrawable(j)->getName() == "CameraLon" 
												&& (mpr_cameraPos.x() != cameraPos.x()))
					{
						glbChar szBuffer[200]={0};
						glbDouble mouselon = cameraPos.x();						
						if(mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)
						{
							//swprintf(wBuff,L"相机： 经度 %.3f°E",mouselon);  
							//OutputDebugString(wBuff);

							glbChar str[256] = "相机： 经度："; 
							if(mouselon >= 0.0)
							{
								sprintf(szBuffer,"%.3f°E",mouselon);
								strcat(str,szBuffer);
							}
							else
							{
								sprintf(szBuffer,"%.3f°W",-mouselon);
								strcat(str,szBuffer);
							}
							osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
							if(text)
							{
								text->setText(((CGlbString)str).ToWString().c_str());									
							}
						}
						else if(mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
						{
							//swprintf(wBuff,L"相机： X：%.3f",mouselon);  
							//OutputDebugString(wBuff);

							glbChar str[256] = "相机： X：";
							sprintf(szBuffer,"%.3f",mouselon);
							strcat(str,szBuffer); 
							strcat(str,"米");  
							osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
							if(text)
								text->setText(((CGlbString)str).ToWString().c_str());
						}
					}
					else if(geodeStatusBar->getDrawable(j)->getName() == "CameraLat" 
													&& (mpr_cameraPos.y() != cameraPos.y()))
					{
						glbChar szBuffer[200]={0};
						glbDouble mouselat = cameraPos.y();						
						if(mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)
						{
							glbChar str[256] = "纬度："; 
							if(mouselat >= 0.0)
							{
								sprintf(szBuffer,"%.3f°N",mouselat);
								strcat(str,szBuffer); 
							}
							else
							{ 
								sprintf(szBuffer,"%.3f°S",-mouselat);
								strcat(str,szBuffer); 
							}
							osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
							if(text)
								text->setText(((CGlbString)str).ToWString().c_str());
						}
						else if(mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
						{
							//swprintf(wBuff,L"相机：Y：%.3f",mouselat);  
							//OutputDebugString(wBuff);

							glbChar str[256] = "Y：";
							sprintf(szBuffer,"%.3f",mouselat);
							strcat(str,szBuffer); 
							strcat(str,"米"); 
							osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
							if(text)
								text->setText(((CGlbString)str).ToWString().c_str());
						}
					}
					else if(geodeStatusBar->getDrawable(j)->getName() == "CameraAlt"
															&& (mpr_cameraPos.z() != cameraPos.z()))
					{
						glbChar szBuffer[200]={0};
						glbDouble altterrain = cameraPos.z();//-mpr_p_glbView->GetTerrainElevationOfCameraPos();

						//if (altterrain > 10000.0) 
						//	swprintf(wBuff,L"海拔高度:：%.3f",altterrain/1000.0);  
						//else
						//	swprintf(wBuff,L"海拔高度:：%.3f",altterrain);  
						//OutputDebugString(wBuff);

						if (altterrain > 10000.0) 
							sprintf(szBuffer,"%.3f",altterrain/1000.0);
						else
							sprintf(szBuffer,"%.3f",altterrain);
						glbChar str[256] = "海拔高度:";
						strcat(str,szBuffer);
						if (altterrain > 10000.0)
						{
							strcat(str,"千米"); 
						}else{
							strcat(str,"米"); 
						}						
						osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
						if(text)
							text->setText(((CGlbString)str).ToWString().c_str());
					}
					else if(geodeStatusBar->getDrawable(j)->getName() == "FocusLon"
																&& (mpr_center.x() != center.x()))
					{					
						glbChar szBuffer[200]={0};
						if(mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)
						{
							glbChar str[256] = "焦点： 经度：";
							if(center.x() >= 0.0)
							{
								sprintf(szBuffer,"%.3f°E",center.x());
								strcat(str,szBuffer);
							}
							else
							{
								sprintf(szBuffer,"%.3f°W",-center.x());
								strcat(str,szBuffer);
							} 
							osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
							if(text)
								text->setText(((CGlbString)str).ToWString().c_str());
						}
						else if(mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
						{
							//swprintf(wBuff,L"焦点： X：：%.3f",center.x());  
							//OutputDebugString(wBuff);

							glbChar str[256] = "焦点： X：";
							sprintf(szBuffer,"%.3f",center.x());
							strcat(str,szBuffer); 
							strcat(str,"米"); 
							osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
							if(text)
								text->setText(((CGlbString)str).ToWString().c_str());
						}
					}
					else if(geodeStatusBar->getDrawable(j)->getName() == "FocusLat" 
																	&& (mpr_center.y() != center.y()))
					{			
						glbChar szBuffer[200]={0};
						if(mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)
						{
							glbChar str[256] = "纬度：";
							if(center.y() >= 0.0)
							{
								sprintf(szBuffer,"%.3f°N",center.y());
								strcat(str,szBuffer); 
							} 
							else
							{
								sprintf(szBuffer,"%.3f°S",-center.y());
								strcat(str,szBuffer); 
							}
							osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
							if(text)
								text->setText(((CGlbString)str).ToWString().c_str());
						}
						else if(mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
						{
							//swprintf(wBuff,L"焦点： Y：：%.3f",center.y());  
							//OutputDebugString(wBuff);

							glbChar str[256] = "Y：";
							sprintf(szBuffer,"%.3f",center.y());
							strcat(str,szBuffer); 
							strcat(str,"米"); 
							osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
							if(text)
								text->setText(((CGlbString)str).ToWString().c_str());
						}
					}
					else if(geodeStatusBar->getDrawable(j)->getName() == "FocusAlt"
																	&& (mpr_center.z() != center.z()))
					{
						glbChar szBuffer[200]={0};
						glbDouble hei = center.z();		
						//if (hei > 10000.0)
						//	swprintf(wBuff,L"海拔高度: %.3f",hei/1000.0);  
						//else
						//	swprintf(wBuff,L"海拔高度: %.3f",hei);  
						//OutputDebugString(wBuff);

						if (hei > 10000.0)
							sprintf(szBuffer,"%.3f",hei/1000.0);
						else
							sprintf(szBuffer,"%.3f",hei);
						glbChar str[256] = "海拔高度:";
						strcat(str,szBuffer); 
						if(hei > 10000.0)
							strcat(str,"千米"); 
						else
							strcat(str,"米"); 
						osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
						if(text)
							text->setText(((CGlbString)str).ToWString().c_str());
					}
					else if(geodeStatusBar->getDrawable(j)->getName() == "Yaw")
					{
						glbChar szBuffer[200]={0};
						glbDouble localAzim = mpr_p_glbView->GetYaw();			
						if(localAzim < 0.0)
							localAzim = 360.0 + localAzim;		

						//swprintf(wBuff,L"偏航角： %.3f°",localAzim);  
						//OutputDebugString(wBuff);

						sprintf(szBuffer,"%.3f°",localAzim);
						glbChar str[256] = "偏航角：";
						strcat(str,szBuffer); 
						osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
						if(text)
							text->setText(((CGlbString)str).ToWString().c_str());
					}
					else if(geodeStatusBar->getDrawable(j)->getName() == "Pitch")
					{
						glbChar szBuffer[200]={0};
						glbDouble localPitch = mpr_p_glbView->GetPitch();	

						//swprintf(wBuff,L"俯仰角： %.3f°",-localPitch);  
						//OutputDebugString(wBuff);

						//localPitch = testy;						
						sprintf(szBuffer,"%.3f°",-localPitch);
						glbChar str[256] = "俯仰角：";
						strcat(str,szBuffer); 
						osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
						if(text)
							text->setText(((CGlbString)str).ToWString().c_str());
						break;
					}else if (geodeStatusBar->getDrawable(j)->getName() == "PositionLabel")
					{
						glbChar szBuffer[200]={0};
						glbDouble altterrain = cameraPos.z()-mpr_p_glbView->GetTerrainElevationOfCameraPos();	
						//if (altterrain > 10000.0)
						//	swprintf(wBuff,L"距地: %.3f",altterrain/1000.0);  
						//else
						//	swprintf(wBuff,L"距地: %.3f",altterrain);  
						//OutputDebugString(wBuff);

						if (altterrain > 10000.0) 
							sprintf(szBuffer,"%.2f",altterrain/1000.0);
						else
							sprintf(szBuffer,"%.2f",altterrain);
						glbChar str[256] = "距地:";
						strcat(str,szBuffer);
						if (altterrain > 10000.0)
						{
							strcat(str,"千米"); 
						}else{
							strcat(str,"米"); 
						}						
						osgText::Text* text = dynamic_cast<osgText::Text*>(geodeStatusBar->getDrawable(j));
						if(text)
							text->setText(((CGlbString)str).ToWString().c_str());
					}
				}
				mpr_cameraPos = cameraPos;
				mpr_center = center;
			}
		}
		//traverse(node,nv);
	}
	traverse(node,nv);	
}

void CGlbGlobeViewStatusBar::CGlbGlobeViewStatusBarUpdateCallback::Put_GlbGlobeView(CGlbGlobeView* pView)
{
	mpr_p_glbView = pView;
}

void CGlbGlobeViewStatusBar::CGlbGlobeViewStatusBarUpdateCallback::Put_GlbGlobe(CGlbGlobe* pGlobe)
{
	mpr_p_globe = pGlobe;
}

void CGlbGlobeViewStatusBar::SetPosition(int left,int top,int width,int height)
{
	this->setViewport(new osg::Viewport(left,top,width,25));
}

osg::ref_ptr<osg::Geode> CGlbGlobeViewStatusBar::createViewStatusBar()
{
	setlocale(LC_ALL, "");

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	//osg::ref_ptr<osgText::Font> fontEn = new osgText::Font();
	//fontEn = osgText::readFontFile("fonts/simsum.ttf");
	osg::ref_ptr<osgText::Font> fontCh = new osgText::Font();
	fontCh = osgText::readFontFile("fonts/simkai.ttf");
	osg::Vec4 fontcolor(1.0,1.0,1.0,0.8);

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(-20.0f,0.0f,0.4f));
	vertices->push_back(osg::Vec3(-20.0f,0.0f,-0.4f));
	vertices->push_back(osg::Vec3(20.0f,0.0f,-0.4f));
	vertices->push_back(osg::Vec3(20.0f,0.0f,0.4f));
	geom->setVertexArray(vertices);
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,1.0f,0.0f));
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(0.0f,0.0f,0.0f,0.8f));
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));	
	geom->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	geom->setName("StatusBar");
	geode->addDrawable(geom.get());

	osg::ref_ptr<osgText::Text> textlabel = new osgText::Text;
	textlabel->setFont(fontCh.get());
	textlabel->setText(L" 超维创想Creatar ");
	textlabel->setCharacterSize(0.4f);
	textlabel->setColor(fontcolor);
	textlabel->setPosition(osg::Vec3f(-20.0f,0.0f,0.0f));
	textlabel->setAlignment(osgText::Text::LEFT_CENTER);
	textlabel->setAxisAlignment(osgText::Text::XZ_PLANE);
	textlabel->setName("PositionLabel");
	geode->addDrawable(textlabel.get());

	osg::ref_ptr<osgText::Text> mouselon = new osgText::Text;
	mouselon->setFont(fontCh.get());
	mouselon->setText(L"相机： 经度：116.3°E");
	mouselon->setCharacterSize(0.4f);
	mouselon->setColor(fontcolor);
	mouselon->setPosition(osg::Vec3f(-16.0f,0.0f,0.0f));
	mouselon->setDataVariance(osg::Object::DYNAMIC);
	mouselon->setAlignment(osgText::Text::LEFT_CENTER);
	mouselon->setAxisAlignment(osgText::Text::XZ_PLANE);
	mouselon->setName("CameraLon");
	geode->addDrawable(mouselon.get());

	osg::ref_ptr<osgText::Text> mouselat = new osgText::Text;
	mouselat->setFont(fontCh.get());
	mouselat->setText("lat:39.9N");
	mouselat->setCharacterSize(0.4f);
	mouselat->setColor(fontcolor);
	mouselat->setPosition(osg::Vec3f(-10.50f,0.0f,0.0f));
	mouselat->setDataVariance(osg::Object::DYNAMIC);
	mouselat->setAlignment(osgText::Text::LEFT_CENTER);
	mouselat->setAxisAlignment(osgText::Text::XZ_PLANE);
	mouselat->setName("CameraLat");
	geode->addDrawable(mouselat.get());

	osg::ref_ptr<osgText::Text> mousealt = new osgText::Text;
	mousealt->setFont(fontCh.get());
	mousealt->setText("alt:300000.0km");
	mousealt->setCharacterSize(0.4f);
	mousealt->setColor(fontcolor);
	mousealt->setPosition(osg::Vec3f(-6.5f,0.0f,0.0f));
	mousealt->setDataVariance(osg::Object::DYNAMIC);
	mousealt->setAlignment(osgText::Text::LEFT_CENTER);
	mousealt->setAxisAlignment(osgText::Text::XZ_PLANE);
	mousealt->setName("CameraAlt");
	geode->addDrawable(mousealt.get());

	osg::ref_ptr<osgText::Text> textlon = new osgText::Text;
	textlon->setFont(fontCh.get());
	textlon->setText("Focus: lon:116.3E");
	textlon->setCharacterSize(0.4f);
	textlon->setColor(fontcolor);
	textlon->setPosition(osg::Vec3f(-1.0f,0.0f,0.0f));
	textlon->setDataVariance(osg::Object::DYNAMIC);
	textlon->setAlignment(osgText::Text::LEFT_CENTER);
	textlon->setAxisAlignment(osgText::Text::XZ_PLANE);
	textlon->setName("FocusLon");
	geode->addDrawable(textlon.get());

	osg::ref_ptr<osgText::Text> textlat = new osgText::Text;
	textlat->setFont(fontCh.get());
	textlat->setText("lat:39.9N");
	textlat->setCharacterSize(0.4f);
	textlat->setColor(fontcolor);
	textlat->setPosition(osg::Vec3f(4.5f,0.0f,0.0f));
	textlat->setDataVariance(osg::Object::DYNAMIC);
	textlat->setAlignment(osgText::Text::LEFT_CENTER);
	textlat->setAxisAlignment(osgText::Text::XZ_PLANE);
	textlat->setName("FocusLat");
	geode->addDrawable(textlat.get());

	osg::ref_ptr<osgText::Text> textalt = new osgText::Text;
	textalt->setFont(fontCh.get());
	textalt->setText("alt:0.0m");
	textalt->setCharacterSize(0.4f);
	textalt->setColor(fontcolor);
	textalt->setPosition(osg::Vec3f(8.0f,0.0f,0.0f));
	textalt->setDataVariance(osg::Object::DYNAMIC);
	textalt->setAlignment(osgText::Text::LEFT_CENTER);
	textalt->setAxisAlignment(osgText::Text::XZ_PLANE);
	textalt->setName("FocusAlt");
	geode->addDrawable(textalt.get());

	osg::ref_ptr<osgText::Text> textazim = new osgText::Text;
	textazim->setFont(fontCh.get());
	textazim->setText("Yaw:0.0°");
	textazim->setCharacterSize(0.4f);
	textazim->setColor(fontcolor);
	textazim->setPosition(osg::Vec3f(13.0f,0.0f,0.0f));
	textazim->setDataVariance(osg::Object::DYNAMIC);
	textazim->setAlignment(osgText::Text::LEFT_CENTER);
	textazim->setAxisAlignment(osgText::Text::XZ_PLANE);
	textazim->setName("Yaw");
	geode->addDrawable(textazim.get());

	osg::ref_ptr<osgText::Text> textpitch = new osgText::Text;
	textpitch->setFont(fontCh.get());
	textpitch->setText("Pitch:-90.0°  ");
	textpitch->setCharacterSize(0.4f);
	textpitch->setColor(fontcolor);
	textpitch->setPosition(osg::Vec3f(16.5f,0.0f,0.0f));
	textpitch->setDataVariance(osg::Object::DYNAMIC);
	textpitch->setAlignment(osgText::Text::LEFT_CENTER);
	textpitch->setAxisAlignment(osgText::Text::XZ_PLANE);
	textpitch->setName("Pitch");
	geode->addDrawable(textpitch.get());
	
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	osg::ref_ptr<osg::BlendColor> pBlenColor = new osg::BlendColor(osg::Vec4(1.0,1.0,1.0,(float)0.8));  
	//osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::CONSTANT_ALPHA,osg::BlendFunc::DST_ALPHA);
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::DST_ALPHA );	//设置混合方程
	geode->getOrCreateStateSet()->setAttributeAndModes(pBlenColor.get(),osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	geode->setName("StatusBargeode");
	geode->setNodeMask(0x02);
	return geode.get();
}