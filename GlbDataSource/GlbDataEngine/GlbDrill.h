#pragma once
#include "GlbFeature.h"

/**
  * @brief ��׶���
  * @version 1.0
  * @date    2014-11-09
  * @author  ������
  */
class GLBDATAENGINE_API CGlbDrill :	public CGlbFeature
{
public:
	CGlbDrill(IGlbFeatureClass* cls,CGlbFields* fields);
	~CGlbDrill(void);

public:
	/**
	  * @brief             ��ѯ�ز�
	  *
	  * @param queryFilter ��ѯ������
	  * @return            �α�,ʧ��NULL
	*/
	const IGlbFeatureCursor* QueryStratum(CGlbQueryFilter* queryFilter);
	/**
	  * @brief             ��ѯ��б
	  *
	  * @param queryFilter ��ѯ������
	  * @return            �α�,ʧ��NULL
	*/
	const IGlbObjectCursor* QuerySkew(CGlbQueryFilter* queryFilter);
	/**
	  * @brief             ��ѯ��Ʒ
	  *
	  * @param queryFilter ��ѯ������
	  * @return            �α�,ʧ��NULL
	*/
	const IGlbObjectCursor* QuerySample(CGlbQueryFilter* queryFilter, const glbWChar* spDatasetName);
};

