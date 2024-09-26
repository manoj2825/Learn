//	Copyright 2013(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//==================================================================================
//  File          : MimicImage.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Gajanana D S
//
//  Date		  : 17 November 2013
//
//	Description   : Implementation file for mimic control.
//	Modified the file According to UD3.1,Add Extra Functions to the Support of Mimic
//	Based on CR4.5/UD3.1, 19-11-2020,SSDI:Chanchal Parkash
//===================================================================================
#include "precomp.h"
#include "..\..\BitmapResource\Include\resource.h"
#include "..\Include\define.h"
#include "shjsonms.h"
//For Logging Purpose
#include "MFPLogger.h"
#define MM_BKCOL_Y		   6
#define MM_SUBOPTL_X       5
#define MM_SUBOPTL_Y       100
#define MM_PUNCH_BASEID		7000
#define MM_PUNCHTYPE_BASEID	9000

#define MM_BKCOL_X					66
#define MMC_SUBOPT_MARGIN_WIDTH		10
#define MMC_SUBOPT_MARGIN_HEIGHT	10
#define MMC_SUBOPT_ICON_WIDTH		24
#define MMC_SUBOPT_ICON_HEIGHT		30

#define MMC_SUBOPT_1_TOP	16
#define MMC_SUBOPT_1_LEFT	270
#define MMC_SUBOPT_2_TOP	(MMC_SUBOPT_1_TOP + MMC_SUBOPT_MARGIN_HEIGHT + MMC_SUBOPT_ICON_HEIGHT)
#define MMC_SUBOPT_2_LEFT	MMC_SUBOPT_1_LEFT
#define MMC_SUBOPT_3_TOP	(MMC_SUBOPT_2_TOP + MMC_SUBOPT_MARGIN_HEIGHT + MMC_SUBOPT_ICON_HEIGHT)
#define MMC_SUBOPT_3_LEFT	MMC_SUBOPT_1_LEFT
#define MMC_SUBOPT_4_TOP	MMC_SUBOPT_1_TOP
#define MMC_SUBOPT_4_LEFT	(MMC_SUBOPT_1_LEFT + MMC_SUBOPT_MARGIN_WIDTH + MMC_SUBOPT_ICON_WIDTH)
#define MMC_SUBOPT_5_TOP	MMC_SUBOPT_2_TOP
#define MMC_SUBOPT_5_LEFT	MMC_SUBOPT_4_LEFT
#define MMC_SUBOPT_6_TOP	MMC_SUBOPT_3_TOP
#define MMC_SUBOPT_6_LEFT	MMC_SUBOPT_4_LEFT
//=============================================================================
// function
//      CMimicImage
//
// return value
//     NILL
//
// outline
//      Constructor to the class CMimicImage.
//=============================================================================
CMimicImage::CMimicImage(
							_In_ CONST INT   iCtrlID,
							HWND hParentHandle,
							PFEATUREINFOLIST pFeatureInfoList,
							HMODULE hStringResourceHandle,
							HMODULE hBitmapResourceHandle,
							WCHAR*	pPrinterName,
							POEMDEV pOemPrivateDevMode,
							SHORT* sWaterMarkIndex
						):
										CUIControl(hStringResourceHandle,hBitmapResourceHandle),
										m_pFeatureInfoList(pFeatureInfoList),
										m_pBitMap(NULL),
										m_hParentHandle(hParentHandle),
										m_pPrinterName(pPrinterName),
										m_pOemPrivateDevMode(pOemPrivateDevMode),
										m_pWaterMarkIndex(sWaterMarkIndex)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	m_hMimicImageHandle = GetDlgItem(m_hParentHandle,iCtrlID);
    Attach(m_hMimicImageHandle);
	m_hDC = GetDC(m_hMimicImageHandle);
	RectMimicImage = {};
	GetWindowRect(m_hMimicImageHandle, &RectMimicImage);
	m_lBBoxW = RectMimicImage.right- RectMimicImage.left;
	m_lBBoxH = RectMimicImage.bottom - RectMimicImage.top;
	m_hBitmapResourceHandle = hBitmapResourceHandle;

	m_pBitMap = new CBitmap(m_hBitmapResourceHandle);
	//m_WaterMarkIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
	if(NULL == m_pBitMap)
	{
		slog.putLog("CMimicImage::CMimicImage -> NULL_POINTER(m_pBitMap)\n");
	}
	SecureZeroMemory(&m_rcItem1, sizeof(RECT));
	SecureZeroMemory(&m_rcItem2, sizeof(RECT));
	SecureZeroMemory(&m_rcItem3, sizeof(RECT));
	SecureZeroMemory(&m_rcItem4, sizeof(RECT));
	SecureZeroMemory(&m_rcItem5, sizeof(RECT));
	SecureZeroMemory(&m_rcItem6, sizeof(RECT));
	m_pBmpList1 = NULL;
	m_pBmpList2 = NULL;
	m_pBmpList3 = NULL;
	m_pBmpList4 = NULL;
	m_pBmpList5 = NULL;
	m_pBmpList6 = NULL;
	m_lSubOptOffset_X = 0;		// X
	m_lSubOptOffset_Y = 0;		// Y

	m_CRColor = 0;
	m_lIconWidth = 0;
	m_lIconHeight = 0;
	m_blReset = FALSE;
	m_blResourceRtoLCnv = FALSE;
}
//=============================================================================
// function
//      CMimicImage
//
// return value
//     NILL
//
// outline
//      Destructor to the class CMimicImage.
//=============================================================================
CMimicImage::~CMimicImage(void)
{
	if(NULL != m_hDC)
	{
		ReleaseDC(m_hMimicImageHandle, m_hDC);
	}
	if(nullptr != m_pBitMap)
	{
		delete m_pBitMap;
		m_pBitMap = NULL ;
	}

	if (m_pBmpList1 != NULL) {
		(*m_pBmpList1).Destroy();
		delete m_pBmpList1;
	}

	if (m_pBmpList2 != NULL) {
		(*m_pBmpList2).Destroy();
		delete m_pBmpList2;
	}

	if (m_pBmpList3 != NULL) {
		(*m_pBmpList3).Destroy();
		delete m_pBmpList3;
	}

	if (m_pBmpList4 != NULL) {
		(*m_pBmpList4).Destroy();
		delete m_pBmpList4;
	}

	if (m_pBmpList5 != NULL) {
		(*m_pBmpList5).Destroy();
		delete m_pBmpList5;
	}

	if (m_pBmpList6 != NULL) {
		(*m_pBmpList6).Destroy();
		delete m_pBmpList6;
	}
}
//=============================================================================
// function
//      Draw
//
// return value
//     NILL
//
// outline
//      Draw Preview Image.
//=============================================================================
VOID CMimicImage::Draw(PPROPSTATE pps)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImage::Draw -> NULL_POINTER(m_pBitMap)\n");
		return;
	}
	HBITMAP hbm  = NULL ;
	HBITMAP		hbmOld2 = NULL;
	HDC			hDCBM = NULL;
	HDC hDCMem = CreateCompatibleDC(m_hDC);
	if(nullptr == hDCMem)
	{
		slog.putLog("CMimicImage::Draw -> NULL_HANDLE(hDCMem)\n");
		return ;
	}
	hbm = CreateCompatibleBitmap(m_hDC, m_lBBoxW, m_lBBoxH);
	if(nullptr == hbm)
	{
		slog.putLog("CMimicImage::Draw -> NULL_HANDLE(hbm)\n");
		ReleaseResources(hDCMem);
		return ;
	}
	HBITMAP hbmOld = (HBITMAP)SelectObject(hDCMem, hbm);
	hDCBM = ::CreateCompatibleDC(m_hDC);
	if(nullptr == hDCBM )
	{
		slog.putLog("CMimicImage::Draw -> NULL_HANDLE(hDCBM)\n");
		ReleaseResources(hDCMem,hbm);
		return ;
	}
	m_pBitMap->load(IDB_MM_BKGRND);
	hbmOld2 = (HBITMAP)::SelectObject(hDCBM, m_pBitMap->handle());
	::StretchBlt(hDCMem, 0, 0, m_lBBoxW, m_lBBoxH, hDCBM, 0, 0, m_pBitMap->width(), m_pBitMap->height(), SRCCOPY);
	::SelectObject(hDCBM, hbmOld2);
	//<S><A>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	//TThe spacing and positioning of Output image and icons are not proper
	//<S><C>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	LONG lBkImgX = 96;
	//LONG lBkImgX  = GetDeviceCaps(hDCMem, LOGPIXELSY);
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	drawPaperImg(pps,hDCMem,lBkImgX);

	// ================================================
	// _/_/_/  sub optionÉCÉÅÅ[ÉWï`âÊ
	// ================================================
	drawSubOptionImg(pps, hDCMem, &RectMimicImage, lBkImgX);

	::SelectObject(hDCBM, hbmOld2);
	::BitBlt(m_hDC, 0, 0, m_lBBoxW, m_lBBoxH, hDCMem, 0, 0, SRCCOPY);

	::SelectObject(hDCMem, hbmOld);
	ReleaseResources(hDCMem,hbm,hDCBM);
}
//=============================================================================
// Function :
//		OnInit
// parameters
//      HWND      Handle to Window Dialog
//		iCtrlID	  Control ID
//
// return value
//      HRESULT
//
// outline
//      OnInit
//=============================================================================
HRESULT CMimicImage::OnInit(const INT iCtrlID, const HWND hDlg)
{
	HRESULT hr = S_OK;
	InitSubOptImgList();
	setSubOptImgList1();
	setSubOptImgList2();
	setSubOptImgList3();
	setSubOptImgList4();
	setSubOptImgList5();
	setSubOptImgList6();
	return hr;
}
//=============================================================================
// function
//      InitSubOptImgList
//
// parameters
//   None
//
// return value
//     None
//
// outline
//     InitSubOptImgList
//=============================================================================
void CMimicImage::InitSubOptImgList(void)
{
	BYTE			wMaskR = 0, wMaskG = 0, wMaskB = 0;

	m_lSubOptOffset_X = 0;
	m_lSubOptOffset_Y = 0;

	wMaskR = (BYTE)128;
	wMaskG = (BYTE)0;
	wMaskB = (BYTE)128;

	m_lIconWidth = (BYTE)24;
	m_lIconHeight = (BYTE)30;

//<S><D>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	int iDPIPer = GetScreenResolution();
	m_lSubOptOffset_X = (m_lSubOptOffset_X * iDPIPer) / 100;
	m_lSubOptOffset_Y = (m_lSubOptOffset_Y * iDPIPer) / 100;
	m_lResizedIconWidth = (m_lIconWidth * iDPIPer) / 100;
	m_lResizedIconHeight = (m_lIconHeight * iDPIPer) / 100;
//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	m_CRColor = RGB(wMaskR, wMaskG, wMaskB);

	m_blReset = TRUE;

	return;
}
//=============================================================================
// function
//      setSubOptImgList1
//
// parameters
//   None
//
// return value
//     None
//
// outline
//      setSubOptImgList1
//=============================================================================
void CMimicImage::setSubOptImgList1(void)
{
	if (m_pBmpList1 == NULL) {
		// BMP List
		m_pBmpList1 = new CShBitmapList(m_hBitmapResourceHandle);
		if (m_pBmpList1 == NULL)
		{
			goto EXIT;
		}
	}
	//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	//TThe spacing and positioning of Output image and icons are not proper
	LONG lSubopt_1_TOP = MMC_SUBOPT_1_TOP;
	LONG lSubopt_1_LEFT  = MMC_SUBOPT_1_LEFT;
	SetSubOptImgTop_and_Left(MMC_SUBOPT_1_TOP, MMC_SUBOPT_1_LEFT, &lSubopt_1_TOP, &lSubopt_1_LEFT);

	// BMP List
	(*m_pBmpList1).Destroy();

	(*m_pBmpList1).Create(m_lIconWidth, m_lIconHeight, ILC_COLOR8 | ILC_MASK, 0, 0, 2);

	// IconÇ
	(*m_pBmpList1).AddMasked((short)IDB_CMBI_COLLATE_UNCOLLATE, m_CRColor);
	(*m_pBmpList1).AddMasked((short)IDB_CMBI_COLLATE_COLLATE, m_CRColor);
	
	//<S><D>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	/*m_rcItem1.top = lSubopt_1_TOP + m_lSubOptOffset_Y;
	m_rcItem1.left = lSubopt_1_LEFT + m_lSubOptOffset_X;
	m_rcItem1.bottom = m_rcItem1.top + m_lIconHeight;
	m_rcItem1.right = m_rcItem1.left + m_lIconWidth;*/
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
EXIT:
	return;
}
//=============================================================================
// function
//      setSubOptImgList2
//
// parameters
//   None
//
// return value
//     None
//
// outline
//      setSubOptImgList2
//=============================================================================
void CMimicImage::setSubOptImgList2(void)
{
	if (m_pBmpList2 == NULL) {
		// BMP ListÇ
		m_pBmpList2 = new CShBitmapList(m_hBitmapResourceHandle);
		if (m_pBmpList2 == NULL)
		{
			goto EXIT;
		}
	}
	//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	//TThe spacing and positioning of Output image and icons are not proper
	LONG lSubopt_2_TOP = MMC_SUBOPT_2_TOP;
	LONG lSubopt_2_LEFT = MMC_SUBOPT_2_LEFT;
	SetSubOptImgTop_and_Left(MMC_SUBOPT_2_TOP, MMC_SUBOPT_2_LEFT, &lSubopt_2_TOP, &lSubopt_2_LEFT);
	
	// BMP ListÇ
	(*m_pBmpList2).Destroy();
	(*m_pBmpList2).Create(m_lIconWidth, m_lIconHeight, ILC_COLOR8 | ILC_MASK, 0, 0, 3);
	
	(*m_pBmpList2).AddMasked((short)IDB_CMBI_COLORMODE_BLACKANDWHITE, m_CRColor); //0
	(*m_pBmpList2).AddMasked((short)IDB_CMBI_COLORMODE_COLOR, m_CRColor);      //1
	(*m_pBmpList2).AddMasked((short)IDB_CMBI_COLORMODE_AUTOMATIC, m_CRColor);    // 2
	
	//<S><D>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	/*m_rcItem2.top = lSubopt_2_TOP + m_lSubOptOffset_Y;
	m_rcItem2.left = lSubopt_2_LEFT  + m_lSubOptOffset_X;
	m_rcItem2.bottom = m_rcItem2.top + m_lIconHeight;
	m_rcItem2.right = m_rcItem2.left + m_lIconWidth;*/
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
EXIT:
	return;
}
//=============================================================================
// function
//      setSubOptImgList3
//
// parameters
//   None
//
// return value
//     None
//
// outline
//      setSubOptImgList3
//=============================================================================
void CMimicImage::setSubOptImgList3(void)
{
	if (m_pBmpList3 == NULL) {
		// BMP ListÇ
		m_pBmpList3 = new CShBitmapList(m_hBitmapResourceHandle);
		if (m_pBmpList3 == NULL)
		{
			goto EXIT;
		}
	}
	//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	//TThe spacing and positioning of Output image and icons are not proper
	LONG lSubopt_3_TOP = MMC_SUBOPT_3_TOP;
	LONG lSubopt_3_LEFT = MMC_SUBOPT_3_LEFT;
	SetSubOptImgTop_and_Left(MMC_SUBOPT_3_TOP, MMC_SUBOPT_3_LEFT, &lSubopt_3_TOP, &lSubopt_3_LEFT);
	// BMP ListÇ
	(*m_pBmpList3).Destroy();

	(*m_pBmpList3).Create(m_lIconWidth, m_lIconHeight, ILC_COLOR8 | ILC_MASK, 0, 0, 5);

	// IconÇ
	(*m_pBmpList3).AddMasked((short)IDB_CMBI_2_SIDEDPRINTING_NONE, m_CRColor);		// 0
	(*m_pBmpList3).AddMasked((short)IDB_CMBI_2_SIDEDPRINTING_LONGEDGE, m_CRColor);	// 1
	(*m_pBmpList3).AddMasked((short)IDB_CMBI_2_SIDEDPRINTING_SHORTEDGE, m_CRColor);	// 2
	(*m_pBmpList3).AddMasked((short)IDB_CMBI_BOOKLET_TILED, m_CRColor);				// 3
	//(*m_pBmpList3).AddMasked((short)IDB_CMBI_BOOKLET_FULLIMAGE, m_CRColor);			// 4
	//<S><D>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	/*m_rcItem3.top = lSubopt_3_TOP + m_lSubOptOffset_Y;
	m_rcItem3.left = lSubopt_3_LEFT + m_lSubOptOffset_X;
	m_rcItem3.bottom = m_rcItem3.top + m_lIconHeight;
	m_rcItem3.right = m_rcItem3.left + m_lIconWidth;*/
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
EXIT:
	return;
}
//=============================================================================
// function
//      setSubOptImgList4
//
// parameters
//   None
//
// return value
//     None
//
// outline
//      setSubOptImgList4
//=============================================================================
void CMimicImage::setSubOptImgList4(void)
{
	if (m_pBmpList4 == NULL) {
		// BMP ListÇ
		m_pBmpList4 = new CShBitmapList(m_hBitmapResourceHandle);
		if (m_pBmpList4 == NULL)
		{
			goto EXIT;
		}
	}
	//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	//TThe spacing and positioning of Output image and icons are not proper
	LONG lSubopt_4_TOP = MMC_SUBOPT_4_TOP;
	LONG lSubopt_4_LEFT = MMC_SUBOPT_4_LEFT;
	SetSubOptImgTop_and_Left(MMC_SUBOPT_4_TOP, MMC_SUBOPT_4_LEFT, &lSubopt_4_TOP, &lSubopt_4_LEFT);
	
	// BMP ListÇ
	(*m_pBmpList4).Destroy();

	(*m_pBmpList4).Create(m_lIconWidth, m_lIconHeight, ILC_COLOR8 | ILC_MASK, 0, 0, 2);
		// IconÇ
	(*m_pBmpList4).AddMasked((short)IDB_CMBI_OFFSET_NONE, m_CRColor);				// 0
	(*m_pBmpList4).AddMasked((short)IDB_CMBI_OFFSET_OFFSETPERSET, m_CRColor);		// 1

	//<S><D>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	/*m_rcItem4.top = lSubopt_4_TOP + m_lSubOptOffset_Y;
	m_rcItem4.left = lSubopt_4_LEFT + m_lSubOptOffset_X;
	m_rcItem4.bottom = m_rcItem4.top + m_lIconHeight;
	m_rcItem4.right = m_rcItem4.left + m_lIconWidth;*/
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
EXIT:
	return;
}
//=============================================================================
// function
//      setSubOptImgList5
//
// parameters
//   None
//
// return value
//     None
//
// outline
//      setSubOptImgList5
//=============================================================================
void CMimicImage::setSubOptImgList5(void)
{
	if (m_pBmpList5 == NULL) {
		// BMP ListÇ
		m_pBmpList5 = new CShBitmapList(m_hBitmapResourceHandle);
		if (m_pBmpList5 == NULL)
		{
			goto EXIT;
		}
	}
	//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	//TThe spacing and positioning of Output image and icons are not proper	
	LONG lSubopt_5_TOP = MMC_SUBOPT_5_TOP;
	LONG lSubopt_5_LEFT = MMC_SUBOPT_5_LEFT;
	SetSubOptImgTop_and_Left(MMC_SUBOPT_5_TOP, MMC_SUBOPT_5_LEFT, &lSubopt_5_TOP, &lSubopt_5_LEFT);
	
	// BMP ListÇ
	(*m_pBmpList5).Destroy();
	(*m_pBmpList5).Create(m_lIconWidth, m_lIconHeight, ILC_COLOR8 | ILC_MASK, 0, 0, 6);

	// IconÇ
	(*m_pBmpList5).AddMasked((short)IDB_CMBI_RETENTION_NONE, m_CRColor);			// 0 Document Filing - None
	(*m_pBmpList5).AddMasked((short)IDB_CMBI_RETENTION_HOLDAFTERPRINT, m_CRColor);	// 1 Document Filing - Hold After Print
	(*m_pBmpList5).AddMasked((short)IDB_CMBI_RETENTION_HOLDONLY, m_CRColor);		// 2 Document Filing - Hold Only
	(*m_pBmpList5).AddMasked((short)IDB_CMBI_RETENTION_SAMPLEPRINT, m_CRColor);		// 3 Document Filing - Hold Sample Print
	(*m_pBmpList5).AddMasked((short)IDB_MM_PRINTSHARE, m_CRColor);					// 4 Print share
	(*m_pBmpList5).AddMasked((short)IDB_MM_PRINTANDSEND_ON, m_CRColor);				// 5 Print and Send On
	
	//<S><D>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	/*m_rcItem5.top = lSubopt_5_TOP + m_lSubOptOffset_Y;
	m_rcItem5.left = lSubopt_5_LEFT + m_lSubOptOffset_X;
	m_rcItem5.bottom = m_rcItem5.top + m_lIconHeight;
	m_rcItem5.right = m_rcItem5.left + m_lIconWidth;*/
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
EXIT:
	return;
}
//=============================================================================
// function
//      setSubOptImgList6
//
// parameters
//   None
//
// return value
//     None
//
// outline
//      setSubOptImgList6
//=============================================================================
void CMimicImage::setSubOptImgList6(void)
{
	if (m_pBmpList6 == NULL) {
		// BMP ListÇ
		m_pBmpList6 = new CShBitmapList(m_hBitmapResourceHandle);
		if (m_pBmpList6 == NULL)
		{
			goto EXIT;
		}
	}
	//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	//TThe spacing and positioning of Output image and icons are not proper
	LONG lSubopt_6_TOP = MMC_SUBOPT_6_TOP;
	LONG lSubopt_6_LEFT = MMC_SUBOPT_6_LEFT;
	SetSubOptImgTop_and_Left(MMC_SUBOPT_6_TOP, MMC_SUBOPT_6_LEFT, &lSubopt_6_TOP, &lSubopt_6_LEFT);
	
	// BMP ListÇ
	(*m_pBmpList6).Destroy();


	(*m_pBmpList6).Create(m_lIconWidth, m_lIconHeight, ILC_COLOR8 | ILC_MASK, 0, 0, 5);

		// IconÇ
	(*m_pBmpList6).AddMasked((short)IDB_CMBI_AUTHENTICATION_NONE, m_CRColor);		// 0
	(*m_pBmpList6).AddMasked((short)IDB_CMBI_AUTHENTICATION_LOGIN, m_CRColor);		// 1
	(*m_pBmpList6).AddMasked((short)IDB_CMBI_AUTHENTICATION_LOGINPASS, m_CRColor);	// 2
	(*m_pBmpList6).AddMasked((short)IDB_CMBI_AUTHENTICATION_USERNUMBER, m_CRColor);	// 3
	(*m_pBmpList6).AddMasked((short)IDB_CMBI_AUTHENTICATION_SSO, m_CRColor);		// 4

	//<S><D>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	/*m_rcItem6.top = lSubopt_6_TOP + m_lSubOptOffset_Y;
	m_rcItem6.left = lSubopt_6_LEFT + m_lSubOptOffset_X;
	m_rcItem6.bottom = m_rcItem6.top + m_lIconHeight;
	m_rcItem6.right = m_rcItem6.left + m_lIconWidth;*/
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
EXIT:
	return;
}

