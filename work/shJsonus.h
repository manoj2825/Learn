// ============================================================================
// shJsonus.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.2.25
// ============================================================================

#pragma once

#include "json/json.h"
#include "shJson.h"
#include "shregus.h"
#include "jsonkey.h"
#include "define.h"
#include "regkey.h"
#ifndef _SHJSONUS_H
#define _SHJSONUS_H

// Make sure this include is present if PUSUPDATELIST is used in shJsonus.h
// Forward declaration in shJsonus.h

//#ifndef _SHREGUS_H
//#include "shregus.h"
//#endif
//
//typedef struct _UsUpdateList USUPDATELIST, FAR *PUSUPDATELIST;
//
//typedef struct _UsUpdateList
//{
//	short			wVer;
//	WCHAR 			szEntry[128];
//
//}USUPDATELIST, FAR *PUSUPDATELIST;

// ================================================
// _/_/_/  define
// ================================================

// ================================================
// _/_/_/  class
// ================================================
class CShJsonUS : public CShJson
{
public:

	CShJsonUS(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName /*CShPrnLog FAR *pLog*/);

	CShJsonUS(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName, HMODULE hStringResourceHandle /*CShPrnLog FAR *pLog*/);
	virtual ~CShJsonUS();

	BOOL reset(PSCDM pscdm, BOOL blAllReset = TRUE);
	BOOL reset(PSCDM pscdm, PFEATUREINFOLIST pFeatureInfoList, PGPDFEATUREOPTIONLIST pFeatureList, BOOL blAllReset = TRUE);
	//long WriteData(WCHAR FAR *pszTitle, PSCDM pscdm, long lIndex, BOOL blOverWrite, WCHAR FAR *pszTime = NULL, BOOL bSelf = TRUE);
	long WriteData(WCHAR FAR *pszTitle, PSCDM pscdm, PGPDFEATUREOPTIONLIST pFeatureList, long lIndex, BOOL blOverWrite, WCHAR FAR *pszTime = NULL, BOOL bSelf = TRUE);

	//long WriteData(WCHAR FAR *pszTitle, PGPDFEATUREOPTIONLIST pFeatureList, PSCDM pscdm, long lIndex, BOOL blOverWrite, WCHAR FAR *pszTime = NULL, BOOL bSelf = TRUE);
	//long WriteData(WCHAR FAR *pszTitle, PGPDFEATUREOPTIONLIST pFeatureList, PSCDM pscdm, long lIndex, long lID, WCHAR FAR *pszTime = NULL, BOOL bSelf = TRUE);
	long ReadData(PSCDM pscdm, long lIndex);

	virtual BOOL WriteCount(DWORD dwCount);
	virtual DWORD ReadCount();
	virtual void DeleteData(short wIndex);
	virtual BOOL GetTitle(WCHAR FAR *pszTitle, DWORD dwBufSize, long lIndex);
	virtual BOOL GetSelfFlag(long lIndex);
	virtual BOOL GetTime(WCHAR *pszTime, DWORD dwBufSize, long lIndex);

	BOOL IsLocaleChanged();
	BOOL IsVersionChanged();

	BOOL update(PSCDM pscdm);
	BOOL IsEnableUpdate();

	long ReadDataJson(long lIndex);

	long ReadDateTime(WCHAR FAR *pszDateTime, DWORD dwBufSize);
	long WriteDateTime(WCHAR FAR *pszDateTime, DWORD dwBufSize);

	BOOL ReadLCID(DWORD *dwID);
	BOOL WriteLCID(DWORD dwID);

	BOOL WriteChapterInsPage(long lIndex, WCHAR FAR *pszChpInsPage, LONG size);
	long ReadChapterInsPage(long lIndex, WCHAR FAR *pszChpInsPage, LONG size);

	void DeleteDataLimitByOption(BOOL *pblLimitByOptionTbl);

//<S><A> 2022.04.13 Jelly, for cr5 function2 fav
	virtual BOOL WriteShareDayTimeFunc(WCHAR *pszSvrPrnName,WCHAR* pszDayTime, BOOL bValid);
	virtual BOOL WriteShareDayTimeFuncToJSON(WCHAR *pszSvrPrnName, WCHAR *pszText, DWORD dwSize);
	virtual BOOL ReadShareDayTimeFuncFromJSON(WCHAR *pszSvrPrnName,WCHAR *pszText, DWORD dwTextSize);
	virtual BOOL ReadShareDayTimeFuncFromHKLM(WCHAR *pszSvrPrnName,WCHAR *pszText, DWORD dwTextSize);
//<E> 2022.04.13 Jelly

// <S><A> 2022.05.27 sbc R.Tanaka
	// TabShift関係 (Advancedダイアログ)
	BOOL ResetUnit();
	BOOL WriteUnit(short wVal);
	short ReadUnit();

	virtual BOOL ClearUnit();

	BOOL SetUnit(short wVal);
// <E> 2022.05.27 sbc R.Tanaka


public:
	//long WriteData(WCHAR FAR *pszTitle, PSCDM pscdm, long lIndex, long lID, WCHAR FAR *pszTime = NULL, BOOL bSelf = TRUE);
	long WriteData(WCHAR FAR *pszTitle, PGPDFEATUREOPTIONLIST pFeatureList, PSCDM pscdm, long lIndex, long lID, WCHAR FAR *pszTime = NULL, BOOL bSelf = TRUE);
	BOOL WriteVersion(PSCDM pscdm);
	BOOL ReadVersion(short *pwVersion, short *pwSize);
	long ReadID(long lIndex);

