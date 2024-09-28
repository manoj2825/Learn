//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : DevicePropertyPage.h
//
//  Module        : PrinterUi Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Sujan
//
//  Date		  : 27 September 2013
//
//	Description   : Implementation of DevicePropertyPage class

//===========================================================================

//===========================================================================
//  Reviews 
//
//  Sl#     Review Date     Reviewer(s)               Remarks
//===========================================================================

#include "shregmt.h"

#pragma once
typedef std::map<INT_PTR, CUIControl*> UIControlMap;

class CDevicePropertyPage
{
public:
// <S><M> Modified the constructor for additiona parameters required for ComboBox, 20200414 - SSDI:Seetharam
/*	CDevicePropertyPage(HANDLE hPrinter,HMODULE hStringResourceHandle,HMODULE hBitmapResourceHandle,HMODULE hDialogResourceHandle,
						WCHAR* pPrinterName,BOOL bIsPrinterIcon,
						LPTSTR pHelpFilePath,
						DWORD *pHelpID);*/
	CDevicePropertyPage(HANDLE hPrinter,HMODULE hStringResourceHandle,HMODULE hBitmapResourceHandle,HMODULE hDialogResourceHandle,
						WCHAR* pPrinterName,BOOL bIsPrinterIcon,
						LPTSTR pHelpFilePath,
						DWORD *pHelpID, PFEATUREINFOLIST &pFeatureInfoList,
						std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings, ResourceIDInfoMap* pResourceIDInfoMap, IPrintCoreHelperUni* pPrintCoreHelperUni, HINSTANCE hInstance, SHORT& wUsIndex, PPROPINFO ppi);
// <S><E> Modified the constructor for additiona parameters required for ComboBox, 20200414 - SSDI:Seetharam
	virtual ~CDevicePropertyPage(void);

	virtual HRESULT
    PropPageInit(
        _In_ CONST PPROPSHEETUI_INFO pPSUIInfo
        );

	static INT_PTR CALLBACK
    DlgProc(
        _In_ HWND   hDlg,
        _In_ UINT   uiMessage,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam
        );

	VOID UpdateMimic(VOID);

	// <S><A> To add owner ondraw for handling the custom controls like combox with image etc., 20200414 - SSDI:Seetharam
	 HRESULT OnWmDrawItem(_In_ HWND   hDlg,
		_In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
	);
	// <S><E> To add owner ondraw for handling the custom controls like combox with image etc., 20200414 - SSDI:Seetharam

    BOOL GetPrinterName(WCHAR *pPrinterName);
    BOOL InitItems(BOOL blForceMB,HWND hWnd);    
    void SetPreviousItemIndex(long lCheckPos);
    void SetCurrentItemIndex(long uChangedOptionIndex);
    void SetDeviceType(short wDeviceType);
    void PsnSetActive(HWND hDlg);
    void PsnApply(HWND hDlg);
// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	void SetControlState(HWND hDlg, short wCtrlID, BYTE cStatus);
	void SetTabState(
		_In_ CONST HWND   hDlg, BOOL bSilentAutoConfig = FALSE);
// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V

// <S><A> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
	void DevmodeToPropstate();
	VOID  SetDevmode(HANDLE hPrinter, OEMDEV	*pOemDevMode);
	BOOL DevdmodeToReg(char *pszSvrPrnName, LPSCDM pscdm);
	HWND m_hPropertySheetHandle;
	OEMDEV         m_pOemDevMode;
	DEVMODE		   m_OemPublicDevMode;
	SHORT m_DeviceType;
// <E> Implementing config tab saving the values in devmode,2020.05.25, SSDI:Chandrashekar .V
// <S><A> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V	
	void PropstatetoDevmode();
	void ResetDevDmode(HWND hDlg);
	UINT GetDefaultGPDOption(HWND hDlg, UINT iControlID, UINT wfeatureIndex, BOOL flag);
// <E> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V
	LRESULT ExecuteAutoConfiguration(bool bSilentAutoConfig = false);
    WCHAR m_pPrinterName[MAX_PATH];
    long m_uPreviousItemIndex;
    long m_uCurrentItemIndex;
    BOOL m_bIsPrinterIcon;
	BOOL m_bBWPrint;
	DWORD m_dwSetupType;
	BOOL m_bDefaultClicked;
	BOOL m_bColorBWSelected;
	BOOL m_bExecuteSilentAutoConfig;
	SHORT* m_pUsIndex;
	PPROPINFO m_ppi;
	BOOL m_bIsSilentAutoConfig;
protected:
	HRESULT
    RemovePropertyPageInstance(
        _In_ CONST HWND hDlg
        );

	HRESULT
    SetComPropSheetFunc(
        _In_ CONST PFNCOMPROPSHEET pfnComPropSheet
        );

    HRESULT
    SetPageHandle(
        _In_ CONST HANDLE hPage
        );

    HRESULT
    SetComPropSheetHandle(
        _In_ CONST HANDLE hComPropSheet
        );

    HRESULT
    GetComPropSheetFunc(
        _Outptr_ PFNCOMPROPSHEET* ppfnComPropSheet
        );

    HANDLE
    GetPageHandle(
        VOID
        );

    HANDLE
    GetComPropSheetHandle(
        VOID
        );

	HRESULT
    AddUIControl(
        _In_ CONST INT iCtrlID,
        _In_ CUIControl* pUIControl
        );

