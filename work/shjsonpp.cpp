// ============================================================================
// shjsonpp.cpp
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.3.17
// ============================================================================

#include <precomp.h>
#include "shJson.h"
#include "shjsonpp.h"
#include "jsonkey.h"


#define PARENT_CLASSNAME 	CShJson
#define MY_CLASSNAME 		CShJsonPP

//=============================================================================
// function
//      CShJsonPP
//
// parameters
//      hInst               モジュールハンドル
//      pLog                CShPrnLog クラスアドレス
//
// return value
//      無し
//
// outline
//      CShJsonPP クラスコンストラクタ
//=============================================================================
CShJsonPP::CShJsonPP(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName /*CShPrnLog FAR *pLog*/)
			: CShJson(hInst, pszSvrPrnName /*pLog*/)
{
	m_pPrintPositionData = NULL;
	m_dwCount = -1;
	m_blWriteFlg = FALSE;
	m_plData = NULL;
	SetParent(this);
}


//=============================================================================
// function
//      ~CShJsonPP
//
// parameters
//      無し
//
// return value
//      無し
//
// outline
//      CShJsonPP クラスデストラクタ
//=============================================================================
CShJsonPP::~CShJsonPP()
{
	if(m_pPrintPositionData != NULL){
		delete m_pPrintPositionData;
	}
	if (m_plData != NULL)
		delete[] m_plData;
}

//=============================================================================
// function
//      reset
//
// parameters
//      pIniFile            CShIniFile クラスアドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      INIファイルのデータを使用してレジストリを初期化する
//=============================================================================
BOOL CShJsonPP::reset()
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	PRINTPOSITION	ppd;

	// ================================================
	// _/_/_/  既に書き込まれている場合はresetしない
	// ================================================
	if(ReadPPCount() > 1)
		return TRUE;

	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;
*/
	// ================================================
	// _/_/_/  初期化情報取得と書き込み
	// ================================================
	dwCount = GetCountIni();
	if(dwCount == 0)
		goto EXIT;

	SecureZeroMemory(&ppd, sizeof(ppd));
	GetPPDefData(&ppd);

	SetPPData(&ppd);
	SetPPCount(dwCount);

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      resetToUs
//
// parameters
//      pIniFile            CShIniFile クラスアドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      INIファイルのデータを使用してレジストリを初期化する
//=============================================================================
BOOL CShJsonPP::resetToUs(long lIndex)
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	PRINTPOSITION	ppd;
	
	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/

	// ================================================
	// _/_/_/  初期化情報取得と書き込み
	// ================================================
	dwCount = GetCountIni();
	if(dwCount == 0)
		goto EXIT;

	SecureZeroMemory(&ppd, sizeof(ppd));
	GetPPDefData(&ppd);
	WritePPDataToUs(lIndex, &ppd);
	WritePPCount(JSON_KEY_PP_ROOT_BASE_US, lIndex, dwCount);

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      WritePPData
//
// parameters
//      pppd                書き込むPP構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたPP構造体の内容を指定のインデックスでレジストリに
//      書き込む
//=============================================================================
BOOL CShJsonPP::WritePPData(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;

	// ================================================
	// _/_/_/  PRINTPOSITION確定
	// ================================================
	if(pppd == NULL)
		goto EXIT;
	
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/
	
	blRet = xWritePPData(JSON_KEY_PP_ROOT_BASE,pppd);

	SetPPData(pppd);

EXIT:

	return blRet;
}

