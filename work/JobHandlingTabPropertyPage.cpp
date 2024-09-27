//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : JobHandlingTabPropertyPage.cpp
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
#include "aes.h"
#include "shjsonstr.h"
#include <algorithm>
//For Logging Purpose
#include "MFPLogger.h"
#define MAX_ENCODING_STRING_LENGTH 16
//=============================================================================
// function
//      CJobHandlingTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Constructor to the class CJobHandlingTabPropertyPage.
//=============================================================================
CJobHandlingTabPropertyPage::CJobHandlingTabPropertyPage(HMODULE hStringResourceHandle,HMODULE hBitmapResourceHandle,HMODULE hDialogResourceHandle, HWND PropertySheetHandle,IPrintCoreHelperUni* pPrintCoreHelperUni,ResourceIDInfoMap* pResourceIDInfoMap,ConstraintFeatureInfoMap &ConstraintFeatureInfoMap,PFEATUREINFOLIST &pFeatureInfoList,PaperInfoMap PaperInfo, CustomPaperInfoMap CustomPaperInfoList,POEMDEV pOemPrivateDevMode,
                                           PDEVMODE pOemPublicDevMode,LPTSTR pHelpFilePath,DWORD *pHelpID,std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings,POEMDEV pPrivateDevmode,HANDLE hPrinter,
        WCHAR* pPrinterName,SHORT& sWaterMarkIndex, SHORT& sUsIndex, PPROPINFO ppi):CDocumentPropertyPage(hStringResourceHandle,pHelpFilePath,pHelpID,hPrinter,pPrinterName,hBitmapResourceHandle,hDialogResourceHandle),m_ObjShowJobHandlingMessage(NULL)

{
	
	m_pPrintCoreHelperUni = pPrintCoreHelperUni;
	m_pResourceIDInfoMap = pResourceIDInfoMap ;
	m_ConstraintFeatureInfoMap = &ConstraintFeatureInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	m_PaperInfo = PaperInfo;
	m_hPropertySheetHandle = PropertySheetHandle;
	m_CustomPaperInfoList =  CustomPaperInfoList ;
	m_pOemPrivateDevMode = pOemPrivateDevMode ;
	m_pOemPublicDevMode = pOemPublicDevMode ;
	//m_hDialogResourceHandle = hDialogResourceHandle;
    m_DefaultSettings = pDefaultSettings;
	m_wID				= IDD_TAB_JOBHAND_0;
    m_pPrivateDevmode  = pPrivateDevmode;
	SecureZeroMemory(&m_PropertySheetState,sizeof(PROPSTATE));
	m_pWaterMarkIndex = &sWaterMarkIndex ;
	m_pUsIndex = &sUsIndex;
	m_ppi = ppi;
	//Coverity Fix 90562 - SSDI:Seetharam - 20200827
	SecureZeroMemory(&m_TempJobControl, sizeof(JOBCONTROL));
	//Coverity Fix 90889 - SSDI:Goutham - 20201123
	m_plvFolderSel = NULL;
}

//=============================================================================
// function
//      CJobHandlingTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Destructor to the class CJobHandlingTabPropertyPage.
//=============================================================================
CJobHandlingTabPropertyPage::~CJobHandlingTabPropertyPage(VOID)
{
	if (NULL != m_ObjCShowMessage)
	{
		delete m_ObjCShowMessage;
		m_ObjCShowMessage = NULL;
	}
	if (NULL != m_ObjShowJobHandlingMessage)
	{
		delete m_ObjShowJobHandlingMessage;
		m_ObjShowJobHandlingMessage = NULL;
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
CJobHandlingTabPropertyPage::InitDlgBox(
    _Out_ LPCTSTR* ppszTemplate,
    _Out_ LPCTSTR* ppszTitle
    )
{
    HRESULT hr = S_OK;

	//Coverity Fix 90370 - SSDI:Seetharam - 20200908
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
CJobHandlingTabPropertyPage::InitializeUIControls(
    _In_ CONST HWND   hDlg
    )
{
	HRESULT hr = S_OK;
	m_hPropertySheetHandle = hDlg;
	JobControlDataToPropState();
	CreateMessageClass(hDlg);
	__super::InitializeUIControls(hDlg);
	
	//CreateDepenedentContolMap();	
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
VOID CJobHandlingTabPropertyPage::CreateContorlClass(VOID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;	
	PRESOURCEIDINFO pResourceIDInfo = NULL;
	CUIControl* pControl;
//<S><A>Added control class for print release feature,20.03.26 SSDI:Goutham
	//print release
	pControl = new CCheckBox(
		GetResourceIDInfoOfControl(PRINTRELEASE),
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINTRELEASE],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_PRINTRELEASE]
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_PRINTRELEASE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_CHK_PRINTRELEASE)\n");
	}
	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_PRINTRELEASE_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PRINTRELEASE_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
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
		IDC_CMB_RETENTION
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
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}



	pResourceIDInfo = GetResourceIDInfoOfControl(DOCUMENT_FILING_SETTINGS);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_DOCUMENT_FILING_SETTINGS],
		m_ObjCShowMessage,
		IDC_CMB_RETENTION_STOREDTO
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_RETENTION_STOREDTO, pControl);
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

//<S><A>Added control class for authentication feature,20.03.24 SSDI:Goutham
//<S><M> Bug #2159 - Modified the constructor with the missing parameter 20200624 SSDI: Seetharam
//<S><A> Implementation of restrictions with PPLCY dialog "Use Windows Loginin Name as "Login Name", Harika, 20201019
	if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
	{
		m_DefaultSettings[FEATURE_AUTHENTICATION].ResourceID = IDS_AUTHENTICATION_LOGIN;

	}
	else if (m_PropertySheetState.wPPlcyWinLogin == bool_false)
	{
		m_DefaultSettings[FEATURE_AUTHENTICATION].ResourceID = IDS_AUTHENTICATION_NONE;
	}
//<E> Implementation of restrictions with PPLCY dialog "Use Windows Loginin Name as "Login Name", Harika, 20201019
	pResourceIDInfo = GetResourceIDInfoOfControl(AUTHENTICATION);
	pControl = new  CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_AUTHENTICATION],
		m_ObjCShowMessage,
		IDC_CMB_AUTHENTICATION,
		m_PropertySheetState
	);
//<S><E> Bug #2159 - Modified the constructor with the missing parameter 20200624 SSDI: Seetharam
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_CMB_AUTHENTICATION, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
//<S><A>Added for Authentication image icon,2020.05.14 - SSDI:Goutham
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
//<S><E>Added for Authentication image icon,2020.05.14 - SSDI:Goutham
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_STC_UA_PASS)\n");
	}
	//<E>Added control class for authentication feature,20.03.24 SSDI:Goutham

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_USR_AUTH_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_USR_AUTH_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}


	pControl = new CCheckBox(
												GetResourceIDInfoOfControl(DEFAULT_JOB_USER_NAME),
												&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DEFAULT_JOB_USER_NAME],
												&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEFAULT_JOB_USER_NAME],
												m_hStringResourceHandle,
                                                m_DefaultSettings[FEATURE_DEFAULT_JOB_USER_NAME]
											);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_CHK_UNAME, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_CHK_UNAME)\n");
	}

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEFAULT_JOB_USER_NAME],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_UNAME_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_UNAME_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
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
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}
	pControl = new CCheckBox(
												GetResourceIDInfoOfControl(DEFAULT_JOB_JOB_NAME),
												&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DEFAULT_JOB_JOB_NAME],
												&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEFAULT_JOB_JOB_NAME],
												m_hStringResourceHandle,
                                                m_DefaultSettings[FEATURE_DEFAULT_JOB_JOB_NAME]
											);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_CHK_JNAME, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_CHK_JNAME)\n");
	}
	pControl = new CCheckBox(
												GetResourceIDInfoOfControl(AUTO_JOB_CONTROL_REVIEW),
												&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTO_JOB_CONTROL_REVIEW],
												&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTO_JOB_CONTROL_REVIEW],
												m_hStringResourceHandle,
                                                m_DefaultSettings[FEATURE_AUTO_JOB_CONTROL_REVIEW]
											);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_CHK_AUTO_JCREV, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_CHK_AUTO_JCREV)\n");
	}
	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTO_JOB_CONTROL_REVIEW],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_pPrinterName, &m_PropertySheetState, IDC_BTN_AUTO_JCREV_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_AUTO_JCREV_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}
	
	//<S><C>To Implement Task#3137,25-07-2024,SSDI:Manoj S
	//pControl = new CJobControlEdit(m_PropertySheetState.szJobName, JC_STR_JOBNAME );
	pControl = new CShEditJName(m_hStringResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, m_pOemPublicDevMode, m_pOemPrivateDevMode,
		m_PropertySheetState.szJobName, (JC_STR_JOBNAME + 1) * 2, IDC_EDT_JNAME);
	//<E>To Implement Task #3137,25-07-2024,SSDI:Manoj S
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_EDT_JNAME, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_EDT_JNAME)\n");
	}
	//<S><C>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	//Known Issue : Login Name is not concealed on UI when Printing Policy - Use Windows Login Name as ëLogin Nameí is checked.
	pControl = new CShEditLName(m_hStringResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, m_pOemPublicDevMode, m_pOemPrivateDevMode,
		m_PropertySheetState.szLoginNameUA, (JC_STR_LOGINNAME + 1) * 2, IDC_EDT_UA_LOGNAME);
	//pControl = new CJobControlEdit(m_PropertySheetState.szLoginNameUA, JC_STR_LOGINNAME);
	//<E>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_EDT_UA_LOGNAME, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CShEditLName)\n");
	}

	pControl = new CJobControlEdit(m_PropertySheetState.szUserName, JC_STR_JUNAME);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_EDT_UNAME, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_EDT_UNAME)\n");
	}

	pControl = new CJobControlEdit(m_PropertySheetState.szRetentionPassword,JC_STR_PINLENGTH,eNumberPasswordEditBox, TRUE);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_EDT_PIN, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_EDT_PIN)\n");
	}

	pControl = new CJobControlEdit(m_PropertySheetState.szLoginPasswordUA,JC_STR_LOGINPASS,ePasswordEditBox, TRUE);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_EDT_UA_PASS, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_EDT_UA_PASS)\n");
	}

	pControl = new CJobControlEdit(m_PropertySheetState.szUserNumber,JC_STR_PINLENGTH,eNumberPasswordEditBox, TRUE);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_EDT_ACNUM, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_EDT_ACNUM)\n");
	}

	//<S><A>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
	//--- Mimic Left Side
	pControl = new CMimicImage(IDC_BTN_SHARED_BBOX, m_hPropertySheetHandle, m_pFeatureInfoList,
		 m_hStringResourceHandle, m_hBitmapResourceHandle, m_pPrinterName, m_pOemPrivateDevMode, m_pWaterMarkIndex );
	if (NULL !=pControl)
    {
        hr = AddUIControl(IDC_BTN_SHARED_BBOX, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_BTN_SHARED_BBOX)\n");
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
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CMimicImage)\n");
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

