#include "precomp.h"
#include "shjsonms.h"
#include "shjsonpp.h"
#include "shjsonstr.h"

CDevModeUpDate::CDevModeUpDate(POEMDEV pOemDevMode,
							   PaperInfoMap PaperInfo,
							   PRINT_FEATURE_OPTION* pPrintFeatureOptionPair,
							   UINT uNumFeatureOptionPair,
							   HANDLE hPrinter,
							   HMODULE hStringResourceHandle,
							   PRESOURCEIDINFO pResourceIDInfo,
							   WCHAR FAR *pszSvrPrnName,
								PUSERPAPERSIZEDATA pPaperSize_upsd,
								PUSERPAPERSIZEDATA pFitToPaperSize_upsd):
								m_pOemPrivateDevMode(pOemDevMode),
								m_PaperInfo(PaperInfo),
								m_pPrintFeatureOptionPair(pPrintFeatureOptionPair),
								m_uNumFeatureOptionPair(uNumFeatureOptionPair),
								m_FeatureIndex(0),m_bPamphletStyleSelected(FALSE),
								m_bFitToPaperSelected(FALSE),
								m_hPrinter(hPrinter),
								m_hStringResourceHandle(hStringResourceHandle),
								m_pResourceIDInfo(pResourceIDInfo),
								m_pPaperSize_upsd(pPaperSize_upsd),
								m_pFitToPaperSize_upsd(pFitToPaperSize_upsd)
{
	m_PaperPixelInfoMap = InitializePaperPixelInfoMap();
	m_pszSvrPrnName = pszSvrPrnName;
}


CDevModeUpDate::~CDevModeUpDate(void)
{
}
void CDevModeUpDate::UpDateDevMode(void)
{
	for(;m_FeatureIndex < m_uNumFeatureOptionPair ;m_FeatureIndex++)
	{
		if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"PaperSize"))
		{
			UpDatePaperSize();
		}
		//<S><C> to implement Custom Orientation 13/8/2020 K M Harika
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"PageShOrientation"))
		//<E> to implement Custom Orientation 13/8/2020 K M Harika
		{
			UpDateOrientation();
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"PagesPerSheet"))
		{
			UpDateNUP();
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"Border"))
		{
			UpDateNUPBorder();
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"Order"))
		{
			UpDateNUPOrder();
		}
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, HUNDRED_PERCENT_NUP))
		{
			UpDate100PercentNup();
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"BindingEdge"))
		{
			UpDateBindingEdge();
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"InputBin"))
		{
			UpDateInputBin();
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"MediaType"))
		{
			UpDateMediaType();
		}
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, COLLATE))
		{
			UpDateCollate();
		}
		//<S><D> Deleted the code because PamphletStyle Feature is not Supported in UD3L,2020-08-2020,SSDI:Chanchal Singla
		/*else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"Pamphlet"))
		{
			UpDatePamphletStyle();
		}*/
		//<E> Deleted the code because PamphletStyle Feature is not Supported in UD3L,2020-08-2020,SSDI:Chanchal Singla
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"Duplex"))
		{
			UpDateDuplexStyle();
		}
		
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"ZoomSettings"))
		{
			UpDateZoomSettings();
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"FitToPaperSize"))
		{
			if (!strcmp(m_pPrintFeatureOptionPair[FEATURE_BOOKLET].pszOption, "None"))
			{
				UpDateFitToPaperSize();
			}
		}
		//<S><D> Deleted the code because BlackAndWhite Feature is not Supported in UD3L,2020-11-27,SSDI:Chanchal Singla
	/*	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"BlackAndWhitePrint"))
		{
			UpDateBlackAndWhitePrint();
		}*/
		//<S><D> Deleted the code because BlackAndWhite Feature is not Supported in UD3L,2020-11-27,SSDI:Chanchal Singla
		//<S><D> to implement Custom Orientation 13/8/2020 K M Harika
		/*else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"Rotate180D"))
		{
			UpDateRotate180();
		}*/
		//<E> to implement Custom Orientation 13/8/2020 K M Harika
        else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"StapleOption"))
		{
			UpDateStaple();
		}
        else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,"Punch"))
		{
			UpDatePunch();
		}
//<S><C>To add Zoom Settings  saving to UD3L  2020.04.30, SSDI:Sneha TG
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, "ZoomBasing"))
		{
			//UpDateZoomBasing();
		}
//<E>To add Zoom Settings saving to UD3L 2020.04.30, SSDI:Sneha TG
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,RETENTION))
		{
			UpDateJobControlFields(DM_DOCFILING);
			
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,RETENTION_SETTINGS))
		{
			UpDateRetentionSettings();
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,DOCUMENT_FILING_SETTINGS))
		{
			UpDateDocumentFilingSettings();
		}
//<S><A> Added calling fuction for authentication feature,20.03.24 SSDI:Goutham
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, AUTHENTICATION))
		{
			UpDateAuthenticationSettings();
		}

		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, PRINTABLE_AREA))
		{
			UpDatePrintableArea();
		}
//<E> Added calling fuction for authentication feature,20.03.24 SSDI:Goutham
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, RETENTION_PASSWORD))
		{
			UpDateJobControlFields(DM_USE_PIN);
		}
		 else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,USER_AUTHENTICATION_LOGIN_NAME))
		{
			UpDateJobControlFields(DM_USE_LOGINNAME);
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,USER_AUTHENTICATION_PASSWORD))
		{
			UpDateJobControlFields(DM_USE_LOGINPASS);
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,USER_NUMBER))
		{
			UpDateJobControlFields(DM_ALWAYSUSE_ACNUM);
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,DEFAULT_JOB_USER_NAME))
		{
			UpDateJobControlFields(DM_ALWAYSUSE_USERID);
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,DEFAULT_JOB_JOB_NAME))
		{
			UpDateJobControlFields(DM_ALWAYSUSE_JOBID);
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,AUTO_JOB_CONTROL_REVIEW))
		{
			UpDateJobControlFields(DM_AUTOJOB);
		}
//<S><A>Updated devmode value for Print Release 2020.07.29 SSDI:Sneha TG
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, PRINTRELEASE))
		{
			UpDateJobControlFields(DM_PRINTSHARE);
		}
//<E>Updated devmode value for Print Release 2020.07.29 SSDI:Sneha TG
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature,COLOR))
		{
			UpdateColorFields();
		}
//<S><A> Added calling fuction for Booklet and Offset feature,20.07.19 SSDI:Goutham
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, BOOKLET))
		{
		    UpDateBooklet();
		} 
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, OFFSET))
		{
			UpDateOffset();
		}
//<S><E> Added calling fuction for Booklet and Offset feature,20.07.19 SSDI:Goutham	
//<S><A> Added calling fuction for Margin Shift and Stamp feature,20.07.27 SSDI:Goutham	
//<S><D> To Deleted because nWatermark variable is updated with Wrong value,2020-09-08,SSDI:Chanchal Singla
// nwatermark is updated with Watermark index drop down value inside  watermark dialog 
//		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, STAMP))
//		{
//			UpDateWatermark();
//		}
//<E> To Deleted because nWatermark variable is updated with Wrong value,2020-09-08,SSDI:Chanchal Singla
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, MARGIN_SHIFT))
		{
			UpDateMarginShift();
		}
