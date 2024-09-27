//	Copyright 2020(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//======================================================================================
//  File          : CShDlgStored2.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Goutham R D
//
//  Date		  : 10 August 2020
//
//	Description   : This class handles Property sheet dialog functions for Stored To dialog
//========================================================================================

#include "precomp.h"
#include "regkey.h"
#include "aes.h"
#include "shjsonstr.h"
//For Logging Purpose
#include "MFPLogger.h"
#include "windowsx.h"


//=============================================================================
// function
//      CShDlgStored2
//
// parameters
//      hStringResourceHandle - Handle to String Resource
//		uControlID	- Control ID stored in resource file
// return value
//     NILL
//
// outline
//      Constructor to the class CShDlgStored2.
//=============================================================================
CShDlgStored2::CShDlgStored2(UINT uContorlID, HMODULE hStringResourceHandle,HMODULE hDialogResourceHandle, HANDLE hPrinter, HWND PropertySheetHandle, ResourceIDInfoMap* pResourceIDInfoMap,
	PFEATUREINFOLIST &pFeatureInfoList, std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings, HMODULE hBitmapResourceHandle,
	PDEVMODE pOemPublicDevMode, POEMDEV pTempOemPrivateDevMode, PPROPSTATE pPropertySheetState,WCHAR *pPrinterName,BOOL wIsJobHandlingDlg) :
	CUIControl(uContorlID, hStringResourceHandle, hPrinter)
{
	m_hStringResourceHandle = hStringResourceHandle;
	m_hDialogResourceHandle = hDialogResourceHandle;
	m_hWndParent = PropertySheetHandle;
	m_DefaultSettings = pDefaultSettings;
	m_pFeatureInfoList = pFeatureInfoList;
	m_pResourceIDInfoMap = pResourceIDInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	m_DefaultSettings = pDefaultSettings;
	m_hPrinterHandle = hPrinter;
	m_hBitmapResourceHandle = hBitmapResourceHandle;
	m_pOemPublicDevMode = pOemPublicDevMode;
	m_pTempOemPrivateDevMode = pTempOemPrivateDevMode;
	SecureZeroMemory(&m_ps_sub, sizeof(PROPSTATE));
	m_ps_sub = pPropertySheetState;
	m_plvFolderSel = NULL;
	m_pszSvrPrnName = pPrinterName;
	m_IsJobHandlingDlg = wIsJobHandlingDlg;
	//Coverity Fix 90581 - SSDI:Seetharam - 20200827
	m_stStoredFolderList.dwCount = 0;
	m_stStoredFolderList.pstFolderData = NULL;
	m_blForceUpdate = TRUE;
	//Reverted because of UI crashing issue
	//Coverity Fix 90599 - SSDI:Goutham - 20201123
	m_pPrivateDevmode = pTempOemPrivateDevMode;
	//SecureZeroMemory(&m_pPrivateDevmode, sizeof(OEMDEV));
	SecureZeroMemory(&m_ps_pre, sizeof(PROPSTATE));
//Coverity Fix 90599 - SSDI:Goutham - 20201201
	//m_lCurValue = -1;
	m_lCurValue = 0;

	// Coverity Fix 91536 - Initializing the uninitialized variables SSDI:Seetharam - 20210907
	m_dwEvnetFlg = STRD_LV_EVENT_NONE;
	m_stStoredFolderListSv.dwCount = 0;
	m_stStoredFolderListSv.pstFolderData = NULL;
	m_wSaveFolderPass = FALSE;
	m_dwMaxCount = SCUI_STOREDTOFOLDER_MAX;
	m_blUnicode = FALSE;
	m_hPropertySheetHandle = NULL;
	// Coverity Fix 91536 - Initializing the uninitialized variables SSDI:Seetharam - 20210907
}
//=============================================================================
// function
//      CShDlgStored2
//
// parameters
//      NILL            
//
// return value
//     NILL
//
// outline
//      Destructure to the class CShDlgStored2
//=============================================================================

CShDlgStored2::~CShDlgStored2()
{
	DestroyUIComponents();
}

//=============================================================================
//
//Routine Name:
//
//	CShDlgStored2::SendSetActive
//
//Routine Description:
//
//	Call the OnActivate() method in all control handlers in the collection.
//
//Arguments:
//
//	hDlg - Handle of property page.
//
//Return Value:
//
//	HRESULT
//	S_OK - On success
//	E_*  - On error
//
//=============================================================================

HRESULT
CShDlgStored2::ActivateUIControl(
	_In_ CONST HWND   hDlg
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	// Use the wParam as the index into the control map and
	// inform the control of an activation

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
			slog.putLog("CShDlgStored2::ActivateUIControl -> NULL_POINTER(pControl)\n");
		}
	}
	return hr;
}


//=============================================================================
//Routine Name:
//
//	CShDlgStored2::SetTitle
//
//Routine Description:
//
//	Routine for Setting the Title for the dialogs
//
//Arguments:
//
//	None
//
//Return Value:
//
//	TRUE/FALSE
//
//=============================================================================

BOOL CShDlgStored2::SetTitle(void)
{
	TCHAR szItem[MAX_PATH] = { 0 };
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (LoadString(m_hStringResourceHandle, m_uControlID, szItem, countof(szItem)) > 0)
	{
		SetWindowText(m_hWnd, szItem);
	}
	else
	{
		slog.putLog("CShDlgStored2::SetTitle -> ERROR_LOAD_STRING\n");
	}
	return TRUE;

}

//<S><A>Added function for Seting all control strings,2020.08.11 -SSDI:Goutham R D
//=============================================================================
// function
//      SetControlStrings
//
// return value
//     NILL
//
// outline
//      Sets all control strings.
//=============================================================================
void CShDlgStored2::SetControlStrings(void)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	
	CUIControl* pControl = new CStaticText(IDS_IDC_BTN_STRD_DEL, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_STRD_DEL, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgStored2::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}
//<S><A> To add folder selection in UI for custom folder dialog,2020.08.13 -SSDI:Goutham
	pControl = new CStaticText(IDS_IDC_GRP_STORED, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_GRP_STORED, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgStored2::SetControlStrings -> NULL_OBJECT(IDC_GRP_STORED)\n");
	}