//<E>Deleted control string for print release feature,20.03.26 SSDI:Goutham
    pControl = new CSysLinkCtrl(IDC_SYSLINK_TAB_DEFAULTS,m_hStringResourceHandle);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_SYSLINK_TAB_DEFAULTS, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_SYSLINK_TAB_DEFAULTS)\n");
	}
	//<E>newly added some controls,removed some controls and changed UI Controls as per UPD3 UI,20.02.14,SSDI:Goutham
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
VOID CJobHandlingTabPropertyPage::SetControlStrings(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	//<S><C>newly added some controls,removed some controls and changed  Control strings as per UPD3 UI,20.02.14,SSDI:Goutham
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
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
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
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}
	

	//StoredTo for Doc-Filing
	pControl = new CStaticText(IDS_IDC_STC_RETENTION_STOREDTO, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_RETENTION_STOREDTO, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_RETENTION_STOREDTO)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_AUTHENTICATION,m_hStringResourceHandle);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDS_IDC_STC_AUTHENTICATION, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_POINTER(IDS_IDC_STC_AUTHENTICATION)\n");
	}

	pControl = new CStaticText(IDC_GRP_JOBID,m_hStringResourceHandle);
	if (NULL != pControl)
    {
		hr = AddUIControl(IDC_GRP_JOBID, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_GRP_JOBID)\n");
	}
pControl = new CStaticText(IDS_IDC_STC_UA_PASS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_UA_PASS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_UA_PASS)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_UA_LOGNAME, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_UA_LOGNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_UA_PASS)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_ACNUM, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_ACNUM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_ACNUM)\n");
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
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
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
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
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
		slog.putLog("CJobHandlingTabPropertyPage::SetControlStrings -> NULL_OBJECT(IDC_STC_COPIES)\n");
	}
	//<E>newly added some controls,removed some controls and changed  Control strings as per UPD3 UI,20.02.14,SSDI:Goutham
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
VOID CJobHandlingTabPropertyPage::CreateMessageClass(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	m_ObjCShowMessage = new CShowMessage(hDlg, m_hStringResourceHandle);
	if (NULL == m_ObjCShowMessage)
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateMessageClass -> NULL_POINTER(m_ObjCShowMessage)\n");
	}
	m_ObjShowJobHandlingMessage = new CShowJobHandlingMessage(hDlg,m_hStringResourceHandle);
	if(NULL == m_ObjShowJobHandlingMessage)
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateMessageClass -> NULL_POINTER(m_ObjShowJobHandlingMessage)\n");
	}
}
//=============================================================================
// function
//      CreateDepenedentContolMap
//
// return value
//     NILL
//
// outline
//    Creates Map of dependent controls.
//=============================================================================
VOID CJobHandlingTabPropertyPage::CreateDepenedentContolMap(VOID)
{
	
	UIControlMap::const_iterator iterUIComponents;
	CUIControl *pControl = NULL ;
//	iterUIComponents = m_UIControls.find(IDC_CHK_RETENTION);
   iterUIComponents = m_UIControls.find(IDC_CMB_RETENTION);
	if(m_UIControls.end() != iterUIComponents)
	{
		pControl  = iterUIComponents->second;
//<S><C> modified Ids and commented unwanted code,20.03.24 SSDI:Goutham
		INT iRetentionCheckBoxDependentControlIDs[] = {
										IDS_LIST_US_RET_HOLD,
										IDS_LIST_US_RET_AFTER_PRN,
										IDS_LIST_US_RET_SAMPLE,
										//IDC_CHK_PIN,
										//IDC_CMB_QUICKFILE,
										//IDC_CMB_MAIN_FOLDER
										};
//<E> modified Ids and commented unwanted code,20.03.24 SSDI:Goutham
//<S><C>commented unwanted code,20.03.24 SSDI:Goutham
	//	m_ControlIDs.push_back(IDC_CHK_RETENTION);
//<E>commented unwanted code,20.03.24 SSDI:Goutham
		pControl->CreateDependentControlMap(iRetentionCheckBoxDependentControlIDs,countof(iRetentionCheckBoxDependentControlIDs));
	}
	//iterUIComponents = m_UIControls.find(IDC_CHK_PIN);
	if(m_UIControls.end() != iterUIComponents)
	{
//<S><C>commented unwanted code,20.03.24 SSDI:Goutham
	//	m_ControlIDs.push_back(IDC_CHK_PIN);
//<E>commented unwanted code,20.03.24 SSDI:Goutham
		pControl  = iterUIComponents->second;
//<S><C>commented unwanted code,20.03.24 SSDI:Goutham
		pControl->CreateDependentControlMap(IDC_EDT_PIN);
//<E>commented unwanted code,20.03.24 SSDI:Goutham
	}
	//iterUIComponents = m_UIControls.find(IDC_CHK_UA_LOGNAME);
    iterUIComponents = m_UIControls.find(IDC_STC_UA_LOGNAME);
	if(m_UIControls.end() != iterUIComponents)
	{
		//<S><D> To fix Coverity issue 90535, Harika	20201211
		//pControl  = iterUIComponents->second;
		//<E> To fix Coverity issue 90535, Harika	20201211
	//	INT iLoginNameCheckBoxDependentControlIDs[] = {
	//										IDC_EDT_UA_LOGNAME,
	//										IDC_EDT_UA_PASS
	//	};
	
	//	m_ControlIDs.push_back(IDC_CHK_UA_LOGNAME);
        m_ControlIDs.push_back(IDC_STC_UA_LOGNAME);
	//	pControl->CreateDependentControlMap(iLoginNameCheckBoxDependentControlIDs,countof(iLoginNameCheckBoxDependentControlIDs));
	}
	iterUIComponents = m_UIControls.find(IDC_CHK_ACNUM);
	if(m_UIControls.end() != iterUIComponents)
	{
		m_ControlIDs.push_back(IDC_CHK_ACNUM);
		pControl  = iterUIComponents->second;

		pControl->CreateDependentControlMap(IDC_EDT_ACNUM);
	}
//	iterUIComponents = m_UIControls.find(IDC_CHK_UA_PASS);
    	iterUIComponents = m_UIControls.find(IDC_STC_UA_PASS);
	if(m_UIControls.end() != iterUIComponents)
	{
//	m_ControlIDs.push_back(IDC_CHK_UA_PASS);
       m_ControlIDs.push_back(IDC_STC_UA_PASS);
		pControl  = iterUIComponents->second;

		pControl->CreateDependentControlMap(IDC_EDT_UA_PASS);
	}
    //<S><D>Deleted unused code,20.03.26 SSDI:Goutham
	//iterUIComponents = m_UIControls.find(IDC_CHK_UNAME);
    //<E>Deleted unused code,20.03.26 SSDI:Goutham
	if(m_UIControls.end() != iterUIComponents)
	{
		m_ControlIDs.push_back(IDC_CHK_UNAME);
		pControl  = iterUIComponents->second;

		pControl->CreateDependentControlMap(IDC_EDT_UNAME);
	}
	iterUIComponents = m_UIControls.find(IDC_CHK_JNAME);
	if(m_UIControls.end() != iterUIComponents)
	{
		m_ControlIDs.push_back(IDC_CHK_JNAME);
		pControl  = iterUIComponents->second;

		pControl->CreateDependentControlMap(IDC_EDT_JNAME);
	}
}
//=============================================================================
// function
//      ActivateUIControl
//
// return value
//     NILL
//
// outline
//     activates UI control.
//=============================================================================
HRESULT
CJobHandlingTabPropertyPage::ActivateUIControl(
    _In_ CONST HWND   hDlg
    )
{
	JobControlDataToPropState();
	__super::ActivateUIControl(hDlg);

	UpdateUserSetting(hDlg, IDC_CMB_SHARED_USRSET);
	SetTabState(hDlg);
	return S_OK;
}
//=============================================================================
// function
//      DeActivateUIControl
//
// return value
//     NILL
//
// outline
//     deactivates UI control.
//=============================================================================
HRESULT
CJobHandlingTabPropertyPage::DeActivateUIControl(
    _In_ CONST HWND   hDlg
    )
{
	if(0 != m_ObjShowJobHandlingMessage->Message(hDlg, &m_PropertySheetState,m_pFeatureInfoList))
	{
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
		return TRUE;
	}
	return __super::DeActivateUIControl(hDlg);
	

}

//=============================================================================
// function
//        OnPsnKillActive
//
// parameters
//        hWnd         
//
// return value
//        TRUE
//       FALSE
//
// outline
//        WM_NOTIFY - PSN_KILLACTIVE
//=============================================================================
LRESULT CJobHandlingTabPropertyPage::OnPsnKillActiveEX(HWND hWnd)
{
	LRESULT			lRes = FALSE;

	// this function is related to message box display 
	if (0 == m_ObjShowJobHandlingMessage->Message(hWnd, &m_PropertySheetState, m_pFeatureInfoList))
	{
		//<S><C>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
		//Known Issue : Printing Preferences window is getting closed, 
		//when OK is clicked on any validation message box in Job-Handling tab. 
		SetWindowLongPtr(hWnd, DWLP_MSGRESULT, TRUE);
		//<E>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
		lRes = TRUE;
	}

	return lRes;
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
HRESULT	CJobHandlingTabPropertyPage::SendCommand(
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
	__super::SendCommand(hDlg,wParam);
//<S><A>To fix redmine bug #2164 2020.06.12 Sneha TG
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
		OnCbnSelchange(hDlg, 0, wContolID);
	}
	break;
	default:
		break;
	}
//<E>To fix redmine bug #2164 2020.06.12 Sneha TG
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
		//Fixed Coverity #91468,13-08-2021,SSDI:Chanchal Singla
		if (NULL != wTempFeatureInfoList)
		{
			delete wTempFeatureInfoList;
			wTempFeatureInfoList = NULL;
		}
		//Fixed Coverity #91435,13-08-2021,SSDI:Chanchal Singla
		if (NULL != wFeatureInfoList_Pre)
		{
			delete wFeatureInfoList_Pre;
			wFeatureInfoList_Pre = NULL;
		}
	}
	//<E> To Implement Factory Default functionality,2021-April-09,SSDI:Chanchal Singla

	return S_OK;
}
//=============================================================================
// function
//      SetTabState
//
// return value
//     VOID
//
// outline
//     To set states of all controls.
//=============================================================================
VOID CJobHandlingTabPropertyPage::SetTabState( 
		_In_ CONST HWND   hDlg,
       	_In_ CONST INT iCtrlID
		)
{
	SetUserSettingState(hDlg, IDC_CMB_SHARED_USRSET);

	m_PropertySheetState.wRetentionMode = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_RETENTION, m_PropertySheetState.wRetentionMode);

	m_PropertySheetState.wDocFileMode = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uChangedOptionIndex;
	InitializeFeatureOption(hDlg, IDC_CMB_RETENTION_STOREDTO, m_PropertySheetState.wDocFileMode);

	//<S><C>to Fix Bug #3487,25-03-2021,SSDI:Chanchal Singla
	//Document Filling drop down is enabled when Print Release checkbox is On
	m_PropertySheetState.wPrintRelease = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex;;
	CheckDlgButton(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);
	SetCheckBoxValue(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);
