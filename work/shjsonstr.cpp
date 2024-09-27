// ============================================================================
// shjsonstr.cpp
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.04.08
// ============================================================================

#include "precomp.h"
#include "shjson.h"
#include "aes.h"
#include "shjsonstr.h"
#include "jsonkey.h"



#define PARENT_CLASSNAME 	CShJson
#define PARENT_CLASSNAME2 	CShRegAC
#define MY_CLASSNAME 		CShJsonStored

//=============================================================================
// function
//      CShJsonStored
//
// parameters
//      hInst               ���W���[���n���h��
//      pLog                CShPrnLog �N���X�A�h���X
//
// return value
//      ����
//
// outline
//      CShJsonStored �N���X�R���X�g���N�^
//=============================================================================
CShJsonStored::CShJsonStored(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName, HMODULE hStringResourceHandle/*, CShPrnLog FAR *pLog*/)
	          : CShJson(hInst, pszSvrPrnName/*, pLog*/), CRegistryAPI()/*CShRegAC(hInst, pszSvrPrnName,NULL/*, pLog*///bear*/
{
	m_hStringResourceHandle = hStringResourceHandle;
	SetParent(this);
}


//=============================================================================
// function
//      ~CShJsonStored
//
// parameters
//      ����
//
// return value
//      ����
//
// outline
//      CShJsonStored �N���X�f�X�g���N�^
//=============================================================================
 CShJsonStored::~CShJsonStored()
{
}

//=============================================================================
// function
//      reset
//
// parameters
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      INI�t�@�C���̃f�[�^���g�p���ă��W�X�g��������������
//=============================================================================
BOOL CShJsonStored::reset(CShIniFile FAR *pIniFile)
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	WCHAR FAR		*pszName = NULL;
	DWORD			dwNameSize = 0;
	BOOL			blHasPin = FALSE;

	WCHAR FAR		*psz = NULL;

	CShIniFile FAR	*pini = NULL;

	DWORD			i;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	if(pIniFile != NULL)
		pini = pIniFile;
	else
	{
		TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
		GetProjectFileName(szCommonDatFilePath, L"wm62.DAT", ghInstance);
		pini = new CShIniFile(ghInstance, m_pszSvrPrnName, szCommonDatFilePath, FALSE);
		if (pini == NULL)
			goto EXIT;
	}

	// ================================================
	// _/_/_/  ���������擾�Ə�������
	// ================================================
	dwNameSize = (JC_STR_FOLDERPATH + 1) * 2;
	pszName = new WCHAR[dwNameSize];
	if(pszName == NULL)
		goto EXIT;

	dwCount = GetCountIni(pini);
	if(dwCount == 0)
		goto EXIT;

	//CShJson::LoadDll(scui_dll_str);

	for(i = 0; i < dwCount; i++)
	{
		SecureZeroMemory(pszName, dwNameSize);

		if(GetStrdDefData(i, pszName, dwNameSize, &blHasPin, pini) == FALSE)
			break;

		if(i == 0)
		//LoadStr(IDS_INDEX0_FOLDER, pszName, dwNameSize, TRUE);
			LoadString(m_hStringResourceHandle, IDS_INDEX0_FOLDER, pszName, dwNameSize);

		WriteStrdData(i, pszName, blHasPin);
	}

	blRet = WriteStrdCount( dwCount);

EXIT:

	if(pszName != NULL)
		delete[] pszName;

	if (pini != NULL)
	{
		delete pini;
		pini = NULL;
	}

	return blRet;
}


//=============================================================================
// function
//      WriteStrdData
//
// parameters
//      lIndex              �������݃C���f�b�N�X
//      pszTitle            �^�C�g��������
//      pszPath             �I�[�o�[���C�p�X
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      ���W�X�g���ɕۑ���t�H���_�f�[�^����������
//=============================================================================
BOOL CShJsonStored::WriteStrdData(long lIndex, WCHAR FAR *pszName, BOOL blHasPin)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszName == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;
    SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_STRD_INDEX_BASE, lIndex);

	// --- folder_name
	m_pParent->WriteJsonStrData(pszKey, JSON_ENT_STRD_FOLDERNAME, pszName, (DWORD)(::wcslen((const wchar_t *)pszName)* sizeof(wchar_t)));

	// --- has_folder_pin
	blRet = m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_STRD_HASPIN,(DWORD)blHasPin);

