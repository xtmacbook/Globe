

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Wed Nov 30 16:57:42 2016
 */
/* Compiler settings for cwMine.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __cwMine_i_h__
#define __cwMine_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IComponentRegistrar_FWD_DEFINED__
#define __IComponentRegistrar_FWD_DEFINED__
typedef interface IComponentRegistrar IComponentRegistrar;
#endif 	/* __IComponentRegistrar_FWD_DEFINED__ */


#ifndef __IMenu_MineModel_FWD_DEFINED__
#define __IMenu_MineModel_FWD_DEFINED__
typedef interface IMenu_MineModel IMenu_MineModel;
#endif 	/* __IMenu_MineModel_FWD_DEFINED__ */


#ifndef __ICommand_PriorProbability_FWD_DEFINED__
#define __ICommand_PriorProbability_FWD_DEFINED__
typedef interface ICommand_PriorProbability ICommand_PriorProbability;
#endif 	/* __ICommand_PriorProbability_FWD_DEFINED__ */


#ifndef __ICommand_EvidenceMethod_FWD_DEFINED__
#define __ICommand_EvidenceMethod_FWD_DEFINED__
typedef interface ICommand_EvidenceMethod ICommand_EvidenceMethod;
#endif 	/* __ICommand_EvidenceMethod_FWD_DEFINED__ */


#ifndef __ICommand_PosteriorProbability_FWD_DEFINED__
#define __ICommand_PosteriorProbability_FWD_DEFINED__
typedef interface ICommand_PosteriorProbability ICommand_PosteriorProbability;
#endif 	/* __ICommand_PosteriorProbability_FWD_DEFINED__ */


#ifndef __ICommand_Evidence_FWD_DEFINED__
#define __ICommand_Evidence_FWD_DEFINED__
typedef interface ICommand_Evidence ICommand_Evidence;
#endif 	/* __ICommand_Evidence_FWD_DEFINED__ */


#ifndef __ICommand_InformationMethod_FWD_DEFINED__
#define __ICommand_InformationMethod_FWD_DEFINED__
typedef interface ICommand_InformationMethod ICommand_InformationMethod;
#endif 	/* __ICommand_InformationMethod_FWD_DEFINED__ */


#ifndef __IMenu_DataStatistics_FWD_DEFINED__
#define __IMenu_DataStatistics_FWD_DEFINED__
typedef interface IMenu_DataStatistics IMenu_DataStatistics;
#endif 	/* __IMenu_DataStatistics_FWD_DEFINED__ */


#ifndef __ICmd_DataStatistics_FWD_DEFINED__
#define __ICmd_DataStatistics_FWD_DEFINED__
typedef interface ICmd_DataStatistics ICmd_DataStatistics;
#endif 	/* __ICmd_DataStatistics_FWD_DEFINED__ */


#ifndef __ICmd_DataTG_FWD_DEFINED__
#define __ICmd_DataTG_FWD_DEFINED__
typedef interface ICmd_DataTG ICmd_DataTG;
#endif 	/* __ICmd_DataTG_FWD_DEFINED__ */


#ifndef __ICmd_TongJi_FWD_DEFINED__
#define __ICmd_TongJi_FWD_DEFINED__
typedef interface ICmd_TongJi ICmd_TongJi;
#endif 	/* __ICmd_TongJi_FWD_DEFINED__ */


#ifndef __CompReg_FWD_DEFINED__
#define __CompReg_FWD_DEFINED__

#ifdef __cplusplus
typedef class CompReg CompReg;
#else
typedef struct CompReg CompReg;
#endif /* __cplusplus */

#endif 	/* __CompReg_FWD_DEFINED__ */


#ifndef __Menu_MineModel_FWD_DEFINED__
#define __Menu_MineModel_FWD_DEFINED__

#ifdef __cplusplus
typedef class Menu_MineModel Menu_MineModel;
#else
typedef struct Menu_MineModel Menu_MineModel;
#endif /* __cplusplus */

