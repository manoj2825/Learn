// ============================================================================
// inifile.cpp
//
//     Copyright 2004 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2004.06.02
// ============================================================================


#include "precomp.h"
#include "sh_api.h"
//For Logging Purpose
#include "MFPLogger.h"

//=============================================================================
// function
//      CShIniFile
//
// parameters
//      hInst               ���W���[���n���h��
//      pLog                CShPrnLog �N���X�A�h���X
//      pIniFilePath        ini�t�@�C���p�X
//      lMaxData            long�z��̍ő吔
//
// return value
//      ����
//
// outline
//      CShIniFile �N���X�R���X�g���N�^
//=============================================================================
CShIniFile::CShIniFile(HINSTANCE hInst, WCHAR* pPrinterName, WCHAR *pIniFilePath, BOOL blDirect, long lMaxData)
				
{
	UNREFERENCED_PARAMETER(hInst);
	::SecureZeroMemory(m_pszDrvName, sizeof(m_pszDrvName));
	wcscpy_s(m_pszDrvName, wcslen(pPrinterName)+1, pPrinterName);
	m_blDirect		= blDirect;

	m_pszDefFName	= NULL;
	m_pszDefExt		= NULL;
	m_pszIniFPath	= NULL;

	m_pdata		= NULL;
	m_psecdata	= NULL;
	m_pszSection = NULL;

	m_lCount	= lMaxData;
	m_plData	= NULL;
	m_byWriteToJson = -1;
// <S><A> #437 in Redmine,2012.07.30, SSDI:Akshay
// As per discussion with SDC in Redmine ticket #437, below source code is merged from CR3 production driver.
// <S><A> 2011.08.26 hashida
	m_szDateTime = NULL;
// <E> 2011.08.26 hashida
// <E> #437 in RedMine, 2012.07.30, SSDI:Akshay
	LCID  dwID = 0;
	short wSectionNo = 0;

	if(pIniFilePath != NULL)
		Initialize(pIniFilePath);

	{
		dwID = GetLCID(GetLong(MCF_SEC_DEF_CHANGE_LCID, MCF_KEY_SELECT_LCID_TYPE, 0));
		wSectionNo = (short)GetLong(MCF_SEC_DEF_CHANGE_LCID, (long)dwID, 0);
		if (wSectionNo > 0)
		{
			if (m_pszSection == NULL)
				m_pszSection = new WCHAR[_MAX_PATH];
			SecureZeroMemory(m_pszSection, _MAX_PATH);
			wsprintf(m_pszSection, L"%s%d", MCF_SEC_DEF_LCID_SECTION, wSectionNo);
		}
	}
}


//=============================================================================
// function
//      ~CShIniFile
//
// parameters
//      ����
//
// return value
//      ����
//
// outline
//      CShIniFile �N���X�f�X�g���N�^
//=============================================================================
CShIniFile::~CShIniFile()
{
	DeleteSectionData();

	if(m_pszIniFPath != NULL)
		delete[] m_pszIniFPath;
	
	if (m_pszSection != NULL)
		delete[] m_pszSection;

	if(m_pdata != NULL)
	{
		delete[] m_pdata;
		m_pdata = NULL;
	}

	if(m_plData != NULL)
		delete[] m_plData;

// <S><A> #437 in Redmine,2012.07.30, SSDI:Akshay
// As per discussion with SDC in Redmine ticket #437, below source code is merged from CR3 production driver.
// <S><A> 2011.08.26 hashida
	/*if (m_szDateTime != NULL)
	{
		delete[] m_szDateTime;
		m_szDateTime = NULL;
	}*/
// <E> 2011.08.26 hashida
// <E> #437 in RedMine, 2012.07.30, SSDI:Akshay
}