//<E>to Fix Bug #3487,25-03-2021,SSDI:Chanchal Singla
//<S><A> Added to retain UserName and JobName Checkbox values when we switch between tabs, Harika-20210422
	//m_PropertySheetState.wUseUName = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEFAULT_JOB_USER_NAME].uChangedOptionIndex;;
	CheckDlgButton(hDlg, IDC_CHK_UNAME, m_PropertySheetState.wUseUName);
	SetCheckBoxValue(hDlg, IDC_CHK_UNAME, m_PropertySheetState.wUseUName);

	//m_PropertySheetState.wUseJName = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DEFAULT_JOB_JOB_NAME].uChangedOptionIndex;;
	CheckDlgButton(hDlg, IDC_CHK_JNAME, m_PropertySheetState.wUseJName);
	SetCheckBoxValue(hDlg, IDC_CHK_JNAME, m_PropertySheetState.wUseJName);
//<S><A> Added to retain UserName and JobName Checkbox values when we switch between tabs, Harika-20210422

	ShowConstraintsOnAutoJobReview(hDlg, IDC_CHK_AUTO_JCREV);

	ShowConstraintsOnPrnRelease(hDlg, IDC_CHK_PRINTRELEASE);
	ShowConstraintsOnRetention(hDlg, IDC_CMB_RETENTION);
	ShowConstraintsOnAuthentication(hDlg, IDC_CMB_AUTHENTICATION);
	ShowConstraintsOnUserName(hDlg, IDC_CHK_UNAME);
	ShowConstraintsOnJobName(hDlg, IDC_CHK_JNAME);
	memcpy(m_ppi->pps, &m_PropertySheetState, sizeof(PROPSTATE));

	//if (IsPrinterOperators(m_pPrinterName) == FALSE)
	//	DisableAllCtrls();
}

// <S><A>Added To handle OnBnClicked on controls, 2020.03.24 - SSDI:Goutham
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
LRESULT CJobHandlingTabPropertyPage::OnBnClicked(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	LRESULT			res = 0;
	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	switch (wCtrlID)
	{
//<S><A> added on button click event for print release and auto job control review features,20.03.26 SSDI:Goutham
	case IDC_CHK_PRINTRELEASE:
	{
		pControl = (*pComponents)[IDC_CHK_PRINTRELEASE];
		((CCheckBox*)pControl)->OnBnClicked(wCtrlID, hDlg);
		m_PropertySheetState.wPrintRelease = IsDlgButtonChecked(hDlg, wCtrlID);
		// --- DM_PRINTSHARE
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wPrintRelease == bool_true) ?
			(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_PRINTSHARE) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_PRINTSHARE);

		if (m_PropertySheetState.wPrintRelease == TRUE)
		{
			m_ControlState.cCmbRetention = SCUI_STATE_SHOW;
			m_ControlState.cRetention_I = SCUI_STATE_ON;
		}
		else
		{
			m_ControlState.cCmbRetention = SCUI_STATE_ON;
			m_ControlState.cRetention_I = SCUI_STATE_OFF;
		}
		SetControlState(hDlg, IDC_STC_RETENTION, m_ControlState.cCmbRetention);
		SetControlState(hDlg, IDC_CMB_RETENTION, m_ControlState.cCmbRetention);
		SetControlState(hDlg, IDC_BTN_RETENTION_I, m_ControlState.cRetention_I);
	}
	break;
	case IDC_CHK_AUTO_JCREV:
	{
		pControl = (*pComponents)[IDC_CHK_AUTO_JCREV];
		((CCheckBox*)pControl)->OnBnClicked(wCtrlID, hDlg);
		m_PropertySheetState.wAutoJCR = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTO_JOB_CONTROL_REVIEW].uChangedOptionIndex;
	}
	break;
//<S><E> added on button click event for print release and auto job control review features,20.03.26 SSDI:Goutham
	case IDC_BTN_SHARED_OK:
		//<S><A> To fix ssdi bug#3782, Harika 2021/5/17
		if (m_PropertySheetState.wLoginNameSSO == bool_true)
		{
			DWORD dwSSONameSize = (JC_STR_JUNAME + 1) * 2;
			::GetUserName(m_PropertySheetState.szSingleSignOnName, &dwSSONameSize);
			::wcscpy_s(m_PropertySheetState.szLoginNameUA, _countof(m_PropertySheetState.szUserName) / sizeof(WCHAR), m_PropertySheetState.szSingleSignOnName);
		}
		//<S><A> To fix ssdi bug#3782, Harika 2021/5/17
		PropStateToJobControlData();
		if (0 != m_ObjShowJobHandlingMessage->Message(hDlg, &m_PropertySheetState, m_TempJobControl.wFieldJC))
		{
			SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
			return TRUE;
		}
	//	memcpy_s(m_pJobControl, sizeof(JOBCONTROL), &m_TempJobControl, sizeof(JOBCONTROL));
	//	__super::OnBnClicked(hDlg, hWndCtrl, wCtrlID);
		break;
	case IDC_BTN_SHARED_CANCEL:
	{
		//StoredToSetTabDefaults(hDlg);
		//::ZeroMemory(m_PropertySheetState.szFolderPass, sizeof(m_PropertySheetState.szFolderPass));
		CShowMessage showmessage(hDlg, m_hStringResourceHandle);
		if (IDNO == showmessage.Message(hDlg, IDS_MESTITLE_1, IDS_MESSAGE_65, MB_YESNO | MB_ICONQUESTION))
			return res;
	}
	//	__super::OnBnClicked(hDlg, hWndCtrl, wCtrlID);
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
	}
	break;
	case IDC_CHK_UNAME:
	{
		pControl = (*pComponents)[IDC_CHK_UNAME];
		((CCheckBox*)pControl)->OnBnClicked(wCtrlID, hDlg);
		m_PropertySheetState.wUseUName = IsDlgButtonChecked(hDlg, IDC_CHK_UNAME);
		// --- DM_ALWAYSUSE_USERID
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wUseUName == bool_true) ?
			(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_ALWAYSUSE_USERID) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_ALWAYSUSE_USERID);

		if (m_PropertySheetState.wUseUName == TRUE)
		{
			SetControlState(hDlg, IDC_EDT_UNAME, SCUI_STATE_ON);
		}
		else
		{
			SetControlState(hDlg, IDC_EDT_UNAME, SCUI_STATE_SHOW);
		}
	}
	break;
	case IDC_CHK_JNAME:
	{
	   pControl = (*pComponents)[IDC_CHK_JNAME];
	   ((CCheckBox*)pControl)->OnBnClicked(wCtrlID, hDlg);
	   m_PropertySheetState.wUseJName = IsDlgButtonChecked(hDlg, IDC_CHK_JNAME);
	   // --- DM_ALWAYSUSE_JOBID
	   m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wUseJName == bool_true) ?
		   (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_ALWAYSUSE_JOBID) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_ALWAYSUSE_JOBID);

	   if (m_PropertySheetState.wUseJName == TRUE)
		{
			SetControlState(hDlg, IDC_EDT_JNAME, SCUI_STATE_ON);
		}
		else
		{
			SetControlState(hDlg, IDC_EDT_JNAME, SCUI_STATE_SHOW);
		}
	}
	break;
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
	case IDC_BTN_RETENTION_I:
	case IDC_BTN_AUTO_JCREV_I:
	case IDC_BTN_USR_AUTH_I:
	case IDC_BTN_UNAME_I:
	case IDC_BTN_PRINTRELEASE_I:
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
	//<E>to Fix Redmine Bug #2240 Issue 3,03.02.2022,SSDI:Chanchal Singla
	default:
	//	__super::OnBnClicked(hDlg, hWndCtrl, wCtrlID);
		break;
	}
	//SetTabState(hDlg,wCtrlID);
	return S_OK;
}
// <E>Added To handle OnBnClicked on controls, 2020.03.24 - SSDI:Goutham
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
LRESULT CJobHandlingTabPropertyPage::OnCbnSelchange(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	short			wPre = 0;


	switch (wCtrlID)
	{
	case IDC_CMB_RETENTION:
	{
		m_PropertySheetState.wRetentionMode = (SHORT)GetValue(hDlg, IDC_CMB_RETENTION);
		if (m_PropertySheetState.wRetentionMode == retention_normal) {
			m_PropertySheetState.wRetention = bool_false;
		}
		else {
			m_PropertySheetState.wRetention = bool_true;
		}
		//<S><A>to fix #3762,20210520,SSDI:Chanchal Singla
		//Document Filing is not set to 'Hold After Print' when Quick file is set.
		if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption, "QUICKFILE"))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption = "MAINFOLDER";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uChangedOptionIndex = eSecondOption;
			m_PropertySheetState.wDocFileMode = eSecondOption;
		}
		//<E>to fix #3762,20210520,SSDI:Chanchal Singla
	}
	break;
	//<S><A> to fix bug #2332 2020.06.12 Harika
	case IDC_CMB_AUTHENTICATION:
	{
		short wAuthen = (SHORT)GetValue(hDlg, wCtrlID);
		ChangedAuthentication(wAuthen);
	}
	break;
	//<E> to fix bug #2332 2020.06.12 Harika
	//<S><A> To open custom folder dialog,2020.08.11 Goutham
	case IDC_CMB_RETENTION_STOREDTO:
	{
		m_PropertySheetState.wDocFileMode = (SHORT)GetValue(hDlg, wCtrlID);
		//<S><A>to fix #3762,20210520,SSDI:Chanchal Singla
		//Document Filing is not set to 'Hold After Print' when Quick file is set.
		if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption, "QUICKFILE"))
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption = "HOLDAFTERPRINT";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS].uChangedOptionIndex = eThirdOption;
			m_PropertySheetState.wRetentionMode = eSecondOption;
		}
		//<E>to fix #3762,20210520,SSDI:Chanchal Singla
		if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption, "CUSTOMFOLDER"))
		{
			OnCustomFolderDlg(hDlg);
		}
	}
	break;
	//<S><E> To open custom folder dialog,2020.08.11 Goutham

	case IDC_CMB_SHARED_USRSET:
		OnUsSelect(hDlg, m_pPrinterName, IDC_CMB_SHARED_USRSET);

		break;
	default:
		break;
	}

	res = 1;

	return res;
}
//=============================================================================
// function
//      UpdateEditBoxContent
//
// return value
//     VOID
//
// outline
//     Updates edit box contents .
//=============================================================================
VOID CJobHandlingTabPropertyPage::UpdateEditBoxContent(
    _In_ CONST HWND hDlg,	_In_ CONST INT iCtrlID)
{
	WCHAR* pszEditBoxContent = NULL ;
    //<S><D>Deleted unused code,20.03.26 SSDI:Goutham
	if ((IDC_CHK_PIN == iCtrlID) && (BST_UNCHECKED == IsDlgButtonChecked(hDlg, iCtrlID)))
	{
		pszEditBoxContent = m_PropertySheetState.szRetentionPassword;
		ClearEditBoxContent(IDC_EDT_PIN, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szRetentionPassword));
	}
    //<E>Deleted unused code,20.03.26 SSDI:Goutham
	if((IDC_CHK_UNAME == iCtrlID)&& (BST_UNCHECKED == IsDlgButtonChecked(hDlg, iCtrlID)))
	{
		pszEditBoxContent = m_PropertySheetState.szUserName;
		ClearEditBoxContent(IDC_EDT_UNAME,hDlg,pszEditBoxContent,sizeof(m_PropertySheetState.szUserName));
	}
	if((IDC_CHK_JNAME == iCtrlID)&& (BST_UNCHECKED == IsDlgButtonChecked(hDlg, iCtrlID)))
	{
		pszEditBoxContent = m_PropertySheetState.szJobName;
		ClearEditBoxContent(IDC_EDT_JNAME,hDlg,pszEditBoxContent,sizeof(m_PropertySheetState.szJobName));
	}
	//<S><A> Added to clear edit boxed when authentication and Document filing is none. 3/6/2020 K M Harika
	if ((IDC_CMB_AUTHENTICATION== iCtrlID) && (authentication_none == SendDlgItemMessage(hDlg, IDC_CMB_AUTHENTICATION, CB_GETCURSEL, 0, 0)))
	{
		pszEditBoxContent = m_PropertySheetState.szLoginNameUA;
		ClearEditBoxContent(IDC_EDT_UA_LOGNAME, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginNameUA));
		pszEditBoxContent = m_PropertySheetState.szLoginPasswordUA;
		ClearEditBoxContent(IDC_EDT_UA_PASS, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginPasswordUA));
		pszEditBoxContent = m_PropertySheetState.szUserNumber;
		ClearEditBoxContent(IDC_EDT_ACNUM, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserNumber));
		//<S><D>Deleted to fix redmine bug #2851,2020.10.29,SSDI:Goutham
		//pszEditBoxContent = m_PropertySheetState.szRetentionPassword;
		//ClearEditBoxContent(IDC_EDT_PIN, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserNumber));
		//<S><E>Deleted to fix redmine bug #2851,2020.10.29,SSDI:Goutham
	}
