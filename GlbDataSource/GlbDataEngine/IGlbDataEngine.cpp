#include "StdAfx.h"
#include "IGlbDataEngine.h"
#include "GlbConvert.h"

glbInt32 getPixelSize(GlbPixelTypeEnum pixelType)
{
	glbInt32 size = 0;
	switch (pixelType)
	{
	case GLB_PIXELTYPE_BYTE:		 	        //"�ֽ�"		  size=1�ֽ�
		size = 1;
		break;
	case GLB_PIXELTYPE_USHORT16:               //"�޷���λ����"   size=2�ֽ�
		size = 2;
		break;
	case GLB_PIXELTYPE_SHORT16:                //"16λ����"       size=2�ֽ�
		size = 2;
		break;
	case GLB_PIXELTYPE_UINT32:                 //"�޷���λ����"   size=4�ֽ�
		size = 4;
		break;
	case GLB_PIXELTYPE_INT32:                  //"32λ����"       size=4�ֽ�
		size = 4;
		break;
	case GLB_PIXELTYPE_FLOAT32:                //"32λ������"     size=4�ֽ�
		size = 4;
		break;
	case GLB_PIXELTYPE_DOUBLE64:               //"64λ������"     size=8�ֽ�
		size = 8;
		break;
	}
	return size;
}
GLBVARIANT* defautToVariant(CGlbField* field)
{
	if(field == NULL || field->GetDefault() == NULL)
		return NULL;
	glbBool   rt = false;
	glbInt64  ll = 0;
	glbDouble db = 0.0;
	glbDate   dt = 0;
	glbDouble  x,y,z;
	switch(field->GetType())
	{
		case GLB_DATATYPE_BYTE://1�޷��� 8λ			
		//case GLB_DATATYPE_CHAR:			
		case GLB_DATATYPE_BOOLEAN://3����
		case GLB_DATATYPE_INT16://
		case GLB_DATATYPE_INT32://��������32λ
		case GLB_DATATYPE_INT64://64λ����
			{
				rt = CGlbConvert::ToInt64(field->GetDefault(),ll);
			}break;
		case GLB_DATATYPE_FLOAT://7�����ȸ�������32�ֽ�
		case GLB_DATATYPE_DOUBLE://8��������64λ
			{
				rt = CGlbConvert::ToDouble(field->GetDefault(),db);
			}break;
		case GLB_DATATYPE_STRING://9�ַ���
			rt = true;break;
		case GLB_DATATYPE_DATETIME:
			{
				glbInt32 y,m,d,h,mi,s;
				glbInt32 i = swscanf_s(field->GetDefault(),L"%d-%d-%d %d:%d:%d",
					                 &y,&m,&d,&h,&mi,&s);
				if(i== -1)break;
				rt = true;
				struct tm tms;
				memset((void*)&tms,0,sizeof(tms));
				tms.tm_mday = d;
				tms.tm_mon  = m - 1; 
				tms.tm_year = y - 1900;
				tms.tm_hour = h;
				tms.tm_min  = mi;
				tms.tm_sec  = s;
				dt = mktime(&tms);
			}break;
		case GLB_DATATYPE_VECTOR:
			{				
				glbInt32 i = swscanf_s(field->GetDefault(),L"%lf,%lf,%lf",&x,&y,&z);
				if(i == -1)break;
				rt = true;
			}break;
	}
	if(rt == false)return NULL;
	GLBVARIANT *val = new GLBVARIANT();
	val->vt     = field->GetType();
	val->isnull = false;
	val->isarray= false;
	switch(field->GetType())
	{
		case GLB_DATATYPE_BYTE://1�޷��� 8λ	
			val->bVal = (glbByte)ll;break;
		//case GLB_DATATYPE_CHAR:			
		case GLB_DATATYPE_BOOLEAN://3����
			{
				glbInt32 bl = (glbInt32)ll;
				val->blVal = bl==1?true:false;
			}
			break;
		case GLB_DATATYPE_INT16://
			val->iVal = (glbInt16)ll;break;
		case GLB_DATATYPE_INT32://��������32λ
			val->lVal = (glbInt32)ll;break;
		case GLB_DATATYPE_INT64://64λ����
			val->llVal = (glbInt64)ll;break;
		case GLB_DATATYPE_FLOAT://7�����ȸ�������32�ֽ�
			val->fVal = (glbFloat)db;break;
		case GLB_DATATYPE_DOUBLE://8��������64λ
			val->dblVal=db;break;
		case GLB_DATATYPE_STRING://9�ַ���
			{
				if(field->GetDefault())
				{
					CGlbWString df = field->GetDefault();
					val->strVal = (glbWChar*)malloc(sizeof(glbWChar*)*(df.length()+1));
					if(val->strVal = NULL)
					{
						delete val;
						return NULL;
					}
					memset(val->strVal,0,sizeof(glbWChar)*(df.length()+1));
					wcscpy_s(val->strVal,df.length()+1,df.c_str());
				}
			}break;
		case GLB_DATATYPE_DATETIME:
			val->date = dt;break;
		case GLB_DATATYPE_VECTOR:
			{
				val->vector.x = x;
				val->vector.y = y;
				val->vector.z = z;
			}break;
	}
	return val;
}

void IGlbRasterDataset::SetSelfDealMoveExtent( CGlbExtent *extent,glbBool isMove )
{

}

void IGlbRasterDataset::SetSelfDealChangeExtent( CGlbExtent *extent,glbDouble value,glbBool isChange )
{

}