//=============================================================================
// function
//      drawColorBkImg
//
// return value
//     LONG
//
// outline
//      Draw Color Back Ground Image.
//=============================================================================
LONG CMimicImage::drawColorBkImg(HDC hDC)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	LONG			lRet = 0;
	if(nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImage::drawColorBkImg -> NULL_POINTER(m_pBitMap)\n");
		return lRet;
	}
	SHORT wID = GetColorMimicID() ;
	m_pBitMap->load(wID);
	HDC hDCMem = CreateCompatibleDC(hDC);
	if(nullptr == hDCMem)
	{
		slog.putLog("CMimicImage::drawColorBkImg -> NULL_HANDLE(hDCMem)\n");
		return lRet;
	}
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());
	LONG lStartX = (m_lBBoxW - m_pBitMap->width()) / 2;
	LONG lStartY = MM_BKCOL_Y;
	::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width(), m_pBitMap->height(), hDCMem, 0, 0, SRCCOPY);
	::SelectObject(hDCMem, hbmOld);
	lRet = m_pBitMap->height();
	ReleaseResources(hDCMem);
	return lRet ;
}
//=============================================================================
// function
//      drawPaperImg
//
// return value
//     VOID
//
// outline
//      Draw Paper Image.
//=============================================================================
VOID CMimicImage::drawPaperImg(PPROPSTATE pps,HDC hDC,long lBkWidth)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImage::drawPaperImg -> NULL_POINTER(m_pBitMap)\n");
		return ;
	}
	HDC hDCMem = CreateCompatibleDC(hDC);
	if(nullptr == hDCMem)
	{
		slog.putLog("CMimicImage::drawPaperImg -> NULL_HANDLE(hDCMem)\n");
		return ;
	}
	SHORT wOri = GetOrientation(m_pFeatureInfoList);
	UINT NupValue = atol(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption);
	
	SHORT wNupOrder = GetNupOrderIndex();
	
	SHORT wID = 0;
	SHORT wFTPSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	SHORT wPaper = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	SHORT wFTP = (wFTPSize == 0) ? ftp_not : ftp_ftp;
	SHORT wBinEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	UINT wZoomBasing =  m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ZOOM_BASING].uChangedOptionIndex;
	if((ftp_not != m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex)&&
		(nup_1 == NupValue)
		)
	{
		// =================================
		// _/_/_/  Booklet Standard 
		// =================================
		if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
		{
			wOri = (wOri == ori_port) ? ori_land : ori_port;
		}
		if (0 == strcmp("ON", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption))
		{
			wFTP = ftp_xyzoom;
		}
		else if (0 == strcmp("OFF", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption))
		{
			wFTP = ftp_ftp;
		}
		
		if (wFTP == ftp_ftp)
		{
			wID = (wOri == ori_port) ? IDB_MM_FTP_P : IDB_MM_FTP_L;
		}
		else
		{
			if (wZoomBasing == zoom_basing_upperleft)
			{	// upper left
				wID = (wOri == ori_port) ? IDB_MM_ZOOM_P : IDB_MM_ZOOM_L;
			}
			else
			{	// center
				wID = (wOri == ori_port) ? IDB_MM_ZOOM_C_P : IDB_MM_ZOOM_C_L;
			}
		}
		if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
		{
			wID = (wOri == ori_port) ? IDB_MM_PAMPH_P : IDB_MM_PAMPH_L;
			wID += 100 * wBinEdge;
		}
	}
	else 
	{
	    if (pamph_none == m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex)
		{
			wOri = GetOrientation(m_pFeatureInfoList);
			wID = IDB_MM_1UP_P;
			if (ori_land == wOri)
			{
				wID = IDB_MM_1UP_L;
			}
			if (nup_1 != NupValue)
			{
				wID += (NupValue * 10);
			}
			if (0 == strcmp("ON", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BORDER].pszOption))
			{
				wID += 1000;
			}
			if (nup_order_randd <= wNupOrder)
			{
				wID += wNupOrder - nup_order_randd;
			}
			else if ((nup_order_ltor <= wNupOrder) &&
				(nup_order_ttob > wNupOrder)
				)
			{
				wID += wNupOrder - nup_order_ltor;
			}
		}
		else
		{
			wOri = GetOrientation(m_pFeatureInfoList);
			wID = (wOri == ori_port) ? IDB_MM_PAMPH_L : IDB_MM_PAMPH_P;
			wID += 100 * wBinEdge;
		}
	}
	m_pBitMap->load(wID);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());
	//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	//TThe spacing and positioning of Output image and icons are not proper
	LONG lResolution = GetDeviceCaps(hDCMem, LOGPIXELSY);
	INT iScaling = static_cast<INT>(100.0 * lResolution / 96);
	LONG lBkcol_X = MM_BKCOL_X;
	LONG lWidth = m_pBitMap->width() / 2;
	LONG lHeight = m_pBitMap->height();
	//<S><C>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	//SetPaperWidth_Height(iScaling, (m_pBitMap->width() / 2), m_pBitMap->height(), &lWidth, &lHeight, &lBkcol_X);

	//LONG lStartX = MM_BKCOL_X + ((lBkWidth - (m_pBitMap->width() / 2)) / 2);
	LONG lStartX = lBkcol_X + ((lBkWidth - (m_pBitMap->width() / 2)) / 2);
	int iDPIPer = GetScreenResolution();
	lStartX = (lStartX * iDPIPer) / 100;
	long lResizedHeight = (m_pBitMap->height() * iDPIPer) / 100;
	//LONG lStartY =  ((m_lBBoxH - lHeight) / 2);
	LONG lStartY = ((m_lBBoxH - lResizedHeight) / 2);
	
	int wDest = (m_pBitMap->width() / 2 * iDPIPer) / 100;
	int hDest = (m_pBitMap->height() * iDPIPer) / 100;
	//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2 , m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
	//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2   , m_pBitMap->height() , hDCMem, 0, 0, SRCAND);
	/*::StretchBlt(hDC, lStartX, lStartY, 0, 0, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCPAINT);
	::StretchBlt(hDC, lStartX, lStartY, lWidth, lHeight, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCAND);*/
	::StretchBlt(hDC, lStartX, lStartY, 0, 0, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCPAINT);
	::StretchBlt(hDC, lStartX, lStartY, wDest, hDest, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCAND);
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	::SelectObject(hDCMem, hbmOld);
	//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla

	// ================================================
	// _/_/_/  Margin Shift
	// ================================================
	drawMarginShiftImg(hDC, m_pPrinterName,lBkWidth);

	// ================================================
	// _/_/_/  Staple
	// ================================================
	drawStapleImg(hDC, m_pPrinterName, lBkWidth);

	// ================================================
	// _/_/_/  Punch
	// ================================================
	if(punch_none != m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex)
	{
		wID  = MM_PUNCH_BASEID;
		//<S><A>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla
		//Mimic is not displayed correctly when Punch is set to 2 / 3 in Configuration tab
		if (Is3holesPunchCase(pps, m_pPrinterName) == TRUE)
			wID += (m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PUNCH].uChangedOptionIndex * 100);
		else 
			wID += (punchtype_1 * 100);
		//<E>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla
		if(ori_land == wOri)
		{
			wID += 10;
		}
		wID += m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex ;
		m_pBitMap->load(wID);
		//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
		//TThe spacing and positioning of Output image and icons are not proper
		LONG lBkcol_X = MM_BKCOL_X;
		LONG lWidth = m_pBitMap->width() / 2;
		LONG lHeight = m_pBitMap->height();
		//<S><C>To Implement Task#3117,08-08-2024,SSDI:Manoj S
		//SetPaperWidth_Height(iScaling, (m_pBitMap->width() / 2), m_pBitMap->height(), &lWidth, &lHeight, &lBkcol_X);

		hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());
		//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
		//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, 0, 0, SRCAND);
		//::StretchBlt(hDC, lStartX, lStartY, 0, 0, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCPAINT);
		//::StretchBlt(hDC, lStartX, lStartY, lWidth, lHeight, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCAND);
		::StretchBlt(hDC, lStartX, lStartY, 0, 0, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCPAINT);
		::StretchBlt(hDC, lStartX, lStartY, wDest, hDest, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCAND);
		//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
		::SelectObject(hDCMem, hbmOld);
		//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	}

	// ================================================
	// _/_/_/  WaterMark
	// ================================================
	//m_WaterMarkIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
	//if(0 != m_WaterMarkIndex)
	if ((nullptr != m_pWaterMarkIndex) && (0 != *m_pWaterMarkIndex))
	{
		wID  = IDB_WATERMARK;
		m_pBitMap->load(wID);
		
		//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
		//TThe spacing and positioning of Output image and icons are not proper
		LONG lBkcol_X = MM_BKCOL_X;
		LONG lWidth = m_pBitMap->width() / 2;
		LONG lHeight = m_pBitMap->height();
		//<S><C>To Implement Task#3117,08-08-2024,SSDI:Manoj S
		//SetPaperWidth_Height(iScaling, (m_pBitMap->width() / 2), m_pBitMap->height(), &lWidth, &lHeight, &lBkcol_X);

		hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());
		//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
		//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, 0, 0, SRCAND);
		/*::StretchBlt(hDC, lStartX, lStartY, 0, 0, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCPAINT);
		::StretchBlt(hDC, lStartX, lStartY, lWidth, lHeight, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCAND);*/
		::StretchBlt(hDC, lStartX, lStartY, 0, 0, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCPAINT);
		::StretchBlt(hDC, lStartX, lStartY, wDest, hDest, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCAND);
		//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
		::SelectObject(hDCMem, hbmOld);
		//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	}
	else
	{
		slog.putLog("CMimicImage::drawPaperImg -> NULL_POINTER\n");
	}
	ReleaseResources(hDCMem);
}
//=============================================================================
// function
//      drawDuplexImg
//
// return value
//     VOID
//
// outline
//      Draw Duplex Image.
//=============================================================================
VOID CMimicImage::drawDuplexImg(HDC hDC,long lBkWidth)
{
#if 0
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImage::drawDuplexImg -> NULL_POINTER(m_pBitMap)\n");
		return ;
	}
	HDC hDCMem = CreateCompatibleDC(hDC);
	if(nullptr == hDCMem)
	{
		slog.putLog("CMimicImage::drawDuplexImg -> NULL_HANDLE(hDCMem)\n");
		return ;
	}
	SHORT wID = IDB_ARROW_STD;
	SHORT wOri = GetOrientation(m_pFeatureInfoList);
	SHORT wBindingEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	//UINT NupValue = atol(m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PAGESPERSHEET].pszOption);
	
    //<S><C> RedMine Ticket(#2086): Mimic displays wrong when ì2-Up Pamphletî and Landscape is set: 2015.04.14,SSDI:Sonali 
    //Mimic id should be updated even when Pamphlet is either tiled or 2-up
	if((binedge_top == wBindingEdge) &&( printbothsides_none != m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex) &&
        (pamph_none == m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAMPHLET].uChangedOptionIndex))
    //<E> RedMine Ticket(#2086): Mimic displays wrong when ì2-Up Pamphletî and Landscape is set: 2015.04.14,SSDI:Sonali 
	{
		wID += wOri;
	}
	m_pBitMap->load(wID);
	//LONG lStartX = (m_lBBoxW - (m_pBitMap->width() / 2)) / 2;
	//LONG lStartY = MM_BKCOL_Y + lBkHeight;

	LONG lStartX = MM_BKCOL_X + lBkWidth;
	LONG lStartY = (m_lBBoxH - m_pBitMap->height()) / 2;

    //<S><C> RedMine Ticket(#2086): Mimic displays wrong when ì2-Up Pamphletî and Landscape is set: 2015.04.14,SSDI:Sonali 
    //Mimic id should be updated even when Pamphlet is either tiled or 2-up
	if((binedge_top == wBindingEdge) &&( printbothsides_none != m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex) &&
        (pamph_none == m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAMPHLET].uChangedOptionIndex))
    //<E> RedMine Ticket(#2086): Mimic displays wrong when ì2-Up Pamphletî and Landscape is set: 2015.04.14,SSDI:Sonali 
	{ 
		lStartX -= 10;
	}
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());
	::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
	::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, 0, 0, SRCAND);
	::SelectObject(hDCMem, hbmOld);
	lStartY += m_pBitMap->height();
	lStartX = 0;
	if(pamph_none == m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAMPHLET].uChangedOptionIndex)
	{
		switch(m_pFeatureInfoList->pFeatureOptionsList[FEATURE_DUPLEX].uChangedOptionIndex)
		{
			case printbothsides_flip_long_edge: 
				{
					if(ori_port == wOri)
					{
						wID = IDB_2SIDED_P_L;
						wID += wBindingEdge;
						lStartX = -13;
						lStartY -= 10;
					}
					else
					{
						wID = IDB_2SIDED_L_T;
						lStartY -= 29;
					}
				}
				break;
			case printbothsides_flip_short_edge: 
				{
					if(ori_port == wOri)
					{
						wID = IDB_2SIDED_P_T;
						lStartY -= 36;
					}
					if(ori_land == wOri)
					{
				
						wID = IDB_2SIDED_L_L ;
						wID += wBindingEdge;
						lStartX = -14;
						lStartY -= 6;
					}
				}
				break;
			case printbothsides_none: 
			default:
				{
					if(ori_port == wOri)
					{
						wID = IDB_1SIDED_P;
						lStartX = 2;
						lStartY -= 19;
					}
					else
					{
						wID = IDB_1SIDED_L;
						lStartX = 2;
						lStartY -= 16;
					}
				}
				break;
		}
	}
	else
	{
		wID  = IDB_PAMPH_L;
		wID += wBindingEdge;
		lStartY -= 8;

        //<S><C> RedMine Ticket(#2086): Issue#2: 2015.05.12,SSDI:Sonali 
		//if(ori_port == wOri)
        if(binedge_top == wBindingEdge)
        //Updating X and Y position based on Binding Edge
        //<E> RedMine Ticket(#2086): Issue#2: 2015.05.12,SSDI:Sonali 
		{
			lStartX += 6;
			lStartY += 2;
		}
	}
	m_pBitMap->load(wID);
	hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());
	lStartX += (m_lBBoxW - (m_pBitMap->width() / 2)) / 2;
	::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
	::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, 0, 0, SRCAND);
	::SelectObject(hDCMem, hbmOld);
	ReleaseResources(hDCMem);
