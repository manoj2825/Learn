//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : intrface.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 18 June 2013
//
//	Description   : Implementation of interface for PScript5, Unidrv5 UI plug-ins.

//===========================================================================

//===========================================================================
//  Reviews 
//
//  Sl#     Review Date     Reviewer(s)               Remarks
//===========================================================================

#include "precomp.h"
#include "ptpc.h"
#include "RegistryApi.h"
#include "JobHandlingDialog.h"
#include "Helper.h"
//For Logging Purpose
#include "MFPLogger.h"
#include "dlg_jobhand01.h"
#include "shregacmib.h"
#include "shregmt.h"
#include "shregus.h"
#include "shregst.h"
#include "shjsonups.h"
#include "shjsonjc.h"
#define	CLASSNAME_UD2LDUMMYWND	L"___DUMMYDIALOG___"
#define SHDEVCAP_BINNAMES_SIZE					 24
#define SHDEVCAP_PAPERNAMES_SIZE				64
typedef ULONG(_stdcall *SSOAPI_GetServiceTicketSize)(LPSTR);
typedef BOOL(_stdcall *SSOAPI_GetServiceTicket)(PBYTE, ULONG);
BOOL RegToDevdmode(char FAR *, LPSCDM);
BOOL WINAPI ExecuteAutoConf(HINSTANCE hInst, WCHAR *pPrinterName, long lMode = config_device);
BOOL WINAPI ExecuteSilentAutoConf(HINSTANCE hInst, WCHAR *pPrinterName, long lMode = config_device);
long WINAPI ScConvertPrintTicketToDevMode(char FAR *, DWORD, DWORD, LPSCDM);
long WINAPI ScConvertDevModeToPrintTicket(char FAR *, DWORD, LPDWORD, LPSCDM);
long ipifConvDevModetoPrintTicket(char FAR *, DWORD, LPDWORD, LPSCDM);
long ConvDevModetoPrintTicket(char FAR *, DWORD, LPDWORD, LPSCDM);
long ipifConvPrintTickettoDevMode(char FAR *, DWORD, DWORD, LPSCDM);
long ConvPrintTickettoDevMode(char FAR *, DWORD, DWORD, LPSCDM);
BOOL ConvPrintTickettoDevModeCopies(DWORD, LPSCDM);
BOOL ConvPrintTickettoDevModeStaple(DWORD, LPSCDM);
BOOL ConvPrintTickettoDevModeOrientation(DWORD , LPSCDM );
BOOL ConvPrintTickettoDevModeCollate(DWORD, LPSCDM);
BOOL ConvPrintTickettoDevModePunch(DWORD, LPSCDM);
BOOL ConvPrintTickettoDevModePaperTray(DWORD, LPSCDM);
BOOL ConvPrintTickettoDevModeNup(DWORD, LPSCDM);
void WINAPI WriteDEVDModeValueToReg(HINSTANCE, WCHAR*, LPSCDM);
//OEMDEV WINAPI GetUniDrvPrivateDevmodePTPC(LPDEVMODE); //<S><D> Since this function is not called anywhere, deleted this function to fix coverity issue 91562, Harika 20211027
BOOL ConvPrintTickettoDevModeDuplex(DWORD, LPSCDM);
DWORD ConvDevModetoPrintTicketNup(LPSCDM);
DWORD ConvDevModetoPrintTicketOrientation(LPSCDM);
DWORD ConvDevModetoPrintTicketStaple(LPSCDM);
DWORD ConvDevModetoPrintTicketPaperTray(LPSCDM);
DWORD ConvDevModetoPrintTicketPunch(LPSCDM);
DWORD ConvDevModetoPrintTicketCollate(LPSCDM);
DWORD ConvDevModetoPrintTicketPaperSize(LPSCDM);
SHORT WINAPI SearchSelectMFP(WCHAR* pPrinterName, LONG uCurrentMFPIndex,BOOL bMfpIndex);
DWORD WINAPI scGetLangID(WCHAR* pszSvrPrnName, DWORD* dwLangID);
PFEATUREINFOLIST FeatureInfoList;
//BOOL OrgMergeSCDM(LPSCDM pscdm_s, LPSCDM pscdm_d, HANDLE hPrinter, char FAR *pszPrnName, char FAR *pszCall);
//#include "debug.h"
// This indicates to Prefast that this is a usermode driver file.
_Analysis_mode_(_Analysis_code_type_user_driver_);
////////////////////////////////////////////////////////
//      INTERNAL MACROS and DEFINES
////////////////////////////////////////////////////////
//#define DISCOVERYMODULENAME	L"DISCUI.DLL"
typedef BOOL (__cdecl *pfnShowDiscoveryUI)(WCHAR* pszSvrPrnName,HMODULE hStringDllHandle,BOOL bIsDevicePropertySheetEvent,
	long& uCurrentMFPIndex, DLGMFPSLCT_INFO *mfpSlctInfo, PSCDM pscdm_drv);
LPTSTR	g_DriverFileName[]	=
{
	{ TEXT("discui")	},
};
//--------------------------------------------------------------------------
//      Globals
//--------------------------------------------------------------------------

HINSTANCE   ghInstance = NULL;
// Module's Instance handle from DLLEntry of process.
HANDLE      ghActCtx    = INVALID_HANDLE_VALUE; 

long g_CurrentIndex = 0;
//--------------------------------------------------------------------------
//      Internal Globals
//--------------------------------------------------------------------------

static long g_cComponents = 0;
    // Count of active components

static long g_cServerLocks = 0;
    // Count of locks
//<S><A>To Fix the Redmine Bug 4702-issue 2,3,4,5,23.03.2022,SSDI:chanchal Singla
//Added New Static member g_cPTPCStatus , its motive to add-in code is that when we
//open any metro Application,that time that variable is updating to True value,
static long g_cPTPCStatus = 0;
//<E>To Fix the Redmine Bug 4702-issue 2,3,4,5,23.03.2022,SSDI:chanchal Singla
//--------------------------------------------------------------------------
//      Class factory for the Plug-In object
//--------------------------------------------------------------------------
class COemCF : public IClassFactory
{
public:

    // IUnknown methods
    STDMETHOD(QueryInterface) (THIS_
        REFIID riid,
        _COM_Outptr_ LPVOID FAR* ppvObj
        );

    STDMETHOD_(ULONG,AddRef) (THIS);
    STDMETHOD_(ULONG,Release) (THIS);

    // IClassFactory methods
    STDMETHOD(CreateInstance) (THIS_
        _In_opt_     LPUNKNOWN pUnkOuter,
        _In_         REFIID riid,
        _COM_Outptr_ LPVOID FAR* ppvObject
        );

    STDMETHOD(LockServer) (THIS_
        BOOL bLock
        );

    // Class-specific methods.
    COemCF(): m_cRef(1) { };

    ~COemCF() { };

protected:
    LONG m_cRef;

};


//+---------------------------------------------------------------------------
//
//  Member:
//      ::DllCanUnloadNow
//
//  Synopsis:
//      Can the DLL be unloaded from memory?  Answers no if any objects are
//      still allocated, or if the server has been locked.
//
//      To avoid leaving OEM DLL still in memory when Unidrv or Pscript
//      drivers are unloaded, Unidrv and Pscript driver ignore the return
//      value of DllCanUnloadNow of the OEM DLL, and always call FreeLibrary
//      on the OEMDLL.
//
//      If the plug-in spins off a working thread that also uses this DLL,
//      the thread needs to call LoadLibrary and FreeLibraryAndExitThread,
//      otherwise it may crash after Unidrv or Pscript calls FreeLibrary.
//
//  Returns:
//      S_OK if the DLL can be unloaded safely, otherwise S_FALSE
//
//
//----------------------------------------------------------------------------
STDAPI DllCanUnloadNow()
{
    if ((g_cComponents == 0) && (g_cServerLocks == 0))
    {
        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}


//+---------------------------------------------------------------------------
//
//  Member:
//      ::DllGetClassObject
//
//  Synopsis:
//      Retrieve the class factory object for the indicated class.
//
//  Returns:
//      CLASS_E_CLASSNOTAVAILABLE, E_OUTOFMEMORY, or S_OK.
//
//  Notes:
//
//
//----------------------------------------------------------------------------
STDAPI
DllGetClassObject(
    _In_ CONST CLSID& clsid,
        // GUID of the class object that the caller wants a class factory
        // for
    _In_ CONST IID& iid,
        // Interface ID to provide in ppv
    _Outptr_ VOID** ppv
        // out pointer to the interface requested.
    )
{
    VERBOSE(DLLTEXT("DllGetClassObject:Create class factory."));

    if (ppv == NULL)
    {
        return E_POINTER;
    }
    *ppv = NULL;

    //
    // Can we create this component?
    //
    if (clsid != CLSID_OEMUI)
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    //
    // Create class factory.
    // Reference count set to 1 in Constructor
    //
    COemCF* pUIReplacementCF = new COemCF;
    if (pUIReplacementCF == NULL)
    {
        return E_OUTOFMEMORY;
    }

    //
    // Get requested interface.
    //
    HRESULT hr = pUIReplacementCF->QueryInterface(iid, ppv);
    pUIReplacementCF->Release();

    return hr;
}


//+---------------------------------------------------------------------------
//
//  Member:
//      COemCF::QueryInterface
//
//  Synopsis:
//      Standard COM IUnknown implementation.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemCF::QueryInterface(
    CONST IID& iid,
    _COM_Outptr_ VOID** ppv
    )
{
    VERBOSE(DLLTEXT("COemCF::QueryInterface entry."));

    if ((iid == IID_IUnknown) || (iid == IID_IClassFactory))
    {
        *ppv = static_cast<COemCF*>(this);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemCF::AddRef
//
//  Synopsis:
//      Standard COM IUnknown implementation.
//
//
//----------------------------------------------------------------------------
ULONG __stdcall
COemCF::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemCF::Release
//
//  Synopsis:
//      Standard COM IUnknown implementation.
//
//
//----------------------------------------------------------------------------
ULONG __stdcall
COemCF::Release()
{
    assert(0 != m_cRef);

   ULONG cRef = InterlockedDecrement(&m_cRef);
   if (0 == cRef)
   {
      delete this;
   }
   return cRef;
}


//+---------------------------------------------------------------------------
//
//  Member:
//      COemCF::CreateInstance
//
//  Synopsis:
//      Attempt to create an object that implements the specified interface.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemCF::CreateInstance(
    _In_opt_    IUnknown* pUnknownOuter,
    _In_        CONST IID& iid,
    _COM_Outptr_ VOID** ppv
    )
{
    VERBOSE(DLLTEXT("COemCF::CreateInstance entry."));

    if (ppv == NULL)
    {
        return E_POINTER;
    }
    *ppv = NULL;

    //
    // Cannot aggregate.
    //
    if (pUnknownOuter != NULL)
    {
        return CLASS_E_NOAGGREGATION;
    }

    HRESULT hr = S_OK;

    if (iid == IID_IUnknown ||
        iid == IID_IPrintOemUI2)
    {
        VERBOSE(DLLTEXT("COemCF::IID_IUnknown\r\n"));

        //
        // Create component.
        //
        COemUI2* pOemCB = new COemUI2;
        if (pOemCB == NULL)
        {
            hr = E_OUTOFMEMORY;
        }
        else
        {
            //
            // Get the requested interface.
            //
            hr = pOemCB->QueryInterface(iid, ppv);

            //
            // Release the IUnknown pointer.
            // (If QueryInterface failed, component will delete itself.)
            //
            pOemCB->Release();
        }
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}


//+---------------------------------------------------------------------------
//
//  Member:
//      COemCF::LockServer
//
//  Synopsis:
//      Locking the server holds the objects server in memory, so that new
//      instances of objects can be created more quickly.
//
//  Returns:
//      S_OK
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemCF::LockServer(
    BOOL bLock
        // If true, increment the lock count, otherwise decrement the lock
        // count.
    )
{
    if (bLock)
    {
        InterlockedIncrement(&g_cServerLocks);
    }
    else
    {
        InterlockedDecrement(&g_cServerLocks);
    }

    return S_OK;
}


//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2 (destructor)
//
//  Synopsis:
//      Release the helper interface, as well as any other resources that
//      the plug-in is holding onto.
//
//  Returns:
//
//  Notes:
//
//
//----------------------------------------------------------------------------
COemUI2::~COemUI2()
{
    //
    // Make sure that helper interface is released.
    //
    if(NULL != m_pCoreHelperUni)
    {
        m_pCoreHelperUni->Release();
        m_pCoreHelperUni = NULL;
    }
	DestroyPropPages();
	DestroyDevicePropPages();
    //<S><A> Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    //Putting NULL check to ensure that the Handles are not available before release so that there is no crash.
    if(NULL != m_hStringResourceHandle)
    {
        FreeLibrary(m_hStringResourceHandle);
        m_hStringResourceHandle = NULL ;
    }
    //<E> Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    //<S><A> Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    if(NULL != m_hBitmapResourceHandle)
    {
        FreeLibrary(m_hBitmapResourceHandle);
        m_hBitmapResourceHandle = NULL ;
    }
    //<E> Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    //<S><A> Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    if(NULL != m_hDialogResourceHandle)
    {
        FreeLibrary(m_hDialogResourceHandle);
        m_hDialogResourceHandle = NULL ;
    }
    //<E> Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
	
	if(m_pHelpID)
	{
		delete [] m_pHelpID;
		m_pHelpID = NULL;
	}
    
    if(NULL != m_pPrivateDevmode)
    {
        delete m_pPrivateDevmode;
        m_pPrivateDevmode = NULL;
    }

	if (m_ppi.pps != NULL)
	{
		delete m_ppi.pps;
		m_ppi.pps = NULL;
	}
	FreeMemory();

    //
    // If this instance of the object is being deleted, then the reference
    // count should be zero.
    //
    assert(0 == m_cRef);
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::QueryInterface
//
//  Synopsis:
//      Standard COM IUnknown implementation.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::QueryInterface(
    CONST IID& iid,
    _COM_Outptr_ VOID** ppv
    )
{
    VERBOSE(DLLTEXT("COemUI2::QueryInterface entry."));

    if (iid == IID_IUnknown)
    {
        *ppv = static_cast<IUnknown*>(this);
        VERBOSE(DLLTEXT("COemUI2::QueryInterface - Return pointer to IUnknown.\r\n"));
    }
    else if (iid == IID_IPrintOemUI2)
    {
        *ppv = static_cast<IPrintOemUI2*>(this);
        VERBOSE(DLLTEXT("COemUI2::QueryInterface - Return pointer to IPrintOemUI.\r\n"));
    }
    else
    {
#if DBG
        TCHAR szOutput[80] = {0};
        StringFromGUID2(iid, szOutput, _countof(szOutput)); // can not fail!
        VERBOSE(DLLTEXT("COemUI2::QueryInterface %s not supported.\r\n"), szOutput);
#endif

        *ppv = NULL;
        return E_NOINTERFACE;
    }
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::AddRef
//
//  Synopsis:
//      Standard COM IUnknown implementation.
//
//
//----------------------------------------------------------------------------
ULONG __stdcall
COemUI2::AddRef()
{
    VERBOSE(DLLTEXT("COemUI2::AddRef entry."));

    return InterlockedIncrement(&m_cRef);
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::Release
//
//  Synopsis:
//      Standard COM IUnknown implementation.
//
//
//----------------------------------------------------------------------------
ULONG __stdcall
COemUI2::Release()
{
   VERBOSE(DLLTEXT("COemUI2::Release entry."));

   assert(0 != m_cRef);

   ULONG cRef = InterlockedDecrement(&m_cRef);

   if (0 == cRef)
   {
      delete this;
      return 0;

   }

   return cRef;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::PublishDriverInterface
//
//  Synopsis:
//      This routine is called by the core driver to supply objects to the
//      plug-in which the plug-in can use to implement various features.
//
//      This implementation requires an object that IPrintCoreHelper interface
//      from the core driver to implement full UI replacement.  If the object
//      passed in is the one desired, this routine will return S_OK, otherwise
//      it will return E_FAIL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::PublishDriverInterface(
    IUnknown *pIUnknown
    )
{
    VERBOSE(DLLTEXT("COemUI2::PublishDriverInterface entry."));

    HRESULT hrResult = S_OK;

    //
    // Determine whether the published object is the helper interface
    // introduced in Vista to support full UI replacement.
    //
    if (m_pCoreHelperUni == NULL)
    {
        hrResult = pIUnknown->QueryInterface(IID_IPrintCoreHelperUni, (VOID**) &(m_pCoreHelperUni));

        if (!SUCCEEDED(hrResult))
        {
            m_pCoreHelperUni = NULL;
            hrResult = E_FAIL;
        }
		
    }

    return hrResult;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::GetInfo
//
//  Synopsis:
//      Most of this routine is a standard implementation common to most
//      plug-ins.  Of particular note in this sample is that to get a handle
//      the IPrintCoreHelper plug-in in the PublishDriverInterface callback,
//      this routine must return OEMPUBLISH_IPRINTCOREHELPER in the out
//      parameter when the mode is OEMGI_GETREQUESTEDHELPERINTERFACES.
//
//  Returns:
//      S_OK for supported modes, E_FAIL for everything else.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::GetInfo(
    _In_ DWORD  dwMode,
        // The mode indicates what information is being requested by the
        // core driver.
    _Out_writes_bytes_to_(cbSize, *pcbNeeded) PVOID  pBuffer,
        // The output buffer is the location into which the requested info
        // should be written
    _In_ DWORD  cbSize,
        // The size of the output buffer.
    _Out_ PDWORD pcbNeeded
        // If the size of the buffer is insufficient, use this parameter
        // to indicate to the core driver how much space is required.
    )
{
    HRESULT hrResult = S_OK;

#if DBG
    PWSTR pszTag;

    switch(dwMode)
    {
        case OEMGI_GETSIGNATURE: pszTag = L"COemUI2::GetInfo entry. [OEMGI_GETSIGNATURE]"; break;
        case OEMGI_GETVERSION: pszTag = L"COemUI2::GetInfo entry. [OEMGI_GETVERSION]"; break;
        case OEMGI_GETREQUESTEDHELPERINTERFACES: pszTag = L"COemUI2::GetInfo entry. [OEMGI_GETREQUESTEDHELPERINTERFACES]"; break;
        default: pszTag = L"COemUI2::GetInfo entry."; break;
    }

    VERBOSE(pszTag);
#endif

    //
    // Validate parameters.  OEMGI_GETPUBLISHERINFO is excluded from this
    // list because that GetInfo mode applies only to PScript rendering plug-ins.
    //
    if ((NULL == pcbNeeded) ||
        ((OEMGI_GETSIGNATURE != dwMode) &&
         (OEMGI_GETVERSION != dwMode) &&
         (OEMGI_GETREQUESTEDHELPERINTERFACES != dwMode)))
    {
        WARNING(DLLTEXT("COemUI2::GetInfo() exit pcbNeeded is NULL or mode is not supported\r\n"));
        SetLastError(ERROR_INVALID_PARAMETER);
        return E_FAIL;
    }

    //
    // Set expected buffer size and number of bytes written.
    //
    *pcbNeeded = sizeof(DWORD);

    //
    // Check buffer size is sufficient.
    //
    if((cbSize < *pcbNeeded) || (NULL == pBuffer))
    {
        WARNING(DLLTEXT("COemUI2::GetInfo() exit insufficient buffer!\r\n"));
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        hrResult = E_FAIL;
    }
    else
    {
        switch(dwMode)
        {
            //
            // OEM DLL Signature
            //
            case OEMGI_GETSIGNATURE:
                *(PDWORD)pBuffer = OEM_SIGNATURE;
                break;

            //
            // OEM DLL version
            //
            case OEMGI_GETVERSION:
                *(PDWORD)pBuffer = OEM_VERSION;
                break;

            //
            // New core helper interface support
            //
            case OEMGI_GETREQUESTEDHELPERINTERFACES:
                *(PDWORD)pBuffer = 0;
                *(PDWORD)pBuffer |= OEMPUBLISH_IPRINTCOREHELPER;
                break;

            //
            // dwMode not supported.
            //
				//<S><D> To fix Coverity issue 90331, Harika	20201211
            ////default:
            ////    // Set written bytes to zero since nothing was written.
            ////    //
            ////    WARNING(DLLTEXT("COemUI2::GetInfo() exit mode not supported.\r\n"));
            ////    *pcbNeeded = 0;
            ////    SetLastError(ERROR_NOT_SUPPORTED);
            ////    hrResult = E_FAIL;
				//<E> To fix Coverity issue 90331, Harika	20201211
        }
    }

    return hrResult;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::DevMode
//
//  Synopsis:
//      This routine operates in various modes to manage the private DEVMODE.
//      This sample does not provide much by way of illustration of how to
//      handle OEM settings in the private DEVMODE.  See the sample OEMUI for
//      a more in-depth look at this routine.  The function hrOEMDevMode in
//      devmode.cpp also provides more information on how to handle the various
//      modes.
//
//  Returns:
//      S_OK on success, else E_*
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::DevMode(
    DWORD  dwMode,
        // The operation that should be performed
    POEMDMPARAM pOemDMParam
        // The input & output DEVMODEs, including pointers to the public &
        // private DEVMODE data.
    )
{
    VERBOSE(DLLTEXT("COemUI2::Devmode entry."));

    return hrOEMDevMode(dwMode, pOemDMParam);
}


//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::HideStandardUI
//
//  Synopsis:
//      This routine is critical to supporting full UI replacement.  This
//      routine allows the plug-in to disable Unidrv's (or PScript's) standard
//      UI panels.  If this returns E_NOTIMPL, the panel isn't hidden.  If
//      this routine returns S_OK, Unidrv will not display any UI for the
//      specified mode.  If this returns E_NOTIMPL, Unidrv will display it's
//      standard UI.
//
//  Returns:
//      S_OK to hide the indicated UI if desired, otherwise E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::HideStandardUI(
    DWORD       dwMode
        // document property sheets, printer property sheets, or possibly
        // something not yet defined.
    )
{
    VERBOSE(DLLTEXT("COemUI2::HideStandardUI entry."));

    HRESULT hrResult = E_NOTIMPL;

    switch(dwMode)
    {
        case OEMCUIP_DOCPROP:
        case OEMCUIP_PRNPROP:
            hrResult = S_OK;
            break;
        default:
            // If we don't recognize the mode, assume that we don't want to hide
            // the UI provided by Unidrv.
            hrResult = E_NOTIMPL;
    }

    return hrResult;
}



//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::DocumentPropertySheets
//
//  Synopsis:
//      See function hrOEMDocumentPropertySheets in uniuirep.cpp.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::DocumentPropertySheets(
    PPROPSHEETUI_INFO   pPSUIInfo,
    LPARAM              lParam
    )
{
    //  LONG_PTR lResult = TRUE;
    // result supplied to COMPSTUI via pPSUIInfo

    HRESULT hrResult = S_OK;
    // Result returned to the immediate caller.
    BOOL bResult = FALSE;
	//POEMUIPSPARAM pOemUIPsParam = (POEMUIPSPARAM)pPSUIInfo->lParamInit;
	/*StringCbCopy(m_pPrinterName,MAX_PATH*sizeof(WCHAR),pOemUIPsParam->pPrinterName);
    m_hPrinter = pOemUIPsParam->hPrinter;
    LoadStringResource(m_pPrinterName);*/
    VERBOSE(DLLTEXT("hrCommonPropSheetMethod entry."));

    //
    // Validate parameters.
    //
    if ((NULL == pPSUIInfo) ||
        (PROPSHEETUI_INFO_VERSION != pPSUIInfo->Version))
    {
        ERR(ERRORTEXT("hrCommonPropSheetMethod() ERROR_INVALID_PARAMETER.\r\n"));

        //
        // Return invalid parameter error.
        // 
        SetLastError(ERROR_INVALID_PARAMETER);
        hrResult = E_FAIL;
        return hrResult;
    }

	//Coverity Fix 90381 - SSDI:Seetharam - 20200908
	//Moved this statement from above so that the use of pPSUIInfois done
	//after the parameter NULL check
	POEMUIPSPARAM pOemUIPsParam = (POEMUIPSPARAM)pPSUIInfo->lParamInit;

    eDeviceType wPrevDevicetype = eColor;
    CRegistryAPI retgApi(ghInstance);
	CPrintCoreHelper cprintcorehelper(m_pCoreHelperUni);
	FeatureInfoList = cprintcorehelper.RetrieveFeaturesAndOptions();
    Dump(pPSUIInfo);
	if (PROPSHEETUI_REASON_INIT == pPSUIInfo->Reason)
	{
		SetLoggerRegInfo();
		StringCbCopy(m_pPrinterName, MAX_PATH * sizeof(WCHAR), pOemUIPsParam->pPrinterName);
		m_hPrinter = pOemUIPsParam->hPrinter;
		//<S><C>To Fix Redmine Bug #4650 ,Bug #4652,02.01.2022,SSDI:Chanchal Singla
		//Bug #4650:-Strings in Printing Preferences is not displayed as per the Language set
		// Bug #4652:- Localized strings are not displayed in Metro app
		TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
		GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
		CShIniFile *pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
		//short wSetupType = eBasicMode;
		//CShIniFile *pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
		//wSetupType = pmcf->GetInstallationType();
		//CShRegMdlTbl RegMdlTbl(ghInstance, m_pPrinterName, pmcf);
		//RegMdlTbl.SetInstallationType(wSetupType);

		// <S><A> SSDI Big #4011 Fix: SSDI Seetharam - 20210702
		DWORD wSetupType = eBasicMode;
		// <S><C> SSDI Big #4188 Fix: SSDI Seetharam - 20210818
		{
			if (retgApi.GetInstallationType(m_pPrinterName, wSetupType) == ERROR_FILE_NOT_FOUND)
			{
				wSetupType = pmcf->GetInstallationType();
				retgApi.SetInstallationType(m_pPrinterName, (short)wSetupType);
			}
			//if (wSetupType != eBasicMode)
			{
				SHORT dwLangID = -1;
				CShRegMdlTbl RegMdlTbl(ghInstance, m_pPrinterName, pmcf);
				RegMdlTbl.GetLangID(&dwLangID,true);
				retgApi.SetLangID(m_pPrinterName, (DWORD)dwLangID);
			}
		}
		//<S><A> Coverity Fix 91533 - SSDI:Seetharam - 20210907
		if (pmcf != NULL)
		{
			delete pmcf;
			pmcf = NULL;
		}
		//<S><E> Coverity Fix 91533 - SSDI:Seetharam - 20210907

		LoadStringResource(m_pPrinterName);
		LoadBitmapResource(m_pPrinterName);
		LoadDialogResource(m_pPrinterName);
		// <S><E> SSDI Big #4188 Fix: SSDI Seetharam - 20210818
		//<E>To Fix Redmine Bug #4650 ,Bug #4652,02.01.2022,SSDI:Chanchal Singla
		//<S><A>To Fix Redmine #Bug #4186,01.04.2022,SSDI:Chanchal Singla
		//Configuration Tab : Silent auto configuration fails when driver is installed through installer.
		CRegistryAPI cRegApi(ghInstance);
		cRegApi.ExecuteSilentAutoConfig(ghInstance, pOemUIPsParam->pPrinterName);
		//Configuration Tab : Silent auto configuration fails when driver is installed through installer.
		//<E>To Fix Redmine #Bug #4186,01.04.2022,SSDI:Chanchal Singla
		if (wSetupType != eBasicMode)
		// <S><A> SSDI Big #4011 Fix: SSDI Seetharam - 20210702
		{
			// 2013.09.06, SSDI:Gajanana D S
			// Do not show Discovery dialog when clicking Printing Prefernces from application.
			// When IPrintOemUI::DocumentPropertySheets is called, the lParamInit member of the 
			// PROPSHEETUI_INFO structure contains the address of an OEMUIPSPARAM structure.
			// OEMUIPSPARAM structure is an analog of 
			// DOCUMENTPROPERTYHEADER structure of DrvDocumentPropertySheets function
			// dwFlagMember of OEMUIPSPARAM structure contains
			//contents of the fMode member of the DOCUMENTPROPERTYHEADER structure received by the printer driver's DrvDocumentPropertySheets function.
			// Refer to below msdn link for more information
			// http://msdn.microsoft.com/en-us/library/windows/hardware/ff559576(v=vs.85).aspx
			if ((pOemUIPsParam->dwFlags & DM_USER_DEFAULT) && !(pOemUIPsParam->dwFlags & DM_NOPERMISSION))
			{
				try
				{
					HMODULE	hDiscoveryModule = NULL;
					WCHAR pDiscoveryDLLName[_MAX_PATH] = { 0 };
					GetProjectFileName(pDiscoveryDLLName, DISCOVERYMODULENAME);
					hDiscoveryModule = ::LoadLibrary(pDiscoveryDLLName);
					if (NULL != hDiscoveryModule)
					{
						pfnShowDiscoveryUI pDF_CreateMyDlg = NULL;
						pDF_CreateMyDlg = (pfnShowDiscoveryUI)::GetProcAddress(hDiscoveryModule, "ShowDiscoveryUI");
						DWORD CheckState = 0;/*
											 short wPrevDevicetype = eColor;
											 CRegistryAPI retgApi;*/
						retgApi.GetDeviceType(m_pPrinterName, wPrevDevicetype);
						long uCurrentMFPIndex = 0;
						retgApi.GetDefaultrMFP(m_pPrinterName, uCurrentMFPIndex);
						LRESULT lRet = retgApi.GetShowDailogRegInfo(m_pPrinterName, CheckState);
						BOOL bIsDevicePropertySheetEvent = FALSE;
						//<S><C>To Fix Redmine #3151 Issue 3,26.august.2021,SSDI:Chanchal Singla
						//Issue 3:-Silent Autoconfiguration is not executing from General tab
						POEMDEV pPrivateDemode = NULL;
						if ((ERROR_SUCCESS != lRet) || ((ERROR_SUCCESS == lRet) && (1 == CheckState)))
						{
							pPrivateDemode = (POEMDEV)pOemUIPsParam->pOEMDM;

							retgApi.ReadData(m_pPrinterName, &pPrivateDemode->scpi.dev);
							bResult = (pDF_CreateMyDlg)(pOemUIPsParam->pPrinterName, m_hStringResourceHandle, bIsDevicePropertySheetEvent,
								uCurrentMFPIndex, NULL, &pPrivateDemode->scpi);
							if (bResult != 0)
							{
								long Count = 0;
								long CurrentINdex = 0;
								long previousMFPIndex = 0;

								previousMFPIndex = uCurrentMFPIndex;

								retgApi.GetRegisterMFPCountAndCheckPos(m_pPrinterName, Count, CurrentINdex);
								g_CurrentIndex = CurrentINdex;

								pPrivateDemode->scpi.dev.nTargetDeviceID = (short)g_CurrentIndex;
								ExecuteAutoConfiguration(m_pPrinterName,m_hPrinter, m_hStringResourceHandle, m_hBitmapResourceHandle,
									m_hDialogResourceHandle,pPrivateDemode, g_CurrentIndex, previousMFPIndex);
							}
						}
						//<E>To Fix Redmine #3151 Issue 3,26.august.2021,SSDI:Chanchal Singla
						FreeLibrary(hDiscoveryModule);
						hDiscoveryModule = NULL;
					}
				}
				catch (CXDException &e)
				{
					return e;
				}
			}
	}
    }
    switch(pPSUIInfo->Reason)
    {
    case PROPSHEETUI_REASON_INIT:
        {

            try
            {
                if (SUCCEEDED(hrResult= StoreHelpFilePathAndID(pOemUIPsParam->pPrinterName)))
                {
                    if (SUCCEEDED(hrResult = CreatePropertyPages((POEMDEV)pOemUIPsParam->pOEMDM,pOemUIPsParam->pPublicDM)))
                    {
						//<S><M> create the tab frame instead of the property pages, 20200327 - SSDI:Seetharam
						m_TabDlgFrame->PropPageInit(pPSUIInfo);
                        /*DocumentPropertyPageMap::const_iterator iterPropSheets = m_DocumentPropertyPages.begin();
                        while (iterPropSheets != m_DocumentPropertyPages.end() &&
                            (NULL != *iterPropSheets)&&
                            SUCCEEDED(hrResult = (*iterPropSheets)->PropPageInit(pPSUIInfo)))
                        {
                            iterPropSheets++;
                        }*/
						//<S><E> create the tab frame instead of the property pages, 20200327 - SSDI:Seetharam
                    }
                }
            }
            catch (CXDException &e)
            {
                return e;
            }
        }
        break;

    case PROPSHEETUI_REASON_GET_INFO_HEADER:
        {
            //
            // Set header title
            // 
            //PPROPSHEETUI_INFO_HEADER pHeader = (PPROPSHEETUI_INFO_HEADER)m_lParam;
            //pHeader->pTitle = (LPTSTR)PROP_TITLE;
            if(NULL != pPSUIInfo)
            {
                pPSUIInfo->Result = TRUE;
            }
        }
        break;

    case PROPSHEETUI_REASON_SET_RESULT:
        {
            eDeviceType dwCurrentDeviceType = eColor;
            retgApi.GetDeviceType(m_pPrinterName,dwCurrentDeviceType);
            ((POEMDEV)(pOemUIPsParam->pOEMDM))->scpi.wFieldSPI = (eColor == dwCurrentDeviceType) ? COLOR_MODEL_ID : MONO_MODEL_ID;
            if(NULL != pPSUIInfo)
            {
                pPSUIInfo->Result = reinterpret_cast<PSETRESULT_INFO>(lParam)->Result;
            }
        }
        break;

    case PROPSHEETUI_REASON_DESTROY:
        {
            if(NULL != pPSUIInfo)
            {
                pPSUIInfo->Result = TRUE;
            }
        }
        break;
    default:
        {
            hrResult = E_FAIL;
        }
        break;
    }
    return hrResult;
}

    //+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::DevicePropertySheets
//
//  Synopsis:
//      See function hrOEMDevicePropertySheets in uniuirep.cpp
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::DevicePropertySheets(
    PPROPSHEETUI_INFO   pPSUIInfo,
    LPARAM              lParam
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hrResult = S_OK;
    VERBOSE(DLLTEXT("COemUI2::DevicePropertySheets entry."));

	if(pPSUIInfo==NULL)
	{
        return E_FAIL;
	}

	POEMUIPSPARAM pOemUIPsParam = (POEMUIPSPARAM)pPSUIInfo->lParamInit; 
    /*StringCbCopy(m_pPrinterName,MAX_PATH*sizeof(WCHAR),pOemUIPsParam->pPrinterName);
    LoadStringResource(m_pPrinterName);*/
	if ((NULL == pPSUIInfo) || (PROPSHEETUI_INFO_VERSION != pPSUIInfo->Version))
    {
        ERR(ERRORTEXT("hrCommonPropSheetMethod() ERROR_INVALID_PARAMETER.\r\n"));

        //
        // Return invalid parameter error.
        // 
        SetLastError(ERROR_INVALID_PARAMETER);
        hrResult = E_FAIL;
        return hrResult;
    }

	switch(pPSUIInfo->Reason)
	{
	case PROPSHEETUI_REASON_INIT:
		{
            try
            {
                //LoadStringResource();
                //LoadDiscoveryStringResource();
                StringCbCopy(m_pPrinterName,MAX_PATH*sizeof(WCHAR),pOemUIPsParam->pPrinterName);
				g_cPTPCStatus = 0;
                LoadStringResource(m_pPrinterName);
                LoadBitmapResource(m_pPrinterName);
                LoadDialogResource(m_pPrinterName);
				TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
				GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
				CShIniFile *pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
				if (NULL != pmcf)
				{
					CShRegMdlTbl RegMdlTbl(ghInstance, m_pPrinterName, pmcf);
					DWORD dwSetupType = eBasicMode;
					RegMdlTbl.GetInstallationType(dwSetupType);
					RegMdlTbl.SetInstallationType((short)dwSetupType);
					//<S><A> Coverity Fix 91533 - SSDI:Seetharam - 20210907
					delete pmcf;
					//<S><E> Coverity Fix 91533 - SSDI:Seetharam - 20210907
					//CRegistryAPI regApi;
					//regApi.SetInstallationType(m_pPrinterName, dwSetupType);
				}
				//<S><A>To Fix Redmine #Bug #4186,01.04.2022,SSDI:Chanchal Singla
				//Configuration Tab : Silent auto configuration fails when driver is installed through installer.
				CRegistryAPI cRegApi(ghInstance);
				cRegApi.ExecuteSilentAutoConfig(ghInstance, pOemUIPsParam->pPrinterName);
				//Configuration Tab : Silent auto configuration fails when driver is installed through installer.
				//<E>To Fix Redmine #Bug #4186,01.04.2022,SSDI:Chanchal Singla
                if (SUCCEEDED(hrResult= StoreHelpFilePathAndID(pOemUIPsParam->pPrinterName)))
                {
					if (SUCCEEDED(hrResult = CreateDevicePropertyPages(pOemUIPsParam->hPrinter)))
					{
						DevicePropertyPageMap::const_iterator iterPropSheets = m_DevicePropertyPages.begin();
						while (iterPropSheets != m_DevicePropertyPages.end() &&
							(NULL != *iterPropSheets)&&
							SUCCEEDED(hrResult = (*iterPropSheets)->PropPageInit(pPSUIInfo)))
						{
							iterPropSheets++;
						}
					}
				}
			}
			catch (CXDException &e)
			{
				return e;
			}
		}
	break;
	case PROPSHEETUI_REASON_GET_INFO_HEADER:
		{
			if(NULL != pPSUIInfo)
			{
				pPSUIInfo->Result = TRUE;
			}
		}
		break;
	case PROPSHEETUI_REASON_SET_RESULT:
	{
			//Need to add the silent auto-configuration code here
			DevicePropertyPageMap::const_iterator iterPropSheets = m_DevicePropertyPages.begin();
			while (iterPropSheets != m_DevicePropertyPages.end() && (NULL != *iterPropSheets))
			{
				CDevicePropertyPage *pDevicePropPage = ((CDevicePropertyPage *)*iterPropSheets);
				if (pDevicePropPage != NULL && pDevicePropPage->m_bExecuteSilentAutoConfig)
					pDevicePropPage->ExecuteAutoConfiguration(TRUE);
				iterPropSheets++;
			}

			if(NULL != pPSUIInfo)
			{
				pPSUIInfo->Result = reinterpret_cast<PSETRESULT_INFO>(lParam)->Result;
			}
		}
		break;

	case PROPSHEETUI_REASON_DESTROY:
		{
			if(NULL != pPSUIInfo)
			{
				pPSUIInfo->Result = TRUE;
			}
		}
		break;
	default:
        {
             hrResult = E_FAIL;
        }
        break;
	}
	return hrResult;
}


//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::CommonUIProp
//
//  Synopsis:
//      This routine is used to interact with the Unidrv-supplied
//      property sheet pages.  Since this plug-in implements full-UI
//      replacement, those property sheet pages are disabled, and this
//      routine does not need to do anything.
//
//  Returns:
//      S_OK
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::CommonUIProp(
    DWORD  dwMode,
    POEMCUIPPARAM   pOemCUIPParam
    )
{
    VERBOSE(DLLTEXT("COemUI2::CommonUIProp entry."));

    UNREFERENCED_PARAMETER(dwMode);
    UNREFERENCED_PARAMETER(pOemCUIPParam);

    return S_OK;
}


//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::DeviceCapabilities
//
//  Synopsis:
//      This routine can be used to replace the Unidrvs device capabilities
//      handling.  In this implementation, no custom capabilities handling is
//      provided.
//
//  Returns:
//      E_FAIL
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::DeviceCapabilities(
    _Inout_ POEMUIOBJ poemuiobj,
        // OEM settings & information
    _In_  HANDLE      hPrinter,
        // Handle to the printer that the capabilities are being requested for
    _In_  PWSTR       pDeviceName,
        // Name of the device / driver
    _In_  WORD        wCapability,
        // The DC_ ID indicating which capability was requested.
    _Out_writes_(_Inexpressible_("varies with wCapability")) PVOID       pOutput,
        // Buffer to write requested information to.
    _In_  PDEVMODE    pPublicDM,
        // Pointer to the public DEVMODE representing the settings to get
        // capabilities with respect to.
    _In_  PVOID       pOEMDM,
        // OEM private DEVMODE settings.
    _In_  DWORD       dwOld,
        // Result from previous plug-in call to this routine.
    _Out_ DWORD       *dwResult
        // Result of this call.  If there are multiple UI plug-ins, this
        // result is passed to the next one as dwOld
    )
{
    VERBOSE(DLLTEXT("COemUI2::DeviceCapabilities entry."));

    UNREFERENCED_PARAMETER(poemuiobj);
    UNREFERENCED_PARAMETER(hPrinter);
    //UNREFERENCED_PARAMETER(pDeviceName);
    UNREFERENCED_PARAMETER(pOutput);
   // UNREFERENCED_PARAMETER(pPublicDM);
    UNREFERENCED_PARAMETER(pOEMDM);
    UNREFERENCED_PARAMETER(dwOld);
  //  UNREFERENCED_PARAMETER(dwResult);
    //
    // Do nothing.  Let Unidrv handle the device capabilities processing.
    //
//<S><C>To Fix general tab issue,20210113 SSDI:Goutham 
	if ((DC_ENUMRESOLUTIONS == wCapability))
	{
		*dwResult = (DWORD)-1;
		return S_OK;
	}
//<S><C>To Fix SSDI HLK Bug #3846 . Harsh 20220426
	else if ((DC_STAPLE == wCapability))
	{
		//LPDEVMODE pdev = NULL;
		//OEMDEV *pOemDev = GetUniDrvPrivateDevmode(pPublicDM);
		_DEVDMODE dev;
		SecureZeroMemory(&dev, sizeof(_DEVDMODE));
		FillDevDModeData(&dev);
		CRegistryAPI retgApi;
		//retgApi.ReadData(pDeviceName, &pOemDev->scpi.dev);
		retgApi.ReadData(pDeviceName, &dev);
		DWORD dwstaple = (0 != dev.byFieldVDMStaple) ? 1 : 0;
		// DWORD dwstaple= retgApi.SetDriverDataToReg(pDeviceName, &pOemDev->scpi.dev);
		*dwResult = dwstaple;
		//<S><E>To Fix general tab issue,20210113 SSDI:Goutham 
		return S_OK;
	}
//<S><E>To Fix SSDI HLK Bug #3846 . Harsh 20220426
	else if(DC_MEDIAREADY == wCapability)
	{
		*dwResult = 0;
		return S_OK;
	}
	else if(DC_COLORDEVICE == wCapability)
	{
        eDeviceType wDeviceType = eColor;
        //<S><A> RedMine Ticket(#1040) Cluster:General tab values are not updated.: 2014.02.22, SSDI:Sonali
        //Commenting the below code as calling GetPureName truncates the srver name from printer name.
        //WCHAR			szPrinterName[MAX_PATH] = {0};
		//Calling GetPureName to get the pure name from lpPrintername which is"\\SONALI-SSDI01\SHARP UD2L PCL6,LocalsplOnly"
		//GetPureName(szPrinterName, pDeviceName, MAX_PATH, RTPR_PURENAME);
        //<E> RedMine Ticket(#1040) Cluster:General tab values are not updated.: 2014.02.22, SSDI:Sonali
		CRegistryAPI retgApi;
		retgApi.GetDeviceType(pDeviceName,wDeviceType);
		*dwResult = wDeviceType;
		return S_OK;
	}
#if 0
	//else if (DC_PAPERS == wCapability)
	//{
	//	DWORD dwNumberSupportedPapers = 0;
	//	GetSupportedPaper(pOutput, dwNumberSupportedPapers, pDeviceName);
	//	if (NULL == pOutput)
	//	{
	//		*dwResult = dwNumberSupportedPapers;
	//		return S_OK;
	//	}
	//	*dwResult = dwNumberSupportedPapers;

	//	//m_hStringResourceHandle;
	//	return S_OK;

	//}
	//else if (DC_PAPERNAMES == wCapability)
	//{
	//	char *pName = new char[255];

	//	::ZeroMemory(pName, 255);
	//	LoadStringResource(pDeviceName);

	//	//int paper = PaperSizeArray[0][0];
	//	//LoadString(m_hStringResourceHandle, paper + IDS_PAPERSIZE_BASE, (LPWSTR)pName, 255);
	//	PVOID pOutput = new PVOID[255];
	//	::ZeroMemory(pOutput, 255);

	//	pName = (char *)pOutput;
	//	int size = _countof(PaperSizeArray);
	//	for (int i = 0; i <= size; i++)
	//	{
	//		int paper = PaperSizeArray[i][0];
	//		LoadString(m_hStringResourceHandle, paper + IDS_PAPERSIZE_BASE, (LPWSTR)pName, 255);
	//		pName += SHDEVCAP_PAPERNAMES_SIZE;
	//	}
	//	
	//	//if (NULL == pOutput)
	//	//{
	//		*dwResult = (DWORD)pName;
	//	//}
	//	
	//	
	//	return S_OK;

	//}
	//else if (DC_PAPERSIZE == wCapability)
	//{
	//	POINT 		*pt = NULL;
	//	int size = _countof(PaperSizeArray);
	//	pt = new POINT[size];
	//	::ZeroMemory(pt, sizeof(pt) );

	//	PVOID pOutput = new PVOID[255];
	//	::ZeroMemory(pOutput, 255);

	//	pt = (POINT *)pOutput;
	//	for (int i = 0; i <= size; i++)
	//	{
	//		int paper = PaperSizeArray[i][0];
	//		int index = findindex0fpapersize(PaperSizeArray, size, paper);
	//		int unit = PaperSizeArray[index][3];
	//		switch (unit)
	//		{

	//		case MM_LOMETRIC:
	//			pt[i].x = PaperSizeArray[index][1];
	//			pt[i].y = PaperSizeArray[index][2];

	//			break;

	//		case MM_LOENGLISH:
	//			pt[i].x = InchToMilli(PaperSizeArray[index][1], MM_LOENGLISH, MM_LOMETRIC);
	//			pt[i].y = InchToMilli(PaperSizeArray[index][2], MM_LOENGLISH, MM_LOMETRIC);

	//			break;
	//		}

	//		
	//	}

	//	*dwResult = (DWORD)pt;

	//	return S_OK;

	//}
#endif
#if 0
	 //<S><A> RedMine Ticket(#1144) Trays are not displaying according to the Paper Size on MS-Office: 2014.04.08, SSDI:Gajanana D S
	//<S><D> RedMine Ticket(#1144) Trays are consistantly shown for all PaperSizes supported.All GPD drivers(HP and Samsung) Behave the same,SSDI::Sujan
	else if(DC_BINNAMES == wCapability)
	{
		DWORD dwNumberSupportedPaperSources = 0;
		GetSupportedPaperSource(pOutput,dwNumberSupportedPaperSources,pDeviceName);
		if(NULL == pOutput)
		{
			*dwResult = dwNumberSupportedPaperSources;
		}
		return S_OK
	}
	//<E> RedMine Ticket(#1144) Trays are consistantly shown for all PaperSizes supported.All GPD drivers(HP and Samsung) Behave the same,SSDI::Sujan
	//<E> RedMine Ticket(#1144) Trays are not displaying according to the Paper Size on MS-Office: 2014.04.08, SSDI:Gajanana D S
#endif
    return E_NOTIMPL;
}
#if 0
VOID COemUI2::GetSupportedPaper(PVOID& pOutput, DWORD& dwNumberSupportedPapers, PWSTR pDeviceName)
{

	LoadStringResource(pDeviceName);
	
	//pt = new POINT[size];
	//::ZeroMemory(pt, sizeof(pt));
	long 		*lData = NULL;
	WORD 		*pw = NULL;
	

	lData = new long[64];
	if (lData == NULL)
		goto EXIT;
	::ZeroMemory(lData, sizeof(lData));

	int lCount = _countof(PaperSizeArray);
	for (int i = 0; i <= lCount; i++)
	{
		lData[i] = (long) PaperSizeArray[i][0];
		//int index = findindex0fpapersize(PaperSizeArray, size, paper);
		//int unit = PaperSizeArray[index][3];
		//lData++;
	}
	dwNumberSupportedPapers = (DWORD)lCount;
	if (pOutput == NULL)
		goto EXIT;

	pw = (WORD  *)pOutput;
	for (int i = 0; i < lCount; i++)
	{
		*pw = (WORD)lData[i];
		pw++;

		
	}

		//if (0 == m_ResourceIDInfoMap.size())
		//{
		//	// Load GDL snap shot only when Resource ID Info map is not constructed.
		//	CGPDData gpddata(m_pCoreHelperUni, TRUE);
		//	m_ResourceIDInfoMap = gpddata.GetResourceIDInfoMap();

		//}
		//CGPDData gpddata(m_pCoreHelperUni, TRUE);
		//m_ResourceIDInfoMap = gpddata.GetResourceIDInfoMap();

		//DWORD dwOptions = 0;
		//PCSTR *ppszOptions = NULL;
		//if (NULL == m_pCoreHelperUni)
		//{
		//	return;
		//}
		//m_pCoreHelperUni->EnumOptions("PaperSize", &ppszOptions, &dwOptions);
		//auto iteratorPapersizeFeature = m_ResourceIDInfoMap.find("PaperSize");

		//LoadStringResource(pDeviceName);

		//WCHAR *pszItem = NULL;
		//if (iteratorPapersizeFeature != m_ResourceIDInfoMap.end())
		//{
		//	if (NULL != pOutput)
		//	{
		//		pszItem = (WCHAR*)pOutput;
		//		SecureZeroMemory(pszItem, SHDEVCAP_PAPERNAMES_SIZE*iteratorPapersizeFeature->second.uOptionResourceID.size() * sizeof(WCHAR));
		//	}

		//	for (DWORD dwOptionIndex = 0; dwOptionIndex < dwOptions; dwOptionIndex++)
		//	{
		//		BOOL bSupportPapersizeOption = TRUE;
		//		auto iteratorPaperSizeOption = iteratorPapersizeFeature->second.uOptionResourceID.find(ppszOptions[dwOptionIndex]);
		//		if (iteratorPaperSizeOption != iteratorPapersizeFeature->second.uOptionResourceID.end())
		//		{
		//			/*for (auto iConstraintPapersizeOption : iteratorPaperSizeOption->second.second.ConstraintFeatureOptionMessageIDList)
		//			{
		//				if (iConstraintPapersizeOption.uNumberOfConstraints == iConstraintPapersizeOption.ConstraintFeatureOptionIDList.size())
		//				{
		//					bSupportPapersizeOption = FALSE;
		//					break;
		//				}
		//			}*/
		//		}
		//		if (TRUE == bSupportPapersizeOption)
		//		{
		//			if (NULL != pOutput)
		//			{
		//				if (LoadString(m_hStringResourceHandle, iteratorPaperSizeOption->second.first, pszItem, sizeof(WCHAR)*SHDEVCAP_PAPERNAMES_SIZE) > 0)
		//				{
		//					pszItem += SHDEVCAP_PAPERNAMES_SIZE;

		//				}
		//			}
		//			dwNumberSupportedPapers++;
		//		}
		//	}
		//}

EXIT:

	if (lData != NULL)
		delete[] lData;
	}
#endif

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::DevQueryPrintEx
//
//  Synopsis:
//      This routine is used to determine print job compatibility with the
//      current driver.  The plug-in can supplement Unidrv's handling of this
//      routine.  If Unidrv determines that a job can be printed on the current
//      printer it will call this routine to ask the plug-in whether it can
//      also handle the current job.  If Unidrv determines that the job cannot
//      be printed, it will not call the plug-in.  Additionally, XPS drivers
//      built on the Unidrv and PScript UI modules may not recieve this call-
//      back.
//
//      Implementation of this routine is optional.  To indicate that the
//      plug-in does not support this call, return E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::DevQueryPrintEx(
    POEMUIOBJ               poemuiobj,
    PDEVQUERYPRINT_INFO     pDQPInfo,
    PDEVMODE                pPublicDM,
    PVOID                   pOEMDM
    )
{
    VERBOSE(DLLTEXT("COemUI2::DevQueryPrintEx entry."));

    UNREFERENCED_PARAMETER(poemuiobj);
    UNREFERENCED_PARAMETER(pDQPInfo);
    UNREFERENCED_PARAMETER(pPublicDM);
    UNREFERENCED_PARAMETER(pOEMDM);

    return E_NOTIMPL;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::UpgradePrinter
//
//  Synopsis:
//      Use this callback to upgrade any settings from previous versions
//      that are stored in the registry by the plug-in Not applicabe to
//      this sample.
//
//      Implementation of this routine is optional.  To indicate that the
//      plug-in does not support this call, return E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::UpgradePrinter(
    _In_ DWORD   dwLevel,
    _At_((PDRIVER_UPGRADE_INFO_1)pDriverUpgradeInfo, _In_) PBYTE   pDriverUpgradeInfo
    )
{
    VERBOSE(DLLTEXT("COemUI2::UpgradePrinter entry."));

    UNREFERENCED_PARAMETER(dwLevel);

    //<S><A> RedMine Ticket(#1021) pdate Driver: UD2L driver is not updated when latest binaries are provided: 2014.02.14, SSDI:Sonali
    //In Upgrade scenario, the "Privated Devmode" key is not updated, hnece rewriting in UpgradePrinter call.
    LPDRIVER_UPGRADE_INFO_1	lpUpgradeInfo	= (LPDRIVER_UPGRADE_INFO_1)pDriverUpgradeInfo;
    POEMDEV pDefaultDevMode = new OEMDEV;
    if(NULL == pDefaultDevMode)
    {
        return E_FAIL;
    }
    // Set all the member variables to zero
    memset(pDefaultDevMode,0,sizeof(OEMDEV));

    CGPDData gpddata(m_pCoreHelperUni);
    m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();
    m_PaperInfo = gpddata.GetPaperInfoList();
    PrinterEventInitialize(lpUpgradeInfo->pPrinterName,pDefaultDevMode,m_DefDMInfo,m_PaperInfo);

    if(NULL != pDefaultDevMode)
    {
        delete pDefaultDevMode;
        pDefaultDevMode = NULL;
    }
    //<E> RedMine Ticket(#1021) pdate Driver: UD2L driver is not updated when latest binaries are provided: 2014.02.14, SSDI:Sonali
    return S_OK;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::PrinterEvent
//
//  Synopsis:
//      Perform any special processing needed when various events occur to the
//      print queue.
//
//      Implementation of this routine is optional.  To indicate that the
//      plug-in does not support this call, return E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::PrinterEvent(
    _In_ PWSTR   pPrinterName,
    _In_ INT     iDriverEvent,
    _In_ DWORD   dwFlags,
    _In_ LPARAM  lParam
    )
{
    VERBOSE(DLLTEXT("COemUI2::PrinterEvent entry."));

    UNREFERENCED_PARAMETER(dwFlags);
    UNREFERENCED_PARAMETER(lParam);

    if (NULL == pPrinterName)
    {
        return E_FAIL;
    }
   
    BOOL bRet = FALSE;

    switch(iDriverEvent)
    {
    case PRINTER_EVENT_ADD_CONNECTION:
        VERBOSE(DLLTEXT("PRINTER_EVENT_ADD_CONNECTION\n"));
        break;

    case PRINTER_EVENT_DELETE_CONNECTION:
        VERBOSE(DLLTEXT("PRINTER_EVENT_DELETE_CONNECTION\n"));
        break;

    case PRINTER_EVENT_CACHE_REFRESH:
        VERBOSE(DLLTEXT("PRINTER_EVENT_CACHE_REFRESH\n"));
        break;

    case PRINTER_EVENT_CACHE_DELETE:
        VERBOSE(DLLTEXT("PRINTER_EVENT_CACHE_DELETE\n"));
        break;

    case PRINTER_EVENT_INITIALIZE:
        {    
            POEMDEV pDefaultDevMode = new OEMDEV;
            if(NULL == pDefaultDevMode)
            {
                return E_FAIL;
            }
            // Set all the member variables to zero
            memset(pDefaultDevMode,0,sizeof(OEMDEV));

            CGPDData gpddata(m_pCoreHelperUni);
            m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();
            m_PaperInfo = gpddata.GetPaperInfoList();
            //LoadStringResource();
			CPrintCoreHelper cprintcorehelper(m_pCoreHelperUni);
			FeatureInfoList = cprintcorehelper.RetrieveFeaturesAndOptions();
            bRet = PrinterEventInitialize(pPrinterName,pDefaultDevMode,m_DefDMInfo,m_PaperInfo);
            WCHAR			szPrinterName[MAX_PATH] = {0};
			//Calling GetPureName to get the pure name from lpPrintername which is"\\SONALI-SSDI01\SHARP UD2L PCL6,LocalsplOnly"
			GetPureName(szPrinterName, pPrinterName, MAX_PATH, RTPR_PURENAME);
#if 1 //watermark spec change
          // CShIniFile cshinifile;
			TCHAR szWaterMarkDatFilePath[_MAX_PATH] = {0};
			GetProjectFileName(szWaterMarkDatFilePath,L"wm62.dat");
			
			CShIniFile* cshinifile = new CShIniFile(ghInstance, pPrinterName, szWaterMarkDatFilePath,FALSE);
			CWaterMarkRegistryData watermarkregdata(ghInstance, pPrinterName,m_hStringResourceHandle);
			watermarkregdata.reset(szPrinterName,cshinifile);
			delete cshinifile;
			cshinifile = NULL ;
#endif
            if(NULL != pDefaultDevMode)
            {
                delete pDefaultDevMode;
                pDefaultDevMode = NULL;
            }
            wcscpy_s(m_pPrinterName,MAX_PATH,szPrinterName);
        }
        break;

    case PRINTER_EVENT_DELETE:
        VERBOSE(DLLTEXT("PRINTER_EVENT_DELETE\n"));
        break;

    default:
        SetLastError(ERROR_INVALID_PARAMETER);
        break;
    }
    return bRet ? S_OK : E_FAIL;
}

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::DriverEvent
//
//  Synopsis:
//      Notifies the plug-in of changes or events relevant to the driver,
//      such as installing and upgrading.
//
//      Implementation of this routine is optional.  To indicate that the
//      plug-in does not support this call, return E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::DriverEvent(
    _In_ DWORD   dwDriverEvent,
    _In_ DWORD   dwLevel,
    _In_reads_(_Inexpressible_("varies")) LPBYTE  pDriverInfo,
    _In_ LPARAM  lParam
    )
{
    VERBOSE(DLLTEXT("COemUI2::DriverEvent entry."));

    UNREFERENCED_PARAMETER(dwDriverEvent);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(pDriverInfo);
    UNREFERENCED_PARAMETER(lParam);

    return E_NOTIMPL;
};


//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::QueryColorProfile
//
//  Synopsis:
//      This routine can be implemented to provide a color profile from the
//      driver.
//
//      Implementation of this routine is optional.  To indicate that the
//      plug-in does not support this call, return E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::QueryColorProfile(
            HANDLE      hPrinter,
            POEMUIOBJ   poemuiobj,
            PDEVMODE    pPublicDM,
            PVOID       pOEMDM,
            ULONG       ulQueryMode,
            VOID       *pvProfileData,
            ULONG      *pcbProfileData,
            FLONG      *pflProfileData
            )
{
    VERBOSE(DLLTEXT("COemUI2::QueryColorProfile entry."));

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(poemuiobj);
    UNREFERENCED_PARAMETER(pPublicDM);
    UNREFERENCED_PARAMETER(pOEMDM);
    UNREFERENCED_PARAMETER(ulQueryMode);
    UNREFERENCED_PARAMETER(pvProfileData);
    UNREFERENCED_PARAMETER(pcbProfileData);
    UNREFERENCED_PARAMETER(pflProfileData);

    return E_NOTIMPL;
};

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::FontInstallerDlgProc
//
//  Synopsis:
//      Plug-ins can use this method to replace Unidrv's provided soft-font
//      installer dialog.
//
//      Implementation of this routine is optional.  To indicate that the
//      plug-in does not support this call, return E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::FontInstallerDlgProc(
    HWND    hWnd,
    UINT    usMsg,
    WPARAM  wParam,
    LPARAM  lParam
    )
{
    VERBOSE(DLLTEXT("COemUI2::FontInstallerDlgProc entry."));

    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(usMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    return E_NOTIMPL;
};

//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::UpdateExternalFonts
//
//  Synopsis:
//      This routine is used to notify the UI of any changes to installed
//      font cartridges, primarily for supporting soft-font UI replacement.
//
//      Implementation of this routine is optional.  To indicate that the
//      plug-in does not support this call, return E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::UpdateExternalFonts(
        _In_ HANDLE  hPrinter,
        _In_ HANDLE  hHeap,
        _In_ PWSTR   pwstrCartridges
        )
{
    VERBOSE(DLLTEXT("COemUI2::UpdateExternalFonts entry."));

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(hHeap);
    UNREFERENCED_PARAMETER(pwstrCartridges);

    return E_NOTIMPL;
}

