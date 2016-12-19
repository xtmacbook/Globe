#pragma once

#include "GlbGlobeObjectFactory.h"

//< ����� ������
class CGlbGlobePointProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobePointProvider();
	~CGlbGlobePointProvider();
	CGlbGlobeObject* Create();
};
//< �߶��� ������
class CGlbGlobeLineProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeLineProvider();
	~CGlbGlobeLineProvider();
	CGlbGlobeObject* Create();
};
//< ����� ������
class CGlbGlobePolygonProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobePolygonProvider();
	~CGlbGlobePolygonProvider();
	CGlbGlobeObject* Create();
};
//< Tin���� ������
class CGlbGlobeTinProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeTinProvider();
	~CGlbGlobeTinProvider();
	CGlbGlobeObject* Create();
};
//< ���߶��� ������
class CGlbGlobeVisualLineAnalysisProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeVisualLineAnalysisProvider();
	~CGlbGlobeVisualLineAnalysisProvider();
	CGlbGlobeObject* Create();
};
//< ������� ������
class CGlbGlobeViewAnalysisProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeViewAnalysisProvider();
	~CGlbGlobeViewAnalysisProvider();
	CGlbGlobeObject* Create();
};
//< Complex���� ������
class CGlbGlobeComplexObjectProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeComplexObjectProvider();
	~CGlbGlobeComplexObjectProvider();
	CGlbGlobeObject* Create();
};
//< Drill���� ������
class CGlbGlobeDrillProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeDrillProvider();
	~CGlbGlobeDrillProvider();
	CGlbGlobeObject* Create();
};
//< DigHole���� ������
class CGlbGlobeDigHoleProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeDigHoleProvider();
	~CGlbGlobeDigHoleProvider();
	CGlbGlobeObject* Create();
};

//< Section���� ������
class CGlbGlobeSectionProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeSectionProvider();
	~CGlbGlobeSectionProvider();
	CGlbGlobeObject* Create();
};

//< Dynamiac���� ������
class CGlbGlobeDynamicProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeDynamicProvider();
	~CGlbGlobeDynamicProvider();
	CGlbGlobeObject* Create();
};

//< ModifyTerrain���� ������
class CGlbGlobeModifyTerrainProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeModifyTerrainProvider();
	~CGlbGlobeModifyTerrainProvider();
	CGlbGlobeObject* Create();
};

//< Video���� ������
class CGlbGlobeVideoProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeVideoProvider();
	~CGlbGlobeVideoProvider();
	CGlbGlobeObject* Create();
};

//< Network���� ������
class CGlbGlobeNetworkProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeNetworkProvider();
	~CGlbGlobeNetworkProvider();
	CGlbGlobeObject* Create();
};

//< CoordinateAxis���� ������
class CGlbGlobeCoordinateAxisProvider : public IGlbGlobeObjectProvider
{
public:
	CGlbGlobeCoordinateAxisProvider();
	~CGlbGlobeCoordinateAxisProvider();
	CGlbGlobeObject* Create();
};

//< ���󴴽�ע����
class CGlbObjectRegisterCreator : public CGlbReference
{
public:
	CGlbObjectRegisterCreator(void);
	~CGlbObjectRegisterCreator(void);
};
