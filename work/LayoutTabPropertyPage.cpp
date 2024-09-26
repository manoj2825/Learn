//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : LayoutTabPropertyPage.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 09 October 2013
//
//	Description   : This class handles Property sheet dialog functions  for Layout tab
//===========================================================================
#include "precomp.h"
#include "define.h"
#include "DevModeUpDate.h"
#include "RegistryAPI.h"
#include "shjsonms.h"
//For Logging Purpose
#include "MFPLogger.h"

//=============================================================================
// function
//      CLayoutTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Constructor to the class CLayoutTabPropertyPage.
//=============================================================================
CLayoutTabPropertyPage::CLayoutTabPropertyPage(HMODULE hStringResourceHandle, HMODULE hBitmapResourceHandle, HMODULE hDialogResourceHandle, IPrintCoreHelperUni* pPrintCoreHelperUni, ResourceIDInfoMap* pResourceIDInfoMap, ConstraintFeatureInfoMap &ConstraintFeatureInfoMap, PFEATUREINFOLIST &pFeatureInfoList, PaperInfoMap PaperInfo, CustomPaperInfoMap CustomPaperInfoList, POEMDEV pOemPrivateDevMode,
	PDEVMODE pOemPublicDevMode, LPTSTR pHelpFilePath, DWORD *pHelpID, std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings, POEMDEV pPrivateDevmode, HANDLE hPrinter,
	WCHAR* pPrinterName, SHORT& sWaterMarkIndex, SHORT& sUsIndex, PPROPINFO ppi) :CDocumentPropertyPage(hStringResourceHandle, pHelpFilePath, pHelpID, hPrinter, pPrinterName, hBitmapResourceHandle, hDialogResourceHandle)

{

	m_pPrintCoreHelperUni = pPrintCoreHelperUni;
	m_pResourceIDInfoMap = pResourceIDInfoMap;
	m_ConstraintFeatureInfoMap = &ConstraintFeatureInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	m_PaperInfo = PaperInfo;
	m_CustomPaperInfoList = CustomPaperInfoList;
	m_pOemPrivateDevMode = pOemPrivateDevMode;
	m_pOemPublicDevMode = pOemPublicDevMode;
	//m_hDialogResourceHandle = hDialogResourceHandle;
	m_DefaultSettings = pDefaultSettings;
	//m_wID				= IDD_TAB_MAIN_121_03;
	m_wID = IDD_TAB_LAYOUT_01_03;
	m_pPrivateDevmode = pPrivateDevmode;
	m_eColorModeValue = eAutoMode;
	m_pWaterMarkIndex = &sWaterMarkIndex;
	m_pUsIndex = &sUsIndex;
	m_ppi = ppi;
	//<S><A> To Add Support of Print Position Dialog, 20-05-2020, SSDI:Chanchal Parkash
	//SecureZeroMemory(&m_PropertySheetState, sizeof(PROPSTATE));
	//<E> To Add Support of Print Position Dialog, 20-05-2020, SSDI:Chanchal Parkash
}

//=============================================================================
// function
//      CLayoutTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Destructor to the class CLayoutTabPropertyPage.
//=============================================================================
CLayoutTabPropertyPage::~CLayoutTabPropertyPage(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (NULL == m_ObjCShowMessage)
	{
		slog.putLog("CFinishingTabPropertyPage::~CFinishingTabPropertyPage -> NULL_OBJECT(m_ObjCShowMessage)\n");
	}
}
/*++

Routine Name:

	CLayoutTabPropertyPage::InitDlgBox

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
CLayoutTabPropertyPage::InitDlgBox(
	_Out_ LPCTSTR* ppszTemplate,
	_Out_ LPCTSTR* ppszTitle
)
{
	HRESULT hr = S_OK;

	//Coverity Fix 90328 - SSDI:Seetharam - 20200908
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
CLayoutTabPropertyPage::InitializeUIControls(
	_In_ CONST HWND   hDlg
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	m_hPropertySheetHandle = hDlg;
	CreateMessageClass(hDlg);
	//<S><A> To Add Support of Print Position Dialog, 20-05-2020, SSDI:Chanchal Parkash
	memcpy(&m_PropertySheetState, m_ppi->pps, sizeof(PROPSTATE));
	UpdatePropState(m_pOemPrivateDevMode, &m_PropertySheetState, m_pPrinterName);
	memcpy( m_ppi->pps, &m_PropertySheetState, sizeof(PROPSTATE));
	//<E> To Add Support of Print Position Dialog, 20-05-2020, SSDI:Chanchal Parkash
	__super::InitializeUIControls(hDlg);
	
	return hr;
}
HRESULT
CLayoutTabPropertyPage::ActivateUIControl(
	_In_ CONST HWND   hDlg
)
{
	HRESULT hr = S_OK;
	//SetPamphletIndex(hDlg);
	//<S><A>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	memcpy(&m_PropertySheetState, m_ppi->pps, sizeof(PROPSTATE));
	//<E>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	__super::ActivateUIControl(hDlg);

	ShowFinishingInformationIcon(hDlg);
	UpdateUserSetting(hDlg, IDC_CMB_SHARED_USRSET);
	//ShowConstraintsOnFactoryDefaults(hDlg, IDC_CMB_SHARED_USRSET);
	
	SetTabState(hDlg);
	return hr;
}


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
void CLayoutTabPropertyPage::CreateContorlClass(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	CUIControl* pControl;
	std::string strFeatureName; // = m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszFeature;
	PRESOURCEIDINFO pResourceIDInfo = NULL; // GetResourceIDInfoOfControl(strFeatureName);

	//N-Up
	pControl = new CShImageCombo(
		GetResourceIDInfoOfControl(PAGESPERSHEET),
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_PAGESPERSHEET],
		m_ObjCShowMessage,
		IDC_CMB_NUP
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_NUP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}


	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_NUP_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_NUP_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}

	//<S><C>To Modified the Support of Booklet Standard Output Size ,2020-09-02,SSDI:Chanchal Singla
		//Output Size
		//<S><A>To fix SSDI Bug #2709,04.09.2020,SSDI:Chanchal Parkash
		//Booklet Output Size value is not displayed according to System Locale
		/*Few OS Specific Language has default language as A4/A3 to justify this initialise Default Setting with
		"A3" as default paper */
		//<S><C> To support maximum paper size, 2020.10.09, SSDI: Chanchal Singla
		//Maxpapersize
	if (m_PropertySheetState.byMaxPaperSize == eFirstOption)
	{
		m_DefaultSettings[FEATURE_BOOKLET_OUTPUTSIZE].ResourceID = (DMPAPER_A4 == GetPaperSizeDef()) ? IDS_PAPERSIZE_8 : IDS_PAPERSIZE_3;
	}
	else if (m_PropertySheetState.byMaxPaperSize == eSecondOption)
	{
		m_DefaultSettings[FEATURE_BOOKLET_OUTPUTSIZE].ResourceID = (DMPAPER_A4 == GetPaperSizeDef()) ? IDS_PAPERSIZE_9 : IDS_PAPERSIZE_1;
	}
	//<E>To fix SSDI Bug #2709,04.09.2020,SSDI:Chanchal Parkash
	pResourceIDInfo = GetResourceIDInfoOfControl(BOOKLET_PAPER_SIZE);
	pControl = new  CShBookletStandardPSize(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET_OUTPUTSIZE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE],
		m_pFeatureInfoList,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_BOOKLET_OUTPUTSIZE],
		m_ObjCShowMessage,
		IDC_CMB_OUTPUTSIZE,
		m_PropertySheetState
	);
	//<E> To support maximum paper size, 2020.10.09, SSDI: Chanchal Singla
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_OUTPUTSIZE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}
	//<E>To Modified the Support of Booklet Standard Output Size ,2020-09-02,SSDI:Chanchal Singla
		//Binding
	pResourceIDInfo = GetResourceIDInfoOfControl(BINDING_EDGE);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE],
		m_pFeatureInfoList,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_BINDINGEDGE],
		m_ObjCShowMessage,
		IDC_CMB_BINEDGE3,
		m_PropertySheetState
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_BINEDGE3, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	//Booklet
	pResourceIDInfo = GetResourceIDInfoOfControl(BOOKLET);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_BOOKLET],
		m_ObjCShowMessage,
		IDC_CMB_PAMPHLET
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_PAMPHLET, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_PAMPHLET_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PAMPHLET_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}


	//Stamp
	pResourceIDInfo = GetResourceIDInfoOfControl(STAMP);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAMP],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAMP],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_STAMP],
		m_ObjCShowMessage,
		IDC_CMB_STAMP
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_STAMP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	//N-Up Order
	pResourceIDInfo = GetResourceIDInfoOfControl(ORDER);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ORDER],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER],
		m_pFeatureInfoList,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_ORDER],
		m_ObjCShowMessage,
		IDC_CMB_NUP_ORDER,
		m_PropertySheetState
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_NUP_ORDER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	//Border Checkbox
	pResourceIDInfo = GetResourceIDInfoOfControl(BORDER);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BORDER],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BORDER],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_BORDER],
		m_ObjCShowMessage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_NUP_BORDER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}

	//100% N-UP Checkbox
	pResourceIDInfo = GetResourceIDInfoOfControl(HUNDRED_PERCENT_NUP);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_HUNDRED_PERCENT_NUP],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_HUNDRED_PERCENT_NUP],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_HUNDRED_PERCENT_NUP],
		m_ObjCShowMessage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_CMPT_SAMENUP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}

	//Duplex
	pResourceIDInfo = GetResourceIDInfoOfControl(DUPLEX);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_DUPLEX],
		m_ObjCShowMessage,
		IDC_CMB_DUPLEX
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_DUPLEX, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}


	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_DUPLEX_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_DUPLEX_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}


	//<S><A> To Add Support of Print Position Dialog, 20-05-2020, SSDI:Chanchal Parkash
		//Print Position sButton

	pControl = new CButton(IDC_BTN_PRINTPOSITION, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PRINTPOSITION, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_BTN_PRINTPOSITION)\n");
	}
	//<E> To Add Support of Print Position Dialog, 20-05-2020, SSDI:Chanchal Parkash
	//<S><A> To Add Support of Water Mark Dialog, 04-09-2020, SSDI:Chanchal Parkash
		//Settings - Button
	pControl = new CButton(IDC_BTN_WATERMARK, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WATERMARK, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_BTN_WATERMARK)\n");
	}
	//<E> To Add Support of Water Mark Dialoge, 04-09-2020, SSDI:Chanchal Parkash

		//<S><A>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
		//--- Mimic Left Side
	pControl = new CMimicImage(IDC_BTN_SHARED_BBOX, m_hPropertySheetHandle, m_pFeatureInfoList,
		m_hStringResourceHandle, m_hBitmapResourceHandle, m_pPrinterName, m_pOemPrivateDevMode, m_pWaterMarkIndex);
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
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CMimicImage)\n");
	}
	//--- Mimic Right Side
	pControl = new CMimicText(IDC_BTN_SHARED_BBOX_SUB, m_hPropertySheetHandle, m_pFeatureInfoList,
		m_hStringResourceHandle, m_hBitmapResourceHandle, m_pPrinterName, m_pOemPrivateDevMode, m_hPrinterHandle);
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
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CMimicImage)\n");
	}
	//<E>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla

	//Favorites Combo box
	// <S><C> Implementing Factory defaults,2021.01.28, SSDI:Chandrashekar .V
	pControl = new CShCmbUserset(
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
		m_PropertySheetState
	);
	// <E> Implementing Factory defaults,2021.01.28, SSDI:Chandrashekar .V
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
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

}


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
void CLayoutTabPropertyPage::SetControlStrings(_In_ CONST HWND   hDlg)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	//Favorites label
	CUIControl* pControl = new CStaticText(IDC_STC_SHARED_USRSET, m_hStringResourceHandle);

	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_SHARED_USRSET, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	//Favorites - Delete button
	pControl = new CStaticText(IDS_IDC_BTN_SHARED_US_DEL, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_SHARED_US_DEL, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}
	//<S><A>Bug 1952 : 2020.06.17, SSDI:Seetharam
	//Hide the Delete button on dialog initialization
	ShowWindow(GetDlgItem(hDlg, IDS_IDC_BTN_SHARED_US_DEL), SW_HIDE);
	//<E>Bug 1952 : 2020.06.17, SSDI:Seetharam

	//Favorites - Save button
	pControl = new CStaticText(IDS_IDC_BTN_SHARED_US_SAVE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_SHARED_US_SAVE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}


	// Defaults button
	pControl = new CStaticText(IDS_IDC_BTN_SHARED_DEFAULT, m_hStringResourceHandle);
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
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}
	//N-Up - Pages per sheet
	pControl = new CStaticText(IDS_IDC_STC_NUP, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_NUP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_NUP)\n");
	}
	//2-Sided printing
	pControl = new CStaticText(m_pResourceIDInfoMap->find(DUPLEX)->second.uFeatureResourceID, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_GRP_PRINT_ON_BOTH_SIDES, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_PRINT_ON_BOTH_SIDES)\n");
	}
	//Zoom Settings - Button
	pControl = new CStaticText(IDS_IDC_BTN_WATERMARK, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WATERMARK, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_PRINT_ON_BOTH_SIDES)\n");
	}
	//Print Position - Button
	pControl = new CStaticText(IDS_IDC_BTN_PRINTPOSITION, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PRINTPOSITION, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_PRINT_ON_BOTH_SIDES)\n");
	}
	//2-Sided printing
	pControl = new CStaticText(IDS_IDC_STC_DUPLEX, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_DUPLEX, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_NUP)\n");
	}

	//NUP Order label
	pControl = new CStaticText(IDS_IDC_STC_NUP_ORDER, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_NUP_ORDER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_PRINT_ON_BOTH_SIDES)\n");
	}
	//Booklet label
	pControl = new CStaticText(IDS_IDC_STC_PAMPHLET, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PAMPHLET, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_PAMPHLET)\n");
	}
