#include "StdAfx.h"
#include "GlbGlobeMarkerSmokeSymbol.h"
#include "GlbPoint.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"
#include "GlbGlobeObjectRenderInfos.h"
#include <osgParticle/ExplosionEffect>

using namespace GlbGlobe;

CGlbGlobeMarkerSmokeSymbol::CGlbGlobeMarkerSmokeSymbol(void)
{
}


CGlbGlobeMarkerSmokeSymbol::~CGlbGlobeMarkerSmokeSymbol(void)
{
}


osg::Node *CGlbGlobeMarkerSmokeSymbol::Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo)
{
	GlbRenderInfo *renderInfo = obj->GetRenderInfo();
	GlbMarkerSmokeSymbolInfo *markerSmokeInfo = dynamic_cast<GlbMarkerSmokeSymbolInfo *>(renderInfo);

	if(markerSmokeInfo == NULL)
		return NULL;

	GlbGlobeTypeEnum globeType = obj->GetGlobe()->GetType();

	std::vector<osg::Vec3> positions;
	GlbGeometryTypeEnum geoType = geo->GetType();
	switch(geoType)
	{
	case GLB_GEO_POINT:
		{
			CGlbPoint* pt = dynamic_cast<CGlbPoint*>(geo);
			if(pt)
			{
				osg::Vec3d pos;
				pt->GetXYZ(&pos.x(),&pos.y(),&pos.z());			
				positions.push_back(pos);
			}
		}
		break;
	case GLB_GEO_MULTIPOINT:
		{
			CGlbMultiPoint* mulPt = dynamic_cast<CGlbMultiPoint*>(geo);
			if (mulPt)
			{
				osg::Vec3d pos;
				glbInt32 cnt = mulPt->GetCount();
				for (glbInt32 idx = 0; idx < cnt; idx++)
				{
					mulPt->GetPoint(idx,&pos.x(),&pos.y(),&pos.z());
					positions.push_back(pos);
				}
			}
		}
		break;
	}	

	if (positions.size()<=0) return NULL;

	osg::Group* smokeGroupNode = new osg::Group();

	float scale = 1.0f;
	float intensity = 1.0f;

	CGlbFeature* feature = obj->GetFeature();
	glbInt32 startColor = RGB(25,25,25);
	if (markerSmokeInfo->startColor)
		startColor = markerSmokeInfo->startColor->GetValue(feature);
	glbInt32 endColor = RGB(100,100,100);
	if (markerSmokeInfo->endColor)
		endColor = markerSmokeInfo->endColor->GetValue(feature);
	if (markerSmokeInfo->scale)
		scale = markerSmokeInfo->scale->GetValue(feature);
	if (markerSmokeInfo->intensity)
		intensity = markerSmokeInfo->intensity->GetValue(feature);

	osg::Vec3 _c1,_c2;
	osg::Vec4 cc = GetColor(startColor);
	_c1.set(cc.r(),cc.g(),cc.b());
	cc = GetColor(endColor);
	_c2.set(cc.r(),cc.g(),cc.b());

	// ����
	osg::Vec3 wind(0.0f,0.0f,1.0f);	
	if (globeType == GLB_GLOBETYPE_GLOBE)
	{		
		wind = positions.at(0);
		wind.normalize();
		//wind.set(wind.x(),wind.y(),wind.z()+0.1);
		//wind.normalize();
	}

	for (size_t k = 0; k < positions.size(); k++)
	{
		osg::Vec3 pos = positions.at(k);			

		// ��ըģ�⣬ 10.0fΪ���űȣ�Ĭ��Ϊ1.0f��������
		osg::ref_ptr<osgParticle::ExplosionEffect> explosion = new osgParticle::ExplosionEffect(pos,scale,1);

		// �Զ����������
		osg::ref_ptr<osg::Group> smoke = CreateMySmokeParticle(pos,_c1,_c2,scale,intensity);

		explosion->setWind(wind);

		smokeGroupNode->addChild(explosion.get());
		smokeGroupNode->addChild(smoke.get());

	}
	return smokeGroupNode;	
}

