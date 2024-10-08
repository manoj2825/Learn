// ============================================================================
// shJsonwm.cpp
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.03.07
// ============================================================================

#include <precomp.h>
#include "shjson.h"
#include "shJsonwm.h"
#include "RegistryAPI.h"
#include "jsonkey.h"


#define PARENT_CLASSNAME 	CShJson
#define MY_CLASSNAME 		CShJsonWm

//=============================================================================
// function
//      CShJsonWm
//
// parameters
//      hInst               ƒ‚ƒWƒ…[ƒ‹ƒnƒ“ƒhƒ‹
//      pLog                CShPrnLog ƒNƒ‰ƒXƒAƒhƒŒƒX
//
// return value
//      –³‚µ
//
// outline
//      CShJsonWm ƒNƒ‰ƒXƒRƒ“ƒXƒgƒ‰ƒNƒ^
//=============================================================================
CShJsonWm::CShJsonWm(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName, HMODULE	hStringResourceHandle)/*, CShPrnLog FAR *pLog*/
			: CShJson(hInst, pszSvrPrnName/*, pLog*/), CRegistryAPI(),m_wLanguageID(0), m_cshIniFile(NULL), m_hStringResourceHandle(hStringResourceHandle)
{
	SetParent(this);
}


//=============================================================================
// function
//      ~CShJsonWm
//
// parameters
//      –³‚µ
//
// return value
//      –³‚µ
//
// outline
//      CShJsonWm ƒNƒ‰ƒXƒfƒXƒgƒ‰ƒNƒ^
//=============================================================================
CShJsonWm::~CShJsonWm()
{
}


//=============================================================================
// function
//      reset
//
// parameters
//      pIniFile            CShIniFile ƒNƒ‰ƒXƒAƒhƒŒƒX
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALSE
//
// outline
//      INIƒtƒ@ƒCƒ‹‚Ìƒf[ƒ^‚ðŽg—p‚µ‚ÄƒŒƒWƒXƒgƒŠ‚ð‰Šú‰»‚·‚é
//=============================================================================
BOOL CShJsonWm::reset(WCHAR FAR *pszSvrPrnName, CShIniFile FAR *pIniFile)
{
	BOOL			blRet = FALSE;
	m_cshIniFile = pIniFile;
	DWORD			dwCount = 0;

	CShIniFile FAR	*pini = NULL;

	WATERMARKDATA	wmd;
	
	DWORD			i;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	if(pszSvrPrnName == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  Šù‚É‘‚«ž‚Ü‚ê‚Ä‚¢‚éê‡‚Íreset‚µ‚È‚¢
	// _/_/_/  2005.10.18 S.Kawabata (SQAŽw“E‘Î‰žj
	// ================================================
	if(ReadCount() > 1)
		return TRUE;

	// ================================================
	// _/_/_/  INIƒtƒ@ƒCƒ‹ƒNƒ‰ƒXŠm’è
	// ================================================
	if(pIniFile != NULL)
		pini = pIniFile;
	//else
	//{
	///*	if(CreateObj(scui_obj_ini_lang) == FALSE)
	//		goto EXIT;*/
	//	
	//	pini = (CShIniFile FAR *)GetObjAddr(scui_obj_ini_lang);
	//	if(pini == NULL)
	//		goto EXIT;
	//}

	// ================================================
	// _/_/_/  ‰Šú‰»î•ñŽæ“¾‚Æ‘‚«ž‚Ý
	// ================================================
	dwCount = GetCountIni(pIniFile);
	if(dwCount == 0)
		goto EXIT;

	SecureZeroMemory(&wmd, sizeof(wmd));
	for(i = 0; i < dwCount; i++)
	{
		GetWMDefData(i, &wmd/*, pIniFile*/);
		
		WriteWMData(i, &wmd, wmd.szMainWM);
	}

	WriteWMCount(dwCount);

// for Watermark JobInfo
#ifdef _WIN32
	// ================================================
	// _/_/_/  JobInfoî•ñ‚Ì‘‚«ž‚Ý
	// ================================================
	ResetJobInfoSetting(pszSvrPrnName);
#endif

	blRet = TRUE;

EXIT:

	return blRet;
}


//=============================================================================
// function
//      WriteWMData
//
// parameters
//      lIndex              ‘‚«ž‚ÞƒCƒ“ƒfƒbƒNƒX
//      pwmd                ‘‚«ž‚ÞWATERMARKDATA\‘¢‘ÌƒAƒhƒŒƒX
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      Žw’è‚³‚ê‚½WATERMARKDATA\‘¢‘Ì‚Ì“à—e‚ðŽw’è‚ÌƒCƒ“ƒfƒbƒNƒX‚ÅJson‚É
//      ‘‚«ž‚Þ
//=============================================================================
BOOL CShJsonWm::WriteWMData(long lIndex, LPWATERMARKDATA pwmd, WCHAR FAR *pszTitle, WCHAR FAR *pszTime, BOOL bSelf)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*pszBase = NULL;

	short			wBy = 1;
	DWORD			dwData = 0;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	if(m_pszSvrPrnName == NULL || pszTitle == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  WATERMARKDATAŠm’è
	// ================================================
	if(pwmd == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  ‘‚«ž‚Ý
	// ================================================
		//wBy = 2;


	pszBase = new WCHAR[SCUI_REGKEYSIZE];
	if(pszBase == NULL)
		goto EXIT;

	SecureZeroMemory(pszBase, SCUI_REGKEYSIZE);
	::wsprintf(pszBase, JSON_KEY_WM_ROOT_BASE, lIndex);

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_INDEX_BASE, lIndex);

	if(m_pParent == NULL)
		goto EXIT;

	// --- Title
	m_pParent->WriteJsonStrData(pszKey, JSON_ENT_WM_TITLE, pszTitle, (DWORD)wcslen(pszTitle) * wBy);

	if(lIndex > 0)
	{
		//time
		WCHAR	szTimeA[ SCUI_FILEDATETIME_STRLEN * 2] = {0};
		WCHAR	szTime[ SCUI_FILEDATETIME_STRLEN * 2] = {0};
		if(pszTime == NULL)
		{
			GetLocalTimeStr(szTimeA);
			//convert_wchar(szTime, szTimeA,sizeof(szTime));
		}
		else
		{
			memcpy(szTimeA,pszTime,sizeof(szTimeA));
		}

		m_pParent->WriteJsonStrData(pszKey, JSON_ENT_US_TIME, szTimeA, sizeof(szTimeA));

		//self flag
		dwData = (DWORD)bSelf;
		m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_SELF, dwData);
	}

	// --- wFieldWMD --- DM_WM_DEC_CIRCLE
	dwData = (*pwmd).wFieldWMD & DM_WM_DEC_CIRCLE ? 1 : 0;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_DEC_CIRCLE, dwData);

	// --- wFieldWMD --- DM_WM_DEC_SQUARE
	dwData = (*pwmd).wFieldWMD & DM_WM_DEC_SQUARE ? 1 : 0;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_DEC_SQUARE, dwData);

	// --- wFieldWMD --- DM_WM_FIRSTPAGEONLY
	dwData = (*pwmd).wFieldWMD & DM_WM_FIRSTPAGEONLY ? 1 : 0;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FIRSTPAGEONLY, dwData);

	// --- wFieldWMD --- DM_WM_TRANSPARENT
	dwData = (*pwmd).wFieldWMD & DM_WM_TRANSPARENT ? 1 : 0;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_TRANSPARENT, dwData);

	// --- wFieldWMD --- DM_WM_OUTLINE
	dwData = (*pwmd).wFieldWMD & DM_WM_OUTLINE ? 1 : 0;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_OUTLINE, dwData);

	// --- wFieldWMD --- DM_WM_TRANSPARENT2
	dwData = (*pwmd).wFieldWMD & DM_WM_TRANSPARENT2 ? 1 : 0;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_TRANSPARENT2, dwData);

	// --- wFieldWMD --- DM_WM_PILE
	dwData = (*pwmd).wFieldWMD & DM_WM_PILE ? 1 : 0;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_PILE, dwData);

	// --- colorRef
	dwData = (DWORD)(*pwmd).colorRef;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_COLORREF, dwData);

#if	1	// ƒƒ\ƒbƒh‰»‚·‚éB
	CalcGrayscale(pwmd);
#else	//
// LCID‚ðƒ`ƒFƒbƒN‚µAƒfƒtƒHƒ‹ƒgƒvƒŠƒ“ƒ^[‚Ì’l‚ð•ÏX‚·‚é‘Î‰ž
// GetLong(MCF_SEC_DEFAULTPRINTER, -> GetLongDPLCID(
	wR = (SHORT)(*m_pmcf).GetLongDPLCID(MCF_KEY_DEF_C_NGBRED);
	wG = (SHORT)(*m_pmcf).GetLongDPLCID(MCF_KEY_DEF_C_NGBGREEN);
	wB = (SHORT)(*m_pmcf).GetLongDPLCID(MCF_KEY_DEF_C_NGBBLUE);

	cR = GetRValue((*pwmd).colorRef);
	cG = GetGValue((*pwmd).colorRef);
	cB = GetBValue((*pwmd).colorRef);

	sum = wR + wG + wB;

	if (sum != 0)
	{
		(*pwmd).nGrayscale = ((unsigned short)cR * wR +
						  (unsigned short)cG * wG +
					 	  (unsigned short)cB * wB) / sum;
	}
	else
	{
		// ‚ ‚è‚¦‚È‚¢‘g‚Ý‡‚í‚¹‚Ì‚½‚ßA‰½‚à‚µ‚È‚¢B
	}

	if( 255 < (*pwmd).nGrayscale )
		(*pwmd).nGrayscale = 255;
#endif	//

	//// --- nGrayscale
	//dwData = (DWORD)(*pwmd).nGrayscale;
	//m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_NGRAYSCALE, dwData);

	// --- dwPosX
	dwData = (DWORD)(*pwmd).dwPosX;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_DWPOSX, dwData);
	
	// --- dwPosY
	dwData = (DWORD)(*pwmd).dwPosY;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_DWPOSY, dwData);

	// --- nSize
	dwData = (DWORD)(*pwmd).nSize;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_NSIZE, dwData);

	// --- nAngle
	dwData = (DWORD)(*pwmd).nAngle;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_NANGLE, dwData);

