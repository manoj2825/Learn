//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : ShCmbMarginShift.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Chanchal Parkash
//
//  Date		  : 10 June 2020
//
//	Description   : This class implements Margin Shift combo box control
//===========================================================================
//#pragma once
#include "precomp.h"
//For Logging Purpose
#include "MFPLogger.h"
#include "../../common/include/sh_api.h"
#include <wchar.h>
#include <winuser.h>
#include "shregus.h"
#include "shJsonus.h"


//=============================================================================
// function
//      CShCmbUserset
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
//      Constructor to the class CShCmbUserset.
//=============================================================================

CShCmbUserset::CShCmbUserset(
								PFEATUREINFOLIST pFeatureInfoList,
								IPrintCoreHelperUni* pPrintCoreHelperUni,
								HMODULE hStringResourceHandle,
								HMODULE hDialogResourceHandle,
								HMODULE hBitmapResourceHandle,
								CShowMessage* ObjCShowMessage,
								POEMDEV pOemPrivateDevMode,
								PDEVMODE pOemPublicDevMode,
								WCHAR * pPrinterName,
								SHORT* sUsIndex,
								PPROPINFO ppi,
								UINT iCtrlID,
								const PROPSTATE& PropertySheetState,
								BOOL IsDevicePropertyPage):CShImageCombo(
																	iCtrlID,
																	hStringResourceHandle
																	)
{
	m_pFeatureInfoList = pFeatureInfoList;
	m_pPrintCoreHelperUni = pPrintCoreHelperUni;
	m_ObjCShowMessage = ObjCShowMessage;
	m_hDialogResourceHandle = hDialogResourceHandle;
	m_hBitmapResourceHandle = hBitmapResourceHandle;
	m_lUntitled = -1;
	m_pUsIndex = sUsIndex;
	m_ppi = ppi;
	m_pps = PropertySheetState;
	m_pPrinterName = NULL;
	m_pOemPrivateDevMode = pOemPrivateDevMode;
	m_pOemPublicDevMode = pOemPublicDevMode;
	m_pPrinterName = pPrinterName;
	CGPDData gpddata(m_pPrintCoreHelperUni);
	m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();
	m_PaperInfo = gpddata.GetPaperInfoList();
	m_IsDevicePropertyPage = IsDevicePropertyPage;
	m_szDateTime = NULL;
	InitializeGPDTimeStamp();
}

//=============================================================================
// function
//      CShCmbUserset
//
// parameters
//      NILL            
//
// return value
//     NILL
//
// outline
//      Destructure to the class CShCmbUserset.
//=============================================================================
CShCmbUserset::~CShCmbUserset()
{
	if (m_szDateTime != NULL)
	{
		delete[] m_szDateTime;
		m_szDateTime = NULL;
	}
}

//=============================================================================
// function
//      OnCommand
//
// parameters
//      iCtrlID --- Control ID of combo  box control  
//		hDlg ------- handle to the dialog box that contains the control.  
//     iCommand ---  combo  box Commands   
// return value
//     HRESULT
//
// outline
//      Handles command sent by property sheet
//=============================================================================
HRESULT
CShCmbUserset::OnCommand(_In_ CONST INT   iCtrlID,
	_In_ CONST HWND hDlg,
	_In_ INT iCommand
)
{
	UNREFERENCED_PARAMETER(iCtrlID);
	UNREFERENCED_PARAMETER(hDlg);
	HRESULT hr = S_OK;
	return hr;
}
//=======================================================================================
//Routine Name:
//
//	CShCmbUserset::SetTabDefaults
//
//Routine Description:
//
//	The routine resets the UI to factory defaults
//
//Arguments:
//
//	iCtrlID - Control ID
//	hDlg - handle to the dialog
//
//Return Value:
//
//   S_OK
//
//========================================================================================
HRESULT CShCmbUserset::SetTabDefaults(_In_ CONST INT   iCtrlID, _In_ CONST HWND hDlg)
{
	UNREFERENCED_PARAMETER(iCtrlID);
	UNREFERENCED_PARAMETER(hDlg);
	HRESULT hr = S_OK;
	return hr;
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
HRESULT CShCmbUserset::OnInit(const INT iCtrlID, const HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	BOOL				blRet = FALSE;

	DWORD				dwCount = 0;

	short				wRet = -1;
	short				wType = us_no_reset;

	WCHAR				*pTitle = NULL;

	PSCDM				pscdm = NULL;
	POEMDEV				pDefaultDevMode = NULL;
	PDEVMODE			pOemPublicDevMode = NULL;
	HANDLE				hPrinter = NULL;
	CShRegUS			*preg = NULL;
	GPDFEATUREOPTIONLIST pFeatureOptionList; 
	CShJsonUS			*pjsonus = NULL;
	vector<Json::Value> vecValue;
	WCHAR 				*pszGPDDateTime = NULL;
	WCHAR                *pszRegDateTime = NULL;
	WCHAR				*pdwRegDateTime = NULL;
	long				bufsize = 0;
	LCID				dwID = 0;
	LCID				dwRegID = 0;
	
	DWORD			dwCmbHeight = 0;
	int				height = 0;
	DWORD				i;
	//<S><A>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	//Change the specification for function limitation, display method by changing option
	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S

#if 0  // For a testing purpose Later we remove Following line : SSDI:Chanchal Singla

	m_hWnd = hDlg;
#endif
	DWORD				dwIndex = 0;
	GetComboBoxSizeInfo(&dwCmbHeight);
	if (dwCmbHeight != 0)
	{
		HWND hWnd = GetDlgItem(hDlg, iCtrlID);
		//int iCmbHeight = 30;
		height = GetScreenHeight();
		//		//<S><A> SSDI Bug Fix 3687 - SSDI:Seetharam - 20210811
		//#ifdef ARM64
		//		if (height > 1080)
		//			iCmbHeight = height / iCmbHeight;
		//#endif
		//		//<S><E> SSDI Bug Fix 3687 - SSDI:Seetharam - 20210811
		SendMessage(hWnd, CB_SETITEMHEIGHT, (WPARAM)-1, (LPARAM)dwCmbHeight);
		SendMessage(hWnd, CB_SETITEMHEIGHT, (WPARAM)0, (LPARAM)dwCmbHeight);
		int lines = CalcurateLines(height, dwCmbHeight);
		SendMessage(hWnd, CB_SETMINVISIBLE, (WPARAM)lines, 0);
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
	if (m_pPrinterName != NULL && preg != NULL)
	{
		// ================================================
		// _/_/_/  UserSettingÇ
		// ================================================
		if ((*m_pmcf).IsWriteToJson() == TRUE)
			dwCount = (*pjsonus).ReadCount();
		else
			dwCount = (*preg).ReadCount(m_pPrinterName);

		if (dwCount < 1)
		{
			wType = us_reset_all;
		}
		else
		{
			dwID = GetSystemDefaultLCID();

			if ((*m_pmcf).IsWriteToJson() == TRUE)
				(*pjsonus).ReadLCID(&dwRegID);
			else
				(*preg).ReadLCID(m_pPrinterName, &dwRegID);
			
			if (dwRegID != dwID)
				wType = us_reset_fd;

			
				pszGPDDateTime = GetTimeStamp();
			

			if (pszGPDDateTime != NULL)
			{
				pdwRegDateTime =  new WCHAR[(SCUI_FILEDATETIME_STRLEN * 2)];

				if (pdwRegDateTime == NULL)
					goto EXIT;

				SecureZeroMemory(pdwRegDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));

				// Factory DefaultsÅ
				if ((*m_pmcf).IsWriteToJson() == TRUE)
					(*pjsonus).ReadDateTime(pdwRegDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));
				else
					(*preg).ReadDateTime(m_pPrinterName, pdwRegDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));

				pszRegDateTime =  new WCHAR[(SCUI_FILEDATETIME_STRLEN * 2)];

				if (pszRegDateTime == NULL)
					goto EXIT;

				SecureZeroMemory(pszRegDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));
				//<S><C>To fix Coverity #91861,02.05.2022,SSDI: Chanchal Singla
				///wcscpy(pszRegDateTime, pdwRegDateTime);
				memcpy(pszRegDateTime, pdwRegDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));
				//<E>To fix Coverity #91861,02.05.2022,SSDI: Chanchal Singla
				if (wcscmp(pszRegDateTime, pszGPDDateTime) != 0)
					wType = us_reset_fd;
			}

			if ((*m_pmcf).IsWriteToJson() == FALSE)
			{
				if ((*preg).IsVersionChanged(m_pPrinterName) == FALSE)
				{
					slog.putLog("... version is not changed.");
					if ((*preg).IsLocaleChanged(m_pPrinterName) == TRUE)
					{
						slog.putLog("... locale is changed.");
						wType = us_reset_fd;
					}
				}
				else
				{
					slog.putLog("... version is changed.");
					wType = us_reset_all;

				}
			}
			else
			{
				if ((*pjsonus).IsVersionChanged() == FALSE)
				{
					slog.putLog("... version is not changed.");
					if ((*pjsonus).IsLocaleChanged() == TRUE)
					{
						slog.putLog("... locale is changed.");
						wType = us_reset_fd;
					}
				}
				else
				{
					slog.putLog("... version is changed.");
					wType = us_reset_all;

				}
			}
		}
		switch (wType)
		{
		case us_reset_all:
			slog.putLog("----> reset user settings."); break;
		case us_reset_fd:
			slog.putLog("----> reset factory defaults."); break;
			//fix Coverity #91315,20200510,SSDI:Chanchal Singla
		/*case us_update:
			slog.putLog("----> reset factory defaults, update others."); break;*/
		case us_no_reset:
		default:
			slog.putLog("----> not reset user settings."); break;
		}

		// ================================================
		// _/_/_/  UserSettingÇ
		// ================================================
		if (wType != us_no_reset)
		{
			LRESULT			res = 0;
			
			pscdm = new SCDM[2];
			if (pscdm == NULL)
				goto EXIT;

			SecureZeroMemory(pscdm, sizeof(SCDM) * 2);
			pOemPublicDevMode = new DEVMODE;
			if (NULL == pOemPublicDevMode)
			{
				return E_FAIL;
			}
			pDefaultDevMode = new OEMDEV;
			if (NULL == pDefaultDevMode)
			{
				return E_FAIL;
			}
			// Set all the member variables to zero
			memset(pDefaultDevMode, 0, sizeof(OEMDEV));
			memset(pOemPublicDevMode, 0, sizeof(DEVMODE));
			//-----------------------------------------------------------------
			// OpenPrinter()Ç
			//-----------------------------------------------------------------
			ResetDevmode(m_pPrinterName, pDefaultDevMode, m_DefDMInfo, m_PaperInfo);
			ResetPublicDevmode(pOemPublicDevMode, m_pPrinterName);
			FillPubDModeData(pDefaultDevMode, pOemPublicDevMode);
			if (DMPAPER_A4 == GetPaperSizeDef())
			{
				// first time when install a driver Orignal size drop down Default Should be A4
				pDefaultDevMode->scpi.pub.dmPaperSize = DMPAPER_A4;
				pDefaultDevMode->scpi.pub.dmPaperLength = 2970;
				pDefaultDevMode->scpi.pub.dmPaperWidth = 2100;
				PaperInfoMap::const_iterator it;
				it = m_PaperInfo.find("A4");
				if (it != m_PaperInfo.end())
				{
					pDefaultDevMode->scpi.ext.nDstWidthMilli = (SHORT)it->second.PaperWidth;
					pDefaultDevMode->scpi.ext.nDstHeightMilli = (SHORT)it->second.PaperLength;

				}
				pDefaultDevMode->scpi.ext.nZoomSrcPaperSize = DMPAPER_A4;
				pDefaultDevMode->scpi.ext.nFTPPaperSize = DMPAPER_A4;
				pDefaultDevMode->scpi.ext.nPamphPaperSize = DMPAPER_A4;
				pDefaultDevMode->scpi.doc.dwImageableWidthPixel = 4760;
				pDefaultDevMode->scpi.doc.dwImageableHeightPixel = 6814;
				pDefaultDevMode->scpi.ext.dwDstWidthPixel = 2 * pDefaultDevMode->scpi.doc.nLeftMarginPixel + pDefaultDevMode->scpi.doc.dwImageableWidthPixel;
				pDefaultDevMode->scpi.ext.dwDstHeightPixel = 2 * pDefaultDevMode->scpi.doc.nTopMarginPixel + pDefaultDevMode->scpi.doc.dwImageableHeightPixel;
				//<S><A>To Fix SSDI Bug #2706,2020-09-04,SSDI:Chanchal Singla
			}
			else if (DMPAPER_LETTER == GetPaperSizeDef())
			{
				pDefaultDevMode->scpi.pub.dmPaperSize = DMPAPER_LETTER;
				pDefaultDevMode->scpi.pub.dmPaperLength = 2794;
				pDefaultDevMode->scpi.pub.dmPaperWidth = 2159;
				PaperInfoMap::const_iterator it;
				it = m_PaperInfo.find("LETTER");
				if (it != m_PaperInfo.end())
				{
					pDefaultDevMode->scpi.ext.nDstWidthMilli = (SHORT)it->second.PaperWidth;
					pDefaultDevMode->scpi.ext.nDstHeightMilli = (SHORT)it->second.PaperLength;

				}
				pDefaultDevMode->scpi.ext.nZoomSrcPaperSize = DMPAPER_LETTER;
				pDefaultDevMode->scpi.ext.nFTPPaperSize = DMPAPER_LETTER;
				pDefaultDevMode->scpi.ext.nPamphPaperSize = DMPAPER_LETTER;
				pDefaultDevMode->scpi.doc.dwImageableWidthPixel = 4900;
				pDefaultDevMode->scpi.doc.dwImageableHeightPixel = 6400;
				pDefaultDevMode->scpi.ext.dwDstWidthPixel = 2 * pDefaultDevMode->scpi.doc.nLeftMarginPixel + pDefaultDevMode->scpi.doc.dwImageableWidthPixel;
				pDefaultDevMode->scpi.ext.dwDstHeightPixel = 2 * pDefaultDevMode->scpi.doc.nTopMarginPixel + pDefaultDevMode->scpi.doc.dwImageableHeightPixel;	
			}

			MergeDevmode(pDefaultDevMode);
			if (m_DefDMInfo[FEATURE_COLLATE].iDevmodeValue == bool_true)
			{
				pDefaultDevMode->scpi.pub.dmCollate = DMCOLLATE_TRUE;
			}
			else if (m_DefDMInfo[FEATURE_COLLATE].iDevmodeValue == bool_false)
			{
				pDefaultDevMode->scpi.pub.dmCollate = DMCOLLATE_FALSE;
			}
			ResetGPDFeatureOptionList(m_pPrinterName, &pFeatureOptionList);
#if 0  // For a testing purpose Later we remove that line : SSDI:Chanchal Singla
			MessageBox(m_hWnd, pFeatureOptionList.BookletOutputSize, L"pFeatureOptionList.BookletOutputSize", MB_OK | MB_ICONEXCLAMATION);
#endif		
			if (wType == us_reset_all)
			{
				if ((*m_pmcf).IsWriteToJson() == TRUE)
				{
					(*pjsonus).reset(&pDefaultDevMode->scpi, m_pFeatureInfoList, &pFeatureOptionList, TRUE);
					dwCount = (*pjsonus).ReadCount();
				}
				else
				{
					(*preg).reset(m_pPrinterName, &pDefaultDevMode->scpi, m_pFeatureInfoList, &pFeatureOptionList, TRUE);
					dwCount = (*preg).ReadCount(m_pPrinterName);
				}
				
				bufsize = SCUI_FILEDATETIME_STRLEN;
				
				pszGPDDateTime = GetTimeStamp();
				
				if (pszGPDDateTime != NULL)
				{
					pdwRegDateTime = new WCHAR[(SCUI_FILEDATETIME_STRLEN * 2)];
					if (pdwRegDateTime == NULL) {
						goto EXIT;
					}
					SecureZeroMemory(pdwRegDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));
				}
			}
			else if (wType == us_reset_fd)
			{
				if ((*m_pmcf).IsWriteToJson() == TRUE)
					(*pjsonus).reset(&pDefaultDevMode->scpi, m_pFeatureInfoList, &pFeatureOptionList, FALSE);
				else
					(*preg).reset(m_pPrinterName, &pDefaultDevMode->scpi, m_pFeatureInfoList, &pFeatureOptionList, FALSE);
			}
			//Å//User Setting
			//<S><C>To fix Coverity #91860,02.05.2022,SSDI: Chanchal Singla
			if (pszGPDDateTime != NULL)
			{
				//wcscpy(pdwRegDateTime, pszGPDDateTime);
				memcpy(pdwRegDateTime, pszGPDDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));
			}
			//<E>To fix Coverity #91861,02.05.2022,SSDI: Chanchal Singla
			if ((*m_pmcf).IsWriteToJson() == TRUE)
			{
				(*pjsonus).WriteDateTime(pdwRegDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));

				(*pjsonus).WriteLCID(dwID);
			}
			else
			{
				(*preg).WriteDateTime(m_pPrinterName, pdwRegDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));

				(*preg).WriteLCID(m_pPrinterName, dwID);
			}

		}

		pjsonus->WriteJsonDataToFile();
	
		setLimitByOptionTbl();
		

		if (CreateShCtrlItem((short)dwCount) != FALSE)
		{
			for (i = 0; i < dwCount; i++)
			{
				//<S><A>To Implement Task #3118,22-04-2024,SSDI:Manoj S
				if (TRUE == (*m_pmcf).IsLimitByOptionHideUI())
				{ 
					if ((chkLimitByOptionTblTab(i) != FALSE) && i > 0) {	//ignore in case of Factory default as it is must		
						DecrementItemCount();
						continue;
					}
				}
				//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S
				pTitle = NULL;

				pTitle = new WCHAR[(SCUI_TITLE_READ_MAX + 1) * 2];
				if (pTitle == NULL)
					break;

				SecureZeroMemory(pTitle, ((SCUI_TITLE_READ_MAX + 1) * 2));

				if (i == 0)
				{
					LoadString(m_hStringResourceHandle, IDS_INDEX0_USERSETTING, pTitle, (SCUI_TITLE_READ_MAX + 1) * 2);
				}
				else
				{
					if ((*m_pmcf).IsWriteToJson() == TRUE)
					{
						if ((*pjsonus).GetTitle(pTitle, (SCUI_TITLE_READ_MAX + 1) * 2, i) == FALSE)
							break;
					}
					else
					{
						if ((*preg).GetTitle(m_pPrinterName, pTitle, (SCUI_TITLE_READ_MAX + 1) * 2, i) == FALSE)
							break;
					}
				}
	
				m_pci[dwIndex].wIndex = (short)i;
				m_pci[dwIndex].lValue = (long)i;
				m_pci[dwIndex].pStr = pTitle;
				dwIndex++;
			}

			blRet = InitItems(iCtrlID, hDlg);
		}
	}
	if (*m_pUsIndex == (short)0x7fff)
	{
		*m_pUsIndex = (*m_ppi).wUsIndex;
	}
	if (blRet != FALSE)
		wRet = wType;

