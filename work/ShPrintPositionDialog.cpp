//	Copyright 2020(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : CShPrintPositionDialog.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Chanchal Parkash Singla
//
//  Date		  : 04 May 2020
//
//	Description   : This class Implements Print Position Dialog Functionality
//===========================================================================

//===========================================================================
//  Reviews 
//
//  Sl#     Review Date     Reviewer(s)               Remarks
//===========================================================================

#include "precomp.h"
#include "DevModeUpDate.h"
#include "regkey.h"
#include "shjsonms.h"
#include "shjsonpp.h"
//For Logging Purpose
#include "MFPLogger.h"
#include "windowsx.h"

//=============================================================================
// function
//      CShPrintPositions
//
// parameters
//      hStringResourceHandle - Handle to String Resource
//		uControlID	- Control ID stored in resource file
//		hPrinter
// return value
//     NILL
//
// outline
//      Constructor to the class CShPrintPosition.
//=============================================================================

CShPrintPosition::CShPrintPosition(UINT uContorlID, HMODULE hStringResourceHandle, HANDLE hPrinter, WCHAR* pPrinterName, ResourceIDInfoMap* pResourceIDInfoMap,
			PFEATUREINFOLIST &pFeatureInfoList,std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings, HMODULE hBitmapResourceHandle,
			PDEVMODE pOemPublicDevMode,POEMDEV pTempOemPrivateDevMode,PPROPSTATE pPropertySheetState,CShowMessage* ObjCShowMessage):
			CUIControl(uContorlID, hStringResourceHandle, hPrinter)
{
	m_ObjCShowMessage = ObjCShowMessage;
	m_pResourceIDInfoMap = pResourceIDInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	m_DefaultSettings = pDefaultSettings;
	m_hPrinterHandle = hPrinter;
	m_hBitmapResourceHandle = hBitmapResourceHandle;
	m_pOemPublicDevMode = pOemPublicDevMode;
	m_pTempOemPrivateDevMode = pTempOemPrivateDevMode;
	m_lCurValue = 0;
	SecureZeroMemory(&m_ps_sub, sizeof(PROPSTATE));
	m_ps_sub = pPropertySheetState;
	m_ps_sub->wPrintPosition = ppos_marginshift;
	wcscpy_s(m_PszPrnName, INI_STR_MAX, pPrinterName); // Copy the printer name String to m_PszPrnName
	//Coverity Fix 90386 - SSDI:Seetharam - 20200827
	SecureZeroMemory(&m_ps_pre, sizeof(PROPSTATE));
	//Coverity Fix 90424 - SSDI:Seetharam - 20200907
	memcpy_s(&m_ps_pre, sizeof(PROPSTATE), &m_ps_sub, sizeof(PROPSTATE));
	m_hPropertySheetHandle = NULL;
	wBinEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	wMarginShiftVal = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex;
}

//=============================================================================
// function
//      CShPrintPosition
//
// parameters
//      NILL            
//
// return value
//     NILL
//
// outline
//      Destructure to the class CShPrintPosition.
//=============================================================================
CShPrintPosition::~CShPrintPosition()
{
	DestroyUIComponents();
}

//=============================================================================
//
//Routine Name:
//
//	CShPrintPosition::OnWmInitdialog
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

LRESULT CShPrintPosition::OnWmInitdialog(void)
{
	LRESULT result = 1;
	SetTitle();
	SetControlStrings();
	ActivateUIControl(m_hWnd);
	m_hPropertySheetHandle = m_hWnd;
	CreateControlClass();
	PPOS_ChangedUnit(m_hPropertySheetHandle, m_ps_sub->wPPosUnit);
	MS_ChangedUnit(m_hPropertySheetHandle, m_ps_sub->wMarginUnit);
	InitializeUIControls(m_hWnd);
	SetTabState(m_hWnd);
	return result;
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
CShPrintPosition::ActivateUIControl(
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
			slog.putLog("CDocumentPropertyPage::ActivateUIControl -> NULL_POINTER(pControl)\n");
		}
	}
	return hr;
}

//=============================================================================
// Function :
//		CShPrintPosition::SetTitle
// parameters
//      None
//
// return value
//      
//  TRUE / FALSE
//
// outline
//      Routine for Setting the Title for the dialogs
//=============================================================================s

