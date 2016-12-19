/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    IGlbGlobeTask.h
* @brief   �������� ͷ�ļ�
*
* �����������IGlbGlobeTask���class ,������
*
* @version 1.0
* @author  ����
* @date    2014-5-12 14:18
*********************************************************************/

#pragma once

#include "GlbReference.h"
//#include "GlbGlobeTypes.h"

namespace GlbGlobe
{
	class CGlbGlobeObject;
	class GLB_DLLCLASS_EXPORT IGlbGlobeTask : public CGlbReference
	{
	public:
		IGlbGlobeTask(void);
		~IGlbGlobeTask(void);

		/**
		* @brief ִ������
		* @note ����ӿڣ���Ҫ��������ʵ��
		* @return �ɹ�����true
		-	ʧ�ܷ���false
		*/
		virtual glbBool doRequest()=0;
		/**
		* @brief ��ȡ������صĶ���
		* @note ����ӿڣ���Ҫ��������ʵ��
		* @return �ɹ����ض���ָ��
		-	ʧ�ܷ���NULL
		*/
		virtual CGlbGlobeObject* getObject()=0;
		/**
		* @brief �����Ƿ�����ִ�У������Ƿ����ڼ����У�
		* @return �Ƿ���true
		-	�񷵻�false
		*/
		glbBool isDoing();
		/**
		* @brief �����Ƿ�ִ����ϣ������Ƿ������ϣ�
		* @return �Ƿ���true
		-	�񷵻�false
		*/
		glbBool isFinished();

	protected:
		glbBool mpr_isDoing;
		glbBool mpr_isFinished;
	};

}


