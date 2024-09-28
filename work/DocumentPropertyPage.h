#pragma once
typedef std::map<INT_PTR, CUIControl*> UIControlMap;
typedef std::map<INT, std::vector<INT>> DependentContolIDMap;
#define ID_XDSMPL_DLG_ATOM 1000

class CDocumentPropertyPage
{
public:
    CDocumentPropertyPage(HMODULE hStringResourceHandle,LPTSTR pHelpFilePath,DWORD *pHelpID,HANDLE hPrinter,WCHAR* pPrinterName,HMODULE hBitmapResourceHandle,HMODULE hDialogResourceHandle);
	virtual ~CDocumentPropertyPage(void);
	 //
    // Implementation
    //
	virtual HRESULT
    SetDevMode(
        _In_ CONST POEMDEV pOemDevMode
        );
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
    VOID SetDeviceType(eDeviceType eValue);
//<S><A> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam
	virtual BOOL OnShowHideTab(HWND hWnd, BOOL bShow);
//<S><E> Bug 2169 - Issue1 - 20200629 - SSDI:Seetharam

	//void ShowPaperSizeDimensions(HWND hDlg, INT iCtrlID);
// <S><A> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	void DevDmodetopropstate();
	SHORT UpdatePunchTypeToPropstate();
	BOOL ModeLimit(HWND hDlg, INT iCtrlID, UINT FeatureIndex);
	//<S><A>To Fix Redmine #7424,31-05-2024,SSDI:Manoj S
	BOOL limStapleVsBooklet(HWND hDlg, INT iCtrlID, UINT FeatureIndex);
	//<E>To Fix Redmine #7424,31-05-2024,SSDI:Manoj S
	long GetValue(HWND hDlg, WORD wCtrlID);
	long SetCheckBoxValue(HWND hDlg, WORD wCtrlID, UINT wCheck);
	VOID InitializeFeatureOption(HWND hDlg, WORD wCtrlID, LONG lVal);
	VOID SetJobControlEdit(HWND hDlg, WORD wCtrlID, LPWSTR newVal, BOOL blForceMB = FALSE);
	VOID SetEditLName(HWND hDlg, WORD wCtrlID, LPWSTR newVal, BOOL blForceMB = FALSE);
	BOOL SetRegJCInfoToPropState(PSCDM pscdm, PPROPSTATE pps, WCHAR * pszSvrPrnName);
	BOOL RegJCInfoToPropState(PREGJCINFO prji, PPROPSTATE pps, WCHAR * pszSvrPrnName);
	BOOL PropStateToRegJCInfo(PPROPSTATE pps, PREGJCINFO prji, WCHAR * pszSvrPrnName);
	SHORT GetRetentionPropValue();
	SHORT GetDocFillingPropValue();
	long limTabCtrlPosition(HWND hDlg,PFEATUREINFOLIST m_pFeatureInfoList);
	BOOL limDuplexVsBinEdge(HWND hDlg,short wBindedge);
	BOOL DisplayPopUpInformationMessage(HWND hDlg, DWORD wControlID);
	VOID ShowStapleVsOriginalSizeVsOutputSizeInformationIcon(HWND hDlg, INT iCtrlID = 0);
	VOID ShowPunchVsOriginalSizeVsOutputSizeInformationIcon(HWND hDlg, INT iCtrlID = 0);
	VOID ShowStapleVsMarginShiftInformationIcon(HWND hDlg, INT iCtrlID = 0);
	VOID ShowStapleVsPaperTypeInformationIcon(HWND hDlg, INT iCtrlID = 0);
// <E> Implementing UI constraint between staplemodulevsStapletype, punchmodule and colormodule,2020.09.29, SSDI:Chandrashekar .V
	BOOL OnUsSelect(HWND hDlg, WCHAR* pPrinterName,DWORD wControlID);

	virtual BOOL SetMarginShiftDataToPropState(POEMDEV pPrivateDevmode, PPROPSTATE pPropertySheetState, WCHAR * pPrinterName);
	
	virtual BOOL SetPPosDataToPropState(POEMDEV pPrivateDevmode, PPROPSTATE pPropertySheetState, WCHAR * pPrinterName);

	void SetUSUntitled(POEMDEV pOemPrivateDevMode_pre, POEMDEV pOemPrivateDevMode_Current,PDEVMODE pOemPublicDevMode_Pre,PDEVMODE pOemPublicDevMode_Current, PFEATUREINFOLIST pFeatureInfoList_pre, PFEATUREINFOLIST pFeatureInfoList_current, short wUsIndexz, short wUsIndex_Current, BOOL blDefault = FALSE, BOOL blAfterLim = FALSE);

	BOOL SyncIgnoreData(POEMDEV pOemPrivateDevMode_pre, POEMDEV pOemPrivateDevMode_Current);