#endif 	/* __Menu_MineModel_FWD_DEFINED__ */


#ifndef __Command_PriorProbability_FWD_DEFINED__
#define __Command_PriorProbability_FWD_DEFINED__

#ifdef __cplusplus
typedef class Command_PriorProbability Command_PriorProbability;
#else
typedef struct Command_PriorProbability Command_PriorProbability;
#endif /* __cplusplus */

#endif 	/* __Command_PriorProbability_FWD_DEFINED__ */


#ifndef __Command_EvidenceMethod_FWD_DEFINED__
#define __Command_EvidenceMethod_FWD_DEFINED__

#ifdef __cplusplus
typedef class Command_EvidenceMethod Command_EvidenceMethod;
#else
typedef struct Command_EvidenceMethod Command_EvidenceMethod;
#endif /* __cplusplus */

#endif 	/* __Command_EvidenceMethod_FWD_DEFINED__ */


#ifndef __Command_PosteriorProbability_FWD_DEFINED__
#define __Command_PosteriorProbability_FWD_DEFINED__

#ifdef __cplusplus
typedef class Command_PosteriorProbability Command_PosteriorProbability;
#else
typedef struct Command_PosteriorProbability Command_PosteriorProbability;
#endif /* __cplusplus */

#endif 	/* __Command_PosteriorProbability_FWD_DEFINED__ */


#ifndef __Command_Evidence_FWD_DEFINED__
#define __Command_Evidence_FWD_DEFINED__

#ifdef __cplusplus
typedef class Command_Evidence Command_Evidence;
#else
typedef struct Command_Evidence Command_Evidence;
#endif /* __cplusplus */

#endif 	/* __Command_Evidence_FWD_DEFINED__ */


#ifndef __Command_InformationMethod_FWD_DEFINED__
#define __Command_InformationMethod_FWD_DEFINED__

#ifdef __cplusplus
typedef class Command_InformationMethod Command_InformationMethod;
#else
typedef struct Command_InformationMethod Command_InformationMethod;
#endif /* __cplusplus */

#endif 	/* __Command_InformationMethod_FWD_DEFINED__ */


#ifndef __Menu_DataStatistics_FWD_DEFINED__
#define __Menu_DataStatistics_FWD_DEFINED__

#ifdef __cplusplus
typedef class Menu_DataStatistics Menu_DataStatistics;
#else
typedef struct Menu_DataStatistics Menu_DataStatistics;
#endif /* __cplusplus */

#endif 	/* __Menu_DataStatistics_FWD_DEFINED__ */


#ifndef __Cmd_DataStatistics_FWD_DEFINED__
#define __Cmd_DataStatistics_FWD_DEFINED__

#ifdef __cplusplus
typedef class Cmd_DataStatistics Cmd_DataStatistics;
#else
typedef struct Cmd_DataStatistics Cmd_DataStatistics;
#endif /* __cplusplus */

#endif 	/* __Cmd_DataStatistics_FWD_DEFINED__ */


#ifndef __Cmd_DataTG_FWD_DEFINED__
#define __Cmd_DataTG_FWD_DEFINED__

#ifdef __cplusplus
typedef class Cmd_DataTG Cmd_DataTG;
#else
typedef struct Cmd_DataTG Cmd_DataTG;
#endif /* __cplusplus */

#endif 	/* __Cmd_DataTG_FWD_DEFINED__ */


#ifndef __Cmd_TongJi_FWD_DEFINED__
#define __Cmd_TongJi_FWD_DEFINED__

#ifdef __cplusplus
typedef class Cmd_TongJi Cmd_TongJi;
#else
typedef struct Cmd_TongJi Cmd_TongJi;
#endif /* __cplusplus */

#endif 	/* __Cmd_TongJi_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "shobjidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IComponentRegistrar_INTERFACE_DEFINED__
#define __IComponentRegistrar_INTERFACE_DEFINED__

