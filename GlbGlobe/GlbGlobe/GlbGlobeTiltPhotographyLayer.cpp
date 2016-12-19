#include "StdAfx.h"
#include "GlbGlobeTiltPhotographyLayer.h"
#include "GlbGlobeTypes.h"
#include <osg/MatrixTransform>
#include<iostream> 
#include <osgDB/ReadFile>
#include <osg/BlendColor>
#include <osg/BlendFunc>
//#include "CGlbGlobe.h"
#include "GlbGlobeDigHole.h"

using namespace GlbGlobe;

CGlbGlobeTiltPhotographyLayer::CGlbGlobeTiltPhotographyLayer(CGlbWString tpfFile,glbBool isGlobe)
{
	mpr_critical.SetName(L"globe_tiltPhotographylayer");	
	mpr_layer_type = GLB_GLOBELAYER_TILTPHOTO;	
	mpr_opacity=100;
	mpr_isGlobe = isGlobe;
	mpr_layer_node = new osg::Switch;
	mpr_layer_node->setName("倾斜摄影图层");
	mpr_anchor.set(0,0,0);
	mpr_digHoleObj = NULL;
	mpr_layer_name = L"倾斜摄影图层";
	mpr_defaultvisibledist = 1000000000000.0; // 设置图层无限远处可见
	mpr_tpfFilePath = tpfFile;
	loadTPFfile(tpfFile);	
}

CGlbGlobeTiltPhotographyLayer::~CGlbGlobeTiltPhotographyLayer(void)
{
	mpr_layer_node = NULL;	
	mpr_digHoleObj = NULL;
}

const glbWChar*	CGlbGlobeTiltPhotographyLayer::GetName()
{
	return mpr_layer_name.c_str();
}
glbBool CGlbGlobeTiltPhotographyLayer::SetName(glbWChar* name)
{
	mpr_layer_name = name;
	return true;
}
GlbGlobeLayerTypeEnum CGlbGlobeTiltPhotographyLayer::GetType()
{
	return mpr_layer_type;
}
glbDouble CGlbGlobeTiltPhotographyLayer::GetDistance( osg::Vec3d &cameraPos)
{
	glbDouble distance;
	osg::Vec3d cameraPoint = cameraPos;
	osg::Vec3d position = mpr_anchor;
	if (mpr_isGlobe)
	{
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
			osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());	
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_anchor.y()),
			osg::DegreesToRadians(mpr_anchor.x()),mpr_anchor.z(),position.x(),position.y(),position.z());			
	}
	distance = (position - cameraPoint).length();
	return distance;
}

