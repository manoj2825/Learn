//	Copyright 2020(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//======================================================================================
//  File          : WaterMarkDialog.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Chanchal Parkash
//
//  Date		  : 02 July 2020
//
//	Description   : This class handles Water Mark dialog functions 
//========================================================================================

#include "precomp.h"
#include "sh_api.h"
#include "shJsonwm.h"
#include "..\..\BitmapResource\Include\resource.h"
#include "WaterMarkEditDialog.h"
//For Logging Purpose
#include "MFPLogger.h"
#include "windowsx.h"
#define MESSAGELENGTH 1024
#define MESSAGETITLELENGTH 768
#define MaximumNumberOfWaterMarks 100

//=============================================================================
// function
//      CWaterMarkDialog
//
// parameters
//      hStringResourceHandle - Handle to String Resource
//		uControlID	- Control ID stored in resource file
// return value
//     NILL
//
// outline
//      Constructor to the class CWaterMarkDialog.
//=============================================================================
CWaterMarkDialog::CWaterMarkDialog(
	UINT uContorlID,
	HMODULE hStringResourceHandle,
	HANDLE hPrinter,
	HMODULE	hDialogResourceHandle,
	WCHAR* pPrinterName,
	ResourceIDInfoMap* pResourceIDInfoMap,
	PFEATUREINFOLIST &pFeatureInfoList,
	std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings,
	HMODULE hBitmapResourceHandle,
	PDEVMODE pOemPublicDevMode,
	POEMDEV pTempOemPrivateDevMode,
	PaperInfoMap PaperInfo
) :CUIControl(uContorlID, hStringResourceHandle, hPrinter)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	m_pResourceIDInfoMap = pResourceIDInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	m_DefaultSettings = pDefaultSettings;
	m_hPrinterHandle = hPrinter;
	m_hBitmapResourceHandle = hBitmapResourceHandle;
	m_hDialogResourceHandle = hDialogResourceHandle;
	m_pOemPublicDevMode = pOemPublicDevMode;
	m_pTempOemPrivateDevMode = pTempOemPrivateDevMode;
	wcscpy_s(m_PszPrnName, INI_STR_MAX, pPrinterName); // Copy the printer name String to m_PszPrnName
	m_pPrinterName = pPrinterName;
	m_PaperInfo = PaperInfo;
	// get language ID 
	// get language id from registry
	m_dwLangID = GetLanguageID(hPrinter);
	m_pTempOemPrivateDevMode->scpi.ext2.Watermark.data.dwWMLangID = m_dwLangID;
	m_bIsWaterMarkTextEmpty = FALSE;
	m_pWatermarkregdata = NULL,
	m_dwSelectedPaperWidthPixel = 0;
	m_dwSelectedPaperHeightPixel = 0;
	m_sOrientation = 0;
	m_pWaterMarkFileData = NULL;
	m_pIniFile = NULL;
	m_bIsWaterMarkDefaultClicked = FALSE;
	m_pCustomColor = new COLORREF[16]();
	if (NULL == m_pCustomColor)
	{
		slog.putLog("CWaterMarkDialog::CWaterMarkDialog -> NULL_HANDLE(m_pCustomColor)\n");
	}
	SecureZeroMemory(&m_PropertySheetState, sizeof(PROPSTATE));
	SecureZeroMemory(&m_WaterMarkData, sizeof(WATERMARKDATA));
	SecureZeroMemory(&m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
	InitializeWaterMarkFileData();
	m_PropertySheetState.hcsHandUp = ::LoadCursor(m_hBitmapResourceHandle, MAKEINTRESOURCE(IDC_HAND_UP));
	m_PropertySheetState.hcsHandDown = ::LoadCursor(m_hBitmapResourceHandle, MAKEINTRESOURCE(IDC_HAND_DOWN));

	//Fixed Coverity issue - 90620 - SSDI:Seetharam-20200729
	m_hPropertySheetHandle = NULL;
	m_bWaterMarkChanged = FALSE;
	m_blForceUpdate = FALSE;
}
//=============================================================================
// function
//      CWaterMarkDialog
//
// parameters
//      NILL            
//
// return value
//     NILL
//
// outline
//      Destructure to the class CWaterMarkDialog.
//=============================================================================
CWaterMarkDialog::~CWaterMarkDialog()
{
	delete m_pWatermarkregdata;
	m_pWatermarkregdata = nullptr;

	delete m_pjsonwm;
	m_pjsonwm = NULL;

	if (m_pWaterMarkFileData)
	{
		delete m_pWaterMarkFileData;
		m_pWaterMarkFileData = NULL;
	}
	if (m_pIniFile)
	{
		delete m_pIniFile;
		m_pIniFile = NULL;
	}
	if (m_pCustomColor)
	{
		delete[] m_pCustomColor;
		m_pCustomColor = NULL;
	}
}

/*+
RouRoutine Name:

	CWaterMarkDialog::SetTitle

Routine Description:

	Routine for Setting the Title for the dialogs

Arguments:

	None

Return Value:

	TRUE/FALSE

--*/

BOOL CWaterMarkDialog::SetTitle(void)
{
	TCHAR szItem[MAX_PATH] = { 0 };
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (LoadString(m_hStringResourceHandle, m_uControlID, szItem, countof(szItem)) > 0)
	{
		SetWindowText(m_hWnd, szItem);
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetTitle -> ERROR_LOAD_STRING\n");
	}
	return TRUE;

}

PRESOURCEIDINFO CWaterMarkDialog::GetResourceIDInfoOfControl(std::string strFeatureName)
{
	ResourceIDInfoMap::iterator ResourceInfoIterator = m_pResourceIDInfoMap->find(strFeatureName);
	if (ResourceInfoIterator != m_pResourceIDInfoMap->end())
	{
		return &(ResourceInfoIterator->second);
	}
	return NULL;
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
CWaterMarkDialog::ActivateUIControl(
	_In_ CONST HWND hDlg
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	//
   // Use the wParam as the index into the control map and
   // inform the control of an activation
   //
	HRESULT hr = S_OK;
	ProcessFeatureInfoList();
	
	if ((FALSE == m_bIsWaterMarkDefaultClicked) && (0 == m_PropertySheetState.wWmIndex))
	{
		return S_OK;
	}
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
			slog.putLog("CWaterMarkDialog::ActivateUIControl -> NULL_POINTER(pControl)\n");
		}
	}
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
void CWaterMarkDialog::CreateControlClass(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
//	if ((*m_pIniFile).IsWriteToJson() == TRUE)
//	{
//		//m_pjsonwm = new CShJsonWm(ghInstance, m_pPrinterName, m_hStringResourceHandle);
//		//m_pjsonwm->Init();
//		m_pjsonwm->CShJsonWm::DealWithFavItemsInHKLM(m_pPrinterName);
//		m_pjsonwm->CShJsonWm::SelectTheCorrectFavItem(m_PropertySheetState.szWMNameHash, m_PropertySheetState.wWmIndex);
//	}
////<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
//	else
//	{
		m_pWatermarkregdata->DealWithFavItemsInHKLM(m_pPrinterName);
		m_pWatermarkregdata->SelectTheCorrectFavItem(m_pPrinterName, m_PropertySheetState.szWMNameHash, m_PropertySheetState.wWmIndex);
	//}
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	CUIControl* pControl = new CWaterMarkCombo(m_pWaterMarkFileData, m_pWatermarkregdata, m_PropertySheetState.wWmIndex, m_hStringResourceHandle,
		m_pPrinterName, m_PropertySheetState.szWM, m_bWaterMarkChanged);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CMB_WM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
		((CShImageCombo*)pControl)->SetDrawIcon(TRUE);
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(CWaterMarkCombo)\n");
	}

	pControl = new CWaterMarkScrollBar(m_PropertySheetState.wWmSize, 6, 300, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_SCR_WM_SIZE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(CWaterMarkScrollBar)\n");
	}

	pControl = new CWaterMarkScrollBar(m_PropertySheetState.wWmAngle, -90, 90, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_SCR_WM_ANGLE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(CWaterMarkScrollBar)\n");
	}

	pControl = new CWaterMarkScrollBar(m_PropertySheetState.lWmPosX, -50, 50, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_SCR_WM_HORZ, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(CWaterMarkScrollBar)\n");
	}

	pControl = new CWaterMarkScrollBar(m_PropertySheetState.lWmPosY, -50, 50, m_hStringResourceHandle, -1);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_SCR_WM_VERT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(CWaterMarkScrollBar)\n");
	}

	pControl = new CWaterMarkEdit(m_PropertySheetState.szWM, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_EDT_WM_TEXT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(CWaterMarkEdit)\n");
	}

	pControl = new CButton(IDC_BTN_WM_ADD, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_ADD, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(CWaterMarkEdit)\n");
	}
	
	pControl = new CButton(IDC_BTN_WM_DELETE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_DELETE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(CWaterMarkEdit)\n");
	}

	pControl = new CButton(IDC_BTN_WM_EDIT, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_EDIT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(IDC_BTN_WM_EDIT)\n");
	}

	pControl = new CButton(IDC_BTN_WM_CENTER, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_CENTER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(IDC_BTN_WM_CENTER)\n");
	}

	pControl = new CWaterMarkCheckBox(m_PropertySheetState.wOnFirstPage, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ON_1STPAGE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(CWaterMarkCheckBox)\n");
	}

	pControl = new CWaterMarkPreview(IDC_BTN_WM_PREV, m_hPropertySheetHandle, &m_PropertySheetState, m_pPrinterName, m_hStringResourceHandle, m_hPrinterHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_PREV, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::CreateContorlClass -> NULL_POINTER(IDC_BTN_WM_PREV)\n");
	}

}
//=============================================================================
// function
//      initializeWaterMarkFileData
//
// return value
//     NILL
//
// outline
//    Initialize WaterMark File Data.
//=============================================================================
void CWaterMarkDialog::InitializeWaterMarkFileData(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	WCHAR szWaterMarkDatFilePath[_MAX_PATH] = {};
	GetProjectFileName(szWaterMarkDatFilePath, L"wm62.dat");

	m_pIniFile = new CShIniFile(ghInstance, m_pPrinterName, szWaterMarkDatFilePath, FALSE);
	if (NULL == m_pIniFile)
	{
		slog.putLog("CWaterMarkDialog::InitializeWaterMarkFileData -> NULL_OBJECT(m_pIniFile)\n");
	}


	m_pWaterMarkFileData = new CWaterMarkFileData(m_hStringResourceHandle, m_pIniFile, m_dwLangID);
	if (NULL == m_pWaterMarkFileData)
	{
		slog.putLog("CWaterMarkDialog::InitializeWaterMarkFileData -> NULL_POINTER(m_pWaterMarkFileData)\n");
	}
}