//<S><E> To add folder selection in UI for custom folder dialog,2020.08.13 -SSDI:Goutham
	pControl = new CStaticText(IDS_IDC_BTN_STRD_ADD, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_STRD_ADD, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgStored2::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_STRD_FOLDER_PASS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_STRD_FOLDER_PASS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgStored2::SetControlStrings -> NULL_OBJECT(IDC_STC_STRD_FOLDER_PASS)\n");
	}

	pControl = new CStaticText(IDS_IDC_CHK_STRD_FOLDER_ALWAYS_PASS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_STRD_FOLDER_ALWAYS_PASS, pControl);
		if (S_OK != hr)
			slog.LoggingErrorHResultValues();
	}
	else
	{
		slog.putLog("CJobHandlingDialog::SetControlStrings -> NULL_POINTER(IDC_CHK_ALWAYS_PIN)");
	}

	//OK button 
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
		slog.putLog("CShDlgStored2::SetControlStrings -> NULL_OBJECT(IDOK)\n");
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
		slog.putLog("CShDlgStored2::SetControlStrings -> NULL_OBJECT(IDCANCEL)\n");
	}

	// Defaults button
	pControl = new CStaticText(IDS_IDC_BTN_SHARED_DEFAULT, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_SHARED_SUB_DEFAULT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgStored2::SetControlStrings -> NULL_OBJECT(IDC_BTN_SHARED_DEFAULT)\n");
	}

}
//<S><E>Added function for Seting all control strings,2020.08.11 -SSDI:Goutham R D

PRESOURCEIDINFO CShDlgStored2::GetResourceIDInfoOfControl(std::string strFeatureName)
{
	ResourceIDInfoMap::iterator ResourceInfoIterator = m_pResourceIDInfoMap->find(strFeatureName);
	if (ResourceInfoIterator != m_pResourceIDInfoMap->end())
	{
		return &(ResourceInfoIterator->second);
	}
	return NULL;
}

HRESULT
CShDlgStored2::SendCommand(
	_In_ CONST HWND   hDlg,
	_In_ CONST WPARAM wParam
)
{

	// Use the wParam as the index into the control map and
	// inform the control that generated the command

	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	UIControlMap* pComponents = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	CUIControl* pControl = (*pComponents)[LOWORD(wParam)];

	if (NULL != pControl)
	{
		hr = pControl->OnCommand(LOWORD(wParam), hDlg, HIWORD(wParam));
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgStored2::SendCommand -> NULL_POINTER(pControl)\n");
	}


	return hr;
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
LRESULT CShDlgStored2::OnWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
	case EN_KILLFOCUS:
	{
		OnEnKillFocus(hWnd, hWndCtrl, wCtrlId);
	}
	break;
	case EN_SETFOCUS:
	{
		OnEnSetFocus(hWnd, hWndCtrl, wCtrlId);
	}
	break;
	case EN_CHANGE:
	{
		OnEnChange(hWnd, hWndCtrl, wCtrlId);
	}
	break;
	case WM_NOTIFY:
	{
		OnWmNotify(hWnd, wParam, lParam);
	}
	break;
	default:
		break;
	}
	return TRUE;
}

//<S><C>Modified to create control class for  stored to dialog ,2020.08.27 SSDI:Goutham
//=============================================================================
// function
//      CreateContorlClass
//
// return value
//     NILL
//
// outline
//      Create all controls.
//=============================================================================
void CShDlgStored2::CreateControlClass(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	
	CUIControl* pControl;
	std::string strFeatureName;
	PRESOURCEIDINFO pResourceIDInfo = NULL;
	pControl = new CJobControlEdit(
		m_hStringResourceHandle,
		m_hPrinter,
		m_hWndParent,
		IDC_EDT_STRD_FOLDER_PASS ,
		(WCHAR*)m_ps_sub->szFolderPass,
		JC_STR_PINLENGTH, 
		eNumberPasswordEditBox
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_STRD_FOLDER_PASS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgStored2::CreateControlClass -> NULL_POINTER(IDC_EDT_STRD_FOLDER_PASS)");
	}
	CheckDlgButton(m_hWnd,IDC_CHK_STRD_FOLDER_ALWAYS_PASS, m_wSaveFolderPass);


	//CShLVStored
	pControl = new CShLVStored(m_hStringResourceHandle, m_hWnd, m_pOemPublicDevMode,m_pPrivateDevmode,IDC_LISTV_STRD_FOLDER_SEL);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_LISTV_STRD_FOLDER_SEL, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		m_plvFolderSel = (CShLVStored *)pControl;
		(*m_plvFolderSel).createSortSubNo(1);
	}
	else
	{
		slog.putLog("CShDlgStored2::CreateControlClass -> NULL_POINTER(IDC_LISTV_STRD_FOLDER_SEL)");
	}
}
//<S><E>Modified to create control class for  stored to dialog ,2020.08.27 SSDI:Goutham


//<S><A>Added the SetTabDefaults() function to handle defaults button,2020.08.11 -SSDI:Goutham R D
//=============================================================================
//Routine Name:
//
//	CShDlgStored2::SetTabDefaults
//
//Routine Description:
//
//	The routine calls the SetTabDefaults() function of each control class
//
//Arguments:
//
//	hDlg - handle to the dialog
//
//Return Value:
//
//   S_OK,
//   return value from overridden SetTabDefaults()
//
//=============================================================================
HRESULT CShDlgStored2::SetTabDefaults(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	CUIControl* pControl = NULL;
	for (auto iterUIComponents : m_UIControls)
	{
		pControl = iterUIComponents.second;
		if (NULL != pControl)
		{
			if (FAILED(hr = pControl->SetTabDefaults(iterUIComponents.first, hDlg)))
			{
				if (S_OK != hr)
				{
					slog.LoggingErrorHResultValues();
				}
				break;
			}

		}
	}
	//Always Use This ...
	CheckDlgButton(hDlg,IDC_CHK_STRD_FOLDER_ALWAYS_PASS, BST_UNCHECKED);

	::ZeroMemory(m_ps_sub->szFolderPass, sizeof(m_ps_sub->szFolderPass));
	//<S><A>To fix feedback  issue 24 sprint19,10-21-2020,SSDI:Goutham
    //<S><C>To fix redmine bug #2741,2020.10.29,SSDI:Goutham
		if (m_ps_sub->wDocFileFolder > 1) {
    //<S><E>To fix redmine bug #2741,2020.10.29,SSDI:Goutham
			// Main Folder à»äOÇ…ÉfÅ[É^Ç™ë∂ç›Ç∑ÇÈ
			// Ç±ÇÃèÍçáÇÕÅAListviewÇÃêÊì™ÇÃÉfÅ[É^ÇëIëÇµÇƒÇ¢ÇÈèÛë‘Ç…Ç∑ÇÈ
			m_ps_sub->wDocFileFolder = 1;
		}
		//<S><C>To fix feedback  issue 24 sprint19,0-21-2020,SSDI:Goutham

	SetTabState(hDlg);
	return hr;
}
//<S><E>Added the SetTabDefaults() function to handle defaults button,2020.08.11 -SSDI:Goutham R D