//=============================================================================
// function
//      Initialize
//
// parameters
//      pIniFilePath        ini�t�@�C���p�X
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      ShIniFile ������������
//=============================================================================
BOOL CShIniFile::Initialize(WCHAR *pIniFilePath)
{
	BOOL			blRet = FALSE;
	
	DWORD			dwSize = 0;

	WCHAR			szFName[_MAX_PATH]={0};
	MFPPrinterUI_Logger slog(__FUNCTION__"\n"); 
	if(pIniFilePath != NULL)
		StringCbCopy(szFName, sizeof(szFName),pIniFilePath);
	else
		GetProjectFileName(szFName,L"wm62.dat");
	
	if(m_pszIniFPath != NULL)
	{
		delete[] m_pszIniFPath;
		m_pszIniFPath = NULL;
	}
	else
	{
		slog.putLog("CShIniFile::Initialize -> NULL_OBJECT(m_pszIniFPath)\n");
	}
	
	m_pszIniFPath = new WCHAR[::wcslen(szFName) + 1];
	if(m_pszIniFPath != NULL)
	{
		SecureZeroMemory(m_pszIniFPath, (::wcslen(szFName) + 1)*sizeof(WCHAR));
		StringCbCopy(m_pszIniFPath, (wcslen(szFName) + 1)*sizeof(WCHAR),szFName);
	}
	else
	{
		slog.putLog("CShIniFile::Initialize -> NULL_HANDLE(m_pszIniFPath)\n");
	}

	//LogComment(szFName);

	if(m_pdata != NULL)
	{
		delete[] m_pdata;
		m_pdata = NULL;
//<S><A> RedMine Ticket(#604) Individualized Special UD2 Driver, 2013.05.28, SSDI:Padma
// Stress test is failing due to memory leak
		DeleteSectionData();
//<E> RedMine Ticket(#604) Individualized Special UD2 Driver, 2013.05.28, SSDI:Padma
	}
	else
	{
		slog.putLog("CShIniFile::Initialize -> NULL_HANDLE(m_pdata)\n");
	}

// <S><A> #437 in Redmine,2012.07.30, SSDI:Akshay
// As per discussion with SDC in Redmine ticket #437, below source code is merged from CR3 production driver.
// <S><A> 2011.08.26 hashida
	/*if (m_szDateTime != NULL)
	{
		delete[] m_szDateTime;
		m_szDateTime = NULL;
	}*/
// <E> 2011.08.26 hashida
// <E> #437 in RedMine, 2012.07.30, SSDI:Akshay
	if(m_blDirect == FALSE)
	{

		DWORD		dwRead;

		HANDLE		hFile = INVALID_HANDLE_VALUE;
// <S><A> #437 in Redmine,2012.07.30, SSDI:Akshay
// As per discussion with SDC in Redmine ticket #437, below source code is merged from CR3 production driver.
// <S><A> 2011.08.26 hashida
		/*FILETIME	fileTime, localFileTime;
		SYSTEMTIME	systemTime;*/
		
// <E> 2011.08.26 hashida
// <E> #437 in RedMine, 2012.07.30, SSDI:Akshay
		hFile = ::CreateFile(szFName,
							 GENERIC_READ,
							 FILE_SHARE_READ,
							 NULL,
							 OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL,
							 NULL
							 );

		if(hFile != INVALID_HANDLE_VALUE)
		{
			//dwSize = ::SetFilePointer(hFile, 0, NULL, FILE_END);
			LARGE_INTEGER largeintegerbytes = {0};
			if(0 == GetFileSizeEx(hFile,&largeintegerbytes))
			{
				::CloseHandle(hFile);
				return FALSE ;
			}
			dwSize = (DWORD)largeintegerbytes.LowPart/sizeof(WCHAR);
			//::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
// <S><D> #437 in Redmine,2012.07.30, SSDI:Akshay
// This member variable is not used after initialization. Hence commented the below piece of code
//			m_dwFileSize = dwSize;
// <E> #437 in RedMine, 2012.07.30, SSDI:Akshay			
			m_pdata = new WCHAR[dwSize + 1];
			if(m_pdata != NULL)
			{
				SecureZeroMemory(m_pdata, (dwSize + 1)*sizeof(WCHAR));

				if(::ReadFile(hFile, m_pdata, largeintegerbytes.LowPart, &dwRead, NULL) != 0)
					blRet = TRUE;
			}
/*
// <S><A> #437 in Redmine,2012.07.30, SSDI:Akshay
// As per discussion with SDC in Redmine ticket #437, below source code is merged from CR3 production driver.
// <S><A> 2011.08.26 hashida
// <S><C> #487 in Redmine, 2012.10.31, SSDI:Purna
// Following source code is merged from the latest source files provided by SDC.
// <S><C> 2012.10.02 SBC Terauchi(Win8-11)
// Lot4 �㏑���C���X�g�[�����
// Lot4�Ή�1�@�^�C���X�^���v��r �萔�̒�`��ύX������{NULL�������ɕύX
// SCUI_FILEDATETIME_STRLEN�@�iYYYYMMDDHHMMSS �{\n�j
//			m_szDateTime = new WCHAR[SCUI_FILEDATETIME_STRLEN + 1];
			m_szDateTime = new WCHAR[SCUI_FILEDATETIME_STRLEN];
// <E> 2012.10.02 SBC Terauchi(Win8-11)
// <E> #487 in Redmine, 2012.10.31, SSDI:Purna

			if (m_szDateTime != NULL)
			{
// <S><C> #487 in Redmine, 2012.10.31, SSDI:Purna
// Following source code is merged from the latest source files provided by SDC.
// <S><C> 2012.10.02 SBC Terauchi(Win8-11)
// Lot4 �㏑���C���X�g�[�����
//				SecureZeroMemory(m_szDateTime, sizeof(WCHAR)*(SCUI_FILEDATETIME_STRLEN + 1));
				SecureZeroMemory(m_szDateTime, SCUI_FILEDATETIME_STRLEN);
// <E> 2012.10.02 SBC Terauchi(Win8-11)
// <E> #487 in Redmine, 2012.10.31, SSDI:Purna
				// �t�@�C���̎��ԏ����擾
				// �s�v��OUTPUT��NULL
				// http://msdn.microsoft.com/ja-jp/library/cc429752.aspx
// <S><C> #487 in Redmine, 2012.10.31, SSDI:Purna
// Following source code is merged from the latest source files provided by SDC.
// <S><C> 2012.10.02 SBC Terauchi(Win8-11)
// Lot4 �㏑���C���X�g�[�����
//				if(0 == GetFileTime(
				if(0 == ::GetFileTime(
// <E> 2012.10.02 SBC Terauchi(Win8-11)
// <E> #487 in Redmine, 2012.10.31, SSDI:Purna
					hFile,		// [IN]�t�@�C���̃n���h��
					NULL,		// [OUT]�쐬����
					NULL,		// [OUT]�ŏI�A�N�Z�X����
					&fileTime))	// [OUT]�ŏI�X�V����
				{
					wprintf(L"GetFileTime���s\n");
				}

				// ���[�J���̎��Ԃ֕ϊ�
				// http://msdn.microsoft.com/ja-jp/library/cc429724.aspx
// <S><C> #487 in Redmine, 2012.10.31, SSDI:Purna
// Following source code is merged from the latest source files provided by SDC.
// <S><C> 2012.10.02 SBC Terauchi(Win8-11)
// Lot4 �㏑���C���X�g�[�����
//				if(0 == FileTimeToLocalFileTime(&fileTime, &localFileTime))
				if(0 == ::FileTimeToLocalFileTime(&fileTime, &localFileTime))
// <E> 2012.10.02 SBC Terauchi(Win8-11)
// <E> #487 in Redmine, 2012.10.31, SSDI:Purna
				{
					wprintf(L"FileTimeToLocalFileTime���s\n");
				}

				// SYSTEMTIME�\���̂֕ϊ�
				// http://msdn.microsoft.com/ja-jp/library/cc429727.aspx
// <S><C> #487 in Redmine, 2012.10.31, SSDI:Purna
// Following source code is merged from the latest source files provided by SDC.
// <S><C> 2012.10.02 SBC Terauchi(Win8-11)
// Lot4 �㏑���C���X�g�[�����
//				if(0 == FileTimeToSystemTime(&localFileTime, &systemTime))
				if(0 == ::FileTimeToSystemTime(&localFileTime, &systemTime))
// <E> 2012.10.02 SBC Terauchi(Win8-11)
// <E> #487 in Redmine, 2012.10.31, SSDI:Purna

				{
					wprintf(L"FileTimeToSystemTime���s\n");
				}

				StringPrintf(m_szDateTime,
					L"%04d%02d%02d%02d%02d%02d",
					systemTime.wYear,
					systemTime.wMonth,
					systemTime.wDay,
					systemTime.wHour,
					systemTime.wMinute,
					systemTime.wSecond
					);
			}
// <E> 2011.08.26 hashida
// <E> #437 in RedMine, 2012.07.30, SSDI:Akshay
			::CloseHandle(hFile);
		}
*/
            ::CloseHandle(hFile);
	}
	else
	{
		slog.putLog("CShIniFile::Initialize -> INVALID_HANDLE_VALUE(CreateFile)\n"); 
	}
}
return blRet;
}