	HRESULT
    InitDlgBox(
        _Out_ LPCTSTR* ppszTemplate,
        _Out_ LPCTSTR* ppszTitle
        );

	 HRESULT 
	 SendCommand(
        _In_ CONST HWND   hDlg,
        _In_ CONST WPARAM wParam
        );
	 
	HRESULT 
	InitializeUIControls(
		_In_ CONST HWND   hDlg
    );
    HRESULT
ActivateUIControl(
    _In_ CONST HWND   hDlg
    );
	IPrintCoreHelperUni* m_pPrintCoreHelperUni;
	PROPSTATE m_PropertySheetState;
	void WriteDEVDModeValueToRegistry();
	HRESULT  ShowHelp(LPHELPINFO pHelpInformation);
// <S><A> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V
	virtual HRESULT SetTabDefaults(_In_ CONST HWND   hDlg);
// <E> Implementing config tab Default function,2020.06.10, SSDI:Chandrashekar .V

private:
	HRESULT	StorePropertyPageInstance(_In_ CONST HWND hDlg);
	static HRESULT RetrievePropertyPageInstance( _In_ CONST HWND  hDlg, _Outptr_ CDevicePropertyPage** pDocPropPage);
	VOID DestroyUIComponents(VOID);
	VOID SetControlStrings(VOID);
	BOOL CreatePPLCYDlg(HWND hDlg);
	// <S><A> Added the Configuration tab related features, 20200414 - SSDI:Seetharam
	VOID CreateContorlClass();
	PRESOURCEIDINFO GetResourceIDInfoOfControl(std::string strFeatureName);
	VOID CreateMessageClass(_In_ CONST HWND   hDlg);
	// <S><A> Added the Configuration tab related features, 20200414 - SSDI:Seetharam

	LRESULT OnIdcBtnPrnSelCfg(_In_ CONST HWND   hDlg);
	LRESULT OnIdcClickSPN(_In_ CONST HWND   hDlg);


	void StapleModuleToStapleType(PPROPSTATE pps);

	void PunchModuleToPunchType(PPROPSTATE pps);

	SHORT UpdatePunchModuleToPropstate();

	SHORT UpdatePropstateToPunchModule();

	VOID MimicConstraintsForPunchStaple();

	VOID SelectComboFeatureOption(HWND hDlg, WORD wCtrlID, LONG lVal);

	VOID SelectCheckBoxFeatureOption(HWND hDlg, WORD wCtrlID, LONG lVal);

	void EnableOrDisableControl(HWND hDlg, short wDialogID, short wStringID, BYTE cControlStatus);


	HINSTANCE					 m_hInstance;
	HANDLE						 m_hPrinter;
	HMODULE						 m_hStringResourceHandle;
	HMODULE						 m_hBitmapResourceHandle;
	HMODULE						 m_hDialogResourceHandle;
	HANDLE                       m_hPage;
	HANDLE                       m_hComPropSheet;
    PFNCOMPROPSHEET              m_pfnComPropSheet;
	TCHAR						 m_szItem[MAX_PATH];
	UIControlMap                 m_UIControls;
	LPTSTR						 m_pHelpFilePath;
	DWORD						 *m_pHelpID;
    DWORD                        m_dwLangID;
// <S><A> To add color-mode UD3L, 20200128 - SSDI:Seetharam
	DWORD						 m_dwColorMode;
	BOOL						 m_bDisablePrnSel;
	
// <S><E> To add color-mode UD3L, 20200128 - SSDI:Seetharam
    std::vector<PUDMRI> RegisteredPrintersPudmriVector;
	// <S><A> Added the additional parameters required for combox, 20200414 - SSDI:Seetharam
	ResourceIDInfoMap* m_pResourceIDInfoMap;
	PFEATUREINFOLIST m_pFeatureInfoList;
	std::vector<DEFAULTDEVMODEINFO> m_DefaultSettings;
	CShowMessage* m_ObjCShowMessage;
	// <S><A> Added the additional parameters required for combox, 20200414 - SSDI:Seetharam
	CShIniFile			*m_pmcf;
	BOOL InitCommonFile(HINSTANCE hInst);
	void UpdateDevicetypeAndColorInfo(BOOL bBWPrint, short & wDeviceType, BOOL bIsIndexChanged, BOOL bIsManuallyEdited);
	BOOL bIsSilentAutoConfig();

	LRESULT OnIdcBtnPrnClicked(_In_ CONST HWND   hDlg);
	BOOL UpdateFeatureListBasedOnDevMode( POEMDEV pOemPrivateDevMode);
	BOOL m_dwIsPrinterIcon;
	DLGMFPSLCT_INFO	*m_MfpSlctInfo;
	void GetBWPrint(CShRegMdlTbl *pRegMdlTbl);
	virtual BOOL CreateMfpSlctInfo(void);
	virtual BOOL DeleteMfpSlctInfo(void);
	BOOL IsPrinterIcon();
	void CheckPrinterIcon(WCHAR FAR *pszSvrPrnName);
	BOOL DisableAllCtrls(short *pwExcept = NULL);
	BOOL DisablePrinterSelection();
	BOOL LoadStringResource(WCHAR * pPrinterName, WCHAR *pszPath);
	//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	BOOL ApplyShareFav();
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
	BOOL ApplyShareFavForJson();
};
