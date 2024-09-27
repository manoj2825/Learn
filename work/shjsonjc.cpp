// ============================================================================
// shjsonjc.cpp
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.04.24
// ============================================================================

#include "precomp.h""
#include "shjsonjc.h"
#include "aes.h"
#define MAX_ENCODING_STRING_LENGTH 16
#define PARENT_CLASSNAME 	CShJson
#define MY_CLASSNAME 		CShJsonJC

//=============================================================================
// function
//      CShJsonJC
//
// parameters
//      hInst               モジュールハンドル
//      pLog                CShPrnLog クラスアドレス
//
// return value
//      無し
//
// outline
//      CShJsonJC クラスコンストラクタ
//=============================================================================
CShJsonJC::CShJsonJC(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName/*, CShPrnLog FAR *pLog*/)
			: CShJson(hInst, pszSvrPrnName/*, pLog*/)
{
	SetParent(this);
}


//=============================================================================
// function
//      ~CShJsonJC
//
// parameters
//      無し
//
// return value
//      無し
//
// outline
//      CShJsonJC クラスデストラクタ
//=============================================================================
CShJsonJC::~CShJsonJC()
{
}


//=============================================================================
// function
//      InitJobCtrl
//
// parameters
//      wDefUName           デフォルトユーザー名リソースID
//      wDefJName           デフォルトジョブ名リソースID
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      Job Control情報を初期化する
//=============================================================================
BOOL CShJsonJC::InitJobCtrl(short wDefUName, short wDefJName)
{
	BOOL			blRet = FALSE;

//	WCHAR			sz[256];
//
//	if(m_pParent == NULL)
//		goto EXIT;
//
//	LoadDll(scui_dll_str);
//	//if(IsStrResNotUseMB())
//	//{
//	//	//CShString	st;
//
//	//	//// --- user name
//	//	//st = LoadStr(wDefUName, sizeof(sz));
//	//	//m_pParent->WriteJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_USERNAME, st.GetPtr(), st.GetLength());
//
//	//	//// --- job name
//	//	//st = LoadStr(wDefJName, sizeof(sz));
//	//	//m_pParent->WriteJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_JOBNAME, st.GetPtr(), st.GetLength());
//	//}
//	//else
//	//{
//		// --- user name
//		SecureZeroMemory(sz, sizeof(sz));
//		LoadStr(wDefUName, sz, sizeof(sz));
//		m_pParent->WriteJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_USERNAME, sz, ::lstrlen(sz), TRUE);
//
//		// --- job name
//		SecureZeroMemory(sz, sizeof(sz));
//		LoadStr(wDefJName, sz, sizeof(sz));
//		m_pParent->WriteJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_JOBNAME, sz, ::lstrlen(sz), TRUE);
//	//}
//
//	// --- PIN
//	// --- account number
//	// --- folder pass
//	// --- login name
//	// --- login password
//	SecureZeroMemory(sz, sizeof(sz));
//	m_pParent->WriteJsonBinaryData(JSON_KEY_JC_ROOT_BASE,JSON_ENT_JC_PIN,sz, 2);
//	m_pParent->WriteJsonBinaryData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ACNUM, sz, 2);
//	m_pParent->WriteJsonBinaryData(JSON_KEY_JC_ROOT_BASE,JSON_ENT_JC_FOLDERPASS,sz, 2);
//	m_pParent->WriteJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_LOGINNAME, sz, 2);
//	m_pParent->WriteJsonBinaryData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_LOGINPASS, sz, 2);
//
//	blRet = TRUE;
//
//EXIT:
//
return blRet;
}