//Routine Name:
//
//	CWaterMarkDialog::SetTabDefaults
//
//Routine Description:
//
//	The routine calls the SetTabDefaults() function of each control class
//
//Arguments:
//
//	hDlg - handle to the dialog
//
//Return Value:
//
//   S_OK,
//   return value from overridden SetTabDefaults()
//

HRESULT CWaterMarkDialog::SetTabDefaults(_In_ CONST INT   iCtrlID, _In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;

	m_pWatermarkregdata->ReadWMData(m_pPrinterName, 0 ,&m_WaterMarkData);

	memcpy_s(&m_WaterMarkDataChanged, sizeof(WATERMARKDATA), &m_WaterMarkData, sizeof(WATERMARKDATA));
	if (0 == m_PropertySheetState.wWmIndex)
	{
		SecureZeroMemory(m_PropertySheetState.szWM, sizeof(m_PropertySheetState.szWM));
	}
	WatermarkDataToPropState(&m_WaterMarkDataChanged);
	
	m_bIsWaterMarkDefaultClicked = TRUE;
	//Fixed Coverity issue - 90610 - SSDI:Seetharam-20200928
	hr = __super::SetTabDefaults(iCtrlID,hDlg);
	SaveFeatureOptions();
	return hr;
}



/*++

Routine Name:

	CWaterMarkDialog::OnWmInitdialog

Routine Description:

	This Functions Initialises Dialog Controls

Arguments:

		None

Return Value:

	TRUE/FALSE

--*/

