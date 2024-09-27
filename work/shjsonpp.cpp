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
//      hInst               ���W���[���n���h��
//      pLog                CShPrnLog �N���X�A�h���X
//
// return value
//      ����
//
// outline
//      CShJsonPP �N���X�R���X�g���N�^
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
//      ����
//
// return value
//      ����
//
// outline
//      CShJsonPP �N���X�f�X�g���N�^
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
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      INI�t�@�C���̃f�[�^���g�p���ă��W�X�g��������������
//=============================================================================
BOOL CShJsonPP::reset()
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	PRINTPOSITION	ppd;

	// ================================================
	// _/_/_/  ���ɏ������܂�Ă���ꍇ��reset���Ȃ�
	// ================================================
	if(ReadPPCount() > 1)
		return TRUE;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;
*/
	// ================================================
	// _/_/_/  ���������擾�Ə�������
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
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      INI�t�@�C���̃f�[�^���g�p���ă��W�X�g��������������
//=============================================================================
BOOL CShJsonPP::resetToUs(long lIndex)
{
	BOOL			blRet = FALSE;

	DWORD			dwCount = 0;

	PRINTPOSITION	ppd;
	
	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/

	// ================================================
	// _/_/_/  ���������擾�Ə�������
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
//      pppd                ��������PP�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽPP�\���̂̓��e���w��̃C���f�b�N�X�Ń��W�X�g����
//      ��������
//=============================================================================
BOOL CShJsonPP::WritePPData(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;

	// ================================================
	// _/_/_/  PRINTPOSITION�m��
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
//      lIndex              �������ރC���f�b�N�X
//      pppd                ��������PRINTPOSITION�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽPRINTPOSITION�\���̂̓��e���w��̃C���f�b�N�X�Ń��W�X�g����
//      ��������
//=============================================================================
BOOL CShJsonPP::WritePPDataToUs(long lIndexUs, LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  PRINTPOSITION�m��
	// ================================================
	if(pppd == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  ��������
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
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      PP������������
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
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      PP������������
//=============================================================================
BOOL CShJsonPP::WritePPCount(WCHAR FAR *pszBase, long lIndex, DWORD dwCount)
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
//      pppd                �ǂݎ��PP�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��PP�����Ń��W�X�g������ǂݍ���
//=============================================================================
BOOL CShJsonPP::ReadPPData(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;

	if(m_pPrintPositionData == NULL){
		// �����������ɂ����Ă��Ȃ��̂Ń��W�X�g������擾
		blRet = ReadPPDataBase(pppd);
		if(blRet == FALSE){
			// ���W�X�g�����Ȃ�
			// �G���[�ŕԂ�
			goto EXIT;
		}
		// �擾�����f�[�^���������ɂ��R�s�[���Ă���
		m_pPrintPositionData = new PRINTPOSITION;
		if(m_pPrintPositionData == NULL){
			goto EXIT;
		}
		memcpy(m_pPrintPositionData, pppd, sizeof(PRINTPOSITION));
		m_blWriteFlg = TRUE;
	}
	else{
		// ���Ƀ������ɂ����Ă���̂ł����Ԃ�
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
//      pppd                �ǂݎ��PP�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��PP�����Ń��W�X�g������ǂݍ���
//=============================================================================
BOOL CShJsonPP::ReadPPDataBase(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*psz = NULL;

	if(ReadPPCount() <= 0)
		goto EXIT;

	// ================================================
	// _/_/_/  PRINTPOSITION�m��
	// ================================================
	if(pppd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pppd, sizeof(PRINTPOSITION));

	// ================================================
	// _/_/_/  �ǂݍ���
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
//      lIndex              �ǂݎ��C���f�b�N�X
//      pwmd                �ǂݎ��PRINTPOSITION�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��PRINTPOSITION�����Ń��W�X�g������ǂݍ���
//=============================================================================
BOOL CShJsonPP::ReadPPDataToUs(long lIndexUs, LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*psz = NULL;

	// ================================================
	// _/_/_/  PRINTPOSITION�m��
	// ================================================
	if(pppd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pppd, sizeof(PRINTPOSITION));

	// ================================================
	// _/_/_/  �ǂݍ���
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
//      ���� : �o�^PP��
//      ���s : 0
//
// outline
//      PP����ǂݍ���
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
//      ���� : �o�^PP��
//      ���s : 0
//
// outline
//      PP����ǂݍ���
//=============================================================================
DWORD CShJsonPP::ReadPPCount(WCHAR *pszBase, long lIndex)
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
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : INI�t�@�C������擾������
//      ���s : 0
//
// outline
//      INI�t�@�C���̃f�t�H���gPP�����擾����
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
//      pppd                �����󂯎��PP�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽINI�t�@�C���̃f�t�H���gPP���擾����
//=============================================================================
BOOL CShJsonPP::GetPPDefData(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;

	long			lCount;
	long			lPos;

	WCHAR FAR		*pc = NULL;
	long FAR		*pl = NULL;

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
	// ================================================
	if(pppd == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
		goto EXIT;*/

	// ================================================
	// _/_/_/  ���̎擾
	// ================================================
	//if((lCount = (*m_pmcf).GetLongArray(MCF_SEC_PROPSETTINGS, MCF_KEY_PROP_SUP_PPOS)) == 0)
	//	goto EXIT;

	if ((lCount = (GetLongArray()) == 0))
		goto EXIT;

	pl = GetLongArrayData();
	if(pl == NULL)
		goto EXIT;

	// ================================================
	// _/_/_/  ���̍쐬
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
//      pppd                ��������PP�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽPRINTPOSITION�\���̂̓��e���w��̃C���f�b�N�X�Ń��W�X�g����
//      ��������
//=============================================================================
BOOL CShJsonPP::SetPPData(LPPRINTPOSITION pppd)
{
	BOOL			blRet = FALSE;
	LPPRINTPOSITION	pPrintPositionDataWk = NULL;

	// ================================================
	// _/_/_/  �p�����[�^�`�F�b�N
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
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      PP������������
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
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽPRINTPOSITION�\���̂̓��e���w��̃C���f�b�N�X�Ń��W�X�g����
//      ��������
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
//      pppd                �ǂݎ��PP�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��PP�����Ń��W�X�g������ǂݍ���
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
//      pppd                �ǂݎ��PP�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��PP�����Ń��W�X�g������ǂݍ���
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
//      ����
//
// outline
//     add functions name to json file path
//=============================================================================
void CShJsonPP::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_US);
}
// <E> 2022.05.17 sbc R.Tanaka
