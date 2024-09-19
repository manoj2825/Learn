//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : WatermarkTabPropertyPage.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 09 October 2013
//
//	Description   : This class handles Property sheet dialog functions  for Watermark tab
//===========================================================================
#include "precomp.h"
#include "sh_api.h"
#include "..\..\BitmapResource\Include\resource.h"
#include "WaterMarkEditDialog.h"
//For Logging Purpose
#include "MFPLogger.h"
#define MESSAGELENGTH 1024
#define MESSAGETITLELENGTH 768
#define MaximumNumberOfWaterMarks 100
//=============================================================================
// function
//      CWaterMarkTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Constructor to the class CWaterMarkTabPropertyPage.
//=============================================================================
CWaterMarkTabPropertyPage::CWaterMarkTabPropertyPage(
	HMODULE hStringResourceHandle,
	HMODULE hBitmapResourceHandle,
	HMODULE hDialogResourceHandle,
	ConstraintFeatureInfoMap &ConstraintFeatureInfoMap,
	PFEATUREINFOLIST &pFeatureInfoList,
	PaperInfoMap PaperInfo, 
	CustomPaperInfoMap CustomPaperInfoList,
	POEMDEV pOemPrivateDevMode,
    PDEVMODE pOemPublicDevMode,
	LPTSTR pHelpFilePath,DWORD *pHelpID,
	std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings,
	POEMDEV pPrivateDevmode,
    HANDLE hPrinter,
    WCHAR* pPrinterName,
	SHORT& sWaterMarkIndex
	) :
	CDocumentPropertyPage(
		hStringResourceHandle,
		pHelpFilePath,pHelpID,hPrinter,pPrinterName,hBitmapResourceHandle,hDialogResourceHandle
		),
		m_pWatermarkregdata(NULL),
		
		m_dwSelectedPaperWidthPixel(0),
		m_dwSelectedPaperHeightPixel(0),
		m_sOrientation(0),
		m_pWaterMarkPreview(NULL),
        m_pWaterMarkFileData(NULL),
        m_pIniFile(NULL),
		m_bIsWaterMarkDefaultClicked(FALSE) //Reverting Back changes for bugs#1665:04/24/2015:Debadatta
        //Reverting changes for bug# 1765 & 1665 to fix Watermark issues bug#s 2071,2087,2100,2104,2105 : 04/10/2015:Sonali
        /*,
		m_bIsWaterMarkDefaultClicked(FALSE),
        m_bIsWaterMarkTextEmpty(FALSE)*/

{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	m_ConstraintFeatureInfoMap = &ConstraintFeatureInfoMap;
	m_pFeatureInfoList = pFeatureInfoList;
	m_PaperInfo = PaperInfo;
	m_CustomPaperInfoList =  CustomPaperInfoList ;
	m_pOemPrivateDevMode = pOemPrivateDevMode ;
	m_pOemPublicDevMode = pOemPublicDevMode ;
	//m_hDialogResourceHandle = hDialogResourceHandle;
    m_wID				= IDD_TAB_WM_121_03;
	m_pHelpFilePath     = pHelpFilePath;
	m_pHelpID			= pHelpID;
	m_DefaultSettings = pDefaultSettings;
    m_pPrivateDevmode  = pPrivateDevmode;
	m_pWaterMarkIndex = &sWaterMarkIndex ;
    m_pCustomColor = new COLORREF[16]();
	if(NULL == m_pCustomColor)
	{
		slog.putLog("CWaterMarkTabPropertyPage::CWaterMarkTabPropertyPage -> NULL_HANDLE(m_pCustomColor)\n");
	}
	SecureZeroMemory(&m_PropertySheetState,sizeof(PROPSTATE));
	SecureZeroMemory(&m_WaterMarkData,sizeof(WATERMARKDATA));
	SecureZeroMemory(&m_WaterMarkDataChanged,sizeof(WATERMARKDATA));
	
    WCHAR szWaterMarkDatFilePath[_MAX_PATH] = {};
    GetProjectFileName(szWaterMarkDatFilePath, L"wm62.dat");
   
    m_pIniFile = new CShIniFile(ghInstance, szWaterMarkDatFilePath, FALSE);
	if(NULL == m_pIniFile)
	{
		slog.putLog("CWaterMarkTabPropertyPage::CWaterMarkTabPropertyPage -> NULL_OBJECT(m_pIniFile)\n");
	}

    // get language ID 
    // get language id from registry
    WCHAR szCommonDatFilePath[_MAX_PATH] = {};
    GetProjectFileName(szCommonDatFilePath,L"Common.DAT");
    
    CShIniFile cshLangFile(ghInstance, szCommonDatFilePath, FALSE);
    DWORD dwDefLangID = cshLangFile.GetDefaultLangID();
     
    WCHAR szPrinterName[MAX_PATH] = {};   
    DWORD			dwNeeded = 0;
    GetPrinter(hPrinter, 2, NULL, 0, &dwNeeded);
    BYTE* pTemp = NULL;
    if (0 < dwNeeded)
    {
        pTemp = new BYTE[dwNeeded]();
    }
    GetPrinter(hPrinter, 2, pTemp, dwNeeded, &dwNeeded);
    if(NULL != pTemp)
    {
        PRINTER_INFO_2* pPI2 = reinterpret_cast<PRINTER_INFO_2*>(pTemp);
        wcscpy_s(szPrinterName, MAX_PATH,pPI2->pPrinterName);
        delete[] pTemp;
        pTemp = NULL;
    }

    CRegistryAPI regApi;
    m_dwLangID = regApi.GetLangID(szPrinterName, dwDefLangID);

    m_pWaterMarkFileData = new CWaterMarkFileData(m_hStringResourceHandle, m_pIniFile, m_dwLangID);
	if( NULL == m_pWaterMarkFileData)
	{
		slog.putLog("CWaterMarkTabPropertyPage::CWaterMarkTabPropertyPage -> NULL_POINTER(m_pWaterMarkFileData)\n");
	}

	m_PropertySheetState.hcsHandUp	= ::LoadCursor(m_hBitmapResourceHandle, MAKEINTRESOURCE(IDC_HAND_UP));
    m_PropertySheetState.hcsHandDown= ::LoadCursor(m_hBitmapResourceHandle, MAKEINTRESOURCE(IDC_HAND_DOWN));

	//Coverity Fix 90389 - SSDI:Seetharam - 20200827
	m_bIsWaterMarkTextEmpty = FALSE;
	m_bWaterMarkChanged = FALSE;
	m_bIsWaterMarkTextEmpty = FALSE;
}

