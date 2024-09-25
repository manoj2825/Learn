// ============================================================================
// shjsonups.cpp
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.3.15
// ============================================================================

#include <precomp.h>

#include "shJson.h"
#include "shjsonups.h"
#include "jsonkey.h"

#define PARENT_CLASSNAME 	CShJson
#define MY_CLASSNAME 		CShJsonUserPSize

//=============================================================================
// function
//      CShJsonUserPSize
//
// parameters
//      hInst               モジュールハンドル
//      pLog                CShPrnLog クラスアドレス
//
// return value
//      無し
//
// outline
//      CShJsonUserPSize クラスコンストラクタ
//=============================================================================
CShJsonUserPSize::CShJsonUserPSize(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName/*, CShPrnLog FAR *pLog*/)
			: CShJson(hInst, pszSvrPrnName/*, pLog*/)
{
	m_blDelMulChk	= FALSE;
	//m_pmultichk		= NULL;
	SetParent(this);
}


//=============================================================================
// function
//      ~CShJsonUserPSize
//
// parameters
//      無し
//
// return value
//      無し
//
// outline
//      CShJsonUserPSize クラスデストラクタ
//=============================================================================
CShJsonUserPSize::~CShJsonUserPSize()
{
	//DeleteObj(scui_obj_all);
}

//=============================================================================
// function
//      reset
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
BOOL CShJsonUserPSize::reset()
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;
	long			*pl = NULL;

	USERPAPERSIZEDATA	upsd;

	USERPAPERSIZEDATA	lpsd;

	USERPAPERSIZEDATA	cupsd;
	
	DWORD			i;

	LONG Index[2] = { DMPAPER_CUSTOM,DMPAPER_CUSTOM_ZOOM };
	// ================================================
	// _/_/_/  既に書き込まれている場合はresetしない
	// ================================================
	if(ReadUPSCount() > 1)
		return TRUE;

	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	//(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

