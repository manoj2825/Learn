//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : UtilityFunctions.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 18 June 2013
//
//	Description   : Helper function definition for APIs GetPrinterDriverPath name etc.

//===========================================================================

//===========================================================================
//  Reviews 
//
//  Sl#     Review Date     Reviewer(s)               Remarks
//===========================================================================
#pragma once
#include <WINDOWS.H>
#include <winspool.h>
#include "define.h"
#include <Shlwapi.h>
#include "sh_api.h"
#include "md5.h"
#include <ShlObj.h>
#include "UtilityFunctions.h"

#pragma comment(lib, "Shlwapi.lib")  // Link against the Shlwapi.lib library
#pragma comment(lib, "Shell32.lib")
// To support Pamphlet feature
// These definitions are taken from Aries UI code
//=============================================================================
// function
//      GetPamphSize
//
// parameters
//     Input:
//		  wSrcPaper - Source paper size
//        wPamphMode - Variable that represents pamphlet mode (2up Pamphlet  or tiled pamphlet
//		  blHWPamph - Boolean value that represents whether Hardware Pamphlet is selected or not.
//        ptable          - Pointer to an array of Pamphlet table
//              
// return value
//         short        
// outline
//    This function gets pamphlet paper size
//=============================================================================
short GetPamphSize(short wSrcPaper, short wPamphMode, BOOL blHWPamph, BOOL blRealSize, const short FAR *ptable)
{
	short			wRet = wSrcPaper;

	short			wPos = 0;
	short			wPosSrc = 0;
	short			wPosDst = 0;
	
	const short FAR	*pw = NULL;

	if(ptable != NULL)
		pw = ptable;
	else
		pw = pamph_psize;

	if(pw != NULL)
	{
		switch(wPamphMode)
		{
			case pamph_2up:
			case pamph_2up_g:

				if(blHWPamph == FALSE && blRealSize == FALSE)
					return wRet;

				wPosSrc = 1;
				wPosDst = 0;

				break;

			case pamph_tiled:
			case pamph_tiled_g:
			default:

				if(blHWPamph != FALSE && blRealSize == FALSE)
					return wRet;

				wPosSrc = 0;
				wPosDst = 1;
				break;
		}

		wPos = 0;
		while(!(pw[wPos] == -1 && pw[wPos + 1] == -1))
		{
			if(pw[wPos + wPosSrc] == wSrcPaper)
			{
				wRet = pw[wPos + wPosDst];
				break;
			}

//@CNG 2005.04.14 H.Maegawa ->
//			wPos++;
			wPos+=2;
//@CNG 2005.04.14 H.Maegawa <-

		}
	}

	return wRet;
}

PaperPixelInformationMap InitializePaperPixelInfoMap(void)
{
	PaperPixelInformationMap paperinfomap;
	PAPERPIXELINFORMATION paperpixelinfo;
	DWORD dwPaperPixelInfoMapSize = _countof(PaperPixelInformationArray);
	for(DWORD dwCount = 0 ; dwCount < dwPaperPixelInfoMapSize ; dwCount++)
	{
		paperpixelinfo.dwWidthPixel = PaperPixelInformationArray[dwCount][1];
		paperpixelinfo.dwHeightPixel = PaperPixelInformationArray[dwCount][2];
		paperinfomap[PaperPixelInformationArray[dwCount][0]]=paperpixelinfo;
	}
	return paperinfomap;
}
BOOL WINAPI GetPureName(LPTSTR	lpCmnDModeDeviceName,LPTSTR	lpPrinterName,INT		nCharSize,INT		nMode)
{
   
    BOOL bResult = FALSE;

    ::ZeroMemory(lpCmnDModeDeviceName,nCharSize*sizeof(WCHAR));
    LPTSTR	lpTop;
    if(RTPR_PURENAME == nMode)
    {
        lpTop = wcsrchr(lpPrinterName,SMKEY('\\'));
        if(NULL == lpTop)
        {
            lpTop = lpPrinterName ;
        }
        else
        {
            lpTop ++ ;
        }
    }
    else 
    {		
        lpTop = lpPrinterName ;
    }

    for(int i = 0;i < nCharSize;i++)
    {
        BOOL bEndMark = FALSE ;

        if(SMKEY('\0') == lpTop[i])	bEndMark = TRUE ;
        if(SMKEY(',' ) == lpTop[i])	bEndMark = TRUE ;

        if(bEndMark) 
        {
            lpCmnDModeDeviceName[i] = SMKEY('\0');
            break ;
        }
        lpCmnDModeDeviceName[i] = lpTop[i] ;
    }
    lpCmnDModeDeviceName[nCharSize-1] = SMKEY('\0');

    return bResult ;
}
BOOL GetPrinterNameFromHandle(WCHAR *pPrinterName,HANDLE hPrinter)
{
	
	PPRINTER_INFO_2 pPrinter = NULL;
	DWORD dwNeeded = 0;

	BOOL bResult = GetPrinter(hPrinter, 2, NULL, 0, &dwNeeded);
	if ( ( bResult == FALSE ) && 
		 ( GetLastError() == ERROR_INSUFFICIENT_BUFFER ) &&
		 ( dwNeeded > 0 ) )
	{
		pPrinter = (PPRINTER_INFO_2) new BYTE[dwNeeded];

		if ( pPrinter == NULL )
		{
			return FALSE;
		}

		bResult = GetPrinter(hPrinter, 2, (LPBYTE)pPrinter, dwNeeded, &dwNeeded);
		if ( bResult == FALSE )
		{
			if(pPrinter)
			{
				delete [] pPrinter;
				pPrinter = NULL;
			}
			return FALSE;
		}

		WCHAR *pTemp = pPrinter->pPrinterName;
		
		wcscpy_s(pPrinterName,wcslen(pTemp)+1,pTemp);

		if(pPrinter)
		{
			delete [] pPrinter;
			pPrinter = NULL;
		}

	}
	return TRUE;
}

