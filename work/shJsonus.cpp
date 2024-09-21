// ============================================================================
// shJsonus.cpp
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.2.25
// ============================================================================

#pragma once
//#include <prnui.h>
#include <precomp.h>
#include "shJsonus.h"
#include <shregus.h>
//#include "shjsonms.h"
//#include "shjsonups.h"
//#include "shjsonpp.h"
//#include "shjsonstr.h"


// SCDMÉoÅ[ÉWÉáÉìïœçXéûÇ…UserSettingÇÃà¯åpÇ¨èàóùÇçsÇ§
#define UPDATE_DIFFVER_US

#define PARENT_CLASSNAME 	CShJson
#define MY_CLASSNAME 		CShJsonUS

//=============================================================================
// function
//      CShJsonUS
//
// parameters
//      hInst               ÉÇÉWÉÖÅ[ÉãÉnÉìÉhÉã
//      pLog                CShPrnLog ÉNÉâÉXÉAÉhÉåÉX
//
// return value
//      ñ≥Çµ
//
// outline
//      CShJsonUS ÉNÉâÉXÉRÉìÉXÉgÉâÉNÉ^
//=============================================================================
CShJsonUS::CShJsonUS(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName/*CShPrnLog FAR *pLog*/)
			: CShJson(hInst, pszSvrPrnName /*pLog*/)
{
// <S><A> 2022.05.27 sbc R.Tanaka
	m_wUnitVal = -1;
	m_blUnitWriteFlg = FALSE;
// <E> 2022.05.27 sbc R.Tanaka

	SetParent(this);
}

CShJsonUS::CShJsonUS(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName, HMODULE hStringResourceHandle/*CShPrnLog FAR *pLog*/)
	: CShJson(hInst, pszSvrPrnName /*pLog*/)
{
	// <S><A> 2022.05.27 sbc R.Tanaka
	m_hStringResourceHandle = hStringResourceHandle;
	m_wUnitVal = -1;
	m_blUnitWriteFlg = FALSE;
	// <E> 2022.05.27 sbc R.Tanaka

	SetParent(this);
}



//=============================================================================
// function
//      ~CShJsonUS
//
// parameters
//      ñ≥Çµ
//
// return value
//      ñ≥Çµ
//
// outline
//      CShJsonUS ÉNÉâÉXÉfÉXÉgÉâÉNÉ^
//=============================================================================
CShJsonUS::~CShJsonUS()
{
}


//=============================================================================
// function
//      reset
//
// parameters
//      pIniFile            CShIniFile ÉNÉâÉXÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      INIÉtÉ@ÉCÉãÇÃÉfÅ[É^ÇégópÇµÇƒÉåÉWÉXÉgÉäÇèâä˙âªÇ∑ÇÈ
//=============================================================================
BOOL CShJsonUS::reset(PSCDM pscdm, BOOL blAllReset)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszTitle = NULL;

	long			lRet = -1;

	if(pscdm == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  èâä˙âªèÓïÒéÊìæÇ∆èëÇ´çûÇ›
	// ================================================
	pszTitle = new WCHAR[(SCUI_TITLE_READ_MAX + 1) * 2];
	if(pszTitle == NULL)
		goto EXIT;

	if(blAllReset == TRUE)
	{
		// --- count
		WriteCount(0);

		DeleteEachData();
	}

	// --- title
	//LoadStr(IDS_INDEX0_USERSETTING, pszTitle, SCUI_TITLE_READ_MAX * 2, TRUE);   //json temp

	// --- data
	//WriteData(pszTitle, pscdm, 0, (long)0);//bear

	// --- data (each)
	//WriteEachData(pscdm, 0);//bear

	lRet = WriteDataJson((long)0);

	// --- version
	WriteVersion(pscdm);

	// --- locale
	WriteLocale();

	//RegisterPresetFromFile(m_pszSvrPrnName);

	blRet = TRUE;

EXIT:

	if(pszTitle != NULL)
		delete[] pszTitle;

	return blRet;
}

BOOL CShJsonUS::reset(PSCDM pscdm, PFEATUREINFOLIST pFeatureInfoList, PGPDFEATUREOPTIONLIST pFeatureList, BOOL blAllReset)
{
	BOOL			blRet = FALSE;

	//WCHAR FAR		*pszTitle = NULL;
	WCHAR			pszTitle[256] = {};
	long			lRet = -1;

	if (pscdm == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  èâä˙âªèÓïÒéÊìæÇ∆èëÇ´çûÇ›
	// ================================================
	//pszTitle = new WCHAR[(SCUI_TITLE_READ_MAX + 1) * 2];

	if (pszTitle == NULL)
		goto EXIT;

	if (blAllReset == TRUE)
	{
		// --- count
		WriteCount(0);

		DeleteEachData();
	}

	// --- title
	//LoadStr(IDS_INDEX0_USERSETTING, pszTitle, SCUI_TITLE_READ_MAX * 2, TRUE);   //json temp
	LoadString(m_hStringResourceHandle, IDS_INDEX0_USERSETTING, pszTitle, countof(pszTitle));

	// --- data
	WriteData(pszTitle, pscdm, pFeatureList, 0, (long)0);
	//WriteData(pscdm,pFeatureInfoList, pFeatureList, 0);

	// --- data (each)
	//WriteEachData(pscdm, 0);
	WriteEachData(pscdm, pFeatureInfoList, 0);

	lRet = WriteDataJson((long)0);

	// --- version
	WriteVersion(pscdm);

	// --- locale
	WriteLocale();

	//RegisterPresetFromFile(m_pszSvrPrnName);

	blRet = TRUE;

EXIT:

	//if (pszTitle != NULL)
	//	delete[] pszTitle;

	return blRet;
}

//=============================================================================
// function
//      WriteData
//
// parameters
//      pszTitle            èëÇ´çûÇﬁÉ^ÉCÉgÉã
//      pscdm               èëÇ´çûÇﬁSCDMç\ë¢ëÃÉAÉhÉåÉX
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//      lID		            ÉÅÉìÉoï ÉfÅ[É^ÇÃÉLÅ[Ç∆Ç»ÇÈID
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::WriteData(WCHAR FAR *pszTitle, PSCDM pscdm, PGPDFEATUREOPTIONLIST pFeatureList, long lIndex, BOOL blOverWrite, WCHAR FAR *pszTime, BOOL bSelf)
{
	long			lRet = -1;

	long			lCount = 0;

	DWORD			dwID;
	PFEATUREINFOLIST pFeatureInfoList = NULL;

	if(pscdm == NULL)
		goto EXIT;

	if(blOverWrite == TRUE)
		dwID = ReadID(lIndex);
	else
		dwID = SearchUnusedID();


	lRet = WriteData(pszTitle, pFeatureList,pscdm, lIndex, (long)dwID, pszTime, bSelf);

	//pFeatureInfoList = new FEATUREINFOLIST();
	//SecureZeroMemory(pFeatureInfoList, sizeof(FEATUREINFOLIST));
	//memset(pFeatureInfoList, 0, sizeof(FEATUREINFOLIST));
	//size_t nNumberOfFeatures = 0;
	//nNumberOfFeatures = GetGPDFeatureCount();

	//pFeatureInfoList->nNummberOfFeatures = nNumberOfFeatures;
	//pFeatureInfoList->pFeatureOptionsList = new FEATUREOPTIONSLIST[nNumberOfFeatures];
	//pFeatureInfoList->pFeatureOptionPair = new PRINT_FEATURE_OPTION[nNumberOfFeatures];

	//memset(pFeatureInfoList->pFeatureOptionsList, 0, nNumberOfFeatures * sizeof(FEATUREOPTIONSLIST));
	//memset(pFeatureInfoList->pFeatureOptionPair, 0, nNumberOfFeatures * sizeof(PRINT_FEATURE_OPTION));
	////GetFeatureOptionFromGPDFeatureOptionList(featureinfo, &pFeatureOptionsList, dwIndex);

	//GetFeatureinfoFromGPDFeatureOptionList(pFeatureInfoList, pFeatureList, lIndex);

	//if(lRet != -1)
	//	WriteEachData(pscdm, pFeatureInfoList,(long)dwID);

	long lRetReg = -1;

	lRetReg = WriteDataJson((long)lRet);

EXIT:

	return lRet;
}


//=============================================================================
// function
//      WriteData
//
// parameters
//      pszTitle            èëÇ´çûÇﬁÉ^ÉCÉgÉã
//      pscdm               èëÇ´çûÇﬁSCDMç\ë¢ëÃÉAÉhÉåÉX
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
//long CShJsonUS::WriteData(WCHAR FAR *pszTitle, PSCDM pscdm, long lIndex, long lID, WCHAR FAR *pszTime, BOOL bSelf)
//{
//	long			lRet = -1;
//
//	WCHAR FAR		*pszKey = NULL;
//
//	long			lCount = 0;
//
//	DWORD			dwData = 0;
//	DWORD			dwSize = 0;
//
//	short			wBy = 1;
//
//	if(pscdm == NULL)
//		goto EXIT;
//
//	pszKey = new WCHAR[SCUI_REGKEYSIZE];
//	if(pszKey == NULL)
//		goto EXIT;
//
//	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
//
//	lCount = ReadCount();
//	if(lIndex == -1 || lIndex > lCount)
//		lIndex = lCount;
//
//	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);
//
//	if(pszTitle != NULL)
//	{
//		// --- title
//		WriteJsonStrData(pszKey, JSON_ENT_US_TITLE, pszTitle, (DWORD)(::wcslen((const wchar_t *)pszTitle)));
//	}
//
//	// --- id
//	if(lID > -1)
//	{
//		dwData = (DWORD)lID;
//		WriteJsonDWORDData(pszKey, JSON_ENT_US_ID, dwData);
//	}
//
//	if(lIndex > 0)
//	{
//		//time
//		WCHAR	szTimeA[ SCUI_FILEDATETIME_STRLEN ] = {0};
//		WCHAR	szTime[ SCUI_FILEDATETIME_STRLEN * 2] = {0};
//		if(pszTime == NULL)
//		{
//			GetLocalTimeStr(szTime);
//			//convert_wchar(szTime, szTimeA,sizeof(szTime));
//		}
//		else
//		{
//			memcpy(szTime,pszTime,sizeof(szTime));
//		}
//
//		WriteJsonStrData(pszKey, JSON_ENT_US_TIME, szTime, sizeof(szTime));
//
//		//self flag
//		dwData = (DWORD)bSelf;
//		WriteJsonDWORDData(pszKey, JSON_ENT_US_SELF,dwData);
//	}
//
//	// --- data (binary)
//	int scdmSize = sizeof(SCDM);
//	WriteJsonBinaryData(pszKey, JSON_ENT_US_DATA, (WCHAR *)pscdm, scdmSize);
//
//	// --- count
//	if(lIndex == lCount)
//	{
//		if(WriteCount(lCount + 1) == FALSE)
//			goto EXIT;
//	}
//
//	lRet = lIndex;
//
//EXIT:
//
//	if(pszKey != NULL)
//		delete[] pszKey;
//
//	return lRet;
//}
long CShJsonUS::WriteData(WCHAR FAR *pszTitle, PGPDFEATUREOPTIONLIST pFeatureList, PSCDM pscdm, long lIndex, long lID, WCHAR FAR *pszTime, BOOL bSelf)
{
	long			lRet = -1;

	WCHAR FAR		*pszKey = NULL;

	long			lCount = 0;

	DWORD			dwData = 0;
	DWORD			dwSize = 0;

	short			wBy = 1;

	if (pscdm == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if (pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);

	lCount = ReadCount();
	if (lIndex == -1 || lIndex > lCount)
		lIndex = lCount;

	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);

	if (pszTitle != NULL)
	{
		// --- title
		WriteJsonStrData(pszKey, JSON_ENT_US_TITLE, pszTitle, (DWORD)(::wcslen((const wchar_t *)pszTitle)));
	}

	// --- id
	if (lID > -1)
	{
		dwData = (DWORD)lID;
		WriteJsonDWORDData(pszKey, JSON_ENT_US_ID, dwData);
	}

	if (lIndex > 0)
	{
		//time
		WCHAR	szTimeA[SCUI_FILEDATETIME_STRLEN] = { 0 };
		WCHAR	szTime[SCUI_FILEDATETIME_STRLEN * 2] = { 0 };
		if (pszTime == NULL)
		{
			GetLocalTimeStr(szTime);
			//convert_wchar(szTime, szTimeA,sizeof(szTime));
		}
		else
		{
			memcpy(szTime, pszTime, sizeof(szTime));
		}

		WriteJsonStrData(pszKey, JSON_ENT_US_TIME, szTime, sizeof(szTime));

		//self flag
		dwData = (DWORD)bSelf;
		WriteJsonDWORDData(pszKey, JSON_ENT_US_SELF, dwData);
	}

	// --- data (binary)
	int scdmSize = sizeof(SCDM);
	WriteJsonBinaryData(pszKey, JSON_ENT_US_DATA, (WCHAR *)pscdm, scdmSize);

	// --- FeatureOptionData (binary)
	int featureinfolistsize = sizeof(GPDFEATUREOPTIONLIST);
	WriteJsonBinaryData(pszKey, JSON_ENT_US_FEATUREOPTIONDATA, (WCHAR *)pFeatureList, scdmSize);
	//WriteJsonBinaryData(HKEY_CURRENT_USER, pszKey, REG_ENT_US_FEATUREOPTIONDATA, REG_BINARY, (WCHAR*)pFeatureList, sizeof(GPDFEATUREOPTIONLIST));

	// --- count
	if (lIndex == lCount)
	{
		if (WriteCount(lCount + 1) == FALSE)
			goto EXIT;
	}

	lRet = lIndex;

EXIT:

	if (pszKey != NULL)
		delete[] pszKey;

	return lRet;
}

//=============================================================================
// function
//      WriteCount
//
// parameters
//      dwCount             èëÇ´çûÇﬁíl
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      UserSettingêîÇèëÇ´çûÇﬁ
//=============================================================================
BOOL CShJsonUS::WriteCount(DWORD dwCount)
{
	BOOL			blRet = FALSE;
	
	//blRet = WriteJsonDWORDData(JSON_KEY_US_ROOT_BASE, (const WCHAR*)JSON_ENT_US_COUNT, dwCount);
	blRet = WriteJsonDWORDData(JSON_KEY_US_ROOT_BASE, L"count", dwCount);
		
	blRet = TRUE;

	return blRet;
}


//=============================================================================
// function
//      WriteVersion
//
// parameters
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      SCDMÇÃÉoÅ[ÉWÉáÉìÇèëÇ´çûÇﬁ
//=============================================================================
BOOL CShJsonUS::WriteVersion(PSCDM pscdm)
{
	BOOL			blRet = FALSE;
	DWORD			dwData = 0;

	if(pscdm == NULL)
		goto EXIT;

	// --- version
	dwData = (*pscdm).wStructVersion;
	WriteJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_SCDM_VERSION, dwData);

	// --- size
	dwData = sizeof(SCDM);
	WriteJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_SCDM_SIZE, dwData);

	blRet = TRUE;

EXIT:
	return blRet;
}


//=============================================================================
// function
//      WriteLocale
//
// parameters
//      
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      ÉçÉPÅ[ÉãÇ…àÀë∂Ç∑ÇÈèÓïÒÇèëÇ´çûÇﬁ
//=============================================================================
BOOL CShJsonUS::WriteLocale()
{
	BOOL			blRet = FALSE;
	DWORD			dwData = 0;

	// --- paper size
	dwData = GetPaperSizeDef();
	WriteJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_LOCALE_PSIZE, dwData);

	// --- unit
	dwData = GetUnitDef();
	WriteJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_LOCALE_UNIT, dwData);

	blRet = TRUE;

	return blRet;
}


//=============================================================================
// function
//      WriteID
//
// parameters
//      lIndex              èëÇ´çûÇﬁUserSettingÉCÉìÉfÉbÉNÉX
//      lID                èëÇ´çûÇﬁIDî‘çÜ
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      IDÇèëÇ´çûÇﬁ
//=============================================================================
BOOL CShJsonUS::WriteID(long lIndex, long lID)
{
	BOOL			blRet = FALSE;

	DWORD			dwData = 0;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);

	// --- id
	dwData = (DWORD)lID;
	WriteJsonDWORDData(pszKey, JSON_ENT_US_ID, dwData);

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      WriteChapterInsPage
//
// parameters
//      pszChpInsPage       èëÇ´çûÇﬁíl
//      size                èëÇ´çûÇﬁÉTÉCÉY
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      UserSettingêîÇèëÇ´çûÇﬁ
//=============================================================================
BOOL CShJsonUS::WriteChapterInsPage(long lIndex, WCHAR FAR *pszChpInsPage, LONG size)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	DWORD			dwData = 0;

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);

	// --- szChpInsPage
	WriteJsonBinaryData(pszKey, JSON_ENT_US_CHAPTERINSPG, pszChpInsPage, size);

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      ReadData
//
// parameters
//      lIndex              ì«Ç›éÊÇÈÉCÉìÉfÉbÉNÉX
//      pdpd                ì«Ç›éÊÇÈDIFFPAPERDATAç\ë¢ëÃÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : ÉfÅ[É^Ç…ïKóvÇ»ÉTÉCÉY
//      é∏îs : 0
//
// outline
//      éwíËÇ≥ÇÍÇΩÉCÉìÉfÉbÉNÉXÇÃUserSettingèÓïÒÇÉåÉWÉXÉgÉäÇ©ÇÁì«Ç›çûÇﬁ
//=============================================================================
long CShJsonUS::ReadData(PSCDM pscdm, long lIndex)
{
	long			lRet = 0;

	WCHAR FAR		*pszKey = NULL;

	DWORD			dwSize = 0;

	if(pscdm == NULL)
	{
		lRet = sizeof(SCDM);
		goto EXIT;
	}

	// ================================================
	// _/_/_/  ì«Ç›çûÇ›
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);

	// --- data
	dwSize = sizeof(SCDM);
	if(ReadJsonBinaryData(pszKey,JSON_ENT_US_DATA,(WCHAR*)pscdm, dwSize) == FALSE)
		goto EXIT;

	lRet = sizeof(SCDM);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return lRet;
}


//=============================================================================
// function
//      ReadCount
//
// parameters
//
// return value
//      ê¨å˜ : ìoò^UserSettingêî
//      é∏îs : 0
//
// outline
//      UserSettingêîÇì«Ç›çûÇﬁ
//=============================================================================
DWORD CShJsonUS::ReadCount()
{
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;
	
	// --- count
	//blSuccess = ReadJsonDWORDData(JSON_KEY_US_ROOT_BASE, (const WCHAR*)JSON_ENT_US_COUNT, dwData);
	blSuccess = ReadJsonDWORDData(JSON_KEY_US_ROOT_BASE, L"count", dwData);

	return dwData;
}


//=============================================================================
// function
//      ReadVersion
//
// parameters
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      ÉçÉPÅ[ÉãÇ…àÀë∂Ç∑ÇÈèÓïÒÇèëÇ´çûÇﬁ
//=============================================================================
BOOL CShJsonUS::ReadVersion(short *pwVersion, short *pwSize)
{
	BOOL			blRet = FALSE;
	DWORD			dwData = 0;

	// --- version
	blRet = ReadJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_SCDM_VERSION, dwData);
	if(blRet == FALSE)
		goto EXIT;

	*pwVersion = (short)dwData;

	// --- size
	blRet = ReadJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_SCDM_SIZE, dwData);
	if(blRet == FALSE)
		goto EXIT;

	*pwSize = (short)dwData;

	blRet = TRUE;

EXIT:

	return blRet;
}


//=============================================================================
// function
//      ReadLocale
//
// parameters
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      ÉçÉPÅ[ÉãÇ…àÀë∂Ç∑ÇÈèÓïÒÇì«Ç›çûÇﬁ
//=============================================================================
BOOL CShJsonUS::ReadLocale(long *plPSize, long *plUnit)
{
	BOOL			blRet = FALSE;
	DWORD			dwData = 0;

	// --- paper size
	blRet = ReadJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_LOCALE_PSIZE, dwData);
	if(blRet == FALSE)
		goto EXIT;

	*plPSize = dwData;

	// --- unit
	blRet = ReadJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_LOCALE_UNIT, dwData);
	if(blRet == FALSE)
		goto EXIT;

	*plUnit = dwData;

	blRet = TRUE;

EXIT:
	return blRet;
}


//=============================================================================
// function
//      ReadID
//
// parameters
//      lIndex              ì«Ç›çûÇﬁUserSettingÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      IDÇì«Ç›çûÇﬁ
//=============================================================================
long CShJsonUS::ReadID(long lIndex)
{
	long			lRet = -1;

	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);

	// --- id
	blSuccess = ReadJsonDWORDData(pszKey, JSON_ENT_US_ID, dwData);
	if(blSuccess == FALSE)
		goto EXIT;

	lRet = (long)dwData;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return lRet;
}

//=============================================================================
// function
//      ReadChapterInsPage
//
// parameters
//      lIndex              ì«Ç›çûÇﬁUserSettingÉCÉìÉfÉbÉNÉX
//      pszChpInsPage       ChapterInsPageÇÃäiî[êÊ
//      size                ChapterInsPageÇÃsize
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      IDÇì«Ç›çûÇﬁ
//=============================================================================
long CShJsonUS::ReadChapterInsPage(long lIndex, WCHAR FAR *pszChpInsPage, LONG size)
{
	long			lRet = -1;

	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	WCHAR FAR		*pszKey = NULL;

	DWORD			dwSize;

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);

	dwSize = size;
	blSuccess = ReadJsonBinaryData(pszKey,JSON_ENT_US_CHAPTERINSPG,pszChpInsPage, dwSize);

	if(blSuccess == FALSE)
		goto EXIT;

	lRet = (long)dwData;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return lRet;
}

//=============================================================================
// function
//      DeleteData
//
// parameters
//      wIndex              çÌèúÇ∑ÇÈUserSettingÉCÉìÉfÉbÉNÉX
//
// return value
//      Ç»Çµ
//
// outline
//      éwíËÇÃÉCÉìÉfÉbÉNÉXÇÃUserSettingÉfÅ[É^ÇçÌèúÇ∑ÇÈ
//=============================================================================
void CShJsonUS::DeleteData(short wIndex)
{
	long			lDelID;
	lDelID = ReadID(wIndex);

	DeleteChildItemData(JSON_KEY_US_ROOT_BASE,wIndex);

	if(lDelID != -1)
		DeleteEachData(lDelID);

	return;
}

//=============================================================================
// function
//      DeleteDataLimitByOption
//
// parameters
//      wIndex              çÌèúÇ∑ÇÈUserSettingÉCÉìÉfÉbÉNÉX
//
// return value
//      Ç»Çµ
//
// outline
//      éwíËÇÃÉCÉìÉfÉbÉNÉXÇÃUserSettingÉfÅ[É^ÇçÌèúÇ∑ÇÈ
//=============================================================================
void CShJsonUS::DeleteDataLimitByOption(BOOL *pblLimitByOptionTbl)
{
	DWORD			dwCount;
	DWORD			dwSrcIndex;

	dwCount = ReadCount();

	for(dwSrcIndex = 1; dwSrcIndex <= dwCount; dwSrcIndex++)
	{
		if(*(pblLimitByOptionTbl + dwCount - dwSrcIndex) == TRUE)
		{
			DeleteData((short)(dwCount - dwSrcIndex));
		}
	}

	return;
}

//=============================================================================
// function
//      GetTitle
//
// parameters
//      lIndex              èÓïÒÇéÊìæÇ∑ÇÈÉCÉìÉfÉbÉNÉX
//      pszTitle            É^ÉCÉgÉãÇéÛÇØéÊÇÈÉoÉbÉtÉ@
//      dwBufSize           pszTitleÉTÉCÉY
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      UserSettingÇÃÉ^ÉCÉgÉãÇéÊìæÇ∑ÇÈ
//=============================================================================
BOOL CShJsonUS::GetTitle(WCHAR FAR *pszTitle, DWORD dwBufSize, long lIndex)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	DWORD			dwSize = 0;

	// ================================================
	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
	// ================================================
	if(pszTitle == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  ì«Ç›çûÇ›
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);

	// --- title
	dwSize = dwBufSize;
	ReadJsonStrData(pszKey,JSON_ENT_US_TITLE,pszTitle, dwSize);

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      GetTime
//
// parameters
//      lIndex              èÓïÒÇéÊìæÇ∑ÇÈÉCÉìÉfÉbÉNÉX
//      pszTime            É^ÉCÉgÉãÇéÛÇØéÊÇÈÉoÉbÉtÉ@
//      dwBufSize           pszTimeÉTÉCÉY
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      Get the UserSetting's time
//=============================================================================
BOOL CShJsonUS::GetTime(WCHAR  *pszTime, DWORD dwBufSize, long lIndex)
{
	BOOL			blRet = FALSE;
	WCHAR			*pszKey = NULL;
	DWORD			dwSize = 0;

	// ================================================
	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
	// ================================================
	if(pszTime == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  ì«Ç›çûÇ›
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);

	// --- time
	dwSize = dwBufSize;
	ReadJsonStrData(pszKey,JSON_ENT_US_TIME,pszTime, dwSize);

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      GetSelfFlag
//
// parameters
//      lIndex              ì«Ç›çûÇﬁUserSettingÉCÉìÉfÉbÉNÉX
//
// return value
//       TRUE: is local fav item
//       FALE: is the shared fav item
//
// outline
//      get the self flag
//=============================================================================
BOOL CShJsonUS::GetSelfFlag(long lIndex)
{
	BOOL			bSelf = TRUE;
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;
	WCHAR 			*pszKey = NULL;

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, lIndex);

	// --- self
	blSuccess = ReadJsonDWORDData(pszKey,JSON_ENT_US_SELF,dwData);
	if(blSuccess == FALSE)
		goto EXIT;

	bSelf = (BOOL)dwData;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return bSelf;
}

