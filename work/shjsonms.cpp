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
//      hInst               ���W���[���n���h��
//      pLog                CShPrnLog �N���X�A�h���X
//
// return value
//      ����
//
// outline
//      CShJsonMS �N���X�R���X�g���N�^
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
//      ����
//
// return value
//      ����
//
// outline
//      CShJsonMS �N���X�f�X�g���N�^
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
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      INI�t�@�C���̃f�[�^���g�p���ă��W�X�g��������������
//=============================================================================
BOOL CShJsonMS::reset()
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	MARGINSHIFTDATA	msd;
	
	DWORD			i;

	// ================================================
	// _/_/_/  ���ɏ������܂�Ă���ꍇ��reset���Ȃ�
	// ================================================
	if(ReadMSCount() > 1)
		return TRUE;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

	// ================================================
	// _/_/_/  ���������擾�Ə�������
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
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      INI�t�@�C���̃f�[�^���g�p���ă��W�X�g��������������
//=============================================================================
BOOL CShJsonMS::resetEx()
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	MARGINSHIFTDATA	msd;
	
	DWORD			i;

	// ================================================
	// _/_/_/  ���ɏ������܂�Ă���ꍇ��reset���Ȃ�
	// ================================================
	if(m_blWriteFlg == TRUE){
		return TRUE;
	}

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

	// ================================================
	// _/_/_/  ���������擾�Ə�������
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
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      INI�t�@�C���̃f�[�^���g�p���ă��W�X�g��������������
//=============================================================================
BOOL CShJsonMS::resetExToUs(long lIndex)
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	MARGINSHIFTDATA	msd;
	
	DWORD			i;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

	// ================================================
	// _/_/_/  ���������擾�Ə�������
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
//      lIndex              �������ރC���f�b�N�X
//      pwmd                ��������MSDATA�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽMS�\���̂̓��e���w��̃C���f�b�N�X�Ń��W�X�g����
//      ��������
//=============================================================================
BOOL CShJsonMS::WriteMSData(long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  MARGINSHIFTDATA�m��
	// ================================================
	if(pmsd == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  ��������
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
//      lIndex              �������ރC���f�b�N�X
//      pmsd                ��������MARGINSHIFTDATA�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽMARGINSHIFTDATA�\���̂̓��e���w��̃C���f�b�N�X�Ń��W�X�g����
//      ��������
//=============================================================================
BOOL CShJsonMS::WriteMSDataToUs(long lIndexUs, long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	// ================================================
	// _/_/_/  MARGINSHIFTDATA�m��
	// ================================================
	if(pmsd == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  ��������
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
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
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
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      write the margin shift count
//=============================================================================
BOOL CShJsonMS::WriteMSCount(WCHAR FAR *pszBase, long lIndex, DWORD dwCount)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	// ================================================
	// _/_/_/  ��������
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
//      lIndex              �ǂݎ��C���f�b�N�X
//      pmsd                �ǂݎ��MS�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��MS�����Ń��W�X�g������ǂݍ���
//=============================================================================
BOOL CShJsonMS::ReadMSData(long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*psz = NULL;

	if(ReadMSCount() < (DWORD)lIndex + 1)
		goto EXIT;

	// ================================================
	// _/_/_/  MARGINSHIFTDATA�m��
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
		// _/_/_/  �ǂݍ���
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
			// ���[�h�Ɏ��s���Ă���B���W�X�g���ɏ�����Ă��Ȃ�
			// �f�t�H���g�l���擾����
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
//      lIndex              �ǂݎ��C���f�b�N�X
//      pmsd                �ǂݎ��MARGINSHIFTDATA�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��MARGINSHIFTDATA�����Ń��W�X�g������ǂݍ���
//=============================================================================
BOOL CShJsonMS::ReadMSDataToUs(long lIndexUs, long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*psz = NULL;
	// ================================================
	// _/_/_/  MARGINSHIFTDATA�m��
	// ================================================
	if(pmsd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pmsd, sizeof(MARGINSHIFTDATA));

	// ================================================
	// _/_/_/  �ǂݍ���
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
//      ���� : �o�^MS��
//      ���s : 0
//
// outline
//      MS����ǂݍ���
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
//      ���� : �o�^MS��
//      ���s : 0
//
// outline
//      MS����ǂݍ���
//=============================================================================
DWORD CShJsonMS::ReadMSCount(WCHAR *pszBase, long lIndex)
{
	BOOL			blSuccess = FALSE;
	DWORD			dwData = 0;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  ��������
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
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : INI�t�@�C������擾������
//      ���s : 0
//
// outline
//      INI�t�@�C���̃f�t�H���gMS�����擾����
//=============================================================================
DWORD CShJsonMS::GetCountIni()
{
	DWORD			dwRet = 0;
	short			wCount = 0;
	long			*plData = NULL;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/

	// ================================================
	// _/_/_/  ���̎擾
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
//      lIndex              �����擾����C���f�b�N�X
//      pmsd                �����󂯎��MS�\���̃A�h���X
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽINI�t�@�C���̃f�t�H���gMS���擾����
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
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(pmsd == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	//if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

	// ================================================
	// _/_/_/  ���̎擾
	// ================================================
	if ((lCount = GetLongArray()) == 0)
		goto EXIT;

	pl = m_plData;
	if(pl == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  ���̍쐬
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
//      lIndex              �������ރC���f�b�N�X
//      pmsd                ��������MARGINSHIFTDATA�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��MARGINSHIFTDATA�\���̂̓��e���������ɕۑ�����
//=============================================================================
BOOL CShJsonMS::SetMSData(long lIndex, LPMARGINSHIFTDATA pmsd)
{
	BOOL			blRet = FALSE;
	LPMARGINSHIFTDATA	pmsdWk = NULL;

	// ================================================
	// _/_/_/  MARGINSHIFTDATA�m��
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
	// _/_/_/  ��������
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
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      Margin Shift��UserSetting������������
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
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �����o�ϐ����N���A����
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
//      pmsd                �ǂݎ��MS�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��MS�����Ń��W�X�g������ǂݍ���
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
//      pmsd                �ǂݎ��MS�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��MS�����Ń��W�X�g������ǂݍ���
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
//      ����
//
// outline
//     add functions name to json file path
//=============================================================================
void CShJsonMS::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_US);
}
// <E> 2022.05.17 sbc R.Tanaka