WCHAR* setstr(WCHAR *pszModelName, WCHAR *pszPortName)
{
    WCHAR szForm[MAX_BUFFER] = {};
    wcscpy_s(szForm,L"%s(%s)");

	WCHAR		*pstrW = NULL;
	
	if(pszModelName == NULL || pszPortName == NULL)
		return pstrW;
	
	WCHAR pIPAddress[SCUI_MAX_PORTNAME + 1] = {};
    StringCopy(pIPAddress,(LPCWSTR)pszPortName);

	DWORD dwLen = wcslen((LPCWSTR)pszModelName) + wcslen((LPCWSTR)pIPAddress) + 1 + 1 + 1;
	
	pstrW = new WCHAR[dwLen]();	
	if(NULL == pstrW)
    {
        return pstrW;
    }

	StringCbPrintf((LPWSTR)pstrW,dwLen*sizeof(WCHAR), (LPCWSTR)szForm,	pszModelName, pIPAddress);

	return pstrW;
}

BOOL CheckSpaceOnly(WCHAR *pwString)
{
	if(NULL == pwString)
	{
		return TRUE;
	}
	std::wstring wstrInputString = pwString;
	std::wstring WhiteSpaces = L"\x20 \u3000";
	return (std::wstring::npos == wstrInputString.find_first_not_of(WhiteSpaces));
}
//<S><A> Redmine #486, 2013.02.12, SSDI:Purna
//Japanese Language Support - To validate the Password field data.
//=============================================================================
// function
//		RemoveDBCSChar
//
// parameters
//		*lpWChar - Pointer to the input string.
//
// return value
//      TRUE - if input string has Multi-Byte characters.
//		FALSE - if input string has all Single-Byte characters.
//
// outline
//      Function validates and removes all the Multi-Byte characters.
//=============================================================================
BOOL RemoveDBCSChar(WCHAR *lpWChar,size_t CharSize)
{
	BOOL	blModify		= FALSE;
	unsigned int uiValue	= 0;

	WCHAR	szTemp[512] = {0};
	WCHAR	*lpTemp		= NULL;
	WCHAR	*lpCur		= NULL;


	lpTemp = szTemp;

	for(lpCur = lpWChar; *lpCur != L'\0'; lpCur++)
	{
		//Get the Numeric value for each character.
		//<S><C> Fix for SQA issue #3880, Harika 20210805
		/*uiValue = (unsigned int)*lpCur;
		if(0x000000FF < uiValue)
		{
			blModify = TRUE;
			continue;
		}*/
		if (::IsDBCSLeadByte((BYTE)(*lpCur)) == TRUE)
		{
			lpCur++;
			blModify = TRUE;
			continue;
		}
		//<E> Fix for SQA issue #3880, Harika 20210805
		*lpTemp = *lpCur;
		lpTemp++;
	}

	if(blModify == TRUE)
		StringCbCopy(lpWChar,CharSize *sizeof(WCHAR),szTemp);

	return blModify;
}
//<S><A>To Fix Bug #4512,2021.December.12,SSDI:Chanchal Singla
//	Job - Handling tab : Password text box is not accepting correct data
BOOL RemoveDBCSChar(CHAR *lpStr)
{
	CHAR	szTemp[512];
	CHAR	*lpTemp = NULL;
	CHAR	*lpCur = NULL;
	BOOL	blModify = FALSE;

	memset(szTemp, 0x00, sizeof(szTemp));
	lpTemp = szTemp;

	for (lpCur = lpStr; *lpCur != '\0'; lpCur++)
	{
		if (::IsDBCSLeadByte((BYTE)(*lpCur)) == TRUE)
		{
			lpCur++;
			blModify = TRUE;
			continue;
		}

		*lpTemp = *lpCur;
		lpTemp++;
	}

	if (blModify == TRUE)
		lstrcpyA(lpStr, szTemp);

	return blModify;
}
//<E>To Fix Bug #4512,2021.December.12,SSDI:Chanchal Singla
//<E> Redmine #486, 2013.02.12, SSDI:Purna
// <S><A> Added to check space double quote and remove double quotes in UD3L, 20200827 - SSDI:Goutham
//===========================================================================
//	Function:	Å	RemoveDoubleQuoteChar
//	Parameter:	Å	lpStr		ê
//
//	 return:	è TRUE /  FALSE
//
//	 Function validates and removes all the Double quote characters.
//===========================================================================
BOOL RemoveDoubleQuoteChar(char *lpStr)
{
	if (IsUnicodeOS() != FALSE)
		return RemoveDoubleQuoteCharW((LPWSTR)lpStr);
	else
		return RemoveDoubleQuoteCharA(lpStr);
}