glbBool CGlbGlobeTiltPhotographyLayer::Load(xmlNodePtr node, glbWChar* relativepath )
{
	xmlChar* szKey = NULL;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerID")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_layer_id);	
		xmlFree(szKey);
		node = node->next;
	}			
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerName")))
	{		
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		mpr_layer_name = glbstr.ToWString();	
		xmlFree(szKey);
		free(str);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LayerGroupID")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_layergroup_id);
		xmlFree(szKey);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_opacity);	
		xmlFree(szKey);
		node = node->next;
	}			

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Visible")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%c",&mpr_show);
		xmlFree(szKey);
		node = node->next;
	}	

	//if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TPFPath")))
	//{		
	//	szKey = xmlNodeGetContent(node);
	//	char* str = u2g((char*)szKey);
	//	CGlbString glbstr = str;
	//	mpr_tpfFilePath = glbstr.ToWString();	
	//	xmlFree(szKey);
	//	free(str);
	//	node = node->next;
	//}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RenderOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_renderorder);	
		xmlFree(szKey);
		node = node->next;
	}		

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LoadOrder")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_loadorder);
		xmlFree(szKey);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActiveAction")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%d",&mpr_activeaction);
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionYaw")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionyaw);	
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionPitch")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionpitch);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionRoll")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionroll);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionLonOrX")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLonOrX);
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionLatOrY")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionLatOrY);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ActionAltOrZ")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_actionAltOrZ);	
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DefaultVisDis")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf((char*)szKey,"%lf",&mpr_defaultvisibledist);	
		xmlFree(szKey);
		node = node->next;
	}

	return true;
}
glbBool CGlbGlobeTiltPhotographyLayer::Save(xmlNodePtr node, glbWChar* relativepath )
{	
	char str[128];
	sprintf(str,"%d",mpr_layer_id);
	xmlNewTextChild(node, NULL, BAD_CAST "LayerID", BAD_CAST str);

	char* szOut = g2u((char*)mpr_layer_name.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "LayerName", BAD_CAST szOut);
	free(szOut);

	sprintf(str,"%d",mpr_layergroup_id);
	xmlNewTextChild(node, NULL, BAD_CAST "LayerGroupID", BAD_CAST str);
	sprintf(str,"%d",mpr_opacity);
	xmlNewTextChild(node, NULL, BAD_CAST "Opacity", BAD_CAST str);
	sprintf(str,"%c",mpr_show);
	xmlNewTextChild(node, NULL, BAD_CAST "Visible", BAD_CAST str);

	szOut = g2u((char*)mpr_tpfFilePath.ToString().c_str());
	xmlNewTextChild(node, NULL, BAD_CAST "TPFPath", BAD_CAST szOut);
	free(szOut);	

	sprintf(str,"%d",mpr_renderorder);
	xmlNewTextChild(node, NULL, BAD_CAST "RenderOrder", BAD_CAST str);
	sprintf(str,"%d",mpr_loadorder);
	xmlNewTextChild(node, NULL, BAD_CAST "LoadOrder", BAD_CAST str);

	sprintf(str,"%d",mpr_activeaction);//图层激活动作
	xmlNewTextChild(node, NULL, BAD_CAST "ActiveAction", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionyaw);//图层飞向跳转时的偏航角【绕y轴旋转】
	xmlNewTextChild(node, NULL, BAD_CAST "ActionYaw", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionpitch);//图层飞向跳转时的倾斜角【绕x轴旋转】
	xmlNewTextChild(node, NULL, BAD_CAST "ActionPitch", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionroll);//图层飞向跳转时的旋转角【绕z轴旋转】
	xmlNewTextChild(node, NULL, BAD_CAST "ActionRoll", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionLonOrX);//图层飞向跳转时的视点经度或X
	xmlNewTextChild(node, NULL, BAD_CAST "ActionLonOrX", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionLatOrY);//图层飞向跳转时的视点纬度或Y
	xmlNewTextChild(node, NULL, BAD_CAST "ActionLatOrY", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_actionAltOrZ);//图层飞向跳转时的视点高度或Z
	xmlNewTextChild(node, NULL, BAD_CAST "ActionAltOrZ", BAD_CAST str);
	sprintf(str,"%.5lf",mpr_defaultvisibledist);//图层飞向跳转时的视点高度或Z
	xmlNewTextChild(node, NULL, BAD_CAST "DefaultVisDis", BAD_CAST str);

	return true;
}

