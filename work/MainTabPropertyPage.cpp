//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : MainTabPropertyPage.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 09 October 2013
//
//	Description   : This class handles Property sheet dialog functions  for Main tab
//===========================================================================
#include "precomp.h"
#include "define.h"
#include "RegistryAPI.h"
#include "shJson.h"
#include "shJsonus.h"
#include "shJsonwm.h"
#include "shjsonups.h"
#include "shjsonjc.h"
//For Logging Purpose
#include "MFPLogger.h"
#include "aes.h"
//#include "sh_cmb_userset.h"
#define MAX_ENCODING_STRING_LENGTH 16
//=============================================================================
// function
//      CMainTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Constructor to the class CMainTabPropertyPage.
//=============================================================================
CMainTabPropertyPage::CMainTabPropertyPage(HMODULE hStringResourceHandle,HMODULE hBitmapResourceHandle,HMODULE hDialogResourceHandle,
	IPrintCoreHelperUni* pPrintCoreHelperUni,ResourceIDInfoMap* pResourceIDInfoMap,ConstraintFeatureInfoMap &ConstraintFeatureInfoMap,
	PFEATUREINFOLIST &pFeatureInfoList,PaperInfoMap PaperInfo, CustomPaperInfoMap CustomPaperInfoList,POEMDEV pOemPrivateDevMode,
    PDEVMODE pOemPublicDevMode,LPTSTR pHelpFilePath,DWORD *pHelpID,std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings,POEMDEV pPrivateDevmode,HANDLE hPrinter,
        WCHAR* pPrinterName,SHORT& sWaterMarkIndex,  SHORT& sUsIndex, PPROPINFO ppi):CDocumentPropertyPage(hStringResourceHandle,pHelpFilePath,pHelpID,hPrinter,pPrinterName,hBitmapResourceHandle,hDialogResourceHandle)

{
	m_pPrintCoreHelperUni = pPrintCoreHelperUni;
	m_pResourceIDInfoMap = pResourceIDInfoMap ;
	m_ConstraintFeatureInfoMap = &ConstraintFeatureInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	m_PaperInfo = PaperInfo;
	m_CustomPaperInfoList =  CustomPaperInfoList ;
	m_pOemPrivateDevMode = pOemPrivateDevMode ;
	m_pOemPublicDevMode = pOemPublicDevMode ;
	//m_hDialogResourceHandle = hDialogResourceHandle;
    m_DefaultSettings = pDefaultSettings;
	m_wID			= IDD_TAB_MAIN_121_03;
    m_pPrivateDevmode  = pPrivateDevmode;
    m_eColorModeValue = eAutoMode;
	m_pWaterMarkIndex = &sWaterMarkIndex ;
	m_pUsIndex = &sUsIndex;
	m_ppi = ppi;
}

//=============================================================================
// function
//      CMainTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Destructor to the class CMainTabPropertyPage.
//=============================================================================
CMainTabPropertyPage::~CMainTabPropertyPage(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(NULL == m_ObjCShowMessage)
	{
		slog.putLog("CMainTabPropertyPage::~CMainTabPropertyPage -> NULL_OBJECT(m_ObjCShowMessage)\n");
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
CMainTabPropertyPage::InitDlgBox(
    _Out_ LPCTSTR* ppszTemplate,
    _Out_ LPCTSTR* ppszTitle
    )
{
    HRESULT hr = S_OK;
	
	//Coverity Fix 90557 & 90442 - SSDI:Seetharam - 20200908
	//Changed the condition from || to && because if any parameter is NULL, then
	//dereference happens and application can crash
    //if ((NULL != ppszTemplate) ||
    if ((NULL != ppszTemplate) &&
        (NULL != ppszTitle))
    {
        *ppszTemplate = MAKEINTRESOURCE(m_wID);
		if (LoadString(m_hStringResourceHandle, m_wID, m_szItem, countof(m_szItem)) > 0)
		{
			*ppszTitle    = m_szItem;
		}
    }

    return hr;
}

//=============================================================================
// function
//      InitializeUIControls
//
// return value
//     NILL
//
// outline
//      Initializes all controls.
//=============================================================================
HRESULT
CMainTabPropertyPage::InitializeUIControls(
    _In_ CONST HWND   hDlg
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	m_hPropertySheetHandle = hDlg;

	CShJson*	json = new CShJson(ghInstance, m_pPrinterName);
	CShJsonWm*	jsonwm = new CShJsonWm(ghInstance, m_pPrinterName, m_hStringResourceHandle);
	json->Init();
	jsonwm->Init();
	CreateMessageClass(hDlg);
	
	if (!IsDlgButtonChecked(hDlg, IDC_CHK_PIN))
	{
		ClearEditBoxContent(IDC_EDT_PIN, hDlg, m_PropertySheetState.szRetentionPassword, sizeof(m_PropertySheetState.szRetentionPassword));
		SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_SHOW);
	}
	else
	{
		SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_ON);
	}
	//ShowConstraintsOnRetention(hDlg, IDC_CMB_RETENTION);
	short devicetype = eColor;
    //<S><A> RedMine Ticket(#971) Black and White check box is displayed for Mono Model when printer is renamed: 2013.11.19, SSDI:Sonali
	// When printer is renamed with maximum characters, the printer name is truncated.
    //Hence added the below code to retain the printer name
    DWORD			dwType = 0;
	DWORD			dwNeeded = 0;
	DWORD			dwVal = 0;
    LRESULT res = GetPrinterDataEx(m_hPrinterHandle, REG_KEY_HKLM_DEVICEINDEX, REG_VAL_HKLM_DI_DEVICETYPE, &dwType, 
							  (LPBYTE)&dwVal, sizeof(dwVal), &dwNeeded);
    if(ERROR_SUCCESS == res)
    {
        devicetype  = (short)dwVal;
    }
	else
	{
		slog.putLog("CMainTabPropertyPage::InitializeUIControls -> ERROR_GETPRINTERDATAEX(GetPrinterDataEx)\n");
	}
	//<E> RedMine Ticket(#971) Black and White check box is displayed for Mono Model when printer is renamed: 2013.11.19, SSDI:Sonali

    if(eMono == devicetype)
	{
        CheckDlgButton(hDlg,IDC_CHECK_BLACKANDWHITE_PRINT,BST_CHECKED);
		HideCtrl(hDlg,IDC_CHECK_BLACKANDWHITE_PRINT);
	}
//<S><A> Bug 2029 - Issue1 - 20200629 - SSDI:Seetharam
	JobControlDataToPropState();
//<S><E> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
	//<S><A> To Added Support  of Custom Paper, 24-07-2020,SSDI:Chanchal Singla
	{
		CShIniFile			*m_pmcf = NULL;
		TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
		GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
		m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
		CShJsonUserPSize	*pjsonups = NULL;
		if ((*m_pmcf).IsWriteToJson() == TRUE)
		{
			pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
			pjsonups->Init();
		}

		CUPSReg FAR		*pregUps = NULL;
		pregUps = new CUPSReg(m_hStringResourceHandle, m_pPrinterName);
		USERPAPERSIZEDATA	upsd;
		SecureZeroMemory(&upsd, sizeof(upsd));
		//Coverity Fix 90576 - SSDI:Seetharam - 20200908
		//moved the reset method call inside the NULL check condition so that 
		//nod dereferences occur
		//(*pregUps).reset(m_pPrinterName);
		if (pregUps != NULL)
		{
			if ((*m_pmcf).IsWriteToJson() == TRUE)
				(*pjsonups).reset();
			else
				(*pregUps).reset(m_pPrinterName);
			//<S><A>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
			///Custom Paper values are changing to Default even though cancel button is clicked
			//<S><C>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
			//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
			
			//(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM, &upsd);
			upsd.dwWidth = m_PropertySheetState.dwCustomWidth;
			upsd.dwLength = m_PropertySheetState.dwCustomLength;
			upsd.wUnit = m_PropertySheetState.wCusUnit;
			//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
			memcpy(&m_ppi->PaperSize_upsd, &upsd, sizeof(USERPAPERSIZEDATA));
			if ((*m_pmcf).IsWriteToJson() == TRUE)
			{
				(*pjsonups).ReadUPSData(DMPAPER_CUSTOM_ZOOM, &upsd);
			}
			else
			{
				(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &upsd);
			}
			memcpy(&m_ppi->FitToPaperSize_upsd, &upsd, sizeof(USERPAPERSIZEDATA));
			//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
			delete pregUps;
			pregUps = NULL;

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
	}
	//<E> To Added Support  of Custom Paper, 24-07-2020,SSDI:Chanchal Singla
	ShowConstraintsOnCollate(hDlg);
	 SetPaperConstraints();
	 SetPamphletMessage();
//<S><A>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
	// UpdatePropState(m_pOemPrivateDevMode, &m_PropertySheetState);
//<E>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
	 __super::InitializeUIControls(hDlg);
	 __super::SelectOptionsonUI(hDlg, m_pOemPrivateDevMode);
	 //<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	 DealWithFavItemsInHKLM();
	 //<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S

	 if (json != NULL)
	 {
		 delete json;
		 json = nullptr;
	 }
	 return hr;
}
HRESULT
CMainTabPropertyPage::ActivateUIControl(
    _In_ CONST HWND   hDlg
    )
{
	HRESULT hr = S_OK;	
	//SetPamphletIndex(hDlg);
//<S><A> Bug 2029 - Issue1 - 20200629 - SSDI:Seetharam
	JobControlDataToPropState();
//<S><E> Bug 2029 - Issue1 - 20200629 - SSDI:Seetharam
	 __super::ActivateUIControl(hDlg);
	 ShowFinishingInformationIcon(hDlg);
	 UpdateUserSetting(hDlg, IDC_CMB_SHARED_USRSET);
	//ShowConstraintsOnFactoryDefaults(hDlg, IDC_CMB_SHARED_USRSET);
	ShowConstraintsOnRetention(hDlg, IDC_CMB_RETENTION);	//<S><D> to fix 3620, Harika-20210422
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
void CMainTabPropertyPage::CreateContorlClass(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	CUIControl* pControl;
	std::string strFeatureName; // = m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION].pszFeature;
	PRESOURCEIDINFO pResourceIDInfo = NULL; // GetResourceIDInfoOfControl(strFeatureName);

	//Favorites Combo box
	/*pControl = new CShImageCombo(
		GetResourceIDInfoOfControl(FAVORITES),
		&m_pFeatureInfoList->pFeatureOptionPair[FAVORITES],
		&m_pFeatureInfoList->pFeatureOptionsList[FAVORITES],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FAVORITES],
		m_ObjCShowMessage,
		IDC_CMB_SHARED_USRSET
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_SHARED_USRSET, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCombo)\n");
	}*/

	//Zoom Checkbox
	pResourceIDInfo = GetResourceIDInfoOfControl(ZOOM);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOMSETTINGS],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_ZOOMSETTINGS],
		m_ObjCShowMessage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ZOOM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOMSETTINGS],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_ZOOM_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_ZOOM_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}
	//ZoomButton

	pControl = new CButton(IDC_BTN_ZOOM, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_ZOOM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_BTN_ZOOM)\n");
	}

	//Copies
	pControl = new CShEditSpin(m_hStringResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, IDC_EDT_COPIES, IDC_BTN_COPIES_SP, IDC_BTN_COPIES_SM, m_pOemPublicDevMode, m_pOemPrivateDevMode);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_COPIES, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShEditSpin)\n");
	}


	//Collate
	pResourceIDInfo = GetResourceIDInfoOfControl(COLLATE);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLLATE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_COLLATE],
		m_ObjCShowMessage,
		IDC_CMB_COLLATE
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_COLLATE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_COLLATE_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_COLLATE_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}



	//<S><C>   2020.01.23, SSDI:Goutham
		//Original Size
		//<S><A>To fix SSDI Bug #2089,24.06.2020,SSDI:Chanchal Parkash
		/*Few OS Specific Language has default language as A4 to justify this initialise Default Setting with
		"A4" as default paper */
	if (DMPAPER_A4 == GetPaperSizeDef())
	{
		m_DefaultSettings[FEATURE_PAPERSIZE].ResourceID = IDS_PAPERSIZE_9;
		// first time when install a driver Orignal size drop down Default Should be A4
		if (DMPAPER_A4 == GetPaperSizeDef() && (m_pOemPrivateDevMode->scpi.pub.dmPaperSize == DMPAPER_A4))
		{
			//<S><A> Fix for feedback issue 64, Harika	20230214
			m_pOemPrivateDevMode->scpi.pub.dmPaperLength = 2970;
			m_pOemPrivateDevMode->scpi.pub.dmPaperWidth = 2100;
			m_pOemPrivateDevMode->scpi.ext.nZoomSrcPaperSize = DMPAPER_A4;
			m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = DMPAPER_A4;
			m_pOemPrivateDevMode->scpi.ext.nPamphPaperSize = DMPAPER_A4;
			m_pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel = 4760;
			m_pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel = 6814;
			m_pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = 2 * m_pOemPrivateDevMode->scpi.doc.nLeftMarginPixel + m_pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel;
			m_pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = 2 * m_pOemPrivateDevMode->scpi.doc.nTopMarginPixel + m_pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel;
			//<E> Fix for feedback issue 64, Harika	20230214
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption = "A4";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex = eSecondOption;
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uDefaultOptionIndex = eSecondOption;
		}
	}
	//<E>To fix SSDI Bug #2089,24.06.2020,SSDI:Chanchal Parkash

	pResourceIDInfo = GetResourceIDInfoOfControl(PAPERSIZE);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_PAPERSIZE],
		m_ObjCShowMessage,
		IDC_CMB_PAPER
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_PAPER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_PAPER_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PAPER_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}


	//Output Size
	pResourceIDInfo = GetResourceIDInfoOfControl(FITTOPAPER);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE],
		m_pFeatureInfoList,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_FITTOPAPERSIZE],
		m_ObjCShowMessage,
		IDC_CMB_OUTPUTSIZE,
		m_PropertySheetState
	);
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
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_OUTPUTSIZE_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_OUTPUTSIZE_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}


	//Orientation
	//<S><C> to implement Custom Orientation 13/8/2020 K M Harika
	pResourceIDInfo = GetResourceIDInfoOfControl(CUSTOMORIENTATION);
	//<E> to implement Custom Orientation 13/8/2020 K M Harika
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OREINTATION],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_OREINTATION],
		m_ObjCShowMessage,
		IDC_CMB_ORIENT
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_ORIENT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}



	//Duplex
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		m_DefaultSettings[FEATURE_DUPLEX].ResourceID = IDS_IDC_CMB_DUPLEX_1S;
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
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCombo)\n");
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
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}
	

	//<S><C>   2020.01.23, SSDI:Goutham