//<S><A>Added string Control for paper size dimention,2020.05.14 - SSDI:Goutham
	pControl = new CStaticText(IDS_IDC_STC_PAPER, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_OUTPAPER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_OUTPAPER)\n");
	}
//<S><E>Added string Control for paper size dimention,2020.05.14 - SSDI:Goutham
	//Output Size label
	pControl = new CStaticText(IDS_IDC_STC_OUTPUTSIZE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_OUTPUTSIZE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_OUTPUTSIZE)\n");
	}

	//Binding edge label
	pControl = new CStaticText(IDS_IDC_STC_BINEDGE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_BINEDGE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_BINEDGE)\n");
	}

	//Stamp label
	pControl = new CStaticText(IDS_IDC_STC_STAMP, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_STAMP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_STAMP)\n");
	}
}


//=============================================================================
// function
//      SendCommand
//
// return value
//     HRESULT
//
// outline
//     To handle button Click events.
//=============================================================================
HRESULT	CLayoutTabPropertyPage::SendCommand(
	_In_ CONST HWND   hDlg,
	_In_ CONST WPARAM wParam
)

{

	//<S><A> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
	CUIControl		pUIControl;
	BOOL			blChanged = FALSE;
	BOOL			blDefault = FALSE;
	m_blIgnoreUntitled = FALSE;
	//========================================================================
	//This Flag variable is reserved , please don't used this flag for other purpose and 
	// don't change the value,otherwise driver maybe crashed
	BOOL wFlag = FALSE;
	//========================================================================

	// Temporary Public Devmode
	DEVMODE wTempOemPublicDevMode;
	SecureZeroMemory(&wTempOemPublicDevMode, sizeof(wTempOemPublicDevMode));
	memcpy_s(&wTempOemPublicDevMode, sizeof(DEVMODE), m_pOemPublicDevMode, sizeof(DEVMODE));

	//// Previous public Devmode
	DEVMODE wOemPublicDevMode_Pre;
	SecureZeroMemory(&wOemPublicDevMode_Pre, sizeof(wOemPublicDevMode_Pre));

	// Temporary Private Devmode
	OEMDEV wTempOemPrivateDevMode;
	SecureZeroMemory(&wTempOemPrivateDevMode, sizeof(wTempOemPrivateDevMode));
	memcpy_s(&wTempOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));

	//// Previous Private Devmode
	OEMDEV wOemPrivateDevMode_Pre = {};
	SecureZeroMemory(&wOemPrivateDevMode_Pre, sizeof(wOemPrivateDevMode_Pre));

	// Temporary FeatureInfoList
	PFEATUREINFOLIST wTempFeatureInfoList = NULL;
	wTempFeatureInfoList = InitFeatureInfoList(wTempFeatureInfoList);
	wTempFeatureInfoList = UpdateFeatureInfoList(wTempFeatureInfoList, m_pFeatureInfoList);

	// Previous FeatureInfoList
	PFEATUREINFOLIST wFeatureInfoList_Pre = NULL;
	wFeatureInfoList_Pre = InitFeatureInfoList(wFeatureInfoList_Pre);

	// temp wUSindex -- user setting
	short wUsIndex = *m_pUsIndex;

	// Previous User Setting Index
	short wUsIndex_pre = 0;
	//<E> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla

	__super::SendCommand(hDlg, wParam);

	WORD wNotificationCode = HIWORD(wParam);
	WORD wContolID = LOWORD(wParam);
	switch (wNotificationCode)
	{
	case BN_CLICKED:
	{
		OnBnClicked(hDlg, FALSE, wContolID);
	}
	break;
	case CBN_SELCHANGE:
	{
		switch (wContolID)
		{
		case IDC_CMB_PAMPHLET:
		{
		
			if (!strcmp("None", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption)
				&& !strcmp("TOPEDGE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE].pszOption))
			{
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex = 0;
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE].pszOption = "LEFTEDGE";
			}
			//<S><C> To support maximum paper size, 2020.10.09, SSDI: Chanchal Singla
			//<S><A>To Fix Redmine Bug #2730,2706,2736,2020-09-08,SSDI:Chanchal Singla
			//Booklet output size drop down value is not set to default when Booklet feature is set to Off and ON
			UINT suCurrentItemId = m_DefaultSettings[FEATURE_BOOKLET_OUTPUTSIZE].ResourceID;
			TCHAR szItem[MAX_PATH] = { 0 };
			UINT uChangedOptionIndex = 0;
			if (LoadString(m_hStringResourceHandle, suCurrentItemId, szItem, countof(szItem)) > 0)
			{
				if (m_PropertySheetState.byMaxPaperSize == eFirstOption)
				{
					uChangedOptionIndex = static_cast<UINT>(SendDlgItemMessage(hDlg, IDC_CMB_OUTPUTSIZE, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szItem));
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET_OUTPUTSIZE].pszOption = (uChangedOptionIndex == eFirstOption) ? "A3" : "TABLOID";
				}
				else if (m_PropertySheetState.byMaxPaperSize == eSecondOption)
				{
					if (0 == wcscmp(szItem, L"A4"))
					{
						uChangedOptionIndex = eSecondOption;
					}
					else if (0 == wcscmp(szItem, L"Letter"))
					{
						uChangedOptionIndex = eFifthOption;
					}
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET_OUTPUTSIZE].pszOption = (uChangedOptionIndex == eSecondOption) ? "A4" : "LETTER";
				}

				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex = uChangedOptionIndex;
				SendDlgItemMessage(hDlg, IDC_CMB_OUTPUTSIZE, CB_SETCURSEL, uChangedOptionIndex, 0);
			}

			//<E>To Fix Redmine Bug #2730,2020-09-08,SSDI:Chanchal Singla
			//<S><C>To Fix Redmine #3358,2021/03/05,SSDI: Chanchal Singla
			//Booklet drop down is grayed out even Output Size droop down contains A4
			//Similar To UD3.1 FitToPaper Size Should be set to default value,
			//when Booklet Select to Standard and again Set To None
			if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
			{
				//<S><C>fix redmine Bug #4123- Issue-1,13.August.2021,SSDI:Chanchal Singla
				//Output Size does not set to Paper Size based on the Region Settings
				UINT uPaperIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
				if ((m_PropertySheetState.byMaxPaperSize == eSecondOption) &&
					((uPaperIndex == PAPERSIZE_A3) || (uPaperIndex == PAPERSIZE_B4)
						|| (uPaperIndex == PAPERSIZE_TABLOID) || (uPaperIndex == PAPERSIZE_JENV_KAKU2)))
				{
					PCSTR SellectedOptionName = NULL;
					SellectedOptionName = (GetPaperSizeDef() == DMPAPER_A4) ? "A4" : "LETTER";
					UINT uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? OUTPUTPAPERSIZE_A4 : OUTPUTPAPERSIZE_LETTER;
					m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex = uChangedOptionIndex;
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption = SellectedOptionName;
				}
				else
				{
					suCurrentItemId = m_DefaultSettings[FEATURE_FITTOPAPERSIZE].ResourceID;
					if (LoadString(m_hStringResourceHandle, suCurrentItemId, szItem, countof(szItem)) > 0)
					{
						m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption = "SAOS";
						m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex = eFirstOption;
					}
				}
				//<E>fix redmine Bug #4123- Issue-1,13.August.2021,SSDI:Chanchal Singla
			}
			//<E>To Fix Redmine #3358,2021/03/05,SSDI: Chanchal Singla
			//<E> To support maximum paper size, 2020.10.09, SSDI: Chanchal Singla
		}
			break;
		case IDC_CMB_STAMP:
		{
			//<S><A>To Fix #3086,20201612,SSDI:Chanchal Singla
			//Watermark drop down is displaying previous selected value.
			if (!strcmp("None", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAMP].pszOption))
			{
				m_pOemPrivateDevMode->scpi.ext.nWatermark = 0;
				*m_pWaterMarkIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
			}
			//<E>To Fix #3086,20201612,SSDI:Chanchal Singla

		}
			break;
		case IDC_CMB_SHARED_USRSET:
			OnUsSelect(hDlg, m_pPrinterName, IDC_CMB_SHARED_USRSET);
			
			break;
		default:
			break;
		}
	}
	break;
	}

	//<S><A> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
	// Update Previous Public devmode
	wOemPublicDevMode_Pre = wTempOemPublicDevMode;

	// Update Previous wUSIndex --USer Setting Index
	wUsIndex_pre = wUsIndex;

	// Update Previous Privatedevmode
	wOemPrivateDevMode_Pre = wTempOemPrivateDevMode;

	// Update Previous FeatureInfoList
	//wFeatureInfoList_Pre = wTempFeatureInfoList;
	wFeatureInfoList_Pre = UpdateFeatureInfoList(wFeatureInfoList_Pre, wTempFeatureInfoList);

	// Compare the Public, Private dev-mode, and m_pFeatureInfoList to change the dropdown of factory default
	if ((pUIControl.IsEqualBuf(&wOemPrivateDevMode_Pre, m_pOemPrivateDevMode, sizeof(OEMDEV)) == FALSE) /*Private devmode*/
		|| (pUIControl.IsEqualBuf(&wOemPublicDevMode_Pre, m_pOemPublicDevMode, sizeof(DEVMODE)) == FALSE) /*Public devmode*/
		|| (CompareFeatureInfoList(wFeatureInfoList_Pre, m_pFeatureInfoList) == FALSE) /*m_pFeatureInfoList*/
		)
	{

		blChanged = TRUE;
		// For Setting the User setting index to Untitled
		if (m_blIgnoreUntitled == FALSE)
			SetUSUntitled(&wOemPrivateDevMode_Pre, m_pOemPrivateDevMode, &wOemPublicDevMode_Pre, m_pOemPublicDevMode, wFeatureInfoList_Pre, m_pFeatureInfoList, wUsIndex_pre, *m_pUsIndex, blDefault, FALSE);
	}
	// caling the SetTabState is Required for Updating the all Features Option and Constraints(Show/Hide)
	SetTabState(hDlg, LOWORD(wParam));
	// For Setting the User setting index to Untitled
	if (m_blIgnoreUntitled == FALSE && blChanged != FALSE)
		SetUSUntitled(&wOemPrivateDevMode_Pre, m_pOemPrivateDevMode, &wOemPublicDevMode_Pre, m_pOemPublicDevMode, wFeatureInfoList_Pre, m_pFeatureInfoList, wUsIndex_pre, *m_pUsIndex, blDefault, TRUE);

	/*Delete the momory allocation of wFeatureInfoList_Pre and wTempFeatureInfoList*/
	//this condition is kept because driver Crashing in case of FeatureInfoList is NULL 
	if ((NULL == wFeatureInfoList_Pre))
	{
		wFlag = TRUE;
	}
	if (wFlag == FALSE)
	{
		//Fixed Coverity #91491,13-08-2021,SSDI:Chanchal Singla
		if (NULL != wTempFeatureInfoList)
		{
			delete wTempFeatureInfoList;
			wTempFeatureInfoList = NULL;
		}
		//Fixed Coverity #91415,13-08-2021,SSDI:Chanchal Singla
		if (NULL != wFeatureInfoList_Pre)
		{
			delete wFeatureInfoList_Pre;
			wFeatureInfoList_Pre = NULL;
		}
	}
	//<E> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
	
	return S_OK;

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
HRESULT CLayoutTabPropertyPage::OnDrawItem(_In_ CONST HWND   hDlg, _In_ CONST WPARAM wParam, _In_ CONST LPARAM lParam)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	short				wCtrlID = (short)wParam;
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;
	UIControlMap::const_iterator iComboIterator = m_UIControls.find(pdis->CtlID);
	INT iIncrementIndex = 0;
	if (iComboIterator != m_UIControls.end())
	{
		CUIControl * pControl = static_cast<CUIControl*>(iComboIterator->second);
		if (pControl != NULL)
		{
			if (pControl->GetControlType() == eComboBox)
			{
				if (NULL != pControl)
				{
					if (wCtrlID != IDC_CMB_SHARED_USRSET)
					{
						((CShImageCombo *)pControl)->OnWmDrawItem(hDlg, wParam, lParam);
					}
				}
				//Fixed Coverity issue - 90392 - SSDI:Seetharam-20201006
				//else
				//{
				//	slog.putLog("CFinishingTabPropertyPage::OnWmDrawItem -> NULL_OBJECT(CShImageCombo)\n");
				//}
			}
		}
	}
	//<S><A>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
	{
		switch (wCtrlID)
		{
		case IDC_BTN_SHARED_BBOX:
		{
			UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_SHARED_BBOX);
			if (Iterator != m_UIControls.end())
			{
				CMimicImage *pMimic = static_cast<CMimicImage*>(Iterator->second);
				if (pMimic)
				{
					pMimic->Draw(&m_PropertySheetState);
				}
			}
		}
		break;
		case IDC_BTN_SHARED_BBOX_SUB:
		{
			UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_SHARED_BBOX_SUB);
			if (Iterator != m_UIControls.end())
			{
				CMimicText *pMimicText = static_cast<CMimicText*>(Iterator->second);
				if (pMimicText)
				{
					pMimicText->Draw(&m_PropertySheetState);
				}
			}
		}
			break;
		case IDC_CMB_SHARED_USRSET:
		{
			UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_SHARED_USRSET);
			if (Iterator != m_UIControls.end())
			{
				CShCmbUserset *pUserSetting = static_cast<CShCmbUserset*>(Iterator->second);
				if (pUserSetting)
				{
					pUserSetting->DrawUserSetting(lParam);
				}
			}
		}
		break;
		//<S><A>to Fix Redmine Bug #2240 Issue 2,03.02.2022,SSDI:Chanchal Singla
		//Issue 2: Information icon should be highlighted when pressing Tab button
		case IDC_BTN_DUPLEX_I:
		case IDC_BTN_PAMPHLET_I:
		case IDC_BTN_NUP_I:
		case IDC_BTN_PRINTPOSITION_I:
		//case IDC_CMB_STAMP: till now not used
		{
			UIControlMap::const_iterator Iterator = m_UIControls.find(wCtrlID);
			if (Iterator != m_UIControls.end())
			{
				CInformationIconButton *pIIcon = static_cast<CInformationIconButton*>(Iterator->second);
				if (pIIcon)
				{
					pIIcon->Draw((DRAWITEMSTRUCT *)lParam);
				}
				else
				{
					slog.putLog("CInformationIconButton::OnWmDrawItem -> NULL_OBJECT(CInformationIconButton)\n");
				}
			}
		}
		break;
		//<E>to Fix Redmine Bug #2240 Issue 2,03.02.2022,SSDI:Chanchal Singla
		default:
			break;
		}
	}
	//<E>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
	return CDocumentPropertyPage::OnDrawItem(hDlg, wParam, lParam);
}
// <S><E> To handle the ondraw for custom controls like custom combo box , 20200128 - SSDI:Seetharam