//<S><E> Added calling fuction for Margin Shift and Stamp feature,20.07.27 SSDI:Goutham
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, INPUT_RESOLUTION))
		{
// As we are Updating Propstate to devmode and devmode to Propstate in DocumentPropertyPage.cpp, commenting below as devmode will be updated twice.  
			//UpDateInputResolution();
		}
		//<S><A> To Update the PrintPosition Custom feature DevmodeValue,2020-08-20,SSDI:Chanchal Singla
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, PRINT_POSITION))
		{
		#if SCDM_VER >= 3000
			UpDatePrintPosition();
		#endif
		}
		else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszFeature, BOOKLET_PAPER_SIZE))
		{
			if(m_bPamphletStyleSelected == TRUE)
			{
				UpDateBookletOutputSize();
			}	
		}
		//<E> To Update the PrintPosition Custom feature DevmodeValue,2020-08-20,SSDI:Chanchal Singla
	}
}
void CDevModeUpDate::UpDatePaperSize(void)
{
	
	
	
 //   if(0 != dwNeeded)
 //   {
 //       pCustomFormInfo = new BYTE[dwNeeded]();
 //       if(GetForm(m_hPrinter,m_pOemPublicDevMode->dmFormName,1,(LPBYTE)pCustomFormInfo,dwNeeded,&pcbNeeded))
 //       {
 //           if(NULL != pCustomFormInfo)
 //           {
 //               FORM_INFO_1* pCustomForm = reinterpret_cast<FORM_INFO_1*>(pCustomFormInfo);
 //               if(FORM_USER != pCustomForm->Flags)
 //               {
 //                   wcscpy_s(m_pOemPrivateDevMode->scpi.pub.dmFormName,MAX_FORMNAME_LENGTH + 1,L"Default");
 //                   wcscpy_s(m_pOemPublicDevMode->dmFormName,MAX_FORMNAME_LENGTH + 1,m_pOemPrivateDevMode->scpi.pub.dmFormName);
 //               }
 //               else
 //               {
 //                   slog.putLog("CCustomPaperDlg::DeleteCustomForm -> NULL_POINTER(pCustomFormInfo)\n");
 //               }
 //           }
 //           if(NULL != pCustomFormInfo)
 //           {
 //               delete[] pCustomFormInfo;
 //               pCustomFormInfo = NULL;
 //           }
 //           else
 //           {
 //               slog.putLog("CCustomPaperDlg::DeleteCustomForm -> NULL_POINTER(pCustomFormInfo)\n");
 //           }
 //       }
 //   }
 //   else
 //   {
 //       m_pOemPublicDevMode->dmPaperWidth = m_pOemPrivateDevMode->scpi.pub.dmPaperWidth ;              
 //       m_pOemPublicDevMode->dmPaperLength = m_pOemPrivateDevMode->scpi.pub.dmPaperLength ;
 //   }
if (!strcmp("CUSTOMSIZE", m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption))
{
	m_pOemPrivateDevMode->scpi.pub.dmPaperSize = DMPAPER_USER;
	m_pOemPrivateDevMode->scpi.pub.dmFields |= DM_PAPERLENGTH | DM_PAPERWIDTH;
	m_pOemPrivateDevMode->scpi.ext.nZoomSrcPaperSize = DMPAPER_USER;
	/*	DWORD dwNeeded = 0;
		DWORD pcbNeeded = 0;
		BYTE* pCustomFormInfo = NULL;
		BOOL bUserFormName = FALSE ;*/

		//<S><A> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
	RECTL			rcMgnI = {};
	RECTL           rcMgnM = {};

	long			lWidthI = 0;
	long			lLengthI = 0;

	CUSTOM_PAPERINFO		paper;
	paper.wNum = CUSTOM_PAPER;
	//<E> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
	//<S><D> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
//      GetForm(m_hPrinter, m_pOemPrivateDevMode->scpi.pub.dmFormName,1,(LPBYTE)pCustomFormInfo,0,&dwNeeded);
//      if(0 < dwNeeded)
	  //{
	  //	pCustomFormInfo = new BYTE[dwNeeded]();
	  //	if(GetForm(m_hPrinter,m_pOemPrivateDevMode->scpi.pub.dmFormName,1,(LPBYTE)pCustomFormInfo,dwNeeded,&pcbNeeded))
	  //	{
	  //		if(NULL != pCustomFormInfo)
	  //		{
//                  PFORM_INFO_1 pCustomForm = reinterpret_cast<PFORM_INFO_1>(pCustomFormInfo);
//                  if(FORM_USER == pCustomForm->Flags)
//                  {
//                      bUserFormName = TRUE;
//                      wcscpy_s(m_pOemPrivateDevMode->scpi.pub.dmFormName,MAX_FORMNAME_LENGTH + 1,pCustomForm->pName);

//                      //<S><A> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
//                      paper.wPaperWidth01mm	= (WORD)(pCustomForm->Size.cx / 100);
//                      paper.wPaperLength01mm	= (WORD)(pCustomForm->Size.cy / 100);
//                      lWidthI		= MilliToInch(paper.wPaperWidth01mm, MM_LOMETRIC, MM_LOENGLISH);
//                      lLengthI	= MilliToInch(paper.wPaperLength01mm, MM_LOMETRIC, MM_LOENGLISH);
//                      m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli	= (SHORT)paper.wPaperWidth01mm;
//                      m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli	= (SHORT)paper.wPaperLength01mm;
//                      //<E> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
//                  }
//              }
//          }
//          delete [] pCustomFormInfo;
//          pCustomFormInfo = NULL ;
//      }
	  //<E> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
	  //<S><A> To Added Support  of Custom Paper, 27-07-2020,SSDI:Chanchal Singla
	WCHAR wcsFormName[MAX_FORMNAME_LENGTH + 1] = {};
	if (NULL != m_pResourceIDInfo)
	{
		OptionResourceID::const_iterator iUIOptionIterator;
		iUIOptionIterator = m_pResourceIDInfo->uOptionResourceID.find(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption);
		if (iUIOptionIterator != m_pResourceIDInfo->uOptionResourceID.end())
		{
			LoadString(m_hStringResourceHandle, iUIOptionIterator->second.first, wcsFormName, countof(wcsFormName));
			wcscpy_s(m_pOemPrivateDevMode->scpi.pub.dmFormName, MAX_FORMNAME_LENGTH + 1, wcsFormName);
		}
	}
	//<S><D>To Fix Coverity #91853,2022.03.17,SSDI:Chanchal Singla
	//That code is not using now hence deleted the code
/*	USERPAPERSIZEDATA upsd;
	SecureZeroMemory(&upsd, sizeof(upsd));
	CUPSReg FAR		*pregUps = NULL;
	pregUps = new CUPSReg(m_hStringResourceHandle, m_pszSvrPrnName);
	if (pregUps == NULL)
	{
		return;
	}*/
	//<E>To Fix Coverity #91853,2022.03.17,SSDI:Chanchal Singla
	//<S><C>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
	//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
	//(*pregUps).ReadUPSData(m_pszSvrPrnName, DMPAPER_CUSTOM, &upsd);
	//DWORD Width = upsd.dwWidth;
	//DWORD Length = upsd.dwLength;
	//SHORT Unit = upsd.wUnit;
	DWORD Width = m_pPaperSize_upsd->dwWidth;
	DWORD Length = m_pPaperSize_upsd->dwLength;
	SHORT Unit = m_pPaperSize_upsd->wUnit;
	//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
	//	<S><A>To Fix Redmine Bug #3667,2021.10.12,SSDI:Chanchal Singla
	//	Original Size Dimensions are not displaying when we are changing maximum paper Size A3 / Ledger to A4 / Letter.
	//Maxpapersize
	SHORT byMaxPaperSize = (m_pOemPrivateDevMode->scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
	if (byMaxPaperSize == eSecondOption)
	{
		if (unit_mm == Unit)
		{
			Width = (Width > MAXPAPER_CUSTOM_WIDTH_MM) ? MAXPAPER_CUSTOM_WIDTH_MM : Width;
			Length = (Length > MAXPAPER_CUSTOM_LENGTH_MM) ? MAXPAPER_CUSTOM_LENGTH_MM : Length;
		}
		else if (unit_inch == Unit)//inch
		{
			Width = (Width > MAXPAPER_CUSTOM_WIDTH_INCH) ? MAXPAPER_CUSTOM_WIDTH_INCH : Width;
			Length = (Length > MAXPAPER_CUSTOM_LENGTH_INCH) ? MAXPAPER_CUSTOM_LENGTH_INCH : Length;
		}
		USERPAPERSIZEDATA UpsdTemp;
		SecureZeroMemory(&UpsdTemp, sizeof(UpsdTemp));
		UpsdTemp.dwWidth = (DWORD)Width;
		UpsdTemp.dwLength = (DWORD)Length;
		UpsdTemp.wUnit = (DWORD)Unit;
		//(*pregUps).WriteUPSData(m_pszSvrPrnName, DMPAPER_CUSTOM, &UpsdTemp);
	}
	//	<E>To Fix Redmine Bug #3667,2021.10.12,SSDI:Chanchal Singla
	//<S><C>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
	//Bug #2539 Issue 2: Custom dialog values are getting updated in Devices and Printers even though values are changed from application
	m_pOemPrivateDevMode->scpi.pub.dmPaperWidth = Width;
	m_pOemPrivateDevMode->scpi.pub.dmPaperLength = Length;
	m_pOemPrivateDevMode->scpi.ext.byCusUnits = (BYTE)Unit;
	//<E>To Fix Redmine Bug #2539 ,25.02.2022,SSDI:Chanchal Singla
	if (unit_inch == Unit)
	{
		m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = (DWORD)InchToPixel(Width, 600, MM_LOENGLISH);
		m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel, 600, MM_LOMETRIC);

		m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = (DWORD)InchToPixel(Length, 600, MM_LOENGLISH);
		m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel, 600, MM_LOMETRIC);

	}
	else if (unit_mm == Unit)
	{
		m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)Width;
	//	m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = MilliToPixel(m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli, 600, MM_LOMETRIC);

		m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)Length;
	//	m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = MilliToPixel(m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli, 600, MM_LOMETRIC);
		MIP_MINMAX_CONV_PARAM stCnvParam;
		long	lInchWA;
		long	lInchLA;
		stCnvParam.mi = mip_mi_inch;
		stCnvParam.pos = 10;
		stCnvParam.wl = mip_wl_width;
		lInchWA = MilliToInchNoChk_CusPaper(Width, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);
		stCnvParam.wl = mip_wl_length;
		lInchLA = MilliToInchNoChk_CusPaper(Length, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);

		stCnvParam.mi = mip_mi_inch;
		stCnvParam.pos = 10;
		stCnvParam.wl = mip_wl_width;
		m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = (DWORD)InchToPixel_CusPaper(lInchWA, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
		stCnvParam.wl = mip_wl_length;
		m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = (DWORD)InchToPixel_CusPaper(lInchLA, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);

	}
	//<E> To Added Support  of Custom Paper, 27-07-2020,SSDI:Chanchal Singla
