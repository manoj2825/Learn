// ============================================================================
// sh_tabdlg_frame.cpp
//
//     Copyright 2020 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2014.08.18
// ============================================================================

#include "precomp.h"
#include "ShTabdlgFrame.h"
#include "shregus.h"
#include "shJsonus.h"
//For Logging Purpose
#include "MFPLogger.h"


//=============================================================================
// function
//      CShTabDlgFrame
//
// parameters
//      hInst               ÉCÉìÉXÉ^ÉìÉXÉnÉìÉhÉã
//      pLog                CShPrnLogÉNÉâÉXÉAÉhÉåÉX
//      wID                 ÉEÉBÉìÉhÉEÅiÉRÉìÉgÉçÅ[ÉãÅjID
//
// return value
//      ñ≥Çµ
//
// outline
//      CShTabDlgFrame ÉNÉâÉXÉRÉìÉXÉgÉâÉNÉ^
//=============================================================================
CShTabDlgFrame::CShTabDlgFrame(HINSTANCE hInst, HWND hWndParent, char FAR *pszSvrPrnName, short wID)
{
	m_pBtnAttcLeft = NULL;	// <
	m_pBtnAttcRight = NULL;	// >
	m_buttons = NULL;
	m_currentPage = 0;
	m_wBtnCount = 0;

	m_wTopTab  = 0;
	m_Reserved1= 0;
	m_wID = IDD_TAB_FRAME_00_00;
	m_hPrinter = NULL;

	//Coverity Fix 90373 - SSDI:Seetharam - 20200827
	m_pOemPrivateDevMode = NULL;
	m_pOemPublicDevMode = NULL;
	m_pPrivateDevmode = NULL;
	m_hDiscoveryStringResourceHandle  = NULL;
	m_eColorModeValue = eMonoMode;
	m_nCurrTabId = 0;
	m_pBtnAttcLeft = NULL;
	m_pBtnAttcRight = NULL;
	m_buttons = NULL;
	m_currentPage = -1;
	m_wBtnCount = 0;
	m_wTopTab = 0;
	m_Reserved1 = 0;
	m_eDeviceType = eMono;
	m_pInformationIconButton = NULL;
	m_hcsHandUp = NULL;
	m_hcsHandDown = NULL;
	m_hPropertySheetHandle = NULL;
	m_ObjCShowMessage = NULL;
	m_hPage = NULL;
	m_hComPropSheet = NULL;
	m_pfnComPropSheet = NULL;
	m_hWnd = NULL;
	m_pHelpFilePath = NULL;
	m_pWaterMarkIndex = NULL;
	m_ptabInfoCtrl = NULL;
	m_hPrevWnd = NULL;
	m_nPrevTabId = 0;
	m_pUsIndex = NULL;
	m_ppi = NULL;
	//Coverity Fix 90373 - SSDI:Seetharam - 20200907
	SecureZeroMemory(&m_InformationIconState, sizeof(INFORMATIONICONSTATE));
	m_pPrintCoreHelperUni = NULL;
	m_pResourceIDInfoMap = NULL;
	m_ConstraintFeatureInfoMap = NULL;
	m_pFeatureInfoList = NULL;
	m_hStringResourceHandle = NULL;
	m_hBitmapResourceHandle = NULL;
	m_hDialogResourceHandle = NULL;
	m_pHelpID = NULL;
	m_blLimitByOption = bool_false;
	//<S><A>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
	//Patent avoidance message is not displayed
	m_blFavoritesByOptionCurrentJob = TRUE;
	//<E>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
}

CShTabDlgFrame::CShTabDlgFrame(HMODULE hStringResourceHandle, HMODULE hBitmapResourceHandle, HMODULE hDialogResourceHandle, 
	IPrintCoreHelperUni* pPrintCoreHelperUni, ResourceIDInfoMap* pResourceIDInfoMap, ConstraintFeatureInfoMap &ConstraintFeatureInfoMap, 
	PFEATUREINFOLIST &pFeatureInfoList, PaperInfoMap PaperInfo, CustomPaperInfoMap CustomPaperInfoList, 
	POEMDEV pOemPrivateDevMode, PDEVMODE pOemPublicDevMode, LPTSTR pHelpFilePath, DWORD *pHelpID, std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings,
	POEMDEV pPrivateDevmode, HANDLE hPrinter, WCHAR* pPrinterName, SHORT& sWaterMarkIndex, SHORT& wUsIndex, PPROPINFO ppi)
{
	m_wID = IDD_TAB_FRAME_00_00;
	m_hStringResourceHandle = hStringResourceHandle;
	m_hBitmapResourceHandle = hBitmapResourceHandle;
	m_hDialogResourceHandle = hDialogResourceHandle;
	m_pPrintCoreHelperUni = pPrintCoreHelperUni;
	m_pResourceIDInfoMap = pResourceIDInfoMap;
	m_ConstraintFeatureInfoMap = &ConstraintFeatureInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	m_PaperInfo = PaperInfo;
	m_CustomPaperInfoList = CustomPaperInfoList;
	m_pOemPrivateDevMode = pOemPrivateDevMode;
	m_pOemPublicDevMode = pOemPublicDevMode;
	m_pHelpFilePath = pHelpFilePath;
	m_pHelpID = pHelpID;
	m_DefaultSettings = pDefaultSettings;
	m_pPrivateDevmode = pPrivateDevmode;
	m_eColorModeValue = eAutoMode;
	m_hPrinter = hPrinter;
	StringCbCopy(m_pPrinterName, MAX_PATH * sizeof(WCHAR), pPrinterName);
	m_pWaterMarkIndex = &sWaterMarkIndex;
	m_pUsIndex = &wUsIndex;
	m_ppi = ppi;
	m_ptabInfoCtrl = NULL;
	m_hPrevWnd = NULL;
	m_nPrevTabId = 0;

	//Coverity Fix 90461 - SSDI:Seetharam - 20200827
	m_nCurrTabId = 0;
	m_pBtnAttcLeft = NULL;
	m_pBtnAttcRight = NULL;
	m_buttons = NULL;
	m_currentPage = -1;
	m_wBtnCount = 0;
	m_wTopTab = 0;
	m_Reserved1 = 0;
	m_hDiscoveryStringResourceHandle = NULL;
	m_eDeviceType = eMono;
	m_pInformationIconButton = NULL;
	m_hcsHandUp = NULL;
	m_hcsHandDown = NULL;
	m_hPropertySheetHandle = NULL;
	m_ObjCShowMessage = NULL;
	m_hPage = NULL;
	m_hComPropSheet = NULL;
	m_pfnComPropSheet = NULL;
	m_hWnd = NULL;
	m_blLimitByOption = bool_false;
	//<S><A>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
	//Patent avoidance message is not displayed
	m_blFavoritesByOptionCurrentJob = TRUE;
	//<E>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
	//Coverity Fix 90386 - SSDI:Seetharam - 20200907
	SecureZeroMemory(&m_InformationIconState, sizeof(INFORMATIONICONSTATE));
}

//=============================================================================
// function
//      ~CShTabDlgFrame
//
// parameters
//      ñ≥Çµ
//
// return value
//      ñ≥Çµ
//
// outline
//      CShTabDlgFrame ÉNÉâÉXÉfÉXÉgÉâÉNÉ^
//=============================================================================
CShTabDlgFrame::~CShTabDlgFrame()
{
	SHORT i = 0;

	if (m_pBtnAttcLeft != NULL)
	{
		delete m_pBtnAttcLeft;
	}

	if (m_pBtnAttcRight != NULL)
	{
		delete m_pBtnAttcRight;
	}

	if(m_buttons != NULL) {
		for (i = 0; i < m_wBtnCount; i++) {
			if(m_buttons[i] != NULL){
				delete m_buttons[i];
			}
 		}
		delete[] m_buttons;
	}

	if (m_ptabInfoCtrl != NULL)
	{
		delete m_ptabInfoCtrl;
		m_ptabInfoCtrl = NULL;
	}
}

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
CShTabDlgFrame::InitDlgBox(
	_Out_ LPCTSTR* ppszTemplate,
	_Out_ LPCTSTR* ppszTitle
)
{
	HRESULT hr = S_OK;

	//Coverity Fix 90469 - SSDI:Seetharam - 20200908
	//Changed the condition from || to && because if any parameter is NULL, then
	//dereference happens and application can crash
	//if ((NULL != ppszTemplate) ||
	if ((NULL != ppszTemplate) &&
		(NULL != ppszTitle))
	{
		*ppszTemplate = MAKEINTRESOURCE(m_wID);
		if (LoadString(m_hStringResourceHandle, m_wID, m_szItem, countof(m_szItem)) > 0)
		{
			*ppszTitle = m_szItem;
		}
	}
	return hr;
}