	PFEATUREINFOLIST UpdateFeatureInfoList(PFEATUREINFOLIST pFeatureInfoListTemp, PFEATUREINFOLIST pFeatureInfoListCurrent);

	PFEATUREINFOLIST InitFeatureInfoList(PFEATUREINFOLIST pFeatureInfoListTemp);

	BOOL CompareFeatureInfoList(PFEATUREINFOLIST pFeatureInfoListTemp, PFEATUREINFOLIST pFeatureInfoListCurrent);

	BOOL OnUsSave(HWND hDlg);

	BOOL OnUsDelete(HWND hDlg);

	short SearchUSData(WCHAR * pszTitle);

	BOOL CompareCurrentSettingVsConfigTabData(PSCDM pscdm1, PSCDM pscdm2, PFEATUREINFOLIST pFeatureInfoList, PFEATUREINFOLIST pFeatureInfoListCurrent,SHORT wIndex = FALSE);

	BOOL Update_GPDFeatureOptionList(/*PFEATUREINFOLIST pFeatureInfoList, PGPDFEATUREOPTIONLIST pFeatureList*/);

protected:
//	HRESULT StorePropertyPageInstance(_In_ CONST HWND hDlg);
	//static HRESULT RetrievePropertyPageInstance( _In_ CONST HWND  hDlg, _Outptr_ CDocumentPropertyPage** pDocPropPage);
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

	//<S><M> Moved the declarations to public, 20200327 - SSDI:Seetharam
    //HRESULT
    //GetComPropSheetFunc(
    //    _Outptr_ PFNCOMPROPSHEET* ppfnComPropSheet
    //    );

    //HANDLE
    //GetPageHandle(
    //    VOID
    //    );

    //HANDLE
    //GetComPropSheetHandle(
    //    VOID
    //    );
	//<S><E> Moved the declarations to public, 20200327 - SSDI:Seetharam
	
	HRESULT
    AddUIControl(
        _In_ CONST INT iCtrlID,
        _In_ CUIControl* pUIControl
        );
	
	HRESULT
    DestroyUIComponents(
        VOID
        );
	virtual HRESULT
    InitDlgBox(
        _Out_ LPCTSTR* ppszTemplate,
        _Out_ LPCTSTR* ppszTitle
        )/* = 0*/;
	virtual HRESULT InitializeUIControls(_In_ CONST HWND   hDlg );
	//<S><M> Moved the declarations to public, 20200327 - SSDI:Seetharam
	//virtual VOID SaveFeatureOptions(VOID);
	//<S><E> Moved the declarations to public, 20200327 - SSDI:Seetharam
	virtual HRESULT
	ActivateUIControl(
    _In_ CONST HWND   hDlg
    );
	virtual HRESULT
	DeActivateUIControl(
    _In_ CONST HWND   hDlg
    );
	virtual HRESULT
	OnQueryCancel(
    _In_ CONST HWND   hDlg
    );
	virtual  HRESULT
		 OnScroll(
        _In_ CONST HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        );
	virtual LRESULT 
		OnContorlColorStatic(
		_In_ CONST HWND hWnd, 
		_In_ CONST WPARAM wParam, 
		_In_ CONST LPARAM lParam
		) ;
	virtual  HRESULT
		 OnDrawItem(
        _In_ CONST HWND   /*hDlg*/,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM /*lParam*/
        );
	virtual  HRESULT
		 OnMouseMove(
        _In_ CONST HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        );
	virtual HRESULT OnEnSetFocus(HWND hDlg, const WPARAM wParam);
	virtual LRESULT OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnLButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual HRESULT
    SendCommand(
        _In_ CONST HWND   hDlg,
        _In_ CONST WPARAM wParam
        );
	virtual void SetControlStrings(_In_ CONST HWND   hDlg)=0;
	virtual void CreateContorlClass(void)=0;
	VOID SetUserSettingState(const HWND hDlg, const INT iCtrlID = 0);
//<S><A>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
	void UpdatePropstateToStructure();
	void UpdatePropState(POEMDEV pPrivateDevmode, PPROPSTATE pPropertySheetState);
//<E>Updating PROPSTATE variable for Reference point(Zoom Basing), 2020.09.02, SSDI:Sneha TG
	UIControlMap                 m_UIControls;
	IPrintCoreHelperUni* m_pPrintCoreHelperUni;
	ResourceIDInfoMap* m_pResourceIDInfoMap;
	ConstraintFeatureInfoMap* m_ConstraintFeatureInfoMap;
	PaperInfoMap m_PaperInfo;
	CustomPaperInfoMap m_CustomPaperInfoList;
	PFEATUREINFOLIST m_pFeatureInfoList;
	PFEATUREINFOLIST pFeatureInfoList;	//<S><A>Fix for Coverity issue 92766. Harika,20230427
	HMODULE	m_hStringResourceHandle;
	HMODULE	m_hBitmapResourceHandle;
	HMODULE	m_hDialogResourceHandle;
	TCHAR m_szItem[MAX_PATH];
	BOOL			m_blIgnoreUntitled ;
	POEMDEV m_pOemPrivateDevMode;
	PDEVMODE m_pOemPublicDevMode;
    POEMDEV m_pPrivateDevmode;
    HMODULE m_hDiscoveryStringResourceHandle;
    eDeviceType m_eDeviceType;
    eColorModeIndex m_eColorModeValue;
	CInformationIconButton* m_pInformationIconButton;
	INFORMATIONICONSTATE m_InformationIconState;
	HCURSOR					m_hcsHandUp;
	HCURSOR					m_hcsHandDown;
	
protected:

	int m_Width;
	int m_Length;
	int	  m_Unit;

	DWORD m_wID;
	HWND m_hPropertySheetHandle;
	LPTSTR	m_pHelpFilePath;
	DWORD	*m_pHelpID;
	CShowMessage* m_ObjCShowMessage;
	DWORD GetContextHelpID(DWORD lCtrlID, DWORD *pHelpID);
	HRESULT  ShowHelp(LPHELPINFO pHelpInformation); 
	PRESOURCEIDINFO GetResourceIDInfoOfControl(std::string strFeatureName);
	void  NotifyConstrainedFeatures(_In_ CONST HWND   hDlg,CUIControl* pControl,UINT   iCtrlID,BOOL bTabDefaults = FALSE);
	void  ProcessDepdendentFeatureConstraints(_In_ CONST HWND   hDlg,UINT   uDependentFeatureControlID,BOOL bTabDefaults = FALSE);
	//<S><C> to fix Coverity issue 90564, Harika 20201214
	VOID AddOrRemoveConstraints(std::map<std::string,UINT>&ConstraintFeatureNameID,const CONSTRAINTFEATUREOPTIONINFO &constraintfeatureoptioninfo,INT iAddOrRemove,std::pair<UINT,UINT>FeatureOptionIdPair = std::make_pair(0,0));
	//<E> to fix Coverity issue 90564, Harika 20201214
	VOID ProcessDependentConstraints(std::map<std::string,UINT>&ConstraintFeatureNameID, CONSTRAINTFEATUREOPTIONINFO& constraintfeatureoptioninfo,INT iAddOrRemove);
	VOID ChangeComboBoxSelectedItem(_In_ CONST HWND   hDlg,UINT&   iCtrlID,BOOL bTabDefaults = FALSE);
	BOOL UndoControlSelection(CONST HWND hDlg,INT   iCtrlID);
	VOID UpdateMimic(VOID);
	VOID UpdateUserSetting(HWND hDlg, WORD wControlID);
	
