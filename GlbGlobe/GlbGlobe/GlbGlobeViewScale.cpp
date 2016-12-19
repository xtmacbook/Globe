#include "StdAfx.h"
#include "GlbGlobeViewScale.h"
#include <osgText/Text>
#include <osgText/Font>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osg/LineWidth>
using namespace GlbGlobe;

CGlbGlobeViewScale::CGlbGlobeViewScale(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe)
{	
	osg::ref_ptr<osg::Geode> geode = createViewScale();
	osg::BoundingSphere bsHeight = geode->computeBound();
	this->setClearMask( GL_DEPTH_BUFFER_BIT);
	this->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	glbDouble bs = 2.0;//bsHeight.radius();
	osg::Vec3d upDirection( 0.0,0.0,1.0 );
	osg::Vec3d viewDirection( 0.0,-1.0,0.0 );
	double viewDistance = bs;//bsHeight.radius();
	osg::Vec3d center = osg::Vec3d(1.0,0.0,0.0);//bsHeight.center();

	osg::Vec3d eyePoint = center + viewDirection * viewDistance;

	this->setViewMatrixAsLookAt( eyePoint, center, upDirection );
	double znear = viewDistance - bs;
	double zfar = viewDistance + bs;
	float top = bs;
	float right = bs;
	SetPosition(0,0,width,height);
	this->setProjectionMatrixAsOrtho( -right, right, -top*0.1, top*0.1, znear, zfar );

	this->setRenderOrder(osg::Camera::POST_RENDER);
	this->setAllowEventFocus(false);

	this->setName("Sale");
	this->addChild(geode.get());
	osg::ref_ptr<CGlbGlobeViewScaleUpdateCallback> vp = new CGlbGlobeViewScaleUpdateCallback(this,width,height);
	vp->Put_GlbGlobe(pglobe);
	vp->Put_GlbGlobeView(pview);	
	this->setUpdateCallback(vp.get());
	this->setNodeMask(0x02);
}


CGlbGlobeViewScale::~CGlbGlobeViewScale(void)
{
}

void CGlbGlobeViewScale::CGlbGlobeViewScaleUpdateCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
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

			osg::Vec3d cameraPos;
			mpr_p_glbView->GetCameraPos(cameraPos);			

			bool b1 = _isnan(cameraPos.x());			
			if ( cameraPos==mpr_cameraPos || b1 )
			{// 无效数字
				traverse(node,nv);	
				return;
			}
			mpr_cameraPos = cameraPos;

			osg::Geode* geode = dynamic_cast<osg::Geode*>(group->getChild(0));
			if (geode)
			{
				if(geode->getDrawable(1))
				{
					glbDouble val = 1000000.0;
					//比例尺线长num个像素
					glbInt32 wid = ::GetSystemMetrics(SM_CXSCREEN);
					glbDouble num = wid/40;
					//获取焦点位置经纬度
					osg::Vec3d focusPos;
					mpr_p_glbView->GetFocusPos(focusPos);
					//偏移(num,0)个像素
					osg::Vec3d pos;
					glbBool isfocus = mpr_p_globe->ScreenToTerrainCoordinate(mpr_width/2+num,mpr_height/2,pos.x(),pos.y(),pos.z());
					if (mpr_p_globe->GetType() == GLB_GLOBETYPE_GLOBE)
					{
						if (mpr_p_globe->IsUnderGroundMode())
						{
							if (isfocus)
							{
								//求球面2点的球面距离
								//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusPos.x(),focusPos.y(),focusPos.z());
								//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pos.y()),osg::DegreesToRadians(pos.x()),pos.z(),pos.x(),pos.y(),pos.z());
								//val = (pos-focusPos).length();
								glbDouble angle = computeGlobeAngle(focusPos.y(),focusPos.x(),pos.y(),pos.x());
								val = (osg::WGS_84_RADIUS_EQUATOR+osg::WGS_84_RADIUS_POLAR)*angle;
							}else{
								isfocus = mpr_p_globe->ScreenToUGPlaneCoordinate(mpr_width/2+num,mpr_height/2,pos.x(),pos.y(),pos.z());
								if (isfocus)
								{
									//求球面2点的球面距离
									//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusPos.x(),focusPos.y(),focusPos.z());
									//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pos.y()),osg::DegreesToRadians(pos.x()),pos.z(),pos.x(),pos.y(),pos.z());
									//val = (pos-focusPos).length();
									glbDouble angle = computeGlobeAngle(focusPos.y(),focusPos.x(),pos.y(),pos.x());
									val = (osg::WGS_84_RADIUS_EQUATOR+osg::WGS_84_RADIUS_POLAR+2*mpr_p_globe->GetUnderGroundAltitude())*angle;
								}
							}
						}else{	//没开地下模式
							//求球面2点的球面距离
							//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(focusPos.y()),osg::DegreesToRadians(focusPos.x()),focusPos.z(),focusPos.x(),focusPos.y(),focusPos.z());
							//g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pos.y()),osg::DegreesToRadians(pos.x()),pos.z(),pos.x(),pos.y(),pos.z());
							//val = (pos-focusPos).length();
							glbDouble angle = computeGlobeAngle(focusPos.y(),focusPos.x(),pos.y(),pos.x());
							val = (osg::WGS_84_RADIUS_EQUATOR+osg::WGS_84_RADIUS_POLAR)*angle;
						}						
					}else if (mpr_p_globe->GetType() == GLB_GLOBETYPE_FLAT)
					{
						if (mpr_p_globe->IsUnderGroundMode())
						{
							if (isfocus)
							{
								val = (focusPos-pos).length();
							}else{
								isfocus = mpr_p_globe->ScreenToUGPlaneCoordinate(mpr_width/2+num,mpr_height/2,pos.x(),pos.y(),pos.z());
								if (isfocus)
								{
									val = (focusPos-pos).length();
								}
							}
						}else{
							val = (focusPos-pos).length();
						}
					}					
					glbChar str[256] = {0};
					mpr_viewScale->UpdateScale(val);
					if (val > 1000000.0)
					{
						val /= 1000.0;
						sprintf(str,"%.1f ",val);
						strcat(str,"千米"); 
					}else if(val > 1000.0)
					{
						val /= 1000.0;
						sprintf(str,"%.2f ",val);
						strcat(str,"千米"); 
					}else{
						sprintf(str,"%.2f ",val);
						strcat(str,"米"); 
					}
					osgText::Text* text = dynamic_cast<osgText::Text*>(geode->getDrawable(1));
					if(text)
						text->setText(((CGlbString)str).ToWString().c_str());
				}
			}
		}
	}

}

