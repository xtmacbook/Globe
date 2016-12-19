/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobePolygon.h
* @brief   ����ζ���ͷ�ļ����ĵ�����CGlbGlobePolygon��
* @version 1.0
* @author  ��ͥ��
* @date    2014-6-18 15:30
*********************************************************************/
#pragma once

#include "GlbGlobeObjectExport.h"

#include "GlbGlobeREObject.h"
#include "IGlbGlobeTask.h"
#include "GlbGlobeCallBack.h"

#include "GlbPolygon.h"

namespace GlbGlobe
{
/**
* @brief CGlbGlobePolygon����ζ�����
* @version 1.0
* @author  ��ͥ��
* @date    2014-6-18 15:30
*/
class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobePolygon : public CGlbGlobeREObject
{
	class CGlbGlobePolygonLoadDataTask : public IGlbGlobeTask
	{
	public:
		CGlbGlobePolygonLoadDataTask(CGlbGlobePolygon *obj,glbInt32 level);
		~CGlbGlobePolygonLoadDataTask();
		glbBool doRequest();
		CGlbGlobeObject *getObject();
	private:
		glbref_ptr<CGlbGlobePolygon>    mpr_obj;
		glbInt32                        mpr_level;
	};
public:
	CGlbGlobePolygon(void);
	~CGlbGlobePolygon(void);

	virtual GlbGlobeObjectTypeEnum GetType();
	glbBool Load( xmlNodePtr *node,const glbWChar* prjPath );

	glbBool Load2( xmlNodePtr *node,const glbWChar* prjPath );
	glbBool Save( xmlNodePtr node,const glbWChar* prjPath );

	virtual glbBool SetAltitudeMode( GlbAltitudeModeEnum mode ,glbBool &isNeedDirectDraw);

	virtual void SetRenderOrder( glbInt32 order );	
		
	virtual glbDouble GetDistance( osg::Vec3d &cameraPos,glbBool isCompute );

	virtual void LoadData( glbInt32 level );

	virtual void AddToScene();

	virtual glbInt32 RemoveFromScene( glbBool isClean );

	glbref_ptr<CGlbExtent> GetBound( glbBool isWorld = true );

	virtual void UpdateElevate();

	virtual IGlbGeometry * GetOutLine();

	virtual glbBool SetRenderInfo( GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw );

	virtual GlbRenderInfo * GetRenderInfo();
		
	virtual glbBool SetShow(glbBool isShow,glbBool isOnState=false);

	virtual glbBool SetSelected( glbBool isSelected );

	virtual glbBool SetBlink( glbBool isBlink );

	virtual void DirectDraw( glbInt32 level );

	virtual void DrawToImage( glbByte *image, glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext );

	virtual glbInt32 GetOsgNodeSize();

	/** ��ʱ��˳��Ϊ�� */
	glbBool SetGeo(CGlbPolygon *polygon,glbBool &isNeedDirectDraw);
	CGlbPolygon *GetGeo()const{return mpr_polygonGeo.get();}
	
	/**
	* @brief ���ö��������˳�"�༭"״̬
	* @param[in]  isEdit ����༭״̬Ϊtrue���˳�Ϊfalse
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	glbBool SetEdit(glbBool isEdit);

	//�����������ɫ[���]��������ʱ����Ҫ
	void ParseObjectFadeColor();
private:
	glbBool DrawPoints(glbInt32 color);
	glbBool SetCurrPoint(glbInt32 idx,glbInt32 color);
	glbBool SetSelectedPoint(glbInt32 idx,glbInt32 color);
	glbBool DrawRubber(glbDouble *points,glbInt32 pcnt,glbInt32 color);
	glbBool DrawPrepareAddPoint(glbDouble xOrLon,glbDouble yOrLat,glbDouble zOrAlt,glbInt32 color);
	glbBool ClearDraw();
	CGlbPolygon *RemoveRepeatePoints(CGlbPolygon *polygon);
	CGlbLine *RemoveRepeatePointsByLine(CGlbLine *line);
private:		
	void ReadData(glbInt32 level ,glbBool isDirect=false);

	void DealSelected();
	void DealBlink();
	void ComputePosByAltitudeAndGloleType(osg::Vec3d &position);
	void DealModePosByChangeAltitudeOrChangePos();

	bool judgePolygon2DNeedDirectDraw(GlbPolygon2DSymbolInfo *polygon2DSmblInfo, bool &isNeedDirectDraw);
	void updatePolygon2DSymbolInfo(osg::StateSet *sset,GlbPolygon2DSymbolInfo *polygon2DSmblInfo);
	void updatePolygon2DOutlineSymbolInfo(osg::StateSet *sset,GlbPolygon2DSymbolInfo *polygon2DSmblInfo);
	bool judgePolyhedronNeedDirectDraw(GlbPolyhedronSymbolInfo *polyhedronSmblInfo, bool &isNeedDirectDraw);
	bool judgePolygonBuildingNeedDirectDraw(GlbPolygonBuildingSymbolInfo *polyhedronSmblInfo, bool &isNeedDirectDraw);
	bool judgePolygonWaterNeedDirectDraw(GlbPolygonWaterSymbolInfo *polygonWaterSmblInfo, bool &isNeedDirectDraw);
	bool judgePolygonLakeNeedDirectDraw(GlbPolygonLakeSymbolInfo *polygonLakeSmblInfo, bool &isNeedDirectDraw);
		
	// ��������ζ����м����� - ����CGlbMultiPolygon����
	IGlbGeometry* AnalysisPolyGeomertry(CGlbFeature* feature, CGlbPolygon* polyGeo);

	glbInt32 ComputeNodeSize( osg::Node *node );

	void DirtyOnTerrainObject();

	//�ӳ������Ƴ��ڵ㣬���ڷ�������ģʽת��Ϊ������ģʽʱ���������еķ������ζ���ڵ��Ƴ�
	void RemoveNodeFromScene(glbBool isClean=true);
	
	void DealEdit();
private:
	glbDouble                                         mpr_distance;        ///<���������ľ���
	glbref_ptr<GlbRenderInfo>                         mpr_renderInfo;      ///<������Ⱦ��Ϣ
	glbref_ptr<IGlbGeometry>                          mpr_outline;         ///<�����2D����
	glbref_ptr<CGlbPolygon>                         mpr_polygonGeo;      ///<����Ķ��������
	glbDouble                                         mpr_altitude;	       ///<����κ��θ߶ȣ�skyline Ϊ���ж���ε��ƽ�����θ߶�
	glbDouble                                         mpr_elevation;		///<��ǰ���θ�

	glbBool                                           mpr_isDrawPoints;    ///<�Ƿ���Ⱦ�༭��
	glbFloat                                          mpr_TexRotation;	

	glbBool                   mpr_isCanDrawImage;		///<���������������
	glbInt32                  mpr_objSize;				///<obj��size
	GlbCriticalSection        mpr_addToScene_critical;	///<�ڵ���ӳ����ٽ���
	GlbCriticalSection		  mpr_readData_critical;	///<ReadData�����ڵ��ٽ���	
	glbBool                   mpr_needReReadData;		///<���¶�ȡ���ݱ�־
	GlbCriticalSection		  mpr_outline_critical;		///<outline��д�ٽ���		
};

}