#endif
}
#if 0
//=============================================================================
// function
//      drawSubOptionImg
//
// return value
//     VOID
//
// outline
//      Draw images like copies/collate/pamphlet.
//=============================================================================
VOID CMimicImage::drawSubOptionImg(HDC hDC)
{

	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImage::drawSubOptionImg -> NULL_POINTER(m_pBitMap)\n");
		return ;
	}
	HDC hDCMem = CreateCompatibleDC(hDC);
	if(nullptr == hDCMem)
	{
		slog.putLog("CMimicImage::drawSubOptionImg -> NULL_HANDLE(hDCMem)\n");
		return ;
	}
	SHORT			wID = 0;
	// ================================================
	// _/_/_/  collate
	// ================================================
	wID = IDB_UNCOLLATE;
	SHORT wCollateIndex = (0 == m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE].uChangedOptionIndex)? 1: 0;



//<S><C>   2020.01.23, SSDI:Goutham
    //if(NULL != GetDlgItem(m_hParentHandle,IDC_CMB_COLLATE))
	if(NULL != GetDlgItem(m_hParentHandle,IDC_CMB_COLLATE))
//<E>   2020.01.23, SSDI:Goutham
	{
//<S><C>   2020.01.23, SSDI:Goutham
        //wCollateIndex = IsDlgButtonChecked(m_hParentHandle,IDC_CHK_COLLATE);
		wCollateIndex = IsDlgButtonChecked(m_hParentHandle,IDC_CMB_COLLATE);
//<E>   2020.01.23, SSDI:Goutham
	}
	wID += wCollateIndex;
	m_pBitMap->load(wID);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());
	LONG lStartX = m_lBBoxW - m_pBitMap->width() / 2 - MM_SUBOPTL_X;
	LONG lStartY = MM_SUBOPTL_Y + 70;
	::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
	::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, 0, 0, SRCAND);
	::SelectObject(hDCMem, hbmOld);
	if(pamph_none != m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAMPHLET].uChangedOptionIndex)
	{
		wID = IDB_PAMPH_NORMAL;
		m_pBitMap->load(wID);
		hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());
		lStartX = m_lBBoxW - m_pBitMap->width() / 2 - MM_SUBOPTL_X;
		lStartY = MM_SUBOPTL_Y + 30;
		::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
		::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, 0, 0, SRCAND);
		::SelectObject(hDCMem, hbmOld);
	}
	ReleaseResources(hDCMem);

}
#endif
	//<S><A>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