//=============================================================================
// function
//      SetTabDefaults
//
// return value
//     HRESULT
//
// outline
//      Sets tab defaults.
//=============================================================================
HRESULT CLayoutTabPropertyPage::SetTabDefaults(_In_ CONST HWND   hDlg)
{
	//<S><C>to Fix #Bug #3690- issue 4,2021.12.22,SSDI:Chanchal Singla
	if (0 == *m_pUsIndex)
	{
		return S_OK;
	}
	//<E>to Fix #Bug #3690- issue 4,2021.12.22,SSDI:Chanchal Singla
	__super::SetTabDefaults(hDlg);
	ShowFinishingInformationIcon(hDlg);
	ShowConstraintsOnNUP(hDlg, IDC_CMB_NUP);
	ShowConstraintsOnBooklet(hDlg, IDC_CMB_PAMPHLET);
	ShowConstraintsOnDuplex(hDlg, IDC_CMB_DUPLEX);
	ShowConstraintsOnStamp(hDlg, IDC_CMB_STAMP);
	//ShowConstraintsOnCollate(hDlg);

	//<S><A> Added to Support PrintPosition Dialoge, SSDI:08-06-2020, Chanchal Parkash
	PrintPositionSetTabDefaults(hDlg);
	//<E> Added to Support PrintPosition Dialoge, SSDI:08-06-2020, Chanchal Parkash
	WaterMarkSetTabDefaults(hDlg);

	return S_OK;
}

