// dllmain.h : 模块类的声明。

class CcwMineModule : public CAtlDllModuleT< CcwMineModule >
{
public :
	DECLARE_LIBID(LIBID_cwMineLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CWMINE, "{820BAC28-D61C-466E-B2F5-637E3B8A2183}")
};

extern class CcwMineModule _AtlModule;