	BOOL WriteLocale();
	BOOL ReadLocale(long *plPSize, long *plUnit);
	BOOL WriteID(long lIndex, long lID);

	DWORD SearchUnusedID();
	//long WriteEachData(PSCDM pscdm,  long lID);
	long WriteEachData(PSCDM pscdm, PFEATUREINFOLIST pFeatureInfoList, long lID);
	long ReadEachData(PSCDM pscdm, long lID, PUSUPDATELIST pusul);
	long DeleteEachData(long lID = -1);

	long MakeUpdateList(short wID, PUSUPDATELIST pusul);
	short SearchUpdateList(PUSUPDATELIST pusul, WCHAR FAR *pszEntry);
	BOOL WriteUpdateList(short wID, PUSUPDATELIST pusul);

	long WriteDataJson(long lIndex);

	long WriteDataJsonMS(long lIndex);
	long WriteDataJsonUPS(long lIndex);
	long WriteDataJsonPP(long lIndex);
	//long WriteDataJsonTR(long lIndex);
	//long WriteDataJsonCPM(long lIndex);
	long WriteDataJsonUnit(long lIndex);
	long WriteDataJsonSTRD(long lIndex);

	long ReadDataJsonMS(long lIndex);
	long ReadDataJsonUPS(long lIndex);
	long ReadDataJsonPP(long lIndex);
	//long ReadDataJsonTR(long lIndex);
	//long ReadDataJsonCPM(long lIndex);
	long ReadDataJsonUnit(long lIndex);
	long ReadDataJsonSTRD(long lIndex);


// ユーザー設定プリセット対応
	//BOOL RegisterPresetFromFile(WCHAR FAR *pszSvrPrnName);

//<S><A> 2022.04.13 Jelly, for cr5 function2 fav
	virtual BOOL	WriteShareData(WCHAR *pszSvrPrnName,BYTE* pShareFav, DWORD dwSize);
	virtual DWORD	ReadShareData(WCHAR *pszSvrPrnName, BYTE* pShareFav);
	virtual DWORD	ReadFavDataFromJSON(WCHAR *pszSvrPrnName, BYTE* pShareFavData,long lIndex);
	virtual BOOL	TransferOneFavDataFromHKLMToJSON(WCHAR *pszSvrPrnName, DWORD dwIndex, BYTE* pShareFavData,DWORD dwSize,WCHAR *pszTime, WCHAR * pszTitle,BOOL bOverWrite);


// <S><A> 2022.05.17 sbc R.Tanaka
	virtual void SetFunctionsJsonPath(wchar_t* wzJsonPath);
// <E> 2022.05.17 sbc R.Tanaka

// <S><A> 2022.05.27 sbc R.Tanaka
	short	m_wUnitVal;
	BOOL	m_blUnitWriteFlg;
// <E> 2022.05.27 sbc R.Tanaka

public:
	long WriteShareDataMSToUS(WCHAR *pszSvrPrnName, long lIndex, BYTE* pMSData, DWORD dwSize);
	DWORD ReadUSMSData(WCHAR *pszSvrPrnName, long lIndex, BYTE* pMSData);
// <S><A> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
	long WriteShareDataSTRToUS(WCHAR *pszSvrPrnName, long lIndex, BYTE* pSTRData, DWORD dwSize);
	DWORD ReadUSSTRData(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pSTRData);

	long WriteShareDataStoredToUS(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pSTRJCData, DWORD dwSize);
	DWORD ReadStoredToData(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pSTRJCData);
// <E> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
	DWORD ReadUSUPSData(WCHAR *pszSvrPrnName, long lIndex, BYTE* pUPSData);
	long WriteShareDataUPSToUS(WCHAR *pszSvrPrnName, long lIndex, BYTE* pUPSData, DWORD dwSize);
	
	DWORD ReadUSPPData(WCHAR *pszSvrPrnName, long lIndex, BYTE* pPPData);
	long WriteShareDataPPToUS(WCHAR *pszSvrPrnName, long lIndex, BYTE* pPPData, DWORD dwSize);
	
	//DWORD ReadUSTRData(char *pszSvrPrnName, long lIndex, BYTE* pTRData);
	//long WriteShareDataTRToUS(char *pszSvrPrnName, long lIndex, BYTE* pTRData, DWORD dwSize);
	
	//DWORD ReadUSCPMData(char *pszSvrPrnName, long lIndex, BYTE* pTRData);//Custom pamphlet
	//long WriteShareDataCPMToUS(char *pszSvrPrnName, long lIndex, BYTE* pTRData, DWORD dwSize);
	
	DWORD ReadUSTSData(WCHAR *pszSvrPrnName, long lIndex, BYTE* pTRData);//tab shift
	long WriteShareDataTSToUS(WCHAR *pszSvrPrnName, long lIndex, BYTE* pTRData, DWORD dwSize);
//<E> 2022.04.13 Jelly

	//CShIniFile* m_pmcf;

	private:
		HMODULE			m_hStringResourceHandle;
};

#endif	// #ifndef _SHJSONUS_H
