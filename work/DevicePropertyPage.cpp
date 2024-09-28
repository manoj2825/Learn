//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : DevicePropertyPage.cpp
//
//  Module        : PrinterUi Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Sujan
//
//  Date		  : 27 September 2013
//
//	Description   : Implementation of DevicePropertyPage class

//===========================================================================

//===========================================================================
//  Reviews 
//
//  Sl#     Review Date     Reviewer(s)               Remarks
//===========================================================================

#include "precomp.h"
#include <HtmlHelp.h>
//For Logging Purpose
#include "MFPLogger.h"
#include "shregmt.h"
#include "sh_dlg_spn.h"
#include "shregus.h"
#include "shJsonus.h"

/*++
Routine Name:

    CDevicePropertyPage::CDevicePropertyPage

Routine Description:

    Contructor Routine for CDevicePropertyPage class.

Arguments:

    hStringResourceHandle - handle to string resource dl
	hPrinter - handle to Printer.
	hDiscoveryStringResourceHandle - handle to Discovery String Resource Handle.

Return Value:

   None
   
--*/

typedef BOOL (__cdecl *pfnShowDiscoveryUI)(WCHAR* pszSvrPrnName,HMODULE hStringDllHandle,BOOL bIsDevicePropertySheetEvent,
	long& uCurrentMFPIndex, DLGMFPSLCT_INFO *mfpSlctInfo, PSCDM pscdm_drv);

// <S><M> Modified the constructor for additiona parameters required for ComboBox, 20200414 - SSDI:Seetharam
CDevicePropertyPage::CDevicePropertyPage(HANDLE hPrinter,HMODULE hStringResourceHandle,HMODULE hBitmapResourceHandle,HMODULE hDialogResourceHandle,
										 WCHAR* pPrinterName,BOOL bIsPrinterIcon,
										 LPTSTR pHelpFilePath, DWORD *pHelpID, 
										 PFEATUREINFOLIST &pFeatureInfoList,
										 std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings,
										 ResourceIDInfoMap* pResourceIDInfoMap,
										 IPrintCoreHelperUni* pPrintCoreHelperUni, HINSTANCE hInstance, SHORT& wUsIndex, PPROPINFO ppi):m_hPage(NULL),
// <S><E> Modified the constructor for additiona parameters required for ComboBox, 20200414 - SSDI:Seetharam
														 m_hComPropSheet(NULL),
														 m_pfnComPropSheet(NULL),
														 m_hStringResourceHandle(hStringResourceHandle),
														 m_hBitmapResourceHandle(hBitmapResourceHandle),
														 m_hDialogResourceHandle(hDialogResourceHandle),
														 m_hPrinter(hPrinter),
                                                         m_bIsPrinterIcon(bIsPrinterIcon),
														 m_pHelpFilePath(pHelpFilePath),
														 m_pHelpID(pHelpID),
														 m_pResourceIDInfoMap(pResourceIDInfoMap),
														 m_pFeatureInfoList(pFeatureInfoList),
														 m_DefaultSettings(pDefaultSettings), 
														 m_pPrintCoreHelperUni(pPrintCoreHelperUni)
{
    ::ZeroMemory(m_pPrinterName,sizeof(m_pPrinterName));
    wcscpy_s(m_pPrinterName,_countof(m_pPrinterName),pPrinterName);
	long lCount = 0;
	long lIndex = -1;
	CRegistryAPI regApi(ghInstance);
	regApi.GetRegisterMFPCountAndCheckPos(m_pPrinterName,lCount,lIndex);
    m_uPreviousItemIndex = lIndex;
    m_uCurrentItemIndex = lIndex;
	g_CurrentIndex = lIndex;
    m_dwLangID = 0;
	m_dwColorMode = 0;
	m_DeviceType = eColor;
// <S><A> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V	
	//Fix coverity #91412,20210603,SSDI:Chanchal Singla
	//OEMDEV* m_pOemDevMode = NULL;
	SecureZeroMemory(&m_pOemDevMode, sizeof(OEMDEV));
	SecureZeroMemory(&m_OemPublicDevMode, sizeof(DEVMODE));
	SecureZeroMemory(&m_PropertySheetState, sizeof(PROPSTATE));
// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
	m_hPropertySheetHandle = NULL;
	//Coverity Fix 90486 - SSDI:Seetharam - 20200827
	m_ObjCShowMessage = NULL;
	m_hInstance = hInstance;
	m_pmcf = NULL;
	InitCommonFile(m_hInstance);

	m_bBWPrint = FALSE;
	m_dwSetupType = eBasicMode;
	m_bDefaultClicked = FALSE;
	m_bColorBWSelected = FALSE;
	m_bExecuteSilentAutoConfig = FALSE;
	m_MfpSlctInfo = NULL;
	m_pUsIndex = &wUsIndex;
	m_ppi = ppi;
	//Coverity Fix: 91412 - SSDI Seetharam : 20210503
	m_dwIsPrinterIcon = FALSE;
	CheckPrinterIcon(m_pPrinterName);
	//Coverity Fix: 91412 - SSDI Seetharam : 20210705
	m_bDisablePrnSel = FALSE;
	m_bIsSilentAutoConfig = FALSE;
}


/*++
Routine Name:

    CDevicePropertyPage::~CDevicePropertyPage

Routine Description:

    Destructor Routine for CDevicePropertyPage class.

Arguments:

    None

Return Value:

    None
   
--*/
CDevicePropertyPage::~CDevicePropertyPage(void)
{
	 DestroyUIComponents();
	 if (m_pmcf != NULL)
	 {
		 delete m_pmcf;
		 m_pmcf = NULL;
	 }
	 //SSDI Bug Fix:3549-SSDI:Seetharam-20210408
	 DeleteMfpSlctInfo();
}	


/*++

Routine Name:

    CDocumentPropertyPage::PropPageInit

Routine Description:

    Initialises the Property Sheet and sets the Property Sheet Handle

Arguments:

    pPSUIInfo - Pointer to PPROPSHEETUI_INFO.
 
Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CDevicePropertyPage::PropPageInit(
    _In_ CONST PPROPSHEETUI_INFO pPSUIInfo
    )
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(NULL == pPSUIInfo)
	{
		slog.putLog("CDevicePropertyPage::PropPageInit -> E_FAIL(pPSUIInfo)\n");
		return E_FAIL;
	}
// <S><A> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V	
	SetDevmode(m_hPrinter, &m_pOemDevMode);
	//<S><C> Apply Button was not enabling first time when changing any feature in config tab,
	// only when driver install through installer in basic mode with valid ip address,19052022,SSDI:Chanchal Singla
	CRegistryAPI regApi(m_hInstance);
	regApi.ReadData(m_pPrinterName, &m_pOemDevMode.scpi.dev);
	DevmodeToPropstate();
	//<E> Apply Button was not enabling first time when changing any feature in config tab,
	// only when driver install through installer in basic mode with valid ip address,19052022,SSDI:Chanchal Singla
// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
	ResetPublicDevmode(&m_OemPublicDevMode, m_pPrinterName);
	UpdateFeatureListBasedOnDevMode(&m_pOemDevMode);
	//
	// Only proceed if we have effectively published the helper interfaces to the UI
	// control objects.
	//
	// We also need to retrieve the dialog template resource and dialog title from the
	// derived property page class.
	//
	PROPSHEETPAGE page = {0};
	if (SUCCEEDED(hr = InitDlgBox(&page.pszTemplate, &page.pszTitle)))
	{
		page.dwSize = sizeof(PROPSHEETPAGE);
		page.dwFlags = PSP_DEFAULT | PSP_USETITLE;
		page.hInstance = m_hDialogResourceHandle;
		/*Redmine #1064 ::Sujan::To Enable Visual Style For UI When Opened From PrintManagement*/
		page.pfnDlgProc = CDevicePropertyPage::DlgProc;
		page.hActCtx = GetMyActivationContext(m_hDialogResourceHandle);
		if( (NULL != page.hActCtx) && (INVALID_HANDLE_VALUE != page.hActCtx))
		{
			page.dwFlags |= PSP_USEFUSIONCONTEXT;
		}
		else
		{
			slog.putLog("CDevicePropertyPage::PropPageInit -> INVALID_HANDLE_VALUE\n");
		}
		page.lParam = reinterpret_cast<LPARAM>(this);

		pPSUIInfo->Result = pPSUIInfo->pfnComPropSheet(pPSUIInfo->hComPropSheet,
			CPSFUNC_ADD_PROPSHEETPAGE,
			reinterpret_cast<LPARAM>(&page),
			0);

		if (SUCCEEDED(hr = SetComPropSheetFunc(pPSUIInfo->pfnComPropSheet)) &&
			SUCCEEDED(hr = SetPageHandle(reinterpret_cast<HANDLE>(pPSUIInfo->Result))))
		{
			hr = SetComPropSheetHandle(pPSUIInfo->hComPropSheet);

		}
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	return hr;
}

// <S><A> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
/*=============================================================================
Routine Name:
	CDevicePropertyPage::SetDevmode
Routine Description:
	Sets the private Devmode.
Arguments:
	hPrinter - handle to printer.
	pOemDevMode - private devmode structure
Return Value:
	VOID
--=============================================================================*/
VOID CDevicePropertyPage::SetDevmode(HANDLE hPrinter, OEMDEV	*pOemDevMode)
{
	// get device type from registry
	eDeviceType sDevicetype = eColor;
	CRegistryAPI cRegApi(m_hInstance);
	cRegApi.GetDeviceType(m_pPrinterName, sDevicetype);
	PRINTER_INFO_2	*pInfo = NULL;
	DWORD			dwNeeded = 0;

	GetPrinter(hPrinter, 2, NULL, 0, &dwNeeded);
	if (NULL == dwNeeded)
		return ;
	pInfo = (PRINTER_INFO_2 *)new BYTE[dwNeeded];
	if (NULL == pInfo)
		return ;
	SecureZeroMemory(pInfo, dwNeeded);
	if (FALSE == GetPrinter(hPrinter, 2, (LPBYTE)pInfo, dwNeeded, &dwNeeded))
	{
		if (NULL != pInfo)
		{
			delete[] pInfo;
			pInfo = NULL;
		}
		return ;
	}
	//Update devmode for Mono device
	OEMDEV *pOemDev1 = NULL;
	if (pInfo->pDevMode)
	{
		DWORD wSize = GET_UNIDRV_PRIVATE_DEVMODE_SIZE(pInfo->pDevMode);
		if (pInfo->pDevMode->dmDriverExtra > wSize)
		{
			BYTE  *  pCustDevmode = (PBYTE)(pInfo->pDevMode) + (pInfo->pDevMode)->dmSize + wSize;
			pOemDev1 = (OEMDEV*)(pCustDevmode);
			//pOemDevMode = (OEMDEV*)(pCustDevmode);
			//::memcpy(pOemDevMode, (OEMDEV*)pCustDevmode, sizeof(POEMDEV));
			//memcpy_s(&pOemDevMode, sizeof(OEMDEV), &pOemDev1, sizeof(OEMDEV));

			int i = 0;
			if (NULL != pOemDev1)
			{

				int x = 0;
				//  To do : to be handle for devicetype
				if (eMono  == sDevicetype)
				{
					pOemDev1->scpi.wFieldSPI = MONO_MODEL_ID;
					pOemDev1->scpi.pub.dmColor = DMCOLOR_MONOCHROME;
					pOemDev1->scpi.pub.dmICMMethod = DMICMMETHOD_NONE;
					pOemDev1->scpi.ext.ColData.byColMode = colmode_mono;
				}
				else
				{
					BOOL bMonoDeviceType = (MONO_MODEL_ID == pOemDev1->scpi.wFieldSPI) ? TRUE : FALSE;

					if (bMonoDeviceType)
					{
						pOemDev1->scpi.pub.dmColor = DMCOLOR_COLOR;
						pOemDev1->scpi.pub.dmICMMethod = DMICMMETHOD_DEVICE;
						pOemDev1->scpi.ext.ColData.byColMode = colmode_auto;
					}
					pOemDev1->scpi.wFieldSPI = COLOR_MODEL_ID;
				}
				pInfo->pDevMode->dmColor = pOemDev1->scpi.pub.dmColor;
				pInfo->pDevMode->dmICMMethod = pOemDev1->scpi.pub.dmICMMethod;
			}
		}
	}

		::SetPrinter(hPrinter, 2, (LPBYTE)pInfo, 0);

	if (NULL != pOemDev1)
	{
		//pOemDevMode->scpi.dev.byFieldVDMStaple = 2;
		//pOemDevMode->scpi.dev.byFieldVDMStaple = pOemDev1->scpi.dev.byFieldVDMStaple;
		memcpy_s(pOemDevMode, sizeof(OEMDEV), pOemDev1, sizeof(OEMDEV));
		//::memcpy(&pOemDevMode, &pOemDev1, sizeof(pInfo->pDevMode));
	}

	//*pOemDevMode = *pOemDev1;

	if (NULL != pInfo)
	{
		delete[] pInfo;
		pInfo = NULL;
	}

	//return *pOemDevMode;
	
}

// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
/*++

Routine Name:

    CDevicePropertyPage::SetComPropSheetFunc

Routine Description:

    Store the pointer to the PFNCOMPROPSHEET function.

Arguments:

    pfnComPropSheet - pointer to the PFNCOMPROPSHEET function.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CDevicePropertyPage::SetComPropSheetFunc(
    _In_ CONST PFNCOMPROPSHEET pfnComPropSheet
    )
{
    HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

    ASSERTMSG(pfnComPropSheet != NULL, "NULL pointer to common propert sheet functions.\n");

    if (NULL != pfnComPropSheet)
    {
        m_pfnComPropSheet = pfnComPropSheet;
    }
	else
	{
		slog.putLog("CDevicePropertyPage::SetComPropSheetFunc -> NULL_POINTER(pfnComPropSheet)\n");
	}
    
    return hr;
}

/*++

Routine Name:

    CDevicePropertyPage::SetPageHandle

Routine Description:

    Store the handle of a property page window.

Arguments:

    hPage - Handle of a property page window.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CDevicePropertyPage::SetPageHandle(
    _In_ CONST HANDLE hPage
    )
{
    HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

    if (NULL != hPage)
    {
        m_hPage = hPage;
    }
	else
	{
		slog.putLog("CDevicePropertyPage::SetPageHandle -> NULL_HANDLE(hPage)\n");
	}

   return hr;
}

/*++

Routine Name:

    CDevicePropertyPage::SetComPropSheetHandle

Routine Description:

    Store the handle of the Common Property Sheet window.

Arguments:

    hComPropSheet - Handle of the common property sheet window.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CDevicePropertyPage::SetComPropSheetHandle(
    _In_ CONST HANDLE hComPropSheet
    )
{
    HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

    if (NULL != hComPropSheet)
    {
        m_hComPropSheet = hComPropSheet;
    }
	else
	{
		slog.putLog("CDevicePropertyPage::SetComPropSheetHandle -> NULL_HANDLE(hComPropSheet)\n");
	}

     return hr;
}

/*++

Routine Name:

    CDevicePropertyPage::GetComPropSheetFunc

Routine Description:

    Retrieve the pointer of the PFNCOMPROPSHEET function.

Arguments:

    ppfnComPropSheet - Address of a pointer that will be filled out with the PFNCOMPROPSHEET function.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CDevicePropertyPage::GetComPropSheetFunc(
    _Outptr_ PFNCOMPROPSHEET* ppfnComPropSheet
    )
{
    HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

    if (NULL !=ppfnComPropSheet)
    {
        *ppfnComPropSheet = m_pfnComPropSheet;
    }
	else
	{
		slog.putLog("CDevicePropertyPage::GetComPropSheetFunc -> NULL_POINTER(ppfnComPropSheet)\n");
	}

    return hr;
}

/*++

Routine Name:

    CDevicePropertyPage::GetPageHandle

Routine Description:

    Retrieves the property page handle.

Arguments:

    None

Return Value:

    Handle to the property page

--*/
HANDLE
CDevicePropertyPage::GetPageHandle(
    VOID
    )
{
    return m_hPage;
}

/*++

Routine Name:

    CDevicePropertyPage::GetComPropSheetHandle

Routine Description:

    Retrieves the common property sheet handle

Arguments:

    None

Return Value:

    Handle to the common property sheet

--*/
HANDLE
CDevicePropertyPage::GetComPropSheetHandle(
    VOID
    )
{
    return m_hComPropSheet;
}

INT_PTR CALLBACK
CDevicePropertyPage::DlgProc(
    _In_ CONST HWND   hDlg,
    _In_ CONST UINT   uiMessage,
    _In_ CONST WPARAM wParam,
    _In_ CONST LPARAM lParam
    )
{
    HRESULT hr = S_OK;	
	//MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	switch (uiMessage)
    {
        case WM_INITDIALOG:
        {
            //
            // Store the class instance
            //
            PROPSHEETPAGE* pPage = reinterpret_cast<PROPSHEETPAGE*>(lParam);
			if( (NULL !=pPage) && (NULL != pPage->lParam))
			{
				CDevicePropertyPage* pPropertyPageInstance = reinterpret_cast<CDevicePropertyPage*>(pPage->lParam);
				if(NULL != pPropertyPageInstance)
				{
					if (SUCCEEDED(hr = pPropertyPageInstance->StorePropertyPageInstance(hDlg)))
					{
						hr = pPropertyPageInstance->InitializeUIControls(hDlg);
					}
				}
				else
                {
					//slog.putLog("CDevicePropertyPage::DlgProc -> E_FAIL\n");
                    hr = E_FAIL;
                }				
			}
		}
		break;
		// <S><A> To add owner ondraw for handling the custom controls like combox with image etc., 20200414 - SSDI:Seetharam
		case WM_DRAWITEM:
		{
			CDevicePropertyPage* pPropertyPageInstance = NULL;

			if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
				(NULL != pPropertyPageInstance))
			{
				pPropertyPageInstance->OnWmDrawItem(hDlg, wParam, lParam);
			}
		}
		break;
		// <S><E> To add owner ondraw for handling the custom controls like combox with image etc., 20200414 - SSDI:Seetharam
		case WM_COMMAND:
        {
            switch (HIWORD(wParam))
            {
                case BN_CLICKED:
                case CBN_SELCHANGE:
				{
                    CDevicePropertyPage* pPropertyPageInstance;

                    if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
					{
						if(NULL != pPropertyPageInstance)
						{
							 hr = pPropertyPageInstance->SendCommand(hDlg, wParam);
							 pPropertyPageInstance->UpdateMimic();
						}
						if(S_OK != hr)
						{
							//slog.LoggingErrorHResultValues();
						}
					}
				}
				break;
				 default:
                {
                    //
                    // Unhandled command so return TRUE
                    //
					hr = S_OK;
                }
                break;
			}
		}
		break;
		case WM_NOTIFY:
        {
            NMHDR* pHdr = reinterpret_cast<NMHDR*>(lParam);
		
           if ((NULL != pHdr) && (NULL != pHdr->code))
            {
                switch (pHdr->code)
                {
                    case PSN_SETACTIVE:
                    {
                        CDevicePropertyPage* pPropertyPageInstance = NULL;

                        if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
                           (NULL != pPropertyPageInstance))
                        {
                            pPropertyPageInstance->PsnSetActive(hDlg);
							pPropertyPageInstance->UpdateMimic();
                        }
                        //
                        // Return FALSE to accept the page activation
                        //
                    }
                    break;
					case PSN_APPLY:
                    {
                        CDevicePropertyPage* pPropertyPageInstance = NULL;

                        if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
                            (NULL != pPropertyPageInstance))
                        {
                            pPropertyPageInstance->PsnApply(hDlg);
                        }

						PFNCOMPROPSHEET pfnComPropSheet = NULL;
						 if (SUCCEEDED(hr) &&  SUCCEEDED(hr = pPropertyPageInstance->GetComPropSheetFunc(&pfnComPropSheet)) &&
						   (NULL != pPropertyPageInstance)
						   )
                        {
							 PropSheet_UnChanged(GetParent(hDlg), hDlg);
							 pfnComPropSheet(pPropertyPageInstance->GetComPropSheetHandle(),
                                                CPSFUNC_SET_RESULT,
                                                reinterpret_cast<LPARAM>(pPropertyPageInstance->GetPageHandle()),
                                                (LPARAM)CPSUI_OK);
						}
						 hr = S_OK;
					}
					break;
				}
			}
		}
		break;
		 case WM_NCDESTROY:
        {
            CDevicePropertyPage* pPropertyPageInstance;

            if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
                (NULL !=pPropertyPageInstance))
            {
                hr = pPropertyPageInstance->RemovePropertyPageInstance(hDlg);
            }
			if(S_OK != hr)
			{
				//slog.LoggingErrorHResultValues(); 
			}

       }
       break;
		 case WM_HELP:
			 {
				 CDevicePropertyPage* pPropertyPageInstance =NULL;

				 if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
					 (NULL !=pPropertyPageInstance))
				 {
					 LPHELPINFO pHelpInformation = (LPHELPINFO)lParam;
					 hr = pPropertyPageInstance->ShowHelp(pHelpInformation);
				 }
				 if(S_OK != hr)
				 {
					 //slog.LoggingErrorHResultValues();
				 }
			 }
			 break;
	}
	return hr ;
}

