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
//  Author        : Chandrashekar V
//
//  Date		  : 10 December 2020
//
//	Description   : Implementation of Job Handling dialog

//===========================================================================



#include "precomp.h"
#include "define.h"
#include "RegistryAPI.h"
#include "shjsonstr.h"
#include "aes.h"
#include <algorithm>
#include "UtilityFunctions.h"
//For Logging Purpose
#include "MFPLogger.h"
#define MAX_ENCODING_STRING_LENGTH 16
//=============================================================================
// function
//      CShDlgJobHand01
//
// return value
//          
//
//Parameters 
//     
//
//
// outline
//      Constructor Routine
//=============================================================================
CShDlgJobHand01::CShDlgJobHand01(UINT uContorlID, HMODULE hStringResourceHandle, HMODULE hBitmapResourceHandle, HMODULE hDialogResourceHandle,
	PFEATUREINFOLIST &pFeatureInfoList, std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings, ResourceIDInfoMap* pResourceIDInfoMap,
	HANDLE  hPrinter, HWND hWnd, WCHAR *pszSvrPrnName, PDEVMODEW pOemPublicDevMode, POEMDEV	pOemDevMode, CShowMessage* wObjCShowMessage):
	CUIControl(uContorlID, hStringResourceHandle, hPrinter, hDialogResourceHandle, hBitmapResourceHandle)
{
	m_pResourceIDInfoMap = pResourceIDInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	//m_hPropertySheetHandle = hWnd;
	m_hPropertySheetHandle = NULL;
	m_DefaultSettings = pDefaultSettings;
	m_hStringResourceHandle = hStringResourceHandle;
	m_hBitmapResourceHandle = hBitmapResourceHandle;
	m_pOemPublicDevMode = pOemPublicDevMode;
	m_pOemPrivateDevMode = pOemDevMode;
	m_hDialogResourceHandle = hDialogResourceHandle;
	memcpy_s(&m_TempJobControl, sizeof(JOBCONTROL), &m_pOemPrivateDevMode->scpi.ext.JobCtrl, sizeof(JOBCONTROL));
	m_ObjCShowMessage = wObjCShowMessage;
	m_hInstance = ghInstance;
	m_pPrinterName = pszSvrPrnName;
	m_pJobControl = NULL;
	m_pPrivateDevmode = NULL;
	m_wAuthen = 0;
	m_wSavePass = 0;
	m_wSaveLoginPass = 0;
	m_wSaveAcNumber = 0;
	m_wSaveUserName = 0;
	m_wSaveJobName = 0;
	m_wSaveFolderPass = 0;
	SecureZeroMemory(&m_PropertySheetState, sizeof(PROPSTATE));
	SecureZeroMemory(m_szDefUserName, USER_NAME_LENGTH);	// user name
	SecureZeroMemory(m_szDefJobName, JOB_NAME_LENGTH);	// job name
}


//=============================================================================
// function
//      ~CShDlgJobHand01
//
// parameters
//      ñ
//
// return value
//      ñ
//
// outline
//      CShDlgJobHand01 
//=============================================================================
CShDlgJobHand01::~CShDlgJobHand01()
{
	
}

