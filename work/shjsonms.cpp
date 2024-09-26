// ============================================================================
// shjsonms.cpp
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.03.11
// ============================================================================

#include <precomp.h>
#include "shJson.h"
#include "shjsonms.h"


#define PARENT_CLASSNAME 	CShJson
#define MY_CLASSNAME 		CShJsonMS

//=============================================================================
// function
//      CShJsonMS
//
// parameters
//      hInst               モジュールハンドル
//      pLog                CShPrnLog クラスアドレス
//
// return value
//      無し
//
// outline
//      CShJsonMS クラスコンストラクタ
//=============================================================================
CShJsonMS::CShJsonMS(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName/*, CShPrnLog FAR *pLog*/)
			: CShJson(hInst, pszSvrPrnName/*, pLog*/)
{
	m_pmsd			= NULL;
	m_lIndex		= -1;
	m_lCount		= -1;
	m_blWriteFlg	= FALSE;
	m_plData = NULL;
	SetParent(this);
}


//=============================================================================
// function
//      ~CShJsonMS
//
// parameters
//      無し
//
// return value
//      無し
//
// outline
//      CShJsonMS クラスデストラクタ
//=============================================================================
CShJsonMS::~CShJsonMS()
{
	if(m_pmsd != NULL){
		delete m_pmsd;
	}
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
BOOL CShJsonMS::reset()
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	MARGINSHIFTDATA	msd;
	
	DWORD			i;

	// ================================================
	// _/_/_/  既に書き込まれている場合はresetしない
	// ================================================
	if(ReadMSCount() > 1)
		return TRUE;

	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

	// ================================================
	// _/_/_/  初期化情報取得と書き込み
	// ================================================
	dwCount = GetCountIni();
	if(dwCount == 0)
		goto EXIT;

	SecureZeroMemory(&msd, sizeof(msd));
	for(i = 0; i < dwCount; i++)
	{
		GetMSDefData(i, &msd);
		SetMSData(i, &msd);
	}

	SetMSCount(dwCount);

	blRet = TRUE;

EXIT:

	return blRet;
}


