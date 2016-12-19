#include "StdAfx.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbPath.h"


using namespace GlbGlobe;

// GlbRenderInt32
bool GlbRenderInt32::operator != (const GlbRenderInt32& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderInt32::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{		
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderInt32::Save(xmlNodePtr node)
{
	char str[32];
	sprintf_s(str,"%d",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderInt32::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&bUseField);		
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderDouble
bool GlbRenderDouble::operator != (const GlbRenderDouble& r) const 
{
	return bUseField != r.bUseField ||
		fabs(value - r.value) > 0.0000001 || field != r.field;
}

glbDouble GlbRenderDouble::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_DOUBLE)
				return va->dblVal;
		}
	}
	return value;
}

void GlbRenderDouble::Save(xmlNodePtr node)
{
	char str[64];
	sprintf_s(str,"%d",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%.8lf",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderDouble::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&bUseField);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&value);	
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderBool
bool GlbRenderBool::operator != (const GlbRenderBool& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbBool GlbRenderBool::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_BOOLEAN)
				return va->blVal;
		}
	}
	return value;
}

void GlbRenderBool::Save(xmlNodePtr node)
{
	char str[8];
	sprintf_s(str,"%d",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderBool::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&bUseField);		
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			field = glbstr.ToWString();
			xmlFree(szKey);
			free(str);
		}
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);	
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderString
bool GlbRenderString::operator != (const GlbRenderString& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbWChar* GlbRenderString::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_STRING)
				return va->strVal;
		}
	}
	return (glbWChar*)value.c_str();
}


void GlbRenderString::Save(xmlNodePtr node)
{
	char str[128];
	sprintf_s(str,"%d",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);	
		if(szOut)free(szOut);
	}
	else
	{
		char* szOut = g2u((char*)value.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST szOut);
		if(szOut)free(szOut);
	}
}

void GlbRenderString::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&bUseField);
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			field = glbstr.ToWString();
			xmlFree(szKey);
			free(str);
		}
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			value = glbstr.ToWString();
			xmlFree(szKey);
			if (glbstr.length()>0)
				free(str);
		}
		node = node->next;
	}
}

// GlbRenderColor
bool GlbRenderColor::operator != (const GlbRenderColor& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderColor::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_DOUBLE)
				return (glbInt32)(va->dblVal);
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_FLOAT)
				return (glbInt32)(va->fVal);
		}
	}
	return value;
}

void GlbRenderColor::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%d",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		if(szOut)free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderColor::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&bUseField);
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			field = glbstr.ToWString();
			xmlFree(szKey);
			free(str);
		}
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);	
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderMaterial
bool GlbRenderMaterial::operator != (const GlbRenderMaterial& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderMaterial::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{		
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderMaterial::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%c",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderMaterial::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bUseField);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);	
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderLinePattern
bool GlbRenderLinePattern::operator != (const GlbRenderLinePattern& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderLinePattern::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderLinePattern::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%d",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		if(szOut)free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderLinePattern::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&bUseField);
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			field = glbstr.ToWString();
			xmlFree(szKey);
			free(str);
		}
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);		
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderTexRepeatMode
bool GlbRenderTexRepeatMode::operator != (const GlbRenderTexRepeatMode& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderTexRepeatMode::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderTexRepeatMode::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%d",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		if(szOut)free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderTexRepeatMode::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&bUseField);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			field = glbstr.ToWString();
			xmlFree(szKey);
			free(str);
		}
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);	
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderArrowPattern
bool GlbRenderArrowPattern::operator != (const GlbRenderArrowPattern& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderArrowPattern::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{		
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderArrowPattern::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%c",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderArrowPattern::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bUseField);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);	
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderBillboard
bool GlbRenderBillboard::operator != (const GlbRenderBillboard& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderBillboard::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderBillboard::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%c",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderBillboard::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bUseField);		
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderMultilineAlign
bool GlbRenderMultilineAlign::operator != (const GlbRenderMultilineAlign& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderMultilineAlign::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderMultilineAlign::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%c",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderMultilineAlign::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bUseField);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);		
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderLabelAlign
bool GlbRenderLabelAlign::operator != (const GlbRenderLabelAlign& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderLabelAlign::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderLabelAlign::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%c",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderLabelAlign::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bUseField);		
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		free(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);	
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderImageAlign
bool GlbRenderImageAlign::operator != (const GlbRenderLabelAlign& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderImageAlign::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderImageAlign::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%c",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST field.ToString().c_str());
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderImageAlign::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bUseField);
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);
		xmlFree(szKey);
		node = node->next;
	}
}

// GlbRenderLabelRelation
bool GlbRenderLabelRelation::operator != (const GlbRenderLabelRelation& r) const 
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbRenderLabelRelation::GetValue(CGlbFeature *feature)
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbRenderLabelRelation::Save(xmlNodePtr node)
{
	char str[16];
	sprintf_s(str,"%c",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST field.ToString().c_str());
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbRenderLabelRelation::Load(xmlNodePtr node)
{			
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bUseField);	
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();			
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);	
		xmlFree(szKey);
		node = node->next;
	}
}

//GlbMarkerModelSymbolInfo
void GlbMarkerModelSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr locatenode = xmlNewNode(NULL,BAD_CAST "Location");
	xmlAddChild(node,locatenode);
	if(locate)
	{
		if(locate->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = locate->bUseField;

			CGlbWString apath = CGlbPath::GetDir(locate->value);	
			CGlbWString filename = CGlbPath::GetFileName(locate->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)				
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}

			rdstring.Save(locatenode);
		}else{
			locate->Save(locatenode);
		}		
	}

	xmlNodePtr opacitynode = xmlNewNode(NULL,BAD_CAST "Opacity");
	xmlAddChild(node,opacitynode);
	if(opacity)opacity->Save(opacitynode);

	xmlNodePtr xscalenode = xmlNewNode(NULL,BAD_CAST "XScale");
	xmlAddChild(node,xscalenode);
	if(xScale)xScale->Save(xscalenode);

	xmlNodePtr yscalenode = xmlNewNode(NULL,BAD_CAST "YScale");
	xmlAddChild(node,yscalenode);
	if(yScale)yScale->Save(yscalenode);

	xmlNodePtr zscalenode = xmlNewNode(NULL,BAD_CAST "ZScale");
	xmlAddChild(node,zscalenode);
	if(zScale)zScale->Save(zscalenode);

	xmlNodePtr pitchnode = xmlNewNode(NULL,BAD_CAST "Pitch");
	xmlAddChild(node,pitchnode);
	if(pitch)pitch->Save(pitchnode);

	xmlNodePtr yawnode = xmlNewNode(NULL,BAD_CAST "Yaw");
	xmlAddChild(node,yawnode);
	if(yaw)yaw->Save(yawnode);

	xmlNodePtr rollnode = xmlNewNode(NULL,BAD_CAST "Roll");
	xmlAddChild(node,rollnode);
	if(roll)roll->Save(rollnode);

	xmlNodePtr xoffsetnode = xmlNewNode(NULL,BAD_CAST "XOffset");
	xmlAddChild(node,xoffsetnode);
	if(xOffset)xOffset->Save(xoffsetnode);

	xmlNodePtr yoffsetnode = xmlNewNode(NULL,BAD_CAST "YOffset");
	xmlAddChild(node,yoffsetnode);
	if(yOffset)yOffset->Save(yoffsetnode);

	xmlNodePtr zoffsetnode = xmlNewNode(NULL,BAD_CAST "ZOffset");
	xmlAddChild(node,zoffsetnode);
	if(zOffset)zOffset->Save(zoffsetnode);

	xmlNodePtr labelNode = xmlNewNode(NULL,BAD_CAST "Label");
	xmlAddChild(node,labelNode);
	if(label)label->Save(labelNode);
}
void GlbMarkerModelSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Location")))
	{
		locate = new GlbRenderString();
		locate->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(locate->value);
		CGlbWString filename = CGlbPath::GetFileName(locate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			locate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = locate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)
				locate->value += L"\\";
			locate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZScale")))
	{
		zScale = new GlbRenderDouble();
		zScale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Label")))
	{
		label = new GlbRenderSimpleLabel();
		label->Load(node->xmlChildrenNode);
		node = node->next;
	}
}
//GlbMarker2DShapeSymbolInfo
void GlbMarker2DShapeSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr fillinfoNode = xmlNewNode(NULL,BAD_CAST "FillInfo");
	xmlAddChild(node,fillinfoNode);
	if(fillInfo)fillInfo->Save(fillinfoNode,relativepath);

	xmlNodePtr shapeInfoNode = xmlNewNode(NULL,BAD_CAST "ShapeInfo");
	xmlAddChild(node,shapeInfoNode);
	if(shapeInfo)shapeInfo->Save(shapeInfoNode);

	xmlNodePtr yawnode = xmlNewNode(NULL,BAD_CAST "Yaw");
	xmlAddChild(node,yawnode);
	if(yaw)yaw->Save(yawnode);

	if(xScale)
	{
		xmlNodePtr xscalenode = xmlNewNode(NULL,BAD_CAST "XScale");
		xmlAddChild(node,xscalenode);
		xScale->Save(xscalenode);
	}

	if(yScale)
	{
		xmlNodePtr yscalenode = xmlNewNode(NULL,BAD_CAST "YScale");
		xmlAddChild(node,yscalenode);
		yScale->Save(yscalenode);
	}

	if(zScale)
	{
		xmlNodePtr zscalenode = xmlNewNode(NULL,BAD_CAST "ZScale");
		xmlAddChild(node,zscalenode);
		zScale->Save(zscalenode);
	}

	xmlNodePtr xoffsetnode = xmlNewNode(NULL,BAD_CAST "XOffset");
	xmlAddChild(node,xoffsetnode);
	if(xOffset)xOffset->Save(xoffsetnode);

	xmlNodePtr yoffsetnode = xmlNewNode(NULL,BAD_CAST "YOffset");
	xmlAddChild(node,yoffsetnode);
	if(yOffset)yOffset->Save(yoffsetnode);

	xmlNodePtr zoffsetnode = xmlNewNode(NULL,BAD_CAST "ZOffset");
	xmlAddChild(node,zoffsetnode);
	if(zOffset)zOffset->Save(zoffsetnode);
}
void GlbMarker2DShapeSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"FillInfo")))
	{
		fillInfo = new GlbPolygon2DSymbolInfo();
		fillInfo->Load(node->xmlChildrenNode,relativepath);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ShapeInfo")))
	{
		xmlNodePtr shapetypeNode = node->xmlChildrenNode;
		xmlChar* szKey;

		if (shapetypeNode && (!xmlStrcmp(shapetypeNode->name, (const xmlChar *)"ShapeType")))
		{
			szKey = xmlNodeGetContent(shapetypeNode);
			GlbMarkerShapeTypeEnum  shapetype;
			sscanf_s((char*)szKey,"%d",&shapetype);
			xmlFree(szKey);

			switch(shapetype)
			{
			case GLB_MARKERSHAPE_CIRCLE:   shapeInfo = new GlbCircleInfo();break;
			case GLB_MARKERSHAPE_RECTANGLE:shapeInfo = new GlbRectangleInfo();break;
			case GLB_MARKERSHAPE_ELLIPSE:  shapeInfo = new GlbEllipseInfo();break;
			case GLB_MARKERSHAPE_ARC:      shapeInfo = new GlbArcInfo();break;
			case GLB_MARKERSHAPE_SPHERE:   shapeInfo = new GlbSphereInfo();break;
			case GLB_MARKERSHAPE_BOX:      shapeInfo = new GlbBoxInfo();break;
			case GLB_MARKERSHAPE_PYRAMID:  shapeInfo = new GlbPyramidInfo();break;
			case GLB_MARKERSHAPE_CONE:     shapeInfo = new GlbConeInfo();break;
			case GLB_MARKERSHAPE_PIE:      shapeInfo = new GlbPieInfo();break;
			case GLB_MARKERSHAPE_CYLINDER: shapeInfo = new GlbCylinderInfo();break;
			}
			if(shapeInfo)shapeInfo->Load(&(shapetypeNode->next));
			node = node->next;
		}
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZScale")))
	{
		zScale = new GlbRenderDouble();
		zScale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}
}
//GlbMarker3DShapeSymbolInfo
void GlbMarker3DShapeSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr fillinfoNode = xmlNewNode(NULL,BAD_CAST "FillInfo");
	xmlAddChild(node,fillinfoNode);
	if(fillInfo)fillInfo->Save(fillinfoNode,relativepath);

	xmlNodePtr shapeInfoNode = xmlNewNode(NULL,BAD_CAST "ShapeInfo");
	xmlAddChild(node,shapeInfoNode);
	if(shapeInfo)shapeInfo->Save(shapeInfoNode);

	xmlNodePtr yawnode = xmlNewNode(NULL,BAD_CAST "Yaw");
	xmlAddChild(node,yawnode);
	if(yaw)yaw->Save(yawnode);

	xmlNodePtr pitchnode = xmlNewNode(NULL,BAD_CAST "Pitch");
	xmlAddChild(node,pitchnode);
	if(pitch)pitch->Save(pitchnode);

	xmlNodePtr rollnode = xmlNewNode(NULL,BAD_CAST "Roll");
	xmlAddChild(node,rollnode);
	if(roll)roll->Save(rollnode);

	if(xScale)
	{
		xmlNodePtr xscalenode = xmlNewNode(NULL,BAD_CAST "XScale");
		xmlAddChild(node,xscalenode);
		xScale->Save(xscalenode);
	}

	if(yScale)
	{
		xmlNodePtr yscalenode = xmlNewNode(NULL,BAD_CAST "YScale");
		xmlAddChild(node,yscalenode);
		yScale->Save(yscalenode);
	}

	if(zScale)
	{
		xmlNodePtr zscalenode = xmlNewNode(NULL,BAD_CAST "ZScale");
		xmlAddChild(node,zscalenode);
		zScale->Save(zscalenode);
	}

	xmlNodePtr xoffsetnode = xmlNewNode(NULL,BAD_CAST "XOffset");
	xmlAddChild(node,xoffsetnode);
	if(xOffset)xOffset->Save(xoffsetnode);

	xmlNodePtr yoffsetnode = xmlNewNode(NULL,BAD_CAST "YOffset");
	xmlAddChild(node,yoffsetnode);
	if(yOffset)yOffset->Save(yoffsetnode);

	xmlNodePtr zoffsetnode = xmlNewNode(NULL,BAD_CAST "ZOffset");
	xmlAddChild(node,zoffsetnode);
	if(zOffset)zOffset->Save(zoffsetnode);
}
void GlbMarker3DShapeSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"FillInfo")))
	{
		fillInfo = new GlbPolygon2DSymbolInfo();
		fillInfo->Load(node->xmlChildrenNode,relativepath);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ShapeInfo")))
	{
		xmlNodePtr shapetypeNode = node->xmlChildrenNode;
		xmlChar* szKey;

		if (shapetypeNode && (!xmlStrcmp(shapetypeNode->name, (const xmlChar *)"ShapeType")))
		{
			szKey = xmlNodeGetContent(shapetypeNode);
			GlbMarkerShapeTypeEnum  shapetype;
			sscanf_s((char*)szKey,"%d",&shapetype);
			xmlFree(szKey);

			switch(shapetype)
			{
			case GLB_MARKERSHAPE_CIRCLE:   shapeInfo = new GlbCircleInfo();break;
			case GLB_MARKERSHAPE_RECTANGLE:shapeInfo = new GlbRectangleInfo();break;
			case GLB_MARKERSHAPE_ELLIPSE:  shapeInfo = new GlbEllipseInfo();break;
			case GLB_MARKERSHAPE_ARC:      shapeInfo = new GlbArcInfo();break;
			case GLB_MARKERSHAPE_SPHERE:   shapeInfo = new GlbSphereInfo();break;
			case GLB_MARKERSHAPE_BOX:      shapeInfo = new GlbBoxInfo();break;
			case GLB_MARKERSHAPE_PYRAMID:  shapeInfo = new GlbPyramidInfo();break;
			case GLB_MARKERSHAPE_CONE:     shapeInfo = new GlbConeInfo();break;
			case GLB_MARKERSHAPE_PIE:      shapeInfo = new GlbPieInfo();break;
			case GLB_MARKERSHAPE_CYLINDER: shapeInfo = new GlbCylinderInfo();break;
			}
			if(shapeInfo)shapeInfo->Load(&(shapetypeNode->next));
			node = node->next;
		}
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZScale")))
	{
		zScale = new GlbRenderDouble();
		zScale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}
}

//GlbMarkerPixelSymbolInfo
void GlbMarkerPixelSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color)color->Save(colorNode);

	xmlNodePtr opacityNode = xmlNewNode(NULL,BAD_CAST "Opacity");
	xmlAddChild(node,opacityNode);
	if(opacity)opacity->Save(opacityNode);

	xmlNodePtr sizeNode = xmlNewNode(NULL,BAD_CAST "Size");
	xmlAddChild(node,sizeNode);
	if(size)size->Save(sizeNode);

	xmlNodePtr labelNode = xmlNewNode(NULL,BAD_CAST "Label");
	xmlAddChild(node,labelNode);
	if(label)label->Save(labelNode);
}

void GlbMarkerPixelSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Size")))
	{
		size = new GlbRenderInt32();
		size->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Label")))
	{
		label = new GlbRenderSimpleLabel();
		label->Load(node->xmlChildrenNode);
		node = node->next;
	}
}
//GlbMarkerLabelSymbolInfo
void GlbMarkerLabelSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr modeNode = xmlNewNode(NULL,BAD_CAST "Mode");
	xmlAddChild(node,modeNode);
	if(mode)mode->Save(modeNode);

	xmlNodePtr labelAlignNode = xmlNewNode(NULL,BAD_CAST "LabelAlign");
	xmlAddChild(node,labelAlignNode);
	if(labelAlign)labelAlign->Save(labelAlignNode);

	xmlNodePtr textInfoNode = xmlNewNode(NULL,BAD_CAST "TextInfo");
	xmlAddChild(node,textInfoNode);
	if(textInfo)textInfo->Save(textInfoNode);

	xmlNodePtr imageInfoNode = xmlNewNode(NULL,BAD_CAST "ImageInfo");
	xmlAddChild(node,imageInfoNode);
	if(imageInfo)imageInfo->Save(imageInfoNode,relativepath);

	xmlNodePtr labelImageRelationInfoNode = xmlNewNode(NULL,BAD_CAST "LabelImageRelationInfo");
	xmlAddChild(node,labelImageRelationInfoNode);
	if(labelImageRelationInfo)labelImageRelationInfo->Save(labelImageRelationInfoNode);

	xmlNodePtr geoInfoNode = xmlNewNode(NULL,BAD_CAST "GeoInfo");
	xmlAddChild(node,geoInfoNode);
	if(geoInfo)geoInfo->Save(geoInfoNode);

	xmlNodePtr yawNode = xmlNewNode(NULL,BAD_CAST "Yaw");
	xmlAddChild(node,yawNode);
	if(yaw)yaw->Save(yawNode);

	xmlNodePtr pitchNode = xmlNewNode(NULL,BAD_CAST "Pitch");
	xmlAddChild(node,pitchNode);
	if(pitch)pitch->Save(pitchNode);

	xmlNodePtr rollNode = xmlNewNode(NULL,BAD_CAST "Roll");
	xmlAddChild(node,rollNode);
	if(roll)roll->Save(rollNode);

	xmlNodePtr xOffsetNode = xmlNewNode(NULL,BAD_CAST "XOffset");
	xmlAddChild(node,xOffsetNode);
	if(xOffset)xOffset->Save(xOffsetNode);

	xmlNodePtr yOffsetNode = xmlNewNode(NULL,BAD_CAST "YOffset");
	xmlAddChild(node,yOffsetNode);
	if(yOffset)yOffset->Save(yOffsetNode);

	xmlNodePtr zOffsetNode = xmlNewNode(NULL,BAD_CAST "ZOffset");
	xmlAddChild(node,zOffsetNode);
	if(zOffset)zOffset->Save(zOffsetNode);
}
void GlbMarkerLabelSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Mode")))
	{
		mode = new GlbRenderBillboard();
		mode->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LabelAlign")))
	{
		labelAlign = new GlbRenderLabelAlign();
		labelAlign->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TextInfo")))
	{
		textInfo = new GlbRenderTextInfo();
		textInfo->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ImageInfo")))
	{
		imageInfo = new GlbRenderImageInfo();
		imageInfo->Load(node->xmlChildrenNode,relativepath);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LabelImageRelationInfo")))
	{
		labelImageRelationInfo = new GlbRenderLabelRelationInfo();
		labelImageRelationInfo->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GeoInfo")))
	{
		geoInfo = new GlbRenderGeoInfo();
		geoInfo->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"XOffset")))
	{
		xOffset = new GlbRenderDouble;
		xOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"YOffset")))
	{
		yOffset = new GlbRenderDouble;
		yOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"ZOffset")))
	{
		zOffset = new GlbRenderDouble;
		zOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
}
void GlbMarkerImageSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr imgInfoNode = xmlNewNode(NULL,BAD_CAST "ImgInfo");
	xmlAddChild(node,imgInfoNode);
	if(imgInfo)imgInfo->Save(imgInfoNode,relativepath);

	xmlNodePtr imgBillboardNode = xmlNewNode(NULL,BAD_CAST "ImgBillboard");
	xmlAddChild(node,imgBillboardNode);
	if(imgBillboard)imgBillboard->Save(imgBillboardNode);

	xmlNodePtr imageAlignNode = xmlNewNode(NULL,BAD_CAST "ImageAlign");
	xmlAddChild(node,imageAlignNode);
	if(imageAlign)imageAlign->Save(imageAlignNode);

	xmlNodePtr geoInfoNode = xmlNewNode(NULL,BAD_CAST "GeoInfo");
	xmlAddChild(node,geoInfoNode);
	if(geoInfo)geoInfo->Save(geoInfoNode);

	xmlNodePtr yawNode = xmlNewNode(NULL,BAD_CAST "Yaw");
	xmlAddChild(node,yawNode);
	if(yaw)yaw->Save(yawNode);

	xmlNodePtr pitchNode = xmlNewNode(NULL,BAD_CAST "Pitch");
	xmlAddChild(node,pitchNode);
	if(pitch)pitch->Save(pitchNode);

	xmlNodePtr rollNode = xmlNewNode(NULL,BAD_CAST "Roll");
	xmlAddChild(node,rollNode);
	if(roll)roll->Save(rollNode);

	xmlNodePtr xOffsetNode = xmlNewNode(NULL,BAD_CAST "XOffset");
	xmlAddChild(node,xOffsetNode);
	if(xOffset)xOffset->Save(xOffsetNode);

	xmlNodePtr yOffsetNode = xmlNewNode(NULL,BAD_CAST "YOffset");
	xmlAddChild(node,yOffsetNode);
	if(yOffset)yOffset->Save(yOffsetNode);

	xmlNodePtr zOffsetNode = xmlNewNode(NULL,BAD_CAST "ZOffset");
	xmlAddChild(node,zOffsetNode);
	if(zOffset)zOffset->Save(zOffsetNode);
}
void GlbMarkerImageSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ImgInfo")))
	{
		imgInfo = new GlbRenderImageInfo();
		imgInfo->Load(node->xmlChildrenNode,relativepath);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ImgBillboard")))
	{
		imgBillboard = new GlbRenderBillboard();
		imgBillboard->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ImageAlign")))
	{
		imageAlign = new GlbRenderImageAlign();
		imageAlign->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GeoInfo")))
	{
		geoInfo = new GlbRenderGeoInfo();
		geoInfo->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"XOffset")))
	{
		xOffset = new GlbRenderDouble;
		xOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"YOffset")))
	{
		yOffset = new GlbRenderDouble;
		yOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"ZOffset")))
	{
		zOffset = new GlbRenderDouble;
		zOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
}
//GlbPolygon2DSymbolInfo
void GlbPolygon2DSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr outlineInfoNode = xmlNewNode(NULL,BAD_CAST "OutLineInfo");
	xmlAddChild(node,outlineInfoNode);
	if(outlineInfo)outlineInfo->Save(outlineInfoNode,relativepath);

	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color)color->Save(colorNode);

	xmlNodePtr opacityNode = xmlNewNode(NULL,BAD_CAST "Opacity");
	xmlAddChild(node,opacityNode);
	if(opacity)opacity->Save(opacityNode);

	xmlNodePtr textureDataNode = xmlNewNode(NULL,BAD_CAST "TextureData");
	xmlAddChild(node,textureDataNode);
	if(textureData->bUseField == false)
	{
		GlbRenderString rdstring;
		rdstring.bUseField = textureData->bUseField;

		CGlbWString apath = CGlbPath::GetDir(textureData->value);
		CGlbWString filename = CGlbPath::GetFileName(textureData->value);
		glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
		if(rpath)
		{
			rdstring.value = rpath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = rdstring.value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (rdstring.value != L"" && needExpand)	
				rdstring.value += L"\\";
			rdstring.value += filename;
			delete rpath;
		}
		rdstring.Save(textureDataNode);
	}else{
		textureData->Save(textureDataNode);
	}		

	xmlNodePtr texRepeatModeNode = xmlNewNode(NULL,BAD_CAST "TexRepeatMode");
	xmlAddChild(node,texRepeatModeNode);
	if(texRepeatMode)texRepeatMode->Save(texRepeatModeNode);

	xmlNodePtr tilingUNode = xmlNewNode(NULL,BAD_CAST "TilingU");
	xmlAddChild(node,tilingUNode);
	if(tilingU)tilingU->Save(tilingUNode);

	xmlNodePtr tilingVNode = xmlNewNode(NULL,BAD_CAST "TilingV");
	xmlAddChild(node,tilingVNode);
	if(tilingV)tilingV->Save(tilingVNode);

	xmlNodePtr textureRotationNode = xmlNewNode(NULL,BAD_CAST "TextureRotation");
	xmlAddChild(node,textureRotationNode);
	if(textureRotation)textureRotation->Save(textureRotationNode);
}
void GlbPolygon2DSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"OutLineInfo")))
	{
		outlineInfo = new GlbLinePixelSymbolInfo();
		outlineInfo->Load(node->xmlChildrenNode,relativepath);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TextureData")))
	{
		textureData = new GlbRenderString();
		textureData->Load(node->xmlChildrenNode);		

		CGlbWString rpath = CGlbPath::GetDir(textureData->value);
		CGlbWString filename = CGlbPath::GetFileName(textureData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			textureData->value = apath;	
			// 判断是否需要在路径后面添加 "\"
			std::string _path = textureData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				textureData->value += L"\\";
			textureData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TexRepeatMode")))
	{
		texRepeatMode = new GlbRenderTexRepeatMode();
		texRepeatMode->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TilingU")))
	{
		tilingU = new GlbRenderInt32();
		tilingU->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TilingV")))
	{
		tilingV = new GlbRenderInt32();
		tilingV->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TextureRotation")))
	{
		textureRotation = new GlbRenderDouble();
		textureRotation->Load(node->xmlChildrenNode);
		node = node->next;
	}
}
void GlbPolyhedronSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	char str[32];
	printf_s(str,"%d",type);
	xmlNewTextChild(node, NULL, BAD_CAST "RenderType", BAD_CAST str);

	sprintf_s(str,"%d",symType);
	xmlNewTextChild(node, NULL, BAD_CAST "SymbolType", BAD_CAST str);

	xmlNodePtr outlineInfoNode = xmlNewNode(NULL,BAD_CAST "OutlineInfo");
	xmlAddChild(node,outlineInfoNode);
	if(outlineInfo)outlineInfo->Save(outlineInfoNode,relativepath);

	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color)color->Save(colorNode);

	xmlNodePtr opacityNode = xmlNewNode(NULL,BAD_CAST "Opacity");
	xmlAddChild(node,opacityNode);
	if(opacity)opacity->Save(opacityNode);

	xmlNodePtr textureDataNode = xmlNewNode(NULL,BAD_CAST "TextureData");
	xmlAddChild(node,textureDataNode);
	if(textureData->bUseField == false)
	{
		GlbRenderString rdstring;
		rdstring.bUseField = textureData->bUseField;

		CGlbWString apath = CGlbPath::GetDir(textureData->value);
		CGlbWString filename = CGlbPath::GetFileName(textureData->value);
		glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
		if(rpath)
		{
			rdstring.value = rpath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = rdstring.value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (rdstring.value != L"" && needExpand)
				rdstring.value += L"\\";
			rdstring.value += filename;
			delete rpath;
		}
		rdstring.Save(textureDataNode);
	}else{
		textureData->Save(textureDataNode);
	}	

	xmlNodePtr texRepeatModeNode = xmlNewNode(NULL,BAD_CAST "texRepeatMode");
	xmlAddChild(node,texRepeatModeNode);
	if(texRepeatMode)texRepeatMode->Save(texRepeatModeNode);

	xmlNodePtr tilingUNode = xmlNewNode(NULL,BAD_CAST "TilingU");
	xmlAddChild(node,tilingUNode);
	if(tilingU)tilingU->Save(tilingUNode);

	xmlNodePtr tilingVNode = xmlNewNode(NULL,BAD_CAST "TilingV");
	xmlAddChild(node,tilingVNode);
	if(tilingV)tilingV->Save(tilingVNode);

	xmlNodePtr textureRotationNode = xmlNewNode(NULL,BAD_CAST "TextureRotation");
	xmlAddChild(node,textureRotationNode);
	if(textureRotation)textureRotation->Save(textureRotationNode);

	xmlNodePtr heightNode = xmlNewNode(NULL,BAD_CAST "Height");
	xmlAddChild(node,heightNode);
	if(height)height->Save(heightNode);

	xmlNodePtr yawNode = xmlNewNode(NULL,BAD_CAST "Yaw");
	xmlAddChild(node,yawNode);
	if(yaw)yaw->Save(yawNode);

	xmlNodePtr pitchNode = xmlNewNode(NULL,BAD_CAST "Pitch");
	xmlAddChild(node,pitchNode);
	if(pitch)pitch->Save(pitchNode);

	xmlNodePtr rollNode = xmlNewNode(NULL,BAD_CAST "Roll");
	xmlAddChild(node,rollNode);
	if(roll)roll->Save(rollNode);
}
void GlbPolyhedronSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"OutLineInfo")))
	{
		outlineInfo = new GlbLinePixelSymbolInfo();
		outlineInfo->Load(node->xmlChildrenNode,relativepath);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TextureData")))
	{
		textureData = new GlbRenderString();
		textureData->Load(node->xmlChildrenNode);		

		CGlbWString rpath = CGlbPath::GetDir(textureData->value);
		CGlbWString filename = CGlbPath::GetFileName(textureData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			textureData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = textureData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				textureData->value += L"\\";
			textureData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TexRepeatMode")))
	{
		texRepeatMode = new GlbRenderTexRepeatMode();
		texRepeatMode->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TilingU")))
	{
		tilingU = new GlbRenderInt32();
		tilingU->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TilingV")))
	{
		tilingV = new GlbRenderInt32();
		tilingV->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TextureRotation")))
	{
		textureRotation = new GlbRenderDouble();
		textureRotation->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Height")))
	{
		height = new GlbRenderDouble();
		height->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load(node->xmlChildrenNode);
		node = node->next;
	}
}
void GlbPolygonWaterSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	char str[32];
	printf_s(str,"%d",type);
	xmlNewTextChild(node, NULL, BAD_CAST "RenderType", BAD_CAST str);

	sprintf_s(str,"%d",symType);
	xmlNewTextChild(node, NULL, BAD_CAST "SymbolType", BAD_CAST str);

	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color)color->Save(colorNode);

	xmlNodePtr opacityNode = xmlNewNode(NULL,BAD_CAST "Opacity");
	xmlAddChild(node,opacityNode);
	if(opacity)opacity->Save(opacityNode);

	xmlNodePtr isUseEnvNode = xmlNewNode(NULL,BAD_CAST "IsUseEnv");
	xmlAddChild(node,isUseEnvNode);
	if(isUseEnv)isUseEnv->Save(isUseEnvNode);

	xmlNodePtr windStrongNode = xmlNewNode(NULL,BAD_CAST "WindStrong");
	xmlAddChild(node,windStrongNode);
	if(windStrong)windStrong->Save(windStrongNode);

	xmlNodePtr windDirectionNode = xmlNewNode(NULL,BAD_CAST "WindDirection");
	xmlAddChild(node,windDirectionNode);
	if(windDirection)windDirection->Save(windDirectionNode);

	xmlNodePtr enableReflectionsNode = xmlNewNode(NULL,BAD_CAST "EnableReflections");
	xmlAddChild(node,enableReflectionsNode);
	if(enableReflections)enableReflections->Save(enableReflectionsNode);

	xmlNodePtr enableRefractionsNode = xmlNewNode(NULL,BAD_CAST "EnableRefractions");
	xmlAddChild(node,enableRefractionsNode);
	if(enableRefractions)enableRefractions->Save(enableRefractionsNode);

	xmlNodePtr refractionImageDataNode = xmlNewNode(NULL,BAD_CAST "RefractionImageData");
	xmlAddChild(node,refractionImageDataNode);
	if(refractionImageData->bUseField == false)
	{
		GlbRenderString rdstring;
		rdstring.bUseField = refractionImageData->bUseField;

		CGlbWString apath = CGlbPath::GetDir(refractionImageData->value);
		CGlbWString filename = CGlbPath::GetFileName(refractionImageData->value);
		glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
		if(rpath)
		{
			rdstring.value = rpath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = rdstring.value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (rdstring.value != L"" && needExpand)
				rdstring.value += L"\\";
			rdstring.value += filename;
			delete rpath;
		}
		rdstring.Save(refractionImageDataNode);
	}else{
		refractionImageData->Save(refractionImageDataNode);
	}	

	xmlNodePtr refractTexRepeatModeNode = xmlNewNode(NULL,BAD_CAST "RefractTexRepeatMode");
	xmlAddChild(node,refractTexRepeatModeNode);
	if(refractTexRepeatMode)refractTexRepeatMode->Save(refractTexRepeatModeNode);

	xmlNodePtr refractTextilingUNode = xmlNewNode(NULL,BAD_CAST "RefractTextilingU");
	xmlAddChild(node,refractTextilingUNode);
	if(refractTextilingU)refractTextilingU->Save(refractTextilingUNode);

	xmlNodePtr refractTextilingVNode = xmlNewNode(NULL,BAD_CAST "RefractTextilingV");
	xmlAddChild(node,refractTextilingVNode);
	if(refractTextilingV)refractTextilingV->Save(refractTextilingVNode);

	xmlNodePtr refractTexRotationNode = xmlNewNode(NULL,BAD_CAST "RefractTexRotation");
	xmlAddChild(node,refractTexRotationNode);
	if(refractTexRotation)refractTexRotation->Save(refractTexRotationNode);
}
void GlbPolygonWaterSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseEnv")))
	{
		isUseEnv = new GlbRenderBool();
		isUseEnv->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"WindStrong")))
	{
		windStrong = new GlbRenderDouble();
		windStrong->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"WindDirection")))
	{
		windDirection = new GlbRenderDouble();
		windDirection->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"EnableReflections")))
	{
		enableReflections = new GlbRenderBool();
		enableReflections->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"EnableRefractions")))
	{
		enableRefractions = new GlbRenderBool();
		enableRefractions->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractionImageData")))
	{
		refractionImageData = new GlbRenderString();
		refractionImageData->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(refractionImageData->value);
		CGlbWString filename = CGlbPath::GetFileName(refractionImageData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			refractionImageData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = refractionImageData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				refractionImageData->value += L"\\";
			refractionImageData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractTexRepeatMode")))
	{
		refractTexRepeatMode = new GlbRenderTexRepeatMode();
		refractTexRepeatMode->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractTextilingU")))
	{
		refractTextilingU = new GlbRenderInt32();
		refractTextilingU->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractTextilingV")))
	{
		refractTextilingV = new GlbRenderInt32();
		refractTextilingV->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractTexRotation")))
	{
		refractTexRotation = new GlbRenderDouble();
		refractTexRotation->Load(node->xmlChildrenNode);
		node = node->next;
	}
}
//GlbLinePixelSymbolInfo
void GlbLinePixelSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr lineWidthNode = xmlNewNode(NULL,BAD_CAST "LineWidth");
	xmlAddChild(node,lineWidthNode);
	if(lineWidth)lineWidth->Save(lineWidthNode);

	xmlNodePtr lineColorNode = xmlNewNode(NULL,BAD_CAST "LineColor");
	xmlAddChild(node,lineColorNode);
	if(lineColor)lineColor->Save(lineColorNode);	

	xmlNodePtr lineOpacityNode = xmlNewNode(NULL,BAD_CAST "LineOpacity");
	xmlAddChild(node,lineOpacityNode);
	if(lineOpacity)lineOpacity->Save(lineOpacityNode);

	xmlNodePtr linePatternNode = xmlNewNode(NULL,BAD_CAST "LinePattern");
	xmlAddChild(node,linePatternNode);
	if(linePattern)linePattern->Save(linePatternNode);
}
void GlbLinePixelSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LineWidth")))
	{
		lineWidth = new GlbRenderDouble();
		lineWidth->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LineColor")))
	{
		lineColor = new GlbRenderColor();
		lineColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LineOpacity")))
	{
		lineOpacity = new GlbRenderInt32();
		lineOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LinePattern")))
	{
		linePattern = new GlbRenderLinePattern();
		linePattern->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbLine2DSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
}
void GlbLine2DSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
}
void GlbLine3DSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
}
void GlbLine3DSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
}

void GlbLineArrowSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr rollNode = xmlNewNode(NULL,BAD_CAST "Roll");
	xmlAddChild(node,rollNode);
	if(roll)roll->Save(rollNode);

	xmlNodePtr patternNode = xmlNewNode(NULL,BAD_CAST "Pattern");
	xmlAddChild(node,patternNode);
	if(pattern)pattern->Save(patternNode);	

	xmlNodePtr heightNode = xmlNewNode(NULL,BAD_CAST "Height");
	xmlAddChild(node,heightNode);
	if(height)height->Save(heightNode);	

	xmlNodePtr linePixelInfoNode = xmlNewNode(NULL,BAD_CAST "LinePixelInfo");
	xmlAddChild(node,linePixelInfoNode);
	if(linePixelInfo)linePixelInfo->Save(linePixelInfoNode,relativepath);

	xmlNodePtr widthNode = xmlNewNode(NULL,BAD_CAST "Width");
	xmlAddChild(node,widthNode);
	if(width)width->Save(widthNode);

	xmlNodePtr fillColorNode = xmlNewNode(NULL,BAD_CAST "FillColor");
	xmlAddChild(node,fillColorNode);
	if(fillColor)fillColor->Save(fillColorNode);

	xmlNodePtr fillOpacityNode = xmlNewNode(NULL,BAD_CAST "FillOpacity");
	xmlAddChild(node,fillOpacityNode);
	if(fillOpacity)fillOpacity->Save(fillOpacityNode);

	xmlNodePtr textureDataNode = xmlNewNode(NULL,BAD_CAST "TextureData");
	xmlAddChild(node,textureDataNode);
	if(textureData->bUseField == false)
	{
		GlbRenderString rdstring;
		rdstring.bUseField = textureData->bUseField;

		CGlbWString apath = CGlbPath::GetDir(textureData->value);
		CGlbWString filename = CGlbPath::GetFileName(textureData->value);
		glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
		if(rpath)
		{
			rdstring.value = rpath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = rdstring.value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (rdstring.value != L"" && needExpand)			
				rdstring.value += L"\\";
			rdstring.value += filename;
			delete rpath;
		}
		rdstring.Save(textureDataNode);
	}else{
		textureData->Save(textureDataNode);
	}		

	xmlNodePtr texRepeatModeNode = xmlNewNode(NULL,BAD_CAST "TexRepeatMode");
	xmlAddChild(node,texRepeatModeNode);
	if(texRepeatMode)texRepeatMode->Save(texRepeatModeNode);

	xmlNodePtr tilingUNode = xmlNewNode(NULL,BAD_CAST "TilingU");
	xmlAddChild(node,tilingUNode);
	if(tilingU)tilingU->Save(tilingUNode);

	xmlNodePtr tilingVNode = xmlNewNode(NULL,BAD_CAST "TilingV");
	xmlAddChild(node,tilingVNode);
	if(tilingV)tilingV->Save(tilingVNode);

	xmlNodePtr textureRotationNode = xmlNewNode(NULL,BAD_CAST "TextureRotation");
	xmlAddChild(node,textureRotationNode);
	if(textureRotation)textureRotation->Save(textureRotationNode);
}
void GlbLineArrowSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Pattern")))
	{
		pattern = new GlbRenderArrowPattern();
		pattern->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Height")))
	{
		height = new GlbRenderDouble();
		height->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LinePixelInfo")))
	{
		linePixelInfo = new GlbLinePixelSymbolInfo();
		linePixelInfo->Load(node->xmlChildrenNode,relativepath);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Width")))
	{
		width = new GlbRenderDouble();
		width->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"FillColor")))
	{
		fillColor = new GlbRenderColor();
		fillColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"FillOpacity")))
	{
		fillOpacity = new GlbRenderInt32();
		fillOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TextureData")))
	{
		textureData = new GlbRenderString();
		textureData->Load(node->xmlChildrenNode);		

		CGlbWString rpath = CGlbPath::GetDir(textureData->value);
		CGlbWString filename = CGlbPath::GetFileName(textureData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			textureData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = textureData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				textureData->value += L"\\";
			textureData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TexRepeatMode")))
	{
		texRepeatMode = new GlbRenderTexRepeatMode();
		texRepeatMode->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TilingU")))
	{
		tilingU = new GlbRenderDouble();
		tilingU->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TilingV")))
	{
		tilingV = new GlbRenderDouble();
		tilingV->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TextureRotation")))
	{
		textureRotation = new GlbRenderDouble();
		textureRotation->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbLineModelSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	char str[32];
	sprintf_s(str,"%d",isRenderLine);
	xmlNewTextChild(node, NULL, BAD_CAST "isRenderLine", BAD_CAST str);

	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color)
		color->Save(colorNode);

	xmlNodePtr opacityNode = xmlNewNode(NULL,BAD_CAST "Opacity");
	xmlAddChild(node,opacityNode);
	if(opacity)
		opacity->Save(opacityNode);

	xmlNodePtr widthNode = xmlNewNode(NULL,BAD_CAST "Width");
	xmlAddChild(node,widthNode);
	if(width)
		width->Save(widthNode);

	xmlNodePtr modelLocateNode = xmlNewNode(NULL,BAD_CAST "ModelLocate");
	xmlAddChild(node,modelLocateNode);
	if(modelLocate)
		modelLocate->Save(modelLocateNode);
}

void GlbLineModelSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderLine")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		sscanf_s((char*)szKey,"%d",&isRenderLine);
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Width")))
	{
		width = new GlbRenderDouble();
		width->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ModelLocate")))
	{
		modelLocate = new GlbRenderString();
		modelLocate->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(modelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(modelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			modelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = modelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				modelLocate->value += L"\\";
			modelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
}

void GlbShapeInfo::Save(xmlNodePtr node)
{
	char str[64];
	sprintf_s(str,"%d",shapeType);
	xmlNewTextChild(node, NULL, BAD_CAST "ShapeType", BAD_CAST str);
}

void GlbShapeInfo::Load(xmlNodePtr* node)
{
	xmlNodePtr pnode = *node;
	xmlChar* szKey;
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"ShapeType")))
	{
		szKey = xmlNodeGetContent(pnode);	
		sscanf_s((char*)szKey,"%d",&shapeType);
		xmlFree(szKey);
		pnode = pnode->next;
	}
}
void GlbCircleInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr radiusNode = xmlNewNode(NULL,BAD_CAST "Radius");
	xmlAddChild(node,radiusNode);
	if(radius)radius->Save(radiusNode);

	xmlNodePtr edgesNode = xmlNewNode(NULL,BAD_CAST "Edges");
	xmlAddChild(node,edgesNode);
	if(edges)edges->Save(edgesNode);
}
void GlbCircleInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);	
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbRectangleInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr widthNode = xmlNewNode(NULL,BAD_CAST "Width");
	xmlAddChild(node,widthNode);
	if(width)width->Save(widthNode);

	xmlNodePtr heightNode = xmlNewNode(NULL,BAD_CAST "Height");
	xmlAddChild(node,heightNode);
	if(height)height->Save(heightNode);
}
void GlbRectangleInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);	
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Width")))
	{
		width = new GlbRenderDouble();
		width->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Height")))
	{
		height = new GlbRenderDouble();
		height->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbEllipseInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr xRadiusNode = xmlNewNode(NULL,BAD_CAST "XRadius");
	xmlAddChild(node,xRadiusNode);
	if(xRadius)xRadius->Save(xRadiusNode);

	xmlNodePtr yRadiusNode = xmlNewNode(NULL,BAD_CAST "YRadius");
	xmlAddChild(node,yRadiusNode);
	if(yRadius)yRadius->Save(yRadiusNode);

	xmlNodePtr edgesNode = xmlNewNode(NULL,BAD_CAST "Edges");
	xmlAddChild(node,edgesNode);
	if(edges)edges->Save(edgesNode);
}
void GlbEllipseInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);	
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"XRadius")))
	{
		xRadius = new GlbRenderDouble();
		xRadius->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"YRadius")))
	{
		yRadius = new GlbRenderDouble();
		yRadius->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbArcInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr xRadiusNode = xmlNewNode(NULL,BAD_CAST "XRadius");
	xmlAddChild(node,xRadiusNode);
	if(xRadius)xRadius->Save(xRadiusNode);

	xmlNodePtr yRadiusNode = xmlNewNode(NULL,BAD_CAST "YRadius");
	xmlAddChild(node,yRadiusNode);
	if(yRadius)yRadius->Save(yRadiusNode);

	xmlNodePtr edgesNode = xmlNewNode(NULL,BAD_CAST "Edges");
	xmlAddChild(node,edgesNode);
	if(edges)edges->Save(edgesNode);

	xmlNodePtr sAngleNode = xmlNewNode(NULL,BAD_CAST "SAngle");
	xmlAddChild(node,sAngleNode);
	if(sAngle)sAngle->Save(sAngleNode);

	xmlNodePtr eAngleNode = xmlNewNode(NULL,BAD_CAST "EAngle");
	xmlAddChild(node,eAngleNode);
	if(eAngle)eAngle->Save(eAngleNode);

	xmlNodePtr isFanNode = xmlNewNode(NULL,BAD_CAST "IsFan");
	xmlAddChild(node,isFanNode);
	if(isFan)isFan->Save(isFanNode);
}
void GlbArcInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);	
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"XRadius")))
	{
		xRadius = new GlbRenderDouble();
		xRadius->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"YRadius")))
	{
		yRadius = new GlbRenderDouble();
		yRadius->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"SAngle")))
	{
		sAngle = new GlbRenderDouble();
		sAngle->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"EAngle")))
	{
		eAngle = new GlbRenderDouble();
		eAngle->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"IsFan")))
	{
		isFan = new GlbRenderBool();
		isFan->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbSphereInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr radiusNode = xmlNewNode(NULL,BAD_CAST "Radius");
	xmlAddChild(node,radiusNode);
	if(radius)radius->Save(radiusNode);
}
void GlbSphereInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbBoxInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr lengthNode = xmlNewNode(NULL,BAD_CAST "Length");
	xmlAddChild(node,lengthNode);
	if(length)length->Save(lengthNode);

	xmlNodePtr widthNode = xmlNewNode(NULL,BAD_CAST "Width");
	xmlAddChild(node,widthNode);
	if(width)width->Save(widthNode);

	xmlNodePtr heightNode = xmlNewNode(NULL,BAD_CAST "Height");
	xmlAddChild(node,heightNode);
	if(height)height->Save(heightNode);
}

void GlbBoxInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Length")))
	{
		length = new GlbRenderDouble();
		length->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Width")))
	{
		width = new GlbRenderDouble();
		width->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Height")))
	{
		height = new GlbRenderDouble();
		height->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbPyramidInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr lengthNode = xmlNewNode(NULL,BAD_CAST "Length");
	xmlAddChild(node,lengthNode);
	if(length)length->Save(lengthNode);

	xmlNodePtr widthNode = xmlNewNode(NULL,BAD_CAST "Width");
	xmlAddChild(node,widthNode);
	if(width)width->Save(widthNode);

	xmlNodePtr heightNode = xmlNewNode(NULL,BAD_CAST "Height");
	xmlAddChild(node,heightNode);
	if(height)height->Save(heightNode);
}
void GlbPyramidInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Length")))
	{
		length = new GlbRenderDouble();
		length->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Width")))
	{
		width = new GlbRenderDouble();
		width->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Height")))
	{
		height = new GlbRenderDouble();
		height->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbConeInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr radiusNode = xmlNewNode(NULL,BAD_CAST "Radius");
	xmlAddChild(node,radiusNode);
	if(radius)radius->Save(radiusNode);

	xmlNodePtr edgesNode = xmlNewNode(NULL,BAD_CAST "Edges");
	xmlAddChild(node,edgesNode);
	if(edges)edges->Save(edgesNode);

	xmlNodePtr heightNode = xmlNewNode(NULL,BAD_CAST "Height");
	xmlAddChild(node,heightNode);
	if(height)height->Save(heightNode);
}
void GlbConeInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);	
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Height")))
	{
		height = new GlbRenderDouble();
		height->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbPieInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr radiusNode = xmlNewNode(NULL,BAD_CAST "Radius");
	xmlAddChild(node,radiusNode);
	if(radius)radius->Save(radiusNode);

	xmlNodePtr edgesNode = xmlNewNode(NULL,BAD_CAST "Edges");
	xmlAddChild(node,edgesNode);
	if(edges)edges->Save(edgesNode);

	xmlNodePtr sAngleNode = xmlNewNode(NULL,BAD_CAST "SAngle");
	xmlAddChild(node,sAngleNode);
	if(sAngle)sAngle->Save(sAngleNode);

	xmlNodePtr eAngleNode = xmlNewNode(NULL,BAD_CAST "EAngle");
	xmlAddChild(node,eAngleNode);
	if(eAngle)eAngle->Save(eAngleNode);

	xmlNodePtr heightNode = xmlNewNode(NULL,BAD_CAST "Height");
	xmlAddChild(node,heightNode);
	if(height)height->Save(heightNode);
}
void GlbPieInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"SAngle")))
	{
		sAngle = new GlbRenderDouble();
		sAngle->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"EAngle")))
	{
		eAngle = new GlbRenderDouble();
		eAngle->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Height")))
	{
		height = new GlbRenderDouble();
		height->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbCylinderInfo::Save(xmlNodePtr node)
{
	GlbShapeInfo::Save(node);

	xmlNodePtr radiusNode = xmlNewNode(NULL,BAD_CAST "Radius");
	xmlAddChild(node,radiusNode);
	if(radius)radius->Save(radiusNode);

	xmlNodePtr edgesNode = xmlNewNode(NULL,BAD_CAST "Edges");
	xmlAddChild(node,edgesNode);
	if(edges)edges->Save(edgesNode);

	xmlNodePtr heightNode = xmlNewNode(NULL,BAD_CAST "Height");
	xmlAddChild(node,heightNode);
	if(height)height->Save(heightNode);
}
void GlbCylinderInfo::Load(xmlNodePtr* node)
{
	GlbShapeInfo::Load(node);
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Height")))
	{
		height = new GlbRenderDouble();
		height->Load(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

//GlbRenderTextInfo
void GlbRenderTextInfo::Save(xmlNodePtr node)
{
	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color)color->Save(colorNode);

	xmlNodePtr sizeNode = xmlNewNode(NULL,BAD_CAST "Size");
	xmlAddChild(node,sizeNode);
	if(size)size->Save(sizeNode);

	xmlNodePtr fontNode = xmlNewNode(NULL,BAD_CAST "Font");
	xmlAddChild(node,fontNode);
	if(font)font->Save(fontNode);

	xmlNodePtr contentNode = xmlNewNode(NULL,BAD_CAST "Content");
	xmlAddChild(node,contentNode);
	if(content)content->Save(contentNode);

	xmlNodePtr isBoldNode = xmlNewNode(NULL,BAD_CAST "IsBold");
	xmlAddChild(node,isBoldNode);
	if(isBold)isBold->Save(isBoldNode);

	xmlNodePtr isUnderlineNode = xmlNewNode(NULL,BAD_CAST "IsUnderline");
	xmlAddChild(node,isUnderlineNode);
	if(isUnderline)isUnderline->Save(isUnderlineNode);

	xmlNodePtr isItalicNode = xmlNewNode(NULL,BAD_CAST "IsItalic");
	xmlAddChild(node,isItalicNode);
	if(isItalic)isItalic->Save(isItalicNode);

	xmlNodePtr backColorNode = xmlNewNode(NULL,BAD_CAST "BackColor");
	xmlAddChild(node,backColorNode);
	if(backColor)backColor->Save(backColorNode);

	xmlNodePtr backOpacityNode = xmlNewNode(NULL,BAD_CAST "BackOpacity");
	xmlAddChild(node,backOpacityNode);
	if(backOpacity)backOpacity->Save(backOpacityNode);

	xmlNodePtr multilineAlignNode = xmlNewNode(NULL,BAD_CAST "MultilineAlign");
	xmlAddChild(node,multilineAlignNode);
	if(multilineAlign)multilineAlign->Save(multilineAlignNode);
}
void GlbRenderTextInfo::Load(xmlNodePtr node)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Size")))
	{
		size = new GlbRenderInt32();
		size->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Font")))
	{
		font = new GlbRenderString();
		font->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Content")))
	{
		content = new GlbRenderString();
		content->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsBold")))
	{
		isBold = new GlbRenderBool();
		isBold->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUnderline")))
	{
		isUnderline = new GlbRenderBool();
		isUnderline->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsItalic")))
	{
		isItalic = new GlbRenderBool();
		isItalic->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"BackColor")))
	{
		backColor = new GlbRenderColor();
		backColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"BackOpacity")))
	{
		backOpacity = new GlbRenderInt32();
		backOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"MultilineAlign")))
	{
		multilineAlign = new GlbRenderMultilineAlign();
		multilineAlign->Load(node->xmlChildrenNode);
		node = node->next;
	}
}
//GlbRenderImageInfo
void GlbRenderImageInfo::Save(xmlNodePtr node,const glbWChar* prjPath)
{
	xmlNodePtr dataSourceNode = xmlNewNode(NULL,BAD_CAST "DataSource");
	xmlAddChild(node,dataSourceNode);
	if(dataSource)
	{
		if(dataSource->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = dataSource->bUseField;

			CGlbWString apath = CGlbPath::GetDir(dataSource->value);
			CGlbWString filename = CGlbPath::GetFileName(dataSource->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),prjPath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}

			rdstring.Save(dataSourceNode);
		}else{
			dataSource->Save(dataSourceNode);
		}		
	}

	xmlNodePtr maxSizeNode = xmlNewNode(NULL,BAD_CAST "MaxSize");
	xmlAddChild(node,maxSizeNode);
	if(maxSize)maxSize->Save(maxSizeNode);

	xmlNodePtr opacityNode = xmlNewNode(NULL,BAD_CAST "Opacity");
	xmlAddChild(node,opacityNode);
	if(opacity)opacity->Save(opacityNode);

	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color)color->Save(colorNode);
}
void GlbRenderImageInfo::Load(xmlNodePtr node,const glbWChar* prjPath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"DataSource")))
	{
		dataSource = new GlbRenderString();
		dataSource->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(dataSource->value);
		CGlbWString filename = CGlbPath::GetFileName(dataSource->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),prjPath);
		if(filename.length()>0 && apath)
		{
			dataSource->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = dataSource->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				dataSource->value += L"\\";
			dataSource->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"MaxSize")))
	{
		maxSize = new GlbRenderInt32();
		maxSize->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbRenderLabelRelationInfo::Save(xmlNodePtr node)
{
	xmlNodePtr relationNode = xmlNewNode(NULL,BAD_CAST "Relation");
	xmlAddChild(node,relationNode);
	if(relation)relation->Save(relationNode);

	xmlNodePtr alignNode = xmlNewNode(NULL,BAD_CAST "Align");
	xmlAddChild(node,alignNode);
	if(align)align->Save(alignNode);
}
void GlbRenderLabelRelationInfo::Load(xmlNodePtr node)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Relation")))
	{
		relation = new GlbRenderLabelRelation();
		relation->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Align")))
	{
		align = new GlbRenderLabelAlign();
		align->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbRenderGeoInfo::Save(xmlNodePtr node)
{
	xmlNodePtr isLimitNode = xmlNewNode(NULL,BAD_CAST "IsLimit");
	xmlAddChild(node,isLimitNode);
	if(isLimit)isLimit->Save(isLimitNode);

	xmlNodePtr ratioNode = xmlNewNode(NULL,BAD_CAST "Ratio");
	xmlAddChild(node,ratioNode);
	if(ratio)ratio->Save(ratioNode);
}
void GlbRenderGeoInfo::Load(xmlNodePtr node)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsLimit")))
	{
		isLimit = new GlbRenderBool();
		isLimit->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Ratio")))
	{
		ratio = new GlbRenderDouble();
		ratio->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbRenderSimpleLabel::Save(xmlNodePtr node)
{
	xmlNodePtr bShowLabelNode = xmlNewNode(NULL,BAD_CAST "bShowLabel");
	xmlAddChild(node,bShowLabelNode);
	if(bShow)
		bShow->Save(bShowLabelNode);
	xmlNodePtr xOffsetNode = xmlNewNode(NULL,BAD_CAST "xOffset");
	xmlAddChild(node,xOffsetNode);
	if(xOffset)
		xOffset->Save(xOffsetNode);
	xmlNodePtr yOffsetNode = xmlNewNode(NULL,BAD_CAST "yOffset");
	xmlAddChild(node,yOffsetNode);
	if(yOffset)
		yOffset->Save(yOffsetNode);
	xmlNodePtr zOffsetNode = xmlNewNode(NULL,BAD_CAST "zOffset");
	xmlAddChild(node,zOffsetNode);
	if(zOffset)
		zOffset->Save(zOffsetNode);
	xmlNodePtr modeNode = xmlNewNode(NULL,BAD_CAST "mode");
	xmlAddChild(node,modeNode);
	if(mode)
		mode->Save(modeNode);
	xmlNodePtr labelAlignNode = xmlNewNode(NULL,BAD_CAST "labelAlign");
	xmlAddChild(node,labelAlignNode);
	if(labelAlign)
		labelAlign->Save(labelAlignNode);
	xmlNodePtr textInfoNode = xmlNewNode(NULL,BAD_CAST "textInfo");
	xmlAddChild(node,textInfoNode);
	if(textInfo)
		textInfo->Save(textInfoNode);
	xmlNodePtr geoInfoNode = xmlNewNode(NULL,BAD_CAST "geoInfo");
	xmlAddChild(node,geoInfoNode);
	if(geoInfo)
		geoInfo->Save(geoInfoNode);
}
void GlbRenderSimpleLabel::Load(xmlNodePtr node)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bShowLabel")))
	{
		bShow = new GlbRenderBool();
		bShow->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"mode")))
	{
		mode = new GlbRenderBillboard();
		mode->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"labelAlign")))
	{
		labelAlign = new GlbRenderLabelAlign();
		labelAlign->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"textInfo")))
	{
		textInfo = new GlbRenderTextInfo();
		textInfo->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"geoInfo")))
	{
		geoInfo = new GlbRenderGeoInfo();
		geoInfo->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

GlbRenderInfo* glbLoadRenderInfo(xmlNodePtr node,glbWChar* relativepath)
{
	xmlChar* szKey = NULL;
	GlbGlobeObjectTypeEnum type=GLB_OBJECTTYPE_UNKNOW;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RenderType")))
	{		
		szKey = xmlNodeGetContent(node);
		sscanf_s((char*)szKey,"%d",&type);
		xmlFree(szKey);
		node = node->next;
	}
	switch(type)
	{
	case GLB_OBJECTTYPE_POINT:   ///<点
		{
			GlbMarkerSymbolTypeEnum symType;
			if (node && (!xmlStrcmp(node->name, (const xmlChar *)"SymbolType")))
			{				
				szKey = xmlNodeGetContent(node);
				sscanf_s((char*)szKey,"%d",&symType);
				xmlFree(szKey);
			}
			node = node->next;
			switch(symType)
			{
			case GLB_MARKERSYMBOL_MODEL:
				{
					GlbMarkerModelSymbolInfo * renderinfo = new GlbMarkerModelSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_2DSHAPE:
				{
					GlbMarker2DShapeSymbolInfo *renderinfo = new GlbMarker2DShapeSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_3DSHAPE:
				{
					GlbMarker3DShapeSymbolInfo *renderinfo = new GlbMarker3DShapeSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_PIXEL:
				{
					GlbMarkerPixelSymbolInfo *renderinfo = new GlbMarkerPixelSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_LABEL:
				{
					GlbMarkerLabelSymbolInfo *renderinfo = new GlbMarkerLabelSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_IMAGE:
				{
					GlbMarkerImageSymbolInfo *renderinfo = new GlbMarkerImageSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			}
		}break;
	case GLB_OBJECTTYPE_LINE:   ///<线
		{
			GlbGlobeLineSymbolTypeEnum symType;
			if (node && (!xmlStrcmp(node->name, (const xmlChar *)"SymbolType")))
			{				
				szKey = xmlNodeGetContent(node);
				sscanf_s((char*)szKey,"%d",&symType);
				xmlFree(szKey);
			}
			node = node->next;
			switch(symType)
			{
			case GLB_LINESYMBOL_PIXEL:
				{
					GlbLinePixelSymbolInfo * renderinfo = new GlbLinePixelSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_LINESYMBOL_2D:
				{
					GlbLine2DSymbolInfo *renderinfo = new GlbLine2DSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_LINESYMBOL_3D:
				{
					GlbLine3DSymbolInfo *renderinfo = new GlbLine3DSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_LINESYMBOL_ARROW:
				{
					GlbLineArrowSymbolInfo *renderinfo = new GlbLineArrowSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			}
		}break;
	case GLB_OBJECTTYPE_POLYGON:///<多边形
		{
			GlbGlobePolygonSymbolTypeEnum symType;
			if (node && (!xmlStrcmp(node->name, (const xmlChar *)"SymbolType")))
			{				
				szKey = xmlNodeGetContent(node);
				sscanf_s((char*)szKey,"%d",&symType);
				xmlFree(szKey);
			}
			node = node->next;
			switch(symType)
			{
			case GLB_POLYGONSYMBOL_2D:
				{
					GlbPolygon2DSymbolInfo* renderinfo = new GlbPolygon2DSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_POLYGONSYMBOL_POLYHEDRON:
				{
					GlbPolyhedronSymbolInfo *renderinfo = new GlbPolyhedronSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_POLYGONSYMBOL_BUILDING:
				{
					GlbPolygonBuildingSymbolInfo *renderinfo = new GlbPolygonBuildingSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			case GLB_POLYGONSYMBOL_WATER:
				{
					GlbPolygonWaterSymbolInfo *renderinfo = new GlbPolygonWaterSymbolInfo();
					renderinfo->Load(node,relativepath);
					return renderinfo;
				}break;
			}
		}break;
		case GLB_OBJECTTYPE_DRILL:
			{
				GlbDrillRenderInfo *renderinfo = new GlbDrillRenderInfo();
				renderinfo->Load(node,relativepath);
				return renderinfo;
			}break;
		case GLB_OBJECTTYPE_SECTION:
			{
				GlbSectionRenderInfo *renderInfo = new GlbSectionRenderInfo;
				renderInfo->Load(node,relativepath);
				return renderInfo;
			}break;
		case GLB_OBJECTTYPE_TIN:
			{
				GlbTinSymbolInfo *renderInfo = new GlbTinSymbolInfo;
				renderInfo->Load(node,relativepath);
				return renderInfo;
			}break;
		case GLB_OBJECTTYPE_NETWORK:
			{
				GlbNetworkRenderInfo *renderInfo = new GlbNetworkRenderInfo;
				renderInfo->Load(node,relativepath);
				return renderInfo;
			}break;
	}
	return NULL;
}

GlbGlobe::GlbRenderInfo* glbLoadRenderInfo2( xmlNodePtr node,glbWChar* relativepath )
{
	xmlChar* szKey = NULL;
	GlbGlobeObjectTypeEnum type=GLB_OBJECTTYPE_UNKNOW;
	xmlNodePtr childNode = NULL;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"type")))
	{
		childNode = node->children;//text
		szKey = xmlNodeGetContent(childNode);
		char* str = u2g((char*)szKey);
		CGlbString glbtype = str;
		if(glbtype == "NGLB_OBJECTTYPE_POINT")
			type = GLB_OBJECTTYPE_POINT;
		else if(glbtype == "NGLB_OBJECTTYPE_LINE")
			type = GLB_OBJECTTYPE_LINE;
		else if(glbtype == "NGLB_OBJECTTYPE_POLYGON")
			type = GLB_OBJECTTYPE_POLYGON;
		else if(glbtype == "NGLB_OBJECTTYPE_TIN")
			type = GLB_OBJECTTYPE_TIN;
		else if(glbtype == "NGLB_OBJECTTYPE_DYNAMIC")
			type = GLB_OBJECTTYPE_DYNAMIC;
		else if(glbtype == "NGLB_OBJECTTYPE_MODIFYTERRIN")
			type = GLB_OBJECTTYPE_MODIFYTERRIN;
		else if(glbtype == "NGLB_OBJECTTYPE_DIGHOLE")
			type = GLB_OBJECTTYPE_DIGHOLE;
		else if(glbtype == "NGLB_OBJECTTYPE_COMPLEX")
			type = GLB_OBJECTTYPE_COMPLEX;
		else if(glbtype == "NGLB_OBJECTTYPE_VISUALLINEANALYSIS")
			type = GLB_OBJECTTYPE_VISUALLINEANALYSIS;
		else if(glbtype == "NGLB_OBJECTTYPE_VIEWANALYSIS")
			type = GLB_OBJECTTYPE_VIEWANALYSIS;
		else if(glbtype == "NGLB_OBJECTTYPE_VIDEO")
			type = GLB_OBJECTTYPE_VIDEO;
		else if(glbtype == "NGLB_OBJECTTYPE_DRILL")
			type = GLB_OBJECTTYPE_DRILL;
		else if(glbtype == "NGLB_OBJECTTYPE_SECTION")
			type = GLB_OBJECTTYPE_SECTION;
		else if(glbtype == "NGLB_OBJECTTYPE_NETWORK")
			type = GLB_OBJECTTYPE_NETWORK;
		else if(glbtype == "NGLB_OBJECTTYPE_COORDINAT_AXIS")
			type = GLB_OBJECTTYPE_COORDINAT_AXIS;
		xmlFree(szKey);
		node = node->next;//text
		node = node->next;//symbolType
	}
	switch(type)
	{
	case GLB_OBJECTTYPE_POINT:   ///<点
		{
			GlbMarkerSymbolTypeEnum symType;
			if (node && (!xmlStrcmp(node->name, (const xmlChar *)"symType")))
			{				
				szKey = xmlNodeGetContent(node);
				char* str = u2g((char*)szKey);
				CGlbString glbsymType = str;
				if(glbsymType == "NGLB_MARKERSYMBOL_PIXEL")
					symType = GLB_MARKERSYMBOL_PIXEL;
				else if(glbsymType == "NGLB_MARKERSYMBOL_MODEL")
					symType = GLB_MARKERSYMBOL_MODEL;
				else if(glbsymType == "NGLB_MARKERSYMBOL_2DSHAPE")
					symType = GLB_MARKERSYMBOL_2DSHAPE;
				else if(glbsymType == "NGLB_MARKERSYMBOL_3DSHAPE")
					symType = GLB_MARKERSYMBOL_3DSHAPE;
				else if(glbsymType == "NGLB_MARKERSYMBOL_LABEL")
					symType = GLB_MARKERSYMBOL_LABEL;
				else if(glbsymType == "NGLB_MARKERSYMBOL_IMAGE")
					symType = GLB_MARKERSYMBOL_IMAGE;
				else if(glbsymType == "NGLB_MARKERSYMBOL_FIRE")
					symType = GLB_MARKERSYMBOL_FIRE;
				else if(glbsymType == "NGLB_MARKERSYMBOL_SMOKE")
					symType = GLB_MARKERSYMBOL_SMOKE;
				else if(glbsymType == "NGLB_MARKERSYMBOL_EPICENTRE")
					symType = GLB_MARKERSYMBOL_EPICENTRE;
				else if(glbsymType == "NGLB_MARKERSYMBOL_DYNAMICLABEL")
					symType = GLB_MARKERSYMBOL_DYNAMICLABEL;
				xmlFree(szKey);
			}
			node = node->next;
			switch(symType)
			{
			case GLB_MARKERSYMBOL_MODEL:
				{
					GlbMarkerModelSymbolInfo * renderinfo = new GlbMarkerModelSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_2DSHAPE:
				{
					GlbMarker2DShapeSymbolInfo *renderinfo = new GlbMarker2DShapeSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_3DSHAPE:
				{
					GlbMarker3DShapeSymbolInfo *renderinfo = new GlbMarker3DShapeSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_PIXEL:
				{
					GlbMarkerPixelSymbolInfo *renderinfo = new GlbMarkerPixelSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_LABEL:
				{
					GlbMarkerLabelSymbolInfo *renderinfo = new GlbMarkerLabelSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_MARKERSYMBOL_IMAGE:
				{
					GlbMarkerImageSymbolInfo *renderinfo = new GlbMarkerImageSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			}
		}break;
	case GLB_OBJECTTYPE_LINE:   ///<线
		{
			GlbGlobeLineSymbolTypeEnum symType;
			if (node && (!xmlStrcmp(node->name, (const xmlChar *)"symType")))
			{				
				szKey = xmlNodeGetContent(node);
				sscanf_s((char*)szKey,"%d",&symType);
				xmlFree(szKey);
			}
			node = node->next;
			switch(symType)
			{
			case GLB_LINESYMBOL_PIXEL:
				{
					GlbLinePixelSymbolInfo * renderinfo = new GlbLinePixelSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_LINESYMBOL_2D:
				{
					GlbLine2DSymbolInfo *renderinfo = new GlbLine2DSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_LINESYMBOL_3D:
				{
					GlbLine3DSymbolInfo *renderinfo = new GlbLine3DSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_LINESYMBOL_ARROW:
				{
					GlbLineArrowSymbolInfo *renderinfo = new GlbLineArrowSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			}
		}break;
	case GLB_OBJECTTYPE_POLYGON:///<多边形
		{
			GlbGlobePolygonSymbolTypeEnum symType;
			if (node && (!xmlStrcmp(node->name, (const xmlChar *)"symType")))
			{
				szKey = xmlNodeGetContent(node);
				char* str = u2g((char*)szKey);
				CGlbString glbsymType = str;
				if(glbsymType == "NGLB_POLYGONSYMBOL_2D")
					symType = GLB_POLYGONSYMBOL_2D;
				else if(glbsymType == "NGLB_POLYGONSYMBOL_POLYHEDRON")
					symType = GLB_POLYGONSYMBOL_POLYHEDRON;
				else if(glbsymType == "NGLB_POLYGONSYMBOL_BUILDING")
					symType = GLB_POLYGONSYMBOL_BUILDING;
				else if(glbsymType == "NGLB_POLYGONSYMBOL_WATER")
					symType = GLB_POLYGONSYMBOL_WATER;
				xmlFree(szKey);
			}
			node = node->next;
			switch(symType)
			{
			case GLB_POLYGONSYMBOL_2D:
				{
					GlbPolygon2DSymbolInfo* renderinfo = new GlbPolygon2DSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_POLYGONSYMBOL_POLYHEDRON:
				{
					GlbPolyhedronSymbolInfo *renderinfo = new GlbPolyhedronSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_POLYGONSYMBOL_BUILDING:
				{
					GlbPolygonBuildingSymbolInfo *renderinfo = new GlbPolygonBuildingSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			case GLB_POLYGONSYMBOL_WATER:
				{
					GlbPolygonWaterSymbolInfo *renderinfo = new GlbPolygonWaterSymbolInfo();
					renderinfo->Load2(node,relativepath);
					return renderinfo;
				}break;
			}
		}break;
	case GLB_OBJECTTYPE_DRILL:
		{
			GlbDrillRenderInfo *renderinfo = new GlbDrillRenderInfo();
			renderinfo->Load2(node,relativepath);
			return renderinfo;
		}break;
	case GLB_OBJECTTYPE_SECTION:
		{
			GlbSectionRenderInfo *renderInfo = new GlbSectionRenderInfo;
			renderInfo->Load2(node,relativepath);
			return renderInfo;
		}break;
	case GLB_OBJECTTYPE_TIN:
		{
			GlbTinSymbolInfo *renderInfo = new GlbTinSymbolInfo;
			renderInfo->Load2(node,relativepath);
			return renderInfo;
		}break;
	case GLB_OBJECTTYPE_NETWORK:
		{
			GlbNetworkRenderInfo *renderInfo = new GlbNetworkRenderInfo;
			renderInfo->Load2(node,relativepath);
			return renderInfo;
		}break;
	}
	return NULL;
}

void GlbDigHoleSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr lineInfoNode = xmlNewNode(NULL,BAD_CAST "LineInfo");
	xmlAddChild(node,lineInfoNode);
	if(lineInfo)
		lineInfo->Save(lineInfoNode);

	xmlNodePtr sideTextureDataNode = xmlNewNode(NULL,BAD_CAST "sideTextureData");
	xmlAddChild(node,sideTextureDataNode);
	if(sideTextureData)
		sideTextureData->Save(sideTextureDataNode);

	xmlNodePtr belowTextureDataNode = xmlNewNode(NULL,BAD_CAST "belowTextureData");
	xmlAddChild(node,belowTextureDataNode);
	if(belowTextureData)
		belowTextureData->Save(belowTextureDataNode);
}
void GlbDigHoleSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LineInfo")))
	{
		lineInfo = new GlbLinePixelSymbolInfo();
		lineInfo->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"sideTextureData")))
	{
		sideTextureData = new GlbRenderString();
		sideTextureData->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"belowTextureData")))
	{
		belowTextureData = new GlbRenderString();
		belowTextureData->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbVideoSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	char str[32];
	sprintf_s(str,"%d",symType);
	xmlNewTextChild(node, NULL, BAD_CAST "symbolType", BAD_CAST str);

	xmlNodePtr videofilenode = xmlNewNode(NULL,BAD_CAST "videofile");
	xmlAddChild(node,videofilenode);
	if(videofile)
	{
		if(videofile->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = videofile->bUseField;

			CGlbWString apath = CGlbPath::GetDir(videofile->value);
			CGlbWString filename = CGlbPath::GetFileName(videofile->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)				
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}

			rdstring.Save(videofilenode);
		}else{
			videofile->Save(videofilenode);
		}		
	}

	xmlNodePtr opacityNode = xmlNewNode(NULL,BAD_CAST "opacity");
	xmlAddChild(node,opacityNode);
	if(opacity)
		opacity->Save(opacityNode);

	xmlNodePtr bvoiceNode = xmlNewNode(NULL,BAD_CAST "bvoice");
	xmlAddChild(node,bvoiceNode);
	if(bvoice)
		bvoice->Save(bvoiceNode);

	xmlNodePtr volumeNode = xmlNewNode(NULL,BAD_CAST "volume");
	xmlAddChild(node,volumeNode);
	if(volume)
		volume->Save(volumeNode);

	xmlNodePtr bPauseNode = xmlNewNode(NULL,BAD_CAST "bPause");
	xmlAddChild(node,bPauseNode);
	if(bPause)
		bPause->Save(bPauseNode);

	xmlNodePtr referenceTimeNode = xmlNewNode(NULL,BAD_CAST "referenceTime");
	xmlAddChild(node,referenceTimeNode);
	if(referenceTime)
		referenceTime->Save(referenceTimeNode);

	xmlNodePtr bLoopNode = xmlNewNode(NULL,BAD_CAST "bLoop");
	xmlAddChild(node,bLoopNode);
	if(bLoop)
		bLoop->Save(bLoopNode);

	xmlNodePtr bRewindNode = xmlNewNode(NULL,BAD_CAST "bRewind");
	xmlAddChild(node,bRewindNode);
	if(bRewind)
		bRewind->Save(bRewindNode);

	xmlNodePtr xOffsetNode = xmlNewNode(NULL,BAD_CAST "xOffset");
	xmlAddChild(node,xOffsetNode);
	if(xOffset)
		xOffset->Save(xOffsetNode);

	xmlNodePtr yOffsetNode = xmlNewNode(NULL,BAD_CAST "yOffset");
	xmlAddChild(node,yOffsetNode);
	if(yOffset)
		yOffset->Save(yOffsetNode);

	xmlNodePtr zOffsetNode = xmlNewNode(NULL,BAD_CAST "zOffset");
	xmlAddChild(node,zOffsetNode);
	if(zOffset)
		zOffset->Save(zOffsetNode);

	xmlNodePtr xScaleNode = xmlNewNode(NULL,BAD_CAST "xScale");
	xmlAddChild(node,xScaleNode);
	if(xScale)
		xScale->Save(xScaleNode);

	xmlNodePtr yScaleNode = xmlNewNode(NULL,BAD_CAST "yScale");
	xmlAddChild(node,yScaleNode);
	if(yScale)
		yScale->Save(yScaleNode);
}

void GlbVideoSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"symbolType")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		sscanf_s((char*)szKey,"%d",&symType);
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"videofile")))
	{
		videofile = new GlbRenderString();
		videofile->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(videofile->value);
		CGlbWString filename = CGlbPath::GetFileName(videofile->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			videofile->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = videofile->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				videofile->value += L"\\";
			videofile->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"opacity")))
	{
		opacity = new GlbRenderDouble();
		opacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bvoice")))
	{
		bvoice = new GlbRenderBool();
		bvoice->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"volume")))
	{
		volume = new GlbRenderDouble();
		volume->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bPause")))
	{
		bPause = new GlbRenderBool();
		bPause->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"referenceTime")))
	{
		referenceTime = new GlbRenderDouble();
		referenceTime->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bLoop")))
	{
		bLoop = new GlbRenderBool();
		bLoop->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bRewind")))
	{
		bRewind = new GlbRenderBool();
		bRewind->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbDrillRenderInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	char str[32];
	sprintf_s(str,"%d",isRenderBaseLine);
	xmlNewTextChild(node, NULL, BAD_CAST "isRenderBaseline", BAD_CAST str);

	xmlNodePtr blColorNode = xmlNewNode(NULL,BAD_CAST "baselineColor");
	xmlAddChild(node,blColorNode);
	if(baselineColor)
		baselineColor->Save(blColorNode);

	xmlNodePtr blOpacityNode = xmlNewNode(NULL,BAD_CAST "baselineOpacity");
	xmlAddChild(node,blOpacityNode);
	if(baselineOpacity)
		baselineOpacity->Save(blOpacityNode);

	xmlNodePtr blWidthNode = xmlNewNode(NULL,BAD_CAST "baselineWidth");
	xmlAddChild(node,blWidthNode);
	if(baselineWidth)
		baselineWidth->Save(blWidthNode);

	xmlNodePtr blModelLocateNode = xmlNewNode(NULL,BAD_CAST "baselineModelLocate");
	xmlAddChild(node,blModelLocateNode);
	if(baselineModelLocate)
	{		
		if(baselineModelLocate->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = baselineModelLocate->bUseField;

			CGlbWString apath = CGlbPath::GetDir(baselineModelLocate->value);
			CGlbWString filename = CGlbPath::GetFileName(baselineModelLocate->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)					
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}
			rdstring.Save(blModelLocateNode);
		}else{
			baselineModelLocate->Save(blModelLocateNode);
		}		
	}

	sprintf_s(str,"%d",isRenderStratum);
	xmlNewTextChild(node, NULL, BAD_CAST "isRenderStratum", BAD_CAST str);

	xmlNodePtr stColorNode = xmlNewNode(NULL,BAD_CAST "stratumColor");
	xmlAddChild(node,stColorNode);
	if(stratumColor)
		stratumColor->Save(stColorNode);

	xmlNodePtr stOpacityNode = xmlNewNode(NULL,BAD_CAST "stratumOpacity");
	xmlAddChild(node,stOpacityNode);
	if(stratumOpacity)
		stratumOpacity->Save(stOpacityNode);

	xmlNodePtr stWidthNode = xmlNewNode(NULL,BAD_CAST "stratumWidth");
	xmlAddChild(node,stWidthNode);
	if(stratumWidth)
		stratumWidth->Save(stWidthNode);

	xmlNodePtr stModelNode = xmlNewNode(NULL,BAD_CAST "stratumModelLocate");
	xmlAddChild(node,stModelNode);	
	if(stratumModelLocate)
	{		
		if(stratumModelLocate->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = stratumModelLocate->bUseField;

			CGlbWString apath = CGlbPath::GetDir(stratumModelLocate->value);
			CGlbWString filename = CGlbPath::GetFileName(stratumModelLocate->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)				
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}
			rdstring.Save(stModelNode);
		}else{
			stratumModelLocate->Save(stModelNode);
		}		
	}

	xmlNodePtr xOffsetNode = xmlNewNode(NULL,BAD_CAST "xOffset");
	xmlAddChild(node,xOffsetNode);
	if(xOffset) xOffset->Save(xOffsetNode);

	xmlNodePtr yOffsetNode = xmlNewNode(NULL,BAD_CAST "yOffset");
	xmlAddChild(node,yOffsetNode);
	if(yOffset) yOffset->Save(yOffsetNode);

	xmlNodePtr zOffsetNode = xmlNewNode(NULL,BAD_CAST "zOffset");
	xmlAddChild(node,zOffsetNode);
	if(zOffset) zOffset->Save(zOffsetNode);
}

void GlbDrillRenderInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderBaseline")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		sscanf_s((char*)szKey,"%d",&isRenderBaseLine);
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"baselineColor")))
	{
		baselineColor = new GlbRenderColor();
		baselineColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"baselineOpacity")))
	{
		baselineOpacity = new GlbRenderInt32();
		baselineOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"baselineWidth")))
	{
		baselineWidth = new GlbRenderDouble();
		baselineWidth->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"baselineModelLocate")))
	{
		baselineModelLocate = new GlbRenderString();
		baselineModelLocate->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(baselineModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(baselineModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			baselineModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = baselineModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				baselineModelLocate->value += L"\\";
			baselineModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderStratum")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		sscanf_s((char*)szKey,"%d",&isRenderStratum);
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"stratumColor")))
	{
		stratumColor = new GlbRenderColor();
		stratumColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"stratumOpacity")))
	{
		stratumOpacity = new GlbRenderInt32();
		stratumOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"stratumWidth")))
	{
		stratumWidth = new GlbRenderDouble();
		stratumWidth->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"stratumModelLocate")))
	{
		stratumModelLocate = new GlbRenderString();
		stratumModelLocate->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(stratumModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(stratumModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			stratumModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = stratumModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				stratumModelLocate->value += L"\\";
			stratumModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbSectionRenderInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	char str[32];
	sprintf_s(str,"%d",isRenderSec);
	xmlNewTextChild(node, NULL, BAD_CAST "isRenderSection", BAD_CAST str);

	xmlNodePtr secColorNode = xmlNewNode(NULL,BAD_CAST "secColor");
	xmlAddChild(node,secColorNode);
	if(secColor)
		secColor->Save(secColorNode);

	xmlNodePtr secOpacityNode = xmlNewNode(NULL,BAD_CAST "secOpacity");
	xmlAddChild(node,secOpacityNode);
	if(secOpacity)
		secOpacity->Save(secOpacityNode);

	xmlNodePtr secTextureDataNode = xmlNewNode(NULL,BAD_CAST "secTextureData");
	xmlAddChild(node,secTextureDataNode);	
	if(secTextureData)
	{		
		if(secTextureData->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = secTextureData->bUseField;

			CGlbWString apath = CGlbPath::GetDir(secTextureData->value);
			CGlbWString filename = CGlbPath::GetFileName(secTextureData->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)	
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}
			rdstring.Save(secTextureDataNode);
		}else{
			secTextureData->Save(secTextureDataNode);
		}		
	}

	xmlNodePtr secTexRepeatModeNode = xmlNewNode(NULL,BAD_CAST "secTexRepeatMode");
	xmlAddChild(node,secTexRepeatModeNode);
	if(secTexRepeatMode)
		secTexRepeatMode->Save(secTexRepeatModeNode);

	xmlNodePtr secTilingUNode = xmlNewNode(NULL,BAD_CAST "secTilingU");
	xmlAddChild(node,secTilingUNode);
	if(secTilingU)
		secTilingU->Save(secTilingUNode);

	xmlNodePtr secTilingVNode = xmlNewNode(NULL,BAD_CAST "secTilingV");
	xmlAddChild(node,secTilingVNode);
	if(secTilingV)
		secTilingV->Save(secTilingVNode);

	xmlNodePtr secTextureRotationNode = xmlNewNode(NULL,BAD_CAST "secTextureRotation");
	xmlAddChild(node,secTextureRotationNode);
	if(secTextureRotation)
		secTextureRotation->Save(secTextureRotationNode);

	xmlNodePtr secModelLocateNode = xmlNewNode(NULL,BAD_CAST "secModelLocate");
	xmlAddChild(node,secModelLocateNode);	
	if(secModelLocate)
	{		
		if(secModelLocate->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = secModelLocate->bUseField;

			CGlbWString apath = CGlbPath::GetDir(secModelLocate->value);
			CGlbWString filename = CGlbPath::GetFileName(secModelLocate->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)					
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}
			rdstring.Save(secModelLocateNode);
		}else{
			secModelLocate->Save(secModelLocateNode);
		}		
	}

	sprintf_s(str,"%d",isRenderArc);
	xmlNewTextChild(node, NULL, BAD_CAST "isRenderArc", BAD_CAST str);

	xmlNodePtr arcColorNode = xmlNewNode(NULL,BAD_CAST "arcColor");
	xmlAddChild(node,arcColorNode);
	if(arcColor)
		arcColor->Save(arcColorNode);

	xmlNodePtr arcOpacityNode = xmlNewNode(NULL,BAD_CAST "arcOpacity");
	xmlAddChild(node,arcOpacityNode);
	if(arcOpacity)
		arcOpacity->Save(arcOpacityNode);

	xmlNodePtr arcWidthNode = xmlNewNode(NULL,BAD_CAST "arcWidth");
	xmlAddChild(node,arcWidthNode);
	if(arcWidth)
		arcWidth->Save(arcWidthNode);

	sprintf_s(str,"%d",isRenderPoly);
	xmlNewTextChild(node, NULL, BAD_CAST "isRenderPoly", BAD_CAST str);

	xmlNodePtr polyColorNode = xmlNewNode(NULL,BAD_CAST "polyColor");
	xmlAddChild(node,polyColorNode);
	if(polyColor)
		polyColor->Save(polyColorNode);

	xmlNodePtr polyOpacityNode = xmlNewNode(NULL,BAD_CAST "polyOpacity");
	xmlAddChild(node,polyOpacityNode);
	if(polyOpacity)
		polyOpacity->Save(polyOpacityNode);

	xmlNodePtr polyTextureDataNode = xmlNewNode(NULL,BAD_CAST "polyTextureData");
	xmlAddChild(node,polyTextureDataNode);
	if(polyTextureData)
	{		
		if(polyTextureData->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = polyTextureData->bUseField;

			CGlbWString apath = CGlbPath::GetDir(polyTextureData->value);
			CGlbWString filename = CGlbPath::GetFileName(polyTextureData->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)	
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}
			rdstring.Save(polyTextureDataNode);
		}else{
			polyTextureData->Save(polyTextureDataNode);
		}		
	}

	xmlNodePtr polyTexRepeatModeNode = xmlNewNode(NULL,BAD_CAST "polyTexRepeatMode");
	xmlAddChild(node,polyTexRepeatModeNode);
	if(polyTexRepeatMode)
		polyTexRepeatMode->Save(polyTexRepeatModeNode);

	xmlNodePtr polyTilingUNode = xmlNewNode(NULL,BAD_CAST "polyTilingU");
	xmlAddChild(node,polyTilingUNode);
	if(polyTilingU)
		polyTilingU->Save(polyTilingUNode);

	xmlNodePtr polyTilingVNode = xmlNewNode(NULL,BAD_CAST "polyTilingV");
	xmlAddChild(node,polyTilingVNode);
	if(polyTilingV)
		polyTilingV->Save(polyTilingVNode);

	xmlNodePtr polyTextureRotationNode = xmlNewNode(NULL,BAD_CAST "polyTextureRotation");
	xmlAddChild(node,polyTextureRotationNode);
	if(polyTextureRotation)
		polyTextureRotation->Save(polyTextureRotationNode);

	xmlNodePtr polyModelLocateNode = xmlNewNode(NULL,BAD_CAST "polyModelLocate");
	xmlAddChild(node,polyModelLocateNode);
	if(polyModelLocate)
	{		
		if(polyModelLocate->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = polyModelLocate->bUseField;

			CGlbWString apath = CGlbPath::GetDir(polyModelLocate->value);
			CGlbWString filename = CGlbPath::GetFileName(polyModelLocate->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)	
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}
			rdstring.Save(polyModelLocateNode);
		}else{
			polyModelLocate->Save(polyModelLocateNode);
		}		
	}

	xmlNodePtr xOffsetNode = xmlNewNode(NULL,BAD_CAST "xOffset");
	xmlAddChild(node,xOffsetNode);
	if(xOffset) xOffset->Save(xOffsetNode);

	xmlNodePtr yOffsetNode = xmlNewNode(NULL,BAD_CAST "yOffset");
	xmlAddChild(node,yOffsetNode);
	if(yOffset) yOffset->Save(yOffsetNode);

	xmlNodePtr zOffsetNode = xmlNewNode(NULL,BAD_CAST "zOffset");
	xmlAddChild(node,zOffsetNode);
	if(zOffset) zOffset->Save(zOffsetNode);
}

void GlbSectionRenderInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderSection")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		sscanf_s((char*)szKey,"%d",&isRenderSec);
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secColor")))
	{
		secColor = new GlbRenderColor();
		secColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secOpacity")))
	{
		secOpacity = new GlbRenderInt32();
		secOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTextureData")))
	{
		secTextureData = new GlbRenderString();
		secTextureData->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(secTextureData->value);
		CGlbWString filename = CGlbPath::GetFileName(secTextureData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			secTextureData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = secTextureData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				secTextureData->value += L"\\";
			secTextureData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTexRepeatMode")))
	{
		secTexRepeatMode = new GlbRenderTexRepeatMode();
		secTexRepeatMode->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTilingU")))
	{
		secTilingU = new GlbRenderInt32();
		secTilingU->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTilingV")))
	{
		secTilingV = new GlbRenderInt32();
		secTilingV->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTextureRotation")))
	{
		secTextureRotation = new GlbRenderDouble();
		secTextureRotation->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secModelLocate")))
	{
		secModelLocate = new GlbRenderString();
		secModelLocate->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(secModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(secModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			secModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = secModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				secModelLocate->value += L"\\";
			secModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderArc")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		sscanf_s((char*)szKey,"%d",&isRenderArc);
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"arcColor")))
	{
		arcColor = new GlbRenderColor();
		arcColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"arcOpacity")))
	{
		arcOpacity = new GlbRenderInt32();
		arcOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"arcWidth")))
	{
		arcWidth = new GlbRenderDouble();
		arcWidth->Load(node->xmlChildrenNode);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderPoly")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		sscanf_s((char*)szKey,"%d",&isRenderPoly);
		xmlFree(szKey);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyColor")))
	{
		polyColor = new GlbRenderColor();
		polyColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyOpacity")))
	{
		polyOpacity = new GlbRenderInt32();
		polyOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTextureData")))
	{
		polyTextureData = new GlbRenderString();
		polyTextureData->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(polyTextureData->value);
		CGlbWString filename = CGlbPath::GetFileName(polyTextureData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			polyTextureData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = polyTextureData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				polyTextureData->value += L"\\";
			polyTextureData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTexRepeatMode")))
	{
		polyTexRepeatMode = new GlbRenderTexRepeatMode();
		polyTexRepeatMode->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTilingU")))
	{
		polyTilingU = new GlbRenderInt32();
		polyTilingU->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTilingV")))
	{
		polyTilingV = new GlbRenderInt32();
		polyTilingV->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTextureRotation")))
	{
		polyTextureRotation = new GlbRenderDouble();
		polyTextureRotation->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyModelLocate")))
	{
		polyModelLocate = new GlbRenderString();
		polyModelLocate->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(polyModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(polyModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			polyModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = polyModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				polyModelLocate->value += L"\\";
			polyModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbNetworkRenderInfo::Save(xmlNodePtr node, const glbWChar* relativepath)
{
	xmlNodePtr edgeColorNode = xmlNewNode(NULL,BAD_CAST "edgeColor");
	xmlAddChild(node,edgeColorNode);
	if(edgeColor)
		edgeColor->Save(edgeColorNode);

	xmlNodePtr edgeOpacityNode = xmlNewNode(NULL,BAD_CAST "edgeOpacity");
	xmlAddChild(node,edgeOpacityNode);
	if(edgeOpacity)
		edgeOpacity->Save(edgeOpacityNode);

	xmlNodePtr edgeLineWidthNode = xmlNewNode(NULL,BAD_CAST "edgeLineWidth");
	xmlAddChild(node,edgeLineWidthNode);
	if(edgeLineWidth)
		edgeLineWidth->Save(edgeLineWidthNode);

	xmlNodePtr edgeModelLocateNode = xmlNewNode(NULL,BAD_CAST "edgeModelLocate");
	xmlAddChild(node,edgeModelLocateNode);	
	if(edgeModelLocate)
	{		
		if(edgeModelLocate->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = edgeModelLocate->bUseField;

			CGlbWString apath = CGlbPath::GetDir(edgeModelLocate->value);
			CGlbWString filename = CGlbPath::GetFileName(edgeModelLocate->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)	
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}
			rdstring.Save(edgeModelLocateNode);
		}else{
			edgeModelLocate->Save(edgeModelLocateNode);
		}		
	}

	xmlNodePtr nodeColorNode = xmlNewNode(NULL,BAD_CAST "nodeColor");
	xmlAddChild(node,nodeColorNode);
	if(nodeColor)
		nodeColor->Save(nodeColorNode);

	xmlNodePtr nodeOpacityNode = xmlNewNode(NULL,BAD_CAST "nodeOpacity");
	xmlAddChild(node,nodeOpacityNode);
	if(nodeOpacity)
		nodeOpacity->Save(nodeOpacityNode);

	xmlNodePtr nodeSizeNode = xmlNewNode(NULL,BAD_CAST "nodeSize");
	xmlAddChild(node,nodeSizeNode);
	if(nodeSize)
		nodeSize->Save(nodeSizeNode);

	xmlNodePtr isRenderFromNodeNode = xmlNewNode(NULL,BAD_CAST "isRenderFromNode");
	xmlAddChild(node,isRenderFromNodeNode);
	if(isRenderFromNode)
		isRenderFromNode->Save(isRenderFromNodeNode);

	xmlNodePtr isRenderToNodeNode = xmlNewNode(NULL,BAD_CAST "isRenderToNode");
	xmlAddChild(node,isRenderToNodeNode);
	if(isRenderToNode)
		isRenderToNode->Save(isRenderToNodeNode);

	xmlNodePtr fromNodeModelLocateNode = xmlNewNode(NULL,BAD_CAST "fromNodeModelLocate");
	xmlAddChild(node,fromNodeModelLocateNode);	
	if(fromNodeModelLocate)
	{		
		if(fromNodeModelLocate->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = fromNodeModelLocate->bUseField;

			CGlbWString apath = CGlbPath::GetDir(fromNodeModelLocate->value);
			CGlbWString filename = CGlbPath::GetFileName(fromNodeModelLocate->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)	
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}
			rdstring.Save(fromNodeModelLocateNode);
		}else{
			fromNodeModelLocate->Save(fromNodeModelLocateNode);
		}		
	}

	xmlNodePtr toNodeModelLocateNode = xmlNewNode(NULL,BAD_CAST "toNodeModelLocate");
	xmlAddChild(node,toNodeModelLocateNode);	
	if(toNodeModelLocate)
	{		
		if(toNodeModelLocate->bUseField == false)
		{
			GlbRenderString rdstring;
			rdstring.bUseField = toNodeModelLocate->bUseField;

			CGlbWString apath = CGlbPath::GetDir(toNodeModelLocate->value);
			CGlbWString filename = CGlbPath::GetFileName(toNodeModelLocate->value);
			glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
			if(rpath)
			{
				rdstring.value = rpath;
				// 判断是否需要在路径后面添加 "\"
				std::string _path = rdstring.value.ToString().c_str();
				glbBool needExpand = true;
				if (_path.length() > 0 &&
					(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
					needExpand = false;

				if (rdstring.value != L"" && needExpand)
					rdstring.value += L"\\";
				rdstring.value += filename;
				delete rpath;
			}
			rdstring.Save(toNodeModelLocateNode);
		}else{
			toNodeModelLocate->Save(toNodeModelLocateNode);
		}		
	}

}

void GlbNetworkRenderInfo::Load(xmlNodePtr node, const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"edgeColor")))
	{
		edgeColor = new GlbRenderColor();
		edgeColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"edgeOpacity")))
	{
		edgeOpacity = new GlbRenderInt32();
		edgeOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"edgeLineWidth")))
	{
		edgeLineWidth = new GlbRenderDouble();
		edgeLineWidth->Load(node->xmlChildrenNode);
		node = node->next;
	}	

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"edgeModelLocate")))
	{
		edgeModelLocate = new GlbRenderString();
		edgeModelLocate->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(edgeModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(edgeModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			edgeModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = edgeModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				edgeModelLocate->value += L"\\";
			edgeModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"nodeColor")))
	{
		nodeColor = new GlbRenderColor();
		nodeColor->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"nodeOpacity")))
	{
		nodeOpacity = new GlbRenderInt32();
		nodeOpacity->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"nodeSize")))
	{
		nodeSize = new GlbRenderDouble();
		nodeSize->Load(node->xmlChildrenNode);
		node = node->next;
	}	
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderFromNode")))
	{
		isRenderFromNode = new GlbRenderBool();
		isRenderFromNode->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderToNode")))
	{
		isRenderToNode = new GlbRenderBool();
		isRenderToNode->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"fromNodeModelLocate")))
	{
		fromNodeModelLocate = new GlbRenderString();
		fromNodeModelLocate->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(fromNodeModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(fromNodeModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			fromNodeModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = fromNodeModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				fromNodeModelLocate->value += L"\\";
			fromNodeModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"toNodeModelLocate")))
	{
		toNodeModelLocate = new GlbRenderString();
		toNodeModelLocate->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(toNodeModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(toNodeModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			toNodeModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = toNodeModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				toNodeModelLocate->value += L"\\";
			toNodeModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
}

void GlbTinSymbolInfo::Save( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color) color->Save(colorNode);

	xmlNodePtr opacityNode = xmlNewNode(NULL,BAD_CAST "Opacity");
	xmlAddChild(node,opacityNode);
	if(opacity) opacity->Save(opacityNode);

	xmlNodePtr locateNode = xmlNewNode(NULL,BAD_CAST "Locate");
	xmlAddChild(node,locateNode);
	if(locate) locate->Save(locateNode);

	xmlNodePtr xScaleNode = xmlNewNode(NULL,BAD_CAST "xScale");
	xmlAddChild(node,xScaleNode);
	if(xScale) xScale->Save(xScaleNode);

	xmlNodePtr yScaleNode = xmlNewNode(NULL,BAD_CAST "yScale");
	xmlAddChild(node,yScaleNode);
	if(yScale) yScale->Save(yScaleNode);

	xmlNodePtr zScaleNode = xmlNewNode(NULL,BAD_CAST "zScale");
	xmlAddChild(node,zScaleNode);
	if(zScale) zScale->Save(zScaleNode);

	xmlNodePtr xOffsetNode = xmlNewNode(NULL,BAD_CAST "xOffset");
	xmlAddChild(node,xOffsetNode);
	if(xOffset) xOffset->Save(xOffsetNode);

	xmlNodePtr yOffsetNode = xmlNewNode(NULL,BAD_CAST "yOffset");
	xmlAddChild(node,yOffsetNode);
	if(yOffset) yOffset->Save(yOffsetNode);

	xmlNodePtr zOffsetNode = xmlNewNode(NULL,BAD_CAST "zOffset");
	xmlAddChild(node,zOffsetNode);
	if(zOffset) zOffset->Save(zOffsetNode);

	xmlNodePtr bSmoothingNode = xmlNewNode(NULL,BAD_CAST "bSmoothing");
	xmlAddChild(node,bSmoothingNode);
	if(bSmoothing) bSmoothing->Save(bSmoothingNode);
}

void GlbTinSymbolInfo::Load( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Locate")))
	{
		locate = new GlbRenderString();
		locate->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zScale")))
	{
		zScale = new GlbRenderDouble();
		zScale->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bSmoothing")))
	{
		bSmoothing = new GlbRenderBool();
		bSmoothing->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbTinWaterSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	char str[32];
	printf_s(str,"%d",type);
	xmlNewTextChild(node, NULL, BAD_CAST "RenderType", BAD_CAST str);

	sprintf_s(str,"%d",symType);
	xmlNewTextChild(node, NULL, BAD_CAST "SymbolType", BAD_CAST str);

	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color)color->Save(colorNode);

	xmlNodePtr opacityNode = xmlNewNode(NULL,BAD_CAST "Opacity");
	xmlAddChild(node,opacityNode);
	if(opacity)opacity->Save(opacityNode);

	xmlNodePtr isUseEnvNode = xmlNewNode(NULL,BAD_CAST "IsUseEnv");
	xmlAddChild(node,isUseEnvNode);
	if(isUseEnv)isUseEnv->Save(isUseEnvNode);

	xmlNodePtr windStrongNode = xmlNewNode(NULL,BAD_CAST "WindStrong");
	xmlAddChild(node,windStrongNode);
	if(windStrong)windStrong->Save(windStrongNode);

	xmlNodePtr windDirectionNode = xmlNewNode(NULL,BAD_CAST "WindDirection");
	xmlAddChild(node,windDirectionNode);
	if(windDirection)windDirection->Save(windDirectionNode);

	xmlNodePtr enableReflectionsNode = xmlNewNode(NULL,BAD_CAST "EnableReflections");
	xmlAddChild(node,enableReflectionsNode);
	if(enableReflections)enableReflections->Save(enableReflectionsNode);

	xmlNodePtr enableRefractionsNode = xmlNewNode(NULL,BAD_CAST "EnableRefractions");
	xmlAddChild(node,enableRefractionsNode);
	if(enableRefractions)enableRefractions->Save(enableRefractionsNode);

	xmlNodePtr refractionImageDataNode = xmlNewNode(NULL,BAD_CAST "RefractionImageData");
	xmlAddChild(node,refractionImageDataNode);
	if(refractionImageData->bUseField == false)
	{
		GlbRenderString rdstring;
		rdstring.bUseField = refractionImageData->bUseField;

		CGlbWString apath = CGlbPath::GetDir(refractionImageData->value);
		CGlbWString filename = CGlbPath::GetFileName(refractionImageData->value);
		glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
		if(rpath)
		{
			rdstring.value = rpath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = rdstring.value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (rdstring.value != L"" && needExpand)
				rdstring.value += L"\\";
			rdstring.value += filename;
			delete rpath;
		}
		rdstring.Save(refractionImageDataNode);
	}else{
		refractionImageData->Save(refractionImageDataNode);
	}	

	xmlNodePtr refractTexRepeatModeNode = xmlNewNode(NULL,BAD_CAST "RefractTexRepeatMode");
	xmlAddChild(node,refractTexRepeatModeNode);
	if(refractTexRepeatMode)refractTexRepeatMode->Save(refractTexRepeatModeNode);

	xmlNodePtr refractTextilingUNode = xmlNewNode(NULL,BAD_CAST "RefractTextilingU");
	xmlAddChild(node,refractTextilingUNode);
	if(refractTextilingU)refractTextilingU->Save(refractTextilingUNode);

	xmlNodePtr refractTextilingVNode = xmlNewNode(NULL,BAD_CAST "RefractTextilingV");
	xmlAddChild(node,refractTextilingVNode);
	if(refractTextilingV)refractTextilingV->Save(refractTextilingVNode);

	xmlNodePtr refractTexRotationNode = xmlNewNode(NULL,BAD_CAST "RefractTexRotation");
	xmlAddChild(node,refractTexRotationNode);
	if(refractTexRotation)refractTexRotation->Save(refractTexRotationNode);
}
void GlbTinWaterSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseEnv")))
	{
		isUseEnv = new GlbRenderBool();
		isUseEnv->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"WindStrong")))
	{
		windStrong = new GlbRenderDouble();
		windStrong->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"WindDirection")))
	{
		windDirection = new GlbRenderDouble();
		windDirection->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"EnableReflections")))
	{
		enableReflections = new GlbRenderBool();
		enableReflections->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"EnableRefractions")))
	{
		enableRefractions = new GlbRenderBool();
		enableRefractions->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractionImageData")))
	{
		refractionImageData = new GlbRenderString();
		refractionImageData->Load(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(refractionImageData->value);
		CGlbWString filename = CGlbPath::GetFileName(refractionImageData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			refractionImageData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = refractionImageData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				refractionImageData->value += L"\\";
			refractionImageData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractTexRepeatMode")))
	{
		refractTexRepeatMode = new GlbRenderTexRepeatMode();
		refractTexRepeatMode->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractTextilingU")))
	{
		refractTextilingU = new GlbRenderInt32();
		refractTextilingU->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractTextilingV")))
	{
		refractTextilingV = new GlbRenderInt32();
		refractTextilingV->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"RefractTexRotation")))
	{
		refractTexRotation = new GlbRenderDouble();
		refractTexRotation->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbMarkerFireSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr scalenode = xmlNewNode(NULL,BAD_CAST "Scale");
	xmlAddChild(node,scalenode);
	if(scale)scale->Save(scalenode);

	xmlNodePtr intensitynode = xmlNewNode(NULL,BAD_CAST "Intensity");
	xmlAddChild(node,intensitynode);
	if(intensity)intensity->Save(intensitynode);
}
void GlbMarkerFireSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Scale")))
	{
		scale = new GlbRenderDouble();
		scale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Intensity")))
	{
		intensity = new GlbRenderDouble();
		intensity->Load(node->xmlChildrenNode);

		node = node->next;
	}
}

void GlbMarkerSmokeSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr startcolornode = xmlNewNode(NULL,BAD_CAST "StartColor");
	xmlAddChild(node,startcolornode);
	if(startColor)startColor->Save(startcolornode);

	xmlNodePtr endcolornode = xmlNewNode(NULL,BAD_CAST "EndColor");
	xmlAddChild(node,endcolornode);
	if(endColor)endColor->Save(endcolornode);

	xmlNodePtr scalenode = xmlNewNode(NULL,BAD_CAST "Scale");
	xmlAddChild(node,scalenode);
	if(scale)scale->Save(scalenode);

	xmlNodePtr intensitynode = xmlNewNode(NULL,BAD_CAST "Intensity");
	xmlAddChild(node,intensitynode);
	if(intensity)intensity->Save(intensitynode);
}
void GlbMarkerSmokeSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"StartColor")))
	{
		startColor = new GlbRenderColor();
		startColor->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"EndColor")))
	{
		endColor = new GlbRenderColor();
		endColor->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Scale")))
	{
		scale = new GlbRenderDouble();
		scale->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Intensity")))
	{
		intensity = new GlbRenderDouble();
		intensity->Load(node->xmlChildrenNode);

		node = node->next;
	}
}

void GlbMarkerEpicentreSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlNodePtr radiusnode = xmlNewNode(NULL,BAD_CAST "Radius");
	xmlAddChild(node,radiusnode);
	if(radius)radius->Save(radiusnode);

	xmlNodePtr colornode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colornode);
	if(color)color->Save(colornode);
}
void GlbMarkerEpicentreSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load(node->xmlChildrenNode);

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);

		node = node->next;
	}
}