LRESULT CWaterMarkDialog::OnWmInitdialog(void)
{
	LRESULT result = 1;

	SetTitle();
	GetWaterMarkRegistryObject();
	ProcessFeatureInfoList();
	if (0 == m_PropertySheetState.wWmIndex)
	{
		SecureZeroMemory(m_PropertySheetState.szWM, sizeof(m_PropertySheetState.szWM));
	}
    SetControlStrings();
	ActivateUIControl(m_hWnd);
	m_hPropertySheetHandle = m_hWnd;
	CreateControlClass();
	InitializeUIControls(m_hWnd);
	InitializeScrollBar();
	SetButtonState(m_hWnd);
	CreateDepenedentContolMap();
	
	return result;
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
void CWaterMarkDialog::SetControlStrings(void)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CUIControl* pControl = NULL;

	// --- preview
	
	pControl = new CStaticText(IDC_GRP_WM_PREV, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_GRP_WM_PREV, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_GRP_WM_PREV)\n");
	}
	
	pControl = new CStaticText(IDC_STC_WM_POS, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_POS, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_POS)\n");
	}
	
	pControl = new CStaticText(IDC_STC_WM_X, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_X, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_X)\n");
	}

	
	pControl = new CStaticText(IDC_STC_WM_Y, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_Y, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_Y)\n");
	}
	
	
	pControl = new CStaticText(IDC_BTN_WM_CENTER, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_CENTER, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_BTN_WM_CENTER)\n");
	}

	pControl = new CStaticText(IDC_STC_WM_SIZE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_SIZE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_SIZE)\n");
	}
	

	pControl = new CStaticText(IDC_STC_WM_ANGLE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_ANGLE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_ANGLE)\n");
	}
	
	
	pControl = new CStaticText(IDC_STC_WM_PREVIMG, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_PREVIMG, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_PREVIMG)\n");
	}

	
	pControl = new CStaticText(IDC_STC_WM_X_VAL, m_hStringResourceHandle, (SHORT&)m_PropertySheetState.lWmPosX);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_X_VAL, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_X_VAL)\n");
	}
	
	pControl = new CStaticText(IDC_STC_WM_Y_VAL, m_hStringResourceHandle, (SHORT&)m_PropertySheetState.lWmPosY);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_Y_VAL, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_Y_VAL)\n");
	}
	
	pControl = new CStaticText(IDC_STC_WM_SIZE_VAL, m_hStringResourceHandle, m_PropertySheetState.wWmSize);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_SIZE_VAL, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_SIZE_VAL)\n");
	}
	
	pControl = new CStaticText(IDC_STC_WM_ANGLE_VAL, m_hStringResourceHandle, m_PropertySheetState.wWmAngle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_ANGLE_VAL, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_ANGLE_VAL)\n");
	}
	

	// --- watermark
	
	pControl = new CStaticText(IDC_GRP_WM_WM, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_GRP_WM_WM, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_GRP_WM_WM)\n");
	}

	pControl = new CStaticText(IDS_IDC_STC_WM_TX, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDS_IDC_STC_WM_TX, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDS_IDC_STC_WM_TX)\n");
	}
	
	// Water Mark text
	pControl = new CStaticText(IDC_STC_WM_TEXT, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_STC_WM_TEXT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_STC_WM_TEXT)\n");
	}
	
	// Add Button
	pControl = new CStaticText(IDC_BTN_WM_ADD, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_ADD, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_BTN_WM_ADD)\n");
	}

	// Delete button
	pControl = new CStaticText(IDC_BTN_WM_DELETE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_DELETE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_BTN_WM_DELETE)\n");
	}
	
	// WM Edit Button
	pControl = new CStaticText(IDC_BTN_WM_EDIT, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_BTN_WM_EDIT, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_BTN_WM_EDIT)\n");
	}

	// --- on first page only
	pControl = new CStaticText(IDC_CHK_ON_1STPAGE, m_hStringResourceHandle);
	if (NULL != pControl)
	{
		hr = AddUIControl(IDC_CHK_ON_1STPAGE, pControl);
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_POINTER(IDC_CHK_ON_1STPAGE)\n");
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
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
	}

	// OK button
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
		slog.putLog("CWaterMarkDialog::SetControlStrings -> NULL_OBJECT(CStaticText)\n");
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
//Routine Name:
//
//	CWaterMarkDialog::OnCbnSelchange
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
LRESULT CWaterMarkDialog::OnCbnSelchange(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	OnComboboxSelectionChange(hDlg);
	m_blForceUpdate = TRUE;
	SetTabState(hDlg, wCtrlID);
	return S_OK;
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
LRESULT CWaterMarkDialog::OnBnClicked(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;
	switch (wCtrlID)
	{

	case IDC_BTN_WM_ADD:
		OnWaterMarkAdd(hWnd);
		break;
	case IDC_BTN_WM_DELETE:
		OnWaterMarkDelete(hWnd);
		m_blForceUpdate = TRUE;
		break;

	case IDC_BTN_WM_EDIT:
		OnWaterMarkEdit(hWnd);
		break;
	case IDC_BTN_WM_CENTER:
	{
		OnWaterMarkCenter();
		m_blForceUpdate = TRUE;
		PropSheet_Changed(GetParent(hWnd), hWnd);
	}
		break;

	case IDC_CHK_ON_1STPAGE:
		m_PropertySheetState.wOnFirstPage = (SHORT)IsDlgButtonChecked(hWnd, IDC_CHK_ON_1STPAGE);
		m_blForceUpdate = TRUE;
		break;
	case IDC_BTN_SHARED_OK:
	case IDOK:
	{
		OnBtnOK(hWnd, hWndCtrl, wCtrlID);
		m_blForceUpdate = TRUE;
		//<S><D>To fix #3088,20201612,SSDI:Chanchal Singla
		//Watermark dialog is closed when trying to unsave the entered text
		//SaveFeatureOptions();
		//::EndDialog(hWnd, wCtrlID);
		//<E>To fix #3088,20201612,SSDI:Chanchal Singla
	}
	break;

	case IDC_BTN_SHARED_CANCEL:
	case IDCANCEL:
	{
		OnBtnCancel(hWnd, hWndCtrl, wCtrlID);
		m_blForceUpdate = TRUE;
	//	CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
	}
		break;
	case IDC_BTN_SHARED_SUB_DEFAULT:
		SetTabDefaults(wCtrlID,hWnd);
		m_blForceUpdate = TRUE;
		break;
	default:
		res = CUIControl::OnBnClicked(hWnd, hWndCtrl, wCtrlID);
	
	}
	if (m_blForceUpdate != FALSE)
	{
		m_blForceUpdate = FALSE;
		SetTabState(hWnd, wCtrlID);
	}
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
LRESULT CWaterMarkDialog::OnEnChange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID)
{
	LRESULT			res = 0;

	UIControlMap* pComponents = NULL;
	CUIControl* pControl = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	switch (wCtrlID)
	{
	case IDC_EDT_WM_TEXT:
	{
		pControl = (*pComponents)[IDC_EDT_WM_TEXT];
		((CWaterMarkEdit*)pControl)->OnEnChange( wCtrlID, hWnd );
	}
	break;
	default:
		break;
	}
	res = 1;


	SetTabState(hWnd, wCtrlID);
	return res;
}

//=============================================================================
// function
//        OnBtnOK
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      hWndCtrl            handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//         TRUE
//         FALSE
//
// outline
//        OnBtnOK
//=============================================================================
LRESULT CWaterMarkDialog::OnBtnOK(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	HRESULT hr = S_OK;
	//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	CWaterMarkRegistryData		*pregwm = new CWaterMarkRegistryData(ghInstance, m_pPrinterName, m_hStringResourceHandle);
	WCHAR			szTitle[(SCUI_WMTITLE_MAX + 1) * 2] = { 0 };
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	BOOL blCancel = FALSE;
	LONG lRet = QuerySaveWmChanges(hDlg);
	switch (lRet)
	{
	case IDYES:

		if (FALSE == OnWaterMarkAdd(hDlg))
		{
			hr = E_FAIL;
		}
		SetTabState(hDlg, IDC_BTN_WM_ADD);
		memcpy_s(&m_WaterMarkData, sizeof(WATERMARKDATA), &m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
		WatermarkDataToPropState(&m_WaterMarkData);
		SaveFeatureOptions();
		break;
	case IDOK:
	{
		SaveFeatureOptions();
		// When user clicks OK , Property Page should remain active.
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
		return TRUE;
	}
	break;
	case IDCANCEL:
		blCancel = TRUE;

		break;
	case IDNO:

		memcpy_s(&m_WaterMarkDataChanged, sizeof(WATERMARKDATA), &m_WaterMarkData, sizeof(WATERMARKDATA));
		WatermarkDataToPropState(&m_WaterMarkData);
		if (0 == m_PropertySheetState.wWmIndex)
		{
			SecureZeroMemory(m_PropertySheetState.szWM, sizeof(m_PropertySheetState.szWM));
		}
		SaveFeatureOptions();
		break;
	default:
		SaveFeatureOptions();
		break;
	}
	if (TRUE == blCancel)
	{
		// When user clicks Cancel , Property Page should remain active.
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
		return TRUE;
	}

//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	if (m_PropertySheetState.wWmIndex > 0)
	{
		if ((*pregwm).GetTitle(m_pPrinterName, szTitle, sizeof(szTitle), m_PropertySheetState.wWmIndex) == TRUE)
		{
			memcpy(m_PropertySheetState.szWMName, szTitle, sizeof(szTitle));
			TransferNameToHash(m_PropertySheetState.szWMName, m_PropertySheetState.szWMNameHash);
			memcpy(m_pTempOemPrivateDevMode->scpi.ext2.stSubFavNames.szWMNameHash, m_PropertySheetState.szWMNameHash, sizeof(m_pTempOemPrivateDevMode->scpi.ext2.stSubFavNames.szWMNameHash));
		}
	}
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	return CUIControl::OnBnClicked(hDlg, hWndCtrl, wCtrlID);

}
//=============================================================================
// function
//        OnBtnCancel
//
// parameters
//      hWnd				handle to the Window that contains the control.
//      hWndCtrl            handle to the Window that contains the control.
//      wCtrlID             Control Id
//
// return value
//         TRUE
//         FALSE
//
// outline
//         OnBtnCancel
//=============================================================================
LRESULT CWaterMarkDialog::OnBtnCancel(HWND hDlg, HWND hWndCtrl, WORD wCtrlID)
{
	HRESULT hr = S_OK;

	BOOL blCancel = FALSE;
	LONG lRet = QuerySaveWmChanges(hDlg);
	switch (lRet)
	{
	case IDYES:

		if (FALSE == OnWaterMarkAdd(hDlg))
		{
			hr = E_FAIL;
		}
		SetTabState(hDlg, IDC_BTN_WM_ADD);
		memcpy_s(&m_WaterMarkData, sizeof(WATERMARKDATA), &m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
		WatermarkDataToPropState(&m_WaterMarkData);
		break;
	case IDOK:
	{
		// When user clicks OK , Property Page should remain active.
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
		return TRUE;
	}
	break;
	case IDCANCEL:
		blCancel = TRUE;
		break;
	case IDNO:
		break;
	default:
		break;
	}
	if (TRUE == blCancel)
	{
		// When user clicks Cancel , Property Page should remain active.
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, TRUE);
		return TRUE;
	}

	return CUIControl::OnBnClicked(hDlg, hWndCtrl, wCtrlID);
}
//=============================================================================
// function
//      QuerySaveWmChanges
//
// return value
//     NILL
//
// outline
//     Queries whether changes made to watermark to be saved.
//=============================================================================
LONG CWaterMarkDialog::QuerySaveWmChanges(HWND hDlg)
{

	LONG wType = MB_YESNO;
	WCHAR szTemp[MESSAGELENGTH] = { 0 };
	WCHAR szMessage[MESSAGELENGTH + (WM_STR_LENGTH + 1)] = { 0 };
	WCHAR szMessageTitle[MESSAGETITLELENGTH] = { 0 };
	WCHAR szWaterMark[WM_STR_LENGTH + 1] = { 0 };
	//<S><D>To Fix #3176,2020-01-06,SSDI:Chanchal Singla
	//Validation Message is not displayed when trying to edit the Watermark
	// When Watermark text box contains only spaces, “To valid this setting,..” 
	//// message box displays
	//if (TRUE == CheckSpaceOnly(m_PropertySheetState.szWM))
	//{
	//	return IDNO;
	//}
	// When Watermark text box contains only spaces, “To valid this setting,..” 
	//<E>To Fix #3176,2020-01-06,SSDI:Chanchal Singla
	// message box displays
	if (0 == m_PropertySheetState.wWmIndex)
	{
		if (0 == wcslen(m_PropertySheetState.szWM))
		{
			return IDNO;
		}
		if (LoadString(m_hStringResourceHandle, IDS_INDEX0_WATERMARK, szTemp, countof(szTemp)) > 0)
		{
			if (0 == wcscmp(m_PropertySheetState.szWM, szTemp))
				return IDNO;
		}
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_2, szMessage, countof(szMessage)) > 0)
		{
			wType = MB_YESNO;
		}
	}
	else
	{
		PropStateToWatermarkData(&m_WaterMarkDataChanged);

		if (0 == memcmp(&m_WaterMarkDataChanged, &m_WaterMarkData, sizeof(WATERMARKDATA)))
		{
			return wType;
		}

		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_8, szTemp, countof(szTemp)) > 0)
		{
			// Do not pass WaterMarkData structure member variable . 
			// Pass temporary WaterMarkData structure to read title only from registry.

			WATERMARKDATA watermarkdata = { 0 };
			m_pWatermarkregdata->ReadWMData(m_pPrinterName, m_PropertySheetState.wWmIndex, &watermarkdata, szWaterMark, sizeof(szWaterMark));

		}
		swprintf_s(szMessage, _countof(szMessage), szTemp, szWaterMark);
		// Message type should be MB_YESNOCANCEL
		wType = MB_YESNOCANCEL;

	}

	if (LoadString(m_hStringResourceHandle, IDS_MESTITLE_10, szMessageTitle, countof(szMessageTitle)) > 0)
	{
		// Pass PRoperty sheet dialog handle to Message box as owner window.
		wType = MessageBox(hDlg, szMessage, szMessageTitle, wType);
	}
	return wType;
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
LRESULT CWaterMarkDialog::OnWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
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

	case EN_CHANGE:
	{
		OnEnChange(hWnd, hWndCtrl, wCtrlId);
	}
	break;
	case CBN_SELCHANGE:
	{
		SendCommand(hWnd, wParam);
		OnCbnSelchange(hWnd, hWndCtrl, wCtrlId);
	}
	break;
	default:
		break;
	}
	return TRUE;
}
//=============================================================================
// function
//      SendCommand
//
// parameters
//      hDlg - Handle to Window.
//		wParam - Message Parameter.
//
// return value
//      TRUE always
//
// outline
//      Handles the Send Command events.
//=============================================================================
HRESULT
CWaterMarkDialog::SendCommand(
	_In_ CONST HWND   hDlg,
	_In_ CONST WPARAM wParam
)
{

	// Use the wParam as the index into the control map and
	// inform the control that generated the command

	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	UIControlMap* pComponents = NULL;
	pComponents = &m_UIControls;
	UIControlMap::iterator iterUIComponents = pComponents->begin();
	CUIControl* pControl = (*pComponents)[LOWORD(wParam)];

	if (NULL != pControl)
	{
		hr = pControl->OnCommand(LOWORD(wParam), hDlg, HIWORD(wParam));
		if (S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
	}
	else
	{
		slog.putLog("CWaterMarkDialog::SendCommand -> NULL_POINTER(pControl)\n");
	}


	return hr;
}
//=============================================================================
// function
//      CWaterMarkDialog::OnWmHScroll
//
// Arguments
//			HWND			hWnd
//			WPARAM			wParam
//			LPARAM			lParam
//
// return value
//    
//		TRUE / FALSE
//
// outline
//    This Functions Processes Notification information to be handled
//=============================================================================

LRESULT CWaterMarkDialog::OnWmHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LRESULT			res = 0;
	INT wCtrlID = GetDlgCtrlID((HWND)lParam);
	
	switch (wCtrlID)
	{
	case IDC_SCR_WM_HORZ:
	case IDC_SCR_WM_SIZE:
	case IDC_SCR_WM_ANGLE:
		OnWaterMarkScrollBar(wCtrlID, hWnd, wParam, lParam);
	break;
	default:
		break;
	}
	res = CUIControl::OnWmHScroll(hWnd, wParam, lParam);
	SetTabState(hWnd, wCtrlID);
	return res;
}