BOOL CShPrintPosition::SetTitle(void)
{
	TCHAR szItem[MAX_PATH] = { 0 };
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (LoadString(m_hStringResourceHandle, m_uControlID, szItem, countof(szItem)) > 0)
	{
		SetWindowText(m_hWnd, szItem);
	}
	else
	{
		slog.putLog("CShPrintPosition::SetTitle -> ERROR_LOAD_STRING\n");
	}
	return TRUE;

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
void CShPrintPosition::SetControlStrings(void)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CUIControl* pControl = NULL;

	pControl = new CStaticText(IDC_STC_PPOS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PPOS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDC_GRP_PPOS_ODD_PAGE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_GRP_PPOS_ODD_PAGE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDC_STC_PPOS_ODD_X_TITLE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PPOS_ODD_X_TITLE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDC_STC_PPOS_ODD_Y_TITLE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PPOS_ODD_Y_TITLE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDC_GRP_PPOS_EVN_PAGE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_GRP_PPOS_EVN_PAGE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDC_STC_PPOS_EVN_X_TITLE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PPOS_EVN_X_TITLE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDC_STC_PPOS_EVN_Y_TITLE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PPOS_EVN_Y_TITLE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_CP_UNIT_TITLE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_PPOS_UNIT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_PLUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PPOS_ODD_X_SP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_MINUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PPOS_ODD_X_SM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_PLUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PPOS_ODD_Y_SP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_MINUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PPOS_ODD_Y_SM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_PLUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PPOS_EVN_X_SP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_MINUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PPOS_EVN_X_SM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_PLUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PPOS_EVN_Y_SP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_MINUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_PPOS_EVN_Y_SM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}


	pControl = new CStaticText(IDC_STC_MARGINSFT, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_MARGINSFT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDC_STC_MARGINSFT_W_TITLE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_MARGINSFT_W_TITLE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_PLUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_MARGINSFT_SP, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_SPIN_MINUS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_MARGINSFT_SM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_CP_UNIT_TITLE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_MARGINSFT_UNIT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_STAPLE_POS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_MARGINSFT_POS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}


	// Defaults button
	pControl = new CStaticText(IDC_BTN_SHARED_SUB_DEFAULT, m_hStringResourceHandle);
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
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
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
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
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
		slog.putLog("CShPrintPosition::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}
	
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
void CShPrintPosition::CreateControlClass(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	CUIControl* pControl;
	PRESOURCEIDINFO pResourceIDInfo = NULL; // GetResourceIDInfoOfControl(strFeatureName);
	
	//-- Print Position 
	pResourceIDInfo = GetResourceIDInfoOfControl(PRINT_POSITION);
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINTPOSITION],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTPOSITION],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_PRINTPOSITION],
		m_ObjCShowMessage,
		IDC_CMB_PPOS,
		(DWORD)m_ps_sub->wPrintPosition
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_PPOS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	//-- Print Position Unit
	pResourceIDInfo = GetResourceIDInfoOfControl(PRINT_POSITION_UNIT);
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PRINT_POSITION_UNIT],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINT_POSITION_UNIT],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_PRINT_POSITION_UNIT],
		m_ObjCShowMessage,
		IDC_CMB_PPOS_UNIT,
		(DWORD)m_ps_sub->wPPosUnit
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_PPOS_UNIT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	// -- mimic
	pControl = new CShBtnMmcMgnsft(
				IDC_BTN_MARGINSFT_BBOX,
				m_hStringResourceHandle,
				m_pFeatureInfoList,
				m_hBitmapResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_MARGINSFT_BBOX, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShBtnMmcMgnsft)\n");
	}

	//-- Margin Shift 
	pResourceIDInfo = GetResourceIDInfoOfControl(MARGIN_SHIFT);
	pControl = new CShCmbMarginSft(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT],
		m_pFeatureInfoList,
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_MARGINSHIFT],
		m_ObjCShowMessage,
		IDC_CMB_MARGINSFT,
		(DWORD)m_ps_sub->wMargin,
		m_PszPrnName
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_MARGINSFT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShCmbMarginSft)\n");
	}

	pControl = new CInformationIconButton(pResourceIDInfo, m_pFeatureInfoList, &m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT],
		m_ObjCShowMessage, m_hPropertySheetHandle, m_hStringResourceHandle, m_hBitmapResourceHandle, ghInstance, m_PszPrnName,m_ps_sub, IDC_BTN_MARGINSFT_I);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_MARGINSFT_I, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CLayoutTabPropertyPage::CreateContorlClass -> NULL_POINTER(CInformationIconButton)\n");
	}
	
	//-- Margin Shift Unit
	pResourceIDInfo = GetResourceIDInfoOfControl(MARGIN_SHIFT_UNIT);
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGIN_SHIFT_UNIT],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGIN_SHIFT_UNIT],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_MARGIN_SHIFT_UNIT],
		m_ObjCShowMessage,
		IDC_CMB_MARGINSFT_UNIT,
		(DWORD)m_ps_sub->wMarginUnit
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_MARGINSFT_UNIT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}

	//Position - Binding Edge
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
		IDC_CMB_MARGINSFT_POS,
		m_ps_pre
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_MARGINSFT_POS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShImageCombo)\n");
	}


	// --- Odd X
	pControl = new CShEditPPOS(m_hStringResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, IDC_EDT_PPOS_ODD_X,
		IDC_BTN_PPOS_ODD_X_SP, IDC_BTN_PPOS_ODD_X_SM, m_pOemPublicDevMode, m_pTempOemPrivateDevMode, m_ps_sub);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_PPOS_ODD_X, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShEditSpin)\n");
	}

	// --- Odd Y
	pControl = new CShEditPPOS(m_hStringResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, IDC_EDT_PPOS_ODD_Y,
		IDC_BTN_PPOS_ODD_Y_SP, IDC_BTN_PPOS_ODD_Y_SM, m_pOemPublicDevMode, m_pTempOemPrivateDevMode, m_ps_sub);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_PPOS_ODD_Y, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShEditSpin)\n");
	}

	// --- Even X
	pControl = new CShEditPPOS(m_hStringResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, IDC_EDT_PPOS_EVN_X,
		IDC_BTN_PPOS_EVN_X_SP, IDC_BTN_PPOS_EVN_X_SM, m_pOemPublicDevMode, m_pTempOemPrivateDevMode, m_ps_sub);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_PPOS_EVN_X, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShEditSpin)\n");
	}

	// --- Even Y
	pControl = new CShEditPPOS(m_hStringResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, IDC_EDT_PPOS_EVN_Y,
		IDC_BTN_PPOS_EVN_Y_SP, IDC_BTN_PPOS_EVN_Y_SM,m_pOemPublicDevMode, m_pTempOemPrivateDevMode, m_ps_sub);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_PPOS_EVN_Y, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShEditSpin)\n");
	}

	// --- Margin Shift Width Edit Box
	pControl = new CShEditPPOS(m_hStringResourceHandle, m_hPrinterHandle, m_hPropertySheetHandle, IDC_EDT_MARGINSFT_W,
		IDC_BTN_MARGINSFT_SP, IDC_BTN_MARGINSFT_SM, m_pOemPublicDevMode, m_pTempOemPrivateDevMode, m_ps_sub);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_MARGINSFT_W, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CShPrintPosition::CreateContorlClass -> NULL_POINTER(CShEditSpin)\n");
	}
}

