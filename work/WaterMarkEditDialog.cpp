//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : WatermarkEditDialog.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : SUJAN K S
//
//  Date		  : 28 October 2013
//
//	Description   : This class Implements  WaterMark Edit Dialog Functionality
//   UI controls for check box, combo box, edit box and radio button and push button classes
//===========================================================================

//===========================================================================
//  Reviews 
//
//  Sl#     Review Date     Reviewer(s)               Remarks
//===========================================================================

#include "precomp.h"
#include "WaterMarkEditDialog.h"
#include "WaterMarkEditFont.h"
#include "WaterMarkComboChartSet.h"
#include "WaterMarkFontPreview.h"
#include "regkey.h"
//For Logging Purpose
#include "MFPLogger.h"

#ifdef _WIN64
#pragma pack(push, WATERMARK_EDIT_DIALOG, 8)
#endif

#include <commdlg.h>

#ifdef _WIN64
#pragma pack(pop, WATERMARK_EDIT_DIALOG, 8)
#endif


//=============================================================================
// function
//      CWatermarkEditDialog
//
// parameters
//      hStringResourceHandle - Handle to String Resource
//		uControlID	- Control ID stored in resource file	
//		WaterMarkDataVector -> Vector holds all the WaterMark Data Structures
//		wmIndex -> Updated index in the WaterMark Combo is strored.
// return value
//     NILL
//
// outline
//      Constructor to the class CWatermarkEditDialog.
//=============================================================================
//<S><C> To fix Coverity issue 90609, Harika	20201211
CWatermarkEditDialog::CWatermarkEditDialog(UINT uContorlID,HMODULE hStringResourceHandle, HMODULE BitmapResourceHandle,
	ResourceIDInfoMap* pResourceIDInfoMap, PFEATUREINFOLIST &pFeatureInfoList, std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings, 
	const WATERMARKDATA &WmData, COLORREF* pCustomColor, short sWMIndex, DWORD dwLangID, 
	HANDLE hPrinter, CWaterMarkFileData* pWaterMarkFileData, WCHAR *PszPrnName):
										   m_wColor(wmcolor_black),
										   m_pWaterMarkFileData(pWaterMarkFileData),
										   CUIControl(uContorlID,hStringResourceHandle, hPrinter)
//<E> To fix Coverity issue 90609, Harika	20201211
{
	m_hBitmapResourceHandle = BitmapResourceHandle;
	m_pResourceIDInfoMap = pResourceIDInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	m_DefaultSettings = pDefaultSettings;
	memcpy_s(&m_WmData,sizeof(m_WmData),&WmData,sizeof(WmData));
    m_pCustomColor = pCustomColor;
    m_sWMIndex = sWMIndex;
    m_dwLangID = dwLangID;

	//Coverity Fix 90518 - SSDI:Seetharam - 20200828
	m_wRGBValue = 0;
	m_PszPrnName = PszPrnName;
}


//=============================================================================
// function
//      CWatermarkEditDialog
//
// parameters
//      NILL            
//
// return value
//     NILL
//
// outline
//      Destructure to the class CWatermarkEditDialog.
//=============================================================================
CWatermarkEditDialog::~CWatermarkEditDialog()
{
	DestroyUIComponents();
}

/*++

Routine Name:

    CWatermarkEditDialog::OnWmInitdialog

Routine Description:

	This Functions Initialises Dialog Controls

Arguments:

		None

Return Value:

    TRUE/FALSE

--*/

LRESULT CWatermarkEditDialog::OnWmInitdialog(void)
{
	LRESULT result = 1;
	m_wColor = GetColor(m_WmData.colorRef);
	m_wRGBValue = GetRGBValuesFromColorRef(m_WmData.colorRef, m_wColor);
	SetTitle();
	SetControlStrings();
	CreateControlClass();
	InitializeUIControls(m_hWnd);
	SetTabState(m_hWnd);
	return result;
}

/*++
Routine Name:

    CWatermarkEditDialog::SetTitle

Routine Description:

    Routine for Setting the Title for the dialogs

Arguments:

    None

Return Value:

    TRUE/FALSE
   
--*/
BOOL CWatermarkEditDialog::SetTitle(void)
{
	TCHAR szItem[MAX_PATH]={0};
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (LoadString(m_hStringResourceHandle, m_uControlID, szItem, countof(szItem)) > 0)
	{
		SetWindowText(m_hWnd,szItem);
	}
	else
	{
		slog.putLog("CWatermarkEditDialog::SetTitle -> ERROR_LOAD_STRING\n");
	}
	return TRUE ;

}

/*++
Routine Name:

    CWatermarkEditDialog::SetTitle

Routine Description:

    Routine for Setting the Strings For all Static Conytrols

Arguments:

    None

Return Value:

    None
   
--*/


