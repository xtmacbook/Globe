#include "StdAfx.h"
#include "GlbGlobeLineModelSymbol.h"
#include "osgDB/ReadFile"
#include "GlbString.h"
#include "GlbLine.h"
#include "CGlbGlobe.h"
#include <osg/Switch>
#include <osg/MatrixTransform>
#include <osg/LineWidth>
#include <osg/BlendColor>
#include <osg/BlendFunc>
#include "GlbLog.h"

using namespace GlbGlobe;

CGlbGlobeLineModelSymbol::CGlbGlobeLineModelSymbol(void)
{
}


CGlbGlobeLineModelSymbol::~CGlbGlobeLineModelSymbol(void)
{
}

osg::Node *CGlbGlobeLineModelSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbLineModelSymbolInfo* renderInfo = dynamic_cast<GlbLineModelSymbolInfo*>(obj->GetRenderInfo());
	if (renderInfo==NULL) return NULL;
	CGlbFeature* feature = obj->GetFeature();
	CGlbWString datalocate = L"";
	osg::Group* grpNode = new osg::Group;
	if (renderInfo->modelLocate)
	{
		datalocate = renderInfo->modelLocate->GetValue(feature);
		if (datalocate.length()>0)
		{
			glbInt32 index = datalocate.find_first_of(L'.');
			if(index == 0)
			{// 处理当前执行文件的相对路径情况 ./AAA/....
				CGlbWString execDir = CGlbPath::GetExecDir();
				datalocate = execDir + datalocate.substr(1,datalocate.size());
			}
			osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile(datalocate.ToString().c_str());
			if (modelNode)
				grpNode->addChild(modelNode);
			else //if (datalocate.length()>0)
			{
				GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取线模型文件(%s)失败. \r\n",datalocate.c_str());
			}
		}
	}

	if (renderInfo->isRenderLine)
	{
		glbInt32 color = renderInfo->color->GetValue(feature);		
		glbInt32 opacity = renderInfo->opacity->GetValue(feature);
		glbInt32 width = renderInfo->width->GetValue(feature);
		CGlbLine* line = dynamic_cast<CGlbLine*>(geo);
		if (line)
		{
			glbInt32 ptCnt = line->GetCount();
			if (ptCnt>1)
			{				osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
				grpNode->addChild(trans);
				osg::ref_ptr<osg::Geode> blGeode = new osg::Geode;
				trans->addChild(blGeode.get());
				osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
				blGeode->addDrawable(geom.get());
				GlbGlobeTypeEnum globeType = obj->GetGlobe()->GetType();

				glbDouble ptx,pty,ptz;
				line->GetPoint(0,&ptx,&pty,&ptz);
				if (GLB_GLOBETYPE_GLOBE==globeType)						
					g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

				osg::Matrix m;
				osg::Vec3d origVec(ptx,pty,ptz);
				m.setTrans(origVec);
				trans->setMatrix(m);

				osg::ref_ptr<osg::Vec3Array> vtArray = new osg::Vec3Array;
				osg::ref_ptr<osg::Vec4Array> clrArray = new osg::Vec4Array;
				for (glbInt32 k = 0; k < ptCnt; k++)
				{
					line->GetPoint(k,&ptx,&pty,&ptz);						
					if (GLB_GLOBETYPE_GLOBE==globeType)						
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,ptx,pty,ptz);

					vtArray->push_back(osg::Vec3(ptx,pty,ptz)-origVec);
				}

				geom->setVertexArray(vtArray.get());
				osg::Vec4 lnColor = GetColor(color);
				lnColor.set(lnColor.r(),lnColor.g(),lnColor.b(),opacity/100.0f);
				clrArray->push_back(lnColor);
				geom->setColorArray(clrArray);
				geom->setColorBinding(osg::Geometry::BIND_OVERALL);

				geom->addPrimitiveSet(
					new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vtArray->size()));

				//线宽
				osg::LineWidth *lineWidth = new osg::LineWidth;
				lineWidth->setWidth(width);
				osg::StateSet* ss = grpNode->getOrCreateStateSet();
				ss->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);

				// 关闭光照
				//ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
				//混合是否启动
				if (opacity<100)
				{// 启用混合
					ss->setMode(GL_BLEND, osg::StateAttribute::ON);			
					if (osg::StateSet::TRANSPARENT_BIN != ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );					
				}
				else
				{// 禁用混合						
					ss->setMode(GL_BLEND, osg::StateAttribute::OFF  | osg::StateAttribute::OVERRIDE);					
					if (osg::StateSet::TRANSPARENT_BIN == ss->getRenderingHint())
						ss->setRenderingHint( osg::StateSet::OPAQUE_BIN );		
				}
			}
		}
	}

	if (grpNode->getNumChildren()>0)
	{
		return grpNode;
	}
	return NULL;
}