//=============================================================================
// function
//      GetResourceIDInfoOfControl
//
// return value
//     PRESOURCEIDINFO: resource id info
//
// outline
//      getting  the resource id  controls.
//=============================================================================
PRESOURCEIDINFO CShPrintPosition::GetResourceIDInfoOfControl(std::string strFeatureName)
{
	ResourceIDInfoMap::iterator ResourceInfoIterator = m_pResourceIDInfoMap->find(strFeatureName);
	if (ResourceInfoIterator != m_pResourceIDInfoMap->end())
	{
		return &(ResourceInfoIterator->second);
	}
	return NULL;
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
LRESULT CShPrintPosition::OnCbnSelchange(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;
	short			wMs_Pre = m_ps_sub->wMarginUnit;
	short			wPpos_Pre = m_ps_sub->wPPosUnit;
	short			wFlag;
	switch (wCtrlID)
	{
	case IDC_CMB_PPOS:
	{
		m_ps_sub->wPrintPosition = (SHORT)GetValue(hDlg, wCtrlID);
	}
	break;
	case IDC_CMB_PPOS_UNIT:
	{
		m_ps_sub->wPPosUnit = (SHORT)GetValue(hDlg, wCtrlID);

		PPOS_ChangedUnit(hDlg, wPpos_Pre);
		PPOS_SetRangeText(hDlg, m_ps_sub->wPPosUnit);
		PPOS_SetUnitText(hDlg, m_ps_sub->wPPosUnit);
	}
	break;
	case IDC_CMB_MARGINSFT:
	{
		m_ps_sub->wMargin = (SHORT)GetValue(hDlg, wCtrlID);
		GetMarginShiftInfo(m_ps_sub->wMargin, &m_ps_sub->wMarginUnit, &m_ps_sub->dwMarginWidth);
		//<S><C>to fix #2381,29-06-2020,SSDI: Chanchal Parkash
	//	MS_ChangedUnit(hDlg, wPpos_Pre);
		MS_ChangedUnit(hDlg, m_ps_sub->wMarginUnit);
		//<S><A>to fix #2381,29-06-2020,SSDI: Chanchal Parkash
		MS_SetRangeText(hDlg, m_ps_sub->wMarginUnit);
		MS_SetUnitText(hDlg, m_ps_sub->wMarginUnit);
		wMarginShiftVal = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex;;
	}
	break;
	case IDC_CMB_MARGINSFT_UNIT:
	{
		m_ps_sub->wMarginUnit = (SHORT)GetValue(hDlg, wCtrlID);
		MS_ChangedUnit(hDlg, wMs_Pre);
		MS_SetRangeText(hDlg, m_ps_sub->wMarginUnit);
		MS_SetUnitText(hDlg, m_ps_sub->wMarginUnit);
	}
	break;
	case IDC_CMB_MARGINSFT_POS:
	{
		// Set the previous index to wBinEdge variable.
		wBinEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;;
		m_ps_sub->wBinEdge = (SHORT)GetValue(hDlg, wCtrlID);
	}
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
//      SetTabState
//
// return value
//     VOID
//
// outline
//     To set states of all controls.
//=============================================================================
void CShPrintPosition::SetTabState(HWND hDlg, WORD wCtrlID)
{
	UIControlMap::const_iterator Iterator;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	long PatternID = 0;

	// Print Position
	Select(hDlg, IDC_CMB_PPOS, m_ps_sub->wPrintPosition);
	
	SetControlState(hDlg, IDC_BTN_PPOS_I, SCUI_STATE_OFF);
	if (m_ps_sub->wPrintPosition == ppos_printposition) // Print position custom
	{
		ShowConstraintsOnMarginShift(hDlg, SCUI_STATE_OFF);
		ShowConstraintsOnPrintPosition(hDlg, SCUI_STATE_ON);

		// --- Set PrintPosition Edit Box Value 
		// --ODDX
		SetEditValue(hDlg, IDC_EDT_PPOS_ODD_X, m_ps_sub->lPPosOddX);
		// --ODDY
		SetEditValue(hDlg, IDC_EDT_PPOS_ODD_Y, m_ps_sub->lPPosOddY);
		// --EVNX
		SetEditValue(hDlg, IDC_EDT_PPOS_EVN_X, m_ps_sub->lPPosEvnX);
		// --EVNY
		SetEditValue(hDlg, IDC_EDT_PPOS_EVN_Y, m_ps_sub->lPPosEvnY);

		// Print Position Unit
		Select(hDlg, IDC_CMB_PPOS_UNIT, m_ps_sub->wPPosUnit);

		PPOS_SetRangeText(hDlg,m_ps_sub->wPPosUnit);
		PPOS_SetUnitText(hDlg,m_ps_sub->wPPosUnit);
		
	}
	else if (m_ps_sub->wPrintPosition == ppos_marginshift)   // Margin Shift
	{

		ShowConstraintsOnMarginShift(hDlg, SCUI_STATE_ON);
		ShowConstraintsOnPrintPosition(hDlg, SCUI_STATE_OFF);

		// --- Margin
		Select(hDlg, IDC_CMB_MARGINSFT, m_ps_sub->wMargin);
		
		SetControlState(hDlg, IDC_CMB_MARGINSFT, SCUI_STATE_ON);
		SetControlState(hDlg, IDC_STC_MARGINSFT, SCUI_STATE_ON);
		SetControlState(hDlg,IDC_BTN_MARGINSFT_I, SCUI_STATE_OFF);
		
		// --- Set Margin Width 
		SetEditValue(hDlg,IDC_EDT_MARGINSFT_W, m_ps_sub->dwMarginWidth);
		
		// --- unit
		Select(hDlg, IDC_CMB_MARGINSFT_UNIT, m_ps_sub->wMarginUnit);

		MS_SetRangeText(hDlg,m_ps_sub->wMarginUnit);
		MS_SetUnitText(hDlg,m_ps_sub->wMarginUnit);
		
		//<S><C>To Modified To Fix #2609 Issue,20-10-2020,SSDI:Chanchal Singla
		// --- Position Edge
		//Select(hDlg, IDC_CMB_MARGINSFT_POS, m_ps_sub->wBinEdge);
	
		m_ps_sub->wBinEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
		
		Iterator = m_UIControls.find(IDC_CMB_MARGINSFT_POS);
		if (Iterator != m_UIControls.end())
		{
			CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
			if (pImageCombo)
			{
				pImageCombo->OnInit(IDC_CMB_MARGINSFT_POS, hDlg);
				pImageCombo->Select(IDC_CMB_MARGINSFT_POS, hDlg,(LONG)m_ps_sub->wBinEdge);
				pImageCombo->OnSelChange(IDC_CMB_MARGINSFT_POS, hDlg);
			}
			else
			{
				slog.putLog("CShPrintPosition::Select -> NULL_OBJECT(pImageCombo)\n");
			}

		}
		
		if (m_ps_sub->wMargin != 0) 
		{
			ShowConstraintsOnBinEdgePosition(hDlg, IDC_CMB_MARGINSFT_POS);
		}
		else
		{
			if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption)
				|| !strcmp("STAPLE_SADDLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
			{
				SetControlState(hDlg, IDC_CMB_MARGINSFT, SCUI_STATE_SHOW);
				SetControlState(hDlg, IDC_STC_MARGINSFT, SCUI_STATE_SHOW);
				SetControlState(hDlg,IDC_BTN_MARGINSFT_I, SCUI_STATE_ON);
			}
			else
			{
				SetControlState(hDlg, IDC_CMB_MARGINSFT_POS, SCUI_STATE_OFF);
				SetControlState(hDlg, IDC_STC_MARGINSFT_POS, SCUI_STATE_OFF);
			}
		}

		// Mimimc Update
	//<E>To Modified To Fix #2609 Issue,20-10-2020,SSDI:Chanchal Singla
	
		HWND hControl = ::GetDlgItem(m_hWnd, IDC_BTN_MARGINSFT_BBOX);
		HDC hDC = ::GetDC(hControl);
		RECT			rc = { 0,0,0,0 };
		::GetWindowRect(hControl, &rc);
		rc.right -= rc.left;
		rc.bottom -= rc.top;
		rc.left = 0;
		rc.top = 0;
		Iterator = m_UIControls.find(IDC_BTN_MARGINSFT_BBOX);
		if (Iterator != m_UIControls.end())
		{
			CShBtnMmcMgnsft *pButton = static_cast<CShBtnMmcMgnsft*>(Iterator->second);
			if (pButton)
			{
				pButton->draw(m_ps_sub, hDC, &rc);
			}
			else
			{
				slog.putLog("CShPrintPosition::SetTabState -> NULL_OBJECT(pButton)\n");
			}
		}
		if (hDC != NULL)
		{
			::ReleaseDC(hControl, hDC);
		}
		else
		{
			slog.putLog("CShPrintPosition::SetTabState -> NULL_HANDLE(hDC)\n");
		}
	}
	
}


//=============================================================================
// function
//      ShowConstraintsOnPrintPosition
//
// parameters
//     HWND hDlg
//
// return value
//      TRUE
//      FALSE
//
// outline
//      Show and Hide Constraints of Print Position feature
//=============================================================================
BOOL CShPrintPosition::ShowConstraintsOnPrintPosition(HWND hDlg, BYTE cPrintPositionGr)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	
	SetControlState(hDlg, IDC_STC_PPOS_ODD_X_TITLE, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_ODD_X_RANGE, cPrintPositionGr);
	SetControlState(hDlg, IDC_EDT_PPOS_ODD_X, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_ODD_X_UNIT, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_ODD_Y_TITLE, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_ODD_Y_RANGE, cPrintPositionGr);
	SetControlState(hDlg, IDC_EDT_PPOS_ODD_Y, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_ODD_Y_UNIT, cPrintPositionGr);
	SetControlState(hDlg, IDC_GRP_PPOS_EVN_PAGE, cPrintPositionGr);
	SetControlState(hDlg, IDC_GRP_PPOS_ODD_PAGE, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_EVN_X_TITLE, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_EVN_X_RANGE, cPrintPositionGr);
	SetControlState(hDlg, IDC_EDT_PPOS_EVN_X, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_EVN_X_UNIT, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_EVN_Y_TITLE, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_EVN_Y_RANGE, cPrintPositionGr);
	SetControlState(hDlg, IDC_EDT_PPOS_EVN_Y, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_EVN_Y_UNIT, cPrintPositionGr);
	SetControlState(hDlg, IDC_BTN_PPOS_ODD_X_SP, cPrintPositionGr);
	SetControlState(hDlg, IDC_BTN_PPOS_ODD_X_SM, cPrintPositionGr);
	SetControlState(hDlg, IDC_BTN_PPOS_ODD_Y_SP, cPrintPositionGr);
	SetControlState(hDlg, IDC_BTN_PPOS_ODD_Y_SM, cPrintPositionGr);
	SetControlState(hDlg, IDC_BTN_PPOS_EVN_X_SP, cPrintPositionGr);
	SetControlState(hDlg, IDC_BTN_PPOS_EVN_X_SM, cPrintPositionGr);
	SetControlState(hDlg, IDC_BTN_PPOS_EVN_Y_SP, cPrintPositionGr);
	SetControlState(hDlg, IDC_BTN_PPOS_EVN_Y_SM, cPrintPositionGr);
	SetControlState(hDlg, IDC_STC_PPOS_UNIT, cPrintPositionGr);
	SetControlState(hDlg, IDC_CMB_PPOS_UNIT, cPrintPositionGr);

	return TRUE;
}

//=============================================================================
// function
//      ShowConstraintsOnMarginShift
//
// parameters
//     HWND hDlg
//
// return value
//       TRUE
//		 FALSE
//
// outline
//      Show and Hide Constraints of Margin Shift feature
//=============================================================================
BOOL CShPrintPosition::ShowConstraintsOnMarginShift(HWND hDlg,BYTE cMarginShiftGr)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	if ((m_ps_sub->wPrintPosition == ppos_marginshift) && (m_ps_sub->wMargin == 0))
	{
		SetControlState(hDlg, IDC_STC_MARGINSFT_W_TITLE, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_MARGINSFT_W_RANGE, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_EDT_MARGINSFT_W, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_BTN_MARGINSFT_SP, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_BTN_MARGINSFT_SM, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_MARGINSFT_W_UNIT, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_MARGINSFT_UNIT, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_MARGINSFT_UNIT, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_STC_MARGINSFT_POS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_CMB_MARGINSFT_POS, SCUI_STATE_OFF);
		SetControlState(hDlg, IDC_BTN_MARGINSFT_I, SCUI_STATE_OFF);

		SetControlState(hDlg, IDC_BTN_MARGINSFT_BBOX, cMarginShiftGr);
	}
	else
	{
		SetControlState(hDlg, IDC_BTN_MARGINSFT_BBOX, cMarginShiftGr);
		SetControlState(hDlg, IDC_STC_MARGINSFT, cMarginShiftGr);
		SetControlState(hDlg, IDC_CMB_MARGINSFT, cMarginShiftGr);
		SetControlState(hDlg, IDC_STC_MARGINSFT_W_TITLE, cMarginShiftGr);
		SetControlState(hDlg, IDC_STC_MARGINSFT_W_RANGE, cMarginShiftGr);
		SetControlState(hDlg, IDC_EDT_MARGINSFT_W, cMarginShiftGr);
		SetControlState(hDlg, IDC_BTN_MARGINSFT_SP, cMarginShiftGr);
		SetControlState(hDlg, IDC_BTN_MARGINSFT_SM, cMarginShiftGr);
		SetControlState(hDlg, IDC_STC_MARGINSFT_W_UNIT, cMarginShiftGr);
		SetControlState(hDlg, IDC_STC_MARGINSFT_UNIT, cMarginShiftGr);
		SetControlState(hDlg, IDC_CMB_MARGINSFT_UNIT, cMarginShiftGr);
		SetControlState(hDlg, IDC_STC_MARGINSFT_POS, cMarginShiftGr);
		SetControlState(hDlg, IDC_CMB_MARGINSFT_POS, cMarginShiftGr);
		SetControlState(hDlg, IDC_BTN_MARGINSFT_I, cMarginShiftGr);
	}
	return TRUE;
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
LRESULT CShPrintPosition::OnBnClicked(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
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
	case IDC_BTN_PPOS_ODD_X_SP:
	case IDC_BTN_PPOS_ODD_X_SM:
	{
		pControl = (*pComponents)[IDC_EDT_PPOS_ODD_X];
		((CShEditPPOS*)pControl)->OnBnClicked(hWnd, hWndCtrl, wCtrlID);
		::SetFocus(GetDlgItem(hWnd, IDC_EDT_PPOS_ODD_X));
	}
	break;
	case IDC_BTN_PPOS_ODD_Y_SP:
	case IDC_BTN_PPOS_ODD_Y_SM:
	{
		pControl = (*pComponents)[IDC_EDT_PPOS_ODD_Y];
		((CShEditPPOS*)pControl)->OnBnClicked(hWnd, hWndCtrl, wCtrlID);
		::SetFocus(GetDlgItem(hWnd, IDC_EDT_PPOS_ODD_Y));
	}
	break;
	case IDC_BTN_PPOS_EVN_X_SP:
	case IDC_BTN_PPOS_EVN_X_SM:
	{
		pControl = (*pComponents)[IDC_EDT_PPOS_EVN_X];
		((CShEditPPOS*)pControl)->OnBnClicked(hWnd, hWndCtrl, wCtrlID);
		::SetFocus(GetDlgItem(hWnd, IDC_EDT_PPOS_EVN_X));
	}
	break;
	case IDC_BTN_PPOS_EVN_Y_SP:
	case IDC_BTN_PPOS_EVN_Y_SM:
	{
		pControl = (*pComponents)[IDC_EDT_PPOS_EVN_Y];
		((CShEditPPOS*)pControl)->OnBnClicked(hWnd, hWndCtrl, wCtrlID);
		::SetFocus(GetDlgItem(hWnd, IDC_EDT_PPOS_EVN_Y));
	}
		break;
	case IDC_BTN_MARGINSFT_SP:
	case IDC_BTN_MARGINSFT_SM:
	{
		pControl = (*pComponents)[IDC_EDT_MARGINSFT_W];
		((CShEditPPOS*)pControl)->OnBnClicked(hWnd, hWndCtrl, wCtrlID);
		::SetFocus(GetDlgItem(hWnd, IDC_EDT_MARGINSFT_W));
	}
		break;
	//<S><A>to Fix Redmine Bug #2240 Issue 3,25.02.2022,SSDI:Chanchal Singla
	//Issue 3: Through tab order, move the focus on to the information icon Press "Space bar" from the keyboard, Information dialog is not popped up
	case IDC_BTN_MARGINSFT_I:
	{
		UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_MARGINSFT_I);
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
	break;
	//<E>to Fix Redmine Bug #2240 Issue 3,25.02.2022,SSDI:Chanchal Singla
	case IDC_BTN_SHARED_OK:
	case IDOK:
	{
		PropstatetoDevMode();
		::EndDialog(hWnd, wCtrlID);
	}
		break;

	case IDC_BTN_SHARED_CANCEL:
	case IDCANCEL:
	{
	//<S><A>To Fix cancel Issue,20-10-2020,SSDI:Chanchal Singla
		SHORT Bindedge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
		if (wBinEdge != Bindedge)
		{
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex = (UINT)wBinEdge;
		}
		SHORT MrgnShft = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex;
		if (wMarginShiftVal != MrgnShft)
		{
			m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex = (UINT)wMarginShiftVal;
			if (wMarginShiftVal == eFirstOption)
				m_pFeatureInfoList->pFeatureOptionPair[FEATURE_MARGINSHIFT].pszOption = "None";
		}
	//<E>To Fix cancel Issue,20-10-2020,SSDI:Chanchal Singla
		CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
	}
		break;
	case IDC_BTN_SHARED_SUB_DEFAULT:
		SetTabDefaults(m_hWnd);
		break;
	default:
		res = CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
	}

	return res;
}



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
LRESULT CShPrintPosition::OnWmDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	short				wCtrlID = (short)wParam;
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;
	m_hPropertySheetHandle = hWnd;
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
				//Fixed Coverity issue - 90394#2 - SSDI:Seetharam - 20201006
				//else
				//{
				//	slog.putLog("CShPrintPosition::OnWmDrawItem -> NULL_OBJECT(CShImageCombo)\n");
				//}
			}
			else if (pControl->GetControlType() == eButton)
			{
				if (NULL != pControl)
				{
					((CShBtnBmpTxt*)pControl)->OnWmDrawItem(hWnd, wParam, lParam);
				}
				//Fixed Coverity issue - 90394#1 - SSDI:Seetharam-20201006
				//else
				//{
				//	slog.putLog("CShPrintPosition::OnWmDrawItem -> NULL_OBJECT(CShImageCombo)\n");
				//}
			}
			
		}
	}
	{
		switch (wCtrlID)
		{
		case IDC_BTN_MARGINSFT_BBOX:
		{
			UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_MARGINSFT_BBOX);
			if (Iterator != m_UIControls.end())
			{
				CShBtnMmcMgnsft *pButton = static_cast<CShBtnMmcMgnsft*>(Iterator->second);
				if (pButton)
				{
					pButton->draw(m_ps_sub, (*pdis).hDC, &(*pdis).rcItem);
				}
				else
				{
					slog.putLog("CShPrintPosition::OnWmDrawItem -> NULL_OBJECT(pButton)\n");
				}
			}
		}
			break;
		case IDC_BTN_MARGINSFT_I:
		{
			UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_MARGINSFT_I);
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
	}
	return CUIControl::OnWmDrawItem(hWnd, wParam, lParam);
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