//<S><A>constraints and conditions have to be implemented
//=============================================================================
// function
//      SetTabState
//
// parameters
//		hWnd	-> Handle to the Dialog
//
// return value
//    BOOL TRUE/FALSE
//
// outline
//      This function Sets the Current Tab/Dialog State for USer Selection
//=============================================================================
void CShDlgStored2::SetTabState(HWND hWnd)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL			blRet = FALSE;
	PSTORED_FOLDER_DATA	pstFolderData = NULL;

	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	pControl = (*pComponents)[IDC_EDT_STRD_FOLDER_PASS];
	if (NULL == pControl)
	{
		slog.putLog("CShDlgStored2::OnIDOK -> NULL_POINTER(CShDlgStored2)\n");
		return;
	}
	
	
	//m_ps_sub->wDocFileFolder = (short)(m_stStoredFolderList.dwCount - 1);
	// --- folder pass code
	((CJobControlEdit*)pControl)->setval(hWnd,m_ps_sub->szFolderPass, TRUE);

	// --- list
	if (m_ps_sub->wDocFileFolder <= 0) {
		
		(*m_plvFolderSel).setItemState(0, 0, (LVIS_SELECTED | LVIS_FOCUSED));
	}
	else {
		(*m_plvFolderSel).setItemState(m_ps_sub->wDocFileFolder - 1, (LVIS_SELECTED | LVIS_FOCUSED), (LVIS_SELECTED | LVIS_FOCUSED));
	}

	if (m_dwMaxCount <= m_stStoredFolderList.dwCount - 1) {
		SetControlState(hWnd, IDC_BTN_STRD_ADD, SCUI_STATE_SHOW);
	}
	else {
		SetControlState(hWnd, IDC_BTN_STRD_ADD, SCUI_STATE_ON);
	}

	if (m_stStoredFolderList.dwCount == 1) 
	{
				// --- Delete
		//<S><A>To fix Redmine bug #2910,20201127 SSDI:Goutham
		::ZeroMemory(m_ps_sub->szFolderPass, sizeof(m_ps_sub->szFolderPass));
		//<S><A>To fix Redmine bug #2910,20201127 SSDI:Goutham
		SetControlState(hWnd, IDC_BTN_STRD_DEL, SCUI_STATE_SHOW);

		// --- folder pass code
		SetControlState(hWnd, IDC_STC_STRD_FOLDER_PASS, SCUI_STATE_SHOW);
		SetControlState(hWnd, IDC_EDT_STRD_FOLDER_PASS, SCUI_STATE_SHOW);
		
		// --- always use this folder pin code
		SetControlState(hWnd, IDC_CHK_STRD_FOLDER_ALWAYS_PASS, SCUI_STATE_SHOW);
		CheckDlgButton(hWnd, IDC_CHK_STRD_FOLDER_ALWAYS_PASS, BST_UNCHECKED);
	}
	else 
	{
		// --- Delete
		SetControlState(hWnd, IDC_BTN_STRD_DEL, SCUI_STATE_ON);

		(*m_plvFolderSel).GetListviewItemStored(m_ps_sub->wDocFileFolder - 1, &pstFolderData);
		if (pstFolderData != NULL)
		{
			if ((*pstFolderData).blHasPin == bool_true)
			{
				// --- folder pass code
				SetControlState(hWnd, IDC_STC_STRD_FOLDER_PASS, SCUI_STATE_ON);
				SetControlState(hWnd, IDC_EDT_STRD_FOLDER_PASS, SCUI_STATE_ON);

				// --- always use this folder pin code
				SetControlState(hWnd,IDC_CHK_STRD_FOLDER_ALWAYS_PASS, SCUI_STATE_ON);
			}
			else 
			{
				// --- folder pass code
				SetControlState(hWnd, IDC_STC_STRD_FOLDER_PASS, SCUI_STATE_SHOW);
				SetControlState(hWnd, IDC_EDT_STRD_FOLDER_PASS, SCUI_STATE_SHOW);

				// --- always use this folder pin code
				SetControlState(hWnd, IDC_CHK_STRD_FOLDER_ALWAYS_PASS, SCUI_STATE_SHOW);
				CheckDlgButton(hWnd, IDC_CHK_STRD_FOLDER_ALWAYS_PASS, BST_UNCHECKED);
			}
		}
		else 
		{
			// --- folder pass code
			SetControlState(hWnd, IDC_STC_STRD_FOLDER_PASS, SCUI_STATE_ON);
			SetControlState(hWnd, IDC_EDT_STRD_FOLDER_PASS, SCUI_STATE_ON);

			// --- always use this folder pin code
			SetControlState(hWnd, IDC_CHK_STRD_FOLDER_ALWAYS_PASS, SCUI_STATE_ON);
		}

	}
	//in job Handling Property Page "always use this pass code" feature is not there,
	// So disable this feature only for job handling property page tab
	if (m_IsJobHandlingDlg == FALSE)
	{
		// --- always use this pass code
		SetControlState(hWnd, IDC_CHK_STRD_FOLDER_ALWAYS_PASS, SCUI_STATE_OFF);
	}
}
//<S><E>constraints and conditions have to be implemented.


//=============================================================================
//Routine Name:
//
//	CShDlgStored2::OnBnClicked
//
//Routine Description:
//
//	Routine for Button Click Events
//
//Arguments:
//
//	hWnd - handle to dialog
//	hWndCtrl - handle to the Control
//	wCtrlID - Control Resource ID
//
//Return Value:
//
//	LRESULT
//=============================================================================