EXIT:

	m_lUntitled = -1;

	if (pscdm != NULL)
		delete[] pscdm;

	if (NULL != pDefaultDevMode)
	{
		delete pDefaultDevMode;
		pDefaultDevMode = NULL;
	}
	if (NULL != pOemPublicDevMode)
	{
		delete pOemPublicDevMode;
		pOemPublicDevMode = NULL;
	}

	if (pszRegDateTime != NULL)
	{
		delete[] pszRegDateTime;
		pszRegDateTime = NULL;
	}

	if (pdwRegDateTime != NULL)
	{
		delete[] pdwRegDateTime;
		pdwRegDateTime = NULL;
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
	//<S><A>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = NULL;
	}
	//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	slog.putLog("CShCmbUserSet::init");

	return wRet;
	//return hr;
}

//=============================================================================
// function
//      AddUntitled
//
// parameters
//      HWND hDlg
//
// return value
//     long
//
// outline
//      Untitled
//=============================================================================
long CShCmbUserset::AddUntitled(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	long	lRet = -1;

	long	lIndex = CB_ERR;
	WCHAR	szStr[128];

	if (m_lUntitled != -1)
	{
		SendDlgItemMessage(hDlg, IDC_CMB_SHARED_USRSET, CB_SETCURSEL, m_lUntitled, 0);
		goto EXIT;
	}

	// Coverity Fix: 91369 - SSDI Seetharam - 20210504
	LoadString(m_hStringResourceHandle,IDS_INDEX0_USERSETTING2, szStr, sizeof(szStr) / sizeof(WCHAR));
	lIndex = SendMessage(::GetDlgItem(hDlg, IDC_CMB_SHARED_USRSET), CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(szStr));
	if (lIndex != CB_ERR)
	{
		if(SendDlgItemMessage(hDlg, IDC_CMB_SHARED_USRSET, CB_SETCURSEL, lIndex, 0) != CB_ERR)
		{
			slog.putLog("CShCmbUserSet::AddUntitled");
			lRet = lIndex;
			m_lUntitled = lIndex;
			SendDlgItemMessage(hDlg, IDC_CMB_SHARED_USRSET, CB_SETCURSEL, lIndex, 0);
		}
	}

EXIT:

	return lRet;
}


//=============================================================================
// function
//      DeleteUntitled
//
// parameters
//      HWND hDlg
//
// return value
//      long
//
// outline
//      Untitled
//=============================================================================
long CShCmbUserset::DeleteUntitled(HWND hDlg)
{
	long	lRet = -1;

	if (m_lUntitled != -1)
	{
		if(SendMessage(::GetDlgItem(hDlg, IDC_CMB_SHARED_USRSET), CB_DELETESTRING, m_lUntitled, 0) != CB_ERR)
		{
			lRet = m_lUntitled;
			m_lUntitled = -1;
		}
	}

	return lRet;
}


//=============================================================================
// function
//      IsUntitled
//
// parameters
//      
//
// return value
//      Untitled TRUE
//      Untitled FALSE
//
// outline
//      UntitledÇ
//=============================================================================
BOOL CShCmbUserset::IsUntitled(void)
{
	return (m_lUntitled != -1);
}


//=============================================================================
// function
//      SetGrayState
//
// parameters
//      ñ≥Çµ
//
// return value
//      ñ≥Çµ
//
// outline
//      
//=============================================================================
void CShCmbUserset::SetGrayState(void)
{
}