glbBool CGlbGlobeTiltPhotographyLayer::Show(glbBool show,glbBool isOnState)
{
	if (mpr_layer_node==NULL) return false;
	if (mpr_show==show) return false;
	mpr_show = show;

	if (mpr_show)
		mpr_layer_node->asSwitch()->setAllChildrenOn();
	else
		mpr_layer_node->asSwitch()->setAllChildrenOff();	
	
	if (mpr_globe)
	{
		glbref_ptr<CGlbExtent> cglbExtent = GetBound(false);
		mpr_globe->AddDomDirtyExtent(*(cglbExtent.get()));	
	}	
	return true;
}
glbBool CGlbGlobeTiltPhotographyLayer::SetOpacity(glbInt32 opacity)
{
	if (mpr_opacity==opacity)
		return true;
	if (mpr_layer_node==NULL)
		return false;
	mpr_opacity = opacity;
	// 纹理alpha值修改
	osg::StateSet* ss = mpr_layer_node->getOrCreateStateSet();
	if (mpr_opacity<100)
	{// 透明			
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
		if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );	//ss->setRenderBinDetails(11,"DepthSortedBin"); 

		osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));		
		if (!bc)
		{
			bc=new osg::BlendColor(osg::Vec4(1.0f,1.0f,1.0f,(float)opacity/ 100.0));  
			ss->setAttributeAndModes(bc,osg::StateAttribute::ON);
		}
		else
		{
			bc->setConstantColor(osg::Vec4(1.0f,1.0f,1.0f,(float)opacity/ 100.0));
		}
		osg::BlendFunc* bf = dynamic_cast<osg::BlendFunc*>(ss->getAttribute(osg::StateAttribute::BLENDFUNC));
		if (!bf)
		{
			bf = new osg::BlendFunc();						
			bf->setFunction(osg::BlendFunc::CONSTANT_ALPHA,osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA,osg::BlendFunc::CONSTANT_ALPHA,osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
			ss->setAttributeAndModes(bf,osg::StateAttribute::ON);
		}		
	}
	else
	{		
			
		osg::BlendColor* bc = dynamic_cast<osg::BlendColor*>(ss->getAttribute(osg::StateAttribute::BLENDCOLOR));
		if (bc)
			ss->removeAttribute(bc);
		osg::BlendFunc* bf = dynamic_cast<osg::BlendFunc*>(ss->getAttribute(osg::StateAttribute::BLENDFUNC));
		if (bf)
			ss->removeAttribute(bf);
		
		ss->setMode(GL_BLEND, osg::StateAttribute::OFF );
		if (osg::StateSet::OPAQUE_BIN != ss->getRenderingHint())
			ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		//ss->setRenderBinDetails( 1, "RenderBin");
	}

	return false;
}

glbref_ptr<CGlbExtent> CGlbGlobeTiltPhotographyLayer::GetBound(glbBool isWorld)
{
	if (mpr_bound.IsValid()==false)
		return NULL;
	glbDouble xmin,xmax,ymin,ymax,zmin,zmax;
	mpr_bound.Get(&xmin,&xmax,&ymin,&ymax,&zmin,&zmax);

	glbref_ptr<CGlbExtent> ext = new CGlbExtent;
	if (mpr_isGlobe)
	{// 将局部坐标的长度转为经度和纬度的度数
		// x对应的是经度，y对应的是纬度
		double minLon = 360.0 * (xmin / (g_ellipsmodel->getRadiusEquator() * cos(osg::DegreesToRadians(mpr_anchor.y())) * 2 * osg::PI));
		double maxLon = 360.0 * (xmax / (g_ellipsmodel->getRadiusEquator() * cos(osg::DegreesToRadians(mpr_anchor.y())) * 2 * osg::PI));
		double minLat = 360.0 * (ymin / (g_ellipsmodel->getRadiusPolar() * 2 * osg::PI) );
		double maxLat = 360.0 * (ymax / (g_ellipsmodel->getRadiusPolar() * 2 * osg::PI) );
		ext->Set(mpr_anchor.x()+minLon,mpr_anchor.x()+maxLon, mpr_anchor.y()+minLat,mpr_anchor.y()+maxLat,mpr_anchor.z()+zmin,mpr_anchor.z()+zmax);

		//ext->Set(100,136,32,48);
		if (isWorld)
		{
			osg::BoundingBoxd bb;
			double x,y,z;
			ext->Get(&xmin,&xmax,&ymin,&ymax,&zmin,&zmax);
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ymin),osg::DegreesToRadians(xmin),zmin,x,y,z);
			bb.expandBy(x,y,z);
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ymin),osg::DegreesToRadians(xmax),zmin,x,y,z);
			bb.expandBy(x,y,z);
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ymax),osg::DegreesToRadians(xmax),zmin,x,y,z);
			bb.expandBy(x,y,z);
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ymax),osg::DegreesToRadians(xmin),zmin,x,y,z);
			bb.expandBy(x,y,z);
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ymin),osg::DegreesToRadians(xmin),zmax,x,y,z);
			bb.expandBy(x,y,z);
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ymin),osg::DegreesToRadians(xmax),zmax,x,y,z);
			bb.expandBy(x,y,z);
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ymax),osg::DegreesToRadians(xmax),zmax,x,y,z);
			bb.expandBy(x,y,z);
			g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(ymax),osg::DegreesToRadians(xmin),zmax,x,y,z);
			bb.expandBy(x,y,z);

			ext->Set(bb.xMin(),bb.xMax(),bb.yMin(),bb.yMax(),bb.zMin(),bb.zMax());
		}		
	}
	else
	{		
		ext->Set(mpr_anchor.x()+xmin,mpr_anchor.x()+xmax, mpr_anchor.y()+ymin,mpr_anchor.y()+ymax, mpr_anchor.z()+zmin,mpr_anchor.z()+zmax);
	}
	return ext;
}