//===========================================================================
//	Function:	RemoveDoubleQuoteCharW
//	Parameter:	lpStr 	ê
//
//	return:	è TRUE /  FALSE
//
//	Function validates and removes all the Double quote Wide characters.
//===========================================================================
BOOL RemoveDoubleQuoteCharW(LPWSTR lpStr)
{
	BOOL	blModify = FALSE;
	LPWSTR	lpCur = NULL;
	char	*lpDst;
	char	*pTemp;
	unsigned char	c;
	unsigned char	p;

	
	lpDst = (char*)lpStr;
	for (lpCur = lpStr; ; lpCur++) {
		pTemp = (char*)lpCur;
		c = *(pTemp + 0);
		p = *(pTemp + 1);

		if (c == '\0' && p == '\0') {
			break;
		}

		if (c == '"' && p == '\0') {
			
			blModify = TRUE;
			continue;
		}

		*(lpDst + 0) = c;
		*(lpDst + 1) = p;
		lpDst++;
		lpDst++;
	}

	*(lpDst + 0) = '\0';
	*(lpDst + 1) = '\0';

	return blModify;
}


//===========================================================================
//	Function:	RemoveDoubleQuoteCharA
//	Parameter:	lpStr 	ê
//
//	return:	è TRUE /  FALSE
//
//	Function validates and removes all the Double quote Wide characters.
//===========================================================================
BOOL RemoveDoubleQuoteCharA(char *lpStr)
{
	BOOL	blModify = FALSE;
	char	*lpCur = NULL;
	char	*lpDst;

	
	lpDst = lpStr;
	for (lpCur = lpStr; *lpCur != '\0'; lpCur++) {
		if (*lpCur == '"') {
			
			blModify = TRUE;
			continue;
		}

		*lpDst = *lpCur;
		lpDst++;
	}

	return blModify;
}

//<S><C>To fix Redmine bug #2737 issue 1,2020.11.11 SSDI:Goutham
//===========================================================================
//	Function:	CheckSpaceDoubleQuoteOnly
//	Parameter:	pszData		
//				lSize			
//
//	return: TRUE / FALSE
//Function validates and Checks space and all the Double quote characters.
//===========================================================================
BOOL CheckSpaceDoubleQuoteOnly(char *pszData, long lSize)
{
	if (IsUnicodeOS() != FALSE)
		return CheckSpaceDoubleQuoteOnlyW((LPWSTR)pszData, lSize);//64 bits system
	else
		return CheckSpaceDoubleQuoteOnlyA(pszData, lSize);//32 bits systems
}
//<S><E>To fix Redmine bug #2737 issue 1,2020.11.11 SSDI:Goutham

//===========================================================================
//	Function:	CheckSpaceDoubleQuoteOnlyW
//	Parameter:	pszData		
//				lSize			
//
//	return: TRUE / FALSE
//Function validates and Checks space and all the Double quote Wide characters.
//===========================================================================
BOOL CheckSpaceDoubleQuoteOnlyW(LPWSTR pszData, long lSize)
{
	BOOL	blRet = TRUE;

	long	lPos;

	
	//	char	szTemp[1024];
	long	lMax = 0;
	char	*pTemp = NULL;
	
	unsigned char	c;
	unsigned char	p;

	
	//	memset(szTemp, 0x00, sizeof(szTemp));
	//	memcpy(szTemp, pszData, sizeof(szTemp));
	pTemp = (char *)pszData;
	

	
	//	c = szTemp[0];
	//	p = szTemp[1];
	c = *(pTemp + 0);
	p = *(pTemp + 1);
	

	lPos = 0;

	
	lMax = lSize * 2;
	
	while (!(c == '\0' && p == '\0'))
	{
		
		if (lPos >= lMax) {
			break;
		}
		
		switch (c)
		{
		case ' ':	 
		case '"':	
			if (p == 0x00)
				break;
			else
				return FALSE;

		case 0x00:	
			if (p == 0x30)
				break;
			else
				return FALSE;

		case 0x1d:	
			if (p == 0x20)
				break;
			else
				return FALSE;

		default:
			return FALSE;
		}

		lPos += 2;
		
		//		c = szTemp[lPos];
		//		p = szTemp[lPos + 1];
		c = *(pTemp + lPos + 0);
		p = *(pTemp + lPos + 1);
		
	}

	return blRet;

}


