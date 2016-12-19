/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeTerrainTileRequestTask.h
* @brief   ���ο�������������
*
* �����������CGlbGlobeTerrainTileRequestTask,CGlbGlobeRefreshTileTextureTask��2��class
*
* @version 1.0
* @author  ����
* @date    2014-5-27 16:10
*********************************************************************/
#pragma once

#include "IGlbGlobeTask.h"
#include "glbref_ptr.h"
#include "GlbGlobeTerrainTile.h"

namespace GlbGlobe
{
	/**
	*@brief ���ο��ʼ��������
	*/
	class CGlbGlobeTerrainTileRequestTask :	public IGlbGlobeTask
	{
	public:
		/**
		* @brief ���캯��
		* @param [in] tile ���ο����	  
		* @param [in] domlayers domͼ�㼯��
		* @param [in] demlayers demͼ�㼯��
		* @param [in] terrainobjs ���ػ��ƶ��󼯺�
		* @param [in] mterrainobjs �����޸Ķ��󼯺�
		*/
		CGlbGlobeTerrainTileRequestTask(CGlbGlobeTerrainTile* tile,
			std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>> mterrainobjs);
		~CGlbGlobeTerrainTileRequestTask(void);
		/**
		* @brief ִ������
		* @return �ɹ�����true
		-	ʧ�ܷ���false
		*/
		glbBool doRequest();
		/**
		* @brief ��ȡ������صĶ���
		* @return �ɹ����ض���ָ��
		-	ʧ�ܷ���NULL
		*/
		CGlbGlobeObject* getObject();
	private:
		glbref_ptr<CGlbGlobeTerrainTile> mpr_terraintile;
		std::vector<glbref_ptr<CGlbGlobeDomLayer>> mpr_domlayers;
		std::vector<glbref_ptr<CGlbGlobeDemLayer>> mpr_demlayers;
		std::vector<glbref_ptr<CGlbGlobeRObject>> mpr_terrainobjs; 
		std::vector<glbref_ptr<CGlbGlobeRObject>> mpr_mterrainobjs;
	};

	/**
	*@brief ���ο��������������
	*/
	class CGlbGlobeRefreshTileTextureTask :	public IGlbGlobeTask
	{
	public:
		/**
		* @brief ���캯��
		* @param [in] tile ���ο����	  
		* @param [in] domlayers domͼ�㼯��
		* @param [in] terrainobjs ���ػ��ƶ��󼯺�
		* @param [in] isdomrefresh �Ƿ���domͼ��ˢ��
		*/
		CGlbGlobeRefreshTileTextureTask(CGlbGlobeTerrainTile* tile,									
			std::vector<glbref_ptr<CGlbGlobeDomLayer>>   domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  terrainObjs,
			glbBool isdomrefresh = false);

		~CGlbGlobeRefreshTileTextureTask(void);
		/**
		* @brief ִ������
		* @return �ɹ�����true
		-	ʧ�ܷ���false
		*/
		glbBool doRequest();
		/**
		* @brief ��ȡ������صĶ���
		* @return �ɹ����ض���ָ��
		-	ʧ�ܷ���NULL
		*/
		CGlbGlobeObject* getObject();
	private:
		glbref_ptr<CGlbGlobeTerrainTile> mpr_terraintile;
		std::vector<glbref_ptr<CGlbGlobeDomLayer>> mpr_domlayers;
		std::vector<glbref_ptr<CGlbGlobeRObject>> mpr_terrainobjs; 
		glbBool mpr_isDomrefesh;
	};


	/**
	*@brief ���ο�dem����������
	*/
	class CGlbGlobeRefreshTileDemTask :	public IGlbGlobeTask
	{
	public:
		/*
		* @brief ���캯��
		* @param [in] tile ���ο����	  
		* @param [in] demlayers demͼ�㼯��
		* @param [in] mterrainobjs �����޸Ķ��󼯺�	  
		*/
		CGlbGlobeRefreshTileDemTask(CGlbGlobeTerrainTile* tile,									
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>   demlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  mterrainObjs);

		~CGlbGlobeRefreshTileDemTask(void);
		/**
		* @brief ִ������
		* @return �ɹ�����true
		-	ʧ�ܷ���false
		*/
		glbBool doRequest();
		/**
		* @brief ��ȡ������صĶ���
		* @return �ɹ����ض���ָ��
		-	ʧ�ܷ���NULL
		*/
		CGlbGlobeObject* getObject();
	private:
		glbref_ptr<CGlbGlobeTerrainTile> mpr_terraintile;
		std::vector<glbref_ptr<CGlbGlobeDemLayer>> mpr_demlayers;
		std::vector<glbref_ptr<CGlbGlobeRObject>> mpr_mterrainobjs; 
	};
}