long CShPrintPosition::GetValue(HWND hDlg, WORD wCtrlID)
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
			Value =	pImageCombo->GetVal(wCtrlID, hDlg);
		}
		else
		{
			slog.putLog("CShPrintPosition::Select -> NULL_OBJECT(pImageCombo)\n");
		}

	}
	return Value;
}
//=============================================================================
// function
//      SetRangeText
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
//
// return value
//      NILL
//
// outline
//      Sets the range text corresponding to Width and Length based on the 
//		Unit selected.
//=============================================================================
void CShPrintPosition::PPOS_SetRangeText(HWND hDlg, int val)
{
	//Fixed Coverity issue - 90379 - SSDI:Seetharam-20200729
	//long		wDigit = 0;
	long		wDigit = 1;
	long		lUpper = 0;
	long		lLower = 0;
	
	switch (val)
	{
	case unit_mm:
	{
		wDigit = SCUI_PPOS_DIGIT_MM;
		lLower = InchToMilli_PPos(MIN_PPOS_X);
		lUpper = InchToMilli_PPos(MAX_PPOS_X);
		PPOS_SetUnitText(hDlg,unit_mm);
	}
	break;
	case unit_inch:
	{
		wDigit = SCUI_PPOS_DIGIT_INCH; 
		lLower = MilliToInch_PPos(MIN_PPOS_X);
		lUpper = MilliToInch_PPos(MAX_PPOS_X);
		PPOS_SetUnitText(hDlg,unit_inch);
	}
	break;
	default:
		break;
	}
	PPOS_SetRangeText(hDlg, IDC_STC_PPOS_ODD_X_RANGE, wDigit, lLower, lUpper);
	PPOS_SetRangeText(hDlg, IDC_STC_PPOS_ODD_Y_RANGE, wDigit, lLower, lUpper);
	PPOS_SetRangeText(hDlg, IDC_STC_PPOS_EVN_X_RANGE, wDigit, lLower, lUpper);
	PPOS_SetRangeText(hDlg, IDC_STC_PPOS_EVN_Y_RANGE, wDigit, lLower, lUpper);
	
}

