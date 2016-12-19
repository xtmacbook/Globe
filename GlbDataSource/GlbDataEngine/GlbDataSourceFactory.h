#pragma once
#include "IGlbDataEngine.h"
#include <map>
using namespace std;

/**
* @brief ����Դ����
*
* ��ʵ������
* 
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API CGlbDataSourceFactory
{
private:
	/**
	* @brief ���캯��
	*
	* û���κβ���������ֵ
	*/
	CGlbDataSourceFactory();
	/**
	* @brief ��������
	*
	* û���κβ���������ֵ
	*/
	~CGlbDataSourceFactory();
	static CGlbDataSourceFactory mpr_instance; ///< ����Դ����ʵ������̬����
public:

 /**
  * @brief ����Դ����ʵ����ȡ����
  *
  * @return ��������Դ��������
  */
  static  CGlbDataSourceFactory* GetInstance();
 /**
  * @brief ����һ������Դ
  *
  * @param url          ����Դ������url,�����ִ�Сд
  * @param user         �û����������ִ�Сд
  * @param password     ����
  * @param providerName ����Դ�ṩ�ߣ�Ĭ��Ϊ�ļ�����
                        �ļ�����  : file
						Oracle����: oracle
  * @return �ɹ� ����Դ����ʧ�� NULL
  */
  IGlbDataSource* CreateDataSource(const glbWChar* url, const glbWChar* user,
	  const glbWChar* password, const glbWChar*providerName = L"file");
/**
  * @brief ��һ������Դ
  *
  * @param url ����Դ������url
  * @param user �û���
  * @param password ����
  * @param providerName ����Դ�ṩ�ߣ�Ĭ��Ϊ�ļ�����
  * @return �ɹ� ����Դ����ʧ�� NULL
  */
  IGlbDataSource* OpenDataSource(const glbWChar* url, const glbWChar* user,
	  const glbWChar* password, const glbWChar*providerName = L"file");
  /**
  * @brief ��ȡ����Դ�ĸ���
  *
  * @return ��������Դ����
  */
  glbInt32 GetCount() {return mpr_datasources.size();}
/**
  * @brief �����±��ȡһ������Դ
  *
  * @param idx ����Դ�±�
  * @return �ɹ� ����Դ����ʧ�� NULL
  */
  IGlbDataSource* GetDataSource(glbInt32 idx);
/**
  * @brief ����Ψһ��Ż�ȡ��Ӧ������Դ
  *
  * @param id ����ԴΨһ���
  * @return �ɹ� ����Դ����ʧ�� NULL
  */
  IGlbDataSource* GetDataSourceByID(glbInt32 id);
/**
  * @brief �ͷ�����Դ���󣬲���map�еĶ���ɾ��
  *
  * @param ds ����Դ
  * @return �ɹ� true��ʧ�� false
  */
  glbBool ReleaseDataSource(IGlbDataSource* ds);
/**
  * @brief �ӹ����ļ���load����Դ
  *
  * @param node ���ڵ�
  * @param prjPath ����Ŀ¼
  * @param prjPassword ��������
  * @return �ɹ� true��ʧ�� false
  */
  glbBool Load(void*/*xmlNodePtr*/ node, const glbWChar* prjPath, const glbWChar* prjPassword);
/**
  * @brief ������Դmap����xml�ĸ�ʽ�����������ļ���
  *
  * @param os ���ڵ�
  * @param prjPath ����Ŀ¼
  * @param prjPassword ��������
  * @return �ɹ� true��ʧ�� false
  */
  glbBool Save(void*/*xmlNodePtr*/ os, glbWChar* prjPath, const glbWChar* prjPassword);

/**
  * @brief ��ȡ�ϴ�ִ�еĴ�����Ϣ
  *
  * @return ������Ϣ
  */
  glbWChar* GetLastError(){return GlbGetLastError();}

private:
	IGlbDataSource* FindDS(const glbWChar* url,const glbWChar* user);
private:
	typedef struct _DatasourceAndPW
	{
		glbref_ptr<IGlbDataSource> datasource;///<����Դ
		CGlbWString url;                      ///<��������Դurl
		CGlbWString user;                     ///<�û���
		CGlbWString password;                 ///<����
	}DatasourceAndPW;
	vector<DatasourceAndPW*> mpr_datasources;///<����Դ������Ϣ����
	glbInt32 mpr_dsbaseID;                   ///<����ԴId ����
public:
	map<long/*userid*/,CGlbString/*web token*/> mpr_webtokens;
};
