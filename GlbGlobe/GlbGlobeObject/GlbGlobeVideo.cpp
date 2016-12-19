#include "StdAfx.h"
#include "GlbGlobeVideo.h"
#include "GlbCalculateBoundBoxVisitor.h"
#include "GlbCalculateNodeSizeVisitor.h"
#include "GlbPoint.h"
#include "GlbGlobeTypes.h"
#include "GlbGlobeVideoSymbol.h"
#include "GlbGlobeMemCtrl.h"
#include "GlbCompareValue.h"
#include "GlbGlobeFeatureLayer.h"
#include "GlbGlobeView.h"
#include "GlbConvert.h"
#include "GlbVideoPlayer.h"
#include <osg/Material>
#include <osgDB/ReadFile>
#include <osg/TexEnv>
#include <osg/Depth>
#include <osg/Image>
#include "GlbGlobeSymbolCommon.h"
#include "GlbGlobeMath.h"
#include <osg/Point>
#include <osg/LineWidth>

using namespace GlbGlobe;

class VideoBlinkCallback : public osg::NodeCallback
{
public:
	VideoBlinkCallback(CGlbGlobeRObject *obj)
	{
		mpr_isChangeFrame = false;
		mpr_markNum = 0;
		mpr_obj = obj;
	}

	~VideoBlinkCallback()
	{
		if(mpr_obj && mpr_obj->GetOsgNode())
			mpr_obj->GetOsgNode()->setStateSet(NULL);
		mpr_obj = NULL;
	}

	void operator()(osg::Node *node,osg::NodeVisitor *nv)
	{
		if(mpr_obj)
		{
			GlbRenderInfo *renderInfo = mpr_obj->GetRenderInfo();
			if(renderInfo)
			{
				GlbVideoSymbolInfo *videoInfo = (GlbVideoSymbolInfo *)renderInfo;
				if(videoInfo)
				{
					unsigned int frameNum = 60;
					unsigned int count = nv->getFrameStamp()->getFrameNumber() - mpr_markNum;

					if(count > frameNum - 1)
					{
						mpr_markNum = nv->getFrameStamp()->getFrameNumber();
						mpr_isChangeFrame = !mpr_isChangeFrame;
						count = 0;
					}
					if(mpr_isChangeFrame)
						count = frameNum - count;
					float a = count * 1 / (float)frameNum;
					osg::Switch *videoNode = node->asSwitch();
					if(videoNode == NULL || videoNode->getNumChildren() < 1)
						return;

					//osg::ref_ptr<osg::StateSet> stateSet = videoNode->getChild(0)->asGroup()->getChild(0)->asGroup()->getChild(0)->asGeode()->getDrawable(0)->getOrCreateStateSet();
					osg::ref_ptr<osg::StateSet> stateSet = NULL;
					if(videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN)
					{
						osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform*>(
							videoNode->asGroup()->getChild(0)->asGroup()->getChild(0));
						if(mt.valid())
							stateSet = mt->asGroup()->getChild(0)->asGeode()->getDrawable(0)->getOrCreateStateSet();
					}
					else
						stateSet = videoNode->asGroup()->getChild(0)->asGroup()->getChild(0)->asGroup()->getChild(0)->asGeode()->getDrawable(0)->getOrCreateStateSet();
					stateSet->setMode(GL_BLEND,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
					stateSet->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
					stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN | osg::StateAttribute::OVERRIDE);
					osg::Material *material = 
						dynamic_cast<osg::Material *>(stateSet->getAttribute(osg::StateAttribute::MATERIAL));

					if(material == NULL)
					{
						material = new osg::Material;
						stateSet->setAttributeAndModes(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
					}
					material->setAlpha(osg::Material::FRONT_AND_BACK,a);
				}//videoInfo
			}//renderInfo
		}//mpr_obj
		traverse(node,nv);
	}

private:
	glbBool mpr_isChangeFrame;
	unsigned int mpr_markNum;
	glbref_ptr<CGlbGlobeRObject> mpr_obj;
};

class VideoEditEventHandler : public osgGA::GUIEventHandler
{
public:
	VideoEditEventHandler(CGlbGlobeVideo*obj):mpr_polygonObj(obj)
	{
		mpr_globe = mpr_polygonObj->GetGlobe();
		mpr_boundGeode = NULL;
		mpr_boundExtent = NULL;
		mpr_pointsGeode = NULL;
		mpr_isGeoChanged = false;
	}

	bool handle(const osgGA::GUIEventAdapter &ea,osgGA::GUIActionAdapter &aa)
	{
		osgViewer::Viewer *viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		if(!viewer) return false;

		viewer->getCamera()->setCullingMode(
			viewer->getCamera()->getCullingMode() & ~osg::CullStack::SMALL_FEATURE_CULLING);
		unsigned int buttonMask = ea.getButtonMask();
		static glbBool isIntersect = false;//��ʶ���������İ�Χ���ཻ
		static osg::Vec3d intersectPoint(0.0,0.0,0.0);//��¼�������Χ���ཻʱ �ཻ�ĵ�һ���㷽���������Ҫ�ƶ�������
		static glbBool isIntersectPoint = false;//��¼�����Ѿ���༭���ཻ���������ж������Ƿ������߶����ཻ
		static glbInt32 intersectIndex = -1;
		static glbBool isInRing = false;
		static glbInt32 inRingIndex = -1;

		static osg::ref_ptr<osg::MatrixTransform> moveMt = NULL;//���move�����ཻ�����ĵ㣬��¼����������ɾ��
		glbBool isGlobe = mpr_globe->GetType() == GLB_GLOBETYPE_GLOBE;

		//����move��༭���ཻ
		static glbBool isMoveIntersect = false;
		static glbInt32 moveIntersectIndex = -1;
		static glbBool isColorChanged = false;//��¼��Ϊ��껬����༭���ཻ�޸Ĺ��༭������һ�����ɫ

		static glbBool isNeedUpdate = false;//�༭��Ⱦ��Ƥ��ڵ㣬���Release����polygonObj
		static glbref_ptr<CGlbLine> updateOutRingline = NULL;//��¼���Release����polygonObj��Ҫ�ĵ㼯
		static glbref_ptr<CGlbLine> updateInRingline = NULL;
		static osg::ref_ptr<osg::MatrixTransform> bungeeMt = NULL;

		//�����ཻ��ֵ
		double pixelWidth = mpr_globe->GetView()->GetCurrentPixelWidth();

		//CallBackԭʼ���롪����������begin

		if(mpr_polygonObj->IsEdit())//���ڱ༭״̬
		{
			GlbGlobeRObEditModeEnum editMode = mpr_polygonObj->GetEditMode();
			if(mpr_boundGeode == NULL)//�����Χ��Ϊ�գ����߶��󴴽���Χ��
			{
				mpr_boundExtent = mpr_polygonObj->GetBound(true);
				osg::Vec3d center(0.0,0.0,0.0);
				mpr_boundExtent->GetCenter(&center.x(),&center.y(),&center.z());
				osg::Matrixd localToWorld;
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(center.x(),center.y(),center.z(),localToWorld);
				mpr_boundGeode = CreateBoundingBox(*mpr_boundExtent);
				osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
				mt->setMatrix(localToWorld);
				mt->addChild(mpr_boundGeode);
				mpr_polygonObj->GetEditNode()->addChild(mt);
			}

			glbref_ptr<CGlbExtent> geoExt = mpr_polygonObj->GetBound(true);
			if(*geoExt != *mpr_boundExtent)//�����Χ���б仯�����°�Χ��
			{
				mpr_boundExtent = geoExt;
				osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform *>(mpr_boundGeode->getParent(0));
				if(mt)
				{
					mt->removeChild(mpr_boundGeode);
					mpr_boundGeode = NULL;
				}
				osg::Vec3d center(0.0,0.0,0.0);
				mpr_boundExtent->GetCenter(&center.x(),&center.y(),&center.z());
				osg::Matrixd localToWorld;
				g_ellipsoidModel->computeLocalToWorldTransformFromXYZ(center.x(),center.y(),center.z(),localToWorld);
				mpr_boundGeode = CreateBoundingBox(*mpr_boundExtent);
				mt->setMatrix(localToWorld);
				mt->addChild(mpr_boundGeode);
			}

			static osg::ref_ptr<osg::MatrixTransform> mt = NULL;
			//�ߵ�Geo�����仯���±༭��ڵ�
			//Geo�ı仯���� 1�������ཻ�����ӵ�
			//                            2���϶��༭��༭��
			if(mpr_isGeoChanged || (isMoveIntersect == false && isColorChanged))
			{
				if(mt)
				{
					mpr_polygonObj->GetEditNode()->removeChild(mt);
					mt = NULL;
				}
				mpr_pointsGeode = NULL;
				if(mpr_isGeoChanged)
					mpr_isGeoChanged = false;
				if(isColorChanged)
					isColorChanged = false;
			}

			if(mpr_pointsGeode == NULL && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)
			{
				mpr_pointsGeode = new osg::Geode;
				osg::ref_ptr<osg::Geometry> pointsGeom = new osg::Geometry;
				CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
				osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
				osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
				CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
				const glbDouble *points = extRingLine->GetPoints();

				osg::Matrixd localToWorld;
				if(isGlobe)
					g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
					osg::DegreesToRadians(points[1]),osg::DegreesToRadians(points[0]),points[2],localToWorld);
				else
					localToWorld.makeTranslate(points[0],points[1],points[2]);
				osg::Matrixd worldTolocal = osg::Matrixd::inverse(localToWorld);

				for (glbInt32 i = 0; i < extRingLine->GetCount();i++)
				{
					osg::Vec3d point(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
						osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
					vertexes->push_back(point * worldTolocal);
					colors->push_back(osg::Vec4(1.0,1.0,1.0,1.0));
				}

				glbInt32 inRingCount = polygon3D->GetInRingCount();
				for(glbInt32 i = 0; i < inRingCount;i++)
				{
					CGlbLine *inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
					if(inRingLine == NULL)
						continue;

					points = inRingLine->GetPoints();
					for (glbInt32 j = 0; j < inRingLine->GetCount();j++)
					{
						osg::Vec3d point(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
						if(isGlobe)
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
							osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
						vertexes->push_back(point * worldTolocal);
						colors->push_back(osg::Vec4(1.0,1.0,1.0,1.0));
					}
				}

				pointsGeom->setVertexArray(vertexes);
				pointsGeom->setColorArray(colors);
				pointsGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
				pointsGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertexes->size()));
				osg::ref_ptr<osg::StateSet> stateset = mpr_pointsGeode->getOrCreateStateSet();
				osg::ref_ptr<osg::Point> point = new osg::Point;
				point->setSize(7);
				stateset->setAttributeAndModes(point,osg::StateAttribute::ON);
				stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
				//stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
				stateset->setRenderBinDetails(2,"RenderBin");
				osg::ref_ptr<osg::Depth> depth = new osg::Depth;
				depth->setFunction(osg::Depth::ALWAYS);
				stateset->setAttributeAndModes(depth,osg::StateAttribute::ON);
				mpr_pointsGeode->addDrawable(pointsGeom);
				mt = new osg::MatrixTransform;
				mt->setMatrix(localToWorld);
				mt->addChild(mpr_pointsGeode);
				osg::Group *editNode = mpr_polygonObj->GetEditNode();
				if(editNode)
					editNode->addChild(mt);
			}

			if(mpr_pointsGeode != NULL && mpr_polygonObj->GetEditMode() != GLB_ROBJECTEDIT_SHAPE)
			{
				osg::Group *editNode = mpr_polygonObj->GetEditNode();
				if(editNode->getNumChildren() > 1)
				{
					osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(editNode->getChild(1));
					if(mt)
					{
						mt->removeChild(mpr_pointsGeode);
						mpr_pointsGeode = NULL;
					}
				}
			}

			if(isMoveIntersect && mpr_pointsGeode)
			{
				osg::Geometry *pointsGeom = dynamic_cast<osg::Geometry *>(mpr_pointsGeode->getDrawable(0));
				osg::Vec4Array *colors = dynamic_cast<osg::Vec4Array *>(pointsGeom->getColorArray());
				osg::ref_ptr<osg::Vec4Array> newColors = new osg::Vec4Array;
				if(isInRing)
				{
					CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
					CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
					glbInt32 tempindex = moveIntersectIndex;
					//��Ȧ���е�
					moveIntersectIndex = extRingLine->GetCount();
					for(glbInt32 i = 0; i < inRingIndex; i++)
					{
						CGlbLine *inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
						moveIntersectIndex += inRingLine->GetCount();
					}
					moveIntersectIndex += tempindex;
				}
				for(glbInt32 i = 0 ; i < colors->size(); i++)
				{
					if(moveIntersectIndex == i)
						newColors->push_back(osg::Vec4(1.0,1.0,0.0,1.0));
					else
						newColors->push_back(colors->at(i));
				}
				pointsGeom->setColorArray(newColors);
				pointsGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
				pointsGeom->dirtyDisplayList();
				isColorChanged = true;
			}
		}