//=============================================================================
// function
//      IsLocaleChanged
//
// parameters
//
// return value
//      ïœçXÇ†ÇË : TRUE
//      ïœçXÇ»Çµ : FALE
//
// outline
//      ÉçÉPÅ[ÉãÇ…àÀë∂Ç∑ÇÈèÓïÒÇÃïœçXÇí≤Ç◊ÇÈ
//=============================================================================
BOOL CShJsonUS::IsLocaleChanged()
{
	BOOL			blRet = TRUE;

	long			lPSize = -1;
	long			lUnit = -1;

	if(ReadLocale(&lPSize, &lUnit) == FALSE)
		goto EXIT;

	if(lPSize != GetPaperSizeDef())
		goto EXIT;

	if(lUnit != GetUnitDef())
		goto EXIT;

	blRet = FALSE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      IsVersionChanged
//
// parameters
//
// return value
//      ïœçXÇ†ÇË : TRUE
//      ïœçXÇ»Çµ : FALE
//
// outline
//      ÉåÉWÉXÉgÉäÇ…ï€ë∂Ç≥ÇÍÇƒÇ¢ÇÈèÓïÒÇÃSCDMÉoÅ[ÉWÉáÉìÇ™
//		åªç›Ç∆ìØÇ∂Ç©Ç«Ç§Ç©ÇéÊìæÇ∑ÇÈ
//=============================================================================
BOOL CShJsonUS::IsVersionChanged()
{
	BOOL			blRet = TRUE;

	PSCDM			pscdm = NULL;

	short			wVer = -1;
	short			wSize = -1;

	if(ReadVersion(&wVer, &wSize) != FALSE)
	{
		if((wVer == SCDM_VER) && (wSize == sizeof(SCDM)))
		{
			blRet = FALSE;
		}
	}
	else
	{
		pscdm = (PSCDM)new SCDM;
		if(pscdm != NULL)
		{
			if(ReadData(pscdm, 0) == 0)
				goto EXIT;

#ifdef _USE_FIXED_SCDM_SIZE
			if(((*pscdm).wStructVersion == SCDM_VER) && ((*pscdm).wStructSize == SCDM_SIZE))
#else	
			if(((*pscdm).wStructVersion == SCDM_VER) && ((*pscdm).wStructSize == sizeof(SCDM)))
#endif	// #ifdef _USE_FIXED_SCDM_SIZE
			{
				blRet = FALSE;
			}
		}
	}

EXIT:

	if(pscdm != NULL)
		delete pscdm;

	return blRet;
}

//=============================================================================
// function
//      SearchUnusedID
//
// parameters
//
// return value
//      ID
//
// outline
//      égópÇ≥ÇÍÇƒÇ¢Ç»Ç¢IDÇéÊìæÇ∑ÇÈ
//=============================================================================
DWORD CShJsonUS::SearchUnusedID()
{
	DWORD		dwRet = 100;
	DWORD		dwID = 0;
	DWORD		dwData = 0;
	DWORD		dwSize = 0;
	BOOL		blSuccess = FALSE;
	DWORD		i;
	vector<Json::Value> vecValue;
	vector<DWORD> vecID;

	GetChildItemsValue(JSON_KEY_US_ROOT_BASE,JSON_ENT_US_ID,vecValue);

	for(i = 0; i < vecValue.size(); i++)
	{
		DWORD dwIDInVec = vecValue[i].asInt();
		vecID.push_back(dwIDInVec);
	}

	while(1)
	{
		for(i = 0; i < vecID.size(); i++)
		{
			if(vecID[i] == dwID)
				break;
		}

		if(i == vecID.size())
			break;

		dwID++;
	}

	dwRet = dwID;

	return dwRet;
}

//=============================================================================
// function
//      update
//
// parameters
//      pscdm				SCDMç\ë¢ëÃÇÃÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      INIÉtÉ@ÉCÉãÇÃÉfÅ[É^ÇégópÇµÇƒÉåÉWÉXÉgÉäÇçXêVÇ∑ÇÈ
//=============================================================================
BOOL CShJsonUS::update(PSCDM pscdm)
{
	BOOL			blRet = FALSE;

	BOOL			blSuccess = FALSE;

	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*pszData = NULL;

	short			wVer = 0;
	short			j = 0;
	short			*pwList = NULL;

	DWORD			dwSize;
	DWORD			dwCount;
	DWORD			dwID;
	DWORD			i;

	PSCDM			preset = NULL;
	LPBYTE			pData = NULL;
	PUSUPDATELIST	plist = NULL;

	// ================================================
	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
	// ================================================
	if(pscdm == NULL)
		goto EXIT;

	preset = (PSCDM)new SCDM;
	if(preset == NULL)
		goto EXIT;

	SecureZeroMemory(preset, sizeof(SCDM));

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);


	// ================================================
	// _/_/_/  ÉåÉWÉXÉgÉäÇÃèÓïÒÇçXêVÇ∑ÇÈ
	// ================================================
	dwCount = ReadCount();

	for(i = 0; i < dwCount; i++)
	{
		memcpy(preset, pscdm, sizeof(SCDM));

		if(i == 0)
		{
			// --- factory defaults
			reset(preset, FALSE);
		}
		else
		{
			// --- others
			// ================================================
			// _/_/_/  IDÇÃéÊìæ
			// ================================================
			::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, i);

			blSuccess = ReadJsonDWORDData(pszKey,JSON_ENT_US_ID,dwID);
			if(blSuccess == FALSE)
				continue;

			// ================================================
			// _/_/_/  èâä˙âªÇ∑ÇÈÉfÅ[É^ÇÃÉäÉXÉgÉAÉbÉv
			// ================================================
			dwSize = MakeUpdateList((short)dwID, NULL);
			if(dwSize != 0)
			{
				plist = (PUSUPDATELIST)new WCHAR[dwSize];
				if(plist != NULL)
					MakeUpdateList((short)dwID, plist);
			}

			// ================================================
			// _/_/_/  ÉåÉWÉXÉgÉäÇÃèÓïÒÇè„èëÇ´
			// ================================================
			ReadEachData(preset, (long)dwID, plist);

			// ================================================
			// _/_/_/  ÉåÉWÉXÉgÉäÇ…ï€ë∂ÇµÇ»Ç®Ç∑
			// ================================================
			//bearWriteData(NULL, preset, i, (long)dwID);

			//bearWriteEachData(preset, (long)dwID);

			WriteUpdateList((short)dwID, plist);

			if(plist != NULL)
			{
				delete[] plist;
				plist = NULL;
			}
		}
	}

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	if(preset != NULL)
		delete preset;

	return blRet;
}

//=============================================================================
// function
//      IsEnableUpdate
//
// parameters
//      pscdm				SCDMç\ë¢ëÃÇÃÉAÉhÉåÉX
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      INIÉtÉ@ÉCÉãÇÃÉfÅ[É^ÇégópÇµÇƒÉåÉWÉXÉgÉäÇçXêVÇ∑ÇÈ
//=============================================================================
BOOL CShJsonUS::IsEnableUpdate()
{
	BOOL			blRet = FALSE;

#ifdef UPDATE_DIFFVER_US
	BOOL			blSuccess = FALSE;

	WCHAR FAR		*pszKey = NULL;

	DWORD			dwCount;
	DWORD			dwID;
	DWORD			dwData;
	DWORD			i;

	//jsontemp/*if(CreateObj(scui_obj_ini_mcf) != FALSE)
	//{
	//	if((*m_pmcf).GetLong(MCF_SEC_DEBUG, MCF_KEY_DEBUG_US_NOT_UPDATE) == 1)
	//		goto EXIT;
	//}*/

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);

	// ================================================
	// _/_/_/  ÉåÉWÉXÉgÉäÇÃèÓïÒÇçXêVÇ∑ÇÈ
	// ================================================
	dwCount = ReadCount();

	for(i = 0; i < dwCount; i++)
	{
		if(i == 0)
			continue;

		// ================================================
		// _/_/_/  IDÇÃéÊìæ
		// ================================================
		SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
		::wsprintf(pszKey, JSON_KEY_US_INDEX_BASE, i);

		blSuccess = ReadJsonDWORDData(pszKey,JSON_ENT_US_ID,dwID);
		if(blSuccess == FALSE)
			break;

		// ================================================
		// _/_/_/  èÓïÒÇ™èëÇ´çûÇ‹ÇÍÇƒÇ¢ÇÈÇ±Ç∆ÇämîF
		// ================================================
		SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
		::wsprintf(pszKey, JSON_KEY_US_EACH_ID_BASE, dwID);

		dwData = 0;
		blSuccess = ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMSIZE,dwData);
		if(blSuccess == FALSE)
			break;
	}

	if(i == dwCount)
		blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;
#endif //UPDATE_DIFFVER_US

	return blRet;
}

//=============================================================================
// function
//      WriteEachData
//
// parameters
//      pscdm               èëÇ´çûÇﬁSCDMç\ë¢ëÃÉAÉhÉåÉX
//      dwID                èëÇ´çûÇﬁID
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæID
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃIDÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::WriteEachData(PSCDM pscdm, PFEATUREINFOLIST pFeatureInfoList, long lID)
{
	long			lRet = -1;

	DWORD			dwData = 0;
	DWORD			dwDesc = 0;
	
	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*pszEnt = NULL;
	LPPUBDMODE		ppub = NULL;
	LP_DOCDMODE		pdoc = NULL;
	LP_EXTDMODE		pext = NULL;
	LP_DEVDMODE		pdev = NULL;
#if SCDM_VER >= 3000
	LP_EXT2DMODE	pext2 = NULL;
#endif


	size_t nFeatureIndex = 0;
	size_t nNumberOfFeatures = 0;
	/*Get the GPD feature Count*/
	nNumberOfFeatures = GetGPDFeatureCount();
	/*Get the GPD feature List */
	WCHAR** GPDFeatureList = GetGPDFeatureListW();
	if (m_pszSvrPrnName == NULL)
		goto EXIT;

	if(pscdm == NULL)
		goto EXIT;

	if(lID < 0)
		goto EXIT;

	ppub = &(*pscdm).pub;
	pdoc = &(*pscdm).doc;
	pext = &(*pscdm).ext;
	pdev = &(*pscdm).dev;
#if SCDM_VER >= 3000
	pext2 = &(*pscdm).ext2;
#endif

	//pszKey = new WCHAR[SCUI_REGKEYSIZE];
	//pszEnt = new WCHAR[SCUI_REGKEYSIZE];
	//if (pszKey == NULL || pszEnt == NULL)
	//	goto EXIT;

	//SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	//SecureZeroMemory(pszEnt, SCUI_REGKEYSIZE);


	//// PrinterName -> Driver Name

	//if (CreateRegPath(REG_KEY_US_EACH_ID_BASE, pszKey, SCUI_REGKEYSIZE, pszSvrPrnName, lID) == FALSE)
	//	goto EXIT;


	//// ================================================
	//// _/_/_/  subkeyÇ
	//// ================================================
	//if (sh_RegCreateKeyEx(HKEY_CURRENT_USER, pszKey, 0, NULL, REG_OPTION_NON_VOLATILE,
	//	KEY_WRITE, NULL, &hKey, &dwDesc) != ERROR_SUCCESS)
	//	goto EXIT;



	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	pszEnt = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	if (pszEnt == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	SecureZeroMemory(pszEnt, SCUI_REGKEYSIZE);

	::wsprintf(pszKey, JSON_KEY_US_EACH_ID_BASE, lID);

	PCSTR			pStr = NULL;
	DWORD			dwSize = 0;
	///*Update GPD feature Option in registry*///
	for (nFeatureIndex = 0; nFeatureIndex < nNumberOfFeatures; nFeatureIndex++)
	{
		pStr = NULL;
		dwSize = 0;
		pStr = pFeatureInfoList->pFeatureOptionPair[nFeatureIndex].pszOption;
		dwSize = strlen(pStr) + 1;
		convert_wchar((char *)pszEnt, (char *)pStr, dwSize);
		//sh_RegSetValueEx(hKey, GPDFeatureList[nFeatureIndex], 0, REG_SZ, (LPBYTE)pszEnt, ::wcslen((const wchar_t *)pszEnt) * 2);

		WriteJsonStrData(pszKey, GPDFeatureList[nFeatureIndex], (WCHAR*)pszEnt, /*sizeof(pszEnt)*/::wcslen((const wchar_t *)pszEnt) * 2);
	}

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
	// --- PUBDMODE
	WriteJsonStrData(pszKey, MCF_KEY_DEF_P_DMDEVICENAME, (WCHAR*)(*ppub).dmDeviceName, sizeof((*ppub).dmDeviceName));

	dwData = (DWORD)(*ppub).dmSpecVersion;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMSPECVERSION, dwData);

	dwData = (DWORD)(*ppub).dmDriverVersion;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMDRIVERVERSION, dwData);

	dwData = (DWORD)(*ppub).dmSize;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMSIZE, dwData);

	dwData = (DWORD)(*ppub).dmDriverExtra;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMDRIVEREXTRA, dwData);

	dwData = (DWORD)(*ppub).dmFields;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMFIELDS , dwData);

	dwData = (DWORD)(*ppub).dmOrientation;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMORIENTATION , dwData);

	dwData = (DWORD)(*ppub).dmPaperSize;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMPAPERSIZE, dwData);

	dwData = (DWORD)(*ppub).dmPaperLength;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMPAPERLENGTH, dwData);

	dwData = (DWORD)(*ppub).dmPaperWidth;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMPAPERWIDTH , dwData);

	dwData = (DWORD)(*ppub).dmScale;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMSCALE , dwData);

	dwData = (DWORD)(*ppub).dmCopies;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMCOPIES , dwData);

	dwData = (DWORD)(*ppub).dmDefaultSource;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMDEFAULTSOURCE , dwData);

	dwData = (DWORD)(*ppub).dmPrintQuality;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMPRINTQUALITY, dwData);

	dwData = (DWORD)(*ppub).dmColor;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMCOLOR , dwData);

	dwData = (DWORD)(*ppub).dmDuplex;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMDUPLEX, dwData);

	dwData = (DWORD)(*ppub).dmYResolution;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMYRESOLUTION, dwData);

	dwData = (DWORD)(*ppub).dmTTOption;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMTTOPTION, dwData);

	dwData = (DWORD)(*ppub).dmCollate;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMCOLLATE, dwData);

	WriteJsonStrData(pszKey,MCF_KEY_DEF_P_DMFORMNAME , (WCHAR*)(*ppub).dmFormName, sizeof((*ppub).dmFormName));

	dwData = (DWORD)(*ppub).dmLogPixels;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMLOGPIXELS , dwData);

	dwData = (DWORD)(*ppub).dmBitsPerPel;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMBITSPERPEL , dwData);

	dwData = (DWORD)(*ppub).dmPelsWidth;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMPELSWIDTH , dwData);

	dwData = (DWORD)(*ppub).dmPelsHeight;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMPELSHEIGHT , dwData);

	dwData = (DWORD)(*ppub).dmDisplayFlags;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMDISPLAYFLAGS, dwData);

	dwData = (DWORD)(*ppub).dmDisplayFrequency;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMDISPLAYFREQUENCY, dwData);

	dwData = (DWORD)(*ppub).dmICMMethod;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMICMMETHOD, dwData);

	dwData = (DWORD)(*ppub).dmICMIntent;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMICMINTENT, dwData);

	dwData = (DWORD)(*ppub).dmMediaType;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMMEDIATYPE, dwData);

	dwData = (DWORD)(*ppub).dmDitherType;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMDITHERTYPE, dwData);

	// --- _DOCDMODE(dwFieldDDM)
	dwData = ((*pdoc).dwFieldDDM & DM_RASTER) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_DMRASTER , dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_NEGATIVEIMG) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_NEGATIVEIMG, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_EUROCHARA) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_EUROCHARA, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_PSERRORINFO) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_PSERRORINFO , dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_PSPASSTHROUGH) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_PSPASSTHROUGH , dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_RASSURF_MONO) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_RUSSURF_MONO, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_RASSURF_GRSC) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_RUSSURF_GRSC, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_RASSURF_COLOR) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_RUSSURF_COLOR, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_RASSURF_AUTOCHG) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_RUSSURF_AUTOCHG, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_FILLTOBLACK) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_FILLTOBLACK, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_FONT_SELECT) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_FONT_SELECT , dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_ALLTEXTBLACK) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_ALLTEXTBLACK, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_ALLVECTORBLACK) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_ALLVECTORBLACK, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_USE_PMFBYPASS) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_USE_PMFBYPASS, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_SCALE_MYSELF) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_SCALE_MYSELF, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_PCLCOLOR) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_PCLCOLOR , dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_EMFJOURNAL) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_EMFJOURNAL , dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_GRAPHMODE_AUTO) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_GRAPHMODE_AUTO , dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_WYSIWYG) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_WYSIWYG, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_LINEWIDTH_HW) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_LINEWIDTH_HW, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_COLORADJ_CORE) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_COLORADJ_CORE, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_PCFAX_MODE) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_PCFAX_MODE, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_SKIP_DEVENT) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_SKIP_DEVENT, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_JPEG_PASSTHROUGH) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_JPEG_PASSTHROUGH, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_SJIS_MSFONT) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_SJIS_MSFONT, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_ZOOM_CENTER) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_ZOOM_CENTER, dwData);

	dwData = ((*pdoc).dwFieldDDM & DM_COLORPRINT_ALERT) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_COLORPRINT_ALERT, dwData);

	// --- _DOCDMODE
	dwData = (DWORD)(*pdoc).wStructSize;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_WSTRUCTSIZE, dwData);

	dwData = (DWORD)(*pdoc).byJobComp;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_BYJOBCOMP , dwData);

	dwData = (DWORD)(*pdoc).byBitmapComp;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYBITMAPCOMP, dwData);

	dwData = (DWORD)(*pdoc).byBitmapCompOption;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYBITMAPCOMPOPTION, dwData);

	dwData = (DWORD)(*pdoc).byJpegSampleFactor;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_BYJPEGSAMPLEFACTOR , dwData);

	dwData = (DWORD)(*pdoc).byDLFormat;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_BYDLFORMAT , dwData);

	dwData = (DWORD)(*pdoc).byOutputBPP;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYOUTPUTBPP, dwData);

	dwData = (DWORD)(*pdoc).byUserPatternFillRatio;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_BYPATTERNFILLRATIO , dwData);

	dwData = (DWORD)(*pdoc).byOutputBitmap;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYOUTPUTBITMAP, dwData);

	dwData = (DWORD)(*pdoc).byOutputTexts;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_BYOUTPUTTEXTS , dwData);

	dwData = (DWORD)(*pdoc).byOutputVectors;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYOUTPUTVECTORS, dwData);

	dwData = (DWORD)(*pdoc).byMinPenWidth;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYMINPENWIDTH, dwData);

#if SCDM_VER >= 1918
	dwData = (DWORD)(*pdoc).byReduced;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_BYREDUCED , dwData);
#endif

	dwData = (DWORD)(*pdoc).dwImageableWidthPixel;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_DWIMAGEABLEWIDTHPIXEL , dwData);

	dwData = (DWORD)(*pdoc).dwImageableHeightPixel;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_DWIMAGEABLEHEIGHTPIXEL , dwData);

	dwData = (DWORD)(*pdoc).nLeftMarginPixel;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_NLEFTMARGINPIXEL , dwData);

	dwData = (DWORD)(*pdoc).nTopMarginPixel;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_NTOPMARGINPIXEL , dwData);

	dwData = (DWORD)(*pdoc).nSrcLeftMarginMilli;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_NSRCLEFTMARGINMILLI, dwData);

	dwData = (DWORD)(*pdoc).nSrcTopMarginMilli;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_NSRCTOPMARGINMILLI, dwData);

	dwData = (DWORD)(*pdoc).nSrcRightMarginMilli;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_NSRCRIGHTMARGINMILLI , dwData);

	dwData = (DWORD)(*pdoc).nSrcBottomMarginMilli;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_NSRCBOTTOMMARGINMILLI, dwData);

	dwData = (DWORD)(*pdoc).wPenWidthRate;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_WPENWIDTHRATE , dwData);

	dwData = (DWORD)(*pdoc).dwDownloadRAM;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_DWDOWNLOADRAM , dwData);

#if SCDM_VER >= 1917
	dwData = (DWORD)(*pdoc).dwPaperFold;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_D_DWPAPERFOLD , dwData);

	dwData = (DWORD)(*pdoc).nInsChapter;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_D_NINSCHAPTER, dwData);
#endif	// #if SCDM_VER >= 1917
	
#if SCDM_VER >= 1918
	// --- _EXTDMODE(dwFieldEDM1)
	dwData = ((*pext).wFieldEDM & DM_WIDENING) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMWIDENING , dwData);
#endif

#if SCDM_VER >= 1920
	// --- _EXTDMODE(dwFieldEDM)
	dwData = ((*pext).wFieldEDM & DM_LONG_PRINT) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMLONGPRINT, dwData);
#endif

#if SCDM_VER >= 1920
	// --- _EXTDMODE(dwFieldEDM)
	dwData = ((*pext).wFieldEDM & DM_PRINTPOSITION) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPRINTPOSITION, dwData);
#endif

	// --- _EXTDMODE(dwFieldEDM1)
	dwData = ((*pext).dwFieldEDM1 & DM_PUNCH) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMPUNCH , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_DIFPAPER) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDIFPAPER, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_DIF2SIDED) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDIF2SIDED, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_TPINSERTS) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMTPINSERTS , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_TPPRINTED) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMTPPRINTED, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_PAGEPROTECT) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPAGEPROTECT, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_RIGHTTRNPAMPH) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMRIGHTTRNPAMPH, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_ROTATE180DEG) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMROTATE180DEG , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_OFFSET) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMOFFSET , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_LONGEDGEFEED) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMLONGEDGEFEED , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_TANDEMPRINT) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMTANDEMPRINT , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_PAGEINTERLEAVE) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPAGEINTERLEAVE, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_NUPBORDER) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMNUPBORDER, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_POSBORDER) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPOSBORDER, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_FITTOPAGE) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMFITTOPAGE, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_TPTEXT) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMTPTEXT , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_FIRSTPAGE) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMFIRSTPAGE, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_OUTLINEONLY) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMOUTLINEONLY, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_QUERYOVERLAY) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMQUERYOVERLAY, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_USEOVERLAY) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMUSEOVERLAY , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_CREATEOVERLAY) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMCREATEOVERLAY, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_REVORDER) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMREVORDER, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_LOCKASPECTRATIO) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMLOCKASPECTRATIO , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_XYFITTOPAGE) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMXYFITTOPAGE, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_XYZOOM) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMXYZOOM, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_DIFCOVERBLANK) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMDIFCOVERBLANK , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_DIFLASTBLANK) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDIFLASTBLANK, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_TABPRINT) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMTABPRINT, dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_DIF1SIDED) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMDIF1SIDED , dwData);

	dwData = ((*pext).dwFieldEDM1 & DM_EXT_RESOLUTION) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMEXTRESUOL , dwData);

#if SCDM_VER >= 1917
	dwData = ((*pext).dwFieldEDM1 & DM_REVERSEPRINT) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMREVERSEPRINT, dwData);
#endif	// #if SCDM_VER >= 1908

	// --- _EXTDMODE(dwFieldEDM2)
	dwData = ((*pext).dwFieldEDM2 & DM_CARBONCOPY) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMCARBONCOPY, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_NUPREPEAT) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMNUPREPEAT, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_ROTATE180DEG_HW) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMROTATE180DEGHW, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_MARGINSHIFT_HW) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMMARGINSHIFTHW, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_PAMPHLET_HW) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMPAMPHLETHW , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_REVORDER_HW) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMREVORDERHW , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_DUPLEX_HW) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDUPLEXHW, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_COLLATE_HW) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMCOLLATEHW, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_UNCOLLATE_HW) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMUNCOLLATEHW, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_SMOOTHING) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMSMOOTHING , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_PHOTOENHANCE) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMPHOTOENHANCE , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_TONERSAVE) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMTONERSAVE , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_CUSTOMTA) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMCUSTOMTA , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_MIRRORVERT) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMMIRRORVERT , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_MIRRORHORZ) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMMIRRORHORZ, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_CARBONCOPY_HW) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMCARBONCOPYHW, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_EDGETOEDGE) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMEDGETOEDGE , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_MGNSHIFT_L) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMMGNSHIFTL , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_MGNSHIFT_T) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMMGNSHIFTT, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_MGNSHIFT_R) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMMGNSHIFTR, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_MGNSHIFT_B) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMMGNSHIFTB , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_DOCUMENTCONTROL) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDOCUMENTCONTROL, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_SPECCOLORMODE) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMSPECCOLORMODE , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_WMTRANS_ROP) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMWMTRANSROP, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_PRIORITY_COLLATE) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMPRICOLLATE , dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_PRIORITY_DUPLEX) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPRIDUPLEX, dwData);

	dwData = ((*pext).dwFieldEDM2 & DM_PRIORITY_COLOR) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMPRICOLOR , dwData);

