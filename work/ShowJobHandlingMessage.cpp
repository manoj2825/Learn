// ============================================================================
// sh_msg_jobhand.cpp
//
//     Copyright 2004 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2005.03.07
// ============================================================================

#include "precomp.h"
#include "sh_api.h"
#include "shjsonstr.h"
//For Logging Purpose
#include "MFPLogger.h"


//=============================================================================
// function
//       CShowJobHandlingMessage
//
// parameters
//      hInst               ÉÇÉWÉÖÅ[ÉãÉnÉìÉhÉã
//      pLog                CShPrnLogÉNÉâÉXÉAÉhÉåÉX
//
// return value
//       ñ≥Çµ
//
// outline
//       CShowJobHandlingMessage ÉNÉâÉXÉRÉìÉXÉgÉâÉNÉ^
//=============================================================================
CShowJobHandlingMessage::CShowJobHandlingMessage(HWND hWnd, HMODULE hStringResourceHandle)
			: CShowMessage( hWnd, hStringResourceHandle)
{
}


//=============================================================================
// function
//       ~CShowJobHandlingMessage
//
// parameters
//       ñ≥Çµ
//
// return value
//       ñ≥Çµ
//
// outline
//       CShowJobHandlingMessage ÉNÉâÉXÉfÉXÉgÉâÉNÉ^
//=============================================================================
CShowJobHandlingMessage::~CShowJobHandlingMessage()
{
}