LRESULT CShDlgStored2::OnBnClicked(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	BOOL wblUpdate = TRUE;
	switch (wCtrlID)
	{
	case IDOK:
	{
		OnIDOK(hWnd, hWndCtrl,wCtrlID);
		wblUpdate = FALSE;
	}
	
	break;
	case IDCANCEL:
		CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
		break;
	case IDC_BTN_STRD_ADD:
	{
		OnAddFolder(hWnd);
	}
	break;
	case IDC_BTN_STRD_DEL:
	{
		OnIdcBtnStrdDelFolder(hWnd, hWndCtrl);
	}
	break;
	case IDC_BTN_SHARED_SUB_DEFAULT:
	{
		SetTabDefaults(hWnd);
	}
	break;
	default:
		res = CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
	}
	if (wblUpdate != FALSE)
	{
		SetTabState(hWnd);
	}
	return res;
}

//=============================================================================
//
//Routine Name:
//
//	CShDlgStored2::OnWmInitdialog
//
//Routine Description:
//
//	This Functions Initialises Dialog Controls
//
//Arguments:
//
//		None
//
//Return Value:
//
//	TRUE/FALSE
//
//=============================================================================

LRESULT CShDlgStored2::OnWmInitdialog(void)
{
	LRESULT result = 1;
	WCHAR			szTitle[256];
	RECT			rcListView;
	LV_COLUMNW		lvc;
	DWORD			dwExStyle;
	DWORD			dwStyle;
	m_hPropertySheetHandle = m_hWnd;
	SetTitle();
	SetControlStrings();
	CreateControlClass();
	ActivateUIControl(m_hWnd);
	m_plvFolderSel->Attach(m_hWnd);

	::ZeroMemory(&rcListView, sizeof(RECT));
	(*m_plvFolderSel).GetListviewRect(&rcListView);					// Listview

	
	::ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

	::ZeroMemory(&rcListView, sizeof(RECT));						// Folder Name
	(*m_plvFolderSel).GetListviewRect(&rcListView);					// Listview
	lvc.cx = rcListView.right - rcListView.left - 4;
	LoadStringW(m_hStringResourceHandle, IDS_STOREDTO_LV_FOLDER_NAME, szTitle, 255);
	
	WCHAR wszTitle[32];
	lvc.pszText = szTitle;
	//mbstowcs(wszTitle, szTitle, sizeof(szTitle));
	//lvc.pszText = (WCHAR*)wszTitle;
	lvc.cchTextMax = ::lstrlenW((WCHAR*)szTitle);
	
	lvc.iSubItem = 0;

	(*m_plvFolderSel).insertColumnW(0, &lvc);

	dwExStyle = (*m_plvFolderSel).getExtendedListViewStyle();		
	(*m_plvFolderSel).setExtendedListViewStyle(dwExStyle | LVS_EX_FULLROWSELECT);

	dwStyle = (*m_plvFolderSel).getListViewStyle();
	(*m_plvFolderSel).setListViewStyle(dwStyle | LVS_SINGLESEL);	
	
	GetStoredDataReg();

	// Listview
	SetListViewStoredData();

	// FolderName in list view set to SCUI_STOREDTOFOLDER_MAX(1000)
	m_dwMaxCount = SCUI_STOREDTOFOLDER_MAX;

	if (m_ps_sub->wDocFileFolder == 0)
	{
		if (m_stStoredFolderList.dwCount > 1)
		{
			// Main Folder 
			// ListviewÇ
			m_ps_sub->wDocFileFolder = 1;
		}
	}
	//<S><A>Added to fix redmine bug #2742,20200924 SSDI:Goutham
	else
	{
		//m_ps_sub->wDocFileFolder = (short)(m_stStoredFolderList.dwCount - 1);
	}
	//<S><E>Added to fix redmine bug #2742,20200924 SSDI:Goutham
    InitializeUIControls(m_hWnd);
	SetTabState(m_hWnd);
	
	return result;
}

// <S><A> To add Add folder Dialog in UD3L, 20200810 - SSDI:Goutham
// <S><C> Modified to add folder name in list view when ok is clicked in Add folder Dialog in UD3L, 20200827 - SSDI:Goutham
//=============================================================================
// function
//      OnAddFolder
//
// return value
//     NILL
//
// outline
//   Handles AddFolder dialog events  .
//=============================================================================
BOOL CShDlgStored2::OnAddFolder(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	char			szConvBuffwchar[(JC_STR_FOLDERPATH+1)*2];
	STORED_FOLDER_DATA	*pstFolderData = NULL;
	CAddFolderDlg *pdlg = new CAddFolderDlg(IDD_DLG_ADD_FOLDER, m_hStringResourceHandle, m_hDialogResourceHandle,  m_hPrinterHandle, 
		m_hWndParent, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings, m_hBitmapResourceHandle,
		m_pOemPublicDevMode, m_pTempOemPrivateDevMode, m_ObjCShowMessage);
	if (NULL == pdlg)
	{
		slog.putLog("CShDlgStored2::OnAddFolder -> NULL_POINTER(IDD_DLG_ADD_FOLDER)\n");
		return FALSE;
	}
	pdlg->SetPrinterName(m_pPrinterName);
	INT_PTR DlgRet = IDCANCEL;
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_ADD_FOLDER),
		hDlg, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
	if (IDOK == DlgRet)
	{
		CUIControl pControl;
		ZeroMemory(szConvBuffwchar, ((JC_STR_FOLDERPATH + 1) * 2));
		(*pdlg).GetFolderName(szConvBuffwchar, ((JC_STR_FOLDERPATH + 1) * 2));
		pstFolderData = &m_stStoredFolderList.pstFolderData[m_stStoredFolderList.dwCount];

		sh_strcpy((WCHAR*)(*pstFolderData).szFolderName, sizeof((*pstFolderData).szFolderName), szConvBuffwchar);

		(*pstFolderData).blHasPin = TRUE;
		m_stStoredFolderList.dwCount += 1;

		moveStoredData(enum_stored_kind_add);

		SetListViewStoredData();

		m_ps_sub->wDocFileFolder = (short)(m_stStoredFolderList.dwCount - 1);
		ZeroMemory(m_ps_sub->szFolderPass, sizeof(m_ps_sub->szFolderPass));

		SetUpdateFlag(TRUE);
	
	}

	if (pdlg != NULL)
	{
		delete pdlg;
		pdlg = NULL;
	}
	return TRUE;
}
// <S><E> To add Add folder Dialog in UD3L, 20200810 - SSDI:Goutham
// <S><E> Modified to add folder name in list view when ok is clicked in Add folder Dialog in UD3L, 20200827 - SSDI:Goutham
//=============================================================================
// function
//      SetUpdateFlag
//
// parameters
//      BOOL blState           
//
// return value
//      ê  None
//       
// outline
//      Set Update Flag
//=============================================================================