//<S><A>To The Support Of Mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
VOID CDevicePropertyPage::UpdateMimic(VOID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_SHARED_BBOX);
	if (Iterator != m_UIControls.end())
	{
		CMimicImageConfig *pMimic = static_cast<CMimicImageConfig*>(Iterator->second);
		if (pMimic)
		{
			pMimic->Draw(&m_PropertySheetState);
		}
		else
		{
			slog.putLog("CDevicePropertyPage::UpdateMimic -> NULL_OBJECT(CMimicImageConfig)\n");
		}
	}

	Iterator = m_UIControls.find(IDC_BTN_SHARED_BBOX_SUB);
	if (Iterator != m_UIControls.end())
	{
		CMimicText *pMimicText = static_cast<CMimicText*>(Iterator->second);
		if (pMimicText)
		{
			pMimicText->Draw(&m_PropertySheetState);
		}
		else
		{
			slog.putLog("CDevicePropertyPage::UpdateMimic -> NULL_OBJECT(CMimicText)\n");
		}
	}
	
}
//<E>To The Support Of Mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
/*++

Routine Name:

    CDevicePropertyPage::StorePropertyPageInstance

Routine Description:

    Store the Property Sheet Handle

Arguments:

    hDlg - Handle to the dialog.
 
Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT CDevicePropertyPage::StorePropertyPageInstance(  _In_ CONST HWND hDlg )
{
 	HRESULT hr = S_OK;

	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

    if (!SetProp(hDlg, MAKEINTATOM(ID_XDSMPL_DLG_ATOM), reinterpret_cast<HANDLE>(this)))
    {
		slog.putLog("CDevicePropertyPage::StorePropertyPageInstance -> E_FAIL\n");
        hr = E_FAIL;
    }
	return hr;
}

/*++

Routine Name:

    CDevicePropertyPage::CDevicePropertyPage::RetrievePropertyPageInstance(

Routine Description:

	Retrieve the PropertyPageInstance Handle

Arguments:

    hDlg - Handle of the property page Window associated with this class.
    pDocPropPage - Address of a pointer to be filled out with the instance of this class.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/

HRESULT
CDevicePropertyPage::RetrievePropertyPageInstance(
    _In_        CONST HWND     hDlg,
    _Outptr_ CDevicePropertyPage** pDocPropPage
    )
{
    HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if((NULL !=hDlg)&&(NULL !=pDocPropPage))
	{
		*pDocPropPage = reinterpret_cast<CDevicePropertyPage*>(GetProp(hDlg, MAKEINTATOM(ID_XDSMPL_DLG_ATOM)));
	}
	if( NULL == pDocPropPage)
	{
		slog.putLog("CDevicePropertyPage::RetrievePropertyPageInstance -> INVALID_DATA_HANDLE(pDocPropPage)\n");
	}
    return hr;
}

/*++

Routine Name:

    CDevicePropertyPage::RemovePropertyPageInstance

Routine Description:

    Removes the PropertyPageInstance associated with the class.

Arguments:

    hDlg - Handle of the property page Window associated with this class.
  
Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT CDevicePropertyPage::RemovePropertyPageInstance(
        _In_ CONST HWND hDlg
        )
{
    HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

    if (NULL != hDlg)
    {
        RemoveProp(hDlg, MAKEINTATOM(ID_XDSMPL_DLG_ATOM));
    }
	else
	{
		slog.putLog("CDevicePropertyPage::RemovePropertyPageInstance -> NULL_HANDLE(hDlg)\n");
	}

    return hr;
}

/*++

Routine Name:

    CDevicePropertyPage::SendCommand

Routine Description:

    Send Control Command associated with the Property Page.

Arguments:

    hDlg - Handle of the property page Window associated with this class.
	wParam - To Identify the Control that generated the command.
  
Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/


HRESULT
CDevicePropertyPage::SendCommand(
    _In_ CONST HWND   hDlg,
    _In_ CONST WPARAM wParam
    )
{
    //
    // Use the wParam as the index into the control map and
    // inform the control that generated the command
    //
    HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	m_bDefaultClicked = FALSE;
	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	
    switch (HIWORD(wParam))
    {
		case BN_CLICKED:
		// <S><A> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
		{
			switch (LOWORD(wParam))
			{
				case IDC_CHK_STAPLE_STAPLELESS:
				{
					pControl = (*pComponents)[IDC_CHK_STAPLE_STAPLELESS];
					((CCheckBox*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CCheckBox*)pControl)->OnBnClicked(wParam, hDlg);

					m_PropertySheetState.byStapleless = IsDlgButtonChecked(hDlg, IDC_CHK_STAPLE_STAPLELESS);

					if (m_PropertySheetState.byStapleless)
					{
						m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_STAPLELESS;
					}
					else
					{
						m_pOemDevMode.scpi.dev.byFieldVDMStaple &= ~DM_VDMS_STAPLELESS;
					}
//<S><C>To The Support of Mimic Control,20201412,SSDi:Chanchal Singla
					if (m_PropertySheetState.byStapleless == 0 && m_pOemDevMode.scpi.ext.byStaple == staple_1_stapleless)
					{
						m_PropertySheetState.wStaple = staple_none;
					}
					else if (m_PropertySheetState.byStapleless == 1 && m_pOemDevMode.scpi.ext.byStaple == staple_1_stapleless)
					{
						m_PropertySheetState.wStaple = m_pOemDevMode.scpi.ext.byStaple;
					}
//<E>To The Support of Mimic Control,20201412,SSDi:Chanchal Singla
					//PropSheet_Changed(GetParent(hDlg), hDlg);
				}
				break;
				case IDC_CHK_SINGLE_SIGNON:
				{
					pControl = (*pComponents)[IDC_CHK_SINGLE_SIGNON];
					((CCheckBox*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CCheckBox*)pControl)->OnBnClicked(wParam, hDlg);

					short wPreSingleSignOn = m_PropertySheetState.wSingleSignOn;
					m_PropertySheetState.wSingleSignOn = IsDlgButtonChecked(hDlg, IDC_CHK_SINGLE_SIGNON);
					if (m_PropertySheetState.wSingleSignOn == bool_true && wPreSingleSignOn == bool_false)
					{
						OnIdcClickSPN(hDlg);
					}
					
					if (m_PropertySheetState.wSingleSignOn)
					{
						m_pOemDevMode.scpi.dev.dwFieldVDM |= DM_SINGLESIGNON;
					}
					else
					{
						m_pOemDevMode.scpi.dev.dwFieldVDM  &= ~DM_SINGLESIGNON;
					}
					//PropSheet_Changed(GetParent(hDlg), hDlg);
				}
				break;
				case IDC_CHK_AUTO_COLORMODE:
				{
					pControl = (*pComponents)[IDC_CHK_AUTO_COLORMODE];
					((CCheckBox*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CCheckBox*)pControl)->OnBnClicked(wParam, hDlg);

					m_PropertySheetState.byAutoColorMode = IsDlgButtonChecked(hDlg,IDC_CHK_AUTO_COLORMODE) ? 0 : 1;
					m_pOemDevMode.scpi.dev.dwFieldVDM = ( IsDlgButtonChecked(hDlg, IDC_CHK_AUTO_COLORMODE)) ?
						(m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_VDM_COLORMODE_AUTO_DISABLE) : (m_pOemDevMode.scpi.dev.dwFieldVDM | DM_VDM_COLORMODE_AUTO_DISABLE);
				//	PropSheet_Changed(GetParent(hDlg), hDlg);
				}
				break;
				case IDC_CHK_PIN_COMPATIBILITY:
				{
					pControl = (*pComponents)[IDC_CHK_PIN_COMPATIBILITY];
					((CCheckBox*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CCheckBox*)pControl)->OnBnClicked(wParam, hDlg);

					m_PropertySheetState.byPinCompatibility = IsDlgButtonChecked(hDlg,IDC_CHK_PIN_COMPATIBILITY) ? 1 : 0;
					m_pOemDevMode.scpi.dev.dwFieldVDM = (IsDlgButtonChecked(hDlg,IDC_CHK_PIN_COMPATIBILITY)) ?
						(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_VDM_PIN_CLEAN_TEXT) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_VDM_PIN_CLEAN_TEXT);
			
					//PropSheet_Changed(GetParent(hDlg), hDlg);
				}
				break;
				//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
				case IDC_CHK_SHARE_FAV:
				{
					pControl = (*pComponents)[IDC_CHK_SHARE_FAV];
					((CCheckBox*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CCheckBox*)pControl)->OnBnClicked(wParam, hDlg);
					m_PropertySheetState.bShare = IsDlgButtonChecked(hDlg, IDC_CHK_SHARE_FAV) ? 1 : 0;
				}
				break;
				//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
				case IDC_BTN_EXTRA_SETTINGS:
				{
					CreatePPLCYDlg(hDlg);
				}
				break;
				case IDC_BTN_AUTO_CONF:
				{
					OnIdcBtnPrnSelCfg(hDlg);
					PropSheet_Changed(GetParent(hDlg), hDlg);
				}
				break;
			}
		}
// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
    case CBN_SELCHANGE:
        {
            switch(LOWORD(wParam))
            {
            case IDC_BTN_ABOUT:
                {
                    CAboutDialog *pdlg = NULL;
                    pdlg = new CAboutDialog(IDD_DLG_ABOUT,m_hStringResourceHandle,m_hBitmapResourceHandle,m_hPrinter,m_hDialogResourceHandle);
					if( NULL == pdlg)
					{
						slog.putLog("CDevicePropertyPage::SendCommand -> NULL_OBJECT(pdlg)\n");
					}

                    INT_PTR DlgRet = IDCANCEL;
                    DlgRet = ::DialogBoxParam(m_hDialogResourceHandle,MAKEINTRESOURCE(IDD_DLG_ABOUT),hDlg,(DLGPROC)(*pdlg).WindowMapProc, (LPARAM)pdlg);
					if( NULL == DlgRet || -1 == DlgRet)
					{
						slog.putLog("CDevicePropertyPage::SendCommand -> INVALID_POINTER(DlgRet)\n");
					}

                    delete pdlg;
                    pdlg = NULL;
                }
                break;
            case IDC_BTN_PRNSEL:
                {
					OnIdcBtnPrnClicked(hDlg);
                }
                break;
            
            case IDC_CMB_PRNSEL:
                {
                    eDeviceType wDeviceType = eColor;

					// <S><C> SSDI Bug #3736 - Changed the impelentaion based on index to item based for sorted order  - SSDI Seetharam : 20210621
					if (m_MfpSlctInfo == NULL)
					{
						CreateMfpSlctInfo();
					}
                    long lCheckPos = m_MfpSlctInfo->lCheckIndex;
                    //long lCount = 0;
                    //CRegistryAPI regApi(m_hInstance);
                    //regApi.GetRegisterMFPCountAndCheckPos(m_pPrinterName,lCount,lCheckPos);

					UIControlMap* pComponents = NULL;
					pComponents = &m_UIControls;
					CPrinterSelectionCombo *pControl = (CPrinterSelectionCombo *)m_UIControls[IDC_CMB_PRNSEL];
					m_MfpSlctInfo->lCheckIndex = pControl->GetSelVal(hDlg);
					if (lCheckPos != m_MfpSlctInfo->lCheckIndex)
					{
						g_CurrentIndex = m_MfpSlctInfo->lCheckIndex;
						pControl->Select(IDC_CMB_PRNSEL, hDlg, m_MfpSlctInfo->lCheckIndex);
						m_bExecuteSilentAutoConfig = TRUE;
						PropSheet_Changed(GetParent(hDlg), hDlg);
					}
					//pControl->ResetPrinterCombo(hDlg, m_MfpSlctInfo);

                    //SetPreviousItemIndex(lCheckPos);
                    //long uChangedOptionIndex = static_cast<LONG>(SendDlgItemMessage(hDlg, IDC_CMB_PRNSEL, CB_GETCURSEL, 0, 0));
					//int uChangedOptionIndex = pControl->GetSelVal(hDlg);
                    //  SetCurrentItemIndex(uChangedOptionIndex);
					// <S><C> SSDI Bug #3736 - Changed the impelentaion based on index to item based for sorted order  - SSDI Seetharam : 20210621
                }
                break;
                case IDC_CMB_LANGSEL:
                {
                    DWORD dwLangID = static_cast<WORD>(SendDlgItemMessage(hDlg, IDC_CMB_LANGSEL, CB_GETCURSEL, 0, 0));
                    
                    if (dwLangID != m_dwLangID)
                    {
                        PropSheet_Changed(GetParent(hDlg), hDlg);
                        SendDlgItemMessage(hDlg, IDC_CMB_LANGSEL, CB_SETCURSEL, dwLangID, 0);
                    }
                }
                break;
// <S><A> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
				case IDC_CMB_STAPLE_OPT:
				{
					pControl = (*pComponents)[IDC_CMB_STAPLE_OPT];
					((CShImageCombo*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CShImageCombo*)pControl)->OnSelChange(LOWORD(wParam), hDlg);
					//<S><A>to Fix Bug#2255 Issue 2,23-06-2020,SSDI:Chachal Parkash
						DWORD lIndex = m_PropertySheetState.wStapleOption;
					//<E>to Fix Bug#2255 Issue 2,23-06-2020,SSDI:Chachal Parkash
					    m_PropertySheetState.wStapleOption = static_cast<WORD>(SendDlgItemMessage(hDlg, IDC_CMB_STAPLE_OPT, CB_GETCURSEL, 0, 0));
						
						m_pOemDevMode.scpi.dev.byFieldVDMStaple &= ~(DM_VDMS_1STAPLE | DM_VDMS_2STAPLE | DM_VDMS_SADDLE);

						switch (m_PropertySheetState.wStapleOption)
						{
						case stapleopt_1:
							m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_1STAPLE;
							break;
						case stapleopt_2:
							m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_1STAPLE;
							m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_2STAPLE;

							break;
						case stapleopt_3:
							m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_1STAPLE;
							m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_2STAPLE;
							m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_SADDLE;
							break;
						default:
							break;

						}
	
// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
						if ((0 == m_PropertySheetState.wStapleOption) || (1 == m_PropertySheetState.wStapleOption))
						{
							SetControlState(hDlg, IDC_CHK_STAPLE_STAPLELESS, SCUI_STATE_SHOW);
							m_pOemDevMode.scpi.dev.byFieldVDMStaple &= ~DM_VDMS_STAPLELESS;
							m_PropertySheetState.byStapleless = 0;
						}
						else
						{
							SetControlState(hDlg, IDC_CHK_STAPLE_STAPLELESS, SCUI_STATE_ON);
							//m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_STAPLELESS;
						}
// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
					//<S><A>to Fix Bug#2255 Issue 2,23-06-2020,SSDI:Chachal Parkash
						/*if (lIndex != m_PropertySheetState.wStapleOption)
						{
							PropSheet_Changed(GetParent(hDlg), hDlg);
							SendDlgItemMessage(hDlg, IDC_CMB_STAPLE_OPT, CB_SETCURSEL, m_PropertySheetState.wStapleOption, 0);
						}*/
					//<E>to Fix Bug#2255 Issue 2,23-06-2020,SSDI:Chachal Parkash
				}
				break;
				case IDC_CMB_COLORMODULE:
				{
					pControl = (*pComponents)[IDC_CMB_COLORMODULE];
					((CShImageCombo*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CShImageCombo*)pControl)->OnSelChange(LOWORD(wParam), hDlg);
					DWORD lIndex = m_PropertySheetState.wColorOption;
					m_PropertySheetState.wColorOption = static_cast<WORD>(SendDlgItemMessage(hDlg, IDC_CMB_COLORMODULE, CB_GETCURSEL, 0, 0));

					if (0 == m_PropertySheetState.wColorOption) // color
					{
						m_pOemDevMode.scpi.dev.byFieldVDM |= DM_VDM_COLORMODEL;
					}
					else    // black and white
					{
						m_pOemDevMode.scpi.dev.byFieldVDM &= ~DM_VDM_COLORMODEL;
					}
					//<S><C>To Fix Redmine Bug #4651 issue 2 ,2022.02.01,SSDI:Chanchal Singla
					//Issue 2:Apply button is disabled
					if (lIndex != m_PropertySheetState.wColorOption)
					{
						PropSheet_Changed(GetParent(hDlg), hDlg);
					}
					//<E>To Fix Redmine Bug #4651 issue 2 ,2022.02.01,SSDI:Chanchal Singla
				}
				break;
				case IDC_CMB_PUNCH_OPT:
				{
					pControl = (*pComponents)[IDC_CMB_PUNCH_OPT];
					((CShImageCombo*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CShImageCombo*)pControl)->OnSelChange(LOWORD(wParam), hDlg);
					DWORD lIndex = m_PropertySheetState.wPunchOption;
					m_PropertySheetState.wPunchOption = static_cast<WORD>(SendDlgItemMessage(hDlg, IDC_CMB_PUNCH_OPT, CB_GETCURSEL, 0, 0));
				//<S><C>update the devmode similar to ud3.1,20201412,Chanchal Singla	
					//m_pOemDevMode.scpi.dev.byPunchmodule = m_PropertySheetState.wPunchOption;
					m_pOemDevMode.scpi.dev.byPunchmodule = (BYTE)UpdatePropstateToPunchModule();
				//<E>update the devmode similar to ud3.1,20201412,Chanchal Singla
					/*if(lIndex != m_PropertySheetState.wPunchOption)
					{
						PropSheet_Changed(GetParent(hDlg), hDlg);
						SendDlgItemMessage(hDlg, IDC_CMB_PUNCH_OPT, CB_SETCURSEL, m_PropertySheetState.wPunchOption, 0);
					}*/
				}
				break;
				case IDC_CMB_PRN_RELEASE:
				{
					pControl = (*pComponents)[IDC_CMB_PRN_RELEASE];
					((CShImageCombo*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CShImageCombo*)pControl)->OnSelChange(LOWORD(wParam), hDlg);
					DWORD lIndex = m_PropertySheetState.byPrintRelease;
					m_PropertySheetState.byPrintRelease = static_cast<WORD>(SendDlgItemMessage(hDlg, IDC_CMB_PRN_RELEASE, CB_GETCURSEL, 0, 0));

					if (1 == m_PropertySheetState.byPrintRelease)
					{
						m_pOemDevMode.scpi.dev.byFieldVDM |= DM_VDM_NON_PRITNRLEASE;
						//<S><D>To Fix Redmine #7483,31-05-2024,SSDI:Manoj S
						//<S><A> To fix bug No. 2856, Harika	20201021
						//m_PropertySheetState.wPPlcyPrintRelease = bool_false;
						//m_pOemDevMode.scpi.dev.dwFieldVDM &= ~DM_PP_PRINTRELEASE;
						//<S><A> To fix bug No. 2856, Harika	20201021
						//<E>To Fix Redmine #7483,31-05-2024,SSDI:Manoj S
					}
					else
					{
						m_pOemDevMode.scpi.dev.byFieldVDM &= ~DM_VDM_NON_PRITNRLEASE;
					}
					/*if (lIndex != m_PropertySheetState.byPrintRelease)
					{
						PropSheet_Changed(GetParent(hDlg), hDlg);
						SendDlgItemMessage(hDlg, IDC_CMB_PRN_RELEASE, CB_SETCURSEL, m_PropertySheetState.byPrintRelease, 0);
					}*/
				}
				break;
				case IDC_CMB_MAXPAPERSIZE:
				{
					pControl = (*pComponents)[IDC_CMB_MAXPAPERSIZE];
					((CShImageCombo*)pControl)->Set_isDevicePropertyPage(TRUE);
					((CShImageCombo*)pControl)->OnSelChange(LOWORD(wParam), hDlg);
					DWORD lIndex = m_PropertySheetState.byMaxPaperSize;
					m_PropertySheetState.byMaxPaperSize = static_cast<WORD>(SendDlgItemMessage(hDlg, IDC_CMB_MAXPAPERSIZE, CB_GETCURSEL, 0, 0));
					if (m_PropertySheetState.byMaxPaperSize == bool_false)
					{
						m_pOemDevMode.scpi.dev.byMaxPaperSize = paper_120;
					}
					else
					{
						m_pOemDevMode.scpi.dev.byMaxPaperSize = paper_121;
					}
					//m_pOemDevMode.scpi.dev.byMaxPaperSize = m_PropertySheetState.byMaxPaperSize;
					/*if (lIndex != m_PropertySheetState.byMaxPaperSize)
					{
						PropSheet_Changed(GetParent(hDlg), hDlg);
						SendDlgItemMessage(hDlg, IDC_CMB_MAXPAPERSIZE, CB_SETCURSEL, m_PropertySheetState.byMaxPaperSize, 0);
					}*/
				}
				break;
// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
// <S><A> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V
				case IDC_BTN_SHARED_DEFAULT:
				{
					SetTabDefaults(hDlg);
				}
				break;
// <E> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V
        default:
        {
			slog.putLog("CDevicePropertyPage::SendCommand -> NOT_IMPLEMENTED\n");
            hr = E_NOTIMPL;
        }
        break;
    }
        }
        break;
    default:
        {
			slog.putLog("CDevicePropertyPage::SendCommand -> NOT_IMPLEMENTED\n");
            hr = E_NOTIMPL;
        }
        break;
    }
	if(S_OK != hr)
	{
		slog.LoggingErrorHResultValues();
	}
	SetTabState(hDlg);
     return hr;
}
// <S><A> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V
HRESULT CDevicePropertyPage::SetTabDefaults(_In_ CONST HWND   hDlg)
{
	m_bDefaultClicked = TRUE;
	m_bIsSilentAutoConfig = FALSE;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	//<S><C>To Fix Redmine Bug #4272,2021.September.09, SSDI:Chanchal Singla
	//Configuration tab values are resetting to Defaults even though grayed out when Defaults button is clicked
	CRegistryAPI regApi;
	//The controls on Configuration Tab are not grayed out even though
	//the user does not have the authority of Managed Printer.
	bool bReset = false;
	BOOL blOperatable = IsPrinterOperators(m_pPrinterName);
	if (blOperatable)
	{
		bReset = true;
	}
	if (bReset)
	{
		//<S><C> Apply Button was not enabling first time when changing any feature in config tab,
		// only when driver install through installer in basic mode with valid ip address,19052022,SSDI:Chanchal Singla
		if ((eBasicMode == m_dwSetupType) && (bIsSilentAutoConfig() == TRUE))
		{
			m_bIsSilentAutoConfig = TRUE;
			CRegistryAPI regApi;
			P_DEVDMODE		pdevDefault = NULL;
			CUIControl pControl;

			pdevDefault = new _DEVDMODE; // Default data is containing for a Configuration tab
			if (pdevDefault != NULL)
			{
				SecureZeroMemory(pdevDefault, sizeof(_DEVDMODE));
				FillDevDModeData(pdevDefault);
				regApi.ReadSilentDevmodeDataFromRegistry(pdevDefault, m_pPrinterName);
				//<E>Fix for the defaults issue when silent autoconfiguration executes. Harika
				if (FALSE == pControl.IsEqualBuf(&m_pOemDevMode.scpi.dev, pdevDefault, sizeof(_DEVDMODE)))
				{
					PropSheet_Changed(GetParent(hDlg), hDlg);
				}
				delete pdevDefault;
				pdevDefault = NULL;
			}

		}
		else
		{
			UIControlMap* pComponents = NULL;
			pComponents = &m_UIControls;
			UIControlMap::iterator iterUIComponents = pComponents->begin();

			while (iterUIComponents != pComponents->end())
			{
				CUIControl* pControl = iterUIComponents->second;
				if (NULL != pControl)
				{
					if (FAILED(hr = pControl->SetTabDefaults(iterUIComponents->first, hDlg)))
					{
						if (S_OK != hr)
						{
							slog.LoggingErrorHResultValues();
						}
						break;
					}
					iterUIComponents++;
					//NotifyConstrainedFeatures(hDlg,pControl,iterUIComponents.first,TRUE);

				}
			}
		}
		//<E> Apply Button was not enabling first time when changing any feature in config tab,
		// only when driver install through installer in basic mode with valid ip address,19052022,SSDI:Chanchal Singla
		//To set default values in Printing policy Dialog
		OEMDEV m_ptempPrivateDevmode = {};
		memcpy_s(&m_ptempPrivateDevmode, sizeof(OEMDEV), &m_pOemDevMode, sizeof(OEMDEV));
		CShDlgPntPolicy *pdlgPntPlcy = new CShDlgPntPolicy(IDD_DLG_PRINTING_POLICY, m_hStringResourceHandle, m_hInstance,
			m_hBitmapResourceHandle, m_hPrinter, m_hDialogResourceHandle, m_pFeatureInfoList, m_DefaultSettings,
			m_pResourceIDInfoMap, &m_ptempPrivateDevmode, &m_PropertySheetState, m_ObjCShowMessage, m_pPrinterName);
		if (NULL == pdlgPntPlcy)
		{
			slog.putLog("CMainTabPropertyPage::OnZoomSettings -> NULL_POINTER(IDD_DLG_ZOOM_11)\n");
			return FALSE;
		}
		pdlgPntPlcy->CreateCtrlClass();
		pdlgPntPlcy->SetTabDefaults(hDlg);

		//<S><A>To fix redmine bug #3156 ,20210713 SSDI:Goutham
		regApi.GetBWPRint(m_pPrinterName, m_bBWPrint);
		//<S><E>To fix redmine bug #3156 ,20210713 SSDI:Goutham

		ResetDevDmode(hDlg);
		//<S><C> Apply Button was not enabling first time when changing any feature in config tab,
		// only when driver install through installer in basic mode with valid ip address,19052022,SSDI:Chanchal Singla
		if (m_bIsSilentAutoConfig == FALSE)
		{
			PropstatetoDevmode();
		}
		//<E> Apply Button was not enabling first time when changing any feature in config tab,
		// only when driver install through installer in basic mode with valid ip address,19052022,SSDI:Chanchal Singla
		delete pdlgPntPlcy;
		pdlgPntPlcy = NULL;
	}
	//<S><C>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
	//In Basic mode Printing Preferences is not displayed as per the Language set
	if (eEnhancedMode == m_dwSetupType)
	{
		//TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
		//GetProjectFileName(szCommonDatFilePath, L"Common.DAT");

		//CShIniFile cshLangFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
		//DWORD dwDefLangID = cshLangFile.GetDefaultLangID();
		//<S><C>To Fix SSDI Bug #2069,26-06-2020,SSDI: Chanchal Parkash
		CShRegMdlTbl RegMdlTbl(ghInstance, m_pPrinterName, m_pmcf);
		short dwDefLangID = RegMdlTbl.GetDefaultLanguage(m_dwSetupType);
		DWORD dwLangID = static_cast<DWORD>(SendDlgItemMessage(hDlg, IDC_CMB_LANGSEL, CB_GETCURSEL, 0, 0));
		if (dwLangID != dwDefLangID)
		{
			regApi.SetLangID(m_pPrinterName, dwDefLangID);
			PropSheet_Changed(GetParent(hDlg), hDlg);
		}
		//<E>To Fix SSDI Bug #2069,26-06-2020,SSDI: Chanchal Parkash
		SendDlgItemMessage(hDlg, IDC_CMB_LANGSEL, CB_SETCURSEL, dwDefLangID, 0);
		//<E>To Fix Redmine Bug #4272,2021.September.09, SSDI:Chanchal Singla
	}

	//<E>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla

	return hr;
}
// <E> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V


    /*++

Routine Name:

    CFeaturePropPage::InitDlgBox

Routine Description:

    Provides the base class with the data required to intialise the dialog box.

Arguments:

    ppszTemplate - Pointer to dialog box template to be intialised.
    ppszTitle    - Pointer to dialog box title to be intialised.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CDevicePropertyPage::InitDlgBox(
    _Out_ LPCTSTR* ppszTemplate,
    _Out_ LPCTSTR* ppszTitle
    )
{
    HRESULT hr = S_OK;

	//Coverity Fix 90525 - SSDI:Seetharam - 20201020
	//Changed the condition from || to && because if any parameter is NULL, then
	//dereference happens and application can crash
	//if ((NULL != ppszTemplate) || (NULL != ppszTitle))
	if ((NULL != ppszTemplate) && (NULL != ppszTitle))
    {
        *ppszTemplate = MAKEINTRESOURCE(IDD_TAB_SETTINGS_0);
		if (LoadString(m_hStringResourceHandle, IDD_TAB_SETTINGS_0, m_szItem, countof(m_szItem)) > 0)
		{
			*ppszTitle    = m_szItem;
		}
    }

    return hr;
}

/*++

Routine Name:

    CDocumentPropertyPage::AddUIControl

Routine Description:

    Adds a control handler class into the collection.

Arguments:

    iCtrlID - Resource Identifier of control.
    pUIControl - Pointer to a control handler class.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CDevicePropertyPage::AddUIControl(
    _In_ CONST INT   iCtrlID,
    _In_ CUIControl* pUIControl
    )
{
    HRESULT hr = S_OK;

	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    if (NULL != pUIControl)
    {
         m_UIControls[iCtrlID] = pUIControl;        
    }
	else
	{
		slog.putLog("CDevicePropertyPage::AddUIControl -> NULL_OBJECT(pUIControl)\n");
	}

     return hr;
}

/*++

Routine Name:

    CDevicePropertyPage::InitializeUIControls

Routine Description:

    Initialise all the controls in the dialog.

Arguments:

    hDlg - Handle of the property page Window associated with this class.
  
Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/

HRESULT
CDevicePropertyPage::InitializeUIControls(
    _In_ CONST HWND   hDlg
    )
{
    //
    // Use the wParam as the index into the control map and
    // inform the control of an activation
    //
    HRESULT hr = S_OK;
	m_hPropertySheetHandle = hDlg;
// <S><A> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V	
	CRegistryAPI regApi(m_hInstance);
	regApi.ReadData(m_pPrinterName, &m_pOemDevMode.scpi.dev);
//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	CWaterMarkRegistryData regwm(ghInstance, m_pPrinterName, m_hStringResourceHandle);
	BOOL			bShare = FALSE;
	WCHAR			szTextHKLMW[REG_ENT_SHARE_KEYSIZEW] = { 0 };
	regwm.ReadShareDayTimeFuncFromHKLM(m_pPrinterName, szTextHKLMW, REG_ENT_SHARE_KEYSIZEW);
	if (wcslen(szTextHKLMW) > 0)
	{
		bShare = (regwm.GetValidFlag(szTextHKLMW)? true:false);
	}
	m_PropertySheetState.bShare = bShare;
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
//<S><C>update the Propstate variable to the Support of MimicControl,20201412,Chanchal Singla
	// staple
	m_PropertySheetState.wStaple = m_pOemDevMode.scpi.ext.byStaple;	
	//punchtype
	m_PropertySheetState.wPunchType = m_pOemDevMode.scpi.ext.byPunchType;
	//PUNCH
	m_PropertySheetState.wPunch = m_pOemDevMode.scpi.ext.dwFieldEDM1 & DM_PUNCH ? bool_true : bool_false;

	// color mode
	if (m_pOemDevMode.scpi.pub.dmColor == DMCOLOR_MONOCHROME)
		m_PropertySheetState.wColMode = colmode_mono;
	else
	{
		if (m_pOemDevMode.scpi.ext.ColData.byColMode == colmode_mono)
			m_PropertySheetState.wColMode = colmode_auto;
		else
			m_PropertySheetState.wColMode = m_pOemDevMode.scpi.ext.ColData.byColMode;
	}

	// --- byRetention
	m_PropertySheetState.wRetentionMode = m_pOemDevMode.scpi.ext.JobCtrl.byRetention;
	
	// --- DM_PRINTSHARE
	m_PropertySheetState.wPrintRelease = m_pOemDevMode.scpi.ext.JobCtrl.wFieldJC & DM_PRINTSHARE ?
										bool_true : bool_false;

	// --- DM_ALWAYSUSE_ACNUM
	m_PropertySheetState.wUseAcNum = m_pOemDevMode.scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM ?
									 bool_true : bool_false;

	// --- DM_USE_LOGINNAME
	m_PropertySheetState.wLoginName = m_pOemDevMode.scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME ?
									 bool_true : bool_false;

	// --- DM_USE_LOGINPASS
	m_PropertySheetState.wLoginPass = m_pOemDevMode.scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS ?	
									 bool_true : bool_false;

	// --- DM_USE_SINGLESIGNON
	m_PropertySheetState.wLoginNameSSO = m_pOemDevMode.scpi.ext.JobCtrl.wFieldJC & DM_USE_SINGLESIGNON ?
									bool_true : bool_false;

//<E>update the Propstate variable to the Support of MimicControl,20201412,Chanchal Singla

	DevmodeToPropstate();
// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V

	// To create all the labels in the configuration tab, 20200414 - SSDI:Seetharam
	CreateMessageClass(hDlg);
	SetControlStrings();
	// To create all the controls in the configuration tab, 20200414 - SSDI:Seetharam
	CreateContorlClass();
	UIControlMap* pComponents = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();

    while (iterUIComponents != pComponents->end())
    {
        CUIControl* pControl = iterUIComponents->second;
        if (NULL != pControl)
        {
			if (FAILED(hr = pControl->OnInit(iterUIComponents->first,hDlg)))
            {
                break;
            }
        }
		iterUIComponents++;
    }
	SetTabState(hDlg);
	return hr;
}

/*++

Routine Name:

    CDevicePropertyPage::DestroyUIComponents

Routine Description:

    Destroys all the memory allocation

Arguments:

    None
  
Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
VOID
CDevicePropertyPage::DestroyUIComponents(
    VOID
    )
{
	UIControlMap::iterator iterUIComponents = m_UIControls.begin();

	while (iterUIComponents != m_UIControls.end())
	{
		if (iterUIComponents->second != NULL)
		{
			delete iterUIComponents->second;
			iterUIComponents->second = NULL;
		}
		iterUIComponents++;
	}

}
// <S><A> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
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
void CDevicePropertyPage::DevmodeToPropstate()
{

	//<S><A> update propstate of printing policy dialog when Defaults is selected 2020/21/9 K M Harika
	//PPPrintRelease
	m_PropertySheetState.wPPlcyPrintRelease = m_pOemDevMode.scpi.dev.dwFieldVDM & DM_PP_PRINTRELEASE ? bool_true : bool_false;
	//PPDocumentFiling
	m_PropertySheetState.wPPlcyRetention = m_pOemDevMode.scpi.dev.dwFieldVDM & DM_PP_RETENTION ? bool_true : bool_false;
	//PPUserAuthentication
	m_PropertySheetState.wPPlcyUserAuth = m_pOemDevMode.scpi.dev.dwFieldVDM & DM_PP_USERAUTH ? bool_true : bool_false;
	//PPWindowsLoginNameasUserName
	m_PropertySheetState.wPPlcyWinLogin = m_pOemDevMode.scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN ? bool_true : bool_false;
	//PPLgoinNameasUserName
	m_PropertySheetState.wPPlcyLoginName = m_pOemDevMode.scpi.dev.dwFieldVDM & DM_PP_LOGINN_AS_USERN ? bool_true : bool_false;
	//<E> update propstate of printing policy dialog when Defaults is selected 2020/21/9 K M Harika
		
	//Automatic color mode
	m_PropertySheetState.byAutoColorMode = m_pOemDevMode.scpi.dev.dwFieldVDM & DM_VDM_COLORMODE_AUTO_DISABLE ? bool_true : bool_false;
	//PinCompatibility
	m_PropertySheetState.byPinCompatibility = m_pOemDevMode.scpi.dev.dwFieldVDM & DM_VDM_PIN_CLEAN_TEXT ? bool_true : bool_false;
	// --- DM_SINGLESIGNON
	m_PropertySheetState.wSingleSignOn = m_pOemDevMode.scpi.dev.dwFieldVDM & DM_SINGLESIGNON ? bool_true : bool_false;
	//Maxpapersize
	//m_PropertySheetState.byMaxPaperSize = m_pOemDevMode.scpi.dev.byMaxPaperSize;
	m_PropertySheetState.byMaxPaperSize = (m_pOemDevMode.scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
	//ByPrintRelease
	m_PropertySheetState.byPrintRelease = m_pOemDevMode.scpi.dev.byFieldVDM & DM_VDM_NON_PRITNRLEASE ? bool_true : bool_false;
	//Color Mode
	if (m_pOemDevMode.scpi.dev.byFieldVDM & DM_VDM_COLORMODEL) //Color
	{
		m_PropertySheetState.wColorOption = 0;
	}
	else // Black and White
	{
		m_PropertySheetState.wColorOption = 1;
	}
	//<S><A>Retrictions with Printing Policy Dialog,Harika,20201014
	//<S><C>To Implement Task#3132,15-05-2024,SSDI:Manoj S
	if ((m_PropertySheetState.wPPlcyPrintRelease == bool_true) && ((m_PropertySheetState).byPrintRelease == bool_false))
	{
	//<E>To Implement Task #3132,15-05-2024,SSDI:Manoj S
		m_PropertySheetState.wPrintRelease = bool_true;
		m_pOemDevMode.scpi.ext.JobCtrl.wFieldJC |= DM_PRINTSHARE;
		m_PropertySheetState.wPPlcyRetention = bool_false;
		m_pOemDevMode.scpi.dev.dwFieldVDM &= ~DM_PP_RETENTION;
	}
	if (m_PropertySheetState.wPPlcyRetention != bool_false)
	{
		m_PropertySheetState.wRetention = bool_true;
		m_PropertySheetState.wRetentionMode = retention_hold_before;
		m_PropertySheetState.wDocFileMode = docfile_main;
		m_PropertySheetState.wUsePIN = bool_false;
		m_PropertySheetState.wPPlcyPrintRelease = bool_false;
		m_pOemDevMode.scpi.dev.dwFieldVDM &= ~DM_PP_PRINTRELEASE;
		m_PropertySheetState.wPrintRelease = bool_false;
	}
	//<E>Retrictions with Printing Policy Dialog,Harika,20201014
	//ByPunchmodule
	//m_PropertySheetState.wPunchOption = m_pOemDevMode.scpi.dev.byPunchmodule;
	m_PropertySheetState.wPunchOption = UpdatePunchModuleToPropstate();

	//ByStaplemodule
	if (m_pOemDevMode.scpi.dev.byFieldVDMStaple & DM_VDMS_SADDLE)
	{
		m_PropertySheetState.wStapleOption = stapleopt_3;
	}
	else if (m_pOemDevMode.scpi.dev.byFieldVDMStaple & DM_VDMS_2STAPLE)
	{
		m_PropertySheetState.wStapleOption = stapleopt_2;
	}
	else if (m_pOemDevMode.scpi.dev.byFieldVDMStaple & DM_VDMS_1STAPLE)
	{
		m_PropertySheetState.wStapleOption = stapleopt_1;
	}
	else
	{
		m_PropertySheetState.wStapleOption = stapleopt_none;
	}

	//ByStapleless
	m_PropertySheetState.byStapleless = m_pOemDevMode.scpi.dev.byFieldVDMStaple & DM_VDMS_STAPLELESS ? bool_true : bool_false;
//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
#if SCDM_VER >= 3000

	memcpy(m_pOemDevMode.scpi.ext2.stSubFavNames.szWMNameHash, m_PropertySheetState.szWMNameHash, sizeof(m_pOemDevMode.scpi.ext2.stSubFavNames.szWMNameHash));

#endif
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
}
// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V

// <S><A> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V
/*=============================================================================
Routine Name:
	CDevicePropertyPage::PropstatetoDevmode
Routine Description:
	converts  propsate to devmode.
Arguments:
	Nil
Return Value:
	void
--=============================================================================*/
void CDevicePropertyPage::PropstatetoDevmode()
{
	//m_pOemDevMode.scpi.dev.byPunchmodule = (BYTE)m_PropertySheetState.wPunchOption;
	m_pOemDevMode.scpi.dev.byPunchmodule = (BYTE)UpdatePropstateToPunchModule();
	if (m_PropertySheetState.wStapleOption == stapleopt_3)
	{
		m_pOemDevMode.scpi.dev.byFieldVDMStaple = DM_VDMS_SADDLE;
	}
	else if (m_PropertySheetState.wStapleOption == stapleopt_2)
	{
		m_pOemDevMode.scpi.dev.byFieldVDMStaple = DM_VDMS_2STAPLE;
	}
	else if (m_PropertySheetState.wStapleOption == stapleopt_1)
	{
		m_pOemDevMode.scpi.dev.byFieldVDMStaple = DM_VDMS_1STAPLE;
	}
	else
	{
		m_pOemDevMode.scpi.dev.byFieldVDMStaple = stapleopt_none;
	}

	m_pOemDevMode.scpi.dev.byFieldVDM = (m_PropertySheetState.wColorOption == bool_false) ?
		(m_pOemDevMode.scpi.dev.byFieldVDM | DM_VDM_COLORMODEL) : (m_pOemDevMode.scpi.dev.byFieldVDM & ~DM_VDM_COLORMODEL);

	//m_pOemDevMode.scpi.dev.byMaxPaperSize = m_PropertySheetState.byMaxPaperSize;
	m_pOemDevMode.scpi.dev.byMaxPaperSize = (m_PropertySheetState.byMaxPaperSize == bool_true ) ? paper_121 : paper_120;

	m_pOemDevMode.scpi.dev.byFieldVDM = (m_PropertySheetState.byPrintRelease == bool_true) ?
		(m_pOemDevMode.scpi.dev.byFieldVDM | DM_VDM_NON_PRITNRLEASE) : (m_pOemDevMode.scpi.dev.byFieldVDM & ~DM_VDM_NON_PRITNRLEASE);

	m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wSingleSignOn == bool_true) ?
		(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_SINGLESIGNON) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_SINGLESIGNON);

	m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.byAutoColorMode == bool_true) ?
		(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_VDM_COLORMODE_AUTO_DISABLE) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_VDM_COLORMODE_AUTO_DISABLE);
	
	m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.byPinCompatibility == bool_true) ?
		(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_VDM_PIN_CLEAN_TEXT) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_VDM_PIN_CLEAN_TEXT);
	
	m_pOemDevMode.scpi.dev.byFieldVDMStaple = (m_PropertySheetState.byStapleless == bool_true) ?
		(m_pOemDevMode.scpi.dev.byFieldVDMStaple | DM_VDMS_STAPLELESS) : (m_pOemDevMode.scpi.dev.byFieldVDMStaple & ~DM_VDMS_STAPLELESS);