//=============================================================================
// function
//      Message
//
// parameters
//       hWnd               ÉEÉBÉìÉhÉEÉnÉìÉhÉã
//       pps                PROPSTATEç\ë¢ëÃÇÃÉAÉhÉåÉX
//
// return value
//      ÉGÉâÅ[Ç†ÇË : ëŒâûÇ∑ÇÈÉtÉâÉO
//      ÉGÉâÅ[Ç»Çµ : 0
//
// outline
//      ê›íËÇ…ïsë´Ç™Ç»Ç¢Ç©ämîFÇµÅAÉÅÉbÉZÅ[ÉWÇÃï\é¶ÇçsÇ§
//=============================================================================
short CShowJobHandlingMessage::Message(HWND hWnd, PPROPSTATE pps,PFEATUREINFOLIST pFeatureInfoList)
{
	WCHAR			szUName[(JC_STR_JUNAME + 1) * 2];	// user name
	WCHAR			szJName[(JC_STR_JOBNAME + 1) * 2];	// job name
	WCHAR			szLName[(JC_STR_LOGINNAME + 1) * 2];// login name
	WCHAR			szLPass[JC_STR_LOGINPASS + 1];	// login password

	WORD			wDisp = 0;
	size_t sInputStringLength = 0;
	if(nullptr == pps)
		return wDisp;

//

	wcscpy_s(szUName, _countof(szUName), pps->szUserName);
	wcscpy_s(szJName, _countof(szJName), pps->szJobName);
	wcscpy_s(szLName, _countof(szLName), pps->szLoginNameUA);
	//wcscpy_s(szLPass, _countof(szLPass), pps->szLoginPasswordUA);
	::lstrcpy(szLPass, (*pps).szLoginPasswordUA);

	// ÉXÉyÅ[ÉXÇÃÇ›ÇÃèÍçáÇÕñ≥å¯
	if(CheckSpaceOnly(szUName))
		SecureZeroMemory(szUName, sizeof(szUName));

	if(CheckSpaceOnly(szJName))
		SecureZeroMemory(szJName, sizeof(szJName));

	if(CheckSpaceOnly(szLName))
		SecureZeroMemory(szLName, sizeof(szLName));

	if(CheckSpaceOnly(szLPass))
		SecureZeroMemory(szLPass, sizeof(szLPass));

	if(!strcmp(pFeatureInfoList->pFeatureOptionPair[FEATURE_DEFAULT_JOB_USER_NAME].pszOption,"ON"))
	{
		if(0 == wcsnlen_s(szUName,JC_STR_JUNAME))
		{
			wDisp |= JC_USERNAME;
		}
	}
	if(!strcmp(pFeatureInfoList->pFeatureOptionPair[FEATURE_DEFAULT_JOB_JOB_NAME].pszOption,"ON"))
	{
		if(0 == wcsnlen_s(szJName,JC_STR_JOBNAME))
		{
			wDisp |= JC_JOBNAME;
		}
	}
	//<S><C>Modified the Condtion To Fix Bug #4056,2021.28.07,SSDI:Chanchal Singla
	//Information message displays on Clicking OK when Password text box is left blank in Job - Handling tab
	if(!strcmp(pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption, "LoginName")
		|| !strcmp(pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption, "LoginPass"))
	{
		if(0 == wcsnlen_s(szLName,JC_STR_LOGINNAME))
		{
			wDisp |= JC_LOGINNAME;
		}
	}
	
	//if (!strcmp(pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption, "LoginPass"))
	//{
	//	if (0 == wcsnlen_s(szLPass, JC_STR_LOGINPASS))
	//	{
	//		wDisp |= JC_LOGINPASS;
	//	}
	//}
	//<E>Modified the Condtion To Fix Bug #4056,2021.28.07,SSDI:Chanchal Singla
	if(!strcmp(pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption,"UserNumber"))
	{
		//Fixed Coverity issue - 90450 - SSDI:Seetharam-20200804
		//sInputStringLength = wcsnlen_s(pps->szUserNumber,JC_STR_ACCOUNTNUM);
		sInputStringLength = wcsnlen_s(pps->szUserNumber, JC_STR_PINLENGTH+1);
		if(5 > sInputStringLength)
		{
			wDisp |= JC_ACCOUNTNUM;
		}
		
	}
	if(!strcmp(pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_PASSWORD].pszOption,"ON"))
	{
		sInputStringLength = wcsnlen_s(pps->szRetentionPassword,JC_STR_PINLENGTH);
		if((0 < sInputStringLength) &&(5 > sInputStringLength))
		{
			wDisp |= JC_PASSCODE;
		}
		
	}


	if(wDisp != 0)
	{
		display(hWnd, wDisp);
	}
	else
	{
		wcscpy_s(pps->szUserName, _countof(pps->szUserName), szUName);
		wcscpy_s(pps->szJobName, _countof(pps->szJobName), szJName);
		wcscpy_s(pps->szLoginNameUA, _countof(pps->szLoginNameUA), szLName);
		//wcscpy_s(pps->szLoginPasswordUA,_countof(pps->szLoginPasswordUA), szLPass);
		::lstrcpy((*pps).szLoginPasswordUA, szLPass);
	}

	return wDisp;
}