// 長尺印刷機能対応
	/*(CreateObj(scui_obj_cls_multichk) == FALSE)
		goto EXIT;
*/
	// ================================================
	// _/_/_/  初期化情報取得と書き込み
	// ================================================
	SecureZeroMemory(&upsd, sizeof(upsd));
	//GetUPSDefData(&upsd);

	//SecureZeroMemory(&lpsd, sizeof(lpsd));
	//GetLPSDefData(&lpsd);

	//dwCount = (DWORD)(*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_CUSTOM_PAPERSIZE);
	//dwCount = 2;
	//if(dwCount == 0)
	//{
	//	// paper_custom
	//	WriteUPSData(paper_custom, &upsd);
	//	dwCount = 1;
	//}
	//else
	//{
	//	pl = (*m_pmcf).GetLongArrayData();
	//	for(i = 0; i < dwCount; i++)
	//	{

	//		if((*m_pmultichk).IsCustomPaperSize((short)pl[i], CUSTOMPSIZE_MODE_LNG))
	//		{	// Long Paper Size
	//			WriteUPSData(pl[i], &lpsd);
	//		}
	//		else
	//		{	// Custom Paper or User Paper
	//			if ((short)pl[i] != 256)
	//			{
	//				SecureZeroMemory(&cupsd, sizeof(cupsd));
	//				GetUPSDefData(&cupsd, (long)pl[i]);
	//				WriteUPSData(pl[i], &cupsd);
	//			}
	//			else
	//				WriteUPSData(pl[i], &upsd);
	//		}
	//	}
	//}

	dwCount = ReadUPSCount();
	if (dwCount == 2)
		goto EXIT;

	dwCount = 2;
	GetUPSDefData(&upsd);
	for (i = 0; i < dwCount; i++)
	{
		WriteUPSData(Index[i], &upsd);
	}
	//WriteUPSCount(dwCount);
	//blRet = TRUE;

	WriteUPSCount(dwCount);

	WriteJsonDataToFile();

	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      resetUs
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
BOOL CShJsonUserPSize::resetUs(long lIndex)
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;
	long			*pl = NULL;

	USERPAPERSIZEDATA	upsd;
	USERPAPERSIZEDATA	lpsd;
	USERPAPERSIZEDATA	cupsd;
	
	DWORD			i;

	// ================================================
	// _/_/_/  INIファイルクラス確定
	// ================================================
	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//{
	//	goto EXIT;
	//}

	//(CreateObj(scui_obj_cls_multichk) == FALSE)
 //  {
	//	goto EXIT;
	//}

	// ================================================
	// _/_/_/  初期化情報取得と書き込み
	// ================================================
	SecureZeroMemory(&upsd, sizeof(upsd));
	//GetUPSDefData(&upsd);

	//SecureZeroMemory(&lpsd, sizeof(lpsd));
	//GetLPSDefData(&lpsd);

	//dwCount = (DWORD)(*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_CUSTOM_PAPERSIZE);
	//if(dwCount == 0){
	//	// paper_custom
	//	WriteUPSDataToUs(lIndex, paper_custom, &upsd);

	//	dwCount = 1;
	//}
	//else{
	//	pl = (*m_pmcf).GetLongArrayData();
	//	for(i = 0; i < dwCount; i++){
	//		//if((*m_pmultichk).IsCustomPaperSize((short)pl[i], CUSTOMPSIZE_MODE_LNG)){
	//		//	// Long Paper Size
	//		//	if((short)pl[i] == paper_long){
	//		//		WriteUPSDataToUs(lIndex, paper_long, &lpsd);
	//		//	}
	//		//	else{
	//		//		WriteUPSDataToUs(lIndex, paper_long_out, &lpsd);
	//		//	}
	//		//}
	//		//else{
	//			// Custom Paper or User Paper
	//			if((short)pl[i] == paper_custom){
	//				WriteUPSDataToUs(lIndex, paper_custom, &upsd);
	//			}
	//			else{
	//				SecureZeroMemory(&cupsd, sizeof(cupsd));
	//				GetUPSDefData(&cupsd, (long)pl[i]);
	//				WriteUPSDataToUs(lIndex, pl[i], &cupsd);
	//			}
	//		//}
	//	}
	//}

	//WriteUPSCount(JSON_KEY_UPS_ROOT_BASE_US, lIndex, dwCount);

	//blRet = TRUE;


	LONG Index[2] = { DMPAPER_CUSTOM,DMPAPER_CUSTOM_ZOOM };
	//DWORD			i;

	// ================================================
	// _/_/_/  pszSvrPrnName
	// ================================================
	if (m_pszSvrPrnName == NULL) {
		goto EXIT;
	}

	dwCount = ReadUPSCount();
	/*if (dwCount == 2)
		goto EXIT;*/

	dwCount = 2;
	GetUPSDefData(&upsd);
	for (i = 0; i < dwCount; i++)
	{
		WriteUPSDataToUs(lIndex, Index[i], &upsd);
	}
	WriteUPSCount(JSON_KEY_UPS_ROOT_BASE_US, lIndex, dwCount);
	blRet = TRUE;

EXIT:

	return blRet;
}

BOOL CShJsonUserPSize::resetEx(LONG index)
{
	BOOL			blRet = FALSE;
	DWORD			dwCount = 0;

	USERPAPERSIZEDATA	upsd;
	DWORD			i;

	if (m_pszSvrPrnName == NULL)
		goto EXIT;

	SecureZeroMemory(&upsd, sizeof(upsd));

	GetUPSDefData(&upsd);
	WriteUPSData(index, &upsd);

	dwCount = 2;
	WriteUPSCount(dwCount);
	blRet = TRUE;

EXIT:

	return blRet;
}