//<S><A>Added function for paper size dimentions in layout tab,2020.05.14 - SSDI:Goutham
VOID CLayoutTabPropertyPage::ShowPaperSizeDimensions(HWND hDlg, INT iCtrlID)
{
	UIControlMap::const_iterator Iterator = m_UIControls.find(iCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pCombo = static_cast<CShImageCombo*>(Iterator->second);
		INT index = pCombo->GetSelectedIndex();
		WCHAR cSep = GetDecimalSeparator();

		WCHAR			szText[256] = {};
		WCHAR			sztemp[256] = {};
		WCHAR			szw[32] = {};
		WCHAR			szl[32] = {};
		WCHAR			szu[32] = {};

		if (iCtrlID == IDC_CMB_OUTPUTSIZE && index == 0)
		{
			//<S><C> to fix #2253 :2020/6/23  Harika
			//index = static_cast<UINT>(SendDlgItemMessage(hDlg, IDC_CMB_PAPER, CB_GETCURSEL, 0, 0));
			index = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
			//<E> to fix #2253 :2020/6/23  Harika
			//index = (short)SendMessage(::GetDlgItem(hDlg, IDC_CMB_PAPER), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			//int index = SendDlgItemMessage(hDlg, IDC_CMB_PAPER, CB_GETCURSEL, 0, 0);
		}
		else if (iCtrlID == IDC_CMB_OUTPUTSIZE && index != 0)
		{
			index = index - 1;
			//int index = SendDlgItemMessage(hDlg, IDC_CMB_PAPER, CB_GETCURSEL, 0, 0);
		}
		int PaperSize = PaperSizeArray[index][0];
		int Width = PaperSizeArray[index][1];
		int Length = PaperSizeArray[index][2];
		int unit = PaperSizeArray[index][3];
		/*if (CUSTOM_PAPER_INDEX > index)
		{
			PaperSize = PaperSizeArray[index][0];
			Width = PaperSizeArray[index][1];
			Length = PaperSizeArray[index][2];
			unit = PaperSizeArray[index][3];
		}
		else
		{*/
		//PaperSize = DMPAPER_CUSTOM;
		//UIControlMap::const_iterator Iterator = m_UIControls.find(PaperSize);
		//if (Iterator != m_UIControls.end())
		//{
		//	CCustomPaperButton *pCustomPaperButton = static_cast<CCustomPaperButton*>(Iterator->second);
		//	Width = pCustomPaperButton->GetWidth();
		//	Length = pCustomPaperButton->GetLength();

		//	if (unit_mm == pCustomPaperButton->GetUnit())
		//	{
		//		unit = MM_LOMETRIC;
		//	}
		//	else//inch
		//	{
		//		unit = MM_LOENGLISH;
		//	}
		//}
	//}
		switch (unit)
		{
		case MM_LOENGLISH:
		{
			WCHAR szForm[MAX_FORMNAME_TEXT] = {};
			wcscpy_s(szForm, L"%d%c%02d");
			StringCbPrintf(szw, sizeof(szw), szForm, Width / 100, cSep, Width % 100);
			StringCbPrintf(szl, sizeof(szl), szForm, Length / 100, cSep, Length % 100);
			LoadString(m_hStringResourceHandle, IDS_UNIT_INCH, szu, countof(szu));
		};
		break;
		case MM_LOMETRIC:
		default:
		{
			WCHAR szForm[MAX_FORMNAME_TEXT] = {};
			if (DMPAPER_CUSTOM == PaperSize)
			{
				wcscpy_s(szForm, L"%d%c%d");
			}
			else
			{
				wcscpy_s(szForm, L"%d");
			}
			StringCbPrintf(szw, sizeof(szw), szForm, Width / 10, cSep, Width % 10);
			StringCbPrintf(szl, sizeof(szl), szForm, Length / 10, cSep, Length % 10);
			LoadString(m_hStringResourceHandle, IDS_UNIT_MM, szu, countof(szu));
		};

		break;
		}
		if (iCtrlID == IDC_CMB_PAPER)
		{
			LoadString(m_hStringResourceHandle, IDC_STC_PAPER, szText, countof(szText));
			StringCbPrintf(sztemp, sizeof(sztemp), szText, szw, szl, szu);
			SetDlgItemText(hDlg, IDC_STC_PAPER, sztemp);
		}
		if (iCtrlID == IDC_CMB_OUTPUTSIZE)
		{
			LoadString(m_hStringResourceHandle, IDS_IDC_STC_PAPER, szText, countof(szText));
			StringCbPrintf(sztemp, sizeof(sztemp), szText, szw, szl, szu);
			SetDlgItemText(hDlg, IDC_STC_OUTPAPER, sztemp);
		}
		SecureZeroMemory(szText, sizeof(szText));
		SecureZeroMemory(szl, sizeof(szl));
		SecureZeroMemory(sztemp, sizeof(sztemp));

	}

}
//<S><E>Added function for paper size dimentions in layout tab,2020.05.14 - SSDI:Goutham

	
VOID CLayoutTabPropertyPage::SetTabState(
	_In_ CONST HWND   hDlg,
	_In_ CONST INT iCtrlID
)
{
	SetUserSettingState(hDlg, IDC_CMB_SHARED_USRSET);

	//<S><A>To Add the Support of Booklet Standard Output Size ,2020-09-02,SSDI:Chanchal Singla
	UIControlMap::const_iterator Iterator;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	Iterator = m_UIControls.find(IDC_CMB_OUTPUTSIZE);
	long BookletStandardPSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex;
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			pImageCombo->Select(IDC_CMB_OUTPUTSIZE, hDlg, BookletStandardPSize);
		}
		else
		{
			slog.putLog("CLayoutTabPropertyPage::Select -> NULL_OBJECT(pImageCombo)\n");
		}

	}

	//Duplex
	DuplexFeatureInitialization(hDlg, IDC_CMB_DUPLEX);

	// ---Feature Booklet
	long Booklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_PAMPHLET, Booklet);

	// ---Feature Binding Edge
	long Bindedge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_BINEDGE3, Bindedge);
	//----Feature Nup
	long wNUP = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_NUP, wNUP);
	//----Feature NupOrder
	long wNupOrder = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_NUP_ORDER, wNupOrder);

	//-- feature border
	LONG wNupBorder = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BORDER].uChangedOptionIndex;
	CheckDlgButton(hDlg, IDC_CHK_NUP_BORDER, wNupBorder);
	SetCheckBoxValue(hDlg, IDC_CHK_NUP_BORDER, wNupBorder);

	//-- feature 100%
	LONG wSameNUp = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_HUNDRED_PERCENT_NUP].uChangedOptionIndex;
	CheckDlgButton(hDlg, IDC_CHK_CMPT_SAMENUP, wSameNUp);
	SetCheckBoxValue(hDlg, IDC_CHK_CMPT_SAMENUP, wSameNUp);

	long wStamp = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAMP].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_STAMP, wStamp);

	SetControlState(hDlg, IDC_BTN_STAMP_I, SCUI_STATE_OFF);
	SetControlState(hDlg, IDC_BTN_PRINTPOSITION_I, SCUI_STATE_OFF);
	//ShowPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	ShowBookletPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	//<E>To Add the Support of Booklet Standard Output Size ,2020-09-02,SSDI:Chanchal Singla
	ShowConstraintsOnNUP(hDlg, IDC_CMB_NUP);
	ShowConstraintsOnBooklet(hDlg, IDC_CMB_PAMPHLET);
	ShowConstraintsOnDuplex(hDlg, IDC_CMB_DUPLEX);
	ShowConstraintsOnStamp(hDlg, IDC_CMB_STAMP);
	ShowConstraintsOnZoomSetting(hDlg);
	//<S><A>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	memcpy(m_ppi->pps, &m_PropertySheetState, sizeof(PROPSTATE));
	//<E>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla

	//if (IsPrinterOperators(m_pPrinterName) == FALSE)
	//	DisableAllCtrls();
}

//=============================================================================
// function
//       ShowFinishingInformationIcon
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       ShowFinishingInformationIcon
//=============================================================================
VOID CLayoutTabPropertyPage::ShowFinishingInformationIcon(HWND hDlg)
{
	UINT uStapleIndex = 0;
	UINT uPunchIndex = 0;
	uStapleIndex = SendDlgItemMessage(hDlg, IDC_CMB_STAPLE, CB_GETCURSEL, 0, 0);
	uPunchIndex = SendDlgItemMessage(hDlg, IDC_CMB_PUNCH, CB_GETCURSEL, 0, 0);
	if ((0 == uStapleIndex) && (0 == uPunchIndex))
	{
		SetControlState(hDlg, IDC_BTN_FINISH_I, SCUI_STATE_OFF);
	}
	else
	{
		SetControlState(hDlg, IDC_BTN_FINISH_I, SCUI_STATE_ON);
	}

}

//=============================================================================
// function
//      OnBnClicked
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      hWndCtrl            handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      LRESULT
//
// outline
//      BN_CLICKED 
//=============================================================================
LRESULT CLayoutTabPropertyPage::OnBnClicked(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	LRESULT			res = 0;
	BOOL			blChecked;

	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();

	switch (wCtrlID)
	{
	case IDC_CHK_NUP_BORDER:
	{
		pControl = (*pComponents)[IDC_CHK_NUP_BORDER];
		((CCheckBox*)pControl)->OnBnClicked(wCtrlID, hDlg);
	}
	break;
	case IDC_CHK_CMPT_SAMENUP:
	{
		pControl = (*pComponents)[IDC_CHK_CMPT_SAMENUP];
		((CCheckBox*)pControl)->OnBnClicked(wCtrlID, hDlg);
	}
	break;
	//<S><A> To Add Support of Print Position Dialoge, 20-05-2020, SSDI:Chanchal Parkash
	case IDC_BTN_PRINTPOSITION:
		OnPrintPositionDialog(hDlg);
		break;
	//<E> To Add Support of Print Position Dialog, 20-05-2020, SSDI:Chanchal Parkash
	//<S><A> To Add Support of WaterMark Dialog, 04-09-2020, SSDI:Chanchal Parkash
	case IDC_BTN_WATERMARK:
		OnWaterMarkDialog(hDlg);
		break;
	//<E> To Add Support of WaterMark Dialog, 04-09-2020, SSDI:Chanchal Parkash
	case IDC_BTN_SHARED_DEFAULT:
		SetTabDefaults(hDlg);
		break;
	case IDC_BTN_SHARED_US_SAVE:
		OnUsSave(hDlg);
		break;

	case IDC_BTN_SHARED_US_DEL:
		OnUsDelete(hDlg);
		break;
	//<S><A>to Fix Redmine Bug #2240 Issue 3,25.02.2022,SSDI:Chanchal Singla
	//Issue 3: Through tab order, move the focus on to the information icon Press "Space bar" from the keyboard, Information dialog is not popped up
	case IDC_BTN_DUPLEX_I:
	case IDC_BTN_PAMPHLET_I:
	case IDC_BTN_NUP_I:
	case IDC_BTN_PRINTPOSITION_I:
		//case IDC_CMB_STAMP: till now not used
	{
		UIControlMap::const_iterator Iterator = m_UIControls.find(wCtrlID);
		if (Iterator != m_UIControls.end())
		{
			CInformationIconButton *pIIcon = static_cast<CInformationIconButton*>(Iterator->second);
			if (pIIcon)
			{
				pIIcon->OnBnClickedInformationIcon(hDlg, wCtrlID, FALSE);
			}
			else
			{
				slog.putLog("CInformationIconButton::OnBnClicked -> NULL_OBJECT(CInformationIconButton)\n");
			}
		}
	}
	break;
	//<E>to Fix Redmine Bug #2240 Issue 3,25.02.2022,SSDI:Chanchal Singla
	default:
		//return PARENT_CLASSNAME::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
		break;
		;
	}

	res = 1;
	
	return res;
}
//=============================================================================
// function
//       ShowConstraintsOnNUP
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints On Nup
//=============================================================================
VOID CLayoutTabPropertyPage::ShowConstraintsOnNUP(HWND hDlg, INT iCtrlID)
{
	UINT uChangedOptionIndex = 0;
	SHORT wOri = GetOrientation(m_pFeatureInfoList);
	UINT NupValue = atol(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption);
	if (0 != SendDlgItemMessage(hDlg, iCtrlID, CB_GETCURSEL, 0, 0))
	{
		SetControlState(hDlg, IDC_STC_NUP_ORDER, SCUI_STATE_ON);
		SetControlState(hDlg, IDC_CMB_NUP_ORDER, SCUI_STATE_ON);
		SetControlState(hDlg, IDC_CHK_NUP_BORDER, SCUI_STATE_ON);
		SetControlState(hDlg, IDC_CHK_CMPT_SAMENUP, SCUI_STATE_ON);
		//<S><A>To Fix #3011,20201130,SSDI:Chanchal Singla
		//N-Up order drop down is not grayed out when Landscape and 2-Up is set
		if ((ori_port == wOri) && NupValue == nup_2)
		{
			SetControlState(hDlg, IDC_STC_NUP_ORDER, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_CMB_NUP_ORDER, SCUI_STATE_SHOW);
		}
		//<E>To Fix #3008,20201130,SSDI:Chanchal Singla
	}
	else
	{
		SetControlState(hDlg, IDC_STC_NUP_ORDER, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_NUP_ORDER, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CHK_NUP_BORDER, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CHK_CMPT_SAMENUP, SCUI_STATE_OFF);
		//<S><A>To Fix #3008,20201130,SSDI:Chanchal Singla
		//Mimic is not displayed when N-Up is set to None
		{
			// Nup Border
			(CheckDlgButton(hDlg, IDC_CHK_NUP_BORDER, BST_UNCHECKED));
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BORDER].pszOption = "OFF";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BORDER].uChangedOptionIndex = bool_false;
			
			//Sameup
			(CheckDlgButton(hDlg, IDC_CHK_CMPT_SAMENUP, BST_UNCHECKED));
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_HUNDRED_PERCENT_NUP].pszOption = "OFF";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_HUNDRED_PERCENT_NUP].uChangedOptionIndex = bool_false;
		}
		//<E>To Fix #3008,20201130,SSDI:Chanchal Singla
	}
}