	//VOID ChangeComboBoxSelectedItem(_In_ CONST HWND   hDlg,UINT   iCtrlID,BOOL bTabDefaults = FALSE);
	VOID SetWaterMarkIndex(SHORT sWaterMarkIndex);
	BOOL HideCtrl(HWND hDlg,short wTargetID);
	void SetControlState(HWND hDlg,short wCtrlID, BYTE cStatus);
	OptionNameResourceIDPair GetOptionNameResourceIDPair(std::string strFeatureName,std::string strOptionName);
    std::vector<DEFAULTDEVMODEINFO> m_DefaultSettings;
	DependentContolIDMap m_DependentContolMap;
	PROPSTATE m_PropertySheetState;
	CTRLSTATE m_ControlState;
	WATERMARKDATA m_WaterMarkData;
	WATERMARKDATA m_WaterMarkDataChanged;
    WCHAR m_pPrinterName[MAX_PATH];
    HANDLE m_hPrinterHandle;
	SHORT* m_pWaterMarkIndex;
	SHORT* m_pUsIndex;
	PPROPINFO m_ppi;
	BOOL				m_blEMFMode;
	BOOL				m_blLocalPrn;
	BOOL				m_blNetworkPrn;
	void CreateMapElement(INT iMapKey,INT iMapValue);
    void CreateMapElement(INT iMapKey,INT* iDependentControlIDs,DWORD dwCount);
	virtual HRESULT SetTabDefaults(_In_ CONST HWND   hDlg) ;
	VOID CreateMessageClass(_In_ CONST HWND   hDlg);
	VOID CreatePaperSizeToTiledPamphletPaperSizeMap();
	BOOL SetTiledPamphletPaperSizeIndex(_In_ CONST HWND   hDlg,INT& iTilesPamphletPaperSizeIndex);
	BOOL DisplayStapleInformationMessage(HWND hDlg);
	BOOL DisplayPaperTypeInformationMessage(HWND hDlg, WORD iCtrlID = 0);
	BOOL DisplayOutputandPaperSizeInformationMessage(HWND hDlg, WORD iCtrlID = 0);
	BOOL DisplayPaperTrayVsOriginalSizeDialog(HWND hDlg, WORD iCtrlID = 0);
	BOOL DisplayPaperTrayVsOutputSizeDialog(HWND hDlg, WORD iCtrlID);
	//<S><A>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	//Add the ristrict between Stapleless of Staple and Auto Select of Paper Tray
	BOOL DisplayPaperTrayVsStapleInformationMessage(HWND hhDlg);
	BOOL ReadStapleLessFlag(BYTE& staplelessvalue);
	//<E>To Fix Redmine #3135,12-04-2024,SSDI:Manoj S
	BOOL DisplayOrientationInformationMessage(HWND hDlg);
	BOOL DisplayNUPInformationMessage(HWND hDlg);
	BOOL DisplayDuplexInformationMessage(HWND hDlg);
	HRESULT  DisplayCustomDialog(HWND hDlg, WORD iCtrlID);
	int ShowCustomPaperDialog(int iCtrlID, HWND   hDlg);
	BOOL DisableAllCtrls(short *pwExcept = NULL);
	void SelectOptionsonUI(HWND   hDlg, POEMDEV pOemDevMode);
	BOOL UndoPaperSelection(const HWND hDlg, INT uPaperSizeIndex, INT uOutputIndex);
	DWORD OnPaperTypeChanged(HWND hWnd, WORD iCtrlID= 0);
	//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
	BOOL DealWithFavItemsInHKLM();
	BOOL DealWithFavItemsInHKLMForJson();
	//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
private:
	void  DisplayHelpInformation( _In_ CONST HWND hDlg);
	HRESULT StorePropertyPageInstance(_In_ CONST HWND hDlg);
	static HRESULT OnWmInitDialog(_In_ HWND   hDlg,
        _In_ LPARAM lParam
        );
	static HRESULT OnWmCommand(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam
        );
	static HRESULT OnWmNotify(_In_ HWND   hDlg,
        _In_ LPARAM lParam
        );
	static HRESULT OnWmScroll(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        );
	static HRESULT OnWmDrawItem(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        );
	static HRESULT OnWmColorStatic(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        );
	static HRESULT OnWmMouseMove(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        );
	static HRESULT OnWmLButtonDown(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        );
	static HRESULT OnWmLButtonUp(_In_ HWND   hDlg,
        _In_ CONST WPARAM wParam,
		_In_ CONST LPARAM lParam
        );
	VOID ResetInvalidSelectedOption(_In_ HWND   hDlg,
									ConstraintFeatureInfoMap::iterator iIteratorToConstraintFeatureInfoMap
									);
	VOID SetFeatureOption(_In_ HWND   hDlg,
							ConstraintFeatureInfoMap::iterator iIteratorToConstraintFeatureInfoMap
							);
	BOOL IsAllowSetFeatureOption(VOID);
	VOID SetFeatureControlID(INT iCtrlID);
	static HRESULT OnPsnSetActive(_In_ HWND   hDlg );
	//<S><M> Moved the declarations to public, 20200327 - SSDI:Seetharam
	//static HRESULT OnPsnApply(_In_ HWND   hDlg);
	//static HRESULT OnPsnKillActive(_In_ HWND   hDlg );
	//<S><E> Moved the declarations to public, 20200327 - SSDI:Seetharam
	static HRESULT OnPsnQueryCancel(_In_ HWND   hDlg );
	static HRESULT OnWmDestroy(_In_ HWND   hDlg);
	static HRESULT RetrievePropertyPageInstance( _In_ CONST HWND  hDlg, _Outptr_ CDocumentPropertyPage** pDocPropPage);
    static HRESULT OnNmClick(_In_ HWND   hDlg );
	HANDLE                       m_hPage;
	HANDLE                       m_hComPropSheet;
    PFNCOMPROPSHEET              m_pfnComPropSheet;
	UINT m_uFeatureControlID;
	std::map<std::string,std::string> m_PaperSizeToTiledPamphletPaperSizeMap;

//<S><M> Moved the declarations to public, 20200327 - SSDI:Seetharam
public:
	long GetResourceID();
	virtual VOID SaveFeatureOptions(VOID);
	static HRESULT OnPsnApply(_In_ HWND   hDlg);
	LRESULT OnPsnApplyEX(HWND hWnd, long lCtrlID, LPPSHNOTIFY ppsn);
	LRESULT OnPsnApplyEX(HWND hDlg, long lCtrlID);
	static HRESULT OnPsnKillActive(_In_ HWND   hDlg);

	LRESULT OnPsnKillActiveEX(HWND hWnd);
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
	//<S><E> Moved the declarations to public, 20200327 - SSDI:Seetharam
};

