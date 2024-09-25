//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : PaperTabPropertyPage.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 02 November 2013
//
//	Description   : This class handles Property sheet dialog functions  for Paper tab
//===========================================================================
#include "precomp.h"
//For Logging Purpose
#include "MFPLogger.h"
#include "shjsonups.h"
#include <windowsx.h>

#define ENVELOPE_PAPER_TYPE_ID 4268
//=============================================================================
// function
//      CPaperTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Constructor to the class CPaperTabPropertyPage.
//=============================================================================
CPaperTabPropertyPage::CPaperTabPropertyPage(HMODULE hStringResourceHandle,HMODULE hBitmapResourceHandle,HMODULE hDialogResourceHandle,IPrintCoreHelperUni* pPrintCoreHelperUni,ResourceIDInfoMap* pResourceIDInfoMap,ConstraintFeatureInfoMap &ConstraintFeatureInfoMap,PFEATUREINFOLIST &pFeatureInfoList,PaperInfoMap PaperInfo, CustomPaperInfoMap CustomPaperInfoList,POEMDEV pOemPrivateDevMode,
											 PDEVMODE pOemPublicDevMode,LPTSTR pHelpFilePath,DWORD * pHelpID,std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings,POEMDEV pPrivateDevmode,HANDLE hPrinter,
        WCHAR* pPrinterName,SHORT& sWaterMarkIndex, SHORT& sUsIndex, PPROPINFO ppi):CDocumentPropertyPage(hStringResourceHandle,pHelpFilePath,pHelpID,hPrinter,pPrinterName,hBitmapResourceHandle,hDialogResourceHandle)

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
	//m_wID				= IDD_TAB_PAPER_0;
	m_wID = IDD_TAB_PAPER_03;
    m_pPrivateDevmode = pPrivateDevmode;
	m_ObjCShowMessage = NULL;
	m_pWaterMarkIndex = &sWaterMarkIndex ;
	m_pUsIndex = &sUsIndex;
	m_ppi = ppi;
    rsize_t size = wcslen(pPrinterName) +1;
	m_pPrinterName = new WCHAR[size];
    if(NULL != m_pPrinterName)
    {
        ::SecureZeroMemory(m_pPrinterName,sizeof(WCHAR) * size);
    }
	wcscpy_s(m_pPrinterName,size,pPrinterName);
}
//=============================================================================
// function
//      CPaperTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Destructor to the class CPaperTabPropertyPage.
//=============================================================================
CPaperTabPropertyPage::~CPaperTabPropertyPage(void)
{
	if(NULL != m_pPrinterName)
	{
		delete[] m_pPrinterName;
		m_pPrinterName = NULL;
	}	
	delete m_ObjCShowMessage;
	m_ObjCShowMessage = NULL ;
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
CPaperTabPropertyPage::InitDlgBox(
    _Out_ LPCTSTR* ppszTemplate,
    _Out_ LPCTSTR* ppszTitle
    )
{
    HRESULT hr = S_OK;

	//Fixed Coverity issue - 90557 - SSDI:Seetharam - 20201020
	//if ((NULL != ppszTemplate) || (NULL != ppszTitle))
 	if ((NULL != ppszTemplate) && (NULL != ppszTitle))
   {
        *ppszTemplate = MAKEINTRESOURCE(m_wID);
        if (LoadString(m_hStringResourceHandle, m_wID, m_szItem, countof(m_szItem)) > 0)
		{
			*ppszTitle    = m_szItem ;
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
CPaperTabPropertyPage::InitializeUIControls(
    _In_ CONST HWND   hDlg
    )
{
	HRESULT hr = S_OK;
	m_hPropertySheetHandle = hDlg;
	CreatePaperSizeToTiledPamphletPaperSizeMap();
	CreateMessageClass(hDlg);
	__super::InitializeUIControls(hDlg);
	//<S><A> RedMine Ticket(#1041) "Zoom Settings": "Zoom Settings" paper size list box has wrong paper size.: 2014.02.24, SSDI:Gajanana D S
	// When PAper size changes update fit to paper size value
	UINT iCtrlID = IDC_CMB_PAPER;
	ChangeComboBoxSelectedItem(hDlg,iCtrlID);
	//<E> RedMine Ticket(#1041) "Zoom Settings": "Zoom Settings" paper size list box has wrong paper size.: 2014.02.24, SSDI:Gajanana D S
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		ShowBookletPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	}
	else
	{
		ShowPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	}
	//SetPaperTypeIndex(hDlg);
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
void CPaperTabPropertyPage::CreateContorlClass(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
//<S><C>To add Paper source tab UD3L  2020.02.12, SSDI:Sneha TG
	//std::string strFeatureName = m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszFeature;
	//PRESOURCEIDINFO pResourceIDInfo = GetResourceIDInfoOfControl(strFeatureName);
	std::string strFeatureName; 
	PRESOURCEIDINFO pResourceIDInfo = NULL;
//<E>To add Paper source tab UD3L 2020.02.12, SSDI:Sneha TG
	//<S><D>Deleted To fix SSDI Bug #2089,24.06.2020,SSDI:Chanchal Parkash
	// Move To Main Tab to Fix #2089
	/*Few OS Specific Language has default language as A4 to justify this initialise Default Setting with 
	"A4" as default paper */
	/*if(DMPAPER_A4 == GetPaperSizeDef())
	{
		m_DefaultSettings[FEATURE_PAPERSIZE].ResourceID = IDS_PAPERSIZE_9;
	}*/
	//<E>To fix SSDI Bug #2089,24.06.2020,SSDI:Chanchal Parkash
 
	//Output Size
	//<S><A>modified output paper size control ID as custom paper crash in UD3L UI 2020.03.05, SSDI:Goutham
	pResourceIDInfo = GetResourceIDInfoOfControl(FITTOPAPER);
	//<E>modified output paper size control ID as custom paper crash in UD3L UI 2020.03.05, SSDI:Goutham
	CUIControl* pControl = new  CShImageCombo(
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
		slog.putLog("CPaperTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName,&m_PropertySheetState, IDC_BTN_OUTPUTSIZE_I);
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
		slog.putLog("CPaperTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}


	//Paper Tray
	pResourceIDInfo = GetResourceIDInfoOfControl(PAPER_SOURCE);
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN],
		m_pFeatureInfoList,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_INPUTBIN],
		m_ObjCShowMessage,
		IDC_CMB_PSEL_PSRC,
		m_PropertySheetState
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_PSEL_PSRC, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CPaperTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCombo)\n");
	}


	//Paper Type
	pResourceIDInfo = GetResourceIDInfoOfControl(MEDIA_TYPE);
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE],
		m_pFeatureInfoList,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_MEDIATYPE],
		m_ObjCShowMessage,
		IDC_CMB_PSEL_PTYPE,
		m_PropertySheetState
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_PSEL_PTYPE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CPaperTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCombo)\n");
	}	
	//<S><A>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_PAPERTYPE_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PAPERTYPE_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CPaperTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}
	//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S

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
		slog.putLog("CPaperTabPropertyPage::CreateContorlClass -> NULL_POINTER(CMimicImage)\n");
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
		slog.putLog("CPaperTabPropertyPage::CreateContorlClass -> NULL_POINTER(CMimicImage)\n");
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
    pControl = new CSysLinkCtrl(IDC_SYSLINK_TAB_DEFAULTS,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_SYSLINK_TAB_DEFAULTS, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CPaperTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_SYSLINK_TAB_DEFAULTS)\n");
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
void CPaperTabPropertyPage::SetControlStrings(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
//<S><A>To add Favorites, Save and Delete button label to Paper source tab UD3L  2020.02.12, SSDI:Sneha TG
	//Favorites label
//<S><C>Modified Favorites string ID : 2020.03.05, SSDI:Goutham
	CUIControl* pControl = new CStaticText(IDC_STC_SHARED_USRSET, m_hStringResourceHandle);
//<E>Modified Favorites string ID : 2020.03.05, SSDI:Goutham
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
		slog.putLog("CPaperTabPropertyPage::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	//Favorites - Copies label
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
		slog.putLog("CPaperTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}

	//Favorites - Delete button label
    //<S><A>Added delete string ID : 2020.03.05, SSDI:Goutham
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
		slog.putLog("CPaperTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_BTN_SHARED_US_DEL)\n");
	}
    //<E>Added delete string ID : 2020.03.05, SSDI:Goutham
	//<S><A>Bug 1952 : 2020.06.17, SSDI:Seetharam
	//Hide the Delete button on dialog initialization
	ShowWindow(GetDlgItem(hDlg, IDS_IDC_BTN_SHARED_US_DEL), SW_HIDE);
	//<E>Bug 1952 : 2020.06.17, SSDI:Seetharam

	//Favorites - Save button label
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
		slog.putLog("CPaperTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_BTN_SHARED_US_SAVE)\n");
	}

	//<E>To add Favorites, Save and Delete button label to Paper source tab UD3L 2020.02.12, SSDI:Sneha TG
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
		slog.putLog("CPaperTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_BTN_SHARED_DEFAULT)\n");
	}

	//Output Size label
    //<S><C>Modified output paper size string ID : 2020.03.05, SSDI:Goutham
	pControl = new CStaticText(IDS_IDC_STC_OUTPUTSIZE, m_hStringResourceHandle);
    //<E>Modified output paper size string ID : 2020.03.05, SSDI:Goutham
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
		slog.putLog("CPaperTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_OUTPUTSIZE)\n");
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
		slog.putLog("CPaperTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_OUTPAPER)\n");
	}


	//Paper Tray label
	pControl = new CStaticText(IDS_IDC_STC_PAPERSRC, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PAPERSRC, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CPaperTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_OUTPUTSIZE)\n");
	}


	//Paper Type label
	pControl = new CStaticText(IDS_IDC_STC_PAPERTYPE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PAPERTYPE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CPaperTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_OUTPUTSIZE)\n");
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
LRESULT CPaperTabPropertyPage::OnBnClicked(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	LRESULT			res = 0;
	BOOL			blChecked;

	switch (wCtrlID)
	{
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
	//<S><C>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
	case IDC_BTN_PAPERTYPE_I:
	//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	case IDC_BTN_OUTPUTSIZE_I:
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
	}

	res = 1;
	return res;
}
//////////////////////////////////////////////////////////////////////////////
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
HRESULT CPaperTabPropertyPage::OnDrawItem(_In_ CONST HWND   hDlg, _In_ CONST WPARAM wParam, _In_ CONST LPARAM lParam)
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
				//Fixed Coverity issue - 90369 - SSDI:Seetharam-20201006
				//else
				//{
				//	//slog.putLog("CPaperTabPropertyPage::SetPaperTypeIndex -> NULL_OBJECT(CCombo)\n");
				//	slog.putLog("CPaperTabPropertyPage::SetPamphletIndex -> NULL_OBJECT(CCombo)\n");
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
		//<S><C>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
		//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
		case IDC_BTN_PAPERTYPE_I:
		//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
		case IDC_BTN_OUTPUTSIZE_I:
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


