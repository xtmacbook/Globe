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
	//添加工作簿 count为添加工作簿的个数
	bool AddSheet(int count=1);
	//创建Excel文件
	void CreateExcel(CString path);

	//打开指定的Excel文件
	bool Open(CString path);

	//保存修改 ------ 默认实在析构函数中调用 也可以手动调用
	void Save();

	//得到工作簿个数
	int GetSheetsCount();

	//设置当前活动的工作簿 从1开始计数
	bool SetCurSheet(int index);

	//得到当前活动工作簿名称
	CString GetSheetName();

	//设置当前活动工作簿名称
	void SetSheetName(CString name);

	//的到指定位置的单元格内容 单元格位置用A1 B2 C5表示法
	CString GetText(CString pos);

	//设置指定单元格内容
	void SetText(CString pos,CString text);
	void SetItem(CString pos, CString pos2, CString text);

	//合并单元格区域 Pos1表示合并区域的左上角单元格位置，Pos2表示合并右下角单元格位置
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