#if SCUIDATA_VER >= 0x0004
	// --- wBorderSize
	dwData = (DWORD)(*pwmd).wBorderSize;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_WBORDERSIZE, dwData);
#endif

	// --- szMainWM
	m_pParent->WriteJsonStrData(pszKey, JSON_ENT_WM_SZMAINWM, (WCHAR FAR *)(*pwmd).szMainWM, (DWORD)wcslen((*pwmd).szMainWM) * wBy);

	// --- LGCFONT --- lfHeight
	dwData = (DWORD)(*pwmd).lfWatermark.lfHeight;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFHEIGHT, dwData);

	// --- LGCFONT --- lfWidth
	dwData = (DWORD)(*pwmd).lfWatermark.lfWidth;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFWIDTH, dwData);

	// --- LGCFONT --- lfEscapement
	dwData = (DWORD)(*pwmd).lfWatermark.lfEscapement;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFESC, dwData);

	// --- LGCFONT --- lfOrientation
	dwData = (DWORD)(*pwmd).lfWatermark.lfOrientation;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFORI, dwData);

	// --- LGCFONT --- lfWeight
	dwData = (DWORD)(*pwmd).lfWatermark.lfWeight;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFWEIGHT, dwData);

	// --- LGCFONT --- lfItalic
	dwData = (DWORD)(*pwmd).lfWatermark.lfItalic;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFITALIC, dwData);

	// --- LGCFONT --- lfUnderline
	dwData = (DWORD)(*pwmd).lfWatermark.lfUnderline;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFUNDERLINE, dwData);

	// --- LGCFONT --- lfStrikeOut
	dwData = (DWORD)(*pwmd).lfWatermark.lfStrikeOut;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFSTRIKEOUT, dwData);

	// --- LGCFONT --- lfCharSet
	dwData = (DWORD)(*pwmd).lfWatermark.lfCharSet;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFCHARSET, dwData);

	// --- LGCFONT --- lfOutPrecision
	dwData = (DWORD)(*pwmd).lfWatermark.lfOutPrecision;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFOUTPRE, dwData);

	// --- LGCFONT --- lfClipPrecision
	dwData = (DWORD)(*pwmd).lfWatermark.lfClipPrecision;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFCLIPPRE, dwData);

	// --- LGCFONT --- lfQuality
	dwData = (DWORD)(*pwmd).lfWatermark.lfQuality;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFQUALITY, dwData);

	// --- LGCFONT --- lfPitchAndFamily
	dwData = (DWORD)(*pwmd).lfWatermark.lfPitchAndFamily;
	m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFPANDF, dwData);

	// --- LGCFONT --- lfFaceName
	m_pParent->WriteJsonStrData(pszKey, JSON_ENT_WM_FNT_LFFACENAME, (WCHAR FAR *)(*pwmd).lfWatermark.lfFaceName, (DWORD)wcslen((*pwmd).lfWatermark.lfFaceName) * wBy);

	blRet = TRUE;


EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	if(pszBase != NULL)
		delete[] pszBase;

	return blRet;
}


//=============================================================================
// function
//      WriteWMCount
//
// parameters
//      dwCount             ‘‚«ž‚Þ’l
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      Watermark”‚ð‘‚«ž‚Þ
//=============================================================================
BOOL CShJsonWm::WriteWMCount(DWORD dwCount)
{
	BOOL			blRet = FALSE;

	DWORD			dwData = 0;

//	// ================================================
//	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
//	// ================================================
//	if(pszSvrPrnName == NULL)
//		goto EXIT;
//
	// ================================================
	// _/_/_/  ‘‚«ž‚Ý
	// ================================================

	// --- count
	dwData = dwCount;
	if(m_pParent != NULL)
		blRet = m_pParent->WriteJsonDWORDData(JSON_KEY_WM_ROOT_BASE, JSON_ENT_WM_COUNT, dwData);

	return blRet;
}

BOOL CShJsonWm::WriteColorData(WCHAR *pData)
{
	BOOL			blRet = FALSE;

	//DWORD			dwData = 0;

	//	// ================================================
	//	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	//	// ================================================
	//	if(pszSvrPrnName == NULL)
	//		goto EXIT;
	//
		// ================================================
		// _/_/_/  ‘‚«ž‚Ý
		// ================================================

		// --- count
	//dwData = pData;
	if (m_pParent != NULL)
		blRet = m_pParent->WriteJsonBinaryData(JSON_KEY_WM_ROOT_BASE, JSON_ENT_WM_CUSTOM_COLOR, (CONST WCHAR *)pData, sizeof(pData));

	return blRet;
}

//=============================================================================
// function
//      ReadWMData
//
// parameters
//      lIndex              “Ç‚ÝŽæ‚éƒCƒ“ƒfƒbƒNƒX
//      pwmd                “Ç‚ÝŽæ‚éWATERMARKDATA\‘¢‘ÌƒAƒhƒŒƒX
//      dwTitleSize         “Ç‚ÝŽæ‚éWATERMARKDATA\‘¢‘Ì‚Ìƒ^ƒCƒgƒ‹ƒTƒCƒY
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      Žw’è‚³‚ê‚½ƒCƒ“ƒfƒbƒNƒX‚ÌWatermarkî•ñ‚ð‚ÅƒŒƒWƒXƒgƒŠ‚©‚ç“Ç‚Ýž‚Þ
//=============================================================================
BOOL CShJsonWm::ReadWMData(long lIndex, LPWATERMARKDATA pwmd, WCHAR FAR *pszTitle, DWORD dwTitleSize)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*pszBase = NULL;

	WCHAR FAR		*psz = NULL;

	DWORD			dwData = 0;
	DWORD			dwSize = 0;

//	// ================================================
//	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
//	// ================================================
	//if(pszSvrPrnName == NULL)
	//	goto EXIT;

	if(ReadCount() < (DWORD)lIndex + 1)
		goto EXIT;
//
//	// ================================================
//	// _/_/_/  WATERMARKDATAŠm’è
//	// ================================================
	if(pwmd == NULL)
		goto EXIT;

	if(m_pParent == NULL)
		goto EXIT;
	
	SecureZeroMemory(pwmd, sizeof(WATERMARKDATA));

//
//	// ================================================
//	// _/_/_/  “Ç‚Ýž‚Ý
//	// ================================================
//	// --- dwSignature
	psz = (WCHAR FAR *)&(*pwmd).dwSignature;
	*psz = 'W';
	psz++;
	*psz = 'M';
	psz++;
	*psz = 'D';
	psz++;
	*psz = 'T';
//
	pszBase = new WCHAR[SCUI_REGKEYSIZE];
	if(pszBase == NULL)
		goto EXIT;

	SecureZeroMemory(pszBase, SCUI_REGKEYSIZE);

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_INDEX_BASE, lIndex);

	// --- Title
	if(pszTitle != NULL && dwTitleSize != 0)
	{
		dwSize = dwTitleSize;
		m_pParent->ReadJsonStrData(pszKey, JSON_ENT_WM_TITLE, pszTitle, dwSize);
	}

	// --- wFieldWMD --- DM_WM_UNICODE
	if(IsUnicodeOS())
		(*pwmd).wFieldWMD |= DM_WM_UNICODE;

	// --- wFieldWMD --- DM_WM_DEC_CIRCLE
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_DEC_CIRCLE, dwData);
	(*pwmd).wFieldWMD = dwData != 0 ? (*pwmd).wFieldWMD | DM_WM_DEC_CIRCLE : (*pwmd).wFieldWMD;

	// --- wFieldWMD --- DM_WM_DEC_SQUARE
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_DEC_SQUARE, dwData);
	(*pwmd).wFieldWMD = dwData != 0 ? (*pwmd).wFieldWMD | DM_WM_DEC_SQUARE : (*pwmd).wFieldWMD;

	// --- wFieldWMD --- DM_WM_FIRSTPAGEONLY
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FIRSTPAGEONLY, dwData);
	(*pwmd).wFieldWMD = dwData != 0 ? (*pwmd).wFieldWMD | DM_WM_FIRSTPAGEONLY : (*pwmd).wFieldWMD;

	// --- wFieldWMD --- DM_WM_TRANSPARENT
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_TRANSPARENT, dwData);
	(*pwmd).wFieldWMD = dwData != 0 ? (*pwmd).wFieldWMD | DM_WM_TRANSPARENT : (*pwmd).wFieldWMD;

	// --- wFieldWMD --- DM_WM_OUTLINE
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_OUTLINE, dwData);
	(*pwmd).wFieldWMD = dwData != 0 ? (*pwmd).wFieldWMD | DM_WM_OUTLINE : (*pwmd).wFieldWMD;

	// --- wFieldWMD --- DM_WM_TRANSPARENT2
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_TRANSPARENT2, dwData);
	(*pwmd).wFieldWMD = dwData != 0 ? (*pwmd).wFieldWMD | DM_WM_TRANSPARENT2 : (*pwmd).wFieldWMD;

	// --- wFieldWMD --- DM_WM_PILE
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_PILE, dwData);
	(*pwmd).wFieldWMD = dwData != 0 ? (*pwmd).wFieldWMD | DM_WM_PILE : (*pwmd).wFieldWMD;

	// --- colorRef
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_COLORREF, dwData);
	(*pwmd).colorRef = (COLORREF)dwData;

	//// --- nGrayscale
	//dwSize = sizeof(dwData);
	//m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_NGRAYSCALE, dwData);
	//(*pwmd).nGrayscale = (SHORT)dwData;

	// --- dwPosX
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_DWPOSX, dwData);
	(*pwmd).dwPosX = (LONG)dwData;

	// --- dwPosY
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_DWPOSY, dwData);
	(*pwmd).dwPosY = (LONG)dwData;

	// --- nSize
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_NSIZE, dwData);
	(*pwmd).nSize = (SHORT)dwData;

	// --- nAngle
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_NANGLE, dwData);
	(*pwmd).nAngle = (SHORT)dwData;

#if SCUIDATA_VER >= 0x0004
	// --- wBorderSize
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_WBORDERSIZE, dwData);
	(*pwmd).wBorderSize = (WORD)dwData;