//=============================================================================
// function
//      CWaterMarkDialog::OnWmVScroll
//
// Arguments
//			HWND			hWnd
//			WPARAM			wParam
//			LPARAM			lParam
//
// return value
//    
//		TRUE / FALSE
//
// outline
//    This Functions Processes Notification information to be handled
//=============================================================================
LRESULT CWaterMarkDialog::OnWmVScroll(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LRESULT			res = 0;
	INT wCtrlID = GetDlgCtrlID((HWND)lParam);
	switch (wCtrlID)
	{
		case IDC_SCR_WM_VERT:
			OnWaterMarkScrollBar(wCtrlID, hWnd, wParam, lParam);
		break;
	default:
		break;
	}
	res = CUIControl::OnWmVScroll(hWnd, wParam, lParam);
	SetTabState(hWnd, wCtrlID);
	return res;
}

//=============================================================================
// function
//      CWaterMarkDialog::OnWaterMarkScrollBar
//
// Arguments
//			INT				wCtrlID
//			HWND			hWnd
//			WPARAM			wParam
//			LPARAM			lParam
//
// return value
//    
//		TRUE / FALSE
//
// outline
//    This Functions Processes WaterMark ScrollBar
//=============================================================================

LRESULT CWaterMarkDialog::OnWaterMarkScrollBar(INT wCtrlID, HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UIControlMap::const_iterator Iterator = m_UIControls.find(wCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CWaterMarkScrollBar *pWaterMarkScrollBar = static_cast<CWaterMarkScrollBar*>(Iterator->second);
		pWaterMarkScrollBar->OnScroll(wCtrlID, LOWORD(wParam), hWnd, HIWORD(wParam));
	}
	return S_OK;
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
LRESULT CWaterMarkDialog::OnWmDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CWaterMarkPreview*	m_pWaterMarkPreview = NULL;
	short				wCtrlID = (short)wParam;
	LPDRAWITEMSTRUCT	pdis = (LPDRAWITEMSTRUCT)lParam;
	m_hPropertySheetHandle = hWnd;

	UIControlMap::const_iterator Iterator;
	switch (wCtrlID)
	{
	case IDC_CMB_WM:
	{
		Iterator = m_UIControls.find(IDC_CMB_WM);
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

	case IDC_BTN_WM_PREV:
	{
		Iterator = m_UIControls.find(IDC_BTN_WM_PREV);
		if (Iterator != m_UIControls.end())
		{
			m_pWaterMarkPreview = static_cast<CWaterMarkPreview*>(Iterator->second);
			if (NULL == m_pWaterMarkPreview)
			{
				slog.putLog("CWaterMarkDialog::OnWmDrawItem -> NULL_OBJECT(m_pWaterMarkPreview)\n");
			}
		}

		if (NULL != m_pWaterMarkPreview)
		{
			WATERMARKDATA WatermarkData = { 0 };
			memcpy_s(&WatermarkData, sizeof(WATERMARKDATA), &m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
			PropStateToWatermarkData(&WatermarkData);
			m_pWaterMarkPreview->Draw(&WatermarkData, m_dwSelectedPaperWidthPixel, m_dwSelectedPaperHeightPixel, m_sOrientation);
		}
		else
		{
			slog.putLog("CWaterMarkDialog::OnWmDrawItem -> NULL_OBJECT(m_pWaterMarkPreview)\n");
		}
		break;
	}
	default: 
		break;
	}
	return CUIControl::OnWmDrawItem(hWnd, wParam, lParam);
}
//=============================================================================
// function
//      GetWaterMarkRegistryObject
//
// return value
//     NILL
//
// outline
//      Gets watermark registry object.
//=============================================================================
void CWaterMarkDialog::GetWaterMarkRegistryObject(void)
{

	TCHAR szWaterMarkDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szWaterMarkDatFilePath, L"wm62.dat");
	CShIniFile* cshinifile = new CShIniFile(ghInstance, m_pPrinterName, szWaterMarkDatFilePath, FALSE);
	m_pWatermarkregdata = new CWaterMarkRegistryData(ghInstance, m_pPrinterName, m_hStringResourceHandle);

	/*CShJsonWm**/ m_pjsonwm = new CShJsonWm(ghInstance, m_pPrinterName, m_hStringResourceHandle);
	m_pjsonwm->Init();
	m_pWatermarkregdata->reset(m_pPrinterName, cshinifile);
	if ((*cshinifile).IsWriteToJson() == TRUE)
	{	// <S><A> 2023.12.28,To Fix feedback issue SSDI:Manoj S
		if ((*m_pjsonwm).ReadCount() == 0)
		{
			(*m_pjsonwm).reset(m_pPrinterName,cshinifile);
			m_pjsonwm->WriteJsonDataToFile();
		}
		// <E> 2023.12.28,To Fix feedback issue SSDI:Manoj S
		//(*m_pjsonwm).SelectTheCorrectFavItem((*pext2).stSubFavNames.szWMNameHash, (*pps).wWmIndex);
	}
	//<S><D>To Fix Bug #3689,20210520,SSDI:Chanchal Singla
	//Following Line has moved in if Condition for fixing the redmine #3689
	//m_pWatermarkregdata->ReadWMData(m_pPrinterName, m_pTempOemPrivateDevMode->scpi.ext.nWatermark, &m_WaterMarkData);
	//<E>To Fix Bug #3689,20210520,SSDI:Chanchal Singla
	m_PropertySheetState.wWmIndex = m_pTempOemPrivateDevMode->scpi.ext.nWatermark;
//<S><C>To the Support of the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla
	// if scdm watermark language and registry language is different, read the watermark data from file
	if (m_pTempOemPrivateDevMode->scpi.ext2.Watermark.data.dwWMLangID == m_dwLangID  && m_PropertySheetState.wWmIndex != 0)
	//if (m_pTempOemPrivateDevMode->scpi.ext.WaterMark.dwWMLangID == m_dwLangID)
	{
		//<S><C>To Fix Bug #3689,20210520,SSDI:Chanchal Singla
		//	Deleted Watermark is displayed when saved User Settings is selected
		// copy watermark data from SCDM to m_WaterMarkData
		//memcpy_s(&m_WaterMarkData, sizeof(WATERMARKDATA), &(m_pTempOemPrivateDevMode->scpi.ext.WaterMark), sizeof(WATERMARKDATA));
		//memcpy_s(&m_WaterMarkData, sizeof(WATERMARKDATA), &(m_pTempOemPrivateDevMode->scpi.ext2.Watermark.data), sizeof(WATERMARKDATA));
		BOOL wRet = m_pWatermarkregdata->ReadWMData(m_pPrinterName, m_pTempOemPrivateDevMode->scpi.ext.nWatermark, &m_WaterMarkData);
		//<S><A>To Fix Bug #3882,20210603,SSDI:Chanchal Singla
		//Watermak: Delete button is enabled even though None exists in the Watermark drop down
		if (wRet == FALSE)
		{
			m_PropertySheetState.wWmIndex = FALSE;
			m_pWaterMarkFileData->GetWMDefData(m_PropertySheetState.wWmIndex, &m_WaterMarkData);
		}
		else
		{
			m_PropertySheetState.wWmIndex = m_pTempOemPrivateDevMode->scpi.ext.nWatermark;
		}
		//<E>To Fix Bug #3882,20210603,SSDI:Chanchal Singla
		//<E>To Fix Bug #3689,20210520,SSDI:Chanchal Singla
	}
	//<E>To the Support of the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla
	else
	{
		m_pWaterMarkFileData->GetWMDefData(m_PropertySheetState.wWmIndex, &m_WaterMarkData);
		//<S><A> Fix for Feedback issue 51(Watermark option "None" is not changed based on the language selection, Harika 20210805
		//m_pWatermarkregdata->WriteWMData(m_pPrinterName, m_PropertySheetState.wWmIndex, &m_WaterMarkData, m_WaterMarkData.szMainWM);
		//<E> Fix for Feedback issue 51(Watermark option "None" is not changed based on the language selection, Harika 20210805
	}
	memcpy_s(&m_WaterMarkDataChanged, sizeof(WATERMARKDATA), &m_WaterMarkData, sizeof(WATERMARKDATA));

	WatermarkDataToPropState(&m_WaterMarkDataChanged);

	delete cshinifile;
	cshinifile = NULL;

	//delete m_pjsonwm;
	//m_pjsonwm = NULL;

}

//=============================================================================
// function
//      ProcessFeatureInfoList
//
// return value
//     NILL
//
// outline
//      Processes feature info list.
//=============================================================================
void CWaterMarkDialog::ProcessFeatureInfoList()
{
	UINT FeatureCount = (UINT)m_pFeatureInfoList[0].nNummberOfFeatures;
	BOOL bPaperSizeFeatureInfoFound = FALSE;
	BOOL bOrientationFeatureInfoFound = FALSE;
	UINT uFeatureIndex = 0;
	while (
		((FALSE == bPaperSizeFeatureInfoFound) || (FALSE == bOrientationFeatureInfoFound)) &&
		(uFeatureIndex < FeatureCount)
		)
	{
		if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszFeature, "PaperSize"))
		{
			GetPaperSize(uFeatureIndex);
			bPaperSizeFeatureInfoFound = TRUE;
		}
		if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszFeature, "PageShOrientation"))
		{
			GetOrientation(uFeatureIndex);
			bOrientationFeatureInfoFound = TRUE;
		}
		uFeatureIndex++;
	}
}
//=============================================================================
// function
//      GetPaperSize
//
// return value
//     NILL
//
// outline
//      Gets selected paper size.
//=============================================================================
void CWaterMarkDialog::GetPaperSize(UINT uFeatureIndex)
{
	if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszFeature, "PaperSize"))
	{
		if (!strcmp("CUSTOMSIZE", m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption))
		{
			m_dwSelectedPaperWidthPixel = m_pTempOemPrivateDevMode->scpi.ext.dwCusWidthPixel;
			m_dwSelectedPaperHeightPixel = m_pTempOemPrivateDevMode->scpi.ext.dwCusLengthPixel;
		}
		else
		{
			PaperInfoMap::const_iterator it;
			it = m_PaperInfo.find(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption);
			if (it != m_PaperInfo.end())
			{
				DWORD dwImageableWidthPixel = it->second.PageImageaDimensions.first;
				DWORD dwImageableHeightPixel = it->second.PageImageaDimensions.second;
				SHORT sLeftMarginPixel = it->second.PageOrigin.first;
				SHORT sTopMarginPixel = it->second.PageOrigin.second;
				m_dwSelectedPaperWidthPixel = 2 * sLeftMarginPixel + dwImageableWidthPixel;
				m_dwSelectedPaperHeightPixel = 2 * sTopMarginPixel + dwImageableHeightPixel;
			}
		}


	}

}
//=============================================================================
// function
//      GetOrientation
//
// return value
//     NILL
//
// outline
//      Gets selected Orientation.
//=============================================================================
void CWaterMarkDialog::GetOrientation(UINT uFeatureIndex)
{
	if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption, "PORTRAIT"))
	{
		m_sOrientation = DMORIENT_PORTRAIT;
	}
	if (!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption, "LANDSCAPE_CC90"))
	{
		m_sOrientation = DMORIENT_LANDSCAPE;
	}
}