// <S><A> To add the custom combo box for N-Up and staple, 20200128 - SSDI:Seetharam
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
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCombo)\n");
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
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}

	//Staple
	pResourceIDInfo = GetResourceIDInfoOfControl(STAPLING);
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING],
		m_pFeatureInfoList,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_STAPLING],
		m_ObjCShowMessage,
		IDC_CMB_STAPLE,
		m_PropertySheetState
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_STAPLE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCombo)\n");
	}
	// <S><E> To add the custom combo box for N-Up and staple, 20200128 - SSDI:Seetharam
	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_STAPLE_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_STAPLE_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}

	//Position - Bindong Edge
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
		IDC_CMB_BINEDGE,
		m_PropertySheetState
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_BINEDGE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCombo)\n");
	}

	//Doc-Filing - Retention	
	pResourceIDInfo = GetResourceIDInfoOfControl(RETENTION_SETTINGS);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_RETENTION_SETTINGS],
		m_ObjCShowMessage,
		IDC_CMB_RETENTION,
		m_PropertySheetState
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_RETENTION, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCombo)\n");
	}


	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_RETENTION_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_RETENTION_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}


	//Doc-Filing PIN Code
	pControl = new CCheckBox(
		GetResourceIDInfoOfControl(RETENTION_PASSWORD),
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_PASSWORD],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_PASSWORD],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_RETENTION_PASSWORD],
		m_ObjCShowMessage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_PIN, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}

	pControl = new CJobControlEdit(m_PropertySheetState.szRetentionPassword, JC_STR_PINLENGTH, eNumberPasswordEditBox);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_PIN, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_EDT_PIN)\n");
	}
	
	//<S><A>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla    
	 // ---Mimic Left Side
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
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CMimicImage)\n");
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
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CMimicImage)\n");
	}
	//<E>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla

	//<S><C> To Fix Redmine #2806,20201127,SSDI:Chanchal Singla
	//Main Tab : Color Mode drop down is displaying Black and White.
	//Default VaLues are Set Based on Constraints between ColorMode/ColorType(Config tab)/AutoColorMode
	if (1 == m_PropertySheetState.wColorOption) // Black and White
	{
		m_DefaultSettings[FEATURE_COLOR_MODE].ResourceID = IDS_IDC_CMB_COLMODE_GRAY;
		m_DefaultSettings[FEATURE_COLOR_MODE].pszDefaultOptionString = L"Mono";
	}
	else if (0 == m_PropertySheetState.wColorOption) // Color
	{
		if (m_PropertySheetState.byAutoColorMode == 1) {
			//<S><A> Fixed SOL2 SQA bug #47180. Harika, 20221011
			if (m_DefaultSettings[FEATURE_COLOR_MODE].iDevmodeValue == colmode_mono)
			{
				m_DefaultSettings[FEATURE_COLOR_MODE].ResourceID = IDS_IDC_CMB_COLMODE_GRAY;
				m_DefaultSettings[FEATURE_COLOR_MODE].pszDefaultOptionString = L"Mono";
			}
			else
			{
			//<E> Fixed SOL2 SQA bug #47180. Harika, 20221011
				m_DefaultSettings[FEATURE_COLOR_MODE].ResourceID = IDS_IDC_CMB_COLMODE_COLOR;
				m_DefaultSettings[FEATURE_COLOR_MODE].pszDefaultOptionString = L"Color";
			}
		}
		else
		{
			//<S><C>To Fix Feedback 23 and Redmine Bug #3432,2021-03-05,SSDI:Chanchal Singla
			//Color Mode is changing to Automatic when we click Defaults.
			if (m_DefaultSettings[FEATURE_COLOR_MODE].iDevmodeValue == colmode_mono)
			{
				m_DefaultSettings[FEATURE_COLOR_MODE].ResourceID = IDS_IDC_CMB_COLMODE_GRAY;
				m_DefaultSettings[FEATURE_COLOR_MODE].pszDefaultOptionString = L"Mono";
			}
			else if (m_DefaultSettings[FEATURE_COLOR_MODE].iDevmodeValue == colmode_color)
			{
				m_DefaultSettings[FEATURE_COLOR_MODE].ResourceID = IDS_IDC_CMB_COLMODE_COLOR;
				m_DefaultSettings[FEATURE_COLOR_MODE].pszDefaultOptionString = L"Color";
			}
			else
			{
				m_DefaultSettings[FEATURE_COLOR_MODE].ResourceID = IDS_IDC_CMB_COLMODE_AUTO;
				m_DefaultSettings[FEATURE_COLOR_MODE].pszDefaultOptionString = L"Auto";
			}
			//<E>To Fix Feedback 23 and Redmine Bug #3432,2021-03-05,SSDI:Chanchal Singla
		}
	}
	//<E> To Fix Redmine #2806,20201127,SSDI:Chanchal Singla
	//<S><C>   2020.01.23, SSDI:Sneha TG
	pResourceIDInfo = GetResourceIDInfoOfControl(COLOR);
	pControl = new CShComboColors(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE],
		m_pFeatureInfoList,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_COLOR_MODE],
		m_ObjCShowMessage,
		IDC_CMB_COLMODE,
		m_PropertySheetState
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_COLMODE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCombo)\n");
	}
	//<E> 2020.01.23, SSDI:Sneha TG

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

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_COLORMODE_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_COLORMODE_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
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
void CMainTabPropertyPage::SetControlStrings(_In_ CONST HWND   hDlg)
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
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	//Favorites - Save button
	pControl = new CStaticText(IDS_IDC_STC_COPIES, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_COPIES, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
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
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
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
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
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
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}

	//Copies + button 
	pControl = new CStaticText(IDS_SPIN_PLUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_COPIES_SP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}

	//Copies - button 
	pControl = new CStaticText(IDS_SPIN_MINUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_COPIES_SM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}

	//Collate label
//<S><C>   2020.01.23, SSDI:Goutham
    //pControl = new CStaticText(IDS_IDC_CHK_COLLATE, m_hStringResourceHandle);
	pControl = new CStaticText(IDS_IDC_STC_COLLATE, m_hStringResourceHandle);
//<E>   2020.01.23, SSDI:Goutham
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_COLLATE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COLLATE)\n");
	}

	//Original Size label
	pControl = new CStaticText(IDS_IDC_GRP_PAPER, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_GRP_PAPER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_PAPER)\n");
	}

	//Original Size Value label
	pControl = new CStaticText(IDS_IDC_STC_PAPER, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PAPER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_PAPER)\n");
	}

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
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_OUTPUTSIZE)\n");
	}

	//Output Size value label
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
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_OUTPAPER)\n");
	}

	//2-Sided printing
	/*pControl = new CStaticText(m_pResourceIDInfoMap->find(DUPLEX)->second.uFeatureResourceID, m_hStringResourceHandle);
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
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_PRINT_ON_BOTH_SIDES)\n");
	}
*/
	//Orientation group box
	//pControl = new CStaticText(m_pResourceIDInfoMap->find(ORIENTATION)->second.uFeatureResourceID, m_hStringResourceHandle);
	pControl = new CStaticText(IDS_IDC_GRP_ORIENT, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_GRP_ORIENT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_ORIENT)\n");
	}

	//Zoom checkbox
	/*pControl = new CCheckBox(
		GetResourceIDInfoOfControl(ZOOM),
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOMSETTINGS],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_ZOOMSETTINGS],
		m_ObjCShowMessage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ZOOM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}*/

	//Harika
	//pControl = new CStaticText(IDS_IDC_CHK_ZOOM, m_hStringResourceHandle);
	//if (NULL != pControl)
	//{
	//	hr = AddUIControl(IDC_CHK_ZOOM, pControl);
	//	if (S_OK != hr)
	//	{
	//		slog.LoggingErrorHResultValues();
	//	}
	//}
	//else
	//{
	//	slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_PRINT_ON_BOTH_SIDES)\n");
	//}


	////Zoom Settings - Button
	//pControl = new CStaticText(IDS_IDC_BTN_ZOOM, m_hStringResourceHandle);
	//if (NULL != pControl)
	//{
	//	hr = AddUIControl(IDC_BTN_ZOOM, pControl);
	//	if (S_OK != hr)
	//	{
	//		slog.LoggingErrorHResultValues();
	//	}
	//}
	//else
	//{
	//	slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_GRP_PRINT_ON_BOTH_SIDES)\n");
	//}
	//Harika
	//2-Sided printing
	pControl = new CStaticText(IDS_IDC_STC_DUPLEX, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_DUPLEX, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_NUP)\n");
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
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_NUP)\n");
	}

	//Staple
	pControl = new CStaticText(IDS_IDC_STC_STAPLE, m_hStringResourceHandle);
	//pControl = new CStaticText(m_pResourceIDInfoMap->find(STAPLING)->second.uFeatureResourceID, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_STAPLE, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	//Punch
	pControl = new CStaticText(IDS_IDC_STC_STAPLE_POS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_STAPLE_POS, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	//Doc-filing
	pControl = new CStaticText(IDS_IDC_STC_RETENTION, m_hStringResourceHandle);
	//pControl = new CStaticText(m_pResourceIDInfoMap->find(DOCUMENT_FILING_SETTINGS)->second.uFeatureResourceID, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_RETENTION, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	//Pin Code
	/*pControl = new CCheckBox(
			GetResourceIDInfoOfControl(RETENTION_PASSWORD),
			&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_PASSWORD],
			&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_PASSWORD],
			m_hStringResourceHandle,
			m_DefaultSettings[FEATURE_RETENTION_PASSWORD],
			m_ObjCShowMessage
		);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_PIN, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}*/

	//PIN Code for Doc-Filing
	//Harika
	/*pControl = new CStaticText(IDS_IDC_CHK_PIN, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_PIN, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_PAMPHLET)\n");
	}*/
	//<E> Harika
	
	//Color Mode
	pControl = new CStaticText(IDS_IDC_GRP_COLOR_MODE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_GRP_COLOR_MODE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_PAMPHLET)\n");
	}

	pControl = new CStaticText(IDS_IDC_BTN_OTHERSET, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_OTHERSET, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CMainTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_PAMPHLET)\n");
	}
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
HRESULT CMainTabPropertyPage::OnDrawItem(_In_ CONST HWND   hDlg, _In_ CONST WPARAM wParam, _In_ CONST LPARAM lParam)
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
				//Fixed Coverity issue - 90556#1 - SSDI:Seetharam-20201006
				//else
				//{
				//	slog.putLog("CMainTabPropertyPage::SetPamphletIndex -> NULL_OBJECT(CCombo)\n");
				//}
			}
			//<S><A> Added the condition for Image button. SSDI:Seetharam - 20200212
			else if(pControl->GetControlType() == eButton)
			{
				if (NULL != pControl)
				{
					((CShBtnBmpTxt*)pControl)->OnWmDrawItem(hDlg, wParam, lParam);
				}
				//Fixed Coverity issue - 90556#2 - SSDI:Seetharam-20201006
				//else
				//{
				//	slog.putLog("CMainTabPropertyPage::SetPamphletIndex -> NULL_OBJECT(CCombo)\n");
				//}
			}
			//<S><E> Added the condition for Image button. SSDI:Seetharam - 20200212
		}
	}
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
				else
				{
					slog.putLog("CShPrintPosition::OnWmDrawItem -> NULL_OBJECT(CMimicImage)\n");
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
		case IDC_BTN_ZOOM_I:
		case IDC_BTN_COLLATE_I:
		case IDC_BTN_NUP_I:
		case IDC_BTN_STAPLE_I:
		case IDC_BTN_OUTPUTSIZE_I:
		case IDC_BTN_COLORMODE_I:
		case IDC_BTN_PAPER_I:
		case IDC_BTN_RETENTION_I:
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
	return CDocumentPropertyPage::OnDrawItem(hDlg, wParam, lParam);
}
// <S><E> To handle the ondraw for custom controls like custom combo box , 20200128 - SSDI:Seetharam

//=============================================================================
// function
//      SendCommand
//
// return value
//     HRESULT
//
// outline
//      Handles the button click event  of Black and White CHeck box.
//=============================================================================
HRESULT CMainTabPropertyPage::SendCommand(_In_ CONST HWND   hDlg, _In_ CONST WPARAM wParam)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
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
	WORD wNotificationCode = HIWORD(wParam);
	WORD wContolID = LOWORD(wParam);
	if (TRUE == UndoControlSelection(hDlg, LOWORD(wParam)))
	{
		if (IDC_CMB_PAPER == LOWORD(wParam))
		{
			SendDlgItemMessage(hDlg, LOWORD(wParam), CB_SETCURSEL, m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex, 0);
		}
		if (IDC_CMB_OUTPUTSIZE == LOWORD(wParam))
		{
			//<S><C>To Fix Bug #4698,2021.01.07,SSDI:Chanchal Singla
			//Output size changing to random paper size, when Maximum Paper Size is A4 / Letter.
			UIControlMap::const_iterator Iterator;
			MFPPrinterUI_Logger slog(__FUNCTION__"\n");
			Iterator = m_UIControls.find(IDC_CMB_OUTPUTSIZE);
			if (Iterator != m_UIControls.end())
			{
				CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
				if (pImageCombo)
				{
					long lVal = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
					pImageCombo->Select(IDC_CMB_OUTPUTSIZE, hDlg, lVal);
					//	SendDlgItemMessage(hDlg, LOWORD(wParam), CB_SETCURSEL, m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex, 0);
				}
				else
				{
					slog.putLog("CMainTabPropertyPage::Select -> NULL_OBJECT(pImageCombo)\n");
				}
			}
			//<E>To Fix Bug #4698,2021.01.07,SSDI:Chanchal Singla
		}
		// <S><M> Coverity Fix 91466 & 91525 - SSDI:Seetharam - 20210907
		//return S_OK;
		hr = S_OK;
		goto EXIT;
		// <S><E> Coverity Fix 91466 & 91525 - SSDI:Seetharam - 20210907
	}
	//<S><M>Move the code to UndoControlSelection() to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
	//Bug #3666:Information message fails to display after clicking factory defaults
	//Bug #3464 :Information Message is displaying even though 2-Sided Printing drop down is None.
	////<S><A>To Fix Redmine Bug #3936,2021.28.07,ssdi:Chanchal Singla
	////Wrong information message is displayed
	//if (DisplayPaperTrayVsOutputSizeDialog(hDlg, LOWORD(wParam)) == TRUE)
	//{
	//	if (IDC_CMB_OUTPUTSIZE == LOWORD(wParam))
	//	{
	//		m_ObjCShowMessage->Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_77, MB_ICONINFORMATION | MB_OK);
	//		SendDlgItemMessage(hDlg, LOWORD(wParam), CB_SETCURSEL, m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex, 0);
	//	}
	//	// <S><M> Coverity Fix 91466 - SSDI:Seetharam - 20210907
	//	//return S_OK;
	//	hr = S_OK;
	//	goto EXIT;
	//	// <S><E> Coverity Fix 91466 - SSDI:Seetharam - 20210907
	//}
	//<E>To Fix Redmine Bug #3936,2021.28.07,ssdi:Chanchal Singla
	//<E>Move the code to UndoControlSelection() to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
	CDocumentPropertyPage::SendCommand(hDlg, wParam);

	switch (wNotificationCode)
	{
	case BN_CLICKED:
	{
		// <S><A> To add Zoom Settings Dialog and other settings dialog in UD3L, 20200408 - SSDI:Sneha TG
		OnBnClicked(hDlg, FALSE, wContolID);
		// <S><E> To add Zoom Settings Dialog and other settings dialog in UD3L, 20200408 - SSDI:Sneha TG
	}
	break;
	case CBN_SELCHANGE:
	{
		OnCbnSelchange(hDlg, 0, wContolID);

		if (IDC_COMBO_PAMPHLET == wContolID)
		{
			SetControlState(hDlg, IDC_BTN_DUPLEX_I, SCUI_STATE_OFF);
		}
		//<S><A> RedMine Ticket(#1185) Mimic Image: Mimic image is displayed wrong when Border is selected: 2014.11.11, SSDI:Sonali
		if (nup_1 == atoi(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BORDER].pszOption = "OFF";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BORDER].uChangedOptionIndex = eFirstOption;
		}
		//<E> RedMine Ticket(#1185) Mimic Image: Mimic image is displayed wrong when Border is selected: 2014.11.11, SSDI:Sonali
	}
	break;
	//<S><A> Added to retain Document filing PINCode value when we switch between tabs, Harika-20210422
	case EN_CHANGE:
	{
		switch (wContolID)
		{
		case IDC_EDT_PIN:
		{
			UIControlMap* pComponents = NULL;
			CUIControl* pControl = NULL;
			pComponents = &m_UIControls;
			pControl = (*pComponents)[wContolID];
			((CJobControlEdit*)pControl)->OnEnChange(wContolID, hDlg);
			PropStateToJobControlData();
		}
		break;
		}
	}
	//<S><E> Added to retain Document filing PINCode value when we switch between tabs, Harika-20210422
	default:
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

EXIT:
	/*Delete the momory allocation of wFeatureInfoList_Pre and wTempFeatureInfoList*/
	//this condition is kept because driver Crashing in case of FeatureInfoList is NULL 
	if ((NULL == wFeatureInfoList_Pre) )
	{
		wFlag = TRUE;
	}
	if (wFlag == FALSE)
	{
		//Fixed Coverity #91466,13-08-2021,SSDI:Chanchal Singla
		if (NULL != wTempFeatureInfoList)
		{
			delete wTempFeatureInfoList;
			wTempFeatureInfoList = NULL;
		}
		//Fixed Coverity #91432,13-08-2021,SSDI:Chanchal Singla
		if (NULL != wFeatureInfoList_Pre)
		{
			delete wFeatureInfoList_Pre;
			wFeatureInfoList_Pre = NULL;
		}
	}
	//<E> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
	return hr;
}

