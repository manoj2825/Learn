// ============================================================================
// shjsonstr.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.04.08
// ============================================================================
#include "ShJson.h"  
#include "ShRegAC.h" 

#ifndef _SHJSONSTR_H
#define _SHJSONSTR_H

//typedef struct STORED_TO {
//	WCHAR			pszFolderName[512];
//	DWORD			dwFolderIndex;
//} STORED_TO_DATA;
//
//typedef struct PRINTUI_STORED_TO {
//	WCHAR			FolderName[512];
//	BOOL			HasFolderPin;
//}STORED_TO_DATA_printui;
// ================================================
// _/_/_/  class
// ================================================
class CShJsonStored : public CShJson, public CRegistryAPI//CShRegAC
{
public:

	CShJsonStored(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName, HMODULE hStringResourceHandle/*, CShPrnLog FAR *pLog*/);
	virtual ~CShJsonStored();

	BOOL reset(CShIniFile FAR *pIniFile = NULL);

	BOOL WriteStrdData(long lIndex, WCHAR FAR *pszName, BOOL blHasPin);
	BOOL WriteStrdCount(DWORD dwCount);
	BOOL WriteStrdPin(WCHAR FAR *pszPin);
	BOOL WriteLastSelectedFolder(WCHAR FAR *pszFolderName, WCHAR FAR *pszFolderPIN);
	long ReadLastSelectedFolder(WCHAR FAR *pszFolderName, DWORD dwLength, WCHAR FAR *pszFolderPIN, DWORD dwLengthPIN);

	BOOL ReadStrdData(long lIndex, WCHAR FAR *pszName, DWORD dwNameSize, BOOL *pblHasPin);
	DWORD ReadStrdCount();
	BOOL ReadStrdPin(WCHAR FAR *pszPin, DWORD dwPinSize);
	BOOL ReadRetPin(WCHAR FAR *pszPin, DWORD dwPinSize);

	BOOL CreateData(short wPort);

	BOOL WriteLastSelectedFolderIndex(DWORD dwIndex);
	DWORD ReadLastSelectedFolderIndex();

	BOOL WriteSTDDataToUs(long lIndexUs, WCHAR FAR *pszFolderName, DWORD dwFolderSize,DWORD dwFoldIndex);
	BOOL ReadSTDDataToUs(long lIndexUs, WCHAR FAR *pszFolderName, DWORD& dwFolderSize,DWORD& dwFoldIndex);
	BOOL WriteSTDData(WCHAR FAR *pszFolderName, DWORD dwFolderSize);
	BOOL ReadSTDData(WCHAR FAR *pszFolderName, DWORD& dwFolderSize);

	//void SetObjects(PSHOHOBJ psho);

protected:

	DWORD GetCountIni(CShIniFile FAR *pIniFile = NULL);
	BOOL GetStrdDefData(long lIndex, WCHAR FAR *pszPath, DWORD dwPathSize, BOOL *pblHas, CShIniFile FAR *pIniFile);

	BOOL GetConfigData(HKEY hKey, short wPort, long lIndex, WCHAR FAR *pszName, DWORD dwNameSize, BOOL *pblHasPin);
// <S><A> 2022.05.17 sbc R.Tanaka
	virtual void SetFunctionsJsonPath(wchar_t* wzJsonPath);
// <E> 2022.05.17 sbc R.Tanaka
	HMODULE			m_hStringResourceHandle;
};

#endif	// #ifndef _SHJSONSTR_H
