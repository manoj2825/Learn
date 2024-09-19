//	Copyright 2020(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//======================================================================================
//  File          : WaterMarkDialog.h
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

#pragma once
#include "UIControl.h"
#include "shJsonwm.h"

class CWaterMarkDialog : public CUIControl
{
public:
	CWaterMarkDialog(UINT uContorlID, HMODULE hStringResourceHandle,
		HANDLE hPrinter, HMODULE	hDialogResourceHandle,
		WCHAR* pPrinterName,
		ResourceIDInfoMap* pResourceIDInfoMap,
		PFEATUREINFOLIST &pFeatureInfoList,
		std::vector<DEFAULTDEVMODEINFO> &pDefaultSettings,
		HMODULE hBitmapResourceHandle,
		PDEVMODE pOemPublicDevMode,
		POEMDEV pPrivateDevmode,
		PaperInfoMap PaperInfo
	);
	~CWaterMarkDialog();

	HRESULT ActivateUIControl(_In_ CONST HWND hDlg);
	HRESULT SendCommand(_In_ CONST HWND   hDlg, _In_ CONST WPARAM wParam);
	virtual LRESULT OnCbnSelchange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnBnClicked(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnEnChange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmVScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnBtnOK(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnBtnCancel(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnWaterMarkScrollBar(INT wCtrlID, HWND hWnd, WPARAM wParam, LPARAM lParam);
	void CreateControlClass(void);
	void SetTabState(
		_In_ CONST HWND   hDlg,
		_In_ CONST INT iCtrlID
	);
	HRESULT SetTabDefaults(_In_ CONST INT   iCtrlID, _In_ CONST HWND   hDlg);
	
	void GetWaterMarkRegistryObject(void);
	void GetPaperSize(UINT uFeatureIndex);
	void ProcessFeatureInfoList();
	void GetOrientation(UINT uFeatureIndex);
	void SaveFeatureOptions();
	
protected:		

	virtual LRESULT OnWmInitdialog(void);
	PRESOURCEIDINFO GetResourceIDInfoOfControl(std::string strFeatureName);
	DWORD GetLanguageID(HANDLE hPrinter);
	BOOL InitializeScrollBar(void);

	void SetPos(const HWND hDlg, const INT iCtrlID);

	void SetPosString(const HWND hDlg, const INT iCtrlID);
	
	ResourceIDInfoMap*					m_pResourceIDInfoMap;
	PFEATUREINFOLIST					m_pFeatureInfoList;
	std::vector<DEFAULTDEVMODEINFO>		m_DefaultSettings;
	HANDLE							    m_hPrinterHandle;
	PDEVMODE							m_pOemPublicDevMode;
	POEMDEV								m_pTempOemPrivateDevMode;
	HWND								m_hPropertySheetHandle;
	WCHAR								m_PszPrnName[INI_STR_MAX];
	PaperInfoMap						m_PaperInfo;
	std::map<INT, std::vector<INT>>     m_DependentContolMap;
	
	//Flag to check if Watermark text edit box is empty.
	BOOL m_bIsWaterMarkTextEmpty;
private:
	BOOL SetTitle(void);
	void SetControlStrings(void);
	

	LONG QuerySaveWmChanges(HWND hDlg);
	void InitializeWaterMarkFileData(void);
	LRESULT OnComboboxSelectionChange(HWND hDlg);
	BOOL WatermarkDataToPropState(LPWATERMARKDATA pWaterMarkData);
	BOOL PropStateToWatermarkData(LPWATERMARKDATA pWaterMarkData);
	void OnWaterMarkCenter(void);
	void SetButtonState(HWND hDlg);
	

	void CreateDepenedentContolMap(void);
	void CreateMapElement(INT iMapKey, INT iMapValue);
	void CreateMapElement(INT iMapKey, INT* iDependentControlIDs, DWORD dwCount);
	BOOL OnWaterMarkAdd(HWND hDlg);
	BOOL OnWaterMarkEdit(HWND hDlg);
	BOOL OnWaterMarkDelete(HWND hDlg);
	BOOL IsAddWaterMark();
	BOOL AddWaterMark(SHORT sWaterMarkIndex, WCHAR* szTitle);

	PROPSTATE							m_PropertySheetState;
	WATERMARKDATA						m_WaterMarkData;
	WATERMARKDATA						m_WaterMarkDataChanged;
	CWaterMarkRegistryData*				m_pWatermarkregdata;
	CWaterMarkFileData*					m_pWaterMarkFileData;
	CShIniFile*							m_pIniFile;
	COLORREF*							m_pCustomColor;
	DWORD								m_dwSelectedPaperWidthPixel;
	DWORD								m_dwSelectedPaperHeightPixel;
	DWORD								m_dwLangID;
	BOOL								m_bWaterMarkChanged;
	SHORT								m_sOrientation;
	BOOL								m_bIsWaterMarkDefaultClicked;
	BOOL								m_blForceUpdate;
	CShJsonWm*							m_pjsonwm;
	BOOL								m_checkjson;
};