//=============================================================================
// function
//       ShowConstraintsOnZoomSetting
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints On ZoomSetting
//=============================================================================
VOID CLayoutTabPropertyPage::ShowConstraintsOnZoomSetting(HWND hDlg, INT iCtrlID)
{
	if (!strcmp("ON", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption))
	{
		m_ControlState.cCmbNup = SCUI_STATE_SHOW;
		m_ControlState.cNup_I = SCUI_STATE_ON;
	}
	else if (!strcmp("OFF", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption))
	{
		m_ControlState.cCmbNup = SCUI_STATE_ON;
		m_ControlState.cNup_I = SCUI_STATE_OFF;
	}
	SetControlState(hDlg, IDC_STC_NUP, m_ControlState.cCmbNup);
	SetControlState(hDlg, IDC_CMB_NUP, m_ControlState.cCmbNup);
	SetControlState(hDlg, IDC_BTN_NUP_I, m_ControlState.cNup_I);
}

//=============================================================================
// function
//       ShowConstraintsOnStamp
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints On Stamp
//=============================================================================
VOID CLayoutTabPropertyPage::ShowConstraintsOnStamp(HWND hDlg, INT iCtrlID)
{
	if (0 != SendDlgItemMessage(hDlg, iCtrlID, CB_GETCURSEL, 0, 0))
	{
		SetControlState(hDlg, IDC_BTN_WATERMARK, SCUI_STATE_ON);
	}
	else
	{
		SetControlState(hDlg, IDC_BTN_WATERMARK, SCUI_STATE_OFF);
	}
}
//=============================================================================
// function
//       ShowConstraintsOnBooklet
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints On Booklet
//=============================================================================
VOID CLayoutTabPropertyPage::ShowConstraintsOnBooklet(HWND hDlg, INT iCtrlID)
{
	UINT NupValue = atol(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption);
	m_PropertySheetState.wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	if (0 != SendDlgItemMessage(hDlg, iCtrlID, CB_GETCURSEL, 0, 0))
	{

		m_ControlState.cDuplex_I = SCUI_STATE_ON;
		m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
		SetControlState(hDlg, IDC_STC_OUTPUTSIZE, SCUI_STATE_ON);
		SetControlState(hDlg, IDC_CMB_OUTPUTSIZE, SCUI_STATE_ON);
//<S><A>Added contraint for paper size dimention,2020.05.14 - SSDI:Goutham
		SetControlState(hDlg, IDC_STC_OUTPAPER, SCUI_STATE_ON);
//<S><E>Added contraint for paper size dimention,2020.05.14 - SSDI:Goutham
		//<S><C>Modified to fix #3014,20201130,SSDI:Chanchal Singla
		//Binding Edge displayed when N - Up is set
		SHORT wOri = GetOrientation(m_pFeatureInfoList);
		if (wOri == ori_port){
			if ((NupValue == nup_2) ||
				(NupValue == nup_6) ||
				(NupValue == nup_8)) {
				SetControlState(hDlg, IDC_STC_BINEDGE, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_CMB_BINEDGE3, SCUI_STATE_ON);
			}
			else {
				SetControlState(hDlg, IDC_STC_BINEDGE, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_CMB_BINEDGE3, SCUI_STATE_OFF);
			}
		
		}
		else if (wOri == ori_land){
			if ((NupValue == nup_2) ||
				(NupValue == nup_6) ||
				(NupValue == nup_8)) {
				SetControlState(hDlg, IDC_STC_BINEDGE, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_CMB_BINEDGE3, SCUI_STATE_OFF);
			}
			else {
				SetControlState(hDlg, IDC_STC_BINEDGE, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_CMB_BINEDGE3, SCUI_STATE_ON);
			}
		}
		//<E>Modified to fix #3014,20201130,SSDI:Chanchal Singl
	}
	else
	{
		m_ControlState.cDuplex_I = SCUI_STATE_OFF;
		m_ControlState.cCmbDuplex = SCUI_STATE_ON;
		if ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "LABEL")
			|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "TRANSPARENCY1")
			|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "ENVELOPE")
			|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "JAPANESEPOSTCARD"))
			&& (m_PropertySheetState.wStaple != 0))
		{
			m_ControlState.cDuplex_I = SCUI_STATE_ON;
			m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
		}
		SetControlState(hDlg, IDC_STC_OUTPUTSIZE, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_OUTPUTSIZE, SCUI_STATE_OFF);
//<S><A>Added contraint for paper size dimention,2020.05.14 - SSDI:Goutham
		SetControlState(hDlg, IDC_STC_OUTPAPER, SCUI_STATE_OFF);
