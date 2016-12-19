#include "StdAfx.h"
#include "GlbDataSourceFactory.h"
#include "GlbConvert.h"
#include "GlbPath.h"
#include <fstream>
#include "assert.h"
#include "libxml/tree.h"
#include "GRKSLib.h"

#define GLOBE_DEVELOPER_MODE 1

#if GLB_USE_VLD > 0
#   ifndef VLD_FORCE_ENABLE
#   define VLD_FORCE_ENABLE
#   endif 

#include "vld.h"
#endif

typedef IGlbDataSourceProvider* (*GetProviderFunc)();

CGlbWString MakeProviderDLLName(const glbWChar* name)
{
  CGlbWString dllFileName;
  dllFileName = L"GlbDataEngine";
  dllFileName.append(name);

#ifdef _DEBUG
	dllFileName += L"D";
#else
#endif
	dllFileName += L".dll";
	return dllFileName;
}

CGlbDataSourceFactory CGlbDataSourceFactory::mpr_instance;//定义静态全局变量

CGlbDataSourceFactory::CGlbDataSourceFactory()
{
	mpr_dsbaseID = 0;
}

CGlbDataSourceFactory::~CGlbDataSourceFactory()
{
	vector<DatasourceAndPW*>::iterator itr    = mpr_datasources.begin();
	// 2015.1.13 修改 马林
	while(itr != mpr_datasources.end())
	{
		if((*itr) != NULL)
		{
			(*itr)->datasource = NULL;
			DatasourceAndPW* dd = *itr;
			itr = mpr_datasources.erase(itr);
			delete dd;
		}
		else
			itr++;
	}

	mpr_datasources.clear();
	
}

CGlbDataSourceFactory* CGlbDataSourceFactory::GetInstance()
{
	return &CGlbDataSourceFactory::mpr_instance;
}