#endif

	// --- szMainWM
	dwSize = sizeof((*pwmd).szMainWM);
	m_pParent->ReadJsonStrData(pszKey, JSON_ENT_WM_SZMAINWM, (WCHAR FAR *)(*pwmd).szMainWM, dwSize);

	// --- LGCFONT --- lfHeight
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFHEIGHT, dwData);
	(*pwmd).lfWatermark.lfHeight = (LONG)dwData;

	// --- LGCFONT --- lfWidth
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFWIDTH, dwData);
	(*pwmd).lfWatermark.lfWidth = (LONG)dwData;

	// --- LGCFONT --- lfEscapement
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFESC, dwData);
	(*pwmd).lfWatermark.lfEscapement = (LONG)dwData;

	// --- LGCFONT --- lfOrientation
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFORI, dwData);
	(*pwmd).lfWatermark.lfOrientation = (LONG)dwData;

	// --- LGCFONT --- lfWeight
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFWEIGHT, dwData);
	(*pwmd).lfWatermark.lfWeight = (LONG)dwData;

	// --- LGCFONT --- lfItalic
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFITALIC, dwData);
	(*pwmd).lfWatermark.lfItalic = (BYTE)dwData;

	// --- LGCFONT --- lfUnderline
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFUNDERLINE, dwData);
	(*pwmd).lfWatermark.lfUnderline = (BYTE)dwData;

	// --- LGCFONT --- lfStrikeOut
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFSTRIKEOUT, dwData);
	(*pwmd).lfWatermark.lfStrikeOut = (BYTE)dwData;

	// --- LGCFONT --- lfCharSet
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFCHARSET, dwData);
	(*pwmd).lfWatermark.lfCharSet = (BYTE)dwData;

	// --- LGCFONT --- lfOutPrecision
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFOUTPRE, dwData);
	(*pwmd).lfWatermark.lfOutPrecision = (BYTE)dwData;

	// --- LGCFONT --- lfClipPrecision
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFCLIPPRE, dwData);
	(*pwmd).lfWatermark.lfClipPrecision = (BYTE)dwData;

	// --- LGCFONT --- lfQuality
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFQUALITY, dwData);
	(*pwmd).lfWatermark.lfQuality = (BYTE)dwData;

	// --- LGCFONT --- lfPitchAndFamily
	dwSize = sizeof(dwData);
	m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_FNT_LFPANDF, dwData);
	(*pwmd).lfWatermark.lfPitchAndFamily = (BYTE)dwData;

	// --- LGCFONT --- lfFaceName
	dwSize = sizeof((*pwmd).lfWatermark.lfFaceName);
	m_pParent->ReadJsonStrData(pszKey, JSON_ENT_WM_FNT_LFFACENAME, (WCHAR FAR *)(*pwmd).lfWatermark.lfFaceName, dwSize);

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	if(pszBase != NULL)
		delete[] pszBase;

	return blRet;
return TRUE;
}


//=============================================================================
// function
//      ReadCount
//
// parameters
//
// return value
//      ¬Œ÷ : “o˜^Watermark”
//      Ž¸”s : 0
//
// outline
//      Watermark”‚ð“Ç‚Ýž‚Þ
//=============================================================================
DWORD CShJsonWm::ReadCount()
{
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	//if(pszSvrPrnName == NULL)
	//	goto EXIT;
	
	// --- count
	if(m_pParent != NULL)
		blSuccess = m_pParent->ReadJsonDWORDData(JSON_KEY_WM_ROOT_BASE, JSON_ENT_WM_COUNT, dwData);

	return dwData;
}

BOOL CShJsonWm::ReadColor(WCHAR *pData, DWORD pdwSize)
{
	BOOL			blSuccess = FALSE;
	WCHAR *			dwData = 0;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	//if(pszSvrPrnName == NULL)
	//	goto EXIT;

	// --- count
	if (m_pParent != NULL)
		blSuccess = m_pParent->ReadJsonBinaryData(JSON_KEY_WM_ROOT_BASE, JSON_ENT_WM_CUSTOM_COLOR, pData, pdwSize);

	return blSuccess;
}


//=============================================================================
// function
//      DeleteData
//
// parameters
//      wIndex              íœ‚·‚éWatermarkƒCƒ“ƒfƒbƒNƒX
//
// return value
//      ‚È‚µ
//
// outline
//      Žw’è‚ÌƒCƒ“ƒfƒbƒNƒX‚ÌWatermarkƒf[ƒ^‚ðíœ‚·‚é
//=============================================================================
void CShJsonWm::DeleteData(short wIndex)
{
	if(m_pParent != NULL)
		m_pParent->DeleteChildItemData(JSON_KEY_WM_ROOT_BASE,wIndex);

	return;
}


//=============================================================================
// function
//      GetCountIni
//
// parameters
//      pIniFile            CShIniFile ƒNƒ‰ƒXƒAƒhƒŒƒX
//
// return value
//      ¬Œ÷ : INIƒtƒ@ƒCƒ‹‚©‚çŽæ“¾‚µ‚½ŒÂ”
//      Ž¸”s : 0
//
// outline
//      INIƒtƒ@ƒCƒ‹‚ÌƒfƒtƒHƒ‹ƒgWatermarkŒÂ”‚ðŽæ“¾‚·‚é
//=============================================================================
DWORD CShJsonWm::GetCountIni(CShIniFile FAR *pIniFile)
{
	DWORD			dwRet = 0;

	CShIniFile FAR	*pini = NULL;
	WCHAR			szWaterMarkLangSection[MAX_PATH] = {};
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	// ================================================
	// _/_/_/  INIƒtƒ@ƒCƒ‹ƒNƒ‰ƒXŠm’è
	// ================================================
	if(pIniFile != NULL)
		pini = pIniFile;
	else
	{
	slog.putLog("CWaterMarkRegistryData::GetCountIni -> NULL_POINTER(pIniFile)\n");
	}

	// ================================================
	// _/_/_/  ŒÂ”‚ÌŽæ“¾
	// ================================================
	dwRet = (*pini).GetLong(LANGD_SEC_WMDEFAULT, LANGD_KEY_WMDEF_COUNT);

	::swprintf_s(szWaterMarkLangSection, MAX_PATH, L"%s%d", LANGD_SEC_WMDEFAULT, m_wLanguageID);

	if (NULL != pini)
		dwRet = pini->GetLong(szWaterMarkLangSection, LANGD_KEY_WMDEF_COUNT);
	return dwRet;

}


