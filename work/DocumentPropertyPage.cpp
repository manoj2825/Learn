//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : DocumentPropertyPage.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 09 October 2013
//
//	Description   : This class Property sheet dialog functions
//===========================================================================
#include "precomp.h"
#include "shregus.h"
#include "shJsonus.h"
#include "shjsonups.h"
#include "shjsonms.h"
#include "shjsonpp.h"
#include "..\..\BitmapResource\Include\resource.h"
#include <HtmlHelp.h>
//For Logging Purpose
#include "MFPLogger.h"
#define CUSTOM_PAPER_ID 1256
#define STAPLE_NONE_ID 20040
#define STAPLE_INFORMATION_MESSAGE_ID 30056
//=============================================================================
// function
//      CDocumentPropertyPage
//
// return value
//     NILL
//
// outline
//      Constructor to the class CDocumentPropertyPage.
//=============================================================================
CDocumentPropertyPage::CDocumentPropertyPage(
	HMODULE hStringResourceHandle,
	LPTSTR pHelpFilePath,
	DWORD *pHelpID,
    HANDLE hPrinter,
    WCHAR* pPrinterName,
    HMODULE hBitmapResourceHandle,
    HMODULE hDialogResourceHandle
	) :
	m_pPrintCoreHelperUni(NULL),
	m_hPage(NULL),
	m_hComPropSheet(NULL),
	m_pfnComPropSheet(NULL),
	m_hStringResourceHandle(hStringResourceHandle),
	m_pOemPrivateDevMode(NULL),
	m_pHelpFilePath(pHelpFilePath),
	m_pHelpID(pHelpID),
	m_hPropertySheetHandle(NULL),
	m_pInformationIconButton(NULL),
	m_uFeatureControlID(0),
    m_hBitmapResourceHandle(hBitmapResourceHandle),
	m_ObjCShowMessage(NULL),
	m_pFeatureInfoList(NULL),
    m_hDialogResourceHandle(hDialogResourceHandle)
{
    m_eDeviceType = eColor;
	SecureZeroMemory(&m_InformationIconState,sizeof(m_InformationIconState));
	m_hcsHandUp	= ::LoadCursor(m_hBitmapResourceHandle, MAKEINTRESOURCE(IDC_HAND_UP));
	m_hcsHandDown	= ::LoadCursor(m_hBitmapResourceHandle, MAKEINTRESOURCE(IDC_HAND_DOWN));
    m_hPrinterHandle = hPrinter;
    StringCbCopy(m_pPrinterName,MAX_PATH*sizeof(WCHAR),pPrinterName);
	m_blIgnoreUntitled = FALSE;
	//<S><C>to fix Bug #4007,2021.29.07,ssdi:Chanchal Singla
	//	Auto job control review / User authentication not restricted in point and print scenario
	//To The support of Client/server scenario
	m_blEMFMode = IsEMFMode(m_pPrinterName);
	m_blLocalPrn = IsLocalPrinter(m_pPrinterName);
	m_blNetworkPrn = IsNetworkPrinter(m_pPrinterName);
	//<E>to fix Bug #4007,2021.29.07,ssdi:Chanchal Singla
	//Coverity Fix 90403 - SSDI:Seetharam - 20200828
	m_pWaterMarkIndex = NULL;
	m_ppi = NULL;
	//Coverity Fix 90403 - SSDI:Seetharam - 20200908
	SecureZeroMemory(&m_WaterMarkData, sizeof(WATERMARKDATA));
	SecureZeroMemory(&m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
	SecureZeroMemory(&m_PropertySheetState, sizeof(PROPSTATE));
	SecureZeroMemory(&m_ControlState, sizeof(CTRLSTATE));	
	pFeatureInfoList = NULL; //<S><A>Fix for Coverity issue 92766. Harika,20230427
}

//=============================================================================
// function
//      CDocumentPropertyPage
//
// return value
//     NILL
//
// outline
//      Destructor to the class CDocumentPropertyPage.
//=============================================================================
CDocumentPropertyPage::~CDocumentPropertyPage(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
    if(m_ObjCShowMessage)
	{
		delete m_ObjCShowMessage;
		m_ObjCShowMessage = NULL;
	}
    hr = DestroyUIComponents();
	if(S_OK != hr)
	{
		slog.LoggingErrorHResultValues();
	}
}

// <S><A> Added a function to get the resource ID of the current tab, 20200327 - SSDI:Seetharam
long CDocumentPropertyPage::GetResourceID()
{
	return m_wID;
}
// <S><E> Added a function to get the resource ID of the current tab, 20200327 - SSDI:Seetharam

VOID CDocumentPropertyPage::SetDeviceType(eDeviceType eValue)
{
    m_eDeviceType = eValue;
}

HRESULT
CDocumentPropertyPage::SetDevMode(
     _In_ CONST POEMDEV pOemDevMode
    )
{
    HRESULT hr = S_OK;
	m_pOemPrivateDevMode = pOemDevMode;
     return hr;
}

// <S><A> Added this function for doing the initializations for the dialog, 20200327 - SSDI:Seetharam
HRESULT
CDocumentPropertyPage::InitDlgBox(
	_Out_ LPCTSTR* ppszTemplate,
	_Out_ LPCTSTR* ppszTitle
)
{
	return NULL;
}
// <S><E> Added this function for doing the initializations for the dialog, 20200327 - SSDI:Seetharam

HRESULT
CDocumentPropertyPage::PropPageInit(
    _In_ CONST PPROPSHEETUI_INFO pPSUIInfo
    )
{
    HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(NULL == pPSUIInfo)
	{
		slog.putLog("CDocumentPropertyPage::PropPageInit -> E_FAIL(pPSUIInfo)\n");
		return E_FAIL;
	}
    
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
            page.dwFlags = PSP_DEFAULT | PSP_USETITLE|PSP_HASHELP;
			page.hInstance = m_hDialogResourceHandle;

            page.pfnDlgProc = CDocumentPropertyPage::DlgProc;
			page.hActCtx = GetMyActivationContext(m_hDialogResourceHandle);
			//page.hActCtx = ghActCtx ;
			 if( (NULL != page.hActCtx) && (INVALID_HANDLE_VALUE != page.hActCtx))
            {
                page.dwFlags |= PSP_USEFUSIONCONTEXT;
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
				if(S_OK != hr)
				{
					slog.LoggingErrorHResultValues();
				}
            }
        }
   // }

    //ERR_ON_HR(hr);
    return hr;
}
/*++

Routine Name:

    CDocumentPropertyPage::SetComPropSheetFunc

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
CDocumentPropertyPage::SetComPropSheetFunc(
    _In_ CONST PFNCOMPROPSHEET pfnComPropSheet
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    HRESULT hr = S_OK;

    ASSERTMSG(pfnComPropSheet != NULL, "NULL pointer to common propert sheet functions.\n");

    if (NULL != pfnComPropSheet)
    {
        m_pfnComPropSheet = pfnComPropSheet;
    }
	else
	{
		slog.putLog("CDocumentPropertyPage::SetComPropSheetFunc -> NULL_POINTER(pfnComPropSheet) \n");
	}

    
    return hr;
}

/*++

Routine Name:

    CDocumentPropertyPage::SetPageHandle

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
CDocumentPropertyPage::SetPageHandle(
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
		slog.putLog("CDocumentPropertyPage::SetPageHandle -> NULL_HANDLE(hPage)\n");
	}

   return hr;
}

/*++

Routine Name:

    CDocumentPropertyPage::SetComPropSheetHandle

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
CDocumentPropertyPage::SetComPropSheetHandle(
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
		slog.putLog("CDocumentPropertyPage::SetComPropSheetHandle -> NULL_HANDLE(hComPropSheet)\n");
	}
     return hr;
}

/*++

Routine Name:

    CDocumentPropertyPage::GetComPropSheetFunc

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
CDocumentPropertyPage::GetComPropSheetFunc(
    _Outptr_ PFNCOMPROPSHEET* ppfnComPropSheet
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    HRESULT hr = S_OK;

    if (NULL !=ppfnComPropSheet)
    {
        *ppfnComPropSheet = m_pfnComPropSheet;
    }
	else
	{
		slog.putLog("CDocumentPropertyPage::GetComPropSheetFunc -> NULL_HANDLE(ppfnComPropSheet)\n");
	}

    return hr;
}

/*++

Routine Name:

    CDocumentPropertyPage::GetPageHandle

Routine Description:

    Retrieves the property page handle.

Arguments:

    None

Return Value:

    Handle to the property page

--*/
HANDLE
CDocumentPropertyPage::GetPageHandle(
    VOID
    )
{
    return m_hPage;
}

/*++

Routine Name:

    CDocumentPropertyPage::GetComPropSheetHandle

Routine Description:

    Retrieves the common property sheet handle

Arguments:

    None

Return Value:

    Handle to the common property sheet

--*/
HANDLE
CDocumentPropertyPage::GetComPropSheetHandle(
    VOID
    )
{
    return m_hComPropSheet;
}

INT_PTR CALLBACK
CDocumentPropertyPage::DlgProc(
    _In_ CONST HWND   hDlg,
    _In_ CONST UINT   uiMessage,
    _In_ CONST WPARAM wParam,
    _In_ CONST LPARAM lParam
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    HRESULT hr = S_OK;
	//BOOL retVal = FALSE;
	switch (uiMessage)
    {
        case WM_INITDIALOG:
        {
            //
            // Store the class instance
            //
			hr = OnWmInitDialog(hDlg,lParam);
			
		}
		break;
		case WM_COMMAND:
        {
			hr = OnWmCommand(hDlg,wParam);
		}
		break;
        case WM_NOTIFY:
        {
			hr = OnWmNotify(hDlg,lParam);
		}
		break;
		//case WM_SETTEXT:
			//break;
		case WM_HSCROLL:
		case WM_VSCROLL:
		{
			hr = OnWmScroll(hDlg,wParam,lParam);
			
		}
		break;
		case WM_DRAWITEM:
		{
				hr = OnWmDrawItem(hDlg,wParam,lParam);
			
		}
		break;
		case WM_CTLCOLORSTATIC:
		{
				hr = OnWmColorStatic(hDlg,wParam,lParam);
			
		}
		break;
		case WM_NCDESTROY:
        {
            hr = OnWmDestroy(hDlg);
			
        }
        break;
		case WM_HELP:
		{
			CDocumentPropertyPage* pPropertyPageInstance =NULL;

			if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
				(NULL !=pPropertyPageInstance))
			{
				LPHELPINFO pHelpInformation = (LPHELPINFO)lParam;
				hr = pPropertyPageInstance->ShowHelp(pHelpInformation);
			}
		}
		break;
	}
	if(S_OK != hr)
	{
		slog.LoggingErrorHResultValues();
	}
	return hr ;
}
HRESULT CDocumentPropertyPage::OnWmInitDialog(_In_ HWND   hDlg,
        _In_ LPARAM lParam
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance = reinterpret_cast<CDocumentPropertyPage*>(lParam);
	if(NULL != pPropertyPageInstance)
	{
		{
			switch (pPropertyPageInstance->m_wID)
			{
			case IDD_TAB_MAIN_121_03:
				//Fixed Coverity issue - 90336 - SSDI:Seetharam-20200928
				//hResult = SUCCEEDED(hResult = pPropertyPageInstance->StorePropertyPageInstance(hDlg));
				hResult = pPropertyPageInstance->StorePropertyPageInstance(hDlg);
				hResult = ((CMainTabPropertyPage *)pPropertyPageInstance)->InitializeUIControls(hDlg);
				break;
			case IDD_TAB_PAPER_03:
				hResult = pPropertyPageInstance->StorePropertyPageInstance(hDlg);
				hResult = ((CPaperTabPropertyPage *)pPropertyPageInstance)->InitializeUIControls(hDlg);
				break;
			case IDD_TAB_FINISHING_01_03:
				hResult = pPropertyPageInstance->StorePropertyPageInstance(hDlg);
				hResult = ((CFinishingTabPropertyPage *)pPropertyPageInstance)->InitializeUIControls(hDlg);
				break;
			case IDD_TAB_LAYOUT_01_03:
				hResult = pPropertyPageInstance->StorePropertyPageInstance(hDlg);
				hResult = ((CLayoutTabPropertyPage *)pPropertyPageInstance)->InitializeUIControls(hDlg);
				break;
			case IDD_TAB_JOBHAND_0:
				hResult = pPropertyPageInstance->StorePropertyPageInstance(hDlg);
				hResult = ((CJobHandlingTabPropertyPage *)pPropertyPageInstance)->InitializeUIControls(hDlg);
				break;
			default:
				break;
			}
		}
	}
	if(S_OK != hResult)
	{
		slog.LoggingErrorHResultValues();
	}

	return hResult;
}

HRESULT CDocumentPropertyPage::OnWmCommand(_In_ HWND   hDlg,
	_In_ CONST WPARAM wParam
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	//{
	//	CDocumentPropertyPage* pPropertyPageInstance = NULL;
	//	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
	//	{
	//		if (NULL != pPropertyPageInstance)
	//		{
	//			switch (pPropertyPageInstance->m_wID)
	//			{
	//			case IDD_TAB_MAIN_121_03:
	//				hResult = ((CMainTabPropertyPage*)pPropertyPageInstance)->OnBnClicked(hDlg, FALSE, wParam);
	//				break;

	//			case IDD_TAB_PAPER_03:
	//				hResult = ((CPaperTabPropertyPage*)pPropertyPageInstance)->OnBnClicked(hDlg, FALSE, wParam);
	//				break;

	//			case IDD_TAB_FINISHING_01_03:
	//				hResult = ((CFinishingTabPropertyPage*)pPropertyPageInstance)->OnBnClicked(hDlg, FALSE, wParam);
	//				break;

	//			case IDD_TAB_LAYOUT_01_03:
	//				hResult = ((CLayoutTabPropertyPage*)pPropertyPageInstance)->OnBnClicked(hDlg, FALSE, wParam);
	//				break;

	//			case IDD_TAB_JOBHAND_0:
	//				hResult = ((CJobHandlingTabPropertyPage*)pPropertyPageInstance)->OnBnClicked(hDlg, FALSE, wParam);
	//				break;

	//			default:
	//				;
	//			}
	//			pPropertyPageInstance->UpdateMimic();// update Mimic
	//		}
	//	}
	//}
	//break;
	case EN_CHANGE:
	case CBN_SELCHANGE:
	{
		CDocumentPropertyPage* pPropertyPageInstance;
		if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
		{
			if (NULL != pPropertyPageInstance)
			{
				hResult = pPropertyPageInstance->SendCommand(hDlg, wParam);
				pPropertyPageInstance->UpdateMimic();
			}
		}
	}
	break;
	//<S><A>to Fix Bug #2738 issue 2,2021.Dec.10,SSDI:Chanchal Singla	
	//Printing Preferences : Tab is not highlighted with blue Color when Clicked
	//Issue 2: Copies text box number is not highlighted with blue Color when Clicked
	case EN_SETFOCUS:
		CDocumentPropertyPage* pPropertyPageInstance;
		if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
		{
			if (NULL != pPropertyPageInstance)
			{
				hResult = pPropertyPageInstance->OnEnSetFocus(hDlg, wParam);
			}
		}
		break;
		//<E>to Fix Bug #2738,2021.Dec.10,SSDI:Chanchal Singla	
	default:
		//
		// Unhandled command so return TRUE
		//
		// hResult = S_OK;
		break;
	}

	if (S_OK != hResult)
	{
		slog.LoggingErrorHResultValues();
	}
	return hResult;
}
HRESULT CDocumentPropertyPage::OnWmScroll(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
	{
		if(NULL != pPropertyPageInstance)
		{
				hResult = pPropertyPageInstance->OnScroll(hDlg, wParam,lParam);
		}
	}

	if(S_OK != hResult)
	{
		slog.LoggingErrorHResultValues();
	}
	return hResult;
}
HRESULT CDocumentPropertyPage::OnWmDrawItem(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
	{
		if(NULL != pPropertyPageInstance)
		{
				hResult = pPropertyPageInstance->OnDrawItem(hDlg, wParam,lParam);
		}
	}
	if(S_OK != hResult)
	{
		slog.LoggingErrorHResultValues();
	}

	return hResult;
}
//<S><A>to Fix Bug #2738 Issue 2,2021.Dec.10,SSDI:Chanchal Singla	
//Printing Preferences : Tab is not highlighted with blue Color when Clicked
//Issue 2: Copies text box number is not highlighted with blue Color when Clicked
HRESULT CDocumentPropertyPage::OnEnSetFocus(_In_ HWND   hDlg,
	_In_ CONST WPARAM wParam
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
	{
		if (NULL != pPropertyPageInstance)
		{
			switch (pPropertyPageInstance->m_wID)
			{
			case IDD_TAB_MAIN_121_03:
				hResult = ((CMainTabPropertyPage*)pPropertyPageInstance)->OnEnSetFocus(hDlg, wParam);
				break;
			default:
				;
			}
		}
		if (S_OK != hResult)
		{
			slog.LoggingErrorHResultValues();
		}

		return hResult;
	}
}
//<E>to Fix Bug #2738,2021.Dec.10,SSDI:Chanchal Singla	
HRESULT CDocumentPropertyPage::OnWmColorStatic(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
	{
		if(NULL != pPropertyPageInstance)
		{
			hResult = pPropertyPageInstance->OnContorlColorStatic(hDlg, wParam,lParam);
		}
	}
	if(S_OK != hResult)
	{
		slog.LoggingErrorHResultValues();
	}

	return hResult;
}
HRESULT CDocumentPropertyPage::OnWmMouseMove(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
	{
		if(NULL != pPropertyPageInstance)
		{
				hResult = pPropertyPageInstance->OnMouseMove(hDlg, wParam,lParam);
				if(S_OK != hResult)
				{
					slog.LoggingErrorHResultValues();
				}
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::OnWmMouseMove -> NULL_OBJECT(pPropertyPageInstance)\n");
		}
	}
	return hResult;
}
HRESULT CDocumentPropertyPage::OnWmLButtonDown(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
	{
		if(NULL != pPropertyPageInstance)
		{
				hResult = pPropertyPageInstance->OnLButtonDown(hDlg, wParam,lParam);
				if(S_OK != hResult)
				{
					slog.LoggingErrorHResultValues();
				}
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::OnWmLButtonDown -> NULL_POINTER(pPropertyPageInstance)\n");
		}
	}
	return hResult;
}
HRESULT CDocumentPropertyPage::OnWmLButtonUp(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
	{
		if(NULL != pPropertyPageInstance)
		{
				hResult = pPropertyPageInstance->OnLButtonUp(hDlg, wParam,lParam);
				if(S_OK != hResult)
				{
					slog.LoggingErrorHResultValues();
				}
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::OnWmLButtonUp -> NULL_POINTER(pPropertyPageInstance)\n");
		}
	}
	return hResult;
}
/*++
Routine Name:

    CDocumentPropertyPage::OnWmNotify

Routine Description:

    The routine resets the UI to factory defaults

Arguments:

    hDlg - handle to the dialog
    lParam - LPARAM message information

Return Value:

   S_OK,
   return value from OnPsnSetActive() or OnPsnApply() or OnNmClick()
   
--*/
HRESULT CDocumentPropertyPage::OnWmNotify(_In_ HWND   hDlg,
        _In_ LPARAM lParam
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	NMHDR* pHdr = reinterpret_cast<NMHDR*>(lParam);
			
    if ((NULL != pHdr) && (NULL != pHdr->code))
    {
        switch (pHdr->code)
		//switch (((LPNMHDR)lParam)->code)
        {
            case PSN_SETACTIVE:
            {
				hResult = OnPsnSetActive(hDlg);
				if(S_OK != hResult)
				{
					slog.LoggingErrorHResultValues();
				}
				//
                // Return FALSE to accept the page activation
                //
                //retVal = FALSE;
            }
            break;
			case PSN_KILLACTIVE:
			{
				hResult = OnPsnKillActive(hDlg);
				if(S_OK != hResult)
				{
					slog.LoggingErrorHResultValues();
				}
				//
                // Return FALSE to accept the page activation
                //
                //retVal = FALSE;
            }
            break;
			//<S><A> RedMine Ticket(#871) No message displayed when close(X) button is clicked  .: 2013.11.19, SSDI:Gajanana D S
			// Handle PSN_QUERYCANCEL notification.
			case PSN_QUERYCANCEL:
			//<E> RedMine Ticket(#871) No message displayed when close(X) button is clicked  .: 2013.11.19, SSDI:Gajanana D S
            {
				hResult = OnPsnQueryCancel(hDlg);
				if(S_OK != hResult)
				{
					slog.LoggingErrorHResultValues();
				}
				//
                // Return FALSE to accept the page activation
                //
                //retVal = FALSE;
            }
            break;
			case PSN_APPLY:
            {
				hResult = OnPsnApply(hDlg);
				if(S_OK != hResult)
				{
					slog.LoggingErrorHResultValues();
				}
			}
			break;
			case PSN_HELP:
			{
				CDocumentPropertyPage* pPropertyPageInstance;
                if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
                    (NULL != pPropertyPageInstance))
                {
					pPropertyPageInstance->DisplayHelpInformation(hDlg);
                }
				if(S_OK != hResult)
				{
					slog.LoggingErrorHResultValues();
				}
			
			}
			break;
                    
            case NM_CLICK:
                {
                    hResult = OnNmClick(hDlg);
					if(S_OK != hResult)
					{
						slog.LoggingErrorHResultValues();
					}
                }
			break;
			
			default:
			{
				//DefWindowProc(hDlg,uiMessage,wParam,lParam);
				slog.putLog("CDocumentPropertyPage::OnWmNotify -> E_FAIL\n");
				return E_FAIL;

			}
			break;
		}
	}
	return hResult;

}
HRESULT CDocumentPropertyPage::OnPsnSetActive(_In_ HWND   hDlg )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
        (NULL != pPropertyPageInstance))
    {
        hResult = pPropertyPageInstance->ActivateUIControl(hDlg);
		if(S_OK != hResult)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	return hResult;
}
HRESULT CDocumentPropertyPage::OnPsnKillActive(_In_ HWND   hDlg )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
        (NULL != pPropertyPageInstance))
    {
        hResult = pPropertyPageInstance->DeActivateUIControl(hDlg);
		if(S_OK != hResult)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	return hResult;
}
//=============================================================================
// function
//      OnPsnKilltActiveEX
//
// parameters
//      HWND hWnd
//
// return value
//      0
//
// outline
//      ù
//=============================================================================
LRESULT CDocumentPropertyPage::OnPsnKillActiveEX(HWND hWnd)
{
	return TRUE;
}

HRESULT CDocumentPropertyPage::OnPsnQueryCancel(_In_ HWND   hDlg )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
        (NULL != pPropertyPageInstance))
    {
        hResult = pPropertyPageInstance->OnQueryCancel(hDlg);
		if(S_OK != hResult)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	return hResult;
}
HRESULT CDocumentPropertyPage::OnPsnApply(_In_ HWND   hDlg )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	PFNCOMPROPSHEET pfnComPropSheet = NULL; 
	CDocumentPropertyPage* pPropertyPageInstance;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
    (NULL != pPropertyPageInstance) &&
	SUCCEEDED(hResult = pPropertyPageInstance->GetComPropSheetFunc(&pfnComPropSheet)) &&
	(NULL != pPropertyPageInstance)
	)
    {
		PropSheet_UnChanged(GetParent(hDlg), hDlg);
		pPropertyPageInstance->SaveFeatureOptions();
		pfnComPropSheet(pPropertyPageInstance->GetComPropSheetHandle(),
                        CPSFUNC_SET_RESULT,
                        reinterpret_cast<LPARAM>(pPropertyPageInstance->GetPageHandle()),
                        (LPARAM)CPSUI_OK);
	}
	if(S_OK != hResult)
	{
		slog.LoggingErrorHResultValues();
	}

	return hResult;
}
LRESULT CDocumentPropertyPage::OnPsnApplyEX(HWND hDlg, long lCtrlID, LPPSHNOTIFY ppsn)
{
	return TRUE;
}

HRESULT CDocumentPropertyPage::OnWmDestroy(_In_ HWND   hDlg )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance;
    if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
        (NULL !=pPropertyPageInstance))
    {
        hResult = pPropertyPageInstance->RemovePropertyPageInstance(hDlg);
		if(S_OK != hResult)
		{
			slog.LoggingErrorHResultValues();
		}
    }

    //
    // Set return to FALSE to indicate that the message was processed
    //
    hResult = E_FAIL;
	return hResult;
}
HRESULT CDocumentPropertyPage::StorePropertyPageInstance(  _In_ CONST HWND hDlg )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
   /* HRESULT hr = S_OK;
	SetWindowLongPtr (hDlg, DWLP_USER, (LONG_PTR)this);
	return hr;*/
	HRESULT hr = S_OK;

    if (!SetProp(hDlg, MAKEINTATOM(ID_XDSMPL_DLG_ATOM), reinterpret_cast<HANDLE>(this)))
    {
		slog.putLog("CDocumentPropertyPage::StorePropertyPageInstance -> E_FAIL\n");
        hr = E_FAIL;
    }
	return hr;
}
/*++

Routine Name:

    CDocumentPropertyPage::RetrieveThis

Routine Description:

    Obtains the instance of the CDocumentPropertyPage class that is associated with a windows handle.

Arguments:

    hDlg - Handle of the property page Window associated with this class.
    pDocPropPage - Address of a pointer to be filled out with the instance of this class.

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/

HRESULT
CDocumentPropertyPage::RetrievePropertyPageInstance(
    _In_        CONST HWND     hDlg,
    _Outptr_ CDocumentPropertyPage** pDocPropPage
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    HRESULT hr = S_OK;
	if((NULL !=hDlg)&&(NULL !=pDocPropPage))
	{
		//*pDocPropPage = reinterpret_cast<CDocumentPropertyPage*>(GetWindowLongPtr(hDlg, DWLP_USER));
		*pDocPropPage = reinterpret_cast<CDocumentPropertyPage*>(GetProp(hDlg, MAKEINTATOM(ID_XDSMPL_DLG_ATOM)));
	}
	else
	{
		slog.putLog("CDocumentPropertyPage::RetrievePropertyPageInstance -> NULL_HANDLE\n");
	}
    return hr;
}
HRESULT CDocumentPropertyPage::RemovePropertyPageInstance(
        _In_ CONST HWND hDlg
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	 HRESULT hr = S_OK;

    if (NULL != hDlg)
    {
        RemoveProp(hDlg, MAKEINTATOM(ID_XDSMPL_DLG_ATOM));
    }
	else
	{
		slog.putLog("CDocumentPropertyPage::RemovePropertyPageInstance -> NULL_HANDLE(hDlg)\n");
	}

   // ERR_ON_HR(hr);
    return hr;
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
VOID CDocumentPropertyPage::CreateMessageClass(_In_ CONST HWND   hDlg)
{
	m_ObjCShowMessage = new CShowMessage(hDlg,m_hStringResourceHandle);
}

HRESULT CDocumentPropertyPage::InitializeUIControls(
    _In_ CONST HWND   hDlg
    )
{
    //
    // Use the wParam as the index into the control map and
    // inform the control of an activation
    //
    HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	//m_pWaterMarkIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark ;
//<S><A> To fix redmine #2792 issue 2020.12.09 SSDI:Sneha TG
	//if (m_pOemPrivateDevMode->scpi.ext.byPamph == PAMPH_TILED) {
	//	m_pOemPrivateDevMode->scpi.ext.byPamph = PAMPH_2UP;
	//}
	//SelectOptionsonUI(hDlg, m_pOemPrivateDevMode);
//<E> To fix redmine #2792 issue 2020.12.09 SSDI:Sneha TG
// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	//DevDmodetopropstate();
// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	SetControlStrings(hDlg);
	CreateContorlClass();
 
    //<E>To The Support of Mimic Control,20201412,SSDi:Chanchal Singla
	// Visual studio 2012 supports range based for loop , to iterate
	// through Arrays, Containers like std::Vector, std::map etc.
	// Refer below MSDN link for Syntax & use of range based for loop
	// http://msdn.microsoft.com/en-us/library/jj203382.aspx
	for(auto iterUIComponents : m_UIControls)
	{
		CUIControl* pControl = iterUIComponents.second;
		 if (NULL != pControl)
        {
			if (FAILED(hr = pControl->OnInit(iterUIComponents.first,hDlg)))
            {
                break;
            }
		}
		 else
		 {
			 slog.putLog("CDocumentPropertyPage::InitializeUIControls -> ERROR_NULL_POINTER(pControl)\n");
		 }
	}
	    
	//if(NULL != pStr)
	//{
	//	delete[] pStr;
	//	pStr = NULL;
	//}
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
CDocumentPropertyPage::AddUIControl(
    _In_ CONST INT   iCtrlID,
    _In_ CUIControl* pUIControl
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    if(NULL == pUIControl)
	{
		slog.putLog("CDocumentPropertyPage::AddUIControl -> NULL_POINTER(pUIControl)\n");
		return E_FAIL;
	}
    m_UIControls[iCtrlID] = pUIControl;
    return S_OK;
}
/*++

Routine Name:

    CDocumentPropertyPage::SendSetActive

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
CDocumentPropertyPage::ActivateUIControl(
    _In_ CONST HWND   hDlg
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    //
    // Use the wParam as the index into the control map and
    // inform the control of an activation
    //
    HRESULT hr = S_OK;

	for(auto iterUIComponents : m_UIControls)
	{
		CUIControl* pControl = iterUIComponents.second;
        if (NULL != pControl)
        {
			if (FAILED(hr = pControl->ActivateUIControl(iterUIComponents.first,hDlg)))
            {
				if(S_OK != hr)
				{
					slog.LoggingErrorHResultValues();
				}
				break;
            }
			
        }
		else
		{
			slog.putLog("CDocumentPropertyPage::ActivateUIControl -> NULL_POINTER(pControl)\n");
		}
	}
	
    return hr;
}
/*++

Routine Name:

    CDocumentPropertyPage::SendSetActive

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
CDocumentPropertyPage::DeActivateUIControl(
    _In_ CONST HWND   hDlg
    )
{
    UNREFERENCED_PARAMETER(hDlg);
    //
    // Use the wParam as the index into the control map and
    // inform the control of an activation
    //
   return S_OK;
}
HRESULT
CDocumentPropertyPage::OnQueryCancel(
    _In_ CONST HWND   hDlg
    )
{
    UNREFERENCED_PARAMETER(hDlg);
    //
    // Use the wParam as the index into the control map and
    // inform the control of an activation
    //
   return S_OK;
}

HRESULT
CDocumentPropertyPage::OnScroll(
    _In_ CONST HWND   hDlg,
    _In_ CONST WPARAM wParam,
	_In_ CONST LPARAM lParam
    )
{
    //
    // Use the wParam as the index into the control map and
    // inform the control that generated the command
    //
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    HRESULT hr = S_OK;
   UIControlMap* pComponents = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	INT iControlID = GetDlgCtrlID((HWND)lParam);
	CUIControl* pControl = (*pComponents)[iControlID];
	if (NULL != pControl)	
    {
		hr = pControl->OnScroll(iControlID,LOWORD(wParam),hDlg, HIWORD(wParam));
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CDocumentPropertyPage::OnScroll -> NULL_POINTER(pControl)\n");
	}
	return hr;
}
HRESULT
CDocumentPropertyPage::OnDrawItem(
    _In_ CONST HWND   /*hDlg*/,
    _In_ CONST WPARAM wParam,
	_In_ CONST LPARAM /*lParam*/
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap::const_iterator Iterator = m_UIControls.find((short)wParam);
	if(Iterator == m_UIControls.end())
	{
		slog.putLog("CDocumentPropertyPage::OnDrawItem -> E_FAIL\n");
		return E_FAIL;
	}
	CUIControl *pControl = 	Iterator->second;
	if(NULL == pControl)
	{
		slog.putLog("CDocumentPropertyPage::OnDrawItem -> E_FAIL\n");
		return E_FAIL;
	}
	pControl->Draw();
	return S_OK;
}
LRESULT CDocumentPropertyPage::OnContorlColorStatic(
		 _In_ CONST HWND /*hWnd */,
		 _In_ CONST WPARAM /*wParam*/,
		 _In_ CONST LPARAM /*lParam*/
		) 
{
	return S_OK;
}
LRESULT CDocumentPropertyPage::OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(hWnd);
   ::SetCursor(m_hcsHandDown);
     
    return S_OK;
}