#if SCDM_VER >= 1918
	dwData = ((*pext).dwFieldEDM2 & DM_SHORT_JOBNAME) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_SHORTJOBNAME , dwData);
#endif

	// --- _EXTDMODE
	dwData = (DWORD)(*pext).wStructSize;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_WSTRUCTSIZE , dwData);

	dwData = (DWORD)(*pext).byNupX;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYNUPX, dwData);

	dwData = (DWORD)(*pext).byNupY;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYNUPY , dwData);

	dwData = (DWORD)(*pext).byNupOrder;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYNUPORDER , dwData);

	dwData = (DWORD)(*pext).byPoster;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYPOSTER, dwData);

	dwData = (DWORD)(*pext).byPamph;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYPAMPH , dwData);

	dwData = (DWORD)(*pext).byBindedge;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYBINDEDGE, dwData);

	dwData = (DWORD)(*pext).byStaple;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYSTAPLE , dwData);

	dwData = (DWORD)(*pext).byCusUnits;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYCUSUNITS , dwData);

	dwData = (DWORD)(*pext).byZoomUnit;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYZOOMUNIT, dwData);

	dwData = (DWORD)(*pext).byMgnShift;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYMGNSHIFT , dwData);

	dwData = (DWORD)(*pext).byMgnShiftY;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYMGNSHIFTY , dwData);

	dwData = (DWORD)(*pext).byThresholdArray;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYTHRESHOLDARRAY, dwData);

	dwData = (DWORD)(*pext).nZoom;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NZOOM , dwData);

	dwData = (DWORD)(*pext).nZoomY;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NZOOMY, dwData);

	dwData = (DWORD)(*pext).nZoomSrcPaperSize;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NZOOMSRCPAPERSIZE , dwData);

	dwData = (DWORD)(*pext).nOverlap;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NOVERLAP , dwData);

	dwData = (DWORD)(*pext).nFTPPaperSize;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NFTPPAPERSIZE, dwData);

	dwData = (DWORD)(*pext).nPamphPaperSize;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NPAMPHPAPERSIZE , dwData);

	dwData = (DWORD)(*pext).nSignature;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NSIGNATURE , dwData);

	dwData = (DWORD)(*pext).nOutput;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NOUTPUT , dwData);

	dwData = (DWORD)(*pext).nMgnShiftPixel;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NMGNSHIFTPIXEL, dwData);

	dwData = (DWORD)(*pext).nMgnShiftYPixel;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NMGNSHIFTYPIXEL, dwData);

	dwData = (DWORD)(*pext).nCbnTopPaperSource;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NCBNTOPPAPERSOURCE , dwData);

	dwData = (DWORD)(*pext).nCbnPaperSource;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NCBNPAPERSOURCE, dwData);

	dwData = (DWORD)(*pext).nDiffPaperSource;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDIFFPAPERSOURCE , dwData);

	dwData = (DWORD)(*pext).nDiffPaperType;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NDIFFPAPERTYPE, dwData);

	dwData = (DWORD)(*pext).nDiffPaperSourceLast;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NDIFFPAPERSOURCELAST, dwData);

	dwData = (DWORD)(*pext).nDiffPaperTypeLast;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NDIFFPAPERTYPELAST, dwData);

	dwData = (DWORD)(*pext).nTransPaperSource;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NTRANSPAPERSOURCE , dwData);

	dwData = (DWORD)(*pext).nTransPaperType;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NTRANSPAPERTYPE , dwData);

	dwData = (DWORD)(*pext).nTabShiftPixel;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NTABSHIFTPIXEL , dwData);

	dwData = (DWORD)(*pext).nOverlayFile;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NOVERLAYFILE, dwData);

	dwData = (DWORD)(*pext).nWatermark;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NWATERMARK , dwData);

	dwData = (DWORD)(*pext).nImageStamp;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NIMAGESTAMP, dwData);

	dwData = (DWORD)(*pext).nAntiCopy;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NANTICOPY , dwData);

	dwData = (DWORD)(*pext).nDiffPaper;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NDIFFPAPER, dwData);

	dwData = (DWORD)(*pext).nTabPrint;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NTABPRINT, dwData);

	dwData = (DWORD)(*pext).nCarbonCopy;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NCARBONCOPY , dwData);

	dwData = (DWORD)(*pext).nDstWidthMilli;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NDSTWIDTHMILLI, dwData);

	dwData = (DWORD)(*pext).nDstHeightMilli;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDSTHEIGHTMILLI , dwData);

	dwData = (DWORD)(*pext).nDstLeftMarginPixel;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDSTLEFTMARGINPIXEL , dwData);

	dwData = (DWORD)(*pext).nDstTopMarginPixel;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_NDSTTOPMARGINPIXEL, dwData);

	dwData = (DWORD)(*pext).dmDuplexSave;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMDUPLEXSAVE , dwData);

	dwData = (DWORD)(*pext).dmDefaultSourceSave;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDEFAULTSOURCESAVE, dwData);

	dwData = (DWORD)(*pext).dwCusWidthPixel;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWCUSWIDTHPIXEL, dwData);

	dwData = (DWORD)(*pext).dwCusLengthPixel;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWCUSLENGTHPIXEL, dwData);

	dwData = (DWORD)(*pext).dwDstWidthPixel;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWDSTWIDTHPIXEL, dwData);

	dwData = (DWORD)(*pext).dwDstHeightPixel;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWDSTHEIGHTPIXEL, dwData);

	dwData = (DWORD)(*pext).dwZoomWidthPixel;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_DWZOOMWIDTHPIXEL , dwData);

	dwData = (DWORD)(*pext).dwZoomHeightPixel;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWZOOMHEIGHTPIXEL, dwData);

	dwData = (DWORD)(*pext).byResolution;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYRESOLUTION, dwData);

	dwData = (DWORD)(*pext).byPosterSave;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYPOSTERSAVE , dwData);

	dwData = (DWORD)(*pext).byDocPatternCol;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYDOCPATTERNCOL, dwData);

#if SCDM_VER >= 1917
	dwData = (DWORD)(*pext).byPunchType;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYPUNCHTYPE , dwData);
#endif	// #if SCDM_VER >= 1917

	dwData = (DWORD)(*pext).nCbnTopPaperType;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NCBNTOPPAPERTYPE , dwData);

	dwData = (DWORD)(*pext).nCbnPaperType;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_NCBNPAPERTYPE , dwData);

	// --- _EXTDMODE(JobCtrl)
	dwData = (DWORD)(*pext).JobCtrl.wFieldJC;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_JC_WFIELDJC, dwData);

	dwData = (DWORD)(*pext).JobCtrl.byRetention;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_JC_BYRETENTION, dwData);

	dwData = (DWORD)(*pext).JobCtrl.byPinEncryption;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_JC_BYPINENCRYPTION , dwData);

	dwData = (DWORD)(*pext).JobCtrl.byDocumentFiling;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_JC_BYDOCUMENTFILING , dwData);

	dwData = (DWORD)(*pext).JobCtrl.nDocFilingFolder;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_JC_NDOCFILINGFOLDER , dwData);

	// --- _EXTDMODE(ColData)
	dwData = (DWORD)(*pext).ColData.wFieldCD;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_WFIELDCD, dwData);

	dwData = (DWORD)(*pext).ColData.byDocType;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYDOCTYPE , dwData);

	dwData = (DWORD)(*pext).ColData.byColRender;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYCOLRENDER , dwData);

	dwData = (DWORD)(*pext).ColData.byUCRSelect;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYUCRSELECT , dwData);

	dwData = (DWORD)(*pext).ColData.byScreeningBmp;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYSCREENINGBMP , dwData);

	dwData = (DWORD)(*pext).ColData.byScreeningGraph;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYSCREENINGGRAPH , dwData);

	dwData = (DWORD)(*pext).ColData.byScreeningText;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYSCREENINGTEXT, dwData);

	dwData = (DWORD)(*pext).ColData.byColMode;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYCOLMODE, dwData);

	dwData = (DWORD)(*pext).ColData.byPrnMode;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYPRNMODE , dwData);

	dwData = (DWORD)(*pext).ColData.bySpotColorMathing;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYSPOTCOLORMATHING , dwData);

	dwData = (DWORD)(*pext).ColData.byInputProfile;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYINPUTPROFILE , dwData);

	dwData = (DWORD)(*pext).ColData.byOutputProfile;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYOUTPUTPROFILE, dwData);

	dwData = (DWORD)(*pext).ColData.byInkSimulation;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYINKSIMULATION, dwData);

	dwData = (DWORD)(*pext).ColData.byIntentBmp;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYINTENTBMP, dwData);

	dwData = (DWORD)(*pext).ColData.byIntentGraph;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYINTENTGRAPH , dwData);

	dwData = (DWORD)(*pext).ColData.byIntentText;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYINTENTTEXT , dwData);

#if SCDM_VER >= 1918
	dwData = (DWORD)(*pext).ColData.byResolType;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYRESOLTYPE , dwData);
#endif	

	dwData = (DWORD)(*pext).ColData.nBrightness;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NBRIGHTNESS, dwData);

	dwData = (DWORD)(*pext).ColData.nContrast;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NCONTRAST, dwData);

	dwData = (DWORD)(*pext).ColData.nSaturation;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_NSATURATION , dwData);

	dwData = (DWORD)(*pext).ColData.nRedStrength;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NREDSTRENGTH, dwData);

	dwData = (DWORD)(*pext).ColData.nGreenStrength;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_NGREENSTRENGTH , dwData);

	dwData = (DWORD)(*pext).ColData.nBlueStrength;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NBLUESTRENGTH, dwData);

	dwData = (DWORD)(*pext).ColData.nGBRed;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_NGBRED , dwData);

	dwData = (DWORD)(*pext).ColData.nGBGreen;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NGBGREEN, dwData);

	dwData = (DWORD)(*pext).ColData.nGBBlue;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NGBBLUE, dwData);

	dwData = (DWORD)(*pext).ColData.nScreenFreq;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NSCREENFREQ, dwData);

	dwData = (DWORD)(*pext).ColData.nScreenAng;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_NSCREENANG , dwData);

	dwData = (DWORD)(*pext).ColData.byInputProfileDevice;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYINPUTPROFILEDEVICE , dwData);

	dwData = (DWORD)(*pext).ColData.byOutputProfileDevice;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYOUTPUTPROFILEDEVICE, dwData);

#if SCDM_VER >= 1919
	dwData = (DWORD)(*pext).ColData.byDensity;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYDENSITY , dwData);
#endif	

#if SCDM_VER >= 1920
	dwData = (DWORD)(*pext).ColData.byDitherID;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_CD_BYDITHERID , dwData);
#endif	

	// --- _EXTDMODE(PCFax)
	dwData = (DWORD)(*pext).PCFax.wColorMode;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_FX_WCOLORMODE , dwData);

	dwData = (DWORD)(*pext).PCFax.wCompType;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_FX_WCOMPTYPE , dwData);

	dwData = (DWORD)(*pext).PCFax.wRotate;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E_FX_WROTATE , dwData);

	// --- _DEVDMODE(dwFieldVDM)
	dwData = ((*pdev).dwFieldVDM & DM_DUPLEXMODULE) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_DMDUPLEXMODULE , dwData);

	dwData = ((*pdev).dwFieldVDM & DM_INVERTERUNIT) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_DMINVERTERUNIT , dwData);

	dwData = ((*pdev).dwFieldVDM & DM_HARDDISK) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_DMHARDDISK , dwData);

	dwData = ((*pdev).dwFieldVDM & DM_SCANNERUNIT) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_DMSCANNERUNIT , dwData);

	dwData = ((*pdev).dwFieldVDM & DM_ROPM) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_DMROPM , dwData);

	dwData = ((*pdev).dwFieldVDM & DM_DATASECURITYKIT) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_DMDATASECURITYKIT , dwData);

#if SCDM_VER >= 1917

	dwData = ((*pdev).dwFieldVDM & DM_GBCPUNCH) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_DMGBCPUNCH , dwData);

	dwData = ((*pdev).dwFieldVDM & DM_FOLDINGUNIT) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_DMFOLDINGUNIT , dwData);
#endif	

	// --- _DEVDMODE
	dwData = (DWORD)(*pdev).byPunchmodule;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_BYPUNCHMODULE , dwData);

	dwData = (DWORD)(*pdev).dwInputTray;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_NINPUTTRAY , dwData);

	dwData = (DWORD)(*pdev).dwOutputTray;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_NOUTPUTTRAY , dwData);

	dwData = (DWORD)(*pdev).nInstalledRAM;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_V_NINSTALLEDRAM, dwData);

	dwData = (DWORD)(*pdev).nSTSPSource;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_NSTSPSOURCE , dwData);

	dwData = (DWORD)(*pdev).nSTSPSize;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_V_NSTSPSIZE , dwData);

	dwData = (DWORD)(*pdev).nSTSPType;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_V_NSTSPTYPE, dwData);

#if SCDM_VER >= 3000
	// --- _EXT2DMODE AdvancedData
	dwData = (DWORD)((*pext2).AdvancedData.byExpand & DM_EXPAND_TEXT) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E2_ADV_EXP_TEXT, dwData);

	dwData = (DWORD)((*pext2).AdvancedData.byExpand & DM_EXPAND_GRAPHICS) ? 1 : 0;
	WriteJsonDWORDData(pszKey,MCF_KEY_DEF_E2_ADV_EXP_GRAPHICS , dwData);

	dwData = (DWORD)((*pext2).AdvancedData.byExpand & DM_EXPAND_PHOTO) ? 1 : 0;
	WriteJsonDWORDData(pszKey, MCF_KEY_DEF_E2_ADV_EXP_PHOTO, dwData);
#endif

	lRet = lID;

EXIT:	
	if(pszKey != NULL)
		delete[] pszKey;

	if (pszEnt != NULL)
		delete[] pszEnt;

	return lRet;
}

//=============================================================================
// function
//      ReadEachData
//
// parameters
//      pscdm               ì«Ç›çûÇﬁSCDMç\ë¢ëÃÉAÉhÉåÉX
//      dwID                ì«Ç›çûÇﬁID
//      pwReset             ÉäÉZÉbÉgÇ∑ÇÈÉÅÉìÉoÇÃÉäÉXÉg(óvÇ∑ÇÈÇ…ì«Ç›çûÇ‹Ç»Ç¢)
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæID
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃIDÇ≈ÉåÉWÉXÉgÉäÇ©ÇÁ
//      ì«Ç›çûÇﬁ
//=============================================================================
long CShJsonUS::ReadEachData(PSCDM pscdm, long lID, PUSUPDATELIST pusul)
{
	long			lRet = -1;

	DWORD			dwData = 0;
	DWORD			dwSize = 0;

	WCHAR*			pszOut = NULL;

	WCHAR FAR		*pszKey = NULL;

	LPPUBDMODE		ppub = NULL;
	LP_DOCDMODE		pdoc = NULL;
	LP_EXTDMODE		pext = NULL;
	LP_DEVDMODE		pdev = NULL;
#if SCDM_VER >= 3000
	LP_EXT2DMODE	pext2 = NULL;
#endif

	if(pscdm == NULL)
		goto EXIT;

	if(lID < 0)
		goto EXIT;

	ppub = &(*pscdm).pub;
	pdoc = &(*pscdm).doc;
	pext = &(*pscdm).ext;
	pdev = &(*pscdm).dev;
#if SCDM_VER >= 3000
	pext2 = &(*pscdm).ext2;
#endif

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_US_EACH_ID_BASE, lID);

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
	// --- PUBDMODE
	if(this->SearchUpdateList(pusul, MCF_KEY_DEF_P_DMDEVICENAME) == -1)
	{
		dwSize = sizeof((*ppub).dmDeviceName);
		pszOut = (WCHAR*)(*ppub).dmDeviceName;			
		CShJson::ReadJsonStrData(pszKey,MCF_KEY_DEF_P_DMDEVICENAME,pszOut, dwSize);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMSPECVERSION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMSPECVERSION, dwData);
		(*ppub).dmSpecVersion = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMDRIVERVERSION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMDRIVERVERSION , dwData);
		(*ppub).dmDriverVersion = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMSIZE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMSIZE, dwData);
		(*ppub).dmSize = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMFIELDS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMFIELDS, dwData);
		(*ppub).dmFields = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMORIENTATION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMORIENTATION, dwData);
		(*ppub).dmOrientation = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMPAPERSIZE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMPAPERSIZE , dwData);
		(*ppub).dmPaperSize = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMPAPERLENGTH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMPAPERLENGTH, dwData);
		(*ppub).dmPaperLength = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMPAPERWIDTH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMPAPERWIDTH, dwData);
		(*ppub).dmPaperWidth = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMSCALE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMSCALE, dwData);
		(*ppub).dmScale = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMCOPIES) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMCOPIES, dwData);
		(*ppub).dmCopies = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMDEFAULTSOURCE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMDEFAULTSOURCE, dwData);
		(*ppub).dmDefaultSource = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMPRINTQUALITY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMPRINTQUALITY , dwData);
		(*ppub).dmPrintQuality = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMCOLOR) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMCOLOR , dwData);
		(*ppub).dmColor = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMDUPLEX) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMDUPLEX, dwData);
		(*ppub).dmDuplex = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMYRESOLUTION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMYRESOLUTION, dwData);
		(*ppub).dmYResolution = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMTTOPTION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMTTOPTION , dwData);
		(*ppub).dmTTOption = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMCOLLATE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMCOLLATE, dwData);
		(*ppub).dmCollate = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMFORMNAME) == -1)
	{
		dwSize = sizeof((*ppub).dmFormName);
		pszOut = (WCHAR*)(*ppub).dmFormName;
		ReadJsonStrData(pszKey,MCF_KEY_DEF_P_DMFORMNAME,pszOut, dwSize);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMLOGPIXELS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMLOGPIXELS, dwData);
		(*ppub).dmLogPixels = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMBITSPERPEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMBITSPERPEL, dwData);
		(*ppub).dmBitsPerPel = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMPELSWIDTH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMPELSWIDTH, dwData);
		(*ppub).dmPelsWidth = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMPELSHEIGHT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMPELSHEIGHT, dwData);
		(*ppub).dmPelsHeight = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMDISPLAYFLAGS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMDISPLAYFLAGS , dwData);
		(*ppub).dmDisplayFlags = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMDISPLAYFREQUENCY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMDISPLAYFREQUENCY, dwData);
		(*ppub).dmDisplayFrequency = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMICMMETHOD) == -1)
	{
		dwData = 0;
		this->ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMICMMETHOD , dwData);
		(*ppub).dmICMMethod = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMICMINTENT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_P_DMICMINTENT, dwData);
		(*ppub).dmICMIntent = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMMEDIATYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMMEDIATYPE , dwData);
		(*ppub).dmMediaType = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_P_DMDITHERTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_P_DMDITHERTYPE , dwData);
		(*ppub).dmDitherType = dwData;
	}

	// --- _DOCDMODE(dwFieldDDM)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_DMRASTER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_DMRASTER, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_RASTER) : ((*pdoc).dwFieldDDM & ~DM_RASTER);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_NEGATIVEIMG) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_NEGATIVEIMG, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_NEGATIVEIMG) : ((*pdoc).dwFieldDDM & ~DM_NEGATIVEIMG);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_EUROCHARA) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_EUROCHARA, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_EUROCHARA) : ((*pdoc).dwFieldDDM & ~DM_EUROCHARA);
	}

	//if(SearchUpdateList(pusul, MCF_KEY_DEF_D_PSERRORINFO) == -1)
	if (this->SearchUpdateList(pusul, MCF_KEY_DEF_D_PSERRORINFO) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_PSERRORINFO, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_PSERRORINFO) : ((*pdoc).dwFieldDDM & ~DM_PSERRORINFO);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_PSPASSTHROUGH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_PSPASSTHROUGH , dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_PSPASSTHROUGH) : ((*pdoc).dwFieldDDM & ~DM_PSPASSTHROUGH);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_RUSSURF_MONO) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_RUSSURF_MONO, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_RASSURF_MONO) : ((*pdoc).dwFieldDDM & ~DM_RASSURF_MONO);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_RUSSURF_GRSC) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_RUSSURF_GRSC, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_RASSURF_GRSC) : ((*pdoc).dwFieldDDM & ~DM_RASSURF_GRSC);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_RUSSURF_COLOR) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_RUSSURF_COLOR , dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_RASSURF_COLOR) : ((*pdoc).dwFieldDDM & ~DM_RASSURF_COLOR);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_RUSSURF_AUTOCHG) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_RUSSURF_AUTOCHG , dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_RASSURF_AUTOCHG) : ((*pdoc).dwFieldDDM & ~DM_RASSURF_AUTOCHG);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_FILLTOBLACK) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_FILLTOBLACK, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_FILLTOBLACK) : ((*pdoc).dwFieldDDM & ~DM_FILLTOBLACK);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_FONT_SELECT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_FONT_SELECT, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_FONT_SELECT) : ((*pdoc).dwFieldDDM & ~DM_FONT_SELECT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_ALLTEXTBLACK) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_ALLTEXTBLACK, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_ALLTEXTBLACK) : ((*pdoc).dwFieldDDM & ~DM_ALLTEXTBLACK);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_ALLVECTORBLACK) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_ALLVECTORBLACK, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_ALLVECTORBLACK) : ((*pdoc).dwFieldDDM & ~DM_ALLVECTORBLACK);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_USE_PMFBYPASS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_USE_PMFBYPASS, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_USE_PMFBYPASS) : ((*pdoc).dwFieldDDM & ~DM_USE_PMFBYPASS);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_SCALE_MYSELF) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_SCALE_MYSELF, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_SCALE_MYSELF) : ((*pdoc).dwFieldDDM & ~DM_SCALE_MYSELF);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_PCLCOLOR) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_PCLCOLOR , dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_PCLCOLOR) : ((*pdoc).dwFieldDDM & ~DM_PCLCOLOR);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_EMFJOURNAL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_EMFJOURNAL, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_EMFJOURNAL) : ((*pdoc).dwFieldDDM & ~DM_EMFJOURNAL);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_GRAPHMODE_AUTO) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_GRAPHMODE_AUTO, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_GRAPHMODE_AUTO) : ((*pdoc).dwFieldDDM & ~DM_GRAPHMODE_AUTO);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_WYSIWYG) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_WYSIWYG , dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_WYSIWYG) : ((*pdoc).dwFieldDDM & ~DM_WYSIWYG);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_LINEWIDTH_HW) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_LINEWIDTH_HW, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_LINEWIDTH_HW) : ((*pdoc).dwFieldDDM & ~DM_LINEWIDTH_HW);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_COLORADJ_CORE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_COLORADJ_CORE, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_COLORADJ_CORE) : ((*pdoc).dwFieldDDM & ~DM_COLORADJ_CORE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_PCFAX_MODE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_PCFAX_MODE, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_PCFAX_MODE) : ((*pdoc).dwFieldDDM & ~DM_PCFAX_MODE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_SKIP_DEVENT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_SKIP_DEVENT , dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_SKIP_DEVENT) : ((*pdoc).dwFieldDDM & ~DM_SKIP_DEVENT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_JPEG_PASSTHROUGH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_JPEG_PASSTHROUGH, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_JPEG_PASSTHROUGH) : ((*pdoc).dwFieldDDM & ~DM_JPEG_PASSTHROUGH);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_SJIS_MSFONT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_SJIS_MSFONT , dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_SJIS_MSFONT) : ((*pdoc).dwFieldDDM & ~DM_SJIS_MSFONT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_ZOOM_CENTER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_ZOOM_CENTER , dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_ZOOM_CENTER) : ((*pdoc).dwFieldDDM & ~DM_ZOOM_CENTER);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_COLORPRINT_ALERT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_COLORPRINT_ALERT, dwData);
		(*pdoc).dwFieldDDM = (dwData == 1) ? ((*pdoc).dwFieldDDM | DM_COLORPRINT_ALERT) : ((*pdoc).dwFieldDDM & ~DM_COLORPRINT_ALERT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYJOBCOMP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYJOBCOMP, dwData);
		(*pdoc).byJobComp = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYBITMAPCOMP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYBITMAPCOMP, dwData);
		(*pdoc).byBitmapComp = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYBITMAPCOMPOPTION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYBITMAPCOMPOPTION, dwData);
		(*pdoc).byBitmapCompOption = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYJPEGSAMPLEFACTOR) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYJPEGSAMPLEFACTOR, dwData);
		(*pdoc).byJpegSampleFactor = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYDLFORMAT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYDLFORMAT, dwData);
		(*pdoc).byDLFormat = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYOUTPUTBPP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYOUTPUTBPP, dwData);
		(*pdoc).byOutputBPP = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYPATTERNFILLRATIO) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYPATTERNFILLRATIO, dwData);
		(*pdoc).byUserPatternFillRatio = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYOUTPUTBITMAP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYOUTPUTBITMAP, dwData);
		(*pdoc).byOutputBitmap = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYOUTPUTTEXTS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYOUTPUTTEXTS, dwData);
		(*pdoc).byOutputTexts = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYOUTPUTVECTORS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYOUTPUTVECTORS, dwData);
		(*pdoc).byOutputVectors = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYMINPENWIDTH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYMINPENWIDTH, dwData);
		(*pdoc).byMinPenWidth = (BYTE)dwData;
	}