void CWatermarkEditDialog::SetControlStrings(void)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	CUIControl* pControl = new CStaticText(IDC_STC_WME_FONTNAME,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WME_FONTNAME, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_STC_WME_FONTNAME)\n");
	}
	pControl = new CStaticText(IDC_STC_WME_SCRIPT,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WME_SCRIPT, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_STC_WME_SCRIPT)\n");
	}
	pControl = new CStaticText(IDC_STC_WME_SHADING,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WME_SHADING, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_STC_WME_SHADING)\n");
	}
	pControl = new CStaticText(IDC_STC_WME_SHADING_VAL,m_hStringResourceHandle,(SHORT &)m_wRGBValue);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WME_SHADING_VAL, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_STC_WME_SHADING_VAL)\n");
	}
	pControl = new CStaticText(IDC_GRP_WME_FONT,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_GRP_WME_FONT, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_GRP_WME_FONT)\n");
	}
	pControl = new CStaticText(IDC_STC_WME_FONTCOLOR,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WME_FONTCOLOR, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_GRP_WME_FONTCOLOR)\n");
	}
	pControl = new CStaticText(IDC_STC_WME_BORDER,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WME_BORDER, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_STC_WME_BORDER)\n");
	}
	pControl = new CStaticText(IDC_STC_WME11_PRNPATTERN,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WME11_PRNPATTERN, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_STC_WME11_PRNPATTERN)\n");
	}
	/*pControl = new CStaticText(IDC_CHK_WME_BOLD,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_CHK_WME_BOLD, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_CHK_WME_BOLD)\n");
	}*/
	/*pControl = new CStaticText(IDC_CHK_WME_ITALIC,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_CHK_WME_ITALIC, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::SetControlStrings -> NULL_OBJECT(IDC_CHK_WME_ITALIC)\n");
	}*/
	// Ok Button
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
		slog.putLog("CWaterMarkAddDialog::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
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
		slog.putLog("CWaterMarkAddDialog::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}
}

/*++
Routine Name:

    CWatermarkEditDialog::CreateControlClass

Routine Description:

    Routine for Setting Creating the Controls for all the dynamic items in the dialog

Arguments:

    None

Return Value:

    None
   
--*/

void CWatermarkEditDialog::CreateControlClass(void)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	PRESOURCEIDINFO pResourceIDInfo = NULL;

	CUIControl* pControl = new CButton(IDC_BTN_WME_CUSCOLOR,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_BTN_WME_CUSCOLOR, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateControlClass -> NULL_OBJECT(IDC_BTN_WME_CUSCOLOR)\n");
	}

	pControl = new CWaterMarkEditFont(IDC_CMB_WME_FONTNAME,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_CMB_WME_FONTNAME, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(FALSE);
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateControlClass -> NULL_OBJECT(IDC_CMB_WME_FONTNAME)\n");
	}
	
	pControl = new CWaterMarkComboCharSet(&m_WmData.lfWatermark,IDC_CMB_WME_SCRIPT,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_CMB_WME_SCRIPT, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateControlClass -> NULL_OBJECT(IDC_CMB_WME_SCRIPT)\n");
	}

	//-- Font Color
	pResourceIDInfo = GetResourceIDInfoOfControl(WM_FONT_COLOR);
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_WM_FONT_COLOR],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_WM_FONT_COLOR],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_WM_FONT_COLOR],
		m_ObjCShowMessage,
		IDC_CMB_WME_FONTCOLOR
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_WME_FONTCOLOR, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateControlClass -> NULL_OBJECT(IDC_CMB_WME_FONTCOLOR)\n");
	}

	// Water Mark border
	pResourceIDInfo = GetResourceIDInfoOfControl(WM_BORDER);
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_WM_BORDER],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_WM_BORDER],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_WM_BORDER],
		m_ObjCShowMessage,
		IDC_CMB_WME_BORDER
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_WME_BORDER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateControlClass -> NULL_OBJECT(IDC_CMB_WME_BORDER)\n");
	}


	// Printing Pattern
	pResourceIDInfo = GetResourceIDInfoOfControl(WM_PRINTING_PATTERN);
	pControl = new CShImageCombo(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_WM_PRINTING_PATTERN],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_WM_PRINTING_PATTERN],
		m_hStringResourceHandle,
		m_hDialogResourceHandle,
		m_hBitmapResourceHandle,
		m_DefaultSettings[FEATURE_WM_PRINTING_PATTERN],
		m_ObjCShowMessage,
		IDC_CMB_WME11_PRNPATTERN
	);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_CMB_WME11_PRNPATTERN, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateControlClass -> NULL_OBJECT(IDC_CMB_WME11_PRNPATTERN)\n");
	}

    pControl = new CWaterMarkFontPreview(IDC_BTN_WM_FONT_PREV,m_hStringResourceHandle,m_dwLangID, m_PszPrnName);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_BTN_WM_FONT_PREV, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateControlClass -> NULL_OBJECT(IDC_BTN_WM_FONT_PREV)\n");
	}
	pControl = new CWaterMarkScrollBar(m_wRGBValue,0,255,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		 hr = AddUIControl(IDC_SCR_WME_SHADING, pControl);
		 if(S_OK != hr)
		 {
			slog.LoggingErrorHResultValues();
		 }
    }
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateControlClass -> NULL_OBJECT(CWaterMarkScrollBar)\n");
	}

	//Bold Checkbox
	pResourceIDInfo = GetResourceIDInfoOfControl(WMBOLD);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOLD],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOLD],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_BOLD],
		m_ObjCShowMessage
	);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_WME_BOLD, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}
	//Italic Checkbox
	pResourceIDInfo = GetResourceIDInfoOfControl(WMITALIC);
	pControl = new  CCheckBox(
		pResourceIDInfo,
		&m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ITALIC],
		&m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ITALIC],
		m_hStringResourceHandle,
		m_DefaultSettings[FEATURE_ITALIC],
		m_ObjCShowMessage
	);
	if (NULL !=pControl)
	{
		hr = AddUIControl(IDC_CHK_WME_ITALIC, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWatermarkEditDialog::CreateContorlClass -> NULL_POINTER(CCheckBox)\n");
	}
}

