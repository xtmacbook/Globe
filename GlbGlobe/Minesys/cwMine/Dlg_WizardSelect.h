#pragma once
#include "UsefulUtility/DlgMultiExt.h"

class CDlgInput;
class CDlgOutput;
class CDlg_WizardSelect: public CDlgMultiExt
{
public:
	CDlg_WizardSelect(void);
	~CDlg_WizardSelect(void);

	CDlgInput *m_pDlgInput;
	CDlgOutput*m_pDlgOut;
	bool              m_isDataStatistics;
	//÷ÿ‘ÿ∫Ø ˝
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