short CShowJobHandlingMessage::Message(HWND hWnd, PPROPSTATE pps,const DWORD dwFalg)
{
	WCHAR			szUName[(JC_STR_JUNAME + 1) * 2];	// user name
	WCHAR			szJName[(JC_STR_JOBNAME + 1) * 2];	// job name
	WCHAR			szLName[(JC_STR_LOGINNAME + 1) * 2];// login name
	WCHAR			szLPass[JC_STR_LOGINPASS + 1];	// login password

	WORD			wDisp = 0;
	size_t sInputStringLength = 0;
	if(nullptr == pps)
		return wDisp;

//

	wcscpy_s(szUName, _countof(szUName), pps->szUserName);
	wcscpy_s(szJName, _countof(szJName), pps->szJobName);
	wcscpy_s(szLName, _countof(szLName), pps->szLoginNameUA);
	//wcscpy_s(szLPass, _countof(szLPass), pps->szLoginPasswordUA);
	::lstrcpy(szLPass, (*pps).szLoginPasswordUA);

	// ÉXÉyÅ[ÉXÇÃÇ›ÇÃèÍçáÇÕñ≥å¯
	if(CheckSpaceOnly(szUName))
		SecureZeroMemory(szUName, sizeof(szUName));

	if(CheckSpaceOnly(szJName))
		SecureZeroMemory(szJName, sizeof(szJName));

	if(CheckSpaceOnly(szLName))
		SecureZeroMemory(szLName, sizeof(szLName));

	if(CheckSpaceOnly(szLPass))
		SecureZeroMemory(szLPass, sizeof(szLPass));

	if(dwFalg & DM_ALWAYSUSE_ACNUM)
	{
		//Fixed Coverity issue - 90416 - SSDI:Seetharam-20200804
		//if(0 == wcsnlen_s(pps->szUserNumber,JC_STR_JUNAME))
		if (0 == wcsnlen_s(pps->szUserNumber, JC_STR_PINLENGTH))
		{
			wDisp |= JC_ACCOUNTNUM;
		}
	}
	if(dwFalg & DM_USE_PIN)
	{
		sInputStringLength = wcsnlen_s(pps->szRetentionPassword,JC_STR_PINLENGTH);
		if(5 > sInputStringLength)
		{
			wDisp |= JC_PASSCODE;
		}
		
	}
	if(dwFalg & DM_USE_LOGINNAME)
	{
		if(0 == wcsnlen_s(szLName,JC_STR_LOGINNAME))
		{
			wDisp |= JC_LOGINNAME;
		}
	}
	if(dwFalg & DM_USE_LOGINPASS)
	{
		if(0 == wcsnlen_s(szLPass,JC_STR_LOGINPASS))
		{
			wDisp |= JC_LOGINPASS;
		}
	}
	if(0 == wcsnlen_s(szUName,JC_STR_JUNAME))
	{
		wDisp |= JC_USERNAME;
	}
	if(0 == wcsnlen_s(szJName,JC_STR_JOBNAME))
	{
		wDisp |= JC_JOBNAME;
	}
	if(wDisp != 0)
	{
		display(hWnd, wDisp);
	}
	else
	{
		wcscpy_s(pps->szUserName, _countof(pps->szUserName), szUName);
		wcscpy_s(pps->szJobName, _countof(pps->szJobName), szJName);
		wcscpy_s(pps->szLoginNameUA, _countof(pps->szLoginNameUA), szLName);
		//wcscpy_s(pps->szLoginPasswordUA,_countof(pps->szLoginPasswordUA), szLPass);
		::lstrcpy((*pps).szLoginPasswordUA, szLPass);
	}

	return wDisp;
}
//<S><C> to fix Coverity issue 90491, Harika 20201214
short CShowJobHandlingMessage::Message(HWND hWnd, const JOBCONTROL &jobcontrol)
//<E> to fix Coverity issue 90491, Harika 20201214
{
	short wDisp = 0 ;
	if(jobcontrol.wFieldJC & DM_USE_PIN)
	{
		if(0 == strnlen_s((const char *)jobcontrol.byRetentionPassword,RETENTION_PASSWORD_LENGTH))
		{
			wDisp |= JC_PASSCODE;
		}
		
	}
	if(jobcontrol.wFieldJC & DM_USE_LOGINPASS)
	{
		if(0 == strnlen_s((const char *)jobcontrol.byLoginPassword,LOGIN_PASSWORD_LENGTH))
		{
			wDisp |= JC_LOGINPASS;
		}
		
	}
	if(wDisp != 0)
	{
		display(hWnd, wDisp);
	}
	return wDisp;

}
//=============================================================================
// function
//      display
//
// parameters
//      ñ≥Çµ
//
// return value
//      ÉGÉâÅ[Ç†ÇË : TRUE
//      ÉGÉâÅ[Ç»Çµ : FALSE
//
// outline
//      ÉÅÉbÉZÅ[ÉWÇÃï\é¶ÇçsÇ§
//=============================================================================
void CShowJobHandlingMessage::display(HWND hWnd, WORD wField)
{
	short			wCount = 0;

	WCHAR			szMess[512];
	WCHAR			szTemp[512];
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");

	if(hWnd == NULL)
	{
		slog.putLog("CShowJobHandlingMessage::display -> NULL_HANDLE(hWnd)\n");
		return ;
	}

	SecureZeroMemory(szMess, sizeof(szMess));

	if(wField & JC_USERNAME)
	{
		SecureZeroMemory(szTemp, sizeof(szTemp));
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_12, szTemp, countof(szTemp)) > 0)
		{
		
			StringCat(szMess, szTemp);
			StringCat(szMess, L"\n");
		}
	
		wCount++;
	}

	if(wField & JC_JOBNAME)
	{
		SecureZeroMemory(szTemp, sizeof(szTemp));
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_13, szTemp, countof(szTemp)) > 0)
		{
			StringCat(szMess, szTemp);
			StringCat(szMess, L"\n");
		}
		
		wCount++;
	}

	if(wField & JC_LOGINNAME)
	{
		SecureZeroMemory(szTemp, sizeof(szTemp));
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_14, szTemp, countof(szTemp)) > 0)
		{
			StringCat(szMess, szTemp);
			StringCat(szMess, L"\n");
		}
		wCount++;
	}

	if(wField & JC_LOGINPASS)
	{
		SecureZeroMemory(szTemp, sizeof(szTemp));
		//<S><C>To fix Redmine bug #3668- SSDI-Goutham :20210713
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_15, szTemp, countof(szTemp)) > 0)
		//<S><E>To fix Redmine bug #3668- SSDI-Goutham :20210713
		{
			StringCat(szMess, szTemp);
			StringCat(szMess, L"\n");
		}
		wCount++;
	}

	if(wField & JC_ACCOUNTNUM)
	{
		SecureZeroMemory(szTemp, sizeof(szTemp));
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_6, szTemp, countof(szTemp)) > 0)
		{
			StringCat(szMess, szTemp);
			StringCat(szMess, L"\n");
		}
		wCount++;
	}