///====================================================================================
//Routine Name: 
//				CShCmbUserset::DrawUserSetting
//
//Routine Description:
//			Routine for Drawing the Selected USERSETIING String on ComboBox
//
//Arguments:
//				LPARAM     lParam
//
//Return Value:
//			VOID
//=====================================================================================
void CShCmbUserset::DrawUserSetting(LPARAM lParam)
{
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;
	RECT				rcItem = (*pdis).rcItem;
	RECT				rcItemImg = (*pdis).rcItem;
	char				szText[256];
	int                 ret = 0;

	HDC hDCMem = ::CreateCompatibleDC((*pdis).hDC);
	if (hDCMem == NULL)
	{
		return;
	}

	SetBkMode((*pdis).hDC, OPAQUE);
	::ZeroMemory(szText, sizeof(szText));

	ret = GetItemText(lParam, szText, sizeof(szText));


	if (m_blIconDraw == TRUE && m_pBmpList != NULL &&
		(*m_pBmpList).GetEnableData((*pdis).itemID) == TRUE)
	{
		if (m_iPosY == POS_INIT_VAL)
		{
			int iHeight = 0;
			int iWidth = 0;
			int itemHeight = rcItemImg.bottom - rcItemImg.top;
			(*m_pBmpList).GetImageSize(&iWidth, &iHeight);
			m_iWidth = iWidth;
			m_iPosY = (itemHeight - iHeight) / 2;
		}
		rcItem.left = rcItem.left + m_iWidth + LEFT_VOID_VAL;
	}

	rcItem.left += LEFT_VOID_VAL;

	if ((*pdis).itemState & ODS_DISABLED)
	{
		FillRect((*pdis).hDC, &(*pdis).rcItem, GetSysColorBrush(COLOR_3DFACE));
		MyDrawText((*pdis).hDC, &rcItem, szText, COLOR_3DFACE, COLOR_GRAYTEXT, TRANSPARENT, ret);
	}
	else
	{
		if (((*pdis).itemState & ODS_SELECTED) && // SELECTED
			((*pdis).itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
		{
			FillRect((*pdis).hDC, &(*pdis).rcItem, GetSysColorBrush(COLOR_HIGHLIGHT));
			MyDrawText((*pdis).hDC, &rcItem, szText, COLOR_HIGHLIGHT, COLOR_HIGHLIGHTTEXT, TRANSPARENT, ret);
		}

		if (!((*pdis).itemState & ODS_SELECTED) && // !SELECTED
			((*pdis).itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
		{
			FillRect((*pdis).hDC, &(*pdis).rcItem, GetSysColorBrush(COLOR_WINDOW));
			MyDrawText((*pdis).hDC, &rcItem, szText, COLOR_WINDOW, COLOR_WINDOWTEXT, TRANSPARENT, ret);
		}
	}

	if (m_pBmpList != NULL && m_blIconDraw == TRUE &&
		(*m_pBmpList).GetEnableData((*pdis).itemID) == TRUE)
	{
		// Iconï\é¶
		rcItemImg.right = m_iWidth + LEFT_VOID_VAL;
		rcItemImg.top += m_iPosY;
		rcItemImg.bottom += 0;
		rcItemImg.left += LEFT_VOID_VAL;

		(*m_pBmpList).draw((*pdis).hDC, &rcItemImg, (*pdis).itemID);
	}
	if (NULL != hDCMem)
	{
		DeleteDC(hDCMem);
		hDCMem = NULL;
	}
}
//=============================================================================
// function
//      GetItemText
//
// parameters
//        LPARAM lParam, WCHAR* pszText, UINT size
//
// return value
//        TRUE 
//        FALSE
//
// outline
//      GetItemText
//=============================================================================
int CShCmbUserset::GetItemText(LPARAM lParam, CHAR* pszText, UINT size)
{
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;
	int                 lRet = -1;

	if (pszText == NULL)
		goto EXIT;

	if ((*pdis).itemID == m_lUntitled || (*pdis).itemID & 0x80000000)
	{
		LoadString(m_hStringResourceHandle,IDS_INDEX0_USERSETTING2, (WCHAR*)pszText, size);
		lRet = 1;
	}
	else
	{
		lRet = CShImageCombo::GetItemText(lParam, pszText, size);
	}
EXIT:
	return lRet;
}
BOOL CShCmbUserset::ResetDevmode(LPTSTR pPrinterName, POEMDEV pOemDevMode, std::vector<DEFAULTDEVMODEINFO> DefDMInfo, PaperInfoMap PaperInfo)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL	bResult = FALSE;
	WCHAR			szPrinter1[MAX_PATH] = { 0 };
	if ((NULL == pPrinterName) || (NULL == pOemDevMode))
	{
		return FALSE;
	}
	
	//Calling GetPureName to get the pure name from lpPrintername which is"\\SONALI-SSDI01\SHARP UD2L PCL6,LocalsplOnly"
	GetPureName(szPrinter1, pPrinterName, MAX_PATH, RTPR_PURENAME);
	wcscpy(m_pPrinterName, pPrinterName);
	// write langugae ID in local machine
	// if Default Langague ID is mentioned in common.dat file, write the same language ID in Local Machine.
	// otherwise, initialize with zero(English)
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");

	CShIniFile cshLangFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	DWORD dwDefLangID = (DWORD)cshLangFile.GetDefaultLangID();
	CRegistryAPI cRegAPI;
	cRegAPI.SetInstallationLangID(szPrinter1, dwDefLangID);

	// Assumption: pOEMDevmode is large enough to contain OEMDEV structure.
// 
	pOemDevMode->dmOEMExtra.dwSize = sizeof(OEMDEV);
	pOemDevMode->dmOEMExtra.dwSignature = OEM_SIGNATURE;
	pOemDevMode->dmOEMExtra.dwVersion = OEM_VERSION;
	LPSCDM lpScPrintInfo = (LPSCDM)&pOemDevMode->scpi;

	if (NULL != lpScPrintInfo)
	{
		FillSCDMHeader(lpScPrintInfo);
		FillPubModeData(&(lpScPrintInfo->pub), szPrinter1);
		FillExtDModeData(&(lpScPrintInfo->ext));
		lpScPrintInfo->ext.wFieldEDM &= ~DM_PRINTPOSITION;
		lpScPrintInfo->ext.wFieldEDM &= ~DM_PRINTPOSITION_CHG;
		FillDocDModeData(&(lpScPrintInfo->doc));
		FillDevDModeData(&(lpScPrintInfo->dev));
		// update Ext2 mode with defaults value
		FillExt2ModeData(&(lpScPrintInfo->ext2));
		// fill water mark structure with default values 
		// default is "None" and read "None" values from wm62.dat file		
		FillWaterMarkDefaultsInSCDMExt2(&(lpScPrintInfo->ext2), &(lpScPrintInfo->ext), dwDefLangID);
		FillDevmodeWithDefaultValues(lpScPrintInfo, DefDMInfo, PaperInfo);
		//m_pOemPublicDevMode->dmCollate = lpScPrintInfo->pub.dmCollate;
		bResult = TRUE;
	}
	return bResult;
}
//=============================================================================
// function
//    FillWaterMarkDefaultsInSCDMExt2
//
// Parameter
//		P_EXT2DMODE pExt2DMode
//		P_EXTDMODE pExtDMode
//		DWORD dwDefLangID)
//
// return value
//     NILL
//
// outline
//    Fill  the Watermark Default data from Wm62.dat File to Ext2mode
//=============================================================================
VOID CShCmbUserset ::FillWaterMarkDefaultsInSCDMExt2(P_EXT2DMODE pExt2DMode, P_EXTDMODE pExtDMode, DWORD dwDefLangID)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (NULL == pExt2DMode)
	{
		return;
	}
	SHORT sWaterMarkIndex = pExtDMode->nWatermark;

	TCHAR szWaterMarkDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szWaterMarkDatFilePath, L"wm62.dat");

	CShIniFile* pIniFile = new CShIniFile(ghInstance, m_pPrinterName, szWaterMarkDatFilePath, FALSE);

	if (NULL == pIniFile)
	{
		slog.putLog("COemUI2::FillWaterMarkDefaultsInSCDMExt2 -> NULL_POINTER(pIniFile)\n");
		return;
	}
	CWaterMarkFileData watermark(m_hStringResourceHandle, pIniFile, dwDefLangID);
	watermark.GetWMDefData(sWaterMarkIndex, &(pExt2DMode->Watermark.data));

	delete pIniFile;
	pIniFile = NULL;
}
long CShCmbUserset::ResetGPDFeatureOptionList(WCHAR FAR *pszSvrPrnName, PGPDFEATUREOPTIONLIST pFeatureOptionList, long lIndex)
{
	CGPDData gpddata(m_pPrintCoreHelperUni);
	m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();

	CHAR			buffer[MAXLENTHFEATUREOPTION] = {};
	if (pszSvrPrnName == NULL)
	{
		return FALSE;
	}
	//- FEATURE_PAPERSIZE  0
	if (DMPAPER_A4 == GetPaperSizeDef())
	{
		m_DefDMInfo[FEATURE_PAPERSIZE].pszDefaultOptionString = L"A4";
	}
	else
	{
		m_DefDMInfo[FEATURE_PAPERSIZE].pszDefaultOptionString = L"LETTER";
	}
	wcscpy_s(pFeatureOptionList->PaperSize, m_DefDMInfo[FEATURE_PAPERSIZE].pszDefaultOptionString);

	//- FEATURE_OREINTATION  1
	wcscpy_s(pFeatureOptionList->PageShOrientation, m_DefDMInfo[FEATURE_OREINTATION].pszDefaultOptionString);

	//- FEATURE_PAGESPERSHEET 
	wcscpy_s(pFeatureOptionList->PagesPerSheet, m_DefDMInfo[FEATURE_PAGESPERSHEET].pszDefaultOptionString);

	//- FEATURE_ORDER	3
	wcscpy_s(pFeatureOptionList->Order, m_DefDMInfo[FEATURE_ORDER].pszDefaultOptionString);

	//- FEATURE_STAPLING	4
	wcscpy_s(pFeatureOptionList->StapleOption, m_DefDMInfo[FEATURE_STAPLING].pszDefaultOptionString);

	//- FEATURE_PUNCH	5
	wcscpy_s(pFeatureOptionList->Punch, m_DefDMInfo[FEATURE_PUNCH].pszDefaultOptionString);

	//- FEATURE_COLLATE	6
	wcscpy_s(pFeatureOptionList->Collate, m_DefDMInfo[FEATURE_COLLATE].pszDefaultOptionString);

	//- FEATURE_BORDER	7
	wcscpy_s(pFeatureOptionList->Border, m_DefDMInfo[FEATURE_BORDER].pszDefaultOptionString);

	//- FEATURE_INPUTBIN	8
	wcscpy_s(pFeatureOptionList->InputBin, m_DefDMInfo[FEATURE_INPUTBIN].pszDefaultOptionString);

	//- FEATURE_MEDIATYPE	9
	wcscpy_s(pFeatureOptionList->MediaType, m_DefDMInfo[FEATURE_MEDIATYPE].pszDefaultOptionString);

	//- FEATURE_BINDINGEDGE	10	
	wcscpy_s(pFeatureOptionList->BindingEdge, m_DefDMInfo[FEATURE_BINDINGEDGE].pszDefaultOptionString);

	//- FEATURE_DUPLEX	11
	wcscpy_s(pFeatureOptionList->Duplex, m_DefDMInfo[FEATURE_DUPLEX].pszDefaultOptionString);

	//- FEATURE_PAMPHLET	12
	wcscpy_s(pFeatureOptionList->Pamphlet, m_DefDMInfo[FEATURE_PAMPHLET].pszDefaultOptionString);

	//- FEATURE_ZOOMSETTINGS	13
	wcscpy_s(pFeatureOptionList->ZoomSettings, m_DefDMInfo[FEATURE_ZOOMSETTINGS].pszDefaultOptionString);

	//- FEATURE_FITTOPAPERSIZE	14
	wcscpy_s(pFeatureOptionList->FitToPaperSize, m_DefDMInfo[FEATURE_FITTOPAPERSIZE].pszDefaultOptionString);

	//- FEATURE_BWPRINT	15
	wcscpy_s(pFeatureOptionList->BlackAndWhitePrint, m_DefDMInfo[FEATURE_BWPRINT].pszDefaultOptionString);

	//- FEATURE_ROTATE180	16
	wcscpy_s(pFeatureOptionList->Rotate180D, m_DefDMInfo[FEATURE_ROTATE180].pszDefaultOptionString);

	//- FEATURE_RETENTION	17
	wcscpy_s(pFeatureOptionList->Retention, m_DefDMInfo[FEATURE_RETENTION].pszDefaultOptionString);

	//- FEATURE_RETENTION_SETTINGS	18	
	wcscpy_s(pFeatureOptionList->RetentionSettings, m_DefDMInfo[FEATURE_RETENTION_SETTINGS].pszDefaultOptionString);

	//- FEATURE_RETENTION_PASSWORD	19
	wcscpy_s(pFeatureOptionList->RetentionPassWord, m_DefDMInfo[FEATURE_RETENTION_PASSWORD].pszDefaultOptionString);

	//- FEATURE_DOCUMENT_FILING_SETTINGS 20
	wcscpy_s(pFeatureOptionList->DocumentFilingSettings, m_DefDMInfo[FEATURE_DOCUMENT_FILING_SETTINGS].pszDefaultOptionString);

	//- FEATURE_USER_AUTHENTICATION_LOGIN_NAME		21
	wcscpy_s(pFeatureOptionList->UserAuthenticationLogInName, m_DefDMInfo[FEATURE_USER_AUTHENTICATION_LOGIN_NAME].pszDefaultOptionString);

	//- FEATURE_USER_AUTHENTICATION_PASSWORD	22
	wcscpy_s(pFeatureOptionList->UserAuthenticationPassWord, m_DefDMInfo[FEATURE_USER_AUTHENTICATION_PASSWORD].pszDefaultOptionString);

	//- FEATURE_USER_NUMBER	23	
	wcscpy_s(pFeatureOptionList->UserNumber, m_DefDMInfo[FEATURE_USER_NUMBER].pszDefaultOptionString);

	//- FEATURE_DEFAULT_JOB_USER_NAME		24
	wcscpy_s(pFeatureOptionList->DefaultJobUserName, m_DefDMInfo[FEATURE_DEFAULT_JOB_USER_NAME].pszDefaultOptionString);

	//- FEATURE_DEFAULT_JOB_JOB_NAME	25
	wcscpy_s(pFeatureOptionList->DefaultJobJobName, m_DefDMInfo[FEATURE_DEFAULT_JOB_JOB_NAME].pszDefaultOptionString);

	//- FEATURE_AUTO_JOB_CONTROL_REVIEW	26
	wcscpy_s(pFeatureOptionList->AutoJobControlReview, m_DefDMInfo[FEATURE_AUTO_JOB_CONTROL_REVIEW].pszDefaultOptionString);

	//- FEATURE_COLOR_MODE		27
	wcscpy_s(pFeatureOptionList->SharpColorMode, m_DefDMInfo[FEATURE_COLOR_MODE].pszDefaultOptionString);

	//- FEATURE_DEFAULTUNITS	28
	// this feature Should always set userdefault option in registry because ,Constraints related some values has updating in
	// createpropertypages() function in interface.cpp file.
	m_DefDMInfo[FEATURE_DEFAULTUNITS].pszDefaultOptionString = L"UserDefaults";
	wcscpy_s(pFeatureOptionList->DefaultUnits, m_DefDMInfo[FEATURE_DEFAULTUNITS].pszDefaultOptionString);

	//- FEATURE_CUSTOMPAPERSIZE		29
	wcscpy_s(pFeatureOptionList->CustomPaperSize, m_DefDMInfo[FEATURE_CUSTOMPAPERSIZE].pszDefaultOptionString);

	//- FEATURE_OFFSET		30
	wcscpy_s(pFeatureOptionList->Offset, m_DefDMInfo[FEATURE_OFFSET].pszDefaultOptionString);

	//- FEATURE_BOOKLET		31
	wcscpy_s(pFeatureOptionList->Booklet, m_DefDMInfo[FEATURE_BOOKLET].pszDefaultOptionString);

	//- FEATURE_STAMP		32
	wcscpy_s(pFeatureOptionList->Stamp, m_DefDMInfo[FEATURE_STAMP].pszDefaultOptionString);

	//- FEATURE_HUNDRED_PERCENT_NUP		33
	wcscpy_s(pFeatureOptionList->NUp100, m_DefDMInfo[FEATURE_HUNDRED_PERCENT_NUP].pszDefaultOptionString);

	//- FEATURE_AUTHENTICATION		34
	wcscpy_s(pFeatureOptionList->Authentication, m_DefDMInfo[FEATURE_AUTHENTICATION].pszDefaultOptionString);

	//- FEATURE_PRINTRELEASE		35
	wcscpy_s(pFeatureOptionList->PrintRelease, m_DefDMInfo[FEATURE_PRINTRELEASE].pszDefaultOptionString);

	//- FEATURE_ZOOM_BASING		36
	wcscpy_s(pFeatureOptionList->ZoomBasing, m_DefDMInfo[FEATURE_ZOOM_BASING].pszDefaultOptionString);

	//- FEATURE_INPUTRESOLUTION		37
	wcscpy_s(pFeatureOptionList->InputResolution, m_DefDMInfo[FEATURE_INPUTRESOLUTION].pszDefaultOptionString);

	//- FEATURE_SHADEPATTERN		38
	wcscpy_s(pFeatureOptionList->ShadePattern, m_DefDMInfo[FEATURE_SHADEPATTERN].pszDefaultOptionString);

	//- FEATURE_SPOOLDATAFORMAT		39
	wcscpy_s(pFeatureOptionList->SpoolDataFormat, m_DefDMInfo[FEATURE_SPOOLDATAFORMAT].pszDefaultOptionString);

	//- FEATURE_PRINTABLEAREA		40
	wcscpy_s(pFeatureOptionList->PrintableArea, m_DefDMInfo[FEATURE_PRINTABLEAREA].pszDefaultOptionString);

	//- FEATURE_DUPLEXSTYLE	41	
	wcscpy_s(pFeatureOptionList->DuplexStyle, m_DefDMInfo[FEATURE_DUPLEXSTYLE].pszDefaultOptionString);

	//- FEATURE_DEVICECOLORMODE		42
	wcscpy_s(pFeatureOptionList->DeviceColorMode, m_DefDMInfo[FEATURE_DEVICECOLORMODE].pszDefaultOptionString);

	//- FEATURE_DEVICESTAPLEMODULE		43
	wcscpy_s(pFeatureOptionList->DeviceStapleModule, m_DefDMInfo[FEATURE_DEVICESTAPLEMODULE].pszDefaultOptionString);

	//- FEATURE_STAPLELESS		44
	wcscpy_s(pFeatureOptionList->Stapleless, m_DefDMInfo[FEATURE_STAPLELESS].pszDefaultOptionString);

	//- FEATURE_DEVICEPUNCHMODULE		45
	wcscpy_s(pFeatureOptionList->DevicePunchModule, m_DefDMInfo[FEATURE_DEVICEPUNCHMODULE].pszDefaultOptionString);

	//- FEATURE_MAXPAPERSIZE		46
	wcscpy_s(pFeatureOptionList->MaxPaperSize, m_DefDMInfo[FEATURE_MAXPAPERSIZE].pszDefaultOptionString);

	//- FEATURE_SINGLESIGNON		47
	wcscpy_s(pFeatureOptionList->SingleSignOn, m_DefDMInfo[FEATURE_SINGLESIGNON].pszDefaultOptionString);

	//- FEATURE_AUTOCOLORMODE		48
	wcscpy_s(pFeatureOptionList->AutoColorMode, m_DefDMInfo[FEATURE_AUTOCOLORMODE].pszDefaultOptionString);

	//- FEATURE_PINCOMPATIBILITY		49
	wcscpy_s(pFeatureOptionList->PinCompatibility, m_DefDMInfo[FEATURE_PINCOMPATIBILITY].pszDefaultOptionString);

	//- FEATURE_DEVICEPRINTRELEASE		50
	wcscpy_s(pFeatureOptionList->DevicePrintRelease, m_DefDMInfo[FEATURE_DEVICEPRINTRELEASE].pszDefaultOptionString);

	//- FEATURE_JOBNAMESHORTENING	51
	wcscpy_s(pFeatureOptionList->JobNameShortening, m_DefDMInfo[FEATURE_JOBNAMESHORTENING].pszDefaultOptionString);

	//- FEATURE_USEDRIVERTORENDERJPEG		52
	wcscpy_s(pFeatureOptionList->UseDrivertoRenderJPEG, m_DefDMInfo[FEATURE_USEDRIVERTORENDERJPEG].pszDefaultOptionString);

	//- FEATURE_MINIMUMLINEWIDTH		53
	wcscpy_s(pFeatureOptionList->MinimumLineWidth, m_DefDMInfo[FEATURE_MINIMUMLINEWIDTH].pszDefaultOptionString);

	//- FEATURE_CUSTOMPAPERUNITS		54
	wcscpy_s(pFeatureOptionList->CustomPaperUnits, m_DefDMInfo[FEATURE_CUSTOMPAPERUNITS].pszDefaultOptionString);

	//- FEATURE_PRINTPOSITION		55
	wcscpy_s(pFeatureOptionList->PrintPosition, m_DefDMInfo[FEATURE_PRINTPOSITION].pszDefaultOptionString);

	//- FEATURE_PRINT_POSITION_UNIT		56
	if (unit_mm == GetUnitDef())
	{
		wcscpy_s(pFeatureOptionList->PrintPositionUnit, L"Millimeters");
	}
	else
	{
		wcscpy_s(pFeatureOptionList->PrintPositionUnit, m_DefDMInfo[FEATURE_PRINT_POSITION_UNIT].pszDefaultOptionString);
	}


	//- FEATURE_MARGINSHIFT		57
	wcscpy_s(pFeatureOptionList->MarginShift, m_DefDMInfo[FEATURE_MARGINSHIFT].pszDefaultOptionString);

	//- FEATURE_MARGIN_SHIFT_UNIT		58
	if (unit_mm == GetUnitDef())
	{
		wcscpy_s(pFeatureOptionList->MarginShiftUnit, L"Millimeters");
	}
	else
	{
		wcscpy_s(pFeatureOptionList->MarginShiftUnit, m_DefDMInfo[FEATURE_MARGIN_SHIFT_UNIT].pszDefaultOptionString);
	}

	//- FEATURE_BOOKLET_OUTPUTSIZE		59
	if (DMPAPER_A4 == GetPaperSizeDef())
	{
		wcscpy_s(pFeatureOptionList->BookletOutputSize, L"A3");
	}
	else
	{
		wcscpy_s(pFeatureOptionList->BookletOutputSize, m_DefDMInfo[FEATURE_BOOKLET_OUTPUTSIZE].pszDefaultOptionString);
	}

	//- FEATURE_WM_FONT_COLOR		60
	wcscpy_s(pFeatureOptionList->FontColor, m_DefDMInfo[FEATURE_WM_FONT_COLOR].pszDefaultOptionString);

	//- FEATURE_WM_BORDER		61	
	wcscpy_s(pFeatureOptionList->WMBorder, m_DefDMInfo[FEATURE_WM_BORDER].pszDefaultOptionString);

	//- FEATURE_WM_PRINTING_PATTERN		62
	wcscpy_s(pFeatureOptionList->PrintingPattern, m_DefDMInfo[FEATURE_WM_PRINTING_PATTERN].pszDefaultOptionString);

	//- FEATURE_BOLD		63
	wcscpy_s(pFeatureOptionList->Bold, m_DefDMInfo[FEATURE_BOLD].pszDefaultOptionString);

	//- FEATURE_ITALIC		64
	wcscpy_s(pFeatureOptionList->Italic, m_DefDMInfo[FEATURE_ITALIC].pszDefaultOptionString);

	//- FEATURE_PPLCYPRINTRELEASE		65
	wcscpy_s(pFeatureOptionList->PPLCYPrintRelease, m_DefDMInfo[FEATURE_PPLCYPRINTRELEASE].pszDefaultOptionString);

	//- FEATURE_PPLCYRETENTION		66
	wcscpy_s(pFeatureOptionList->PPLCYRetention, m_DefDMInfo[FEATURE_PPLCYRETENTION].pszDefaultOptionString);

	//- FEATURE_PPLCYUSERAUTHENTICATION		67
	wcscpy_s(pFeatureOptionList->PPLCYUserAuthentication, m_DefDMInfo[FEATURE_PPLCYUSERAUTHENTICATION].pszDefaultOptionString);

	//- FEATURE_PPLCYWINLOGIN		68
	wcscpy_s(pFeatureOptionList->PPLCYWinLogin, m_DefDMInfo[FEATURE_PPLCYWINLOGIN].pszDefaultOptionString);

	//- FEATURE_PPLCYLOGINNAME		69
	wcscpy_s(pFeatureOptionList->PPLCYLoginName, m_DefDMInfo[FEATURE_PPLCYLOGINNAME].pszDefaultOptionString);

	//- FEATURE_ALWAYS_USE_UNAME		70
	wcscpy_s(pFeatureOptionList->AlwaysUsethisUserName, m_DefDMInfo[FEATURE_ALWAYS_USE_UNAME].pszDefaultOptionString);

	//- FEATURE_ALWAYS_USE_JNAME		71
	wcscpy_s(pFeatureOptionList->AlwaysUsethisJobName, m_DefDMInfo[FEATURE_ALWAYS_USE_JNAME].pszDefaultOptionString);
	
	//<S><A>To Implement Task#3132,31-05-2024,SSDI:Manoj S
	//- FEATURE_OVERIDE_PPLCYPRINTRELEASE		72
	wcscpy_s(pFeatureOptionList->OveridePPLCYPrintRelease, m_DefDMInfo[FEATURE_OVERIDE_PPLCYPRINTRELEASE].pszDefaultOptionString);

	//- FEATURE_OVERIDE_PPLCYRETENTION		73
	wcscpy_s(pFeatureOptionList->OveridePPLCYRetention, m_DefDMInfo[FEATURE_OVERIDE_PPLCYRETENTION].pszDefaultOptionString);
	//<E>To Implement Task #3132,31-05-2024,SSDI:Manoj S

	//- FEATURE_SHAREFAVORITES		74
	wcscpy_s(pFeatureOptionList->ShareFavorites, m_DefDMInfo[FEATURE_SHAREFAVORITES].pszDefaultOptionString);
	
	return TRUE;
}
VOID CShCmbUserset::FillDevmodeWithDefaultValues(LPSCDM lpScPrintInfo, std::vector<DEFAULTDEVMODEINFO> DefDMInfo, PaperInfoMap PaperInfo)
{
	
	CGPDData gpddata(m_pPrintCoreHelperUni);
	m_DefDMInfo = gpddata.GetdefaultDevmodeInfo();
	size_t len = m_DefDMInfo.size();
	//<S><C>To Fix Feedback 23,2021-02-24,SSDI:Chanchal Singla
	for (size_t i = 0; i < len; i++)
	{
		if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PaperSize"))
		{
			lpScPrintInfo->pub.dmPaperSize = (SHORT)m_DefDMInfo[i].iDevmodeValue;
			StringCbCopy(lpScPrintInfo->pub.dmFormName, sizeof(lpScPrintInfo->pub.dmFormName), m_DefDMInfo[i].pszDefaultOptionString);
			PaperInfoMap::const_iterator paperinfoiterator;
			paperinfoiterator = PaperInfo.find(m_DefDMInfo[i].pszDefaultOptionString);
			if (paperinfoiterator != PaperInfo.end())
			{
				lpScPrintInfo->pub.dmPaperWidth = (SHORT)paperinfoiterator->second.PaperWidth;
				lpScPrintInfo->pub.dmPaperLength = (SHORT)paperinfoiterator->second.PaperLength;
				lpScPrintInfo->ext.nDstWidthMilli = lpScPrintInfo->pub.dmPaperWidth;
				lpScPrintInfo->ext.nDstHeightMilli = lpScPrintInfo->pub.dmPaperLength;
			}
		}
		//<S><C> to implement Custom Orientation 13/8/2020 K M Harika
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PageShOrientation"))
			//<E> to implement Custom Orientation 13/8/2020 K M Harika
		{
			if (m_DefDMInfo[i].iDevmodeValue == ori_port)
			{
				lpScPrintInfo->pub.dmOrientation = DMORIENT_PORTRAIT;
				lpScPrintInfo->ext.dwFieldEDM1 &= ~DM_ROTATE180DEG;
				lpScPrintInfo->ext.dwFieldEDM2 &= ~DM_ROTATE180DEG_HW;
			}
			else if (m_DefDMInfo[i].iDevmodeValue == ori_land)
			{
				lpScPrintInfo->pub.dmOrientation = DMORIENT_LANDSCAPE;
				lpScPrintInfo->ext.dwFieldEDM1 &= ~DM_ROTATE180DEG;
				lpScPrintInfo->ext.dwFieldEDM2 &= ~DM_ROTATE180DEG_HW;
			}
			else if (m_DefDMInfo[i].iDevmodeValue == ori_port_rotated)
			{
				lpScPrintInfo->pub.dmOrientation = DMORIENT_PORTRAIT;
				lpScPrintInfo->ext.dwFieldEDM1 |= DM_ROTATE180DEG;
				lpScPrintInfo->ext.dwFieldEDM2 |= DM_ROTATE180DEG_HW;
			}
			else if (m_DefDMInfo[i].iDevmodeValue == ori_land_rotated)
			{
				lpScPrintInfo->pub.dmOrientation = DMORIENT_LANDSCAPE;
				lpScPrintInfo->ext.dwFieldEDM1 |= DM_ROTATE180DEG;
				lpScPrintInfo->ext.dwFieldEDM2 |= DM_ROTATE180DEG_HW;
			}
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PagesPerSheet"))
		{
			UINT NupValue = (UINT)m_DefDMInfo[i].iDevmodeValue;
			switch (NupValue)
			{
			case 1:
				lpScPrintInfo->ext.byNupX = 1;
				lpScPrintInfo->ext.byNupY = 1;
				break;
			case 2:
				lpScPrintInfo->ext.byNupX = 2;
				lpScPrintInfo->ext.byNupY = 1;
				break;

			case 4:
				lpScPrintInfo->ext.byNupX = 2;
				lpScPrintInfo->ext.byNupY = 2;
				break;

			case 6:
				lpScPrintInfo->ext.byNupX = 3;
				lpScPrintInfo->ext.byNupY = 2;
				break;

			case 8:
				lpScPrintInfo->ext.byNupX = 4;
				lpScPrintInfo->ext.byNupY = 2;
				break;

			case 9:
				lpScPrintInfo->ext.byNupX = 3;
				lpScPrintInfo->ext.byNupY = 3;
				break;

			case 16:
				lpScPrintInfo->ext.byNupX = 4;
				lpScPrintInfo->ext.byNupY = 4;
				break;

			default:
				break; //Some problem

			}

			if (DMORIENT_LANDSCAPE == lpScPrintInfo->pub.dmOrientation)
			{
				BYTE Temp = lpScPrintInfo->ext.byNupX;
				lpScPrintInfo->ext.byNupX = lpScPrintInfo->ext.byNupY;
				lpScPrintInfo->ext.byNupY = Temp;
			}
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"Order"))
		{
			lpScPrintInfo->ext.byNupOrder = (BYTE)m_DefDMInfo[i].iDevmodeValue;

			if (2 == lpScPrintInfo->ext.byNupX * lpScPrintInfo->ext.byNupY)
			{
				lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_Z;

				if (m_DefDMInfo[i].iDevmodeValue == nup_order_ltor)
				{
					lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_Z;
				}
				else if (m_DefDMInfo[i].iDevmodeValue == nup_order_rtol)
				{
					lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_RZ;
				}
				else if (m_DefDMInfo[i].iDevmodeValue == nup_order_ttob)
				{
					lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_Z;
				}
				if (DMORIENT_LANDSCAPE == lpScPrintInfo->pub.dmOrientation)
				{
					lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_Z;
				}
			}
			else
			{
				lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_Z;

				if (m_DefDMInfo[i].iDevmodeValue == nup_order_randd)
				{
					lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_Z;
				}
				else if (m_DefDMInfo[i].iDevmodeValue == nup_order_dandr)
				{
					lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_RN;
				}
				else if (m_DefDMInfo[i].iDevmodeValue == nup_order_landd)
				{
					lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_RZ;
				}
				else if (m_DefDMInfo[i].iDevmodeValue == nup_order_dandl)
				{
					lpScPrintInfo->ext.byNupOrder = DM_NUP_ORDER_N;
				}
			}
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"StapleOption"))
		{
			lpScPrintInfo->ext.byStaple = (BYTE)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"Punch"))
		{
			lpScPrintInfo->ext.byPunchType = (BYTE)m_DefDMInfo[i].iDevmodeValue;
			lpScPrintInfo->ext.dwFieldEDM1 = (m_DefDMInfo[i].iDevmodeValue == bool_false) ?
				lpScPrintInfo->ext.dwFieldEDM1 & ~DM_PUNCH : lpScPrintInfo->ext.dwFieldEDM1 | DM_PUNCH;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"Collate"))
		{
			lpScPrintInfo->pub.dmCollate = (SHORT)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"InputBin"))
		{
			lpScPrintInfo->pub.dmDefaultSource = (SHORT)m_DefDMInfo[i].iDevmodeValue;
			lpScPrintInfo->ext.dmDefaultSourceSave = (SHORT)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"MediaType"))
		{
			lpScPrintInfo->pub.dmMediaType = m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"BindingEdge"))
		{
			lpScPrintInfo->ext.byBindedge = (BYTE)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"MarginShift"))
		{
			lpScPrintInfo->ext.byMgnShift = (BYTE)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"SharpColorMode"))
		{
			BYTE byColMode = (BYTE)m_DefDMInfo[i].iDevmodeValue;
			if (colmode_auto == byColMode)
			{
				lpScPrintInfo->ext.ColData.byColMode = colmode_auto;
				lpScPrintInfo->pub.dmColor = DMCOLOR_COLOR;
				lpScPrintInfo->pub.dmICMMethod = DMICMMETHOD_DEVICE;
			}
			else if (colmode_color == byColMode)
			{
				lpScPrintInfo->ext.ColData.byColMode = colmode_color;
				lpScPrintInfo->pub.dmColor = DMCOLOR_COLOR;
				lpScPrintInfo->pub.dmICMMethod = DMICMMETHOD_DEVICE;

			}
			else if (colmode_mono == byColMode)
			{
				lpScPrintInfo->ext.ColData.byColMode = colmode_mono;
				lpScPrintInfo->pub.dmColor = DMCOLOR_MONOCHROME;
				lpScPrintInfo->pub.dmICMMethod = DMICMMETHOD_NONE;
			}
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"ZoomBasing"))
		{
			lpScPrintInfo->doc.dwFieldDDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->doc.dwFieldDDM | DM_ZOOM_CENTER) : (lpScPrintInfo->doc.dwFieldDDM & ~DM_ZOOM_CENTER);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"InputResolution"))
		{
			lpScPrintInfo->ext.byResolution = (BYTE)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"ShadePattern"))
		{
			lpScPrintInfo->doc.dwFieldDDM = (m_DefDMInfo[i].iDevmodeValue == bool_false) ?
				(lpScPrintInfo->doc.dwFieldDDM | DM_WYSIWYG) : (lpScPrintInfo->doc.dwFieldDDM & ~DM_WYSIWYG);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"SpoolDataFormat"))
		{
			lpScPrintInfo->doc.dwFieldDDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->doc.dwFieldDDM | DM_EMFJOURNAL) : (lpScPrintInfo->doc.dwFieldDDM & ~DM_EMFJOURNAL);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PrintableArea"))
		{
			lpScPrintInfo->ext.dwFieldEDM2 = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->ext.dwFieldEDM2 | DM_EDGETOEDGE) : (lpScPrintInfo->ext.dwFieldEDM2 & ~DM_EDGETOEDGE);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"DuplexStyle"))
		{
			lpScPrintInfo->ext.PCFax.byDuplexStyle = (BYTE)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"JobNameShortening"))
		{
			lpScPrintInfo->ext.dwFieldEDM2 = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->ext.dwFieldEDM2 | DM_SHORT_JOBNAME) : (lpScPrintInfo->ext.dwFieldEDM2 & ~DM_SHORT_JOBNAME);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"UseDrivertoRenderJPEG"))
		{
			lpScPrintInfo->doc.dwFieldDDM = (m_DefDMInfo[i].iDevmodeValue == bool_false) ?
				(lpScPrintInfo->doc.dwFieldDDM | DM_JPEG_PASSTHROUGH) : (lpScPrintInfo->doc.dwFieldDDM & ~DM_JPEG_PASSTHROUGH);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"MinimumLineWidth"))
		{
			lpScPrintInfo->doc.wPenWidthRate = (BYTE)(m_DefDMInfo[i].iDevmodeValue == bool_false) ? WPENLINEWIDTH : bool_false;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"DeviceColorMode"))
		{
			lpScPrintInfo->dev.byFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_false) ?
				(lpScPrintInfo->dev.byFieldVDM | DM_VDM_COLORMODEL) : (lpScPrintInfo->dev.byFieldVDM & ~DM_VDM_COLORMODEL);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"DeviceStapleModule"))
		{
			if (m_DefDMInfo[i].iDevmodeValue == stapleopt_3)
			{
				lpScPrintInfo->dev.byFieldVDMStaple = DM_VDMS_SADDLE;
			}
			else if (m_DefDMInfo[i].iDevmodeValue == stapleopt_2)
			{
				lpScPrintInfo->dev.byFieldVDMStaple = DM_VDMS_2STAPLE;
			}
			else if (m_DefDMInfo[i].iDevmodeValue == stapleopt_1)
			{
				lpScPrintInfo->dev.byFieldVDMStaple = DM_VDMS_1STAPLE;
			}
			else
			{
				lpScPrintInfo->dev.byFieldVDMStaple = stapleopt_none;
			}
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"Stapleless"))
		{
			lpScPrintInfo->dev.byFieldVDMStaple = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->dev.byFieldVDMStaple | DM_VDMS_STAPLELESS) : (lpScPrintInfo->dev.byFieldVDMStaple & ~DM_VDMS_STAPLELESS);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"DevicePunchModule"))
		{
			lpScPrintInfo->dev.byPunchmodule = (BYTE)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"DevicePrintRelease"))
		{
			lpScPrintInfo->dev.byFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->dev.byFieldVDM | DM_VDM_NON_PRITNRLEASE) : (lpScPrintInfo->dev.byFieldVDM & ~DM_VDM_NON_PRITNRLEASE);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"MaxPaperSize"))
		{
			lpScPrintInfo->dev.byMaxPaperSize = (BYTE)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"SingleSignOn"))
		{
			lpScPrintInfo->dev.dwFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->dev.dwFieldVDM | DM_SINGLESIGNON) : (lpScPrintInfo->dev.dwFieldVDM & ~DM_SINGLESIGNON);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"AutoColorMode"))
		{
			lpScPrintInfo->dev.dwFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_false) ?
				(lpScPrintInfo->dev.dwFieldVDM | DM_VDM_COLORMODE_AUTO_DISABLE) : (lpScPrintInfo->dev.dwFieldVDM & ~DM_VDM_COLORMODE_AUTO_DISABLE);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PinCompatibility"))
		{
			lpScPrintInfo->dev.dwFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->dev.dwFieldVDM | DM_VDM_PIN_CLEAN_TEXT) : (lpScPrintInfo->dev.dwFieldVDM & ~DM_VDM_PIN_CLEAN_TEXT);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PPLCYPrintRelease"))
		{
			lpScPrintInfo->dev.dwFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->dev.dwFieldVDM | DM_PP_PRINTRELEASE) : (lpScPrintInfo->dev.dwFieldVDM & ~DM_PP_PRINTRELEASE);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PPLCYRetention"))
		{
			lpScPrintInfo->dev.dwFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->dev.dwFieldVDM | DM_PP_RETENTION) : (lpScPrintInfo->dev.dwFieldVDM & ~DM_PP_RETENTION);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PPLCYUserAuthentication"))
		{
			lpScPrintInfo->dev.dwFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->dev.dwFieldVDM | DM_PP_USERAUTH) : (lpScPrintInfo->dev.dwFieldVDM & ~DM_PP_USERAUTH);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PPLCYWinLogin"))
		{
			lpScPrintInfo->dev.dwFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->dev.dwFieldVDM | DM_PP_WINLOGIN_AS_LOGINN) : (lpScPrintInfo->dev.dwFieldVDM & ~DM_PP_WINLOGIN_AS_LOGINN);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PPLCYLoginName"))
		{
			lpScPrintInfo->dev.dwFieldVDM = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->dev.dwFieldVDM | DM_PP_LOGINN_AS_USERN) : (lpScPrintInfo->dev.dwFieldVDM & ~DM_PP_LOGINN_AS_USERN);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"PrintRelease"))
		{
			lpScPrintInfo->ext.JobCtrl.wFieldJC = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->ext.JobCtrl.wFieldJC | DM_PRINTSHARE) : (lpScPrintInfo->ext.JobCtrl.wFieldJC & ~DM_PRINTSHARE);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"DocumentFilingSettings"))
		{
			lpScPrintInfo->ext.JobCtrl.byDocumentFiling = (BYTE)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"RetentionSettings"))
		{
			lpScPrintInfo->ext.JobCtrl.byRetention = (BYTE)m_DefDMInfo[i].iDevmodeValue;
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"Authentication"))
		{
			if (m_DefDMInfo[i].iDevmodeValue == authentication_none)
			{
				lpScPrintInfo->ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
				lpScPrintInfo->ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;
				lpScPrintInfo->ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
			}
			else if (m_DefDMInfo[i].iDevmodeValue == authentication_lname)
			{
				lpScPrintInfo->ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
				lpScPrintInfo->ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
				lpScPrintInfo->ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
			}
			else if (m_DefDMInfo[i].iDevmodeValue == authentication_lname_pswd)
			{
				lpScPrintInfo->ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;
				lpScPrintInfo->ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
				lpScPrintInfo->ext.JobCtrl.wFieldJC |= DM_USE_LOGINPASS;
			}
			else if (m_DefDMInfo[i].iDevmodeValue == authentication_acnum)
			{
				lpScPrintInfo->ext.JobCtrl.wFieldJC |= DM_ALWAYSUSE_ACNUM;
				lpScPrintInfo->ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;
				lpScPrintInfo->ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
			}
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"DefaultJobUserName"))
		{
			lpScPrintInfo->ext.JobCtrl.wFieldJC = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->ext.JobCtrl.wFieldJC | DM_ALWAYSUSE_USERID) : (lpScPrintInfo->ext.JobCtrl.wFieldJC & ~DM_ALWAYSUSE_USERID);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"DefaultJobJobName"))
		{
			lpScPrintInfo->ext.JobCtrl.wFieldJC = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->ext.JobCtrl.wFieldJC | DM_ALWAYSUSE_JOBID) : (lpScPrintInfo->ext.JobCtrl.wFieldJC & ~DM_ALWAYSUSE_JOBID);
		}
		else if (!lstrcmp(m_DefDMInfo[i].pszFeatureName, L"RetentionPassWord"))
		{
			lpScPrintInfo->ext.JobCtrl.wFieldJC = (m_DefDMInfo[i].iDevmodeValue == bool_true) ?
				(lpScPrintInfo->ext.JobCtrl.wFieldJC | DM_USE_PIN) : (lpScPrintInfo->ext.JobCtrl.wFieldJC & ~DM_USE_PIN);
		}

	}
	//<E>To Fix Feedback 23,2021-02-24,SSDI:Chanchal Singla
}