//     if(FALSE == bUserFormName)
	{
		//<S><C> RedMine Ticket(#1633) Custom Paper: Values which are set are not retaining when opened second time: 2014.11.17, SSDI:Sonali
		//dmFormName was not updated correctly fo Default and Untitled.
	 //   if(strcmp(m_pPrintFeatureOptionPair[m_uNumFeatureOptionPair-1].pszOption,"UnTitled"))
		{
			// WCHAR* pCustomPaperFeatureOption = L"Default";
	//		m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel, 600, MM_LOMETRIC);
	//		m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel, 600, MM_LOMETRIC);
			//   wcscpy_s(m_pOemPrivateDevMode->scpi.pub.dmFormName,MAX_FORMNAME_LENGTH + 1,pCustomPaperFeatureOption);

			   //<S><A> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
			lWidthI = MilliToInch(m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli, MM_LOMETRIC, MM_LOENGLISH);
			lLengthI = MilliToInch(m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli, MM_LOMETRIC, MM_LOENGLISH);
			//<E> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali

		}
		//<S><D> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
		//else
		//{
		//    WCHAR wcsFormName[MAX_FORMNAME_LENGTH + 1] = {};
		//    LoadString(m_hStringResourceHandle, IDS_UNTITLED, wcsFormName, countof(wcsFormName));
		//    wcscpy_s(m_pOemPrivateDevMode->scpi.pub.dmFormName,MAX_FORMNAME_LENGTH + 1,wcsFormName);

		//    //<S><A> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
		//    m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel , 600, MM_LOMETRIC);
		//    m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel , 600, MM_LOMETRIC);                
		//    
		//    lWidthI		= MilliToInch(m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli, MM_LOMETRIC, MM_LOENGLISH);
		//    lLengthI	= MilliToInch(m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli, MM_LOMETRIC, MM_LOENGLISH);
		//    //<E> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali

		//}
		//<E> RedMine Ticket(#1633) Custom Paper: Values which are set are not retaining when opened second time: 2014.11.17, SSDI:Sonali
	//<E> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
	}
	//<S><A>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
	if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_EDGETOEDGE)
	{
		m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli = 10;
		m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli = 10;
		m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli = 10;
		m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli = 10;
	}
	else
	{
		m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli = 42;
		m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli = 42;
		m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli = 42;
		m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli = 42;
	}
	//<E>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
	//<S><A> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
	//<S><C> RedMine Ticket(#1650) Issue#3: 2015.05.13, SSDI:Sonali
	/*if(unit_inch == m_pOemPrivateDevMode->scpi.ext.byCusUnits)
	{*/
	rcMgnI.left = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
	rcMgnI.top = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
	rcMgnI.right = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
	rcMgnI.bottom = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
	/*}
	//<S><D> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
	else
	{
		rcMgnM.left		= m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli;
		rcMgnM.top		= m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli;
		rcMgnM.right	= m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli;
		rcMgnM.bottom	= m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli;
	}*/
	//<D> RedMine Ticket(#1650) Issue#3: 2015.05.13, SSDI:Sonali
		//<E> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
	paper.wOffsetXPixel = (WORD)((rcMgnI.left * 600) / 100);
	paper.wOffsetYPixel = (WORD)((rcMgnI.top * 600) / 100);
	paper.wImageAreaXPixel = (WORD)(((lWidthI - rcMgnI.left - rcMgnI.right) * 600) / 100);
	paper.wImageAreaYPixel = (WORD)(((lLengthI - rcMgnI.top - rcMgnI.bottom) * 600) / 100);
	//<E> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali

	//paper.wOffsetXPixel = 100;
	//paper.wOffsetYPixel = 100;
	//paper.wImageAreaXPixel = 1924;
	//paper.wImageAreaYPixel = 3299;

	//<S><D> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
	//m_pOemPrivateDevMode->scpi.pub.dmPaperWidth = m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli;
	//m_pOemPrivateDevMode->scpi.pub.dmPaperLength = m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli ;
 //   m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel , 600, MM_LOMETRIC);
 //   m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel , 600, MM_LOMETRIC);
	//<S><A> RedMine Ticket(#1650) Custom paper set is not displayed when opened again: 2015.01.05, SSDI:Sonali
	//Changing dmPaperWidth,dmPaperLength,dwDstWidthPixel,dwDstHeightPixel,
	//only when FitToPaper is not selected 
	//<S><C> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
  //  if(1 == strcmp("Normal",m_pPrintFeatureOptionPair[FEATURE_ZOOMSETTINGS].pszOption))
	//<E> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
	//<E> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
	{
		m_pOemPrivateDevMode->scpi.pub.dmPaperWidth = m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli;
		m_pOemPrivateDevMode->scpi.pub.dmPaperLength = m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli;
		m_pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel;
		m_pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel;

		//<S><C> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
		m_pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel = paper.wImageAreaXPixel;
		m_pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel = paper.wImageAreaYPixel;
		m_pOemPrivateDevMode->scpi.doc.nLeftMarginPixel = paper.wOffsetXPixel;
		m_pOemPrivateDevMode->scpi.doc.nTopMarginPixel = paper.wOffsetYPixel;
		//Fixed Coverity #91598,2021.12.22,SSDI:Chanchal Singla
		m_pOemPrivateDevMode->scpi.ext.nDstLeftMarginPixel = paper.wOffsetXPixel;
		m_pOemPrivateDevMode->scpi.ext.nDstTopMarginPixel = paper.wOffsetYPixel;
		//<E> RedMine Ticket(#1650) Issue#2: 2015.01.05, SSDI:Sonali
		//m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = GetPaperSizeDef();
	}
	//<E> RedMine Ticket(#1650) Custom paper set is not displayed when opened again: 2015.01.05, SSDI:Sonali

}
	else
	{
		WCHAR wcsFormName[MAX_FORMNAME_LENGTH + 1] = {};
		if(NULL != m_pResourceIDInfo)
		{
			OptionResourceID::const_iterator iUIOptionIterator;
			iUIOptionIterator = m_pResourceIDInfo->uOptionResourceID.find(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption);
			if(iUIOptionIterator != m_pResourceIDInfo->uOptionResourceID.end())
			{
				LoadString(m_hStringResourceHandle, iUIOptionIterator->second.first, wcsFormName, countof(wcsFormName));
				wcscpy_s(m_pOemPrivateDevMode->scpi.pub.dmFormName,MAX_FORMNAME_LENGTH + 1,wcsFormName);
			}
		}
		PaperInfoMap::const_iterator it;
		it = m_PaperInfo.find(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption);
		if(it != m_PaperInfo.end())
		{
			m_pOemPrivateDevMode->scpi.pub.dmPaperSize = (SHORT)it->second.PaperSize;
			//<S><C>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
			//Following variables are updating to the support of printable-Maximum
			//(*pdoc).dwImageableWidthPixel 
			//(*pdoc).dwImageableHeightPixel 
			//(*pdoc).nLeftMarginPixel
			//(*pdoc).nTopMarginPixel 
			//(*pext).nDstLeftMarginPixel 
			//(*pext).nDstTopMarginPixel 
			//for a more information regarding this updation of variable check UD3.1 Code and xxxxPI61.dat file
			// ================================================
			// _/_/_/  EDGE TO EDGE (Printable AREA)
			// ================================================
			if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_EDGETOEDGE)
			{
				SetPaperSize(&m_pOemPrivateDevMode->scpi,TRUE);
			}
			else
			{
				m_pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel = it->second.PageImageaDimensions.first;
				m_pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel = it->second.PageImageaDimensions.second;
				m_pOemPrivateDevMode->scpi.doc.nLeftMarginPixel = it->second.PageOrigin.first;
				m_pOemPrivateDevMode->scpi.doc.nTopMarginPixel = it->second.PageOrigin.second;
				m_pOemPrivateDevMode->scpi.ext.nDstLeftMarginPixel = it->second.PageOrigin.first;
				m_pOemPrivateDevMode->scpi.ext.nDstTopMarginPixel = it->second.PageOrigin.second;
				// Update Margin Value
				m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli = (short)42;
				m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli = (short)42;
				m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli = (short)42;
				m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli = (short)42;
			}
			//<E>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
			m_pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = 2 * m_pOemPrivateDevMode->scpi.doc.nLeftMarginPixel + m_pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel;
			m_pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = 2 * m_pOemPrivateDevMode->scpi.doc.nTopMarginPixel + m_pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel;
			m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)it->second.PaperWidth;
            m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)it->second.PaperLength;

			//<S><A> To Fix RedmineIssue #6129, 2023.03.27,SSDI:Chandrashekar. V
			m_pOemPrivateDevMode->scpi.pub.dmPaperWidth = m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli;
			m_pOemPrivateDevMode->scpi.pub.dmPaperLength = m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli;
			// <E> To Fix RedmineIssue #6129, 2023.03.27,SSDI:Chandrashekar. V
			
			
			//<E>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
		}
	}
	
}
void CDevModeUpDate::UpDateOrientation(void)
{
	//<S><C> to implement Custom Orientation 13/8/2020 K M Harika
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"PORTRAIT"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmOrientation = DMORIENT_PORTRAIT ;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_ROTATE180DEG;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 &= ~DM_ROTATE180DEG_HW;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"LANDSCAPE_CC90"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmOrientation = DMORIENT_LANDSCAPE ;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_ROTATE180DEG;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 &= ~DM_ROTATE180DEG_HW;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "PORTRAIT_ROTATED"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmOrientation = DMORIENT_PORTRAIT;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_ROTATE180DEG;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 |= DM_ROTATE180DEG_HW;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "LANDSCAPE_CC270"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmOrientation = DMORIENT_LANDSCAPE;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_ROTATE180DEG;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 |= DM_ROTATE180DEG_HW;
	}
	//<E> to implement Custom Orientation 13/8/2020 K M Harika
}
void CDevModeUpDate::UpDateNUP(void)
{
	UINT NupValue = atol(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption);
	switch(NupValue)
	{
		case 1:		
			m_pOemPrivateDevMode->scpi.ext.byNupX = 1;
			m_pOemPrivateDevMode->scpi.ext.byNupY = 1;
			break;
		case 2:
			m_pOemPrivateDevMode->scpi.ext.byNupX = 2;
			m_pOemPrivateDevMode->scpi.ext.byNupY = 1;
			break;

		case 4:
			m_pOemPrivateDevMode->scpi.ext.byNupX = 2;
			m_pOemPrivateDevMode->scpi.ext.byNupY = 2;
			break;

		case 6:
			m_pOemPrivateDevMode->scpi.ext.byNupX = 3;
			m_pOemPrivateDevMode->scpi.ext.byNupY = 2;
			break;

		case 8:
			m_pOemPrivateDevMode->scpi.ext.byNupX = 4;
			m_pOemPrivateDevMode->scpi.ext.byNupY = 2;
			break;

		case 9:
			m_pOemPrivateDevMode->scpi.ext.byNupX = 3;
			m_pOemPrivateDevMode->scpi.ext.byNupY = 3;
			break;

		case 16:
			m_pOemPrivateDevMode->scpi.ext.byNupX = 4;
			m_pOemPrivateDevMode->scpi.ext.byNupY = 4;
			break;

		default:
			 break; //Some problem
		
	}

	if (DMORIENT_LANDSCAPE == m_pOemPrivateDevMode->scpi.pub.dmOrientation)
	{
		BYTE Temp	= m_pOemPrivateDevMode->scpi.ext.byNupX;
		m_pOemPrivateDevMode->scpi.ext.byNupX	= m_pOemPrivateDevMode->scpi.ext.byNupY;
		m_pOemPrivateDevMode->scpi.ext.byNupY = Temp;
	}
}
void CDevModeUpDate::UpDateNUPBorder(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"OFF"))
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_NUPBORDER;
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_NUPBORDER;
	}
}
void CDevModeUpDate::UpDateZoomBasing(void)
{
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "Upper_left"))
	{
		m_pOemPrivateDevMode->scpi.doc.dwFieldDDM &= ~DM_ZOOM_CENTER;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "Center"))
	{
		m_pOemPrivateDevMode->scpi.doc.dwFieldDDM |= DM_ZOOM_CENTER;
	}
}
void CDevModeUpDate::UpDateNUPOrder(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"NONE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_Z;
	}
	else if (2 == m_pOemPrivateDevMode->scpi.ext.byNupX * m_pOemPrivateDevMode->scpi.ext.byNupY)
	{
		m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_Z;

		if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"LeftToRight"))
		{
			m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_Z;
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"RightToLeft"))
		{
			m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_RZ;
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"TopToBottom"))
		{
			m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_Z;
		}
		if(DMORIENT_LANDSCAPE == m_pOemPrivateDevMode->scpi.pub.dmOrientation)
		{
			m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_Z;
		}
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_Z;

		if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"RightAndDown"))
		{
			m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_Z;
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"DownAndRight"))
		{
			m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_RN;
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"LeftAndDown"))
		{
			m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_RZ;
		}
		else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"DownAndLeft"))
		{
			m_pOemPrivateDevMode->scpi.ext.byNupOrder = DM_NUP_ORDER_N;
		}
	}
}
void CDevModeUpDate::UpDateBindingEdge(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"LEFTEDGE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byBindedge = BINDEDGE_LEFT ;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"RIGHTEDGE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byBindedge = BINDEDGE_RIGHT;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"TOPEDGE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byBindedge = BINDEDGE_TOP;
	}
}
void CDevModeUpDate::UpDateMediaType(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"IGNORE"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_IGNORE;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"AUTOSELECT"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_AUTO;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"PLAIN"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_PLAIN;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"LETTERHEAD"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_LETTERHEAD;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"PREPRINTED"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_PREPRINT;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"PREPUNCHED"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_PREPUNCH;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"RECYCLED"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_RECYCLED;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"COLOR"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_COLOR;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"LABEL"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_LABEL;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"HEAVYPAPER"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_HEAVY;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"TRANSPARENCY1"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_T_OHP;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"ENVELOPE"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_ENVELOPE;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"JAPANESEPOSTCARD"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmMediaType = DMMEDIA_J_POSTCARD;
	}
}
void CDevModeUpDate::UpDateInputBin(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"FORMSOURCE"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmDefaultSource = DMBIN_AUTO;
		m_pOemPrivateDevMode->scpi.ext.dmDefaultSourceSave = DMBIN_AUTO;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"BYPASSTRAY"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmDefaultSource = DMBIN_BYPASS_A;
		m_pOemPrivateDevMode->scpi.ext.dmDefaultSourceSave = DMBIN_BYPASS_A;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"TRAY1"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmDefaultSource = DMBIN_TRAY1;
		m_pOemPrivateDevMode->scpi.ext.dmDefaultSourceSave = DMBIN_TRAY1;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"TRAY2"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmDefaultSource = DMBIN_TRAY2;
		m_pOemPrivateDevMode->scpi.ext.dmDefaultSourceSave = DMBIN_TRAY2;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"TRAY3"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmDefaultSource = DMBIN_TRAY3;
		m_pOemPrivateDevMode->scpi.ext.dmDefaultSourceSave = DMBIN_TRAY3;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"TRAY4"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmDefaultSource = DMBIN_TRAY4;
		m_pOemPrivateDevMode->scpi.ext.dmDefaultSourceSave = DMBIN_TRAY4;
	}
	
}
void CDevModeUpDate::UpDateDuplexStyle(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"NONE"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmDuplex = DMDUP_SIMPLEX;
		m_pOemPrivateDevMode->scpi.ext.dmDuplexSave = DMDUP_SIMPLEX;
	//	m_pOemPrivateDevMode->scpi.ext.byPamph = PAMPH_NOT;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"VERTICAL"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmDuplex = DMDUP_VERTICAL;
		m_pOemPrivateDevMode->scpi.ext.dmDuplexSave = DMDUP_VERTICAL;
		//m_pOemPrivateDevMode->scpi.ext.byPamph = PAMPH_NOT;
		
		
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"HORIZONTAL"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmDuplex = DMDUP_HORIZONTAL;
		m_pOemPrivateDevMode->scpi.ext.dmDuplexSave = DMDUP_HORIZONTAL;
		// When Document style is "Tablet", Binding edge should always "TOP" ie left and right edge
		// are constraints. Since Constraint implementation is not done below code is added to ensure "tablet" feature
		// matches that of UD2 driver.
		//m_pOemPrivateDevMode->scpi.ext.byBindedge = BINDEDGE_TOP ;
		//m_pOemPrivateDevMode->scpi.ext.byPamph = PAMPH_NOT;
		
	}
	
	
}
void CDevModeUpDate::UpDatePamphletStyle(void)  
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"NONE"))
	{
		/*m_pOemPrivateDevMode->scpi.pub.dmDuplex = DMDUP_SIMPLEX;
		m_pOemPrivateDevMode->scpi.ext.dmDuplexSave = DMDUP_SIMPLEX;
		m_pOemPrivateDevMode->scpi.ext.byPamph = PAMPH_NOT;*/
		m_pOemPrivateDevMode->scpi.ext.byPamph = PAMPH_NOT;
	}
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"TiledPamphlet"))
	{
		//m_pOemPrivateDevMode->scpi.pub.dmDuplex = DMDUP_SIMPLEX;
		//m_pOemPrivateDevMode->scpi.ext.dmDuplexSave = DMDUP_SIMPLEX;
		m_pOemPrivateDevMode->scpi.ext.byPamph = PAMPH_TILED;
		m_bPamphletStyleSelected = TRUE ;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"2UpPamphlet"))
	{
		//m_pOemPrivateDevMode->scpi.pub.dmDuplex = DMDUP_SIMPLEX;
	//	m_pOemPrivateDevMode->scpi.ext.dmDuplexSave = DMDUP_SIMPLEX;
		m_pOemPrivateDevMode->scpi.ext.byPamph = PAMPH_2UP;
		m_pOemPrivateDevMode->scpi.ext.nPamphPaperSize = GetPamphSize(m_pOemPrivateDevMode->scpi.pub.dmPaperSize,m_pOemPrivateDevMode->scpi.ext.byPamph,TRUE);
		PaperPixelInformationMap::const_iterator paperpixelinfoiterator;
		paperpixelinfoiterator = m_PaperPixelInfoMap.find(m_pOemPrivateDevMode->scpi.ext.nPamphPaperSize);
		if(paperpixelinfoiterator != m_PaperPixelInfoMap.end())
		{
			m_pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = paperpixelinfoiterator->second.dwWidthPixel;
			m_pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = paperpixelinfoiterator->second.dwHeightPixel;
		}
		m_bPamphletStyleSelected = TRUE ;
	}
}
void CDevModeUpDate::UpDateCollate(void)
{
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "ON"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmCollate = DMCOLLATE_TRUE ;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "OFF"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmCollate = DMCOLLATE_FALSE;
	}
}
//<S><C>Modified to Support of Zoom feature , 20-08-2020,SSDI:Goutham
void CDevModeUpDate::UpDateZoomSettings(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"OFF"))
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_XYZOOM;
		
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_XYZOOM;
		
	}
}
//<S><E>Modified to Support of Zoom feature , 20-08-2020,SSDI:Goutham