//=============================================================================
// function
//        OnCbnSelchange
//
// parameters
//        hWnd        
//        hWndCtrl     
//        wCtrlID      
// return value
//        ê TRUE
//         FALSE
//
// outline
//        WM_COMMAND - CBN_SELCHANGE
//=============================================================================
LRESULT CMainTabPropertyPage::OnCbnSelchange(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	short			wPre = 0;


	switch (wCtrlID)
	{
	//<S><C> Modified The Code to Fix Bug #3533,#3593(Issue 1),2021.07.28,SSDI:Chanchal Singla
	//Information icon is not displayed when Stapleless and Paper size is set
	case IDC_CMB_PAPER:
	{
		ModeLimit(hDlg, IDC_CMB_PAPER, FEATURE_PAPERSIZE);
		if (!strcmp("None", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
		{
			//<S><A>To Fix Feedback Issue 16 and added Constarints between paper tray/type ,output size ,original size,2020-10-29,SSDI:Chanchal Singla
			UINT PaperSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
			UINT wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
			UINT wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
			UINT wPunch = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex;
			if (!strcmp("SAOS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption))
			{
				UINT	Pre_SelectedOptionindex = 0;
				PCSTR Pre_SellectedOptionName = NULL;
				if ((PaperSize >= PAPERSIZE_ENV_DL && PaperSize <= PAPERSIZE_JENV_YOU4)/* && (wStaple == staple_none)*/)// ENV_DL to JENV_YOU4(105x235)
				{
					Pre_SelectedOptionindex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex;
					Pre_SellectedOptionName = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].ppszOptions[Pre_SelectedOptionindex];
					m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_Envelope;
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "ENVELOPE";
					
					if (OnPaperTypeChanged(hDlg, wCtrlID) == TRUE)
					{
						m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = Pre_SelectedOptionindex;
						m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = Pre_SellectedOptionName;
					}
				}
				else if ((PaperSize == PAPERSIZE_JAPANESE_POSTCARD) /*&& (wStaple == staple_none)*/)
				{
					Pre_SelectedOptionindex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex;
					Pre_SellectedOptionName = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].ppszOptions[Pre_SelectedOptionindex];
					m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_JapanesePostCard;
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "JAPANESEPOSTCARD";
					if (OnPaperTypeChanged(hDlg, wCtrlID) == TRUE)
					{
						m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = Pre_SelectedOptionindex;
						m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = Pre_SellectedOptionName;
					}
				}
				//<S><C> Fix for bug 6035. (for any staple, for other than small papersizes, Media type should be  Autoselect. Harika 20230216 
				else if (PaperSize <= PAPERSIZE_EXECUTIVE /*&& (wStaple != staple_none)*/ && (wDuplex == eFirstOption)
					&& (!strcmp("ENVELOPE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)
						|| !strcmp("JAPANESEPOSTCARD", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)))
				//<E> Fix for bug 6035. (for any staple, for other than small papersizes, Media type should be  Autoselect. Harika 20230216
				{
					m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_AutoSelect;
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "AUTOSELECT";
				}
			}
		}
	}
	break;
	//<E> Modified The Code to Fix Bug #3533,#3593(Issue 1),2021.07.28,SSDI:Chanchal Singla
	//<S><C>Modiled the Conditions To Fix Redmine Bug #3531,#3593(Issue 1),2021-28-07,SSDI:Chanchal Singla
	//No Information icon is displayed for Staple
	case IDC_CMB_OUTPUTSIZE:
	{
		//FitToPSize  variable are updated with feature's latest index.
		UINT FitToPSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
		UINT PaperSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
		UINT wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
		UINT wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
		UINT wPunch = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex;
		if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
		{
			//bFlag = 2;
		}
		else
		{
			UINT	Pre_SelectedOptionindex = 0;
			PCSTR Pre_SellectedOptionName = NULL;
			if ((FitToPSize >= OUTPUTPAPERSIZE_ENV_DL && FitToPSize <= OUTPUTPAPERSIZE_JENV_YOU4)&&(wStaple == staple_none))
			{
				Pre_SelectedOptionindex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex;
				Pre_SellectedOptionName = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].ppszOptions[Pre_SelectedOptionindex];
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_Envelope;
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "ENVELOPE";
				if (OnPaperTypeChanged(hDlg, wCtrlID) == TRUE)
				{
					m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = Pre_SelectedOptionindex;
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = Pre_SellectedOptionName;
				}
			}
			else if ((FitToPSize == OUTPUTPAPERSIZE_JAPANESE_POSTCARD) && (wStaple == staple_none))
			{
				Pre_SelectedOptionindex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex;
				Pre_SellectedOptionName = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].ppszOptions[Pre_SelectedOptionindex];
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_JapanesePostCard;
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "JAPANESEPOSTCARD";
				if (OnPaperTypeChanged(hDlg, wCtrlID) == TRUE)
				{
					m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = Pre_SelectedOptionindex;
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = Pre_SellectedOptionName;
				}
			}
			else
			{
				//<S><C> Fix for bug 6035. (for any staple, for other than small papersizes, Media type should be  Autoselect. Harika 20230216
				if ((FitToPSize >= OUTPUTPAPERSIZE_A3 && FitToPSize <= OUTPUTPAPERSIZE_EXECUTIVE)
					/*&& (wStaple != staple_none)*/ && (wDuplex == eFirstOption)
					&& (!strcmp("ENVELOPE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)
						|| !strcmp("JAPANESEPOSTCARD", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)))
				//<E> Fix for bug 6035. (for any staple, for other than small papersizes, Media type should be  Autoselect. Harika 20230216
				{
					m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_AutoSelect;
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "AUTOSELECT";
				}
				else if (!strcmp("SAOS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption))
				{
					if (PaperSize <= PAPERSIZE_EXECUTIVE
						/*&& (wStaple != staple_none)*/ && (wDuplex == eFirstOption)
						&& (!strcmp("ENVELOPE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)
							|| !strcmp("JAPANESEPOSTCARD", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)))
					{
						m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_AutoSelect;
						m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "AUTOSELECT";
					}
				}
			}
		}
		//<E>Modiled the Conditions To Fix Redmine Bug #3531,#3593(Issue 1),2021-28-07,SSDI:Chanchal Singla

	}
	break;
	//<E>To Fix Feedback Issue 16 and added Constarints between paper tray/type ,output size ,original size,2020-10-29,SSDI:Chanchal Singla
	case IDC_CMB_STAPLE:
	{
		UINT wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
		//<S><C>To fix Redmine Bug #3508,25-03-2021,SSDI:Chanchal Singla
		//Offset drop down is displaying Offset each set,when we selected 1 Staple/2 Staples.
		if (wStaple != staple_none)
		{
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OFFSET].uChangedOptionIndex = eFirstOption;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OFFSET].pszOption = "None";
			if (!strcmp("STAPLE_SADDLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
			{
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex = punchtype_none;
			}
		}
	}
	break;
	case IDC_CMB_RETENTION:
	{
		m_PropertySheetState.wRetentionMode = (SHORT)GetValue(hDlg, IDC_CMB_RETENTION);
		if (m_PropertySheetState.wRetentionMode == retention_normal) {
			m_PropertySheetState.wRetention = bool_false;
			//<S><A> SSID Bug# 4122-Issue2 - SSDI:Seetharam - 20210727
			m_PropertySheetState.wUsePIN = FALSE;
			CheckDlgButton(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
			SetCheckBoxValue(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
			//<S><E> SSID Bug# 4122-Issue2 - SSDI:Seetharam - 20210727
		}
		else {
			m_PropertySheetState.wRetention = bool_true;
			//ShowConstraintsOnRetention(hDlg, IDC_CMB_RETENTION);		//<S><A> to fix 3620, Harika-20210422
		}
		if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption, "QUICKFILE"))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption = "MAINFOLDER";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uChangedOptionIndex = eSecondOption;
			m_PropertySheetState.wDocFileMode = eSecondOption;
			m_PropertySheetState.wUsePIN = BST_UNCHECKED;
			// --- DM_USE_PIN
			m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_USE_PIN);
			wcscpy(m_PropertySheetState.szRetentionPassword, L"");
		}
	}
	break;
	case IDC_CMB_SHARED_USRSET:
	{
		OnUsSelect(hDlg, m_pPrinterName, IDC_CMB_SHARED_USRSET);
		//ShowConstraintsOnRetention(hDlg, IDC_CMB_RETENTION);
	}
		break;

	default:
		break;
	}

	res = 1;

	return res;
}
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
HRESULT CMainTabPropertyPage::SetTabDefaults(_In_ CONST HWND   hDlg) 
{

//<S><A>To fix Redmine Bug #2238 and #2239 2020.05.13 Sneha TG
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	OEMDEV temp_pOemPrivateDevMode = {};
//<S><A>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
	PROPSTATE pPropertySheetState;
	SecureZeroMemory(&pPropertySheetState, sizeof(PROPSTATE));
//<E>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
//<A>To fix Redmine Bug #2238 and #2239 2020.05.13 Sneha TG
    __super::SetTabDefaults(hDlg);
//<S><A>To fix Redmine Bug #2254 2020.05.25 Sneha TG
	SetControlState(hDlg, IDC_BTN_ZOOM, SCUI_STATE_SHOW);
//<A>To fix Redmine Bug #2254 2020.05.25 Sneha TG
	ShowFinishingInformationIcon(hDlg);
	ShowConstraintsOnCollate(hDlg, IDC_EDT_COPIES);
	ShowPaperSizeDimensions(hDlg, IDC_CMB_PAPER);
	//<S><A>To Fix SSDI Bug #2707,2020-09-04,SSDI:Chanchal Singla
	//After Clicking Defaults,Output Size value and Dimensions are not displaying correctly in Paper Source Tab.
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		ShowBookletPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
		//<S><A>Added to fix redmine bug #2708,2020.09.10 SSDI:Goutham
		TCHAR szItem[MAX_PATH] = { 0 };
		if (LoadString(m_hStringResourceHandle, IDS_IDC_CMB_DUPLEX_1S, szItem, countof(szItem)) > 0)
		{
			UINT uChangedOptionIndex = static_cast<UINT>(SendDlgItemMessage(hDlg, IDC_CMB_DUPLEX, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szItem));
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex = uChangedOptionIndex;
//<S><A> To fix redmine #2744 issue 2020.09.16 SSDI:Sneha TG
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption = "NONE";
//<E> To fix redmine #2744 issue 2020.09.16 SSDI:Sneha TG
			SendDlgItemMessage(hDlg, IDC_CMB_DUPLEX, CB_SETCURSEL, uChangedOptionIndex, 0);
		}
		//<S><A>Added to fix redmine bug #2708,2020.09.10 SSDI:Goutham
	}
	else
	{
		ShowPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	}
	//<E>To Fix SSDI Bug #2707,2020-09-04,SSDI:Chanchal Singla
	m_PropertySheetState.wUsePIN = (m_DefaultSettings[FEATURE_RETENTION_PASSWORD].iDevmodeValue == bool_false) ?
		bool_false : bool_true;
	// --- DM_USE_PIN
	m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wUsePIN == bool_true) ?
		(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_USE_PIN) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_USE_PIN);

	ShowConstraintsOnRetention(hDlg, IDC_CMB_RETENTION);
	ShowConstraintsOnStaple(hDlg, IDC_CMB_STAPLE);
	//<S><A>To Fix Redmine #7424,31-05-2024,SSDI:Manoj S
	limStapleVsBooklet(hDlg, IDC_CMB_STAPLE, FEATURE_STAPLING);
	//<E>To Fix Redmine #7424,31-05-2024,SSDI:Manoj S
//<S><A>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
	m_PropertySheetState.wZoomBasing = zoom_basing_upperleft;
	if (m_DefaultSettings[FEATURE_ZOOM_BASING].iDevmodeValue == zoom_basing_upperleft)
	{
		m_PropertySheetState.wZoomBasing = zoom_basing_upperleft;
	}
	else
	{
		m_PropertySheetState.wZoomBasing = zoom_basing_center;
	}
	m_pOemPrivateDevMode->scpi.doc.dwFieldDDM = (m_PropertySheetState.wZoomBasing == zoom_basing_center) ?
		(m_pOemPrivateDevMode->scpi.doc.dwFieldDDM | DM_ZOOM_CENTER) : (m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & ~DM_ZOOM_CENTER);
//<E>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
//<S><A>To fix Zoom Edit box default value #2377 redmine bug 2020.06.19 Sneha TG
	m_pOemPrivateDevMode->scpi.ext.nZoom = m_pOemPublicDevMode->dmScale;
//<E>To fix Zoom Edit box default value #2377 redmine bug 2020.06.19 Sneha TG
//<S><A>To fix Redmine Bug #2238 and #2239 2020.05.13 Sneha TG
	memcpy_s(&temp_pOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));
//To set default value for Zoom Settings dialog
	// Fixed Coverity issue - 90433
	CZoomSettingsDialog *pdlgZoom = new CZoomSettingsDialog(IDD_DLG_ZOOM_11, m_hStringResourceHandle, m_hPrinterHandle, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings, m_hBitmapResourceHandle, m_pOemPublicDevMode, &temp_pOemPrivateDevMode, m_ObjCShowMessage, &pPropertySheetState);
	if (NULL == pdlgZoom)
	{
		slog.putLog("CMainTabPropertyPage::OnZoomSettings -> NULL_POINTER(IDD_DLG_ZOOM_11)\n");
		return FALSE;
	}
	pdlgZoom->CreateControlClass();
	pdlgZoom->SetTabDefaults(hDlg);

//To set default value for Other Settings dialog
	// Fixed Coverity issue - 90378
//<S><A>To set defaults for OtherSettings dialog, 2020.10.08, SSDI:Sneha TG
	OtherSettingsSetTabDefaults(hDlg);
//<E>To To set defaults for OtherSettings dialog, 2020.10.08, SSDI:Sneha TG
//<A>To fix Redmine Bug #2238 and #2239  2020.05.13 Sneha TG
//	ShowConstraintsOnCollate(hDlg);
	CustomPaperSetTabDefaults(hDlg);
//<S><A>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
	UpdatePropstateToStructure();