//<S><A>To fix redmine bug #2296 issue 2 2020.06.12 Sneha TG
	if ((IDC_CMB_AUTHENTICATION == iCtrlID) && (authentication_lname == SendDlgItemMessage(hDlg, IDC_CMB_AUTHENTICATION, CB_GETCURSEL, 0, 0)))
	{
		pszEditBoxContent = m_PropertySheetState.szLoginPasswordUA;
		ClearEditBoxContent(IDC_EDT_UA_PASS, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginPasswordUA));
		pszEditBoxContent = m_PropertySheetState.szUserNumber;
		ClearEditBoxContent(IDC_EDT_ACNUM, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserNumber));
	}
	if ((IDC_CMB_AUTHENTICATION == iCtrlID) && (authentication_lname_pswd == SendDlgItemMessage(hDlg, IDC_CMB_AUTHENTICATION, CB_GETCURSEL, 0, 0)))
	{
		pszEditBoxContent = m_PropertySheetState.szUserNumber;
		ClearEditBoxContent(IDC_EDT_ACNUM, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserNumber));
	}
	if ((IDC_CMB_AUTHENTICATION == iCtrlID) && (authentication_acnum == SendDlgItemMessage(hDlg, IDC_CMB_AUTHENTICATION, CB_GETCURSEL, 0, 0)))
	{
		pszEditBoxContent = m_PropertySheetState.szLoginNameUA;
		ClearEditBoxContent(IDC_EDT_UA_LOGNAME, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginNameUA));
		pszEditBoxContent = m_PropertySheetState.szLoginPasswordUA;
		ClearEditBoxContent(IDC_EDT_UA_PASS, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginPasswordUA));
	}
	if ((IDC_CMB_AUTHENTICATION == iCtrlID) && (authentication_sso == SendDlgItemMessage(hDlg, IDC_CMB_AUTHENTICATION, CB_GETCURSEL, 0, 0)))
	{
		pszEditBoxContent = m_PropertySheetState.szLoginNameUA;
		ClearEditBoxContent(IDC_EDT_UA_LOGNAME, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginNameUA));
		pszEditBoxContent = m_PropertySheetState.szLoginPasswordUA;
		ClearEditBoxContent(IDC_EDT_UA_PASS, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginPasswordUA));
		pszEditBoxContent = m_PropertySheetState.szUserNumber;
		ClearEditBoxContent(IDC_EDT_ACNUM, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserNumber));
	}
//<E>To fix redmine bug #2296 issue 2 2020.06.12 Sneha TG
	if ((IDC_CMB_RETENTION == iCtrlID) && (retention_normal == SendDlgItemMessage(hDlg, IDC_CMB_RETENTION, CB_GETCURSEL, 0, 0)))
	{
		pszEditBoxContent = m_PropertySheetState.szRetentionPassword;
		ClearEditBoxContent(IDC_EDT_PIN, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szRetentionPassword));
	}
	//<E> Added to clear edit boxed when authentication and Document filing is none. 3/6/2020 K M Harika
}


//=============================================================================
// function
//      ClearEditBoxContent
//
// return value
//     VOID
//
// outline
//     set edit box content to zero .
//=============================================================================
VOID CJobHandlingTabPropertyPage::ClearEditBoxContent(_In_ CONST INT   iCtrlID,
    _In_ CONST HWND hDlg,
	WCHAR* pszEditBoxContent,
	size_t sEditBoxSizeInBytes)
{
	SecureZeroMemory(pszEditBoxContent,sEditBoxSizeInBytes);
	UIControlMap::const_iterator iterUIComponents;
	iterUIComponents = m_UIControls.find(iCtrlID);
	if(m_UIControls.end() != iterUIComponents)
	{
		CJobControlEdit *pJobControlEdit  = static_cast<CJobControlEdit*>(iterUIComponents->second);
		pJobControlEdit->ActivateUIControl(iCtrlID,hDlg);
	}
	
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
VOID CJobHandlingTabPropertyPage::JobControlDataToPropState()
{	
	//<S><A>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
	memcpy( &m_PropertySheetState, m_ppi->pps, sizeof(PROPSTATE));
	//<E>To Fix Job Handling Tab Propstate Issue and Designing Issue of UD3L Driver,2021-05-05,SSDI:Chanchal Singla
}
//=============================================================================
// function
//      PropStateToWatermarkData
//
// return value
//     NILL
//
// outline
//   Updates Job control data structure  based on Property sheet structure.
//=============================================================================
VOID CJobHandlingTabPropertyPage::PropStateToJobControlData()
{
	::lstrcpyn((LPWSTR)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byFolderPassword, m_PropertySheetState.szFolderPass, wcslen(m_PropertySheetState.szFolderPass) * sizeof(WCHAR));
	::lstrcpyn((LPWSTR)m_pOemPrivateDevMode->scpi.ext.JobCtrl.szFolderPass, m_PropertySheetState.szFolderPass, wcslen(m_PropertySheetState.szFolderPass) * sizeof(WCHAR));

	// --- DM_DOCFILING
	m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wRetention == bool_true) ?
		(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_DOCFILING) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_DOCFILING);

	memcpy_s(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname,_countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname)*sizeof(WCHAR),
		m_PropertySheetState.szJobName,_countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname)*sizeof(WCHAR));
	
	memcpy_s(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName,_countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName)*sizeof(WCHAR),
		m_PropertySheetState.szLoginNameUA,_countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName)*sizeof(WCHAR));
	memcpy_s(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername,_countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername)*sizeof(WCHAR),
		m_PropertySheetState.szUserName,_countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername)*sizeof(WCHAR));

	GetEncodedPassWordData((CHAR*)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byUserNumber,
								m_PropertySheetState.szUserNumber,
								JC_STR_PINLENGTH,
								USER_NUMBER_LENGTH
								);
	GetEncodedPassWordData((CHAR*)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetentionPassword,
								m_PropertySheetState.szRetentionPassword,
								JC_STR_PINLENGTH,
								RETENTION_PASSWORD_LENGTH
								);
	GetEncodedPassWordData((CHAR*)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byLoginPassword,
								m_PropertySheetState.szLoginPasswordUA,
								JC_STR_LOGINPASS,
								LOGIN_PASSWORD_LENGTH
								);