//=============================================================================
// function
//      OnWmInitdialog
//
// return value
//          LRESULT
//
//Parameters 
//    NIL
//
//
// outline
//      Initializes the  Dialog
//=============================================================================
LRESULT CShDlgJobHand01::OnWmInitdialog(void)
{
	LRESULT result = 1;
	//SetTitle();
	JobControlToPropState();
	m_hPropertySheetHandle = m_hWnd;
	SetControlStrings(m_hWnd);
	ActivateUIControl(m_hWnd);
	CreateCtrlClass();
	InitializeUIControls(m_hWnd);
	SetTabState(m_hWnd);
	CenterWindow();
	return result;
}
//=============================================================================
// Function :
//		JobControlToPropState
// parameters
//      NILL
//
// return value
//      NILL
//
// outline
//      Update JobControl To PropState
//=============================================================================
VOID CShDlgJobHand01::JobControlToPropState()
{
	//PPPrintRelease
	m_PropertySheetState.wPPlcyPrintRelease = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_PRINTRELEASE ? bool_true : bool_false;

	//PPDocumentFiling
	m_PropertySheetState.wPPlcyRetention = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_RETENTION ? bool_true : bool_false;
	
	//PPUserAuthentication
	m_PropertySheetState.wPPlcyUserAuth = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_USERAUTH ? bool_true : bool_false;
	
	//PPWindowsLoginNameasUserName
	m_PropertySheetState.wPPlcyWinLogin = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_WINLOGIN_AS_LOGINN ? bool_true : bool_false;
	
	//PPLgoinNameasUserName
	m_PropertySheetState.wPPlcyLoginName = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_PP_LOGINN_AS_USERN ? bool_true : bool_false;

	//DM_SINGLESIGNON
	m_PropertySheetState.wSingleSignOn = m_pOemPrivateDevMode->scpi.dev.dwFieldVDM & DM_SINGLESIGNON ? bool_true : bool_false;

	//ByPrintRelease
	m_PropertySheetState.byPrintRelease = m_pOemPrivateDevMode->scpi.dev.byFieldVDM & DM_VDM_NON_PRITNRLEASE ? bool_true : bool_false;
	// --- DM_DOCFILING
	m_PropertySheetState.wRetention = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_DOCFILING ? bool_true : bool_false;
	// --- byRetention
	m_PropertySheetState.wRetentionMode = GetRetentionPropValue();

	// --- byDocumentFiling
	m_PropertySheetState.wDocFileMode = GetDocFillingPropValue();
	// --- DM_ALWAYSUSE_ACNUM
	m_PropertySheetState.wUseAcNum = m_TempJobControl.wFieldJC & DM_ALWAYSUSE_ACNUM ? bool_true : bool_false;

	// --- DM_ALWAYSUSE_USERID
	m_PropertySheetState.wUseUName = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_USERID ? bool_true : bool_false;

	// --- DM_ALWAYSUSE_JOBID
	m_PropertySheetState.wUseJName = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_JOBID ? bool_true : bool_false;

	// --- DM_USE_LOGINNAME
	m_PropertySheetState.wLoginName = m_TempJobControl.wFieldJC & DM_USE_LOGINNAME ? bool_true : bool_false;

	// --- DM_USE_LOGINPASS
	m_PropertySheetState.wLoginPass = m_TempJobControl.wFieldJC & DM_USE_LOGINPASS ? bool_true : bool_false;
	
	// --- DM_USE_SINGLESIGNON
	m_PropertySheetState.wLoginNameSSO = m_TempJobControl.wFieldJC & DM_USE_SINGLESIGNON ? bool_true : bool_false;

	// --- DM_USE_PIN
	m_PropertySheetState.wUsePIN = m_TempJobControl.wFieldJC & DM_USE_PIN ? bool_true : bool_false;

	// --- DM_PRINTSHARE
	m_PropertySheetState.wPrintRelease = m_TempJobControl.wFieldJC & DM_PRINTSHARE ? bool_true : bool_false;

	m_PropertySheetState.wDocFileFolder = m_TempJobControl.nDocFilingFolder;

	// _/_/_/  job control data
	// ================================================
	SetRegJCInfoToPropState(&m_pOemPrivateDevMode->scpi, &m_PropertySheetState, m_pPrinterName);



	//<S><A>To fix redmine #3227,2020-02-05,SSDI:Chanchal Singla
	//Login Name text box is not displayed in Job handling dialog when User Authentication is set ON
	//PPUserAuthentication
	if (m_PropertySheetState.wPPlcyUserAuth == bool_true)
	{
		// --- DM_USE_LOGINNAME
		m_PropertySheetState.wLoginName = bool_true;
		// --- DM_USE_LOGINPASS
		m_PropertySheetState.wLoginPass = bool_false;
		// ---DM_ALWAYSUSE_ACNUM
		m_PropertySheetState.wUseAcNum = bool_false;
		// --- DM_USE_SINGLESIGNON
		m_PropertySheetState.wLoginNameSSO = bool_false;
		//<S><A>To fix Bug #4015,2021-07-12,SSDI:Chanchal Singla 
		//Values which are set in Job - handling tab are displayed in User Authentication dialog
		SecureZeroMemory(m_PropertySheetState.szUserNumber, sizeof(m_PropertySheetState.szUserNumber));
		SecureZeroMemory(m_PropertySheetState.szLoginNameUA, sizeof(m_PropertySheetState.szLoginNameUA));
		SecureZeroMemory(m_PropertySheetState.szLoginPasswordUA, sizeof(m_PropertySheetState.szLoginPasswordUA));
		if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
		{
			unsigned int dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
			::GetUserName((m_PropertySheetState).szLoginNameUA, (LPDWORD)&dwSize);
		}
		//<E>To fix Bug #4015,2021-07-12,SSDI:Chanchal Singla
	}
	//<E>To fix redmine #3227,2020-02-05,SSDI:Chanchal Singla

		// --- szUserName
	if (m_PropertySheetState.wUseUName == 0)
	{
		//Coverity Fix - 91221 - SSDI:Seetharam - 20210226
		//DWORD dwUserNameSize = sizeof(m_PropertySheetState.szUserName);
		DWORD dwUserNameSize = (JC_STR_JUNAME + 1) * 2;
		GetUserName(m_PropertySheetState.szUserName, &dwUserNameSize);
	}
	/*memcpy_s(m_szDefUserName,
		(_countof(m_szDefUserName) - 1) * sizeof(WCHAR),
		m_PropertySheetState.szUserName,
		(_countof(m_PropertySheetState.szUserName) - 1) * sizeof(WCHAR));*/
	::lstrcpy(m_szDefUserName, m_PropertySheetState.szUserName);

	//if (0 == wcsnlen_s(m_TempJobControl.szUsername, USER_NAME_LENGTH))
	//{
	//	DWORD dwUserNameSize = USER_NAME_LENGTH;
	//	GetUserName(m_TempJobControl.szUsername, &dwUserNameSize);
	//}
	/*memcpy_s(m_szDefUserName,
		(_countof(m_szDefUserName) - 1) * sizeof(WCHAR),
		m_TempJobControl.szUsername,
		(_countof(m_TempJobControl.szUsername) - 1) * sizeof(WCHAR));*/

		// --- szUserName
	if (m_PropertySheetState.wUseJName == 0)
	{
		::lstrcpy(m_PropertySheetState.szJobName, m_TempJobControl.szJobname);
	}

	short wShortJobName = m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_SHORT_JOBNAME ? TRUE : FALSE;
	if (wShortJobName == 1)
	{
		JobNameShortening(m_PropertySheetState.szJobName, wcslen(m_PropertySheetState.szJobName));
	}
	::lstrcpy(m_szDefJobName, m_PropertySheetState.szJobName);

	/*memcpy_s(m_szDefJobName,
		(_countof(m_szDefJobName) - 1) * sizeof(WCHAR),
		m_TempJobControl.szJobname,
		(_countof(m_TempJobControl.szJobname) - 1) * sizeof(WCHAR));
*/
	//memcpy_s(m_PropertySheetState.szUserName,
	//	(_countof(m_PropertySheetState.szUserName) - 1) * sizeof(WCHAR),
	//	m_TempJobControl.szUsername,
	//	(_countof(m_TempJobControl.szUsername) - 1) * sizeof(WCHAR));

	/*memcpy_s(m_PropertySheetState.szJobName,
		(_countof(m_PropertySheetState.szJobName) - 1) * sizeof(WCHAR),
		m_TempJobControl.szJobname,
		(_countof(m_PropertySheetState.szJobName) - 1) * sizeof(WCHAR)
	);*/
	
	//if (m_PropertySheetState.wPPlcyWinLogin == bool_true)
	//{	
	//	unsigned int dwSize = sizeof((m_PropertySheetState).szLoginNameUA);
	//	::GetUserName((m_PropertySheetState).szLoginNameUA, (LPDWORD)&dwSize);	
	//}
	//else
	//{
	//	memcpy_s(m_PropertySheetState.szLoginNameUA,
	//		(_countof(m_PropertySheetState.szLoginNameUA) - 1) * sizeof(WCHAR),
	//		m_TempJobControl.szLoginName,
	//		(_countof(m_PropertySheetState.szLoginNameUA) - 1) * sizeof(WCHAR)
	//	);
	//}



	//WCHAR wszUserNumber[32];
	//GetDecodedPassWordData((CHAR*)m_TempJobControl.byUserNumber,
	//	wszUserNumber,
	//	sizeof(m_TempJobControl.byUserNumber)
	//);
	//wcscpy_s(m_PropertySheetState.szUserNumber, JC_STR_PINLENGTH + 1, wszUserNumber);


	//WCHAR wszRetentionPassword[32];
	//GetDecodedPassWordData((CHAR*)m_TempJobControl.byRetentionPassword,
	//	wszRetentionPassword,
	//	sizeof(m_TempJobControl.byRetentionPassword)
	//);
	//wcscpy_s(m_PropertySheetState.szRetentionPassword, JC_STR_PINLENGTH + 10, wszRetentionPassword);


	//WCHAR wszLoginPasswordUA[32];
	//GetDecodedPassWordData((CHAR*)m_TempJobControl.byLoginPassword,
	//	wszLoginPasswordUA,
	//	sizeof(m_TempJobControl.byLoginPassword)
	//);
	//wcscpy_s(m_PropertySheetState.szLoginPasswordUA, JC_STR_LOGINPASS + 1, wszLoginPasswordUA);
	//
	//GetLastSelectedPassword();
	/*::lstrcpyn(m_PropertySheetState.szFolderPass, (LPWSTR)m_TempJobControl.byFolderPassword, JC_STR_FOLDERPASS + 1);

	m_PropertySheetState.wDocFileFolder = m_TempJobControl.nDocFilingFolder;*/


}
//=============================================================================
// Function :
//		PropStateToJobControl
// parameters
//      NILL
//
// return value
//      NILL
//
// outline
//      Update PropState To JobControl
//=============================================================================
VOID CShDlgJobHand01::PropStateToJobControl()
{

	::lstrcpyn((LPWSTR)m_TempJobControl.byFolderPassword, m_PropertySheetState.szFolderPass, wcslen(m_PropertySheetState.szFolderPass) * sizeof(WCHAR));
	::lstrcpyn((LPWSTR)m_TempJobControl.szFolderPass, m_PropertySheetState.szFolderPass, wcslen(m_PropertySheetState.szFolderPass) * sizeof(WCHAR));

	memcpy_s(m_TempJobControl.szJobname, _countof(m_TempJobControl.szJobname) * sizeof(WCHAR),
		m_PropertySheetState.szJobName, _countof(m_TempJobControl.szJobname) * sizeof(WCHAR));

	memcpy_s(m_TempJobControl.szLoginName, _countof(m_TempJobControl.szLoginName) * sizeof(WCHAR),
		m_PropertySheetState.szLoginNameUA, _countof(m_TempJobControl.szLoginName) * sizeof(WCHAR));
	memcpy_s(m_TempJobControl.szUsername, _countof(m_TempJobControl.szUsername) * sizeof(WCHAR),
		m_PropertySheetState.szUserName, _countof(m_TempJobControl.szUsername) * sizeof(WCHAR));

	GetEncodedPassWordData((CHAR*)m_TempJobControl.byUserNumber,
		m_PropertySheetState.szUserNumber,
		JC_STR_PINLENGTH,
		USER_NUMBER_LENGTH
	);
	GetEncodedPassWordData((CHAR*)m_TempJobControl.byRetentionPassword,
		m_PropertySheetState.szRetentionPassword,
		JC_STR_PINLENGTH,
		RETENTION_PASSWORD_LENGTH
	);

	GetEncodedPassWordData((CHAR*)m_TempJobControl.byLoginPassword,
		m_PropertySheetState.szLoginPasswordUA,
		JC_STR_LOGINPASS,
		LOGIN_PASSWORD_LENGTH
	);
	if (m_PropertySheetState.wLoginNameSSO == bool_true) {
		memcpy_s(m_TempJobControl.szLoginName, _countof(m_TempJobControl.szLoginName) * sizeof(WCHAR),
			m_PropertySheetState.szSingleSignOnName, _countof(m_TempJobControl.szLoginName) * sizeof(WCHAR));
	}
	// --- byRetention
	m_TempJobControl.byRetention = GetRetentionJobControlValue();

	// --- byDocumentFiling
	m_TempJobControl.byDocumentFiling  = GetDocFillingJobControlValue();
	//<S><C>Fixed SOL2 SQA Bug  #46675: assigned the folder name to szFilingPath of JOBCONTROL, Harika, 20220908
	if(m_TempJobControl.byDocumentFiling == docfile_custom)
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
		::SecureZeroMemory(szTemp, sizeof(szTemp));
		WCHAR	szFilingPassword[JC_STR_FOLDERPASS + 1 + 8];
		::SecureZeroMemory(szFilingPassword, sizeof(szFilingPassword));
		WCHAR			szFilingPath[(JC_STR_FOLDERPATH + 1) * 2];
		::SecureZeroMemory(szFilingPath, sizeof(szFilingPath));
		preg = new CShRegStored(m_hStringResourceHandle, m_pPrinterName);
		if (preg == NULL)
		{
			return;
		}
		SHORT wDocFileFolder;
		if ((*m_pmcf).IsWriteToJson() == TRUE)
		{
			wDocFileFolder = (short)(*pjsonstr).ReadLastSelectedFolder(szTemp, sizeof(szTemp), szFilingPassword, sizeof(szFilingPassword));
			if ((*pjsonstr).ReadStrdData((SHORT)wDocFileFolder,
				szFilingPath, sizeof(szFilingPath), &blHasPin) == FALSE)
			{
				//Fixed Coverity issue - 91508#2 - SSDI:Seetharam-20201005
				delete pjsonstr;
				pjsonstr = NULL;
				return;
			}
		}
		else
		{
			wDocFileFolder = (short)(*preg).ReadLastSelectedFolder(m_pPrinterName, szTemp, sizeof(szTemp), szFilingPassword, sizeof(szFilingPassword));
			if ((*preg).ReadStrdData(m_pPrinterName, (SHORT)wDocFileFolder,
				szFilingPath, sizeof(szFilingPath), &blHasPin) == FALSE)
			{
				//Fixed Coverity issue - 91508#2 - SSDI:Seetharam-20201005
				delete preg;
				preg = NULL;
				return;
			}
		}
		memcpy_s(m_TempJobControl.szFilingPath,
			(_countof(szFilingPath) - 1) * sizeof(WCHAR),
			szFilingPath,
			(_countof(szFilingPath) - 1) * sizeof(WCHAR)
		);
		//Fixed Coverity issue - 92761, Harika- 20230321
		if (preg != NULL)
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
	}
	//<E>Fixed SOL2 SQA Bug  #46675: assigned the folder name to szFilingPath of JOBCONTROL, Harika, 20220908
	// -- nDocFilingFolder
	m_TempJobControl.nDocFilingFolder = m_PropertySheetState.wDocFileFolder;

	// --- DM_DOCFILING
	m_TempJobControl.wFieldJC = (m_PropertySheetState.wRetention == bool_true) ?
		(m_TempJobControl.wFieldJC | DM_DOCFILING) : (m_TempJobControl.wFieldJC & ~DM_DOCFILING);
	// --- DM_ALWAYSUSE_ACNUM
	m_TempJobControl.wFieldJC = (m_PropertySheetState.wUseAcNum == bool_true) ?
		(m_TempJobControl.wFieldJC | DM_ALWAYSUSE_ACNUM) : (m_TempJobControl.wFieldJC & ~DM_ALWAYSUSE_ACNUM);

	// --- DM_USE_LOGINNAME
	m_TempJobControl.wFieldJC = (m_PropertySheetState.wLoginName == bool_true) ?
		(m_TempJobControl.wFieldJC | DM_USE_LOGINNAME) : (m_TempJobControl.wFieldJC & ~DM_USE_LOGINNAME);

	// --- DM_USE_LOGINPASS
	m_TempJobControl.wFieldJC = (m_PropertySheetState.wLoginPass == bool_true) ?
		(m_TempJobControl.wFieldJC | DM_USE_LOGINPASS) : (m_TempJobControl.wFieldJC & ~DM_USE_LOGINPASS);

	// --- DM_USE_SINGLESIGNON
	m_TempJobControl.wFieldJC = (m_PropertySheetState.wLoginNameSSO == bool_true) ?
		(m_TempJobControl.wFieldJC | DM_USE_SINGLESIGNON) : (m_TempJobControl.wFieldJC & ~DM_USE_SINGLESIGNON);

	// --- DM_USE_PIN
	m_TempJobControl.wFieldJC = (m_PropertySheetState.wUsePIN == bool_true) ?
		(m_TempJobControl.wFieldJC | DM_USE_PIN) : (m_TempJobControl.wFieldJC & ~DM_USE_PIN);

	// --- DM_PRINTSHARE
	m_TempJobControl.wFieldJC = (m_PropertySheetState.wPrintRelease == bool_true) ?
		(m_TempJobControl.wFieldJC | DM_PRINTSHARE) : (m_TempJobControl.wFieldJC & ~DM_PRINTSHARE);

	m_TempJobControl.wFieldJC |= DM_ALWAYSUSE_JOBID;
	m_TempJobControl.wFieldJC |= DM_ALWAYSUSE_USERID;

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
SHORT CShDlgJobHand01::GetDocFillingPropValue()
{
	SHORT wDocFileMode = eFirstOption;
	switch (m_TempJobControl.byDocumentFiling)
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
SHORT CShDlgJobHand01::GetRetentionPropValue()
{
	SHORT wRetentionMode = eFirstOption;
//<S><C>To Fix Redmine issue #6210,2023.05.12,SSDI:Chandrashekar V
	//switch (m_DefaultSettings[FEATURE_RETENTION_SETTINGS].iDevmodeValue)//switch (m_TempJobControl.byRetention) 	//<S><C>Fix for SOL2 SQA issue 3106. Harika, 20230427
	switch (m_TempJobControl.byRetention)
//<E>To Fix Redmine issue #6210,2023.05.12,SSDI:Chandrashekar V
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
//      GetDocFillingJobControlValue
//
// return value
//     SHORT
//
// outline
//      Get DocFilling Value. 
//=============================================================================
SHORT CShDlgJobHand01::GetDocFillingJobControlValue()
{
	SHORT wDocFileMode = docfile_quick;
	switch (m_PropertySheetState.wDocFileMode)
	{
	case eFirstOption :
		wDocFileMode = docfile_quick;
		break;
	case eSecondOption :
		wDocFileMode = docfile_main;
		break;
	case eThirdOption :
		wDocFileMode = docfile_custom;
		break;
	default:
		break;
	}
	return wDocFileMode;
}

//=============================================================================
// function
//      GetRetentionJobControlValue
//
// return value
//     SHORT
//
// outline
//      Get Retention Value. 
//=============================================================================
SHORT CShDlgJobHand01::GetRetentionJobControlValue()
{
	SHORT wRetentionMode = retention_normal;
	switch (m_PropertySheetState.wRetentionMode)
	{
	case eFirstOption :
		wRetentionMode = retention_normal;
		break;
	case eSecondOption :
		wRetentionMode = retention_hold_before;
		break;
	case eThirdOption :
		wRetentionMode = retention_hold_after;
		break;
	case eFourthOption :
		wRetentionMode = retention_sample;
		break;
	default:
		break;
	}
	return wRetentionMode;
}
//=============================================================================
// Function :
//		ActivateUIControl
// parameters
//      HWND      Handle to Window Dialog
//		iCtrlID	  Control ID
//
// return value
//      HRESULT
//
// outline
//      ActivateUIControl
//=============================================================================
HRESULT
CShDlgJobHand01::ActivateUIControl(
	_In_ CONST HWND hDlg
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
			slog.putLog("CShDlgJobHand01::ActivateUIControl -> NULL_POINTER(pControl)\n");
		}
	}
	return hr;
}
LRESULT CShDlgJobHand01::OnWmDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	short				wCtrlID = (short)wParam;
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;
	//m_hPropertySheetHandle = hWnd;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

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
					((CShImageCombo *)pControl)->OnWmDrawItem(hWnd, wParam, lParam);
				}
				
			}

		}
	}
	UIControlMap::const_iterator Iterator = m_UIControls.find(wCtrlID);
	switch (wCtrlID)
	{
	case	IDC_BTN_PRINTRELEASE_I:
	case 	IDC_BTN_RETENTION_I:
	case	IDC_BTN_USR_AUTH_I:
	case	IDC_BTN_UNAME_I:
	{
		if (Iterator != m_UIControls.end())
		{
			CInformationIconButton *pIIcon = static_cast<CInformationIconButton*>(Iterator->second);
			if (pIIcon)
			{
				//<S><C>to Fix Redmine Bug #2240 Issue 2,03.02.2022,SSDI:Chanchal Singla
				//Issue 2: Information icon should be highlighted when pressing Tab button
				//pIIcon->Draw();
				pIIcon->Draw((DRAWITEMSTRUCT *)lParam);
				//<E>to Fix Redmine Bug #2240 Issue 2,03.02.2022,SSDI:Chanchal Singla	
			}
			else
			{
				slog.putLog("CInformationIconButton::OnWmDrawItem -> NULL_OBJECT(CInformationIconButton)\n");
			}
		}
	}
	break;
	default:
	 break;
	}
	return CUIControl::OnWmDrawItem(hWnd, wParam, lParam);
}