//<S><A> update Devmode of printing policy dialog 2020/21/9 K M Harika
	m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyPrintRelease == bool_true) ?
		(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_PRINTRELEASE) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_PRINTRELEASE);

	m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyRetention == bool_true) ?
		(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_RETENTION) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_RETENTION);

	m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyUserAuth == bool_true) ?
		(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_USERAUTH) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_USERAUTH);

	m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyWinLogin == bool_true) ?
		(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_WINLOGIN_AS_LOGINN) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_WINLOGIN_AS_LOGINN);

	m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyLoginName == bool_true) ?
		(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_LOGINN_AS_USERN) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_LOGINN_AS_USERN);
//<E> update Devmode of printing policy dialog 2020/21/9 K M Harika
//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
#if SCDM_VER >= 3000

	memcpy(m_PropertySheetState.szWMNameHash, m_pOemDevMode.scpi.ext2.stSubFavNames.szWMNameHash, sizeof(m_PropertySheetState.szWMNameHash));

#endif
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
}
// <E> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V

// <S><A> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V
/*=============================================================================
Routine Name:
	CDevicePropertyPage::ResetDevDmode
Routine Description:
	Resets to devDmode.
Arguments:
	hDlg
Return Value:
	void
--=============================================================================*/
void CDevicePropertyPage::ResetDevDmode(HWND hDlg)
{
	m_pOemDevMode.scpi.dev.dwFieldVDM = 0;
	//<S><A>#to Fix Redmine #5056,09.05.2022,SSDI:Chanchal Singla
	// Redmine #5056:Default values should not be reset when driver is installed through Silent Auto Configuration
	// UD3.1 Feedback issue #106 mentioned test cases are also fixed.
	CRegistryAPI regApi;
	P_DEVDMODE		pdevDefault = NULL;
	if ((eBasicMode == m_dwSetupType) && (bIsSilentAutoConfig() == TRUE))
	{
		pdevDefault = new _DEVDMODE; // Default data is containing for a Configuration tab
		if (pdevDefault != NULL)
		{
			SecureZeroMemory(pdevDefault, sizeof(_DEVDMODE));
			FillDevDModeData(pdevDefault);
			regApi.ReadSilentDevmodeDataFromRegistry(pdevDefault, m_pPrinterName);
			//<E>Fix for the defaults issue when silent autoconfiguration executes. Harika
			memcpy(&m_pOemDevMode.scpi.dev, pdevDefault, sizeof(_DEVDMODE));
			delete pdevDefault;
			pdevDefault = NULL;
		}
	}
	//<E>#to Fix Redmine #5056,09.05.2022,SSDI:Chanchal Singla
	else
	{
		//<E> Reset DevMode of printing policy dialog when Defaults is selected 2020/21/9 K M Harika
		//-- PPLCY_PRINTRELASE
		m_PropertySheetState.wPPlcyPrintRelease = (BYTE)m_DefaultSettings[FEATURE_PPLCYPRINTRELEASE].iDevmodeValue;
		m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyPrintRelease == bool_true) ?
			(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_PRINTRELEASE) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_PRINTRELEASE);

		//--PPLCY_RETENTION
		m_PropertySheetState.wPPlcyRetention = (BYTE)m_DefaultSettings[FEATURE_PPLCYRETENTION].iDevmodeValue;
		m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyRetention == bool_true) ?
			(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_RETENTION) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_RETENTION);

		//--PPLCY_USERAUTH
		m_PropertySheetState.wPPlcyUserAuth = (BYTE)m_DefaultSettings[FEATURE_PPLCYUSERAUTHENTICATION].iDevmodeValue;
		m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyUserAuth == bool_true) ?
			(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_USERAUTH) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_USERAUTH);

		//-- PPLCY_WINLOGIN
		m_PropertySheetState.wPPlcyWinLogin = (BYTE)m_DefaultSettings[FEATURE_PPLCYWINLOGIN].iDevmodeValue;
		m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyWinLogin == bool_true) ?
			(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_WINLOGIN_AS_LOGINN) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_WINLOGIN_AS_LOGINN);

		//-- PPLCY_LOGINNAME
		m_PropertySheetState.wPPlcyLoginName = (BYTE)m_DefaultSettings[FEATURE_PPLCYLOGINNAME].iDevmodeValue;
		m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wPPlcyLoginName == bool_true) ?
			(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_PP_LOGINN_AS_USERN) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_PP_LOGINN_AS_USERN);

		//Defaults Values Are Assigned wrong,
		//based on GPD feature default value get and update
		// --byPunchmodule
		m_PropertySheetState.wPunchOption = (BYTE)m_DefaultSettings[FEATURE_DEVICEPUNCHMODULE].iDevmodeValue;
		m_pOemDevMode.scpi.dev.byPunchmodule = (BYTE)m_PropertySheetState.wPunchOption;

		//--wStapleOption
		m_PropertySheetState.wStapleOption = (BYTE)m_DefaultSettings[FEATURE_DEVICESTAPLEMODULE].iDevmodeValue;
		m_pOemDevMode.scpi.dev.byFieldVDMStaple &= ~(DM_VDMS_1STAPLE | DM_VDMS_2STAPLE | DM_VDMS_SADDLE);
		if (m_PropertySheetState.wStapleOption == stapleopt_3)
		{
			m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_SADDLE;
		}
		else if (m_PropertySheetState.wStapleOption == stapleopt_2)
		{
			m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_2STAPLE;
		}
		else if (m_PropertySheetState.wStapleOption == stapleopt_1)
		{
			m_pOemDevMode.scpi.dev.byFieldVDMStaple |= DM_VDMS_1STAPLE;
		}
		else
		{
			m_pOemDevMode.scpi.dev.byFieldVDMStaple = stapleopt_none;
		}

		//--StapleLess
		m_PropertySheetState.byStapleless = (BYTE)m_DefaultSettings[FEATURE_STAPLELESS].iDevmodeValue;
		m_pOemDevMode.scpi.dev.byFieldVDMStaple = (m_PropertySheetState.byStapleless == bool_true) ?
			(m_pOemDevMode.scpi.dev.byFieldVDMStaple | DM_VDMS_STAPLELESS) : (m_pOemDevMode.scpi.dev.byFieldVDMStaple & ~DM_VDMS_STAPLELESS);

#if SCDM_VER >= 3000
		// --- DM_SINGLESIGNON
		m_PropertySheetState.wSingleSignOn = (BYTE)m_DefaultSettings[FEATURE_SINGLESIGNON].iDevmodeValue;
		m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.wSingleSignOn == bool_true) ?
			(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_SINGLESIGNON) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_SINGLESIGNON);
#endif
		//-- AutoColorMode
		m_PropertySheetState.byAutoColorMode = (BYTE)m_DefaultSettings[FEATURE_AUTOCOLORMODE].iDevmodeValue;
		m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.byAutoColorMode == bool_false) ?
			(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_VDM_COLORMODE_AUTO_DISABLE) : (m_pOemDevMode.scpi.dev.dwFieldVDM& ~DM_VDM_COLORMODE_AUTO_DISABLE);

		//-- PinCompatibility
		m_PropertySheetState.byPinCompatibility = (BYTE)m_DefaultSettings[FEATURE_PINCOMPATIBILITY].iDevmodeValue;
		m_pOemDevMode.scpi.dev.dwFieldVDM = (m_PropertySheetState.byPinCompatibility == bool_true) ?
			(m_pOemDevMode.scpi.dev.dwFieldVDM | DM_VDM_PIN_CLEAN_TEXT) : (m_pOemDevMode.scpi.dev.dwFieldVDM & ~DM_VDM_PIN_CLEAN_TEXT);
		
		//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
		//-- ShareFavorites
		m_PropertySheetState.bShare = (BYTE)m_DefaultSettings[FEATURE_SHAREFAVORITES].iDevmodeValue;
		//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
		
		//<S><C>To fix redmine bug #3156 ,20210713 SSDI:Goutham
		//-- ColorMode
		if (m_bBWPrint == TRUE) // BLack & White
		{
			m_PropertySheetState.wColorOption = eSecondOption;
			if ((eBasicMode == m_dwSetupType) && (bIsSilentAutoConfig() == FALSE))
			{
				m_PropertySheetState.wColorOption = (SHORT)m_DefaultSettings[FEATURE_DEVICECOLORMODE].iDevmodeValue;
			}
		}
		else
		{
			m_PropertySheetState.wColorOption = (SHORT)m_DefaultSettings[FEATURE_DEVICECOLORMODE].iDevmodeValue;
		}
		//<S><E>To fix redmine bug #3156 ,20210713 SSDI:Goutham
		m_pOemDevMode.scpi.dev.byFieldVDM = (m_PropertySheetState.wColorOption == bool_false) ?
			(m_pOemDevMode.scpi.dev.byFieldVDM | DM_VDM_COLORMODEL) : (m_pOemDevMode.scpi.dev.byFieldVDM & ~DM_VDM_COLORMODEL);

		//-- MaxPaperSize
		m_PropertySheetState.byMaxPaperSize = (BYTE)m_DefaultSettings[FEATURE_MAXPAPERSIZE].iDevmodeValue;
		m_pOemDevMode.scpi.dev.byMaxPaperSize = m_PropertySheetState.byMaxPaperSize;


		//-- PrintRelease
		m_PropertySheetState.byPrintRelease = (BYTE)m_DefaultSettings[FEATURE_DEVICEPRINTRELEASE].iDevmodeValue;
		m_pOemDevMode.scpi.dev.byFieldVDM = (m_PropertySheetState.byPrintRelease == bool_true) ?
			(m_pOemDevMode.scpi.dev.byFieldVDM | DM_VDM_NON_PRITNRLEASE) : (m_pOemDevMode.scpi.dev.byFieldVDM & ~DM_VDM_NON_PRITNRLEASE);
	}
	DevmodeToPropstate();
}
// <E> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V


//=============================================================================
//Routine Name:
//	CDevicePropertyPage::GetDefaultGPDOption
//
//Routine Description:
//	Get Default GPDOption 
//
//Arguments:
//	HWND hDlg
//	UINT iControlID 
//	UINT wfeatureIndex
//
//Return Value:
//	UINT DefaultOptionIndex
//--=============================================================================
UINT CDevicePropertyPage::GetDefaultGPDOption(HWND hDlg,UINT iControlID ,UINT wfeatureIndex,BOOL flag)
{
	UINT suCurrentItemId = m_DefaultSettings[wfeatureIndex].ResourceID;
	TCHAR szItem[MAX_PATH] = { 0 };
	UINT DefaultOptionIndex = 0;
	UINT uCheckState = BST_UNCHECKED;
	switch (flag)
	{
	case eComboBox: // combo box default value
	{
		if (LoadString(m_hStringResourceHandle, suCurrentItemId, szItem, countof(szItem)) > 0)
		{
			DefaultOptionIndex = static_cast<UINT>(SendDlgItemMessage(hDlg, iControlID, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szItem));
		}
	}
		break;
	case eCheckBox: // check box default value
	{
		if (LoadString(m_hStringResourceHandle, suCurrentItemId, szItem, countof(szItem)) > 0)
		{
			uCheckState = (0 == wcscmp(szItem, L"On")) ? BST_CHECKED : BST_UNCHECKED;
		
			DefaultOptionIndex = static_cast<UINT>((uCheckState == BST_UNCHECKED) ? 0 : 1);
		}
	}
		break;
	default:
		break;
	}

	return DefaultOptionIndex;
}
/*++

Routine Name:

    CDevicePropertyPage::SetControlStrings

Routine Description:

    Initialises all the strings for the Controls Present in the dialog.

Arguments:

    None
  
Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
VOID CDevicePropertyPage::SetControlStrings(VOID)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	
	//Printer Selection Group Box
	CUIControl *pControl = new CStaticText(IDC_GRP_PRNSEL, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDS_IDC_GRP_PRNSEL, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	// Language group box
	pControl = new CStaticText(IDC_GRP_LANGSEL, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDS_IDC_GRP_LANGSEL, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_LANGSEL)\n");
	}

	// Language selection message
	pControl = new CStaticText(IDC_STC_LANGSEL_MSG, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_LANGSEL_MSG, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_LANGSEL_MSG)\n");
	}

	// Color Module
	pControl = new CStaticText(IDC_STC_COLORMODE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_COLORMODE, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_LANGSEL_MSG)\n");
	}

	// Staple Label - IDC_STC_STAPLE_OPT
	pControl = new CStaticText(IDC_STC_STAPLE_OPT, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_STAPLE_OPT, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_LANGSEL_MSG)\n");
	}

	// Staple Label - IDC_STC_PUNCH_OPT
	pControl = new CStaticText(IDC_STC_PUNCH_OPT, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PUNCH_OPT, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_LANGSEL_MSG)\n");
	}

	// Print Release Label - IDC_STC_PRN_RELEASE
	pControl = new CStaticText(IDC_STC_PRN_RELEASE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PRN_RELEASE, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_LANGSEL_MSG)\n");
	}

	// Max Paper Size Label - IDC_STC_MAXPAPERSIZE
	pControl = new CStaticText(IDC_STC_MAXPAPERSIZE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_MAXPAPERSIZE, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_LANGSEL_MSG)\n");
	}
	
	
	// Move to CreateControlClass

}

VOID CDevicePropertyPage::CreateContorlClass(VOID)
{
	HRESULT hr = S_OK;
	m_bDefaultClicked = FALSE;
	m_bColorBWSelected = FALSE;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	//Apply Button is not Enabled For a Device Property page Features,
	//So passing TRUE Argument to the features class Constructor.
	BOOL wIsDevicePropertyPage = TRUE;
	//<S><C>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
	//In Basic mode Printing Preferences is not displayed as per the Language set
	// get language id from registry
	//TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	//GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	//CShIniFile cshLangFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	//DWORD dwDefLangID = cshLangFile.GetDefaultLangID();
	CRegistryAPI regApi(m_hInstance);
	//m_dwLangID = regApi.GetLangID(m_pPrinterName, dwDefLangID);
	SHORT dwLangID = -1;
	CShRegMdlTbl RegMdlTbl(ghInstance, m_pPrinterName, m_pmcf);
	RegMdlTbl.GetLangID(&dwLangID);
	m_dwLangID = (DWORD)dwLangID;
	regApi.SetLangID(m_pPrinterName, m_dwLangID);
	//<E>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla

	//Get the installation type
	//Coverity 91516 issue Fix - SSDI Seetharam : 20210705
	if (regApi.GetInstallationType(m_pPrinterName, m_dwSetupType) == ERROR_FILE_NOT_FOUND)
	//if (regApi.GetInstallationType(m_pPrinterName, (short&)m_dwSetupType) == ERROR_FILE_NOT_FOUND)
	{
		m_dwSetupType = m_pmcf->GetInstallationType();
		regApi.SetInstallationType(m_pPrinterName, m_dwSetupType);
	}

	// Defaults button
	CUIControl *pControl = new CButton(IDC_BTN_SHARED_DEFAULT, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_SHARED_DEFAULT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(IDC_BTN_ZOOM)\n");
	}

	//Printer Selection Combo
	pControl = new CPrinterSelectionCombo(IDC_CMB_PRNSEL, m_hPrinter, m_hStringResourceHandle, m_pPrinterName);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_PRNSEL, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_OBJECT(IDC_CMB_PRNSEL)\n");
	}
	//((CPrinterSelectionCombo *)pControl)->SetCurrentItemIndex(m_uCurrentItemIndex);

	//Printer Selection Button
	pControl = new CPrinterSelectionButton(IDC_BTN_PRNSEL, m_hPrinter, m_hStringResourceHandle, m_pPrinterName);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PRNSEL, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_OBJECT(IDC_BTN_PRNSEL)\n");
	}

	pControl = new CLangCombo(IDC_CMB_LANGSEL, m_hStringResourceHandle, m_dwLangID, m_pPrinterName);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_LANGSEL, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_OBJECT(IDC_CMB_LANGSEL)\n");
	}

	//IDC_CMB_COLORMODULE
	PRESOURCEIDINFO pResourceIDInfo = GetResourceIDInfoOfControl(DEVICECOLORMODE);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DEVICECOLORMODE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEVICECOLORMODE],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_DEVICECOLORMODE],
		m_ObjCShowMessage,
		IDC_CMB_COLORMODULE,
		m_PropertySheetState.wColorOption, wIsDevicePropertyPage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_COLORMODULE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}
	
	//IDC_CMB_STAPLE_OPT
	pResourceIDInfo = GetResourceIDInfoOfControl(DEVICESTAPLEMODULE);
// <S><C> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V	
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DEVICESTAPLEMODULE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEVICESTAPLEMODULE],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_DEVICESTAPLEMODULE],
		m_ObjCShowMessage,
		IDC_CMB_STAPLE_OPT,
		(DWORD)m_PropertySheetState.wStapleOption, wIsDevicePropertyPage
	);
// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_STAPLE_OPT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	// Stapleless
	pResourceIDInfo = GetResourceIDInfoOfControl(STAPLELESS);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLELESS],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLELESS],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_STAPLELESS],
		m_ObjCShowMessage,
		(DWORD)m_PropertySheetState.byStapleless, wIsDevicePropertyPage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_STAPLE_STAPLELESS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}

	//IDC_CMB_PUNCH_OPT
	pResourceIDInfo = GetResourceIDInfoOfControl(DEVICEPUNCHMODULE);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DEVICEPUNCHMODULE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEVICEPUNCHMODULE],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_DEVICEPUNCHMODULE],
		m_ObjCShowMessage,
		IDC_CMB_PUNCH_OPT,
		(DWORD)m_PropertySheetState.wPunchOption, wIsDevicePropertyPage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_PUNCH_OPT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	// IDC_CMB_PRN_RELEASE
	pResourceIDInfo = GetResourceIDInfoOfControl(DEVICEPRINTRELEASE);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DEVICEPRINTRELEASE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEVICEPRINTRELEASE],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_DEVICEPRINTRELEASE],
		m_ObjCShowMessage,
		IDC_CMB_PRN_RELEASE,
		(DWORD)m_PropertySheetState.byPrintRelease, wIsDevicePropertyPage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_PRN_RELEASE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	//IDC_CMB_MAXPAPERSIZE
	pResourceIDInfo = GetResourceIDInfoOfControl(MAXPAPERSIZE);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MAXPAPERSIZE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MAXPAPERSIZE],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_MAXPAPERSIZE],
		m_ObjCShowMessage,
		IDC_CMB_MAXPAPERSIZE,
		(DWORD)m_PropertySheetState.byMaxPaperSize, wIsDevicePropertyPage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_MAXPAPERSIZE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	// Single Sign-On
	pResourceIDInfo = GetResourceIDInfoOfControl(SINGLESIGNON);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_SINGLESIGNON],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_SINGLESIGNON],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_SINGLESIGNON],
		m_ObjCShowMessage,
		(DWORD)m_PropertySheetState.wSingleSignOn, wIsDevicePropertyPage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_SINGLE_SIGNON, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}

	// Auto Color-Mode
	pResourceIDInfo = GetResourceIDInfoOfControl(AUTOCOLORMODE);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTOCOLORMODE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTOCOLORMODE],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_AUTOCOLORMODE],
		m_ObjCShowMessage,
		(DWORD)m_PropertySheetState.byAutoColorMode, wIsDevicePropertyPage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_AUTO_COLORMODE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}

	// Pin Compatibiity
	pResourceIDInfo = GetResourceIDInfoOfControl(PINCOMPATIBILITY);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PINCOMPATIBILITY],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PINCOMPATIBILITY],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_PINCOMPATIBILITY],
		m_ObjCShowMessage,
		(DWORD)m_PropertySheetState.byPinCompatibility, wIsDevicePropertyPage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_PIN_COMPATIBILITY, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}
	
	//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	// Share Favorites
	pResourceIDInfo = GetResourceIDInfoOfControl(SHAREFAVORITES);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_SHAREFAVORITES],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_SHAREFAVORITES],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_SHAREFAVORITES],
		m_ObjCShowMessage,
		(DWORD)m_PropertySheetState.bShare, wIsDevicePropertyPage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_SHARE_FAV, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	
	//Auto-Configuration
	pControl = new CButton(IDC_BTN_AUTO_CONF, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_AUTO_CONF, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(IDC_BTN_ZOOM)\n");
	}

	// Printing Policy
	pControl = new CButton(IDC_BTN_EXTRA_SETTINGS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_EXTRA_SETTINGS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(IDC_BTN_ZOOM)\n");
	}

	// About
	pControl = new CButton(IDC_BTN_ABOUT, m_hStringResourceHandle, NULL);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_ABOUT, pControl);

		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SetControlStrings -> NULL_OBJECT(IDC_BTN_ABOUT)\n");
	}
	//<S><A>To The Support Of Mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
	// ---Mimic Left Side
	pControl = new CMimicImageConfig(IDC_BTN_SHARED_BBOX, m_hPropertySheetHandle, m_pFeatureInfoList,
		m_hStringResourceHandle, m_hBitmapResourceHandle, m_pPrinterName, &m_pOemDevMode,NULL);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_SHARED_BBOX, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CMimicImage)\n");
	}

	//--- Mimic Right Side
	pControl = new CMimicText(IDC_BTN_SHARED_BBOX_SUB, m_hPropertySheetHandle, m_pFeatureInfoList,
		m_hStringResourceHandle, m_hBitmapResourceHandle, m_pPrinterName, &m_pOemDevMode, m_hPrinter);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_SHARED_BBOX_SUB, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CMimicText)\n");
	}
	
	//<E>To The Support Of Mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla

	//Favorites Combo box
	pControl = new CShCmbUserset(
		m_pFeatureInfoList,
		m_pPrintCoreHelperUni,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_ObjCShowMessage,
		&m_pOemDevMode,
		&m_OemPublicDevMode,
		m_pPrinterName,
		m_pUsIndex,
		m_ppi,
		IDC_CMB_SHARED_USRSET,
		m_PropertySheetState
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_SHARED_USRSET, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CDevicePropertyPage::CreateContorlClass -> NULL_POINTER(CShCmbUserset)\n");
	}

	m_bDisablePrnSel = DisablePrinterSelection();
}

void CDevicePropertyPage::SetPreviousItemIndex(long lCheckPos)
{
    m_uPreviousItemIndex = lCheckPos;
}

void CDevicePropertyPage::SetCurrentItemIndex(long uChangedOptionIndex)
{
    m_uCurrentItemIndex = uChangedOptionIndex;
	g_CurrentIndex = m_uCurrentItemIndex;
}

void CDevicePropertyPage::PsnSetActive(HWND hDlg) 
{  
    CRegistryAPI regApi(ghInstance);
    long lIndex = 0;
    long lCount = 0;
    WCHAR* pStr = NULL;
    UDMRI udmri = {0} ;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	//<S><A>To the Support of Save And Retrieving the GPD Feature,20201123,SSDI:Chanchal Singla
	HRESULT hResult = S_OK;
	CDevicePropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
		(NULL != pPropertyPageInstance))
	{
		hResult = pPropertyPageInstance->ActivateUIControl(hDlg);
		if (S_OK != hResult)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	//if (m_bExecuteSilentAutoConfig)
	//	m_bExecuteSilentAutoConfig = FALSE;


	//<E>To the Support of Save And Retrieving the GPD Feature,20201123,SSDI:Chanchal Singla
    if(!m_bIsPrinterIcon)
    {
        regApi.GetRegisterMFPCountAndCheckPos(m_pPrinterName,lCount,lIndex);
        LRESULT lRet = regApi.GetModelRegInfo(m_pPrinterName, lIndex,  &udmri);
		m_bBWPrint = udmri.blBWprint;
		//SSDI Bug Fix:3800 - SSDI:Seetharam - 20210520
		//m_pOemDevMode.scpi.dev.nTargetDeviceID = lIndex + 1;
		m_pOemDevMode.scpi.dev.nTargetDeviceID = lIndex;
		//<S><A> SSDI Bug #4016 Fix - SSDI Seetharam : 20210702
        if((0 != lCount) && (ERROR_FILE_NOT_FOUND == lRet))
        {
            short wDeviceType = 1;	//tbd
            //regApi.SetCheckPosDeviceTypeInHKLM(m_pPrinterName,(long&)lIndex,wDeviceType);
            //regApi.SetCount(m_pPrinterName,lCount);
			regApi.SetCheckPos(m_pPrinterName, 0, 0, 1);
			regApi.SetCount(m_pPrinterName, 0);
			regApi.SetStartPos(m_pPrinterName, 0);
			lCount = 0;
        }
		//<S><A> SSDI Bug #4016 Fix - SSDI Seetharam : 20210702

        SendMessage(GetDlgItem(hDlg,IDC_CMB_PRNSEL),CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
		//SSDI Bug Fix - 3787 - SSDI Seetharam - 20210616
 		CPrinterSelectionCombo *pControl = (CPrinterSelectionCombo *)m_UIControls[IDC_CMB_PRNSEL];
		//pControl->ResetPrinterCombo(hDlg);

        if((0 == lCount) || (ERROR_FILE_NOT_FOUND == lRet))
        {
			if(0 == lCount)
			{
				slog.putLog("CDevicePropertyPage::PsnSetActive -> LCOUNT_IS_ZERO\n");
			}
			//else
			//{
			//	slog.putLog("CDevicePropertyPage::PsnSetActive -> ERROR_FILE_NOT_FOUND");
			//}
			m_pOemDevMode.scpi.dev.nTargetDeviceID = 0;
			m_bBWPrint = FALSE;
            WCHAR szItem[SCUI_PRNNAME_MAX]={0};
            if (LoadString(m_hStringResourceHandle, IDS_PRNSEL_NOTSET, szItem, countof(szItem)) > 0)
            {
                SendDlgItemMessage(hDlg, IDC_CMB_PRNSEL, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(szItem));
            } 
            SendDlgItemMessage(hDlg, IDC_CMB_PRNSEL, CB_SETCURSEL, 0, 0); 
            ::EnableWindow(GetDlgItem(hDlg,IDC_CMB_PRNSEL),FALSE);
            /*//<S><C>To fix Redmine bug #2995,20200912 SSDI:Goutham
            WCHAR wscPortName[SCUI_MAX_PORTNAME*2] = {0};
            WCHAR wcsModelName[SCUI_MODELNAME_MAX*2] = {0};