#include <fstream>
#include <osg/Material>
#include <osg/Geometry>
#include <osg/LightModel>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osgDB/WriteFile>

CGlbGlobeDataExchange::CGlbGlobeDataExchange()
{

}

bool CGlbGlobeDataExchange::tinToOsgFile(glbWChar* tinfile, glbWChar* osgFile, glbInt32 tinColor, glbBool isGlobe)
{	
	CGlbWString _tinFile(tinfile);
	CGlbWString _osgFile(osgFile);
	std::fstream cin(_tinFile.ToString().c_str(),std::ios::in);

	if (cin)
	{
		double ptx,pty,ptz;
		glbInt32 first_ptidx,second_ptidx,third_ptidx;
		osg::Vec3 normal;
		osg::Vec3d drawPoint,centerPoint;	
		osg::Vec3d pt;
		centerPoint.set(0,0,0);

		int ptNum, triNum;
		int itemp;
		double dtemp;
		char str[255]; 

		cin.getline(str,255);	// Creatar Tin 2
		cin.getline(str,255);	// TIN BM0001
		//cin >> str >> str >> str;
		//cin >> str >> str;

		cin >> ptNum >> triNum >> itemp;
		// tin有效性判断
		if (ptNum<3 || triNum < 1)
			return false;

		cin.precision(12);//浮点数的数字个数为12个
		osg::ref_ptr<osg::Vec3Array> drawPoints = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array(ptNum);
		for (int i =0; i < ptNum; i++)
		{
			cin >> itemp >> ptx >> pty >> ptz >> dtemp;
			
			if(isGlobe)
			{
				g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
					pt.x(),pt.y(),pt.z());

				drawPoint.set(pt.x(),pt.y(),pt.z());
			}
			else			
				drawPoint.set(ptx,pty,ptz);
			
			if (i==0)
				centerPoint = drawPoint;		
			drawPoints->push_back(drawPoint - centerPoint);
		}
		
		osg::ref_ptr<osg::UIntArray> indexArray = new osg::UIntArray();
		for (int j =0; j < triNum; j++)
		{
			cin >> itemp >> first_ptidx >> second_ptidx >> third_ptidx >> itemp >> itemp >> itemp;

			indexArray->push_back(first_ptidx);
			indexArray->push_back(second_ptidx);
			indexArray->push_back(third_ptidx);

			//计算三角形的法线
			osg::Vec3 vec0 = drawPoints->at(first_ptidx);
			osg::Vec3 vec1 = drawPoints->at(second_ptidx);
			osg::Vec3 vec2 = drawPoints->at(third_ptidx);
			normal = (vec1-vec0)^(vec2-vec0);
			normal.normalize();
			{// 法线求平均
				normalArray->at(first_ptidx) += normal;
				normalArray->at(second_ptidx) += normal;
				normalArray->at(third_ptidx) += normal;
			}		
		}

		for(int k = 0; k < ptNum; k++)
			normalArray->at(k).normalize();

		osg::ref_ptr<osg::DrawElementsUInt> primitiveSet = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,indexArray->size(),&indexArray->front());
		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
		osg::Matrix m;
		m.makeTranslate(centerPoint);
		mt->setMatrix(m);
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		geometry->setVertexArray(drawPoints.get());
		geometry->setNormalArray(normalArray.get());
		geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

		geometry->addPrimitiveSet(primitiveSet.get());
		geode->addDrawable(geometry.get());
		mt->addChild(geode.get());

		// 不止一个tin
		while (!cin.eof())
		{// 如果没有结束,说明有多个tin文件
			cin >> str >> str;			
			//cin.getline(str,255);	// TIN BM0001
			cin >> ptNum >> triNum >> itemp;
			osg::ref_ptr<osg::Vec3Array> drawPoints = new osg::Vec3Array();
			osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array(ptNum);
			for (int i =0; i < ptNum; i++)
			{
				cin >> itemp >> ptx >> pty >> ptz >> dtemp;

				if(isGlobe)
				{
					g_ellipsmodel->convertLatLongHeightToXYZ(osg::DegreesToRadians(pty),osg::DegreesToRadians(ptx),ptz,
						pt.x(),pt.y(),pt.z());

					drawPoint.set(pt.x(),pt.y(),pt.z());
				}
				else			
					drawPoint.set(ptx,pty,ptz);
				drawPoints->push_back(drawPoint - centerPoint);
			}

			osg::ref_ptr<osg::UIntArray> indexArray = new osg::UIntArray();
			for (int j =0; j < triNum; j++)
			{
				cin >> itemp >> first_ptidx >> second_ptidx >> third_ptidx >> itemp >> itemp >> itemp;

				indexArray->push_back(first_ptidx);
				indexArray->push_back(second_ptidx);
				indexArray->push_back(third_ptidx);

				//计算三角形的法线
				osg::Vec3 vec0 = drawPoints->at(first_ptidx);
				osg::Vec3 vec1 = drawPoints->at(second_ptidx);
				osg::Vec3 vec2 = drawPoints->at(third_ptidx);
				normal = (vec1-vec0)^(vec2-vec0);
				normal.normalize();
				{// 法线求平均
					normalArray->at(first_ptidx) += normal;
					normalArray->at(second_ptidx) += normal;
					normalArray->at(third_ptidx) += normal;
				}		
			}

			for(int k = 0; k < ptNum; k++)
				normalArray->at(k).normalize();

			osg::ref_ptr<osg::DrawElementsUInt> primitiveSet = new osg::DrawElementsUInt(osg::DrawElementsUInt::TRIANGLES,indexArray->size(),&indexArray->front());
			osg::ref_ptr<osg::Geode> geode = new osg::Geode;
			osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
			geometry->setVertexArray(drawPoints.get());
			geometry->setNormalArray(normalArray.get());
			geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

			geometry->addPrimitiveSet(primitiveSet.get());
		
			//if(alpha < 255)
			//{
			//	ss->setMode(GL_BLEND,osg::StateAttribute::ON);
			//	if(ss->getRenderingHint() != osg::StateSet::TRANSPARENT_BIN)
			//		ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);			
			//}
			geode->addDrawable(geometry.get());

			mt->addChild(geode.get());
		}
		cin.close();	

		osg::ref_ptr<osg::StateSet> ss = mt->getOrCreateStateSet();

		osg::ref_ptr<osg::Material> material = new osg::Material;
		glbInt32 blue = LOBYTE(tinColor);//GetRValue(color);
		glbInt32 green = LOBYTE(((tinColor & 0xFFFF)) >> 8);//GetGValue(color);
		glbInt32 red = LOBYTE((tinColor)>>16);//GetBValue(color);
		glbInt32 alpha = LOBYTE((tinColor)>>24);//GetAValue(color);
		osg::Vec4 color = osg::Vec4(red / 255.0,green / 255.0,blue / 255.0, 1.0);		
		material->setDiffuse(osg::Material::FRONT_AND_BACK,color);
		//material->setAlpha(osg::Material::FRONT_AND_BACK,alpha/255.0);
		ss->setAttribute(material,osg::StateAttribute::ON);

		osg::ref_ptr<osg::LightModel> lm = new osg::LightModel;
		lm->setTwoSided(true);
		lm->setAmbientIntensity(color);
		ss->setAttribute(lm.get(),osg::StateAttribute::ON);
		// 启用光照
		ss->setMode(GL_LIGHTING,osg::StateAttribute::ON);

		osg::ref_ptr<osgDB::Options> opt = new osgDB::Options();
		//浮点数输出的数字个数为12个
		opt->setOptionString("PRECISION 12");
		osgDB::writeNodeFile(*(mt.get()),_osgFile.ToString().c_str(),opt.get());

		return true;
	}
	return false;
}