//=============================================================================
// function
//      WritePPDataToUs
//
// parameters
//      lIndex              書き込むインデックス
//      pppd                書き込むPRINTPOSITION構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたPRINTPOSITION構造体の内容を指定のインデックスでレジストリに
//      書き込む
//=============================================================================
BOOL CShJsonPP::WritePPDataToUs(long lIndexUs, LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  PRINTPOSITION確定
	// ================================================
	if(pppd == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  書き込み
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_PP_ROOT_BASE_US, lIndexUs);

	blRet = xWritePPData(pszKey,pppd);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      WritePPCount
//
// parameters
//      dwCount             書き込む値
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      PP数を書き込む
//=============================================================================
BOOL CShJsonPP::WritePPCount(DWORD dwCount)
{
	BOOL			blRet = FALSE;

	// --- count
	if(m_pParent != NULL)
	{
		m_pParent->WriteJsonDWORDData(JSON_KEY_PP_ROOT_BASE, JSON_ENT_PP_COUNT, dwCount);
	}
	
	SetPPCount(dwCount);
	m_blWriteFlg = TRUE;

	blRet = TRUE;

	return blRet;
}


//=============================================================================
// function
//      WritePPCount
//
// parameters
//      dwCount             書き込む値
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      PP数を書き込む
//=============================================================================
BOOL CShJsonPP::WritePPCount(WCHAR FAR *pszBase, long lIndex, DWORD dwCount)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  書き込み
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, pszBase, lIndex);

	// --- count
	if(m_pParent != NULL)
	{
		m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_PP_COUNT, dwCount);
	}

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      ReadPPData
//
// parameters
//      pppd                読み取るPP構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのPP情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonPP::ReadPPData(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;

	if(m_pPrintPositionData == NULL){
		// 未だメモリにもっていないのでレジストリから取得
		blRet = ReadPPDataBase(pppd);
		if(blRet == FALSE){
			// レジストリもない
			// エラーで返す
			goto EXIT;
		}
		// 取得したデータをメモリにもコピーしておく
		m_pPrintPositionData = new PRINTPOSITION;
		if(m_pPrintPositionData == NULL){
			goto EXIT;
		}
		memcpy(m_pPrintPositionData, pppd, sizeof(PRINTPOSITION));
		m_blWriteFlg = TRUE;
	}
	else{
		// 既にメモリにもっているのでそれを返す
		memcpy(pppd, m_pPrintPositionData, sizeof(PRINTPOSITION));
	}

	blRet = TRUE;
EXIT:
	return blRet;
}


//=============================================================================
// function
//      ReadPPDataBase
//
// parameters
//      pppd                読み取るPP構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのPP情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonPP::ReadPPDataBase(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*psz = NULL;

	if(ReadPPCount() <= 0)
		goto EXIT;

	// ================================================
	// _/_/_/  PRINTPOSITION確定
	// ================================================
	if(pppd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pppd, sizeof(PRINTPOSITION));

	// ================================================
	// _/_/_/  読み込み
	// ================================================
	// --- dwSignature
	psz = (WCHAR FAR *)&(*pppd).dwSignature;
	*psz = 'P';
	psz++;
	*psz = 'P';
	psz++;
	*psz = 'D';
	psz++;
	*psz = 'T';

	blRet = xReadPPData(JSON_KEY_PP_ROOT_BASE,pppd);

EXIT:

	return blRet;
}


//=============================================================================
// function
//      ReadPPDataToUs
//
// parameters
//      lIndex              読み取るインデックス
//      pwmd                読み取るPRINTPOSITION構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのPRINTPOSITION情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonPP::ReadPPDataToUs(long lIndexUs, LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*psz = NULL;

	// ================================================
	// _/_/_/  PRINTPOSITION確定
	// ================================================
	if(pppd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pppd, sizeof(PRINTPOSITION));

	// ================================================
	// _/_/_/  読み込み
	// ================================================
	// --- dwSignature
	psz = (WCHAR FAR *)&(*pppd).dwSignature;
	*psz = 'P';
	psz++;
	*psz = 'P';
	psz++;
	*psz = 'D';
	psz++;
	*psz = 'T';

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_PP_ROOT_BASE_US, lIndexUs);

	blRet = xReadPPData(pszKey,pppd);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      ReadPPCount
//
// parameters
//
// return value
//      成功 : 登録PP数
//      失敗 : 0
//
// outline
//      PP数を読み込む
//=============================================================================
DWORD CShJsonPP::ReadPPCount()
{
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	if(m_blWriteFlg == TRUE){
		dwData = m_dwCount;
		goto EXIT;
	}
	
	if(m_pParent != NULL)
		blSuccess = m_pParent->ReadJsonDWORDData(JSON_KEY_PP_ROOT_BASE,JSON_ENT_PP_COUNT,dwData);

	if(blSuccess != FALSE){
		m_blWriteFlg = TRUE;
		m_dwCount = dwData;
	}

EXIT:

	return dwData;
}