		//CallBackԭʼ���롪����������end
		switch(ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::PUSH):
			{
				CGlbGlobeView *view =  mpr_globe->GetView();
				osg::Vec3d start(ea.getX(),ea.getY(),0.0);
				osg::Vec3d end(ea.getX(),ea.getY(),1.0);
				view->ScreenToWorld(start.x(),start.y(),start.z());
				view->ScreenToWorld(end.x(),end.y(),end.z());
				osg::Node *node = mpr_polygonObj->GetOsgNode();
				if(!node)
					return false;

				glbref_ptr<CGlbExtent> boundExtent = mpr_polygonObj->GetBound(true);
				glbDouble minX,minY,minZ,maxX,maxY,maxZ;
				boundExtent->GetMin(&minX,&minY,&minZ);
				boundExtent->GetMax(&maxX,&maxY,&maxZ);
				osg::BoundingBox bb(minX,minY,minZ,maxX,maxY,maxZ);
				isIntersect = intersectRayBoundingBox(start,end,bb);//�ж����������İ�Χ���ཻ
				if(isIntersect == false)
					return false;

				//��ȷ���������ʱ�Ƿ���༭���ཻ���ཻ���¼�ཻ��λ��
				CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
				CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
				const glbDouble *points = extRingLine->GetPoints();
				osg::Vec3d point;
				for (glbInt32 i = 0 ; i < extRingLine->GetCount() ; i++)
				{
					point = osg::Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
						osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
					osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment(start,end);
					bool intersected = intersect3D_SegmentPoint(seg,point,1.5 * pixelWidth/*0.1*/);
					if(intersected == false)
						continue;

					isIntersectPoint = true;
					intersectIndex = i;
					break;
				}

				if(isIntersectPoint == false)
				{
					glbInt32 inRingCount = polygon3D->GetInRingCount();
					for(glbInt32 i = 0; i < inRingCount; i++)
					{
						CGlbLine *inRingLine = const_cast<CGlbLine *>(polygon3D->GetInRing(i));
						if(inRingLine == NULL)
							continue;
						points = inRingLine->GetPoints();
						for (glbInt32 j = 0 ; j < extRingLine->GetCount() ; j++)
						{
							point = osg::Vec3d(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
								osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
							osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment(start,end);
							bool intersected = intersect3D_SegmentPoint(seg,point,1.5 * pixelWidth/*0.1*/);
							if(intersected == false)
								continue;

							isIntersectPoint = true;
							isInRing = true;
							inRingIndex = i;
							intersectIndex = j;
							break;
						}
					}
				}

				//��¼��ʼ�ཻ�㣬�Ա����߶�����Ҫ�ƶ��ľ���
				osg::Vec3d center;
				boundExtent->GetCenter(&center.x(),&center.y(),&center.z());
				glbDouble radius = (center - osg::Vec3d(0.0,0.0,0.0)).length();

				std::vector<osg::Vec3d> IntersectPos;
				glbInt32 pointNum = IntersectRaySphere(start,end,osg::Vec3d(0.0,0.0,0.0),radius,IntersectPos);
				if(pointNum > 0)
					intersectPoint = IntersectPos.at(0);
				return false;
			}
		case(osgGA::GUIEventAdapter::DRAG):
			{
				if(isIntersect && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_HORIZONTAL)//DRAG��ˮƽƽ�ƶ���
				{
					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());

					glbref_ptr<CGlbExtent> extent = mpr_polygonObj->GetBound(true);
					osg::Vec3d center;
					extent->GetCenter(&center.x(),&center.y(),&center.z());
					glbDouble radius = (center - osg::Vec3d(0.0,0.0,0.0)).length();

					std::vector<osg::Vec3d> IntersectPos;
					glbInt32 pointNum = 0;
					if(isGlobe)
						pointNum = IntersectRaySphere(start,end,osg::Vec3d(0.0,0.0,0.0),radius,IntersectPos);
					else
					{
						osg::Vec3d IntersectPoint;
						osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment(start,end);
						osg::Vec3d dir(0,0,1);
						osg::Plane *plane = new osg::Plane(dir,center);
						bool Intersected = intersect3D_SegmentPlane(line,plane,center,IntersectPoint);
						if(Intersected)
							IntersectPos.push_back(IntersectPoint);
					}

					if(IntersectPos.size() > 0)
					{
						osg::Vec3d newPoint = IntersectPos.at(0);
						if(isGlobe)
						{
							osg::Vec3d tempPoint;
							g_ellipsoidModel->convertXYZToLatLongHeight(intersectPoint.x(),intersectPoint.y(),
								intersectPoint.z(),tempPoint.y(),tempPoint.x(),tempPoint.z());
							g_ellipsoidModel->convertXYZToLatLongHeight(newPoint.x(),newPoint.y(),
								newPoint.z(),newPoint.y(),newPoint.x(),newPoint.z());

							g_ellipsoidModel->convertLatLongHeightToXYZ(newPoint.y(),newPoint.x(),
								tempPoint.z(),newPoint.x(),newPoint.y(),newPoint.z());
						}

						osg::Matrixd trans;
						//trans.setTrans(osg::Vec3d(newPoint.x() - intersectPoint.x(),newPoint.y() - intersectPoint.y(),0));
						//osg::Vec3d test = newPoint - intersectPoint;
						trans.setTrans(newPoint - intersectPoint);

						CGlbPolygon *polygonGeo = mpr_polygonObj->GetGeo();
						CGlbLine *extRingLine = const_cast<CGlbLine *>(polygonGeo->GetExtRing());
						const glbDouble *points = extRingLine->GetPoints();
						glbref_ptr<CGlbPolygon> tempPolygonGeo = new CGlbPolygon(3,false);
						glbref_ptr<CGlbLine> outRing = new CGlbLine(3,false);
						for (glbInt32 i = 0 ; i < extRingLine->GetCount(); i++)
						{
							osg::Vec3d point = osg::Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
								osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
							point = point * trans;
							if(isGlobe)
							{
								g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),
									point.z(),point.y(),point.x(),point.z());
								point.x() = osg::RadiansToDegrees(point.x());
								point.y() = osg::RadiansToDegrees(point.y());
							}
							outRing->AddPoint(point.x(),point.y(),points[3 * i + 2]);
							//outRing->AddPoint(point.x(),point.y(),point.z());
						}
						tempPolygonGeo->SetExtRing(outRing.get());
						glbBool isDirectDraw = false;
						mpr_polygonObj->SetGeo(tempPolygonGeo.get(),isDirectDraw);
						mpr_polygonObj->DirectDraw(0);
						intersectPoint = newPoint;
						//return false;
						return true;
					}
				}
				else if(isIntersect && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_VERTICAL)//DRAG����ֱƽ�ƶ���
				{
					osg::Vec3d center;
					CGlbPolygon *polygonGeo = mpr_polygonObj->GetGeo();
					if(!polygonGeo)
						return false;
					CGlbExtent *lineExt = const_cast<CGlbExtent *>(polygonGeo->GetExtent());
					if(!lineExt)
						return false;
					lineExt->GetCenter(&center.x(),&center.y(),&center.z());

					osg::Vec3d focusPoint;
					osg::Vec3d cameraPoint;
					mpr_globe->GetView()->GetFocusPos(focusPoint);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(focusPoint.y()),
						osg::DegreesToRadians(focusPoint.x()),focusPoint.z(),focusPoint.x(),focusPoint.y(),focusPoint.z());
					mpr_globe->GetView()->GetCameraPos(cameraPoint);
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPoint.y()),
						osg::DegreesToRadians(cameraPoint.x()),cameraPoint.z(),cameraPoint.x(),cameraPoint.y(),cameraPoint.z());
					osg::Vec3d dir = focusPoint - cameraPoint;
					dir.normalize();
					osg::Vec3d objPoint;
					if(isGlobe)
						g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(center.y()),
						osg::DegreesToRadians(center.x()),center.z(),objPoint.x(),objPoint.y(),objPoint.z());
					else
						objPoint = center;

					osg::Plane *plane = new osg::Plane(dir,objPoint);
					osg::Vec3d outPoint;

					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());

					osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment(start,end);
					bool intersected = intersect3D_SegmentPlane(line,plane,objPoint,outPoint);
					if(intersected == false)
						return false;
					osg::Matrixd trans;
					//trans.setTrans(outPoint - intersectPoint);
					trans.setTrans(osg::Vec3d(0,0,outPoint.z() - intersectPoint.z()));

					CGlbLine *extRingLine = const_cast<CGlbLine *>(polygonGeo->GetExtRing());
					const glbDouble *points = extRingLine->GetPoints();
					glbref_ptr<CGlbPolygon> newPolygonGeo = new CGlbPolygon(3,false);
					glbref_ptr<CGlbLine> outRing = new CGlbLine(3,false);
					for (glbInt32 i = 0; i < extRingLine->GetCount(); i++)
					{
						osg::Vec3d point = osg::Vec3d(points[3 * i],points[3 * i +1],points[3 * i + 2]);
						if(isGlobe)
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
							osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
						point = point * trans;
						if(isGlobe)
							g_ellipsoidModel->convertXYZToLatLongHeight(point.x(),point.y(),point.z(),point.y(),point.x(),point.z());
						//point.x() = osg::RadiansToDegrees(point.x());
						//point.y() = osg::RadiansToDegrees(point.y());
						//outRing->AddPoint(point.x(),point.y(),point.z());
						outRing->AddPoint(points[3 * i],points[3 * i +1],point.z());
					}
					newPolygonGeo->SetExtRing(outRing.get());
					glbBool isNULL = false;
					mpr_polygonObj->SetGeo(newPolygonGeo.get(),isNULL);
					mpr_polygonObj->DirectDraw(0);
					intersectPoint = outPoint;
					//return false;
					return true;
				}
				else if(isIntersect && isIntersectPoint && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)//DRAG�±༭��̬
				{
					if(isIntersectPoint)
					{
						CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
						CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
						glbInt32 inRingCount = polygon3D->GetInRingCount();
						const glbDouble *points = extRingLine->GetPoints();
						CGlbGlobeView *view =  mpr_globe->GetView();
						osg::Vec3d start(ea.getX(),ea.getY(),0.0);
						osg::Vec3d end(ea.getX(),ea.getY(),1.0);
						view->ScreenToWorld(start.x(),start.y(),start.z());
						view->ScreenToWorld(end.x(),end.y(),end.z());

						glbref_ptr<CGlbExtent> extent = mpr_polygonObj->GetBound(true);
						osg::Vec3d center;
						extent->GetCenter(&center.x(),&center.y(),&center.z());
						glbDouble radius = (center - osg::Vec3d(0.0,0.0,0.0)).length();

						std::vector<osg::Vec3d> IntersectPos;
						glbInt32 pointNum = 0;
						if(isGlobe)
							pointNum = IntersectRaySphere(start,end,osg::Vec3d(0.0,0.0,0.0),radius,IntersectPos);
						else
						{
							osg::Vec3d IntersectPoint;
							osg::ref_ptr<osg::LineSegment> line = new osg::LineSegment(start,end);
							osg::Vec3d dir(0,0,1);
							osg::Plane *plane = new osg::Plane(dir,center);
							bool Intersected = intersect3D_SegmentPlane(line,plane,center,IntersectPoint);
							if(Intersected)
								IntersectPos.push_back(IntersectPoint);
						}
						if(IntersectPos.size() < 1)
							return false;

						//�ཻ�༭�㣬�༭�߶����Geo
						osg::Vec3d tempIntersect = IntersectPos.at(0);
						if(isGlobe)
						{
							g_ellipsoidModel->convertXYZToLatLongHeight(tempIntersect.x(),tempIntersect.y(),
								tempIntersect.z(),tempIntersect.y(),tempIntersect.x(),tempIntersect.z());
							tempIntersect.x() = osg::RadiansToDegrees(tempIntersect.x());
							tempIntersect.y() = osg::RadiansToDegrees(tempIntersect.y());
						}
						updateOutRingline = new CGlbLine(3,false);
						updateInRingline = new CGlbLine(3,false);
						osg::ref_ptr<osg::Vec3dArray> bungeeVers = new osg::Vec3dArray;
						if(isInRing == false)
						{
							glbInt32 extRingVerCnt = extRingLine->GetCount();
							for(glbInt32 j = 0; j < extRingVerCnt; j++)
							{
								if(j == intersectIndex)
								{
									updateOutRingline->AddPoint(tempIntersect.x(),tempIntersect.y(),tempIntersect.z());
									////�齨bungeeGeom����
									if(j == 0)
									{
										bungeeVers->push_back(osg::Vec3d(points[3 * (extRingVerCnt -  1)],points[3 * (extRingVerCnt - 1)+ 1],points[3 * (extRingVerCnt - 1) + 2]));
										bungeeVers->push_back(osg::Vec3d(tempIntersect.x(),tempIntersect.y(),tempIntersect.z()));
										bungeeVers->push_back(osg::Vec3d(points[3 * (j + 1)],points[3 * (j + 1)+ 1],points[3 * (j + 1) + 2]));
									}
									else if(j == extRingVerCnt - 1)
									{
										bungeeVers->push_back(osg::Vec3d(points[3 * (j -  1)],points[3 * (j - 1)+ 1],points[3 * (j - 1) + 2]));
										bungeeVers->push_back(osg::Vec3d(tempIntersect.x(),tempIntersect.y(),tempIntersect.z()));
										bungeeVers->push_back(osg::Vec3d(points[0],points[1],points[2]));
									}
									else
									{
										bungeeVers->push_back(osg::Vec3d(points[3 * (j -  1)],points[3 * (j - 1)+ 1],points[3 * (j - 1) + 2]));
										bungeeVers->push_back(osg::Vec3d(tempIntersect.x(),tempIntersect.y(),tempIntersect.z()));
										bungeeVers->push_back(osg::Vec3d(points[3 * (j + 1)],points[3 * (j + 1)+ 1],points[3 * (j + 1) + 2]));
									}
								}
								else
									updateOutRingline->AddPoint(points[3 * j],points[3 * j + 1],points[3 * j + 2]);
							}
						}

						//ɾ���ϵ�
						osg::Group *editNode = mpr_polygonObj->GetEditNode();
						if(bungeeMt && editNode)
						{
							editNode->removeChild(bungeeMt);
							bungeeMt = NULL;
						}

						//������Ƥ��ڵ�
						osg::ref_ptr<osg::Geode> bungeeGeode = new osg::Geode;
						osg::ref_ptr<osg::Geometry> bungeeGeom = new osg::Geometry;
						osg::ref_ptr<osg::Vec3dArray> vertexes = new osg::Vec3dArray;
						osg::ref_ptr<osg::Vec4Array> color = new osg::Vec4Array;

						osg::Matrixd localToWorld;
						if(isGlobe)
							g_ellipsoidModel->computeLocalToWorldTransformFromLatLongHeight(
							osg::DegreesToRadians(points[1]),osg::DegreesToRadians(points[0]),points[2],localToWorld);
						else
							localToWorld.makeTranslate(points[0],points[1],points[2]);
						osg::Matrixd worldTolocal = osg::Matrixd::inverse(localToWorld);

						for (glbInt32 i = 0; i < bungeeVers->size(); i++)
						{
							osg::Vec3d temPoint(0.0,0.0,0.0);
							if(isGlobe)
								g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(bungeeVers->at(i).y()),
								osg::DegreesToRadians(bungeeVers->at(i).x()),bungeeVers->at(i).z(),temPoint.x(),temPoint.y(),temPoint.z());
							else
								temPoint = osg::Vec3d(bungeeVers->at(i).x(),bungeeVers->at(i).y(),bungeeVers->at(i).z());
							vertexes->push_back(temPoint * worldTolocal);
						}
						color->push_back(osg::Vec4(1.0,0.0,0.0,1.0));

						bungeeGeom->setVertexArray(vertexes);
						bungeeGeom->setColorArray(color);
						bungeeGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
						bungeeGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vertexes->size()));
						osg::ref_ptr<osg::StateSet> stateset = bungeeGeode->getOrCreateStateSet();
						osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
						lineWidth->setWidth(3);//��Ҫ���������޸Ĵ�С,����osg�������ԣ�������õľ�����Ļ���ش�С
						stateset->setAttributeAndModes(lineWidth,osg::StateAttribute::ON);
						stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
						stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
						bungeeGeode->addDrawable(bungeeGeom);
						bungeeMt = new osg::MatrixTransform;
						bungeeMt->setMatrix(localToWorld);
						bungeeMt->addChild(bungeeGeode);
						if(editNode)
							editNode->addChild(bungeeMt);

						isNeedUpdate = true;
						return true;
					}
				}
			}
		case(osgGA::GUIEventAdapter::MOVE):
			{
				//û����༭���ཻ�����Ե������ཻʱҪ��ӱ༭�򣬲��ཻ��ʱ���Ƴ��༭��
				if(isIntersectPoint == false && mpr_polygonObj->GetEditMode() == GLB_ROBJECTEDIT_SHAPE)
				{
					CGlbGlobeView *view =  mpr_globe->GetView();
					osg::Vec3d start(ea.getX(),ea.getY(),0.0);
					osg::Vec3d end(ea.getX(),ea.getY(),1.0);
					view->ScreenToWorld(start.x(),start.y(),start.z());
					view->ScreenToWorld(end.x(),end.y(),end.z());
					osg::Node *node = mpr_polygonObj->GetOsgNode();
					if(!node)
						return false;

					glbref_ptr<CGlbExtent> boundExtent = mpr_polygonObj->GetBound(true);
					glbDouble minX,minY,minZ,maxX,maxY,maxZ;
					boundExtent->GetMin(&minX,&minY,&minZ);
					boundExtent->GetMax(&maxX,&maxY,&maxZ);
					osg::BoundingBox bb(minX,minY,minZ,maxX,maxY,maxZ);
					glbBool tempIsIntersect = intersectRayBoundingBox(start,end,bb);
					if(tempIsIntersect == false)
						return false;

					CGlbPolygon *polygon3D = mpr_polygonObj->GetGeo();
					CGlbLine *extRingLine = const_cast<CGlbLine *>(polygon3D->GetExtRing());
					const glbDouble *points = extRingLine->GetPoints();

					//����move��༭���ཻʱ�������ཻ�༭����ɫ
					isMoveIntersect = false;
					moveIntersectIndex = -1;
					isInRing = false;
					inRingIndex = -1;
					osg::Vec3d point;
					for (glbInt32 i = 0 ; i < extRingLine->GetCount() ; i++)
					{
						point = osg::Vec3d(points[3 * i],points[3 * i + 1],points[3 * i + 2]);
						if(isGlobe)
							g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(point.y()),
							osg::DegreesToRadians(point.x()),point.z(),point.x(),point.y(),point.z());
						osg::ref_ptr<osg::LineSegment> seg = new osg::LineSegment(start,end);
						bool intersected = intersect3D_SegmentPoint(seg,point,1.5 * pixelWidth/*0.1*/);
						if(intersected == false)
							continue;
						//����i��λ�ñ༭�����ɫ
						isMoveIntersect = true;
						moveIntersectIndex = i;
						break;
					}
				}
				return false;
			}
		case(osgGA::GUIEventAdapter::RELEASE):
			{
				//����polygonObject
				if(isInRing == false && true == isNeedUpdate)
				{
					glbref_ptr<CGlbPolygon> polygonGeo = new CGlbPolygon(3,false);
					if(updateOutRingline)
					{
						polygonGeo->SetExtRing(updateOutRingline.get());
						glbBool isNull = false;
						CGlbPolygon *oldPolygon3D = mpr_polygonObj->GetGeo();
						for(glbInt32 i = 0; i < oldPolygon3D->GetInRingCount(); i++)
						{
							CGlbLine *inRingLine = const_cast<CGlbLine *>(oldPolygon3D->GetInRing(i));
							polygonGeo->AddInRing(inRingLine);
						}
						mpr_polygonObj->SetGeo(polygonGeo.get(),isNull);
						mpr_isGeoChanged = true;
						mpr_polygonObj->DirectDraw(0);
						osg::Group *group = mpr_polygonObj->GetEditNode();
						if(group && bungeeMt)
						{
							group->removeChild(bungeeMt);
							bungeeMt = NULL;
						}

						isNeedUpdate = false;
					}
				}

				isIntersectPoint = false;
				intersectIndex = -1;
				isInRing = false;
				inRingIndex = -1;

				return false;
			}
		default:
			return false;//return false�������������¼���return true�����ٵ��������¼�
		}
		return false;
	}