//=============================================================================
// function
//      chkLimitByOptionTblTab
//
// parameters
//      lVal               
// return value
//      TRUE
//     FALSE
//
// outline
//      chkLimitByOptionTblTab
//		
//=============================================================================
BOOL CShCmbUserset::chkLimitByOptionTblTab(DWORD dwIndex)
{
	BOOL	blRet = FALSE;

	if (m_ppi == NULL) {
		goto EXIT;
	}

	blRet = chkLimitByOptionTblCore((*m_ppi).pblLimitByOptionTbl, (*m_ppi).dwFavoritesDataCount, dwIndex);

EXIT:
	return blRet;
}
//=============================================================================
// function
//      chkLimitByOptionTbl
//
// parameters
//      lVal                
//
// return value
//     : TRUE
//      ê : FALSE
//
// outline
//     chkLimitByOptionTblCore
//=============================================================================
BOOL CShCmbUserset::chkLimitByOptionTblCore(BOOL *pblLimitByOptionTbl, DWORD dwCount, DWORD dwIndex)
{
	BOOL	blRet = FALSE;

	if (pblLimitByOptionTbl == NULL) {
		goto EXIT;
	}

	if (dwIndex >= dwCount) {
		goto EXIT;
	}

	blRet = *(pblLimitByOptionTbl + dwIndex);

EXIT:
	return blRet;
}