//=============================================================================
// function
//      GetString
//
// parameters
//      pszSec              �Z�N�V������
//      pszKey              �L�[��
//      pszDef              ����̕�����
//      pszRet              ��񂪊i�[�����o�b�t�@
//      dwSize              ���o�b�t�@�̃T�C�Y
//      blUnicode           Unicode�t���O
//
// return value
//      ���� : �o�b�t�@�Ɋi�[���ꂽ������
//      ���s : 
//
// outline
//      Ini�t�@�C�����當������擾����
//=============================================================================
DWORD CShIniFile::GetString(WCHAR *pszSec, WCHAR *pszKey, WCHAR *pszDef, WCHAR *pszRet, DWORD dwSize)
{
	DWORD			dwRet = 0;
	PSISECDATA		pSecData = NULL;

	WCHAR *psz = NULL;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n"); 

	if(pszRet == NULL || dwSize == 0)
		return 0;

	SecureZeroMemory(pszRet, dwSize*sizeof(WCHAR));

	psz = new WCHAR[dwSize + 1];
	if(psz == NULL)
	{
		slog.putLog("CShIniFile::GetString -> NULL_POINTER(psz)\n");
		return 0;
	}

	SecureZeroMemory(psz, (dwSize + 1)*sizeof(WCHAR));

	if(m_blDirect != FALSE || m_pdata == NULL)
	{
		dwRet = (DWORD)::GetPrivateProfileString(pszSec, pszKey, pszDef, psz, dwSize, m_pszIniFPath);
		wcscpy_s(pszRet,dwSize,psz);
		/*if(blUnicode && IsUnicode())
#ifdef _WIN32
			::MultiByteToWideWCHAR(SCUI_CP, SCUI_MB, psz, -1, (wWCHAR_t *)pszRet, dwSize);
#else
			mbstowcs((wWCHAR_t *)pszRet, psz, ::strlen(psz)); // �����{��ŉ�����(16bit�ł͏�������Ȃ�)
#endif
		else
			::strcpy_s(pszRet, dwSize, psz);*/
	}
	else
	{
		if(m_pdata != NULL)
		{
			pSecData = GetSectionDataAddr(pszSec, m_pdata);
			dwRet = GetKeyData(pszKey, pSecData, psz, dwSize);
			wcscpy_s(pszRet,dwSize,psz);
			/*if(blUnicode && IsUnicode())
	#ifdef _WIN32
				::MultiByteToWideWCHAR(SCUI_CP, SCUI_MB, psz, -1, (wWCHAR_t *)pszRet, dwSize);
	#else
				mbstowcs((wWCHAR_t *)pszRet, psz, ::strlen(psz)); // �����{��ŉ�����(16bit�ł͏�������Ȃ�)
	#endif
			else
				::strcpy_s(pszRet, dwSize, psz);*/
		}
		else
		{
			slog.putLog("CShIniFile::GetString -> NULL_POINTER(m_pdata)\n"); 
		}

		if(pszDef != NULL && dwRet == 0)
		{
			::wcsncpy_s(pszRet, dwSize, pszDef, (size_t)dwSize - 1);
			dwRet = ::wcslen(pszRet);
		}
	}

	if(psz != NULL)
		delete[] psz;

	return dwRet;
}


//=============================================================================
// function
//      GetString
//
// parameters
//      pszSec              �Z�N�V������
//      lKey                �L�[�l
//      pszDef              ����̕�����
//      pszRet              ��񂪊i�[�����o�b�t�@
//      dwSize              ���o�b�t�@�̃T�C�Y
//      blUnicode           Unicode�t���O
//
// return value
//      ���� : �o�b�t�@�Ɋi�[���ꂽ������
//      ���s : 
//
// outline
//      Ini�t�@�C�����當������擾����
//=============================================================================
DWORD CShIniFile::GetString(WCHAR *pszSec, long lKey, WCHAR *pszDef, WCHAR *pszRet, DWORD dwSize)
{
	WCHAR			szKey[32];

	SecureZeroMemory(szKey, sizeof(szKey));
	StringCbPrintf(szKey,sizeof(szKey), L"%ld", lKey);

	return GetString(pszSec, szKey, pszDef, pszRet, dwSize);
}

// <S><A> #437 in Redmine,2012.07.30, SSDI:Akshay
// As per discussion with SDC in Redmine ticket #437, below source code is merged from CR3 production driver.
// <S><A> 2011.08.26 hashida
//=============================================================================
// function
//      GetTimeStamp
//
// parameters
//
// return value
//      ���� : �o�b�t�@�Ɋi�[���ꂽ������
//      ���s : 
//
// outline
//      ini File�̍X�V�����擾
//=============================================================================
WCHAR* CShIniFile::GetTimeStamp()
{
	return m_szDateTime;
}
// <E> 2011.08.26 hashida
// <E> #437 in RedMine, 2012.07.30, SSDI:Akshay

//=============================================================================
// function
//      GetLong
//
// parameters
//      pszSec              �Z�N�V������
//      pszKey              �L�[��
//      lDef                ����̒l
//
// return value
//      ���� : �擾�����l
//      ���s : �f�t�H���g�l
//
// outline
//      Ini�t�@�C������long�l���擾����
//=============================================================================
long CShIniFile::GetLong(WCHAR *pszSec, WCHAR *pszKey, long lDef)
{
	long			lRet;
	WCHAR			szDef[64] ={0};
	WCHAR			szData[64] = {0};

	lRet = lDef;
	StringCopy(szDef, SCUI_INI_LONGSTR_DEF);
	if(GetString(pszSec, pszKey, szDef, szData, _countof(szData)) != (DWORD)::wcslen(szDef))
		lRet = ::_wtol(szData);

	return lRet;
}


