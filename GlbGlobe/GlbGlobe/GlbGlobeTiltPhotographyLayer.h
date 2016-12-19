/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbGlobeTiltPhotographyLayer.h
  * @brief   倾斜摄影数据图层头文件
  *
  * 这个档案定义CGlbGlobeTiltPhotographyLayer这个class
  *
  * @version 1.0
  * @author  马林
  * @date    2015-8-4 10:40
*********************************************************************/
#pragma once 

#include "GlbGlobeExport.h"
#include "GlbGlobeDomLayer.h"
#include <osg/Texture2D>
#include "GlbGlobeRObject.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CGlbGlobeTiltPhotographyLayer : public CGlbGlobeLayer
	{
	private:
		// 块信息结构体
		struct GlbTileInfo
		{	// 名称
			std::string name;
			//  块node
			osg::ref_ptr<osg::Node>  node;
			// 范围
			double xmin;
			double xmax;
			double ymin;
			double ymax;
			double zmin;
			double zmax;
			// 子块数
			int	childCnt;
			// 纹理数据
			osg::ref_ptr<osg::Texture>  texture;
			unsigned char* orignData; // 原始的纹理 
		}_GlbTileInfo;

	public:
		/*
		* @brief 倾斜摄影数据图层构造函数
		* @param [in] tpFile creatarGlobe定义的倾斜摄影数据索引文件*.tpf
		* @return 无
		*/
		CGlbGlobeTiltPhotographyLayer(CGlbWString tpfFile,glbBool isGlobe=true);
		/*
		* @brief 倾斜摄影数据图层析构函数
		* @return 无
		*/
		~CGlbGlobeTiltPhotographyLayer(void);
		/*
		* @brief 获取相机与图层的距离
		* @param[in]  cameraPos 相机在场景中的位置
		* @return 相机与图层的距离
		*/
		glbDouble GetDistance( osg::Vec3d &cameraPos);
		/* 基类接口 */
		 /*
		  * @brief 获取图层名称
		  * @return 图层名称 
		  */
		const glbWChar* GetName();
		 /*
		  * @brief 设置图层名称
		  * @param name 图层名称
		  * @return  成功返回true
				-	 失败返回false
		  */
		glbBool SetName(glbWChar* name);
		 /*
		  * @brief 获取图层类型
		  * @return  图层类型
		  */
		GlbGlobeLayerTypeEnum GetType();

		 /*
		  * @brief 图层序列化（加载）
		  * @param [in] node xml读取节点
		  * @param [in] relativepath 相对路径
		  * @return  成功返回true
				-	 失败返回false
		  */
		glbBool   Load(xmlNodePtr node, glbWChar* relativepath );
		 /*
		  * @brief 图层序列化（保存）
		  * @param [in] node xml写入节点
		  * @param [in] relativepath 相对路径
		  * @return  成功返回true
				-	 失败返回false
		  */
		glbBool   Save(xmlNodePtr node, glbWChar* relativepath );
		/*
		* @brief 设置图层显示或隐藏
		* @param [in] show 显隐  显示为true，隐藏为false
		* @param [in] isOnState true表示在可见区域  false表示不在可见区域
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Show(glbBool show,glbBool isOnState=false);
		 /*
		  * @brief 设置图层不透明度
		  * @param [in] opacity 不透明度(0-100)
						-	0表示全透明
						-	100表示不透明
		  * @return  成功返回true
				-	 失败返回false
		  */
		glbBool SetOpacity(glbInt32 opacity);
		/*
		* @brief 获取图层范围
		* @param [in] isWorld 是否是世界坐标，如果是世界坐标，以米为单位;如果不是世界坐标，则使用地理坐标，以度为单位			
		* @return  图层范围
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld);

		 /*
		  * @brief 获取图层对应的tpf文件全路径
		  * @return tpf文件全路径 
		  */
		const glbWChar* GetTPFFilePath();
		 /*
		  * @brief 设置图层关联的挖洞对象
		  * @param [in] dighole 挖洞对象
		  */		
		void SetAssociateObject(CGlbGlobeObject* dighole);
		/*
		* @brief 获取图层关联的挖洞对象
		* @return 无
		*/
		CGlbGlobeObject* GetAssociateObject();
		/*
		* @brief 图层更新
		* @param [in] domlayers 栅格图层集合
		* @param [in] objs		贴地绘制和挖洞对象集合
		* @return 无
		*/	
		void Update(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
						std::vector<glbref_ptr<CGlbGlobeRObject>> objs	);

		/*
		* @brief 刷新图层区域内的纹理
		* @param [in] objDirtyExts 需要刷新的贴地形对象区域
		* @param [in] domDirtyExts 需要刷新的dom图层区域
		* @param [in] domlayers		dom图层集合
		* @param [in] terrainObjs	贴地形绘制的对象集合
		* @return 无
		*/		
		void RefreshTexture(std::vector<CGlbExtent>                    &objDirtyExts,
							std::vector<CGlbExtent>                    &domDirtyExts,
							std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers, 	
							std::vector<glbref_ptr<CGlbGlobeRObject>>  &objs	);
	private:
		// 加载tpf文件，生成图层顶级块
		glbBool loadTPFfile(CGlbWString tpfFile);
	private:
		CGlbWString			mpr_tpfFilePath;					///< tpf文件路径
		glbBool				mpr_isGlobe;						///< 是否是球面方式			
		osg::Vec3d			mpr_anchor;							///< 锚点
		CGlbExtent			mpr_bound;							///< 外包
		CGlbString			mpr_DataPath;						///< 数据路径
		GlbCriticalSection	mpr_critical;						///< 临界区
		std::map<std::string, GlbTileInfo> _tilesRenderingMap;	///< 现在渲染的tile的map数组
		std::map<std::string, GlbTileInfo> _topmostTilesMap;	///< 顶级tile的map数组
		glbref_ptr<CGlbGlobeObject>		mpr_digHoleObj;			///< 与之关联的挖洞对象
	};
}


