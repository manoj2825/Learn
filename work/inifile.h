// ============================================================================
// inifile.h
//
//     Copyright 2004 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2004.06.02
// ============================================================================
#pragma once
#include "RegistryAPI.h"
#include "define.h"

// ================================================
// _/_/_/  define
// ================================================
#define INI_STR_MAX							512
#define INI_SEC_MAX							64
//
//#define INI_PTR_SECTION
////#undef INI_PTR_SECTION
//
//// ================================================
//// _/_/_/  struct
//// ================================================
//#ifdef INI_PTR_SECTION
//
//typedef struct _SectionData
//{
//	WCHAR FAR		*pszSection;
//	WCHAR FAR		*pDataAddr;
//	DWORD			dwSize;
//
//	void FAR		*pnext;
//}SISECDATA, *PSISECDATA;
//
//#else
//
//typedef struct _SectionData
//{
//	WCHAR			szSection[INI_SEC_MAX];
//	WCHAR FAR		*pDataAddr;
//	DWORD			dwSize;
//
//	void FAR		*pnext;
//}SISECDATA, *PSISECDATA;
//
//#endif

// ================================================
// _/_/_/  class
// ================================================
class CShIniFile 
{
public:
	CShIniFile(HINSTANCE hInst, WCHAR* pPrinterName, WCHAR *pIniFilePath = NULL, BOOL blDirect = FALSE, long lMaxData = SCUI_LONGDATA_MAX);
	virtual ~CShIniFile();

	virtual BOOL Initialize(WCHAR *pIniFilePath = NULL);

	DWORD GetString(WCHAR *pszSec, WCHAR *pszKey, WCHAR *pszDef, WCHAR *pszRet, DWORD dwSize);
	DWORD GetString(WCHAR *pszSec, long lKey, WCHAR *pszDef, WCHAR *pszRet, DWORD dwSize);
	long GetLong(WCHAR *pszSec, WCHAR *pszKey, long lDef = 0);
	long GetLong(WCHAR *pszSec, long lKey, long lDef);

	long GetLongArray(WCHAR *pszSec, WCHAR *pszKey, long *plValue = NULL, long lCount = 0);
	long GetLongArray(WCHAR *pszSec, long lKey, long *plValue = NULL, long lCount = 0);

	long GetArrayValue(WCHAR *pszSection, WCHAR *pszKey, short wID, short wArray = 2, short wDataPos = 1, short wIDPos = 0, long lDef = -1);
	long GetArrayValue(WCHAR *pszSection, long lKey, short wID, short wArray = 2, short wDataPos = 1, short wIDPos = 0, long lDef = -1);

	long *GetLongArrayData(void);
	WCHAR *GetDataAddr(void);

// <S><A> #437 in Redmine,2012.07.30, SSDI:Akshay
// As per discussion with SDC in Redmine ticket #437, below source code is merged from CR3 production driver.
// <S><A> 2011.08.26 hashida
	WCHAR* GetTimeStamp(void);
// <E> 2011.08.26 hashida
// <E> #437 in RedMine, 2012.07.30, SSDI:Akshay

    DWORD GetDefaultLangID();
	BOOL IsUseACM(void);
	BOOL IsACDir(void);
	BOOL IsACSingleSignOn(void);
	BOOL IsExistValue(WCHAR FAR *pszSec, WCHAR FAR *pszKey, long lValue);
	BOOL IsExistValue(WCHAR FAR *pszSec, long lKey, long lValue);

protected:

	BOOL		m_blDirect;
	
	WCHAR FAR	*m_pszDefFName;
	WCHAR FAR	*m_pszDefExt;
	WCHAR FAR	*m_pszIniFPath;
	WCHAR FAR	*m_pszSection;
	WCHAR m_pszDrvName[MAX_PATH];

	WCHAR FAR	*m_pdata;
	PSISECDATA	m_psecdata;

	long		m_lCount;
	long FAR	*m_plData;

// <S><C> #437 in Redmine,2012.07.30, SSDI:Akshay
// This member variable is not used after initialization. Hence commented the below piece of code.
  //DWORD		m_dwFileSize;
// As per discussion with SDC in Redmine ticket #437, below source code is merged from CR3 production driver.
// <S><A> 2011.08.26 hashida
	WCHAR FAR	*m_szDateTime; 
// <E> 2011.08.26 hashida
// <E> #437 in RedMine, 2012.07.30, SSDI:Akshay
	PSISECDATA GetSectionDataAddr(WCHAR *pszSec, WCHAR *pData);
	WCHAR *GetNextSectionAddr(WCHAR *pData);

	DWORD GetKeyData(WCHAR *pszKey, PSISECDATA pSecData, WCHAR *pszRet, DWORD dwSize);

	PSISECDATA CreateSectionData(WCHAR *pszSec, WCHAR *pDataAddr);
	PSISECDATA GetLastSectionDataAddr(void);

	long InitLongArray(void);

	void SetLastSectionData(PSISECDATA psd);
	void DeleteSectionData(void);
	char		m_byWriteToJson;

public:
	LCID GetLCID(BOOL blFlag = FALSE);
	long GetLongDPLCID(WCHAR FAR *pszKey, long lDef = 0);
	long GetLongDPLCID(long lKey, long lDef = 0);
	short GetDefaultLang(void);
	short GetInstallationType(void);
	BOOL IsACPrintRelease(void);
	long IsDiscovery();
	BOOL IsACDebug(void);
	//<S><A>To Implement Task#3118,22-04-2024,SSDI:Manoj S
	//Change the specification for function limitation, display method by changing option
	BOOL IsLimitByOptionHideUI();
	//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S
	BOOL IsWriteToJson();
};

//#endif	// #ifndef _INIFILE_H