#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/BoxPlacer>

// �����Զ�����������ϵͳ
osg::ref_ptr<osg::Group> CGlbGlobeMarkerSmokeSymbol::CreateMySmokeParticle(osg::Vec3 pos, osg::Vec3 startColor, osg::Vec3 endColor,float scale, float intensity)
{
	osg::ref_ptr<osg::Group> smokeNode = new osg::Group();

	// ��������ϵͳģ��
	osgParticle::Particle ptemplate;
	// ������������
	ptemplate.setLifeTime(1.5*scale);
	// �������Ӵ�С�仯��Χ
	ptemplate.setSizeRange(osgParticle::rangef(0.75f*scale,3.0f*scale)); // 0.75-3.0
	// ��������alpha�仯��Χ
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
	float _rclr = 0.4f;//0.2f + rand()/double(RAND_MAX);//����ɫ����
	// ����������ɫ�仯��Χ
	//ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.1f,0.1f,0.1f,1.0f), osg::Vec4(_rclr,_rclr,_rclr,0.0f)));
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(startColor.x(),startColor.y(),startColor.z(),1.0f), osg::Vec4(endColor.x(),endColor.y(),endColor.z(),0.0f)));
	// ���ð뾶
	ptemplate.setRadius(0.05f); //0.05
	// ��������
	ptemplate.setMass(0.05f); //0.05

	// ��������ϵͳ
	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
	// ���ò��ʣ� �Ƿ�������ӣ��Ƿ���ӹ���
	ps->setDefaultAttributes("res/smoke.rgb",false,false);

	// ����ģ��
	ps->setDefaultParticleTemplate(ptemplate);

	// �������ӷ�����(�������������������ͷ�������
	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
	// ��������ϵͳ
	emitter->setParticleSystem(ps.get());

	// �����������ͼ������� ����ÿһ֡���ӵ�������Ŀ
	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
	// ����ÿ����ӵ����ӵĸ���
	counter->setRateRange(100.00f*intensity,100.0f*intensity);
	// ����������
	emitter->setCounter(counter.get());
	// ����һ���������
	osg::ref_ptr<osgParticle::PointPlacer> placer = new osgParticle::PointPlacer();
	// ����λ��
	placer->setCenter(pos);
	// �����������
	emitter->setPlacer(placer.get());

	// box������
	//osg::ref_ptr<osgParticle::BoxPlacer> placer = new osgParticle::BoxPlacer();
	//placer->setXRange(-100,100);
	//placer->setYRange(-100,100);
	//placer->setZRange(-100,100);
	//emitter->setPlacer(placer.get());

	// �������ȷ�����
	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
	// ���÷����ٶȱ仯��Χ
	shooter->setInitialSpeedRange(0,30*scale); // 100-0
	// ����������
	emitter->setShooter(shooter.get());

	// ���뵽������
	smokeNode->addChild(emitter.get());

	// ������׼��������󣬿������������������еĸ���
	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
	// ��������ϵͳ
	program->setParticleSystem(ps.get());

	// ��������ģ�����
	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
	// �����������ٵģ�Ĭ��Ϊ9.80665f
	ap->setToGravity(-1.0f/scale);
	// ��������
	program->addOperator(ap.get());

	// ������������ģ��
	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
	// ���ÿ�������
	// FluidViscosity Ϊ1.8e-5f,FluidDensity Ϊ1.2929f
	ffo->setFluidToAir();
	// ������������
	program->addOperator(ffo.get());

	// ��ӵ�����
	smokeNode->addChild(program.get());

	// ��Ӹ�������ʵ��ÿ֡���ӵĹ���
	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
	// ��������ϵͳ
	psu->addParticleSystem(ps.get());

	// ���볡��
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(ps.get());

	smokeNode->addChild(geode.get());
	smokeNode->addChild(psu.get());
	return smokeNode.get();
}