// << 2009.11.24 SBC:S.Kawabata

	if(wField & JC_PASSCODE)
	{
		SecureZeroMemory(szTemp, sizeof(szTemp));
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_16, szTemp, countof(szTemp)) > 0)
		{
			StringCat(szMess, szTemp);
			StringCat(szMess, L"\n");
		}
	
		wCount++;
	}
	//<S><A> To Add the Support of Stored to Custom Folder,2020-09-09,SSDI: Chanchal Singla
	if (wField & JC_FOLDERPASS)
	{
		SecureZeroMemory(szTemp, sizeof(szTemp));
		if (LoadString(m_hStringResourceHandle, IDS_MESSAGE_17, szTemp, countof(szTemp)) > 0)
		{
			StringCat(szMess, szTemp);
			StringCat(szMess, L"\n");
		}
		
		wCount++;
	}
	//<E> To Add the Support of Stored to Custom Folder,2020-09-09,SSDI: Chanchal Singla
	if(wCount > 0)
	{
		SecureZeroMemory(szTemp, sizeof(szTemp));
		if(wCount == 1)
			LoadString(m_hStringResourceHandle, IDS_MESSAGE_10, szTemp, countof(szTemp)) ;
			
		else
			LoadString(m_hStringResourceHandle, IDS_MESSAGE_11, szTemp, countof(szTemp)) ;
			
		StringCat(szTemp, L"\n");
		StringCat(szTemp, szMess);
		
		SecureZeroMemory(szMess, sizeof(szMess));
		memcpy(szMess, szTemp, sizeof(szMess));
		
		__super::Message(hWnd, IDS_MESTITLE_1, szMess, MB_OK);
	}



	return;
}