//<S><A>To Fix Redmine bug #2741 issue1 and issue2,10-21-2020,SSDI:Goutham
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.nDocFilingFolder = m_PropertySheetState.wDocFileFolder;
//<S><E>To Fix Redmine bug #2741 issue1 and issue2,10-21-2020,SSDI:Goutham
}
//=============================================================================
// function
//      SaveFeatureOptions
//
// return value
//     NILL
//
// outline
//   Saves feature options set by user in  Job Handling tab to devmode.
//=============================================================================
VOID CJobHandlingTabPropertyPage::SaveFeatureOptions()
{
	PropStateToJobControlData();
//<S><A>Added to retain password value from registry to devmode
	//GetLastSelectedPassword();
//<S><E>TAdded to retain password value from registry to devmode

__super::SaveFeatureOptions();
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
//        ê˜TRUE
//         FALSE
//
// outline
//        WM_COMMAND - OnPsnApplyEX
//=============================================================================
LRESULT CJobHandlingTabPropertyPage::OnPsnApplyEX(HWND hWnd, long lCtrlID, LPPSHNOTIFY ppsn)
{
	LRESULT			res = 0;
	PREGJCINFO		pregjc = NULL;

	CShRegJC		*preg = NULL;

	memcpy(&m_PropertySheetState, m_ppi->pps, sizeof(PROPSTATE));
	
	pregjc = new REGJCINFO;
	if (pregjc == NULL)
		goto EXIT;

	preg = new CShRegJC(m_hStringResourceHandle, m_pPrinterName);
	if (preg == NULL)
		goto EXIT;

	SecureZeroMemory(pregjc, sizeof(REGJCINFO));

	if (PropStateToRegJCInfo(&m_PropertySheetState, pregjc, m_pPrinterName) != FALSE)
		(*preg).WriteJCData(m_pPrinterName, pregjc, JC_ALL, m_PropertySheetState.wPPlcyWinLogin);
	

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

	return res;
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
void CJobHandlingTabPropertyPage::GetEncodedPassWordData(CHAR* byEncodedPasswordData,WCHAR* pInputPasswordData,size_t sMaximumInputStringLength,size_t sOutputStringLength)
{
	size_t sInputStringLength = wcsnlen_s(pInputPasswordData,sMaximumInputStringLength);
	if(0 < sInputStringLength)
	{
		CHAR strMultiByte[MAX_ENCODING_STRING_LENGTH*2 + 1] = {0};
		WideCharToMultiByte(CP_ACP, 0, pInputPasswordData, -1, strMultiByte,MAX_ENCODING_STRING_LENGTH*2 + 1 , NULL, NULL);
		Encode_AES(byEncodedPasswordData,strMultiByte);
		if(MAX_ENCODING_STRING_LENGTH < sInputStringLength)
		{
			Encode_AES(byEncodedPasswordData +  MAX_ENCODING_STRING_LENGTH ,strMultiByte + sizeof(char)*MAX_ENCODING_STRING_LENGTH);
		}
	}
	else
	{
		SecureZeroMemory(byEncodedPasswordData,sOutputStringLength);
	}
	
}
//=============================================================================
// function
//      GetDecodedPassWordData
//
// return value
//     NILL
//
// outline
//   Decodes AES encrypted password data.
//=============================================================================
void CJobHandlingTabPropertyPage::GetDecodedPassWordData(CHAR* byEncodedPasswordData,WCHAR* pOuputPasswordData,size_t sEncodedStringLength)
{
	size_t sInputStringLength = strnlen_s(byEncodedPasswordData,sEncodedStringLength);
	if(0 < sInputStringLength)
	{
		CHAR strMultiByte[MAX_ENCODING_STRING_LENGTH *2+1] = {0};
		Decode_AES(strMultiByte,byEncodedPasswordData);
		if(MAX_ENCODING_STRING_LENGTH < sInputStringLength )
		{
			Decode_AES(strMultiByte + sizeof(char)*MAX_ENCODING_STRING_LENGTH,byEncodedPasswordData + MAX_ENCODING_STRING_LENGTH);
		}
		MultiByteToWideChar(CP_ACP, 0, strMultiByte, -1,pOuputPasswordData,MAX_ENCODING_STRING_LENGTH*2);
	}
	else
	{
		SecureZeroMemory(pOuputPasswordData, MAX_ENCODING_STRING_LENGTH);
	}
}

//=============================================================================
// function
//      SetTabDefaults
//
// return value
//     NILL
//
// outline
//  Sets all controls in job handling tab  to their default values..
//=============================================================================
HRESULT CJobHandlingTabPropertyPage::SetTabDefaults(_In_ CONST HWND   hDlg)
{
	__super::SetTabDefaults(hDlg);
	

	m_PropertySheetState.wUsePIN = (m_DefaultSettings[FEATURE_RETENTION_PASSWORD].iDevmodeValue == bool_false) ?
						bool_false : bool_true;
	m_PropertySheetState.wUseUName = (m_DefaultSettings[FEATURE_DEFAULT_JOB_USER_NAME].iDevmodeValue == bool_false) ?
		bool_false : bool_true;
	m_PropertySheetState.wUseJName = (m_DefaultSettings[FEATURE_DEFAULT_JOB_JOB_NAME].iDevmodeValue == bool_false) ?
		bool_false : bool_true;

	m_PropertySheetState.wUseAcNum = 0;
	m_PropertySheetState.wLoginPass = 0;
	m_PropertySheetState.wLoginNameSSO = 0;
	if (m_PropertySheetState.wPPlcyWinLogin == bool_false)
	{
		m_PropertySheetState.wLoginName = 0;
	}
	else {
		m_PropertySheetState.wLoginName = 1;
		DWORD dwSize = sizeof(m_PropertySheetState.szLoginNameUA) / sizeof(WCHAR);
		GetUserName(m_PropertySheetState.szLoginNameUA, &dwSize);
	}

	SecureZeroMemory(m_PropertySheetState.szPIN, sizeof(m_PropertySheetState.szPIN));
	SecureZeroMemory(m_PropertySheetState.szUserNumber, sizeof(m_PropertySheetState.szUserNumber));
	SecureZeroMemory(m_PropertySheetState.szJobName, sizeof(m_PropertySheetState.szJobName));
	SecureZeroMemory(m_PropertySheetState.szLoginPasswordUA, sizeof(m_PropertySheetState.szLoginPasswordUA));
	SecureZeroMemory(m_PropertySheetState.szRetentionPassword, sizeof(m_PropertySheetState.szRetentionPassword));
	//ClearEditBoxContent(IDC_EDT_PIN, hDlg, m_PropertySheetState.szRetentionPassword, sizeof(m_PropertySheetState.szRetentionPassword));
	//ClearEditBoxContent(IDC_EDT_UA_PASS, hDlg, m_PropertySheetState.szLoginPasswordUA, sizeof(m_PropertySheetState.szLoginPasswordUA));
	//ClearEditBoxContent(IDC_EDT_ACNUM, hDlg, m_PropertySheetState.szUserNumber, sizeof(m_PropertySheetState.szUserNumber));

	wcscpy(m_PropertySheetState.szRetentionPassword, L"");
	GetEncodedPassWordData((CHAR*)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetentionPassword,
		m_PropertySheetState.szRetentionPassword,
		JC_STR_PINLENGTH,
		RETENTION_PASSWORD_LENGTH
	);
	/*UIControlMap::const_iterator iComboIterator = m_UIControls.find(IDC_EDT_PIN);
	if (iComboIterator != m_UIControls.end())
	{
		CJobControlEdit * pControl = static_cast<CJobControlEdit*>(iComboIterator->second);
		if (pControl != NULL)
		{
			pControl->setVal(m_PropertySheetState.szRetentionPassword);
		}
	}*/

	if (m_PropertySheetState.wLoginName == 0)
	{
		SecureZeroMemory(m_PropertySheetState.szLoginNameUA, sizeof(m_PropertySheetState.szLoginNameUA));
	}

	if (m_PropertySheetState.wUseUName == 0)
	{
		SecureZeroMemory(m_PropertySheetState.szUserName, sizeof(m_PropertySheetState.szUserName));
	}
	if (m_PropertySheetState.wLoginNameSSO == 0)
	{
		SecureZeroMemory(m_PropertySheetState.szSingleSignOnName, sizeof(m_PropertySheetState.szSingleSignOnName));
	}
	m_PropertySheetState.wPrintRelease = (m_DefaultSettings[FEATURE_PRINTRELEASE].iDevmodeValue == bool_false) ?
		bool_false : bool_true;
	//<S><A>To fix redmine bug #2741,2020.10.29,SSDI:Goutham
	StoredToSetTabDefaults(hDlg);
	::ZeroMemory(m_PropertySheetState.szFolderPass, sizeof(m_PropertySheetState.szFolderPass));
     //<S><E>To fix redmine bug #2741,2020.10.29,SSDI:Goutham
//<S><E> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam

	SetTabState(hDlg);
	//<S><A> to fix bug #2332 2020.06.12 Harika
	//ShowConstraintsOnAuthentication(hDlg, IDC_CMB_AUTHENTICATION);
	//ShowConstraintsOnRetention(hDlg, IDC_CMB_RETENTION);
	////<E> to fix bug #2332 2020.06.12 Harika
	//ShowConstraintsOnPrnRelease(hDlg, IDC_CHK_PRINTRELEASE);
	//<S><A> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam

	return S_OK ;
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
HRESULT CJobHandlingTabPropertyPage::OnDrawItem(_In_ CONST HWND   hDlg, _In_ CONST WPARAM wParam, _In_ CONST LPARAM lParam)
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
				//Fixed Coverity issue - 90528 - SSDI:Seetharam-20201006
				//else
				//{
				//	slog.putLog("CJobHandlingTabPropertyPage::SetPamphletIndex -> NULL_OBJECT(CCombo)\n");
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
		case IDC_BTN_RETENTION_I:
		case IDC_BTN_AUTO_JCREV_I:
		case IDC_BTN_USR_AUTH_I:
		case IDC_BTN_UNAME_I:
		case IDC_BTN_PRINTRELEASE_I:
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

//<S><A> To add the Show Constraints On Retention  function for hiding and showing Pin edit box ,11-02-2020,SSDI:Goutham
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
VOID CJobHandlingTabPropertyPage::ShowConstraintsOnRetention(HWND hDlg, INT iCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UINT Retention = static_cast<UINT>(SendDlgItemMessage(hDlg, iCtrlID, CB_GETCURSEL, 0, 0));
	if (retention_normal == Retention)
	{
		SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
		//<S><C>To Fix Redmine Bug #2908,20201126,SSDI;Chanchal Singla
		//Pin Code check box is set to ON when Document filing is set to Hold Only.
//<S><A> Added to fix redmine bug 2164,2020.05.14 - SSDI:Goutham
		CheckDlgButton(hDlg, IDC_CHK_PIN, BST_UNCHECKED);
		m_PropertySheetState.wUsePIN = BST_UNCHECKED;
		// --- DM_USE_PIN
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wUsePIN == bool_true) ?
			(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_USE_PIN) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_USE_PIN);
		SetCheckBoxValue(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
		wcscpy(m_PropertySheetState.szRetentionPassword, L"");
		//UpdateEditBoxContent(hDlg, IDC_CHK_PIN);
//<S><E> Added to fix redmine bug 2164,2020.05.14 - SSDI:Goutham
		//<E>To Fix Redmine Bug #2908,20201126,SSDI;Chanchal Singla
//<S><A>Added for hiding document filling stored to controls ,20.03.26 SSDI:Goutham
		SetControlState(hDlg, IDC_STC_RETENTION_STOREDTO, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_RETENTION_STOREDTO, SCUI_STATE_OFF);
//<E>Added for hiding document filling stored to controls ,20.03.26 SSDI:Goutham
	}
	else if (retention_hold_after == Retention || retention_hold_before == Retention || retention_sample == Retention)
	{
		//<S><C>to Fix feedback issue #61,2021.12.15,SSDI:Chanchal Singla
		//When "Stored to" is "Quick File", PIN Code is not hidden.
		if (m_PropertySheetState.wDocFileMode == eFirstOption)//Quick Store
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
			//<S><C>to Fix Redmine #3505,20201-05-05,SSDI:Chanchal Singla
			//Job Handling Tab :Print Release checkbox and Document Filing drop down are enabled in Job Handling Tab.
			if ((m_PropertySheetState.wPPlcyRetention == bool_true) 
				|| (m_PropertySheetState.wDocFileMode == eFirstOption))//Quick Store
			{
				SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
			}
			else
			{
				SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_SHOW);
			}
			
			wcscpy(m_PropertySheetState.szRetentionPassword, L"");
			//<E>to Fix feedback issue #61,2021.12.15,SSDI:Chanchal Singla
		}
		if (m_PropertySheetState.wPPlcyRetention == bool_true)
		{
			SetControlState(hDlg, IDC_STC_RETENTION_STOREDTO, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_CMB_RETENTION_STOREDTO, SCUI_STATE_OFF);
		}
		else
		{
			SetControlState(hDlg, IDC_STC_RETENTION_STOREDTO, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_CMB_RETENTION_STOREDTO, SCUI_STATE_ON);
		}
		//<E>to Fix Redmine #3505,20201-05-05,SSDI:Chanchal Singla
//<E>Added for showing document filling stored to controls ,20.03.26 SSDI:Goutham
	}
	//<S><C>To Fix Redmine Bug #4513,2021.11.29,SSDI:Chanchal Singla
	//Doc Filing: PIN Code stores dissimilar value in different Tabs
	//SetJobControlEdit(hDlg, IDC_EDT_PIN, m_PropertySheetState.szRetentionPassword, TRUE);
			// --- DM_ALWAYSUSE_PIN
	CheckDlgButton(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
	SetCheckBoxValue(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
	SetJobControlEdit(hDlg, IDC_EDT_PIN, m_PropertySheetState.szRetentionPassword, FALSE);
	//<E>To Fix Redmine Bug #4513,2021.11.29,SSDI:Chanchal Singla
}
// To add the Show Constraints On Retention  function for hiding and showing Pin edit box ,11-02-2020,SSDI:Goutham

VOID CJobHandlingTabPropertyPage::ShowConstraintsOnFactoryDefaults(HWND hDlg, INT iCtrlID)
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
VOID CJobHandlingTabPropertyPage::ShowConstraintsOnAuthentication(HWND hDlg, INT iCtrlID)
{
	//<S><C> Implementation of restrictions with PPLCY dialog "Use Windows Loginin Name as "Login Name", Harika, 20201019
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
	{
		if ((m_PropertySheetState.wLoginName == bool_true) && (m_PropertySheetState.wLoginPass == bool_true)
			&& (m_PropertySheetState.wUseAcNum == bool_false) && (m_PropertySheetState.wLoginNameSSO == bool_false))
		{
			m_PropertySheetState.wLoginName = bool_true;
			m_PropertySheetState.wLoginPass = bool_true;
			m_PropertySheetState.wUseAcNum = bool_false;
			m_PropertySheetState.wLoginNameSSO = bool_false;
		}
		else if ((m_PropertySheetState.wLoginName == bool_false) && (m_PropertySheetState.wLoginPass == bool_false)
			&& (m_PropertySheetState.wUseAcNum == bool_false) && (m_PropertySheetState.wLoginNameSSO == bool_true))
		{
			m_PropertySheetState.wLoginName = bool_false;
			m_PropertySheetState.wLoginPass = bool_false;
			m_PropertySheetState.wUseAcNum = bool_false;
			m_PropertySheetState.wLoginNameSSO = bool_true;
		}
		else
		{
			m_PropertySheetState.wLoginName = bool_true;
			m_PropertySheetState.wLoginPass = bool_false;
			m_PropertySheetState.wUseAcNum = bool_false;
			m_PropertySheetState.wLoginNameSSO = bool_false;
		}
	}
	if (m_PropertySheetState.wPPlcyUserAuth == bool_true)
	{
		m_PropertySheetState.wLoginName = bool_false;
		m_PropertySheetState.wLoginPass = bool_false;
		m_PropertySheetState.wUseAcNum = bool_false;
		if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
		{
			m_PropertySheetState.wLoginName = bool_true;
			m_PropertySheetState.wLoginPass = bool_false;
			m_PropertySheetState.wUseAcNum = bool_false;
		}
		m_PropertySheetState.wLoginNameSSO = bool_false;
	}
	//UINT Authentication = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTHENTICATION].uChangedOptionIndex;
	UINT Authentication = SelectAuthentication(m_PropertySheetState.wLoginName, m_PropertySheetState.wLoginPass, m_PropertySheetState.wUseAcNum, m_PropertySheetState.wLoginNameSSO);
	InitializeFeatureOption(hDlg, IDC_CMB_AUTHENTICATION, Authentication);
	if (authentication_none == Authentication)
	{
		if (m_PropertySheetState.wPPlcyUserAuth == bool_true)
		{
			m_ControlState.cUserAuth_I = SCUI_STATE_ON;
			m_ControlState.cCmbUserAuth = SCUI_STATE_SHOW;
		}
		else
		{
			m_ControlState.cUserAuth_I = SCUI_STATE_OFF;
			m_ControlState.cCmbUserAuth = SCUI_STATE_ON;
		}
		SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_OFF);
		//UpdateEditBoxContent(hDlg, IDC_CMB_AUTHENTICATION);
		wcscpy(m_PropertySheetState.szLoginNameUA, L"");
		wcscpy(m_PropertySheetState.szLoginPasswordUA, L"");
		wcscpy(m_PropertySheetState.szUserNumber, L"");
	}
	else if (authentication_lname == Authentication)
	{
		if (m_PropertySheetState.wPPlcyUserAuth == bool_true) 
		{
			if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
			{
				//Coverity Fix 90984,20201124 SSDI:Goutham
				//DWORD dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
				unsigned int dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
				::GetUserName((m_PropertySheetState).szLoginNameUA, (LPDWORD)&dwSize);
				//<S><E>Coverity Fix 90984, 20201124 SSDI:Goutham
				//<S><A>to fix bug #2901 issue2, Harika 202021127
				//SetDlgItemText(hDlg, IDC_EDT_UA_LOGNAME, (m_PropertySheetState).szLoginNameUA);
				m_ControlState.cUserAuth_I = SCUI_STATE_ON;
				m_ControlState.cCmbUserAuth = SCUI_STATE_SHOW;
				SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_SHOW);
				SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_SHOW);
				SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_OFF);

				wcscpy(m_PropertySheetState.szLoginPasswordUA, L"");
				wcscpy(m_PropertySheetState.szUserNumber, L"");
				//<E>to fix bug #2901 issue2, Harika 202021127
			}
		}
		else if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
		{
			//Coverity Fix 90984,20201124 SSDI:Goutham
			//DWORD dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
			unsigned int dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
			::GetUserName((m_PropertySheetState).szLoginNameUA, (LPDWORD)&dwSize);
			//<S><E>Coverity Fix 90984,20201124 SSDI:Goutham
			//SetDlgItemText(hDlg, IDC_EDT_UA_LOGNAME, (m_PropertySheetState).szLoginNameUA);
			m_ControlState.cUserAuth_I = SCUI_STATE_ON;
			m_ControlState.cCmbUserAuth = SCUI_STATE_ON;
			SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_OFF);
			wcscpy(m_PropertySheetState.szLoginPasswordUA, L"");
			wcscpy(m_PropertySheetState.szUserNumber, L"");
		}
		else
		{
			m_ControlState.cUserAuth_I = SCUI_STATE_OFF;
			m_ControlState.cCmbUserAuth = SCUI_STATE_ON;
			SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_OFF);

			wcscpy(m_PropertySheetState.szLoginPasswordUA, L"");
			wcscpy(m_PropertySheetState.szUserNumber, L"");
		}
	}
	else if (authentication_lname_pswd == Authentication)
	{
		if ((m_PropertySheetState.wPPlcyUserAuth == bool_true) && (m_PropertySheetState.wPPlcyWinLogin == bool_true))
		{
			m_ControlState.cUserAuth_I = SCUI_STATE_ON;
			m_ControlState.cCmbUserAuth = SCUI_STATE_SHOW;
			SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_OFF);
			wcscpy(m_PropertySheetState.szUserNumber, L"");
		}
		else if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
		{
			//Coverity Fix 90984,20201124 SSDI:Goutham
			//DWORD dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
			unsigned int dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
			//<S><E>Coverity Fix 90984, 20201124 SSDI:Goutham
			::GetUserName((m_PropertySheetState).szLoginNameUA, (LPDWORD)&dwSize);
			m_ControlState.cUserAuth_I = SCUI_STATE_ON;
			m_ControlState.cCmbUserAuth = SCUI_STATE_ON;
			SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_OFF);
			wcscpy(m_PropertySheetState.szUserNumber, L"");
		}
		else
		{
			m_ControlState.cUserAuth_I = SCUI_STATE_OFF;
			m_ControlState.cCmbUserAuth = SCUI_STATE_ON;
			SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_OFF);
			wcscpy(m_PropertySheetState.szUserNumber, L"");
		}
	}
	else if (authentication_acnum == Authentication)
	{
		m_ControlState.cUserAuth_I = SCUI_STATE_OFF;
		m_ControlState.cCmbUserAuth = SCUI_STATE_ON;
		SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_ON);
		SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_ON);
		wcscpy(m_PropertySheetState.szLoginNameUA, L"");
		wcscpy(m_PropertySheetState.szLoginPasswordUA, L"");
	}
	else if (authentication_sso == Authentication)
	{
	m_ControlState.cUserAuth_I = SCUI_STATE_OFF;
	m_ControlState.cCmbUserAuth = SCUI_STATE_ON;
	SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_OFF);
	SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_OFF);
	SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_OFF);
	SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_OFF);
	SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_OFF);
	SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_OFF);
	//UpdateEditBoxContent(hDlg, IDC_CMB_AUTHENTICATION);
	wcscpy(m_PropertySheetState.szLoginNameUA, L"");
	wcscpy(m_PropertySheetState.szLoginPasswordUA, L"");
	wcscpy(m_PropertySheetState.szUserNumber, L"");
	}
	//<S><C>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	//Known Issue : Login Name is not concealed on UI when Printing Policy - Use Windows Login Name as ëLogin Nameí is checked.
	SetEditLName(hDlg, IDC_EDT_UA_LOGNAME, m_PropertySheetState.szLoginNameUA);
	//SetJobControlEdit(hDlg, IDC_EDT_UA_LOGNAME, m_PropertySheetState.szLoginNameUA);
	//<E>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	SetJobControlEdit(hDlg, IDC_EDT_UA_PASS, m_PropertySheetState.szLoginPasswordUA, TRUE);
	SetJobControlEdit(hDlg, IDC_EDT_ACNUM, m_PropertySheetState.szUserNumber, TRUE);
	SetControlState(hDlg, IDC_STC_AUTHENTICATION, m_ControlState.cCmbUserAuth);
	SetControlState(hDlg, IDC_CMB_AUTHENTICATION, m_ControlState.cCmbUserAuth);
	SetControlState(hDlg, IDC_BTN_USR_AUTH_I, m_ControlState.cUserAuth_I);
	//<E> Implementation of restrictions with PPLCY dialog "Use Windows Loginin Name as "Login Name", Harika, 20201019
}
//<S><A> Restrictions with Printing Policy Dialog, Harika, 20201014
VOID CJobHandlingTabPropertyPage::ShowConstraintsOnUserName(HWND hDlg, INT iCtrlID)
{
	if (m_PropertySheetState.wPPlcyLoginName == bool_true)
	{
		m_ControlState.cUName_I = SCUI_STATE_ON;
		m_ControlState.cEdtUserName = SCUI_STATE_SHOW;
		m_ControlState.cChkUserName = SCUI_STATE_SHOW;
		//<S><A>  to fix #3799,20210520,SSDI:Chanchal Singla
		//Pop up displayed unneccessarily when ok button is clicked on Job handling Tab.
		m_PropertySheetState.wUseUName = bool_false;
			// --- DM_ALWAYSUSE_USERID
		CheckDlgButton(hDlg, IDC_CHK_UNAME, m_PropertySheetState.wUseUName);
		SetCheckBoxValue(hDlg, IDC_CHK_UNAME, m_PropertySheetState.wUseUName);
		//<E>  to fix #3799,20210520,SSDI:Chanchal Singla
		wcscpy(m_PropertySheetState.szUserName, L"");
	}
	else
	{
		m_ControlState.cUName_I = SCUI_STATE_OFF;
		m_ControlState.cChkUserName = SCUI_STATE_ON;
		// --- DM_ALWAYSUSE_USERID
		CheckDlgButton(hDlg, IDC_CHK_UNAME, m_PropertySheetState.wUseUName);
		SetCheckBoxValue(hDlg, IDC_CHK_UNAME, m_PropertySheetState.wUseUName);
		if (m_PropertySheetState.wUseUName == TRUE)
		{
			m_ControlState.cEdtUserName = SCUI_STATE_ON;
		}
		else
		{
			m_ControlState.cEdtUserName = SCUI_STATE_SHOW;
			wcscpy(m_PropertySheetState.szUserName, L"");
		}
	}
	SetJobControlEdit(hDlg, IDC_EDT_UNAME, m_PropertySheetState.szUserName);
	SetControlState(hDlg, IDC_CHK_UNAME, m_ControlState.cChkUserName);
	SetControlState(hDlg, IDC_EDT_UNAME, m_ControlState.cEdtUserName);
	SetControlState(hDlg, IDC_BTN_UNAME_I, m_ControlState.cUName_I);
}