void CDevModeUpDate::UpDateFitToPaperSize(void)
{
	//<S><A> To Added Support  of Custom Paper, 24-07-2020,SSDI:Chanchal Singla
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "SAOS"))
	{
		//<S><C> Changed to fix bug #2073 13/8/2020 K M Harika
		//m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_FITTOPAGE;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_FITTOPAGE;
		//<E> Changed to fix bug #2073 13/8/2020 K M Harika
		m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = m_pOemPrivateDevMode->scpi.pub.dmPaperSize;
	}
	else
	{
		m_bFitToPaperSelected = TRUE;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_FITTOPAGE;
	}
	if (TRUE == m_bFitToPaperSelected)
	{
		if (!strcmp("CUSTOMSIZE", m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption))
		{
	//<S><C>To Fix Fittopage custom paper,2023.04.08,SSDI:Chandrashekar V

	//			m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = CUSTOM_PAPER;
	//			m_pOemPrivateDevMode->scpi.pub.dmFields = DM_PAPERLENGTH | DM_PAPERWIDTH;
	//			BOOL blEtoEPrint = FALSE;
	//			if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_EDGETOEDGE)
	//			{
	//				blEtoEPrint = TRUE;
	//			}
	//			RECTL			rcMgnI = {};
	//			RECTL           rcMgnM = {};
	//
	//			long			lWidthI = 0;
	//			long			lLengthI = 0;
	//			MIP_MINMAX_CONV_PARAM	stCnvParam;
	//			SecureZeroMemory(&stCnvParam, sizeof(stCnvParam));
	//			CUSTOM_PAPERINFO		paper;
	//			SecureZeroMemory(&paper, sizeof(paper));
	//			USERPAPERSIZEDATA upsd;
	//			SecureZeroMemory(&upsd, sizeof(upsd));
	//			CUPSReg FAR		*pregUps = NULL;
	//			pregUps = new CUPSReg(m_hStringResourceHandle, m_pszSvrPrnName);
	//			if (pregUps == NULL)
	//			{
	//				return;
	//			}
	//			(*pregUps).ReadUPSData(m_pszSvrPrnName, DMPAPER_CUSTOM_ZOOM, &upsd);
	//			DWORD Width = upsd.dwWidth;
	//			DWORD Length = upsd.dwLength;
	//			SHORT Unit = upsd.wUnit;
	//			
	//			//m_pOemPrivateDevMode->scpi.ext.byCusUnits = (BYTE)Unit;
	//			//	<S><A>To Fix Redmine Bug #3667,2021.10.12,SSDI:Chanchal Singla
	//			//	Original Size Dimensions are not displaying when we are changing maximum paper Size A3 / Ledger to A4 / Letter.
	//			//Maxpapersize
	//			SHORT byMaxPaperSize = (m_pOemPrivateDevMode->scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
	//			if (byMaxPaperSize == eSecondOption)
	//			{
	//				if (unit_mm == Unit)
	//				{
	//					Width = (Width > MAXPAPER_CUSTOM_WIDTH_MM) ? MAXPAPER_CUSTOM_WIDTH_MM : Width;
	//					Length = (Length > MAXPAPER_CUSTOM_LENGTH_MM) ? MAXPAPER_CUSTOM_LENGTH_MM : Length;
	//				}
	//				else if (unit_inch == Unit)//inch
	//				{
	//					Width = (Width > MAXPAPER_CUSTOM_WIDTH_INCH) ? MAXPAPER_CUSTOM_WIDTH_INCH : Width;
	//					Length = (Length > MAXPAPER_CUSTOM_LENGTH_INCH) ? MAXPAPER_CUSTOM_LENGTH_INCH : Length;
	//				}
	//				USERPAPERSIZEDATA UpsdTemp;
	//				SecureZeroMemory(&UpsdTemp, sizeof(UpsdTemp));
	//				UpsdTemp.dwWidth = (DWORD)Width;
	//				UpsdTemp.dwLength = (DWORD)Length;
	//				UpsdTemp.wUnit = (DWORD)Unit;
	//				(*pregUps).WriteUPSData(m_pszSvrPrnName, DMPAPER_CUSTOM_ZOOM, &UpsdTemp);
	//			}
	//			//	<E>To Fix Redmine Bug #3667,2021.10.12,SSDI:Chanchal Singla
	//			paper.wNum = CUSTOM_PAPER;
	//			paper.wRes = m_pOemPrivateDevMode->scpi.pub.dmPrintQuality;
	//			if (upsd.wUnit == unit_inch)
	//			{
	//				SecureZeroMemory(&stCnvParam, sizeof(stCnvParam));
	//				stCnvParam.paper = mip_paper_cus_usr;
	//				stCnvParam.mi = mip_mi_inch;
	//				stCnvParam.pos = 1;
	//
	//				stCnvParam.wl = mip_wl_width;
	//				paper.wPaperWidth01mm = (WORD)InchToMilli_CusPaper(upsd.dwWidth, MM_LOENGLISH, MM_LOMETRIC, &stCnvParam);
	//
	//				stCnvParam.wl = mip_wl_length;
	//				paper.wPaperLength01mm = (WORD)InchToMilli_CusPaper(upsd.dwLength, MM_LOENGLISH, MM_LOMETRIC, &stCnvParam);
	//
	//			}
	//			else
	//			{
	//				paper.wPaperWidth01mm = (WORD)upsd.dwWidth;
	//				paper.wPaperLength01mm = (WORD)upsd.dwLength;
	//			}
	//
	//
	//			if (SetCustomPaperInfo_FitToPaper(&paper, blEtoEPrint) == TRUE)
	//			{
	//				m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (short)paper.wPaperWidth01mm;
	//				m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (short)paper.wPaperLength01mm;
	//				m_pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = 6000; //(DWORD)paper.wPaperWidthPixel;
	//				m_pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = 6000;// (DWORD)paper.wPaperLengthPixel;
	//#if SCDM_VER >= 1905
	//
	//				m_pOemPrivateDevMode->scpi.ext.nDstLeftMarginPixel = (short)paper.wOffsetXPixel;
	//				m_pOemPrivateDevMode->scpi.ext.nDstTopMarginPixel = (short)paper.wOffsetYPixel;
	//
	//#endif	// #if SCDM_VER >= 1905
	//			}
	//			/*if (unit_inch == Unit)
	//			{
	//				m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = (DWORD)InchToPixel(Width, 600, MM_LOENGLISH);
	//				m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel, 600, MM_LOMETRIC);
	//
	//				m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = (DWORD)InchToPixel(Length, 600, MM_LOENGLISH);
	//				m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel, 600, MM_LOMETRIC);
	//
	//			}
	//			else if (unit_mm == Unit)
	//			{
	//				m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)Width;
	//				m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = MilliToPixel(m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli, 600, MM_LOMETRIC);
	//
	//				m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)Length;
	//				m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = MilliToPixel(m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli, 600, MM_LOMETRIC);
	//
	//			}*/
	//
	//			//CUSTOM_PAPERINFO		paper;
	//			//paper.wNum = CUSTOM_PAPER;
	//
	//			//m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel, 600, MM_LOMETRIC);
	//			//m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel, 600, MM_LOMETRIC);
	//
	//			//lWidthI = MilliToInch(m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli, MM_LOMETRIC, MM_LOENGLISH);
	//			//lLengthI = MilliToInch(m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli, MM_LOMETRIC, MM_LOENGLISH);
	//			//
	//			////<S><A>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
	//			//if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_EDGETOEDGE)
	//			//{
	//			//	m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli = 10;
	//			//	m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli = 10;
	//			//	m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli = 10;
	//			//	m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli = 10;
	//			//}
	//			//else
	//			//{
	//			//	m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli = 42;
	//			//	m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli = 42;
	//			//	m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli = 42;
	//			//	m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli = 42;
	//			//}
	//			////<E>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
	//
	//			///*if(unit_inch == m_pOemPrivateDevMode->scpi.ext.byCusUnits)
	//			//{*/
	//			//rcMgnI.left = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
	//			//rcMgnI.top = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
	//			//rcMgnI.right = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
	//			//rcMgnI.bottom = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
	//			///*}
	//			////<S><D> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
	//			//else
	//			//{
	//			//	rcMgnM.left		= m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli;
	//			//	rcMgnM.top		= m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli;
	//			//	rcMgnM.right	= m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli;
	//			//	rcMgnM.bottom	= m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli;
	//			//}*/
	//			////<E> Deleted the code for Support  of Custom Paper According to UD3.1, 27-07-2020,SSDI:Chanchal Singla
	//			//paper.wOffsetXPixel = (WORD)((rcMgnI.left * 600) / 100);
	//			//paper.wOffsetYPixel = (WORD)((rcMgnI.top * 600) / 100);
	//			//paper.wImageAreaXPixel = (WORD)(((lWidthI - rcMgnI.left - rcMgnI.right) * 600) / 100);
	//			//paper.wImageAreaYPixel = (WORD)(((lLengthI - rcMgnI.top - rcMgnI.bottom) * 600) / 100);
	//
	//			//m_pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel;
	//			//m_pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel;
	//
	//			//m_pOemPrivateDevMode->scpi.doc.nLeftMarginPixel = paper.wOffsetXPixel;
	//			//m_pOemPrivateDevMode->scpi.doc.nTopMarginPixel = paper.wOffsetYPixel;
	//
	//			if (pregUps != NULL)
	//			{
	//				delete pregUps;
	//				pregUps = NULL;
	//			}


			m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = CUSTOM_PAPER;
			m_pOemPrivateDevMode->scpi.ext.nZoomSrcPaperSize = DMPAPER_USER;
			m_pOemPrivateDevMode->scpi.pub.dmFields = DM_PAPERLENGTH | DM_PAPERWIDTH;

			RECTL			rcMgnI = {};
			RECTL           rcMgnM = {};

			long			lWidthI = 0;
			long			lLengthI = 0;

			CUSTOM_PAPERINFO		paper;
			paper.wNum = CUSTOM_PAPER;



			//DWORD Width = m_pPaperSize_upsd->dwWidth;
			//DWORD Length = m_pPaperSize_upsd->dwLength;
			//SHORT Unit = m_pPaperSize_upsd->wUnit;

			USERPAPERSIZEDATA upsd;
			SecureZeroMemory(&upsd, sizeof(upsd));
			CUPSReg FAR		*pregUps = NULL;
			pregUps = new CUPSReg(m_hStringResourceHandle, m_pszSvrPrnName);//bear
			if (pregUps == NULL)
			{
				return;
			}
			(*pregUps).ReadUPSData(m_pszSvrPrnName, DMPAPER_CUSTOM_ZOOM, &upsd);
			DWORD Width = upsd.dwWidth;
			DWORD Length = upsd.dwLength;
			SHORT Unit = upsd.wUnit;

			//Maxpapersize
			SHORT byMaxPaperSize = (m_pOemPrivateDevMode->scpi.dev.byMaxPaperSize == paper_121) ? bool_true : bool_false;
			if (byMaxPaperSize == eSecondOption)
			{
				if (unit_mm == Unit)
				{
					Width = (Width > MAXPAPER_CUSTOM_WIDTH_MM) ? MAXPAPER_CUSTOM_WIDTH_MM : Width;
					Length = (Length > MAXPAPER_CUSTOM_LENGTH_MM) ? MAXPAPER_CUSTOM_LENGTH_MM : Length;
				}
				else if (unit_inch == Unit)//inch
				{
					Width = (Width > MAXPAPER_CUSTOM_WIDTH_INCH) ? MAXPAPER_CUSTOM_WIDTH_INCH : Width;
					Length = (Length > MAXPAPER_CUSTOM_LENGTH_INCH) ? MAXPAPER_CUSTOM_LENGTH_INCH : Length;
				}
				USERPAPERSIZEDATA UpsdTemp;
				SecureZeroMemory(&UpsdTemp, sizeof(UpsdTemp));
				UpsdTemp.dwWidth = (DWORD)Width;
				UpsdTemp.dwLength = (DWORD)Length;
				UpsdTemp.wUnit = (DWORD)Unit;

			}

			m_pOemPrivateDevMode->scpi.ext.byCusUnits = (BYTE)Unit;
			if (unit_inch == Unit)
			{
				m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = (DWORD)InchToPixel(Width, 600, MM_LOENGLISH);
				m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel, 600, MM_LOMETRIC);

				m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = (DWORD)InchToPixel(Length, 600, MM_LOENGLISH);
				m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)PixelToMilli(m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel, 600, MM_LOMETRIC);

			}
			else if (unit_mm == Unit)
			{
				m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)Width;
				//	m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = MilliToPixel(m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli, 600, MM_LOMETRIC);

				m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)Length;
				//	m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = MilliToPixel(m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli, 600, MM_LOMETRIC);
				MIP_MINMAX_CONV_PARAM stCnvParam;
				long	lInchWA;
				long	lInchLA;
				stCnvParam.mi = mip_mi_inch;
				stCnvParam.pos = 10;
				stCnvParam.wl = mip_wl_width;
				lInchWA = MilliToInchNoChk_CusPaper(Width, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);
				stCnvParam.wl = mip_wl_length;
				lInchLA = MilliToInchNoChk_CusPaper(Length, MM_LOMETRIC, MM_LOENGLISH, stCnvParam.pos);

				stCnvParam.mi = mip_mi_inch;
				stCnvParam.pos = 10;
				stCnvParam.wl = mip_wl_width;
				m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel = (DWORD)InchToPixel_CusPaper(lInchWA, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);
				stCnvParam.wl = mip_wl_length;
				m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel = (DWORD)InchToPixel_CusPaper(lInchLA, m_pOemPrivateDevMode->scpi.pub.dmPrintQuality, MM_LOENGLISH, stCnvParam.pos);

			}

			{

				//   if(strcmp(m_pPrintFeatureOptionPair[m_uNumFeatureOptionPair-1].pszOption,"UnTitled"))
				{
					
					lWidthI = MilliToInch(m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli, MM_LOMETRIC, MM_LOENGLISH);
					lLengthI = MilliToInch(m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli, MM_LOMETRIC, MM_LOENGLISH);

				}

			}

			if (m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 & DM_EDGETOEDGE)
			{
				m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli = 10;
				m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli = 10;
				m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli = 10;
				m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli = 10;
			}
			else
			{
				m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli = 42;
				m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli = 42;
				m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli = 42;
				m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli = 42;
			}

			rcMgnI.left = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcLeftMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
			rcMgnI.top = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcTopMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
			rcMgnI.right = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcRightMarginMilli, MM_LOMETRIC, MM_LOENGLISH);
			rcMgnI.bottom = MilliToInch(m_pOemPrivateDevMode->scpi.doc.nSrcBottomMarginMilli, MM_LOMETRIC, MM_LOENGLISH);


			paper.wOffsetXPixel = (WORD)((rcMgnI.left * 600) / 100);
			paper.wOffsetYPixel = (WORD)((rcMgnI.top * 600) / 100);
			paper.wImageAreaXPixel = (WORD)(((lWidthI - rcMgnI.left - rcMgnI.right) * 600) / 100);
			paper.wImageAreaYPixel = (WORD)(((lLengthI - rcMgnI.top - rcMgnI.bottom) * 600) / 100);


			{
				//m_pOemPrivateDevMode->scpi.pub.dmPaperWidth = m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli;
				//m_pOemPrivateDevMode->scpi.pub.dmPaperLength = m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli;
				m_pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = m_pOemPrivateDevMode->scpi.ext.dwCusWidthPixel;
				m_pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = m_pOemPrivateDevMode->scpi.ext.dwCusLengthPixel;


				m_pOemPrivateDevMode->scpi.doc.dwImageableWidthPixel = paper.wImageAreaXPixel;
				m_pOemPrivateDevMode->scpi.doc.dwImageableHeightPixel = paper.wImageAreaYPixel;
				m_pOemPrivateDevMode->scpi.doc.nLeftMarginPixel = paper.wOffsetXPixel;
				m_pOemPrivateDevMode->scpi.doc.nTopMarginPixel = paper.wOffsetYPixel;

				m_pOemPrivateDevMode->scpi.ext.nDstLeftMarginPixel = paper.wOffsetXPixel;
				m_pOemPrivateDevMode->scpi.ext.nDstTopMarginPixel = paper.wOffsetYPixel;
				//<E>To Fix Fittopage custom paper,2023.04.08,SSDI:Chandrashekar V
			}
		}
		
		else
		{
			PaperInfoMap::const_iterator it;
			it = m_PaperInfo.find(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption);
			if (it != m_PaperInfo.end())
			{
				m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = (SHORT)it->second.PaperSize;
				//m_pOemPrivateDevMode->scpi.doc.nLeftMarginPixel = it->second.PageOrigin.first;
				//m_pOemPrivateDevMode->scpi.doc.nTopMarginPixel = it->second.PageOrigin.second;
				m_pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = 2 * m_pOemPrivateDevMode->scpi.doc.nLeftMarginPixel + it->second.PageImageaDimensions.first;
				m_pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = 2 * m_pOemPrivateDevMode->scpi.doc.nTopMarginPixel + it->second.PageImageaDimensions.second;
				m_pOemPrivateDevMode->scpi.ext.nDstWidthMilli = (SHORT)it->second.PaperWidth;
				m_pOemPrivateDevMode->scpi.ext.nDstHeightMilli = (SHORT)it->second.PaperLength;
			}
		}
	}
}
void CDevModeUpDate::UpDateBlackAndWhitePrint(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"OFF"))
	{
		m_pOemPrivateDevMode->scpi.pub.dmColor = DMCOLOR_COLOR;
        m_pOemPrivateDevMode->scpi.pub.dmICMMethod = DMICMMETHOD_DEVICE;
	}
	else
	{
		m_pOemPrivateDevMode->scpi.pub.dmColor = DMCOLOR_MONOCHROME;
        m_pOemPrivateDevMode->scpi.pub.dmICMMethod = DMICMMETHOD_NONE;
	}
}
void CDevModeUpDate::UpDateRotate180(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"OFF"))
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_ROTATE180DEG;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 &= ~DM_ROTATE180DEG_HW;
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_ROTATE180DEG;
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 |= DM_ROTATE180DEG_HW;
	}
}
void CDevModeUpDate::UpDateStaple(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"NONE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byStaple = 0;
	}
    else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"STAPLE_1"))
	{
		m_pOemPrivateDevMode->scpi.ext.byStaple = 1;
	}
    else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"STAPLE_2"))
	{
		m_pOemPrivateDevMode->scpi.ext.byStaple = 2;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "STAPLE_SADDLE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byStaple = 4;
	}
