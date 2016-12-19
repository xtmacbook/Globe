#include "StdAfx.h"
#include "GlbGlobeViewCentermark.h"
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/TexEnv>
#include <osg/BlendFunc>

#define CentermarkSize 20

using namespace GlbGlobe;

CGlbGlobeViewCentermark::CGlbGlobeViewCentermark(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe)
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
	CGlbWString dpath = CGlbPath::GetExecDir();
	dpath += L"\\res\\007.png";
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(dpath.ToString().c_str());

	osg::ref_ptr<osg::Texture2D> tex2d = new osg::Texture2D;
	if (image.get())
		tex2d->setImage(image.get());
	tex2d->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP);
	tex2d->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP);
	//设置纹理环境
	osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
	texenv->setMode(osg::TexEnv::REPLACE);//贴花
	//启用纹理单元0
	geom->getOrCreateStateSet()->setTextureAttributeAndModes(0,tex2d.get(),osg::StateAttribute::ON);
	//设置纹理环境
	geom->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::DST_ALPHA );	//设置混合方程
	//osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_COLOR,osg::BlendFunc::ONE_MINUS_SRC_COLOR,osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
	geode->getOrCreateStateSet()->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);

	osg::BoundingSphere bsHeight = geode->computeBound();
	osg::Vec3d upDirection( 0.0,0.0,1.0 );
	osg::Vec3d viewDirection( 0.0,-1.0,0.0 );
	double viewDistance = bsHeight.radius();
	osg::Vec3d center = bsHeight.center();

	osg::Vec3d eyePoint = center + viewDirection * viewDistance;
	this->setClearMask( GL_DEPTH_BUFFER_BIT);
	this->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	this->setViewMatrixAsLookAt(eyePoint, center, upDirection);
	double znear = viewDistance - bsHeight.radius();
	double zfar = viewDistance + bsHeight.radius();
	float top = bsHeight.radius();
	float right = bsHeight.radius();
	SetPosition(0,0,width,height);
	this->setProjectionMatrixAsOrtho(  -right, right, -top, top, znear, zfar  );
	this->setRenderOrder(osg::Camera::POST_RENDER);
	this->setAllowEventFocus(false);
	this->setName("Navigator");
	this->addChild(geode.get());
	this->setNodeMask(0x02);	
}


CGlbGlobeViewCentermark::~CGlbGlobeViewCentermark(void)
{

}

void CGlbGlobeViewCentermark::SetPosition(int left,int top,int width,int height)
{
	this->setViewport(new osg::Viewport(width/2 - CentermarkSize + left,height/2 - CentermarkSize + top,2*CentermarkSize,2*CentermarkSize));
}