/*++
Routine Name:

    CWatermarkEditDialog::OnBnClicked

Routine Description:

    Routine for Button Click Events

Arguments:

    hWnd - handle to dialog
	hWndCtrl - handle to the Control
	wCtrlID - Control Resource ID

Return Value:

    LRESULT
   
--*/
LRESULT CWatermarkEditDialog::OnBnClicked(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	switch(wCtrlID)
	{
		
		case IDOK:
		case IDCANCEL:
			CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
			break;
		case IDC_CHK_WME_BOLD:
			{
				m_WmData.lfWatermark.lfWeight = ::IsDlgButtonChecked(hWnd,IDC_CHK_WME_BOLD) ? FW_BOLD : FW_NORMAL;
			}
			break;
		case IDC_CHK_WME_ITALIC:
			{
				m_WmData.lfWatermark.lfItalic = (BYTE)::IsDlgButtonChecked(hWnd,IDC_CHK_WME_ITALIC);
			}
			break;
		case IDC_BTN_WME_CUSCOLOR:
			OnWMCustomColor(hWnd, hWndCtrl);
			break;
		default:
			res = CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
	}
	SetTabState(hWnd);
	return res;
}

/*++
Routine Name:

    CWatermarkEditDialog::OnCbnSelchange

Routine Description:

    Routine for Changing the Values in the Combo Box
Arguments:

    hWnd - handle to dialog
	hWndCtrl - handle to the Control
	wCtrlID - Control Resource ID

Return Value:

    LRESULT
   
--*/
LRESULT CWatermarkEditDialog::OnCbnSelchange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	short			wFlag;

	switch(wCtrlID)
	{
		case IDC_CMB_WME_FONTNAME:
			{
				UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_WME_FONTNAME);
				if(Iterator != m_UIControls.end())
				{
					CWaterMarkEditFont *pImageCombo  = static_cast<CWaterMarkEditFont*>(Iterator->second);
					if(pImageCombo)
					{
						pImageCombo->Getdata(wCtrlID,hWnd,m_WmData.lfWatermark.lfFaceName, sizeof(m_WmData.lfWatermark.lfFaceName));
						UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_WME_SCRIPT);
						CWaterMarkComboCharSet *pImageCombo  = static_cast<CWaterMarkComboCharSet*>(Iterator->second);
						if(pImageCombo)
						{
							m_WmData.lfWatermark.lfCharSet = (BYTE)pImageCombo->Init(IDC_CMB_WME_SCRIPT,hWnd,&m_WmData.lfWatermark,IDS_ANSI_CHARSET);
						}
					}
				}
			}
			break;
		case IDC_CMB_WME_SCRIPT:
			{
				UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_WME_SCRIPT);
				if(Iterator != m_UIControls.end())
				{
					CWaterMarkComboCharSet *pImageCombo  = static_cast<CWaterMarkComboCharSet*>(Iterator->second);
					if(pImageCombo)
					{
						m_WmData.lfWatermark.lfCharSet = (BYTE)pImageCombo->GetVal(wCtrlID,hWnd);
					}
				}
			}
			break;

		case IDC_CMB_WME_FONTCOLOR:
			{
				UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_WME_FONTCOLOR);
				if(Iterator != m_UIControls.end())
				{
					CShImageCombo *pImageCombo  = static_cast<CShImageCombo*>(Iterator->second);
					if(pImageCombo)
					{
						m_wColor = pImageCombo->GetVal(wCtrlID,hWnd);
						m_WmData.colorRef = GetDefaultColor(m_wColor);
					}
				}
			}
			break;
		case IDC_CMB_WME_BORDER:
			{
				wFlag = DM_WM_DEC_CIRCLE | DM_WM_DEC_SQUARE;
				m_WmData.wFieldWMD &= ~wFlag;
				UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_WME_BORDER);
				if(Iterator != m_UIControls.end())
				{
					CShImageCombo *pImageCombo  = static_cast<CShImageCombo*>(Iterator->second);
					if(pImageCombo)
					{

						switch(pImageCombo->GetVal(wCtrlID,hWnd))
						{
						case wmboder_circle:
							m_WmData.wFieldWMD |= DM_WM_DEC_CIRCLE;
							break;

						case wmboder_rect:
							m_WmData.wFieldWMD |= DM_WM_DEC_SQUARE;
							break;

						default:
							break;
						}
					}
				}
			}
			break;
			case IDC_CMB_WME11_PRNPATTERN:
			{
				UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_WME11_PRNPATTERN);
				if(Iterator != m_UIControls.end())
				{
					CShImageCombo *pImageCombo  = static_cast<CShImageCombo*>(Iterator->second);
					if(pImageCombo)
					{
						 long PatternID = (short)pImageCombo->GetVal(wCtrlID,hWnd);
						 SetPrnPattern(PatternID);
					}
				}
				
			}
			break;
		default:
			res = CUIControl::OnCbnSelchange(hWnd, hWndCtrl, wCtrlID);
			break;
	}
	SetTabState(hWnd);
	return res;
}
LRESULT CWatermarkEditDialog::OnWmHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LRESULT			res = 0;
	INT wCtrlID = GetDlgCtrlID((HWND)lParam);
	switch(wCtrlID)
	{
		case IDC_SCR_WME_SHADING:
			{
				UIControlMap::const_iterator Iterator = m_UIControls.find(wCtrlID);
				if(Iterator != m_UIControls.end())
				{
					CWaterMarkScrollBar *pWaterMarkScrollBar = static_cast<CWaterMarkScrollBar*>(Iterator->second);
					pWaterMarkScrollBar->OnScroll(wCtrlID,LOWORD(wParam),hWnd,HIWORD(wParam));
					GetColorRefFromRGBValues();
					UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_STC_WME_SHADING_VAL);
					if(Iterator != m_UIControls.end())
					{
						CStaticText *pStaticTect = static_cast<CStaticText*>(Iterator->second);
						pStaticTect->OnInit(IDC_STC_WME_SHADING_VAL,hWnd);
					}
				}
			}
		break;
		default: 
			break;
	}
	res = CUIControl::OnWmHScroll(hWnd, wParam, lParam);
	SetTabState(hWnd);
	return res;
}
/*++

Routine Name:

    CUIControl::OnWmDrawItem

Routine Description:

	This Functions Processes all the Drawing Calls

Arguments:

		HWND hWnd, WPARAM wParam, LPARAM lParam

Return Value:

    TRUE/FALSE

--*/
LRESULT CWatermarkEditDialog::OnWmDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	short				wCtrlID = (short)wParam;
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;

	UIControlMap::const_iterator Iterator;
	switch (wCtrlID)
	{
	case IDC_BTN_WM_FONT_PREV:
	{
		Iterator = m_UIControls.find(IDC_BTN_WM_FONT_PREV);
		if (Iterator != m_UIControls.end())
		{
			CWaterMarkFontPreview *pButton = static_cast<CWaterMarkFontPreview*>(Iterator->second);
			if (pButton)
			{
				pButton->Draw((*pdis).hDC, &(*pdis).rcItem, &m_WmData, m_sWMIndex);
			}
		}
	}
	break;
	 case IDC_CMB_WME_FONTNAME:
	{
		Iterator = m_UIControls.find(IDC_CMB_WME_FONTNAME);
		if (Iterator != m_UIControls.end())
		{
			CWaterMarkEditFont *pControl = static_cast<CWaterMarkEditFont*>(Iterator->second);
			if (pControl)
			{
				((CWaterMarkEditFont *)pControl)->Draw(hWnd, wParam, lParam);
			}
			else
			{
				slog.putLog("CWaterMarkDialog::OnWmDrawItem -> NULL_OBJECT(pControl)\n");
			}
		}
	}
	break;
	 case IDC_CMB_WME_SCRIPT:
	 {
		 Iterator = m_UIControls.find(IDC_CMB_WME_SCRIPT);
		 if (Iterator != m_UIControls.end())
		 {
			 CShImageCombo *pControl = static_cast<CShImageCombo*>(Iterator->second);
			 if (pControl)
			 {
				 ((CShImageCombo *)pControl)->OnWmDrawItem(hWnd, wParam, lParam);
			 }
			 else
			 {
				 slog.putLog("CWaterMarkDialog::OnWmDrawItem -> NULL_OBJECT(pControl)\n");
			 }
		 }
	 }
	 break;
	 case IDC_CMB_WME_FONTCOLOR:
	 {
		 Iterator = m_UIControls.find(IDC_CMB_WME_FONTCOLOR);
		 if (Iterator != m_UIControls.end())
		 {
			 CShImageCombo *pControl = static_cast<CShImageCombo*>(Iterator->second);
			 if (pControl)
			 {
				 ((CShImageCombo *)pControl)->OnWmDrawItem(hWnd, wParam, lParam);
			 }
			 else
			 {
				 slog.putLog("CWaterMarkDialog::OnWmDrawItem -> NULL_OBJECT(pControl)\n");
			 }
		 }
	 }
	 break;
	 case IDC_CMB_WME_BORDER:
	 {
		 Iterator = m_UIControls.find(IDC_CMB_WME_BORDER);
		 if (Iterator != m_UIControls.end())
		 {
			 CShImageCombo *pControl = static_cast<CShImageCombo*>(Iterator->second);
			 if (pControl)
			 {
				 ((CShImageCombo *)pControl)->OnWmDrawItem(hWnd, wParam, lParam);
			 }
			 else
			 {
				 slog.putLog("CWaterMarkDialog::OnWmDrawItem -> NULL_OBJECT(pControl)\n");
			 }
		 }
	 }
	 break;
	 case IDC_CMB_WME11_PRNPATTERN:
	 {
		 Iterator = m_UIControls.find(IDC_CMB_WME11_PRNPATTERN);
		 if (Iterator != m_UIControls.end())
		 {
			 CShImageCombo *pControl = static_cast<CShImageCombo*>(Iterator->second);
			 if (pControl)
			 {
				 ((CShImageCombo *)pControl)->OnWmDrawItem(hWnd, wParam, lParam);
			 }
			 else
			 {
				 slog.putLog("CWaterMarkDialog::OnWmDrawItem -> NULL_OBJECT(pControl)\n");
			 }
		 }
	 }
	 break;
	default:
		break;
	}

	return CUIControl::OnWmDrawItem(hWnd, wParam, lParam);
}