//=============================================================================
// function
//      drawSubOptionImg
//
// return value
//     VOID
//
// outline
//      Draw images like copies/collate/pamphlet.
//=============================================================================
void CMimicImage::drawSubOptionImg(PPROPSTATE pps, HDC hDC, RECT *prcItem, long lBkWidth)
{
	short				wImgIndex;
	long				lBBoxW;
	//<S><A>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	long				lBBoxH;
	long				lStartX = 0;
	long				lStartY = 0;
//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	if (pps == NULL || prcItem == NULL)
		return;
	//<S><A>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	int iDPIPer = GetScreenResolution();
	lBBoxW = (*prcItem).right - (*prcItem).left;
	lBBoxH = (*prcItem).bottom - (*prcItem).top;
	long lBBoxHalfW = lBBoxW / 2;
	lStartX = lBBoxHalfW + (lBBoxHalfW - m_lResizedIconWidth * 3 - MMC_SUBOPT_MARGIN_WIDTH * 2) / 2;
	lStartY = (lBBoxH - m_lResizedIconHeight * 3 - MMC_SUBOPT_MARGIN_HEIGHT * 2) / 2;

	m_rcItem1.top = lStartY;
	m_rcItem1.left = lStartX;
	m_rcItem1.bottom = m_rcItem1.top + m_lResizedIconHeight;
	m_rcItem1.right = m_rcItem1.left + m_lResizedIconWidth;

	m_rcItem2.top = m_rcItem1.bottom + MMC_SUBOPT_MARGIN_HEIGHT;
	m_rcItem2.left = m_rcItem1.left;
	m_rcItem2.bottom = m_rcItem2.top + m_lResizedIconHeight;
	m_rcItem2.right = m_rcItem2.left + m_lResizedIconWidth;

	m_rcItem3.top = m_rcItem2.bottom + MMC_SUBOPT_MARGIN_HEIGHT;
	m_rcItem3.left = m_rcItem2.left;
	m_rcItem3.bottom = m_rcItem3.top + m_lResizedIconHeight;
	m_rcItem3.right = m_rcItem3.left + m_lResizedIconWidth;

	m_rcItem4.top = m_rcItem1.top;
	m_rcItem4.left = m_rcItem1.right + MMC_SUBOPT_MARGIN_WIDTH;
	m_rcItem4.bottom = m_rcItem4.top + m_lResizedIconHeight;
	m_rcItem4.right = m_rcItem4.left + m_lResizedIconWidth;

	m_rcItem5.top = m_rcItem4.bottom + MMC_SUBOPT_MARGIN_HEIGHT;
	m_rcItem5.left = m_rcItem2.right + MMC_SUBOPT_MARGIN_WIDTH;
	m_rcItem5.bottom = m_rcItem5.top + m_lResizedIconHeight;
	m_rcItem5.right = m_rcItem5.left + m_lResizedIconWidth;

	m_rcItem6.top = m_rcItem5.bottom + MMC_SUBOPT_MARGIN_HEIGHT;
	m_rcItem6.left = m_rcItem3.right + MMC_SUBOPT_MARGIN_WIDTH;
	m_rcItem6.bottom = m_rcItem6.top + m_lResizedIconHeight;
	m_rcItem6.right = m_rcItem6.left + m_lResizedIconWidth;
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	// ================================================
	// _/_/_/  Collate(1)
	// ================================================
	SHORT wCollateIndex = (0 == m_pFeatureInfoList->pFeatureOptionsList[FEATURE_COLLATE].uChangedOptionIndex) ? 1 : 0;
	(*m_pBmpList1).draw(hDC, &m_rcItem1, wCollateIndex);

	// ================================================
	// _/_/_/  Color Mode(2)
	// ================================================
	SHORT wColMode = GetColModeValue();

	(*m_pBmpList2).draw(hDC, &m_rcItem2, (wColMode - 1));

	// ================================================
	// _/_/_/  2-Sided Pamphlet(3)
	// ================================================
	SHORT wDuplex = GetDuplexValue();

	// if Booklet is Standard that time wDuplex should be duplex_pamphlet(4) 
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		wDuplex = duplex_pamphlet;
	}
	if (wDuplex != duplex_pamphlet) {
		(*m_pBmpList3).draw(hDC, &m_rcItem3, (wDuplex - 1));
	}
	else
	{
		(*m_pBmpList3).draw(hDC, &m_rcItem3, 3);
	}

	// ================================================
	// _/_/_/  Offset(4)
	// ================================================
	SHORT wNoOffset = GetOffsetValue();
	
	(*m_pBmpList4).draw(hDC, &m_rcItem4, wNoOffset - 1);


	// ================================================
	// _/_/_/  Print Release / Document Filing(5)
	// ================================================
	SHORT wPrintRelease =  m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PRINTRELEASE].uChangedOptionIndex;
	SHORT wRetentionMode = GetDocFillingValue();

		// Printing Policy - Print ReleaseÅ Document FilingÇ
	//<S><C>To Fix Redmine #7585,03-07-2024,SSDI:Jyothi
	if (((*pps).wPPlcyPrintRelease != bool_true && (*pps).wPPlcyRetention != bool_true)
		|| (*pps).wPPlcyPrintRelease == bool_true && (*pps).byPrintRelease == bool_true)
	//<E>To Fix Redmine #7585,03-07-2024,SSDI:Jyothi
	{
		if (0 != (*pps).byPrintRelease) {		//check with by print release flag also
			wPrintRelease = bool_false;
		}
		if (wPrintRelease == bool_true)
		{
			(*m_pBmpList5).draw(hDC, &m_rcItem5, 4);
		}
		else
		{
			(*m_pBmpList5).draw(hDC, &m_rcItem5, wRetentionMode);
		}
	}
	else
	{
		(*m_pBmpList5).draw(hDC, &m_rcItem5, 0);
	}
		
	
	// ================================================
		// _/_/_/  Authentication(6)
	// ================================================
	wImgIndex = GetAuthenticationValue();

	(*m_pBmpList6).draw(hDC, &m_rcItem6, wImgIndex);

	return;
}
//=============================================================================
// function
//       drawMarginShiftImg
//
// parameters
//      WCHAR					pPrinterName
//
// return value
//      None
//
// outline
//       Update Mimic Margin Shift
//=============================================================================
VOID CMimicImage::drawMarginShiftImg(HDC hDC, WCHAR FAR *pPrinterName, long lBkWidth)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImage::drawMarginShiftImg -> NULL_POINTER(m_pBitMap)\n");
		return;
	}
	HDC hDCMem = CreateCompatibleDC(hDC);
	if (nullptr == hDCMem)
	{
		slog.putLog("CMimicImage::drawMarginShiftImg -> NULL_HANDLE(hDCMem)\n");
		return;
	}
	BOOL				blRet = FALSE;
	MARGINSHIFTDATA		msd;
	CShRegMS FAR		*pregms = NULL;
	SecureZeroMemory(&msd, sizeof(MARGINSHIFTDATA));
	DWORD dwMarginWidth = 0;
	DWORD wMarginUnit = 0;
	SHORT wMargin = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_MARGINSHIFT].uChangedOptionIndex;

	SHORT wOri = GetOrientation(m_pFeatureInfoList);
	SHORT wID = 0;
	SHORT wBinEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
	CShJsonMS	*pjsonms = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonms = new CShJsonMS(ghInstance, pPrinterName);
		pjsonms->Init();
	}

	pregms = new CShRegMS(m_hStringResourceHandle, pPrinterName);
	if (pregms == NULL)
	{
		goto EXIT;
	}

	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		blRet = (*pjsonms).ReadMSData(wMargin, &msd);
		if (blRet != TRUE)
		{
			(*pjsonms).GetMSDefData(wMargin, &msd);
		}
	}
	else
	{
		blRet = (*pregms).ReadMSData(m_pPrinterName, wMargin, &msd);
		if (blRet != TRUE)
		{
			(*pregms).GetMSDefData(wMargin, &msd);
		}
	}

	wMarginUnit = msd.dwUnit;
	if (wMarginUnit == unit_inch)
	{
		dwMarginWidth = msd.dwInch;
	}
	else
	{
		dwMarginWidth = msd.dwMilli;
	}
	if ((wMargin != 0) && (dwMarginWidth != 0))
	{
		wID = MM_MGN_BASEID;
		if (GetOrientation(m_pFeatureInfoList) == ori_land)
			wID += 100;

		wID += (wBinEdge * 10);
		if (wMarginUnit == unit_inch)
		{
			wID += 1;
			if (dwMarginWidth > 4)
			{
				wID += 1;
			}
			if (dwMarginWidth > 8)
			{
				wID += 1;
			}
		}
		else
		{
			wID += 1;
			if (dwMarginWidth > 10)
			{
				wID += 1;
			}
			if (dwMarginWidth > 20)
			{
				wID += 1;
			}
		}
		m_pBitMap->load(wID);
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());
		//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
		//TThe spacing and positioning of Output image and icons are not proper
		LONG lResolution = GetDeviceCaps(hDCMem, LOGPIXELSY);
		INT iScaling = static_cast<INT>(100.0 * lResolution / 96);
		LONG lBkcol_X = MM_BKCOL_X;
		LONG lWidth = m_pBitMap->width() / 2;
		LONG lHeight = m_pBitMap->height();
		//<S><C>To Implement Task#3117,08-08-2024,SSDI:Manoj S
		//SetPaperWidth_Height(iScaling, (m_pBitMap->width() / 2), m_pBitMap->height(), &lWidth, &lHeight, &lBkcol_X);

		LONG lStartX = lBkcol_X + ((lBkWidth - (m_pBitMap->width() / 2)) / 2);
		//LONG lStartY = ((m_lBBoxH - lHeight) / 2);
		int iDPIPer = GetScreenResolution();

		lStartX = (lStartX * iDPIPer) / 100;
		long lResizedHeight = (m_pBitMap->height() * iDPIPer) / 100;
		//LONG lStartY =  ((m_lBBoxH - lHeight) / 2);
		LONG lStartY = ((m_lBBoxH - lResizedHeight) / 2);

		int wDest = (m_pBitMap->width() / 2 * iDPIPer) / 100;
		int hDest = (m_pBitMap->height() * iDPIPer) / 100;

		//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
		//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, 0, 0, SRCAND);
		::StretchBlt(hDC, lStartX, lStartY, 0, 0, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCPAINT);
		::StretchBlt(hDC, lStartX, lStartY, wDest, hDest, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCAND);
		//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
		::SelectObject(hDCMem, hbmOld);
		//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	}
	