LRESULT CDocumentPropertyPage::OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(hWnd);
   ::SetCursor(m_hcsHandUp);

    return S_OK;
}
HRESULT
CDocumentPropertyPage::OnMouseMove(
    _In_ CONST HWND   hDlg,
    _In_ CONST WPARAM wParam,
	_In_ CONST LPARAM lParam
    )
{
	UNREFERENCED_PARAMETER(hDlg);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	if(wParam & MK_LBUTTON)
        {
            ::SetCursor(m_hcsHandDown);
            
        }
        else	
        {
			::SetCursor(m_hcsHandUp);
        }
  
    return S_OK;
	/*POINT		pt;

    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);
    
    if (PtInRect(&m_RectPaper, pt) != 0)
    {
        if(wParam & MK_LBUTTON)
        {
            ::SetCursor(m_pPropertyState->hcsHandDown);
            SetObjectPoint(pt);
        }
        else	
        {
            ::SetCursor(m_pPropertyState->hcsHandUp);
        }
    }*/
    
	//return S_OK;
}
HRESULT
CDocumentPropertyPage::SendCommand(
    _In_ CONST HWND   hDlg,
    _In_ CONST WPARAM wParam
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    //
    // Use the wParam as the index into the control map and
    // inform the control that generated the command
    //
    HRESULT hr = S_OK;
	UIControlMap* pComponents = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	CUIControl* pControl = (*pComponents)[LOWORD(wParam)];
	if (NULL != pControl)	
    {
		// When Pop Message box Display that time Contol Goes back. Other Code Not to be Executed.  
		if (TRUE == DisplayPopUpInformationMessage(hDlg, LOWORD(wParam)))
		{
			return S_OK;
		}

		if (LOWORD(wParam) == IDC_CMB_PAPER || LOWORD(wParam) == IDC_CMB_OUTPUTSIZE)
		{
			DisplayCustomDialog(hDlg, LOWORD(wParam));
		}
		hr = pControl->OnCommand(LOWORD(wParam),hDlg, HIWORD(wParam));
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		SetFeatureControlID(LOWORD(wParam));
		//NotifyConstrainedFeatures(hDlg,pControl,LOWORD(wParam));
    }
	else
	{
		slog.putLog("CDocumentPropertyPage::SendCommand -> NULL_POINTER(pControl)\n");
	}
	return hr;
}
	//<S><C>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
VOID CDocumentPropertyPage::UpdateMimic(VOID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_SHARED_BBOX);
	if (Iterator != m_UIControls.end())
	{
		CMimicImage *pMimic = static_cast<CMimicImage*>(Iterator->second);
		if (pMimic)
		{
			pMimic->Draw(&m_PropertySheetState);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::UpdateMimic -> NULL_OBJECT(CMimicImage)\n");
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
			slog.putLog("CDocumentPropertyPage::UpdateMimic -> NULL_OBJECT(CMimicText)\n");
		}
	}
}
	//<E>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla

//<S><A> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
VOID CDocumentPropertyPage::UpdateUserSetting(HWND hDlg,WORD wControlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_SHARED_USRSET);
	if (Iterator != m_UIControls.end())
	{
		CShCmbUserset *pUserSetting = static_cast<CShCmbUserset*>(Iterator->second);
		if (pUserSetting)
		{
			pUserSetting->OnInit(IDC_CMB_SHARED_USRSET, hDlg);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::UpdateUserSetting -> NULL_OBJECT(CShCmbUserset)\n");
		}
	}
}
VOID CDocumentPropertyPage::SetUserSettingState(
	_In_ CONST HWND   hDlg,
	_In_ CONST INT iCtrlID
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	LONG wUsIndex = (LONG)*m_pUsIndex;
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_SHARED_USRSET);
	if (Iterator != m_UIControls.end())
	{
		CShCmbUserset *pUserSetting = static_cast<CShCmbUserset*>(Iterator->second);
		if (pUserSetting)
		{
			// --- user settings
			if (pUserSetting->IsUntitled() == FALSE)
				((CShImageCombo*)pUserSetting)->Select(IDC_CMB_SHARED_USRSET, hDlg, wUsIndex);

			if (wUsIndex == -1)
			{
				SetControlState(hDlg,IDC_BTN_SHARED_US_SAVE, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_BTN_SHARED_US_DEL, SCUI_STATE_OFF);

				pUserSetting->AddUntitled(hDlg);
			}
			else
			{
				if (wUsIndex == 0)
				{
					SetControlState(hDlg, IDC_BTN_SHARED_US_SAVE, SCUI_STATE_SHOW);
					SetControlState(hDlg, IDC_BTN_SHARED_US_DEL, SCUI_STATE_OFF);
				}
				else
				{
					SetControlState(hDlg, IDC_BTN_SHARED_US_SAVE, SCUI_STATE_OFF);
					SetControlState(hDlg, IDC_BTN_SHARED_US_DEL, SCUI_STATE_ON);
				}

				pUserSetting->DeleteUntitled(hDlg);
			}

		}
		else
		{
			slog.putLog("CDocumentPropertyPage::SetUserSettingState -> NULL_OBJECT(CShCmbUserset)\n");
		}
	}
}
//<S><E> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
/*++

Routine Name:

    CDocPropPage::DestroyUIComponents

Routine Description:

    Destroy all control handler classes that have been added into the collection.

Arguments:

    None

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
HRESULT
CDocumentPropertyPage::DestroyUIComponents(
    VOID
    )
{
    HRESULT hr = S_OK;

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
 
    return hr;
}

void  CDocumentPropertyPage::SaveFeatureOptions()
{
	DWORD dwPairsWritten = 0;
	DWORD dwResult = 0;
	BOOL bCustomPaperSelected = FALSE ;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UINT FeatureCount = (UINT)m_pFeatureInfoList[0].nNummberOfFeatures;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile (ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);

	//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
	///Custom Paper values are changing to Default even though cancel button is clicked
	CUPSReg pregUps(m_hStringResourceHandle, m_pPrinterName);
	//pjsonups = new CShJsonUserPSize(m_hInst, m_pszSvrPrnName);
	CShJsonUserPSize	*pjsonups = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
		pjsonups->Init();
	}
//<S><D>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
   //pregUps.WriteUPSData(m_pPrinterName, DMPAPER_CUSTOM, &m_ppi->PaperSize_upsd);
//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
	if(m_pmcf->IsWriteToJson() == TRUE)
		pjsonups->WriteUPSData(DMPAPER_CUSTOM_ZOOM, &m_ppi->FitToPaperSize_upsd);
	else
		pregUps.WriteUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &m_ppi->FitToPaperSize_upsd);
	//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
	//<S><A>To Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
	//<S><A>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
	//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
	MIP_MINMAX_CONV_PARAM stCnvParam;
	SecureZeroMemory(&stCnvParam, sizeof(MIP_MINMAX_CONV_PARAM));
	stCnvParam.paper = mip_paper_cus_usr;
	stCnvParam.mi = mip_mi_mill;
	stCnvParam.pos = 1;
	// <E> 2016.04.12 SSL:K.Hamaguchi
	if (m_PropertySheetState.wCusUnit == unit_inch)
	{
		stCnvParam.wl = mip_wl_width;
		m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = InchToPixel_CusPaper(m_PropertySheetState.dwCustomWidth, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
		stCnvParam.wl = mip_wl_length;
		m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = InchToPixel_CusPaper(m_PropertySheetState.dwCustomLength, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);

	}
	else
	{
		{
			long	lInchWA;
			long	lInchLA;
			stCnvParam.mi = mip_mi_inch;
			stCnvParam.pos = 10;
			stCnvParam.wl = mip_wl_width;
			lInchWA = MilliToInchNoChk_CusPaper(m_PropertySheetState.dwCustomWidth, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);
			stCnvParam.wl = mip_wl_length;
			lInchLA = MilliToInchNoChk_CusPaper(m_PropertySheetState.dwCustomLength, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);

			stCnvParam.mi = mip_mi_inch;
			stCnvParam.pos = 10;
			stCnvParam.wl = mip_wl_width;
			m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = (DWORD)InchToPixel_CusPaper(lInchWA, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
			stCnvParam.wl = mip_wl_length;
			m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = (DWORD)InchToPixel_CusPaper(lInchLA, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
		}
	}
	//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla

	//while Clicking on apply/ok button, that time data should be store in the registry
	//if factory default drop down is selected that time that code should execute
	if (m_ppi->wFDJobHandling == TRUE)
	{
		REGJCINFO regInfo;
		SecureZeroMemory(&regInfo, sizeof(REGJCINFO));
		CShRegJC pregJC(m_hStringResourceHandle, m_pPrinterName);
		if (PropStateToRegJCInfo(&m_PropertySheetState, &regInfo, m_pPrinterName) != FALSE)
		{
			(pregJC).WriteJCData(m_pPrinterName, &regInfo, JC_ALL, m_PropertySheetState.wPPlcyWinLogin);
		}
	}
	//<E>To Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
	CDevModeUpDate devmodeupdate(m_pOemPrivateDevMode,m_PaperInfo,m_pFeatureInfoList->pFeatureOptionPair,FeatureCount,m_hPrinterHandle,
		m_hStringResourceHandle,GetResourceIDInfoOfControl(PAPERSIZE), m_pPrinterName, &m_ppi->PaperSize_upsd, &m_ppi->FitToPaperSize_upsd);
	devmodeupdate.UpDateDevMode();
	m_pOemPublicDevMode->dmPaperSize = m_pOemPrivateDevMode->scpi.pub.dmPaperSize ;
	m_pOemPublicDevMode->dmPaperWidth = m_pOemPrivateDevMode->scpi.pub.dmPaperWidth ;
	m_pOemPublicDevMode->dmPaperLength = m_pOemPrivateDevMode->scpi.pub.dmPaperLength ;
	wcscpy_s(m_pOemPublicDevMode->dmFormName, MAX_FORMNAME_LENGTH + 1, m_pOemPrivateDevMode->scpi.pub.dmFormName);
	//<S><A> to implement Custom Orientation 13/8/2020 K M Harika
	m_pOemPublicDevMode->dmOrientation = m_pOemPrivateDevMode->scpi.pub.dmOrientation;
	//<E> to implement Custom Orientation 13/8/2020 K M Harika
   // wcscpy_s(m_pOemPublicDevMode->dmFormName,MAX_FORMNAME_LENGTH + 1,m_pOemPrivateDevMode->scpi.pub.dmFormName);
	// Driver setting will be given high priority for duplex case
	m_pOemPublicDevMode->dmDuplex = m_pOemPrivateDevMode->scpi.pub.dmDuplex ;

	m_pOemPublicDevMode->dmColor = m_pOemPrivateDevMode->scpi.pub.dmColor;
    m_pOemPublicDevMode->dmICMMethod = m_pOemPrivateDevMode->scpi.pub.dmICMMethod;
	m_pOemPublicDevMode->dmCopies = m_pOemPrivateDevMode->scpi.pub.dmCopies;
	m_pOemPublicDevMode->dmCollate = m_pOemPrivateDevMode->scpi.pub.dmCollate;

	/*if (m_ppi->pps->blLimitByOption == bool_true) {
		m_pOemPrivateDevMode->scpi.ext2.wFieldE2DM |= DM_LIMT_BY_OPTION;
	}
	else {
		m_pOemPrivateDevMode->scpi.ext2.wFieldE2DM &= ~DM_LIMT_BY_OPTION;
	}*/
	DWORD NumberOfChangedFeatureOptionPairs = 0;
	/*if(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FeatureCount-1].pszOption,"Default"))
	{
		m_pFeatureInfoList->pFeatureOptionsList[FeatureCount-1].uChangedOptionIndex = 0 ;
	}
	else if(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FeatureCount-1].pszOption,"UnTitled"))
	{
		m_pFeatureInfoList->pFeatureOptionsList[FeatureCount-1].uChangedOptionIndex = 2 ;
	}
	else
	{
		m_pFeatureInfoList->pFeatureOptionsList[FeatureCount-1].uChangedOptionIndex = 1 ;
	}*/
	for(UINT uFeatureIndex=0; uFeatureIndex < FeatureCount;uFeatureIndex++)
    {
        if(m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uDefaultOptionIndex != m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex)
        {
            NumberOfChangedFeatureOptionPairs ++;

        }
		
		if((!strcmp("PaperSize",m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszFeature)) &&
	        (!strcmp("CUSTOMSIZE",m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption)) )
        {
			//m_pOemPublicDevMode->dmPaperSize = 0;
			m_pOemPublicDevMode->dmPaperSize = CUSTOM_PAPER;
			m_pOemPublicDevMode->dmFields |= DM_PAPERLENGTH|DM_PAPERWIDTH;
			////m_pOemPublicDevMode->dmPaperSize = m_pOemPrivateDevMode->scpi.pub.dmPaperSize;
			wcscpy_s(m_pOemPublicDevMode->dmFormName, MAX_FORMNAME_LENGTH + 1, m_pOemPrivateDevMode->scpi.pub.dmFormName);
			
		}
		
    }

	if(0 < NumberOfChangedFeatureOptionPairs)
	{
		UINT uChangedFeatureOptionList = 0;
		PRINT_FEATURE_OPTION* pfo = new PRINT_FEATURE_OPTION[NumberOfChangedFeatureOptionPairs]();
		for(UINT uFeatureIndex=0; uFeatureIndex < FeatureCount;uFeatureIndex++)
		{
			if(m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uDefaultOptionIndex !=m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex)
			{
				m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uDefaultOptionIndex = m_pFeatureInfoList->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex;
				pfo[uChangedFeatureOptionList].pszFeature = m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszFeature;
				pfo[uChangedFeatureOptionList].pszOption = m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption;
				uChangedFeatureOptionList++;
			}
		}
		
		HRESULT hRes = m_pPrintCoreHelperUni->SetOptions(NULL,0,SETOPTIONS_FLAG_KEEP_CONFLICT,pfo,NumberOfChangedFeatureOptionPairs,&dwPairsWritten,&dwResult);
		//<S><A>To Fix Redmine #7651,03-07-2024,SSDI:Harika
		DWORD tempPairsWritten = 0;
		HRESULT hr;
		for (int i = 0; i < NumberOfChangedFeatureOptionPairs; ++i) {
			// Check each pair individually to find the problematic ones
			hr = m_pPrintCoreHelperUni->SetOptions(NULL, 0, SETOPTIONS_FLAG_KEEP_CONFLICT, &pfo[i], 1, &tempPairsWritten, &dwResult);
		}
		//<E>To Fix Redmine #7651,03-07-2024,SSDI:Harika
		bCustomPaperSelected = FALSE ;
		if(NULL != pfo)
		{
			delete[] pfo;
			pfo = NULL ;
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::SaveFeatureOptions -> NULL_OBJECT(pfo)\n");
		}
	}

	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = nullptr;
	}
	if (pjsonups != NULL)
	{
		delete pjsonups;
		pjsonups = nullptr;
	}
}

/*++

Routine Name:

    CDocumentPropertyPage::DisplayHelpInformation

Routine Description:

    This routine will invoke the HTML Help API, which will display the help information
	using the chm file.

Arguments:

    hDlg - handle to the parent window

Return Value:

   None
--*/

void CDocumentPropertyPage::DisplayHelpInformation( _In_ CONST HWND hDlg)
{
	HtmlHelp(hDlg, m_pHelpFilePath, HH_HELP_CONTEXT,
								(DWORD)GetContextHelpID(m_wID,
								m_pHelpID));
}

/*++

Routine Name:

    CDocumentPropertyPage::ShowHelp

Routine Description:

    This routine will invoke the HTML Help API, which will display the help information
	using the chm file.This will be invoked only when user clicks F1 for the control highligted
	in the UI.

Arguments:

    pHelpInformation - LPHELPINFO given by the UI.

Return Value:

   None
--*/
HRESULT CDocumentPropertyPage::ShowHelp(LPHELPINFO pHelpInformation)
{
	HRESULT hr = E_FAIL;
	if(pHelpInformation)
	{
		DWORD			dwHelpIDs[3] = {0};
		if(CreateHtmlHelpArray(dwHelpIDs, m_pHelpID, (*pHelpInformation).iCtrlId) != FALSE)
		{
			HtmlHelp((HWND)(*pHelpInformation).hItemHandle, m_pHelpFilePath, HH_HELP_CONTEXT, (DWORD_PTR)dwHelpIDs[1]);
			hr = S_OK;
		}
	}
	return hr;
}

/*++
Routine Name:

    CDocumentPropertyPage::GetContextHelpID

Routine Description:

    This routine will fetch the Help ID's for the coreesponding Contol ID.

Arguments:

    pHelpID - Destination buffer to get the actual Help ID value.
	lCtrlID - Input Control ID.

Return Value:

   TRUE, FALSE
--*/
DWORD CDocumentPropertyPage::GetContextHelpID(DWORD lCtrlID, DWORD *pHelpID)
{
	DWORD			dwRet = 0;
	long			index;
	if(pHelpID != NULL)
	{
		index = 0;

		while(pHelpID[index] != 0)
		{
			if(pHelpID[index] == (DWORD)lCtrlID)
			{
				dwRet = pHelpID[index + 1];
				break;
			}

			index += 2;
		}
	}
	return dwRet;
}
PRESOURCEIDINFO CDocumentPropertyPage::GetResourceIDInfoOfControl(std::string strFeatureName)
{
	ResourceIDInfoMap::iterator ResourceInfoIterator = m_pResourceIDInfoMap->find(strFeatureName);
	if(ResourceInfoIterator != m_pResourceIDInfoMap->end())
	{
		return &(ResourceInfoIterator->second) ;
	}
	return NULL;
}

VOID CDocumentPropertyPage::CreatePaperSizeToTiledPamphletPaperSizeMap()
{
	m_PaperSizeToTiledPamphletPaperSizeMap["A4"] = "A3";
	m_PaperSizeToTiledPamphletPaperSizeMap["A5"] = "A4";
	m_PaperSizeToTiledPamphletPaperSizeMap["B5"] = "B4";
	m_PaperSizeToTiledPamphletPaperSizeMap["LETTER"] = "TABLOID";
}
BOOL CDocumentPropertyPage::DisplayStapleInformationMessage(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL lRet = FALSE;
	BYTE Flag = 0;
	short	msg_id_lst[30];
	CShLimit	*plim = NULL;

	plim = new CShLimit(NULL, NULL);
	if (plim == NULL)
	{
		return false;
	}
	CShowMsgStaple*  wObjCStapleShowMessage = NULL;
	wObjCStapleShowMessage = new CShowMsgStaple(hDlg, m_hStringResourceHandle, m_hDialogResourceHandle,
		m_hBitmapResourceHandle,m_pPrinterName);
	if(wObjCStapleShowMessage == NULL)
	{
		return FALSE;
	}
	SHORT wPre  = (short)m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	SHORT wStaple = (short)GetValue(hDlg,IDC_CMB_STAPLE);
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		if ((wStaple == staple_1) || (wStaple == staple_2) || (wStaple == staple_1_stapleless))
		{
			if (m_ObjCShowMessage != NULL) {
				short wCount = (*plim).Lim_MakeMsgForID(&msg_id_lst[0], IDS_MESSAGE_203);
				(*m_ObjCShowMessage).MakeDispMessage(hDlg, &msg_id_lst[0], wCount);
				lRet = TRUE;
				goto EXIT;
			}
		}

	}
	//<S><A>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
	if (!strcmp("FORMSOURCE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption) && ReadStapleLessFlag(Flag))
	{
		if (wStaple == staple_1_stapleless)
		{
			if (m_ObjCShowMessage != NULL) {
				short wCount = (*plim).Lim_MakeMsgForID(&msg_id_lst[0], IDS_MESSAGE_96);
				(*m_ObjCShowMessage).MakeDispMessage(hDlg, &msg_id_lst[0], wCount);
				lRet = TRUE;
				goto EXIT;
			}
		}
	}
	//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	if (wPre == staple_none && wStaple != staple_none)
	{
		if (wObjCStapleShowMessage->IsNeverShow() == FALSE)
		{
			wObjCStapleShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_109, MB_ICONINFORMATION | MB_OK);
			lRet = FALSE;
		}
	}
EXIT:
	if (wObjCStapleShowMessage != NULL)
	{
		delete wObjCStapleShowMessage;
		wObjCStapleShowMessage = NULL;
	}
	if (plim != NULL)
	{
		delete	plim;
		plim = NULL;
	}
	return lRet;
}
//<S><A>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
//to display information message for paper tray combo box
//=============================================================================
// function
//      DisplayPaperTrayVsStapleInformationMessage
//
// parameters
//     HWND
//
// return value
//      BOOL 
//		
// outline
//      DisplayPaperTrayVsStapleInformationMessage
//=============================================================================
BOOL CDocumentPropertyPage::DisplayPaperTrayVsStapleInformationMessage(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL lRet = FALSE;
	short	msg_id_lst[30];
	CShLimit	*plim = NULL;
	plim = new CShLimit(NULL, NULL);
	BYTE Flag = 0;

	if (plim == NULL)
	{
		return false;
	}
	CShowMsgStaple*  wObjCStapleShowMessage = NULL;
	wObjCStapleShowMessage = new CShowMsgStaple(hDlg, m_hStringResourceHandle, m_hDialogResourceHandle,
		m_hBitmapResourceHandle, m_pPrinterName);
	if (wObjCStapleShowMessage == NULL)
	{
		return FALSE;
	}

	SHORT wPre = (short)m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex;
	SHORT wPaperTray = (short)GetValue(hDlg, IDC_CMB_PSEL_PSRC);
	//<S><A>To Fix Redmine #7346,31-05-2024,SSDI:Manoj S
	SHORT wstapleval = (short)m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	//if (!strcmp("STAPLE_STAPLELESS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption) && ReadStapleLessFlag(Flag))
	if ((wstapleval == staple_1_stapleless) && ReadStapleLessFlag(Flag) && (m_PropertySheetState.byStapleless))
	{
		if (wPaperTray == PAPERSRC_AutoSelect)
		{
			if (m_ObjCShowMessage != NULL) {
				short wCount = (*plim).Lim_MakeMsgForID(&msg_id_lst[0], IDS_MESSAGE_95);
				(*m_ObjCShowMessage).MakeDispMessage(hDlg, &msg_id_lst[0], wCount);
				lRet = TRUE;
				goto EXIT;
			}
		}
	}
	//<E>To Fix Redmine #7346,31-05-2024,SSDI:Manoj S

EXIT:
	if (wObjCStapleShowMessage != NULL)
	{
		delete wObjCStapleShowMessage;
		wObjCStapleShowMessage = NULL;
	}
	if (plim != NULL)
	{
		delete	plim;
		plim = NULL;
	}
	return lRet;
}
//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
//<S><A>To Fix redmine Bug #3510,2021.28.07,ssdi:Chanchal Singla
//Paper Type retains incorrect value
BOOL CDocumentPropertyPage::DisplayPaperTypeInformationMessage(HWND hDlg, WORD iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL blRet = FALSE;
	SHORT msgID = 0;
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	LONG wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	LONG wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	LONG wPunch = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex;
	UINT PaperType = GetValue(hDlg, IDC_CMB_PSEL_PTYPE);
	
	//<S><D>to Delete the code to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
	//Bug #3666:Information message fails to display after clicking factory defaults
	//Bug #3464 :Information Message is displaying even though 2-Sided Printing drop down is None.
	//if (/*(wBooklet != booklet_none) && */(wDuplex != 0))
	//{
	//	return FALSE;
	//}
	//<E>to Delete the code to  fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
	//Staple Restrictions
	if (((PaperType == PAPERTYPE_Labels)
		|| (PaperType == PAPERTYPE_HeavyPaper)
		|| (PaperType == PAPERTYPE_Transparency)
		|| (PaperType == PAPERTYPE_Envelope)
		|| (PaperType == PAPERTYPE_JapanesePostCard))
		&& (wStaple != 0))
	{
		if ((PaperType == PAPERTYPE_HeavyPaper)
			&& !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption, "STAPLE_SADDLE"))
		{
			blRet = FALSE;
		}
		else
		{
			blRet = TRUE;
			msgID = IDS_MESSAGE_29;
			goto EXIT;
		}
	}
	//Punch Restrictions
	if (((PaperType == PAPERTYPE_Labels)
		|| (PaperType == PAPERTYPE_PrePunched)
		|| (PaperType == PAPERTYPE_Transparency)
		|| (PaperType == PAPERTYPE_Envelope)
		|| (PaperType == PAPERTYPE_JapanesePostCard))
		&& (wPunch != 0))
	{
		blRet = TRUE;
		msgID = IDS_MESSAGE_29;
		goto EXIT;
	}
	//<S><A>to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
	//Bug #3666:Information message fails to display after clicking factory defaults
	//Bug #3464 :Information Message is displaying even though 2-Sided Printing drop down is None.
	// vs 2-sided
	if ((wDuplex == eSecondOption) || (wDuplex == eThirdOption))
	{
		//<S><C> To fix feedback issues 66 and 68, Harika, 20220830
		if ((PaperType == PAPERTYPE_Labels)
			|| (PaperType == PAPERTYPE_Transparency)|| (PaperType == PAPERTYPE_Envelope))
		//<E> To fix feedback issues 66 and 68, Harika, 20220830
		{
			blRet = TRUE;
			msgID = IDS_MESSAGE_28;
			goto EXIT;
		}
	}
	// Booklet
	if ((wDuplex == eFirstOption) && (wBooklet == booklet_standard))
	{
		if ((PaperType == PAPERTYPE_Labels)
			|| (PaperType == PAPERTYPE_Transparency))
		{
			blRet = TRUE;
			msgID = IDS_MESSAGE_28;
			goto EXIT;
		}
	}
	//<E>to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
EXIT:
	if (blRet == TRUE)
	{
		if (m_ObjCShowMessage != NULL)
		{
			m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, msgID, MB_ICONINFORMATION | MB_OK);
			blRet = TRUE;
		}
	}
	return blRet;
}
BOOL CDocumentPropertyPage::DisplayOutputandPaperSizeInformationMessage(HWND hDlg, WORD iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	long wOutputSize = 0;
	BOOL blRet = FALSE;
	LONG wPaper = 0; 
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	LONG wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	LONG wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	LONG wPunch = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex;
	long wBindedge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	UINT wPreOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;

	CShLimit	plim(NULL, NULL);
	//OutPutSize
	UIControlMap::const_iterator Iterator;
	Iterator = m_UIControls.find(IDC_CMB_OUTPUTSIZE);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			wOutputSize = (short)pImageCombo->GetVal(IDC_CMB_OUTPUTSIZE, hDlg);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::ShowStapleVsOriginalSizeVsOutputSizeInformationIcon -> NULL_OBJECT(pImageCombo)\n");
		}
	}
	else
	{
		wOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	}

	//PaperSize
	Iterator = m_UIControls.find(IDC_CMB_PAPER);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			wPaper = (short)pImageCombo->GetVal(IDC_CMB_PAPER, hDlg);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::ShowStapleVsOriginalSizeVsOutputSizeInformationIcon -> NULL_OBJECT(pImageCombo)\n");
		}
	}
	else
	{
		wPaper = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	}

	//Restriction Between Staple,OutputSize,PaperSize
	if (plim.limInfoMsgOutputSizeVsStaple(&m_PropertySheetState,m_pFeatureInfoList, wStaple,
		wOutputSize, wPreOutputSize, wPaper, wBooklet, wBindedge, iCtrlID) == TRUE)
	{
		blRet = TRUE;
	}

	//Restriction Between Punch,OutputSize,PaperSize
	if (plim.limInfoMsgOutputSizeVsPunch(&m_PropertySheetState, m_pFeatureInfoList, wPunch,
		wOutputSize, wPreOutputSize, wPaper, wBooklet, wBindedge, iCtrlID) == TRUE)
	{
		blRet = TRUE;
	}

	if (blRet == TRUE)
	{
		if (m_ObjCShowMessage != NULL)
		{
			m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_29, MB_ICONINFORMATION | MB_OK);
			blRet = TRUE;
		}
	}
	return blRet;
}
//<E>To Fix redmine Bug #3510,2021.28.07,ssdi:Chanchal Singla
//<S><A>To Fix Bug #3937,2021.07.28,SSDI: Chanchal Singla
//Information icons missing/ information icons are un-necessarily displayed
BOOL CDocumentPropertyPage::DisplayPaperTrayVsOriginalSizeDialog(HWND hDlg, WORD iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL bRet = FALSE;
	UINT PaperSize = GetValue(hDlg, IDC_CMB_PAPER);
	if (!strcmp("SAOS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption))
	{
		if (((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY1")
			|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY2")
			|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY3")
			|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY4"))
			&& ((PaperSize >= PAPERSIZE_ENV_DL && PaperSize <= PAPERSIZE_JAPANESE_POSTCARD))))
		{
			bRet = TRUE;
			return bRet;
		}
	}
	return bRet;
}
//<S><A>To Fix Redmine Bug #3936,2021.28.07,ssdi:Chanchal Singla
//Wrong information message is displayed
BOOL CDocumentPropertyPage::DisplayPaperTrayVsOutputSizeDialog(HWND hDlg, WORD iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL bRet = FALSE;
	UINT wOutPutSize = GetValue(hDlg, IDC_CMB_OUTPUTSIZE);
	if (((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY1")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY2")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY3")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY4"))
		&& ((wOutPutSize >= OUTPUTPAPERSIZE_ENV_DL && wOutPutSize <= OUTPUTPAPERSIZE_JAPANESE_POSTCARD))))
	{
		bRet = TRUE;
		return bRet;
	}
	return bRet;
}
//<E>To Fix Redmine Bug #3936,2021.28.07,ssdi:Chanchal Singlas
//<E>To Fix Bug #3937,2021.07.28,SSDI: Chanchal Singla
BOOL CDocumentPropertyPage::DisplayOrientationInformationMessage(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	SHORT lOrientationPre = (short)m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex;
	SHORT val = (short)GetValue(hDlg,IDC_CMB_ORIENT);
	BOOL			blFlg = FALSE;
	short			wOrient;				// orientation
	BOOL dwRet = FALSE;
	long			lMsgID = 0;

	short	msg_id_lst[30];
	CShLimit	*plim = NULL;

	plim = new CShLimit(NULL, NULL);
	if (plim == NULL)
	{
		return false;
	}
	if ((eFirstOption == val)|| (eThirdOption == val)) {
		wOrient = ori_port;
	}
	else {
		wOrient = ori_land;
	}

	// Orientation - Booklet
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		// Orientation 
		if ((lOrientationPre == eFirstOption) || (lOrientationPre == eThirdOption)) {
			if (wOrient != ori_port) {
				blFlg = TRUE;
			}
		}
		else {
			if ( wOrient != ori_land) {
				blFlg = TRUE;
			}
		}

		if (blFlg)
		{
			m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_81, MB_ICONINFORMATION | MB_OK);
			dwRet = TRUE;
			goto EXIT;
		}
	}

	// Margin Shift - Orientation / 2-Sided Printing / N-Up
	// Margin Shift - Position 
	lMsgID = limTabCtrlPosition(hDlg, m_pFeatureInfoList);
	if (lMsgID != 0)
	{
		short	wCount = (*plim).Lim_MakeMsgForID(&msg_id_lst[0], (short)lMsgID);
		(*m_ObjCShowMessage).MakeDispMessage(hDlg, &msg_id_lst[0], wCount);
		dwRet = TRUE;
	}

