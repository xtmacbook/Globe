/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeTerrain.h
* @brief   ���� ͷ�ļ�
*
* �����������CGlbGlobeTerrain���class
*
* @version 1.0
* @author  ����
* @date    2014-5-8 16:10
*********************************************************************/
#pragma once

#include "GlbReference.h"
#include "GlbGlobeObject.h"
#include "GlbWString.h"
#include "GlbGlobeTerrainTile.h"
#include <osg/NodeCallback>

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief ������			 
	* @version 1.0
	* @author  ����
	* @date    2014-5-8 16:10
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeTerrain : public CGlbReference
	{
	public:
		enum GLBOperationEnum
		{// �����������
			GLB_OPENERAION_ADD = 0,
			GLB_OPENERAION_REMOVE = 1,
			GLB_OPENERAION_MODIFY = 2
		};

		enum GLBOperationObjectEnum
		{// �����������
			GLB_NODE = 0,
			GLB_DRAWABLE = 2,
			GLB_VECARRAY = 3,
			GLB_TEXTURE = 4
		};
		/** 
		* @brief ���ο��޸���Ϣ�ṹ��
		* @note �����ڲ��ṹ,�����ο�ʹ��
		*/
		struct GlbTileOperation
		{
		public:	
			/**
			* @brief �ṹ�幹�캯��
			* @param addordelormodity ��������. 0ɾ��, 1���, 2�޸�
			* @param nodeORdrawableORvecarray ��������. 0�ڵ�, 1�ɻ��ƶ���, 2��������
			* @param p_tile_node �ڵ�
			* @param p_drawable �ɻ��ƶ���
			* @param p_vecarray �������� 
			* @return  ��	
			*/
			GlbTileOperation(	GLBOperationEnum addordelormodity, 
				GLBOperationObjectEnum operation, 
				osg::Group* p_tile_node,
				double compiletime=0.0);

			GlbTileOperation(	GLBOperationEnum addordelormodity, 
				GLBOperationObjectEnum operation, 
				osg::Geode* p_tile_geode_node,
				osg::Drawable* p_drawable=NULL, 
				osg::Vec3Array* p_vecarray=NULL);

			GlbTileOperation ( GLBOperationEnum addordelormodity, 
				GLBOperationObjectEnum operation,
				osg::Texture* p_texture = NULL,
				osg::Image* p_image = NULL);
			virtual ~GlbTileOperation();
		public:
			GLBOperationEnum _addordelormodify;				///< 0 - add; 1 - delete	; 2 - modify
			GLBOperationObjectEnum _nodeordrawableorvecarrayortexture;		///< 0 - node ; 1 - drawable	; 2 - vec3array ; 3 - texture
			osg::ref_ptr<osg::Group> _p_tile_node;			// �ڵ�
			osg::ref_ptr<osg::Geode> _p_tile_geode_node;	///< Ҷ�ӽڵ�
			osg::ref_ptr<osg::Drawable> _p_drawable; ///< �ɻ��ƶ���
			osg::ref_ptr<osg::Vec3Array> _p_vecarray; ///< ��������
			double _node_compiletime;						///< ����_p_tile_node��Ҫ��ʱ��
			osg::ref_ptr<osg::Texture>	_p_texture;
			osg::ref_ptr<osg::Image>	_p_image;
		};
	private:
		/** 
		* @brief ����osg�ڵ���»ص���
		* @author ����
		* @date    2014-5-12 11:10
		* @note �����ڲ���,����ժ�ҽڵ�,drawable
		*/
		class CGlbGlobeTerrainCallBack : public osg::NodeCallback
		{
		public:
			/**
			* @brief ����osg�ڵ�ص��๹�캯��
			* @param [in] pGlobeTerrain ���ζ���	  
			* @return  ��
			*/
			CGlbGlobeTerrainCallBack(CGlbGlobeTerrain* pGlobeTerrain); 	
			/**
			* @brief ����osg�ڵ�ص�ʵ��
			* @param [in] node ���ε�osg�ڵ�
			* @param [in] nv osg�ڵ������	  
			* @return  ��
			*/
			virtual void operator() (osg::Node* node,osg::NodeVisitor* nv);
		public:
			CGlbGlobeTerrain* m_p_globe_terrian;
		};
	public:
		/**
		* @brief ���ι��캯��
		* @param [in] globe ������������	  
		* @return  ��
		*/
		CGlbGlobeTerrain(CGlbGlobe* globe);
		/**
		* @brief ������������	   
		* @return  ��
		*/
		~CGlbGlobeTerrain(void);

		/**
		* @brief ���õ������ݼ�
		* @param [in] tdataset �������ݼ�	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetTerrainDataset(IGlbTerrainDataset* tdataset);
		/**
		* @brief ��ȡ�������ݼ� 
		* @return  �ɹ����ص������ݼ�ָ��
		-	 ʧ�ܷ���NULL
		*/
		IGlbTerrainDataset* GetTerrainDataset();
		/**
		* @brief ���γ�ʼ��	   
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Initialize();
		/**
		* @brief ��ȡ�����Ƿ��ʼ��	   
		* @return  �ѳ�ʼ������true
		-	 û�г�ʼ������false
		*/
		glbBool IsInitialized();
		/**
		* @brief ���θ���
		* @param [in] domlayers domͼ�㼯��
		* @param [in] demlayers demͼ�㼯��
		* @param [in] terrainobjs �����λ��ƶ��󼯺�
		* @param [in] mTerrainobjs �����޸Ķ��󼯺�
		* @return  ��
		*/
		void Update(std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>> &demlayers,
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainobjs, 
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &mTerrainobjs);
		/**
		* @brief ��������  
		* @return  ��
		*/
		void Dispose();
		/**
		* @brief ��ӵ����޸Ĳ���
		* @param [in] opera �����޸Ĳ���	  
		* @return ��
		*/
		void AddOperation(GlbTileOperation opera);
		/**
		* @brief �����Ҫ�������ĵ����޸Ĳ���
		* @param [in] opera �����޸Ĳ���	  
		* @return ��
		*/
		void AddPackageOperation(GlbTileOperation opera);
		/**
		* @brief ������Ļ��ĳ��(x,y)���ĵ�������      
		*	@param [in] x ��Ļ���x����
		* @param [in] y ��Ļ���y����
		* @param [out] lonOrX ���Ȼ�X
		* @param [out] latOrY γ�Ȼ�Y
		* @param [out] altOrZ �߶Ȼ�Z

		* @note �����(x,y)Ϊ��Ļ����,���������½ǵ㴦Ϊԭ�㣨0,0��
		- GLB_GLOBETYPE_GLOBE��ģʽ�� lonOrX,latOrY��λΪ��
		- GLB_GLOBETYPE_FLATƽ��ģʽ�� lonOrX,latOrY��λΪ��

		* @return �ɹ�����true
		*		-	ʧ�ܷ���false
		*/
		glbBool ScreenToTerrainCoordinate(glbInt32 x,glbInt32 y,
			glbDouble &lonOrx,glbDouble &latOry,glbDouble &altOrz);
		/**
		* @brief ���������Ƿ�������ཻ      
		*	@param [in] ln_pt1������һ��(�˵�)
		* @param [in] ln_dir���߷���
		* @param [out] InterPos ����ཻ����������	
		* @return �ཻ����true
		*		-	���ཻ����false
		*/
		glbBool IsRayInterTerrain(osg::Vec3d ln_pt1, osg::Vec3d ln_dir, osg::Vec3d &InterPos);
		/**
		* @brief ����osg�ڵ�ĸ��»ص�
		* @param [in] node osg�ڵ�
		* @param [in] nv osg�ڵ������
		* @return  �ɹ�����S_OK,S_FALSE
		-	 ʧ�ܷ���E_FAIL,E_POINTER
		*/
		HRESULT UpdateOsgCallBack(osg::Node* node,osg::NodeVisitor* nv);
		/**
		* @brief ˢ�µ�������Ӱ��
		* @param [in] objDirtyExts ��Ҫˢ�µ������ζ�������
		* @param [in] domDirtyExts ��Ҫˢ�µ�domͼ������
		* @param [in] domlayers	domͼ�㼯��
		* @param [in] terrainObjs �����λ��ƵĶ��󼯺�
		* @note ���ӿ���domͼ��������λ��ƶ������.
		- GLB_GLOBETYPE_GLOBE��ģʽ�� Ϊ�������� ��λ����
		- GLB_GLOBETYPE_FLATƽ��ģʽ�� Ϊ�������� ��λ����
		* @return  �� 			
		*/
		void RefreshTexture(std::vector<CGlbExtent>                    &objDirtyExts,
			std::vector<CGlbExtent>                    &domDirtyExts,
			std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainObjs );
		/**
		* @brief ˢ�µ��θ߳�����
		* @param [in] mtobjDirtyExts ��Ҫˢ�µĵ����޸Ķ�������
		* @param [in] demDirtyExts ��Ҫˢ�µ�demͼ������
		* @param [in] demlayers	demͼ�㼯��
		* @param [in] mterrainObjs �����޸Ķ��󼯺�
		* @note ���ӿ���demͼ��͵����޸Ķ������.
		- GLB_GLOBETYPE_GLOBE��ģʽ�� Ϊ�������� ��λ����
		- GLB_GLOBETYPE_FLATƽ��ģʽ�� Ϊ�������� ��λ����
		* @return  �� 			
		*/
		void RefreshDem(std::vector<CGlbExtent>                      &mtobjDirtyExts,
			std::vector<CGlbExtent>                      &demDirtyExts,
			std::vector<glbref_ptr<CGlbGlobeDemLayer>>   &demlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>    &mterrainObjs );
		/**
		* @brief ���õ��β�͸����
		* @param [in] domlayers	domͼ�㼯��
		* @param [in] terrainObjs �����λ��ƵĶ��󼯺�
		* @param [in] opacity ���β�͸����	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetOpacity(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,                   
			std::vector<glbref_ptr<CGlbGlobeRObject>>  &terrainObjs,
			glbInt32 opacity);
		/**
		* @brief ��ȡ���β�͸����  
		* @return ��͸����
		*/
		glbInt32 GetOpacity();
		/**
		* @brief ���õ��ο���ϵ��
		* @param [in] exaggrate ���ο���ϵ��	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetExaggrate(glbDouble exaggrate);
		/**
		* @brief ��ȡ���ο���ϵ��
		* @return ���ο���ϵ��
		*/
		glbDouble  GetExaggrate();
		/**
		* @brief ���õ�������
		* @param [in] visible �������� ��ʾΪtrue,����Ϊfalse	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetVisible(glbBool visible);
		/**
		* @brief ��ȡ��������
		* @return �������� 
		- ��ʾΪtrue,����Ϊfalse	  
		*/
		glbBool GetVisible();
		/**
		* @brief ��ȡ���νڵ�
		* @return ���νڵ�   
		*/
		osg::Node* GetNode();
		/**
		* @brief ��ȡ(lonOrX,latOrY)����λ�ô��ĸ߳�   
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
		* @brief ֪ͨ����ڵ����ϻ������
		* @param [in] underground �����ϻ������. ������Ϊfalse��������Ϊtrue
		* @return ��
		*/
		void NotifyCameraIsUnderground(glbBool underground);
		/**
		* @brief ��ȡ���η�Χ
		* @param [out] west ���߽�	  
		* @param [out] east ���߽�
		* @param [out] south �ϱ߽�
		* @param [out] north ���߽�
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool GetExtent(glbDouble &west, glbDouble &east, glbDouble &south , glbDouble &north);
		/**
		* @brief �������ɼ�����
		* @param [out] extent_one �ɼ�����1	  
		* @param [out] extent_two �ɼ�����2
		* @param [in] min_southDegree ��Ҫ�������γ��ʼ�Ƕ� (Ĭ��Ϊ-66.5��)
		* @param [in] max_norhtDegree ��Ҫ����ı�γ�����Ƕ� (Ĭ��Ϊ66.5��)
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool  ComputeVisibleExtents(CGlbExtent& extent_one, CGlbExtent& extent_two,
			glbDouble min_southDegree=-66.5, glbDouble max_norhtDegree=66.5);
		/**
		* @brief ����������رձ����޳�
		* @param [in] enablecull �����޳� ����Ϊtrue,�ر�Ϊfalse	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		void EnableCullFace(glbBool enablecull);
		/**
		* @brief ���θ���������ر�
		* @param [in] enableupdate ���θ��� ����Ϊtrue,�ر�Ϊfalse	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		void EnableUpdate(glbBool enableupdate);
		/**
		* @brief ���θ����Ƿ�����	 	  
		* @return  �����з���true
		-	 ֹͣ�˷���false
		*/
		glbBool IsUpdateEnable();
		/**
		* @brief ��ȡ��ǰ������ͼ 
		* @return  �ɹ�������ͼ����ָ��
		-	 ʧ�ܷ���NULL
		*/
		CGlbGlobeView* GetView();
		/**
		* @brief ��ȡGLOBEģʽ��0���ķֿ����[Ĭ��Ϊ36��]
		* @return  0���ֿ����
		*/
		glbInt32 GetLevelZeroTileSizeOfDegrees();
		/**
		* @brief ��ȡ0����Ӧkey��tile[GLOBEģʽ]
		* @return  0�����ο�
		*/
		CGlbGlobeTerrainTile* GetTerrainLevelZeroTile(glbInt32 key);
		/**
		* @brief ��ȡp_tile�����Ķ�����ı������ڿ�
		* @param [in] p_tile ���ο�	 
		* @return  ���ο����ָ��
		*/
		CGlbGlobeTerrainTile* GetTopmostNorthNeibourTile(CGlbGlobeTerrainTile* p_tile);
		/**
		* @brief ��ȡp_tile�����Ķ�������ϱ����ڿ�
		* @param [in] p_tile ���ο�	 
		* @return  ���ο����ָ��
		*/
		CGlbGlobeTerrainTile* GetTopmostSouthNeibourTile(CGlbGlobeTerrainTile* p_tile);
		/**
		* @brief ��ȡp_tile�����Ķ�������������ڿ�
		* @param [in] p_tile ���ο�	 
		* @return  ���ο����ָ��
		*/
		CGlbGlobeTerrainTile* GetTopmostWestNeibourTile(CGlbGlobeTerrainTile* p_tile);
		/**
		* @brief ��ȡp_tile�����Ķ�����Ķ������ڿ�
		* @param [in] p_tile ���ο�	 
		* @return  ���ο����ָ��
		*/
		CGlbGlobeTerrainTile* GetTopmostEastNeibourTile(CGlbGlobeTerrainTile* p_tile);
		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		glbBool Load(xmlNodePtr node,const glbWChar* prjPath=NULL);
		/**
		* @brief ���л��������л��������Ե�xml�����ļ�
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return ���л��ɹ�����true����֮����false
		*/
		glbBool Save(xmlNodePtr node,const glbWChar* prjPath=NULL);
		 /*
		  * @brief �����Ӱ����ͼ�����
		  * @param [in] layer ��Ӱ����ͼ�����
		  * @return �� 
		  */
		void AddTiltPhotographLayer(CGlbGlobeLayer* layer);
		 /*
		  * @brief ɾ����Ӱ����ͼ�����
		  * @param [in] layer ��Ӱ����ͼ�����
		  * @return �ɹ�����true��ʧ�ܷ���false 
		  */
		glbBool RemoveTiltPhotographLayer(CGlbGlobeLayer* layer);
		 /*
		  * @brief  ��������֮��ĵ������
		  * @param [in] xOrLonStart, yOrLatStart ��ʼ��x,y��γ��
						xOrLonEnd,yOrLatEnd ��ֹ��x,y��γ��
						step ���㾫��(����)
		  * @return  ����֮��ĵ������
		  */
		glbDouble ComputeOnTerrainDistance(glbDouble xOrLonStart, glbDouble yOrLatStart, glbDouble xOrLonEnd, glbDouble yOrLatEnd, glbDouble step);

		/*
		  * @brief �ж��Ƿ���ҪֹͣTile��Update		  
		  * @return ��Ҫֹͣ����true������Ҫ����false
		  * @note Ŀǰƽ��ģʽ�»�ʹ�ã�����������Add node������������ֵ��5-10��
		  */
		glbBool IsUpdateNeedTerminate();

		//���Խӿ� �������رգ��߿�ģʽ
		void EnableWireMode(glbBool wiremode);
		glbBool IsWireMode();
private:
	void UpdateGlobeTerrain(std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
		std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlayers,
		std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs, 
		std::vector<glbref_ptr<CGlbGlobeRObject>> mTerrainobjs);
	void UpdateFlatTerrain(std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
		std::vector<glbref_ptr<CGlbGlobeDemLayer>> demlayers,
		std::vector<glbref_ptr<CGlbGlobeRObject>> terrainobjs, 
		std::vector<glbref_ptr<CGlbGlobeRObject>> mTerrainobjs);
	void RemoveInvisibleTiles();
	glbInt32 GetRowFromLatitude(glbDouble latitude, glbDouble tileSize);
	glbInt32 GetColFromLongitude(glbDouble longitude, glbDouble tileSize);
	glbBool IsIntersect(CGlbExtent* src_extent, std::vector<CGlbExtent>& target_extents);
	glbBool GetSortedTopmostTiles(std::map<glbInt32,glbref_ptr<CGlbGlobeTerrainTile>> tiles, osg::Vec3d camerapos, std::map<glbDouble,glbref_ptr<CGlbGlobeTerrainTile>>& sorted_tiles);

	// ���(lonOrX, latOrY������б��Ӱͼ�㷶Χ�ڣ�����õ㴦�ĸ߶ȣ��ɹ�����true��ʧ�ܷ���false
	// ���(lonOrX, latOrY��������б��Ӱͼ�㷶Χ�ڣ�����false
	glbBool GetElevationAtFromTiltPhotographLayer(glbDouble lonOrX, glbDouble latOrY, glbDouble &elevtion);
private:	
	glbref_ptr<IGlbTerrainDataset> mpr_tdataset;						///<�������ݼ�
	glbBool	mpr_isInitialized;											///<���γ�ʼ����־
	glbDouble mpr_south;												///<�ϱ߽�
	glbDouble mpr_north;												///<���߽�
	glbDouble mpr_west;													///<���߽�
	glbDouble mpr_east;													///<���߽�
	glbDouble mpr_lzts;													///<0���ֿ鷶Χ	��ģʽ����λ�Ƕ�
	glbBool mpr_enableUpdate;											///<����/ֹͣ ���θ���
	glbBool	mpr_isLinemode;												///<�߿�ģʽ	
	CRITICAL_SECTION mpr_osgcritical;									///<osg�ڵ�ص�������޸�����ȡ�߳� ����
	osg::ref_ptr<osg::Switch> mpr_node;									///<����osg�ڵ�
	std::map<glbInt32,glbref_ptr<CGlbGlobeTerrainTile>> mpr_topmost_tiles;	///<0�����ο鼯��mpr_lztiles , 0��������	
	glbInt32 mpr_opacity;												///<����͸����
	glbDouble mpr_exaggrate;											///<���ο���ϵ��
	glbBool mpr_visible;												///<��������
	glbBool mpr_enablecullface;											///<�����޳���������رա�
	
	CRITICAL_SECTION mpr_tilebuffer_exchange_criticalsection;			///<���ݽ����ٽ���
	std::vector<GlbTileOperation> mpr_operations;						///<��������
	std::vector<GlbTileOperation> mpr_operations_buffer;				///<�������ϻ���		

	std::vector<GlbTileOperation> mpr_packageOperations;				///<��Ҫһ���Դ���Ĳ�������
	std::vector<GlbTileOperation> mpr_packageOperations_buffer;			///<��Ҫһ���Դ���Ĳ������ϻ���

	//glbDouble mpr_levelzero_tilesize_degrees;							///<0���ֿ��С
	glbInt32 mpr_levelzero_tiles_columns;								///<0���ֿ�����
	glbInt32 mpr_levelzero_tiles_rows;									///<0���ֿ�����
	glbBool	mpr_is_camera_underground;									///<��¼����Ƿ��ڵ�����

	std::vector<glbref_ptr<CGlbGlobeLayer>> mpr_tilt_photography_layers;///<��б��Ӱͼ�㼯��
	//osg::Timer_t mpr_prebufferchange_time;							///<��һ��operationbuffer������ʱ��
	glbBool	mpr_bOpacityChanged;										///<����

	glbInt32 mpr_addOperationCount;										///<��Ҫ�������ӽڵ��������
public:
	//CRITICAL_SECTION mpr_critical;									//�������ݼ�/���ݼ���ȡ���ݻ���
	CRITICAL_SECTION mpr_tDatasetAccessCritical;						//���õ������ݼ�/��ȡ�������ݼ����ݻ���

	GlbGlobeTypeEnum mpr_globe_type;									//��������
	CGlbGlobe* mpr_globe;												//globe��������
	};
}