//<S><A>Updated devmode variable for stapleless feature,20.07.19 -SSDI:Goutham
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "STAPLE_STAPLELESS"))
	{
		m_pOemPrivateDevMode->scpi.ext.byStaple = 3;
	}
//<S><E>Updated devmode variable for stapleless feature,20.07.19 -SSDI:Goutham	
}



void CDevModeUpDate::UpDatePunch(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"OFF"))
	{
		m_pOemPrivateDevMode->scpi.ext.byPunchType = PUNCH_NONE;
	}
    else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"2HOLE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byPunchType = PUNCH_2HOLES;
	}
    else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"3HOLE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byPunchType = PUNCH_3HOLES;
	}
    else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"4HOLE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byPunchType = PUNCH_4HOLES;
	}
    else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"4HOLE_WIDE"))
	{
		m_pOemPrivateDevMode->scpi.ext.byPunchType = PUNCH_4HOLES_WIDE;
	}

	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "OFF"))
	{
		// --- DM_PUNCH
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_PUNCH;
	}
	else
	{
		// --- DM_PUNCH
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_PUNCH;
	}
	

}
void CDevModeUpDate::UpDateJobControlFields(const DWORD dwFlag )
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"OFF"))
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~dwFlag;
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC |= dwFlag;
	}
}
void CDevModeUpDate::UpDateRetentionSettings(void)
{
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "NONE"))
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_DOCFILING;
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC |= DM_DOCFILING;
	}

	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "NONE"))
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetention = retention_normal;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"HOLDONLY"))
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetention = retention_hold_before;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"HOLDAFTERPRINT"))
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetention = retention_hold_after;
	}
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"SAMPLEPRINT"))
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.byRetention = retention_sample;
	}
}
// <S><A>Added devmode values for authentication feature based on UD3.0, 20.03.24 - SSDI:Goutham
void CDevModeUpDate::UpDateAuthenticationSettings(void)
{

	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "None"))
	{
		//m_pOemPrivateDevMode->scpi.ext.JobCtrl.byAuthentication = authentication_none;
		
		// --- DM_ALWAYSUSE_ACNUM , wUseAcNum = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC  &= ~DM_ALWAYSUSE_ACNUM;
		
		// --- DM_USE_LOGINNAME ,wLoginName = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;

		// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;

		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_SINGLESIGNON;

	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "LoginName"))
	{
		//m_pOemPrivateDevMode->scpi.ext.JobCtrl.byAuthentication = authentication_lname;
		// --- DM_ALWAYSUSE_ACNUM, wUseAcNum = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;

		// --- DM_USE_LOGINNAME ,wLoginName = bool_true
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC |= DM_USE_LOGINNAME;

		// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_SINGLESIGNON;

	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "LoginPass"))
	{
		//m_pOemPrivateDevMode->scpi.ext.JobCtrl.byAuthentication = authentication_lname_pswd;
		// --- DM_ALWAYSUSE_ACNUM , wUseAcNum = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC  &= ~DM_ALWAYSUSE_ACNUM;

		// --- DM_USE_LOGINNAME ,wLoginName = bool_true
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC  |= DM_USE_LOGINNAME;

		// --- DM_USE_LOGINPASS, wLoginPass = bool_true
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC  |= DM_USE_LOGINPASS;
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_SINGLESIGNON;

	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "UserNumber"))
	{
		//m_pOemPrivateDevMode->scpi.ext.JobCtrl.byAuthentication = authentication_acnum;

		// --- DM_ALWAYSUSE_ACNUM , wUseAcNum = bool_true
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC  |= DM_ALWAYSUSE_ACNUM;

		// --- DM_USE_LOGINNAME, ,wLoginName = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;

		// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC  &= ~DM_USE_LOGINPASS;
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_SINGLESIGNON;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "LoginNameSSO"))
	{
		//m_pOemPrivateDevMode->scpi.ext.JobCtrl.byAuthentication = authentication_acnum;

		// --- DM_ALWAYSUSE_ACNUM , wUseAcNum = bool_true
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_ALWAYSUSE_ACNUM;

		// --- DM_USE_LOGINNAME, ,wLoginName = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINNAME;

		// --- DM_USE_LOGINPASS ,wLoginPass = bool_false
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC &= ~DM_USE_LOGINPASS;
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.wFieldJC |= DM_USE_SINGLESIGNON;

	}
}
// <E>Added devmode values for authentication feature based on UD3.0, 20.03.24 - SSDI:Goutham
// <S><C>Updated devmode for document filling settings based on UD3.0, 20.03.24 - SSDI:Goutham
void CDevModeUpDate::UpDateDocumentFilingSettings(void)
{
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "MAINFOLDER"))
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.byDocumentFiling = docfile_main;
	}
		
	else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"QUICKFILE"))
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.byDocumentFiling = docfile_quick;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "CUSTOMFOLDER"))
	{
		m_pOemPrivateDevMode->scpi.ext.JobCtrl.byDocumentFiling = docfile_custom;
		
		CShRegStored	*preg = NULL;

		CShIniFile			*m_pmcf = NULL;
		TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
		GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
		m_pmcf = new CShIniFile(ghInstance, m_pszSvrPrnName, szCommonDatFilePath, FALSE);
		CShJsonStored	*pjsonstr = NULL;
		if ((*m_pmcf).IsWriteToJson() == TRUE)
		{
			pjsonstr = new CShJsonStored(ghInstance, m_pszSvrPrnName, m_hStringResourceHandle);
			pjsonstr->Init();
		}

		BOOL			blHasPin = FALSE;
		WCHAR szTemp[512];
		::SecureZeroMemory(szTemp, sizeof(szTemp));
		WCHAR	szFilingPassword[JC_STR_FOLDERPASS + 1 + 8];
		::SecureZeroMemory(szFilingPassword, sizeof(szFilingPassword));
		WCHAR			szFilingPath[(JC_STR_FOLDERPATH + 1) * 2];
		::SecureZeroMemory(szFilingPath, sizeof(szFilingPath));

		//Coverity Fix:91471 - SSDI Seetharam - 20210518
		//Memory allocated, which is not allocated and used
		preg = new CShRegStored(m_hStringResourceHandle, m_pszSvrPrnName);
		if (preg == NULL)
		{
			return ;
		}
		SHORT wDocFileFolder;
		if ((*m_pmcf).IsWriteToJson() == TRUE)
		{
			wDocFileFolder = (short)(*pjsonstr).ReadLastSelectedFolder(szTemp, sizeof(szTemp), szFilingPassword, sizeof(szFilingPassword));

		}
		else
		{
			wDocFileFolder = (short)(*preg).ReadLastSelectedFolder(m_pszSvrPrnName, szTemp, sizeof(szTemp), szFilingPassword, sizeof(szFilingPassword));

		}
		if ((*preg).ReadStrdData(m_pszSvrPrnName, (SHORT)wDocFileFolder,
			szFilingPath, sizeof(szFilingPath), &blHasPin) == FALSE)
		{
			//Fixed Coverity issue - 91508#2 - SSDI:Seetharam-20201005
			delete preg;
			preg = NULL;
			return;
		}
		memcpy_s(m_pOemPrivateDevMode->scpi.ext.JobCtrl.szFilingPath,
			(_countof(szFilingPath) - 1) * sizeof(WCHAR),
			szFilingPath,
			(_countof(szFilingPath) - 1) * sizeof(WCHAR)
		);
			
		if (blHasPin == TRUE)
		{
			//Fixed Coverity issue - 90628 - SSDI:Seetharam-20201005
			//::lstrcpyn((LPWSTR)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byFolderPassword, szFilingPassword, JC_STR_FOLDERPASS + 1);
			::lstrcpyn((LPWSTR)m_pOemPrivateDevMode->scpi.ext.JobCtrl.byFolderPassword, szFilingPassword, wcslen(szFilingPassword) * sizeof(WCHAR));
		}

		//Fixed Coverity issue - 91508#1 - SSDI:Seetharam-20201005
		delete preg;
		preg = NULL;

		if (pjsonstr != NULL) {
			delete pjsonstr;
			pjsonstr = NULL;
		}
		if (m_pmcf != NULL)
		{
			delete m_pmcf;
			m_pmcf = NULL;
		}
	}
}
// <E>Updated devmode for document filling settings based on UD3.0, 20.03.24 - SSDI:Goutham
void CDevModeUpDate::UpdateColorFields(void)
{
	if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"Auto"))
	{
        m_pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_auto;
		m_pOemPrivateDevMode->scpi.pub.dmColor = DMCOLOR_COLOR;
		m_pOemPrivateDevMode->scpi.pub.dmICMMethod = DMICMMETHOD_DEVICE;
	}
    else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"Color"))
	{
        m_pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_color;
		m_pOemPrivateDevMode->scpi.pub.dmColor = DMCOLOR_COLOR;
		m_pOemPrivateDevMode->scpi.pub.dmICMMethod = DMICMMETHOD_DEVICE;

	}
    else if(!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption,"Mono"))
	{
        m_pOemPrivateDevMode->scpi.ext.ColData.byColMode = colmode_mono;
		m_pOemPrivateDevMode->scpi.pub.dmColor = DMCOLOR_MONOCHROME;
		m_pOemPrivateDevMode->scpi.pub.dmICMMethod = DMICMMETHOD_NONE;
	}
}
//<S><C> To Modified tho condition to the Support of Booklet Output Size,2020-09-02,SSDI Chanchal Singla
//<S><A>Added function to update devmode variable for booklet and offset feature,2020.07.19 SSDI:Goutham
void CDevModeUpDate::UpDateBooklet(void)
{
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "None"))
	{
		m_pOemPrivateDevMode->scpi.ext.byPamph = booklet_none;
		m_pOemPrivateDevMode->scpi.ext.nPamphPaperSize = GetPamphSize(m_pOemPrivateDevMode->scpi.pub.dmPaperSize, m_pOemPrivateDevMode->scpi.ext.byPamph, TRUE);
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_FITTOPAGE;
		m_pOemPrivateDevMode->scpi.ext.byPamph = PAMPH_2UP;
		m_bPamphletStyleSelected = TRUE;
	}
}
//<E> To Modified tho condition to the Support of Booklet Output Size,2020-09-02,SSDI Chanchal Singla
//<S><A> To Add the Support of Booklet Output Size,2020-09-02,SSDI Chanchal Singla
void CDevModeUpDate::UpDateBookletOutputSize(void)
{
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "A3"))
	{
		m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = paper_a3;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "A4"))
	{
		m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = paper_a4;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "B4"))
	{
		m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = paper_b4;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "TABLOID"))
	{
		m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = paper_ledger;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "LETTER"))
	{
		m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = paper_letter;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "LEGAL"))
	{
		m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize = paper_legal;
	}

	m_pOemPrivateDevMode->scpi.ext.nPamphPaperSize = GetPamphSize(m_pOemPrivateDevMode->scpi.ext.nFTPPaperSize, m_pOemPrivateDevMode->scpi.ext.byPamph, TRUE);
	PaperPixelInformationMap::const_iterator paperpixelinfoiterator;
	paperpixelinfoiterator = m_PaperPixelInfoMap.find(m_pOemPrivateDevMode->scpi.ext.nPamphPaperSize);
	if (paperpixelinfoiterator != m_PaperPixelInfoMap.end())
	{
		m_pOemPrivateDevMode->scpi.ext.dwDstWidthPixel = paperpixelinfoiterator->second.dwWidthPixel;
		m_pOemPrivateDevMode->scpi.ext.dwDstHeightPixel = paperpixelinfoiterator->second.dwHeightPixel;
	}