//=============================================================================
// function
//      GetWMDefData
//
// parameters
//      lIndex              î•ñ‚ðŽæ“¾‚·‚éƒCƒ“ƒfƒbƒNƒX
//      pwmd                î•ñ‚ðŽó‚¯Žæ‚éWATERMARKDATA\‘¢‘ÌƒAƒhƒŒƒX
//      pIniFile            CShIniFile ƒNƒ‰ƒXƒAƒhƒŒƒX
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      Žw’è‚³‚ê‚½INIƒtƒ@ƒCƒ‹‚ÌƒfƒtƒHƒ‹ƒgWatermark‚ðŽæ“¾‚·‚é
//=============================================================================
BOOL CShJsonWm::GetWMDefData(long lIndex, LPWATERMARKDATA pwmd, CShIniFile FAR *pIniFile)
{
	BOOL			blRet = FALSE;

	long			lCount;
	long			lPos;
	long			lVal;

	short			wRange;

	BYTE			r, g, b;
	WCHAR			szData[384];

	WCHAR FAR		*pc = NULL;
	WCHAR			szWaterMarkLangSection[MAX_PATH] = { 0 };
	long FAR		*pl = NULL;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
// WM.dat‚Ì“à—e‚ÅLocalize‘ÎÛ‚ÌƒL[‚ðDLL‚ÉˆÚs‚·‚é‘Î‰ž
#ifdef WMCONFIGFILE_TO_RES_DLL
	long			*plData = NULL;
	long			dwOffset = 0;
	WCHAR			szDataW[384] = {0};
#endif

	CShIniFile FAR	*pini = NULL;

	short			i;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	if(pwmd == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  INIƒtƒ@ƒCƒ‹ƒNƒ‰ƒXŠm’è
	// ================================================
	if(pIniFile != NULL)
		pini = pIniFile;
	else
	{
	/*	if(CreateObj(scui_obj_ini_lang) == FALSE)
			goto EXIT;*/
		
		pini = m_cshIniFile;
		DWORD dwSize = 0;
		dwSize = sizeof(m_wLanguageID);
		WCHAR pszKey[SCUI_REGKEYSIZE] = { 0 };
		CreateRegPathEx(REG_KEY_HKCU_DI_BASE, pszKey, SCUI_REGKEYSIZE, m_pszSvrPrnName);
		ReadRegData(HKEY_CURRENT_USER, pszKey, REG_ENT_LANGID, (WCHAR *)&m_wLanguageID, &dwSize);
		::swprintf_s(szWaterMarkLangSection, MAX_PATH, L"%s%d", LANGD_SEC_WMDEFAULT, m_wLanguageID);
	}

	// ================================================
	// _/_/_/  î•ñ‚ÌŽæ“¾
	// ================================================
//	if((lCount = (*pini).GetLongArray(LANGD_SEC_WMDEFAULT, lIndex)) == 0)
//// WM.dat‚Ì“à—e‚ÅLocalize‘ÎÛ‚ÌƒL[‚ðDLL‚ÉˆÚs‚·‚é‘Î‰ž
//#ifdef WMCONFIGFILE_TO_RES_DLL
//	{
////		if (LoadStr(wID, szText, sizeof(szText))) // DLL ƒŠƒ\[ƒX‚©‚ç“¯“™‚Ì‚à‚Ì‚ðREAD‚·‚éB
//		dwOffset = (*pini).GetLong(LANGD_SEC_WMDEFAULT, LANGD_KEY_WMDEF_OFFSET, 0);
//
//		plData = new long[SCUI_LONGDATA_MAX];
//		SecureZeroMemory(plData, sizeof(long) * SCUI_LONGDATA_MAX);
////		if (LoadStrArray(IDS_WATERMARK_RES_BASE + lIndex, szData, sizeof(szData), plData, SCUI_LONGDATA_MAX)==0) // DLL ƒŠƒ\[ƒX‚©‚ç“¯“™‚Ì‚à‚Ì‚ðREAD‚·‚éB
//	//To fix :Default WaterMark values were not updating on the UI 
//	//	if (LoadStrArray(IDS_WATERMARK_RES_BASE + lIndex + dwOffset, szData, sizeof(szData), plData, SCUI_LONGDATA_MAX)==0) // DLL ƒŠƒ\[ƒX‚©‚ç“¯“™‚Ì‚à‚Ì‚ðREAD‚·‚éB
//		if (LoadStrArray(IDS_WATERMARK_RES_BASE + lIndex + dwOffset, szDataW, sizeof(szDataW), plData, SCUI_LONGDATA_MAX)==0) // DLL ƒŠƒ\[ƒX‚©‚ç“¯“™‚Ì‚à‚Ì‚ðREAD‚·‚éB
//			goto EXIT;
//
//		SecureZeroMemory(szData, sizeof(szData));
////		LoadStr(IDS_WATERMARK_RES_BASE + lIndex, szData, sizeof(szData), IsUnicode());
//		LoadStr(IDS_WATERMARK_RES_BASE + lIndex + dwOffset, szData, sizeof(szData), IsUnicode());
//		pl = plData;
//		pl[22] = GetCharSet();
//	}
//	else
//	{
//		pl = (*pini).GetLongArrayData();
//		if(pl == NULL)
//			goto EXIT;
//
//		SecureZeroMemory(szData, sizeof(szData));
//		if((*pini).GetString(LANGD_SEC_WMDEFAULT, lIndex, "", szData, _countof(szData), IsUnicode()) == 0)
//			goto EXIT;
//	}
//#endif

	if ((lCount = (*pini).GetLongArray(szWaterMarkLangSection, lIndex)) == 0)
		return blRet;

	pl = (*pini).GetLongArrayData();
	if (nullptr == pl)
	{
		slog.putLog("CWaterMarkRegistryData::GetWMDefData -> NULL_POINTER\n");
		return blRet;
	}

	SecureZeroMemory(szData, sizeof(szData));
	//if((*pini).GetString(LANGD_SEC_WMDEFAULT, lIndex, "", szData, sizeof(szData), IsUnicode()) == 0)
	if ((*pini).GetString(szWaterMarkLangSection, lIndex, L"", szData, sizeof(szData) / sizeof(WCHAR)) == 0)
		return blRet;

	//if(IsUnicodeOS() != FALSE)
	//	wRange = 2;
	//else
	//	wRange = 1;

	// ================================================
	// _/_/_/  î•ñ‚Ìì¬
	// ================================================
	SecureZeroMemory(pwmd, sizeof(WATERMARKDATA));
	lPos = 0;

	// --- dwSignature
	pc = (WCHAR FAR *)&(*pwmd).dwSignature;
	*pc = 'W';
	pc++;
	*pc = 'M';
	pc++;
	*pc = 'D';
	pc++;
	*pc = 'T';

	// --- wFieldWMD --- DM_WM_UNICODE
	if(IsUnicodeOS())
		(*pwmd).wFieldWMD |= DM_WM_UNICODE;

	// --- wFieldWMD --- DM_WM_DEC_CIRCLE
	if(pl[lPos++] != 0)
		(*pwmd).wFieldWMD |= DM_WM_DEC_CIRCLE;

	// --- wFieldWMD --- DM_WM_DEC_SQUARE
	if(pl[lPos++] != 0)
		(*pwmd).wFieldWMD |= DM_WM_DEC_SQUARE;

	// --- wFieldWMD --- DM_WM_FIRSTPAGEONLY
	if(pl[lPos++] != 0)
		(*pwmd).wFieldWMD |= DM_WM_FIRSTPAGEONLY;

	// --- wFieldWMD --- DM_WM_TRANSPARENT
// ---------------------------
// Printing Pattern
//	0:none
//	1:Transparent1
//	2:Transparent2
//	3:Overprinting
// ---------------------------
//	if(pl[lPos++] != 0)
//		(*pwmd).wFieldWMD |= DM_WM_TRANSPARENT;
	if(pl[lPos++] != 0)
	{
		switch(pl[lPos-1])
		{
		case 1:			// Transparent1
			(*pwmd).wFieldWMD |= DM_WM_TRANSPARENT;
			break;
		case 2:			// Transparent2
			(*pwmd).wFieldWMD |= DM_WM_TRANSPARENT2;
			break;
		case 3:			// Overprinting
			(*pwmd).wFieldWMD |= DM_WM_PILE;
			break;
		default:
			break;
		}
	}

	// --- wFieldWMD --- DM_WM_OUTLINE
	if(pl[lPos++] != 0)
		(*pwmd).wFieldWMD |= DM_WM_OUTLINE;

	// --- colorRef
	r = (BYTE)pl[lPos++];
	g = (BYTE)pl[lPos++];
	b = (BYTE)pl[lPos++];

	(*pwmd).colorRef = RGB(r, g, b);

	lPos++;
#if 0
	(*pwmd).nGrayscale = (SHORT)pl[lPos++];
#endif

	// --- dwPosX
	(*pwmd).dwPosX = (DWORD)pl[lPos++];

	// --- dwPosY
	(*pwmd).dwPosY = (DWORD)pl[lPos++];

	// --- nSize
	(*pwmd).nSize = (SHORT)pl[lPos++];

	// --- nAngle
	(*pwmd).nAngle = (SHORT)pl[lPos++];

#if SCUIDATA_VER >= 0x0004
	// --- wBorderSize
	(*pwmd).wBorderSize = (WORD)pl[lPos++];
#endif

	// --- LGCFONT --- lfHeight
	lVal = pl[lPos++];
	if(lVal == 0)
	{
		HDC				hDC = NULL;
		HWND			hWnd = NULL;

		hWnd = ::GetDesktopWindow();
		hDC = ::GetDC(hWnd);
		(*pwmd).lfWatermark.lfHeight = -MulDiv((*pwmd).nSize, ::GetDeviceCaps(hDC, LOGPIXELSY), SCUI_POINTSPERINCH);
		ReleaseDC(hWnd, hDC);
	}
	else
		(*pwmd).lfWatermark.lfHeight = lVal;

	// --- LGCFONT --- lfWidth
	(*pwmd).lfWatermark.lfWidth = pl[lPos++];

	// --- LGCFONT --- lfEscapement
	(*pwmd).lfWatermark.lfEscapement = pl[lPos++];

	// --- LGCFONT --- lfOrientation
	(*pwmd).lfWatermark.lfOrientation = pl[lPos++];

	// --- LGCFONT --- lfWeight
	(*pwmd).lfWatermark.lfWeight = pl[lPos++];

	// --- LGCFONT --- lfItalic
	(*pwmd).lfWatermark.lfItalic = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfUnderline
	(*pwmd).lfWatermark.lfUnderline = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfStrikeOut
	(*pwmd).lfWatermark.lfStrikeOut = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfCharSet
	(*pwmd).lfWatermark.lfCharSet = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfOutPrecision
	(*pwmd).lfWatermark.lfOutPrecision = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfClipPrecision
	(*pwmd).lfWatermark.lfClipPrecision = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfQuality
	(*pwmd).lfWatermark.lfQuality = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfPitchAndFamily
	(*pwmd).lfWatermark.lfPitchAndFamily = (BYTE)pl[lPos++];

	// --- szMainWM
	pc = wcsrchr(szData, L',');
	if (nullptr == pc)
	{
		slog.putLog("CWaterMarkRegistryData::GetWMDefData -> NULL_POINTER\n");
		return blRet;
	}

	//for(i = 0; i < wRange; i++)
	//{
	//	*pc = '\0';
	//	pc++;
	//}
	*pc = L'\0';
	pc++;

	if(lIndex != 0)
		wcsncpy_s((*pwmd).szMainWM, _countof((*pwmd).szMainWM), pc, WM_STR_LENGTH);
	else
	{
		//LoadDll(scui_dll_str);
		//LoadString(IDS_INDEX0_WATERMARK, (*pwmd).szMainWM, sizeof((*pwmd).szMainWM), TRUE);
		LoadString(m_hStringResourceHandle, IDS_INDEX0_WATERMARK, (*pwmd).szMainWM, sizeof((*pwmd).szMainWM) / sizeof(WCHAR));
	}

	// --- LGCFONT --- lfFaceName
	pc = wcsrchr(szData, L',');
	if (nullptr == pc)
	{
		slog.putLog("CWaterMarkRegistryData::GetWMDefData -> NULL_POINTER\n");
		return blRet;
	}

	pc ++;

	wcsncpy_s((*pwmd).lfWatermark.lfFaceName, _countof((*pwmd).lfWatermark.lfFaceName), pc, LGCF_FACESIZE*2);

	blRet = TRUE;

EXIT:
// WM.dat‚Ì“à—e‚ÅLocalize‘ÎÛ‚ÌƒL[‚ðDLL‚ÉˆÚs‚·‚é‘Î‰ž
#ifdef WMCONFIGFILE_TO_RES_DLL
	if (plData != NULL)
	{
		delete[] plData;
		plData = NULL;
	}
#endif

	return blRet;
}


// for Watermark Jobinfo
#ifdef _WIN32
//=============================================================================
// function
//      GetJobInfoSetting
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      JobInfoˆóüÝ’è‚ðŽæ“¾‚·‚é
//=============================================================================
DWORD CShJsonWm::GetJobInfoSetting(WCHAR *pszSvrPrnName, WCHAR *pszEntry)
{
	DWORD			dwRet = 0;
	DWORD			dwSize;
	DWORD			dwType;

	WCHAR FAR		*pszKey = NULL;

	HANDLE			hPrinter = NULL;
	PRINTER_DEFAULTS	pd = {NULL, NULL, PRINTER_ACCESS_USE};

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_ROOT_BASE);
	
	// ================================================
	// _/_/_/  ŠÇ—Œ ŒÀ‚ðŽ‚Á‚Ä‚¢‚éê‡‚ÍAƒ[ƒJƒ‹
	// _/_/_/  ‚Å“ÆŽ©‚ÌÝ’è‚ð•ÛŽ‚Å‚«‚é
	// ================================================
	//if(IsWatermarkJobInfoSUser(pszSvrPrnName) != FALSE)
	//{
	//	dwSize = sizeof(dwRet);
	//	if(m_pParent->ReadJsonDWORDData(pszKey, pszEntry, dwRet) != FALSE)
	//		goto EXIT;
	//}//bear

	// ================================================
	// _/_/_/  ƒT[ƒo[‚©‚çŽæ“¾
	// ================================================
	if(sh_OpenPrinter(pszSvrPrnName, &hPrinter, &pd) == FALSE)
		goto EXIT;

	dwType = REG_DWORD;
	dwSize = sizeof(dwRet);
	::GetPrinterData(hPrinter, pszEntry, &dwType, (LPBYTE)&dwRet, sizeof(dwRet), &dwSize);

EXIT:

	if(hPrinter != NULL)
		::ClosePrinter(hPrinter);

	if(pszKey != NULL)
		delete[] pszKey;

	return dwRet;
}


