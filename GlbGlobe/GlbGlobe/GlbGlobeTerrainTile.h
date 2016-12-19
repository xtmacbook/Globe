/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeTerrainTile.h
* @brief   ���ο� ͷ�ļ�
*
* �����������CGlbGlobeTerrainTile���class
*
* @version 1.0
* @author  ����
* @date    2014-5-8 16:10
*********************************************************************/
#pragma once
#include <osg/BoundingBox>
#include <osg/Polytope>
#include "GlbReference.h"
#include "GlbGlobeDomLayer.h"
#include "GlbGlobeDemLayer.h"
#include "GlbGlobeRObject.h"
#include "GlbGlobeTypes.h"
#include "IGlbGlobeTask.h"
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Texture2D>

namespace GlbGlobe
{

	class CGlbGlobeTerrain;
	class CGlbGlobeTerrainTile : public CGlbReference
	{
	private:
		/** 
		* @brief ���ο�ڵ�ü��ص���[����]
		* @author ����
		* @date    2015-3-5 11:10
		* @note ���ο��ڲ���,������Ʋ��ɼ��ü�
		*/
		class CGlbGlobeTerrainTileCullCallBack : public osg::NodeCallback
		{
		public:
			/**
			* @brief ���ο�ڵ�ü��ص��๹�캯��
			* @param [in] bbox  ���ο����
			* @param [in] level ���ο�ȼ�	  
			* @return  ��
			*/
			CGlbGlobeTerrainTileCullCallBack(const osg::BoundingBox& bbox,glbInt32 level); 			

			~CGlbGlobeTerrainTileCullCallBack();
			/**
			* @brief ����osg�ڵ�ص�ʵ��
			* @param [in] node ���ο��osg�ڵ�
			* @param [in] nv osg�ڵ������	  
			* @return  ��
			*/
			virtual void operator() (osg::Node* node,osg::NodeVisitor* nv);
		private:
			osg::BoundingBox mpr_bbox;	
			//osg::Camera* mpr_p_osgCamera;
			//CGlbGlobeTerrainTile* mpr_p_terraintile;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief ���ο鹹�캯��
		* @param [in] terrain ���ζ���
		* @param [in] level ���ο�ȼ�
		* @param [in] south,north,west,east  ���ο� �����ϱ� �߽�
		* @param [in] minAlt,maxAlt ���ο����ͺ���� �߳� 
		* @param [in] parent ����	
		* @return  ��	
		*/
		CGlbGlobeTerrainTile(CGlbGlobeTerrain* terrain,glbInt32 level,
			glbDouble south,glbDouble north,glbDouble west,glbDouble east,
			glbDouble minAlt=0,glbDouble maxAlt=0,
			CGlbGlobeTerrainTile* parent=NULL);
		/**
		* @brief ���ο���������
		* @return ��
		*/
		~CGlbGlobeTerrainTile(void);
		/**
		* @brief ���ο��ʼ��
		* @param [in] domlayers domͼ�㼯��
		* @param [in] demlayers demͼ�㼯��
		* @param [in] terrainobjs �����λ��ƶ��󼯺�
		* @param [in] mTerrainobjs �����޸Ķ��󼯺�
		* @return  ��
		*/
		void Initialize(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>& mTerrainobjs,
			glbBool	load_direct=false);
		/**
		* @brief ��ȡ���ο��Ƿ��ʼ��
		* @return  �ѳ�ʼ������true
		-	 û�г�ʼ������false
		*/
		glbBool IsInitialized();
		/**
		* @brief ���ο����
		* @param [in] domlayers domͼ�㼯��
		* @param [in] demlayers demͼ�㼯��
		* @param [in] terrainobjs �����λ��ƶ��󼯺�
		* @param [in] mTerrainobjs �����޸Ķ��󼯺�
		* @return  ��
		*/
		void Update(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>& mTerrainobjs);
		/**
		* @brief ���ο����ݼ���
		* @param [in] domlayers domͼ�㼯��
		* @param [in] demlayers demͼ�㼯��
		* @param [in] terrainobjs �����λ��ƶ��󼯺�
		* @param [in] mTerrainobjs �����޸Ķ��󼯺�
		* @note ���ο��ʼ��ʱ�ļ����̵߳��ô˽ӿ�
		* @return  ��
		*/
		void LoadData(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>& mTerrainobjs);
		/**
		* @brief ���ο��������ݼ���	  
		* @param [in] domlayers domͼ�㼯��
		* @param [in] terrainobjs �����λ��ƶ��󼯺�
		* @param [in] domrefresh �Ƿ���domͼ��ˢ��
		* @note  RefreshTextureʱ�ļ����̵߳��ô˽ӿ�
		* @return  ��
		*/
		void LoadDomData(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>& domlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs,
			glbBool domrefresh);