//<S><A> To fix redmine #2792 issue 2020.12.09 SSDI:Sneha TG
//To fix redmine #2792 based on Output size we are updating booklet PJL.
//So after updating Output size devmode we are updating booklet devmode value.
	if (!strcmp(m_pPrintFeatureOptionPair[FEATURE_BOOKLET].pszOption, "Standard"))
	{
		m_pOemPrivateDevMode->scpi.ext.byPamph = GetSCDMPamph();
	}
//<E> To fix redmine #2792 issue 2020.12.09 SSDI:Sneha TG	
}
//<E> To Add the Support of Booklet Output Size,2020-09-02,SSDI Chanchal Singla
void CDevModeUpDate::UpDateOffset(void)
{
	m_pOemPrivateDevMode->scpi.ext2.wFieldE2DM &= ~DM_NONE_OFFSET;
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "Offset_each_set"))
	{
		m_pOemPrivateDevMode->scpi.ext.byOffset = offset_offset_each_set;
		// Offset each set
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 |= DM_OFFSET;
	}
	else
	{
		// None
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM1 &= ~DM_OFFSET;
		m_pOemPrivateDevMode->scpi.ext.byOffset = offset_none;
	}
}
//<S><E>Added function to update devmode variable for booklet and offset feature,2020.07.19 SSDI:Goutham
//<S><D> To Deleted because nWatermark variable is updated with Wrong value,2020-09-08,SSDI:Chanchal Singla
// nwatermark is updated with Watermark index drop down value inside watermark dialog 
//<S><A>Added function to update devmode variable for Stamp and margin shift feature,2020.07.27 SSDI:Goutham
//void CDevModeUpDate::UpDateWatermark(void)
//{
//	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "None"))
//	{
		//m_pOemPrivateDevMode->scpi.ext.nWatermark = stamp_none;

