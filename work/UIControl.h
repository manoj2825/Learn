//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : UIControl.h
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 09 October 2013
//
//	Description   : This class Implements  abstract UI control class for 
//   UI controls for check box, combo box, edit box and radio button and push button classes
//===========================================================================

//===========================================================================
//  Reviews 
//
//  Sl#     Review Date     Reviewer(s)               Remarks
//===========================================================================
#pragma once


// <S><C> Added the control type as button for Imge button. SSDI:Seetharam - 20200212
typedef enum ControlType{ eNone = 0,eStaticText = 1, eComboBox = 2, eCheckBox = 3,eRadioButton = 4,eEditBox = 5, eButton = 6 } ECONTROLTYPE;
// <S><E> Added the control type as button for Imge button. SSDI:Seetharam - 20200212

#define CUICONTROL_SETPROP_STRING				L"CUIControl_Class"
#define CUSTOMPAPEREDITLENGTH 5
#define DECIMALLENGTH 2

typedef enum TextLimit{ eWidthLimit = 5, eLengthLimit } ETEXTLIMIT;

typedef struct _ShControlItemInfo
{
	short			wIndex;
	short			wStrID;
	long			lValue;
	char			*pVal;
	DWORD			dwValSize;
	WCHAR			*pStr;
}SHCTRLITEM, FAR *PSHCTRLITEM;

class CUIControl
{
public:
	CUIControl(HMODULE hStringResourceHandle = NULL, HMODULE hBitmapResourceHandle = NULL, HMODULE hDialogResourceHandle = NULL);
	CUIControl(UINT uContorlID,HMODULE hStringResourceHandle = NULL,HANDLE Printer= NULL,HMODULE hDialogResourceHandle = NULL,HMODULE hBitmapResourceHandle = NULL);
	virtual ~CUIControl(void);
	typedef std::map<INT_PTR, CUIControl*> UIControlMap;
	static LRESULT CALLBACK WindowMapProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
//<S><C>To add Zoom Settings  saving part to UD3L  2020.04.30, SSDI:Sneha TG	
	void EnableApplyButton(_In_ CONST HWND hDlg);
	BOOL IsEqualBuf(void FAR *pbuf1, void FAR *pbuf2, DWORD dwSize);
//<E>To add Zoom Settings saving part to UD3L 2020.04.30, SSDI:Sneha TG
	virtual HRESULT
    OnInit(_In_ CONST INT   iCtrlID,
        _In_ CONST HWND hDlg
        );

   
    virtual HRESULT
    OnNotify(
        _In_ CONST HWND hDlg,
        _In_ CONST NMHDR* pNmhdr
        ) ;

	virtual HRESULT
    ActivateUIControl(_In_ CONST INT   iCtrlID,
        _In_ CONST HWND hDlg
        ) ;

	 virtual HRESULT
    OnCommand(_In_ CONST INT   iCtrlID,
        _In_ CONST HWND hDlg,
        _In_ INT        iCommand
        );
	 virtual HRESULT
    OnScroll(
	_In_ CONST INT   iCtrlID,
	_In_ CONST INT   iScrollBarCommand,
    _In_ CONST HWND hDlg,
    _In_ INT iScrollPosition
        );
	 UINT GetPreviousItemId(void);
	 UINT GetCurrentItemId(void);
	 void SetPreviousItemId(HWND hDlg,PRESOURCEIDINFO pResourceIDInfo,PFEATUREOPTIONSLIST pFeatureOptionsList,BOOL bIsRadioButton = FALSE);
	 BOOL DisplayInformationIcon(VOID);
	 BOOL			m_blForceUpdate;
	 void SetCurrentItemId(HWND hDlg,PRESOURCEIDINFO pResourceIDInfo,PFEATUREOPTIONSLIST pFeatureOptionsList,BOOL bIsRadioButton = FALSE);
	 BOOL IsRadioButton(void);
	 ECONTROLTYPE GetControlType(void);
	 virtual UINT GetFeatureControlID();