//+---------------------------------------------------------------------------
//
//  Member:
//
//  Synopsis:
//
//      Implementation of this routine is optional.  To indicate that the
//      plug-in does not support this call, return E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::QueryJobAttributes(
    _In_             HANDLE      hPrinter,
    _In_             PDEVMODE    pDevmode,
    _In_range_(1, 4) DWORD       dwLevel,
    _In_reads_(_Inexpressible_("varies")) LPBYTE      lpAttributeInfo
    )
{
    VERBOSE(DLLTEXT("COemUI2::QueryJobAttributes entry."));

    UNREFERENCED_PARAMETER(hPrinter);
    UNREFERENCED_PARAMETER(pDevmode);
    UNREFERENCED_PARAMETER(dwLevel);
    UNREFERENCED_PARAMETER(lpAttributeInfo);

    return E_NOTIMPL;
}

VOID COemUI2::UpdateColorInfo(PDEVMODEW pDevMode, WCHAR* pPrinterName)
{
    if (NULL == pDevMode)
    {
        return;
    }
    CRegistryAPI cRegAPI;
    eDeviceType enDeviceType = eColor;

    OEMDEV *pOemDev = GetUniDrvPrivateDevmode(pDevMode);
	//<S><A> to fix coverity issue 90892, Harika 20201123
	if (NULL == pOemDev)
	{
		return;
	}  
	//<E> to fix coverity issue 90892, Harika 20201123
        // get device type from local machine
    cRegAPI.GetDeviceType(pPrinterName, enDeviceType);
	cRegAPI.ReadData(pDevMode->dmDeviceName, &pOemDev->scpi.dev);
	
    if (eMono == enDeviceType)
    {
        pDevMode->dmColor = DMCOLOR_MONOCHROME;
        pOemDev->scpi.pub.dmColor = DMCOLOR_MONOCHROME;
        pOemDev->scpi.ext.ColData.byColMode = colmode_mono;
    }
    else
    {
        PV_COLMODE enColMode = colmode_auto;
        
        if (DMCOLOR_MONOCHROME == pDevMode->dmColor)
        {
            enColMode = colmode_mono;
        }
        else
        {
			if (pOemDev->scpi.dev.dwFieldVDM & DM_VDM_COLORMODE_AUTO_DISABLE)
			{
				pOemDev->scpi.ext.ColData.byColMode = colmode_color;
			}
			//<S><A>to fix redmine bug #2823,10-21-2020,SSDI:Goutham
            if (DMCOLOR_MONOCHROME == pOemDev->scpi.ext.ColData.byColMode)
            {
                enColMode = colmode_mono;
            }
			//<S><E>to fix redmine bug #2823,10-21-2020,SSDI:Goutham
            else
            {
                enColMode = (PV_COLMODE)pOemDev->scpi.ext.ColData.byColMode;
            }
        }

        pOemDev->scpi.pub.dmColor = (colmode_mono == enColMode) ? DMCOLOR_MONOCHROME : DMCOLOR_COLOR;
        pOemDev->scpi.ext.ColData.byColMode = (BYTE)enColMode;
        pDevMode->dmColor = pOemDev->scpi.pub.dmColor;
    }

    if (DMCOLOR_MONOCHROME == pOemDev->scpi.pub.dmColor)
    {
        pOemDev->scpi.pub.dmICMMethod = DMICMMETHOD_NONE;
        pDevMode->dmICMMethod = DMICMMETHOD_NONE;
    }
    else
    {
        pOemDev->scpi.pub.dmICMMethod = DMICMMETHOD_DEVICE;
        pDevMode->dmICMMethod = DMICMMETHOD_DEVICE;
    }
}
//+---------------------------------------------------------------------------
//
//  Member:
//      COemUI2::DocumentEvent
//
//  Synopsis:
//      Perform any special processing needed at various points in time while
//      a job is printing.
//
//      Implementation of this routine is optional.  To indicate that the
//      plug-in does not support this call, return E_NOTIMPL.
//
//
//----------------------------------------------------------------------------
HRESULT __stdcall
COemUI2::DocumentEvent(
    HANDLE      hPrinter,
    HDC         hdc,
    INT         iEsc,
    ULONG       cbIn,
    PVOID       pbIn,
    ULONG       cbOut,
    PVOID       pbOut,
    PINT        piResult
    )
{
    VERBOSE(DLLTEXT("COemUI2::DocumentEvent entry.")); 

    UNREFERENCED_PARAMETER(hdc);
    UNREFERENCED_PARAMETER(cbIn);
    UNREFERENCED_PARAMETER(cbOut);
	UNREFERENCED_PARAMETER(pbOut);
    
    //<S><A> RedMine Ticket(#842) Port is not changing to default port when test page is printed.: 2013.12.11, SSDI:Sonali
    //In Ports tab, Port should change to the port which is displayed as default in Settings tab(e.g. 172.29.240.80)
    //and test page should print from that port
    INT	rtc = DOCUMENTEVENT_UNSUPPORTED;

    
    CRegistryAPI regApi(ghInstance);
    WCHAR szPrinterName[MAX_PATH] = {};   
    DWORD			dwNeeded = 0;
    GetPrinter(hPrinter, 2, NULL, 0, &dwNeeded);
    BYTE* pTemp = NULL;
    if (0 < dwNeeded)
    {
        pTemp = new BYTE[dwNeeded]();
    }
    GetPrinter(hPrinter, 2, pTemp, dwNeeded, &dwNeeded);
    if(NULL != pTemp)
    {
        PRINTER_INFO_2* pPI2 = reinterpret_cast<PRINTER_INFO_2*>(pTemp);
        wcscpy_s(szPrinterName, MAX_PATH,pPI2->pPrinterName);
        delete[] pTemp;
        pTemp = NULL;
    }
    
    regApi.CheckPrinterIcon(szPrinterName,m_bIsPrinterIcon);    
    LoadDialogResource(szPrinterName);
    rtc  = DocumentEventInitialize(hPrinter,(LONG)iEsc,m_bIsPrinterIcon);

    // While printing, application calls DocumentEvent with CreateDCPre or ResetDCPre.
    // update color information in devmode 
	if ((DOCUMENTEVENT_CREATEDCPRE == iEsc) || (DOCUMENTEVENT_RESETDCPRE == iEsc))
	{
		PDEVMODEW pDevMode = NULL;

		if (DOCUMENTEVENT_CREATEDCPRE == iEsc)
		{
			pDevMode = ((PDCEVENT_CREATEDCPRE)pbIn)->pdm;
		}
		else // DOCUMENTEVENT_RESETDCPRE 
		{
			pDevMode = (PDEVMODEW)*((PDEVMODEW*)pbIn);
		}

		*((PDEVMODEW*)pbOut) = pDevMode;
		UpdateColorInfo(pDevMode, szPrinterName);

		//<S><A> Redmine #2066 Staple is not working when Flip on Short Edge is set on Metro app: 2015.03.23, SSDI:Sonali
		//Updating Binding Edge information when printed from any application without opening Printer UI.
		UpdateBindingEdgeInfo(pDevMode, szPrinterName);
		//<E> Redmine #2066 Staple is not working when Flip on Short Edge is set on Metro app: 2015.03.23, SSDI:Sonali

		//<S><A> Redmine #1981 Windows 10: ìPCL XL errorî is displayed when ë1 Stapleí is set from metro app: 2015.03.26, SSDI:Sonali
		//Updating Duplex information when printed from any application without opening Printer UI.
		//Always setting Duplex as Flip on long edge when Pamphplet is already selected from Control Panel.
		UpdateDuplexInfo(pDevMode, szPrinterName);
		//<E> Redmine #1981 Windows 10: ìPCL XL errorî is displayed when ë1 Stapleí is set from metro app: 2015.03.26, SSDI:Sonali
		// <S><A> To Fix redmine#2810, 2021.09.06, SSDI:Uday
		//Updating Job Staple when printed from any application without opening Printer UI.
		UpdateJobStaple(pDevMode, szPrinterName);
		// <E> To Fix redmine#2810, 2021.09.06, SSDI:Uday
		//<S><A> To update Job Handling tab "UserName" based on PPLCY dialog, Harika, 20201019
		UpdateDevDMode(pDevMode, szPrinterName);
		//<E> To update Job Handling tab "UserName" based on PPLCY dialog, Harika, 20201019
		//<S><A>To Fix Redmine Bug #4363 issue 1 and 2,08.04.2022,SSDI:Chanchasl Singla
		UpdateCollateInfo(pDevMode, szPrinterName);
		//<E>To Fix Redmine Bug #4363 issue 1 and 2,08.04.2022,SSDI:Chanchasl Singla
		g_cPTPCStatus = 1;// its indicates we are using Metro Application //To Fix redmine #4701 
		CPrintCoreHelper cprintcorehelper(m_pCoreHelperUni);
		m_pFeatureInfoList = cprintcorehelper.RetrieveFeaturesAndOptions();
	}
    if(rtc == DOCUMENTEVENT_FAILURE)
    {
        return rtc;
    }
    //<E> RedMine Ticket(#842) Port is not changing to default port when test page is printed.: 2013.12.11, SSDI:Sonali
	

	if(DOCUMENTEVENT_EVENT(iEsc)==DOCUMENTEVENT_CREATEDCPRE) 
	{
		*piResult = DOCUMENTEVENT_SUCCESS;
		return DOCUMENTEVENT_SUCCESS;
	}

	if( (DOCUMENTEVENT_EVENT(iEsc)==DOCUMENTEVENT_STARTDOCPOST) )
	{
		DWORD uJobID = (DWORD)(((PULONG)pbIn)[0]); // Get JobID from pbIn[0]
		DWORD cbNeeded=0;
		
		BOOL bResult = GetJob( hPrinter, (DWORD)uJobID, 2, NULL, 0, &cbNeeded );

		if (0 == cbNeeded)
		{
			return DOCUMENTEVENT_SUCCESS;
		}

		LPBYTE pJob = new BYTE[cbNeeded]();

		if (NULL == pJob)
		{
			return DOCUMENTEVENT_SUCCESS;
		}

		bResult = GetJob( hPrinter, (DWORD)uJobID, 2, pJob, cbNeeded, &cbNeeded );

		if (FALSE == bResult)
		{
			delete[] pJob;
			pJob = NULL;
			return DOCUMENTEVENT_SUCCESS;
		}
		JOB_INFO_2 *pJobData = (JOB_INFO_2 *)pJob;
		
		OEMDEV *pOemDev = GetUniDrvPrivateDevmode(pJobData->pDevMode);
		regApi.ReadData(szPrinterName, &pOemDev->scpi.dev);

		//<S><A> DevMode update based on Windows login name as login name, Harika, 20201028
		if (pOemDev && (DM_PP_WINLOGIN_AS_LOGINN == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN)))
		{
			// --- DM_ALWAYSUSE_ACNUM
			BOOL wUseAcNum = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM ? bool_true : bool_false;
			// --- DM_USE_LOGINNAME
			BOOL wLoginName = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME ? bool_true : bool_false;
			// --- DM_USE_LOGINPASS
			BOOL wLoginPass = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS ? bool_true : bool_false;
			
			DWORD dwSize = sizeof(pOemDev->scpi.ext.JobCtrl.szLoginName);
			//Fixed Coverity #90896 ,20200112,SSDI;Chanchal Singla
			WCHAR		szLoginName[LOGIN_NAME_LENGTH * 2];
			::GetUserName(szLoginName, &dwSize);
			wcscpy_s(pOemDev->scpi.ext.JobCtrl.szLoginName, LOGIN_NAME_LENGTH, szLoginName);
			if ((wLoginName == bool_true) && (wLoginPass == bool_true)
				&& (wUseAcNum == bool_false))
			{
				// --- DM_ALWAYSUSE_ACNUM , wUseAcNum = bool_false
				pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
				// --- DM_USE_LOGINNAME ,wLoginName = bool_true
				pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
				// --- DM_USE_LOGINPASS, wLoginPass = bool_true
				pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINPASS;
			}
			else
			{
				// --- DM_ALWAYSUSE_ACNUM, wUseAcNum = bool_false
				pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
				// --- DM_USE_LOGINNAME ,wLoginName = bool_true
				pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
				// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
				pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
			}

		}
		//<E> DevMode update based on Windows login name as login name, Harika, 20201028

		if (pOemDev && (DM_PP_USERAUTH == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_USERAUTH)))
		{
			// --- DM_ALWAYSUSE_ACNUM, wUseAcNum = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
			// --- DM_USE_LOGINNAME ,wLoginName = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;
			// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
			if (pOemDev && (DM_PP_WINLOGIN_AS_LOGINN == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN)))
			{
				// --- DM_ALWAYSUSE_ACNUM, wUseAcNum = bool_false
				pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
				// --- DM_USE_LOGINNAME ,wLoginName = bool_true
				pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
				// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
				pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
			}

		}

		if (pOemDev && (DM_PP_LOGINN_AS_USERN == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_LOGINN_AS_USERN)))
		{
			WCHAR* pTemp = pOemDev->scpi.ext.JobCtrl.szLoginName;
			memcpy(pOemDev->scpi.ext.JobCtrl.szUsername, pTemp, USER_NAME_LENGTH * sizeof(WCHAR));
		}

		if (pOemDev && (DM_SINGLESIGNON == (pOemDev->scpi.dev.dwFieldVDM & DM_SINGLESIGNON)))
		{
			// --- DM_ALWAYSUSE_ACNUM
			BOOL wUseAcNum = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM ? bool_true : bool_false;
			// --- DM_USE_LOGINNAME
			BOOL wLoginName = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME ? bool_true : bool_false;
			// --- DM_USE_LOGINPASS
			BOOL wLoginPass = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS ? bool_true : bool_false;
			// --- DM_USE_SINGLESIGNON
			BOOL wLoginNameSSO = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_SINGLESIGNON ? bool_true : bool_false;
			DWORD dwSize = sizeof(pOemDev->scpi.ext.JobCtrl.szLoginName);
			//Fixed Coverity #90896 ,20200112,SSDI;Chanchal Singla
			WCHAR		szLoginName[LOGIN_NAME_LENGTH * 2];
			::GetUserName(szLoginName, &dwSize);
			wcscpy_s(pOemDev->scpi.ext.JobCtrl.szLoginName, LOGIN_NAME_LENGTH, szLoginName);
			if ((wLoginName == bool_false) && (wLoginPass == bool_false)
				&& (wUseAcNum == bool_false) && (wLoginNameSSO == bool_true))
			{
				// --- DM_ALWAYSUSE_ACNUM , wUseAcNum = bool_false
				pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
				// --- DM_USE_LOGINNAME ,wLoginName = bool_false
				pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;
				// --- DM_USE_LOGINPASS, wLoginPass = bool_false
				pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
				// --- DM_USE_SINGLESIGNON, wLoginNameSSO = bool_true
				pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_SINGLESIGNON;
				/*LPBYTE pData = NULL;
				DWORD dwData = 0;
				LPDWORD pdwNeeded = NULL;
				(*preg).ReadRegData(HKEY_CURRENT_USER, REG_KEY_SPN_ROOT_BASE, REG_ENT_SSO_SERVICE_TICKET, REG_BINARY, (WCHAR FAR *)&dwData, &dwSize);
*/
				//pData = (LPBYTE)new char[dwData + 16];
				//callGetSingleSignOn(szPrinterName, regApi);
				 //GetSingleSignOn(szPrinterName, regApi, pData, pdwNeeded);
			}
		}

		if(nullptr != pOemDev)
		{

            BOOL AutoJobControlReview = (DM_AUTOJOB == (pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_AUTOJOB)) ? TRUE : FALSE;

            LoadStringResource(szPrinterName);

            HWND hWnd = GetParentWnd();

			CShowJobHandlingMessage ObjJobHandling(hWnd,m_hStringResourceHandle);
			if(0 != ObjJobHandling.Message(hWnd,pOemDev->scpi.ext.JobCtrl))
			{
				AutoJobControlReview = TRUE ;
			}

			if(AutoJobControlReview == TRUE)
			{
				if(0 == wcsnlen_s(pOemDev->scpi.ext.JobCtrl.szJobname,JC_STR_JOBNAME))
				{
					memcpy_s(&pOemDev->scpi.ext.JobCtrl.szJobname,JOB_NAME_LENGTH*sizeof(WCHAR),pJobData->pDocument,JOB_NAME_LENGTH*sizeof(WCHAR));
				}
		// <S><A> Implemented Job Control Dialog UI,2020.12.10, SSDI:Chandrashekar .V
				
				//ShowJobHandlingDialogAndUpdateTheJobData(hWnd,hPrinter,&(pOemDev->scpi),pJob,uJobID);
				DisplayJobHandlingDialog(hWnd, hPrinter, pJobData->pDevMode, pOemDev, pJob, uJobID, szPrinterName);
		// <E> Implemented Job Control Dialog UI,2020.12.10, SSDI:Chandrashekar .V
				DestroyDummyWindow(hWnd);

				delete[] pJob;
				pJob = NULL;

				*piResult = DOCUMENTEVENT_SUCCESS;
			}
			// <S><A> Implemented UserAuthentication Dialog UI,2021.06.28, SSDI:Chanchal Singla
			else
			{
				//PPUserAuthentication
				SHORT wPPlcyUserAuth = pOemDev->scpi.dev.dwFieldVDM & DM_PP_USERAUTH ? bool_true : bool_false;
				if (wPPlcyUserAuth == bool_true)
				{
					if (0 == wcsnlen_s(pOemDev->scpi.ext.JobCtrl.szJobname, JC_STR_JOBNAME))
					{
						memcpy_s(&pOemDev->scpi.ext.JobCtrl.szJobname, JOB_NAME_LENGTH * sizeof(WCHAR), pJobData->pDocument, JOB_NAME_LENGTH * sizeof(WCHAR));
					}
					DisplayUserAuthenticatonDialog(hWnd, hPrinter, pJobData->pDevMode, pOemDev, pJob, uJobID, szPrinterName);
					
					DestroyDummyWindow(hWnd);

					delete[] pJob;
					pJob = NULL;

					*piResult = DOCUMENTEVENT_SUCCESS;
				}
			}
			// <S><A> Implemented UserAUthentication Dialog UI,2021.06.28, SSDI:Chanchal Singla
		}
        if(pJob)
		{
			delete[] pJob;
			pJob = NULL;
		}
	}
	return DOCUMENTEVENT_SUCCESS;
}