//	}
//	else
//	{
		//m_pOemPrivateDevMode->scpi.ext.nWatermark = stamp_watermark;
//	}
//}
//<E> To Deleted because nWatermark variable is updated with Wrong value,2020-09-08,SSDI:Chanchal Singla
//<S><C>Modified function to update devmode variable for margin shift feature,2020.07.29 SSDI:Goutham
void CDevModeUpDate::UpDateMarginShift(void)
{

	MARGINSHIFTDATA	msd;
	short			wShift = 0;
	long			lDPI = 0;
	long			lPixel = 0;
	SHORT			wDPI = 0;
	CShRegMS FAR		*pregms = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pszSvrPrnName, szCommonDatFilePath, FALSE);
	CShJsonMS	*pjsonms = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonms = new CShJsonMS(ghInstance, m_pszSvrPrnName);
		pjsonms->Init();
	}

	SecureZeroMemory(&msd, sizeof(msd));
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "None"))
	{
		m_pOemPrivateDevMode->scpi.ext.byMgnShift = 0;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "Inch_Point_4"))
	{
		m_pOemPrivateDevMode->scpi.ext.byMgnShift = 1;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "Inch_Point_8"))
	{
		m_pOemPrivateDevMode->scpi.ext.byMgnShift = 2;
	}
	else if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "Inch_One_Point_2"))
	{
		m_pOemPrivateDevMode->scpi.ext.byMgnShift = 3;
	}

	pregms = new CShRegMS(m_hStringResourceHandle, m_pszSvrPrnName);
	if (pregms == NULL)
	{
		goto EXIT;
	}
	//<S><C>To Fix #2674.2020-11-10,SSDI:Chanchal Parkash
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		DWORD dwCount = (*pjsonms).ReadMSCount();
		if (dwCount <= 0)
		{
			(*pjsonms).reset();
			dwCount = (*pjsonms).ReadMSCount();
		}
		(*pjsonms).ReadMSData( m_pOemPrivateDevMode->scpi.ext.byMgnShift, &msd);

	}
	else
	{

		DWORD dwCount = (*pregms).ReadMSCount(m_pszSvrPrnName);
		if (dwCount <= 0)
		{
			(*pregms).reset(m_pszSvrPrnName);
			dwCount = (*pregms).ReadMSCount(m_pszSvrPrnName);
		}
		(*pregms).ReadMSData(m_pszSvrPrnName, m_pOemPrivateDevMode->scpi.ext.byMgnShift, &msd);
	}
	wDPI = m_pOemPrivateDevMode->scpi.pub.dmPrintQuality;
	lDPI = msd.dwDPI;
	lPixel = msd.dwPixel;

	if (lDPI != 0 && lPixel != 0)
		wShift = (short)((lPixel * wDPI) / lDPI);
	m_pOemPrivateDevMode->scpi.ext.nMgnShiftPixel = (SHORT)wShift;
	//<E>To Fix #2674.2020-11-10,SSDI:Chanchal Parkash
	//<S><A> To Update the devmode MarginShift and To Fix Printing Issue,04-08-2020,SSDI:Chanchal Parkash
	/*if (m_pOemPrivateDevMode->scpi.ext.byMgnShift == 0)
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 |= DM_MARGINSHIFT_HW;
		m_pOemPrivateDevMode->scpi.ext.nMgnShiftPixel = (SHORT)msd.dwPixel;
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 &= ~DM_MARGINSHIFT_HW;
		m_pOemPrivateDevMode->scpi.ext.nMgnShiftPixel = (SHORT)msd.dwPixel;
	}*/
	//<E> To Update the devmode MarginShift and To Fix Printing Issue,04-08-2020,SSDI:Chanchal Parkash
EXIT:
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
}
//<S><E>Modified function to update devmode variable for margin shift feature,2020.07.29 SSDI:Goutham
//<S><E>Added function to update devmode variable for Stamp and margin shift feature,2020.07.27 SSDI:Goutham

//<S><A>Added function to update devmode variable for Input Resolution and Printable area feature,2020.08.11 SSDI:Goutham
void CDevModeUpDate::UpDateInputResolution(void)
{
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "300dpi"))
	{

		m_pOemPrivateDevMode->scpi.ext.byResolution = 0;
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.byResolution = 1;
	}
}