/* interface IComponentRegistrar */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IComponentRegistrar;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a817e7a2-43fa-11d0-9e44-00aa00b6770a")
    IComponentRegistrar : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Attach( 
            /* [in] */ BSTR bstrPath) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RegisterAll( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UnregisterAll( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetComponents( 
            /* [out] */ SAFEARRAY * *pbstrCLSIDs,
            /* [out] */ SAFEARRAY * *pbstrDescriptions) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RegisterComponent( 
            /* [in] */ BSTR bstrCLSID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UnregisterComponent( 
            /* [in] */ BSTR bstrCLSID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComponentRegistrarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IComponentRegistrar * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IComponentRegistrar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IComponentRegistrar * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IComponentRegistrar * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IComponentRegistrar * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IComponentRegistrar * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IComponentRegistrar * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Attach )( 
            IComponentRegistrar * This,
            /* [in] */ BSTR bstrPath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RegisterAll )( 
            IComponentRegistrar * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterAll )( 
            IComponentRegistrar * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetComponents )( 
            IComponentRegistrar * This,
            /* [out] */ SAFEARRAY * *pbstrCLSIDs,
            /* [out] */ SAFEARRAY * *pbstrDescriptions);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RegisterComponent )( 
            IComponentRegistrar * This,
            /* [in] */ BSTR bstrCLSID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterComponent )( 
            IComponentRegistrar * This,
            /* [in] */ BSTR bstrCLSID);
        
        END_INTERFACE
    } IComponentRegistrarVtbl;

    interface IComponentRegistrar
    {
        CONST_VTBL struct IComponentRegistrarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComponentRegistrar_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IComponentRegistrar_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IComponentRegistrar_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IComponentRegistrar_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IComponentRegistrar_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IComponentRegistrar_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IComponentRegistrar_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IComponentRegistrar_Attach(This,bstrPath)	\
    ( (This)->lpVtbl -> Attach(This,bstrPath) ) 

#define IComponentRegistrar_RegisterAll(This)	\
    ( (This)->lpVtbl -> RegisterAll(This) ) 

#define IComponentRegistrar_UnregisterAll(This)	\
    ( (This)->lpVtbl -> UnregisterAll(This) ) 

#define IComponentRegistrar_GetComponents(This,pbstrCLSIDs,pbstrDescriptions)	\
    ( (This)->lpVtbl -> GetComponents(This,pbstrCLSIDs,pbstrDescriptions) ) 

#define IComponentRegistrar_RegisterComponent(This,bstrCLSID)	\
    ( (This)->lpVtbl -> RegisterComponent(This,bstrCLSID) ) 

#define IComponentRegistrar_UnregisterComponent(This,bstrCLSID)	\
    ( (This)->lpVtbl -> UnregisterComponent(This,bstrCLSID) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IComponentRegistrar_INTERFACE_DEFINED__ */


#ifndef __IMenu_MineModel_INTERFACE_DEFINED__
#define __IMenu_MineModel_INTERFACE_DEFINED__

/* interface IMenu_MineModel */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IMenu_MineModel;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("03604537-A338-4256-BCF4-DD871697ACEB")
    IMenu_MineModel : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMenu_MineModelVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMenu_MineModel * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMenu_MineModel * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMenu_MineModel * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMenu_MineModel * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMenu_MineModel * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMenu_MineModel * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMenu_MineModel * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IMenu_MineModelVtbl;

    interface IMenu_MineModel
    {
        CONST_VTBL struct IMenu_MineModelVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMenu_MineModel_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMenu_MineModel_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMenu_MineModel_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMenu_MineModel_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMenu_MineModel_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMenu_MineModel_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMenu_MineModel_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMenu_MineModel_INTERFACE_DEFINED__ */


#ifndef __ICommand_PriorProbability_INTERFACE_DEFINED__
#define __ICommand_PriorProbability_INTERFACE_DEFINED__

/* interface ICommand_PriorProbability */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICommand_PriorProbability;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C4C9C43B-4543-4BE0-B4C6-ACCBFCC4EC9F")
    ICommand_PriorProbability : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICommand_PriorProbabilityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICommand_PriorProbability * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICommand_PriorProbability * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICommand_PriorProbability * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICommand_PriorProbability * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICommand_PriorProbability * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICommand_PriorProbability * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICommand_PriorProbability * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICommand_PriorProbabilityVtbl;

    interface ICommand_PriorProbability
    {
        CONST_VTBL struct ICommand_PriorProbabilityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICommand_PriorProbability_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICommand_PriorProbability_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICommand_PriorProbability_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICommand_PriorProbability_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICommand_PriorProbability_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICommand_PriorProbability_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICommand_PriorProbability_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICommand_PriorProbability_INTERFACE_DEFINED__ */


#ifndef __ICommand_EvidenceMethod_INTERFACE_DEFINED__
#define __ICommand_EvidenceMethod_INTERFACE_DEFINED__

/* interface ICommand_EvidenceMethod */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICommand_EvidenceMethod;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6AA9DE39-38E3-4C78-9088-F2A43AC2D332")
    ICommand_EvidenceMethod : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICommand_EvidenceMethodVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICommand_EvidenceMethod * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICommand_EvidenceMethod * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICommand_EvidenceMethod * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICommand_EvidenceMethod * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICommand_EvidenceMethod * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICommand_EvidenceMethod * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICommand_EvidenceMethod * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICommand_EvidenceMethodVtbl;

    interface ICommand_EvidenceMethod
    {
        CONST_VTBL struct ICommand_EvidenceMethodVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICommand_EvidenceMethod_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICommand_EvidenceMethod_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICommand_EvidenceMethod_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICommand_EvidenceMethod_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICommand_EvidenceMethod_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICommand_EvidenceMethod_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICommand_EvidenceMethod_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICommand_EvidenceMethod_INTERFACE_DEFINED__ */


#ifndef __ICommand_PosteriorProbability_INTERFACE_DEFINED__
#define __ICommand_PosteriorProbability_INTERFACE_DEFINED__

/* interface ICommand_PosteriorProbability */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICommand_PosteriorProbability;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4737EDC0-1198-44A1-ACD3-A7DC7D369578")
    ICommand_PosteriorProbability : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICommand_PosteriorProbabilityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICommand_PosteriorProbability * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICommand_PosteriorProbability * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICommand_PosteriorProbability * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICommand_PosteriorProbability * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICommand_PosteriorProbability * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICommand_PosteriorProbability * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICommand_PosteriorProbability * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICommand_PosteriorProbabilityVtbl;

    interface ICommand_PosteriorProbability
    {
        CONST_VTBL struct ICommand_PosteriorProbabilityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICommand_PosteriorProbability_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICommand_PosteriorProbability_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICommand_PosteriorProbability_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICommand_PosteriorProbability_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICommand_PosteriorProbability_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICommand_PosteriorProbability_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICommand_PosteriorProbability_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICommand_PosteriorProbability_INTERFACE_DEFINED__ */


#ifndef __ICommand_Evidence_INTERFACE_DEFINED__
#define __ICommand_Evidence_INTERFACE_DEFINED__

/* interface ICommand_Evidence */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICommand_Evidence;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2B162E03-0D07-4ADC-B9E7-E7A6D23C45CB")
    ICommand_Evidence : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICommand_EvidenceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICommand_Evidence * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICommand_Evidence * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICommand_Evidence * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICommand_Evidence * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICommand_Evidence * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICommand_Evidence * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICommand_Evidence * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICommand_EvidenceVtbl;

    interface ICommand_Evidence
    {
        CONST_VTBL struct ICommand_EvidenceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICommand_Evidence_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICommand_Evidence_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICommand_Evidence_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICommand_Evidence_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICommand_Evidence_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICommand_Evidence_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICommand_Evidence_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICommand_Evidence_INTERFACE_DEFINED__ */


#ifndef __ICommand_InformationMethod_INTERFACE_DEFINED__
#define __ICommand_InformationMethod_INTERFACE_DEFINED__

/* interface ICommand_InformationMethod */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICommand_InformationMethod;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DC60E2BC-8B45-4E76-8740-702563CAF3CD")
    ICommand_InformationMethod : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICommand_InformationMethodVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICommand_InformationMethod * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICommand_InformationMethod * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICommand_InformationMethod * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICommand_InformationMethod * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICommand_InformationMethod * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICommand_InformationMethod * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICommand_InformationMethod * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICommand_InformationMethodVtbl;

    interface ICommand_InformationMethod
    {
        CONST_VTBL struct ICommand_InformationMethodVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICommand_InformationMethod_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICommand_InformationMethod_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICommand_InformationMethod_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICommand_InformationMethod_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICommand_InformationMethod_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICommand_InformationMethod_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICommand_InformationMethod_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICommand_InformationMethod_INTERFACE_DEFINED__ */


#ifndef __IMenu_DataStatistics_INTERFACE_DEFINED__
#define __IMenu_DataStatistics_INTERFACE_DEFINED__

/* interface IMenu_DataStatistics */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IMenu_DataStatistics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AF685B94-A3AF-4256-9CAB-C8C19038DAB0")
    IMenu_DataStatistics : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMenu_DataStatisticsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMenu_DataStatistics * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMenu_DataStatistics * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMenu_DataStatistics * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMenu_DataStatistics * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMenu_DataStatistics * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMenu_DataStatistics * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMenu_DataStatistics * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IMenu_DataStatisticsVtbl;

    interface IMenu_DataStatistics
    {
        CONST_VTBL struct IMenu_DataStatisticsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMenu_DataStatistics_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMenu_DataStatistics_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMenu_DataStatistics_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMenu_DataStatistics_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMenu_DataStatistics_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMenu_DataStatistics_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMenu_DataStatistics_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMenu_DataStatistics_INTERFACE_DEFINED__ */


#ifndef __ICmd_DataStatistics_INTERFACE_DEFINED__
#define __ICmd_DataStatistics_INTERFACE_DEFINED__

/* interface ICmd_DataStatistics */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICmd_DataStatistics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("909B64C7-109C-462F-952D-EB5217A35CA0")
    ICmd_DataStatistics : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICmd_DataStatisticsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICmd_DataStatistics * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICmd_DataStatistics * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICmd_DataStatistics * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICmd_DataStatistics * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICmd_DataStatistics * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICmd_DataStatistics * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICmd_DataStatistics * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICmd_DataStatisticsVtbl;

    interface ICmd_DataStatistics
    {
        CONST_VTBL struct ICmd_DataStatisticsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICmd_DataStatistics_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICmd_DataStatistics_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICmd_DataStatistics_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICmd_DataStatistics_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICmd_DataStatistics_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICmd_DataStatistics_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICmd_DataStatistics_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICmd_DataStatistics_INTERFACE_DEFINED__ */


#ifndef __ICmd_DataTG_INTERFACE_DEFINED__
#define __ICmd_DataTG_INTERFACE_DEFINED__

/* interface ICmd_DataTG */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICmd_DataTG;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E446B88E-A064-41C1-B8C7-0D27A7D9691B")
    ICmd_DataTG : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICmd_DataTGVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICmd_DataTG * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICmd_DataTG * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICmd_DataTG * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICmd_DataTG * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICmd_DataTG * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICmd_DataTG * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICmd_DataTG * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICmd_DataTGVtbl;

    interface ICmd_DataTG
    {
        CONST_VTBL struct ICmd_DataTGVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICmd_DataTG_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICmd_DataTG_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICmd_DataTG_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICmd_DataTG_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICmd_DataTG_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICmd_DataTG_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICmd_DataTG_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICmd_DataTG_INTERFACE_DEFINED__ */


#ifndef __ICmd_TongJi_INTERFACE_DEFINED__
#define __ICmd_TongJi_INTERFACE_DEFINED__

/* interface ICmd_TongJi */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICmd_TongJi;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2B8263D8-933E-4E11-84D6-B7E116CCF7E0")
    ICmd_TongJi : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ICmd_TongJiVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICmd_TongJi * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICmd_TongJi * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICmd_TongJi * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICmd_TongJi * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICmd_TongJi * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICmd_TongJi * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICmd_TongJi * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } ICmd_TongJiVtbl;

    interface ICmd_TongJi
    {
        CONST_VTBL struct ICmd_TongJiVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICmd_TongJi_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICmd_TongJi_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICmd_TongJi_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICmd_TongJi_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICmd_TongJi_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICmd_TongJi_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICmd_TongJi_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICmd_TongJi_INTERFACE_DEFINED__ */



#ifndef __cwMineLib_LIBRARY_DEFINED__
#define __cwMineLib_LIBRARY_DEFINED__

/* library cwMineLib */
/* [custom][helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_cwMineLib;

EXTERN_C const CLSID CLSID_CompReg;

#ifdef __cplusplus

class DECLSPEC_UUID("DC96D3E1-8C76-4C22-87E3-F6DBCDFB7C35")
CompReg;
#endif

EXTERN_C const CLSID CLSID_Menu_MineModel;

#ifdef __cplusplus

class DECLSPEC_UUID("0C3FBA56-D531-4FAD-8BDB-7F929423C7AF")
Menu_MineModel;
#endif

EXTERN_C const CLSID CLSID_Command_PriorProbability;

#ifdef __cplusplus

class DECLSPEC_UUID("36F1C1B5-1DA9-4C9F-A73A-FA0D56A7D916")
Command_PriorProbability;
#endif

EXTERN_C const CLSID CLSID_Command_EvidenceMethod;

#ifdef __cplusplus

class DECLSPEC_UUID("22E27E50-0256-4266-9626-BB760B7FFE35")
Command_EvidenceMethod;
#endif

EXTERN_C const CLSID CLSID_Command_PosteriorProbability;

#ifdef __cplusplus

class DECLSPEC_UUID("AB193386-17DF-4AC7-91B8-9958480AC5D3")
Command_PosteriorProbability;
#endif

EXTERN_C const CLSID CLSID_Command_Evidence;

#ifdef __cplusplus

class DECLSPEC_UUID("3D66EC1E-FBEC-46DF-BA67-D697239A2E8A")
Command_Evidence;
#endif

EXTERN_C const CLSID CLSID_Command_InformationMethod;

#ifdef __cplusplus

class DECLSPEC_UUID("2DBA37FC-6ECD-4842-82A2-9137C99E61B6")
Command_InformationMethod;
#endif

EXTERN_C const CLSID CLSID_Menu_DataStatistics;

#ifdef __cplusplus

class DECLSPEC_UUID("6ED92A40-4287-43F2-B193-0E1A9064D596")
Menu_DataStatistics;
#endif

EXTERN_C const CLSID CLSID_Cmd_DataStatistics;

#ifdef __cplusplus

class DECLSPEC_UUID("E2D04B13-9307-48F0-BD78-75B775AA2183")
Cmd_DataStatistics;
#endif

EXTERN_C const CLSID CLSID_Cmd_DataTG;

#ifdef __cplusplus

class DECLSPEC_UUID("75B25508-2306-4546-B3F9-0D9AA1022C43")
Cmd_DataTG;
#endif

EXTERN_C const CLSID CLSID_Cmd_TongJi;

#ifdef __cplusplus

class DECLSPEC_UUID("2D2824DF-0E04-46E1-9CCB-078528AB74AB")
Cmd_TongJi;
#endif
#endif /* __cwMineLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long *, unsigned long            , LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long *, unsigned char *, LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long *, LPSAFEARRAY * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