void CShDlgStored2::SetUpdateFlag(BOOL blState)
{
	m_blForceUpdate = blState;
}

//<S><A>Added to move and  set folder name data in the structure to list view,20200827 SSDI:Goutham
//=============================================================================
// function
//      moveStoredData
//
// parameters
//      DWORD    dwKind            
//
// return value
//      ê  TRUE/FALSE
//       
// outline
//      move Stored Data
//=============================================================================

BOOL CShDlgStored2::moveStoredData(DWORD dwKind)
{
	BOOL	blRet = FALSE;
	LPSTORED_FOLDER_DATA	pstFolderData = NULL;
	LPSTORED_FOLDER_DATA	pstFolderDataSrc = NULL;
	LPSTORED_FOLDER_DATA	pstFolderDataDstBase = NULL;
	LPSTORED_FOLDER_DATA	pstFolderDataDst = NULL;
	DWORD					dwCount;
	DWORD					dwIndex;
	DWORD					dwDataCount;
	DWORD					dwDelIndex;
	DWORD					dwItemIndex;
	LV_ITEM					item;

	dwCount = m_stStoredFolderList.dwCount;
	pstFolderDataSrc = m_stStoredFolderList.pstFolderData;

	
	dwDataCount = 1;								// Main Folder
	dwDelIndex = dwCount;
	for (dwIndex = 1; dwIndex < dwCount; dwIndex++) {	// Main Folder
		if ((*(pstFolderDataSrc + dwIndex)).blDelFlg == TRUE) {
			dwDelIndex = dwIndex;
		}
		else {
			dwDataCount++;
		}
	}
	
	pstFolderDataDst = new STORED_FOLDER_DATA[dwDataCount + 1];	// Add Folder
	if (pstFolderDataDst == NULL) {
		goto EXIT;
	}
	ZeroMemory(pstFolderDataDst, (sizeof(STORED_FOLDER_DATA)) * (dwDataCount + 1));
	pstFolderDataDstBase = pstFolderDataDst;

	//Main Folder
	memcpy(pstFolderDataDst, pstFolderDataSrc, sizeof(STORED_FOLDER_DATA));

	pstFolderDataDst++;

	for (dwItemIndex = 0; dwItemIndex < dwCount - 1; dwItemIndex++) {
		ZeroMemory(&item, sizeof(item));
		item.mask = LVIF_PARAM;
		item.iItem = (int)dwItemIndex;
		item.iSubItem = 0;
		(*m_plvFolderSel).getItem(&item);

		pstFolderData = (PSTORED_FOLDER_DATA)item.lParam;
		if (pstFolderData == NULL) {
			break;
		}

		if ((*pstFolderData).blDelFlg != TRUE) {
			memcpy(pstFolderDataDst, pstFolderData, sizeof(STORED_FOLDER_DATA));
			pstFolderDataDst++;
		}
	}

	if (dwKind == enum_stored_kind_add) {
		
		memcpy(pstFolderDataDst, (pstFolderDataSrc + dwCount - 1), sizeof(STORED_FOLDER_DATA));
	}

	
	delete[] pstFolderDataSrc;

	m_stStoredFolderList.dwCount = dwDataCount;
	m_stStoredFolderList.pstFolderData = pstFolderDataDstBase;

	blRet = TRUE;

EXIT:
	return blRet;
}
//=============================================================================
// function
//      SetListViewStoredData
//
// parameters
//      hWnd                
//      hWndCtrl            
//
// return value
//      ê  TRUE/FALSE
//       
//
// outline
//      BN_CLICKED - IDC_BTN_STRD_ADD SetListView Stored Data
//=============================================================================

BOOL CShDlgStored2::SetListViewStoredData(void)
{
	BOOL	blRet = FALSE;

	LPSTORED_FOLDER_DATA	pstFolderData = NULL;
	DWORD	dwCount;
	DWORD	dwIndex;

	dwCount = m_stStoredFolderList.dwCount;
	pstFolderData = m_stStoredFolderList.pstFolderData;
	pstFolderData++;

	(*m_plvFolderSel).deleteAllItems();

	// êMain Folder
	for (dwIndex = 1; dwIndex < dwCount; dwIndex++) {
		(*m_plvFolderSel).SetListviewItemStored(dwIndex - 1, pstFolderData);
		pstFolderData++;
	}

	blRet = TRUE;

	return blRet;
}
//<S><E>Added to move and set folder name data in the structure to list view,20200827 SSDI:Goutham