//<S><E>To fix Redmine bug #2995,20200912 SSDI:Goutham
            if(NULL != m_hPrinter)
			{
                regApi.GetModelNameAndDefaultPortName(m_pPrinterName,m_hPrinter,wscPortName,wcsModelName);
			}
			else
			{
				slog.putLog("CDevicePropertyPage::PsnSetActive -> NULL_HANDLE(m_hPrinter)\n");
			}
//<S><C>To fix Redmine bug #2995,20200912 SSDI:Goutham
            //pStr = setstr(wcsModelName,wscPortName);
            //SetDlgItemText(hDlg,IDC_STC_SHARED_EXP,pStr);
//<S><E>To fix Redmine bug #2995,20200912 SSDI:Goutham*/
            //return; - 
        }
		else
		{
			if (m_MfpSlctInfo == NULL)
			{
				CreateMfpSlctInfo();
				m_MfpSlctInfo->lCheckIndex = m_pOemDevMode.scpi.dev.nTargetDeviceID;
				//<S><A> fix for issue 6018. Harika, 20230324
				regApi.Set_WM_MFP_INDEX_CHANGE(m_pPrinterName, FALSE);
				regApi.Set_Manually_MFP_INDEX_CHANGE(m_pPrinterName, FALSE);
				//<E> fix for issue 6018. Harika, 20230324			
			}
			else
			{
				//This is to refresh the mfpSlctInfo data when the user
				//switches between disocvery dialog via preferences and configuration tab

				//<S><C>To Fix Redmine #3151 Issue 3,26.august.2021,SSDI:Chanchal Singla
				//Issue 3:-Silent Autoconfiguration is not executing from General tab
				BOOL bRet = FALSE;
				int ManuallyAdd = 0;
				DWORD WM_MFP_ChangedIndex = 0;
				DWORD WM_Manually_ChangedIndex = 0;
				WM_MFP_ChangedIndex = regApi.Get_WM_MFP_INDEX_CHANGE(m_pPrinterName);
				WM_Manually_ChangedIndex = regApi.Get_Manually_MFP_INDEX_CHANGE(m_pPrinterName);
				//<S><A> Add the condition to fix redmine Bug #4229: 2021.07,September: SSDI:Chanchal Singla
				//Silent Auto Configuration fails to execute when we switch between Tabs
				if ((WM_MFP_ChangedIndex == TRUE) || (WM_Manually_ChangedIndex == TRUE))
				//<E> Add the condition to fix redmine Bug #4229: 2021.07,September: SSDI:Chanchal Singla
				{
					LONG wPreviousIndex = m_MfpSlctInfo->lCheckIndex;
					LONG lPreviousCount = m_MfpSlctInfo->lCount;
					DeleteMfpSlctInfo();
					CreateMfpSlctInfo();
					

					PUDMRI pudmri = NULL;
					if (NULL != m_MfpSlctInfo)
					{
						m_MfpSlctInfo->lCheckIndex = m_pOemDevMode.scpi.dev.nTargetDeviceID;
						if (NULL != m_MfpSlctInfo->pudmriList)
						{
							if (0 < m_MfpSlctInfo->lCount)
							{
								for (long i = 0; i < m_MfpSlctInfo->lCount; i++)
								{
									pudmri = m_MfpSlctInfo->pudmriList[i];
									if (m_MfpSlctInfo->lCheckIndex == pudmri->wIndex)
									{
										ManuallyAdd = (*pudmri).iManuallyAdded;
										(*pudmri).iManuallyAdded = (0 == ManuallyAdd) ? 0 : eManuallyAdded;
									}
								}
							}
						}
					}
					if ((wPreviousIndex != m_MfpSlctInfo->lCheckIndex)
						&& (WM_MFP_ChangedIndex == TRUE))
					{
						bRet = TRUE;
					}
					else
					{
						if ((wPreviousIndex != m_MfpSlctInfo->lCheckIndex)
							&& (ManuallyAdd != FALSE)
							&& (WM_Manually_ChangedIndex == TRUE))
						{
							m_MfpSlctInfo->lCheckIndex = m_pOemDevMode.scpi.dev.nTargetDeviceID;
							regApi.Set_Manually_MFP_INDEX_CHANGE(m_pPrinterName, FALSE);
						}
						else
						{
							m_MfpSlctInfo->lCheckIndex = wPreviousIndex;
							//m_MfpSlctInfo->lCount = lPreviousCount;
						}
						if ((WM_MFP_ChangedIndex == TRUE))
						{
							bRet = TRUE;
						}
					}
					if (bRet == TRUE)
					{
						regApi.ReadData(m_pPrinterName, &m_pOemDevMode.scpi.dev);
						DevmodeToPropstate();
						//<S><A> fix for issue 6018. Harika, 20230324
						UINT TempArray[9] = {
							FEATURE_DEVICECOLORMODE,
							FEATURE_DEVICESTAPLEMODULE,
							FEATURE_STAPLELESS,
							FEATURE_DEVICEPUNCHMODULE,
							FEATURE_MAXPAPERSIZE,
							FEATURE_SINGLESIGNON,
							FEATURE_AUTOCOLORMODE,
							FEATURE_PINCOMPATIBILITY,
							FEATURE_DEVICEPRINTRELEASE
						};
						SHORT SelectedArray[9] = {
							m_PropertySheetState.wColorOption,
							m_PropertySheetState.wStapleOption,
							(SHORT)m_PropertySheetState.byStapleless,
							m_PropertySheetState.wPunchOption,
							(SHORT)m_PropertySheetState.byMaxPaperSize,
							m_PropertySheetState.wSingleSignOn,
							(SHORT)m_PropertySheetState.byAutoColorMode,
							(SHORT)m_PropertySheetState.byPinCompatibility,
							(SHORT)m_PropertySheetState.byPrintRelease
						};
						size_t nFeatureIndex = 0;
						size_t nNumberOfFeatures = 9;
						for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
						{
							m_pFeatureInfoList->pFeatureOptionsList[TempArray[nFeatureIndex]].uDefaultOptionIndex = (UINT)SelectedArray[nFeatureIndex];
						}
						//<E> fix for issue 6018. Harika, 20230324
						regApi.Set_WM_MFP_INDEX_CHANGE(m_pPrinterName, FALSE);
						regApi.Set_Manually_MFP_INDEX_CHANGE(m_pPrinterName, FALSE);
					}
					//<E>To Fix Redmine #3151 Issue 3,26.august.2021,SSDI:Chanchal Singla
				}
			}
			pControl->ResetPrinterCombo(hDlg, m_MfpSlctInfo);
			m_pOemDevMode.scpi.dev.nTargetDeviceID = m_MfpSlctInfo->lCheckIndex;
		}			
		//SSDI Bug Fix:3800 - SSDI:Seetharam - 20210520
		//Change the loop counter from 0 to 1 since the registry count starts from 1
        /*for(int i = 1;i <= lCount;i++)
        {
            regApi.GetModelRegInfo(m_pPrinterName, i,  &udmri);
            pStr = setstr(udmri.szModelName,udmri.szPortName);
            if(i == g_CurrentIndex)
            {
				//m_pOemDevMode.scpi.dev.nTargetDeviceID = i + 1;
				m_pOemDevMode.scpi.dev.nTargetDeviceID = i;
               // SetDlgItemText(hDlg,IDC_STC_SHARED_EXP,pStr);
            }
            //SendDlgItemMessage(hDlg, IDC_CMB_PRNSEL, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(pStr));
        }*/

        //SendDlgItemMessage(hDlg, IDC_CMB_PRNSEL, CB_SETCURSEL, g_CurrentIndex-1, 0);
        BOOL bEnable = FALSE;
        if(regApi.IsPrinterOperators(m_pPrinterName) )
        {
            if (((TRUE == regApi.m_bClusterFlag) || (FALSE == regApi.IsSharedServerSide(m_pPrinterName))) && (lCount > 1))
            {
                bEnable = TRUE;
            }
        }
        ::EnableWindow(GetDlgItem(hDlg,IDC_CMB_PRNSEL),bEnable);
    }
  /*  else
    {
//<S><C>To fix Redmine bug #2995,20200912 SSDI:Goutham
       // WCHAR pIPAddress[SCUI_MAX_PORTNAME] = {0};
       // regApi.GetModelRegInfo(m_pPrinterName, lIndex+1,  &udmri);
       // GetIPAddress(m_hPrinter,udmri.szPortName,pIPAddress);
        //pStr = setstr(udmri.szModelName,pIPAddress);
        //SetDlgItemText(hDlg,IDC_STC_SHARED_EXP,pStr);
//<S><E>To fix Redmine bug #2995,20200912 SSDI:Goutham
    }*/	
	//<S><D> Deleted the code to fix redmine Bug #4229: 2021.07,September: SSDI:Chanchal Singla
	//Silent Auto Configuration fails to execute when we switch between Tabs
	/* if (m_bExecuteSilentAutoConfig)
		m_bExecuteSilentAutoConfig = FALSE;*/
	//<E> Deleted the code to fix redmine Bug #4229: 2021.07,September: SSDI:Chanchal Singla
	SetTabState(hDlg, TRUE);
}
	//<S><A>To the Support of Save And Retrieving the GPD Feature,20201123,SSDI:Chanchal Singla
/*++

Routine Name:

	CDevicePropertyPage::ActivateUIControl

Routine Description:

	Call the OnActivate() method in all control handlers in the collection.

Arguments:

	hDlg - Handle of property page.

Return Value:

	HRESULT
	S_OK - On success
	E_*  - On error

--*/
HRESULT
CDevicePropertyPage::ActivateUIControl(
	_In_ CONST HWND   hDlg
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	//
	// Use the wParam as the index into the control map and
	// inform the control of an activation
	//
	HRESULT hr = S_OK;
	for (auto iterUIComponents : m_UIControls)
	{
		CUIControl* pControl = iterUIComponents.second;
		if (NULL != pControl)
		{
			if (FAILED(hr = pControl->ActivateUIControl(iterUIComponents.first, hDlg)))
			{
				if (S_OK != hr)
				{
					slog.LoggingErrorHResultValues();
				}
				break;
			}

		}
		else
		{
			slog.putLog("CDevicePropertyPage::ActivateUIControl -> NULL_POINTER(pControl)\n");
		}
	}

	return hr;
}
	//<S><A>To the Support of Save And Retrieving the GPD Feature,20201123,SSDI:Chanchal Singla