//<E>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
	return S_OK;
}
VOID CMainTabPropertyPage::SetPamphletIndex(_In_ CONST HWND   hDlg,INT iCtrlID)
{
	BOOL bChangePamphletIndex = TRUE;
	UINT uPamphletIndex = SendDlgItemMessage(hDlg,IDC_COMBO_PAMPHLET,CB_GETCURSEL, 0, 0);;
	UINT uPaperIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	if(0 != uPamphletIndex)
	{
		switch(uPaperIndex)
		{
			case 0:
			case 3: 
			case 5:
			case 7:
				uPamphletIndex = 2 ;
				break;
			case 1:
			case 6:
				{
					bChangePamphletIndex = FALSE ;
			
				}
				break;
			default:
				uPamphletIndex = 1;
				break ;


		}
	}
	if(TRUE == bChangePamphletIndex)
	{
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAMPHLET].uChangedOptionIndex = uPamphletIndex;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAMPHLET].pszOption = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAMPHLET].ppszOptions[uPamphletIndex];
		UIControlMap::const_iterator iPamphletComboIterator =  m_UIControls.find(IDC_COMBO_PAMPHLET);
		INT iIncrementIndex = 0;
		if(0 == uPamphletIndex && (0 < iCtrlID))
		{
			iIncrementIndex = -1 ;
		}
		if(iPamphletComboIterator != m_UIControls.end())
		{
			CCombo * pControl = static_cast<CCombo*>(iPamphletComboIterator->second);
			if(NULL != pControl)
			{
				pControl->SetComboBoxIndex(m_hPropertySheetHandle,IDC_COMBO_PAMPHLET,uPamphletIndex,iIncrementIndex);
			}
			else
			{
				slog.putLog("CMainTabPropertyPage::SetPamphletIndex -> NULL_OBJECT(CCombo)\n");
			}
		}
	}
}
VOID CMainTabPropertyPage::ShowFinishingInformationIcon(HWND hDlg)
{
	UINT uStapleIndex = 0 ;
	UINT uPunchIndex = 0 ;
	uStapleIndex = SendDlgItemMessage(hDlg,IDC_CMB_STAPLE,CB_GETCURSEL, 0, 0);
	uPunchIndex = SendDlgItemMessage(hDlg,IDC_CMB_PUNCH,CB_GETCURSEL, 0, 0);
	if( (0 == uStapleIndex) && (0 == uPunchIndex))
	{
		SetControlState(hDlg,IDC_BTN_FINISH_I,SCUI_STATE_OFF);
	}
	else
	{
		SetControlState(hDlg,IDC_BTN_FINISH_I,SCUI_STATE_ON);
	}
	
}
VOID CMainTabPropertyPage::ShowConstraintsOnCollate(HWND hDlg,INT iCtrlID)
{ 
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UINT dmCopies  = m_pOemPrivateDevMode->scpi.pub.dmCopies;
	BOOL bComboCollate = FALSE ;
	m_ControlState.cChkCollate = SCUI_STATE_ON;
	m_ControlState.cCollate_I = SCUI_STATE_OFF;
//<S><C>   2020.01.24, SSDI:Goutham
	//UIControlMap::const_iterator iCollateCheckBoxIterator =  m_UIControls.find(IDC_CMB_COLLATE);
	UIControlMap::const_iterator iCollateComboBoxIterator = m_UIControls.find(IDC_CMB_COLLATE);
//<E>   2020.01.24, SSDI:Goutham
	if(iCollateComboBoxIterator == m_UIControls.end())
	{
		return ;
	}
	CShImageCombo * pControl = static_cast<CShImageCombo*>(iCollateComboBoxIterator->second);
	if(NULL == pControl)
	{
		slog.putLog("CMainTabPropertyPage::ShowConstraintsOnCollate -> NULL_OBJECT(CCheckBox)\n");
		return ;
	}
	if(1 == dmCopies)
	{
		bComboCollate = TRUE;
		m_ControlState.cChkCollate = SCUI_STATE_OFF;
	}
	else
	{
		
		UINT uBookletindex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	
		if(0 != uBookletindex)
		{
			m_ControlState.cChkCollate = SCUI_STATE_SHOW;
			m_ControlState.cCollate_I = SCUI_STATE_ON;
		}

	}
	if (TRUE == bComboCollate)
	{
		//<S><C> to fix SSDI Bug #2376,14-07-2020,SSDI:Chanchal Parkash
		//Collate drop down value is not set to default when copies is set
		//pControl->SetComboBoxIndex(hDlg, IDC_CMB_COLLATE, 0, 0);
		UINT suCurrentItemId = m_DefaultSettings[FEATURE_COLLATE].ResourceID;
		TCHAR szItem[MAX_PATH] = { 0 };
		if (LoadString(m_hStringResourceHandle, suCurrentItemId, szItem, countof(szItem)) > 0)
		{
			UINT uChangedOptionIndex = static_cast<UINT>(SendDlgItemMessage(hDlg, IDC_CMB_COLLATE, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szItem));
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE].uChangedOptionIndex = uChangedOptionIndex;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLLATE].pszOption = "ON";
			
			SendDlgItemMessage(hDlg, IDC_CMB_COLLATE, CB_SETCURSEL, uChangedOptionIndex, 0);
		}
		//<E> to fix SSDI Bug #2376,14-07-2020,SSDI:Chanchal Parkash
	}
	// --- collate
	SetControlState(hDlg,IDC_CMB_COLLATE, m_ControlState.cChkCollate);
	SetControlState(hDlg,IDC_STC_COLLATE, m_ControlState.cChkCollate);

	if (m_ControlState.cChkCollate == SCUI_STATE_OFF) {
		SetControlState(hDlg, IDC_BTN_COLLATE_I, SCUI_STATE_OFF);
	}
	else {
		SetControlState(hDlg, IDC_BTN_COLLATE_I, m_ControlState.cCollate_I);
	}


}
VOID CMainTabPropertyPage::SetPaperConstraints()
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(!strcmp("FitToPaper",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption))
	{
		return ;
	}
	PCSTR pFitToPaperFeature = m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszFeature ;
	PCSTR pFitToPaperOption = m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption ;
	PCSTR pPaperOption = m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption ;
	//m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption = pPaperOption ;
	auto iFitToPaperFeatureIterator = m_pResourceIDInfoMap->find(pFitToPaperFeature);
	if(iFitToPaperFeatureIterator == m_pResourceIDInfoMap->end())
	{
		return ;
	}
	auto iFitToPaperOptionIterator = iFitToPaperFeatureIterator->second.uOptionResourceID.find(pFitToPaperOption);
	if(iFitToPaperOptionIterator == iFitToPaperFeatureIterator->second.uOptionResourceID.end())
	{
		return ;
	}
	auto iPaperOptionIterator = iFitToPaperFeatureIterator->second.uOptionResourceID.find(pPaperOption);
	if(iPaperOptionIterator == iFitToPaperFeatureIterator->second.uOptionResourceID.end())
	{
		return ;
	}
	std::pair<UINT,UINT>FeatureOptionIdPair = std::make_pair(iFitToPaperFeatureIterator->second.uFeatureResourceID,iFitToPaperOptionIterator->second.first);
	std::map<std::string,UINT>ConstraintFeatureNameID;
	CONSTRAINTFEATUREOPTIONINFO constraintfeatureoptioninfo ;
	ConstraintFeatureInfoMap::iterator iIteratorToConstraintFeatureInfoMap = m_ConstraintFeatureInfoMap->find(FeatureOptionIdPair);
	if(iIteratorToConstraintFeatureInfoMap != m_ConstraintFeatureInfoMap->end())
	{
		constraintfeatureoptioninfo =  iIteratorToConstraintFeatureInfoMap->second;
		AddOrRemoveConstraints(ConstraintFeatureNameID,constraintfeatureoptioninfo,-1,FeatureOptionIdPair);
	}
	FeatureOptionIdPair = std::make_pair(iFitToPaperFeatureIterator->second.uFeatureResourceID,iPaperOptionIterator->second.first);
	iIteratorToConstraintFeatureInfoMap = m_ConstraintFeatureInfoMap->find(FeatureOptionIdPair);
	if(iIteratorToConstraintFeatureInfoMap != m_ConstraintFeatureInfoMap->end())
	{
		constraintfeatureoptioninfo =  iIteratorToConstraintFeatureInfoMap->second;
		AddOrRemoveConstraints(ConstraintFeatureNameID,constraintfeatureoptioninfo,1,FeatureOptionIdPair);
	}
	UINT uConstrainedControlID = 0;
	for(auto iIteratorToConstraintFeatureNameID : ConstraintFeatureNameID)
	{
		UIControlMap::const_iterator iterUIComponents = m_UIControls.find(iIteratorToConstraintFeatureNameID.second);
		if(m_UIControls.end() != iterUIComponents)
		{
			CUIControl *pUIControl  = iterUIComponents->second;
			if(nullptr !=  pUIControl)
			{
				uConstrainedControlID = iIteratorToConstraintFeatureNameID.second;
				pUIControl->OnInit(uConstrainedControlID,m_hPropertySheetHandle);
			}
			else
			{
				slog.putLog("CMainTabPropertyPage::SetPaperConstraints -> NULL_OBJECT(pUIControl)\n");
			}
		}	
	}
}

void CMainTabPropertyPage::SetPamphletMessage()
{
	auto ItrPamphletFeature = m_pResourceIDInfoMap->find(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAMPHLET].pszFeature);
	if(ItrPamphletFeature != m_pResourceIDInfoMap->end())
	{
		auto ItrPamphletOption = ItrPamphletFeature->second.uOptionResourceID.find("TiledPamphlet");
		if(ItrPamphletOption != ItrPamphletFeature->second.uOptionResourceID.end())
		{
			if(GetUnitDef() == 0)
			{
				ItrPamphletOption->second.second.uInformationMessageID = IDS_MESSAGE_508;
			}

		}
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
LRESULT CMainTabPropertyPage::OnBnClicked(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
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
	case IDC_BTN_COPIES_SP:
	case IDC_BTN_COPIES_SM:
	{
		pControl = (*pComponents)[IDC_EDT_COPIES];
		((CShEditSpin*)pControl)->OnBnClicked(hDlg, hWndCtrl, wCtrlID);
		::SetFocus(GetDlgItem(hDlg, IDC_EDT_COPIES));
	}
	break;
	case IDC_CHK_ZOOM:
	{
		pControl = (*pComponents)[IDC_CHK_ZOOM];
		((CCheckBox*)pControl)->OnBnClicked(wCtrlID, hDlg);
		ShowConstraintsOnZoomSetting(hDlg);
		BOOL blChecked = IsDlgButtonChecked(hDlg, IDC_CHK_ZOOM);
		UINT wPaper = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
		UINT wFitToPSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
		if (blChecked != FALSE)
		{
			if (!strcmp("SAOS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption))
			{
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex = (wPaper + 1);
			}
		}
		else
		{
			if (wFitToPSize == (wPaper + 1))
			{
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex = eFirstOption;
			}
		}
	}
	break;
	case IDC_CHK_PIN:
	{
		pControl = (*pComponents)[IDC_CHK_PIN];
		((CCheckBox*)pControl)->OnBnClicked(wCtrlID, hDlg);
		m_PropertySheetState.wUsePIN = IsDlgButtonChecked(hDlg, IDC_CHK_PIN);
		// --- DM_USE_PIN
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wUsePIN == bool_true) ?
			(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_USE_PIN) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_USE_PIN);

		if (m_PropertySheetState.wUsePIN == TRUE)
		{
			SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_ON);
		}
		else
		{
			SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_SHOW);
		}
		//UpdateEditBoxContent(hDlg, IDC_CHK_PIN);

	}
	break;
	// <S><A> To add Zoom Settings Dialog in UD3L, 20200408 - SSDI:Sneha TG
	case IDC_BTN_ZOOM:
		OnZoomSettings(hDlg);
		break;
		// <S><E> To add Zoom Settings Dialog in UD3L, 20200408 - SSDI:Sneha TG
		// <S><A> To add Other Settings Dialog in UD3L, 20200409 - SSDI:Goutham
	case IDC_BTN_OTHERSET:
		OnOtherSettings(hDlg);
		break;
		// <S><E> To add Other Settings Dialog in UD3L, 20200409 - SSDI:Goutham
	case IDC_BTN_SHARED_DEFAULT:
	{
		SetTabDefaults(hDlg);
	}
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
	case IDC_BTN_ZOOM_I:
	case IDC_BTN_COLLATE_I:
	case IDC_BTN_NUP_I:
	case IDC_BTN_STAPLE_I:
	case IDC_BTN_OUTPUTSIZE_I:
	case IDC_BTN_COLORMODE_I:
	case IDC_BTN_PAPER_I:
	case IDC_BTN_RETENTION_I:
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
		;
	}
	res = 1;
//	SetTabState(hDlg);
	return res;
}