void GlbGlobe::GlbMarkerDynamicLabelSymbolInfo::Save( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	xmlNodePtr modeNode = xmlNewNode(NULL,BAD_CAST "Mode");
	xmlAddChild(node,modeNode);
	if(mode)mode->Save(modeNode);

	xmlNodePtr labelAlignNode = xmlNewNode(NULL,BAD_CAST "LabelAlign");
	xmlAddChild(node,labelAlignNode);
	if(labelAlign)labelAlign->Save(labelAlignNode);

	xmlNodePtr textInfoNode = xmlNewNode(NULL,BAD_CAST "TextInfo");
	xmlAddChild(node,textInfoNode);
	if(textInfo)textInfo->Save(textInfoNode);

	xmlNodePtr imageInfoNode = xmlNewNode(NULL,BAD_CAST "ImageInfo");
	xmlAddChild(node,imageInfoNode);
	if(imageInfo)imageInfo->Save(imageInfoNode,relativepath);

	xmlNodePtr geoInfoNode = xmlNewNode(NULL,BAD_CAST "GeoInfo");
	xmlAddChild(node,geoInfoNode);
	if(geoInfo)geoInfo->Save(geoInfoNode);

	xmlNodePtr yawNode = xmlNewNode(NULL,BAD_CAST "Yaw");
	xmlAddChild(node,yawNode);
	if(yaw)yaw->Save(yawNode);

	xmlNodePtr pitchNode = xmlNewNode(NULL,BAD_CAST "Pitch");
	xmlAddChild(node,pitchNode);
	if(pitch)pitch->Save(pitchNode);

	xmlNodePtr rollNode = xmlNewNode(NULL,BAD_CAST "Roll");
	xmlAddChild(node,rollNode);
	if(roll)roll->Save(rollNode);

	xmlNodePtr xOffsetNode = xmlNewNode(NULL,BAD_CAST "XOffset");
	xmlAddChild(node,xOffsetNode);
	if(xOffset)xOffset->Save(xOffsetNode);

	xmlNodePtr yOffsetNode = xmlNewNode(NULL,BAD_CAST "YOffset");
	xmlAddChild(node,yOffsetNode);
	if(yOffset)yOffset->Save(yOffsetNode);

	xmlNodePtr zOffsetNode = xmlNewNode(NULL,BAD_CAST "ZOffset");
	xmlAddChild(node,zOffsetNode);
	if(zOffset)zOffset->Save(zOffsetNode);

	xmlNodePtr fadeModeNode = xmlNewNode(NULL,BAD_CAST "FadeMode");
	xmlAddChild(node,fadeModeNode);
	if(fadeMode)fadeMode->Save(fadeModeNode);

	xmlNodePtr fadeSpeedNode = xmlNewNode(NULL,BAD_CAST "FadeSpeed");
	xmlAddChild(node,fadeSpeedNode);
	if(fadeSpeed)fadeSpeed->Save(fadeSpeedNode);
}

void GlbGlobe::GlbMarkerDynamicLabelSymbolInfo::Load( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Mode")))
	{
		mode = new GlbRenderBillboard();
		mode->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LabelAlign")))
	{
		labelAlign = new GlbRenderLabelAlign();
		labelAlign->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"TextInfo")))
	{
		textInfo = new GlbRenderTextInfo();
		textInfo->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ImageInfo")))
	{
		imageInfo = new GlbRenderImageInfo();
		imageInfo->Load(node->xmlChildrenNode,relativepath);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"GeoInfo")))
	{
		geoInfo = new GlbRenderGeoInfo();
		geoInfo->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"XOffset")))
	{
		xOffset = new GlbRenderDouble;
		xOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"YOffset")))
	{
		yOffset = new GlbRenderDouble;
		yOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"ZOffset")))
	{
		zOffset = new GlbRenderDouble;
		zOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"FadeMode")))
	{
		fadeMode = new GlbRenderDynamicLabelFadeMode;
		fadeMode->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"FadeSpeed")))
	{
		fadeSpeed = new GlbRenderDynamicLabelFadeSpeed;
		fadeSpeed->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbMarkerDynamicLabelSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{

}

bool GlbGlobe::GlbRenderDynamicLabelFadeMode::operator!=( const GlbRenderDynamicLabelFadeMode& r ) const
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

void GlbGlobe::GlbRenderDynamicLabelFadeMode::Save( xmlNodePtr node )
{
	char str[16];
	sprintf_s(str,"%c",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbGlobe::GlbRenderDynamicLabelFadeMode::Load( xmlNodePtr node )
{
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bUseField);		
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);
		xmlFree(szKey);
		node = node->next;
	}
}

glbInt32 GlbGlobe::GlbRenderDynamicLabelFadeMode::GetValue( CGlbFeature *feature )
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

bool GlbGlobe::GlbRenderDynamicLabelFadeSpeed::operator!=( const GlbRenderDynamicLabelFadeSpeed& r ) const
{
	return bUseField != r.bUseField ||
		value != r.value || field != r.field;
}

glbInt32 GlbGlobe::GlbRenderDynamicLabelFadeSpeed::GetValue( CGlbFeature *feature )
{
	if (feature)
	{
		if (bUseField == true)
		{
			const GLBVARIANT* va = feature->GetValueByName(field.c_str());
			if (va && va->isnull == false && va->vt == GLB_DATATYPE_INT32)
				return va->lVal;
		}
	}
	return value;
}

void GlbGlobe::GlbRenderDynamicLabelFadeSpeed::Save( xmlNodePtr node )
{
	char str[16];
	sprintf_s(str,"%c",bUseField);
	xmlNewTextChild(node, NULL, BAD_CAST "IsUseField", BAD_CAST str);

	if (bUseField)
	{
		char* szOut = g2u((char*)field.ToString().c_str());
		xmlNewTextChild(node, NULL, BAD_CAST "Field", BAD_CAST szOut);
		free(szOut);
	}
	else
	{
		sprintf_s(str,"%d",value);
		xmlNewTextChild(node, NULL, BAD_CAST "Value", BAD_CAST str);	
	}
}

void GlbGlobe::GlbRenderDynamicLabelFadeSpeed::Load( xmlNodePtr node )
{
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsUseField")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bUseField);		
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Field")))
	{
		szKey = xmlNodeGetContent(node);	
		char* str = u2g((char*)szKey);
		CGlbString glbstr = str;
		field = glbstr.ToWString();
		xmlFree(szKey);
		free(str);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Value")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%d",&value);
		xmlFree(szKey);
		node = node->next;
	}
}

