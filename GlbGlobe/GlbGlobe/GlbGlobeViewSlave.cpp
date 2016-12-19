#include "StdAfx.h"
#include "GlbGlobeViewSlave.h"
using namespace GlbGlobe;

CGlbGlobeViewSlave::CGlbGlobeViewSlave(CGlbGlobeView* pview,int x,int y,int width,int height)
{
	osg::Camera *camera = pview->GetOsgCamera();
	this->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	this->setViewport(new osg::Viewport(x,y,width,height));

	double fovy, aspectRatio, zNear, zFar;
	camera->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
	this->setProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

	osg::ref_ptr<CGlbGlobeViewSlaveUpdateCallback> sucb = new CGlbGlobeViewSlaveUpdateCallback(pview);
	this->addUpdateCallback(sucb.get());

	osg::Node *node = camera->getChild(0)->asGroup()->getChild(0);
	this->setName("slaveCamera");
	this->addChild(node);
}

CGlbGlobeViewSlave::~CGlbGlobeViewSlave(void)
{
}

GlbGlobe::CGlbGlobeViewSlaveUpdateCallback::CGlbGlobeViewSlaveUpdateCallback(CGlbGlobeView* pview)
{
	mpr_view = pview;
}

GlbGlobe::CGlbGlobeViewSlaveUpdateCallback::~CGlbGlobeViewSlaveUpdateCallback()
{

}

void GlbGlobe::CGlbGlobeViewSlaveUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
	osg::Vec3d eye,center,up;
	mpr_view->GetOsgCamera()->getViewMatrixAsLookAt(eye,center,up);
	mpr_view->GetFocusPos(center);
	g_ellipsmodel->convertLatLongHeightToXYZ(
		osg::DegreesToRadians(center.y()),osg::DegreesToRadians(center.x()),center.z(),center.x(),center.y(),center.z());
	mpr_view->GetGlobe()->SetUnderGroundMode(true);
	mpr_view->GetGlobe()->mpr_ugplane->SetShowFilter(true);

	osg::Matrixd masterMatrix = mpr_view->GetOsgCamera()->getViewMatrix();

	osg::Camera *slaveCamera = dynamic_cast<osg::Camera *>(node);
	osg::Vec3d transDirection = eye;
	transDirection.normalize();

	double lat,lon,alt;
	osg::Vec3d terrainPos;
	g_ellipsmodel->convertXYZToLatLongHeight(eye.x(),eye.y(),eye.z(),lat,lon,alt);
	g_ellipsmodel->convertLatLongHeightToXYZ(lat,lon,0.0,terrainPos.x(),terrainPos.y(),terrainPos.z());
	double length = eye.length() - terrainPos.length();
	double pitch = -mpr_view->GetPitch();
	if(length > 0)
	{
		slaveCamera->setViewMatrix(osg::Matrixd::translate(transDirection * length * 2) * masterMatrix);
		if(pitch > 1.0)
		{
			osg::Vec3d tempCenter;
			masterMatrix.getLookAt(eye,tempCenter,up);
			osg::Vec3d viewDir = eye - center;
			viewDir.normalize();
			osg::Vec3d east = viewDir ^ up;
			east.normalize();
			osg::Vec3d slaveEye,slaveCenter,slaveUp;
			slaveCamera->getViewMatrix().getLookAt(slaveEye,slaveCenter,slaveUp);
			osg::Vec3d slaveDir = slaveEye - center;
			slaveDir.normalize();
			slaveUp =  east ^ slaveDir;
			slaveUp.normalize();
			slaveCamera->setViewMatrixAsLookAt(slaveEye,center, slaveUp);
		}
	}
	else
	{
		slaveCamera->setViewMatrix(osg::Matrixd::translate(transDirection * length * 2) * masterMatrix);
		if(pitch < -1.0)
		{
			osg::Vec3d tempCenter;
			masterMatrix.getLookAt(eye,tempCenter,up);
			osg::Vec3d viewDir = eye - center;
			viewDir.normalize();
			osg::Vec3d east = viewDir ^ up;
			east.normalize();
			osg::Vec3d slaveEye,slaveCenter,slaveUp;
			slaveCamera->getViewMatrix().getLookAt(slaveEye,slaveCenter,slaveUp);
			osg::Vec3d slaveDir = slaveEye - center;
			slaveDir.normalize();
			slaveUp =  east ^ slaveDir;
			slaveUp.normalize();
			slaveCamera->setViewMatrixAsLookAt(slaveEye,center, slaveUp);
		}
	}
}