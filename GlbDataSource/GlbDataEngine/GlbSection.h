#pragma once
#include "GlbFeature.h"

/**
  * @brief �������
  * @version 1.0
  * @date    2014-11-09
  * @author  ������
  */
class GLBDATAENGINE_API CGlbSection : public CGlbFeature
{
public:
	CGlbSection(IGlbFeatureClass* cls,CGlbFields* fields);
	~CGlbSection(void);

public:
	/**
	  * @brief ��ѯ����Ŀ��Ƶ�
	  *
	  * @param queryFilter ������
	  * @return            �α�,ʧ��:NULL
	*/
	const IGlbObjectCursor* QueryCtrlPt(CGlbQueryFilter* queryFilter);
	/**
	  * @brief ��ѯ����Ľڵ�
	  *
	  * @param queryFilter ������
	  * @return            �α�,ʧ��:NULL
	*/
	const IGlbFeatureCursor* QueryNode(CGlbQueryFilter* queryFilter);
	/**
	  * @brief ��ѯ����Ļ���
	  *
	  * @param queryFilter ������
	  * @return            �α�,ʧ��:NULL
	*/
	const IGlbFeatureCursor* QueryArc(CGlbQueryFilter* queryFilter);
	/**
	  * @brief ��ѯ����Ķ����
	  *
	  * @param queryFilter ������
	  * @return            �α�,ʧ��:NULL
	*/
	const IGlbFeatureCursor* QueryPolygon(CGlbQueryFilter* queryFilter);
};