		/**
		* @brief ���ο�dem���ݼ���	  
		* @param [in] demlayers demͼ�㼯��
		* @param [in] mterrainobjs �����޸Ķ��󼯺�
		* @note  RefreshDemʱ�ļ����̵߳��ô˽ӿ�
		* @return  ��
		*/
		void LoadDemData(	std::vector<glbref_ptr<CGlbGlobeDemLayer>>& demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>& mterrainobjs);
		/**
		* @brief ���ο�����
		* @param [in] dispos_direct �Ƿ�ֱ������ �ƺ�����Ҫ�˲�����������
		* @param [in] dispose_level_zero_tile ����0�����ο�
		* @return ��
		*/
		void Dispose(glbBool dispos_direct=false ,glbBool dispose_level_zero_tile = false);
		/**
		* @brief ˢ�µ��ο�����Ӱ��
		* @param [in] objDirtyExts ��Ҫˢ�µ������ζ�������
		* @param [in] domDirtyExts ��Ҫˢ�µ�domͼ������
		* @param [in] domlayers	domͼ�㼯��
		* @param [in] terrainObjs �����λ��ƵĶ��󼯺�
		* @note ���ӿ���domͼ��������λ��ƶ������.
		- GLB_GLOBETYPE_GLOBE��ģʽ�� Ϊ�������� ��λ����
		- GLB_GLOBETYPE_FLATƽ��ģʽ�� Ϊ�������� ��λ����
		* @return  �� 			
		*/
		void RefreshTexture(std::vector<CGlbExtent>						&objDirtyExts,
			std::vector<CGlbExtent>						&domDirtyExts,
			std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainObjs);
		/**
		* @brief ˢ�µ��ο�߳�����
		* @param [in] mtobjDirtyExts ��Ҫˢ�µĵ����޸Ķ�������
		* @param [in] demDirtyExts ��Ҫˢ�µ�demͼ������
		* @param [in] demlayers	demͼ�㼯��
		* @param [in] mterrainObjs �����޸Ķ��󼯺�
		* @param [in] operationPackage ������
		* @note ���ӿ���demͼ��͵����޸Ķ������.
		- GLB_GLOBETYPE_GLOBE��ģʽ�� Ϊ�������� ��λ����
		- GLB_GLOBETYPE_FLATƽ��ģʽ�� Ϊ�������� ��λ����
		* @return  �� 			
		*/
		void RefreshDem(	std::vector<CGlbExtent>						&mtobjDirtyExts,
			std::vector<CGlbExtent>						&demDirtyExts,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>  &demlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>   &mterrainObjs
			/*CGlbGlobeTerrain::GlbTileOperationPackage	&operationPackage*/ );
		/**
		* @brief ���ο�ӱ�
		* @return ��
		*/
		void Merge();
		/*
		* @brief ����(lonOrX,latOrY)����λ�ô��ĸ߳�   
		*	@param [in] lonOrX ���Ȼ�X  ����˵��:
		-		GLB_GLOBETYPE_GLOBE����ģʽ�£�Ϊ����(��λ����)
		-		GLB_GLOBETYPE_FLATƽ��ģʽ�£�ΪX(��λ����)
		*	@param [in] latOrY γ�Ȼ�Y ����˵��:
		-		GLB_GLOBETYPE_GLOBE����ģʽ�£�Ϊγ��(��λ����)
		-		GLB_GLOBETYPE_FLATƽ��ģʽ�£�ΪY(��λ����)
		*
		* @return �߳�(��λ:��)
		*/
		glbDouble GetElevationAt(glbDouble lonOrX, glbDouble latOrY);
		/**
		* @brief ������ο�ɼ�����
		* @param [in] cv ��׶��
		* @param [in] min_southDegree ��Ҫ�������γ��ʼ�Ƕ� (Ĭ��Ϊ-66.5��)
		* @param [in] max_norhtDegree ��Ҫ����ı�γ�����Ƕ� (Ĭ��Ϊ66.5��)
		* @return  �ɼ��ķ�Χboundingbox
		*/
		osg::BoundingBox GetVisibleExtent(osg::Polytope cv,glbDouble min_southDegree, glbDouble max_norhtDegree);
		/**
		* @brief ��ȡ���ο�ȼ�
		* @return ���ο�ȼ�
		*/
		glbInt32 GetLevel();
		/**
		* @brief ��ȡ���ο�����к�
		* @param [out] row �к�
		* @param [out] column �к�
		* @return ��
		*/
		void GetPosition(glbInt32& row, glbInt32& column);
		/**
		* @brief ���õ��ο�����к�
		* @param [in] row �к�
		* @param [in] column �к�
		* @return ��
		*/
		glbBool SetPosition(glbInt32 row, glbInt32 column);
		/**
		* @brief ��ȡ���ο�ķ�Χ
		* @param [out] south,north,west,east  ���ο� �����ϱ� �߽�
		* @param [out] minAlt,maxAlt ���ο����ͺ���� �߳� 
		* @return ��
		*/
		void GetRange(glbDouble& east, glbDouble& west, glbDouble& south, glbDouble& north, glbDouble& minAlt, glbDouble& maxAlt);

