//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : WatermarkCombo.cpp
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
//For Logging Purpose
#include "MFPLogger.h"

#define POS_INIT_VAL -999
#define LEFT_VOID_VAL 3

#define MAX_LINE 30
#define MIN_LINE 12

//=============================================================================
// function
//      CWaterMarkCombo
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
//      Constructor to the class CWaterMarkCombo.
//=============================================================================
CWaterMarkCombo::CWaterMarkCombo(
                    CWaterMarkFileData* pWaterMarkFileData,
					CWaterMarkRegistryData* pWaterMarkRegistryData,
					SHORT& sWaterMarkIndex,
					HMODULE hStringResourceHandle,
					WCHAR* pPrinterName, WCHAR* pWaterMarkString, BOOL& bWaterMarkChanged
			  ):
				CShImageCombo(hStringResourceHandle),
					m_pWaterMarkIndex(&sWaterMarkIndex),
                    m_pbWaterMarkChanged(&bWaterMarkChanged)
{
	SetPrinterName(pPrinterName);
	m_pRegistryWaterMarkData = pWaterMarkRegistryData;
    m_pWaterMarkFileData = pWaterMarkFileData;
    m_pWaterMarkString = pWaterMarkString;
	m_iWidth = 0;
	m_iPosY = POS_INIT_VAL;
	m_lIconWidth = 24;
	m_lIconHeight = 30;
	m_CRColor = 0;
	m_wBmpbid = -1;
	m_pBmpList = NULL;
	m_pJsonwm = NULL;
	m_jsonFlag = false;
}

CWaterMarkCombo::CWaterMarkCombo(
	CWaterMarkFileData* pWaterMarkFileData,
	CWaterMarkRegistryData* pWaterMarkRegistryData,
	CShJsonWm*	pjsonwm,
	BOOL		FlagJson,
	SHORT& sWaterMarkIndex,
	HMODULE hStringResourceHandle,
	WCHAR* pPrinterName, WCHAR* pWaterMarkString, BOOL& bWaterMarkChanged
) :
	CShImageCombo(hStringResourceHandle),
	m_pWaterMarkIndex(&sWaterMarkIndex),
	m_pbWaterMarkChanged(&bWaterMarkChanged)
{
	SetPrinterName(pPrinterName);
	m_pRegistryWaterMarkData = pWaterMarkRegistryData;
	m_pWaterMarkFileData = pWaterMarkFileData;
	m_pWaterMarkString = pWaterMarkString;
	m_iWidth = 0;
	m_iPosY = POS_INIT_VAL;
	m_lIconWidth = 24;
	m_lIconHeight = 30;
	m_CRColor = 0;
	m_wBmpbid = -1;
	m_pBmpList = NULL;
	m_pJsonwm = pjsonwm;
	m_jsonFlag = FlagJson;
}

//=============================================================================
// function
//      CWaterMarkCombo
//
// parameters
//      NILL            
//
// return value
//     NILL
//
// outline
//      Destructure to the class CWaterMarkCombo.
//=============================================================================
CWaterMarkCombo::~CWaterMarkCombo(void)
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
HRESULT
CWaterMarkCombo::OnInit(
_In_ CONST INT   iCtrlID,
    _In_ CONST HWND hDlg
    )
{
    HRESULT hr = S_OK;
#if 1 //watermark spec change
	if(NULL == m_pRegistryWaterMarkData)
	{
		return E_FAIL ;
	}
#else
    if(NULL == m_pWaterMarkFileData)
	{
		return E_FAIL ;
	}
#endif
	//SendDlgItemMessage(hDlg, iCtrlID, CB_RESETCONTENT,0,0);

	BOOL blRet = FALSE;
	WCHAR szTitle[SCUI_MAX_STRING] = {0};
#if 1 //watermark spec change
	DWORD dwCount = 0;
	if(m_jsonFlag)
		dwCount = m_pJsonwm->ReadCount();
	else
		dwCount = m_pRegistryWaterMarkData->ReadWMCount(GetPrinterName());
	
	if (CreateShCtrlItem((short)dwCount) != FALSE)
	{
		for (DWORD index = 0, wIndex = 0; index < dwCount; index++)
		{
			WATERMARKDATA WaterMarkDataElement = { 0 };
			SecureZeroMemory(szTitle, sizeof(szTitle));
			if (m_jsonFlag)
				m_pJsonwm->ReadWMData(index, &WaterMarkDataElement, szTitle, sizeof(szTitle));
			else
				m_pRegistryWaterMarkData->ReadWMData(GetPrinterName(), index, &WaterMarkDataElement, szTitle, sizeof(szTitle));

			m_pci[wIndex].wIndex = (short)wIndex;
			m_pci[wIndex].lValue = (long)index;
			WCHAR *pstr = new WCHAR[INI_STR_MAX];
			if (pstr == NULL)
				break; 
			//Fixed Coverity issue - 90619 - SSDI:Seetharam-20200928
			//::ZeroMemory(pstr, sizeof(pstr));
			::ZeroMemory(pstr, INI_STR_MAX);
			wcscpy_s(pstr, INI_STR_MAX, szTitle); // Copy the String to pstr
			m_pci[wIndex].pStr = pstr;

			wIndex++;
		}

		blRet = InitItems(iCtrlID, hDlg);
	}
#else
    WORD wCount = m_pWaterMarkFileData->GetWMCount();
		if (CreateShCtrlItem((short)dwCount) != FALSE)
		{
			for (long index = 0, wIndex = 0; index < dwCount; index++)
			{
				WATERMARKDATA WaterMarkDataElement = { 0 };
				SecureZeroMemory(szTitle, sizeof(szTitle));
				WATERMARKDATA WaterMarkDataElement = { 0 };
			
				m_pWaterMarkFileData->GetWMDefData(wOptionIndex, &WaterMarkDataElement);
				
				m_pci[wIndex].wIndex = (short)wIndex;
				m_pci[wIndex].lValue = (long)index;
				WCHAR *pstr = new WCHAR[INI_STR_MAX];
				if (pstr == NULL)
					break;
				::ZeroMemory(pstr, sizeof(pstr));
				wcscpy_s(szTitle, SCUI_MAX_STRING, WaterMarkDataElement.szMainWM);
				wcscpy_s(pstr, INI_STR_MAX, szTitle); // Copy the String to pstr
				m_pci[wIndex].pStr = pstr;

				wIndex++;
			}

			blRet = InitItems(iCtrlID, hDlg);
		}
	}
