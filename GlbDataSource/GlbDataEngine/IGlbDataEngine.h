#pragma once
#include "GlbFields.h"
#include "GlbIndexes.h"
#include "GlbPixelBlock.h"
#include "GlbQueryFilter.h"
#include "IGlbProgress.h"

#pragma warning(disable:4091)

/**
* @brief ���ӿ�
* 
*�ṩ��ѯ�����ӿڵķ���
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbUnknown:virtual public CGlbReference
{
public:
	/**
	* @brief ��ѯ�����ӿ�
	*
	* @param riid Ҫ��ѯ�Ľӿ�����
	* @param ppvoid ���ղ�ѯ���Ľӿ�ָ��
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool QueryInterface(const glbWChar *riid,void **ppvoid)=0;
};
class IGlbDataSource;
class IGlbDataset;
class IGlbObjectCursor;
class IGlbObjectUpdateCursor;
class IGlbObjectInsertCursor;
class IGlbFeatureCursor;
class IGlbFeatureUpdateCursor;
class IGlbFeatureInsertCursor;
class IGlbRasterDataset;
class IGlbTerrainDataset;
class IGlbRasterBand;
class CGlbObject;
class CGlbFeature;
class CGlbPixelBlock;
class CGlbDataSourceFactory;
struct GlbSpatialIndexInfo;
/**
* @brief ����Դ�ṩ�߽ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbDataSourceProvider:public CGlbReference
{
public:
	/**
	* @brief ��ȡ��ʶ�ṩ�����͵�����
	*
	* @return �����ļ�ϵͳ����L"file";����db2����L"db2";����oracle����L"oracle".
	*/
	virtual const glbWChar *	GetName()=0;
	/**
	  * @brief ����һ������Դ
	  *
	  * @param url ����Դ������url
	  * @param user �û���
	  * @param password ����
	  * @return �ɹ� ����Դ����ʧ�� NULL
	  */
	virtual IGlbDataSource*	CreateDataSource(const glbWChar* url, const glbWChar* user,
		const glbWChar* password)=0;
	/**
	* @brief ��������Դ�����ִ�����Դ
	*
	* @param url ����Դ������url
	* @param user �û���
	* @param password ����
	* @return �ɹ�:��������Դ�ӿ�ָ�룬ʧ��:NULL
	*/
	virtual IGlbDataSource*	OpenDataSource(const glbWChar* url, const glbWChar* user,
		const glbWChar* password)=0;
	/**
	* @brief ��ȡ�ϴ�ִ�еĴ�����Ϣ
	*
	* @return ������Ϣ
	*/
	virtual glbWChar* GetLastError() = 0;
};
/**
* @brief ����Դ�ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class  GLBDATAENGINE_API IGlbDataSource:public IGlbUnknown
{
public:
	/**
	* @brief  ��ȡ���ݼ���Ŀ
	*
	* @return ���ݼ���Ŀ
	*/
	virtual glbInt32 GetCount() = 0;
	/**
	* @brief ��ȡ���ݼ�
	*
	* @param datasetIndex ���ݼ�����
	* @return             ���ݼ��ӿ�ָ�룬ʧ��:NULL
	*/
	virtual IGlbDataset* GetDataset(glbInt32 datasetIndex) =0;
	/**
	* @brief ��ȡ���ݼ�
	*
	* @param datasetName ���ݼ�����
	* @return            ���ݼ��ӿ�ָ�룬ʧ��:NULL
	*/
	virtual IGlbDataset* GetDatasetByName(const glbWChar* datasetName) = 0;
	/**
	* @brief ɾ�����ݼ�
	*
	* @param datasetName ���ݼ�������
	* @return            �ɹ�:true��ʧ��:false
	*/
	virtual glbBool DeleteDataset (const glbWChar * datasetName) = 0;
	/**
	* @brief ��ȡ����Դ�ı���
	*
	* @return �ɹ�:��������Դ������ʧ��:NULL
	*/
	virtual const glbWChar*	GetAlias() = 0;
	/**
	* @brief ��ȡ����Դ����
	*
	* @param alias ����Դ����
	* @return      �ɹ�:trueʧ��:false
	*/
	virtual glbBool SetAlias(const glbWChar* alias) = 0;
	/**
	* @brief  ��������Դ����
	*
	* @return ����Դ������ʧ��:NULL
	*/
	virtual const glbWChar*	GetProviderName()=0;
	/**
	* @brief  ��ȡ���ݼ�id
	*
	* @return ���ݼ�id
	*/
	virtual glbInt32 GetID() = 0 ;
	/**
	* @brief �������ݼ�id
	*
	* @param  id ����Դid
	* @return    �ɹ�:true��ʧ��:false
	*/
	virtual glbBool SetID(glbInt32 id) = 0 ;
	/**
	* @brief ��ȡ���һ��ִ�еĴ�����Ϣ
	*
	* @return ������Ϣ
	*/
	virtual glbWChar* GetLastError() = 0;
};
/**
* @brief Ҫ������Դ�ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureDataSource:virtual public IGlbDataSource
{
public:
	/**
	* @brief ����һ�����ݼ�
	*
	* @param name ���ݼ�����
	* @param alias ���ݼ�����
	* @param type ���ݼ�����
	* @param srs ���ݼ��ռ�ο�����ϵ
	* @param hasZ ���ݼ��Ƿ����Zֵ
	* @param hasM ���ݼ��Ƿ��������ֵ
	* @return �������ݼ�
	*/
	virtual IGlbDataset* CreateDataset(const glbWChar* name ,
		const glbWChar* alias,
		GlbDatasetTypeEnum type,
		const glbWChar* srs,
		glbBool hasZ, 
		glbBool hasM) = 0;
	/**
	* @brief ִ��һ��sql���
	*
	* @param sqlstr sql�ַ���
	* @return �ɹ�:trueʧ��:false
	*/
	virtual glbBool ExecuteSQL(const glbWChar* sqlstr ) = 0;
	/**
	* @brief ���ݲ�ѯ����ȡ��ѯ������α�
	*
	* @param querystr ��ѯ����ַ���
	* @return ���ز�ѯ������α�
	*/
	virtual IGlbObjectCursor* Query(const glbWChar* querystr) = 0;
};
/**
* @brief ���ݼ��Ļ���ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbDataset:public IGlbUnknown
{
public:
	/**
	* @brief ��ȡ���ݼ�������
	*
	* @return �������ݼ�����
	*/
	virtual GlbDatasetTypeEnum GetType() = 0;
	/**
	* @brief ��ȡ���ݼ�������
	*
	* @return �������ݼ�������
	*/
	virtual const glbWChar* GetName() = 0;
	/**
	* @brief ��ȡ���ݼ��ı���
	*
	* @return �������ݼ��ı���
	*/
	virtual const glbWChar* GetAlias() = 0;
	/**
	* @brief ��ȡ���ݼ�����������Դ
	*
	* @return ������������Դ�ӿ�ָ��
	*/
	virtual const IGlbDataSource* GetDataSource() = 0;
	/**
	* @brief ��ȡ���ݼ��ĸ����ݼ�
	*
	* @return �и����ݼ������ظ����ݼ��ӿ�ָ�롣û�У�����NULL
	*/
	virtual const IGlbDataset* GetParentDataset() = 0;
	/**
	* @brief ��ȡ�ӽ�ɫ
	*/
	virtual const GlbSubDatasetRoleEnum GetSubRole() = 0;
	/**
	* @brief ��ȡ���һ��ִ�еĴ�����Ϣ
	*
	* @return ������Ϣ
	*/
	virtual glbWChar* GetLastError() = 0;
};
/**
* @brief ����ռ����ݼ�����ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbGeoDataset:public IGlbDataset
{
public:
	/**
	* @brief  ��ȡ����ϵ,WKT�ַ���
	*
	* @return ����ϵ��ʧ��:NULL
	*/
	virtual const glbWChar*	GetSRS() = 0;
	/**
	* @brief ��������ϵ
	*        
	* @param srs ����ϵ
	             ������: Proj4�ַ���
				         WKT�ַ���
						 Esri����ϵ�ļ�
						 ��Ϊ��֪�ļ��,��WGS84
						 EPSG����
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool AlterSRS(const glbWChar* srs) = 0;
	/**
	* @brief ��ȡ���ݼ����
	*
	* @return �������
	*/
	virtual const CGlbExtent* GetExtent() = 0;
	/**
	* @brief �Ƿ���Z��
	*/
	virtual glbBool HasZ() = 0;
	/**
	* @brief �Ƿ���M��
	*/
	virtual glbBool HasM() = 0;
};
/**
* @brief ��������ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbTransactions:public IGlbUnknown
{
public:
	/**
	* @brief ��ʼһ������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool StartTransaction() = 0;
	/**
	* @brief �ύһ������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool CommitTransaction() = 0;
	/**
	* @brief �ع�һ������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool RollbackTransaction() = 0;
	/**
	* @brief �ж��Ƿ��п���������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool IsInTransaction() = 0;
	/**
	* @brief ��ȡ���һ��ִ�еĴ�����Ϣ
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbWChar*	GetLastError() = 0;
};
/**
* @brief ��ͨ���ݼ��ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbObjectClass: public IGlbDataset
{
public:
	/**
	* @brief ����ֶ�
	*
	* @param field �ֶ�
	* @return      �ɹ�:true��ʧ��:false
	*/
	virtual glbBool AddField(CGlbField* field) = 0;
	/**
	* @brief ɾ���ֶ�
	*
	* @param fname �ֶ���
	* @return      �ɹ�:true��ʧ��:false
	*/
	virtual glbBool DeleteField(const glbWChar * fname) = 0;
	/**
	* @brief  ��ȡ�ֶ���Ϣ
	*
	* @return �ֶ���Ϣ,ʧ��:NULL
	*/
	virtual CGlbFields* GetFields() = 0;
	/**
	* @brief  ��ȡ������Ϣ
	*
	* @return ������Ϣ,ʧ��:NULL
	*/
	virtual CGlbIndexes* GetIndexes() = 0;
	/**
	* @brief �������
	*
	* @param index ����
	* @return      �ɹ�:true��ʧ��:false
	*/
	virtual glbBool AddIndex(CGlbIndex* index) = 0;
	/**
	* @brief ɾ������
	*
	* @param iname ������
	* @return      �ɹ�:true��ʧ��:false
	*/
	virtual glbBool DeleteIndex(const glbWChar* iname) = 0;
	/**
	* @brief  ��ȡ������Ŀ
	*
	* @return ������Ŀ
	*/
	virtual glbInt32 GetObjectCount() = 0;
	/**
	* @brief ��ȡ������
	*
	* @param oid ����id
	* @return    ����,ʧ��:NULL
	*/
	virtual CGlbObject* GetObject(glbInt32 oid) = 0;
	/**
	* @brief ���¶���
	*
	* @param obj ����
	* @return    �ɹ�:true��ʧ��:false
	*/
	virtual glbBool UpdateObject(CGlbObject* obj) = 0;
	/**
	* @brief ɾ������
	*
	* @param oid ����id
	* @return    �ɹ�:true��ʧ��:false
	*/
	virtual glbBool DeleteObject(glbInt32 oid) = 0;
	/**
	* @brief ɾ��һ������
	*
	* @param queryfilter ������
	* @return            �ɹ�:true��ʧ��:false
	*/
	virtual glbBool DeleteObjects(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief ��ȡ��ѯ�α�
	*
	* @param queryfilter ������
	* @return            �α�,ʧ��:NULL
	*/
	virtual IGlbObjectCursor* Query(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief ��ȡ�����α�
	*
	* @param queryfilter ������
	* @return            �α�,ʧ��NULL
	*/
	virtual IGlbObjectUpdateCursor*	Update(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief ��ȡ�����α�
	*
	* @return �α�,ʧ��NULL
	*/
	virtual IGlbObjectInsertCursor*	Insert() = 0;
};
/**
* @brief Ҫ�����ݼ��ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureClass:public IGlbGeoDataset
{
public:
	/**
	* @brief ����ֶ�
	*
	* @param field �ֶ�
	* @return      �ɹ�:true��ʧ��:false
	*/
	virtual glbBool            AddField(CGlbField* field) = 0;
	/**
	* @brief ɾ���ֶ�
	*
	* @param fname �ֶ���
	* @return      �ɹ�:true��ʧ��:false
	*/
	virtual glbBool            DeleteField(const glbWChar * fname) = 0;
	/**
	* @brief  ��ȡ�ֶ���Ϣ
	*
	* @return �ֶ���Ϣ,ʧ��:NULL
	*/
	virtual const CGlbFields*  GetFields() = 0;
	/**
	* @brief   ��ȡ������Ϣ
	*
	* @return  ������Ϣ
	*/
	virtual const CGlbIndexes* GetIndexes() = 0;
	/**
	* @brief �������
	*
	* @param index ����
	* @return      �ɹ�:true��ʧ��:false
	*/
	virtual glbBool            AddIndex(CGlbIndex* index) = 0;
	/**
	* @brief ɾ������
	*
	* @param iname ������
	* @return      �ɹ�:true��ʧ��:false
	*/
	virtual glbBool            DeleteIndex(const glbWChar* iname) = 0;
	/**
	* @brief  ��ȡ�ռ�������Ϣ
	*
	* @return �ռ�������Ϣ
	*/
	virtual GlbSpatialIndexInfo* GetSpatialIndexInfo()=0;
	/**
	* @brief �����ռ�����
	*
	* @param sidxInfo ������Ϣ
	* @return         �ɹ�:true��ʧ��:false
	*/
	virtual glbBool              BuildSpatialIndex(GlbSpatialIndexInfo * sidxInfo)=0;
	/**
	* @brief ɾ���ռ�������Ϣ
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool              DeleteSpatialIndex()=0;
	/**
	* @brief  ��ȡҪ����Ŀ
	*
	* @return Ҫ����Ŀ
	*/
	virtual glbInt32             GetFeatureCount()=0;
	/**
	* @brief ��ȡҪ��
	*
	* @param oid Ҫ��id
	* @return    Ҫ��,ʧ��:NULL
	*/
	virtual CGlbFeature*         GetFeature(glbInt32 oid) = 0;
	/**
	* @brief ����Ҫ��
	*
	* @param feature Ҫ��
	* @return        �ɹ�:true��ʧ��:false
	*/
	virtual glbBool	             UpdateFeature(CGlbFeature* feature) = 0;
	/**
	* @brief ɾ��Ҫ��
	*
	* @param oid Ҫ��id
	* @return    �ɹ�:true��ʧ��:false
	*/
	virtual glbBool              DeleteFeature(glbInt32 oid) = 0;
	/**
	* @brief ɾ��һ��Ҫ��
	*
	* @param queryfilter ������
	* @return            �ɹ�:true��ʧ��:false
	*/
	virtual glbBool              DeleteFeatures(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief ��ȡ��ѯ�α�
	*
	* @param queryfilter ������
	* @return            �α�,ʧ��NULL
	*/
	virtual IGlbFeatureCursor*       Query(CGlbQueryFilter* queryfilter) = 0;
	/**
	* @brief ��ȡ�����α�
	*
	* @param queryfilter ������
	* @return            �α�,ʧ��NULL
	*/
	virtual IGlbFeatureUpdateCursor* Update(CGlbQueryFilter* queryfilter)=0;
	/**
	* @brief   ��ȡ�����α�
	*
	* @return  �α�,ʧ��NULL
	*/
	virtual IGlbFeatureInsertCursor* Insert() = 0;
	/**
	* @brief  ��ȡ��������
	*
	* @return ��������
	*/
	virtual GlbGeometryTypeEnum	     GetGeometryType() = 0;
	/**
	* @brief ��Ӽ������ֶ�
	*
	* @param geocls  ������
	* @param field   �ֶ�
	* @return        �ɹ�:true��ʧ��:false
	*/
	virtual glbBool                  AddGeoField(GlbGeometryClassEnum geocls, CGlbField* field) = 0;
	/**
	* @brief ɾ���������ֶ�
	*
	* @param geocls  ������
	* @param fname   �ֶ���
	* @return        �ɹ�:true��ʧ��:false
	*/
	virtual glbBool                  DeleteGeoField(GlbGeometryClassEnum geocls, const glbWChar* fname) = 0;
	/**
	* @brief ��ȡ������
	*
	* @param geocls  ������
	* @return        ���������
	*/
	virtual const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum subgeocls) = 0;
	/**
	* @brief ���ݼ���,���ݶ����oid,��ȡ���������ֶ�ָ�����������
	*
	* @param oid  �����oid
	* @param geo  ��ȡ���������ֶ�����
	* @param level  �������ݵļ���
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool                  GetGeometry(glbInt32 oid,IGlbGeometry** geo,glbInt32 level = 0) = 0;
};
/**
* @brief ��ͨ���ݼ�����ֻ���α�ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB 
*/
class GLBDATAENGINE_API IGlbObjectCursor:public CGlbReference
{
public:	
	/**
	* @brief  ��ȡ�ֶ���Ϣ
	*
	* @return �ֶ���Ϣ,ʧ��:NULL
	*/
	virtual CGlbFields*      GetFields() = 0;
	/**
	* @brief  ��ȡ��
	*
	* @return ������,ʧ��:NULL
	*/
	virtual IGlbObjectClass* GetClass() = 0;
	/**
	* @brief �ز�ѯ
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool          Requery() = 0;
	/**
	* @brief �α��ƶ�����һ��
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool          MoveNext() = 0;
	/**
	* @brief ��ȡ��������
	*
	* @return ���ض����ܸ���
	*/
	virtual glbInt32         GetCount()=0;
	/**
	* @brief ��ȡ��ǰ����
	*
	* @return ���ظö���
	*/
	virtual CGlbObject*      GetObject() = 0;
	/**
	* @brief ��ȡ���һ��ִ�еĴ�����Ϣ
	*
	* @return ���ش�����Ϣ
	*/
	virtual glbWChar*        GetLastError() = 0;
};
/**
* @brief ��ͨ��������α�ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbObjectUpdateCursor:public IGlbObjectCursor
{
public:	
	/**
	* @brief ���¶���
	*
	* @param obj  ����
	* @return     �ɹ�:true��ʧ��:false
	*/
	virtual glbBool Update(CGlbObject* obj) = 0;
	/**
	* @brief ɾ���α���ָ�Ķ���,ɾ����ָ��ǰ��
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool Delete() = 0;
	/**
	* @brief ��ʼ��������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool BeginBatch()   = 0;
	/**
	* @brief ȡ����������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool CancelBatch()  = 0;
	/**
	* @brief �ύ��������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool CommitBatch()  = 0;
};
/**
* @brief ��ͨ��������α�ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbObjectInsertCursor:public CGlbReference
{
public:
	/**
	* @brief  ��ȡ�ֶ���Ϣ
	*
	* @return �ֶ���Ϣ,ʧ��:NULL
	*/
	virtual CGlbFields*      GetFields()             = 0;
	/**
	* @brief ��ȡ��
	*
	* @return ������,ʧ��NULL
	*/
	virtual IGlbObjectClass* GetClass()              = 0;
	/**
	* @brief      ����һ������
	*
	* @param obj  ����
	* @return     �ɹ�:true��ʧ��:false
	*/
	virtual glbBool	         Insert(CGlbObject* obj) = 0;
	/**
	* @brief ��ʼ��������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool          BeginBatch()   = 0;
	/**
	* @brief ȡ����������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool          CancelBatch()  = 0;
	/**
	* @brief �ύ��������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool	         CommitBatch()  = 0;
	/**
	* @brief ��ȡ���һ��ִ�еĴ�����Ϣ
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbWChar*        GetLastError() = 0;
};
/**
* @brief Ҫ�ض���ֻ���α�ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureCursor : public CGlbReference
{
public:
	/**
	* @brief  ��ȡҪ����Ŀ
	*
	* @return Ҫ����Ŀ
	*/
	virtual glbInt32                GetCount()=0;
	/**
	* @brief  ��ȡ�ֶ���Ϣ
	*
	* @return �ֶ���Ϣ,ʧ��:NULL
	*/
	virtual const CGlbFields*       GetFields() = 0;
	/**
	* @brief  ��ȡ��
	*
	* @return Ҫ����,ʧ��:NULL
	*/
	virtual const IGlbFeatureClass* GetClass() = 0;
	/**
	* @brief �ز�ѯ
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool	                Requery() = 0;
	/**
	* @brief �α��ƶ�����һ��
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool	                MoveNext() = 0;
	/**
	* @brief ��ȡ�α굱ǰ��ָ����
	*
	* @return ���ظö���
	*/
	virtual CGlbFeature*            GetFeature() = 0;
	/**
	* @brief ��ȡ���һ��ִ�еĴ�����Ϣ
	*
	* @return ���ش�����Ϣ
	*/
	virtual glbWChar*               GetLastError() = 0;
};
/**
* @brief Ҫ�ض�������α�ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureUpdateCursor : public IGlbFeatureCursor
{
public:	
	/**
	* @brief  ɾ��Ҫ��,ͬʱ�α�����
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool   Delete() = 0;
	/**
	* @brief     ����Ҫ��
	*
	* @param ft  Ҫ��
	* @return    �ɹ�:true��ʧ��:false
	*/
	virtual glbBool   Update(CGlbFeature* ft) = 0;
	/**
	* @brief ��ʼ��������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool   BeginBatch()   = 0;
	/**
	* @brief ȡ����������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool   CancelBatch()  = 0;
	/**
	* @brief �ύ��������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool   CommitBatch()  = 0;
	/**
	* @brief �����Ƿ�Ϊֻ����ģʽ
	*
	* @param isloadOnly  �ǣ�true�����¿ռ�����,��:false
	* @return            �ɹ�:true��ʧ��:false
	*/
	virtual glbBool	  SetUpdateOnly(glbBool isloadOnly)=0;
};
/**
* @brief Ҫ�ض�������α�ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbFeatureInsertCursor:public CGlbReference
{
public:
	/**
	* @brief  ��ȡ�ֶ���Ϣ
	*
	* @return �ֶ���Ϣ,ʧ��:NULL
	*/
	virtual CGlbFields*	      GetFields() = 0;
	/**
	* @brief  ��ȡ��
	*
	* @return Ҫ����,ʧ��:NULL
	*/
	virtual IGlbFeatureClass* GetClass() = 0;
	/**
	* @brief     ����Ҫ��
	*
	* @param ft  Ҫ��
	* @return    �ɹ�:true��ʧ��:false
	*/
	virtual glbBool           Insert(CGlbFeature* ft) = 0;
	/**
	* @brief ��ʼ��������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool           BeginBatch()   = 0;
	/**
	* @brief ȡ����������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool           CancelBatch()  = 0;
	/**
	* @brief �ύ��������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool           CommitBatch()  = 0;
	/**
	* @brief ����Ϊֻ����ģʽ
	*
	* @param isloadOnly  �ǣ�true�����¿ռ�����,��:false
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool           SetLoadOnly(glbBool isloadOnly) = 0;
	/**
	* @brief ��ȡ���һ��ִ�еĴ�����Ϣ
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbWChar*         GetLastError() = 0;	
};
/**
* @brief �������ݼ��ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbSectionDataset:public IGlbFeatureClass
{
public:
	/**
	  * @brief  ȡ���Ƶ����ݼ�
	  *
	  * @return ���Ƶ������ݼ�,,ʧ��NULL
	*/
	virtual IGlbObjectClass* GetCtrlPtClass() = 0;
	/**
	  * @brief  ȡ�ڵ����ݼ�
	  *
	  * @return �ڵ������ݼ�,ʧ��NULL
	  */
	virtual IGlbFeatureClass* GetNodeClass() = 0;
	/**
	  * @brief  ��ȡ�ӻ��������ݼ�
	  *
	  * @return ���������ݼ�,ʧ��NULL
	  */
	virtual IGlbFeatureClass* GetArcClass()=0;
	/**
	  * @brief  ��ȡ����������ݼ�
	  *
	  * @return ����������ݼ�,ʧ��NULL
	  */
	virtual IGlbFeatureClass* GetPolygonClass()=0;
};
/**
* @brief �������ݼ��ӿ�
         ����ID GLBEDGEID ��ʼ�ڵ�ID GLBFNODEID ��ֹ�ڵ�ID GLBTNODEID
*        ��ͨ����
*        {
             �ߡ��ڵ㡢�ϰ��ߡ��ϰ��㡢ת���
*            ����Ȩֵ: ͨ���û��εĻ��ѣ���Ϊ����Ȩֵ(F-->T)������Ȩֵ(T-->F)
*            ��ͨ���򣺵�·Ϊ�������ߡ��������ߡ�˫��ͨ���߻�����ߡ�
*		 }
*        
*        ��ʩ���磺
*        {
              �����ڵ㡢Դ(Source)����(Sink)������Ȩֵ���ϰ�
			  ����0 �뻡�����ֻ�������ͬ
			        1 �뻡�����ֻ������෴
					2 ����ȷ������Ч
					3 ����ͨ ������Դ����Ľڵ㲻��ͨ
			  �ڵ����ͣ�
			        0 ��ͨ�� 1 Դ 2 ��
			  �ȼ�
*        }
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbNetworkDataset:public IGlbFeatureClass
{
public:
	/**
	* @brief  ��ȡ�ڵ������ݼ�
	*         �ڵ�ID GLBNODEID
	*         �㵽���εľ������ޣ������������Ƿ�����������
	* @return �ڵ������ݼ�,ʧ��NULL
	*/
	virtual const IGlbFeatureClass* GetNodeClass()   =0;
	/**
	* @brief  ��ȡת�������ݼ�
	*         ת��ڵ�ID GLBNODEID ��ʼ����ID GLBFEDGEID ��ֹ����ID GLBTEDGEID
	*         ת��Ȩֵ��ͨ����ת��ڵ�Ļ���
	* @return ת�������ݼ�,ʧ��NULL
	*/
	virtual const IGlbObjectClass*  GetTurnClass()   =0;
	/**
	* @brief  ����ת�������ݼ�
	*
	* @return ת�������ݼ�,ʧ��NULL
	*/
	virtual const IGlbObjectClass*  CreateTurnClass()=0;
	/**
	* @brief ɾ��ת�������ݼ�
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool                 DeleteTurnClass()=0;	
};
/**
* @brief ������ݼ��ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbDrillDataset:public IGlbFeatureClass
{
public:
	/**
	* @brief  ��ȡ��б�����ݼ�
	*
	* @return ��б�����ݼ�,ʧ��:NULL
	*/
	virtual IGlbObjectClass* GetSkewClass()=0;
	/**
	* @brief  ��ȡ�ֲ������ݼ�
	*
	* @return �ֲ������ݼ�,ʧ��:NULL
	*/
	virtual IGlbFeatureClass* GetStratumClass()=0;
	/**
	* @brief  ��ȡ��Ʒ�����ݼ���Ŀ
	*
	* @return ��Ʒ�����ݼ�����
	*/
	virtual glbInt32 GetSampleClassCount()=0;
	/**
	* @brief      ��ȡ��Ʒ�����ݼ�
	*
	* @param idx  ��Ʒ������
	* @return     ��Ʒ�����ݼ�,ʧ��:NULL
	*/
	virtual IGlbObjectClass* GetSampleClass(glbInt32 idx)=0;
	/**
	* @brief       ��ȡ��Ʒ�����ݼ�
	*
	* @param name  ��Ʒ������
	* @return      ��Ʒ�����ݼ�,ʧ��:NULL
	*/
	virtual IGlbObjectClass* GetSampleClassByName(const glbWChar* name)=0;
	/**
	* @brief        ������Ʒ�����ݼ�
	*
	* @param name   ��Ʒ������
	* @param alias  ��Ʒ�����
	* @return       ��Ʒ�����ݼ�,ʧ��NULL
	*/
	virtual IGlbObjectClass* CreateSampleClass(const glbWChar* name, const glbWChar* alias)=0;
	/**
	* @brief        ɾ����Ʒ�����ݼ�
	*
	* @param name   ��Ʒ������
	* @return       �ɹ�:true��ʧ��:false
	*/
	virtual glbBool DeleteSampleClass(const glbWChar* name)=0;
};
/**
* @brief դ�����ݼ��洢��ʽ�Ķ���
*
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
struct GlbRasterStorageDef
{
	glbInt32               blockSizeX;	///< x����洢�ֿ��С
	glbInt32               blockSizeY;	///< y����洢�ֿ��С
	GlbPixelLayoutTypeEnum pixelLayout;	///< ���������ʽ
	GlbCompressTypeEnum    compress;    ///< ѹ����ʽ
	glbBool                hasNodata;   ///< �Ƿ���nodata
	glbDouble              nodata;      ///< nodataֵ
};
/**
* @brief �ռ�������Ϣ
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
struct GlbSpatialIndexInfo
{
	GlbSpatialIndexTypeEnum		m_idxType;		///<
	glbDouble					m_grid1XYSize;	///<
	glbDouble					m_grid1ZSize;	///<
	glbDouble					m_grid2XYSize;	///<
	glbDouble					m_grid2ZSize;	///<
	glbDouble					m_grid3XYSize;	///<
	glbDouble					m_grid3ZSize;	///<
};

/**
* @brief ɾ������Դ�ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbRasterDataSource:virtual public IGlbDataSource
{
public:
	/**
	* @brief ��ȡդ�����ݼ�
	*
	* @param datasetName ���ݼ�����
	* @return            դ�����ݼ�,ʧ�ܷ���NULL
	*/
	virtual IGlbRasterDataset* GetRasterDataset(const glbWChar * datasetName) =0;
	/**
	  * @brief ��ȡ�������ݼ�
	  *
	  * @param terrainDatasetName ���ݼ�����
	  * @return                  �������ݼ�,ʧ�ܷ���NULL
	*/
	virtual IGlbTerrainDataset* GetTerrainDataset( const glbWChar* terrainDatasetName)=0;
	/**
	* @brief ����դ�����ݼ�
	*
	* @param name      ���ݼ�����
	* @param alias     ���ݼ�����
	* @param nBands    ������Ŀ,�������0
	* @param pixelType ��������
	* @param columns   ����,�еķ���Ϊx����
	* @param rows      ����,�еķ���Ϊy����
	* @param orgX      ԭ���x����
	* @param orgY      ԭ���y����
	* @param rsX       x����ֱ���
	* @param rsY       y����ֱ���
	* @param format    ���ݼ��ļ���ʽ
	* @param SRS       �ռ�ο�����ϵ
	* @param storeDef  �洢��ʽ�Ķ���
	* @return          դ�����ݼ�,ʧ�ܷ���NULL
	*/
	virtual IGlbRasterDataset* CreateRasterDataset
		   (const glbWChar * name,
		    const glbWChar * alias,
		    glbInt32         nBands,
		    GlbPixelTypeEnum pixelType,
		    glbInt32         columns,
		    glbInt32        rows,
		    glbDouble       orgX   =0,
		    glbDouble       orgY   =0,
		    glbDouble       rsX    =1,
		    glbDouble       rsY    =1,
		    const glbWChar* format =L"GTiff",
		    const glbWChar* SRS    =NULL,
		    const GlbRasterStorageDef *storeDef=NULL)=0;
	/**
	* @brief �����������ݼ�
	*
	* @param name        ���ݼ�����
	* @param alias       ���ݼ�����
	* @param isGlobe     �Ƿ�����
	* @param lzts        0���ֿ鷶Χ
	* @param SRS         �ռ�ο�����ϵ
	* @param domCompress Ӱ��ѹ����ʽ
	* @param demCompress �߳�ѹ����ʽ
	* @return            �������ݼ�,ʧ�ܷ���NULL
	*/
	virtual IGlbTerrainDataset* CreateTerrainDataset(
		const glbWChar*     name,
		const glbWChar*     alias,
		bool                isGlobe    =true,
		glbDouble           lzts       =36,
		const glbWChar*     SRS        =NULL,
		GlbCompressTypeEnum domCompress=GLB_COMPRESSTYPE_JPEG,
		GlbCompressTypeEnum demCompress=GLB_COMPRESSTYPE_ZLIB)=0;
};