//=============================================================================
// function
//      SetRangeText
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
// return value
//      NILL
//
// outline
//      Sets the range text corresponding to Width and Length based on the 
//		Unit selected.
//=============================================================================
void CShPrintPosition::PPOS_SetRangeText(HWND hDlg, long wCtrlID, long wDigit, long wLower, long wUpper)
{
	WCHAR szRnge[MAX_PATH] = { 0 };
	WCHAR szForm[MAX_PATH] = { 0 };
	WCHAR szDisp[MAX_PATH] = { 0 };

	wLower = wLower * -1;
	long  wUpInt = wUpper / wDigit;
	long  wUpDec = wUpper - (wUpInt * wDigit);
	long  wLoInt = wLower / wDigit;
	long  wLoDec = wLower - (wLoInt * wDigit);


	
	if (LoadString(m_hStringResourceHandle, IDS_VAL_RANGE_FORM, szRnge, countof(szRnge)) > 0)
	{
		if (wDigit > 1)
		{	
			WCHAR cSep = GetDecimalSeparator();
			::swprintf_s(szForm, _countof(szForm), szRnge, L"-%d%c%d", L"%d%c%d");
			::swprintf_s((LPWSTR)szDisp, _countof(szDisp), (LPCWSTR)szForm, wLoInt, cSep, wLoDec, wUpInt, cSep, wUpDec);
		}
		else
		{
			::swprintf_s(szForm, _countof(szForm), szRnge, L"-%d", L"%d");
			::swprintf_s((LPWSTR)szDisp, _countof(szDisp) / sizeof(WCHAR), (LPWSTR)szForm, wLoInt, wUpInt);
		}
		::SetWindowTextW(::GetDlgItem(m_hWnd, wCtrlID), (LPWSTR)szDisp);
	}

}

//=============================================================================
// function
//      SetUnitText
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
//
// return value
//      NILL
//
// outline
//      Sets the Unit selected.
//=============================================================================
void CShPrintPosition::PPOS_SetUnitText(HWND hDlg,short wVal)
{
	long wDigit = 0;
	WCHAR szDisp[MAX_PATH] = { 0 };

	switch (wVal)
	{
	case unit_mm:
	{
		wDigit = IDS_UNIT_MM;
	}
	break;
	case unit_inch:
	default:
	{
		wDigit = IDS_UNIT_INCH;
	}
	break;
	}

		if (LoadString(m_hStringResourceHandle, wDigit, szDisp, countof(szDisp)) > 0)
		{
			::SetWindowTextW(::GetDlgItem(m_hWnd, IDC_STC_PPOS_ODD_X_UNIT), (LPWSTR)szDisp);
			::SetWindowTextW(::GetDlgItem(m_hWnd, IDC_STC_PPOS_ODD_Y_UNIT), (LPWSTR)szDisp);
			::SetWindowTextW(::GetDlgItem(m_hWnd, IDC_STC_PPOS_EVN_X_UNIT), (LPWSTR)szDisp);
			::SetWindowTextW(::GetDlgItem(m_hWnd, IDC_STC_PPOS_EVN_Y_UNIT), (LPWSTR)szDisp);
		}
	
}


//=============================================================================
// function
//      SetRangeText
//
// parameters
//      hDlg			Handle to window
//		value		
//
// return value
//      NILL
//
// outline
//      Sets the range text corresponding to Width and Length based on the 
//		Unit selected.
//=============================================================================
void CShPrintPosition::MS_SetRangeText(HWND hDlg, int val)
{
	//Fixed Coverity issue - 90506 - SSDI:Seetharam-20200729
	//long		wDigit = 0;
	long		wDigit = 1;
	long		lUpper = 0;
	long		lLower = 0;

	switch (val)
	{
	case unit_mm:
	{
		wDigit = SCUI_MGNSFT_DIGIT_MM;
		lUpper = InchToMilli_MrgnSft(MAX_MS_X);
		lLower = InchToMilli_MrgnSft(MIN_MS_X);
		MS_SetUnitText(hDlg, unit_mm);
	}
	break;
	case unit_inch:
	{
		wDigit = SCUI_MGNSFT_DIGIT_INCH;
		lUpper = MilliToInch_MrgnSft(MAX_MS_X);
		lLower = MilliToInch_MrgnSft(MIN_MS_X);
		MS_SetUnitText(hDlg, unit_inch);
	}
	break;
	default:
		break;
	}
	MS_SetRangeText(hDlg, IDC_STC_MARGINSFT_W_RANGE, wDigit, lLower, lUpper);

}

