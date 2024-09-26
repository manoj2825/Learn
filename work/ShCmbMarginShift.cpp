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

#include "precomp.h"
//For Logging Purpose
#include "MFPLogger.h"
#include "../../common/include/sh_api.h"
#include "shjsonms.h"
#include <wchar.h>
#include <winuser.h>

//=============================================================================
// function
//      CShCmbMarginSfts
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
//      Constructor to the class CShImageCombo.
//=============================================================================


CShCmbMarginSft::CShCmbMarginSft(
	PRESOURCEIDINFO pResourceIDInfo,
	PRINT_FEATURE_OPTION*  pFeatureOptionPair,
	PFEATUREOPTIONSLIST pFeatureOptionsList,
	PFEATUREINFOLIST pFeatureInfoList,
	HMODULE hStringResourceHandle,
	HMODULE hDialogResourceHandle,
	HMODULE hBitmapResourceHandle,
	DEFAULTDEVMODEINFO& pDefaultSettings,
	CShowMessage* ObjCShowMessage,
	UINT iCtrlID,
	DWORD slectedIndex,
	WCHAR * pszPrnName
) :CShImageCombo(
	pResourceIDInfo,
	pFeatureOptionPair,
	pFeatureOptionsList,
	hStringResourceHandle,
	hDialogResourceHandle,
	hBitmapResourceHandle,
	pDefaultSettings,
	ObjCShowMessage,
	iCtrlID,
	slectedIndex
)
{
	m_pFeatureInfoList = pFeatureInfoList;
	wcscpy_s(m_PszPrnName, INI_STR_MAX, pszPrnName); // Copy the printer name String to m_PszPrnName
}

//=============================================================================
// function
//      CShCmbMarginSft
//
// parameters
//      NILL            
//
// return value
//     NILL
//
// outline
//      Destructure to the class CShImageCombo.
//=============================================================================
CShCmbMarginSft::~CShCmbMarginSft()
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


HRESULT CShCmbMarginSft::OnInit(const INT iCtrlID, const HWND hDlg)
{
	HRESULT hr = S_OK;
	switch (iCtrlID)
	{
	case IDC_CMB_MARGINSFT:
	{
		hr = InitMS(iCtrlID, hDlg);
	}
	break;
	default:
		break;
	}
	return hr;
}
//=============================================================================
// function
//      InitMS
//
// parameters
//      iCtrlID --- Control ID of check  box control  
//		hDlg ------- handle to the dialog box that contains the control.  
//		mode ---  unit Inch/mm   
// return value
//     HRESULT
//
// outline
//      Handles command sent by property sheet
//=============================================================================

BOOL CShCmbMarginSft::InitMS(CONST INT   iCtrlID, CONST HWND hDlg, short wMode)
{
	BOOL			blRet = FALSE;
	CShRegMS		*preg = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_PszPrnName, szCommonDatFilePath, FALSE);
	CShJsonMS	*pjsonms = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonms = new CShJsonMS(ghInstance, m_PszPrnName);
		pjsonms->Init();
	}

	DWORD			dwCount;
	DWORD			i;
	MARGINSHIFTDATA	msd;
	long			lUnit;
	long			lInt;
	long			lDec;

	//Fixed Coverity issue - 90449 - SSDI:Seetharam-20200804
	char			szData[256];
	char			szBase[256];


	preg = new CShRegMS(m_hStringResourceHandle, m_PszPrnName);
	if (preg == NULL)
		goto EXIT;

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		dwCount = (*pjsonms).ReadMSCount();
		if (dwCount <= 0)
		{
			(*pjsonms).reset();
			dwCount = (*pjsonms).ReadMSCount();
		}
	}
	else
	{
		dwCount = (*preg).ReadMSCount(m_PszPrnName);
		if (dwCount <= 0)
		{
			(*preg).reset(m_PszPrnName);
			dwCount = (*preg).ReadMSCount(m_PszPrnName);
		}
	}

	blRet = FALSE;;

	if (CreateShCtrlItem((short)dwCount) != FALSE)
	{
		for (i = 0; i < dwCount; i++)
		{
			if ((*m_pmcf).IsWriteToJson() == TRUE)
				(*pjsonms).ReadMSData(i, &msd);
			else
				(*preg).ReadMSData(m_PszPrnName, i, &msd);
			if (wMode == mode_unit)
			{
				::ZeroMemory(szBase, sizeof(szBase));
				lUnit = msd.dwUnit;
				//Fixed Coverity issue - 90441 - SSDI:Seetharam-20200827
				if (lUnit == unit_mm)
					LoadString(m_hStringResourceHandle, IDS_MARGIN_SFT_MM, (LPWSTR)szBase, sizeof(szBase)/2);
				else
					LoadString(m_hStringResourceHandle, IDS_MARGIN_SFT_INCH, (LPWSTR)szBase, sizeof(szBase)/2);

				m_pci[i].wIndex = (short)i;
				m_pci[i].lValue = (short)msd.dwVal;

				if (wMode == mode_std || m_pci[i].lValue == 0)
				{
					m_pci[i].wStrID = (short)IDS_MARGIN_NONE;
					char *pstr = new char[255];
					//Fixed Coverity issue - 90549 - SSDI:Seetharam-20200928
					//::ZeroMemory(pstr, sizeof(pstr));
					::ZeroMemory(pstr, 255);
					LoadString(m_hStringResourceHandle, m_pci[i].wStrID, (LPWSTR)pstr, 255);
					m_pci[i].pStr = (LPWSTR)pstr;
				}
				else
				{
					::ZeroMemory(szData, sizeof(szData));

					if (lUnit == unit_mm)
					{
						::swprintf_s((LPWSTR)szData, _countof(szData) / sizeof(WCHAR), (LPWSTR)szBase, i, msd.dwMilli);
					}
					else
					{
						lInt = msd.dwInch / 10;
						lDec = msd.dwInch - (lInt * 10);
						::swprintf_s((LPWSTR)szData, _countof(szData) / sizeof(WCHAR), (LPWSTR)szBase, i, lInt, GetDecimalSeparator(), lDec);

					}
						m_pci[i].pStr = new WCHAR[(sh_strlen(szData) + 1) * 2];
						if (m_pci[i].pStr == NULL)
							continue;
						sh_strcpy(m_pci[i].pStr, (sh_strlen(szData) + 1) * 2, szData);
				}
			}
		}
		blRet = InitItems(iCtrlID, hDlg);

	
	}
EXIT:
	if (preg != NULL)
		delete preg;

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