//===========================================================================
//	Function:	CheckSpaceDoubleQuoteOnlyA
//	Parameter:	pszData		
//				lSize			
//
//	return: TRUE / FALSE
//Function validates and Checks space and all the Double quote Wide characters.
//===========================================================================
BOOL CheckSpaceDoubleQuoteOnlyA(char *pszData, long lSize)
{
	BOOL	blRet = TRUE;

	long	lPos;

	
	//	char	szTemp[1024];
	long	lMax = 0;
	char	*pTemp = NULL;
	
	unsigned char	c;

	
	//	memset(szTemp, 0x00, sizeof(szTemp));
	//	lstrcpy(szTemp, pszData);
	pTemp = pszData;
	

	
	//	c = szTemp[0];
	c = *(pTemp + 0);
	
	lPos = 0;

	
	lMax = lSize;
	
	while (c != '\0')
	{
		
		if (lPos >= lMax) {
			break;
		}
		
		switch (c)
		{
		case ' ':	
		case '"':	
			break;

		case 0x81:	
			lPos++;
			
			//				c = szTemp[lPos];
			c = *(pTemp + lPos);
			

			switch (c)
			{
			case 0x40:	
				break;

			case 0x68:	
				break;

			default:
				return FALSE;
			}

			break;

		default:
			return FALSE;
		}

		lPos++;
		
		//		c = szTemp[lPos];
		c = *(pTemp + lPos);
		
	}

	return blRet;

}
// <S><E> Added to check space double quote and remove double quote in UD3L, 20200827 - SSDI:Goutham
//=============================================================================
// function
//		Cutstrlen
//
// parameters
//		WCHAR* pszData, long lSize, WORD wPoint
//
// return value
//      WORD
//
// outline
//  Treats unicode characters as 2 bytes .
//=============================================================================
WORD Cutstrlen(WCHAR* pszData, long lSize, WORD wPoint)
{
	WORD	wPos = 0;
	WORD	wDelStr = 0;
	WORD	wDelPoint = wPoint;

	WCHAR	szTemp[1024] = {0};
	
	if(wDelPoint == 0 || wcslen(pszData) < wDelPoint)
	{
		wDelPoint = (WORD)wcslen(pszData);
	}

	wPos = Getstrlen(szTemp, pszData, lSize, wDelPoint);

	if(wPos != 0)
	{
		wDelStr = (WORD)wcslen(pszData) - (WORD)wcslen(szTemp);
		::memset(pszData, 0x00, (size_t)wPos+2);
		::memcpy(pszData, szTemp, (size_t)wPos);
	}

	if(wPoint - wDelStr < 0)
		wDelStr = 0;

	return wDelStr;
}
//=============================================================================
// function
//		Getstrlen
//
// parameters
//		LPWSTR pszOutData, LPWSTR pszInData, long lSize, WORD wPoint
//
// return value
//      WORD
//
// outline
//  Get string length in bytes.
//=============================================================================
WORD Getstrlen(LPWSTR pszOutData, LPWSTR pszInData, long lSize, WORD wPoint)
{
	BOOL	blchange = 0;
	//DWORD	blRet = FALSE;

	WORD	wPos = 0;
	long	lStrlen = 0;
	short	i = 0;
	WORD	wDelWord = 0;

	char	szTemp[1024] = {0};
	unsigned char	c = 0;
	unsigned char	p = 0;
	memcpy_s(szTemp, sizeof(szTemp),pszInData, sizeof(szTemp));

	wPos = 0;
	c = szTemp[wPos];
	p = szTemp[wPos + 1];

	while(!(c == '\0' && p == '\0'))
	{
		switch(p)
		{
			case 0x00:
				lStrlen++;
				break;

			case 0xff:
				if(c > 0x65)
					lStrlen++;
				else
					lStrlen+=2;
				break;

			default:
				lStrlen+=2;
				break;
		}

		if(lSize < lStrlen)
		{
			if(wPoint == 0 || wcslen((WCHAR*)szTemp) < wPoint)
				wPoint = (WORD)strlen(szTemp);

			//ì¸óÕÇµÇΩç≈å„ÇçÌèúÇµÇ¬ÇﬂÇÈ
			for(i = (wPoint-1)*2 ; !(szTemp[i] == '\0' && szTemp[i + 1] == '\0'); i += 2)
			{
				szTemp[i    ] = szTemp[i     + 2];
				szTemp[i + 1] = szTemp[i + 1 + 2];
			}
			wPos = 0;
			lStrlen = 0;
			wDelWord++;
			c = szTemp[wPos];
			p = szTemp[wPos + 1];
			wPoint--;
			blchange = TRUE;
		}
		else
		{
			wPos+=2;
			c = szTemp[wPos];
			p = szTemp[wPos + 1];
		}
	}

	::memcpy((void *)pszOutData, (void *)szTemp, (size_t)wPos);

	/*if(blchange == TRUE)
		blRet = wPos;
	else
		blRet = FALSE;*/
	if( FALSE == blchange)
		wPos = 0;
	return wPos;


}

short GetUnitDef(void)
{
    WCHAR			szUnit[MAX_SEPARATION] = {};
    if(0 == ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, szUnit, _countof(szUnit)))
			return 0;
	short wRet = (short)::_wtoi(szUnit);

	return wRet;
}

short GetPaperSizeDef(void)
{
	short	wRet = 0;
	
	long	lCountry = 0;

	lCountry = GetCountryNumber();

	if((lCountry == 1) || (lCountry == 2) || (lCountry == 63) ||
	  ((lCountry >= 50) && (lCountry < 60) && (lCountry != 55)) ||
	  ((lCountry >= 500) && (lCountry < 600) && (lCountry != 598)))
		wRet = DMPAPER_LETTER;
	else
		wRet = DMPAPER_A4;

	return wRet;
}