		/**
		* @brief ��ȡ���ο�ķ�Χ
		* @return ���ο鷶Χ
		*/
		CGlbExtent* GetExtent();
		/**
		* @brief ��ȡ���ο����һ�����׿�
		* @return ����
		*/
		CGlbGlobeTerrainTile* GetParentTile();
		/**
		* @brief ��ȡ���ο��Ƿ�ɼ�
		* @return �ɼ�����true
		-	���ɼ�����false
		*/
		glbBool IsVisible();
		/**
		* @brief ��ȡ���ο��ڸ����еķ�λ
		* @return ��λ
		- GlB_SOUTHWEST	����
		- GLB_SOUTHEAST ����
		- GLB_NORTHWEST ����
		- GLB_NORTHEAST ����
		*/
		GlbGlobeChildLocationEnum GetTileLocation();
		/**
		* @brief ��ȡ���ο����� [����]
		* @return �������			
		*/
		osg::Texture2D* GetTexture();
		/**
		* @brief ��ȡ���ο��dem�߳�����
		* @param [out] p_data �߳�����
		* @param [out] dem_columns �߳���������
		* @param [out] dem_rows �߳���������
		* @return ��
		*/
		void GetDemdata(glbFloat** p_data, glbInt32 &dem_columns, glbInt32 &dem_rows);
		/**
		* @brief ��ȡ���ο��dom��������
		* @param [out] p_data ��������
		* @param [out] dom_columns  ������������
		* @param [out] dom_rows  ������������
		* @return ��
		*/
		void GetDomdata(glbByte** p_data, glbInt32 &dom_columns, glbInt32 &dom_rows);
		/**
		* @brief ��ȡ���ο�Ķ����ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeTerrainTile* GetNorthEastChild();
		/**
		* @brief ��ȡ���ο�������ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeTerrainTile* GetNorthWestChild();
		/**
		* @brief ��ȡ���ο�Ķ����ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeTerrainTile* GetSouthEastChild();
		/**
		* @brief ��ȡ���ο�������ӿ�
		* @return ��������ӿ���ڷ����ӿ�ָ��
		-   ��������ӿ鲻���ڷ���NULL
		*/
		CGlbGlobeTerrainTile* GetSouthWestChild();
		/**
		* @brief ��ȡ���ο��Ե�ĸ߳�����
		* @param [in] edge ���ο��Ե
		* @return �߳���������
		*/
		osg::Vec3dArray* GetEdgeVertexArray(GlbTileEdgeEnum edge);
		/**
		* @brief ���õ��ο鲻͸����
		* @param [in] domlayers	domͼ�㼯��
		* @param [in] terrainObjs �����λ��ƵĶ��󼯺�
		* @param [in] opacity ��͸����
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetOpacity(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainObjs,
			glbInt32 opacity);

		/**
		* @brief ���õ��ο����ϵ��
		* @param [in] exaggrate ���ο���ϵ��	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetExaggrate(glbDouble exaggrate);

		/**
		* @brief ��ȡ���ζ���[����]
		* @return ���ζ���
		*/
		CGlbGlobeTerrain* GetTerrain();