//=============================================================================
// function
//      CMainTabPropertyPage
//
// return value
//     NILL
//
// outline
//      Destructor to the class CWaterMarkTabPropertyPage.
//=============================================================================
CWaterMarkTabPropertyPage::~CWaterMarkTabPropertyPage(void)
{
	delete m_pWatermarkregdata;
	m_pWatermarkregdata = nullptr;

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
CWaterMarkTabPropertyPage::InitDlgBox(
    _Out_ LPCTSTR* ppszTemplate,
    _Out_ LPCTSTR* ppszTitle
    )
{
    HRESULT hr = S_OK;

	//Coverity Fix 90446 - SSDI:Seetharam - 20201020
	//Changed the condition from || to && because if any parameter is NULL, then
	//dereference happens and application can crash
	//if ((NULL != ppszTemplate) ||
	if ((NULL != ppszTemplate) && (NULL != ppszTitle))
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
//      CreateContorlClass
//
// return value
//     NILL
//
// outline
//      Create all controls.
//=============================================================================
void CWaterMarkTabPropertyPage::CreateContorlClass(void)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	HRESULT hr = S_OK;
	CUIControl* pControl = new CWaterMarkCombo(m_pWaterMarkFileData, m_pWatermarkregdata,m_PropertySheetState.wWmIndex,m_hStringResourceHandle,
                                m_pOemPublicDevMode->dmDeviceName, m_PropertySheetState.szWM, m_bWaterMarkChanged);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_CMB_WM, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(CWaterMarkCombo)\n");
	}
	pControl = new CWaterMarkScrollBar(m_PropertySheetState.wWmSize,6,300,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_SCR_WM_SIZE, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(CWaterMarkScrollBar)\n");
	}
	pControl = new CWaterMarkScrollBar(m_PropertySheetState.wWmAngle,-90,90,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		 hr = AddUIControl(IDC_SCR_WM_ANGLE, pControl);
		 if(S_OK != hr)
		 {
			slog.LoggingErrorHResultValues();
		 }
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(CWaterMarkScrollBar)\n");
	}
	pControl = new CWaterMarkScrollBar(m_PropertySheetState.lWmPosX,-50,50,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		 hr = AddUIControl(IDC_SCR_WM_HORZ, pControl);
		 if(S_OK != hr)
		 {
			slog.LoggingErrorHResultValues();
		 }
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(CWaterMarkScrollBar)\n");
	}
	pControl = new CWaterMarkScrollBar(m_PropertySheetState.lWmPosY,-50,50,m_hStringResourceHandle,-1);
	if (NULL !=pControl)
    {
		 hr = AddUIControl(IDC_SCR_WM_VERT, pControl);
		 if(S_OK != hr)
		 {
			slog.LoggingErrorHResultValues();
		 }
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(CWaterMarkScrollBar)\n");
	}
	pControl = new CWaterMarkEdit(m_PropertySheetState.szWM,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_EDT_WM_TEXT, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(CWaterMarkEdit)\n");
	}
#if 0 //watermark spec change
	pControl = new CButton(IDC_BTN_WM_ADD,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_BTN_WM_ADD, pControl);
    }
	pControl = new CButton(IDC_BTN_WM_DELETE,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_BTN_WM_DELETE, pControl);
    }