void CDevicePropertyPage::PsnApply(HWND hDlg)
{
	//<S><C>To the Support of MimicControl,20201412,SSDI:Chanchal Singla
	// Based On Device type Devmode is updtaing
	long			lPPM = 0;
	BOOL			ModelType = eColor;
	BOOL			BWprint = FALSE;
	// To decide whether selected Model is Manuallay added
	INT				ManuallyAdd = FALSE;

	short			wDeviceType = 1;
	CRegistryAPI regApi(m_hInstance);
	//<S><C>To Fix Redmine #3151 Issue 3,26.august.26,SSDI:Chanchal Singla
	//Issue 3:-Silent Autoconfiguration is not executing from General tab
	DWORD WM_MFP_ChangedIndex = 0;
	DWORD Manually_MFP_ChangedIndex = 0;
	WM_MFP_ChangedIndex = regApi.Get_WM_MFP_INDEX_CHANGE(m_pPrinterName);
	Manually_MFP_ChangedIndex = regApi.Get_Manually_MFP_INDEX_CHANGE(m_pPrinterName);
	// condition will Execute only when  Silent Autoconfiguration is executing from General tab
	// and then close the driver without opening Configuration tab.
	if ((WM_MFP_ChangedIndex == TRUE)|| (Manually_MFP_ChangedIndex == TRUE))
	{
		DeleteMfpSlctInfo();
		CreateMfpSlctInfo();
		m_MfpSlctInfo;
		regApi.ReadData(m_pPrinterName, &m_pOemDevMode.scpi.dev);
		DevmodeToPropstate();
		regApi.Set_WM_MFP_INDEX_CHANGE(m_pPrinterName, FALSE);
		regApi.Set_Manually_MFP_INDEX_CHANGE(m_pPrinterName, FALSE);
	}
	//<E>To Fix Redmine #3151 Issue 3,26.august.2021,SSDI:Chanchal Singla
	//Color Mode
	if (m_pOemDevMode.scpi.dev.byFieldVDM & DM_VDM_COLORMODEL)
	{
		m_DeviceType = eColor ; // color 
	}
	else
	{
		m_DeviceType = eMono; //mono
	}
	wDeviceType = m_DeviceType;
    if(!m_bIsPrinterIcon)
    {
        long lIndex = static_cast<LONG>(SendDlgItemMessage(hDlg, IDC_CMB_PRNSEL, CB_GETCURSEL, 0, 0));
        long lCount = 0;
        short DeviceType = 1;
       // long ppm = 0;

		//Save the Printer selection combo list and 
		//also the printer selection if 
#if 1	//Printer selection save from UD3.1
		CShRegMdlTbl	*pregMdlTbl = NULL;
		long			i;
		PUDMRI			pudmri = NULL;
		UDMRI			uTempUdmri = { 0 };
		DWORD			dwNeeded = 0;
		WCHAR			*szPortName = new WCHAR[MAX_LENGTH];
		WCHAR			*szPortName2 = new WCHAR[MAX_LENGTH];
		//Coverity Fix: 91272 - SSDI Seetharam : 20210503
		//SecureZeroMemory(szPortName, sizeof(szPortName));
		//SecureZeroMemory(szPortName2, sizeof(szPortName));
		SecureZeroMemory(szPortName, MAX_LENGTH * sizeof(WCHAR));
		SecureZeroMemory(szPortName2, MAX_LENGTH * sizeof(WCHAR));
		PRINTER_INFO_2	*ppi2 = NULL;
		PRINTER_DEFAULTS pd = { NULL, NULL, PRINTER_ACCESS_USE };
		HANDLE			hPrinter = NULL;
		//CShRegPort		*pRegPort = NULL;
		pregMdlTbl = new CShRegMdlTbl(m_hInstance, m_pPrinterName/*, m_pLog*/, m_pmcf);
		if (pregMdlTbl == NULL)
		{
			//goto EXIT;
		}

		if(NULL != m_MfpSlctInfo)
		{
			if (NULL != m_MfpSlctInfo->pudmriList)
			{
				if (0 < m_MfpSlctInfo->lCount)
				{
					for (i = 0; i < m_MfpSlctInfo->lCount; i++)
					{
						pudmri = m_MfpSlctInfo->pudmriList[i];
						(*pregMdlTbl).SetModelRegInfo((*pudmri).wIndex, pudmri);
						//(*pRegPort).AddPortStdTcpIpPort(m_pPrinterName, pudmri);
						regApi.AddPortStdTcpIpPort(pudmri, m_pPrinterName);
						if (m_MfpSlctInfo->lCheckIndex == pudmri->wIndex)
						{
							memcpy(&uTempUdmri, pudmri, sizeof(UDMRI));
							lPPM = (*pudmri).wPPM;
							ModelType = (*pudmri).blModelType;
							BWprint = (*pudmri).blBWprint;
							ManuallyAdd = (*pudmri).iManuallyAdded;
							(*pudmri).iManuallyAdded = (0 == ManuallyAdd) ? 0 : eManuallyAdded;
							m_bBWPrint = BWprint;
							//TBD
							//memcpy(&(*m_ppi).ipif.pudc->udmri, pudmri, sizeof(UDMRI));

							sh_OpenPrinter(m_pPrinterName, &hPrinter, &pd);
							if (hPrinter != NULL)
							{
								dwNeeded = 0;
								sh_GetPrinter(hPrinter, 2, NULL, 0, &dwNeeded);

								ppi2 = (PRINTER_INFO_2 *)new char[dwNeeded + 1];
								if (ppi2 == NULL)
								{
									//goto EXIT;
								}

								SecureZeroMemory(ppi2, dwNeeded);

								sh_GetPrinter(hPrinter, 2, (LPBYTE)ppi2, dwNeeded, &dwNeeded);

								//wcscpy_s(szPortName, sizeof(szPortName), (*ppi2).pPortName);
								wcscpy_s(szPortName, 512, (*ppi2).pPortName);
							}

							wsprintf((LPWSTR)szPortName2, L"UD3L_%s", (*pudmri).szPortName);

							if (hPrinter == NULL)
								// (*pRegPort).ChangePortStdTcpIpPort(m_pszSvrPrnName, (*pudmri).szPortName);
								regApi.ChangePortStdTcpIpPort(m_pPrinterName, (*pudmri).szPortName);
							else
							{
								if (::lstrcmpiW((LPCWSTR)szPortName, (LPCWSTR)szPortName2) != 0)
									// (*pRegPort).ChangePortStdTcpIpPort(m_pszSvrPrnName, (*pudmri).szPortName);
									regApi.ChangePortStdTcpIpPort(m_pPrinterName, (*pudmri).szPortName);
								else
								{
									if (::lstrlenW((LPCWSTR)szPortName) != ::lstrlenW((LPCWSTR)szPortName2))
										// (*pRegPort).ChangePortStdTcpIpPort(m_pszSvrPrnName, (*pudmri).szPortName);
										regApi.ChangePortStdTcpIpPort(m_pPrinterName, (*pudmri).szPortName);
								}
							}

							if (ppi2 != NULL)
								delete[] ppi2;

							if (hPrinter != NULL)
								::ClosePrinter(hPrinter);
						}
					}
					//(*pRegPort).AddPortStdTcpIpPort(m_pPrinterName, &uTempUdmri);
					regApi.AddPortStdTcpIpPort(&uTempUdmri, m_pPrinterName);
				}
			}

			//<S><A> SSDI Bug Fix#4063 - SSDI:Seetharam 20210727
			if(m_dwSetupType == eEnhancedMode)
			{
				//SSDI Bug Fix#3619 - SSDI:Seetharam 20210512
				WCHAR FAR		*pszEntry = NULL;
				pszEntry = new WCHAR[SCUI_REGKEYSIZE];
				::ZeroMemory(pszEntry, SCUI_REGKEYSIZE * sizeof(WCHAR));
				wcscpy_s(pszEntry, SCUI_REGKEYSIZE, REG_ENT_MFPSRCH_ALWAYSDISP);
				pregMdlTbl->sh_SetPrinterData(m_pPrinterName, pszEntry, REG_DWORD, (LPBYTE)&m_MfpSlctInfo->dwChkAlwaysDisp, sizeof(m_MfpSlctInfo->dwChkAlwaysDisp));
				//Coverity Fix:91465 - SSDI:Seetharam - 20210518
				delete[]pszEntry;
				pszEntry = NULL;
			}
			//<S><E> SSDI Bug Fix#4063 - SSDI:Seetharam 20210727
		}

		
		if(NULL != m_MfpSlctInfo)
		{
			//<S><A>To Fix Bug #4274,2021-07.September,SSDI:Chanchal Singla
			//Printer driver language changes to English when Default Printer is changed from General tab
			//<S><C>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
			//In Basic mode Printing Preferences is not displayed as per the Language set
			SHORT dwLangID = -1;
			CShRegMdlTbl RegMdlTbl(ghInstance, m_pPrinterName, m_pmcf);
			RegMdlTbl.GetLangID(&dwLangID);
			//m_MfpSlctInfo->lLangID = regApi.GetLangID(m_pPrinterName, 0);
			m_MfpSlctInfo->lLangID = (LONG)dwLangID;
			//<E>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
			//<E>To Fix Bug #4274,2021-07.September,SSDI:Chanchal Singla
			regApi.SetDeviceType(m_pPrinterName, m_DeviceType);
			regApi.SetLangID(m_pPrinterName, m_MfpSlctInfo->lLangID);
			if (eEnhancedMode == m_dwSetupType)
			{
				regApi.SetCheckPos(m_pPrinterName, m_MfpSlctInfo->lCheckIndex, m_MfpSlctInfo->lLangID, m_DeviceType);
				regApi.SetCheckPosInHKLM(m_pPrinterName, m_MfpSlctInfo->lCheckIndex);
				regApi.SetCount(m_pPrinterName, m_MfpSlctInfo->lCount);
				regApi.SetStartPos(m_pPrinterName, m_MfpSlctInfo->lStartPos);
				regApi.SetChkSelectModelWithOutDiscovery(m_pPrinterName, m_MfpSlctInfo->dwModelWithOutDisc);
			}
		}
		//Coverity Fix: 91254 - SSDI Seetharam : 20210504
		delete []szPortName;
		szPortName = NULL;
		delete []szPortName2;
		szPortName2 = NULL;
#endif	//Printer selection save from UD3.1
//<S><C>To fix retrive language drop down ,20210713 SSDI:Goutham
		//if (m_MfpSlctInfo == NULL)
		//{
		//	CreateMfpSlctInfo();
		//}
        BOOL bEnable = FALSE;
		if (m_MfpSlctInfo != NULL)
		{
			if (regApi.IsPrinterOperators(m_pPrinterName))
			{
				if (((TRUE == regApi.m_bClusterFlag) || (FALSE == regApi.IsSharedServerSide(m_pPrinterName))) && (m_MfpSlctInfo->lCount > 1))
				{
					bEnable = TRUE;
				}
			}
		}
//<S><E>To fix retrive language drop down ,20210713 SSDI:Goutham
        ::EnableWindow(GetDlgItem(hDlg,IDC_CMB_PRNSEL),bEnable);
		//regApi.SetDeviceType(m_pPrinterName, DeviceType);
        //<S><A> RedMine Ticket(#1025) ìBlack and White Printî value is not retained first time when cancel is clicked on mono ui.: 2014.02.24, SSDI:Sonali
        //Calling UpdateDevmodeBasedOnDeviceType() to update devmode when Apply or OK is clicked from Printer Properties.
       // regApi.UpdateDevmodeBasedOnDeviceType(m_hPrinter,DeviceType);
        //<E> RedMine Ticket(#1025) ìBlack and White Printî value is not retained first time when cancel is clicked on mono ui.: 2014.02.24, SSDI:Sonali
		//<S><A>Coverity fix 91262#3 - SSDI-Seetharam:20210428
		if (pregMdlTbl != NULL)
		{
			delete pregMdlTbl;
			pregMdlTbl = NULL;
		}
		//<S><A>Coverity fix 91262#3 - SSDI-Seetharam:20210428
    }

	//BOOL bIsIndexChanged = FALSE;
	BOOL bIsManuallyEdited = (eManuallyEdited == ManuallyAdd) ? TRUE : FALSE;
	//<S><C>Fix for SOL-1 feedback #39 - SSDI-Seetharam:20210223
	//if (FALSE == m_bIsPrinterIcon && 
	//	0 != m_pOemDevMode.scpi.dev.nTargetDeviceID
	//	)
	//{
	//	bIsIndexChanged =  m_pOemDevMode.scpi.dev.nTargetDeviceID == m_uCurrentItemIndex + 1 ? FALSE : TRUE;
	//}
	if (FALSE == m_bIsPrinterIcon)
	{
		//<S><C> SSDI Bug #3785 Fix - SSDI Seetharam: 20210618
		//if (m_uPreviousItemIndex == -1 && m_uCurrentItemIndex >= 0)
		//{
		//	ExecuteAutoConfiguration(FALSE);
		//	bIsIndexChanged = TRUE;
		//}
		//else if (0 != m_pOemDevMode.scpi.dev.nTargetDeviceID)
		//{
		//	bIsIndexChanged = m_pOemDevMode.scpi.dev.nTargetDeviceID == m_uCurrentItemIndex + 1 ? FALSE : TRUE;
		//}

		UDMRI udmri = { 0 };
		regApi.GetModelRegInfo(m_pPrinterName, g_CurrentIndex, &udmri);
		//<S><A>To Fix Redmine Bug #3848- Issue 2,2022.03.16,SSDI:Chanchal Singla
		//General tab color value displays as yes even though Color Mode value is set to Black and White in the Configuration tab
		// Staple value has not been updated first time in the general tab, Similar behavior observed in UD3.1, hence fixed remaining issues
		//in the case of enhanced mode, before executing autoconfiguration DriverCapabilities values should be updated properly.
		if (eEnhancedMode == m_dwSetupType)
		{
			BOOL wStaple = (0 != m_pOemDevMode.scpi.dev.byFieldVDMStaple) ? 1 : 0;
			SHORT wTempDeviceType = (udmri.blBWprint == 1) ? eMono : eColor;
			regApi.UpdateDriverCapabilities(m_pPrinterName, m_hStringResourceHandle, wTempDeviceType, IDS_INDEX0_TS_PSIZE, lPPM, wStaple);
		}
		//<E>To Fix Redmine Bug #3848- Issue 2,2022.03.16,SSDI:Chanchal Singla
		if (m_bExecuteSilentAutoConfig && udmri.iManuallyAdded == 0)
		{
			ExecuteAutoConfiguration(FALSE);
		}
		//<S><E> SSDI Bug #3785 Fix - SSDI Seetharam: 20210618
		else
		{
			//Reset the printer date here and then refresh the 
			//configuration tab UI with the new printer's values
		}
	}
	//<S><E>Fix for SOL-1 feedback #39 - SSDI-Seetharam:20210223
	
	UpdateDevicetypeAndColorInfo(BWprint, wDeviceType,
		m_bExecuteSilentAutoConfig, bIsManuallyEdited);


	// color mode
	if (m_pOemDevMode.scpi.pub.dmColor == DMCOLOR_MONOCHROME)
		m_PropertySheetState.wColMode = colmode_mono;
	else
	{
		if (m_pOemDevMode.scpi.ext.ColData.byColMode == colmode_mono)
			m_PropertySheetState.wColMode = colmode_auto;
		else
			m_PropertySheetState.wColMode = m_pOemDevMode.scpi.ext.ColData.byColMode;
	}

    DWORD dwLangID = static_cast<DWORD>(SendDlgItemMessage(hDlg, IDC_CMB_LANGSEL, CB_GETCURSEL, 0, 0));
	if (dwLangID != m_dwLangID)
	{
		m_dwLangID = dwLangID;
		// CRegistryAPI regApi;
		regApi.SetLangID(m_pPrinterName, m_dwLangID);
		m_pOemDevMode.scpi.dev.nLanguageID = (short)m_dwLangID;
		//	<S><A> To Fix Redmine ,feedback 51,Bug #4017,2021.07.14,SSDI: Chanchal Singla
		//Bug #4017 :-Watermark Drop down values are not localized when we changed the language.
		//Feedback issue 51(Watermark option "None" is not changed based on the language selection
		
		TCHAR szWaterMarkDatFilePath[_MAX_PATH] = { 0 };
		GetProjectFileName(szWaterMarkDatFilePath, L"wm62.dat");
		CShIniFile cshinifile(ghInstance, m_pPrinterName, szWaterMarkDatFilePath, FALSE);

		WCHAR			szDll[MAX_PATH] = { 0 };
		HMODULE hStringResourceHandle = NULL;
		LoadStringResource(m_pPrinterName, szDll) ;
		hStringResourceHandle = ::LoadLibrary(szDll);

		CWaterMarkRegistryData pWatermarkregdata(ghInstance,
			m_pPrinterName, hStringResourceHandle);

		pWatermarkregdata.resetEx(m_pPrinterName, &cshinifile);
		pWatermarkregdata.WriteWMLangID(m_pPrinterName, (short)m_dwLangID);
		if (hStringResourceHandle)
		{
			FreeLibrary(hStringResourceHandle);
			hStringResourceHandle = NULL;
		}
		//	<E> To Fix Redmine Bug #4017,2021.07.14,SSDI: Chanchal Singla
	}
//<S><A>To Update staple value in general Information tab,301220 SSDI:Goutham
	BOOL wStaple = (0 != m_pOemDevMode.scpi.dev.byFieldVDMStaple) ? 1 : 0;
//<S><E>To Update staple value in general Information tab,301220 SSDI:Goutham

	//<S><A>To Fix Redmine Bug #4703,2022.01.18,SSDI:Chanchal Singla
	//Bug #4703: Color Mode value in Configuration Tab does not update correctly
	m_pOemDevMode.scpi.dev.byFieldVDM = (wDeviceType == eColor) ?
		(m_pOemDevMode.scpi.dev.byFieldVDM | DM_VDM_COLORMODEL) : (m_pOemDevMode.scpi.dev.byFieldVDM & ~DM_VDM_COLORMODEL);
	//<E>To Fix Redmine Bug #4703,2022.01.18,SSDI:Chanchal Singla

	regApi.SetDeviceType(m_pPrinterName, wDeviceType);
	//<S><C>To Fix Redmine Bug #4566,December.13.2021, SSDI:Chanchal Singla
	//	Add another printer icon : Color mode drop down is enabled for Mono model
	if (m_bIsPrinterIcon)
	{
		long lIndex = 0;
		long lCount = 0;
		UDMRI udmri = { 0 };
		regApi.GetRegisterMFPCountAndCheckPos(m_pPrinterName, lCount, lIndex);
		LRESULT lRet = regApi.GetModelRegInfo(m_pPrinterName, lIndex, &udmri);
		m_bBWPrint = udmri.blBWprint;
		regApi.SetBWPRint(m_pPrinterName, m_bBWPrint);
	}
	else
	{
		regApi.SetBWPRint(m_pPrinterName, m_bBWPrint);
	}
	//<E>To Fix Redmine Bug #4566,December.13.2021, SSDI:Chanchal Singla
	//<S><A>To Update staple value in general Information tab,301220 SSDI:Goutham
    //regApi.UpdateDriverCapabilities(m_pPrinterName, m_hStringResourcHandle, wDeviceType, IDS_INDEX0_TS_PSIZE, lPPM);
	regApi.UpdateDriverCapabilities(m_pPrinterName, m_hStringResourceHandle, wDeviceType, IDS_INDEX0_TS_PSIZE, lPPM,wStaple);
//<S><E>To Update staple value in general Information tab,301220 SSDI:Goutham	
	if (eManuallyEdited == ManuallyAdd)
	{
         //<S><C>To fix redmine bug #3156 ,20210713 SSDI:Goutham
		//m_bBWPrint will indicate the value to Enable or GreyOut the ColorMode Combo.
		if (BWprint)
		{
			  // black and white
			m_pOemDevMode.scpi.dev.byFieldVDM &= ~DM_VDM_COLORMODEL;
			SetControlState(hDlg, IDC_CMB_COLORMODULE, SCUI_STATE_SHOW);
		}
		else
		{
			m_pOemDevMode.scpi.dev.byFieldVDM |= DM_VDM_COLORMODEL;
			SetControlState(hDlg, IDC_CMB_COLORMODULE, SCUI_STATE_ON);
		}
        //<S><E>To fix redmine bug #3156 ,20210713 SSDI:Goutham
	}

// <S><A> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
	WriteDEVDModeValueToRegistry();
// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V

	//<E>To the Support of MimicControl,20201412,SSDI:Chanchal Singla
	DevmodeToPropstate();
	//<S><C>To Fix Redmine #3151 Issue 3,26.august.2021,SSDI:Chanchal Singla
	//Issue 3:-Silent Autoconfiguration is not executing from General tab
	//SetIndexChanged
	DevdmodeToReg((char FAR *)m_pPrinterName, &m_pOemDevMode.scpi);
	regApi.Set_WM_MFP_INDEX_CHANGE(m_pPrinterName, FALSE);
	regApi.Set_Manually_MFP_INDEX_CHANGE(m_pPrinterName, FALSE);
	//<S><A>To Fix Redmine #Bug #4186,01.04.2022,SSDI:Chanchal Singla
	//Configuration Tab : Silent auto configuration fails when driver is installed through installer.
	// Motive: to update the L"AutoConfigStatus"  value True(Logical One),while clicking apply button in configuration tab,so  ExecuteAutoConfig() should not call evrytime,
	// that function should be call only one time when we install driver first time with valid ip Address in basic mode thorugh installer
	regApi.SetAutoConfigStatus(m_pPrinterName, TRUE);
	//<E>To Fix Redmine #Bug #4186,01.04.2022,SSDI:Chanchal Singla
	//<E>To Fix Redmine #3151 Issue 3,26.august.2021,SSDI:Chanchal Singla
	//<S><A>To the Support of Save And Retrieving the GPD Feature,20201123,SSDI:Chanchal Singla
	DWORD dwPairsWritten = 0;
	DWORD dwResult = 0;
	BOOL bCustomPaperSelected = FALSE;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UINT FeatureCount = (UINT)m_pFeatureInfoList[0].nNummberOfFeatures;
	DWORD NumberOfChangedFeatureOptionPairs = 0;

	for (UINT uFeatureIndex = 0; uFeatureIndex < FeatureCount; uFeatureIndex++)
	{
		if (m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uDefaultOptionIndex != m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex)
		{
			NumberOfChangedFeatureOptionPairs++;
		}
	}

	if (0 < NumberOfChangedFeatureOptionPairs)
	{
		UINT uChangedFeatureOptionList = 0;
		PRINT_FEATURE_OPTION* pfo = new PRINT_FEATURE_OPTION[NumberOfChangedFeatureOptionPairs]();
		for (UINT uFeatureIndex = 0; uFeatureIndex < FeatureCount; uFeatureIndex++)
		{
			if (m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uDefaultOptionIndex != m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex)
			{
				m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uDefaultOptionIndex = m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex;
				pfo[uChangedFeatureOptionList].pszFeature = m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszFeature;
				pfo[uChangedFeatureOptionList].pszOption = m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption;
				uChangedFeatureOptionList++;
			}
		}

		HRESULT hRes = m_pPrintCoreHelperUni->SetOptions(NULL, 0, SETOPTIONS_FLAG_KEEP_CONFLICT, pfo, NumberOfChangedFeatureOptionPairs, &dwPairsWritten, &dwResult);
		bCustomPaperSelected = FALSE;
		if (NULL != pfo)
		{
			delete[] pfo;
			pfo = NULL;
		}
		else
		{
			slog.putLog("CDevicePropertyPage::SaveFeatureOptions -> NULL_OBJECT(pfo)\n");
		}
	}

	//<E>To the Support of Save And Retrieving the GPD Feature,20201123,SSDI:Chanchal Singla	
	m_bExecuteSilentAutoConfig = FALSE;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		ApplyShareFavForJson();
	}
	else
	{
		//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
		ApplyShareFav();
		//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	}
}

//=============================================================================
// function
//      DevdmodeToReg
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      pscdm               LPSCDM
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      DevdmodeÇÉåÉWÉXÉgÉäÇ…ÉZÉbÉgÇ∑ÇÈ
//
//=============================================================================
BOOL CDevicePropertyPage::DevdmodeToReg(char FAR *pszSvrPrnName, LPSCDM pscdm)
{
	HANDLE				hPrinter = NULL;
	DWORD				dwSize = 0;
	DWORD				dwType = 0;
	DWORD				dwRet = 0;
	PRINTER_DEFAULTS	pd;
	BOOL				bRet = FALSE;
	DWORD				dwRecp = BSM_APPLICATIONS;

	// OpenPrinterÇ≈Open
	// OPEN
	pd.DesiredAccess = PRINTER_ALL_ACCESS;
	pd.pDatatype = NULL;
	pd.pDevMode = NULL;

	if (sh_OpenPrinter((WCHAR *)pszSvrPrnName, &hPrinter, &pd) != FALSE) {
		// SetPrintData()Ç≈Write
		dwSize = sizeof(SCDM);
		dwType = REG_BINARY;
		if (IsUnicodeOS()) {
			dwRet = ::SetPrinterDataW(
				hPrinter,
				(LPWSTR)L"Private DevMode",
				//					REG_PRIVETE_DEVMODE2W,
				dwType,
				(LPBYTE)pscdm,
				dwSize);
		}
		else {
			dwRet = ::SetPrinterData(
				hPrinter,
				(LPWSTR)"Private DevMode",
				//					REG_PRIVETE_DEVMODE2A,
				dwType,
				(LPBYTE)pscdm,
				dwSize);
		}
		if (dwRet == ERROR_SUCCESS) {
			// ãNìÆÇµÇƒÇ¢ÇÈÉAÉvÉäÇ…ÅADevmodeÇ™ïœÇÌÇ¡ÇΩÇ±Ç∆Çí ím
			::BroadcastSystemMessage(
				BSF_FORCEIFHUNG,
				&dwRecp,
				WM_DEVMODECHANGE,
				(WPARAM)0,
				(LPARAM)pszSvrPrnName);
			bRet = TRUE;
		}
	}

	if (hPrinter != NULL) {
		// CLOSE
		ClosePrinter(hPrinter);
		hPrinter = NULL;
	}
	return bRet;
}

//=============================================================================
// function
//      WriteDEVDModeValueToRegistry
//
// parameters
//      None

// return value
//     None
//
// outline
//      Writes DEVDMode value to registry .
//=============================================================================
void CDevicePropertyPage::WriteDEVDModeValueToRegistry()
{
	CRegistryAPI regApi(m_hInstance);
	regApi.WriteData(m_pPrinterName, &m_pOemDevMode.scpi.dev);
}


HRESULT CDevicePropertyPage::ShowHelp(LPHELPINFO pHelpInformation)
{
	HRESULT hr = E_FAIL;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(pHelpInformation)
	{
		DWORD			dwHelpIDs[3] = {0};
		if(CreateHtmlHelpArray(dwHelpIDs, m_pHelpID, (*pHelpInformation).iCtrlId) != FALSE)
		{
			HtmlHelp((HWND)(*pHelpInformation).hItemHandle, m_pHelpFilePath, HH_HELP_CONTEXT, (DWORD_PTR)dwHelpIDs[1]);
			hr = S_OK;
		}
		if( S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	return hr;
}

// <S><A> Added the new methods required to process the custom controls, 20200414 - SSDI:Seetharam
//To get the resource id info the given gontrol id
PRESOURCEIDINFO CDevicePropertyPage::GetResourceIDInfoOfControl(std::string strFeatureName)
{
	ResourceIDInfoMap::iterator ResourceInfoIterator = m_pResourceIDInfoMap->find(strFeatureName);
	if (ResourceInfoIterator != m_pResourceIDInfoMap->end())
	{
		return &(ResourceInfoIterator->second);
	}
	return NULL;
}

//=============================================================================
// function
//      CreateMessageClass
//
// return value
//     VOID
//
// outline
//     Create Message class object .
//=============================================================================
VOID CDevicePropertyPage::CreateMessageClass(_In_ CONST HWND   hDlg)
{
	m_ObjCShowMessage = new CShowMessage(hDlg, m_hStringResourceHandle);
}

// <S><A> To handle the ondraw for custom controls like custom combo box , 20200128 - SSDI:Seetharam
//=============================================================================
// function
//      OnDrawItem
//
// return value
//     NILL
//
// outline
//    Draws Owner drawn button.
//=============================================================================
HRESULT CDevicePropertyPage::OnWmDrawItem(_In_ CONST HWND   hDlg, _In_ CONST WPARAM wParam, _In_ CONST LPARAM lParam)
{
	//MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	short				wCtrlID = (short)wParam;
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;
	HRESULT hResult = S_OK;
	CDevicePropertyPage* pPropertyPageInstance = NULL;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
	{
		if (NULL != pPropertyPageInstance)
		{
			//hResult = pPropertyPageInstance->OnDrawItem(hDlg, wParam, lParam);
		}
	}

	if (pPropertyPageInstance != NULL)
	{
		UIControlMap::const_iterator iComboIterator = pPropertyPageInstance->m_UIControls.find(pdis->CtlID);
		INT iIncrementIndex = 0;
		if (iComboIterator != pPropertyPageInstance->m_UIControls.end())
		{
			CUIControl * pControl = static_cast<CUIControl*>(iComboIterator->second);
			if (pControl != NULL)
			{
				if (pControl->GetControlType() == eComboBox)
				{
					if (NULL != pControl)
					{
						//<S><M> Moved and updated the code to do custom draw. SSDI: Seetharam - 20200623
						if (pControl->GetFeatureControlID() == IDC_CMB_LANGSEL)
						{
							((CLangCombo *)pControl)->OnWmDrawItem(hDlg, wParam, lParam);
						}
						else if(pControl->GetFeatureControlID() == IDC_CMB_PRNSEL)
						{
							((CPrinterSelectionCombo *)pControl)->OnWmDrawItem(hDlg, wParam, lParam);
						}
						else
						{
							((CShImageCombo *)pControl)->OnWmDrawItem(hDlg, wParam, lParam);
						}
						//<S><E> Updated the code to do custom draw. SSDI: Seetharam - 20200623
					}
					//Fixed Coverity issue - 90530#2 - SSDI:Seetharam-20201006
					//else
					//{
					//	slog.putLog("CMainTabPropertyPage::SetPamphletIndex -> NULL_OBJECT(CCombo)\n");
					//}
				}
				//<S><A> Added the condition for Image button. SSDI:Seetharam - 20200212
				else if (pControl->GetControlType() == eButton)
				{
					if (NULL != pControl)
					{
						((CShBtnBmpTxt*)pControl)->OnWmDrawItem(hDlg, wParam, lParam);
					}
					//Fixed Coverity issue - 90530#1 - SSDI:Seetharam-20201006
					//else
					//{
					//	slog.putLog("CMainTabPropertyPage::SetPamphletIndex -> NULL_OBJECT(CCombo)\n");
					//}
				}
				//<S><E> Added the condition for Image button. SSDI:Seetharam - 20200212
			}
		}
	}
	//<S><A>To the Support of Mimic Control According To Ud3.1,20201123,SSDI:Chanchal Singla
	{
		switch (wCtrlID)
		{
		case IDC_BTN_SHARED_BBOX:
		{
			UIControlMap::const_iterator Iterator = pPropertyPageInstance->m_UIControls.find(IDC_BTN_SHARED_BBOX);
			if (Iterator != pPropertyPageInstance->m_UIControls.end())
			{
				CMimicImageConfig *pMimic = static_cast<CMimicImageConfig*>(Iterator->second);
				if (pMimic)
				{
					pMimic->Draw(&m_PropertySheetState);
				}
				
			}
		}
		break;
		case IDC_BTN_SHARED_BBOX_SUB:
		{
			UIControlMap::const_iterator Iterator = pPropertyPageInstance->m_UIControls.find(IDC_BTN_SHARED_BBOX_SUB);
			if (Iterator != pPropertyPageInstance->m_UIControls.end())
			{
				CMimicText *pMimicText = static_cast<CMimicText*>(Iterator->second);
				if (pMimicText)
				{
					pMimicText->Draw(&m_PropertySheetState);
				}
			}
		}
		break;
		default:
			break;
		}
	}
	//<E>To the Support of Mimic Control According To Ud3.1,20201123,SSDI:Chanchal Singla
	return 0L;
}
// <S><E> To handle the ondraw for custom controls like custom combo box
// <S><A> Added the new methods required to process the custom controls, 20200414 - SSDI:Seetharam
//<S><A> Printing Policy Dialog box creation 22/9/2020 K M Harika
BOOL CDevicePropertyPage::CreatePPLCYDlg(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	OEMDEV m_ptempPrivateDevmode = {};
	memcpy_s(&m_ptempPrivateDevmode, sizeof(OEMDEV), &m_pOemDevMode, sizeof(OEMDEV));
	CShDlgPntPolicy *pdlg = new CShDlgPntPolicy(IDD_DLG_PRINTING_POLICY, m_hStringResourceHandle, m_hInstance,
		m_hBitmapResourceHandle, m_hPrinter,m_hDialogResourceHandle, m_pFeatureInfoList, m_DefaultSettings,
		m_pResourceIDInfoMap, &m_ptempPrivateDevmode, &m_PropertySheetState, m_ObjCShowMessage, m_pPrinterName);
	if (NULL == pdlg)
	{
		slog.putLog("CDevicePropertyPage::CreatePPLCYDlg -> NULL_OBJECT(pdlg)\n");
	}

	INT_PTR DlgRet = IDCANCEL;
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_PRINTING_POLICY), hDlg, (DLGPROC)(*pdlg).WindowMapProc, (LPARAM)pdlg);
	if (NULL == DlgRet || -1 == DlgRet)
	{
		slog.putLog("CDevicePropertyPage::CreatePPLCYDlg -> INVALID_POINTER(DlgRet)\n");
	}
	if(IDOK == DlgRet)
	{
		CUIControl pControl;
		if (FALSE == pControl.IsEqualBuf(&m_pOemDevMode, &m_ptempPrivateDevmode, sizeof(OEMDEV)))
		{
			memcpy_s(&m_pOemDevMode, sizeof(OEMDEV), &m_ptempPrivateDevmode, sizeof(OEMDEV));
			PropSheet_Changed(GetParent(hDlg), hDlg);
			pControl.EnableApplyButton(hDlg);
		}
	//<S><A>To the Support of MimicControl,20201412,SSDI:Chanchal Singla
		MimicConstraintsForPunchStaple();
	//<E>To the Support of MimicControl,20201412,SSDI:Chanchal Singla
	}
	delete pdlg;
	pdlg = NULL;
	return TRUE;
}
//<E> Printing Policy Dialog box creation 22/9/2020 K M Harika

// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
//=============================================================================
// function
//       SetControlState
//
// parameters
//       hDlg         
//       wCtrlID          
//		 cStatus
// return value
//      
//
// outline
//       SetControlState based on status 
//=============================================================================
void CDevicePropertyPage::SetControlState(HWND hDlg, short wCtrlID, BYTE cStatus)
{
	HWND			hWnd = NULL;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	hWnd = GetDlgItem(hDlg, wCtrlID);
	if (hWnd == NULL)
	{
		slog.putLog("CDevicePropertyPage::SetControlState -> NULL_HANDLE(hWnd)\n");
		return;
	}

	// --- show / hide
	if (cStatus & SCUI_STATE_SHOW)
		ShowWindow(hWnd, SW_SHOW);
	else
		ShowWindow(hWnd, SW_HIDE);

	// --- enable / disable
	if (cStatus & SCUI_STATE_ENABLE)
		EnableWindow(hWnd, TRUE);
	else
		EnableWindow(hWnd, FALSE);
}

//=============================================================================
// function
//       SetTabState
//
// parameters
//       hDlg         
//                 
//		 
// return value
//      
//
// outline
//       SetTabState  
//=============================================================================
VOID CDevicePropertyPage::SetTabState(
	_In_ CONST HWND   hDlg, BOOL bSilentAutoConfig)
{
	//<S><C>To the Support of MimicControl,20201412,SSDI:Chanchal Singla
	if (0 == m_PropertySheetState.wColorOption) // Color
	{
		if (m_PropertySheetState.byAutoColorMode == bool_true)
		{
			m_pOemDevMode.scpi.ext.ColData.byColMode = colmode_color;
		}
		else
		{
			if (m_pOemDevMode.scpi.ext.ColData.byColMode != colmode_color)
			{
				m_pOemDevMode.scpi.ext.ColData.byColMode = colmode_auto;
				
			}
		}
	}
	
	if (m_pOemDevMode.scpi.pub.dmColor == DMCOLOR_MONOCHROME)
		m_PropertySheetState.wColMode = colmode_mono;
	else
	{
		if (m_pOemDevMode.scpi.ext.ColData.byColMode == colmode_mono)
			m_PropertySheetState.wColMode = colmode_auto;
		else
			m_PropertySheetState.wColMode = m_pOemDevMode.scpi.ext.ColData.byColMode;
	}
	StapleModuleToStapleType(&m_PropertySheetState);
	PunchModuleToPunchType(&m_PropertySheetState);
	//<S><C>To Fix Feedback #53 ,2021.august.2021,SSDI:Chanchal Singla
	//The controls on Configuration Tab are not grayed out even though
	//the user does not have the authority of Managed Printer.
	BOOL blOperatable = IsPrinterOperators(m_pPrinterName);
	BYTE byStatus = SCUI_STATE_SHOW;
	if (blOperatable)
	{
		byStatus = SCUI_STATE_ON;
	}
	else if (TRUE == m_bDisablePrnSel)
	{
		byStatus = SCUI_STATE_SHOW;
	}

	if (eBasicMode == m_dwSetupType)
	{
		SetControlState(m_hPropertySheetHandle, IDC_CMB_PRNSEL, SCUI_STATE_OFF);
		SetControlState(m_hPropertySheetHandle, IDC_BTN_PRNSEL, SCUI_STATE_OFF);
		SetControlState(m_hPropertySheetHandle, IDC_GRP_PRNSEL, SCUI_STATE_OFF);
		SetControlState(m_hPropertySheetHandle, IDC_GRP_LANGSEL, SCUI_STATE_OFF);
		SetControlState(m_hPropertySheetHandle, IDC_STC_LANGSEL_MSG, SCUI_STATE_OFF);
		SetControlState(m_hPropertySheetHandle, IDC_CMB_LANGSEL, SCUI_STATE_OFF);
	}
	else
	{
		if (m_dwIsPrinterIcon || (TRUE == m_bDisablePrnSel))
		{
			SetControlState(m_hPropertySheetHandle, IDC_GRP_PRNSEL, SCUI_STATE_SHOW);
			SetControlState(m_hPropertySheetHandle, IDC_CMB_PRNSEL, SCUI_STATE_SHOW);
			SetControlState(m_hPropertySheetHandle, IDC_BTN_PRNSEL, SCUI_STATE_SHOW);
		}
		else
		{
			SetControlState(m_hPropertySheetHandle, IDC_GRP_PRNSEL, byStatus);
			BYTE cControlStatus = byStatus;
			if (SCUI_STATE_ON == cControlStatus)
			{
				long lCount = SendMessageW(GetDlgItem(m_hPropertySheetHandle, IDC_CMB_PRNSEL), CB_GETCOUNT, 0, 0);
				cControlStatus = 1 < lCount ? SCUI_STATE_ON : SCUI_STATE_SHOW;
			}
			SetControlState(m_hPropertySheetHandle, IDC_CMB_PRNSEL, cControlStatus);
			SetControlState(m_hPropertySheetHandle, IDC_BTN_PRNSEL, byStatus);
		}
	}
	//<E>To Fix Feedback #53 ,2021.august.2021,SSDI:Chanchal Singla
	//--staple less
	if ((0 == m_PropertySheetState.wStapleOption) || (1 == m_PropertySheetState.wStapleOption))
	{
		SetControlState(hDlg, IDC_CHK_STAPLE_STAPLELESS, SCUI_STATE_SHOW);
		m_PropertySheetState.byStapleless = 0;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLELESS].pszOption = "OFF";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLELESS].uChangedOptionIndex = bool_false;
	}
	else
	{
		SetControlState(hDlg, IDC_CHK_STAPLE_STAPLELESS, byStatus);
	}
	//<S><C>To Fix Feedback #53 ,2021.august.2021,SSDI:Chanchal Singla
	SelectCheckBoxFeatureOption(hDlg, IDC_CHK_STAPLE_STAPLELESS, m_PropertySheetState.byStapleless);