//=============================================================================
// function
//      GetLong
//
// parameters
//      pszSec              �Z�N�V������
//      pszKey              �L�[��
//      lDef                ����̒l
//
// return value
//      ���� : �擾�����l
//      ���s : �f�t�H���g�l
//
// outline
//      Ini�t�@�C������long�l���擾����
//=============================================================================
long CShIniFile::GetLong(WCHAR *pszSec, long lKey, long lDef)
{
	long			lRet;
	WCHAR			szDef[64] = {0};
	WCHAR			szData[64] = {0};

	lRet = lDef;
	StringCopy(szDef, SCUI_INI_LONGSTR_DEF);
	if(GetString(pszSec, lKey, szDef, szData, _countof(szData)) != (DWORD)::wcslen(szDef))
		lRet = ::_wtol(szData);

	return lRet;
}


//=============================================================================
// function
//      GetLongArray
//
// parameters
//      pszSec              �Z�N�V������
//      pszKey              �L�[��
//      plValue             Long�l���i�[����o�b�t�@
//      lCount              plValue�Ɋi�[�ł���l��
//
// return value
//      ���� : �擾�����f�[�^��
//      ���s : 0
//
// outline
//      Ini�t�@�C������long�l���擾����
//=============================================================================
long CShIniFile::GetLongArray(WCHAR *pszSec, WCHAR *pszKey, long *plValue, long lCount)
{
	long			lRet = 0;
	long			lRemain = 0;
	WCHAR *pszData = NULL;
	WCHAR *psz = NULL;
	WCHAR *psz2 = NULL;
	long *pl = plValue;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n"); 
	if( (nullptr == plValue) || (0 == lCount))
	{
		lRemain = InitLongArray();
		if(lRemain == 0)
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

	if((nullptr == pl) || (0 == lRemain))
	{
		return lRet;
	}
	pszData = new WCHAR[INI_STR_MAX];
	if(nullptr == pszData)
	{
		slog.putLog("CShIniFile::GetLongArray -> NULL_POINTER(pszData)\n"); 
		return lRet;
	}
	SecureZeroMemory(pszData, INI_STR_MAX*sizeof(WCHAR));
	if(GetString(pszSec, pszKey, L"", pszData, INI_STR_MAX) != 0)
	{
		psz = pszData;
		while(psz != NULL && *psz != L'\0' && lRemain != 0)
		{
			psz2 = ::wcschr(psz, L',');
			if(psz2 == NULL)
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

			while(*psz == 0x20 || *psz == L'\t')
				psz++;
		}
	}
	delete[] pszData;
	pszData = NULL ;
	return lRet;
}


//=============================================================================
// function
//      GetLongArray
//
// parameters
//      pszSec              �Z�N�V������
//      lKey                �L�[�l
//      plValue             Long�l���i�[����o�b�t�@
//      lCount              plValue�Ɋi�[�ł���l��
//
// return value
//      ���� : �擾�����f�[�^��
//      ���s : 0
//
// outline
//      Ini�t�@�C������long�l���擾����
//=============================================================================
long CShIniFile::GetLongArray(WCHAR *pszSec, long lKey, long *plValue, long lCount)
{
	WCHAR			szKey[32];

	SecureZeroMemory(szKey, sizeof(szKey));
	StringCbPrintf(szKey, sizeof(szKey),L"%ld", lKey);

	return GetLongArray(pszSec, szKey, plValue, lCount);
}


//=============================================================================
// function
//      GetLongArrayData
//
// parameters
//      ����
//
// return value
//      m_plData�A�h���X
//
// outline
//      m_plData�A�h���X��Ԃ�
//=============================================================================
long *CShIniFile::GetLongArrayData(void)
{
	return m_plData;
}


//=============================================================================
// function
//       GetArrayValue
//
// parameters
//       pszSection    ini�t�@�C���Z�N�V����
//       pszKey        ini�t�@�C���L�[
//       wID           �L�[ID
//       wArray        1�g�ɑ�����f�[�^��
//       wDataPos      �f�[�^�̈ʒu
//       wIDPos        wID�̈ʒu
//
// return value
//       �擾�����f�[�^
//
// outline
//       �w�肳�ꂽINI�t�@�C������f�[�^���擾����
//=============================================================================
long CShIniFile::GetArrayValue(WCHAR *pszSection, WCHAR *pszKey, short wID,
								short wArray, short wDataPos, short wIDPos, long lDef)
{
	long			lVal = lDef;

	long			lCount;
	long *pl = NULL;

	long			i;

	if((lCount = GetLongArray(pszSection, pszKey)) != 0)
	{
		pl = GetLongArrayData();
		for(i = 0; i < lCount / wArray; i++)
		{
			if(pl[(i * wArray) + wIDPos] == wID)
			{
				lVal = pl[(i * wArray) + wDataPos];
				break;
			}			
		}
	}

	return lVal;
}


//=============================================================================
// function
//       GetArrayValue
//
// parameters
//       pszSection    ini�t�@�C���Z�N�V����
//       lKey          �L�[�l
//       wID           �L�[ID
//       wArray        1�g�ɑ�����f�[�^��
//       wDataPos      �f�[�^�̈ʒu
//       wIDPos        wID�̈ʒu
//
// return value
//       �擾�����f�[�^
//
// outline
//       �w�肳�ꂽINI�t�@�C������f�[�^���擾����
//=============================================================================
long CShIniFile::GetArrayValue(WCHAR *pszSection, long lKey, short wID,
								short wArray, short wDataPos, short wIDPos, long lDef)
{
	WCHAR			szKey[32];

	SecureZeroMemory(szKey, sizeof(szKey));
	StringCbPrintf(szKey,sizeof(szKey), L"%ld", lKey);

	return GetArrayValue(pszSection, szKey, wID, wArray, wDataPos, wIDPos, lDef);
}


//=============================================================================
// function
//      GetDataAddr
//
// parameters
//      ����
//
// return value
//      m_plData�A�h���X
//
// outline
//      m_pdata�A�h���X��Ԃ�
//=============================================================================
WCHAR *CShIniFile::GetDataAddr(void)
{
	return m_pdata;
}

//=============================================================================
// function
//      GetSectionDataAddr
//
// parameters
//      pszSec        �Z�N�V������
//      pData         �J�����g�f�[�^�A�h���X
//
// return value
//      ���� : SISECDATA�A�h���X
//      ���s : NULL
//
// outline
//      �w�肳�ꂽ�Z�N�V������SISECDATA�A�h���X���擾����
//=============================================================================
PSISECDATA CShIniFile::GetSectionDataAddr(WCHAR *pszSec, WCHAR *pData)
{
	WCHAR *pAddr = NULL;
	WCHAR *p = NULL;
	WCHAR *p2 = NULL;

	WCHAR *pszSection = NULL;

	PSISECDATA		psd = NULL;

	PSISECDATA		psd1 = NULL;
	PSISECDATA		psd2 = NULL;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n"); 

	long			i;
	if(m_psecdata != NULL)
	{
		psd1 = m_psecdata;

		while(psd1 != NULL)
		{
			if(::_wcsicmp((*psd1).pszSection, pszSec) == 0)
				return psd1;

			psd2 = (PSISECDATA)(*psd1).pnext;
			psd1 = psd2;
		}
	}

	pszSection = NULL;

	if(pszSec != NULL && pData != NULL)
	{
		p = pData;

		while(*p != L'\0')
		{
			// ================================================
			// _/_/_/  �s���̔��p�X�y�[�X�ƃ^�u���X�L�b�v
			// ================================================
			while(*p == 0x20 || *p == L'\t')
				p++;
		
			// ================================================
			// _/_/_/  �Z�N�V�����̎n�܂�ł���Ώ���
			// ================================================
			if(*p == L'[')
			{
				p++;

				// ================================================
				// _/_/_/  []���̔��p�X�y�[�X�ƃ^�u���X�L�b�v
				// ================================================
				while(*p == 0x20 || *p == L'\t')
					p++;
			
				// ================================================
				// _/_/_/  �Z�N�V�������̔����o��
				// ================================================
				if(pszSection == NULL)
				{
					pszSection = (WCHAR *)new WCHAR[SCUI_INI_SEC_MAX];
					if(pszSection == NULL)
					{
						slog.putLog("CShIniFile::GetSectionDataAddr -> NULL_POINTER(pszSection)\n");
						break;
					}
				}

				SecureZeroMemory(pszSection, SCUI_INI_SEC_MAX*sizeof(WCHAR));
				i = 0;
				while(*p != L'\n' && *p != L'\0' && i < SCUI_INI_SEC_MAX)
				{
					pszSection[i] = *p;
					i++;
					p++;
				}

				p2 = ::wcsrchr(pszSection, L']');
				if(p2 == NULL)
				{
					p2 = ::wcsrchr(pszSection, L'\0');
					if(p2 != NULL)
					{
						p2--;
						while(*p2 == 0x20 || *p2 == L'\t' || *p2 == L'\r')
							p2--;

						p2++;
					}
				}

				if(p2 != NULL)
				{
					*p2 = L'\0';
					while(*p2 == 0x20 || *p2 == L'\t')
					{
						*p2 = L'\0';
						p2--;
						if(p2 == pszSection)
							break;
					}

					if(::_wcsicmp(pszSection, pszSec) == 0)
					{
						if(*p != L'\0')
							p++;

						pAddr = p;
						break;
					}
				}

			}
			else
			{
				// ================================================
				// _/_/_/  ���s�܂ŃX�L�b�v
				// ================================================
				while(*p != L'\n' && *p != L'\0')
					p++;

				if(*p == L'\0')
					break;

				p++;
			}
		}
	}

	if(pAddr != NULL)
	{
		psd = CreateSectionData(pszSec, pAddr);
		if(psd != NULL)
			SetLastSectionData(psd);			
	}

	if(pszSection != NULL)
		delete[] pszSection;

	return psd;
}


//=============================================================================
// function
//      GetNextSectionAddr
//
// parameters
//      pData               �J�����g�f�[�^�A�h���X
//
// return value
//      ���� : �쐬���ꂽSISECDATA�\���̃A�h���X
//      ���s : NULL
//
// outline
//      �w��̃f�[�^���g�p����SISECDATA�\���̂��쐬����
//=============================================================================
WCHAR *CShIniFile::GetNextSectionAddr(WCHAR *pData)
{
	WCHAR *pline = NULL;
	WCHAR *p = NULL;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n"); 

	if(pData != NULL)
	{
		p = pData;
		pline = pData;

		while(*p != L'\0')
		{
			// ================================================
			// _/_/_/  �s���̔��p�X�y�[�X�ƃ^�u���X�L�b�v
			// ================================================
			while(*p == 0x20 || *p == L'\t')
				p++;

			// ================================================
			// _/_/_/  �Z�N�V�����̎n�܂�ł���Ώ���
			// ================================================
			if(*p == L'[')
				break;
			else
			{
				// ================================================
				// _/_/_/  ���s�܂ŃX�L�b�v
				// ================================================
				while(*p != L'\n' && *p != L'\0')
					p++;

				if(*p == L'\0')
				{
// >> 2008.12.11 SSL:K.Hamaguchi
//					pline = p;
					pline = NULL;
// << 2008.12.11 SSL:K.Hamaguchi
					break;
				}

				p++;
				pline = p;
			}
		}
	}
	else
	{
		slog.putLog("CShIniFile::GetNextSectionAddr -> NULL_POINTER(pData)\n"); 
	}

	return pline;
}


//=============================================================================
// function
//      GetKeyData
//
// parameters
//      pszKey              �L�[��
//      pData               �J�����g�f�[�^�A�h���X
//      pszRet              ��񂪊i�[�����o�b�t�@
//      dwSize              ���o�b�t�@�̃T�C�Y
//
// return value
//      ���� : �o�b�t�@�Ɋi�[���ꂽ������
//      ���s : 0
//
// outline
//      �w��̃L�[�ɊY������f�[�^���擾����
//=============================================================================
DWORD CShIniFile::GetKeyData(WCHAR *pszKey, PSISECDATA pSecData, WCHAR *pszRet, DWORD dwSize)
{
	DWORD			dwRet = 0;
	DWORD			dwRemain;
	long			lDataRemain;

	WCHAR *pKeyTemp = NULL;

	WCHAR *p = NULL;
	WCHAR *pbuf = NULL;

	long			i;

	if(pSecData != NULL && pszRet != NULL)
	{
		p = (*pSecData).pDataAddr;
		lDataRemain = (long)(*pSecData).dwSize;

		pKeyTemp = new WCHAR[SCUI_INI_KEY_MAX];
		if(pKeyTemp != NULL)
		{
			while(*p != L'\0' && lDataRemain > 0)
			{
				// ================================================
				// _/_/_/  �s���̔��p�X�y�[�X�ƃ^�u���X�L�b�v
				// ================================================
				//while(*p == 0x20 || *p == L'\t'  || *p == L'\r' || *p == L'\n'&& lDataRemain > 0)
				
				while(*p == 0x20 || *p == L'\t'  && lDataRemain > 0)
				{
					p++;
					lDataRemain--;
					
				}
				i = 0;
				SecureZeroMemory(pKeyTemp, SCUI_INI_KEY_MAX*sizeof(WCHAR));
				//while(*p != L'=' && *p != L'\n' && *p != L'\0' && *p != 0x20 && *p != L'\t' && *p != L'\r' && lDataRemain > 0)
				while(*p != L'=' && *p != L'\n' && *p != L'\0' && *p != 0x20 && *p != L'\t' && lDataRemain > 0)
				{
					pKeyTemp[i] = *p;
					p++;
					i++;
					lDataRemain--;
				}
				//<S><C> To Fix Feedback issue 3090 (SDC SQA bug). Harika, 20230217
				//Fixed as per UD3. Issue is: Model name we are getting is wrong from OID File itself Ex For : "MX-PN15B" as "MX-PN15B ". Hence compared based on length of the string of DAT file
				if (sh_strcmp(MCF_SEC_AUTOCONFIG_PUNCHMODULE, (*pSecData).pszSection) == 0)
				{
					if (_wcsnicmp(pKeyTemp, pszKey, wcslen(pKeyTemp)) == 0)
						break;
				}
				else
				{
					if (::_wcsicmp(pKeyTemp, pszKey) == 0)
						break;
				}			
				//<E> To Fix Feedback issue 3090 (SDC SQA bug). Harika, 20230217

			//	while(*p != L'\n' && *p != L'\0' && *p != L'\r' && lDataRemain > 0)
				while(*p != L'\n' && *p != L'\0' && lDataRemain > 0)
				{
					p++;
					lDataRemain--;
				}

				if(*p == L'\0')
					break;

				p++;
				lDataRemain--;
			}
		
			// ================================================
			// _/_/_/  �s���̔��p�X�y�[�X�ƃ^�u���X�L�b�v
			// ================================================
			/*while(*p == 0x20 || *p == L'\t' && lDataRemain > 0)
			{
				p++;
				lDataRemain--;
			}*/
			//while(*p == 0x20 || *p == L'\t'  || *p == L'\r' || *p == L'\n'&& lDataRemain > 0)
			while(*p == 0x20 || *p == L'\t' && lDataRemain > 0)
				{
					p++;
					lDataRemain--;
					
				}
			if(*p == L'=')
			{
				p++;
				lDataRemain--;

				// ================================================
				// _/_/_/  �s���̔��p�X�y�[�X�ƃ^�u���X�L�b�v
				// ================================================
				while(*p == 0x20 || *p == L'\t' && lDataRemain > 0)
				{
					p++;
					lDataRemain--;
				}

				if(*p == L'\'' || *p == L'\"')
				{
					p++;
					lDataRemain--;
				}
			
				dwRemain = dwSize - 1;
				pbuf = pszRet;

				while(dwRemain > 0 && *p != L'\r' && *p != L'\0' && *p != L'\'' && *p != L'\"' && lDataRemain > 0)
				{
					*pbuf = *p;
					pbuf++;
					p++;
					dwRemain--;
					dwRet++;
					lDataRemain--;
				}

				if(pbuf != pszRet)
				{
					pbuf--;

					if(*p != L'\'' && *p != L'\"')
					{
						while(*pbuf == 0x20)
						{
							*pbuf = L'\0';
							pbuf--;
						}
					}
				}
			}
	
			delete[] pKeyTemp;
		}
	}


	return dwRet;
}


//=============================================================================
// function
//      CreateSectionData
//
// parameters
//      pszSec              �Z�N�V������
//      pDataAddr           �Z�N�V�����f�[�^�A�h���X
//
// return value
//      ���� : �쐬���ꂽSISECDATA�\���̃A�h���X
//      ���s : NULL
//
// outline
//      �w��̃f�[�^���g�p����SISECDATA�\���̂��쐬����
//=============================================================================
PSISECDATA CShIniFile::CreateSectionData(WCHAR *pszSec, WCHAR *pDataAddr)
{
	WCHAR *pnext = NULL;

	PSISECDATA		psd = NULL;

	psd = new SISECDATA;
	if(psd != NULL)
	{
		SecureZeroMemory(psd, sizeof(SISECDATA));

		(*psd).pszSection = new WCHAR[::wcslen(pszSec) + 1];
		if((*psd).pszSection != NULL)
		{
			SecureZeroMemory((*psd).pszSection, (::wcslen(pszSec) + 1)*sizeof(WCHAR));
			StringCbCopy((*psd).pszSection,(wcslen(pszSec) + 1)*sizeof(WCHAR), pszSec);
		}

		(*psd).pDataAddr = pDataAddr;
		
		pnext = GetNextSectionAddr(pDataAddr);
		if(pnext == NULL)
			(*psd).dwSize = 0;
		else
			(*psd).dwSize = (DWORD)((WCHAR*)pnext - (WCHAR*)pDataAddr) + 1;
	}
	
	return psd;
}


//=============================================================================
// function
//      GetLastSectionDataAddr
//
// parameters
//      ����
//
// return value
//      ���� : �쐬���ꂽSISECDATA�\���̃A�h���X
//      ���s : NULL
//
// outline
//      �Ō��SISECDATA�\���̃A�h���X���擾����
//=============================================================================
PSISECDATA CShIniFile::GetLastSectionDataAddr(void)
{
	PSISECDATA		psd = NULL;
	PSISECDATA		psd2 = NULL;
	MFPPrinterUI_Logger slog(__FUNCTION__"\n"); 

	if(m_psecdata != NULL)
	{
		psd = m_psecdata;

		while(TRUE)
		{
			if((*psd).pnext != NULL)
			{
				psd2 = (PSISECDATA)(*psd).pnext;
				psd = psd2;
			}
			else
				break;
		}
	}
	else
	{
		slog.putLog("CShIniFile::CreateSectionData -> NULL_POINTER(m_psecdata)\n"); 
	}

	return psd;
}


//=============================================================================
// function
//      InitLongArray
//
// parameters
//      ����
//
// return value
//      ���� : �z��̌�
//      ���s : 0
//
// outline
//      SISECDATA�\���̂��Z�b�g����
//=============================================================================
long CShIniFile::InitLongArray(void)
{
	if(m_plData == NULL)
		m_plData = new long[m_lCount];

	if(m_plData == NULL)
		return 0;

	SecureZeroMemory(m_plData, sizeof(long) * m_lCount);

	return m_lCount;
}


//=============================================================================
// function
//      SetLastSectionData
//
// parameters
//      psd                 �Z�b�g����SISECDATA�\���̃A�h���X
//
// return value
//      ����
//
// outline
//      SISECDATA�\���̂��Z�b�g����
//=============================================================================
void CShIniFile::SetLastSectionData(PSISECDATA psd)
{
	PSISECDATA		psdtmp = NULL;
	if(m_psecdata == NULL)
		m_psecdata = psd;
	else
	{
		psdtmp = GetLastSectionDataAddr();
		(*psdtmp).pnext = psd;
	}
}


//=============================================================================
// function
//      DeleteSectionData
//
// parameters
//      ����
//
// return value
//      ����
//
// outline
//      SISECDATA�\���̂��폜����
//==============================================================================
void CShIniFile::DeleteSectionData(void)
{
    MFPPrinterUI_Logger slog(__FUNCTION__"\n"); 

	if(NULL != m_psecdata)
	{
		PSISECDATA psd = GetLastSectionDataAddr();
        if(NULL == psd)
            return;
		while(psd != m_psecdata)
		{
            PSISECDATA		psdp = m_psecdata;

			if((NULL != psd) && NULL != (*psd).pszSection)
			{
				delete[] (*psd).pszSection;
				(*psd).pszSection = NULL;
			}

			while(psd != (PSISECDATA)(*psdp).pnext)
			{
				PSISECDATA psdtmp = (PSISECDATA)(*psdp).pnext;
				psdp = psdtmp;
			}

			delete psd;
			(*psdp).pnext = NULL;

			psd = GetLastSectionDataAddr();
		}


		if(NULL != (*m_psecdata).pszSection)
		{
			delete[] (*m_psecdata).pszSection;
			(*m_psecdata).pszSection = NULL;
		}

		delete m_psecdata;
		m_psecdata = NULL;
    }
	else
	{
		slog.putLog("CShIniFile::DeleteSectionData -> NULL_HANDLE(m_psecdata)\n"); 
	}
}
DWORD CShIniFile::GetDefaultLangID()
{
    DWORD dwLang_Count = (DWORD)GetLongArray(m_pszDrvName, L"SupportLang");
    DWORD dwDefaultLangID = (DWORD)GetLong(m_pszDrvName, L"DefaultLang");
    
    //Set the Language ID to 0 (English) if sLangID is other than the supported languages.
    if (dwDefaultLangID >= dwLang_Count)
    {
        dwDefaultLangID = 0;
    }
    return dwDefaultLangID;
}

//=============================================================================
// function
//      IsUseACM
//
// parameters
//
// return value
//      �g�p����@ : TRUE
//      �g�p���Ȃ� : FALSE
//
// outline
//      AutoConfigModule���g�p���邩�ǂ������擾����
//=============================================================================
BOOL CShIniFile::IsUseACM(void)
{
	if (GetLong(MCF_SEC_AUTOCONFIG, MCF_KEY_AC_USE_ACM, 1) == 1)
		return TRUE;

	return FALSE;
}


// >> 2009.03.03 SSL:K.Hamaguchi
//=============================================================================
// function
//      IsACDir
//
// parameters
//
// return value
//      ACM�`��   : TRUE
//      Smon�`��  : FALSE
//
// outline
//      AutoConfig���̃��W�X�g���`�����擾����B
//      
//=============================================================================
BOOL CShIniFile::IsACDir(void)
{
	if (GetLong(MCF_SEC_AUTOCONFIG, MCF_KEY_AC_DIR, 1) == 1)
		return TRUE;

	return FALSE;
}
// << 2009.03.03 SSL:K.Hamaguchi

// <S><A> To Support Single Sign-on , 2019.03.26, SSDI:Chandrashekar .V
//=============================================================================
// function
//      IsACSingleSignOn
//
// parameters
//
// return value
//      Success  : TRUE
//      Failure  : FALSE
//
// outline
//     
//      
//=============================================================================
BOOL CShIniFile::IsACSingleSignOn(void)
{
	if (GetLong(MCF_SEC_AUTOCONFIG, MCF_KEY_AC_SINGLE_SIGN_ON) == 1)
		return TRUE;

	return FALSE;
}
//<E> To Support Single Sign-on ,2019.03.26, SSDI:Chandrashekar .V

//=============================================================================
// function
//      IsExistValue
//
// parameters
//      pszSec              �Z�N�V������
//      lKey                �L�[�l
//      lValue              �`�F�b�N����l
//
// return value
//      �f�[�^�����݂���   : TRUE
//      �f�[�^�����݂��Ȃ� : FALSE
//
// outline
//      �w��̃L�[����擾�����f�[�^�Ɏw��̃f�[�^�����݂��邩�ǂ������`�F�b�N����
//=============================================================================
BOOL CShIniFile::IsExistValue(WCHAR FAR *pszSec, WCHAR FAR *pszKey, long lValue)
{
	BOOL			blRet = FALSE;

	long			lCount = 0;
	long			i;

	lCount = GetLongArray(pszSec, pszKey);
	for (i = 0; i < lCount; i++)
	{
		if (m_plData[i] == lValue)
		{
			blRet = TRUE;
			break;
		}
	}

	return blRet;
}


//=============================================================================
// function
//      IsExistValue
//
// parameters
//      pszSec              �Z�N�V������
//      lKey                �L�[�l
//      lValue              �`�F�b�N����l
//
// return value
//      �f�[�^�����݂���   : TRUE
//      �f�[�^�����݂��Ȃ� : FALSE
//
// outline
//      �w��̃L�[����擾�����f�[�^�Ɏw��̃f�[�^�����݂��邩�ǂ������`�F�b�N����
//=============================================================================
BOOL CShIniFile::IsExistValue(WCHAR FAR *pszSec, long lKey, long lValue)
{
	BOOL			blRet = FALSE;

	WCHAR			szKey[32];

	SecureZeroMemory(szKey, sizeof(szKey));
	::wsprintf(szKey, L"%ld", lKey);

	return IsExistValue(pszSec, szKey, lValue);
}

// LCID���`�F�b�N���A�f�t�H���g�v�����^�[�̒l��ύX����Ή�
//=============================================================================
// function
//      GetLongDPLCID
//
// parameters
//      pszKey              �L�[��
//      lDef                ����̒l
//
// return value
//      ���� : �擾�����l
//      ���s : �f�t�H���g�l
//
// outline
//      Ini�t�@�C������long�l���擾����
//=============================================================================
long CShIniFile::GetLongDPLCID(WCHAR FAR *pszKey, long lDef)
{
	long lRet = -1;

	if (m_pszSection != NULL)
		lRet = GetLong(m_pszSection, pszKey, -1);

	if (lRet == -1)
		lRet = GetLong(MCF_SEC_DEFAULTPRINTER, pszKey, lDef);

	return lRet;
}

// LCID���`�F�b�N���A�f�t�H���g�v�����^�[�̒l��ύX����Ή�
//=============================================================================
// function
//       GetLCID
//
// parameters
//       blFlag  LCID�̎擾�Ώۂ̐؂�ւ�
//          FALSE : System��LCID
//          TRUE  : User��LCID
//
// return value
//       ���ԍ�
//
// outline
//       ���ݓ��삵�Ă�����̍��ԍ����擾����
//=============================================================================
LCID CShIniFile::GetLCID(BOOL blFlag)
{
	LCID dwLcid = 0;

	if (blFlag == FALSE)
		dwLcid = GetSystemDefaultLCID();
	else
		dwLcid = GetUserDefaultLCID();

	return dwLcid;
}

//=============================================================================
// function
//      GetLongDPLCID
//
// parameters
//      lKey                �L�[��
//      lDef                ����̒l
//
// return value
//      ���� : �擾�����l
//      ���s : �f�t�H���g�l
//
// outline
//      Ini�t�@�C������long�l���擾����
//=============================================================================
long CShIniFile::GetLongDPLCID(long lKey, long lDef)
{
	long lRet = -1;

	if (m_pszSection != NULL)
		lRet = GetLong(m_pszSection, lKey, -1);

	if (lRet == -1)
		lRet = GetLong(MCF_SEC_DEFAULTPRINTER, lKey, lDef);

	return lRet;
}

//=============================================================================
// function : GetDefaultLang
// parameters : None             
// return value : short
// outline : Gets Default language id from RLV file
//=============================================================================
short CShIniFile::GetDefaultLang(void)
{
	short	sLangID = (short)GetLong(m_pszDrvName, RLV_KEY_DEFAULTLANG, SCUI_DEFAULTLANG_DEF);
	long	SupLangCount = GetLongArray(m_pszDrvName, RLV_KEY_SUPPORTLANG);
	//Set the Language ID to 0 (English) if sLangID is other than the supported languages.
	if ((0 > sLangID) || (SupLangCount <= sLangID))
	{
		sLangID = 0;
	}
	return sLangID;
}

//=============================================================================
// function : GetInstallationType
// parameters : None             
// return value : short
// outline : Retrieves Setup Type specified in the RLV file (0 == BasicMode, 1 = EnhancedMode)
// Refer Redmine ticket #1617 for more details about Setup Type
//=============================================================================
short CShIniFile::GetInstallationType(void)
{
	//<S><C> RedMine Ticket(#1617) , 2014.11.03, SSDI:Gajanana D S
	//return (short)GetLong(m_pszDrvName, RLV_KEY_SETUP_TYPE, SCUI_SETUP_TYPE);
	// If Installation type specified in RLV file is Other than Enhanced Mode(value 1)
	// consider Installation type as Basic Mode.
	short	sInstallationType = (short)GetLong(m_pszDrvName, RLV_KEY_SETUP_TYPE, SCUI_SETUP_TYPE);
	if (eEnhancedMode != sInstallationType)
	{
		sInstallationType = eBasicMode;
	}
	return sInstallationType;
	//<E> RedMine Ticket(#1617) , 2014.11.03, SSDI:Gajanana D S
}

// To Support "Print Release" 
//=============================================================================
// function
//      IsACPrintRelease
//
// parameters
//
// return value
// TRUE or FALSE
//
// outline
//      Checks whether ac_print_release key exists in MCF file
//      
//=============================================================================

BOOL CShIniFile::IsACPrintRelease(void)
{
	if (GetLong(MCF_SEC_AUTOCONFIG, MCF_KEY_AC_PRINT_RELEASE) == 1)
		return TRUE;

	return FALSE;
}

long CShIniFile::IsDiscovery()
{
	return GetLong(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_DISCOVERY, 0);
}

BOOL CShIniFile::IsACDebug(void)
{
	if (GetLong(MCF_SEC_DEBUG, MCF_KEY_DEBUG_AC, 0) == 1)
		return TRUE;

	return FALSE;
}
//<S><A>To Implement Task #3118,22-04-2024,SSDI:Manoj S
//Change the specification for function limitation, display method by changing option
//=============================================================================
// function
//      IsLimitByOptionHideUI
//
// parameters
//     void
//
// return value
//	   TRUE : Hide UI flow
//     FALSE: Not hide UI flow
//
// outline
//     mcf�t�@�C�����炨�C�ɓ���ݒ��json�ɕۑ����邩�`�F�b�N����(����̂�mcf�t�@�C������ǂݍ���)
//=============================================================================
BOOL CShIniFile::IsLimitByOptionHideUI()
{
	if (GetLong(MCF_SEC_PRNOPTONS, MCF_KEY_PRNOPT_LMTBYOPTHIDEUI) == 1)
		return TRUE;

	return FALSE;
}
//<E>To Implement Task #3118,22-04-2024,SSDI:Manoj S
BOOL CShIniFile::IsWriteToJson() {
	BOOL blRet = FALSE;

	if (m_byWriteToJson == -1) {
		//�f�t�H���g��json�ɕۑ�
		long lWriteToJson = this->GetLong(MCF_SEC_PRNOPTONS, MCF_KEY_PRNOPT_WRITE_TO_JSON, 1);

		if (lWriteToJson == 1)
			m_byWriteToJson = 1;
		else
			m_byWriteToJson = 0;
	}

	blRet = m_byWriteToJson == 1 ? TRUE : FALSE;

	return blRet;
}