//<S><E>Added contraint for paper size dimention,2020.05.14 - SSDI:Goutham
		SetControlState(hDlg, IDC_STC_BINEDGE, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_BINEDGE3, SCUI_STATE_OFF);
	}

	SetControlState(hDlg, IDC_STC_DUPLEX, m_ControlState.cCmbDuplex);
	SetControlState(hDlg, IDC_CMB_DUPLEX, m_ControlState.cCmbDuplex);
	SetControlState(hDlg, IDC_BTN_DUPLEX_I, m_ControlState.cDuplex_I);
}
//=============================================================================
// function
//       ShowConstraintsOnDuplex
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints On Duplex
//=============================================================================
VOID CLayoutTabPropertyPage::ShowConstraintsOnDuplex(HWND hDlg, INT iCtrlID)
{
	long wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	LONG wOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	INT wPaperSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;

	if (0 != SendDlgItemMessage(hDlg, iCtrlID, CB_GETCURSEL, 0, 0))
	{
		m_ControlState.cPamph_I = SCUI_STATE_ON;
		m_ControlState.cCmbPamph = SCUI_STATE_SHOW;
		SetControlState(hDlg, IDC_STC_PAMPHLET, m_ControlState.cCmbPamph);
		SetControlState(hDlg, IDC_CMB_PAMPHLET, m_ControlState.cCmbPamph);
		SetControlState(hDlg, IDC_BTN_PAMPHLET_I, m_ControlState.cPamph_I);
		ShowConstraintsOnDuplexVsOutputSize(hDlg);	//Fix for 5521 issue, Harika, 20220902
	}
	else
	{
		//<S><A>To Modified the Support of Booklet Standard Output Size ,2020-09-02,SSDI:Chanchal Singla
		if (wOutputSize == OUTPUTPAPERSIZE_A3 || wOutputSize == OUTPUTPAPERSIZE_A4 || wOutputSize == OUTPUTPAPERSIZE_B4
			|| wOutputSize == OUTPUTPAPERSIZE_TABLOID || wOutputSize == OUTPUTPAPERSIZE_LETTER || wOutputSize == OUTPUTPAPERSIZE_LEGAL
			|| (wOutputSize == OUTPUTPAPERSIZE_SAOS && (wPaperSize == PAPERSIZE_A3 || wPaperSize == PAPERSIZE_A4 || wPaperSize == PAPERSIZE_B4
			|| wPaperSize == PAPERSIZE_TABLOID || wPaperSize == PAPERSIZE_LETTER || wPaperSize == PAPERSIZE_LEGAL)))
		{
			if (!strcmp("Inch_Point_4", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT].pszOption)
				|| !strcmp("Inch_Point_8", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT].pszOption)
				|| !strcmp("Inch_One_Point_2", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT].pszOption)
				|| !strcmp("STAPLE_1", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption)
				|| !strcmp("STAPLE_2", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption)
				|| !strcmp("STAPLE_STAPLELESS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption)
				|| !strcmp("2HOLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption)
				|| !strcmp("3HOLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption)
				|| !strcmp("4HOLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption)
				|| !strcmp("4HOLE_WIDE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption)
				|| !strcmp("ON", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption))
			{
				m_ControlState.cPamph_I = SCUI_STATE_ON;
				m_ControlState.cCmbPamph = SCUI_STATE_SHOW;
				SetControlState(hDlg, IDC_STC_PAMPHLET, m_ControlState.cCmbPamph);
				SetControlState(hDlg, IDC_CMB_PAMPHLET, m_ControlState.cCmbPamph);
				SetControlState(hDlg, IDC_BTN_PAMPHLET_I, m_ControlState.cPamph_I);
				if (!strcmp("LABEL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)
					|| !strcmp("ENVELOPE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)
					|| !strcmp("TRANSPARENCY1", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption))
				{
					m_ControlState.cDuplex_I = SCUI_STATE_ON;
					m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
					SetControlState(hDlg, IDC_STC_DUPLEX, m_ControlState.cCmbDuplex);
					SetControlState(hDlg, IDC_CMB_DUPLEX, m_ControlState.cCmbDuplex);
					SetControlState(hDlg, IDC_BTN_DUPLEX_I, m_ControlState.cDuplex_I);
				}
			}
			else
			{
				if (!strcmp("OFF", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLLATE].pszOption))
				{
					m_ControlState.cPamph_I = SCUI_STATE_ON;
					m_ControlState.cCmbPamph = SCUI_STATE_SHOW;
				}
				else
				{
					m_ControlState.cPamph_I = SCUI_STATE_OFF;
					m_ControlState.cCmbPamph = SCUI_STATE_ON;
				}
				SetControlState(hDlg, IDC_STC_PAMPHLET, m_ControlState.cCmbPamph);
				SetControlState(hDlg, IDC_CMB_PAMPHLET, m_ControlState.cCmbPamph);
				SetControlState(hDlg, IDC_BTN_PAMPHLET_I, m_ControlState.cPamph_I);
				ShowConstraintsOnDuplexVsOutputSize(hDlg);
			}
		}
		else
		{
			m_ControlState.cPamph_I = SCUI_STATE_ON;
			m_ControlState.cCmbPamph = SCUI_STATE_SHOW;
			SetControlState(hDlg, IDC_STC_PAMPHLET, m_ControlState.cCmbPamph);
			SetControlState(hDlg, IDC_CMB_PAMPHLET, m_ControlState.cCmbPamph);
			SetControlState(hDlg, IDC_BTN_PAMPHLET_I, m_ControlState.cPamph_I);
			ShowConstraintsOnDuplexVsOutputSize(hDlg);
		}
		//<E>To Modified the Support of Booklet Standard Output Size ,2020-09-02,SSDI:Chanchal Singla
	}
}
//=============================================================================
// function
//       ShowConstraintsOnDuplexVsOutputSize
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints On Duplex
//=============================================================================
VOID CLayoutTabPropertyPage::ShowConstraintsOnDuplexVsOutputSize(HWND hDlg, INT iCtrlID)
{
	long wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	LONG wOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	LONG wPaperSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	if (eFirstOption == wDuplex)
	{
		if (wOutputSize >= 9 || ((wPaperSize >= 8) && (wOutputSize == 0)))
		{
			m_ControlState.cDuplex_I = SCUI_STATE_ON;
			m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
			if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
			{
				m_ControlState.cPamph_I = SCUI_STATE_OFF;
				m_ControlState.cDuplex_I = SCUI_STATE_ON;
				m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
				SetControlState(hDlg, IDC_STC_PAMPHLET, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_CMB_PAMPHLET, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_BTN_PAMPHLET_I, m_ControlState.cPamph_I);
			}
			else
			{
				m_ControlState.cPamph_I = SCUI_STATE_ON;
				SetControlState(hDlg, IDC_STC_PAMPHLET, SCUI_STATE_SHOW);
				SetControlState(hDlg, IDC_CMB_PAMPHLET, SCUI_STATE_SHOW);
				SetControlState(hDlg, IDC_BTN_PAMPHLET_I, m_ControlState.cPamph_I);
				if ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption, "CUSTOMSIZE") && (wOutputSize == 0))
					|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "CUSTOMSIZE"))
				{
					m_ControlState.cDuplex_I = SCUI_STATE_OFF;
					m_ControlState.cCmbDuplex = SCUI_STATE_ON;
				}

				if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "JAPANESEPOSTCARD")
					&& !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "CUSTOMSIZE"))
				{
					m_ControlState.cDuplex_I = SCUI_STATE_OFF;
					m_ControlState.cCmbDuplex = SCUI_STATE_ON;
				}
				else if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "JAPANESEPOSTCARD")
					&& !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption, "CUSTOMSIZE")
					&& (wOutputSize == 0))
				{
					m_ControlState.cDuplex_I = SCUI_STATE_OFF;
					m_ControlState.cCmbDuplex = SCUI_STATE_ON;
				}
				//<S><A> To fix feedback issus 66 and 68, Harika, 20220830
				if (((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption, "CUSTOMSIZE")) && (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "SAOS")) && ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "LABEL")) || (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "ENVELOPE")) || (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "TRANSPARENCY1"))))
					|| ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "CUSTOMSIZE")) && ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "LABEL")) || (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "ENVELOPE")) || (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "TRANSPARENCY1")))))
				{
					m_ControlState.cDuplex_I = SCUI_STATE_ON;
					m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
				}
				//<E> To fix feedback issus 66 and 68, Harika, 20220830
			}
		}
		else
		{
			if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
			{
				m_ControlState.cDuplex_I = SCUI_STATE_ON;
				m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
				m_ControlState.cPamph_I = SCUI_STATE_OFF;
				SetControlState(hDlg, IDC_STC_PAMPHLET, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_CMB_PAMPHLET, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_BTN_PAMPHLET_I, m_ControlState.cPamph_I);
			}
			else
			{
				m_ControlState.cDuplex_I = SCUI_STATE_OFF;
				m_ControlState.cCmbDuplex = SCUI_STATE_ON;
				if ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "LABEL")
					|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "TRANSPARENCY1"))
					&& (wBooklet == 0)
					&& (wDuplex == 0))
				{
					m_ControlState.cDuplex_I = SCUI_STATE_ON;
					m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
					m_ControlState.cPamph_I = SCUI_STATE_ON;
					SetControlState(hDlg, IDC_STC_PAMPHLET, SCUI_STATE_SHOW);
					SetControlState(hDlg, IDC_CMB_PAMPHLET, SCUI_STATE_SHOW);
					SetControlState(hDlg, IDC_BTN_PAMPHLET_I, m_ControlState.cPamph_I);
				}
			}
			
		}
		SetControlState(hDlg, IDC_STC_DUPLEX, m_ControlState.cCmbDuplex);
		SetControlState(hDlg, IDC_CMB_DUPLEX, m_ControlState.cCmbDuplex);
		SetControlState(hDlg, IDC_BTN_DUPLEX_I, m_ControlState.cDuplex_I);
	}
	//<S><A>Fix for 5521 issue, Harika, 20221013
	else
	{
		if (((wPaperSize >= 8) && (wOutputSize == 0)) && (wBooklet == 0) && (wPaperSize < 20))
		{
			{
				m_ControlState.cDuplex_I = SCUI_STATE_ON;
				m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
				SetControlState(hDlg, IDC_STC_DUPLEX, m_ControlState.cCmbDuplex);
				SetControlState(hDlg, IDC_CMB_DUPLEX, m_ControlState.cCmbDuplex);
				SetControlState(hDlg, IDC_BTN_DUPLEX_I, m_ControlState.cDuplex_I);
				SendDlgItemMessage(hDlg, IDC_CMB_DUPLEX, CB_SETCURSEL, 0, 0);
			}
		}
	}
	//<E>Fix for 5521 issue, Harika, 20221013
}
//=============================================================================
// function
//       ShowConstraintsOnFactoryDefaults
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints On FactoryDefaults
//=============================================================================
VOID CLayoutTabPropertyPage::ShowConstraintsOnFactoryDefaults(HWND hDlg, INT iCtrlID)
{
	if (0 == SendDlgItemMessage(hDlg, iCtrlID, CB_GETCURSEL, 0, 0))
	{
		
		SetControlState(hDlg, IDC_BTN_SHARED_US_SAVE, SCUI_STATE_SHOW);
	}
	else
	{
		
		SetControlState(hDlg, IDC_BTN_SHARED_US_SAVE, SCUI_STATE_ON);
	}
}
//<S><A> To Add Support of Print Position Dialog, 20-05-2020, SSDI:Chanchal Parkash
//=============================================================================
// function
//      OnPrintPositionDialog(HWND hDlg)
//
// return value
//    Bool
//
// outline
//   Handles Printing Position dialog events  .
//=============================================================================
BOOL CLayoutTabPropertyPage::OnPrintPositionDialog(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	CUIControl		pControl;
	BOOL			bIsUIUpdated = FALSE;
	INT_PTR			DlgRet = IDCANCEL;
	
	OEMDEV pOemPrivateDevMode = {};
	memcpy_s(&pOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));
	
	// Update PropState is required before initializing the PrintPosition Dialoge