/*++
Routine Name:

    CWatermarkEditDialog::OnWMCustomColor

Routine Description:

    Routine for Button click for CustomColor dialog and its settings to be saved

Arguments:

    hWnd - handle to dialog
	hWndCtrl - handle to the Control
	wCtrlID - Control Resource ID

Return Value:

    LRESULT
   
--*/
LRESULT CWatermarkEditDialog::OnWMCustomColor(HWND hWnd, HWND hWndCtrl)
{
	UNREFERENCED_PARAMETER(hWndCtrl);

	TCHAR szWaterMarkDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szWaterMarkDatFilePath, L"wm62.dat");
	CShIniFile* cshinifile = new CShIniFile(ghInstance, m_pPrinterName, szWaterMarkDatFilePath, FALSE);
	CShJsonWm* m_pjsonwm = new CShJsonWm(ghInstance, m_pPrinterName, m_hStringResourceHandle);
	m_pjsonwm->Init();

	LRESULT			res = TRUE;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

    if (NULL == m_pCustomColor)
    {
		slog.putLog("CWatermarkEditDialog::OnWMCustomColor -> NULL_OBJECT(m_pCustomColor)\n");
        return res;
    }

    COLORREF color = m_WmData.colorRef;
    COLORREF colorCustom[16] = {0};
	CRegistryAPI regApi;
	DWORD			dwSize;

	WCHAR			szRegPath[512];
	SecureZeroMemory(szRegPath, sizeof(szRegPath));

	regApi.CreateRegPath(REG_KEY_WM_ROOT_BASE, szRegPath, sizeof(szRegPath), m_PszPrnName);

	dwSize = sizeof(colorCustom);
	if ((*cshinifile).IsWriteToJson() == true)
	{
		m_pjsonwm->ReadColor((WCHAR FAR *)m_pCustomColor,dwSize);
	}
	else
	{
		regApi.ReadRegData(HKEY_CURRENT_USER, szRegPath, REG_ENT_WM_CUSTOM_COLOR,
			(WCHAR FAR *)m_pCustomColor, &dwSize);
		
	}
	memcpy(colorCustom, m_pCustomColor, sizeof(colorCustom));

    CHOOSECOLOR	cc = {0};

    cc.lStructSize		= sizeof(CHOOSECOLOR);
    cc.hwndOwner		= hWnd;
    cc.hInstance		= NULL;
    cc.rgbResult		= color;
    cc.lpCustColors		= colorCustom;
    cc.Flags			= CC_RGBINIT;

	if (::ChooseColor(&cc) != 0)
	{
		m_WmData.colorRef = cc.rgbResult;
		m_wColor = wmcolor_custom;
		memcpy(m_pCustomColor, colorCustom, sizeof(colorCustom));
		//<S><A>to Fix Redmine #3085 Issue 2,20202912,SSDI:Chanchal Singla
		//Watermark: Preview of watermark displays wrong when trying to add a new Watermark
		WATERMARKDATA	wmd;
		SecureZeroMemory(&wmd, sizeof(WATERMARKDATA));
		if ((*cshinifile).IsWriteToJson() == TRUE)
			m_pjsonwm->GetWMDefDataEx(0, &wmd, cshinifile);
		else
			m_pWaterMarkFileData->GetWMDefData(0, &wmd);
		long wColor = GetColor(wmd.colorRef);
		m_wRGBValue = GetRGBValuesFromColorRef(wmd.colorRef, wColor);
		UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_SCR_WME_SHADING);
		if (Iterator != m_UIControls.end())
		{
			CWaterMarkScrollBar *pWaterMarkScrollBar = static_cast<CWaterMarkScrollBar*>(Iterator->second);
			pWaterMarkScrollBar->OnInit(IDC_SCR_WME_SHADING, hWnd);
		}
		Iterator = m_UIControls.find(IDC_STC_WME_SHADING_VAL);
		if (Iterator != m_UIControls.end())
		{
			CStaticText *pStaticTect = static_cast<CStaticText*>(Iterator->second);
			pStaticTect->OnInit(IDC_STC_WME_SHADING_VAL, hWnd);
		}
		
			if((*cshinifile).IsWriteToJson() == true)
			{
				
				m_pjsonwm->WriteColorData((WCHAR FAR *)m_pCustomColor);
			}
			else
			{
				regApi.WriteRegData(HKEY_CURRENT_USER, szRegPath, REG_ENT_WM_CUSTOM_COLOR, REG_BINARY,
					(WCHAR FAR *)m_pCustomColor, sizeof(colorCustom));
			}
		//<E>to Fix Redmine #3085 Issue 2,20202912,SSDI:Chanchal Singla
	}

	delete cshinifile;
	cshinifile = nullptr;

	delete m_pjsonwm;
	m_pjsonwm = nullptr;

    return res;
}

