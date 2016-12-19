/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbCompareValue.h
* @brief   ��ֵ�Ƚ��࣬�ĵ�����CGlbCompareValue��
* @version 1.0
* @author  ����
* @date    2014-8-14 13:30
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbFeature.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbCompareValue��				 
	* @version 1.0
	* @author  ����
	* @date    2014-8-14 13:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbCompareValue
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbCompareValue(void);
		/**
		* @brief ��������
		*/
		~CGlbCompareValue(void);
	public:
		/**
		* @brief �ж�����glbDouble��ֵ�Ƿ����
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareDoubleEqual(glbDouble leftValue,glbDouble rightValue);
		/**
		* @brief �ж�����GlbRenderDouble��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueDoubleEqual(GlbRenderDouble *leftValue,GlbRenderDouble *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderInt32��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueIntEqual(GlbRenderInt32 *leftValue,GlbRenderInt32 *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderBool��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueBoolEqual(GlbRenderBool *leftValue,GlbRenderBool *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderColor��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueColorEqual(GlbRenderColor *leftValue,GlbRenderColor *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderString��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueStringEqual(GlbRenderString *leftValue,GlbRenderString *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderTexRepeatMode��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueTexRepeatEnumEqual(GlbRenderTexRepeatMode *leftValue,GlbRenderTexRepeatMode *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderLinePattern��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueLinePatternEnumEqual(GlbRenderLinePattern *leftValue,GlbRenderLinePattern *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderMultilineAlign��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueMultilineAlignEnumEqual(GlbRenderMultilineAlign *leftValue,GlbRenderMultilineAlign *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderLabelRelation��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueLabelRelationEnumEqual(GlbRenderLabelRelation *leftValue,GlbRenderLabelRelation *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderLabelAlign��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueLabelAlignEnumEqual(GlbRenderLabelAlign *leftValue,GlbRenderLabelAlign *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderImageAlign��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueImageAlignEnumEqual(GlbRenderImageAlign *leftValue,GlbRenderImageAlign *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderBillboard��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueBillboardEnumEqual(GlbRenderBillboard *leftValue,GlbRenderBillboard *rightValue,CGlbFeature *feature);
		/**
		* @brief �ж�����GlbRenderArrowPattern��ֵ�Ƿ����޸�
		* @return  ��ֵ��ֵ��ͬ����true�����򷵻�false
		*/
		static glbBool CompareValueArrowPatternEnumEqual(GlbRenderArrowPattern *leftValue,GlbRenderArrowPattern *rightValue,CGlbFeature *feature);

		static glbBool CompareValueDynamicLabelFadeEnumEqual(GlbRenderDynamicLabelFadeMode *leftValue,GlbRenderDynamicLabelFadeMode *rightValue,CGlbFeature *feature);

		static glbBool CompareValueDynamicLabelFadeSpeedEnumEqual(GlbRenderDynamicLabelFadeSpeed *leftValue,GlbRenderDynamicLabelFadeSpeed *rightValue,CGlbFeature *feature);
	};
}