//=============================================================================
// function
//      setlimitByOptionTbl
//
// parameters
//      void
//
// return value
//     TRUE
//      FALSE
//
// outline
//      setLimitByOptionTbl
//=============================================================================
BOOL CShCmbUserset::setLimitByOptionTbl(void)
{
	BOOL			blRet = FALSE;
	//========================================================================
	//This Flag variable is reserved , please don't used this flag for other purpose and 
	// don't change the value,otherwise driver maybe crashed
	BOOL wFlag = FALSE;
	//========================================================================
	CShRegUS		*preg = NULL;
	CShLimit		*plim = NULL;
	PPROPSTATE		pps = NULL;
	PSCDM			pscdm = NULL;
	DWORD			dwCount = -1;
	DWORD			dwIndex;
	BOOL			*pblLimitByOptionTbl = NULL;
	PFEATUREINFOLIST pFeatureInfoListCurrent = NULL;
	PFEATUREINFOLIST pFeatureInfoList = NULL;
	PSCDM			pscdmCurrent = NULL;
	eDeviceType wDeviceType = eColor;
	short blLimitByOption = FALSE;
	CRegistryAPI cRegApi;
	cRegApi.GetDeviceType(m_pPrinterName, wDeviceType);
	BOOL bIsMonoModel = (MONO_MODEL_ID == m_pOemPrivateDevMode->scpi.wFieldSPI) ? TRUE : FALSE;
	size_t nNumberOfFeatures = 0;
	/*Get the GPD feature Count*/
	nNumberOfFeatures = GetGPDFeatureCount();
	BOOL			blFlg = FALSE;
	if (m_ppi == NULL) {
		goto EXIT;
	}

	if ((*m_ppi).pblLimitByOptionTbl != NULL) {
		blRet = TRUE;
		goto EXIT;
	}


	preg = new CShRegUS(ghInstance, m_pPrinterName, m_hStringResourceHandle, NULL, m_pFeatureInfoList, m_pOemPrivateDevMode);
	if (preg == NULL)
	{
		goto EXIT;
	}
	if (*m_pUsIndex == (short)0x7fff) {
		pscdmCurrent = new SCDM;
		if (pscdmCurrent == NULL) {
			goto EXIT;
		}
		(*m_ppi).wUsIndex = -1;
		// Current FeatureInfoList
		pFeatureInfoListCurrent = NULL;
		pFeatureInfoListCurrent = InitFeatureInfoList(pFeatureInfoListCurrent);
		pFeatureInfoListCurrent = UpdateTempFeatureInfoList(pFeatureInfoListCurrent, m_pFeatureInfoList);

		//<S><A>To Fix Dropdwon is not changing to Untitled,2022.10.01,SSDI:Chanchal Singla
		//Bug #4705: Favorites displays untitled when Maximum Paper Size is set to A4 / Letter in Configuration tab,2022.17.01,SSDI:Chanchal Singla
		UpdateBookletFeature(pFeatureInfoListCurrent);
		//<E>To Fix Dropdwon is not changing to Untitled,2022.10.01,SSDI:Chanchal Singla
	// Update Current featurelist
		::memcpy(pscdmCurrent, &m_pOemPrivateDevMode->scpi, sizeof(SCDM));

	}

	if (m_pPrinterName != NULL) {
		
		dwCount = (*preg).ReadCount(m_pPrinterName);
		(*m_ppi).dwFavoritesDataCount = dwCount;

		pblLimitByOptionTbl = new BOOL[dwCount + 1];
		if (pblLimitByOptionTbl == NULL) {
			goto EXIT;
		}

		// 0(FALSE)Ç≈ÉNÉäÉA
		SecureZeroMemory(pblLimitByOptionTbl, (sizeof(BOOL) * (dwCount + 1)));

		pscdm = new SCDM;
	
		if (pscdm == NULL) {
			goto EXIT;
		}
		//pFeatureInfoList
		pFeatureInfoList = NULL;
		pFeatureInfoList = InitFeatureInfoList(pFeatureInfoList);
	


		for (dwIndex = 0; dwIndex < dwCount; dwIndex++) {
			SecureZeroMemory(pscdm, sizeof(SCDM));
			pFeatureInfoList = UpdateFeatureInfoList(pFeatureInfoList, (short)dwIndex);
				
			if  ((*preg).ReadData(m_pPrinterName, pscdm, (short)dwIndex) == 0)
			{
				goto EXIT;
			}
			
			//<S><A>To Fix Redmine Bug #3845,#3783,2021.august.09, SSDI:Chanchal Singla
			//Bug #3845:- Saved favorites are not displayed
			//Bug #3783:- Registered Favorites are not hidden even if the setting value of Staple module is changed in Configuration tab
			blLimitByOption = LimitOptionUpdateforStapleAndPunch(pscdm, pFeatureInfoList);
			if (blLimitByOption != bool_false) {
				*(pblLimitByOptionTbl + dwIndex) = TRUE;
			}
			//<E>To Fix Redmine Bug #3845,#3783,2021.august.09, SSDI:Chanchal Singla

			if (*m_pUsIndex == (short)0x7fff) {
				if (blFlg == FALSE) {

					if (bIsMonoModel)
					{
						if (eColor == wDeviceType)
						{
							(*pscdm).pub.dmColor = DMCOLOR_COLOR;
							(*pscdm).pub.dmICMMethod = DMICMMETHOD_DEVICE;
							(*pscdm).ext.ColData.byColMode = colmode_auto;
							pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = "Auto";
							pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = eAutoMode;
						}
					}
					else
					{
						if (eMono == wDeviceType)
						{
							(*pscdm).pub.dmColor = DMCOLOR_MONOCHROME;
							(*pscdm).pub.dmICMMethod = DMICMMETHOD_NONE;
							(*pscdm).ext.ColData.byColMode = colmode_mono;
							pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = "Mono";
							pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = eMonoMode;
						}
					}
					if (dwIndex == 0) {
						MergeConfigTabDevmodeAndfeatureInfoList(pscdm, pscdmCurrent, pFeatureInfoList, pFeatureInfoListCurrent);
					}
					// ignore the Following features and devmode variables
					SyncIgnoreData(pscdm, pscdmCurrent, pFeatureInfoList, pFeatureInfoListCurrent);

					UINT TempArray[4] = { FEATURE_RETENTION_PASSWORD, FEATURE_AUTHENTICATION , FEATURE_DEFAULT_JOB_USER_NAME ,FEATURE_DEFAULT_JOB_JOB_NAME };
					size_t nFeatureIndex = 0;
					size_t nNumberOfFeatures = 4;

					for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
					{
						pFeatureInfoList->pFeatureOptionPair[TempArray[nFeatureIndex]].pszOption = pFeatureInfoListCurrent->pFeatureOptionPair[TempArray[nFeatureIndex]].pszOption;

						pFeatureInfoList->pFeatureOptionsList[TempArray[nFeatureIndex]].uDefaultOptionIndex = pFeatureInfoListCurrent->pFeatureOptionsList[TempArray[nFeatureIndex]].uDefaultOptionIndex;

						pFeatureInfoList->pFeatureOptionsList[TempArray[nFeatureIndex]].uChangedOptionIndex = pFeatureInfoListCurrent->pFeatureOptionsList[TempArray[nFeatureIndex]].uChangedOptionIndex;
					}

					if (CompareData(pscdm, pscdmCurrent, pFeatureInfoList, pFeatureInfoListCurrent) == 0)
					{
						(*m_ppi).wUsIndex = dwIndex;
						blFlg = TRUE;
					}
				}
			}
		}
	}
	(*m_ppi).pblLimitByOptionTbl = pblLimitByOptionTbl;
	blRet = TRUE;

EXIT:

	if (preg != NULL) {
		delete	preg;
		preg = NULL;
	}

	if (pscdm != NULL) {
		delete	pscdm;
	}

	if (pscdmCurrent != NULL) {
		delete pscdmCurrent;
	}
	if (NULL != pFeatureInfoListCurrent)
	{
		delete pFeatureInfoListCurrent;
		pFeatureInfoListCurrent = NULL;
	}
	//Fixed Coverity #91321,13-08-2021,SSDI:Chanchal Singla
	if (NULL != pFeatureInfoList)
	{
		delete pFeatureInfoList;
		pFeatureInfoList = NULL;
	}
	return blRet;
}
//=============================================================================
// function
//      SyncIgnoreData
//
// parameters
//      pscdm1				
//      pscdm2				SCDM
//
// return value
//      ê: TRUE
//      : FALSE
//
// outline
//		SyncIgnoreData
//=============================================================================
BOOL CShCmbUserset::SyncIgnoreData(PSCDM pscdm1, PSCDM pscdm2, PFEATUREINFOLIST pFeatureInfoList1, PFEATUREINFOLIST pFeatureInfoList2)
{
	BOOL		blRet = FALSE;


	if (pscdm1 == NULL || pscdm2 == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  PUBDMODE
	// ================================================
	// --- reserved members
	((*pscdm1).pub).dmReserved1 = ((*pscdm2).pub).dmReserved1;
	((*pscdm1).pub).dmReserved2 = ((*pscdm2).pub).dmReserved2;
	(*pscdm1).pub.dmFields      =  (*pscdm2).pub.dmFields;
	// ================================================
	// _/_/_/  _DOCDMODE
	// ================================================

#if SCDM_VER < 1918
	// --- reserved members
	((*pscdm1).doc).byReserved = ((*pscdm2).doc).byReserved;
#endif
	


#if SCDM_VER < 1917
	((*pscdm1).doc).dwReserved1 = ((*pscdm2).doc).dwReserved1;
	((*pscdm1).doc).dwReserved2 = ((*pscdm2).doc).dwReserved2;
#else
#if SCDM_VER < 1922
	((*pscdm1).doc).nReserved = ((*pscdm2).doc).nReserved;
#else	// SCDM_VER < 1922
// <S><D> 2011.11.11 SSL:K.Hamaguchi
//	((*pscdm1).doc).byReserved	= ((*pscdm2).doc).byReserved;
// <E> 2011.11.11 SSL:K.Hamaguchi
#endif	// SCDM_VER < 1922
#endif	


	// --- dwPaperFold
	if (((*pscdm2).doc).dwPaperFold & FOLD_ENABLE)
		((*pscdm1).doc).dwPaperFold |= FOLD_ENABLE;
	else
		((*pscdm1).doc).dwPaperFold &= ~FOLD_ENABLE;



		// --- dwDownloadRAM
	((*pscdm1).doc).dwDownloadRAM = ((*pscdm2).doc).dwDownloadRAM;

	// --- DM_FONT_SELECT
	if ((*pscdm2).doc.dwFieldDDM & DM_FONT_SELECT)
		(*pscdm1).doc.dwFieldDDM |= DM_FONT_SELECT;
	else
		(*pscdm1).doc.dwFieldDDM &= ~DM_FONT_SELECT;

	// --- DM_SCALE_MYSELF
	if ((*pscdm2).doc.dwFieldDDM & DM_SCALE_MYSELF)
		(*pscdm1).doc.dwFieldDDM |= DM_SCALE_MYSELF;
	else
		(*pscdm1).doc.dwFieldDDM &= ~DM_SCALE_MYSELF;


		// --- DM_DUPLEX_APP
	if ((*pscdm2).doc.dwFieldDDM & DM_DUPLEX_APP)
		(*pscdm1).doc.dwFieldDDM |= DM_DUPLEX_APP;
	else
		(*pscdm1).doc.dwFieldDDM &= ~DM_DUPLEX_APP;


		// ================================================
		// _/_/_/  _EXTDMODE
		// ================================================
		// --- reserved members
	
#if SCDM_VER < 1918
	((*pscdm1).ext).wReserve = ((*pscdm2).ext).wReserve;
#endif



#if SCDM_VER < 1917
	((*pscdm1).ext).byReserved = ((*pscdm2).ext).byReserved;
#endif	
	// << 2005.10.25 SBC:Kawabata

	// for CR2
#if SCDM_VER < 1918
	((*pscdm1).ext).ColData.byReserved = ((*pscdm2).ext).ColData.byReserved;
#endif	


#if SCDM_VER >= 1919
#if SCDM_VER >= 1920
	((*pscdm1).ext).ColData.byDitherID = ((*pscdm2).ext).ColData.byDitherID;
#else
	((*pscdm1).ext).ColData.byReserved = ((*pscdm2).ext).ColData.byReserved;
#endif
#if SCDM_VER >= 1922
	// >> 2010.8.16 sbc:hahsida
	//	((*pscdm1).ext).ColData.byReserved	= ((*pscdm2).ext).ColData.byReserved;
	// << 2010.8.16 sbc:hahsida
#else	// SCDM_VER >= 1922
	((*pscdm1).ext).ColData.wReserved = ((*pscdm2).ext).ColData.wReserved;
#endif	// SCDM_VER >= 1922
#else
	((*pscdm1).ext).ColData.dwReserved1 = ((*pscdm2).ext).ColData.dwReserved1;
#endif

#if SCDM_VER < 1922
	((*pscdm1).ext).PCFax.wReserved = ((*pscdm2).ext).PCFax.wReserved;
	((*pscdm1).ext).PCFax.dwReserved1 = ((*pscdm2).ext).PCFax.dwReserved1;
#endif	// SCDM_VER >= 1922


		// --- dwFieldEDM1
		// --- DM_ALWAYSUSE_ACNUM
	if ((*pscdm2).ext.dwFieldEDM1 & DM_TPTEXT)
		(*pscdm1).ext.dwFieldEDM1 |= DM_TPTEXT;
	else
		(*pscdm1).ext.dwFieldEDM1 &= ~DM_TPTEXT;

	// --- DM_FIRSTPAGE
	if ((*pscdm2).ext.dwFieldEDM1 & DM_FIRSTPAGE)
		(*pscdm1).ext.dwFieldEDM1 |= DM_FIRSTPAGE;
	else
		(*pscdm1).ext.dwFieldEDM1 &= ~DM_FIRSTPAGE;

	// --- DM_OUTLINEONLY
	if ((*pscdm2).ext.dwFieldEDM1 & DM_OUTLINEONLY)
		(*pscdm1).ext.dwFieldEDM1 |= DM_OUTLINEONLY;
	else
		(*pscdm1).ext.dwFieldEDM1 &= ~DM_OUTLINEONLY;

	// --- wFieldJC
	// --- DM_ALWAYSUSE_ACNUM
	if ((*pscdm2).ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM)
		(*pscdm1).ext.JobCtrl.wFieldJC |= DM_ALWAYSUSE_ACNUM;
	else
		(*pscdm1).ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;

	// --- DM_ALWAYSUSE_USERID
	if ((*pscdm2).ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_USERID)
		(*pscdm1).ext.JobCtrl.wFieldJC |= DM_ALWAYSUSE_USERID;
	else
		(*pscdm1).ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_USERID;

	// --- DM_ALWAYSUSE_JOBID
	if ((*pscdm2).ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_JOBID)
		(*pscdm1).ext.JobCtrl.wFieldJC |= DM_ALWAYSUSE_JOBID;
	else
		(*pscdm1).ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_JOBID;

	// --- DM_USE_PIN
	if ((*pscdm2).ext.JobCtrl.wFieldJC & DM_USE_PIN)
		(*pscdm1).ext.JobCtrl.wFieldJC |= DM_USE_PIN;
	else
		(*pscdm1).ext.JobCtrl.wFieldJC &= ~DM_USE_PIN;

	// --- DM_USE_LOGINNAME
	if ((*pscdm2).ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME)
		(*pscdm1).ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;
	else
		(*pscdm1).ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;

	// --- DM_USE_LOGINPASS
	if ((*pscdm2).ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS)
		(*pscdm1).ext.JobCtrl.wFieldJC |= DM_USE_LOGINPASS;
	else
		(*pscdm1).ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;

	//-- Job name
	memcpy_s((*pscdm1).ext.JobCtrl.szJobname, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname) * sizeof(WCHAR),
		(*pscdm2).ext.JobCtrl.szJobname, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szJobname) * sizeof(WCHAR));

	//-- user Name
	memcpy_s((*pscdm1).ext.JobCtrl.szUsername, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername) * sizeof(WCHAR),
		(*pscdm2).ext.JobCtrl.szUsername, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szUsername) * sizeof(WCHAR));

	// LOGIN Name
	memcpy_s((*pscdm1).ext.JobCtrl.szLoginName, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName) * sizeof(WCHAR),
		(*pscdm2).ext.JobCtrl.szLoginName, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szLoginName) * sizeof(WCHAR));

	// User number
	memcpy_s((*pscdm1).ext.JobCtrl.byUserNumber, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byUserNumber) * sizeof(WCHAR),
		(*pscdm2).ext.JobCtrl.byUserNumber, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byUserNumber) * sizeof(WCHAR));

	// LOGIN password
	memcpy_s((*pscdm1).ext.JobCtrl.byLoginPassword, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byLoginPassword) * sizeof(WCHAR),
		(*pscdm2).ext.JobCtrl.byLoginPassword, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byLoginPassword) * sizeof(WCHAR));

	// retention Password
	memcpy_s((*pscdm1).ext.JobCtrl.byRetentionPassword, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetentionPassword) * sizeof(WCHAR),
		(*pscdm2).ext.JobCtrl.byRetentionPassword, _countof(m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetentionPassword) * sizeof(WCHAR));

	blRet = TRUE;

EXIT:

	return blRet;
}