HRESULT	CShDlgJobHand01::SendCommand(
	_In_ CONST HWND   hDlg,
	_In_ CONST WPARAM wParam
)

{
	//__super::SendCommand(hDlg, wParam);

	return S_OK;
}

//=============================================================================
// function
//      InitializeUIControls
//
// return value
//          NIL
//
//Parameters 
//     hDlg : handle
//
//
// outline
//      Initializes the  Controls
//=============================================================================
HRESULT
CShDlgJobHand01::InitializeUIControls(
	_In_ CONST HWND   hDlg
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	UIControlMap* pComponents = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();

	while (iterUIComponents != pComponents->end())
	{
		CUIControl* pControl = iterUIComponents->second;
		if (NULL != pControl)
		{
			if (FAILED(hr = pControl->OnInit((INT)iterUIComponents->first, hDlg)))
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
			slog.putLog("CUIControl::InitializeUIControls -> NULL_OBJECT(pControl)\n");
		}
		iterUIComponents++;
	}
	return hr;
}
//=============================================================================
// function
//      GetResourceIDInfoOfControl
//
// return value
//          NIL
//
//Parameters 
//     strFeatureName
//
//
// outline
//      Gets the ResourceId of Control
//=============================================================================

PRESOURCEIDINFO CShDlgJobHand01::GetResourceIDInfoOfControl(std::string strFeatureName)
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
//      CreateCtrlClass
//
// return value
//          NIL
//
//Parameters 
//     NIL
//
//
// outline
//      Creates the controls
//=============================================================================
BOOL CShDlgJobHand01::CreateCtrlClass(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT		hr = E_FAIL;
	BOOL		result = FALSE;
	CUIControl* pControl;
	std::string strFeatureName;
	PRESOURCEIDINFO pResourceIDInfo = NULL;

	//Defaults
	pControl = new CButton(IDC_BTN_SHARED_SUB_DEFAULT, m_hStringResourceHandle);
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
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_BTN_SHARED_SUB_DEFAULT)\n");
	}

	pControl = new CCheckBox(
		GetResourceIDInfoOfControl(ALWAYS_USE_UNAME),
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ALWAYS_USE_UNAME],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ALWAYS_USE_UNAME],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_ALWAYS_USE_UNAME]
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ALWAYS_UNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_CHK_ALWAYS_UNAME)\n");
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
	
	pControl = new CCheckBox(
		GetResourceIDInfoOfControl(ALWAYS_USE_JNAME),
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ALWAYS_USE_JNAME],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ALWAYS_USE_JNAME],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_ALWAYS_USE_JNAME]
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ALWAYS_JNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_CHK_ALWAYS_JNAME)\n");
	}

	//Print Release
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
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_CHK_PRINTRELEASE)\n");
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
		slog.putLog("CFinishingTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}
	//Document Filing
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
		m_PropertySheetState.wRetentionMode
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
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(CCombo)\n");
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

	BOOL wJCHandling = TRUE;
	//User Authentication
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
		m_PropertySheetState, wJCHandling
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_AUTHENTICATION, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
		
	}
	else
	{
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_CMB_AUTHENTICATION)\n");
	}

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
		IDC_CMB_RETENTION_STOREDTO,
		m_PropertySheetState.wDocFileMode
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
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(CCombo)\n");
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
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
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
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_EDT_PIN)\n");
	}
	//<S><C>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	//Known Issue : Login Name is not concealed on UI when Printing Policy - Use Windows Login Name as ëLogin Nameí is checked.
	pControl = new CShEditLName(m_hStringResourceHandle, m_hPrinter, m_hPropertySheetHandle, m_pOemPublicDevMode, m_pOemPrivateDevMode,
		m_PropertySheetState.szLoginNameUA, JC_STR_LOGINNAME, IDC_EDT_UA_LOGNAME);
	//pControl = new CJobControlEdit(m_PropertySheetState.szLoginNameUA, JC_STR_LOGINNAME);
	//<E>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_UA_LOGNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_EDT_UA_LOGNAME)\n");
	}

	pControl = new CJobControlEdit(m_PropertySheetState.szLoginPasswordUA, JC_STR_LOGINPASS, ePasswordEditBox);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_UA_PASS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_EDT_UA_PASS)\n");
	}

	pControl = new CJobControlEdit(m_PropertySheetState.szUserNumber, JC_STR_PINLENGTH, eNumberPasswordEditBox);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_ACNUM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_EDT_ACNUM)\n");
	}
		

	pControl = new CJobControlEdit(m_PropertySheetState.szUserName, JC_STR_JUNAME);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_UNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_EDT_UNAME)\n");
	}
	//<S><C>To Implement Task#3137,25-07-2024,SSDI:Manoj S
	//pControl = new CJobControlEdit(m_PropertySheetState.szJobName, JC_STR_JOBNAME);
	pControl = new CShEditJName(m_hStringResourceHandle, m_hPrinter, m_hPropertySheetHandle, m_pOemPublicDevMode, m_pOemPrivateDevMode,
		m_PropertySheetState.szJobName, (JC_STR_JOBNAME + 1) * 2, IDC_EDT_JNAME);
	//<E>To Implement Task #3137,25-07-2024,SSDI:Manoj S
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_JNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::CreateContorlClass -> NULL_POINTER(IDC_EDT_JNAME)\n");
	}

	   
	result = (hr == S_OK) ? TRUE : FALSE;
	return result;
}