long GetCountryNumber(void)
{
    WCHAR szCountry[MAX_LEN_COUNTRYCODE] = {};
    ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ICOUNTRY, szCountry, _countof(szCountry));
	long lCountry = ::_wtoi(szCountry);
	return lCountry;
}

BOOL GetIPAddress(HANDLE hPrinter,WCHAR* /*pPortName*/,WCHAR *pRegisteredPortName)
{
	BOOL	blRet = FALSE;
	if (NULL == pRegisteredPortName)
	{
		return blRet;
	}
	DWORD	dwNeeded = 0;

    GetPrinter(hPrinter, 2, NULL, 0, &dwNeeded);
    if(0 == dwNeeded)
	{
        return blRet;
	}

    PRINTER_INFO_2	*ppi2 = (PRINTER_INFO_2 *)new BYTE[dwNeeded]();
    if(NULL == ppi2)
	{
        return blRet;
	}

    if(FALSE == GetPrinter(hPrinter, 2, (LPBYTE)ppi2, dwNeeded, &dwNeeded))
    {
        delete[] ppi2;
        ppi2 = NULL;
        return blRet;
    }

	WCHAR szSelectedPortName[SCUI_MAX_PORTNAME] = {};
    wcscpy_s(szSelectedPortName,SCUI_MAX_PORTNAME, ppi2->pPortName);
	WCHAR* pTCPIPPortAddress = GetIPAddressFromXCVInterface(ppi2->pServerName,szSelectedPortName); // Gets TCPIP address
	if (NULL != pTCPIPPortAddress)
	{
        wcscpy_s(szSelectedPortName,SCUI_MAX_PORTNAME, pTCPIPPortAddress);
        delete[] pTCPIPPortAddress;
        pTCPIPPortAddress = NULL;
	}
    if(0 != wcscmp(szSelectedPortName, pRegisteredPortName))
	{
        wcscpy_s(pRegisteredPortName,SCUI_MAX_PORTNAME,szSelectedPortName);
		blRet = TRUE;
    }
    delete[] ppi2;
    ppi2 = NULL;

    return blRet;
}
WCHAR* GetIPAddressFromXCVInterface(LPTSTR lpServerName, WCHAR* lpPortName)
{
    WCHAR* lpIPAddress = NULL; // Holds IP Address
    //String to hold TCPMON Xcv Port format
    HANDLE hXcv = GetXcvHandle(lpServerName,lpPortName);

    if(NULL == hXcv)
	{
        return lpIPAddress;
	}

    // Buffer variable
    DWORD pcbOutputNeeded = 0;  
    // pointer to location to recieve the status value returned by XcvData
    DWORD pdwStatus = 0; 
    // Get the Buffer size needed to hold IP address
    BOOL bRetVal = XcvData(hXcv,L"IPAddress",NULL,0,(PBYTE)lpIPAddress,0,&pcbOutputNeeded,&pdwStatus);
    if (0 == pcbOutputNeeded)
    {
        ::ClosePrinter(hXcv); 
        hXcv = NULL;
        return lpIPAddress;
    }
    lpIPAddress = new WCHAR[pcbOutputNeeded];
    if(NULL == lpIPAddress)
    {
        ::ClosePrinter(hXcv); 
        hXcv = NULL;
        return lpIPAddress;
    }
    bRetVal = XcvData(hXcv,L"IPAddress",NULL,0,(PBYTE)lpIPAddress,pcbOutputNeeded,&pcbOutputNeeded,&pdwStatus);		

    // If IP address retrieval is successful, return IP address
    if (NO_ERROR != pdwStatus) 
    {
        delete [] lpIPAddress ;
        lpIPAddress = NULL ;

        ::ClosePrinter(hXcv); 
        hXcv = NULL;
        return lpIPAddress;
    }	
    ::ClosePrinter(hXcv); 
    hXcv = NULL;
    return lpIPAddress;	
}