EXIT:
	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      WriteStrdCount
//
// parameters
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �ۑ���t�H���_������������
//=============================================================================
BOOL CShJsonStored::WriteStrdCount(DWORD dwCount)
{
	BOOL			blRet = FALSE;
		
	if(m_pParent == NULL)
		goto EXIT;

	// --- count
	blRet = m_pParent->WriteJsonDWORDData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_COUNT,dwCount);

EXIT:
	return blRet;
}


//=============================================================================
// function
//      WriteStrdPin
//
// parameters
//      pszPin              �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �ۑ���t�H���_�̃p�X�R�[�h����������
//=============================================================================
BOOL CShJsonStored::WriteStrdPin(WCHAR FAR *pszPin)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszTemp = NULL;

	DWORD			dwSize = 0;

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszPin == NULL)
		goto EXIT;

	pszTemp = new WCHAR[SCUI_CIPHER_BUFSIZE];
	if(pszTemp == NULL)
		goto EXIT;

	 SecureZeroMemory(pszTemp, SCUI_CIPHER_BUFSIZE);

// Login Password����ŕۑ�����ƕ��A���ɃS�~�����錏�̏C��
	if(::lstrlen(pszPin) != 0)
	{
		dwSize = SCUI_CIPHER_BUFSIZE;

		Encode_AES((char*) pszTemp, (char*) pszPin);

// Login Password����ŕۑ�����ƕ��A���ɃS�~�����錏�̏C��
	}
	else
	{
		dwSize = 1;
	}

	// --- folder_pass_code
	blRet = m_pParent->WriteJsonBinaryData(JSON_KEY_JC_ROOT_BASE,JSON_ENT_JC_FOLDERPASS,pszTemp, dwSize);

EXIT:

	if(pszTemp != NULL)
		delete[] pszTemp;

	return blRet;
}


// Excel��A���񂪕ۑ������V�[�g��B�����PC�����������
// A���񂪎w�肵���t�H���_�ɕۑ�����錏�̑Ή�
//=============================================================================
// function
//      WriteLastSelectedFolder
//
// parameters
//      pszPin              �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �ۑ���t�H���_�̃p�X�R�[�h����������
//=============================================================================
BOOL CShJsonStored::WriteLastSelectedFolder(WCHAR FAR *pszFolderName, WCHAR FAR *pszFolderPIN)
{
	BOOL			blRet = FALSE;

	DWORD			dwSize = 0;
	DWORD			dwData = 0;
	WCHAR			szBlank[8];
	WCHAR FAR		*pData = NULL;
	WCHAR FAR		*pSrc = NULL;
	WCHAR FAR		*pDst = NULL;

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszFolderName == NULL || pszFolderPIN == NULL)
		goto EXIT;

	// --- folder name
	// <S><A> 2024.03.06,To fix Redmine #7064 SSDI:Manoj S
	//m_pParent->WriteJsonStrData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_LASTSEL,pszFolderName , (DWORD)(::lstrlen(pszFolderName) + 1),TRUE);
	m_pParent->WriteJsonStrData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_LASTSEL,pszFolderName , (DWORD)(wcslen(pszFolderName) + 1)*2,FALSE);
	// <E> 2024.03.06,To fix Redmine #7064 SSDI:Manoj S
	// --- folder_pass_code
	pSrc = new WCHAR[SCUI_CIPHER_BUFSIZE];
	pDst = new WCHAR[SCUI_CIPHER_BUFSIZE];
	if(pSrc == NULL || pDst == NULL)
		goto EXIT;

	    SecureZeroMemory(szBlank, sizeof(szBlank));

	if(::lstrlen(pszFolderPIN) != 0)
	{
		SecureZeroMemory(pSrc, SCUI_CIPHER_BUFSIZE);
		SecureZeroMemory(pDst, SCUI_CIPHER_BUFSIZE);

		::memcpy(pSrc, pszFolderPIN, ::lstrlen(pszFolderPIN));
		Encode_AES((char*) pDst, (char*) pSrc);

		pData = pDst;
		dwSize = SCUI_CIPHER_BUFSIZE;

	}
	else
	{
		pData = szBlank;
		dwSize = 1;
	}

	blRet = m_pParent->WriteJsonBinaryData(JSON_KEY_STRD_ROOT_BASE,JSON_ENT_STRD_LASTSELPIN,(WCHAR FAR *)pData,dwSize);