	 virtual LRESULT WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	 virtual LRESULT WindowProcedure(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	 BOOL Attach(HWND hWnd);
	 BOOL Detach();

     virtual HRESULT SetTabDefaults(_In_ CONST INT   iCtrlID,_In_ CONST HWND hDlg);
	 virtual HRESULT SetTabDefaults(_In_ CONST INT   iCtrlID, _In_ CONST HWND hDlg, UINT Val, BOOL DefaultValueChanged);
	 virtual LRESULT OnWmHScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
	 virtual VOID Draw();
	 void SetPrinterName(WCHAR *pPrinterName);
	 WCHAR * GetPrinterName();
	void CreateDependentControlMap(INT iMapValue);
	void CreateDependentControlMap(INT* iDependentControlIDs,DWORD dwCount);
	 VOID EnableOrDisableDependentControls(
		_In_ CONST HWND   hDlg,
        _In_ CONST INT iCtrlID
        );
	 VOID ChangeRadioButtonCheckState(
		_In_ CONST HWND   hDlg,
        _In_ CONST INT iCtrlID,
		UIControlMap mapOfUIControl
        );
	 virtual BOOL SetFeatureOption(
		_In_ CONST HWND   hDlg,
		_In_ CONST INT iCtrlID,
		std::string strOption);
	 virtual BOOL ResetInvalidFeatureOption(
		_In_ CONST HWND   hDlg,
		_In_ CONST INT iCtrlID,
		std::vector<std::pair<std::string,std::string>>ListOfInvalidFeatureOptions);

// <S><A> To add the control id of the controls, 20200128 - SSDI:Seetharam
	 UINT GetControlID();
// <S><E> To add the control id of the controls, 20200128 - SSDI:Seetharam

    BOOL m_bisCustomPaper;
    //<S><A>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	static int          m_dpi;
	static int          m_dpiPer;
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
protected:

	CWaterMarkRegistryData* m_pRegistryWaterMarkData;
	//CShJsonWm*				m_pJsonwm;
    CWaterMarkFileData* m_pWaterMarkFileData;
	HMODULE			m_hStringResourceHandle;
	HMODULE			m_hBitmapResourceHandle;
    HMODULE         m_hDialogResourceHandle;
	HANDLE			m_hPrinter;	
	UINT			m_uPreviousItemId;
	UINT			m_uCurrentItemId;
	UINT			m_uControlID;
	ECONTROLTYPE	m_eControlType;
	BOOL 			m_bIsRadioButton;
	HWND			m_hWnd;
	HWND			m_hWndParent;

	WNDPROC			m_procOld;
	BOOL			m_blDlg;
	BOOL			m_bShowInformationIcon;
	short			m_wCtrlItem;
	PSHCTRLITEM		m_pci;
    WCHAR*			m_pPrinterName;
	PRESOURCEIDINFO m_pResourceIDInfo;
    POEMDEV			m_pOemPrivateDevMode;
	PDEVMODE		m_pOemPublicDevMode;
	CShowMessage*	m_ObjCShowMessage;
    WCHAR			m_CustomFormName[MAX_FORMNAME_LENGTH + 1];
	virtual LRESULT OnWmInitdialog(void);

	virtual LRESULT OnWmCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmPaint(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmClose(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmQueryEndSession(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmVScroll(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmSetText(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmHelp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmContextMenu(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCtrlColorStatic(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmMouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmLButtonDblClk(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmLButtonUp(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWmSetCursor(HWND hWnd, WPARAM wParam, LPARAM lParam);

	// ================================================
	// _/_/_/  WM_NOTIFY
	// ================================================
	virtual LRESULT OnPsnSetActive(HWND hWnd, long lCtrlID, LPNMHDR pnmh);
	virtual LRESULT OnPsnKillActive(HWND hWnd, long lCtrlID, LPNMHDR pnmh);
	virtual LRESULT OnPsnApply(HWND hWnd, long lCtrlID, LPPSHNOTIFY ppsn);
	virtual LRESULT OnPsnQueryCancel(HWND hWnd, long lCtrlID, LPPSHNOTIFY ppsn);
	virtual LRESULT OnPsnHelp(HWND hWnd, long lCtrlID, LPPSHNOTIFY ppsn);
	virtual LRESULT OnUdnDeltaPos(HWND hWnd, long lCtrlID, LPNMUPDOWN pnud);
	virtual LRESULT OnNmClick(HWND hWnd, long lCtrlID, PNMLINK pnnm);
	// ================================================
	// _/_/_/  WM_COMMAND
	// ================================================
	virtual LRESULT OnBnClicked(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnCbnSelchange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnEnUpdate(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnEnChange(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnEnSetFocus(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);
	virtual LRESULT OnEnKillFocus(HWND hWnd, HWND hWndCtrl, WORD wCtrlID);

	virtual HRESULT	AddUIControl(_In_ CONST INT   iCtrlID,_In_ CUIControl* pUIControl);	
	virtual HRESULT InitializeUIControls( _In_ CONST HWND   hDlg );
	
	virtual LRESULT OnIDOK(HWND hWnd, HWND hWndCtrl);
	virtual LRESULT OnIDCancel(HWND hWnd, HWND hWndCtrl);

	BOOL CreateShCtrlItem(short wCount);
	void DeleteShCtrlItem(void);

//<S><C>To add Zoom Settings saving to UD3L  2020.04.30, SSDI:Sneha TG
	//void EnableApplyButton(_In_ CONST HWND hDlg);
	//BOOL IsEqualBuf(void FAR *pbuf1, void FAR *pbuf2, DWORD dwSize);
//<E>To add Zoom Settings saving to UD3L 2020.04.30, SSDI:Sneha TG
	VOID DestroyUIComponents(VOID);
	void  GetCustomPaperValue(WCHAR* szCustomPaperValue,BYTE byCustomPaperUnit,DWORD dwCustomPaperSizeInPixel,DWORD dwCustomPaperSizeInMM,SHORT dwTextLimit);
	void DisplayCustomPaperValues(_In_ CONST HWND hWnd, _In_ CONST INT iCtrlID,BYTE byCustomPaperUnit,DWORD dwInchValue,SHORT dwMilliValue);
	void KeepWindowHandle(HWND hWnd);
	BOOL DisplayMessageInformation(HWND hWnd,std::vector<CONSTRAINTFEATUREOPTIONMESSAGEIDSINFO>ConstraintFeatureOptionMessageIDsInfo,UINT uInformationMessageID,BOOL bIsInformationIconClicked = FALSE,INT iCtrlID = 0);
	VOID ShowInformationIcon(HWND hWnd,OptionResourceID::const_iterator iOptionResourceIDIteraotor,UINT uFeatureConstraintIconID = 0,UINT uInformationButtonID = 0);
	void SetControlState(HWND hDlg,INT wCtrlID, BYTE cStatus);
// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	void DecrementItemCount(void);
// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	UIControlMap                 m_UIControls;
	typedef std::vector<INT> DependentContolIDList;
	std::list<UINT> m_InformationMessageIDList;
	DependentContolIDList m_DependentContolIDList;
	
//	UIControlMap                 m_UIControls;
	

//<S><A>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	int GetScreenResolution(void);
//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S    

private:

	HRESULT ShowHelp(LPHELPINFO pHelpInformation);
	VOID UpdateRadioButtonState(
		_In_ CONST HWND   hDlg,
        _In_ CONST INT iCtrlID,
		UIControlMap mapOfUIControl
        );
};	 