IGlbDataSource* CGlbDataSourceFactory::FindDS(const glbWChar* url,const glbWChar* user)
{
	CGlbWString urlstr = L"";
	CGlbWString userstr= L"";
	if(url != NULL)urlstr = url;
	if(user!= NULL)userstr= user;
	urlstr.ToLower();
	userstr.ToLower();

	vector<DatasourceAndPW*>::iterator itr    = mpr_datasources.begin();
	vector<DatasourceAndPW*>::iterator itrEnd = mpr_datasources.end();
	for (itr; itr != itrEnd; itr++)
	{
		if((*itr) != NULL)
		{
			if((*itr)->url == urlstr
			   &&(*itr)->user == userstr)
			   return (*itr)->datasource.get();
		}
	}
	return NULL;
}
IGlbDataSource* CGlbDataSourceFactory::CreateDataSource
	(const glbWChar* url, 
	 const glbWChar* user,
	 const glbWChar* password, 
	 const glbWChar* providerName)
{
	// 验证加密狗
#ifndef GLOBE_DEVELOPER_MODE
	bool isok = false;
	long rt = gverify("glbglobe","2.0");
	if(rt == ERROR_OK)isok=true;
	if(!isok)
	{
		std::string msg="";			
		msg += "本地没有注册或已过有效期";
		::MessageBoxA(NULL,msg.c_str(),"请注册",MB_OK);			
		return NULL;
	}
#endif

	if (url == NULL)
	{
		GlbSetLastError(L"url参数是NULL");
		return NULL;
	}
	if (providerName == NULL)
	{
		GlbSetLastError(L"providerName参数是NULL");
		return NULL;
	}

	IGlbDataSource* ds = NULL;
	//ds = FindDS(url,user);
	//if(ds) return ds;

	CGlbWString name = providerName;
	name.ToLower();//转为小写
	HMODULE hmd = GetModuleHandle(L"GlbDataEngine.dll");
	CGlbWString mdir = CGlbPath::GetModuleDir(hmd);
	CGlbWString dllName = MakeProviderDLLName(name.c_str());
	dllName = mdir + dllName;
	//HMODULE handle = ::LoadLibraryW(dllName.c_str());
	HMODULE handle = LoadLibraryEx(dllName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if(handle == NULL)
	{
		CGlbWString errinfo = L"无法加载提供者对应的dll : ";
		errinfo += dllName;
		GlbSetLastError(errinfo.c_str());
		return NULL;
	}
	GetProviderFunc _getprovider = (GetProviderFunc)GetProcAddress(handle, "GetProvider");
	if(_getprovider == NULL)
	{
		GlbSetLastError(L"获取提供者方法句柄获取失败");
		return NULL;
	}
	IGlbDataSourceProvider* provider = (IGlbDataSourceProvider*)_getprovider();
	if(provider != NULL)
	{
		ds = provider->CreateDataSource(url, user, password);
		if (ds != NULL)
		{
			DatasourceAndPW* dspw = new DatasourceAndPW;
			//设置唯一标示
			ds->SetID(mpr_dsbaseID);
			dspw->datasource = ds;
			dspw->url = url;
			dspw->url.ToLower();
			dspw->user= user;
			dspw->user.ToLower();
			dspw->password = password;
			mpr_datasources.push_back(dspw);
			
			mpr_dsbaseID++;
		}
		//else 会有错误信息，但provider里面已经设置全局错误，不需要再获取、设置
		delete provider;
		return ds;
	}
	else
	{
		GlbSetLastError(L"初始化提供者对象失败");
		return NULL;
	}    
}

IGlbDataSource* CGlbDataSourceFactory::OpenDataSource(const glbWChar* url, const glbWChar* user,
	const glbWChar* password, const glbWChar*providerName)
{
	// 验证加密狗
#ifndef GLOBE_DEVELOPER_MODE
	bool isok = false;
	long rt = gverify("glbglobe","2.0");
	if(rt == ERROR_OK)isok=true;
	if(!isok)
	{
		std::string msg="";			
		msg += "本地没有注册或已过有效期";
		::MessageBoxA(NULL,msg.c_str(),"请注册",MB_OK);			
		return NULL;
	}
#endif

	if (url == NULL)
	{
		GlbSetLastError(L"url参数是NULL");
		return NULL;
	}

	CGlbWString strUser=L"", strPassword=L"";
	if(user != NULL)
		strUser = user;
	if(password != NULL)
		strPassword = password;

	if (providerName == NULL)
	{
		GlbSetLastError(L"providerName参数是NULL");
		return NULL;
	}

	//IGlbDataSource* ds = FindDS(url,user);
	//if(ds) return ds;
	
	CGlbWString name = providerName;
	name.ToLower();//转为小写
	HMODULE hmd = GetModuleHandle(L"GlbDataEngine.dll");
	CGlbWString mdir = CGlbPath::GetModuleDir(hmd);
	CGlbWString dllName = MakeProviderDLLName(name.c_str());
	dllName = mdir + dllName;
	//HMODULE handle = ::LoadLibraryW(dllName.c_str());
	HMODULE handle = LoadLibraryEx(dllName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if(handle == NULL)
	{		
		CGlbWString errinfo = L"无法加载提供者对应的dll : ";
		errinfo += dllName;		
		GlbSetLastError(errinfo.c_str());
		return NULL;
	}
	GetProviderFunc _getprovider = (GetProviderFunc)GetProcAddress(handle, "GetProvider");
	if(_getprovider == NULL)
	{
		GlbSetLastError(L"获取提供者方法句柄获取失败");
		return NULL;
	}
	IGlbDataSourceProvider* provider = (IGlbDataSourceProvider*)_getprovider();
	if(provider != NULL)
	{
		IGlbDataSource* ds = provider->OpenDataSource(url, user, password);
		if (ds != NULL)
		{
			DatasourceAndPW* dspw = new DatasourceAndPW;
			ds->SetID(mpr_dsbaseID);
			mpr_dsbaseID++;
			dspw->datasource = ds;
			dspw->url = url;
			dspw->url.ToLower();
			dspw->user= strUser;
			dspw->user.ToLower();
			dspw->password = strPassword;
			mpr_datasources.push_back(dspw);
		}
		delete provider;		
		return ds;
	}
	else
	{
		GlbSetLastError(L"引擎不存在");		
		return NULL;
	}	
}

IGlbDataSource* CGlbDataSourceFactory::GetDataSource( glbInt32 idx )
{
	if (idx <0 || idx>=(glbInt32)mpr_datasources.size())
	{
		GlbSetLastError(L"idx必须>=0小于数据源数目");
		return NULL;
	}
	
	return mpr_datasources[idx]->datasource.get();
}

IGlbDataSource* CGlbDataSourceFactory::GetDataSourceByID( glbInt32 id )
{
	if (id <0)
	{
		GlbSetLastError(L"id不能小于0");
		return NULL;
	}
	vector<DatasourceAndPW*>::iterator itr = mpr_datasources.begin();
	while (itr != mpr_datasources.end())
	{
		if ((*itr)
			&&(*itr)->datasource->GetID() == id)
		{
			return (*itr)->datasource.get();
		}
		itr++;
	}
	GlbSetLastError(L"当前id对应的数据源不存在");
	return NULL;
}

glbBool CGlbDataSourceFactory::ReleaseDataSource( IGlbDataSource* ds )
{
	if (ds == NULL)
	{
		GlbSetLastError(L"传入参数为NULL");
		return false;
	}
	vector<DatasourceAndPW*>::iterator itr = mpr_datasources.begin();
	while(itr!=mpr_datasources.end())
	{
		if((*itr)
			&&(*itr)->datasource->GetID() == ds->GetID())
		{
			delete (*itr);
			mpr_datasources.erase(itr);
			break;
		}else{
			itr++;
		}
	}
	
	GlbSetLastError(L"数据源不存在");
	return false;
}


glbBool CGlbDataSourceFactory::Load(void* /*xmlNodePtr*/ node, const glbWChar* prjPath, const glbWChar* prjPassword )
{	
	if (node == NULL || prjPath == NULL || prjPassword == NULL)
	{
		GlbSetLastError(L"传入参数失败，不能为NULL");
		return false;
	}
	//walk the tree
	glbWChar* wstr = NULL;
	xmlNodePtr cur = ((xmlNodePtr)node)->xmlChildrenNode;//get sub node
	while(cur !=NULL)
	{
		glbChar* name = (glbChar*)(cur->name);
		glbChar* cvalue = NULL;
		//如果是BaseId，表示解析完成
		if (strcmp(name, "BaseId") == 0)
		{
			cvalue = (glbChar*)xmlNodeGetContent(cur);
			if(!CGlbConvert::ToInt32(cvalue, mpr_dsbaseID))
			{
				xmlFree(cvalue);
				return false;
			}
			xmlFree(cvalue);
			return true;
		}
		else if(strcmp(name, "GlbDataSource") != 0)
		{
			GlbSetLastError(L"当前节点应该是GlbDataSource");
			return false;
		}
		xmlNodePtr childNode = cur->xmlChildrenNode;
		CGlbWString url = prjPath, user= L"", password=L"", provider=L"", alias=L"";
		glbInt32 id=-1;
		while(childNode !=NULL)
		{
			//判断是否还有子节点
			if (childNode->children != NULL &&
				childNode->children->type != XML_TEXT_NODE)
			{
				GlbSetLastError(L"解析xml失败，不应该再有下一层");
				return false;
			}
			name = (glbChar*)(childNode->name);
			cvalue = (char*)xmlNodeGetContent(childNode);
			CGlbString strtemp = cvalue;
			if (strcmp(name, "Url") == 0)
			{
				wstr = UTF82WChar((glbChar*)strtemp.c_str());
				url = wstr;				
				if(wstr)delete wstr;
			}
			else if (strcmp(name, "User") == 0)
			{
				wstr = UTF82WChar((glbChar*)strtemp.c_str());
				user = wstr;//EncDecData(strtemp, false).ToWString();
				if(wstr)delete wstr;
			}
			else if (strcmp(name, "Password") == 0)
			{
				wstr = UTF82WChar((glbChar*)strtemp.c_str());
				password = wstr;//EncDecData(strtemp, false).ToWString();
				if(wstr)delete wstr;
				//格式是prjPassword+”:”+数据源口令
				glbInt32 index = password.find_first_of(L':');
				CGlbWString prjpw = password.substr(0, index);//工程密码
				password = password.substr(index+1, password.length()-index-1);//数据源密码
				CGlbWString inppw = prjPassword;
				if (prjpw != inppw)
				{
					GlbSetLastError(L"工程密码不正确，无法打开");
					xmlFree(cvalue);
					return false;
				}
			}
			else if (strcmp(name, "Provider") == 0)
			{
				wstr = UTF82WChar((glbChar*)strtemp.c_str());
				provider = wstr;
				if(wstr)delete wstr;
			}
			else if (strcmp(name, "Id") == 0)
			{
				if(!CGlbConvert::ToInt32(cvalue, id))
				{
					xmlFree(cvalue);
					return false;
				}
			}
			else if (strcmp(name, "Alias") == 0)
			{
				wstr = UTF82WChar((glbChar*)strtemp.c_str());
				alias = wstr;
				if(wstr)delete wstr;
			}
			else
			{
				GlbSetLastError(L"当前节点名字不正确");
				xmlFree(cvalue);
				return false;
			}
			xmlFree(cvalue);
			childNode = childNode->next;
		}
		provider.ToUpper();
		if(provider==L"FILE")
		{
			url = CGlbPath::RelativeToAbsolute(url.c_str(), prjPath) ;//转成绝对路径
		}
		IGlbDataSource* ds = OpenDataSource(url.c_str(), user.c_str(), password.c_str(), provider.c_str());
		if (ds == NULL)
		{
			// 不能直接返回,因为后面可能有多个数据源，改为continue 2016.11.2 马林
			//return false;
			cur = cur->next;
			continue;
		}
		ds->SetID(id);
		ds->SetAlias(alias.c_str());
		cur = cur->next;
	}
	return true;
}

glbBool CGlbDataSourceFactory::Save(void* /*xmlNodePtr*/ node, glbWChar* prjPath, const glbWChar* prjPassword)
{	
	if (prjPath == NULL || prjPassword == NULL)
	{
		GlbSetLastError(L"传入参数失败，不能为NULL");
		return false;
	}
	char str[128];
	CGlbWString glbwstr;
	vector<DatasourceAndPW*>::iterator itr = mpr_datasources.begin();
	while (itr != mpr_datasources.end())
	{
		CGlbWString url = (*itr)->url;
		url = CGlbPath::AbsoluteToRelative( url.c_str(),prjPath);//转成相对路径存储
		
		CGlbWString user = (*itr)->user;	
		//user = EncDecData(user.ToString(), true).ToWString();//加密
		
		CGlbWString password = prjPassword;
		password += L":";
		password += (*itr)->password;
		//password = EncDecData(password.ToString(), true).ToWString();//加密
		
		xmlNodePtr childnode = xmlNewNode(NULL,BAD_CAST "GlbDataSource");
		xmlAddChild((xmlNodePtr)node,childnode);
		xmlNewTextChild(childnode, NULL, BAD_CAST "Url", BAD_CAST  url.ToUTF8String().c_str());
		xmlNewTextChild(childnode, NULL, BAD_CAST "User", BAD_CAST  user.ToUTF8String().c_str());
		xmlNewTextChild(childnode, NULL, BAD_CAST "Password", BAD_CAST  password.ToUTF8String().c_str());
		glbwstr = (*itr)->datasource->GetProviderName();
		xmlNewTextChild(childnode, NULL, BAD_CAST "Provider", BAD_CAST  glbwstr.ToUTF8String().c_str());		
		sprintf_s(str,"%d",(*itr)->datasource->GetID());
		xmlNewTextChild(childnode, NULL, BAD_CAST "Id", BAD_CAST  str);
		glbwstr = (*itr)->datasource->GetAlias();
		xmlNewTextChild(childnode, NULL, BAD_CAST "Alias", BAD_CAST  glbwstr.ToUTF8String().c_str());
		itr++;
	}
	//os<<L"<BaseId>"<<mpr_dsbaseID<<L"</BaseId>"<<endl;
	sprintf_s(str,"%d",mpr_dsbaseID);
	xmlNewTextChild((xmlNodePtr)node, NULL, BAD_CAST "BaseId", BAD_CAST  str);
	return true;
}
