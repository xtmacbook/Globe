// ExcelMgr.cpp: implementation of the ExcelMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExcelMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ExcelMgr::ExcelMgr()
{
	Init();
}

ExcelMgr::~ExcelMgr()
{
	this->Save();

	books.Close();
}

bool ExcelMgr::Init()
{
	if(!app.CreateDispatch(L"Excel.Application")) 
	{
		AfxMessageBox(L"无法启动Excel服务器!");
		return false;
	}
	return true;
}

void ExcelMgr::CreateExcel(CString path)
{
	DeleteFile(path);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	books=app.GetWorkbooks();
	book=books.Add(covOptional);
	book.SaveAs(_variant_t(path),vtMissing,vtMissing,vtMissing,vtMissing,
                    vtMissing,0,vtMissing,vtMissing,vtMissing,vtMissing);
	sheets.AttachDispatch(book.GetWorksheets()); 
}

bool ExcelMgr::Open(CString path)
{
	COleVariant
	covTrue((short)TRUE),covFalse((short)FALSE),covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	books.AttachDispatch(app.GetWorkbooks());
	lpDisp = books.Open(path,
		covOptional, covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional );
	//得到Workbook
	book.AttachDispatch(lpDisp);
	//得到Worksheets 
	sheets.AttachDispatch(book.GetWorksheets()); 
	//得到当前活跃sheet
	//如果有单元格正处于编辑状态中，此操作不能返回，会一直等待
// 	lpDisp=book.GetActiveSheet();
// 	sheet.AttachDispatch(lpDisp);
	return true;
}

int ExcelMgr::GetSheetsCount()
{
	return sheets.GetCount();
}


bool ExcelMgr::SetCurSheet(int index)
{
	if(index <= sheets.GetCount() && index > 0)
	{
		Cursheet = sheets.GetItem(COleVariant((short)index));
		return true;
	}
	return false;
}

CString ExcelMgr::GetText(CString pos)
{
	COleVariant olestr;
	try
	{
		Range range = Cursheet.GetRange(COleVariant(pos),COleVariant(pos));
		olestr = range.GetValue();
	}
	catch (CException* e)
	{
		return _T("");
	}
	
	
	CString str;
	switch(olestr.vt)
	{
	case VT_BSTR:
		{
			str=olestr.bstrVal;
			break;
		}
	case VT_I4:
		{
			str.Format(_T("%d"), olestr.lVal);
			break;
		}
	case VT_I4 | VT_BYREF:
		{
			str.Format(_T("%d"), olestr.plVal);
			break;
		}
	case VT_R8:
		{
			str.Format(_T("%f"), olestr.dblVal);
			break;
		}
	default:
		return L""; // unknown VARIANT type (this ASSERT is optional)
	}
	str.Replace(L" ",L"");
	str.Replace(L"\r",L"");
	str.Replace(L"\n",L"");
	str.Replace(L"\t",L"");
	RemoveDot(str);
	return str;
}

void ExcelMgr::SetText(CString pos, CString text)
{
	Range range = Cursheet.GetRange(COleVariant(pos),COleVariant(pos));
	range.SetValue(COleVariant(text));
	Range cols = range.GetEntireColumn();
	cols.AutoFit();
	//range.AutoFit();
}
void ExcelMgr::SetItem(CString pos, CString pos2, CString text)
{
	Range range = Cursheet.GetRange(COleVariant(pos),COleVariant(pos2));
	range.SetValue(COleVariant(text));
	Range cols = range.GetEntireColumn();
	cols.AutoFit();
	//range.AutoFit();
}


void ExcelMgr::RemoveDot(CString &str)
{
	int i = 0;
	int pos=str.Find(L".");
	if(pos==-1)
	{
		return;
	}
	int length=str.GetLength();
	for(i=pos+1;i<length;i++)
	{
		if(str.GetAt(i)!='0')
		{
			break;
		}
	}
	if(i<length)
	{
		return;
	}
	CString temp;
	for(i=0;i<pos;i++)
	{
		temp.Insert(i,str.GetAt(i));
	}
	str=temp;
}

CString ExcelMgr::GetSheetName()
{
	return Cursheet.GetName();
}

void ExcelMgr::Close()
{
	books.Close();
}


void ExcelMgr::Save()
{
	if (!book.GetSaved())
		book.Save();
}

void ExcelMgr::Merge(CString Pos1,CString Pos2)
{
	Range range = Cursheet.GetRange(COleVariant(Pos1),COleVariant(Pos2));
	range.Merge(COleVariant((long)0));   //合并单元格
}

bool ExcelMgr::AddSheet(int count)
{
	for (int i = 0; i < count ; i++)
	{
		Cursheet = sheets.Add(vtMissing,vtMissing,vtMissing,COleVariant((short)1));
	}
	return true;
}

void ExcelMgr::SetSheetName(CString name)
{
	Cursheet.SetName(name);
}