//=============================================================================
// function
//      ReadPPCount
//
// parameters
//
// return value
//      成功 : 登録PP数
//      失敗 : 0
//
// outline
//      PP数を読み込む
//=============================================================================
DWORD CShJsonPP::ReadPPCount(WCHAR *pszBase, long lIndex)
{
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  書き込み
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, pszBase, lIndex);

	if(m_pParent != NULL)
		blSuccess = m_pParent->ReadJsonDWORDData(pszKey,JSON_ENT_PP_COUNT,dwData);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return dwData;
}


//=============================================================================
// function
//      GetCountIni
//
// parameters
//      pIniFile            CShIniFile クラスアドレス
//
// return value
//      成功 : INIファイルから取得した個数
//      失敗 : 0
//
// outline
//      INIファイルのデフォルトPP個数を取得する
//=============================================================================
DWORD CShJsonPP::GetCountIni()
{
	DWORD			dwRet = 0;
	short			wCount = 0;
	long			*plData = NULL;

	//wCount = (short)(*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_PPOS);
	wCount = (short)GetLongArray();
	dwRet = wCount / 5;

EXIT:

	return dwRet;
}

long CShJsonPP::GetLongArray(long *plValue, long lCount)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	long			lRet = 0;
	long			lRemain = 0;
	WCHAR			*pszData = NULL;
	WCHAR			*psz = NULL;
	WCHAR			*psz2 = NULL;
	long			*pl = plValue;
	WCHAR           *pszData2 = L"0, 0, 0, 0, 0";

	pszData = new WCHAR[INI_STR_MAX];
	if (nullptr == pszData)
	{
		slog.putLog("CShRegMS::GetLongArray -> NULL_POINTER(pszData)\n");
		return lRet;
	}
	SecureZeroMemory(pszData, INI_STR_MAX * sizeof(WCHAR));


	if ((nullptr == plValue) || (0 == lCount))
	{
		lRemain = InitLongArray();  // initialize the array where string data stored
		if (lRemain == 0)
		{
			return lRet;
		}
		else
		{
			pl = m_plData;
		}
	}
	else
	{
		lRemain = lCount;
		pl = plValue;
	}

	if ((nullptr == pl) || (0 == lRemain))
	{
		return lRet;
	}

	wcscpy_s(pszData, INI_STR_MAX, pszData2); // Copy the String to pszData
	psz = pszData;
	while (psz != NULL && *psz != L'\0' && lRemain != 0) // Separate each and every character and stored in array 
	{
		psz2 = ::wcschr(psz, L',');
		if (psz2 == NULL)
		{
			pl[lRet] = ::_wtol(psz);
			lRet++;
			break;
		}
		else
			*psz2 = L'\0';

		pl[lRet] = ::_wtol(psz);
		lRemain--;
		lRet++;
		psz = psz2;
		psz++;

		while (*psz == 0x20 || *psz == L'\t')
			psz++;
	}

	delete[] pszData;
	pszData = NULL;
	return lRet;
}

long CShJsonPP::InitLongArray(void)
{
	long lCount = 64;
	if (m_plData == NULL)
		m_plData = new long[lCount];

	if (m_plData == NULL)
		return 0;

	SecureZeroMemory(m_plData, sizeof(long) * lCount);

	return lCount;
}

