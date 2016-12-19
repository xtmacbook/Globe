/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeTerrainModify.h
* @brief   �����޸Ķ��� ͷ�ļ�
*
* �����������GlbGlobeTerrainModify���class
*
* @version 1.0
* @author  ����
* @date    2014-7-15 16:30
*********************************************************************/

#pragma once
//#include "GlbGlobeObjectExport.h"
#include "GlbGlobeExport.h"
#include "GlbGlobeREObject.h"
#include "GlbLine.h"
#include "CGlbGlobe.h"
#include "GlbGlobeTypes.h"
#include <osg/Geometry>

namespace GlbGlobe
{
	/**
	* @brief �����޸ķ����ṩ��
	*
	* ����̳���CGlbReference�� ���ڿͻ��Զ�������޸�
	* 
	*/
	class GLB_DLLCLASS_EXPORT IGlbGlobeTerrainProvider : public CGlbReference
	{
	public:
		IGlbGlobeTerrainProvider(){}
		virtual ~IGlbGlobeTerrainProvider(){}

		/**
		* @brief ���ö���Χ
		* @param[in]  region ��Χ
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetRegion(CGlbLine* region)=0;

		/**
		* @brief ��ȡ(xOrLon,yOrLat)���ĸ߶�ֵ
		* @param[in]  xOrLon x����򾭶�(��λ:��)
		* @param[in]  yOrLat y�����γ��(��λ:��)
		* @param[in,out]  zOrAlt z�����߶�
		* @return (xOrLon,yOrLat)�ڵ����޸Ķ���ķ�Χ�ڷ���true��ͬʱ�޸�zOrAltֵ
		���ڷ�Χ�ڷ���false.				
		*/
		virtual glbBool GetElevate(glbDouble xOrLon,glbDouble yOrLat,glbDouble &zOrAlt)=0;
	};
	/**
	* @brief �����޸Ķ���
	*
	* ����̳���CGlbGlobeREObject�� �滻��������Χ�ڵĵ��������̬
	* 
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeTerrainModify :	public CGlbGlobeREObject
	{
	public:
		CGlbGlobeTerrainModify(void);
		~CGlbGlobeTerrainModify(void);
		/**
		* @brief ��ȡ��������
		* @return ����ö������
		*/
		GlbGlobeObjectTypeEnum GetType();
		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		glbBool Load(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		glbBool Load2(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief ���л��������л��������Ե�xml�����ļ�
		* @param[in]  os �����ļ������
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return ���л��ɹ�����true����֮����false
		*/
		glbBool Save(xmlNodePtr node,const glbWChar* prjPath);
		// CGlbRObject	
		/**
		* @brief ��ȡ��������ľ���
		* @param[in]  cameraPos ����ڳ����е�λ��
		* @param[in]  isCompute �����Ƿ���Ҫ���¼��㣬���¼�������true����֮����false
		* @return ��������ľ���
		*/
		virtual glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute);
		/**
		* @brief װ�����ݣ���������Ƶ���ĵ��ø÷���
		* @param[in]  level ��Ҫװ�����ݵĵȼ�
		*/
		virtual void LoadData(glbInt32 level);
		/**
		* @brief ��osg�ڵ���߽���osg�ڵ㵽������
		*/
		virtual void AddToScene();
		/**
		* @brief �ӳ�����ժ��osg�ڵ�
		* @param[in]  isClean true��ʾͬʱɾ������osg�ڵ��Խ�ʡ�ڴ�
		* @return ���ؽ�ʡ���ڴ��С
		*/
		virtual glbInt32 RemoveFromScene(glbBool isClean);
		/**
		* @brief ��ȡ����ڵ��Χ��
		* @param[in]  isWorld true��ʾ��ȡ���������Χ��(��λ����)
		*  -                                    false��ʾ��ȡ���������Χ�У���λ���ȣ�
		* @return �ڵ��Χ��
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		virtual IGlbGeometry *GetOutLine();
		/**
		* @brief ���ö�������
		* @param[in]  isShow ������ʾ����Ϊtrue����������Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetShow(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief ���ö����ѡ��״̬
		* @param[in]  isSelected ����ѡ������Ϊtrue����֮����Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetSelected(glbBool isSelected);
		/**
		* @brief ����������Ⱦ	
		* @param[in]  level ������Ⱦ�ȼ�
		*/
		virtual void DirectDraw(glbInt32 level);
		/**
		* @brief ��ȡ��ǰLOD�ڵ�Ĵ�С����λ���ֽڡ�
		* @return ��ǰLOD����osg�ڵ��С
		*/
		virtual glbInt32 GetOsgNodeSize();

		// CGlbGlobeTerrainModify
		/**
		* @brief ���ö���Χ
		* @param[in]  region ��Χ
		* @return ���óɹ�����true����֮����false
		*/
		glbBool SetRegion(CGlbLine* region);
		/**
		* @brief ��ȡ����Χ
		* @return ��Χ
		*/
		CGlbLine* GetRegion();
		/**
		* @brief �����Ƿ�ƽ̹
		* @param[in]  isFlat �Ƿ�ƽ̹
		* @return ���óɹ�����true����֮����false
		*/
		glbBool SetFlat(glbBool isFlat);
		/**
		* @brief ��ȡ�Ƿ�ƽ̹
		* @return ƽ̹����true�����򷵻�false
		*/
		glbBool IsFlat();
		/**
		* @brief ���õ����޸ķ�ʽ
		* @param[in]  mode �����޸ķ�ʽ
		*						GLB_TERRAINMODIFY_REPLACE	�滻
		GLB_TERRAINMODIFY_BELOW		�޼��·�
		GLB_TERRAINMODIFY_UP		�޼��Ϸ�
		* @return ���óɹ�����true����֮����false
		*/
		glbBool SetModifyMode(GlbGlobeTerrainModifyModeEnum mode);
		/**
		* @brief ��ȡ�����޸ķ�ʽ
		* @return �����޸ķ�ʽ
		*/
		GlbGlobeTerrainModifyModeEnum GetModifyMode();
		/**
		* @brief ���ø߶�
		* @param[in]  hei �߶�
		* @return ���óɹ�����true����֮����false
		*/
		glbBool SetHeight(glbDouble hei);
		/**
		* @brief ��ȡ�߶�
		* @return �߶�
		*/
		glbDouble GetHeight();
		/**
		* @brief ��ȡ(xOrLon,yOrLat)���ĸ߶�ֵ
		* @param[in]  xOrLon x����򾭶�(��λ:��)
		* @param[in]  yOrLat y�����γ��(��λ:��)
		* @param[in,out]  zOrAlt z�����߶�
		* @return (xOrLon,yOrLat)�ڵ����޸Ķ���ķ�Χ�ڷ���true��ͬʱ�޸�zOrAltֵ
		���ڷ�Χ�ڷ���false.				
		*/
		glbBool GetElevate(glbDouble xOrLon,glbDouble yOrLat,glbDouble &zOrAlt);
		/**
		* @brief ���õ����޸���
		* @param[in]  op �����޸���
		* @return ���óɹ�����true��ʧ�ܷ���false
		*/
		glbBool SetElevateOp(IGlbGlobeTerrainProvider* op);

		/**
		* @brief �ж����������Ƿ���ʾ	
		* @return ��ʾ����true�����ط���false
		*/
		glbBool IsShowOutline();
		/**
		* @brief ����������������
		* @param[in]  isShow �Ƿ���ʾ�������� ��ʾΪtrue������Ϊfalse
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool SetOutlineShow(glbBool isShow);

		/**
		* @brief �ɵ����޸Ķ����޸�dem����
		* @param[in]  demdata dem����
		* @param[in]  demsize dem�ߴ�
		* @param[in]  tileExtent dem���ݵĸ�������
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool ModifyDem(glbFloat* demdata, glbInt32 demsize, CGlbExtent* tileExtent);

		/**
		* @brief ���ö��������˳�"�༭"״̬
		* @param[in]  isEdit ����༭״̬Ϊtrue���˳�Ϊfalse
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool SetEdit(glbBool isEdit);
	private:
		// ����ڵ�ռ�õ��ڴ��С
		glbInt32 ComputeNodeSize( osg::Node *node );
		// ����༭״̬
		void DealEdit();
		// ���Ʒ�Χ������
		glbBool ReadData();
		// ���Ƶ�ǰ��
		glbBool DrawCurrPoint();
		/**
		* @brief ���Ʒ�Χ���ϵĵ�͵�ǰ��
		* @param[in]  color ��Χ���ϵ����ɫ
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool DrawPoints(glbInt32 color);
		/**
		* @brief ���õ�ǰ��
		* @param[in]  idx ��ǰ���������
		* @param[in]  color ��ǰ����ɫ 
		* @return ���óɹ�����true��ʧ�ܷ���false
		*/
		glbBool SetCurrPoint(glbInt32 idx, glbInt32 color);
		/**
		* @brief ������ʱ��
		* @param[in]  xOrLon x����򾭶�(��λ:��)
		* @param[in]  yOrLat y�����γ��(��λ:��)
		* @param[in,out]  zOrAlt z�����߶�
		* @param[in]  color ��ʱ�����ɫ
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool DrawTemp(glbDouble xOrLon, glbDouble yOrLat, glbDouble zOrAlt, glbInt32 color);
		/**
		* @brief ɾ����Χ���ϵ�ĳ����
		* @param[in]  idx ���������
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool RemovePoint(glbInt32 idx);
		/**
		* @brief ������л��Ƴ��ĵ�	
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool ClearDraw();
	private:
		glbDouble							mpr_distance;				///<���������
		glbref_ptr<IGlbGeometry>			mpr_outline;				///<�����λ��ƶ���Ķ�ά������ ��λ������λ
		glbref_ptr<CGlbLine>				mpr_region;					///<�����޸ķ�Χ
		glbBool								mpr_isFlat;					///<��ʶ�Ƿ�ƽ̹
		GlbGlobeTerrainModifyModeEnum		mpr_modifymode;				///<�̱߳��ַ�ʽ
		glbDouble							mpr_height;					///<�����޸ĵĸ߶�
		glbref_ptr<IGlbGlobeTerrainProvider> mpr_terrainProvider;		///<�����㷨�ṩ����

		osg::ref_ptr<osg::Geometry>			mpr_osgOutlineGeometry;		///<������drawable
		osg::ref_ptr<osg::Geometry>			mpr_lnPtsGeometry;			///<�������ϵĵ�drawable
		osg::ref_ptr<osg::Geometry>			mpr_currPtGeometry;			///<��ǰ��drawable
		osg::ref_ptr<osg::Geometry>			mpr_tempPtGeometry;			///<��ʱ��drawable
		glbInt32							mpr_curPointIndex;			///<��ǰ��������
		glbInt32							mpr_curPointColor;			///<��ǰ����ɫ

		glbInt32                            mpr_objSize;				///<�ڵ��С
		GlbCriticalSection                  mpr_readData_critical;		///<�����ڵ��ٽ���
		GlbCriticalSection                  mpr_addToScene_critical;	///<�ڵ���ӳ����ٽ���
		glbBool								mpr_isShowOutline;
	};
}
