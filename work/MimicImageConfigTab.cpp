//	Copyright 2020(c), SHARP CORPORATION. All rights are reserved. 
//	Reproduction or transmission in whole or in part,in any form or
//  by any means, electronic,mechanical or otherwise, is prohibited
//	without the prior written consent of the copyright owner.

//==================================================================================
//  File          : CMimicImageConfig.cpp
//
//  Module        : PrinterUI Module
//                                                                             
//  Owner         : Sharp Software Development India
//
//  Author        : Chanchal Singla
//
//  Date		  : 04 December 2020
//
//	Description   : Implementation file for mimic control.

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
//      CMimicImageConfig
//
// return value
//     NILL
//
// outline
//      Constructor to the class CMimicImageConfig.
//=============================================================================
CMimicImageConfig::CMimicImageConfig(
							_In_ CONST INT   iCtrlID,
							HWND hParentHandle,
							PFEATUREINFOLIST pFeatureInfoList,
							HMODULE hStringResourceHandle,
							HMODULE hBitmapResourceHandle,
							WCHAR*	pPrinterName,
							POEMDEV pOemPrivateDevMode,
							SHORT* sWaterMarkIndex
						):
					CMimicImage(
								iCtrlID,
								hParentHandle,
								pFeatureInfoList,
								hStringResourceHandle,
								hBitmapResourceHandle,
								pPrinterName,
								pOemPrivateDevMode,
								sWaterMarkIndex),
									m_pFeatureInfoList(pFeatureInfoList),
									m_pBitMap(NULL),
									m_hParentHandle(hParentHandle),
									m_pPrinterName(pPrinterName),
									m_pOemPrivateDevMode(pOemPrivateDevMode),
									m_pWaterMarkIndex(sWaterMarkIndex)
										
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	m_hMimicImageHandle = GetDlgItem(m_hParentHandle, iCtrlID);
	Attach(m_hMimicImageHandle);
	m_hDC = GetDC(m_hMimicImageHandle);
	RectMimicImage = {};
	GetWindowRect(m_hMimicImageHandle, &RectMimicImage);
	m_lBBoxW = RectMimicImage.right - RectMimicImage.left;
	m_lBBoxH = RectMimicImage.bottom - RectMimicImage.top;
	m_hBitmapResourceHandle = hBitmapResourceHandle;
	m_pBitMap = new CBitmap(m_hBitmapResourceHandle);
	//m_WaterMarkIndex = m_pOemPrivateDevMode->scpi.ext.nWatermark;
	if (NULL == m_pBitMap)
	{
		slog.putLog("CMimicImage::CMimicImage -> NULL_POINTER(m_pBitMap)\n");
	}
}
//=============================================================================
// function
//      CMimicImageConfig
//
// return value
//     NILL
//
// outline
//      Destructor to the class CMimicImageConfig.
//=============================================================================
CMimicImageConfig::~CMimicImageConfig(void)
{
	if (NULL != m_hDC)
	{
		ReleaseDC(m_hMimicImageHandle, m_hDC);
	}
	if (nullptr != m_pBitMap)
	{
		delete m_pBitMap;
		m_pBitMap = NULL;
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
VOID CMimicImageConfig::Draw(PPROPSTATE pps)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImage::Draw -> NULL_POINTER(m_pBitMap)\n");
		return;
	}
	HBITMAP hbm = NULL;
	HBITMAP		hbmOld2 = NULL;
	HDC			hDCBM = NULL;
	HDC hDCMem = CreateCompatibleDC(m_hDC);
	if (nullptr == hDCMem)
	{
		slog.putLog("CMimicImage::Draw -> NULL_HANDLE(hDCMem)\n");
		return;
	}
	hbm = CreateCompatibleBitmap(m_hDC, m_lBBoxW, m_lBBoxH);
	if (nullptr == hbm)
	{
		slog.putLog("CMimicImage::Draw -> NULL_HANDLE(hbm)\n");
		ReleaseResources(hDCMem);
		return;
	}
	HBITMAP hbmOld = (HBITMAP)SelectObject(hDCMem, hbm);
	hDCBM = ::CreateCompatibleDC(m_hDC);
	if (nullptr == hDCBM)
	{
		slog.putLog("CMimicImage::Draw -> NULL_HANDLE(hDCBM)\n");
		ReleaseResources(hDCMem, hbm);
		return;
	}
	m_pBitMap->load(IDB_MM_BKGRND);
	hbmOld2 = (HBITMAP)::SelectObject(hDCBM, m_pBitMap->handle());
	::StretchBlt(hDCMem, 0, 0, m_lBBoxW, m_lBBoxH, hDCBM, 0, 0, m_pBitMap->width(), m_pBitMap->height(), SRCCOPY);
	::SelectObject(hDCBM, hbmOld2);

	LONG lBkImgX = 96;

	// ================================================
	// _/_/_/  Papaer Image
	// ================================================
	drawPaperImg(pps, hDCMem, lBkImgX);

	// ================================================
	// _/_/_/  sub option
	// ================================================
	drawSubOptionImg(pps, hDCMem, &RectMimicImage, lBkImgX);

	::SelectObject(hDCBM, hbmOld2);
	::BitBlt(m_hDC, 0, 0, m_lBBoxW, m_lBBoxH, hDCMem, 0, 0, SRCCOPY);

	::SelectObject(hDCMem, hbmOld);
	ReleaseResources(hDCMem, hbm, hDCBM);
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
VOID CMimicImageConfig::drawPaperImg(PPROPSTATE pps,HDC hDC,long lBkWidth)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if(nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImageConfig::drawPaperImg -> NULL_POINTER(m_pBitMap)\n");
		return ;
	}
	HDC hDCMem = CreateCompatibleDC(hDC);
	if(nullptr == hDCMem)
	{
		slog.putLog("CMimicImageConfig::drawPaperImg -> NULL_HANDLE(hDCMem)\n");
		return ;
	}
	SHORT wOri = GetOrientationConfig();
	
	// --- DM_NUP
	SHORT NupValue = (short)(m_pOemPrivateDevMode->scpi.ext.byNupX * m_pOemPrivateDevMode->scpi.ext.byNupY);

	// --- DM_NUPORDER
	BYTE wNupOrder =  GetPropNupOrder(NupValue, m_pOemPrivateDevMode->scpi.ext.byNupOrder, wOri);

	// --- DM_NUPBORDER
	BYTE wNupBorder = m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_NUPBORDER ? bool_true : bool_false;

	SHORT wID = 0;
	SHORT wFTP = ftp_not;
	SHORT wFTPSize = m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize;
	SHORT wPaper = m_pOemPrivateDevMode->scpi.pub.dmPaperSize;
	if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_FITTOPAGE)
		wFTP = ftp_ftp;
	else if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_XYZOOM)
		wFTP = ftp_xyzoom;

	BYTE wBinEdge = m_pOemPrivateDevMode->scpi.ext.byBindedge;
	SHORT wZoomBasing = (m_pOemPrivateDevMode->scpi.doc.dwFieldDDM & DM_ZOOM_CENTER) ? zoom_basing_center : zoom_basing_upperleft;

	if ((ftp_not != wFTP)&&(nup_1 == NupValue))
	{
		// =================================
		// _/_/_/  Booklet Standard 
		// =================================
		if (m_pOemPrivateDevMode->scpi.ext.byPamph != PAMPH_NOT)
		{
			wOri = (wOri == ori_port) ? ori_land : ori_port;
		}
		if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_XYZOOM) //Zoom ON
		{
			wFTP = ftp_xyzoom;
		}
		else// Zoom  Off
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
		if (m_pOemPrivateDevMode->scpi.ext.byPamph != PAMPH_NOT)
		{
			wID = (wOri == ori_port) ? IDB_MM_PAMPH_P : IDB_MM_PAMPH_L;
			wID += 100 * wBinEdge;
		}
	}
	else 
	{
	    if (pamph_none == m_pOemPrivateDevMode->scpi.ext.byPamph)
		{
			
			wOri = GetOrientationConfig();
			wID = IDB_MM_1UP_P;
			if (ori_land == wOri)
			{
				wID = IDB_MM_1UP_L;
			}
			if (nup_1 != NupValue)
			{
				wID += (NupValue * 10);
			}
			if (wNupBorder != bool_false)
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
			wOri = GetOrientationConfig();
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
	//LONG lStartY = ((m_lBBoxH - lHeight) / 2);
	int iDPIPer = GetScreenResolution();

	lStartX = (lStartX * iDPIPer) / 100;
	long lResizedHeight = (m_pBitMap->height() * iDPIPer) / 100;
	//LONG lStartY =  ((m_lBBoxH - lHeight) / 2);
	LONG lStartY = ((m_lBBoxH - lResizedHeight) / 2);

	int wDest = (m_pBitMap->width() / 2 * iDPIPer) / 100;
	int hDest = (m_pBitMap->height() * iDPIPer) / 100;

	//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2 , m_pBitMap->height(), hDCMem, m_pBitMap->width() / 2, 0, SRCPAINT);
	//::BitBlt(hDC, lStartX, lStartY, m_pBitMap->width() / 2   , m_pBitMap->height() , hDCMem, 0, 0, SRCAND);
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
	drawStapleImg(pps,hDC, m_pPrinterName, lBkWidth);

	// ================================================
	// _/_/_/  Punch
	// ================================================
	//if(punch_none != m_pOemPrivateDevMode->scpi.ext.byPunchType)
	if (punch_none != pps->wPunch)
	{
		wID  = MM_PUNCHTYPE_BASEID;
		//<S><C>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla
		//Mimic is not displayed correctly when Punch is set to 2 / 3 in Configuration tab
		if (Is3holesPunchCase(pps, m_pPrinterName) == TRUE)
			wID += (pps->wPunchType * 100);
		else
			wID += (punchtype_2 * 100);
		//<E>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla
		if(ori_land == wOri)
		{
			wID += 10;
		}
		wID += wBinEdge;
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

	// ================================================
	// _/_/_/  WaterMark
	// ================================================
	m_pWaterMarkIndex = &m_pOemPrivateDevMode->scpi.ext.nWatermark;
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
		slog.putLog("CMimicImageConfig::drawPaperImg -> NULL_POINTER\n");
	}
	ReleaseResources(hDCMem);
}
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
void CMimicImageConfig::drawSubOptionImg(PPROPSTATE pps, HDC hDC, RECT *prcItem, long lBkWidth)
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
	m_lResizedIconWidth = (m_lIconWidth * iDPIPer) / 100;
	m_lResizedIconHeight = (m_lIconHeight * iDPIPer) / 100;
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
	SHORT wCollate = m_pOemPrivateDevMode->scpi.pub.dmCollate == DMCOLLATE_TRUE ? bool_true : bool_false;
	(*m_pBmpList1).draw(hDC, &m_rcItem1, wCollate);

	// ================================================
	// _/_/_/  Color Mode(2)
	// ================================================
	//SHORT wColMode = m_pOemPrivateDevMode->scpi.ext.ColData.byColMode;
	
	(*m_pBmpList2).draw(hDC, &m_rcItem2, ((*pps).wColMode - 1));

	// ================================================
	// _/_/_/  2-Sided Pamphlet(3)
	// ================================================
	SHORT wDuplex = m_pOemPrivateDevMode->scpi.ext.dmDuplexSave;

	// if Booklet is Standard that time wDuplex should be duplex_pamphlet(4) 
	if (PAMPH_NOT != m_pOemPrivateDevMode->scpi.ext.byPamph)
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
	SHORT wNoOffset = (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_OFFSET) ? offset_offset_each_set : offset_none;
	(*m_pBmpList4).draw(hDC, &m_rcItem4, wNoOffset - 1);


	// ================================================
	// _/_/_/  Print Release / Document Filing(5)
	// ================================================
	// Printing Policy - Print ReleaseÅ Document FilingÇ
	if ((*pps).wPPlcyPrintRelease == bool_true)
	{
		(*pps).wPrintRelease = bool_true;
		(*pps).wPPlcyRetention = bool_false;
	}
	if ((*pps).wPPlcyRetention != bool_false)
	{
		(*pps).wRetention = bool_true;
		(*pps).wRetentionMode = retention_hold_before;
		(*pps).wDocFileMode = docfile_main;
		(*pps).wUsePIN = bool_false;
		(*pps).wPPlcyPrintRelease = bool_false;
		(*pps).wPrintRelease = bool_false;
	}

	// Printing Policy - Print ReleaseÅ Document FilingÇ
	//<S><C>To Fix Redmine #7585,03-07-2024,SSDI:Jyothi
	if (((*pps).wPPlcyPrintRelease != bool_true && (*pps).wPPlcyRetention != bool_true)
		|| (*pps).wPPlcyPrintRelease == bool_true && (*pps).byPrintRelease == bool_true)
	//<E>To Fix Redmine #7585,03-07-2024,SSDI:Jyothi
	{
		if (0 != (*pps).byPrintRelease) {		//check with by print release flag also
			(*pps).wPrintRelease = bool_false;
		}
		if ((*pps).wPrintRelease == bool_true)
		{
			(*m_pBmpList5).draw(hDC, &m_rcItem5, 4);
		}
		else
		{
			(*m_pBmpList5).draw(hDC, &m_rcItem5, (*pps).wRetentionMode);
		}
	}
	else
	{
		(*m_pBmpList5).draw(hDC, &m_rcItem5, 0);
	}
		
	
	// ================================================
		// _/_/_/  Authentication(6)
	// ================================================
	////PPWindowsLoginNameasUserName
	if ((*pps).wPPlcyWinLogin == bool_true)
	{
		if (((*pps).wLoginName == bool_true) && ((*pps).wLoginPass == bool_true)
			&& ((*pps).wUseAcNum == bool_false))
		{
			(*pps).wLoginName = bool_true;
			(*pps).wLoginPass = bool_true;
			(*pps).wUseAcNum = bool_false;
		}
		else
		{
			(*pps).wLoginName = bool_true;
			(*pps).wLoginPass = bool_false;
			(*pps).wUseAcNum = bool_false;
		}
	}
	if ((*pps).wPPlcyUserAuth == bool_true)
	{
		(*pps).wLoginName = bool_false;
		(*pps).wLoginPass = bool_false;
		(*pps).wUseAcNum = bool_false;
		if ((*pps).wPPlcyWinLogin == bool_true)
		{
			(*pps).wLoginName = bool_true;
			(*pps).wLoginPass = bool_false;
			(*pps).wUseAcNum = bool_false;
		}
	}
	else if ((*pps).wPPlcyWinLogin == bool_false)
	{
		// --- DM_ALWAYSUSE_ACNUM
		(*pps).wUseAcNum = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_ALWAYSUSE_ACNUM ?
			bool_true : bool_false;

		// --- DM_USE_LOGINNAME
		(*pps).wLoginName = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINNAME ?
			bool_true : bool_false;

		// --- DM_USE_LOGINPASS
		(*pps).wLoginPass = m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC & DM_USE_LOGINPASS ?
			bool_true : bool_false;
	}
	wImgIndex = authentication_none;

	if (((*pps).wLoginName == bool_false) && ((*pps).wLoginPass == bool_false) && ((*pps).wUseAcNum == bool_false) && ((*pps).wLoginNameSSO == bool_false)) {
		wImgIndex = authentication_none;
	}
	else if (((*pps).wLoginName == bool_true) && ((*pps).wLoginPass == bool_false) && ((*pps).wUseAcNum == bool_false) && ((*pps).wLoginNameSSO == bool_false)) {
		wImgIndex = authentication_lname;
	}
	else if (((*pps).wLoginName == bool_true) && ((*pps).wLoginPass == bool_true) && ((*pps).wUseAcNum == bool_false) && ((*pps).wLoginNameSSO == bool_false)) {
		wImgIndex = authentication_lname_pswd;
	}
	else if (((*pps).wLoginName == bool_false) && ((*pps).wLoginPass == bool_false) && ((*pps).wUseAcNum == bool_true) && ((*pps).wLoginNameSSO == bool_false)) {
		wImgIndex = authentication_acnum;
	}
	else if (((*pps).wLoginName == bool_false) && ((*pps).wLoginPass == bool_false) && ((*pps).wUseAcNum == bool_false) && ((*pps).wLoginNameSSO == bool_true)) {
		wImgIndex = authentication_sso;
	}
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
VOID CMimicImageConfig::drawMarginShiftImg(HDC hDC, WCHAR FAR *pPrinterName, long lBkWidth)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImageConfig::drawMarginShiftImg -> NULL_POINTER(m_pBitMap)\n");
		return;
	}
	HDC hDCMem = CreateCompatibleDC(hDC);
	if (nullptr == hDCMem)
	{
		slog.putLog("CMimicImageConfig::drawMarginShiftImg -> NULL_HANDLE(hDCMem)\n");
		return;
	}
	BOOL				blRet = FALSE;
	MARGINSHIFTDATA		msd;
	CShRegMS FAR		*pregms = NULL;

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

	SecureZeroMemory(&msd, sizeof(MARGINSHIFTDATA));
	DWORD dwMarginWidth = 0;
	DWORD wMarginUnit = 0;
	SHORT wMargin = m_pOemPrivateDevMode->scpi.ext.byMgnShift;

	SHORT wOri = GetOrientationConfig();
	SHORT wID = 0;
	BYTE wBinEdge = m_pOemPrivateDevMode->scpi.ext.byBindedge;
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
		if (GetOrientationConfig() == ori_land)
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
//      GetStapleMimicID
//
// return value
//     SHORT
//
// outline
//      Gets Bitmap ID for Staple feature .
//=============================================================================
SHORT CMimicImageConfig::GetStapleMimicID(SHORT *wId, PPROPSTATE pps)
{
	SHORT bRet = FALSE;
	*wId = IDB_MM_STP_P_P;
	BYTE wBinEdge = m_pOemPrivateDevMode->scpi.ext.byBindedge;
	//BYTE byStaple = m_pOemPrivateDevMode->scpi.ext.byStaple;
	
	/*if ((staple_1 == byStaple)
		|| (staple_2 == byStaple))*/
	if ((staple_1 == pps->wStaple)
		|| (staple_2 == pps->wStaple))
	{
		bRet = FALSE;
		return bRet ;
	}
	//else if (staple_1_stapleless == byStaple)
	else if (staple_1_stapleless == pps->wStaple)
	{
		if (GetOrientationConfig() == ori_port)
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
VOID CMimicImageConfig::drawStapleImg(PPROPSTATE pps,HDC hDC, WCHAR FAR *pPrinterName, long lBkWidth)
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	if (nullptr == m_pBitMap)
	{
		slog.putLog("CMimicImageConfig::drawStapleImg -> NULL_POINTER(m_pBitMap)\n");
		return;
	}
	HDC hDCMem = CreateCompatibleDC(hDC);
	if (nullptr == hDCMem)
	{
		slog.putLog("CMimicImageConfig::drawStapleImg -> NULL_HANDLE(hDCMem)\n");
		return;
	}
	SHORT wOri = GetOrientationConfig();
	SHORT wID = 0;
	BYTE wBinEdge = m_pOemPrivateDevMode->scpi.ext.byBindedge;
	
	//BYTE byStaple = m_pOemPrivateDevMode->scpi.ext.byStaple;

	//if (staple_saddle_staple == byStaple)
	if (staple_saddle_staple == pps->wStaple)
	{
		wID = IDB_MM_STP_P_P;
		if (GetOrientationConfig() == ori_land)
			wID += 1;
	}
	//else if (staple_none != byStaple)
	else if (staple_none != pps->wStaple)
	{
		wID = IDB_MM_STP_P_P;
		if (pamph_none != m_pOemPrivateDevMode->scpi.ext.byPamph) //Booklet
		{
			if (ori_land == wOri)
			{
				wID += 1;
			}
		}
		else
		{
			//<S><C>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla
			//Mimic is not displayed correctly when Punch is set to 2 / 3 in Configuration tab
			SHORT wStapletemp = GetStapleValue();
			//if (GetStapleMimicID(&wID,pps) != TRUE)
			if (IsY7Y8Case(m_pPrinterName, &wID) != TRUE)
			//<E>To Fix Redmine Bug #3532,12.August.2021,SSDI:Chanchal Singla
			{
				//wID += byStaple * 100;
				//wID += pps->wStaple * 100;
				wID += wStapletemp * 100;
				if (ori_land == wOri)
				{
					wID += 10;
				}
				wID += wBinEdge;
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
SHORT CMimicImageConfig::GetOrientationConfig()
{
	MFPPrinterUI_Logger slog(__FUNCTION__"\n");
	short	wRet = ori_port;

	UINT NupValue = (short)(m_pOemPrivateDevMode->scpi.ext.byNupX * m_pOemPrivateDevMode->scpi.ext.byNupY);

	if (m_pOemPrivateDevMode->scpi.pub.dmOrientation == DMORIENT_LANDSCAPE)
	
	{
		if (booklet_none != m_pOemPrivateDevMode->scpi.ext.byPamph)
		{
			return ori_port;
		}
		switch (NupValue)
		{
		case nup_1:
		case nup_4:
		case nup_9:
		case nup_16:
			wRet = ori_land;
			break;
		default:
			break;

		}
	}
	else
	{
		if (booklet_none != m_pOemPrivateDevMode->scpi.ext.byPamph)
		{
			return ori_land;
		}
		switch (NupValue)
		{
		case nup_1:
		case nup_4:
		case nup_9:
		case nup_16:
			break;
		default:
			wRet = ori_land;
			break;

		}
	}
	return wRet;
}
//<S><A>To Fix FeedBack #35,29122020,SSDI:Chanchal Singla
//=============================================================================
// function
//      GetPropNupOrder
//
// parameters
//      wNup                N-Up
//      cNupOrderSCDM       Nup-
//
// return value
//      PV_NUPORDER
//
// outline
//      SCDMÇN-Up orderÇ
//=============================================================================
short CMimicImageConfig::GetPropNupOrder(short wNup, BYTE cNupOrderSCDM, short wOrient)
{
	short			wOrder = nup_order_none;

	switch (wNup)
	{
	case nup_1:
		break;

	case nup_2:
		wOrder = nup_order_ltor;
		if (wOrient == ori_port)
			wOrder = nup_order_ttob;
		else
		{
			if (cNupOrderSCDM == DM_NUP_ORDER_Z)
				wOrder = nup_order_ltor;
			else
				wOrder = nup_order_rtol;
		}

		break;

	default:
		wOrder = nup_order_randd;
		wOrder += cNupOrderSCDM;
		break;
	}

	return wOrder;
}
//<E>To Fix FeedBack #35,29122020,SSDI:Chanchal Singla
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
VOID CMimicImageConfig::ReleaseResources(	HDC hDCMem,
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
BOOL CMimicImageConfig::Is3holesPunchCase(PPROPSTATE pps, WCHAR *pPrinterName)
{
	BOOL		blRet = TRUE;

	SHORT		bOri;
	long		lPaper;
	long 		pl;
	WCHAR		pszPath[8];

	SecureZeroMemory(pszPath, sizeof(pszPath));
	SHORT wFTP = ftp_not;
	SHORT wFTPSize = m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize;
	SHORT wPaper = m_pOemPrivateDevMode->scpi.pub.dmPaperSize;
	if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_FITTOPAGE)
		wFTP = ftp_ftp;

	SHORT wBinEdge = m_pOemPrivateDevMode->scpi.ext.byBindedge;
	SHORT wPunchType = m_pOemPrivateDevMode->scpi.ext.byPunchType;

	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	CShIniFile plim(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
	//3-holesñ
	if (wPunchType != punchtype_3)
		goto EXIT;



	bOri = GetOrientationConfig();
	if (wFTP == ftp_not)
	{
		lPaper = wPaper;
	}
	else
	{
		lPaper = wFTPSize;
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
BOOL CMimicImageConfig::IsY7Y8Case(WCHAR *pPrinterName, short *pwBitmapID)
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
	SHORT			bOri;
	short			i;

	long FAR		*pl = NULL;

	WCHAR 		*pSec = NULL;
	SHORT wFTP = ftp_not;
	SHORT wFTPSize = m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize;
	SHORT wPaper = m_pOemPrivateDevMode->scpi.pub.dmPaperSize;
	if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 & DM_FITTOPAGE)
		wFTP = ftp_ftp;

	SHORT wBinEdge = m_pOemPrivateDevMode->scpi.ext.byBindedge;
	SHORT wPunchType = m_pOemPrivateDevMode->scpi.ext.byPunchType;
	SHORT wStaple = m_pOemPrivateDevMode->scpi.ext.byStaple;

	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	CShIniFile pinilim(ghInstance, pPrinterName, szCommonDatFilePath, FALSE);
	SHORT wDuplex = m_pOemPrivateDevMode->scpi.ext.dmDuplexSave;

	// if Booklet is Standard that time wDuplex should be duplex_pamphlet(4) 
	if (m_pOemPrivateDevMode->scpi.ext.byPamph == PAMPH_2UP)
	{
		wDuplex = duplex_pamphlet;
	}

	ws = wStaple;
	wb = wBinEdge;


	if (ws == staple_none)
		goto EXIT;

	bOri = GetOrientationConfig();
	if (wFTP == ftp_not)
	{
		wPaperSize = wPaper;
	}
	else
	{
		wPaperSize = wFTPSize;
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
					if (GetOrientationConfig() == ori_port)
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
					if (GetOrientationConfig() == ori_port)
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
SHORT CMimicImageConfig::GetStapleValue()
{
	SHORT wStaple = staple_none;
	if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_NONE)
	{
		wStaple = staple_none;
	}
	else if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_ONE)
	{
		wStaple = staple_1;
	}
	else if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_TWO)
	{
		wStaple = staple_2;
	}
	else if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_STAPLELESS)
	{
		wStaple = 8;
	}
	else if (m_pOemPrivateDevMode->scpi.ext.byStaple == STAPLE_SADDLE)
	{
		wStaple = 9;
	}
	return wStaple;
}