//=============================================================================
// function
//      SetControlStrings
//
// return value
//          NIL
//
//Parameters 
//     hDlg : Handle to dialog
//
//
// outline
//      Sets all control State
//=============================================================================
VOID CShDlgJobHand01::SetControlStrings(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;

	SetTitle(IDS_TAB_05_JOBHANDLING);
	

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
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}
	//Doc-filing
	pControl = new CStaticText(IDS_IDC_STC_RETENTION, m_hStringResourceHandle);
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
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
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
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDC_STC_RETENTION_STOREDTO)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_AUTHENTICATION, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDS_IDC_STC_AUTHENTICATION, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_POINTER(IDS_IDC_STC_AUTHENTICATION)\n");
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
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDC_STC_UA_LOGNAME)\n");
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
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDC_STC_UA_PASS)\n");
	}

	pControl = new CStaticText(IDC_CHK_UA_ALWAYS_PASS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_UA_ALWAYS_PASS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingDialog::SetControlStrings -> NULL_POINTER(IDC_CHK_UA_ALWAYS_PASS)");
	}
	pControl = new CStaticText(IDC_CHK_UA_ALWAYS_PASS2, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_UA_ALWAYS_PASS2, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingDialog::SetControlStrings -> NULL_POINTER(IDC_CHK_UA_ALWAYS_PASS)");
	}
	
	pControl = new CStaticText(IDS_IDC_STC_USERNAME, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_USERNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDC_STC_USERNAME)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_JOBNAME, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_JOBNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDC_STC_JOBNAME)\n");
	}

	pControl = new CStaticText(IDS_IDC_CHK_ALWAYS_UNAME, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ALWAYS_UNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDC_CHK_ALWAYS_UNAME)\n");
	}

	
	pControl = new CStaticText(IDS_IDC_CHK_ALWAYS_JNAME, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ALWAYS_JNAME, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDC_CHK_ALWAYS_JNAME)\n");
	}
	
	//ShowWindow(GetDlgItem(hDlg, IDS_IDC_BTN_SHARED_US_DEL), SW_HIDE);

	pControl = new CStaticText(IDC_CHK_ALWAYS_PIN, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ALWAYS_PIN, pControl);
		if (S_OK != hr)
			slog.LoggingErrorHResultValues();
	}
	else
	{
		slog.putLog("CJobHandlingDialog::SetControlStrings -> NULL_POINTER(IDC_CHK_ALWAYS_PIN)");
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

	pControl = new CStaticText(IDC_CHK_ALWAYS_ACNUM, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ALWAYS_ACNUM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CJobHandlingDialog::SetControlStrings -> NULL_POINTER(IDC_CHK_ALWAYS_ACNUM)");
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
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDC_BTN_SHARED_DEFAULT)\n");
	}

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
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDOK)\n");
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
		slog.putLog("CShDlgJobHand01::SetControlStrings -> NULL_OBJECT(IDCANCEL)\n");
	}

	
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
void CShDlgJobHand01::SetTabDefaults(HWND hWnd)
{

//<S><C>To Fix Redmine issue #6210,2023.05.12,SSDI:Chandrashekar V
	/*m_TempJobControl.wFieldJC = 0;
	m_TempJobControl.byDocumentFiling = docfile_main;
	m_TempJobControl.byRetention = retention_normal;*/

	m_TempJobControl.byDocumentFiling = m_DefaultSettings[FEATURE_DOCUMENT_FILING_SETTINGS].iDevmodeValue;
	m_TempJobControl.byRetention = m_DefaultSettings[FEATURE_RETENTION_SETTINGS].iDevmodeValue;
//<E>To Fix Redmine issue #6210,2023.05.12,SSDI:Chandrashekar V
	// --- byRetention
	m_PropertySheetState.wRetentionMode = GetRetentionPropValue();

	// --- byDocumentFiling
	m_PropertySheetState.wDocFileMode = GetDocFillingPropValue();

	if (m_PropertySheetState.wPPlcyRetention == bool_false)
	{
		m_PropertySheetState.wRetention = bool_false;
	}
	//<S><C>Fix for SOL2 SQA issue 3106. Harika, 20230427
	if(m_DefaultSettings[FEATURE_PRINTRELEASE].iDevmodeValue == bool_true)
		m_PropertySheetState.wPrintRelease = bool_true;
	else
		m_PropertySheetState.wPrintRelease = bool_false;
	//<E>Fix for SOL2 SQA issue 3106. Harika, 20230427
	m_PropertySheetState.wUsePIN = bool_false;
	m_PropertySheetState.wUseAcNum = bool_false;
	m_PropertySheetState.wLoginPass = bool_false;
	m_PropertySheetState.wLoginNameSSO = bool_false;
	if (m_PropertySheetState.wPPlcyWinLogin == bool_false)
	{
		
		if ((m_PropertySheetState.wPPlcyLoginName != bool_false)
			|| (m_PropertySheetState.wPPlcyUserAuth != bool_false))
		{
				m_PropertySheetState.wLoginName = 1;
		}
		else if (m_PropertySheetState.wPPlcyUserNumber != bool_false) {
			m_PropertySheetState.wLoginName = 0;
			m_PropertySheetState.wUseAcNum = 1;
			m_PropertySheetState.wLoginPass = 0;
			m_PropertySheetState.wLoginNameSSO = 0;
		}
		else
		{
			m_PropertySheetState.wLoginName = 0;
		}

		SecureZeroMemory(m_PropertySheetState.szLoginNameUA, sizeof(m_PropertySheetState.szLoginNameUA));
	}
	else {
			m_PropertySheetState.wLoginName = 1;
	}
	if (m_PropertySheetState.wLoginName == 1) {
		//DWORD dwSize = sizeof(m_PropertySheetState.szLoginNameUA);
		DWORD dwSize = sizeof(m_PropertySheetState.szLoginNameUA) / sizeof(WCHAR);	//Coverity Fix - 91176
		GetUserName(m_PropertySheetState.szLoginNameUA, &dwSize);
	}
	if (m_PropertySheetState.wPPlcyWinLogin == bool_false)
		SecureZeroMemory(m_PropertySheetState.szLoginNameUA, sizeof(m_PropertySheetState.szLoginNameUA));

	ClearEditBoxContent(IDC_EDT_PIN, hWnd, m_PropertySheetState.szRetentionPassword, sizeof(m_PropertySheetState.szRetentionPassword));
	//ClearEditBoxContent(IDC_EDT_UA_LOGNAME, hWnd, m_PropertySheetState.szLoginNameUA, sizeof(m_PropertySheetState.szLoginNameUA));
	ClearEditBoxContent(IDC_EDT_UA_PASS, hWnd, m_PropertySheetState.szLoginPasswordUA, sizeof(m_PropertySheetState.szLoginPasswordUA));
	ClearEditBoxContent(IDC_EDT_ACNUM, hWnd, m_PropertySheetState.szUserNumber, sizeof(m_PropertySheetState.szUserNumber));


	UIControlMap::const_iterator iterUIComponents;
	iterUIComponents = m_UIControls.find(IDC_EDT_UA_LOGNAME);
	if (m_UIControls.end() != iterUIComponents)
	{
		CJobControlEdit *pJobControlEdit = static_cast<CJobControlEdit*>(iterUIComponents->second);
		pJobControlEdit->ActivateUIControl(IDC_EDT_UA_LOGNAME, hWnd);
	}

	//UIControlMap::const_iterator iterUIComponents;
	iterUIComponents = m_UIControls.find(IDC_EDT_JNAME);
	if (m_UIControls.end() != iterUIComponents)
	{
		memcpy_s(m_PropertySheetState.szJobName,
			(_countof(m_PropertySheetState.szJobName) - 1) * sizeof(WCHAR),
			m_szDefJobName,
			(_countof(m_szDefJobName) - 1) * sizeof(WCHAR));
		CJobControlEdit *pJobControlEdit = static_cast<CJobControlEdit*>(iterUIComponents->second);
		pJobControlEdit->ActivateUIControl(IDC_EDT_JNAME, hWnd);
	}
	iterUIComponents = m_UIControls.find(IDC_EDT_UNAME);
	if (m_UIControls.end() != iterUIComponents)
	{
		memcpy_s(m_PropertySheetState.szUserName,
			(_countof(m_PropertySheetState.szUserName) - 1) * sizeof(WCHAR),
			m_szDefUserName,
			(_countof(m_szDefUserName) - 1) * sizeof(WCHAR));
		CJobControlEdit *pJobControlEdit = static_cast<CJobControlEdit*>(iterUIComponents->second);
		pJobControlEdit->ActivateUIControl(IDC_EDT_UNAME, hWnd);
	}

	m_wAuthen = SelectAuthentication(m_PropertySheetState.wLoginName, m_PropertySheetState.wLoginPass, m_PropertySheetState.wUseAcNum, m_PropertySheetState.wLoginNameSSO);

	//Always Use This ...
	CheckDlgButton(hWnd,IDC_CHK_ALWAYS_PIN, BST_UNCHECKED);
	CheckDlgButton(hWnd, IDC_CHK_ALWAYS_ACNUM, BST_UNCHECKED);
	CheckDlgButton(hWnd, IDC_CHK_UA_ALWAYS_PASS, BST_UNCHECKED);
	CheckDlgButton(hWnd, IDC_CHK_UA_ALWAYS_PASS2, BST_UNCHECKED);
	CheckDlgButton(hWnd, IDC_CHK_ACNUM, BST_UNCHECKED);
	CheckDlgButton(hWnd, IDC_CHK_ALWAYS_UNAME, BST_UNCHECKED);
	CheckDlgButton(hWnd, IDC_CHK_ALWAYS_JNAME, BST_UNCHECKED);
	
}

//=============================================================================
// function
//      SetTitle
// parameters:
//       wStrID : string id
//
// return value
//     Void
//
// outline
//      Sets Tab Title
//=============================================================================
void CShDlgJobHand01::SetTitle(short wStrID)
{
	TCHAR szItem[MAX_PATH] = { 0 };
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (LoadString(m_hStringResourceHandle, wStrID, szItem, countof(szItem)) > 0)
	{
		SetWindowText(m_hWnd, szItem);
	}
	else
	{
		slog.putLog("CShDlgJobHand01::SetTitle -> ERROR_LOAD_STRING\n");
	}
	//return TRUE;
}

//=============================================================================
// function
//      CenterWindow
//
// return value
//     BOOL
//
// outline
//    Moves the Window to be displayed at Center of the 
//	  Screen	
//=============================================================================