EXIT:

	if(pDst != NULL)
		delete[] pDst;

	if(pSrc != NULL)
		delete[] pSrc;

	return blRet;
}


//=============================================================================
// function
//      ReadLastSelectedFolder
//
// parameters
//      pszPin              �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �ۑ���t�H���_�̃p�X�R�[�h����������
//=============================================================================
long CShJsonStored::ReadLastSelectedFolder(WCHAR FAR *pszFolderName, DWORD dwLength, WCHAR FAR *pszFolderPIN, DWORD dwLengthPIN)
{
	long			lRet = -1;

	long			lResult = 0;

	DWORD			dwSize = 0;
	DWORD			dwData = 0;

	WCHAR			szName[512];
	BOOL			blHasPIN;

	DWORD			i;

	WCHAR FAR		*pBuff = NULL;

	/*CShJson::LogStartFunc("CShJsonStored::ReadLastSelectedFolder");*/

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszFolderName == NULL || pszFolderPIN == NULL)
		goto EXIT;

	// --- folder name
	dwSize = dwLength;
	m_pParent->ReadJsonStrData(JSON_KEY_STRD_ROOT_BASE,JSON_ENT_STRD_LASTSEL,pszFolderName, dwSize);

	// --- szFolderPass
	pBuff = new WCHAR[SCUI_CIPHER_BUFSIZE];
	if(pBuff == NULL)
		goto EXIT;

	dwSize = SCUI_CIPHER_BUFSIZE;
    SecureZeroMemory(pBuff, dwSize);
	SecureZeroMemory(pszFolderPIN, dwLengthPIN);

	if(m_pParent->ReadJsonBinaryData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_LASTSELPIN, pBuff, dwSize) != FALSE)
	{
		if(dwSize == SCUI_CIPHER_BUFSIZE)
			Decode_AES((CHAR*) pszFolderPIN, (CHAR*) pBuff);
	}

	// --- �C���f�b�N�X����
	for(i = 0; i < ReadStrdCount(); i++)
	{
		//PARENT_CLASSNAME::LogString("szName", szName);
		//PARENT_CLASSNAME::LogStringW("szName", szName);
		::ZeroMemory(szName, sizeof(szName));
		ReadStrdData(i, szName, sizeof(szName), &blHasPIN);
		if(IsUnicodeOS())
			lResult = ::CompareStringW(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE | NORM_IGNOREWIDTH, (wchar_t *)szName, -1, (wchar_t *)pszFolderName, -1);
		else
			lResult = ::CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE | NORM_IGNOREWIDTH, szName, -1, pszFolderName, -1);

		if(lResult == CSTR_EQUAL)
		{
			lRet = (long)i;
			break;
		}
	}

EXIT:

	if(pBuff != NULL)
		delete[] pBuff;

	//PARENT_CLASSNAME::LogLong("return value", (long)lRet);
	/*CShJson::LogEndFunc("CShJsonStored::ReadLastSelectedFolder");*/

	return lRet;
}

//=============================================================================
// function
//      ReadStrdData
//
// parameters
//      lIndex              �ǂݎ��C���f�b�N�X
//      pszTitle            �^�C�g����������󂯎��o�b�t�@
//      pszPath             �I�[�o�[���C�p�X���󂯎��o�b�t�@
//      dwTitleSize         pszTitle�T�C�Y
//      dwPathSize          pszPath�T�C�Y
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X�̕ۑ���t�H���_�����Ń��W�X�g������ǂݍ���
//=============================================================================
BOOL CShJsonStored::ReadStrdData(long lIndex, WCHAR FAR *pszName, DWORD dwNameSize, BOOL *pblHasPin)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	DWORD			dwData = 0;
	DWORD			dwSize = 0;

	if(m_pParent == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_STRD_INDEX_BASE, lIndex);

	// --- folder_name
	if(pszName != NULL)
	{
		dwSize = dwNameSize;
		m_pParent->ReadJsonStrData(pszKey,JSON_ENT_STRD_FOLDERNAME,pszName, dwSize);
	}

	// --- has_folder_pin
	if(pblHasPin != NULL)
	{
		m_pParent->ReadJsonDWORDData(pszKey,JSON_ENT_STRD_HASPIN,dwData);
		*pblHasPin = (BOOL)dwData;
	}

	blRet = TRUE;

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}