//<S><<A>Added to get the stored dialog registry data,20200827 SSDI:Goutham
//=============================================================================
// function
//      GetStoredDataReg
//
// parameters
//      void
//
// return value
//      êBOOL
//     
//
// outline
//     Function to get the stored dialog registry data.
//=============================================================================
BOOL CShDlgStored2::GetStoredDataReg(void)
{
	BOOL			blRet = FALSE;

	CShRegStored	*preg = NULL;
	DWORD			i;
	DWORD			dwCount;
	long			lRetRegRead = 0;
	LPSTORED_FOLDER_DATA	pstFolderData = NULL;
	WCHAR			szConvBuff[(JC_STR_FOLDERPATH + 1) * 2];

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pszSvrPrnName, szCommonDatFilePath, FALSE);
	CShJsonStored	*pjsonstr = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonstr = new CShJsonStored(ghInstance, m_pszSvrPrnName, m_hStringResourceHandle);
		pjsonstr->Init();
	}

	preg = new CShRegStored(m_hStringResourceHandle, m_pszSvrPrnName);
	if (preg == NULL)
	{
		goto EXIT;
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
		dwCount = (*pjsonstr).ReadStrdCount();
	else
		dwCount = (*preg).ReadStrdCount(m_pszSvrPrnName);

	if (dwCount == 0)
	{
		if ((*m_pmcf).IsWriteToJson() == TRUE)
		{
			(*pjsonstr).reset();
			dwCount = (*pjsonstr).ReadStrdCount();
		}
		else
		{
			(*preg).reset(m_pszSvrPrnName);
			dwCount = (*preg).ReadStrdCount(m_pszSvrPrnName);
		}

	}

	if (dwCount == 0)
		goto EXIT;
	
	
	pstFolderData = new STORED_FOLDER_DATA[dwCount + 1];	// Main Folderóp + Add Folderóp
	if (pstFolderData == NULL) {
		goto EXIT;
	}
	ZeroMemory(pstFolderData, (sizeof(STORED_FOLDER_DATA)) * (dwCount + 1));

	m_stStoredFolderListSv.dwCount = dwCount;				// Main Folder
	m_stStoredFolderListSv.pstFolderData = pstFolderData;

	
	pstFolderData = new STORED_FOLDER_DATA[dwCount + 1];	// Main Folderóp + Add Folderóp
	if (pstFolderData == NULL) {
		goto EXIT;
	}
	ZeroMemory(pstFolderData, (sizeof(STORED_FOLDER_DATA)) * (dwCount + 1));

	m_stStoredFolderList.dwCount = dwCount;
	m_stStoredFolderList.pstFolderData = pstFolderData;

	
	for (i = 0; i < dwCount; i++)
	{
		(*pstFolderData).blDelFlg = FALSE;
		(*pstFolderData).blHasPin = TRUE;
		ZeroMemory((*pstFolderData).szFolderName, ((JC_STR_FOLDERPATH + 1) * 2));
		ZeroMemory(szConvBuff, (JC_STR_FOLDERPATH + 1));

		if ((*m_pmcf).IsWriteToJson() == TRUE)
			(*pjsonstr).ReadStrdData(i, szConvBuff, ((JC_STR_FOLDERPATH + 1) * 2), &(*pstFolderData).blHasPin);
		else
			(*preg).ReadStrdData(m_pszSvrPrnName, i, szConvBuff, ((JC_STR_FOLDERPATH + 1) * 2), &(*pstFolderData).blHasPin);

		::wcscpy_s((WCHAR*)(*pstFolderData).szFolderName, sizeof((*pstFolderData).szFolderName) / sizeof(WCHAR), (WCHAR *)szConvBuff);
		
		pstFolderData++;
	}

	::memcpy(m_stStoredFolderListSv.pstFolderData, m_stStoredFolderList.pstFolderData, (sizeof(STORED_FOLDER_DATA))*(dwCount + 1));

	blRet = TRUE;

EXIT:
	if (NULL != preg)
	{
		delete preg;
		preg = NULL;
	}
	if (pjsonstr != NULL) {
		delete pjsonstr;
		pjsonstr = NULL;
	}
	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = NULL;
	}
	return blRet;
}

//<S><E>Added to get the stored dialog registry data,20200827 SSDI:Goutham

//=============================================================================
// function
//      SetSaveData
//
// parameters
//      WORD		 wVal
//
// return value
//      None
//
// outline
//		Set Save Data
//=============================================================================
void CShDlgStored2::SetSaveData(WORD wVal)
{
	m_wSaveFolderPass = wVal;
}
//=============================================================================
// function
//      IsSaveData
//
// parameters
//      Nill
//
// return value
//      Word
//
// outline
//		IsSaveData∑
//=============================================================================
WORD CShDlgStored2::IsSaveData(void)
{
	return m_wSaveFolderPass;
}

//=============================================================================
// function
//      GetCount
//
// parameters
//    None
//
// return value
//     DWORD
//
// outline
//		Get folder Count∑
//=============================================================================
DWORD CShDlgStored2::GetCount(void)
{
	return m_stStoredFolderList.dwCount;
}