//=============================================================================
// function
//      SetTabState
//
// parameters
//		hWnd	-> Handle to the Dialog
//
// return value
//    BOOL TRUE/FALSE
//
// outline
//      This function Sets the Current Tab/Dialog State for USer Selection
//=============================================================================


void CWatermarkEditDialog::SetTabState(HWND hWnd)
{
	INT cStatus = SCUI_STATE_ON;
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_CMB_WME11_PRNPATTERN);
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(Iterator != m_UIControls.end())
	{
		long PatternID = GetPrnPattern();
		CShImageCombo *pImageCombo  = static_cast<CShImageCombo*>(Iterator->second);
		if(pImageCombo)
		{
			pImageCombo->Select(IDC_CMB_WME11_PRNPATTERN,hWnd,PatternID);
		}
		else
		{
			slog.putLog("CWatermarkEditDialog::SetTabState -> NULL_OBJECT(pImageCombo)\n");
		}
	}

	Iterator = m_UIControls.find(IDC_CMB_WME_BORDER);
	if(Iterator != m_UIControls.end())
	{
		short wVal = wmboder_none;
		if(m_WmData.wFieldWMD & DM_WM_DEC_CIRCLE)
			wVal = wmboder_circle;
		else if(m_WmData.wFieldWMD & DM_WM_DEC_SQUARE)
			wVal = wmboder_rect;
		else
			wVal = wmboder_none;

		CShImageCombo *pImageCombo  = static_cast<CShImageCombo*>(Iterator->second);
		if(pImageCombo)
		{
			pImageCombo->Select(IDC_CMB_WME_BORDER,hWnd,wVal);
		}
		else
		{
			slog.putLog("CWatermarkEditDialog::SetTabState -> NULL_OBJECT(pImageCombo)\n");
		}
	}

	Iterator = m_UIControls.find(IDC_CMB_WME_FONTCOLOR);
	if(Iterator != m_UIControls.end())
	{
		CShImageCombo *pImageCombo  = static_cast<CShImageCombo*>(Iterator->second);
		if(pImageCombo)
		{
			pImageCombo->Select(IDC_CMB_WME_FONTCOLOR,hWnd,m_wColor);
		}
		//<S><A> RedMine Ticket(#1003) Watermarks:Information Icon and Constraints issues. : 2014.2.24, SSDI:Gajanana D S
		cStatus = SCUI_STATE_ON;
		if(wmcolor_custom == m_wColor)
		{
			cStatus = SCUI_STATE_SHOW ;
		}
		SetControlState(m_hWnd,IDC_SCR_WME_SHADING,cStatus);
		SetControlState(m_hWnd,IDC_STC_WME_SHADING,cStatus);
		SetControlState(m_hWnd,IDC_STC_WME_SHADING_VAL,cStatus);
		//<E>  RedMine Ticket(#1003) Watermarks:Information Icon and Constraints issues. : 2014.2.24, SSDI:Gajanana D S
	}

	Iterator = m_UIControls.find(IDC_CMB_WME_SCRIPT);
	if(Iterator != m_UIControls.end())
	{
		CWaterMarkComboCharSet *pImageCombo  = static_cast<CWaterMarkComboCharSet*>(Iterator->second);
		if(pImageCombo)
		{
			pImageCombo->Select(IDC_CMB_WME_SCRIPT,hWnd,m_WmData.lfWatermark.lfCharSet);
		}
		else
		{
			slog.putLog("CWatermarkEditDialog::SetTabState -> NULL_OBJECT(pImageCombo)\n");
		}
	}

	Iterator = m_UIControls.find(IDC_CMB_WME_FONTNAME);
	if(Iterator != m_UIControls.end())
	{
		CWaterMarkEditFont *pImageCombo  = static_cast<CWaterMarkEditFont*>(Iterator->second);
		if(pImageCombo)
		{
			pImageCombo->Select(IDC_CMB_WME_FONTNAME,hWnd,m_WmData.lfWatermark.lfFaceName);
		}
		else
		{
			slog.putLog("CWatermarkEditDialog::SetTabState -> NULL_OBJECT(pImageCombo)\n");
		}
		//<S><A> RedMine Ticket(#1003) Watermarks:Information Icon and Constraints issues. : 2014.2.24, SSDI:Gajanana D S
		cStatus = SCUI_STATE_ON;
		if(1 == SendDlgItemMessage(m_hWnd,IDC_CMB_WME_SCRIPT,CB_GETCOUNT,0,0))
		{
			cStatus = SCUI_STATE_SHOW;
		}
		SetControlState(m_hWnd,IDC_CMB_WME_SCRIPT,cStatus);
		SetControlState(m_hWnd,IDC_STC_WME_SCRIPT,cStatus);
		//<E>  RedMine Ticket(#1003) Watermarks:Information Icon and Constraints issues. : 2014.2.24, SSDI:Gajanana D S
	}

	HWND hControl = ::GetDlgItem(hWnd,IDC_BTN_WM_FONT_PREV);
	HDC hDC = ::GetDC(hControl);
	RECT			rc = {0,0,0,0};
	::GetWindowRect(hControl, &rc);
	rc.right   -= rc.left;
	rc.bottom  -= rc.top;
	rc.left	    = 0;
	rc.top      = 0;
	Iterator = m_UIControls.find(IDC_BTN_WM_FONT_PREV);
	if(Iterator != m_UIControls.end())
	{
		CWaterMarkFontPreview *pButton  = static_cast<CWaterMarkFontPreview*>(Iterator->second);
		if(pButton)
		{
			pButton->Draw(hDC,&rc,&m_WmData, m_sWMIndex);
		}
		else
		{
			slog.putLog("CWatermarkEditDialog::SetTabState -> NULL_OBJECT(pButton)\n");
		}
	}
	if(hDC != NULL)
	{
		::ReleaseDC(hControl, hDC);
	}
	else
	{
		slog.putLog("CWatermarkEditDialog::SetTabState -> NULL_HANDLE(hDC)\n");
	}
	// --- bold
	CheckDlgButton(hWnd,IDC_CHK_WME_BOLD, m_WmData.lfWatermark.lfWeight == FW_BOLD ? BST_CHECKED : BST_UNCHECKED);

	// --- italic
	CheckDlgButton(hWnd,IDC_CHK_WME_ITALIC, m_WmData.lfWatermark.lfItalic);
	
}