#if SCDM_VER >= 1918
	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_BYREDUCED) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_BYREDUCED, dwData);
		(*pdoc).byReduced = (BYTE)dwData;
	}
#endif

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_DWIMAGEABLEWIDTHPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_DWIMAGEABLEWIDTHPIXEL, dwData);
		(*pdoc).dwImageableWidthPixel = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_DWIMAGEABLEHEIGHTPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_DWIMAGEABLEHEIGHTPIXEL, dwData);
		(*pdoc).dwImageableHeightPixel = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_NLEFTMARGINPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_NLEFTMARGINPIXEL , dwData);
		(*pdoc).nLeftMarginPixel = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_NTOPMARGINPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_NTOPMARGINPIXEL, dwData);
		(*pdoc).nTopMarginPixel = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_NSRCLEFTMARGINMILLI) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_NSRCLEFTMARGINMILLI, dwData);
		(*pdoc).nSrcLeftMarginMilli = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_NSRCTOPMARGINMILLI) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_NSRCTOPMARGINMILLI, dwData);
		(*pdoc).nSrcTopMarginMilli = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_NSRCRIGHTMARGINMILLI) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_NSRCRIGHTMARGINMILLI, dwData);
		(*pdoc).nSrcRightMarginMilli = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_NSRCBOTTOMMARGINMILLI) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_NSRCBOTTOMMARGINMILLI , dwData);
		(*pdoc).nSrcBottomMarginMilli = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_WPENWIDTHRATE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_WPENWIDTHRATE, dwData);
		(*pdoc).wPenWidthRate = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_DWDOWNLOADRAM) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_DWDOWNLOADRAM , dwData);
		(*pdoc).dwDownloadRAM = dwData;
	}

#if SCDM_VER >= 1917
	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_DWPAPERFOLD) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_D_DWPAPERFOLD , dwData);
		(*pdoc).dwPaperFold = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_D_NINSCHAPTER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_D_NINSCHAPTER, dwData);
		(*pdoc).nInsChapter = (SHORT)dwData;
	}

#endif	// #if SCDM_VER >= 1917

#if SCDM_VER >= 1918
	// --- _EXTDMODE(wFieldEDM)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMWIDENING) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMWIDENING, dwData);
		(*pext).wFieldEDM = (WORD)((dwData == 1) ? ((*pext).wFieldEDM | DM_WIDENING) : ((*pext).wFieldEDM & ~DM_WIDENING));
	}
#endif

#if SCDM_VER >= 1920
	// --- _EXTDMODE(wFieldEDM)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMLONGPRINT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMLONGPRINT, dwData);
		(*pext).wFieldEDM = (WORD)((dwData == 1) ? ((*pext).wFieldEDM | DM_LONG_PRINT) : ((*pext).wFieldEDM & ~DM_LONG_PRINT));
	}
#endif

#if SCDM_VER >= 1920
	// --- _EXTDMODE(wFieldEDM)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPRINTPOSITION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPRINTPOSITION, dwData);
		(*pext).wFieldEDM = (WORD)((dwData == 1) ? ((*pext).wFieldEDM | DM_PRINTPOSITION) : ((*pext).wFieldEDM & ~DM_PRINTPOSITION));
	}
#endif
#if SCDM_VER >= 1923
	// --- _EXTDMODE(wFieldEDM)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMWOVEN) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMWOVEN , dwData);
		(*pext).wFieldEDM = (WORD)((dwData == 1) ? ((*pext).wFieldEDM | DM_WOVEN) : ((*pext).wFieldEDM & ~DM_WOVEN));
	}
#endif

	// --- _EXTDMODE(dwFieldEDM1)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPUNCH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPUNCH, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_PUNCH) : ((*pext).dwFieldEDM1 & ~DM_PUNCH);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMDIFPAPER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDIFPAPER, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_DIFPAPER) : ((*pext).dwFieldEDM1 & ~DM_DIFPAPER);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMDIF2SIDED) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDIF2SIDED, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_DIF2SIDED) : ((*pext).dwFieldEDM1 & ~DM_DIF2SIDED);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMTPINSERTS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMTPINSERTS , dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_TPINSERTS) : ((*pext).dwFieldEDM1 & ~DM_TPINSERTS);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMTPPRINTED) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMTPPRINTED, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_TPPRINTED) : ((*pext).dwFieldEDM1 & ~DM_TPPRINTED);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPAGEPROTECT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPAGEPROTECT, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_PAGEPROTECT) : ((*pext).dwFieldEDM1 & ~DM_PAGEPROTECT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMRIGHTTRNPAMPH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMRIGHTTRNPAMPH, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_RIGHTTRNPAMPH) : ((*pext).dwFieldEDM1 & ~DM_RIGHTTRNPAMPH);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMROTATE180DEG) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMROTATE180DEG, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_ROTATE180DEG) : ((*pext).dwFieldEDM1 & ~DM_ROTATE180DEG);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMOFFSET) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMOFFSET , dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_OFFSET) : ((*pext).dwFieldEDM1 & ~DM_OFFSET);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMLONGEDGEFEED) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMLONGEDGEFEED, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_LONGEDGEFEED) : ((*pext).dwFieldEDM1 & ~DM_LONGEDGEFEED);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMTANDEMPRINT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMTANDEMPRINT, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_TANDEMPRINT) : ((*pext).dwFieldEDM1 & ~DM_TANDEMPRINT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMNUPBORDER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMNUPBORDER , dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_NUPBORDER) : ((*pext).dwFieldEDM1 & ~DM_NUPBORDER);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPAGEINTERLEAVE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMPAGEINTERLEAVE , dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_PAGEINTERLEAVE) : ((*pext).dwFieldEDM1 & ~DM_PAGEINTERLEAVE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPOSBORDER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPOSBORDER, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_POSBORDER) : ((*pext).dwFieldEDM1 & ~DM_POSBORDER);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMFITTOPAGE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMFITTOPAGE, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_FITTOPAGE) : ((*pext).dwFieldEDM1 & ~DM_FITTOPAGE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMTPTEXT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMTPTEXT , dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_TPTEXT) : ((*pext).dwFieldEDM1 & ~DM_TPTEXT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMFIRSTPAGE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMFIRSTPAGE, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_FIRSTPAGE) : ((*pext).dwFieldEDM1 & ~DM_FIRSTPAGE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMOUTLINEONLY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMOUTLINEONLY, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_OUTLINEONLY) : ((*pext).dwFieldEDM1 & ~DM_OUTLINEONLY);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMQUERYOVERLAY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMQUERYOVERLAY , dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_QUERYOVERLAY) : ((*pext).dwFieldEDM1 & ~DM_QUERYOVERLAY);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMUSEOVERLAY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMUSEOVERLAY, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_USEOVERLAY) : ((*pext).dwFieldEDM1 & ~DM_USEOVERLAY);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMCREATEOVERLAY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMCREATEOVERLAY, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_CREATEOVERLAY) : ((*pext).dwFieldEDM1 & ~DM_CREATEOVERLAY);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMREVORDER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMREVORDER , dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_REVORDER) : ((*pext).dwFieldEDM1 & ~DM_REVORDER);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMLOCKASPECTRATIO) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMLOCKASPECTRATIO, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_LOCKASPECTRATIO) : ((*pext).dwFieldEDM1 & ~DM_LOCKASPECTRATIO);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMXYFITTOPAGE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMXYFITTOPAGE, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_XYFITTOPAGE) : ((*pext).dwFieldEDM1 & ~DM_XYFITTOPAGE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMXYZOOM) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMXYZOOM , dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_XYZOOM) : ((*pext).dwFieldEDM1 & ~DM_XYZOOM);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMDIFCOVERBLANK) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDIFCOVERBLANK, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_DIFCOVERBLANK) : ((*pext).dwFieldEDM1 & ~DM_DIFCOVERBLANK);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMDIFLASTBLANK) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDIFLASTBLANK, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_DIFLASTBLANK) : ((*pext).dwFieldEDM1 & ~DM_DIFLASTBLANK);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMTABPRINT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMTABPRINT, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_TABPRINT) : ((*pext).dwFieldEDM1 & ~DM_TABPRINT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMDIF1SIDED) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMDIF1SIDED , dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_DIF1SIDED) : ((*pext).dwFieldEDM1 & ~DM_DIF1SIDED);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMEXTRESUOL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMEXTRESUOL, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_EXT_RESOLUTION) : ((*pext).dwFieldEDM1 & ~DM_EXT_RESOLUTION);
	}

#if SCDM_VER >= 1917
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMREVERSEPRINT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMREVERSEPRINT, dwData);
		(*pext).dwFieldEDM1 = (dwData == 1) ? ((*pext).dwFieldEDM1 | DM_REVERSEPRINT) : ((*pext).dwFieldEDM1 & ~DM_REVERSEPRINT);
	}
#endif	// #if SCDM_VER >= 1908

	// --- _EXTDMODE(dwFieldEDM2)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMCARBONCOPY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMCARBONCOPY, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_CARBONCOPY) : ((*pext).dwFieldEDM2 & ~DM_CARBONCOPY);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMNUPREPEAT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMNUPREPEAT, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_NUPREPEAT) : ((*pext).dwFieldEDM2 & ~DM_NUPREPEAT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMROTATE180DEGHW) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMROTATE180DEGHW, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_ROTATE180DEG_HW) : ((*pext).dwFieldEDM2 & ~DM_ROTATE180DEG_HW);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMMARGINSHIFTHW) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMMARGINSHIFTHW , dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_MARGINSHIFT_HW) : ((*pext).dwFieldEDM2 & ~DM_MARGINSHIFT_HW);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPAMPHLETHW) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPAMPHLETHW, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_PAMPHLET_HW) : ((*pext).dwFieldEDM2 & ~DM_PAMPHLET_HW);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMREVORDERHW) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMREVORDERHW, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_REVORDER_HW) : ((*pext).dwFieldEDM2 & ~DM_REVORDER_HW);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMDUPLEXHW) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMDUPLEXHW, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_DUPLEX_HW) : ((*pext).dwFieldEDM2 & ~DM_DUPLEX_HW);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMCOLLATEHW) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMCOLLATEHW, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_COLLATE_HW) : ((*pext).dwFieldEDM2 & ~DM_COLLATE_HW);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMUNCOLLATEHW) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMUNCOLLATEHW, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_UNCOLLATE_HW) : ((*pext).dwFieldEDM2 & ~DM_UNCOLLATE_HW);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMSMOOTHING) == -1)
	{
		dwData = 0;
		dwData = ((*pext).dwFieldEDM2 & DM_SMOOTHING) ? 1 : 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMSMOOTHING, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_SMOOTHING) : ((*pext).dwFieldEDM2 & ~DM_SMOOTHING);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPHOTOENHANCE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMPHOTOENHANCE , dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_PHOTOENHANCE) : ((*pext).dwFieldEDM2 & ~DM_PHOTOENHANCE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMTONERSAVE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMTONERSAVE, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_TONERSAVE) : ((*pext).dwFieldEDM2 & ~DM_TONERSAVE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMCUSTOMTA) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMCUSTOMTA, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_CUSTOMTA) : ((*pext).dwFieldEDM2 & ~DM_CUSTOMTA);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMMIRRORVERT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMMIRRORVERT , dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_MIRRORVERT) : ((*pext).dwFieldEDM2 & ~DM_MIRRORVERT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMMIRRORHORZ) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMMIRRORHORZ , dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_MIRRORHORZ) : ((*pext).dwFieldEDM2 & ~DM_MIRRORHORZ);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMCARBONCOPYHW) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMCARBONCOPYHW , dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_CARBONCOPY_HW) : ((*pext).dwFieldEDM2 & ~DM_CARBONCOPY_HW);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMEDGETOEDGE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMEDGETOEDGE, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_EDGETOEDGE) : ((*pext).dwFieldEDM2 & ~DM_EDGETOEDGE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMMGNSHIFTL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMMGNSHIFTL , dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_MGNSHIFT_L) : ((*pext).dwFieldEDM2 & ~DM_MGNSHIFT_L);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMMGNSHIFTT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMMGNSHIFTT, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_MGNSHIFT_T) : ((*pext).dwFieldEDM2 & ~DM_MGNSHIFT_T);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMMGNSHIFTR) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMMGNSHIFTR, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_MGNSHIFT_R) : ((*pext).dwFieldEDM2 & ~DM_MGNSHIFT_R);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMMGNSHIFTB) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMMGNSHIFTB, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_MGNSHIFT_B) : ((*pext).dwFieldEDM2 & ~DM_MGNSHIFT_B);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMDOCUMENTCONTROL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMDOCUMENTCONTROL , dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_DOCUMENTCONTROL) : ((*pext).dwFieldEDM2 & ~DM_DOCUMENTCONTROL);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMSPECCOLORMODE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMSPECCOLORMODE , dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_SPECCOLORMODE) : ((*pext).dwFieldEDM2 & ~DM_SPECCOLORMODE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMWMTRANSROP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMWMTRANSROP, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_WMTRANS_ROP) : ((*pext).dwFieldEDM2 & ~DM_WMTRANS_ROP);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMINSCHAP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMINSCHAP, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_INSCHAP) : ((*pext).dwFieldEDM2 & ~DM_INSCHAP);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPRICOLLATE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPRICOLLATE, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_PRIORITY_COLLATE) : ((*pext).dwFieldEDM2 & ~DM_PRIORITY_COLLATE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPRIDUPLEX) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DMPRIDUPLEX, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_PRIORITY_DUPLEX) : ((*pext).dwFieldEDM2 & ~DM_PRIORITY_DUPLEX);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMPRICOLOR) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMPRICOLOR , dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_PRIORITY_COLOR) : ((*pext).dwFieldEDM2 & ~DM_PRIORITY_COLOR);
	}

#if SCDM_VER >= 1918
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_SHORTJOBNAME) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_SHORTJOBNAME, dwData);
		(*pext).dwFieldEDM2 = (dwData == 1) ? ((*pext).dwFieldEDM2 | DM_SHORT_JOBNAME) : ((*pext).dwFieldEDM2 & ~DM_SHORT_JOBNAME);
	}
#endif

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYNUPX) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYNUPX, dwData);
		(*pext).byNupX = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYNUPY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYNUPY, dwData);
		(*pext).byNupY = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYNUPORDER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYNUPORDER, dwData);
		(*pext).byNupOrder = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYPOSTER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYPOSTER, dwData);
		(*pext).byPoster = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYPAMPH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYPAMPH, dwData);
		(*pext).byPamph = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYBINDEDGE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYBINDEDGE , dwData);
		(*pext).byBindedge = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYSTAPLE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYSTAPLE, dwData);
		(*pext).byStaple = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYCUSUNITS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYCUSUNITS, dwData);
		(*pext).byCusUnits = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYZOOMUNIT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYZOOMUNIT, dwData);
		(*pext).byZoomUnit = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYMGNSHIFT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYMGNSHIFT , dwData);
		(*pext).byMgnShift = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYMGNSHIFTY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYMGNSHIFTY, dwData);
		(*pext).byMgnShiftY = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYTHRESHOLDARRAY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_BYTHRESHOLDARRAY , dwData);
		(*pext).byThresholdArray = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NZOOM) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NZOOM, dwData);
		(*pext).nZoom = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NZOOMY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NZOOMY , dwData);
		(*pext).nZoomY = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NZOOMSRCPAPERSIZE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NZOOMSRCPAPERSIZE, dwData);
		(*pext).nZoomSrcPaperSize = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NOVERLAP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NOVERLAP, dwData);
		(*pext).nOverlap = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NFTPPAPERSIZE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NFTPPAPERSIZE, dwData);
		(*pext).nFTPPaperSize = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NPAMPHPAPERSIZE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NPAMPHPAPERSIZE, dwData);
		(*pext).nPamphPaperSize = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NSIGNATURE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NSIGNATURE, dwData);
		(*pext).nSignature = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NOUTPUT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NOUTPUT, dwData);
		(*pext).nOutput = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NMGNSHIFTPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NMGNSHIFTPIXEL , dwData);
		(*pext).nMgnShiftPixel = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NMGNSHIFTYPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NMGNSHIFTYPIXEL , dwData);
		(*pext).nMgnShiftYPixel = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NCBNTOPPAPERSOURCE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NCBNTOPPAPERSOURCE, dwData);
		(*pext).nCbnTopPaperSource = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NCBNPAPERSOURCE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NCBNPAPERSOURCE, dwData);
		(*pext).nCbnPaperSource = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NDIFFPAPERSOURCE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDIFFPAPERSOURCE , dwData);
		(*pext).nDiffPaperSource = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NDIFFPAPERTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDIFFPAPERTYPE , dwData);
		(*pext).nDiffPaperType = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NDIFFPAPERSOURCELAST) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NDIFFPAPERSOURCELAST, dwData);
		(*pext).nDiffPaperSourceLast = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NDIFFPAPERTYPELAST) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDIFFPAPERTYPELAST , dwData);
		(*pext).nDiffPaperTypeLast = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NTRANSPAPERSOURCE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NTRANSPAPERSOURCE, dwData);
		(*pext).nTransPaperSource = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NTRANSPAPERTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NTRANSPAPERTYPE, dwData);
		(*pext).nTransPaperType = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NTABSHIFTPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NTABSHIFTPIXEL, dwData);
		(*pext).nTabShiftPixel = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NOVERLAYFILE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NOVERLAYFILE, dwData);
		(*pext).nOverlayFile = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NWATERMARK) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NWATERMARK , dwData);
		(*pext).nWatermark = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NIMAGESTAMP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NIMAGESTAMP , dwData);
		(*pext).nImageStamp = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NANTICOPY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NANTICOPY, dwData);
		(*pext).nAntiCopy = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NDIFFPAPER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDIFFPAPER , dwData);
		(*pext).nDiffPaper = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NTABPRINT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NTABPRINT, dwData);
		(*pext).nTabPrint = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NCARBONCOPY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NCARBONCOPY, dwData);
		(*pext).nCarbonCopy = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NDSTWIDTHMILLI) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDSTWIDTHMILLI , dwData);
		(*pext).nDstWidthMilli = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NDSTHEIGHTMILLI) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NDSTHEIGHTMILLI, dwData);
		(*pext).nDstHeightMilli = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NDSTLEFTMARGINPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDSTLEFTMARGINPIXEL , dwData);
		(*pext).nDstLeftMarginPixel = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NDSTTOPMARGINPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_NDSTTOPMARGINPIXEL , dwData);
		(*pext).nDstTopMarginPixel = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMDUPLEXSAVE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMDUPLEXSAVE , dwData);
		(*pext).dmDuplexSave = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DMDEFAULTSOURCESAVE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DMDEFAULTSOURCESAVE , dwData);
		(*pext).dmDefaultSourceSave = (short)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DWCUSWIDTHPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_E_DWCUSWIDTHPIXEL , dwData);
		(*pext).dwCusWidthPixel = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DWCUSLENGTHPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWCUSLENGTHPIXEL, dwData);
		(*pext).dwCusLengthPixel = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DWDSTWIDTHPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWDSTWIDTHPIXEL, dwData);
		(*pext).dwDstWidthPixel = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DWDSTHEIGHTPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWDSTHEIGHTPIXEL, dwData);
		(*pext).dwDstHeightPixel = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DWZOOMWIDTHPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWZOOMWIDTHPIXEL, dwData);
		(*pext).dwZoomWidthPixel = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_DWZOOMHEIGHTPIXEL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_DWZOOMHEIGHTPIXEL, dwData);
		(*pext).dwZoomHeightPixel = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYRESOLUTION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYRESOLUTION, dwData);
		(*pext).byResolution = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYPOSTERSAVE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYPOSTERSAVE, dwData);
		(*pext).byPosterSave = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYDOCPATTERNCOL) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYDOCPATTERNCOL, dwData);
		(*pext).byDocPatternCol = (BYTE)dwData;
	}

#if SCDM_VER >= 1917
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_BYPUNCHTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_BYPUNCHTYPE, dwData);
		(*pext).byPunchType = (BYTE)dwData;
	}
#endif	// #if SCDM_VER >= 1917

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NCBNTOPPAPERTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NCBNTOPPAPERTYPE, dwData);
		(*pext).nCbnTopPaperType = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_NCBNPAPERTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_NCBNPAPERTYPE, dwData);
		(*pext).nCbnPaperType = (SHORT)dwData;
	}

	// --- _EXTDMODE(JobCtrl)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_JC_WFIELDJC) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_JC_WFIELDJC, dwData);
		(*pext).JobCtrl.wFieldJC = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_JC_BYRETENTION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_JC_BYRETENTION, dwData);
		(*pext).JobCtrl.byRetention = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_JC_BYPINENCRYPTION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_JC_BYPINENCRYPTION, dwData);
		(*pext).JobCtrl.byPinEncryption = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_JC_BYDOCUMENTFILING) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_JC_BYDOCUMENTFILING, dwData);
		(*pext).JobCtrl.byDocumentFiling = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_JC_NDOCFILINGFOLDER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_JC_NDOCFILINGFOLDER, dwData);
		(*pext).JobCtrl.nDocFilingFolder = (SHORT)dwData;
	}

	// --- _EXTDMODE(ColData)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_WFIELDCD) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_WFIELDCD, dwData);
		(*pext).ColData.wFieldCD = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYDOCTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYDOCTYPE, dwData);
		(*pext).ColData.byDocType = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYCOLRENDER) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYCOLRENDER, dwData);
		(*pext).ColData.byColRender = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYUCRSELECT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYUCRSELECT, dwData);
		(*pext).ColData.byUCRSelect = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYSCREENINGBMP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYSCREENINGBMP, dwData);
		(*pext).ColData.byScreeningBmp = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYSCREENINGGRAPH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYSCREENINGGRAPH, dwData);
		(*pext).ColData.byScreeningGraph = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYSCREENINGTEXT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYSCREENINGTEXT, dwData);
		(*pext).ColData.byScreeningText = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYCOLMODE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYCOLMODE, dwData);
		(*pext).ColData.byColMode = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYPRNMODE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYPRNMODE, dwData);
		(*pext).ColData.byPrnMode = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYSPOTCOLORMATHING) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYSPOTCOLORMATHING, dwData);
		(*pext).ColData.bySpotColorMathing = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYINPUTPROFILE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYINPUTPROFILE, dwData);
		(*pext).ColData.byInputProfile = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYOUTPUTPROFILE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYOUTPUTPROFILE, dwData);
		(*pext).ColData.byOutputProfile = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYINKSIMULATION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYINKSIMULATION, dwData);
		(*pext).ColData.byInkSimulation = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYINTENTBMP) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYINTENTBMP, dwData);
		(*pext).ColData.byIntentBmp = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYINTENTGRAPH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYINTENTGRAPH, dwData);
		(*pext).ColData.byIntentGraph = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYINTENTTEXT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYINTENTTEXT, dwData);
		(*pext).ColData.byIntentText = (BYTE)dwData;
	}

#if SCDM_VER >= 1918
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYRESOLTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYRESOLTYPE, dwData);
		(*pext).ColData.byResolType = (BYTE)dwData;
	}
#endif	

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NBRIGHTNESS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NBRIGHTNESS, dwData);
		(*pext).ColData.nBrightness = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NCONTRAST) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NCONTRAST, dwData);
		(*pext).ColData.nContrast = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NSATURATION) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NSATURATION, dwData);
		(*pext).ColData.nSaturation = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NREDSTRENGTH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NREDSTRENGTH, dwData);
		(*pext).ColData.nRedStrength = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NGREENSTRENGTH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NGREENSTRENGTH, dwData);
		(*pext).ColData.nGreenStrength = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NBLUESTRENGTH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NBLUESTRENGTH, dwData);
		(*pext).ColData.nBlueStrength = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NGBRED) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NGBRED, dwData);
		(*pext).ColData.nGBRed = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NGBGREEN) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NGBGREEN, dwData);
		(*pext).ColData.nGBGreen = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NGBBLUE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NGBBLUE, dwData);
		(*pext).ColData.nGBBlue = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NSCREENFREQ) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NSCREENFREQ, dwData);
		(*pext).ColData.nScreenFreq = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_NSCREENANG) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_NSCREENANG, dwData);
		(*pext).ColData.nScreenAng = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYINPUTPROFILEDEVICE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYINPUTPROFILEDEVICE, dwData);
		(*pext).ColData.byInputProfileDevice = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYOUTPUTPROFILEDEVICE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYOUTPUTPROFILEDEVICE, dwData);
		(*pext).ColData.byOutputProfileDevice = (BYTE)dwData;
	}

#if SCDM_VER >= 1919
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYDENSITY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYDENSITY, dwData);
		(*pext).ColData.byDensity = (BYTE)dwData;
	}
#endif	// SCDM_VER >= 1919

#if SCDM_VER >= 1920
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_CD_BYDITHERID) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_CD_BYDITHERID, dwData);
		(*pext).ColData.byDitherID = (BYTE)dwData;
	}