VOID CJobHandlingTabPropertyPage::ShowConstraintsOnJobName(HWND hDlg, INT iCtrlID)
{
	// --- DM_ALWAYSUSE_JOBID
	CheckDlgButton(hDlg, IDC_CHK_JNAME, m_PropertySheetState.wUseJName);
	SetCheckBoxValue(hDlg, IDC_CHK_JNAME, m_PropertySheetState.wUseJName);
	if (m_PropertySheetState.wUseJName == TRUE)
	{
		SetControlState(hDlg, IDC_CHK_JNAME, SCUI_STATE_ON);
		SetControlState(hDlg, IDC_EDT_JNAME, SCUI_STATE_ON);
	}
	else
	{
		SetControlState(hDlg, IDC_EDT_JNAME, SCUI_STATE_SHOW);
		wcscpy(m_PropertySheetState.szJobName, L"");
	}
	SetJobControlEdit(hDlg, IDC_EDT_JNAME, m_PropertySheetState.szJobName);
}
//<S><C>to fix Bug #4007,2021.29.07,ssdi:Chanchal Singla
//	Auto job control review / User authentication not restricted in point and print scenario
VOID CJobHandlingTabPropertyPage::ShowConstraintsOnAutoJobReview(HWND hDlg, INT iCtrlID)
{
	m_PropertySheetState.wAutoJCR = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_AUTO_JOB_CONTROL_REVIEW].uChangedOptionIndex;
	//<S><C>to fix Bug #85 feedback issue,2022.11.08, Chandrashekar V
	if (m_blNetworkPrn == TRUE) //&&
		//(m_PropertySheetState.wCmptSpool == cmpt_spool_emf))
	{
		// Auto Job Control ReviewÇ
		m_PropertySheetState.wAutoJCR = bool_false;
		m_ControlState.cChkAutoJCR = SCUI_STATE_OFF;
		m_ControlState.cAutoJCR_I = SCUI_STATE_OFF;
		//m_ControlState.cChkAutoJCR = SCUI_STATE_SHOW;
		//m_ControlState.cAutoJCR_I = SCUI_STATE_ON;
		//<E>to fix Bug #85 feedback issue,2022.11.08, Chandrashekar V
	}
	else
	{
		m_ControlState.cChkAutoJCR = SCUI_STATE_ON;
		m_ControlState.cAutoJCR_I = SCUI_STATE_OFF;
	}
	CheckDlgButton(hDlg, IDC_CHK_AUTO_JCREV, m_PropertySheetState.wAutoJCR);
	SetCheckBoxValue(hDlg, IDC_CHK_AUTO_JCREV, m_PropertySheetState.wAutoJCR);
	SetControlState(hDlg, IDC_CHK_AUTO_JCREV, m_ControlState.cChkAutoJCR);
	SetControlState(hDlg, IDC_BTN_AUTO_JCREV_I, m_ControlState.cAutoJCR_I);
}
//<E>to fix Bug #4007,2021.29.07,ssdi:Chanchal Singla
VOID CJobHandlingTabPropertyPage::ShowConstraintsOnPrnRelease(HWND hDlg, INT iCtrlID)
{
	//<S><C>To Fix Redmine #7483,31-05-2024,SSDI:Manoj S
	if (((m_PropertySheetState.wPPlcyPrintRelease == bool_true && (m_PropertySheetState.byPrintRelease == bool_false))|| (m_PropertySheetState.wPPlcyRetention == bool_true))/* && !(m_PropertySheetState.byPrintRelease)*/)
	//<E>To Fix Redmine #7483,31-05-2024,SSDI:Manoj S
	{
		//<S><A> to fix #Bug #2906,20211802,SSDI:Chanchal Singla
		//Print Release/Doc Filing not set correctly when Printing Policy options are reset to defaults.
		if ((m_PropertySheetState.wPPlcyPrintRelease == bool_true) && (m_PropertySheetState.byPrintRelease == bool_false))
		{
			m_PropertySheetState.wPrintRelease = bool_true;
			CheckDlgButton(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);
			SetCheckBoxValue(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);

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
			InitializeFeatureOption(hDlg, IDC_CMB_RETENTION_STOREDTO, m_PropertySheetState.wDocFileMode);
			
			// PIN CodeÇOFF
			m_PropertySheetState.wUsePIN = bool_false;
			// --- DM_USE_PIN
			/*m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wUsePIN == bool_true) ?
				(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_USE_PIN) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_USE_PIN);*/

			CheckDlgButton(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
			SetCheckBoxValue(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);

			// Print Release, OFF
			m_PropertySheetState.wPrintRelease = bool_false;
			// --- DM_PRINTSHARE
			m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC = (m_PropertySheetState.wPrintRelease == bool_true) ?
				(m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC | DM_PRINTSHARE) : (m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & ~DM_PRINTSHARE);
			CheckDlgButton(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);
			SetCheckBoxValue(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);

			m_PropertySheetState.wPPlcyPrintRelease = bool_false;	
		}
		//Print Release/Doc Filing not set correctly when Printing Policy options are reset to defaults.
		//<E> to fix #Bug #2906,20211802,SSDI:Chanchal Singla
		SetControlState(hDlg, IDC_CHK_PRINTRELEASE, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_BTN_RETENTION_I, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_BTN_PRINTRELEASE_I, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_RETENTION, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_RETENTION, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_RETENTION_STOREDTO, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_RETENTION_STOREDTO, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
		wcscpy(m_PropertySheetState.szRetentionPassword, L"");
		SetJobControlEdit(hDlg, IDC_EDT_PIN, m_PropertySheetState.szRetentionPassword, TRUE);
	}
	else
	{
		//<S><C>To fix SSDI bug 2847, Harika 20201014
		if (1 == m_PropertySheetState.byPrintRelease)
		{
			if (m_PropertySheetState.wPrintRelease == TRUE)
//<S><A>To fix Redmine bug #2853,20201127 SSDI:Goutham
			{
				CheckDlgButton(hDlg, IDC_CHK_PRINTRELEASE, BST_UNCHECKED);
				m_PropertySheetState.wPrintRelease = BST_UNCHECKED;
				SetCheckBoxValue(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);
			}
//<S><E>To fix Redmine bug #2853,20201127 SSDI:Goutham
			m_ControlState.cPrintRelease_I = SCUI_STATE_OFF;
			m_ControlState.cChkPrintRelease = SCUI_STATE_SHOW;
			m_ControlState.cRetention_I = SCUI_STATE_OFF;
			m_ControlState.cCmbRetention = SCUI_STATE_ON;
		}
		else
		{
			if (m_PropertySheetState.wPrintRelease == TRUE)
			{
				m_ControlState.cCmbRetention = SCUI_STATE_SHOW;
				m_ControlState.cRetention_I = SCUI_STATE_ON;
			}
			else
			{
				m_ControlState.cCmbRetention = SCUI_STATE_ON;
				m_ControlState.cRetention_I = SCUI_STATE_OFF;
			}

			UINT Retention = GetValue(hDlg, IDC_CMB_RETENTION);
			if (retention_normal != Retention)
			{
				m_PropertySheetState.wRetention = bool_true;
				m_ControlState.cPrintRelease_I = SCUI_STATE_ON;
				m_ControlState.cChkPrintRelease = SCUI_STATE_SHOW;
			}
			else
			{
				m_PropertySheetState.wRetention = bool_false;
				m_ControlState.cPrintRelease_I = SCUI_STATE_OFF;
				m_ControlState.cChkPrintRelease= SCUI_STATE_ON;
			}		
		}
		SetControlState(hDlg, IDC_STC_RETENTION, m_ControlState.cCmbRetention);
		SetControlState(hDlg, IDC_CMB_RETENTION, m_ControlState.cCmbRetention);
		SetControlState(hDlg, IDC_BTN_RETENTION_I, m_ControlState.cRetention_I);
		SetControlState(hDlg, IDC_CHK_PRINTRELEASE, m_ControlState.cChkPrintRelease);
		SetControlState(hDlg, IDC_BTN_PRINTRELEASE_I, m_ControlState.cPrintRelease_I);
		//<E>To fix SSDI bug 2847, Harika 20201014
	}
}
//<E> Restrictions with Printing Policy Dialog, Harika, 20201014
//<S><A> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
BOOL CJobHandlingTabPropertyPage::OnShowHideTab(HWND hWnd, BOOL bShow)
{
	if (bShow)
	{
		JobControlDataToPropState();
	}
	else
	{
		PropStateToJobControlData();
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
	return TRUE;
}
//<S><E> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam

//=============================================================================
// function
//        OnCbnSelchange
//
// parameters
//        hWnd         ÉEÉBÉìÉhÉEÇÃÉnÉìÉhÉã
//        hWndCtrl     ëŒè€ÉRÉìÉgÉçÅ[ÉãÉEÉBÉìÉhÉEÉnÉìÉhÉã
//        wCtrlID      ëŒè€ÉRÉìÉgÉçÅ[ÉãID
//
// return value
//        ê¨å˜ : TRUE
//        é∏îs : FALSE
//
// outline
//        WM_COMMAND - CBN_SELCHANGE
//=============================================================================
LRESULT CJobHandlingTabPropertyPage::OnCustomFolderDlg(HWND hWnd)
{

	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	CUIControl		pControl;
	BOOL bIsUIUpdated = FALSE;
	INT_PTR			DlgRet = IDCANCEL;

	OEMDEV pOemPrivateDevMode = {};
	memcpy_s(&pOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));

	// Get last Selected Password function is required for Updation of Propstate before calling Custom Folder Dialog
	//GetLastSelectedPassword();

	PROPSTATE pPropertySheetState;
	SecureZeroMemory(&pPropertySheetState, sizeof(PROPSTATE));
	memcpy_s(&pPropertySheetState, sizeof(PROPSTATE), &m_PropertySheetState, sizeof(PROPSTATE));

		CShDlgStored2	*pdlg = new CShDlgStored2(IDD_DLG_STORED_2, m_hStringResourceHandle, m_hDialogResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings,
			m_hBitmapResourceHandle, m_pOemPublicDevMode, &pOemPrivateDevMode, &pPropertySheetState, m_pPrinterName);
		if (pdlg == NULL)
		{
			slog.putLog("CJobHandlingTabPropertyPage::OnCustomFolderDlg -> NULL_POINTER(IDD_DLG_STORED_2)\n");
			return FALSE;
		}

		pdlg->SetPrinterName(m_pPrinterName);
	
		DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_STORED_2),
			hWnd, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
		if (IDOK == DlgRet)
		{

			if (FALSE == pControl.IsEqualBuf(m_pOemPrivateDevMode, &pOemPrivateDevMode, sizeof(OEMDEV)))
			{
				memcpy_s(m_pOemPrivateDevMode, sizeof(OEMDEV), &pOemPrivateDevMode, sizeof(OEMDEV));
				bIsUIUpdated = TRUE;
			}


			if (FALSE == pControl.IsEqualBuf(&m_PropertySheetState, &pPropertySheetState, sizeof(PROPSTATE)))
			{
				memcpy_s(&m_PropertySheetState, sizeof(PROPSTATE), &pPropertySheetState, sizeof(PROPSTATE));
				m_pOemPrivateDevMode->scpi.ext.JobCtrl.nDocFilingFolder = m_PropertySheetState.wDocFileFolder;
				
				bIsUIUpdated = TRUE;
			}
			//<S><A> Added to fix redmine bug #2671,2020.02.09 SSDI:Goutham
			//Modified the Condition because drop down is not changed to MainFolder if No folder is added in Custom Folder Dialog
			//if ((*pdlg).GetCount() <= 1 && m_PropertySheetState.wDocFileFolder == 0) 
			if ((*pdlg).GetCount() <= 1) 
			{
				UINT suCurrentItemId = m_DefaultSettings[FEATURE_DOCUMENT_FILING_SETTINGS].ResourceID;
				TCHAR szItem[MAX_PATH] = { 0 };
				if (LoadString(m_hStringResourceHandle, suCurrentItemId, szItem, countof(szItem)) > 0)
				{
					UINT uChangedOptionIndex = static_cast<UINT>(SendDlgItemMessage(hWnd, IDC_CMB_RETENTION_STOREDTO, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szItem));
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption = "MAINFOLDER";
					m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uChangedOptionIndex = uChangedOptionIndex;
					//m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uDefaultOptionIndex = uChangedOptionIndex;
					SendDlgItemMessage(hWnd, IDC_CMB_RETENTION_STOREDTO, CB_SETCURSEL, uChangedOptionIndex, 0);
				}
			}
				
			//<S><E> Added to fix redmine bug #2671,2020.02.09 SSDI:Goutham
			if (TRUE == bIsUIUpdated)
			{
				pControl.EnableApplyButton(hWnd);
			}
		}
		//<S><A>To fix Redmine bug #2671(issue 2),2020.09.10 SSDI:Goutham
		else
		{
			//Modified the Condition because drop down is not changed to MainFolder if No folder is added in Custom Folder Dialog
			//if ((*pdlg).GetCount() <= 1 && m_PropertySheetState.wDocFileFolder == 0) 
			//<S><C>To fix bug#2671 issue3 ,     K M Harika, 17/9/2020
			/*if ((*pdlg).GetCount() <= 1)
			{*/
			//<E>To fix bug#2671 issue3 ,     K M Harika, 17/9/2020
				UINT suCurrentItemId = m_DefaultSettings[FEATURE_DOCUMENT_FILING_SETTINGS].ResourceID;
				TCHAR szItem[MAX_PATH] = { 0 };
				if (LoadString(m_hStringResourceHandle, suCurrentItemId, szItem, countof(szItem)) > 0)
				{
					UINT uChangedOptionIndex = static_cast<UINT>(SendDlgItemMessage(hWnd, IDC_CMB_RETENTION_STOREDTO, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szItem));
					m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption = "MAINFOLDER";
					m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uChangedOptionIndex = uChangedOptionIndex;
				//	m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uDefaultOptionIndex = uChangedOptionIndex;
					SendDlgItemMessage(hWnd, IDC_CMB_RETENTION_STOREDTO, CB_SETCURSEL, uChangedOptionIndex, 0);
				}
			/*}*/
		
		}
		//<S><E>To fix Redmine bug #2671(issue 2),2020.09.10 SSDI:Goutham
		
		if (pdlg != NULL)
		{
			delete pdlg;
			pdlg = NULL;
		}
		
		return TRUE;

}
BOOL CJobHandlingTabPropertyPage ::GetLastSelectedPassword()
{
	CShRegStored	*preg = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	CShJsonStored	*pjsonstr = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonstr = new CShJsonStored(ghInstance, m_pPrinterName, m_hStringResourceHandle);
		pjsonstr->Init();
	}

	BOOL			blHasPin = FALSE;
	WCHAR szTemp[512];
	::ZeroMemory(szTemp, sizeof(szTemp));
	WCHAR	szFilingPassword[JC_STR_FOLDERPASS + 1 + 8];
	::ZeroMemory(szFilingPassword, sizeof(szFilingPassword));
	WCHAR			szFilingPath[(JC_STR_FOLDERPATH + 1) * 2];
	::ZeroMemory(szFilingPath, sizeof(szFilingPath));
	//Coverity Fix:91481 - SSDI Seetharam - 20210518
	//Memory allocated, which is not allocated and used
	preg = new CShRegStored(m_hStringResourceHandle, m_pPrinterName);
	if (preg == NULL)
	{
		return FALSE;
	}
//<S><A>To Fix Redmine bug #2741 issue1 and issue2,10-21-2020,SSDI:Goutham
	SHORT wDocFileFolder;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		wDocFileFolder = (short)(*pjsonstr).ReadLastSelectedFolder(szTemp, sizeof(szTemp), szFilingPassword, sizeof(szFilingPassword));
		if ((*pjsonstr).ReadStrdData(wDocFileFolder,
			szFilingPath, sizeof(szFilingPath), &blHasPin) == FALSE)
		{
			//Coverity Fix: 92502 - SSDI Seetharam : 20210531
			delete pjsonstr;
			pjsonstr = NULL;
			return 0;
		}
	}
	else
	{
		wDocFileFolder = (short)(*preg).ReadLastSelectedFolder(m_pPrinterName, szTemp, sizeof(szTemp), szFilingPassword, sizeof(szFilingPassword));
		if ((*preg).ReadStrdData(m_pPrinterName, wDocFileFolder,
			szFilingPath, sizeof(szFilingPath), &blHasPin) == FALSE)
		{
			//Coverity Fix: 92502 - SSDI Seetharam : 20210531
			delete preg;
			preg = NULL;
			return 0;
		}
	}

	if (blHasPin == TRUE)
		//::lstrcpyn(m_PropertySheetState.szFolderPass, szFilingPassword, JC_STR_FOLDERPASS + 1);
		//Coverity Fix 90895 - SSDI:Chanchal Singla - 20201201
		::lstrcpyn((LPWSTR)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byFolderPassword, szFilingPassword, wcslen(szFilingPassword) * sizeof(WCHAR));