//<S><A> To add the Show Constraints On Retention  function for hiding and showing Pin edit box ,06-02-2020,SSDI:Chanchal 
//=============================================================================
// function
//      ShowConstraintsOnRetention
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      0
//
// outline
//      ShowConstraintsOnRetention 
//=============================================================================
VOID CMainTabPropertyPage::ShowConstraintsOnRetention(HWND hDlg, INT iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UINT Retention = static_cast<UINT>(SendDlgItemMessage(hDlg, iCtrlID, CB_GETCURSEL, 0, 0));
	//<S><C> to implement restrictions with PPLCY dialog, Harika, 20201021
	//<S><A>To Fix Redmine #7483,31-05-2024,SSDI:Manoj S
	if (((m_PropertySheetState.wPPlcyPrintRelease == bool_true && (m_PropertySheetState.byPrintRelease == bool_false)) || (m_PropertySheetState.wPPlcyRetention == bool_true)) /*&& !(m_PropertySheetState.byPrintRelease)*/)
	//<E>To Fix Redmine #7483,31-05-2024,SSDI:Manoj S
	{
		//<S><A> to fix #Bug #2906,20211802,SSDI:Chanchal Singla
		//Print Release/Doc Filing not set correctly when Printing Policy options are reset to defaults.
		if ((m_PropertySheetState.wPPlcyPrintRelease == bool_true) && (m_PropertySheetState.byPrintRelease == bool_false))
		{
			m_PropertySheetState.wPrintRelease = bool_true;
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex = m_PropertySheetState.wPrintRelease;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption = "ON";

			if (m_PropertySheetState.wRetention != bool_false)
			{
				m_PropertySheetState.wRetentionMode = retention_normal;
				InitializeFeatureOption(hDlg, IDC_CMB_RETENTION, m_PropertySheetState.wRetentionMode);
				m_PropertySheetState.wRetention = bool_false;
			}
			//Coverity Fix 90879 - SSDI:Goutham - 20201123
			m_PropertySheetState.wPPlcyRetention = bool_false;
		}
		if (m_PropertySheetState.wPPlcyRetention != bool_false)
		{
			m_PropertySheetState.wRetention = bool_true;

			// retention_hold_before // Document FilingÇHold Only
			m_PropertySheetState.wRetentionMode = eSecondOption;
			InitializeFeatureOption(hDlg, IDC_CMB_RETENTION, m_PropertySheetState.wRetentionMode);

			//docfile_main  // Stored ToÇMain Folder
			m_PropertySheetState.wDocFileMode = eSecondOption;
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uChangedOptionIndex = m_PropertySheetState.wDocFileMode;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption = "MAINFOLDER";

			// PIN CodeÇOFF
			m_PropertySheetState.wUsePIN = bool_false;
			// --- DM_USE_PIN
			m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wUsePIN == bool_true) ?
				(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_USE_PIN) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_USE_PIN);

			CheckDlgButton(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
			SetCheckBoxValue(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);

			// Print Release, OFF
			m_PropertySheetState.wPrintRelease = bool_false;
			m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wPrintRelease == bool_true) ?
				(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_PRINTSHARE) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_PRINTSHARE);
			CheckDlgButton(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex = m_PropertySheetState.wPrintRelease;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption = "OFF";

			m_PropertySheetState.wPPlcyPrintRelease = bool_false;
		}
		//Print Release/Doc Filing not set correctly when Printing Policy options are reset to defaults.
		//<E> to fix #Bug #2906,20211802,SSDI:Chanchal Singla
		m_ControlState.cRetention_I = SCUI_STATE_OFF;
		m_ControlState.cCmbRetention = SCUI_STATE_OFF;
		wcscpy(m_PropertySheetState.szRetentionPassword, L"");
		SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
	}
	else if (m_PropertySheetState.byPrintRelease == bool_true)
	{
		if(m_PropertySheetState.wPPlcyRetention == bool_true)
		{
			m_ControlState.cRetention_I = SCUI_STATE_OFF;
			m_ControlState.cCmbRetention = SCUI_STATE_OFF;
		}
		else
		{
			if (m_PropertySheetState.wPrintRelease == TRUE)
			{
				m_PropertySheetState.wPrintRelease = BST_UNCHECKED;
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex = m_PropertySheetState.wPrintRelease;
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption = "OFF";
			}
			m_ControlState.cRetention_I = SCUI_STATE_OFF;
			m_ControlState.cCmbRetention = SCUI_STATE_ON;
			//<S><A>To fix 2909, Harika, 20201030
			short wTarget = (short)SendMessage(::GetDlgItem(hDlg, IDC_CMB_RETENTION), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			if(0!=wTarget)
			{
				//<S><C>to Fix feedback issue #61,2021.12.15,SSDI:Chanchal Singla
				//When "Stored to" is "Quick File", PIN Code is not hidden.
				if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption, "QUICKFILE"))//Quick Store
				{
					SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
					SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
					m_PropertySheetState.wUsePIN = BST_UNCHECKED;
					// --- DM_USE_PIN
					m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_USE_PIN);
					wcscpy(m_PropertySheetState.szRetentionPassword, L"");
				}
				else
				{
					SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_ON);
				}
				//<S><C> SSID Bug# 4122-Issue1 - SSDI:Seetharam - 20210727
				//SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_ON);
				if (m_PropertySheetState.wUsePIN == TRUE)
				{
					SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_ON);
				}
				else
				{
					if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption, "QUICKFILE"))//Quick Store)
					{
						SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
					}
					else
					{
						SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_SHOW);
					}

					wcscpy(m_PropertySheetState.szRetentionPassword, L"");
				}
				//<S><E> SSID Bug# 4122-Issue1 - SSDI:Seetharam - 20210727
				//<E>to Fix feedback issue #61,2021.12.15,SSDI:Chanchal Singla
			}
			else
			{
				wcscpy(m_PropertySheetState.szRetentionPassword, L"");
				SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
			}
			//<E>To fix 2909, Harika, 20201030
		}
	}
	//else if (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex == 1)
	else if (!strcmp("ON", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption))
	{
		m_ControlState.cRetention_I = SCUI_STATE_ON;
		m_ControlState.cCmbRetention = SCUI_STATE_SHOW;
	}
	else
	{
		m_ControlState.cRetention_I = SCUI_STATE_OFF;
		m_ControlState.cCmbRetention = SCUI_STATE_ON;
		if (retention_normal == Retention)
		{
			SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
			//<S><C>To Fix Redmine Bug #2908,20201126,SSDI;Chanchal Singla
			//Pin Code check box is set to ON when Document filing is set to Hold Only.
			CheckDlgButton(hDlg, IDC_CHK_PIN, BST_UNCHECKED);
			m_PropertySheetState.wUsePIN = BST_UNCHECKED;
			// --- DM_USE_PIN
			m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wUsePIN == bool_true) ?
				(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_USE_PIN) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_USE_PIN);

			SetCheckBoxValue(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
			wcscpy(m_PropertySheetState.szRetentionPassword, L"");
			//<E>To Fix Redmine Bug #2908,20201126,SSDI;Chanchal Singla
			//UpdateEditBoxContent(hDlg, IDC_CHK_PIN);
		}
		else if (retention_hold_after == Retention || retention_hold_before == Retention || retention_sample == Retention)
		{
			//<S><C>to Fix feedback issue #61,2021.12.15,SSDI:Chanchal Singla
			//When "Stored to" is "Quick File", PIN Code is not hidden.
			if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption, "QUICKFILE"))//Quick Store
			{
				SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
				m_PropertySheetState.wUsePIN = BST_UNCHECKED;
				// --- DM_USE_PIN
				m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_USE_PIN);
				wcscpy(m_PropertySheetState.szRetentionPassword, L"");
			}
			else
			{
				SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_ON);
			}
			if (m_PropertySheetState.wUsePIN == TRUE)
			{
				SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_ON);
			}
			else
			{
				if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption, "QUICKFILE"))//Quick Store)
				{
					SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
				}
				else
				{
					SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_SHOW);
				}

				wcscpy(m_PropertySheetState.szRetentionPassword, L"");
			}
			// --- DM_ALWAYSUSE_PIN
			CheckDlgButton(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
			SetCheckBoxValue(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
			//<E>to Fix feedback issue #61,2021.12.15,SSDI:Chanchal Singla
		}
	}
	//<S><C>To Fix Redmine Bug #4513,2021.11.29,SSDI:Chanchal Singla
	//Doc Filing: PIN Code stores dissimilar value in different Tabs
	//SetJobControlEdit(hDlg, IDC_EDT_PIN, m_PropertySheetState.szRetentionPassword, TRUE);
	SetJobControlEdit(hDlg, IDC_EDT_PIN, m_PropertySheetState.szRetentionPassword, FALSE);
	//<E>To Fix Redmine Bug #4513,2021.11.29,SSDI:Chanchal Singla
	SetControlState(hDlg, IDC_BTN_RETENTION_I, m_ControlState.cRetention_I);
	SetControlState(hDlg, IDC_STC_RETENTION, m_ControlState.cCmbRetention);
	SetControlState(hDlg, IDC_CMB_RETENTION, m_ControlState.cCmbRetention);
	//<E> to implement restrictions with PPLCY dialog, Harika, 20201021
}
//=============================================================================
// function
//      ShowConstraintsOnRetention
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      0
//
// outline
//      ShowConstraintsOnRetention 
//=============================================================================
VOID CMainTabPropertyPage::ShowConstraintsOnFactoryDefaults(HWND hDlg, INT iCtrlID)
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
//=============================================================================
// function
//      ShowConstraintsOnStaple
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      0
//
// outline
//      ShowConstraintsOnStaple 
//=============================================================================
VOID CMainTabPropertyPage::ShowConstraintsOnStaple(HWND hDlg, INT iCtrlID)
{
	CShowMsgStaple*  wObjCStapleShowMessage = NULL;
	wObjCStapleShowMessage = new CShowMsgStaple(hDlg, m_hStringResourceHandle, m_hDialogResourceHandle,
		m_hBitmapResourceHandle, m_pPrinterName);
	if (wObjCStapleShowMessage == NULL)
	{
		return;
	}
	if (0 != GetValue(hDlg, IDC_CMB_STAPLE))
	{
		if (wObjCStapleShowMessage->IsNeverShow() == TRUE) {
			m_ControlState.cStaple_I = SCUI_STATE_ON;
		}
		if (!strcmp("STAPLE_SADDLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
		{
			SetControlState(hDlg, IDC_STC_STAPLE_POS, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_CMB_BINEDGE, SCUI_STATE_OFF);
		}
		else
		{
			SetControlState(hDlg, IDC_STC_STAPLE_POS, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_CMB_BINEDGE, SCUI_STATE_ON);
			//<S><A>To fix #2609 issue and Added the Constarints Between Duplex,Orientation and Position Feature,2020-10-2020,SSDI Chanchal Singla
			ShowConstraintsOnBinEdgePosition(hDlg, IDC_CMB_BINEDGE, IDC_STC_STAPLE_POS);
			//<E>To fix #2609 issue and Added the Constarints Between Duplex,Orientation and Position Feature,2020-10-2020,SSDI Chanchal Singla
		}
	}
	else
	{
		m_ControlState.cStaple_I = SCUI_STATE_OFF;
		SetControlState(hDlg, IDC_STC_STAPLE_POS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_BINEDGE, SCUI_STATE_OFF);

		//<S><A>to fix redmine Bug #4644,Bug #4699,29.dec.2021,SSDI:Chanchal Singla
		//Bug #4644:- Position drop down is displaying as top by default.
		//Bug #4699:- Position value fails to update when set to Top Edge in Printing Preferences
		UINT uStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
		UINT uPunch = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex;
		UINT uMargin = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex;
		if ((uStaple == staple_none) &&
			(uPunch == punchtype_none) &&
			(uMargin == 0))
		{
			if ((!strcmp("NONE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
				&& (!strcmp("TOPEDGE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE].pszOption)))
			{
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex = eFirstOption;
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BINDINGEDGE].pszOption = "LEFTEDGE";
			}
		}
		//<E>to fix redmine Bug #4644,Bug #4699,29.dec.2021,SSDI:Chanchal Singla
		if (0 == m_PropertySheetState.wStapleOption)
		{
			SetControlState(hDlg, IDC_STC_STAPLE, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_CMB_STAPLE, SCUI_STATE_SHOW);
		}
	}
	SetControlState(hDlg, IDC_BTN_STAPLE_I, m_ControlState.cStaple_I);
	if (wObjCStapleShowMessage != NULL)
	{
		delete wObjCStapleShowMessage;
		wObjCStapleShowMessage = NULL;
	}
}
//=============================================================================
// function
//       ShowConstraintsonColorModeVsMiniLineWidth
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints on ColorMode Vs Minimum Line Width
//=============================================================================
VOID CMainTabPropertyPage::ShowConstraintsonColorModeVsMiniLineWidth(HWND hDlg, INT iCtrlID)
{
	if (1 == m_PropertySheetState.wColorOption)
	{
		m_ControlState.cColMode_I = SCUI_STATE_OFF;
		m_ControlState.cCmbColorMode = SCUI_STATE_OFF;
	}
	else
	{
		//<S><C>To Fix Redmine Bug #2956, 2020.11.23, SSDI:Chanchal Singla
		if (1 == m_PropertySheetState.wSetLineToOne)
		{
			m_ControlState.cColMode_I = SCUI_STATE_ON;
			m_ControlState.cCmbColorMode = SCUI_STATE_SHOW;
		}
		else
		{
			m_ControlState.cColMode_I = SCUI_STATE_OFF;
			m_ControlState.cCmbColorMode = SCUI_STATE_ON;
		}
		//<E>To Fix Redmine Bug #2956 Issue 2, 2020.11.23, SSDI:Chanchal Singla
	}
	SetControlState(hDlg, IDC_CMB_COLMODE, m_ControlState.cCmbColorMode);
	SetControlState(hDlg, IDC_GRP_COLOR_MODE, m_ControlState.cCmbColorMode);
	SetControlState(hDlg, IDC_BTN_COLORMODE_I, m_ControlState.cColMode_I);
}
//<S><A>To Fix #3076,20201412,SSDI:Chanchal Singla
//Mimic is not displayed correctly, when N-Up is set to None in Main Tab.
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
VOID CMainTabPropertyPage::ShowConstraintsOnNUP(HWND hDlg, INT iCtrlID)
{
	UINT uChangedOptionIndex = 0;
	SHORT wOri = GetOrientation(m_pFeatureInfoList);
	UINT NupValue = atol(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption);
	if (1 != NupValue)
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
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ORDER].pszOption = "TopToBottom";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex = eThirdOption;
		}
		else if ((ori_land == wOri) && NupValue == nup_2)
		{
			if (!strcmp("RightToLeft", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ORDER].pszOption))
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ORDER].pszOption = "RightToLeft";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex = eSecondOption;
			}
			else
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ORDER].pszOption = "LeftToRight";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex = eFirstOption;
			}
		}
		else 
		{
			if (!strcmp("LeftToRight", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ORDER].pszOption))
			{
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ORDER].pszOption = "RightAndDown";
				m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex = eFourthOption;
			}
		}

		//<E>To Fix #3008,20201130,SSDI:Chanchal Singla
	}
	else
	{
		SetControlState(hDlg, IDC_STC_NUP_ORDER, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_NUP_ORDER, SCUI_STATE_OFF);

		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ORDER].pszOption = "LeftToRight";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex = eFirstOption;
		
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
//<E>To Fix #3076,20201412,SSDI:Chanchal Singla
//=============================================================================
// function
//      ShowConstraintsOnBinEdgePosition
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      0
//
// outline
//      ShowConstraintsOnBinEdgePosition 
//=============================================================================
VOID CMainTabPropertyPage::ShowConstraintsOnBinEdgePosition(HWND hDlg, SHORT iCtrlID, SHORT StrinId)
{

	//1. 2 Sided Printing Is None binding Position dropdown Should be display Left/Right/Top.
	//2. if Orientation is Landscape/LandScape(Rotated) then Binding Position dropdown Should be TOP 
	//   but if Orientation is Landscape/LandScape(Rotated) and  2 Sided Printing Is  Short Edge then binding Position dropdown Should be display Left/Right.
	//3. if Orientation is Portrait/Portrait(Rotated) then binding Position dropdown Should be display Left/Right.
	//   but if Orientation is Portrait/Portrait(Rotated) and  2 Sided Printing Is  Short Edge then binding Position dropdown Should be display Top.

	SHORT wOri = GetOrientation(m_pFeatureInfoList);

	if (!strcmp("NONE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
	{
		SetControlState(hDlg, iCtrlID, SCUI_STATE_ON);
	}
	else  if (wOri == ori_land)
	{
		if (!strcmp("HORIZONTAL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
		{
			SetControlState(hDlg, iCtrlID, SCUI_STATE_ON);
		}
		else
		{
			SetControlState(hDlg, iCtrlID, SCUI_STATE_SHOW);
			SetControlState(hDlg, StrinId, SCUI_STATE_SHOW);
		}
	}
	else if (wOri == ori_port)
	{
		if (!strcmp("HORIZONTAL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
		{
			SetControlState(hDlg, iCtrlID, SCUI_STATE_SHOW);
			SetControlState(hDlg, StrinId, SCUI_STATE_SHOW);
		}
		else
		{
			SetControlState(hDlg, iCtrlID, SCUI_STATE_ON);
		}
	}

}
//<E> To add the Show Constraints On Retention  function for Pin edit box ,06-02-2020,SSDI:Chanchal
//=============================================================================
// function
//      ShowPaperSizeDimensions
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      0
//
// outline
//      ShowPaperSizeDimensions 
//=============================================================================
VOID CMainTabPropertyPage::ShowPaperSizeDimensions(HWND hDlg, INT iCtrlID)
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

		if(iCtrlID== IDC_CMB_OUTPUTSIZE && index==0)
		{
			index = static_cast<UINT>(SendDlgItemMessage(hDlg, IDC_CMB_PAPER, CB_GETCURSEL, 0, 0));
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
		if (CUSTOM_PAPER_INDEX > index)
		{
			PaperSize = PaperSizeArray[index][0];
			Width = PaperSizeArray[index][1];
			Length = PaperSizeArray[index][2];
			unit = PaperSizeArray[index][3];
		}
		else
		{
			//<S><A> To Added Support  of Custom Paper, 24-07-2020,SSDI:Chanchal Singla
			PaperSize = DMPAPER_CUSTOM;
			//int Unit = m_pOemPrivateDevMode->scpi.ext.byCusUnits;
			USERPAPERSIZEDATA upsd;
			SecureZeroMemory(&upsd, sizeof(upsd));
			CUPSReg FAR		*pregUps = NULL;
			pregUps = new CUPSReg(m_hStringResourceHandle, m_pPrinterName);
			if (pregUps == NULL)
			{
				return;
			}
			if (iCtrlID == IDC_CMB_PAPER &&
				!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption, "CUSTOMSIZE"))
			{
				//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				///Custom Paper values are changing to Default even though cancel button is clicked
				memcpy( &upsd, &m_ppi->PaperSize_upsd, sizeof(USERPAPERSIZEDATA));
				//(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM, &upsd);
				//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				Width = upsd.dwWidth;
				Length = upsd.dwLength;
				unit = upsd.wUnit;
			}
			else if (iCtrlID == IDC_CMB_OUTPUTSIZE &&
				!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "SAOS"))
			{
				//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				///Custom Paper values are changing to Default even though cancel button is clicked
				memcpy(&upsd, &m_ppi->PaperSize_upsd, sizeof(USERPAPERSIZEDATA));
				//(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM, &upsd);
				//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				Width = upsd.dwWidth;
				Length = upsd.dwLength;
				unit = upsd.wUnit;
			}
			else if (iCtrlID == IDC_CMB_OUTPUTSIZE &&
				!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "CUSTOMSIZE"))
			{
				//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				///Custom Paper values are changing to Default even though cancel button is clicked
				memcpy(&upsd, &m_ppi->FitToPaperSize_upsd, sizeof(USERPAPERSIZEDATA));
				//(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &upsd);
				//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				Width = upsd.dwWidth;
				Length = upsd.dwLength;
				unit = upsd.wUnit;
			}
			//<S><A>To Fix Redmine #2903, 30102020,SSDI:Chanchal Singla
			//Custom paper :Output Size dimensions displays 11.69 X 17 in,
			//even though Maximum Paper Size is set to A4/Letter in Configuration tab.
			if (m_PropertySheetState.byMaxPaperSize == eSecondOption)
			{
				if (unit_mm == unit)
				{
					Width = (Width > MAXPAPER_CUSTOM_WIDTH_MM) ? MAXPAPER_CUSTOM_WIDTH_MM : Width;
					Length = (Length > MAXPAPER_CUSTOM_LENGTH_MM) ? MAXPAPER_CUSTOM_LENGTH_MM : Length;
				}
				else if (unit_inch == unit)//inch
				{
					Width = (Width > MAXPAPER_CUSTOM_WIDTH_INCH) ? MAXPAPER_CUSTOM_WIDTH_INCH : Width;
					Length = (Length > MAXPAPER_CUSTOM_LENGTH_INCH) ? MAXPAPER_CUSTOM_LENGTH_INCH : Length;
				}
			}
			//<E>To Fix Redmine #2903, 30102020,SSDI:Chanchal Singla
			if (unit_mm == unit)
			{
				unit = MM_LOMETRIC;
			}
			else//inch
			{
				unit = MM_LOENGLISH;

			}
			if (pregUps != NULL)
			{
				delete pregUps;
				pregUps = NULL;
			}
		}
		//<E> To Added Support  of Custom Paper, 24-07-2020,SSDI:Chanchal Singla
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
		if (iCtrlID==IDC_CMB_PAPER)
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
		//LoadString(m_hStringResourceHandle, IDC_STATIC_ZOOM_SIZE, szText, countof(szText));
		//LoadString(m_hStringResourceHandle, (IDS_PAPERSIZE_BASE + PaperSize), szl, countof(szl));
		//StringCbPrintf(sztemp, sizeof(sztemp), szText, szl);
		//SetDlgItemText(hDlg, IDC_STATIC_ZOOM_SIZE, sztemp);

	}
}

//=============================================================================
// function
//      UpdateEditBoxContent
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      0
//
// outline
//      UpdateEditBoxContent 
//=============================================================================

VOID CMainTabPropertyPage::UpdateEditBoxContent(
_In_ CONST HWND hDlg, _In_ CONST INT iCtrlID)
{
WCHAR* pszEditBoxContent = NULL;
//<S><D>Deleted unused code,20.03.26 SSDI:Goutham
if((IDC_CHK_PIN == iCtrlID)&& (BST_UNCHECKED == IsDlgButtonChecked(hDlg, iCtrlID)))
{
	pszEditBoxContent = m_PropertySheetState.szRetentionPassword;
	ClearEditBoxContent(IDC_EDT_PIN,hDlg,pszEditBoxContent,sizeof(m_PropertySheetState.szRetentionPassword));
}
//<E>Deleted unused code,20.03.26 SSDI:Goutham
}
//=============================================================================
// function
//      ClearEditBoxContent
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      0
//
// outline
//      ClearEditBoxContent 
//=============================================================================
VOID CMainTabPropertyPage::ClearEditBoxContent(_In_ CONST INT   iCtrlID,
	_In_ CONST HWND hDlg,
	WCHAR* pszEditBoxContent,
	size_t sEditBoxSizeInBytes)
{
	SecureZeroMemory(pszEditBoxContent, sEditBoxSizeInBytes);
	UIControlMap::const_iterator iterUIComponents;
	iterUIComponents = m_UIControls.find(iCtrlID);
	if (m_UIControls.end() != iterUIComponents)
	{
		CJobControlEdit *pJobControlEdit = static_cast<CJobControlEdit*>(iterUIComponents->second);
		pJobControlEdit->ActivateUIControl(iCtrlID, hDlg);
	}

}
//=============================================================================
// function
//      GetDecodedPassWordData
//
// parameters
//      byEncodedPasswordData				
//      pOuputPasswordData           
//		sEncodedStringLength
// return value
//      0
//
// outline
//      GetDecodedPassWordData 
//=============================================================================
void CMainTabPropertyPage::GetDecodedPassWordData(CHAR* byEncodedPasswordData, WCHAR* pOuputPasswordData, size_t sEncodedStringLength)
{
	size_t sInputStringLength = strnlen_s(byEncodedPasswordData, sEncodedStringLength);
	if (0 < sInputStringLength)
	{
		CHAR strMultiByte[MAX_ENCODING_STRING_LENGTH * 2 + 1] = { 0 };
		Decode_AES(strMultiByte, byEncodedPasswordData);
		if (MAX_ENCODING_STRING_LENGTH < sInputStringLength)
		{
			Decode_AES(strMultiByte + sizeof(char)*MAX_ENCODING_STRING_LENGTH, byEncodedPasswordData + MAX_ENCODING_STRING_LENGTH);
		}
		MultiByteToWideChar(CP_ACP, 0, strMultiByte, -1, pOuputPasswordData, MAX_ENCODING_STRING_LENGTH * 2);
	}
	else
	{
		SecureZeroMemory(pOuputPasswordData, MAX_ENCODING_STRING_LENGTH);
	}
}


// <S><A> To add Zoom Settings Dialog in UD3L, 20200408 - SSDI:Sneha TG
//=============================================================================
// function
//      OnZoomSettings
//
// return value
//     NILL
//
// outline
//   Handles Zoom Settings dialog events  .
//=============================================================================
BOOL CMainTabPropertyPage::OnZoomSettings(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
//<S><C>To add Zoom Settings  saving part to UD3L  2020.04.30, SSDI:Sneha TG
	OEMDEV temp_pOemPrivateDevMode = {};
	memcpy_s(&temp_pOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));	

	BOOL bIsUIUpdated = FALSE;
//<S><A>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
	//UpdatePropState(m_pOemPrivateDevMode, &m_PropertySheetState);
	PROPSTATE pPropertySheetState;
	SecureZeroMemory(&pPropertySheetState, sizeof(PROPSTATE));
	memcpy_s(&pPropertySheetState, sizeof(PROPSTATE), &m_PropertySheetState, sizeof(PROPSTATE));
//<E>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
//<E>To add Zoom Settings saving part to UD3L 2020.04.30, SSDI:Sneha TG
	CZoomSettingsDialog *pdlg = new CZoomSettingsDialog(IDD_DLG_ZOOM_11, m_hStringResourceHandle, m_hPrinterHandle, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings, m_hBitmapResourceHandle, m_pOemPublicDevMode, &temp_pOemPrivateDevMode, m_ObjCShowMessage, &pPropertySheetState);
	if (NULL == pdlg)
	{
		slog.putLog("CMainTabPropertyPage::OnZoomSettings -> NULL_POINTER(IDD_DLG_ZOOM_11)\n");
		return FALSE;
	}
	
	pdlg->SetPrinterName(m_pPrinterName);
	INT_PTR DlgRet = IDCANCEL;
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_ZOOM_11),
		hDlg, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
	if (IDOK == DlgRet)
	{
//<S><C>To add Zoom Settings  saving part to UD3L  2020.04.30, SSDI:Sneha TG		
		CUIControl pControl;
		
		UINT uSelectedIndex = static_cast<UINT>(SendDlgItemMessage(hDlg, IDC_CMB_ZOOM_BASING, CB_GETCURSEL, 0, 0));
		UINT uChangedComboBoxIndex = *(&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOM_BASING].uChangedOptionIndex);

		if(uChangedComboBoxIndex != uSelectedIndex)
		{
			bIsUIUpdated = TRUE;
		}

		if(FALSE == pControl.IsEqualBuf(m_pOemPrivateDevMode, &temp_pOemPrivateDevMode, sizeof(OEMDEV)))
		{
			memcpy_s(m_pOemPrivateDevMode, sizeof(OEMDEV), &temp_pOemPrivateDevMode, sizeof(OEMDEV));
			bIsUIUpdated = TRUE;
		}
//<S><A>Updating PROPSTATE variable for Reference point(Zoom Basing) and fix Zoom dialog cancel issue, 2020.09.02, SSDI:Sneha TG
		if (FALSE == pControl.IsEqualBuf(&m_PropertySheetState, &pPropertySheetState, sizeof(PROPSTATE)))
		{
			memcpy_s(&m_PropertySheetState, sizeof(PROPSTATE), &pPropertySheetState, sizeof(PROPSTATE));
			UpdatePropstateToStructure();
			bIsUIUpdated = TRUE;
		}
//<E>Updating PROPSTATE variable for Reference point(Zoom Basing) and fix Zoom dialog cancel issue, 2020.09.02, SSDI:Sneha TG
		if (TRUE == bIsUIUpdated)
		{
			pControl.EnableApplyButton(hDlg);
		}
		
//<E>To add Zoom Settings saving part to UD3L 2020.04.30, SSDI:Sneha TG

	}
	//<S><A>To Fix #3078 cancel Issue,Mimic is displayed wrong,30-11-2020,SSDI:Chanchal Singla
	else
	{
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOM_BASING].uChangedOptionIndex = m_PropertySheetState.wZoomBasing;
	}
	//<E>To Fix cancel Issue,Mimic is displayed wrong,30-11-2020,SSDI:Chanchal Singla
	delete pdlg;
	pdlg = NULL;
	return TRUE;
}
// <S><E> To add Zoom Settings Dialog in UD3L, 20200408 - SSDI:Sneha TG