//=============================================================================
// function
//      ReadStrdCount
//
// parameters
//
// return value
//      ���� : �o�^Folder��
//      ���s : 0
//
// outline
//      �ۑ���t�H���_����ǂݍ���
//=============================================================================
DWORD CShJsonStored::ReadStrdCount()
{
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	// --- count
	blSuccess = m_pParent->ReadJsonDWORDData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_COUNT, dwData);
	if(dwData == 0 || blSuccess == FALSE)
	{
		reset();
		blSuccess = m_pParent->ReadJsonDWORDData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_COUNT, dwData);
	}

	return dwData;
}


//=============================================================================
// function
//      ReadStrdPin
//
// parameters
//      pszPin              �p�X�R�[�h���󂯎��o�b�t�@
//      dwPinSize           pszPin�T�C�Y
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      �ۑ���t�H���_�̃p�X�R�[�h��ǂݍ���
//=============================================================================
BOOL CShJsonStored::ReadStrdPin(WCHAR FAR *pszPin, DWORD dwPinSize)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszTemp = NULL;

	DWORD			dwSize = 0;
	DWORD			dwVal = 0;

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszPin == NULL)
		goto EXIT;

	pszTemp = new WCHAR[SCUI_CIPHER_BUFSIZE];
	if(pszTemp == NULL)
		goto EXIT;

	// --- folder_pin
	dwSize = SCUI_CIPHER_BUFSIZE;
	SecureZeroMemory(pszTemp, dwSize);
	// <S><C> 2024.01.11,To Fix issue #6884 SSDI:Manoj S
	//if(m_pParent->ReadJsonBinaryData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_FOLDERPASS,pszTemp, dwSize) == FALSE)
	//	goto EXIT;
	if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_FOLDERPASS_SIZE, dwVal) != FALSE)
	{
		dwSize = (short)dwVal;
		m_pParent->ReadJsonBinaryData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_FOLDERPASS,pszTemp, dwSize);
	}
// Login Password����ŕۑ�����ƕ��A���ɃS�~�����錏�̏C��
	//if(dwSize == SCUI_CIPHER_BUFSIZE)
	// <E> 2024.01.11,To Fix issue #6884 SSDI:Manoj S
		Decode_AES((CHAR * )pszPin,(CHAR *) pszTemp);

	blRet = TRUE;

EXIT:
	if(pszTemp != NULL)
		delete[] pszTemp;

	return blRet;
}


//=============================================================================
// function
//      ReadRetPin
//
// parameters
//      pszPin              �p�X�R�[�h���󂯎��o�b�t�@
//      dwPinSize           pszPin�T�C�Y
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      �ۑ���t�H���_�̃p�X�R�[�h��ǂݍ���
//=============================================================================
BOOL CShJsonStored::ReadRetPin(WCHAR FAR *pszPin, DWORD dwPinSize)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszTemp = NULL;

	long			lVal = 0;
	DWORD			dwVal = 0;
	DWORD			dwSize = 0;

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszPin == NULL)
		goto EXIT;

	// --- retention_pin
	if(m_pParent->ReadJsonDWORDData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_ALWAYSPIN, dwVal) != FALSE)
	{
		lVal = (long)dwVal;
		if(lVal != 0)
		{
			pszTemp = new WCHAR[SCUI_CIPHER_BUFSIZE];
			if(pszTemp == NULL)
				goto EXIT;

			dwSize = SCUI_CIPHER_BUFSIZE;
			SecureZeroMemory(pszTemp, dwSize);
			if(m_pParent->ReadJsonBinaryData(JSON_KEY_JC_ROOT_BASE, JSON_ENT_JC_PIN,pszTemp, dwSize) != FALSE)
// Login Password����ŕۑ�����ƕ��A���ɃS�~�����錏�̏C��
			{
				if(dwSize == SCUI_CIPHER_BUFSIZE)
				{	
					Decode_AES((CHAR *)pszPin,(CHAR *) pszTemp);

// Login Password����ŕۑ�����ƕ��A���ɃS�~�����錏�̏C��
				}
			}
		}
	}

	blRet = TRUE;

