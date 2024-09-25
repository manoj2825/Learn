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
//      hInst               ���W���[���n���h��
//      pLog                CShPrnLog �N���X�A�h���X
//
// return value
//      ����
//
// outline
//      CShJsonUserPSize �N���X�R���X�g���N�^
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
//      ����
//
// return value
//      ����
//
// outline
//      CShJsonUserPSize �N���X�f�X�g���N�^
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
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      INI�t�@�C���̃f�[�^���g�p���ă��W�X�g��������������
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
	// _/_/_/  ���ɏ������܂�Ă���ꍇ��reset���Ȃ�
	// ================================================
	if(ReadUPSCount() > 1)
		return TRUE;

	// ================================================
	// _/_/_/  INI�t�@�C���N���X�m��
	// ================================================
	//(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;

// ���ڈ���@�\�Ή�
	/*(CreateObj(scui_obj_cls_multichk) == FALSE)
		goto EXIT;
*/
	// ================================================
	// _/_/_/  ���������擾�Ə�������
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
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      INI�t�@�C���̃f�[�^���g�p���ă��W�X�g��������������
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
	// _/_/_/  INI�t�@�C���N���X�m��
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
	// _/_/_/  ���������擾�Ə�������
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
//      lIndex              �������ރC���f�b�N�X
//      pwmd                ��������USERPAPERSIZEDATA�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽUSERPAPERSIZEDATA�\���̂̓��e���w��̃C���f�b�N�X�Ń��W�X�g����
//      ��������
//=============================================================================
BOOL CShJsonUserPSize::WriteUPSData(long lIndex, LPUSERPAPERSIZEDATA pupsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  USERPAPERSIZEDATA�m��
	// ================================================
	if(pupsd == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  ��������
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
//      lIndex              �������ރC���f�b�N�X
//      pwmd                ��������USERPAPERSIZEDATA�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽUSERPAPERSIZEDATA�\���̂̓��e���w��̃C���f�b�N�X�Ń��W�X�g����
//      ��������
//=============================================================================
BOOL CShJsonUserPSize::WriteUPSDataToUs(long lIndexUs, long lIndex, LPUSERPAPERSIZEDATA pupsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	// ================================================
	// _/_/_/  USERPAPERSIZEDATA�m��
	// ================================================
	if(pupsd == NULL)
		goto EXIT;
	
	// ================================================
	// _/_/_/  ��������
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
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      UPS������������
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
//      dwCount             �������ޒl
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      UserSetting������������
//=============================================================================
BOOL CShJsonUserPSize::WriteUPSCount(WCHAR FAR *pszBase, long lIndex, DWORD dwCount)
{
	BOOL			blRet = FALSE;
	WCHAR FAR		*pszKey = NULL;
	DWORD			dwData = 0;

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
//      lIndex              �ǂݎ��C���f�b�N�X
//      pwmd                �ǂݎ��USERPAPERSIZEDATA�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��UPS�����Ń��W�X�g������ǂݍ���
//=============================================================================
BOOL CShJsonUserPSize::ReadUPSData(long lIndex, LPUSERPAPERSIZEDATA pupsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;
	WCHAR FAR		*psz = NULL;

	BOOL			blRetRead = FALSE;

	// ================================================
	// _/_/_/  USERPAPERSIZEDATA�m��
	// ================================================
	if(pupsd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pupsd, sizeof(USERPAPERSIZEDATA));

	// ================================================
	// _/_/_/  �ǂݍ���
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
//      lIndex              �ǂݎ��C���f�b�N�X
//      pwmd                �ǂݎ��USERPAPERSIZEDATA�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��UPS�����Ń��W�X�g������ǂݍ���
//=============================================================================
BOOL CShJsonUserPSize::ReadUPSDataToUs(long lIndexUs, long lIndex, LPUSERPAPERSIZEDATA pupsd)
{
	BOOL			blRet = FALSE;

	WCHAR FAR		*pszKey = NULL;

	WCHAR FAR		*psz = NULL;

	// ================================================
	// _/_/_/  USERPAPERSIZEDATA�m��
	// ================================================
	if(pupsd == NULL)
		goto EXIT;
	
	SecureZeroMemory(pupsd, sizeof(USERPAPERSIZEDATA));

	// ================================================
	// _/_/_/  �ǂݍ���
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
//      ���� : �o�^UPS��
//      ���s : 0
//
// outline
//      UPS����ǂݍ���
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
//      ���� : �o�^UPS��
//      ���s : 0
//
// outline
//      UPS����ǂݍ���
//=============================================================================
DWORD CShJsonUserPSize::ReadUPSCount(WCHAR *pszBase, long lIndex)
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
//      lIndex              �����擾����C���f�b�N�X
//      pwmd                �����󂯎��USERPAPERSIZEDATA�\���̃A�h���X
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽINI�t�@�C���̃f�t�H���gUPS���擾����
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
//	// _/_/_/  �p�����[�^�`�F�b�N
//	// ================================================
//	if(pupsd == NULL)
//		goto EXIT;
//
//	// ================================================
//	// _/_/_/  INI�t�@�C���N���X�m��
//	// ================================================
//	/*if(CreateObj(scui_obj_ini_mcf) == FALSE)
//		goto EXIT;*/
//
//	/*if(CreateObj(scui_obj_cls_multichk) == FALSE)
//		goto EXIT;
//*/
//	// ================================================
//	// _/_/_/  ���̎擾
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
//		// CustomPaper1...�̗p���T�C�Y�擾���s or lIndex == 0�̏ꍇ�B
//		// �ŏ��̃T�C�Y
//		lCount = (*m_pmcf).GetLongArray(MCF_SEC_PRNOPTONS, MCF_KEY_PRNOPT_DC_MINEXTENT, lData, sizeof(lData) / sizeof(long));
//	}
//
//	if(lCount != 0)
//	{
//		lPaperKind = (*m_pmultichk).getMIPPaper((short)lPaperSize);
//
//		// ================================================
//		// _/_/_/  ���̍쐬
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

// ���ڈ���@�\�Ή�
//=============================================================================
// function
//      GetLPSDefData
//
// parameters
//      lIndex              �����擾����C���f�b�N�X
//      pwmd                �����󂯎��USERPAPERSIZEDATA�\���̃A�h���X
//      pIniFile            CShIniFile �N���X�A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽINI�t�@�C���̃f�t�H���gUPS���擾����
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
	//// _/_/_/  �p�����[�^�`�F�b�N
	//// ================================================
	//if(pupsd == NULL)
	//	goto EXIT;

	//// ================================================
	//// _/_/_/  INI�t�@�C���N���X�m��
	//// ================================================
	///*if(CreateObj(scui_obj_ini_mcf) == FALSE)
	//	goto EXIT;*/

	//// ================================================
	//// _/_/_/  ���̎擾
	//// ================================================
	//// �ŏ��̃T�C�Y
	//lCountW = (*m_pmcf).GetLongArray(MCF_SEC_VALUERANGE, MCF_KEY_VAL_LP_WIDTH,  lDataW, sizeof(lDataW) / sizeof(long));
	//lCountL = (*m_pmcf).GetLongArray(MCF_SEC_VALUERANGE, MCF_KEY_VAL_LP_LENGTH, lDataL, sizeof(lDataL) / sizeof(long));
	//if(lCountW != 0 && lCountL != 0)
	//{
	//	// ================================================
	//	// _/_/_/  ���̍쐬
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
//      lFlag               �ΏۃN���X�t���O
//
// return value
//      ���� : TRUE
//      ���s : FALSE
//
// outline
//      �w�肳�ꂽ�I�u�W�F�N�g�����쐬�̏ꍇ�쐬����
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
//      lFlag               �ΏۃN���X�t���O
//
// return value
//      ����
//
// outline
//      �w�肳�ꂽ�I�u�W�F�N�g���폜����
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
//      pupsd                �ǂݎ��UPS�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��UPS�����Ń��W�X�g������ǂݍ���
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
//      pmsd                �ǂݎ��MS�\���̃A�h���X
//
// return value
//      ���� : TRUE
//      ���s : FALE
//
// outline
//      �w�肳�ꂽ�C���f�b�N�X��UPS�����Ń��W�X�g������ǂݍ���
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
//      ����
//
// outline
//     add functions name to json file path
//=============================================================================
void CShJsonUserPSize::SetFunctionsJsonPath(wchar_t* wzJsonPath)
{
	::wcscat_s(wzJsonPath, MAX_PATH, JSON_FUNCTION_NAMEW_US);
}
// <E> 2022.05.17 sbc R.Tanaka