//=============================================================================
// function
//      GetPrnPattern
//
// parameters
//	short wFieldWMD -> Flag to the Pattern ID
//
// return value
//    BOOL TRUE/FALSE
//
// outline
//      This function gets the Pattern ID based on User Selection
//=============================================================================

long CWatermarkEditDialog::GetPrnPattern()
{
	long	lRet = prnpattern_transparency;

	if(m_WmData.wFieldWMD & DM_WM_TRANSPARENT)
	{
		lRet = prnpattern_transparency1;
	}
	else if(m_WmData.wFieldWMD & DM_WM_TRANSPARENT2)
	{
		lRet = prnpattern_transparency2;
	}
	else if(m_WmData.wFieldWMD & DM_WM_PILE)
	{
		lRet = prnpattern_pile;
	}
	else if(m_WmData.wFieldWMD & DM_WM_OUTLINE)
	{
		lRet = prnpattern_outline;
	}

	return lRet;
}
//=============================================================================
// function
//      SetPrnPattern
//
// parameters
//	short wFieldWMD -> Flag to the Pattern ID
//
// return value
//    BOOL TRUE/FALSE
//
// outline
//      This function gets the Pattern ID based on User Selection
//=============================================================================
BOOL CWatermarkEditDialog::SetPrnPattern(long lPrnPttrn)
{
	m_WmData.wFieldWMD &= ~(DM_WM_TRANSPARENT | DM_WM_TRANSPARENT2 | DM_WM_PILE | DM_WM_OUTLINE);

	switch(lPrnPttrn)
	{
	case prnpattern_transparency1:
		m_WmData.wFieldWMD |= DM_WM_TRANSPARENT;
		break;

	case prnpattern_transparency2:
		m_WmData.wFieldWMD |= DM_WM_TRANSPARENT2;
		break;

	case prnpattern_pile:
		m_WmData.wFieldWMD |= DM_WM_PILE;
		break;

	case prnpattern_outline:
		m_WmData.wFieldWMD |= DM_WM_OUTLINE;
		break;

	default:
		break;
	}
	return TRUE;
}
//=============================================================================
// function
//      GetColor
//
// parameters
//	COLORREF color -> Color Reference Value for the WaterMark String
//
// return value
//    BOOL TRUE/FALSE
//
// outline
//      This function gets the Color based on User Selection
//=============================================================================