EXIT:
	if (plim != NULL)
	{
		delete	plim;
		plim = NULL;
	}
	return dwRet;
}
BOOL CDocumentPropertyPage::DisplayNUPInformationMessage(HWND hDlg)
{
	BOOL lRet = FALSE;
	long			lMsgID = 0;
	short	msg_id_lst[30];
	CShLimit	*plim = NULL;

	plim = new CShLimit(NULL, NULL);
	if (plim == NULL)
	{
		return false;
	}
	// Orientation / N-Up - Booklet
	// Booklet - Binding 
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		SHORT wBindedge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
		if (limDuplexVsBinEdge(hDlg, wBindedge) != FALSE) {

			if (m_ObjCShowMessage != NULL) {
				short wCount = (*plim).Lim_MakeMsgForID(&msg_id_lst[0], IDS_MESSAGE_203);
				(*m_ObjCShowMessage).MakeDispMessage(hDlg, &msg_id_lst[0], wCount);
				lRet = TRUE;
				goto EXIT;
			}
		}
	}
	// Margin Shift - Orientation / 2-Sided Printing / N-Up
	// Margin Shift - Position 
	lMsgID = limTabCtrlPosition(hDlg,m_pFeatureInfoList);
	if (lMsgID != 0) 
	{
		short	wCount = (*plim).Lim_MakeMsgForID(&msg_id_lst[0], (short)lMsgID);

		// Coverity Fix: 91235 - SSDI Seetharam - 20210504
		if (m_ObjCShowMessage != NULL) 
			(*m_ObjCShowMessage).MakeDispMessage(hDlg, &msg_id_lst[0], wCount);
		lRet = TRUE;
	}

EXIT:
	if (plim != NULL)
	{
		delete	plim;
		plim = NULL;
	}
	return lRet;
}
BOOL CDocumentPropertyPage::DisplayDuplexInformationMessage(HWND hDlg)
{
	BOOL lRet = FALSE;
	long			lMsgID = 0;
	short	msg_id_lst[30];
	CShLimit	*plim = NULL;

	plim = new CShLimit(NULL, NULL);
	if (plim == NULL)
	{
		return false;
	}
	// Margin Shift - Orientation / 2-Sided Printing / N-Up
	// Margin Shift - Position 
	lMsgID = limTabCtrlPosition(hDlg, m_pFeatureInfoList);
	if (lMsgID != 0)
	{
		short	wCount = (*plim).Lim_MakeMsgForID(&msg_id_lst[0], (short)lMsgID);
		(*m_ObjCShowMessage).MakeDispMessage(hDlg, &msg_id_lst[0], wCount);
		lRet = TRUE;
	}

//EXIT:
	if (plim != NULL)
	{
		delete	plim;
		plim = NULL;
	}
	return lRet;
}
HRESULT  CDocumentPropertyPage::DisplayCustomDialog(HWND hDlg,WORD iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	int nCount = SendMessage(::GetDlgItem(hDlg, iCtrlID), CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
	int nCurSel = SendMessage(::GetDlgItem(hDlg, iCtrlID), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	
	if (!strcmp("ON", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption)
		&& (nCurSel == PAPERSIZE_CUSTOMSIZE) && (IDC_CMB_PAPER == iCtrlID))
	{
		m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_78, MB_ICONINFORMATION | MB_OK);
		SendDlgItemMessage(hDlg, iCtrlID, CB_SETCURSEL, m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex, 0);
		return S_OK;
	}
	//<S><A>to fix Bug #3504,2021-05-20,SSDi:Chanchal Singla
	//	Paper Source Tab : Information Message is not displaying, when we selected Custom paper.
	if (!strcmp("None", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		if ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY3")
			|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY4"))
			&& (nCurSel == 20) && (IDC_CMB_PAPER == iCtrlID))
		{
			m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_76, MB_ICONINFORMATION | MB_OK);
			SendDlgItemMessage(hDlg, iCtrlID, CB_SETCURSEL, m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex, 0);
			return S_OK;
		}
		if ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY3")
			|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "TRAY4"))
			&& ((nCount - 1) == nCurSel) && (IDC_CMB_OUTPUTSIZE == iCtrlID))
		{
			m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_77, MB_ICONINFORMATION | MB_OK);
			SendDlgItemMessage(hDlg, iCtrlID, CB_SETCURSEL, m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex, 0);
			return S_OK;
		}
	}
	//<E>to fix Bug #3504,2021-05-20,SSDi:Chanchal Singla
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		// Dilaog Should only open for Original Paper Size.
		if ((nCount - 1) == nCurSel && (iCtrlID == IDC_CMB_PAPER))
		{
			// Display the custom paper dialog box
			ShowCustomPaperDialog(iCtrlID, hDlg);
		}
	}
	else if ((nCount - 1) == nCurSel)
	{
		// Display the custom paper dialog box
		ShowCustomPaperDialog(iCtrlID, hDlg);
	}

	return 0L;
}

BOOL CDocumentPropertyPage::SetTiledPamphletPaperSizeIndex(_In_ CONST HWND   hDlg,INT& iTilesPamphletPaperSizeIndex)

{
	iTilesPamphletPaperSizeIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	if(strcmp("TiledPamphlet",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAMPHLET].pszOption))
	{
		return FALSE ;
	}
	std::string strPaperSize = m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption;
	auto iIteratorToPaperSizeToPamphletPaperSizeMap = m_PaperSizeToTiledPamphletPaperSizeMap.find(strPaperSize.c_str());
	if(m_PaperSizeToTiledPamphletPaperSizeMap.end() == iIteratorToPaperSizeToPamphletPaperSizeMap)
	{
		return FALSE ;
	}
	auto iIteratorToPaperSizeFeatureResourceID = m_pResourceIDInfoMap->find(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszFeature);
	if(iIteratorToPaperSizeFeatureResourceID == m_pResourceIDInfoMap->end())
	{
		return FALSE ;
	}
	auto iIteratorToPaperSizeOptionResourceID = iIteratorToPaperSizeFeatureResourceID->second.uOptionResourceID.find(iIteratorToPaperSizeToPamphletPaperSizeMap->second);
	if(iIteratorToPaperSizeOptionResourceID == iIteratorToPaperSizeFeatureResourceID->second.uOptionResourceID.end())
	{
		return FALSE ;
	}
	
	WCHAR szItem[MAX_PATH]={0};
	if (LoadString(m_hStringResourceHandle, iIteratorToPaperSizeOptionResourceID->second.first,
									szItem, countof(szItem)) > 0)
	{            
		iTilesPamphletPaperSizeIndex = static_cast<UINT>(SendDlgItemMessage(hDlg, IDC_CMB_OUTPUTSIZE,CB_FINDSTRINGEXACT ,(WPARAM)-1,(LPARAM)szItem));
	}
	
	return TRUE ;	 
}
BOOL CDocumentPropertyPage::UndoControlSelection(CONST HWND hDlg,INT   iCtrlID)
{
	BOOL bUndoSelection = FALSE ;
	BOOL bOutputSAOS = FALSE;
	CShImageCombo * pFitToPaperSizeControl = NULL;
	if((IDC_CMB_PAPER == iCtrlID ) || IDC_CMB_OUTPUTSIZE == iCtrlID)
	{
		UINT uPaperSelectedIndex = 0;
		UINT uOutputIndex = 0;
		auto iUIControlIterator =  m_UIControls.find(IDC_CMB_PAPER);
		if(iUIControlIterator == m_UIControls.end())
		{
			uPaperSelectedIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
		}
		else
		{
			uPaperSelectedIndex = GetValue(hDlg, IDC_CMB_PAPER);
			if (-1 == uPaperSelectedIndex)
			{
				uPaperSelectedIndex = 0;
			}
		}

		//Coverity 91205 fix,20210603,SSDI;Chanchal Singla
		UIControlMap::const_iterator iFitToPaperSizeComboIterator =  m_UIControls.find(IDC_CMB_OUTPUTSIZE);
	
		if(iFitToPaperSizeComboIterator != m_UIControls.end())
		{
			pFitToPaperSizeControl = static_cast<CShImageCombo*>(iFitToPaperSizeComboIterator->second);
			uOutputIndex = pFitToPaperSizeControl->GetVal(IDC_CMB_OUTPUTSIZE, hDlg);
			if (uOutputIndex == 0)
			{
				bOutputSAOS = TRUE;
			}
		}
		else
		{
			uOutputIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
		}
	
		
		if (uOutputIndex == 0)
		{
			//<S><A>To Fix Bug #3937,2021.07.28,SSDI: Chanchal Singla
			//Information icons missing/ information icons are un-necessarily displayed
			if (TRUE == DisplayPaperTrayVsOriginalSizeDialog(hDlg, IDC_CMB_PAPER))
			{
				m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_76, MB_ICONINFORMATION | MB_OK);
				
				bUndoSelection = TRUE;
			}
			//<E>To Fix Bug #3937,2021.07.28,SSDI: Chanchal Singla
			//Coverity 91519 Coverity Fix - SSDI Seetharam : 20210705
			else if (pFitToPaperSizeControl != NULL)
			{
				//<S><C>fix redmine Bug #4123- Issue-1,13.August.2021,SSDI:Chanchal Singla
				//Output Size does not set to Paper Size based on the Region Settings
				if ((m_PropertySheetState.byMaxPaperSize == eSecondOption) && 
					((uPaperSelectedIndex == PAPERSIZE_A3) || (uPaperSelectedIndex == PAPERSIZE_B4)
						|| (uPaperSelectedIndex == PAPERSIZE_TABLOID) || (uPaperSelectedIndex == PAPERSIZE_JENV_KAKU2)))
				{
					bUndoSelection = FALSE;
				}
				else
				{
					//<S><A>to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
					//Bug #3666:Information message fails to display after clicking factory defaults
					//Bug #3464 :Information Message is displaying even though 2-Sided Printing drop down is None.
					//bUndoSelection = pFitToPaperSizeControl->UndoControlSelection(hDlg, (uPaperSelectedIndex + 1), iCtrlID);
					bUndoSelection = UndoPaperSelection(hDlg, uPaperSelectedIndex, uOutputIndex);
					//<E>to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
				}
				//<E>fix redmine Bug #4123- Issue-1,13.August.2021,SSDI:Chanchal Singla
			}
		}
		//<S><A>to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
		//Bug #3666:Information message fails to display after clicking factory defaults
		//Bug #3464 :Information Message is displaying even though 2-Sided Printing drop down is None.
		else
		{
			//this condition for a fittopapersize
			//<S><A>To Fix Redmine Bug #3936,2021.28.07,ssdi:Chanchal Singla
			//Wrong information message is displayed
			if (DisplayPaperTrayVsOutputSizeDialog(hDlg, IDC_CMB_OUTPUTSIZE) == TRUE)
			{
				m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_77, MB_ICONINFORMATION | MB_OK);
				bUndoSelection = TRUE;
			}
			//<E>To Fix Redmine Bug #3936,2021.28.07,ssdi:Chanchal Singla
			else
			{
				bUndoSelection = UndoPaperSelection(hDlg, uPaperSelectedIndex, uOutputIndex);
			}
			if (bUndoSelection == TRUE)
			{
				bOutputSAOS = TRUE;
			}
		}
		//<E>to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
	}
	return (bUndoSelection & bOutputSAOS);
}
VOID CDocumentPropertyPage::ChangeComboBoxSelectedItem(_In_ CONST HWND   hDlg,UINT&   iCtrlID,BOOL bTabDefaults)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
   if((IDC_CMB_PAPER == iCtrlID) /*|| (iCtrlID == IDC_RADIO_ZOOM_NORMAL)*/)
	{
		UINT iInputCtrlID = iCtrlID;
		auto iUIControlIterator =  m_UIControls.find(IDC_CMB_PAPER);
		if(iUIControlIterator == m_UIControls.end())
		{
			return ;
		}
		CUIControl * pUIControl = iUIControlIterator->second;
		if(NULL == pUIControl)
		{
			slog.putLog("CDocumentPropertyPage::ChangeComboBoxSelectedItem -> NULL_OBJECT(pUIControl)\n");
			return ;
		}
		INT iIndex = -1;
		SetTiledPamphletPaperSizeIndex(hDlg,iIndex);
		iCtrlID = IDC_CMB_OUTPUTSIZE;
		INT iFitToPaperSizeComboSize = SendDlgItemMessage(hDlg, iCtrlID, CB_GETCOUNT,0, 0);
		if((0 < iFitToPaperSizeComboSize)&& (iIndex== iFitToPaperSizeComboSize))
		{
			WCHAR szItem[MAX_PATH]={};
			if (0 < LoadString(m_hStringResourceHandle, m_DefaultSettings[FEATURE_FITTOPAPERSIZE].ResourceID,
											szItem, countof(szItem)))
			{            
				iIndex = static_cast<UINT>(SendDlgItemMessage(hDlg,iCtrlID,CB_FINDSTRINGEXACT ,(WPARAM)-1,(LPARAM)szItem));
			}
		}
		auto iFitToPaperSizeComboIterator =  m_UIControls.find(iCtrlID);
		if(iFitToPaperSizeComboIterator == m_UIControls.end())
		{
			return ;
		}
		CShImageCombo * pFitToPaperSizeControl = static_cast<CShImageCombo*>(iFitToPaperSizeComboIterator->second);
		if(NULL == pFitToPaperSizeControl)
		{
			slog.putLog("CDocumentPropertyPage::ChangeComboBoxSelectedItem -> NULL_OBJECT\n");
			return ;
		}
        
		INT iAddRemove = 0 ;
		if((IDC_CMB_PAPER == iInputCtrlID))
		{
			// <S><D> Redmine Ticket (#1741) - Envelope is not displayed when Custom Paper is selected.Date : 2014.12.24,SSDI : Debadatta Satpathy
			/*if(IDS_PAPERSIZE_256 == pUIControl->GetPreviousItemId())
			{
				iAddRemove = 1 ;
			}
			if(IDS_PAPERSIZE_256 == pUIControl->GetCurrentItemId())
			{
				iAddRemove = -1 ;
			}*/
			// <E> Redmine Ticket (#1741) - Envelope is not displayed when Custom Paper is selected.Date : 2014.12.24,SSDI : Debadatta Satpathy
			UINT uIndex = iIndex + 1;
			UINT uPreviousItemID = pFitToPaperSizeControl->GetPreviousItemId();
			UINT uCurrentItemID = pFitToPaperSizeControl->GetCurrentItemId();
			UINT iDependentControlID = iCtrlID ;
			pFitToPaperSizeControl->ChangeSelectedComboboxItem(hDlg,iInputCtrlID,uIndex);
			uPreviousItemID = pFitToPaperSizeControl->GetPreviousItemId();
			uCurrentItemID = pFitToPaperSizeControl->GetCurrentItemId();
			SendDlgItemMessage(hDlg, iCtrlID, CB_SETCURSEL, uIndex, 0);
			//Fixed Coverity issue - 90464 - SSDI:Seetharam - 20201020
			//if((0 != iAddRemove) && (FALSE == bTabDefaults))
			////if(0 != iAddRemove)
			//{
			//	UINT uItemID = uPreviousItemID ;
			//	if(0 > iAddRemove)
			//	{
			//		uItemID = uCurrentItemID ;
			//	}
			//	std::pair<UINT,UINT>FeatureOptionIdPair = std::make_pair(iDependentControlID,uItemID);
			//	std::map<std::string,UINT>ConstraintFeatureNameID;
			//	CONSTRAINTFEATUREOPTIONINFO constraintfeatureoptioninfo ;
			//	ConstraintFeatureInfoMap::iterator iIteratorToConstraintFeatureInfoMap = m_ConstraintFeatureInfoMap->find(FeatureOptionIdPair);
			//	if(iIteratorToConstraintFeatureInfoMap != m_ConstraintFeatureInfoMap->end())
			//	{
			//		constraintfeatureoptioninfo =  iIteratorToConstraintFeatureInfoMap->second;
			//		AddOrRemoveConstraints(ConstraintFeatureNameID,constraintfeatureoptioninfo,iAddRemove,FeatureOptionIdPair);
			//	}
			//	UINT uConstrainedControlID = 0;
			//	for(auto iIteratorToConstraintFeatureNameID : ConstraintFeatureNameID)
			//	{
			//		UIControlMap::const_iterator iterUIComponents = m_UIControls.find(iIteratorToConstraintFeatureNameID.second);
			//		if(m_UIControls.end() != iterUIComponents)
			//		{
			//			CUIControl *pUIControl1  = iterUIComponents->second;
			//			if(nullptr !=  pUIControl1)
			//			{
			//				uConstrainedControlID = iIteratorToConstraintFeatureNameID.second;
			//				pUIControl1->OnInit(uConstrainedControlID,hDlg);
			//			}
			//			else
			//			{
			//				slog.putLog("CDocumentPropertyPage::ChangeComboBoxSelectedItem -> NULL_OBJECT\n");
			//			}
			//		}	
			//	}
			//	if(0 < iIteratorToConstraintFeatureInfoMap->second.uDependentControlID.size())
			//	{
			//		for(auto uIteratorToDependentControlID: iIteratorToConstraintFeatureInfoMap->second.uDependentControlID)
			//		ProcessDepdendentFeatureConstraints(hDlg,uIteratorToDependentControlID);
			//	}
			//
			//}
			
		}
	//	/*if((IDC_CMB_PAPER == iInputCtrlID) &&(FALSE == IsDlgButtonChecked(hDlg,IDC_RADIO_ZOOM_NORMAL)))
	//	{
	//		iCtrlID = iInputCtrlID ;
	//	}*/
	}
}

void  CDocumentPropertyPage::ProcessDepdendentFeatureConstraints(_In_ CONST HWND   hDlg,UINT   uDependentFeatureControlID,BOOL bTabDefaults)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	auto iterUIControl= m_UIControls.find(uDependentFeatureControlID);
	if(m_UIControls.end() != iterUIControl)
	{
		CUIControl *pDependentControl = iterUIControl->second ;
		if(NULL != pDependentControl)
		{
			NotifyConstrainedFeatures(hDlg,pDependentControl,uDependentFeatureControlID,bTabDefaults);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::ProcessDepdendentFeatureConstraints -> NULL_OBJECT(pDependentControl)\n");
		}
	}
}
void  CDocumentPropertyPage::NotifyConstrainedFeatures(_In_ CONST HWND   hDlg,CUIControl* pControl,UINT   iCtrlID,BOOL bTabDefaults)
{
		
		MFPPrinterUI_Logger slog(__FUNCTION__"\n");
		if((FALSE == bTabDefaults) && (IDC_BTN_CUSTOM_PAPER == iCtrlID))
		{
			iCtrlID = IDC_CMB_PAPER ;
			auto iPaperSizeComboControlIterator =  m_UIControls.find(iCtrlID);
			if(iPaperSizeComboControlIterator == m_UIControls.end())
			{
				return ;
			}
			pControl = iPaperSizeComboControlIterator->second ;
			if(nullptr == pControl)
			{
				slog.putLog("CDocumentPropertyPage::NotifyConstrainedFeatures -> NULL_OBJECT(pControl)\n");
				return ;
			}
			//<S><A> RedMine Ticket(#1141):Issue3: Japanese Post Card is displaying : 2014.04.08,SSDI:Gajanana D S 
			if(pControl->GetPreviousItemId() == pControl->GetCurrentItemId())
			{
				return ;
			}
			//<E> RedMine Ticket(#1141):Issue3: Japanese Post Card is displaying : 2014.04.08,SSDI:Gajanana D S 
		}
		//if(IDC_RADIO_ZOOM_NORMAL == iCtrlID)// && (FALSE == bTabDefaults))
		//{
		//	iCtrlID = IDC_COMBO_PAPER;
		//	UINT iPaperControlID = IDC_RADIO_ZOOM_NORMAL;
		//	ChangeComboBoxSelectedItem(hDlg,iPaperControlID,bTabDefaults);
		//	//Fixed Coverity issue - 90418 - SSDI:Seetharam-20201006
		//	if(iCtrlID == IDC_RADIO_ZOOM_FITTOPAGE_PAPER_SIZE)
		//	{
		//		return ;
		//	}
		//	ProcessDepdendentFeatureConstraints(hDlg,iPaperControlID);
		//}
		/*if((IDC_COMBO_PAPER == iCtrlID)&&(FALSE == bTabDefaults))
		{
			UINT iPaperControlID = iCtrlID;
			ChangeComboBoxSelectedItem(hDlg,iPaperControlID,bTabDefaults);
			if((iPaperControlID != IDC_COMBO_PAPER)&&(FALSE == bTabDefaults))
			{
				ProcessDepdendentFeatureConstraints(hDlg,iPaperControlID,bTabDefaults);
			}
		}*/
		std::vector<UINT>uDependentFeatureControlID;
		UINT uPrevoiusItemId = pControl->GetPreviousItemId();
		UINT uCurrentItemId = pControl->GetCurrentItemId();
		if(uCurrentItemId != uPrevoiusItemId)
		{
			if(eRadioButton == pControl->GetControlType())
			{
				iCtrlID = pControl->GetFeatureControlID();
			}
		}
		//<S><A> RedMine Ticket(#1105) Apply button is not enabled when Paper Type value is changed . : 2014.03.09, SSDI:Gajanana D S
		if((uPrevoiusItemId == uCurrentItemId) && (iCtrlID != IDC_CMB_PAPER) && (iCtrlID != IDC_CMB_OUTPUTSIZE))
		{
			return ;
		}
		//<E> RedMine Ticket(#1105) Apply button is not enabled when Paper Type value is changed . : 2014.03.09, SSDI:Gajanana D S
		std::pair<UINT,UINT>FeatureOptionIdPair = std::make_pair(iCtrlID,uPrevoiusItemId);
		std::map<std::string,UINT>ConstraintFeatureNameID;
		CONSTRAINTFEATUREOPTIONINFO constraintfeatureoptioninfo;
		ConstraintFeatureInfoMap::iterator iIteratorToConstraintFeatureInfoMap = m_ConstraintFeatureInfoMap->find(FeatureOptionIdPair);
		if(iIteratorToConstraintFeatureInfoMap != m_ConstraintFeatureInfoMap->end())
		{
			constraintfeatureoptioninfo =  iIteratorToConstraintFeatureInfoMap->second;
			AddOrRemoveConstraints(ConstraintFeatureNameID,constraintfeatureoptioninfo,-1,FeatureOptionIdPair);
			if(0 < constraintfeatureoptioninfo.uDependentControlID.size())
			{
				uDependentFeatureControlID = constraintfeatureoptioninfo.uDependentControlID;
				/*for(auto uIteratorToDependentControlID: constraintfeatureoptioninfo.uDependentControlID)
				{
					uDependentFeatureControlID.push_back(uIteratorToDependentControlID);
				}*/
				//ProcessDependentConstraints(ConstraintFeatureNameID,constraintfeatureoptioninfo,-1);
			}	
		}
		FeatureOptionIdPair = std::make_pair(iCtrlID,uCurrentItemId);
		iIteratorToConstraintFeatureInfoMap = m_ConstraintFeatureInfoMap->find(std::make_pair(iCtrlID,uCurrentItemId));
		if(iIteratorToConstraintFeatureInfoMap != m_ConstraintFeatureInfoMap->end())
		{
			constraintfeatureoptioninfo =  iIteratorToConstraintFeatureInfoMap->second;
			AddOrRemoveConstraints(ConstraintFeatureNameID,constraintfeatureoptioninfo,1,FeatureOptionIdPair);
			if(0 < constraintfeatureoptioninfo.uDependentControlID.size())
			{
				
				uDependentFeatureControlID = constraintfeatureoptioninfo.uDependentControlID;
				/*for(auto uIteratorToDependentControlID: constraintfeatureoptioninfo.uDependentControlID)
				{
					uDependentFeatureControlID.push_back(uIteratorToDependentControlID);
				}*/
			//	ProcessDependentConstraints(ConstraintFeatureNameID,constraintfeatureoptioninfo,1);
			}
		}
		
	/*	UINT uConstrainedControlID = 0;
		for(auto iIteratorToConstraintFeatureNameID : ConstraintFeatureNameID)
		{
			UIControlMap::const_iterator iterUIComponents = m_UIControls.find(iIteratorToConstraintFeatureNameID.second);
			if(m_UIControls.end() != iterUIComponents)
			{
				CUIControl *pUIControl  = iterUIComponents->second;
				if(nullptr !=  pUIControl)
				{
					uConstrainedControlID = iIteratorToConstraintFeatureNameID.second;
					pUIControl->OnInit(uConstrainedControlID,hDlg);
				}
			}	
		}*/
		if((0 < m_uFeatureControlID)/* && (IDC_RADIO_ZOOM_NORMAL != m_uFeatureControlID)*/)
		{
			SetFeatureOption(hDlg,iIteratorToConstraintFeatureInfoMap);
		}
		ResetInvalidSelectedOption(hDlg,iIteratorToConstraintFeatureInfoMap);
		if(0 < uDependentFeatureControlID.size())
		{
			for(auto uIteratorToDependentControlID: uDependentFeatureControlID)
			ProcessDepdendentFeatureConstraints(hDlg,uIteratorToDependentControlID);
		}
		/*if((IDC_CMB_PAPER == iCtrlID)&&(FALSE == bTabDefaults))
		{
			UINT iPaperControlID = iCtrlID;
			ChangeComboBoxSelectedItem(hDlg,iPaperControlID,bTabDefaults);
			if((iPaperControlID != IDC_CMB_PAPER)&&(FALSE == bTabDefaults))
			{
				ProcessDepdendentFeatureConstraints(hDlg,iPaperControlID,bTabDefaults);
			}
		}*/
		/*if((IDC_COMBO_PAPER == iCtrlID)&&(FALSE == bTabDefaults))
		{
			ChangeComboBoxSelectedItem(hDlg,iCtrlID,bTabDefaults);
			if((iCtrlID != IDC_COMBO_PAPER)&&(FALSE == bTabDefaults))
			{
				ProcessDepdendentFeatureConstraints(hDlg,iCtrlID,bTabDefaults);
			}
		}*/
		//ChangeComboBoxSelectedItem(hDlg,iCtrlID,bTabDefaults);
}
VOID CDocumentPropertyPage::ResetInvalidSelectedOption(_In_ HWND   hDlg,
									ConstraintFeatureInfoMap::iterator iIteratorToConstraintFeatureInfoMap
									)
{
	if(	(iIteratorToConstraintFeatureInfoMap != m_ConstraintFeatureInfoMap->end())&&
			(0 < iIteratorToConstraintFeatureInfoMap->second.ListOfInvalidFeatureOptions.size()))
		{
			auto iInvalidFeatureOptionIterator = m_pResourceIDInfoMap->find(iIteratorToConstraintFeatureInfoMap->second.ListOfInvalidFeatureOptions[0].first.c_str());
			if(iInvalidFeatureOptionIterator != m_pResourceIDInfoMap->end())
			{
				auto iterUIControl= m_UIControls.find(iInvalidFeatureOptionIterator->second.uFeatureResourceID);
				if(m_UIControls.end() != iterUIControl)
				{
					CUIControl *pInavlidFeatureControl = iterUIControl->second ;
					if(NULL != pInavlidFeatureControl)
					{
						if(TRUE == pInavlidFeatureControl->ResetInvalidFeatureOption(hDlg,iInvalidFeatureOptionIterator->second.uFeatureResourceID,iIteratorToConstraintFeatureInfoMap->second.ListOfInvalidFeatureOptions))
						{
							NotifyConstrainedFeatures(hDlg,pInavlidFeatureControl,iInvalidFeatureOptionIterator->second.uFeatureResourceID);
						}
					}
				}
			}
		}
}
BOOL CDocumentPropertyPage::IsAllowSetFeatureOption(VOID)
{
	if((IDC_CMB_PAPER == m_uFeatureControlID) || (IDC_CMB_OUTPUTSIZE == m_uFeatureControlID))
	{
		if((FALSE == strcmp("NONE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))&&
			(FALSE == strcmp("NONE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))&&
			(FALSE == strcmp("OFF",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption))
		  )
		{
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}
VOID CDocumentPropertyPage::SetFeatureOption(_In_ HWND   hDlg,
							ConstraintFeatureInfoMap::iterator iIteratorToConstraintFeatureInfoMap
							)
{
		if(	(iIteratorToConstraintFeatureInfoMap != m_ConstraintFeatureInfoMap->end())&&
			(0 < iIteratorToConstraintFeatureInfoMap->second.SetFeatureOption.first.size()) &&
			(0 < iIteratorToConstraintFeatureInfoMap->second.SetFeatureOption.second.size())
		  )
		{
			auto iSetFeatureOptionIterator = m_pResourceIDInfoMap->find(iIteratorToConstraintFeatureInfoMap->second.SetFeatureOption.first.c_str());
			if(iSetFeatureOptionIterator != m_pResourceIDInfoMap->end())
			{
				if(FALSE == IsAllowSetFeatureOption())
				{
					return ;
				}
				auto iterUIControl= m_UIControls.find(iSetFeatureOptionIterator->second.uFeatureResourceID);
				if(m_UIControls.end() != iterUIControl)
				{
					CUIControl *pSetFeatureControl = iterUIControl->second ;
					if(NULL != pSetFeatureControl)
					{
						if(TRUE == pSetFeatureControl->SetFeatureOption(hDlg,iSetFeatureOptionIterator->second.uFeatureResourceID,iIteratorToConstraintFeatureInfoMap->second.SetFeatureOption.second.c_str()))
						{
							//NotifyConstrainedFeatures(hDlg,pSetFeatureControl,iSetFeatureOptionIterator->second.uFeatureResourceID);
						}
					}
				}
			}
		}
}
//<S><C> to fix Coverity issue 90564, Harika 20201214
VOID CDocumentPropertyPage::AddOrRemoveConstraints(std::map<std::string,UINT>&ConstraintFeatureNameID,const CONSTRAINTFEATUREOPTIONINFO &constraintfeatureoptioninfo,INT iAddOrRemove,std::pair<UINT,UINT>FeatureOptionIdPair)
//<E> to fix Coverity issue 90564, Harika 20201214
{
	for(auto iIteratorToConstarintOptionMap : constraintfeatureoptioninfo.ConstrFeatureOptionMap)
		{
			ResourceIDInfoMap::iterator iIteratorToResourceIdInfoMap = m_pResourceIDInfoMap->find(iIteratorToConstarintOptionMap.first);
			if(iIteratorToResourceIdInfoMap != m_pResourceIDInfoMap->end())
			{
				iIteratorToResourceIdInfoMap->second.bIsControlItemsInitialized = FALSE;
				UIControlMap::const_iterator iterUIComponents = m_UIControls.find(iIteratorToResourceIdInfoMap->second.uFeatureResourceID);
                ECONTROLTYPE eControlType = eNone;
				if(m_UIControls.end() != iterUIComponents)
				{
					CUIControl *pUIControl  = iterUIComponents->second;
					if(nullptr !=  pUIControl)
					{
						eControlType = pUIControl->GetControlType();
						if(eStaticText != eControlType)
						{
							ConstraintFeatureNameID[iIteratorToConstarintOptionMap.first] = iIteratorToResourceIdInfoMap->second.uFeatureResourceID;
							// comment below code
							if( 0 < iIteratorToConstarintOptionMap.second.ConstraintFeatureAction)
							{
								iIteratorToResourceIdInfoMap->second.uConstraintAction = iIteratorToConstarintOptionMap.second.ConstraintFeatureAction;
							}
						}
					}
				}		
				for(auto iIteratorToConstraintOptionNameList : iIteratorToConstarintOptionMap.second.ConstraintOptionNameList)
				{
					OptionResourceID::iterator iOptionResourceIDIterator = iIteratorToResourceIdInfoMap->second.uOptionResourceID.find(iIteratorToConstraintOptionNameList);
					if(iOptionResourceIDIterator != iIteratorToResourceIdInfoMap->second.uOptionResourceID.end())
					{
						if(eStaticText == eControlType)
						{
							ConstraintFeatureNameID[iIteratorToConstarintOptionMap.first] = iOptionResourceIDIterator->second.first;
						}
						if(0 == iOptionResourceIDIterator->second.second.ConstraintFeatureOptionMessageIDList.size())
						{
							iOptionResourceIDIterator->second.second.uNumberOfConstraints += iAddOrRemove ;
							if( 0 < iIteratorToConstarintOptionMap.second.ConstraintFeatureAction)
							{
								iOptionResourceIDIterator->second.second.uConstraintAction = iIteratorToResourceIdInfoMap->second.uConstraintAction;
							}
							if(0 > iOptionResourceIDIterator->second.second.uNumberOfConstraints)
							{
								iOptionResourceIDIterator->second.second.uNumberOfConstraints = 0;
							}
							if(-1 == iAddOrRemove)
							{
							//	iOptionResourceIDIterator->second.second.ConstraintMessageIDList.erase(iIteratorToConstarintOptionMap.second.ConstraintMessageID);
							}
							else
							{
								if(0 < iIteratorToConstarintOptionMap.second.ConstraintMessageID)
								{
									//iOptionResourceIDIterator->second.second.ConstraintMessageIDList.insert(iIteratorToConstarintOptionMap.second.ConstraintMessageID);
						
								}
							}
						}
						
						// Mulitple Constraints
							for( auto &iFeatureOptionVec : iOptionResourceIDIterator->second.second.ConstraintFeatureOptionMessageIDList)
							{
								auto ConstraintFeatureOptionIDMap =  iFeatureOptionVec.ConstraintFeatureOptionIDList.find(FeatureOptionIdPair.first);
								if(ConstraintFeatureOptionIDMap != iFeatureOptionVec.ConstraintFeatureOptionIDList.end())
								{
									if(FeatureOptionIdPair.second == ConstraintFeatureOptionIDMap->second)
									{
										iFeatureOptionVec.uNumberOfConstraints += iAddOrRemove ;
										if(iFeatureOptionVec.uNumberOfConstraints > iFeatureOptionVec.ConstraintFeatureOptionIDList.size())
										{
										  iFeatureOptionVec.uNumberOfConstraints = iFeatureOptionVec.ConstraintFeatureOptionIDList.size();
										}
										if(iFeatureOptionVec.uNumberOfConstraints < 0)
										{
											iFeatureOptionVec.uNumberOfConstraints = 0;
										}
									//	iFeatureOptionVec.uConstraintAction = iIteratorToConstarintOptionMap.second.ConstraintFeatureAction;
									}
								}
							}
					}
				}
			}
		}
}
VOID CDocumentPropertyPage::ProcessDependentConstraints(std::map<std::string,UINT>&ConstraintFeatureNameID,
				CONSTRAINTFEATUREOPTIONINFO& constraintfeatureoptioninfo,INT iAddOrRemove)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UINT uDependentControlCurrentItemID = 0;
	for(auto iIteartorToDependentControlID : constraintfeatureoptioninfo.uDependentControlID)
	{
		auto iterUIComponents = m_UIControls.find(iIteartorToDependentControlID);
		if(m_UIControls.end() != iterUIComponents)
		{
			CUIControl *pUIControl  = iterUIComponents->second;
			if(nullptr !=  pUIControl)
			{
				uDependentControlCurrentItemID = pUIControl->GetCurrentItemId();
				ConstraintFeatureInfoMap::iterator iIteratorToConstraintFeatureInfoMap = m_ConstraintFeatureInfoMap->find(std::make_pair(iIteartorToDependentControlID,uDependentControlCurrentItemID));
				if(iIteratorToConstraintFeatureInfoMap != m_ConstraintFeatureInfoMap->end())
				{
					constraintfeatureoptioninfo =  iIteratorToConstraintFeatureInfoMap->second;
					AddOrRemoveConstraints(ConstraintFeatureNameID,constraintfeatureoptioninfo,iAddOrRemove);
				}
			
			}
			else
			{
				slog.putLog("CDocumentPropertyPage::ProcessDependentConstraints -> NULL_POINTER(pUIControl)\n");
			}
		}
	}
}
	
/*++
Routine Name:

    CDocumentPropertyPage::OnNmClick

Routine Description:

    The routine handles the click event for Factory defaults syslink control

Arguments:

    hDlg - handle to the dialog

Return Value:

   S_OK,
   return value from overridden SetTabDefaults()
   
--*/

HRESULT CDocumentPropertyPage::OnNmClick(_In_ HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance = NULL;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
        (NULL != pPropertyPageInstance))
    {
        hResult = pPropertyPageInstance->SetTabDefaults(hDlg);
		if(S_OK != hResult)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	return hResult;
}

/*++
Routine Name:

    CDocumentPropertyPage::SetTabDefaults

Routine Description:

    The routine calls the SetTabDefaults() function of each control class

Arguments:

    hDlg - handle to the dialog

Return Value:

   S_OK,
   return value from overridden SetTabDefaults()
   
--*/
HRESULT CDocumentPropertyPage::SetTabDefaults(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    HRESULT hr = S_OK;  
    CUIControl* pControl = NULL;
    for (auto iterUIComponents : m_UIControls)
    {
		pControl = iterUIComponents.second ;
		if (NULL != pControl)
        {
        	//<S><A>Fix for SSDI bug 5791 and 5708, Harika-12/6/2022 
			if (pControl->GetControlID() == IDC_CMB_DUPLEX)
			{
				UINT Val = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex;
				hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val, FALSE);
			}
			else if (pControl->GetControlID() == IDC_CMB_NUP)
			{
				UINT Val = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOMSETTINGS].uChangedOptionIndex;
				hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val, FALSE);
			}
			//<S><A> Fix for #5714. K M Harika 16/03/23
			else if (pControl->GetControlID() == IDC_CMB_BINEDGE)
			{
				UINT Val = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex;
				if(m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uDefaultOptionIndex > 0)
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val,TRUE);
				else
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val, FALSE);
			}
			else if (pControl->GetControlID() == IDC_CMB_NUP_ORDER)
			{
				UINT Val = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex;
				if (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uDefaultOptionIndex > 0)
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val,TRUE);
				else
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val, FALSE);
			}
			else if (pControl->GetControlID() == IDC_CMB_PUNCH_POS)
			{
				UINT Val = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex;
				if (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uDefaultOptionIndex > 0)
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val,TRUE);
				else
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val, FALSE);
			}
			else if (pControl->GetControlID() == IDC_CMB_BINEDGE3)
			{
				UINT Val = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex;
				if (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uDefaultOptionIndex > 0)
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val, TRUE);
				else
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val, FALSE);
			}
			//<E> Fix for #5714. K M Harika 16/03/23
			//<S><A>To Fix Redmine #7347,31-05-2024,SSDI:Manoj S
			else if (pControl->GetControlID() == IDC_CMB_OFFSET)
			{
				UINT Val = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
				hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val, FALSE);
			}
			//<E>To Fix Redmine #7347,31-05-2024,SSDI:Manoj S
			//<S><A>To Fix Redmine #7580,21-06-2024,SSDI:Manoj S
			else if (pControl->GetControlID() == IDC_CMB_PSEL_PSRC)
			{
				BYTE Flag = 0;
				ReadStapleLessFlag(Flag);
				if (m_PropertySheetState.byStapleless && Flag)
				{
					UINT Val = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg, Val, FALSE);
				}
				//<S><A>To Fix Redmine #7649,25-06-2024,SSDI:Manoj S
				else
				{
					hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg);
				}
				//<E>To Fix Redmine #7580,25-06-2024,SSDI:Manoj S
			}
			//<E>To Fix Redmine #7580,21-06-2024,SSDI:Manoj S
			else
			{
				hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg);
			}
            if (FAILED(hr))
            //<E>Fix for SSDI bug 5791 and 5708, Harika-12/6/2022 
            {
				if(S_OK != hr)
				{
					slog.LoggingErrorHResultValues();
				}
                break;
            }
			//NotifyConstrainedFeatures(hDlg,pControl,iterUIComponents.first,TRUE);
		
        }
    }
	UpdateMimic();
    return hr;
}