HANDLE GetXcvHandle(LPTSTR lpServerName,WCHAR* lpPortName)
{
	WCHAR lpXCvPort [SCUI_PRNNAME_MAX]={};
	// Check whether Local Printer or Network Printer
	if (NULL != lpServerName) 
	{
		// to represent Servername in TCPMON Xcv Port format add '\\'
        wcscat_s(lpXCvPort, lpServerName);
        wcscat_s(lpXCvPort, L"\\");
	}
	wcscat_s(lpXCvPort, L",XcvPort ");
	wcscat_s(lpXCvPort,lpPortName);
	// Handle variable
	HANDLE hXcv = INVALID_HANDLE_VALUE;   

	PRINTER_DEFAULTSW pDefaults = {NULL, NULL, NULL};

	//Get handle for TCPIP port ; using TCPMON Xcv format string instead of Printername
	// Open printer with Xcv string format to get Handle
	::OpenPrinter(lpXCvPort, &hXcv, &pDefaults);
	return hXcv;
}
//=============================================================================
// function
//      ConvertStdSizeToRSize
//
// parameters
//      wSize               
//      ptable            s
//
// return value
//     SHORT
//
// outline
//     ConvertStdSizeToRSize
//=============================================================================
short ConvertStdSizeToRSize(short wSize, const short FAR *ptable)
{
	short			wPaper = wSize;

	short			wPos = 0;

	const short FAR *pw = NULL;

	if (ptable != NULL)
		pw = ptable;
	else
		pw = r_std_psize;
	if (pw != NULL)
	{
		wPos = 1;
		while (pw[wPos] != -1)
		{
			if (pw[wPos] == wSize)
			{
				wPaper = pw[wPos - 1];
				break;
			}

			wPos += 2;
		}
	}

	return wPaper;
}
//<S><A>To Implement Task#3132,15-05-2024,SSDI:Manoj S
//Delete Print Release and Document Filing from Printing Policy
//=============================================================================
// function
//      IsNumber
//		
// parameters
//
// return value
//      TRUE: ëSÇƒêîéö		
//Å@Å@Å@FALSE: êîéöà»äOÇ™ä‹Ç‹ÇÍÇƒÇ¢ÇÈ
//
// outline
//      ï∂éöóÒ(multibyte)Ç™ëSÇƒêîéöÇ≈ç\ê¨Ç≥ÇÍÇƒÇ¢ÇÈÇ©É`ÉFÉbÉNÇ∑ÇÈ
//=============================================================================
BOOL IsNumber(char* const szStr) {
	char *p = szStr;
	while (*p != '\0') {
		if (*p < '0' && *p > '9')
			return FALSE;
		p++;
	}
	return TRUE;
}
//<E>To Implement Task #3132,15-05-2024,SSDI:Manoj S

//<S><A>To Implement Task#3137,25-07-2024,SSDI:Manoj S
//===========================================================================
//	ä÷êî	ÅF	RemoveForbiddenChar
//	à¯êî	ÅF	lpStr (IN/OUT)		êßå¿èàóùëOï∂éöóÒ / êßå¿èàóùå„ï∂éöóÒ
//
//	ï‘íl	ÅF	èúãéÇµÇΩ : TRUE / ä‹Ç‹ÇÍÇ»Ç©Ç¡ÇΩ : FALSE
//
//	if there are forbidden chars, delete them
//===========================================================================
BOOL RemoveForbiddenChar(char *lpStr, BOOL bReplace)
{
	if (IsUnicodeOS() != FALSE)
		return RemoveForbiddenCharW((LPWSTR)lpStr, bReplace);
	else
		return RemoveForbiddenCharA(lpStr, bReplace);
}


//===========================================================================
//	ä÷êî	ÅF	RemoveForbiddenCharW
//	à¯êî	ÅF	lpStr (IN/OUT)		êßå¿èàóùëOï∂éöóÒ / êßå¿èàóùå„ï∂éöóÒ
//
//	ï‘íl	ÅF	èúãéÇµÇΩ : TRUE / ä‹Ç‹ÇÍÇ»Ç©Ç¡ÇΩ : FALSE
//
//	if there are forbidden chars, delete them
//===========================================================================
BOOL RemoveForbiddenCharW(LPWSTR lpStr, BOOL bReplace)
{
	BOOL	blModify = FALSE;
	BOOL	blDelete = FALSE;
	LPWSTR	lpCur = NULL;
	char	*lpDst;
	char	*pTemp;
	unsigned char	c;
	unsigned char	p;
	short	i = 0;

	lpDst = (char*)lpStr;
	for (lpCur = lpStr; ; lpCur++) {
		pTemp = (char*)lpCur;
		c = *(pTemp + 0);
		p = *(pTemp + 1);
		blDelete = FALSE;

		if (c == '\0' && p == '\0') {
			break;
		}

		i = 0;
		while (FORBIDDEN_CHAR[i] != '\0')
		{
			if (c == FORBIDDEN_CHAR[i] && p == '\0')
			{
				blDelete = TRUE;
				blModify = TRUE;
				break;
			}

			i++;
		}

		if (blDelete != TRUE)
		{
			*(lpDst + 0) = c;
			*(lpDst + 1) = p;
			lpDst++;
			lpDst++;
		}
		else if (bReplace == TRUE)
		{
			*(lpDst + 0) = ' ';
			*(lpDst + 1) = p;
			lpDst++;
			lpDst++;
		}
	}

	*(lpDst + 0) = '\0';
	*(lpDst + 1) = '\0';

	return blModify;
}


//===========================================================================
//	ä÷êî	ÅF	RemoveForbiddenCharA
//	à¯êî	ÅF	lpStr (IN/OUT)		êßå¿èàóùëOï∂éöóÒ / êßå¿èàóùå„ï∂éöóÒ
//
//	ï‘íl	ÅF	èúãéÇµÇΩ : TRUE / ä‹Ç‹ÇÍÇ»Ç©Ç¡ÇΩ : FALSE
//
//	if there are forbidden chars, delete them
//===========================================================================
BOOL RemoveForbiddenCharA(char *lpStr, BOOL bReplace)
{
	BOOL	blModify = FALSE;
	BOOL	blDelete = FALSE;
	char	*lpCur = NULL;
	char	*lpDst;
	short	i = 0;

	lpDst = lpStr;
	for (lpCur = lpStr; *lpCur != '\0'; lpCur++) {
		blDelete = FALSE;
		i = 0;

		while (FORBIDDEN_CHAR[i] != '\0')
		{
			if (*lpCur == FORBIDDEN_CHAR[i])
			{
				blDelete = TRUE;
				blModify = TRUE;
				break;
			}

			i++;
		}

		if (blDelete != TRUE)
		{
			*lpDst = *lpCur;
			lpDst++;
		}
		else if (bReplace == TRUE)
		{
			*lpDst = ' ';
			lpDst++;
		}
	}

	return blModify;
}
//<E>To Implement Task #3137,25-07-2024,SSDI:Manoj S

