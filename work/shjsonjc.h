// ============================================================================
// shjsonjc.h
//
//     Copyright 2022 by SHARP Corporation.
// ----------------------------------------------------------------------------
//                                                                  2022.04.24
// ============================================================================

#ifndef _SHJSONJC_H
#define _SHJSONJC_H

#include "shjson.h"


// ================================================
// _/_/_/  class
// ================================================
class CShJsonJC : public CShJson
{
public:

	CShJsonJC(HINSTANCE hInst, WCHAR FAR *pszSvrPrnName/*, CShPrnLog FAR *pLog*/);
	virtual ~CShJsonJC();

	BOOL InitJobCtrl(short wDefUName, short wDefJName);

	BOOL ReadJCData(PREGJCINFO pjc, WORD wEicoh = FALSE);
	BOOL WriteJCData(PREGJCINFO pjc, WORD wField = JC_ALL, WORD wEicoh = FALSE);

//If the pszIn is longer than 16 bytes, use these functions below.
	//BOOL Encode_AES_EX(WCHAR FAR *pszOut, WCHAR FAR *pszIn, DWORD& dwSize);
	//BOOL Decode_AES_EX(WCHAR FAR *pszOut, WCHAR FAR *pszIn, DWORD& dwSize);

	void GetEncodedPassWordData(CHAR * byEncodedPasswordData, WCHAR * pInputPasswordData, size_t sMaximumInputStringLength, size_t sOutputStringLength);
	void GetDecodedPassWordData(CHAR * byEncodedPasswordData, WCHAR * pOuputPasswordData, size_t sEncodedStringLength);

protected:
// <S><A> 2022.05.17 sbc R.Tanaka
	virtual void SetFunctionsJsonPath(wchar_t* wzJsonPath);
// <E> 2022.05.17 sbc R.Tanaka

};

#endif	// #ifndef _SHJSONJC_H
