#include "Stdafx.h"
#include "GlbGlobeObject.h"
#include "GlbString.h"
using namespace GlbGlobe;

CGlbGlobeObject::CGlbGlobeObject()
{
	mpt_id = -1;
	mpt_grpId = -1;
	mpt_name = L"";
	mpt_globe = NULL;
	mpt_action = GLB_GLOBE_FLYTO;
}

CGlbGlobeObject::~CGlbGlobeObject()
{

}

const glbWChar * CGlbGlobeObject::GetName()
{
	return mpt_name.c_str();
}

void CGlbGlobeObject::SetName(const glbWChar *name )
{
	mpt_name = name;
}

GlbGlobeObjectTypeEnum CGlbGlobeObject::GetType()
{
	return GLB_OBJECTTYPE_UNKNOW;
}

glbInt32 CGlbGlobeObject::GetId()
{
	return mpt_id;
}

void CGlbGlobeObject::SetId( glbInt32 id )
{
 	mpt_id = id;
}

glbInt32 CGlbGlobeObject::GetGroupId()
{
	return mpt_grpId;
}

void CGlbGlobeObject::SetGroupId( glbInt32 id )
{
	mpt_grpId = id;
}

void CGlbGlobeObject::SetGlobe( CGlbGlobe *globe )
{
	mpt_globe = globe;
}

CGlbGlobe * CGlbGlobeObject::GetGlobe()
{
	return mpt_globe;
}

void CGlbGlobeObject::SetActiveAction( GlbGlobeActiveActionEnum action )
{
	mpt_action = action;
}

GlbGlobeActiveActionEnum CGlbGlobeObject::GetActiveAction()
{
	return mpt_action;
}

glbBool CGlbGlobeObject::Load( xmlNodePtr* node,const glbWChar* prjPath )
{
	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;
	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Id")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_id);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GroupId")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_grpId);	
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"Name")))
	{
		szKey = xmlNodeGetContent(pnode);		
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			mpt_name = glbstr.ToWString();		
			xmlFree(szKey);
			free(str);
		}
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"ActiveAction")))
	{
		szKey = xmlNodeGetContent(pnode);
		sscanf_s((char*)szKey,"%d",&mpt_action);	
		xmlFree(szKey);

		*node = pnode->next;
	}
	return true;
}

glbBool CGlbGlobeObject::Save( xmlNodePtr node,const glbWChar* prjPath )
{
	char str[32];

	sprintf_s(str,"%d",mpt_id);
	xmlNewTextChild(node, NULL, BAD_CAST "Id", BAD_CAST str);

	sprintf_s(str,"%d",mpt_grpId);
	xmlNewTextChild(node, NULL, BAD_CAST "GroupId", BAD_CAST str);

	char* szOut = g2u((char*)mpt_name.ToString().c_str());	
	xmlNewTextChild(node, NULL, BAD_CAST "Name", BAD_CAST szOut);
	if(szOut)free(szOut);

	sprintf_s(str,"%d",mpt_action);
	xmlNewTextChild(node, NULL, BAD_CAST "ActiveAction", BAD_CAST str);
	
	return true;
}

glbWChar * CGlbGlobeObject::GetLastError()
{
	//调用 GlbComm库的GlbGetLastError 获取最近调用方法的错误信息
	return NULL;
}

glbBool GlbGlobe::CGlbGlobeObject::Load2( xmlNodePtr* node,const glbWChar* prjPath )
{
	xmlChar* szKey = NULL;
	xmlNodePtr pnode = *node;
	xmlNodePtr parentNode;

	if(pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		parentNode = pnode->parent;//GetRenderInfo
	}

	if (parentNode && (!xmlStrcmp(parentNode->name, (const xmlChar *)"GetRenderInfo")))
	{
		parentNode = parentNode->parent;//SerialObject
	}

	if(parentNode && (!xmlStrcmp(parentNode->name, (const xmlChar *)"SerialObject")))
	{
		pnode = parentNode->xmlChildrenNode;
	}

	if(pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GetActiveAction")))
	{
		szKey = xmlNodeGetContent(pnode);
		char* str = u2g((char*)szKey);
		CGlbString tempValue = (char*)str;
		if(tempValue == "NGLB_GLOBE_FLYTO")
			mpt_action = GLB_GLOBE_FLYTO;
		else
			mpt_action = GLB_GLOBE_JUMPTO;
		xmlFree(szKey);

		pnode = pnode->next;
	}

	if(pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"text")))
	{
		pnode = pnode->next;
	}

	if (pnode && (!xmlStrcmp(pnode->name, (const xmlChar *)"GetName")))
	{
		szKey = xmlNodeGetContent(pnode);		
		char* str = u2g((char*)szKey);
		if(str)
		{
			CGlbString glbstr = str;
			mpt_name = glbstr.ToWString();		
			xmlFree(szKey);
			free(str);
		}
		pnode = pnode->next;
		*node = pnode;
	}

	return true;
}
