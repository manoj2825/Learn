//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : WaterMarkAddDialog.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 09 October 2013
//
//	Description   : This class implements combo box control
//===========================================================================
#include "precomp.h"
#include "resource.h"
//For Logging Purpose
#include "MFPLogger.h"
//=============================================================================
// function
//      CWaterMarkAddDialog
//
// parameters
// RESOURCEIDINFO& pResourceIDInfo,
// PRINT_FEATURE_OPTION*  pFeatureOptionPair,
// PFEATUREOPTIONSLIST pFeatureOptionsList,
// HMODULE hStringResourceHandle,
// DEFAULTDEVMODEINFO& pDefaultSettings
//
// return value
//     NILL
//
// outline
//      Constructor to the class CWaterMarkAddDialog.
//=============================================================================
CWaterMarkAddDialog::CWaterMarkAddDialog(
	UINT uContorlID,
	HMODULE hStringResourceHandle,
	WCHAR* szWaterMark
) :
	CUIControl(uContorlID, hStringResourceHandle),
	m_sWaterMarkIndex(0)

{
	wcscpy_s(m_szWaterMark, WM_STR_LENGTH + 1, szWaterMark);

	//Fixed Coverity issue - 90616 - SSDI:Seetharam-20200729
	m_dwSelectedPaperWidthPixel = 0;
	m_dwSelectedPaperHeightPixel = 0;
	m_sOrientation = 0;
	m_hPropertySheetHandle = NULL;
	//Fixed Coverity issue - 90628 - SSDI:Seetharam-20201005
	//SecureZeroMemory(m_PropertySheetState, sizeof(PPROPSTATE));
	//SecureZeroMemory(m_WaterMarkDataChanged , sizeof(LPWATERMARKDATA));
	m_PropertySheetState = NULL;
	m_WaterMarkDataChanged = NULL;

}


CWaterMarkAddDialog::CWaterMarkAddDialog(
					UINT uContorlID,
					HMODULE hStringResourceHandle,
					PPROPSTATE pPropertyState,
					WCHAR* pPrinterName,
					HANDLE hPrinter,
					LPWATERMARKDATA WaterMarkDataChanged,
					DWORD dwPaperWidthPixel,
					DWORD dwPaperHeightPixel,
					SHORT sOriantation
			  ):
					CUIControl(uContorlID,hStringResourceHandle),
					m_sWaterMarkIndex(0)

{
	
	m_PropertySheetState = pPropertyState;
	m_hPrinter = hPrinter;
	m_WaterMarkDataChanged = WaterMarkDataChanged;
	//<S><A>To fix #3085 Issue 1,20201612,SSDI:Chanchal Singla
	//Watermark: Preview of watermark displays wrong when trying to add a new Watermark
	memcpy_s(m_WaterMarkDataChanged->szMainWM, _countof(m_WaterMarkDataChanged->szMainWM) * sizeof(WCHAR),
		m_PropertySheetState->szWM, _countof(m_PropertySheetState->szWM) * sizeof(WCHAR));
	//<E>To fix #3085 Issue 1,20201612,SSDI:Chanchal Singla
	m_dwSelectedPaperWidthPixel = dwPaperWidthPixel;
	m_dwSelectedPaperHeightPixel = dwPaperHeightPixel;
	m_sOrientation = sOriantation;
	wcscpy_s(m_PszPrnName, INI_STR_MAX, pPrinterName); // Copy the printer name String to m_PszPrnName
	wcscpy_s(m_szWaterMark, WM_STR_LENGTH + 1, m_PropertySheetState->szWM);
	//Fixed Coverity issue - 90615 - SSDI:Seetharam - 20201020
	m_hPropertySheetHandle = NULL;
}

//=============================================================================
// function
//      CWaterMarkAddDialog
//
// parameters
//      NILL            
//
// return value
//     NILL
//
// outline
//      Destructure to the class CWaterMarkAddDialog.
//=============================================================================
CWaterMarkAddDialog::~CWaterMarkAddDialog(void)
{
}
//=============================================================================
// function
//      OnInit
//
// parameters
//      iCtrlID --- Control ID of combo box  control          
//		hDlg ------- handle to the dialog box that contains the control.   
// return value
//     HRESULT
//
// outline
//      Initializes combo  box values
//=============================================================================
LRESULT
CWaterMarkAddDialog::OnWmInitdialog(void)