// <S><A> 2023.09.14,Code Merge #3114 SSDI:Manoj S
//=============================================================================
// function
//      GetLocalTimeStr
//
// parameters
//      pszTime            out: the time string
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//     Get the local time string
//=============================================================================
BOOL GetLocalTimeStr(WCHAR* pszTime)
{
	BOOL bRet = FALSE;
	SYSTEMTIME		systemTime;
	if (pszTime == NULL)
		goto EXIT;

	::GetLocalTime(&systemTime);
	::wsprintf(pszTime,
		L"%04d%02d%02d%02d%02d%02d",
		systemTime.wYear,
		systemTime.wMonth,
		systemTime.wDay,
		systemTime.wHour,
		systemTime.wMinute,
		systemTime.wSecond
	);

	bRet = TRUE;

EXIT:
	return bRet;
}
// <E> 2021.9.16 Jelly

// <S><A> 2021.12.8 Jelly add for func2 fav 
//=============================================================================
// function
//      TransferNameToHash
//
// parameters
//      pName            in: the name string
//      pNameHash        out: the name hash string
// return value
//      ê¨å˜ : TRUE
//      é∏îs : FALSE
//
// outline
//     transfer name to hash
//=============================================================================
void TransferNameToHash(WCHAR* pName, WCHAR* pNameHash)
{
	MD5 md5;
	string strMD5;

	int nLength = wcslen(pName);
	if (nLength > 0)
	{
		int nBuffer = IsUnicodeOS() ? nLength * sizeof(WCHAR) : nLength;
		md5.update((void*)pName, nBuffer);
		strMD5 = md5.toString();
		memcpy(pNameHash, strMD5.c_str(), SCUI_HASH_LENGTH);
	}
}

//=============================================================================
// function
//      BinarytoHexString
//		
// parameters
//      szBinary            input binary
//		dwSize				the size of szBinary
// return value
//      string				hex string
//
// outline
//      transfer binary to hex string
//=============================================================================
std::wstring BinarytoHexString(const /*unsigned*/ WCHAR* szBinary, const DWORD& dwSize)
{
	std::wstring strOutput;
	WCHAR ch[5];//bear
	if (szBinary == NULL || dwSize <= 0)
		goto EXIT;

	for (DWORD i = 0; i < dwSize; i++)
	{
		//wsprintf(ch, 3, "%02x", szBinary[i]);
		wsprintf(ch, L"%02x", szBinary[i]);
		strOutput += ch;
	}
EXIT:
	return strOutput;
}
////=============================================================================
//// function
////      UTF8ToUnicode
////
//// parameters
////      szUtf8             multibyte
////
//// return value
////      wstring				wchar
////
//// outline
////      transfer utf-8 to unicode 
////=============================================================================
//std::wstring UTF8ToUnicode(const WCHAR* szUtf8)
//{
//	int wcsLen = 0;
//	wchar_t* wszUnicode = NULL;
//	string wstrOutput;
//
//	if (szUtf8 == NULL)
//		goto EXIT;
//
//	wcsLen = ::MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, NULL, 0);
//
//	wszUnicode = new wchar_t[wcsLen + 1];
//	memset(wszUnicode, 0, (wcsLen + 1) * sizeof(wchar_t));
//
//	::MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, wszUnicode, wcsLen);
//
//	wstrOutput = wszUnicode;
//
//EXIT:
//	if (wszUnicode != NULL)
//		delete[] wszUnicode;
//
//	return wstrOutput;
//}