long *CShJsonPP::GetLongArrayData(void)
{
	return m_plData;
}
//=============================================================================
// function
//      GetPPDefData
//
// parameters
//      pppd                情報を受け取るPP構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたINIファイルのデフォルトPPを取得する
//=============================================================================
BOOL CShJsonPP::GetPPDefData(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;

	long			lCount;
	long			lPos;

	WCHAR FAR		*pc = NULL;
	long FAR		*pl = NULL;

	// ================================================
	// _/_/_/  パラメータチェック
	// ================================================
	if(pppd == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/

	// ================================================
	// _/_/_/  情報の取得
	// ================================================
	//if((lCount = (*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_PPOS)) == 0)
	//	goto EXIT;

	if ((lCount = (GetLongArray()) == 0))
		goto EXIT;

	pl = GetLongArrayData();
	if(pl == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  情報の作成
	// ================================================
	SecureZeroMemory(pppd, sizeof(PRINTPOSITION));
	lPos = 0;

	// --- dwSignature
	pc = (WCHAR FAR *)&(*pppd).dwSignature;
	*pc = 'P';
	pc++;
	*pc = 'P';
	pc++;
	*pc = 'D';
	pc++;
	*pc = 'T';

	// --- dwFieldPPDT
	(*pppd).dwFieldPPDT = 0;

	// --- wOddX
	(*pppd).wOddX = (SHORT)pl[lPos++];

	// --- wOddY
	(*pppd).wOddY = (SHORT)pl[lPos++];

	// --- wEvenX
	(*pppd).wEvenX = (SHORT)pl[lPos++];

	// --- wEvenY
	(*pppd).wEvenY = (SHORT)pl[lPos++];

	// --- dwUnit
	(*pppd).dwUnit = (SHORT)pl[lPos++];

	if(((*pppd).dwUnit == unit_mm) || ((*pppd).dwUnit == unit_inch))
	{
		if(GetUnitDef() != (short)(*pppd).dwUnit)
		{
			if(GetUnitDef() == unit_mm)
			{
				(*pppd).wOddX = (short)InchToMilli_PPos((*pppd).wOddX);
				(*pppd).wOddY = (short)InchToMilli_PPos((*pppd).wOddY);
				(*pppd).wEvenX = (short)InchToMilli_PPos((*pppd).wEvenX);
				(*pppd).wEvenY = (short)InchToMilli_PPos((*pppd).wEvenY);
				(*pppd).dwUnit = unit_mm;
			}
			else
			{
				(*pppd).wOddX = (short)MilliToInch_PPos((*pppd).wOddX);
				(*pppd).wOddY = (short)MilliToInch_PPos((*pppd).wOddY);
				(*pppd).wEvenX = (short)MilliToInch_PPos((*pppd).wEvenX);
				(*pppd).wEvenY = (short)MilliToInch_PPos((*pppd).wEvenY);
				(*pppd).dwUnit = unit_inch;
			}
		}
	}
	//else if(((*pppd).dwUnit == unit_mm_2) || ((*pppd).dwUnit == unit_inch_2))
	//{
	//	if(GetUnitDef()+unit_mm_2 != (short)(*pppd).dwUnit)
	//	{
	//		if(GetUnitDef()+unit_mm_2 == unit_mm_2)
	//		{
	//			(*pppd).wOddX = (short)InchToMilli_PPos2((*pppd).wOddX);
	//			(*pppd).wOddY = (short)InchToMilli_PPos2((*pppd).wOddY);
	//			(*pppd).wEvenX = (short)InchToMilli_PPos2((*pppd).wEvenX);
	//			(*pppd).wEvenY = (short)InchToMilli_PPos2((*pppd).wEvenY);
	//			(*pppd).dwUnit = unit_mm_2;
	//		}
	//		else
	//		{
	//			(*pppd).wOddX = (short)MilliToInch_PPos2((*pppd).wOddX);
	//			(*pppd).wOddY = (short)MilliToInch_PPos2((*pppd).wOddY);
	//			(*pppd).wEvenX = (short)MilliToInch_PPos2((*pppd).wEvenX);
	//			(*pppd).wEvenY = (short)MilliToInch_PPos2((*pppd).wEvenY);
	//			(*pppd).dwUnit = unit_inch_2;
	//		}
	//	}
	//}
	blRet = TRUE;

EXIT:

	return blRet;
}


//=============================================================================
// function
//      SetPPData
//
// parameters
//      pppd                書き込むPP構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたPRINTPOSITION構造体の内容を指定のインデックスでレジストリに
//      書き込む
//=============================================================================
BOOL CShJsonPP::SetPPData(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;
	LPPRINTPOSITION	pPrintPositionDataWk = NULL;

	// ================================================
	// _/_/_/  パラメータチェック
	// ================================================
	if(pppd == NULL){
		goto EXIT;
	}
	
	pPrintPositionDataWk = new PRINTPOSITION;
	if(pPrintPositionDataWk == NULL){
		goto EXIT;
	}

	if(m_pPrintPositionData != NULL){
		delete m_pPrintPositionData;
	}

	m_pPrintPositionData = pPrintPositionDataWk;

	memcpy(m_pPrintPositionData, pppd, sizeof(PRINTPOSITION));

	blRet = TRUE;

EXIT:

	return blRet;
}


//=============================================================================
// function
//      SetPPCount
//
// parameters
//      dwCount             書き込む値
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      PP数を書き込む
//=============================================================================
BOOL CShJsonPP::SetPPCount(DWORD dwCount)
{
	m_dwCount = dwCount;
	return TRUE;
}

//=============================================================================
// function
//      Clear
//
// parameters
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたPRINTPOSITION構造体の内容を指定のインデックスでレジストリに
//      書き込む
//=============================================================================
BOOL CShJsonPP::Clear()
{
	BOOL			blRet = FALSE;

	if(m_pPrintPositionData != NULL){
		delete m_pPrintPositionData;
		m_pPrintPositionData = NULL;
	}

	m_dwCount = -1;
	m_blWriteFlg = FALSE;
	blRet = TRUE;

	return blRet;
}

//=============================================================================
// function
//      xReadPPData
//
// parameters
//      pszKeyPath          PP key path
//      pppd                読み取るPP構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのPP情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonPP::xReadPPData(WCHAR *pszKeyPath,LPPRINTPOSITION pppd)
{
	BOOL	bRet = FALSE;
	DWORD	dwData = 0;
	
	if(pszKeyPath == NULL || pppd == NULL)
		goto EXIT;

	if(m_pParent != NULL)
	{
		// --- wOddX
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_PP_WODDX,dwData);
		(*pppd).wOddX = (SHORT)dwData;

		// --- wOddY
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_PP_WODDY,dwData);
		(*pppd).wOddY = (SHORT)dwData;

		// --- wEvenX
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_PP_WEVENX,dwData);
		(*pppd).wEvenX = (SHORT)dwData;

		// --- wEvenY
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_PP_WEVENY,dwData);
		(*pppd).wEvenY = (SHORT)dwData;

		// --- dwUnit
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_PP_DWUNIT,dwData);
		(*pppd).dwUnit = dwData;

		bRet = TRUE;
	}

