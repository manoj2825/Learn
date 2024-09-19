//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : Combo.h
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 09 October 2013
//
//	Description   : Header file for combo box control
//===========================================================================
#pragma once
class CWaterMarkCombo:public CShImageCombo
{
public:
	CWaterMarkCombo(
                        CWaterMarkFileData* pWaterMarkFileData,
						CWaterMarkRegistryData* pWaterMarkRegistryData,
						SHORT& sWaterMarkIndex,
						HMODULE hStringResourceHandle,
						WCHAR* pPrinterName, WCHAR* pWaterMarkString, BOOL& bWaterMarkChanged
				   );

	CWaterMarkCombo(
		CWaterMarkFileData* pWaterMarkFileData,
		CWaterMarkRegistryData* pWaterMarkRegistryData,
		CShJsonWm*	pjsonwm,
		BOOL		FlagJson,
		SHORT& sWaterMarkIndex,
		HMODULE hStringResourceHandle,
		WCHAR* pPrinterName, WCHAR* pWaterMarkString, BOOL& bWaterMarkChanged
	);
	
	virtual ~CWaterMarkCombo(void);
	HRESULT
    OnInit(_In_ CONST INT   iCtrlID,
        _In_ CONST HWND hDlg
        );
	  HRESULT
    ActivateUIControl(_In_ CONST INT   iCtrlID,
        _In_ CONST HWND hDlg
        );
	  HRESULT
    OnCommand(_In_ CONST INT   iCtrlID,
        _In_ CONST HWND hDlg,
        _In_ INT        iCommand
        );
	  virtual HRESULT
     OnSelChange(_In_ CONST INT   iCtrlID,
        _In_ CONST HWND hDlg
        );
      HRESULT SetTabDefaults(_In_ CONST INT   iCtrlID,_In_ CONST HWND hDlg);
private:
	SHORT* m_pWaterMarkIndex;
    WCHAR* m_pWaterMarkString;
    BOOL*  m_pbWaterMarkChanged;
	CShJsonWm*	m_pJsonwm;
	BOOL	m_jsonFlag;

};

