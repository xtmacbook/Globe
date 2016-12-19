#include "StdAfx.h"
#include "GlbGlobeMarkerImageSymbol.h"
#include "osg/Billboard"
#include "osg/BlendColor"
#include "osg/BlendFunc"
#include "osg/Texture2D"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbPoint.h"
#include "osg/Material"
#include "osg/Depth"

//#include "osg/AutoTransform"
#include "osg/PolygonMode"
#include "GlbLog.h"

using namespace GlbGlobe;
using namespace osg;

CGlbGlobeMarkerImageSymbol::CGlbGlobeMarkerImageSymbol(void)
{
}

CGlbGlobeMarkerImageSymbol::~CGlbGlobeMarkerImageSymbol(void)
{
}

osg::Node * CGlbGlobeMarkerImageSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerImageSymbolInfo *imageInfo = 
		static_cast<GlbMarkerImageSymbolInfo *>(renderInfo);
	if(imageInfo == NULL)
		return NULL;
	CGlbFeature *feature = obj->GetFeature();
	CGlbGlobe *globe = obj->GetGlobe();
	if(globe == NULL) return NULL;

	//osg::Group *group = new osg::Group;
	osg::MatrixTransform *group = new osg::MatrixTransform;
	//像素尺寸 默认值为-1
	glbInt32 maxSize = 0;
	if(imageInfo->imgInfo && imageInfo->imgInfo->maxSize)
		maxSize = imageInfo->imgInfo->maxSize->GetValue(feature);
	if(0 == maxSize)
		return group;
	osg::ref_ptr<osg::Image> image = NULL;
	CGlbWString datalocate = L"";
	if(imageInfo->imgInfo && imageInfo->imgInfo->dataSource)
	{
		datalocate = imageInfo->imgInfo->dataSource->GetValue(feature);
		glbInt32 index = datalocate.find_first_of(L'.');
		if(index == 0)
		{
			CGlbWString execDir = CGlbPath::GetExecDir();
			datalocate = execDir + datalocate.substr(1,datalocate.size());
		}
		image = osgDB::readImageFile(datalocate.ToString());
	}
	if(!image)
	{
		GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取图像文件(%s)失败. \r\n",datalocate.c_str());
		return NULL;
	}

	osg::ref_ptr<GlbGlobeAutoTransform> autoTransform = new GlbGlobeAutoTransform(globe);
	osg::ref_ptr<GlbGlobeAutoTransform> outAutoTransform = new GlbGlobeAutoTransform(globe);
	GlbGlobeLabelAlignTypeEnum labelAlign = GLB_LABELALG_LEFTBOTTOM;
	if(imageInfo->imageAlign)
		labelAlign = (GlbGlobeLabelAlignTypeEnum)imageInfo->imageAlign->GetValue(feature);

	GlbGlobeBillboardModeEnum bbMode = GLB_BILLBOARD_SCREEN;
	if(imageInfo->imgBillboard)
	{
		bbMode = (GlbGlobeBillboardModeEnum)imageInfo->imgBillboard->GetValue(feature);

		if (bbMode == GLB_BILLBOARD_SCREEN)
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		else if (bbMode == GLB_BILLBOARD_AXIS)
		{
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			autoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
			autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		else if(GLB_BILLBOARD_AUTOSLOPE == bbMode)
		{
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			autoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
			autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
	}

	glbInt32 imageWidth = image->s();
	glbInt32 imageHeight = image->t();
	double pixelRatioW = 1.0;
	double pixelRatioH = 1.0;

	if(maxSize >= 0 && (maxSize != imageWidth || maxSize != imageHeight))
	{
		if(imageWidth >= imageHeight)
		{
			pixelRatioW = (double)maxSize / imageWidth;
			pixelRatioH = (double)maxSize / imageWidth;
		}
		else
		{
			pixelRatioW = (double)maxSize / imageHeight;
			pixelRatioH = (double)maxSize / imageHeight;
		}
	}

	ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	if (image)
	{
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
	}

	glbBool isLimit = false;
	glbDouble ratio = 1.0;
	if(imageInfo->geoInfo->isLimit)
		isLimit = imageInfo->geoInfo->isLimit->GetValue(feature);
	if(imageInfo->geoInfo->ratio)
		ratio = imageInfo->geoInfo->ratio->GetValue(feature);

	//创建几何体
	osg::ref_ptr<osg::Geometry> labelGeom = NULL;
	osg::Vec3 corner(0.0,0.0,0.0);//GLB_LABELALG_LEFTBOTTOM
	osg::Vec3 width,height;

	imageWidth = imageWidth * pixelRatioW;
	imageHeight = imageHeight * pixelRatioH;

	if(!isLimit)
	{
		width = osg::Vec3(imageWidth * ratio,0.0,0.0);
		height = osg::Vec3(0.0,imageHeight * ratio,0.0);
		corner = ComputeCornerByAlign(imageWidth,imageHeight,ratio,labelAlign);
		labelGeom = osg::createTexturedQuadGeometry(corner,width,height);
	}
	else
	{
		width = osg::Vec3(imageWidth/* * ratio*/,0.0,0.0);
		height = osg::Vec3(0.0,imageHeight/* * ratio*/,0.0);
		corner = ComputeCornerByAlign(imageWidth,imageHeight,1.0,labelAlign);
		labelGeom = osg::createTexturedQuadGeometry(corner,width,height);
		autoTransform->setAutoScaleToScreen(true);
		autoTransform->setMinimumScale(0.0);
		//if(-1 == maxSize)
		//{
		//	autoTransform->setMaximumScale(DBL_MAX);
		//}
		//else
			autoTransform->setMaximumScale(/*maxSize * */ratio);
	}

	glbInt32 opacity = 100;
	if(imageInfo->imgInfo->opacity)
		opacity = imageInfo->imgInfo->opacity->GetValue(feature);
	glbInt32 color = 0xFFFFFFFF;
	if(imageInfo->imgInfo->color)
		color = imageInfo->imgInfo->color->GetValue(feature);

	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	material->setEmission(osg::Material::FRONT_AND_BACK,GetColor(color));
	//material->setDiffuse(osg::Material::FRONT_AND_BACK,GetColor(color));
	material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	material->setAlpha(osg::Material::FRONT_AND_BACK,opacity / 100.0);
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);

	osg::ref_ptr<osg::Depth> depth = dynamic_cast<osg::Depth *>(stateset->getAttribute(osg::StateAttribute::DEPTH));
	if(!depth.valid())
	{
		depth = new osg::Depth;
		depth->setWriteMask(false);
		stateset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
	}
	else
	{
		if(depth->getWriteMask())
			depth->setWriteMask(false);
	}

	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::/*ONE*/SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);	//设置混合方程
	stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON);

	stateset->setAttribute(material,osg::StateAttribute::ON);
	//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	labelGeom->setStateSet(stateset);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geode> outGeode = new osg::Geode;
	//内框
	osg::ref_ptr<osg::Geometry> lineGeometry = CreateOutlineQuad(corner,width,height);
	geode->addDrawable(labelGeom);
	//geode->addDrawable(lineGeometry);
	autoTransform->addChild(geode);
	group->addChild(autoTransform);

	////添加外框
	if(isLimit)
	{
		osg::ref_ptr<osg::Geometry> outlineGeometry = CreateOutlineQuad(corner,width,height);
		if(NULL == outlineGeometry) return group;
		//outGeode->addDrawable(outlineGeometry);
		outAutoTransform->addChild(outGeode);
		if(GLB_BILLBOARD_SCREEN == bbMode)
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		else if(GLB_BILLBOARD_AXIS == bbMode)
		{
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			outAutoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
			outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		else if(GLB_BILLBOARD_AUTOSLOPE == bbMode)
		{
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			outAutoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
			outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		outAutoTransform->setAutoScaleToScreen(true);
		outAutoTransform->setMinimumScale(DBL_MAX);
		outAutoTransform->setMaximumScale(/*maxSize * */ratio);
		group->addChild(outAutoTransform);
	}

	glbDouble xOffset = 0.0,yOffset = 0.0,zOffset = 0.0;
	if(imageInfo->xOffset && imageInfo->yOffset && imageInfo->zOffset)
	{
		xOffset = imageInfo->xOffset->GetValue(feature);
		yOffset = imageInfo->yOffset->GetValue(feature);
		zOffset = imageInfo->zOffset->GetValue(feature);
		osg::Matrix m;
		m.makeTranslate(xOffset,yOffset,zOffset);
		group->setMatrix(m);
	}

	return group;
}

osg::Geometry * GlbGlobe::CGlbGlobeMarkerImageSymbol::CreateOutlineQuad( osg::Vec3 cornerVec,osg::Vec3 widthVec,osg::Vec3 heightVec )
{
	osg::Geometry *geom = new osg::Geometry;
	osg::ref_ptr<osg::StateSet> geomState = geom->getOrCreateStateSet();
	//geomState->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	osg::ref_ptr<osg::Material> geomMaterial = 
		dynamic_cast<osg::Material *>(geomState->getAttribute(osg::StateAttribute::MATERIAL));
	if(geomMaterial == NULL)
		geomMaterial = new osg::Material;
	geomMaterial->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,0,1));
	geomMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	geomState->setAttribute(geomMaterial,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	osg::ref_ptr<osg::Vec3dArray> vertexArray = new osg::Vec3dArray(4);
	(*vertexArray)[0] = cornerVec + heightVec;
	(*vertexArray)[1] = cornerVec;
	(*vertexArray)[2] = cornerVec + widthVec;
	(*vertexArray)[3] = cornerVec + widthVec + heightVec;
	geom->setVertexArray(vertexArray);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::Vec3dArray> normals = new osg::Vec3dArray(1);
	(*normals)[0] = widthVec ^ heightVec;
	(*normals)[0].normalize();
	geom->setNormalArray(normals);
	geom->setNormalBinding(Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::LINE_LOOP,0,4));
	return geom;
}