void GlbCoordinateAxisSymbolInfo::Save(xmlNodePtr node,const glbWChar* relativepath)
{
	char str[64];
	sprintf_s(str,"%c",bShowGrid);
	xmlNewTextChild(node, NULL, BAD_CAST "ShowGrid", BAD_CAST str);

	sprintf_s(str,"%c", bShowLabel);
	xmlNewTextChild(node, NULL, BAD_CAST "ShowLabel", BAD_CAST str);

	sprintf_s(str,"%c", bInvertZ);
	xmlNewTextChild(node, NULL, BAD_CAST "InvertZ", BAD_CAST str);

	sprintf_s(str,"%.2lf",originX);
	xmlNewTextChild(node, NULL, BAD_CAST "OriginX", BAD_CAST str);

	sprintf_s(str,"%.2lf",originY);
	xmlNewTextChild(node, NULL, BAD_CAST "OriginY", BAD_CAST str);

	sprintf_s(str,"%.2lf",originZ);
	xmlNewTextChild(node, NULL, BAD_CAST "OriginZ", BAD_CAST str);

	sprintf_s(str,"%.2lf",axisLengthX);
	xmlNewTextChild(node, NULL, BAD_CAST "AxisLengthX", BAD_CAST str);

	sprintf_s(str,"%.2lf",axisLengthY);
	xmlNewTextChild(node, NULL, BAD_CAST "AxisLengthY", BAD_CAST str);

	sprintf_s(str,"%.2lf",axisLengthZ);
	xmlNewTextChild(node, NULL, BAD_CAST "AxisLengthZ", BAD_CAST str);

	sprintf_s(str,"%.2lf",axisStepX);
	xmlNewTextChild(node, NULL, BAD_CAST "AxisStepX", BAD_CAST str);

	sprintf_s(str,"%.2lf",axisStepY);
	xmlNewTextChild(node, NULL, BAD_CAST "AxisStepY", BAD_CAST str);

	sprintf_s(str,"%.2lf",axisStepZ);
	xmlNewTextChild(node, NULL, BAD_CAST "AxisStepZ", BAD_CAST str);

	sprintf_s(str,"%.2lf",labelSize);
	xmlNewTextChild(node, NULL, BAD_CAST "LabelSize", BAD_CAST str);

	sprintf_s(str,"%.2lf",xOffset);
	xmlNewTextChild(node, NULL, BAD_CAST "XOffset", BAD_CAST str);

	sprintf_s(str,"%.2lf",yOffset);
	xmlNewTextChild(node, NULL, BAD_CAST "YOffset", BAD_CAST str);

	sprintf_s(str,"%.2lf",zOffset);
	xmlNewTextChild(node, NULL, BAD_CAST "ZOffset", BAD_CAST str);

	sprintf_s(str,"%.2lf",xScale);
	xmlNewTextChild(node, NULL, BAD_CAST "XScale", BAD_CAST str);
	
	sprintf_s(str,"%.2lf",yScale);
	xmlNewTextChild(node, NULL, BAD_CAST "YScale", BAD_CAST str);

	sprintf_s(str,"%.2lf",zScale);
	xmlNewTextChild(node, NULL, BAD_CAST "ZScale", BAD_CAST str);
}
void GlbCoordinateAxisSymbolInfo::Load(xmlNodePtr node,const glbWChar* relativepath)
{
	xmlChar* szKey;
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ShowGrid")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bShowGrid);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ShowLabel")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bShowLabel);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"InvertZ")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%c",&bInvertZ);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"OriginX")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&originX);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"OriginY")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&originY);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"OriginZ")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&originZ);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"AxisLengthX")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&axisLengthX);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"AxisLengthY")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&axisLengthY);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"AxisLengthZ")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&axisLengthZ);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"AxisStepX")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&axisStepX);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"AxisStepY")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&axisStepY);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"AxisStepZ")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&axisStepZ);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"LabelSize")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&labelSize);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XOffset")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&xOffset);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YOffset")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&yOffset);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZOffset")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&zOffset);		
		xmlFree(szKey);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XScale")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&xScale);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YScale")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&yScale);		
		xmlFree(szKey);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZScale")))
	{
		szKey = xmlNodeGetContent(node);	
		sscanf_s((char*)szKey,"%lf",&zScale);		
		xmlFree(szKey);
		node = node->next;
	}

}
void GlbGlobe::GlbLineDynamicArrowSymbolInfo::Save( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	xmlNodePtr IsForwardNode = xmlNewNode(NULL,BAD_CAST "IsForward");
	xmlAddChild(node,IsForwardNode);
	if(isForwardDirection)isForwardDirection->Save(IsForwardNode);

	xmlNodePtr IsShowLineNode = xmlNewNode(NULL,BAD_CAST "IsShowLine");
	xmlAddChild(node,IsShowLineNode);
	if(isShowLine)isShowLine->Save(IsShowLineNode);

	xmlNodePtr numberNode = xmlNewNode(NULL,BAD_CAST "Number");
	xmlAddChild(node,numberNode);
	if(number)number->Save(numberNode);	

	xmlNodePtr colorNode = xmlNewNode(NULL,BAD_CAST "Color");
	xmlAddChild(node,colorNode);
	if(color)color->Save(colorNode);	

	xmlNodePtr diameterNode = xmlNewNode(NULL,BAD_CAST "Diameter");
	xmlAddChild(node,diameterNode);
	if(diameter)diameter->Save(diameterNode);

	xmlNodePtr timeNode = xmlNewNode(NULL,BAD_CAST "Time");
	xmlAddChild(node,timeNode);
	if(time)time->Save(timeNode);

	xmlNodePtr modelPathNode = xmlNewNode(NULL,BAD_CAST "ModelPath");
	xmlAddChild(node,modelPathNode);
	if(modelPath->bUseField == false)
	{
		GlbRenderString rdstring;
		rdstring.bUseField = modelPath->bUseField;

		CGlbWString apath = CGlbPath::GetDir(modelPath->value);
		CGlbWString filename = CGlbPath::GetFileName(modelPath->value);
		glbWChar* rpath = CGlbPath::AbsoluteToRelative((glbWChar*)apath.c_str(),relativepath);
		if(rpath)
		{
			rdstring.value = rpath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = rdstring.value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (rdstring.value != L"" && needExpand)			
				rdstring.value += L"\\";
			rdstring.value += filename;
			delete rpath;
		}
		rdstring.Save(modelPathNode);
	}else{
		modelPath->Save(modelPathNode);
	}

	xmlNodePtr yawNode = xmlNewNode(NULL,BAD_CAST "Yaw");
	xmlAddChild(node,yawNode);
	if(yaw)yaw->Save(yawNode);

	xmlNodePtr pitchNode = xmlNewNode(NULL,BAD_CAST "Pitch");
	xmlAddChild(node,pitchNode);
	if(pitch)pitch->Save(pitchNode);

	xmlNodePtr rollNode = xmlNewNode(NULL,BAD_CAST "Roll");
	xmlAddChild(node,rollNode);
	if(roll)roll->Save(rollNode);

	xmlNodePtr xScaleNode = xmlNewNode(NULL,BAD_CAST "XScale");
	xmlAddChild(node,xScaleNode);
	if(xScale)xScale->Save(xScaleNode);

	xmlNodePtr yScaleNode = xmlNewNode(NULL,BAD_CAST "YScale");
	xmlAddChild(node,yScaleNode);
	if(yScale)yScale->Save(yScaleNode);

	xmlNodePtr zScaleNode = xmlNewNode(NULL,BAD_CAST "ZScale");
	xmlAddChild(node,zScaleNode);
	if(zScale)zScale->Save(zScaleNode);

	xmlNodePtr xOffsetNode = xmlNewNode(NULL,BAD_CAST "XOffset");
	xmlAddChild(node,xOffsetNode);
	if(xOffset)xOffset->Save(xOffsetNode);

	xmlNodePtr yOffsetNode = xmlNewNode(NULL,BAD_CAST "YOffset");
	xmlAddChild(node,yOffsetNode);
	if(yOffset)yOffset->Save(yOffsetNode);

	xmlNodePtr zOffsetNode = xmlNewNode(NULL,BAD_CAST "ZOffset");
	xmlAddChild(node,zOffsetNode);
	if(zOffset)zOffset->Save(zOffsetNode);
}

void GlbGlobe::GlbLineDynamicArrowSymbolInfo::Load( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsForward")))
	{
		isForwardDirection = new GlbRenderBool();
		isForwardDirection->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"IsShowLine")))
	{
		isShowLine = new GlbRenderBool();
		isShowLine->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Number")))
	{
		number = new GlbRenderInt32();
		number->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Color")))
	{
		color = new GlbRenderColor();
		color->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Diameter")))
	{
		diameter = new GlbRenderDouble();
		diameter->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Time")))
	{
		time = new GlbRenderDouble();
		time->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ModelPath")))
	{
		modelPath = new GlbRenderString();
		modelPath->Load(node->xmlChildrenNode);		

		CGlbWString rpath = CGlbPath::GetDir(modelPath->value);
		CGlbWString filename = CGlbPath::GetFileName(modelPath->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			modelPath->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = modelPath->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				modelPath->value += L"\\";
			modelPath->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZScale")))
	{
		zScale = new GlbRenderDouble();
		zScale->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"XOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"YOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ZOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbLineDynamicArrowSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{

}

void GlbGlobe::GlbMarkerModelSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"locate")))
	{
		locate = new GlbRenderString();
		locate->Load2(node->xmlChildrenNode);
		node = node->next;//text
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zScale")))
	{
		zScale = new GlbRenderDouble();
		zScale->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load(node->xmlChildrenNode);

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load(node->xmlChildrenNode);

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load2(node->xmlChildrenNode);

		node = node->next;
	}
	//if (node && (!xmlStrcmp(node->name, (const xmlChar *)"Label")))
	//{
	//	label = new GlbRenderSimpleLabel();
	//	label->Load2(node->xmlChildrenNode);
	//	node = node->next;
	//}
}

void GlbGlobe::GlbRenderString::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		xmlFree(szKey);
		value = tempValue.ToWString();
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderInt32::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&value);
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderDouble::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%lf",&value);
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderBool::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		xmlFree(szKey);
		if(tempValue == "false")
			value = false;
		else
			value = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbMarker2DShapeSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"fillInfo")))
	{
		fillInfo = new GlbPolygon2DSymbolInfo();
		fillInfo->Load2(node->xmlChildrenNode,relativepath);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"shapeInfo")))
	{
		xmlNodePtr shapetypeNode = node->xmlChildrenNode;
		xmlChar* szKey;

		if (shapetypeNode && (!xmlStrcmp(shapetypeNode->name, (const xmlChar *)"text")))
		{
			shapetypeNode = shapetypeNode->next;
		}

		if (shapetypeNode && (!xmlStrcmp(shapetypeNode->name, (const xmlChar *)"shapeType")))
		{
			szKey = xmlNodeGetContent(shapetypeNode->children);
			char* str = u2g((char*)szKey);
			CGlbString tempType = (char*)str;
			GlbMarkerShapeTypeEnum  shapetype;
			if(tempType == "NGLB_MARKERSHAPE_CIRCLE")
				shapetype = GLB_MARKERSHAPE_CIRCLE;
			else if(tempType == "NGLB_MARKERSHAPE_RECTANGLE")
				shapetype = GLB_MARKERSHAPE_RECTANGLE;
			else if(tempType == "NGLB_MARKERSHAPE_ELLIPSE")
				shapetype = GLB_MARKERSHAPE_ELLIPSE;
			else if(tempType == "NGLB_MARKERSHAPE_ARC")
				shapetype = GLB_MARKERSHAPE_ARC;
			xmlFree(szKey);

			switch(shapetype)
			{
			case GLB_MARKERSHAPE_CIRCLE:   shapeInfo = new GlbCircleInfo();break;
			case GLB_MARKERSHAPE_RECTANGLE:shapeInfo = new GlbRectangleInfo();break;
			case GLB_MARKERSHAPE_ELLIPSE:  shapeInfo = new GlbEllipseInfo();break;
			case GLB_MARKERSHAPE_ARC:      shapeInfo = new GlbArcInfo();break;
			}
			if(shapeInfo)shapeInfo->Load2(&(shapetypeNode->next));
			node = node->next;
		}
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zScale")))
	{
		zScale = new GlbRenderDouble();
		zScale->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load2(node->xmlChildrenNode);

		node = node->next;
	}
}

void GlbGlobe::GlbPolygon2DSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"type")))//NGLB_OBJECTTYPE_POLYGON
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"symType")))//NGLB_POLYGONSYMBOL_2D
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"outlineInfo")))
	{
		outlineInfo = new GlbLinePixelSymbolInfo();
		outlineInfo->Load2(node->xmlChildrenNode,relativepath);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"color")))
	{
		color = new GlbRenderColor();
		color->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"textureData")))
	{
		textureData = new GlbRenderString();
		textureData->Load2(node->xmlChildrenNode);		

		CGlbWString rpath = CGlbPath::GetDir(textureData->value);
		CGlbWString filename = CGlbPath::GetFileName(textureData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			textureData->value = apath;	
			// 判断是否需要在路径后面添加 "\"
			std::string _path = textureData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				textureData->value += L"\\";
			textureData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"texRepeatMode")))
	{
		texRepeatMode = new GlbRenderTexRepeatMode();
		texRepeatMode->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"tilingU")))
	{
		tilingU = new GlbRenderInt32();
		tilingU->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"tilingV")))
	{
		tilingV = new GlbRenderInt32();
		tilingV->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"textureRotation")))
	{
		textureRotation = new GlbRenderDouble();
		textureRotation->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbCircleInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;
	
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbRectangleInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"width")))
	{
		width = new GlbRenderDouble();
		width->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"height")))
	{
		height = new GlbRenderDouble();
		height->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbEllipseInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"xRadius")))
	{
		xRadius = new GlbRenderDouble();
		xRadius->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"yRadius")))
	{
		yRadius = new GlbRenderDouble();
		yRadius->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbArcInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"xRadius")))
	{
		xRadius = new GlbRenderDouble();
		xRadius->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"yRadius")))
	{
		yRadius = new GlbRenderDouble();
		yRadius->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"sAngle")))
	{
		sAngle = new GlbRenderDouble();
		sAngle->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"eAngle")))
	{
		eAngle = new GlbRenderDouble();
		eAngle->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"isFan")))
	{
		isFan = new GlbRenderBool();
		isFan->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbSphereInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbBoxInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"length")))
	{
		length = new GlbRenderDouble();
		length->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"width")))
	{
		width = new GlbRenderDouble();
		width->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"height")))
	{
		height = new GlbRenderDouble();
		height->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbPyramidInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"length")))
	{
		length = new GlbRenderDouble();
		length->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"width")))
	{
		width = new GlbRenderDouble();
		width->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"height")))
	{
		height = new GlbRenderDouble();
		height->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbConeInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"height")))
	{
		height = new GlbRenderDouble();
		height->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbPieInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"sAngle")))
	{
		sAngle = new GlbRenderDouble();
		sAngle->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"eAngle")))
	{
		eAngle = new GlbRenderDouble();
		eAngle->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"height")))
	{
		height = new GlbRenderDouble();
		height->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbCylinderInfo::Load2( xmlNodePtr* node )
{
	xmlNodePtr pnode = *node;

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"radius")))
	{
		radius = new GlbRenderDouble();
		radius->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"edges")))
	{
		edges = new GlbRenderInt32();
		edges->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"height")))
	{
		height = new GlbRenderDouble();
		height->Load2(pnode->xmlChildrenNode);
		pnode = pnode->next;
	}
}

void GlbGlobe::GlbShapeInfo::Load2( xmlNodePtr* node )
{

}

void GlbGlobe::GlbLinePixelSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"type")))//NGLB_OBJECTTYPE_LINE
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"symType")))//NGLB_LINESYMBOL_PIXEL
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"lineWidth")))
	{
		lineWidth = new GlbRenderDouble();
		lineWidth->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"lineColor")))
	{
		lineColor = new GlbRenderColor();
		lineColor->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"lineOpacity")))
	{
		lineOpacity = new GlbRenderInt32();
		lineOpacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"linePattern")))
	{
		linePattern = new GlbRenderLinePattern();
		linePattern->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbRenderColor::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		sscanf_s((char*)szKey,"%d",&value);	
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderTexRepeatMode::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "NGLB_TEXTUREREPEAT_TIMES")
		{
			value = GlbGlobeTexRepeatModeEnum::GLB_TEXTUREREPEAT_TIMES;
		}
		else if(tempValue == "NGLB_TEXTUREREPEAT_SIZE")
		{
			value = GlbGlobeTexRepeatModeEnum::GLB_TEXTUREREPEAT_SIZE;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderLinePattern::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "NGLB_LINE_SOLID")
		{
			value = GlbGlobeLinePatternEnum::GLB_LINE_SOLID;
		}
		else if(tempValue == "NGLB_LINE_DOTTED")
		{
			value = GlbGlobeLinePatternEnum::GLB_LINE_DOTTED;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbMarker3DShapeSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"fillInfo")))
	{
		fillInfo = new GlbPolygon2DSymbolInfo();
		fillInfo->Load2(node->xmlChildrenNode,relativepath);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"shapeInfo")))
	{
		xmlNodePtr shapetypeNode = node->xmlChildrenNode;
		xmlChar* szKey;

		if (shapetypeNode && (!xmlStrcmp(shapetypeNode->name, (const xmlChar *)"text")))
		{
			shapetypeNode = shapetypeNode->next;
		}

		if (shapetypeNode && (!xmlStrcmp(shapetypeNode->name, (const xmlChar *)"shapeType")))
		{
			szKey = xmlNodeGetContent(shapetypeNode->children);
			char* str = u2g((char*)szKey);
			CGlbString tempType = (char*)str;
			GlbMarkerShapeTypeEnum  shapetype;
			if(tempType == "NGLB_MARKERSHAPE_SPHERE")
				shapetype = GLB_MARKERSHAPE_SPHERE;
			else if(tempType == "NGLB_MARKERSHAPE_BOX")
				shapetype = GLB_MARKERSHAPE_BOX;
			else if(tempType == "NGLB_MARKERSHAPE_PYRAMID")
				shapetype = GLB_MARKERSHAPE_PYRAMID;
			else if(tempType == "NGLB_MARKERSHAPE_CONE")
				shapetype = GLB_MARKERSHAPE_CONE;
			else if(tempType == "NGLB_MARKERSHAPE_PIE")
				shapetype = GLB_MARKERSHAPE_PIE;
			else if(tempType == "NGLB_MARKERSHAPE_CYLINDER")
				shapetype = GLB_MARKERSHAPE_CYLINDER;
			xmlFree(szKey);

			switch(shapetype)
			{
			case GLB_MARKERSHAPE_SPHERE:   shapeInfo = new GlbSphereInfo();break;
			case GLB_MARKERSHAPE_BOX:shapeInfo = new GlbBoxInfo();break;
			case GLB_MARKERSHAPE_PYRAMID:  shapeInfo = new GlbPyramidInfo();break;
			case GLB_MARKERSHAPE_CONE:      shapeInfo = new GlbConeInfo();break;
			case GLB_MARKERSHAPE_PIE:      shapeInfo = new GlbPieInfo();break;
			case GLB_MARKERSHAPE_CYLINDER:      shapeInfo = new GlbCylinderInfo();break;
			}
			if(shapeInfo)shapeInfo->Load2(&(shapetypeNode->next));
			node = node->next;
		}
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zScale")))
	{
		zScale = new GlbRenderDouble();
		zScale->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load2(node->xmlChildrenNode);

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load2(node->xmlChildrenNode);

		node = node->next;
	}
}

void GlbGlobe::GlbLine2DSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{

}

void GlbGlobe::GlbLine3DSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{

}