#endif	// SCDM_VER >= 1920

	// --- _EXTDMODE(PCFax)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_FX_WCOLORMODE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_FX_WCOLORMODE, dwData);
		(*pext).PCFax.wCompType = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_FX_WCOMPTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_FX_WCOMPTYPE, dwData);
		(*pext).PCFax.wRotate = (WORD)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E_FX_WROTATE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E_FX_WROTATE, dwData);
		(*pext).PCFax.wColorMode = (WORD)dwData;
	}

	// --- _DEVDMODE(dwFieldVDM)
	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_DMDUPLEXMODULE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey,MCF_KEY_DEF_V_DMDUPLEXMODULE , dwData);
		(*pdev).dwFieldVDM = (dwData == 1) ? ((*pdev).dwFieldVDM | DM_DUPLEXMODULE) : ((*pdev).dwFieldVDM & ~DM_DUPLEXMODULE);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_DMINVERTERUNIT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_DMINVERTERUNIT, dwData);
		(*pdev).dwFieldVDM = (dwData == 1) ? ((*pdev).dwFieldVDM | DM_INVERTERUNIT) : ((*pdev).dwFieldVDM & ~DM_INVERTERUNIT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_DMHARDDISK) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_DMHARDDISK, dwData);
		(*pdev).dwFieldVDM = (dwData == 1) ? ((*pdev).dwFieldVDM | DM_HARDDISK) : ((*pdev).dwFieldVDM & ~DM_HARDDISK);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_DMSCANNERUNIT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_DMSCANNERUNIT, dwData);
		(*pdev).dwFieldVDM = (dwData == 1) ? ((*pdev).dwFieldVDM | DM_SCANNERUNIT) : ((*pdev).dwFieldVDM & ~DM_SCANNERUNIT);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_DMROPM) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_DMROPM, dwData);
		(*pdev).dwFieldVDM = (dwData == 1) ? ((*pdev).dwFieldVDM | DM_ROPM) : ((*pdev).dwFieldVDM & ~DM_ROPM);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_DMDATASECURITYKIT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_DMDATASECURITYKIT, dwData);
		(*pdev).dwFieldVDM = (dwData == 1) ? ((*pdev).dwFieldVDM | DM_DATASECURITYKIT) : ((*pdev).dwFieldVDM & ~DM_DATASECURITYKIT);
	}

#if SCDM_VER >= 1917

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_DMGBCPUNCH) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_DMGBCPUNCH, dwData);
		(*pdev).dwFieldVDM = (dwData == 1) ? ((*pdev).dwFieldVDM | DM_GBCPUNCH) : ((*pdev).dwFieldVDM & ~DM_GBCPUNCH);
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_DMFOLDINGUNIT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_DMFOLDINGUNIT, dwData);
		(*pdev).dwFieldVDM = (dwData == 1) ? ((*pdev).dwFieldVDM | DM_FOLDINGUNIT) : ((*pdev).dwFieldVDM & ~DM_FOLDINGUNIT);
	}

#endif	

	// --- _DEVDMODE
	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_BYPUNCHMODULE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_BYPUNCHMODULE, dwData);
		(*pdev).byPunchmodule = (BYTE)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_NINPUTTRAY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_NINPUTTRAY, dwData);
		(*pdev).dwInputTray = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_NOUTPUTTRAY) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_NOUTPUTTRAY, dwData);
		(*pdev).dwOutputTray = dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_NINSTALLEDRAM) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_NINSTALLEDRAM, dwData);
		(*pdev).nInstalledRAM = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_NSTSPSOURCE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_NSTSPSOURCE, dwData);
		(*pdev).nSTSPSource = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_NSTSPSIZE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_NSTSPSIZE, dwData);
		(*pdev).nSTSPSize = (SHORT)dwData;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_V_NSTSPTYPE) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_V_NSTSPTYPE, dwData);
		(*pdev).nSTSPType = (SHORT)dwData;
	}

#if SCDM_VER >= 3000
	// --- _EXT2DMODE AdvancedData
	if(SearchUpdateList(pusul, MCF_KEY_DEF_E2_ADV_EXP_TEXT) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E2_ADV_EXP_TEXT, dwData);
		(*pext2).AdvancedData.byExpand |= (SHORT)dwData == 1 ? DM_EXPAND_TEXT : 0;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E2_ADV_EXP_GRAPHICS) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E2_ADV_EXP_GRAPHICS, dwData);
		(*pext2).AdvancedData.byExpand |= (SHORT)dwData == 1 ? DM_EXPAND_GRAPHICS : 0;
	}

	if(SearchUpdateList(pusul, MCF_KEY_DEF_E2_ADV_EXP_PHOTO) == -1)
	{
		dwData = 0;
		ReadJsonDWORDData(pszKey, MCF_KEY_DEF_E2_ADV_EXP_PHOTO, dwData);
		(*pext2).AdvancedData.byExpand |= (SHORT)dwData == 1 ? DM_EXPAND_PHOTO : 0;
	}

#endif

	lRet = lID;


EXIT:
	if(pszKey != NULL)
		delete[] pszKey;

	return lRet;
}

//=============================================================================
// function
//      DeleteEachData
//
// parameters
//      lID					çÌèúÇ∑ÇÈIDÅiÇ∑Ç◊ÇƒçÌèúÇ∑ÇÈèÍçáÅF-1Åj
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃIDÇ≈ÉåÉWÉXÉgÉäÇ©ÇÁ
//      çÌèúÇ∑ÇÈ
//=============================================================================
long CShJsonUS::DeleteEachData(long lID)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);

	if(lID == -1)
	{
		if(DeleteJsonTreeData(JSON_KEY_US_EACH_ID) == FALSE)
			goto EXIT;
	}
	else
	{
		::wsprintf(pszKey, JSON_KEY_US_EACH_ID_BASE, lID);
		if(DeleteJsonTreeData(pszKey) == FALSE)
			goto EXIT;
	}

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      MakeUpdateList
//
// parameters
//      dwID                ì«Ç›çûÇﬁID
//      pusul               ÉäÉZÉbÉgÇ∑ÇÈÉÅÉìÉoÇÃÉäÉXÉg(óvÇ∑ÇÈÇ…ì«Ç›çûÇ‹Ç»Ç¢)
//
// return value
//      ïKóvÇ»ÉÅÉÇÉäÇÃÉTÉCÉY
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃIDÇ≈ÉåÉWÉXÉgÉäÇ©ÇÁ
//      ì«Ç›çûÇﬁ
//=============================================================================
long CShJsonUS::MakeUpdateList(short wID, PUSUPDATELIST pusul)
{
	long		lRet = 0;
//
//	short		j = 0;
//	short		wCount = 0;
//	short		wMcfVer = 0;
//
//	BOOL		blSuccess = FALSE;
//
//	DWORD		dwRegVer = 0;
//	DWORD		dwSize = 0;
//
//	WCHAR FAR	*psep = NULL;
//	WCHAR FAR	*pszKey = NULL;
//	WCHAR FAR	*pszData = NULL;
//
//	pszKey = new WCHAR[SCUI_REGKEYSIZE];
//	pszData = new WCHAR[128];
//	if(pszKey == NULL || pszData == NULL)
//		goto EXIT;
//
//	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
//
//	// ================================================
//	// _/_/_/  MCFÉtÉ@ÉCÉãÉNÉâÉXÇÃçÏê¨
//	// ================================================
//	//if(CreateObj(scui_obj_ini_mcf) == FALSE) json temp
//	//	goto EXIT;
//
//	::wsprintf(pszKey, JSON_KEY_US_EACH_ID_UPDATE, wID);
//
//	while(TRUE)
//	{
//		SecureZeroMemory(pszData, 128);
//		dwRegVer = 0;
//
//		// MCFÇ©ÇÁçXêVÇ∑ÇÈÉÅÉìÉoÇéÊìæ
//		if((*m_pmcf).GetString(MCF_SEC_DEFAULTPRINTER_UPDATE, j, "", pszData, 128) == 0)
//		{
//			if(pusul != NULL)
//			{
//				(*pusul).wVer = -1;
//				SecureZeroMemory((*pusul).szEntry, sizeof((*pusul).szEntry));
//			}
//
//			break;
//		}
//
//		//psep = ::strchr(pszData, ',');
//		psep = ::wcschr(pszData, L',');
//		wMcfVer = ::wcstoul(psep + 1, nullptr, 10);
//		::_wcsset_s(psep, 128 - (psep - pszData), L'\0');
//
//		// ÉåÉWÉXÉgÉäÇ©ÇÁÉÅÉìÉoÇÃÉoÅ[ÉWÉáÉìÇéÊìæ
//		blSuccess = ReadJsonDWORDData(pszKey, pszData, dwRegVer);
//		if(blSuccess == FALSE)
//		{
//			dwRegVer = 0;
//		}
//
//		// î‰är
//		if((short)dwRegVer != wMcfVer)
//		{
//			if(pusul != NULL)
//			{
//				(*pusul).wVer = wMcfVer;
//				wcscpy_s((*pusul).szEntry, _countof((*pusul).szEntry), pszData);
//
//				pusul++;
//			}
//
//			wCount++;
//		}
//
//		j++;
//	}
//
//	lRet = sizeof(USUPDATELIST) * (wCount + 1);
//
//EXIT:
//
//	if(pszData != NULL)
//		delete[] pszData;
//
//	if(pszKey != NULL)
//		delete[] pszKey;
//
	return lRet;
}


//=============================================================================
// function
//      SearchUpdateList
//
// parameters
//      dwID                ì«Ç›çûÇﬁID
//      pusul               ÉäÉZÉbÉgÇ∑ÇÈÉÅÉìÉoÇÃÉäÉXÉg(óvÇ∑ÇÈÇ…ì«Ç›çûÇ‹Ç»Ç¢)
//
// return value
//      ë∂ç›Ç∑ÇÈÅFÉÅÉìÉoÇÃçXêVÉoÅ[ÉWÉáÉì
//      ë∂ç›ÇµÇ»Ç¢ÅF-1
//
// outline
//      USUPDATELISTÇÃÉäÉXÉgì‡Ç…éwíËÇÃèÓïÒÇ™ë∂ç›Ç∑ÇÈÇ©Ç«Ç§Ç©ÇéÊìæÇ∑ÇÈ
//=============================================================================
short CShJsonUS::SearchUpdateList(PUSUPDATELIST pusul, WCHAR FAR *pszEntry)
{
	short	wRet = -1;

	if(pusul == NULL)
		goto EXIT;

	if(pszEntry == NULL)
		goto EXIT;

	while(TRUE)
	{
		if((*pusul).wVer == -1)
			break;

		if(sh_strcmp((*pusul).szEntry, pszEntry) == 0)
		{
			wRet = (*pusul).wVer;
			break;
		}

		pusul++;
	}

EXIT:

	return wRet;
}