/*++
Routine Name:

    CDocumentPropertyPage::HideCtrl

Routine Description:

    This routine Hides the Control in the UI.

Arguments:

    hDlg - handle to the dialog
	wTargetID - UI Control ID.

Return Value:

  BOOL
   
--*/
BOOL CDocumentPropertyPage::HideCtrl(HWND hDlg,short wTargetID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HWND hWnd = GetDlgItem(hDlg,wTargetID);
	if(hWnd == NULL)
	{
		slog.putLog("CDocumentPropertyPage::HideCtrl -> NULL_HANDLE(hWnd)\n");
		return FALSE;
	}
	::EnableWindow(hWnd, FALSE);
	::ShowWindow(hWnd, SW_HIDE);
	return TRUE;
}
void CDocumentPropertyPage::SetControlState(HWND hDlg,short wCtrlID, BYTE cStatus)
{
	HWND			hWnd = NULL;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	hWnd = GetDlgItem(hDlg,wCtrlID);
	if(hWnd == NULL)
	{
		slog.putLog("CDocumentPropertyPage::SetControlState -> NULL_HANDLE(hWnd)\n");
		return;
	}

	// --- show / hide
	if(cStatus & SCUI_STATE_SHOW)
		ShowWindow(hWnd, SW_SHOW);
	else
		ShowWindow(hWnd, SW_HIDE);

	// --- enable / disable
	if(cStatus & SCUI_STATE_ENABLE)
		EnableWindow(hWnd, TRUE);
	else
		EnableWindow(hWnd, FALSE);
}
//=============================================================================
// function
//      CreateMapElement
//
// return value
//     NILL
//
// outline
//    Creates Map elements.
//=============================================================================
void CDocumentPropertyPage::CreateMapElement(INT iMapKey,INT iMapValue)
{


	std::vector<INT> vectorDependentContolID;
	vectorDependentContolID.push_back(iMapValue);
	m_DependentContolMap[iMapKey] = vectorDependentContolID;
	vectorDependentContolID.clear();
	


}
//=============================================================================
// function
//      CreateMapElement
//
// return value
//     NILL
//
// outline
//    Creates Map elements.
//=============================================================================
void CDocumentPropertyPage::CreateMapElement(INT iMapKey,INT* iDependentControlIDs,DWORD dwCount)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n"); 
	if(nullptr == iDependentControlIDs)
	{
		slog.putLog("CDocumentPropertyPage::CreateMapElement -> NULL_POINTER(iDependentControlIDs)\n"); 
		return ;
	}
	std::vector<INT>DependentContolID;
	for(DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		DependentContolID.push_back(iDependentControlIDs[dwIndex]);
	}
	m_DependentContolMap[iMapKey] = DependentContolID;
	DependentContolID.clear();
}
VOID  CDocumentPropertyPage::SetFeatureControlID(INT iCtrlID)
{
	m_uFeatureControlID = iCtrlID;
}
VOID CDocumentPropertyPage::SetWaterMarkIndex(SHORT sWaterMarkIndex)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n"); 
	if(nullptr != m_pWaterMarkIndex)
	{
		*m_pWaterMarkIndex = sWaterMarkIndex ;
	}
	else
	{
		slog.putLog("CDocumentPropertyPage::SetWaterMarkIndex -> NULL_POINTER(m_pWaterMarkIndex)\n"); 
	}
}

int CDocumentPropertyPage::ShowCustomPaperDialog(int iCtrlID, HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL bIsUIUpdated = FALSE;
	OEMDEV pOemPrivateDevMode = {};
	memcpy_s(&pOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));
	pOemPrivateDevMode.scpi.ext.byCusUnits = (BYTE)m_pOemPrivateDevMode->scpi.ext.byCusUnits;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	CShJsonUserPSize	*pjsonups = NULL;

	// <S><C> To Added Support of Custom paper ,2020.07.24, SSDI:Chanchal Singla
	USERPAPERSIZEDATA upsd;
	SecureZeroMemory(&upsd, sizeof(upsd));
	CUPSReg FAR		*pregUps = NULL;
	pregUps = new CUPSReg(m_hStringResourceHandle, m_pPrinterName);
	if (pregUps == NULL)
	{
		return FALSE;
	}

	if (m_pmcf->IsWriteToJson() == TRUE)
	{
		pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
		pjsonups->Init();
	}


	PRESOURCEIDINFO pResourceIDInfo;
	CCustomPaperDlg FAR *pdlg = NULL;
	if (iCtrlID == IDC_CMB_PAPER)
	{
		pResourceIDInfo = GetResourceIDInfoOfControl(PAPERSIZE);

		pdlg = new CCustomPaperDlg(IDD_DLG_CUSTOMPAPER, m_CustomPaperInfoList, m_hStringResourceHandle, m_hDialogResourceHandle, m_hPrinterHandle, m_pPrinterName, &pOemPrivateDevMode, m_pOemPublicDevMode, m_pFeatureInfoList, m_DefaultSettings, m_pResourceIDInfoMap, m_pFeatureInfoList->pFeatureOptionsList, pResourceIDInfo);
		if (NULL == pdlg)
		{
			slog.putLog("CCustomPaperDlg::OnBnClicked -> NULL_POINTER(pdlg)\n");
			return FALSE;
		}
		//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
		///Custom Paper values are changing to Default even though cancel button is clicked
		//<S><C>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
		//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
		m_ppi->PaperSize_upsd.dwWidth = m_PropertySheetState.dwCustomWidth;
		m_ppi->PaperSize_upsd.dwLength = m_PropertySheetState.dwCustomLength;
		m_ppi->PaperSize_upsd.wUnit = m_PropertySheetState.wCusUnit;
		//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
		memcpy( &upsd, &m_ppi->PaperSize_upsd, sizeof(USERPAPERSIZEDATA));
		//Read Data from registry and update structure.
		//	(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM, &upsd);
		//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla

		// If Accidently Remove the Registry folder then update the UPSD structure with default value 
		//and create the new folder
		if (upsd.dwWidth == 0 || upsd.dwLength == 0)
		{
			if (m_pmcf->IsWriteToJson() == TRUE)
			{
				(*pjsonups).resetEx(DMPAPER_CUSTOM);
				(*pjsonups).ReadUPSData(DMPAPER_CUSTOM, &upsd);
			}
			else
			{
				(*pregUps).resetEx(m_pPrinterName, DMPAPER_CUSTOM);
				(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM, &upsd);
			}
		}
		(*pdlg).SetData(upsd.dwWidth, upsd.dwLength, upsd.wUnit);
		(*pdlg).SetCustomPaperFeatureOption(m_pFeatureInfoList->pFeatureOptionPair);
	}
	
	else if (iCtrlID == IDC_CMB_OUTPUTSIZE)
	{
		pResourceIDInfo = GetResourceIDInfoOfControl(FITTOPAPER);
	
		pdlg = new CCustomPaperDlg(IDD_DLG_CUSTOMPAPER, m_CustomPaperInfoList, m_hStringResourceHandle, m_hDialogResourceHandle, m_hPrinterHandle, m_pPrinterName, &pOemPrivateDevMode, m_pOemPublicDevMode, m_pFeatureInfoList, m_DefaultSettings, m_pResourceIDInfoMap, m_pFeatureInfoList->pFeatureOptionsList, pResourceIDInfo);
		if (NULL == pdlg)
		{
			slog.putLog("CCustomPaperDlg::OnBnClicked -> NULL_POINTER(pdlg)\n");
			return FALSE;
		}	
		//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
		///Custom Paper values are changing to Default even though cancel button is clicked
		memcpy(&upsd, &m_ppi->FitToPaperSize_upsd, sizeof(USERPAPERSIZEDATA));
		//Read Data from registry and update structure.
		//	(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &upsd);
		//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla

		// If Accidently Remove the Registry folder then update the UPSD structure with default value 
		//and create the new folder
		if (upsd.dwWidth == 0 || upsd.dwLength == 0)
		{
			if (m_pmcf->IsWriteToJson() == TRUE)
			{
				(*pjsonups).resetEx(DMPAPER_CUSTOM);
				(*pjsonups).ReadUPSData(DMPAPER_CUSTOM, &upsd);
			}
			else
			{
				(*pregUps).resetEx(m_pPrinterName, DMPAPER_CUSTOM_ZOOM);
				(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &upsd);
			}
		}
		(*pdlg).SetData(upsd.dwWidth, upsd.dwLength, upsd.wUnit);
		(*pdlg).SetCustomPaperFeatureOption(m_pFeatureInfoList->pFeatureOptionPair);
	}


	INT_PTR DlgRet = IDCANCEL;

	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_CUSTOMPAPER), hDlg, (DLGPROC)(*pdlg).WindowMapProc, (LPARAM)pdlg);
	if (IDOK == DlgRet)
	{
		CUIControl pControl;
		
		WCHAR item[MAX_PATH] = {};
		LoadString(m_hStringResourceHandle, IDS_PAPERSIZE_256, item, countof(item));
		UINT iPaperIndex = static_cast<UINT>(SendDlgItemMessage(hDlg, IDC_CMB_OUTPUTSIZE, CB_FINDSTRING, (WPARAM)-1, (LPARAM)item));
		if (iCtrlID == IDC_CMB_PAPER)
		{
			upsd.dwWidth = (DWORD)pdlg->GetWidth();
			upsd.dwLength = (DWORD)pdlg->GetLength();
			upsd.wUnit = (DWORD)pdlg->GetUnit();
			//<S><C>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
			//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
			m_PropertySheetState.dwCustomWidth = upsd.dwWidth;
			m_PropertySheetState.dwCustomLength = upsd.dwLength;
			m_PropertySheetState.wCusUnit = upsd.wUnit;
			//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
			//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
			///Custom Paper values are changing to Default even though cancel button is clicked
			//(*pregUps).WriteUPSData(m_pPrinterName, DMPAPER_CUSTOM, &upsd);
			memcpy(&m_ppi->PaperSize_upsd, &upsd, sizeof(USERPAPERSIZEDATA));
			//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
		}
		else if (iCtrlID == IDC_CMB_OUTPUTSIZE)
		{
			upsd.dwWidth = (DWORD)pdlg->GetWidth();
			upsd.dwLength = (DWORD)pdlg->GetLength();
			upsd.wUnit = (DWORD)pdlg->GetUnit();
			//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
			///Custom Paper values are changing to Default even though cancel button is clicked
			memcpy(&m_ppi->FitToPaperSize_upsd, &upsd, sizeof(USERPAPERSIZEDATA));
			//(*pregUps).WriteUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &upsd);
			//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
		}

		if (FALSE == pControl.IsEqualBuf(m_pOemPrivateDevMode, &pOemPrivateDevMode, sizeof(OEMDEV)))
		{
			memcpy_s(m_pOemPrivateDevMode, sizeof(OEMDEV), &pOemPrivateDevMode, sizeof(OEMDEV));
			
			bIsUIUpdated = TRUE;
		}
		if (TRUE == bIsUIUpdated)
		{
			pControl.EnableApplyButton(hDlg);
		}
	}

	delete pdlg;
	pdlg = NULL;
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
	// <E> To Addeds Support of Custom paper ,2020.07.24, SSDI:Chanchal Singla
	return 1;
}

//<S><A> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
BOOL CDocumentPropertyPage::OnShowHideTab(HWND hWnd, BOOL bShow)
{
	return TRUE;
}
//<S><E> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
//<S><A>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
void CDocumentPropertyPage::UpdatePropState(POEMDEV pPrivateDevmode, PPROPSTATE pPropertySheetState)
{
	// --- Zoom reference point
	if (pPrivateDevmode->scpi.doc.dwFieldDDM & DM_ZOOM_CENTER)
		(*pPropertySheetState).wZoomBasing = zoom_basing_center;
	else
		(*pPropertySheetState).wZoomBasing = zoom_basing_upperleft;
//<S><A>Updating PROPSTATE variable for Other Setttings dialog, 2020.10.08, SSDI:Sneha TG
	// --- byResolution
	m_PropertySheetState.wResolution = pPrivateDevmode->scpi.ext.byResolution;

	// --- byDuplexStyle
	m_PropertySheetState.wDupStyle = pPrivateDevmode->scpi.ext.PCFax.byDuplexStyle;

	// --- DM_WYSIWYG
	m_PropertySheetState.wWysiWyg = pPrivateDevmode->scpi.doc.dwFieldDDM & DM_WYSIWYG ? bool_true : bool_false;

	if (m_PropertySheetState.wWysiWyg == bool_true)			// CnvWysiWygToCmptShade(m_PropertySheetState.wWysiWyg)
		m_PropertySheetState.wCmptShade = cmpt_shade_standard;
	else
		m_PropertySheetState.wCmptShade = cmpt_shade_fine;

	// --- DM_EMFJOURNAL
	m_PropertySheetState.wEMFSpool = pPrivateDevmode->scpi.doc.dwFieldDDM & DM_EMFJOURNAL ? bool_true : bool_false;

	if (m_PropertySheetState.wEMFSpool == bool_true)			// CnvEMFSpoolToCmptSpool(m_PropertySheetState.wEMFSpool);
		m_PropertySheetState.wCmptSpool = cmpt_spool_emf;
	else
		m_PropertySheetState.wCmptSpool = cmpt_spool_raw;

	// --- DM_EDGETOEDGE
	m_PropertySheetState.wEdgeToEdge = pPrivateDevmode->scpi.ext.dwFieldEDM2 & DM_EDGETOEDGE ? bool_true : bool_false;

	if (m_PropertySheetState.wEdgeToEdge == bool_true)			// CnvEdgeToEdgeToCmptPrnArea(m_PropertySheetState.wEdgeToEdge);
		m_PropertySheetState.wCmptPrnArea = cmpt_prnarea_maximum;
	else
		m_PropertySheetState.wCmptPrnArea = cmpt_prnarea_standard;

#if SCDM_VER >= 1918
	// --- DM_SHORT_JOBNAME
	m_PropertySheetState.wShortJobName = pPrivateDevmode->scpi.ext.dwFieldEDM2 & DM_SHORT_JOBNAME ? bool_true : bool_false;
#endif

	// --- DM_JPEG_PASSTHROUGH
	m_PropertySheetState.wJpegPass = pPrivateDevmode->scpi.doc.dwFieldDDM & DM_JPEG_PASSTHROUGH ? bool_true : bool_false;

	// --- wSetLineToOne
	m_PropertySheetState.wSetLineToOne = pPrivateDevmode->scpi.doc.wPenWidthRate == 0 ? bool_true : bool_false;

//<E>Updating PROPSTATE variable for Other Setttings dialog, 2020.10.08, SSDI:Sneha TG
		// --- byMgnShift
	m_PropertySheetState.wMargin = pPrivateDevmode->scpi.ext.byMgnShift;

	// --- DM_PRINTPOSITION_CHG
	m_PropertySheetState.wPPosChg = pPrivateDevmode->scpi.ext.wFieldEDM & DM_PRINTPOSITION_CHG ? bool_true : bool_false;
}
void CDocumentPropertyPage::UpdatePropstateToStructure()
{
	// --- DM_ZOOM_CENTER
	m_pOemPrivateDevMode->scpi.doc.dwFieldDDM = (m_PropertySheetState.wZoomBasing == bool_true) ?
		(m_pOemPrivateDevMode->scpi.doc.dwFieldDDM | DM_ZOOM_CENTER) : (m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & ~DM_ZOOM_CENTER);
//<S><A>Updating PROPSTATE to devmode for Other Setttings dialog, 2020.10.08, SSDI:Sneha TG
	// --- Input Resolution
	m_pOemPrivateDevMode->scpi.ext.byResolution = m_PropertySheetState.wResolution;

	// --- DM_WYSIWYG
	m_pOemPrivateDevMode->scpi.doc.dwFieldDDM = (m_PropertySheetState.wWysiWyg == bool_true) ?
		(m_pOemPrivateDevMode->scpi.doc.dwFieldDDM | DM_WYSIWYG) : (m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & ~DM_WYSIWYG);

	// --- DM_EMFJOURNAL
	m_pOemPrivateDevMode->scpi.doc.dwFieldDDM = (m_PropertySheetState.wEMFSpool == bool_true) ?
		(m_pOemPrivateDevMode->scpi.doc.dwFieldDDM | DM_EMFJOURNAL) : (m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & ~DM_EMFJOURNAL);

	// --- DM_EDGETOEDGE
	m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 = (m_PropertySheetState.wEdgeToEdge == bool_true) ?
		(m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 | DM_EDGETOEDGE) : (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & ~DM_EDGETOEDGE);

	// --- Duplex Style
	m_pOemPrivateDevMode->scpi.ext.PCFax.byDuplexStyle = m_PropertySheetState.wDupStyle ;

#if SCDM_VER >= 1918
	// --- DM_SHORT_JOBNAME
	m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 = (m_PropertySheetState.wShortJobName == bool_true) ?
		(m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 | DM_SHORT_JOBNAME) : (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & ~DM_SHORT_JOBNAME);
#endif

	// --- DM_JPEG_PASSTHROUGH
	m_pOemPrivateDevMode->scpi.doc.dwFieldDDM = (m_PropertySheetState.wJpegPass == bool_true) ?
		(m_pOemPrivateDevMode->scpi.doc.dwFieldDDM | DM_JPEG_PASSTHROUGH) : (m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & ~DM_JPEG_PASSTHROUGH);

	// --- wPenWidthRate
	if (m_PropertySheetState.wSetLineToOne == bool_true)
		m_pOemPrivateDevMode->scpi.doc.wPenWidthRate = bool_false;
	else
		m_pOemPrivateDevMode->scpi.doc.wPenWidthRate = WPENLINEWIDTH;

//<E>Updating PROPSTATE to devmode for Other Setttings dialog, 2020.10.08, SSDI:Sneha TG
}
//<E>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG

// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
//=============================================================================
// function
//       DevDmodetopropstate
//
// parameters
//                 
//
// return value :
//					Void
//
// outline
//       DevDmodetopropstate : converts DevDmode value to propstate 
//=============================================================================
void CDocumentPropertyPage::DevDmodetopropstate()
{
	//Automatic color mode
	m_PropertySheetState.byAutoColorMode = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_VDM_COLORMODE_AUTO_DISABLE ? bool_true : bool_false;
	//PinCompatibility
	m_PropertySheetState.byPinCompatibility = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_VDM_PIN_CLEAN_TEXT ? bool_true : bool_false;
	// --- DM_SINGLESIGNON
	m_PropertySheetState.wSingleSignOn = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_SINGLESIGNON ? bool_true : bool_false;
	//Maxpapersize
	//m_PropertySheetState.byMaxPaperSize = m_pOemPrivateDevMode->scpi.dev.byMaxPaperSize;
	 m_PropertySheetState.byMaxPaperSize = (m_pOemPrivateDevMode->scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
	//ByPrintRelease
	m_PropertySheetState.byPrintRelease = m_pOemPrivateDevMode->scpi.dev.byFieldVDM & DM_VDM_NON_PRITNRLEASE ? bool_true : bool_false;

	//Color Mode
	if (m_pOemPrivateDevMode->scpi.dev.byFieldVDM & DM_VDM_COLORMODEL)
	{
		m_PropertySheetState.wColorOption = 0; //color
	}
	else
	{
		m_PropertySheetState.wColorOption = 1; //black and white
	}

	//ByPunchmodule
	//m_PropertySheetState.wPunchOption = m_pOemPrivateDevMode->scpi.dev.byPunchmodule;
	m_PropertySheetState.wPunchOption = UpdatePunchTypeToPropstate();

	//ByStaplemodule
	if (m_pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_SADDLE)
	{
		m_PropertySheetState.wStapleOption = stapleopt_3;
	}
	else if (m_pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_2STAPLE)
	{
		m_PropertySheetState.wStapleOption = stapleopt_2;
	}
	else if (m_pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_1STAPLE)
	{
		m_PropertySheetState.wStapleOption = stapleopt_1;
	}
	else
	{
		m_PropertySheetState.wStapleOption = stapleopt_none;
	}

	//ByStapleless
	m_PropertySheetState.byStapleless = m_pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_STAPLELESS ? bool_true : bool_false;
	//PPPrintRelease
	m_PropertySheetState.wPPlcyPrintRelease = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_PRINTRELEASE ? bool_true : bool_false;
	if (m_PropertySheetState.wPPlcyPrintRelease == bool_true)
		m_PropertySheetState.wPrintRelease = bool_true;
	//else
		//<S><D>To Fix Redmine #7584,03-07-2024,SSDI:Manoj S
		//m_PropertySheetState.wPrintRelease = bool_false;
		//<E>To Fix Redmine #7584,03-07-2024,SSDI:Manoj S
	//PPDocumentFiling
	m_PropertySheetState.wPPlcyRetention = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_RETENTION ? bool_true : bool_false;
	//PPUserAuthentication
	m_PropertySheetState.wPPlcyUserAuth = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_USERAUTH ? bool_true : bool_false;
	//PPWindowsLoginNameasUserName
	m_PropertySheetState.wPPlcyWinLogin = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN ? bool_true : bool_false;
	//PPLgoinNameasUserName
	m_PropertySheetState.wPPlcyLoginName = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_LOGINN_AS_USERN ? bool_true : bool_false;

	//PUNCH
	m_PropertySheetState.wPunch = m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_PUNCH ? bool_true : bool_false;
}
//<S><C>To Update the Propstate based on devmode,20201412,SSDi:Chanchal Singla
//=============================================================================
// Function :
//			UpdatePunchTypeToPropstate
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
SHORT CDocumentPropertyPage::UpdatePunchTypeToPropstate()
{
	SHORT punch = punchtype_none;
	switch (m_pOemPrivateDevMode->scpi.dev.byPunchmodule)
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
//<E>To Update the Propstate based on devmode,20201412,SSDi:Chanchal Singla

// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
//<S><A> To support maximum paper size, 2020.10.09, SSDI: Chanchal Singla
//=============================================================================
// function
//      ModelLimit
//
// parameters
//			HWND		hDlg
//			INT			iCtrlID
//			UINT		FeatureIndex
// return value
//       TRUE
//		 FALSE
//
// outline
//      ModeLimit: Based On Some Constraints  Feature option limit has been set.
//=============================================================================
BOOL CDocumentPropertyPage::ModeLimit(HWND hDlg, INT iCtrlID, UINT FeatureIndex)
{
	BOOL bRet = FALSE;
	UINT uChangedOptionIndex = 0;
	LONG lVal = m_pFeatureInfoList->pFeatureOptionsList[FeatureIndex].uChangedOptionIndex;
	CShLimit*    limit = new CShLimit(&m_pFeatureInfoList->pFeatureOptionPair[FeatureIndex],
		&m_pFeatureInfoList->pFeatureOptionsList[FeatureIndex]);
	switch (iCtrlID)
	{
	case IDC_CMB_OUTPUTSIZE:
	{
		if (m_PropertySheetState.byMaxPaperSize == eSecondOption)
		{
			if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
			{
				bRet = limit->limCmbBookletStdPSize(&m_PropertySheetState, lVal);
				if (bRet != FALSE)
				{
					uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? BOOKLET_A4_INDEX : BOOKLET_LETTER_INDEX;
					m_pFeatureInfoList->pFeatureOptionsList[FeatureIndex].uChangedOptionIndex = uChangedOptionIndex;
					bRet = TRUE;
				}
			}
			else
			{
				bRet = limit->limCmbFitToPaperSize(&m_PropertySheetState, lVal, m_pFeatureInfoList);
				if (bRet != FALSE)
				{
					uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? OUTPUTPAPERSIZE_A4 : OUTPUTPAPERSIZE_LETTER;
					m_pFeatureInfoList->pFeatureOptionsList[FeatureIndex].uChangedOptionIndex = uChangedOptionIndex;
					bRet = TRUE;
				}
			}
		}
	}
	break;
	//<S><A> To Fix Redmine #2810,2020-10-14,SSDI:Chanchal Singla
	//PJL displays Staple: ON even though Staple is set to None in Configuration tab
	case IDC_CMB_STAPLE:
		if (m_PropertySheetState.wStapleOption == stapleopt_none)
		{
			if (lVal != stapleopt_none)
			{
				m_pFeatureInfoList->pFeatureOptionsList[FeatureIndex].uChangedOptionIndex = stapleopt_none;
			}
			bRet = TRUE;
		}
		break;
		//<E> To Fix Redmine #2810,2020-10-14,SSDI:Chanchal Singla
	//<S><A> To Fix #2848,2020-16-10,SSDI;Chanchal Singla
	//Maximum Paper Size: Same as original Size is displayed in Output Size drop down when Original Size is set to A3
	case IDC_CMB_PAPER:
		if ((m_PropertySheetState.byMaxPaperSize == eSecondOption)
			&& !strcmp("SAOS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption))
		{
			if (!strcmp("A3", m_pFeatureInfoList->pFeatureOptionPair[FeatureIndex].pszOption)
				|| !strcmp("B4", m_pFeatureInfoList->pFeatureOptionPair[FeatureIndex].pszOption)
				|| !strcmp("TABLOID", m_pFeatureInfoList->pFeatureOptionPair[FeatureIndex].pszOption)
				|| !strcmp("JENV_KAKU2", m_pFeatureInfoList->pFeatureOptionPair[FeatureIndex].pszOption))
			{
				uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? OUTPUTPAPERSIZE_A4 : OUTPUTPAPERSIZE_LETTER;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex = uChangedOptionIndex;
			}
		}
		break;
	//<E> To Fix #2848,2020-16-10,SSDI;Chanchal Singla
	//<S><A>To implement constraints with PPlcy Windows login and JobHandlingUA, Harika, 20201021
	case IDC_CMB_AUTHENTICATION:
		if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
		{
			bRet = limit->limPPlcyAuthVsAuthentication(&m_PropertySheetState, lVal);
			if (bRet != FALSE)
			{
				uChangedOptionIndex = authentication_lname;
				m_pFeatureInfoList->pFeatureOptionsList[FeatureIndex].uChangedOptionIndex = uChangedOptionIndex;
				bRet = TRUE;
			}
		}
		break;
		//<E>To implement constraints with PPlcy Windows login and JobHandlingUA, Harika, 20201021
	default:
		break;
	
	}
	delete	limit;
	limit = NULL;
	return bRet;
}
//=============================================================================
// function
//      GetValue
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
//
// return value
//      Unsigned int
//
// outline
//      get the value from drop down feature.
//=============================================================================

long CDocumentPropertyPage::GetValue(HWND hDlg, WORD wCtrlID)
{
	UIControlMap::const_iterator Iterator;
	long		Value = 0;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	Iterator = m_UIControls.find(wCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			Value = pImageCombo->GetVal(wCtrlID, hDlg);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::Select -> NULL_OBJECT(pImageCombo)\n");
		}

	}
	return Value;
}
//=============================================================================
// function
//      GetValue
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
//
// return value
//      Unsigned int
//
// outline
//      get the value from drop down feature.
//=============================================================================

long CDocumentPropertyPage::SetCheckBoxValue(HWND hDlg, WORD wCtrlID,UINT wCheck)
{
	UIControlMap::const_iterator Iterator;
	long		Value = 0;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	Iterator = m_UIControls.find(wCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CCheckBox *pCheckBox = static_cast<CCheckBox*>(Iterator->second);
		if (pCheckBox)
		{
			Value = pCheckBox->SetCheckBoxValue(hDlg, wCtrlID, wCheck);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::SetCheckBoxValue -> NULL_OBJECT(CCheckBox)\n");
		}

	}
	return Value;
}
//=============================================================================
// function
//      InitializeFeatureOption
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
//		LONG
// return value
//      Void
//
// outline
//      InitializeFeatureOption.
//=============================================================================


VOID CDocumentPropertyPage::InitializeFeatureOption(HWND hDlg, WORD wCtrlID,LONG lVal)
{
	UIControlMap::const_iterator Iterator;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	Iterator = m_UIControls.find(wCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			pImageCombo->OnInit(wCtrlID, hDlg);
			pImageCombo->Select(wCtrlID, hDlg, lVal);
			pImageCombo->SelectFeatureOption(wCtrlID, hDlg);
		}
		else
		{
			slog.putLog("CMainTabPropertyPage::Select -> NULL_OBJECT(pImageCombo)\n");
		}

	}
}
//<E> To support maximum paper size, 2020.10.09, SSDI: Chanchal Singla
//=============================================================================
// function
//      SetJobControlEdit
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
//		LPWSTR			newVal
//		BOOL			blForceMB
// return value
//      Void
//
// outline
//      SetJobControlEdit.
//=============================================================================
VOID CDocumentPropertyPage::SetJobControlEdit(HWND hDlg, WORD wCtrlID, LPWSTR newVal,BOOL blForceMB)
{
	UIControlMap::const_iterator Iterator;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	Iterator = m_UIControls.find(wCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CJobControlEdit *pJobControlEdit = static_cast<CJobControlEdit*>(Iterator->second);
		if (pJobControlEdit)
		{
			pJobControlEdit->SetValue(wCtrlID, hDlg, newVal, blForceMB);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::SetJobControlEdit -> NULL_OBJECT(CJobControlEdit)\n");
		}
	}
}
//<S><A>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
//Known Issue : Login Name is not concealed on UI when Printing Policy - Use Windows Login Name as ëLogin Nameí is checked.
//=============================================================================
// function
//      SetEditLName
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
//		LPWSTR			newVal
//		BOOL			blForceMB
// return value
//      Void
//
// outline
//      SetEditLName.
//=============================================================================
VOID CDocumentPropertyPage::SetEditLName(HWND hDlg, WORD wCtrlID, LPWSTR newVal, BOOL blForceMB)
{
	UIControlMap::const_iterator Iterator;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	Iterator = m_UIControls.find(wCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CShEditLName *pEditLName = static_cast<CShEditLName*>(Iterator->second);
		if (pEditLName)
		{
			pEditLName->SetValueLoginName(hDlg, newVal);	
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::CShEditLName -> NULL_OBJECT(CShEditLName)\n");
		}

	}
}
//<E>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
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
BOOL CDocumentPropertyPage::SetRegJCInfoToPropState(PSCDM pscdm, PPROPSTATE pps, WCHAR FAR *pszSvrPrnName)
{
	BOOL			blRet = FALSE;

	PREGJCINFO		pregjcinfo = NULL;

	DWORD			dwSize = 0;
	CShRegJC *pregjc = new CShRegJC(m_hStringResourceHandle, pszSvrPrnName);
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
		(*pregjcinfo).wSingleSignOn = (*pps).wLoginNameSSO;
		(*pregjcinfo).wFolderIndex = (*pps).wDocFileFolder;
		if ((*pregjc).ReadJCData(pszSvrPrnName, pregjcinfo, (*pps).wPPlcyWinLogin) != FALSE)
			RegJCInfoToPropState(pregjcinfo, pps, pszSvrPrnName);


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
BOOL CDocumentPropertyPage::RegJCInfoToPropState(PREGJCINFO prji, PPROPSTATE pps, WCHAR FAR *pszSvrPrnName)
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
	(*pps).wLoginNameSSO = (*prji).wSingleSignOn;
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
	if ((*prji).wSingleSignOn == bool_true)
		wcscpy_s((*pps).szSingleSignOnName, _countof((*pps).szSingleSignOnName), (*prji).szSingleSignOnName);
	if ((*prji).wLoginPass == bool_true)
		::lstrcpy((*pps).szLoginPasswordUA, (*prji).szLoginPassword);
	if ((*prji).wUsePIN == bool_true)
		::lstrcpy((*pps).szRetentionPassword, (*prji).szPIN);
	if ((*prji).wFolderIndex != 0)
		::lstrcpy((*pps).szFolderPass, (*prji).szFolderPass);

	blRet = TRUE;

EXIT:

	return blRet;
}
//=============================================================================
// function
//       PropStateToRegJCInfo
//
// parameters
//      pps                 PROPSTATEç
//      prji                REGJCINFOç
//      pszPrnName        
// return value
//      êTRUE
//      FALSE
//
// outline
//       PROPSTATEÇREGJCINFOÇ
//=============================================================================
BOOL CDocumentPropertyPage::PropStateToRegJCInfo(PPROPSTATE pps, PREGJCINFO prji, WCHAR FAR *pszSvrPrnName)
{
	BOOL			blRet = FALSE;


	if (prji == NULL || pps == NULL || pszSvrPrnName == NULL)
		goto EXIT;
	
	(*prji).wUsePIN = (*pps).wUsePIN;
	(*prji).wUseAcNum = (*pps).wUseAcNum;
	(*prji).wUseUName = (*pps).wUseUName;
	(*prji).wUseJName = (*pps).wUseJName;
	(*prji).wLoginName = (*pps).wLoginName;
	(*prji).wLoginPass = (*pps).wLoginPass;
	(*prji).wSingleSignOn = (*pps).wLoginNameSSO;
	(*prji).wFolderIndex = (*pps).wDocFileFolder;


	SecureZeroMemory((*prji).szUserName, sizeof((*prji).szUserName));
	SecureZeroMemory((*prji).szJobName, sizeof((*prji).szJobName));
	SecureZeroMemory((*prji).szAccountNum, sizeof((*prji).szAccountNum));
	SecureZeroMemory((*prji).szLoginName, sizeof((*prji).szLoginName));
	SecureZeroMemory((*prji).szLoginPassword, sizeof((*prji).szLoginPassword));
	SecureZeroMemory((*prji).szSingleSignOnName, sizeof((*prji).szSingleSignOnName));
	SecureZeroMemory((*prji).szPIN, sizeof((*prji).szPIN));
	SecureZeroMemory((*prji).szFolderPass, sizeof((*prji).szFolderPass));

	if ((*prji).wUseUName == bool_true)
		wcscpy_s((*prji).szUserName, _countof((*prji).szUserName), (*pps).szUserName);
	if ((*prji).wUseJName == bool_true)
		wcscpy_s((*prji).szJobName, _countof((*prji).szJobName), (*pps).szJobName);
	if ((*prji).wUseAcNum == bool_true)
		wcscpy_s((*prji).szAccountNum, _countof((*prji).szAccountNum), (*pps).szUserNumber);
	if ((*prji).wLoginName == bool_true)
		wcscpy_s((*prji).szLoginName, _countof((*prji).szLoginName), (*pps).szLoginNameUA);
	if ((*prji).wLoginPass == bool_true)
		::lstrcpy((*prji).szLoginPassword, (*pps).szLoginPasswordUA);
	if ((*prji).wSingleSignOn == bool_true)
		::lstrcpy((*prji).szSingleSignOnName, (*pps).szSingleSignOnName);
	if ((*prji).wUsePIN == bool_true)
		::lstrcpy((*prji).szPIN, (*pps).szRetentionPassword);
	if ((*prji).wFolderIndex != 0)
		::lstrcpy((*prji).szFolderPass, (*pps).szFolderPass);


	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      GetRetentionPropValue
//
// return value
//     SHORT
//
// outline
//      Get Retention Value. 
//=============================================================================
SHORT CDocumentPropertyPage::GetRetentionPropValue()
{
	SHORT wRetentionMode = eFirstOption;
	switch (m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetention)
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
	return wRetentionMode;
}
//=============================================================================
// function
//      GetDocFillingPropValue
//
// return value
//     SHORT
//
// outline
//      Get DocFilling Value. 
//=============================================================================
SHORT CDocumentPropertyPage::GetDocFillingPropValue()
{
	SHORT wDocFileMode = eFirstOption;
	switch (m_pOemPrivateDevMode->scpi.ext.JobCtrl.byDocumentFiling)
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
	return wDocFileMode;
}

//<S><A>To the support of Display Information messages, 2020-15-02, SSDI: Chanchal Singla
//=============================================================================
// function
//      limTabCtrlPosition
//
// parameters
//      pps                 PROPSTATEç
//
// return value
//      LONG
//
// outline
//		Staple / Punch / Margin Shift - Orientation / 2-Sided Printing / N-UpÇù
//=============================================================================
long CDocumentPropertyPage::limTabCtrlPosition(HWND hDlg,PFEATUREINFOLIST pFeatureInfoList)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	long	lRet = 0;

	SHORT wStaple = (short)m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	SHORT wPunch = (short)m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex;
	SHORT wMargin = (short)m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex;
	SHORT wBindedge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;

	if (wStaple != staple_none) {
		lRet = IDS_MESSAGE_209;
	}
	else if (wPunch != punch_none) {
		lRet = IDS_MESSAGE_210;
	}
	else if (wMargin != 0) {
		lRet = IDS_MESSAGE_283;
	}
	if (lRet != 0) {
		// Orientation / 2-Sided Printing / N-Up 
		if (limDuplexVsBinEdge(hDlg, wBindedge) == FALSE) {
			lRet = 0;
		}
	}
	return lRet;
}
BOOL CDocumentPropertyPage::limDuplexVsBinEdge(HWND hDlg, short wBindedge)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL			blRet = FALSE;
	BOOL			blFlg = FALSE;
	short			wOrient = ori_port;				// orientation
	long			lMsgID = 0;
	SHORT			Orientval = 0;
	SHORT			wOri = ori_port;

	SHORT			wDuplex = GetValue(hDlg, IDC_CMB_DUPLEX);
	SHORT			NupValue = GetValue(hDlg, IDC_CMB_NUP);

	UIControlMap::const_iterator Iterator;
	Iterator = m_UIControls.find(IDC_CMB_ORIENT);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			Orientval = (short)pImageCombo->GetVal(IDC_CMB_ORIENT, hDlg);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::limDuplexVsBinEdge -> NULL_OBJECT(pImageCombo)\n");
		}
	}
	else
	{
		Orientval = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex;
	}


	if ((eFirstOption == Orientval) || (eThirdOption == Orientval)) {
		wOrient = ori_port;
	}
	else {
		wOrient = ori_land;
	}
	if (wDuplex == 0 &&
		!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		switch (NupValue)
		{
		case eFirstOption://nup_1:
		case eThirdOption://nup_4:
		case eSixthOption://nup_9:
		case eSevenOption://nup_16:
			wDuplex = duplex_long_edge;
			break;
		default:
			wDuplex = duplex_short_edge;
			break;

		}
		// Landscape
		if (wOrient == ori_land)
		{
			if (wDuplex == duplex_long_edge)
			{
				wDuplex = duplex_short_edge;
			}
			else
			{
				wDuplex = duplex_long_edge;
			}
		}
	}
	else if (wDuplex == duplex_long_edge || wDuplex == duplex_short_edge)
	{

		switch (NupValue)
		{
		case eFirstOption://nup_1:
		case eThirdOption://nup_4:
		case eSixthOption://nup_9:
		case eSevenOption://nup_16:
			break;
		default:
			wOri = ori_land;
			break;

		}
		if (wDuplex == duplex_long_edge) {
			// 2-Sided Printing(Long Edge)
			if (wOrient == ori_land) {
				if (wOri == ori_port) {
					wDuplex = duplex_short_edge;
				}
			}
			else {
				if (wOri == ori_land) {
					wDuplex = duplex_short_edge;
				}
			}
		}
		else {
			// 2-Sided Printing(Short Edge)
			if (wOrient == ori_land) {
				if (wOri == ori_port) {
					wDuplex = duplex_long_edge;
				}
			}
			else {
				if (wOri == ori_land) {
					wDuplex = duplex_long_edge;
				}
			}
		}
	}
	if ((wDuplex == duplex_short_edge) &&
		((wBindedge == binedge_left) || (wBindedge == binedge_right)))
	{
		blRet = TRUE;
	}
	if ((wDuplex == duplex_long_edge) &&
		(wBindedge == binedge_top))
	{
		blRet = TRUE;
	}
	return blRet;
}
BOOL CDocumentPropertyPage::DisplayPopUpInformationMessage(HWND hDlg, DWORD wControlID)
{
	BOOL lRet = FALSE;
	UINT uChangedOptionIndex = 0;
	switch (wControlID)
	{
	case IDC_CMB_ORIENT:
	{
		if (TRUE == DisplayOrientationInformationMessage(hDlg))
		{
			uChangedOptionIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex;
			lRet = TRUE;
		}
	}
	break;
	case IDC_CMB_DUPLEX:
	{
		if (TRUE == DisplayDuplexInformationMessage(hDlg))
		{
			uChangedOptionIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
			lRet = TRUE;
		}
	}
	break;
	case IDC_CMB_NUP:
	{
		if (TRUE == DisplayNUPInformationMessage(hDlg))
		{
			uChangedOptionIndex =  m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex;
			lRet = TRUE;
		}
	}
	break;
	case IDC_CMB_STAPLE:
	{
		if (TRUE == DisplayStapleInformationMessage(hDlg))
		{
			uChangedOptionIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
			lRet = TRUE;
		}
	}
	break;
	case IDC_CMB_PSEL_PTYPE:
	{
		if (TRUE == DisplayPaperTypeInformationMessage(hDlg, IDC_CMB_PSEL_PTYPE))
		{
			uChangedOptionIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex;
			lRet = TRUE;
		}
	}
	break;
	case IDC_CMB_OUTPUTSIZE:
	case IDC_CMB_PAPER:
	{
		if (DisplayOutputandPaperSizeInformationMessage(hDlg, wControlID) == TRUE)
		{
			lRet = FALSE;
		}
	}
		break;
	//<S><A>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
	case IDC_CMB_PSEL_PSRC:
	{
		if (TRUE == DisplayPaperTrayVsStapleInformationMessage(hDlg))
		{
			lRet = TRUE;
		}
	}
		break;
	//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	default:
		lRet = FALSE;
		break;
	}
	if (lRet == TRUE)
	{
		SendDlgItemMessage(hDlg, wControlID, CB_SETCURSEL, uChangedOptionIndex, 0);
	}
	return lRet;
}
//<S><C> Modified The Code to Fix Bug #3533,2021.07.28,SSDI:Chanchal Singla
//Information icon is not displayed when Stapleless and Paper size is set
//<S><A>To the support of Display Information messages, 2020-15-02, SSDI: Chanchal Singla
//<S><C>Modified the conditions to fix Bug #4012,2021.August.03,SSDI:Chanchal Singla
//Constraints Missing for combinations of Output Size, Staple / Punch
//=============================================================================
// function
//       ShowConstraintsOnInformationIcon
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints On Information Icon
//=============================================================================
VOID CDocumentPropertyPage::ShowStapleVsOriginalSizeVsOutputSizeInformationIcon(HWND hDlg, INT iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap::const_iterator Iterator;
	Iterator = m_UIControls.find(IDC_CMB_OUTPUTSIZE);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			m_PropertySheetState.wOutputSize = (short)pImageCombo->GetVal(IDC_CMB_OUTPUTSIZE, hDlg);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::ShowStapleVsOriginalSizeVsOutputSizeInformationIcon -> NULL_OBJECT(pImageCombo)\n");
		}
	}
	else
	{
		m_PropertySheetState.wOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	}
	m_PropertySheetState.wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	m_PropertySheetState.wPaper = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	m_PropertySheetState.wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	m_PropertySheetState.wOreint = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex;
	long wBindedge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	m_ControlState.cPaper_I = SCUI_STATE_OFF;
	m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
	m_ControlState.cStaple_I = SCUI_STATE_OFF;
	//<S><C> Fix for feedback issue #46885. Get Orientation based on NUP and Orientation features. Harika, 20220923
	short Orient = GetOrientation(m_pFeatureInfoList);
	//<E> Fix for feedback issue #46885. Get Orientation based on NUP and Orientation features. Harika, 20220923
	if ((m_PropertySheetState.wStaple == staple_none)
		|| (wBooklet != booklet_none))
	{
		goto EXIT;
	}
	if (m_PropertySheetState.wStaple == staple_1)
	{
		if ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A5) || m_PropertySheetState.wOutputSize >= OUTPUTPAPERSIZE_ENV_DL
			&& m_PropertySheetState.wOutputSize <= OUTPUTPAPERSIZE_CUSTOMSIZE)
		{
			m_ControlState.cOutputSize_I = SCUI_STATE_ON;
			m_ControlState.cStaple_I = SCUI_STATE_ON;
		}
		else if (((m_PropertySheetState.wPaper == PAPERSIZE_A5) || m_PropertySheetState.wPaper >= PAPERSIZE_ENV_DL
			&& m_PropertySheetState.wPaper <= PAPERSIZE_CUSTOMSIZE)
			&& (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_SAOS))
		{
			m_ControlState.cPaper_I = SCUI_STATE_ON;
			m_ControlState.cStaple_I = SCUI_STATE_ON;
		}
	}
	else if (m_PropertySheetState.wStaple == staple_2)
	{
		if ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A3 || m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A5
			|| m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B4 || m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_TABLOID
			|| m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LEGAL) || m_PropertySheetState.wOutputSize >= OUTPUTPAPERSIZE_EXECUTIVE
			&& m_PropertySheetState.wOutputSize <= OUTPUTPAPERSIZE_CUSTOMSIZE)
		{
			if (Orient==ori_port/*((m_PropertySheetState.wOreint == eFirstOption) || (m_PropertySheetState.wOreint == eThirdOption))*///Portrait
				&& ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A3)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B4)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LEGAL)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_TABLOID)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_EXECUTIVE)) && (wBindedge == binedge_top))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else if (Orient == ori_land/*((m_PropertySheetState.wOreint == eSecondOption) || (m_PropertySheetState.wOreint == eFourthOption))*///landscape
				&& ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A3)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B4)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LEGAL)
					||(m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_TABLOID)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_EXECUTIVE))
				&& ((wBindedge == binedge_left) || (wBindedge == binedge_right)))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_ON;
				m_ControlState.cStaple_I = SCUI_STATE_ON;
			}
		}
		else if (((m_PropertySheetState.wPaper == PAPERSIZE_A3 || m_PropertySheetState.wPaper == PAPERSIZE_A5
			|| m_PropertySheetState.wPaper == PAPERSIZE_B4 || m_PropertySheetState.wPaper == PAPERSIZE_TABLOID
			|| m_PropertySheetState.wPaper == PAPERSIZE_LEGAL) || m_PropertySheetState.wPaper >= PAPERSIZE_EXECUTIVE
			&& m_PropertySheetState.wPaper <= PAPERSIZE_CUSTOMSIZE)
			&& (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_SAOS))
		{
			if (Orient == ori_port/*((m_PropertySheetState.wOreint == eFirstOption) || (m_PropertySheetState.wOreint == eThirdOption))*///Portrait
				&& ((m_PropertySheetState.wPaper == PAPERSIZE_A3)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_B4)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_LEGAL)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_TABLOID)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_EXECUTIVE)) && (wBindedge == binedge_top))
			{
				m_ControlState.cPaper_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else if (Orient == ori_land/*((m_PropertySheetState.wOreint == eSecondOption) || (m_PropertySheetState.wOreint == eFourthOption))*///landscape
				&& ((m_PropertySheetState.wPaper == PAPERSIZE_A3)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_B4)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_LEGAL)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_TABLOID)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_EXECUTIVE))
				&& ((wBindedge == binedge_left) || (wBindedge == binedge_right)))
			{
				m_ControlState.cPaper_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else
			{
				m_ControlState.cPaper_I = SCUI_STATE_ON;
				m_ControlState.cStaple_I = SCUI_STATE_ON;
			}
		}
	}
	else if (m_PropertySheetState.wStaple == staple_1_stapleless)
	{
		if ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A3 || m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A5
			|| m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B4 || m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_TABLOID
			|| m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LEGAL || m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LETTER
			|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A4) || (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B5))
			|| m_PropertySheetState.wOutputSize >= OUTPUTPAPERSIZE_EXECUTIVE
			&& m_PropertySheetState.wOutputSize <= OUTPUTPAPERSIZE_CUSTOMSIZE)
		{
			if (Orient == ori_port/*((m_PropertySheetState.wOreint == eFirstOption) || (m_PropertySheetState.wOreint == eThirdOption))*///Portrait
				&& ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A3)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B4)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_TABLOID))
				&& ((wBindedge == binedge_right) || (wBindedge == binedge_top)))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else if (Orient == ori_land/*((m_PropertySheetState.wOreint == eSecondOption) || (m_PropertySheetState.wOreint == eFourthOption))*///landscape
				&& ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A3)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B4)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_TABLOID))
				&& ((wBindedge == binedge_left) || (wBindedge == binedge_top)))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else if (Orient == ori_port/*((m_PropertySheetState.wOreint == eFirstOption) || (m_PropertySheetState.wOreint == eThirdOption))*///Portrait
				&& ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A4)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B5)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LETTER))
				&& ((wBindedge == binedge_left) || (wBindedge == binedge_top)))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else if (Orient == ori_land/*((m_PropertySheetState.wOreint == eSecondOption) || (m_PropertySheetState.wOreint == eFourthOption))*///landscape
				&& ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A4)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B5)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LETTER))
				&& ((wBindedge == binedge_right) || (wBindedge == binedge_top)))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_ON;
				m_ControlState.cStaple_I = SCUI_STATE_ON;
			}
		}
		else if (((m_PropertySheetState.wPaper == PAPERSIZE_A3 || m_PropertySheetState.wPaper == PAPERSIZE_A5
			|| m_PropertySheetState.wPaper == PAPERSIZE_B4 || m_PropertySheetState.wPaper == PAPERSIZE_TABLOID
			|| m_PropertySheetState.wPaper == PAPERSIZE_LEGAL || m_PropertySheetState.wPaper == PAPERSIZE_LETTER
			|| m_PropertySheetState.wPaper == PAPERSIZE_A4 || (m_PropertySheetState.wPaper == PAPERSIZE_B5))
			|| m_PropertySheetState.wPaper >= PAPERSIZE_EXECUTIVE
			&& m_PropertySheetState.wPaper <= PAPERSIZE_CUSTOMSIZE)
			&& (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_SAOS))
		{
			if (Orient==ori_port/*((m_PropertySheetState.wOreint == eFirstOption) || (m_PropertySheetState.wOreint == eThirdOption))*///Portrait
				&& ((m_PropertySheetState.wPaper == PAPERSIZE_A3)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_B4)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_TABLOID))
				&& ((wBindedge == binedge_right) || (wBindedge == binedge_top)))
			{
				m_ControlState.cPaper_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else if (Orient==ori_land/*((m_PropertySheetState.wOreint == eSecondOption) || (m_PropertySheetState.wOreint == eFourthOption))*///landscape
				&& ((m_PropertySheetState.wPaper == PAPERSIZE_A3)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_B4)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_TABLOID))
				&& ((wBindedge == binedge_left) || (wBindedge == binedge_top)))
			{
				m_ControlState.cPaper_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else if (Orient == ori_port/*((m_PropertySheetState.wOreint == eFirstOption) || (m_PropertySheetState.wOreint == eThirdOption))*///Portrait
				&& ((m_PropertySheetState.wPaper == PAPERSIZE_A4)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_B5)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_LETTER))
				&& ((wBindedge == binedge_left) || (wBindedge == binedge_top)))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else if (Orient == ori_land/*((m_PropertySheetState.wOreint == eSecondOption) || (m_PropertySheetState.wOreint == eFourthOption))*///landscape
				&& ((m_PropertySheetState.wPaper == PAPERSIZE_A4)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_B5)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_LETTER))
				&& ((wBindedge == binedge_right) || (wBindedge == binedge_top)))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else
			{
				m_ControlState.cPaper_I = SCUI_STATE_ON;
				m_ControlState.cStaple_I = SCUI_STATE_ON;
			}
		}
	}
	else if (m_PropertySheetState.wStaple == staple_saddle_staple )
	{
		if ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A5) ||
			m_PropertySheetState.wOutputSize >= OUTPUTPAPERSIZE_EXECUTIVE
			&& m_PropertySheetState.wOutputSize <= OUTPUTPAPERSIZE_JAPANESE_POSTCARD)
		{
			m_ControlState.cOutputSize_I = SCUI_STATE_ON;
			m_ControlState.cStaple_I = SCUI_STATE_ON;
		}
		else if (((m_PropertySheetState.wPaper == PAPERSIZE_A5) || m_PropertySheetState.wPaper >= PAPERSIZE_EXECUTIVE
			&& m_PropertySheetState.wPaper <= PAPERSIZE_JAPANESE_POSTCARD)
			&& (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_SAOS))
		{
			m_ControlState.cPaper_I = SCUI_STATE_ON;
			m_ControlState.cStaple_I = SCUI_STATE_ON;
		}
	}
	SetControlState(hDlg, IDC_BTN_OUTPUTSIZE_I, m_ControlState.cOutputSize_I);
	SetControlState(hDlg, IDC_BTN_PAPER_I, m_ControlState.cPaper_I);
	SetControlState(hDlg, IDC_BTN_STAPLE_I, m_ControlState.cStaple_I);
EXIT:
	return;
}
//<E> Modified The Code to Fix Bug #3533,2021.07.28,SSDI:Chanchal Singla
//=============================================================================
// function
//       ShowConstraintsOnInformationIcon
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Punch Vs OriginalSize Vs OutputSize Information Icon
//=============================================================================
VOID CDocumentPropertyPage::ShowPunchVsOriginalSizeVsOutputSizeInformationIcon(HWND hDlg, INT iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap::const_iterator Iterator;
	Iterator = m_UIControls.find(IDC_CMB_OUTPUTSIZE);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			m_PropertySheetState.wOutputSize = (short)pImageCombo->GetVal(IDC_CMB_OUTPUTSIZE, hDlg);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::ShowPunchVsOriginalSizeVsOutputSizeInformationIcon -> NULL_OBJECT(pImageCombo)\n");
		}
	}
	else
	{
		m_PropertySheetState.wOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	}
	m_PropertySheetState.wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	m_PropertySheetState.wPaper = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	m_PropertySheetState.wPunch = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex;
	m_PropertySheetState.wOreint = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex;
	long wBindedge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	short Orient = GetOrientation(m_pFeatureInfoList);
	if ((m_PropertySheetState.wPunch == punch_none)
		|| (wBooklet != booklet_none))
	{
		goto EXIT;
	}
	if ((m_PropertySheetState.wPunch != punch_none))
	{
		if ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A3 || m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A5
			|| m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B4 || m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_TABLOID
			|| m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LEGAL) || m_PropertySheetState.wOutputSize >= OUTPUTPAPERSIZE_EXECUTIVE
			&& m_PropertySheetState.wOutputSize <= OUTPUTPAPERSIZE_CUSTOMSIZE)
		{
			if ((Orient == ori_port/*(m_PropertySheetState.wOreint == eFirstOption) || (m_PropertySheetState.wOreint == eThirdOption)*/)//Portrait
				&& ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A3)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B4)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LEGAL)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_TABLOID) 
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_EXECUTIVE)) && (wBindedge == binedge_top))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cStaple_I = SCUI_STATE_OFF;
			}
			else if ((Orient == ori_land/*(m_PropertySheetState.wOreint == eSecondOption) || (m_PropertySheetState.wOreint == eFourthOption)*/)//landscape
				&& ((m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_A3)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_B4)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_LEGAL)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_TABLOID)
					|| (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_EXECUTIVE))
				&& ((wBindedge == binedge_left) || (wBindedge == binedge_right)))
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
				m_ControlState.cPunch_I = SCUI_STATE_OFF;
			}
			else
			{
				m_ControlState.cOutputSize_I = SCUI_STATE_ON;
				m_ControlState.cPunch_I = SCUI_STATE_ON;
			}
		}
		else if (((m_PropertySheetState.wPaper == PAPERSIZE_A3 || m_PropertySheetState.wPaper == PAPERSIZE_A5
			|| m_PropertySheetState.wPaper == PAPERSIZE_B4 || m_PropertySheetState.wPaper == PAPERSIZE_TABLOID
			|| m_PropertySheetState.wPaper == PAPERSIZE_LEGAL) ||( m_PropertySheetState.wPaper >= PAPERSIZE_EXECUTIVE
			&& m_PropertySheetState.wPaper <= PAPERSIZE_CUSTOMSIZE))
			&& (m_PropertySheetState.wOutputSize == OUTPUTPAPERSIZE_SAOS))
		{
			if ((Orient == ori_port/*(m_PropertySheetState.wOreint == eFirstOption) || (m_PropertySheetState.wOreint == eThirdOption)*/)//Portrait
				&& ((m_PropertySheetState.wPaper == PAPERSIZE_A3)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_B4)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_LEGAL)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_TABLOID)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_EXECUTIVE)) && (wBindedge == binedge_top))
			{
				m_ControlState.cPaper_I = SCUI_STATE_OFF;
				m_ControlState.cPunch_I = SCUI_STATE_OFF;
			}
			else if ((Orient == ori_land/*(m_PropertySheetState.wOreint == eSecondOption) || (m_PropertySheetState.wOreint == eFourthOption)*/)//landscape
				&& ((m_PropertySheetState.wPaper == PAPERSIZE_A3)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_B4)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_LEGAL)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_TABLOID)
					|| (m_PropertySheetState.wPaper == PAPERSIZE_EXECUTIVE))
				&& ((wBindedge == binedge_left) || (wBindedge == binedge_right)))
			{
				m_ControlState.cPaper_I = SCUI_STATE_OFF;
				m_ControlState.cPunch_I = SCUI_STATE_OFF;
			}
			else
			{
				m_ControlState.cPaper_I = SCUI_STATE_ON;
				m_ControlState.cPunch_I = SCUI_STATE_ON;
			}
		}
	}

	SetControlState(hDlg, IDC_BTN_OUTPUTSIZE_I, m_ControlState.cOutputSize_I);
	SetControlState(hDlg, IDC_BTN_PAPER_I, m_ControlState.cPaper_I);
	SetControlState(hDlg, IDC_BTN_PUNCH_I, m_ControlState.cPunch_I);
