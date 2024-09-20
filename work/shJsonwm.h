// ============================================================================
// shJsonwm.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.03.07
// ============================================================================
#pragma once

#ifndef _SHJSONWM_H
#define _SHJSONWM_H

#include "shJson.h"

// ================================================
// _/_/_/  class
// ================================================
class CShJsonWm : public CShJson ,public CRegistryAPI//bear
{
public:
	CShJsonWm(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName, HMODULE	hStringResourceHandle/*, CShPrnLog FAR *pLog*/);
	virtual ~CShJsonWm();

	BOOL reset(WCHAR FAR *pszSvrPrnName, CShIniFile FAR *pIniFile = NULL);

	BOOL WriteWMData(long lIndex, LPWATERMARKDATA pwmd, WCHAR FAR *pszTitle, WCHAR FAR *pszTime = NULL, BOOL bSelf = TRUE);
	BOOL WriteWMCount(DWORD dwCount);
	BOOL WriteColorData(WCHAR *pData);

	BOOL ReadWMData(long lIndex, LPWATERMARKDATA pwmd, WCHAR FAR *pszTitle = NULL, DWORD dwTitleSize = 0);

	virtual DWORD ReadCount();
	BOOL ReadColor(WCHAR *pData, DWORD pdwSize);
	virtual void DeleteData(short wIndex);
	virtual BOOL GetTitle(WCHAR FAR *pszTitle, DWORD dwBufSize, long lIndex);
	virtual BOOL GetTime(WCHAR FAR *pszTime, DWORD dwBufSize, long lIndex);
	virtual BOOL GetSelfFlag(long lIndex);

	DWORD GetCountIni(CShIniFile FAR *pIniFile = NULL);

	virtual BOOL GetWMDefData(long lIndex, LPWATERMARKDATA pwmd = NULL, CShIniFile FAR *pIniFile = NULL);

// >> 2007.03.14 SBC:Kawabata
// for Watermark Jobinfo
#ifdef _WIN32
	DWORD GetJobInfoSetting(WCHAR *pszSvrPrnName, WCHAR *pszEntry);
	BOOL GetJobInfoCustomString(WCHAR *pszSvrPrnName, WCHAR *pStr, DWORD dwStrSize);
	BOOL GetJobInfoWATERMARKDATA(WCHAR *pszSvrPrnName, WATERMARKDATA *pwmd);

	BOOL ResetJobInfoSetting(WCHAR *pszSvrPrnName);

	BOOL SetJobInfoSetting(WCHAR *pszSvrPrnName, WCHAR *pszEntry, DWORD dwData);
	BOOL SetJobInfoCustomString(WCHAR *pszSvrPrnName, WCHAR *pStr);
	BOOL SetJobInfoWATERMARKDATA(WCHAR *pszSvrPrnName, WATERMARKDATA *pwmd);
#endif
// << 2007.03.14 SBC:Kawabata

// <S><A> 2016.04.18 SSL:K.Hamaguchi
	BOOL CalcGrayscale(LPWATERMARKDATA pwmd);
// <E> 2016.04.18 SSL:K.Hamaguchi

//<S><A> 2022.04.19 Jelly, for cr5 function2 fav
	virtual BOOL WriteShareDayTimeFunc(WCHAR *pszSvrPrnName,WCHAR* pszDayTime, BOOL bValid);
	virtual BOOL WriteShareDayTimeFuncToJSON(WCHAR *pszSvrPrnName, WCHAR *pszText, DWORD dwSize);
	virtual BOOL ReadShareDayTimeFuncFromJSON(WCHAR *pszSvrPrnName,WCHAR *pszText, DWORD dwTextSize);
	virtual BOOL ReadShareDayTimeFuncFromHKLM(WCHAR *pszSvrPrnName,WCHAR *pszText, DWORD dwTextSize);
//<E> 2022.04.19 Jelly
	// <S><A> 2024.02.28,To Fix Redmine #6882 SSDI:Manoj S
	BOOL resetEx(WCHAR FAR *pszSvrPrnName, CShIniFile FAR *pIniFile = NULL);
	BOOL resetWM(WCHAR FAR *pszSvrPrnName, CShIniFile FAR *pIniFile = NULL);
	// <E> 2024.02.28,To Fix Redmine #6882 SSDI:Manoj S
	BOOL GetWMDefDataEx(long lIndex, WATERMARKINFO* pWaterMark,CShIniFile FAR *pIniFile = NULL);
protected:

//<S><A> 2022.04.19 Jelly, for cr5 function2 fav
	virtual BOOL	WriteShareData(WCHAR *pszSvrPrnName,BYTE* pShareFav, DWORD dwSize);
	virtual DWORD	ReadShareData(WCHAR *pszSvrPrnName, BYTE* pShareFav);
	virtual DWORD ReadFavDataFromJSON(WCHAR *pszSvrPrnName, BYTE* pShareFavData,long lIndex);
	virtual BOOL TransferOneFavDataFromHKLMToJSON(WCHAR *pszSvrPrnName, DWORD dwIndex, BYTE* pShareFavData,DWORD dwSize,WCHAR *pszTime, WCHAR * pszTitle,BOOL bOverWrite);
//<E> 2022.04.19 Jelly

// <S><A> 2022.05.17 sbc R.Tanaka
	virtual void SetFunctionsJsonPath(wchar_t* wzJsonPath);
// <E> 2022.05.17 sbc R.Tanaka

	HMODULE	m_hStringResourceHandle;
	CShIniFile* m_cshIniFile;
	DWORD m_wLanguageID;

};

#endif	// #ifndef _SHJSONWM_H