private:
	glbref_ptr<CGlbGlobe> mpr_globe;
	glbref_ptr<CGlbGlobeVideo> mpr_polygonObj;
	osg::ref_ptr<osg::Geode> mpr_boundGeode;
	glbref_ptr<CGlbExtent> mpr_boundExtent;
	osg::ref_ptr<osg::Geode> mpr_pointsGeode;
	glbBool mpr_isGeoChanged;
};

#define VideoSize 10
static glbBool index_used[VideoSize] = {false};

CGlbGlobeVideo::CGlbGlobeVideo(void)
{
	mpr_readData_critical.SetName(L"video_readdata");
	mpr_addToScene_critical.SetName(L"video_addscene");
	//InitializeCriticalSection(&mpr_readData_critical);
	//InitializeCriticalSection(&mpr_addToScene_critical);
	//InitializeCriticalSection(&mpr_createNode_critical);
	mpr_distance = DBL_MAX;
	mpr_renderInfo = NULL;
	mpr_outline = NULL;
	mpr_elevation = 0.0;
	mpr_isAltitudeChanged = false;
	mpr_needReReadData = false;
	mpr_objSize = 0;
	mpr_isCanDrawImage = true;
	mpr_videoPlayer = NULL;
	mpr_isNeedRecoveState = false;
}


CGlbGlobeVideo::~CGlbGlobeVideo(void)
{
	osg::ref_ptr<VideoPlayer> vp = dynamic_cast<VideoPlayer *>(mpr_videoPlayer.get());
	if(vp.valid())
		vp->quit();
	mpr_videoPlayer = NULL;
	//DeleteCriticalSection(&mpr_readData_critical);
	//DeleteCriticalSection(&mpr_addToScene_critical);
	//DeleteCriticalSection(&mpr_createNode_critical);
}