void CGlbGlobeViewScale::CGlbGlobeViewScaleUpdateCallback::Put_GlbGlobeView(CGlbGlobeView* pView)
{
	mpr_p_glbView = pView;
}

void CGlbGlobeViewScale::CGlbGlobeViewScaleUpdateCallback::Put_GlbGlobe(CGlbGlobe* pGlobe)
{
	mpr_p_globe = pGlobe;
}

void CGlbGlobeViewScale::SetPosition(int left,int top,int width,int height)
{
	//获取屏幕分辨率
	glbInt32 wid = ::GetSystemMetrics(SM_CXSCREEN);
	glbInt32 hei = ::GetSystemMetrics(SM_CYSCREEN);
	glbInt32 size_width = wid/10;	//比例尺线长为1/20个屏幕宽度
	glbInt32 size_height = 20;
	this->setViewport(new osg::Viewport(30 + left,35 + top,size_width,size_height));	//左下角
}

osg::ref_ptr<osg::Geode> CGlbGlobeViewScale::createViewScale()
{	
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry; 
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(osg::Vec3(-1.0,0.0,0.0));
	vertices->push_back(osg::Vec3(0.0,0.0,0.0));
	vertices->push_back(osg::Vec3(1.0,0.0,0.0));
	vertices->push_back(osg::Vec3(-1.0,0.0,0.1));
	vertices->push_back(osg::Vec3(0.0,0.0,0.05));
	vertices->push_back(osg::Vec3(1.0,0.0,0.1));
	geom->setVertexArray(vertices.get());
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,1.0f,0.0f));
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	geom->setColorArray(colors);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::DrawElementsUInt> indexArray = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINE_STRIP,0);
	indexArray->push_back(0);
	indexArray->push_back(1);
	indexArray->push_back(2);
	geom->addPrimitiveSet(indexArray.get());
	osg::ref_ptr<osg::DrawElementsUInt> indexArray2 = new osg::DrawElementsUInt(osg::DrawElementsUInt::LINES,0);
	indexArray2->push_back(0);	indexArray2->push_back(3);
	indexArray2->push_back(1);	indexArray2->push_back(4);
	indexArray2->push_back(2);	indexArray2->push_back(5);
	geom->addPrimitiveSet(indexArray2.get());	
	osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
	lineWidth->setWidth(2);
	geom->getOrCreateStateSet()->setAttributeAndModes(lineWidth.get(),osg::StateAttribute::ON);
	geom->setName("SaleLine");
	geode->addDrawable(geom.get());
	//文字
	setlocale(LC_ALL, "");
	osg::ref_ptr<osgText::Font> fontCh = new osgText::Font();
	fontCh = osgText::readFontFile("fonts/simkai.ttf");
	osg::Vec4 fontcolor(1.0,1.0,1.0,0.8);
	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	text->setFont(fontCh.get());
	text->setText(L"999999千米");
	text->setCharacterSize(0.3f);
	text->setColor(fontcolor);
	text->setPosition(osg::Vec3f(1.2f,0.0f,0.0f));
	text->setDataVariance(osg::Object::DYNAMIC);
	text->setAlignment(osgText::Text::LEFT_CENTER);
	text->setAxisAlignment(osgText::Text::XZ_PLANE);
	text->setName("SaleText");
	geode->addDrawable(text.get());

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