EXIT:
	ReleaseResources(hDCMem);
	if (pregms != NULL) {
		delete pregms;
		pregms = NULL;
	}
	if (pjsonms != NULL) {
		delete pjsonms;
		pjsonms = NULL;
	}
	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = NULL;
	}
	return;
}
//=============================================================================
// function
//      GetColModeValue
//
// return value
//     SHORT
//
// outline
//      Get Colmode Value.
//=============================================================================
SHORT CMimicImage::GetColModeValue()
{
	SHORT wColMode = colmode_auto;
	if (!strcmp("Auto", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption))
	{
		wColMode = colmode_auto;
	}
	else if (!strcmp("Color", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption))
	{
		wColMode = colmode_color;
	}
	else if (!strcmp("Mono", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption))
	{
		wColMode = colmode_mono;
	}
	return wColMode;
}
//=============================================================================
// function
//      GetOffsetValue
//
// return value
//     SHORT
//
// outline
//      Get offset Value.
//=============================================================================
SHORT CMimicImage::GetOffsetValue()
{
	SHORT wNoOffset = offset_offset_each_set;
	if (!strcmp("Offset_each_set", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OFFSET].pszOption))
	{
		wNoOffset = offset_offset_each_set;
	}
	else if (!strcmp("None", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_OFFSET].pszOption))
	{
		wNoOffset = offset_none;
	}
	return wNoOffset;
}
//=============================================================================
// function
//      GetAuthenticationValue
//
// return value
//     SHORT
//
// outline
//      Get Authentication Value.
//=============================================================================
SHORT CMimicImage::GetAuthenticationValue()
{
	SHORT wImgIndex = authentication_none;

	if (!strcmp("None", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption))
	{
		wImgIndex = authentication_none;
	}
	else if (!strcmp("LoginName", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption))
	{
		wImgIndex = authentication_lname;
	}
	else if (!strcmp("LoginPass", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption))
	{
		wImgIndex = authentication_lname_pswd;
	}
	else if (!strcmp("UserNumber", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption))
	{
		wImgIndex = authentication_acnum;
	}
	else if (!strcmp("LoginNameSSO", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_AUTHENTICATION].pszOption))
	{
		wImgIndex = authentication_sso;
	}
	return wImgIndex;
}
//=============================================================================
// function
//      GetDocFillingValue
//
// return value
//     SHORT
//
// outline
//      Get DocFilling Value. 
//=============================================================================
SHORT CMimicImage::GetDocFillingValue()
{

	SHORT wRetentionMode = retention_normal;

	if (!strcmp("NONE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption))
	{
		wRetentionMode = retention_normal;
	}
	else if (!strcmp("HOLDONLY", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption))
	{
		wRetentionMode = retention_hold_before;
	}
	else if (!strcmp("HOLDAFTERPRINT", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption))
	{
		wRetentionMode = retention_hold_after;
	}
	else if (!strcmp("SAMPLEPRINT", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_RETENTION_SETTINGS].pszOption))
	{
		wRetentionMode = retention_sample;
	}
	return wRetentionMode;
}
//=============================================================================
// function
//      GetDuplexValue
//
// return value
//     SHORT
//
// outline
//      Get Duplex Value.
//=============================================================================
SHORT CMimicImage::GetDuplexValue()
{
	SHORT wDuplex = DMDUP_VERTICAL;
	if (!strcmp("NONE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
	{
		wDuplex = DMDUP_SIMPLEX;
	}
	else if (!strcmp("VERTICAL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
	{
		wDuplex = DMDUP_VERTICAL;
	}
	else if (!strcmp("HORIZONTAL", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_DUPLEX].pszOption))
	{
		wDuplex = DMDUP_HORIZONTAL;
	}
	return wDuplex;
}
//=============================================================================
// function
//      GetPunchValue
//
// return value
//     SHORT
//
// outline
//     Get Punch Value
//=============================================================================
SHORT CMimicImage::GetPunchValue()
{
	SHORT wPunch = PUNCH_NONE;
	if (!strcmp("OFF", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption))
	{
		wPunch = PUNCH_NONE;
	}
	else if (!strcmp("2HOLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption))
	{
		wPunch = PUNCH_2HOLES;
	}
	else if (!strcmp("3HOLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption))
	{
		wPunch = PUNCH_3HOLES;
	}
	else if (!strcmp("4HOLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption))
	{
		wPunch = PUNCH_4HOLES;
	}
	else if (!strcmp("4HOLE_WIDE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_PUNCH].pszOption))
	{
		wPunch = PUNCH_4HOLES_WIDE;
	}
	return wPunch;
}
//=============================================================================
// function
//      GetStapleValue
//
// return value
//     SHORT
//
// outline
//     Get Staple Value
//=============================================================================
SHORT CMimicImage::GetStapleValue()
{
	SHORT wStaple = staple_none;
	if (!strcmp("NONE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
	{
		wStaple = staple_none;
	}
	else if (!strcmp("STAPLE_1", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
	{
		wStaple = staple_1;
	}
	else if (!strcmp("STAPLE_2", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
	{
		wStaple = staple_2;
	}
	else if (!strcmp("STAPLE_STAPLELESS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
	{
		wStaple = 8;
	}
	else if (!strcmp("STAPLE_SADDLE", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
	{
		wStaple = 9;
	}
	return wStaple;
}
//=============================================================================
// function
//      GetNupOrderIndex
//
// return value
//     SHORT
//
// outline
//      Gets Nup Order Index.
//=============================================================================
SHORT CMimicImage::GetNupOrderIndex()
{
	SHORT wNupOrder = nup_order_none;
	SHORT wNupOrderIndex = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_ORDER].uChangedOptionIndex;
	
	switch(wNupOrderIndex)
	{
	case	eFirstOption :	//  --LeftToRight
		wNupOrder = nup_order_ltor;					//0	 
		break;
	case	eSecondOption:	// --RightToLeft
		wNupOrder = nup_order_rtol;					// 2
		break;
	case	eThirdOption: // --TopToBottom
		wNupOrder = nup_order_ttob;					// 3
		break;
	case 	eFourthOption: // --RightAndDown
		wNupOrder = nup_order_randd;				// 4
		break;
	case	eFifthOption: // --DownAndRight
		wNupOrder = nup_order_dandr;				// 5
		break;
	case	eSixthOption: // --LeftAndDown
		wNupOrder = nup_order_landd;				// 6
		break;
	case	eSevenOption: // --DownAndLeft
		wNupOrder = nup_order_dandl;				// 7
		break;
	default:
		wNupOrder = nup_order_none;
		break;
	}
	
	return wNupOrder;

}
//=============================================================================
// function
//      GetStapleMimicID
//
// return value
//     SHORT
//
// outline
//      Gets Bitmap ID for Staple feature .
//=============================================================================
SHORT CMimicImage::GetStapleMimicID(SHORT *wId)
{
	SHORT bRet = FALSE;
	*wId = IDB_MM_STP_P_P;
	SHORT wBinEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	if (!strcmp("STAPLE_1", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption)
		|| !strcmp("STAPLE_2", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
	{
		bRet = FALSE;
		return bRet ;
	}
	else if (!strcmp("STAPLE_STAPLELESS", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_STAPLING].pszOption))
	{
		if (GetOrientation(m_pFeatureInfoList) == ori_port)
		{
			*wId = (wBinEdge == binedge_right) ? IDB_MM_STP_LESS_P_R : IDB_MM_STP_LESS_P_L; 
		}
		else
		{
			*wId = (wBinEdge == binedge_left) ? IDB_MM_STP_LESS_L_L : IDB_MM_STP_LESS_L_R;
		}
		bRet = TRUE;
		return bRet;
	}
	//Fixed Coverity #91075 ,20200112,SSDI;Chanchal Singla
	return bRet;
}
//=============================================================================
// function
//       drawStapleImg
//
// parameters
//      WCHAR					pPrinterName
//
// return value
//      None
//
// outline
//       Update Mimic Staple Image
//=============================================================================
VOID CMimicImage::drawStapleImg(HDC hDC, WCHAR FAR *pPrinterName, long lBkWidth)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImage::drawStapleImg -> NULL_POINTER(m_pBitMap)\n");
		return;
	}
	HDC hDCMem = CreateCompatibleDC(hDC);
	if (nullptr == hDCMem)
	{
		slog.putLog("CMimicImage::drawStapleImg -> NULL_HANDLE(hDCMem)\n");
		return;
	}
	SHORT wOri = GetOrientation(m_pFeatureInfoList);
	SHORT wID = 0;
	SHORT wBinEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;

	if (staple_saddle_staple == m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex)
	{
		wID = IDB_MM_STP_P_P;
		if (GetOrientation(m_pFeatureInfoList) == ori_land)
			wID += 1;
	}
	else if (staple_none != m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex)
	{
		wID = IDB_MM_STP_P_P;
		if (pamph_none != m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BOOKLET].uChangedOptionIndex)
		{
			if (ori_land == wOri)
			{
				wID += 1;
			}
		}
		else
		{
			//<S><A>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla
			//Mimic is not displayed correctly when Punch is set to 2 / 3 in Configuration tab
			SHORT wStapletemp = GetStapleValue();
			//if (GetStapleMimicID(&wID) != TRUE)
			if (IsY7Y8Case(m_pPrinterName, &wID) != TRUE)
			//<E>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla
			{
				wID += wStapletemp * 100;
				if (ori_land == wOri)
				{
					wID += 10;
				}
				wID += m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
			}
		}
	}

	m_pBitMap->load(wID);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCMem, m_pBitMap->handle());

	//<S><C>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	//TThe spacing and positioning of Output image and icons are not proper
	LONG lResolution = GetDeviceCaps(hDCMem, LOGPIXELSY);
	INT iScaling = static_cast<INT>(100.0 * lResolution / 96);
	LONG lBkcol_X = MM_BKCOL_X;
	LONG lWidth = m_pBitMap->width() / 2;
	LONG lHeight = m_pBitMap->height();
	//<S><C>To Implement Task#3117,08-08-2024,SSDI:Manoj S
	//SetPaperWidth_Height(iScaling, (m_pBitMap->width() / 2), m_pBitMap->height(), &lWidth, &lHeight, &lBkcol_X);

	LONG lStartX = lBkcol_X + ((lBkWidth - (m_pBitMap->width() / 2)) / 2);
	//LONG lStartY = (m_lBBoxH - m_pBitMap->height()) / 2;
	//LONG lStartY = ((m_lBBoxH - lHeight) / 2);
	int iDPIPer = GetScreenResolution();

	lStartX = (lStartX * iDPIPer) / 100;
	long lResizedHeight = (m_pBitMap->height() * iDPIPer) / 100;
	//LONG lStartY =  ((m_lBBoxH - lHeight) / 2);
	LONG lStartY = ((m_lBBoxH - lResizedHeight) / 2);

	int wDest = (m_pBitMap->width() / 2 * iDPIPer) / 100;
	int hDest = (m_pBitMap->height() * iDPIPer) / 100;
	//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
	//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2, m_pBitMap->height(), hDCMem, 0, 0, SRCAND);
	::StretchBlt(hDC, lStartX, lStartY, 0, 0, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCPAINT);
	::StretchBlt(hDC, lStartX, lStartY, wDest, hDest, hDCMem, 0, 0, m_pBitMap->width() / 2, m_pBitMap->height(), SRCAND);
	//<E>To Implement Task #3117,08-08-2024,SSDI:Manoj S
	::SelectObject(hDCMem, hbmOld);
	ReleaseResources(hDCMem);
	//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
	return;
}

	//<E>To the Support of mimic Control According to UD3.1,20201123,SSDI:Chanchal Singla
//=============================================================================
// function
//      GetColorMimicID
//
// return value
//     SHORT
//
// outline
//      Gets Bitmap ID for Color feature .
//=============================================================================
SHORT CMimicImage::GetColorMimicID()
{
	if(0 == strcmp("Auto",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption))
	{
		return IDB_MM_BKAUTO ;
	}
	else if(0 == strcmp("Color",m_pFeatureInfoList->pFeatureOptionPair[FEATURE_COLOR_MODE].pszOption))
	{
		return IDB_MM_BK_COLOR ;
	}
	else 
	{
		return IDB_MMBK_GRAY ;
	}
}
//=============================================================================
// function
//      ReleaseResources
//
// return value
//     VOID
//
// outline
//  Releases all aquired resources.
//=============================================================================
VOID CMimicImage::ReleaseResources(	HDC hDCMem,
								   HBITMAP hbm,
									HDC hDCBM
									
							)
{
	if(nullptr != hDCMem )
	{
		DeleteDC(hDCMem);
		hDCMem = nullptr ;
	}
	if(nullptr != hbm )
	{
		DeleteObject(hbm);
		hbm = nullptr ;
	}
	if(nullptr != hDCBM )
	{
		DeleteDC(hDCBM);
		hDCBM = nullptr ;
	}
	
}
//<S><A>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla
//Mimic is not displayed correctly when Punch is set to 2 / 3 in Configuration tab
//===========================================================================
//	Function:	Is3holesPunchCase
//
// parameters :
//				WCHAR*						PrinterNAme               
//				
//
//	Return Type	:	TRUE  : 3-holes
//					FALSE : 2-holes
//
//Outline:
//			Is3holesPunchCase Function returns 2 hole(FLASE) or 3 hole(TRUE)
//===========================================================================
BOOL CMimicImage::Is3holesPunchCase(PPROPSTATE pps,WCHAR *pPrinterName)
{
	BOOL		blRet = TRUE;

	SHORT		bOri;
	long		lPaper;
	long 		pl;
	WCHAR		pszPath[8];

	SecureZeroMemory(pszPath, sizeof(pszPath));
	SHORT wBinEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	SHORT wFTPSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	SHORT wPaper = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	SHORT wPunchType = GetPunchValue();

	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	CShIniFile plim(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
	//3-holesñ
	if (wPunchType != punchtype_3)
		goto EXIT;



	bOri = GetOrientation(m_pFeatureInfoList);
	if (wFTPSize == OUTPUTPAPERSIZE_SAOS)
	{
		lPaper = PaperSizeArray[wPaper][0];
	}
	else
	{
		lPaper = PaperSizeArray[wFTPSize - 1][0];
	}
	::wsprintf(pszPath, L"%d", lPaper);

	pl = plim.GetLong(LIMTBL_SEC_PSIZE_PUNCH_BINEDGE_3HOLES, pszPath, lim_psb_no);

	switch (pl)
	{
	case lim_psb_se:
		if (bOri == ori_port)
		{
			if (wBinEdge == binedge_top)
				blRet = FALSE;
		}
		else
		{
			if (wBinEdge != binedge_top)
				blRet = FALSE;
		}
		break;

	case lim_psb_le:
		if (bOri == ori_port)
		{
			if (wBinEdge != binedge_top)
				blRet = FALSE;
		}
		else
		{
			if (wBinEdge == binedge_top)
				blRet = FALSE;
		}
		break;

	case lim_psb_lese:
		blRet = FALSE;
		break;

	default:
		break;
	}

EXIT:

	return blRet;

}
//===========================================================================
//	Function:	IsY7Y8Case
//
// parameters :
//				WCHAR*						PrinterNAme               
//				
//
//	Return Type	:	TRUE  : 3-holes
//					FALSE : 2-holes
//
//Outline:
//			IsY7Y8Case Function for staple cases
//===========================================================================
BOOL CMimicImage::IsY7Y8Case(WCHAR *pPrinterName, short *pwBitmapID)
{
	BOOL			blRet = FALSE;


	BOOL			*pbl;

	BOOL			bl0 = FALSE;
	BOOL			bl1 = FALSE;

	short			wBitmapID0 = IDB_MM_STP_P_P;
	short			wBitmapID1 = IDB_MM_STP_P_P;
	short			*pwBMPID;

	short			wLoop = 1;

	short			wPSize;
	short			wPaperSize;

	short			wSTPSize;

	short			wConvPSize1;
	short			wConvPSize2;
	short			ws, wb;

	short			wPos;
	bool bOri;
	short			i;

	long FAR		*pl = NULL;

	WCHAR 		*pSec = NULL;
	SHORT wBinEdge = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;
	SHORT wFTPSize = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_FITTOPAPERSIZE].uChangedOptionIndex;
	SHORT wPaper = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_PAPERSIZE].uChangedOptionIndex;
	SHORT wPunchType = GetPunchValue();

	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	CShIniFile pinilim(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
	SHORT wDuplex = GetDuplexValue();

	// if Booklet is Standard that time wDuplex should be duplex_pamphlet(4) 
	if (!strcmp("Standard", m_pFeatureInfoList->pFeatureOptionPair[FEATURE_BOOKLET].pszOption))
	{
		wDuplex = duplex_pamphlet;
	}

	ws = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_STAPLING].uChangedOptionIndex;
	wb = m_pFeatureInfoList->pFeatureOptionsList[FEATURE_BINDINGEDGE].uChangedOptionIndex;


	if (ws == staple_none)
		goto EXIT;

	bOri = GetOrientation(m_pFeatureInfoList);
	if (wFTPSize == OUTPUTPAPERSIZE_SAOS)
	{
		wPaperSize = PaperSizeArray[wPaper][0];
	}
	else
	{
		wPaperSize = PaperSizeArray[wFTPSize - 1][0];
	}

	wSTPSize = 0;

	wLoop = 2;
	wConvPSize1 = wPaperSize;
	wSTPSize = ConvertStdSizeToRSize(wPaperSize);



	for (i = 0; i < wLoop; i++)
	{
		if (i == 0)
		{
			pbl = &bl0;
			pwBMPID = &wBitmapID0;
			wPSize = wConvPSize1;
		}
		else
		{
			pbl = &bl1;
			pwBMPID = &wBitmapID0;
			wPSize = wSTPSize;
		}


		if (pinilim.GetLongArray(LIMTBL_SEC_PSIZE_STAPLE_BINEDGE, wPSize) != 0)
		{
			pl = pinilim.GetLongArrayData();
			if (pl != NULL)
			{
				switch (ws)
				{
				case staple_saddle_staple:
					wPos = 2;
					break;
				case staple_1_stapleless:
					wPos = 3;
					break;
				case staple_2:
					switch (wDuplex)
					{
					case duplex_pamphlet:
						wPos = 2;
						break;

					default:
						wPos = 1;
						break;
					}
					break;

				case staple_1:
				default:
					wPos = 0;
					break;
				}

				switch (pl[wPos])
				{
				case lim_psb_y7:
					*pbl = TRUE;
					if (GetOrientation(m_pFeatureInfoList) == ori_port)
					{
						*pwBMPID = (wb == binedge_left) ? IDB_MM_STP_LESS_P_L : IDB_MM_STP_LESS_P_R;
					}
					else
					{
						// LandscapeÇ
						*pwBMPID = (wb == binedge_right) ? IDB_MM_STP_LESS_L_R : IDB_MM_STP_LESS_L_L;
					}

					if (wPos != 3) {
						*pwBMPID = *pwBMPID - (IDB_MM_STP_LESS_P_L - IDB_MM_STP_1_P_L);
					}
					break;

				case lim_psb_y8:
					*pbl = TRUE;
					if (GetOrientation(m_pFeatureInfoList) == ori_port)
					{
						// Portrite
						*pwBMPID = (wb == binedge_right) ? IDB_MM_STP_LESS_P_R : IDB_MM_STP_LESS_P_L;
					}
					else
					{
						// Portrite
						*pwBMPID = (wb == binedge_left) ? IDB_MM_STP_LESS_L_L : IDB_MM_STP_LESS_L_R;
					}

					if (wPos != 3) {
						*pwBMPID = *pwBMPID - (IDB_MM_STP_LESS_P_L - IDB_MM_STP_1_P_L);
					}
					break;

				case lim_psb_no:
				case lim_psb_se:
				case lim_psb_se_ex:
				case lim_psb_le:
				case lim_psb_le_ex:
				case lim_psb_lese:
				case lim_psb_c:
				case lim_psb_yes:
				default:
					*pbl = FALSE;
					break;
				}
			}
		}
	}

EXIT:

	blRet = bl0;
	*pwBitmapID = wBitmapID0;

	return blRet;
}
//<E>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla

//<S><A>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
//TThe spacing and positioning of Output image and icons are not proper
//=============================================================================
// function
//      SetPaperWidth_Height
//
// Parameters:
//				INT iScaling
//				LONG lWidth
//				LONG lHeight
//				LONG *plWidth
//				LONG *plHeight
//				LONG *plStartX
//
// return value
//     BOOL
//
// outline
//			Set Paper Width and Height in Left mimic box based on scaling factor
//=============================================================================
BOOL CMimicImage::SetPaperWidth_Height(INT iScaling, LONG lWidth, LONG lHeight, LONG *plWidth, LONG *plHeight, LONG *plStartX)
{
	LONG lBkcol_X = MM_BKCOL_X;
	LONG l_Width = lWidth;
	LONG l_Height = lHeight;
	if (iScaling >= 100 && iScaling < 125) // 100- 125 %
	{
		lBkcol_X = MM_BKCOL_X;
		l_Width = lWidth;
		l_Height = lHeight;
	}
	else if (iScaling >= 125 && iScaling < 150) // 125- 150 %
	{
		lBkcol_X = MM_BKCOL_X + 10;
		l_Width = lWidth + 15;
		l_Height = lHeight + 15;
	}
	else if (iScaling >= 150 && iScaling < 175)// 150- 175 %
	{
		lBkcol_X = MM_BKCOL_X + 20;
		l_Width = (lWidth) + 30;
		l_Height = lHeight + 30;
	}
	else if (iScaling >= 175 && iScaling < 200)// 175- 200 %
	{
		lBkcol_X = MM_BKCOL_X + 30;
		l_Width = (lWidth) + 45;
		l_Height = lHeight + 45;
	}
	else if (iScaling >= 200 && iScaling < 225)// 200- 225 %
	{
		lBkcol_X = MM_BKCOL_X + 40;
		l_Width = (lWidth) + 60;
		l_Height = lHeight + 60;
	}
	else if (iScaling >= 225 && iScaling < 250)// 225- 250 %
	{
		lBkcol_X = MM_BKCOL_X + 50;
		l_Width = (lWidth) + 75;
		l_Height = lHeight + 75;
	}
	else if (iScaling >= 250 && iScaling < 275)// 250- 275 %
	{
		lBkcol_X = MM_BKCOL_X + 60;
		l_Width = lWidth + 90;
		l_Height = lHeight + 90;
	}
	else if (iScaling >= 275 && iScaling < 300)// 275- 300 %
	{
		lBkcol_X = MM_BKCOL_X + 70;
		l_Width = lWidth + 105;
		l_Height = lHeight + 105;
	}
	else if (iScaling >= 300 && iScaling < 325)// 300- 325 %
	{
		lBkcol_X = MM_BKCOL_X + 80;
		l_Width = lWidth + 120;
		l_Height = lHeight + 120;
	}
	else if (iScaling >= 325 && iScaling < 350)// 325- 350 %
	{
		lBkcol_X = MM_BKCOL_X + 90;
		l_Width = lWidth + 135;
		l_Height = lHeight + 135;
	}
	else if (iScaling >= 350 && iScaling < 375)// 350- 375 %
	{
		lBkcol_X = MM_BKCOL_X + 100;
		l_Width = lWidth + 150;
		l_Height = lHeight + 150;
	}
	else if (iScaling >= 375 && iScaling < 400)// 375- 400 %
	{
		lBkcol_X = MM_BKCOL_X + 110;
		l_Width = lWidth + 165;
		l_Height = lHeight + 165;
	}
	else if (iScaling >= 400 && iScaling < 425)// 400- 425 %
	{
		lBkcol_X = MM_BKCOL_X + 120;
		l_Width = lWidth + 180;
		l_Height = lHeight + 180;
	}
	else if (iScaling >= 425 && iScaling < 450)// 425- 450 %
	{
		lBkcol_X = MM_BKCOL_X + 130;
		l_Width = lWidth + 195;
		l_Height = lHeight + 195;
	}
	else if (iScaling >= 450 && iScaling < 475)// 450- 475 %
	{
		lBkcol_X = MM_BKCOL_X + 140;
		l_Width = lWidth + 210;
		l_Height = lHeight + 210;
	}
	else if (iScaling >= 475 && iScaling < 500)// 470- 500 %
	{
		lBkcol_X = MM_BKCOL_X + 150;
		l_Width = lWidth + 225;
		l_Height = lHeight + 225;
	}
	else if (iScaling == 500)// 500%
	{
		lBkcol_X = MM_BKCOL_X + 160;
		l_Width = lWidth + 240;
		l_Height = lHeight + 240;
	}
	*plStartX = lBkcol_X;
	*plWidth = l_Width;
	*plHeight = l_Height;
	return TRUE;
}

//=============================================================================
// function
//      SetSubOptImgTop_and_Left
//
// Parameters:
//				LONG lTopValue
//				LONG lLeftValue
//				LONG *plSubopt_TOP
//				LONG plSubopt_LEFT
//
// return value
//     BOOL
//
// outline
//			Set SubOpt ImgT op_and_Left in Left mimic box based on scaling factor
//=============================================================================
BOOL CMimicImage::SetSubOptImgTop_and_Left(LONG lTopValue,LONG lLeftValue,LONG *plSubopt_TOP,LONG *plSubopt_LEFT)
{
	HDC hDCMem = CreateCompatibleDC(m_hDC);
	if (nullptr == hDCMem)
	{
		return 0 ;
	}
	LONG lResolution = GetDeviceCaps(hDCMem, LOGPIXELSY);
	ReleaseResources(hDCMem);
	INT iScaling = static_cast<INT>(100.0 * lResolution / 96);
	LONG lSubopt_TOP = lTopValue;
	LONG lSubopt_LEFT = lLeftValue;
	if (iScaling >= 100 && iScaling < 125) // 100- 125 %
	{
		lSubopt_TOP = lTopValue;
		lSubopt_LEFT = lLeftValue;
	}
	else if (iScaling >= 125 && iScaling < 150) // 125- 150 %
	{
		lSubopt_TOP = lTopValue + 10;
		lSubopt_LEFT = lLeftValue + 50;
	}
	else if (iScaling >= 150 && iScaling < 175)// 150- 175 %
	{
		lSubopt_TOP = lTopValue + 20;
		lSubopt_LEFT = lLeftValue + 140;
	}
	else if (iScaling >= 175 && iScaling < 200)// 175- 200 %
	{
		lSubopt_TOP = lTopValue + 30;
		lSubopt_LEFT = lLeftValue + 230;
	}
	else if (iScaling >= 200 && iScaling < 225)// 200- 225 %
	{
		lSubopt_TOP = lTopValue + 40;
		lSubopt_LEFT = lLeftValue + 320;
	}
	else if (iScaling >= 225 && iScaling < 250)// 225- 250 %
	{
		lSubopt_TOP = lTopValue + 50;
		lSubopt_LEFT = lLeftValue + 410;
	}
	else if (iScaling >= 250 && iScaling < 275)// 250- 275 %
	{
		lSubopt_TOP = lTopValue + 60;
		lSubopt_LEFT = lLeftValue + 500;
	}
	else if (iScaling >= 275 && iScaling < 300)// 275- 300 %
	{
		lSubopt_TOP = lTopValue + 70;
		lSubopt_LEFT = lLeftValue + 590;
	}
	else if (iScaling >= 300 && iScaling < 325)// 300- 325 %
	{
		lSubopt_TOP = lTopValue + 80;
		lSubopt_LEFT = lLeftValue + 680;
	}
	else if (iScaling >= 325 && iScaling < 350)// 325- 350 %
	{

		lSubopt_TOP = lTopValue + 90;
		lSubopt_LEFT = lLeftValue + 770;
	}
	else if (iScaling >= 350 && iScaling < 375)// 350- 375 %
	{
		lSubopt_TOP = lTopValue + 100;
		lSubopt_LEFT = lLeftValue + 860;
	}
	else if (iScaling >= 375 && iScaling < 400)// 375- 400 %
	{
		lSubopt_TOP = lTopValue + 110;
		lSubopt_LEFT = lLeftValue + 950;
	}
	else if (iScaling >= 400 && iScaling < 425)// 400- 425 %
	{
		lSubopt_TOP = lTopValue + 120;
		lSubopt_LEFT = lLeftValue + 1040;
	}
	else if (iScaling >= 425 && iScaling < 450)// 425- 450 %
	{
		lSubopt_TOP = lTopValue + 130;
		lSubopt_LEFT = lLeftValue + 1130;
	}
	else if (iScaling >= 450 && iScaling < 475)// 450- 475 %
	{
		lSubopt_TOP = lTopValue + 140;
		lSubopt_LEFT = lLeftValue + 1220;
	}
	else if (iScaling >= 475 && iScaling < 500)// 470- 500 %
	{
		lSubopt_TOP = lTopValue + 150;
		lSubopt_LEFT = lLeftValue + 1310;
	}
	else if (iScaling == 500)// 500%
	{
		lSubopt_TOP = lTopValue + 160;
		lSubopt_LEFT = lLeftValue + 1400;
	}
	*plSubopt_TOP = lSubopt_TOP ;
	*plSubopt_LEFT = lSubopt_LEFT;
	return TRUE;
}
//<E>To Fix Redmine Bug #3687,29.December.2021,SSDI:Chanchal Singla