//=============================================================================
// function
//      WatermarkDataToPropState
//
// return value
//     NILL
//
// outline
//   Updates Property sheet structure based on Watermark data structure.
//=============================================================================
BOOL CWaterMarkDialog::WatermarkDataToPropState(LPWATERMARKDATA pWaterMarkData)
{


	m_PropertySheetState.wOnFirstPage = pWaterMarkData->wFieldWMD & DM_WM_FIRSTPAGEONLY ? BST_CHECKED : BST_UNCHECKED;
	m_PropertySheetState.lWmPosX = (SHORT)pWaterMarkData->dwPosX;
	m_PropertySheetState.lWmPosY = (SHORT)pWaterMarkData->dwPosY;
	m_PropertySheetState.wWmSize = pWaterMarkData->nSize;
	m_PropertySheetState.wWmAngle = pWaterMarkData->nAngle - 90;
	// _countof Macro returns number of elements
	// For unicode strings this should be multiplied with size of WCHAR
	memcpy_s(m_PropertySheetState.szWM, _countof(m_PropertySheetState.szWM) * sizeof(WCHAR), pWaterMarkData->szMainWM, _countof(pWaterMarkData->szMainWM) * sizeof(WCHAR));

//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	//memcpy(m_pTempOemPrivateDevMode->scpi.ext2.stSubFavNames.szWMNameHash, m_PropertySheetState.szWMNameHash, sizeof(m_pTempOemPrivateDevMode->scpi.ext2.stSubFavNames.szWMNameHash));
	memcpy(m_PropertySheetState.szWMNameHash, m_pTempOemPrivateDevMode->scpi.ext2.stSubFavNames.szWMNameHash, sizeof(m_PropertySheetState.szWMNameHash));
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S	
	return TRUE;

}
//=============================================================================
// function
//      PropStateToWatermarkData
//
// return value
//     NILL
//
// outline
//   Updates Watermark data structure  based on Property sheet structure.
//=============================================================================
BOOL CWaterMarkDialog::PropStateToWatermarkData(LPWATERMARKDATA pWaterMarkData)
{
	pWaterMarkData->wFieldWMD = m_PropertySheetState.wOnFirstPage == BST_CHECKED ?
		pWaterMarkData->wFieldWMD | DM_WM_FIRSTPAGEONLY : pWaterMarkData->wFieldWMD & ~DM_WM_FIRSTPAGEONLY;
	pWaterMarkData->dwPosX = m_PropertySheetState.lWmPosX;
	pWaterMarkData->dwPosY = m_PropertySheetState.lWmPosY;
	pWaterMarkData->nSize = m_PropertySheetState.wWmSize;
	pWaterMarkData->nAngle = m_PropertySheetState.wWmAngle + 90;
	memcpy_s(pWaterMarkData->szMainWM, _countof(pWaterMarkData->szMainWM) * sizeof(WCHAR), m_PropertySheetState.szWM, _countof(m_PropertySheetState.szWM) * sizeof(WCHAR));
	return TRUE;

}

