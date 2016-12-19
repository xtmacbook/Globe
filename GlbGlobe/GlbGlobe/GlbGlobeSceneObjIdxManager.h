/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeSceneObjIdxManager.h
* @brief   Globe������������������ͷ�ļ�
*
* �����������CGlbGlobeSceneObjIdxManager���class,�����˲������Ĳ����������ƣ��ֱ���������ռ����������λ��ƵĶ���
*	����ҵ��˵����1. ����objects����������������ɢ�˲���������+����ɢ�Ĳ��������飬�˲���
*					���������ռ����λ�û��Ƶĳ����ɻ��ƶ���RenderObject���Ĳ����������
*					������ģʽ���Ƶĳ����ɻ��ƶ���RenderObject��
*				  2. ���ٵĶ���ѡ����ơ�֧�ֵ�ѡ�Ϳ�ѡ
* @version 1.0
* @author  ����
* @date    2014-5-13 11:20
*********************************************************************/
#pragma once
#include <osg/Polytope>
#include "GlbExtent.h"
#include "glbref_ptr.h"
#include "GlbGlobeRObject.h"

namespace GlbGlobe
{
	class CGlbAxisAlignedBox;
	class CGlbGlobeOctree;
	class CGlbGlobeQtree;
	/**
	* @brief Globe������������������			 
	* @version 1.0
	* @author  ����
	* @date    2014-5-13 11:20
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeSceneObjIdxManager : public CGlbReference
	{
	public:
		/**
		* @brief ���캯��
		* @param [in] name ����������������������
		* @param [in] max_depth �������ȣ������㼶�� ��Ĭ��Ϊ20����
		* @param [in] bound ��ʼ��Χ(ƽ�泡��ģʽ����Ҫ����)
		* @return  ��
		*/
		CGlbGlobeSceneObjIdxManager(const WCHAR* name=NULL, int max_depth=20, CGlbExtent* bound=NULL);
		/**
		* @brief ��������
		* @return  ��
		*/
		~CGlbGlobeSceneObjIdxManager(void);
	public:
		/**
		* @brief �ӳ�������������Ӷ���
		* @param [in] obj �ɻ��ƶ��� 
		* @return  ��
		*/
		void AddObject(CGlbGlobeRObject* obj);
		/**
		* @brief �ӳ�����������ɾ������
		* @param [in] obj �ɻ��ƶ��� 
		* @return  ��
		*/
		void RemoveObject(CGlbGlobeRObject* obj);	
		/**
		* @brief ���³����������ж���
		* @param [in] obj �ɻ��ƶ��� 
		* @return  ��
		*/
		void UpdateObject(CGlbGlobeRObject* obj); 
		/**
		* @brief ����ѡ��-����������ƶ���
		* @param [in] ln_pt1,ln_pt2 ���ߵ����pt1���������췽���ϵ�һ��pt2 
		* @param [out] result ѡ�еĶ��󼯺�,������pt1���ɽ���Զ����
		* @param [out] inter_pts ���㼯�ϣ�������pt1���ɽ���Զ���� �� һ������ȡһ������
		* @return  ��ѡ�ж��󷵻�true��û��ѡ�ж��󷵻�false
		*/
		bool Query(osg::Vec3d ln_pt1, osg::Vec3d ln_pt2, std::vector<glbref_ptr<CGlbGlobeObject>>& result, std::vector<osg::Vec3d>& inter_pts,bool query_crudeAndquickly=false/*��ѡ*/);
		/**
		* @brief ����ѡ��-��������ƶ���
		* @param [in] on_terrain_point ���ߵ������Ľ��㣨�������� �� ���ȣ�γ�ȣ� 
		* @param [out] result ѡ�е���������ƶ��󼯺�
		* @param [in] nearest_distance ���뷧ֵ ����on_terrain_point�þ���С�ڴ˷�ֵ�ı�ѡ��
		* @param [in] globeTye ��������
		* @return  ��ѡ�ж��󷵻�true��û��ѡ�ж��󷵻�false
		*/
		bool QueryOnTerrainObjects(osg::Vec3d on_terrain_point, std::vector<glbref_ptr<CGlbGlobeObject>>& result, double nearest_distance=0.1, GlbGlobeTypeEnum globeTye=GLB_GLOBETYPE_GLOBE);