const glbWChar* CGlbGlobeTiltPhotographyLayer::GetTPFFilePath()
{
	return mpr_tpfFilePath.c_str();
}

void CGlbGlobeTiltPhotographyLayer::SetAssociateObject(CGlbGlobeObject* dighole)
{
	mpr_digHoleObj = dighole;
}
CGlbGlobeObject* CGlbGlobeTiltPhotographyLayer::GetAssociateObject()
{
	return mpr_digHoleObj.get();
}

void CGlbGlobeTiltPhotographyLayer::Update(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
											std::vector<glbref_ptr<CGlbGlobeRObject>> objs	)
{

}

void CGlbGlobeTiltPhotographyLayer::RefreshTexture(std::vector<CGlbExtent>                    &objDirtyExts,
													std::vector<CGlbExtent>                    &domDirtyExts,
													std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers, 	
													std::vector<glbref_ptr<CGlbGlobeRObject>>  &objs	)
{

}

glbBool CGlbGlobeTiltPhotographyLayer::loadTPFfile(CGlbWString tpfFile)
{
	CGlbString fileName = tpfFile.ToString();
	ifstream cin(fileName.c_str());
	if (cin.is_open()==false)
		return false;

	char buff[128];

	std::string strTemp;
	cin >> strTemp;
	if (strTemp=="锚点坐标")
	{
		cin >> mpr_anchor.x() >> mpr_anchor.y() >> mpr_anchor.z();
	}
	cin >> strTemp;
	if (strTemp=="数据范围")
	{
		double xmin,xmax,ymin,ymax,zmin,zmax;
		cin >> xmin >> xmax >> ymin >> ymax >> zmin >> zmax;
		mpr_bound.Set(xmin,xmax,ymin,ymax,zmin,zmax);
	}
	cin >> strTemp;
	if (strTemp=="数据路径")
	{		
		cin >> buff;
		mpr_DataPath = buff;
	}

	cin >> strTemp;
	if (strTemp=="块信息")
	{
		char buff2[128];
		do
		{
			GlbTileInfo tileInfo;
			cin >> buff  >> buff2 >> tileInfo.xmin >> tileInfo.xmax >> tileInfo.ymin >> tileInfo.ymax >> tileInfo.zmin >> tileInfo.zmax;
			tileInfo.name = buff;			
			std::string filename = buff2;
			if (filename.length()>0)
				_topmostTilesMap[filename] = tileInfo;
		}while(!cin.eof());
	}

	cin.close();
	
	// 一次性加载所有的顶级块
	osg::ref_ptr<osg::MatrixTransform> transNode = new osg::MatrixTransform();
	osg::Matrixd localToWorld;
	if (mpr_isGlobe)
	{
		double x,y,z;
		g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(mpr_anchor.y()), osg::DegreesToRadians(mpr_anchor.x()),mpr_anchor.z(),x,y,z);
		g_ellipsmodel->computeLocalToWorldTransformFromXYZ(x,y,z,localToWorld);
	}
	else
	{
		localToWorld.makeTranslate(mpr_anchor.x(),mpr_anchor.y(),mpr_anchor.z());		
	}
	transNode->setMatrix(localToWorld);
	
	std::map<std::string, GlbTileInfo>::iterator itr = _topmostTilesMap.begin();
	while (itr != _topmostTilesMap.end())
	{		
		std::string filename = mpr_DataPath + "\\" + itr->second.name + "\\" + itr->first;
		osg::Node* node = osgDB::readNodeFile(filename);
		if (node)
		{
			transNode->addChild(node);
		}
		itr++;
	}

	mpr_layer_node->asSwitch()->addChild(transNode);

	return true;
}