#endif
	pControl = new CButton(IDC_BTN_WM_EDIT,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_BTN_WM_EDIT, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_BTN_WM_EDIT)\n");
	}
	pControl = new CButton(IDC_BTN_WM_CENTER,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_BTN_WM_CENTER, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_BTN_WM_CENTER)\n");
	}
	pControl = new CWaterMarkCheckBox(m_PropertySheetState.wOnFirstPage,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_CHK_ON_1STPAGE, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    } 
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(CWaterMarkCheckBox)\n");
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
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_SYSLINK_TAB_DEFAULTS)\n");
	}
	pControl = new CWaterMarkPreview(IDC_BTN_WM_PREV,m_hPropertySheetHandle,&m_PropertySheetState,m_pOemPublicDevMode->dmDeviceName,m_hStringResourceHandle, m_hPrinterHandle);
    if (NULL !=pControl)
    {
        hr = AddUIControl(IDC_BTN_WM_PREV, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::CreateContorlClass -> NULL_POINTER(IDC_BTN_WM_PREV)\n");
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
void CWaterMarkTabPropertyPage::SetControlStrings(void)
{
	HRESULT hr = S_OK;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	CUIControl* pControl = new CStaticText(IDC_GRP_WM_WM,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_GRP_WM_WM, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_GRP_WM_WM)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_TEXT,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_TEXT, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_TEXT)\n");
	}
	pControl = new CStaticText(IDC_GRP_WM_PREV,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_GRP_WM_PREV, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_GRP_WM_PREV)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_PREVIMG,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_PREVIMG, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_PREVIMG)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_POS,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_POS, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_POS)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_X,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_X, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_X)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_X_VAL,m_hStringResourceHandle,(SHORT&)m_PropertySheetState.lWmPosX);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_X_VAL, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_X_VAL)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_Y,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_Y, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_Y)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_Y_VAL,m_hStringResourceHandle,(SHORT&)m_PropertySheetState.lWmPosY);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_Y_VAL, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_Y_VAL)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_SIZE,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_SIZE, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_SIZE)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_SIZE_VAL,m_hStringResourceHandle,m_PropertySheetState.wWmSize);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_SIZE_VAL, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_SIZE_VAL)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_ANGLE,m_hStringResourceHandle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_ANGLE, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_ANGLE)\n");
	}
	pControl = new CStaticText(IDC_STC_WM_ANGLE_VAL,m_hStringResourceHandle,m_PropertySheetState.wWmAngle);
	if (NULL !=pControl)
    {
		hr = AddUIControl(IDC_STC_WM_ANGLE_VAL, pControl);
		if(S_OK != hr)
		{
			slog.LoggingErrorHResultValues();
		}
    }
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::SetControlStrings -> NULL_POINTER(IDC_STC_WM_ANGLE_VAL)\n");
	}	
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
void CWaterMarkTabPropertyPage::GetWaterMarkRegistryObject(void)
{
#if 0 //watermark spec change
	TCHAR szWaterMarkDatFilePath[_MAX_PATH] = {0};
	GetProjectFileName(szWaterMarkDatFilePath,L"wm62.dat");
	CShIniFile* cshinifile = new CShIniFile(ghInstance,m_pOemPrivateDevMode->scpi.pub.dmDeviceName,szWaterMarkDatFilePath,FALSE);
	m_pWatermarkregdata = new CWaterMarkRegistryData(ghInstance,m_pOemPrivateDevMode->scpi.pub.dmDeviceName,m_hStringResourceHandle);
	m_pWatermarkregdata->reset(m_pOemPrivateDevMode->scpi.pub.dmDeviceName,cshinifile);
	m_pWatermarkregdata->ReadWMData(m_pOemPublicDevMode->dmDeviceName,m_pOemPrivateDevMode->scpi.ext.nWatermark,&m_WaterMarkData);
#endif
    m_PropertySheetState.wWmIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
   //<S><C>To the Support of the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla
    // if scdm watermark language and registry language is different, read the watermark data from file
   // if (m_pOemPrivateDevMode->scpi.ext.WaterMark.dwWMLangID == m_dwLangID)
	if (m_pOemPrivateDevMode->scpi.ext2.Watermark.data.dwWMLangID == m_dwLangID)
    {
        // copy watermark data from SCDM to m_WaterMarkData
       // memcpy_s(&m_WaterMarkData,sizeof(WATERMARKDATA),&(m_pOemPrivateDevMode->scpi.ext.WaterMark),sizeof(WATERMARKDATA));
		memcpy_s(&m_WaterMarkData, sizeof(WATERMARKDATA), &(m_pOemPrivateDevMode->scpi.ext2.Watermark.data), sizeof(WATERMARKDATA));
    }
    //<E>To the Support of the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla
    else
    {
        m_pWaterMarkFileData->GetWMDefData(m_PropertySheetState.wWmIndex, &m_WaterMarkData);
    }
	memcpy_s(&m_WaterMarkDataChanged,sizeof(WATERMARKDATA),&m_WaterMarkData,sizeof(WATERMARKDATA));
	
	WatermarkDataToPropState(&m_WaterMarkDataChanged);
#if 0 //watermark spec change
	delete cshinifile;
	cshinifile = NULL ;
#endif
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
void CWaterMarkTabPropertyPage::ProcessFeatureInfoList()
{
	UINT FeatureCount = (UINT)m_pFeatureInfoList[0].nNummberOfFeatures;
	BOOL bPaperSizeFeatureInfoFound = FALSE;
	BOOL bOrientationFeatureInfoFound = FALSE;
	UINT uFeatureIndex = 0;
	while(
			((FALSE == bPaperSizeFeatureInfoFound)||(FALSE == bOrientationFeatureInfoFound))&&
			(uFeatureIndex < FeatureCount)
		 )
	{
		if(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszFeature,"PaperSize"))
		{
			GetPaperSize(uFeatureIndex);
			bPaperSizeFeatureInfoFound = TRUE;
		}
		//<S><C> to implement Custom Orientation 13/8/2020 K M Harika
		if(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszFeature,"PageShOrientation"))
		//<E> to implement Custom Orientation 13/8/2020 K M Harika
		{
			GetOrientation(uFeatureIndex);
			bOrientationFeatureInfoFound = TRUE;
		}
		uFeatureIndex++ ;
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
void CWaterMarkTabPropertyPage::GetPaperSize(UINT uFeatureIndex)
{
	if(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszFeature,"PaperSize"))
	{
		if(!strcmp("CUSTOMSIZE",m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption))
		{
			m_dwSelectedPaperWidthPixel = m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel;
			m_dwSelectedPaperHeightPixel = m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel ;
		}
		else
		{
			PaperInfoMap::const_iterator it;
			it = m_PaperInfo.find(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption);
			if(it != m_PaperInfo.end())
			{
				DWORD dwImageableWidthPixel = it->second.PageImageaDimensions.first;
				DWORD dwImageableHeightPixel = it->second.PageImageaDimensions.second ;
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
void CWaterMarkTabPropertyPage::GetOrientation(UINT uFeatureIndex)
{
	if(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption,"PORTRAIT"))
	{
		m_sOrientation = DMORIENT_PORTRAIT ;
	}
	if(!strcmp(m_pFeatureInfoList->pFeatureOptionPair[uFeatureIndex].pszOption,"LANDSCAPE_CC90"))
	{
		m_sOrientation = DMORIENT_LANDSCAPE ;
	}
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
CWaterMarkTabPropertyPage::DeActivateUIControl(
    _In_ CONST HWND   hDlg
    )
{
	UNREFERENCED_PARAMETER(hDlg);
	HRESULT hr = S_OK;
//<S><A> RedMine Ticket(#989)
    // if custom water mark is selected, text is empty and move to any other tab and come back to watermark tab,
    // custom string should be displayed in edit box.
    if(TRUE == CheckSpaceOnly(m_PropertySheetState.szWM))
    {
        if ((m_pWaterMarkFileData->GetWMCount() - 1) == m_PropertySheetState.wWmIndex)
        {
            memcpy_s(m_PropertySheetState.szWM,_countof(m_PropertySheetState.szWM)*sizeof(WCHAR),m_WaterMarkData.szMainWM,_countof(m_WaterMarkData.szMainWM)*sizeof(WCHAR));
        }
    }
    PropStateToWatermarkData(&m_WaterMarkDataChanged);
    memcpy_s(&m_WaterMarkData,sizeof(WATERMARKDATA),&m_WaterMarkDataChanged,sizeof(WATERMARKDATA));
	SetWaterMarkIndex(m_PropertySheetState.wWmIndex);
//<E> RedMine Ticket(#989)
//<S><C> RedMine Ticket(#989)
    // message box is not displaying if watermark position, size etc,, are changed
#if 0
	BOOL blCancel = FALSE;
	LONG lRet = QuerySaveWmChanges(hDlg);
	switch(lRet)
	{
		case IDYES:
#if 0 //watermark spec change
			if(FALSE == OnWaterMarkAdd(hDlg))
			{
				hr = E_FAIL;
			}
			SetTabState(hDlg,IDC_BTN_WM_ADD);
#endif			
            memcpy_s(&m_WaterMarkData,sizeof(WATERMARKDATA),&m_WaterMarkDataChanged,sizeof(WATERMARKDATA));
            WatermarkDataToPropState(&m_WaterMarkData);
			break;
        case IDOK:
            {
                // When user clicks OK , Property Page should remain active.
                SetWindowLongPtr(hDlg,DWLP_MSGRESULT, TRUE);
                return TRUE ;
            }
            break;
		case IDCANCEL:
			blCancel = TRUE;
			
			break;
		case IDNO:

			memcpy_s(&m_WaterMarkDataChanged,sizeof(WATERMARKDATA),&m_WaterMarkData,sizeof(WATERMARKDATA));
			WatermarkDataToPropState(&m_WaterMarkData);
			if(0 == m_PropertySheetState.wWmIndex)
			{
				SecureZeroMemory(m_PropertySheetState.szWM,sizeof(m_PropertySheetState.szWM));
			}
		default:
			break;
	}
	if(TRUE == blCancel)
	{
		// When user clicks Cancel , Property Page should remain active.
		SetWindowLongPtr(hDlg,DWLP_MSGRESULT, TRUE);
		return TRUE ;
	}
#endif
    //<E> RedMine Ticket(#989)
   return hr;

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
CWaterMarkTabPropertyPage::OnQueryCancel(
    _In_ CONST HWND   hDlg
    )
{
	return DeActivateUIControl(hDlg);
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
CWaterMarkTabPropertyPage::ActivateUIControl(
    _In_ CONST HWND   hDlg
    )
{
    ProcessFeatureInfoList();
	//<S><A> Redmine Ticket(#1765):Watermark tab: Size, Angle and Position scroll bars blinks on clicking Apply button.:2015.04.29,SSDI:Debadatta Satpathy
	//When the watermark default is clicked, the m_bIsWaterMarkDefaultClicked get reset to FALSE in SendCommand() and the watermark index become 0 (Watermark type - NONE).
	//In that scenario call below code.
	if( (FALSE == m_bIsWaterMarkDefaultClicked) && (0 == m_PropertySheetState.wWmIndex))
	{
		return S_OK;
	}
	//<E> Redmine Ticket(#1765):Watermark tab: Size, Angle and Position scroll bars blinks on clicking Apply button.:2015.04.29,SSDI:Debadatta Satpathy

    //Reverting changes for bug# 1765 & 1665 to fix Watermark issues bug#s 2071,2087,2100,2104,2105 : 04/10/2015:Sonali
    //<S><A> Redmine Ticket(#1765):Watermark tab: Size, Angle and Position scroll bars blinks on clicking Apply button.:2015.01.27,SSDI:Debadatta Satpathy
#if 0
    if(PropSheet_Changed(GetParent(hDlg),hDlg))
        //<E> Redmine Ticket(#1765):Watermark tab: Size, Angle and Position scroll bars blinks on clicking Apply button.:2015.01.27,SSDI:Debadatta Satpathy
    {		
#endif
        return CDocumentPropertyPage::ActivateUIControl(hDlg);
#if 0
    }
    //<S><A> Redmine Ticket(#1765):Watermark tab: Size, Angle and Position scroll bars blinks on clicking Apply button.:2015.01.27,SSDI:Debadatta Satpathy
    else
    {
        PropSheet_UnChanged(GetParent(hDlg),hDlg);
        //<S><A> RedMine Ticket(#2071) Watermark: Text which is deleted from “Text:” textbox is not displayed again when Apply is clicked: 2015.03.16, SSDI:Sonali
        //When Watermark edit text box is empty, the edit box is re-written with "Custom" string.

        //<S><A> RedMine Ticket(#2087)Watermarks Tab: “Custom” watermark is displayed in text box when Defaults and Apply is clicked: 2015.03.18, SSDI:Sonali
        //Added additional check to know if selected watermark index is Custom watermark,
        //only then we need to set with "Custom" string when text is deleted from edit box.
        if((TRUE == m_bIsWaterMarkTextEmpty) && (6 == m_PropertySheetState.wWmIndex))
            //<E> RedMine Ticket(#2087)Watermarks Tab: “Custom” watermark is displayed in text box when Defaults and Apply is clicked: 2015.03.18, SSDI:Sonali
        {
            WCHAR szitem[WM_STR_LENGTH + 1] = {};
            LoadString(m_hStringResourceHandle,IDS_CUSTOM,szitem,_countof(szitem));
            SetDlgItemText(hDlg, IDC_EDT_WM_TEXT,szitem) ;
        }

        //<S><A> RedMine Ticket(#2071) Issue#2 2015.03.18, SSDI:Sonali
        //Resetting the flag once the text box is set with "Custom" string.
        m_bIsWaterMarkTextEmpty = FALSE;
        //<E> RedMine Ticket(#2071) Issue#2 2015.03.18, SSDI:Sonali

        //<E> RedMine Ticket(#2071) Watermark: Text which is deleted from “Text:” textbox is not displayed again when Apply is clicked: 2015.03.16, SSDI:Sonali
        return S_OK;
    }		
#endif
    //<E> Redmine Ticket(#1765):Watermark tab: Size, Angle and Position scroll bars blinks on clicking Apply button.:2015.01.27,SSDI:Debadatta Satpathy
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
LONG CWaterMarkTabPropertyPage::QuerySaveWmChanges(HWND hDlg)
{
	
	LONG wType = MB_YESNO;
	WCHAR szTemp[MESSAGELENGTH] = {0};
	WCHAR szMessage[MESSAGELENGTH + (WM_STR_LENGTH + 1)]={0};
	WCHAR szMessageTitle[MESSAGETITLELENGTH]={0};
	WCHAR szWaterMark[WM_STR_LENGTH +1]={0};
    
	//<S><A> RedMine Ticket(#938) 
	// When Watermark text box contains only spaces, “To valid this setting,..” 
	// message box displays: 2013.12.24, SSDI:Gajanana D S
	if(TRUE == CheckSpaceOnly(m_PropertySheetState.szWM))
	{
		return IDNO;
	}
	//<E> RedMine Ticket(#938) 
	// When Watermark text box contains only spaces, “To valid this setting,..” 
	// message box displays: 2013.12.24, SSDI:Gajanana D S
	if(0 == m_PropertySheetState.wWmIndex)
	{
		if(0 == wcslen(m_PropertySheetState.szWM))
		{
			return IDNO;
		}
		if (LoadString(m_hStringResourceHandle, IDS_INDEX0_WATERMARK, szTemp, countof(szTemp)) > 0)
		{
			if(0 == wcscmp(m_PropertySheetState.szWM,szTemp))
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

        if(0 == memcmp(&m_WaterMarkDataChanged,&m_WaterMarkData,sizeof(WATERMARKDATA)))
        {
            return wType;
        }

        if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_8, szTemp, countof(szTemp)) > 0)
        {
            //<S><A> RedMine Ticket(#838) UI values are not retaining: 2013.11.13, SSDI:Gajanana D S
            // Do not pass WaterMarkData structure member variable . 
            // Pass temporary WaterMarkData structure to read title only from registry.

            WATERMARKDATA watermarkdata = {0};
#if 0 //watermark spec change
            m_pWatermarkregdata->ReadWMData(m_pOemPublicDevMode->dmDeviceName,m_PropertySheetState.wWmIndex,&watermarkdata,szWaterMark,sizeof(szWaterMark));
#else
            m_pWaterMarkFileData->GetWMDefData(m_PropertySheetState.wWmIndex, &watermarkdata);
            wcscpy_s(szWaterMark, WM_STR_LENGTH +1, watermarkdata.szMainWM);
#endif
        }
        swprintf_s(szMessage, _countof(szMessage), szTemp, szWaterMark);
        //<E> RedMine Ticket(#838) UI values are not retaining: 2013.11.13, SSDI:Gajanana D S

        //<S><A> RedMine Ticket(#853) Watermarks:Save dialog box has, "Cancel" button missing.: 2013.11.18, SSDI:Gajanana D S
        // Message type should be MB_YESNOCANCEL
        wType = MB_YESNOCANCEL;
        //<E> RedMine Ticket(#853) Watermarks:Save dialog box has, "Cancel" button missing.: 2013.11.18, SSDI:Gajanana D S	
    }

    if (LoadString(m_hStringResourceHandle, IDS_MESTITLE_10, szMessageTitle, countof(szMessageTitle)) > 0)
	{
		//<S><A> RedMine Ticket(#812) Multiple message boxes are displayed on clicking on another tab, when 
		// watermark is not saved.: 2013.11.11, SSDI:Gajanana D S
		// Pass PRoperty sheet dialog handle to Message box as owner window.
		wType = MessageBox(hDlg,szMessage,szMessageTitle,wType);
		//<E> RedMine Ticket(#812) Multiple message boxes are displayed on clicking on another tab, when 
		// watermark is not saved.: 2013.11.11, SSDI:Gajanana D S
	} 
	return wType;
}
//=============================================================================
// function
//      InitializeUIControls
//
// return value
//     NILL
//
// outline
//    Initializes  UI control.
//=============================================================================
HRESULT CWaterMarkTabPropertyPage::InitializeUIControls(_In_ CONST HWND   hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	m_hPropertySheetHandle = hDlg;
	GetWaterMarkRegistryObject();
	ProcessFeatureInfoList();
	if(0 == m_PropertySheetState.wWmIndex)
	{
		SecureZeroMemory(m_PropertySheetState.szWM,sizeof(m_PropertySheetState.szWM));
	}
	CDocumentPropertyPage::InitializeUIControls(hDlg);
	UIControlMap::const_iterator Iterator = m_UIControls.find(IDC_BTN_WM_PREV);
	if(Iterator != m_UIControls.end())
	{
		m_pWaterMarkPreview = static_cast<CWaterMarkPreview*>(Iterator->second);
		if(NULL == m_pWaterMarkPreview)
		{
			slog.putLog("CWaterMarkTabPropertyPage::InitializeUIControls -> NULL_OBJECT(m_pWaterMarkPreview)\n");
		}
	}

    HWND hWMSCHandleX = NULL;
    HWND hWMSCHandleY = NULL;

    Iterator = m_UIControls.find(IDC_SCR_WM_HORZ);
	if(Iterator != m_UIControls.end())
	{
		CWaterMarkScrollBar* pWaterMarkHScroll = static_cast<CWaterMarkScrollBar*>(Iterator->second);
		if(NULL == pWaterMarkHScroll)
		{
			slog.putLog("CWaterMarkTabPropertyPage::InitializeUIControls -> NULL_OBJECT(pWaterMarkHScroll)\n");
		}
        else
            hWMSCHandleX = pWaterMarkHScroll->GetScrollBarHandle();
	}

    Iterator = m_UIControls.find(IDC_SCR_WM_VERT);
	if(Iterator != m_UIControls.end())
	{
		CWaterMarkScrollBar* pWaterMarkVScroll = static_cast<CWaterMarkScrollBar*>(Iterator->second);
		if(NULL == pWaterMarkVScroll)
		{
			slog.putLog("CWaterMarkTabPropertyPage::InitializeUIControls -> NULL_OBJECT(pWaterMarkVScroll)\n");
		}
        else
            hWMSCHandleY = pWaterMarkVScroll->GetScrollBarHandle();
	}

    if (m_pWaterMarkPreview)
    {
        m_pWaterMarkPreview->SetScBarClass(hWMSCHandleX, hWMSCHandleY);
    }
	SetButtonState(hDlg);
	CreateDepenedentContolMap();
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
HRESULT CWaterMarkTabPropertyPage::OnDrawItem(
        _In_ CONST HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        )
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(NULL != m_pWaterMarkPreview)
	{
		WATERMARKDATA WatermarkData = {0};
		memcpy_s(&WatermarkData,sizeof(WATERMARKDATA),&m_WaterMarkDataChanged,sizeof(WATERMARKDATA));
		PropStateToWatermarkData(&WatermarkData);
		m_pWaterMarkPreview->Draw(&WatermarkData,m_dwSelectedPaperWidthPixel,m_dwSelectedPaperHeightPixel,m_sOrientation);
	}
	else
	{
		slog.putLog("CWaterMarkTabPropertyPage::OnDrawItem -> NULL_POINTER(m_pWaterMarkPreview)\n");
	}
	return CDocumentPropertyPage::OnDrawItem(hDlg,wParam,lParam);
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
void CWaterMarkTabPropertyPage::CreateDepenedentContolMap(void)
{
	CreateMapElement(IDC_SCR_WM_SIZE,IDC_STC_WM_SIZE_VAL);
	CreateMapElement(IDC_SCR_WM_ANGLE,IDC_STC_WM_ANGLE_VAL);
	CreateMapElement(IDC_SCR_WM_HORZ,IDC_STC_WM_X_VAL);
	CreateMapElement(IDC_SCR_WM_VERT,IDC_STC_WM_Y_VAL);
	CreateMapElement(IDC_CMB_WM,IDC_SCR_WM_SIZE);
#if 0 //watermark spec change
    CreateMapElement(IDC_BTN_WM_ADD,IDC_CMB_WM);
#endif
	//<S><A> RedMine Ticket(#811) “On First Page Only” check box is in checked state when “None” is selected.: 2013.11.11, SSDI:Gajanana D S
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
	//<E> RedMine Ticket(#811) “On First Page Only” check box is in checked state when “None” is selected.: 2013.11.11, SSDI:Gajanana D S
	CreateMapElement(IDC_CMB_WM,iComboBoxDependentControlIDs,countof(iComboBoxDependentControlIDs));
	INT iCenterButtonDependentControlIDs[] = {
		IDC_SCR_WM_HORZ,
		IDC_SCR_WM_VERT,
		IDC_STC_WM_X_VAL,
		IDC_STC_WM_Y_VAL

	};
	CreateMapElement(IDC_BTN_WM_CENTER,iCenterButtonDependentControlIDs,countof(iCenterButtonDependentControlIDs));
	//<S><A> RedMine Ticket(#809) Watermark text is not deleted from Text field when deleted. : 2013.11.11, SSDI:Gajanana D S
	// Edit text control is dependent on Water mark delete button control. 
	// Whenever delete button is clicked, notification is sent to Edit text field. 
	// With this edit text field displays newly selected water mark string.
	INT iWaterMarkDeleteButtonDependentContolIDs[] =
	{
		IDC_CMB_WM,
		IDC_EDT_WM_TEXT
	};
	CreateMapElement(IDC_BTN_WM_DELETE,iWaterMarkDeleteButtonDependentContolIDs,countof(iWaterMarkDeleteButtonDependentContolIDs));
	//<E> RedMine Ticket(#809) Watermark text is not deleted from Text field when deleted. : 2013.11.11, SSDI:Gajanana D S
}

//=============================================================================
// function
//      OnScroll
//
// return value
//     NILL
//
// outline
//    Scroll notifications.
//=============================================================================

HRESULT
		 CWaterMarkTabPropertyPage::OnScroll(
        _In_ CONST HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        )
{
	CDocumentPropertyPage::OnScroll(hDlg,wParam,lParam);
	INT sControlID = GetDlgCtrlID((HWND)lParam);
	SetTabState(hDlg,sControlID);
	
	
	return S_OK;
}
//=============================================================================
// function
//      SendCommand
//
// return value
//     NILL
//
// outline
//   command notifications.
//=============================================================================
HRESULT
	CWaterMarkTabPropertyPage::SendCommand(
        _In_ CONST HWND   hDlg,
        _In_ CONST WPARAM wParam
       )
  
{
	CDocumentPropertyPage::SendCommand(hDlg,wParam);
	WORD wNotificationCode = HIWORD(wParam);
	WORD wContolID = LOWORD(wParam);
	switch(wNotificationCode)
	{
		case BN_CLICKED:
			OnButtonClicked(hDlg,wContolID);
			break;
		case CBN_SELCHANGE:
			OnComboboxSelectionChange(hDlg);
			break;
		
	}
	//<S><C> Redmine ticket (#1665) -Size, Angle and Position scroll bars blinks on clicking Apply button and NONE option from Drop.:2014.12.08,SSDI:Debadatta Satpathy
	//Avoid calling SetTabState and SetButtonState when "NONE" is pressed repeated, as it creates the scrollbar thumb to blink 
	//when Windows theme is classic. 
	
	//This condition is executed only when we click on default in watermarktab and watermark index is not NONE.
	//Scenario: Select Watermarkindex as DRAFT and then press on OK.Again open printerui dialog and click on the watermarktab.
	//Now click on the default in that dialog.The default values of the control will be displayed as it calls SetTabState().
	//After clicking on default, the watermark index becomes 0 and m_bIsWaterMarkDefaultClicked as TRUE in WaterMarkTab Property Page SetTabDefaults().

    //Reverting changes for bug# 1765 & 1665 to fix Watermark issues bug#s 2071,2087,2100,2104,2105 : 04/10/2015:Sonali
//#if 0 
	//Reverting changes for bugs# 1665 : 04/24/2015:Debadatta
    if((0 == m_PropertySheetState.wWmIndex) && (TRUE == m_bIsWaterMarkDefaultClicked))
    {
    	m_bIsWaterMarkDefaultClicked = FALSE;
    }
    //If the watermark index is not changed then don't call SetTabState() as it causes scrollbar thumb flickering.
    //For example if we click on NONE and again click on NONE, then no need to call SetTabState() due to above reasons.
	//<S><A> Redmine ticket #2182-Changes made to the custom watermark is not updated in Preview. :2015.05.15,SSDI:Debadatta Satpathy
	//Incase the WM index is custom, then only call SetTabState() in order to update the preview. Incase of any other 
	//WM index, return with S_OK.
	else if (FALSE == m_bWaterMarkChanged && ((m_pWaterMarkFileData->GetWMCount() - 1) != m_PropertySheetState.wWmIndex))
    {
		//<E> Redmine ticket #2182.
    	return S_OK;
    }
    //<E> Redmine ticket (#1665) -Size, Angle and Position scroll bars blinks on clicking Apply button and NONE option from Drop.:2014.12.08,SSDI:Debadatta Satpathy
    //Below statement is not called only when m_bWaterMarkChanged = FALSE
    	
//#endif

    SetTabState(hDlg,wContolID);

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
void  CWaterMarkTabPropertyPage::SetTabState( 
		_In_ CONST HWND   hDlg,
       	_In_ CONST INT iCtrlID
		)
{
	SetButtonState(hDlg);
	DependentContolIDMap::const_iterator IteratorToDepandentContolMap = m_DependentContolMap.find(iCtrlID);
	if(IteratorToDepandentContolMap != m_DependentContolMap.end())
	{
		std::vector<INT>DependentContolID = IteratorToDepandentContolMap->second;
		for(DWORD dwCount = 0; dwCount < DependentContolID.size(); dwCount++)
		{
			INT iControlMapId = DependentContolID[dwCount];
			m_UIControls[iControlMapId]->OnInit(iControlMapId,hDlg);
		
		}
	}
	//<S><A> RedMine Ticket(#870) None is displaying when custom watermark entering in text box.: 2013.11.19, SSDI:Gajanana D S
	// Always send changed watermarkdata structure for drawing.
	if(NULL != m_pWaterMarkPreview)
	{
		WATERMARKDATA WatermarkData = {0};
		memcpy_s(&WatermarkData,sizeof(WATERMARKDATA),&m_WaterMarkDataChanged,sizeof(WATERMARKDATA));
		PropStateToWatermarkData(&WatermarkData);
		m_pWaterMarkPreview->Draw(&WatermarkData,m_dwSelectedPaperWidthPixel,m_dwSelectedPaperHeightPixel,m_sOrientation);
	}
	//<S><A> RedMine Ticket(#870) None is displaying when custom watermark entering in text box.: 2013.11.19, SSDI:Gajanana D S
	
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
BOOL CWaterMarkTabPropertyPage::WatermarkDataToPropState(LPWATERMARKDATA pWaterMarkData)
{
	//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	//memcpy(m_PropertySheetState.szWMNameHash, m_pOemDevMode.scpi.ext2.stSubFavNames.szWMNameHash, sizeof(m_PropertySheetState.szWMNameHash));
	//(*m_pregwm).SelectTheCorrectFavItem(pszSvrPrnName, m_PropertySheetState.szWMNameHash, m_PropertySheetState.wWmIndex);
	//(*pext).nWatermark = (*pps).wWmIndex;
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	m_PropertySheetState.wOnFirstPage	= pWaterMarkData->wFieldWMD & DM_WM_FIRSTPAGEONLY ? BST_CHECKED : BST_UNCHECKED;
	m_PropertySheetState.lWmPosX		= (SHORT)pWaterMarkData->dwPosX;
	m_PropertySheetState.lWmPosY		= (SHORT)pWaterMarkData->dwPosY;
	m_PropertySheetState.wWmSize		= pWaterMarkData->nSize;
	m_PropertySheetState.wWmAngle		= pWaterMarkData->nAngle - 90;
	//<S><A> RedMine Ticket(#825) Watermark is saved only upto 51 characters.: 2013.11.11, SSDI:Gajanana D S
	// _countof Macro returns number of elements
	// For unicode strings this should be multiplied with size of WCHAR
	memcpy_s(m_PropertySheetState.szWM,_countof(m_PropertySheetState.szWM)*sizeof(WCHAR),pWaterMarkData->szMainWM,_countof(pWaterMarkData->szMainWM)*sizeof(WCHAR));
	//<S><E> RedMine Ticket(#825) Watermark is saved only upto 51 characters.: 2013.11.11, SSDI:Gajanana D S
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
BOOL CWaterMarkTabPropertyPage::PropStateToWatermarkData(LPWATERMARKDATA pWaterMarkData)
{
	pWaterMarkData->wFieldWMD = m_PropertySheetState.wOnFirstPage == BST_CHECKED ? 
								pWaterMarkData->wFieldWMD | DM_WM_FIRSTPAGEONLY: pWaterMarkData->wFieldWMD & ~DM_WM_FIRSTPAGEONLY ;
	pWaterMarkData->dwPosX  = m_PropertySheetState.lWmPosX	;
	pWaterMarkData->dwPosY	= m_PropertySheetState.lWmPosY	;
	pWaterMarkData->nSize	= m_PropertySheetState.wWmSize	;
	pWaterMarkData->nAngle  = m_PropertySheetState.wWmAngle + 90 ;
	memcpy_s(pWaterMarkData->szMainWM,_countof(pWaterMarkData->szMainWM)*sizeof(WCHAR),m_PropertySheetState.szWM,_countof(m_PropertySheetState.szWM)*sizeof(WCHAR));
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
void CWaterMarkTabPropertyPage::SaveFeatureOptions()
{
	m_pOemPrivateDevMode->scpi.ext.nWatermark = m_PropertySheetState.wWmIndex;
	PropStateToWatermarkData(&m_WaterMarkData);
	//<S><C>To the Support of the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla
    // copy watermark data into SCDM 
    //memcpy_s(&(m_pOemPrivateDevMode->scpi.ext.WaterMark), sizeof(WATERMARKINFO), &m_WaterMarkData, sizeof(WATERMARKINFO));
	memcpy_s(&(m_pOemPrivateDevMode->scpi.ext2.Watermark.data), sizeof(WATERMARKINFO), &m_WaterMarkData, sizeof(WATERMARKINFO));
    //<S><A> RedMine Ticket(#2071) Watermark: Text which is deleted from “Text:” textbox is not displayed again when Apply is clicked: 2015.03.16, SSDI:Sonali
    //<E>To the Support of the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla
    
    //Checking if Watermark edit text box is empty, then make m_bIsWaterMarkTextEmpty as TRUE.
    WCHAR szWaterMarkText[WM_STR_LENGTH + 1] = {};
    GetDlgItemText(m_hPropertySheetHandle,IDC_EDT_WM_TEXT,szWaterMarkText,(WM_STR_LENGTH + 1));
    unsigned char p = szWaterMarkText[0];
    if((0 == wcscmp(L" ",szWaterMarkText)) || (0 == wcscmp(L"",szWaterMarkText)) || (0x09 == p) || (0x3000 == p))
    {
        m_bIsWaterMarkTextEmpty = TRUE;
    }
    //<E> RedMine Ticket(#2071) Watermark: Text which is deleted from “Text:” textbox is not displayed again when Apply is clicked: 2015.03.16, SSDI:Sonali
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
LRESULT CWaterMarkTabPropertyPage::OnComboboxSelectionChange(HWND hDlg)
{
#if 0 //watermark spec change
	m_pWatermarkregdata->ReadWMData(m_pOemPublicDevMode->dmDeviceName,m_PropertySheetState.wWmIndex,&m_WaterMarkDataChanged);
	memcpy_s(&m_WaterMarkData,sizeof(WATERMARKDATA),&m_WaterMarkDataChanged,sizeof(WATERMARKDATA));
	WatermarkDataToPropState(&m_WaterMarkData);
	if(0 == m_PropertySheetState.wWmIndex)
	{
		SecureZeroMemory(m_PropertySheetState.szWM,sizeof(m_PropertySheetState.szWM));
	}
	SetButtonState(hDlg);
#endif
    // get watermark data from file
//<S><C> RedMine Ticket(#989)
// if same index is selected, not reading data from dat file
    if (m_bWaterMarkChanged)
    {
        WATERMARKDATA stTempWaterMark = {0};
   
    m_pWaterMarkFileData->GetWMDefData(m_PropertySheetState.wWmIndex, &stTempWaterMark);
   
    if (0 == m_PropertySheetState.wWmIndex)
    {
        wcscpy_s(stTempWaterMark.szMainWM, WM_STRING_LENGTH + 1, m_PropertySheetState.szWM);
    }
    memcpy_s(&m_WaterMarkDataChanged,sizeof(WATERMARKDATA),&stTempWaterMark,sizeof(WATERMARKDATA));
    memcpy_s(&m_WaterMarkData,sizeof(WATERMARKDATA),&m_WaterMarkDataChanged,sizeof(WATERMARKDATA));
	WatermarkDataToPropState(&m_WaterMarkData);
    }
//<E> RedMine Ticket(#989)
#if 0
	if(0 == m_PropertySheetState.wWmIndex)
	{
		SecureZeroMemory(m_PropertySheetState.szWM,sizeof(m_PropertySheetState.szWM));
	}
#endif
	SetButtonState(hDlg);
	return S_OK;
}
//=============================================================================
// function
//      OnButtonClicked
//
// return value
//     NILL
//
// outline
//   Processes button click events.
//=============================================================================
LRESULT CWaterMarkTabPropertyPage::OnButtonClicked(HWND hDlg,WORD wContolID)
{
	switch(wContolID)
	{
#if 0 //watermark spec change
		case IDC_BTN_WM_ADD:
			OnWaterMarkAdd(hDlg);
		break;
		case IDC_BTN_WM_DELETE:
			OnWaterMarkDelete(hDlg);
			break;
#endif
		case IDC_BTN_WM_EDIT :
			OnWaterMarkEdit(hDlg);
			break;
		case IDC_BTN_WM_CENTER:
			OnWaterMarkCenter();
            PropSheet_Changed(GetParent(hDlg), hDlg);
			break;
		default:
			break;

	}
	return S_OK;
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
BOOL CWaterMarkTabPropertyPage::OnWaterMarkAdd(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(FALSE == IsAddWaterMark())
	{
		WCHAR szMessage[MAX_PATH] ={ 0};
        WCHAR szItem[MAX_PATH] = {0};
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_47, szMessage, countof(szMessage)) > 0)
		{
            LoadString(m_hStringResourceHandle, IDS_MESTITLE_1, szItem, countof(szItem));
			MessageBox(hDlg,szMessage,szItem,MB_ICONINFORMATION | MB_OK);
		}
		return FALSE;
	}
	CWaterMarkAddDialog *pdlg = new CWaterMarkAddDialog(IDD_DLG_WM_SAVE,m_hStringResourceHandle,m_PropertySheetState.szWM);
	//pdlg->SetPrinterName(m_pOemPublicDevMode->dmDeviceName);
	if(NULL == pdlg)
	{
		slog.putLog("CWaterMarkTabPropertyPage::OnWaterMarkAdd -> NULL_OBJECT(IDD_DLG_WM_SAVE)\n");
		return FALSE;
	}
	//Coverity Fix 90497 - SSDI:Seetharam - 20200908
	//Moved the pdlg->SetPrinterName after the NULL check for pdlg from above 
	//to here so that all dereferences are checked
	pdlg->SetPrinterName(m_pOemPublicDevMode->dmDeviceName);

	INT_PTR DlgRet = IDCANCEL;
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_WM_SAVE),
		hDlg, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
	if(IDOK == DlgRet)
	{
		AddWaterMark(pdlg->GetWaterMarkIndex(),pdlg->GetSavedWaterMarkString());
		OnComboboxSelectionChange(hDlg);
		//<S><A> RedMine Ticket(#814) "Apply" button should not be enabled. : 2013.11.11, SSDI:Gajanana D S
		//Send Property sheet change information to property sheet dialog only during following events. 
		// 1. Water mark combo is changed. 
		// 2. User clicks "Edit" dialog and changed some settings in "Edit dialog". 
		// 3. User clicks "Yes " on "Add" dialog Confirmation message box. 
		// 4. User clicks "Yes " on "Delete" dialog Confirmation message box. 
		PropSheet_Changed(GetParent(hDlg), hDlg);
		//<E> RedMine Ticket(#814) "Apply" button should not be enabled. : 2013.11.11, SSDI:Gajanana D S
	}
	else
	{
		memcpy_s(&m_WaterMarkDataChanged,sizeof(WATERMARKDATA),&m_WaterMarkData,sizeof(WATERMARKDATA));
		WatermarkDataToPropState(&m_WaterMarkData);
		if(0 == m_PropertySheetState.wWmIndex)
		{
			SecureZeroMemory(m_PropertySheetState.szWM,sizeof(m_PropertySheetState.szWM));
		}
	}
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
BOOL CWaterMarkTabPropertyPage::OnWaterMarkEdit(HWND hDlg)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	PropStateToWatermarkData(&m_WaterMarkDataChanged);
	CWatermarkEditDialog *pdlg = new CWatermarkEditDialog(IDD_DLG_WM_EDT_11,m_hStringResourceHandle,m_WaterMarkDataChanged, m_pCustomColor, m_PropertySheetState.wWmIndex,m_dwLangID, m_hPrinterHandle);
	if(NULL == pdlg)
	{
		slog.putLog("CWaterMarkTabPropertyPage::OnWaterMarkEdit -> NULL_POINTER(IDD_DLG_WM_EDT_11)\n");
		return FALSE;
	}
	pdlg->SetPrinterName(m_pOemPublicDevMode->dmDeviceName);
	INT_PTR DlgRet = IDCANCEL;
	DlgRet = ::DialogBoxParam(m_hDialogResourceHandle, MAKEINTRESOURCE(IDD_DLG_WM_EDT_11),
		hDlg, (DLGPROC)pdlg->WindowMapProc, (LPARAM)pdlg);
	if(IDOK == DlgRet)
	{
		LPWATERMARKDATA pWaterMarkData = pdlg->GetWatermarkDataAddr();
		if(0 != memcmp(&m_WaterMarkDataChanged,pWaterMarkData,sizeof(WATERMARKDATA)))
		{
			PropSheet_Changed(GetParent(hDlg), hDlg);
			memcpy_s(&m_WaterMarkDataChanged,sizeof(WATERMARKDATA),pWaterMarkData,sizeof(WATERMARKDATA));
			WatermarkDataToPropState(&m_WaterMarkDataChanged);

            //Reverting changes for bug# 1765 & 1665 to fix Watermark issues bug#s 2071,2087,2100,2104,2105 : 04/10/2015:Sonali
#if 0
            //<S><A> RedMine Ticket(#2062) Watermark: Values are not getting updated when changes are made to saved Custom Watermark: 2015.03.16, SSDI:Sonali
            //Setting focus to Watermark tab when exiting from Watermark Edit dialog.
            SetFocus(hDlg);
            //<E> RedMine Ticket(#2062) Watermark: Values are not getting updated when changes are made to saved Custom Watermark: 2015.03.16, SSDI:Sonali

#endif
		}
		
	}
	delete pdlg;
	pdlg = NULL;
	return TRUE;
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
BOOL CWaterMarkTabPropertyPage::OnWaterMarkDelete(HWND hDlg)
{
	m_pWatermarkregdata->ReadWMData(m_pOemPublicDevMode->dmDeviceName,m_PropertySheetState.wWmIndex,&m_WaterMarkDataChanged);
	WCHAR szMessageTitle[768]={0};
	if (LoadString(m_hStringResourceHandle, IDS_MESTITLE_11, szMessageTitle, countof(szMessageTitle)) > 0)
	{
		WATERMARKDATA WaterMarkDataElement = {0}; 
		WCHAR szWaterMarkTitle[SCUI_MAX_STRING] = {0};
		m_pWatermarkregdata->ReadWMData(m_pOemPublicDevMode->dmDeviceName,m_PropertySheetState.wWmIndex,&WaterMarkDataElement,szWaterMarkTitle,sizeof(szWaterMarkTitle));
		//<S><A> RedMine Ticket(#822) Watermark is deleting when pressing 'NO'..: 2013.11.11, SSDI:Gajanana D S
		// Do not delete Water mark when user clicks "No" on Water Mark delete confirmation message.
		if(IDNO ==  MessageBox(hDlg,szWaterMarkTitle,szMessageTitle,MB_YESNO))
		{
			return FALSE;
		}
		//<E> RedMine Ticket(#822) Watermark is deleting when pressing 'NO'..: 2013.11.11, SSDI:Gajanana D S
	} 
	m_pWatermarkregdata->DeleteWMData(m_PropertySheetState.wWmIndex);
	m_PropertySheetState.wWmIndex--;
	OnComboboxSelectionChange(hDlg);
	memcpy_s(&m_WaterMarkData,sizeof(WATERMARKDATA),&m_WaterMarkDataChanged,sizeof(WATERMARKDATA));
	PropSheet_Changed(GetParent(hDlg), hDlg);
	return TRUE;
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
void CWaterMarkTabPropertyPage::OnWaterMarkCenter(void)
{
	m_PropertySheetState.lWmPosX = 0 ;
	m_PropertySheetState.lWmPosY = 0 ;
	
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
BOOL CWaterMarkTabPropertyPage::IsAddWaterMark()
{
	DWORD dwCount = m_pWatermarkregdata->ReadWMCount(m_pOemPublicDevMode->dmDeviceName);
	if(MaximumNumberOfWaterMarks < dwCount)
	{
		return FALSE ;
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
BOOL CWaterMarkTabPropertyPage::AddWaterMark(SHORT sWaterMarkIndex,WCHAR* szTitle)
{
	PropStateToWatermarkData(&m_WaterMarkDataChanged);
	m_pWatermarkregdata->WriteWMData(m_pOemPublicDevMode->dmDeviceName,sWaterMarkIndex,&m_WaterMarkDataChanged,szTitle);
	DWORD dwNumberOfWaterMarks = m_pWatermarkregdata->ReadWMCount(m_pOemPublicDevMode->dmDeviceName);
	if(sWaterMarkIndex == (SHORT)dwNumberOfWaterMarks)
	{
		m_pWatermarkregdata->WriteWMCount(m_pOemPublicDevMode->dmDeviceName,++dwNumberOfWaterMarks);
	}
	m_PropertySheetState.wWmIndex = sWaterMarkIndex ;
	memcpy_s(&m_WaterMarkData,sizeof(WATERMARKDATA),&m_WaterMarkDataChanged,sizeof(WATERMARKDATA));
	return TRUE ;
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
void CWaterMarkTabPropertyPage::SetButtonState(HWND hDlg)
{
#if 0 //watermark spec change
	BOOL bAddWaterMark = FALSE ;
    
	
	if(	
		(0 !=  wcslen(m_PropertySheetState.szWM)) &&
		(FALSE == CheckSpaceOnly(m_PropertySheetState.szWM))
		)
	{
		bAddWaterMark = TRUE ;
	}

	if(0 == m_PropertySheetState.wWmIndex)
	{
		SetControlState(hDlg,IDC_BTN_WM_DELETE,SCUI_STATE_SHOW);
		if(TRUE == bAddWaterMark)
		SetControlState(hDlg,IDC_BTN_WM_ADD,SCUI_STATE_ON);
		else
		SetControlState(hDlg,IDC_BTN_WM_ADD,SCUI_STATE_SHOW);
	}
	else
	{
		SetControlState(hDlg,IDC_BTN_WM_DELETE,SCUI_STATE_ON);
		if(TRUE == bAddWaterMark)
		{
			PropStateToWatermarkData(&m_WaterMarkDataChanged);
			if(0 == memcmp(&m_WaterMarkDataChanged,&m_WaterMarkData,sizeof(WATERMARKDATA)))
			{
				SetControlState(hDlg,IDC_BTN_WM_ADD,SCUI_STATE_SHOW);
			}
			else
			{
				SetControlState(hDlg,IDC_BTN_WM_ADD,SCUI_STATE_ON);
			}
		}
		else
		SetControlState(hDlg,IDC_BTN_WM_ADD,SCUI_STATE_SHOW);
	}
#else 
    BYTE cStatus = SCUI_STATE_ON;
    BYTE cEditStatus = SCUI_STATE_ON;

    if (m_PropertySheetState.wWmIndex == 0)
    {
        cStatus = SCUI_STATE_SHOW;
        cEditStatus = SCUI_STATE_SHOW;
    }
    else if (m_PropertySheetState.wWmIndex < m_pWaterMarkFileData->GetWMCount() - 1)
    {
        cEditStatus = SCUI_STATE_SHOW;
    }

    SetControlState(hDlg,IDC_BTN_WM_EDIT,cEditStatus);
    SetControlState(hDlg,IDC_EDT_WM_TEXT,cEditStatus);
    SetControlState(hDlg,IDC_STC_WM_TEXT,cEditStatus);

    SetControlState(hDlg,IDC_SCR_WM_SIZE,cStatus);
    SetControlState(hDlg,IDC_SCR_WM_ANGLE,cStatus);
    SetControlState(hDlg,IDC_SCR_WM_HORZ,cStatus);
    SetControlState(hDlg,IDC_SCR_WM_VERT,cStatus);
    SetControlState(hDlg,IDC_BTN_WM_CENTER,cStatus);
    SetControlState(hDlg,IDC_CHK_ON_1STPAGE,cStatus);
    SetControlState(hDlg,IDC_STC_WM_POS,cStatus);
    SetControlState(hDlg,IDC_STC_WM_X,cStatus);
    SetControlState(hDlg,IDC_STC_WM_X_VAL,cStatus);
    SetControlState(hDlg,IDC_STC_WM_Y,cStatus);
    SetControlState(hDlg,IDC_STC_WM_Y_VAL,cStatus);
    SetControlState(hDlg,IDC_STC_WM_SIZE,cStatus);
    SetControlState(hDlg,IDC_STC_WM_SIZE_VAL,cStatus);
    SetControlState(hDlg,IDC_STC_WM_ANGLE,cStatus);
    SetControlState(hDlg,IDC_STC_WM_ANGLE_VAL,cStatus);
    
    
#endif
}
//=============================================================================
// function
//      SetTabDefaults
//
// return value
//     NILL
//
// outline
//  Sets all controls in watermark to their default values..
//=============================================================================
HRESULT CWaterMarkTabPropertyPage::SetTabDefaults(_In_ CONST HWND   hDlg) 
{
#if 0 //watermark spec change
	m_pWatermarkregdata->ReadWMData(m_pOemPublicDevMode->dmDeviceName, m_PropertySheetState.wWmIndex,&m_WaterMarkData);
#else
    //<S><A> RedMine Ticket(#961) 
	// Size" and "Angle" values are not going to default when clicking "Default" button first time:2014.01.08, SSDI:Sonali
    //For settings, we are reading from private devmode,hence commenting the call to below function
    m_pWaterMarkFileData->GetWMDefData(m_pPrivateDevmode->scpi.ext.nWatermark, &m_WaterMarkData);
    //m_pWatermarkregdata->ReadWMData(m_pOemPublicDevMode->dmDeviceName,m_pPrivateDevmode->scpi.ext.nWatermark,&m_WaterMarkData);        
    //<E> RedMine Ticket(#961) 
	// Size" and "Angle" values are not going to default when clicking "Default" button first time:2014.01.08, SSDI:Sonali

#endif
    memcpy_s(&m_WaterMarkDataChanged,sizeof(WATERMARKDATA),&m_WaterMarkData,sizeof(WATERMARKDATA));
    if(0 == m_PropertySheetState.wWmIndex)
    {
        SecureZeroMemory(m_PropertySheetState.szWM,sizeof(m_PropertySheetState.szWM));
    }
    WatermarkDataToPropState(&m_WaterMarkDataChanged);

    //Reverting changes for bug# 1765 & 1665 to fix Watermark issues bug#s 2071,2087,2100,2104,2105 : 04/10/2015:Sonali
//#if 0
	//Reverting changes for bugs# 1665 : 04/24/2015:Debadatta
	m_bIsWaterMarkDefaultClicked = TRUE;
//#endif
    //<S><A> RedMine Ticket(#961) 
	// Size" and "Angle" values are not going to default when clicking "Default" button first time:2014.01.08, SSDI:Sonali
    // Moving the call to " __super::SetTabDefaults(hDlg);" from beginning to end in order to execute custom defaults first
    // and then the base class defaults
    __super::SetTabDefaults(hDlg);
    //<E> RedMine Ticket(#961) 
	// Size" and "Angle" values are not going to default when clicking "Default" button first time:2014.01.08, SSDI:Sonali
	return S_OK;
}