// <S><A> To add Other Settings Dialog in UD3L, 20200409 - SSDI:Goutham
//=============================================================================
// function
//      OnOtherSettings
//
// return value
//     NILL
//
// outline
//   Handles Other Settings dialog events  .
//=============================================================================
BOOL CMainTabPropertyPage::OnOtherSettings(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	//PropStateToWatermarkData(&m_WaterMarkDataChanged);
	//<S><C> to Fix SSDI BUG #2255 Issue 1,23-06-2020,SSDI:Chanchal Parkash
//<S><A>Added to implement other settings saving and retrieving,2020.04.29 SSDI:Goutham
	//POEMDEV *m_ptempPrivateDevmode= &m_pPrivateDevmode;
	OEMDEV m_ptempPrivateDevmode = {};
	memcpy_s(&m_ptempPrivateDevmode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));
	//<S><E>Added to implement other settings saving and retrieving,2020.04.29 SSDI:Goutham
		//<E> to Fix SSDI BUG #2255 Issue 1,23-06-2020,SSDI:Chanchal Parkash
//<S><A>Updating PROPSTATE variable for Other Setttings dialog, 2020.10.08, SSDI:Sneha TG
	BOOL bIsUIUpdated = FALSE;
	//UpdatePropState(m_pOemPrivateDevMode, &m_PropertySheetState);
	PROPSTATE pPropertySheetState;
	SecureZeroMemory(&pPropertySheetState, sizeof(PROPSTATE));
	memcpy_s(&pPropertySheetState, sizeof(PROPSTATE), &m_PropertySheetState, sizeof(PROPSTATE));	
	
	COtherSettingsDialog *pdlg = new COtherSettingsDialog(IDD_DLG_OTHERSETTINGS_06_03, m_hStringResourceHandle, m_hPrinterHandle, 
		m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings, m_hBitmapResourceHandle, m_pOemPublicDevMode,
		&m_ptempPrivateDevmode, m_ObjCShowMessage, &pPropertySheetState, m_pPrinterName);
//<E>Updating PROPSTATE variable for Other Setttings dialog, 2020.10.08, SSDI:Sneha TG
	if (NULL == pdlg)
	{
		slog.putLog("CMainTabPropertyPage::OnZoomSettings -> NULL_POINTER(IDD_DLG_ZOOM_11)\n");
		return FALSE;
	}
	pdlg->SetPrinterName(m_pPrinterName);
	INT_PTR DlgRet = IDCANCEL;
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_OTHERSETTINGS_06_03),
		hDlg, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
	if (IDOK == DlgRet)
	{
		//<S><A>Added to implement other settings saving and retrieving,2020.04.29 SSDI:Goutham
		CUIControl pControl;
		//#2255 redmine is fixed after Updating PropState,So removing code added before.
		//As we are not updating devmode inside Other Settings dialog commenting below condition
		//if (FALSE == pControl.IsEqualBuf(m_pOemPrivateDevMode, &m_ptempPrivateDevmode, sizeof(OEMDEV)))
		//{
		//	memcpy_s(m_pOemPrivateDevMode, sizeof(OEMDEV), &m_ptempPrivateDevmode, sizeof(OEMDEV));
		//	bIsUIUpdated = TRUE;
		//}
//<S><A>Updating PROPSTATE variable for Other Setttings dialog, 2020.10.08, SSDI:Sneha TG
		if (FALSE == pControl.IsEqualBuf(&m_PropertySheetState, &pPropertySheetState, sizeof(PROPSTATE)))
		{
			memcpy_s(&m_PropertySheetState, sizeof(PROPSTATE), &pPropertySheetState, sizeof(PROPSTATE));
			UpdatePropstateToStructure();
			bIsUIUpdated = TRUE;
		}
//<E>Updating PROPSTATE variable for Other Setttings dialog, 2020.10.08, SSDI:Sneha TG
	
		//<S><A>To Fix #2914,20201102,Chanchal Singla		
		//Apply button is not enabled even though value is changed in Other Settings dialog
		if (TRUE == bIsUIUpdated)
		{
			//pControl.EnableApplyButton(hDlg);
			pControl.EnableApplyButton(hDlg);
		}
		//<E>To Fix #2914,20201102,Chanchal Singla	
		//<E> to Fix SSDI BUG #2255 Issue 1,23-06-2020,SSDI:Chanchal Parkash
//<S><E>Added to implement other settings saving and retrieving,2020.04.29 SSDI:Goutham
	}
	delete pdlg;
	pdlg = NULL;
	return TRUE;
}
// <S><E> To add Other Settings Dialog in UD3L, 20200409 - SSDI:Goutham
//=============================================================================
// function
//      SaveFeatureOptions
//
// parameters
//      Nill
//
// return value
//      0
//
// outline
//      SaveFeatureOptions 
//=============================================================================
//<S><A> Bug 2029 - Issue1 - 20200629 - SSDI:Seetharam
VOID CMainTabPropertyPage::SaveFeatureOptions()
{
	PropStateToJobControlData();
	__super::SaveFeatureOptions();
}
//<S><E> Bug 2029 - Issue1 - 20200629 - SSDI:Seetharam

//<S><A> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
//=============================================================================
// function
//      PropStateToJobControlData
//
// return value
//     NILL
//
// outline
//   Updates Job control data structure  based on Property sheet structure.
//=============================================================================
VOID CMainTabPropertyPage::PropStateToJobControlData()
{

	// --- DM_DOCFILING
	m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wRetention == bool_true) ?
		(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_DOCFILING) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_DOCFILING);


  /*memcpy_s(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname) * sizeof(WCHAR),
		m_PropertySheetState.szJobName, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname) * sizeof(WCHAR));

	memcpy_s(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName) * sizeof(WCHAR),
		m_PropertySheetState.szLoginNameUA, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName) * sizeof(WCHAR));
	memcpy_s(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername) * sizeof(WCHAR),
		m_PropertySheetState.szUserName, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername) * sizeof(WCHAR));

	GetEncodedPassWordData((CHAR*)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byUserNumber,
		m_PropertySheetState.szUserNumber,
		JC_STR_PINLENGTH,
		USER_NUMBER_LENGTH
	);*/
	//Fix Coverity #91523,15.09.2021,SSDI:Chanchal Singla
	GetEncodedPassWordData((CHAR*)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetentionPassword,
		m_PropertySheetState.szRetentionPassword,
		JC_STR_PINLENGTH,
		RETENTION_PASSWORD_LENGTH
	);
//	GetEncodedPassWordData((CHAR*)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byLoginPassword,
	//	m_PropertySheetState.szLoginPasswordUA,
	//	JC_STR_LOGINPASS,
	//	LOGIN_PASSWORD_LENGTH
//	);
}