//=============================================================================
// function
//      SetButtonState
//    
// return value
//     NILL
//
// outline
//  Sets button state .
//=============================================================================
void CWaterMarkDialog::SetButtonState(HWND hDlg)
{

	BOOL bAddWaterMark = FALSE;


	if (
		(0 != wcslen(m_PropertySheetState.szWM)) &&
		(FALSE == CheckSpaceOnly(m_PropertySheetState.szWM))
		)
	{
		bAddWaterMark = TRUE;
	}

	if (0 == m_PropertySheetState.wWmIndex)
	{
		SetControlState(hDlg, IDC_BTN_WM_DELETE, SCUI_STATE_SHOW);
		if (TRUE == bAddWaterMark)
			SetControlState(hDlg, IDC_BTN_WM_ADD, SCUI_STATE_ON);
		else
			SetControlState(hDlg, IDC_BTN_WM_ADD, SCUI_STATE_SHOW);
	}
	else
	{
		SetControlState(hDlg, IDC_BTN_WM_DELETE, SCUI_STATE_ON);
		if (TRUE == bAddWaterMark)
		{
			PropStateToWatermarkData(&m_WaterMarkDataChanged);
			if (0 == memcmp(&m_WaterMarkDataChanged, &m_WaterMarkData, sizeof(WATERMARKDATA)))
			{
				SetControlState(hDlg, IDC_BTN_WM_ADD, SCUI_STATE_SHOW);
			}
			else
			{
				SetControlState(hDlg, IDC_BTN_WM_ADD, SCUI_STATE_ON);
			}
		}
		else
			SetControlState(hDlg, IDC_BTN_WM_ADD, SCUI_STATE_SHOW);
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
void CWaterMarkDialog::CreateDepenedentContolMap(void)
{
	CreateMapElement(IDC_SCR_WM_SIZE, IDC_STC_WM_SIZE_VAL);
	CreateMapElement(IDC_SCR_WM_ANGLE, IDC_STC_WM_ANGLE_VAL);
	CreateMapElement(IDC_SCR_WM_HORZ, IDC_STC_WM_X_VAL);
	CreateMapElement(IDC_SCR_WM_VERT, IDC_STC_WM_Y_VAL);
	CreateMapElement(IDC_CMB_WM, IDC_SCR_WM_SIZE);
	CreateMapElement(IDC_BTN_WM_ADD, IDC_CMB_WM);

	// Check box  control is dependent on watermark combo  box control. 
	// Whenever watermamrk  selection is changed, notification is sent to Check box contol. 
	// With this check box  displays check state of newly selected water mark string.
	INT iComboBoxDependentControlIDs[] = {
									IDC_SCR_WM_SIZE,
									IDC_SCR_WM_ANGLE,
									IDC_SCR_WM_HORZ,
									IDC_SCR_WM_VERT,
									IDC_STC_WM_SIZE_VAL,
									IDC_STC_WM_ANGLE_VAL,
									IDC_STC_WM_X_VAL,
									IDC_STC_WM_Y_VAL,
									IDC_EDT_WM_TEXT,
									IDC_CHK_ON_1STPAGE



	};

	CreateMapElement(IDC_CMB_WM, iComboBoxDependentControlIDs, countof(iComboBoxDependentControlIDs));
	INT iCenterButtonDependentControlIDs[] = {
		IDC_SCR_WM_HORZ,
		IDC_SCR_WM_VERT,
		IDC_STC_WM_X_VAL,
		IDC_STC_WM_Y_VAL

	};
	CreateMapElement(IDC_BTN_WM_CENTER, iCenterButtonDependentControlIDs, countof(iCenterButtonDependentControlIDs));
	
	// Edit text control is dependent on Water mark delete button control. 
	// Whenever delete button is clicked, notification is sent to Edit text field. 
	// With this edit text field displays newly selected water mark string.
	INT iWaterMarkDeleteButtonDependentContolIDs[] =
	{
		IDC_CMB_WM,
		IDC_EDT_WM_TEXT
	};
	CreateMapElement(IDC_BTN_WM_DELETE, iWaterMarkDeleteButtonDependentContolIDs, countof(iWaterMarkDeleteButtonDependentContolIDs));
}
//=============================================================================
// function
//      CreateMapElement
//
// return value
//     NILL
//
// outline
//    Creates Map elements.
//=============================================================================
void CWaterMarkDialog::CreateMapElement(INT iMapKey, INT iMapValue)
{
	std::vector<INT> vectorDependentContolID;
	vectorDependentContolID.push_back(iMapValue);
	m_DependentContolMap[iMapKey] = vectorDependentContolID;
	vectorDependentContolID.clear();
}
//=============================================================================
// function
//      CreateMapElement
//
// return value
//     NILL
//
// outline
//    Creates Map elements.
//=============================================================================
void CWaterMarkDialog::CreateMapElement(INT iMapKey, INT* iDependentControlIDs, DWORD dwCount)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	
	if (nullptr == iDependentControlIDs)
	{
		slog.putLog("CWaterMarkDialog::CreateMapElement -> NULL_POINTER(iDependentControlIDs)\n");
		return;
	}
	std::vector<INT>DependentContolID;
	for (DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		DependentContolID.push_back(iDependentControlIDs[dwIndex]);
	}
	m_DependentContolMap[iMapKey] = DependentContolID;
	DependentContolID.clear();
}

//=============================================================================
// function
//      OnComboboxSelectionChange
//
// return value
//     NILL
//
// outline
//   Processes combo box change events.
//=============================================================================
LRESULT CWaterMarkDialog::OnComboboxSelectionChange(HWND hDlg)
{

	m_pWatermarkregdata->ReadWMData(m_pPrinterName, m_PropertySheetState.wWmIndex, &m_WaterMarkDataChanged);
	memcpy_s(&m_WaterMarkData, sizeof(WATERMARKDATA), &m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
	WatermarkDataToPropState(&m_WaterMarkData);
	if (0 == m_PropertySheetState.wWmIndex)
	{
		SecureZeroMemory(m_PropertySheetState.szWM, sizeof(m_PropertySheetState.szWM));
	}
	SetButtonState(hDlg);
	return S_OK;
}

//=============================================================================
// function
//      SetTabState
//
// return value
//     NILL
//
// outline
//   sets tab state.
//=============================================================================
void  CWaterMarkDialog::SetTabState(
	_In_ CONST HWND   hDlg,
	_In_ CONST INT iCtrlID
)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	SetButtonState(hDlg);
	DependentContolIDMap::const_iterator IteratorToDepandentContolMap = m_DependentContolMap.find(iCtrlID);
	if (IteratorToDepandentContolMap != m_DependentContolMap.end())
	{
		std::vector<INT>DependentContolID = IteratorToDepandentContolMap->second;
		for (DWORD dwCount = 0; dwCount < DependentContolID.size(); dwCount++)
		{
			INT iControlMapId = DependentContolID[dwCount];
			m_UIControls[iControlMapId]->OnInit(iControlMapId, hDlg);

		}
	}
	//<S><A>To fix #3175,2020-01-06,SSDI:Chanchal Singla
	//ON First Page Only check box is checked automatically when saved Watermark is deleted
	// --- on first page only
	CheckDlgButton(hDlg,IDC_CHK_ON_1STPAGE, m_PropertySheetState.wOnFirstPage);
	// --- preview
	SetPos(hDlg, IDC_SCR_WM_SIZE);
	SetPosString(hDlg, IDC_STC_WM_SIZE_VAL);

	SetPos(hDlg, IDC_SCR_WM_ANGLE);
	SetPosString(hDlg, IDC_STC_WM_ANGLE_VAL);

	SetPos(hDlg, IDC_SCR_WM_HORZ);
	SetPosString(hDlg, IDC_STC_WM_X_VAL);

	SetPos(hDlg, IDC_SCR_WM_VERT);
	SetPosString(hDlg, IDC_STC_WM_Y_VAL);
	
	//<E>To fix #3175,2020-01-06,SSDI:Chanchal Singla
	CWaterMarkPreview*					m_pWaterMarkPreview = NULL;
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_WM_PREV);
	if (Iterator != m_UIControls.end())
	{
		m_pWaterMarkPreview = static_cast<CWaterMarkPreview*>(Iterator->second);
		if (NULL == m_pWaterMarkPreview)
		{
			slog.putLog("CWaterMarkDialog::SetTabState -> NULL_OBJECT(m_pWaterMarkPreview)\n");
		}
	}
	if (NULL != m_pWaterMarkPreview)
	{
		WATERMARKDATA WatermarkData = { 0 };
		memcpy_s(&WatermarkData, sizeof(WATERMARKDATA), &m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
		PropStateToWatermarkData(&WatermarkData);
		m_pWaterMarkPreview->Draw(&WatermarkData, m_dwSelectedPaperWidthPixel, m_dwSelectedPaperHeightPixel, m_sOrientation);
	}

}

//=============================================================================
// function
//      OnWaterMarkCenter
//
// return value
//     NILL
//
// outline
//   Makes watermark position at the center  .
//=============================================================================
void CWaterMarkDialog::OnWaterMarkCenter(void)
{
	m_PropertySheetState.lWmPosX = 0;
	m_PropertySheetState.lWmPosY = 0;

}
//=============================================================================
// function
//      OnWaterMarkDelete
//
// return value
//     NILL
//
// outline
//   Deletes selected watermark  .
//=============================================================================
BOOL CWaterMarkDialog::OnWaterMarkDelete(HWND hDlg)
{
	//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	BOOL			bShare = FALSE;
	WCHAR			szTextHKLMW[REG_ENT_SHARE_KEYSIZEW] = { 0 };
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	m_pWatermarkregdata->ReadWMData(m_pPrinterName, m_PropertySheetState.wWmIndex, &m_WaterMarkDataChanged);
	WCHAR szMessageTitle[768] = { 0 };
	if (LoadString(m_hStringResourceHandle, IDS_MESTITLE_11, szMessageTitle, countof(szMessageTitle)) > 0)
	{
		WATERMARKDATA WaterMarkDataElement = { 0 };
		WCHAR szWaterMarkTitle[SCUI_MAX_STRING] = { 0 };
		m_pWatermarkregdata->ReadWMData(m_pPrinterName, m_PropertySheetState.wWmIndex, &WaterMarkDataElement, szWaterMarkTitle, sizeof(szWaterMarkTitle));
		
		if (IDNO == MessageBox(hDlg, szWaterMarkTitle, szMessageTitle, MB_YESNO))
		{
			return FALSE;
		}
		
	}
	//<S><C>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	m_pWatermarkregdata->DeleteData(m_PropertySheetState.wWmIndex);
	m_PropertySheetState.wWmIndex--;
	OnComboboxSelectionChange(hDlg);
	memcpy_s(&m_WaterMarkData, sizeof(WATERMARKDATA), &m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
	PropSheet_Changed(GetParent(hDlg), hDlg);

	m_pWatermarkregdata->ReadShareDayTimeFuncFromHKLM(m_pPrinterName, szTextHKLMW, REG_ENT_SHARE_KEYSIZEW);

	if (wcslen(szTextHKLMW) > 0)
	{
		bShare = m_pWatermarkregdata->GetValidFlag(szTextHKLMW);
	}

	if (bShare == TRUE)
	{
		m_pWatermarkregdata->WriteFavItemsFromHKCUToHKLM(m_pPrinterName);
	}
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	return TRUE;
}

//=============================================================================
// function
//      OnWaterMarkAdd
//
// return value
//     NILL
//
// outline
//   Updates new /existed watermark data  .
//=============================================================================
BOOL CWaterMarkDialog::OnWaterMarkAdd(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	BOOL			bShare = FALSE;
	WCHAR			szTextHKLMW[REG_ENT_SHARE_KEYSIZEW] = { 0 };
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	if (FALSE == IsAddWaterMark())
	{
		WCHAR szMessage[MAX_PATH] = { 0 };
		WCHAR szItem[MAX_PATH] = { 0 };
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_47, szMessage, countof(szMessage)) > 0)
		{
			LoadString(m_hStringResourceHandle, IDS_MESTITLE_1, szItem, countof(szItem));
			MessageBox(hDlg, szMessage, szItem, MB_ICONINFORMATION | MB_OK);
		}
		return FALSE;
	}
	//<S><A>To Fix #3155,2020-01-06,SSDI:Chanchal Singla
	//Watermark size displays incorrectly in sub dialog when watermark size is increased.
	PropStateToWatermarkData(&m_WaterMarkDataChanged);
	//<E>To Fix #3155,2020-01-06,SSDI:Chanchal Singla
	CWaterMarkAddDialog *pdlg = new CWaterMarkAddDialog(IDD_DLG_WM_SAVE, m_hStringResourceHandle,
		&m_PropertySheetState, m_pPrinterName,  m_hPrinterHandle,
		&m_WaterMarkDataChanged, m_dwSelectedPaperWidthPixel,m_dwSelectedPaperHeightPixel, m_sOrientation);
	//Fixed Coverity issue - 90611 - SSDI:Seetharam - 20201020
	//Moved this statement after NULL check 
	//pdlg->SetPrinterName(m_pPrinterName);
	if (NULL == pdlg)
	{
		slog.putLog("CWaterMarkTabPropertyPage::OnWaterMarkAdd -> NULL_OBJECT(IDD_DLG_WM_SAVE)\n");
		return FALSE;
	}
	pdlg->SetPrinterName(m_pPrinterName);

	INT_PTR DlgRet = IDCANCEL;
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_WM_SAVE),
		hDlg, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
	if (IDOK == DlgRet)
	{
		AddWaterMark(pdlg->GetWaterMarkIndex(), pdlg->GetSavedWaterMarkString());
		OnComboboxSelectionChange(hDlg);
		//Send Property sheet change information to property sheet dialog only during following events. 
		// 1. Water mark combo is changed. 
		// 2. User clicks "Edit" dialog and changed some settings in "Edit dialog". 
		// 3. User clicks "Yes " on "Add" dialog Confirmation message box. 
		// 4. User clicks "Yes " on "Delete" dialog Confirmation message box. 
		m_blForceUpdate = TRUE;
		PropSheet_Changed(GetParent(hDlg), hDlg);
	}
	else
	{
		m_blForceUpdate = FALSE;
	//	memcpy_s(&m_WaterMarkData, sizeof(WATERMARKDATA), &m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
	//	WatermarkDataToPropState(&m_WaterMarkData); 
	//	//<S><D>To Fix Feedback #37,29122020,SSDI:Chanchal Singla
	//	/*if (0 == m_PropertySheetState.wWmIndex)
	//	{
	//		SecureZeroMemory(m_PropertySheetState.szWM, sizeof(m_PropertySheetState.szWM));
	//	}*/
	//	//<E>To Fix Feedback #37,29122020,SSDI:Chanchal Singla
	}
