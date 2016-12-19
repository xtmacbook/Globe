#include "StdAfx.h"
#include "GlbGlobeViewLogo.h"
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/TexEnv>
#include <osg/BlendFunc>
#include <osg/TexGen>

#define LOGOSIZE 100
using namespace GlbGlobe;

CGlbGlobeViewLogo::CGlbGlobeViewLogo(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe)
{
	mpr_geode = createViewLogo();
	osg::BoundingSphere bsHeight = mpr_geode->computeBound();
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

	this->setName("Logo");
	this->addChild(mpr_geode.get());
	this->setNodeMask(0x02);
}


CGlbGlobeViewLogo::~CGlbGlobeViewLogo(void)
{
	mpr_geode = NULL;
}

void CGlbGlobeViewLogo::SetPosition(int left,int top,int width,int height)
{
	this->setViewport(new osg::Viewport(width - LOGOSIZE*1.2 + left,LOGOSIZE*0.2 + top,LOGOSIZE,LOGOSIZE));	//右下角
}

void CGlbGlobeViewLogo::SetImageLogo(CGlbWString path)
{
	//换Logo
	if (path != L"")
	{
		osg::ref_ptr<osg::Image> image0 = osgDB::readImageFile(path.ToString().c_str());
		osg::ref_ptr<osg::Texture2D> tex2d0 = new osg::Texture2D;
		if (image0.get())
			tex2d0->setImage(image0.get());
		tex2d0->setWrap(osg::Texture::WRAP_S,osg::Texture2D::CLAMP);
		tex2d0->setWrap(osg::Texture::WRAP_T,osg::Texture2D::CLAMP);
		//设置纹理环境
		osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
		texenv->setMode(osg::TexEnv::DECAL);//贴花
		//启用纹理单元0
		mpr_geode->getOrCreateStateSet()->setTextureAttributeAndModes(0,tex2d0.get(),osg::StateAttribute::ON);
		//设置纹理环境
		mpr_geode->getOrCreateStateSet()->setTextureAttribute(0,texenv.get());
	}
}

osg::ref_ptr<osg::Geode> CGlbGlobeViewLogo::createViewLogo()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry; 
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texcoord = new osg::Vec2Array;
	vertices->push_back(osg::Vec3(-1.0,0.0,-1.0)); 
	vertices->push_back(osg::Vec3(1.0,0.0,-1.0));
	vertices->push_back(osg::Vec3(1.0,0.0,1.0));
	vertices->push_back(osg::Vec3(-1.0,0.0,1.0));
	geom->setVertexArray(vertices.get());
	texcoord->push_back(osg::Vec2(0.0,0.0));
	texcoord->push_back(osg::Vec2(1.0,0.0));
	texcoord->push_back(osg::Vec2(1.0,1.0));
	texcoord->push_back(osg::Vec2(0.0,1.0));
	geom->setTexCoordArray(0,texcoord.get());
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

	//纹理Logo
	HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
	CGlbWString dpath = CGlbPath::GetModuleDir(hmd);
	CGlbWString dpath0 = dpath + L"\\res\\tree.png";
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

	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	//osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::DST_ALPHA );	//设置混合方程
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::SRC_COLOR,osg::BlendFunc::ONE_MINUS_SRC_COLOR,osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );	//设置混合方程
	geode->getOrCreateStateSet()->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	geode->setName("Logogeode");
	geode->setNodeMask(0x02);
	return geode;
}