//=============================================================================
// function
//      resetEx
//
// parameters
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      INIファイルのデータを使用してレジストリを初期化する
//=============================================================================
BOOL CShJsonMS::resetEx()
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	MARGINSHIFTDATA	msd;
	
	DWORD			i;

	// ================================================
	// _/_/_/  既に書き込まれている場合はresetしない
	// ================================================
	if(m_blWriteFlg == TRUE){
		return TRUE;
	}

	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

	// ================================================
	// _/_/_/  初期化情報取得と書き込み
	// ================================================
	dwCount = GetCountIni();
	if(dwCount == 0)
		goto EXIT;

	SecureZeroMemory(&msd, sizeof(msd));
	for(i = 0; i < dwCount; i++)
	{
		GetMSDefData(i, &msd);
		
		WriteMSData(i, &msd);
	}

	WriteMSCount(dwCount);

	WriteJsonDataToFile();

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      resetExToUs
//
// parameters
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      INIファイルのデータを使用してレジストリを初期化する
//=============================================================================
BOOL CShJsonMS::resetExToUs(long lIndex)
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	MARGINSHIFTDATA	msd;
	
	DWORD			i;

	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

	// ================================================
	// _/_/_/  初期化情報取得と書き込み
	// ================================================
	dwCount = GetCountIni();
	if(dwCount == 0)
		goto EXIT;

	SecureZeroMemory(&msd, sizeof(msd));
	for(i = 0; i < dwCount; i++)
	{
		GetMSDefData(i, &msd);
		
		WriteMSDataToUs(lIndex, i, &msd);
	}

	WriteMSCount(JSON_KEY_MS_ROOT_BASE_US, lIndex, dwCount);

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      WriteMSData
//
// parameters
//      lIndex              書き込むインデックス
//      pwmd                書き込むMSDATA構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたMS構造体の内容を指定のインデックスでレジストリに
//      書き込む
//=============================================================================
BOOL CShJsonMS::WriteMSData(long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  MARGINSHIFTDATA確定
	// ================================================
	if(pmsd == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  書き込み
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
		
	::wsprintf(pszKey, JSON_KEY_MS_INDEX_BASE, lIndex);

	/*(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/
	
	blRet = xWriteMSData(pszKey,pmsd);

	SetMSData(lIndex, pmsd);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      WriteMSDataToUs
//
// parameters
//      lIndex              書き込むインデックス
//      pmsd                書き込むMARGINSHIFTDATA構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたMARGINSHIFTDATA構造体の内容を指定のインデックスでレジストリに
//      書き込む
//=============================================================================
BOOL CShJsonMS::WriteMSDataToUs(long lIndexUs, long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	// ================================================
	// _/_/_/  MARGINSHIFTDATA確定
	// ================================================
	if(pmsd == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  書き込み
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_MS_INDEX_BASE_US, lIndexUs, lIndex);

	blRet = xWriteMSData(pszKey,pmsd);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      WriteMSCount
//
// parameters
//      dwCount             書き込む値
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      write the margin shift count
//=============================================================================
BOOL CShJsonMS::WriteMSCount(DWORD dwCount)
{
	BOOL			blRet = FALSE;

	// --- count
	if(m_pParent != NULL)
	{
		m_pParent->WriteJsonDWORDData(JSON_KEY_MS_ROOT_BASE, JSON_ENT_MS_COUNT, dwCount);
	}

	blRet = TRUE;

	SetMSCount(dwCount);
	m_blWriteFlg = TRUE;

	return blRet;
}


//=============================================================================
// function
//      WriteMSCount
//
// parameters
//      dwCount             書き込む値
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      write the margin shift count
//=============================================================================
BOOL CShJsonMS::WriteMSCount(WCHAR FAR *pszBase, long lIndex, DWORD dwCount)
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
		m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_MS_COUNT, dwCount);
	}

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      ReadMSData
//
// parameters
//      lIndex              読み取るインデックス
//      pmsd                読み取るMS構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのMS情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonMS::ReadMSData(long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*psz = NULL;

	if(ReadMSCount() < (DWORD)lIndex + 1)
		goto EXIT;

	// ================================================
	// _/_/_/  MARGINSHIFTDATA確定
	// ================================================
	if(pmsd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pmsd, sizeof(MARGINSHIFTDATA));

	if(m_blWriteFlg == TRUE){
		if(lIndex == m_lIndex){
			memcpy(pmsd, m_pmsd, sizeof(MARGINSHIFTDATA));
			blRet = TRUE;
			goto EXIT;
		}
	}

	if(m_lIndex != lIndex){
		// ================================================
		// _/_/_/  読み込み
		// ================================================
		// --- dwSignature
		psz = (WCHAR FAR *)&(*pmsd).dwSignature;
		*psz = 'M';
		psz++;
		*psz = 'S';
		psz++;
		*psz = 'D';
		psz++;
		*psz = 'T';

		pszKey = new WCHAR[SCUI_REGKEYSIZE];
		if(pszKey == NULL)
			goto EXIT;

		SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
		::wsprintf(pszKey, JSON_KEY_MS_INDEX_BASE, lIndex);

		blRet = xReadMSData(pszKey,pmsd);

		if( ((*pmsd).dwVal == 0) &&
			((*pmsd).dwMilli == 0) &&
			((*pmsd).dwInch == 0) &&
			((*pmsd).dwDPI == 0) &&
			((*pmsd).dwPixel == 0) &&
			((*pmsd).dwUnit == 0) ){
			// リードに失敗している。レジストリに書かれていない
			// デフォルト値を取得する
			GetMSDefData(lIndex, pmsd);
		}

		SetMSData(lIndex, pmsd);
		
	}
	else{
		memcpy(pmsd, m_pmsd, sizeof(MARGINSHIFTDATA));
	}

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      ReadMSDataToUs
//
// parameters
//      lIndex              読み取るインデックス
//      pmsd                読み取るMARGINSHIFTDATA構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのMARGINSHIFTDATA情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonMS::ReadMSDataToUs(long lIndexUs, long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*psz = NULL;
	// ================================================
	// _/_/_/  MARGINSHIFTDATA確定
	// ================================================
	if(pmsd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pmsd, sizeof(MARGINSHIFTDATA));

	// ================================================
	// _/_/_/  読み込み
	// ================================================
	// --- dwSignature
	psz = (WCHAR FAR *)&(*pmsd).dwSignature;
	*psz = 'M';
	psz++;
	*psz = 'S';
	psz++;
	*psz = 'D';
	psz++;
	*psz = 'T';

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_MS_INDEX_BASE_US, lIndexUs, lIndex);

	blRet = xReadMSData(pszKey,pmsd);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      ReadMSCount
//
// parameters
//
// return value
//      成功 : 登録MS数
//      失敗 : 0
//
// outline
//      MS数を読み込む
//=============================================================================
DWORD CShJsonMS::ReadMSCount()
{
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	if(m_blWriteFlg == TRUE){
		dwData = (DWORD)m_lCount;
		goto EXIT;
	}

	if(m_pParent != NULL)
		blSuccess = m_pParent->ReadJsonDWORDData(JSON_KEY_MS_ROOT_BASE,JSON_ENT_MS_COUNT,dwData);

	if(blSuccess == TRUE){
		m_blWriteFlg = TRUE;
		m_lCount = (long)dwData;
	}
	else{
		if(m_lCount != -1){
			dwData = m_lCount;
		}
	}

EXIT:

	return dwData;
}


//=============================================================================
// function
//      ReadMSCount
//
// parameters
//
// return value
//      成功 : 登録MS数
//      失敗 : 0
//
// outline
//      MS数を読み込む
//=============================================================================
DWORD CShJsonMS::ReadMSCount(WCHAR *pszBase, long lIndex)
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
		blSuccess = m_pParent->ReadJsonDWORDData(pszKey,JSON_ENT_MS_COUNT,dwData);

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
//      INIファイルのデフォルトMS個数を取得する
//=============================================================================
DWORD CShJsonMS::GetCountIni()
{
	DWORD			dwRet = 0;
	short			wCount = 0;
	long			*plData = NULL;

	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/

	// ================================================
	// _/_/_/  個数の取得
	// ================================================
	//wCount = (short)(*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_MARGIN);
	wCount = (short)GetLongArray();
	dwRet = wCount / 5;

EXIT:

	return dwRet;
}

long CShJsonMS::GetLongArray(long *plValue, long lCount)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	long			lRet = 0;
	long			lRemain = 0;
	WCHAR			*pszData = NULL;
	WCHAR			*psz = NULL;
	WCHAR			*psz2 = NULL;
	long			*pl = plValue;
	WCHAR           *pszData2 = L"0, 0, 0, 600, 0, 1, 10, 4, 600, 236, 2, 20, 8, 600, 472, 3, 30, 12, 600, 708";

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

long CShJsonMS::InitLongArray(void)
{
	long lCount = 64;
	if (m_plData == NULL)
		m_plData = new long[lCount];

	if (m_plData == NULL)
		return 0;

	SecureZeroMemory(m_plData, sizeof(long) * lCount);

	return lCount;
}

//=============================================================================
// function
//      GetMSDefData
//
// parameters
//      lIndex              情報を取得するインデックス
//      pmsd                情報を受け取るMS構造体アドレス
//      pIniFile            CShIniFile クラスアドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたINIファイルのデフォルトMSを取得する
//=============================================================================
BOOL CShJsonMS::GetMSDefData(long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;

	long			lCount;
	long			lPos;

	WCHAR FAR		*pc = NULL;
	long FAR		*pl = NULL;

	long			i;

	// ================================================
	// _/_/_/  パラメータチェック
	// ================================================
	if(pmsd == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

	// ================================================
	// _/_/_/  情報の取得
	// ================================================
	if ((lCount = GetLongArray()) == 0)
		goto EXIT;

	pl = m_plData;
	if(pl == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  情報の作成
	// ================================================
	SecureZeroMemory(pmsd, sizeof(MARGINSHIFTDATA));
	lPos = 0;

	// --- dwSignature
	pc = (WCHAR FAR *)&(*pmsd).dwSignature;
	*pc = 'M';
	pc++;
	*pc = 'S';
	pc++;
	*pc = 'D';
	pc++;
	*pc = 'T';

	for(i=0; i < lCount/5; i++)
	{
		if(i ==  lIndex)
		{
			// --- dwVal
			(*pmsd).dwVal = (DWORD)pl[lPos++];

			// --- dwMilli
			(*pmsd).dwMilli = (DWORD)pl[lPos++];

			// --- dwInch
			(*pmsd).dwInch = (DWORD)pl[lPos++];

			// --- dwDPI
			(*pmsd).dwDPI = (DWORD)pl[lPos++];

			// --- dwPixel
			(*pmsd).dwPixel = (DWORD)pl[lPos++];
			
			// --- dwUnit
			(*pmsd).dwUnit =  GetUnitDef();

			blRet = TRUE;
			break;
		}
		else
		{
			lPos += 5;
		}
	}

EXIT:

	return blRet;
}

long *CShJsonMS::GetLongArrayData(void)
{
	return m_plData;
}

//=============================================================================
// function
//      SetMSData
//
// parameters
//      lIndex              書き込むインデックス
//      pmsd                書き込むMARGINSHIFTDATA構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスとMARGINSHIFTDATA構造体の内容をメモリに保存する
//=============================================================================
BOOL CShJsonMS::SetMSData(long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;
	LPMARGINSHIFTDATA	pmsdWk = NULL;

	// ================================================
	// _/_/_/  MARGINSHIFTDATA確定
	// ================================================
	if(pmsd == NULL)
		goto EXIT;
	
	pmsdWk = new MARGINSHIFTDATA;
	if(pmsdWk == NULL){
		goto EXIT;
	}

	if(m_pmsd != NULL){
		delete m_pmsd;
	}
	
	m_pmsd = pmsdWk;

	// ================================================
	// _/_/_/  書き込み
	// ================================================
	m_lIndex = lIndex;
	
	memcpy(m_pmsd, pmsd, sizeof(MARGINSHIFTDATA));

	blRet = TRUE;

EXIT:
	return blRet;
}


//=============================================================================
// function
//      SetMSCount
//
// parameters
//      dwCount             書き込む値
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      Margin ShiftのUserSetting数を書き込む
//=============================================================================
BOOL CShJsonMS::SetMSCount(DWORD dwCount)
{
	m_lCount = (long)dwCount;
	return TRUE;
}

//=============================================================================
// function
//      Clear
//
// parameters
//      dwCount             書き込む値
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      メンバ変数をクリアする
//=============================================================================
BOOL CShJsonMS::Clear()
{
	if(m_pmsd != NULL){
		delete m_pmsd;
		m_pmsd = NULL;
	}
	m_lIndex		= -1;
	m_lCount		= -1;
	m_blWriteFlg	= FALSE;

	return TRUE;
}

//=============================================================================
// function
//      xReadMSData
//
// parameters
//      pszKeyPath          MS key path
//      pmsd                読み取るMS構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのMS情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonMS::xReadMSData(WCHAR *pszKeyPath,LPMARGINSHIFTDATA pmsd)
{
	BOOL	bRet = FALSE;
	DWORD	dwData = 0;
	
	if(pszKeyPath == NULL || pmsd == NULL)
		goto EXIT;

	if(m_pParent != NULL)
	{
		// --- dwVal
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_MS_DWVAL,dwData);
		(*pmsd).dwVal = (SHORT)dwData;

		// --- dwMilli
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_MS_DWMILLI,dwData);
		(*pmsd).dwMilli = (LONG)dwData;

		// --- dwInch
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_MS_DWINCH,dwData);
		(*pmsd).dwInch = (LONG)dwData;

		// --- dwDPI
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_MS_DWDPI,dwData);
		(*pmsd).dwDPI = (SHORT)dwData;

		// --- dwPixel
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_MS_DWPIXEL,dwData);
		(*pmsd).dwPixel = (SHORT)dwData;

		// --- dwUnit
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_MS_DWUNIT,dwData);
		(*pmsd).dwUnit = (SHORT)dwData;

		bRet = TRUE;
	}

EXIT:
	return bRet;
}

//=============================================================================
// function
//      xWriteMSData
//
// parameters
//      pszKeyPath          MS key path
//      pmsd                読み取るMS構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのMS情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonMS::xWriteMSData(WCHAR *pszKeyPath,LPMARGINSHIFTDATA pmsd)
{
	BOOL	bRet = FALSE;
	DWORD	dwData = 0;
	
	if(pszKeyPath == NULL || pmsd == NULL)
		goto EXIT;

	if(m_pParent != NULL)
	{
		// --- dwVal
		dwData = (*pmsd).dwVal;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_MS_DWVAL, dwData);

		// --- dwMilli
		dwData = (*pmsd).dwMilli;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_MS_DWMILLI, dwData);

		// --- dwInch
		dwData = (*pmsd).dwInch;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_MS_DWINCH, dwData);

		// --- dwDPI
		dwData = (*pmsd).dwDPI;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_MS_DWDPI, dwData);

		// --- dwPixel
		dwData = (*pmsd).dwPixel;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_MS_DWPIXEL, dwData);

		// --- dwUnit
		dwData = (*pmsd).dwUnit;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_MS_DWUNIT, dwData);

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
void CShJsonMS::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_US);
}
// <E> 2022.05.17 sbc R.Tanaka