BOOL CShDlgJobHand01::CenterWindow(HWND hWndParent, BOOL blRepaint)
{
	BOOL		blRet = FALSE;

	HWND		hWndP;

	RECT		rcWindow;
	RECT		rcParent;

	long		lPosX, lPosY;
	long		lWidth, lHeight;

	::ZeroMemory(&rcWindow, sizeof(rcWindow));
	::ZeroMemory(&rcParent, sizeof(rcParent));

	if (hWndParent != NULL)
		hWndP = hWndParent;
	else
		hWndP = ::GetDesktopWindow();

	::GetWindowRect(m_hWnd, &rcWindow);
	::GetWindowRect(hWndP, &rcParent);

	lPosX = ((rcParent.right - rcParent.left) - (rcWindow.right - rcWindow.left)) / 2;
	lPosX += rcParent.left;
	lPosY = ((rcParent.bottom - rcParent.top) - (rcWindow.bottom - rcWindow.top)) / 2;
	lPosY += rcParent.top;
	lWidth = rcWindow.right - rcWindow.left;
	lHeight = rcWindow.bottom - rcWindow.top;

	::MoveWindow(m_hWnd, lPosX, lPosY, lWidth, lHeight, blRepaint);

	blRet = TRUE;

	return blRet;
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
LRESULT CShDlgJobHand01::OnWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
	case CBN_SELCHANGE:
	{
		OnCbnSelchange(hWnd, hWndCtrl, wCtrlId);
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
//Routine Name:
//
//	CShPrintPosition::OnCbnSelchange
//
//Routine Description:
//
//	Routine for Changing the Values in the Combo Box
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
LRESULT CShDlgJobHand01::OnCbnSelchange(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;
	short			wFlag;
	switch (wCtrlID)
	{
	case IDC_CMB_RETENTION:
		m_PropertySheetState.wRetentionMode = (SHORT)GetValue(hDlg, wCtrlID);
		if (m_PropertySheetState.wRetentionMode == retention_normal) {
			m_PropertySheetState.wRetention = bool_false;
		}
		else {
			m_PropertySheetState.wRetention = bool_true;
		}

		//<S><A>to fix Bug #4062,2021012,SSDI:Chanchal Singla
		//Document Filing is not set to 'Hold After Print' when Quick file is set.
		if (m_PropertySheetState.wDocFileMode == eFirstOption)//QuickFile
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption = "MAINFOLDER";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uChangedOptionIndex = eSecondOption;
			m_PropertySheetState.wDocFileMode = eSecondOption;
		}
		//<E>to fix #4062,2021012,SSDI:Chanchal Singla
		break;

	case IDC_CMB_RETENTION_STOREDTO:
		m_PropertySheetState.wDocFileMode = (SHORT)GetValue(hDlg, wCtrlID);
		//<S><A>to fix Bug #4062,2021012,SSDI:Chanchal Singla
		//Document Filing is not set to 'Hold After Print' when Quick file is set.
		if (m_PropertySheetState.wDocFileMode == eFirstOption)//QuickFile
		{
			m_pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption = "HOLDAFTERPRINT";
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS].uChangedOptionIndex = eThirdOption;
			m_PropertySheetState.wRetentionMode = eThirdOption;
		}
		//<E>to fix #4062,2021012,SSDI:Chanchal Singla
		if (m_PropertySheetState.wDocFileMode == docfile_custom) {
			ShowCustomFolderDlg(hDlg);
		}
		break;
	case IDC_CMB_AUTHENTICATION:
		m_wAuthen = (SHORT)GetValue(hDlg, wCtrlID);
		ChangedAuthentication(m_wAuthen);
		break;
	default:
		break;
	}
	{
		UIControlMap::const_iterator Iterator = m_UIControls.find(wCtrlID);
		if (Iterator != m_UIControls.end())
		{
			CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
			if (pImageCombo)
			{
				pImageCombo->OnSelChange(wCtrlID, hDlg);
			}
		}
	}
	SetTabState(hDlg, wCtrlID);
	return res;
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
LRESULT CShDlgJobHand01::OnBnClicked(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	LRESULT			res = 0;

	BOOL bIsUIUpdated = FALSE;

	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	switch (wCtrlID)
	{
	case IDC_CHK_PRINTRELEASE:
		m_PropertySheetState.wPrintRelease = IsDlgButtonChecked(hWnd,IDC_CHK_PRINTRELEASE);
		if (m_PropertySheetState.wPrintRelease == bool_true)
		{
			SetControlState(hWnd, IDC_STC_RETENTION, SCUI_STATE_SHOW);
			SetControlState(hWnd, IDC_CMB_RETENTION, SCUI_STATE_SHOW);
		}
		else
		{
			SetControlState(hWnd, IDC_STC_RETENTION, SCUI_STATE_ON);
			SetControlState(hWnd, IDC_CMB_RETENTION, SCUI_STATE_ON);
		}
		break;
	case IDC_CHK_PIN:
		m_PropertySheetState.wUsePIN = IsDlgButtonChecked(hWnd, IDC_CHK_PIN);
		if (m_PropertySheetState.wUsePIN == TRUE)
		{
			SetControlState(hWnd, IDC_EDT_PIN, SCUI_STATE_ON);
		}
		else
		{
			SetControlState(hWnd, IDC_EDT_PIN, SCUI_STATE_SHOW);
		}
		break;
	case IDC_CHK_UA_ALWAYS_PASS:
		CheckDlgButton(hWnd,IDC_CHK_UA_ALWAYS_PASS2, IsDlgButtonChecked(hWnd,IDC_CHK_UA_ALWAYS_PASS));
		break;

	case IDC_CHK_UA_ALWAYS_PASS2:
		CheckDlgButton(hWnd,IDC_CHK_UA_ALWAYS_PASS, IsDlgButtonChecked(hWnd,IDC_CHK_UA_ALWAYS_PASS2));
		break;
	case IDC_BTN_SHARED_OK:
	case IDOK:
	{
		OnIDOK(hWnd, hWndCtrl, wCtrlID);
		
	}
	break;

	case IDC_BTN_SHARED_CANCEL:
	case IDCANCEL:
	{
		CShowMessage showmessage(m_hWnd, m_hStringResourceHandle);
		if (IDNO == showmessage.Message(m_hWnd, IDS_MESTITLE_1, IDS_MESSAGE_65, MB_YESNO | MB_ICONQUESTION))
			return res;

		__super::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
		break;
	}
	break;
	case IDC_BTN_SHARED_SUB_DEFAULT:
	case IDC_BTN_SHARED_DEFAULT:
		SetTabDefaults(m_hWnd);
		break;
	//<S><A>to Fix Redmine Bug #2240 Issue 3,25.02.2022,SSDI:Chanchal Singla
	//Issue 3: Through tab order, move the focus on to the information icon Press "Space bar" from the keyboard, Information dialog is not popped up
	case	IDC_BTN_PRINTRELEASE_I:
	case 	IDC_BTN_RETENTION_I:
	case	IDC_BTN_USR_AUTH_I:
	case	IDC_BTN_UNAME_I:
	{
		UIControlMap::const_iterator Iterator = m_UIControls.find(wCtrlID);
		if (Iterator != m_UIControls.end())
		{
			CInformationIconButton *pIIcon = static_cast<CInformationIconButton*>(Iterator->second);
			if (pIIcon)
			{
				pIIcon->OnBnClickedInformationIcon(hWnd, wCtrlID, FALSE);
			}
			else
			{
				slog.putLog("CInformationIconButton::OnBnClicked -> NULL_OBJECT(CInformationIconButton)\n");
			}
		}
	}
	//<E>to Fix Redmine Bug #2240 Issue 3,25.02.2022,SSDI:Chanchal Singla
	default:
		res = CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
	}
	SetTabState(hWnd, wCtrlID);
	return res;
}

//=============================================================================
// function
//      OnEnChange
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      hWndCtrl            handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//      TRUE
//       FALSE
//
// outline
//      WM_COMMAND - EN_CHANGE
//=============================================================================
LRESULT CShDlgJobHand01::OnEnChange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	switch (wCtrlID)
	{
	case IDC_EDT_UA_LOGNAME:
	{
		pControl = (*pComponents)[wCtrlID];
		((CShEditLName*)pControl)->OnEnChange(wCtrlID, hWnd);
	}
		break;
	case IDC_EDT_UA_PASS:
	case IDC_EDT_ACNUM:
	case IDC_EDT_PIN:
	//case IDC_EDT_JNAME:
	case IDC_EDT_UNAME:
	{
		pControl = (*pComponents)[wCtrlID];
		((CJobControlEdit*)pControl)->OnEnChange(wCtrlID, hWnd);
	}
	break;
	//<S><A>To Implement Task#3137,25-07-2024,SSDI:Manoj S
	case IDC_EDT_JNAME:
	{
		pControl = (*pComponents)[wCtrlID];
		((CShEditJName*)pControl)->OnEnChange(wCtrlID, hWnd);
	}
	break;
	//<E>To Implement Task #3137,25-07-2024,SSDI:Manoj S 
	default:
		break;
	}
	res = 1;
	return res;
}
//=============================================================================
// function
//        OnIDOK
//
// parameters
//        hWnd         
//        wCtrlID      
//        pnmh        
//
// return value
//         TRUE
//        FALSE
//
// outline
//        WM_COMMAND - BN_CLICKED
//=============================================================================
LRESULT CShDlgJobHand01::OnIDOK(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = FALSE;
	DWORD			dwSize = 0;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CShowJobHandlingMessage* m_ObjShowJobHandlingMessage = NULL;
	m_ObjShowJobHandlingMessage = new CShowJobHandlingMessage(hWnd, m_hStringResourceHandle);
	if (NULL == m_ObjShowJobHandlingMessage)
	{
		slog.putLog("CShDlgJobHand01::CreateMessageClass -> NULL_OBJECT(CShowJobHandlingMessage)");
		return res;	//Coverity Fix - 91173
	}

	m_PropertySheetState.wUseUName = BST_CHECKED;
	m_PropertySheetState.wUseJName = BST_CHECKED;

	//::wcscpy_s(m_szDefUserName, _countof(m_PropertySheetState.szUserName), m_PropertySheetState.szUserName);

	if (m_PropertySheetState.wPPlcyLoginName == bool_true)
	{
		SecureZeroMemory(m_PropertySheetState.szUserName, sizeof(m_PropertySheetState.szUserName));
		if (m_PropertySheetState.wLoginName == bool_true)
		{
			//Coverity - 91178
			//::wcscpy_s(m_PropertySheetState.szUserName, _countof(m_PropertySheetState.szUserName), m_PropertySheetState.szLoginNameUA);
			//::wcscpy_s(m_PropertySheetState.szUserName, _countof(m_PropertySheetState.szUserName) / sizeof(WCHAR), m_PropertySheetState.szLoginNameUA);
			//<S><C> changed to fix when login name is entered with max characters
			//::wcscpy_s(m_PropertySheetState.szUserName, sizeof(m_PropertySheetState.szUserName) / sizeof(WCHAR), m_PropertySheetState.szLoginNameUA);
			::wcsncpy_s(m_PropertySheetState.szUserName, sizeof(m_PropertySheetState.szUserName) / sizeof(WCHAR), m_PropertySheetState.szLoginNameUA, JC_STR_JUNAME);
			//<E> changed to fix when login name is entered with max characters
		}
		else
		{
			//Coverity - 91178
			//::wcscpy_s(m_PropertySheetState.szUserName, _countof(m_PropertySheetState.szUserName), m_szDefUserName);
			::wcscpy_s(m_PropertySheetState.szUserName, _countof(m_PropertySheetState.szUserName) / sizeof(WCHAR), m_szDefUserName);
		}
	}
	//<S><A> To fix ssdi bug#3782, Harika 2021/5/17
	if (m_PropertySheetState.wLoginNameSSO == bool_true)
	{
		DWORD dwSSONameSize = (JC_STR_JUNAME + 1) * 2;
		::GetUserName(m_PropertySheetState.szSingleSignOnName, &dwSSONameSize);
		::wcscpy_s(m_PropertySheetState.szLoginNameUA, _countof(m_PropertySheetState.szUserName) / sizeof(WCHAR), m_PropertySheetState.szSingleSignOnName);
	}
	//<E> To fix ssdi bug#3782, Harika 2021/5/17
	// ================================================
	// _/_/_/ message
	// ================================================
	if (m_ObjShowJobHandlingMessage->MessageJobHandlingDlg(hWnd, &m_PropertySheetState, 
		m_pPrinterName) == 0)
	{
		m_PropertySheetState.wUseUName = BST_CHECKED;
		m_PropertySheetState.wUseJName = BST_CHECKED;

		if (m_PropertySheetState.wPPlcyLoginName == bool_true)
		{
			if (m_PropertySheetState.wLoginName == bool_true)
			{
				m_PropertySheetState.wUseUName = bool_true;
				SecureZeroMemory(m_PropertySheetState.szUserName, sizeof(m_PropertySheetState.szUserName));
				//<S><C> changed to fix when login name is entered with max characters
				::wcsncpy_s(m_PropertySheetState.szUserName, sizeof(m_PropertySheetState.szUserName) / sizeof(WCHAR), m_PropertySheetState.szLoginNameUA, JC_STR_JUNAME);
				//::wcscpy_s(m_PropertySheetState.szUserName, _countof(m_PropertySheetState.szUserName), m_PropertySheetState.szLoginNameUA);
				//<E> changed to fix when login name is entered with max characters
				SetEditValue(hWnd, IDC_EDT_UNAME, m_PropertySheetState.szUserName);
			}
		}

		// ================================================
		// _/_/_/  Always Use ... Ç
		// ================================================
		m_wSavePass = IsDlgButtonChecked(hWnd,IDC_CHK_ALWAYS_PIN);
		m_wSaveLoginPass = IsDlgButtonChecked(hWnd, IDC_CHK_UA_ALWAYS_PASS);
		m_wSaveAcNumber = IsDlgButtonChecked(hWnd, IDC_CHK_ALWAYS_ACNUM);
		m_wSaveUserName = IsDlgButtonChecked(hWnd, IDC_CHK_ALWAYS_UNAME);
		m_wSaveJobName = IsDlgButtonChecked(hWnd, IDC_CHK_ALWAYS_JNAME);

		// User Authentication, User Number Ç
		if (m_wSaveLoginPass == BST_CHECKED && m_PropertySheetState.wLoginName == BST_CHECKED)
			m_wSaveAcNumber = BST_CHECKED;

		if (m_wSaveAcNumber == BST_CHECKED && m_PropertySheetState.wUseAcNum == BST_CHECKED)
			m_wSaveLoginPass = BST_CHECKED;
		
		WriteJCdata_toReg(hWnd);

		PropStateToJobControl();
		memcpy_s(&m_pOemPrivateDevMode->scpi.ext.JobCtrl, sizeof(JOBCONTROL), &m_TempJobControl, sizeof(JOBCONTROL));

		res = __super::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
	
	}
	else
	{
		SetWindowLongPtr(m_hWnd, DWLP_MSGRESULT, TRUE);
		res  = TRUE;
		goto EXIT;
	}
	
EXIT:
	if (m_ObjShowJobHandlingMessage != NULL)
	{
		delete m_ObjShowJobHandlingMessage;
		m_ObjShowJobHandlingMessage = NULL;
	}
	return res;
}
JOBCONTROL *CShDlgJobHand01::GetJobControlData()
{
	return m_pJobControl;
}