//=============================================================================
// function
//      WriteUPSData
//
// parameters
//      lIndex              書き込むインデックス
//      pwmd                書き込むUSERPAPERSIZEDATA構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたUSERPAPERSIZEDATA構造体の内容を指定のインデックスでレジストリに
//      書き込む
//=============================================================================
BOOL CShJsonUserPSize::WriteUPSData(long lIndex, LPUSERPAPERSIZEDATA pupsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  USERPAPERSIZEDATA確定
	// ================================================
	if(pupsd == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  書き込み
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_UPS_INDEX_BASE, lIndex);

	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/
	
	blRet = xWriteUPSData(pszKey,pupsd);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      WriteUPSData
//
// parameters
//      lIndex              書き込むインデックス
//      pwmd                書き込むUSERPAPERSIZEDATA構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたUSERPAPERSIZEDATA構造体の内容を指定のインデックスでレジストリに
//      書き込む
//=============================================================================
BOOL CShJsonUserPSize::WriteUPSDataToUs(long lIndexUs, long lIndex, LPUSERPAPERSIZEDATA pupsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  USERPAPERSIZEDATA確定
	// ================================================
	if(pupsd == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  書き込み
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_UPS_INDEX_BASE_US, lIndexUs, lIndex);

	blRet = xWriteUPSData(pszKey,pupsd);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      WriteUPSCount
//
// parameters
//      dwCount             書き込む値
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      UPS数を書き込む
//=============================================================================
BOOL CShJsonUserPSize::WriteUPSCount(DWORD dwCount)
{
	BOOL			blRet = FALSE;

	// --- count
	if(m_pParent != NULL)
	{
		m_pParent->WriteJsonDWORDData(JSON_KEY_UPS_ROOT_BASE, JSON_ENT_UPS_COUNT, dwCount);
	}	
	blRet = TRUE;

	return blRet;
}


//=============================================================================
// function
//      WriteCount
//
// parameters
//      dwCount             書き込む値
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      UserSetting数を書き込む
//=============================================================================
BOOL CShJsonUserPSize::WriteUPSCount(WCHAR FAR *pszBase, long lIndex, DWORD dwCount)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	DWORD			dwData = 0;

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
		m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_UPS_COUNT, dwCount);
	}
	
	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      ReadUPSData
//
// parameters
//      lIndex              読み取るインデックス
//      pwmd                読み取るUSERPAPERSIZEDATA構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのUPS情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonUserPSize::ReadUPSData(long lIndex, LPUSERPAPERSIZEDATA pupsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*psz = NULL;

	BOOL			blRetRead = FALSE;

	// ================================================
	// _/_/_/  USERPAPERSIZEDATA確定
	// ================================================
	if(pupsd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pupsd, sizeof(USERPAPERSIZEDATA));

	// ================================================
	// _/_/_/  読み込み
	// ================================================
	// --- dwSignature
	psz = (WCHAR FAR *)&(*pupsd).dwSignature;
	*psz = 'U';
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
	::wsprintf(pszKey, JSON_KEY_UPS_INDEX_BASE, lIndex);

	blRet = xReadUPSData(pszKey,pupsd);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      ReadUPSDataToUs
//
// parameters
//      lIndex              読み取るインデックス
//      pwmd                読み取るUSERPAPERSIZEDATA構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのUPS情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonUserPSize::ReadUPSDataToUs(long lIndexUs, long lIndex, LPUSERPAPERSIZEDATA pupsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	WCHAR FAR		*psz = NULL;

	// ================================================
	// _/_/_/  USERPAPERSIZEDATA確定
	// ================================================
	if(pupsd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pupsd, sizeof(USERPAPERSIZEDATA));

	// ================================================
	// _/_/_/  読み込み
	// ================================================
	// --- dwSignature
	psz = (WCHAR FAR *)&(*pupsd).dwSignature;
	*psz = 'U';
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
	::wsprintf(pszKey, JSON_KEY_UPS_INDEX_BASE_US, lIndexUs, lIndex);

	blRet = xReadUPSData(pszKey,pupsd);

EXIT:
	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      ReadUPSCount
//
// parameters
//
// return value
//      成功 : 登録UPS数
//      失敗 : 0
//
// outline
//      UPS数を読み込む
//=============================================================================
DWORD CShJsonUserPSize::ReadUPSCount()
{
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	if(m_pParent != NULL)
		blSuccess = m_pParent->ReadJsonDWORDData(JSON_KEY_UPS_ROOT_BASE,JSON_ENT_UPS_COUNT,dwData);

	return dwData;
}