//<S><A>To Fix Redmine bug #2741 issue1 and issue2,10-21-2020,SSDI:Goutham
	if(NULL != preg)
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
	return TRUE;
}
//<S><A>To fix redmine bug #2741,2020.10.29,SSDI:Goutham
void CJobHandlingTabPropertyPage::StoredToSetTabDefaults(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CUIControl pControl;
	BOOL bIsUIUpdated = FALSE;
	OEMDEV temp_pOemPrivateDevMode = {};
	PROPSTATE temp_pPropertySheetState;
	SecureZeroMemory(&temp_pPropertySheetState, sizeof(PROPSTATE));
	memcpy_s(&temp_pPropertySheetState, sizeof(PROPSTATE), &m_PropertySheetState, sizeof(PROPSTATE));

	CShDlgStored2	*pdlgStored = new CShDlgStored2(IDD_DLG_STORED_2, m_hStringResourceHandle, m_hDialogResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings,
		m_hBitmapResourceHandle, m_pOemPublicDevMode, &temp_pOemPrivateDevMode, &temp_pPropertySheetState, m_pPrinterName);
	if (NULL == pdlgStored)
	{
		slog.putLog("COtherSettingsDialog::SetTabDefaults->NULL_POINTER(CShImageCombo)\n");
	}

	pdlgStored->CreateControlClass();
	pdlgStored->SetTabDefaults(hDlg);

	if (FALSE == pControl.IsEqualBuf(&m_PropertySheetState, &temp_pPropertySheetState, sizeof(PROPSTATE)))
	{
		memcpy_s(&m_PropertySheetState, sizeof(PROPSTATE), &temp_pPropertySheetState, sizeof(PROPSTATE));
		bIsUIUpdated = TRUE;
	}

	if (TRUE == bIsUIUpdated)
	{
		pControl.EnableApplyButton(hDlg);
	}
	delete pdlgStored;
	pdlgStored = NULL;
}
//<S><E>To fix redmine bug #2741,2020.10.29,SSDI:Goutham

