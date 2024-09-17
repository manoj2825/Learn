//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//===========================================================================
//  File          : UtilityFunctions.h
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 18 June 2013
//
//	Description   : Helper function declaration for APIs GetPrinterDriverPath name etc.

//===========================================================================

//===========================================================================
//  Reviews 
//
//  Sl#     Review Date     Reviewer(s)               Remarks
//===========================================================================
#pragma once
#include <map>
#define		SMKEY(x)		TEXT(x)
#define	RTPR_PURENAME	2	//	             FriendlyName
typedef enum
{
	pamph_none,
	pamph_tiled,
	pamph_2up,
	pamph_tiled_g,
	pamph_2up_g,
}PV_PAMPH;

// ================================================
// _/_/_/  paper size
// ================================================
typedef enum
{
	paper_letter = 1,				//   1
	paper_ledger = 3,				//   3
	paper_legal = 5,				//   5
	paper_invoice,					//   6
	paper_exective,					//   7
	paper_a3,						//   8
	paper_a4,						//   9
	paper_a5 = 11,					//  11
	paper_b4,						//  12
	paper_b5,						//  13
	paper_foolscap,					//  14
	paper_11x17 = 17,				//  17
	paper_com9 = 19,				//  19
	paper_com10 = 20,				//  20
	paper_dl = 27,					//  27
	paper_c5,						//  28
	paper_c4 = 30,					//  30
	paper_c6,						//  31
	paper_iso_b5 = 34,				//  34
	paper_monarch = 37,				//  37
	paper_jpn_post = 43,			//  43
	paper_iso_b5_ex = 65,			//  65
	paper_a2,						//  66
	paper_jpn_d_post = 69,			//  69
	paper_a6,						//  70
	paper_240x332,					//  71 kaku2
	paper_216x277,					//  72 kaku3
	paper_jpn_c3 = 73,				//  73
	paper_jpn_c4,					//  74
	paper_a6_r = 83,				//  83
	paper_b6 = 88,					//  88
	paper_b6_r,						//  89
	paper_jpn_y4 = 91,				//  91
	paper_16k = 93,					//  93
	paper_32k,						//  94
	paper_120 = 120,				//  120 for A3/Ledger
	paper_121 = 121,				//  121 for A4/Letter
	paper_custom = 256,				// 256
	paper_a4_r = 260,				// 260
	paper_a5_r,						// 261
	paper_b5_r,						// 262
	paper_letter_r,					// 263
	paper_16k_r,					// 264
	paper_folio = 280,				// 280
	paper_8k,						// 281
	paper_invoice_r,				// 282
	paper_executive_r,				// 283
	paper_a3_wide,					// 284
	paper_jpn_y2,					// 285
	paper_foolscap_r,				// 286
	paper_com10_r,					// 287
	paper_dl_r,						// 288
	paper_c5_r,						// 289
	paper_computer,					// 290
	paper_13inchlg,					// 291
	paper_85sq,						// 292
	paper_jpn_env_y2_r,				// 293
	paper_8k_r,						// 294
	paper_12x18,					// 295
	paper_12x9,						// 296
	paper_13x19,					// 297
	paper_216x330,					// 298
	paper_sra3,						// 299
	paper_sra4,						// 300
	paper_kiku4,					// 301
	paper_kiku8,					// 302
	paper_a_4,						// 303
	paper_a_8,						// 304
	paper_mexican_legal,			// 305
	paper_extra,					// 306
	paper_a4_wide,					// 307
	paper_halflegal,				// 308
	paper_mexican_legal2,			// 309
	paper_a0,						// 310
	paper_a1,						// 311
	paper_a3w_half,					// 312
	paper_half_mexican_legal,		// 313
	paper_half_mexican_legal2,		// 314
	paper_half_foolscap,			// 315
	paper_half_folio,				// 316
	paper_half_13x19,				// 317
	paper_200x296,					// 318 四面連刷はがき
	paper_90x225,					// 319 長形40号
	paper_120x235,					// 320 洋形長3号
	paper_162x229,					// 321 角形6号

	paper_long = 400,				// 400
	paper_custom1 = 401,			// 401
	paper_custom2,					// 402
	paper_custom3,					// 403
	paper_custom4,					// 404
	paper_custom5,					// 405
	paper_custom6,					// 406
	paper_custom7,					// 407

	paper_custom_pamph = 450,		// 450
	paper_custom_pamph_half,		// 451
	paper_custom_zoom,				// 452
	paper_custom_zoom_half,			// 453

	paper_a4_a = 454,				// 454
	paper_letter_a,					// 455
	paper_a3_a,						// 456
	paper_ledger_a,					// 457
	paper_a4_b,						// 458
	paper_letter_b,					// 459

	paper_same_as_org = 499,		// 499
	paper_max						// 
}PV_PAPER;