		/**
		* @brief ˢ�µ��ο�ӱߵ�geometry�Ķ�������
		* @return ���ζ���
		*/
		glbBool UpdateEdgeGeometryVertexArray();
		/** 
		* @brief ����tile��Χ���˳���˵ؿ��ཻ�����ػ��ƶ���
		* @param [in] terrainobjs �����λ��ƶ��󼯺�
		* @param [out] outObjs  ����ο��ཻ�Ķ��󼯺�
		* @return  �н��� ����true ��û�н��� ���� false
		*/
		glbBool FilterOnTerrainObjects(std::vector<glbref_ptr<CGlbGlobeRObject>>& terrainobjs, std::vector<glbref_ptr<CGlbGlobeRObject>>& outObjs);
		/** 
		* @brief ����tile��Χ���˳���˵ؿ��ཻ��"��"���򼯺�
		* @param [in] objDirtyExts "��"���򼯺�
		* @param [out] outObjDirtyExts  ����ο��ཻ��"��"���򼯺�
		* @return  �н��� ����true ��û�н��� ���� false
		*/
		glbBool FilterObjDirtyExt(std::vector<CGlbExtent>&objDirtyExts, std::vector<CGlbExtent>& outObjDirtyExts);

		// ���ԡ�������
		glbDouble computeSrcProjArea();
	private:
		void UpdateGlobe(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>> &demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &mTerrainobjs);
		void UpdateGlobe_Pro(std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs);

		void UpdateFlat(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs);

		void MergeGlobe();
		void MergeFlat();

		glbDouble SphericalDistance(glbDouble latA, glbDouble lonA, glbDouble latB, glbDouble lonB);

		void ComputeChildren();
		glbInt32 GetRowFromLatitude(glbDouble latitude, glbDouble tileSize);
		glbInt32 GetColFromLongitude(glbDouble longitude, glbDouble tileSize);
		// ���ص��ο�dom��dem����
		glbBool LoadTileData();
		// ���ظ��ǵ��ο��domlayer��terrainobjects����->image�γ�����
		glbBool LoadOverlayTextureData(	std::vector<glbref_ptr<CGlbGlobeDomLayer>>	&domlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainobjs);
		// ���ظ��Ǵ˵��ο��demlayer��mterrainobjects����->ֱ���޸ĵ��ο��dem����17*17
		glbBool LoadOverlayDemData(		std::vector<glbref_ptr<CGlbGlobeDemLayer>>	&demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>	&mTerrainobjs);	
		// ��ֵ���ܲ���dem����
		glbFloat* MultifyDemSection(glbFloat* p_data, glbInt32 dem_columns, glbInt32 dem_rows, GlbGlobeChildLocationEnum location);
		// ��ֵ���ܲ���dom����
		glbByte* MultifyDomSection(glbByte* p_data, glbInt32 dom_columns, glbInt32 dom_rows, GlbGlobeChildLocationEnum location);
		// �������ο��������ڵ�(����geometry)
		osg::ref_ptr<osg::Group> CreateElevatedMesh();
		// ��ȡ��row,column�����߳�ֵ
		glbFloat GetHeightData(glbFloat* p_heightdatas, glbInt32 column, glbInt32 row, GlbGlobeChildLocationEnum location);
		// ���ݵȼ�������ɫ
		osg::Vec4 AdjustTileColor(long level);
		// �жϿ��Ƿ���Ҫ����
		glbBool IsTileSplit();
		// ��������ת��Ļ����
		void WorldToScreen(glbDouble &x, glbDouble &y, glbDouble &z);
		// ɾ���߽�geometry
		void RemoveEdgeGeometry(GlbTileEdgeEnum edge);
		// ���ɱ߽�geometry
		void BuildEdgeGeometry(osg::Vec3dArray* p_neighbour_edge_vertex_array,GlbTileEdgeEnum edge);
		// �޸ı߽�geometry
		void ModifyEdgeGeometry(osg::Vec3dArray* p_neighbour_edge_vertex_array,GlbTileEdgeEnum edge);
		// ��ȡ��������tile����
		CGlbGlobeTerrainTile* GetNorthDirectionTile();
		CGlbGlobeTerrainTile* GetNorthDirectionTileOnFlatTerrain();
		// ��ȡ��������tile����
		CGlbGlobeTerrainTile* GetWestDirectionTile();
		CGlbGlobeTerrainTile* GetWestDirectionTileOnFlatTerrain();
		// ��ȡ��������tile����
		CGlbGlobeTerrainTile* GetSouthDirectionTile();
		CGlbGlobeTerrainTile* GetSouthDirectionTileOnFlatTerrain();
		// ��ȡ��������tile����
		CGlbGlobeTerrainTile* GetEastDirectionTile();
		CGlbGlobeTerrainTile* GetEastDirectionTileOnFlatTerrain();