//=============================================================================
// function
//      ChangedAuthentication
//
// parameters
//    short wAuthen
//
// return value
//      ê¨ TRUE
//       FALSE
//
// outline
//     ChangedAuthentication
//=============================================================================
BOOL CJobHandlingTabPropertyPage::ChangedAuthentication(short wAuthen)
{
	BOOL			blRet = FALSE;
	DWORD			dwSize = 0;

	if (authentication_none == wAuthen) {
		m_PropertySheetState.wLoginName = bool_false;
		m_PropertySheetState.wLoginPass = bool_false;
		m_PropertySheetState.wUseAcNum = bool_false;
		m_PropertySheetState.wLoginNameSSO = bool_false;
	}
	else if (authentication_lname == wAuthen) {
		m_PropertySheetState.wLoginName = bool_true;
		m_PropertySheetState.wLoginPass = bool_false;
		m_PropertySheetState.wUseAcNum = bool_false;

		if (m_PropertySheetState.wPPlcyWinLogin == bool_true) {

			unsigned int dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
			::GetUserName((m_PropertySheetState).szLoginNameUA, (LPDWORD)&dwSize);
		}m_PropertySheetState.wLoginNameSSO = bool_false;

	}
	else if (authentication_lname_pswd == wAuthen) {
		m_PropertySheetState.wLoginName = bool_true;
		m_PropertySheetState.wLoginPass = bool_true;
		m_PropertySheetState.wUseAcNum = bool_false;
		if (m_PropertySheetState.wPPlcyWinLogin == bool_true) {
			unsigned int dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
			::GetUserName((m_PropertySheetState).szLoginNameUA, (LPDWORD)&dwSize);
		}
		m_PropertySheetState.wLoginNameSSO = bool_false;
	}
	else if (authentication_acnum == wAuthen) {
		m_PropertySheetState.wLoginName = bool_false;
		m_PropertySheetState.wLoginPass = bool_false;
		m_PropertySheetState.wUseAcNum = bool_true;
		m_PropertySheetState.wLoginNameSSO = bool_false;
	}
	else if (authentication_sso == wAuthen) {
		m_PropertySheetState.wLoginName = bool_false;
		m_PropertySheetState.wLoginPass = bool_false;
		m_PropertySheetState.wUseAcNum = bool_false;
		m_PropertySheetState.wLoginNameSSO = bool_true;
	}

	blRet = TRUE;

	return blRet;
}