#endif
	SendDlgItemMessage(hDlg, iCtrlID, CB_SETCURSEL,*m_pWaterMarkIndex, 0);
	return (blRet == FALSE) ? E_FAIL : S_OK;
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
HRESULT
CWaterMarkCombo::ActivateUIControl(_In_ CONST INT   iCtrlID,
    _In_ CONST HWND hDlg
    )
{
	HRESULT  hr       = S_OK;
	
	SendDlgItemMessage(hDlg, iCtrlID, CB_SETCURSEL,*m_pWaterMarkIndex, 0);
	
	return hr;
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
CWaterMarkCombo::OnCommand(_In_ CONST INT   iCtrlID,
    _In_ CONST HWND hDlg,
    _In_ INT iCommand
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    HRESULT hr = S_OK;

    switch (iCommand)
    {
        case CBN_SELCHANGE:
        {
			hr = OnSelChange(iCtrlID,hDlg);
			if(S_OK != hr)
			{
				slog.LoggingErrorHResultValues();
			}
        }
        break;

        default:
        {
			slog.putLog("CWaterMarkCombo::OnCommand -> NOT_IMPLEMENTED\n");
            hr = E_NOTIMPL;
        }
    }

     return hr;
}
//=============================================================================
// function
//      OnSelChange
//
// parameters
//     iCtrlID ----- Control ID of combo  box control 
//		hDlg ------- handle to the dialog box that contains the control.   
// return value
//     HRESULT
//
// outline
//      Handles combo  box selection change  events
//=============================================================================
HRESULT
CWaterMarkCombo::OnSelChange(_In_ CONST INT   iCtrlID,
    _In_ CONST HWND hDlg
    )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    if(nullptr == m_pWaterMarkIndex) 
	{
		slog.putLog("CWaterMarkCombo::OnSelChange -> Error_WaterMarkIndex\n");
		return E_FAIL;
	}
    if (nullptr == m_pWaterMarkFileData)
	{
		slog.putLog("CWaterMarkCombo::OnSelChange -> Error_WaterMarkFileData\n");
		return E_FAIL;
	}
	//<S><C> RedMine Ticket(#893): 
	// Apply button enables when Default is clicked on Watermark tab. : 2013.12.09, SSDI:Gajanana D S
	SHORT sWaterMArkIndex= static_cast<SHORT>(SendDlgItemMessage(hDlg, iCtrlID, CB_GETCURSEL, 0, 0));
	if( sWaterMArkIndex != *m_pWaterMarkIndex)
	{
		EnableApplyButton(hDlg);
		*m_pWaterMarkIndex = sWaterMArkIndex;

        if (0 == sWaterMArkIndex) 
        {
            SecureZeroMemory(m_pWaterMarkString,(WM_STR_LENGTH + 1) * sizeof(WCHAR));
        }
        *m_pbWaterMarkChanged = TRUE;
	}
    else
    {
        *m_pbWaterMarkChanged = FALSE;
    }
	//<E> RedMine Ticket(#893): 
	return S_OK;
}

/*++
Routine Name:

    CWaterMarkCombo::SetTabDefaults

Routine Description:

    The routine resets the UI to factory defaults

Arguments:

    iCtrlID - Control ID
    hDlg - handle to the dialog

Return Value:

   S_OK
   
--*/
HRESULT CWaterMarkCombo::SetTabDefaults(_In_ CONST INT   iCtrlID,_In_ CONST HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
    if(nullptr == m_pWaterMarkIndex)
    {
		slog.putLog("CWaterMarkCombo::SetTabDefaults -> Error_WaterMarkIndex\n");
        return E_FAIL;
    }
	SHORT sWaterMarkIndex = *m_pWaterMarkIndex;
	*m_pWaterMarkIndex = 0;
	SendDlgItemMessage(hDlg, iCtrlID, CB_SETCURSEL,*m_pWaterMarkIndex, 0);
	if(sWaterMarkIndex != *m_pWaterMarkIndex)
	{
		EnableApplyButton(hDlg);
	}

    return S_OK;
}