EXIT:
	return;
}
//<E>Modified the conditions to fix Bug #4012,2021.August.03,SSDI:Chanchal Singla
//<S><C>Modiled the Conditions To Fix Redmine Bug #3531,2021-28-07,SSDI:Chanchal Singla
//No Information icon is displayed for Staple
//=============================================================================
// function
//       ShowConstraintsOnInformationIcon
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Punch Vs OriginalSize Vs OutputSize Information Icon
//=============================================================================
VOID CDocumentPropertyPage::ShowStapleVsMarginShiftInformationIcon(HWND hDlg, INT iCtrlID)
{
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	m_PropertySheetState.wMargin = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex;
	m_PropertySheetState.wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;

	CShowMsgStaple  wObjCStapleShowMessage(hDlg, m_hStringResourceHandle, m_hDialogResourceHandle,
		m_hBitmapResourceHandle, m_pPrinterName);
	if ((wBooklet != booklet_none) && (m_PropertySheetState.wMargin == marginshift_none))
	{
		goto EXIT;
	}
	if (m_PropertySheetState.wMargin != marginshift_none)
	{
		m_ControlState.cStaple_I = SCUI_STATE_ON;
	}

	if ((m_PropertySheetState.wStaple != 0) && (wObjCStapleShowMessage.IsNeverShow() == TRUE))
	{
		m_ControlState.cStaple_I = SCUI_STATE_ON;
	}

	SetControlState(hDlg, IDC_BTN_STAPLE_I, m_ControlState.cStaple_I);
EXIT:
	return;
}
//=============================================================================
// function
//       ShowConstraintsOnInformationIcon
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Punch Vs PaperType Information Icon
//=============================================================================
VOID CDocumentPropertyPage::ShowStapleVsPaperTypeInformationIcon(HWND hDlg, INT iCtrlID)
{
	LONG wPtype = FALSE;
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	m_PropertySheetState.wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	m_PropertySheetState.wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	m_PropertySheetState.wPunch = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex;

	CShowMsgStaple wObjCStapleShowMessage(hDlg, m_hStringResourceHandle, m_hDialogResourceHandle,
			m_hBitmapResourceHandle, m_pPrinterName);

	if ((wBooklet != booklet_none) && (m_PropertySheetState.wDuplex != 0))
	{
		goto EXIT;
	}
	if ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "LABEL")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "PREPUNCHED")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "TRANSPARENCY1")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "ENVELOPE")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "HEAVYPAPER")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "JAPANESEPOSTCARD")))
	{
		wPtype = TRUE;
	}
	if ((wPtype == TRUE)
		&& ((m_PropertySheetState.wStaple != 0)))
	{
		if (wObjCStapleShowMessage.IsNeverShow() == TRUE) {
			m_ControlState.cStaple_I = SCUI_STATE_ON;
		}
		else
		{
			if (!strcmp("STAPLE_SADDLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
			{
				m_ControlState.cStaple_I = (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "HEAVYPAPER")
					|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "PREPUNCHED"))?
					SCUI_STATE_OFF: SCUI_STATE_ON;
			}
			else
			{
				m_ControlState.cStaple_I = (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "PREPUNCHED")) ?
					SCUI_STATE_OFF : SCUI_STATE_ON;
			}
		}
	
	}
	if ((wPtype == TRUE)
		&& ((m_PropertySheetState.wPunch != 0)))
	{
		m_ControlState.cPunch_I =(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "HEAVYPAPER"))?
			SCUI_STATE_OFF: SCUI_STATE_ON;
	}
	SetControlState(hDlg, IDC_BTN_STAPLE_I, m_ControlState.cStaple_I);
	SetControlState(hDlg, IDC_BTN_PUNCH_I, m_ControlState.cPunch_I);
EXIT:
	return;
}
//<E>Modiled the Conditions To Fix Redmine Bug #3531,2021-28-07,SSDI:Chanchal Singla
//<S><A> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
BOOL CDocumentPropertyPage::OnUsSelect(HWND hDlg, WCHAR* pPrinterName, DWORD wControlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL			blRet = FALSE;
	long			lRet = false;
	short			wIndex = 0;
	CShRegUS		*preg = NULL;
	PREGJCINFO		pregInfo = NULL;
	BOOL bIsUIUpdated = FALSE;
	//========================================================================
	//This Flag variable is reserved , please don't used this flag for other purpose and 
	// don't change the value,otherwise driver maybe crashed
	BOOL wFlag = FALSE;
	//========================================================================
	CShRegJC		*pregJC = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	CShJsonUS			*pjsonus = NULL;
	CShJsonMS			*pjsonms = NULL;
	POEMDEV pPrivatetDevMode = NULL;
	//PFEATUREINFOLIST pFeatureInfoList = NULL; //<S><D>Fix for Coverity issue 92766. Moved the declaration to Header file. Harika,20230427
	PROPSTATE wPropertySheetState_Pre;
	SecureZeroMemory(&wPropertySheetState_Pre, sizeof(wPropertySheetState_Pre));
	memcpy_s(&wPropertySheetState_Pre, sizeof(PROPSTATE), &m_PropertySheetState, sizeof(PROPSTATE));
	//// Previous public Devmode
	DEVMODE wOemPublicDevMode_Pre;
	SecureZeroMemory(&wOemPublicDevMode_Pre, sizeof(wOemPublicDevMode_Pre));
	memcpy_s(&wOemPublicDevMode_Pre, sizeof(DEVMODE), m_pOemPublicDevMode, sizeof(DEVMODE));

	//// Previous Private Devmode
	OEMDEV wOemPrivateDevMode_Pre = {};
	SecureZeroMemory(&wOemPrivateDevMode_Pre, sizeof(wOemPrivateDevMode_Pre));
	memcpy_s(&wOemPrivateDevMode_Pre, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));
	UIControlMap::const_iterator Iterator;
	CShCmbUserset *pCmbUserset = NULL;
	Iterator = m_UIControls.find(IDC_CMB_SHARED_USRSET);
	if (Iterator != m_UIControls.end())
	{
		pCmbUserset = static_cast<CShCmbUserset*>(Iterator->second);
		if (pCmbUserset == NULL)
		{
			slog.putLog("CDocumentPropertyPage::OnUsSelect -> NULL_OBJECT(pCmbUserset)\n");
		}
	}
	else
	{
		goto EXIT;
	}


	preg = new CShRegUS(ghInstance, pPrinterName, m_hStringResourceHandle, NULL, m_pFeatureInfoList, m_pOemPrivateDevMode);;
	if (preg == NULL)
	{
		goto EXIT;
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonus = new CShJsonUS(ghInstance, m_pPrinterName, m_hStringResourceHandle);
		pjsonus->Init();
		if (pjsonus == NULL)
			goto EXIT;
		pjsonms = new CShJsonMS(ghInstance, m_pPrinterName);
		pjsonms->Init();
	}



	/*Get the Latest index of Favorities Feature*/
	wIndex = GetValue(hDlg, wControlID);

	if (*m_pUsIndex != wIndex)
	{
		pPrivatetDevMode = new OEMDEV;
		if (NULL == pPrivatetDevMode)
		{
			return E_FAIL;
		}
		// Set all the member variables to zero
		memset(pPrivatetDevMode, 0, sizeof(OEMDEV));
		//Set OEMExtra Structure
		pPrivatetDevMode->dmOEMExtra.dwSize = sizeof(OEMDEV);
		pPrivatetDevMode->dmOEMExtra.dwSignature = OEM_SIGNATURE;
		pPrivatetDevMode->dmOEMExtra.dwVersion = OEM_VERSION;
		if (wIndex == 0)
		{
			bIsUIUpdated = TRUE;
			if (pCmbUserset)
			{
				GPDFEATUREOPTIONLIST FeatureOptionList = {};
				pCmbUserset->ResetDevmode(pPrinterName, pPrivatetDevMode, m_DefaultSettings, m_PaperInfo);
			//	FillPubDModeData(pPrivatetDevMode, m_pOemPublicDevMode);
				pCmbUserset->MergeDevmode(pPrivatetDevMode);
				pFeatureInfoList = pCmbUserset->UpdateFeatureInfoList(pFeatureInfoList, wIndex);
			//<S><A>to Fix SSDI BUG #3457,2021-04-22,SSDI:Chanchal Singla
				if (DMPAPER_A4 == GetPaperSizeDef())
				{
					pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption = "A4";
					pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex = eSecondOption;
					pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uDefaultOptionIndex = eSecondOption;
				}
				//<E>to Fix SSDI BUG #3457,2021-04-22,SSDI:Chanchal Singla
			}
			else
			{
				slog.putLog("CDocumentPropertyPage::OnUsSelect -> NULL_OBJECT(pCmbUserset)\n");
			}
		}
		else
		{
			bIsUIUpdated = TRUE;
			if ((*m_pmcf).IsWriteToJson() == TRUE)
			{
				if ((*pjsonus).ReadData(&pPrivatetDevMode->scpi, wIndex) == 0)
				{
					//if data is not available in registry that time fbove code executes
					wIndex = 0;
					if (pCmbUserset)	//Added to fix coverity issue 92762 issue1
					{
						pCmbUserset->ResetDevmode(pPrinterName, pPrivatetDevMode, m_DefaultSettings, m_PaperInfo);
						FillPubDModeData(pPrivatetDevMode, m_pOemPublicDevMode);
						pCmbUserset->MergeDevmode(pPrivatetDevMode);
					}
				}

			}
			else
			{
				if ((*preg).ReadData(pPrinterName, &pPrivatetDevMode->scpi, wIndex) == 0)
				{
					//if data is not available in registry that time fbove code executes
					wIndex = 0;
					if (pCmbUserset)	//Added to fix coverity issue 92762 issue1
					{
						pCmbUserset->ResetDevmode(pPrinterName, pPrivatetDevMode, m_DefaultSettings, m_PaperInfo);
						FillPubDModeData(pPrivatetDevMode, m_pOemPublicDevMode);
						pCmbUserset->MergeDevmode(pPrivatetDevMode);
					}
				}
			}
			if (pCmbUserset)		//Added to fix coverity issue 92762 issue2
			pFeatureInfoList = pCmbUserset->UpdateFeatureInfoList(pFeatureInfoList, wIndex);
		}


		if (CompareCurrentSettingVsConfigTabData(&pPrivatetDevMode->scpi, &m_pOemPrivateDevMode->scpi, pFeatureInfoList, m_pFeatureInfoList, wIndex) == TRUE)
		{
			if (wIndex != 0)
			{
				if ((*m_ObjCShowMessage).Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_43, MB_ICONINFORMATION | MB_YESNO) == IDYES)
				{
					if ((*m_pmcf).IsWriteToJson() == TRUE)
					{
						(*pjsonus).DeleteData(wIndex);
						(*pjsonus).WriteJsonDataToFile();
					}
					else
					{
						(*preg).DeleteData(wIndex);
					}

					if (*m_pUsIndex > wIndex)
					{
						//Fixed Coverity #91482,13-08-2021,SSDI:Chanchal Singla
						//*m_pUsIndex--;
						m_pUsIndex--;
					}
					pCmbUserset->OnInit(IDC_CMB_SHARED_USRSET, hDlg);
				}
				goto EXIT;
			}
		}


		size_t nFeatureIndex = 0;
		size_t nNumberOfFeatures = 0;
		/*Get the GPD feature Count*/
		nNumberOfFeatures = GetGPDFeatureCount();
		/*===============================================================================================================*/
		/*===============Update the FeatureInfo from local pFeatureInfoList to Global m_pFeatureInfolist=================*/
		/*==============Make Sure don't copy the direct address(m_pFeatureInfoList = pFeatureInfoList) of================*/
		/*===============pFeatureInfoList to m_pFeatureInfoList,othersise it's may effect the whole UI===================*/
		/*================================================================================================================*/
		for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
		{
			m_pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].pszFeature = pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].pszFeature;
			m_pFeatureInfoList->pFeatureOptionPair[nFeatureIndex].pszFeature = pFeatureInfoList->pFeatureOptionPair[nFeatureIndex].pszFeature;
			m_pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].ppszOptions = pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].ppszOptions;
			m_pFeatureInfoList->pFeatureOptionPair[nFeatureIndex].pszOption = pFeatureInfoList->pFeatureOptionPair[nFeatureIndex].pszOption;
			/*uDefaultOptionIndex is commented because changed feature has not saved and retaining after reload printing preferences*/
			//m_pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].uDefaultOptionIndex = pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].uDefaultOptionIndex;
			m_pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].uChangedOptionIndex = pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].uChangedOptionIndex;

			m_pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].dwOptions = pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].dwOptions;
		}

		//update User-Setting PrivateDevMode to m_pOemPrivateDevMode
		OEMDEV m_ptempPrivateDevmode = {};
		memcpy_s(&m_ptempPrivateDevmode, sizeof(OEMDEV), pPrivatetDevMode, sizeof(OEMDEV));
		memcpy_s(m_pOemPrivateDevMode, sizeof(OEMDEV), &m_ptempPrivateDevmode, sizeof(OEMDEV));
		//<S><A>to Fix SSDI BUG #3631,2021-04-22,SSDI:Chanchal Singla	
		CRegistryAPI cRegApi;
		cRegApi.ReadData(m_pPrinterName, &m_pOemPrivateDevMode->scpi.dev);
		DevDmodetopropstate();
		UpdatePropState(m_pOemPrivateDevMode, &m_PropertySheetState);
		//<S><A>To Fix feedback #49,2021.08.01,SSDI:Chanchal Singla
		{
			// --- copies
			DWORD wPos = m_pOemPrivateDevMode->scpi.pub.dmCopies;
			UIControlMap::const_iterator Iterator;
			Iterator = m_UIControls.find(IDC_EDT_COPIES);
			if (Iterator != m_UIControls.end())
			{
				CShEditSpin *pEditSpin = static_cast<CShEditSpin*>(Iterator->second);
				if (pEditSpin)
				{
						pEditSpin->SetPos(hDlg, IDC_EDT_COPIES, wPos);
				}
				else
				{
					slog.putLog("CMainTabPropertyPage::Select -> NULL_OBJECT(CShEditSpins)\n");
				}

			}
		}
		//<E>To Fix feedback #49,2021.08.01,SSDI:Chanchal Singla

		//added to the support of restriction between congi tab and preferences tab for favourites
		m_PropertySheetState.blLimitByOption = bool_false;
		//<S><A>To Fix Redmine Bug #3908,2021-06-15,SSDI:Chanchal Singla 
		if (wIndex != 0)
		{
			if ((*m_pmcf).IsWriteToJson() == TRUE)
				lRet = (*pjsonus).ReadDataJson(wIndex);
			else
				lRet = (*preg).ReadDataReg(pPrinterName, wIndex);
			//<S><C>To Fix Redmine Bug #3681,2021.11.29,SSDI:Chanchal Singla
			///Custom Paper values are changing to Default even though cancel button is clicked
			USERPAPERSIZEDATA upsd = {};
			CUPSReg pregUps(m_hStringResourceHandle, m_pPrinterName);
			CShJsonUserPSize	*pjsonups = NULL;
			if ((*m_pmcf).IsWriteToJson() == TRUE)
			{
				pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
				pjsonups->Init();
			}
			//<S><C>To Fix Redmine Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
			//pregUps.ReadUPSDataToUs(m_pPrinterName, wIndex,DMPAPER_CUSTOM, &upsd);
			//wCusUnit
			m_PropertySheetState.wCusUnit = m_pOemPrivateDevMode->scpi.ext.byCusUnits;
			if (m_pOemPrivateDevMode->scpi.pub.dmPaperSize == DMPAPER_CUSTOM)
			{
//<S><C>To Fix Redmine issue  #6162,2023.04.05,SSDI:Chandrashekar V
				//m_PropertySheetState.dwCustomWidth = m_pOemPrivateDevMode->scpi.pub.dmPaperWidth;
				//m_PropertySheetState.dwCustomLength = m_pOemPrivateDevMode->scpi.pub.dmPaperLength;
				if (unit_inch == m_PropertySheetState.wCusUnit)
				{
					int m_Width = MilliToInch(m_pOemPrivateDevMode->scpi.pub.dmPaperWidth, MM_LOMETRIC, MM_LOENGLISH);
					int m_Length = MilliToInch(m_pOemPrivateDevMode->scpi.pub.dmPaperLength, MM_LOMETRIC, MM_LOENGLISH);

					m_PropertySheetState.dwCustomWidth = m_Width;
					m_PropertySheetState.dwCustomLength = m_Length;

				}
				else
				{
					m_PropertySheetState.dwCustomWidth = m_pOemPrivateDevMode->scpi.pub.dmPaperWidth;
					m_PropertySheetState.dwCustomLength = m_pOemPrivateDevMode->scpi.pub.dmPaperLength;
				}
//<E>To Fix Redmine issue  #6162,2023.04.05,SSDI:Chandrashekar V
		
			}
			else
			{
				if (m_PropertySheetState.wCusUnit == unit_inch)
				{
					MIP_MINMAX_CONV_PARAM stCnvParam;
					SecureZeroMemory(&stCnvParam, sizeof(MIP_MINMAX_CONV_PARAM));
					stCnvParam.paper = mip_paper_cus_usr;
					stCnvParam.mi = mip_mi_inch;
					stCnvParam.pos = 1;
					m_PropertySheetState.dwCustomWidth = PixelToInch_CusPaper(m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
					stCnvParam.wl = mip_wl_width;
					m_PropertySheetState.dwCustomWidth = checkMinMax(m_PropertySheetState.dwCustomWidth, &stCnvParam);
					stCnvParam.wl = mip_wl_length;
					m_PropertySheetState.dwCustomLength = PixelToInch_CusPaper(m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
					m_PropertySheetState.dwCustomLength = checkMinMax(m_PropertySheetState.dwCustomLength, &stCnvParam);
				}
				else
				{
					MIP_MINMAX_CONV_PARAM stCnvParam;
					SecureZeroMemory(&stCnvParam, sizeof(MIP_MINMAX_CONV_PARAM));
					stCnvParam.paper = mip_paper_cus_usr;
					stCnvParam.mi = mip_mi_mill;
					stCnvParam.pos = 10;
					stCnvParam.wl = mip_wl_width;
					m_PropertySheetState.dwCustomWidth = PixelToMilli_CusPaper(m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOMETRIC, &stCnvParam);
					stCnvParam.wl = mip_wl_length;
					m_PropertySheetState.dwCustomLength = PixelToMilli_CusPaper(m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOMETRIC, &stCnvParam);
				}
			}
				
			upsd.dwWidth	= m_PropertySheetState.dwCustomWidth;
			upsd.dwLength	= m_PropertySheetState.dwCustomLength;
			upsd.wUnit		= m_PropertySheetState.wCusUnit;

			//Custom paper :Output Size dimensions displays 11.69 X 17 in,
			//even though Maximum Paper Size is set to A4/Letter in Configuration tab.
			if (m_PropertySheetState.byMaxPaperSize == eSecondOption)
			{
				if (unit_mm == upsd.wUnit)
				{
					upsd.dwWidth = (upsd.dwWidth > MAXPAPER_CUSTOM_WIDTH_MM) ? MAXPAPER_CUSTOM_WIDTH_MM : upsd.dwWidth;
					upsd.dwLength = (upsd.dwLength > MAXPAPER_CUSTOM_LENGTH_MM) ? MAXPAPER_CUSTOM_LENGTH_MM : upsd.dwLength;
				}
				else if (unit_inch == upsd.wUnit)//inch
				{
					upsd.dwWidth = (upsd.dwWidth > MAXPAPER_CUSTOM_WIDTH_INCH) ? MAXPAPER_CUSTOM_WIDTH_INCH : upsd.dwWidth;
					upsd.dwLength = (upsd.dwLength > MAXPAPER_CUSTOM_LENGTH_INCH) ? MAXPAPER_CUSTOM_LENGTH_INCH : upsd.dwLength;
				}
			}
			//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
			memcpy(&m_ppi->PaperSize_upsd, &upsd, sizeof(USERPAPERSIZEDATA));
			//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
			if ((*m_pmcf).IsWriteToJson() == TRUE)
				pjsonups->ReadUPSDataToUs(wIndex, DMPAPER_CUSTOM_ZOOM, &upsd);
			else
				pregUps.ReadUPSDataToUs(m_pPrinterName, wIndex, DMPAPER_CUSTOM_ZOOM, &upsd);

			memcpy(&m_ppi->FitToPaperSize_upsd, &upsd, sizeof(USERPAPERSIZEDATA));
			//<E>To Fix Redmine Bug #3681,2021.11.29,SSDI:Chanchal Singla
		}
		// -- Update Margin Shift Data To Propstate
		SetMarginShiftDataToPropState(m_pOemPrivateDevMode, &m_PropertySheetState, pPrinterName);
		//<E>To Fix Redmine Bug #3908,2021-06-15,SSDI:Chanchal Singla 
		//<E>to Fix SSDI BUG #3631,2021-04-22,SSDI:Chanchal Singla	
		if (wIndex != 0)
		{
			//<S><A>To Fix Redmine Bug #5522,26-12-2022,SSDI:Manoj Sunagar
			//Bug #5522 Print position-Custom : Saved User setting values are not retained when reselected
			SetPPosDataToPropState(m_pOemPrivateDevMode, &m_PropertySheetState, pPrinterName);
			//<E>To Fix Redmine Bug #5522 ,26-12-2022,SSDI:Manoj Sunagar

			// --- DM_ALWAYSUSE_ACNUM
			m_PropertySheetState.wUseAcNum = wOemPrivateDevMode_Pre.scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM ? bool_true : bool_false;

			// --- DM_ALWAYSUSE_USERID
			m_PropertySheetState.wUseUName = wOemPrivateDevMode_Pre.scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_USERID ? bool_true : bool_false;

			// --- DM_ALWAYSUSE_JOBID
			m_PropertySheetState.wUseJName = wOemPrivateDevMode_Pre.scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_JOBID ? bool_true : bool_false;

			// --- DM_USE_LOGINNAME
			m_PropertySheetState.wLoginName = wOemPrivateDevMode_Pre.scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME ? bool_true : bool_false;

			// --- DM_USE_LOGINPASS
			m_PropertySheetState.wLoginPass = wOemPrivateDevMode_Pre.scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS ? bool_true : bool_false;

			// --- DM_DOCFILING
			m_PropertySheetState.wRetention = wOemPrivateDevMode_Pre.scpi.ext.JobCtrl.wFieldJC & DM_DOCFILING ? bool_true : bool_false;

			// --- DM_USE_PIN
			m_PropertySheetState.wUsePIN = wOemPrivateDevMode_Pre.scpi.ext.JobCtrl.wFieldJC & DM_USE_PIN ? bool_true : bool_false;

			m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = wOemPrivateDevMode_Pre.scpi.ext.JobCtrl.wFieldJC;
			// ================================================
			// _/_/_/  job control data
			// ================================================
			SetRegJCInfoToPropState(&m_pOemPrivateDevMode->scpi, &m_PropertySheetState, pPrinterName);

			//Set WaterMark index
			*m_pWaterMarkIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
			// update the wFDJobHandling variable to true when factory default is selected ,otherwise update to FALSE value
			m_ppi->wFDJobHandling = FALSE;
		}
		else
		{
			//If Favoutrites is Set To factory Defaults

			//<S><D>To Deleted the code  Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
			// data should not store immediately in the registry when we select factory default,
			//while Clicking on apply/ok button, that time data should be store in the registry
			//pregInfo = new REGJCINFO;
			//if (pregInfo == NULL)
			//	goto EXIT;

			//pregJC = new CShRegJC(m_hStringResourceHandle, pPrinterName);
			//if (pregJC == NULL)
			//	goto EXIT;
			//<E>To Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
			//<S><A>To Fix Redmine #7582,21-06-2024,SSDI:Manoj S
			if (strcmp("NONE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption) &&
				(m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OFFSET].uChangedOptionIndex != eFirstOption))
			{
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OFFSET].uChangedOptionIndex = eFirstOption;
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OFFSET].pszOption = "None";
			}

			if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
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
			//<E>To Fix Redmine #7582,21-06-2024,SSDI:Manoj S
			m_PropertySheetState.wUsePIN = bool_false;
			m_PropertySheetState.wUseAcNum = bool_false;
			m_PropertySheetState.wUseJName = bool_false;
			m_PropertySheetState.wLoginPass = bool_false;
			m_PropertySheetState.wLoginNameSSO = bool_false;
			m_PropertySheetState.wDocFileFolder = bool_false;
			
			if (m_PropertySheetState.wPPlcyWinLogin == bool_false)
			{
				m_PropertySheetState.wLoginName = bool_false;
			}

			if (m_PropertySheetState.wPPlcyLoginName == bool_false)
			{
				m_PropertySheetState.wUseUName = bool_false;
				SecureZeroMemory(m_PropertySheetState.szUserName, sizeof(m_PropertySheetState.szUserName));
			}

			if (m_PropertySheetState.wLoginName == 0)
			{
				SecureZeroMemory(m_PropertySheetState.szLoginNameUA, sizeof(m_PropertySheetState.szLoginNameUA));
			}

			SecureZeroMemory(m_PropertySheetState.szPIN, sizeof(m_PropertySheetState.szPIN));
			SecureZeroMemory(m_PropertySheetState.szRetentionPassword, sizeof(m_PropertySheetState.szRetentionPassword));
			SecureZeroMemory(m_PropertySheetState.szSingleSignOnName, sizeof(m_PropertySheetState.szSingleSignOnName));
			SecureZeroMemory(m_PropertySheetState.szUserNumber, sizeof(m_PropertySheetState.szUserNumber));
			SecureZeroMemory(m_PropertySheetState.szJobName, sizeof(m_PropertySheetState.szJobName));
			SecureZeroMemory(m_PropertySheetState.szLoginPasswordUA, sizeof(m_PropertySheetState.szLoginPasswordUA));
			SecureZeroMemory(m_PropertySheetState.szFolderPass, sizeof(m_PropertySheetState.szFolderPass));
		
			//<S><D>To Deleted the code  Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
			// data should not store immediately in the registry when we select factory default,
			//while Clicking on apply/ok button, that time data should be store in the registry
			//Coverity Fix: 91328 - SSDI Seetharam - 20210503
			//SecureZeroMemory(pregInfo, sizeof(pregInfo));
			/*SecureZeroMemory(pregInfo, sizeof(REGJCINFO));
			if (PropStateToRegJCInfo(&m_PropertySheetState, pregInfo, pPrinterName) != FALSE)
			{
				(*pregJC).WriteJCData(pPrinterName, pregInfo, JC_ALL, m_PropertySheetState.wPPlcyWinLogin);
			}*/
			//<E>To Deleted the code  Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
			
			// update the wFDJobHandling variable to true when factory default is selected ,otherwise update to FALSE value
			m_ppi->wFDJobHandling = TRUE;
			//Set never show flag
			{
				CShowMsgStaple*  wObjCStapleShowMessage = NULL;
				wObjCStapleShowMessage = new CShowMsgStaple(hDlg, m_hStringResourceHandle, m_hDialogResourceHandle,
					m_hBitmapResourceHandle, pPrinterName);
				if (wObjCStapleShowMessage != NULL)
				{
					(*wObjCStapleShowMessage).SetNeverShow(FALSE);
					delete wObjCStapleShowMessage;
					wObjCStapleShowMessage = NULL;
				}
			}
			//Set WaterMark index
			m_pOemPrivateDevMode->scpi.ext.nWatermark = 0;
			*m_pWaterMarkIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
			{
				USERPAPERSIZEDATA upsd;
				SecureZeroMemory(&upsd, sizeof(upsd));
				CUPSReg FAR		*pregUps = NULL;
				CShJsonUserPSize	*pjsonups = NULL;
				pregUps = new CUPSReg(m_hStringResourceHandle, m_pPrinterName);
				if (pregUps == NULL)
				{
					return FALSE;
				}

				if ((*m_pmcf).IsWriteToJson() == TRUE)
				{
					pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
					pjsonups->Init();
				}
				//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				///Custom Paper values are changing to Default even though cancel button is clicked
				if ((*m_pmcf).IsWriteToJson() == TRUE)
				{
					(*pjsonups).GetUPSDefData(&upsd);
				}
				else
				{
					(*pregUps).GetUPSDefData(&upsd);
				}
				memcpy(&m_ppi->PaperSize_upsd, &upsd, sizeof(USERPAPERSIZEDATA));
//<S><A>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
				m_PropertySheetState.dwCustomWidth = upsd.dwWidth;
				m_PropertySheetState.dwCustomLength = upsd.dwLength;
				m_PropertySheetState.wCusUnit = upsd.wUnit;
//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla

				memcpy(&m_ppi->FitToPaperSize_upsd, &upsd, sizeof(USERPAPERSIZEDATA));
				//(*pregUps).resetEx(m_pPrinterName, DMPAPER_CUSTOM);
				//(*pregUps).resetEx(m_pPrinterName, DMPAPER_CUSTOM_ZOOM);
				//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				if (pregUps != NULL)
				{
					delete pregUps;
					pregUps = NULL;
				}
				if (pjsonups != NULL)
				{
					delete pjsonups;
					pjsonups = NULL;
				}
			}
			//<S><A>To Fix Redmine Bug #3908,2021-06-15,SSDI:Chanchal Singla 
			//Factory Defaults is not working for Print Position drop down
			m_PropertySheetState.wPPosChg = bool_false;
			// --- DM_PRINTPOSITION
			m_pOemPrivateDevMode->scpi.ext.wFieldEDM = (m_PropertySheetState.wPPosChg == bool_true) ?
				(m_pOemPrivateDevMode->scpi.ext.wFieldEDM | DM_PRINTPOSITION_CHG) : (m_pOemPrivateDevMode->scpi.ext.wFieldEDM & ~DM_PRINTPOSITION_CHG);
			SetPPosDataToPropState(m_pOemPrivateDevMode,&m_PropertySheetState,pPrinterName);
			// --- byMgnShift
			m_pOemPrivateDevMode->scpi.ext.byMgnShift = 0; 
			m_PropertySheetState.wMargin = m_pOemPrivateDevMode->scpi.ext.byMgnShift;
			//<S><C> To fix feedback issue 67, Harika, 20220830
			if ((*m_pmcf).IsWriteToJson() == TRUE)
			{
				pjsonms->resetEx();
			}
			else
			{
				CShRegMS regms(m_hStringResourceHandle, pPrinterName);
				regms.resetEx(pPrinterName);
			}
			//<E> To fix feedback issue 67, Harika, 20220830
			//<E>To Fix Redmine Bug #3908,2021-06-15,SSDI:Chanchal Singla 
			//<S><A>To Fix #3506,2021-04-29,SSDI:Chanchal Singla
			//Mimic displays in Color even though Black and White is set in Configuration tab
			//update color feature based on constraints
			{
				UINT	SelectedOptionindex = 0;
				PCSTR SellectedOptionName = NULL;
				if (1 == m_PropertySheetState.wColorOption) // Black and White
				{
					SelectedOptionindex = eThirdOption;
					SellectedOptionName = "Mono";
					m_pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_mono;
				}
				else if (0 == m_PropertySheetState.wColorOption) // Color
				{
					if (m_PropertySheetState.byAutoColorMode == 1) {
						//<S><A> To fix SSDI bug #5649(SDC SQA bug #47180 when Factory defaults is selected). Harika, 20221018
						if (m_DefaultSettings[FEATURE_COLOR_MODE].iDevmodeValue == colmode_mono)
						{
							SelectedOptionindex = eThirdOption;
							SellectedOptionName = "Mono";
							m_pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_mono;
						}
						else
						{
							SelectedOptionindex = eSecondOption;
							SellectedOptionName = "Color";
							m_pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_color;
						}
						//<E> To fix SSDI bug #5649(SDC SQA bug #47180 when Factory defaults is selected). Harika, 20221018
					}
					else
					{
						if (m_DefaultSettings[FEATURE_COLOR_MODE].iDevmodeValue == colmode_mono)
						{
							SelectedOptionindex = eThirdOption;
							SellectedOptionName = "Mono";
							m_pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_mono;
						}
						else if (m_DefaultSettings[FEATURE_COLOR_MODE].iDevmodeValue == colmode_color)
						{
							SelectedOptionindex = eSecondOption;
							SellectedOptionName = "Color";
							m_pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_color;
						}
						else
						{
							SelectedOptionindex = eFirstOption;
							SellectedOptionName = "Auto";
							m_pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_auto;
						}
					}
				}
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = SellectedOptionName;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = SelectedOptionindex;
			}
			//<E>To Fix #3506,2021-04-29,SSDI:Chanchal Singla
		}
		*m_pUsIndex = wIndex;
	}
	//enable apply Button 
	if(bIsUIUpdated == TRUE)
	{
		CUIControl pControl;
		pControl.EnableApplyButton(hDlg);
	}