EXIT:

	if(pszTemp != NULL)
		delete[] pszTemp;

	return blRet;
}


//=============================================================================
// function
//      GetCountIni
//
// parameters
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : INI�t�@�C������擾������
//      ���s : 0
//
// outline
//      INI�t�@�C���̃f�t�H���g�ۑ���t�H���_�����擾����
//=============================================================================
DWORD CShJsonStored::GetCountIni(CShIniFile FAR *pIniFile)
{
	DWORD			dwRet = 0;

	CShIniFile FAR	*pini = NULL;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	if(pIniFile != NULL)
		pini = pIniFile;
	else
	{
		WCHAR szWaterMarkDatFilePath[_MAX_PATH] = {};
		GetProjectFileName(szWaterMarkDatFilePath, L"wm62.dat", ghInstance);

		pini = new CShIniFile(ghInstance, m_pszSvrPrnName, szWaterMarkDatFilePath, FALSE);
		if (NULL == pini)
		{
			return 0;
		}
	}

	// ================================================
	// _/_/_/  ���̎擾
	// ================================================
	dwRet = (*pini).GetLong(LANGD_SEC_STRDDEFAULT, LANGD_KEY_STRDDEF_COUNT);

EXIT:

	return dwRet;
}


//=============================================================================
// function
//      GetDFDefData
//
// parameters
//      lIndex              �����擾����C���f�b�N�X
//      pdfd                �����󂯎��WATERMARKDATA�\���̃A�h���X
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽINI�t�@�C���̃f�t�H���gDocumentFiling���擾����
//=============================================================================
BOOL CShJsonStored::GetStrdDefData(long lIndex, WCHAR FAR *pszName, DWORD dwNameSize, BOOL *pblHas, CShIniFile FAR *pIniFile)
{
	BOOL			blRet = FALSE;

	short			wRange = 1;

	WCHAR			szData[384];

	WCHAR FAR		*pc = NULL;

	long FAR		*pl = NULL;

	CShIniFile FAR	*pini = NULL;

	short			i;
	int				idt;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	if(pIniFile != NULL)
		pini = pIniFile;
	else
	{
		TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
		GetProjectFileName(szCommonDatFilePath, L"wm62.DAT", ghInstance);
		pini = new CShIniFile(ghInstance, m_pszSvrPrnName, szCommonDatFilePath, FALSE);
		if (pini == NULL)
			goto EXIT;
	}

	// ================================================
	// _/_/_/  ���̎擾
	// ================================================
     SecureZeroMemory(szData, sizeof(szData));
	//if((*pini).GetString(LANGD_SEC_STRDDEFAULT, lIndex, "", szData, _countof(szData), CShJson::IsUnicode()) == 0)
	//if ((*pini).GetString(LANGD_SEC_STRDDEFAULT, lIndex, L"", szData, _countof(szData)/*, IsUnicodeOS()*/) == 0)
	if ((*pini).GetString(LANGD_SEC_STRDDEFAULT, lIndex, (WCHAR*) "", (WCHAR*)szData, _countof(szData) / 2) == 0)
// WM.dat�̓��e��Localize�Ώۂ̃L�[��DLL�Ɉڍs����Ή�
#ifdef WMCONFIGFILE_TO_RES_DLL
	{
		CShJson::SecureZeroMemory(szData, sizeof(szData));
		if (CShJson::LoadStr(IDS_STOREDTO_RES_BASE + lIndex, szData, _countof(szData), CShJson::IsUnicode()) == 0)
			goto EXIT;
	}
#else
		goto EXIT;
#endif
	//if(IsUnicodeOS() != FALSE)
	//	wRange = 2;
	//else
	//	wRange = 1;

	// ================================================
	// _/_/_/  ���̍쐬
	// ================================================
	// folder_name
	pc = wcsrchr(szData, ',');
	if(pc == NULL)
		goto EXIT;

	for(i = 0; i < wRange; i++)
	{
		*pc = '\0';
		pc++;
	}

	//::wcsncpy(pszName, dwNameSize, pc, (int)(dwNameSize / wRange) - 1);
	::wcsncpy_s(pszName, dwNameSize, pc, (int)(dwNameSize / wRange) - 1);

	// has_folder_pin
	idt = _wtoi(szData);
	// idt = sh_atoi(szData); 
	*pblHas = (idt == 0) ? FALSE : TRUE;
	blRet = TRUE;

EXIT:

	if (pIniFile == NULL)
	{
		if (pini != NULL)
		{
			delete pini;
			pini = NULL;
		}
	}
	return blRet;
}