		CGlbGlobeTerrainTile* GetNorthNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south);
		CGlbGlobeTerrainTile* GetWestNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south);
		CGlbGlobeTerrainTile* GetSouthNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south);
		CGlbGlobeTerrainTile* GetEastNeighbourInTileRange(glbDouble west, glbDouble east, glbDouble north, glbDouble south);

		osg::ref_ptr<osg::Vec3dArray> GetNeighbourTileEdgeVecArray_Ext(CGlbGlobeTerrainTile* p_neighbour_tile,GlbTileEdgeEnum base_edge);

		BOOL IsEdgeNeedMerge(CGlbGlobeTerrainTile* p_base_tile, 
			CGlbGlobeTerrainTile* p_neighbout_tile, GlbTileEdgeEnum base_edge, osg::Vec3dArray* p_VertexArray);

		CGlbGlobeTerrainTile* GetTopMostTile(CGlbGlobeTerrainTile* p_tile);

		/*
		�ݹ�Ѱ�Ҿ�ȷ�ɼ�����tile
		loopdepth : ������� [Ĭ��Ϊ3]
		*/
		osg::BoundingBox AAA(glbDouble childSouth,glbDouble childNorth,glbDouble childWest,glbDouble childEast,
			glbDouble childMinHei, glbDouble childMaxHei,glbInt32 level, glbInt32 row, glbInt32 column, 
			osg::Polytope cv,glbDouble min_southDegree, glbDouble max_norhtDegree,glbInt32 loopdepth=2);

		GlbGlobeChildLocationEnum GetLocationFromLongitudeLatitude(glbDouble longOrX,glbDouble latOrY);

		// �ж�domlayer�뱾tile�Ƿ��н���
		glbBool IsDomlayerInterTile(CGlbGlobeDomLayer* pDomlayer);
		// �ж�demlayer�뱾tile�Ƿ��н���
		glbBool IsDemlayerInterTile(CGlbGlobeDemLayer* pDemlayer);
		// ��maxlevel��ֵ����tile��domlayer����
		glbBool InterpolateDomLayerData(CGlbGlobeDomLayer* pLayer,glbInt32 maxLevel,CGlbPixelBlock* p_pixelblock);
		// ��maxlevel��ֵ����tile��demlayer����
		glbBool InterpolateDemLayerData(CGlbGlobeDemLayer* pLayer,glbInt32 maxLevel,CGlbPixelBlock* p_pixelblock);
		// �ݹ��ֵ���ɵ�ǰ���demlayer��dem����
		glbBool InterpolateDemDataFromDemLayer(glbFloat* demData, glbByte* dataMask,glbInt32 row, glbInt32 column, glbDouble &srcWest,glbDouble &srcEast, glbDouble &srcSouth,glbDouble &srcNorth);
		// ���¼���mpr_boundbox
		void ComputeBoundBox();

		// �ݹ����edge�ߵĶ�������
		glbBool UpdateEdgeGeomVerttexArray(GlbTileEdgeEnum edge);

		// ���������ĸ�geometry
		void RebuildElevatedMesh();

		// �����ȡ�����������ɽ���Զ�Ŀ�����
		glbBool GetSortedChildTiles(std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>>& tiles);	

	public:
		void RefreshTexture_Step1(std::vector<CGlbExtent>						&objDirtyExts,
								std::vector<CGlbExtent>						&domDirtyExts,
								std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,                   
								std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainObjs);
		void RefreshTexture_Step2(std::vector<CGlbExtent>						&objDirtyExts,
								std::vector<CGlbExtent>						&domDirtyExts,
								std::vector<glbref_ptr<CGlbGlobeDomLayer>>  &domlayers,                   
								std::vector<glbref_ptr<CGlbGlobeRObject>>	&terrainObjs);
	private:
		CGlbGlobeTerrain*	mpr_terrain;				//���ζ���
		glbref_ptr<CGlbGlobeTerrainTile> mpr_nwchild;	//�����ӿ�
		glbref_ptr<CGlbGlobeTerrainTile> mpr_nechild;	//�����ӿ�
		glbref_ptr<CGlbGlobeTerrainTile> mpr_swchild;	//�����ӿ�
		glbref_ptr<CGlbGlobeTerrainTile> mpr_sechild;	//�����ӿ�
		glbref_ptr<CGlbGlobeTerrainTile> mpr_parent_tile;	//����
		GlbGlobeChildLocationEnum mpr_location_enum;		//�����ڸ����е�λ��
		//osg::ref_ptr<osg::MatrixTransform> mpr_node;		//osg�ڵ�
		osg::ref_ptr<osg::Group> mpr_node;				//osg�ڵ� - switch
		osg::ref_ptr<osg::Texture2D> mpr_texture;		//���ο�����
		glbDouble mpr_south;							//�ϱ߽�
		glbDouble mpr_north;							//���߽�
		glbDouble mpr_east;								//���߽�
		glbDouble mpr_west;								//���߽�
		glbDouble mpr_minalt;							//��С�߶�
		glbDouble mpr_maxalt;							//���߶�
		glbDouble mpr_center_longitude;					//�����ĵ�ľ���
		glbDouble mpr_center_latitude;					//�����ĵ��γ��
		glbref_ptr<CGlbExtent>	mpr_extent;				//���ο鷶Χ		
		osg::BoundingBox mpr_boundbox;					//���ο鷶Χ
		//std::vector<osg::Vec3> mpr_surrond_pointvec;
		osg::Vec3d		mpr_local_origin;				//���ο��׼��[Ĭ��Ϊ���ο�����ĵ�]
		glbBool mpr_isinitialized;						//��ʼ����־
		glbBool	mpr_isLoadingData;						//���ڼ������ݱ�־
		glbBool mpr_isDataLoaded;						//��־�����Ƿ��Ѿ��������
		glbInt32 mpr_level;								//�ȼ�
		glbInt32 mpr_row;								//�к�
		glbInt32 mpr_column;							//�к�
		//CRITICAL_SECTION mpr_critical;				//�ٽ���
		glbByte* mpr_image;								//256*256*4 ���ο�dom����
		glbInt32	mpr_imagesize;						//dom������
		glbByte* mpr_origin_image;						//ԭʼ���ο�dom����

		glbByte* mpr_overlayImage;						//����dom ��DOMͼ���ںϺ��256*256*4
		osg::ref_ptr<osg::Image> mpr_p_osgImg;			//��Ӧ���ɵ�osg::Image����	

		GlbGlobeTypeEnum mpr_globe_type;				//globe��������
		glbFloat*		mpr_demdata;					//dem����17*17
		glbFloat*		mpr_overlaydemdata;				//���ο�dem �ںϵ����޸Ķ����DEMͼ����17*17
		glbInt32		mpr_demsize;					//dem������
		glbInt32		mpr_vertex_count;

		glbDouble		mpr_longitude_span;
		glbDouble		mpr_latitude_span;	

		// �����geometry
		osg::ref_ptr<osg::Geometry> mpr_p_north_easthalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_north_westhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_south_easthalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_south_westhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_east_northhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_east_southhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_west_northhalf_geom;
		osg::ref_ptr<osg::Geometry> mpr_p_west_southhalf_geom;
		// �߽綥�㼯
		osg::ref_ptr<osg::Vec3dArray> mpr_p_north_easthalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_north_westhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_south_easthalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_south_westhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_east_northhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_east_southhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_west_northhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_west_southhalf_vertexarray;

		osg::ref_ptr<osg::Vec3dArray> mpr_p_center_westhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_center_easthalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_center_northhalf_vertexarray;
		osg::ref_ptr<osg::Vec3dArray> mpr_p_center_southhalf_vertexarray;

		osg::ref_ptr<osg::Vec2Array> mpr_p_north_easthalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_north_westhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_south_easthalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_south_westhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_east_northhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_east_southhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_west_northhalf_texcoords;
		osg::ref_ptr<osg::Vec2Array> mpr_p_west_southhalf_texcoords;
		osg::ref_ptr<osg::Geode> mpr_p_geode;
		osg::ref_ptr<osg::Geometry> mpr_p_northWestgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_northEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southEastgeom;
		osg::ref_ptr<osg::Geometry> mpr_p_southWestgeom;	

		glbref_ptr<IGlbGlobeTask>	mpr_terrainTask;

		// ���Դ���
		glbInt32  mpr_opacity;

		glbBool						mpr_textureRefreshed;
	};
}