void CDevModeUpDate::UpDatePrintableArea(void)
{
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "Standard"))
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 &= ~DM_EDGETOEDGE;
		
	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.dwFieldEDM2 |= DM_EDGETOEDGE;
		
	}
}
//<S><E>Added function to update devmode variable for Input Resolution and Printable area feature,2020.08.11 SSDI:Goutham
//<S><A> To Update the PrintPosition Custom feature DevmodeValue,2020-08-20,SSDI:Chanchal Singla
void CDevModeUpDate::UpDatePrintPosition(void)
{
	PRINTPOSITION	ppd;
	PRINTPOSITION	ppdDef;
	CShRegPP FAR		*pregPP = NULL;

	CShIniFile			*m_pmcf = NULL;
	TCHAR szCommonDatFilePath[_MAX_PATH] = { 0 };
	GetProjectFileName(szCommonDatFilePath, L"Common.DAT");
	m_pmcf = new CShIniFile(ghInstance, m_pszSvrPrnName, szCommonDatFilePath, FALSE);
	CShJsonPP	*pjsonpp = NULL;
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{
		pjsonpp = new CShJsonPP(ghInstance, m_pszSvrPrnName);
		pjsonpp->Init();
	}

	//-- Print Position
	SecureZeroMemory(&ppd, sizeof(PRINTPOSITION));
	SecureZeroMemory(&ppdDef, sizeof(PRINTPOSITION));

	pregPP = new CShRegPP(m_hStringResourceHandle, m_pszSvrPrnName);
	if (pregPP == NULL)
	{
		goto EXIT;
	}
	if ((*m_pmcf).IsWriteToJson() == TRUE)
	{ 
		BOOL Flag = (*pjsonpp).ReadPPData(&ppd);
	}
	else
	{
		BOOL Flag = (*pregPP).ReadPPData(m_pszSvrPrnName, &ppd);
	}
	// --- DM_PRINTPOSITION_CHG
	BOOL wPPosChg = m_pOemPrivateDevMode->scpi.ext.wFieldEDM & DM_PRINTPOSITION_CHG ? bool_true : bool_false;
//<S><C>To Add the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla
// PrintPosition Structure is moved in ext2 Mode
	if (wPPosChg == bool_false)
	{
		m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wOddX = (short)0;
		m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wOddY = (short)0;
		m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wEvenX = (short)0;
		m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wEvenY = (short)0;
		m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.dwUnit = GetUnitDef();
		m_pOemPrivateDevMode->scpi.ext.wFieldEDM &= ~DM_PRINTPOSITION;
	}
	else
	{
		if (ppd.dwUnit == unit_mm)
		{
			m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wOddX = (short)MilliToPixel_PPos(ppd.wOddX);
			m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wOddY = (short)MilliToPixel_PPos(ppd.wOddY);
			m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wEvenX = (short)MilliToPixel_PPos(ppd.wEvenX);
			m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wEvenY = (short)MilliToPixel_PPos(ppd.wEvenY);
		}
		else if (ppd.dwUnit == unit_inch)
		{
			m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wOddX = (short)InchToPixel_PPos(ppd.wOddX);
			m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wOddY = (short)InchToPixel_PPos(ppd.wOddY);
			m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wEvenX = (short)InchToPixel_PPos(ppd.wEvenX);
			m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.wEvenY = (short)InchToPixel_PPos(ppd.wEvenY);
		}
			
		m_pOemPrivateDevMode->scpi.ext2.PrintPosition.data.dwUnit = unit_pixel;
		m_pOemPrivateDevMode->scpi.ext.wFieldEDM |= DM_PRINTPOSITION;
	}

//<E>To Add the EXT2 devmode in SCDM Stucture,20201104,SSDI:Chanchal Singla

EXIT:
	if (pregPP != NULL) {
		delete pregPP;
		pregPP = NULL;
	}
	if (pjsonpp != NULL) {
		delete pjsonpp;
		pjsonpp = NULL;
	}
	if (m_pmcf != NULL)
	{
		delete m_pmcf;
		m_pmcf = NULL;
	}
}		
//<S><A> To Update the PrintPosition Custom feature DevmodeValue,2020-08-20,SSDI:Chanchal Singla
//<S><A> To fix redmine #2792 issue 2020.12.09 SSDI:Sneha TG
BYTE CDevModeUpDate::GetSCDMPamph(void)
{	
	BYTE	cRet = PAMPH_NOT;
	OEMDEV temp_pOemPrivateDevMode = {0};
	memcpy_s(&temp_pOemPrivateDevMode, sizeof(OEMDEV), m_pOemPrivateDevMode, sizeof(OEMDEV));

	if (IsConvTiledPamphlet(temp_pOemPrivateDevMode.scpi.pub.dmPaperSize, temp_pOemPrivateDevMode.scpi.ext.nFTPPaperSize))
	{				
		cRet = PAMPH_TILED;
	}
	else
	{
		cRet = PAMPH_2UP;
	}

	return cRet;
}

BOOL CDevModeUpDate::IsConvTiledPamphlet(short OriginalSize, short OutputSize)
{
	BOOL					blRet = FALSE;

	switch (OriginalSize)
	{
		case paper_a4:
			if (OutputSize == paper_a3)
				goto Ret;
				break;
		case paper_a5:
			if (OutputSize == paper_a4)
				goto Ret;
			break;
		case paper_b5:
			if (OutputSize == paper_b4)
				goto Ret;
			break;
		case paper_letter:
			if (OutputSize == paper_ledger)
				goto Ret;
			break;
		Ret:
			blRet = TRUE;
		default:
			;
	}
	return blRet;
}
//<E> To fix redmine #2792 issue 2020.12.09 SSDI:Sneha TG

void CDevModeUpDate::UpDate100PercentNup(void)
{
	if (!strcmp(m_pPrintFeatureOptionPair[m_FeatureIndex].pszOption, "OFF"))
	{
		m_pOemPrivateDevMode->scpi.ext.wFieldEDM &= ~DM_SAMENUP;

	}
	else
	{
		m_pOemPrivateDevMode->scpi.ext.wFieldEDM |= DM_SAMENUP;

	}
}
//=============================================================================
// function
//      SetCustomPaperInfo_FitToPaper
//
// parameters
// 
//		CUSTOM_PAPERINFO
//		BOOL  blEtoEPrint              
//
// return value
//      ê: TRUE
//     : FALSE
//
// outline
//      Set CustomPaperInfo FitToPaper
//=============================================================================
BOOL CDevModeUpDate::SetCustomPaperInfo_FitToPaper(CUSTOM_PAPERINFO	*paper,BOOL  blEtoEPrint)
{
	short			wPaper;
	short			wDPI;

	WORD			wMgnUnit;

	long			lWidthM = 0L;
	long			lWidthI = 0L;
	long			lLengthM = 0L;
	long			lLengthI = 0L;
	RECTL			rcMgn;
	RECTL			rcMgnM;
	RECTL			rcMgnI;


	PAPERSECINFO	psi;
	PAPERPIXINFO	ppi;

	MIP_MINMAX_CONV_PARAM	stCnvParam;
	SecureZeroMemory(&stCnvParam, sizeof(stCnvParam));

	wPaper = (*paper).wNum;
	wDPI = (*paper).wRes;

	// ================================================
	// _/_/_/  Width/Length/Unit
	// ================================================
	SecureZeroMemory(&psi, sizeof(psi));
	psi.lWidth = (*paper).wPaperWidth01mm;
	psi.lLength = (*paper).wPaperLength01mm;
	psi.wUnit = MM_LOMETRIC;

	stCnvParam.paper = mip_paper_cus_usr;
	stCnvParam.mi = mip_mi_inch;
	stCnvParam.pos = 10;

	stCnvParam.wl = mip_wl_width;
	lWidthI = MilliToInch_CusPaper(lWidthM, MM_LOMETRIC, MM_LOENGLISH, &stCnvParam);

	stCnvParam.wl = mip_wl_length;
	lLengthI = MilliToInch_CusPaper(lLengthM, MM_LOMETRIC, MM_LOENGLISH, &stCnvParam);
	//<S><A>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla
	if (blEtoEPrint == TRUE)
	{
		rcMgnM.left = 10;
		rcMgnM.top = 10;
		rcMgnM.right = 10;
		rcMgnM.bottom = 10;
	}
	else
	{
		rcMgnM.left = 42;
		rcMgnM.top = 42;
		rcMgnM.right = 42;
		rcMgnM.bottom = 42;
	}
	//<E>To The Support of Printable Area Feature Printing Part Functionality,03-12-2021,SSDI:Chanchal Singla


	rcMgnI.left = MilliToInch(rcMgnM.left, MM_LOMETRIC, MM_LOENGLISH);
	rcMgnI.top = MilliToInch(rcMgnM.top, MM_LOMETRIC, MM_LOENGLISH);
	rcMgnI.right = MilliToInch(rcMgnM.right, MM_LOMETRIC, MM_LOENGLISH);
	rcMgnI.bottom = MilliToInch(rcMgnM.bottom, MM_LOMETRIC, MM_LOENGLISH);

	rcMgnI.left *= 10;
	rcMgnI.top *= 10;
	rcMgnI.right *= 10;
	rcMgnI.bottom *= 10;

	(*paper).wImageAreaX01mm = (WORD)(lWidthM - rcMgnM.left - rcMgnM.right);
	(*paper).wImageAreaY01mm = (WORD)(lLengthM - rcMgnM.top - rcMgnM.bottom);
	(*paper).wOffsetX01mm = (WORD)rcMgnM.left;
	(*paper).wOffsetY01mm = (WORD)rcMgnM.top;


	(*paper).wPaperWidthPixel = (WORD)roundL((lWidthI * wDPI), 1000);
	(*paper).wPaperLengthPixel = (WORD)roundL((lLengthI * wDPI), 1000);
	(*paper).wImageAreaXPixel = (WORD)roundL(((lWidthI - rcMgnI.left - rcMgnI.right) * wDPI), 1000);
	(*paper).wImageAreaYPixel = (WORD)roundL(((lLengthI - rcMgnI.top - rcMgnI.bottom) * wDPI), 1000);
	(*paper).wOffsetXPixel = (WORD)roundL((rcMgnI.left * wDPI), 1000);
	(*paper).wOffsetYPixel = (WORD)roundL((rcMgnI.top * wDPI), 1000);


	if (blEtoEPrint == TRUE)
	{
		(*paper).wPaperWidthPixel += EDGE2EDGEPRINTING_CUSTOM_300 * wDPI / 300;
		(*paper).wPaperLengthPixel -= EDGE2EDGEPRINTING_CUSTOM_300 * wDPI / 300;
	}

	SecureZeroMemory(&ppi, sizeof(PAPERPIXINFO));
	if (GetPaperPixelInfo(&ppi, wPaper, wDPI, blEtoEPrint) != FALSE)
	{
		(*paper).wImageAreaXPixel = (WORD)((*paper).wPaperWidthPixel - ppi.wOffsetXPixel * 2);
		(*paper).wImageAreaYPixel = (WORD)((*paper).wPaperLengthPixel - ppi.wOffsetYPixel * 2);
		(*paper).wOffsetXPixel = (WORD)ppi.wOffsetXPixel;
		(*paper).wOffsetYPixel = (WORD)ppi.wOffsetYPixel;
	}
	return TRUE;
}