//<E>To the Support of MimicControl,20201412,SSDI:Chanchal Singla
//<S><A>To the Support of AutoConfiguration UI Feature Values updataion,2020-18-12,SSDI:Chanchal Singla
	 //-- SignOn
	SelectCheckBoxFeatureOption(hDlg, IDC_CHK_SINGLE_SIGNON, m_PropertySheetState.wSingleSignOn);
	SetControlState(hDlg, IDC_CHK_SINGLE_SIGNON, byStatus);
	
	//-- AutoColorMode
	SelectCheckBoxFeatureOption(hDlg, IDC_CHK_AUTO_COLORMODE,!m_PropertySheetState.byAutoColorMode);
	SetControlState(hDlg, IDC_CHK_AUTO_COLORMODE, byStatus);
	
	//-- PIN_COMPATIBILITY
	SelectCheckBoxFeatureOption(hDlg, IDC_CHK_PIN_COMPATIBILITY, m_PropertySheetState.byPinCompatibility);	
	SetControlState(hDlg, IDC_CHK_PIN_COMPATIBILITY, byStatus);
	
	//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	//-- Share Favorites
	SelectCheckBoxFeatureOption(hDlg, IDC_CHK_SHARE_FAV, m_PropertySheetState.bShare);
	SetControlState(hDlg, IDC_CHK_SHARE_FAV, byStatus);
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	
	SetControlState(hDlg, IDC_BTN_EXTRA_SETTINGS, byStatus);

	//-- ColorMode
	SelectComboFeatureOption( hDlg, IDC_CMB_COLORMODULE, m_PropertySheetState.wColorOption);
	EnableOrDisableControl(hDlg, IDC_CMB_COLORMODULE, IDC_STC_COLORMODE, byStatus);
	
	//-- Staple Option
	SelectComboFeatureOption(hDlg, IDC_CMB_STAPLE_OPT, m_PropertySheetState.wStapleOption);
	EnableOrDisableControl(hDlg, IDC_CMB_STAPLE_OPT, IDC_STC_STAPLE_OPT, byStatus);
	
	//-- Punch Option
	SelectComboFeatureOption(hDlg, IDC_CMB_PUNCH_OPT, m_PropertySheetState.wPunchOption);
	EnableOrDisableControl(hDlg, IDC_CMB_PUNCH_OPT, IDC_STC_PUNCH_OPT, byStatus);
	
	//--- Print Relase Combo
	SelectComboFeatureOption(hDlg, IDC_CMB_PRN_RELEASE, m_PropertySheetState.byPrintRelease);
	EnableOrDisableControl(hDlg, IDC_CMB_PRN_RELEASE, IDC_STC_PRN_RELEASE, byStatus);
	
	//-- MaxPaperSize
	SelectComboFeatureOption(hDlg, IDC_CMB_MAXPAPERSIZE, m_PropertySheetState.byMaxPaperSize);
	EnableOrDisableControl(hDlg, IDC_CMB_MAXPAPERSIZE, IDC_STC_MAXPAPERSIZE, byStatus);
    
	//<S><C>To fix redmine bug #3156 ,20210713 SSDI:Goutham
	CRegistryAPI regApi;
	regApi.GetBWPRint(m_pPrinterName,m_bBWPrint);
	//CShRegMdlTbl RegMdlTbl(m_hInstance, m_pPrinterName, m_pmcf/*, NULL*/);
	//RegMdlTbl.GetBWPRint(m_bBWPrint);
    //<S><E>To fix redmine bug #3156 ,20210713 SSDI:Goutham
	BOOL bSilentConfig = bIsSilentAutoConfig();

	if (TRUE == m_bBWPrint)	//mfp is mono
	{
		if (m_bDefaultClicked == TRUE)
		{
			//case1 - Basic mode: Silent auto configuration - true
			if (m_dwSetupType == eBasicMode)
			{
				if (bSilentConfig == TRUE)
					EnableOrDisableControl(hDlg, IDC_CMB_COLORMODULE, IDC_STC_COLORMODE, SCUI_STATE_SHOW);
				else
					EnableOrDisableControl(hDlg, IDC_CMB_COLORMODULE, IDC_STC_COLORMODE, SCUI_STATE_ON);
			}
			else
			{
				EnableOrDisableControl(hDlg, IDC_CMB_COLORMODULE, IDC_STC_COLORMODE, SCUI_STATE_SHOW);
			}
		}
		else
		{
			if (m_PropertySheetState.wColorOption == 0)//Color	//Case 3 - first open dialog bDefaultClicked is FALSE 
			{
				EnableOrDisableControl(hDlg, IDC_CMB_COLORMODULE, IDC_STC_COLORMODE, SCUI_STATE_ON);
			}
			else
			{
				if (m_bColorBWSelected)
					EnableOrDisableControl(hDlg, IDC_CMB_COLORMODULE, IDC_STC_COLORMODE, SCUI_STATE_ON);
				else
					EnableOrDisableControl(hDlg, IDC_CMB_COLORMODULE, IDC_STC_COLORMODE, SCUI_STATE_SHOW);
			}
		}
	}
	else
	{
		EnableOrDisableControl(hDlg, IDC_CMB_COLORMODULE, IDC_STC_COLORMODE, byStatus);
	}
	m_bDefaultClicked = FALSE;
	//<E>To the Support of AutoConfiguration UI Feature Values updataion,2020-18-12,SSDI:Chanchal Singla
	
	// <S><D> SSDI Bug# 4133 Fix - SSDI:Seetharam - 20210805
	//if (bSilentAutoConfig)
	//{
	//	PropSheet_UnChanged(GetParent(hDlg), hDlg);
	//}
	// <S><E> SSDI Bug# 4133 Fix - SSDI:Seetharam - 20210805

	SetControlState(hDlg, IDC_BTN_AUTO_CONF, byStatus);
	SetControlState(hDlg, IDC_BTN_EXTRA_SETTINGS, byStatus);
	
	//<E>To Fix Feedback #53 ,2021.august.2021,SSDI:Chanchal Singla
	if ((blOperatable == FALSE) && (eBasicMode == m_dwSetupType))
	{
		short	wExptCtrl[] = {
								 IDC_BTN_ABOUT,
								 -1
		};
		DisableAllCtrls(wExptCtrl);
	}
}

// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V

//=============================================================================
// function
//       SetTabState
//
// parameters
//       hDlg         
//                 
//		 
// return value
//      
//
// outline
//       SetTabState  
//=============================================================================
LRESULT CDevicePropertyPage::OnIdcBtnPrnSelCfg(
	_In_ CONST HWND   hDlg)
{
	return ExecuteAutoConfiguration(FALSE);
}

LRESULT CDevicePropertyPage::ExecuteAutoConfiguration(bool bSilentAutoConfig)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	LRESULT			res = 0;
	long			lRet = IDCANCEL;
	short			wID = 0;
	//bDefaultClicked = FALSE;
	//bColorBWSelected = FALSE;
	CUIControl pControl;
	OEMDEV m_ptempPrivateDevmode = {};
	memcpy_s(&m_ptempPrivateDevmode, sizeof(OEMDEV), &m_pOemDevMode, sizeof(OEMDEV));
	CShRegMdlTbl RegMdlTbl(m_hInstance, m_pPrinterName, m_pmcf/*, NULL*/);
	short wPrevDeviceType = eColor;
	RegMdlTbl.GetDeviceType(&wPrevDeviceType);
	//CShDlgUpdating *pDlg = new CShDlgUpdating(m_hInst, m_hWnd, m_ppi, m_pszSvrPrnName, m_pLog, wPort, IDD_DLG_UPDATING);
	//int wPort = 80;
	//CShDlgUpdating *pDlg = new CShDlgUpdating(NULL, hDlg, m_pPrinterName, wPort, IDD_DLG_UPDATING,
	//	m_hStringResourceHandle, m_hBitmapResourceHandle, m_hPrinter, m_hDialogResourceHandle, &m_PropertySheetState, m_pmcf);
	HWND hWnd = GetActiveWindow();
	//if m_ObjC Show Message variable is NULL, that time driver is crashing, that's why this condition has been added.
	if (m_ObjCShowMessage == NULL)
	{
		m_ObjCShowMessage = new CShowMessage(hWnd, m_hStringResourceHandle);
	}
	//<S><C>To Implement Task#3132,31-05-2024,SSDI:Manoj S
	//called different constructor to support #3132 using GPD Tool.
	/*CShDlgAutoConf *pDlg = new CShDlgAutoConf(m_hInstance, hWnd, m_pPrinterName, IDD_DLG_UPDATING,
		m_hStringResourceHandle, m_hBitmapResourceHandle, m_hPrinter, m_hDialogResourceHandle,
		&m_PropertySheetState, m_pmcf, m_ObjCShowMessage, &m_ptempPrivateDevmode);*/
	CShDlgAutoConf *pDlg = new CShDlgAutoConf(m_hInstance, hWnd, m_pPrinterName, IDD_DLG_UPDATING,
		m_hStringResourceHandle, m_hBitmapResourceHandle, m_hPrinter, m_hDialogResourceHandle,
		&m_PropertySheetState, m_pmcf, m_ObjCShowMessage, &m_ptempPrivateDevmode, m_pFeatureInfoList);
	//<E>To Implement Task #3132,31-05-2024,SSDI:Manoj S
	if (NULL == pDlg)
	{
		slog.putLog("CDevicePropertyPage::OnIdcBtnPrnSelCfg -> NULL_OBJECT(pdlg)\n");
		return res;	//Coverity Fix - 91162
	}

	INT_PTR DlgRet = IDCANCEL;
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCEW(IDD_DLG_UPDATING), hWnd,
		(DLGPROC)(*pDlg).WindowMapProc, (LPARAM)pDlg);
	//<S><A>To the Support of AutoConfiguration UI Feature Values updataion,2020-18-12,SSDI:Chanchal Singla

	if (FALSE == pControl.IsEqualBuf(&m_pOemDevMode, &m_ptempPrivateDevmode, sizeof(OEMDEV)))
	{
		memcpy_s(&m_pOemDevMode, sizeof(OEMDEV), &m_ptempPrivateDevmode, sizeof(OEMDEV));
	}

	DevmodeToPropstate();

	if (dev_state_ok == pDlg->GetAutoConfigurationResult())
	{
		CRegistryAPI regApi;

		//<S><C>To Fix Redmine Bug #4651,Bug #4564,2022.01.18,SSDI:Chanchal Singla
		//Bug #4651:- Black and White check box is disabled, when we reopen Color Printer in Printer Selection.
		//Bug #4564:- Black and White Print Checkbox : ON for Color Models
		m_bBWPrint = FALSE;
		m_DeviceType = eColor;
		//<E>To Fix Redmine Bug #4651,Bug #4564,2022.01.18,SSDI:Chanchal Singla

		if(!bSilentAutoConfig)
			PropSheet_Changed(GetParent(hWnd), hWnd);
		short sDeviceIndex = 0;
		RegMdlTbl.GetCheckPos(&sDeviceIndex);
		//<S><A>To Fix Redmine Bug #4634,2021.20.12,SSDI:Chanchal Singla	
		//	Color mode drop down gets enabled even though Add another printer icon is created for Mono model
		if (m_bIsPrinterIcon)
		{
			long lIndex = 0;
			long lCount = 0;
			UDMRI udmri = { 0 };
			regApi.GetRegisterMFPCountAndCheckPos(m_pPrinterName, lCount, lIndex);
			LRESULT lRet = regApi.GetModelRegInfo(m_pPrinterName, lIndex, &udmri);
			m_bBWPrint = udmri.blBWprint;
			
			if (m_bBWPrint == TRUE)// Black and White
			{
				m_PropertySheetState.wColorOption = 1;
			}
			if (0 == m_PropertySheetState.wColorOption) // color
			{
				m_pOemDevMode.scpi.dev.byFieldVDM |= DM_VDM_COLORMODEL;
			}
			else    // black and white
			{
				m_pOemDevMode.scpi.dev.byFieldVDM &= ~DM_VDM_COLORMODEL;
			}
		}
		//<E>To Fix Redmine Bug #4634,2021.20.12,SSDI:Chanchal Singla
		//<S><A>To Fix Redmine Bug #3791,August.10.2021. SSDI:Chanchal Singla
		//Color mode is not grayed out in Configuration tab after discovering monochrome models
		//CRegistryAPI regApi(ghInstance);
		//UDMRI udmri = { 0 };
		//regApi.GetModelRegInfo(m_pPrinterName, sDeviceIndex + 1, &udmri);

		//m_DeviceType = (udmri.blBWprint) ? 0 : 1;
		//m_bBWPrint = udmri.blBWprint;
		if (m_PropertySheetState.wColorOption == 1) // black and white
		{
			m_DeviceType = eMono;
			m_bBWPrint = TRUE;
		}
		
		if (m_MfpSlctInfo != NULL)
		{
			if (NULL != m_MfpSlctInfo->pudmriList)
			{
				PUDMRI			pudmri = NULL;
				for (long i = 0; i < m_MfpSlctInfo->lCount; i++)
				{
					pudmri = m_MfpSlctInfo->pudmriList[i];
					// m_MfpSlctInfo.lCheckIndex and deviceIndex are different.
					// after clicking on Apply button, both will be the same.
					// in this case,  m_MfpSlctInfo.lCheckIndex is changed and device index is not changed.
					// auto configuration is executed on device index and not on  m_MfpSlctInfo.lCheckIndex
					if (sDeviceIndex == (*pudmri).wIndex)
					{
						m_MfpSlctInfo->pudmriList[i]->blModelType = m_DeviceType;
						m_MfpSlctInfo->pudmriList[i]->blBWprint = m_bBWPrint;
						break;
					}
				}
			}
		}
		
		regApi.SetBWPRint(m_pPrinterName, m_bBWPrint);
		//<E>To Fix Redmine Bug #3791,august.10.2021. SSDI:Chanchal Singla
		//<S><A>Bug #3848 issue 3- Apply button is grayed out,2021.09.15,SSDI:Chanchal Singla
		//General tab color value displays as yes even though Color Mode value is set to Black and White in Configuration tab
		// after run the Autoconfiguration Feature info was not updated properly,
		//so we are updating the feature information by using array
		UINT TempArray[9] = {
			FEATURE_DEVICECOLORMODE,
			FEATURE_DEVICESTAPLEMODULE,
			FEATURE_STAPLELESS,
			FEATURE_DEVICEPUNCHMODULE,
			FEATURE_MAXPAPERSIZE,
			FEATURE_SINGLESIGNON,
			FEATURE_AUTOCOLORMODE,
			FEATURE_PINCOMPATIBILITY,
			FEATURE_DEVICEPRINTRELEASE
		};
		SHORT SelectedArray[9] = {
			m_PropertySheetState.wColorOption ,
			m_PropertySheetState.wStapleOption,
			(SHORT)m_PropertySheetState.byStapleless,
			m_PropertySheetState.wPunchOption,
			(SHORT)m_PropertySheetState.byMaxPaperSize,
			m_PropertySheetState.wSingleSignOn,
			(SHORT)m_PropertySheetState.byAutoColorMode,
			(SHORT)m_PropertySheetState.byPinCompatibility,
			(SHORT)m_PropertySheetState.byPrintRelease 
		};
		size_t nFeatureIndex = 0;
		size_t nNumberOfFeatures = 9;

		for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
		{
			//m_pFeatureInfoList->pFeatureOptionsList[TempArray[nFeatureIndex]].uDefaultOptionIndex = (UINT)SelectedArray[nFeatureIndex];	//<S><D>To fix SSDI issue #4410, Harika, 20211222
			m_pFeatureInfoList->pFeatureOptionsList[TempArray[nFeatureIndex]].uChangedOptionIndex = (UINT)SelectedArray[nFeatureIndex];
		}
		//<E>Bug #3848 issue 3,2021.09.15,SSDI:Chanchal Singla
	}
	else
	{
		if(!bSilentAutoConfig)
			PropSheet_Changed(GetParent(hWnd), hWnd);
	}
	//<E>To the Support of AutoConfiguration UI Feature Values updataion,2020-18-12,SSDI:Chanchal Singla
		//UpdatePunchStapleColorModeValues();


	res = 1L;
	//Coverity Fix: 90893 - SSDI:Seetharam - 20201125
	if (pDlg != NULL)
	{
		delete pDlg;
		pDlg = NULL;
	}
	return res;
}

BOOL CDevicePropertyPage::InitCommonFile(HINSTANCE hInst)
{
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");

	m_pmcf = new CShIniFile(m_hInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	if (m_pmcf == NULL)
		return FALSE;
	return TRUE;
}

//<S><C>To the Support of MimicControl,20201412,SSDI:Chanchal Singla
void CDevicePropertyPage::UpdateDevicetypeAndColorInfo(BOOL bBWPrint,short&	wDeviceType,BOOL bIsIndexChanged,
								BOOL bIsManuallyEdited)
{
	CRegistryAPI cRegAPI;
	//regApi.ReadData(m_pPrinterName, &pOemDev->scpi.dev);
	//If the Model is changed and Model is Manually Edited the Device is updated based on bBWPrint value.
	if ((TRUE == bIsIndexChanged) ||
		(TRUE == bIsManuallyEdited)
		)
	{
		if (eSelected == bBWPrint)
		{
			wDeviceType = eMono;
		}
		else
		{
			wDeviceType = eColor;
		}

	}
	
	eDeviceType enPrevDeviceType = eColor;
	WORD			wRet = 0;
	cRegAPI.GetDeviceType(m_pPrinterName, enPrevDeviceType);
	
	//Based on the Current Device Type and Previous Device Type the Devmode values are updated.
	if (wDeviceType != enPrevDeviceType)
	{
		m_pOemDevMode.scpi.pub.dmColor = (eColor == wDeviceType) ? DMCOLOR_COLOR : DMCOLOR_MONOCHROME;
		m_pOemDevMode.scpi.pub.dmICMMethod = (eColor == wDeviceType) ? DMICMMETHOD_DEVICE : DMICMMETHOD_NONE;
		m_PropertySheetState.wColMode = (eColor == wDeviceType) ? colmode_auto : colmode_mono;
		m_pOemDevMode.scpi.ext.ColData.byColMode = (eColor == wDeviceType) ? colmode_auto : colmode_mono;
		cRegAPI.SetDeviceType(m_pPrinterName,wDeviceType);

		short wModelID = COLOR_MODEL_ID;
		if (eColor == wDeviceType)
		{
			//wModelID = 130;
			wModelID = (short)COLOR_MODEL_ID;
		}
		else
		{
			//wModelID = 131;
			wModelID = (short)MONO_MODEL_ID;
		}
		wModelID = wModelID << 8;
		wRet |= wModelID;
		m_pOemDevMode.scpi.wFieldSPI = wRet;
		
		//IndexChanged is False SetPrinter needs to called here as in case if Index is changed SetPrinter
		// is called in Core UI.
		if (FALSE == bIsIndexChanged)
		{
			cRegAPI.UpdateDevmodeBasedOnDeviceType(m_hPrinter, wDeviceType);
		}
	}
}

//=============================================================================
// Function :
//			StapleModuleToStapleType
//
// Parameters :
//			PPROPSTATE pps  
//
// Return value :
//			 none
// Outline : 
//			This function sets staple type in property state and in ext devmode. 
//
//=============================================================================
void CDevicePropertyPage::StapleModuleToStapleType(PPROPSTATE pps)
{
	
	switch ((*pps).wStapleOption)
	{
	case stapleopt_1:
		pps->wStaple = stapleopt_1;
		break;
	case stapleopt_2:
	case stapleopt_3:
		pps->wStaple = stapleopt_2;
		break;
	default:
		pps->wStaple = stapleopt_none;
		break;
	}

	if (pps->wStaple == staple_none && pps->wStapleOption == stapleopt_none)
	{
		m_pOemDevMode.scpi.ext.byStaple = STAPLE_NONE;
	}


	if (pps->wStaple == staple_1 && m_pOemDevMode.scpi.ext.byStaple == staple_2)
	{
		pps->wStaple = staple_none;
	}
	else if (pps->wStaple == staple_1 && m_pOemDevMode.scpi.ext.byStaple == staple_1_stapleless)
	{
		pps->wStaple = staple_none;
	}
	else if (pps->byStapleless == 0 && m_pOemDevMode.scpi.ext.byStaple == staple_1_stapleless)
	{
		pps->wStaple = staple_none;
	}
	else if (pps->wStapleOption != stapleopt_3 && m_pOemDevMode.scpi.ext.byStaple == staple_saddle_staple)
	{
		pps->wStaple = staple_none;
	}
	else
	{
		pps->wStaple = m_pOemDevMode.scpi.ext.byStaple;
	}
}
//=============================================================================
//
// Function : 
//			PunchModuleToPunchType
// Parameters :  
//			PPROPSTATE pps          
// Return value : 
//			none
// Outline : 
//			This function sets punch type in property state and in ext devmode. 
//
//=============================================================================
void CDevicePropertyPage::PunchModuleToPunchType(PPROPSTATE pps)
{
	pps->wPunch = bool_false;
	switch (m_pOemDevMode.scpi.ext.byPunchType)
	{
	case PUNCH_2HOLES:
		if ((m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_1) ||
			(m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_2) ||
			(m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_3))
		{
			pps->wPunch = bool_true;
		}
		break;
	case PUNCH_3HOLES:
		if ((m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_2) ||
			(m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_6))
		{
			pps->wPunch = bool_true;
		}
		break;
	case PUNCH_4HOLES:
		if ((m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_3) ||
			(m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_7))
		{
			pps->wPunch = bool_true;
		}
		break;
	case PUNCH_4HOLES_WIDE:
		if ((m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_4))
		{
			pps->wPunch = bool_true;
		}
		break;
	default:
		pps->wPunch = bool_false;
		break;
	}
}
//=============================================================================
// Function :
//			UpdatePunchModuleToPropstate
//
// Parameters :
//			nil
//
// Return value :
//			 SHORT
// Outline : 
//			This function sets Punch type in property state from devmode. 
//
//=============================================================================
SHORT CDevicePropertyPage::UpdatePunchModuleToPropstate()
{
	SHORT punch = punchtype_none;
	switch (m_pOemDevMode.scpi.dev.byPunchmodule)
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
	return punch;
}
//=============================================================================
// Function :
//			UpdatePropstateToPunchModule()
//
// Parameters :
//			nil
//
// Return value :
//			 SHORT
// Outline : 
//			This function sets byPunchmodule in devmode from property state. 
//
//=============================================================================
SHORT CDevicePropertyPage::UpdatePropstateToPunchModule()
{
	SHORT punchmodule = punchtype_none;
	switch (m_PropertySheetState.wPunchOption)
	{
	case punchtype_none:				// 0 none
		punchmodule = punchopt_none;
		break;
	case punchtype_1 :				// 1 //2 Holes
		punchmodule = punchopt_1;
		break;
	case  punchtype_2:				// 2 //3 Holes
		punchmodule = punchopt_6;
		break;
	case punchtype_3 :				// 3 //4 Holes
		punchmodule = punchopt_7;
		break;
	case punchtype_4 :				// 4 //4(Wide) Holes
		punchmodule = punchopt_4;
		break;
	case punchtype_5:				// 5 //2/3 Holes
		punchmodule = punchopt_2;
		break;
	case punchtype_6 :				// 6 //2/4 Holes
		punchmodule = punchopt_3;
		break;
	default:
		break;
	}
	return punchmodule;
}
//=============================================================================
// Function :
//			MimicConstraintsForPunchStaple()
//
// Parameters :
//			nil
//
// Return value :
//			 none
// Outline : 
//			Mimic Constraints For PunchStaple
//
//=============================================================================
VOID CDevicePropertyPage::MimicConstraintsForPunchStaple()
{
	// For a Punch Type Mimic Display
	switch (m_pOemDevMode.scpi.ext.byPunchType)
	{
	case PUNCH_2HOLES:
		if ((m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_1) ||
			(m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_2) ||
			(m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_3))
		{
			m_PropertySheetState.wPunch = 1;
		}
		else
		{
			m_pOemDevMode.scpi.ext.byPunchType = 0;
			m_PropertySheetState.wPunch = 0;
		}
		break;
	case PUNCH_3HOLES:
		if ((m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_2) ||
			(m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_6))
		{
			m_PropertySheetState.wPunch = 1;
		}
		else
		{
			m_pOemDevMode.scpi.ext.byPunchType = 0;
			m_PropertySheetState.wPunch = 0;
		}
		break;
	case PUNCH_4HOLES:
		if ((m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_3) ||
			(m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_7))
		{
			m_PropertySheetState.wPunch = 1;
		}
		else
		{
			m_pOemDevMode.scpi.ext.byPunchType = 0;
			m_PropertySheetState.wPunch = 0;
		}
		break;
	case PUNCH_4HOLES_WIDE:
		if ((m_pOemDevMode.scpi.dev.byPunchmodule == punchopt_4))
		{
			m_PropertySheetState.wPunch = 1;
		}
		else
		{
			m_pOemDevMode.scpi.ext.byPunchType = 0;
			m_PropertySheetState.wPunch = 0;
		}
		break;
	default:
		m_PropertySheetState.wPunch = 0;
		m_pOemDevMode.scpi.ext.byPunchType = 0;
		break;
	}

	// For a Staple Mimic Display
	switch (m_pOemDevMode.scpi.ext.byStaple)
	{
	case staple_1:
		if (m_PropertySheetState.wStaple == staple_none)
		{
			m_pOemDevMode.scpi.ext.byStaple = staple_none;
		}
		break;
	case staple_2:
		if (m_PropertySheetState.wStaple == staple_none ||
			m_PropertySheetState.wStaple == staple_1)
		{
			m_pOemDevMode.scpi.ext.byStaple = staple_none;
		}
		break;
	case staple_1_stapleless:
		if (m_PropertySheetState.wStaple == staple_none ||
			m_PropertySheetState.byStapleless == 0)
		{
			m_pOemDevMode.scpi.ext.byStaple = staple_none;
		}
		break;
	case staple_saddle_staple:
		if (m_PropertySheetState.wStaple == staple_none ||
			m_PropertySheetState.wStaple == staple_1 ||
			m_PropertySheetState.wStaple == staple_2)
		{
			m_pOemDevMode.scpi.ext.byStaple = staple_none;
		}
		break;
	default:
		break;
	}
}
//<E>To the Support of MimicControl,20201412,SSDI:Chanchal Singla
//<S><A>To the Support of AutoConfiguration UI Feature Values updataion,2020-18-12,SSDI:Chanchal Singla
//=============================================================================
// function
//      SelectComboFeatureOption
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
//		LONG
// return value
//      Void
//
// outline
//      SelectComboFeatureOption.
//=============================================================================
VOID CDevicePropertyPage::SelectComboFeatureOption(HWND hDlg, WORD wCtrlID, LONG lVal)
{
	UIControlMap::const_iterator Iterator;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	Iterator = m_UIControls.find(wCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			//pImageCombo->Set_isDevicePropertyPage(FALSE);
			pImageCombo->Select(wCtrlID, hDlg, lVal);
			pImageCombo->SelectFeatureOption(wCtrlID, hDlg);
		}
		else
		{
			slog.putLog("CDevicePropertyPage::Select -> NULL_OBJECT(pImageCombo)\n");
		}

	}
}