//=============================================================================
// function
//      WriteUpdateList
//
// parameters
//      dwID                ì«Ç›çûÇﬁID
//      pusul               ÉäÉZÉbÉgÇ∑ÇÈÉÅÉìÉoÇÃÉäÉXÉg(óvÇ∑ÇÈÇ…ì«Ç›çûÇ‹Ç»Ç¢)
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      USUPDATELISTÇÃÉäÉXÉgÇÃèÓïÒÇÉåÉWÉXÉgÉäÇ…îΩâfÇ∑ÇÈ
//=============================================================================
BOOL CShJsonUS::WriteUpdateList(short wID, PUSUPDATELIST pusul)
{
	BOOL		blRet = FALSE;

	DWORD		dwData = 0;

	WCHAR FAR	*pszKey = NULL;

	if(pusul == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	::wsprintf(pszKey, JSON_KEY_US_EACH_ID_UPDATE, wID);

	while(TRUE)
	{
		if((*pusul).wVer == -1)
			break;

		if((*pusul).wVer > 0)
		{
			dwData = (*pusul).wVer;
			WriteJsonDWORDData(pszKey, (*pusul).szEntry, dwData);
		}

		pusul++;
	}

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      RegisterPresetFromFile
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//
// return value
//      ê¨å˜Åiìoò^äÆóπÅj        : TRUE
//      é∏îsÅiÇ‡ÇµÇ≠ÇÕèÓïÒñ≥ÇµÅj: FALSE
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÉtÉ@ÉCÉãÇÉåÉWÉXÉgÉäÇ…ìoò^Ç∑ÇÈ
//=============================================================================
//BOOL CShJsonUS::RegisterPresetFromFile(WCHAR FAR *pszSvrPrnName)
//{
//	BOOL			blRet = FALSE;
//
//	WCHAR			szSrcFName[_MAX_FNAME];
//	WCHAR			szSrcPath[_MAX_PATH];
//	WCHAR			szTempPath[_MAX_PATH];
//	WCHAR			szTempFName[_MAX_PATH];
//	WCHAR			szRegEditPath[_MAX_PATH];
//	WCHAR			szCmdLine[384];
//
//	FILE			*pfSrc = NULL;
//	FILE			*pfDst = NULL;
//	errno_t			errSrc;				// fopen_s ÇÃñﬂÇËílÇì¸ÇÍÇÈ
//	errno_t			errDst;				// fopen_s ÇÃñﬂÇËílÇì¸ÇÍÇÈ
//
//	WCHAR			*p = NULL;
//	WCHAR			*p2 = NULL;
//	WCHAR			*p3 = NULL;
//
//	WCHAR			szSrcData[2048];
//	WCHAR			szDstData[2048];
//	WCHAR			szRegHead[256];
//	WCHAR			szRoot[16];
//
//	SHELLEXECUTEINFO	sei;
//
//
//
//	// ================================================
//	// _/_/_/  ÉvÉäÉZÉbÉgÇÉZÉbÉgÇ∑ÇÈÇ©Ç«Ç§Ç©ÇÃîªíË
//	// ================================================
//	SecureZeroMemory(szSrcFName, sizeof(szSrcFName));
//	(*m_pmcf).GetString(MCF_SEC_USPRESET, MCF_KEY_DEF_USPRESET_FILE, "", szSrcFName, _countof(szSrcFName));
//	if(::lstrlen(szSrcFName) == 0)
//		goto EXIT;
//
//	// ================================================
//	// _/_/_/  å≥ÉtÉ@ÉCÉãÉpÉXÇÃçÏê¨
//	// ================================================
//	SecureZeroMemory(szSrcPath, sizeof(szSrcPath));
//	if(::GetModuleFileName(m_hInst, szSrcPath, sizeof(szSrcPath)) == 0)
//		goto EXIT;
//
//	p = ::wcsrchr(szSrcPath, L'\\');
//	if(p == NULL)
//		p = szSrcPath;
//	else
//		p++;
//
//	::lstrcpy(p, szSrcFName);
//	
//	//(*m_pLog).LogString("source file name", szSrcPath);
//
//	// ================================================
//	// _/_/_/  ÉeÉìÉ|ÉâÉäÉtÉ@ÉCÉãñºÇÃçÏê¨
//	// ================================================
//	SecureZeroMemory(szTempFName, sizeof(szTempFName));
//	SecureZeroMemory(szTempPath, sizeof(szTempPath));
//
//	if(::GetTempPath(sizeof(szTempPath), szTempPath) == 0)
//		goto EXIT;
//
//	if(::GetTempFileName(szTempPath, SCUI_FHEAD_USPRESET_TMP, 0, szTempFName) == 0)
//		goto EXIT;
//
//
//	//(*m_pLog).LogString("temp file name", szTempFName);
//
//	// ================================================
//	// _/_/_/  ÉåÉWÉXÉgÉäìoò^ÉtÉ@ÉCÉãÇÃçÏê¨
//	// ================================================
//	if(IsUnicodeOS())	// unicode
//	{
//		// ================================================
//		// _/_/_/  äeÉtÉ@ÉCÉãÇÃÉIÅ[ÉvÉì
//		// ================================================
//		//errSrc = ::fopen_s(&pfSrc, szSrcPath, "rb");
//		errSrc = ::_wfopen_s(&pfSrc, szSrcPath, L"rb");
//		errDst = ::_wfopen_s(&pfDst, szTempFName, L"wb");
//		
//		if(errSrc != 0 || errDst != 0)
//			goto EXIT;
//
//		// ================================================
//		// _/_/_/  ìoò^ópÉfÅ[É^ÇÃçÏê¨
//		// ================================================
//		SecureZeroMemory(szRegHead, sizeof(szRegHead));
//		//convert_wchar(szRegHead, SCUI_REG_HEAD_USKEY, sizeof(szRegHead));
//	
//		SecureZeroMemory(szRoot, sizeof(szRoot));
//		//convert_wchar(szRoot, SCUI_REG_USKEY_ROOT, sizeof(szRoot));
//
//		SecureZeroMemory(szSrcData, sizeof(szSrcData));
//		while(::fgetws((wchar_t *)szSrcData, sizeof(szSrcData) / sizeof(wchar_t), pfSrc) != NULL)
//		{
//			if(_wcsnicmp((wchar_t *)szRegHead, (wchar_t *)szSrcData, wcslen((wchar_t *)szRegHead)) == 0)
//			{
//				SecureZeroMemory(szDstData, sizeof(szDstData));
//				p = szSrcData + (wcslen((wchar_t *)szRegHead) * sizeof(wchar_t));
//				p2 = (WCHAR *)wcschr((wchar_t*)p, '\\');
//				if(p2 == NULL)
//					p2 = p;
//
//				p3 = (WCHAR *)wcschr((wchar_t *)pszSvrPrnName, '\\');
//				if(p3 == pszSvrPrnName)
//					p3 += (sizeof(wchar_t) * 2);
//				else
//					p3 = pszSvrPrnName;
//
//				wcscpy_s((wchar_t *)szDstData, sizeof(szDstData) / sizeof(wchar_t), (wchar_t *)szRegHead);
//				wcscat_s((wchar_t *)szDstData, sizeof(szDstData) / sizeof(wchar_t), (wchar_t *)p3);
//				wcscat_s((wchar_t *)szDstData, sizeof(szDstData) / sizeof(wchar_t), (wchar_t *)p2);
//
//				::fputws((wchar_t *)szDstData, pfDst);
//			}
//			else
//			{
//				::fputws((wchar_t *)szSrcData, pfDst);
//			}
//
//			SecureZeroMemory(szSrcData, sizeof(szSrcData));
//		}
//	}
//	else
//	{
//		// ================================================
//		// _/_/_/  äeÉtÉ@ÉCÉãÇÃÉIÅ[ÉvÉì
//		// ================================================
//		errSrc = ::_wfopen_s(&pfSrc, szSrcPath, L"rt");
//		errDst = ::_wfopen_s(&pfDst, szTempFName, L"wt");
//
//		if(errSrc != 0 || errDst != 0)
//			goto EXIT;
//
//		// ================================================
//		// _/_/_/  ìoò^ópÉfÅ[É^ÇÃçÏê¨
//		// ================================================
//		SecureZeroMemory(szSrcData, sizeof(szSrcData));
//		//while(::fgets(szSrcData, sizeof(szSrcData) / sizeof(WCHAR), pfSrc) != NULL)
//		while (fgetws(szSrcData, sizeof(szSrcData) / sizeof(WCHAR), pfSrc) != NULL)
//		{
//			if(_wcsnicmp(SCUI_REG_HEAD_USKEY, szSrcData, lstrlen(SCUI_REG_HEAD_USKEY)) == 0)
//			{
//				SecureZeroMemory(szDstData, sizeof(szDstData));
//				p = szSrcData + (lstrlen(SCUI_REG_HEAD_USKEY) * sizeof(WCHAR));
//				p2 = wcsrchr(p, L'\\');
//				if(p2 == NULL)
//					p2 = p;
//
//				p3 = (WCHAR *)wcsrchr(pszSvrPrnName, '\\');
//				if(p3 == pszSvrPrnName)
//					p3 += (sizeof(WCHAR) * 2);
//				else
//					p3 = pszSvrPrnName;
//
//				wcscpy_s(szDstData, _countof(szDstData), SCUI_REG_HEAD_USKEY);
//				wcscpy_s(szDstData, _countof(szDstData), pszSvrPrnName);
//				wcscpy_s(szDstData, _countof(szDstData), p2);
//
//				::fputws(szDstData, pfDst);
//			}
//			else
//			{
//				::fputws(szSrcData, pfDst);
//			}
//
//			SecureZeroMemory(szSrcData, sizeof(szSrcData));
//		}
//	}
//
//	// ================================================
//	// _/_/_/  ÉfÅ[É^ÇÃìoò^
//	// ================================================
//	if(pfDst != NULL)
//	{
//		::fclose(pfDst);
//		pfDst = NULL;
//	}
//
//	SecureZeroMemory(szRegEditPath, sizeof(szRegEditPath));
//	if(::GetWindowsDirectory(szRegEditPath, sizeof(szRegEditPath)) == 0)
//		goto EXIT;
//
//	if(szRegEditPath[::lstrlen(szRegEditPath) -1] != '\\')
//		::lstrcat(szRegEditPath, L"\\");
//
//	::lstrcat(szRegEditPath, SCUI_FNAME_REGEDIT);
//
//	SecureZeroMemory(szCmdLine, sizeof(szCmdLine));
//	::wsprintf(szCmdLine, SCUI_REGEDIT_CMDLINE, szTempFName);
//
//	memset(&sei, 0x00, sizeof(sei));
//	sei.cbSize = sizeof(sei);
//	sei.lpVerb = "open";
//	sei.lpFile = szRegEditPath;
//	sei.lpParameters = szCmdLine;
//	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
//	sei.nShow = SW_HIDE;
//	ShellExecuteEx(&sei);
//	WaitForSingleObject( sei.hProcess, 60000 ) ;	
//
//	::CloseHandle(sei.hProcess);
//
//	blRet = TRUE;
//
//EXIT:
//
//
//	if(pfDst != NULL)
//		::fclose(pfDst);
//
//	::DeleteFile(szTempFName);
//
//	if(pfSrc != NULL)
//		::fclose(pfSrc);
//
//	if(m_pLog != NULL)
//	{
//		(*m_pLog).Log("return value", (long)blRet);
//		(*m_pLog).LogEndFunc("CShRegUS::RegisterPresetFromFile");
//	}
//
//	return blRet;
//}

//=============================================================================
// function
//      ReadDateTime
//
// parameters
//      pszDateTime         YYYYMMDDHHmmSS å`éÆÇÃì˙éûï∂éöóÒÇäiî[Ç∑ÇÈÇΩÇﬂÇÃÉoÉbÉtÉ@Å[
//      dwBufSize           ÉoÉbÉtÉ@Å[ÉTÉCÉY
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      User Setting 0ÇÃÉxÅ[ÉXÇ∆Ç»Ç¡ÇΩÉtÉ@ÉCÉãÇÃçXêVéûçèÇì«Ç›çûÇﬁ
//=============================================================================
long CShJsonUS::ReadDateTime(WCHAR FAR *pszDateTime, DWORD dwBufSize)
{
	BOOL			blRet = FALSE;

	BOOL			blSuccess = FALSE;
	DWORD			dwSize;

	// ================================================
	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
	// ================================================
	if(pszDateTime == NULL)
		goto EXIT;

	dwSize = dwBufSize;
	blSuccess = ReadJsonStrData(JSON_KEY_US_ROOT_BASE,JSON_ENT_US_MCF_TIMESTAMP,pszDateTime, dwSize);
	if(blSuccess == FALSE)
		goto EXIT;

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      WriteDateTime
//
// parameters
//      pszDateTime         YYYYMMDDHHmmSS å`éÆÇÃì˙éûï∂éöóÒ
//      dwBufSize           ÉoÉbÉtÉ@Å[ÉTÉCÉY
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      User Setting 0ÇÃÉxÅ[ÉXÇ∆Ç»Ç¡ÇΩÉtÉ@ÉCÉãÇÃçXêVéûçèÇèëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::WriteDateTime(WCHAR FAR *pszDateTime, DWORD dwBufSize)
{
	BOOL			blRet = FALSE;
	short			wBy = 1;

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
	if(IsUnicodeOS())
		wBy = 2;

	// ================================================
	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
	// ================================================
	if(pszDateTime == NULL)
		goto EXIT;

	// --- time stamp ãLò^
	if(WriteJsonStrData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_MCF_TIMESTAMP, pszDateTime, (DWORD)wcslen(pszDateTime) * wBy) == FALSE)
		goto EXIT;

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      ReadLCID
//
// parameters
//      dwID                ÉåÉWÉXÉgÉäÇ©ÇÁì«Ç›çûÇÒÇæIDèÓïÒ
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      User Setting 0ÇÃÉxÅ[ÉXÇ∆Ç»Ç¡ÇΩLCIDÇèëÇ´çûÇﬁ
//=============================================================================
BOOL CShJsonUS::ReadLCID(DWORD *dwID)
{
	BOOL			blRet = FALSE;

	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	// ================================================
	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
	// ================================================
	if(dwID == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
	blSuccess = ReadJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_SELECT_LCID, dwData);
	if(blSuccess == FALSE)
		goto EXIT;

	*dwID = dwData;
	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      WriteLCID
//
// parameters
//      dwID                ÉåÉWÉXÉgÉäÇ…èëÇ´çûÇﬁLCIDèÓïÒ
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      User Setting 0ÇÃÉxÅ[ÉXÇ∆Ç»Ç¡ÇΩLCIDÇèëÇ´çûÇﬁ
//=============================================================================
BOOL CShJsonUS::WriteLCID(DWORD dwID)
{
	BOOL			blRet = FALSE;

	blRet = WriteJsonDWORDData(JSON_KEY_US_ROOT_BASE, JSON_ENT_US_SELECT_LCID, dwID);

	return blRet;
}

//=============================================================================
// function
//      WriteDataJson
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::WriteDataJson(long lIndex)
{
	long			lRet = -1;

	//scui_obj_cls_regups
	lRet = WriteDataJsonUPS((long)lIndex);
	
	// scui_obj_cls_regms
	lRet = WriteDataJsonMS((long)lIndex);

	lRet = WriteDataJsonPP((long)lIndex);

	//lRet = WriteDataJsonTR((long)lIndex);

	//lRet = WriteDataJsonCPM((long)lIndex);

	lRet = WriteDataJsonSTRD((long)lIndex);

	lRet = WriteDataJsonUnit((long)lIndex);

	return lRet;
}

//=============================================================================
// function
//      WriteDataJsonMS
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::WriteDataJsonMS(long lIndex)
{
	long			lRet = -1;

//	CShJsonMS		*pjsonms = NULL;//soup
//	DWORD			dwCount;
//	BOOL			blRet;
//	WCHAR FAR		*pszKey = NULL;
//
//	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
//	//	goto EXIT;
//
//	pjsonms = new CShJsonMS(m_hInst, m_pszSvrPrnName);
//	if(pjsonms == NULL)
//		goto EXIT;
//
//	pjsonms->SetParent(this);
//
//	dwCount = (*pjsonms).ReadMSCount();
//	if(dwCount == 0)
//		goto EXIT;
//
//	if(lIndex == 0){
//		blRet = (*pjsonms).resetExToUs(lIndex);
//	}
//	else{
//		pszKey = new WCHAR[SCUI_REGKEYSIZE];
//		if(pszKey == NULL)
//			goto EXIT;
//
//		SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
//
//		::wsprintf(pszKey, JSON_KEY_MS_ROOT_BASE_US, lIndex);
//		CopyJsonTreeNode(pszKey,JSON_KEY_MS_ROOT_BASE);
//	}
//
//EXIT:
//	if(pjsonms != NULL)
//		delete pjsonms;
//
//	if(pszKey != NULL)
//		delete[] pszKey;

	return lRet;
}

//=============================================================================
// function
//      WriteDataJsonUPS
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::WriteDataJsonUPS(long lIndex)
{
	long			lRet = -1;

//	CShJsonUserPSize	*pjsonups = NULL;//soup
//	DWORD			dwCount;
//	DWORD			dwCountMcf;
//	WCHAR FAR		*pszKey = NULL;
//		
//	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
//	//	goto EXIT;
//
//	dwCountMcf = (DWORD)(*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_CUSTOM_PAPERSIZE);
//	if(dwCountMcf == 0)
//		goto EXIT;
//
//	pjsonups = new CShJsonUserPSize(m_hInst, m_pszSvrPrnName);
//	if(pjsonups == NULL)
//		goto EXIT;
//
//	pjsonups->SetParent(this);
//
//	if(lIndex == 0){
//		(*pjsonups).resetUs(lIndex);
//		//add to fix Custom paper default values when opened from json
//		dwCount = (*pjsonups).ReadUPSCount();
//		if(dwCount == 0)
//		{
//			(*pjsonups).reset();
//			dwCount = (*pjsonups).ReadUPSCount();
//		}
//	}
//	else{
//		dwCount = (*pjsonups).ReadUPSCount();
//		if(dwCount == 0)
//		{
//			(*pjsonups).reset();
//			dwCount = (*pjsonups).ReadUPSCount();
//		}
//
//		pszKey = new WCHAR[SCUI_REGKEYSIZE];
//		if(pszKey == NULL)
//			goto EXIT;
//
//		SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
//
//		::wsprintf(pszKey, JSON_KEY_UPS_ROOT_BASE_US, lIndex);
//		CopyJsonTreeNode(pszKey,JSON_KEY_UPS_ROOT_BASE);
//	}
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjsonups != NULL)
//		delete pjsonups;
//	if(pszKey != NULL)
//		delete[] pszKey;
	return lRet;
}

//=============================================================================
// function
//      WriteDataJsonPP
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::WriteDataJsonPP(long lIndex)
{
	long			lRet = -1;
//
//	CShJsonPP		*pjsonpp = NULL;//soup
//	DWORD			dwCount;
//	BOOL			blRet;
//	WCHAR FAR		*pszKey = NULL;
//
//	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
//	//	goto EXIT;
//
//	pjsonpp = new CShJsonPP(m_hInst, m_pszSvrPrnName);
//	if(pjsonpp == NULL)
//		goto EXIT;
//
//	pjsonpp->SetParent(this);
//
//	dwCount = (*pjsonpp).ReadPPCount();
//	if(dwCount == 0)
//		goto EXIT;
//
//	if(lIndex == 0){
//		blRet = (*pjsonpp).resetToUs(lIndex);
//	}
//	else{
//		pszKey = new WCHAR[SCUI_REGKEYSIZE];
//		if(pszKey == NULL)
//			goto EXIT;
//
//		SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
//
//		::wsprintf(pszKey, JSON_KEY_PP_ROOT_BASE_US, lIndex);
//		CopyJsonTreeNode(pszKey,JSON_KEY_PP_ROOT_BASE);
//	}
//	lRet = lIndex;
//
//EXIT:
//	if(pjsonpp != NULL)
//		delete pjsonpp;
//	if(pszKey != NULL)
//		delete[] pszKey;
	return lRet;
}

//=============================================================================
// function
//      WriteDataJsonTR
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
#if 0
long CShJsonUS::WriteDataJsonTR(long lIndex)
{
	long			lRet = -1;

//
	DWORD			dwCount;
	BOOL			blRet;
	WCHAR FAR		*pszKey = NULL;
		
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/

	pjsontr = new CShJsonTR(m_hInst, m_pszSvrPrnName, m_pLog);
	if(pjsontr == NULL)
		goto EXIT;

	pjsontr->SetParent(this);

	dwCount = (*pjsontr).ReadTRCount();
	if(dwCount == 0)
		goto EXIT;

	if(lIndex == 0){
		blRet = (*pjsontr).resetToUs(lIndex);
	}
	else{
		pszKey = new WCHAR[SCUI_REGKEYSIZE];
		if(pszKey == NULL)
			goto EXIT;

		SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);

		::wsprintf(pszKey, JSON_KEY_TR_ROOT_BASE_US, lIndex);
		CopyJsonTreeNode(pszKey,JSON_KEY_TR_ROOT_BASE);
	}
	lRet = lIndex;

EXIT:
	if(pjsontr != NULL)
		delete pjsontr;
	if(pszKey != NULL)
		delete[] pszKey;
	return lRet;
}
#endif
//=============================================================================
// function
//      WriteDataJsonCPM
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
#if 0
long CShJsonUS::WriteDataJsonCPM(long lIndex)
{
	long			lRet = -1;

//
	DWORD			dwCount;
	WCHAR FAR		*pszKey = NULL;
		
	pjsoncpm = new CShJsonCPM(m_hInst, m_pszSvrPrnName, m_pLog);
	if(pjsoncpm == NULL)
		goto EXIT;

	pjsoncpm->SetParent(this);

	dwCount = (*pjsoncpm).ReadCPMCount();
	if(dwCount == 0)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);

	::wsprintf(pszKey, JSON_KEY_CPM_ROOT_BASE_US, lIndex);
	CopyJsonTreeNode(pszKey,JSON_KEY_CPM_ROOT_BASE);

	lRet = lIndex;

EXIT:
	if(pjsoncpm != NULL)
		delete pjsoncpm;
	if(pszKey != NULL)
		delete[] pszKey;
	return lRet;
}
#endif

//=============================================================================
// function
//      WriteDataJsonUnit
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      read from tab_shift, write to user setting 
//=============================================================================
long CShJsonUS::WriteDataJsonUnit(long lIndex)
{
	long			lRet = -1;
	WCHAR FAR		*pszKey = NULL;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);

	::wsprintf(pszKey, JSON_KEY_TSF_ROOT_BASE_US, lIndex);
	CopyJsonTreeNode(pszKey,JSON_KEY_TSF_ROOT_BASE);

	lRet = lIndex;

EXIT:
	
	if(pszKey != NULL)
		delete[] pszKey;
	return lRet;
}

//=============================================================================
// function
//      WriteDataJsonSTRD
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      read from job_control\stored_to, write to user setting 
//=============================================================================
long CShJsonUS::WriteDataJsonSTRD(long lIndex)
{
	long			lRet = -1;
//	DWORD			dwFoldIndex = 0;//soup
//	CShJsonStored		*pjson = NULL;
//	WCHAR			szSelFolderName[512];
//	DWORD			dwFolderSize = sizeof(szSelFolderName);
//	::ZeroMemory(szSelFolderName, dwFolderSize);	
//
//	pjson = new CShJsonStored(m_hInst, m_pszSvrPrnName);
//	if(pjson == NULL)
//		goto EXIT;
//
//	(*pjson).SetParent(this);
//
//	(*pjson).ReadSTDData(szSelFolderName, dwFolderSize);
//	dwFoldIndex = (*pjson).ReadLastSelectedFolderIndex();
//	if(szSelFolderName[0] != '\0')
//	{
//		(*pjson).WriteSTDDataToUs(lIndex, szSelFolderName, dwFolderSize,dwFoldIndex);
//		lRet = lIndex;
//	}
//
//EXIT:
//	if(pjson != NULL)
//		delete pjson;

	return lRet;
}

//=============================================================================
// function
//      ReadDataJson
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::ReadDataJson(long lIndex)
{
	long			lRet = -1;

	// scui_obj_cls_regups
	lRet = ReadDataJsonUPS((long)lIndex);
	
	// scui_obj_cls_regms
	lRet = ReadDataJsonMS((long)lIndex);

	// scui_obj_cls_regpp
	lRet = ReadDataJsonPP((long)lIndex);

	// scui_obj_cls_regpp
	//lRet = ReadDataJsonTR((long)lIndex);

	//lRet = ReadDataJsonCPM((long)lIndex);

	lRet = ReadDataJsonSTRD((long)lIndex);

	lRet = ReadDataJsonUnit((long)lIndex);

	return lRet;
}

//=============================================================================
// function
//      ReadDataJsonMS
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::ReadDataJsonMS(long lIndex)
{
	long			lRet = -1;

//	CShJsonMS		*pjsonms = NULL;//soup
//	DWORD			dwCount;
//	WCHAR FAR		*pszKey = NULL;
//
//	//(CreateObj(scui_obj_ini_mcf) == FALSE)
//	//	goto EXIT;
//
//	pjsonms = new CShJsonMS(m_hInst, m_pszSvrPrnName);
//	if(pjsonms == NULL)
//		goto EXIT;
//
//	pjsonms->SetParent(this);
//	
//	dwCount = (*pjsonms).ReadMSCount(JSON_KEY_MS_ROOT_BASE_US, lIndex);
//	if(dwCount == 0)
//	{
//		(*pjsonms).resetEx();
//		lRet = lIndex;
//		goto EXIT;
//	}
//
//	pszKey = new WCHAR[SCUI_REGKEYSIZE];
//	if(pszKey == NULL)
//		goto EXIT;
//
//	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
//
//	::wsprintf(pszKey, JSON_KEY_MS_ROOT_BASE_US, lIndex);
//	CopyJsonTreeNode(JSON_KEY_MS_ROOT_BASE,pszKey);
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjsonms != NULL)
//		delete pjsonms;
//	if(pszKey != NULL)
//		delete[] pszKey;

	return lRet;
}

//=============================================================================
// function
//      ReadDataJsonUPS
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::ReadDataJsonUPS(long lIndex)
{
	long			lRet = -1;

//	short			wBy = 1;
//	WCHAR FAR		*pszKey = NULL;//soup
//
//	CShJsonUserPSize	*pjsonups = NULL;
//	DWORD			dwCount;
//	DWORD			dwCountMcf;
//		
//	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
//		goto EXIT;*/
//
//	dwCountMcf = (DWORD)(*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_CUSTOM_PAPERSIZE);
//	if(dwCountMcf == 0)
//		goto EXIT;
//
//	pjsonups = new CShJsonUserPSize(m_hInst, m_pszSvrPrnName);
//	if(pjsonups == NULL)
//		goto EXIT;
//
//	pjsonups->SetParent(this);
//
//	dwCount = (*pjsonups).ReadUPSCount(JSON_KEY_UPS_ROOT_BASE_US, lIndex);
//	if(dwCount == 0)
//		goto EXIT;
//		
//	pszKey = new WCHAR[SCUI_REGKEYSIZE];
//	if(pszKey == NULL)
//		goto EXIT;
//
//	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
//
//	::wsprintf(pszKey, JSON_KEY_UPS_ROOT_BASE_US, lIndex);
//	CopyJsonTreeNode(JSON_KEY_UPS_ROOT_BASE,pszKey);
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjsonups != NULL)
//		delete pjsonups;
//	if(pszKey != NULL)
//		delete[] pszKey;
	return lRet;
}

//=============================================================================
// function
//      ReadDataJsonPP
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::ReadDataJsonPP(long lIndex)
{
	long			lRet = -1;

//	CShJsonPP		*pjsonpp = NULL;//soup
//	DWORD			dwCount;
//	WCHAR FAR		*pszKey = NULL;
//		
//	////if(CreateObj(scui_obj_ini_mcf) == FALSE)
//	//	goto EXIT;
//
//	pjsonpp = new CShJsonPP(m_hInst, m_pszSvrPrnName);
//	if(pjsonpp == NULL)
//		goto EXIT;
//
//	pjsonpp->SetParent(this);
//	
//	dwCount = (*pjsonpp).ReadPPCount(JSON_KEY_PP_ROOT_BASE_US, lIndex);
//	if(dwCount == 0)
//		goto EXIT;
//
//	pszKey = new WCHAR[SCUI_REGKEYSIZE];
//	if(pszKey == NULL)
//		goto EXIT;
//
//	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
//
//	::wsprintf(pszKey, JSON_KEY_PP_ROOT_BASE_US, lIndex);
//	CopyJsonTreeNode(JSON_KEY_PP_ROOT_BASE,pszKey);
//	
//	lRet = lIndex;
//
//EXIT:
//	if(pjsonpp != NULL)
//		delete pjsonpp;
//	if(pszKey != NULL)
//		delete[] pszKey;
	return lRet;
}

//=============================================================================
// function
//      ReadDataJsonTR
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
//long MY_CLASSNAME::ReadDataJsonTR(long lIndex)
//{
//	long			lRet = -1;
//
//	//CShJsonTR		*pjsontr = NULL;
//	DWORD			dwCount;
//	char FAR		*pszKey = NULL;
//
//	if(CreateObj(scui_obj_ini_mcf) == FALSE)
//		goto EXIT;
//
//	pjsontr = new CShJsonTR(m_hInst, m_pszSvrPrnName, m_pLog);
//	if(pjsontr == NULL)
//		goto EXIT;
//
//	pjsontr->SetParent(this);
//	
//	dwCount = (*pjsontr).ReadTRCount(JSON_KEY_TR_ROOT_BASE_US, lIndex);
//	if(dwCount == 0)
//		goto EXIT;
//
//	pszKey = new char[SCUI_REGKEYSIZE];
//	if(pszKey == NULL)
//		goto EXIT;
//
//	memzero(pszKey, SCUI_REGKEYSIZE);
//
//	::wsprintf(pszKey, JSON_KEY_TR_ROOT_BASE_US, lIndex);
//	CopyJsonTreeNode(JSON_KEY_TR_ROOT_BASE,pszKey);
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjsontr != NULL)
//		delete pjsontr;
//	if(pszKey != NULL)
//		delete[] pszKey;
//	return lRet;
//}

//=============================================================================
// function
//      ReadDataJsonCPM
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
//long MY_CLASSNAME::ReadDataJsonCPM(long lIndex)
//{
//	long			lRet = -1;
//
//	//CShJsonCPM		*pjsoncpm = NULL;
//	DWORD			dwCount;
//	char FAR		*pszKey = NULL;
//
//	pjsoncpm = new CShJsonCPM(m_hInst, m_pszSvrPrnName, m_pLog);
//	if(pjsoncpm == NULL)
//		goto EXIT;
//
//	pjsoncpm->SetParent(this);
//	
//	dwCount = (*pjsoncpm).ReadCPMCount(JSON_KEY_CPM_ROOT_BASE_US, lIndex);
//	if(dwCount == 0)
//		goto EXIT;
//
//	pszKey = new char[SCUI_REGKEYSIZE];
//	if(pszKey == NULL)
//		goto EXIT;
//
//	memzero(pszKey, SCUI_REGKEYSIZE);
//
//	::wsprintf(pszKey, JSON_KEY_CPM_ROOT_BASE_US, lIndex);
//	CopyJsonTreeNode(JSON_KEY_CPM_ROOT_BASE,pszKey);
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjsoncpm != NULL)
//		delete pjsoncpm;
//	if(pszKey != NULL)
//		delete[] pszKey;
//	return lRet;
//}

//=============================================================================
// function
//      ReadDataJsonUnit
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::ReadDataJsonUnit(long lIndex)
{
	long			lRet = -1;
	WCHAR FAR		*pszKey = NULL;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);

	::wsprintf(pszKey, JSON_KEY_TSF_ROOT_BASE_US, lIndex);
	CopyJsonTreeNode(JSON_KEY_TSF_ROOT_BASE,pszKey);

	lRet = lIndex;

EXIT:
	if(pszKey != NULL)
		delete[] pszKey;
	return lRet;
}

//=============================================================================
// function
//      ReadDataJsonSTRD
//
// parameters
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//      éwíËÇ≥ÇÍÇΩUserSettingèÓïÒÇéwíËÇÃÉCÉìÉfÉbÉNÉXÇ≈ÉåÉWÉXÉgÉäÇ…
//      èëÇ´çûÇﬁ
//=============================================================================
long CShJsonUS::ReadDataJsonSTRD(long lIndex)
{
	long			lRet = -1;
//	BOOL			blRet = FALSE;//soup
//	DWORD			dwFoldIndex = 0;
//	CShJsonStored		*pjson = NULL;
//	WCHAR			szSelFolderName[512];
//	DWORD			dwFolderSize = sizeof(szSelFolderName);
//	::ZeroMemory(szSelFolderName,dwFolderSize);	
//
//	pjson = new CShJsonStored(m_hInst, m_pszSvrPrnName);
//	if(pjson == NULL)
//		goto EXIT;
//
//	(*pjson).SetParent(this);
//
//	blRet = (*pjson).ReadSTDDataToUs(lIndex, szSelFolderName, dwFolderSize,dwFoldIndex);
//	if(!blRet)
//		goto EXIT;
//
//	if(szSelFolderName[0] != '\0')
//	{	
//		blRet = (*pjson).WriteSTDData(szSelFolderName, dwFolderSize);
//		blRet = (*pjson).WriteLastSelectedFolderIndex(dwFoldIndex);
//		if(!blRet)
//			goto EXIT;
//	}
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjson != NULL)
//		delete pjson;

	return lRet;
}

// <S><A> 2022.04.13 Jelly for cr5 function2
//=============================================================================
// function
//      WriteShareDayTimeFunc
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//     write share_daytime_func_user_setting to JSON and HKLM: eg: 2,20210811103941,user_setting
//=============================================================================
BOOL CShJsonUS::WriteShareDayTimeFunc(WCHAR* pszSvrPrnName,WCHAR* pszDayTime, BOOL bValid)
{
	BOOL	blRet = FALSE;

	blRet = CShJson::WriteShareDayTimeFunc(pszSvrPrnName,JSON_KEY_US_ROOT_BASE,pszDayTime,REG_ENT_SHARE_US,bValid);

	return blRet;
}


//=============================================================================
// function
//      WriteShareDayTimeFuncToJSON
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      pszText				the total string: eg: 2,20210811103941,user_setting
//		dwSize				the size of pszText
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//     write share_daytime_func_user_setting to JSON
//=============================================================================
BOOL CShJsonUS::WriteShareDayTimeFuncToJSON(WCHAR* pszSvrPrnName, WCHAR* pszText, DWORD dwSize)
{
	BOOL		blRet = FALSE;

	blRet = CShJson::WriteShareDayTimeFuncToJSON(pszSvrPrnName,JSON_KEY_US_ROOT_BASE, REG_ENT_SHARE_US,pszText, dwSize);

	return blRet;
}

//=============================================================================
// function
//      ReadShareDayTimeFuncFromJSON
//
// parameters
//      pszSvrPrnName      ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//		pszText            the value
//		dwTextSize		   the size of pszText
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//     Read share_daytime_func_user_setting from JSON: eg: 2,20210811103941,user_setting
//=============================================================================
BOOL CShJsonUS::ReadShareDayTimeFuncFromJSON(WCHAR* pszSvrPrnName,WCHAR* pszText,DWORD dwTextSize)
{
	BOOL		blRet = FALSE;

	blRet = CShJson::ReadShareDayTimeFuncFromJSON(pszSvrPrnName,JSON_KEY_US_ROOT_BASE,REG_ENT_SHARE_US,pszText,dwTextSize);

	return blRet;
}

//=============================================================================
// function
//      ReadShareDayTimeFuncFromHKLM
//
// parameters
//      pszSvrPrnName      ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//		pszText            the value
//		dwTextSize		   the size of pszText
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//     Read share_daytime_func_user_setting from HKLM: eg: 2,20210811103941,user_setting
//=============================================================================
BOOL CShJsonUS::ReadShareDayTimeFuncFromHKLM(WCHAR  *pszSvrPrnName, WCHAR  *pszText,DWORD dwTextSize)
{
	BOOL		blRet = FALSE;

	blRet = CShJson::ReadShareDayTimeFuncFromHKLM(pszSvrPrnName,REG_ENT_SHARE_US,pszText,dwTextSize);

	return blRet;
}



//=============================================================================
// function
//      WriteShareData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      pShareFav           pShareFav data
//      dwSize				the size of pShareFav
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      Write the pShareFav to HKLM's PrinterDriverData
//=============================================================================
BOOL CShJsonUS::WriteShareData(WCHAR  *pszSvrPrnName, BYTE* pShareFav, DWORD dwSize)
{
	BOOL		bRet = FALSE;

	bRet = CShJson::WriteShareData(pszSvrPrnName, REG_ENT_SHARE_US, pShareFav, dwSize);

	return bRet;
}

//=============================================================================
// function
//      ReadShareData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      pShareFav           read to the pShareFav
//
// return value
//      ê¨å˜ : buffer size
//      é∏îs : 0
//
// outline
//      Read the share fav data from HKLM
//=============================================================================
DWORD CShJsonUS::ReadShareData(WCHAR  *pszSvrPrnName, BYTE* pShareFav)
{
	DWORD		dwRet = 0;

	dwRet = CShJson::ReadShareData(pszSvrPrnName,REG_ENT_SHARE_US,pShareFav);

	return dwRet;
}

