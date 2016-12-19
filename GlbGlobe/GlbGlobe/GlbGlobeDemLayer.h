/********************************************************************
  * Copyright (c) 2014 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbGlobeDemLayer.h
  * @brief   Demͼ��ͷ�ļ�
  *
  * �����������CGlbGlobeDemLayer���class
  *
  * @version 1.0
  * @author  ����
  * @date    2014-5-7 17:40
*********************************************************************/
#pragma once

#include "GlbGlobeExport.h"
#include "GlbGlobeLayer.h"
namespace GlbGlobe
{

class GLB_DLLCLASS_EXPORT CGlbGlobeDemLayer : public CGlbGlobeLayer
{
//public:
//	enum GlbGlobeDemType
//	{
//		GLBDEMTYPE_OVERLAY = 0,		//�滻ģʽ
//		GLBDEMTYPE_ACCUMULATION = 1	//�ۼ�ģʽ
//	};
public:
	 /**
	  * @brief demͼ�㹹�캯��
	  * @return ��
	  */
	CGlbGlobeDemLayer(void);
	 /**
	  * @brief Ҫ��ͼ����������
	  * @return ��
	  */
	~CGlbGlobeDemLayer(void);

	/*����ӿ�*/
	 /**
	  * @brief ��ȡͼ������
	  * @return ͼ������ 
	  */
	const glbWChar* GetName();
	 /**
	  * @brief ����ͼ������
	  * @param [in] name ͼ������
	  * @return  �ɹ�����true
			-	 ʧ�ܷ���false
	  */
	glbBool SetName(glbWChar* name);
	 /**
	  * @brief ��ȡͼ������
	  * @return  ͼ������
	  */
	GlbGlobeLayerTypeEnum GetType();
	 /**
	  * @brief ͼ�����л������أ�
	  * @param [in] node xml��ȡ�ڵ�
	  * @param [in] relativepath ���·��
	  * @return  �ɹ�����true
			-	 ʧ�ܷ���false
	  */
	glbBool   Load(xmlNodePtr node, glbWChar* relativepath );
	glbBool   Load2(xmlNodePtr node, glbWChar* relativepath );
	 /**
	  * @brief ͼ�����л������棩
	  * @param [in] node xmlд��ڵ�
	  * @param [in] relativepath ���·��
	  * @return  �ɹ�����true
			-	 ʧ�ܷ���false
	  */
	glbBool   Save(xmlNodePtr node, glbWChar* relativepath );
	 /**
	  * @brief ����ͼ����ʾ������
	  * @param [in] show ����  ��ʾΪtrue������Ϊfalse
	  * @param [in] isOnState true��ʾ�ڿɼ�����  false��ʾ���ڿɼ�����
	  * @return  �ɹ�����true
			-	 ʧ�ܷ���false
	  */
	glbBool Show(glbBool show,glbBool isOnState=false);
	 /**
	  * @brief ����ͼ�㲻͸����
	  * @param [in] opacity ��͸����(0-100)
					-	0��ʾȫ͸��
					-	100��ʾ��͸��
	  * @return  �ɹ�����true
			-	 ʧ�ܷ���false
	  */
	glbBool SetOpacity(glbInt32 opacity);
	 /**
	  * @brief ��ȡͼ�㷶Χ
	  * @param [in] isWorld �Ƿ����������꣬������������꣬����Ϊ��λ;��������������꣬��ʹ�õ������꣬�Զ�Ϊ��λ				
	  * @return  ͼ�㷶Χ
	  */
	glbref_ptr<CGlbExtent> GetBound(glbBool isWorld);

	/*��ӿ�*/
	 /**
	  * @brief ��ȡ�Ƿ��н�����			
	  * @return  true��ʾ�н�������false��ʾû�н�����
	  */
	glbBool HasPyramid();
	 /**
	  * @brief ���ɽ�����
	  * @param [in]	progress ������Ϣ
	  * @param [in] lzts �ֿ��С
	  * @param [in] isGlobe �Ƿ�����ģʽ true��ʾ����ģʽ��false��ʾƽ��ģʽ
	  * @return  �ɹ�����true��ʧ�ܷ���false.
	  */
	glbBool BuildPyramid(IGlbProgress * progress,glbDouble lzts , glbBool isGlobe);
	 /**
	  * @brief ��ȡ������������С�ȼ�
	  * @param [out] minLevel ����������С�ȼ�
	  * @param [out] maxLevel �����������ȼ�	
	  * @return  �ɹ�����true��
				 û�н�������ʧ�ܷ���false.
	  */
	glbBool GetPyramidLevels(glbInt32 *minLevel, glbInt32 *maxLevel);
	 /**
	  * @brief �������ݿ�
	  * @param [in][out] pixelblock ���ݿ�ָ��				
	  * @return   �ɹ�����true��ʧ�ܷ���false.
	  */
	glbBool CreatePixelBlock(CGlbPixelBlock ** pixelblock );
	 /**
	  * @brief ��ȡ���ݿ�
	  * @param [in] level �ȼ�
	  * @param [in] tilerow ����к�
	  * @param [in] tilecolumn ����к�
	  * @param [out] p_pixelblock ��ȡ���ݺ�����ݿ�
	  * @return   �ɹ�����true��ʧ�ܷ���false.
	  */
	glbBool ReadData(glbInt32 level,glbInt32 tilerow,glbInt32 tilecolumn,CGlbPixelBlock* p_pixelblock);
	 /**
	  * @brief ��ȡ���ݵ����ݿ�
	  * @param [in] p_extent ���ݷ�Χ
	  * @param [out] p_pixelblock ��ȡ���ݺ�����ݿ�
	  * @return   �ɹ�����true��ʧ�ܷ���false.
	  */
	glbBool ReadDataEx(CGlbExtent* p_extent, CGlbPixelBlock* p_pixelblock);
	 /**
	  * @brief ��ȡ�Ƿ����ۼ�ģʽ			
	  * @return  true��ʾ���ۼ�ģʽ��false��ʾ���滻ģʽ
	  */	
	glbBool IsAccumulationMode();
	 /**
	  * @brief �����Ƿ����ۼ�ģʽ			
	  * @param  isAccumulation true��ʾ���ۼ�ģʽ��false��ʾ���滻ģʽ
	  */	
	void	SetAccumulationMode(glbBool isAccumulation);
private:
	glbBool mpr_isAccumulationMode; // �Ƿ��Ǹ߳��ۼ�ģʽ
};
}