inline VOID
COemUI2::DestroyPropPages(
    VOID
    )
{
    while (!m_DocumentPropertyPages.empty())
    {
        if (m_DocumentPropertyPages.back() != NULL)
        {
            delete m_DocumentPropertyPages.back();
            m_DocumentPropertyPages.back() = NULL;
        }

        m_DocumentPropertyPages.pop_back();
    }
}

inline VOID
COemUI2::DestroyDevicePropPages(
    VOID
    )
{
    while (!m_DevicePropertyPages.empty())
    {
        if (m_DevicePropertyPages.back() != NULL)
        {
            delete m_DevicePropertyPages.back();
            m_DevicePropertyPages.back() = NULL;
        }

        m_DevicePropertyPages.pop_back();
    }

 
}


/*++
Routine Name:

    COemUI2::CreatePropertyPages

Routine Description:

    The routine create property pages for each tab

Arguments:

    pOemPrivateDevMode - pointer to OEM private devmode
    pOemPublicDevMode - pointer to Public devmode

Return Value:

   S_OK
   
--*/
HRESULT
COemUI2::CreatePropertyPages(
      POEMDEV pOemPrivateDevMode,
		PDEVMODE pOemPublicDevMode
    ) 
{
	//Coverity Fix 90388 - SSDI:Seetharam - 20200828
	m_TabDlgFrame = NULL;

	HRESULT hr = S_OK;
    CPrintCoreHelper cprintcorehelper(m_pCoreHelperUni);
    m_pFeatureInfoList = cprintcorehelper.RetrieveFeaturesAndOptions();
	CGPDData gpddata(m_pCoreHelperUni, TRUE);
	m_ResourceIDInfoMap = gpddata.GetResourceIDInfoMap();
	m_ConstraintFeatureInfoMap = gpddata.GetConstraintFeatureInfoMap();
	m_PaperInfoList = gpddata.GetPaperInfoList();
	m_CustomPaperInfoList = gpddata.GetCustomPaperInfoList();
	m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();
	m_sWaterMarkIndex = pOemPrivateDevMode->scpi.ext.nWatermark;
	//<S><A> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
	m_wUsIndex = (SHORT)0x7fff;				// user setting index
	m_ppi.pblLimitByOptionTbl = NULL;
	m_ppi.dwFavoritesDataCount = 0;
	m_ppi.wUsIndex = 0;
	m_ppi.wDummy = 0;
	m_ppi.wFDJobHandling = 0;
	m_ppi.hWnd_Tab = NULL;
	m_ppi.blHideAllControl = FALSE;
	m_ppi.bllLimitByOptionSafe = FALSE;
	SecureZeroMemory(&m_ppi.PaperSize_upsd, sizeof(USERPAPERSIZEDATA));
	SecureZeroMemory(&m_ppi.FitToPaperSize_upsd, sizeof(USERPAPERSIZEDATA));

	//<S><D> Deleted the code because of pub structure memory corruption and when printer name is large around 260 bit ,
	//in that case  in devmode printer name some portion has missed,so in whole code only m_pPrinterName data SSDI is using, 2021.Dec.13,SSDI:Chanchal Singla
	//<S><A>to Fix Point and Print Bugs#4129,#4130,#4131,#4128,#4007,2021.Agust.18,SSDI:Chanchal Singla
	//While Clent and server scenario that time printer name is bigger but in devmode printer name some portion has missed
	// so here we are updating current printer name to pub.dmDeviceName devmode variable.
  //	rsize_t size = wcslen(m_pPrinterName) + 1;
	//::SecureZeroMemory(pOemPrivateDevMode->scpi.pub.dmDeviceName, (CCHDEVICENAME * sizeof(WCHAR)));
	//wcscpy_s(pOemPrivateDevMode->scpi.pub.dmDeviceName, size, m_pPrinterName);
	//memcpy(pOemPrivateDevMode->scpi.pub.dmDeviceName, m_pPrinterName, sizeof(pOemPrivateDevMode->scpi.pub.dmDeviceName));
	//<E>to Fix Point and Print Bugs#4129,#4130,#4131,#4128,#4007,2021.Agust.18,SSDI:Chanchal Singla
	//<E> Deleted the code because of pub structure memory corruption and when printer name is large around 260 bit ,
	//in that case  in devmode printer name some portion has missed,so in whole code only m_pPrinterName data SSDI is using, 2021.Dec.13,SSDI:Chanchal Singla
 //<S><E> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
	//<S><C> To Fix Redmine #2806,20201127,SSDI:Chanchal Singla
	//Main Tab : Color Mode drop down is displaying Black and White.
    // get device type from registry
    eDeviceType sDevicetype = eColor;  
    CRegistryAPI cRegApi(ghInstance);
    cRegApi.GetDeviceType(m_pPrinterName,sDevicetype);
	// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	cRegApi.ReadData(m_pPrinterName, &pOemPrivateDevMode->scpi.dev);
	// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V

	//<E> To Fix Redmine #2806,20201127,SSDI:Chanchal Singla
	//<S><A> RedMine Ticket(#1483) Millimeters and Inches are not displayed based on Region set: 2014.09.23, SSDI:Sonali
	//Since at Install time, GetUnitDef() returns the system langauge value nad not User defaults, hence once the installation is over,
	//Calling GetUnitDef() only once when the UI is opened first time based on the GPD feature "DefaultUnits" option set.
	//If option is "System",GetUnitDef() is called only first time.
	//The feature option pair is updated accordingly.
	if (0 == strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DEFAULTUNITS].pszOption, "System"))
	{
		pOemPrivateDevMode->scpi.ext.byCusUnits = (BYTE)GetUnitDef();
		//<S><C> To fix feedback issue 64, Harika, 20220830
		if (GetUnitDef() == unit_inch)
		{	
			//<S><A> Fix for feedback issue 64, Harika	20230214		
			pOemPrivateDevMode->scpi.ext.nDstWidthMilli = 2159;
			pOemPrivateDevMode->scpi.ext.nDstHeightMilli = 2794;
			//<E> Fix for feedback issue 64, Harika	20230214
			pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = 2124;
			pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = 3498;
		}
		else
		{
			//<S><A> Fix for feedback issue 64, Harika	20230214
			pOemPrivateDevMode->scpi.ext.nDstWidthMilli = 2099;
			pOemPrivateDevMode->scpi.ext.nDstHeightMilli = 2969;
			//<E> Fix for feedback issue 64, Harika	20230214
			pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = 2126;
			pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = 3496;
		}
		//<E> To fix feedback issue 64, Harika, 20220830
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DEFAULTUNITS].pszOption = "UserDefaults";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEFAULTUNITS].uChangedOptionIndex = eSecondOption;
		//<S><C>to Fix #Bug #3690-  issue 1,isuue 4,2021.12.22,SSDI:Chanchal Singla
		if (unit_mm == GetUnitDef())
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGIN_SHIFT_UNIT].pszOption = "Millimeters";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGIN_SHIFT_UNIT].uChangedOptionIndex = eFirstOption;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINT_POSITION_UNIT].pszOption = "Millimeters";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINT_POSITION_UNIT].uChangedOptionIndex = eFirstOption;
		}
		//<S><A>To Fix the Redmine Bug 4702-issue 2,3,4,5,23.03.2022,SSDI:chanchal Singla
		//Added New Static member g_cPTPCStatus , its motive to add-in code is that when we open any metro Application,
		//that time inside this condition code is executed every time, so restricts the code by adding this condition,
		//when any Word, Excel document has open at that time that variable is updating to True value,
// <S><C> To Fix feedback Issue #72,2023.03.17, SSDI:Chandrashekar .V
		if (g_cPTPCStatus == FALSE && (IsThunkProcess() == FALSE))
// <E> To Fix feedback Issue #72,2023.03.17, SSDI:Chandrashekar .V
		{
			//<S><A>To fix SSDI Bug #2089,24.06.2020,SSDI:Chanchal Parkash
			if (DMPAPER_A4 == GetPaperSizeDef())
			{
				// first time when install a driver Orignal size drop down Default Should be A4
				pOemPrivateDevMode->scpi.pub.dmPaperSize = DMPAPER_A4;
				PaperInfoMap::const_iterator it;
				it = m_PaperInfoList.find("A4");
				if (it != m_PaperInfoList.end())
				{
					pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)it->second.PaperWidth;
					pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)it->second.PaperLength;

				}
				pOemPrivateDevMode->scpi.pub.dmPaperLength = 2970;
				pOemPrivateDevMode->scpi.pub.dmPaperWidth = 2100;
				pOemPrivateDevMode->scpi.ext.nZoomSrcPaperSize = DMPAPER_A4;
				pOemPrivateDevMode->scpi.ext.nFTPPaperSize = DMPAPER_A4;
				pOemPrivateDevMode->scpi.ext.nPamphPaperSize = DMPAPER_A4;
				pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel = 4760;
				pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel = 6814;
				pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = 2 * pOemPrivateDevMode->scpi.doc.nLeftMarginPixel + pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel;
				pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = 2 * pOemPrivateDevMode->scpi.doc.nTopMarginPixel + pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel;

				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption = "A4";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex = eSecondOption;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uDefaultOptionIndex = eSecondOption;

			}
			//<S><A>To Fix SSDI Bug #2706,2020-09-04,SSDI:Chanchal Singla

			else if (DMPAPER_LETTER == GetPaperSizeDef())
			{
				pOemPrivateDevMode->scpi.pub.dmPaperSize = DMPAPER_LETTER;
				PaperInfoMap::const_iterator it;
				it = m_PaperInfoList.find("LETTER");
				if (it != m_PaperInfoList.end())
				{
					pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)it->second.PaperWidth;
					pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)it->second.PaperLength;

				}
				pOemPrivateDevMode->scpi.pub.dmPaperLength = 2794;
				pOemPrivateDevMode->scpi.pub.dmPaperWidth = 2159;
				pOemPrivateDevMode->scpi.ext.nZoomSrcPaperSize = DMPAPER_LETTER;
				pOemPrivateDevMode->scpi.ext.nFTPPaperSize = DMPAPER_LETTER;
				pOemPrivateDevMode->scpi.ext.nPamphPaperSize = DMPAPER_LETTER;
				pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel = 4900;
				pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel = 6400;
				pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = 2 * pOemPrivateDevMode->scpi.doc.nLeftMarginPixel + pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel;
				pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = 2 * pOemPrivateDevMode->scpi.doc.nTopMarginPixel + pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel;

				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption = "LETTER";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex = eSevenOption;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uDefaultOptionIndex = eSevenOption;
			}
			//	<E>To fix SSDI Bug #2089,24.06.2020,SSDI:Chanchal Parkash
			//<S><D> Commented the code to fix #4360, #4184 issues, Harika 20220307
		/*if (g_cPTPCStatus == FALSE)
		{*/
			//<S><A> To fix #4364 issue, Harika 20211229
			if (m_DefDMInfo[FEATURE_OREINTATION].iDevmodeValue == ori_port)
			{
				pOemPrivateDevMode->scpi.pub.dmOrientation = DMORIENT_PORTRAIT;
				pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_ROTATE180DEG;
				pOemPrivateDevMode->scpi.ext.dwFieldEDM2 &= ~DM_ROTATE180DEG_HW;
			}
			else if (m_DefDMInfo[FEATURE_OREINTATION].iDevmodeValue == ori_land)
			{
				pOemPrivateDevMode->scpi.pub.dmOrientation = DMORIENT_LANDSCAPE;
				pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_ROTATE180DEG;
				pOemPrivateDevMode->scpi.ext.dwFieldEDM2 &= ~DM_ROTATE180DEG_HW;
			}
			//<E> To fix #4364 issue, Harika 20211229
			//<E> Commented the code to fix #4360, #4184 issues, Harika 20220307
			if (m_DefDMInfo[FEATURE_COLLATE].iDevmodeValue == bool_true)
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLLATE].pszOption = "ON";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE].uChangedOptionIndex = eFirstOption;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE].uDefaultOptionIndex = eFirstOption;
				pOemPrivateDevMode->scpi.pub.dmCollate = TRUE;
				pOemPublicDevMode->dmCollate = TRUE;
			}
			else if (m_DefDMInfo[FEATURE_COLLATE].iDevmodeValue == bool_false)
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLLATE].pszOption = "OFF";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE].uChangedOptionIndex = eSecondOption;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE].uDefaultOptionIndex = eSecondOption;
				pOemPrivateDevMode->scpi.pub.dmCollate = FALSE;
				pOemPublicDevMode->dmCollate = FALSE;
			}
		}
		if (g_cPTPCStatus == TRUE)
		{
			g_cPTPCStatus = FALSE;
		}
		//<E>To Fix the Redmine Bug 4702-issue 2,3,4,5,23.03.2022,SSDI:chanchal Singla

		if (DMPAPER_A4 == GetPaperSizeDef())
		{
			SHORT byMaxPaperSize = (pOemPrivateDevMode->scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
			if (byMaxPaperSize == eFirstOption)
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET_OUTPUTSIZE].pszOption = "A3";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex = eFirstOption;
			}
			else if (byMaxPaperSize == eSecondOption)
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET_OUTPUTSIZE].pszOption = "A4";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex = eSecondOption;
			}
		}
		else if (DMPAPER_LETTER == GetPaperSizeDef())
		{
			SHORT byMaxPaperSize = (pOemPrivateDevMode->scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
			if (byMaxPaperSize == eFirstOption)
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET_OUTPUTSIZE].pszOption = "TABLOID";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex = eFourthOption;
			}
			else if (byMaxPaperSize == eSecondOption)
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET_OUTPUTSIZE].pszOption = "LETTER";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex = eFifthOption;
			}
		}
		//<E>to Fix #Bug #3690- issue 1,isuue 4,2021.12.22,SSDI:Chanchal Singla
		
		//<S><A>To  Fix Feedback #52 and Redmine Bug #4132,#4189,2021.August.03, SSDI:Chanchal Singla
		//Redmine Bug #4132 :- Color Mode displays incorrect value while changing Default values in GPD Tool
		//Redmine Bug #4189:-Default value of color mode is not reflected based on GPD tool settings.
		//Feedback #42 :- In ARM64 environment, the default value of Color Mode is not as set by GPD Tool.
		if (m_DefDMInfo[FEATURE_COLOR_MODE].iDevmodeValue == colmode_mono)
		{
			pOemPublicDevMode->dmColor = DMCOLOR_MONOCHROME;
			pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_mono;
		}
		else if (m_DefDMInfo[FEATURE_COLOR_MODE].iDevmodeValue == colmode_color)
		{
			pOemPublicDevMode->dmColor = DMCOLOR_COLOR;
			pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_color;
		}
		else if (m_DefDMInfo[FEATURE_COLOR_MODE].iDevmodeValue == colmode_auto)
		{
			pOemPublicDevMode->dmColor = DMCOLOR_COLOR;
			pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_auto;
		}
		//<E>To  Fix FEedback #52 and Redmine Bug #4132,#4189,2021.August.03, SSDI:Chanchal Singla
		//INPUT BIN___PAPER Tray
		GetDefaultFeatureOption_PaperTray((short)m_DefDMInfo[FEATURE_INPUTBIN].iDevmodeValue, m_pFeatureInfoList);
		
		//MEDIA TYPE==== PAPER Type
		GetDefaultFeatureOption_PaperType((short)m_DefDMInfo[FEATURE_MEDIATYPE].iDevmodeValue, m_pFeatureInfoList);
	}
	//<S><A>To fix Redmine #2027,20201021,SSDI:Chanchal Singla
	//Long Edge / Short Edge values are not retaining from 2 - Sided Printing drop down.
/*	pOemPrivateDevMode->scpi.pub.dmDuplex = pOemPrivateDevMode->scpi.ext.dmDuplexSave;
	m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex = (UINT)GetDuplexIndex(pOemPrivateDevMode->scpi.ext.dmDuplexSave);
	*///<E>To fix Redmine #2027,20201021,SSDI:Chanchal Singla

	// if device type is mono, set FEATURE_BWPRINT feature to ON and FEATURE_COLOR_MODE feature to Mono
	// if device type is color, 
	//  1. check for wFiledSPI member for color or mono model
	//  2. If it is color model, do not change FEATURE_BWPRINT and FEATURE_COLOR_MODE values.
	//  3. If it is mono model, set FEATURE_BWPRINT feature to OFF and FEATURE_COLOR_MODE feature to Auto
	//Color Mode
	if (pOemPrivateDevMode->scpi.dev.byFieldVDM & DM_VDM_COLORMODEL)
	{
		sDevicetype = eColor; //color
	}
	else
	{
		sDevicetype = eMono; //black and white
	}
	if (eMono == sDevicetype)
	{
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = eMonoMode;
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uDefaultOptionIndex = eMonoMode;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = "Mono";
		pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_mono;
	}
	else // eColor
	{
		BOOL bIsMonoModel = (MONO_MODEL_ID == pOemPrivateDevMode->scpi.wFieldSPI) ? TRUE : FALSE;

		if (bIsMonoModel)
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = "Auto";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = eAutoMode;
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uDefaultOptionIndex = eAutoMode;
			pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_auto;
			if ((m_DefDMInfo[FEATURE_MINIMUMLINEWIDTH].iDevmodeValue == bool_false))
			{
				pOemPrivateDevMode->scpi.doc.wPenWidthRate = WPENLINEWIDTH;
			}
		}
		else
		{
			if (pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_VDM_COLORMODE_AUTO_DISABLE)
			{
				pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_color;
			}

			if (DMCOLOR_COLOR == pOemPublicDevMode->dmColor)
			{
				if (colmode_mono == pOemPrivateDevMode->scpi.ext.ColData.byColMode)
				{
					pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_auto;
				}
			}
			if (DMCOLOR_MONOCHROME == pOemPublicDevMode->dmColor)
			{
				pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_mono;

			}
			//<E> RedMine Ticket(#1186,issue-2) : 2014.12.24, SSDI:Chnadrashekar V
			if (colmode_auto == pOemPrivateDevMode->scpi.ext.ColData.byColMode)
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = "Auto";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = eAutoMode;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uDefaultOptionIndex = eAutoMode;
				//<S><C>Uncommented to fix redmine Bug #3017,12-03-21 SSDI:Goutham 
			   //<S><C>To Fix Feedback 23,2021-02-24,SSDI:Chanchal Singla
				if ((m_DefDMInfo[FEATURE_MINIMUMLINEWIDTH].iDevmodeValue == bool_false))
				{
					pOemPrivateDevMode->scpi.doc.wPenWidthRate = WPENLINEWIDTH;
				}
				//<E>To Fix Feedback 23,2021-02-24,SSDI:Chanchal Singla
				//<S><E>Uncommented to fix redmine Bug #3017,12-03-21 SSDI:Goutham 
			}
			else if (colmode_color == pOemPrivateDevMode->scpi.ext.ColData.byColMode)
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = "Color";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = eColorMode;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uDefaultOptionIndex = eColorMode;
				//<S><C>Uncommented to fix redmine Bug #3017,12-03-21 SSDI:Goutham 
					  //<S><C>To Fix Feedback 23,2021-02-24,SSDI:Chanchal Singla
				if ((m_DefDMInfo[FEATURE_MINIMUMLINEWIDTH].iDevmodeValue == bool_false))
				{
					pOemPrivateDevMode->scpi.doc.wPenWidthRate = WPENLINEWIDTH;
				}
				//<E>To Fix Feedback 23,2021-02-24,SSDI:Chanchal Singla
		  //<S><E>Uncommented to fix redmine Bug #3017,12-03-21 SSDI:Goutham         
			}
			else
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = "Mono";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = eMonoMode;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uDefaultOptionIndex = eMonoMode;

			}
		}
	}

    CDocumentPropertyPage* pPropPage = NULL;
	//<S><A>To Add the Support Of Stamp Watermark Dialog,2020-04-09,SSDI:Chanchal Singla
	//Similar to UD3.1, When  Watermark Index Is 0 that time Dropdown of Stamp Should not be Set to Watermark
	//But if WaterMark index nWatermark is non Zero number (like 1,2,3,....etc) that time Stamp feature drop down Should be WaterMark
	if ((m_sWaterMarkIndex == 0)&& (m_DefDMInfo[FEATURE_STAMP].iDevmodeValue == bool_false))
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAMP].pszOption = "None";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAMP].uChangedOptionIndex = eFirstOption;
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAMP].uDefaultOptionIndex = eFirstOption;
	}
	//<E>To Add the Support Of Stamp Watermark Dialog,2020-04-09,SSDI:Chanchal Singla
	//<S><A>To Fix #3013,20201412,SSDI:Chanchal Singla
	//Mimic is not displayed correctly when Use Windows Login Name as Login Name is set in Printing policy dialog
	//PPWindowsLoginNameasUserName
	SHORT wPPlcyWinLogin = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN ? bool_true : bool_false;
	if (wPPlcyWinLogin == bool_true)
	{
		if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption, "LoginPass"))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "LoginPass";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eThirdOption;
		}
		else
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "LoginName";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eSecondOption;
		}	
	}
	//<E>To Fix #3013,20201412,SSDI:Chanchal Singla
	//<S><A>To Fix #3152,2020-01-06,SSDI:Chanchal Singla
	//Mimic is not displayed correctly when User Authentication is set in Printing policy dialog
		//PPUserAuthentication
	BOOL wPPlcyUserAuth = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_USERAUTH ? bool_true : bool_false;
	if (wPPlcyUserAuth == bool_true)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "None";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eFirstOption;
		if (wPPlcyWinLogin == bool_true)
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "LoginName";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eSecondOption;
		}
	}
	//<E>To Fix #3152,2020-01-06,SSDI:Chanchal Singla
    //<S><A> RedMine Ticket(#971) Black and White check box is displayed for Mono Model when printer is renamed: 2013.11.19, SSDI:Sonali
    // When printer is renamed with maximum characters, the printer name is truncated.
    //Hence commenting the below code to retain the printer name

    /*HANDLE	hPrinter = NULL;
    PRINTER_DEFAULTS PrinterDefaults = {NULL,NULL,PRINTER_ACCESS_USE};*/
    m_pPrivateDevmode = new OEMDEV;
    if(NULL == m_pPrivateDevmode)
    {
        return E_FAIL;
    }
    ::ZeroMemory(m_pPrivateDevmode,sizeof(OEMDEV));

    /* if(::OpenPrinter(pOemPublicDevMode->dmDeviceName,&hPrinter,&PrinterDefaults))
    {*/
    if (FALSE == ReadDevmodeDataFromRegistry(m_pPrivateDevmode,m_hPrinter))
    {
        //::ClosePrinter(hPrinter);
        return E_FAIL;
    }
// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	/*CRegistryAPI regApi;
	regApi.ReadData(m_pPrinterName, &pOemPrivateDevMode->scpi.dev);*/
// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	//<S><A> To Fix #2026,16-07-2020,SSDI:Chanchal Parkash
	//Custom paper value is not retaining from Original Size drop down
	if (DMPAPER_USER == pOemPrivateDevMode->scpi.pub.dmPaperSize)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption = "CUSTOMSIZE";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex = CUSTOM_PAPER_INDEX;
		//<S><A> To fix Redmine Bug #2255 Issue 4,26-08-2020,SSDI:Chanchal Singla
		//Apply button is enabled even though no value is changed in main tab
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uDefaultOptionIndex = CUSTOM_PAPER_INDEX;
		//<E> To fix Redmine Bug #2255 Issue 4,26-08-2020,SSDI:Chanchal Singla
	}
	//<S><A> Fixed Known Issue and Bug #3326.2021.06.28,SSDI:Chanchal Singla
	//Envelop paper size is not retaining
	//Paper type: Envelope is not retained when Custom Paper size is set
	if (DMMEDIA_ENVELOPE == pOemPrivateDevMode->scpi.pub.dmMediaType)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "ENVELOPE";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_Envelope;
		//<S><A> To fix Redmine Bug #2255 Issue 4,26-08-2020,SSDI:Chanchal Singla
		//Apply button is enabled even though no value is changed in main tab
		//m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uDefaultOptionIndex = PAPERTYPE_Envelope;
		//<E> To fix Redmine Bug #2255 Issue 4,26-08-2020,SSDI:Chanchal Singla
	}
	//<E> Fixed Known Issue:.2021.06.28,SSDI:Chanchal Singla
	//<E> To Fix #2026,16-07-2020,SSDI:Chanchal Parkash
	else if (DMMEDIA_LABEL == pOemPrivateDevMode->scpi.pub.dmMediaType)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "LABEL";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_Labels;
	}
	else if (DMMEDIA_T_OHP == pOemPrivateDevMode->scpi.pub.dmMediaType)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "TRANSPARENCY1";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_Transparency;
	}
	if (DMDUP_SIMPLEX == pOemPrivateDevMode->scpi.pub.dmDuplex)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption = "NONE";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex = duplex_none;
	}

		//<S><D> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
	
	//<S><C> To Fix Redmine Bug #2608,20210218,SSDI: Chanchal Singla 	
	//Orientation displays Portrait even for Landscape document
	//<S><A> to get the data in landscape 13/8/2020 K M Harika
	if ((DMORIENT_LANDSCAPE == pOemPrivateDevMode->scpi.pub.dmOrientation) && (ori_port == m_DefDMInfo[FEATURE_OREINTATION].iDevmodeValue || ori_port_rotated == m_DefDMInfo[FEATURE_OREINTATION].iDevmodeValue))
	{
		//Fixed Coverity issue - 90582 - SSDI:Seetharam-20201006
		//Actually here insetad of using double equals == for condition checking, 
		//assignment is used, which has been fixed
		//<S><C> To fix Coverity issues 90880, 90883. Harika 20201123
		//if (m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption == "LANDSCAPE_CC90")
		if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption, "PORTRAIT"))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption = "LANDSCAPE_CC90";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex = eSecondOption;
		}
		//else if (m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption == "LANDSCAPE_CC270")
		else if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption, "PORTRAIT_ROTATED"))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption = "LANDSCAPE_CC270";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex = eFourthOption;
		}
		//<E> To fix Coverity issues 90880, 90883. Harika 20201123
	}
	else if ((DMORIENT_PORTRAIT == pOemPrivateDevMode->scpi.pub.dmOrientation) && (ori_port == m_DefDMInfo[FEATURE_OREINTATION].iDevmodeValue || ori_port_rotated == m_DefDMInfo[FEATURE_OREINTATION].iDevmodeValue))
	 {
		if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption, "LANDSCAPE_CC90"))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption = "PORTRAIT";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex = eFirstOption;
		}
		else if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption, "LANDSCAPE_CC270"))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption = "PORTRAIT_ROTATED";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex = eThirdOption;
		}
	}
	//<E> To Fix Redmine Bug #2608,20210218,SSDI: Chanchal Singla 
	//<E> to get the data in landscape 13/8/2020 K M Harika
	//<S><A>To Fix Redmine #7423,21-06-2024,SSDI:Manoj S
	//<S><C>To Fix Redmine #7267,03-07-2024,SSDI:Manoj S
	BYTE Flag = 0;
	short byStaplelessFlag = pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_STAPLELESS ? bool_true : bool_false;
	{
		CShRegAC		*read = NULL;
		read = new CShRegAC(ghInstance, m_pPrinterName, NULL);
		if (read == NULL)
		{
			return false;
		}

		read->ReadStapleLessData(Flag);

		if (read != NULL)
		{
			delete	read;
			read = NULL;
		}
	}
	if (byStaplelessFlag && Flag)
	{
		if (!strcmp("STAPLE_STAPLELESS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption) &&
			!strcmp("FORMSOURCE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption = "BYPASSTRAY";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex = PAPERSRC_BypassTray;
		}
	}
	//<E>To Fix Redmine #7267,03-07-2024,SSDI:Manoj S
	/*if (m_DefDMInfo[FEATURE_STAPLING].iDevmodeValue == 3 && (!strcmp("FORMSOURCE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption)))
	{
		if (testbyStapleless && Flag)
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption = "BYPASSTRAY";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uDefaultOptionIndex = PAPERSRC_BypassTray;
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex = PAPERSRC_BypassTray;
		}
	}*/
	//<E>To Fix Redmine #7423,21-06-2024,SSDI:Manoj S
	//<S><A> Fix for #5714. K M Harika 16/03/23
	if (m_DefDMInfo[FEATURE_PAGESPERSHEET].iDevmodeValue == 2 || m_DefDMInfo[FEATURE_PAGESPERSHEET].iDevmodeValue == 6 || m_DefDMInfo[FEATURE_PAGESPERSHEET].iDevmodeValue == 8)
	{
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uDefaultOptionIndex = eThirdOption;
		pOemPrivateDevMode->scpi.ext.byBindedge = eThirdOption;
	}
	if (m_DefDMInfo[FEATURE_PAGESPERSHEET].iDevmodeValue > 2 && m_DefDMInfo[FEATURE_ORDER].iDevmodeValue == 1)
	//else if (m_DefDMInfo[FEATURE_PAGESPERSHEET].iDevmodeValue == 4 || m_DefDMInfo[FEATURE_PAGESPERSHEET].iDevmodeValue == 9 || m_DefDMInfo[FEATURE_PAGESPERSHEET].iDevmodeValue == 16)
	{
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uDefaultOptionIndex = eFourthOption;
	}
	if(m_DefDMInfo[FEATURE_BORDER].iDevmodeValue == 1 && m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BORDER].uChangedOptionIndex == eSecondOption && (!strcmp("ON", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BORDER].pszOption)))
		pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_NUPBORDER;
	if (m_DefDMInfo[FEATURE_HUNDRED_PERCENT_NUP].iDevmodeValue == 1 && m_pFeatureInfoList->pFeatureOptionsList[FEATURE_HUNDRED_PERCENT_NUP].uChangedOptionIndex == eSecondOption && (!strcmp("ON", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_HUNDRED_PERCENT_NUP].pszOption)))
		pOemPrivateDevMode->scpi.ext.wFieldEDM |= DM_SAMENUP;
	//<E> Fix for #5714. K M Harika 16/03/23
	//<S><A>To fix #2958,20201127,SSDI:Chanchal	Singla
	//Output size drop down value is not set to A4
	//when Maximum paper size value is changed in configuration tab
	if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "CUSTOMSIZE")
		&& pOemPrivateDevMode->scpi.dev.byMaxPaperSize == paper_121)
	{
		DWORD Width = 0;
		DWORD Length = 0;
		SHORT unit = GetUnitDef();
		USERPAPERSIZEDATA upsd;
		BOOL wCustom = FALSE;
		SecureZeroMemory(&upsd, sizeof(upsd));
		CUPSReg FAR		*pregUps = NULL;
		pregUps = new CUPSReg(m_hStringResourceHandle, m_pPrinterName);
		if (pregUps == NULL)
		{
			return 0;
		}
		CShIniFile			*m_pmcf = NULL;
		TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
		GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
		m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
		CShJsonUserPSize	*pjsonups = NULL;
		if ((*m_pmcf).IsWriteToJson() == TRUE)
		{
			pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
			pjsonups->Init();
			(*pjsonups).ReadUPSData(DMPAPER_CUSTOM_ZOOM, &upsd);
		}
		else
		{
			(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &upsd);
		}
		Width = upsd.dwWidth;
		Length = upsd.dwLength;
		unit = upsd.wUnit;
		//<S><C> To fix 3680-Output Size Custom Paper value is not retaining when Maximum Paper size is set to A4/Letter, Harika, 20210714
		if (unit == 0)
		{
			if ((Width > MAXPAPER_CUSTOM_WIDTH_MM))
			{
				wCustom = TRUE;
			}
			if ((Length > MAXPAPER_CUSTOM_LENGTH_MM))
			{
				wCustom = TRUE;
			}
			if (wCustom == TRUE)
			{
				UINT uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? OUTPUTPAPERSIZE_A4 : OUTPUTPAPERSIZE_LETTER;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex = uChangedOptionIndex;
			}
		}
		else
		{
			if ((Width > MAXPAPER_CUSTOM_WIDTH_INCH))
			{
				wCustom = TRUE;
			}
			if ((Length > MAXPAPER_CUSTOM_LENGTH_INCH))
			{
				wCustom = TRUE;
			}
			if (wCustom == TRUE)
			{
				UINT uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? OUTPUTPAPERSIZE_A4 : OUTPUTPAPERSIZE_LETTER;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex = uChangedOptionIndex;
			}
		}
		//<E> To fix 3680-Output Size Custom Paper value is not retaining when Maximum Paper size is set to A4/Letter, Harika, 20210714
		if (pregUps != NULL)
		{
			delete pregUps;
			pregUps = NULL;
		}
		if (m_pmcf != NULL)
		{
			delete m_pmcf;
			m_pmcf = NULL;
		}
		if (pjsonups != NULL)
		{
			delete pjsonups;
			pjsonups = NULL;
		}
	}
	//<E>To fix #2958,20201127,SSDI:Chanchal Singla
     //<S><A> RedMine Ticket(#1526) Custom Paper is not retained in the drop down when created with standard paper size values: 2014.09.23, SSDI:Sonali
    //Reading the dmFormName  to retrieve the flag info.
    //If Custom form, then PRint feature option pair is modified accordingly.
    //if(DMPAPER_USER == pOemPrivateDevMode->scpi.pub.dmPaperSize)
    //{
    //    if(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_CUSTOMPAPERSIZE].pszOption,"UserForm"))
    //    {
    //        DWORD dwNeeded = 0;
    //        GetForm(m_hPrinter,pOemPrivateDevMode->scpi.pub.dmFormName,1,NULL,0,&dwNeeded);    
    //        if(0 < dwNeeded)
    //        {
    //            BYTE* pCustomFormInfo = new BYTE[dwNeeded]();
    //            if(NULL != pCustomFormInfo)
    //            {
    //                DWORD pcbNeeded = 0;
    //                GetForm(m_hPrinter,pOemPrivateDevMode->scpi.pub.dmFormName,1,(PBYTE)pCustomFormInfo,dwNeeded,&pcbNeeded);
    //                FORM_INFO_1* pFormInfo = reinterpret_cast<FORM_INFO_1*>(pCustomFormInfo);
    //                if(NULL != pFormInfo && (FORM_USER == pFormInfo->Flags))
    //                {
    //                    //<S><A> RedMine Ticket(#1636) Custom Paper value is not updated when updated through forms tab: 2014.11.21, SSDI:Sonali
    //                    //Updating custom form values again if in case the form valus are changed from Forms Tab.
    //                    UpdateCustomForm(pOemPrivateDevMode,pFormInfo,pOemPrivateDevMode->scpi.ext.byCusUnits);
    //                    //<S><A> RedMine Ticket(#1636) Custom Paper value is not updated when updated through forms tab: 2014.11.21, SSDI:Sonali
    //                    m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption = "CUSTOMSIZE";
    //                    m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex = CUSTOM_PAPER_INDEX;
    //                }

    //                delete[] pCustomFormInfo;
    //                pCustomFormInfo = NULL;
    //            }
    //        }
    //        //<E> RedMine Ticket(#1526) Custom Paper is not retained in the drop down when created with standard paper size values: 2014.09.23, SSDI:Sonali
    //        //
    //        //<E> RedMine Ticket(#1483) Millimeters and Inches are not displayed based on Region set: 2014.09.23, SSDI:Sonali

    //        //<S><A> RedMine Ticket(#1509) Default values are not displayed in client when Custom Paper is deleted in server: 2014.10.14, SSDI:Sonali
    //        //When selected custom paper on client is deleted on server,the devmode also needs to updated with default values.
    //        //Hence calling UpdateValuesForDefaultCustomPaper().
    //        else 
    //        {
    //            UpdateValuesForDefaultCustomPaper(pOemPrivateDevMode);
    //        }
    //    }
    //}
    //<E> RedMine Ticket(#1509) Default values are not displayed in client when Custom Paper is deleted in server: 2014.10.14, SSDI:Sonali
	//<E> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
		
//<S><A> To Fix #1993 isuue 1,14-07-2020,SSDI:Chanchal Parkash
	//Copies text box accepts more than 999
	// if dmcopies is more than MAX_COPIES(999) then dmcopies values sets to MIN_COPIES(1)
	if(MAX_COPIES < pOemPrivateDevMode->scpi.pub.dmCopies)
	{
		pOemPrivateDevMode->scpi.pub.dmCopies = MIN_COPIES;
		pOemPublicDevMode->dmCopies = MIN_COPIES;
	}
//<E> To Fix #1993 isuue 1,14-07-2020,SSDI:Chanchal Parkash
	//<S><A>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	//Update private devmode tro propstate Structure
	m_ppi.pps = new PROPSTATE;
	if (m_ppi.pps == NULL)
		return 0;
	SecureZeroMemory(m_ppi.pps, sizeof(PROPSTATE));
	PrivateDevmodeDataToPropState(m_hStringResourceHandle, m_pPrinterName, pOemPrivateDevMode, m_ppi.pps);

	CShLimit limit(NULL, NULL);

	if (limit.limStapleOptVsStapleType(m_ppi.pps, m_ppi.pps->wStaple) != FALSE)
	{
		m_ppi.pps->wStaple = staple_none;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption = "NONE";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex = m_ppi.pps->wStaple;
		m_ppi.pps->blLimitByOption = bool_true;
	}
	if (limit.limPunchOptVsPunchType(m_ppi.pps, m_ppi.pps->wPunchType) != FALSE) {
		m_ppi.pps->wPunchType = punchtype_none;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption = "OFF";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex = m_ppi.pps->wPunchType;
		m_ppi.pps->wPunch = bool_false;
		m_ppi.pps->blLimitByOption = bool_true;	
	}
	//<E>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
    //::ClosePrinter(hPrinter);
    //}
    //<E> RedMine Ticket(#971) Black and White check box is displayed for Mono Model when printer is renamed: 2013.11.19, SSDI:Sonali
    //
    // This Prefast warning indicates that memory could be leaked
    // in the event of an exception. We suppress this false positive because we
    // know that the local try/catch block will clean up a single
    // CDocPropPage, and the destructor will clean up any that are
    // successfully added to the vector.
    //
#pragma prefast(push)
#pragma prefast(disable:__WARNING_ALIASED_MEMORY_LEAK_EXCEPTION)

	// <S><M> Create the tab frame that contains the buttons representing the menu items (for displaying different tabs) on selection of the button, 20200327
	m_TabDlgFrame = new (std::nothrow) CShTabDlgFrame(m_hStringResourceHandle, m_hBitmapResourceHandle, m_hDialogResourceHandle, m_pCoreHelperUni, &m_ResourceIDInfoMap, m_ConstraintFeatureInfoMap,
		m_pFeatureInfoList, m_PaperInfoList, m_CustomPaperInfoList, pOemPrivateDevMode, pOemPublicDevMode, m_pHelpFilePath, m_pHelpID, m_DefDMInfo, m_pPrivateDevmode, m_hPrinter, m_pPrinterName, m_sWaterMarkIndex,m_wUsIndex, &m_ppi);
	
    /*pPropPage = new(std::nothrow) CMainTabPropertyPage(m_hStringResourceHandle,m_hBitmapResourceHandle,m_hDialogResourceHandle,m_pCoreHelperUni,&m_ResourceIDInfoMap,m_ConstraintFeatureInfoMap,
		m_pFeatureInfoList,m_PaperInfoList,m_CustomPaperInfoList,pOemPrivateDevMode,pOemPublicDevMode,m_pHelpFilePath,m_pHelpID,m_DefDMInfo,m_pPrivateDevmode,m_hPrinter,m_pPrinterName,m_sWaterMarkIndex);

    pPropPage->SetDeviceType((eDeviceType)sDevicetype);
    hr = AddPropPage(pPropPage);
	if (SUCCEEDED(hr))
    {
        pPropPage = new(std::nothrow) CPaperTabPropertyPage(m_hStringResourceHandle,m_hBitmapResourceHandle,m_hDialogResourceHandle,m_pCoreHelperUni,&m_ResourceIDInfoMap,m_ConstraintFeatureInfoMap,
            m_pFeatureInfoList,m_PaperInfoList,m_CustomPaperInfoList,pOemPrivateDevMode,pOemPublicDevMode,m_pHelpFilePath,m_pHelpID,m_DefDMInfo,m_pPrivateDevmode,m_hPrinter,m_pPrinterName,m_sWaterMarkIndex);
        hr = AddPropPage(pPropPage);
    }
    if (SUCCEEDED(hr))
    {
        pPropPage = new(std::nothrow) CJobHandlingTabPropertyPage(m_hStringResourceHandle,m_hBitmapResourceHandle,m_hDialogResourceHandle,m_pCoreHelperUni,&m_ResourceIDInfoMap,m_ConstraintFeatureInfoMap,
            m_pFeatureInfoList,m_PaperInfoList,m_CustomPaperInfoList,pOemPrivateDevMode,pOemPublicDevMode,m_pHelpFilePath,m_pHelpID,m_DefDMInfo,m_pPrivateDevmode,m_hPrinter,m_pPrinterName,m_sWaterMarkIndex);

        hr = AddPropPage(pPropPage);
    }
    if (SUCCEEDED(hr))
    {
        pPropPage = new(std::nothrow) CWaterMarkTabPropertyPage(m_hStringResourceHandle,m_hBitmapResourceHandle,m_hDialogResourceHandle,m_ConstraintFeatureInfoMap,
            m_pFeatureInfoList,m_PaperInfoList,m_CustomPaperInfoList,pOemPrivateDevMode,pOemPublicDevMode,m_pHelpFilePath,m_pHelpID,m_DefDMInfo,m_pPrivateDevmode,m_hPrinter,m_pPrinterName,m_sWaterMarkIndex);
        hr = AddPropPage(pPropPage);

    }
//<S><A>Added for finishing and layout tab,20.02.14,SSDI:Goutham
    if (SUCCEEDED(hr))
	{
		pPropPage = new(std::nothrow) CFinishingTabPropertyPage(m_hStringResourceHandle, m_hBitmapResourceHandle, m_hDialogResourceHandle, m_pCoreHelperUni, &m_ResourceIDInfoMap, m_ConstraintFeatureInfoMap,
			m_pFeatureInfoList, m_PaperInfoList, m_CustomPaperInfoList, pOemPrivateDevMode, pOemPublicDevMode, m_pHelpFilePath, m_pHelpID, m_DefDMInfo, m_pPrivateDevmode, m_hPrinter, m_pPrinterName, m_sWaterMarkIndex);
		hr = AddPropPage(pPropPage);
	}
	if (SUCCEEDED(hr))
	{
		pPropPage = new(std::nothrow) CLayoutTabPropertyPage(m_hStringResourceHandle, m_hBitmapResourceHandle, m_hDialogResourceHandle, m_pCoreHelperUni, &m_ResourceIDInfoMap, m_ConstraintFeatureInfoMap,
			m_pFeatureInfoList, m_PaperInfoList, m_CustomPaperInfoList, pOemPrivateDevMode, pOemPublicDevMode, m_pHelpFilePath, m_pHelpID, m_DefDMInfo, m_pPrivateDevmode, m_hPrinter, m_pPrinterName, m_sWaterMarkIndex);
		hr = AddPropPage(pPropPage);
	}
//<E>Added for finishing and layout tab,20.02.14,SSDI:Goutham
    if(eMono != sDevicetype) 
    {
        if (SUCCEEDED(hr))
        {
            pPropPage = new(std::nothrow) CColorTabPropertyPage(m_hStringResourceHandle,m_hBitmapResourceHandle,m_hDialogResourceHandle,m_pCoreHelperUni,&m_ResourceIDInfoMap,m_ConstraintFeatureInfoMap,
                m_pFeatureInfoList,m_PaperInfoList,m_CustomPaperInfoList,pOemPrivateDevMode,pOemPublicDevMode,m_pHelpFilePath,m_pHelpID,m_DefDMInfo,m_pPrivateDevmode,m_hPrinter,m_pPrinterName,m_sWaterMarkIndex);
            hr = AddPropPage(pPropPage);
        }
    }
    if (FAILED(hr))
    {
        //
        // If we successfully created a property page but failed to push it onto
        // the vector we need to free the allocated property page
        //
        if (pPropPage != NULL)
        {
            delete pPropPage;
            pPropPage = NULL;
        }
    }*/
	// <S><E> Create the tab frame that contains the buttons representing the menu items (for displaying different tabs) on selection of the button, 20200327
#pragma prefast(pop)

    return hr;
}
HRESULT
COemUI2::CreateDevicePropertyPages(HANDLE hPrinter )
{
    HRESULT hr = S_OK;

    CDevicePropertyPage* pPropPage = NULL;
	CPrintCoreHelper cprintcorehelper(m_pCoreHelperUni);
	if (m_pFeatureInfoList == NULL)
	{
		m_pFeatureInfoList = cprintcorehelper.RetrieveFeaturesAndOptions();
	}
	CGPDData gpddata(m_pCoreHelperUni, TRUE);
	m_ResourceIDInfoMap = gpddata.GetResourceIDInfoMap();
	m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();
	m_wUsIndex = (SHORT)0x7fff;				// user setting index
	m_ppi.pblLimitByOptionTbl = NULL;
	m_ppi.dwFavoritesDataCount = 0;
	m_ppi.wUsIndex = 0;
	m_ppi.wDummy = 0;
	m_ppi.wFDJobHandling = 0;
    //
    // This Prefast warning indicates that memory could be leaked
    // in the event of an exception. We suppress this false positive because we
    // know that the local try/catch block will clean up a single
    // CDocPropPage, and the destructor will clean up any that are
    // successfully added to the vector.
    //
#pragma prefast(push)
#pragma prefast(disable:__WARNING_ALIASED_MEMORY_LEAK_EXCEPTION)

    CRegistryAPI regApi(ghInstance);
    regApi.CheckPrinterIcon(m_pPrinterName,m_bIsPrinterIcon);
	pPropPage = new(std::nothrow) CDevicePropertyPage(hPrinter,m_hStringResourceHandle,m_hBitmapResourceHandle,m_hDialogResourceHandle,m_pPrinterName,
													  m_bIsPrinterIcon,m_pHelpFilePath,m_pHelpID, m_pFeatureInfoList, m_DefDMInfo, &m_ResourceIDInfoMap, m_pCoreHelperUni, ghInstance,m_wUsIndex, &m_ppi);

    hr = AddDevicePropPage(pPropPage);

    if (FAILED(hr))
    {
        //
        // If we successfully created a property page but failed to push it onto
        // the vector we need to free the allocated property page
        //
        if (pPropPage != NULL)
        {
            delete pPropPage;
            pPropPage = NULL;
        }
    }

#pragma prefast(pop)

    return hr;
}
//
// Use __drv_aliasesMem annotation to avoid PREfast warning 28197: Possibly leaking memory,
//
HRESULT
// Prefast warning 28194: The function was declared as aliasing the value in variable and exited without doing so.
// We suppress this false positive because STL vector does not have annotation, and we know the pointer has been saved
// to STL vector, which is released in DestroyPropPages().
#pragma warning(suppress: 28194)
COemUI2::AddPropPage(
    _In_opt_ __drv_aliasesMem CDocumentPropertyPage* pPropPage
    )
{
    HRESULT hr = S_OK;
	if(NULL == pPropPage)
	{
		return E_FAIL;
	}
	m_DocumentPropertyPages.push_back(pPropPage);
	
	/*CHECK_POINTER(pPropPage, E_OUTOFMEMORY);

    if (SUCCEEDED(hr))
    {
        m_DocumentPropertyPages.push_back(pPropPage);
    }*/

    return hr;
}
//
// Use __drv_aliasesMem annotation to avoid PREfast warning 28197: Possibly leaking memory,
//
HRESULT
// Prefast warning 28194: The function was declared as aliasing the value in variable and exited without doing so.
// We suppress this false positive because STL vector does not have annotation, and we know the pointer has been saved
// to STL vector, which is released in DestroyPropPages().
#pragma warning(suppress: 28194)
COemUI2::AddDevicePropPage(
    _In_opt_ __drv_aliasesMem CDevicePropertyPage* pPropPage
    )
{
    HRESULT hr = S_OK;
	if(NULL == pPropPage)
	{
		return E_FAIL;
	}
	m_DevicePropertyPages.push_back(pPropPage);
	
    return hr;
}
/*++

Routine Name:

    CXDSmplUI::LoadStringResource

Routine Description:

   Load the String Resource all

Arguments:

    None

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HMODULE COemUI2::LoadStringResource(WCHAR* pPrinterName)
{
    //<S><A>Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    //Putting NULL check to ensure that the dlls are not loaded second time if it is already loaded.
    if(NULL == m_hStringResourceHandle)
    {
        // get language ID 
        // get language id from registry
        TCHAR szCommonDatFilePath[_MAX_PATH] = {0};
        GetProjectFileName(szCommonDatFilePath,L"Common.DAT");
	//<S><C>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
	//In Basic mode Printing Preferences is not displayed as per the Language set
	
        CShIniFile cIniFile(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
      //  DWORD dwDefLangID = cIniFile.GetDefaultLangID();
        //CRegistryAPI regApi;
        //DWORD dwLangID = regApi.GetLangID(pPrinterName, dwDefLangID);
		SHORT dwLangID = -1;
		CShRegMdlTbl RegMdlTbl(ghInstance, pPrinterName, &cIniFile);
		RegMdlTbl.GetLangID(&dwLangID);
	//<E>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla


        WCHAR pUserDLL[20] = {0};

        wcscpy_s(pUserDLL, L"USR");

        if (dwLangID > 0)
        {
            WCHAR LocaleID[10] = {0};
            _itow_s(dwLangID,LocaleID,16);
            wcscat_s(pUserDLL,LocaleID);
        }
        wcscat_s(pUserDLL,L".dll");

        TCHAR szPath[_MAX_PATH] = {0};
        GetProjectFileName(szPath,pUserDLL);
        m_hStringResourceHandle = ::LoadLibrary(szPath);
    }
    //<E>Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    return m_hStringResourceHandle ;
}
HMODULE COemUI2::LoadBitmapResource(WCHAR* /*pPrinterName*/)
{
    //<S><A>Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    //Putting NULL check to ensure that the dlls are not loaded second time if it is already loaded.
    if(NULL == m_hBitmapResourceHandle)
    {
        LPTSTR pBMPDLL =L"UBR.DLL";
        WCHAR szPath[_MAX_PATH] = {};
        GetProjectFileName(szPath, pBMPDLL);
        m_hBitmapResourceHandle = ::LoadLibrary(szPath);
    }
    //<E>Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    return m_hBitmapResourceHandle ;
}
HMODULE COemUI2::LoadDialogResource(WCHAR* /*pPrinterName*/)
{
    //<S><A>Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    //Putting NULL check to ensure that the dlls are not loaded second time if it is already loaded.
	//<S><C>To Fix Redmine Bug #4124,21.02.2022,SSDI:Chanchal Singla
	//Printing Preferences window size displays small when compared to UD3 in Japanese OS
	//To Load Separate Resouce Dialog for Japanese OS
	UINT			SysLCID = 0;
	LPTSTR pBMPDLL = NULL;
    if(NULL == m_hDialogResourceHandle)
    {
		SysLCID = GetSystemDefaultLCID();
		if (SysLCID == JPN_LCID)
		{
			pBMPDLL = JPN_URB_RESOURCE;
		}
		else
		{
			 pBMPDLL = UR_RESOURCE;
		}
        TCHAR szPath[_MAX_PATH] = {0};
        GetProjectFileName(szPath, pBMPDLL);
        m_hDialogResourceHandle = ::LoadLibrary(szPath);
    }
	//<E>To Fix Redmine Bug #4124,21.02.2022,SSDI:Chanchal Singla
    //<E>Redmine #930 Un-installation: Unable to remove the driver package in Print Server Properties without restarting spooler.: 2015.02.19, SSDI:Sonali
    return m_hDialogResourceHandle ;
}

HRESULT COemUI2::StoreHelpFilePathAndID(WCHAR* pPrinterName)
{
	m_pHelpID = new DWORD[SCUI_HELPID_COUNT * 2];
	if(m_pHelpID == NULL)
		return E_FAIL;
	::memset(m_pHelpID, 0x00, (SCUI_HELPID_COUNT * 2) * sizeof(DWORD));
	if(FALSE == LoadHelpFilePathAndID(m_pHelpFilePath,m_pHelpID, pPrinterName))
		return E_FAIL;
	return S_OK;
}

// <S><A> Implemented Job Control Dialog UI,2020.12.10, SSDI:Chandrashekar .V
//=============================================================================
// function
//      DisplayJobHandlingDialog
//
// return value
//     NILL
//
// outline
//      DisplayJobHandling Dialog
//=============================================================================
VOID COemUI2::DisplayJobHandlingDialog(HWND hWnd, HANDLE hPrinter, PDEVMODEW pOemPublicDevMode, OEMDEV	*pOemPrivateDevMode, LPBYTE pJob, DWORD uJobID, WCHAR *pPrinterName)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	INT_PTR DlgRet = IDCANCEL;
	CPrintCoreHelper cprintcorehelper(m_pCoreHelperUni);
	m_pFeatureInfoList = cprintcorehelper.RetrieveFeaturesAndOptions();

	CGPDData gpddata(m_pCoreHelperUni, TRUE);
	m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();
	m_ResourceIDInfoMap = gpddata.GetResourceIDInfoMap();
	m_hBitmapResourceHandle=LoadBitmapResource(pPrinterName);
	CShowMessage* wObjCShowMessage;
	wObjCShowMessage = new CShowMessage(hWnd, m_hStringResourceHandle);
	if (NULL == wObjCShowMessage)
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateMessageClass -> NULL_POINTER(m_ObjCShowMessage)\n");
	}
	CShDlgJobHand01* pJobHandlingdialog = new CShDlgJobHand01(IDD_DLG_JOBHAND_01_01,m_hStringResourceHandle, m_hBitmapResourceHandle,
		m_hDialogResourceHandle, m_pFeatureInfoList, m_DefDMInfo, &m_ResourceIDInfoMap,hPrinter,
		hWnd, pPrinterName, pOemPublicDevMode, pOemPrivateDevMode, wObjCShowMessage);
	
	if (NULL == pJobHandlingdialog)
		return;
	pJobHandlingdialog->SetPrinterName(pPrinterName);
	
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_JOBHAND_01_01),
		hWnd, (DLGPROC)pJobHandlingdialog->WindowMapProc, (LPARAM)pJobHandlingdialog);

	if (IDOK == DlgRet)
	{
		
		//TBD:: To add Logic for JobHandling Dialog
		JOBCONTROL	*pJobCtrl = &pOemPrivateDevMode->scpi.ext.JobCtrl;
		
		if (NULL != pJobCtrl)
		{
			DWORD dwParameter = (sizeof(JOBCONTROL) / 3 + 1) * 4 + 1 + sizeof(WCHAR);

			BYTE *pParameter = new BYTE[dwParameter];
			SecureZeroMemory(pParameter, dwParameter);
			CHelper::eBase64((BYTE*)pJobCtrl, sizeof(JOBCONTROL), pParameter, dwParameter);

			((JOB_INFO_2*)pJob)->pParameters = (LPWSTR)pParameter;
			((JOB_INFO_2*)pJob)->Position = JOB_POSITION_UNSPECIFIED;
			SetJob(hPrinter, uJobID, 2, pJob, 0);

			delete[] pParameter;
			pParameter = NULL;
		}
	}
	if (NULL != wObjCShowMessage)
	{
		delete wObjCShowMessage;
		wObjCShowMessage = NULL;
	}
	if (pJobHandlingdialog != NULL)
	{
		delete pJobHandlingdialog;
		pJobHandlingdialog = NULL;
	}
}

// <E> Implemented Job Control Dialog UI,2020.12.10, SSDI:Chandrashekar .V
//=============================================================================
// function
//      DisplayUserAuthenticatonDialog
//
// return value
//     NILL
//
// outline
//      Display UserAuthenticaton Dialog
//=============================================================================
VOID COemUI2::DisplayUserAuthenticatonDialog(HWND hWnd, HANDLE hPrinter, PDEVMODEW pOemPublicDevMode, OEMDEV	*pOemPrivateDevMode, LPBYTE pJob, DWORD uJobID, WCHAR *pPrinterName)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	INT_PTR DlgRet = IDCANCEL;
	CPrintCoreHelper cprintcorehelper(m_pCoreHelperUni);
	m_pFeatureInfoList = cprintcorehelper.RetrieveFeaturesAndOptions();

	CGPDData gpddata(m_pCoreHelperUni, TRUE);
	m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();
	m_ResourceIDInfoMap = gpddata.GetResourceIDInfoMap();
	m_hBitmapResourceHandle = LoadBitmapResource(pPrinterName);
	CShowMessage* wObjCShowMessage;
	wObjCShowMessage = new CShowMessage(hWnd, m_hStringResourceHandle);
	if (NULL == wObjCShowMessage)
	{
		slog.putLog("COemUI2::DisplayUserAuthenticatonDialog -> NULL_POINTER(wObjCShowMessage)\n");
	}
	CShDlgUserAuth* pAuthDialog = new CShDlgUserAuth(IDD_DLG_USER_AUTHENTICATION_2, m_hStringResourceHandle, m_hBitmapResourceHandle,
		m_hDialogResourceHandle, m_pFeatureInfoList, m_DefDMInfo, &m_ResourceIDInfoMap, hPrinter,
		hWnd, pPrinterName, pOemPublicDevMode, pOemPrivateDevMode, wObjCShowMessage);

	if (NULL == pAuthDialog)
		return;
	pAuthDialog->SetPrinterName(pPrinterName);

	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_USER_AUTHENTICATION_2),
		hWnd, (DLGPROC)pAuthDialog->WindowMapProc, (LPARAM)pAuthDialog);

	if (IDOK == DlgRet)
	{
		JOBCONTROL	*pJobCtrl = &pOemPrivateDevMode->scpi.ext.JobCtrl;

		if (NULL != pJobCtrl)
		{
			DWORD dwParameter = (sizeof(JOBCONTROL) / 3 + 1) * 4 + 1 + sizeof(WCHAR);

			BYTE *pParameter = new BYTE[dwParameter];
			SecureZeroMemory(pParameter, dwParameter);
			CHelper::eBase64((BYTE*)pJobCtrl, sizeof(JOBCONTROL), pParameter, dwParameter);

			((JOB_INFO_2*)pJob)->pParameters = (LPWSTR)pParameter;
			((JOB_INFO_2*)pJob)->Position = JOB_POSITION_UNSPECIFIED;
			SetJob(hPrinter, uJobID, 2, pJob, 0);

			delete[] pParameter;
			pParameter = NULL;
		}
	}
	if (NULL != wObjCShowMessage)
	{
		delete wObjCShowMessage;
		wObjCShowMessage = NULL;
	}
	if (pAuthDialog != NULL)
	{
		delete pAuthDialog;
		pAuthDialog = NULL;
	}
}
VOID COemUI2::ShowJobHandlingDialogAndUpdateTheJobData(HWND hWnd,HANDLE hPrinter,LPSCDM pScdm,LPBYTE pJob,DWORD uJobID)
{
	CJobHandlingDialog *pJobHandlingdialog = new CJobHandlingDialog(IDD_TAB_JOBHAND_0,m_hStringResourceHandle,&pScdm->ext.JobCtrl,hPrinter);
	if(NULL == pJobHandlingdialog)
		return ;
	pJobHandlingdialog->SetPrinterName(pScdm->pub.dmDeviceName);
	INT_PTR DlgRet = IDCANCEL;

	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_TAB_JOBHAND_0),
		hWnd, (DLGPROC)pJobHandlingdialog->WindowMapProc, (LPARAM)pJobHandlingdialog);

	if(IDOK == DlgRet)
	{
		//TBD:: To add Logic for JobHandling Dialog
		JOBCONTROL	*pJobCtrl = pJobHandlingdialog->GetJobControlData();
		if(NULL!= pJobCtrl)
		{
			DWORD dwParameter = (sizeof(JOBCONTROL)/3+1)*4+1+sizeof(WCHAR);

			BYTE *pParameter = new BYTE[dwParameter];
			SecureZeroMemory(pParameter,dwParameter);
			CHelper::eBase64((BYTE*)pJobCtrl,sizeof(JOBCONTROL),pParameter,dwParameter);

			((JOB_INFO_2*)pJob)->pParameters = (LPWSTR)pParameter;
			((JOB_INFO_2*)pJob)->Position = JOB_POSITION_UNSPECIFIED;
			SetJob(hPrinter,uJobID,2,pJob,0);

			delete [] pParameter;
			pParameter = NULL;
		}
	}

	delete pJobHandlingdialog;
	pJobHandlingdialog = NULL;

}