//=============================================================================
// function
//      SetRangeText
//
// parameters
//      hDlg			Handle to window
//		wCtrlID			Control ID
// return value
//      NILL
//
// outline
//      Sets the range text corresponding to Width and Length based on the 
//		Unit selected.
//=============================================================================
void CShPrintPosition::MS_SetRangeText(HWND hDlg, long wCtrlID, long wDigit, long wLower, long wUpper)
{
	WCHAR szRnge[MAX_PATH] = { 0 };
	WCHAR szForm[MAX_PATH] = { 0 };
	WCHAR szDisp[MAX_PATH] = { 0 };

	wLower = wLower * -1;
	long  wUpInt = wUpper / wDigit;
	long  wUpDec = wUpper - (wUpInt * wDigit);
	long  wLoInt = wLower / wDigit;
	long  wLoDec = wLower - (wLoInt * wDigit);



	if (LoadString(m_hStringResourceHandle, IDS_VAL_RANGE_FORM, szRnge, countof(szRnge)) > 0)
	{
		if (wDigit > 1)
		{
			WCHAR cSep = GetDecimalSeparator();
			::swprintf_s(szForm, _countof(szForm), szRnge, L"%d%c%d", L"%d%c%d");
			::swprintf_s((LPWSTR)szDisp, _countof(szDisp), (LPCWSTR)szForm, wLoInt, cSep, wLoDec, wUpInt, cSep, wUpDec);
		}
		else
		{
			::swprintf_s(szForm, _countof(szForm), szRnge, L"%d", L"%d");
			::swprintf_s((LPWSTR)szDisp, _countof(szDisp) / sizeof(WCHAR), (LPWSTR)szForm, wLoInt, wUpInt);
		}
		::SetWindowTextW(::GetDlgItem(m_hWnd, wCtrlID), (LPWSTR)szDisp);
	}

}