//=============================================================================
// function
//      JobControlDataToPropState
//
// return value
//     VOID
//
// outline
//    //   Updates Property sheet structure based on Job control data structure.
//=============================================================================
VOID CMainTabPropertyPage::JobControlDataToPropState()
{
	//<S><A>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	memcpy(&m_PropertySheetState,m_ppi->pps,sizeof(PROPSTATE));
	//<E>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
	{
		if ((m_PropertySheetState.wLoginName == bool_true) && (m_PropertySheetState.wLoginPass == bool_true)
			&& (m_PropertySheetState.wUseAcNum == bool_false) && (m_PropertySheetState.wLoginNameSSO == bool_false))
		{
			m_PropertySheetState.wLoginName = bool_true;
			m_PropertySheetState.wLoginPass = bool_true;
			m_PropertySheetState.wUseAcNum = bool_false;
			m_PropertySheetState.wLoginNameSSO = bool_false;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "LoginPass";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eThirdOption;
		}
		else if ((m_PropertySheetState.wLoginName == bool_false) && (m_PropertySheetState.wLoginPass == bool_false)
			&& (m_PropertySheetState.wUseAcNum == bool_false) && (m_PropertySheetState.wLoginNameSSO == bool_true))
		{
			m_PropertySheetState.wLoginName = bool_false;
			m_PropertySheetState.wLoginPass = bool_false;
			m_PropertySheetState.wUseAcNum = bool_false;
			m_PropertySheetState.wLoginNameSSO = bool_true;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "LoginNameSSO";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eFifthOption;
		}
		else
		{
			m_PropertySheetState.wLoginName = bool_true;
			m_PropertySheetState.wLoginPass = bool_false;
			m_PropertySheetState.wUseAcNum = bool_false;
			m_PropertySheetState.wLoginNameSSO = bool_false;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "LoginName";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eSecondOption;
		}
	}
	if (m_PropertySheetState.wPPlcyUserAuth == bool_true)
	{
		m_PropertySheetState.wLoginName = bool_false;
		m_PropertySheetState.wLoginPass = bool_false;
		m_PropertySheetState.wUseAcNum = bool_false;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "None";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eFirstOption;
		if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
		{
			m_PropertySheetState.wLoginName = bool_true;
			m_PropertySheetState.wLoginPass = bool_false;
			m_PropertySheetState.wUseAcNum = bool_false;
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "LoginName";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eSecondOption;
		}
		m_PropertySheetState.wLoginNameSSO = bool_false;
	}

	UINT Authentication = SelectAuthentication(m_PropertySheetState.wLoginName, m_PropertySheetState.wLoginPass, m_PropertySheetState.wUseAcNum, m_PropertySheetState.wLoginNameSSO);
	if (authentication_none == Authentication)
	{
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption = "None";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex = eFirstOption;
	}


}

//=============================================================================
// function
//      GetEncodedPassWordData
//
// return value
//     NILL
//
// outline
//   Gets AES encrypted password data.
//=============================================================================
void CMainTabPropertyPage::GetEncodedPassWordData(CHAR* byEncodedPasswordData, WCHAR* pInputPasswordData, size_t sMaximumInputStringLength, size_t sOutputStringLength)
{
	size_t sInputStringLength = wcsnlen_s(pInputPasswordData, sMaximumInputStringLength);
	if (0 < sInputStringLength)
	{
		CHAR strMultiByte[MAX_ENCODING_STRING_LENGTH * 2 + 1] = { 0 };
		WideCharToMultiByte(CP_ACP, 0, pInputPasswordData, -1, strMultiByte, MAX_ENCODING_STRING_LENGTH * 2 + 1, NULL, NULL);
		Encode_AES(byEncodedPasswordData, strMultiByte);
		if (MAX_ENCODING_STRING_LENGTH < sInputStringLength)
		{
			Encode_AES(byEncodedPasswordData + MAX_ENCODING_STRING_LENGTH, strMultiByte + sizeof(char)*MAX_ENCODING_STRING_LENGTH);
		}
	}
	else
	{
		SecureZeroMemory(byEncodedPasswordData, sOutputStringLength);
	}

}
//=============================================================================
// function
//      OnShowHideTab
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      Bool				bShow
//
// return value
//      0
//
// outline
//      OnShowHideTab 
//=============================================================================
BOOL CMainTabPropertyPage::OnShowHideTab(HWND hWnd, BOOL bShow)
{
	if (bShow)
	{
		JobControlDataToPropState();

		UIControlMap::const_iterator iComboIterator = m_UIControls.find(IDC_EDT_PIN);
		if (iComboIterator != m_UIControls.end())
		{
			CJobControlEdit * pControl = static_cast<CJobControlEdit*>(iComboIterator->second);
			if (pControl != NULL)
			{
				pControl->setVal(m_PropertySheetState.szRetentionPassword);
			}
		}
	}
	else
	{
		PropStateToJobControlData();
	}
	return TRUE;
}
//<S><E> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
//=============================================================================
// function
//       CustomPaperSetTabDefaults
//
// parameters
//
// return value
//       TRUE
//      FALSE
//
// outline
//        CustomPaper SetTabDefaults
//=============================================================================
HRESULT  CMainTabPropertyPage::CustomPaperSetTabDefaults(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	USERPAPERSIZEDATA upsd;
	SecureZeroMemory(&upsd, sizeof(upsd));
	CUPSReg FAR		*pregUps = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
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
	//(*pregUps).resetEx(m_pPrinterName, DMPAPER_CUSTOM);
	//(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM, &upsd);
	if ((*m_pmcf).IsWriteToJson() == TRUE)
		(*pjsonups).GetUPSDefData(&upsd);
	else
		(*pregUps).GetUPSDefData(&upsd);

	 memcpy(&m_ppi->PaperSize_upsd, &upsd, sizeof(USERPAPERSIZEDATA));
//<S><A>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application	 
	 m_PropertySheetState.dwCustomWidth = upsd.dwWidth;
	 m_PropertySheetState.dwCustomLength = upsd.dwLength;
	 m_PropertySheetState.wCusUnit = upsd.wUnit;
//<S><A>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla

	//<S><D> To fix bug no. 2575, 2020/09/10 Harika K M
	//(*pregUps).resetEx(m_pPrinterName, DMPAPER_CUSTOM_ZOOM);
	//<E> To fix bug no. 2575, 2020/09/10 Harika K M
	//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
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
	return S_OK;
}
//<S><A>To Fix Redmine #2299,17-08-2020,SSDI:Chanchal Singla
//Zoom check box is not unchecked when Defaults is clicked in Paper Source tab
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
VOID CMainTabPropertyPage::ShowConstraintsOnZoomSetting(HWND hDlg)
{
	BOOL blChecked = IsDlgButtonChecked(hDlg, IDC_CHK_ZOOM);
	if (FALSE != blChecked)
	{
		m_ControlState.cBtnZoom = SCUI_STATE_ON;
		m_ControlState.cCmbNup = SCUI_STATE_SHOW;
		m_ControlState.cNup_I = SCUI_STATE_ON;

	}
	else
	{
		m_ControlState.cBtnZoom = SCUI_STATE_SHOW;
		m_ControlState.cCmbNup = SCUI_STATE_ON;
		m_ControlState.cNup_I = SCUI_STATE_OFF;
	}
	SetControlState(hDlg, IDC_BTN_ZOOM, m_ControlState.cBtnZoom);
	SetControlState(hDlg, IDC_STC_NUP, m_ControlState.cCmbNup);
	SetControlState(hDlg, IDC_CMB_NUP, m_ControlState.cCmbNup);
	SetControlState(hDlg, IDC_BTN_NUP_I, m_ControlState.cNup_I);

}
//<E>To Fix Redmine #2299,17-08-2020,SSDI:Chanchal Singla
//=============================================================================
// function
//      ShowConstraintsOnBooklet
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      0
//
// outline
//      ShowConstraintsOnBooklet 
//=============================================================================
VOID CMainTabPropertyPage::ShowConstraintsOnBooklet(HWND hDlg)
{
	m_PropertySheetState.wStaple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{

		m_ControlState.cChkZoom = SCUI_STATE_SHOW;
		m_ControlState.cZoom_I = SCUI_STATE_ON;
		m_ControlState.cDuplex_I = SCUI_STATE_ON;
		m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
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
		UINT NupValue = atol(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption);
		if ((NupValue != TRUE)
			|| (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption, "CUSTOMSIZE")))
		{
			m_ControlState.cChkZoom = SCUI_STATE_SHOW;
			m_ControlState.cZoom_I = SCUI_STATE_ON;
		}
		else
		{
			m_ControlState.cChkZoom = SCUI_STATE_ON;
			m_ControlState.cZoom_I = SCUI_STATE_OFF;
		}
	}

	SetControlState(hDlg, IDC_CHK_ZOOM, m_ControlState.cChkZoom);
	SetControlState(hDlg, IDC_STC_ZOOM, m_ControlState.cChkZoom);
	SetControlState(hDlg, IDC_BTN_ZOOM_I, m_ControlState.cZoom_I);
	SetControlState(hDlg, IDC_STC_DUPLEX, m_ControlState.cCmbDuplex);
	SetControlState(hDlg, IDC_CMB_DUPLEX, m_ControlState.cCmbDuplex);
	SetControlState(hDlg, IDC_BTN_DUPLEX_I, m_ControlState.cDuplex_I);
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
VOID CMainTabPropertyPage::ShowConstraintsOnDuplexVsOutputSize(HWND hDlg, INT iCtrlID)
{
	long wDuplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
	LONG wOutputSize = GetValue(hDlg, IDC_CMB_OUTPUTSIZE);
	LONG wPaperSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	LONG wBooklet = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex;
	m_ControlState.cPaper_I = SCUI_STATE_OFF;
	m_ControlState.cOutputSize_I = SCUI_STATE_OFF;
	if (eFirstOption == wDuplex)
	{

		if (wOutputSize >= 9 || ((wPaperSize >= 8) && (wOutputSize == 0)) && (wBooklet == 0))
		{

			if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
			{
				m_ControlState.cDuplex_I = SCUI_STATE_ON;
				m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
			}
			else
			{
				m_ControlState.cDuplex_I = SCUI_STATE_ON;
				m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
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
				if(((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption, "CUSTOMSIZE"))&&(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "SAOS"))&&((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "LABEL"))||(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "ENVELOPE"))||(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "TRANSPARENCY1"))))
					||((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "CUSTOMSIZE"))&& ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "LABEL"))|| (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "ENVELOPE"))|| (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "TRANSPARENCY1")))))
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
			}
			else
			{
				//<S><C>To Fix Redmine Bug #6161,05-04-2023,SSDI:Manoj Sunagar
				//Bug #6161 2-Sided printing drop down is grayed out even though Output Size is set to default value
				if ((!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "LABEL")
					|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "TRANSPARENCY1")
					/*|| !strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption, "ENVELOPE")*/)
					&& (wBooklet == 0)
					&& (wDuplex == 0))
				//<E>To Fix Redmine Bug #6161,05-04-2023,SSDI:Manoj Sunagar
				{
					m_ControlState.cDuplex_I = SCUI_STATE_ON;
					m_ControlState.cCmbDuplex = SCUI_STATE_SHOW;
				}
				else
				{
					m_ControlState.cDuplex_I = SCUI_STATE_OFF;
					m_ControlState.cCmbDuplex = SCUI_STATE_ON;
				}
			}
		}
		SetControlState(hDlg, IDC_STC_DUPLEX, m_ControlState.cCmbDuplex);
		SetControlState(hDlg, IDC_CMB_DUPLEX, m_ControlState.cCmbDuplex);
		SetControlState(hDlg, IDC_BTN_DUPLEX_I, m_ControlState.cDuplex_I);

	}
	//<S><C>Fix for 5521 issue, Harika, 20220902
	else
	{	//<S><C>Removed the constraints between Duplex and Custom Paper, 26-12-2022, SSDI:Manoj Sunagar
		if (((wPaperSize >= 8) && (wOutputSize == 0)) && ((wBooklet == 0) && (wPaperSize < 20)))
		//<E>Removed the constraints between Duplex and Custom Paper, 26-12-2022, SSDI:Manoj Sunagar
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
	//<E>Fix for 5521 issue, Harika, 20220902
	SetControlState(hDlg, IDC_BTN_OUTPUTSIZE_I, m_ControlState.cOutputSize_I);
	SetControlState(hDlg, IDC_BTN_PAPER_I, m_ControlState.cPaper_I);
}
//S><A>To Add the Support of Booklet Standard Output Size ,2020-09-02,SSDI:Chanchal Singla
//=============================================================================
// function
//       SetTabState
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//      Handle the control When Change Any Feature 
//=============================================================================
BOOL CMainTabPropertyPage::SetTabState(_In_ CONST HWND   hDlg, _In_ CONST INT iCtrlID)
{
	
	HRESULT hr = S_OK;
    
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	SetUserSettingState(hDlg, IDC_CMB_SHARED_USRSET);
	//<S><A>To Add the Support of Booklet Standard Output Size and fix SSDI #Bug 2067,2020-09-02,SSDI:Chanchal Singla
	// Initialize the Booklet and FitToPaper Size Drop down 
	// When Booklet is Standard then Only Booklet Supported Output Size Paper Should be Initialize in DropDown
	BookletVsFitToPaperSizeInitialization(hDlg, IDC_CMB_OUTPUTSIZE);
	AutoColorModeVsColorModeInitialization(hDlg, IDC_CMB_COLMODE);
	//<S><A>To fix #2609 issue and Added the Constarints Between Duplex,Orientation and Position Feature,2020-10-2020,SSDI Chanchal Singla
	// ---Feature Staple
	ModeLimit(hDlg, IDC_CMB_STAPLE, FEATURE_STAPLING);
	long Staple = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_STAPLE, Staple);
	/*SetControlState(hDlg, IDC_BTN_OUTPUTSIZE_I, SCUI_STATE_OFF);
	SetControlState(hDlg, IDC_BTN_STAPLE_I, SCUI_STATE_OFF);*/
		// ---Feature Binding Edge
	long Bindedge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_BINEDGE, Bindedge);
	//<S><E>To fix #2609 issue and Added the Constarints Between Duplex,Orientation and Position Feature,2020-10-2020,SSDI Chanchal Singla
	// ---FEATURE collate
	long wCollate = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_COLLATE, wCollate);
	// ---Feature PAPER SIZE
	long wPaper = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_PAPER, wPaper);
	// ---FEATURE OREINTATION
	long wOrient = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OREINTATION].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_ORIENT, wOrient);
	// ---FEATURE PAGESPERSHEET(NUP)
	long wNUP = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_NUP, wNUP);
	//FEATURE_RETENTION_SETTINGS
	long wRetention = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_RETENTION, wRetention);

	LONG wZoom = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOMSETTINGS].uChangedOptionIndex;;
	CheckDlgButton(hDlg, IDC_CHK_ZOOM, wZoom);
	SetCheckBoxValue(hDlg, IDC_CHK_ZOOM, wZoom);
	
	//<S><D>To Fix Redmine #7347,21-06-2024,SSDI:Manoj S
	//<S><A>To Fix Redmine #7343,15-05-2024,SSDI:Manoj S
	//Paper Tray sets to "Auto Select" automatically, while changing Staple value.
	BYTE Flag = 0;
	//short wdefaultstaple = (short)m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uDefaultOptionIndex;
	//<S><A>To Fix Redmine #7347,21-06-2024,SSDI:Manoj S
	if (m_PropertySheetState.byStapleless && ReadStapleLessFlag(Flag))//&&(defaultstapleval ==3))
	{
		if (!strcmp("STAPLE_STAPLELESS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption) &&
			!strcmp("FORMSOURCE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption = "BYPASSTRAY";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex = PAPERSRC_BypassTray;
		}
	}
	//<E>To Fix Redmine #7514,31-05-2024,SSDI:Manoj S
	//<E>To Fix Redmine #7343,15-05-2024,SSDI:Manoj S
	//<E>To Fix Redmine #7347,21-06-2024,SSDI:Manoj S
	//<S><D>To Fix feedback #49,2021.08.01,SSDI:Chanchal Singla
	//deleted The Code To Fix feeedback 49 and 
	//that code has moved in OnUsSelect() Function under documentpropertypage.cpp file
	// --- copies
	/*DWORD wPos = m_pOemPrivateDevMode->scpi.pub.dmCopies;
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

	}*/
	//<E>To Fix feedback #49,2021.08.01,SSDI:Chanchal Singla
	ShowPaperSizeDimensions(hDlg, IDC_CMB_PAPER);
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		ShowBookletPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	}
	else
	{
		ShowPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	}
	//<E>To Add the Support of Booklet Standard Output Size and fix SSDI #Bug 2067,2020-09-02,SSDI:Chanchal Singla
	/*<S><A>Show Constraints on Retention combo box for pin edit box,06-02-2020:SSDI:Chanchal*/
	//if (IDC_CMB_RETENTION == iCtrlID)
	{
		ShowConstraintsOnRetention(hDlg, IDC_CMB_RETENTION);
	}
	
	ShowConstraintsOnStaple(hDlg, IDC_CMB_STAPLE);
	/*<E>Show Constraints on Retention combo box for pin edit box, 06 - 02 - 2020:SSDI:Chanchal*/
	ShowConstraintsOnBooklet(hDlg);
	ShowConstraintsOnDuplexVsOutputSize(hDlg, IDC_CMB_OUTPUTSIZE);
	ShowFinishingInformationIcon(hDlg);
	ShowConstraintsOnCollate(hDlg, IDC_CMB_COLLATE);
	//<S><A>To Fix Redmine #2299,17-08-2020,SSDI:Chanchal Singla
	//Zoom check box is not unchecked when Defaults is clicked in Paper Source tab
	ShowConstraintsOnZoomSetting(hDlg);
	//<E>To Fix Redmine #2299,17-08-2020,SSDI:Chanchal Singla

	ShowConstraintsOnNUP(hDlg, IDC_CMB_NUP);
// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	ShowConstraintsonColorModeVsMiniLineWidth(hDlg, IDC_CMB_COLMODE);
// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	ShowStapleVsOriginalSizeVsOutputSizeInformationIcon(hDlg);
	ShowStapleVsMarginShiftInformationIcon(hDlg);
	ShowStapleVsPaperTypeInformationIcon(hDlg);
	ShowPunchVsOriginalSizeVsOutputSizeInformationIcon(hDlg);
	//<S><A>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	memcpy( m_ppi->pps, &m_PropertySheetState,sizeof(PROPSTATE));
	//<E>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla

	//if(IsPrinterOperators(m_pPrinterName) == FALSE)
	//	DisableAllCtrls();

	return TRUE;
}

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
VOID CMainTabPropertyPage::ShowBookletPaperSizeDimensions(HWND hDlg, INT iCtrlID)
{
	UIControlMap::const_iterator Iterator = m_UIControls.find(iCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pCombo = static_cast<CShImageCombo*>(Iterator->second);
		INT Windex = pCombo->GetSelectedIndex();
		INT index = 0;

		// Update the PaperSize Array index based on booklet drop down
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
//=============================================================================
// function
//       BookletVsFitToPaperSizeInitialization
//
// parameters
//			HWND  hDlg			Handler to window
//			INT   iCtrlID		Control ID
// return value
//      VOID
//
// outline
//       Booklet or FitToPaperSize Initialization
//=============================================================================
VOID CMainTabPropertyPage::BookletVsFitToPaperSizeInitialization(HWND hDlg, INT iCtrlID)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CUIControl* pControl;
	std::string strFeatureName;
	PRESOURCEIDINFO pResourceIDInfo = NULL;
	// OutputSize Feature initialize
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		//<S><C> To support maximum paper size, 2020.10.09, SSDI: Chanchal Singla
		ModeLimit(hDlg, IDC_CMB_OUTPUTSIZE, FEATURE_BOOKLET_OUTPUTSIZE);
		//<S><A> Deleted the existing control before creating new Control. Harika, 20230405
		UIControlMap::const_iterator Iterator = m_UIControls.find(iCtrlID);
		if (Iterator != m_UIControls.end())
		{
			CShImageCombo *pComboOutputSize = static_cast<CShImageCombo*>(Iterator->second);
			if (pComboOutputSize)
			{
				m_UIControls.erase(Iterator);	//Control will be deleted
				delete pComboOutputSize;
				pComboOutputSize = NULL;
			}
		}
		//<E> Deleted the existing control before creating new Control. Harika, 20230405
		//<S><A>To Fix #2707 Issue 2,2020-09-09,SSDI:Chanchal Singla
		UINT BookletStandardPSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex;
		SetDefaultsForBooklet(hDlg, BookletStandardPSize);
		//<E>To Fix #2707 Issue 2,2020-09-09,SSDI:Chanchal Singla
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
		pControl->OnInit(IDC_CMB_OUTPUTSIZE, hDlg);
		((CShImageCombo*)pControl)->Select(IDC_CMB_OUTPUTSIZE, hDlg, BookletStandardPSize);
		((CShImageCombo*)pControl)->SelectFeatureOption(IDC_CMB_OUTPUTSIZE, hDlg);
	}
	else
	{
		//<S><C> To support maximum paper size, 2020.10.09, SSDI: Chanchal Singla
		ModeLimit(hDlg, IDC_CMB_OUTPUTSIZE, FEATURE_FITTOPAPERSIZE);
		//<S><A> Deleted the existing control before creating new Control. Harika, 20230405
		UIControlMap::const_iterator Iterator = m_UIControls.find(iCtrlID);
		if (Iterator != m_UIControls.end())
		{
			CShImageCombo *pComboOutputSize = static_cast<CShImageCombo*>(Iterator->second);
			if (pComboOutputSize)
			{
				m_UIControls.erase(Iterator);	//Control will be deleted
				delete pComboOutputSize;
				pComboOutputSize = NULL;
			}
		}
		//<E> Deleted the existing control before creating new Control. Harika, 20230405
		pResourceIDInfo = GetResourceIDInfoOfControl(FITTOPAPER);
		pControl = new  CShImageCombo(
			pResourceIDInfo,
			&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE],
			&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE],
			m_pFeatureInfoList,
			m_hStringResourceHandle,
			m_hDialogResourceHandle,
			m_hBitmapResourceHandle,
			m_DefaultSettings[FEATURE_FITTOPAPERSIZE],
			m_ObjCShowMessage,
			IDC_CMB_OUTPUTSIZE,
			m_PropertySheetState
		);
		//<E> To support maximum paper size, 2020.10.09, SSDI: Chanchal Singla
		UINT FitToPaperSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
		if (NULL != pControl)
		{
			hr = AddUIControl(IDC_CMB_OUTPUTSIZE, pControl);
			if (S_OK != hr)
			{
				slog.LoggingErrorHResultValues();
			}
			((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
		}
		pControl->OnInit(IDC_CMB_OUTPUTSIZE, hDlg);
		((CShImageCombo*)pControl)->Select(IDC_CMB_OUTPUTSIZE, hDlg, FitToPaperSize);
		((CShImageCombo*)pControl)->SelectFeatureOption(IDC_CMB_OUTPUTSIZE, hDlg);
	}
	//Duplex
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		m_DefaultSettings[FEATURE_DUPLEX].ResourceID = IDS_IDC_CMB_DUPLEX_1S;
	}
	else
	{
		//<S><C>To Fix Feedback 23,2021-02-24,SSDI:Chanchal Singla
		if (m_DefaultSettings[FEATURE_DUPLEX].iDevmodeValue == DMDUP_SIMPLEX)
		{
			m_DefaultSettings[FEATURE_DUPLEX].ResourceID = IDS_IDC_CMB_DUPLEX_1S;
		}
		else 	if (m_DefaultSettings[FEATURE_DUPLEX].iDevmodeValue == DMDUP_VERTICAL)
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
	UINT Duplex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex;
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
	((CShImageCombo*)pControl)->Select(IDC_CMB_DUPLEX, hDlg, Duplex);
	((CShImageCombo*)pControl)->SelectFeatureOption(IDC_CMB_DUPLEX, hDlg);
}