long CWatermarkEditDialog::GetColor(COLORREF color)
{
	long		wMode = wmcolor_black;
	
	BYTE		r, g, b;
	r = GetRValue(color);
	g = GetGValue(color);
	b = GetBValue(color);

	if(r == g && g == b && b == r)
		wMode = wmcolor_black;
	else if(r == 255 && b == g)
		wMode = wmcolor_red;
	else if(r == b && g == 255)
		wMode = wmcolor_green;
	else if(r == g && b == 255)
		wMode = wmcolor_blue;
	else if(g == 255 && b == 255)
		wMode = wmcolor_cyan;
	else if(r == 255 && b == 255)
		wMode = wmcolor_magenta;
	else if(r == 255 && g == 255)
		wMode = wmcolor_yellow;
	else
		wMode = wmcolor_custom;
	return wMode;
}

//=============================================================================
// function
//      GetDefaultColor
//
// parameters
//	color -> Color Value for the WaterMark String
//
// return value
//    BOOL TRUE/FALSE
//
// outline
//      This function returns ColorRef value based on User Selection
//=============================================================================
COLORREF CWatermarkEditDialog::GetDefaultColor(long wColor)
{
	COLORREF		color = 0;
	long preset_black [] = {0,0,0};
	long preset_red   [] = {255,0,0};
	long preset_green [] = {0,255,0};
	long preset_blue  [] = {0,0,255};
	long preset_cyan  [] = {0,255,255};
	long preset_magenta	[] = {255,0,255};
	long preset_yellow []  = {255,255,0};
	long preset_custom []  = {127,128,129};

	UNREFERENCED_PARAMETER(preset_custom);

    UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_SCR_WME_SHADING);
	if(Iterator != m_UIControls.end())
	{
		CWaterMarkScrollBar* pWMScroll = static_cast<CWaterMarkScrollBar*>(Iterator->second);
        SHORT sShadeValue = pWMScroll->GetScrollValue();

        if (wColor == wmcolor_custom)
			color = m_WmData.colorRef;
		else
			color = pWMScroll->GetValue(wColor, sShadeValue);
	}
	else
	{
		switch(wColor)
		{
		case wmcolor_black:
			color = RGB((BYTE)preset_black[0], (BYTE)preset_black[1], (BYTE)preset_black[2]);
			break;

		case wmcolor_red:
			color = RGB((BYTE)preset_red[0], (BYTE)preset_red[1], (BYTE)preset_red[2]);
			break;

		case wmcolor_green:
			color = RGB((BYTE)preset_green[0], (BYTE)preset_green[1], (BYTE)preset_green[2]);
			break;

		case wmcolor_blue:
			color = RGB((BYTE)preset_blue[0], (BYTE)preset_blue[1], (BYTE)preset_blue[2]);
			break;

		case wmcolor_cyan:
			color = RGB((BYTE)preset_cyan[0], (BYTE)preset_cyan[1], (BYTE)preset_cyan[2]);
			break;

		case wmcolor_magenta:
			color = RGB((BYTE)preset_magenta[0], (BYTE)preset_magenta[1], (BYTE)preset_magenta[2]);
			break;

		case wmcolor_yellow:
			color = RGB((BYTE)preset_yellow[0], (BYTE)preset_yellow[1], (BYTE)preset_yellow[2]);
			break;

		default:
			return m_WmData.colorRef;
		}
	}
	return color;
}