//	UpdatePropState(m_pOemPrivateDevMode, &m_PropertySheetState, m_pPrinterName); 
	PROPSTATE pPropertySheetState;
	SecureZeroMemory(&pPropertySheetState, sizeof(PROPSTATE));
	memcpy_s(&pPropertySheetState, sizeof(PROPSTATE), &m_PropertySheetState, sizeof(PROPSTATE));

	CShPrintPosition *pdlg = new CShPrintPosition(IDD_DLG_PRINTPOSITON, m_hStringResourceHandle, m_hPrinterHandle, m_pPrinterName, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings,
		m_hBitmapResourceHandle, m_pOemPublicDevMode, &pOemPrivateDevMode, &pPropertySheetState, m_ObjCShowMessage);
	
	if (NULL == pdlg)
	{
		slog.putLog("CLayoutTabPropertyPage::OnPrintPositionDlg -> NULL_POINTER(IDD_DLG_PRINTPOSITON)\n");
		return FALSE;
	}
	pdlg->SetPrinterName(m_pPrinterName);
	
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_PRINTPOSITON),
		hDlg, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
	
	if (IDOK == DlgRet)
	{	
		//<S><A>Modified to fix Redmine #bug 2668,2020.09.10 SSDI:Goutham
		/*if (FALSE == pControl.IsEqualBuf(m_pOemPrivateDevMode, &pOemPrivateDevMode, sizeof(OEMDEV)))
		{
			memcpy_s(m_pOemPrivateDevMode, sizeof(OEMDEV), &pOemPrivateDevMode, sizeof(OEMDEV));
			bIsUIUpdated = TRUE;
		}*/

		if (FALSE == pControl.IsEqualBuf(&m_PropertySheetState, &pPropertySheetState, sizeof(PROPSTATE)))
		{
			memcpy_s(&m_PropertySheetState, sizeof(PROPSTATE), &pPropertySheetState, sizeof(PROPSTATE));
			memcpy_s(m_pOemPrivateDevMode, sizeof(OEMDEV), &pOemPrivateDevMode, sizeof(OEMDEV));
			UpdatePropstateToStructure();
			bIsUIUpdated = TRUE;
		}
		//<S><E>Modified to fix Redmine #bug 2668,2020.09.10 SSDI:Goutham
		
		if (!strcmp("NONE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption)
			&& m_pOemPrivateDevMode->scpi.ext.byMgnShift != 0 )
		{
			SetControlState(hDlg, IDC_STC_PAMPHLET, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_CMB_PAMPHLET, SCUI_STATE_SHOW);
		}
		else if(!strcmp("NONE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption)
			&& m_pOemPrivateDevMode->scpi.ext.byMgnShift == 0)
		{
			SetControlState(hDlg, IDC_STC_PAMPHLET, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_CMB_PAMPHLET, SCUI_STATE_ON);
		}

		if (TRUE == bIsUIUpdated)
		{
			pControl.EnableApplyButton(hDlg);
		}
	}
	//<S><A>To Fix #3078 cancel Issue,Mimic is displayed wrong,14-12-2020,SSDI:Chanchal Singla
	else
	{
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex = m_PropertySheetState.wMargin;
	}
	//<E>To Fix cancel Issue,Mimic is displayed wrong,14-12-2020,SSDI:Chanchal Singla
	delete pdlg;
	pdlg = NULL;
	return TRUE;
}
// Following finction moved in DocumentPropertypage.cpp file
#if 0
//=============================================================================
// function
//       SetMarginShiftDataToPropState
//
// parameters
//      POEMDEV					pPrivateDevmode
//      PPROPSTATE				pPropertySheetState
//      WCHAR					pPrinterName
//
// return value
//       TRUE
//      FALSE
//
// outline
//       Set Margin Shift Structure Data to Prostate
//=============================================================================
BOOL CLayoutTabPropertyPage::SetMarginShiftDataToPropState(POEMDEV pPrivateDevmode,PPROPSTATE  pPropertySheetState, WCHAR FAR *pPrinterName)
{
	BOOL			blRet = FALSE;
	MARGINSHIFTDATA	msd;
	CShRegMS FAR		*pregms = NULL;
	
	if (pPropertySheetState == NULL)
		goto EXIT;

	pregms = new CShRegMS(m_hStringResourceHandle, pPrinterName);
	if (pregms == NULL)
	{
		goto EXIT;
	}
	blRet = (*pregms).ReadMSData(m_pPrinterName, (*pPropertySheetState).wMargin, &msd);
	if (blRet != TRUE)
	{
		(*pregms).resetEx(pPrinterName);
		(*pregms).reset(pPrinterName);
		(*pregms).ReadMSData(pPrinterName, (*pPropertySheetState).wMargin, &msd);
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
	return blRet;
}

//=============================================================================
// function
//       SetPPosDataToPropState
//
// parameters
//      POEMDEV					pPrivateDevmode
//      PPROPSTATE				pPropertySheetState
//      WCHAR					pPrinterName
//
// return value
//       TRUE
//      FALSE
//
// outline
//       Set Print Position Structure Data to Prostate
//=============================================================================
BOOL CLayoutTabPropertyPage::SetPPosDataToPropState(POEMDEV pPrivateDevmode, PPROPSTATE pPropertySheetState, WCHAR FAR *pPrinterName)
{
	BOOL			blRet = FALSE;
	PRINTPOSITION	ppd;
	CShRegPP FAR		*pregpp = NULL;
	pregpp = new CShRegPP(m_hStringResourceHandle, pPrinterName);
	if (pregpp == NULL)
	{
		goto EXIT;
	}
	
	if (pPropertySheetState == NULL)
		goto EXIT;


	memset(&ppd, 0x00, sizeof(PRINTPOSITION));

	blRet = (*pregpp).ReadPPData(pPrinterName, &ppd);
	if (blRet != TRUE)
	{
		(*pregpp).reset(pPrinterName);
	}

	memset(&ppd, 0x00, sizeof(PRINTPOSITION));
	if ((*pPropertySheetState).wPPosChg == bool_false)
	{
		(*pregpp).GetPPDefData(&ppd);
	}
	else
	{
		(*pregpp).ReadPPData(pPrinterName, &ppd);
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
	return blRet;
}
#endif
//=============================================================================
// function
//       UpdatePropState
//
// parameters
//      POEMDEV					pPrivateDevmode
//      PPROPSTATE				pPropertySheetState
//      WCHAR					pPrinterName
//
// return value
//       TRUE
//      FALSE
//
// outline
//       Update Prostate
//=============================================================================
BOOL CLayoutTabPropertyPage::UpdatePropState(POEMDEV pPrivateDevmode, PPROPSTATE pPropertySheetState, WCHAR FAR *pPrinterName)
{
	// --- byMgnShift
	(*pPropertySheetState).wMargin = pPrivateDevmode->scpi.ext.byMgnShift;
	
	// --- DM_PRINTPOSITION_CHG
	(*pPropertySheetState).wPPosChg = pPrivateDevmode->scpi.ext.wFieldEDM & DM_PRINTPOSITION_CHG ? bool_true : bool_false;

	// -- Update Margin Shift Data To Propstate
	SetMarginShiftDataToPropState(pPrivateDevmode, pPropertySheetState, pPrinterName);
	
	// -- Update Print Position Data To Propstate
	SetPPosDataToPropState(pPrivateDevmode, pPropertySheetState, pPrinterName);
	return TRUE;
}

//=============================================================================
// function
//       UpdatePropstateToStructure
//
// parameters
//
// return value
//       TRUE
//      FALSE
//
// outline
//        UpdatePropstateToStructure
//=============================================================================
BOOL CLayoutTabPropertyPage::UpdatePropstateToStructure()
{
	CShRegPP		*pregPP = NULL;
	CShRegMS		*pregMS = NULL;
	BOOL			bRet = FALSE;
	PRINTPOSITION	ppd;
	PRINTPOSITION	ppdDef;
	MARGINSHIFTDATA	msd;

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

	pregPP = new CShRegPP(m_hStringResourceHandle, m_pPrinterName);
	if (pregPP == NULL)
	{
		goto EXIT;
	}
	pregMS = new CShRegMS(m_hStringResourceHandle, m_pPrinterName);
	if (pregMS == NULL)
	{
		goto EXIT;
	}

	//-- Print Position
	SecureZeroMemory(&ppdDef, sizeof(PRINTPOSITION));
	pregPP->GetPPDefData(&ppdDef);
	if ((ppdDef.wOddX != (short)(m_PropertySheetState.lPPosOddX)) ||
		(ppdDef.wOddY != (short)(m_PropertySheetState.lPPosOddY)) ||
		(ppdDef.wEvenX != (short)(m_PropertySheetState.lPPosEvnX)) ||
		(ppdDef.wEvenY != (short)(m_PropertySheetState.lPPosEvnY)) ||
		(ppdDef.dwUnit != (unsigned short)(m_PropertySheetState.wPPosUnit)))
	{
		ppd.wOddX = (short)m_PropertySheetState.lPPosOddX;
		ppd.wOddY = (short)m_PropertySheetState.lPPosOddY;
		ppd.wEvenX = (short)m_PropertySheetState.lPPosEvnX;
		ppd.wEvenY = (short)m_PropertySheetState.lPPosEvnY;
		ppd.dwUnit = (short)m_PropertySheetState.wPPosUnit;
		pregPP->WritePPData(m_pPrinterName, &ppd);
		pregPP->WritePPCount(m_pPrinterName, 1);

		m_PropertySheetState.wPPosChg = bool_true;

		// --- DM_PRINTPOSITION_CHG
		m_pOemPrivateDevMode->scpi.ext.wFieldEDM = (m_PropertySheetState.wPPosChg == bool_true) ?
			(m_pOemPrivateDevMode->scpi.ext.wFieldEDM | DM_PRINTPOSITION_CHG) : (m_pOemPrivateDevMode->scpi.ext.wFieldEDM & ~DM_PRINTPOSITION_CHG);
		//	pregPP->PPOS_GetPrintPositionData(ppposd);
	}
	else
	{
		m_PropertySheetState.wPPosChg = bool_false;
		// --- DM_PRINTPOSITION_CHG
		m_pOemPrivateDevMode->scpi.ext.wFieldEDM = (m_PropertySheetState.wPPosChg == bool_true) ?
			(m_pOemPrivateDevMode->scpi.ext.wFieldEDM | DM_PRINTPOSITION_CHG) : (m_pOemPrivateDevMode->scpi.ext.wFieldEDM & ~DM_PRINTPOSITION_CHG);

	}

	//-- Margin Shift
	{
		if ((*m_pmcf).IsWriteToJson() == TRUE)
			pjsonms->ReadMSData(m_PropertySheetState.wMargin, &msd);
		else
			pregMS->ReadMSData(m_pPrinterName, m_PropertySheetState.wMargin, &msd);

		if (m_PropertySheetState.wMarginUnit == unit_inch)
		{
			msd.dwInch = m_PropertySheetState.dwMarginWidth;
			msd.dwMilli = InchToMilli_MrgnSft(m_PropertySheetState.dwMarginWidth);
			msd.dwPixel = InchToPixel_MrgnSft(m_PropertySheetState.dwMarginWidth);
		}
		else
		{

			msd.dwMilli = m_PropertySheetState.dwMarginWidth;
			msd.dwInch = MilliToInch_MrgnSft(m_PropertySheetState.dwMarginWidth);
			msd.dwPixel = MilliToPixel_MrgnSft(m_PropertySheetState.dwMarginWidth);
		}
		msd.dwUnit = (DWORD)(m_PropertySheetState.wMarginUnit);

		if ((*m_pmcf).IsWriteToJson() == TRUE)
			pjsonms->WriteMSData(m_PropertySheetState.wMargin, &msd);
		else
			pregMS->WriteMSData(m_pPrinterName, m_PropertySheetState.wMargin, &msd);
	}

	bRet = TRUE;

EXIT:
	if (pregPP != NULL)
	{
		delete pregPP;
		pregPP = NULL;
	}
	if (pregMS != NULL)
	{
		delete pregMS;
		pregMS = NULL;
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
	return bRet;
}

//=============================================================================
// function
//       PrintPositionSetTabDefaults
//
// parameters
//
// return value
//       TRUE
//      FALSE
//
// outline
//        PrintPosition SetTabDefaults
//=============================================================================
HRESULT  CLayoutTabPropertyPage::PrintPositionSetTabDefaults(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CShPrintPosition *pdlgPP = new CShPrintPosition(IDD_DLG_PRINTPOSITON, m_hStringResourceHandle, m_hPrinterHandle, m_pPrinterName, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings,
		m_hBitmapResourceHandle, m_pOemPublicDevMode, m_pOemPrivateDevMode, &m_PropertySheetState, m_ObjCShowMessage);

	if (NULL == pdlgPP)
	{
		slog.putLog("CLayoutTabPropertyPage::PrintPositionSetTabDefaults -> NULL_POINTER(CShPrintPosition)\n");
		return FALSE;
	}
	pdlgPP->CreateControlClass();
	pdlgPP->SetTabDefaults(hDlg);
    //<S><C>To fix feedback issue 21(sprint 16),2020.08.13 -SSDI:Goutham
	pdlgPP->GetDefPrintPosition(hDlg);
	//m_PropertySheetState.wPPosChg = bool_false;
    //<S><E>To fix feedback issue 20(sprint 16),2020.08.13 -SSDI:Goutham
	UpdatePropstateToStructure();
	if (pdlgPP != NULL)
	{
		delete pdlgPP;
		pdlgPP = NULL;
	}
	return S_OK;
}
//<E> To Add Support of Print Position Dialog, 20-05-2020, SSDI:Chanchal Parkash
//<S><A>To Add the Support of Booklet Standard Output Size and fix SSDI #Bug 2067,2020-09-02,SSDI:Chanchal Singla
//=============================================================================
// function
//       ShowBookletPaperSizeDimensions
//
// parameters
//			HWND  hDlg			Handler to window
//			INT   iCtrlID		Control ID
// return value
//      VOID
//
// outline
//     Show Booklet PaperSize Dimensions
//=============================================================================
VOID CLayoutTabPropertyPage::ShowBookletPaperSizeDimensions(HWND hDlg, INT iCtrlID)
{
	UIControlMap::const_iterator Iterator = m_UIControls.find(iCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pCombo = static_cast<CShImageCombo*>(Iterator->second);
		INT Windex = pCombo->GetSelectedIndex();
		INT index = 0;

		switch (Windex)
		{
		case BOOKLET_A3_INDEX:
			index = Booklet_A3_Array;
			break;
		case BOOKLET_A4_INDEX:
			index = Booklet_A4_Array;
			break;
		case BOOKLET_B4_INDEX:
			index = Booklet_B4_Array;
			break;
		case BOOKLET_LEDGER_INDEX:
			index = Booklet_Ledger_Array;
			break;
		case BOOKLET_LETTER_INDEX:
			index = Booklet_Letter_Array;
			break;
		case BOOKLET_LEGAL_INDEX:
			index = Booklet_Legal_Array;
			break;
		default:
			break;
		}
		WCHAR cSep = GetDecimalSeparator();

		WCHAR			szText[256] = {};
		WCHAR			sztemp[256] = {};
		WCHAR			szw[32] = {};
		WCHAR			szl[32] = {};
		WCHAR			szu[32] = {};

		int PaperSize = PaperSizeArray[index][0];
		int Width = PaperSizeArray[index][1];
		int Length = PaperSizeArray[index][2];
		int unit = PaperSizeArray[index][3];

		switch (unit)
		{
		case MM_LOENGLISH:
		{
			WCHAR szForm[MAX_FORMNAME_TEXT] = {};
			wcscpy_s(szForm, L"%d%c%02d");
			StringCbPrintf(szw, sizeof(szw), szForm, Width / 100, cSep, Width % 100);
			StringCbPrintf(szl, sizeof(szl), szForm, Length / 100, cSep, Length % 100);
			LoadString(m_hStringResourceHandle, IDS_UNIT_INCH, szu, countof(szu));
		};
		break;
		case MM_LOMETRIC:
		default:
		{
			WCHAR szForm[MAX_FORMNAME_TEXT] = {};
			if (DMPAPER_CUSTOM == PaperSize)
			{
				wcscpy_s(szForm, L"%d%c%d");
			}
			else
			{
				wcscpy_s(szForm, L"%d");
			}
			StringCbPrintf(szw, sizeof(szw), szForm, Width / 10, cSep, Width % 10);
			StringCbPrintf(szl, sizeof(szl), szForm, Length / 10, cSep, Length % 10);
			LoadString(m_hStringResourceHandle, IDS_UNIT_MM, szu, countof(szu));
		};

		break;
		}
		if (iCtrlID == IDC_CMB_PAPER)
		{
			LoadString(m_hStringResourceHandle, IDC_STC_PAPER, szText, countof(szText));
			StringCbPrintf(sztemp, sizeof(sztemp), szText, szw, szl, szu);
			SetDlgItemText(hDlg, IDC_STC_PAPER, sztemp);
		}
		if (iCtrlID == IDC_CMB_OUTPUTSIZE)
		{
			LoadString(m_hStringResourceHandle, IDS_IDC_STC_PAPER, szText, countof(szText));
			StringCbPrintf(sztemp, sizeof(sztemp), szText, szw, szl, szu);
			SetDlgItemText(hDlg, IDC_STC_OUTPAPER, sztemp);
		}
		SecureZeroMemory(szText, sizeof(szText));
		SecureZeroMemory(szl, sizeof(szl));
		SecureZeroMemory(sztemp, sizeof(sztemp));
	}
}
//<E>To Add the Support of Booklet Standard Output Size and fix SSDI #Bug 2067,2020-09-02,SSDI:Chanchal Singla
//<S><A> To Add Support of Water Mark Dialog, 04-09-2020, SSDI:Chanchal Parkash
// ============================================================================
// function
//      OnWaterMarkDialog(HWND hDlg)
//
// return value
//    Bool
//
// outline
//   Handles WaterMark dialog events  .
//=============================================================================
BOOL CLayoutTabPropertyPage::OnWaterMarkDialog(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	CUIControl		pControl;
	BOOL			bIsUIUpdated = FALSE;
	INT_PTR			DlgRet = IDCANCEL;

	OEMDEV pOemPrivateDevMode = {};
	memcpy_s(&pOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));

	CWaterMarkDialog *pdlg = new CWaterMarkDialog(IDD_DLG_WM, m_hStringResourceHandle, m_hPrinterHandle, m_hDialogResourceHandle, m_pPrinterName,
		m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings, m_hBitmapResourceHandle, m_pOemPublicDevMode,
		&pOemPrivateDevMode, m_PaperInfo);

	if (NULL == pdlg)
	{
		slog.putLog("CLayoutTabPropertyPage::OnPrintPositionDlg -> NULL_POINTER(IDD_DLG_PRINTPOSITON)\n");
		return FALSE;
	}
	//	<S><C>To Fix Redmine Bug #4511,2021.December.06,SSDI: Chanchal Singla
	//Watermark drop down displays empty in Client machine
	//pdlg->SetPrinterName(m_pOemPublicDevMode->dmDeviceName);
	pdlg->SetPrinterName(m_pPrinterName);
	//	<E>To Fix Redmine Bug #4511,2021.December.06,SSDI: Chanchal Singla
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_WM),
		hDlg, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);

	if (IDOK == DlgRet)
	{
		if (FALSE == pControl.IsEqualBuf(m_pOemPrivateDevMode, &pOemPrivateDevMode, sizeof(OEMDEV)))
		{
			memcpy_s(m_pOemPrivateDevMode, sizeof(OEMDEV), &pOemPrivateDevMode, sizeof(OEMDEV));
			*m_pWaterMarkIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
			pControl.EnableApplyButton(hDlg);
		}
	}
	delete pdlg;
	pdlg = NULL;
	return TRUE;
}

//=============================================================================
// function
//       WaterMarkSetTabDefaults
//
// parameters
//
// return value
//       TRUE
//      FALSE
//
// outline
//        WaterMark SetTabDefaults
//=============================================================================
HRESULT  CLayoutTabPropertyPage::WaterMarkSetTabDefaults(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");


	CWaterMarkDialog *pdlgWM = new CWaterMarkDialog(IDD_DLG_WM, m_hStringResourceHandle, m_hPrinterHandle, m_hDialogResourceHandle, m_pPrinterName,
		m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings, m_hBitmapResourceHandle, m_pOemPublicDevMode,
		m_pOemPrivateDevMode, m_PaperInfo);

	if (NULL == pdlgWM)
	{
		slog.putLog("CLayoutTabPropertyPage::OnPrintPositionDlg -> NULL_POINTER(IDD_DLG_PRINTPOSITON)\n");
		return FALSE;
	}
	pdlgWM->CreateControlClass();
	pdlgWM->SetTabDefaults(0,hDlg);
	//<S><A>To Fix #3084,20201216,SSDI:Chanchal Singla
	//Watermark mimic is displaying ,when we click Defaults button.
	*m_pWaterMarkIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
	//<E>To Fix #3084,20201216,SSDI:Chanchal Singla
	if (pdlgWM != NULL)
	{
		delete pdlgWM;
		pdlgWM = NULL;
	}
	return S_OK;
}
//<E> To Add Support of Water Mark Dialog, 04-09-2020, SSDI:Chanchal Parkash
//=============================================================================
// function
//       DuplexFeatureInitialization
//
// parameters
//			HWND  hDlg			Handler to window
//			INT   iCtrlID		Control ID
// return value
//      VOID
//
// outline
//       Duplex  Feature Initialization
//=============================================================================
VOID CLayoutTabPropertyPage::DuplexFeatureInitialization(HWND hDlg, INT iCtrlID)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CUIControl* pControl;
	std::string strFeatureName;
	PRESOURCEIDINFO pResourceIDInfo = NULL;

	long wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	LONG wOutputSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	LONG wPaperSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	if ((eFirstOption == wDuplex) && (wOutputSize >= 9 || ((wPaperSize >= 8) && (wOutputSize == 0))))
	{
		//<S><C>To Fix #3398 ,2021-03-11, SSDI: Chanchal Singla
		//2-sided printing does not reset to default value from Layout tab
		if ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption, "CUSTOMSIZE") && (wOutputSize == 0))
			|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "CUSTOMSIZE"))
		{
			m_DefaultSettings[FEATURE_DUPLEX].ResourceID = IDS_IDC_CMB_DUPLEX_LE;
		}
		else
		{
			m_DefaultSettings[FEATURE_DUPLEX].ResourceID = IDS_IDC_CMB_DUPLEX_1S;
		}
		//<E>To Fix #3398 ,2021-03-11, SSDI: Chanchal Singla
	}
	else
	{
		//<S><C>To Fix Feedback 23,2021-02-24,SSDI:Chanchal Singla
		if (m_DefaultSettings[FEATURE_DUPLEX].iDevmodeValue == DMDUP_SIMPLEX)
		{
			m_DefaultSettings[FEATURE_DUPLEX].ResourceID = IDS_IDC_CMB_DUPLEX_1S;
		}
		else if (m_DefaultSettings[FEATURE_DUPLEX].iDevmodeValue == DMDUP_VERTICAL)
		{
			m_DefaultSettings[FEATURE_DUPLEX].ResourceID = IDS_IDC_CMB_DUPLEX_LE;
		}
		else
		{
			m_DefaultSettings[FEATURE_DUPLEX].ResourceID = IDS_IDC_CMB_DUPLEX_SE;
		}
		//<E>To Fix Feedback 23,2021-02-24,SSDI:Chanchal Singla
	}
	//<S><A> Deleted the existing control before creating new Control. Harika, 20230405
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_DUPLEX);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pComboDuplex = static_cast<CShImageCombo*>(Iterator->second);
		if (pComboDuplex)
		{
			m_UIControls.erase(Iterator);	//Control will be deleted
			delete pComboDuplex;
			pComboDuplex = NULL;
		}
	}
	//<E> Deleted the existing control before creating new Control. Harika, 20230405
	pResourceIDInfo = GetResourceIDInfoOfControl(DUPLEX);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_DUPLEX],
		m_ObjCShowMessage,
		IDC_CMB_DUPLEX
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_DUPLEX, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	pControl->OnInit(IDC_CMB_DUPLEX, hDlg);
	((CShImageCombo*)pControl)->Select(IDC_CMB_DUPLEX, hDlg, wDuplex);
	((CShImageCombo*)pControl)->SelectFeatureOption(IDC_CMB_DUPLEX, hDlg);
}