HRESULT
CPaperTabPropertyPage::SendCommand(
    _In_ CONST HWND   hDlg,
    _In_ CONST WPARAM wParam
    )
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

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
		// <S><M> Coverity Fix 91476 & 91527 - SSDI:Seetharam - 20210907
		//return S_OK;
		hr = S_OK;
		goto EXIT;
		// <S><E> Coverity Fix 91476 & 91527  - SSDI:Seetharam - 20210907
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
	//	return S_OK;
	//}
	////<E>To Fix Redmine Bug #3936,2021.28.07,ssdi:Chanchal Singla
	//<E>Move the code to UndoControlSelection() to fix redmine Bug #3666,#3464,2021.Dec.13,SSDI:Chanchal Singla
	__super::SendCommand(hDlg,wParam);
//<S><A>To Fix Feedback Issue 16 and added Constarints between paper tray/type ,output size ,original size,2020-10-29,SSDI:Chanchal Singla
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
					if ((FitToPSize >= OUTPUTPAPERSIZE_ENV_DL && FitToPSize <= OUTPUTPAPERSIZE_JENV_YOU4) && (wStaple == staple_none))
					{
						Pre_SelectedOptionindex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex;
						Pre_SellectedOptionName = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].ppszOptions[Pre_SelectedOptionindex];
						m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_Envelope;
						m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "ENVELOPE";
						if (OnPaperTypeChanged(hDlg, wContolID) == TRUE)
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
						if (OnPaperTypeChanged(hDlg, wContolID) == TRUE)
						{
							m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = Pre_SelectedOptionindex;
							m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = Pre_SellectedOptionName;
						}
					}
					else
					{
						if ((FitToPSize >= OUTPUTPAPERSIZE_A3 && FitToPSize <= OUTPUTPAPERSIZE_EXECUTIVE)
							&& (wStaple != staple_none) && (wDuplex == eFirstOption)
							&& (!strcmp("ENVELOPE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)
								|| !strcmp("JAPANESEPOSTCARD", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption)))
						{
							m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_AutoSelect;
							m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption = "AUTOSELECT";
						}
						else if (!strcmp("SAOS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption))
						{
							if (PaperSize <= PAPERSIZE_EXECUTIVE
								&& (wStaple != staple_none) && (wDuplex == eFirstOption)
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
			case IDC_CMB_PSEL_PSRC:
				{
					UINT Paper_Tray = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex;
					if (Paper_Tray >= PAPERSRC_Tray1 && Paper_Tray <= PAPERSRC_Tray4)
					{
						m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex = PAPERTYPE_UsePaperTraySettings;
					}
				}
				break;
			case IDC_CMB_PSEL_PTYPE:
				{	
					UINT Paper_Type = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex;
					if (Paper_Type != PAPERTYPE_UsePaperTraySettings)
					{
						if (!strcmp("TRAY1", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption)
							|| !strcmp("TRAY2", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption)
							|| !strcmp("TRAY3", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption)
							|| !strcmp("TRAY4", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption))
						{
							m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex = PAPERSRC_AutoSelect;
						}
					}
			
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
		default:
			break;
	}
	//<E>To Fix Feedback Issue 16 and added Constarints between paper tray/type ,output size ,original size,2020-10-29,SSDI:Chanchal Singla
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
	if ((NULL == wFeatureInfoList_Pre))
	{
		wFlag = TRUE;
	}
	if (wFlag == FALSE)
	{
		//Fixed Coverity #91476,13-08-2021,SSDI:Chanchal Singla
		if (NULL != wTempFeatureInfoList)
		{
			delete wTempFeatureInfoList;
			wTempFeatureInfoList = NULL;
		}
		//Fixed Coverity #91422,13-08-2021,SSDI:Chanchal Singla
		if (NULL != wFeatureInfoList_Pre)
		{
			delete wFeatureInfoList_Pre;
			wFeatureInfoList_Pre = NULL;
		}
	}
	//<E> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla
	return S_OK;
}
void CPaperTabPropertyPage::ShowInformationIconOnPaper(HWND hDlg,INT iCtrlID)
{
	
	if((0 < iCtrlID) &&
		/*(IDC_RADIO_ZOOM_NORMAL != iCtrlID)&&
		(IDC_RADIO_ZOOM_FITTOPAGE_PAPER_SIZE != iCtrlID)&&*/
        //<S><C>Modified output paper size control ID : 2020.03.05, SSDI:Goutham
		/*(IDC_CMB_PAPER !=iCtrlID)&&*/
        //<E>Modified output paper size control ID : 2020.03.05, SSDI:Goutham
		(IDC_CMB_OUTPUTSIZE != iCtrlID))
	{
		return ;
	}
	if(BST_CHECKED == IsDlgButtonChecked(hDlg,IDC_RADIO_ZOOM_NORMAL))
	 {
		 SetControlState(hDlg,IDC_COMBO_FITTOPAGE_PAPER_SIZE,SCUI_STATE_SHOW);
		 SetControlState(hDlg,IDC_STATIC_ZOOM_SIZE,SCUI_STATE_SHOW);
		  if(IsWindowEnabled(GetDlgItem(hDlg,IDC_BTN_FITTOPAGE_PAPER_I)))
		  {
			  SetControlState(hDlg,IDC_BTN_FITTOPAGE_PAPER_I,SCUI_STATE_OFF);
			  SetControlState(hDlg,IDC_BTN_PAPER_I,SCUI_STATE_ON);
		  }
		  if(!strcmp("CUSTOMSIZE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption) &&
				  ((strcmp("OFF",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption))||
				  (strcmp("NONE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption)))
				  )
		  {
			  SetControlState(hDlg,IDC_BTN_PAPER_I,SCUI_STATE_ON);
		  }
	 }
	 else
	 {
		  SetControlState(hDlg,IDC_COMBO_FITTOPAGE_PAPER_SIZE,SCUI_STATE_ON);
		  SetControlState(hDlg,IDC_STATIC_ZOOM_SIZE,SCUI_STATE_ON);
		  if(IsWindowEnabled(GetDlgItem(hDlg,IDC_BTN_PAPER_I)))
		  {
			  SetControlState(hDlg,IDC_BTN_PAPER_I,SCUI_STATE_OFF);
			  SetControlState(hDlg,IDC_BTN_FITTOPAGE_PAPER_I,SCUI_STATE_ON);
			  //Commented code fr Coverity 91209 - since this control is no more available in UD3L
			  //if(iCtrlID == IDC_RADIO_ZOOM_FITTOPAGE_PAPER_SIZE)
			  //{
				 // if((!strcmp("CUSTOMSIZE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption)) &&
				 //   (strcmp("4HOLE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption)))
				 // {
					//  SetControlState(hDlg,IDC_BTN_FITTOPAGE_PAPER_I,SCUI_STATE_OFF);
				 // }
			  //}
			  //<S><A> RedMine Ticket(#1107) Information icon is displaying on Fit To Paper Size list when 'Letter'. : 2014.03.09, SSDI:Gajanana D S
			 /* if((!strcmp("CUSTOMSIZE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption)) &&
				  (strcmp("LETTER",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption))&&
				  (strcmp("4HOLE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption)))
			  {
				  SetControlState(hDlg,IDC_BTN_FITTOPAGE_PAPER_I,SCUI_STATE_OFF);
			  }*/
			  //<S><A> RedMine Ticket(#1107) Information icon is displaying on Fit To Paper Size list when 'Letter'. : 2014.03.09, SSDI:Gajanana D S
		  }
	 }
}

void CPaperTabPropertyPage::ShowPaperSizeDimensions(HWND hDlg, INT iCtrlID)
{
    //<S><C>Modified output paper size control ID : 2020.03.05, SSDI:Goutham
      UIControlMap::const_iterator Iterator = m_UIControls.find(iCtrlID);
    //<E>Modified output paper size control ID : 2020.03.05, SSDI:Goutham
    if(Iterator != m_UIControls.end())
    {
        CShImageCombo *pCombo  = static_cast<CShImageCombo*>(Iterator->second);
        int index = pCombo->GetSelectedIndex();
        WCHAR cSep = GetDecimalSeparator();

        WCHAR			szText[256] = {};
        WCHAR			sztemp[256] = {};
        WCHAR			szw[32] = {};
        WCHAR			szl[32] = {};
        WCHAR			szu[32] = {};
		if (iCtrlID == IDC_CMB_OUTPUTSIZE && index == 0)
		{
			index = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
		}
		else if (iCtrlID == IDC_CMB_OUTPUTSIZE && index != 0)
		{
			index = index - 1;
		}
        int PaperSize = PaperSizeArray[index][0];
        int Width     = PaperSizeArray[index][1];
        int Length    = PaperSizeArray[index][2];
        int unit      =	PaperSizeArray[index][3];
        if(CUSTOM_PAPER_INDEX > index)
        {
            PaperSize = PaperSizeArray[index][0];
            Width = PaperSizeArray[index][1];
            Length= PaperSizeArray[index][2];
            unit  =	PaperSizeArray[index][3];
        }
		else
		{
			//<S><A> To Added Support  of Custom Paper, 24-07-2020,SSDI:Chanchal Singla
			PaperSize = DMPAPER_CUSTOM;
			//int Unit = m_pOemPrivateDevMode->scpi.ext.byCusUnits;
			USERPAPERSIZEDATA upsd;
			SecureZeroMemory(&upsd, sizeof(upsd));
			CUPSReg FAR		*pregUps = NULL;

			//CShIniFile			*m_pmcf = NULL;
			//TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
			//GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
			//m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
			//CShJsonUserPSize	*pjsonups = NULL;

			pregUps = new CUPSReg(m_hStringResourceHandle, m_pPrinterName);
			if (pregUps == NULL)
			{
				return;
			}
			//if ((*m_pmcf).IsWriteToJson() == TRUE)
			//{
			//	pjsonups = new CShJsonUserPSize(ghInstance, m_pPrinterName);
			//}

			if (iCtrlID == IDC_CMB_OUTPUTSIZE &&
				!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE].pszOption, "CUSTOMSIZE"))
			{
				//<S><C>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				///Custom Paper values are changing to Default even though cancel button is clicked
				memcpy(&upsd, &m_ppi->FitToPaperSize_upsd, sizeof(USERPAPERSIZEDATA));
				//	(*pregUps).ReadUPSData(m_pPrinterName, DMPAPER_CUSTOM_ZOOM, &upsd);
				//<E>To Fix Redmine Bug #3681,2021.10.12,SSDI:Chanchal Singla
				Width = upsd.dwWidth;
				Length = upsd.dwLength;
				unit = upsd.wUnit;
			}
			//<S><A> To Fix Redmine #Bug #2535,28-07-2020,SSDI:Chanchal Singla
			else if (iCtrlID == IDC_CMB_OUTPUTSIZE &&
				!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAPERSIZE].pszOption, "CUSTOMSIZE") &&
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
			//<E> To Fix Redmine #Bug #2535,28-07-2020,SSDI:Chanchal Singla
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

        switch(unit)
        {
        case MM_LOENGLISH:
            {
                WCHAR szForm[MAX_FORMNAME_TEXT] = {};
                wcscpy_s(szForm,L"%d%c%02d");
                StringCbPrintf(szw,sizeof(szw),szForm, Width / 100, cSep, Width % 100);
                StringCbPrintf(szl,sizeof(szl),szForm, Length / 100, cSep, Length % 100);
                LoadString(m_hStringResourceHandle, IDS_UNIT_INCH, szu, countof(szu));
            };
            break;
        case MM_LOMETRIC:
        default:
            {
                WCHAR szForm[MAX_FORMNAME_TEXT] = {};
                if (DMPAPER_CUSTOM == PaperSize)
                {
                    wcscpy_s(szForm,L"%d%c%d");
                }
                else
                {
                    wcscpy_s(szForm,L"%d");
                }
                StringCbPrintf(szw,sizeof(szw),szForm, Width / 10, cSep, Width % 10);
                StringCbPrintf(szl,sizeof(szl),szForm, Length / 10, cSep, Length % 10);
                LoadString(m_hStringResourceHandle, IDS_UNIT_MM, szu, countof(szu));
            };

            break;
        }
        LoadString(m_hStringResourceHandle, IDS_IDC_STC_PAPER, szText, countof(szText));
        StringCbPrintf(sztemp,sizeof(sztemp),szText, szw, szl, szu);
        SetDlgItemText(hDlg, IDC_STC_OUTPAPER,sztemp);
        SecureZeroMemory(szText, sizeof(szText));
        SecureZeroMemory(szl, sizeof(szl));
        SecureZeroMemory(sztemp, sizeof(sztemp));
        /*LoadString(m_hStringResourceHandle,IDC_STATIC_ZOOM_SIZE,szText,countof(szText));
        LoadString(m_hStringResourceHandle,(IDS_PAPERSIZE_BASE + PaperSize),szl,countof(szl));
        StringCbPrintf(sztemp,sizeof(sztemp),szText,szl);
        SetDlgItemText(hDlg,IDC_STATIC_ZOOM_SIZE,sztemp);*/

    }
}

VOID CPaperTabPropertyPage::ShowConstraintsOnFactoryDefaults(HWND hDlg, INT iCtrlID)
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

//<S><A>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
//=============================================================================
// function
//       ShowConstraintsonPTypeVsPSrcStaple
//
// parameters
//			HWND		Handler to window
// return value
//      VOID
//
// outline
//       Show Constraints on Ptype Vs staple and paper source
//=============================================================================
VOID CPaperTabPropertyPage::ShowConstraintsonPTypeVsPSrcStaple(HWND hDlg, INT iCtrlID)
{
	BYTE Flag = 0;
	m_ControlState.cPselPType_I = SCUI_STATE_OFF;
	if (ReadStapleLessFlag(Flag))
	{
		if (strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "FORMSOURCE") != 0
			&& strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption, "BYPASSTRAY") != 0)
		{
			if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption, "STAPLE_STAPLELESS"))
			{
				m_ControlState.cCmbPselPType = SCUI_STATE_SHOW;
				m_ControlState.cPselPType_I = SCUI_STATE_ON;
			}
			else
			{
				m_ControlState.cCmbPselPType = SCUI_STATE_ON;
				m_ControlState.cPselPType_I = SCUI_STATE_OFF;
			}
		}
		else
		{
			m_ControlState.cCmbPselPType = SCUI_STATE_ON;
			m_ControlState.cPselPType_I = SCUI_STATE_OFF;
		}
		SetControlState(hDlg, IDC_CMB_PSEL_PTYPE, m_ControlState.cCmbPselPType);
		SetControlState(hDlg, IDC_BTN_PAPERTYPE_I, m_ControlState.cPselPType_I);
	}
}
//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S

HRESULT
CPaperTabPropertyPage::ActivateUIControl(
    _In_ CONST HWND   hDlg
    )
{
	HRESULT hr = S_OK;
	//<S><A>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	memcpy(&m_PropertySheetState, m_ppi->pps, sizeof(PROPSTATE));
	//<E>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	__super::ActivateUIControl(hDlg);
    //<S><C>Modified output paper size control ID : 2020.03.05, SSDI:Goutham
	UINT iCtrlID = IDC_CMB_OUTPUTSIZE;
    //<E>Modified output paper size control ID : 2020.03.05, SSDI:Goutham
	ChangeComboBoxSelectedItem(hDlg,iCtrlID);
//	ShowInformationIconOnPaper(hDlg);
	UpdateUserSetting(hDlg, IDC_CMB_SHARED_USRSET);
	//ShowConstraintsOnFactoryDefaults(hDlg, IDC_CMB_SHARED_USRSET);
	SetTabState(hDlg);
	return hr;
}

void CPaperTabPropertyPage::SetPaperTypeIndex(HWND hDlg,INT iCtrlID)
{
    //<S><C>Modified output paper size and Paper source control ID : 2020.03.05, SSDI:Goutham
	if((IDC_CMB_OUTPUTSIZE != iCtrlID)/* && (IDC_COMBO_FITTOPAGE_PAPER_SIZE != iCtrlID)*/ &&(IDC_CMB_PSEL_PSRC != iCtrlID)/*&& (iCtrlID != IDC_RADIO_ZOOM_NORMAL)*/)
    //<E>Modified output paper size and Paper source control ID : 2020.03.05, SSDI:Goutham
	{
		return ;
	}
    //<S><C>Modified Paper type control ID : 2020.03.05, SSDI:Goutham
	UIControlMap::const_iterator iPaperTypeComboIterator =  m_UIControls.find(IDC_CMB_PSEL_PTYPE);
   //<E>Modified Paper type control ID : 2020.03.05, SSDI:Goutham
	if(iPaperTypeComboIterator == m_UIControls.end())
	{
		return ;
	}
	CShImageCombo * pControl = static_cast<CShImageCombo*>(iPaperTypeComboIterator->second);
	if(NULL == pControl)
	{
		return ;
	}
    //<S><C>Modified output Paper size control ID : 2020.03.05, SSDI:Goutham
	//if((IDC_CMB_OUTPUTSIZE == iCtrlID) &&
	////<E>Modified output Paper size control ID : 2020.03.05, SSDI:Goutham
	//	(strcmp("NONE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption) ||
	//	strcmp("OFF",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption)))
	//{
	//	return ;
	//}
	BOOL bPaperTypeIndexChanged = FALSE ;
    //<S><C>Modified Paper source and paper type control ID : 2020.03.05, SSDI:Goutham
	INT uPaperSourceIndex = GetValue(hDlg, IDC_CMB_PSEL_PSRC);
	INT uPaperTypeIndex = GetValue(hDlg, IDC_CMB_PSEL_PTYPE);
	//INT uPaperSourceIndex = SendDlgItemMessage(hDlg, IDC_CMB_PSEL_PSRC,CB_GETCURSEL, 0, 0);
	//INT uPaperTypeIndex = SendDlgItemMessage(hDlg, IDC_CMB_PSEL_PTYPE,CB_GETCURSEL, 0, 0);
    //<E>Modified Paper source and paper type control ID : 2020.03.05, SSDI:Goutham
	//<S><A> RedMine Ticket(#1088) Paper Type: “Paper type” values are not displaying correctly when 
	// Paper Source is set : 2014.03.03, SSDI:Gajanana D S	
	if(0 == uPaperSourceIndex && 
		!strcmp("IGNORE",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption))
	{
		
		auto iFeatureIterator = m_pResourceIDInfoMap->find(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszFeature);
		if(iFeatureIterator != m_pResourceIDInfoMap->end())
		{
			auto iOptionIterator =  iFeatureIterator->second.uOptionResourceID.find(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MEDIATYPE].pszOption);
			if(iOptionIterator != iFeatureIterator->second.uOptionResourceID.end())
			{
				iOptionIterator->second.second.ConstraintFeatureOptionMessageIDList[uPaperSourceIndex].uNumberOfConstraints++;
				iOptionIterator->second.second.ConstraintFeatureOptionMessageIDList[uPaperSourceIndex].uConstraintAction = eHideControl;
			}
           //<S><C>Modified Paper type control ID : 2020.03.05, SSDI:Goutham
			iPaperTypeComboIterator->second->OnInit(IDC_CMB_PSEL_PTYPE,hDlg);
           //<E>Modified output Paper size control ID : 2020.03.05, SSDI:Goutham
		}
	}
	//<E> RedMine Ticket(#700) Paper Type: “Paper type” values are not displaying correctly when 
	// Paper Source is set : 2014.03.03, SSDI:Gajanana D S
	INT uPaperSizeIndex = 0 ;
	INT uPaperTypeItemControlID = pControl->GetCurrentItemId();
	UINT uIncrementIndex = 0;
	if(BST_CHECKED == IsDlgButtonChecked(hDlg,IDC_RADIO_ZOOM_NORMAL))
	{
		uPaperSizeIndex = GetValue(hDlg, IDC_CMB_PAPER);
        //<S><C>Modified output Paper size control ID : 2020.03.05, SSDI:Goutham
		//uPaperSizeIndex = SendDlgItemMessage(hDlg, IDC_CMB_PAPER,CB_GETCURSEL, 0, 0);
        //<E>Modified output Paper size control ID : 2020.03.05, SSDI:Goutham
	}
	else
	{
		uPaperSizeIndex = GetValue(hDlg, IDC_CMB_OUTPUTSIZE);
		//uPaperSizeIndex = SendDlgItemMessage(hDlg, IDC_CMB_OUTPUTSIZE,CB_GETCURSEL, 0, 0);
	
	}
	if(0 > uPaperTypeIndex)
	{
		uPaperTypeIndex = 0 ;
		//SendDlgItemMessage(hDlg, IDC_COMBO_PAPER_TYPE, CB_SETCURSEL, uPaperTypeIndex,0);
	}
    //<S><C>Modified output Paper size control ID : 2020.03.05, SSDI:Goutham
	if(IDC_CMB_PAPER == iCtrlID ||(IDC_CMB_OUTPUTSIZE ==  iCtrlID))
    //<E>Modified output paper size control ID : 2020.03.05, SSDI:Goutham
	{
		switch(uPaperSizeIndex)
		{
			case 9:
			case 10:
			case 11:
			case 12:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
				if(10 == uPaperTypeIndex)
				{
					bPaperTypeIndexChanged = FALSE ;
				}
				else
				{
					bPaperTypeIndexChanged = TRUE ;
				}
				uPaperTypeIndex = 10;
				break;
			case 13:
				uPaperTypeIndex = 10;
				uIncrementIndex =1 ;

				break;
			case 20:
				break;
			default:
				if(ENVELOPE_PAPER_TYPE_ID == uPaperTypeItemControlID || (4269 == uPaperTypeItemControlID))
				//if(10 == uPaperTypeIndex)
				{
					uPaperTypeIndex = 0;
					bPaperTypeIndexChanged = TRUE ;
				}
				
				break ;
		}
	}
	//Commented code fr Coverity 91213 - since this control is no more available in UD3L
	//if(IDC_RADIO_ZOOM_NORMAL == iCtrlID)
	//{
	//	if(ENVELOPE_PAPER_TYPE_ID == uPaperTypeItemControlID || (4269 == uPaperTypeItemControlID))
	////	if(10 == uPaperTypeIndex)
	//	{
	//		uPaperTypeIndex = 0;
	//		bPaperTypeIndexChanged = TRUE ;

	//	}

	//}
 //<S><C>Modified Paper source control ID : 2020.03.05, SSDI:Goutham
	if(IDC_CMB_PSEL_PSRC == iCtrlID)
    //<E>Modified Paper source control ID : 2020.03.05, SSDI:Goutham
	{
		if(2 <= uPaperSourceIndex)
		{
			uPaperTypeIndex = GetValue(hDlg, IDC_CMB_PSEL_PTYPE) - 1;
            //<S><C>Modified Paper type control ID : 2020.03.05, SSDI:Goutham
			//uPaperTypeIndex = SendDlgItemMessage(hDlg, IDC_CMB_PSEL_PTYPE,CB_GETCOUNT, 0, 0) - 1;
            //<E>Modified Paper type control ID : 2020.03.05, SSDI:Goutham
		}
	}
    //<S><C>Modified Paper type control ID : 2020.03.05, SSDI:Goutham
	pControl->SetComboBoxIndex(m_hPropertySheetHandle, IDC_CMB_PSEL_PTYPE,uPaperTypeIndex,uIncrementIndex);
    //<E>Modified Paper type control ID : 2020.03.05, SSDI:Goutham
	//<S><A> RedMine Ticket(#1083) Print on Both sides:“Print on Both sides” drop down is disabled 
	// when “None” is selected from “Pamphlet style” drop down: 2014.03.03, SSDI:Gajanana D S
	// Issue 2 : When "100 * 148" paper is selected, Paper type is set to "Japanese Postcard". 
	// In this case limitation of Transparency  should be removed. 
	//if(uPreviousPaperTypeFeatureIndex != m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex &&
		//!strcmp("TRANSPARENCY1",m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].ppszOptions[uPreviousPaperTypeFeatureIndex]))
	//if(uPreviousPaperTypeFeatureIndex != m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex)
	if(20 != uPaperSizeIndex && TRUE == bPaperTypeIndexChanged)	
	{
       //<S><C>Modified Paper Type control ID : 2020.03.05, SSDI:Goutham
		NotifyConstrainedFeatures(hDlg,pControl, IDC_CMB_PSEL_PTYPE);
        //<E>Modified Paper Type control ID : 2020.03.05, SSDI:Goutham
	}
	//<S><A> RedMine Ticket(#1083) Print on Both sides:“Print on Both sides” drop down is disabled 
	// when “None” is selected from “Pamphlet style” drop down: 2014.03.03, SSDI:Gajanana D S
	
	
	
}
void CPaperTabPropertyPage::SetPaperSourceIndex(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
     //<S><C>Modified Paper source control ID : 2020.03.05, SSDI:Goutham
  	  ////UINT uPaperSourceIndex = SendDlgItemMessage(hDlg, IDC_CMB_PSEL_PSRC,CB_GETCURSEL, 0, 0);
	  //UINT uPaperTypeIndex = SendDlgItemMessage(hDlg, IDC_CMB_PSEL_PTYPE,CB_GETCURSEL, 0, 0);
	  UINT uPaperSourceIndex = GetValue(hDlg, IDC_CMB_PSEL_PSRC);
	  UINT uPaperTypeIndex = GetValue(hDlg, IDC_CMB_PSEL_PTYPE);
	  UIControlMap::const_iterator iPaperSourceComboIterator =  m_UIControls.find(IDC_CMB_PSEL_PSRC);
     //<E>Modified Paper source control ID : 2020.03.05, SSDI:Goutham
	if(iPaperSourceComboIterator == m_UIControls.end())
	{
		return ;
	}
	CShImageCombo * pControl = static_cast<CShImageCombo*>(iPaperSourceComboIterator->second);
	if(NULL == pControl)
	{
		slog.putLog("CPaperTabPropertyPage::SetPaperSourceIndex -> NULL_POINTER(CCombo)\n");
		return ;
	}
	if(2 <= uPaperTypeIndex)
	{
		if(1 != uPaperSourceIndex)
		{
			uPaperSourceIndex = 0;
		}
	}
    //<S><C>Modified Paper source control ID : 2020.03.05, SSDI:Goutham
	pControl->SetComboBoxIndex(m_hPropertySheetHandle, IDC_CMB_PSEL_PSRC,uPaperSourceIndex);
	NotifyConstrainedFeatures(hDlg,pControl, IDC_CMB_PSEL_PSRC);
    //<E>Modified Paper source control ID : 2020.03.05, SSDI:Goutham

}
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
VOID CPaperTabPropertyPage::SetTabState(
	_In_ CONST HWND   hDlg,
	_In_ CONST INT iCtrlID
)
{
	SetUserSettingState(hDlg, IDC_CMB_SHARED_USRSET);
	//<S><A>To Add the Support of Booklet Standard Output Size and fix SSDI #Bug 2067,2020-09-02,SSDI:Chanchal Singla
	// Initialize the Booklet and FitToPaper Size Drop down 
	// When Booklet is Standard then Only Booklet Supported Output Size Paper Should be Intialize in DropDown
	BookletVsFitToPaperSizeInitialization(hDlg, IDC_CMB_OUTPUTSIZE);

	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		ShowBookletPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	}
	else
	{
		ShowPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	}
	//<E>To Add the Support of Booklet Standard Output Size and fix SSDI #Bug 2067,2020-09-02,SSDI:Chanchal Singla

//<S><A>To Fix Feedback Issue 16 and added Constarints between paper tray/type ,output size ,original size,2020-10-29,SSDI:Chanchal Singla
	//Envelope and Japanese Postcard are not displayed in Paper Source Tab : Paper Type.
	//<S><A>To Fix Redmine #7330,#7268,#7267,22-04-2024,SSDI:Manoj S
	//constraints on change in GPD default staple value.
	BYTE Flag = 0;
	//<S><A>To Fix Redmine #7513,21-06-2024,SSDI:Manoj S
	//short defaultstapleval = (short)m_DefaultSettings[FEATURE_STAPLING].iDevmodeValue;
	if (m_PropertySheetState.byStapleless && ReadStapleLessFlag(Flag)) //&& (defaultstapleval == 3))
	{
		if (!strcmp("STAPLE_STAPLELESS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption) &&
			!strcmp("FORMSOURCE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption = "BYPASSTRAY";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex = PAPERSRC_BypassTray;
		}
	}
	//<E>To Fix Redmine #7513,21-06-2024,SSDI:Manoj S
		//----Feature Paper Tray
		long PaperTray = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex;
		InitializeFeatureOption(hDlg, IDC_CMB_PSEL_PSRC, PaperTray);
	//<E>To Fix Redmine #7330,#7268,#7267,22-04-2024,SSDI:Manoj S
	////----Feature Paper Type
	long PaperType = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MEDIATYPE].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_PSEL_PTYPE, PaperType);
//<E>To Fix Feedback Issue 16 and added Constarints between paper tray/type ,output size ,original size,2020-10-29,SSDI:Chanchal Singla
	SetControlState(hDlg, IDC_BTN_PAPERTYPE_I, SCUI_STATE_OFF);
	//<S><A>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
	ShowConstraintsonPTypeVsPSrcStaple(hDlg, IDC_CMB_PSEL_PTYPE);
	//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	SetControlState(hDlg, IDC_BTN_OUTPUTSIZE_I, SCUI_STATE_OFF);
	ShowStapleVsOriginalSizeVsOutputSizeInformationIcon(hDlg);
	ShowPunchVsOriginalSizeVsOutputSizeInformationIcon(hDlg);
	//<S><A>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	memcpy( m_ppi->pps, &m_PropertySheetState, sizeof(PROPSTATE));
	//<E>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla

	//if (IsPrinterOperators(m_pPrinterName) == FALSE)
	//	DisableAllCtrls();
}


HRESULT CPaperTabPropertyPage::SetTabDefaults(_In_ CONST HWND   hDlg) 
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	__super::SetTabDefaults(hDlg);
	//<S><A>To Fix SSDI Bug #2707,2020-09-04,SSDI:Chanchal Singla
	//After Clicking Defaults,Output Size value and Dimensions are not displaying correctly in Paper Source Tab.
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		ShowBookletPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	}
	else
	{
		ShowPaperSizeDimensions(hDlg, IDC_CMB_OUTPUTSIZE);
	}
	//<E>To Fix SSDI Bug #2707,2020-09-04,SSDI:Chanchal Singla

	INT iIndex = -1;
	BYTE Flag = 0;
	SetTiledPamphletPaperSizeIndex(hDlg,iIndex);
	{
		SendDlgItemMessage(hDlg, IDC_COMBO_FITTOPAGE_PAPER_SIZE, CB_SETCURSEL,iIndex, 0);
	}

	//<S><A>To Fix Redmine #2299,17-08-2020,SSDI:Chanchal Singla
	//Zoom check box is not unchecked when Defaults is clicked in Paper Source tab
	UINT uCheckState = BST_UNCHECKED;
	CUIControl pControl;
	if (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOMSETTINGS].uChangedOptionIndex != 0)
	{
		pControl.EnableApplyButton(hDlg);
	}
	if (m_DefaultSettings[FEATURE_ZOOMSETTINGS].iDevmodeValue == bool_false)
	{
		uCheckState = BST_UNCHECKED;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption = "OFF";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOMSETTINGS].uChangedOptionIndex = uCheckState;
	}
	else if (m_DefaultSettings[FEATURE_ZOOMSETTINGS].iDevmodeValue == bool_true)
	{
		uCheckState = BST_CHECKED;
		m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption = "ON";
		m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOMSETTINGS].uChangedOptionIndex = uCheckState;
	}
	//<S><A>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
	//<S><C>To Fix Redmine#7345,15-05-2024,SSDI:Manoj S
	//Paper Tray options are incorrect w.r.t Staple settings
	//<S><C>To Fix Redmine #7513,21-06-2024,SSDI:Manoj S
	short wstaple = (short)m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	//<S><A>To Fix Redmine #7513,31-05-2024,SSDI:Manoj S
	short defaultstapleval = (short)m_DefaultSettings[FEATURE_STAPLING].iDevmodeValue;
	if (m_PropertySheetState.byStapleless && ReadStapleLessFlag(Flag) && (defaultstapleval == staple_1_stapleless || wstaple == staple_1_stapleless))
	//<E>To Fix Redmine#7345,15-05-2024,SSDI:Manoj S
	//<E>To Fix Redmine #7513,21-06-2024,SSDI:Manoj S
	{
	
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_INPUTBIN].pszOption = "BYPASSTRAY";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex = PAPERSRC_BypassTray;
		
	}
	//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	//<E>To Fix Redmine #7513,31-05-2024,SSDI:Manoj S
	//<E>To Fix Redmine #2299,17-08-2020,SSDI:Chanchal Singla
	//ShowInformationIconOnPaper(hDlg);
	//<S><D> To fix bug no. 2575, 2020/09/10 Harika K M
	//CustomPaperSetTabDefaults(hDlg);
	//<S><D> To fix bug no. 2575, 2020/09/10 Harika K M
	return S_OK;
}
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
HRESULT  CPaperTabPropertyPage::CustomPaperSetTabDefaults(_In_ CONST HWND   hDlg)
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
		(*pjsonups).resetEx(DMPAPER_CUSTOM_ZOOM);
	}
	else
	{
		(*pregUps).resetEx(m_pPrinterName, DMPAPER_CUSTOM_ZOOM);
	}
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
VOID CPaperTabPropertyPage::ShowBookletPaperSizeDimensions(HWND hDlg, INT iCtrlID)
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
VOID CPaperTabPropertyPage::BookletVsFitToPaperSizeInitialization(HWND hDlg, INT iCtrlID)
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
	//<S><A>To Fix SSDI Bug #2707,2020-09-04,SSDI:Chanchal Singla
	//After Clicking Defaults,Output Size value and Dimensions are not displaying correctly in Paper Source Tab.
		m_DefaultSettings[FEATURE_BOOKLET_OUTPUTSIZE].ResourceID = (DMPAPER_A4 == GetPaperSizeDef()) ?IDS_PAPERSIZE_9 : IDS_PAPERSIZE_1;
	//<E>To Fix SSDI Bug #2707,2020-09-04,SSDI:Chanchal Singla
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
		UINT BookletStandardPSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex;
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
}
//<E>To Add the Support of Booklet Standard Output Size and fix SSDI #Bug 2067,2020-09-02,SSDI:Chanchal Singla