OEMDEV *COemUI2::GetUniDrvPrivateDevmode(LPDEVMODE pDevMode)
{
	if(pDevMode)
	{
		DWORD wSize = GET_UNIDRV_PRIVATE_DEVMODE_SIZE(pDevMode);
		if(pDevMode->dmDriverExtra > wSize)
		{
			BYTE * pCustDevmode = (PBYTE)(pDevMode) + (pDevMode)->dmSize + wSize;
			OEMDEV *pOemDev =(OEMDEV*)(pCustDevmode);
			return pOemDev;
		}
	}
	return NULL;
}
DWORD COemUI2::GetDefaultLangID()
{
    TCHAR szCommonDatFilePath[_MAX_PATH] = {0};
    GetProjectFileName(szCommonDatFilePath,L"Common.DAT");

    CShIniFile cshLangFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
    DWORD dwDefaultLangID = (DWORD)cshLangFile.GetDefaultLangID();
    return dwDefaultLangID;
}

HWND COemUI2::CreateDummyWindow()
{
	HWND		hWnd	= NULL;

	WNDCLASSEX	wcex;
	::memset(&wcex, 0x00, sizeof(WNDCLASSEX));

	wcex.cbSize			= sizeof(wcex);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= COemUI2::DummyWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= ghInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= CLASSNAME_UD2LDUMMYWND;
	wcex.hIconSm		= NULL;

	if(::RegisterClassEx(&wcex) != 0)
	{
		hWnd = ::CreateWindowEx(WS_EX_TOOLWINDOW,
								CLASSNAME_UD2LDUMMYWND,
								NULL,
								WS_POPUP | WS_VISIBLE,
								-100, -100, 10, 10,	
								NULL,
								NULL,
								ghInstance,
								NULL);
	}

	if(hWnd != NULL)
	{
		::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	return hWnd;
}

LRESULT CALLBACK COemUI2::DummyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL COemUI2::DestroyDummyWindow(HWND hWnd)
{
	BOOL	blRet = FALSE;

	if(m_hWnd_DummyWindow != NULL)
		//<S><C> to fix Coverity issue 90440, Harika 20201214
		//blRet = ::DestroyWindow(m_hWnd_DummyWindow);
		::DestroyWindow(m_hWnd_DummyWindow);
		//<E> to fix Coverity issue 90440, Harika 20201214
	blRet = UnregisterClass(CLASSNAME_UD2LDUMMYWND,ghInstance);

	return blRet;
}

BOOL COemUI2::IsThunkProcess(void)
{
	BOOL	blRet = FALSE;

    WCHAR	szProcFName[_MAX_PATH] = {};
    GetModuleFileName(NULL, szProcFName, _countof(szProcFName));

	if(0 < wcslen(szProcFName))
	{
		WCHAR* pSep = ::wcsrchr(szProcFName, '\\');
        if(NULL != pSep)
        {
            if(0 == ::lstrcmpi(pSep + 1, L"splwow64.exe"))
            {
                blRet = TRUE;
            }
        }
	}

	return blRet;
}
//<S><D>since the calling is in commented code, the definition is also Commented to fix the coverity issue 90569. if required, need to uncomment this. Harika, 20201125
#if 0
VOID COemUI2::GetSupportedPaperSource(PVOID& pOutput,DWORD& dwNumberSupportedPaperSources,PWSTR pDeviceName)
{
		if(0 == m_ResourceIDInfoMap.size())
		{
			// Load GDL snap shot only when Resource ID Info map is not constructed.
			CGPDData gpddata(m_pCoreHelperUni,TRUE);
			m_ResourceIDInfoMap = gpddata.GetResourceIDInfoMap();

		}
		DWORD dwOptions = 0;
		PCSTR *ppszOptions = NULL;
		if(NULL == m_pCoreHelperUni)
		{
			return ;
		}
		m_pCoreHelperUni->EnumOptions("InputBin",&ppszOptions,&dwOptions);
        auto iteratorInputBinFeature = m_ResourceIDInfoMap.find("InputBin");

        LoadStringResource(pDeviceName);

        WCHAR *pszItem = NULL ;
		if(iteratorInputBinFeature != m_ResourceIDInfoMap.end())
		{
			if(NULL != pOutput)
			{
				pszItem = (WCHAR*)pOutput;
				SecureZeroMemory(pszItem,SHDEVCAP_BINNAMES_SIZE*iteratorInputBinFeature->second.uOptionResourceID.size()*sizeof(WCHAR));
			}
			
			for(DWORD dwOptionIndex = 0; dwOptionIndex < dwOptions; dwOptionIndex++)
			{
				BOOL bSupportInputBinOption = TRUE;
				auto iteratorInputBinOption = iteratorInputBinFeature->second.uOptionResourceID.find(ppszOptions[dwOptionIndex]);
				if(iteratorInputBinOption != iteratorInputBinFeature->second.uOptionResourceID.end())
				{
					for( auto iConstraintInputBinOption :iteratorInputBinOption->second.second.ConstraintFeatureOptionMessageIDList)
					{
						if(iConstraintInputBinOption.uNumberOfConstraints == iConstraintInputBinOption.ConstraintFeatureOptionIDList.size())
						{
							bSupportInputBinOption = FALSE ;
							break;
						}
					}
				}
				if(TRUE == bSupportInputBinOption)
				{
					if(NULL != pOutput)
					{
						if (LoadString(m_hStringResourceHandle, iteratorInputBinOption->second.first, pszItem, sizeof(WCHAR)*SHDEVCAP_BINNAMES_SIZE) > 0)
						{
							pszItem += SHDEVCAP_BINNAMES_SIZE;
							
						}
					}
					dwNumberSupportedPaperSources++;
				}
			}
		}
}
#endif
//<E>since the calling is in commented code, the definition is also Commented to fix the coverity issue 90569. if required, need to uncomment this. Harika, 20201125

BOOL COemUI2::SetLoggerRegInfo()
{
    MFPPrinterUI_Logger slog(__FUNCTION__"\n");


    HKEY hk; 
    DWORD dwLogFileFlag = 0, dwLogFileMaxSizeDefault = 1000, dwDisp; 

    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, MFPCOMM_LOG_INFO_REG_PATH, 
        0, NULL, REG_OPTION_NON_VOLATILE,
        KEY_WRITE, NULL, &hk, &dwDisp)) 
    {
        return FALSE;
    }

    //Check if newely created then only
    //create and assign values to subkeys
    if(REG_CREATED_NEW_KEY == dwDisp)
    {
        WCHAR LogFileFullPath[MAX_PATH] = {};
        // Get the Windows directory.       
        WCHAR infoBuf[MAX_PATH] = {};
        if( !GetWindowsDirectory( infoBuf, MAX_PATH ) )
        {
            wcscpy_s(LogFileFullPath,MAX_PATH,MFPCOMM_LOG_FILE_DEFAULT_LOGFILE_PATH);
            slog.putLog("GetWindowsDirectory Failed\n"); 
        }
        else
        {
            //Retrive the drive info
            WCHAR LogFileDrive[4] = {};
            wcsncat_s(LogFileDrive,infoBuf,3);
            LogFileDrive[3] = L'\0';

            //Concatenate the drive with the default logfilename
            wcscpy_s(LogFileFullPath,MAX_PATH,LogFileDrive);
            wcscpy_s(LogFileFullPath,MAX_PATH,MFPCOMM_LOG_FILE_DEFAULT_FILENAME);
        }

        if (RegSetValueEx(hk,								// subkey handle 
            MFPCOMM_LOG_FILE_NAME_REG_KEY,				    // value name 
            0,												// must be zero 
            REG_SZ,											// value type 
            (LPBYTE) LogFileFullPath,					   // pointer to value data 
            (DWORD) (wcslen(LogFileFullPath)+1)*sizeof(WCHAR))) // data size
        {
            RegCloseKey(hk); 
            return FALSE;
        }

        if (RegSetValueEx(hk,					    // subkey handle 
            MFPCOMM_LOG_GENERATE_FLAG_REG_KEY,		// value name 
            0,									    // must be zero 
            REG_DWORD,							    // value type 
            (LPBYTE) &dwLogFileFlag,				// pointer to value data 
            sizeof(DWORD)))							// length of value data 
        {
            RegCloseKey(hk); 
            return FALSE;
        }

        if (RegSetValueEx(hk,						 // subkey handle 
            MFPCOMM_LOG_FILE_MAXSIZE_REG_KEY,		 // value name 
            0,										 // must be zero 
            REG_DWORD,								 // value type 
            (LPBYTE) &dwLogFileMaxSizeDefault,		 // pointer to value data 
            sizeof(DWORD)))							 // length of value data 
        {
            RegCloseKey(hk); 
            return FALSE;
        }
    }


    RegCloseKey(hk); 
    return TRUE;
}
void COemUI2::FreeMemory(void)
{
	if(NULL == m_pFeatureInfoList)
	{
		return;
	}
	if(NULL != m_pFeatureInfoList->pFeatureOptionPair)
	{
		delete [] m_pFeatureInfoList->pFeatureOptionPair;
		m_pFeatureInfoList->pFeatureOptionPair = NULL ;
	}
	if(NULL != m_pFeatureInfoList->pFeatureOptionsList)
	{
		delete [] m_pFeatureInfoList->pFeatureOptionsList;
		m_pFeatureInfoList->pFeatureOptionsList = NULL ;
	}
    if(NULL != m_pFeatureInfoList)
	{
		delete m_pFeatureInfoList;
		m_pFeatureInfoList = NULL;
	}
}

VOID COemUI2::UpdateValuesForDefaultCustomPaper(POEMDEV pOemPrivateDevMode)
{
    CustomPaperInfoMap::const_iterator CustomPaperIterator = m_CustomPaperInfoList.find("CUSTOMSIZE");
    if(m_CustomPaperInfoList.end() != CustomPaperIterator)
    {
        DWORD dwminimumsizeWidth  = CustomPaperIterator->second.MinumPageSize.first;
        DWORD dwminimumsizeLength = CustomPaperIterator->second.MinumPageSize.second;

        long lMilliMinWidth = PixelToMilli(dwminimumsizeWidth,600,MM_LOMETRIC);
        long lMilliMinLength = PixelToMilli(dwminimumsizeLength,600,MM_LOMETRIC);

        pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (short)lMilliMinWidth;
        pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (short)lMilliMinLength ;

        pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = dwminimumsizeWidth;
        pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = dwminimumsizeLength;
    }
    wcscpy_s(pOemPrivateDevMode->scpi.pub.dmFormName,CCHFORMNAME,L"Default"/*m_pPrivateDevmode->scpi.pub.dmFormName*/);
    m_pFeatureInfoList->pFeatureOptionPair[FEATURE_CUSTOMPAPERSIZE].pszOption = "Default";
    m_pFeatureInfoList->pFeatureOptionsList[FEATURE_CUSTOMPAPERSIZE].uChangedOptionIndex = eFirstOption;
}

//<S><A> RedMine Ticket(#1636) Custom Paper value is not updated when updated through forms tab: 2014.11.21, SSDI:Sonali
//Updating custom form values again if in case the form valus are changed from Forms Tab.
VOID COemUI2::UpdateCustomForm(POEMDEV pOemPrivateDevMode,FORM_INFO_1* pFormInfo,BYTE bUnits)
{
    //<S><A> RedMine Ticket(#1636) Issue#2 2015.03.17, SSDI:Sonali
    //In case the user form has dimensions beyond allowable range, then resettting it to default values.
    LONG lWidth = pFormInfo->Size.cx;
    LONG lLength = pFormInfo->Size.cy;

    if((MIN_CUSTOM_SIZE_WIDTH > lWidth || MAX_CUSTOM_SIZE_WIDTH < lWidth) ||
        (MIN_CUSTOM_SIZE_LENGTH > lLength || MAX_CUSTOM_SIZE_LENGTH < lLength))
    {
        lWidth = MIN_CUSTOM_SIZE_WIDTH;        
        lLength = MIN_CUSTOM_SIZE_LENGTH;
        m_pFeatureInfoList->pFeatureOptionPair[FEATURE_CUSTOMPAPERSIZE].pszOption = "Default";
        m_pFeatureInfoList->pFeatureOptionsList[FEATURE_CUSTOMPAPERSIZE].uChangedOptionIndex = eFirstOption;
    }
    //<E> RedMine Ticket(#1636) Issue#2 2015.03.17, SSDI:Sonali

    //<S><C> RedMine Ticket(#1636) Issue#2 2015.03.17, SSDI:Sonali
    pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (short) lWidth / 100;
    pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (short) lLength / 100;
    //<E> RedMine Ticket(#1636) Issue#2 2015.03.17, SSDI:Sonali

    if(unit_inch == bUnits)
    {
        pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = MilliToPixel(pOemPrivateDevMode->scpi.ext.nDstWidthMilli , 600, MM_LOMETRIC);
        pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = MilliToPixel(pOemPrivateDevMode->scpi.ext.nDstHeightMilli , 600, MM_LOMETRIC);                    
    }
    pOemPrivateDevMode->scpi.pub.dmPaperLength = pOemPrivateDevMode->scpi.ext.nDstWidthMilli;
    pOemPrivateDevMode->scpi.pub.dmPaperWidth = pOemPrivateDevMode->scpi.ext.nDstHeightMilli;
}//<S><A> RedMine Ticket(#1636) Custom Paper value is not updated when updated through forms tab: 2014.11.21, SSDI:Sonali


HWND COemUI2::GetParentWnd( VOID )
{
	HWND	hWnd = NULL;

	if( m_bGetPhWnd == FALSE )
	{
		if( IsDesktopActive() == FALSE )
		{
			hWnd = ::GetDesktopWindow();
		}
		else
		{
			hWnd = ::GetFocus();
			if(hWnd == NULL){
				if( m_hWnd_DummyWindow == NULL ){
					m_hWnd_DummyWindow = CreateDummyWindow();
				}

				hWnd = m_hWnd_DummyWindow;
			}
		}
		m_hWnd_GetPhWnd = hWnd;
		m_bGetPhWnd = TRUE;
	}
	return( m_hWnd_GetPhWnd );
}

BOOL COemUI2::IsDesktopActive()
{
	TCHAR	ClassName[256];		
	HWND	hWnd;
	int		size;
	int		nMaxCount = 256;	

	hWnd = GetForegroundWindow();
	size = GetClassName( hWnd, (LPWSTR)&ClassName, nMaxCount );
	if( wcscmp( ClassName, L"Windows.UI.Core.CoreWindow" ) == 0 )
		return FALSE;
	else
		return TRUE;
}

//<S><A> Redmine #2066 Staple is not working when Flip on Short Edge is set on Metro app: 2015.03.23, SSDI:Sonali
/*++
Routine Name:

    COemUI2::UpdateBindingEdgeInfo

Routine Description:

    The routine updates Binding Edge information when printed from any application 
    without opening Printer UI.

Arguments:

    pDevMode - pointer to Public devmode
    pPrinterName - pointer to Printer Name

Return Value:

   void
   
--*/
VOID COemUI2::UpdateBindingEdgeInfo(PDEVMODEW pDevMode, WCHAR* pPrinterName)
{
    if (NULL == pDevMode)
    {
        return;
    }
    OEMDEV *pOemDev = GetUniDrvPrivateDevmode(pDevMode);
    if(NULL == pOemDev)
    {
        return;
    }
    
    BOOL bLeftOrRightBinding = TRUE;
	BOOL bTopBinding = TRUE ;
	if(
        ((DMDUP_VERTICAL == pOemDev->scpi.pub.dmDuplex) && (DMORIENT_PORTRAIT == pOemDev->scpi.pub.dmOrientation)) ||
		((DMDUP_HORIZONTAL == pOemDev->scpi.pub.dmDuplex) && (DMORIENT_LANDSCAPE == pOemDev->scpi.pub.dmOrientation))
	  )
	{
		bTopBinding = FALSE;
	}
	if(
		((DMDUP_VERTICAL == pOemDev->scpi.pub.dmDuplex) && (DMORIENT_LANDSCAPE == pOemDev->scpi.pub.dmOrientation)) ||
		((DMDUP_HORIZONTAL == pOemDev->scpi.pub.dmDuplex) && (DMORIENT_PORTRAIT == pOemDev->scpi.pub.dmOrientation))
	  )
	{
		bLeftOrRightBinding = FALSE;
	}
	if((FALSE == bLeftOrRightBinding) || (FALSE == bTopBinding))
	{
        BOOL bSwap = (pOemDev->scpi.ext.byNupX != pOemDev->scpi.ext.byNupY);
        if(bSwap)
        {
            bTopBinding = (FALSE == bTopBinding);
            bLeftOrRightBinding = (FALSE == bLeftOrRightBinding);
        }

		if((FALSE == bTopBinding) && (BINDEDGE_TOP == pOemDev->scpi.ext.byBindedge))
		{
			pOemDev->scpi.ext.byBindedge = BINDEDGE_LEFT ;
		}

		if((FALSE == bLeftOrRightBinding) && (BINDEDGE_TOP != pOemDev->scpi.ext.byBindedge))
		{
			pOemDev->scpi.ext.byBindedge = BINDEDGE_TOP ;
		}
	}
}
//<E> Redmine #2066 Staple is not working when Flip on Short Edge is set on Metro app: 2015.03.23, SSDI:Sonali


//<S><A> Redmine #1981 Windows 10: ìPCL XL errorî is displayed when ë1 Stapleí is set from metro app: 2015.03.26, SSDI:Sonali
/*++
Routine Name:

    COemUI2::UpdateDuplexInfo

Routine Description:

    The routine updates Duplex information when printed from any application 
    without opening Printer UI.

Arguments:

    pDevMode - pointer to Public devmode
    pPrinterName - pointer to Printer Name

Return Value:

   void
   
--*/
VOID COemUI2::UpdateDuplexInfo(PDEVMODEW pDevMode, WCHAR* pPrinterName)
{
    if (NULL == pDevMode)
    {
        return;
    }

    OEMDEV *pOemDev = GetUniDrvPrivateDevmode(pDevMode);
    if(NULL == pOemDev)
    {
        return;
    }

    if(PAMPH_NOT != pOemDev->scpi.ext.byPamph)
    { 
        pOemDev->scpi.pub.dmDuplex = DMDUP_VERTICAL;
        pDevMode->dmDuplex = DMDUP_VERTICAL;
    }
}
//<S><A>To Fix Redmine Bug #4363 issue 1 and 2,08.04.2022,SSDI:Chanchasl Singla
//PJL issued for Copy(Collate / Uncollate) does not display correct value
VOID COemUI2::UpdateCollateInfo(PDEVMODEW pDevMode, WCHAR* pPrinterName)
{
	if (NULL == pDevMode)
	{
		return;
	}

	OEMDEV *pOemDev = GetUniDrvPrivateDevmode(pDevMode);
	if (NULL == pOemDev)
	{
		return;
	}

	if (DMCOLLATE_TRUE == pOemDev->scpi.pub.dmCollate)
	{
		pDevMode->dmCollate = DMCOLLATE_TRUE;
	}
}
//<E>To Fix Redmine Bug #4363 issue 1 and 2,08.04.2022,SSDI:Chanchasl Singla
//<E> Redmine #1981 Windows 10: ìPCL XL errorî is displayed when ë1 Stapleí is set from metro app: 2015.03.26, SSDI:Sonali
//<S><A> To update Devmode based on device prop page values, Harika, 20201026
VOID COemUI2::UpdateDevDMode(PDEVMODEW pDevMode, WCHAR* pPrinterName)
{
	if (NULL == pDevMode)
	{
		return;
	}
	
	OEMDEV *pOemDev = GetUniDrvPrivateDevmode(pDevMode);
	if (NULL == pOemDev)
	{
		return;
	}
	CRegistryAPI regApi;
	regApi.ReadData(pDevMode->dmDeviceName, &pOemDev->scpi.dev);
	//CShLimitCom shLimitCom = new(pOemDev);
	if (pOemDev && (DM_VDM_NON_PRITNRLEASE == (pOemDev->scpi.dev.byFieldVDM & DM_VDM_NON_PRITNRLEASE)))
	{
		pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_PRINTSHARE;
	}
	else {
		if (pOemDev && (DM_PP_PRINTRELEASE == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_PRINTRELEASE)))
		{
			pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_PRINTSHARE;
		}
	}
	if (pOemDev && (DM_PP_RETENTION == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_RETENTION)))
	{
		pOemDev->scpi.ext.JobCtrl.byRetention = retention_hold_before;
		pOemDev->scpi.ext.JobCtrl.byDocumentFiling = docfile_main;
	}

	//<S><A> DevMode update based on Windows login name as login name, Harika, 20201028
	if (pOemDev && (DM_PP_WINLOGIN_AS_LOGINN == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN)))
	{
		// --- DM_ALWAYSUSE_ACNUM
		BOOL wUseAcNum = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM ? bool_true : bool_false;

		// --- DM_USE_LOGINNAME
		BOOL wLoginName = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME ? bool_true : bool_false;

		// --- DM_USE_SINGLESIGNON
		BOOL wLoginNameSSO = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_SINGLESIGNON ? bool_true : bool_false;

		// --- DM_USE_LOGINPASS
		BOOL wLoginPass = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS ? bool_true : bool_false;
		DWORD dwSize = sizeof(pOemDev->scpi.ext.JobCtrl.szLoginName);
		//Fixed Coverity #90896 ,20200112,SSDI;Chanchal Singla
		WCHAR		szLoginName[LOGIN_NAME_LENGTH * 2];
		::GetUserName(szLoginName, &dwSize);
		wcscpy_s(pOemDev->scpi.ext.JobCtrl.szLoginName, LOGIN_NAME_LENGTH, szLoginName);
		if ((wLoginName == bool_true) && (wLoginPass == bool_true)
			&& (wUseAcNum == bool_false))
		{
			// --- DM_ALWAYSUSE_ACNUM , wUseAcNum = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
			// --- DM_USE_LOGINNAME ,wLoginName = bool_true
			pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
			// --- DM_USE_LOGINPASS, wLoginPass = bool_true
			pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINPASS;
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_SINGLESIGNON;
		}
		else
		{
			// --- DM_ALWAYSUSE_ACNUM, wUseAcNum = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
			// --- DM_USE_LOGINNAME ,wLoginName = bool_true
			pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
			// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_SINGLESIGNON;
		}

	}
	if (pOemDev && (DM_PP_USERAUTH == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_USERAUTH)))
	{
		// --- DM_ALWAYSUSE_ACNUM, wUseAcNum = bool_false
		pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
		// --- DM_USE_LOGINNAME ,wLoginName = bool_false
		pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;
		// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
		pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
		pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_SINGLESIGNON;
		if (pOemDev && (DM_PP_WINLOGIN_AS_LOGINN == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN)))
		{
			// --- DM_ALWAYSUSE_ACNUM, wUseAcNum = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
			// --- DM_USE_LOGINNAME ,wLoginName = bool_true
			pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
			// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_SINGLESIGNON;
		}
	}
	//<E> DevMode update based on Windows login name as login name, Harika, 20201028
	if (pOemDev && (DM_PP_LOGINN_AS_USERN == (pOemDev->scpi.dev.dwFieldVDM & DM_PP_LOGINN_AS_USERN)))
	{
		//<S><C> Fix for SOL2 feedback issue #90. Harika, 221116
		if (::wcslen(pOemDev->scpi.ext.JobCtrl.szLoginName) > 0)
		{
			WCHAR* pTemp = pOemDev->scpi.ext.JobCtrl.szLoginName;
			memcpy(pOemDev->scpi.ext.JobCtrl.szUsername, pTemp, USER_NAME_LENGTH * sizeof(WCHAR));
		}
		else
		{
			DWORD dwSize = sizeof(pOemDev->scpi.ext.JobCtrl.szLoginName);
			WCHAR		szLoginName[LOGIN_NAME_LENGTH * 2];
			::GetUserName(szLoginName, &dwSize);
			wcscpy_s(pOemDev->scpi.ext.JobCtrl.szLoginName, LOGIN_NAME_LENGTH, szLoginName);
		}
		//<E> Fix for SOL2 feedback issue #90. Harika, 221116
	}

	if (pOemDev && (DM_SINGLESIGNON == (pOemDev->scpi.dev.dwFieldVDM & DM_SINGLESIGNON)))
	{
		// --- DM_ALWAYSUSE_ACNUM
		BOOL wUseAcNum = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM ? bool_true : bool_false;
		// --- DM_USE_LOGINNAME
		BOOL wLoginName = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME ? bool_true : bool_false;
		// --- DM_USE_LOGINPASS
		BOOL wLoginPass = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS ? bool_true : bool_false;
		// --- DM_USE_SINGLESIGNON
		BOOL wLoginNameSSO = pOemDev->scpi.ext.JobCtrl.wFieldJC & DM_USE_SINGLESIGNON ? bool_true : bool_false;
		DWORD dwSize = sizeof(pOemDev->scpi.ext.JobCtrl.szLoginName);
		//Fixed Coverity #90896 ,20200112,SSDI;Chanchal Singla
		WCHAR		szLoginName[LOGIN_NAME_LENGTH * 2];
		::GetUserName(szLoginName, &dwSize);
		wcscpy_s(pOemDev->scpi.ext.JobCtrl.szLoginName, LOGIN_NAME_LENGTH, szLoginName);
		if ((wLoginName == bool_false) && (wLoginPass == bool_false)
			&& (wUseAcNum == bool_false) && (wLoginNameSSO == bool_true))
		{
			// --- DM_ALWAYSUSE_ACNUM , wUseAcNum = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
			// --- DM_USE_LOGINNAME ,wLoginName = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;
			// --- DM_USE_LOGINPASS, wLoginPass = bool_false
			pOemDev->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
			// --- DM_USE_SINGLESIGNON, wLoginNameSSO = bool_true
			pOemDev->scpi.ext.JobCtrl.wFieldJC |= DM_USE_SINGLESIGNON;
		}
	}
	//<S><D>Deleted the code To Fix Bug #2911 and Bug #4066,20021.07.14 SSDI: Chanchal singla
	//	Document Filing with PIN Code fails to work. and PIN Compatibility feature fails to work
	//<S><A> To support PIN Compatibility	-	2020.10.29, SSDI:Goutham
	//if (pOemDev && (DM_VDM_PIN_CLEAN_TEXT == (pOemDev->scpi.dev.dwFieldVDM & DM_VDM_PIN_CLEAN_TEXT)))
	//{
	//	BYTE* pTemp = pOemDev->scpi.ext.JobCtrl.byRetentionPassword;
	//	BYTE pPassWord[RETENTION_PASSWORD_LENGTH + 1] = { 0 };
	//	memcpy(pPassWord, pTemp, RETENTION_PASSWORD_LENGTH);
	//	CHAR pDecodePassWord[RETENTION_PASSWORD_LENGTH + 1] = { 0 };
	//	Decode_AES(pDecodePassWord, (LPSTR)pPassWord);
	//	::lstrcpyn((LPWSTR)pOemDev->scpi.ext.JobCtrl.byRetentionPassword, (LPCWSTR)pDecodePassWord, wcslen((WCHAR*)pDecodePassWord) * sizeof(WCHAR));
	//	
	//}
	//else
	//{
	//	BYTE* pTemp = pOemDev->scpi.ext.JobCtrl.byRetentionPassword;
	//	BYTE pPassWord[RETENTION_PASSWORD_LENGTH + 1] = { 0 };
	//	memcpy(pPassWord, pTemp, RETENTION_PASSWORD_LENGTH);
	//	CHAR pDecodePassWord[RETENTION_PASSWORD_LENGTH + 1] = { 0 };
	//	Encode_AES(pDecodePassWord, (LPSTR)pPassWord);
	//	CHAR dat[128] = { 0 };
	//	CHAR temp[128] = { 0 };

	//	strcpy_s(temp, 128, pDecodePassWord);
	//	//strcat_s(temp, 128, m_pSpoolTime);
	//	CHelper::MAKEHASH(dat, HASH_MD5, NULL, temp);
	//	//<S><D>To Fix SSDI Bug #2913. If required, need to check for Retention password. Harika 20210113
	//	//::lstrcpyn((LPWSTR)pOemDev->scpi.ext.JobCtrl.byRetentionPassword, (LPCWSTR)dat, wcslen((WCHAR*)dat) * sizeof(WCHAR));
	//	//<E>To Fix SSDI Bug #2913. If required, need to check for Retention password. Harika 20210113
	//}
	//<S><E> To support PIN Compatibility	-	2020.10.29, SSDI:Goutham
	//<E>Deleted the code To Fix Bug #2911 and Bug #4066,20021.07.14 SSDI: Chanchal singla
	//<S><A> To Fix redmine #2850 and  To Support of MaxPaper Size,20201102,SSDI:Chanchal Singla
	//Maximum Paper Size: MFP asks to load A3 even though output size drop down does not contains A3
//<S><D>Deleted the code To Fix Bug #3846, Harsh 20220426
/*	if (pOemDev->scpi.dev.byMaxPaperSize == paper_121)
	{
		CGPDData gpddata(m_pCoreHelperUni, TRUE);
		m_PaperInfoList = gpddata.GetPaperInfoList();
		short nFTPPaperSize = pOemDev->scpi.ext.nFTPPaperSize;
		PaperInfoMap::const_iterator it;
		if ((nFTPPaperSize == DMPAPER_A3) /*A3*/
//			|| (nFTPPaperSize == DMPAPER_B4)/*B4*/
//			|| (nFTPPaperSize == DMPAPER_TABLOID)/*TABLOID -- LEDGER*/
//			|| (nFTPPaperSize == DMPAPER_JENV_KAKU2))/*JENUKAKU2 -- 240x332*/
/*		{
			pOemDev->scpi.ext.dwFieldEDM1 |= DM_FITTOPAGE;
			it = (GetPaperSizeDef() == DMPAPER_A4) ? m_PaperInfoList.find("A4") : m_PaperInfoList.find("LETTER");
			if (it != m_PaperInfoList.end())
			{
				pOemDev->scpi.ext.nFTPPaperSize = (SHORT)it->second.PaperSize;
				pOemDev->scpi.ext.dwDstWidthPixel = 2 * pOemDev->scpi.doc.nLeftMarginPixel + it->second.PageImageaDimensions.first;
				pOemDev->scpi.ext.dwDstHeightPixel = 2 * pOemDev->scpi.doc.nTopMarginPixel + it->second.PageImageaDimensions.second;
				pOemDev->scpi.ext.nDstWidthMilli = (SHORT)it->second.PaperWidth;
				pOemDev->scpi.ext.nDstHeightMilli = (SHORT)it->second.PaperLength;
			}
		}
	}*/
//<S><E>Deleted the code To Fix Bug #3846, Harsh 20220426
//<S><A>To Fix the HLK Bug #3846, Harsh 20220426
	if (pOemDev->scpi.dev.byMaxPaperSize == paper_121)
	{
		PaperInformationMap paperinfomap = InitializeMap();
		PaperInformationMap::const_iterator paperinfoiterator;

		short nFTPPaperSize = pOemDev->scpi.ext.nFTPPaperSize;
		PaperInfoMap::const_iterator it;
		if ((nFTPPaperSize == DMPAPER_A3) /*A3*/
			|| (nFTPPaperSize == DMPAPER_B4)/*B4*/
			|| (nFTPPaperSize == DMPAPER_TABLOID)/*TABLOID -- LEDGER*/
			|| (nFTPPaperSize == DMPAPER_JENV_KAKU2))/*JENUKAKU2 -- 240x332*/
		{
			SHORT		nDstWidthMilli = 0;		// 
			SHORT		nDstHeightMilli = 0;	// 
			pOemDev->scpi.ext.dwFieldEDM1 |= DM_FITTOPAGE;

			if ((GetPaperSizeDef() == DMPAPER_A4))
			{
				nFTPPaperSize = DMPAPER_A4;
				nDstWidthMilli = (SHORT)PaperSizeArray[1][1];
				nDstHeightMilli = (SHORT)PaperSizeArray[1][2];
			}
			else
			{
				nFTPPaperSize = DMPAPER_LETTER;
				nDstWidthMilli = (SHORT)PaperSizeArray[6][1];
				nDstHeightMilli = (SHORT)PaperSizeArray[6][2];
			}

			paperinfoiterator = paperinfomap.find(pOemDev->scpi.ext.nFTPPaperSize);

			if (paperinfoiterator != paperinfomap.end())
			{
				pOemDev->scpi.ext.nFTPPaperSize = nFTPPaperSize;
				pOemDev->scpi.doc.nLeftMarginPixel = paperinfoiterator->second.PageOrigin.first;
				pOemDev->scpi.doc.nTopMarginPixel = paperinfoiterator->second.PageOrigin.second;
				pOemDev->scpi.ext.dwDstWidthPixel = 2 * pOemDev->scpi.doc.nLeftMarginPixel + paperinfoiterator->second.PageImageaDimensions.first;
				pOemDev->scpi.ext.dwDstHeightPixel = 2 * pOemDev->scpi.doc.nTopMarginPixel + paperinfoiterator->second.PageImageaDimensions.second;
				pOemDev->scpi.ext.nDstWidthMilli = (SHORT)nDstWidthMilli;
				pOemDev->scpi.ext.nDstHeightMilli = (SHORT)nDstHeightMilli;
			}
		}
	}
//<S><E>To Fix the HLK Bug #3846, Harsh 20220426
	//<E> To Fix redmine #2850 and  To Support of MaxPaper Size,20201102,SSDI:Chanchal Singla
//<S><A>To fix PJL issue for Auto color mode  2020.11.11 Goutham
/*	if (pOemDev && (DM_VDM_COLORMODE_AUTO_DISABLE == (pOemDev->scpi.dev.dwFieldVDM & DM_VDM_COLORMODE_AUTO_DISABLE)))
	{

		if (pOemDev->scpi.ext.ColData.byColMode == colmode_auto)
		{
			pOemDev->scpi.ext.ColData.byColMode = colmode_color;
		}

		else

			if (DMCOLOR_MONOCHROME == pDevMode->dmColor)
			{
				pOemDev->scpi.ext.ColData.byColMode = colmode_mono;
			}

	}

	else
	{

		if (DMCOLOR_MONOCHROME == pDevMode->dmColor)

		{
			pOemDev->scpi.ext.ColData.byColMode = colmode_mono;
		}

	}*/
//<S><E>To fix PJL issue for Auto color mode  2020.11.11 Goutham
}

//<E> To update Devmode based on device prop page values, Harika, 20201026