CGlbGlobeVideo::CGlbGlobeVideoTask::CGlbGlobeVideoTask( CGlbGlobeVideo *obj,glbInt32 level )
{
	mpr_obj = obj;
	mpr_level = level;
}

CGlbGlobeVideo::CGlbGlobeVideoTask::~CGlbGlobeVideoTask()
{
	mpr_obj = NULL;
}

glbBool CGlbGlobeVideo::CGlbGlobeVideoTask::doRequest()
{
	if(mpr_obj)
	{
		mpr_obj->ReadData(mpr_level);
		return true;
	}
	return false;
}

CGlbGlobeObject * CGlbGlobeVideo::CGlbGlobeVideoTask::getObject()
{
	return mpr_obj.get();
}

GlbGlobeObjectTypeEnum CGlbGlobeVideo::GetType()
{
	return GLB_OBJECTTYPE_VIDEO;
}

glbBool CGlbGlobeVideo::Load(xmlNodePtr *node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Load(node,prjPath);
	xmlChar *szKey = NULL;
	xmlNodePtr pnode = *node;

	glbInt32 pointsCount = 0;
	if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)"OutRingPointsCount")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf((char*)szKey,"%d",&pointsCount);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	std::string index = "outRingPointsIndex";
	glbDouble tempData0,tempData1,tempData2;

	mpr_polygonGeo = new CGlbPolygon(3,false);
	glbref_ptr<CGlbLine> outRing = new CGlbLine(3,false);
	for(glbInt32 i = 0; i < pointsCount * 3;i++)
	{
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char *)szKey,"%lf",&tempData0);
			xmlFree(szKey);

			pnode = pnode->next;
			index = "outRingPointsIndex";
		}

		i++;
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char *)szKey,"%lf",&tempData1);
			xmlFree(szKey);

			pnode = pnode->next;
			index = "outRingPointsIndex";
		}

		i++;
		index += CGlbConvert::Int32ToStr(i);
		if(pnode && (!xmlStrcmp(pnode->name,(const xmlChar *)index.c_str())))
		{
			szKey = xmlNodeGetContent(pnode);
			sscanf((char *)szKey,"%lf",&tempData2);
			xmlFree(szKey);

			pnode = pnode->next;
			index = "outRingPointsIndex";
		}

		outRing->AddPoint(tempData0,tempData1,tempData2);
	}
	mpr_polygonGeo->SetExtRing(outRing.get());

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"RenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		xmlNodePtr rdchild = pnode->xmlChildrenNode;

		if (rdchild && (!xmlStrcmp(rdchild->name, (const xmlChar *)"RenderType")))
		{
			szKey = xmlNodeGetContent(rdchild);
			sscanf((char*)szKey,"%d",&rdtype);	
			xmlFree(szKey);

			rdchild = rdchild->next;
		}
		GlbVideoSymbolInfo* videoInfo = new GlbVideoSymbolInfo();
		videoInfo->Load(rdchild,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)videoInfo;
	}
	return true;
}

glbBool CGlbGlobeVideo::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	CGlbGlobeRObject::Save(node,prjPath);

	char str[32];
	CGlbLine *ring = const_cast<CGlbLine *>(mpr_polygonGeo->GetExtRing());
	glbInt32 pointsCount = ring->GetCount();
	sprintf(str,"%d",pointsCount);
	xmlNewTextChild(node,NULL,BAD_CAST "OutRingPointsCount",BAD_CAST str);

	const glbDouble *points = ring->GetPoints();
	std::string index = "outRingPointsIndex";
	for (glbInt32 i = 0; i < pointsCount * 3; i++)
	{
		std::string index = "outRingPointsIndex";
		index += CGlbConvert::Int32ToStr(i);
		sprintf_s(str,"%8lf",points[i]);
		xmlNewTextChild(node,NULL,(unsigned char *)index.c_str(),BAD_CAST str);
	}

	xmlNodePtr rdnode = xmlNewNode(NULL,BAD_CAST "RenderInfo");
	xmlAddChild(node,rdnode);
	if(mpr_renderInfo)
	{
		sprintf_s(str,"%d",mpr_renderInfo->type);
		xmlNewTextChild(rdnode, NULL, BAD_CAST "RenderType", BAD_CAST str);
		((GlbVideoSymbolInfo*)mpr_renderInfo.get())->Save(rdnode,prjPath);				
	}
	return true;
}

glbDouble CGlbGlobeVideo::GetDistance(osg::Vec3d &cameraPos,glbBool isCompute)
{
	if (isCompute == false)
		return mpr_distance;
	if (!mpt_globe)	return DBL_MAX;

	if (mpt_isCameraAltitudeAsDistance)	
		mpr_distance = cameraPos.z();	
	else
	{
		osg::Vec3d position;
		if (mpt_feature)
		{
			CGlbExtent* ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
			ext->GetCenter(&position.x(),&position.y(),&position.z());	
		}
		else
		{
			if(mpr_polygonGeo == NULL)
				return DBL_MAX;
			CGlbExtent *polyExtent = const_cast<CGlbExtent *>(mpr_polygonGeo->GetExtent());
			polyExtent->GetCenter(&position.x(),&position.y(),&position.z());
		}
		ComputePosByAltitudeAndGloleType(position);
		osg::Vec3d cameraPos_w = cameraPos;
		if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(cameraPos.y()),
			osg::DegreesToRadians(cameraPos.x()),cameraPos.z(),cameraPos_w.x(),cameraPos_w.y(),cameraPos_w.z());
		mpr_distance = (position - cameraPos_w).length();
	}
	mpr_distance = mpr_distance - 0.001;
	return mpr_distance;//ǿ���޸�distance��ֹ���ȸ��ݱ��ߵ�distance��ͬ��video���˵�
}

void CGlbGlobeVideo::LoadData(glbInt32 level)
{
	if(mpt_isEdit == true) return;
	if(mpt_loadState == true) return;//����װ�أ�ֱ�ӷ���
	if(mpt_currLevel == level) return;
	GlbVideoSymbolInfo *videoSymbol = dynamic_cast<GlbVideoSymbolInfo *>(mpr_renderInfo.get());
	if (videoSymbol==NULL||(videoSymbol->type != GLB_OBJECTTYPE_VIDEO && mpt_currLevel != -1)) 
		return;
	mpt_loadState = true;
	if(mpt_currLevel != level)
	{
		if(mpt_currLevel < level)
			level = mpt_currLevel + 1;
	}
	glbref_ptr<CGlbGlobeVideoTask> task = new CGlbGlobeVideoTask(this,level);
	mpt_globe->mpr_taskmanager->AddTask(task.get());
}

void CGlbGlobeVideo::AddToScene()
{
	if(mpt_node == NULL)
		return;
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	bool needUpdate = false;
	if(mpt_preNode == NULL)
	{//û�в�����osg�ڵ�
		if(mpt_HangTaskNum == 0
			&& mpt_node->getNumParents() == 0
			/*&& mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN*/)
		{//����ģʽ,�ڵ㲻��Ҫ��.
			//��mpt_node����
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;
			needUpdate = true;
		}			
	}
	else
	{//��������osg�ڵ�						
		{//�ɽڵ��Ѿ��ҵ�������
			//ժ�ɽڵ�mpt_preNode������
			if(mpr_isNeedRecoveState)
			{
				GlbVideoSymbolInfo* videoInfo = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
				if (!videoInfo)
					return;
				osg::Node *terrainNode = mpt_globe->GetTerrain()->GetNode();
				osg::StateSet *ss = terrainNode->getStateSet();
				if(ss)
				{
					CGlbString recoveNumStr = CGlbConvert::Int32ToStr(mpt_videoIndex);
					recoveNumStr = "recoverState" + recoveNumStr;
					if(ss->getUniform(recoveNumStr.c_str()))
					{
						ss->getUniform(recoveNumStr.c_str())->set(true);
						index_used[mpt_videoIndex - 1] = false;
					}
				}
				mpr_isNeedRecoveState = false;
			}

			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
			task->mpr_size = this->ComputeNodeSize(mpt_preNode);
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			mpt_preNode = NULL;
			CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(task->mpr_size);
		}
		//if(mpt_altitudeMode != GLB_ALTITUDEMODE_ONTERRAIN)
		{//���½ڵ�mpt_node����
			CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();				
			mpt_globe->mpr_p_callback->AddHangTask(task);
			mpt_HangTaskNum ++;	
		}
		needUpdate = true;
	}
	if(needUpdate)
	{
		// 		//UpdateWorldExtent();
		// 		if (mpt_node && !mpr_isaddCB)
		// 		{	
		// 			//�ҽ�һ���ڵ�ص�
		// 			osg::ref_ptr<CGlbGlobeVideoObjCallback> vc = new CGlbGlobeVideoObjCallback(this);
		// 			mpt_node->addUpdateCallback(vc.get());
		// 			mpr_isaddCB = true;
		// 		}

		mpt_globe->UpdateObject(this);
		glbBool isnew = mpt_isDispShow && mpt_isShow;
		if (isnew == false)			
			mpt_node->asSwitch()->setValue(0,false);
		if (mpt_isSelected)
			DealSelected();
		if(mpt_isBlink)
			DealBlink();
	}
}

glbInt32 CGlbGlobeVideo::RemoveFromScene(glbBool isClean)
{
	SetEdit(false);
	GlbVideoSymbolInfo* videoInfo = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
	if (!videoInfo)
		return -1;
	if(videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN)
	{
		osg::Node *terrainNode = mpt_globe->GetTerrain()->GetNode();
		osg::StateSet *ss = terrainNode->getStateSet();
		if(ss)
		{
			CGlbString recoveNumStr = CGlbConvert::Int32ToStr(mpt_videoIndex);
			recoveNumStr = "recoverState" + recoveNumStr;
			if(ss->getUniform(recoveNumStr.c_str()))
			{
				ss->getUniform(recoveNumStr.c_str())->set(true);
				index_used[mpt_videoIndex - 1] = false;
			}
		}
	}
	{//��LoadData����.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)return 0;	
		mpt_loadState = true;
		/*
		*    mpt_node = node1,pre=NULL
		*    ��:mpt_node = node2,pre=node1		
		*/
	}
	GlbScopedLock<GlbCriticalSection> lock(mpr_addToScene_critical);
	//����������	
	//if(    mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN )
	//{		
	//	DirtyOnTerrainObject();
	//}
	glbInt32 tsize = 0;
	if(isClean == false)
	{//���Դ�ж�ض��󣬽�Լ�Դ�.
		if ((mpt_node != NULL))// && (mpt_node->getNumParents() != 0))
		{//obj���Դ�	
			/*
			mpt_node �Ѿ��ڹҶ��У����ǻ�û�ҵ���������
			��ʱ�ж�getNumParents() != 0 �ǲ����Ե�.
			*/
			CGlbGlobeCallBack::CGlbGlobeCallBackTask *task = 
				new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
			task->mpr_size = this->GetOsgNodeSize();
			mpt_globe->mpr_p_callback->AddRemoveTask(task);
			tsize = task->mpr_size;
		}
	}
	else
	{//���ڴ���ж�ض��󣬽�Լ�ڴ�
		//ɾ�� ��һ��װ�صĽڵ�		
		if (mpt_preNode != NULL)
		{
			//if(mpt_preNode->getNumParents() > 0 )
			{				
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_preNode);
				task->mpr_size = ComputeNodeSize(mpt_preNode);
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			tsize += ComputeNodeSize(mpt_preNode);
			mpt_preNode = NULL;
		}
		//ɾ����ǰ�ڵ�
		if (mpt_node != NULL) 
		{
			//if(mpt_node->getNumParents() > 0)
			{		
				CGlbGlobeCallBack::CGlbGlobeCallBackTask* task = 
					new CGlbGlobeCallBack::CGlbGlobeCallBackTask(this,mpt_node);
				task->mpr_size = this->GetOsgNodeSize();
				mpt_globe->mpr_p_callback->AddRemoveTask(task);
			}
			tsize       += this->GetOsgNodeSize();			
			mpt_node    =  NULL;
			mpr_objSize =  0;
		}		
		if (mpt_feature != NULL && mpt_featureLayer != NULL)
		{
			CGlbGlobeFeatureLayer* fl = dynamic_cast<CGlbGlobeFeatureLayer*>(mpt_featureLayer);
			if (fl)
				fl->NotifyFeatureDelete(mpt_feature->GetOid());
		}
		mpt_currLevel = -1;					
	}
	mpt_loadState = false;
	//����ʹ���ڴ�
	if(tsize>0 && isClean)
	{
		CGlbGlobeMemCtrl::GetInstance()->DecrementUsedMem(tsize);		
	}
	return tsize;	
}