//=============================================================================
// function
//      SelectCheckBoxFeatureOption
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
//		LONG
// return value
//      Void
//
// outline
//      SelectCheckBoxFeatureOption.
//=============================================================================
VOID CDevicePropertyPage::SelectCheckBoxFeatureOption(HWND hDlg, WORD wCtrlID, LONG lVal)
{
	UIControlMap::const_iterator Iterator;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	Iterator = m_UIControls.find(wCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CCheckBox *pCheckBox = static_cast<CCheckBox*>(Iterator->second);
		if (pCheckBox)
		{
			CheckDlgButton(hDlg, wCtrlID,(UINT)lVal);
			//pCheckBox->Set_isDevicePropertyPage(FALSE);
			pCheckBox->SetCheckBoxValue( hDlg,wCtrlID, (UINT)lVal);
		}
		else
		{
			slog.putLog("CDevicePropertyPage::Select -> NULL_OBJECT(pCheckBox)\n");
		}

	}
}

//=============================================================================
// function
//      EnableOrDisableControl
//
// parameters
//     short wDialogID,short wStringID,BYTE cControlStatus
//
// return value
//     None
//
// outline
//     Enables Or Disables Controls.
//=============================================================================
void CDevicePropertyPage::EnableOrDisableControl(HWND hDlg,
	short wDialogID,
	short wStringID,
	BYTE cControlStatus
)
{
	INT nCount = SendMessage(::GetDlgItem(hDlg, wDialogID), CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
	if (SCUI_STATE_ON == cControlStatus)
	{
		cControlStatus = 1 < nCount ? SCUI_STATE_ON : SCUI_STATE_SHOW;
	}
	SetControlState(hDlg,wDialogID, cControlStatus);
	SetControlState(hDlg,wStringID, cControlStatus);
}
//<E>To the Support of AutoConfiguration UI Feature Values updataion,2020-18-12,SSDI:Chanchal Singla

//=============================================================================
// function
//       OnIdcClickSPN
//
// parameters
//       hDlg         
//                 
//		 
// return value
//      
//
// outline
//       SetTabState  
//=============================================================================
LRESULT CDevicePropertyPage::OnIdcClickSPN(
	_In_ CONST HWND   hDlg)
{
	LRESULT			res = 0;
	CShDlgSPN		*pdlg = NULL;
	BOOL			blFlag = FALSE;
	
	//<S><C>To Fix Redmine #7721,12-07-2024,SSDI:Manoj S
	//pdlg = new CShDlgSPN(m_hInst, hWnd, m_ppi, m_pszSvrPrnName, m_pLog, IDD_DLG_UPDATING);
	//pdlg = new CShDlgSPN(m_hInstance, hDlg, m_pPrinterName, 0, IDD_DLG_UPDATING,
	//	m_hStringResourceHandle, m_hBitmapResourceHandle, m_hPrinter, m_hDialogResourceHandle, &m_PropertySheetState, m_pmcf, m_ObjCShowMessage, &m_pOemDevMode);
	pdlg = new CShDlgSPN(m_hInstance, hDlg, m_pPrinterName, 0, IDD_DLG_UPDATING,
		m_hStringResourceHandle, m_hBitmapResourceHandle, m_hPrinter, m_hDialogResourceHandle, &m_PropertySheetState, m_pmcf, m_ObjCShowMessage, &m_pOemDevMode, m_pFeatureInfoList);
	//<E>To Fix Redmine #7721,12-07-2024,SSDI:Manoj S
	
	if (pdlg == NULL)
		goto EXIT;

	if (::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCEW(IDD_DLG_UPDATING),
		hDlg, (DLGPROC)(*pdlg).WindowMapProc, (LPARAM)pdlg) == IDOK)
	{
		if (m_PropertySheetState.wSingleSignOn != bool_false) {
			blFlag = TRUE;
		}
	}
	else {
		m_PropertySheetState.wSingleSignOn = bool_false;
	}

	delete pdlg;

	if (blFlag == FALSE) {
		(*m_ObjCShowMessage).Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_75, MB_ICONINFORMATION | MB_OK);
	}

	res = 1;

EXIT:

	return res;
}

BOOL CDevicePropertyPage::bIsSilentAutoConfig()
{
	DWORD dwData = 0;
	DWORD dwSize = 0;
	BOOL bSilentConfig = FALSE;	//normal installation by default(no silent autoconfig as done while installing via installer setup.exe)
	WCHAR FAR *pszLclMcIPAddr = NULL;
	pszLclMcIPAddr = new WCHAR[SCUI_REGKEYSIZE];
	SecureZeroMemory(pszLclMcIPAddr, SCUI_REGKEYSIZE);
	WCHAR FAR *pszCurUsrIPAddr = NULL;
	pszCurUsrIPAddr = new WCHAR[SCUI_REGKEYSIZE];
	SecureZeroMemory(pszCurUsrIPAddr, SCUI_REGKEYSIZE);
	//Coverity Fix: 91229 & 91234 - SSDI:Seetharam - 20210518
	//WCHAR FAR *pszBase = NULL;
	//pszBase = new WCHAR[SCUI_REGKEYSIZE];
	//::lstrcpy(pszBase, REG_KEY_ACM_ROOT_BASE);
	//WCHAR FAR	*pszTemp = NULL;
	//pszTemp = new WCHAR[SCUI_REGKEYSIZE];
	WCHAR FAR	*pszKey = NULL;
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	WCHAR FAR *pszSilent = L"Silent";
	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	//::lstrcpy(pszTemp, REG_KEY_ACM_ROOT_BASE);
	//convert_wchar(pszKey, pszTemp, SCUI_REGKEYSIZE);
	wcscpy_s(pszKey, SCUI_REGKEYSIZE, REG_KEY_ACM_ROOT_BASE);

	CShRegMdlTbl			*preg = NULL;
	preg = new CShRegMdlTbl(m_hInstance, m_pPrinterName, m_pmcf);
	if (preg == NULL)
		goto EXIT;

	BOOL blSuccess = FALSE;
	if ((*preg).CreateRegPath(pszKey, pszKey, SCUI_REGKEYSIZE, m_pPrinterName) != FALSE)
	{
		dwSize = sizeof(DWORD);
		blSuccess = (*preg).ReadRegData(HKEY_LOCAL_MACHINE, pszKey, pszSilent, REG_DWORD, (WCHAR FAR *)&dwData, &dwSize);
		if (!blSuccess)
			goto EXIT;

		preg->GetIPAddress(pszCurUsrIPAddr);
		if (wcscmp((LPCWSTR)pszCurUsrIPAddr, L"") == 0)
			goto EXIT;

		blSuccess = preg->SearchIfAutoDiscovered(pszCurUsrIPAddr);
		if (!blSuccess)
			goto EXIT;

		bSilentConfig = blSuccess;
	}

EXIT:
	// Coverity Fix: 91232 - SSDI Seetharam - 20210504
	if (pszLclMcIPAddr != NULL)
		delete []pszLclMcIPAddr;
	if (pszCurUsrIPAddr != NULL)
		delete []pszCurUsrIPAddr;
	//if (pszBase != NULL)
	//	delete []pszBase;
	//if (pszTemp != NULL)
	//	delete []pszTemp;
	if (pszKey != NULL)
		delete []pszKey;
	if (preg != NULL)
		delete preg;

	return bSilentConfig;
}

void CDevicePropertyPage::GetBWPrint(CShRegMdlTbl *pRegMdlTbl)
{
	if (NULL == pRegMdlTbl)
	{
		return;
	}
	//TBD seetharam
	if ((FALSE == m_dwIsPrinterIcon) && (0 < m_pOemDevMode.scpi.dev.nTargetDeviceID))
	{
		// Registered Printers/ Manually added Models
		UDMRI udmri = {};
		pRegMdlTbl->GetModelRegInfo(m_pOemDevMode.scpi.dev.nTargetDeviceID, &udmri, BWPRINT);
		m_bBWPrint = udmri.blBWprint;
	}
	else
	{
		// Basic Mode / Dummy Model(Enhanced type /Create Printer Icon
		pRegMdlTbl->GetBWPRint(m_bBWPrint);
	}
}

//=============================================================================
// function
//        CreateMfpSlctInfo
//
// parameters
//
// return value
//        ê¨å˜ : TRUE
//        é∏îs : FALSE
//
// outline
//        WM_NOTIFY - PSN_APPLY
//=============================================================================
BOOL CDevicePropertyPage::CreateMfpSlctInfo()
{
	BOOL			blRet = FALSE;
	CShRegMdlTbl	*pregMdlTbl = NULL;

	WCHAR FAR		*pszEntry = NULL;
	DWORD			dwType;
	DWORD			dwSize;

	short			wPos;
	long			lColumn;
	PUDMRI			pudmri = NULL;
	//LogStartFunc("CShTabSettingsBase::CreateMfpSlctInfo");


	pregMdlTbl = new CShRegMdlTbl(m_hInstance, m_pPrinterName, m_pmcf);
	if (pregMdlTbl == NULL)
	{
		//LogError("Could not allocate pregMdlTbl!!!");
		goto EXIT;
	}

	//<S><A>Merging UPD specific changes from UD2.5. 2015.09.14 : SSDI:Sonali
	//<S><A> Task #1617: Support Basic mode, 2015.06.11, SSDI:Gajanana D S
	GetBWPrint(pregMdlTbl);

	if ((eBasicMode == m_dwSetupType) && (bIsSilentAutoConfig() == FALSE) && (m_bBWPrint == TRUE))
	{
		//Do nothing
	}
	else
	{
		if (TRUE == m_bBWPrint)
			m_PropertySheetState.wColorOption = 1;
	}

	//pregMdlTbl->GetInstallationType(m_dwSetupType);

	m_MfpSlctInfo = new DLGMFPSLCT_INFO();
	SecureZeroMemory(m_MfpSlctInfo, sizeof(DLGMFPSLCT_INFO));

	if (eBasicMode == m_dwSetupType)
	{
		(*pregMdlTbl).GetLangID((short*)&m_MfpSlctInfo->lLangID);
		m_pOemDevMode.scpi.dev.nTargetDeviceID = 0;
	}
	else
	{
		//<E> Task #1617: Support Basic mode, 2015.06.11, SSDI:Gajanana D S
		//<E>Merging UPD specific changes from UD2.5. 2015.09.14 : SSDI:Sonali

		(*pregMdlTbl).GetCheckPos((short*)&m_MfpSlctInfo->lCheckIndex, (short*)&m_MfpSlctInfo->lLangID);
		(*pregMdlTbl).GetCount((short*)&m_MfpSlctInfo->lCount);
		(*pregMdlTbl).GetStartPos((short*)&m_MfpSlctInfo->lStartPos);
		(*pregMdlTbl).GetChkSelectModelWithOutDiscovery(&m_MfpSlctInfo->dwModelWithOutDisc);

		pszEntry = new WCHAR[SCUI_REGKEYSIZE];
		if (pszEntry == NULL)
		{
			//LogError("Could not allocate pszEntry!!!");
			goto EXIT;
		}
		::ZeroMemory(pszEntry, SCUI_REGKEYSIZE);
		//convert_wchar(pszEntry, REG_ENT_MFPSRCH_ALWAYSDISP, SCUI_REGKEYSIZE);

		dwSize = 0;
		dwType = REG_DWORD;
		m_MfpSlctInfo->dwChkAlwaysDisp = bool_false;
		if (ERROR_SUCCESS != (*pregMdlTbl).sh_GetPrinterData(m_pPrinterName, REG_ENT_MFPSRCH_ALWAYSDISP, &dwType, (LPBYTE)&m_MfpSlctInfo->dwChkAlwaysDisp, sizeof(m_MfpSlctInfo->dwChkAlwaysDisp), &dwSize))
		{
			m_MfpSlctInfo->dwChkAlwaysDisp = bool_true;
		}

		if (m_MfpSlctInfo->lCount != 0)
		{
			if (m_MfpSlctInfo->lCheckIndex > 0)
			{
				UDMRI udmri;
				if(ERROR_FILE_NOT_FOUND == (*pregMdlTbl).GetModelRegInfo(m_MfpSlctInfo->lCheckIndex, &udmri))
				{
					(*pregMdlTbl).SetCheckPos(0, 0, 1);
					(*pregMdlTbl).SetCount(0);
					(*pregMdlTbl).SetStartPos(0);
					goto EXIT;
				}
			}
			m_MfpSlctInfo->pudmriList = new PUDMRI[m_MfpSlctInfo->lCount];
			if (m_MfpSlctInfo->pudmriList == NULL)
			{
				//LogError("Could not allocate m_MfpSlctInfo.pudmriList!!!");
				goto EXIT;
			}

			::SecureZeroMemory(m_MfpSlctInfo->pudmriList, sizeof(PUDMRI) * m_MfpSlctInfo->lCount);

			wPos = (short)m_MfpSlctInfo->lStartPos;
			for (lColumn = 0; lColumn < m_MfpSlctInfo->lCount; lColumn++)
			{
				pudmri = new UDMRI;
				if (pudmri == NULL)
				{
					//LogError("Could not allocate pudmri!!!");
					goto EXIT;
				}

				::ZeroMemory(pudmri, sizeof(UDMRI));
				(*pregMdlTbl).GetModelRegInfo((short)wPos, pudmri);
				//When the port is changed for Creater Printer icon, need to update the new
				//port in udmri structure for client use, since client does not have permission 
				//to write the changed port in local machine of the registry
				//if ((*pregMdlTbl).IsPrinterIcon()) // Check whether driver is a Printer Icon
				if (IsPrinterIcon()) // Check whether driver is a Printer Icon
				{
					if (IsSharedServerSide(m_pPrinterName/*, m_pLog*/) == TRUE)
					{
						(*pregMdlTbl).IsPortChanged(pudmri->szPortName); // Check whether Port is changed
					}
				}
				wPos = (*pudmri).wNext;
				m_MfpSlctInfo->pudmriList[lColumn] = pudmri;
			}
		}
		//<S><A>Merging UPD specific changes from UD2.5. 2015.09.14 : SSDI:Sonali
		//<S><A> Task #1617: Support Basic mode, 2015.06.11, SSDI:Gajanana D S
	}
	//<E> Task #1617: Support Basic mode, 2015.06.11, SSDI:Gajanana D S
	//<E>Merging UPD specific changes from UD2.5. 2015.09.14 : SSDI:Sonali
EXIT:

	if (pregMdlTbl != NULL)
		delete pregMdlTbl;

	if (pszEntry != NULL)
		delete[] pszEntry;

	//LogEndFunc("CShTabSettingsBase::CreateMfpSlctInfo");
	return blRet;
}


//=============================================================================
// function
//        DeleteMfpSlctInfo
//
// parameters
//
// return value
//        ê¨å˜ : TRUE
//        é∏îs : FALSE
//
// outline
//        WM_NOTIFY - PSN_APPLY
//=============================================================================
BOOL CDevicePropertyPage::DeleteMfpSlctInfo()
{
	long	i;
	PUDMRI	pudmri = NULL;

	if(m_MfpSlctInfo != NULL)
	{
		if (m_MfpSlctInfo->pudmriList != NULL)
		{
			//for (i = 0; i < m_MfpSlctInfo->lCount; i++)
			//{
			//	pudmri = m_MfpSlctInfo->pudmriList[i];
			//	delete pudmri;
			//}
			delete[] m_MfpSlctInfo->pudmriList;
			m_MfpSlctInfo->pudmriList = NULL;
		}
		//SSDI Bug Fix:3549-SSDI:Seetharam-20210408
		delete m_MfpSlctInfo;
		m_MfpSlctInfo = NULL;
	}
	return TRUE;
}

//=============================================================================
// function : IsPrinterIcon
// parameters : none           
// return value : DWORD
// outline : This function returns value of member variable m_dwIsPrinterIcon. 
// If m_dwIsPrinterIcon is 1 , then selected driver is a Printer Icon

//=============================================================================
BOOL CDevicePropertyPage::IsPrinterIcon()
{
	return m_dwIsPrinterIcon;
}

//=============================================================================
// function : CheckPrinterIcon
// parameters : pszSvrPrnName             
// return value : void
// outline : This function checks whether selected driver is a printer icon. 
// If driver is a Printer Icon , value of m_dwIsPrinterIcon is set to 1. 

//============================================================================
void CDevicePropertyPage::CheckPrinterIcon(WCHAR FAR *pszSvrPrnName)
{
	CShRegMdlTbl	*pregMdlTbl = NULL;
	pregMdlTbl = new CShRegMdlTbl(m_hInstance, m_pPrinterName, m_pmcf);
	if (pregMdlTbl == NULL)
	{
		return;
	}	
	
	DWORD dwType = REG_DWORD;
	DWORD dwNeeded = 0;
	// Check whether entry 	"CreatePrinterIcon" exists in registry for selected printer driver.
	(*pregMdlTbl).sh_GetPrinterData(pszSvrPrnName, REG_ENT_HKCU_CREATE_PRINTERICON, &dwType, (LPBYTE)&m_dwIsPrinterIcon, sizeof(m_dwIsPrinterIcon), &dwNeeded, TRUE);

	delete pregMdlTbl;
}