LPWATERMARKDATA CWatermarkEditDialog::GetWatermarkDataAddr(void)
{
	return &m_WmData;
}
SHORT CWatermarkEditDialog::GetRGBValuesFromColorRef(COLORREF color,long wColor)
{
	BYTE			r =0;
	BYTE g = 0;
	BYTE b = 0;
	short			wTarget = 0;
	
	r = GetRValue(color);
	g = GetGValue(color);
	b = GetBValue(color);

	switch(wColor)
	{
		case wmcolor_black:
			wTarget = (short)255 - r;
			break;

		case wmcolor_red:
			wTarget = (short)255 - g;
			break;

		case wmcolor_green:
			wTarget = (short)255 - r;
			break;

		case wmcolor_blue:
			wTarget = (short)255 - r;
			break;

		case wmcolor_cyan:
			wTarget = (short)255 - r;
			break;

		case wmcolor_magenta:
			wTarget = (short)255 - g;
			break;

		case wmcolor_yellow:
			wTarget = (short)255 - b;
			break;

		default:
		{
			WATERMARKDATA	wmd;
			SecureZeroMemory(&wmd, sizeof(WATERMARKDATA));
			m_pWaterMarkFileData->GetWMDefData(0, &wmd);
			long wColor = GetColor(wmd.colorRef);
			wTarget = GetRGBValuesFromColorRef(wmd.colorRef, wColor);
		}
			break;
	}
	return wTarget;
	//wRange = (short)(m_lMax - m_lMin);
	//wVal = ((wRange * wTarget) + 128) / 255;
}
void CWatermarkEditDialog::GetColorRefFromRGBValues()
{
		
	BYTE			r =0 ;
	BYTE g =0;
	BYTE b = 0 ;
	BYTE base = (BYTE)m_wRGBValue ;
	/*if(wShade < 0)
		wVal = (short)SendMessage(SBM_GETPOS, (WPARAM)0, (LPARAM)0);
	else
		wVal = wShade;

	wRange = (short)(m_lMax - m_lMin);
	base = (BYTE)(((255 * wVal) + (wRange / 2)) / wRange);*/
		

	switch(m_wColor)
	{
		case wmcolor_black:
			r = 255 - base;
			g = 255 - base;
			b = 255 - base;
			break;

		case wmcolor_red:
			r = 255;
			g = 255 - base;
			b = 255 - base;
			break;

		case wmcolor_green:
			r = 255 - base;
			g = 255;
			b = 255 - base;
			break;

		case wmcolor_blue:
			r = 255 - base;
			g = 255 - base;
			b = 255;
			break;

		case wmcolor_cyan:
			r = 255 - base;
			g = 255;
			b = 255;
			break;

		case wmcolor_magenta:
			r = 255;
			g = 255 - base;
			b = 255;
			break;

		case wmcolor_yellow:
			r = 255;
			g = 255;
			b = 255 - base;
			break;

		default:
			break ;
	}

	m_WmData.colorRef = RGB(r, g, b);
}

PRESOURCEIDINFO CWatermarkEditDialog::GetResourceIDInfoOfControl(std::string strFeatureName)
{
	ResourceIDInfoMap::iterator ResourceInfoIterator = m_pResourceIDInfoMap->find(strFeatureName);
	if (ResourceInfoIterator != m_pResourceIDInfoMap->end())
	{
		return &(ResourceInfoIterator->second);
	}
	return NULL;
}