//=============================================================================
// function
//      HexStringtoBYTE
//		
// parameters
//      szHex           hex string,2 bytes
//		
// return value
//      BYTE			
//
// outline
//      transfer hex string(2 bytes) to 1 byte
//=============================================================================
BYTE HexStringtoBYTE(const WCHAR* szHex)
{
	BYTE btRet = 0;
	if (szHex == NULL)
		goto EXIT;

	BYTE h = szHex[0];
	BYTE l = szHex[1];
	if (isalpha(h))
		h = h - 'a' + 10;
	else
		h = h - '0';

	if (isalpha(l))
		l = l - 'a' + 10;
	else
		l = l - '0';

	btRet = h * 16 + l;
EXIT:
	return btRet;
}
////=============================================================================
//// function
////      HexStringtoBinary
////		
//// parameters
////      szHex           IN hex string
////		dwHexSize		IN the size of szHex
////		szBinary		OUT binary data
////		dwBinarySize	IN the size of szBinary
////
//// return value
////      BOOL		
////
//// outline
////      transfer hex string to binary data
////=============================================================================
BOOL HexStringtoBinary(const WCHAR* szHex, const DWORD& dwHexSize, BYTE* szBinary, const DWORD& dwBinarySize)
{
	BOOL bRet = FALSE;
	const WCHAR* pszTmpHex = szHex;
	BYTE* pszTmpBinary = szBinary;
	DWORD dwSize = 0;

	if (dwHexSize != dwBinarySize * 2)
		goto EXIT;

	while (dwSize < dwHexSize)
	{
		BYTE btOne = HexStringtoBYTE(pszTmpHex);
		memcpy(pszTmpBinary, &btOne, sizeof(BYTE));

		pszTmpHex += 2;
		dwSize += 2;
		pszTmpBinary++;
	}
	bRet = TRUE;

EXIT:
	return bRet;
}
//=============================================================================
// function
//      CreateSharpLocalAppDataDierctory
//
// parameters
//      pwzPath		Full path of Json File
// return value
//      void
//
// outline
//     create directory: %user%\Appdata\Local\SHARP
//=============================================================================
void CreateSharpLocalAppDataDierctory(const wchar_t* pwzPath)
{
	wchar_t	wzPath[MAX_PATH] = { 0 };

	if (pwzPath == NULL)
		goto EXIT;

	::wcscpy_s(wzPath, MAX_PATH, pwzPath);

	wchar_t* pwzFileName = NULL;
	wchar_t* pwzModelName = NULL;

	pwzFileName = wcsrchr(wzPath, '\\');
	*pwzFileName = '\0';

	pwzModelName = wcsrchr(wzPath, '\\');
	*pwzModelName = '\0';

	if (!PathIsDirectoryW(wzPath))
	{
		CreateDirectoryW(wzPath, NULL);
	}
	*pwzModelName = '\\';

	if (!PathIsDirectoryW(wzPath))
	{
		CreateDirectoryW(wzPath, NULL);
	}
	*pwzFileName = '\\';

EXIT:
	return;
}
void GetSharpLocalAppDataPath(wchar_t* pwzPath, const wchar_t* pwzDeviceName, const wchar_t* pwzCompanyName)
{
	wchar_t	wzLocalAppData[MAX_PATH] = { 0 };

	if (pwzPath == NULL)
		goto EXIT;

	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, wzLocalAppData)))
	{
		::wcscpy_s(pwzPath, MAX_PATH, wzLocalAppData);
		::wcscat_s(pwzPath, MAX_PATH, L"\\");
		::wcscat_s(pwzPath, MAX_PATH, pwzCompanyName);

		::wcscat_s(pwzPath, MAX_PATH, L"\\");
		::wcscat_s(pwzPath, MAX_PATH, pwzDeviceName);
	}
EXIT:
	return;
}
//=============================================================================
// function
//      WritePrinerNameToFile
//
// parameters
//      pwzPath					Destination path
//      pwzDeviceName			Writing device name
//
// return value
//      ñ≥Çµ
//
// outline
//     Create a text file with the Device name
//=============================================================================
BOOL WritePrinerNameToFile(const wchar_t* pwzPath, const wchar_t* pwzDeviceName)
{
	BOOL bRet = FALSE;
	WCHAR szPath[MAX_PATH] = { 0 };
	WCHAR szDeviceName[SCUI_PRNNAME_MAX] = { 0 };
	std::wofstream os;

	//json temp /*::WideCharToMultiByte(GetACP(), 0, pwzPath, wcslen(pwzPath), szPath, MAX_PATH, NULL, NULL);
	//::WideCharToMultiByte(GetACP(), 0, pwzDeviceName, wcslen(pwzDeviceName), szDeviceName, SCUI_PRNNAME_MAX, NULL, NULL);*/

	::wcscat_s(szPath, MAX_PATH, L"\\printer_name.txt");

	os.open(szPath, std::ios::out);
	if (!os.is_open())
		goto EXIT;

	os << szDeviceName;
	os.close();

	bRet = TRUE;

EXIT:
	return bRet;

}
//=============================
std::string UnicodeToUTF8(const wchar_t* pwzUnicode)
{
	int len = 0;
	char *szUtf8 = NULL;
	std::string strOutput;
	if (pwzUnicode == NULL)
		goto EXIT;

	len = WideCharToMultiByte(CP_UTF8, 0, pwzUnicode, -1, NULL, 0, NULL, NULL);
	szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, pwzUnicode, -1, szUtf8, len, NULL, NULL);

	strOutput = szUtf8;

EXIT:
	if (szUtf8 != NULL)
		delete[] szUtf8;

	return strOutput;
}

std::wstring UTF8ToUnicode(const char* szUtf8)
{
	int wcsLen = 0;
	wchar_t* wszUnicode = NULL;
	std::wstring wstrOutput;

	if (szUtf8 == NULL)
		goto EXIT;

	wcsLen = ::MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, NULL, 0);

	wszUnicode = new wchar_t[wcsLen + 1];
	memset(wszUnicode, 0, (wcsLen + 1) * sizeof(wchar_t));

	::MultiByteToWideChar(CP_UTF8, 0, szUtf8, -1, wszUnicode, wcsLen);

	wstrOutput = wszUnicode;

EXIT:
	if (wszUnicode != NULL)
		delete[] wszUnicode;

	return wstrOutput;
}