/**
* @brief ɾ�����ݼ��ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbRasterDataset:public IGlbGeoDataset
{
public:
	/**
	* @brief ��ȡ���ݼ�ԭ������
	*        ԭ�������Ͻ�,originX = ��Χ.minX
	*                    originY = ��Χ.maxY
	*
	* @param originX ԭ��x����
	* @param originY ԭ��y����
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetOrigin(glbDouble* originX, glbDouble* originY)=0;
	/**
	* @brief ��ȡ���ݼ���������
	*
	* @param columns ����
	* @param rows    ����
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetSize(glbInt32* columns, glbInt32* rows)= 0;
	/**
	* @brief ��ȡ���ݼ��ķֱ���
	*
	* @param rsX x����ֱ���
	* @param rsY y����ֱ���
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetRS(glbDouble* rsX, glbDouble* rsY)= 0;
	/**
	* @brief ��ȡ���ݼ���������
	*
	* @return ������������
	*/
	virtual GlbPixelTypeEnum GetPixelType()=0;
	/**
	* @brief ��ȡ���ݼ����ز���
	*
	* @return �������ز���
	*/
	virtual GlbPixelLayoutTypeEnum GetPixelLayout()=0;
	/**
	* @brief ��ȡ���ݼ�ѹ������
	*
	* @return ����ѹ������
	*/
	virtual const glbWChar* GetCompressType()=0;
	/**
	* @brief ��ȡ�ļ���ʽ
	*
	* @return ���ر����ʽ
	*/
	virtual const glbWChar*	GetFormat()=0 ;
	/**
	* @brief ��ȡ���ݼ�����Чֵ
	*        ��֧�ֲ�ͬ�����в�ͬ����Чֵ
	*
	* @param noDataVal ��Чֵ
	* @return �ɹ�:����Чֵ��ʧ��:û����Чֵ
	*/
	virtual glbBool GetNoData(glbDouble* noDataVal)=0 ;
	/**
	* @brief �������ݼ�����Чֵ
	*        ��֧�ֲ�ͬ�����в�ͬ����Чֵ
	*
	* @param noDataVal ��Чֵ
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool AlterNoData(glbDouble noDataVal)=0 ;
	/**
	* @brief ��ȡ���ݼ�������
	*
	* @return ���ز�����
	*/
	virtual glbInt32 GetBandCount() = 0;
	/**
	* @brief ����������ȡ���ݼ�����
	*
	* @return ���ز���
	*/
	virtual const IGlbRasterBand* GetBand(glbInt32 bandIndex) =0;
	/**
	* @brief �������ݼ��ķֿ��С
	*
	* @param blockSizeX x�������ظ���
	* @param blockSizeY y�������ظ���
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetBlockSize(glbInt32* blockSizeX,glbInt32* blockSizeY)=0;
	/**
	* @brief ��ָ��������������һ�����ؿ�
	*
	* @param columns ����
	* @param rows ����
	* @return ���ش��������ؿ�
	*/
	virtual CGlbPixelBlock* CreatePixelBlock(glbInt32 columns, glbInt32 rows) = 0;
	/**
	* @brief ��ȡһ�����ؿ�
	*        ����Ƿֿ�洢��,��GetBlockSize�Ĵ�С��ȡЧ�����
	*        û�����ݵĲ�������ֵ��ʼ��Ϊ��Чֵ,��Ӧ������ֵΪ0.
	*
	* @param startColum  ��ʼ��>=0
	* @param startRow    ��ʼ��>=0
	* @param pPixelBlock ��ȡ�����ؿ�
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool Read(glbInt32 startColum, glbInt32 startRow, CGlbPixelBlock *pPixelBlock)=0;
	/**
	* @brief ��ȡһ�����ؿ�
	*        ���д�����ݷֱ��������ݼ��ֱ��ʲ�һ��,�ز���.
	*
	* @param destExtent  ��Χ
	* @param pPixelBlock ��ȡ�����ؿ�
	* @param rspType     ������ʽ
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool ReadEx(const CGlbExtent *destExtent,
		                   CGlbPixelBlock   *pPixelBlock,
						   GlbRasterResampleTypeEnum rspType)=0;
	/**
	* @brief д��һ�����ؿ�
	*
	* @param  startColum  ��ʼ��
	* @param  startRow    ��ʼ��
	* @param  pPixelBlock д������ؿ�
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool Write(glbInt32 startColum, glbInt32 startRow, CGlbPixelBlock *pPixelBlock)=0;
	/**
	* @brief �ж����ݼ��Ƿ��н���������
	*
	* @return ��:true��û��:false
	*/
	virtual glbBool HasPyramid() =0;
	/**
	* @brief ����������
	*        ���isShareEdge,�Ǵ���DEM�������������Ǵ���DOM������.
	*        DEM��������
	*                 ֻ֧��1�������ݼ�
	*                 ����������ֵ������Float32
	*        DOM��������
	*                 ֻ֧��3���λ�4�������ݼ�
	*                 ����������������BYTE.
	*
	* @param srcCol      ԭʼ������ʼ��
	* @param srcRow      ԭʼ������ʼ��        
	* @param srcColumns  ԭʼ��������
	* @param srcRows     ԭʼ��������
	* @param rspType     ������ʽ
	* @param tileSizeX   ��ƬX�������ظ���
	* @param tileSizeY   ��ƬY�������ظ���
	* @param lzts        0����Ƭ��Χ
	* @param isGlobe     �Ƿ�����ģʽ
	* @param isShareEdge �Ƿ���߽�,�߳����ݱ��빲���
	* @param isReplace   �Ƿ��滻value(ֻ��Ը߳�)
	* @param rpValue     �滻ֵ(ֻ��Ը߳�)
	* @param progress    �����������Ľ�����Ϣ��ȡ�ӿ�
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool BuildPyramid(
		glbInt32  srcCol,
		glbInt32  srcRow,
		glbInt32  srcColumns,
		glbInt32  srcRows,
		GlbRasterResampleTypeEnum rspType,
		glbInt32  tileSizeX,
		glbInt32  tileSizeY,
		glbDouble lzts        = 36,
		glbBool   isGlobe     = true, 
		glbBool   isShareEdge = false,
		glbBool   isReplace   =false,
		glbDouble rpValue     =0.0,
		IGlbProgress* progress=0)=0;
	/**
	* @brief ɾ�����ݼ�������
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool DeletePyramid() = 0;
	/**
	* @brief ��ȡ���������ز��ַ�ʽ
	*
	* @return ���ؽ�������������
	*/
	virtual GlbPixelLayoutTypeEnum GetPyramidPixelLayout()=0;
	/**
	* @brief ��ȡ��������������
	*
	* @return ���ؽ�������������
	*/
	virtual GlbPixelTypeEnum GetPyramidPixelType()=0;
	/**
	* @brief  ��ȡ������0����Ƭ��Χ
	*
	* @return ��Ƭ��Χ
	*/
	virtual glbDouble GetPyramidLZTS()=0;
	/**
	* @brief ��ȡ�������Ƿ���߽�
	*
	* @return ����:true��������:false
	*/
	virtual glbBool IsPyramidShareEdge() =0;
	/**
	* @brief ��ȡ����������
	*
	* @param minLevel ��ȡ��С����
	* @param maxLevel ��ȡ��󼶱�
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool	GetPyramidLevels(glbInt32* minLevel, glbInt32* maxLevel) =0;
	/**
	* @brief ��ȡĳһ����������ķֱ���
	*
	* @param level ������
	* @param rsX ��ȡx����ֱ���
	* @param rsY ��ȡy����ֱ���
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetPyramidLevelRS(glbInt32 level, glbDouble* rsX, glbDouble* rsY)=0;
	/**
	  * @brief ��ȡ��������Ƭ���б�ŷ�Χ
	  *
	  * @param iLevel            ����
	  * @param startTileColumn   ���ؿ�ʼ�б��
	  * @param startTileRow      ���ؿ�ʼ�б��
	  * @param endTileColumn     ���ؽ����б��
	  * @param endTileRow        ���ؽ����б��
	  * @return �ɹ�:true��ʧ��:false
	  */
	virtual glbBool GetPyramidTileIndexes(glbInt32 iLevel,glbInt32& startTileColumn,glbInt32& startTileRow,
		glbInt32& endTileColumn,glbInt32& endTileRow)=0;
	/**
	  * @brief ��ȡ�ռ䷶Χ�ڽ�������Ƭ���б�ŷ�Χ
	  *
	  * @param extent            �ռ䷶Χ            
	  * @param iLevel            ����
	  * @param startTileColumn   ���ؿ�ʼ�б��
	  * @param startTileRow      ���ؿ�ʼ�б��
	  * @param endTileColumn     ���ؽ����б��
	  * @param endTileRow        ���ؽ����б��
	  * @return �ɹ�:true��ʧ��:false
	  */
	virtual glbBool GetExtentCoverTiles(const CGlbExtent* extent, glbInt32 iLevel,glbInt32& startTileColumn, glbInt32& startTileRow,
		glbInt32& endTileColumn, glbInt32& endTileRow)=0;
	/**
	  * @brief ��ȡһ���������Ƭ�Ŀռ䷶Χ
	  *      
	  * @param iLevel            ����
	  * @param startTileColumn   ��ʼ�б��
	  * @param startTileRow      ��ʼ�б��
	  * @param endTileColumn     �����б��
	  * @param endTileRow        �����б��
	  * @param extent            �ռ䷶Χ 
	  * @return �ɹ�:true��ʧ��:false
	  */
	virtual glbBool GetTilesCoverExtent(glbInt32 iLevel,glbInt32 startTileColumn, glbInt32 startTileRow,glbInt32 endTileColumn, 
		glbInt32 endTileRow,CGlbExtent *extent)=0;
	/**
	* @brief ��ȡ�������ķ�Χ
	*
	* @return ���ط�Χ��ʧ�ܷ���NULL
	*/
	virtual glbBool GetLevelExtent(glbInt32 ilevel, CGlbExtent* ext) = 0;

	/**
	* @brief ��ȡ����������Ƭ��С
	*
	* @param blockSizeX ����x�������ظ���
	* @param blockSizeY ����y�������ظ���
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetPyramidBlockSize(glbInt32* blockSizeX, glbInt32* blockSizeY) =0;

	/**
	  * @brief ����һ�����������ؿ�
	  *
	  * @return ���ش��������ؿ飬ʧ�ܷ���NULL
	  */
	virtual CGlbPixelBlock* CreatePyramidPixelBlock() = 0;

	/**
	  * @brief ��ȡ��������Ƭ
	  *
	  * @param level        ����
	  * @param tileColumn   ��
	  * @param tileRow      ��
	  * @param pPixelBlock  ���ؿ�
	  * @return �ɹ�:true��ʧ��:false
	  */
	virtual glbBool ReadPyramidByTile(glbInt32 level,
		                              glbInt32 tileColumn,
		                              glbInt32 tileRow,
		                              CGlbPixelBlock* pPixelBlock)=0;
	/**
	* @brief д���������Ƭ
	*
	* @param level      ����
	* @param tileColumn ��
	* @param tileRow    ��
	* @param pPixelBlock���ؿ�
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool WritePyramidByTile(glbInt32 level,
		                               glbInt32 tileColumn,
		                               glbInt32 tileRow,
		                               CGlbPixelBlock* pPixelBlock)=0;
	/**
	* @brief �����Զ����Ϳ�ָ������
	*
	* @param extent   ָ������
	* @param isMove  �Ƿ��Ϳ�
	*/
	virtual void SetSelfDealMoveExtent(CGlbExtent *extent,glbBool isMove);
	/**
	* @brief �����Զ���ѹƽָ������
	*
	* @param extent   ָ������
	* @param value     �޸Ľ��ֵ
	* @param isChange �Ƿ��޸�
	*/
	virtual void SetSelfDealChangeExtent(CGlbExtent *extent,glbDouble value,glbBool isChange);
};
/**
* @brief ���εĽӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class GLBDATAENGINE_API IGlbRasterBand:public CGlbReference
{
public:
	/**
	* @brief ��ȡ����id
	*
	* @return ����id
	*/
	virtual glbInt32 GetID()=0;
	/**
	* @brief ��ȡ������
	*
	* @return ���ز�����
	*/
	virtual const glbWChar* GetName()=0;
};
struct GlbTileNeighbor
{
	glbBool hasLeft;   ///<��Ƭ�Ƿ������ھ�
	glbBool hasRight;  ///<��Ƭ�Ƿ������ھ�
	glbBool hasUp;     ///<��Ƭ�Ƿ������ھ�
	glbBool hasDown;   ///<��Ƭ�Ƿ������ھ�
	glbBool hasLeftUp; ///<��Ƭ�Ƿ��������ھ�
	glbBool hasRightUp;///<��Ƭ�Ƿ��������ھ�
	glbBool hasLeftDown;///<��Ƭ�Ƿ��������ھ�
	glbBool hasRightDown;///<��Ƭ�Ƿ��������ھ�
};
/**
* @brief �������ݼ��ӿ�
*
* @version 1.0
* @date    2014-03-10 11:21
* @author  GWB
*/
class  GLBDATAENGINE_API IGlbTerrainDataset:public IGlbGeoDataset
{
public:
	/**
	* @brief      ��ȡ0����Ƭ��Χ��С
	*
	* @param lzts ��Ƭ��С
	* @return     �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetLZTS(glbDouble& lzts)=0;
	/**
	* @brief �Ƿ�����ģʽ
	*
	* @return true������ģʽ��false��ƽ��ģʽ
	*/
	virtual glbBool IsGlobe()=0;
	/**
	* @brief ��ȡӰ����������
	*
	* @return ����Ӱ����������
	*/
	virtual GlbPixelTypeEnum GetDomPixelType()=0;
	/**
	* @brief ��ȡ�߳���������
	*
	* @return ���ظ߳���������
	*/
	virtual GlbPixelTypeEnum GetDemPixelType()=0;
	/**
	* @brief ��ȡӰ��������������
	*
	* @return ����Ӱ��������������
	*/
	virtual GlbPixelLayoutTypeEnum GetDomPixelLayout()=0;
	/**
	* @brief ��ȡ�߳�������������
	*
	* @return ���ظ߳�������������
	*/
	virtual GlbPixelLayoutTypeEnum GetDemPixelLayout()=0;
	/**
	* @brief ����Ӱ��ķֿ��С
	*
	* @param blockSizeX x��������ظ���
	* @param blockSizeY y��������ظ���
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetDomBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY)=0;
	/**
	* @brief �����̵߳ķֿ��С
	*
	* @param blockSizeX x��������ظ���
	* @param blockSizeY y��������ظ���
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetDemBlockSize(glbInt32& blockSizeX, glbInt32& blockSizeY)=0;
	/**
	* @brief ��ȡӰ�񲨶���
	*
	* @return ����Ӱ�񲨶���
	*/
	virtual glbInt32 GetDomBandCount() = 0;
	/**
	* @brief ��ȡ�̲߳�����
	*
	* @return ���ظ̲߳�����
	*/
	virtual glbInt32 GetDemBandCount() = 0;
	/**
	* @brief ��ȡӰ�񼶱���
	*
	* @param maxLevel ��󼶱�
	* @param minLevel ��С����
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetDomLevels(glbInt32& maxLevel,glbInt32& minLevel)=0;
	/**
	* @brief ��ȡ�̼߳�����
	*
	* @param maxLevel ��󼶱�
	* @param minLevel ��С����
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool GetDemLevels(glbInt32& maxLevel,glbInt32& minLevel)=0;	
	/**
	* @brief ��ȡӰ��Χ
	*
	* @param iLevel ����
	* @param ext    ��Χ
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual glbBool GetDomLevelExtent(glbInt32 iLevel, CGlbExtent* ext)=0;
	/**
	* @brief ��ȡ�̷߳�Χ
	*
	* @param iLevel ����
	* @param ext    ��Χ
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual glbBool GetDemLevelExtent(glbInt32 iLevel, CGlbExtent* ext)=0;
	/** 
	* @brief ��ȡӰ��ֿ�����
	* @brief ����ģʽ:
	*                ԭ����(-180,90),��ʼ�д���СX��ʼ,�����������X
	*                                ��ʼ�д����Y��ʼ,����������СY
	*        ƽ��ģʽ:
	*                ԭ��(0,0)��     ��ʼ�д���СX��ʼ,�����������X
	*                                ��ʼ�д���СY��ʼ,�����������Y
	*
	* @param level           ����
	* @param startTileCol    ��ʼ��ֵ
	* @param startTileRow    ��ʼ��ֵ
	* @param endTileCol      ������ֵ
	* @param endTileRow      ������ֵ
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual glbBool GetDomTileIndexes(glbInt32 level, 
		                              glbInt32& startTileCol,  
									  glbInt32& startTileRow, 
		                              glbInt32& endTileCol,  
									  glbInt32& endTileRow)=0;
	/** 
	* @brief ��ȡ�̷ֿ߳�����
	* @brief ����ģʽ:
	*                ԭ����(-180,90),��ʼ�д���СX��ʼ,�����������X
	*                                ��ʼ�д����Y��ʼ,����������СY
	*        ƽ��ģʽ:
	*                ԭ��(0,0)��     ��ʼ�д���СX��ʼ,�����������X
	*                                ��ʼ�д���СY��ʼ,�����������Y
	*
	* @param level           ����
	* @param startTileCol    ��ʼ��ֵ
	* @param startTileRow    ��ʼ��ֵ
	* @param endTileCol      ������ֵ
	* @param endTileRow      ������ֵ
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual glbBool GetDemTileIndexes(glbInt32 level, 
		                              glbInt32& startTileCol,
									  glbInt32& startTileRow, 
		                              glbInt32& endTileCol,  
									  glbInt32& endTileRow)=0;
	/** 
	* @brief ����Ӱ��Χ�ķֿ�����
	* @brief ����ģʽ:
	*                ԭ����(-180,90),��ʼ�д���СX��ʼ,�����������X
	*                                ��ʼ�д����Y��ʼ,����������СY
	*        ƽ��ģʽ:
	*                ԭ��(0,0)��     ��ʼ�д���СX��ʼ,�����������X
	*                                ��ʼ�д���СY��ʼ,�����������Y
	*
	* @param extent          �ռ䷶Χ
	* @param level           ����
	* @param startTileCol    ��ʼ��ֵ
	* @param startTileRow    ��ʼ��ֵ
	* @param endTileCol      ������ֵ
	* @param endTileRow      ������ֵ
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual glbBool GetDomExtentCoverTiles(const CGlbExtent* extent, 
		                                   glbInt32  level,
										   glbInt32& startTileCol,
										   glbInt32& startTileRow,
		                                   glbInt32& endTileCol,
										   glbInt32& endTileRow)=0;
	/** 
	* @brief ����̷߳�Χ�ķֿ�����
	* @brief ����ģʽ:
	*                ԭ����(-180,90),��ʼ�д���СX��ʼ,�����������X
	*                                ��ʼ�д����Y��ʼ,����������СY
	*        ƽ��ģʽ:
	*                ԭ��(0,0)��     ��ʼ�д���СX��ʼ,�����������X
	*                                ��ʼ�д���СY��ʼ,�����������Y
	*
	* @param extent          �ռ䷶Χ
	* @param level           ����
	* @param startTileCol    ��ʼ��ֵ
	* @param startTileRow    ��ʼ��ֵ
	* @param endTileCol      ������ֵ
	* @param endTileRow      ������ֵ
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual glbBool GetDemExtentCoverTiles(const CGlbExtent* extent, 
		                                  glbInt32  level,
										  glbInt32& startTileCol, 
										  glbInt32& startTileRow,
		                                  glbInt32& endTileCol, 
										  glbInt32& endTileRow)=0;
	/** 
	* @brief ��ȡӰ��һ��ֿ�ķ�Χ
	*
	* @param level           ����
	* @param startTileCol    ��ʼ��ֵ
	* @param startTileRow    ��ʼ��ֵ
	* @param endTileCol      ������ֵ
	* @param endTileRow      ������ֵ
	* @param extent          �ռ䷶Χ
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual glbBool GetDomTilesCoverExtent(glbInt32 level,
		                                   glbInt32 startTileCol, 
										   glbInt32 startTileRow,
		                                   glbInt32 endTileCol, 
										   glbInt32 endTileRow, 
										   CGlbExtent* extent)=0;
	/** 
	* @brief ��ȡ�߳�һ��ֿ�ķ�Χ
	*
	
	* @param level           ����
	* @param startTileCol    ��ʼ��ֵ
	* @param startTileRow    ��ʼ��ֵ
	* @param endTileCol      ������ֵ
	* @param endTileRow      ������ֵ
	* @param extent          �ռ䷶Χ
	* @return �ɹ�����true��ʧ�ܷ���false
	*/
	virtual glbBool GetDemTilesCoverExtent(glbInt32 level,
		                                   glbInt32 startTileCol, 
										   glbInt32 startTileRow,
		                                   glbInt32 endTileCol, 
										   glbInt32 endTileRow, 
										   CGlbExtent* extent)=0;
	/**
	* @brief ����һ��Ӱ�����ؿ�
	*
	* @return ����һ��Ӱ�����ؿ�
	*/
	virtual CGlbPixelBlock* CreateDemPixelBlock()=0;
	/**
	* @brief ����һ���߳����ؿ�
	*
	* @return ����һ���߳����ؿ�
	*/
	virtual CGlbPixelBlock* CreateDomPixelBlock()=0;
	/**
	* @brief ��ȡһ��Ӱ�����ؿ�
	*
	* @param level Ӱ�񼶱�
	* @param startColumn ��ʼ��ȡ������
	* @param startRow ��ʼ��ȡ������
	* @param pixelBlock ��������ؿ�
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool ReadDom(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock)=0;
	/**
	* @brief ��ȡһ���߳����ؿ�
	*
	* @param level �̼߳���
	* @param startColumn ��ʼ��ȡ������
	* @param startRow ��ʼ��ȡ������
	* @param pixelBlock ��������ؿ�
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool ReadDem(glbInt32 level, glbInt32 tileCol,glbInt32 tileRow, CGlbPixelBlock* pixelBlock)=0;
	/**
	* @brief ��ȡ��߷ֱ��ʵ���DEM
	* 
	* @param lonOrX  ���ο麭�ǵľ���
	* @param latOrY  ���ο麭�ǵ�γ��
	* @param pixelBlock ��������ؿ�
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool ReadDem(glbDouble lonOrX,glbDouble latOrY, CGlbPixelBlock* pixelBlock) = 0;
	/**
	* @brief д��һ��Ӱ�����ؿ�
	*
	* @param level      Ӱ�񼶱�
	* @param tileCol    �к�
	* @param tileRow    �к�
	* @param neighbors  �ھ��Ƿ����Left,Right,Up,Down
	* @param pixelBlock ���ؿ�
	* @return �ɹ�:true��ʧ��:false
	*/	
	virtual glbBool WriteDom(glbInt32 level, 
		                     glbInt32 tileCol, 
							 glbInt32 tileRow, 
							 GlbTileNeighbor& neighbors, 
							 CGlbPixelBlock* pixelBlock)=0;
	/**
	* @brief д��һ���߳̿�
	*
	* @param level       �̼߳���
	* @param tileCol     �к�
	* @param tileRow     �к�
	* @param neighbors   �ھ��Ƿ����Left,Right,Up,Down,LeftUp,RightUp,LeftDown,RightDown
	* @param pixelBlock  ���ؿ�
	* @return �ɹ�:true��ʧ��:false
	*/										 
	virtual glbBool WriteDem(glbInt32 level, 
		                     glbInt32 tileCol,
							 glbInt32 tileRow, 
							 GlbTileNeighbor& neighbors,
							 CGlbPixelBlock*  pixelBlock)=0;	
	/**
	* @brief �������DEM�ı߽磨-180��180��
	*
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool MergerGlobeDem()=0;
	/**
	* @brief ����Ӱ�����ݼ�
	*
	* @param dataset  Ӱ�����ݼ�
	* @param minLevel ��С����
	* @param maxLevel �����
	* @param progress ������Ϣ
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool ImportDomDataset(IGlbRasterDataset* dataset,
		                             glbInt32 minLevel,
								     glbInt32 maxLevel,
		                             IGlbProgress* progress=NULL)=0;
	/**
	* @brief ����߳����ݼ�
	*
	* @param dataset  դ�����ݼ�
	* @param minLevel ��С����
	* @param maxLevel �����
	* @param progress ������Ϣ
	* @return �ɹ�:true��ʧ��:false
	*/
	virtual glbBool ImportDemDataset(IGlbRasterDataset* dataset,
		                             glbInt32 minLevel,
								     glbInt32 maxLevel,
		                             IGlbProgress* progress=NULL)=0;
};

GLBDATAENGINE_API extern glbInt32	 getPixelSize(GlbPixelTypeEnum pixelType);
GLBDATAENGINE_API extern GLBVARIANT* defautToVariant(CGlbField* field);