//=============================================================================
// function
//      GetJobInfoCustomString
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      pStr                •¶Žš—ñ‚ðŽó‚¯Žæ‚éƒoƒbƒtƒ@
//      dwStrSize           pStrƒTƒCƒY
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      JobInfo—pAƒJƒXƒ^ƒ€•¶Žš—ñ‚ðŽæ“¾‚·‚é
//=============================================================================
BOOL CShJsonWm::GetJobInfoCustomString(WCHAR *pszSvrPrnName, WCHAR *pStr, DWORD dwStrSize)
{
	BOOL			blRet = FALSE;
	DWORD			dwSize;
	DWORD			dwType;

	WCHAR			szEntry[512];

	WCHAR FAR		*pszKey = NULL;

	HANDLE			hPrinter = NULL;
	PRINTER_DEFAULTS	pd = {NULL, NULL, PRINTER_ACCESS_USE};

	if(pStr == NULL || dwStrSize == 0)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_ROOT_BASE);
	
	// ================================================
	// _/_/_/  ŠÇ—Œ ŒÀ‚ðŽ‚Á‚Ä‚¢‚éê‡‚ÍAƒ[ƒJƒ‹
	// _/_/_/  ‚Å“ÆŽ©‚ÌÝ’è‚ð•ÛŽ‚Å‚«‚é
	// ================================================
	//if(IsWatermarkJobInfoSUser(pszSvrPrnName) != FALSE)
	//{
	//	dwSize = dwStrSize;
	//	if(	m_pParent->ReadJsonStrData(pszKey, JSON_ENT_WM_JOBINFO_WDM, pStr, dwSize) != FALSE)
	//	{
	//		blRet = TRUE;
	//		goto EXIT;
	//	}
	//}bear

	// ================================================
	// _/_/_/  ƒT[ƒo[‚©‚çŽæ“¾
	// ================================================
	if(sh_OpenPrinter(pszSvrPrnName, &hPrinter, &pd) == FALSE)
		goto EXIT;

	dwType = REG_SZ;
	dwSize = dwStrSize;

	::ZeroMemory(szEntry, sizeof(szEntry));
	//convert_wchar(szEntry, LANGD_KEY_WMDEF_JI_STRING, sizeof(szEntry));
	//bearsh_GetPrinterData(pszSvrPrnName, szEntry, &dwType, (LPBYTE)pStr, dwSize, &dwSize);
//	::GetPrinterData(hPrinter, LANGD_KEY_WMDEF_JI_STRING, &dwType, (LPBYTE)pStr, dwSize, &dwSize);
	
	blRet = TRUE;