EXIT:
	if (preg != NULL)
	{
		delete preg;
		preg = NULL;
	}

	if (pPrivatetDevMode != NULL)
	{
		delete pPrivatetDevMode;
		pPrivatetDevMode = NULL;

	}
	//<S><D>To Fix Cverity #91659,#91660, 2022.02.25,SSDI:Chanchal Singla
	/*if (pregJC != NULL)
	{
		delete pregJC;
		pregJC = NULL;
	}
	if (pregInfo != NULL)
	{
		delete pregInfo;
		pregInfo = NULL;
	}*/
	//<E>To Fix Cverity #91659,#91660, 2022.02.25,SSDI:Chanchal Singla
	//ths condition is kept because driver Crashing in case of FeatureInfoList is NULL 
	if (pFeatureInfoList == NULL)
	{
		wFlag = TRUE;
	}
	if (wFlag == FALSE)
	{
		//Fixed Coverity #91426,13-08-2021,SSDI:Chanchal Singla
		if (pFeatureInfoList != NULL)
		{
			delete pFeatureInfoList;
			pFeatureInfoList = NULL;

		}
	}
	if (preg != NULL)
	{
		delete preg;
		preg = NULL;
	}
	if (pjsonus != NULL)
	{
		delete pjsonus;
		pjsonus = NULL;
	}
	if (pjsonms != NULL)
	{
		delete pjsonms;
		pjsonms = NULL;
	}
	return blRet;
}
//=============================================================================
// function
//       SetMarginShiftDataToPropState
//
// parameters
//      POEMDEV					pPrivateDevmodeç
//      PPROPSTATE				pPropertySheetStateç
//      WCHAR					pPrinterName
//
// return value
//       TRUE
//      FALSE
//
// outline
//       Set Margin Shift Structure Data to Prostate
//=============================================================================
BOOL CDocumentPropertyPage::SetMarginShiftDataToPropState(POEMDEV pPrivateDevmode, PPROPSTATE  pPropertySheetState, WCHAR FAR *pPrinterName)
{
	BOOL			blRet = FALSE;
	MARGINSHIFTDATA	msd;
	CShRegMS FAR		*pregms = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	CShJsonMS	*pjsonms = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonms = new CShJsonMS(ghInstance, m_pPrinterName);
		pjsonms->Init();
	}

	if (pPropertySheetState == NULL)
		goto EXIT;

	pregms = new CShRegMS(m_hStringResourceHandle, pPrinterName);
	if (pregms == NULL)
	{
		goto EXIT;
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		blRet = (*pjsonms).ReadMSData((*pPropertySheetState).wMargin, &msd);
		if (blRet != TRUE)
		{
			(*pjsonms).resetEx();
			(*pjsonms).reset();
			(*pjsonms).ReadMSData((*pPropertySheetState).wMargin, &msd);
		}
	}
	else
	{
		blRet = (*pregms).ReadMSData(m_pPrinterName, (*pPropertySheetState).wMargin, &msd);
		if (blRet != TRUE)
		{
			(*pregms).resetEx(pPrinterName);
			(*pregms).reset(pPrinterName);
			(*pregms).ReadMSData(pPrinterName, (*pPropertySheetState).wMargin, &msd);
		}
	}

	(*pPropertySheetState).wMarginUnit = (short)msd.dwUnit;
	if ((*pPropertySheetState).wMarginUnit == unit_inch)
	{
		(*pPropertySheetState).dwMarginWidth = msd.dwInch;
	}
	else
	{
		(*pPropertySheetState).dwMarginWidth = msd.dwMilli;
	}
	blRet = TRUE;

EXIT:
	if (pregms != NULL) {
		delete pregms;
		pregms = NULL;
	}
	if (pjsonms != NULL) {
		delete pjsonms;
		pjsonms = NULL;
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
//       SetPPosDataToPropState
//
// parameters
//      POEMDEV					pPrivateDevmodeç
//      PPROPSTATE				pPropertySheetStateç
//      WCHAR					pPrinterName
//
// return value
//       TRUE
//      FALSE
//
// outline
//       Set Print Position Structure Data to Prostate
//=============================================================================
BOOL CDocumentPropertyPage::SetPPosDataToPropState(POEMDEV pPrivateDevmode, PPROPSTATE pPropertySheetState, WCHAR FAR *pPrinterName)
{
	BOOL			blRet = FALSE;
	PRINTPOSITION	ppd;
	CShRegPP FAR		*pregpp = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
	CShJsonPP	*pjsonpp = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonpp = new CShJsonPP(ghInstance, pPrinterName);
		pjsonpp->Init();
	}

	pregpp = new CShRegPP(m_hStringResourceHandle, pPrinterName);
	if (pregpp == NULL)
	{
		goto EXIT;
	}

	if (pPropertySheetState == NULL)
		goto EXIT;


	memset(&ppd, 0x00, sizeof(PRINTPOSITION));

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		blRet = (*pjsonpp).ReadPPData(&ppd);
		if (blRet != TRUE)
		{
			(*pjsonpp).reset();
		}
	}
	else
	{
		blRet = (*pregpp).ReadPPData(pPrinterName, &ppd);
		if (blRet != TRUE)
		{
			(*pregpp).reset(pPrinterName);
		}
	}

	memset(&ppd, 0x00, sizeof(PRINTPOSITION));

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		if ((*pPropertySheetState).wPPosChg == bool_false)
		{
			(*pjsonpp).GetPPDefData(&ppd);
		}
		else
		{
			(*pjsonpp).ReadPPData(&ppd);
		}
	}
	else
	{
		if ((*pPropertySheetState).wPPosChg == bool_false)
		{
			(*pregpp).GetPPDefData(&ppd);
		}
		else
		{
			(*pregpp).ReadPPData(pPrinterName, &ppd);
		}
	}

	(*pPropertySheetState).wPPosUnit = (short)ppd.dwUnit;
	(*pPropertySheetState).lPPosOddX = (long)ppd.wOddX;
	(*pPropertySheetState).lPPosOddY = (long)ppd.wOddY;
	(*pPropertySheetState).lPPosEvnX = (long)ppd.wEvenX;
	(*pPropertySheetState).lPPosEvnY = (long)ppd.wEvenY;

	blRet = TRUE;

EXIT:
	if (pregpp != NULL) {
		delete pregpp;
		pregpp = NULL;
	}
	if (pjsonpp != NULL) {
		delete pjsonpp;
		pjsonpp = NULL;
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
//      SetUSUntitled
//
// parameters
//      blDefault		
//      blAfterLim		
//
// return value
//     void
//
// outline
//      UserSetting SetUSUntitled
//=============================================================================
void CDocumentPropertyPage::SetUSUntitled(POEMDEV pOemPrivateDevMode_pre, POEMDEV pOemPrivateDevMode_Current, PDEVMODE pOemPublicDevMode_Pre, PDEVMODE pOemPublicDevMode_Current, PFEATUREINFOLIST pFeatureInfoList_pre, PFEATUREINFOLIST pFeatureInfoList_current, short wUsIndex_pre,short wUsIndex_Current, BOOL blDefault, BOOL blAfterLim)
{


	if (blAfterLim == TRUE)
		return;

	if (wUsIndex_Current == 0 && blDefault == TRUE)
		return;

	if (wUsIndex_pre != wUsIndex_Current)
		return;


	// ignore the Following features and devmode variables
	{
		SyncIgnoreData(pOemPrivateDevMode_pre, pOemPrivateDevMode_Current);
		UINT TempArray[4] = { FEATURE_RETENTION_PASSWORD,FEATURE_AUTHENTICATION , FEATURE_DEFAULT_JOB_USER_NAME ,FEATURE_DEFAULT_JOB_JOB_NAME };
		size_t nFeatureIndex = 0;
		size_t nNumberOfFeatures = 4;

		for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
		{
			pFeatureInfoList_pre->pFeatureOptionPair[TempArray[nFeatureIndex]].pszOption = pFeatureInfoList_current->pFeatureOptionPair[TempArray[nFeatureIndex]].pszOption;

			pFeatureInfoList_pre->pFeatureOptionsList[TempArray[nFeatureIndex]].uDefaultOptionIndex = pFeatureInfoList_current->pFeatureOptionsList[TempArray[nFeatureIndex]].uDefaultOptionIndex;

			pFeatureInfoList_pre->pFeatureOptionsList[TempArray[nFeatureIndex]].uChangedOptionIndex = pFeatureInfoList_current->pFeatureOptionsList[TempArray[nFeatureIndex]].uChangedOptionIndex;
		}
		CUIControl pControl;
		if ((pControl.IsEqualBuf(pOemPrivateDevMode_pre, pOemPrivateDevMode_Current, sizeof(OEMDEV)) == FALSE)	/*Private devmode*/
			|| (pControl.IsEqualBuf(pOemPublicDevMode_Pre, pOemPublicDevMode_Current, sizeof(DEVMODE)) == FALSE) /*Public devmode*/
			|| (CompareFeatureInfoList(pFeatureInfoList_pre, pFeatureInfoList_current) == FALSE) /*m_pFeatureInfoList*/
			)
		{
			*m_pUsIndex = -1;
		}
	}

}
//=============================================================================
// function
//      SyncIgnoreData
//
// parameters
//     POEMDEV			pOemPrivateDevMode_pre,
//	   POEMDEV			pOemPrivateDevMode_Current
//
// return value
//      êTRUE
//     FALSE
//
// outline
//		SyncIgnoreData
//=============================================================================
BOOL CDocumentPropertyPage::SyncIgnoreData(POEMDEV pOemPrivateDevMode_pre, POEMDEV pOemPrivateDevMode_Current)
{
	BOOL		blRet = FALSE;


	if (pOemPrivateDevMode_pre == NULL || pOemPrivateDevMode_Current == NULL)
		goto EXIT;
	//=================================
	//--- job Control
	//==================================
	pOemPrivateDevMode_pre->scpi.ext.JobCtrl.wFieldJC = pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC;
	// --- dwFieldEDM1
	// --- DM_ALWAYSUSE_ACNUM
	if (pOemPrivateDevMode_pre->scpi.ext.dwFieldEDM1 & DM_TPTEXT)
		pOemPrivateDevMode_Current->scpi.ext.dwFieldEDM1 |= DM_TPTEXT;
	else
		pOemPrivateDevMode_Current->scpi.ext.dwFieldEDM1 &= ~DM_TPTEXT;

	// --- DM_FIRSTPAGE
	if (pOemPrivateDevMode_pre->scpi.ext.dwFieldEDM1 & DM_FIRSTPAGE)
		pOemPrivateDevMode_Current->scpi.ext.dwFieldEDM1 |= DM_FIRSTPAGE;
	else
		pOemPrivateDevMode_Current->scpi.ext.dwFieldEDM1 &= ~DM_FIRSTPAGE;

	// --- DM_OUTLINEONLY
	if (pOemPrivateDevMode_pre->scpi.ext.dwFieldEDM1 & DM_OUTLINEONLY)
		pOemPrivateDevMode_Current->scpi.ext.dwFieldEDM1 |= DM_OUTLINEONLY;
	else
		pOemPrivateDevMode_Current->scpi.ext.dwFieldEDM1 &= ~DM_OUTLINEONLY;

	// --- wFieldJC
	// --- DM_ALWAYSUSE_ACNUM
	if (pOemPrivateDevMode_pre->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM)
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC |= DM_ALWAYSUSE_ACNUM;
	else
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;

	// --- DM_ALWAYSUSE_USERID
	if (pOemPrivateDevMode_pre->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_USERID)
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC |= DM_ALWAYSUSE_USERID;
	else
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_USERID;

	// --- DM_ALWAYSUSE_JOBID
	if (pOemPrivateDevMode_pre->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_JOBID)
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC |= DM_ALWAYSUSE_JOBID;
	else
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_JOBID;

	// --- DM_USE_PIN
	if (pOemPrivateDevMode_pre->scpi.ext.JobCtrl.wFieldJC & DM_USE_PIN)
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC |= DM_USE_PIN;
	else
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_PIN;

	// --- DM_USE_LOGINNAME
	if (pOemPrivateDevMode_pre->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME)
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
	else
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;

	// --- DM_USE_LOGINPASS
	if (pOemPrivateDevMode_pre->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS)
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINPASS;
	else
		pOemPrivateDevMode_Current->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;

	//-- Job name
	memcpy_s(pOemPrivateDevMode_Current->scpi.ext.JobCtrl.szJobname, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname) * sizeof(WCHAR),
		pOemPrivateDevMode_pre->scpi.ext.JobCtrl.szJobname, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname) * sizeof(WCHAR));

	//-- user Name
	memcpy_s(pOemPrivateDevMode_Current->scpi.ext.JobCtrl.szUsername, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername) * sizeof(WCHAR),
		pOemPrivateDevMode_pre->scpi.ext.JobCtrl.szUsername, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername) * sizeof(WCHAR));

	// LOGIN Name
	memcpy_s(pOemPrivateDevMode_Current->scpi.ext.JobCtrl.szLoginName, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName) * sizeof(WCHAR),
		pOemPrivateDevMode_pre->scpi.ext.JobCtrl.szLoginName, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName) * sizeof(WCHAR));

	// User number
	memcpy_s(pOemPrivateDevMode_Current->scpi.ext.JobCtrl.byUserNumber, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byUserNumber) * sizeof(WCHAR),
		pOemPrivateDevMode_pre->scpi.ext.JobCtrl.byUserNumber, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byUserNumber) * sizeof(WCHAR));

	// LOGIN password
	memcpy_s(pOemPrivateDevMode_Current->scpi.ext.JobCtrl.byLoginPassword, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byLoginPassword) * sizeof(WCHAR),
		pOemPrivateDevMode_pre->scpi.ext.JobCtrl.byLoginPassword, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byLoginPassword) * sizeof(WCHAR));

	// retention Password
	memcpy_s(pOemPrivateDevMode_Current->scpi.ext.JobCtrl.byRetentionPassword, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetentionPassword) * sizeof(WCHAR),
		pOemPrivateDevMode_pre->scpi.ext.JobCtrl.byRetentionPassword, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetentionPassword) * sizeof(WCHAR));

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      UpdateFeatureInfoList
//
// parameters
//     PFEATUREINFOLIST pFeatureInfoListTemp,
//		PFEATUREINFOLIST pFeatureInfoListCurrent		
//
// return value
//     PFEATUREINFOLIST
//
// outline
//     UpdateFeatureInfoList
//=============================================================================
PFEATUREINFOLIST CDocumentPropertyPage::UpdateFeatureInfoList(PFEATUREINFOLIST pFeatureInfoListTemp, PFEATUREINFOLIST pFeatureInfoListCurrent)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 0;
	/*Get the GPD feature Count*/
	nNumberOfFeatures = GetGPDFeatureCount();

	for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
	{
		pFeatureInfoListTemp->pFeatureOptionsList[nFeatureIndex].pszFeature = pFeatureInfoListCurrent->pFeatureOptionsList[nFeatureIndex].pszFeature;
		pFeatureInfoListTemp->pFeatureOptionPair[nFeatureIndex].pszFeature = pFeatureInfoListCurrent->pFeatureOptionPair[nFeatureIndex].pszFeature;
		pFeatureInfoListTemp->pFeatureOptionsList[nFeatureIndex].ppszOptions = pFeatureInfoListCurrent->pFeatureOptionsList[nFeatureIndex].ppszOptions;
		pFeatureInfoListTemp->pFeatureOptionPair[nFeatureIndex].pszOption = pFeatureInfoListCurrent->pFeatureOptionPair[nFeatureIndex].pszOption;
		pFeatureInfoListTemp->pFeatureOptionsList[nFeatureIndex].uDefaultOptionIndex = pFeatureInfoListCurrent->pFeatureOptionsList[nFeatureIndex].uDefaultOptionIndex;
		pFeatureInfoListTemp->pFeatureOptionsList[nFeatureIndex].uChangedOptionIndex = pFeatureInfoListCurrent->pFeatureOptionsList[nFeatureIndex].uChangedOptionIndex;

		pFeatureInfoListTemp->pFeatureOptionsList[nFeatureIndex].dwOptions = m_pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].dwOptions;
	}
	return pFeatureInfoListTemp;
}
//=============================================================================
// function
//      InitFeatureInfoList
//
// parameters
//      PFEATUREINFOLIST pFeatureInfoListTemps	
//
// return value
//     PFEATUREINFOLIST
//
// outline
//     InitFeatureInfoList
//=============================================================================
PFEATUREINFOLIST CDocumentPropertyPage::InitFeatureInfoList(PFEATUREINFOLIST pFeatureInfoListTemp)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 0;
	/*Get the GPD feature Count*/
	nNumberOfFeatures = GetGPDFeatureCount();
	pFeatureInfoListTemp = new FEATUREINFOLIST();
	if (NULL == pFeatureInfoListTemp)
	{
		slog.putLog("CDocumentPropertyPage::InitFeatureInfoList -> NULL_POINTER(pFeatureInfoList)\n");
	}

	/*intialize to Zero pFeatureInfoList*/
	memset(pFeatureInfoListTemp, 0, sizeof(FEATUREINFOLIST));

	/*Update pFeatureInfoList nNummberOfFeatures with Total numbers of feature availbale in GPD file*/
	pFeatureInfoListTemp->nNummberOfFeatures = nNumberOfFeatures;
	pFeatureInfoListTemp->pFeatureOptionsList = new FEATUREOPTIONSLIST[nNumberOfFeatures];
	if (NULL == pFeatureInfoListTemp->pFeatureOptionsList)
	{
		slog.putLog("CDocumentPropertyPage::InitFeatureInfoList -> NULL_POINTER(pFeatureInfoList->pFeatureOptionsList)\n");
	}
	pFeatureInfoListTemp->pFeatureOptionPair = new PRINT_FEATURE_OPTION[nNumberOfFeatures];
	if (NULL == pFeatureInfoListTemp->pFeatureOptionPair)
	{
		slog.putLog("CDocumentPropertyPage::InitFeatureInfoList -> NULL_POINTER(pFeatureInfoList->pFeatureOptionPair)\n");
	}
	/*intialize to Zero FEATUREOPTIONSLIST and PRINT_FEATURE_OPTION*/
	memset(pFeatureInfoListTemp->pFeatureOptionsList, 0, nNumberOfFeatures * sizeof(FEATUREOPTIONSLIST));
	memset(pFeatureInfoListTemp->pFeatureOptionPair, 0, nNumberOfFeatures * sizeof(PRINT_FEATURE_OPTION));
	return pFeatureInfoListTemp;
}
//=============================================================================
// function
//      CompareFeatureInfoList
//
// parameters
//      PFEATUREINFOLIST pFeatureInfoListTemps	
//
// return value
//     BOOL
//
// outline
//     CompareFeatureInfoList
//=============================================================================
BOOL CDocumentPropertyPage::CompareFeatureInfoList(PFEATUREINFOLIST pFeatureInfoListTemp, PFEATUREINFOLIST pFeatureInfoListCurrent)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 0;
	/*Get the GPD feature Count*/
	nNumberOfFeatures = GetGPDFeatureCount();
	DWORD blRet = TRUE;
	for (UINT uFeatureIndex = 0; uFeatureIndex < nNumberOfFeatures; uFeatureIndex++)
	{
		if (pFeatureInfoListTemp->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex != pFeatureInfoListCurrent->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex)
		{
			blRet = FALSE;
			break; 
		}
	}
	return blRet;
}
//=============================================================================
// function
//      OnUsSave
//
// parameters
//        
//
// return value
//      ê TRUE
//      FALSE
//
// outline
//      OnUsSave
//=============================================================================
BOOL CDocumentPropertyPage::OnUsSave(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL			blRet = FALSE;
	INT_PTR			DlgRet = IDCANCEL;
	BOOL			blOverWrite = FALSE;

	WCHAR			*pszName = NULL;
	CDevModeUpDate *devmodeupdate = NULL;
	long			lIndex = -1;
	long			lMax = 0;
	long			lCount = 0;
	//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	BOOL			bShare = FALSE;
	WCHAR			szTextHKLMW[REG_ENT_SHARE_KEYSIZEW] = { 0 };
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	CShRegUS		*preg = NULL;
	CUPSReg pregUps(m_hStringResourceHandle, m_pPrinterName);
	UINT FeatureCount = (UINT)m_pFeatureInfoList[0].nNummberOfFeatures;
	PPROPSTATE		pps = NULL;
	PSCDM			pscdm = NULL;

	HWND			hDlg_t = NULL;

	WCHAR Buffer[50] = {};
	WCHAR *String = NULL;
	GPDFEATUREOPTIONLIST pFeatureList;
	SecureZeroMemory(&pFeatureList, sizeof(GPDFEATUREOPTIONLIST));
	WCHAR			*p = NULL;
	DLGTEMPLATE		*pdt;
	RECT			rc;
	long			lBase; 
	// Temporary Private Devmode
	OEMDEV wTempOemPrivateDevMode;
	SecureZeroMemory(&wTempOemPrivateDevMode, sizeof(wTempOemPrivateDevMode));
	CShDlgUserSet *pdlg = NULL;
	BOOL			*pblLimitByOptionTbl = NULL;
	UIControlMap::const_iterator Iterator;
	CShCmbUserset *pCmbUserset = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	CShJsonUS			*pjsonus = NULL;
	CShJsonUserPSize	*pjsonups = NULL;

	if (m_ppi == NULL)
		goto EXIT;

	Iterator = m_UIControls.find(IDC_CMB_SHARED_USRSET);
	if (Iterator != m_UIControls.end())
	{
		pCmbUserset = static_cast<CShCmbUserset*>(Iterator->second);
		if (pCmbUserset == NULL)
		{
			slog.putLog("CDocumentPropertyPage::OnUsSelect -> NULL_OBJECT(pCmbUserset)\n");
		}
	}
	else
	{
		goto EXIT;
	}

	lMax = 50;

	preg = new CShRegUS(ghInstance, m_pPrinterName, m_hStringResourceHandle, NULL, m_pFeatureInfoList, m_pOemPrivateDevMode);;
	if (preg == NULL)
	{
		goto EXIT;
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonus = new CShJsonUS(ghInstance, m_pPrinterName, m_hStringResourceHandle);
		pjsonus->Init();
		if (pjsonus == NULL)
			goto EXIT;
		pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
		pjsonups->Init();
	}


	if ((*m_pmcf).IsWriteToJson() == TRUE)
		lCount = (*pjsonus).ReadCount();
	else
		lCount = (*preg).ReadCount(m_pPrinterName);

	if (lCount > lMax)
	{
	
		if ((*pCmbUserset).IsExistLimitByOptionTblTab() == TRUE)
		{
			pblLimitByOptionTbl = (*pCmbUserset).getLimitByOptionTblAddrTab();

			if ((*m_pmcf).IsWriteToJson() == TRUE)
			{
				(*pjsonus).DeleteDataLimitByOption(pblLimitByOptionTbl);
				(*pjsonus).WriteJsonDataToFile();
			}
			else
			{
				(*preg).DeleteDataLimitByOption(pblLimitByOptionTbl);
			}
			(*pCmbUserset).deleteAllLimitByOptionTblTab();
		}
		else {
		
			(*m_ObjCShowMessage).Message(hDlg , IDS_MESTITLE_1, IDS_MESSAGE_45, MB_ICONINFORMATION | MB_OK);
			goto EXIT;
		}
	
	}

	// ================================================
	// _/_/_/  UserSetting
	// ================================================

	pdlg = new CShDlgUserSet(IDD_DLG_USERSET, m_hStringResourceHandle, m_hPropertySheetHandle, m_hPrinterHandle, m_pPrinterName, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings,
		m_hBitmapResourceHandle, m_pOemPublicDevMode,m_pOemPrivateDevMode, &m_PropertySheetState, m_ObjCShowMessage, m_pWaterMarkIndex, m_ppi);

	if (NULL == pdlg)
	{
		slog.putLog("CLayoutTabPropertyPage::OnPrintPositionDlg -> NULL_POINTER(IDD_DLG_PRINTPOSITON)\n");
		return 0;
	}
	pdlg->SetPrinterName(m_pPrinterName);
	pszName = new WCHAR[(SCUI_USTITLE_SAVE_MAX + 1) * 2];
	if (pszName == NULL)
	{
		slog.putLog("CLayoutTabPropertyPage::OnPrintPositionDlg -> NULL_POINTER(IDD_DLG_PRINTPOSITON)\n");
		return 0;
	}
	SecureZeroMemory(pszName, (SCUI_USTITLE_SAVE_MAX + 1) * 2);
	pdlg->SetBuffer(pszName, (SCUI_USTITLE_SAVE_MAX + 1) * 2);

	::GetClientRect(hDlg, &rc);

	lBase = GetDialogBaseUnits();

	p = new WCHAR[sizeof(DLGTEMPLATE) + 10];
	if (p == NULL)
		return 0;
	pdt = (DLGTEMPLATE *)p;

	SecureZeroMemory(p, sizeof(DLGTEMPLATE) + 10);


	lBase = GetDialogBaseUnits();
	(*pdt).style = WS_POPUP | WS_VISIBLE;
	(*pdt).dwExtendedStyle = 0;
	(*pdt).cdit = 0;
	(*pdt).x = 0;
	(*pdt).y = 0;
	(*pdt).cx = (short)::MulDiv((rc.right - rc.left), 4, LOWORD(lBase));
	(*pdt).cy = (short)::MulDiv((rc.bottom - rc.top), 8, HIWORD(lBase));
	
	hDlg_t = ::CreateDialogIndirectParam(ghInstance, pdt, (*m_ppi).hWnd_Tab, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
	::EnableWindow(::GetParent((*m_ppi).hWnd_Tab), FALSE);

	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_USERSET),
		hDlg_t, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);

	if (IDCANCEL != DlgRet)
	{
		//Test Code for Checking the String is Comming or Not
	/*	String = pdlg->GetString();
		if (String != NULL)
		{
			wcscpy(Buffer, String);
		}	*/
	}
	else
	{
		goto EXIT;
	}

	// ================================================
	// _/_/_/  èSearchUSData
	// ================================================
	lIndex = SearchUSData(pszName);
	if (lIndex != CB_ERR)
	{
		
		if ((*pCmbUserset).chkLimitByOptionTblTab((DWORD)lIndex) != FALSE) {
			
			if ((*m_pmcf).IsWriteToJson() == TRUE)
				(*pjsonus).DeleteData(lIndex);
			else
				(*preg).DeleteData(lIndex);

			(*pCmbUserset).deleteLimitByOptionTblTab(lIndex);
			lIndex = -1;
		}
		else {
			if ((*m_ObjCShowMessage).Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_42, MB_ICONINFORMATION | MB_OKCANCEL) == IDCANCEL) {
				goto EXIT;
			}
			blOverWrite = TRUE;
		}
		
	}

	// ================================================
	// _/_/_/  
	// ================================================
	pscdm = new SCDM;
	if (pscdm == NULL)
		return 0;

	memcpy_s(&wTempOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));
	
	//<S><C>To Fix Redmine Bug #3681,2021.11.29,SSDI:Chanchal Singla
	///Custom Paper values are changing to Default even though cancel button is clicked
	//CUPSReg pregUps(m_hStringResourceHandle, m_pPrinterName);
	//pregUps.WriteUPSData(m_pPrinterName, DMPAPER_CUSTOM, &m_ppi->PaperSize_upsd);
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonups->WriteUPSData(DMPAPER_CUSTOM_ZOOM, &m_ppi->FitToPaperSize_upsd);
	}
	else
	{
		pregUps.WriteUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &m_ppi->FitToPaperSize_upsd);
	}
	//<E>To Fix Redmine Bug #3681,2021.11.29,SSDI:Chanchal Singla
	
	//<S><A>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
	//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
	MIP_MINMAX_CONV_PARAM stCnvParam;
	SecureZeroMemory(&stCnvParam, sizeof(MIP_MINMAX_CONV_PARAM));
	stCnvParam.paper = mip_paper_cus_usr;
	stCnvParam.mi = mip_mi_mill;
	stCnvParam.pos = 1;

	if (m_PropertySheetState.wCusUnit == unit_inch)
	{
		stCnvParam.wl = mip_wl_width;
		wTempOemPrivateDevMode.scpi.ext.dwCusWidthPixel = InchToPixel_CusPaper(m_PropertySheetState.dwCustomWidth, wTempOemPrivateDevMode.scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
		stCnvParam.wl = mip_wl_length;
		wTempOemPrivateDevMode.scpi.ext.dwCusLengthPixel = InchToPixel_CusPaper(m_PropertySheetState.dwCustomLength, wTempOemPrivateDevMode.scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);

	}
	else
	{
		long	lInchWA;
		long	lInchLA;
		stCnvParam.mi = mip_mi_inch;
		stCnvParam.pos = 10;
		stCnvParam.wl = mip_wl_width;
		lInchWA = MilliToInchNoChk_CusPaper(m_PropertySheetState.dwCustomWidth, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);
		stCnvParam.wl = mip_wl_length;
		lInchLA = MilliToInchNoChk_CusPaper(m_PropertySheetState.dwCustomLength, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);

		stCnvParam.mi = mip_mi_inch;
		stCnvParam.pos = 10;
		stCnvParam.wl = mip_wl_width;
		wTempOemPrivateDevMode.scpi.ext.dwCusWidthPixel = (DWORD)InchToPixel_CusPaper(lInchWA, wTempOemPrivateDevMode.scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
		stCnvParam.wl = mip_wl_length;
		wTempOemPrivateDevMode.scpi.ext.dwCusLengthPixel = (DWORD)InchToPixel_CusPaper(lInchLA, wTempOemPrivateDevMode.scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);

	}
	//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla


	devmodeupdate = new CDevModeUpDate(&wTempOemPrivateDevMode, m_PaperInfo, m_pFeatureInfoList->pFeatureOptionPair, FeatureCount,
		m_hPrinterHandle, m_hStringResourceHandle, GetResourceIDInfoOfControl(PAPERSIZE), m_pPrinterName, &m_ppi->PaperSize_upsd, &m_ppi->FitToPaperSize_upsd);
	devmodeupdate->UpDateDevMode();

	::memcpy(pscdm, &wTempOemPrivateDevMode.scpi, sizeof(SCDM));
	Update_FeatureInfoList_TO_GPDFeatureOptionList(m_pFeatureInfoList, &pFeatureList);
	if ((*m_pmcf).IsWriteToJson() == FALSE)
	{
		lIndex = (*preg).WriteData(m_pPrinterName, pszName, &pFeatureList, pscdm, lIndex, blOverWrite);
		(*preg).WriteEachData(m_pPrinterName, pscdm, m_pFeatureInfoList, lIndex);
	}
	else
	{
		lIndex = (*pjsonus).WriteData(pszName, pscdm, &pFeatureList, lIndex, blOverWrite);
		(*pjsonus).WriteEachData( pscdm, m_pFeatureInfoList, lIndex);
	}
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		(*pjsonus).WriteJsonDataToFile();
	}

	if (lIndex != -1)
		*m_pUsIndex = (short)lIndex;

	if (pCmbUserset)	//<S><A> Fix for coverity issue 92768. Harika 20230331
	{
		(*pCmbUserset).addLimitByOptionTblTab(*m_pUsIndex, blOverWrite);

		// --- user setting
		(*pCmbUserset).OnInit(IDC_CMB_SHARED_USRSET, hDlg);
	}



	//enable apply Button 
	{
		CUIControl pControl;
		pControl.EnableApplyButton(hDlg);
	}
	
	//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	preg->ReadShareDayTimeFuncFromHKLM(m_pPrinterName, szTextHKLMW, REG_ENT_SHARE_KEYSIZEW);

	if (wcslen(szTextHKLMW) > 0)
	{
		bShare = preg->GetValidFlag(szTextHKLMW);
	}

	if (bShare == TRUE)
	{
		preg->WriteFavItemsFromHKCUToHKLM(m_pPrinterName);
	}
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	blRet = TRUE;
	//reverted coverity  91445 to Fix Redmine  #3757,2021--May-10,SSDI: Chanchal Singla
	// Coverity Fix: 91445 - SSDI Seetharam - 20210504
	//Moved this line from after EXIT
	//::EnableWindow(::GetParent((*m_ppi).hWnd_Tab) , TRUE);

EXIT:
	// Coverity Fix: 91445 - SSDI Chanchal Singla - 20210603
	if (NULL != m_ppi)
	{
		::EnableWindow(::GetParent((*m_ppi).hWnd_Tab), TRUE);
	}
	if (hDlg_t != NULL) {
		::DestroyWindow(hDlg_t);
	}

	if (p != NULL)
	{
		delete[] p;
		p = NULL;
	}
		

	if (pscdm != NULL)
	{
		delete pscdm;
		pscdm = NULL;
	}

	if (pszName != NULL)
	{
		delete[] pszName;
		pszName = NULL;
	}
	if (preg != NULL)
	{
		delete preg;
		preg = NULL;
	}
	if (pjsonus != NULL)
	{
		delete pjsonus;
		pjsonus = NULL;
	}
	if (pjsonups != NULL)
	{
		delete pjsonups;
		pjsonups = NULL;
	}
	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = NULL;
	}
	
	if (devmodeupdate != NULL)
	{
		delete devmodeupdate;
		devmodeupdate = NULL;
	}
	if (pdlg != NULL)
	{
		delete pdlg;
		pdlg = NULL;
	}
	return blRet;
}