//=============================================================================
// function
//      OnIdcBtnStrdDelFolder
//
// parameters
//      hWnd                Handler To Window
//      hWndCtrl            Handle to control window
//
// return value
//     LRESULT
//
// outline
//      BN_CLICKED - IDC_BTN_STRD_DELù
//=============================================================================
LRESULT CShDlgStored2::OnIdcBtnStrdDelFolder(HWND hWnd, HWND hWndCtrl)
{
	LRESULT				res = 0;
	PSTORED_FOLDER_DATA	pstFolderData = NULL;
	long				lIndex;
	LV_ITEM				item;

	lIndex = -1;
	while (1) 
	{
		lIndex = (*m_plvFolderSel).getNextItem(lIndex, LVNI_ALL | LVNI_SELECTED);
		
		if (lIndex == -1) {
			lIndex = (*m_plvFolderSel).getNextItem(lIndex, LVNI_ALL | LVIS_FOCUSED);
			if (lIndex == -1) {
				goto EXIT;
			}
		}
	
		ZeroMemory(&item, sizeof(item));
		item.mask = LVIF_PARAM;
		item.iItem = lIndex;
		item.iSubItem = 0;
		(*m_plvFolderSel).getItem(&item);

		pstFolderData = (PSTORED_FOLDER_DATA)item.lParam;
		(*pstFolderData).blDelFlg = TRUE;
		break;
	}

EXIT:
	
	moveStoredData(enum_stored_kind_del);

	SetListViewStoredData();

	if (m_stStoredFolderList.dwCount == 1) {
		m_ps_sub->wDocFileFolder = 0;
	}
	else {
		if (m_ps_sub->wDocFileFolder > (short)(m_stStoredFolderList.dwCount - 1))
			m_ps_sub->wDocFileFolder = (short)(m_stStoredFolderList.dwCount - 1);
	}

	ZeroMemory(m_ps_sub->szFolderPass, sizeof(m_ps_sub->szFolderPass));
	
	SetUpdateFlag(TRUE);

	return res;
}
//<S><C> To Modified the Function to Fix Saving And Retriving the Edit Box Password,2020-09-07,SSDi:Chanchal Singla
//=============================================================================
// function
//        OnIDOK
//
// parameters
//        hWnd         Handle to Window
//        wCtrlID      Control ID
//
// return value
//         TRUE
//        FALSE
//
// outline
//        OnIDOK
//=============================================================================
LRESULT CShDlgStored2::OnIDOK(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	LRESULT			res = 0;
	PSTORED_FOLDER_DATA	pstFolderData = NULL;
	LV_ITEM				item;
	long				lIndex = -1;
	CShowJobHandlingMessage* m_ObjShowJobHandlingMessage = NULL;
	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pszSvrPrnName, szCommonDatFilePath, FALSE);
	CShJsonStored	*pjsonstr = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonstr = new CShJsonStored(ghInstance, m_pszSvrPrnName, m_hStringResourceHandle);
		pjsonstr->Init();
	}

	UIControlMap::iterator iterUIComponents = pComponents->begin();
	pControl = (*pComponents)[IDC_EDT_STRD_FOLDER_PASS];
	if (NULL == pControl)
	{
		slog.putLog("CShDlgStored2::OnIDOK -> NULL_POINTER(CShDlgStored2)\n");
		return FALSE;
	}

	m_ObjShowJobHandlingMessage = new CShowJobHandlingMessage(hWnd, m_hStringResourceHandle);
	if (m_ObjShowJobHandlingMessage == NULL)
	{
		slog.putLog("CShDlgStored2::OnIDOK -> NULL_POINTER(CShDlgStored2)\n");
		return FALSE;
	}

	m_wSaveFolderPass = IsDlgButtonChecked(hWnd, IDC_CHK_STRD_FOLDER_ALWAYS_PASS);

	// Get the Password Edit Box Value
	((CJobControlEdit*)pControl)->getval(hWnd,m_ps_sub->szFolderPass, sizeof(m_ps_sub->szFolderPass), TRUE);
	
	// Check Password Edit Box Character is More than 5,then that function retruns Zero Value
	// But If password Character is less than 5 then message dialog is open and return any non Zero Value
	SHORT bReturn = m_ObjShowJobHandlingMessage->message(m_hWnd, m_ps_sub); 
	if (bReturn == 0)
	{
		res = CUIControl::OnIDOK(hWnd, hWndCtrl);

		if (res == IDOK)
		{
			CShRegStored	*preg = NULL;
			lIndex = (*m_plvFolderSel).getNextItem(lIndex, LVNI_ALL | LVNI_SELECTED);
			if (lIndex != -1)
			{
				ZeroMemory(&item, sizeof(item));
				item.mask = LVIF_PARAM;
				item.iItem = lIndex;
				item.iSubItem = 0;
				(*m_plvFolderSel).getItem(&item);

				pstFolderData = (PSTORED_FOLDER_DATA)item.lParam;
				
			}
			else 
			{
				pstFolderData = m_stStoredFolderList.pstFolderData;
				
			}
			if ((*m_pmcf).IsWriteToJson() == TRUE)
			{
				(*pjsonstr).WriteLastSelectedFolder((WCHAR*)(*pstFolderData).szFolderName, m_ps_sub->szFolderPass);
				pjsonstr->WriteJsonDataToFile();
			}
			else
			{
				(*preg).WriteLastSelectedFolder(m_pszSvrPrnName, (WCHAR*)(*pstFolderData).szFolderName, m_ps_sub->szFolderPass);
			}
			moveStoredData(enum_stored_kind_ok);
			SetStoredDataReg();	// data stored in registry
		}
	}
	
//EXIT:

	if (m_ObjShowJobHandlingMessage != NULL)
	{
		delete m_ObjShowJobHandlingMessage;
		m_ObjShowJobHandlingMessage = NULL;
	}
	// Only Restrict for closing the dialog,
	// When Entered Password is not a More than 5 character then the dialog is not closed,
	// but other scenarios condition is true for every time and dialog is closing when clicking the ok button.
	if ((m_blForceUpdate == TRUE) && (bReturn == 0))
	{
		::EndDialog(hWnd, wCtrlID);
	}

	if (pjsonstr != NULL) {
		delete pjsonstr;
		pjsonstr = NULL;
	}
	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = NULL;
	}

	return res;

}
//<E> To Modified the Function to Fix Saving And Retriving the Edit Box Password,2020-09-07,SSDi:Chanchal Singla
//=============================================================================
// function
//      SetStoredDataReg
//
// parameters
//      None
//
// return value
//       TRUE
//       FALSE
//
// outline
//     Set Stored Data Reg
//=============================================================================
BOOL CShDlgStored2::SetStoredDataReg(void)
{
	BOOL			blRet = FALSE;
	CShRegStored	*preg = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pszSvrPrnName, szCommonDatFilePath, FALSE);
	CShJsonStored	*pjsonstr = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonstr = new CShJsonStored(ghInstance, m_pszSvrPrnName, m_hStringResourceHandle);
		pjsonstr->Init();
	}

	BOOL			blFlg = FALSE;
	LPSTORED_FOLDER_DATA	pstFolderData = NULL;
	LPSTORED_FOLDER_DATA	pstFolderDataSv = NULL;
	DWORD					dwCount;
	DWORD					dwIndex;
	char					szConvBuff[(JC_STR_FOLDERPATH + 1) * 2];
	
	if (m_stStoredFolderList.dwCount == m_stStoredFolderListSv.dwCount) {
		pstFolderData = m_stStoredFolderList.pstFolderData;
		pstFolderDataSv = m_stStoredFolderListSv.pstFolderData;
		if (0 != ::memcmp(pstFolderData, pstFolderDataSv, sizeof(STORED_FOLDER_DATA) * m_stStoredFolderList.dwCount)) {
			blFlg = TRUE;
		}
	}
	else {
		blFlg = TRUE;
	}

	if (blFlg) 
	{
		dwCount = m_stStoredFolderList.dwCount;
		pstFolderData = m_stStoredFolderList.pstFolderData;

		for (dwIndex = 0; dwIndex < dwCount; dwIndex++) {
			
			if ((*m_pmcf).IsWriteToJson() == TRUE)
				(*pjsonstr).WriteStrdData(dwIndex, (WCHAR*)(*pstFolderData).szFolderName, (*pstFolderData).blHasPin);
			else
				(*preg).WriteStrdData(m_pszSvrPrnName, dwIndex, (WCHAR*)(*pstFolderData).szFolderName, (*pstFolderData).blHasPin);
			
			pstFolderData++;
		}

		if ((*m_pmcf).IsWriteToJson() == TRUE)
		{
			(*pjsonstr).WriteStrdCount(dwCount);
			pjsonstr->WriteJsonDataToFile();
		}
		else
		{
			(*preg).WriteStrdCount(m_pszSvrPrnName, dwCount);
		}
	}

	blRet = TRUE;