EXIT:

	if(hPrinter != NULL)
		::ClosePrinter(hPrinter);

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      GetJobInfoWATERMARKDATA
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      pwmd                “Ç‚ÝŽæ‚éWATERMARKDATA\‘¢‘ÌƒAƒhƒŒƒX
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      JobInfo—pAWatermarkƒf[ƒ^‚ðŽæ“¾‚·‚é
//=============================================================================
BOOL CShJsonWm::GetJobInfoWATERMARKDATA(WCHAR *pszSvrPrnName, WATERMARKDATA *pwmd)
{
	BOOL			blRet = FALSE;
	DWORD			dwSize;
	DWORD			dwType;

	WCHAR			szEntry[512];

	WCHAR FAR		*pszKey = NULL;

	HANDLE			hPrinter = NULL;
	PRINTER_DEFAULTS	pd = {NULL, NULL, PRINTER_ACCESS_USE};

	if(pszSvrPrnName == NULL || pwmd == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_ROOT_BASE);
	
	// ================================================
	// _/_/_/  ŠÇ—Œ ŒÀ‚ðŽ‚Á‚Ä‚¢‚éê‡‚ÍAƒ[ƒJƒ‹
	// _/_/_/  ‚Å“ÆŽ©‚ÌÝ’è‚ð•ÛŽ‚Å‚«‚é
	// ================================================
	//if(IsWatermarkJobInfoSUser(pszSvrPrnName) != FALSE)
	//{
	//	dwSize = sizeof(WATERMARKDATA);
	//	if(m_pParent->ReadJsonBinaryData(pszKey, JSON_ENT_WM_JOBINFO_WDM, (WCHAR*)pwmd, dwSize) != FALSE)
	//	{
	//		blRet = TRUE;
	//		goto EXIT;
	//	}
	//}//bear

	// ================================================
	// _/_/_/  ƒT[ƒo[‚©‚çŽæ“¾
	// ================================================
	if(sh_OpenPrinter(pszSvrPrnName, &hPrinter, &pd) == FALSE)
		goto EXIT;

	dwType = REG_BINARY;
	dwSize = sizeof(WATERMARKDATA);

	::ZeroMemory(szEntry, sizeof(szEntry));
	//convert_wchar(szEntry, REG_ENT_WM_JOBINFO_WDM, sizeof(szEntry));
//bear	sh_GetPrinterData(pszSvrPrnName, szEntry, &dwType, (LPBYTE)pwmd, dwSize, &dwSize);
//	::GetPrinterData(hPrinter, LANGD_KEY_WMDEF_JI_STRING, &dwType, (LPBYTE)pStr, dwSize, &dwSize);
	
	blRet = TRUE;

EXIT:

	if(hPrinter != NULL)
		::ClosePrinter(hPrinter);

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      ResetJobInfoSetting
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      JobInfoˆóüÝ’è‚Ì‰Šú’l‚ð‘‚«ž‚Þ
//=============================================================================
BOOL CShJsonWm::ResetJobInfoSetting(WCHAR *pszSvrPrnName)
{
	BOOL			blRet = FALSE;

	DWORD			dwData;

	WCHAR			szData[256];
	WCHAR			szMethod[128];

	CShIniFile FAR	*pini = NULL;

	HANDLE			hPrinter = NULL;
	PRINTER_DEFAULTS	pd = {NULL, NULL, PRINTER_ALL_ACCESS};

	WATERMARKDATA	wmd;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	if(pszSvrPrnName == NULL)
		goto EXIT;

	//(CreateObj(scui_obj_ini_lang) == FALSE)
	//	goto EXIT;
		
//bear	pini = (CShIniFile FAR *)GetObjAddr(scui_obj_ini_lang);
	if(pini == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  ƒvƒŠƒ“ƒ^ƒI[ƒvƒ“
	// ================================================
	if(sh_OpenPrinter(pszSvrPrnName, &hPrinter, &pd) == FALSE)
		goto EXIT;

	// --- jobinfo setting (watermark tab)
	dwData = (DWORD)(*pini).GetLong(LANGD_SEC_WMDEFAULT, REG_ENT_WM_JOBINFO);
	::SetPrinterData(hPrinter, REG_ENT_WM_JOBINFO, REG_DWORD, (LPBYTE)&dwData, sizeof(dwData));

	// --- logon user name setting
	dwData = (DWORD)(*pini).GetLong(LANGD_SEC_WMDEFAULT, LANGD_KEY_WMDEF_JI_UNAME);
	::SetPrinterData(hPrinter, LANGD_KEY_WMDEF_JI_UNAME, REG_DWORD, (LPBYTE)&dwData, sizeof(dwData));

	// --- pc name setting
	dwData = (DWORD)(*pini).GetLong(LANGD_SEC_WMDEFAULT, LANGD_KEY_WMDEF_JI_PCNAME);
	::SetPrinterData(hPrinter, LANGD_KEY_WMDEF_JI_PCNAME, REG_DWORD, (LPBYTE)&dwData, sizeof(dwData));

	// --- date setting
	dwData = (DWORD)(*pini).GetLong(LANGD_SEC_WMDEFAULT, LANGD_KEY_WMDEF_JI_DATE);
	::SetPrinterData(hPrinter, LANGD_KEY_WMDEF_JI_DATE, REG_DWORD, (LPBYTE)&dwData, sizeof(dwData));

	// --- time setting
	dwData = (DWORD)(*pini).GetLong(LANGD_SEC_WMDEFAULT, LANGD_KEY_WMDEF_JI_TIME);
	::SetPrinterData(hPrinter, LANGD_KEY_WMDEF_JI_TIME, REG_DWORD, (LPBYTE)&dwData, sizeof(dwData));

	// --- custom setting
	dwData = (DWORD)(*pini).GetLong(LANGD_SEC_WMDEFAULT, LANGD_KEY_WMDEF_JI_CUSTOM);
	::SetPrinterData(hPrinter, LANGD_KEY_WMDEF_JI_CUSTOM, REG_DWORD, (LPBYTE)&dwData, sizeof(dwData));

	// --- string setting
	if(dwData != 0)
	{
		SecureZeroMemory(szData, sizeof(szData));
		//bear(*pini).GetString(LANGD_SEC_WMDEFAULT, LANGD_KEY_WMDEF_JI_STRING, "", szData, _countof(szData)/*, IsUnicodeOS()*/);
		if(IsUnicodeOS())
		{
			//(szMethod, LANGD_KEY_WMDEF_JI_STRING, sizeof(szMethod));
			::SetPrinterDataW(hPrinter, (WCHAR *)szMethod, REG_SZ, (LPBYTE)&szData, (wcslen(szData) + 1) * 2);
		}
		else
			::SetPrinterData(hPrinter, LANGD_KEY_WMDEF_JI_STRING, REG_SZ, (LPBYTE)&szData, wcslen(szData) + 1);
	}

	// --- WATERMARKDATA
	::ZeroMemory(&wmd, sizeof(wmd));
	GetWMDefData(0, &wmd);
	::SetPrinterData(hPrinter, REG_ENT_WM_JOBINFO_WDM, REG_BINARY, (LPBYTE)&wmd, sizeof(wmd));

	blRet = TRUE;

EXIT:

	if(hPrinter != NULL)
		::ClosePrinter(hPrinter);

	return blRet;
}

//=============================================================================
// function
//      SetJobInfoSetting
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      pszEntry			‘‚«ž‚ÝæƒGƒ“ƒgƒŠ–¼
//      dwData			@@‘‚«ž‚Þƒf[ƒ^
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      JobInfoˆóüÝ’è‚ð‘‚«ž‚Þ
//=============================================================================
BOOL CShJsonWm::SetJobInfoSetting(WCHAR *pszSvrPrnName, WCHAR *pszEntry, DWORD dwData)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	HANDLE			hPrinter = NULL;
	PRINTER_DEFAULTS	pd = {NULL, NULL, PRINTER_ALL_ACCESS};

	if(pszSvrPrnName == NULL || pszEntry == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_ROOT_BASE);

	// ================================================
	// _/_/_/  ŠÇ—Œ ŒÀ‚ðŽ‚Á‚Ä‚¢‚éê‡‚ÍAƒ[ƒJƒ‹
	// _/_/_/  ‚Å“ÆŽ©‚ÌÝ’è‚ð•ÛŽ‚Å‚«‚é
	// ================================================
//bear	if(IsWatermarkJobInfoSUser(pszSvrPrnName) != FALSE)
		m_pParent->WriteJsonDWORDData(pszKey, pszEntry, dwData);

	// ================================================
	// _/_/_/  ƒT[ƒo[‚É•Û‘¶
	// ================================================
	if(IsSharedServerSide(pszSvrPrnName/*, m_pLog*/) != FALSE)
	{
		if(sh_OpenPrinter(pszSvrPrnName, &hPrinter, &pd) == FALSE)
			goto EXIT;

		::SetPrinterData(hPrinter, pszEntry, REG_DWORD, (LPBYTE)&dwData, sizeof(dwData));
	}

EXIT:

	if(hPrinter != NULL)
		::ClosePrinter(hPrinter);

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      SetJobInfoCustomString
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      pStr                ‘‚«ž‚Þ•¶Žš—ñ
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      JobInfo—pAƒJƒXƒ^ƒ€•¶Žš—ñ‚ð‘‚«ž‚Þ
//=============================================================================
BOOL CShJsonWm::SetJobInfoCustomString(WCHAR *pszSvrPrnName, WCHAR *pStr)
{
	BOOL			blRet = FALSE;

	short			wBy = 2;

	WCHAR FAR		*pszKey = NULL;

	HANDLE			hPrinter = NULL;
	PRINTER_DEFAULTS	pd = {NULL, NULL, PRINTER_ALL_ACCESS};

	if(pszSvrPrnName == NULL || pStr == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_ROOT_BASE);

	//if(IsUnicode() != FALSE)
	//	wBy = 2;
	//else
	//	wBy = 1;

	// ================================================
	// _/_/_/  ŠÇ—Œ ŒÀ‚ðŽ‚Á‚Ä‚¢‚éê‡‚ÍAƒ[ƒJƒ‹
	// _/_/_/  ‚Å“ÆŽ©‚ÌÝ’è‚ð•ÛŽ‚Å‚«‚é
	// ================================================
//	if(IsWatermarkJobInfoSUser(pszSvrPrnName) != FALSE)
	//	m_pParent->WriteJsonStrData(pszKey, LANGD_KEY_WMDEF_JI_STRING, (WCHAR*)pStr, (DWORD)wcslen(pStr) * wBy);
	//bear
	// ================================================
	// _/_/_/  ƒT[ƒo[‚É•Û‘¶
	// ================================================
	if(IsSharedServerSide(pszSvrPrnName/*, m_pLog*/) != FALSE)
	{
		if(sh_OpenPrinter(pszSvrPrnName, &hPrinter, &pd) == FALSE)
			goto EXIT;

		//convert_wchar(pszKey, LANGD_KEY_WMDEF_JI_STRING, SCUI_REGKEYSIZE);		
//		sh_SetPrinterData(pszSvrPrnName, pszKey, REG_SZ, (LPBYTE)pStr, (DWORD)wcslen(pStr) * wBy);
	}

EXIT:

	if(hPrinter != NULL)
		::ClosePrinter(hPrinter);

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      SetJobInfoWATERMARKDATA
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      pwmd                ‘‚«ž‚ÞWATERMARKDATA\‘¢‘ÌƒAƒhƒŒƒX
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      JobInfo—pAWatermarkƒf[ƒ^‚ð‘‚«ž‚Þ
//=============================================================================
BOOL CShJsonWm::SetJobInfoWATERMARKDATA(WCHAR *pszSvrPrnName, WATERMARKDATA *pwmd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	HANDLE			hPrinter = NULL;
	PRINTER_DEFAULTS	pd = {NULL, NULL, PRINTER_ALL_ACCESS};

	if(pszSvrPrnName == NULL || pwmd == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_ROOT_BASE);

	// ================================================
	// _/_/_/  ŠÇ—Œ ŒÀ‚ðŽ‚Á‚Ä‚¢‚éê‡‚ÍAƒ[ƒJƒ‹
	// _/_/_/  ‚Å“ÆŽ©‚ÌÝ’è‚ð•ÛŽ‚Å‚«‚é
	// ================================================
	/*if(IsWatermarkJobInfoSUser(pszSvrPrnName) != FALSE)
		m_pParent->WriteJsonBinaryData(pszKey, JSON_ENT_WM_JOBINFO_WDM, (WCHAR*)pwmd, (DWORD)sizeof(WATERMARKDATA));*///bear

	// ================================================
	// _/_/_/  ƒT[ƒo[‚É•Û‘¶
	// ================================================
	if(IsSharedServerSide(pszSvrPrnName/*, m_pLog*/) != FALSE)
	{
		if(sh_OpenPrinter(pszSvrPrnName, &hPrinter, &pd) == FALSE)
			goto EXIT;

		//convert_wchar(pszKey, REG_ENT_WM_JOBINFO_WDM, SCUI_REGKEYSIZE);		
		//bearsh_SetPrinterData(pszSvrPrnName, pszKey, REG_BINARY, (LPBYTE)pwmd, (DWORD)sizeof(WATERMARKDATA));
	}

EXIT:

	if(hPrinter != NULL)
		::ClosePrinter(hPrinter);

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


#endif

// << 2007.03.14 SBC:Kawabata


// <S><A> 2016.04.18 SSL:K.Hamaguchi
//=============================================================================
// function
//      CalcGrayscale
//
// parameters
//      pwmd       
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALSE
//
// outline
//      GrayScale’l‚ðXV‚·‚é
//=============================================================================
BOOL CShJsonWm::CalcGrayscale(LPWATERMARKDATA pwmd)
{
	BOOL			blRet = FALSE;
//	short			wR = 0;
//	short			wG = 0;
//	short			wB = 0;
//	short			sum;
//
//	BYTE			cR = 0;
//	BYTE			cG = 0;
//	BYTE			cB = 0;
//
//	if(pwmd == NULL){
//		goto EXIT;
//	}
//
//	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
//	{
//		goto EXIT;
//	}*/
//
//	wR = (SHORT)(*m_pmcf).GetLongDPLCID(MCF_KEY_DEF_C_NGBRED);
//	wG = (SHORT)(*m_pmcf).GetLongDPLCID(MCF_KEY_DEF_C_NGBGREEN);
//	wB = (SHORT)(*m_pmcf).GetLongDPLCID(MCF_KEY_DEF_C_NGBBLUE);
//
//	cR = GetRValue((*pwmd).colorRef);
//	cG = GetGValue((*pwmd).colorRef);
//	cB = GetBValue((*pwmd).colorRef);
//
//	sum = wR + wG + wB;
//
//	//if(sum != 0){
//	//	(*pwmd).nGrayscale = ((unsigned short)cR * wR +
//	//					  (unsigned short)cG * wG +
//	//				 	  (unsigned short)cB * wB) / sum;
//	//}
//	//else{
//	//	// ‚ ‚è‚¦‚È‚¢‘g‚Ý‡‚í‚¹‚Ì‚½‚ßA‰½‚à‚µ‚È‚¢B
//	//}
//
//	//if( 255 < (*pwmd).nGrayscale ){
//	//	(*pwmd).nGrayscale = 255;
//	//}
//
//	blRet = TRUE;
//
//EXIT:
//
	return blRet;
}
// <E> 2016.04.18 SSL:K.Hamaguchi

//=============================================================================
// function
//      GetTitle
//
// parameters
//      lIndex              î•ñ‚ðŽæ“¾‚·‚éƒCƒ“ƒfƒbƒNƒX
//      pszTitle            ƒ^ƒCƒgƒ‹‚ðŽó‚¯Žæ‚éƒoƒbƒtƒ@
//      dwBufSize           pszTitleƒTƒCƒY
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//     get title
//=============================================================================
BOOL CShJsonWm::GetTitle(WCHAR FAR *pszTitle, DWORD dwBufSize, long lIndex)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	DWORD			dwSize = 0;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	if(pszTitle == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  “Ç‚Ýž‚Ý
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_INDEX_BASE, lIndex);
	
	// --- title
	dwSize = dwBufSize;
	if(m_pParent != NULL)
		blRet = m_pParent->ReadJsonStrData(pszKey,JSON_ENT_WM_TITLE,pszTitle, dwSize);

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
//      lIndex              î•ñ‚ðŽæ“¾‚·‚éƒCƒ“ƒfƒbƒNƒX
//      pszTime            ƒ^ƒCƒgƒ‹‚ðŽó‚¯Žæ‚éƒoƒbƒtƒ@
//      dwBufSize           pszTimeƒTƒCƒY
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALE
//
// outline
//      get time
//=============================================================================
BOOL CShJsonWm::GetTime(WCHAR FAR *pszTime, DWORD dwBufSize, long lIndex)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	DWORD			dwSize = 0;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	if(pszTime == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  “Ç‚Ýž‚Ý
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_INDEX_BASE, lIndex);
	
	// --- time
	dwSize = dwBufSize;
	if(m_pParent != NULL)
		blRet = m_pParent->ReadJsonStrData(pszKey,JSON_ENT_US_TIME,pszTime, dwSize);

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
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      lIndex              “Ç‚Ýž‚ÞUserSettingƒCƒ“ƒfƒbƒNƒX
//
// return value
//       TRUE: is local fav item
//       FALE: is the shared fav item
//
// outline
//      get the self flag
//=============================================================================
BOOL CShJsonWm::GetSelfFlag(long lIndex)
{
	BOOL			bSelf = TRUE;

	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	WCHAR 			*pszKey = NULL;


	// ================================================
	// _/_/_/  ‘‚«ž‚Ý
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_WM_INDEX_BASE, lIndex);

	// --- self
	if(m_pParent != NULL)
		blSuccess =m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_WM_SELF, dwData);
	
	if(blSuccess == FALSE)
		goto EXIT;

	bSelf = (BOOL)dwData;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return bSelf;
}

//<S><A> 2022.04.19 Jelly, for cr5 function2 fav
//=============================================================================
// function
//      WriteShareDayTimeFunc
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALSE
//
// outline
//     write share_daytime_func_different_paper to JSON and HKLM: eg: 2,20210811103941,different_paper
//=============================================================================
BOOL CShJsonWm::WriteShareDayTimeFunc(WCHAR* pszSvrPrnName,WCHAR* pszDayTime, BOOL bValid)
{
	BOOL	blRet = FALSE;

	blRet = CShJson::WriteShareDayTimeFunc(pszSvrPrnName,JSON_KEY_WM_ROOT_BASE,pszDayTime,REG_ENT_SHARE_WM,bValid);

	return blRet;
}


//=============================================================================
// function
//      WriteShareDayTimeFuncToJSON
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      pszText				the total string: eg: 2,20210811103941,different_paper
//		dwSize				the size of pszText
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALSE
//
// outline
//     write share_daytime_func_different_paper to JSON
//=============================================================================
BOOL CShJsonWm::WriteShareDayTimeFuncToJSON(WCHAR* pszSvrPrnName, WCHAR* pszText, DWORD dwSize)
{
	BOOL		blRet = FALSE;

	blRet = CShJson::WriteShareDayTimeFuncToJSON(pszSvrPrnName,JSON_KEY_WM_ROOT_BASE, REG_ENT_SHARE_WM,pszText, dwSize);

	return blRet;
}

//=============================================================================
// function
//      ReadShareDayTimeFuncFromJSON
//
// parameters
//      pszSvrPrnName      ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//		pszText            the value
//		dwTextSize		   the size of pszText
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALSE
//
// outline
//     Read share_daytime_func_different_paper from JSON: eg: 2,20210811103941,different_paper
//=============================================================================
BOOL CShJsonWm::ReadShareDayTimeFuncFromJSON(WCHAR* pszSvrPrnName,WCHAR* pszText,DWORD dwTextSize)
{
	BOOL		blRet = FALSE;

	blRet = CShJson::ReadShareDayTimeFuncFromJSON(pszSvrPrnName,JSON_KEY_WM_ROOT_BASE,REG_ENT_SHARE_WM,pszText,dwTextSize);

	return blRet;
}

//=============================================================================
// function
//      ReadShareDayTimeFuncFromHKLM
//
// parameters
//      pszSvrPrnName      ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//		pszText            the value
//		dwTextSize		   the size of pszText
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALSE
//
// outline
//     Read share_daytime_func_different_paper from HKLM: eg: 2,20210811103941,different_paper
//=============================================================================
BOOL CShJsonWm::ReadShareDayTimeFuncFromHKLM(WCHAR  *pszSvrPrnName, WCHAR  *pszText,DWORD dwTextSize)
{
	BOOL		blRet = FALSE;

	blRet = CShJson::ReadShareDayTimeFuncFromHKLM(pszSvrPrnName,REG_ENT_SHARE_WM,pszText,dwTextSize);

	return blRet;
}



//=============================================================================
// function
//      WriteShareData
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      pShareFav           pShareFav data
//      dwSize				the size of pShareFav
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALSE
//
// outline
//      Write the pShareFav to HKLM's PrinterDriverData
//=============================================================================
BOOL CShJsonWm::WriteShareData(WCHAR  *pszSvrPrnName, BYTE* pShareFav, DWORD dwSize)
{
	BOOL		bRet = FALSE;

	bRet = CShJson::WriteShareData(pszSvrPrnName, REG_ENT_SHARE_WM, pShareFav, dwSize);

	return bRet;
}

//=============================================================================
// function
//      ReadShareData
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      pShareFav           read to the pShareFav
//
// return value
//      ¬Œ÷ : buffer size
//      Ž¸”s : 0
//
// outline
//      Read the share fav data from HKLM
//=============================================================================
DWORD CShJsonWm::ReadShareData(WCHAR  *pszSvrPrnName, BYTE* pShareFav)
{
	DWORD		dwRet = 0;

	dwRet = CShJson::ReadShareData(pszSvrPrnName,REG_ENT_SHARE_WM,pShareFav);

	return dwRet;
}

//=============================================================================
// function
//      ReadFavDataFromJSON
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      lIndex              fav list index
//      pShareFav           the buffer to save the fav data
//
// return value
//      ¬Œ÷ : fav data buffer size
//      Ž¸”s : 0
//
// outline
//      Read the fav data specified by lIndex
//=============================================================================
DWORD CShJsonWm::ReadFavDataFromJSON(WCHAR  *pszSvrPrnName, BYTE* pShareFavData,long lIndex)
{
	DWORD			dwSize = 0;
	LPWATERMARKDATA	pData = NULL;

	if(pszSvrPrnName == NULL)
		goto EXIT;
	
	dwSize = sizeof(WATERMARKDATA);

	if(pShareFavData == NULL)
	{
		goto EXIT;
	}
		
	pData = (LPWATERMARKDATA)new WCHAR[dwSize];
	if(pData == NULL)
		goto EXIT;

	SecureZeroMemory(pData, dwSize);
	
	if(ReadWMData(lIndex, pData) == 0)
		goto EXIT;
	
	memcpy(pShareFavData,pData,dwSize);

EXIT:

	if(pData != NULL)
		delete[] (WCHAR*)pData;

	return dwSize;
}

//=============================================================================
// function
//      TransferOneFavDataFromHKLMToJSON
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//		dwIndex				the WM item index in JSON
//      pShareFavData       the one WM fav data in HKLM. it is the [DATA] in 20210811103941,AAA,DWORD,[DATA],
//		dwSize				the size of pShareFavData
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALSE
//
// outline
//      transfer the one WM fav data from HKLM to JSON.
//=============================================================================
BOOL CShJsonWm::TransferOneFavDataFromHKLMToJSON(WCHAR  *pszSvrPrnName, DWORD dwIndex, BYTE* pShareFavData,DWORD dwSize,WCHAR* pszTime, WCHAR* pszTitle,BOOL bOverWrite)
{
	BOOL			bRet = FALSE;
	LPWATERMARKDATA	pData = NULL;
	DWORD			dwCount = 0;
	
	if(pszSvrPrnName == NULL || pShareFavData == NULL || pszTime == NULL || pszTitle == NULL)
		goto EXIT;

	pData = (LPWATERMARKDATA)pShareFavData;
	
	dwCount = ReadCount();
	WriteWMData(dwIndex, pData, pszTitle, pszTime,FALSE);
	
	if(bOverWrite == FALSE)
	{
		WriteWMCount(dwCount+1);
	}

	bRet = TRUE;
	
EXIT:

	return bRet;
}
// <E> 2022.04.19 Jelly for cr5 function2

// <S><A> 2022.05.17 sbc R.Tanaka
//=============================================================================
// function
//      SetFunctionsJsonPath
//
// parameters
//      pszDeviceName		
//
// return value
//      –³‚µ
//
// outline
//     add functions name to json file path
//=============================================================================
void CShJsonWm::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_WM);
}
// <E> 2022.05.17 sbc R.Tanaka

// <S><A> 2024.02.28,To Fix Redmine #6882 SSDI:Manoj S
//=============================================================================
// function
//      resetEx
//
// parameters
//      pszSvrPrnName       ƒT[ƒo[–¼•t‚«ƒvƒŠƒ“ƒ^–¼
//      pIniFile            CShIniFile ƒNƒ‰ƒXƒAƒhƒŒƒX
//
// return value
//      ¬Œ÷ : TRUE
//      Ž¸”s : FALSE
//
// outline
//      INIƒtƒ@ƒCƒ‹‚Ìƒf[ƒ^‚ðŽg—p‚µ‚ÄƒŒƒWƒXƒgƒŠ‚ð‰Šú‰»‚·‚é
//=============================================================================
BOOL CShJsonWm::resetEx(WCHAR FAR *pszSvrPrnName, CShIniFile FAR *pIniFile)
{	
	//m_hInstStr		= NULL;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	CShIniFile FAR	*pini = NULL;

	WATERMARKDATA	wmd;
	WATERMARKDATA	pwmd;
	
	DWORD			i;

	short			wRegCount;
	short			wIndex;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	if(pszSvrPrnName == NULL)
		goto EXIT;

	wRegCount = (short)ReadCount();
	// <S><A> 2024.02.28,To Fix Redmine #6882 SSDI:Manoj S
	//if(wRegCount > 0)
	//{
	//	for(wIndex = wRegCount-1; 0 <= wIndex; wIndex--)
	//	{	
	//		DeleteData(wIndex);
	//	}
	//}
	// <E> 2024.02.28,To Fix Redmine #6882 SSDI:Manoj S

	// ================================================
	// _/_/_/  INIƒtƒ@ƒCƒ‹ƒNƒ‰ƒXŠm’è
	// ================================================
	if(pIniFile != NULL)
		pini = pIniFile;
	else
	{
		slog.putLog("CWaterMarkRegistryData::resetEx -> NULL_POINTER(pIniFile)\n");
	}

	// ================================================
	// _/_/_/  ‰Šú‰»î•ñŽæ“¾‚Æ‘‚«ž‚Ý
	// ================================================
	dwCount = GetCountIni(pIniFile);
	if(dwCount == 0)
		goto EXIT;

	SecureZeroMemory(&wmd, sizeof(wmd));
	for(i = 0; i < dwCount; i++)
	{
		GetWMDefData(i, &wmd, pIniFile);
		
		WriteWMData(i, &wmd, wmd.szMainWM);
	}

    // Whenever language is changed,reset the standard watermarks and retain the watermark  count to actual without resetting the count.

	if(0 == wRegCount)
		wRegCount = dwCount;

	WriteWMCount(wRegCount);

// for Watermark JobInfo
#ifdef _WIN32
	// ================================================
	// _/_/_/  JobInfoî•ñ‚Ì‘‚«ž‚Ý
	// ================================================
	ResetJobInfoSetting(pszSvrPrnName);
#endif

	blRet = TRUE;

EXIT:

	return blRet;
}

BOOL CShJsonWm::resetWM(WCHAR FAR *pszSvrPrnName, CShIniFile FAR *pIniFile)
{
	//m_hInstStr		= NULL;
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	CShIniFile FAR	*pini = NULL;

	WATERMARKDATA	wmd;
	
	DWORD			i;

	// ================================================
	// _/_/_/  ƒpƒ‰ƒ[ƒ^ƒ`ƒFƒbƒN
	// ================================================
	if(pszSvrPrnName == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  Šù‚É‘‚«ž‚Ü‚ê‚Ä‚¢‚éê‡‚Íreset‚µ‚È‚¢
	// _/_/_/  2005.10.18 S.Kawabata (SQAŽw“E‘Î‰žj
	// ================================================
	if(ReadCount() > 1)
	{
		// ================================================
		// _/_/_/  INIƒtƒ@ƒCƒ‹ƒNƒ‰ƒXŠm’è
		// ================================================
		if(pIniFile != NULL)
			pini = pIniFile;
		else
		{
		/*	if(CreateObj(scui_obj_ini_lang) == FALSE)
				goto EXIT;*/

			//pini = (CShIniFile FAR *)GetObjAddr(scui_obj_ini_lang);
			//if(pini == NULL)
			//	goto EXIT;//bear
		}

		// ================================================
		// _/_/_/  ‰Šú‰»î•ñŽæ“¾‚Æ‘‚«ž‚Ý
		// ================================================
		dwCount = GetCountIni();
		if(dwCount == 0)
			goto EXIT;

		SecureZeroMemory(&wmd, sizeof(wmd));
		for(i = 0; i < dwCount; i++)
		{
			GetWMDefData(i, &wmd);

			WriteWMData(i, &wmd, wmd.szMainWM);
		}
	}
	blRet = TRUE;
EXIT:
	return blRet;
}
// <E> 2024.02.28,To Fix Redmine #6882 SSDI:Manoj S

#define OUTLINE     6
#define DECORATE    10
BOOL CShJsonWm::GetWMDefDataEx(long lIndex, WATERMARKINFO* pWaterMark,CShIniFile FAR *pIniFile)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	DWORD m_wWMCount = ReadCount();
	if (NULL == pWaterMark)
	{
		slog.putLog("CWaterMarkFileData::GetWMDefData -> NULL_POINTER(pWaterMark)\n");
		return FALSE;
	}

	WCHAR szWaterMarkLangSection[MAX_PATH] = { 0 };

	::swprintf_s(szWaterMarkLangSection, MAX_PATH, L"%s%d", LANGD_SEC_WMDEFAULT, m_wLanguageID);

	long lCount = pIniFile->GetLongArray(szWaterMarkLangSection, lIndex);

	if (0 == lCount)
	{
		return FALSE;
	}

	long* pl = pIniFile->GetLongArrayData();

	if (NULL == pl)
	{
		slog.putLog("CWaterMarkFileData::GetWMDefData -> NULL_POINTER(pl)\n");
		return FALSE;
	}

	WCHAR szData[384] = { 0 };

	if (pIniFile->GetString(szWaterMarkLangSection, lIndex, L"", szData, sizeof(szData) / sizeof(WCHAR)) == 0)
	{
		slog.putLog("CWaterMarkFileData::GetWMDefData -> NULL_POINTER(m_pIniFile)\n");
		return FALSE;
	}

	SecureZeroMemory(pWaterMark, sizeof(WATERMARKINFO));
	pWaterMark->dwWMLangID = m_wLanguageID;
	long lPos = 0;

	// --- dwSignature
	char* psz = (char*)&(*pWaterMark).dwSignature;
	*psz = 'W';
	psz++;
	*psz = 'M';
	psz++;
	*psz = 'D';
	psz++;
	*psz = 'T';

	// --- wFieldWMD --- DM_WM_UNICODE
	pWaterMark->wFieldWMD |= DM_WM_UNICODE;

	// --- wFieldWMD --- DM_WM_DEC_CIRCLE
	if (pl[lPos++] != 0)
		pWaterMark->wFieldWMD |= DM_WM_DEC_CIRCLE;

	// --- wFieldWMD --- DM_WM_DEC_SQUARE
	if (pl[lPos++] != 0)
		pWaterMark->wFieldWMD |= DM_WM_DEC_SQUARE;

	// --- wFieldWMD --- DM_WM_FIRSTPAGEONLY
	if (pl[lPos++] != 0)
		pWaterMark->wFieldWMD |= DM_WM_FIRSTPAGEONLY;

	// --- wFieldWMD --- DM_WM_TRANSPARENT
// Printing Pattern
//	0:none
//	1:Transparent1
//	2:Transparent2
//	3:Overprinting

	if (pl[lPos++] != 0)
	{
		switch (pl[lPos - 1])
		{
		case 1:			// Transparent1
			pWaterMark->wFieldWMD |= DM_WM_TRANSPARENT;
			break;
		case 2:			// Transparent2
			pWaterMark->wFieldWMD |= DM_WM_TRANSPARENT2;
			break;
		case 3:			// Overprinting
			pWaterMark->wFieldWMD |= DM_WM_PILE;
			break;
		default:
			break;
		}
	}

	// --- wFieldWMD --- DM_WM_OUTLINE
	if (pl[lPos++] != 0)
		pWaterMark->wFieldWMD |= DM_WM_OUTLINE;

	// --- colorRef
	BYTE r = (BYTE)pl[lPos++];
	BYTE g = (BYTE)pl[lPos++];
	BYTE b = (BYTE)pl[lPos++];

	pWaterMark->colorRef = RGB(r, g, b);

	// --- increment for nGrayscale
	lPos++;
	// --- dwPosX
	pWaterMark->dwPosX = (DWORD)pl[lPos++];

	// --- dwPosY
	pWaterMark->dwPosY = (DWORD)pl[lPos++];

	// --- nSize
	pWaterMark->nSize = (SHORT)pl[lPos++];

	// --- nAngle
	pWaterMark->nAngle = (SHORT)pl[lPos++];

	// --- wBorderSize
	pWaterMark->wBorderSize = (WORD)pl[lPos++];

	// --- LGCFONT --- lfHeight
	long lVal = pl[lPos++];

	if (0 == lVal)
	{
		HDC				hDC = NULL;
		HWND			hWnd = NULL;

		hWnd = ::GetDesktopWindow();
		hDC = ::GetDC(hWnd);
		pWaterMark->lfWatermark.lfHeight = -MulDiv(pWaterMark->nSize, ::GetDeviceCaps(hDC, LOGPIXELSY), SCUI_POINTSPERINCH);
		ReleaseDC(hWnd, hDC);
	}
	else
	{
		pWaterMark->lfWatermark.lfHeight = lVal;
	}

	// --- LGCFONT --- lfWidth
	pWaterMark->lfWatermark.lfWidth = pl[lPos++];

	// --- LGCFONT --- lfEscapement
	pWaterMark->lfWatermark.lfEscapement = pl[lPos++];

	// --- LGCFONT --- lfOrientation
	pWaterMark->lfWatermark.lfOrientation = pl[lPos++];

	// --- LGCFONT --- lfWeight
	pWaterMark->lfWatermark.lfWeight = pl[lPos++];

	// --- LGCFONT --- lfItalic
	pWaterMark->lfWatermark.lfItalic = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfUnderline
	pWaterMark->lfWatermark.lfUnderline = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfStrikeOut
	pWaterMark->lfWatermark.lfStrikeOut = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfWCHARSet
	pWaterMark->lfWatermark.lfCharSet = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfOutPrecision
	pWaterMark->lfWatermark.lfOutPrecision = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfClipPrecision
	pWaterMark->lfWatermark.lfClipPrecision = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfQuality
	pWaterMark->lfWatermark.lfQuality = (BYTE)pl[lPos++];

	// --- LGCFONT --- lfPitchAndFamily
	pWaterMark->lfWatermark.lfPitchAndFamily = (BYTE)pl[lPos++];

	// --- szMainWM
	WCHAR* pc = wcsrchr(szData, L',');

	if (NULL == pc)
	{
		slog.putLog("CWaterMarkFileData::GetWMDefData -> NULL_CHARACTER_POINTER\n");
		return FALSE;
	}

	*pc = L'\0';
	pc++;

	if ((0 == lIndex) || ((m_wWMCount - 1) == lIndex))
	{
		UINT uStrID = (0 == lIndex) ? IDS_INDEX0_WATERMARK : IDS_WMCOLOR_CUSTOM;
		LoadString(m_hStringResourceHandle, uStrID, pWaterMark->szMainWM, sizeof(pWaterMark->szMainWM) / sizeof(WCHAR));
	}
	else
	{
		wcsncpy_s(pWaterMark->szMainWM, _countof(pWaterMark->szMainWM), pc, WM_STR_LENGTH);
	}

	// --- LGCFONT --- lfFaceName
	pc = wcsrchr(szData, L',');

	if (NULL == pc)
	{
		slog.putLog("CWaterMarkFileData::GetWMDefData -> NULL_CHARACTER_POINTER\n");
		return FALSE;
	}

	pc++;
	wcsncpy_s(pWaterMark->lfWatermark.lfFaceName, _countof(pWaterMark->lfWatermark.lfFaceName), pc, LGCF_FACESIZE * 2);

	pWaterMark->wOutlineWidthPixel = OUTLINE;
	pWaterMark->wDecoratedWidthPixel = DECORATE;

	lCount = pIniFile->GetLongArray(szWaterMarkLangSection, LANGD_KEY_WMDEF_OUTLINE_W);

	if (lCount)
	{
		pl = pIniFile->GetLongArrayData();

		if (NULL != pl)
		{
			pWaterMark->wOutlineWidthPixel = (WORD)(pl[0] * 2);
		}
		else
		{
			slog.putLog("CWaterMarkFileData::GetWMDefData -> NULL_POINTER\n");
		}
	}

	lCount = pIniFile->GetLongArray(szWaterMarkLangSection, LANGD_KEY_WMDEF_DECORATE_W);

	if (lCount)
	{
		pl = pIniFile->GetLongArrayData();

		if (NULL != pl)
		{
			pWaterMark->wDecoratedWidthPixel = (WORD)(pl[0] * 2);
		}
		else
		{
			slog.putLog("CWaterMarkFileData::GetWMDefData -> NULL_POINTER\n");
		}
	}

	return TRUE;
}