//<S><A> To Add the Support of Stored to Custom Folder,2020-09-09,SSDI: Chanchal Singla
//=============================================================================
// function
//      Message
//
// parameters
//       hWnd              Handler To Window
//       pps                PROPSTATEç
//
// return value
//      SHORT
//
// outline
//     Message For Folder Password
//=============================================================================
short CShowJobHandlingMessage::message(HWND hWnd, PPROPSTATE pps)
{
	WORD			wDisp = 0;

	if (pps == NULL)
		goto EXIT;

	if (0 < ::lstrlen((*pps).szFolderPass) && 5 > ::lstrlen((*pps).szFolderPass))
		wDisp |= JC_FOLDERPASS;

	if (wDisp != 0)
	{
		display(hWnd, wDisp);
	}

EXIT:

	return wDisp;
}
//<E> To Add the Support of Stored to Custom Folder,2020-09-09,SSDI: Chanchal Singla
//= ============================================================================
// function
//      Message
//
// parameters
//       hWnd               handle to windowã
//       pps                PROPSTATE
//
// return value
//      SHORT
//
// outline
//      êMessageForMainTab
//=============================================================================
short CShowJobHandlingMessage::MessageForMainTab(HWND hWnd, PPROPSTATE pps)
{
	
	long			lMin = 5;
	long			lMax = JC_STR_PINLENGTH;
	

	WORD			wDisp = 0;

	if (pps == NULL)
		goto EXIT;

	if (0 < ::lstrlen((*pps).szRetentionPassword) && lMin > ::lstrlen((*pps).szRetentionPassword)) {
		wDisp |= JC_PASSCODE;
	}
	

	if (wDisp != 0)
	{
		display(hWnd, wDisp);
	}

EXIT:

	return wDisp;
}
//=============================================================================
// function
//      MessageJobHandlingDlg
//
// parameters
//       hWnd               Handle to window
//       pps                PROPSTATEç
//
// return value
//    SHORT
//
// outline
//      MessageJobHandlingDlg
//=============================================================================
short CShowJobHandlingMessage::MessageJobHandlingDlg(HWND hWnd, PPROPSTATE pps, WCHAR* pszSvrPrnName, BOOL blDlg)
{

	WORD			wDisp = MessageEX(hWnd, pps, pszSvrPrnName, blDlg);

	if (wDisp != 0)
	{
		display(hWnd, wDisp);
	}
	return wDisp;
}
//=============================================================================
// function
//      MessageEX
//
// parameters
//       hWnd              Handle to window
//       pps                PROPSTATEç
//
// return value
//      SHORT
//
// outline
//      MessageEX
//=============================================================================
short CShowJobHandlingMessage::MessageEX(HWND hWnd, PPROPSTATE pps,WCHAR* pszSvrPrnName, BOOL blDlg)
{

	long			lMin = 5;
	long			lMax = JC_STR_PINLENGTH;


	WCHAR			szUName[(JC_STR_JUNAME + 1) * 2];	// user name
	WCHAR			szJName[(JC_STR_JOBNAME + 1) * 2];	// job name
	WCHAR			szLName[(JC_STR_LOGINNAME + 1) * 2];// login name
	WCHAR			szLPass[JC_STR_LOGINPASS + 1];	// login password
	WCHAR			szTemp[512];

	WORD			wDisp = 0;

	BOOL			blHasPin = FALSE;

	CShRegStored	*preg = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, pszSvrPrnName, szCommonDatFilePath, FALSE);
	CShJsonStored	*pjsonstr = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonstr = new CShJsonStored(ghInstance, pszSvrPrnName, m_hStringResourceHandle);
		pjsonstr->Init();
	}

	preg = new CShRegStored(m_hStringResourceHandle, pszSvrPrnName);
	if (preg == NULL)
		goto EXIT;
	if (pps == NULL)
		goto EXIT;


	wcscpy_s(szUName, _countof(szUName), pps->szUserName);
	wcscpy_s(szJName, _countof(szJName), pps->szJobName);
	wcscpy_s(szLName, _countof(szLName), pps->szLoginNameUA);
	wcscpy_s(szLPass, _countof(szLPass), pps->szLoginPasswordUA);

	if (CheckSpaceOnly(szUName))
		SecureZeroMemory(szUName, sizeof(szUName));

	if (CheckSpaceOnly(szJName))
		SecureZeroMemory(szJName, sizeof(szJName));

	if (CheckSpaceOnly(szLName))
		SecureZeroMemory(szLName, sizeof(szLName));

	if (CheckSpaceOnly(szLPass))
		SecureZeroMemory(szLPass, sizeof(szLPass));


	if ((*pps).wUseAcNum == BST_CHECKED && ::lstrlen((*pps).szUserNumber) == 0)
		wDisp |= JC_ACCOUNTNUM;

	if ((*pps).wUsePIN == BST_CHECKED && ::lstrlen((*pps).szRetentionPassword) == 0)
		wDisp |= JC_PASSCODE;

	if ((*pps).wPPlcyLoginName != bool_true)
	{
		if (blDlg != TRUE)
		{
			if ((*pps).wUseUName == BST_CHECKED && ::wcslen((const wchar_t *)szUName) == 0)
				wDisp |= JC_USERNAME;
		}
		else
		{
			if (::wcslen((const wchar_t *)szUName) == 0)
			{
				wDisp |= JC_USERNAME;
			}
		}
	}

	if (blDlg != TRUE)
	{
		if ((*pps).wUseJName == BST_CHECKED && ::wcslen((const wchar_t *)szJName) == 0)
			wDisp |= JC_JOBNAME;
	}
	else
	{
		if (::wcslen((const wchar_t *)szJName) == 0)
		{
			wDisp |= JC_JOBNAME;
		}
	}

	if ((*pps).wLoginName == BST_CHECKED && ::wcslen((const wchar_t *)szLName) == 0)
		wDisp |= JC_LOGINNAME;

	if ((*pps).wPPlcyUserAuth == bool_false) {
		if ((*pps).wLoginPass == BST_CHECKED && ::lstrlen(szLPass) == 0) {
			wDisp |= JC_LOGINPASS;
		}
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		if (blDlg != TRUE)
		{
			if (pjsonstr != NULL)
			{
				(*pjsonstr).ReadStrdData((*pps).wDocFileFolder, szTemp, sizeof(szTemp), &blHasPin);
				if (blHasPin == TRUE)
				{
					if ((*pjsonstr).ReadStrdPin(szTemp, sizeof(szTemp)) == FALSE)
						wDisp |= JC_FOLDERPASS;
				}
			}
		}
	}
	else
	{
		if (blDlg != TRUE)
		{
			if (preg != NULL)
			{
				(*preg).ReadStrdData(pszSvrPrnName, (*pps).wDocFileFolder, szTemp, sizeof(szTemp), &blHasPin);
				if (blHasPin == TRUE)
				{
					if ((*preg).ReadStrdPin(pszSvrPrnName, szTemp, sizeof(szTemp)) == FALSE)
						wDisp |= JC_FOLDERPASS;
				}
			}
		}
	}


	if (0 < ::lstrlen((*pps).szUserNumber) && 5 > ::lstrlen((*pps).szUserNumber))
		wDisp |= JC_ACCOUNTNUM;


	if (0 < ::lstrlen((*pps).szRetentionPassword) && lMin > ::lstrlen((*pps).szRetentionPassword)) {
		wDisp |= JC_PASSCODE;
	}


	if (blDlg != TRUE)
	{
		if (0 < ::lstrlen((*pps).szFolderPass) && 5 > ::lstrlen((*pps).szFolderPass))
			wDisp |= JC_FOLDERPASS;
	}

	if (wDisp == 0)
	{
		wcscpy_s(pps->szUserName, _countof(pps->szUserName), szUName);
		wcscpy_s(pps->szJobName, _countof(pps->szJobName), szJName);
		wcscpy_s(pps->szLoginNameUA, _countof(pps->szLoginNameUA), szLName);
		wcscpy_s(pps->szLoginPasswordUA, _countof(pps->szLoginPasswordUA), szLPass);
	}

EXIT:
	if(preg != NULL)
	{
		delete preg;
		preg = NULL;
	}
	if (pjsonstr != NULL) {
		delete pjsonstr;
		pjsonstr = NULL;
	}
	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = NULL;
	}
	return wDisp;
}