// ================================================
// _/_/_/  paper source
// ================================================
typedef enum
{
	psrc_auto = 7,				//   7
	psrc_tray1 = 257,			//   257	tray1 Body Cassette
	psrc_tray2,					//   258	tray2 Body Cassette
	psrc_tray3,					//   259	tray3 Body Cassette
	psrc_tray4,					//   260	tray4 Body Cassette
	psrc_bypass,				//   261
	psrc_bypass_m,				//   262
	psrc_lct,					//   263
	psrc_tandem_lcf,			//   264
	psrc_ext_lcf,				//   265
	psrc_bypass_s,				//   266
	psrc_bypass_l,				//   267
	psrc_psize_auto,			//   268
	psrc_ptype_auto,			//   269
	psrc_carbon_prn,			//   270
	psrc_inserter,				//   271
	psrc_tray5,					//   272
	psrc_a3lcc,					//   273
	psrc_a4lcc,					//   274
	psrc_3tray_a3lcc_upper,		//   275
	psrc_3tray_a3lcc_middle,	//   276
	psrc_3tray_a3lcc_lower,		//   277
	psrc_3tray_a4lcc_upper,		//   278
	psrc_3tray_a4lcc_middle,	//   279
	psrc_3tray_a4lcc_lower,		//   280
	psrc_bypass_500,			//   281
	psrc_bypass_100,			//   282
	psrc_inserter_lower,		//   283
	psrc_2tray_a3lct1_5,		//   284
	psrc_2tray_a3lct1_6,		//   285
	psrc_2tray_a3lct2_5,		//   286
	psrc_2tray_a3lct2_6,		//   287
	psrc_2tray_a3lct2_7,		//   288
	psrc_2tray_a3lct2_8,		//   289
	psrc_bypass_lct,			//   290
	psrc_bypass_body,			//   291
	psrc_a4lcc_air,				//   292
	psrc_tray3_lc,				//   293
	psrc_tray4_lc,				//   294
	psrc_body_tray5,			//   295	tray5 Body Cassette

	psrc_max					//   
}PV_PSRC;

const short pamph_psize[] =
{
//   -- src --          -- dst --
	 paper_a4,			paper_a3,
	 paper_a5,			paper_a4,
	 paper_b5,			paper_b4,
	 paper_invoice,		paper_letter,
	 paper_letter,		paper_ledger,
	 paper_16k,			paper_8k,
	 paper_32k,			paper_16k,
	 paper_12x9,		paper_12x18,
	 paper_a4_wide,		paper_a3_wide,
	 paper_b6,			paper_b5,
	 paper_halflegal,	paper_legal,
	 -1,				-1 
};

// ================================================
// _/_/_/  Rotated paper size table
// ================================================
const short r_std_psize[] =
{
	//   -- r-size --       -- standard size --
		 paper_a6_r,		paper_a6,
		 paper_b6_r,		paper_b6,
		 paper_a4_r,		paper_a4,
		 paper_a5_r,		paper_a5,
		 paper_b5_r,		paper_b5,
		 paper_letter_r,	paper_letter,
		 paper_16k_r,		paper_16k,
		 paper_invoice_r,	paper_invoice,
		 paper_executive_r,	paper_exective,
		 paper_foolscap_r,	paper_foolscap,
		 paper_com10_r,		paper_com10,
		 paper_dl_r,		paper_dl,
		 paper_c5_r,		paper_c5,
		 paper_8k_r,		paper_8k,
		 -1,				-1
};

const unsigned int PaperPixelInformationArray[][3]=	{ 
													{1,5100,6600},
													{3,6600,10200},
													{5,5100,8400},
													{6,3300,5100},
													{7,4350,6300},
													{8,7014,9918},
													{9,4960,7014},
													{11,3496,4960},
													{12,6070,8598},
													{13,4298,6070},
													{14,5100,7800},
													{17,6600,10200},
													{20,2474,5700},
													{27,2598,5196},
													{27,2598,5196},
													{34,4156,5904},
													{37,2324,4500},
													{43,2362,3496},
                                                    {71,5668,7842},
                                                    {72,5102,6542},
                                                    {73,2836,5552},
                                                    {74,2124,4842},
                                                    {285,2692,3826},
                                                    {91,2480,5550},
													{308,4200,5100}
													
												};