//=============================================================================
// function
//      SetUnitText
//
// parameters
//		hDlg			Handle to window
//		value		
//
// return value
//      NILL
//
// outline
//      Sets the Unit selected.
//=============================================================================
void CShPrintPosition::MS_SetUnitText(HWND hDlg, short wVal)
{
	long wDigit = 0;
	WCHAR szDisp[MAX_PATH] = { 0 };

	switch (wVal)
	{
	case unit_mm:
	{
		wDigit = IDS_UNIT_MM;
	}
	break;
	case unit_inch:
	default:
	{
		wDigit = IDS_UNIT_INCH;
	}
	break;
	}

	if (LoadString(m_hStringResourceHandle, wDigit, szDisp, countof(szDisp)) > 0)
	{
		::SetWindowTextW(::GetDlgItem(m_hWnd, IDC_STC_MARGINSFT_W_UNIT), (LPWSTR)szDisp);
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
HRESULT CShPrintPosition::SetTabDefaults(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	
	UINT index = 0;
	WORD MSCtrlID[3] = { IDC_CMB_MARGINSFT, IDC_CMB_MARGINSFT_UNIT, IDC_CMB_MARGINSFT_POS};
	UIControlMap::const_iterator Iterator;
	
	if (m_ps_sub->wPrintPosition == ppos_printposition)
	{
		//-- Get PrintPosition and Update Propstate
		GetDefPrintPosition(hDlg);
		
		m_ps_sub->wPPosChg = bool_false;

		Iterator = m_UIControls.find(IDC_CMB_PPOS_UNIT);
		if (Iterator != m_UIControls.end())
		{
			CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
			pImageCombo->SetTabDefaults(IDC_CMB_PPOS_UNIT, hDlg);
		}
			
		
	}
	else if (m_ps_sub->wPrintPosition == ppos_marginshift)
	{
		while (index < 3)
		{
			Iterator = m_UIControls.find(MSCtrlID[index]);
			if (Iterator != m_UIControls.end())
			{
				CShImageCombo *pImageCombo = static_cast<CShImageCombo*>(Iterator->second);
				pImageCombo->SetTabDefaults(MSCtrlID[index], hDlg);
			}
			index++;
		}
		//<S><C> To Fix Feedback#23,20210222,SSDI: Chanchal Singla
		// update default value based on GPD feature default values
		if (m_DefaultSettings[FEATURE_MARGINSHIFT].iDevmodeValue == marginshift_04)
		{
			m_ps_sub->wMargin = marginshift_04;
		}
		else if(m_DefaultSettings[FEATURE_MARGINSHIFT].iDevmodeValue == marginshift_08)
		{
			m_ps_sub->wMargin = marginshift_08;
		}
		else if (m_DefaultSettings[FEATURE_MARGINSHIFT].iDevmodeValue == marginshift_12)
		{
			m_ps_sub->wMargin = marginshift_12;
		}
		else
		{
			m_ps_sub->wMargin = marginshift_none;
		}
		//<E> To Fix Feedback#23,20210222,SSDI: Chanchal Singla
		
		SHORT wOri = GetOrientation(m_pFeatureInfoList);
		if (wOri == ori_port)
		{
			m_ps_sub->wBinEdge = binedge_left;
		}
		else {
			m_ps_sub->wBinEdge = binedge_top;
		}

		//-- Get Margin Shift and Update Propstate
		GetMarginShiftInfo(0, &m_ps_sub->wMarginUnit, &m_ps_sub->dwMarginWidth);
	}
	PropstatetoDevMode();
	SetTabState(hDlg);
	return hr;
}

//=============================================================================
// function
//      OnEnKillFocus
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
//      Handles the EN_KILLFOCUS events.
//=============================================================================
LRESULT CShPrintPosition::OnEnKillFocus(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	
	switch (wCtrlID)
	{
	
	case IDC_EDT_PPOS_ODD_X:
	case IDC_EDT_PPOS_ODD_Y:
	case IDC_EDT_PPOS_EVN_X:
	case IDC_EDT_PPOS_EVN_Y:
	{
		m_ps_sub->lPPosOddX = GetEditVal(hWnd, IDC_EDT_PPOS_ODD_X);
		m_ps_sub->lPPosOddY = GetEditVal(hWnd, IDC_EDT_PPOS_ODD_Y);
		m_ps_sub->lPPosEvnX = GetEditVal(hWnd, IDC_EDT_PPOS_EVN_X);
		m_ps_sub->lPPosEvnY = GetEditVal(hWnd, IDC_EDT_PPOS_EVN_Y);
	}
		break;
	case IDC_EDT_MARGINSFT_W:
		m_ps_sub->dwMarginWidth = GetEditVal(hWnd, wCtrlID);
		break;
	default:
		return WindowProcedure(hWnd, WM_COMMAND, (WPARAM)MAKEWPARAM(wCtrlID, EN_KILLFOCUS), (LPARAM)hWndCtrl);
	}
	SetTabState(hWnd, wCtrlID);
	return TRUE;
}

LRESULT CShPrintPosition::OnEnSetFocus(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	switch (wCtrlID)
	{
	case IDC_EDT_PPOS_ODD_X:
	case IDC_EDT_PPOS_ODD_Y:
	case IDC_EDT_PPOS_EVN_X:	
	case IDC_EDT_PPOS_EVN_Y:
	case IDC_EDT_MARGINSFT_W:
		::SendMessage(::GetDlgItem(hWnd, wCtrlID), EM_SETSEL, (WPARAM)0, (LPARAM)(-1));
		break;

	default:
		return WindowProcedure(hWnd, WM_COMMAND, (WPARAM)MAKEWPARAM(wCtrlID, EN_SETFOCUS), (LPARAM)hWndCtrl);
	}

	return TRUE;
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
LRESULT CShPrintPosition::OnEnChange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	switch (wCtrlID)
	{
	case IDC_EDT_PPOS_ODD_X:
	case IDC_EDT_PPOS_ODD_Y:
	case IDC_EDT_PPOS_EVN_X:
	case IDC_EDT_PPOS_EVN_Y:
	case IDC_EDT_MARGINSFT_W:
	{
		pControl = (*pComponents)[wCtrlID];
		((CShEditPPOS*)pControl)->OnEnChange(wCtrlID, hWnd);
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
LRESULT CShPrintPosition::OnWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
	case CBN_SELCHANGE:
	{
		OnCbnSelchange(hWnd, hWndCtrl, wCtrlId);
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
	default:
		break;
	}
	return TRUE;
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

DWORD CShPrintPosition::GetEditVal(HWND hWnd, INT wCtrlID)
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
		slog.putLog("CShPrintPosition::SetEditValue -> NULL_POINTER(IDD_DLG_PRINTPOSITON)\n");
		return FALSE;
	}
	bRetVal = ((CShEditPPOS*)pControl)->getval(hWnd, wCtrlID);

	return bRetVal;
}

//=============================================================================
// function
//      ChangedUnit
//
// parameters
//      HWND hDlg           Handle to window,
//		short wPreUnit      Previous Unit,
//		short wUnit			Current  Unit,
//
// return value
//       TRUE
//       FALSE
//
// outline
//      change the unit inch to mm/ mm to inch
//=============================================================================
BOOL CShPrintPosition::PPOS_ChangedUnit(HWND hDlg, short wPreUnit)
{
	BOOL			blRet = FALSE;
	short			wBefore;
	
	wBefore = wPreUnit;
	
	UINT index = 0;
	UINT wCtrlID[4] = { IDC_EDT_PPOS_ODD_X , IDC_EDT_PPOS_ODD_Y,IDC_EDT_PPOS_EVN_X,IDC_EDT_PPOS_EVN_Y };
	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	
	while (index < 4)  // initialize the unit(inch/mm) and  edit box 
	{
		pControl = (*pComponents)[wCtrlID[index]];
		((CShEditPPOS*)pControl)->SetUnit(m_ps_sub->wPPosUnit);
		((CShEditPPOS*)pControl)->OnInit(wCtrlID[index], hDlg);
		index++;
	}

	if (wBefore != m_ps_sub->wPPosUnit)
	{
		if (m_ps_sub->wPPosUnit == unit_inch)
		{
			m_ps_sub->lPPosOddX = MilliToInch_PPos(m_ps_sub->lPPosOddX);
			m_ps_sub->lPPosOddY = MilliToInch_PPos(m_ps_sub->lPPosOddY);
			m_ps_sub->lPPosEvnX = MilliToInch_PPos(m_ps_sub->lPPosEvnX);
			m_ps_sub->lPPosEvnY = MilliToInch_PPos(m_ps_sub->lPPosEvnY);
		}
		else
		{
			m_ps_sub->lPPosOddX = InchToMilli_PPos(m_ps_sub->lPPosOddX);
			m_ps_sub->lPPosOddY = InchToMilli_PPos(m_ps_sub->lPPosOddY);
			m_ps_sub->lPPosEvnX = InchToMilli_PPos(m_ps_sub->lPPosEvnX);
			m_ps_sub->lPPosEvnY = InchToMilli_PPos(m_ps_sub->lPPosEvnY);
		}
	}


	blRet = TRUE;
	return blRet;
}


//=============================================================================
// function
//      ChangedUnit
//
// parameters
//      HWND hDlg           Handle to window,
//		short wPreUnit      Previous Unit,
//		short wUnit			Current  Unit,
//
// return value
//       TRUE
//       FALSE
//
// outline
//      change the unit inch to mm/ mm to inch
//=============================================================================
BOOL CShPrintPosition::MS_ChangedUnit(HWND hDlg, short wPreUnit)
{
	BOOL			blRet = FALSE;

	short			wBefore;
	short			wDPI = 600;

	wBefore = wPreUnit;

	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();

	pControl = (*pComponents)[IDC_EDT_MARGINSFT_W];
	((CShEditPPOS*)pControl)->SetUnit(m_ps_sub->wMarginUnit);
	((CShEditPPOS*)pControl)->OnInit(IDC_EDT_MARGINSFT_W, hDlg);
	
	

	if (wBefore != m_ps_sub->wMarginUnit)
	{
		if (m_ps_sub->wMarginUnit == unit_inch)
			m_ps_sub->dwMarginWidth = MilliToInch_MrgnSft(m_ps_sub->dwMarginWidth);
		else
			m_ps_sub->dwMarginWidth = InchToMilli_MrgnSft(m_ps_sub->dwMarginWidth);
	}

	blRet = TRUE;

	return blRet;
}

//=============================================================================
// function
//       GetMarginShiftInfo
//
// parameters
//
//		short		wMargin,
//		short		*pwUnit,
//		DWORD		*pdwWidth
//
// return value
//       TRUE
//       FALSE
//
// outline
//      GetMarginShiftInfo
//=============================================================================
BOOL CShPrintPosition::GetMarginShiftInfo(short wMargin, short *pwUnit, DWORD *pdwWidth)
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
	MARGINSHIFTDATA	msd;


	if (pwUnit == NULL || pdwWidth == NULL)
		goto EXIT;

	preg = new CShRegMS(m_hStringResourceHandle, m_PszPrnName);
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		blRet = pjsonms->ReadMSData(wMargin, &msd);
		if (blRet != TRUE)
		{
			(*pjsonms).reset();
			(*pjsonms).ReadMSData(wMargin, &msd);
		}
	}
	else
	{
		blRet = preg->ReadMSData(m_PszPrnName, wMargin, &msd);
		if (blRet != TRUE)
		{
			(*preg).reset(m_PszPrnName);
			(*preg).ReadMSData(m_PszPrnName, wMargin, &msd);
		}
	}

	*pwUnit = (short)msd.dwUnit;
	if (*pwUnit == unit_inch)
	{
		*pdwWidth = msd.dwInch;
	}
	else
	{
		*pdwWidth = msd.dwMilli;
	}
	blRet = TRUE;

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


//=============================================================================
// function
//      GetDefMarginShift
//
// parameters
//      wMargin             Margin Shift index
//      wMarginUnit         Margin Shift Unit
//
// return value
//      Margin Shift Width
//
// outline
//     GetDefMarginShift
//=============================================================================
short CShPrintPosition::GetDefMarginShift(short wMargin, short wMarginUnit)
{
	short			wMarginWidth = 0;
	long			lCount;
	long FAR		*pl = NULL;
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

	long			i;
	preg = new CShRegMS(m_hStringResourceHandle, m_PszPrnName);
	if (preg == NULL)
	{
		goto EXIT;
	}
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		if ((lCount = pjsonms->GetLongArray()) != 0)
		{
			pl = pjsonms->GetLongArrayData();

			for (i = 0; i < lCount; i += INI_NUM_MCF_SUP_MGN)
			{
				if (pl[i] == (long)wMargin)
				{
					if (wMarginUnit == unit_mm)
					{
						wMarginWidth = (short)pl[i + 1];

					}
					else
					{
						wMarginWidth = (short)pl[i + 2];
					}
					break;
				}
			}
		}
	}
	else
	{
		if ((lCount = preg->GetLongArray()) != 0)
		{
			pl = preg->GetLongArrayData();

			for (i = 0; i < lCount; i += INI_NUM_MCF_SUP_MGN)
			{
				if (pl[i] == (long)wMargin)
				{
					if (wMarginUnit == unit_mm)
					{
						wMarginWidth = (short)pl[i + 1];

					}
					else
					{
						wMarginWidth = (short)pl[i + 2];
					}
					break;
				}
			}
		}
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
	return wMarginWidth;
}

//=============================================================================
// function
//      PropstatetoDevMode
//
// parameters
//     None
//
// return value
//      
//
// outline
//     PropstatetoDevMode
//=============================================================================
BOOL CShPrintPosition::PropstatetoDevMode()
{
	// --- byMarginShift
	m_pTempOemPrivateDevMode->scpi.ext.byMgnShift = (BYTE)m_ps_sub->wMargin;

	// --- byBindedge
	//m_pTempOemPrivateDevMode->scpi.ext.byBindedge = (BYTE)m_ps_sub->wBinEdge;

	return TRUE;
}

//= ============================================================================
// function
//      GetDefPrintPosition
//
// parameters
//
// return value
//
// outline
//
//=============================================================================
BOOL CShPrintPosition::GetDefPrintPosition(HWND hDlg)
{
	BOOL			blRet = FALSE;
	PRINTPOSITION ppd;
	short	wSel;
	CShRegPP *pregPP = NULL;
	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_PszPrnName, szCommonDatFilePath, FALSE);
	CShJsonPP	*pjsonpp = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonpp = new CShJsonPP(ghInstance, m_PszPrnName);
		pjsonpp->Init();
	}
	pregPP = new CShRegPP(m_hStringResourceHandle, m_PszPrnName);
	if (pregPP == NULL)
	{
		goto EXIT;
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
		pjsonpp->GetPPDefData(&ppd);
	else
		pregPP->GetPPDefData(&ppd);

	wSel = m_ps_sub->wPPosUnit;

	// --- Odd X
	m_ps_sub->lPPosOddX = ppd.wOddX;

	// --- Odd Y
	m_ps_sub->lPPosOddY = ppd.wOddY;

	// --- Even X
	m_ps_sub->lPPosEvnX = ppd.wEvenX;

	// --- Even Y
	m_ps_sub->lPPosEvnY = ppd.wEvenY;

	// --- unit
	m_ps_sub->wPPosUnit = (short)ppd.dwUnit;

	PPOS_ChangedUnit(hDlg, wSel);
	PPOS_SetRangeText(hDlg,(ppd.dwUnit));
	PPOS_SetUnitText(hDlg,(short)ppd.dwUnit);

	blRet = TRUE;

EXIT:
	if (pregPP != NULL)
	{
		delete pregPP;
		pregPP = NULL;
	}
	if (pjsonpp != NULL) {
		delete pjsonpp;
		pjsonpp = NULL;
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
//     Select
//
// return value
//     BOOL
//
// outline
//     To set states of combo box.
//=============================================================================
BOOL CShPrintPosition::Select(HWND hDlg, WORD wCtrlID, long PatternID)
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
			slog.putLog("CShPrintPosition::Select -> NULL_OBJECT(pImageCombo)\n");
		}

	}
	return TRUE;
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
BOOL CShPrintPosition::SetEditValue(HWND hDlg, INT wCtrlID,long wValue)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	pControl = (*pComponents)[wCtrlID];		//set the edit box value 
	if (NULL == pControl)
	{
		slog.putLog("CShPrintPosition::SetEditValue -> NULL_POINTER(IDD_DLG_PRINTPOSITON)\n");
		return FALSE;
	}
	((CShEditPPOS*)pControl)->setval(hDlg, wCtrlID, wValue);

	return TRUE;
}
//=============================================================================
// function
//      PPOS_GetPrintPositionData
//
// parameters
//	LPPRINTPOSITION			ppposd
// 
//	return value
//      TRUE 
//		FALSE
//
// outline
//      GetPrintPositionData
//=============================================================================
BOOL CShPrintPosition::PPOS_GetPrintPositionData(LPPRINTPOSITION ppposd)
{
	
	if (ppposd != NULL)
	{

		if (m_ps_sub->wPPosUnit == unit_mm) {
			(*ppposd).wOddX = (short)MilliToPixel_PPos(m_ps_sub->lPPosOddX);
			(*ppposd).wOddY = (short)MilliToPixel_PPos(m_ps_sub->lPPosOddY);
			(*ppposd).wEvenX = (short)MilliToPixel_PPos(m_ps_sub->lPPosEvnX);
			(*ppposd).wEvenY = (short)MilliToPixel_PPos(m_ps_sub->lPPosEvnY);
		}
		else if (m_ps_sub->wPPosUnit == unit_inch) {
			(*ppposd).wOddX = (short)InchToPixel_PPos(m_ps_sub->lPPosOddX);
			(*ppposd).wOddY = (short)InchToPixel_PPos(m_ps_sub->lPPosOddY);
			(*ppposd).wEvenX = (short)InchToPixel_PPos(m_ps_sub->lPPosEvnX);
			(*ppposd).wEvenY = (short)InchToPixel_PPos(m_ps_sub->lPPosEvnY);
		}
		(*ppposd).dwUnit = unit_pixel;
	}

	return TRUE;
}
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
VOID CShPrintPosition::ShowConstraintsOnBinEdgePosition(HWND hDlg, SHORT iCtrlID)
{

	//1. 2 Sided Printing Is None binding Position dropdown Should be display Left/Right/Top.
	//2. if Orientation is Landscape/LandScape(Rotated) then Binding Position dropdown Should be TOP 
	//   but if Orientation is Landscape/LandScape(Rotated) and  2 Sided Printing Is  Short Edge then binding Position dropdown Should be display Left/Right.
	//3. if Orientation is Portrait/Portrait(Rotated) then binding Position dropdown Should be display Left/Right.
	//   but if Orientation is Portrait/Portrait(Rotated) and  2 Sided Printing Is  Short Edge then binding Position dropdown Should be display Top.

	SHORT wOri = GetOrientation(m_pFeatureInfoList);
	//<S><C> To Fix Redmine bug #2610,17-08-2020,SSDI:Chanchal Singla
	//On Print Position Dialog (Margin-Shift), Position - Top is not displayed
	//<S><C> To fix feedback issue 20(sprint 16),2020.08.13 -SSDI:Goutham
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
			SetControlState(hDlg, IDC_STC_MARGINSFT_POS, SCUI_STATE_SHOW);
		}
	}
	else if (wOri == ori_port)
	{
		if (!strcmp("HORIZONTAL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
		{
			SetControlState(hDlg, iCtrlID, SCUI_STATE_SHOW);
			SetControlState(hDlg, IDC_STC_MARGINSFT_POS, SCUI_STATE_SHOW);
		}
		else
		{
			SetControlState(hDlg, iCtrlID, SCUI_STATE_ON);
		}
	}

	//<S><E> To fix feedback issue 20(sprint 16),2020.08.13 -SSDI:Goutham
	//<E> To Fix Redmine bug #2610,17-08-2020,SSDI:Chanchal Singla

}