//EXIT:
	if (pjsonstr != NULL) {
		delete pjsonstr;
		pjsonstr = NULL;
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
//      OnWmNotify
//
// parameters
//        hWnd         handle to the Window that contains the control.
//        wParam       Word parameter
//        lParam       Long parameter
//
// return value
//      0
//
// outline
//      WM_NOTIFY ÉÅÉbÉZÅ[ÉWèàóù
//=============================================================================
LRESULT CShDlgStored2::OnWmNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LRESULT			res = 0;

	long			lCtrlID = (WORD)wParam;
	LPNMHDR			pnmh = (LPNMHDR)lParam;

	switch ((*pnmh).code)
	{
	case LVN_COLUMNCLICK:
		res = OnLvnColumnClick(hWnd, lCtrlID, pnmh);
		break;

	case LVN_ITEMCHANGED:
		res = OnLvnItemChanged(hWnd, lCtrlID, pnmh);
		break;

	case HDN_BEGINTRACKA:
	case HDN_BEGINTRACKW:
	case HDN_DIVIDERDBLCLICKA:
	case HDN_DIVIDERDBLCLICKW:
		res = TRUE;
		break;

	default:
		res = CUIControl::OnWmNotify(hWnd, wParam, lParam);
	}

	return res;
}

//=============================================================================
// function
//        OnLvnItemChg
//
// parameters
//        hWnd         handle to the Window that contains the control
//        wCtrlID      Control ID
//        pnmh         NMHDRç
//
// return value
//        ê¨å˜ : TRUE
//        é∏îs : FALSE
//
// outline
//        WM_NOTIFY - OnLvnColumnClick
//=============================================================================
LRESULT CShDlgStored2::OnLvnColumnClick(HWND hWnd, long lCtrlID, LPNMHDR pnmh)
{
	NM_LISTVIEW		*pNMLV = (NM_LISTVIEW *)pnmh;
	long			lIndex = -1;
	PSTORED_FOLDER_DATA	pstFolderData = NULL;

	switch (lCtrlID)
	{
	case IDC_LISTV_STRD_FOLDER_SEL:
		(*m_plvFolderSel).sortItems((*pNMLV).iSubItem);
		
		lIndex = (*m_plvFolderSel).getNextItem(lIndex, LVNI_ALL | LVNI_SELECTED);
		if (lIndex != -1) {
			m_ps_sub->wDocFileFolder = (short)lIndex + 1;
		}
		else {
			lIndex = (*m_plvFolderSel).getNextItem(lIndex, LVNI_ALL | LVIS_FOCUSED);
			if (lIndex != -1) {
				m_ps_sub->wDocFileFolder = (short)lIndex + 1;
			}
			else {
				m_ps_sub->wDocFileFolder = (short)0;
			}
		}
		break;

	default:
		break;
	}

	return TRUE;
}

//=============================================================================
// function
//        OnLvnItemChanged
//
// parameters
//        hWnd         handle to the Window that contains the control
//        wCtrlID      Control ID
//        pnmh         NMHDRç
//
// return value
//         TRUE
//        FALSE
//
// outline
//        WM_NOTIFY - OnLvnItemChanged
//=============================================================================
LRESULT CShDlgStored2::OnLvnItemChanged(HWND hWnd, long lCtrlID, LPNMHDR pnmh)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	NM_LISTVIEW		*pNMLV = (NM_LISTVIEW *)pnmh;
	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	pControl = (*pComponents)[IDC_EDT_STRD_FOLDER_PASS];
	if (NULL == pControl)
	{
		slog.putLog("CShDlgStored2::OnIDOK -> NULL_POINTER(CShDlgStored2)\n");
		return FALSE;
	}


	if (!((*pNMLV).uNewState & LVIS_FOCUSED) && ((*pNMLV).uOldState & LVIS_FOCUSED))
	{
		m_dwEvnetFlg |= STRD_LV_EVENT_A;
		(*m_plvFolderSel).setItemState((*pNMLV).iItem, 0, (LVIS_SELECTED | LVIS_FOCUSED));
	}
	else if (!((*pNMLV).uNewState & LVIS_SELECTED) && ((*pNMLV).uOldState & LVIS_SELECTED))
	{
		m_dwEvnetFlg |= STRD_LV_EVENT_B;
		if (!(m_dwEvnetFlg & STRD_LV_EVENT_A))
		{
			(*m_plvFolderSel).setItemState((*pNMLV).iItem, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
	else if (((*pNMLV).uNewState & LVIS_SELECTED) && !((*pNMLV).uOldState & LVIS_SELECTED))
	{
		m_dwEvnetFlg |= STRD_LV_EVENT_C;
		if (m_dwEvnetFlg == (STRD_LV_EVENT_B | STRD_LV_EVENT_C)) 
		{
			m_dwEvnetFlg = STRD_LV_EVENT_NONE;
		}
		else {
			if (m_ps_sub->wDocFileFolder != (*pNMLV).iItem + 1) 
			{
				SecureZeroMemory(m_ps_sub->szFolderPass, sizeof(m_ps_sub->szFolderPass));
				// --- folder pass code
				((CJobControlEdit*)pControl)->setval(hWnd, m_ps_sub->szFolderPass, TRUE);
				m_ps_sub->wDocFileFolder = (*pNMLV).iItem + 1;
				SetTabState(hWnd);
			}
			m_dwEvnetFlg = STRD_LV_EVENT_NONE;
		}
	}

	return TRUE;

}
//<S><A> To Add teh Support of Custom folder Edit Box Conrol ,2020-09-07,SSDi:Chanchal Singla
// ============================================================================
// function
//      OnEnChange
//
// parameters
//      hWnd               Handle To Window
//      hWndCtrl           Handle To Control
//      wCtrlID            Control id
//
// return value
//      êTRUE
//       FALSE
//
// outline
//      WM_COMMAND - EN_CHANGE
//=============================================================================
LRESULT CShDlgStored2::OnEnChange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	switch (wCtrlID)
	{
	case IDC_EDT_STRD_FOLDER_PASS:
	{
		pControl = (*pComponents)[wCtrlID];
		((CJobControlEdit*)pControl)->OnEnChange(wCtrlID, hWnd);
	}
	break;
	default:
		break;
	}
	res = 1;
	SetTabState(hWnd);
	return res;
}
//<E> To Add teh Support of Custom folder Edit Box Conrol ,2020-09-07,SSDi:Chanchal Singla