//=============================================================================
// function
//      ReadUPSCount
//
// parameters
//
// return value
//      成功 : 登録UPS数
//      失敗 : 0
//
// outline
//      UPS数を読み込む
//=============================================================================
DWORD CShJsonUserPSize::ReadUPSCount(WCHAR *pszBase, long lIndex)
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
		blSuccess = m_pParent->ReadJsonDWORDData(pszKey,JSON_ENT_UPS_COUNT,dwData);

EXIT:
	if(pszKey != NULL)
		delete[] pszKey;

	return dwData;
}


//=============================================================================
// function
//      GetUPSDefData
//
// parameters
//      lIndex              情報を取得するインデックス
//      pwmd                情報を受け取るUSERPAPERSIZEDATA構造体アドレス
//      pIniFile            CShIniFile クラスアドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたINIファイルのデフォルトUPSを取得する
//=============================================================================
BOOL CShJsonUserPSize::GetUPSDefData(LPUSERPAPERSIZEDATA pupsd, long lIndex)
{
//	BOOL			blRet = FALSE;
//	long			lCount = 0;
//	long			lData[4] = {0};
//
//	WCHAR FAR		*pszKey		= NULL;
//
//	WCHAR FAR		*pc = NULL;
//	long FAR		*pl = NULL;
//
//	short			wUnit;
//
//	long			lPaperSize;
//	long			lPaperKind;
//
//	MIP_MINMAX_CONV_PARAM	stCnvParam;
//
//	// ================================================
//	// _/_/_/  パラメータチェック
//	// ================================================
//	if(pupsd == NULL)
//		goto EXIT;
//
//	// ================================================
//	// _/_/_/  INIファイルクラス確定
//	// ================================================
//	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
//		goto EXIT;*/
//
//	/*if(CreateObj(scui_obj_cls_multichk) == FALSE)
//		goto EXIT;
//*/
//	// ================================================
//	// _/_/_/  情報の取得
//	// ================================================
//	if (0 != lIndex)
//	{
//		lPaperSize = lIndex;
//		pszKey		= new WCHAR[SCUI_REGKEYSIZE];
//		if (pszKey != NULL)
//		{
//			memset(pszKey, 0, sizeof(WCHAR)*SCUI_REGKEYSIZE);
//			::wsprintf(pszKey, MCF_KEY_PROP_SUP_CUSTOMUSERPAPER, lIndex);
//
//			lCount = (*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, pszKey, lData, sizeof(lData) / sizeof(long));
//		}
//	}
//
//	if (0 == lCount)
//	{
//		lPaperSize = paper_custom;
//		// CustomPaper1...の用紙サイズ取得失敗 or lIndex == 0の場合。
//		// 最小のサイズ
//		lCount = (*m_pmcf).GetLongArray(MCF_SEC_PRNOPTONS, MCF_KEY_PRNOPT_DC_MINEXTENT, lData, sizeof(lData) / sizeof(long));
//	}
//
//	if(lCount != 0)
//	{
//		lPaperKind = (*m_pmultichk).getMIPPaper((short)lPaperSize);
//
//		// ================================================
//		// _/_/_/  情報の作成
//		// ================================================
//		// --- dwSignature
//		pc = (WCHAR FAR *)&(*pupsd).dwSignature;
//		*pc = 'U';
//		pc++;
//		*pc = 'P';
//		pc++;
//		*pc = 'D';
//		pc++;
//		*pc = 'T';
//		(*pupsd).dwWidth = (DWORD)lData[0];
//		(*pupsd).dwLength = (DWORD)lData[1];
//		if(lData[2] == MM_LOENGLISH)
//			(*pupsd).wUnit = unit_inch;
//		else
//			(*pupsd).wUnit = unit_mm;
//
//		if(lCount == 4)
//		{
//			if(lData[3] == -1)
//				wUnit = GetUnitDef();
//			else if(lData[3] == MM_LOENGLISH)
//				wUnit = unit_inch;
//			else
//				wUnit = unit_mm;
//
//			if((*pupsd).wUnit != wUnit)
//			{
//				stCnvParam.paper = lPaperKind;
//				if(wUnit == unit_inch)
//				{
//					stCnvParam.mi = mip_mi_inch;
//					stCnvParam.pos = 1;
//
//					stCnvParam.wl = mip_wl_width;
//					(*pupsd).dwWidth	= (short)CShMIPCnv::MilliToInch_CusPaper((*pupsd).dwWidth, MM_LOMETRIC, MM_LOENGLISH, &stCnvParam);
//
//					stCnvParam.wl = mip_wl_length;
//					(*pupsd).dwLength	= (short)CShMIPCnv::MilliToInch_CusPaper((*pupsd).dwLength, MM_LOMETRIC, MM_LOENGLISH, &stCnvParam);
//					{
//						WORD	wMix;
//						WORD	wMax;
//
//						stCnvParam.mi = mip_mi_inch;
//						stCnvParam.pos = 1;
//						stCnvParam.wl = mip_wl_width;
//						CShMIPCnv::getMinMax(&stCnvParam, &wMix, &wMax);
//						if(wMix < (*pupsd).dwWidth){
//							(*pupsd).dwWidth = wMix;
//						}
//						stCnvParam.wl = mip_wl_length;
//						CShMIPCnv::getMinMax(&stCnvParam, &wMix, &wMax);
//						if(wMix < (*pupsd).dwLength){
//							(*pupsd).dwLength = wMix;
//						}
//					}
//					(*pupsd).wUnit = unit_inch;
//				}
//				else
//				{
//					stCnvParam.mi = mip_mi_mill;
//					stCnvParam.pos = 1;
//
//					stCnvParam.wl = mip_wl_width;
//					(*pupsd).dwWidth	= (short)CShMIPCnv::InchToMilli_CusPaper((*pupsd).dwWidth, MM_LOENGLISH, MM_LOMETRIC, &stCnvParam);
//					stCnvParam.wl = mip_wl_length;
//					(*pupsd).dwLength	= (short)CShMIPCnv::InchToMilli_CusPaper((*pupsd).dwLength, MM_LOENGLISH, MM_LOMETRIC, &stCnvParam);
//					(*pupsd).wUnit = unit_mm;
//				}
//			}
//		}
//	}
//	blRet = TRUE;


	BOOL			blRet = FALSE;

	WCHAR			*pc = NULL;
	long			*pl = NULL;

	if (pupsd == NULL)
		goto EXIT;

	SecureZeroMemory(pupsd, sizeof(USERPAPERSIZEDATA));


	// --- dwSignature
	pc = (WCHAR FAR *)&(*pupsd).dwSignature;
	*pc = 'U';
	pc++;
	*pc = 'P';
	pc++;
	*pc = 'D';
	pc++;
	*pc = 'T';

	DWORD m_Width = PaperSizeArray[20][1];
	DWORD m_Length = PaperSizeArray[20][2];
	SHORT m_Unit = GetUnitDef();
	if (unit_mm == m_Unit)
	{
		m_Width = InchToMilli(m_Width, MM_LOENGLISH, MM_LOMETRIC);
		m_Length = InchToMilli(m_Length, MM_LOENGLISH, MM_LOMETRIC);
	}
	(*pupsd).dwWidth = m_Width;
	(*pupsd).dwLength = m_Length;
	if (unit_inch == m_Unit)
		(*pupsd).wUnit = unit_inch;
	else
		(*pupsd).wUnit = unit_mm;
	blRet = TRUE;

EXIT:
	//if (pszKey != NULL)
	//{
	//	delete[] pszKey;
	//	pszKey = NULL;
	//}
	return blRet;
}