//=============================================================================
// function
//      ReadFavDataFromJSON
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              fav list index
//      pShareFav           the buffer to save the fav data
//
// return value
//      ê¨å˜ : fav data buffer size
//      é∏îs : 0
//
// outline
//      Read the fav data specified by lIndex
//=============================================================================
DWORD CShJsonUS::ReadFavDataFromJSON(WCHAR  *pszSvrPrnName, BYTE* pShareFavData,long lIndex)
{
	DWORD			dwRet = 0;	
	BYTE*			pShareFavDataTmp = NULL;
	PShareFavHeader	pShareFavHeader = NULL;
	PSCDM			pscdm = NULL;
	WCHAR*			pChapterInsert = NULL;
	BYTE*			pMSData = NULL;
	BYTE*			pUPSData = NULL;
	BYTE*			pPPData = NULL;
	BYTE*			pTRData = NULL;
	BYTE*			pCPMData = NULL;
	// <S><A> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
	BYTE*			pSTRData = NULL;
	BYTE*			pSTRJCData = NULL;
	// <E> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
	BYTE*			pTabShiftData = NULL;
	WCHAR*			pTitle = NULL;

	pscdm = new SCDM;
	if(pscdm == NULL)
		goto EXIT;
	SecureZeroMemory(pscdm, sizeof(SCDM)); 

	pShareFavHeader = new ShareFavHeader;
	if(pShareFavHeader == NULL)
		goto EXIT;
	SecureZeroMemory(pShareFavHeader, sizeof(ShareFavHeader));

	if(pShareFavData == NULL)
	{
		//calculate the fav size

		//scdm
		pShareFavHeader->dwSize = sizeof(ShareFavHeader);
		pShareFavHeader->dwScdmSize = sizeof(SCDM);
		pShareFavHeader->dwSize += pShareFavHeader->dwScdmSize;

		pShareFavHeader->dwChapterInsertSize = (SCUI_CHPINSPAGE_BUFSIZE + 1) * 2;
		pShareFavHeader->dwSize += pShareFavHeader->dwChapterInsertSize;

		//margin shift
		pShareFavHeader->dwMarginShiftSize = ReadUSMSData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwMarginShiftSize > 0)
		{
			pShareFavHeader->dwSize += pShareFavHeader->dwMarginShiftSize;
		}	

		//user paper size
		pShareFavHeader->dwUserPaperSize = ReadUSUPSData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwUserPaperSize > 0)
		{
			pShareFavHeader->dwSize += pShareFavHeader->dwUserPaperSize;
		}	

		//print position
		pShareFavHeader->dwPrintPositionSize = ReadUSPPData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwPrintPositionSize > 0)
		{
			pShareFavHeader->dwSize += pShareFavHeader->dwPrintPositionSize;
		}	

		//trimming
		//pShareFavHeader->dwTrimmingSize = ReadUSTRData(pszSvrPrnName,lIndex,NULL);
		//if(pShareFavHeader->dwTrimmingSize > 0)
		//{
		//	pShareFavHeader->dwSize += pShareFavHeader->dwTrimmingSize;
		//}	

		//custom pamphlet
		//pShareFavHeader->dwCPMSize = ReadUSCPMData(pszSvrPrnName,lIndex,NULL);
		//if(pShareFavHeader->dwCPMSize > 0)
		//{
		//	pShareFavHeader->dwSize += pShareFavHeader->dwCPMSize;
		//}	

		////tab shift
		//pShareFavHeader->dwTabShiftSize = ReadUSTSData(pszSvrPrnName,lIndex,NULL);
		//if(pShareFavHeader->dwTabShiftSize > 0)
		//{
		//	pShareFavHeader->dwSize += pShareFavHeader->dwTabShiftSize;
		//}	
		
		// <S><A> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
		//storedTo
		//To support custom folder sharing
		pShareFavHeader->dwStoredToSize = ReadUSSTRData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwStoredToSize > 0)
		{
			pShareFavHeader->dwSize += pShareFavHeader->dwStoredToSize;
		}
		
		//storedtojc
		pShareFavHeader->dwStoredToPrintUISize = ReadStoredToData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwStoredToPrintUISize > 0)
		{
			pShareFavHeader->dwSize += pShareFavHeader->dwStoredToPrintUISize;
		}
		// <E> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
		dwRet = pShareFavHeader->dwSize;
		goto EXIT;
	}
	else
	{
		pShareFavHeader->dwSize = sizeof(ShareFavHeader);
		pShareFavHeader->dwID = ReadID(lIndex);
		pShareFavHeader->dwFavField = DM_FAV_SCDM;
		pShareFavHeader->dwVersion = SHAREFAV_VER;
		pShareFavHeader->dwOffset = sizeof(ShareFavHeader);
		pShareFavHeader->dwScdmSize = sizeof(SCDM);
	
		//read the pscdm from JSON
		if(ReadData(pscdm, lIndex) == 0)
			goto EXIT;

		pShareFavHeader->dwSize += pShareFavHeader->dwScdmSize;

		//chapter insert
		pShareFavHeader->dwChapterInsertSize = (SCUI_CHPINSPAGE_BUFSIZE + 1) * 2;
		pChapterInsert = new WCHAR[pShareFavHeader->dwChapterInsertSize];
		ReadChapterInsPage(lIndex, pChapterInsert, (SCUI_CHPINSPAGE_BUFSIZE + 1) * 2);
		pShareFavHeader->dwFavField |= DM_FAV_CHAPTERINSERT;
		pShareFavHeader->dwSize += pShareFavHeader->dwChapterInsertSize;

		//margin shift
		pShareFavHeader->dwMarginShiftSize = ReadUSMSData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwMarginShiftSize > 0)
		{
			pMSData = new BYTE[pShareFavHeader->dwMarginShiftSize];
			ReadUSMSData(pszSvrPrnName,lIndex,pMSData);

			pShareFavHeader->dwFavField |= DM_FAV_MARGINSHIFT;
			pShareFavHeader->dwSize += pShareFavHeader->dwMarginShiftSize;
		}	

		//user paper size
		pShareFavHeader->dwUserPaperSize = ReadUSUPSData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwUserPaperSize > 0)
		{
			pUPSData = new BYTE[pShareFavHeader->dwUserPaperSize];
			ReadUSUPSData(pszSvrPrnName,lIndex,pUPSData);

			pShareFavHeader->dwFavField |= DM_FAV_USERPAPERSIZE;
			pShareFavHeader->dwSize += pShareFavHeader->dwUserPaperSize;
		}

		//print position
		pShareFavHeader->dwPrintPositionSize = ReadUSPPData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwPrintPositionSize > 0)
		{
			pPPData = new BYTE[pShareFavHeader->dwPrintPositionSize];
			ReadUSPPData(pszSvrPrnName,lIndex,pPPData);

			pShareFavHeader->dwFavField |= DM_FAV_PRINTPOSITION;
			pShareFavHeader->dwSize += pShareFavHeader->dwPrintPositionSize;
		}	

		////trimming
		//pShareFavHeader->dwTrimmingSize = ReadUSTRData(pszSvrPrnName,lIndex,NULL);
		//if(pShareFavHeader->dwTrimmingSize > 0)
		//{
		//	pTRData = new BYTE[pShareFavHeader->dwTrimmingSize];
		//	ReadUSTRData(pszSvrPrnName,lIndex,pTRData);

		//	pShareFavHeader->dwFavField |= DM_FAV_TRIMMING;
		//	pShareFavHeader->dwSize += pShareFavHeader->dwTrimmingSize;
		//}

		////custom pamphlet
		//pShareFavHeader->dwCPMSize = ReadUSCPMData(pszSvrPrnName,lIndex,NULL);
		//if(pShareFavHeader->dwCPMSize > 0)
		//{
		//	pCPMData = new BYTE[pShareFavHeader->dwCPMSize];
		//	ReadUSCPMData(pszSvrPrnName,lIndex,pCPMData);

		//	pShareFavHeader->dwFavField |= DM_FAV_CUSPAMPHLET;
		//	pShareFavHeader->dwSize += pShareFavHeader->dwCPMSize;
		//}

		//tab shift
/*		pShareFavHeader->dwTabShiftSize = ReadUSTSData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwTabShiftSize > 0)
		{
			pTabShiftData = new BYTE[pShareFavHeader->dwTabShiftSize];
			ReadUSTSData(pszSvrPrnName,lIndex,pTabShiftData);
			pShareFavHeader->dwFavField |= DM_FAV_TABSHIFT;
			pShareFavHeader->dwSize += pShareFavHeader->dwTabShiftSize;
		}*/	
		
		
		// <S><A> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
		//storedTo
		//To support custom folder sharing
		pShareFavHeader->dwStoredToSize = ReadUSSTRData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwStoredToSize > 0)
		{
			pSTRData = new BYTE[pShareFavHeader->dwStoredToSize];
			ReadUSSTRData(pszSvrPrnName,lIndex,pSTRData);

			pShareFavHeader->dwFavField |= DM_FAV_STOREDTO;
			pShareFavHeader->dwSize += pShareFavHeader->dwStoredToSize;
		}
		
		//storedtojc
		pShareFavHeader->dwStoredToPrintUISize = ReadStoredToData(pszSvrPrnName,lIndex,NULL);
		if(pShareFavHeader->dwStoredToPrintUISize > 0)
		{
			//pShareFavHeader->dwSize += pShareFavHeader->dwStoredToPrintUISize;
			pSTRJCData = new BYTE[pShareFavHeader->dwStoredToPrintUISize];
			ReadStoredToData(pszSvrPrnName,lIndex,pSTRJCData);

			pShareFavHeader->dwFavField |= DM_FAV_STOREDTO_PRT_UI;
			pShareFavHeader->dwSize += pShareFavHeader->dwStoredToPrintUISize;
		}
		// <E> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S

		pShareFavDataTmp = pShareFavData;
		memcpy(pShareFavDataTmp, pShareFavHeader, sizeof(ShareFavHeader));
		pShareFavDataTmp += sizeof(ShareFavHeader);

		if(pShareFavHeader->dwFavField & DM_FAV_SCDM)
		{
			memcpy(pShareFavDataTmp, pscdm, pShareFavHeader->dwScdmSize);
			pShareFavDataTmp += pShareFavHeader->dwScdmSize;
		}

		if(pShareFavHeader->dwFavField & DM_FAV_CHAPTERINSERT)
		{
			memcpy(pShareFavDataTmp, pChapterInsert, pShareFavHeader->dwChapterInsertSize);
			pShareFavDataTmp += pShareFavHeader->dwChapterInsertSize;
		}

		if(pShareFavHeader->dwFavField & DM_FAV_MARGINSHIFT)
		{
			memcpy(pShareFavDataTmp, pMSData, pShareFavHeader->dwMarginShiftSize);
			pShareFavDataTmp += pShareFavHeader->dwMarginShiftSize;
		}

		if(pShareFavHeader->dwFavField & DM_FAV_USERPAPERSIZE)
		{
			memcpy(pShareFavDataTmp, pUPSData, pShareFavHeader->dwUserPaperSize);
			pShareFavDataTmp += pShareFavHeader->dwUserPaperSize;
		}

		if(pShareFavHeader->dwFavField & DM_FAV_PRINTPOSITION)
		{
			memcpy(pShareFavDataTmp, pPPData, pShareFavHeader->dwPrintPositionSize);
			pShareFavDataTmp += pShareFavHeader->dwPrintPositionSize;
		}

		// <S><A> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
		if(pShareFavHeader->dwFavField & DM_FAV_STOREDTO)
		{
			memcpy(pShareFavDataTmp, pSTRData, pShareFavHeader->dwStoredToSize);
			pShareFavDataTmp += pShareFavHeader->dwStoredToSize;
		}
		
		if(pShareFavHeader->dwFavField & DM_FAV_STOREDTO_PRT_UI)
		{
			memcpy(pShareFavDataTmp, pSTRJCData, pShareFavHeader->dwStoredToPrintUISize);
			pShareFavDataTmp += pShareFavHeader->dwStoredToPrintUISize;
		}
		// <E> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S

	}

EXIT:	
	if(pShareFavHeader != NULL)
		delete pShareFavHeader;
	
	if(pscdm != NULL)
		delete pscdm;
	
	if(pChapterInsert != NULL)
		delete []pChapterInsert;

	if(pSTRData != NULL)
		delete []pSTRData;

	if(pMSData != NULL)
		delete []pMSData;

	if(pUPSData != NULL)
		delete []pUPSData;
	
	if(pPPData != NULL)
		delete []pPPData;

	if(pSTRJCData != NULL)
		delete []pSTRJCData;

	if(pTRData != NULL)
		delete []pTRData;

	if(pCPMData != NULL)
		delete []pCPMData;

	if(pTabShiftData != NULL)
		delete []pTabShiftData;

	return dwRet;
}


//=============================================================================
// function
//      TransferOneFavDataFromHKLMToJSON
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//		lIndex				the item index in JSON
//      pShareFavData       the one fav data in HKLM. it is the [DATA] in 20210811103941,AAA,DWORD,[DATA],
//		dwSize				the size of pShareFavData
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      transfer the one fav data from HKLM to JSON.
//=============================================================================
BOOL CShJsonUS::TransferOneFavDataFromHKLMToJSON(WCHAR  *pszSvrPrnName, DWORD dwIndex, BYTE* pShareFavData,DWORD dwSize,WCHAR* pszTime, WCHAR* pszTitle,BOOL bOverWrite)
{
	BOOL			bRet = FALSE;
	BYTE*			pShareFavTmp = NULL;
	PSCDM			pscdm = NULL;
	PShareFavHeader	pHeader = NULL;
	WCHAR		*pszChpInsPage = NULL;

	if(pszSvrPrnName == NULL || pShareFavData == NULL || pszTime == NULL || pszTitle == NULL)
		goto EXIT;

	pShareFavTmp = pShareFavData;
	
	pHeader = new ShareFavHeader;
	if(pHeader == NULL)
		goto EXIT;
	SecureZeroMemory(pHeader, sizeof(ShareFavHeader)); 
	memcpy(pHeader,pShareFavTmp,sizeof(ShareFavHeader));

	//scdm
	if(pHeader->dwFavField & DM_FAV_SCDM)
	{
		if(pHeader->dwScdmSize == 0)
			goto EXIT;

		pscdm = new SCDM;
		if(pscdm == NULL)
			goto EXIT;
		SecureZeroMemory(pscdm, sizeof(SCDM)); 

		pShareFavTmp += pHeader->dwOffset;
		memcpy(pscdm,pShareFavTmp,pHeader->dwScdmSize);

		//Write the scdm to JSON
		//if(WriteData(pszTitle, pscdm,dwIndex,bOverWrite, pszTime, FALSE) == -1)//bear
			//goto EXIT;

		pShareFavTmp += pHeader->dwScdmSize;
	}

	//chapter insert
	if(pHeader->dwFavField & DM_FAV_CHAPTERINSERT)
	{
		pszChpInsPage = new WCHAR[pHeader->dwChapterInsertSize];
		if(pszChpInsPage == NULL)
			goto EXIT;

		memcpy(pszChpInsPage,pShareFavTmp,pHeader->dwChapterInsertSize);
		WriteChapterInsPage(dwIndex, pszChpInsPage, pHeader->dwChapterInsertSize);
		
		pShareFavTmp += pHeader->dwChapterInsertSize;
	}

	//margin shift
	if(pHeader->dwFavField & DM_FAV_MARGINSHIFT)
	{
		WriteShareDataMSToUS(pszSvrPrnName, dwIndex,pShareFavTmp,pHeader->dwMarginShiftSize);
		pShareFavTmp += pHeader->dwMarginShiftSize;
	}

	//user paper size
	if(pHeader->dwFavField & DM_FAV_USERPAPERSIZE)
	{
		WriteShareDataUPSToUS(pszSvrPrnName, dwIndex,pShareFavTmp,pHeader->dwUserPaperSize);
		pShareFavTmp += pHeader->dwUserPaperSize;
	}

	//print position
	if(pHeader->dwFavField & DM_FAV_PRINTPOSITION)
	{
		WriteShareDataPPToUS(pszSvrPrnName, dwIndex,pShareFavTmp,pHeader->dwPrintPositionSize);
		pShareFavTmp += pHeader->dwPrintPositionSize;
	}

	////trimming
	//if(pHeader->dwFavField & DM_FAV_TRIMMING)
	//{
	//	WriteShareDataTRToUS(pszSvrPrnName, dwIndex,pShareFavTmp,pHeader->dwTrimmingSize);
	//	pShareFavTmp += pHeader->dwTrimmingSize;
	//}

	//custom pamphlet
	//if(pHeader->dwFavField & DM_FAV_CUSPAMPHLET)
	//{
	//	WriteShareDataCPMToUS(pszSvrPrnName, dwIndex,pShareFavTmp,pHeader->dwCPMSize);
	//	pShareFavTmp += pHeader->dwCPMSize;
	//}

	////tab shift
	//if(pHeader->dwFavField & DM_FAV_TABSHIFT)
	//{
	//	WriteShareDataTSToUS(pszSvrPrnName, dwIndex,pShareFavTmp,pHeader->dwTabShiftSize);
	//	pShareFavTmp += pHeader->dwTabShiftSize;
	//}
// <S><A> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
	if(pHeader->dwFavField & DM_FAV_STOREDTO)
	{
		WriteShareDataSTRToUS(pszSvrPrnName, dwIndex,pShareFavTmp,pHeader->dwStoredToSize);
		pShareFavTmp += pHeader->dwStoredToSize;
	}
	//storedTo
	if(pHeader->dwFavField & DM_FAV_STOREDTO_PRT_UI)
	{
		WriteShareDataStoredToUS(pszSvrPrnName, dwIndex,pShareFavTmp,pHeader->dwStoredToPrintUISize);
		pShareFavTmp += pHeader->dwStoredToPrintUISize;
	}
// <E> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S

EXIT:
	if(pHeader != NULL)
		delete pHeader;

	if(pscdm != NULL)
		delete pscdm;

	if(pszChpInsPage != NULL)
		delete[] pszChpInsPage;

	return bRet;
}

//=============================================================================
// function
//      ReadUSMSData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              ì«Ç›éÊÇÈÉCÉìÉfÉbÉNÉX
//      pMSData				read to the pMSData
//
// return value
//      ê¨å˜ : ÉfÅ[É^Ç…ïKóvÇ»ÉTÉCÉY
//      é∏îs : 0
//
// outline
//      Read the user setting's margin shift data specified by lIndex
//=============================================================================
DWORD CShJsonUS::ReadUSMSData(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pMSData)
{
	DWORD			dwSize = 0;
	/*CShJsonMS		*pjsonms = NULL;
	
	DWORD			dwMSCount = 0;
	BYTE			*pMSDataTmp = NULL;
	DWORD			i = 0;
	MARGINSHIFTDATA	msd;

	if(pszSvrPrnName == NULL)
		goto EXIT;
	
	pjsonms = new CShJsonMS(m_hInst, pszSvrPrnName);
	if(pjsonms == NULL)
		goto EXIT;

	pjsonms->SetParent(this);

	dwMSCount = (*pjsonms).ReadMSCount(JSON_KEY_MS_ROOT_BASE_US, lIndex);
	if(dwMSCount == 0)
		goto EXIT;

	dwSize = sizeof(dwMSCount) + dwMSCount * sizeof(MARGINSHIFTDATA);			
	if(pMSData == NULL)
	{
		goto EXIT;
	}

	pMSDataTmp = pMSData;
	memcpy(pMSDataTmp,&dwMSCount,sizeof(dwMSCount));
	pMSDataTmp += sizeof(dwMSCount);

	for(i = 0; i < dwMSCount; i++)
	{
		memset(&msd, 0, sizeof(msd));
		(*pjsonms).ReadMSDataToUs(lIndex, i, &msd);
		memcpy(pMSDataTmp,&msd,sizeof(MARGINSHIFTDATA));
		pMSDataTmp += sizeof(MARGINSHIFTDATA);
	}

EXIT:
	if(pjsonms != NULL)
		delete pjsonms;*///soup

	return dwSize;
}

//=============================================================================
// function
//      WriteShareDataMSToUS
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//		pMSData				the margin shift data in HKLM
//		dwSize				the size of pMSData
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//       analyze the ms data from HKLM and write them to JSON's user setting 
//=============================================================================
long CShJsonUS::WriteShareDataMSToUS(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pMSData, DWORD dwSize)
{
	long			lRet = -1;

//	CShJsonMS		*pjsonms = NULL;
//	MARGINSHIFTDATA	msd;
//	DWORD			dwCount;
//	DWORD			dwDataSize;
//	DWORD			i;
//	BYTE			*pMSDataTmp = NULL;
//		
//	// ================================================
//	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
//	// ================================================
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//
//	if(pMSData == NULL || dwSize <= 0)
//		goto EXIT;
//
//	pjsonms = new CShJsonMS(m_hInst, pszSvrPrnName);
//	if(pjsonms == NULL)
//		goto EXIT;
//
//	pjsonms->SetParent(this);
//
//	pMSDataTmp = pMSData;
//	memcpy(&dwCount,pMSDataTmp,sizeof(dwCount));
//	if(dwCount == 0)
//		goto EXIT;
//	
//	pMSDataTmp += sizeof(dwCount);
//
//	dwDataSize = sizeof(dwCount) + dwCount * sizeof(MARGINSHIFTDATA);
//	if(dwSize != dwDataSize)
//		goto EXIT;
//
//	for(i=0; i < dwCount; i++)
//	{
//		memset(&msd, 0, sizeof(msd));
//		memcpy(&msd,pMSDataTmp,sizeof(MARGINSHIFTDATA));
//
//		(*pjsonms).WriteMSDataToUs(lIndex, i, &msd);
//		pMSDataTmp += sizeof(MARGINSHIFTDATA);
//	}
//		
//	(*pjsonms).WriteMSCount(JSON_KEY_MS_ROOT_BASE_US, lIndex, dwCount);
//	lRet = lIndex;
//
//EXIT:
//	if(pjsonms != NULL)
//		delete pjsonms;//soup

	return lRet;
}
// <S><A> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
//=============================================================================
// function
//      ReadUSSTRData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              ì«Ç›éÊÇÈÉCÉìÉfÉbÉNÉX
//      pSTRData			read to the pSTRData
//
// return value
//      ê¨å˜ : ÉfÅ[É^Ç…ïKóvÇ»ÉTÉCÉY
//      é∏îs : 0
//
// outline
//      Read the user setting's storedTo data specified by lIndex
//=============================================================================
DWORD CShJsonUS::ReadUSSTRData(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pSTRData)
{
	//CShJsonStored 	*pjsonstrd;
	DWORD			dwSize = 0;
//	BOOL			blRet = FALSE;
//	BYTE			*pSTRDataTmp = NULL;
//	DWORD			dwFoldIndex = 0;
//	WCHAR			szSelFolderName[512];
//	STORED_TO_DATA	strd;
//	DWORD			dwFolderSize = sizeof(szSelFolderName);
//
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//	
//	pjsonstrd = new CShJsonStored(m_hInst, pszSvrPrnName);
//	if(pjsonstrd == NULL)
//		goto EXIT;
//
//	pjsonstrd->SetParent(this);
//
//	dwSize = sizeof(STORED_TO_DATA);			
//	if(pSTRData == NULL)
//	{
//		goto EXIT;
//	}
//
//	pSTRDataTmp = pSTRData;
//
//	memset(&strd, 0, sizeof(strd));
// 	blRet = (*pjsonstrd).ReadSTDDataToUs(lIndex, szSelFolderName, dwFolderSize,dwFoldIndex);
//	if(!blRet)
//		goto EXIT;
//
//	    wcsncpy(strd.pszFolderName, szSelFolderName, sizeof(strd.pszFolderName) - 1);
//		strd.pszFolderName[sizeof(strd.pszFolderName) - 1] = '\0'; // Null-terminate the string
//
//		strd.dwFolderIndex = dwFoldIndex;
//		memcpy(pSTRDataTmp, &strd, sizeof(STORED_TO_DATA));
//		pSTRDataTmp += sizeof(STORED_TO_DATA);
//
//
//EXIT:
//	if(pjsonstrd != NULL)
//		delete pjsonstrd;//soup

	return dwSize;
}

//=============================================================================
// function
//      WriteShareDataSTRToUS
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//		pSTRData			the stored to data in HKLM
//		dwSize				the size of pSTRData
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//       analyze the storedTo data from HKLM and write them to JSON's user setting 
//=============================================================================
long CShJsonUS::WriteShareDataSTRToUS(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pSTRData, DWORD dwSize)
{
	long			lRet = -1;
//	CShJsonStored 	*pjsonstrd = NULL;
//	STORED_TO_DATA	strd;
//	DWORD			dwCount;
//	DWORD			dwDataSize;
//	DWORD			i;
//	BYTE			*pSTRDataTmp = NULL;
//		
//	// ================================================
//	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
//	// ================================================
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//
//	if(pSTRData == NULL || dwSize <= 0)
//		goto EXIT;
//
//	pjsonstrd = new CShJsonStored(m_hInst, pszSvrPrnName);
//	if(pjsonstrd == NULL)
//		goto EXIT;
//
//	pjsonstrd->SetParent(this);
//
//	pSTRDataTmp = pSTRData;
//
//
//	dwDataSize = sizeof(STORED_TO_DATA);
//	if(dwSize != dwDataSize)
//		goto EXIT;
//
//	memset(&strd, 0, sizeof(strd));
//	memcpy(&strd,pSTRDataTmp,sizeof(STORED_TO_DATA));
//
//
//	(*pjsonstrd).WriteSTDDataToUs(lIndex, strd.pszFolderName, sizeof(strd.pszFolderName),strd.dwFolderIndex);
//	lRet = lIndex;
//	pSTRDataTmp += sizeof(STORED_TO_DATA);
//	lRet = lIndex;
//
//EXIT:
//	if(pjsonstrd != NULL)
//		delete pjsonstrd;//soup

	return lRet;
}

//=============================================================================
// function
//      ReadStoredToData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              ì«Ç›éÊÇÈÉCÉìÉfÉbÉNÉX
//      pSTRJCData			read to the pSTRJCData
//
// return value
//      ê¨å˜ : ÉfÅ[É^Ç…ïKóvÇ»ÉTÉCÉY
//      é∏îs : 0
//
// outline
//      Read the storedTo data which is outside the user setting's specified by lIndex
//=============================================================================
DWORD CShJsonUS::ReadStoredToData(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pSTRJCData)
{
		DWORD			dwSize = 0;
//	CShJsonStored 	*pjsonstrd = NULL;

//	DWORD			dwSTRJCCount = 0;
//	BYTE			*pSTRJCDataTmp = NULL;
//	DWORD			i = 0;
//	BOOL			blSuccess = FALSE;
//	STORED_TO_DATA_printui strdjc;
//
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//	
//	pjsonstrd = new CShJsonStored(m_hInst, pszSvrPrnName);
//	if(pjsonstrd == NULL)
//		goto EXIT;
//
//	pjsonstrd->SetParent(this);
//
//	//dwSTRJCCount = (*pjsonstrd).ReadMSCount(JSON_KEY_MS_ROOT_BASE_US, lIndex);
//	blSuccess = m_pParent->ReadJsonDWORDData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_COUNT, dwSTRJCCount);
//	if(dwSTRJCCount == 0)
//		goto EXIT;
//
//	dwSize = sizeof(dwSTRJCCount) + dwSTRJCCount * sizeof(STORED_TO_DATA_printui);			
//	if(pSTRJCData == NULL)
//	{
//		goto EXIT;
//	}
//
//	pSTRJCDataTmp = pSTRJCData;
//	memcpy(pSTRJCDataTmp,&dwSTRJCCount,sizeof(dwSTRJCCount));
//	pSTRJCDataTmp += sizeof(dwSTRJCCount);
//
//	for(i = 0; i < dwSTRJCCount; i++)
//	{
//		memset(&strdjc, 0, sizeof(strdjc));
//		(*pjsonstrd).ReadStrdData(i, strdjc.FolderName, sizeof(strdjc.FolderName), &strdjc.HasFolderPin);
//		memcpy(pSTRJCDataTmp,&strdjc,sizeof(STORED_TO_DATA_printui));
//		pSTRJCDataTmp += sizeof(STORED_TO_DATA_printui);
//	}
//
//EXIT:
//	if(pjsonstrd != NULL)
//		delete pjsonstrd;//soup

	return dwSize;
}