//=============================================================================
// function
//       AutoColorModeVsColorModeInitialization
//
// parameters
//			HWND  hDlg			Handler to window
//			INT   iCtrlID		Control ID
// return value
//      VOID
//
// outline
//       AutoColorMode and ColorMode functionality
//=============================================================================
VOID CMainTabPropertyPage::AutoColorModeVsColorModeInitialization(HWND hDlg, INT iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap::const_iterator Iterator = m_UIControls.find(iCtrlID);
	long ColorMode = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex;
	if (Iterator != m_UIControls.end())
	{
		CShComboColors *pComboColors = static_cast<CShComboColors*>(Iterator->second);
		if (pComboColors)
		{
			pComboColors->OnInit(iCtrlID, hDlg);
			pComboColors->Select(iCtrlID, hDlg, ColorMode);
			pComboColors->SelectFeatureOption(iCtrlID, hDlg);
		}
		else
		{
			slog.putLog("CMainTabPropertyPage::Select -> NULL_OBJECT(pComboColors)\n");
		}
	}
}
//=============================================================================
// function
//       SetDefaultsForBooklet
//
// parameters
//			HWND	hDlg						Handler to window
//			UINT	BookletStandardPSize		Booklet Index
// return value
//      VOID
//
// outline
//      Set Defaults For Booklet
//=============================================================================
VOID CMainTabPropertyPage::SetDefaultsForBooklet(HWND hDlg, UINT BookletStandardPSize)
{
	//UINT uCurrentItemId;
	//Coverity Fix 90607 - SSDI:Seetharam - 20200928
	UINT uCurrentItemId = IDS_PAPERSIZE_9;
	switch (BookletStandardPSize)
	{
	case BOOKLET_A3_INDEX:
		uCurrentItemId = IDS_PAPERSIZE_8;
		break;
	case BOOKLET_A4_INDEX:
		uCurrentItemId = IDS_PAPERSIZE_9;
		break;
	case BOOKLET_B4_INDEX:
		uCurrentItemId = IDS_PAPERSIZE_12;
		break;
	case BOOKLET_LEDGER_INDEX:
		uCurrentItemId = IDS_PAPERSIZE_3;
		break;
	case BOOKLET_LETTER_INDEX:
		uCurrentItemId = IDS_PAPERSIZE_1;
		break;
	case BOOKLET_LEGAL_INDEX:
		uCurrentItemId = IDS_PAPERSIZE_5;
		break;
	default:
		break;
	}
	m_DefaultSettings[FEATURE_BOOKLET_OUTPUTSIZE].ResourceID = uCurrentItemId;
}
//<E>To Add the Support of Booklet Standard Output Size ,2020-09-02,SSDI:Chanchal Singla
//<S><A>Updating PROPSTATE variable for Other Setttings dialog and to fix redmine #2239 issue, 2020.10.08, SSDI:Sneha TG
//=============================================================================
// function
//       OtherSettingsSetTabDefaults
//
// parameters
//			HWND	hDlg						Handler to window
// return value
//      void
//
// outline
//      Set Defaults For Other Settings dialog
//=============================================================================
void CMainTabPropertyPage::OtherSettingsSetTabDefaults(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CUIControl pControl;
	BOOL bIsUIUpdated = FALSE;
	OEMDEV temp_pOemPrivateDevMode = {};
	PROPSTATE temp_pPropertySheetState;
	SecureZeroMemory(&temp_pPropertySheetState, sizeof(PROPSTATE));
	memcpy_s(&temp_pPropertySheetState, sizeof(PROPSTATE), &m_PropertySheetState, sizeof(PROPSTATE));

	COtherSettingsDialog *pdlgOther = new COtherSettingsDialog(IDD_DLG_OTHERSETTINGS_06_03, m_hStringResourceHandle, m_hPrinterHandle,
		m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings, m_hBitmapResourceHandle, m_pOemPublicDevMode,
		&temp_pOemPrivateDevMode, m_ObjCShowMessage, &temp_pPropertySheetState, m_pPrinterName);
	if (NULL == pdlgOther)
	{
		slog.putLog("COtherSettingsDialog::SetTabDefaults->NULL_POINTER(CShImageCombo)\n");
	}

	pdlgOther->CreateControlClass();
	pdlgOther->SetTabDefaults(hDlg);

	if (FALSE == pControl.IsEqualBuf(&m_PropertySheetState, &temp_pPropertySheetState, sizeof(PROPSTATE)))
	{
		memcpy_s(&m_PropertySheetState, sizeof(PROPSTATE), &temp_pPropertySheetState, sizeof(PROPSTATE));
		bIsUIUpdated = TRUE;
	}

	if (TRUE == bIsUIUpdated)
	{
		pControl.EnableApplyButton(hDlg);
	}
	delete pdlgOther;
	pdlgOther = NULL;
}
//<E>Updating PROPSTATE variable for Other Setttings dialog and to fix redmine #2239 issue, 2020.10.08, SSDI:Sneha TG
//=============================================================================
// function
//        OnPsnKillActive
//
// parameters
//        hWnd        
//
// return value
//        TRUE
//         FALSE
//
// outline
//        WM_NOTIFY - PSN_KILLACTIVE
//=============================================================================
LRESULT CMainTabPropertyPage::OnPsnKillActiveEX(HWND hWnd)
{
	LRESULT			lRes = FALSE;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CShowJobHandlingMessage	*wObjShowJobHandlingMessage = new CShowJobHandlingMessage(hWnd, m_hStringResourceHandle);
	if (NULL == wObjShowJobHandlingMessage)
	{
		slog.putLog("CMainTabPropertyPage::CreateMessageClass -> NULL_POINTER(m_ObjShowJobHandlingMessage)\n");
	}

	if (wObjShowJobHandlingMessage->MessageForMainTab(hWnd, &m_PropertySheetState) == 0) {

		lRes = TRUE;
	}

	if(wObjShowJobHandlingMessage != NULL)
	{ 
		delete wObjShowJobHandlingMessage;
		wObjShowJobHandlingMessage = NULL;
	}
	return lRes;
}

//=============================================================================
// function
//        OnPsnApply
//
// parameters
//        hWnd        
//        wCtrlID      
//        pnmh         
//
// return value
//       TRUE
//       FALSE
//
// outline
//        WM_COMMAND - BN_CLICKED
//=============================================================================
LRESULT CMainTabPropertyPage::OnPsnApplyEX(HWND hWnd, long lCtrlID, LPPSHNOTIFY ppsn)
{
	LRESULT			res = 0;
	PREGJCINFO		pregjc = NULL;

	CShRegJC		*preg = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	CShJsonJC	*pjsonjc = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonjc = new CShJsonJC(ghInstance, m_pPrinterName);
		pjsonjc->Init();
	}

	memcpy( &m_PropertySheetState, m_ppi->pps, sizeof(PROPSTATE));
	pregjc = new REGJCINFO;
	if (pregjc == NULL)
		goto EXIT;

	preg = new CShRegJC(m_hStringResourceHandle, m_pPrinterName);
	if (preg == NULL)
		goto EXIT;

	SecureZeroMemory(pregjc, sizeof(REGJCINFO));

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		if (PropStateToRegJCInfo(&m_PropertySheetState, pregjc, m_pPrinterName) != FALSE)
			(*pjsonjc).WriteJCData(pregjc, JC_ALL, m_PropertySheetState.wPPlcyWinLogin);
	}
	else
	{
		if (PropStateToRegJCInfo(&m_PropertySheetState, pregjc, m_pPrinterName) != FALSE)
			(*preg).WriteJCData(m_pPrinterName, pregjc, JC_ALL, m_PropertySheetState.wPPlcyWinLogin);
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonjc->WriteJsonDataToFile();
	}
	
EXIT:
	if (pregjc != NULL)
	{
		delete pregjc;
		pregjc = NULL;
	}
	if (preg != NULL)
	{
		delete preg;
		preg = NULL;
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

	return res;
}

//<S><A>to Fix Bug #2738 issue 2,2021.Dec.10,SSDI:Chanchal Singla	
//Printing Preferences : Tab is not highlighted with blue Color when Clicked
//Issue 2: Copies text box number is not highlighted with blue Color when Clicked
//=============================================================================
// function
//      OnEnSetFocus
//
// return value
//     HRESULT
//
// outline
//    OnEnSetFocus
//=============================================================================
HRESULT CMainTabPropertyPage::OnEnSetFocus(_In_ CONST HWND   hDlg, _In_ CONST WPARAM wParam)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	short				wCtrlID = (short)wParam;
	UIControlMap::const_iterator Iterator = m_UIControls.find(wCtrlID);

	switch (wCtrlID)
	{
	case IDC_EDT_COPIES:
	{
		if (Iterator != m_UIControls.end())
		{
			CShEditSpin *pEditSpin = static_cast<CShEditSpin*>(Iterator->second);
			if (pEditSpin)
			{
				pEditSpin->setsel(hDlg, wCtrlID, 0, -1);
			}
			else
			{
				slog.putLog("CMainTabPropertyPage::Select -> NULL_OBJECT(pEditSpin)\n");
			}

		}
	}
	break;
	default:
		//return CDocumentPropertyPage::OnEnSetFocus(hDlg, wParam);
		break;
	}
	//To Fix Coverity #91599,2021.12.15,SSDI:Chanchal Singla
	return S_OK;
}
//<E>to Fix Bug #2738,2021.Dec.10,SSDI:Chanchal Singla