void GlbGlobe::GlbLineArrowSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"pattern")))
	{
		pattern = new GlbRenderArrowPattern();
		pattern->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"height")))
	{
		height = new GlbRenderDouble();
		height->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"width")))
	{
		width = new GlbRenderDouble();
		width->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"linePixelInfo")))
	{
		linePixelInfo = new GlbLinePixelSymbolInfo();
		linePixelInfo->Load2(node->xmlChildrenNode,relativepath);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"fillColor")))
	{
		fillColor = new GlbRenderColor();
		fillColor->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"fillOpacity")))
	{
		fillOpacity = new GlbRenderInt32();
		fillOpacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"textureData")))
	{
		textureData = new GlbRenderString();
		textureData->Load2(node->xmlChildrenNode);		

		CGlbWString rpath = CGlbPath::GetDir(textureData->value);
		CGlbWString filename = CGlbPath::GetFileName(textureData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			textureData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = textureData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				textureData->value += L"\\";
			textureData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"texRepeatMode")))
	{
		texRepeatMode = new GlbRenderTexRepeatMode();
		texRepeatMode->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"tilingU")))
	{
		tilingU = new GlbRenderDouble();
		tilingU->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"tilingV")))
	{
		tilingV = new GlbRenderDouble();
		tilingV->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"textureRotation")))
	{
		textureRotation = new GlbRenderDouble();
		textureRotation->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbPolygonWaterSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"color")))
	{
		color = new GlbRenderColor();
		color->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isUseEnv")))
	{
		isUseEnv = new GlbRenderBool();
		isUseEnv->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"windStrong")))
	{
		windStrong = new GlbRenderDouble();
		windStrong->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"windDirection")))
	{
		windDirection = new GlbRenderDouble();
		windDirection->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"enableReflections")))
	{
		enableReflections = new GlbRenderBool();
		enableReflections->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"enableRefractions")))
	{
		enableRefractions = new GlbRenderBool();
		enableRefractions->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"refractionImageData")))
	{
		refractionImageData = new GlbRenderString();
		refractionImageData->Load2(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(refractionImageData->value);
		CGlbWString filename = CGlbPath::GetFileName(refractionImageData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			refractionImageData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = refractionImageData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				refractionImageData->value += L"\\";
			refractionImageData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"refractTexRepeatMode")))
	{
		refractTexRepeatMode = new GlbRenderTexRepeatMode();
		refractTexRepeatMode->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"refractTextilingU")))
	{
		refractTextilingU = new GlbRenderInt32();
		refractTextilingU->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"refractTextilingV")))
	{
		refractTextilingV = new GlbRenderInt32();
		refractTextilingV->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"refractTexRotation")))
	{
		refractTexRotation = new GlbRenderDouble();
		refractTexRotation->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbPolyhedronSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{

}

void GlbGlobe::GlbMarkerPixelSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"color")))
	{
		color = new GlbRenderColor();
		color->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"size")))
	{
		size = new GlbRenderInt32();
		size->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"label")))
	{
		label = new GlbRenderSimpleLabel();
		label->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbMarkerLabelSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"mode")))
	{
		mode = new GlbRenderBillboard();
		mode->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"labelAlign")))
	{
		labelAlign = new GlbRenderLabelAlign();
		labelAlign->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"textInfo")))
	{
		textInfo = new GlbRenderTextInfo();
		textInfo->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"imageInfo")))
	{
		imageInfo = new GlbRenderImageInfo();
		imageInfo->Load2(node->xmlChildrenNode,relativepath);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"labelImageRelationInfo")))
	{
		labelImageRelationInfo = new GlbRenderLabelRelationInfo();
		labelImageRelationInfo->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"geoInfo")))
	{
		geoInfo = new GlbRenderGeoInfo();
		geoInfo->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble;
		xOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble;
		yOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble;
		zOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbMarkerImageSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"imgInfo")))
	{
		imgInfo = new GlbRenderImageInfo();
		imgInfo->Load2(node->xmlChildrenNode,relativepath);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"imgBillboard")))
	{
		imgBillboard = new GlbRenderBillboard();
		imgBillboard->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"imageAlign")))
	{
		imageAlign = new GlbRenderImageAlign();
		imageAlign->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"geoInfo")))
	{
		geoInfo = new GlbRenderGeoInfo();
		geoInfo->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"pitch")))
	{
		pitch = new GlbRenderDouble();
		pitch->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yaw")))
	{
		yaw = new GlbRenderDouble();
		yaw->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"roll")))
	{
		roll = new GlbRenderDouble();
		roll->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble;
		xOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble;
		yOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name,(const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble;
		zOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbMarkerFireSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{

}

void GlbGlobe::GlbMarkerSmokeSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{

}

void GlbGlobe::GlbMarkerEpicentreSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{

}

void GlbGlobe::GlbRenderSimpleLabel::Load2( xmlNodePtr node )
{

}

void GlbGlobe::GlbRenderImageInfo::Load2( xmlNodePtr node,const glbWChar* prjPath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"dataSource")))
	{
		dataSource = new GlbRenderString();
		dataSource->Load2(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(dataSource->value);
		CGlbWString filename = CGlbPath::GetFileName(dataSource->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),prjPath);
		if(filename.length()>0 && apath)
		{
			dataSource->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = dataSource->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				dataSource->value += L"\\";
			dataSource->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"maxSize")))
	{
		maxSize = new GlbRenderInt32();
		maxSize->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"color")))
	{
		color = new GlbRenderColor();
		color->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbRenderBillboard::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "NGLB_BILLBOARD_AUTOSLOPE")
		{
			value = GlbGlobeBillboardModeEnum::GLB_BILLBOARD_AUTOSLOPE;
		}
		else if(tempValue == "NGLB_BILLBOARD_AXIS")
		{
			value = GlbGlobeBillboardModeEnum::GLB_BILLBOARD_AXIS;
		}
		else if(tempValue == "NGLB_BILLBOARD_SCREEN")
		{
			value = GlbGlobeBillboardModeEnum::GLB_BILLBOARD_SCREEN;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderImageAlign::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "NGLB_LABELALG_CENTERBOTTOM")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_CENTERBOTTOM;
		}
		else if(tempValue == "NGLB_LABELALG_CENTERCENTER")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_CENTERCENTER;
		}
		else if(tempValue == "NGLB_LABELALG_CENTERTOP")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_CENTERTOP;
		}
		else if(tempValue == "NGLB_LABELALG_LEFTBOTTOM")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_LEFTBOTTOM;
		}
		else if(tempValue == "NGLB_LABELALG_LEFTCENTER")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_LEFTCENTER;
		}
		else if(tempValue == "NGLB_LABELALG_LEFTTOP")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_LEFTTOP;
		}
		else if(tempValue == "NGLB_LABELALG_RIGHTBOTTOM")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_RIGHTBOTTOM;
		}
		else if(tempValue == "NGLB_LABELALG_RIGHTCENTER")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_RIGHTCENTER;
		}
		else if(tempValue == "NGLB_LABELALG_RIGHTTOP")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_RIGHTTOP;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderGeoInfo::Load2( xmlNodePtr node )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isLimit")))
	{
		isLimit = new GlbRenderBool();
		isLimit->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"ratio")))
	{
		ratio = new GlbRenderDouble();
		ratio->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbRenderLabelAlign::Load2( xmlNodePtr node )
{
		xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "NGLB_LABELALG_CENTERBOTTOM")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_CENTERBOTTOM;
		}
		else if(tempValue == "NGLB_LABELALG_CENTERCENTER")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_CENTERCENTER;
		}
		else if(tempValue == "NGLB_LABELALG_CENTERTOP")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_CENTERTOP;
		}
		else if(tempValue == "NGLB_LABELALG_LEFTBOTTOM")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_LEFTBOTTOM;
		}
		else if(tempValue == "NGLB_LABELALG_LEFTCENTER")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_LEFTCENTER;
		}
		else if(tempValue == "NGLB_LABELALG_LEFTTOP")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_LEFTTOP;
		}
		else if(tempValue == "NGLB_LABELALG_RIGHTBOTTOM")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_RIGHTBOTTOM;
		}
		else if(tempValue == "NGLB_LABELALG_RIGHTCENTER")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_RIGHTCENTER;
		}
		else if(tempValue == "NGLB_LABELALG_RIGHTTOP")
		{
			value = GlbGlobeLabelAlignTypeEnum::GLB_LABELALG_RIGHTTOP;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderLabelRelationInfo::Load2( xmlNodePtr node )
{
	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"relation")))
	{
		relation = new GlbRenderLabelRelation();
		relation->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"align")))
	{
		align = new GlbRenderLabelAlign();
		align->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbRenderLabelRelation::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "NGLB_LABEL_ON_IMAGE")
		{
			value = GlbGlobeLabelRelationEnum::GLB_LABEL_ON_IMAGE;
		}
		else if(tempValue == "NGLB_LABEL_SURROUND_IMAGE")
		{
			value = GlbGlobeLabelRelationEnum::GLB_LABEL_SURROUND_IMAGE;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderTextInfo::Load2( xmlNodePtr node )
{
	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"color")))
	{
		color = new GlbRenderColor();
		color->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"size")))
	{
		size = new GlbRenderInt32();
		size->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"font")))
	{
		font = new GlbRenderString();
		font->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"content")))
	{
		content = new GlbRenderString();
		content->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isBold")))
	{
		isBold = new GlbRenderBool();
		isBold->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isUnderline")))
	{
		isUnderline = new GlbRenderBool();
		isUnderline->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isItalic")))
	{
		isItalic = new GlbRenderBool();
		isItalic->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"backColor")))
	{
		backColor = new GlbRenderColor();
		backColor->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"backOpacity")))
	{
		backOpacity = new GlbRenderInt32();
		backOpacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"multilineAlign")))
	{
		multilineAlign = new GlbRenderMultilineAlign();
		multilineAlign->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbRenderMultilineAlign::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "NGLB_MULTILINEALG_CENTER")
		{
			value = GlbGlobeMultilineAlignTypeEnum::GLB_MULTILINEALG_CENTER;
		}
		else if(tempValue == "NGLB_MULTILINEALG_LEFT")
		{
			value = GlbGlobeMultilineAlignTypeEnum::GLB_MULTILINEALG_LEFT;
		}
		else if(tempValue == "NGLB_MULTILINEALG_RIGHT")
		{
			value = GlbGlobeMultilineAlignTypeEnum::GLB_MULTILINEALG_RIGHT;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString tempField = (char*)str;
		xmlFree(szKey);
		field = tempField.ToWString();
	}
}

void GlbGlobe::GlbRenderArrowPattern::Load2( xmlNodePtr node )
{
	xmlChar* szKey;

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"bUseField")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);	
		char* str = u2g((char*)szKey);
		CGlbString bUse = (char*)str;
		xmlFree(szKey);
		if(bUse == "false")
			bUseField = false;
		else
			bUseField = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"value")))
	{
		szKey = xmlNodeGetContent(node->xmlChildrenNode);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "NGLB_ARROWPATTERN_1")
		{
			value = GlbArrowPatternEnum::GLB_ARROWPATTERN_D1;
		}
		else if(tempValue == "NGLB_ARROWPATTERN_2")
		{
			value = GlbArrowPatternEnum::GLB_ARROWPATTERN_D2;
		}
		else if(tempValue == "NGLB_ARROWPATTERN_3")
		{
			value = GlbArrowPatternEnum::GLB_ARROWPATTERN_D3;
		}
		else if(tempValue == "NGLB_ARROWPATTERN_4")
		{
			value = GlbArrowPatternEnum::GLB_ARROWPATTERN_D4;
		}
		else if(tempValue == "NGLB_ARROWPATTERN_5")
		{
			value = GlbArrowPatternEnum::GLB_ARROWPATTERN_D5;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"field")))
	{
		if(node->xmlChildrenNode)
		{
			szKey = xmlNodeGetContent(node->xmlChildrenNode);	
			char* str = u2g((char*)szKey);
			CGlbString tempField = (char*)str;
			xmlFree(szKey);
			field = tempField.ToWString();
		}
	}
}

void GlbGlobe::GlbTinSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"symType")))//NGLB_TINSYMBOL_SIMPLE
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"color")))
	{
		color = new GlbRenderColor();
		color->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"opacity")))
	{
		opacity = new GlbRenderInt32();
		opacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"locate")))
	{
		locate = new GlbRenderString();
		locate->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zScale")))
	{
		zScale = new GlbRenderDouble();
		zScale->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbVideoSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"type")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"symType")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "GLB_VIDEOSYMBOL_FREE")
		{
			symType = GLB_VIDEOSYMBOL_FREE;
		}
		else if(tempValue == "GLB_VIDEOSYMBOL_BILLBOARD")
		{
			symType = GLB_VIDEOSYMBOL_BILLBOARD;
		}
		else if(tempValue == "GLB_VIDEOSYMBOL_TERRAIN")
		{
			symType = GLB_VIDEOSYMBOL_TERRAIN;
		}
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"videofile")))
	{
		videofile = new GlbRenderString();
		videofile->Load2(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(videofile->value);
		CGlbWString filename = CGlbPath::GetFileName(videofile->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			videofile->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = videofile->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				videofile->value += L"\\";
			videofile->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"opacity")))
	{
		opacity = new GlbRenderDouble();
		opacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bvoice")))
	{
		bvoice = new GlbRenderBool();
		bvoice->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"volume")))
	{
		volume = new GlbRenderDouble();
		volume->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bPause")))
	{
		bPause = new GlbRenderBool();
		bPause->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"referenceTime")))
	{
		referenceTime = new GlbRenderDouble();
		referenceTime->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bLoop")))
	{
		bLoop = new GlbRenderBool();
		bLoop->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"bRewind")))
	{
		bRewind = new GlbRenderBool();
		bRewind->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xScale")))
	{
		xScale = new GlbRenderDouble();
		xScale->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yScale")))
	{
		yScale = new GlbRenderDouble();
		yScale->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbDigHoleSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"type")))
	{
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"lineInfo")))
	{
		lineInfo = new GlbLinePixelSymbolInfo();
		lineInfo->Load2(node->xmlChildrenNode);
		node = node->next;
	}
	
	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"sideTextureData")))
	{
		sideTextureData = new GlbRenderString();
		sideTextureData->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"belowTextureData")))
	{
		belowTextureData = new GlbRenderString();
		belowTextureData->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbDrillRenderInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderBaseLine")))
	{
		xmlChar* szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "false")
			isRenderBaseLine = false;
		else
			isRenderBaseLine = true;
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"baselineColor")))
	{
		baselineColor = new GlbRenderColor();
		baselineColor->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"baselineOpacity")))
	{
		baselineOpacity = new GlbRenderInt32();
		baselineOpacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"baselineWidth")))
	{
		baselineWidth = new GlbRenderDouble();
		baselineWidth->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"baselineModelLocate")))
	{
		baselineModelLocate = new GlbRenderString();
		baselineModelLocate->Load2(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(baselineModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(baselineModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			baselineModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = baselineModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				baselineModelLocate->value += L"\\";
			baselineModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderStratum")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "false")
			isRenderStratum = false;
		else
			isRenderStratum = true;
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"stratumColor")))
	{
		stratumColor = new GlbRenderColor();
		stratumColor->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"stratumOpacity")))
	{
		stratumOpacity = new GlbRenderInt32();
		stratumOpacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"stratumWidth")))
	{
		stratumWidth = new GlbRenderDouble();
		stratumWidth->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"stratumModelLocate")))
	{
		stratumModelLocate = new GlbRenderString();
		stratumModelLocate->Load2(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(stratumModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(stratumModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			stratumModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = stratumModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				stratumModelLocate->value += L"\\";
			stratumModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbNetworkRenderInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{

}

void GlbGlobe::GlbSectionRenderInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderSec")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "false")
			isRenderSec = false;
		else
			isRenderSec = true;
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secColor")))
	{
		secColor = new GlbRenderColor();
		secColor->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secOpacity")))
	{
		secOpacity = new GlbRenderInt32();
		secOpacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTextureData")))
	{
		secTextureData = new GlbRenderString();
		secTextureData->Load2(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(secTextureData->value);
		CGlbWString filename = CGlbPath::GetFileName(secTextureData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			secTextureData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = secTextureData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				secTextureData->value += L"\\";
			secTextureData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTexRepeatMode")))
	{
		secTexRepeatMode = new GlbRenderTexRepeatMode();
		secTexRepeatMode->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTilingU")))
	{
		secTilingU = new GlbRenderInt32();
		secTilingU->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTilingV")))
	{
		secTilingV = new GlbRenderInt32();
		secTilingV->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secTextureRotation")))
	{
		secTextureRotation = new GlbRenderDouble();
		secTextureRotation->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"secModelLocate")))
	{
		secModelLocate = new GlbRenderString();
		secModelLocate->Load2(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(secModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(secModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			secModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = secModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				secModelLocate->value += L"\\";
			secModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderArc")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "false")
			isRenderArc = false;
		else
			isRenderArc = true;
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"arcWidth")))
	{
		arcWidth = new GlbRenderDouble();
		arcWidth->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"arcColor")))
	{
		arcColor = new GlbRenderColor();
		arcColor->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"arcOpacity")))
	{
		arcOpacity = new GlbRenderInt32();
		arcOpacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"isRenderPoly")))
	{				
		xmlChar* szKey = xmlNodeGetContent(node);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "false")
			isRenderPoly = false;
		else
			isRenderPoly = true;
		xmlFree(szKey);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyColor")))
	{
		polyColor = new GlbRenderColor();
		polyColor->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyOpacity")))
	{
		polyOpacity = new GlbRenderInt32();
		polyOpacity->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTextureData")))
	{
		polyTextureData = new GlbRenderString();
		polyTextureData->Load2(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(polyTextureData->value);
		CGlbWString filename = CGlbPath::GetFileName(polyTextureData->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			polyTextureData->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = polyTextureData->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				polyTextureData->value += L"\\";
			polyTextureData->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTexRepeatMode")))
	{
		polyTexRepeatMode = new GlbRenderTexRepeatMode();
		polyTexRepeatMode->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTilingU")))
	{
		polyTilingU = new GlbRenderInt32();
		polyTilingU->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTilingV")))
	{
		polyTilingV = new GlbRenderInt32();
		polyTilingV->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyTextureRotation")))
	{
		polyTextureRotation = new GlbRenderDouble();
		polyTextureRotation->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"polyModelLocate")))
	{
		polyModelLocate = new GlbRenderString();
		polyModelLocate->Load2(node->xmlChildrenNode);

		CGlbWString rpath = CGlbPath::GetDir(polyModelLocate->value);
		CGlbWString filename = CGlbPath::GetFileName(polyModelLocate->value);		
		glbWChar* apath = CGlbPath::RelativeToAbsolute(rpath.c_str(),relativepath);
		if(filename.length()>0 && apath)
		{
			polyModelLocate->value = apath;
			// 判断是否需要在路径后面添加 "\"
			std::string _path = polyModelLocate->value.ToString().c_str();
			glbBool needExpand = true;
			if (_path.length() > 0 &&
				(_path.at(_path.length()-1)  == '\\' || _path.at(_path.length()-1)  == 47) )  // 【/或\】 
				needExpand = false;

			if (needExpand)	
				polyModelLocate->value += L"\\";
			polyModelLocate->value += filename;
		}
		if(apath)delete apath;

		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"xOffset")))
	{
		xOffset = new GlbRenderDouble();
		xOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"yOffset")))
	{
		yOffset = new GlbRenderDouble();
		yOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if(node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"zOffset")))
	{
		zOffset = new GlbRenderDouble();
		zOffset->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbPolygonLakeSymbolInfo::Save( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	xmlNodePtr windStrongNode = xmlNewNode(NULL,BAD_CAST "WindStrong");
	xmlAddChild(node,windStrongNode);
	if(windStrong)windStrong->Save(windStrongNode);

	xmlNodePtr windDirectionNode = xmlNewNode(NULL,BAD_CAST "WindDirection");
	xmlAddChild(node,windDirectionNode);
	if(windDirection)windDirection->Save(windDirectionNode);

	xmlNodePtr WaterLevelNode = xmlNewNode(NULL,BAD_CAST "WaterLevel");
	xmlAddChild(node,WaterLevelNode);
	if(waterLevel)waterLevel->Save(WaterLevelNode);
}

void GlbGlobe::GlbPolygonLakeSymbolInfo::Load( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"WindStrong")))
	{
		windStrong = new GlbRenderDouble();
		windStrong->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"WindDirection")))
	{
		windDirection = new GlbRenderDouble();
		windDirection->Load(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"WaterLevel")))
	{
		waterLevel = new GlbRenderDouble();
		waterLevel->Load(node->xmlChildrenNode);
		node = node->next;
	}
}

void GlbGlobe::GlbPolygonLakeSymbolInfo::Load2( xmlNodePtr node,const glbWChar* relativepath/*=NULL*/ )
{
	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"windStrong")))
	{
		windStrong = new GlbRenderDouble();
		windStrong->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"windDirection")))
	{
		windDirection = new GlbRenderDouble();
		windDirection->Load2(node->xmlChildrenNode);
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"text")))
	{
		node = node->next;
	}

	if (node && (!xmlStrcmp(node->name, (const xmlChar *)"waterLevel")))
	{
		waterLevel = new GlbRenderDouble();
		waterLevel->Load2(node->xmlChildrenNode);
		node = node->next;
	}
}