		/**
		* @brief ��׶ѡ��-����������ƶ���
		* @param [in] polytope ��׶�� 
		* @param [out] result ѡ�еĶ��󼯺�
		* @param [in] query_crudeAndquickly �Ƿ�ʹ�ÿ��ٴ�ѡ��ʽ true��ֻ�ö������������׶���󽻣�false��ϸ��
		* @return ��ѡ�ж��󷵻�true��û��ѡ�ж��󷵻�false
		*/
		// ��׶ѡ��-������������
		bool Query(osg::Polytope& polytope,std::vector<glbref_ptr<CGlbGlobeObject>>& result,bool query_crudeAndquickly=false);

		/**
		* @brief ��������ѡ��-��������ƶ���
		* @param [in] t_extent ѡ�����򣨵������� �����ȣ�γ�ȣ� 
		* @param [out] result ѡ�е���������ƶ��󼯺�
		* @return  ��ѡ�ж��󷵻�true��û��ѡ�ж��󷵻�false
		*/
		bool QueryOnTerrainObjects(CGlbExtent* t_extent,std::vector<glbref_ptr<CGlbGlobeObject>>& result);

		/**
		* @brief ��������ѡ��-����������ƶ���
		* @param [in] t_extent ѡ�������������꣩ 
		* @param [out] result ѡ�еķ���������ƶ��󼯺�
		* @return  ��ѡ�ж��󷵻�true��û��ѡ�ж��󷵻�false
		*/
		bool QueryObjects(CGlbExtent* t_extent,std::vector<glbref_ptr<CGlbGlobeObject>>& result);

		/**
		* @brief ��ȡ�������������λ��ƶ��������
		* @return  �����λ��ƶ��������
		*/	
		long GetOnTerrainObjectCount();

	private:
		// ���ݾ�γ�ȼ����octree��key, latitude,longitude : ��λ:�� 
		long GetKeyFromPosition(long latitude, long longitude);		
		CGlbAxisAlignedBox GetRootOctreeBoundBoxFromPosition(long col, long row, BOOL is_on_terrain=FALSE);
		void _addOctreeObject(CGlbGlobeObject* n,CGlbGlobeOctree* octant,long depth=0);
		void _removeObject(CGlbGlobeObject * n ,  BOOL is_on_terrain = FALSE);		
		void _addQtreeObject(CGlbGlobeObject* n,CGlbGlobeQtree* qtant,long depth=0);
		//void _removeQtreeObject(CGlbGlobeObject * n );
		bool _isObjectHaveSelected(CGlbGlobeObject* obj, std::vector<glbref_ptr<CGlbGlobeObject>>& result);
		// ȥ��result�е��ظ�����ֻ����һ��. ����������������仯�����ظ�����ʱ����true�� û�б仯ʱ����false
		bool _makeobjectsUnique(std::vector<glbref_ptr<CGlbGlobeObject>>& result);
	private:
		CGlbGlobeOctree* mpr_globe_top_octree;
		std::map<long,CGlbGlobeOctree*> mpr_map_octrees;
		long mpr_max_depth;
		std::wstring mpr_name;
		CGlbGlobeQtree* mpr_globe_top_qtree;
		std::map<long,CGlbGlobeQtree*> mpr_map_qtrees;
		CGlbExtent*		mpr_p_extent;
		bool	mpr_b_globe_type;
		CRITICAL_SECTION	mpr_objects_criticalsection;//��������ٽ���
		long					mpr_onterrain_objects_count;//�����λ��Ƶ��������
	};
}