{
    SetTitle();
	m_hPropertySheetHandle = m_hWnd;
	SetControlStrings();
	CreateControlClass();
	InitializeUIControls(m_hWnd);
	return S_OK;
}
/*++
Routine Name:

    CWaterMarkAddDialog::SetTitle

Routine Description:

    Routine for Setting the Title for the dialogs

Arguments:

    None

Return Value:

    TRUE/FALSE
   
--*/
BOOL CWaterMarkAddDialog::SetTitle(void)
{
	TCHAR szItem[MAX_PATH]={0};
	if (LoadString(m_hStringResourceHandle, m_uControlID, szItem, countof(szItem)) > 0)
	{
		SetWindowText(m_hWnd,szItem);
	}
	return TRUE;

}
/*++
Routine Name:

    CWatermarkEditDialog::OnBnClicked

Routine Description:

    Routine for handling button click events

Arguments:

    None

Return Value:

    None
   
--*/
LRESULT CWaterMarkAddDialog::OnBnClicked(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	 switch(wCtrlID)
	{
		case IDOK:
			OnIDOK(hWnd, hWndCtrl,wCtrlID);
			break;
		default:
			return CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
	}
	res = 1;
	return res;
}
/*++
Routine Name:

    CWatermarkEditDialog::OnIDOK

Routine Description:

    Routine for handling ok button click 

Arguments:

    None

Return Value:

    None
   
--*/
LRESULT CWaterMarkAddDialog::OnIDOK(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	//Fixed Coverity issue - 90490 - SSDI:Seetharam-20200928
	//Cannot check NULL for an object that is not pointer
	//if(nullptr == m_szWaterMark)
	//{
	//	slog.putLog("CWaterMarkAddDialog::OnIDOK -> ERROR_m_szWaterMark\n");
	//	return E_FAIL;
	//}
	TCHAR szWaterMarkDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szWaterMarkDatFilePath, L"wm62.dat");
	CShIniFile* cshinifile = new CShIniFile(ghInstance, m_pPrinterName, szWaterMarkDatFilePath, FALSE);
	CShJsonWm* jsonwm = new CShJsonWm(ghInstance, m_pPrinterName, m_hStringResourceHandle);
	jsonwm->Init();

	WATERMARKDATA	watermarkdata = {0};
	BOOL bWaterMarkAlreadyExists = FALSE;
	DWORD dwWaterMarkIndex = 1;
	WCHAR szWaterMark[WM_STR_LENGTH +1]={0};
	if ((*cshinifile).IsWriteToJson() == TRUE)
	{

		DWORD dwCountjson = jsonwm->ReadWMCount(GetPrinterName());
		for (; dwWaterMarkIndex < dwCountjson; dwWaterMarkIndex++)
		{
			jsonwm->ReadWMData(dwWaterMarkIndex, &watermarkdata, szWaterMark, sizeof(szWaterMark));
			if (0 == wcscmp(m_szWaterMark, szWaterMark))
			{
				bWaterMarkAlreadyExists = TRUE;

				break;
			}
		}
	}
	//else
	//{
		DWORD dwCount = m_pRegistryWaterMarkData->ReadWMCount(GetPrinterName());
		for (; dwWaterMarkIndex < dwCount; dwWaterMarkIndex++)
		{
			m_pRegistryWaterMarkData->ReadWMData(GetPrinterName(), dwWaterMarkIndex, &watermarkdata, szWaterMark, sizeof(szWaterMark));
			if (0 == wcscmp(m_szWaterMark, szWaterMark))
			{
				bWaterMarkAlreadyExists = TRUE;

				break;
			}
		}
	//}
	if(TRUE == bWaterMarkAlreadyExists)
	{
		WCHAR szMessage[MAX_PATH] ={ 0};
        WCHAR szItem[MAX_PATH] = {0};
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_42, szMessage, countof(szMessage)) > 0)
		{
            LoadString(m_hStringResourceHandle, IDS_MESTITLE_1, szItem, countof(szItem));
			if ( IDCANCEL == MessageBox(hWnd,szMessage,szItem,MB_ICONINFORMATION | MB_OKCANCEL))
			{
				return 0;
			}
		}
		
	}
	m_sWaterMarkIndex = (SHORT)dwWaterMarkIndex ;
	CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);

	delete cshinifile;
	cshinifile = NULL;

	delete jsonwm;
	jsonwm = NULL;

	return S_OK;
}
/*++
Routine Name:

    CWatermarkEditDialog::OnEnChange

Routine Description:

    Routine for handling ok edit box change events

Arguments:

    None

Return Value:

    None
   
--*/
LRESULT CWaterMarkAddDialog::OnEnChange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_EDT_WMSAVE_NAME);
	if(Iterator != m_UIControls.end())
	{
		CUIControl *pContol = Iterator->second ;
		pContol->OnCommand(IDC_EDT_WMSAVE_NAME,hWnd,EN_CHANGE);
	}
	//Fixed Coverity issue - 90443 - SSDI:Seetharam-20200928
	//Cannot check NULL for an object that is not pointer
	//if(nullptr == m_szWaterMark)
	//{
	//	slog.putLog("CWaterMarkAddDialog::OnEnChange -> ERROR_Watermark\n");
	//	return E_FAIL;
	//}
	if(0 == wcslen(m_szWaterMark) || (TRUE == CheckSpaceOnly(m_szWaterMark)))
	{
		EnableWindow(GetDlgItem(hWnd, IDOK),FALSE);
	}
	else
	{
		EnableWindow(GetDlgItem(hWnd, IDOK),TRUE);
	}
	CUIControl::OnEnChange(hWnd, hWndCtrl, wCtrlID);
	return S_OK;
}
/*++

Routine Name:

    CUICtrlDefaultCombo::OnActivate

Routine Description:

    Called when the parent property page becomes active.
    This method initialises the state of the control to reflect the Unidrv GPD settings.

Arguments:

    hDlg - handle to the parent window

Return Value:

    HRESULT
    S_OK - On success
    E_*  - On error

--*/
void CWaterMarkAddDialog::CreateControlClass(void)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CUIControl* pControl = new CWaterMarkEdit(m_szWaterMark,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_EDT_WMSAVE_NAME, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkAddDialog::CreateControlClass -> NULL_OBJECT(CWaterMarkEdit)\n");
	}
	pControl = new CWaterMarkPreview(IDC_BTN_WM_PREV, m_hPropertySheetHandle, m_PropertySheetState, m_PszPrnName, 
		m_hStringResourceHandle, m_hPrinter);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_PREV, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(IDC_BTN_WM_PREV)\n");
	}
	
}
/*++
Routine Name:

    CWatermarkEditDialog::SetControlStrings

Routine Description:

    Routine for Setting the Strings For all Static Conytrols

Arguments:

    None

Return Value:

    None
   
--*/
void CWaterMarkAddDialog::SetControlStrings(void)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CUIControl* pControl = new CStaticText(IDC_STC_WMSAVE_NAME,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WMSAVE_NAME, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkAddDialog::SetControlStrings -> NULL_OBJECT(IDC_STC_WMSAVE_NAME)\n");
	}
	
	// Ok Button
	pControl = new CStaticText(IDS_IDC_BTN_SHARED_OK, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDOK, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkAddDialog::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	//Cancel Button
	pControl = new CStaticText(IDS_IDC_BTN_SHARED_CANCEL, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDCANCEL, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkAddDialog::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}
}
/*++
Routine Name:

    CWatermarkEditDialog::GetWaterMarkIndex

Routine Description:

    Routine for getting watermark index

Arguments:

    None

Return Value:

    None
   
--*/
SHORT CWaterMarkAddDialog::GetWaterMarkIndex(void)
{
	return m_sWaterMarkIndex;
}
/*++
Routine Name:

    CWatermarkEditDialog::GetSavedWaterMarkString

Routine Description:

    Routine for getting saved watermark string

Arguments:

    None

Return Value:

    None
   
--*/
WCHAR* CWaterMarkAddDialog::GetSavedWaterMarkString(void)
{
	return m_szWaterMark;
}
//=============================================================================
// function
//      OnWmCommand
//
// parameters
//      hWnd - Handle to Window.
//		wParam - Message Parameter.
//		lParam - Message Parameter.
//
// return value
//      TRUE always
//
// outline
//      Handles the BN_CLICKED and EN_KILLFOCUS events.
//=============================================================================
LRESULT CWaterMarkAddDialog::OnWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND	hWndCtrl = (HWND)lParam;
	WORD	wNotify = HIWORD(wParam);
	WORD	wCtrlId = LOWORD(wParam);

	switch (wNotify)
	{
	case BN_CLICKED:
	{
		if ((NULL == hWndCtrl) || (TRUE == ::IsWindowEnabled(hWndCtrl)))
		{
			OnBnClicked(hWnd, hWndCtrl, wCtrlId);
		}
	}
	break;

	case EN_CHANGE:
	{
		OnEnChange(hWnd, hWndCtrl, wCtrlId);
	}
	break;
	
	default:
		break;
	}
	return TRUE;
}

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
LRESULT CWaterMarkAddDialog::OnWmDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CWaterMarkPreview*	m_pWaterMarkPreview = NULL;
	
	UIControlMap::const_iterator Iterator;
		Iterator = m_UIControls.find(IDC_BTN_WM_PREV);
		if (Iterator != m_UIControls.end())
		{
			m_pWaterMarkPreview = static_cast<CWaterMarkPreview*>(Iterator->second);
			if (NULL == m_pWaterMarkPreview)
			{
				slog.putLog("CWaterMarkAddDialog::OnWmDrawItem -> NULL_OBJECT(m_pWaterMarkPreview)\n");
			}
		}

		if (NULL != m_pWaterMarkPreview)
		{
			m_pWaterMarkPreview->Draw(m_WaterMarkDataChanged, m_dwSelectedPaperWidthPixel, m_dwSelectedPaperHeightPixel, m_sOrientation);
		}
		else
		{
			slog.putLog("CWaterMarkAddDialog::OnWmDrawItem -> NULL_POINTER(m_pWaterMarkPreview)\n");
		}
		
	
	return CUIControl::OnWmDrawItem(hWnd, wParam, lParam);
}