LRESULT CDevicePropertyPage::OnIdcBtnPrnClicked(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	UIControlMap* pComponents = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();

	WCHAR pDiscoveryDLLName[_MAX_PATH] = { 0 };
	GetProjectFileName(pDiscoveryDLLName, DISCOVERYMODULENAME);
	HMODULE	hDiscoveryModule = ::LoadLibrary(pDiscoveryDLLName);
	if (NULL != hDiscoveryModule)
	{
		pfnShowDiscoveryUI pDF_CreateMyDlg = NULL;
		pDF_CreateMyDlg = (pfnShowDiscoveryUI)::GetProcAddress(hDiscoveryModule, "ShowDiscoveryUI");

		long uCurrentMFPIndex = static_cast<LONG>(SendDlgItemMessage(hDlg, IDC_CMB_PRNSEL, CB_GETCURSEL, 0, 0));
		BOOL bIsDevicePropertySheetEvent = TRUE;
		if(m_MfpSlctInfo == NULL)
			CreateMfpSlctInfo();
		else
		{
			m_MfpSlctInfo->lCheckIndex = g_CurrentIndex;
		}
		BOOL bResult = (pDF_CreateMyDlg)(m_pPrinterName, m_hStringResourceHandle, bIsDevicePropertySheetEvent, 
			uCurrentMFPIndex, m_MfpSlctInfo,&m_pOemDevMode.scpi);
		if (TRUE == bResult)
		{
			WCHAR szItem[SCUI_PRNNAME_MAX + 1] = { 0 };
			long lCount = 0;
			short			wIndex = 0;
			long lCheckPos = 0;
			UDMRI udmri = { 0 };
			//CRegistryAPI regApi(ghInstance);
			//regApi.GetRegisterMFPCountAndCheckPos(m_pPrinterName, lCount, lCheckPos);
			//regApi.GetModelNamePortNameFromRegInfo(m_pPrinterName, wIndex, &udmri);
			//g_CurrentIndex = lCheckPos;
			g_CurrentIndex = lCheckPos = m_MfpSlctInfo->lCheckIndex;
			lCount = m_MfpSlctInfo->lCount;
			if (0 == lCount)
			{
				SendMessage(GetDlgItem(hDlg, IDC_CMB_PRNSEL), CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
				if (LoadString(m_hStringResourceHandle, IDS_PRNSEL_NOTSET, szItem, countof(szItem)) > 0)
				{
					SendDlgItemMessage(hDlg, IDC_CMB_PRNSEL, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(szItem));
				}
				SendDlgItemMessage(hDlg, IDC_CMB_PRNSEL, CB_SETCURSEL, lCount, 0);
				::EnableWindow(GetDlgItem(hDlg, IDC_CMB_PRNSEL), FALSE);
			}
			else
			{
				//<S><M> Moved and updated the code to do custom draw. SSDI: Seetharam - 20200623
				CPrinterSelectionCombo *pControl = (CPrinterSelectionCombo *)(*pComponents)[IDC_CMB_PRNSEL];
				pControl->ResetPrinterCombo(hDlg, m_MfpSlctInfo);
				//<S><E> Moved and updated the code to do custom draw. SSDI: Seetharam - 20200623
			}

			//Update the registry with UDMRIList and 
			//memcpy(&udmri, m_MfpSlctInfo->pudmriList[lCheckPos], sizeof(UDMRI));
			//CRegistryAPI regApi;
			//regApi.SetModelReg_PortNameInfo(m_pPrinterName, lCheckPos, udmri.szPortName);
			////Update the nTargetDeviceID of DEVmode to the latest selected IP from the discovery dialog
			m_pOemDevMode.scpi.dev.nTargetDeviceID = lCheckPos;
			//DeleteMfpSlctInfo();

			// Sometimes auto-config does not execute
			//Setting the previous selected index and also the autocongfig flag to TRUE
			if (m_uPreviousItemIndex != g_CurrentIndex)
			{
				m_uPreviousItemIndex = g_CurrentIndex;
				m_bExecuteSilentAutoConfig = TRUE;
			}

			PropSheet_Changed(GetParent(hDlg), hDlg);
		}
		//else
		//{
		//	//SSDI Bug Fix:3549-SSDI:Seetharam-20210408
		//	DeleteMfpSlctInfo();
		//}

		FreeLibrary(hDiscoveryModule);
		hDiscoveryModule = NULL;
	}
	else
	{
		slog.putLog("CDevicePropertyPage::SendCommand -> INVALID_HANDLE\n");
	}

	return 0L;
}
BOOL CDevicePropertyPage::UpdateFeatureListBasedOnDevMode(POEMDEV m_pOemPrivateDevMode)
{
	CGPDData gpddata(m_pPrintCoreHelperUni);
	std::vector<DEFAULTDEVMODEINFO>	 m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();
	UINT SelectedIndex = 0;
	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 0;
	/*Get the GPD feature Count*/
	nNumberOfFeatures = GetGPDFeatureCount();
	//increased the size to support two added features for task #3132,#7484
	UINT FeatureArray[75] = {};

	// Coverity Fix: 91439 - SSDI Seetharam - 20210504
	// Moved from bottom to here so that this check is before the first reference of m_pOemPrivateDevMode
	if (m_pOemPrivateDevMode == NULL)
	{
		return 0;
	}

	SHORT wOri = GetOrientationPPS(m_pOemPrivateDevMode);
	// --- byNupX / byNupY
	SHORT wNup = (short)((m_pOemPrivateDevMode->scpi.ext.byNupX) * (m_pOemPrivateDevMode->scpi.ext.byNupY));
	// --- DM_NUPORDER
	BYTE wNupOrder = GetPPSNupOrder(wNup, m_pOemPrivateDevMode->scpi.ext.byNupOrder, wOri);

	//if (m_pOemPrivateDevMode == NULL)
	//{
	//	return 0;
	//}
	DWORD dwOptions =  m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].dwOptions;
	for (UINT i = 0; i < dwOptions; i++)
	{
		UINT temp = PaperSizeArray[i][0];
		if (temp == (UINT)m_pOemPrivateDevMode->scpi.pub.dmPaperSize)
		{
#if 0	// For a testing purpose Later we remove Following line : SSDI:Chanchal Singla
			WCHAR szTitle[MAX_PATH];
			SecureZeroMemory(&szTitle, MAX_PATH);
			wcscpy_s(szTitle, L"PaperSize");
			WCHAR szMsg[MAX_PATH];
			SecureZeroMemory(&szMsg, MAX_PATH);
			WCHAR szBuffer[MAX_PATH];
			SecureZeroMemory(&szBuffer, MAX_PATH);

			wsprintfW(szBuffer, L"dmPaperSize =  %d    ", m_pOemPrivateDevMode->scpi.pub.dmPaperSize);
		
			MessageBox(NULL, szBuffer, szTitle, MB_OK | MB_ICONEXCLAMATION);
#endif
			SelectedIndex = i;
			break;
		}
	}
	//- FEATURE_PAPERSIZE  0
	FeatureArray[FEATURE_PAPERSIZE] = SelectedIndex;
	//- FEATURE_OREINTATION  1
	SHORT wOrient = m_pOemPrivateDevMode->scpi.pub.dmOrientation;
	SHORT wR180Deg = m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_ROTATE180DEG ? bool_true : bool_false;
	if (wOrient == ori_port)
	{ // Portrait
		SelectedIndex = (wR180Deg == FALSE) ? eFirstOption : eThirdOption;
	}
	else
	{ // Landscape
		SelectedIndex = (wR180Deg == FALSE) ? eSecondOption : eFourthOption;
	}
	FeatureArray[FEATURE_OREINTATION] = SelectedIndex;

	//- FEATURE_PAGESPERSHEET  2
	switch (wNup)
	{
	case 1:
		SelectedIndex = eFirstOption;
		break;
	case 2:
		SelectedIndex = eSecondOption;
		break;

	case 4:
		SelectedIndex = eThirdOption;
		break;

	case 6:
		SelectedIndex = eFourthOption;;
		break;

	case 8:
		SelectedIndex = eFifthOption;
		break;

	case 9:
		SelectedIndex = eSixthOption;
		break;

	case 16:
		SelectedIndex = eSevenOption;
		break;

	default:
		SelectedIndex = eFirstOption;
		break; //Some problem

	}
	FeatureArray[FEATURE_PAGESPERSHEET] = SelectedIndex;

	//- FEATURE_ORDER	3
	SelectedIndex = wNupOrder;
	FeatureArray[FEATURE_ORDER] = SelectedIndex;
	
	//- FEATURE_STAPLING	4
	SHORT wStaple = m_pOemPrivateDevMode->scpi.ext.byStaple;
	SelectedIndex = wStaple;
	FeatureArray[FEATURE_STAPLING] = SelectedIndex;

	//- FEATURE_PUNCH	5
	SHORT wPunch = m_pOemPrivateDevMode->scpi.ext.byPunchType;
	switch (wPunch)
	{
	case PUNCH_NONE:
		SelectedIndex = eFirstOption;
		break;
	case PUNCH_2HOLES:
		SelectedIndex = eSecondOption;
		break;
	case PUNCH_3HOLES:
		SelectedIndex = eThirdOption;
		break;
	case PUNCH_4HOLES:
		SelectedIndex = eFourthOption;;
		break;
	case PUNCH_4HOLES_WIDE:
		SelectedIndex = eFifthOption;
		break;
	default:
		SelectedIndex = eFirstOption;
		break; //Some problem
	}
	FeatureArray[FEATURE_PUNCH] = SelectedIndex;

	//- FEATURE_COLLATE	6
	SelectedIndex = m_pOemPrivateDevMode->scpi.pub.dmCollate == DMCOLLATE_TRUE ? bool_false : bool_true;
	FeatureArray[FEATURE_COLLATE] = SelectedIndex;

	//- FEATURE_BORDER	7
	// --- DM_NUPBORDER
	BYTE wNupBorder = m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_NUPBORDER ? bool_true : bool_false;
	SelectedIndex = wNupBorder;
	FeatureArray[FEATURE_BORDER] = SelectedIndex;

	//- FEATURE_INPUTBIN	8
	SHORT wPaperSrc  = m_pOemPrivateDevMode->scpi.pub.dmDefaultSource;
	switch (wPaperSrc)
	{
	case DMBIN_AUTO:
		SelectedIndex = eFirstOption;
		break;
	case DMBIN_BYPASS_A:
		SelectedIndex = eSecondOption;
		break;
	case DMBIN_TRAY1:
		SelectedIndex = eThirdOption;
		break;
	case DMBIN_TRAY2:
		SelectedIndex = eFourthOption;;
		break;
	case DMBIN_TRAY3:
		SelectedIndex = eFifthOption;
		break;
	case DMBIN_TRAY4:
		SelectedIndex = eSixthOption;
		break;
	default:
		SelectedIndex = eFirstOption;
		break; //Some problem
	}
	FeatureArray[FEATURE_INPUTBIN] = SelectedIndex;

	//- FEATURE_MEDIATYPE	9
	short wPaperType =  m_pOemPrivateDevMode->scpi.pub.dmMediaType;
	switch (wPaperType)
	{
	case DMMEDIA_AUTO:
		SelectedIndex = PAPERTYPE_AutoSelect;
		break;
	case DMMEDIA_PLAIN:
		SelectedIndex = PAPERTYPE_Plain;
		break;
	case DMMEDIA_LETTERHEAD:
		SelectedIndex = PAPERTYPE_LetterHead;
		break;
	case DMMEDIA_PREPRINT:
		SelectedIndex = PAPERTYPE_PrePrinted;;
		break;
	case DMMEDIA_PREPUNCH:
		SelectedIndex = PAPERTYPE_PrePunched;
		break;
	case DMMEDIA_RECYCLED:
		SelectedIndex = PAPERTYPE_Recycled;
		break;
	case DMMEDIA_COLOR:
		SelectedIndex = PAPERTYPE_Color;;
		break;
	case DMMEDIA_LABEL:
		SelectedIndex = PAPERTYPE_Labels;
		break;
	case DMMEDIA_HEAVY:
		SelectedIndex = PAPERTYPE_HeavyPaper;
		break;
	case DMMEDIA_T_OHP:
		SelectedIndex = PAPERTYPE_Transparency;
		break;
	case DMMEDIA_ENVELOPE:
		SelectedIndex = PAPERTYPE_Envelope;
		break;
	case DMMEDIA_J_POSTCARD:
		SelectedIndex = PAPERTYPE_JapanesePostCard;
		break;
	case DMMEDIA_IGNORE:
		SelectedIndex = PAPERTYPE_UsePaperTraySettings;
		break;
	default:
		SelectedIndex = eFirstOption;
		break; //Some problem
	}
	FeatureArray[FEATURE_MEDIATYPE] = SelectedIndex;

	//- FEATURE_BINDINGEDGE	10	
	SelectedIndex = m_pOemPrivateDevMode->scpi.ext.byBindedge;
	FeatureArray[FEATURE_BINDINGEDGE] = SelectedIndex;
	
	//- FEATURE_DUPLEX	11
	SelectedIndex = GetDuplexIndex(m_pOemPrivateDevMode->scpi.pub.dmDuplex);
	FeatureArray[FEATURE_DUPLEX] = SelectedIndex;

	//- FEATURE_PAMPHLET	12
	SelectedIndex = 0;
	FeatureArray[FEATURE_PAMPHLET] = SelectedIndex;
	
	//- FEATURE_ZOOMSETTINGS	13
	SelectedIndex = (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_XYZOOM) ? 1 : 0;
	FeatureArray[FEATURE_ZOOMSETTINGS] = SelectedIndex;

	//- FEATURE_FITTOPAPERSIZE	14
	if (m_pOemPrivateDevMode->scpi.pub.dmPaperSize == m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize)
	{
		SelectedIndex = 0;
	}
	else
	{
		DWORD dwOptions = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].dwOptions;
		dwOptions = dwOptions - 1;
		for (UINT i = 0; i < dwOptions; i++)
		{
			UINT temp = PaperSizeArray[i][0];
			if (temp == (UINT)m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize)
			{
				SelectedIndex = i + 1;
				break;
			}
		}
	}
	FeatureArray[FEATURE_FITTOPAPERSIZE] = SelectedIndex;

	//- FEATURE_BWPRINT	15
	SelectedIndex = 0;
	FeatureArray[FEATURE_BWPRINT] = SelectedIndex;

	//- FEATURE_ROTATE180	16
	SelectedIndex = 0;
	FeatureArray[FEATURE_ROTATE180] = SelectedIndex;

	//- FEATURE_RETENTION	17
	SelectedIndex = 0;
	FeatureArray[FEATURE_RETENTION] = SelectedIndex;

	//- FEATURE_RETENTION_SETTINGS	18	
	switch (m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetention)
	{
	case retention_normal:
		SelectedIndex = eFirstOption;
		break;
	case retention_hold_before:
		SelectedIndex = eSecondOption;
		break;
	case retention_hold_after:
		SelectedIndex = eThirdOption;
		break;
	case retention_sample:
		SelectedIndex = eFourthOption;
		break;
	default:
		SelectedIndex = eFirstOption;
		break;
	}
	FeatureArray[FEATURE_RETENTION_SETTINGS] = SelectedIndex;

	//- FEATURE_RETENTION_PASSWORD	19
	SelectedIndex = (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_USE_PIN) ? bool_true : bool_false;
	FeatureArray[FEATURE_RETENTION_PASSWORD] = SelectedIndex;

	//- FEATURE_DOCUMENT_FILING_SETTINGS 20
	switch (m_pOemPrivateDevMode->scpi.ext.JobCtrl.byDocumentFiling)
	{
	case docfile_quick:
		SelectedIndex = eFirstOption;
		break;
	case docfile_main:
		SelectedIndex = eSecondOption;
		break;
	case docfile_custom:
		SelectedIndex = eThirdOption;
		break;
	default:
		SelectedIndex = eSecondOption;
		break;
	}
	FeatureArray[FEATURE_DOCUMENT_FILING_SETTINGS] = SelectedIndex;

	//- FEATURE_USER_AUTHENTICATION_LOGIN_NAME		21
	SelectedIndex = 0;
	FeatureArray[FEATURE_USER_AUTHENTICATION_LOGIN_NAME] = SelectedIndex;

	//- FEATURE_USER_AUTHENTICATION_PASSWORD	22
	SelectedIndex = 0;
	FeatureArray[FEATURE_USER_AUTHENTICATION_PASSWORD] = SelectedIndex;

	//- FEATURE_USER_NUMBER	23	
	SelectedIndex = 0;
	FeatureArray[FEATURE_USER_NUMBER] = SelectedIndex;

	//- FEATURE_DEFAULT_JOB_USER_NAME		24
	SelectedIndex = (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_USERID) ? bool_true : bool_false;
	FeatureArray[FEATURE_DEFAULT_JOB_USER_NAME] = SelectedIndex;

	//- FEATURE_DEFAULT_JOB_JOB_NAME	25
	SelectedIndex = (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_JOBID) ? bool_true : bool_false;
	FeatureArray[FEATURE_DEFAULT_JOB_JOB_NAME] = SelectedIndex;

	//- FEATURE_AUTO_JOB_CONTROL_REVIEW	26
	SelectedIndex = (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & 	DM_AUTOJOB) ? bool_true : bool_false;
	FeatureArray[FEATURE_AUTO_JOB_CONTROL_REVIEW] = SelectedIndex;

	//- FEATURE_COLOR_MODE		27
	switch (m_pOemPrivateDevMode->scpi.ext.ColData.byColMode)
	{
	case colmode_auto:
		SelectedIndex = eFirstOption;
		break;
	case colmode_color:
		SelectedIndex = eSecondOption;
		break;
	case colmode_mono:
		SelectedIndex = eThirdOption;
		break;
	default:
		SelectedIndex = eFirstOption;
		break;
	}
	
	FeatureArray[FEATURE_COLOR_MODE] = SelectedIndex;

	//- FEATURE_DEFAULTUNITS	28
	SelectedIndex = eSecondOption;
	FeatureArray[FEATURE_DEFAULTUNITS] = SelectedIndex;

	//- FEATURE_CUSTOMPAPERSIZE		29
	SelectedIndex = eFirstOption;
	FeatureArray[FEATURE_CUSTOMPAPERSIZE] = SelectedIndex;

	//- FEATURE_OFFSET		30
	SelectedIndex = (m_pOemPrivateDevMode->scpi.ext.byOffset == offset_offset_each_set)? bool_true: bool_false;
	FeatureArray[FEATURE_OFFSET] = SelectedIndex;

	//- FEATURE_BOOKLET		31
	SelectedIndex = (m_pOemPrivateDevMode->scpi.ext.byPamph == booklet_none) ? booklet_none : booklet_standard;
	FeatureArray[FEATURE_BOOKLET] = SelectedIndex;

	//- FEATURE_STAMP		32
	SHORT wWmIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
	SelectedIndex = (wWmIndex != 0) ? stamp_watermark : stamp_none;
	FeatureArray[FEATURE_STAMP] = SelectedIndex;

	//- FEATURE_HUNDRED_PERCENT_NUP		33
	SelectedIndex = (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_SAMENUP) ? bool_true : bool_false;
	FeatureArray[FEATURE_HUNDRED_PERCENT_NUP] = SelectedIndex;

	//- FEATURE_AUTHENTICATION		34
	// --- DM_ALWAYSUSE_ACNUM
	SHORT wUseAcNum = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM ? bool_true : bool_false;
	// --- DM_USE_LOGINNAME
	short wLoginName = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME ? bool_true : bool_false;
	// --- DM_USE_LOGINPASS
	SHORT wLoginPass = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS ? bool_true : bool_false;
	SelectedIndex = authentication_none;
	if ((wLoginName == bool_false) && (wLoginPass == bool_false) && (wUseAcNum == bool_false)) {
		SelectedIndex = authentication_none;
	}
	else if ((wLoginName == bool_true) && (wLoginPass == bool_false) && (wUseAcNum == bool_false)) {
		SelectedIndex = authentication_lname;
	}
	else if ((wLoginName == bool_true) && (wLoginPass == bool_true) && (wUseAcNum == bool_false)) {
		SelectedIndex = authentication_lname_pswd;
	}
	else if ((wLoginName == bool_false) && (wLoginPass == bool_false) && (wUseAcNum == bool_true)) {
		SelectedIndex = authentication_acnum;
	}
	FeatureArray[FEATURE_AUTHENTICATION] = SelectedIndex;

	//- FEATURE_PRINTRELEASE		35
	SelectedIndex = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_PRINTSHARE ? bool_true : bool_false;
	FeatureArray[FEATURE_PRINTRELEASE] = SelectedIndex;

	//- FEATURE_ZOOM_BASING		36
	SelectedIndex = (m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & DM_ZOOM_CENTER)? zoom_basing_center: zoom_basing_upperleft;
	FeatureArray[FEATURE_ZOOM_BASING] = SelectedIndex;

	//- FEATURE_INPUTRESOLUTION		37
	/*SelectedIndex = m_pOemPrivateDevMode->scpi.ext.byResolution;
	FeatureArray[FEATURE_INPUTRESOLUTION] = SelectedIndex;*/

	//- FEATURE_SHADEPATTERN		38
	short wWysiWyg = m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & DM_WYSIWYG ? bool_true : bool_false;
	SelectedIndex = (wWysiWyg == bool_true) ? cmpt_shade_standard : cmpt_shade_fine;
	FeatureArray[FEATURE_SHADEPATTERN] = SelectedIndex;

	//- FEATURE_SPOOLDATAFORMAT		39
	SHORT wEMFSpool = m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & DM_EMFJOURNAL ? bool_true : bool_false;

	SelectedIndex = (wEMFSpool == bool_true)? cmpt_spool_emf : cmpt_spool_raw;
	FeatureArray[FEATURE_SPOOLDATAFORMAT] = SelectedIndex;

	//- FEATURE_PRINTABLEAREA		40
	SHORT wEdgeToEdge = m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_EDGETOEDGE ? bool_true : bool_false;

	SelectedIndex = (wEdgeToEdge == bool_true) ? cmpt_prnarea_maximum : cmpt_prnarea_standard;
	FeatureArray[FEATURE_PRINTABLEAREA] = SelectedIndex;

	//- FEATURE_DUPLEXSTYLE	41	
	// --- byDuplexStyle
	SelectedIndex = m_pOemPrivateDevMode->scpi.ext.PCFax.byDuplexStyle;
	FeatureArray[FEATURE_DUPLEXSTYLE] = SelectedIndex;

	//<S><C> Apply Button was not enabling first time when changing any feature in config tab,
	// only when driver install through installer in basic mode with valid ip address,19052022,SSDI:Chanchal Singla
	//- FEATURE_DEVICECOLORMODE		42
//	FeatureArray[FEATURE_DEVICECOLORMODE] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEVICECOLORMODE].uChangedOptionIndex;
	FeatureArray[FEATURE_DEVICECOLORMODE] = m_PropertySheetState.wColorOption;
	
	//- FEATURE_DEVICESTAPLEMODULE		43
	//FeatureArray[FEATURE_DEVICESTAPLEMODULE] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEVICESTAPLEMODULE].uChangedOptionIndex;
	FeatureArray[FEATURE_DEVICESTAPLEMODULE] = m_PropertySheetState.wStapleOption;
	
	//- FEATURE_STAPLELESS		44
	//FeatureArray[FEATURE_STAPLELESS] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLELESS].uChangedOptionIndex;
	FeatureArray[FEATURE_STAPLELESS] = m_PropertySheetState.byStapleless;
	
	//- FEATURE_DEVICEPUNCHMODULE		45
	//FeatureArray[FEATURE_DEVICEPUNCHMODULE] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEVICEPUNCHMODULE].uChangedOptionIndex;
	FeatureArray[FEATURE_DEVICEPUNCHMODULE] = m_PropertySheetState.wPunchOption;
	
	//- FEATURE_MAXPAPERSIZE		46
	//FeatureArray[FEATURE_MAXPAPERSIZE] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MAXPAPERSIZE].uChangedOptionIndex;
	FeatureArray[FEATURE_MAXPAPERSIZE] = m_PropertySheetState.byMaxPaperSize;
	
	//- FEATURE_SINGLESIGNON		47
	//FeatureArray[FEATURE_SINGLESIGNON] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_SINGLESIGNON].uChangedOptionIndex;
	FeatureArray[FEATURE_SINGLESIGNON] = m_PropertySheetState.wSingleSignOn;
	
	//- FEATURE_AUTOCOLORMODE		48
	//FeatureArray[FEATURE_AUTOCOLORMODE] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTOCOLORMODE].uChangedOptionIndex;
	FeatureArray[FEATURE_AUTOCOLORMODE] =  !m_PropertySheetState.byAutoColorMode;
	
	//- FEATURE_PINCOMPATIBILITY		49
	//FeatureArray[FEATURE_PINCOMPATIBILITY] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PINCOMPATIBILITY].uChangedOptionIndex;
	FeatureArray[FEATURE_PINCOMPATIBILITY] = m_PropertySheetState.byPinCompatibility;
	
	//- FEATURE_DEVICEPRINTRELEASE		50
	//FeatureArray[FEATURE_DEVICEPRINTRELEASE] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEVICEPRINTRELEASE].uChangedOptionIndex;
	FeatureArray[FEATURE_DEVICEPRINTRELEASE] = m_PropertySheetState.byPrintRelease;
	//<E> Apply Button was not enabling first time when changing any feature in config tab,
	// only when driver install through installer in basic mode with valid ip address,19052022,SSDI:Chanchal Singla

	//- FEATURE_JOBNAMESHORTENING	51
	SelectedIndex = m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_SHORT_JOBNAME ? bool_true : bool_false;
	FeatureArray[FEATURE_JOBNAMESHORTENING] = SelectedIndex;

	//- FEATURE_USEDRIVERTORENDERJPEG		52
	SelectedIndex  = m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & DM_JPEG_PASSTHROUGH ? bool_true : bool_false;
	FeatureArray[FEATURE_USEDRIVERTORENDERJPEG] = SelectedIndex;

	//- FEATURE_MINIMUMLINEWIDTH		53
	SelectedIndex  = m_pOemPrivateDevMode->scpi.doc.wPenWidthRate == 0 ? bool_true : bool_false;
	FeatureArray[FEATURE_MINIMUMLINEWIDTH] = SelectedIndex;

	//- FEATURE_CUSTOMPAPERUNITS		54
	SelectedIndex = m_pOemPrivateDevMode->scpi.ext.byCusUnits;
	FeatureArray[FEATURE_CUSTOMPAPERUNITS] = SelectedIndex;

	//- FEATURE_PRINTPOSITION		55
	SelectedIndex = 0;
	FeatureArray[FEATURE_PRINTPOSITION] = SelectedIndex;

	//- FEATURE_PRINT_POSITION_UNIT		56
	SelectedIndex = m_pOemPrivateDevMode->scpi.ext.byCusUnits;
	FeatureArray[FEATURE_PRINT_POSITION_UNIT] = SelectedIndex;

	//- FEATURE_MARGINSHIFT		57
	SelectedIndex = m_pOemPrivateDevMode->scpi.ext.byMgnShift;
	FeatureArray[FEATURE_MARGINSHIFT] = SelectedIndex;

	//- FEATURE_MARGIN_SHIFT_UNIT		58
	SelectedIndex = m_pOemPrivateDevMode->scpi.ext.byCusUnits;
	FeatureArray[FEATURE_MARGIN_SHIFT_UNIT] = SelectedIndex;

	//- FEATURE_BOOKLET_OUTPUTSIZE		59
	SHORT wBooklet  = (m_pOemPrivateDevMode->scpi.ext.byPamph == booklet_none) ? booklet_none : booklet_standard;
	if (wBooklet == booklet_standard)
	{
		switch (m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize)
		{
		case paper_a3:
			SelectedIndex = eFirstOption;
			break;
		case paper_a4:
			SelectedIndex = eSecondOption;
			break;
		case paper_b4:
			SelectedIndex = eThirdOption;
			break;
		case paper_ledger:
			SelectedIndex = eFourthOption;
			break;
		case paper_letter:
			SelectedIndex = eFifthOption;
			break;
		case paper_legal:
			SelectedIndex = eSixthOption;
			break;
		default:
			SelectedIndex = (DMPAPER_A4 == GetPaperSizeDef()) ? eFirstOption : eFourthOption;
			break;
		}
	}
	else
	{
		SelectedIndex = (DMPAPER_A4 == GetPaperSizeDef()) ? eFirstOption : eFourthOption;
	}
	FeatureArray[FEATURE_BOOKLET_OUTPUTSIZE] = SelectedIndex;

	//- FEATURE_WM_FONT_COLOR		60
	SelectedIndex = 1;
	FeatureArray[FEATURE_WM_FONT_COLOR] = SelectedIndex;

	//- FEATURE_WM_BORDER		61	
	SelectedIndex = 0;
	FeatureArray[FEATURE_WM_BORDER] = SelectedIndex;

	//- FEATURE_WM_PRINTING_PATTERN		62
	SelectedIndex = 0;
	FeatureArray[FEATURE_WM_PRINTING_PATTERN] = SelectedIndex;

	//- FEATURE_BOLD		63
	SelectedIndex = 0;
	FeatureArray[FEATURE_BOLD] = SelectedIndex;

	//- FEATURE_ITALIC		64
	SelectedIndex = 0;
	FeatureArray[FEATURE_ITALIC] = SelectedIndex;

	//- FEATURE_PPLCYPRINTRELEASE		65
	FeatureArray[FEATURE_PPLCYPRINTRELEASE] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PPLCYPRINTRELEASE].uChangedOptionIndex;

	//- FEATURE_PPLCYRETENTION		66
	FeatureArray[FEATURE_PPLCYRETENTION] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PPLCYRETENTION].uChangedOptionIndex;

	//- FEATURE_PPLCYUSERAUTHENTICATION		67
	FeatureArray[FEATURE_PPLCYUSERAUTHENTICATION] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PPLCYUSERAUTHENTICATION].uChangedOptionIndex;

	//- FEATURE_PPLCYWINLOGIN		68
	FeatureArray[FEATURE_PPLCYWINLOGIN] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PPLCYWINLOGIN].uChangedOptionIndex;

	//- FEATURE_PPLCYLOGINNAME		69
	FeatureArray[FEATURE_PPLCYLOGINNAME] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PPLCYLOGINNAME].uChangedOptionIndex;

	//- FEATURE_ALWAYS_USE_UNAME		70
	SelectedIndex = 0;
	FeatureArray[FEATURE_ALWAYS_USE_UNAME] = SelectedIndex;

	//- FEATURE_ALWAYS_USE_JNAME		71
	SelectedIndex = 0;
	FeatureArray[FEATURE_ALWAYS_USE_JNAME] = SelectedIndex;
	//<S><A>To Fix Redmine #7484,31-05-2024,SSDI:Manoj S
	//Added two features to support #3132.
	//- FEATURE_OVERIDE_PPLCYPRINTRELEASE		72
	FeatureArray[FEATURE_OVERIDE_PPLCYPRINTRELEASE] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OVERIDE_PPLCYPRINTRELEASE].uChangedOptionIndex;

	//- FEATURE_OVERIDE_PPLCYPRINTRELEASE		73
	FeatureArray[FEATURE_OVERIDE_PPLCYRETENTION] = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OVERIDE_PPLCYRETENTION].uChangedOptionIndex;
	//<E>To Fix Redmine #7484,31-05-2024,SSDI:Manoj S

	//- FEATURE_SHAREFAVORITES					74
	FeatureArray[FEATURE_SHAREFAVORITES] = m_PropertySheetState.bShare;

	for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
	{
		m_pFeatureInfoList->pFeatureOptionPair[nFeatureIndex].pszOption = m_pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].ppszOptions[FeatureArray[nFeatureIndex]];
		m_pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].uChangedOptionIndex = FeatureArray[nFeatureIndex];
		m_pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].uDefaultOptionIndex = FeatureArray[nFeatureIndex];
	}
	return TRUE;
}

BOOL CDevicePropertyPage::DisableAllCtrls(short *pwExcept)
{
	::EnumChildWindows(m_hPropertySheetHandle, (WNDENUMPROC)CShTabDlg_DisableProc, (LPARAM)pwExcept);

	return TRUE;
}

BOOL CDevicePropertyPage::DisablePrinterSelection()
{
	CRegistryAPI regApi(m_hInstance);
	BOOL bDisablePrnSel = FALSE;
	BOOL bClient = IsSharedServerSide(m_pPrinterName);
	BOOL bPortType = regApi.IsPortTypeClientOrTS(m_pPrinterName);

	if (bClient)
	{
		bDisablePrnSel = TRUE;
		// In cluster, if Discovery = 1 in MCF file, then enable printer selection button.Otherwise disable.
		//<S><C> Redmine #679 (Slow printing with UD2), 2013.09.04, SSDI:Purna
		//In client machine, checking for IsServerCluster() (NetServerGetInfo API) is consuming more time,
		//hence, first check for 'Discovery' flag in MCF file and then check for IsServerCluster().
		//if (IsServerCluster(m_pszSvrPrnName) && m_pmcf->IsDiscovery())
		//<S><A> 2015.18.10,SSDI:Debadatta
		//UD3 Code review comment
		if (m_pmcf->IsDiscovery() && regApi.IsServerCluster(m_pPrinterName))
			//<E> 2015.18.10,SSDI:Debadatta
			//<E> Redmine #679 (Slow printing with UD2), 2013.09.04, SSDI:Purna
		{
			bDisablePrnSel = FALSE;
		}
	}
	if (bPortType)
	{
		bDisablePrnSel = TRUE;
	}
	return bDisablePrnSel;
}
//<S><A> To Fix Redmine ,feedback 51,Bug #4017,2021.07.14,SSDI: Chanchal Singla
//Bug #4017 :-Watermark Drop down values are not localized when we changed the language.
//Feedback issue 51(Watermark option "None" is not changed based on the language selection
BOOL CDevicePropertyPage::LoadStringResource(WCHAR* pPrinterName, WCHAR *pszPath)
{
	//<S><C>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
	//In Basic mode Printing Preferences is not displayed as per the Language set
	// get language ID 
		// get language id from registry
	//TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	//GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	//CShIniFile cshLangFile(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
	//DWORD dwDefLangID = cshLangFile.GetDefaultLangID();
	//CRegistryAPI regApi;
	//DWORD dwLangID = regApi.GetLangID(pPrinterName, dwDefLangID);
	
	SHORT dwLangID = -1;
	CShRegMdlTbl RegMdlTbl(ghInstance, pPrinterName, m_pmcf);
	RegMdlTbl.GetLangID(&dwLangID);
	//<E>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla

	WCHAR pUserDLL[20] = { 0 };

	wcscpy_s(pUserDLL, L"USR");

	if (dwLangID > 0)
	{
		WCHAR LocaleID[10] = { 0 };
		_itow_s(dwLangID, LocaleID, 16);
		wcscat_s(pUserDLL, LocaleID);
	}
	wcscat_s(pUserDLL, L".dll");

	TCHAR szPath[_MAX_PATH] = { 0 };
	GetProjectFileName(szPath, pUserDLL);
	::lstrcpy(pszPath, szPath);
	return TRUE;
}
//<E> To Fix Redmine ,feedback 51,Bug #4017,2021.07.14,SSDI: Chanchal Singla

//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
//=============================================================================
// function
//      ApplyShareFav
//
// parameters
//      ñ≥Çµ
//
// return value
//      ê¨å˜Å@: TRUE
//      é∏îsÅ@: FALSE
//
// outline
//      apply the share/unshare function about fav items
//=============================================================================
BOOL CDevicePropertyPage::ApplyShareFav()
{
	BOOL bRet = FALSE;

	PPROPSTATE		pps;

	CShRegUS		*preg = new CShRegUS(ghInstance, m_pPrinterName, m_hStringResourceHandle, NULL, m_pFeatureInfoList, /*m_pOemPrivateDevMode*/NULL);
	CWaterMarkRegistryData		*pregwm = new CWaterMarkRegistryData(ghInstance, m_pPrinterName, m_hStringResourceHandle);


	if (preg == NULL)
		goto EXIT;

	if (pregwm == NULL)
		goto EXIT;

	WCHAR	szTime[SCUI_FILEDATETIME_STRLEN * 2] = { 0 };


	if (m_PropertySheetState.bShare == TRUE)
	{
		preg->WriteFavItemsFromHKCUToHKLM(m_pPrinterName);

		pregwm->WriteFavItemsFromHKCUToHKLM(m_pPrinterName);
	}
	else
	{
		//write the share_daytime_func to invalid in HKCU and HKLM.
		GetLocalTimeStr(szTime);
		preg->WriteShareDayTimeFunc(m_pPrinterName, szTime, FALSE);
		pregwm->WriteShareDayTimeFunc(m_pPrinterName, szTime, FALSE);
	}

	bRet = TRUE;
EXIT:
	return bRet;
}
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S

BOOL CDevicePropertyPage::ApplyShareFavForJson()
{
	BOOL bRet = FALSE;

	CShJsonUS		*pjsonus = NULL;
	CShJsonWm		*pjsonwm = NULL;


	pjsonus = new CShJsonUS(ghInstance, m_pPrinterName);
	pjsonus->Init();

	pjsonwm = new CShJsonWm(ghInstance, m_pPrinterName, m_hStringResourceHandle);
	pjsonwm->Init();

	if (pjsonus == NULL)
		goto EXIT;

	if (pjsonwm == NULL)
		goto EXIT;

	WCHAR	szTime[SCUI_FILEDATETIME_STRLEN * 2] = { 0 };


	if (m_PropertySheetState.bShare == TRUE)

	{
		pjsonus->WriteFavItemsFromJSONToHKLM(m_pPrinterName);
		pjsonwm->WriteFavItemsFromJSONToHKLM(m_pPrinterName);

	}
	else
	{
		//write the share_daytime_func to invalid in JSON and HKLM.
		GetLocalTimeStr(szTime);
		pjsonus->WriteShareDayTimeFunc(m_pPrinterName, szTime, FALSE);
		pjsonwm->WriteShareDayTimeFunc(m_pPrinterName, szTime, FALSE);
	}

	pjsonus->WriteJsonDataToFile();
	pjsonwm->WriteJsonDataToFile();

	bRet = TRUE;
EXIT:

	//if (cshinifile != NULL)
	//{
	//	delete cshinifile;
	//	cshinifile = NULL;
	//}
	if (pjsonus != NULL)
	{
		delete pjsonus;
		pjsonus = NULL;
	}
	if (pjsonwm != NULL)
	{
		delete pjsonwm;
		pjsonwm = NULL;
	}
	return bRet;

}