//=============================================================================
// function
//      ReadJCData
//
// parameters
//      pjc                 値を受け取るREGJCINFO構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      Job Control情報を取得する
//=============================================================================
BOOL CShJsonJC::ReadJCData(PREGJCINFO pjc, WORD wEicoh)
{
	BOOL			blRet = FALSE;

	DWORD			dwSize = 0;
	DWORD			dwVal = 0;

	WCHAR FAR		*pBuff = NULL;

// ログイン名のデフォルトにWindowsログイン名を入れる対応
	long			lEicohFlg;

	long			lCount = 0;
	long FAR		*plData = new long[SCUI_LONGDATA_MAX];
	if(plData == NULL){
		goto EXIT;
	}
		
	//(*pjc).dwUserNameSize = (JC_STR_JUNAME+1) * 2;
	//(*pjc).dwAccountNumSize = JC_STR_ACCOUNTNUM + 1;
	//(*pjc).dwLoginNameSize = (JC_STR_LOGINNAME+1) * 2;
	//(*pjc).dwLoginPasswordSize = JC_STR_LOGINPASS + 1;
	//(*pjc).dwPINSize = JC_STR_PINLENGTH + 1;
	//(*pjc).dwFolderPassSize = JC_STR_ACCOUNTNUM + 1;
	//(*pjc).dwSingleSignOnNameSize = (JC_STR_LOGINNAME + 1)*2;

	// ================================================
	// _/_/_/  パラメータチェック
	// ================================================
	if(m_pParent == NULL || pjc == NULL)
		goto EXIT;

	if (wEicoh == TRUE)
	{
		lEicohFlg = 1;
	}
	else
	{
		lEicohFlg = 0;
	}


	pBuff = new WCHAR[SCUI_CIPHER_BUFSIZE];
	if(pBuff == NULL)
		goto EXIT;

	// --- wUsePIN
	if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ALWAYSPIN, dwVal) != FALSE)
		(*pjc).wUsePIN = (short)dwVal;

	// --- wUseAcNum
	if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ALWAYSACNUM, dwVal) != FALSE)
		(*pjc).wUseAcNum = (short)dwVal;

	// --- wUseUName
	if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ALWAYSUNAME, dwVal) != FALSE)
		(*pjc).wUseUName = (short)dwVal;

	// --- wUseJName
	if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ALWAYSJNAME, dwVal) != FALSE)
		(*pjc).wUseJName = (short)dwVal;

	// --- wLoginName
	if(lEicohFlg != bool_false)
	{
		(*pjc).wLoginName = BST_CHECKED;
	}
	else
	{
		if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_SETLNAME, dwVal) != FALSE){
			(*pjc).wLoginName = (short)dwVal;
		}
	}

	// --- wSingleSignOn
	if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_SETSINGLESIGNON, dwVal) != FALSE){
		(*pjc).wSingleSignOn = (short)dwVal;
	}

	// --- wLoginPass
	if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_SETLPASS, dwVal) != FALSE)
		(*pjc).wLoginPass = (short)dwVal;

	// --- szUserName
	if((*pjc).wUseUName != 0)
	{	
		//if( (*pjc).pUserName != NULL)
		//{
		//	delete [](*pjc).pUserName;
		//	(*pjc).pUserName = NULL;
		//}

		//(*pjc).pUserName = new char[(*pjc).dwUserNameSize];		
		if((*pjc).szUserName != NULL)
		{		
			SecureZeroMemory((*pjc).szUserName, (*pjc).wUseUName);
			dwSize = /*(*pjc).szUserName;*/sizeof((*pjc).szUserName);
			m_pParent->ReadJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_USERNAME, (*pjc).szUserName, dwSize);
		}
	}

	// --- szJobName
	if((*pjc).wUseJName != 0)
	{
		dwSize = sizeof((*pjc).szJobName);
		m_pParent->ReadJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_JOBNAME, (*pjc).szJobName, dwSize);
	}

	// --- szLoginName
	if((*pjc).wLoginName != 0)
	{
/*		if( (*pjc).pLoginName != NULL)
		{
			delete [](*pjc).pLoginName;
			(*pjc).pLoginName = NULL;
		}

		(*pjc).pLoginName = new char[(*pjc).dwLoginNameSize];*/		
		//if((*pjc).szLoginName != NULL)
		//{
			SecureZeroMemory((*pjc).szLoginName, (*pjc).wLoginName);

		
		if(lEicohFlg != bool_false)
		{
			dwSize = sizeof((*pjc).szLoginName) / sizeof(WCHAR);		//Coverity Fix -91172
			GetUserName((*pjc).szLoginName, &dwSize);
		}
		else
		{
			if((*pjc).szLoginName != NULL)
			{
				if(m_pParent->ReadJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_LOGINNAME, (*pjc).szLoginName, dwSize) == FALSE){
					SecureZeroMemory((*pjc).szLoginName, (*pjc).wLoginName);
				}
			}
		}
	}

	// --- szLoginPassword
	if((*pjc).wLoginPass != 0)
	{
		WCHAR FAR		*pTemp = NULL;

		/*if( (*pjc).pLoginPassword != NULL)
		{
			delete [](*pjc).pLoginPassword;
			(*pjc).pLoginPassword = NULL;
		}

		(*pjc).pLoginPassword = new char[(*pjc).dwLoginPasswordSize];	*/	
		if((*pjc).szLoginPassword != NULL)
		{
			SecureZeroMemory((*pjc).szLoginPassword, (*pjc).wLoginPass);
		}

		if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_LOGINPASS_SIZE, dwVal) != FALSE)
		{
			WCHAR byLoginPassword[33] = {};
			dwSize = JC_STR_LOGINPASS + 1;

			if(m_pParent->ReadJsonBinaryData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_LOGINPASS, byLoginPassword, dwSize) != FALSE)
			{
				WCHAR wszLoginPasswordUA[32];
				GetDecodedPassWordData((CHAR*)byLoginPassword,
					wszLoginPasswordUA,
					sizeof(byLoginPassword)
				);
				wcscpy_s((*pjc).szLoginPassword, sizeof((*pjc).szLoginPassword) * sizeof(WCHAR), wszLoginPasswordUA);
				(*pjc).szLoginPassword[32] = 0x00;
			}		
		}

		if(pTemp != NULL)
			delete[] pTemp;
	}

	// --- szAccountNum
	if((*pjc).wUseAcNum != 0)
	{
		WCHAR FAR		*pTemp = NULL;

		/*if( (*pjc).pAccountNum != NULL)
		{
			delete [](*pjc).pAccountNum;
			(*pjc).pAccountNum = NULL;
		}

		(*pjc).pAccountNum = new char[(*pjc).dwAccountNumSize];	*/	
		if((*pjc).szAccountNum != NULL)
		{
			SecureZeroMemory((*pjc).szAccountNum, (*pjc).wUseAcNum);
		}

		if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ACNUM_SIZE, dwVal) != FALSE)
		{
			dwSize = (short)dwVal;		
			pTemp = new WCHAR[dwSize];
			if(pTemp == NULL)
				goto EXIT;
			SecureZeroMemory(pBuff, dwSize);

			if(m_pParent->ReadJsonBinaryData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ACNUM, pBuff, dwSize) != FALSE)
			{
				if (dwSize == SCUI_CIPHER_BUFSIZE)
				{
					Decode_AES((char*)(*pjc).szAccountNum, (char*)pBuff);
				}

				//if(pTmpOut != NULL)
				//{
				//	delete []pTmpOut;
				//	pTmpOut = NULL;
				//}
			}		
		}
				
		if(pTemp != NULL)
			delete[] pTemp;
	}

	// --- szPIN
	if((*pjc).wUsePIN != 0)
	{
		WCHAR FAR		*pTemp = NULL;

		//if( (*pjc).pPIN != NULL)
		//{
		//	delete [](*pjc).pPIN;
		//	const char* temp = NULL;
		//	strcpy((*pjc).pPIN, temp);
		//	//(*pjc).pPIN = NULL;
		//}

		//(*pjc).pPIN = new char[(*pjc).dwPINSize];		
		if((*pjc).szPIN != NULL)
		{
			SecureZeroMemory((*pjc).szPIN, (*pjc).wUsePIN);
		}

		if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_PIN_SIZE, dwVal) != FALSE)
		{
			dwSize = (short)dwVal;		
			pTemp = new WCHAR[dwSize];
			if(pTemp == NULL)
				goto EXIT;
			SecureZeroMemory(pTemp, dwSize);

			if(m_pParent->ReadJsonBinaryData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_PIN, pTemp, dwSize) != FALSE)
			{
				if (dwSize == SCUI_CIPHER_BUFSIZE)
				{
					Decode_AES((char*)(*pjc).szPIN, (char*)pTemp);

				}

			}		
		}
				
		if(pTemp != NULL)
			delete[] pTemp;
	}

	// --- szFolderPass
	if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_FOLDERINDEX, dwVal) != FALSE)
	{
		if((short)dwVal == (*pjc).wFolderIndex && (*pjc).wFolderIndex != 0)
		{
			WCHAR FAR		*pTemp = NULL;

			if((*pjc).szFolderPass != NULL)
			{
				SecureZeroMemory((*pjc).szFolderPass, (*pjc).wFolderIndex);
			}

			if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_FOLDERPASS_SIZE, dwVal) != FALSE)
			{
				dwSize = (short)dwVal;
				pTemp = new WCHAR[dwSize];
				if(pTemp == NULL)
					goto EXIT;
				SecureZeroMemory(pTemp, dwSize);

				if(m_pParent->ReadJsonBinaryData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_FOLDERPASS,pTemp, dwSize) != FALSE)
				{
					if (dwSize == SCUI_CIPHER_BUFSIZE)
					{

						Decode_AES((char*)(*pjc).szFolderPass, (char*)pTemp);
					}

				}		
			}

			if(pTemp != NULL)
				delete[] pTemp;
		}
		else
		{
			SecureZeroMemory((*pjc).szFolderPass, (*pjc).wFolderIndex);
		}
	}

	// --- szSingleSignOn
	if((*pjc).wSingleSignOn != 0)
	{
		dwSize = sizeof((*pjc).szSingleSignOnName) / sizeof(WCHAR);	//Coverity Fix 91175
		GetUserName((*pjc).szSingleSignOnName, &dwSize);
	}

	blRet = TRUE;