// <S><A> To Fix redmine#2810, 2021.09.06, SSDI:Uday
/*++
Routine Name:

	COemUI2::UpdateJobStaple

Routine Description:

	The routine updates JobStaple information

Arguments:

	pDevMode - pointer to Public devmode
	pPrinterName - pointer to Printer Name

Return Value:

   void

--*/
VOID COemUI2::UpdateJobStaple(PDEVMODEW pDevMode, WCHAR* pPrinterName)
{
	CRegistryAPI cRegApi(ghInstance);

	if (NULL == pDevMode)
	{
		return;
	}
	OEMDEV *pOemDev = GetUniDrvPrivateDevmode(pDevMode);
	if (NULL == pOemDev)
	{
		return;
	}

	cRegApi.ReadData(m_pPrinterName, &pOemDev->scpi.dev);

	CShLimit limit(NULL, NULL);
	PROPSTATE pps;
	SecureZeroMemory(&pps,sizeof(PROPSTATE));
	// -- Staple
	pps.wStaple = pOemDev->scpi.ext.byStaple;
	//ByStaplemodule
	if (pOemDev->scpi.dev.byFieldVDMStaple & DM_VDMS_SADDLE)
	{
		pps.wStapleOption = stapleopt_3;
	}
	else if (pOemDev->scpi.dev.byFieldVDMStaple & DM_VDMS_2STAPLE)
	{
		pps.wStapleOption = stapleopt_2;
	}
	else if (pOemDev->scpi.dev.byFieldVDMStaple & DM_VDMS_1STAPLE)
	{
		pps.wStapleOption = stapleopt_1;
	}
	else
	{
		pps.wStapleOption = stapleopt_none;
	}

	//ByStapleless
	pps.byStapleless = pOemDev->scpi.dev.byFieldVDMStaple & DM_VDMS_STAPLELESS ? bool_true : bool_false;

	if (limit.limStapleOptVsStapleType(&pps, pps.wStaple) != FALSE)
	{
		pOemDev->scpi.ext.byStaple = STAPLE_NONE;
	}	
	//<S><A>To Fix RedmineBug #4510- issue 1 and Issue 3,2021.11.29,SSDI:Chanchal Singla
	//Function limitation is not proper when Staple is set in Office application
	//Issue 1:- 5.6.1 Offset is not restored to None when Staple setting is done in Office application
	//Issue 2 and 5:- 5.6.2 Staple is not restored to None when Staple setting is done in Office application
	//Issue 3:- 5.6.3 Margin Shift is not restored to None when Staple setting is done in Office application
	//Issue 4:- 5.6.4 Staple is not changed to 2 - Staples
	if (pOemDev->scpi.ext.byPamph == PAMPH_2UP)
	{
		if ((pOemDev->scpi.ext.byStaple == STAPLE_SADDLE))
		{
			pOemDev->scpi.ext.byStaple = STAPLE_SADDLE;
		}
		else
		{
			pOemDev->scpi.ext.byStaple = STAPLE_NONE;
		}
	}
	//To Fix Coverity #91600,2021.12.15,SSDI:Chanchal Singla
	if (((DMMEDIA_T_OHP == pOemDev->scpi.pub.dmMediaType)
		|| (DMMEDIA_ENVELOPE == pOemDev->scpi.pub.dmMediaType) 
		|| (DMMEDIA_J_POSTCARD == pOemDev->scpi.pub.dmMediaType)
		|| (DMMEDIA_LABEL == pOemDev->scpi.pub.dmMediaType))
		&& (pOemDev->scpi.ext.byStaple == STAPLE_SADDLE))
	{
		pOemDev->scpi.ext.byStaple = STAPLE_TWO;
	}
	if (pOemDev->scpi.ext.byStaple != staple_none)
	{
		// None
		pOemDev->scpi.ext.dwFieldEDM1 &= ~DM_OFFSET;
		pOemDev->scpi.ext.byOffset = offset_none;
	}

	if (pOemDev->scpi.ext.byStaple == STAPLE_SADDLE)
	{
		pOemDev->scpi.ext.byMgnShift = marginshift_none;
	}
	//<E>To Fix RedmineBug #4510- issue 1 and Issue 3,2021.11.29,SSDI:Chanchal Singla
}
// <E> To Fix redmine#2810, 2021.09.06, SSDI:Uday

//<S><A>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
//=============================================================================
// function
//      PrivateDevmodeDataToPropState
//
// return value
//     VOID
//
// outline
//    //   Updates Property sheet structure based on Private Devmode data structure.
//=============================================================================
VOID COemUI2::PrivateDevmodeDataToPropState(HMODULE	hStringResourceHandle, WCHAR* pPrinterName, POEMDEV pOemPrivateDevMode,PPROPSTATE PropertySheetState)
{
	// Checking the condition for stringresource handle and printer name
	if (pPrinterName == NULL || hStringResourceHandle == NULL)
	{
		return;
	}

	// -- Staple
	PropertySheetState->wStaple = pOemPrivateDevMode->scpi.ext.byStaple;

	//PUNCH
	PropertySheetState->wPunch = pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_PUNCH ? bool_true : bool_false;
	//<S><A>To Fix Redmine Bug #4005 and Feedback #41(issue 11),2021.August.05,SSDI:Chanchal Singla
	//Collate dropdown does not restore back to correct value while Punch is set
	PropertySheetState->wPunchPre = pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_PUNCH ? bool_true : bool_false;
	//<E>To Fix Redmine Bug #4005 and Feedback #41(issue 11),2021.August.05,SSDI:Chanchal Singlas
	//--wPunchType
	SHORT wPunchType = punchtype_none;
	switch (pOemPrivateDevMode->scpi.ext.byPunchType)
	{
	case PUNCH_NONE:
		wPunchType = punchtype_none;
		break;
	case PUNCH_2HOLES:
		wPunchType = punch_2holes;
		break;
	case PUNCH_3HOLES:
		wPunchType = punch_3holes;
		break;
	case PUNCH_4HOLES:
		wPunchType = punch_4holes;
		break;
	case PUNCH_4HOLES_WIDE:
		wPunchType = punch_4holes_wide;
		break;
	default:
		break;
	}
	PropertySheetState->wPunchType = wPunchType;

	//bLimitByOption
	PropertySheetState->blLimitByOption = (pOemPrivateDevMode->scpi.ext2.wFieldE2DM & DM_LIMT_BY_OPTION) ? bool_true : bool_false;

	/*=========================================================================*/
	/*+++++++++++++++++++OthertSetting Dialog Values+++++++++++++++++++++++++*/
	/*=========================================================================*/
	// --- Zoom reference point
	if (pOemPrivateDevMode->scpi.doc.dwFieldDDM & DM_ZOOM_CENTER)
		PropertySheetState->wZoomBasing = zoom_basing_center;
	else
		PropertySheetState->wZoomBasing = zoom_basing_upperleft;
	//<S><A>Updating PROPSTATE variable for Other Setttings dialog, 2020.10.08, SSDI:Sneha TG
		// --- byResolution
	PropertySheetState->wResolution = pOemPrivateDevMode->scpi.ext.byResolution;

	// --- byDuplexStyle
	PropertySheetState->wDupStyle = pOemPrivateDevMode->scpi.ext.PCFax.byDuplexStyle;

	// --- DM_WYSIWYG
	PropertySheetState->wWysiWyg = pOemPrivateDevMode->scpi.doc.dwFieldDDM & DM_WYSIWYG ? bool_true : bool_false;

	if (PropertySheetState->wWysiWyg == bool_true)			// CnvWysiWygToCmptShade(m_PropertySheetState.wWysiWyg)
		PropertySheetState->wCmptShade = cmpt_shade_standard;
	else
		PropertySheetState->wCmptShade = cmpt_shade_fine;

	// --- DM_EMFJOURNAL
	PropertySheetState->wEMFSpool = pOemPrivateDevMode->scpi.doc.dwFieldDDM & DM_EMFJOURNAL ? bool_true : bool_false;

	if (PropertySheetState->wEMFSpool == bool_true)			// CnvEMFSpoolToCmptSpool(m_PropertySheetState.wEMFSpool);
		PropertySheetState->wCmptSpool = cmpt_spool_emf;
	else
		PropertySheetState->wCmptSpool = cmpt_spool_raw;

	// --- DM_EDGETOEDGE
	PropertySheetState->wEdgeToEdge = pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_EDGETOEDGE ? bool_true : bool_false;

	if (PropertySheetState->wEdgeToEdge == bool_true)			// CnvEdgeToEdgeToCmptPrnArea(m_PropertySheetState.wEdgeToEdge);
		PropertySheetState->wCmptPrnArea = cmpt_prnarea_maximum;
	else
		PropertySheetState->wCmptPrnArea = cmpt_prnarea_standard;

#if SCDM_VER >= 1918
	// --- DM_SHORT_JOBNAME
	PropertySheetState->wShortJobName = pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_SHORT_JOBNAME ? bool_true : bool_false;
#endif

	// --- DM_JPEG_PASSTHROUGH
	PropertySheetState->wJpegPass = pOemPrivateDevMode->scpi.doc.dwFieldDDM & DM_JPEG_PASSTHROUGH ? bool_true : bool_false;

	// --- wSetLineToOne
	PropertySheetState->wSetLineToOne = pOemPrivateDevMode->scpi.doc.wPenWidthRate == 0 ? bool_true : bool_false;

	//<S><C>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
	//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
	//wCusUnit
	PropertySheetState->wCusUnit = pOemPrivateDevMode->scpi.ext.byCusUnits;
	// update Custom paper value to propstate
	{
		
		CUPSReg pregUps(hStringResourceHandle, pPrinterName);

		//CShIniFile			*m_pmcf = NULL;
		//TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
		//GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
		//m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
		//CShJsonUserPSize	*pjsonups = NULL;
		//if ((*m_pmcf).IsWriteToJson() == TRUE)
		//{
		//	pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
		//}

		USERPAPERSIZEDATA	upsd;
		SecureZeroMemory(&upsd, sizeof(upsd));
		if (pOemPrivateDevMode->scpi.pub.dmPaperSize == DMPAPER_CUSTOM)
		{
			if (unit_inch == PropertySheetState->wCusUnit)
			{
				int m_Width = MilliToInch(pOemPrivateDevMode->scpi.pub.dmPaperWidth, MM_LOMETRIC, MM_LOENGLISH);
				int m_Length = MilliToInch(pOemPrivateDevMode->scpi.pub.dmPaperLength, MM_LOMETRIC, MM_LOENGLISH);

				PropertySheetState->dwCustomWidth = m_Width;
				PropertySheetState->dwCustomLength = m_Length;

			}
			else
			{
				PropertySheetState->dwCustomWidth = pOemPrivateDevMode->scpi.pub.dmPaperWidth;
				PropertySheetState->dwCustomLength = pOemPrivateDevMode->scpi.pub.dmPaperLength;
			}
		}
		else
		{
			if (PropertySheetState->wCusUnit == unit_inch)
			{
				MIP_MINMAX_CONV_PARAM stCnvParam;
				SecureZeroMemory(&stCnvParam, sizeof(MIP_MINMAX_CONV_PARAM));
				stCnvParam.paper = mip_paper_cus_usr;
				stCnvParam.mi = mip_mi_inch;
				stCnvParam.pos = 1;
				PropertySheetState->dwCustomWidth = PixelToInch_CusPaper(pOemPrivateDevMode->scpi.ext.dwCusWidthPixel, pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
				stCnvParam.wl = mip_wl_width;
				PropertySheetState->dwCustomWidth = checkMinMax(PropertySheetState->dwCustomWidth, &stCnvParam);
				stCnvParam.wl = mip_wl_length;
				PropertySheetState->dwCustomLength = PixelToInch_CusPaper(pOemPrivateDevMode->scpi.ext.dwCusLengthPixel, pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
				PropertySheetState->dwCustomLength = checkMinMax(PropertySheetState->dwCustomLength, &stCnvParam);
			}
			else
			{
				MIP_MINMAX_CONV_PARAM stCnvParam;
				SecureZeroMemory(&stCnvParam, sizeof(MIP_MINMAX_CONV_PARAM));
				stCnvParam.paper = mip_paper_cus_usr;
				stCnvParam.mi = mip_mi_mill;
				stCnvParam.pos = 10;
				stCnvParam.wl = mip_wl_width;
				PropertySheetState->dwCustomWidth = PixelToMilli_CusPaper(pOemPrivateDevMode->scpi.ext.dwCusWidthPixel, pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOMETRIC, &stCnvParam);
				stCnvParam.wl = mip_wl_length;
				PropertySheetState->dwCustomLength = PixelToMilli_CusPaper(pOemPrivateDevMode->scpi.ext.dwCusLengthPixel, pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOMETRIC, &stCnvParam);
			}

		}
	}
	//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
	/*=========================================================================*/
	/*+++++++++++++++++++Configuration Tab Values(Dev Structure)+++++++++++++++++++++++++*/
	/*=========================================================================*/
	//Automatic color mode
	PropertySheetState->byAutoColorMode = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_VDM_COLORMODE_AUTO_DISABLE ? bool_true : bool_false;

	//PinCompatibility
	PropertySheetState->byPinCompatibility = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_VDM_PIN_CLEAN_TEXT ? bool_true : bool_false;

	// --- DM_SINGLESIGNON
	PropertySheetState->wSingleSignOn = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_SINGLESIGNON ? bool_true : bool_false;

	//Maxpapersize
	PropertySheetState->byMaxPaperSize = (pOemPrivateDevMode->scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;

	//ByPrintRelease
	PropertySheetState->byPrintRelease = pOemPrivateDevMode->scpi.dev.byFieldVDM & DM_VDM_NON_PRITNRLEASE ? bool_true : bool_false;

	//Color Mode
	if (pOemPrivateDevMode->scpi.dev.byFieldVDM & DM_VDM_COLORMODEL)
	{
		PropertySheetState->wColorOption = 0; //color
	}
	else
	{
		PropertySheetState->wColorOption = 1; //black and white
	}

	//ByPunchmodule
	SHORT punch = punchtype_none;
	switch (pOemPrivateDevMode->scpi.dev.byPunchmodule)
	{
	case punchopt_none:				// 0 none
		punch = punchtype_none;
		break;
	case punchopt_1:				// 1 //2 Holes
		punch = punchtype_1;
		break;
	case punchopt_6:				// 6 //3 Holes
		punch = punchtype_2;
		break;
	case punchopt_7:				// 7 //4 Holes
		punch = punchtype_3;
		break;
	case punchopt_4:				// 4 //4(Wide) Holes
		punch = punchtype_4;
		break;
	case punchopt_2:				// 2 //2/3 Holes
		punch = punchtype_5;
		break;
	case punchopt_3:				// 3 //2/4 Holes
		punch = punchtype_6;
		break;
	default:
		break;
	}
	PropertySheetState->wPunchOption = punch;

	//ByStaplemodule
	if (pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_SADDLE)
	{
		PropertySheetState->wStapleOption = stapleopt_3;
	}
	else if (pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_2STAPLE)
	{
		PropertySheetState->wStapleOption = stapleopt_2;
	}
	else if (pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_1STAPLE)
	{
		PropertySheetState->wStapleOption = stapleopt_1;
	}
	else
	{
		PropertySheetState->wStapleOption = stapleopt_none;
	}

	//ByStapleless
	PropertySheetState->byStapleless = pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_STAPLELESS ? bool_true : bool_false;

	//PPPrintRelease
	PropertySheetState->wPPlcyPrintRelease = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_PRINTRELEASE ? bool_true : bool_false;
	if (PropertySheetState->wPPlcyPrintRelease == bool_true)
		PropertySheetState->wPrintRelease = bool_true;
	else
		PropertySheetState->wPrintRelease = bool_false;

	//PPDocumentFiling
	PropertySheetState->wPPlcyRetention = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_RETENTION ? bool_true : bool_false;

	//PPUserAuthentication
	PropertySheetState->wPPlcyUserAuth = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_USERAUTH ? bool_true : bool_false;

	//PPWindowsLoginNameasUserName
	PropertySheetState->wPPlcyWinLogin = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN ? bool_true : bool_false;

	//PPLgoinNameasUserName
	PropertySheetState->wPPlcyLoginName = pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_LOGINN_AS_USERN ? bool_true : bool_false;


	/*=========================================================================*/
	/*+++++++++++++++++++Job Handling Values+++++++++++++++++++++++++*/
	/*=========================================================================*/

	// --- byRetention
	SHORT wRetentionMode = eFirstOption;
	switch (pOemPrivateDevMode->scpi.ext.JobCtrl.byRetention)
	{
	case retention_normal:
		wRetentionMode = eFirstOption;
		break;
	case retention_hold_before:
		wRetentionMode = eSecondOption;
		break;
	case retention_hold_after:
		wRetentionMode = eThirdOption;
		break;
	case retention_sample:
		wRetentionMode = eFourthOption;
		break;
	default:
		break;
	}
	PropertySheetState->wRetentionMode = wRetentionMode;

	// --- byDocumentFiling
	SHORT wDocFileMode = eFirstOption;
	switch (pOemPrivateDevMode->scpi.ext.JobCtrl.byDocumentFiling)
	{
	case docfile_quick:
		wDocFileMode = eFirstOption;
		break;
	case docfile_main:
		wDocFileMode = eSecondOption;
		break;
	case docfile_custom:
		wDocFileMode = eThirdOption;
		break;
	default:
		break;
	}
	PropertySheetState->wDocFileMode = wDocFileMode;

	// --- wDocFileFolder
	PropertySheetState->wDocFileFolder = pOemPrivateDevMode->scpi.ext.JobCtrl.nDocFilingFolder;

	// --- DM_PRINTSHARE
	PropertySheetState->wPrintRelease = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_PRINTSHARE ? bool_true : bool_false;

	// --- DM_DOCFILING
	PropertySheetState->wRetention = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_DOCFILING ? bool_true : bool_false;

	// --- DM_ALWAYSUSE_ACNUM
	PropertySheetState->wUseAcNum = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM ? bool_true : bool_false;

	// --- DM_ALWAYSUSE_USERID
	PropertySheetState->wUseUName = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_USERID ? bool_true : bool_false;

	// --- DM_ALWAYSUSE_JOBID
	PropertySheetState->wUseJName = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_JOBID ? bool_true : bool_false;

	// --- DM_USE_LOGINNAME
	PropertySheetState->wLoginName = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME ? bool_true : bool_false;

	// --- DM_USE_LOGINPASS
	PropertySheetState->wLoginPass = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS ? bool_true : bool_false;

	// --- DM_DOCFILING
	PropertySheetState->wRetention = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_DOCFILING ? bool_true : bool_false;

	// --- DM_USE_PIN
	PropertySheetState->wUsePIN = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_USE_PIN ? bool_true : bool_false;

	// --- DM_USE_SINGLESIGNON
	PropertySheetState->wLoginNameSSO = pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_USE_SINGLESIGNON ? bool_true : bool_false;
	// ================================================
	// _/_/_/  job control data
	// ================================================
	SetRegJCInfoToPropState(hStringResourceHandle,&pOemPrivateDevMode->scpi, PropertySheetState, m_pPrinterName);

}
//=============================================================================
// function
//      SetRegJCInfoToPropState
//
// parameters
//      pps                 PROPSTATEç
//      pszPrnName          
//
// return value
//      êTRUE
//       FALSE
//
// outline
//     SetRegJCInfoToPropState
//=============================================================================
BOOL COemUI2::SetRegJCInfoToPropState(HMODULE	hStringResourceHandle,PSCDM pscdm, PPROPSTATE pps, WCHAR FAR *pszSvrPrnName)
{
	BOOL			blRet = FALSE;

	PREGJCINFO		pregjcinfo = NULL;

	DWORD			dwSize = 0;
	CShRegJC *pregjc = new CShRegJC(hStringResourceHandle, pszSvrPrnName);

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, pszSvrPrnName, szCommonDatFilePath, FALSE);
	CShJsonJC	*pjsonjc = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonjc = new CShJsonJC(ghInstance, pszSvrPrnName);
		pjsonjc->Init();
	}

	if (pregjc == NULL)
	{
		goto EXIT;
	}
	if (pps == NULL)
		goto EXIT;

	pregjcinfo = new REGJCINFO;
	if (pregjcinfo != NULL)
	{
		SecureZeroMemory(pregjcinfo, sizeof(REGJCINFO));
		// Some Code is Commented because Devmode changes and registry part is pending
		(*pregjcinfo).wUsePIN = (*pps).wUsePIN;
		(*pregjcinfo).wUseAcNum = (*pps).wUseAcNum;
		(*pregjcinfo).wUseUName = (*pps).wUseUName;
		(*pregjcinfo).wUseJName = (*pps).wUseJName;
		(*pregjcinfo).wLoginName = (*pps).wLoginName;
		(*pregjcinfo).wLoginPass = (*pps).wLoginPass;
		(*pregjcinfo).wFolderIndex = (*pps).wDocFileFolder;
	//	(*pregjcinfo).wSingleSignOn = (*pps).wSingleSignOn;
		(*pregjcinfo).wSingleSignOn = (*pps).wLoginNameSSO;

		if ((*m_pmcf).IsWriteToJson() == TRUE)
		{
			if ((*pjsonjc).ReadJCData(pregjcinfo, (*pps).wPPlcyWinLogin) != FALSE)
				RegJCInfoToPropState(pregjcinfo, pps, pszSvrPrnName);
		}
		else
		{
			if ((*pregjc).ReadJCData(pszSvrPrnName, pregjcinfo, (*pps).wPPlcyWinLogin) != FALSE)
				RegJCInfoToPropState(pregjcinfo, pps, pszSvrPrnName);
		}


		blRet = TRUE;
	}

EXIT:

	if (pregjc != NULL)
	{
		delete pregjc;
		pregjc = NULL;
	}
	if (pregjcinfo != NULL)
	{
		delete pregjcinfo;
		pregjcinfo = NULL;
	}
	if (pjsonjc != NULL) {
		delete pjsonjc;
		pjsonjc = NULL;
	}
	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = NULL;
	}
	return blRet;
}
//=============================================================================
// function
//       RegJCInfoToPropState
//
// parameters
//      prji                REGJCINFO
//      pps                 PROPSTATEç
//      pszPrnName          
//
// return value
//      TRUE
//     FALSE
//
// outline
//       REGJCINFOÇÁPROPSTATEÇ
//=============================================================================
BOOL COemUI2::RegJCInfoToPropState(PREGJCINFO prji, PPROPSTATE pps, WCHAR FAR *pszSvrPrnName)
{
	BOOL			blRet = FALSE;


	if (prji == NULL || pps == NULL || pszSvrPrnName == NULL)
		goto EXIT;

	(*pps).wUsePIN = (*prji).wUsePIN;
	(*pps).wUseAcNum = (*prji).wUseAcNum;
	(*pps).wUseUName = (*prji).wUseUName;
	(*pps).wUseJName = (*prji).wUseJName;
	(*pps).wLoginName = (*prji).wLoginName;
	(*pps).wLoginPass = (*prji).wLoginPass;
	//(*pps).wSingleSignOn = (*prji).wSingleSignOn;
	(*pps).wLoginNameSSO = (*prji).wSingleSignOn;

	if ((*pps).wLoginNameSSO == bool_true) {
		// Single Sign-onÇLoginNameÇ
		(*pps).wLoginName = bool_false;
	}
	(*pps).wDocFileFolder = (*prji).wFolderIndex;


	SecureZeroMemory((*pps).szUserName, sizeof((*pps).szUserName));
	SecureZeroMemory((*pps).szJobName, sizeof((*pps).szJobName));
	SecureZeroMemory((*pps).szUserNumber, sizeof((*pps).szUserNumber));
	SecureZeroMemory((*pps).szLoginNameUA, sizeof((*pps).szLoginNameUA));
	SecureZeroMemory((*pps).szLoginPasswordUA, sizeof((*pps).szLoginPasswordUA));
	SecureZeroMemory((*pps).szSingleSignOnName, sizeof((*pps).szSingleSignOnName));
	SecureZeroMemory((*pps).szRetentionPassword, sizeof((*pps).szRetentionPassword));
	SecureZeroMemory((*pps).szFolderPass, sizeof((*pps).szFolderPass));

	if ((*prji).wUseUName == bool_true)
	wcscpy_s((*pps).szUserName, _countof((*pps).szUserName), (*prji).szUserName);
	if ((*prji).wUseJName == bool_true)
	wcscpy_s((*pps).szJobName, _countof((*pps).szJobName), (*prji).szJobName);
	if ((*prji).wUseAcNum == bool_true)
		wcscpy_s((*pps).szUserNumber, _countof((*pps).szUserNumber), (*prji).szAccountNum);
	if ((*prji).wLoginName == bool_true)
		wcscpy_s((*pps).szLoginNameUA, _countof((*pps).szLoginNameUA), (*prji).szLoginName);
	if ((*prji).wLoginPass == bool_true)
		::lstrcpy((*pps).szLoginPasswordUA, (*prji).szLoginPassword);
	if ((*prji).wUsePIN == bool_true)
		::lstrcpy((*pps).szRetentionPassword, (*prji).szPIN);
	/*if ((*prji).wSingleSignOn == bool_true)
		::lstrcpy((*pps).szSingleSignOnName, (*prji).szSingleSignOnName);*/
	if ((*prji).wSingleSignOn != 0) {
		::wcscpy_s((*pps).szSingleSignOnName, _countof((*pps).szSingleSignOnName), (*prji).szSingleSignOnName);
	}
	if ((*prji).wFolderIndex != 0)
		::lstrcpy((*pps).szFolderPass, (*prji).szFolderPass);

	blRet = TRUE;

EXIT:

	return blRet;
}
//<E>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla


short  GetSingleSignOn(LPSCDM pscdm, char FAR *pszSvrPrnName, DWORD dwFlag, LPBYTE pData, LPDWORD pdwNeeded)
{
	BOOL				blRetSSOAPI = FALSE;
	LPSINGLESIGNDATA	pssd = NULL;
	PPROPSTATE			pps = NULL;
	char FAR			*p = NULL;

	SSOAPI_GetServiceTicketSize		lpSSOGetServiceTicketSize = NULL;
	SSOAPI_GetServiceTicket			lpSSOGetServiceTicket = NULL;
	CShRegAC		*preg = NULL;
	char FAR		*pSPN = NULL;
	DWORD			dwSPNSize = 0;

	HMODULE			hDllSSO = NULL;
	ULONG			ulServiceTicketSize = 0;
	PBYTE			pbServiceTicket = NULL;
	DWORD			dwServiceTicketSize = 0;

	DWORD			dwCount = 0;
	DWORD			dwTempBufSize = 0;
	char FAR		*pTemp = NULL;

	char FAR		*pcServiceTicketData = NULL;

	WCHAR			szPath[_MAX_PATH + 1];			// SSO.DLL FilePath 

	short			wRet = -1;

	//LogStartFunc("CShGetPrnInfo::GetSingleSignOn");

	// ================================================
	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
	// ================================================
	/*if ( pszSvrPrnName == NULL || pdwNeeded == NULL)
		goto EXIT;*/
		//CRegistryAPI retgApi;
	PWSTR pDeviceName = NULL;
	eDeviceType wDeviceType = eColor;
	HINSTANCE			m_hInstance = NULL;
	CShIniFile			*m_pmcf = NULL;
	//retgApi.GetDeviceType(pDeviceName, wDeviceType);
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");

	m_pmcf = new CShIniFile(ghInstance, (WCHAR*)pszSvrPrnName, szCommonDatFilePath, FALSE);
	if (m_pmcf == NULL)
		CShRegMdlTbl RegMdlTbl(ghInstance, (WCHAR*)pszSvrPrnName, m_pmcf/*, NULL*/);
	preg = new CShRegAC(ghInstance, (WCHAR*)pszSvrPrnName, m_pmcf);


	// ================================================
	// _/_/_/  ÉTÉCÉYÇÃéÊìæ
	// ================================================
	if (pData == NULL || *pdwNeeded == 0)
	{
		// SPNéÊìæ
		(*preg).ReadSPN(pSPN, &dwSPNSize);
		pSPN = new char[dwSPNSize];
		if (pSPN != NULL) {
			SecureZeroMemory(pSPN, dwSPNSize);
			(*preg).ReadSPN(pSPN, &dwSPNSize);
		}
		else {
			goto EXIT;
		}

		// SSO.DLLÉtÉ@ÉCÉãÉpÉXéÊìæ
		::ZeroMemory(szPath, sizeof(szPath));
		if (CreateModuleFilePath(ghInstance, SCUI_MOD_ORG_SSO, SCUI_MOD_FOTTER_SSO, szPath, _countof(szPath)) == FALSE) {
			goto EXIT;
		}

		// SSO.DLLÉçÅ[Éh
		hDllSSO = ::LoadLibrary(szPath);
		if (hDllSSO == NULL) {
			goto EXIT;
		}

		// SSO.DLLÉÇÉWÉÖÅ[ÉãÇÃä÷êîÉAÉhÉåÉXÇéÊìæ
		lpSSOGetServiceTicketSize = (SSOAPI_GetServiceTicketSize)GetProcAddress(hDllSSO, "GetServiceTicketSize");
		lpSSOGetServiceTicket = (SSOAPI_GetServiceTicket)GetProcAddress(hDllSSO, "GetServiceTicket");

		if ((lpSSOGetServiceTicketSize != NULL) && (lpSSOGetServiceTicket != NULL)) {
			if (lpSSOGetServiceTicketSize != NULL) {
				// ÉTÅ[ÉrÉXÉ`ÉPÉbÉgÉTÉCÉYéÊìæ
				ulServiceTicketSize = lpSSOGetServiceTicketSize(pSPN);
				if (ulServiceTicketSize == 0) {
					goto EXIT;
				}

				// ÉTÅ[ÉrÉXÉ`ÉPÉbÉgäiî[ópÉoÉbÉtÉ@Çämï€
				pbServiceTicket = new BYTE[ulServiceTicketSize];
				if (pbServiceTicket != NULL) {
					// ÉTÅ[ÉrÉXÉ`ÉPÉbÉgéÊìæ
					blRetSSOAPI = lpSSOGetServiceTicket(pbServiceTicket, ulServiceTicketSize);
					if (blRetSSOAPI == TRUE) {
						(*preg).WriteServiceTicket((char*)pbServiceTicket, ulServiceTicketSize);
						wRet = 1;
						goto EXIT;
					}
				}
			}
		}
		goto EXIT;
	}
	// ================================================
	// _/_/_/  ÉTÉCÉYÉ`ÉFÉbÉN
	// ================================================
	if (*pdwNeeded < sizeof(SINGLESIGNDATA))
		goto EXIT;

	// ================================================
	// _/_/_/  èÓïÒçÏê¨
	// ================================================
	//pps = (*ppi).pps;
	pssd = (LPSINGLESIGNDATA)pData;

	if (pssd != NULL)
	{
		//LogComment("input SINGLESIGNDATA");
		//LogSINGLESIGNDATA(pssd);
	}

	// --- dwSignature
	p = (char *)&(*pssd).dwSignature;
	*p = 'S';
	p++;
	*p = 'S';
	p++;
	*p = 'D';
	p++;
	*p = 'T';

	// --- dwFieldSSDT
	(*pssd).dwFieldSSDT = 0;

	// --- dwSize
	dwServiceTicketSize = ((*pdwNeeded) - sizeof(SINGLESIGNDATA));
	(*pssd).dwSize = dwServiceTicketSize;

	// --- dwOffset
	(*pssd).dwOffset = sizeof(SINGLESIGNDATA);

	// ServiceTicketéÊìæ
	dwCount = dwServiceTicketSize / 16;
	if (dwServiceTicketSize % 16 != 0) {
		dwCount++;
	}
	dwTempBufSize = dwCount * 16;
	pTemp = new char[dwTempBufSize];
	if (pTemp != NULL) {
		(*preg).ReadServiceTicket(pTemp, &dwTempBufSize);
		memcpy((char*)(pData + (*pssd).dwOffset), pTemp, dwServiceTicketSize);
	}
	else {
		goto EXIT;
	}
	// ToDo ÉåÉWÉXÉgÉäè¡ãé

	//LogComment("output SINGLESIGNDATA");
	//LogSINGLESIGNDATA(pssd);

	ulServiceTicketSize = dwServiceTicketSize;

	wRet = 1;

EXIT:

	if (pdwNeeded != NULL)
		*pdwNeeded = sizeof(SINGLESIGNDATA) + ulServiceTicketSize;

	if (pTemp != NULL) {
		delete[] pTemp;
	}

	if (pbServiceTicket != NULL) {
		delete[] pbServiceTicket;
	}

	if (pSPN != NULL) {
		delete[] pSPN;
	}

	if (preg != NULL) {
		delete preg;
	}

	//<S><A> Coverity Fix 91533 - SSDI:Seetharam - 20210907
	if (m_pmcf != NULL)
		delete m_pmcf;
	//<S><E> Coverity Fix 91533 - SSDI:Seetharam - 20210907

	// <S><A> 2016.11.02 SSL:K.Hamaguchi
	if (hDllSSO != NULL) {
		::FreeLibrary(hDllSSO);
	}

	return wRet;
}