//=============================================================================
// function
//      CompareData
//
// parameters
//      pscdm1				SCDM
//      pscdm2				SCDM
//
// return value
//				: 0
//				: 1
//				-1
//
// outline
//	CompareData SCDM
//=============================================================================
long CShCmbUserset::CompareData(PSCDM pscdm1, PSCDM pscdm2, PFEATUREINFOLIST pFeatureInfoList1, PFEATUREINFOLIST pFeatureInfoList2)
{
	long		lRet = -1;

	if (pscdm1 == NULL || pscdm2 == NULL)
		goto EXIT;

	if (CompareFeatureInfoList(pFeatureInfoList1, pFeatureInfoList2) == FALSE)
	{
		lRet = 1;
		goto EXIT;
	}

	// Later have to check 
	/*if (IsEqualBuf((LPBYTE)&((*pscdm1).pub), (LPBYTE)&((*pscdm2).pub), sizeof(PUBDMODE)) == FALSE)
	{
		lRet = 1;
		goto EXIT;
	}*/
	if ((*pscdm1).pub.dmCopies != ((*pscdm2).pub.dmCopies))
	{
		lRet = 1;
#if 0  // For a testing purpose Later we remove Following line : SSDI:Chanchal Singla
		MessageBox(m_hWnd, L"FAILE_PUBDMODE   ", L"Compare PUBDMODE  ", MB_OK | MB_ICONEXCLAMATION);
#endif
		goto EXIT;
	}
	// DOCDMODEÇ
	if (IsEqualBuf(&((*pscdm1).doc), &((*pscdm2).doc), sizeof(_DOCDMODE)) == FALSE)
	{
		lRet = 1;
#if 0  // For a testing purpose Later we remove Following line : SSDI:Chanchal Singla
		MessageBox(m_hWnd, L"FAILE_DDOCMODE   ", L"Compare DOCMODE  ", MB_OK | MB_ICONEXCLAMATION);
#endif
		goto EXIT;
	}

	// EXTDMODEÇ
	if (IsEqualBuf(&((*pscdm1).ext), &((*pscdm2).ext), sizeof(_EXTDMODE)) == FALSE)
	{
		lRet = 1;
#if 0  // For a testing purpose Later we remove Following line : SSDI:Chanchal Singla
		MessageBox(m_hWnd, L"FAILED_EXTDMODE   ", L"Compare EXTDMODE  ", MB_OK | MB_ICONEXCLAMATION);
#endif
		goto EXIT;
	}

	lRet = 0;

EXIT:

	return lRet;
}
//=============================================================================
// function
//      UpdateFeatureInfoList
//
// parameters
//      PFEATUREINFOLIST pFeatureInfoList,
//		LONG Index
//
// return value
//		PFEATUREINFOLIST
//
// outline
//	UpdateFeatureInfoList 
//=============================================================================
PFEATUREINFOLIST CShCmbUserset::UpdateFeatureInfoList(PFEATUREINFOLIST pFeatureInfoList, LONG Index)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	PRINT_FEATURE_OPTION* pFeatureOptionPair = NULL;
	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 0;
	BOOL m_bReadCustomPaperFeature = 0;
	CShRegUS		*preg = NULL;
	GPDFEATUREOPTIONLIST pFeatureOptionList;
	SecureZeroMemory(&pFeatureOptionList, sizeof(pFeatureOptionList));
	/*Get the GPD feature Count*/
	nNumberOfFeatures = GetGPDFeatureCount();
	pFeatureInfoList = new FEATUREINFOLIST();
	if (NULL == pFeatureInfoList)
	{
		slog.putLog("CShCmbUserset::OnUsSelect -> NULL_POINTER(pFeatureInfoList)\n");
	}
	/*Get the GPD feature List */
	CHAR** GPDFeatureList = GetGPDFeatureList();

	/*intialize to Zero pFeatureInfoList*/
	memset(pFeatureInfoList, 0, sizeof(FEATUREINFOLIST));

	/*Update pFeatureInfoList nNummberOfFeatures with Total numbers of feature availbale in GPD file*/
	pFeatureInfoList->nNummberOfFeatures = nNumberOfFeatures;
	pFeatureInfoList->pFeatureOptionsList = new FEATUREOPTIONSLIST[nNumberOfFeatures];
	if (NULL == pFeatureInfoList->pFeatureOptionsList)
	{
		slog.putLog("CShCmbUserset::OnUsSelect -> NULL_POINTER(pFeatureInfoList->pFeatureOptionsList)\n");
	}
	pFeatureInfoList->pFeatureOptionPair = new PRINT_FEATURE_OPTION[nNumberOfFeatures];
	if (NULL == pFeatureInfoList->pFeatureOptionPair)
	{
		slog.putLog("CShCmbUserset::OnUsSelect -> NULL_POINTER(pFeatureInfoList->pFeatureOptionPair)\n");
	}
	/*intialize to Zero FEATUREOPTIONSLIST and PRINT_FEATURE_OPTION*/
	memset(pFeatureInfoList->pFeatureOptionsList, 0, nNumberOfFeatures * sizeof(FEATUREOPTIONSLIST));
	memset(pFeatureInfoList->pFeatureOptionPair, 0, nNumberOfFeatures * sizeof(PRINT_FEATURE_OPTION));
	preg = new CShRegUS(ghInstance, m_pPrinterName, m_hStringResourceHandle, NULL, m_pFeatureInfoList, m_pOemPrivateDevMode);;
	if (preg == NULL)
	{
		goto EXIT;
	}
	if ((*preg).ReadFeatureOptionData(m_pPrinterName, &pFeatureOptionList, Index) == 0)
	{
		ResetGPDFeatureOptionList(m_pPrinterName, &pFeatureOptionList);
	}
	
	//Update the feature info list 
	for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
	{
		pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].pszFeature = GPDFeatureList[nFeatureIndex];
		pFeatureInfoList->pFeatureOptionPair[nFeatureIndex].pszFeature = GPDFeatureList[nFeatureIndex];
		DWORD dwOptions = 0;
		PCSTR *ppszOptions = NULL;
		PCSTR pszOption = NULL;
		m_pPrintCoreHelperUni->EnumOptions(pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].pszFeature, &ppszOptions, &dwOptions);
		pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].ppszOptions = ppszOptions;
		pszOption = GetFeatureOptionFromGPDFeatureOptionList(pFeatureInfoList, &pFeatureOptionList, nFeatureIndex);
		//pszOption = (*preg).ReadEachDataFeatureOption(m_pPrinterName, pFeatureInfoList, nFeatureIndex, Index);
		if (NULL != pszOption)
		{
			for (DWORD dwOptionIndex = 0; dwOptionIndex < dwOptions; dwOptionIndex++)
			{
				if (!_stricmp(ppszOptions[dwOptionIndex], pszOption))
				{
					pFeatureInfoList->pFeatureOptionPair[nFeatureIndex].pszOption = ppszOptions[dwOptionIndex];
				//	pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].uDefaultOptionIndex = dwOptionIndex;
					pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].uChangedOptionIndex = dwOptionIndex;
				}
			}
			//Coverity Fix: 91419 - SSDI Seetharam:20210503
			delete []pszOption;
			pszOption = NULL;
		}
		pFeatureInfoList->pFeatureOptionsList[nFeatureIndex].dwOptions = dwOptions;
	}
EXIT:
	if (preg != NULL)
	{
		delete preg;
		preg = NULL;
	}
	return pFeatureInfoList;
}
PaperInformationMap CShCmbUserset::InitializeMap(void)
{
	PaperInformationMap paperinfomap;
	PAPERINFOINFORMATION paperinfo;
	DWORD dwPaperInfoMapSize = _countof(PaperInformationArray);
	for (DWORD dwCount = 0; dwCount < dwPaperInfoMapSize; dwCount++)
	{
		paperinfo.PageImageaDimensions = std::make_pair(PaperInformationArray[dwCount][1], PaperInformationArray[dwCount][2]);
		paperinfo.PageOrigin = std::make_pair(PaperInformationArray[dwCount][3], PaperInformationArray[dwCount][4]);
		paperinfomap[PaperInformationArray[dwCount][0]] = paperinfo;
	}
	return paperinfomap;
}