glbref_ptr<CGlbExtent> CGlbGlobeVideo::GetBound(glbBool isWorld)
{
	if (mpt_globe == NULL && isWorld) return NULL;
	CGlbExtent * geoExt = NULL;
	if( mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN )
		return NULL;

	if(mpt_feature)
	{
		geoExt =  const_cast<CGlbExtent *>(mpt_feature->GetExtent());
	}
	else if(mpr_polygonGeo)
	{
		geoExt = const_cast<CGlbExtent *>(mpr_polygonGeo->GetExtent());	
	}

	if(isWorld == false)
	{//��������
		/*
		*   �������꣬����Ӧ��Ե��λ��Ƶĵ��κ���.
		*/
		return geoExt;
	}
	else
	{//��������		
		if(geoExt == NULL)return NULL;
		CGlbExtent* worldExt = new CGlbExtent();
		GlbGlobeTypeEnum globeType = mpt_globe->GetType();

		if(   mpt_node != NULL
			&&mpt_node->getBound().valid() )
		{
			osg::ref_ptr<osg::Node> node = mpt_node;
			//CGlbCalculateBoundBoxVisitor bboxV;
			//node->accept(bboxV);
			//osg::BoundingBoxd bb =bboxV.getBoundBox();
			//worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
			//worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
			osg::BoundingSphere bs = node->getBound();
			osg::BoundingBoxd bb;
			bb.expandBy(bs._center - osg::Vec3d(bs._radius,bs._radius,bs._radius));
			bb.expandBy(bs._center + osg::Vec3d(bs._radius,bs._radius,bs._radius));
			worldExt->SetMin(bb.xMin(),bb.yMin(),bb.zMin());
			worldExt->SetMax(bb.xMax(),bb.yMax(),bb.zMax());
		}
		else
		{//osg �ڵ㻹û����.				
			double x,y,z;
			geoExt->GetMin(&x,&y,&z);
			if( mpr_elevation == DBL_MAX)
				UpdateElevate();
			if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			{					
				z += mpr_elevation;
			}
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);					
			}
			worldExt->SetMin(x,y,z);

			geoExt->GetMax(&x,&y,&z);
			if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			{					
				z += mpr_elevation;
			}
			if(globeType == GLB_GLOBETYPE_GLOBE)
			{
				g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(y),
					osg::DegreesToRadians(x),z,x,y,z);
			}
			worldExt->SetMax(x,y,z);
		}
		return worldExt;
	}//��������	
	return NULL;
}

void CGlbGlobeVideo::UpdateElevate()
{
	if (!mpt_globe) return;
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)//��ʱ��֧������ģʽ
		return;

	glbDouble elevation = 0.0;
	CGlbExtent *ext = NULL;
	if (mpt_feature)
		ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
	else if(mpr_polygonGeo)
		ext = const_cast<CGlbExtent *>(mpr_polygonGeo->GetExtent());

	if(!ext)
		return;

	glbDouble xOrLon,yOrLat;
	ext->GetCenter(&xOrLon,&yOrLat);
	elevation = mpt_globe->GetElevationAt(xOrLon,yOrLat);

	if(fabs(elevation - mpr_elevation) < 0.0001)
		return;

	mpr_elevation = elevation;
	//???���������ģʽֱ��return �ǲ��ǿ���ֱ�ӷŵ������Ŀ�ʼ
	if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		DealModePosByChangeAltitudeOrChangePos();
}

IGlbGeometry* CGlbGlobeVideo::GetOutLine()
{
	if(mpr_outline != NULL)
		return mpr_outline.get();
	if(mpt_feature)
	{
		CGlbExtent* ext = const_cast<CGlbExtent *>(mpt_feature->GetExtent());
		if(ext == NULL)return NULL;
		double x,y,z;
		ext->GetCenter(&x,&y,&z);
		return new CGlbPoint(x,y,z);
	}else{
		//return new CGlbPoint(mpr_xOrLon,mpr_yOrLat,mpr_zOrAlt);
	}
}

glbBool CGlbGlobeVideo::SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw)
{
	if (renderInfo == NULL || renderInfo->type != GLB_OBJECTTYPE_VIDEO)
		return false;
	GlbVideoSymbolInfo* videoInfo = dynamic_cast<GlbVideoSymbolInfo*>(renderInfo);
	if (!videoInfo)
		return false;
	isNeedDirectDraw = false;
	{//��LoadData����.
		/*
		*   ������Ϊ�� ReadDataִ������;ʱ�����ɵĽ�����ܲ��ᷴӦ �����õ�����.
		*/
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			isNeedDirectDraw = true;
			mpr_renderInfo   = renderInfo;
			return true;			
		}
		mpt_loadState = true;
	}
	if(mpt_node == NULL)
	{		
		mpr_renderInfo   = renderInfo;
		mpt_loadState    = false;
		//isNeedDirectDraw = true;
		return true;
	}
	glbref_ptr<GlbVideoSymbolInfo> oldvideoInfo = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
	if (!oldvideoInfo)
	{
		mpr_renderInfo = renderInfo;
		isNeedDirectDraw = true;
		mpt_loadState  = false;
		return true;
	}

	if(oldvideoInfo->symType != videoInfo->symType)
	{
		if(videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN
			&& oldvideoInfo->symType != GLB_VIDEOSYMBOL_TERRAIN)
		{
			mpr_renderInfo = renderInfo;
			isNeedDirectDraw = true;
			mpt_loadState  = false;
			return true;
		}
	}

	if(oldvideoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN
		&& videoInfo->symType != GLB_VIDEOSYMBOL_TERRAIN)
	{
		mpr_isNeedRecoveState = true;
		mpr_renderInfo = renderInfo;
		isNeedDirectDraw = true;
		mpt_loadState  = false;
		return true;
	}
	
	glbBool isOK = false;
	if(videoInfo->symType == GLB_VIDEOSYMBOL_FREE
		|| videoInfo->symType == GLB_VIDEOSYMBOL_BILLBOARD)
		isOK = DealBillboardSymbolChange(videoInfo,isNeedDirectDraw);
	else
		isOK = DealTerrainSymbolChange(videoInfo,isNeedDirectDraw);

	if(isOK && isNeedDirectDraw)
	{
		mpr_renderInfo = renderInfo;
		mpt_loadState  = false;
		return true;
	}

	return isOK;
}

GlbRenderInfo* CGlbGlobeVideo::GetRenderInfo()
{
	return mpr_renderInfo.get();
}

glbBool CGlbGlobeVideo::SetShow(glbBool isShow,glbBool isOnState)
{
	if(mpt_node == NULL && isOnState==true)
		return false;	

	if (mpt_isEdit && isOnState)// �༭״̬�����ɵ��ȿ�������
		return true;
	/*
	* ���������ݿɼ���Χ���ƶ�����ʾ�����ش�ʱisOnState==true,isShow==true��false
	*/

	glbBool isold = mpt_isDispShow && mpt_isShow;

	if(isOnState)
	{// ��������������
		mpt_isDispShow = isShow;
	}
	else
	{// �û���������
		mpt_isShow = isShow;
	}

	glbBool isnew = mpt_isDispShow && mpt_isShow;

	if(isold == isnew)
		return true;

	glbref_ptr<GlbVideoSymbolInfo> videoInfo = static_cast<GlbVideoSymbolInfo *>(mpr_renderInfo.get());
	if (!videoInfo)	return false;	

	if( videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN )	//ͶӰ������
	{
		if (mpt_node)
		{
			osg::ref_ptr<Geode> videoGeode = NULL;
			osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform*>(
				mpt_node->asGroup()->getChild(0)->asGroup()->getChild(0));
			if(mt.valid())
				videoGeode = mt->asGroup()->getChild(0)->asGeode();

			if(!videoGeode.valid())
				return false;
			if(isnew)
			{
				mpr_videoPlayer->play();
				videoGeode->setNodeMask(1);
			}
			else
			{
				mpr_videoPlayer->pause();
				videoGeode->setNodeMask(0);
			}
		}
	}
	else if (videoInfo->symType == GLB_VIDEOSYMBOL_BILLBOARD ||
		          videoInfo->symType == GLB_VIDEOSYMBOL_FREE)	//ͶӰ������
	{
		if (mpt_node)
		{
			if(isnew)
			{
				mpr_videoPlayer->play();
				mpt_node->asSwitch()->setAllChildrenOn();
			}
			else
			{
				mpr_videoPlayer->pause();
				mpt_node->asSwitch()->setAllChildrenOff();
			}
		}
	}	
	return true;
}

glbBool CGlbGlobeVideo::SetSelected(glbBool isSelected)
{
	if(mpt_isSelected == isSelected) return true;
	mpt_isSelected = isSelected;
	if(mpt_node != NULL)
		DealSelected();
	return true;
}

glbBool CGlbGlobeVideo::SetBlink(glbBool isBlink)
{
	if(mpt_isBlink == isBlink) return true;
	mpt_isBlink = isBlink;
	if(mpt_node != NULL)
		DealBlink();
	return true;
}

void CGlbGlobeVideo::DirectDraw(glbInt32 level)
{
	if (mpt_parent    == NULL) return;	
	{//��LoadData����.
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			LoadData->ReadData��ѡ���˳������ǻ�����������⣺
			LoadData->ReadData ʹ�õ��Ǿɵ���Ⱦ��ص����ԣ����ܷ�ӳ�����õ���Ⱦ����.
			������ֱ���˳������ѡ��ȴ�LoadData->ReadData�Ļ�������������̣߳���������

			��������һ����־��ReadData ִ�е�ĩβʱ�����ñ�־,�����ʾ��true��ReadData��ִ��һ��.
			*/
			mpr_needReReadData = true;
			return;
		}
		mpt_loadState = true;
	}
	if(mpt_preNode != NULL)
	{
		/*
		*    mpr_node = node2,preNode = node1  ----A�̵߳���AddToScene��
		*                                         ��û��preNode Remove��ûִ����
		*    ����ReadData �Ѿ����꣺
		*    mpr_ndoe = node3,preNode = node2   ���node1��Զ����ժ��.
		*/
		AddToScene();
	}
	/*
	*    ��Ϊ��LoadData->ReadDataʱ��ReadData װ�ض�����ֳ����ڴ��޶�
	*        �������¼��ص�osg�ڵ�.
	*    ����DirectDraw���ԣ���������µ�osg�ڵ㣬�Ͳ��ܷ�ӳ���µı仯.
	*    ���ԣ�
	*         ReadData��������һ�������������DirectDraw���õ�,�����Ƿ񳬹�
	*         �ڴ��޶�,���������µ�osg�ڵ�.
	*/
	ReadData(level,true);
	AddToScene();
	mpt_loadState = false;
}