//=============================================================================
// function
//      WriteShareDataStoredToUS
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//		pSTRJCData			the stored to data in HKLM
//		dwSize				the size of pSTRJCData
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//       analyze the storedTo data from HKLM and write them to JSON file.
//=============================================================================
long CShJsonUS::WriteShareDataStoredToUS(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pSTRJCData, DWORD dwSize)
{
	long			lRet = -1;
//	CShJsonStored 	*pjsonstrd = NULL;
//	STORED_TO_DATA_printui strdjc;
//	DWORD			dwSTRJCCount;
//	DWORD			dwDataSize;
//	DWORD			i;
//	BYTE			*pSTRJCDataTmp = NULL;
//		
//	// ================================================
//	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
//	// ================================================
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//
//	if(pSTRJCData == NULL || dwSize <= 0)
//		goto EXIT;
//
//	pjsonstrd = new CShJsonStored(m_hInst, pszSvrPrnName);
//	if(pjsonstrd == NULL)
//		goto EXIT;
//
//	pjsonstrd->SetParent(this);
//
//	pSTRJCDataTmp = pSTRJCData;
//	memcpy(&dwSTRJCCount,pSTRJCDataTmp,sizeof(dwSTRJCCount));
//	if(dwSTRJCCount == 0)
//		goto EXIT;
//	
//	pSTRJCDataTmp += sizeof(dwSTRJCCount);
//
//	dwDataSize = sizeof(dwSTRJCCount) + dwSTRJCCount * sizeof(STORED_TO_DATA_printui);
//	if(dwSize != dwDataSize)
//		goto EXIT;
//
//	for(i=0; i < dwSTRJCCount; i++)
//	{
//		memset(&strdjc, 0, sizeof(strdjc));
//		memcpy(&strdjc,pSTRJCDataTmp,sizeof(STORED_TO_DATA_printui));
//		(*pjsonstrd).WriteStrdData(i, strdjc.FolderName, strdjc.HasFolderPin);
//		pSTRJCDataTmp += sizeof(STORED_TO_DATA_printui);
//	}
//	
//	(*pjsonstrd).WriteStrdCount(dwSTRJCCount);
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjsonstrd != NULL)
//		delete pjsonstrd;//soup

	return lRet;
}
// <E> 2024.02.15,To Fix Feedback #3115 SSDI:Manoj S
//=============================================================================
// function
//      ReadUSUPSData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              ì«Ç›éÊÇÈÉCÉìÉfÉbÉNÉX
//      pUPSData			read to the pUPSData
//
// return value
//      ê¨å˜ : ÉfÅ[É^Ç…ïKóvÇ»ÉTÉCÉY
//      é∏îs : 0
//
// outline
//      Read the user setting's user paper size data specified by lIndex
//=============================================================================
DWORD CShJsonUS::ReadUSUPSData(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pUPSData)
{
	DWORD			dwSize = 0;
//	CShJsonUserPSize	*pjson = NULL;
//	
//	DWORD			dwCount = 0;
//	DWORD			dwCountMcf;
//	long			*pl = NULL;
//	BYTE			*pDataTmp = NULL;
//	DWORD			i = 0;
//	USERPAPERSIZEDATA	upsd;
//
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//	
//	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
//	//	goto EXIT;
//
//	dwCountMcf = (DWORD)(*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_CUSTOM_PAPERSIZE);
//	if(dwCountMcf == 0)
//		goto EXIT;
//
//	pl = (*m_pmcf).GetLongArrayData();
//
//	pjson = new CShJsonUserPSize(m_hInst, pszSvrPrnName);
//	if(pjson == NULL)
//		goto EXIT;
//
//	pjson->SetParent(this);
//
//	dwCount = (*pjson).ReadUPSCount(JSON_KEY_UPS_ROOT_BASE_US, lIndex);
//	if(dwCount == 0)
//		goto EXIT;
//
//	dwSize = sizeof(dwCount) + dwCount * sizeof(USERPAPERSIZEDATA);			
//	if(pUPSData == NULL)
//	{
//		goto EXIT;
//	}
//
//	pDataTmp = pUPSData;
//	memcpy(pDataTmp,&dwCount,sizeof(dwCount));
//	pDataTmp += sizeof(dwCount);
//
//	for(i=0; i < dwCount; i++)
//	{
//		memset(&upsd, 0, sizeof(upsd));
//		BOOL blRet = (*pjson).ReadUPSDataToUs(lIndex, pl[i], &upsd);
//		if(!blRet)
//			goto EXIT;
//		memcpy(pDataTmp,&upsd,sizeof(USERPAPERSIZEDATA));
//		pDataTmp += sizeof(USERPAPERSIZEDATA);
//	}
//
//EXIT:
//	if(pjson != NULL)
//		delete pjson;//soup

	return dwSize;
}

//=============================================================================
// function
//      WriteShareDataUPSToUS
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//		pUPSData			the user paper size data in HKLM
//		dwSize				the size of pUPSData
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//       analyze the user paper size data from HKLM and write them to JSON's user setting 
//=============================================================================
long CShJsonUS::WriteShareDataUPSToUS(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pUPSData, DWORD dwSize)
{
	long			lRet = -1;

//	CShJsonUserPSize		*pjson = NULL;
//	USERPAPERSIZEDATA	upsd;
//	DWORD			dwCount;
//	DWORD			dwCountMcf;
//	long			*pl = NULL;
//	DWORD			dwDataSize;
//	DWORD			i;
//	BYTE			*pDataTmp = NULL;
//		
//	// ================================================
//	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
//	// ================================================
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//
//	if(pUPSData == NULL || dwSize <= 0)
//		goto EXIT;
//
//	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
//		goto EXIT;*/
//
//	dwCountMcf = (DWORD)(*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_CUSTOM_PAPERSIZE);
//	if(dwCountMcf == 0)
//		goto EXIT;
//
//	pl = (*m_pmcf).GetLongArrayData();
//
//	pjson = new CShJsonUserPSize(m_hInst, pszSvrPrnName);
//	if(pjson == NULL)
//		goto EXIT;
//
//	pjson->SetParent(this);
//	
//	pDataTmp = pUPSData;
//	memcpy(&dwCount,pDataTmp,sizeof(dwCount));
//	if(dwCount != dwCountMcf)
//		goto EXIT;
//	
//	pDataTmp += sizeof(dwCount);
//
//	dwDataSize = sizeof(dwCount) + dwCount * sizeof(USERPAPERSIZEDATA);
//	if(dwSize != dwDataSize)
//		goto EXIT;
//
//	for(i=0; i < dwCount; i++)
//	{
//		memset(&upsd, 0, sizeof(upsd));
//		memcpy(&upsd,pDataTmp,sizeof(USERPAPERSIZEDATA));
//
//		(*pjson).WriteUPSDataToUs(lIndex, pl[i], &upsd);
//		pDataTmp += sizeof(USERPAPERSIZEDATA);
//	}
//	
//	(*pjson).WriteUPSCount(JSON_KEY_UPS_ROOT_BASE_US, lIndex, dwCount);
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjson != NULL)
//		delete pjson;//soup

	return lRet;
}

//=============================================================================
// function
//      ReadUSPPData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              ì«Ç›éÊÇÈÉCÉìÉfÉbÉNÉX
//      pPPData				read to the pPPData
//
// return value
//      ê¨å˜ : ÉfÅ[É^Ç…ïKóvÇ»ÉTÉCÉY
//      é∏îs : 0
//
// outline
//      Read the user setting's print position data specified by lIndex
//=============================================================================
DWORD CShJsonUS::ReadUSPPData(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pPPData)
{
	//CShJsonPP		*pjson = NULL;
	DWORD			dwSize = 0;
	/*DWORD			dwCount = 0;
	PRINTPOSITION	ppd;

	if(pszSvrPrnName == NULL)
		goto EXIT;
	
	pjson = new CShJsonPP(m_hInst, pszSvrPrnName);
	if(pjson == NULL)
		goto EXIT;

	pjson->SetParent(this);

	dwCount = (*pjson).ReadPPCount(JSON_KEY_PP_ROOT_BASE_US, lIndex);
	if(dwCount == 0)
		goto EXIT;

	dwSize = sizeof(PRINTPOSITION);			
	if(pPPData == NULL)
	{
		goto EXIT;
	}

	memset(&ppd, 0, sizeof(ppd));
	BOOL blRet = (*pjson).ReadPPDataToUs(lIndex, &ppd);
	if(!blRet)
		goto EXIT;
	memcpy(pPPData,&ppd,sizeof(PRINTPOSITION));


EXIT:
	if(pjson != NULL)
		delete pjson;*///soup

	return dwSize;
}

//=============================================================================
// function
//      WriteShareDataPPToUS
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//		pPPData				the print position data in HKLM
//		dwSize				the size of pPPData
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//       analyze the print position data from HKLM and write them to JSON's user setting 
//=============================================================================
long CShJsonUS::WriteShareDataPPToUS(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pPPData, DWORD dwSize)
{
	long			lRet = -1;

//	CShJsonPP		*pjson = NULL;
//	PRINTPOSITION	ppd;
//	DWORD			dwDataSize;
//
//	// ================================================
//	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
//	// ================================================
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//
//	if(pPPData == NULL || dwSize <= 0)
//		goto EXIT;
//
//	pjson = new CShJsonPP(m_hInst, pszSvrPrnName);
//	if(pjson == NULL)
//		goto EXIT;
//
//	pjson->SetParent(this);
//
//	dwDataSize = sizeof(PRINTPOSITION);
//	if(dwSize != dwDataSize)
//		goto EXIT;
//
//	memset(&ppd, 0, sizeof(ppd));
//	memcpy(&ppd,pPPData,dwSize);
//
//	(*pjson).WritePPDataToUs(lIndex, &ppd);	
//	(*pjson).WritePPCount(JSON_KEY_PP_ROOT_BASE_US, lIndex, 1);
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjson != NULL)
//		delete pjson;
//	  soup
	return lRet;
}

//=============================================================================
// function
//      ReadUSTRData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              ì«Ç›éÊÇÈÉCÉìÉfÉbÉNÉX
//      pTRData				read to the pTRData
//
// return value
//      ê¨å˜ : ÉfÅ[É^Ç…ïKóvÇ»ÉTÉCÉY
//      é∏îs : 0
//
// outline
//      Read the user setting's trimming data specified by lIndex
//=============================================================================
//DWORD MY_CLASSNAME::ReadUSTRData(char  *pszSvrPrnName, long lIndex, BYTE* pTRData)
//{
////	CShJsonTR		*pjson = NULL;
//	DWORD			dwSize = 0;
//	DWORD			dwCount = 0;
//	TRIMMINGDATA	trd;
//
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//	
//	pjson = new CShJsonTR(m_hInst, pszSvrPrnName, m_pLog);
//	if(pjson == NULL)
//		goto EXIT;
//
//	pjson->SetParent(this);
//
//	dwCount = (*pjson).ReadTRCount(JSON_KEY_TR_ROOT_BASE_US, lIndex);
//	if(dwCount == 0)
//		goto EXIT;
//
//	dwSize = sizeof(TRIMMINGDATA);			
//	if(pTRData == NULL)
//	{
//		goto EXIT;
//	}
//
//	memset(&trd, 0, sizeof(trd));
//	BOOL blRet = (*pjson).ReadTRDataToUs(lIndex, &trd);
//	if(!blRet)
//		goto EXIT;
//	memcpy(pTRData,&trd,sizeof(TRIMMINGDATA));
//
//EXIT:
//	if(pjson != NULL)
//		delete pjson;
//
//	return dwSize;
//}

//=============================================================================
// function
//      WriteShareDataTRToUS
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//		pTRData				the trimming data in HKLM
//		dwSize				the size of pTRData
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//       analyze the trimming data from HKLM and write them to JSON's user setting 
//=============================================================================
//long MY_CLASSNAME::WriteShareDataTRToUS(char  *pszSvrPrnName, long lIndex, BYTE* pTRData, DWORD dwSize)
//{
//	long			lRet = -1;
//
//	//CShJsonTR		*pjson = NULL;
//	TRIMMINGDATA	trd;
//	DWORD			dwDataSize;
//
//	// ================================================
//	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
//	// ================================================
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//
//	if(pTRData == NULL || dwSize <= 0)
//		goto EXIT;
//
//	pjson = new CShJsonTR(m_hInst, pszSvrPrnName, m_pLog);
//	if(pjson == NULL)
//		goto EXIT;
//
//	pjson->SetParent(this);
//
//	dwDataSize = sizeof(TRIMMINGDATA);
//	if(dwSize != dwDataSize)
//		goto EXIT;
//
//	memset(&trd, 0, sizeof(trd));
//	memcpy(&trd,pTRData,dwSize);
//
//	(*pjson).WriteTRDataToUs(lIndex, &trd);	
//	(*pjson).WriteTRCount(JSON_KEY_TR_ROOT_BASE_US, lIndex, 1);
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjson != NULL)
//		delete pjson;
//
//	return lRet;
//}

//=============================================================================
// function
//      ReadUSCPMData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              ì«Ç›éÊÇÈÉCÉìÉfÉbÉNÉX
//      pCPMData			read to the pCPMData
//
// return value
//      ê¨å˜ : ÉfÅ[É^Ç…ïKóvÇ»ÉTÉCÉY
//      é∏îs : 0
//
// outline
//      Read the user setting's custom pamphlet data specified by lIndex
//=============================================================================
//DWORD MY_CLASSNAME::ReadUSCPMData(char  *pszSvrPrnName, long lIndex, BYTE* pCPMData)
//{
////	CShJsonCPM		*pjson = NULL;
//	DWORD			dwSize = 0;
//	DWORD			dwCount = 0;
//	CUSTOMPAMPHLETDATA	cpmd;
//
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//	
//	pjson = new CShJsonCPM(m_hInst, pszSvrPrnName, m_pLog);
//	if(pjson == NULL)
//		goto EXIT;
//
//	pjson->SetParent(this);
//
//	dwCount = (*pjson).ReadCPMCount(JSON_KEY_CPM_ROOT_BASE_US, lIndex);
//	if(dwCount == 0)
//		goto EXIT;
//
//	dwSize = sizeof(CUSTOMPAMPHLETDATA);			
//	if(pCPMData == NULL)
//	{
//		goto EXIT;
//	}
//
//	memset(&cpmd, 0, sizeof(cpmd));
//	BOOL blRet = (*pjson).ReadCPMDataToUs(lIndex, &cpmd);
//	if(!blRet)
//		goto EXIT;
//	memcpy(pCPMData,&cpmd,sizeof(CUSTOMPAMPHLETDATA));
//
//EXIT:
//	if(pjson != NULL)
//		delete pjson;
//
//	return dwSize;
//}

//=============================================================================
// function
//      WriteShareDataCPMToUS
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//		pTRData				the custom pamphlet data in HKLM
//		dwSize				the size of pCPMData
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//       analyze the custom pamphlet data from HKLM and write them to HKCU's user setting 
//=============================================================================
//long MY_CLASSNAME::WriteShareDataCPMToUS(char  *pszSvrPrnName, long lIndex, BYTE* pCPMData, DWORD dwSize)
//{
//	long			lRet = -1;
//
//	//CShJsonCPM		*pjson = NULL;
//	CUSTOMPAMPHLETDATA	cpmd;
//	DWORD			dwDataSize;
//
//	// ================================================
//	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
//	// ================================================
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//
//	if(pCPMData == NULL || dwSize <= 0)
//		goto EXIT;
//
//	pjson = new CShJsonCPM(m_hInst, pszSvrPrnName, m_pLog);
//	if(pjson == NULL)
//		goto EXIT;
//
//	pjson->SetParent(this);
//
//	dwDataSize = sizeof(CUSTOMPAMPHLETDATA);
//	if(dwSize != dwDataSize)
//		goto EXIT;
//
//	memset(&cpmd, 0, sizeof(cpmd));
//	memcpy(&cpmd,pCPMData,dwSize);
//
//	(*pjson).WriteCPMDataToUs(lIndex, &cpmd);	
//	(*pjson).WriteCPMCount(JSON_KEY_CPM_ROOT_BASE_US, lIndex, 1);
//
//	lRet = lIndex;
//
//EXIT:
//	if(pjson != NULL)
//		delete pjson;
//
//	return lRet;
//}

//=============================================================================
// function
//      ReadUSTSData
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              ì«Ç›éÊÇÈÉCÉìÉfÉbÉNÉX
//      pData				read to the pData
//
// return value
//      ê¨å˜ : ÉfÅ[É^Ç…ïKóvÇ»ÉTÉCÉY
//      é∏îs : 0
//
// outline
//      Read the user setting's tab_shift data specified by lIndex
//=============================================================================
DWORD CShJsonUS::ReadUSTSData(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pData)
{
//	CShJsonTabPrn	*pjson = NULL;
	DWORD			dwSize = 0;
	DWORD			dwUnit = 0;

	long			lDef = GetUnitDef();

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
	// ================================================
	if(pszSvrPrnName == NULL)
		goto EXIT;

	if(m_pParent == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  ì«Ç›çûÇ›
	// ================================================
// <S><C> 2022.05.27 sbc R.Tanaka
#if 0
	pjson = new CShJsonTabPrn(m_hInst, pszSvrPrnName, m_pLog);
	if(pjson == NULL)
		goto EXIT;

	pjson->SetParent(this);

	dwSize = sizeof(dwUnit);			
	if(pData == NULL)
	{
		goto EXIT;
	}

	dwUnit = (*pjson).ReadUnitFromUs(lIndex);
	
	memcpy(pData, &dwUnit,dwSize);
#else

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_TSF_ROOT_BASE_US, lIndex);

	dwSize = sizeof(dwUnit);			
	if(pData == NULL)
	{
		goto EXIT;
	}
	
	if(m_pParent->ReadJsonDWORDData(pszKey,JSON_ENT_TSF_UNIT,dwUnit) == FALSE)
	{
		dwUnit = lDef;
	}
	memcpy(pData, &dwUnit, dwSize);

#endif

EXIT:
#if 0
	if(pjson != NULL)
		delete pjson;
#else
	if(pszKey != NULL)
		delete[] pszKey;
#endif
// <E> 2022.05.27 sbc R.Tanaka
	return dwSize;
}

//=============================================================================
// function
//      WriteShareDataTSToUS
//
// parameters
//      pszSvrPrnName       ÉTÅ[ÉoÅ[ñºïtÇ´ÉvÉäÉìÉ^ñº
//      lIndex              èëÇ´çûÇﬁÉCÉìÉfÉbÉNÉX
//		pData				the custom pamphlet data in HKLM
//		dwSize				the size of pData
//
// return value
//      ê¨å˜ : èëÇ´çûÇÒÇæÉCÉìÉfÉbÉNÉX
//      é∏îs : -1
//
// outline
//       analyze the tab shift data from HKLM and write them to HKCU's user setting 
//=============================================================================
long CShJsonUS::WriteShareDataTSToUS(WCHAR  *pszSvrPrnName, long lIndex, BYTE* pData, DWORD dwSize)
{
	long			lRet = -1;

	// CShJsonTabPrn	*pjson = NULL;
	DWORD			dwUnit;
	DWORD			dwDataSize;
	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  ÉpÉâÉÅÅ[É^É`ÉFÉbÉN
	// ================================================
	if(pszSvrPrnName == NULL)
		goto EXIT;

	if(pData == NULL || dwSize <= 0)
		goto EXIT;

	if(m_pParent == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  èëÇ´çûÇ›
	// ================================================
// <S><C> 2022.05.27 sbc R.Tanaka
#if 0
	pjson = new CShJsonTabPrn(m_hInst, pszSvrPrnName, m_pLog);
	if(pjson == NULL)
		goto EXIT;

	pjson->SetParent(this);
	dwDataSize = sizeof(dwUnit);
	if(dwSize != dwDataSize)
		goto EXIT;

	memcpy(&dwUnit,pData,dwSize);

	(*pjson).WriteUnitToUs((short)dwUnit,lIndex);	

	lRet = lIndex;
#else
	dwDataSize = sizeof(dwUnit);
	if(dwSize != dwDataSize)
		goto EXIT;

	memcpy(&dwUnit,pData,dwSize);

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_TSF_ROOT_BASE_US, lIndex);

	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_TSF_UNIT,dwUnit);

	lRet = lIndex;

#endif

EXIT:
#if 0
	if(pjson != NULL)
		delete pjson;
#else
	if(pszKey != NULL)
		delete[] pszKey;
#endif
// <E> 2022.05.27 sbc R.Tanaka

	return lRet;
}
// <E> 2022.04.13 Jelly 


// <S><A> 2022.05.17 sbc R.Tanaka
//=============================================================================
// function
//      SetFunctionsJsonPath
//
// parameters
//      pszDeviceName		
//
// return value
//      ñ≥Çµ
//
// outline
//     add functions name to json file path
//=============================================================================
void CShJsonUS::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_US);
}
// <E> 2022.05.17 sbc R.Tanaka


// <S><A> 2022.05.27 sbc R.Tanaka
// TabShiftä÷åW (AdvancedÉ_ÉCÉAÉçÉO)
//=============================================================================
// function
//      SetUnit
//
// parameters
//      wVal                íPà 
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      éwíËÇ≥ÇÍÇΩílÇÉÅÉÇÉäÇ…ï€ë∂Ç∑ÇÈ
//=============================================================================
BOOL CShJsonUS::SetUnit(short wVal)
{
	BOOL	blRet = FALSE;

	m_wUnitVal = wVal;

	blRet = TRUE;

	return blRet;
}

//=============================================================================
// function
//      ResetUnit
//
// parameters
//      wValÅ@Å@Å@Å@Å@Å@íPà 
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      TabShiftÇÃUnitÇèâä˙ílÇ…Ç∑ÇÈ
//=============================================================================
BOOL CShJsonUS::ResetUnit()
{
	BOOL			blRet = FALSE;

	long			lVal = 0;

	lVal = GetUnitDef();
	blRet = SetUnit((short)lVal);

	return blRet;
}

//=============================================================================
// function
//      ClearUnit
//
// parameters
//      dwCount             èëÇ´çûÇﬁíl
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALE
//
// outline
//      tab shiftÇÃèëÇ´çûÇ›ÉtÉâÉOÇÉNÉäÉAÇ∑ÇÈ
//=============================================================================
BOOL CShJsonUS::ClearUnit()
{
	m_blUnitWriteFlg	= FALSE;

	return TRUE;
}

//=============================================================================
// function
//      WriteUnit
//
// parameters
//      wValÅ@Å@Å@Å@Å@Å@íPà 
//
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//      tab shiftÇÃíPà Çï€ë∂Ç∑ÇÈ
//=============================================================================
BOOL CShJsonUS::WriteUnit(short wVal)
{
	BOOL			blRet = FALSE;

	long			lVal;

	if(m_pParent == NULL)
		goto EXIT;

	lVal = wVal;
	m_pParent->WriteJsonDWORDData(JSON_KEY_TSF_ROOT_BASE, JSON_ENT_TSF_UNIT,lVal);

	SetUnit(wVal);
	m_blUnitWriteFlg = TRUE;

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      ReadUnit
//
// parameters
//      ñ≥Çµ
//
// return value
//      É~Éä : 0
//      ÉCÉìÉ` : 1
//
// outline
//      tab shiftÇÃíPà ÇéÊìæÇ∑ÇÈ
//=============================================================================
short CShJsonUS::ReadUnit()
{
	long			lVal = 0;
	DWORD			dwData = 0;

	long			lDef = GetUnitDef();

	lVal = lDef;

	if (m_blUnitWriteFlg == TRUE){
		lVal = (long)m_wUnitVal;
		goto EXIT;
	}

	if(m_pParent == NULL)
		goto EXIT;

	if(m_pParent->ReadJsonDWORDData(JSON_KEY_TSF_ROOT_BASE, JSON_ENT_TSF_UNIT, dwData) == FALSE)
	{
		ResetUnit();
		lVal = (long)m_wUnitVal;
	}
	else{
		m_blUnitWriteFlg = TRUE;
		lVal = (short)dwData;
		m_wUnitVal = (short)dwData;
		goto EXIT;
	}

EXIT:

	return (short)lVal;
}

// <E> 2022.05.27 sbc R.Tanaka