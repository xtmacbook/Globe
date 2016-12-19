/********************************************************************
  * Copyright (c) 2014 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbGlobeTiltPhotographyLayer.h
  * @brief   ��б��Ӱ����ͼ��ͷ�ļ�
  *
  * �����������CGlbGlobeTiltPhotographyLayer���class
  *
  * @version 1.0
  * @author  ����
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
		// ����Ϣ�ṹ��
		struct GlbTileInfo
		{	// ����
			std::string name;
			//  ��node
			osg::ref_ptr<osg::Node>  node;
			// ��Χ
			double xmin;
			double xmax;
			double ymin;
			double ymax;
			double zmin;
			double zmax;
			// �ӿ���
			int	childCnt;
			// ��������
			osg::ref_ptr<osg::Texture>  texture;
			unsigned char* orignData; // ԭʼ������ 
		}_GlbTileInfo;

	public:
		/*
		* @brief ��б��Ӱ����ͼ�㹹�캯��
		* @param [in] tpFile creatarGlobe�������б��Ӱ���������ļ�*.tpf
		* @return ��
		*/
		CGlbGlobeTiltPhotographyLayer(CGlbWString tpfFile,glbBool isGlobe=true);
		/*
		* @brief ��б��Ӱ����ͼ����������
		* @return ��
		*/
		~CGlbGlobeTiltPhotographyLayer(void);
		/*
		* @brief ��ȡ�����ͼ��ľ���
		* @param[in]  cameraPos ����ڳ����е�λ��
		* @return �����ͼ��ľ���
		*/
		glbDouble GetDistance( osg::Vec3d &cameraPos);
		/* ����ӿ� */
		 /*
		  * @brief ��ȡͼ������
		  * @return ͼ������ 
		  */
		const glbWChar* GetName();
		 /*
		  * @brief ����ͼ������
		  * @param name ͼ������
		  * @return  �ɹ�����true
				-	 ʧ�ܷ���false
		  */
		glbBool SetName(glbWChar* name);
		 /*
		  * @brief ��ȡͼ������
		  * @return  ͼ������
		  */
		GlbGlobeLayerTypeEnum GetType();

		 /*
		  * @brief ͼ�����л������أ�
		  * @param [in] node xml��ȡ�ڵ�
		  * @param [in] relativepath ���·��
		  * @return  �ɹ�����true
				-	 ʧ�ܷ���false
		  */
		glbBool   Load(xmlNodePtr node, glbWChar* relativepath );
		 /*
		  * @brief ͼ�����л������棩
		  * @param [in] node xmlд��ڵ�
		  * @param [in] relativepath ���·��
		  * @return  �ɹ�����true
				-	 ʧ�ܷ���false
		  */
		glbBool   Save(xmlNodePtr node, glbWChar* relativepath );
		/*
		* @brief ����ͼ����ʾ������
		* @param [in] show ����  ��ʾΪtrue������Ϊfalse
		* @param [in] isOnState true��ʾ�ڿɼ�����  false��ʾ���ڿɼ�����
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Show(glbBool show,glbBool isOnState=false);
		 /*
		  * @brief ����ͼ�㲻͸����
		  * @param [in] opacity ��͸����(0-100)
						-	0��ʾȫ͸��
						-	100��ʾ��͸��
		  * @return  �ɹ�����true
				-	 ʧ�ܷ���false
		  */
		glbBool SetOpacity(glbInt32 opacity);
		/*
		* @brief ��ȡͼ�㷶Χ
		* @param [in] isWorld �Ƿ����������꣬������������꣬����Ϊ��λ;��������������꣬��ʹ�õ������꣬�Զ�Ϊ��λ			
		* @return  ͼ�㷶Χ
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld);

		 /*
		  * @brief ��ȡͼ���Ӧ��tpf�ļ�ȫ·��
		  * @return tpf�ļ�ȫ·�� 
		  */
		const glbWChar* GetTPFFilePath();
		 /*
		  * @brief ����ͼ��������ڶ�����
		  * @param [in] dighole �ڶ�����
		  */		
		void SetAssociateObject(CGlbGlobeObject* dighole);
		/*
		* @brief ��ȡͼ��������ڶ�����
		* @return ��
		*/
		CGlbGlobeObject* GetAssociateObject();
		/*
		* @brief ͼ�����
		* @param [in] domlayers դ��ͼ�㼯��
		* @param [in] objs		���ػ��ƺ��ڶ����󼯺�
		* @return ��
		*/	
		void Update(	std::vector<glbref_ptr<CGlbGlobeDomLayer>> domlayers,
						std::vector<glbref_ptr<CGlbGlobeRObject>> objs	);

		/*
		* @brief ˢ��ͼ�������ڵ�����
		* @param [in] objDirtyExts ��Ҫˢ�µ������ζ�������
		* @param [in] domDirtyExts ��Ҫˢ�µ�domͼ������
		* @param [in] domlayers		domͼ�㼯��
		* @param [in] terrainObjs	�����λ��ƵĶ��󼯺�
		* @return ��
		*/		
		void RefreshTexture(std::vector<CGlbExtent>                    &objDirtyExts,
							std::vector<CGlbExtent>                    &domDirtyExts,
							std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers, 	
							std::vector<glbref_ptr<CGlbGlobeRObject>>  &objs	);
	private:
		// ����tpf�ļ�������ͼ�㶥����
		glbBool loadTPFfile(CGlbWString tpfFile);
	private:
		CGlbWString			mpr_tpfFilePath;					///< tpf�ļ�·��
		glbBool				mpr_isGlobe;						///< �Ƿ������淽ʽ			
		osg::Vec3d			mpr_anchor;							///< ê��
		CGlbExtent			mpr_bound;							///< ���
		CGlbString			mpr_DataPath;						///< ����·��
		GlbCriticalSection	mpr_critical;						///< �ٽ���
		std::map<std::string, GlbTileInfo> _tilesRenderingMap;	///< ������Ⱦ��tile��map����
		std::map<std::string, GlbTileInfo> _topmostTilesMap;	///< ����tile��map����
		glbref_ptr<CGlbGlobeObject>		mpr_digHoleObj;			///< ��֮�������ڶ�����
	};
}