glbBool CGlbGlobeVideo::SetAltitudeMode(GlbAltitudeModeEnum mode)
{
	return false;
}

/*
ֻ�������λ���������.
*/
void CGlbGlobeVideo::SetRenderOrder(glbInt32 order)
{
	if (mpt_renderOrder == order)return;
	CGlbGlobeREObject::SetRenderOrder(order);
	if (mpt_altitudeMode==GLB_ALTITUDEMODE_ONTERRAIN)
	{
		DirtyOnTerrainObject();				
	}	
}

glbBool CGlbGlobeVideo::SetGeo( CGlbPolygon *polygon,glbBool &isNeedDirectDraw )
{
	isNeedDirectDraw = false;
	polygon = RemoveRepeatePoints(polygon);
	if(mpt_feature)
	{// �����Ҫ�أ�λ����Ϣ��Դ��Ҫ��.
		mpr_polygonGeo = polygon;//?????������
		return true;
	}

	{
		GlbScopedLock<GlbCriticalSection> lock(mpr_readData_critical);
		if (mpt_loadState == true)
		{
			/*
			*  �������ReadData��ReadData�������µĵ���ģʽ֮ǰʹ���˾ɵ�λ��.
			*  ���ԣ�
			��Ҫ��֪�����ߣ�����DirectDraw.
			*/
			mpr_polygonGeo = polygon;
			isNeedDirectDraw = true;						
			return true;
		}
		mpt_loadState = true;
	}

	mpr_polygonGeo = polygon;
	if (mpt_globe)// ���¿ռ���������λ��
		mpt_globe->UpdateObject(this);
	if (mpt_node != NULL)
	{
		isNeedDirectDraw = true;
		mpt_currLevel = -1;
	}
	mpt_loadState = false;
	return true;
}

glbBool CGlbGlobeVideo::SetAltitudeMode( GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw)
{
	return false;
}

void CGlbGlobeVideo::ReadData( glbInt32 level,glbBool isDirect)
{
	glbref_ptr<GlbVideoSymbolInfo> videoInfo = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
	if (videoInfo == NULL || (mpt_feature == NULL && mpr_polygonGeo == NULL))
	{
		mpt_loadState=false;
		return;
	}

	if(mpt_altitudeMode == GLB_ALTITUDEMODE_ONTERRAIN)
	{
		mpt_loadState = false;
		return;
	}

	if(videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN)
	{
		for(glbInt32 i = 0; i < VideoSize;i++)
		{
			if(index_used[i] == false)
			{
				index_used[i] = true;
				mpt_videoIndex = i + 1;
				break;
			}
		}
	}

	//�ƺ����ÿ���feature�ɣ�����
	osg::Node *node = NULL;
	CGlbGlobeVideoSymbol vidioSymbol;
	if(!mpr_videoPlayer.valid())
	{
		mpr_videoPlayer = new VideoPlayer;
		SetCustomData(mpr_videoPlayer.get());
	}
	node = vidioSymbol.Draw(this,mpr_polygonGeo.get());

	//glbref_ptr<GlbVideoSymbolInfo> info = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
	//if(node && info->symType == GLB_VIDEOSYMBOL_TERRAIN)
	//{
	//	OverlayNode *overlayNode = NULL;
	//	if(mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	//		overlayNode = dynamic_cast<OverlayNode*>(node);
	//	else if(mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	//	{
	//		osg::CoordinateSystemNode *csn = dynamic_cast<osg::CoordinateSystemNode*>(node);
	//		if(!csn)
	//			return;
	//		overlayNode = dynamic_cast<OverlayNode*>(csn->getChild(0));
	//	}
	//	if(!overlayNode)
	//		return;
	//	osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform*>(overlayNode->getOverlaySubgraph());
	//	if (!mt.valid())
	//		return;

	//	CGlbCalculateBoundBoxVisitor bboxV;
	//	mt->accept(bboxV);
	//	osg::BoundingBoxd bb =bboxV.getBoundBox();
	//}

	osg::ref_ptr<osg::Switch> swiNode = new osg::Switch;
	if(node == NULL)
	{
		mpt_loadState = false;
		//Ϊ�˽�����ӵ��ʱ����Կ���polygon�����ʵʱ����
		mpt_node = swiNode;
		return;
	}

	if(mpr_needReReadData)
	{
		/*
		*   �ڼ��mpr_needReReadDataʱ,�����߳�����mpr_needReReadData=true��ûִ�У�����.
		*/
		mpr_needReReadData = false;
		return ReadData(level,true);
	}

	swiNode->addChild(node);
	glbInt32 objSize = this->ComputeNodeSize(swiNode);
	glbBool isOK = CGlbGlobeMemCtrl::GetInstance()->IncrementUsedMem(objSize);
	if(isOK || isDirect)
	{
		swiNode->getBound();
		mpt_preNode = mpt_node;
		mpt_node = swiNode;
		mpt_currLevel = level;
		mpr_objSize = objSize;
	}
	else
		swiNode = NULL;

	mpt_loadState = false;
}

glbInt32 CGlbGlobeVideo::ComputeNodeSize( osg::Node *node )
{
	CGlbCalculateNodeSizeVisitor cnsv;
	node->accept(cnsv);
	return cnsv.getTextureSize() + cnsv.getNodeMemSize();
}


void CGlbGlobeVideo::ComputePosByAltitudeAndGloleType(osg::Vec3d &position)
{
	if (mpt_globe->GetType() == GLB_GLOBETYPE_GLOBE)
	{
		if (mpt_altitudeMode == GLB_ALTITUDEMODE_ABSOLUTE)
		{
			//��xOrLon��yOrLat��zOrAlt��ֵת�����������긳ֵ��position��x��y��z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),osg::DegreesToRadians(position.x()),position.z(),position.x(),position.y(),position.z());
		}
		else if (mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
		{
			double xOrLon = position.x(),yOrLat = position.y();
			position.z() = position.z() + mpt_globe->GetElevationAt(xOrLon,yOrLat);
			//��xOrLon��yOrLat��zOrAlt��ֵת�����������긳ֵ��position��x��y��z
			g_ellipsoidModel->convertLatLongHeightToXYZ(osg::DegreesToRadians(position.y()),osg::DegreesToRadians(position.x()),position.z(),position.x(),position.y(),position.z());			
		}
	}
	else if (mpt_globe->GetType() == GLB_GLOBETYPE_FLAT)
	{
		if(mpt_altitudeMode == GLB_ALTITUDEMODE_RELATIVETERRAIN)
			position.z() = position.z() + mpt_globe->GetElevationAt(position.x(),position.y());
	}
}

//????��û���޸ģ���symbol�޸����ϣ���������
void CGlbGlobeVideo::DealModePosByChangeAltitudeOrChangePos()
{
	if(mpt_node == NULL)return;
	unsigned int numChild = mpt_node->asSwitch()->getNumChildren();
	if(numChild < 1)
	{
		mpt_globe->UpdateObject(this);
		return;
	}

	if(mpt_feature != NULL)
	{
		IGlbGeometry *geo = NULL;
		bool result = false;
		result = mpt_feature->GetGeometry(&geo,mpt_currLevel);
		if(!result || geo == NULL)
			return;

		GlbGeometryTypeEnum geoType = geo->GetType();
		switch(geoType)
		{
		case GLB_GEO_POLYGON:
			{
				CGlbExtent *pGeoExt = const_cast<CGlbExtent *>(geo->GetExtent());
				if (pGeoExt==NULL) return;
				glbDouble xOrLon,yOrLat,zOrAlt;
				pGeoExt->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
				osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
				ComputePosByAltitudeAndGloleType(position);

				osg::Node *node = mpt_node->asSwitch()->getChild(0);
				osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(node);
				osg::Matrix &m = const_cast<osg::Matrix &>(mt->getMatrix());
				m.setTrans(position);
				mt->setMatrix(m);
			}					
			break;
		}
	}
	else if (mpr_polygonGeo!=NULL)
	{
		CGlbExtent *pGeoExt = const_cast<CGlbExtent *>(mpr_polygonGeo->GetExtent());
		if (pGeoExt==NULL)
			return;
		glbDouble xOrLon,yOrLat,zOrAlt;
		pGeoExt->GetCenter(&xOrLon,&yOrLat,&zOrAlt);
		osg::Vec3d position(xOrLon,yOrLat,zOrAlt);
		ComputePosByAltitudeAndGloleType(position);

		osg::Node *node = mpt_node->asSwitch()->getChild(0);
		osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform *>(node);
		osg::Matrix &m = const_cast<osg::Matrix &>(mt->getMatrix());
		m.setTrans(position);
		mt->setMatrix(m);
	}

	//���¶����ڳ�������λ��.
	mpt_globe->UpdateObject(this);
}

glbBool CGlbGlobeVideo::GetFeaturePoint( glbInt32 idx,glbDouble *ptx,glbDouble *pty,glbDouble *ptz,glbDouble *ptm )
{
	IGlbGeometry *geo = NULL;
	bool result = false;
	result = mpt_feature->GetGeometry(&geo,mpt_currLevel);
	if(!result || geo == NULL)  return false;

	GlbGeometryTypeEnum geoType = geo->GetType();
	switch(geoType)
	{
	case GLB_GEO_POINT:				///<��ά��
		{						
			CGlbPoint* pt = dynamic_cast<CGlbPoint *>(geo);
			pt->GetXY(ptx,pty);								
		}
		break;
	case GLB_GEO_MULTIPOINT:		///<��ά���
		{
			CGlbMultiPoint* multipt = dynamic_cast<CGlbMultiPoint*>(geo);						
			multipt->GetPoint(idx,ptx,pty);						
		}
		break;		
	}				
	return true;
}

void CGlbGlobeVideo::DirtyOnTerrainObject()
{
	if (!mpr_outline || !mpt_globe)return;
	CGlbExtent* cglbExtent = const_cast<CGlbExtent *>(mpr_outline->GetExtent());
	mpt_globe->AddDomDirtyExtent(*cglbExtent);	
}

void CGlbGlobeVideo::DealSelected()
{
	osg::ref_ptr<osg::StateSet> stateset = NULL;
	GlbVideoSymbolInfo* videoInfo = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
	if (!videoInfo)
		return;

	if(videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN)
	{
		osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform*>(
			mpt_node->asGroup()->getChild(0)->asGroup()->getChild(0));
		if(mt.valid())
			stateset = mt->asGroup()->getChild(0)->asGeode()->getDrawable(0)->getOrCreateStateSet();
	}
	else
		stateset = mpt_node->asGroup()->getChild(0)->asGroup()->getChild(0)->asGroup()->getChild(0)->asGeode()->getDrawable(0)->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	if(material == NULL)
		material = new osg::Material;
	glbInt32 opacity = 100;
	{
		GlbVideoSymbolInfo *videoInfo = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
		if(videoInfo && videoInfo->opacity)
			opacity = videoInfo->opacity->GetValue(NULL);
	}
	if(mpt_isSelected)
	{
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,opacity / 100.0));
		material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,0,0,opacity / 100.0));
		stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		//stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		if(material)
		{
			material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,opacity / 100.0));
			material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,opacity / 100.0));
			stateset->setAttribute(material,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
	}
}

void CGlbGlobeVideo::DealBlink()
{
	if(mpt_isBlink)
	{
		ref_ptr<VideoBlinkCallback> vbCallback = new VideoBlinkCallback(this);
		//mpt_node->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		mpt_node->addUpdateCallback(vbCallback);
	}
	else
	{
		osg::NodeCallback *callback = mpt_node->getUpdateCallback();
		if (callback)
		{
			mpt_node->removeUpdateCallback(callback);
			//ȡ����˸֮�󣬴���������ʾ
			DealSelected();
		}
	}
}