VOID CShCmbUserset::MergeDevmode(POEMDEV pOEMDevOut)
{
	PaperInformationMap paperinfomap = InitializeMap();
	PaperInformationMap::const_iterator paperinfoiterator;
	paperinfoiterator = paperinfomap.find(pOEMDevOut->scpi.pub.dmPaperSize);
	if (paperinfoiterator != paperinfomap.end())
	{
		pOEMDevOut->scpi.doc.dwImageableWidthPixel = paperinfoiterator->second.PageImageaDimensions.first;
		pOEMDevOut->scpi.doc.dwImageableHeightPixel = paperinfoiterator->second.PageImageaDimensions.second;
		pOEMDevOut->scpi.doc.nLeftMarginPixel = paperinfoiterator->second.PageOrigin.first;
		pOEMDevOut->scpi.doc.nTopMarginPixel = paperinfoiterator->second.PageOrigin.second;
		if ((PAMPH_2UP != pOEMDevOut->scpi.ext.byPamph) && (FALSE == (DM_FITTOPAGE & pOEMDevOut->scpi.ext.dwFieldEDM1)))
		{
			pOEMDevOut->scpi.ext.dwDstWidthPixel = 2 * pOEMDevOut->scpi.doc.nLeftMarginPixel + pOEMDevOut->scpi.doc.dwImageableWidthPixel;
			pOEMDevOut->scpi.ext.dwDstHeightPixel = 2 * pOEMDevOut->scpi.doc.nTopMarginPixel + pOEMDevOut->scpi.doc.dwImageableHeightPixel;
		}
		if (PAMPH_2UP == pOEMDevOut->scpi.ext.byPamph)
		{
			PaperPixelInformationMap paperpixelinfomap = InitializePaperPixelInfoMap();
			//<S><C> To Modified the condition to the Support of Booklet Output Size,2020-09-02,SSDI Chanchal Singla
			//pOEMDevOut->scpi.ext.nPamphPaperSize = GetPamphSize(pOEMDevOut->scpi.pub.dmPaperSize,pOEMDevOut->scpi.ext.byPamph,TRUE);
			pOEMDevOut->scpi.ext.nPamphPaperSize = GetPamphSize(pOEMDevOut->scpi.ext.nFTPPaperSize, pOEMDevOut->scpi.ext.byPamph, TRUE);
			//<E> To Modified the condition to the Support of Booklet Output Size,2020-09-02,SSDI Chanchal Singla
			PaperPixelInformationMap::const_iterator paperpixelinfoiterator;
			paperpixelinfoiterator = paperpixelinfomap.find(pOEMDevOut->scpi.ext.nPamphPaperSize);
			if (paperpixelinfoiterator != paperpixelinfomap.end())
			{
				pOEMDevOut->scpi.ext.dwDstWidthPixel = paperpixelinfoiterator->second.dwWidthPixel;
				pOEMDevOut->scpi.ext.dwDstHeightPixel = paperpixelinfoiterator->second.dwHeightPixel;
			}
		}

	}
	//else if (pOEMDevOut->scpi.pub.dmPaperSize == 256)
	//{
	//	pOEMDevOut->scpi.pub.dmFields |= DM_PAPERLENGTH | DM_PAPERWIDTH;
	//	pOEMDevOut->scpi.pub.dmPaperWidth = pOEMDevOut->scpi.ext.nDstWidthMilli;
	//	pOEMDevOut->scpi.pub.dmPaperLength = pOEMDevOut->scpi.ext.nDstHeightMilli;
	//}

	BYTE byNUP = pOEMDevOut->scpi.ext.byNupX * pOEMDevOut->scpi.ext.byNupY;

	if (byNUP > 1)
	{
		switch (byNUP)
		{
		case 2:
			pOEMDevOut->scpi.ext.byNupX = 2;
			pOEMDevOut->scpi.ext.byNupY = 1;
			break;
		case 4:
			pOEMDevOut->scpi.ext.byNupX = 2;
			pOEMDevOut->scpi.ext.byNupY = 2;
			break;
		case 6:
			pOEMDevOut->scpi.ext.byNupX = 3;
			pOEMDevOut->scpi.ext.byNupY = 2;
			break;
		case 8:
			pOEMDevOut->scpi.ext.byNupX = 4;
			pOEMDevOut->scpi.ext.byNupY = 2;
			break;
		case 9:
			pOEMDevOut->scpi.ext.byNupX = 3;
			pOEMDevOut->scpi.ext.byNupY = 3;
			break;
		case 16:
			pOEMDevOut->scpi.ext.byNupX = 4;
			pOEMDevOut->scpi.ext.byNupY = 4;
			break;
		default:
			break;

		}
		if (DMORIENT_LANDSCAPE == pOEMDevOut->scpi.pub.dmOrientation)
		{
			BYTE Temp = pOEMDevOut->scpi.ext.byNupX;
			pOEMDevOut->scpi.ext.byNupX = pOEMDevOut->scpi.ext.byNupY;
			pOEMDevOut->scpi.ext.byNupY = Temp;
		}
	}
	//<S><A>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
	// that code should be run at the last place because before running the code devmode should be update to correct values
	// otherwise document is not printing as printable- maximum,Following variables are updating to the support of printable-Maximum
	//(*pdoc).dwImageableWidthPixel 
	//(*pdoc).dwImageableHeightPixel 
	//(*pdoc).nLeftMarginPixel
	//(*pdoc).nTopMarginPixel 
	//(*pext).nDstLeftMarginPixel 
	//(*pext).nDstTopMarginPixel 
	//for a more information regarding this updation of variable check UD3.1 Code and xxxxPI61.dat file
	// ================================================
	// _/_/_/  EDGE TO EDGE (Printable AREA)
	// ================================================
	// ================================================
	// _/_/_/  EDGE TO EDGE (Printable AREA)
	// ================================================
	if (pOEMDevOut->scpi.ext.dwFieldEDM2 & DM_EDGETOEDGE)
	{
		SetPaperSize(&pOEMDevOut->scpi);
	}
	//<E>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
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
PFEATUREINFOLIST CShCmbUserset::UpdateTempFeatureInfoList(PFEATUREINFOLIST pFeatureInfoListTemp, PFEATUREINFOLIST pFeatureInfoListCurrent)
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
PFEATUREINFOLIST CShCmbUserset::InitFeatureInfoList(PFEATUREINFOLIST pFeatureInfoListTemp)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 0;
	/*Get the GPD feature Count*/
	nNumberOfFeatures = GetGPDFeatureCount();
	pFeatureInfoListTemp = new FEATUREINFOLIST();
	if (NULL == pFeatureInfoListTemp)
	{
		slog.putLog("CShCmbUserset::InitFeatureInfoList -> NULL_POINTER(pFeatureInfoList)\n");
	}

	/*intialize to Zero pFeatureInfoList*/
	memset(pFeatureInfoListTemp, 0, sizeof(FEATUREINFOLIST));

	/*Update pFeatureInfoList nNummberOfFeatures with Total numbers of feature availbale in GPD file*/
	pFeatureInfoListTemp->nNummberOfFeatures = nNumberOfFeatures;
	pFeatureInfoListTemp->pFeatureOptionsList = new FEATUREOPTIONSLIST[nNumberOfFeatures];
	if (NULL == pFeatureInfoListTemp->pFeatureOptionsList)
	{
		slog.putLog("CShCmbUserset::InitFeatureInfoList -> NULL_POINTER(pFeatureInfoList->pFeatureOptionsList)\n");
	}
	pFeatureInfoListTemp->pFeatureOptionPair = new PRINT_FEATURE_OPTION[nNumberOfFeatures];
	if (NULL == pFeatureInfoListTemp->pFeatureOptionPair)
	{
		slog.putLog("CShCmbUserset::InitFeatureInfoList -> NULL_POINTER(pFeatureInfoList->pFeatureOptionPair)\n");
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
BOOL CShCmbUserset::CompareFeatureInfoList(PFEATUREINFOLIST pFeatureInfoListTemp, PFEATUREINFOLIST pFeatureInfoListCurrent)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 0;
	/*Get the GPD feature Count*/
	nNumberOfFeatures = GetGPDFeatureCount();

	DWORD blRet = TRUE;


	for (UINT uFeatureIndex = 0; uFeatureIndex < nNumberOfFeatures; uFeatureIndex++)
	{
		// <S><A>selected dropdown is not saving everytime for this scenario,01.0.2022,SSDI:Chanchal Singla
		//	GetDevModeFeatureFlag function Motive to add in the code is Don't
		//need to Compare dev-mode features(Config tab Features) in factory default feature becasue selected dropdown is not saving 
		// everytime that function is comparing the devmode feature(Config tab Features).
		if (GetDevModeFeatureFlag(uFeatureIndex) == FALSE)
		{
			blRet = TRUE;
		}
		// <E>selected dropdown is not saving everytime for this scenario,01.0.2022,SSDI:Chanchal Singla
		else if (pFeatureInfoListTemp->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex != pFeatureInfoListCurrent->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex)
		{
			blRet = FALSE;
#if 0  // For a testing purpose Later we remove Following line : SSDI:Chanchal Singla
			UINT uChangedOptionIndex = pFeatureInfoListTemp->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex;
			WCHAR szTitle[MAX_PATH];
			SecureZeroMemory(&szTitle, MAX_PATH);
			wcscpy_s(szTitle,L"FeatureList_Compare");
			WCHAR szMsg[MAX_PATH];
			SecureZeroMemory(&szMsg, MAX_PATH);
			WCHAR szBuffer[MAX_PATH];
			SecureZeroMemory(&szBuffer, MAX_PATH);

			wsprintfW(szBuffer, L"uFeatureIndex =  %d    ", uFeatureIndex);
			wsprintfW(szMsg, L"pFeatureInfoListTemp->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex= %d       ", uChangedOptionIndex);
			wcscat_s(szBuffer, MAX_PATH, szMsg);
			MessageBox(m_hWnd, szBuffer, szTitle, MB_OK | MB_ICONEXCLAMATION);

			szMsg[MAX_PATH];
			SecureZeroMemory(&szMsg, MAX_PATH);
			szBuffer[MAX_PATH];
			SecureZeroMemory(&szBuffer, MAX_PATH);
			uChangedOptionIndex = pFeatureInfoListCurrent->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex;
			wsprintfW(szBuffer, L"uFeatureIndex %d            ", uFeatureIndex);
			wsprintfW(szMsg, L"pFeatureInfoListCurrent->pFeatureOptionsList[uFeatureIndex].uChangedOptionIndex= %d           ", uChangedOptionIndex);
			wcscat_s(szBuffer, MAX_PATH, szMsg);
			MessageBox(m_hWnd, szBuffer, szTitle, MB_OK | MB_ICONEXCLAMATION);
#endif
			break;
		}
	}
	return blRet;
}
//=============================================================================
// function
//      MergeConfigTabDevmodeAndfeatureInfoList
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
//     MergeConfigTabDevmodeAndfeatureInfoList
//=============================================================================
BOOL CShCmbUserset::MergeConfigTabDevmodeAndfeatureInfoList(PSCDM pscdm1, PSCDM pscdm2, PFEATUREINFOLIST pFeatureInfoList, PFEATUREINFOLIST pFeatureInfoListCurrent)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	DWORD blRet = TRUE;

	eDeviceType sDevicetype = eColor;
	CRegistryAPI cRegApi;
	cRegApi.GetDeviceType(m_pPrinterName, sDevicetype);
	// <S><D>selected dropdown is not saving everytime for this scenario,01.0.2022,SSDI:Chanchal Singla
//	memcpy(&pscdm1->dev, &pscdm2->dev, sizeof(_DEVDMODE));

	/*UINT TempArray[14] = { FEATURE_DEVICECOLORMODE,
							FEATURE_DEVICESTAPLEMODULE,
							FEATURE_STAPLELESS,
							FEATURE_DEVICEPUNCHMODULE,
							FEATURE_MAXPAPERSIZE,
							FEATURE_SINGLESIGNON,
							FEATURE_AUTOCOLORMODE,
							FEATURE_PINCOMPATIBILITY,
							FEATURE_DEVICEPRINTRELEASE,
							FEATURE_PPLCYPRINTRELEASE,
							FEATURE_PPLCYRETENTION,
							FEATURE_PPLCYUSERAUTHENTICATION,
							FEATURE_PPLCYWINLOGIN,
							FEATURE_PPLCYLOGINNAME };
	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 14;

	for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
	{
		pFeatureInfoList->pFeatureOptionPair[TempArray[nFeatureIndex]].pszOption = pFeatureInfoListCurrent->pFeatureOptionPair[TempArray[nFeatureIndex]].pszOption;

		pFeatureInfoList->pFeatureOptionsList[TempArray[nFeatureIndex]].uDefaultOptionIndex = pFeatureInfoListCurrent->pFeatureOptionsList[TempArray[nFeatureIndex]].uDefaultOptionIndex;

		pFeatureInfoList->pFeatureOptionsList[TempArray[nFeatureIndex]].uChangedOptionIndex = pFeatureInfoListCurrent->pFeatureOptionsList[TempArray[nFeatureIndex]].uChangedOptionIndex;
	}*/
	// <E>selected dropdown is not saving everytime for this scenario,01.0.2022,SSDI:Chanchal Singla
	//<S><A>To Fix Redmine Bug #3690 Issue 1,2021.August.26,SSDI:Chanchal Singla
	//Favorites displays untitled when Color Mode or Automatic color mode is changed in Configuration tab
	//Automatic color mode
	BYTE byAutoColorMode = pscdm2->dev.dwFieldVDM & DM_VDM_COLORMODE_AUTO_DISABLE ? bool_true : bool_false;
	SHORT wColorOption = pscdm2->dev.byFieldVDM & DM_VDM_COLORMODEL ? 0 : 1;////color == 0, //black and white == 1

	SHORT wColMode = pscdm1->ext.ColData.byColMode;
	//black and white == 1
	if ((wColorOption == 1) && (wColMode != colmode_mono))	//b&w in config & color/automatic in preference
	{
		(*pscdm1).pub.dmColor = DMCOLOR_MONOCHROME;
		(*pscdm1).pub.dmICMMethod = DMICMMETHOD_NONE;
		(*pscdm1).ext.ColData.byColMode = colmode_mono;
		pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = "Mono";
		pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = eMonoMode;
	}
	wColMode = pscdm1->ext.ColData.byColMode;
	if ((byAutoColorMode == bool_true) && (wColMode == colmode_auto))
	{
		(*pscdm1).pub.dmColor = DMCOLOR_COLOR;
		(*pscdm1).pub.dmICMMethod = DMICMMETHOD_DEVICE;
		(*pscdm1).ext.ColData.byColMode = colmode_color;
		pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption = "Color";
		pFeatureInfoList->pFeatureOptionsList[FEATURE_COLOR_MODE].uChangedOptionIndex = eColorMode;
	}
	//<E>To Fix Redmine Bug #3690 Issue 1,2021.August.26,SSDI:Chanchal 
	//<S><A>To Fix Redmine Bug #4275,2021.September.14,SSDI:Chanchal Singla
	//ìThis Favorite is not available on current Configuration Settingî message is not displayed
	//PPDocumentFiling
	SHORT wPPlcyRetention = pscdm2->dev.dwFieldVDM & DM_PP_RETENTION ? bool_true : bool_false;
	if (wPPlcyRetention != bool_false)
	{
		//Fix Coverity #91538,#91539,#91544,2021.09.15,SSDI:Chanchal Singla
		//SHORT wRetention = bool_true;
		// --- DM_DOCFILING
		pscdm1->ext.JobCtrl.wFieldJC |= DM_DOCFILING;
		// retention_hold_before // Document FilingÇHold Only
		pscdm1->ext.JobCtrl.byRetention = retention_hold_before;
		pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS].uChangedOptionIndex = eSecondOption;
		pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption = "HOLDONLY";

		//docfile_main  // Stored ToÇMain Folder
		pFeatureInfoList->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uChangedOptionIndex = eSecondOption;
		pFeatureInfoList->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption = "MAINFOLDER";

		// PIN CodeÇOFF
		//SHORT wUsePIN = bool_false;
		// --- DM_USE_PIN
		pscdm1->ext.JobCtrl.wFieldJC &= ~DM_USE_PIN;
		pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_PASSWORD].uChangedOptionIndex = eFirstOption;
		pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_PASSWORD].pszOption = "OFF";

		// Print Release, OFF
		SHORT wPrintRelease = bool_false;
		pscdm1->ext.JobCtrl.wFieldJC &= ~DM_PRINTSHARE;
		pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex = wPrintRelease;
		pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption = "OFF";
		// <S><A>To Fix Redmine #4700 Issue 3-4,01.0.2022,SSDI:Chanchal Singla
		//Bug #4700: Favorites store inaccurate values when modifications are made in Configuration Tab
		//SHORT wRetention = bool_true;
		// --- DM_DOCFILING
		pscdm2->ext.JobCtrl.wFieldJC |= DM_DOCFILING;
		// retention_hold_before // Document FilingÇHold Only
		pscdm2->ext.JobCtrl.byRetention = retention_hold_before;
		pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS].uChangedOptionIndex = eSecondOption;
		pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption = "HOLDONLY";

		//docfile_main  // Stored ToÇMain Folder
		pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_DOCUMENT_FILING_SETTINGS].uChangedOptionIndex = eSecondOption;
		pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_DOCUMENT_FILING_SETTINGS].pszOption = "MAINFOLDER";

		// PIN CodeÇOFF
		//SHORT wUsePIN = bool_false;
		// --- DM_USE_PIN
		pscdm2->ext.JobCtrl.wFieldJC &= ~DM_USE_PIN;
		pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_RETENTION_PASSWORD].uChangedOptionIndex = eFirstOption;
		pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_RETENTION_PASSWORD].pszOption = "OFF";

		// Print Release, OFF
		pscdm2->ext.JobCtrl.wFieldJC &= ~DM_PRINTSHARE;
		pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex = wPrintRelease;
		pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption = "OFF";
		// <E>To Fix Redmine #4700 Issue 3-4,01.0.2022,SSDI:Chanchal Singla
		//m_PropertySheetState.wPPlcyPrintRelease = bool_false;
	}
	//<E>To Fix Redmine Bug #4275,2021.September.14,SSDI:Chanchal Singla
	//<S><A>to Fix Bug #3690- issue3 , 2021.12.15,SSDi:Chanchal Singla
// --- DM_PRINTSHARE
	short wPrintRelease = pscdm2->ext.JobCtrl.wFieldJC & DM_PRINTSHARE ? bool_true : bool_false;
	SHORT wPPlcyPrintRelease = pscdm2->dev.dwFieldVDM & DM_PP_PRINTRELEASE ? bool_true : bool_false;
	SHORT byPrintRelease = pscdm2->dev.byFieldVDM & DM_VDM_NON_PRITNRLEASE ? bool_true : bool_false;
	if ((wPrintRelease == TRUE) && (byPrintRelease == bool_true))
	{
		pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex = bool_false;
		pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption = "OFF";
		pscdm1->ext.JobCtrl.wFieldJC &= ~DM_PRINTSHARE;
		pscdm2->ext.JobCtrl.wFieldJC &= ~DM_PRINTSHARE;
		pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex = bool_false;
		pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption = "OFF";
	}
	//<E>to Fix Bug #3690- issue3 , 2021.12.15,SSDi:Chanchal Singla

	// <S><A>selected dropdown is not saving everytime for this scenario,01.0.2022,SSDI:Chanchal Singla
	//Bug #4700: Favorites store inaccurate values when modifications are made in Configuration Tab
	if ((wPPlcyPrintRelease == bool_true) && (byPrintRelease == bool_false))
	{

		SHORT wPrintRelease = bool_true;
		pscdm1->ext.JobCtrl.wFieldJC |= DM_PRINTSHARE;
		pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex = wPrintRelease;
		pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption = "ON";
		// --- DM_DOCFILING
		pscdm2->ext.JobCtrl.wFieldJC |= DM_PRINTSHARE;
		pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex = wPrintRelease;
		pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_PRINTRELEASE].pszOption = "ON";

		SHORT wRetention = pscdm2->ext.JobCtrl.wFieldJC & DM_DOCFILING ? bool_true : bool_false;
		if (wRetention != bool_false)
		{
			pscdm1->ext.JobCtrl.byRetention = retention_normal;
			pFeatureInfoList->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS].uChangedOptionIndex = retention_normal;
			pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption = "NONE";
			pscdm1->ext.JobCtrl.wFieldJC &= ~DM_DOCFILING;

			pscdm2->ext.JobCtrl.byRetention = retention_normal;
			pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_RETENTION_SETTINGS].uChangedOptionIndex = retention_normal;
			pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption = "NONE";
			pscdm2->ext.JobCtrl.wFieldJC &= ~DM_DOCFILING;
		}
		SHORT wPPlcyRetention = bool_false;
	}
	//<S><A> Fix for #5714. K M Harika 16/03/23
	if ((!strcmp("ON", pFeatureInfoList->pFeatureOptionPair[FEATURE_BORDER].pszOption)) && pFeatureInfoList->pFeatureOptionsList[FEATURE_BORDER].uChangedOptionIndex == eSecondOption)
		pscdm1->ext.dwFieldEDM1 |= DM_NUPBORDER;
	if ((!strcmp("ON", pFeatureInfoList->pFeatureOptionPair[FEATURE_HUNDRED_PERCENT_NUP].pszOption)) && pFeatureInfoList->pFeatureOptionsList[FEATURE_HUNDRED_PERCENT_NUP].uChangedOptionIndex == eSecondOption)
		pscdm1->ext.wFieldEDM |= DM_SAMENUP;
	if (((!strcmp("2", pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption))|| (!strcmp("6", pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption))|| (!strcmp("8", pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption))) && 
		(pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex == eSecondOption || pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex == eFourthOption || pFeatureInfoList->pFeatureOptionsList[FEATURE_PAGESPERSHEET].uChangedOptionIndex == eFifthOption))
		pscdm1->ext.byBindedge = eThirdOption;
	if (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex != pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex)
		pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex;
	//<E> Fix for #5714. K M Harika 16/03/23
	//<S><A>To Fix Redmine #7423,21-06-2024,SSDI:Manoj S
	//if (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex != pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex)
	//{
	//	pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_INPUTBIN].uChangedOptionIndex;
	//}

	if (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OFFSET].uChangedOptionIndex != pFeatureInfoList->pFeatureOptionsList[FEATURE_OFFSET].uChangedOptionIndex)
	{
		//<S><C>To Fix Redmine #7580,03-07-2024,SSDI:Harika
		if (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OFFSET].uDefaultOptionIndex == 0) {
			pFeatureInfoList->pFeatureOptionsList[FEATURE_OFFSET].uChangedOptionIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_OFFSET].uChangedOptionIndex;
			//pscdm1->ext.byOffset = eSecondOption;
			pFeatureInfoList->pFeatureOptionPair[FEATURE_OFFSET].pszOption = "None";
		}
		//<E>To Fix Redmine #7580,03-07-2024,SSDI:Harika
	}
	//<E>To Fix Redmine #7423,21-06-2024,SSDI:Manoj S
	SHORT byMaxPaperSize = (pscdm2->dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
	if (byMaxPaperSize == eSecondOption)
	{
		pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET_OUTPUTSIZE].pszOption = (DMPAPER_A4 == GetPaperSizeDef()) ? "A4" : "LETTER";
		pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex = (DMPAPER_A4 == GetPaperSizeDef()) ? eSecondOption : eFifthOption;
	}

	// <E>selected dropdown is not saving everytime for this scenario,01.0.2022,SSDI:Chanchal Singla
	return blRet;
}
//=============================================================================
// function
//      IsExistLimitByOptionTblTab
//
// parameters
//      lVal                
//
// return value
//      TRUE
//      êFALSE
//
// outline
//      IsExistLimitByOptionTblTab
//=============================================================================
BOOL CShCmbUserset::IsExistLimitByOptionTblTab()
{
	BOOL	blRet = FALSE;

	if (m_ppi == NULL) {
		goto EXIT;
	}

	blRet = IsExistLimitByOptionTblCore((*m_ppi).pblLimitByOptionTbl, (*m_ppi).dwFavoritesDataCount);

EXIT:
	return blRet;
}
//=============================================================================
// function
//      IsExistLimitByOptionTblCore
//
// parameters
//      BOOL *pblLimitByOptionTbl,
//		DWORD dwCount
//
// return value
//     TRUE
//      ê FALSE
//
// outline
//      IsExistLimitByOptionTblCore
//=============================================================================
BOOL CShCmbUserset::IsExistLimitByOptionTblCore(BOOL *pblLimitByOptionTbl, DWORD dwCount)
{
	BOOL	blRet = FALSE;
	BOOL	*pblLimitByOptionTblWork = NULL;
	DWORD	dwI;

	if (pblLimitByOptionTbl == NULL) {
		goto EXIT;
	}

	for (dwI = 0; dwI < dwCount; dwI++) {
		if (*(pblLimitByOptionTbl + dwI) == TRUE) {
			blRet = TRUE;
			break;
		}
	}

EXIT:
	return blRet;
}
//=============================================================================
// function
//      getLimitByOptionTblAddrTab
//
// parameters
//      
//
// return value
//       TRUE
//     FALSE
//
// outline
//      getLimitByOptionTblAddrTab
//=============================================================================
BOOL* CShCmbUserset::getLimitByOptionTblAddrTab()
{
	return (*m_ppi).pblLimitByOptionTbl;
}
//=============================================================================
// function
//      deleteAllLimitByOptionTblTab
//
// parameters
//      lVal               
//
// return value
//      êTRUE
//       FALSE
//
// outline
//      êdeleteAllLimitByOptionTblTab
//=============================================================================
BOOL CShCmbUserset::deleteAllLimitByOptionTblTab()
{
	BOOL	blRet = FALSE;

	if (m_ppi == NULL) {
		goto EXIT;
	}

	blRet = deleteAllLimitByOptionTblCore(&(*m_ppi).pblLimitByOptionTbl, &(*m_ppi).dwFavoritesDataCount);

EXIT:
	return blRet;
}
//=============================================================================
// function
//      deleteAllLimitByOptionTblCore
//
// parameters
//      lVal               
//
// return value
//      TRUE
//      FALSE
//
// outline
//     deleteAllLimitByOptionTblCore
//=============================================================================
BOOL CShCmbUserset::deleteAllLimitByOptionTblCore(BOOL **ppblLimitByOptionTbl, DWORD *pdwCount)
{
	BOOL	blRet = FALSE;
	BOOL	*pblLimitByOptionTblWork = NULL;
	DWORD	dwI;
	DWORD	dwSrcI;
	DWORD	dwDstI;
	DWORD	dwCount;
	DWORD	dwDataCount;
	BOOL	*pblLimitByOptionTbl = NULL;

	if (m_ppi == NULL) {
		goto EXIT;
	}

	pblLimitByOptionTbl = *ppblLimitByOptionTbl;

	if (pblLimitByOptionTbl == NULL) {
		goto EXIT;
	}

	dwCount = *pdwCount;

	
	dwDataCount = 0;
	for (dwI = 0; dwI < dwCount; dwI++) {
		if (*(pblLimitByOptionTbl + dwI) == FALSE) {
			dwDataCount++;
		}
	}

	
	pblLimitByOptionTblWork = new BOOL[dwDataCount + 1];
	if (pblLimitByOptionTblWork == NULL) {
		goto EXIT;
	}

	for (dwSrcI = 0, dwDstI = 0; dwSrcI < dwCount; dwSrcI++) {
		if (*(pblLimitByOptionTbl + dwSrcI) == FALSE) {
			*(pblLimitByOptionTblWork + dwDstI) = *(pblLimitByOptionTbl + dwSrcI);
			dwDstI++;
		}
	}

	*(pblLimitByOptionTblWork + dwDstI) = TRUE;

	
	delete[] pblLimitByOptionTbl;


	*ppblLimitByOptionTbl = pblLimitByOptionTblWork;
	*pdwCount = dwDataCount;


EXIT:
	return blRet;
}
//= ============================================================================
// function
//      deleteLimitByOptionTblTab
//
// parameters
//      lVal                
//
// return value
//    TRUE
//     FALSE
//
// outline
//      deleteLimitByOptionTblTab
//=============================================================================
BOOL CShCmbUserset::deleteLimitByOptionTblTab(DWORD dwIndex)
{
	BOOL	blRet = FALSE;

	if (m_ppi == NULL) {
		goto EXIT;
	}

	blRet = deleteLimitByOptionTblCore(&(*m_ppi).pblLimitByOptionTbl, &(*m_ppi).dwFavoritesDataCount, dwIndex);

EXIT:
	return blRet;
}
//=============================================================================
// function
//      deleteLimitByOptionTblCore
//
// parameters
//      lVal              
//
// return value
//     TRUE
//      FALSE
//
// outline
//     deleteLimitByOptionTblCore
//=============================================================================
BOOL CShCmbUserset::deleteLimitByOptionTblCore(BOOL **ppblLimitByOptionTbl, DWORD *pdwCount, DWORD dwIndex)
{
	BOOL	blRet = FALSE;
	BOOL	*pblLimitByOptionTblWork = NULL;
	DWORD	dwI;
	DWORD	dwCount;
	BOOL	*pblLimitByOptionTbl = NULL;

	if (m_ppi == NULL) {
		goto EXIT;
	}

	pblLimitByOptionTbl = *ppblLimitByOptionTbl;

	if (pblLimitByOptionTbl == NULL) {
		goto EXIT;
	}

	dwCount = *pdwCount;
	if (dwIndex >= dwCount) {
		goto EXIT;
	}

	
	pblLimitByOptionTblWork = new BOOL[dwCount];
	if (pblLimitByOptionTblWork == NULL) {
		goto EXIT;
	}


	for (dwI = 0; dwI < dwIndex; dwI++) {
		*(pblLimitByOptionTblWork + dwI) = *(pblLimitByOptionTbl + dwI);
	}

	
	dwCount -= 1;
	for (; dwI < dwCount; dwI++) {
		*(pblLimitByOptionTblWork + dwI) = *(pblLimitByOptionTbl + dwI + 1);
	}

	
	*(pblLimitByOptionTblWork + dwI) = TRUE;


	delete[] pblLimitByOptionTbl;


	*ppblLimitByOptionTbl = pblLimitByOptionTblWork;
	*pdwCount = dwCount;


EXIT:
	return blRet;
}