//=============================================================================
// function
//      IsSaveData
//
// parameters
//     Nill
//
// return value
//     Word
//
// outline
//		Is Save Data
//=============================================================================
WORD CShDlgJobHand01::IsSaveData(void)
{
	WORD		wRet = 0;

	if (m_wSavePass == bool_true)
		wRet |= JC_PASSCODE;

	if (m_wSaveLoginPass == bool_true)
	{
		wRet |= JC_LOGINNAME;
		wRet |= JC_LOGINPASS;
	}

	if (m_wSaveAcNumber == bool_true)
		wRet |= JC_ACCOUNTNUM;

	if (m_wSaveUserName == bool_true)
		wRet |= JC_USERNAME;

	if (m_wSaveJobName == bool_true)
		wRet |= JC_JOBNAME;

	if (m_wSaveFolderPass == bool_true)
		wRet |= JC_FOLDERPASS;

	return 	wRet;
}

//=============================================================================
// function
//      SetTabState
//
// return value
//     BOOL
//
// outline
//      Sets Tab state
//=============================================================================
BOOL CShDlgJobHand01::SetTabState(HWND hDlg, WORD wCtrlID)
{
	BOOL			blRet = FALSE;
	// Retention
	Select(hDlg, IDC_CMB_RETENTION, m_PropertySheetState.wRetentionMode);

	//bydocumentFilling
	Select(hDlg, IDC_CMB_RETENTION_STOREDTO, m_PropertySheetState.wDocFileMode);

	CheckDlgButton(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
	CheckDlgButton(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);
	ShowConstraintsOnPrnRelease(hDlg);
	ShowConstraintsOnRetention(hDlg);

	m_wAuthen = SelectAuthentication(m_PropertySheetState.wLoginName, m_PropertySheetState.wLoginPass, m_PropertySheetState.wUseAcNum, m_PropertySheetState.wLoginNameSSO);
	//Authetication
	Select(hDlg, IDC_CMB_AUTHENTICATION, m_wAuthen);
	ShowConstraintsOnAuthentication(hDlg);
	UpdateEditBoxContent(hDlg, wCtrlID, m_wAuthen);
	//<S><A>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	//Known Issue : Login Name is not concealed on UI when Printing Policy - Use Windows Login Name as ëLogin Nameí is checked.
	SetEditLName(hDlg, IDC_EDT_UA_LOGNAME, m_PropertySheetState.szLoginNameUA);
	//<E>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
	ShowConstraintsOnUserName(hDlg);
	blRet = TRUE;

	return blRet;
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
VOID CShDlgJobHand01::SetEditLName(HWND hDlg, WORD wCtrlID, LPWSTR newVal, BOOL blForceMB)
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
			slog.putLog("CShDlgJobHand01::CShEditLName -> NULL_OBJECT(CShEditLName)\n");
		}

	}
}
//<E>To Fix Known Issue,2021.09.27,SSDI:Chanchal Singla
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
VOID CShDlgJobHand01::UpdateEditBoxContent(
	_In_ CONST HWND hDlg, _In_ CONST INT iCtrlID,short wAuthen)
{
	WCHAR* pszEditBoxContent = NULL;
	if ((IDC_CHK_PIN == iCtrlID) && (m_PropertySheetState.wUsePIN == bool_false))
	{
		pszEditBoxContent = m_PropertySheetState.szRetentionPassword;
		ClearEditBoxContent(IDC_EDT_PIN, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szRetentionPassword));
	}
	if ((IDC_CMB_AUTHENTICATION == iCtrlID) && (authentication_none == wAuthen))
	{
		pszEditBoxContent = m_PropertySheetState.szLoginNameUA;
		ClearEditBoxContent(IDC_EDT_UA_LOGNAME, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginNameUA));
		pszEditBoxContent = m_PropertySheetState.szLoginPasswordUA;
		ClearEditBoxContent(IDC_EDT_UA_PASS, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginPasswordUA));
		pszEditBoxContent = m_PropertySheetState.szUserNumber;
		ClearEditBoxContent(IDC_EDT_ACNUM, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserNumber));
	}
	if ((IDC_CMB_AUTHENTICATION == iCtrlID) && (authentication_lname == wAuthen))
	{
		pszEditBoxContent = m_PropertySheetState.szLoginPasswordUA;
		ClearEditBoxContent(IDC_EDT_UA_PASS, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginPasswordUA));
		pszEditBoxContent = m_PropertySheetState.szUserNumber;
		ClearEditBoxContent(IDC_EDT_ACNUM, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserNumber));
	}
	if ((IDC_CMB_AUTHENTICATION == iCtrlID) && (authentication_lname_pswd == wAuthen))
	{
		pszEditBoxContent = m_PropertySheetState.szUserNumber;
		ClearEditBoxContent(IDC_EDT_ACNUM, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserNumber));
	}
	if ((IDC_CMB_AUTHENTICATION == iCtrlID) && (authentication_acnum == wAuthen))
	{
		pszEditBoxContent = m_PropertySheetState.szLoginNameUA;
		ClearEditBoxContent(IDC_EDT_UA_LOGNAME, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginNameUA));
		pszEditBoxContent = m_PropertySheetState.szLoginPasswordUA;
		ClearEditBoxContent(IDC_EDT_UA_PASS, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginPasswordUA));
	}
	if ((IDC_CMB_AUTHENTICATION == iCtrlID) && (authentication_sso == wAuthen))
	{
		//<S><C>To Fix redmine Bug #4126,2021.August.05,SSDI:Chanchal Singla
		//Login Name displays empty in Authentication dialog when Use Window Login Name as Login Name is set to ON in Configuration tab
		if (m_PropertySheetState.wPPlcyWinLogin != bool_true) {
			pszEditBoxContent = m_PropertySheetState.szLoginNameUA;
			ClearEditBoxContent(IDC_EDT_UA_LOGNAME, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginNameUA));
		}
		//<E>To Fix redmine Bug #4126,2021.August.05,SSDI:Chanchal Singla
		pszEditBoxContent = m_PropertySheetState.szLoginPasswordUA;
		ClearEditBoxContent(IDC_EDT_UA_PASS, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szLoginPasswordUA));
		pszEditBoxContent = m_PropertySheetState.szUserNumber;
		ClearEditBoxContent(IDC_EDT_ACNUM, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserNumber));
	}
	if ((IDC_CMB_RETENTION == iCtrlID) && (retention_normal == wAuthen))
	{
		pszEditBoxContent = m_PropertySheetState.szRetentionPassword;
		ClearEditBoxContent(IDC_EDT_PIN, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szRetentionPassword));
	}
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
VOID CShDlgJobHand01::ClearEditBoxContent(_In_ CONST INT   iCtrlID,
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
//      ShowConstraintsOnPrnRelease
//
// return value
//          NIL
//
//Parameters 
//     hDlg : Handle to dialog
//     wCtrlID : Control ID
//
// outline
//      ShowConstraintsOnPrnRelease
//=============================================================================
VOID CShDlgJobHand01::ShowConstraintsOnPrnRelease(HWND hDlg, INT iCtrlID)
{
	//<S><A>To Fix Redmine #7483,07-06-2024,SSDI:Manoj S
	if ((m_PropertySheetState.wPPlcyPrintRelease == bool_true && m_PropertySheetState.byPrintRelease == bool_false) || (m_PropertySheetState.wPPlcyRetention == bool_true))
	//<E>To Fix Redmine #7483,07-06-2024,SSDI:Manoj S
	{
		//<S><C>To Fix Bug #4640,2021.12.22,SSDI:Chanchal Singla
		//ìStored toî drop down displays even though Document Filing drop down is not available
		if ((m_PropertySheetState.wPPlcyPrintRelease == bool_true) && (m_PropertySheetState.byPrintRelease == bool_false))
		{
			m_PropertySheetState.wPrintRelease = bool_true;
			CheckDlgButton(hDlg, IDC_CHK_PRINTRELEASE, m_PropertySheetState.wPrintRelease);

			if (m_PropertySheetState.wRetention != bool_false)
			{
				m_PropertySheetState.wRetentionMode = retention_normal;
				// Retention
				Select(hDlg, IDC_CMB_RETENTION, m_PropertySheetState.wRetentionMode);
				m_PropertySheetState.wRetention = bool_false;
			}
			//Coverity Fix 90879 - SSDI:Goutham - 20201123
			m_PropertySheetState.wPPlcyRetention = bool_false;
		}
		//<E>To Fix Bug #4640,2021.12.22,SSDI:Chanchal Singla
		if (m_PropertySheetState.wPPlcyPrintRelease == bool_true)
		{
			m_PropertySheetState.wPrintRelease = bool_true;
		}
		if (m_PropertySheetState.wPPlcyRetention == bool_true)
		{
			m_PropertySheetState.wPrintRelease = bool_false;
			m_PropertySheetState.wPPlcyRetention = bool_true;
			m_PropertySheetState.wRetentionMode = retention_normal;
		}
		SetControlState(hDlg, IDC_CHK_PRINTRELEASE, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_BTN_RETENTION_I, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_BTN_PRINTRELEASE_I, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_RETENTION, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_RETENTION, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_RETENTION_STOREDTO, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_RETENTION_STOREDTO, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
	}
	else
	{
		if (1 == m_PropertySheetState.byPrintRelease)
		{
			if (IsDlgButtonChecked(hDlg, IDC_CHK_PRINTRELEASE))
				CheckDlgButton(hDlg, IDC_CHK_PRINTRELEASE, BST_UNCHECKED);
			SetControlState(hDlg, IDC_BTN_RETENTION_I, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_BTN_PRINTRELEASE_I, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_CHK_PRINTRELEASE, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_STC_RETENTION, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_CMB_RETENTION, SCUI_STATE_ON);
		}
		else
		{
			if (IsDlgButtonChecked(hDlg, IDC_CHK_PRINTRELEASE))
			{
				SetControlState(hDlg, IDC_STC_RETENTION, SCUI_STATE_SHOW);
				SetControlState(hDlg, IDC_CMB_RETENTION, SCUI_STATE_SHOW);
				SetControlState(hDlg, IDC_BTN_RETENTION_I, SCUI_STATE_ON);
			}
			else
			{
				SetControlState(hDlg, IDC_STC_RETENTION, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_CMB_RETENTION, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_BTN_RETENTION_I, SCUI_STATE_OFF);
			}
			if (retention_normal != m_PropertySheetState.wRetentionMode)
			{
				SetControlState(hDlg, IDC_BTN_PRINTRELEASE_I, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_CHK_PRINTRELEASE, SCUI_STATE_SHOW);
			}
			else
			{
				SetControlState(hDlg, IDC_CHK_PRINTRELEASE, SCUI_STATE_ON);
				SetControlState(hDlg, IDC_BTN_PRINTRELEASE_I, SCUI_STATE_OFF);
			}
		}
	}
}
//=============================================================================
// function
//      ShowConstraintsOnAuthentication
//
// return value
//          NIL
//
//Parameters 
//     hDlg : Handle to dialog
//     wCtrlID : Control ID
//
// outline
//      ShowConstraintsOnAuthentication
//=============================================================================
VOID CShDlgJobHand01::ShowConstraintsOnAuthentication(HWND hDlg, INT iCtrlID)
{
	if (m_wAuthen == authentication_none) {
		SetControlState(hDlg,IDC_STC_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_EDT_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_STC_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_EDT_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_STC_ACNUM, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_EDT_ACNUM, SCUI_STATE_OFF);

		SetControlState(hDlg,IDC_CHK_UA_ALWAYS_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS2, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_CHK_ALWAYS_ACNUM, SCUI_STATE_OFF);
	}
	else if (m_wAuthen == authentication_lname) {
		if (m_PropertySheetState.wPPlcyWinLogin == bool_true) {
			SetControlState(hDlg,IDC_STC_UA_LOGNAME, SCUI_STATE_SHOW);
			SetControlState(hDlg,IDC_EDT_UA_LOGNAME, SCUI_STATE_SHOW);
		}
		else {
			SetControlState(hDlg,IDC_STC_UA_LOGNAME, SCUI_STATE_ON);
			SetControlState(hDlg,IDC_EDT_UA_LOGNAME, SCUI_STATE_ON);
		}
		SetControlState(hDlg,IDC_STC_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_EDT_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_STC_ACNUM, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_EDT_ACNUM, SCUI_STATE_OFF);

		if (m_PropertySheetState.wPPlcyUserAuth == bool_true) {
			SetControlState(hDlg,IDC_CHK_UA_ALWAYS_PASS, SCUI_STATE_OFF);
			SetControlState(hDlg,IDC_CHK_ALWAYS_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS2, SCUI_STATE_OFF);
		}
		else {
			SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_CHK_ALWAYS_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS2, SCUI_STATE_ON);
		}
	}
	else if (m_wAuthen == authentication_lname_pswd) {
		if (m_PropertySheetState.wPPlcyWinLogin == bool_true) {
			SetControlState(hDlg,IDC_STC_UA_LOGNAME, SCUI_STATE_SHOW);
			SetControlState(hDlg,IDC_EDT_UA_LOGNAME, SCUI_STATE_SHOW);
		}
		else {
			SetControlState(hDlg,IDC_STC_UA_LOGNAME, SCUI_STATE_ON);
			SetControlState(hDlg,IDC_EDT_UA_LOGNAME, SCUI_STATE_ON);
		}
		SetControlState(hDlg,IDC_STC_UA_PASS, SCUI_STATE_ON);
		SetControlState(hDlg,IDC_EDT_UA_PASS, SCUI_STATE_ON);
		SetControlState(hDlg,IDC_STC_ACNUM, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_EDT_ACNUM, SCUI_STATE_OFF);

		if (m_PropertySheetState.wPPlcyUserAuth == bool_true) {
			SetControlState(hDlg,IDC_CHK_UA_ALWAYS_PASS, SCUI_STATE_OFF);
			SetControlState(hDlg,IDC_CHK_ALWAYS_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS2, SCUI_STATE_OFF);
		}
		else {
			SetControlState(hDlg,IDC_CHK_ALWAYS_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg,IDC_CHK_UA_ALWAYS_PASS, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS2, SCUI_STATE_OFF);
		}
	}
	else if (m_wAuthen == authentication_acnum) {
		SetControlState(hDlg,IDC_STC_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_EDT_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_STC_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_EDT_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg,IDC_STC_ACNUM, SCUI_STATE_ON);
		SetControlState(hDlg,IDC_EDT_ACNUM, SCUI_STATE_ON);

		if ((m_PropertySheetState.wPPlcyUserAuth == bool_true) ||
			(m_PropertySheetState.wPPlcyUserNumber == bool_true)) {
			
			SetControlState(hDlg,IDC_CHK_UA_ALWAYS_PASS, SCUI_STATE_OFF);
			SetControlState(hDlg,IDC_CHK_ALWAYS_ACNUM, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS2, SCUI_STATE_OFF);
		}
		else {
			SetControlState(hDlg,IDC_CHK_UA_ALWAYS_PASS, SCUI_STATE_OFF);
			SetControlState(hDlg,IDC_CHK_ALWAYS_ACNUM, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS2, SCUI_STATE_OFF);
		}
	}
	//<S><A> To fix ssdi bug#3782, Harika 2021/5/17
	else if (m_wAuthen == authentication_sso)
	{
		SetControlState(hDlg, IDC_STC_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_UA_LOGNAME, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_UA_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_ACNUM, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_ACNUM, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CHK_UA_ALWAYS_PASS2, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CHK_ALWAYS_ACNUM, SCUI_STATE_OFF);
	}
	//<E> To fix ssdi bug#3782, Harika 2021/5/17
	if ((m_PropertySheetState.wPPlcyWinLogin == bool_true)
		|| (m_PropertySheetState.wPPlcyUserAuth == bool_true))
	{
		SetControlState(hDlg, IDC_BTN_USR_AUTH_I, SCUI_STATE_ON);
	}
	else
	{
		SetControlState(hDlg, IDC_BTN_USR_AUTH_I, SCUI_STATE_OFF);
	}
}
//= ============================================================================
// function
//      ShowConstraintsOnUserName
//
// return value
//          NIL
//
//Parameters 
//     hDlg : Handle to dialog
//     wCtrlID : Control ID
//
// outline
//      ShowConstraintsOnUserName
//=============================================================================
VOID CShDlgJobHand01::ShowConstraintsOnUserName(HWND hDlg, INT iCtrlID)
{
	if (m_PropertySheetState.wPPlcyLoginName == bool_true)
	{
		SetControlState(hDlg, IDC_BTN_UNAME_I, SCUI_STATE_ON);
		CheckDlgButton(hDlg, IDC_CHK_ALWAYS_UNAME, 0);
		SetControlState(hDlg, IDC_CHK_ALWAYS_UNAME, SCUI_STATE_SHOW);
		SetControlState(hDlg, IDC_EDT_UNAME, SCUI_STATE_SHOW);
		SetControlState(hDlg, IDS_IDC_STC_USERNAME, SCUI_STATE_SHOW);
		WCHAR* pszEditBoxContent = NULL;
		pszEditBoxContent = m_PropertySheetState.szUserName;
		ClearEditBoxContent(IDC_EDT_UNAME, hDlg, pszEditBoxContent, sizeof(m_PropertySheetState.szUserName));
	}
	else
	{
		SetControlState(hDlg, IDC_BTN_UNAME_I, SCUI_STATE_OFF);
	}
}
//= ============================================================================
// function
//      ShowConstraintsOnRetention
//
// return value
//          NIL
//
//Parameters 
//     hDlg : Handle to dialog
//     wCtrlID : Control ID
//
// outline
//      ShowConstraintsOnRetention
//=============================================================================
VOID CShDlgJobHand01::ShowConstraintsOnRetention(HWND hDlg, INT iCtrlID)
{
	if (retention_normal == m_PropertySheetState.wRetentionMode)
	{
		SetControlState(m_hWnd, IDC_CHK_PIN, SCUI_STATE_OFF);
		SetControlState(m_hWnd, IDC_EDT_PIN, SCUI_STATE_OFF);
		SetControlState(m_hWnd, IDC_CHK_ALWAYS_PIN, SCUI_STATE_OFF);
		m_PropertySheetState.wUsePIN = bool_false;
		CheckDlgButton(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
		wcscpy(m_PropertySheetState.szRetentionPassword, L"");
		UpdateEditBoxContent(hDlg, IDC_CHK_PIN);
		SetControlState(m_hWnd, IDC_STC_RETENTION_STOREDTO, SCUI_STATE_OFF);
		SetControlState(m_hWnd, IDC_CMB_RETENTION_STOREDTO, SCUI_STATE_OFF);

	}
	else if (retention_hold_after == m_PropertySheetState.wRetentionMode
		|| retention_hold_before == m_PropertySheetState.wRetentionMode
		|| retention_sample == m_PropertySheetState.wRetentionMode)
	{
		//<S><C>to Fix feedback issue #61,2021.12.15,SSDI:Chanchal Singla
		//When "Stored to" is "Quick File", PIN Code is not hidden.
		if (m_PropertySheetState.wDocFileMode == eFirstOption)//Quick Store
		{
			SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
			SetControlState(hDlg, IDC_CHK_ALWAYS_PIN, SCUI_STATE_OFF);
			m_PropertySheetState.wUsePIN = BST_UNCHECKED;
			CheckDlgButton(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
			wcscpy(m_PropertySheetState.szRetentionPassword, L"");
			UpdateEditBoxContent(hDlg, IDC_CHK_PIN);
		}
		else
		{
			SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_ON);
		}
		if (!IsDlgButtonChecked(hDlg, IDC_CHK_PIN))
		{
			//<S><C>To Fix Bug #4640,2021.12.22,SSDI:Chanchal Singla
			//ìStored toî drop down displays even though Document Filing drop down is not available
			if ((m_PropertySheetState.wPPlcyRetention == bool_true)
				|| (m_PropertySheetState.wDocFileMode == eFirstOption))//Quick Store
			{
				SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_CHK_PIN, SCUI_STATE_OFF);

				SetControlState(hDlg, IDC_CHK_ALWAYS_PIN, SCUI_STATE_OFF);
			}
			else
			{

				SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_SHOW);

				SetControlState(hDlg, IDC_CHK_ALWAYS_PIN, SCUI_STATE_ON);
			}
			//<E>to Fix feedback issue #61,2021.12.15,SSDI:Chanchal Singla
			//<E>To Fix Bug #4640,2021.12.22,SSDI:Chanchal Singla
			m_PropertySheetState.wUsePIN = bool_false;
			CheckDlgButton(hDlg, IDC_CHK_PIN, m_PropertySheetState.wUsePIN);
			wcscpy(m_PropertySheetState.szRetentionPassword, L"");
			UpdateEditBoxContent(hDlg, IDC_CHK_PIN);
		}
		else
		{
			SetControlState(hDlg, IDC_EDT_PIN, SCUI_STATE_ON);
			SetControlState(hDlg, IDC_CHK_ALWAYS_PIN, SCUI_STATE_ON);
		}
		//<S><C>To Fix Bug #4640,2021.12.22,SSDI:Chanchal Singla
		//ìStored toî drop down displays even though Document Filing drop down is not available
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
		//<E>To Fix Bug #4640,2021.12.22,SSDI:Chanchal Singla
	}

}
//=============================================================================
// function
//     Select
//
// return value
//     BOOL
//
// outline
//     To set states of combo box.
//=============================================================================
BOOL CShDlgJobHand01::Select(HWND hDlg, WORD wCtrlID, long PatternID)
{
	UIControlMap::const_iterator Iterator;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	Iterator = m_UIControls.find(wCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
		if (pImageCombo)
		{
			pImageCombo->Select(wCtrlID, hDlg, PatternID);
		}
		else
		{
			slog.putLog("CShDlgJobHand01::Select -> NULL_OBJECT(pImageCombo)\n");
		}

	}
	return TRUE;
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

long CShDlgJobHand01::GetValue(HWND hDlg, WORD wCtrlID)
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
			slog.putLog("CShDlgJobHand01::Select -> NULL_OBJECT(pImageCombo)\n");
		}

	}
	return Value;
}
//=============================================================================
// function
//      SelectAuthentication
//
// parameters
//      wVal                
//
// return value
//      êSHORT
//
// outline
//=============================================================================