//=============================================================================
// function
//      CreateData
//
// parameters
//      wPortNum           �{�[�g�ԍ�
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      �R���t�B�O���[�V�������ʂ���f�[�^�𐶐�����
//=============================================================================
BOOL CShJsonStored::CreateData(short wPort)
{
//	BOOL		blRet = FALSE;
//
//	BOOL		blPin = FALSE;
//
//	HKEY		hKey = 0;
//
//	unsigned long		lCount = 0;
//	unsigned long		i = 0;
//
//	WCHAR FAR	*pszBase = NULL;
//	WCHAR FAR	*pszData = NULL;
//	WCHAR FAR	*pszBaseIndex = NULL;
//
//	pszBase = new WCHAR[SCUI_REGKEYSIZE];
//	pszData = new WCHAR[256];
//	pszBaseIndex = new WCHAR[SCUI_REGKEYSIZE];
//	if(pszBase == NULL || pszData == NULL || pszBaseIndex == NULL)
//		goto EXIT;
//
//	SecureZeroMemory(pszBase, SCUI_REGKEYSIZE);
//	SecureZeroMemory(pszData, 256);
//	SecureZeroMemory(pszBaseIndex, SCUI_REGKEYSIZE);
//
//	hKey = GetRootKey();
//
//	// ================================================
//	// _/_/_/  �ۑ����̏�����
//	// ================================================
////	if(CreateRegPath(REG_KEY_STRD_ROOT_BASE, pszBase, SCUI_REGKEYSIZE, GetPDName()) == FALSE)
////		goto EXIT;
//
//// <S><M> 2021.06.08 Jelly, for stored to dialog, select the last selected folder.
////	If delete the pszBase, the "last_selected" will be deleted. So modify it, only delete the base index
////	DeleteRegTreeData(hKey, pszBase);
//	for(i = 0; i < ReadStrdCount(); i++)
//	{
//		SecureZeroMemory(pszBaseIndex, SCUI_REGKEYSIZE);
//		::wsprintf(pszBaseIndex, JSON_KEY_STRD_INDEX_BASE, i);
//		m_pParent->DeleteJsonTreeData(pszBaseIndex);
//	}
//// <E> 2021.06.08 Jelly
//
//	// ================================================
//	// _/_/_/  ���擾
//	// ================================================
//	if(GetBaseKeyName(hKey, ackey_df_res, wPort, pszBase, SCUI_REGKEYSIZE, TRUE) == FALSE)
//		goto EXIT;
//
//	lCount = GetRegCount(hKey, pszBase);
//	lCount += 1;	// "Main Folder"��Config�ŏ������܂�Ȃ���
//
//	for(i = 0; i < lCount; i++)
//	{
//		SecureZeroMemory(pszData, 256);
//
//		blPin = FALSE;
//
//		//if(i == 0)//bear
//		//	LoadStr(IDS_INDEX0_FOLDER, pszData, 256, TRUE);
//		//else
//		//	GetConfigData(hKey, wPort, i, pszData, 256, &blPin);
//
//		// ================================================
//		// _/_/_/  ���̕ۑ�
//		// ================================================
//		if(WriteStrdData(i, pszData, blPin) == FALSE)
//			goto EXIT;
//	}
//
//	if(WriteStrdCount(lCount) == FALSE)
//		goto EXIT;
//
//	blRet = TRUE;
//
//EXIT:
//
//	if(pszData != NULL)
//		delete[] pszData;
//
//	if(pszBase != NULL)
//		delete[] pszBase;
//
//	if(pszBaseIndex != NULL)
//		delete[] pszBaseIndex;
//
//	return blRet;
	return 0;//bear
}


//=============================================================================
// function
//      GetConfigData
//
// parameters
//      hKey	           ���[�g�L�[
//      wPortNum           �{�[�g�ԍ�
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      �R���t�B�O���[�V�����ɂ�菑�����܂ꂽ�����擾����
//=============================================================================
BOOL CShJsonStored::GetConfigData(HKEY hKey, short wPort, long lIndex, WCHAR FAR *pszName, DWORD dwNameSize, BOOL *pblHasPin)
{
	//BOOL		blRet = FALSE;

	//DWORD		dwNeed = 0;

	//WCHAR FAR	*pszBase = NULL;
	//WCHAR FAR	*pszTemp = NULL;
	//WCHAR FAR	*pszKey = NULL;
	//WCHAR FAR	*pszData = NULL;

	///*if(CShJson::CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;*/

	///*CShJson		goto EXIT;*/

	////if((*(CShJson::m_pmcf)).IsACDir() == FALSE)
	////	goto EXIT;

	//pszBase = new WCHAR[SCUI_REGKEYSIZE];
	//pszTemp = new WCHAR[SCUI_REGKEYSIZE];
	//pszKey = new WCHAR[SCUI_REGKEYSIZE];
	//pszData = new WCHAR[256];
	//if(pszBase == NULL || pszTemp == NULL  || pszKey == NULL || pszData == NULL)
	//	goto EXIT;
 //   SecureZeroMemory(pszBase, SCUI_REGKEYSIZE);
	//SecureZeroMemory(pszTemp, SCUI_REGKEYSIZE);
	//SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	//SecureZeroMemory(pszData, 256);

	//// ================================================
	//// _/_/_/  ���W�X�g���L�[�����쐬
	//// ================================================
	//if(GetBaseKeyName(hKey, ackey_df_res, wPort, pszBase, SCUI_REGKEYSIZE) == FALSE)
	//	goto EXIT;

	//::lstrcat(pszBase, REG_KEY_SOAP_DF_FILE);

	//// ================================================
	//// _/_/_/  ���W�X�g������ǂݍ���
	//// ================================================
	//// name
	//::lstrcpy(pszTemp, pszBase);
	//::lstrcat(pszTemp, REG_KEY_SOAP_DF_NAME);
	//if(CreateRegPath(pszTemp, pszKey, SCUI_REGKEYSIZE, lIndex) == FALSE)
	//	goto EXIT;

	//dwNeed = 256;
	//if(ReadRegData(hKey, pszKey, NULL, REG_SZ, (WCHAR FAR *)pszData, &dwNeed) == FALSE)
	//	goto EXIT;

	//wcscpy_s(pszName, dwNameSize, pszData);

	//// password
	//::lstrcpy(pszTemp, pszBase);
	//::lstrcat(pszTemp, REG_KEY_SOAP_DF_PASSWORD);
	//if(CreateRegPath(pszTemp, pszKey, SCUI_REGKEYSIZE, lIndex) == FALSE)
	//	goto EXIT;

	//dwNeed = 256;
	//if(ReadRegData(hKey, pszKey, NULL, REG_SZ, (WCHAR FAR *)pszData, &dwNeed, TRUE) == FALSE)
	//	goto EXIT;

	//if(::lstrcmp(pszData, L"true") == 0)
	//	*pblHasPin = TRUE;
	//else
	//	*pblHasPin = FALSE;

	//blRet = TRUE;

	//EXIT:

	//if(pszData != NULL)
	//	delete[] pszData;

	//if(pszKey != NULL)
	//	delete[] pszKey;

	//if(pszTemp != NULL)
	//	delete[] pszTemp;

	//if(pszBase != NULL)
	//	delete[] pszBase;

	//return blRet;
	return 0;//bear
}

//=============================================================================
// function
//      WriteLastSelectedFolderIndex
//
// parameters
//      dwIndex             folder index
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      Save the selected folder index
//=============================================================================
BOOL CShJsonStored::WriteLastSelectedFolderIndex(DWORD dwIndex)
{
	BOOL			blRet = FALSE;

	if(m_pParent == NULL)
		goto EXIT;

	// --- folder name index
	blRet = m_pParent->WriteJsonDWORDData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_LASTSELINDEX,dwIndex);

EXIT:

	return blRet;
}

//=============================================================================
// function
//      ReadLastSelectedFolderIndex
//
// parameters
//
// return value
//      ���� : selected folder index
//      ���s : 0
//
// outline
//      Read the selected folder index
//=============================================================================
DWORD CShJsonStored::ReadLastSelectedFolderIndex()
{
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	if(m_pParent != NULL)
		blSuccess = m_pParent->ReadJsonDWORDData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_LASTSELINDEX,dwData);

	return dwData;
}

//=============================================================================
// function
//      WriteSTDDataToUs
//
// parameters
//      lIndexUs              �������ރC���f�b�N�X
//      
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      write stored to folder name and folder index to user setting 
//=============================================================================
BOOL CShJsonStored::WriteSTDDataToUs(long lIndexUs, WCHAR FAR *pszFolderName, DWORD dwFolderSize,DWORD dwFoldIndex)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszFolderName == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  ��������
	// ================================================
	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_STRD_ROOT_BASE_US, lIndexUs);

	// --- folder name
	blRet = m_pParent->WriteJsonStrData(pszKey, JSON_ENT_STRD_FOLDERNAME,pszFolderName, dwFolderSize,TRUE);
	
	// --- folder name index
	blRet = m_pParent->WriteJsonDWORDData(pszKey, JSON_ENT_STRD_LASTSELINDEX,dwFoldIndex);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      ReadSTDDataToUs
