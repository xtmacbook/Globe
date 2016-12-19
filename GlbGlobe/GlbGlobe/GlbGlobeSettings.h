#pragma once
#include <vector>
#include <osg/Vec3d>
#include <osg/CoordinateSystemNode>
#include "GlbGlobeExport.h"
/********************************************************************
  * Copyright (c) 2014 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *  
  *
  * �����������������Ϣ,
  *
  * @version 1.0
  * @author  ����
  * @date    2014-5-5 10:40
*********************************************************************/
//�ǿջ���˳��
#ifndef GlB_STARS_RENDERBIN
#define GlB_STARS_RENDERBIN       -11000
#endif
//̫������˳��
#ifndef GLB_SUN_RENDERBIN
//#define GLB_SUN_RENDERBIN         -10900
#define GLB_SUN_RENDERBIN         -10500
#endif
//��������˳��
#ifndef GLB_MOON_RENDERBIN
//#define GLB_MOON_RENDERBIN        -10800
#define GLB_MOON_RENDERBIN        -10400
#endif
//��������˳��
#ifndef GLB_ATMOSPHERE_RENDERBIN
#define GLB_ATMOSPHERE_RENDERBIN  -10700
#endif
//��պл���˳��
#ifndef  GLB_SKYBOX_RENDERBIN
#define  GLB_SKYBOX_RENDERBIN     -10600
#endif
//�ڵ������û��������˳��
#ifndef  GLB_BASEGLOBE_RENDERBIN
#define  GLB_BASEGLOBE_RENDERBIN     -10001
#endif
#ifndef  GLB_BASEGLOBE_RENDERBIN
#define  GLB_BASEGLOBE_RENDERBIN     -10001
#endif

//������Ϣ�仯�����ӿ�
//#define LogoFile_Setting   L"LogoFile"
//#define SkyTexFile_Setting L"SkyTexFile"
//#define DateTime_Setting   L"DateTime"
//#define WindDirection_Setting L"WindDirection"
//#define WindStrong_Setting    L"WindStrong"
//#define Weather_Setting       L"Weather"
#define LogoFile_Setting   1
#define SkyTexFile_Setting 2
#define DateTime_Setting   3
#define WindDirection_Setting 4
#define WindStrong_Setting    5
#define Weather_Setting       6

#include "GlbObServerSet.h"
#include "glbref_ptr.h"

class GLB_DLLCLASS_EXPORT IGlbGlobeSettingListen:public CGlbReference
{
public:
	//virtual void SettingUpdate(const glbWChar* settingName){}
	virtual void SettingUpdate(glbInt32 settingName){}
};
class GLB_DLLCLASS_EXPORT CGlbGlobeSettings : public CGlbReference,
											  CGlbObserver
{
public:
	CGlbGlobeSettings(void);
	~CGlbGlobeSettings(void);
public:
	//logo�ļ�
	CGlbWString const &GetLogoFile()const;
	void SetLogoFile(const glbWChar* logoFile);
	//��������ļ�
	CGlbWString const &GetSkyTexFile()const;
	void SetSkyTexFile(const glbWChar* skytexFile);
	//ʱ��
	CGlbWString const &GetTimeZone()const;
	void SetTimeZone(const glbWChar* timezone);
	//ʱ��
	void SetDateTime(glbInt32 year,glbInt32 month,glbInt32 date,glbDouble houresUTC);
	void GetDateTime(glbInt32 &year,glbInt32 &month,glbInt32 &date,glbDouble &houresUTC)const;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//����
	void SetWindDirection(glbDouble ws);
	glbDouble GetWindDirection()const;
	//����
	void SetWindStrong(glbDouble ws);
	glbDouble GetWindStrong()const;
	//����״��
	void SetWeather(glbDouble w);
	glbDouble GetWeather()const;
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//̫��λ�á��������� ����ϵ��
	const osg::Vec3d &GetSunPostion()const;
	//̫�����������ĵľ���
	glbDouble GetSunDistance()const;
	//����λ�á��������� ����ϵ��
	const osg::Vec3d &GetMoonPostion()const;
	//�������������ľ���
	glbDouble GetMoonDistance()const;
	//������Ȧ�뾶
	glbDouble GetInnerAtmosphereRadius()const;
	//������Ȧ�뾶
	glbDouble GetOuterAtmosphereRadius()const;
	//�ǿհ뾶
	glbDouble GetStarsRadius()const;
	//���ǵ���С�뾶
	glbDouble GetMinStarMagnitude()const;
	//����������
	osg::EllipsoidModel *GetEarthEllipsoidModel()const;	
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void ObjectDeleted(void* listen);
	void RegisterListen(IGlbGlobeSettingListen* listen);
	void UnRegisterListen(IGlbGlobeSettingListen* listen);
private:
	//void notifyListens(const glbWChar* changedSetting);
	void notifyListens(glbInt32 changedSetting);
private:
	CGlbWString mpr_logoFile;     //logon �ļ�
	CGlbWString mpr_skytexFile;   //��������ļ�
	CGlbWString mpr_timeZone;     //ʱ��

	glbInt32 mpr_year,mpr_month,mpr_date;glbDouble mpr_houresUTC;          // UTCʱ��

	glbFloat                                mpr_earthRadius;               // ����뾶
	glbFloat                                mpr_innerRadius;               // ��Ȧ�����뾶
	glbFloat                                mpr_outerRadius;               // ��Ȧ�����뾶
	glbFloat                                mpr_sunDistance;               // ̫������
	glbFloat                mpr_starRadius, mpr_minStarMagnitude;          // �ǿհ뾶��������С�뾶

	osg::Vec3d                              mpr_sunPos;                    // ̫��λ��
	osg::Vec3d                              mpr_moonPos;                   // ����λ��

	glbDouble                               mpr_windstrong;
	glbDouble                               mpr_winddirection;
	glbDouble                               mpr_weather;

	std::vector<glbobserver_ptr<IGlbGlobeSettingListen>> mpr_listens;      //���ü�������
	CRITICAL_SECTION                                     mpr_critical;     //�����������ɾ���ٽ���
};