short WINAPI GetPrintInfo(LPSCDM pscdm, char FAR *pszSvrPrnName, DWORD dwFlag, LPBYTE pData, LPDWORD pdwNeeded)
{
	short			wRet = -1;
	wRet = GetSingleSignOn(pscdm, pszSvrPrnName, dwFlag, pData, pdwNeeded);
	return wRet;
}
//<S><C>To Fix Redmine #3151 Issue 3,26.august.2021,SSDI:Chanchal Singla
//Issue 3:-Silent Autoconfiguration is not executing from General tab
LRESULT WINAPI ExecuteAutoConfiguration(WCHAR *pPrinterName, HANDLE hPrinter, HMODULE hStringResourceHandle, HMODULE hBitmapResourceHandle,
	HMODULE hDialogResourceHandle,POEMDEV pPrivateDemode, long g_CurrentIndex,long previousMFPIndex, BOOL bMfpIndex,bool bSilentAutoConfig)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	LRESULT			res = 0;
	long			lRet = IDCANCEL;
	short			wID = 0;

	PROPSTATE PropertySheetState;
	SecureZeroMemory(&PropertySheetState, sizeof(PropertySheetState));
	
	INT_PTR DlgRet = IDCANCEL;
	CUIControl pControl;
	CShDlgAutoConf *pDlg = NULL;
	CRegistryAPI regAPI(ghInstance);
	
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	CShIniFile mcf(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
	DevmodeToPropstate(pPrivateDemode, &PropertySheetState);

	OEMDEV ptempPrivateDevmode ;
	SecureZeroMemory(&ptempPrivateDevmode,sizeof(ptempPrivateDevmode));
	memcpy_s(&ptempPrivateDevmode, sizeof(OEMDEV), pPrivateDemode, sizeof(OEMDEV));
	
	CShRegMdlTbl RegMdlTbl(ghInstance, pPrinterName,&mcf);
	
	short wPrevDeviceType = eColor;
	RegMdlTbl.GetDeviceType(&wPrevDeviceType);
	
	HWND hWnd = GetActiveWindow();

	SHORT sCount = 0;
	RegMdlTbl.GetCount(&sCount);
	
	DWORD wInstalledMode = eBasicMode;
	RegMdlTbl.GetInstallationType(wInstalledMode);
	
	UDMRI udmri = { 0 };
	regAPI.GetModelRegInfo(pPrinterName, g_CurrentIndex, &udmri);
	if ((0 != udmri.iManuallyAdded) ||
		((0 == sCount) && (eEnhancedMode == wInstalledMode)) ||
		(eBasicMode == wInstalledMode)
		)
	{
		//pregac->ProcessColorInformation(&(*pscdm).dev);
		short wDeviceType = (TRUE == udmri.blBWprint) ? eMono : eColor;
		if (eColor == wDeviceType)
		{
			pPrivateDemode->scpi.dev.byFieldVDM |= DM_VDM_COLORMODEL;

		}
		else {
			pPrivateDemode->scpi.dev.byFieldVDM &= ~DM_VDM_COLORMODEL;

		}
		//pscdm->wFieldSPI = wFieldSPI;
		regAPI.WriteData(pPrinterName,&pPrivateDemode->scpi.dev);
		regAPI.Set_Manually_MFP_INDEX_CHANGE(pPrinterName, TRUE);
		//<S><A>To Fix Bug #4274,2021-07.September,SSDI:Chanchal Singla
		//Printer driver language changes to English when Default Printer is changed from General tab
		pPrivateDemode->scpi.dev.nLanguageID = regAPI.GetLangID(pPrinterName, 0);
		//<E>To Fix Bug #4274,2021-07.September,SSDI:Chanchal Singla
		RegMdlTbl.SetCheckPos(pPrivateDemode->scpi.dev.nTargetDeviceID, pPrivateDemode->scpi.dev.nLanguageID, wDeviceType);
		regAPI.UpdateDriverCapabilities(pPrinterName, hStringResourceHandle, wDeviceType, IDS_INDEX0_TS_PSIZE, udmri.wPPM);
		res = IDOK;
		goto EXIT;
	//	(*pregac).SetAutoConfig(FALSE)
	}
	if (previousMFPIndex != g_CurrentIndex)
	{
		CShowMessage ObjCShowMessage(hWnd, hStringResourceHandle);
		//<S><A>To Implement Task#3132,31-05-2024,SSDI:Manoj S
		/*pDlg = new CShDlgAutoConf(ghInstance, hWnd, pPrinterName, IDD_DLG_UPDATING,
			hStringResourceHandle, hBitmapResourceHandle, hPrinter, hDialogResourceHandle,
			&PropertySheetState, &mcf, &ObjCShowMessage, &ptempPrivateDevmode);*/
		pDlg = new CShDlgAutoConf(ghInstance, hWnd, pPrinterName, IDD_DLG_UPDATING,
			hStringResourceHandle, hBitmapResourceHandle, hPrinter, hDialogResourceHandle,
			&PropertySheetState, &mcf, &ObjCShowMessage, &ptempPrivateDevmode, FeatureInfoList);
		//<E>To Implement Task #3132,31-05-2024,SSDI:Manoj S
		if (NULL == pDlg)
		{
			slog.putLog("CDevicePropertyPage::OnIdcBtnPrnSelCfg -> NULL_OBJECT(pdlg)\n");
			return res;	//Coverity Fix - 91162
		}


		DlgRet = ::DialogBoxParam(hDialogResourceHandle, MAKEINTRESOURCEW(IDD_DLG_UPDATING), hWnd,
			(DLGPROC)(*pDlg).WindowMapProc, (LPARAM)pDlg);
		//<S><A>To the Support of AutoConfiguration UI Feature Values updataion,2020-18-12,SSDI:Chanchal Singla

		if (FALSE == pControl.IsEqualBuf(pPrivateDemode, &ptempPrivateDevmode, sizeof(OEMDEV)))
		{
			memcpy_s(pPrivateDemode, sizeof(OEMDEV), &ptempPrivateDevmode, sizeof(OEMDEV));
		}

		DevmodeToPropstate(pPrivateDemode, &PropertySheetState);

		if (dev_state_ok == pDlg->GetAutoConfigurationResult())
		{

			regAPI.WriteData(pPrinterName, &pPrivateDemode->scpi.dev);
			//<S><A>To Fix Bug #4274,2021-07.September,SSDI:Chanchal Singla
			//Printer driver language changes to English when Default Printer is changed from General tab
			//<S><C>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
			//In Basic mode Printing Preferences is not displayed as per the Language set
			TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
			GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
			CShIniFile cIniFile(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
			SHORT dwLangID = -1;
			CShRegMdlTbl RegMdlTbl(ghInstance, pPrinterName, &cIniFile);
			RegMdlTbl.GetLangID(&dwLangID);

			//pPrivateDemode->scpi.dev.nLanguageID  = (short)regAPI.GetLangID(pPrinterName, 0);
			pPrivateDemode->scpi.dev.nLanguageID = (short)dwLangID;
			//<E>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
			//<E>To Fix Bug #4274,2021-07.September,SSDI:Chanchal Singla
			regAPI.Set_WM_MFP_INDEX_CHANGE(pPrinterName, TRUE);
			if (!bSilentAutoConfig)
				PropSheet_Changed(GetParent(hWnd), hWnd);

			UDMRI udmri = {};
			LRESULT lRet = regAPI.GetModelRegInfo(pPrinterName, pPrivateDemode->scpi.dev.nTargetDeviceID, &udmri);
			//RegMdlTbl.GetModelRegInfo(pPrivateDemode->scpi.dev.nTargetDeviceID, &udmri, BWPRINT);

			short wDeviceType = 0;
			if (pPrivateDemode->scpi.dev.byFieldVDM & DM_VDM_COLORMODEL)
			{
				wDeviceType = eColor;
			}
			else
			{
				wDeviceType = eMono;
			}
			regAPI.SetCheckPos(pPrinterName,pPrivateDemode->scpi.dev.nTargetDeviceID, pPrivateDemode->scpi.dev.nLanguageID, wDeviceType);
			regAPI.UpdateDriverCapabilities(pPrinterName, hStringResourceHandle, wDeviceType, IDS_INDEX0_TS_PSIZE, udmri.wPPM);
			res = IDOK;
		}
		else
		{
			if (!bSilentAutoConfig)
				PropSheet_Changed(GetParent(hWnd), hWnd);
			res = IDCANCEL;
		}
	}
	
EXIT:
	//Coverity Fix: 90893 - SSDI:Seetharam - 20201125
	if (pDlg != NULL)
	{
		delete pDlg;
		pDlg = NULL;
	}
	return res;
}
/*=============================================================================
Routine Name:
	CDevicePropertyPage::devmodetoPropstate
Routine Description:
	converts devmode value to propsate.
Arguments:
	Nil
Return Value:
	void
--=============================================================================*/
void DevmodeToPropstate(POEMDEV pOemDevMode, PPROPSTATE PropertySheetState)
{
	//<S><A> update propstate of printing policy dialog when Defaults is selected 2020/21/9 K M Harika
	//PPPrintRelease
	PropertySheetState->wPPlcyPrintRelease = pOemDevMode->scpi.dev.dwFieldVDM & DM_PP_PRINTRELEASE ? bool_true : bool_false;
	//PPDocumentFiling
	PropertySheetState->wPPlcyRetention = pOemDevMode->scpi.dev.dwFieldVDM & DM_PP_RETENTION ? bool_true : bool_false;
	//PPUserAuthentication
	PropertySheetState->wPPlcyUserAuth = pOemDevMode->scpi.dev.dwFieldVDM & DM_PP_USERAUTH ? bool_true : bool_false;
	//PPWindowsLoginNameasUserName
	PropertySheetState->wPPlcyWinLogin = pOemDevMode->scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN ? bool_true : bool_false;
	//PPLgoinNameasUserName
	PropertySheetState->wPPlcyLoginName = pOemDevMode->scpi.dev.dwFieldVDM & DM_PP_LOGINN_AS_USERN ? bool_true : bool_false;
	//<E> update propstate of printing policy dialog when Defaults is selected 2020/21/9 K M Harika

	//Automatic color mode
	PropertySheetState->byAutoColorMode = pOemDevMode->scpi.dev.dwFieldVDM & DM_VDM_COLORMODE_AUTO_DISABLE ? bool_true : bool_false;
	//PinCompatibility
	PropertySheetState->byPinCompatibility = pOemDevMode->scpi.dev.dwFieldVDM & DM_VDM_PIN_CLEAN_TEXT ? bool_true : bool_false;
	// --- DM_SINGLESIGNON
	PropertySheetState->wSingleSignOn = pOemDevMode->scpi.dev.dwFieldVDM & DM_SINGLESIGNON ? bool_true : bool_false;
	//Maxpapersize
	//PropertySheetState->byMaxPaperSize = pOemDevMode->scpi.dev.byMaxPaperSize;
	PropertySheetState->byMaxPaperSize = (pOemDevMode->scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
	//ByPrintRelease
	PropertySheetState->byPrintRelease = pOemDevMode->scpi.dev.byFieldVDM & DM_VDM_NON_PRITNRLEASE ? bool_true : bool_false;
	//Color Mode
	if (pOemDevMode->scpi.dev.byFieldVDM & DM_VDM_COLORMODEL) //Color
	{
		PropertySheetState->wColorOption = 0;
	}
	else // Black and White
	{
		PropertySheetState->wColorOption = 1;
	}
	//<S><A>Retrictions with Printing Policy Dialog,Harika,20201014
	if (PropertySheetState->wPPlcyPrintRelease == bool_true)
	{
		PropertySheetState->wPrintRelease = bool_true;
		pOemDevMode->scpi.ext.JobCtrl.wFieldJC |= DM_PRINTSHARE;
		PropertySheetState->wPPlcyRetention = bool_false;
		pOemDevMode->scpi.dev.dwFieldVDM &= ~DM_PP_RETENTION;
	}
	if (PropertySheetState->wPPlcyRetention != bool_false)
	{
		PropertySheetState->wRetention = bool_true;
		PropertySheetState->wRetentionMode = retention_hold_before;
		PropertySheetState->wDocFileMode = docfile_main;
		PropertySheetState->wUsePIN = bool_false;
		PropertySheetState->wPPlcyPrintRelease = bool_false;
		pOemDevMode->scpi.dev.dwFieldVDM &= ~DM_PP_PRINTRELEASE;
		PropertySheetState->wPrintRelease = bool_false;
	}
	//<E>Retrictions with Printing Policy Dialog,Harika,20201014
	//ByPunchmodule
	SHORT punch = punchtype_none;
	switch (pOemDevMode->scpi.dev.byPunchmodule)
	{
	case punchopt_none:				// 0 none
		punch = punchtype_none;
		break;
	case punchopt_1:				// 1 //2 Holes
		punch = punchtype_1;
		break;
	case punchopt_6:				// 6 //3 Holes
		punch = punchtype_2;
		break;
	case punchopt_7:				// 7 //4 Holes
		punch = punchtype_3;
		break;
	case punchopt_4:				// 4 //4(Wide) Holes
		punch = punchtype_4;
		break;
	case punchopt_2:				// 2 //2/3 Holes
		punch = punchtype_5;
		break;
	case punchopt_3:				// 3 //2/4 Holes
		punch = punchtype_6;
		break;
	default:
		break;
	}
	PropertySheetState->wPunchOption = punch;

	//ByStaplemodule
	if (pOemDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_SADDLE)
	{
		PropertySheetState->wStapleOption = stapleopt_3;
	}
	else if (pOemDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_2STAPLE)
	{
		PropertySheetState->wStapleOption = stapleopt_2;
	}
	else if (pOemDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_1STAPLE)
	{
		PropertySheetState->wStapleOption = stapleopt_1;
	}
	else
	{
		PropertySheetState->wStapleOption = stapleopt_none;
	}

	//ByStapleless
	PropertySheetState->byStapleless = pOemDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_STAPLELESS ? bool_true : bool_false;
//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
#if SCDM_VER >= 3000

	memcpy(pOemDevMode->scpi.ext2.stSubFavNames.szWMNameHash, PropertySheetState->szWMNameHash, sizeof(pOemDevMode->scpi.ext2.stSubFavNames.szWMNameHash));

#endif
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
}
//<E>To Fix Redmine #3151 Issue 3,26.august.2021,SSDI:Chanchal Singla

long WINAPI ScUpdateDEVDMODE(char FAR *pszPrnName, LPSCDM pscdm_s)
{
	BOOL        blRet = FALSE;
	if (RegToDevdmode((char FAR *)pszPrnName, pscdm_s) == FALSE) {
		return 0;
	}
	return 1;
}

BOOL RegToDevdmode(char FAR *pszSvrPrnName, LPSCDM pscdm)
{
	HANDLE				hPrinter = NULL;
	DWORD				dwRet = 0;
	DWORD				dwType = 0;
	DWORD				dwSize = 0;
	DWORD				dwNeeded = 0;
	LPBYTE				lpData = NULL;
	LPSCDM				lpScdm;
	PRINTER_DEFAULTS	pd;
	BOOL				bRet = FALSE;

	// OPEN
	pd.DesiredAccess = PRINTER_ACCESS_USE;
	pd.pDatatype = NULL;
	pd.pDevMode = NULL;

	if (sh_OpenPrinter((WCHAR FAR*)pszSvrPrnName, &hPrinter, &pd) != FALSE) {
		// READ
		// ì«Ç›éÊÇËÉTÉCÉYéÊìæÇÃà◊ÉTÉCÉY0Ç≈CallÇ∑ÇÈ
		dwSize = 0;
		if (IsUnicodeOS()) {
			dwRet = ::GetPrinterDataW(
				hPrinter,
				(LPWSTR)L"Private DevMode",
				//					REG_PRIVETE_DEVMODE2W,
				&dwType,
				lpData,
				dwSize,
				&dwNeeded);
		}
		else {
			dwRet = GetPrinterData(
				hPrinter,
				L"Private DevMode",
				//					REG_PRIVETE_DEVMODE2A,
				&dwType,
				lpData,
				dwSize,
				&dwNeeded);
		}

		// éÊìæÇµÇΩÉTÉCÉYÇ≈ì«Ç›éÊÇÈ
		if (dwRet == ERROR_MORE_DATA) {
			lpData = new BYTE[dwNeeded];
			if (lpData != NULL) {
				if (dwNeeded != 0) {
					dwSize = dwNeeded;
					if (IsUnicodeOS()) {
						dwRet = ::GetPrinterDataW(
							hPrinter,
							(LPWSTR)L"Private DevMode",
							//								REG_PRIVETE_DEVMODE2W,
							&dwType,
							lpData,
							dwSize,
							&dwNeeded);
					}
					else {
						dwRet = ::GetPrinterData(
							hPrinter,
							L"Private DevMode",
							//								REG_PRIVETE_DEVMODE2A,
							&dwType,
							lpData,
							dwSize,
							&dwNeeded);
					}
					if (dwRet == ERROR_SUCCESS) {
						// devdmodeÇÃÇ›èëÇ´ä∑Ç¶ÇÈ
						lpScdm = (LPSCDM)lpData;
						//memcpy(&pscdm, &lpScdm, sizeof(LPSCDM));
						memcpy(&(*pscdm).dev, &(*lpScdm).dev, sizeof(_DEVDMODE));
						bRet = TRUE;
					}
				}
			}
		}
	}

	if (lpData != NULL)
		delete[] lpData;
	//if (m_hPrinter != NULL) {
	//	// CLOSE
	//	ClosePrinter(m_hPrinter);
	//	m_hPrinter = NULL;
	//}
	return bRet;
}

// <S><A> To Fix feedback Issue #72,2023.03.17, SSDI:Chandrashekar .V
BOOL WINAPI ScPTPCMergeSCDM(LPSCDM pscdm_s, LPSCDM pscdm_d, HANDLE hPrinter, char FAR *pszPrnName)
{

// <S><D> As per SOL2 suggestion, Reverted code fix of  feedback Issue #72( Word-1 ) ,2023.03.31, SSDI:Chandrashekar .V
	//LP_EXTDMODE	pext = NULL;
	//LP_DOCDMODE	pdoc = NULL;
	//BOOL		blRet;
	//IPrintCoreHelperUni* m_pCoreHelperUni = NULL;
	//PFEATUREINFOLIST m_pFeatureInfoList;

	//short wpapersize = pscdm_s->pub.dmPaperSize;
	//SHORT Unit = pscdm_s->ext.byCusUnits;
	//DWORD Width = pscdm_s->pub.dmPaperWidth;
	//DWORD Length = pscdm_s->pub.dmPaperLength;

	//*pscdm_d = *pscdm_s;

	//PaperInformationMap paperinfomap = InitializeMap();
	//PaperInformationMap::const_iterator paperinfoiterator;

	//SHORT nDstWidthMilli = 0;
	//SHORT nDstHeightMilli = 0;
	//DWORD		dwCusWidthPixel=0;	
	//DWORD		dwCusLengthPixel=0;

	//int size = _countof(PaperSizeArray);

	//int index= findindex0fpapersize(PaperSizeArray, size, wpapersize);

	//if (index != 20)
	//{
	//	nDstWidthMilli = PaperSizeArray[index][1];
	//	nDstHeightMilli = PaperSizeArray[index][2];
	//}
	//else {

	//	if (unit_inch == Unit)
	//	{
	//		//dwCusWidthPixel = (DWORD)InchToPixel(Width, 600, MM_LOENGLISH);
	//		//nDstWidthMilli = (SHORT)PixelToMilli(dwCusWidthPixel, 600, MM_LOMETRIC);

	//		//dwCusLengthPixel = (DWORD)InchToPixel(Length, 600, MM_LOENGLISH);
	//		//nDstHeightMilli = (SHORT)PixelToMilli(dwCusLengthPixel, 600, MM_LOMETRIC);

	//		nDstWidthMilli = MilliToInch(Width, MM_LOMETRIC, MM_LOENGLISH);
	//		nDstHeightMilli = MilliToInch(Length, MM_LOMETRIC, MM_LOENGLISH);

	//		nDstWidthMilli = (SHORT)Width;

	//		nDstHeightMilli = (SHORT)Length;

	//	}
	//	else if (unit_mm == Unit)
	//	{
	//		nDstWidthMilli = (SHORT)Width;
	//		//	m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = MilliToPixel(m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli, 600, MM_LOMETRIC);

	//		nDstHeightMilli = (SHORT)Length;
	//		//	m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = MilliToPixel(m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli, 600, MM_LOMETRIC);
	//		MIP_MINMAX_CONV_PARAM stCnvParam;
	//		long	lInchWA;
	//		long	lInchLA;
	//		stCnvParam.mi = mip_mi_inch;
	//		stCnvParam.pos = 10;
	//		stCnvParam.wl = mip_wl_width;
	//		lInchWA = MilliToInchNoChk_CusPaper(Width, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);
	//		stCnvParam.wl = mip_wl_length;
	//		lInchLA = MilliToInchNoChk_CusPaper(Length, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);

	//		stCnvParam.mi = mip_mi_inch;
	//		stCnvParam.pos = 10;
	//		stCnvParam.wl = mip_wl_width;
	//		//m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = (DWORD)InchToPixel_CusPaper(lInchWA, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
	//		stCnvParam.wl = mip_wl_length;
	//		//m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = (DWORD)InchToPixel_CusPaper(lInchLA, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);

	//	}

	//	//nDstWidthMilli = 900;
	//	//nDstHeightMilli = 1480;
	//}

	//
	//paperinfoiterator = paperinfomap.find(wpapersize);

	//if (paperinfoiterator != paperinfomap.end())
	//{
	//	pscdm_d->doc.nLeftMarginPixel = paperinfoiterator->second.PageOrigin.first;
	//	pscdm_d->doc.nTopMarginPixel = paperinfoiterator->second.PageOrigin.second;
	//	pscdm_d->ext.dwDstWidthPixel = 2 * pscdm_d->doc.nLeftMarginPixel + paperinfoiterator->second.PageImageaDimensions.first;
	//	pscdm_d->ext.dwDstHeightPixel = 2 * pscdm_d->doc.nTopMarginPixel + paperinfoiterator->second.PageImageaDimensions.second;
	//	pscdm_d->ext.nDstWidthMilli = (SHORT)nDstWidthMilli;
	//	pscdm_d->ext.nDstHeightMilli = (SHORT)nDstHeightMilli;
	//}

	//SetPaperSize(pscdm_d,false);
	//pscdm_d->pub.dmPaperLength = nDstHeightMilli;
	//pscdm_d->pub.dmPaperWidth = nDstWidthMilli;
// <E>  Reverted code fix of  feedback Issue #72( Word-1 ) ,2023.03.31, SSDI:Chandrashekar .V
	
	return TRUE;
	
}
// <E> To Fix feedback Issue #72,2023.03.17, SSDI:Chandrashekar .V

//=============================================================================
// function
//      ScConvertPrintTicketToDevMode
//
// parameters
//      pszPrnName          [IN]     ÉTÅ[ÉoÅ[ñºåéÉvÉäÉìÉ^ñº
//      dwCapability        [IN]     ã@î\
//      dwVal               [IN]     ê›íËì‡óeÅiã@î\Ç≤Ç∆Ç…à”ñ°çáÇ¢Ç™àŸÇ»ÇÈÅj
//      pscdm_s             [IN/OUT] SCDMç\ë¢ëÃÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : > 0          ÉTÉ|Å[ÉgäO
//      ê¨å˜ : = 0          ê≥èÌèIóπ
//      é∏îs : < 0          ÉpÉâÉÅÅ[É^àŸèÌÇ»Ç«
//
// outline
//      PrintTicketÇÃèÓïÒÇã@î\ï Ç…ÅADEVMODEÇ…îΩâfÇ∑ÇÈÅB
//=============================================================================
long WINAPI ScConvertPrintTicketToDevMode(char FAR *pszPrnName, DWORD dwCapability, DWORD dwVal, LPSCDM pscdm_s)
{
	long	lRet = -1;
	lRet = ipifConvPrintTickettoDevMode(pszPrnName, dwCapability, dwVal, pscdm_s);
	return lRet;
}

long ipifConvPrintTickettoDevMode(char FAR *pszPrnName, DWORD dwCapability, DWORD dwVal, LPSCDM pscdm)
{
	long			lRet = -1;
	/*if (paper_121 == pscdm->dev.byMaxPaperSize)			//TBD
	{
		CShMIPCnv::SetMaxInfo(piic->pinimcf);
	}*/
	lRet = ConvPrintTickettoDevMode(pszPrnName, dwCapability, dwVal, pscdm);
	return lRet;
}

long ConvPrintTickettoDevMode(char FAR *pszPrnName, DWORD dwCapability, DWORD dwVal, LPSCDM pscdm)
{
	LPSCDM	pScdmSrc = NULL;
	LP_EXTDMODE	pext = NULL;
	long	lRet = -1;			// àŸèÌèIóπ
	BOOL	blRet = FALSE;
	DWORD	dwKind;

	dwKind = (dwCapability & ~PTPC_CAPABILITY_STRINGFLAG);
	//dwKind &= opt_ptpc_enable;
	switch (dwKind) {
	case	PTPC_CAPABILITY_STAPLE:
		// --- Staple
		blRet = ConvPrintTickettoDevModeStaple(dwVal, pscdm);
		break;

	case	PTPC_CAPABILITY_ORIENTATION:
		// --- Orientation
		blRet = ConvPrintTickettoDevModeOrientation(dwVal, pscdm);
		break;

	case	PTPC_CAPABILITY_DUPLEX:
		// --- Duplex
		blRet = ConvPrintTickettoDevModeDuplex(dwVal, pscdm);
		break;

	case	PTPC_CAPABILITY_COLLATE:
		// --- Collate
		blRet = ConvPrintTickettoDevModeCollate(dwVal, pscdm);
		break;

		case	PTPC_CAPABILITY_NUP:
		case	PTPC_CAPABILITY_NUPORDER:
		case	(PTPC_CAPABILITY_NUP | PTPC_CAPABILITY_NUPORDER):
			// --- N-Up
			blRet = ConvPrintTickettoDevModeNup(dwVal, pscdm);
			break;

		//case	PTPC_CAPABILITY_PAPERSIZE:
		//	// --- Paper Size
		//	blRet = ConvPrintTickettoDevModePaperSize(dwVal, pscdm);
		//	break;

		//case	PTPC_CAPABILITY_PAPERTYPE:
		//	// --- Paper Type
		//	blRet = ConvPrintTickettoDevModePaperType(dwVal, pscdm);
		//	break;

	case	PTPC_CAPABILITY_PAPERTRAY:
		// --- Paper Tray
		blRet = ConvPrintTickettoDevModePaperTray(dwVal, pscdm);
		break;

		//case	PTPC_CAPABILITY_PRINTMODE:
		//	// --- Print Mode
		//	blRet = ConvPrintTickettoDevModePrintMode(dwVal, pscdm);
		//	break;

		//case	PTPC_CAPABILITY_OUTPUTCOLOR:
		//	// --- Color Mode
		//	blRet = ConvPrintTickettoDevModeOutputColor(dwVal, pscdm);
		//	break;

	case	PTPC_CAPABILITY_COPIES:
		// --- Copies
		blRet = ConvPrintTickettoDevModeCopies(dwVal, pscdm);
		break;
	case	PTPC_CAPABILITY_PUNCH:
		// --- Copies
		blRet = ConvPrintTickettoDevModePunch(dwVal, pscdm);
		break;
	case	PTPC_CAPABILITY_RESOLUTION:
		//(*pscdm).ext.byResolution = (BYTE)dwVal;
		//blRet = TRUE;
		// dwVal -> index
		// mcf Ç©ÇÁÅA indexÇ†Ç§ÅAílÇéÊìæÇ∑ÇÈÅB 
	{
			(*pscdm).ext.byResolution = (BYTE)dwVal;
			blRet = TRUE;
	}
	break;
	default:
		blRet = FALSE;
		break;
	}

	if (blRet == TRUE) {
		lRet = SCPTPC_RET_SUCCESS;	// ê≥èÌèIóπ
	}
	else {
		lRet = SCPTPC_RET_NOT_SUPPORT;	// ÉTÉ|Å[ÉgäO
	}

	return lRet;
}

//=============================================================================
// function
//      ConvPrintTickettoDevModeNup
//
// parameters
//      DWORD	dwVal		PrintTichetíl
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      Print TicketÇÃílÇSCDMÇ…îΩâfÇ∑ÇÈ
//=============================================================================
BOOL ConvPrintTickettoDevModeNup(DWORD dwVal, LPSCDM pscdm)
{
	//	BOOL	blRet = TRUE;
	//
	//	return blRet;
	BOOL		blRet = FALSE;
	LPPUBDMODE	ppub = NULL;
	LP_EXTDMODE	pext = NULL;
	WORD		wNupOrder;
	WORD		wNup;
	BYTE		byNupX;
	BYTE		byNupY;
	ppub = &(*pscdm).pub;
	pext = &(*pscdm).ext;
	wNupOrder = HIWORD(dwVal);
	wNup = LOWORD(dwVal);

	// N-Up Çê›íËÇ∑ÇÈ
	switch (wNup) {
	case nup_2:
		byNupX = 2;
		byNupY = 1;
		break;

	case nup_4:
		byNupX = 2;
		byNupY = 2;
		break;

	case nup_6:
		byNupX = 3;
		byNupY = 2;
		break;

	case nup_8:
		byNupX = 4;
		byNupY = 2;
		break;

	case nup_9:
		byNupX = 3;
		byNupY = 3;
		break;

	case nup_16:
		byNupX = 4;
		byNupY = 4;
		break;

	case nup_1:
	default:
		byNupX = 1;
		byNupY = 1;
	}

	if ((*ppub).dmOrientation == DMORIENT_LANDSCAPE) {
		(*pext).byNupX = byNupY;
		(*pext).byNupY = byNupX;
	}
	else {
		(*pext).byNupX = byNupX;
		(*pext).byNupY = byNupY;
	}
	blRet = TRUE;
EXIT:
	return blRet;
}

//=============================================================================
// function
//      ConvPrintTickettoDevModeCopies
//
// parameters
//      DWORD	dwVal		PrintTichetíl
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      Print TicketÇÃílÇSCDMÇ…îΩâfÇ∑ÇÈ
//=============================================================================
BOOL ConvPrintTickettoDevModeCopies(DWORD dwVal, LPSCDM pscdm)
{
	BOOL		blRet = FALSE;
	//long		lData[SCUI_LONGDATA_MAX];
	LPPUBDMODE	ppub = NULL;

	ppub = &(*pscdm).pub;
	//<S><C>To Fix #1993-issue2 ,Dec.13.2021,SSDI:Chanchal SIngla
	if ((dwVal >= (DWORD)1) && (dwVal <= (DWORD)999)) 
	{
		(*ppub).dmCopies = (short)dwVal;
		blRet = TRUE;
	}
	else if (999 < dwVal)
	{
		(*ppub).dmCopies = MAX_COPIES;
		blRet = TRUE;
	}
	//<E>To Fix #1993-issue2 ,Dec.13.2021,SSDI:Chanchal SIngla
//EXIT:

	return blRet;
}


//=============================================================================
// function
//      ConvPrintTickettoDevModeCollate
//
// parameters
//      DWORD	dwVal		PrintTichetíl
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      Print TicketÇÃílÇSCDMÇ…îΩâfÇ∑ÇÈ
//=============================================================================
BOOL ConvPrintTickettoDevModeCollate(DWORD dwVal, LPSCDM pscdm)
{
	BOOL		blRet = FALSE;
	long		lGetLongRet;
	long		lData[PTPC_LIM_DATACNT_COLLATE];
	LPPUBDMODE	ppub = NULL;
	char		szKey[32];

	ppub = &(*pscdm).pub;

	if (dwVal == 0)
		(*ppub).dmCollate = DMCOLLATE_FALSE;
	else
		(*ppub).dmCollate = DMCOLLATE_TRUE;
	blRet = TRUE;
	return blRet;
}


//=============================================================================
// function
//      ConvPrintTickettoDevModeDuplex
//
// parameters
//      DWORD	dwVal		PrintTichetíl
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      Print TicketÇÃílÇSCDMÇ…îΩâfÇ∑ÇÈ
//=============================================================================
BOOL ConvPrintTickettoDevModeDuplex(DWORD dwVal, LPSCDM pscdm)
{
	BOOL		blRet = FALSE;
	long		lGetLongRet;
	long		lData[PTPC_LIM_DATACNT_DUPLEX];
	LP_EXTDMODE	pext = NULL;
	LPPUBDMODE	ppub = NULL;
	char		szKey[32];

	ppub = &(*pscdm).pub;
	pext = &(*pscdm).ext;
	if (dwVal == ptpc_duplex_1_Sided)
	{
		(*ppub).dmDuplex = DMDUP_SIMPLEX;
		(*pext).dmDuplexSave = DMDUP_SIMPLEX;
	}
	if (dwVal == ptpc_duplex_2_Sided_short)
	{
		(*ppub).dmDuplex = DMDUP_HORIZONTAL;
		(*pext).dmDuplexSave = DMDUP_HORIZONTAL;
	}
	if (dwVal == ptpc_duplex_2_Sided_long)
	{
		(*ppub).dmDuplex = DMDUP_VERTICAL;
		(*pext).dmDuplexSave = DMDUP_VERTICAL;
	}

//EXIT:

	return blRet;
}


//=============================================================================
// function
//      ConvPrintTickettoDevModePaperTray
//
// parameters
//      DWORD	dwVal		PrintTichetíl
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      Print TicketÇÃílÇSCDMÇ…îΩâfÇ∑ÇÈ
//=============================================================================
BOOL ConvPrintTickettoDevModePaperTray(DWORD dwVal, LPSCDM pscdm)
{
	BOOL		blRet = FALSE;
	long		lData[SCUI_LONGDATA_MAX];
	long		lIndex = 0;
	long		lCount = 0;
	long		lTmpData = 0;
	LPPUBDMODE	ppub = NULL;
	LP_EXTDMODE	pext = NULL;
	ppub = &(*pscdm).pub;
	pext = &(*pscdm).ext;
	lTmpData = 7; // auto

	switch (dwVal) {
	case ptpc_inputbin_tray1:
		lTmpData = DMBIN_TRAY1;
		break;
	case ptpc_inputbin_tray2:
		lTmpData = DMBIN_TRAY2;
		break;
	case ptpc_inputbin_tray3:
		//if (!limitPaperTray(DMBIN_TRAY3)) 
		lTmpData = DMBIN_TRAY3;
		break;
	case ptpc_inputbin_tray4:
		//if (!limitPaperTray(DMBIN_TRAY4)) 
		lTmpData = DMBIN_TRAY4;
		break;
	case ptpc_inputbin_lct:
		/*if (!limitPaperTray(DMBIN_TRAY5)) lTmpData = DMBIN_TRAY5;
		else if (!limitPaperTray(DMBIN_A3LCC)) lTmpData = DMBIN_A3LCC;
		else if (!limitPaperTray(DMBIN_A4LCC)) lTmpData = DMBIN_A4LCC;
		else if (!limitPaperTray(DMBIN_LCT)) lTmpData = DMBIN_LCT;*/
		lTmpData = DMBIN_TRAY5;
		break;
	case ptpc_inputbin_bypass:
		/*if (!limitPaperTray(DMBIN_BYPASS_A)) lTmpData = DMBIN_BYPASS_A;
		else if (!limitPaperTray(DMBIN_BYPASS_M)) lTmpData = DMBIN_BYPASS_M;
		else if (!limitPaperTray(DMBIN_BYPASS_LCT)) lTmpData = DMBIN_BYPASS_LCT;
		else if (!limitPaperTray(DMBIN_BYPASS_BODY)) lTmpData = DMBIN_BYPASS_BODY;*/
		lTmpData = DMBIN_BYPASS_A;
		break;
	case ptpc_inputbin_auto:
	default:
		break;
	}

	(*ppub).dmDefaultSource = (short)lTmpData;
	(*pext).dmDefaultSourceSave = (short)lTmpData;
	blRet = TRUE;
	return blRet;
}



BOOL ConvPrintTickettoDevModeStaple(DWORD dwVal, LPSCDM pscdm)
{
	BOOL		blRet = FALSE;
	//	BOOL		blReversePrint;
	BOOL		blPreNoneFlg = FALSE;
	long		lGetLongRet;
	long		lData[PTPC_LIM_DATACNT_STAPLE];
	LP_EXTDMODE	pext = NULL;
	LP_DOCDMODE	pdoc = NULL;
	char		szKey[32];
	long		plDataCustom = NULL;
	//BOOL		blBookletFlg = FALSE;	//<S><D>To fix SSDI bug 4409, Harika 20211213
	pext = &(*pscdm).ext;
	pdoc = &(*pscdm).doc;
	if (dwVal == ptpc_staple_none) {
		(*pext).byStaple = (BYTE)staple_none;
		blRet = TRUE;
	}
	else if (dwVal == ptpc_staple_1staple_top_left)
	{
		(*pext).byStaple = (BYTE)staple_1;
		(*pext).byBindedge = BINDEDGE_LEFT;
		blRet = TRUE;
	}
	else if (dwVal == ptpc_staple_1staple_top_right)
	{
		(*pext).byStaple = (BYTE)staple_1;
		(*pext).byBindedge = BINDEDGE_RIGHT;
		blRet = TRUE;
	}
	else if (dwVal == ptpc_staple_2staple_left)
	{
		(*pext).byStaple = (BYTE)staple_2;
		(*pext).byBindedge = BINDEDGE_LEFT;
		blRet = TRUE;
	}
	else if (dwVal == ptpc_staple_2staple_right)
	{
		(*pext).byStaple = (BYTE)staple_2;
		(*pext).byBindedge = BINDEDGE_RIGHT;
		blRet = TRUE;
	}
	else if (dwVal == ptpc_staple_2staple_top)
	{
		(*pext).byStaple = (BYTE)staple_2;
		(*pext).byBindedge = BINDEDGE_TOP;
		blRet = TRUE;
	}
	/*else if (dwVal == ptpc_staple_stapleless_top_left)
	{
		(*pext).byStaple = (BYTE)staple_1_stapleless;
	}*/
	else if (dwVal == ptpc_staple_saddle_stitch) {
		(*pext).byStaple = (BYTE)staple_saddle_staple;
		(*pdoc).dwPaperFold |= FOLD_MULTSHEET;
		(*pext).dwFieldEDM2 |= DM_2BUNCH_OCCASIONS;
		(*pext).dwFieldEDM1 &= ~DM_REVERSEPRINT;
		blRet = TRUE;
	}

	if (blRet == TRUE) {
		if ((dwVal != ptpc_staple_saddle_stitch) && (dwVal != ptpc_staple_saddle_stitch_rev)) {
			(*pdoc).dwPaperFold &= ~FOLD_MULTSHEET;
			(*pext).dwFieldEDM2 &= ~DM_2BUNCH_OCCASIONS;
			if (blPreNoneFlg == FALSE) {
				(*pext).dwFieldEDM1 &= ~DM_REVERSEPRINT;
			}
		}
		//<S><D>To fix SSDI bug 4409, Harika 20211213
		/*if (blBookletFlg == FALSE) {
			(*pext).byPamph = PAMPH_NOT;
		}*/
		//<E>To fix SSDI bug 4409, Harika 20211213
	}
	return blRet;
}
BOOL ConvPrintTickettoDevModeOrientation(DWORD dwVal, LPSCDM pscdm)
{
	BOOL		blRet = FALSE;
	long		lGetLongRet;
	long		lData[PTPC_LIM_DATACNT_ORIENTATION];
	LPPUBDMODE	ppub = NULL;
	LP_EXTDMODE	pext = NULL;
	char		szKey[32];

	ppub = &(*pscdm).pub;
	pext = &(*pscdm).ext;

	SecureZeroMemory(szKey, sizeof(szKey));
	if (dwVal == ori_port)
	{
		ppub->dmOrientation = ori_port;
		(*pext).dwFieldEDM1 &= ~DM_ROTATE180DEG;
		(*pext).dwFieldEDM2 &= ~DM_ROTATE180DEG_HW;
	}
	if (dwVal == ori_land)
	{
		ppub->dmOrientation = ori_land;
		(*pext).dwFieldEDM1 &= ~DM_ROTATE180DEG;
		(*pext).dwFieldEDM2 &= ~DM_ROTATE180DEG_HW;
	}
	if (dwVal == ori_port_rotated)
	{
		ppub->dmOrientation = ori_port;
		(*pext).dwFieldEDM1 |= DM_ROTATE180DEG;
		(*pext).dwFieldEDM2 |= DM_ROTATE180DEG_HW;
	}
	if (dwVal == ori_land_rotated)
	{
		ppub->dmOrientation = ori_land;
		(*pext).dwFieldEDM1 |= DM_ROTATE180DEG;
		(*pext).dwFieldEDM2 |= DM_ROTATE180DEG_HW;
	}
	blRet = TRUE;
	return blRet;
}

void WINAPI WriteDEVDModeValueToReg(HINSTANCE hInstance, WCHAR* pPrinterName, LPSCDM pscdm)
{
	CRegistryAPI regApi(hInstance);
	regApi.WriteData(pPrinterName, &pscdm->dev);
}
//<S><D> Since this function is not called anywhere, deleted this function to fix coverity issue 91562, Harika 20211027
//OEMDEV WINAPI GetUniDrvPrivateDevmodePTPC(LPDEVMODE pDevMode)
//{
//	//Coverity Fix 91545 - SSDI:Seetharam - 20210924
//	//Refactored the code to fix this issue
//	OEMDEV *pOemDev = NULL;
//	if (pDevMode)
//	{
//		DWORD wSize = GET_UNIDRV_PRIVATE_DEVMODE_SIZE(pDevMode);
//		if (pDevMode->dmDriverExtra > wSize)
//		{
//			BYTE * pCustDevmode = (PBYTE)(pDevMode)+(pDevMode)->dmSize + wSize;
//			pOemDev = (OEMDEV*)(pCustDevmode);
//			//OEMDEV *pOemDev = (OEMDEV*)(pCustDevmode);
//			//return *pOemDev;
//		}
//	}
//
//	return *pOemDev;
//}
//<E> Since this function is not called anywhere, deleted this function to fix coverity issue 91562, Harika 20211027

BOOL ConvPrintTickettoDevModePunch(DWORD dwVal, LPSCDM pscdm)
{
	BOOL		blRet = FALSE;
	BOOL		blPreNoneFlg = FALSE;
	LP_EXTDMODE	pext = NULL;
	LP_DOCDMODE	pdoc = NULL;


	pext = &(*pscdm).ext;
	pdoc = &(*pscdm).doc;
	if ((*pext).byPunchType != PUNCH_4HOLES_WIDE)	//<S><A>To fix SSDI issue #4636, Harika,20211222
	{
		if ((*pext).byPunchType == (BYTE)PUNCH_NONE) {
			blPreNoneFlg = TRUE;
		}
		if (dwVal == ptpc_punch_none)
		{
			(*pext).byPunchType = (BYTE)PUNCH_NONE;
			blRet = TRUE;
		}
		if (dwVal == ptpc_punch_2hole_left)
		{
			(*pext).byPunchType = (BYTE)PUNCH_2HOLES;
			(*pext).byBindedge = BINDEDGE_LEFT;
			blRet = TRUE;
		}
		if (dwVal == ptpc_punch_2hole_right)
		{
			(*pext).byPunchType = (BYTE)PUNCH_2HOLES;
			(*pext).byBindedge = BINDEDGE_RIGHT;
			blRet = TRUE;
		}
		if (dwVal == ptpc_punch_2hole_top)
		{
			(*pext).byPunchType = (BYTE)PUNCH_2HOLES;
			(*pext).byBindedge = BINDEDGE_TOP;
			blRet = TRUE;
		}
		if (dwVal == ptpc_punch_3hole_left)
		{
			(*pext).byPunchType = (BYTE)PUNCH_3HOLES;
			(*pext).byBindedge = BINDEDGE_LEFT;
			blRet = TRUE;
		}
		if (dwVal == ptpc_punch_3hole_right)
		{
			(*pext).byPunchType = (BYTE)PUNCH_3HOLES;
			(*pext).byBindedge = BINDEDGE_RIGHT;
			blRet = TRUE;
		}
		if (dwVal == ptpc_punch_3hole_top)
		{
			(*pext).byPunchType = (BYTE)PUNCH_3HOLES;
			(*pext).byBindedge = BINDEDGE_TOP;
			blRet = TRUE;
		}
		if (dwVal == ptpc_punch_4hole_left)
		{
			(*pext).byPunchType = (BYTE)PUNCH_4HOLES;
			(*pext).byBindedge = BINDEDGE_LEFT;
			blRet = TRUE;
		}
		if (dwVal == ptpc_punch_4hole_right)
		{
			(*pext).byPunchType = (BYTE)PUNCH_4HOLES;
			(*pext).byBindedge = BINDEDGE_RIGHT;
			blRet = TRUE;
		}
		if (dwVal == ptpc_punch_4hole_top)
		{
			(*pext).byPunchType = (BYTE)PUNCH_4HOLES;
			(*pext).byBindedge = BINDEDGE_TOP;
			blRet = TRUE;
		}
	}
	return blRet;
}

//=============================================================================
// function
//      ScConvertDevModeToPrintTicket
//
// parameters
//      pszPrnName          [IN] ÉTÅ[ÉoÅ[ñºåéÉvÉäÉìÉ^ñº
//      dwCapability        [IN] ã@î\
//      pdwVal              [OUT]ê›íËì‡óeÅiã@î\Ç≤Ç∆Ç…à”ñ°çáÇ¢Ç™àŸÇ»ÇÈÅj
//      pscdm_s             [IN] SCDMç\ë¢ëÃÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : > 0          ÉTÉ|Å[ÉgäO
//      ê¨å˜ : = 0          ê≥èÌèIóπ
//      é∏îs : < 0          ÉpÉâÉÅÅ[É^àŸèÌÇ»Ç«
//
// outline
//      DEVMODEÇÃì‡óeÇ©ÇÁÅAã@î\ï Ç…PrintTicketÇ…ä÷Ç∑ÇÈèÓïÒÇï‘Ç∑ÅB
//=============================================================================
long WINAPI ScConvertDevModeToPrintTicket(char FAR *pszPrnName, DWORD dwCapability, LPDWORD pdwVal, LPSCDM pscdm_s)
{
	long   lRet = -1;
	lRet = ipifConvDevModetoPrintTicket(pszPrnName, dwCapability, pdwVal, pscdm_s);
	return lRet;
}
long ipifConvDevModetoPrintTicket(char FAR *pszPrnName, DWORD dwCapability, LPDWORD pdwVal, LPSCDM pscdm)
{
	long			lRet = -1;
	lRet = ConvDevModetoPrintTicket(pszPrnName, dwCapability, pdwVal, pscdm);
	return lRet;
}
long ConvDevModetoPrintTicket(char FAR *pszPrnName, DWORD dwCapability, LPDWORD pdwVal, LPSCDM pscdm)
{
	long	lRet = -1;			// àŸèÌèIóπ
	DWORD	dwRet = 0;
	DWORD	dwKind;

	dwKind = (dwCapability & ~PTPC_CAPABILITY_STRINGFLAG);
	//dwKind &= GetCapabilityEnableFlag();
	switch (dwKind) {
	case	PTPC_CAPABILITY_STAPLE:
		// --- Staple
		dwRet = ConvDevModetoPrintTicketStaple(pscdm);
		break;
		case	PTPC_CAPABILITY_COLLATE:
			dwRet = ConvDevModetoPrintTicketCollate(pscdm);
		break;
		case	PTPC_CAPABILITY_ORIENTATION:
			// --- Orientation
			dwRet = ConvDevModetoPrintTicketOrientation(pscdm);
			break;

		case	PTPC_CAPABILITY_NUP:
		//case	PTPC_CAPABILITY_NUPORDER:
		//case	(PTPC_CAPABILITY_NUP | PTPC_CAPABILITY_NUPORDER):
			// --- N-Up
			dwRet = ConvDevModetoPrintTicketNup(pscdm);
			break;
		//case	PTPC_CAPABILITY_NUPORDER:
		//	dwRet = ConvDevModetoPrintTicketNupOrder(pscdm);
		//	break;

		case	PTPC_CAPABILITY_PAPERSIZE:
			// --- Paper Size
			dwRet = ConvDevModetoPrintTicketPaperSize(pscdm);
			break;

		//case	PTPC_CAPABILITY_PAPERTYPE:
		//	// --- Paper Type
		//	dwRet = ConvDevModetoPrintTicketPaperType(pscdm);
		//	break;

		case	PTPC_CAPABILITY_PAPERTRAY:
			// --- Paper Tray
			dwRet = ConvDevModetoPrintTicketPaperTray(pscdm);
			break;

		//	/*case	PTPC_CAPABILITY_PRINTMODE:
		//		// --- Print Mode
		//		dwRet = ConvDevModetoPrintTicketPrintMode(pscdm);
		//		break;*/

		//case	PTPC_CAPABILITY_OUTPUTCOLOR:
		//	// --- Colde Mode
		//	dwRet = ConvDevModetoPrintTicketOutputColor(pscdm);
		//	break;

		//case	PTPC_CAPABILITY_COPIES:
		//	// --- Copies
		//	dwRet = ConvDevModetoPrintTicketCopies(pscdm);
		//	break;
		case	PTPC_CAPABILITY_PUNCH:
		//	// --- Copies
			dwRet = ConvDevModetoPrintTicketPunch(pscdm);
			break;

		case	PTPC_CAPABILITY_RESOLUTION:
			dwRet = (*pscdm).ext.byResolution;
			break;
	default:
		dwRet = 0;								// ÉTÉ|Å[ÉgäO
		break;
	}

	if (dwRet == 0) {
		lRet = SCPTPC_RET_NOT_SUPPORT;			// ÉTÉ|Å[ÉgäO
	}
	else {
		lRet = SCPTPC_RET_SUCCESS;				// ê≥èÌèIóπ
	}

	*pdwVal = dwRet;

	return lRet;
}


//=============================================================================
// function
//      ConvDevModetoPrintTicketPaperSize
//
// parameters
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      DWORD	dwVal		PrintTichetíl
//
// outline
//      SCDMílÇPrint TicketílÇ…Ç∑ÇÈ
//=============================================================================
DWORD ConvDevModetoPrintTicketPaperSize(LPSCDM pscdm)
{
	//	DWORD	dwRet = ptpc_papersize_notsupport;
	//
	//	return dwRet;
	DWORD		dwRet = ptpc_papersize_notsupport;
	LPPUBDMODE	ppub = NULL;

	ppub = &(*pscdm).pub;
	dwRet = (*ppub).dmPaperSize;

	return dwRet;
}


DWORD ConvDevModetoPrintTicketCollate(LPSCDM pscdm)
{
	DWORD		dwRet = ptpc_collate_notsupport;
	LPPUBDMODE	ppub = NULL;
	short		dmCollate;
	ppub = &(*pscdm).pub;

	dmCollate = (*ppub).dmCollate;
	if (pscdm->ext.byPamph > 0)
	{
		if (dmCollate == DMCOLLATE_FALSE)
		{
			(*ppub).dmCollate = DMCOLLATE_TRUE;
		}
	}
	else
	{
		(*ppub).dmCollate = dmCollate;
	}
	dwRet = (*ppub).dmCollate;
	return dwRet;
}

//=============================================================================
// function
//      ConvDevModetoPrintTicketPaperTray
//
// parameters
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      DWORD	dwVal		PrintTichetíl
//
// outline
//      SCDMílÇPrint TicketílÇ…Ç∑ÇÈ
//=============================================================================
DWORD ConvDevModetoPrintTicketPaperTray(LPSCDM pscdm)
{
	DWORD		dwRet = ptpc_papertray_notsupport;
	LPPUBDMODE	ppub = NULL;
	if (pscdm == NULL)
		goto EXIT;
	ppub = &(*pscdm).pub;
	//dwRet = (*ppub).dmDefaultSource;
	switch ((*ppub).dmDefaultSource) {
	case DMBIN_TRAY1:
		dwRet = ptpc_inputbin_tray1;
		break;
	case DMBIN_TRAY2:
		dwRet = ptpc_inputbin_tray2;
		break;
	case DMBIN_TRAY3:
		dwRet = ptpc_inputbin_tray3;
		break;
	case DMBIN_TRAY4:
		dwRet = ptpc_inputbin_tray4;
		break;
	//case DMBIN_TRAY3_4TRAYLC:
	//	dwRet = ptpc_inputbin_tray3;
	//	break;
	//case DMBIN_TRAY4_4TRAYLC:
	//	dwRet = ptpc_inputbin_tray4;
	//	break;
	/*case DMBIN_TRAY5:
	case DMBIN_A3LCC:
	case DMBIN_A4LCC:
	case DMBIN_LCT:
		dwRet = ptpc_inputbin_lct;
		break;*/
	case DMBIN_BYPASS_A:
	//case DMBIN_BYPASS_M:
	/*case DMBIN_BYPASS_LCT:
	case DMBIN_BYPASS_BODY:*/
		dwRet = ptpc_inputbin_bypass;
		break;
	default:
		dwRet = ptpc_inputbin_auto;
		break;
	}

EXIT:
	//<E> To support Universal Print UI ,  Redmine #2978 , 2021.08.21, SSDI:Chandrashekar .V
	return dwRet;
}


//=============================================================================
// function
//      ConvDevModetoPrintTicketStaple
//
// parameters
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      DWORD	dwVal		PrintTichetíl
//
// outline
//      SCDMílÇPrint TicketílÇ…Ç∑ÇÈ
//=============================================================================
DWORD ConvDevModetoPrintTicketStaple(LPSCDM pscdm)
{
	DWORD		dwRet = ptpc_staple_notsupport;
	long		lCount = -1;
	long		lIndex = 0;
	long		lGetLongRet;
	long		lData[PTPC_LIM_DATACNT_STAPLE];
	LP_EXTDMODE	pext = NULL;
	LP_DOCDMODE	pdoc = NULL;
	char		szKey[32];
	BOOL		blReversePrint;
	DWORD		dwPaperFold;
	BYTE		byStaple;
	BYTE		byBindedge;
	BYTE		byPamph;

	pext = &(*pscdm).ext;
	pdoc = &(*pscdm).doc;

	byStaple = (*pext).byStaple;
	byBindedge = (*pext).byBindedge;
	byPamph = (*pext).byPamph;
	dwPaperFold = (*pdoc).dwPaperFold;
	if ((*pext).dwFieldEDM1 & DM_REVERSEPRINT) {
		blReversePrint = TRUE;
	}
	else {
		blReversePrint = FALSE;
	}
	dwRet = lIndex;
	if (byStaple == STAPLE_TWO)
	{
		if (byBindedge == binedge_left) {
			dwRet = ptpc_staple_2staple_left;
		}
		if (byBindedge == binedge_right) {
			dwRet = ptpc_staple_2staple_right;
		}
		if (byBindedge == binedge_top) {
			dwRet = ptpc_staple_2staple_top;
		}
	}
	else if (byStaple == STAPLE_NONE)
		dwRet = ptpc_staple_none;
	else if (byStaple == STAPLE_ONE)
	{
		if (byBindedge == binedge_left) {
			dwRet = ptpc_staple_1staple_top_left;
		}
		if (byBindedge == binedge_right) {
			dwRet = ptpc_staple_1staple_top_right;
		}
		if (byBindedge == binedge_top) {
			dwRet = ptpc_staple_1staple_top_left;
		}
	}
	else if (byStaple == STAPLE_SADDLE)
	{
		dwRet = ptpc_staple_saddle_stitch;
	}
	else if (byStaple == STAPLE_STAPLELESS)
	{
		dwRet = ptpc_staple_stapleless_top_left;
	}
	
/*if (IsOptStapleCustom()) {
	if (IsOptStapleCustomValue((long)dwRet)) {
		dwRet = ptpc_staple_custom;
	}
}*/

EXIT:
	return dwRet;
}

//=============================================================================
// function
//      ConvDevModetoPrintTicketPunch
//
// parameters
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      DWORD	dwVal		PrintTichetíl
//
// outline
//      SCDMílÇPrint TicketílÇ…Ç∑ÇÈ
//=============================================================================
DWORD ConvDevModetoPrintTicketPunch(LPSCDM pscdm)
{
	DWORD		dwRet = ptpc_punch_notsupport;
	long		lCount = -1;
	long		lIndex = 0;
	long		lGetLongRet;
	long		lData[PTPC_LIM_DATACNT_PUNCH];
	LP_EXTDMODE	pext = NULL;
	LP_DOCDMODE	pdoc = NULL;
	char		szKey[32];
	BYTE		byPunch;
	BYTE		byBindedge;
	DWORD		dwPaperFold;
	BYTE		byPamph;
	BOOL		blReversePrint;

	pext = &(*pscdm).ext;
	pdoc = &(*pscdm).doc;

	byPunch = (*pext).byPunchType;
	byBindedge = (*pext).byBindedge;
	byPamph = (*pext).byPamph;
	dwPaperFold = (*pdoc).dwPaperFold;
	if ((*pext).dwFieldEDM1 & DM_REVERSEPRINT) {
		blReversePrint = TRUE;
	}
	else {
		blReversePrint = FALSE;
	}
	if (byPunch != PUNCH_4HOLES_WIDE)	//<S><A>To fix SSDI issue #4636, Harika,20211222
	{
		dwRet = lIndex;
		if (byPunch == PUNCH_2HOLES)
		{
			if (byBindedge == binedge_left) {
				dwRet = ptpc_punch_2hole_left;
			}
			if (byBindedge == binedge_right) {
				dwRet = ptpc_punch_2hole_right;
			}
			if (byBindedge == binedge_top) {
				dwRet = ptpc_punch_2hole_top;
			}
		}
		else if (byPunch == PUNCH_NONE)
			dwRet = ptpc_staple_none;
		else if (byPunch == PUNCH_4HOLES)
		{
			if (byBindedge == binedge_left) {
				dwRet = ptpc_punch_4hole_left;
			}
			if (byBindedge == binedge_right) {
				dwRet = ptpc_punch_4hole_right;
			}
			if (byBindedge == binedge_top) {
				dwRet = ptpc_punch_4hole_top;
			}
		}
		else if (byPunch == PUNCH_3HOLES)
		{
			if (byBindedge == binedge_left) {
				dwRet = ptpc_punch_3hole_left;
			}
			if (byBindedge == binedge_right) {
				dwRet = ptpc_punch_3hole_right;
			}
			if (byBindedge == binedge_top) {
				dwRet = ptpc_punch_3hole_top;
			}
		}
	}
EXIT:

	return dwRet;
}

//=============================================================================
// function
//      ConvDevModetoPrintTicketNup
//
// parameters
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      DWORD	dwVal		PrintTichetíl
//
// outline
//      SCDMílÇPrint TicketílÇ…Ç∑ÇÈ
//=============================================================================
DWORD ConvDevModetoPrintTicketNup(LPSCDM pscdm)
{
	DWORD	dwRet = ptpc_nup_notsupport;
	LP_EXTDMODE	pext = NULL;
	WORD		wNup = 0;
	pext = &(*pscdm).ext;
	// --- wNup
	//<S><A> To implement constraing between ZOOM and NUP on MetroUI.	Harika , 20220223
	if (pext->dwFieldEDM1 & DM_XYZOOM)
	{
		(*pext).byNupX = 1;
		(*pext).byNupY = 1;
	}
	else
	//<E> To implement constraing between ZOOM and NUP on MetroUI.	Harika , 20220223
	wNup = (WORD)((*pext).byNupX * (*pext).byNupY);
	
EXIT:
	dwRet = wNup;
	return dwRet;
}

//=============================================================================
// function
//      ConvDevModetoPrintTicketOrientation
//
// parameters
//      LPSCDM	pscdm		SCDMÉAÉhÉåÉX
//
// return value
//      DWORD	dwVal		PrintTichetíl
//
// outline
//      SCDMílÇPrint TicketílÇ…Ç∑ÇÈ
//=============================================================================
DWORD ConvDevModetoPrintTicketOrientation(LPSCDM pscdm)
{
	DWORD		dwRet = ptpc_orientation_notsupport;
	long		lCount = -1;
	long		lIndex = 0;
	long		lGetLongRet;
	long		lData[PTPC_LIM_DATACNT_ORIENTATION];
	LPPUBDMODE	ppub = NULL;
	LP_EXTDMODE	pext = NULL;
	long		lR180Deg;
	char		szKey[32];
	short		dmOrientation;


	ppub = &(*pscdm).pub;
	pext = &(*pscdm).ext;
	if ((*pext).dwFieldEDM1 & DM_ROTATE180DEG) {
		lR180Deg = 1;
	}
	else {
		lR180Deg = 0;
	}

	//Coverity Fix 91559 - SSDI:Seetharam - 20211006
	dmOrientation = ori_port; //assigning default value
	if(((*ppub).dmOrientation== ori_port) && lR180Deg==0)
		dmOrientation = ori_port;
	else if (((*ppub).dmOrientation == ori_land) && lR180Deg == 0)
		dmOrientation = ori_land;
	else if (((*ppub).dmOrientation == ori_port) && lR180Deg == 1)
		dmOrientation = ori_port_rotated;
	else if (((*ppub).dmOrientation == ori_land) && lR180Deg == 1)
		dmOrientation = ori_land_rotated;

	if (dmOrientation == ori_port)
	{
		dwRet = ori_port;
	}
	if (dmOrientation == ori_land)
	{
		dwRet = ori_land;
	}
	if (dmOrientation == ori_port_rotated)
	{
		dwRet = ori_port_rotated;
	}
	if (dmOrientation == ori_land_rotated)
	{
		dwRet = ori_land_rotated;
	}
EXIT:

	return dwRet;
}
//<S><A>Added To Fix Silent AutoConfiguration doesn't run from installer for a Enhanced Mode Driver,2021.29.09, SSDI:Chanchal Singla
//=============================================================================
// function
//      SearchSelectMFP
//
// parameters
//      WCHAR* pPrinterName, 
//		LONG uCurrentMFPIndex
//
// return value
//        SHORt
//
// outline
//      ÉSearchSelectMFP
//=============================================================================
SHORT WINAPI SearchSelectMFP(WCHAR* pPrinterName, LONG uCurrentMFPIndex, BOOL bMfpIndex)
{
	SHORT 	lRet = IDCANCEL;
	if (bMfpIndex == TRUE)// this means that this call is from Installer
	{

		HMODULE hStringResourceHandle = NULL;
		HMODULE hBitmapResourceHandle = NULL;
		HMODULE	hDialogResourceHandle = NULL;
		
		WCHAR			szDll[MAX_PATH] = { 0 };
	
		// Load String Resource
		GetStringResource(pPrinterName, szDll);
		hStringResourceHandle = ::LoadLibrary(szDll);
		SecureZeroMemory(&szDll, sizeof(szDll));
		// Load Dialog Resource
		GetDialogResource(pPrinterName, szDll);
		hDialogResourceHandle = ::LoadLibrary(szDll);
		SecureZeroMemory(&szDll, sizeof(szDll));
		// Load BitMap Resource
		GetBitmapResource(pPrinterName, szDll);
		hBitmapResourceHandle = ::LoadLibrary(szDll);
		SecureZeroMemory(&szDll, sizeof(szDll));

		long Count = 0;
		long CurrentINdex = 0;
		long previousMFPIndex = 0;

		CRegistryAPI regApi(ghInstance);
		SCDM pscdm_drv;
		SecureZeroMemory(&pscdm_drv, sizeof(SCDM));
		OEMDEV PrivateDevmode;
		SecureZeroMemory(&PrivateDevmode, sizeof(OEMDEV));
		HANDLE			hPrinter = NULL;
		PRINTER_DEFAULTS	pd;
		pd.pDatatype = NULL;
		pd.pDevMode = NULL;
		pd.DesiredAccess = PRINTER_ACCESS_USE;
		if (hPrinter == NULL)
		{
			if (sh_OpenPrinter(pPrinterName, &hPrinter, &pd) == FALSE)
			{
				//Coverity Fix: 91561, 91555, 91552 - SSDI Seetharam - 20211006
				//return FALSE;
				goto EXIT;
			}
		}
		regApi.GetPrivateDevmode(hPrinter, ghInstance, pPrinterName, &pscdm_drv);
		regApi.ReadData(pPrinterName, &pscdm_drv.dev);
		memcpy(&PrivateDevmode.scpi, &pscdm_drv, sizeof(SCDM));
	
		previousMFPIndex = uCurrentMFPIndex;

		regApi.GetRegisterMFPCountAndCheckPos(pPrinterName, Count, CurrentINdex);
		PrivateDevmode.scpi.dev.nTargetDeviceID = CurrentINdex;
		lRet = (SHORT)ExecuteAutoConfiguration(pPrinterName, hPrinter, hStringResourceHandle, hBitmapResourceHandle, hDialogResourceHandle, 
			&PrivateDevmode, CurrentINdex, previousMFPIndex, bMfpIndex);
		regApi.Set_WM_MFP_INDEX_CHANGE(pPrinterName, FALSE);

EXIT:
		if (NULL != hStringResourceHandle)
		{
			FreeLibrary(hStringResourceHandle);
			hStringResourceHandle = NULL;
		}
		if (NULL != hBitmapResourceHandle)
		{
			FreeLibrary(hBitmapResourceHandle);
			hBitmapResourceHandle = NULL;
		}
		if (NULL != hDialogResourceHandle)
		{
			FreeLibrary(hDialogResourceHandle);
			hDialogResourceHandle = NULL;
		}
	}
	return lRet;
}
//<E>Added To Fix Silent AutoConfiguration doesn't run from installer for a Enhanced Mode Driver,2021.29.09, SSDI:Chanchal Singla
//<S><A>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
//In Basic mode Printing Preferences is not displayed as per the Language set
DWORD WINAPI scGetLangID(WCHAR* pszSvrPrnName, DWORD* dwLangID)
{
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	CShIniFile cIniFile(ghInstance, pszSvrPrnName, szCommonDatFilePath, FALSE);

	SHORT dwLanguageID = -1;
	CShRegMdlTbl RegMdlTbl(ghInstance, pszSvrPrnName, &cIniFile);
	RegMdlTbl.GetLangID(&dwLanguageID);
	*dwLangID = dwLanguageID;
	return dwLanguageID;
}
//<E>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla	
//<S><A>To Fix Redmine Bug #4186,,2022.04.26,SSDI:Chanchal Singla
//Configuration Tab : Silent auto configuration fails when driver is installed through installer.
BOOL WINAPI ExecuteAutoConf(HINSTANCE hInst, WCHAR *pPrinterName, long lMode, BOOL SilAutoConf)
{
	BOOL			blRet = FALSE;

	long			lEndFlag = dev_state_timeout;

	short			wPort = 0;

	short			wPortWow = 0;

	BOOL			blUpdate = FALSE;

	BOOL			blUseACM = TRUE;

	PROPSTATE pps;
	SecureZeroMemory(&pps, sizeof(PROPSTATE));

	_DEVDMODE pdev;
	SecureZeroMemory(&pdev, sizeof(_DEVDMODE));
	FillDevDModeData(&pdev);

	CShRegACMIB		*preg = NULL;

	CShIniFile			*pmcf = NULL;
	CRegistryAPI cRegAPI;
	if (pPrinterName == NULL)
		return blRet;

	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");

	pmcf = new CShIniFile(hInst, pPrinterName, szCommonDatFilePath, FALSE);
	if ((pmcf == NULL))
	{
		return blRet;
	}
	//<S><C>To Implement Task#3132,31-05-2024,SSDI:Manoj S
	//preg = new CShRegACMIB(hInst, pPrinterName, pmcf);
	preg = new CShRegACMIB(hInst, pPrinterName, pmcf, FeatureInfoList);
	//<E>To Implement Task #3132,31-05-2024,SSDI:Manoj S
	if ((preg == NULL))
	{
		if (pmcf != NULL)
		{
			delete pmcf;
			pmcf = NULL;
		}
		return blRet;
	}

	blUseACM = (*pmcf).IsUseACM();

	(*preg).SetUsersEnable(FALSE);

	cRegAPI.SetCallACMStatus(pPrinterName, TRUE);

	//(*preg).SetAutoConfig(FALSE);

	wPort = (*preg).GetPortNumber(pPrinterName);

	wPortWow = (*preg).GetPortNumberWow6432();

	(*preg).SetPortNumber(wPort, wPortWow);

	// ================================================
	// _/_/_/  AutoConfiguration
	// ================================================
	blRet = (*preg).Config(wPort, lMode);
	if(SilAutoConf == TRUE)
	{	
		if ((*preg).CreateData(wPort, TRUE) == TRUE)
		{
			blRet = TRUE;
			// Motive: In case of when we install a  driver in basic mode with any valid printer Model (IP/Host ID )that time in registry "PrinterDriverData"
			//	L"AutoConfigStatus" key should be update to True(Logical One) value.
			CRegistryAPI cRegAPI;
			cRegAPI.SetAutoConfigStatus(pPrinterName, TRUE);
			cRegAPI.SetCallACMStatus(pPrinterName, FALSE);
			(*preg).SetUsersEnable(TRUE);
		}
	}
EXIT:

	if (preg != NULL)
	{
		delete preg;
		preg = NULL;
	}
	if (pmcf != NULL)
	{
		delete pmcf;
		pmcf = NULL;
	}
	return blRet;
}
BOOL WINAPI ExecuteSilentAutoConf(HINSTANCE hInst, WCHAR *pPrinterName, long lMode)
{
	short			wPort = 0;
	BOOL			blUpdate = FALSE;
	BOOL			blRet = FALSE;
	long			glTimerLoop = 0;
	long			lInterval = 0;
	CShRegACMIB		*preg = NULL;

	CShIniFile		*pmcf = NULL;

	if (pPrinterName == NULL)
		return blRet;

	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");

	pmcf = new CShIniFile(hInst, pPrinterName, szCommonDatFilePath, FALSE);
	if ((pmcf == NULL))
	{
		return blRet;
	}

	lInterval = (*pmcf).GetLong(MCF_SEC_AUTOCONFIG, MCF_KEY_AC_TIMER_INTERVAL, SCUI_AC_TIMER_INTVL_DEF);
	if (lInterval != 0)
	{
		glTimerLoop = (*pmcf).GetLong(MCF_SEC_AUTOCONFIG, MCF_KEY_AC_TIMER_TIMEOUT, SCUI_AC_TIMER_TMOUT_DEF) / lInterval;
	}
	//<S><A>To Implement Task#3132,31-05-2024,SSDI:Manoj S
	//preg = new CShRegACMIB(hInst, pPrinterName, pmcf);
	preg = new CShRegACMIB(hInst, pPrinterName, pmcf, FeatureInfoList);
	//<E>To Implement Task #3132,31-05-2024,SSDI:Manoj S
	if ((preg == NULL))
	{
		if (pmcf != NULL)
		{
			delete pmcf;
			pmcf = NULL;
		}
		return FALSE;
	}

	wPort = (*preg).GetPortNumber(pPrinterName);

	CShRegST pregST(hInst, pPrinterName, pmcf);
	while (TRUE)
	{
		if ((pregST).IsRequired(wPort, TRUE) == FALSE)
		{
			if ((*preg).CheckUpdating(wPort, TRUE) == 0)
			{
				blUpdate = TRUE;
				break;
			}
		}
		glTimerLoop--;
		if (glTimerLoop < 0)
		{
			blRet = FALSE;
			break;
		}
	}
	//(*preg).SetRequired(wPort, FALSE);

	if (blUpdate == FALSE)
		goto EXIT;

	if ((*preg).CreateData(wPort, TRUE) == TRUE)
	{
		blRet = TRUE;
		// Motive: In case of when we install a  driver in basic mode with any valid printer Model (IP/Host ID )that time in registry "PrinterDriverData"
		//	L"AutoConfigStatus" key should be update to True(Logical One) value.
		CRegistryAPI cRegAPI;
		cRegAPI.SetAutoConfigStatus(pPrinterName, TRUE);
		cRegAPI.SetCallACMStatus(pPrinterName, FALSE);
		(*preg).SetUsersEnable(TRUE);
	}
EXIT:

	if (preg != NULL)
	{
		delete preg;
		preg = NULL;
	}
	if (pmcf != NULL)
	{
		delete pmcf;
		pmcf = NULL;
	}
	return blRet;
}
//<E>To Fix Redmine Bug #4186:- Configuration Tab : Silent auto configuration fails when driver is installed through installer.,2022.04.26,SSDI:Chanchal Singla