EXIT:

	if(plData != NULL)
		delete[] plData;

	if(pBuff != NULL)
		delete[] pBuff;

	return blRet;
}


//=============================================================================
// function
//      WriteJCData
//
// parameters
//      pjc                 書き込む値を保持するREGJCINFO構造体アドレス
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//      Job Control情報をレジストリに書き込む
//=============================================================================
BOOL CShJsonJC::WriteJCData(PREGJCINFO pjc, WORD wField, WORD wEicoh)
{
	BOOL			blRet = FALSE;

	DWORD			dwData = 0;
	short			wBy = 0;
	DWORD			dwSize = 0;

	WCHAR			szBlank[8];

	WCHAR FAR		*pData = NULL;
	WCHAR FAR		*pSrc = NULL;
	WCHAR FAR		*pDst = NULL;

// ログイン名のデフォルトにWindowsログイン名を入れる対応
	long			lEicohFlg;

	// ================================================
	// _/_/_/  パラメータチェック
	// ================================================
	if(m_pParent == NULL || pjc == NULL)
		goto EXIT;

	SecureZeroMemory(szBlank, sizeof(szBlank));

	if (GetIsUnicode())
		wBy = 2;
	else
		wBy = 1;

	if (wEicoh == TRUE)
	{
		lEicohFlg = 1;
	}
	else
	{
		lEicohFlg = 0;
	}

	pSrc = new WCHAR[SCUI_CIPHER_BUFSIZE];
	pDst = new WCHAR[SCUI_CIPHER_BUFSIZE];
	if(pSrc == NULL || pDst == NULL)
		goto EXIT;

	// --- wUsePIN
	if(wField & JC_PASSCODE)
	{
		dwData = (*pjc).wUsePIN;
		m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ALWAYSPIN,dwData);
	}

	// --- wUseFolderPIN
	if(wField & JC_FOLDERPASS)
	{
		dwData = (*pjc).wFolderIndex;
		m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_FOLDERINDEX,dwData);
	}

	// --- wUseAcNum
	if(wField & JC_ACCOUNTNUM)
	{
		dwData = (*pjc).wUseAcNum;
		m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ALWAYSACNUM,dwData);
	}

	// --- wUseUName
	if(wField & JC_USERNAME)
	{
		dwData = (*pjc).wUseUName;
		m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ALWAYSUNAME,dwData);
	}

	// --- wUseJName
	if(wField & JC_JOBNAME)
	{
		dwData = (*pjc).wUseJName;
		m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ALWAYSJNAME,dwData);
	}

	// --- wLoginName
	if(lEicohFlg == bool_false)
	{
		if(wField & JC_LOGINNAME)
		{
			dwData = (*pjc).wLoginName;
			m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_SETLNAME,dwData);
		}
	}

	// --- wLoginPass
	if(wField & JC_LOGINPASS)
	{
		dwData = (*pjc).wLoginPass;
		m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_SETLPASS,dwData);
	}

	// --- wFolderIndex
	if(wField & JC_FOLDERPASS)
	{
		dwData = (*pjc).wFolderIndex;
		m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_FOLDERINDEX,dwData);
	}

	// --- wSingleSignOn
	if(wField & JC_SINGLESIGNON)
	{
		dwData = (*pjc).wSingleSignOn;
		m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_SETSINGLESIGNON,dwData);
	}

	// --- szUserName
	if(wField & JC_USERNAME)
	{
		if((*pjc).wUseUName != 0 /*&& (*pjc).pUserName != NULL && (*pjc).dwUserNameSize > 0*/)
		{
			pData = (*pjc).szUserName;
			//dwSize = (DWORD)sh_strlen(pData) * wBy;
			dwSize = (DWORD)::wcslen((const wchar_t *)pData);
		}

		else
		{
			pData = szBlank;
			dwSize = wBy;
		}

		m_pParent->WriteJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_USERNAME, pData, dwSize);
	}

	// --- szJobName
	if(wField & JC_JOBNAME)
	{
		if((*pjc).wUseJName != 0)
		{
			pData = (*pjc).szJobName;
			//dwSize = (DWORD)sh_strlen(pData) * wBy;
			dwSize = (DWORD)::wcslen((const wchar_t *)pData);
		}
		else
		{
			pData = szBlank;
			dwSize = wBy;
		}

		m_pParent->WriteJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_JOBNAME, pData, dwSize);
	}

	// --- szLoginName
	if(lEicohFlg == bool_false)
	{
		if(wField & JC_LOGINNAME)
		{
			if((*pjc).wLoginName != 0 /*&& (*pjc).pLoginName != NULL && (*pjc).dwLoginNameSize > 0*/)
			{
				pData = (*pjc).szLoginName;
				//dwSize = (DWORD)sh_strlen(pData) * wBy;	
				dwSize = (DWORD)::wcslen((const wchar_t *)pData);
			}
			else
			{
				pData = szBlank;
				dwSize = wBy;
			}

			m_pParent->WriteJsonStrData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_LOGINNAME, pData, dwSize);
		}
	}

	// --- szLoginPassword
	if(wField & JC_LOGINPASS)
	{
		//WCHAR FAR		*pTemp = NULL;

		//if((*pjc).wLoginPass != 0 && ::lstrlen((*pjc).szLoginPassword) != 0)
		//{
		//	if(Encode_AES_EX(NULL,(*pjc).szLoginPassword,dwSize) == TRUE)
		//	{
		//		pTemp = new WCHAR[dwSize];
		//		if(pTemp == NULL)
		//			goto EXIT;
		//		SecureZeroMemory(pTemp, dwSize);
		//		pData = pTemp;

		//		if(Encode_AES_EX(pTemp,(*pjc).szLoginPassword,dwSize) == TRUE)
		//		{
		//			m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_LOGINPASS_SIZE,dwSize);
		//		}
		//	}
		//}

		//else
		//{
		//	pData = szBlank;
		//	dwSize = 1;
		//}

		WCHAR byLoginPassword[JC_STR_LOGINPASS + 1] = {};
		if ((*pjc).wLoginPass != 0 && ::lstrlen((*pjc).szLoginPassword) != 0)
		{
			dwSize = wcslen((*pjc).szLoginPassword) + 1;
			GetEncodedPassWordData((CHAR*)byLoginPassword,
				(*pjc).szLoginPassword,
				JC_STR_LOGINPASS + 1,
				dwSize
			);
			dwSize = wcslen(byLoginPassword) * sizeof(WCHAR);
		}
		else
		{
			::memcpy(byLoginPassword, szBlank, sizeof(szBlank));
			dwSize = wBy;
		}

		m_pParent->WriteJsonBinaryData(JSON_KEY_JC_ROOT_BASE,JSON_ENT_JC_LOGINPASS, byLoginPassword, dwSize);

		//if(pTemp != NULL)
		//	delete[] pTemp;
	}

	// --- szAccountNum
	if(wField & JC_ACCOUNTNUM)
	{
		//WCHAR FAR		*pTemp = NULL;		
		//if((*pjc).wUseAcNum != 0 && ::lstrlen((*pjc).szAccountNum) != 0)
		//{
		//	if(Encode_AES_EX(NULL,(*pjc).szAccountNum,dwSize) == TRUE)
		//	{
		//		pTemp = new WCHAR[dwSize];
		//		if(pTemp == NULL)
		//			goto EXIT;
		//		SecureZeroMemory(pTemp, dwSize);
		//		pData = pTemp;

		//		if(Encode_AES_EX(pTemp,(*pjc).szAccountNum,dwSize) == TRUE)
		//		{
		//			m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ACNUM_SIZE,dwSize);
		//		}
		//	}
		//}
		//else
		//{
		//	pData = szBlank;
		//	dwSize = 1;
		//}

		if ((*pjc).wUseAcNum != 0 && ::lstrlen((*pjc).szAccountNum) != 0)
		{
			SecureZeroMemory(pSrc, SCUI_CIPHER_BUFSIZE);
			SecureZeroMemory(pDst, SCUI_CIPHER_BUFSIZE);

			::memcpy(pSrc, (*pjc).szAccountNum, sizeof((*pjc).szAccountNum));
			Encode_AES((char*)pDst, (char*)pSrc);

			pData = pDst;
			dwSize = SCUI_CIPHER_BUFSIZE;
		}
		else
		{
			pData = szBlank;
			dwSize = 1;
		}

		m_pParent->WriteJsonBinaryData(JSON_KEY_JC_ROOT_BASE,JSON_ENT_JC_ACNUM,pData, dwSize);
		
		//if(pTemp != NULL)
		//	delete[] pTemp;
	}

	// --- szPIN
	if(wField & JC_PASSCODE)
	{
		//WCHAR FAR		*pTemp = NULL;		
		//if((*pjc).wUsePIN != 0 && ::lstrlen((*pjc).szPIN) != 0)
		//{
		//	if(Encode_AES_EX(NULL,(*pjc).szPIN,dwSize) == TRUE)
		//	{
		//		pTemp = new WCHAR[dwSize];
		//		if(pTemp == NULL)
		//			goto EXIT;
		//		SecureZeroMemory(pTemp, dwSize);
		//		pData = pTemp;

		//		if(Encode_AES_EX(pTemp,(*pjc).szPIN,dwSize) == TRUE)
		//		{
		//			m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_PIN_SIZE,dwSize);
		//		}
		//	}
		//}
		//else
		//{
		//	pData = szBlank;
		//	dwSize = 1;
		//}
		if ((*pjc).wUsePIN != 0 && ::lstrlen((*pjc).szPIN) != 0)
		{
			SecureZeroMemory(pSrc, SCUI_CIPHER_BUFSIZE);
			SecureZeroMemory(pDst, SCUI_CIPHER_BUFSIZE);

			::memcpy(pSrc, (*pjc).szPIN, sizeof((*pjc).szPIN));
			Encode_AES((char*)pDst, (char*)pSrc);

			pData = pDst;
			dwSize = SCUI_CIPHER_BUFSIZE;
		}
		else
		{
			pData = szBlank;
			dwSize = 1;
		}

		m_pParent->WriteJsonBinaryData(JSON_KEY_JC_ROOT_BASE,JSON_ENT_JC_PIN,pData, dwSize);

		//if(pTemp != NULL)
		//	delete[] pTemp;

	}

	// --- szFolderPass
	if(wField & JC_FOLDERPASS)
	{
		//WCHAR FAR		*pTemp = NULL;		
		//if((*pjc).wFolderIndex != 0 && ::lstrlen((*pjc).szFolderPass) != 0)
		//{
		//	if(Encode_AES_EX(NULL,(*pjc).szFolderPass,dwSize) == TRUE)
		//	{
		//		pTemp = new WCHAR[dwSize];
		//		if(pTemp == NULL)
		//			goto EXIT;
		//		SecureZeroMemory(pTemp, dwSize);
		//		pData = pTemp;

		//		if(Encode_AES_EX(pTemp,(*pjc).szFolderPass,dwSize) == TRUE)
		//		{
		//			m_pParent->WriteJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_FOLDERPASS_SIZE,dwSize);
		//		}
		//	}
		//}
		//else
		//{
		//	pData = szBlank;
		//	dwSize = 1;
		//}
		if ((*pjc).wFolderIndex != 0 && ::lstrlen((*pjc).szFolderPass) != 0)
		{
			SecureZeroMemory(pSrc, SCUI_CIPHER_BUFSIZE);
			SecureZeroMemory(pDst, SCUI_CIPHER_BUFSIZE);

			::memcpy(pSrc, (*pjc).szFolderPass, sizeof((*pjc).szFolderPass));
			Encode_AES((char*)pDst, (char*)pSrc);

			pData = pDst;
			dwSize = SCUI_CIPHER_BUFSIZE;
		}
		else
		{
			pData = szBlank;
			dwSize = 1;
		}

		m_pParent->WriteJsonBinaryData(JSON_KEY_JC_ROOT_BASE,JSON_ENT_JC_FOLDERPASS,pData, dwSize);
				
		//if(pTemp != NULL)
		//	delete[] pTemp;

	}

	blRet = TRUE;