glbBool GlbGlobe::CGlbGlobeVideo::SetEdit( glbBool isEdit )
{
	GlbVideoSymbolInfo* videoInfo = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
	if (!videoInfo || videoInfo->symType == GLB_VIDEOSYMBOL_TERRAIN)
		return false;

	if(mpt_isEdit == isEdit) return true;
	mpt_isEdit = isEdit;
	if(mpt_isEdit == true && mpr_editNode == NULL)
	{
		mpr_editNode = new osg::Group;
		osg::ref_ptr<VideoEditEventHandler> editEventHandler = new VideoEditEventHandler(this);
		mpr_editNode->addEventCallback(editEventHandler);
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = 
			new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_parent,mpr_editNode);
		task->mpr_size = ComputeNodeSize(mpr_editNode);
		mpt_globe->mpr_p_callback->AddHangTask(task.get());
	}
	else if(mpt_isEdit == false && mpr_editNode != NULL)
	{
		glbref_ptr<CGlbGlobeCallBack::CGlbGlobeCallBackTask> task = 
			new CGlbGlobeCallBack::CGlbGlobeCallBackTask(mpt_parent,mpr_editNode);
		task->mpr_size = ComputeNodeSize(mpr_editNode);
		mpt_globe->mpr_p_callback->AddRemoveTask(task.get());
		mpr_editNode = NULL;
	}
	return true;
}

glbInt32 GlbGlobe::CGlbGlobeVideo::GetOsgNodeSize()
{
	if(mpt_node == NULL) return 0;
	if(mpr_objSize == 0)
		mpr_objSize = ComputeNodeSize(mpt_node);
	return mpr_objSize;
}

CGlbPolygon * GlbGlobe::CGlbGlobeVideo::RemoveRepeatePoints( CGlbPolygon *polygon )
{
	if(polygon == NULL)
		return NULL;

	CGlbLine *pExtRing = const_cast<CGlbLine *>(polygon->GetExtRing());
	if(pExtRing == NULL || pExtRing->IsEmpty())
		return NULL;

	glbInt32 dimension = polygon->GetCoordDimension();
	glbDouble height = 0.0;
	osg::ref_ptr<osg::Vec3dArray> points = new osg::Vec3dArray;
	const glbDouble *pCoords = pExtRing->GetPoints();
	glbInt32 pntCnt = pExtRing->GetCount();
	for(glbInt32 i = 0; i < pntCnt;++i)
	{
		if(dimension == 3)
			height = pCoords[i * dimension + 2];
		osg::Vec3d pnt(pCoords[i * dimension],pCoords[i * dimension + 1],height);
		points->push_back(pnt);
	}

	glbInt32 pointCnt = points->size();
	osg::Vec3dArray::iterator p = points->begin();
	osg::Vec3d v = *p;
	//osg::Vec3dArray *tempPoints = new osg::Vec3dArray;
	//tempPoints->push_back(v);
	CGlbPolygon *newPolygon = new CGlbPolygon(3);
	CGlbLine *extLine = new CGlbLine(3);
	newPolygon->SetExtRing(extLine);
	extLine->AddPoint(v.x(),v.y(),v.z());
	for(; p != points->end();p++)
	{
		if(v[0] == (*p)[0] && v[1] == (*p)[1])
			continue;

		v = *p;
		//tempPoints->push_back(v);
		extLine->AddPoint(v.x(),v.y(),v.z());
	}

	osg::Vec3d beginPoint = points->at(0);
	osg::Vec3d endPoint = points->at(points->size() - 1);
	if(extLine->GetCount() > 1 && isDoubleEqual(beginPoint.x(),endPoint.x()) && isDoubleEqual(beginPoint.y(),endPoint.y()) && isDoubleEqual(beginPoint.z(),endPoint.z()))
		extLine->DeletePoint(extLine->GetCount() - 1);//ɾ�����һ���㣬��Ϊ���һ�������
	return newPolygon;
}

glbBool GlbGlobe::CGlbGlobeVideo::DealBillboardSymbolChange( GlbVideoSymbolInfo *videoInfo,glbBool &isNeedDirectDraw )
{
	glbref_ptr<GlbVideoSymbolInfo> oldvideoInfo = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
	if (!oldvideoInfo)
		return false;
	osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform*>(mpt_node->asSwitch()->getChild(0));
	if (!mt.valid())
		return false;
	osg::ref_ptr<GlbGlobeAutoTransform> autoTransform = dynamic_cast<GlbGlobeAutoTransform *>(mt->getChild(0));
	if (!autoTransform.valid())
		return false;
	osg::ref_ptr<osg::Geode> geoVideo = dynamic_cast<osg::Geode*>(autoTransform->getChild(0));
	if(!geoVideo.valid())
		return false;
	osg::ref_ptr<osg::Geometry> geomVideo = dynamic_cast<osg::Geometry*>(geoVideo->getDrawable(0));
	if(!geomVideo.valid())
		return false;
	osg::ref_ptr<osg::StateSet> ss = geomVideo->getStateSet();
	if(!ss.valid())
		return false;

	static osg::Quat quat;
	if(oldvideoInfo->symType != videoInfo->symType)
	{
		if(videoInfo->symType == GLB_VIDEOSYMBOL_FREE)
		{
			autoTransform->setAutoRotateMode(osg::AutoTransform::NO_ROTATION);
			autoTransform->setRotation(quat);
		}
		else if(videoInfo->symType == GLB_VIDEOSYMBOL_BILLBOARD)
		{
			quat = autoTransform->getRotation();
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		}
	}

	//��Ƶ�ļ�
	if (!CGlbCompareValue::CompareValueStringEqual(oldvideoInfo->videofile,videoInfo->videofile,mpt_feature.get()))
	{
		if(mpr_videoPlayer.valid())
			mpr_videoPlayer->quit(true);
		CGlbWString videofile = videoInfo->videofile->GetValue(mpt_feature.get());
		std::string str = videofile.ToString();
		const char* filepath = str.c_str();
		osg::ref_ptr<VideoPlayer> newVideoPlayer = new VideoPlayer;
		if(newVideoPlayer.valid())
		{
			newVideoPlayer->open(filepath);
			mpr_videoPlayer = newVideoPlayer;
			newVideoPlayer->play();
		}

		if(newVideoPlayer.valid())
		{
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(newVideoPlayer.get());
			texture->setResizeNonPowerOfTwoHint(false);
			texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
			texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
			texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
			ss->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON);
			if (newVideoPlayer->isImageTranslucent())
			{
				osg::notify(osg::NOTICE)<<"Transparent movie, enabling blending."<<std::endl;
				ss->setMode(GL_BLEND, osg::StateAttribute::ON);
				ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			}
		}
		else
		{
			std::cout<<"Unable to read file "<<filepath<<std::endl;
		}
	}

	if (!CGlbCompareValue::CompareValueBoolEqual(oldvideoInfo->bPause,videoInfo->bPause,mpt_feature.get()))
	{
		if(videoInfo->bPause->GetValue(mpt_feature.get()))
			mpr_videoPlayer->pause();
		else
			mpr_videoPlayer->play();
	}

	if (!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->volume,videoInfo->volume,mpt_feature.get()))
	{
		mpr_videoPlayer->setVolume(videoInfo->volume->GetValue(mpt_feature.get()));
	}

	if (!CGlbCompareValue::CompareValueBoolEqual(oldvideoInfo->bvoice,videoInfo->bvoice,mpt_feature.get()))
	{
		if(videoInfo->bvoice->GetValue(mpt_feature.get()))
			mpr_videoPlayer->setVolume(videoInfo->volume->GetValue(mpt_feature.get()));
		else
			mpr_videoPlayer->setVolume(0);
	}

	if(!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->referenceTime,videoInfo->referenceTime,mpt_feature.get()))
	{
		double videoLength = mpr_videoPlayer->getLength();
		double percent = videoInfo->referenceTime->GetValue(NULL);
		double referenceTime = videoLength * percent / 100;
		mpr_videoPlayer->setReferenceTime(referenceTime);
	}

	if (!CGlbCompareValue::CompareValueBoolEqual(oldvideoInfo->bLoop,videoInfo->bLoop,mpt_feature.get()))
	{
		if(videoInfo->bLoop->GetValue(mpt_feature.get()))
			mpr_videoPlayer->setLoopingMode(osg::ImageStream::LOOPING);
		else
			mpr_videoPlayer->setLoopingMode(osg::ImageStream::NO_LOOPING);
	}

	//�ط�
	if(mpr_videoPlayer->getStatus() == osg::ImageStream::PLAYING && videoInfo->bRewind->GetValue(mpt_feature.get()))
	{
		mpr_videoPlayer->rewind();
		mpr_videoPlayer->play();
	}

	//��͸����
	if (!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->opacity,videoInfo->opacity,mpt_feature.get()))
	{
		if (videoInfo->opacity->GetValue(mpt_feature.get()) < 100)
		{
			ss->setMode(GL_BLEND,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
			if (ss->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
				ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN|osg::StateAttribute::OVERRIDE);
		}else{
			ss->setMode(GL_BLEND,osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
			ss->setRenderingHint(osg::StateSet::OPAQUE_BIN|osg::StateAttribute::OVERRIDE);
		}
		glbDouble opacity = videoInfo->opacity->GetValue(mpt_feature.get());
		osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(ss->getAttribute(osg::StateAttribute::MATERIAL)); 
		if(!material.valid())
			return false;
		material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0,1.0,1.0,(glbFloat)opacity/100.0));
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)opacity/100.0));

		//����Ǽ�ѡ״̬
		if(mpt_isSelected)
			DealSelected();
	}

	glbBool isUpdateExtent = false;

	if(!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->xScale,videoInfo->xScale,mpt_feature.get()))
	{
		osg::Matrixd polygonMT = mt->getMatrix();
		osg::Matrix mScale;

		if(oldvideoInfo->xScale && videoInfo->xScale)
		{
			mScale.makeScale(videoInfo->xScale->GetValue(NULL) / oldvideoInfo->xScale->GetValue(NULL),1.0,1.0);
			mt->setMatrix(mScale * polygonMT);
		}

		isUpdateExtent = true;
	}

	if(!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->yScale,videoInfo->yScale,mpt_feature.get()))
	{
		osg::Matrixd polygonMT = mt->getMatrix();
		osg::Matrix mScale;

		if(oldvideoInfo->yScale && videoInfo->yScale)
		{
			mScale.makeScale(1.0,videoInfo->yScale->GetValue(NULL) / oldvideoInfo->yScale->GetValue(NULL),1.0);
			mt->setMatrix(mScale * polygonMT);
		}

		isUpdateExtent = true;
	}

	if(!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->xOffset,videoInfo->xOffset,mpt_feature.get())
		|| !CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->yOffset,videoInfo->yOffset,mpt_feature.get())
		|| !CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->zOffset,videoInfo->zOffset,mpt_feature.get()))
	{
		osg::Matrixd polygonMT = mt->getMatrix();
		osg::Matrix mTranslate;
		if(videoInfo->xOffset && videoInfo->yOffset && videoInfo->zOffset)
			mTranslate.makeTranslate(
			videoInfo->xOffset->GetValue(NULL) - oldvideoInfo->xOffset->GetValue(NULL),
			videoInfo->yOffset->GetValue(NULL) - oldvideoInfo->yOffset->GetValue(NULL),
			videoInfo->zOffset->GetValue(NULL) - oldvideoInfo->zOffset->GetValue(NULL));

		mt->setMatrix(mTranslate * polygonMT);
		isUpdateExtent = true;
	}

	if(isUpdateExtent)
		mpt_globe->UpdateObject(this);

	mpr_renderInfo = videoInfo;
	mpt_loadState = false;
	return true;
}