short CShDlgJobHand01::SelectAuthentication(short wLoginName, short wLoginPass, short wUseAcNum, short wLoginNameSSO)
{
	short			wRet = FALSE;

	wRet = authentication_none;
	if ((wLoginName == bool_false) && (wLoginPass == bool_false) && (wUseAcNum == bool_false) && (wLoginNameSSO == bool_false)) {
		wRet = authentication_none;
	}
	else if ((wLoginName == bool_true) && (wLoginPass == bool_false) && (wUseAcNum == bool_false) && (wLoginNameSSO == bool_false)) {
		wRet = authentication_lname;
	}
	else if ((wLoginName == bool_true) && (wLoginPass == bool_true) && (wUseAcNum == bool_false) && (wLoginNameSSO == bool_false)) {
		wRet = authentication_lname_pswd;
	}
	else if ((wLoginName == bool_false) && (wLoginPass == bool_false) && (wUseAcNum == bool_true) && (wLoginNameSSO == bool_false)) {
		wRet = authentication_acnum;
	}
	else if ((wLoginName == bool_false) && (wLoginPass == bool_false) && (wUseAcNum == bool_false) && (wLoginNameSSO == bool_true)) {
		wRet = authentication_sso;
	}

	return wRet;
}
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
BOOL CShDlgJobHand01::ChangedAuthentication(short wAuthen)
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
		}
		m_PropertySheetState.wLoginNameSSO = bool_false;
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
void CShDlgJobHand01::GetEncodedPassWordData(CHAR* byEncodedPasswordData, WCHAR* pInputPasswordData, size_t sMaximumInputStringLength, size_t sOutputStringLength)
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
//      GetDecodedPassWordData
//
// return value
//     NILL
//
// outline
//   Decodes AES encrypted password data.
//=============================================================================
void CShDlgJobHand01::GetDecodedPassWordData(CHAR* byEncodedPasswordData, WCHAR* pOuputPasswordData, size_t sEncodedStringLength)
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
//=============================================================================
// function
//      GetEditVal
//
// parameters
//      hDlg - Handle to dialog.
//		wCtrlID - Control ID.
//
// return value
//      dwVal - Numeric Value for Width / Length.
//
// outline
//      Gets the value from the Width / Length Editbox and covert it to
//		numeric value and adjust based on the limit.
//=============================================================================

