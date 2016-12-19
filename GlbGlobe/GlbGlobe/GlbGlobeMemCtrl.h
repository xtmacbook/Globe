#pragma once
#include "GlbGlobeExport.h"
class GLB_DLLCLASS_EXPORT CGlbGlobeMemCtrl
{
public:
	/*
	*    ���� ReadData�����У�����osgNode �ɹ���
	*    �ȵ��ø÷�����
	*    �������false�������osgNode.
	*/
	glbBool  IncrementUsedMem(glbInt64 msize);
	void     DecrementUsedMem(glbInt64 msize);
	glbInt64 GetUsedMem();
	static CGlbGlobeMemCtrl* GetInstance();

	// ���������������
	void	 IncrementTimeToCompileCost(glbDouble t);
	glbDouble GetTimeToCompileCost();
	void	 ClearTimeToCompileCost();
	void     ClearUsedMem();
protected:
	CGlbGlobeMemCtrl(void);
	~CGlbGlobeMemCtrl(void);
private:
	GlbCriticalSection      mpr_critical;
	glbInt64                mpr_hasUsedMem;
	static CGlbGlobeMemCtrl mpr_instance;
	glbDouble				mpr_timeToCompileCost;
};