HRESULT
CShTabDlgFrame::PropPageInit(
	_In_ CONST PPROPSHEETUI_INFO pPSUIInfo
)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (NULL == pPSUIInfo)
	{
		slog.putLog("CShTabDlgFrame::PropPageInit -> E_FAIL(pPSUIInfo)\n");
		return E_FAIL;
	}

	//
	// Only proceed if we have effectively published the helper interfaces to the UI
	// control objects.
	//
	// We also need to retrieve the dialog template resource and dialog title from the
	// derived property page class.
	//
	PROPSHEETPAGE page = { 0 };
	if (SUCCEEDED(hr = InitDlgBox(&page.pszTemplate, &page.pszTitle)))
	{
		page.dwSize = sizeof(PROPSHEETPAGE);
		page.dwFlags = PSP_DEFAULT | PSP_USETITLE /*|PSH_NOAPPLYNOW*/ /*| PSP_HASHELP*/;
		page.hInstance = m_hDialogResourceHandle;

		page.pfnDlgProc = CShTabDlgFrame::DlgProc;
		page.hActCtx = GetMyActivationContext(m_hDialogResourceHandle);
		if ((NULL != page.hActCtx) && (INVALID_HANDLE_VALUE != page.hActCtx))
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
			if (S_OK != hr)
			{
				slog.LoggingErrorHResultValues();
			}
		}
	}

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
CShTabDlgFrame::SetComPropSheetFunc(
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
CShTabDlgFrame::SetPageHandle(
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
HRESULT CShTabDlgFrame::SetComPropSheetHandle(	_In_ CONST HANDLE hComPropSheet )
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
HRESULT CShTabDlgFrame::GetComPropSheetFunc( _Outptr_ PFNCOMPROPSHEET* ppfnComPropSheet )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;

	if (NULL != ppfnComPropSheet)
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
HANDLE CShTabDlgFrame::GetPageHandle( VOID )
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
HANDLE CShTabDlgFrame::GetComPropSheetHandle( VOID )
{
	return m_hComPropSheet;
}

//=============================================================================
// function
//      OnBnClicked
//
// parameters
//      hWnd                ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//      hWndCtrl            ëŒè€ÉRÉìÉgÉçÅ[ÉãÇÃÉnÉìÉhÉã
//      wCtrlID             ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//
// return value
//      0
//
// outline
//      BN_CLICKED ÉÅÉbÉZÅ[ÉWèàóù
//=============================================================================
LRESULT CShTabDlgFrame::OnBnClicked(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	PPROPSTATE		pps;
	HWND	hWndWork = NULL;
	SHORT			dispCount = 0;

	dispCount = m_ptabInfoCtrl->GetDispCount();
	if (wCtrlID == IDC_BTN_TABBTN_LEFT || wCtrlID == IDC_BTN_TABBTN_RIGHT)
	{
		OnUpDown(hWnd, hWndCtrl, wCtrlID);
		//m_blForceUpdate = TRUE;
		//m_blIgnorePropSheet = TRUE;
 	}
	else if ((IDC_BTN_TABBTN_1 <= wCtrlID) && (wCtrlID <= (IDC_BTN_TABBTN_1 + dispCount)))
	{
		m_nCurrTabId = wCtrlID - IDC_BTN_TABBTN_1;
		OnChangeDetailSetting(hWnd, hWndCtrl, wCtrlID);
	}
 
	res = 1;

//EXIT:

	return res;
}


//=============================================================================
// function
//        OnPsnHelp
//
// parameters
//        hWnd         ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//        wCtrlID      ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//        pnmh         NMHDRç\ë¢ëÃÉAÉhÉåÉX
//
// return value
//        ê¨å˜ : TRUE
//        é∏îs : FALSE
//
// outline
//        WM_NOTIFY - PSN_HELP
//=============================================================================
LRESULT CShTabDlgFrame::OnPsnHelp(HWND hWnd, long lCtrlID, LPPSHNOTIFY ppsn)
{
	CShTabInfoControl	*pTabInfoCtrl = NULL;
	TABINFOCTRLTBL		*pTabInfoTbl = NULL;
	CDocumentPropertyPage			*pTabDlg = NULL;
	SHORT				wNowTabIndex = 0;
	short				wID = 0;

	//if(m_ppi != NULL)
	{
		//puic = (PUIIFCLS)GetInfoAddr(m_hWnd, PROP_UIIFCLS_STR);
		//if(puic == NULL)
		//	goto EXIT;

	//	pTabInfoCtrl = (*puic).ptabInfoCtrl;
	//	if(pTabInfoCtrl == NULL)
	//		goto EXIT;

	//	wNowTabIndex = (*pTabInfoCtrl).GetWkTabIndex();
	//	pTabInfoTbl = (*pTabInfoCtrl).getTabInfoCtrlTbl(wNowTabIndex);
	//	pTabDlg = (*pTabInfoTbl).pTabDlg;
	//	wID = (*pTabDlg).GetResourceID();

	//	if((*m_ppi).blHtmlHelp != FALSE)
	//		HtmlHelp(hWnd, (*m_ppi).szHelpFile, HH_HELP_CONTEXT, (DWORD)GetContextHelpID(wID, (*m_ppi).pHelp));
	//	else
	//		::WinHelp(hWnd, (*m_ppi).szHelpFile, HELP_CONTEXT, GetContextHelpID(wID, (*m_ppi).pHelp));
	//}

	//// for debug
	//if((*m_ppi).blOutputHelpInfo != FALSE)
	//{
	//	char	szInfo[256];

	//	::ZeroMemory(szInfo, sizeof(szInfo));
	//	::wsprintf(szInfo, "WM_NOTIFY - PSN_HELP / coltrol ID : %d - HelpID : %d\n", wID, GetContextHelpID(wID, (*m_ppi).pHelp));
	//	::OutputDebugString(szInfo);
	}
//EXIT:
	return 1;//CShDialog::OnPsnHelp(hWnd, lCtrlID, ppsn);
}


//=============================================================================
// function
//        OnPsnApply
//
// parameters
//        hWnd         ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//        wCtrlID      ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//        pnmh         NMHDRç\ë¢ëÃÉAÉhÉåÉX
//
// return value
//        ê¨å˜ : TRUE
//        é∏îs : FALSE
//
// outline
//        WM_NOTIFY - PSN_APPLY
//=============================================================================
LRESULT CShTabDlgFrame::OnPsnApply(HWND hWnd, long lCtrlID, LPPSHNOTIFY ppsn)
{
	//CShTabInfoControl	*pTabInfoCtrl = NULL;
	TABINFOCTRLTBL		*pTabInfoTbl = NULL;
	CDocumentPropertyPage			*pTabDlg = NULL;
	short				wTabCount;
	short				i;
	SHORT				wIndexNow;
	SHORT				wPsnKillActiveEX = 0L;
	//// WM_NOTIFY - PSN_KILLACTIVEÇÃèàóù
	//Coverity Fix 90503 - SSDI:Seetharam - 20200908
	//Updated from pTabInfoCtrl to m_ptabInfoCtrl to fix the issue
	//wIndexNow = (*pTabInfoCtrl).GetWkTabIndex();				
	wIndexNow = m_ptabInfoCtrl->GetWkTabIndex();		// çTab ÃIndex
	pTabInfoTbl = m_ptabInfoCtrl->getTabInfoCtrlTbl(wIndexNow);	// çTabTable
	pTabDlg = (CDocumentPropertyPage *)pTabInfoTbl->pTabDlg;
	if (pTabDlg != NULL)
	{

		switch (wIndexNow)
		{
		case 0:		//IDD_TAB_MAIN_121_03:
			wPsnKillActiveEX = ((CMainTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		case 1:		//IDD_TAB_PAPER_03:
			wPsnKillActiveEX = ((CPaperTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		case 2:		//IDD_TAB_FINISHING_01_03:
			wPsnKillActiveEX = ((CFinishingTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		case 3:		//IDD_TAB_LAYOUT_01_03:
			wPsnKillActiveEX = ((CLayoutTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		case 4:		//IDD_TAB_JOBHAND_0:
			wPsnKillActiveEX = ((CJobHandlingTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		default:
			wPsnKillActiveEX = (*pTabDlg).OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		}
	}
	//<S><C>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	//Known Issue : Printing Preferences window is getting closed, 
	//when OK is clicked on any validation message box in Job-Handling tab. 
	if(wPsnKillActiveEX == FALSE) {
		SetWindowLongPtr(hWnd, DWLP_MSGRESULT, TRUE);
		//SetWindowLongPtr(pTabInfoTbl->hWndTabDlg, DWLP_MSGRESULT, TRUE);
		return TRUE;
	}
	//<E>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	wTabCount = (*m_ptabInfoCtrl).GetTabCount();
	for (i = 0; i < wTabCount; i++) {
		pTabInfoTbl = (*m_ptabInfoCtrl).getTabInfoCtrlTbl(i);
		pTabDlg = (CDocumentPropertyPage *)(*pTabInfoTbl).pTabDlg;
		if (pTabDlg != NULL)
		{
			
			switch (i)
			{
			case 0:		//IDD_TAB_MAIN_121_03:
				((CMainTabPropertyPage *)pTabDlg)->OnPsnApplyEX(hWnd, lCtrlID, ppsn);
				break;
			case 1:		//IDD_TAB_PAPER_03:
				((CPaperTabPropertyPage *)pTabDlg)->OnPsnApplyEX(hWnd, lCtrlID, ppsn);
				break;
			case 2:		//IDD_TAB_FINISHING_01_03:
				((CFinishingTabPropertyPage *)pTabDlg)->OnPsnApplyEX(hWnd, lCtrlID, ppsn);
				break;
			case 3:		//IDD_TAB_LAYOUT_01_03:
				((CLayoutTabPropertyPage *)pTabDlg)->OnPsnApplyEX(hWnd, lCtrlID, ppsn);
				break;
			case 4:		//IDD_TAB_JOBHAND_0:
				 ((CJobHandlingTabPropertyPage *)pTabDlg)->OnPsnApplyEX(hWnd, lCtrlID, ppsn);
				break;
			default:
				(*pTabDlg).OnPsnApplyEX(hWnd, lCtrlID, ppsn);
				break;
			}
		}
	}
	return OnPsnApply(hWnd);
}

HRESULT CShTabDlgFrame::OnPsnApply(_In_ HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	PFNCOMPROPSHEET pfnComPropSheet = NULL;
	CShTabDlgFrame* pPropertyPageInstance = NULL;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
		(NULL != pPropertyPageInstance) &&
		SUCCEEDED(hResult = pPropertyPageInstance->GetComPropSheetFunc(&pfnComPropSheet)) &&
		(NULL != pPropertyPageInstance)
		)
	{
		//To disable the Apply button
		PropSheet_UnChanged(GetParent(hDlg), hDlg);
		//<S><A>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
		//Patent avoidance message is not displayed
		if (m_blFavoritesByOptionCurrentJob != FALSE)
		//<E>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
		{
			//<S><A> To Fix  PJL Generation and Updation of Devmode,29-04-2020,SSDI: Chanchal Singla
			//<S><C>Reverted the code to documentpropertypage instance from tabframe instance, 3/6/2020,K M Harika
			CDocumentPropertyPage* pPageInstance = m_ptabInfoCtrl->getTabCtrl(m_ptabInfoCtrl->GetCurrentPosition());
			pPageInstance->SaveFeatureOptions();
			//pPropertyPageInstance->SaveFeatureOptions();
			//<E>Reverted the code to documentpropertypage instance from tabframe instance, 3/6/2020,K M Harika
			//<E> To Fix  PJL Generation and Updation of Devmode,29-04-2020,SSDI: Chanchal Singla
		}
// <S><D> As per SOL2 suggestion, Reverted code fix of  feedback Issue #72( Word-1 ) ,2023.03.31, SSDI:Chandrashekar .V
//#if 0 
// <S><A> To Fix feedback Issue #72,2023.03.17, SSDI:Chandrashekar .V
		//FillPubModeData()
		/*pPropertyPageInstance->m_pOemPublicDevMode->dmFields = pPropertyPageInstance->m_pOemPrivateDevMode->scpi.pub.dmFields;
		pPropertyPageInstance->m_pOemPublicDevMode->dmPaperSize=	pPropertyPageInstance->m_pOemPrivateDevMode->scpi.pub.dmPaperSize;
		pPropertyPageInstance->m_pOemPublicDevMode->dmPaperLength = pPropertyPageInstance->m_pOemPrivateDevMode->scpi.pub.dmPaperLength;
		pPropertyPageInstance->m_pOemPublicDevMode->dmPaperWidth = pPropertyPageInstance->m_pOemPrivateDevMode->scpi.pub.dmPaperWidth;
		wcscpy_s(pPropertyPageInstance->m_pOemPublicDevMode->dmFormName, MAX_FORMNAME_LENGTH + 1, pPropertyPageInstance->m_pOemPrivateDevMode->scpi.pub.dmFormName);*/
// <E> To Fix feedback Issue #72,2023.03.17, SSDI:Chandrashekar .V
//#endif
// <E>  Reverted code fix of  feedback Issue #72( Word-1 ) ,2023.03.31, SSDI:Chandrashekar .V

		pfnComPropSheet(pPropertyPageInstance->GetComPropSheetHandle(),
			CPSFUNC_SET_RESULT,
			reinterpret_cast<LPARAM>(pPropertyPageInstance->GetPageHandle()),
			(LPARAM)CPSUI_OK);
	}
	if (S_OK != hResult)
	{
		slog.LoggingErrorHResultValues();
	}

	return hResult;
}

LRESULT CShTabDlgFrame::OnPsnApplyEX(HWND hWnd, long lCtrlID, LPPSHNOTIFY ppsn)
{
	return 1;
}
//=============================================================================
// function
//      SetTabState
//
// parameters
//      ñ≥Çµ
//
// return value
//      ê¨å˜Å@: TRUE
//      é∏îsÅ@: FALSE
//
// outline
//      É^ÉuÇÃèÛë‘Çê›íËÇ∑ÇÈ
//=============================================================================
BOOL CShTabDlgFrame::SetTabState(void)
{
	if (m_ppi->pps->blLimitByOption == bool_false) {
		if ((*m_ppi).blHideAllControl == TRUE) {
			::ShowWindow(GetDlgItem(m_hWnd,IDC_BTN_CLEAR), SW_SHOW);
		}
	}

	for (int i = 0; i < 5; i++)
	{
		m_buttons[i]->OnInit(IDC_BTN_TABBTN_1 + i, m_hWnd);
	}
	return TRUE;
}

//Creates the tab for the given index
CDocumentPropertyPage* CShTabDlgFrame::makeTabProc(int index)
{
	switch (index)
	{
		case 0:
			//<S><C>Changed 10th parameter from m_pPrivateDevMode to m_pOemPrivateDevMode, 3/6/2020, K M Harika
			return new (std::nothrow) CMainTabPropertyPage(m_hStringResourceHandle, m_hBitmapResourceHandle, m_hDialogResourceHandle, m_pPrintCoreHelperUni, m_pResourceIDInfoMap, *m_ConstraintFeatureInfoMap,
				m_pFeatureInfoList, m_PaperInfo, m_CustomPaperInfoList, m_pOemPrivateDevMode, m_pOemPublicDevMode, m_pHelpFilePath, m_pHelpID, m_DefaultSettings, m_pPrivateDevmode, m_hPrinter, m_pPrinterName, *m_pWaterMarkIndex, *m_pUsIndex, m_ppi);
			break;
		case 1:
			return new (std::nothrow) CPaperTabPropertyPage(m_hStringResourceHandle, m_hBitmapResourceHandle, m_hDialogResourceHandle, m_pPrintCoreHelperUni, m_pResourceIDInfoMap, *m_ConstraintFeatureInfoMap,
				m_pFeatureInfoList, m_PaperInfo, m_CustomPaperInfoList, m_pOemPrivateDevMode, m_pOemPublicDevMode, m_pHelpFilePath, m_pHelpID, m_DefaultSettings, m_pPrivateDevmode, m_hPrinter, m_pPrinterName, *m_pWaterMarkIndex, *m_pUsIndex, m_ppi);
			break;
		case 2:
			return new (std::nothrow) CFinishingTabPropertyPage(m_hStringResourceHandle, m_hBitmapResourceHandle, m_hDialogResourceHandle, m_pPrintCoreHelperUni, m_pResourceIDInfoMap, *m_ConstraintFeatureInfoMap,
				m_pFeatureInfoList, m_PaperInfo, m_CustomPaperInfoList, m_pOemPrivateDevMode, m_pOemPublicDevMode, m_pHelpFilePath, m_pHelpID, m_DefaultSettings, m_pPrivateDevmode, m_hPrinter, m_pPrinterName, *m_pWaterMarkIndex, *m_pUsIndex, m_ppi);
			break;
		case 3:
			return new (std::nothrow) CLayoutTabPropertyPage(m_hStringResourceHandle, m_hBitmapResourceHandle, m_hDialogResourceHandle, m_pPrintCoreHelperUni, m_pResourceIDInfoMap, *m_ConstraintFeatureInfoMap,
				m_pFeatureInfoList, m_PaperInfo, m_CustomPaperInfoList, m_pOemPrivateDevMode, m_pOemPublicDevMode, m_pHelpFilePath, m_pHelpID, m_DefaultSettings, m_pPrivateDevmode, m_hPrinter, m_pPrinterName, *m_pWaterMarkIndex, *m_pUsIndex, m_ppi);
			break;
		case 4:
			return new (std::nothrow) CJobHandlingTabPropertyPage(m_hStringResourceHandle, m_hBitmapResourceHandle, m_hDialogResourceHandle, m_hPropertySheetHandle, m_pPrintCoreHelperUni, m_pResourceIDInfoMap, *m_ConstraintFeatureInfoMap,
				m_pFeatureInfoList, m_PaperInfo, m_CustomPaperInfoList, m_pOemPrivateDevMode, m_pOemPublicDevMode, m_pHelpFilePath, m_pHelpID, m_DefaultSettings, m_pPrivateDevmode, m_hPrinter, m_pPrinterName, *m_pWaterMarkIndex, *m_pUsIndex,m_ppi);
			break;
			//<E>Changed 10th parameter from m_pPrivateDevMode to m_pOemPrivateDevMode, 3/6/2020, K M Harika
		default:
			return NULL;
			break;
	}
}

void CShTabDlgFrame::InitTabInfoControl(void)
{
	//Initialize the tabinfoctrl and propertypages inside
	m_ptabInfoCtrl = new CShTabInfoControl(ghInstance, m_pPrinterName);
	m_ptabInfoCtrl->Init(this);

	CDocumentPropertyPage *pTabDlg = NULL;
	TABINFOCTRLTBL *pTabInfoTbl = (*m_ptabInfoCtrl).getTabInfoCtrlTbl(0);
	if (pTabInfoTbl != NULL) {
		pTabDlg = (*pTabInfoTbl).pTabDlg;
		if (pTabDlg != NULL) {
			(*pTabInfoTbl).hWndTabDlg = ::CreateDialogParam(m_hDialogResourceHandle, MAKEINTRESOURCE((*pTabDlg).GetResourceID()), m_hWnd, (DLGPROC)(*pTabDlg).DlgProc, (LPARAM)pTabDlg);
			if ((*pTabInfoTbl).hWndTabDlg != NULL) {
				HWND hDst = (*pTabInfoTbl).hWndTabDlg;
				m_hPrevWnd = hDst;
				m_nPrevTabId = (*pTabInfoTbl).wTabNo;
			}
			((CMainTabPropertyPage *)pTabDlg)->ActivateUIControl(pTabInfoTbl->hWndTabDlg);
		}
	}
}

//=============================================================================
// function
//      CreateCtrlClass
//
// parameters
//      ñ≥Çµ
//
// return value
//      ê¨å˜Å@: TRUE
//      é∏îsÅ@: FALSE
//
// outline
//      É^ÉuÇ…égópÇ∑ÇÈäeÉRÉìÉgÉçÅ[ÉãópÉNÉâÉXÇçÏê¨Ç∑ÇÈ
//=============================================================================
BOOL CShTabDlgFrame::CreateCtrlClass(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	BOOL			res = FALSE;

	m_pBtnAttcLeft = new CStaticText(IDC_BTN_TABBTN_LEFT, m_hStringResourceHandle);
	EnableWindow(GetDlgItem(m_hWnd, IDC_BTN_TABBTN_LEFT), FALSE);
	m_pBtnAttcRight = new CStaticText(IDC_BTN_TABBTN_RIGHT, m_hStringResourceHandle);
	EnableWindow(GetDlgItem(m_hWnd, IDC_BTN_TABBTN_RIGHT), FALSE);

	m_buttons = new CShBtnBmpTxt* [5];
	if (m_buttons == NULL)
		goto EXIT;

	m_wBtnCount = 5;
	for (int i = 0; i < 5; i++)
	{
		m_buttons[i] = new CShBtnBmpTxt(
			m_hStringResourceHandle,
			m_hBitmapResourceHandle,
			m_hDialogResourceHandle,
			IDC_BTN_TABBTN_1 + i,
			::GetDlgItem(m_hWnd, IDC_BTN_TABBTN_1 + i)
		);
		if (m_buttons == NULL)
			goto EXIT;
	}

	(*m_buttons[0]).SetTabStop();
	(*m_ppi).hWnd_Tab = m_hWnd;
	res = TRUE;

EXIT:
	return res;
}

HRESULT CShTabDlgFrame::InitializeUIControls(_In_ CONST HWND   hDlg)
{
	m_pBtnAttcLeft->OnInit(IDC_BTN_TABBTN_LEFT, hDlg);
	m_pBtnAttcRight->OnInit(IDC_BTN_TABBTN_RIGHT, hDlg);
	for (int i = 0; i < 5; i++)
	{
		m_buttons[i]->OnInit(IDC_BTN_TABBTN_1 + i, hDlg);
	}
	return S_OK;
}

INT_PTR CALLBACK
CShTabDlgFrame::DlgProc(
	_In_ CONST HWND   hDlg,
	_In_ CONST UINT   uiMessage,
	_In_ CONST WPARAM wParam,
	_In_ CONST LPARAM lParam
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	WORD			wNotify = HIWORD(wParam);
	WORD			wCtrlID = LOWORD(wParam);
	HWND			hWndCtrl = (HWND)lParam;

	HRESULT hr = S_OK;
	switch (uiMessage)
	{
	case WM_INITDIALOG:
	{
		hr = OnWmInitDialog(hDlg, lParam);
	}
	break;
	case WM_COMMAND:
	{
		hr = OnWmCommand(hDlg, wParam, lParam);
	}
	break;
	case WM_NOTIFY:
	{
		hr = OnWmNotify(hDlg, wParam, lParam);
	}
	break;
	case WM_HSCROLL:
	case WM_VSCROLL:
	{
		//hr = OnWmScroll(hDlg, wParam, lParam);
	}
	break;
	case WM_ENABLE:
		hr = OnWmEnable(hDlg, wParam, lParam);
		break;
	case WM_DRAWITEM:
	{
		hr = OnWmDrawItem(hDlg, wParam, lParam);

	}
	break;
	case WM_CTLCOLORSTATIC:
	{
		//hr = OnWmColorStatic(hDlg, wParam, lParam);
	}
	break;
	case WM_NCDESTROY:
	{
		//hr = OnWmDestroy(hDlg);
	}
	break;
	case WM_HELP:
	{
		CDocumentPropertyPage* pPropertyPageInstance = NULL;

		//if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
		//	(NULL != pPropertyPageInstance))
		//{
		//	LPHELPINFO pHelpInformation = (LPHELPINFO)lParam;
		//	hr = pPropertyPageInstance->ShowHelp(pHelpInformation);
		//}
	}
	break;
	}
	if (S_OK != hr)
	{
		slog.LoggingErrorHResultValues();
	}
	return hr;
}

HRESULT CShTabDlgFrame::OnWmInitDialog(_In_ HWND   hDlg,
	_In_ LPARAM lParam
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;

	PROPSHEETPAGE* pPage = reinterpret_cast<PROPSHEETPAGE*>(lParam);
	if ((NULL != pPage) && (NULL != pPage->lParam))
	{
		CShTabDlgFrame* pTabDlgFrame = reinterpret_cast<CShTabDlgFrame*>(pPage->lParam);
		if (NULL != pTabDlgFrame)
		{
			pTabDlgFrame->m_hWnd = hDlg;
			if (SUCCEEDED(hResult = pTabDlgFrame->StorePropertyPageInstance(hDlg)))
			{
				pTabDlgFrame->CreateCtrlClass();
				hResult = pTabDlgFrame->InitializeUIControls(hDlg);
				pTabDlgFrame->InitTabInfoControl();
			}

			DWORD			dwType = 0;
			DWORD			dwNeeded = 0;
			DWORD			dwVal = 0;
			LRESULT res = GetPrinterDataEx(pTabDlgFrame->m_hPrinter, REG_KEY_HKLM_DEVICEINDEX, REG_VAL_HKLM_DI_DEVICETYPE, &dwType,
				(LPBYTE)&dwVal, sizeof(dwVal), &dwNeeded);
			if (ERROR_SUCCESS == res)
			{
				short devicetype = (short)dwVal;
				slog.putLog("CMainTabPropertyPage::InitializeUIControls -> SUCCESS_GETPRINTERDATAEX(GetPrinterDataEx)\n");
			}
			else
			{
				slog.putLog("CMainTabPropertyPage::InitializeUIControls -> ERROR_GETPRINTERDATAEX(GetPrinterDataEx)\n");
			}
		}
		else
		{
			hResult = E_FAIL;
		}
	}
	if (S_OK != hResult)
	{
		slog.LoggingErrorHResultValues();
	}

	return hResult;
}
//=============================================================================
// function
//        OnWmDrawItem
//
// parameters
//        hWnd        
//        wParam       
//        lParam       
//
// return value
//        ÅLresult
//
// outline
//        onDRAWITEM
//=============================================================================
HRESULT CShTabDlgFrame::OnWmDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HRESULT				res = 0;

	PPROPSTATE			pps;
	CShTabDlgFrame* pTabDlgFrame = NULL;
	short				wCtrlID = (short)wParam;
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;
	HRESULT hResult = S_OK;
	//<S><A>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	//Change the specification for function limitation, display method by changing option
	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hWnd, &pTabDlgFrame)) &&
		(NULL != pTabDlgFrame))

	{
		//<S><A>To Implement Task #3118,22-04-2024,SSDI:Manoj S
		m_pmcf = new CShIniFile(ghInstance, pTabDlgFrame->m_pPrinterName, szCommonDatFilePath, FALSE);
		//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S
		if (pTabDlgFrame->m_ppi->pps == NULL) {
			return 0;
		}

		switch (wCtrlID)
		{
		case IDC_BTN_TABBTN_LEFT:
		case IDC_BTN_TABBTN_1:
		case IDC_BTN_TABBTN_2:
		case IDC_BTN_TABBTN_3:
		case IDC_BTN_TABBTN_4:
		case IDC_BTN_TABBTN_5:
		case IDC_BTN_TABBTN_RIGHT:
			//<S><A>To Implement Task #3118,22-04-2024,SSDI:Manoj S
			//Change the specification for function limitation, display method by changing option
			if (FALSE == ((*m_pmcf).IsLimitByOptionHideUI()))
			{

				if ((pTabDlgFrame->m_ppi->pps->blLimitByOption == bool_true) && (pTabDlgFrame->m_ppi->bllLimitByOptionSafe == FALSE))
				{
					pTabDlgFrame->m_ppi->pps->blLimitByOption = bool_false;

					CShowMessage m_pmsg(pTabDlgFrame->m_hWnd, pTabDlgFrame->m_hStringResourceHandle);
					m_pmsg.Message(pTabDlgFrame->m_hWnd, IDS_MESTITLE_1, IDS_MESSAGE_122);
				}
				else
				{
					pTabDlgFrame->m_ppi->bllLimitByOptionSafe = bool_true;
				}
				return OnDrawItem(hWnd, wParam, lParam);
			}
			else
			{
			//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S
			if ((pTabDlgFrame->m_ppi->pps->blLimitByOption == bool_true) && (pTabDlgFrame->m_ppi->bllLimitByOptionSafe == FALSE))
			{

				pTabDlgFrame->m_ppi->blHideAllControl = TRUE;
				pTabDlgFrame->HideAllCtrls();
				if (pTabDlgFrame->m_blLimitByOption == bool_false) {
					pTabDlgFrame->m_blLimitByOption = bool_true;
					CShowMessage m_pmsg(pTabDlgFrame->m_hWnd, pTabDlgFrame->m_hStringResourceHandle);
					if (m_pmsg.Message(pTabDlgFrame->m_hWnd, IDS_MESTITLE_1, IDS_MESSAGE_92, MB_ICONINFORMATION | MB_YESNO) == IDYES)
					{
						// Factory DefaultsÇ…
						pTabDlgFrame->SetFactoryDefaults(hWnd, pTabDlgFrame->m_pPrinterName);
						pTabDlgFrame->m_ppi->pps->blLimitByOption = bool_false;
						//<S><A>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
						//Patent avoidance message is not displayed
						pTabDlgFrame->m_blFavoritesByOptionCurrentJob = TRUE;
						//<E>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
					}
					else
					{
						//<S><A>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
						//Patent avoidance message is not displayed
						pTabDlgFrame->m_blFavoritesByOptionCurrentJob = FALSE;
						//<E>To Fix Redmine Bug #4276,14.09.2021,SSDI:Chanchal Singla
					}
					//	SetIgnorePropSheet(TRUE);
				}
				res = 1;
			}
			else {
				if (pTabDlgFrame->m_ppi->blHideAllControl == TRUE) {
					::ShowWindow(GetDlgItem(hWnd, IDC_BTN_CLEAR), SW_SHOW);
					pTabDlgFrame->HideAllCtrls();
				}
				else {
					pTabDlgFrame->m_ppi->bllLimitByOptionSafe = TRUE;
					pTabDlgFrame->m_blFavoritesByOptionCurrentJob = TRUE;
					return OnDrawItem(hWnd, wParam, lParam);
				}
			}
			break;
		}
		case IDC_BTN_CLEAR:
		{
			if (pTabDlgFrame->m_ppi->blHideAllControl == TRUE) {

				LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;
				SetBkMode((*pdis).hDC, OPAQUE);
				// Grayout
				FillRect((*pdis).hDC, &(*pdis).rcItem, (HBRUSH)COLOR_WINDOW);
				::ShowWindow(GetDlgItem(hWnd, wCtrlID), SW_SHOW);
				::EnableWindow(GetDlgItem(hWnd, wCtrlID), FALSE);
			}
			else {
				LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;

				SetBkMode((*pdis).hDC, TRANSPARENT);

				::ShowWindow(GetDlgItem(hWnd, wCtrlID), SW_HIDE);
			}
		}
		break;
		default:
			return OnDrawItem(hWnd, wParam, lParam);
		}

	}
//<S><A>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	if (m_pmcf != NULL)
		delete m_pmcf;
//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	return res;
}

//=============================================================================
// function
//        OnWmDrawItem
//
// parameters
//        hWnd         ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//        wParam       ÉÅÉbÉZÅ[ÉWÇÃç≈èâÇÃÉpÉâÉÅÅ[É^
//        lParam       ÉÅÉbÉZÅ[ÉWÇÃ 2 î‘ñ⁄ÇÃÉpÉâÉÅÅ[É^
//
// return value
//        ÉÅÉbÉZÅ[ÉWèàóùÇÃåãâ Ç™ï‘ÇËÇ‹Ç∑ÅB
//        ñﬂÇËílÇÃà”ñ°ÇÕÅAÉÅÉbÉZÅ[ÉWÇ…ÇÊÇ¡ÇƒàŸÇ»ÇËÇ‹Ç∑ÅB
//
// outline
//        WM_DRAWITEM
//=============================================================================
HRESULT CShTabDlgFrame::OnDrawItem(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	HRESULT				res = 0;

	PPROPSTATE			pps;

	short				wCtrlID = (short)wParam;
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;

	//CShTabInfoControl	*pTabInfoCtrl = NULL;
	SHORT				wPushedTargetID = 0;
	BOOL				blPushed = FALSE;
	SHORT				wIndex = 0;
	SHORT				dispCount = 0;



	CShTabDlgFrame* pTabDlgFrame = NULL;
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pTabDlgFrame)) &&
		(NULL != pTabDlgFrame))
	{
		// <S><A> SSDI Bug Fix# 2738 - SSDI:Seetharam - 20210907
		wPushedTargetID = pTabDlgFrame->m_ptabInfoCtrl->GetWkTabIndex() - pTabDlgFrame->m_ptabInfoCtrl->GetTabBtnIndex();
		if (wPushedTargetID > 0 ||
			wPushedTargetID <= pTabDlgFrame->m_ptabInfoCtrl->GetDispCount() - 1)
		{
			if (wCtrlID == (IDC_BTN_TABBTN_1 + wPushedTargetID))
			{
				blPushed = TRUE;
			}
		}
		// <S><E> SSDI Bug Fix# 2738 - SSDI:Seetharam - 20210907
		
		int wCtrlBase = IDC_BTN_TABBTN_1;
		if ((wCtrlID >= wCtrlBase) && (wCtrlID <= (wCtrlBase + 5)))
		{
			if (pTabDlgFrame != NULL)
			{
				(*(pTabDlgFrame->m_buttons)[wCtrlID - IDC_BTN_TABBTN_1]).Draw(lParam, blPushed);
			}
		}
	}

	return res;
}

HRESULT CShTabDlgFrame::OnWmCommand(_In_ HWND   hDlg,
	_In_ CONST WPARAM wParam, _In_ CONST LPARAM lParam
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;

	WORD			wNotify = HIWORD(wParam);
	WORD			wCtrlID = LOWORD(wParam);
	HWND			hWndCtrl = (HWND)lParam;

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		CShTabDlgFrame* pTabDlgFrame = NULL;
		HRESULT hr = S_OK;
		if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pTabDlgFrame)) &&
			(NULL != pTabDlgFrame))
		{
			if (pTabDlgFrame != NULL)
			{
				pTabDlgFrame->OnBnClicked(hDlg, hWndCtrl, wCtrlID);
			}
		}
	}
	break;
	case EN_CHANGE:
	case CBN_SELCHANGE:
	{
		CDocumentPropertyPage* pPropertyPageInstance;

		/*if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)))
		{
			if (NULL != pPropertyPageInstance)
			{
				hResult = pPropertyPageInstance->SendCommand(hDlg, wParam);
				pPropertyPageInstance->UpdateMimic();

			}
		}*/

	}
	break;
	default:
		//
		// Unhandled command so return TRUE
		//
		// hResult = S_OK;
		break;
	}

	//Fixed Coverity issue - 90571 - SSDI:Seetharam - 20201006
	//if (S_OK != hResult)
	//{
	//	slog.LoggingErrorHResultValues();
	//}
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
HRESULT CShTabDlgFrame::OnWmNotify(_In_ HWND   hDlg, _In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	NMHDR* pHdr = reinterpret_cast<NMHDR*>(lParam);
	LPNMHDR			pnmh = (LPNMHDR)lParam;
	long			lCtrlID = (WORD)wParam;

	if ((NULL != pHdr) && (NULL != pHdr->code))
	{
		switch (pHdr->code)
			//switch (((LPNMHDR)lParam)->code)
		{
		case PSN_SETACTIVE:
		{
			hResult = OnPsnSetActive(hDlg);
			if (S_OK != hResult)
			{
				slog.LoggingErrorHResultValues();
			}
		}
		break;
		case PSN_KILLACTIVE:
		{
			/*hResult = OnPsnKillActive(hDlg);
			if (S_OK != hResult)
			{
				slog.LoggingErrorHResultValues();
			}*/
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
			/*hResult = OnPsnQueryCancel(hDlg);
			if (S_OK != hResult)
			{
				slog.LoggingErrorHResultValues();
			}*/
			//
			// Return FALSE to accept the page activation
			//
			//retVal = FALSE;
		}
		break;
		case PSN_APPLY:
		{
			CShTabDlgFrame* pTabDlgFrame = NULL;
			HRESULT hr = S_OK;
			if (SUCCEEDED(hr = RetrievePropertyPageInstance(hDlg, &pTabDlgFrame)) &&
				(NULL != pTabDlgFrame))
			{
				hResult = pTabDlgFrame->OnPsnApply(hDlg, lCtrlID, (LPPSHNOTIFY)lParam);
				if (S_OK != hResult)
				{
					slog.LoggingErrorHResultValues();
				}
			}
			//CDocumentPropertyPage::OnPsnApply(hDlg);
		}
		break;
		case PSN_HELP:
		{
			/*CDocumentPropertyPage* pPropertyPageInstance;
			if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pPropertyPageInstance)) &&
				(NULL != pPropertyPageInstance))
			{
				pPropertyPageInstance->DisplayHelpInformation(hDlg);
			}*/
			//Fixed Coverity issue - 90511 - SSDI:Seetharam - 20201006
			//if (S_OK != hResult)
			//{
			//	slog.LoggingErrorHResultValues();
			//}

		}
		break;

		case NM_CLICK:
		{
			/*hResult = OnNmClick(hDlg);
			if (S_OK != hResult)
			{
				slog.LoggingErrorHResultValues();
			}*/
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

HRESULT CShTabDlgFrame::OnPsnSetActive(_In_ HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hResult = S_OK;
	CShTabDlgFrame* pTabFrame;
	if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pTabFrame)) &&
		(NULL != pTabFrame))
	{
		hResult = pTabFrame->ActivateUIControl(hDlg);
		pTabFrame->SetTabState();
		if (S_OK != hResult)
		{
			slog.LoggingErrorHResultValues();
		}
	}

	return hResult;
}

LRESULT CShTabDlgFrame::OnWmEnable(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	HRESULT hResult = S_OK;
	if ((BOOL)wParam == TRUE) {         // óLå¯âªÇ≥ÇÍÇΩ
		MFPPrinterUI_Logger slog(__FUNCTION__"\n");
		
		CShTabDlgFrame* pTabFrame;
		if (SUCCEEDED(hResult = RetrievePropertyPageInstance(hDlg, &pTabFrame)) &&
			(NULL != pTabFrame))
		{
			pTabFrame->SetTabState();
			if (S_OK != hResult)
			{
				slog.LoggingErrorHResultValues();
			}
		}
	}

	//Coverity Fix: 91470 - SSDI:Seetharam - 20210518
	return hResult;
//	return WndProc(hWnd, WM_ENABLE, wParam, lParam);s
}

HRESULT CShTabDlgFrame::ActivateUIControl(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	m_pBtnAttcLeft->ActivateUIControl(m_pBtnAttcLeft->GetControlID(), hDlg);
	m_pBtnAttcRight->ActivateUIControl(m_pBtnAttcRight->GetControlID(), hDlg);
	int dispCount = m_ptabInfoCtrl->GetDispCount();
	for (int i = 0; i < dispCount; i++)
	{
		m_buttons[i]->ActivateUIControl(m_buttons[i]->GetControlID(), hDlg);
	}
	return hr;
}

//=============================================================================
// function
//      SetCtrlStrings
//
// parameters
//      ñ≥Çµ
//
// return value
//      ê¨å˜Å@: TRUE
//      é∏îsÅ@: FALSE
//
// outline
//      É^Éuè„ÇÃÉâÉxÉãÉRÉìÉgÉçÅ[ÉãÇ…ï∂éöóÒÇÉZÉbÉgÇ∑ÇÈ
//=============================================================================
BOOL CShTabDlgFrame::SetCtrlStrings(void)
{
	BOOL			blRet = FALSE;

	//if(PARENT_CLASSNAME::SetCtrlStrings() == FALSE)
	//	goto EXIT;

	// --- "<"
	//SetCtrlText(IDC_BTN_TABBTN_LEFT);

	// --- ">"
	//SetCtrlText(IDC_BTN_TABBTN_RIGHT);

	blRet = TRUE;

//EXIT:

	return blRet;
}


//=============================================================================
// function
//      DisplayTab
//
// parameters
//      hTarget             ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//      wCtrlID             ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//
// return value
//      0
//
// outline
//      BN_CLICKED ÉÅÉbÉZÅ[ÉWèàóù
//=============================================================================
void CShTabDlgFrame::DisplayTab(HWND hWnd, HWND hTarget, WORD wCtrlID)
{
	if (hTarget == NULL)
		return;

	if(m_hPrevWnd != NULL)
		::ShowWindow(m_hPrevWnd, SW_HIDE);
	::ShowWindow(hTarget, SW_SHOW);

	if (m_hPrevWnd != NULL)
		::EnableWindow(m_hPrevWnd, FALSE);
	::EnableWindow(hTarget, TRUE);

	::InvalidateRect(hWnd, NULL, FALSE);
	::UpdateWindow(hWnd);

	m_hPrevWnd = hTarget;
}


//=============================================================================
// function
//      OnUpDown
//
// parameters
//      hWnd                ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//      hWndCtrl            ëŒè€ÉRÉìÉgÉçÅ[ÉãÇÃÉnÉìÉhÉã
//      wCtrlID             ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//
// return value
//      0
//
// outline
//      BN_CLICKED ÉÅÉbÉZÅ[ÉWèàóù
//=============================================================================
LRESULT CShTabDlgFrame::OnUpDown(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT	res = 0;
	CShTabInfoControl	*pTabInfoCtrl = NULL;
	TABINFOCTRLTBL		*pTabInfoTbl = NULL;
	SHORT				wIndex;
	SHORT				wTabCount;
	SHORT				dispCount = 0;
	SHORT				i = 0;
	SHORT				tmpIndex = 0;
	SHORT				currentIndex = 0;
	SHORT				currentPage = 0;

	//puic = (PUIIFCLS)GetInfoAddr(hWnd, PROP_UIIFCLS_STR);
	//if(puic == NULL)
	//	goto EXIT;

//	pTabInfoCtrl = (*puic).ptabInfoCtrl;
//	if(pTabInfoCtrl == NULL)
//		goto EXIT;
//
//	tmpIndex = (*pTabInfoCtrl).GetTabBtnIndex();
//	wIndex =(*pTabInfoCtrl).IncTabBtnIndex(wCtrlID);
// 	wTabCount = (*pTabInfoCtrl).GetTabCount();
//	dispCount = (*pTabInfoCtrl).GetDispCount();
//
//	for (i = 0; i < dispCount; i++, wIndex++)
//	{
//		if(wIndex >= wTabCount) {
//			//SetCtrlState((*m_buttons[i]).GetResourceID(), SCUI_STATE_OFF);
//		}
//		else
//		{
//			//SetCtrlState((*m_buttons[i]).GetResourceID(), SCUI_STATE_ON);
//			pTabInfoTbl = (*pTabInfoCtrl).getTabInfoCtrlTbl(wIndex);
//			(*m_buttons[i]).init((*pTabInfoTbl).wStrID, (*pTabInfoTbl).wBmpID);
//		}
//#if 0
//		if(pTabInfoCtrl != NULL)
//		{
//			pTabInfoTbl = (*pTabInfoCtrl).getTabInfoCtrlTbl(wIndex);
//			(*m_buttons[i]).init((*pTabInfoTbl).wStrID, (*pTabInfoTbl).wBmpID);
// 		}
//#endif
//	}
//
//	currentPage = (*pTabInfoCtrl).GetCurrentPage();
//	if (m_currentPage == currentPage)
//	{
//		// ëIëÉ^Éuï\é¶íÜ
//		// É{É^ÉìÇÃÉ^ÉuÉXÉgÉbÉvÇè¡Ç∑ÅB
//		(*m_buttons[(*pTabInfoCtrl).GetCurrentPosition()]).UnSetTabStop();
//		// ëÊ1É{É^ÉìÇ…É^ÉuÉXÉgÉbÉvÇê›íËÇ∑ÇÈÅB
//		(*m_buttons[0]).SetTabStop();
//		SetFocus((*m_buttons[0]).GetHWnd());
//	}
//	else
//	{
//		// ëIëÉ^ÉuîÒï\é¶íÜ
//		// Ç±Ç±Ç≈ìÒéËÇ…ï™Ç©ÇÍÇÈÅB
//		// ëIëÉ^ÉuÇ…ñﬂÇÈÇÃÇ©ÅB
//		// ÇªÇÍÇ∆Ç‡ó£ÇÍÇÈÇÃÇ©ÅB
//		// ñﬂÇÈ
//		if ((wCtrlID == IDC_BTN_TABBTN_LEFT && currentPage < m_currentPage) ||
//			(wCtrlID == IDC_BTN_TABBTN_RIGHT && m_currentPage < currentPage))
//		{
//			// ëIëÉ^ÉuÇ…ñﬂÇÍÇÈÇ∆Ç´
//			if ((wCtrlID == IDC_BTN_TABBTN_LEFT && currentPage + 1 == m_currentPage) ||
//				(wCtrlID == IDC_BTN_TABBTN_RIGHT && m_currentPage + 1 == currentPage))
//			{
//				// ëÊ1É{É^ÉìÇÃÉ^ÉuÉXÉgÉbÉvÇè¡Ç∑ÅB
//				(*m_buttons[0]).UnSetTabStop();
//				// ëIëÉ{É^ÉìÇ…É^ÉuÉXÉgÉbÉvÇê›íËÇ∑ÇÈÅB
//				(*m_buttons[(*pTabInfoCtrl).GetCurrentPosition()]).SetTabStop();
//				SetFocus((*m_buttons[(*pTabInfoCtrl).GetCurrentPosition()]).GetHWnd());
//			}
//			else // ÇªÇÃï˚å¸Ç…ÇÕå¸Ç©Ç¡ÇƒÇ¢ÇÈÇ™ëIëÉ{É^ÉìÉyÅ[ÉWÇ…ÇÕñﬂÇÍÇ»Ç¢Ç∆Ç´
//			{
//				SetFocus((*m_buttons[0]).GetHWnd());
//			}
// 		}
//		else // ó£ÇÍÇÈÇ∆Ç´
//		{
//			SetFocus((*m_buttons[0]).GetHWnd());
// 		}
//	}
//
//	if (wCtrlID == IDC_BTN_TABBTN_LEFT)
//	{
//		m_currentPage--;
// 	}
//	else
//	{
//		m_currentPage++;
//	}
// 
//// <S><C> 2015.06.29 SSL Watanabe
//	// ::InvalidateRect(hWnd, NULL, FALSE);
//	::InvalidateRect((*m_pBtnAttcLeft).GetHWnd(), NULL, FALSE);
//	::InvalidateRect((*m_pBtnAttcRight).GetHWnd(), NULL, FALSE);
//	for (i = 0; i < dispCount; i++) {
//		::InvalidateRect((*m_buttons[i]).GetHWnd(), NULL, FALSE);
//	}
//// <E> 2015.06.29 SSL Watanabe
////		::InvalidateRect(hWnd, NULL, TRUE);
//// <S><C> 2015.06.29 SSL Watanabe
//	// ::UpdateWindow(hWnd.GetHWnd());
//	::UpdateWindow((*m_pBtnAttcLeft).GetHWnd());
//	::UpdateWindow((*m_pBtnAttcRight).GetHWnd());
//	for (i = 0; i < dispCount; i++) {
//		::UpdateWindow((*m_buttons[i]).GetHWnd());
//	}
// <E> 2015.06.29 SSL Watanabe

	res = 1;
//EXIT:

	return res;
}

//=============================================================================
// function
//      OnChangeDetailSetting
//
// parameters
//      hWnd                ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//      hWndCtrl            ëŒè€ÉRÉìÉgÉçÅ[ÉãÇÃÉnÉìÉhÉã
//      wCtrlID             ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//
// return value
//      0
//
// outline
//      BN_CLICKED ÉÅÉbÉZÅ[ÉWèàóù
//=============================================================================
LRESULT CShTabDlgFrame::OnChangeDetailSetting(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	PPROPSTATE		pps;

	HWND			hTarget = NULL;
	HWND			hWndLayoutTab = NULL;

	CShTabInfoControl	*pTabInfoCtrl = NULL;
	TABINFOCTRLTBL		*pTabInfoTbl = NULL;
	SHORT				wIndex;
	SHORT				wIndexNow;
	CDocumentPropertyPage			*pTabDlg = NULL;
	CDocumentPropertyPage			*pTabDlgNext = NULL;

	//	// WM_NOTIFY - PSN_KILLACTIVEÇ
	wIndexNow = m_ptabInfoCtrl->GetWkTabIndex();				// çTabÇIndex
	pTabInfoTbl = m_ptabInfoCtrl->getTabInfoCtrlTbl(wIndexNow);	// çTabTableè
	pTabDlg = (CDocumentPropertyPage *)(*pTabInfoTbl).pTabDlg;
	//SHORT				wPsnKillActiveEX;
	SHORT				wPsnKillActiveEX = FALSE;	//Coverity - 91177 & 91170
	if (pTabDlg != NULL)
	{

		switch (wIndexNow)
		{
		case 0:		//IDD_TAB_MAIN_121_03:
			wPsnKillActiveEX = ((CMainTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		case 1:		//IDD_TAB_PAPER_03:
			wPsnKillActiveEX = ((CPaperTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		case 2:		//IDD_TAB_FINISHING_01_03:
			wPsnKillActiveEX = ((CFinishingTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		case 3:		//IDD_TAB_LAYOUT_01_03:
			wPsnKillActiveEX = ((CLayoutTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		case 4:		//IDD_TAB_JOBHAND_0:
			wPsnKillActiveEX = ((CJobHandlingTabPropertyPage *)pTabDlg)->OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		default:
			wPsnKillActiveEX = (*pTabDlg).OnPsnKillActiveEX(pTabInfoTbl->hWndTabDlg);
			break;
		}
	}
	if (wPsnKillActiveEX == FALSE) {
		goto EXIT;
	}
	//<S><A> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
	if ((*pTabDlg).OnShowHideTab(hWnd, FALSE) == FALSE) {
		// É^Éuà⁄ìÆÇµÇ»Ç¢
		goto EXIT;
	}
	//<S><E> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam

	wIndex = m_ptabInfoCtrl->GetTabBtnIndex();                   // Index
	wIndex = wIndex + (wCtrlID - IDC_BTN_TABBTN_1);             // TabÇIndex
	pTabInfoTbl = m_ptabInfoCtrl->getTabInfoCtrlTbl(wIndex);    // TabTableè
	m_ptabInfoCtrl->SetWkTabIndex(wIndex);
	hTarget = (*pTabInfoTbl).hWndTabDlg;
	//
	if (hTarget == NULL) {
		pTabDlgNext = (*pTabInfoTbl).pTabDlg;
		(*pTabInfoTbl).hWndTabDlg = ::CreateDialogParam(m_hDialogResourceHandle, MAKEINTRESOURCE((*pTabDlgNext).GetResourceID()), m_hWnd, (DLGPROC)(*pTabDlgNext).DlgProc, (LPARAM)pTabDlgNext);
		if ((*pTabInfoTbl).hWndTabDlg != NULL) {
			hTarget = (*pTabInfoTbl).hWndTabDlg;
			::EnableWindow(hTarget, FALSE);
			::ShowWindow(hTarget, SW_HIDE);
		}
	}

	//
	if (m_hPrevWnd == hTarget)
		goto EXIT;
	//
	//	// WM_NOTIFY - PSN_SETACTIVEÇActivateUIControl
	m_nCurrTabId = wCtrlID - IDC_BTN_TABBTN_1;
	HRESULT hResult = S_OK;
	CDocumentPropertyPage* pPropertyPageInstance = m_ptabInfoCtrl->getTabCtrl(m_ptabInfoCtrl->GetCurrentPosition());
	pTabInfoTbl = (*m_ptabInfoCtrl).getTabInfoCtrlTbl(m_ptabInfoCtrl->GetCurrentPosition());
	int tabResourceID = pPropertyPageInstance->GetResourceID();
	//switch (pPropertyPageInstance->GetResourceID())
	switch (m_nCurrTabId)
	{
	case 0:		//IDD_TAB_MAIN_121_03:
		hResult = ((CMainTabPropertyPage *)pPropertyPageInstance)->ActivateUIControl(pTabInfoTbl->hWndTabDlg);
		break;
	case 1:		//IDD_TAB_PAPER_03:
		hResult = ((CPaperTabPropertyPage *)pPropertyPageInstance)->ActivateUIControl(pTabInfoTbl->hWndTabDlg);
		break;
	case 2:		//IDD_TAB_FINISHING_01_03:
		hResult = ((CFinishingTabPropertyPage *)pPropertyPageInstance)->ActivateUIControl(pTabInfoTbl->hWndTabDlg);
		break;
	case 3:		//IDD_TAB_LAYOUT_01_03:
		hResult = ((CLayoutTabPropertyPage *)pPropertyPageInstance)->ActivateUIControl(pTabInfoTbl->hWndTabDlg);
		break;
	case 4:		//IDD_TAB_JOBHAND_0:
		hResult = ((CJobHandlingTabPropertyPage *)pPropertyPageInstance)->ActivateUIControl(pTabInfoTbl->hWndTabDlg);
		break;
	default:
		break;
	}

	//	// LayoutTabÇ©ÇÁà⁄ìÆÇ∑ÇÈèÍçáÇÃèàóù
	//	pTabInfoTbl = (*pTabInfoCtrl).getTabInfoCtrlTbl(ENUM_TABPROC_LAYOUT);    // TabTableèÓïÒÇéÊìæÇ∑ÇÈ
	//	hWndLayoutTab = (*pTabInfoTbl).hWndTabDlg;
	//	if( (*puic).HwndPrev == hWndLayoutTab ){
	//		// åªç›Ç™LayoutTabÇÃèÍçá
	//		pTabDlg = (CShTabDlgFrame *)(*pTabInfoTbl).pTabDlg;
	//		(*pTabDlg).TabEndprocess();
	//	}
	//
	SetOneTabStop(hWnd);

	DisplayTab(hWnd, hTarget, (*pTabInfoTbl).wTabNo);
	//<S><A> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
	if ((*(*pTabInfoTbl).pTabDlg).OnShowHideTab(hWnd, TRUE) == FALSE) {
		// É^Éuà⁄ìÆÇµÇ»Ç¢
		goto EXIT;
	}
	//<S><E> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
	res = 1;

EXIT:

	return res;

}


//=============================================================================
// function
//      TabEndprocess
//
// parameters
//      hTarget             ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//      wCtrlID             ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//
// return value
//      0
//
// outline
//      É^ÉuêÿÇËë÷Ç¶éûÇÃèIóπèàóù
//=============================================================================
void CShTabDlgFrame::TabEndprocess(VOID)
{
	return;
}


//=============================================================================
// function
//      OnPsnSetActiveEX
//
// parameters
//      hTarget             ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//      wCtrlID             ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//
// return value
//      0
//
// outline
//      É^ÉuêÿÇËë÷Ç¶éûÇÃèIóπèàóù
//=============================================================================
LRESULT CShTabDlgFrame::OnPsnSetActiveEX(HWND hWnd)
{
	return TRUE;
}


//=============================================================================
// function
//      OnPsnKilltActiveEX
//
// parameters
//      hTarget             ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//      wCtrlID             ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//
// return value
//      0
//
// outline
//      É^ÉuêÿÇËë÷Ç¶éûÇÃèIóπèàóù
//=============================================================================
LRESULT CShTabDlgFrame::OnPsnKillActiveEX(HWND hWnd)
{
	return TRUE;
}


//<S><A> 2015.03.05 Watanabe Keiichi
//=============================================================================
// function
//      OnWmClose
//
// parameters
//      hWnd                ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//      wParam              ÉÅÉbÉZÅ[ÉWÇÃç≈èâÇÃÉpÉâÉÅÅ[É^
//      lParam              ÉÅÉbÉZÅ[ÉWÇÃ 2 î‘ñ⁄ÇÃÉpÉâÉÅÅ[É^
//
// return value
//      0
//
// outline
//      WM_CLOSE ÉÅÉbÉZÅ[ÉWèàóù
//=============================================================================
LRESULT CShTabDlgFrame::OnWmClose(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HWND hParent = ::GetAncestor(hWnd, GA_ROOT);

	::DestroyWindow(hParent);
	PostQuitMessage(0);

	return 0;
}
//<E>

//<S><A> 2015.03.07 SSL Watanabe
//=============================================================================
// function
//      OnWmNotify
//
// parameters
//      hWnd                ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//      wParam              ÉÅÉbÉZÅ[ÉWÇÃç≈èâÇÃÉpÉâÉÅÅ[É^
//      lParam              ÉÅÉbÉZÅ[ÉWÇÃ 2 î‘ñ⁄ÇÃÉpÉâÉÅÅ[É^
//
// return value
//      0
//
// outline
//      WM_NOTIFY ÉÅÉbÉZÅ[ÉWèàóù
//=============================================================================
//LRESULT CShTabDlgFrame::OnWmNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
//{
//	LRESULT			res = 0;
//
//	long			lCtrlID = (WORD)wParam;
//	LPNMHDR			pnmh = (LPNMHDR)lParam;
//
//	//res = CShTabDlg::OnWmNotify(hWnd, wParam, lParam);
//
//	return res;
//}
//<E>
//=============================================================================
// function
//      SetOneTabStop
//
// parameters
//      wCtrlID             É{É^ÉìÇÃÉRÉìÉgÉçÅ[ÉãID
// return value
//      TRUE
//
// outline
//      É{É^ÉìåQÇÃíÜÇ©ÇÁ1Ç¬ÇæÇØÉ^ÉuÉXÉgÉbÉvÇê›íËÇµÇ‹Ç∑ÅB
//=============================================================================
LRESULT CShTabDlgFrame::SetOneTabStop(HWND hWnd)
{
	//PUIIFCLS		puic = NULL;
//	CShTabInfoControl	*pTabInfoCtrl = NULL;
	SHORT dispCount = 0;
	SHORT index = 0;
	SHORT i = 0;

	//Fixed Coverity 90429,20200212,SSDI:Chanchal Singla
	//if(pTabInfoCtrl == NULL)
	if (m_ptabInfoCtrl == NULL)
		goto EXIT;

	//Fixed Coverity 90429,20200212,SSDI:Chanchal Singla
	//dispCount = (*pTabInfoCtrl).GetDispCount();
	dispCount = (*m_ptabInfoCtrl).GetDispCount();

	// (*m_buttons[(*puic).wPrevID % dispCount]).UnSetTabStop();
	for (i = 0; i < dispCount; i++)
	{
		(*m_buttons[i]).UnSetTabStop();
	}
	//Fixed Coverity 90429,20200212,SSDI:Chanchal Singla
	//(*m_buttons[(*pTabInfoCtrl).GetWkTabIndex() % dispCount]).SetTabStop();
	(*m_buttons[(*m_ptabInfoCtrl).GetWkTabIndex() % dispCount]).SetTabStop();
	
EXIT:
	return TRUE;
}

HRESULT CShTabDlgFrame::StorePropertyPageInstance(_In_ CONST HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;

	if (!SetProp(hDlg, MAKEINTATOM(ID_XDSMPL_DLG_ATOM), reinterpret_cast<HANDLE>(this)))
	{
		slog.putLog("CShTabDlgFrame::StorePropertyPageInstance -> E_FAIL\n");
		hr = E_FAIL;
	}
	return hr;
}

HRESULT
CShTabDlgFrame::RetrievePropertyPageInstance(
	_In_        CONST HWND     hDlg,
	_Outptr_ CShTabDlgFrame** pDocPropPage
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	if ((NULL != hDlg) && (NULL != pDocPropPage))
	{
		*pDocPropPage = reinterpret_cast<CShTabDlgFrame*>(GetProp(hDlg, MAKEINTATOM(ID_XDSMPL_DLG_ATOM)));
	}
	else
	{
		slog.putLog("CShTabDlgFrame::RetrievePropertyPageInstance -> NULL_HANDLE\n");
	}
	return hr;
}
HRESULT CShTabDlgFrame::RemovePropertyPageInstance(
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
		slog.putLog("CShTabDlgFrame::RemovePropertyPageInstance -> NULL_HANDLE(hDlg)\n");
	}

	return hr;
}
//=============================================================================
// function
//      HideAllCtrls
//
// parameters
//      short *pwExcept
//
// return value
//      êTRUE
//      FALSE
//
// outline
//      HideAllCtrls
//=============================================================================
BOOL CShTabDlgFrame::HideAllCtrls(short *pwExcept)
{
	if (m_ppi == NULL) {
		goto EXIT;
	}

	if ((*m_ppi).blHideAllControl == TRUE) {
		::EnumChildWindows(m_hWnd, (WNDENUMPROC)CShTabDlg_HideProc, (LPARAM)pwExcept);
	}

EXIT:
	return TRUE;
}
BOOL CALLBACK CShTabDlg_HideProc(HWND hWnd, LPARAM lParam)
{
	short	wCtrlId = 0;
	short	*pwExcept = NULL;

	if (hWnd == NULL)
		return FALSE;

	pwExcept = (short*)lParam;
	wCtrlId = GetDlgCtrlID(hWnd);
	if (pwExcept != NULL)
	{
		while (*pwExcept != -1)
		{
			if (*pwExcept == wCtrlId)
				goto EXIT;

			pwExcept++;
		}
	}

	if (IDC_BTN_CLEAR != wCtrlId) {
		::ShowWindow(hWnd, SW_HIDE);
	}

EXIT:

	return TRUE;
}
//=============================================================================
// function
//      DisableAllCtrls
//
// parameters
//      short *pwExcept
//
// return value
//      ê TRUE
//      FALSE
//
// outline
//     DisableAllCtrls
//=============================================================================
BOOL CShTabDlgFrame::DisableAllCtrls(short *pwExcept)
{
	::EnumChildWindows(m_hWnd, (WNDENUMPROC)CShTabDlg_DisableProc, (LPARAM)pwExcept);

	return TRUE;
}

BOOL CALLBACK CShTabDlg_DisableProc(HWND hWnd, LPARAM lParam)
{
	short	wCtrlId = 0;
	short	*pwExcept = NULL;

	if (hWnd == NULL)
		return FALSE;

	pwExcept = (short*)lParam;
	if (pwExcept != NULL)
	{
		wCtrlId = GetDlgCtrlID(hWnd);
		while (*pwExcept != -1)
		{
			if (*pwExcept == wCtrlId)
				goto EXIT;

			pwExcept++;
		}
	}

	if (::IsWindowEnabled(hWnd) == TRUE)
		::EnableWindow(hWnd, FALSE);

EXIT:

	return TRUE;
}
//=============================================================================
// function
//      SetFactoryDefaults
//
// parameters
//      ñ≥Çµ
//
// return value
//      TRUE
//      FALSE
//
// outline
//      SetFactoryDefaults
//=============================================================================
BOOL CShTabDlgFrame::SetFactoryDefaults(HWND hDlg, WCHAR* pPrinterName)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL			blRet = FALSE;
	long			lRet = false;
	short			wIndex = 0;
	CShRegUS		*preg = NULL;
	PREGJCINFO		pregInfo = NULL;
	BOOL bIsUIUpdated = FALSE;

	CShJsonUS		*pjsonus = NULL;
	//vector<Json::Value> vecValue;
	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	//========================================================================
	//This Flag variable is reserved , please don't used this flag for other purpose and 
	// don't change the value,otherwise driver maybe crashed
	BOOL wFlag = FALSE;
	//========================================================================
	CShRegJC		*pregJC = NULL;
	CRegistryAPI cRegApi;
	POEMDEV pPrivatetDevMode = NULL;
	PFEATUREINFOLIST pFeatureInfoList = NULL;
	PROPSTATE wPropertySheetState_Pre;
	SecureZeroMemory(&wPropertySheetState_Pre, sizeof(wPropertySheetState_Pre));
	memcpy_s(&wPropertySheetState_Pre, sizeof(PROPSTATE), m_ppi->pps, sizeof(PROPSTATE));
	//// Previous public Devmode
	DEVMODE wOemPublicDevMode_Pre;
	SecureZeroMemory(&wOemPublicDevMode_Pre, sizeof(wOemPublicDevMode_Pre));
	memcpy_s(&wOemPublicDevMode_Pre, sizeof(DEVMODE), m_pOemPublicDevMode, sizeof(DEVMODE));

	//// Previous Private Devmode
	OEMDEV wOemPrivateDevMode_Pre = {};
	SecureZeroMemory(&wOemPrivateDevMode_Pre, sizeof(wOemPrivateDevMode_Pre));
	memcpy_s(&wOemPrivateDevMode_Pre, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));
	PROPSTATE pps;
	SecureZeroMemory(&pps, sizeof(PROPSTATE));
	memcpy(&pps, m_ppi->pps, sizeof(PROPSTATE));
	CShCmbUserset *pCmbUserset = NULL;
	pCmbUserset = new CShCmbUserset(
		m_pFeatureInfoList,
		m_pPrintCoreHelperUni,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_ObjCShowMessage,
		m_pOemPrivateDevMode,
		m_pOemPublicDevMode,
		m_pPrinterName,
		m_pUsIndex,
		m_ppi,
		IDC_CMB_SHARED_USRSET,
		pps
	);
	if (pCmbUserset == NULL)
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
	}

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
		//Fixed Coverity #91472,13-08-2021,SSDI:Chanchal Singla
		/*else
		{
			slog.putLog("CDocumentPropertyPage::OnUsSelect -> NULL_OBJECT(pCmbUserset)\n");
		}*/
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

	cRegApi.ReadData(m_pPrinterName, &m_pOemPrivateDevMode->scpi.dev);
	PrivateDevmodeDataToPropState(m_hStringResourceHandle, m_pPrinterName, m_pOemPrivateDevMode, &pps);
	//<E>to Fix SSDI BUG #3631,2021-04-22,SSDI:Chanchal Singla	

	{
		//If Favoutrites is Set To factory Defaults
		pregInfo = new REGJCINFO;
		if (pregInfo == NULL)
			goto EXIT;

		pregJC = new CShRegJC(m_hStringResourceHandle, pPrinterName);
		if (pregJC == NULL)
			goto EXIT;
		pps.wUsePIN = bool_false;
		pps.wUseAcNum = bool_false;
		pps.wUseJName = bool_false;
		pps.wLoginPass = bool_false;

		if (pps.wPPlcyWinLogin == bool_false)
		{
			pps.wLoginName = bool_false;
		}

		if (pps.wPPlcyLoginName == bool_false)
		{
			pps.wUseUName = bool_false;
			SecureZeroMemory(pps.szUserName, sizeof(pps.szUserName));
		}

		if (pps.wLoginName == 0)
		{
			SecureZeroMemory(pps.szLoginNameUA, sizeof(pps.szLoginNameUA));
		}

		SecureZeroMemory(pps.szPIN, sizeof(pps.szPIN));
		SecureZeroMemory(pps.szRetentionPassword, sizeof(pps.szRetentionPassword));

		SecureZeroMemory(pps.szUserNumber, sizeof(pps.szUserNumber));
		SecureZeroMemory(pps.szJobName, sizeof(pps.szJobName));
		SecureZeroMemory(pps.szLoginPasswordUA, sizeof(pps.szLoginPasswordUA));
		//Coverity Fix: 91328 - SSDI Seetharam - 20210503
		//SecureZeroMemory(pregInfo, sizeof(pregInfo));
	//	SecureZeroMemory(pregInfo, sizeof(PREGJCINFO));	//Fixed Coverity #91477,13-08-2021,SSDI:Chanchal Singla
		SecureZeroMemory(pregInfo, sizeof(REGJCINFO));
		if (PropStateToRegJCInfo(&pps, pregInfo, pPrinterName) != FALSE)
		{
			(*pregJC).WriteJCData(pPrinterName, pregInfo, JC_ALL, pps.wPPlcyWinLogin);
		}
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
			pregUps = new CUPSReg(m_hStringResourceHandle, m_pPrinterName);
			if (pregUps == NULL)
			{
				return FALSE;
			}
			(*pregUps).resetEx(m_pPrinterName, DMPAPER_CUSTOM);//bear
			(*pregUps).resetEx(m_pPrinterName, DMPAPER_CUSTOM_ZOOM);

			if (pregUps != NULL)
			{
				delete pregUps;
				pregUps = NULL;
			}
		}
		//<S><A>To Fix #3506,2021-04-29,SSDI:Chanchal Singla
		//Mimic displays in Color even though Black and White is set in Configuration tab
		//update color feature based on constraints
		{
			UINT	SelectedOptionindex = 0;
			PCSTR SellectedOptionName = NULL;
			if (1 == m_ppi->pps->wColorOption) // Black and White
			{
				SelectedOptionindex = eThirdOption;
				SellectedOptionName = "Mono";
			}
			else if (0 == m_ppi->pps->wColorOption) // Color
			{
				if (m_ppi->pps->byAutoColorMode == 1) {
					SelectedOptionindex = eSecondOption;
					SellectedOptionName = "Color";
				}
				else
				{
					if (m_DefaultSettings[FEATURE_COLOR_MODE].iDevmodeValue == colmode_mono)
					{
						SelectedOptionindex = eThirdOption;
						SellectedOptionName = "Mono";
					}
					else if (m_DefaultSettings[FEATURE_COLOR_MODE].iDevmodeValue == colmode_color)
					{
						SelectedOptionindex = eSecondOption;
						SellectedOptionName = "Color";
					}
					else
					{
						SelectedOptionindex = eFirstOption;
						SellectedOptionName = "Auto";
					}
				}
			}
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = SellectedOptionName;
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = SelectedOptionindex;
		}
		//<E>To Fix #3506,2021-04-29,SSDI:Chanchal Singla
	}
	*m_pUsIndex = wIndex;
	memcpy(m_ppi->pps, &pps, sizeof(PROPSTATE));
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
	if (pregJC != NULL)
	{
		delete pregJC;
		pregJC = NULL;
	}
	if (pregInfo != NULL)
	{
		delete pregInfo;
		pregInfo = NULL;
	}
	if (pCmbUserset != NULL)
	{
		delete pCmbUserset;
		pCmbUserset = NULL;
	}

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
	//ths condition is kept because driver Crashing in case of FeatureInfoList is NULL 
	if (pFeatureInfoList == NULL)
	{
		wFlag = TRUE;
	}
	if (wFlag == FALSE)
	{
		//Fixed Coverity #91461,13-08-2021,SSDI:Chanchal Singla
		if (pFeatureInfoList != NULL)
		{
			delete pFeatureInfoList;
			pFeatureInfoList = NULL;

		}
	}
	return blRet;
}
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
VOID CShTabDlgFrame::PrivateDevmodeDataToPropState(HMODULE	hStringResourceHandle, WCHAR* pPrinterName, POEMDEV pOemPrivateDevMode, PPROPSTATE PropertySheetState)
{
	// Checking the condition for stringresource handle and printer name
	if (pPrinterName == NULL || hStringResourceHandle == NULL)
	{
		return;
	}

	// -- Staple
	PropertySheetState->wStaple = pOemPrivateDevMode->scpi.ext.byStaple;

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
	// ================================================
	// _/_/_/  job control data
	// ================================================
	SetRegJCInfoToPropState(hStringResourceHandle, &pOemPrivateDevMode->scpi, PropertySheetState, m_pPrinterName);


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

	//PUNCH
	PropertySheetState->wPunch = pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_PUNCH ? bool_true : bool_false;
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
BOOL CShTabDlgFrame::SetRegJCInfoToPropState(HMODULE	hStringResourceHandle, PSCDM pscdm, PPROPSTATE pps, WCHAR FAR *pszSvrPrnName)
{
	BOOL			blRet = FALSE;

	PREGJCINFO		pregjcinfo = NULL;

	DWORD			dwSize = 0;
	CShRegJC *pregjc = new CShRegJC(hStringResourceHandle, pszSvrPrnName);
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
BOOL CShTabDlgFrame::RegJCInfoToPropState(PREGJCINFO prji, PPROPSTATE pps, WCHAR FAR *pszSvrPrnName)
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
	(*pps).wDocFileFolder = (*prji).wFolderIndex;


	SecureZeroMemory((*pps).szUserName, sizeof((*pps).szUserName));
	SecureZeroMemory((*pps).szJobName, sizeof((*pps).szJobName));
	SecureZeroMemory((*pps).szUserNumber, sizeof((*pps).szUserNumber));
	SecureZeroMemory((*pps).szLoginNameUA, sizeof((*pps).szLoginNameUA));
	SecureZeroMemory((*pps).szLoginPasswordUA, sizeof((*pps).szLoginPasswordUA));
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
BOOL CShTabDlgFrame::PropStateToRegJCInfo(PPROPSTATE pps, PREGJCINFO prji, WCHAR FAR *pszSvrPrnName)
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
	(*prji).wFolderIndex = (*pps).wDocFileFolder;


	SecureZeroMemory((*prji).szUserName, sizeof((*prji).szUserName));
	SecureZeroMemory((*prji).szJobName, sizeof((*prji).szJobName));
	SecureZeroMemory((*prji).szAccountNum, sizeof((*prji).szAccountNum));
	SecureZeroMemory((*prji).szLoginName, sizeof((*prji).szLoginName));
	SecureZeroMemory((*prji).szLoginPassword, sizeof((*prji).szLoginPassword));
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
	if ((*prji).wUsePIN == bool_true)
		::lstrcpy((*prji).szPIN, (*pps).szRetentionPassword);
	if ((*prji).wFolderIndex != 0)
		::lstrcpy((*prji).szFolderPass, (*pps).szFolderPass);


	blRet = TRUE;

EXIT:

	return blRet;
}