//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	m_pWatermarkregdata->ReadShareDayTimeFuncFromHKLM(m_pPrinterName, szTextHKLMW, REG_ENT_SHARE_KEYSIZEW);

	if (wcslen(szTextHKLMW) > 0)
	{
		bShare = m_pWatermarkregdata->GetValidFlag(szTextHKLMW);
	}

	if (bShare == TRUE)
	{
		m_pWatermarkregdata->WriteFavItemsFromHKCUToHKLM(m_pPrinterName);
	}
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	delete pdlg;
	pdlg = NULL;
	return TRUE;

}
//=============================================================================
// function
//      OnWaterMarkEdit
//
// return value
//     NILL
//
// outline
//   Handles watermark edit dialog events  .
//=============================================================================
BOOL CWaterMarkDialog::OnWaterMarkEdit(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	PropStateToWatermarkData(&m_WaterMarkDataChanged);
	CWatermarkEditDialog *pdlg = new CWatermarkEditDialog(IDD_DLG_WM_EDT, m_hStringResourceHandle, m_hBitmapResourceHandle, m_pResourceIDInfoMap, m_pFeatureInfoList, m_DefaultSettings,
		m_WaterMarkDataChanged, m_pCustomColor, m_PropertySheetState.wWmIndex, m_dwLangID, m_hPrinterHandle, m_pWaterMarkFileData, m_PszPrnName);
	if (NULL == pdlg)
	{
		slog.putLog("CWaterMarkDialog::OnWaterMarkEdit -> NULL_POINTER(IDD_DLG_WM_EDT_11)\n");
		return FALSE;
	}
	pdlg->SetPrinterName(m_PszPrnName);
	INT_PTR DlgRet = IDCANCEL;
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_WM_EDT),
		hDlg, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
	if (IDOK == DlgRet)
	{
		LPWATERMARKDATA pWaterMarkData = pdlg->GetWatermarkDataAddr();
		if (0 != memcmp(&m_WaterMarkDataChanged, pWaterMarkData, sizeof(WATERMARKDATA)))
		{
			PropSheet_Changed(GetParent(hDlg), hDlg);
			memcpy_s(&m_WaterMarkDataChanged, sizeof(WATERMARKDATA), pWaterMarkData, sizeof(WATERMARKDATA));
			WatermarkDataToPropState(&m_WaterMarkDataChanged);
			m_blForceUpdate = TRUE;
		}

	}
	delete pdlg;
	pdlg = NULL;
	return TRUE;
}
//=============================================================================
// function
//      IsAddWaterMark
//
// return value
//     NILL
//
// outline
//   Checks whether to add new watermark or not .
//=============================================================================
BOOL CWaterMarkDialog::IsAddWaterMark()
{
	DWORD dwCount = m_pWatermarkregdata->ReadWMCount(m_pPrinterName);
	if (MaximumNumberOfWaterMarks < dwCount)
	{
		return FALSE;
	}
	return TRUE;
}