// 長尺印刷機能対応
//=============================================================================
// function
//      GetLPSDefData
//
// parameters
//      lIndex              情報を取得するインデックス
//      pwmd                情報を受け取るUSERPAPERSIZEDATA構造体アドレス
//      pIniFile            CShIniFile クラスアドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたINIファイルのデフォルトUPSを取得する
//=============================================================================
BOOL CShJsonUserPSize::GetLPSDefData(LPUSERPAPERSIZEDATA pupsd)
{
	BOOL			blRet = FALSE;

	//long			lCountW;
	//long			lCountL;
	//long			lDataW[5];
	//long			lDataL[5];
	//short			wUnit;

	//WCHAR FAR		*pc = NULL;
	//long FAR		*pl = NULL;

	//MIP_MINMAX_CONV_PARAM	stCnvParam;

	//// ================================================
	//// _/_/_/  パラメータチェック
	//// ================================================
	//if(pupsd == NULL)
	//	goto EXIT;

	//// ================================================
	//// _/_/_/  INIファイルクラス確定
	//// ================================================
	///*if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;*/

	//// ================================================
	//// _/_/_/  情報の取得
	//// ================================================
	//// 最小のサイズ
	//lCountW = (*m_pmcf).GetLongArray(MCF_SEC_VALUERANGE, MCF_KEY_VAL_LP_WIDTH,  lDataW, sizeof(lDataW) / sizeof(long));
	//lCountL = (*m_pmcf).GetLongArray(MCF_SEC_VALUERANGE, MCF_KEY_VAL_LP_LENGTH, lDataL, sizeof(lDataL) / sizeof(long));
	//if(lCountW != 0 && lCountL != 0)
	//{
	//	// ================================================
	//	// _/_/_/  情報の作成
	//	// ================================================
	//	// --- dwSignature
	//	pc = (WCHAR FAR *)&(*pupsd).dwSignature;
	//	*pc = 'U';
	//	pc++;
	//	*pc = 'P';
	//	pc++;
	//	*pc = 'D';
	//	pc++;
	//	*pc = 'T';
	//	(*pupsd).dwWidth  = (DWORD)lDataW[1];
	//	(*pupsd).dwLength = (DWORD)lDataL[1];
	//	if(lDataW[3] == MM_LOENGLISH)
	//		(*pupsd).wUnit = unit_inch;
	//	else
	//		(*pupsd).wUnit = unit_mm;

	//	if(lCountW == 5)
	//	{
	//		if(lDataW[4] == -1)
	//			wUnit = GetUnitDef();
	//		else if(lDataW[4] == MM_LOENGLISH)
	//			wUnit = unit_inch;
	//		else
	//			wUnit = unit_mm;

	//		if((*pupsd).wUnit != wUnit)
	//		{
	//			stCnvParam.paper = mip_paper_long;
	//			stCnvParam.mi = mip_mi_inchmm;
	//			stCnvParam.pos = 1;
	//			if(wUnit == unit_inch)
	//			{
	//				stCnvParam.wl = mip_wl_width;
	//				(*pupsd).dwWidth	= (short)CShMIPCnv::MilliToInch_CusPaper((*pupsd).dwWidth, MM_LOMETRIC, MM_LOENGLISH, &stCnvParam);
	//				stCnvParam.wl = mip_wl_length;
	//				(*pupsd).dwLength	= (short)CShMIPCnv::MilliToInch_CusPaper((*pupsd).dwLength, MM_LOMETRIC, MM_LOENGLISH, &stCnvParam);
	//				(*pupsd).wUnit = unit_inch;
	//			}
	//			else
	//			{
	//				stCnvParam.wl = mip_wl_width;
	//				(*pupsd).dwWidth	= (short)CShMIPCnv::InchToMilli_CusPaper((*pupsd).dwWidth, MM_LOENGLISH, MM_LOMETRIC, &stCnvParam);
	//				stCnvParam.wl = mip_wl_length;
	//				(*pupsd).dwLength	= (short)CShMIPCnv::InchToMilli_CusPaper((*pupsd).dwLength, MM_LOENGLISH, MM_LOMETRIC, &stCnvParam);
	//				(*pupsd).wUnit = unit_mm;
	//			}
	//		}
	//	}
	//}

EXIT:

	return blRet;
}