typedef struct _tagPaperPixelInformation
{
	DWORD dwWidthPixel;
	DWORD dwHeightPixel;
}PAPERPIXELINFORMATION,*PPAPERPIXELINFORMATION;
typedef std::map<DWORD,PAPERPIXELINFORMATION>PaperPixelInformationMap;
short GetPamphSize(short wSrcPaper, short wPamphMode, BOOL blHWPamph, BOOL blRealSize = FALSE, const short FAR *ptable = NULL);
PaperPixelInformationMap InitializePaperPixelInfoMap(void);
BOOL WINAPI GetPureName(LPTSTR	lpCmnDModeDeviceName,LPTSTR	lpPrinterName,INT nCharSize,INT	nMode);
BOOL GetPrinterNameFromHandle(WCHAR *pPrinterName,HANDLE hPrinter);
WCHAR* setstr(WCHAR *pszModelName, WCHAR *pszPortName);
BOOL CheckSpaceOnly(WCHAR *pString);
// <S><A> Added function to check space double quote and remove double quote in UD3L, 20200827 - SSDI:Goutham
BOOL RemoveDoubleQuoteChar(char *lpStr);
BOOL RemoveDoubleQuoteCharW(LPWSTR lpStr);
BOOL RemoveDoubleQuoteCharA(char *lpStr);
BOOL CheckSpaceDoubleQuoteOnly(char *pszData, long lSize);
BOOL CheckSpaceDoubleQuoteOnlyA(char *pszData, long lSize);
BOOL CheckSpaceDoubleQuoteOnlyW(LPWSTR pszData, long lSize);
// <S><E> Added function to check space double quote and remove double quote in UD3L, 20200827 - SSDI:Goutham
WORD Cutstrlen(WCHAR *pszData, long lSize, WORD wPoint);
WORD Getstrlen(LPWSTR pszOutData, LPWSTR pszInData, long lSize, WORD wPoint);
BOOL RemoveDBCSChar(WCHAR *lpWChar,size_t CharSize);
BOOL RemoveDBCSChar(CHAR *lpStr);
short GetUnitDef(void);
short GetPaperSizeDef(void);
long GetCountryNumber(void);
BOOL GetIPAddress(HANDLE hPrinter,WCHAR* /*pPortName*/,WCHAR *pRegisteredPortName);
WCHAR* GetIPAddressFromXCVInterface(LPTSTR lpServerName, WCHAR* lpPortName);
HANDLE GetXcvHandle(LPTSTR lpServerName,WCHAR* lpPortName);
short ConvertStdSizeToRSize(short wSize, const short FAR *ptable = NULL);
//<S><A>To Implement Task#3132,15-05-2024,SSDI:Manoj S
BOOL IsNumber(char* const szStr);
//<E>To Implement Task #3132,15-05-2024,SSDI:Manoj S
//<S><A>To Implement Task#3137,25-07-2024,SSDI:Manoj S
BOOL RemoveForbiddenChar(char *lpStr, BOOL bReplace = FALSE);
BOOL RemoveForbiddenCharW(LPWSTR lpStr, BOOL bReplace = FALSE);
BOOL RemoveForbiddenCharA(char *lpStr, BOOL bReplace = FALSE);
//<E>To Implement Task #3137,25-07-2024,SSDI:Manoj S
//<S><A>To Implement Task#3114,13-09-2024,SSDI:Manoj S
BOOL GetLocalTimeStr(WCHAR* pszTime);
void TransferNameToHash(WCHAR* pName, WCHAR* pNameHash);
//<E>To Implement Task #3114,13-09-2024,SSDI:Manoj S
std::wstring BinarytoHexString(const WCHAR* szBinary, const DWORD& dwSize);
//std::wstring BinarytoHexString(const WCHAR* szBinary, const DWORD& dwSize);
BYTE HexStringtoBYTE(const WCHAR* szHex);
BOOL HexStringtoBinary(const WCHAR* szHex, const DWORD& dwHexSize, BYTE* szBinary, const DWORD& dwBinarySize);
void CreateSharpLocalAppDataDierctory(const wchar_t* pwzPath);
void GetSharpLocalAppDataPath(wchar_t* pwzPath, const wchar_t* pwzDeviceName, const wchar_t* pwzCompanyName);
BOOL WritePrinerNameToFile(const wchar_t* pwzPath, const wchar_t* pwzDeviceName);
std::string UnicodeToUTF8(const wchar_t* pwzUnicode);
std::wstring UTF8ToUnicode(const char* szUtf8);
