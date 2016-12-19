/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeREObject.h
* @brief   �ɱ༭���ƶ���ͷ�ļ����ĵ�����CGlbGlobeREObject��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-6 15:00
*********************************************************************/
#pragma once
#include "GlbGlobeRObject.h"

namespace GlbGlobe
{
	/**
	* @brief �ɱ༭���ƶ�����				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-6 10:50
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeREObject : public CGlbGlobeRObject
	{
	public:
		/**
		* @brief ���ö��󺣰�ģʽ
		* @param[in]  mode ����ģʽö��
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		virtual glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);
		/**
		* @brief ���ö������ɼ�����
		* @param[in]  distance �������ɼ�����
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		virtual glbBool SetMaxVisibleDistance(glbDouble distance);
		/**
		* @brief ���ö�����С�ɼ�����
		* @param[in]  distance ������С�ɼ�����
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		virtual glbBool SetMinVisibleDistance(glbDouble distance);
		/**
		* @brief ���ö�����Ⱦ���ȼ�
		* @param[in]  order ������Ⱦ���ȼ�
		*/
		virtual void SetRenderOrder(glbInt32 order);
		/**
		* @brief ���ö���װ�����ȼ�
		* @param[in]  order ����װ�����ȼ�
		*/
		virtual void SetLoadOrder(glbInt32 order);
		/**
		* @brief ���ö�����ʾ��Ϣ
		* @param[in]  tooltip ������ʾ��Ϣ
		* @return ���óɹ�����true������ʧ�ܷ���false
		* @note �����ŵ�������ʱ����ʾ����
		*/
		virtual glbBool SetToolTip(/*CGlbWString*/ glbWChar* tooltip);
		/**
		* @brief ���ö�����Ϣ
		* @param[in]  msg ������Ϣ
		* @return ���óɹ�����true������ʧ�ܷ���false
		* @note ���������ʱ����ʾ����
		*/
		virtual glbBool SetRMessage(/*CGlbWString*/ glbWChar* msg);
	};
}