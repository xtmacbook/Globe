// ExcelMgr.h: interface for the ExcelMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCELMGR_H__B2A3E58A_44F2_4070_BE2A_E0C3D6B73F34__INCLUDED_)
#define AFX_EXCELMGR_H__B2A3E58A_44F2_4070_BE2A_E0C3D6B73F34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "excel9.h"


class AFX_CLASS_EXPORT ExcelMgr
{
public:
	ExcelMgr();
	virtual ~ExcelMgr();
public:
	//��ӹ����� countΪ��ӹ������ĸ���
	bool AddSheet(int count=1);
	//����Excel�ļ�
	void CreateExcel(CString path);

	//��ָ����Excel�ļ�
	bool Open(CString path);

	//�����޸� ------ Ĭ��ʵ�����������е��� Ҳ�����ֶ�����
	void Save();

	//�õ�����������
	int GetSheetsCount();

	//���õ�ǰ��Ĺ����� ��1��ʼ����
	bool SetCurSheet(int index);

	//�õ���ǰ�����������
	CString GetSheetName();

	//���õ�ǰ�����������
	void SetSheetName(CString name);

	//�ĵ�ָ��λ�õĵ�Ԫ������ ��Ԫ��λ����A1 B2 C5��ʾ��
	CString GetText(CString pos);

	//����ָ����Ԫ������
	void SetText(CString pos,CString text);
	void SetItem(CString pos, CString pos2, CString text);

	//�ϲ���Ԫ������ Pos1��ʾ�ϲ���������Ͻǵ�Ԫ��λ�ã�Pos2��ʾ�ϲ����½ǵ�Ԫ��λ��
	void Merge(CString Pos1,CString Pos2);
	
private:
	void RemoveDot(CString &str);
	bool Init();
	void Close();
private:
	_Application app;    
	Workbooks books;
	_Workbook book;
	Sheets sheets;
	_Worksheet sheet;
	_Worksheet Cursheet;
	Range range;
	Range iCell;
	LPDISPATCH lpDisp;    
	COleVariant vResult;
};

#endif // !defined(AFX_EXCELMGR_H__B2A3E58A_44F2_4070_BE2A_E0C3D6B73F34__INCLUDED_)