//
// parameters
//      lIndex              �ǂݎ��C���f�b�N�X
//      pwmd                �ǂݎ��MARGINSHIFTDATA�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      read stored to folder name and folder index from user setting
//=============================================================================
BOOL CShJsonStored::ReadSTDDataToUs(long lIndexUs, WCHAR FAR *pszFolderName, DWORD& dwFolderSize,DWORD& dwFoldIndex)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszFolderName == NULL)
		goto EXIT;

	pszKey = new WCHAR[SCUI_REGKEYSIZE];
	if(pszKey == NULL)
		goto EXIT;

	SecureZeroMemory(pszKey, SCUI_REGKEYSIZE);
	::wsprintf(pszKey, JSON_KEY_STRD_ROOT_BASE_US, lIndexUs);

	// --- folder_name
	blRet = m_pParent->ReadJsonStrData(pszKey,JSON_ENT_STRD_FOLDERNAME,pszFolderName,dwFolderSize,TRUE);

	blRet = m_pParent->ReadJsonDWORDData(pszKey, JSON_ENT_STRD_LASTSELINDEX,dwFoldIndex);

EXIT:

	if(pszKey != NULL)
		delete[] pszKey;

	return blRet;
}

//=============================================================================
// function
//      WriteSTDData
//
// parameters
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      write stored to folder name 
//=============================================================================
BOOL CShJsonStored::WriteSTDData(WCHAR FAR *pszFolderName, DWORD dwFolderSize)
{
	BOOL			blRet = FALSE;
	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszFolderName == NULL)
		goto EXIT;

	// --- folder name
	blRet = m_pParent->WriteJsonStrData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_LASTSEL,pszFolderName, dwFolderSize,TRUE);

EXIT:

	return blRet;
}

//=============================================================================
// function
//      ReadSTDData
//
// parameters
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      read stored to folder name 
//=============================================================================
BOOL CShJsonStored::ReadSTDData(WCHAR FAR *pszFolderName, DWORD& dwFolderSize)
{
	BOOL			blRet = FALSE;
	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(m_pParent == NULL || pszFolderName == NULL)
		goto EXIT;

	// --- folder name
	blRet = m_pParent->ReadJsonStrData(JSON_KEY_STRD_ROOT_BASE, JSON_ENT_STRD_LASTSEL,pszFolderName, dwFolderSize,TRUE);

EXIT:

	return blRet;
}

//=============================================================================
// function
//      SetObjects
//
// parameters
//      psho                SHOHOBJ�\���̃A�h���X
//
// return value
//      ����
//
// outline
//      �I�u�W�F�N�g�̐ݒ���s��
//=============================================================================
//void CShJsonStored::SetObjects(PSHOHOBJ psho)
//{
//	//CShJson::SetObjects(psho);
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
//      ����
//
// outline
//     add functions name to json file path
//=============================================================================
void CShJsonStored::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_US);
}
// <E> 2022.05.17 sbc R.Tanaka