LPWSTR CShDlgJobHand01::GetEditVal(HWND hWnd, INT wCtrlID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	DWORD bRetVal = 0;
	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	pControl = (*pComponents)[wCtrlID];
	if (NULL == pControl)
	{
		slog.putLog("CShDlgJobHand01::SetEditValue -> NULL_POINTER(IDD_DLG_PRINTPOSITON)\n");
		return FALSE;
	}
	return ((CJobControlEdit*)pControl)->getVal();

	
}

//=============================================================================
// function
//     SelEditValue
//
// return value
//     BOOL
//
// outline
//     To set Edit Box Value.
//=============================================================================
BOOL CShDlgJobHand01::SetEditValue(HWND hDlg, INT wCtrlID, LPWSTR newVal)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	pControl = (*pComponents)[wCtrlID];		//set the edit box value 
	if (NULL == pControl)
	{
		slog.putLog("CShDlgJobHand01::SetEditValue -> NULL_POINTER(IDD_DLG_PRINTPOSITON)\n");
		return FALSE;
	}
	((CJobControlEdit*)pControl)->setVal(newVal);

	return TRUE; //Coverity Fix - 91174
}
//=============================================================================
// function
//        ShowCustomFolderDlg
//
// parameters
//        hWnd         
//
// return value
//     LRESULT
//
// outline
//        ShowCustomFolderDlg
//=============================================================================
LRESULT CShDlgJobHand01::ShowCustomFolderDlg(HWND hWnd)
{

	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	CUIControl		pControl;
	BOOL bIsUIUpdated = FALSE;
	INT_PTR			DlgRet = IDCANCEL;

	//Always wIsJobhandlingDlg variable pass TRUE value in Job handling Dilaog
	BOOL wIsJobHandlingDlg = TRUE;
	OEMDEV pOemPrivateDevMode = {};
	memcpy_s(&pOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));

	PROPSTATE pPropertySheetState;
	SecureZeroMemory(&pPropertySheetState, sizeof(PROPSTATE));
	memcpy_s(&pPropertySheetState, sizeof(PROPSTATE), &m_PropertySheetState, sizeof(PROPSTATE));

	CShDlgStored2	*pdlg = new CShDlgStored2(IDD_DLG_STORED_2, m_hStringResourceHandle, m_hDialogResourceHandle, m_hPrinter, 
		m_hPropertySheetHandle, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings,
		m_hBitmapResourceHandle, m_pOemPublicDevMode, &pOemPrivateDevMode, &pPropertySheetState, m_pPrinterName, wIsJobHandlingDlg);
	if (pdlg == NULL)
	{
		slog.putLog("CJobHandlingTabPropertyPage::OnCustomFolderDlg -> NULL_POINTER(IDD_DLG_STORED_2)\n");
		return FALSE;
	}

	pdlg->SetPrinterName(m_pPrinterName);
	pdlg->SetSaveData(m_wSaveFolderPass);

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
		m_wSaveFolderPass = pdlg->IsSaveData();
		if ((*pdlg).GetCount() <= 1)
		{
			m_PropertySheetState.wDocFileMode = 1;
		}

	}
	else
	{
		m_PropertySheetState.wDocFileMode = 1;
	}
	if (TRUE == bIsUIUpdated)
	{
		pControl.EnableApplyButton(hWnd);
	}

	if (pdlg != NULL)
	{
		delete pdlg;
		pdlg = NULL;
	}

	return TRUE;

}

 //=============================================================================
// function
//        JobNameShortening
//
// parameters
//        WCHAR FAR *pszJobName
//			DWORD dwSize         
//
// return value
//     BOOL
//
// outline
//        JobNameShortening
//=============================================================================
BOOL CShDlgJobHand01::JobNameShortening(WCHAR FAR *pszJobName, DWORD dwSize)
{
	BOOL	blRet = FALSE;
	long	lCount;
	short	i;
	WCHAR	szJobNameWk[(JC_STR_JOBNAME + 1) * 2];
	WCHAR	szData[(JC_STR_JOBNAME + 1) * 2] = { 0 };
	
	lCount = 5;
	std::wstring wszAppName[5];
	wszAppName[0] = L"Microsoft Word - ";
	wszAppName[1] = L"Microsoft PowerPoint - ";
	wszAppName[2] = L"Microsoft Outlook - ";
	wszAppName[3] = L"Microsoft Office Outlook - ";
	wszAppName[4] = L"Microsoft Office InfoPath - ";
	for (i = 0; i < lCount; i++)
	{
		if (0 == _wcsnicmp(pszJobName, wszAppName[i].c_str(), wcslen(wszAppName[i].c_str())))
		{
			wcscpy_s(szJobNameWk, _countof(szJobNameWk), pszJobName + wcslen(wszAppName[i].c_str()));
			wcscpy_s(pszJobName, dwSize, szJobNameWk);
			blRet = TRUE;
			break;
		}
	}
	blRet = TRUE;

	return blRet;
}
//<S><A>To Fix #3261, #3263, #3225, #3222 ,20210218,SSDI: Chanchal Singla
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
BOOL CShDlgJobHand01::SetRegJCInfoToPropState(PSCDM pscdm, PPROPSTATE pps, WCHAR FAR *pszSvrPrnName)
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
		(*pregjcinfo).wSingleSignOn = (*pps).wLoginNameSSO;	//<S><A> To fix ssdi bug#3782, Harika 2021/5/17
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
BOOL CShDlgJobHand01::RegJCInfoToPropState(PREGJCINFO prji, PPROPSTATE pps, WCHAR FAR *pszSvrPrnName)
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
	(*pps).wLoginNameSSO = (*prji).wSingleSignOn;	//<S><A> To fix ssdi bug#3782, Harika 2021/5/17
	(*pps).wDocFileFolder = (*prji).wFolderIndex;


	SecureZeroMemory((*pps).szUserName, sizeof((*pps).szUserName));
	SecureZeroMemory((*pps).szJobName, sizeof((*pps).szJobName));
	SecureZeroMemory((*pps).szUserNumber, sizeof((*pps).szUserNumber));
	SecureZeroMemory((*pps).szLoginNameUA, sizeof((*pps).szLoginNameUA));
	SecureZeroMemory((*pps).szLoginPasswordUA, sizeof((*pps).szLoginPasswordUA));
	SecureZeroMemory((*pps).szRetentionPassword, sizeof((*pps).szRetentionPassword));
	SecureZeroMemory((*pps).szSingleSignOnName, sizeof((*pps).szSingleSignOnName));
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
	if ((*prji).wSingleSignOn == bool_true)
		::lstrcpy((*pps).szSingleSignOnName, (*prji).szSingleSignOnName);
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
BOOL CShDlgJobHand01::PropStateToRegJCInfo(PPROPSTATE pps, PREGJCINFO prji, WCHAR FAR *pszSvrPrnName)
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
	(*prji).wSingleSignOn = (*pps).wLoginNameSSO;	//<S><A> To fix ssdi bug#3782, Harika 2021/5/17


	SecureZeroMemory((*prji).szUserName, sizeof((*prji).szUserName));
	SecureZeroMemory((*prji).szJobName, sizeof((*prji).szJobName));
	SecureZeroMemory((*prji).szAccountNum, sizeof((*prji).szAccountNum));
	SecureZeroMemory((*prji).szLoginName, sizeof((*prji).szLoginName));
	SecureZeroMemory((*prji).szLoginPassword, sizeof((*prji).szLoginPassword));
	SecureZeroMemory((*prji).szPIN, sizeof((*prji).szPIN));
	SecureZeroMemory((*prji).szSingleSignOnName, sizeof((*prji).szSingleSignOnName));
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
//        WriteJCdata_toReg
//
// parameters
//        hWnd              
//
// return value
//         TRUE
//       FALSE
//
// outline
//        WriteJCdata_toReg
//=============================================================================
BOOL CShDlgJobHand01::WriteJCdata_toReg(HWND hWnd)
{
	LRESULT			res = 0;
	PREGJCINFO		pregjc = NULL;

	CShRegJC		*preg = NULL;
	short			wField = 0;

	pregjc = new REGJCINFO;
	if (pregjc == NULL)
		goto EXIT;

	preg = new CShRegJC(m_hStringResourceHandle, m_pPrinterName);
	if (preg == NULL)
		goto EXIT;

	SecureZeroMemory(pregjc, sizeof(REGJCINFO));
	
	wField = IsSaveData();
	if (wField != 0)
	{
		if (PropStateToRegJCInfo(&m_PropertySheetState, pregjc, m_pPrinterName) != FALSE)
			(*preg).WriteJCData(m_pPrinterName, pregjc, wField, m_PropertySheetState.wPPlcyWinLogin);
		res = TRUE;
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

	return res;
}
//<E>To Fix #3261, #3263, #3225, #3222 ,20210218,SSDI: Chanchal Singla