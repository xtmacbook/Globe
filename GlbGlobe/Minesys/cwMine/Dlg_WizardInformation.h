#pragma once
#include "UsefulUtility/DlgMultiExt.h"

class CDlgInput;
class CDlgOutput;
class CDlg_WizardInformation: public CDlgMultiExt
{
public:
	CDlg_WizardInformation(void);
	~CDlg_WizardInformation(void);

	CDlgInput *m_pDlgInput;
	CDlgOutput*m_pDlgOut;
	//÷ÿ‘ÿ∫Ø ˝
protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