//=============================================================================
// function
//      AddWaterMark
//
// return value
//     NILL
//
// outline
//   Adds new watermark .
//=============================================================================
BOOL CWaterMarkDialog::AddWaterMark(SHORT sWaterMarkIndex, WCHAR* szTitle)
{
	PropStateToWatermarkData(&m_WaterMarkDataChanged);
	if ((*m_pIniFile).IsWriteToJson() == TRUE)
	{
		m_pjsonwm->WriteWMData(sWaterMarkIndex, &m_WaterMarkDataChanged, szTitle);
		DWORD dwNumberOfWaterMarks = m_pjsonwm->ReadWMCount(m_pPrinterName);
		if (sWaterMarkIndex == (SHORT)dwNumberOfWaterMarks)
		{
			m_pjsonwm->WriteWMCount(++dwNumberOfWaterMarks);
		}
		m_pjsonwm->WriteJsonDataToFile();
	}
	//else
	//{
		m_pWatermarkregdata->WriteWMData(m_pPrinterName, sWaterMarkIndex, &m_WaterMarkDataChanged, szTitle);
		DWORD dwNumberOfWaterMarks = m_pWatermarkregdata->ReadWMCount(m_pPrinterName);
		if (sWaterMarkIndex == (SHORT)dwNumberOfWaterMarks)
		{
			m_pWatermarkregdata->WriteWMCount(m_pPrinterName, ++dwNumberOfWaterMarks);
		}
	//}
	m_PropertySheetState.wWmIndex = sWaterMarkIndex;
	memcpy_s(&m_WaterMarkData, sizeof(WATERMARKDATA), &m_WaterMarkDataChanged, sizeof(WATERMARKDATA));
	return TRUE;
}
//=============================================================================
// function
//      SaveFeatureOptions
//
// return value
//     NILL
//
// outline
//   Saves selected feature options.
//=============================================================================
void CWaterMarkDialog::SaveFeatureOptions()
{
	m_pTempOemPrivateDevMode->scpi.ext.nWatermark = m_PropertySheetState.wWmIndex;
	PropStateToWatermarkData(&m_WaterMarkData);
	//<S><C>To the Support of the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla
	// copy watermark data into SCDM
	//memcpy_s(&(m_pTempOemPrivateDevMode->scpi.ext.WaterMark), sizeof(WATERMARKINFO), &m_WaterMarkData, sizeof(WATERMARKINFO));
	memcpy_s(&(m_pTempOemPrivateDevMode->scpi.ext2.Watermark.data), sizeof(WATERMARKINFO), &m_WaterMarkData, sizeof(WATERMARKINFO));
//<E>To the Support of the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla

	//Checking if Watermark edit text box is empty, then make m_bIsWaterMarkTextEmpty as TRUE.
	WCHAR szWaterMarkText[WM_STR_LENGTH + 1] = {};
	GetDlgItemText(m_hPropertySheetHandle, IDC_EDT_WM_TEXT, szWaterMarkText, (WM_STR_LENGTH + 1));
	WCHAR p = szWaterMarkText[0];
	if ((0 == wcscmp(L" ", szWaterMarkText)) || (0 == wcscmp(L"", szWaterMarkText)) || (0x09 == p) || (0x3000 == p))
	{
		m_bIsWaterMarkTextEmpty = TRUE;
	}

}
// ============================================================================
// function
//      GetLanguageID(HWND hDlg)
//
// return value
//    Bool
//
// outline
//   Getting Language ID  .
//=============================================================================
DWORD CWaterMarkDialog::GetLanguageID(HANDLE hPrinter)
{
	// get language ID 
	// get language id from registry
	WCHAR szCommonDatFilePath[_MAX_PATH] = {};
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	//<S><C>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
	//In Basic mode Printing Preferences is not displayed as per the Language set
	
	CShIniFile cIniFile(ghInstance, m_pPrinterName, szCommonDatFilePath, FALSE);
	//DWORD dwDefLangID = cIniFile.GetDefaultLangID();

	SHORT dwLangID = -1;
	CShRegMdlTbl RegMdlTbl(ghInstance, m_pPrinterName, &cIniFile);
	RegMdlTbl.GetLangID(&dwLangID);
	//WCHAR szPrinterName[MAX_PATH] = {};
	//DWORD			dwNeeded = 0;
	//GetPrinter(hPrinter, 2, NULL, 0, &dwNeeded);
	//BYTE* pTemp = NULL;
	//if (0 < dwNeeded)
	//{
	//	pTemp = new BYTE[dwNeeded]();
	//}
	//GetPrinter(hPrinter, 2, pTemp, dwNeeded, &dwNeeded);
	//if (NULL != pTemp)
	//{
	//	PRINTER_INFO_2* pPI2 = reinterpret_cast<PRINTER_INFO_2*>(pTemp);
	//	wcscpy_s(szPrinterName, MAX_PATH, pPI2->pPrinterName);
	//	delete[] pTemp;
	//	pTemp = NULL;
	//}
	//CRegistryAPI regApi;
	//DWORD dwLangID = regApi.GetLangID(szPrinterName, dwDefLangID);
	//<E>to Fix Redmine Bug #40062021.12.22,SSDI:Chanchal Singla
	//In Basic mode Printing Preferences is not displayed as per the Language set	
	return (DWORD)dwLangID;
}
// ============================================================================
// function
//      InitializeScrollBar()
//
// return value
//    Bool
//
// outline
//   Initialize Scroll Bar   .
//=============================================================================
BOOL CWaterMarkDialog::InitializeScrollBar()
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	CWaterMarkPreview*					m_pWaterMarkPreview = NULL;
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_WM_PREV);
	if (Iterator != m_UIControls.end())
	{
		m_pWaterMarkPreview = static_cast<CWaterMarkPreview*>(Iterator->second);
		if (NULL == m_pWaterMarkPreview)
		{
			slog.putLog("CWaterMarkTabPropertyPage::InitializeScrollBar -> NULL_OBJECT(m_pWaterMarkPreview)\n");
		}
	}

	HWND hWMSCHandleX = NULL;
	HWND hWMSCHandleY = NULL;

	Iterator = m_UIControls.find(IDC_SCR_WM_HORZ);
	if (Iterator != m_UIControls.end())
	{
		CWaterMarkScrollBar* pWaterMarkHScroll = static_cast<CWaterMarkScrollBar*>(Iterator->second);
		if (NULL == pWaterMarkHScroll)
		{
			slog.putLog("CWaterMarkTabPropertyPage::InitializeScrollBar -> NULL_OBJECT(pWaterMarkHScroll)\n");
		}
		else
			hWMSCHandleX = pWaterMarkHScroll->GetScrollBarHandle();
	}

	Iterator = m_UIControls.find(IDC_SCR_WM_VERT);
	if (Iterator != m_UIControls.end())
	{
		CWaterMarkScrollBar* pWaterMarkVScroll = static_cast<CWaterMarkScrollBar*>(Iterator->second);
		if (NULL == pWaterMarkVScroll)
		{
			slog.putLog("CWaterMarkTabPropertyPage::InitializeScrollBar -> NULL_OBJECT(pWaterMarkVScroll)\n");
		}
		else
			hWMSCHandleY = pWaterMarkVScroll->GetScrollBarHandle();
	}

	if (m_pWaterMarkPreview)
	{
		m_pWaterMarkPreview->SetScBarClass(hWMSCHandleX, hWMSCHandleY);
	}
	return TRUE;
}
//=============================================================================
// function
//      SetPos
//
// return value
//     NILL
//
// outline
//   SetPosition for vertical and horizental bar
//=============================================================================
void  CWaterMarkDialog::SetPos(
	_In_ CONST HWND   hDlg,
	_In_ CONST INT iCtrlID
)
{
	UIControlMap::const_iterator Iterator = m_UIControls.find(iCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CWaterMarkScrollBar *pWaterMarkScrollBar = static_cast<CWaterMarkScrollBar*>(Iterator->second);
		pWaterMarkScrollBar->OnInit(iCtrlID, hDlg);
	}
}
//=============================================================================
// function
//      SetPosString
//
// return value
//     NILL
//
// outline
//   SetPosition string for vertical and horizental bar
//=============================================================================
void  CWaterMarkDialog::SetPosString(_In_ CONST HWND   hDlg, _In_ CONST INT iCtrlID)
{
	UIControlMap::const_iterator Iterator = m_UIControls.find(iCtrlID);
	if (Iterator != m_UIControls.end())
	{
		CStaticText *pStaticTect = static_cast<CStaticText*>(Iterator->second);
		pStaticTect->OnInit(iCtrlID, hDlg);
	}
}