EXIT:

	if(pDst != NULL)
		delete[] pDst;

	if(pSrc != NULL)
		delete[] pSrc;

	return blRet;
}

//=============================================================================
// function
//      Encode_AES_EX
//
// parameters
//		pszOut				the encoded out string
//		pszIn				the input src
//		dwSize				the output buffer size
//
// return value
//      成功 : TRUE
//      失敗 : FALSE
//
// outline
//		If the pszIn is longer than 16 bytes,use this function to encode
//=============================================================================
//BOOL CShJsonJC::Encode_AES_EX(WCHAR FAR *pszOut, WCHAR FAR *pszIn, DWORD& dwSize)
//{
//	dwSize = 0;
//	BOOL bRet = FALSE;
//	WCHAR FAR		*pSrc = NULL;
//	WCHAR FAR		*pDst = NULL;
//	WCHAR FAR		*pEncode = NULL;
//
//	if(pszIn == NULL)
//		goto EXIT;
//	
//	WCHAR* pTmpIn = pszIn;
//	int nLen = ::lstrlen(pTmpIn);
//	dwSize = nLen;
//	if(nLen % 16 > 0)
//		dwSize = (nLen/16 + 1) * 16;
//
//	if(pszOut == NULL)
//	{
//		if(dwSize > 0)
//			bRet = TRUE;
//	
//		goto EXIT;
//	}
//		
//	pSrc = new WCHAR[SCUI_CIPHER_BUFSIZE];
//	pDst = new WCHAR[SCUI_CIPHER_BUFSIZE];
//	if(pSrc == NULL || pDst == NULL)
//		goto EXIT;
//
//	pEncode = pszOut;
//
//	while(nLen > 0)
//	{
//		if(nLen >= 16)
//		{
//			SecureZeroMemory(pSrc, SCUI_CIPHER_BUFSIZE);
//			SecureZeroMemory(pDst, SCUI_CIPHER_BUFSIZE);
//			::memcpy(pSrc, pTmpIn, sizeof(WCHAR) * 16);
//			Encode_AES(pDst, pSrc);
//			::memcpy(pEncode, pDst, sizeof(WCHAR) * 16);
//			pTmpIn += 16;
//			nLen = ::lstrlen(pTmpIn);
//
//			pEncode += 16;
//		}
//		else
//		{
//			SecureZeroMemory(pSrc, SCUI_CIPHER_BUFSIZE);
//			SecureZeroMemory(pDst, SCUI_CIPHER_BUFSIZE);
//			::memcpy(pSrc, pTmpIn, sizeof(WCHAR) * nLen);
//			Encode_AES(pDst, pSrc);
//			::memcpy(pEncode, pDst, sizeof(WCHAR) * 16);
//
//			break;
//		}
//	}
//
//	bRet = TRUE;
//EXIT:
//	if(pDst != NULL)
//		delete[] pDst;
//
//	if(pSrc != NULL)
//		delete[] pSrc;
//
//	return bRet;
//}
//
////=============================================================================
//// function
////      Decode_AES_EX
////
//// parameters
////		pszOut				the decoded out string
////		pszIn				the input src
////		dwSize				the output buffer size
////
//// return value
////      成功 : TRUE
////      失敗 : FALSE
////
//// outline
////		If the pszIn is longer than 16 bytes,use this function to decode
////=============================================================================
//BOOL CShJsonJC::Decode_AES_EX(WCHAR FAR *pszOut, WCHAR FAR *pszIn, DWORD& dwSize)
//{
//	BOOL bRet = FALSE;
//	WCHAR FAR		*pBuff = NULL;
//	if(pszIn == NULL || pszOut == NULL)
//		goto EXIT;
//
//	pBuff = new WCHAR[SCUI_CIPHER_BUFSIZE];
//	if(pBuff == NULL)
//		goto EXIT;
//
//	WCHAR FAR		*pTmpIn = pszIn;
//
//	int nLastLen = 16;
//	WCHAR* pTmpOut = pszOut;
//
//	int nLeft = dwSize;
//	while(nLeft > 0)
//	{
//		if(nLeft >= 16)
//		{
//			SecureZeroMemory(pBuff, SCUI_CIPHER_BUFSIZE);
//			::memcpy(pBuff, pTmpIn, sizeof(WCHAR) * 16);
//			Decode_AES(pTmpOut, pBuff);
//
//			if(nLeft == 16)
//			{
//				nLastLen = ::lstrlen(pTmpOut);
//			}
//
//			pTmpIn += 16;
//			nLeft -= 16;
//
//			pTmpOut += 16;
//		}
//		else
//			break;
//	}
//
//	dwSize = dwSize - 16 + nLastLen;
//	bRet = TRUE;
//
//EXIT:
//	if(pBuff != NULL)
//		delete[] pBuff;
//
//	return bRet;
//}

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
void CShJsonJC::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_US);
}
// <E> 2022.05.17 sbc R.Tanaka
//=============================================================================
// function
//      GetEncodedPassWordData
//
// return value
//     NILL
//
// outline
//   Gets AES encrypted password data.
//=============================================================================
void CShJsonJC::GetEncodedPassWordData(CHAR* byEncodedPasswordData, WCHAR* pInputPasswordData, size_t sMaximumInputStringLength, size_t sOutputStringLength)
{
	size_t sInputStringLength = wcsnlen_s(pInputPasswordData, sMaximumInputStringLength);
	if (0 < sInputStringLength)
	{
		CHAR strMultiByte[MAX_ENCODING_STRING_LENGTH * 2 + 1] = { 0 };
		WideCharToMultiByte(CP_ACP, 0, pInputPasswordData, -1, strMultiByte, MAX_ENCODING_STRING_LENGTH * 2 + 1, NULL, NULL);
		Encode_AES(byEncodedPasswordData, strMultiByte);
		if (MAX_ENCODING_STRING_LENGTH < sInputStringLength)
		{
			Encode_AES(byEncodedPasswordData + MAX_ENCODING_STRING_LENGTH, strMultiByte + sizeof(char)*MAX_ENCODING_STRING_LENGTH);
		}
	}
	else
	{
		SecureZeroMemory(byEncodedPasswordData, sOutputStringLength);
	}

}
//=============================================================================
// function
//      GetDecodedPassWordData
//
// return value
//     NILL
//
// outline
//   Decodes AES encrypted password data.
//=============================================================================
void CShJsonJC::GetDecodedPassWordData(CHAR* byEncodedPasswordData, WCHAR* pOuputPasswordData, size_t sEncodedStringLength)
{
	size_t sInputStringLength = strnlen_s(byEncodedPasswordData, sEncodedStringLength);
	if (0 < sInputStringLength)
	{
		CHAR strMultiByte[MAX_ENCODING_STRING_LENGTH * 2 + 1] = { 0 };
		Decode_AES(strMultiByte, byEncodedPasswordData);
		if (MAX_ENCODING_STRING_LENGTH < sInputStringLength)
		{
			Decode_AES(strMultiByte + sizeof(char)*MAX_ENCODING_STRING_LENGTH, byEncodedPasswordData + MAX_ENCODING_STRING_LENGTH);
		}
		MultiByteToWideChar(CP_ACP, 0, strMultiByte, -1, pOuputPasswordData, MAX_ENCODING_STRING_LENGTH * 2);
	}
	else
	{
		SecureZeroMemory(pOuputPasswordData, MAX_ENCODING_STRING_LENGTH);
	}
}