//=============================================================================
// function
//      OnUsDelete
//
// parameters
//      ñ≥Çµ
//
// return value
//      TRUE
//      FALSE
//
// outline
//      
//=============================================================================
BOOL CDocumentPropertyPage::OnUsDelete(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL			blRet = FALSE;

	short			wCount = 0;
	CShRegUS		*preg = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	CShJsonUS			*pjsonus = NULL;

	PPROPSTATE		pps;
//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	BOOL			bShare = FALSE;
	WCHAR			szTextHKLMW[REG_ENT_SHARE_KEYSIZEW] = { 0 };
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	UIControlMap::const_iterator Iterator;
	CShCmbUserset *pCmbUserset = NULL;
	if (m_ppi == NULL)
		goto EXIT;

	Iterator = m_UIControls.find(IDC_CMB_SHARED_USRSET);
	if (Iterator != m_UIControls.end())
	{
		pCmbUserset = static_cast<CShCmbUserset*>(Iterator->second);
		if (pCmbUserset == NULL)
		{
			slog.putLog("CDocumentPropertyPage::OnUsSelect -> NULL_OBJECT(pCmbUserset)\n");
		}
	}
	else
	{
		goto EXIT;
	}
	preg = new CShRegUS(ghInstance, m_pPrinterName, m_hStringResourceHandle, NULL, m_pFeatureInfoList, m_pOemPrivateDevMode);;
	if (preg == NULL)
	{
		goto EXIT;
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonus = new CShJsonUS(ghInstance, m_pPrinterName, m_hStringResourceHandle);
		pjsonus->Init();
		if (pjsonus == NULL)
			goto EXIT;
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
		(*pjsonus).DeleteData(*m_pUsIndex);
	else
		(*preg).DeleteData(*m_pUsIndex);

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		(*pjsonus).WriteJsonDataToFile();
	}

	if (pCmbUserset)	//<S><A> Fix for Coverity issue 92765. Harika 20230331
	{
		(*pCmbUserset).deleteLimitByOptionTblTab(*m_pUsIndex);



	
		// --- user setting
		(*pCmbUserset).OnInit(IDC_CMB_SHARED_USRSET, hDlg);
	}

	*m_pUsIndex = -1;

	//enable apply Button 
	{
		CUIControl pControl;
		pControl.EnableApplyButton(hDlg);
	}
//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		(*pjsonus).WriteJsonDataToFile();
	}

	preg->ReadShareDayTimeFuncFromHKLM(m_pPrinterName, szTextHKLMW, REG_ENT_SHARE_KEYSIZEW);

	if (wcslen(szTextHKLMW) > 0)
	{
		bShare = preg->GetValidFlag(szTextHKLMW);
	}

	if (bShare == TRUE)
	{
		preg->WriteFavItemsFromHKCUToHKLM(m_pPrinterName);
	}
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	blRet = TRUE;

EXIT:

	if (pjsonus != NULL)
	{
		delete pjsonus;
		pjsonus = NULL;
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
//      SearchUSData
//
// parameters
//      pszTitle            É^ÉCÉgÉãÇÃÉAÉhÉåÉX
//
// return value
//      ê¨å˜ÅFÉÜÅ[ÉUÅ[ÉZÉbÉeÉBÉìÉOÇÃÉCÉìÉfÉbÉNÉX
//      é∏îsÅF-1
//
// outline
//      É^ÉCÉgÉãñºÇ∆àÍívÇ∑ÇÈÉÜÅ[ÉUÅ[ÉZÉbÉeÉBÉìÉOÇéÊìæÇµÇ‹Ç∑
//=============================================================================
short CDocumentPropertyPage::SearchUSData(WCHAR *pszTitle)
{
	short			wRet = -1;

	long			lCount;
	long			i;

	WCHAR 	*pszExist = NULL;

	CShRegUS		*preg = NULL;
	CShJsonUS		*pjsonus = NULL;
	vector<Json::Value> vecValue;
	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	// ================================================
	// _/_/_/  pszTitle
	// ================================================
	if (pszTitle == NULL)
		goto EXIT;


	preg = new CShRegUS(ghInstance, m_pPrinterName, m_hStringResourceHandle, NULL, m_pFeatureInfoList, m_pOemPrivateDevMode);;
	if (preg == NULL)
	{
		goto EXIT;
	}


	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonus = new CShJsonUS(ghInstance, m_pPrinterName, m_hStringResourceHandle);
		pjsonus->Init();
		if (pjsonus == NULL)
			goto EXIT;
	}


	pszExist = new WCHAR[(SCUI_TITLE_READ_MAX + 1) * 2];
	if (pszExist == NULL)
		goto EXIT;

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{

		//lCount = (*pjsonus).ReadCount();//bear
		(*pjsonus).GetChildItemsValue(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_TITLE, vecValue);

		for (i = 0; (DWORD)i < vecValue.size(); i++)
		{
			SecureZeroMemory(pszExist, (SCUI_TITLE_READ_MAX + 1) * 2);
			//if ((*pjsonus).GetTitle(pszExist, (SCUI_TITLE_READ_MAX + 1) * 2, i) == TRUE)
			//{
			string strTitleInVec = vecValue[i].asString();

			wstring wszUnicode = UTF8ToUnicode(strTitleInVec.c_str());
			DWORD dwSize = (wszUnicode.length() + 1) * sizeof(wchar_t);
			memcpy(pszExist, wszUnicode.c_str(), dwSize);

				if (::_wcsicmp((const wchar_t *)pszExist, (const wchar_t *)pszTitle) == 0)
					break;
			//}
		}


		if ((DWORD)i < vecValue.size())
			wRet = (short)i;

	}
	else
	{
		lCount = (*preg).ReadCount(m_pPrinterName);
		for (i = 1; i < lCount; i++)
		{
			SecureZeroMemory(pszExist, (SCUI_TITLE_READ_MAX + 1) * 2);
			if ((*preg).GetTitle(m_pPrinterName, pszExist, (SCUI_TITLE_READ_MAX + 1) * 2, i) == TRUE)
			{
				if (::_wcsicmp((const wchar_t *)pszExist, (const wchar_t *)pszTitle) == 0)
					break;
			}
		}


		if (i < lCount)
			wRet = (short)i;
	}

EXIT:
	//<S><A>Coverity fix 91326 - SSDI-Seetharam:20210428
	if (preg != NULL)
	{
		delete preg;
		preg = NULL;
	}
	//<S><E>Coverity fix 91326 - SSDI-Seetharam:20210428

	if (pszExist != NULL)
		delete[] pszExist;

	if (pjsonus != NULL)
	{
		delete pjsonus;
		pjsonus = NULL;
	}
	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = NULL;
	}

	return wRet;

}
//=============================================================================
// function
//      CompareCurrentSettingVsConfigTabData
//
// parameters
//      PSCDM pscdm1,
//		PSCDM pscdm2,
//		PFEATUREINFOLIST pFeatureInfoListTemp,
//		PFEATUREINFOLIST pFeatureInfoListCurrent	
//
// return value
//     BOOL
//
// outline
//     CompareCurrentSettingVsConfigTabData
//=============================================================================
BOOL CDocumentPropertyPage::CompareCurrentSettingVsConfigTabData(PSCDM pscdm1, PSCDM pscdm2, PFEATUREINFOLIST pFeatureInfoList, PFEATUREINFOLIST pFeatureInfoListCurrent,  SHORT wIndex)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL blRet = FALSE;
	UINT lVal = FALSE;
	CShLimit limit(NULL, NULL);

	//<S><A>To Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
	//ìThis Favorite is not available on current Configuration Settingî message is not displayed
	PROPSTATE PropertySheetState;
	SecureZeroMemory(&PropertySheetState,sizeof(PROPSTATE));
	SetRegJCInfoToPropState(&m_pOemPrivateDevMode->scpi, &PropertySheetState, m_pPrinterName);
	//<E>To Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
	eDeviceType sDevicetype = eColor;
	CRegistryAPI cRegApi;
	cRegApi.GetDeviceType(m_pPrinterName, sDevicetype);

	//b&w in config & color/automatic in preference
	if ((m_PropertySheetState.wColorOption == colmode_mono) && (pscdm1->ext.ColData.byColMode != colmode_mono))	
	{
		blRet = TRUE;
		goto EXIT;
	}
	//Auto ColorMode
	if (pscdm1->ext.ColData.byColMode == colmode_auto && 1 == m_PropertySheetState.byAutoColorMode)
	{
		blRet = TRUE;
		goto EXIT;
	}

	//MaxPaperSize
	if (m_PropertySheetState.byMaxPaperSize ==  eSecondOption)
	{
		if (!strcmp("Standard", pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
		{
			lVal = pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex;

			if (limit.limCmbBookletStdPSize(&m_PropertySheetState, lVal) == TRUE)
			{
				blRet = TRUE;
				goto EXIT;
			}
		}
		else
		{
			lVal = pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
			if (limit.limCmbFitToPaperSizeVsMaxPSize(&m_PropertySheetState, (short)lVal, pFeatureInfoList) == TRUE)
			{
				blRet = TRUE;
				goto EXIT;
			}
			//<S><A>To Fix redmine Bug #4649 issue 4,25.02.2022,SSDI:Chanchal Singla
			//ìThis Favorite is not available on current Configuration Settingî message is not displayed.
			else if ((lVal == 21) && (m_PropertySheetState.byMaxPaperSize == eSecondOption))	//custom
			{
				USERPAPERSIZEDATA upsd;
				SecureZeroMemory(&upsd, sizeof(upsd));
				CUPSReg pregUps(m_hStringResourceHandle, m_pPrinterName);

				CShJsonUserPSize	*pjsonups = NULL;
				CShIniFile			*m_pmcf = NULL;
				TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
				GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
				m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);

				if ((*m_pmcf).IsWriteToJson() == TRUE)
				{
					pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
					pjsonups->Init();
				}

				DWORD dwLength = 0, dwWidth = 0;
				short wUnit = 0;
				//pregUps.ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &upsd);
				if ((*m_pmcf).IsWriteToJson() == TRUE)
					pjsonups->ReadUPSDataToUs((LONG)wIndex, DMPAPER_CUSTOM_ZOOM, &upsd);
				else
					pregUps.ReadUPSDataToUs(m_pPrinterName, (LONG)wIndex, DMPAPER_CUSTOM_ZOOM, &upsd);

				if (upsd.wUnit == unit_inch)	//1
				{
					dwWidth = upsd.dwWidth * 2.54 + 1;
					dwLength = upsd.dwLength * 2.54 + 4;
				}
				else
				{
					dwWidth = upsd.dwWidth;
					dwLength = upsd.dwLength;
				}
				if ((dwWidth > 2160) || (dwLength > 3560))	//max length:356 max width: 216   
				{
					blRet = TRUE;
					goto EXIT;
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
			//<E>To Fix redmine Bug #4649 issue 4,25.02.2022,SSDI:Chanchal Singla
		}
	}
	//ByPrintRelease
	lVal = pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex;
	if ((m_PropertySheetState.byPrintRelease == bool_true && lVal == bool_true) 
		|| (m_PropertySheetState.wPPlcyRetention != bool_false && lVal == bool_true))
	{
		blRet = TRUE;
		goto EXIT;
	}
	//<S><C>To Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
	//ìThis Favorite is not available on current Configuration Settingî message is not displayed
	//PPDocumentFiling
	SHORT wPPlcyRetention = pscdm1->dev.dwFieldVDM & DM_PP_RETENTION ? bool_true : bool_false;
	if ((m_PropertySheetState.wPPlcyRetention == bool_true) && (wPPlcyRetention == bool_false)
		&& (PropertySheetState.wUsePIN == bool_true))
	{
		blRet = TRUE;
		goto EXIT;
	}
	//<E>To Fix Redmine Bug #4275(2021.September.14) and Bug #4649 issue 3(2021.Feb.04),SSDI:Chanchal Singla
	//<S><A>To Fix redmine Bug #4649,03.01.2022,SSDI:Chanchal Singla
	//ìThis Favorite is not available on current Configuration Settingî message is not displayed.
	//<S><C>To Fix Redmine Task#7720,13-09-2024,SSDI:Jyothi N
	//<S><C>To Fix Redmine Task#7720,08-08-2024,SSDI:Jyothi N
	if (lVal && wPPlcyRetention)
	{
		blRet = TRUE;
		goto EXIT;
	}
	//if ((m_PropertySheetState.wPPlcyPrintRelease == bool_true) && (m_PropertySheetState.byPrintRelease == bool_false))
	if ((m_PropertySheetState.wPPlcyPrintRelease == bool_true) && (m_PropertySheetState.byPrintRelease == bool_false)
		&& ((pscdm1->ext.JobCtrl.byRetention != retention_normal) && wPPlcyRetention == bool_false))
	//<E>To Fix Redmine #7720,08-08-2024,SSDI:Jyothi N
	//<E>To Fix Redmine #7720,13-09-2024,SSDI:Jyothi N
	{
		blRet = TRUE;
		goto EXIT;
	}
	//E>To Fix redmine Bug #4649,03.01.2022,SSDI:Chanchal Singla
	//<S><A>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	//Change the specification for function limitation, display method by changing option
	if (limit.limStapleOptVsStapleType(&m_PropertySheetState, pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex) != FALSE)
	{
		blRet = TRUE;
		goto EXIT;
	}
	if (limit.limPunchOptVsPunchType(&m_PropertySheetState, pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex) != FALSE)
	{
		blRet = TRUE;
		goto EXIT;
	}
	//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	//<S><A>To Fix Redmine #7267,03-07-2024,SSDI:Manoj S
	BYTE oldStapleFlag = false,CurrentFlag =false;
	if ((pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex == 0) && (pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex == 3))
	{
		oldStapleFlag = true;
	}
	ReadStapleLessFlag(CurrentFlag);
	if (oldStapleFlag & CurrentFlag)
	{
		if (pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex != pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex)
		{
			blRet = TRUE;
			goto EXIT;
		}
	}
	//<E>To Fix Redmine #7267,03-07-2024,SSDI:Manoj S
EXIT:
	return blRet;
}
//<S><E> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla

BOOL CDocumentPropertyPage::DisableAllCtrls(short *pwExcept)
{
	::EnumChildWindows(m_hPropertySheetHandle, (WNDENUMPROC)CShTabDlg_DisableProc, (LPARAM)pwExcept);

	return TRUE;
}
//=============================================================================
// function
//      OnPaperTypeChanged
//
// parameters
//     HWND
//
// return value
//      DWORD 
//
// outline
//      OnPaperTypeChanged
//=============================================================================
DWORD CDocumentPropertyPage::OnPaperTypeChanged(HWND hDlg , WORD iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	long wOutputSize = 0;
	BOOL blRet = FALSE;
	LONG wPaper = 0;
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	LONG wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	LONG wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	LONG wPunch = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex;
	long wBindedge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	UINT wPreOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;

	CShLimit	plim(NULL, NULL);
	//OutPutSize
	UIControlMap::const_iterator Iterator;
	Iterator = m_UIControls.find(IDC_CMB_OUTPUTSIZE);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			wOutputSize = (short)pImageCombo->GetVal(IDC_CMB_OUTPUTSIZE, hDlg);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::ShowStapleVsOriginalSizeVsOutputSizeInformationIcon -> NULL_OBJECT(pImageCombo)\n");
		}
	}
	else
	{
		wOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	}

	//PaperSize
	Iterator = m_UIControls.find(IDC_CMB_PAPER);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			wPaper = (short)pImageCombo->GetVal(IDC_CMB_PAPER, hDlg);
		}
		else
		{
			slog.putLog("CDocumentPropertyPage::ShowStapleVsOriginalSizeVsOutputSizeInformationIcon -> NULL_OBJECT(pImageCombo)\n");
		}
	}
	else
	{
		wPaper = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	}

	//Restriction Between Staple,OutputSize,PaperSize
	if (plim.limInfoMsgOutputSizeVsStaple(&m_PropertySheetState, m_pFeatureInfoList, wStaple,
		wOutputSize, wPreOutputSize, wPaper, wBooklet, wBindedge,iCtrlID) == TRUE)
	{
		blRet = TRUE;
	}

	//Restriction Between Punch,OutputSize,PaperSize
	if (plim.limInfoMsgOutputSizeVsPunch(&m_PropertySheetState, m_pFeatureInfoList, wPunch,
		wOutputSize, wPreOutputSize, wPaper, wBooklet, wBindedge, iCtrlID) == TRUE)
	{
		blRet = TRUE;
	}
	return blRet;
}

void CDocumentPropertyPage::SelectOptionsonUI(HWND   hDlg, POEMDEV m_pOemPrivateDevMode)
{
	//<S><A>To Fix Redmine Bug #4701-Issue 2,2022.03.23,SSDI:Chanchal Singla
	//Constraints are not updated correctly.
	LONG wPaperSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	LONG wOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	if (((wPaperSize >= PAPERSIZE_A3 && wPaperSize <= PAPERSIZE_LEGAL) && (wOutputSize == OUTPUTPAPERSIZE_SAOS))
		&& ((!strcmp("ENVELOPE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption))
			|| (!strcmp("JAPANESEPOSTCARD", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption))))
	{
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = eFirstOption;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "AUTOSELECT";
	}
	//<S><D> For Small paper size documents, need to display "None" for Duplex, But featureinfoList should not set to "None". This is to match with UD3. Harika 20230214
	/*if (((wPaperSize >= PAPERSIZE_EXECUTIVE && wPaperSize <= PAPERSIZE_JAPANESE_POSTCARD) && (wOutputSize == OUTPUTPAPERSIZE_SAOS))
		&& ((!strcmp("VERTICAL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
			|| (!strcmp("HORIZONTAL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))))
	{
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex = eFirstOption;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption = "NONE";
	}*/
	//<E> For Small paper size documents, need to display "None" for Duplex, But featureinfoList should not set to "None". This is to match with UD3. Harika 20230214
	//<E>To Fix Redmine Bug #4701-Issue 2,2022.03.23,SSDI:Chanchal Singla
	//<S><A>To Fix RedmineBug #4510- issue 2 and Issue 4,2021.12.03,SSDI:Chanchal Singla
	//Function limitation is not proper when Staple is set in Office application
	//Issue 2:- 5.6.2 Staple is not restored to None when Staple setting is done in Office application
	//Issue 4:- 5.6.4 Staple is not changed to 2 - Staples
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		//<S><A> to Fix redmine Bug #4637,2021.12.22,SSDI:ChanChal Singla
		//Collate value changes even when drop down is disabled
		if (!strcmp("NONE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
		{
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE].uChangedOptionIndex = eFirstOption;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLLATE].pszOption = "ON";
		}
		if ((!strcmp("VERTICAL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
				||   (!strcmp("HORIZONTAL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption)))
		{
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex = eFirstOption;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption = "None";
		}
		//<E> to Fix redmine Bug #4637,2021.12.22,SSDI:ChanChal Singla
		if ((m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_SADDLE))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption = "STAPLE_SADDLE";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex = staple_saddle_staple;
		}
		else
		{
			m_pOemPrivateDevMode->scpi.ext.byStaple = STAPLE_NONE;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption = "NONE";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex = staple_none;
		}
	}
	if ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "TRANSPARENCY1")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "ENVELOPE")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "JAPANESEPOSTCARD")
		|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "LABEL"))
		&& (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_SADDLE))
	{
		m_pOemPrivateDevMode->scpi.ext.byStaple = STAPLE_TWO;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption = "STAPLE_2";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex = staple_2;
	}
	//E>To Fix RedmineBug #4510- issue 2 and Issue 4,2021.12.03,SSDI:Chanchal Singla

	if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_NONE) {
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption = "NONE";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex = staple_none;
	}
	else if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_ONE) {
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption = "STAPLE_1";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex = staple_1;
		if (m_pOemPrivateDevMode->scpi.ext.byBindedge == BINDEDGE_LEFT)
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE].pszOption = "LEFTEDGE";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex = binedge_left;
		}
		else if (m_pOemPrivateDevMode->scpi.ext.byBindedge == BINDEDGE_RIGHT)
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE].pszOption = "RIGHTEDGE";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex = binedge_right;
		}
	}
	else if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_TWO) {
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption = "STAPLE_2";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex = staple_2;
		if (m_pOemPrivateDevMode->scpi.ext.byBindedge == BINDEDGE_LEFT)
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE].pszOption = "LEFTEDGE";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex = binedge_left;
		}
		else if (m_pOemPrivateDevMode->scpi.ext.byBindedge == BINDEDGE_RIGHT)
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE].pszOption = "RIGHTEDGE";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex = binedge_right;
		}
		else if (m_pOemPrivateDevMode->scpi.ext.byBindedge == BINDEDGE_TOP)
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE].pszOption = "TOPEDGE";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex = binedge_top;
		}
	}
	else if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_STAPLELESS) {
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption = "STAPLE_STAPLELESS";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex = staple_1_stapleless;
	}
	else if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_SADDLE) {
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption = "STAPLE_SADDLE";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex = staple_saddle_staple;
	}
	//<S><A>To Fix RedmineBug #4510- issue 1 and Issue 3,2021.11.29,SSDI:Chanchal Singla
	//Function limitation is not proper when Staple is set in Office application
	//Issue 1:- 5.6.1 Offset is not restored to None when Staple setting is done in Office application
	//Issue 3:- 5.6.3 Margin Shift is not restored to None when Staple setting is done in Office application
	if (m_pOemPrivateDevMode->scpi.ext.byStaple != staple_none)
	{
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OFFSET].uChangedOptionIndex = eFirstOption;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OFFSET].pszOption = "None";
		if (!strcmp("STAPLE_SADDLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
		{
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex = punchtype_none;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption = "OFF";
		}
	}

	if (!strcmp("STAPLE_SADDLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
	{
		m_pOemPrivateDevMode->scpi.ext.byMgnShift = marginshift_none;
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex = marginshift_none;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT].pszOption = "None";
	}
	//<E>To Fix RedmineBug #4510- issue 1  and Issue 3,2021.11.29,SSDI:Chanchal Singla
	if (m_pOemPrivateDevMode->scpi.pub.dmOrientation == DMORIENT_PORTRAIT) {
		if ((m_pOemPrivateDevMode)->scpi.ext.dwFieldEDM1 & DM_ROTATE180DEG) {
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption = "PORTRAIT_ROTATED";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex = 2;	// ori_port_rotated;
		}
		else
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption = "PORTRAIT";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex = 0;	// ori_port;
		}
	}
	if (m_pOemPrivateDevMode->scpi.pub.dmOrientation == DMORIENT_LANDSCAPE) {
		if ((m_pOemPrivateDevMode)->scpi.ext.dwFieldEDM1 & DM_ROTATE180DEG) {
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption = "LANDSCAPE_CC270";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex = 3;	// ori_land_rotated;
		}
		else {
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszOption = "LANDSCAPE_CC90";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex = 1;	// ori_land;
		}
	}
	//<S><A>To Fix RedmineBug #4575-,2021.21.17,SSDI:Chanchal Singla
	//Invalid mimic is displayed after pressing Factory Defaults button
	if (marginshift_none == m_pOemPrivateDevMode->scpi.ext.byMgnShift)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT].pszOption = "None";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex = marginshift_none;
	}
	else if (marginshift_04 == m_pOemPrivateDevMode->scpi.ext.byMgnShift)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT].pszOption = "Inch_Point_4";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex = marginshift_04;
	}
	else if (marginshift_08 == m_pOemPrivateDevMode->scpi.ext.byMgnShift)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT].pszOption = "Inch_Point_8";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex = marginshift_08;
	}
	else if (marginshift_12 == m_pOemPrivateDevMode->scpi.ext.byMgnShift)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT].pszOption = "Inch_One_Point_2";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex = marginshift_12;
	}
	//<E>To Fix RedmineBug #4575-,2021.21.17,SSDI:Chanchal Singla
}
//<S><A>to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
//Bug #3666:Information message fails to display after clicking factory defaults
//Bug #3464 :Information Message is displaying even though 2-Sided Printing drop down is None.
BOOL CDocumentPropertyPage::UndoPaperSelection(CONST HWND hDlg, INT uPaperSizeIndex, INT uOutputIndex)
{
	BOOL bUndoSelection = FALSE;
	long wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	LONG wOutputSize = GetValue(hDlg, IDC_CMB_OUTPUTSIZE);
	LONG wPaperSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	if (m_ObjCShowMessage == NULL)
	{
		return bUndoSelection;
	}
	if ((eSecondOption == wDuplex) || (eThirdOption == wDuplex))
	{
		if (((uPaperSizeIndex >= 8 && uPaperSizeIndex < PAPERSIZE_CUSTOMSIZE) && (uOutputIndex == 0)) && (wBooklet == 0))
		{
			bUndoSelection = TRUE;
		}
		else
		{
			if ((uOutputIndex >= 9 && uOutputIndex < OUTPUTPAPERSIZE_CUSTOMSIZE) && (wBooklet == 0))
			{
				bUndoSelection = TRUE;
			}
		}
	}
	if (bUndoSelection == TRUE)
	{
		m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_24, MB_ICONINFORMATION | MB_OK);
	}
	return bUndoSelection;
}
//<E>to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla

//<S><A>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
//=============================================================================
// function
//      ReadStapleLessFlag
//
// parameters
//     BYTE
//
// return value
//      BOOL 
//
// outline
//      ReadStapleLessFlag
//=============================================================================
BOOL CDocumentPropertyPage::ReadStapleLessFlag(BYTE& staplelessvalue)
{
	BOOL lRet = FALSE;
	CShRegAC		*read = NULL;
	read = new CShRegAC(ghInstance, m_pPrinterName, NULL);
	if (read == NULL)
	{
		return false;
	}

	read->ReadStapleLessData(staplelessvalue);

	if (staplelessvalue)
	{
		lRet = TRUE;
	}

	if (read != NULL)
	{
		delete	read;
		read = NULL;
	}

	return lRet;
}
//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S

//<S><A>To Fix Redmine #7424,31-05-2024,SSDI:Manoj S
BOOL CDocumentPropertyPage::limStapleVsBooklet(HWND hDlg, INT iCtrlID, UINT FeatureIndex)
{
	BOOL bRet = FALSE;
	UINT uChangedOptionIndex = 0;
	LONG lVal = m_pFeatureInfoList->pFeatureOptionsList[FeatureIndex].uChangedOptionIndex;
	switch (iCtrlID)
	{
	case IDC_CMB_STAPLE:

		if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))

		{
			if (lVal == staple_1_stapleless)
			{
				m_pFeatureInfoList->pFeatureOptionsList[FeatureIndex].uChangedOptionIndex = staple_none;
			}
			bRet = TRUE;
		}
		break;

	default:
		break;

	}
	return bRet;
}
//<E>To Fix Redmine #7424,31-05-2024,SSDI:Manoj S

//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
//=============================================================================
// function
//      DealWithFavItemsInHKLM
//
// parameters
//      ñ≥Çµ
//
// return value
//      ê¨å˜Å@: TRUE
//      é∏îsÅ@: FALSE
//
// outline
//      deal with all of the fav items in HKLM
//=============================================================================
BOOL CDocumentPropertyPage::DealWithFavItemsInHKLM()
{
	BOOL bRet = FALSE;
	TCHAR szWaterMarkDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szWaterMarkDatFilePath, L"wm62.dat");

	CShIniFile* cshinifile = new CShIniFile(ghInstance, m_pPrinterName, szWaterMarkDatFilePath, FALSE);
	CShRegUS		*preg = new CShRegUS(ghInstance, m_pPrinterName, m_hStringResourceHandle, NULL, m_pFeatureInfoList, m_pOemPrivateDevMode);
	CWaterMarkRegistryData		*pregwm = new CWaterMarkRegistryData(ghInstance, m_pPrinterName, m_hStringResourceHandle);

	if (preg == NULL)
	{
		goto EXIT;
	}
	if (pregwm == NULL)
		goto EXIT;


	WORD			wDPI = 0;
	WORD			wOrient = 0;
	char	szTime[SCUI_FILEDATETIME_STRLEN] = { 0 };

	
	preg->DealWithFavItemsInHKLM(m_pPrinterName/*, pFeatureList*/);

	if (pregwm->ReadCount(m_pPrinterName) == 0)
		pregwm->reset(m_pPrinterName, cshinifile);

	pregwm->DealWithFavItemsInHKLM(m_pPrinterName);


	bRet = TRUE;
EXIT:
	return bRet;
}

BOOL CDocumentPropertyPage::Update_GPDFeatureOptionList()
{
	GPDFEATUREOPTIONLIST pFeatureList;
	SecureZeroMemory(&pFeatureList, sizeof(GPDFEATUREOPTIONLIST));

	BOOL ret = Update_FeatureInfoList_TO_GPDFeatureOptionList(m_pFeatureInfoList, &pFeatureList);

	return ret;
}

//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S