//=============================================================================
// function
//      CreateObj
//
// parameters
//      lFlag               対象クラスフラグ
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      指定されたオブジェクトが未作成の場合作成する
//=============================================================================
BOOL CShJsonUserPSize::CreateObj(long lFlag)
{
	//BOOL			blRet = FALSE;

	//switch(lFlag)
	//{
	//	// ================================================
	//	// _/_/_/  CShMultiChk
	//	// ================================================
	//	case scui_obj_cls_multichk:

	//		if(m_pmultichk != NULL)
	//			return TRUE;

	//		m_pmultichk = new CShMultiChk(m_hInst, m_pszSvrPrnName/*, m_pLog*/);
	//		if(m_pmultichk != NULL)
	//		{
	//			m_blDelMulChk	= TRUE;
	//			blRet			= TRUE;
	//		}

	//		break;

	//	default:
	//		blRet = CShObjHold::CreateObj(lFlag);
	//		break;
	//}

	//return blRet;
	return 0;
}


//=============================================================================
// function
//      DeleteObj
//
// parameters
//      lFlag               対象クラスフラグ
//
// return value
//      無し
//
// outline
//      指定されたオブジェクトを削除する
//=============================================================================
void CShJsonUserPSize::DeleteObj(long lFlag)
{
	//switch(lFlag)
	//{
	//	// ================================================
	//	// _/_/_/  CShMultiChk
	//	// ================================================
	//	case scui_obj_all:
	//	case scui_obj_cls_multichk:
	//		if(m_blDelMulChk != FALSE)
	//		{
	//			if(m_pmultichk != NULL)
	//			{
	//				delete m_pmultichk;
	//				m_pmultichk = NULL;
	//				m_blDelMulChk = FALSE;
	//			}
	//		}

	//		if(lFlag != scui_obj_all)
	//			break;

	//	default:
	//		CShObjHold::DeleteObj(lFlag);
	//		break;
	//}
}