//=============================================================================
// function
//      addLimitByOptionTblTab
//
// parameters
//      lVal                
//
// return value
//       TRUE
//      êFALSE
//
// outline
//     addLimitByOptionTblTab
//=============================================================================
BOOL CShCmbUserset::addLimitByOptionTblTab(DWORD dwIndex, BOOL blOverWrite)
{
	BOOL	blRet = FALSE;

	if (m_ppi == NULL) {
		goto EXIT;
	}

	blRet = addLimitByOptionTblCore(&(*m_ppi).pblLimitByOptionTbl, &(*m_ppi).dwFavoritesDataCount, dwIndex, blOverWrite);

EXIT:
	return blRet;
}


//=============================================================================
// function
//      addLimitByOptionTblCore
//
// parameters
//      lVal                ê
//
// return value
//      ê TRUE
//      FALSE
//
// outline
//     addLimitByOptionTblCore
//=============================================================================
BOOL CShCmbUserset::addLimitByOptionTblCore(BOOL **ppblLimitByOptionTbl, DWORD *pdwCount, DWORD dwIndex, BOOL blOverWrite)
{
	BOOL	blRet = FALSE;
	BOOL	*pblLimitByOptionTblWork = NULL;
	DWORD	dwI;
	BOOL	*pblLimitByOptionTbl;
	DWORD	dwCount;

	if (m_ppi == NULL) {
		goto EXIT;
	}

	pblLimitByOptionTbl = *ppblLimitByOptionTbl;
	if (pblLimitByOptionTbl == NULL) {
		goto EXIT;
	}

	dwCount = *pdwCount;
	if (dwIndex > dwCount + 1) {
		goto EXIT;
	}

	if (blOverWrite != TRUE) {
		
		pblLimitByOptionTblWork = new BOOL[dwCount + 1 + 1];
		if (pblLimitByOptionTblWork == NULL) {
			goto EXIT;
		}

		
		for (dwI = 0; dwI < dwCount; dwI++) {
			*(pblLimitByOptionTblWork + dwI) = *(pblLimitByOptionTbl + dwI);
		}

		
		*(pblLimitByOptionTblWork + dwI) = FALSE;

		
		*(pblLimitByOptionTblWork + dwI + 1) = TRUE;

		
		delete[] pblLimitByOptionTbl;

		
		*ppblLimitByOptionTbl = pblLimitByOptionTblWork;
		*pdwCount = dwCount + 1;
	}
	else {
		
		*(pblLimitByOptionTbl + dwIndex) = FALSE;
	}

EXIT:
	return blRet;
}
//<S><A>To Fix Redmine Bug #3845,#3783,2021.august.09, SSDI:Chanchal Singla
//Bug #3845:- Saved favorites are not displayed
//Bug #3783:- Registered Favorites are not hidden even if the setting value of Staple module is changed in Configuration tab
//=============================================================================
// function
//      LimitOptionUpdateforStapleAndPunch
//
// parameters
//      pFeatureInfoList				
//      pscdms				SCDM
//
// return value
//      êSHORT
//
// outline
//		LimitOptionUpdateforStapleAndPunch
//=============================================================================
SHORT CShCmbUserset::LimitOptionUpdateforStapleAndPunch(PSCDM pscdm, PFEATUREINFOLIST pFeatureInfoList)
{
	SHORT blLimitByOption = FALSE;
	PROPSTATE PropertySheetState;
	SecureZeroMemory(&PropertySheetState, sizeof(PROPSTATE));
	// -- Staple
	PropertySheetState.wStaple = pscdm->ext.byStaple;

	//--wPunchType
	SHORT wPunchType = punchtype_none;
	switch (pscdm->ext.byPunchType)
	{
	case PUNCH_NONE:
		wPunchType = punchtype_none;
		break;
	case PUNCH_2HOLES:
		wPunchType = punch_2holes;
		break;
	case PUNCH_3HOLES:
		wPunchType = punch_3holes;
		break;
	case PUNCH_4HOLES:
		wPunchType = punch_4holes;
		break;
	case PUNCH_4HOLES_WIDE:
		wPunchType = punch_4holes_wide;
		break;
	default:
		break;
	}
	PropertySheetState.wPunchType = wPunchType;
	//ByPunchmodule
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
	PropertySheetState.wPunchOption = punch;

	//ByStaplemodule
	if (m_pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_SADDLE)
	{
		PropertySheetState.wStapleOption = stapleopt_3;
	}
	else if (m_pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_2STAPLE)
	{
		PropertySheetState.wStapleOption = stapleopt_2;
	}
	else if (m_pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_1STAPLE)
	{
		PropertySheetState.wStapleOption = stapleopt_1;
	}
	else
	{
		PropertySheetState.wStapleOption = stapleopt_none;
	}

	//ByStapleless
	PropertySheetState.byStapleless = m_pOemPrivateDevMode->scpi.dev.byFieldVDMStaple & DM_VDMS_STAPLELESS ? bool_true : bool_false;
	CShLimit limit(NULL, NULL);

	if (limit.limStapleOptVsStapleType(&PropertySheetState, PropertySheetState.wStaple) != FALSE)
	{
		PropertySheetState.wStaple = staple_none;
		blLimitByOption = bool_true;
	}
	if (limit.limPunchOptVsPunchType(&PropertySheetState, PropertySheetState.wPunchType) != FALSE)
	{
		PropertySheetState.wPunchType = punchtype_none;
		blLimitByOption = bool_true;
	}

	return blLimitByOption;
}
//<E>To Fix Redmine Bug #3845,#3783,2021.august.09, SSDI:Chanchal Singla
// <S><A>selected dropdown is not saving everytime for this scenario,01.0.2022,SSDI:Chanchal Singla
//=============================================================================
// function
//      GetDevModeFeatureFlag
//
// parameters
//     UINT uIndex
//
// return value
//      êBOOL
//
// outline
//		GetDevModeFeatureFlag function Motive to add in the code is Don't
//			need to Compare dev-mode features(Config tab Features) in factory default feature .
//=============================================================================
BOOL CShCmbUserset::GetDevModeFeatureFlag(UINT uIndex)
{
	BOOL bRet = TRUE;
	UINT TempArray[14] = { FEATURE_DEVICECOLORMODE,
							FEATURE_DEVICESTAPLEMODULE,
							FEATURE_STAPLELESS,
							FEATURE_DEVICEPUNCHMODULE,
							FEATURE_MAXPAPERSIZE,
							FEATURE_SINGLESIGNON,
							FEATURE_AUTOCOLORMODE,
							FEATURE_PINCOMPATIBILITY,
							FEATURE_DEVICEPRINTRELEASE,
							FEATURE_PPLCYPRINTRELEASE,
							FEATURE_PPLCYRETENTION,
							FEATURE_PPLCYUSERAUTHENTICATION,
							FEATURE_PPLCYWINLOGIN,
							FEATURE_PPLCYLOGINNAME };
	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 14;

	for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
	{
		if (TempArray[nFeatureIndex] == uIndex)
		{
			bRet = FALSE;
			break;
		}
	}
	return  bRet;
}
//<S><A>To Fix Bug #4705: Favorites displays untitled when Maximum Paper Size is set to A4 / Letter in Configuration tab,2022.17.01SSDI:Chanchal Singla
//=============================================================================
// function
//      UpdateBookletFeature
//
// parameters
//    
//		PFEATUREINFOLIST pFeatureInfoListCurrent	
//
// return value
//     BOOL
//
// outline
//     UpdateBookletFeature based on maxpapersize feature
//=============================================================================
BOOL CShCmbUserset::UpdateBookletFeature(PFEATUREINFOLIST pFeatureInfoListCurrent)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	LONG lVal = pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	CShLimit   limit(&pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_FITTOPAPERSIZE],
		&pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE]);
	SHORT byMaxPaperSize = (m_pOemPrivateDevMode->scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
	BOOL bRet = FALSE;
	UINT uChangedOptionIndex = 0;
	if (byMaxPaperSize == eSecondOption)
	{
		if (!strcmp("Standard", pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
		{
			lVal = pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex;
			bRet = limit.limCmbBookletStdPSize(&m_pps, lVal);
			if (bRet != FALSE)
			{
				uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? BOOKLET_A4_INDEX : BOOKLET_LETTER_INDEX;
				pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex = uChangedOptionIndex;
				bRet = TRUE;
			}
		}
		else
		{
			lVal = pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
			bRet = limit.limCmbFitToPaperSize(&m_pps, lVal, pFeatureInfoListCurrent);
			if (bRet != FALSE)
			{
				uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? OUTPUTPAPERSIZE_A4 : OUTPUTPAPERSIZE_LETTER;
				pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex = uChangedOptionIndex;
				bRet = TRUE;
			}
			//<S><A>To Fix Bug #4705 Issue 2,2022.17.02,SSDI:Chanchal Singla
			if (!strcmp("None", pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
			{
				lVal = pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex;
				bRet = limit.limCmbBookletStdPSize(&m_pps, lVal);
				if (bRet != FALSE)
				{
					uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? BOOKLET_A4_INDEX : BOOKLET_LETTER_INDEX;
					pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex = uChangedOptionIndex;
					bRet = TRUE;
				}
			}
			//<E>To Fix Bug #4705 Issue 2,2022.17.02,SSDI:Chanchal Singla
		}
	}
	else if (!strcmp("None", pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_BOOKLET].pszOption) && (byMaxPaperSize == eFirstOption))
	{
		UINT preBookletlVal = pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex;

		uChangedOptionIndex = (GetPaperSizeDef() == DMPAPER_A4) ? BOOKLET_A3_INDEX : BOOKLET_LEDGER_INDEX;
		pFeatureInfoListCurrent->pFeatureOptionsList[FEATURE_BOOKLET_OUTPUTSIZE].uChangedOptionIndex = uChangedOptionIndex;
		pFeatureInfoListCurrent->pFeatureOptionPair[FEATURE_BOOKLET_OUTPUTSIZE].pszOption = (GetPaperSizeDef() == DMPAPER_A4) ? "A3" : "TABLOID";
		bRet = TRUE;

	}
	return bRet;
}
//<E>To Fix Bug #4705: Favorites displays untitled when Maximum Paper Size is set to A4 / Letter in Configuration tab,2022.17.01,SSDI:Chanchal Singla
// <E>selected dropdown is not saving everytime for this scenario,01.0.2022,SSDI:Chanchal Singla

WCHAR * CShCmbUserset::GetTimeStamp()
{
	return m_szDateTime;
}

VOID * CShCmbUserset::InitializeGPDTimeStamp()
{
	DWORD		dwRead;
	DWORD		dwSize = 0;
	BOOL blRet = FALSE;
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	FILETIME	fileTime, localFileTime;
	SYSTEMTIME	systemTime;
	TCHAR szFName[_MAX_PATH] = { 0 };


	GetProjectFileName(szFName, L"1N01.GPD");
	hFile = ::CreateFile(szFName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		//ÅSCUI_FILEDATETIME_STRLEN
		m_szDateTime = new WCHAR[(SCUI_FILEDATETIME_STRLEN * 2)];

		if (m_szDateTime != NULL)
		{
			SecureZeroMemory(m_szDateTime, (sizeof(WCHAR) * (SCUI_FILEDATETIME_STRLEN * 2)));
			// ïsóvÇ»OUTPUTÇÕNULL
			// http://msdn.microsoft.com/ja-jp/library/cc429752.aspx
			if (0 == ::GetFileTime(
				hFile,		// [IN]
				NULL,		// [OUT]ç
				NULL,		// [OUT]ç
				&fileTime))	// [OUT]ç
			{
				printf("GetFileTimeé∏îs\n");
			}

			// ÉçÅ[ÉJÉãÇÃéûä‘Ç÷ïœä∑
			// http://msdn.microsoft.com/ja-jp/library/cc429724.aspx
			if (0 == ::FileTimeToLocalFileTime(&fileTime, &localFileTime))
			{
				printf("FileTimeToLocalFileTimeé∏îs\n");
			}

			// SYSTEMTIMEç
			// http://msdn.microsoft.com/ja-jp/library/cc429727.aspx
			if (0 == ::FileTimeToSystemTime(&localFileTime, &systemTime))
			{
				printf("FileTimeToSystemTimeé∏îs\n");
			}

			::wsprintf(m_szDateTime,
				L"%04d%02d%02d%02d%02d%02d",
				systemTime.wYear,
				systemTime.wMonth,
				systemTime.wDay,
				systemTime.wHour,
				systemTime.wMinute,
				systemTime.wSecond
			);
		}
		::CloseHandle(hFile);
	}
	return nullptr;
}