EXIT:
	return bRet;
}

//=============================================================================
// function
//      xWritePPData
//
// parameters
//      pszKeyPath          PP key path
//      pppd                読み取るPP構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのPP情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonPP::xWritePPData(WCHAR *pszKeyPath,LPPRINTPOSITION pppd)
{
	BOOL	bRet = FALSE;
	DWORD	dwData = 0;

	if(pszKeyPath == NULL || pppd == NULL)
		goto EXIT;

	if(m_pParent != NULL)
	{
		// --- wOddX
		dwData = (*pppd).wOddX;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_PP_WODDX, dwData);

		// --- wOddY
		dwData = (*pppd).wOddY;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_PP_WODDY, dwData);

		// --- wEvenX
		dwData = (*pppd).wEvenX;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_PP_WEVENX, dwData);

		// --- wEvenY
		dwData = (*pppd).wEvenY;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_PP_WEVENY, dwData);

		// --- dwUnit
		dwData = (*pppd).dwUnit;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_PP_DWUNIT, dwData);

		bRet = TRUE;
	}

EXIT:
	return bRet;
}

// <S><A> 2022.05.17 sbc R.Tanaka
//=============================================================================
// function
//      SetFunctionsJsonPath
//
// parameters
//      pszDeviceName		
//
// return value
//      無し
//
// outline
//     add functions name to json file path
//=============================================================================
void CShJsonPP::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_US);
}
// <E> 2022.05.17 sbc R.Tanaka