//=============================================================================
// function
//      xReadUPSData
//
// parameters
//      pszKeyPath          UPS key path
//      pupsd                読み取るUPS構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのUPS情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonUserPSize::xReadUPSData(WCHAR *pszKeyPath,LPUSERPAPERSIZEDATA pupsd)
{
	BOOL	bRet = FALSE;
	DWORD	dwData = 0;
	
	if(pszKeyPath == NULL || pupsd == NULL)
		goto EXIT;

	if(m_pParent != NULL)
	{
		// --- dwWidth
		bRet = m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_UPS_DWWIDTH,dwData);
		if(bRet == FALSE)
			goto EXIT;

		(*pupsd).dwWidth = (SHORT)dwData;

		// --- dwLength
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_UPS_DWLENGTH,dwData);
		(*pupsd).dwLength = (LONG)dwData;

		// --- wUnit
		m_pParent->ReadJsonDWORDData(pszKeyPath,JSON_ENT_UPS_WUNIT,dwData);
		(*pupsd).wUnit = (short)dwData;

		bRet = TRUE;
	}

EXIT:
	return bRet;
}

//=============================================================================
// function
//      xWriteUPSData
//
// parameters
//      pszKeyPath          UPS key path
//      pmsd                読み取るMS構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALE
//
// outline
//      指定されたインデックスのUPS情報をでレジストリから読み込む
//=============================================================================
BOOL CShJsonUserPSize::xWriteUPSData(WCHAR *pszKeyPath,LPUSERPAPERSIZEDATA pupsd)
{
	BOOL	bRet = FALSE;
	DWORD	dwData = 0;
	
	if(pszKeyPath == NULL || pupsd == NULL)
		goto EXIT;

	if(m_pParent != NULL)
	{
		// --- dwWidth
		dwData = (*pupsd).dwWidth;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_UPS_DWWIDTH, dwData);

		// --- dwLength
		dwData = (*pupsd).dwLength;
		m_pParent->WriteJsonDWORDData(pszKeyPath, JSON_ENT_UPS_DWLENGTH, dwData);

		// --- wUnit
		dwData = (DWORD)(*pupsd).wUnit;
		m_pParent->WriteJsonDWORDData(pszKeyPath,JSON_ENT_UPS_WUNIT, dwData);

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
void CShJsonUserPSize::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_US);
}
// <E> 2022.05.17 sbc R.Tanaka