glbBool GlbGlobe::CGlbGlobeVideo::DealTerrainSymbolChange( GlbVideoSymbolInfo *videoInfo,glbBool &isNeedDirectDraw )
{
	glbref_ptr<GlbVideoSymbolInfo> oldvideoInfo = dynamic_cast<GlbVideoSymbolInfo*>(mpr_renderInfo.get());
	if (!oldvideoInfo)
		return false;

	osg::ref_ptr<Camera> overlyCamera = dynamic_cast<Camera*>(mpt_node->asSwitch()->getChild(0));
	if(!overlyCamera.valid())
		return false;
	osg::ref_ptr<osg::MatrixTransform> mt = dynamic_cast<osg::MatrixTransform*>(overlyCamera->getChild(0));
	if (!mt.valid())
		return false;
	osg::ref_ptr<osg::Geode> geoVideo = dynamic_cast<osg::Geode*>(mt->getChild(0));
	if(!geoVideo.valid())
		return false;
	osg::ref_ptr<osg::Geometry> geomVideo = dynamic_cast<osg::Geometry*>(geoVideo->getDrawable(0));
	if(!geomVideo.valid())
		return false;
	osg::ref_ptr<osg::StateSet> ss = geomVideo->getStateSet();
	if(!ss.valid())
		return false;

	//��Ƶ�ļ�
	if (!CGlbCompareValue::CompareValueStringEqual(oldvideoInfo->videofile,videoInfo->videofile,mpt_feature.get()))
	{
		if(mpr_videoPlayer.valid())
			mpr_videoPlayer->quit(true);
		CGlbWString videofile = videoInfo->videofile->GetValue(mpt_feature.get());
		std::string str = videofile.ToString();
		const char* filepath = str.c_str();
		osg::ref_ptr<VideoPlayer> newVideoPlayer = new VideoPlayer;
		if(newVideoPlayer.valid())
		{
			newVideoPlayer->open(filepath);
			mpr_videoPlayer = newVideoPlayer;
			newVideoPlayer->play();
		}

		if(newVideoPlayer.valid())
		{
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(newVideoPlayer.get());
			texture->setResizeNonPowerOfTwoHint(false);
			texture->setFilter(osg::Texture::MIN_FILTER,osg::Texture::LINEAR);
			texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
			texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
			ss->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON);
			if (newVideoPlayer->isImageTranslucent())
			{
				osg::notify(osg::NOTICE)<<"Transparent movie, enabling blending."<<std::endl;
				ss->setMode(GL_BLEND, osg::StateAttribute::ON);
				ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			}
		}
		else
		{
			std::cout<<"Unable to read file "<<filepath<<std::endl;
		}
	}

	if (!CGlbCompareValue::CompareValueBoolEqual(oldvideoInfo->bPause,videoInfo->bPause,mpt_feature.get()))
	{
		if(videoInfo->bPause->GetValue(mpt_feature.get()))
			mpr_videoPlayer->pause();
		else
			mpr_videoPlayer->play();
	}

	if (!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->volume,videoInfo->volume,mpt_feature.get()))
	{
		mpr_videoPlayer->setVolume(videoInfo->volume->GetValue(mpt_feature.get()));
	}

	if (!CGlbCompareValue::CompareValueBoolEqual(oldvideoInfo->bvoice,videoInfo->bvoice,mpt_feature.get()))
	{
		if(videoInfo->bvoice->GetValue(mpt_feature.get()))
			mpr_videoPlayer->setVolume(videoInfo->volume->GetValue(mpt_feature.get()));
		else
			mpr_videoPlayer->setVolume(0);
	}

	if(!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->referenceTime,videoInfo->referenceTime,mpt_feature.get()))
	{
		double videoLength = mpr_videoPlayer->getLength();
		double percent = videoInfo->referenceTime->GetValue(NULL);
		double referenceTime = videoLength * percent / 100;
		mpr_videoPlayer->setReferenceTime(referenceTime);
	}

	if (!CGlbCompareValue::CompareValueBoolEqual(oldvideoInfo->bLoop,videoInfo->bLoop,mpt_feature.get()))
	{
		if(videoInfo->bLoop->GetValue(mpt_feature.get()))
			mpr_videoPlayer->setLoopingMode(osg::ImageStream::LOOPING);
		else
			mpr_videoPlayer->setLoopingMode(osg::ImageStream::NO_LOOPING);
	}

	//�ط�
	if(mpr_videoPlayer->getStatus() == osg::ImageStream::PLAYING && videoInfo->bRewind->GetValue(mpt_feature.get()))
	{
		mpr_videoPlayer->rewind();
		mpr_videoPlayer->play();
	}

	//��͸����
	if (!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->opacity,videoInfo->opacity,mpt_feature.get()))
	{
		if (videoInfo->opacity->GetValue(mpt_feature.get()) < 100)
		{
			ss->setMode(GL_BLEND,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
			if (ss->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
				ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN|osg::StateAttribute::OVERRIDE);
		}else{
			ss->setMode(GL_BLEND,osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);
			ss->setRenderingHint(osg::StateSet::OPAQUE_BIN|osg::StateAttribute::OVERRIDE);
		}
		glbDouble opacity = videoInfo->opacity->GetValue(mpt_feature.get());
		osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(ss->getAttribute(osg::StateAttribute::MATERIAL)); 
		if(!material.valid())
			return false;
		material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0,1.0,1.0,(glbFloat)opacity/100.0));
		material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0.0,0.0,0.0,(glbFloat)opacity/100.0));

		//����Ǽ�ѡ״̬
		if(mpt_isSelected)
			DealSelected();
	}

	glbBool isUpdateExtent = false;

	if(!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->xScale,videoInfo->xScale,mpt_feature.get()))
	{
		osg::Matrixd polygonMT = mt->getMatrix();
		osg::Matrix mScale;

		if(oldvideoInfo->xScale && videoInfo->xScale)
		{
			mScale.makeScale(videoInfo->xScale->GetValue(NULL) / oldvideoInfo->xScale->GetValue(NULL),1.0,1.0);
			mt->setMatrix(mScale * polygonMT);
		}

		isUpdateExtent = true;
	}

	if(!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->yScale,videoInfo->yScale,mpt_feature.get()))
	{
		osg::Matrixd polygonMT = mt->getMatrix();
		osg::Matrix mScale;

		if(oldvideoInfo->yScale && videoInfo->yScale)
		{
			mScale.makeScale(1.0,videoInfo->yScale->GetValue(NULL) / oldvideoInfo->yScale->GetValue(NULL),1.0);
			mt->setMatrix(mScale * polygonMT);
		}

		isUpdateExtent = true;
	}

	if(!CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->xOffset,videoInfo->xOffset,mpt_feature.get())
		|| !CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->yOffset,videoInfo->yOffset,mpt_feature.get())
		|| !CGlbCompareValue::CompareValueDoubleEqual(oldvideoInfo->zOffset,videoInfo->zOffset,mpt_feature.get()))
	{
		osg::Matrixd polygonMT = mt->getMatrix();
		osg::Matrix mTranslate;
		if(videoInfo->xOffset && videoInfo->yOffset && videoInfo->zOffset)
			mTranslate.makeTranslate(
			videoInfo->xOffset->GetValue(NULL) - oldvideoInfo->xOffset->GetValue(NULL),
			videoInfo->yOffset->GetValue(NULL) - oldvideoInfo->yOffset->GetValue(NULL),
			videoInfo->zOffset->GetValue(NULL) - oldvideoInfo->zOffset->GetValue(NULL));

		mt->setMatrix(mTranslate * polygonMT);
		isUpdateExtent = true;
	}

	if(isUpdateExtent)
		mpt_globe->UpdateObject(this);

	mpr_renderInfo = videoInfo;
	mpt_loadState = false;
	return true;
}

glbBool GlbGlobe::CGlbGlobeVideo::Load2( xmlNodePtr *node,const glbWChar* prjPath )
{
	xmlChar *szKey = NULL;
	xmlNodePtr pnode = *node;
	CGlbGlobeRObject::Load2(node,prjPath);
	xmlNodePtr groupNode = pnode->parent;
	xmlNodePtr childNode;
	CGlbString bUse;

	if (groupNode && (!xmlStrcmp(groupNode->name, (const xmlChar *)"GetRenderInfo")))
	{
		GlbGlobeObjectTypeEnum rdtype;
		xmlNodePtr rdchild = groupNode->xmlChildrenNode;

		GlbVideoSymbolInfo* videoInfo = new GlbVideoSymbolInfo();
		videoInfo->Load2(rdchild,prjPath);
		mpr_renderInfo = (GlbRenderInfo*)videoInfo;
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetRenderOrder
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&mpt_renderOrder);
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetRObject
	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetTooltip

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsBlink
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isBlink = false;
		else
			mpt_isBlink = true;
		xmlFree(szKey);
	}


	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsEdit
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isEdit = false;
		else
			mpt_isEdit = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsGround
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isGround = false;
		else
			mpt_isGround = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsSelected
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isSelected = false;
		else
			mpt_isSelected = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsShow
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isShow = false;
		else
			mpt_isShow = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//IsUseInstance
	if (groupNode->xmlChildrenNode && (!xmlStrcmp(groupNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(groupNode->xmlChildrenNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			mpt_isUseInstance = false;
		else
			mpt_isUseInstance = true;
		xmlFree(szKey);
	}

	groupNode = groupNode->next;//text
	groupNode = groupNode->next;//GetGeo
	childNode = groupNode->xmlChildrenNode;//text
	childNode = childNode->next;//coordDimension
	glbInt32 coordDimension = -1;
	if (childNode->xmlChildrenNode && (!xmlStrcmp(childNode->xmlChildrenNode->name, (const xmlChar *)"text")))
	{
		szKey = xmlNodeGetContent(childNode->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&coordDimension);
		xmlFree(szKey);
	}
	childNode = childNode->next;//text
	childNode = childNode->next;//hasM
	glbBool ishasM = false;
	if (childNode)
	{
		szKey = xmlNodeGetContent(childNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			ishasM = false;
		else
			ishasM = true;
		xmlFree(szKey);
	}
	childNode = childNode->next;//text
	childNode = childNode->next;//ExtRing
	childNode = childNode->xmlChildrenNode;//text
	childNode = childNode->next;//coordDimension
	if (childNode && (!xmlStrcmp(childNode->name, (const xmlChar *)"coordDimension")))
	{
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%d",&coordDimension);
		xmlFree(szKey);
	}
	childNode = childNode->next;//text
	childNode = childNode->next;//hasM
	ishasM = false;
	if (childNode)
	{
		szKey = xmlNodeGetContent(childNode);	
		char* str = u2g_obj((char*)szKey);
		bUse = (char*)str;
		if(bUse == "false")
			ishasM = false;
		else
			ishasM = true;
		xmlFree(szKey);
	}

	childNode = childNode->next;//text
	childNode = childNode->next;//points
	childNode = childNode->xmlChildrenNode;//text
	childNode = childNode->next;//tagPoint3
	mpr_polygonGeo = new CGlbPolygon(3,false);
	CGlbLine *extRing = new CGlbLine(3,false);
	glbDouble tempX,tempY,tempZ;
	while(!xmlStrcmp(childNode->name, (const xmlChar *)"tagPoint3"))
	{
		childNode = childNode->xmlChildrenNode;//text
		childNode = childNode->next;//x
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%lf",&tempX);

		childNode = childNode->next;//text
		childNode = childNode->next;//y
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%lf",&tempY);

		childNode = childNode->next;//text
		childNode = childNode->next;//z
		szKey = xmlNodeGetContent(childNode);	
		sscanf_s((char*)szKey,"%lf",&tempZ);
		xmlFree(szKey);
		extRing->AddPoint(tempX,tempY,tempZ);
		childNode = childNode->parent;
		childNode = childNode->next;//text
		childNode = childNode->next;
		if(!childNode)
			break;
	}
	mpr_polygonGeo->SetExtRing(extRing);
	return true;
}